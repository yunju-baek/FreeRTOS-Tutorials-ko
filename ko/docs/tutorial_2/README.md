# task 생성 (Task Creation)

# 학습 목표 (Learning Objectives)
* FreeRTOS API를 사용하여 task(태스크)를 생성한다.

# 개념 (Concepts)

## task 함수 (Task Functions)
FreeRTOS task는 C 함수로 구현하며 반드시 다음 함수 원형을 따라야 한다:

```c
void TaskFunction( void *pvParameters );
```

각 task는 통상 무한 루프 내에서 영원히 실행되며 종료되지 않는다. 전형적인 task 구조는 다음과 같다:

```c
void TaskFunction( void *pvParameters )
{
    /* A task is normally implemented as an infinite loop. */
    for( ;; )
    {
        /* The code to implement the task functionality goes here. */
    }
}
```

# 실습 단계 (Steps)
1. `source/tutorials/tutorial_2/source/tutorial_2.c` 파일의 모든 TODO를 완료한다.

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=2
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

## 관련 자료

* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_2/README.md)
* [원문 C 소스 (`tutorial_2.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_2/source/tutorial_2.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_2/include/FreeRTOSConfig.h)
* 한국어 커널북: [4.2·4.4: task 함수와 생성](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch04.md)
