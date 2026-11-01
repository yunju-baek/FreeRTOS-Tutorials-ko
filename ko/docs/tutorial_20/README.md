# 메시지 버퍼 (Message Buffer)

# 학습 목표 (Learning Objectives)
* message buffer(메시지 버퍼).

# 개념 (Concepts)
message buffer는 인터럽트 서비스 루틴(ISR)에서 task(태스크)로, 또는 한 task에서 다른 task로 서로 구분되는 가변 길이의 메시지(discrete messages)를 전달하는 task 간 통신 기본 요소(inter-task communication primitive)다. 메시지는 개별 바이트 단위가 아니라 메시지 전체 단위로만 읽어낼 수 있다.

**중요 참고 사항:** message buffer 구현은 버퍼에 데이터를 쓰는 task 또는 인터럽트가 단 하나(writer)이고, 버퍼에서 데이터를 읽는 task 또는 인터럽트가 단 하나(reader)라고 가정한다.

# 실습 단계 (Steps)
1. `source/tutorials/tutorial_20/source/tutorial_20.c` 파일의 모든 TODO를 완료한다.

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=20
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

1. 출력을 이해한다.

## 관련 자료

* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_20/README.md)
* [원문 C 소스 (`tutorial_20.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_20/source/tutorial_20.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_20/include/FreeRTOSConfig.h)
* 한국어 커널북: 커널북에 대응하는 독립 장 없음. 공식 튜토리얼을 직접 근거로 사용
