# 공통 실습 환경 준비 가이드 (Getting Started)

이 문서는 FreeRTOS 튜토리얼 21종을 자신의 PC에서 직접 빌드하고 실행하기 위한 **개발 환경 구축 및 공통 실습 절차**를 안내한다.

---

## 1. 실습 환경 개요

FreeRTOS는 본래 마이크로컨트롤러(MCU)에서 구동되는 실시간 커널이지만, 본 튜토리얼은 하드웨어 장비(MCU 보드, JTAG 디버거, 오실로스코프 등) 없이도 PC에서 곧바로 학습할 수 있도록 **POSIX 시뮬레이션 포트(`GCC/Posix`)**를 기반으로 설계되어 있다.

* **동작 원리**: FreeRTOS 커널의 tick 인터럽트와 task 문맥 전환을 호스트 운영체제(Linux/macOS)의 POSIX 스레드(pthread)와 타이머 시그널로 정밀하게 시뮬레이션한다.
* **지원 운영체제**:
  * **Linux (Ubuntu, Debian, Fedora 등)**: 가장 추천하는 네이티브 환경
  * **macOS**: Apple Silicon 및 Intel Mac 모두 지원
  * **Windows**: WSL2 (Ubuntu 등 Windows Subsystem for Linux) 환경 권장

---

## 2. 필수 도구 점검 및 설치

실습을 진행하기 위해 다음 4가지 도구가 필요하다. 터미널을 열고 아래 명령어를 입력하여 각 도구의 설치 여부와 버전을 확인한다.

### 2.1 도구 설치 확인 명령어

```bash
# 1. C 컴파일러 확인 (GCC 또는 Clang)
gcc --version

# 2. CMake 확인 (3.15.0 이상 필요)
cmake --version

# 3. 빌드 자동화 도구 확인 (GNU Make)
make --version

# 4. Git 확인
git --version
```

### 2.2 도구가 설치되어 있지 않은 경우 설치 방법

* **Ubuntu / Debian (WSL2 포함)**:
  ```bash
  sudo apt update
  sudo apt install -y build-essential cmake git
  ```
* **macOS (Homebrew 사용)**:
  ```bash
  brew install cmake gcc
  ```
  *(macOS 기본 제공 `clang` 및 `make`로도 빌드 가능)*

---

## 3. 저장소 복제 및 서브모듈(커널 소스) 준비

본 저장소는 FreeRTOS 커널의 공식 소스 코드를 Git 서브모듈(`source/FreeRTOS-Kernel`)로 참조한다. 따라서 서브모듈을 반드시 함께 초기화해야 빌드 시 커널 헤더 및 소스를 찾을 수 있다.

```bash
# 1. 저장소를 클론한 디렉터리로 이동
cd FreeRTOS-Tutorials-ko

# 2. 서브모듈 초기화 및 최신 소스 다운로드
git submodule update --init --recursive
```

> **확인 요령**:  
> `ls -la source/FreeRTOS-Kernel/` 명령을 실행했을 때 `tasks.c`, `queue.c`, `include/` 등의 파일이 보여야 정상이다. 만약 디렉터리가 비어 있다면 위 서브모듈 초기화 명령을 다시 실행해야 한다.

---

## 4. 디렉터리 구조 및 파일 역할

실습을 진행할 때 학습자가 다루게 될 주요 파일과 폴더의 구조는 다음과 같다.

```text
FreeRTOS-Tutorials-ko/
├── source/
│   ├── CMakeLists.txt                 # 전체 프로젝트 빌드 스크립트 (-DTUTORIAL=N 처리)
│   ├── FreeRTOS-Kernel/               # FreeRTOS 커널 원천 소스 (서브모듈)
│   └── tutorials/
│       ├── tutorial_1/
│       │   ├── include/FreeRTOSConfig.h # 해당 단원의 커널 설정 (스케줄링, tick rate 등)
│       │   └── source/tutorial_1.c      # 해당 단원의 C 예제 소스 (TODO 주석 포함)
│       ├── tutorial_2/
│       │   ├── include/FreeRTOSConfig.h
│       │   └── source/tutorial_2.c
│       └── ... (tutorial_21까지 동일 구조)
└── ko/
    ├── docs/                          # 공식 영문 튜토리얼 1:1 완역 정본
    └── guides/                        # 자습용 단계별 해설서 (현재 보고 있는 가이드)
        ├── README.md                  # 전체 로드맵 및 단원 목차
        ├── getting-started.md         # 공통 실습 환경 가이드
        ├── tutorial_1.md ~ 21.md      # 단원별 11단계 상세 가이드
        └── solutions/                 # TODO가 있는 단원의 완성형 참조 코드
```

