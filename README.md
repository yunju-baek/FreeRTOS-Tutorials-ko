# FreeRTOS Tutorials 한국어판

[FreeRTOS 공식 튜토리얼](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials)의 소개와 실습 21개를 한국어로 옮겼다. **비공식 한국어 번역**이며 FreeRTOS/AWS의 공식 발행물이 아니다.

**[한국어 튜토리얼 목차](ko/README.md)** · **[커널북 한국어판](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko)** · [커널북 목차](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/main/ko/toc.md) · [영문 원본 소개](README.upstream.md)

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

각 튜토리얼 끝에는 원문 README, C 소스, `FreeRTOSConfig.h`와 관련 커널북 장을 연결했다.

## 실습 준비

예제를 실행하려면 GCC 컴파일러와 CMake 3.15.0 이상이 필요하다. 예제는 POSIX port를 사용하며, 학습자가 완성할 TODO가 포함되어 있다.

```sh
git clone --recurse-submodules https://github.com/yunju-baek/FreeRTOS-Tutorials-ko.git
cd FreeRTOS-Tutorials-ko
```

튜토리얼별 빌드·실행 방법은 각 문서의 실습 단계를 따른다. 문서만 읽을 때는 clone이나 submodule 초기화가 필요하지 않다.

## 출처와 이용 조건

원문: [FreeRTOS/Lab-Project-FreeRTOS-Tutorials](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials). 저작권: Amazon.com, Inc. or its affiliates. All Rights Reserved.

[MIT No Attribution 라이선스](LICENSE)를 따른다. kernel과 하위 submodule, 커널북은 각 저장소에 명시된 라이선스를 따른다.
