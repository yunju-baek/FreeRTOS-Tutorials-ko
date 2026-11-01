# 튜토리얼 한국어판 번역·교정 지침

버전: v1.1 · 갱신일: 2026-11-01

## 기준과 범위

정본은 [https://github.com/yunju-baek/FreeRTOS-Tutorials-ko](https://github.com/yunju-baek/FreeRTOS-Tutorials-ko)의 `main`이며, 문서는 이미 번역·검수된 `ko/`에 있다. 최초 번역 인계 지침 v1.0을 공개 저장소 구조에 맞춰 정리했다. 대상은 공식 소개 1개와 튜토리얼 21개다. 후속 작업은 지정한 파일·절만 수정하며 STM32 포팅·TODO 풀이·새 입문 과정 작성과 구분한다.

공식 원문은 [https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials), 기준 commit은 `c89d7164ec6161c57a2ece61df412d6e3e013317`이다. 최신 원격 상태를 이유로 원문 기준이나 API를 자동 갱신하지 않는다. 원문 README는 `README.upstream.md`를 사용한다.

문체·용어의 기준은 커널북 [STYLE_GUIDE v0.5](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/STYLE_GUIDE.md)와 [GLOSSARY v0.4](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/GLOSSARY.md)다. 커널북은 읽기 연결과 용어 기준이며 튜토리얼의 버전·본문을 바꾸는 근거가 아니다.

## 보호할 내용과 표기

1. 한다체로 쓰고 학습 목표·개념·단계·조건·예외·부정·수치·단위·표·목록을 보존한다. 임의로 요약하지 않는다.
2. task·queue·semaphore·mutex·event group 등 객체명은 영어를 유지한다. 독립 튜토리얼의 첫 등장에 한국어 풀이를 붙인다. `event group(Event group)`처럼 같은 영어를 되풀이하지 않는다.
3. 제목에는 한국어 표제와 영어 원제를 사용할 수 있다. 그림·표·코드·예제의 캡션 종류와 번호는 한국어로만 쓴다. 원문에 없는 번호를 만들지 않는다.
4. 독자용 페이지에 YAML front matter나 작업용 메타데이터 헤더를 넣지 않는다. 출처·hash·검수 상태·작성자·날짜는 manifest에서 관리한다.
5. 코드 블록은 fence·언어 표기·본문·주석·공백·문자열·로그를 포함해 원문 그대로 유지한다. C source·FreeRTOSConfig.h·TODO·원문 명령을 수정하거나 실행하지 않는다. 실제 실행은 별도로 요청받은 범위에서 수행한다.
6. Ready 전환과 Running 진입, task와 ISR, tick과 ms, byte와 stack 원소 수, 선점과 시분할을 구분한다.
7. 원문 오류 의심은 `REVIEW_NOTES.md`에 원문 위치·근거·처리 방법을 적는다. 원문을 조용히 고치지 않는다.
8. 기존 공식 README에는 삽입 그림이 없다. 설명 그림이 필요하면 원문 근거·삽입 위치를 정하고 원본·파생물·검수 범위를 구분한다. 미검수 그림은 본문에 넣지 않는다.

## 파일 대응과 관련 자료

| 원문 | 한국어 |
| --- | --- |
| `README.upstream.md` | `ko/README.md` |
| `docs/tutorial_N/README.md` | `ko/docs/tutorial_N/README.md` |

`N`은 1–21이며 경로의 번호에는 앞자리 0을 붙이지 않는다. 각 튜토리얼 끝의 관련 자료에서 지정 commit의 원문 README·C source·config와 한국어 커널북 장·절을 연결한다. 원문 코드 경로는 `source/tutorials/tutorial_N/source/tutorial_N.c`, 설정은 `source/tutorials/tutorial_N/include/FreeRTOSConfig.h`다.

커널북의 장별 연결은 현재 각 번역문의 관련 자료를 기준으로 한다. T19·T20은 대응 독립 장이 없으므로 임의로 책의 11·12장에 연결하지 않는다. 한국어 목차의 상대 링크는 `docs/tutorial_N/README.md`를 사용한다.

## 검수와 기록

[작업 절차](WORKFLOW.md)를 따른다. 각 파일의 source blob SHA와 번역 SHA-256을 manifest에 기록한다. 원문 대조·문체 검토·HTML 표시·실행 검증은 구분하며, 다른 모델이나 사람이 수행했다고 대신 기록하지 않는다.

상태는 `not-started → draft → source-checked → language-reviewed → release-ready`다. 부분 검토만으로 전체 단계가 끝났다고 표시하지 않는다. 공개됐다는 사실과 `release-ready` 판정은 구분한다.
