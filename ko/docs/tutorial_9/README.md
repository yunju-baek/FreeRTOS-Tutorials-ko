# queue를 통한 task 간 통신 (Using Queues for Inter-Task Communication)

# 학습 목표 (Learning Objectives)
* task(태스크) 간 메시지 전송을 위한 FreeRTOS queue(큐) 생성 및 사용.

# 개념 (Concepts)
queue는 task 간 통신의 기본 메커니즘이다. queue는 task 간 또는 인터럽트(interrupt)와 task 간에 메시지를 전송하는 데 사용할 수 있다.

# 실습 단계 (Steps)
1. `source/tutorials/tutorial_9/source/tutorial_9.c` 파일의 모든 TODO를 완료한다.

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=9
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

1. 출력을 이해한다.

## 관련 자료

* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_9/README.md)
* [원문 C 소스 (`tutorial_9.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_9/source/tutorial_9.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_9/include/FreeRTOSConfig.h)
* 한국어 커널북: [5장: queue 관리](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch05.md)
