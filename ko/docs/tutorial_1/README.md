# 베어메탈 프로젝트에 FreeRTOS 추가 (Add FreeRTOS to a Bare Metal Project)

# 학습 목표 (Learning Objectives)
* 베어메탈 프로젝트에 FreeRTOS를 추가한다.
* 빌드하고 실행한다.

# 개념 (Concepts)
FreeRTOS 코드는 크게 2가지 부분으로 구성된다:
1. **공통 코드(Common Code)** - C 언어로 작성되어 모든 플랫폼에 적용 가능한 코드다. 다음 파일들을 포함한다 -
   * `FreeRTOS-Kernel/list.c`
   * `FreeRTOS-Kernel/tasks.c`
   * `FreeRTOS-Kernel/timers.c`
   * `FreeRTOS-Kernel/queue.c`
   * `FreeRTOS-Kernel/event_groups.c`
   * `FreeRTOS-Kernel/stream_buffer.c`
   * `FreeRTOS-Kernel/portable/MemMang/heap_x.c` (여기서 x는 1, 2, 3, 4, 5 중 하나다)

1. **port 계층(Port Layer)** - 어셈블리와 C로 작성되며 컴파일러(예: GCC) 및 마이크로컨트롤러 아키텍처(예: Cortex-M4)에 특화된 코드다. 본 튜토리얼에서는 `FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix` port를 사용한다.

1. **설정 파일(Config File)** - 애플리케이션 작성자는 FreeRTOS-Kernel 동작을 설정하기 위해 `FreeRTOSConfig.h` 파일을 제공해야 한다.

> [옮긴이 주] 원문은 코드가 2가지 부분으로 구성된다고 설명하지만, 목록에는 공통 코드·port 계층·설정 파일의 세 항목을 나열한다.

# 실습 단계 (Steps)
1. 빌드를 위해 저장소 루트에서 다음 명령을 실행한다:
   ```shell
   rm -rf build
   mkdir build
   cd build
   cmake -B . -S ../source/ -DTUTORIAL=1
   make
   ```

1. 바이너리를 실행하기 위해 다음 명령을 실행한다:
   ```shell
   ./freertos_example
   ```

## 관련 자료

* [원문 README](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/docs/tutorial_1/README.md)
* [원문 C 소스 (`tutorial_1.c`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_1/source/tutorial_1.c)
* [원문 설정 (`FreeRTOSConfig.h`)](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Tutorials/blob/c89d7164ec6161c57a2ece61df412d6e3e013317/source/tutorials/tutorial_1/include/FreeRTOSConfig.h)
* 한국어 커널북: [2.2·2.4: 소스 구성과 프로젝트 생성](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch02.md)
