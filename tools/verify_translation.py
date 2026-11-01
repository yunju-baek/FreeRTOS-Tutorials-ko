#!/usr/bin/env python3
"""Validate pinned sources, translated Markdown, code blocks and local links.

Read-only unless --report is supplied. Requires Python 3 and Git; no packages.
This checks structure and provenance, not semantic accuracy or code execution.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
from urllib.parse import unquote, urlsplit

ROOT = Path(__file__).resolve().parents[1]
BASELINE = 'c89d7164ec6161c57a2ece61df412d6e3e013317'
UPSTREAM = 'https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials'
BOOK = 'https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko'
BOOK_COMMIT = '27d03c61cd2a6cf5a6fa6333082218ef706f5b89'
EXPECTED = ['README.md'] + [f'docs/tutorial_{n}/README.md' for n in range(1, 22)]
BOOK_CHAPTER = {1:2, 2:4, 3:3, 4:4, 5:4, 6:4, 7:4, 8:4, 9:5, 10:7,
                11:6, 12:7, 13:8, 14:8, 15:9, 16:9, 17:10, 18:10, 21:8}


def git(*args: str) -> str:
    return subprocess.check_output(['git', *args], cwd=ROOT, text=True).strip()


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def fenced_blocks(text: str) -> list[str]:
    blocks, buf, active = [], [], None
    for line in text.splitlines(keepends=True):
        match = re.match(r'^([ \t]*)(`{3,}|~{3,})(.*?)(?:\r?\n)?$', line)
        if active:
            buf.append(line)
            if (match and match[2][0] == active[0] and len(match[2]) >= active[1]
                    and not match[3].strip()):
                blocks.append(''.join(buf))
                active = None
        elif match:
            active, buf = (match[2][0], len(match[2])), [line]
    if active:
        raise ValueError('Unclosed code fence')
    return blocks


def check() -> dict:
    errors, documents = [], []
    counts = dict(upstream_files=0, documents=0, code_blocks=0,
                  pages_without_front_matter=0, tutorial_toc_links=0,
                  pinned_source_links=0, pinned_book_links=0, local_links=0)
    manifest = json.loads((ROOT/'ko/translation-manifest.json').read_text())
    if manifest['upstream_commit'] != BASELINE or manifest['book_commit'] != BOOK_COMMIT:
        errors.append('Unexpected upstream or book baseline')
    entries = manifest['files']
    if sorted(e['source_path'] for e in entries) != sorted(EXPECTED):
        errors.append('Manifest must contain the introduction and all 21 tutorials once')
    # README is the only exception to upstream paths: its exact original is preserved.
    for line in git('ls-tree', '-r', BASELINE).splitlines():
        info, path = line.split('\t'); mode, kind, blob = info.split()
        if kind == 'commit':
            current = git('ls-tree', 'HEAD', path).split()[2]
            if current != blob:
                errors.append(f'Changed submodule revision: {path}')
            continue
        target = ROOT/('README.upstream.md' if path == 'README.md' else path)
        if not target.is_file() or git('hash-object', str(target)) != blob:
            errors.append(f'Changed/missing upstream file: {path}')
        counts['upstream_files'] += 1
    for entry in entries:
        rel = entry['source_path']; target_rel = entry['translation_path']
        source = ROOT/('README.upstream.md' if rel == 'README.md' else rel)
        target = ROOT/target_rel
        if target_rel != 'ko/'+rel or not target.is_file():
            errors.append(f'Missing or incorrect translation path: {target_rel}')
            continue
        en, ko = source.read_text(), target.read_text()
        if sha256(source) != entry['source_sha256'] or git('rev-parse', BASELINE+':'+rel) != entry['source_blob_sha']:
            errors.append(f'Source manifest hash mismatch: {rel}')
        if sha256(target) != entry['translation_sha256']:
            errors.append(f'Translation manifest hash mismatch: {target_rel}')
        original_blocks, translated_blocks = fenced_blocks(en), fenced_blocks(ko)
        same_code = original_blocks == translated_blocks
        if not same_code:
            errors.append(f'Changed code block: {target_rel}')
        front_matter = bool(re.match(r'\A\ufeff?---[ \t]*\r?\n', ko))
        if front_matter:
            errors.append(f'Front matter: {target_rel}')
        else:
            counts['pages_without_front_matter'] += 1
        body = ko.split('## 관련 자료')[0]
        for block in translated_blocks:
            body = body.replace(block, '', 1)
        if re.search(r'\b([A-Za-z]+(?: [A-Za-z]+)*)\s*\(\s*\1\s*\)', body, re.I):
            errors.append(f'Repeated English gloss: {target_rel}')
        if re.search(r'(?:그림|표|코드|예제)\s+[\d.]+\s*\((?:Figure|Table|Listing|Example)', body):
            errors.append(f'Repeated English caption: {target_rel}')
        if '__TK' in ko:
            errors.append(f'Unresolved placeholder: {target_rel}')
        if rel == 'README.md':
            toc = re.findall(r'\]\((docs/tutorial_\d+/README\.md)\)', ko)
            counts['tutorial_toc_links'] = len(toc)
            if sorted(toc) != sorted(EXPECTED[1:]):
                errors.append('Korean tutorial index does not match the 21 documents')
            if BOOK not in ko:
                errors.append('Korean index has no companion-book link')
        else:
            n = int(Path(rel).parent.name.split('_')[1])
            refs = ko.split('## 관련 자료', 1)
            if len(refs) != 2:
                errors.append(f'Missing related resources: {target_rel}')
                continue
            for source_path in [rel, f'source/tutorials/tutorial_{n}/source/tutorial_{n}.c',
                                f'source/tutorials/tutorial_{n}/include/FreeRTOSConfig.h']:
                url = UPSTREAM+'/blob/'+BASELINE+'/'+source_path
                if ']('+url+')' not in refs[1]:
                    errors.append(f'Incorrect source link: {target_rel}: {source_path}')
                counts['pinned_source_links'] += 1
            if n in BOOK_CHAPTER:
                url = BOOK+'/blob/'+BOOK_COMMIT+f'/ko/ch{BOOK_CHAPTER[n]:02}.md'
                if ']('+url+')' not in refs[1]:
                    errors.append(f'Incorrect book chapter link: {target_rel}')
                counts['pinned_book_links'] += 1
            elif '독립 장 없음' not in refs[1] or BOOK in refs[1]:
                errors.append(f'Incorrect book mapping for T{n:02}')
        counts['documents'] += 1
        counts['code_blocks'] += len(original_blocks)
        documents.append(dict(source_path=rel, translation_path=target_rel,
                              code_blocks=len(original_blocks), code_blocks_preserved=same_code,
                              front_matter_present=front_matter, translation_sha256=sha256(target)))
    authored = [ROOT/'README.md', ROOT/'AGENTS.md', ROOT/'PROJECT_STATE.md', *sorted((ROOT/'ko').rglob('*.md'))]
    for path in authored:
        text = path.read_text()
        if '/Users/' in text or 'localhost:' in text or '127.0.0.1:' in text:
            errors.append(f'Local machine reference in public document: {path.relative_to(ROOT)}')
        for dest in re.findall(r'\]\(([^\s)]+)\)', text):
            url = urlsplit(dest)
            if url.scheme or not url.path:
                continue
            target = (path.parent/unquote(url.path)).resolve()
            expected_report = ROOT/'publish/translation-validation.json'
            if not target.exists() and target != expected_report:
                errors.append(f'Broken local link: {path.relative_to(ROOT)} -> {dest}')
            counts['local_links'] += 1
    if BOOK not in (ROOT/'README.md').read_text():
        errors.append('Public README has no companion-book link')
    return dict(result='pass' if not errors else 'fail', baseline=BASELINE,
                counts=counts, documents=documents, errors=errors,
                scope='Pinned source/hash/code/structure/local-link checks. Book chapter URLs are checked against the reviewed mapping; external HTTP availability, semantic certification, build/run/hardware are excluded.')


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--report', type=Path, help='Optional JSON report path')
    args = parser.parse_args()
    result = check()
    if args.report:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(json.dumps(result, ensure_ascii=False, indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k != 'documents'}, ensure_ascii=False, indent=2))
    return 0 if not result['errors'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
