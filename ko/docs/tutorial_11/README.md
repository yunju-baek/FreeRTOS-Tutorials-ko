# 소프트웨어 타이머 (Software Timers)

# 학습 목표 (Learning Objectives)
* software timer(소프트웨어 타이머).

# 개념 (Concepts)
software timer는 미래의 지정된 시간이나 고정된 주기로 함수의 실행을 스케줄링하는 데 사용된다. software timer가 실행하는 함수를 software timer의 callback 함수(콜백 함수)라고 한다.

software timer callback 함수는 C 함수로 구현하며 반드시 다음 함수 원형을 따라야 한다:

```c
void TimerCallback( TimerHandle_t xTimer );
```
software timer의 주기(period)는 software timer가 시작된 시점부터 callback 함수가 실행되는 시점까지의 시간이다. software timer에는 두 가지 유형이 있다:

1. one-shot timer(단발 타이머) - 시작하면 callback 함수를 한 번만 실행한다. one-shot timer는 수동으로 다시 시작할 수 있지만 스스로 다시 시작하지는 않는다.

2. auto-reload timer(자동 재장전 타이머) - 시작하면 만료될 때마다 스스로 다시 시작하므로 callback 함수가 주기적으로 실행된다.

# 실습 단계 (Steps)
1. `source/tutorials/tutorial_11/source/tutorial_11.c` 파일의 모든 TODO를 완료한다.

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=11
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

1. 출력을 이해한다.

## 관련 자료
> 다음 링크는 학습을 돕기 위해 번역자가 추가한 보충 자료이며, 공식 튜토리얼 원문의 일부가 아니다.
* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_11/README.md)
* [원문 C 소스 (`tutorial_11.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_11/source/tutorial_11.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_11/include/FreeRTOSConfig.h)
* 한국어 커널북: [6장: software timer 관리](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch06.md)
