# FreeRTOS 시작하기 (Getting-Started-With-FreeRTOS)

> 이 문서는 [FreeRTOS 공식 튜토리얼 저장소](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials)의 비공식 한국어 번역본이다.

이 저장소에는 FreeRTOS 커널을 처음 사용하는 사람을 위한 튜토리얼 모음이 담겨 있다.

관련 개념은 [FreeRTOS 커널북 한국어판](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko)과 [커널북 목차](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/main/ko/toc.md)에서 읽을 수 있다.

# 사전 준비 항목 (Prerequisites)
* GCC 컴파일러
* CMake 3.15.0 이상
* 서브모듈과 함께 이 저장소를 복제한다:
  ```
  git clone --recurse-submodules https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials.git
  ```
  또는
  ```
  git clone https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials.git
  cd Lab-Project-FreeRTOS-Tutorials
  git submodule update --init --recursive
  ```

# 튜토리얼 목록 (Tutorials)
## 베어메탈 프로젝트에 FreeRTOS 추가 (Add FreeRTOS to a Bare Metal Project)
* [베어메탈 프로젝트에 FreeRTOS 추가 (Add FreeRTOS to a Bare Metal Project)](docs/tutorial_1/README.md)

## 태스크와 스케줄링 (Tasks and Scheduling)
* [task 생성 (Task Creation)](docs/tutorial_2/README.md)
* [정적 할당 (Static Allocation)](docs/tutorial_3/README.md)
* [task 우선순위 (Task Priorities)](docs/tutorial_4/README.md)
* [task 매개변수 (Task Parameters)](docs/tutorial_5/README.md)
* [스케줄링 알고리즘 - 시분할을 사용하는 우선순위 기반 선점형 스케줄링 (Scheduling Algorithm - Prioritized Preemptive Scheduling with Time Slicing)](docs/tutorial_6/README.md)
* [스케줄링 알고리즘 - 시분할 없는 우선순위 기반 선점형 스케줄링 (Scheduling Algorithm - Prioritized Preemptive Scheduling without Time Slicing)](docs/tutorial_7/README.md)
* [스케줄링 알고리즘 - 협력형 스케줄링 (Scheduling Algorithm - Co-operative Scheduling)](docs/tutorial_8/README.md)

## 큐 (Queue)
* [queue를 통한 task 간 통신 (Using Queues for Inter-Task Communication)](docs/tutorial_9/README.md)
* [인터럽트와 task 간의 queue 통신 (Using Queues for Communication between Interrupts and Tasks)](docs/tutorial_10/README.md)

## 소프트웨어 타이머 (Software Timer)
* [소프트웨어 타이머 (Software Timers)](docs/tutorial_11/README.md)

## 세마포어와 뮤텍스 (Semaphore and Mutex)
* [세마포어 (Semaphore)](docs/tutorial_12/README.md)
* [뮤텍스 (Mutex)](docs/tutorial_13/README.md)
* [우선순위 역전과 우선순위 상속 (Priority Inversion and Priority Inheritance)](docs/tutorial_14/README.md)

## 이벤트 그룹 (Event Group)
* [event group을 통한 이벤트 관리 (Event Management using Event Groups)](docs/tutorial_15/README.md)
* [event group을 통한 task 동기화 (Task Synchronization using Event Groups)](docs/tutorial_16/README.md)

## 태스크 알림 (Task Notification)
* [task notification을 경량 binary semaphore처럼 사용 (Using Task Notifications as a Light Weight Binary Semaphore)](docs/tutorial_17/README.md)
* [task notification을 경량 event group처럼 사용 (Using Task Notifications as a Light Weight Event Group)](docs/tutorial_18/README.md)

## 스트림 버퍼 (Stream Buffer)
* [스트림 버퍼 (Stream Buffer)](docs/tutorial_19/README.md)

## 메시지 버퍼 (Message Buffer)
* [메시지 버퍼 (Message Buffer)](docs/tutorial_20/README.md)

## 자원 관리 (Resource Management)
* [자원 관리 (Resource Management)](docs/tutorial_21/README.md)
