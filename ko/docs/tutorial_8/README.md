# 스케줄링 알고리즘 - 협력형 스케줄링 (Scheduling Algorithm - Co-operative Scheduling)

# 학습 목표 (Learning Objectives)
* 스케줄링 알고리즘.

# 개념 (Concepts)
FreeRTOS-Kernel은 다음 3가지 스케줄링 알고리즘을 지원한다:
1. 시분할을 사용하는 우선순위 기반 선점형 스케줄링 (Prioritized Preemptive Scheduling with Time Slicing)
2. 시분할 없는 우선순위 기반 선점형 스케줄링 (Prioritized Preemptive Scheduling without Time Slicing)
3. 협력형 스케줄링 (Co-operative Scheduling)

## 협력형 스케줄링 (Co-operative Scheduling)
필요한 `FreeRTOSConfig.h` 설정은 다음과 같다:

```c
#define configUSE_PREEMPTION    0
```

협력형 스케줄링을 사용하는 경우, 문맥 전환(context switch)은 Running 상태(실행 중)의 task(태스크)가 Blocked 상태(이벤트 또는 시간 대기)로 진입하거나, Running 상태의 task가 `taskYIELD()`를 호출하여 명시적으로 실행권을 양보(수동으로 재스케줄링 요청)할 때에만 발생한다. task는 결코 선점되지 않으므로 시분할은 사용할 수 없다.

# 실습 단계 (Steps)

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=8
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

1. 출력을 이해한다.

## 관련 자료

* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_8/README.md)
* [원문 C 소스 (`tutorial_8.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_8/source/tutorial_8.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_8/include/FreeRTOSConfig.h)
* 한국어 커널북: [4.12.5: 협력형 스케줄링](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch04.md)