* **학습자가 수정할 파일**: `source/tutorials/tutorial_N/source/tutorial_N.c`  
  (각 단원 안내에 따라 `TODO` 주석이 달린 빈칸을 채운다.)
* **참조 및 설정 파일**: `source/tutorials/tutorial_N/include/FreeRTOSConfig.h`  
  (이 파일은 커널 옵션을 확인하는 읽기 전용으로 활용하며, 가이드의 안내가 없는 한 직접 수정하지 않는다.)
* **참조 해답 파일**: `ko/guides/solutions/tutorial_N.c`  
  (고민 후 자신의 코드와 비교하거나 복구할 때 참고하는 완성 코드이다.)

---

## 5. 표준 빌드 및 실행 절차

CMake 스크립트는 `-DTUTORIAL=<번호>` 옵션을 통해 빌드할 튜토리얼 단원을 지정하도록 설계되어 있다.

### 5.1 단원 빌드 명령어

저장소 루트(`FreeRTOS-Tutorials-ko/`)에서 터미널을 열고 다음 순서로 입력한다 (아래는 Tutorial 2 실습 예시):

```bash
# 1. 빌드 결과물이 저장될 build 디렉터리 생성 및 빌드 설정 구성
cmake -B build -S source -DTUTORIAL=2

# 2. 컴파일 및 실행 바이너리 생성
cmake --build build
```

> **단원 전환 시 주의사항 (빌드 캐시 분리)**:  
> 이전 단원(예: T02)을 빌드한 후 다른 단원(예: T03)으로 넘어갈 때는 CMake 캐시 충돌을 방지하기 위해 `rm -rf build`로 기존 빌드 폴더를 깨끗이 지우고 다시 `cmake -B build ...`를 실행하거나, 단원별로 빌드 폴더를 분리(`cmake -B build_t02 ...`, `cmake -B build_t03 ...`)하는 것이 안전하다.

### 5.2 바이너리 실행

빌드가 성공하면 `build/` 디렉터리 안에 `freertos_example` 실행 파일이 생성된다.

```bash
./build/freertos_example
```

### 5.3 정상 종료 방법

FreeRTOS의 task는 대부분 실시간 제어를 위해 `for(;;)` 무한 루프로 동작한다. 따라서 프로그램이 자동으로 종료되지 않고 계속 로그를 출력하는 것은 정상적인 동작이다.

* **실행 중지**: 터미널에서 **`Ctrl + C`**를 누르면 시뮬레이터 프로세스가 즉시 안전하게 종료된다.
* 특정 단원(예: T01 등)은 일정 시간 동작 후 호스트 프로세스를 종료하도록 작성되어 있어 스스로 끝날 수도 있다.

---

## 6. 자주 발생하는 공통 문제 해결 (Troubleshooting)

### Q1. `cmake` 실행 시 `fatal error: FreeRTOS.h: No such file or directory` 에러가 발생합니다.
* **원인**: 커널 서브모듈이 다운로드되지 않아 헤더 파일을 찾지 못하는 상태다.
* **해결**: 저장소 루트에서 `git submodule update --init --recursive` 명령을 실행하여 `source/FreeRTOS-Kernel` 디렉터리에 파일들이 채워졌는지 확인한다.

### Q2. 이전 단원의 출력이 그대로 나오거나 소스 코드를 고쳤는데 반영이 안 됩니다.
* **원인**: CMake 빌드 캐시(`CMakeCache.txt`)에 이전 단원 번호가 남아있거나 컴파일 대상이 갱신되지 않은 경우다.
* **해결**: 빌드 디렉터리를 완전히 삭제한 후 다시 빌드한다:
  ```bash
  rm -rf build
  cmake -B build -S source -DTUTORIAL=<원하는번호>
  cmake --build build
  ```

### Q3. `printf()` 출력이 터미널에 바로바로 찍히지 않고 멈춰 있는 것처럼 보입니다.
* **원인**: C 표준 입출력 라이브러리는 개행 문자(`\n`)가 없거나 버퍼가 꽉 차지 않으면 출력을 즉시 터미널로 방출하지 않는다.
* **해결**: 예제 코드에는 버퍼를 강제로 비우는 `fflush(stdout);`이 포함되어 있다. 본인이 새로운 로그 코드를 추가할 때도 출력 문자열 끝에 `\n`을 붙이거나 `fflush(stdout);`을 호출해 준다.

---

환경 준비가 완료되었다면 이제 **[T01: 베어메탈 프로젝트에 FreeRTOS 추가](tutorial_1.md)**부터 본격적인 학습을 시작해 보자!
