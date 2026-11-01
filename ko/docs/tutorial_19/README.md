# 스트림 버퍼 (Stream Buffer)

# 학습 목표 (Learning Objectives)
* stream buffer(스트림 버퍼).

# 개념 (Concepts)
stream buffer는 인터럽트 서비스 루틴(ISR)에서 task(태스크)로, 또는 한 task에서 다른 task로 바이트 스트림(stream of bytes)을 전달하는 task 간 통신 기본 요소(inter-task communication primitive)다. 바이트 스트림은 임의의 길이를 가질 수 있으며 반드시 시작이나 끝이 있을 필요는 없다.

**중요 참고 사항:** stream buffer 구현은 버퍼에 데이터를 쓰는 task 또는 인터럽트가 단 하나(writer)이고, 버퍼에서 데이터를 읽는 task 또는 인터럽트가 단 하나(reader)라고 가정한다.

데이터를 기다리고 있는 task가 Blocked 상태(이벤트 또는 시간 대기)에서 벗어나기 위해 stream buffer 내에 반드시 존재해야 하는 데이터의 양을 stream buffer의 trigger level(트리거 레벨)이라고 한다. 예를 들어:
* task가 trigger level이 1인 빈 stream buffer의 읽기 동작에서 Blocked 상태가 된 경우, 단일 바이트가 버퍼에 기록되거나 task의 최대 대기 시간(block time)이 만료되면 task는 Blocked 상태에서 벗어난다(unblock).
* task가 trigger level이 10인 빈 stream buffer의 읽기 동작에서 Blocked 상태가 된 경우, stream buffer에 최소 10바이트가 포함되거나 task의 최대 대기 시간이 만료될 때까지 task는 Blocked 상태에서 벗어나지 않는다.

stream buffer의 데이터 양이 trigger level에 도달하기 전에 최대 대기 시간이 만료되어도, 읽기 task는 그때 수신할 수 있는 바이트 수만큼 데이터를 받는다.

# 실습 단계 (Steps)
1. `source/tutorials/tutorial_19/source/tutorial_19.c` 파일의 모든 TODO를 완료한다.

1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=19
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

1. 출력을 이해한다.

## 관련 자료

* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_19/README.md)
* [원문 C 소스 (`tutorial_19.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_19/source/tutorial_19.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_19/include/FreeRTOSConfig.h)
* 한국어 커널북: 커널북에 대응하는 독립 장 없음. 공식 튜토리얼을 직접 근거로 사용
