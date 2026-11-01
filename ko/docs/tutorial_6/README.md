# 스케줄링 알고리즘 - 시분할을 사용하는 우선순위 기반 선점형 스케줄링 (Scheduling Algorithm - Prioritized Preemptive Scheduling with Time Slicing)

# 학습 목표 (Learning Objectives)
* 스케줄링 알고리즘.

# 개념 (Concepts)
FreeRTOS-Kernel은 다음 3가지 스케줄링 알고리즘을 지원한다:
1. 시분할을 사용하는 우선순위 기반 선점형 스케줄링 (Prioritized Preemptive Scheduling with Time Slicing)
2. 시분할 없는 우선순위 기반 선점형 스케줄링 (Prioritized Preemptive Scheduling without Time Slicing)
3. 협력형 스케줄링 (Co-operative Scheduling)

## 시분할을 사용하는 우선순위 기반 선점형 스케줄링 (Prioritized Preemptive Scheduling with Time Slicing)
필요한 `FreeRTOSConfig.h` 설정은 다음과 같다:

```c
#define configUSE_PREEMPTION    1
#define configUSE_TIME_SLICING  1
```

시분할을 사용하는 우선순위 기반 선점형 스케줄링 알고리즘은 선점(preemption)과 시분할(time slicing)을 모두 사용한다.

**선점(preemption)** - 선점형 스케줄링 알고리즘은 Running 상태(실행 중)인 task(태스크)보다 우선순위가 더 높은 task가 Ready 상태(실행 가능)로 진입하면 즉시 Running 상태의 task를 선점한다. 선점된다는 것은 다른 task가 실행될 수 있도록 비자발적으로(명시적으로 실행권을 양보하거나 Blocked 상태로 들어가지 않고) Running 상태에서 벗어나 Ready 상태로 이동한다는 뜻이다.

**시분할(time slicing)** - 시분할은 task가 명시적으로 실행권을 양보하거나 Blocked 상태로 들어가지 않더라도, 동일한 우선순위를 가진 task 간에 처리 시간을 공유하는 데 사용된다. 시분할은 Running 상태의 task와 동일한 우선순위를 가진 다른 Ready 상태 task가 존재하는 경우, 각 time slice(시간 분할 구간)의 끝에서 실행할 새 task를 선택한다. 하나의 time slice는 두 번의 RTOS tick interrupt(틱 인터럽트) 사이의 시간과 같다.

# 실습 단계 (Steps)

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=6
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

1. 출력을 이해한다.

## 관련 자료

* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_6/README.md)
* [원문 C 소스 (`tutorial_6.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_6/source/tutorial_6.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_6/include/FreeRTOSConfig.h)
* 한국어 커널북: [4.12.3: 시분할 선점형 스케줄링](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch04.md)
