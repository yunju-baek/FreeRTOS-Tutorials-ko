# task notification을 경량 binary semaphore처럼 사용 (Using Task Notifications as a Light Weight Binary Semaphore)

# 학습 목표 (Learning Objectives)
* task notification(태스크 알림).
* task notification을 경량 binary semaphore(이진 세마포어)처럼 사용.

# 개념 (Concepts)
task notification은 별도의 통신 객체 없이 task(태스크) 간 상호 작용과 ISR(인터럽트 서비스 루틴)과의 동기화를 지원하는 효율적인 메커니즘이다. task notification으로 task에 이벤트나 데이터를 보내면 queue(큐), semaphore(세마포어) 또는 event group(이벤트 그룹)으로 같은 동작을 수행할 때보다 훨씬 빠르다. 이러한 성능상의 이점에는 다음과 같은 사용 조건의 제한이 따른다:

1. task notification은 해당 이벤트의 수신자가 될 수 있는 task가 단 하나만 존재하는 경우에만 사용할 수 있다.
1. 수신 task는 notification을 Blocked 상태(이벤트 또는 시간 대기, CPU 시간을 전혀 소모하지 않음)에서 기다릴 수 있지만, 전송 task는 전송이 즉시 완료될 수 없을 때 전송이 완료되기를 Blocked 상태에서 기다릴 수 없다.

## task notification을 경량 binary semaphore처럼 사용 (Using task notifications as a light weight binary semaphore)
task notification을 binary semaphore 대신 사용할 때는 수신 task의 notification 값이 binary semaphore의 count 값 대신 사용되며, semaphore의 `xSemaphoreTake()` API 함수 대신 `ulTaskNotifyTake()`(또는 `ulTaskNotifyTakeIndexed()`) API 함수가 사용된다. `ulTaskNotifyTake()` 함수의 `xClearOnExit` 매개변수는 `pdTRUE`로 설정되므로 notification을 take(획득)할 때마다 count 값이 0으로 되돌아간다(즉 binary semaphore를 에뮬레이션함). 마찬가지로 semaphore의 `xSemaphoreGive()` 함수 대신 `xTaskNotifyGive()`(또는 `xTaskNotifyGiveIndexed()`)가 사용된다.

# 실습 단계 (Steps)
1. `source/tutorials/tutorial_17/source/tutorial_17.c` 파일의 모든 TODO를 완료한다.

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=17
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

1. 출력을 이해한다.

## 관련 자료

* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_17/README.md)
* [원문 C 소스 (`tutorial_17.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_17/source/tutorial_17.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_17/include/FreeRTOSConfig.h)
* 한국어 커널북: [10.3.2–10.3.4: give/take API](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch10.md)
