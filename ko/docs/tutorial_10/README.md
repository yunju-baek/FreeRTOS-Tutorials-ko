# 인터럽트와 task 간의 queue 통신 (Using Queues for Communication between Interrupts and Tasks)

# 학습 목표 (Learning Objectives)
* 인터럽트와 task(태스크) 간 메시지 전송을 위한 FreeRTOS queue(큐) 생성 및 사용.

# 개념 (Concepts)
queue는 task 간 통신의 기본 메커니즘이다. queue는 task 간 또는 인터럽트와 task 간에 메시지를 전송하는 데 사용할 수 있다.

# 실습 단계 (Steps)
1. `source/tutorials/tutorial_10/source/tutorial_10.c` 파일의 모든 TODO를 완료한다.

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=10
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

1. 출력을 이해한다.

## 관련 자료
> 다음 링크는 학습을 돕기 위해 번역자가 추가한 보충 자료이며, 공식 튜토리얼 원문의 일부가 아니다.
* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_10/README.md)
* [원문 C 소스 (`tutorial_10.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_10/source/tutorial_10.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_10/include/FreeRTOSConfig.h)
* 한국어 커널북: [7.2·7.7: FromISR API와 queue](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch07.md)
