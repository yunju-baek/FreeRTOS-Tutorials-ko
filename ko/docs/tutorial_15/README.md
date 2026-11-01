# event group을 통한 이벤트 관리 (Event Management using Event Groups)

# 학습 목표 (Learning Objectives)
* event group(이벤트 그룹)을 통한 이벤트 관리.

# 개념 (Concepts)
event group은 task(태스크)에 이벤트를 전달할 수 있는 FreeRTOS의 또 다른 동기화 기본 요소(synchronization primitive)다. queue(큐) 및 semaphore(세마포어)와 달리:

* event group을 사용하면 task가 하나 이상의 이벤트 조합이 발생할 때까지 Blocked 상태(이벤트 또는 시간 대기)에서 기다릴 수 있다.
* event group은 이벤트가 발생했을 때 동일한 이벤트 또는 이벤트 조합을 기다리던 모든 task를 Blocked 상태에서 벗어나게(unblock) 한다.

이러한 특성 덕분에 event group을 사용하면 여러 task를 동기화하거나 둘 이상의 task에 이벤트를 브로드캐스트하고, 일련의 이벤트 중 어느 하나가 발생하거나 여러 작업이 완료될 때까지 task를 Blocked 상태에서 대기시킬 수 있다.

# 실습 단계 (Steps)
1. `source/tutorials/tutorial_15/source/tutorial_15.c` 파일의 모든 TODO를 완료한다.

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=15
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

1. 출력을 이해한다.

## 관련 자료

* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_15/README.md)
* [원문 C 소스 (`tutorial_15.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_15/source/tutorial_15.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_15/include/FreeRTOSConfig.h)
* 한국어 커널북: [9.2·9.3: event group의 특성과 이벤트 관리](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch09.md)
