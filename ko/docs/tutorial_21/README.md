# 자원 관리 (Resource Management)

# 학습 목표 (Learning Objectives)
* 자원 관리(Resource Management).

# 개념 (Concepts)
멀티태스킹 시스템에서는 한 task(태스크)가 공유 자원에 접근하기 시작했으나 선점(preempt)되기 전에 접근을 완료하지 못하면 오류가 발생할 가능성이 있다. task가 자원을 일관되지 않은 상태(inconsistent state)로 남겨두면 다른 task나 인터럽트가 같은 자원에 접근할 때 데이터 손상(data corruption)이나 이와 유사한 문제가 발생할 수 있다.

데이터 일관성이 항상 유지되도록 하려면 공유 자원에 대한 접근을 '상호 배제(mutual exclusion)' 기법을 사용하여 관리해야 한다. 그 목표는 한 task가 공유 자원에 접근하기 시작하면 해당 자원이 일관된 상태로 되돌아갈 때까지 동일한 task가 자원에 대한 독점적인 접근 권한을 갖도록 보장하는 것이다.

FreeRTOS는 상호 배제를 구현하는 여러 기능을 제공하지만, 가장 좋은 방법은 가능한 경우 자원을 공유하지 않고 각 자원에 task 하나만 접근하도록 애플리케이션을 설계하는 것이다(실제로는 이렇게 설계하기 어려운 경우가 많다).

## 임계 구역 (Critical Sections)
critical section(임계 구역)은 각각 `taskENTER_CRITICAL()` 및 `taskEXIT_CRITICAL()` 매크로 호출로 둘러싸인 코드 영역이다. critical section은 인터럽트를 비활성화하여 작동하므로 매우 짧게 유지해야 한다. 그렇지 않으면 인터럽트 응답 시간에 부정적인 영향을 미친다.

## scheduler 일시 중지 (또는 잠금) (Suspending (or Locking) the Scheduler)
여러 task가 공유 자원에 접근하고 어떤 인터럽트도 그 자원에 접근하지 않는 경우, scheduler(스케줄러)를 일시 중지(또는 잠금)하여 접근을 동기화할 수 있다. scheduler를 일시 중지하면 문맥 전환(context switch)은 발생하지 않지만 인터럽트는 활성화된 상태로 유지된다. scheduler가 일시 중지된 동안 인터럽트가 문맥 전환을 요청하면 해당 요청은 대기(pending) 상태로 유지되며, scheduler가 재개(un-suspended)될 때에만 수행된다.

scheduler는 `vTaskSuspendAll()`을 호출하여 일시 중지하고, `xTaskResumeAll()`을 호출하여 재개한다.

## 뮤텍스 (Mutex)
상호 배제(mutual exclusion)의 줄임말인 mutex(뮤텍스)는 둘 이상의 task가 공유하는 자원에 대한 접근을 동기화하는 기본 요소(synchronization primitive)다.

# 실습 단계 (Steps)
1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=21
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

1. 출력을 이해한다.

## 관련 자료

* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_21/README.md)
* [원문 C 소스 (`tutorial_21.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_21/source/tutorial_21.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_21/include/FreeRTOSConfig.h)
* 한국어 커널북: [8.2·8.3: critical section·scheduler 중지·mutex](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch08.md)
