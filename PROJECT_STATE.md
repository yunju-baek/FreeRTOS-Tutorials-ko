# FreeRTOS Tutorials 한국어판 현재 상태

갱신일: 2026-11-01

## 정본과 출처

- 공개 정본: [https://github.com/yunju-baek/FreeRTOS-Tutorials-ko](https://github.com/yunju-baek/FreeRTOS-Tutorials-ko), `origin/main`
- 한국어 문서: `ko/README.md`, `ko/docs/tutorial_1/README.md`–`tutorial_21/README.md`
- 공식 원문: [https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials), remote `upstream`
- 원문 기준: `c89d7164ec6161c57a2ece61df412d6e3e013317`, `upstream-baseline` branch와 manifest에 보존
- kernel 기준: `18a168bcd2603354bcc5f8a408b2562d86709dd1`; 하위 port를 포함한 submodule 3개 확보
- 함께 읽는 책: [FreeRTOS-Kernel-Book-ko](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko)

## 완료 범위

소개 1개·튜토리얼 21개를 번역하고 Codex가 18개 문서의 표현·표기를 교정했다. 코드 블록 50개와 영문 원본 파일 70개를 보존했다. 목차 링크 21개·고정 원문 링크 63개·커널북 장 연결 19개, 전체 22개 문서의 front matter 부재와 HTML 표시를 확인했다.

공개본은 공식 원본 clone의 이력·source·LICENSE를 보존하고 검수본을 `ko/`로 옮겼다. 루트 README와 한국어 목차에 커널북 탐색 링크를 추가했다. 자세한 결과는 [검수 기록](ko/REVIEW_NOTES.md), [manifest](ko/translation-manifest.json), [구조 검사](publish/translation-validation.json)에 있다.

## 다음 시작점

현재 상태는 `source-checked`다. 사람의 최종 교열·예제 빌드·실행·실보드·PDF 검증은 수행하지 않았다. 예제 TODO도 그대로다. submodule 확보와 실제 실행 성공은 구분한다.

후속 변경은 [번역 지침](ko/TRANSLATION_GUIDE.md)과 [작업 절차](ko/WORKFLOW.md)를 따른다. 원문 T01의 구성 항목 수 불일치, T07·T15·T20의 표현 문제와 교정 근거는 검수 기록에 남아 있다. 공개와 `release-ready` 판정은 구분한다.
