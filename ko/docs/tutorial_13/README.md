# 뮤텍스 (Mutex)

# 학습 목표 (Learning Objectives)
* mutex(뮤텍스).

# 개념 (Concepts)
상호 배제(mutual exclusion)의 줄임말인 mutex는 공유 자원에 대한 접근을 동기화하는 기본 요소(synchronization primitive)다.

# 실습 단계 (Steps)
1. `source/tutorials/tutorial_13/source/tutorial_13.c` 파일의 모든 TODO를 완료한다.

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=13
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

1. 출력을 이해한다.

## 관련 자료
> 다음 링크는 학습을 돕기 위해 번역자가 추가한 보충 자료이며, 공식 튜토리얼 원문의 일부가 아니다.
* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_13/README.md)
* [원문 C 소스 (`tutorial_13.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_13/source/tutorial_13.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_13/include/FreeRTOSConfig.h)
* 한국어 커널북: [8.3: mutex](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch08.md)
