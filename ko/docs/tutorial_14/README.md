# 우선순위 역전과 우선순위 상속 (Priority Inversion and Priority Inheritance)

# 학습 목표 (Learning Objectives)
* 우선순위 역전(priority inversion)과 우선순위 상속(priority inheritance).

# 개념 (Concepts)
## 우선순위 역전 (Priority Inversion)
우선순위 역전은 낮은 우선순위 task(태스크)가 보유한 자원을 높은 우선순위 task가 기다리는 동안, 자원과 무관한 중간 우선순위 task 때문에 낮은 우선순위 task가 진행하지 못해 높은 우선순위 task가 무기한 지연되는 상황이다. 그 결과 높은 우선순위 task가 자신보다 우선순위가 낮은 중간 우선순위 task 때문에 실행하지 못해, task에 할당된 우선순위가 사실상 뒤바뀐다. 이는 높은 우선순위 task의 실행은 오직 더 높은 우선순위 task만 막을 수 있다는 우선순위 모델을 위배한다.

## 우선순위 상속 (Priority Inheritance)
우선순위 상속은 대기 시간에 상한이 없는(unbounded) 우선순위 역전을 제거하는 방법이다. 높은 우선순위 task가 낮은 우선순위 task가 보유한 자원을 획득하려다가 Blocked 상태로 대기하면, 자원을 보유한 task의 우선순위를 대기 중인 task의 우선순위로 일시적으로 높인다. 이 메커니즘은 높은 우선순위 task가 가능한 한 짧은 시간 동안만 Blocked 상태로 대기하도록 보장하여, 이미 발생한 우선순위 역전을 최소화한다.

# 실습 단계 (Steps)
1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=14
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

1. 출력을 이해한다.

## 관련 자료
> 다음 링크는 학습을 돕기 위해 번역자가 추가한 보충 자료이며, 공식 튜토리얼 원문의 일부가 아니다.
* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_14/README.md)
* [원문 C 소스 (`tutorial_14.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_14/source/tutorial_14.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_14/include/FreeRTOSConfig.h)
* 한국어 커널북: [8.3.2·8.3.3: 역전과 상속](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch08.md)
