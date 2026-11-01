# 정적 할당 (Static Allocation)

# 학습 목표 (Learning Objectives)
* 정적 할당(static allocation).

# 개념 (Concepts)
각 FreeRTOS task(태스크)에는 메모리 블록 2개가 필요하다:
* Task Control Block (TCB) - FreeRTOS-Kernel이 task의 제어 정보를 저장하는 데 사용한다.
* stack(스택) - 각 task는 자신의 stack을 유지한다.

`xTaskCreate` API로 task를 생성하면 `pvPortMalloc`을 호출하여 FreeRTOS heap(힙)에서 TCB와 stack에 필요한 메모리를 할당한다. `xTaskCreateStatic` API로 task를 생성하면 애플리케이션 작성자가 TCB와 stack에 필요한 메모리를 직접 제공한다.

# 실습 단계 (Steps)
1. `source/tutorials/tutorial_3/source/tutorial_3.c` 파일의 모든 TODO를 완료한다.

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=3
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

## 관련 자료
> 다음 링크는 학습을 돕기 위해 번역자가 추가한 보충 자료이며, 공식 튜토리얼 원문의 일부가 아니다.
* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_3/README.md)
* [원문 C 소스 (`tutorial_3.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_3/source/tutorial_3.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_3/include/FreeRTOSConfig.h)
* 한국어 커널북: [3.4: 정적 메모리 할당](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch03.md)
