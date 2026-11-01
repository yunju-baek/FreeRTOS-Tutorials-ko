# task 우선순위 (Task Priorities)

# 학습 목표 (Learning Objectives)
* task(태스크) 우선순위.

# 개념 (Concepts)
FreeRTOS scheduler(스케줄러)는 항상 Ready 상태(실행 가능)의 task 중 우선순위가 가장 높은 task를 실행한다.

# 실습 단계 (Steps)
1. `source/tutorials/tutorial_4/source/tutorial_4.c` 파일의 모든 TODO를 완료한다.

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=4
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

## 관련 자료

* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_4/README.md)
* [원문 C 소스 (`tutorial_4.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_4/source/tutorial_4.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_4/include/FreeRTOSConfig.h)
* 한국어 커널북: [4.5: 우선순위](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch04.md)
