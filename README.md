# FreeRTOS Tutorials 한국어판

**비공식 한국어 번역 · Unofficial Korean translation**

[FreeRTOS/Lab-Project-FreeRTOS-Tutorials](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials)의 소개와 공식 튜토리얼 21개를 한국어로 옮긴 저장소다. FreeRTOS 또는 AWS가 제공하거나 승인·검수한 공식 번역본이 아니다.

This is an **unofficial Korean translation** of the [FreeRTOS tutorials](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials). It is not an official or endorsed FreeRTOS/AWS publication.

**[한국어 튜토리얼 목차에서 읽기](ko/README.md)** · **[함께 읽는 커널북 한국어판](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko)** · [커널북 목차](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/main/ko/toc.md) · [영문 원본 소개](README.upstream.md)

## 튜토리얼

| 번호 | 한국어 문서 |
| --- | --- |
| T01 | [베어메탈 프로젝트에 FreeRTOS 추가 (Add FreeRTOS to a Bare Metal Project)](ko/docs/tutorial_1/README.md) |
| T02 | [task 생성 (Task Creation)](ko/docs/tutorial_2/README.md) |
| T03 | [정적 할당 (Static Allocation)](ko/docs/tutorial_3/README.md) |
| T04 | [task 우선순위 (Task Priorities)](ko/docs/tutorial_4/README.md) |
| T05 | [task 매개변수 (Task Parameters)](ko/docs/tutorial_5/README.md) |
| T06 | [스케줄링 알고리즘 - 시분할을 사용하는 우선순위 기반 선점형 스케줄링 (Scheduling Algorithm - Prioritized Preemptive Scheduling with Time Slicing)](ko/docs/tutorial_6/README.md) |
| T07 | [스케줄링 알고리즘 - 시분할 없는 우선순위 기반 선점형 스케줄링 (Scheduling Algorithm - Prioritized Preemptive Scheduling without Time Slicing)](ko/docs/tutorial_7/README.md) |
| T08 | [스케줄링 알고리즘 - 협력형 스케줄링 (Scheduling Algorithm - Co-operative Scheduling)](ko/docs/tutorial_8/README.md) |
| T09 | [queue를 통한 task 간 통신 (Using Queues for Inter-Task Communication)](ko/docs/tutorial_9/README.md) |
| T10 | [인터럽트와 task 간의 queue 통신 (Using Queues for Communication between Interrupts and Tasks)](ko/docs/tutorial_10/README.md) |
| T11 | [소프트웨어 타이머 (Software Timers)](ko/docs/tutorial_11/README.md) |
| T12 | [세마포어 (Semaphore)](ko/docs/tutorial_12/README.md) |
| T13 | [뮤텍스 (Mutex)](ko/docs/tutorial_13/README.md) |
| T14 | [우선순위 역전과 우선순위 상속 (Priority Inversion and Priority Inheritance)](ko/docs/tutorial_14/README.md) |
| T15 | [event group을 통한 이벤트 관리 (Event Management using Event Groups)](ko/docs/tutorial_15/README.md) |
| T16 | [event group을 통한 task 동기화 (Task Synchronization using Event Groups)](ko/docs/tutorial_16/README.md) |
| T17 | [task notification을 경량 binary semaphore처럼 사용 (Using Task Notifications as a Light Weight Binary Semaphore)](ko/docs/tutorial_17/README.md) |
| T18 | [task notification을 경량 event group처럼 사용 (Using Task Notifications as a Light Weight Event Group)](ko/docs/tutorial_18/README.md) |
| T19 | [스트림 버퍼 (Stream Buffer)](ko/docs/tutorial_19/README.md) |
| T20 | [메시지 버퍼 (Message Buffer)](ko/docs/tutorial_20/README.md) |
| T21 | [자원 관리 (Resource Management)](ko/docs/tutorial_21/README.md) |

각 문서의 관련 자료에서 원문 README, C source, `FreeRTOSConfig.h`, 관련 커널북 장·절을 찾을 수 있다. T19·T20의 stream/message buffer에는 이 커널북 판본의 직접 대응 독립 장이 없다.

## 구성과 검수

- `ko/`: 소개 1개와 튜토리얼 21개의 한국어 Markdown, 번역 manifest와 검수 기록.
- `docs/`, `source/`: 공식 영문 튜토리얼, 예제 C source, 설정, kernel submodule.
- `README.upstream.md`: 원래 저장소 소개를 byte 단위로 보존한 파일.
- [검수 결과](ko/REVIEW_NOTES.md): 표현·용어 교정, 원문 의심 사항, 실제 검수 범위.
- [번역 지침](ko/TRANSLATION_GUIDE.md), [작업 절차](ko/WORKFLOW.md), [현재 상태](PROJECT_STATE.md).

원문과 한국어 문서의 코드 블록 50개가 일치한다. 영문 원본 파일 70개, 한국어 목차 링크 21개, 고정 원문 링크 63개와 커널북 장 연결 19개를 확인했다. 22개 문서의 front matter 부재와 HTML 표시를 검수했다. 현재 상태는 `source-checked`이며 사람의 최종 교열·예제 빌드·실행·실보드 검증을 완료한 판본은 아니다.

## 받기와 검사

```sh
git clone --recurse-submodules https://github.com/yunju-baek/FreeRTOS-Tutorials-ko.git
cd FreeRTOS-Tutorials-ko
python3 tools/verify_translation.py
```

문서만 읽을 때는 submodule 초기화가 필요하지 않다. 원문 예제에는 학습자가 완성할 TODO가 있으며, 이번 번역에서 해답으로 바꾸지 않았다. 원문 빌드는 POSIX port를 사용한다. STM32 포팅이나 플랫폼별 실행 검증은 별도 작업이다.

한국어판 정본은 이 저장소의 `main`이다. `origin`은 한국어판, `upstream`은 [공식 영문 저장소](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials)로 구분한다. 영문 기준은 [`c89d7164ec6161c57a2ece61df412d6e3e013317`](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/tree/c89d7164ec6161c57a2ece61df412d6e3e013317)이며 `upstream-baseline` branch와 manifest에 보존했다. 후속 수정은 작업 branch에서 검수한 뒤 이 저장소에 PR로 제출한다.

## 출처와 이용 조건

원문 저작권 고지: **Amazon.com, Inc. or its affiliates. All Rights Reserved.** 한국어 번역·교정과 커널북 연결은 원문에 대한 변경이다. [원본 LICENSE](LICENSE)의 **MIT No Attribution** 고지를 보존한다. kernel과 하위 submodule의 별도 라이선스도 유지한다. 커널북은 별도 저장소이며 그 저장소에 명시된 이용 조건을 따른다.
