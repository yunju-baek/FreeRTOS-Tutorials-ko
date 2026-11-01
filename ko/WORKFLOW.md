# 한국어판 수정 절차

1. `AGENTS.md`, `PROJECT_STATE.md`, [번역 지침](TRANSLATION_GUIDE.md), [검수 기록](REVIEW_NOTES.md)을 읽는다.
2. Git 상태를 확인하고 기존 변경을 보존한다. `origin/main`에서 작업 branch를 만든다.
3. manifest의 원문 기준과 대상 파일을 확인하고 `ko/`의 요청한 범위만 교정한다. 영문 원문·코드·TODO·라이선스를 보존한다.
4. 아래 검사와 내용 대조를 수행하고 링크·Markdown 표시를 확인한다.

```sh
python3 tools/verify_translation.py --report publish/translation-validation.json
git diff --check
```

5. 원문 의심 사항과 실제 검수 범위를 기록하고 수정 파일의 `translation_sha256`을 갱신한다. 검증 도구는 번역 hash를 자동 변경하지 않는다.
6. 관련 파일만 명시적으로 stage하여 commit하고 `origin`의 작업 branch로 push한 뒤 이 한국어판 저장소에 PR을 만든다. 사용자 요청 또는 저장소의 명시적 병합 규칙에 따라 `main`에 반영한다.

공식 원문 저장소에 한국어 변경을 직접 push하지 않는다. 영문 기준을 갱신할 때는 별도 작업으로 영향 범위를 검토한다. `upstream-baseline`은 현재 번역의 기준이다.

[커널북 한국어판](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko)의 코드·원문·manifest는 이 저장소와 별개로 관리한다. 두 저장소의 소개와 목차에서 서로 이동할 수 있다.
