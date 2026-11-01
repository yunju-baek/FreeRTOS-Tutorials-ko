# event group을 통한 task 동기화 (Task Synchronization using Event Groups)

# 학습 목표 (Learning Objectives)
* event group(이벤트 그룹)을 통한 task(태스크) 동기화.

# 개념 (Concepts)
event group을 사용하여 동기화 지점(synchronization point)을 생성할 수 있다:

* 동기화에 참여해야 하는 각 task에는 event group 내의 고유한 event bit(이벤트 비트)가 할당된다.
* 각 task는 동기화 지점에 도달했을 때 자신에게 할당된 event bit를 설정(set)한다.
* 자신의 event bit를 설정한 각 task는 event group에서 대기(block)하여 동기화에 참여하는 다른 모든 task를 나타내는 event bit 또한 설정되기를 기다린다.

# 실습 단계 (Steps)
1. `source/tutorials/tutorial_16/source/tutorial_16.c` 파일의 모든 TODO를 완료한다.

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=16
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

1. 출력을 이해한다.

## 관련 자료
> 다음 링크는 학습을 돕기 위해 번역자가 추가한 보충 자료이며, 공식 튜토리얼 원문의 일부가 아니다.
* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_16/README.md)
* [원문 C 소스 (`tutorial_16.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_16/source/tutorial_16.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_16/include/FreeRTOSConfig.h)
* 한국어 커널북: [9.4: task 동기화](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch09.md)
