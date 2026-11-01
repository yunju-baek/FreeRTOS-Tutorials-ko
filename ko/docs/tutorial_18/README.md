# task notification을 경량 event group처럼 사용 (Using Task Notifications as a Light Weight Event Group)

# 학습 목표 (Learning Objectives)
* task notification(태스크 알림).
* task notification을 경량 event group(이벤트 그룹)처럼 사용.

# 개념 (Concepts)
task notification은 별도의 통신 객체 없이 task(태스크) 간 상호 작용과 ISR(인터럽트 서비스 루틴)과의 동기화를 지원하는 효율적인 메커니즘이다. task notification으로 task에 이벤트나 데이터를 보내면 queue(큐), semaphore(세마포어) 또는 event group으로 같은 동작을 수행할 때보다 훨씬 빠르다. 이러한 성능상의 이점에는 다음과 같은 사용 조건의 제한이 따른다:

1. task notification은 해당 이벤트의 수신자가 될 수 있는 task가 단 하나만 존재하는 경우에만 사용할 수 있다.
1. 수신 task는 notification을 Blocked 상태(이벤트 또는 시간 대기, CPU 시간을 전혀 소모하지 않음)에서 기다릴 수 있지만, 전송 task는 전송이 즉시 완료될 수 없을 때 전송이 완료되기를 Blocked 상태에서 기다릴 수 없다.

## task notification을 경량 event group처럼 사용 (Using task notifications as a light weight event group)
task notification을 event group 대신 사용할 때는 수신 task의 notification 값이 event group 대신 사용되고, 수신 task의 notification 값 내부 비트가 이벤트 플래그로 사용되며, event group의 `xEventGroupWaitBits()` API 함수 대신 `xTaskNotifyWait()` API 함수가 사용된다. 마찬가지로 `xEventGroupSetBits()` 함수 대신 `xTaskNotify()` API 함수(`eAction` 매개변수를 `eSetBits`로 설정)를 사용하여 비트를 설정한다.

# 실습 단계 (Steps)
1. `source/tutorials/tutorial_18/source/tutorial_18.c` 파일의 모든 TODO를 완료한다.

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=18
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

1. 출력을 이해한다.

## 관련 자료
> 다음 링크는 학습을 돕기 위해 번역자가 추가한 보충 자료이며, 공식 튜토리얼 원문의 일부가 아니다.
* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_18/README.md)
* [원문 C 소스 (`tutorial_18.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_18/source/tutorial_18.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_18/include/FreeRTOSConfig.h)
* 한국어 커널북: [10.3.5·10.3.6: notify/wait API](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch10.md)
