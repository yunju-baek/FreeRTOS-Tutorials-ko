# 세마포어 (Semaphore)

# 학습 목표 (Learning Objectives)
* semaphore(세마포어).

# 개념 (Concepts)
semaphore는 task(태스크) 간 또는 인터럽트(interrupt)와 task 간의 동기화(synchronization)를 가능하게 하는 동기화 기본 요소(synchronization primitive)다.

# 실습 단계 (Steps)
1. `source/tutorials/tutorial_12/source/tutorial_12.c` 파일의 모든 TODO를 완료한다.

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=12
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

1. 출력을 이해한다.

## 관련 자료
> 다음 링크는 학습을 돕기 위해 번역자가 추가한 보충 자료이며, 공식 튜토리얼 원문의 일부가 아니다.
* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_12/README.md)
* [원문 C 소스 (`tutorial_12.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_12/source/tutorial_12.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_12/include/FreeRTOSConfig.h)
* 한국어 커널북: [7.4: binary semaphore를 통한 동기화](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch07.md)
