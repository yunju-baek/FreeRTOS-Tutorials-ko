# 공통 실습 환경 준비

이 과정은 PC의 POSIX port에서 FreeRTOS 예제를 실행한다. MCU 보드 없이 task의 스케줄링·통신·동기화를 관찰할 수 있다. 호스트 운영체제가 pthread와 신호를 처리하므로, 출력 간격은 실제 보드의 실시간 성능을 측정한 값으로 해석하지 않는다.

## 1. 사용할 환경

다음 두 환경에서 T01–T21의 기본 예제를 빌드하고, 반복 출력과 주요 동작을 관찰한 뒤 `Ctrl+C`에 해당하는 SIGINT로 종료했다. TODO가 있는 단원에는 참조 해답을 적용했다.

| 환경 | 도구 | 실행 조건 |
| --- | --- | --- |
| macOS, Apple Silicon | Apple Clang 21, CMake | 저장소의 원래 설정으로 실행 |
| Debian 12, Linux ARM64 | GCC 12.2, CMake 3.25 | 아래 3절의 stack 깊이 호환 패치 적용 |

다른 Linux 배포판·Intel Mac·WSL2는 POSIX 환경의 후보지만, 이 안내에서 모두 실행을 확인한 환경은 아니다. 먼저 T01과 T02를 실행해 도구와 task 생성이 동작하는지 확인한다. Windows에서는 Linux 환경을 준비한 뒤 Linux용 명령을 사용한다.

### 필요한 도구 확인

```sh
cc --version
cmake --version
make --version
git --version
```

CMake는 3.15 이상이 필요하다. 위 명령어 네 개가 모두 버전 정보를 출력하면 다음 단계로 진행한다. 명령을 찾지 못하면 해당 도구부터 설치한다.

macOS에서는 Apple Command Line Tools와 CMake를 사용한다. Command Line Tools가 없으면 `xcode-select --install`로 설치한다. Homebrew를 사용하는 경우 `brew install cmake`로 CMake를 설치할 수 있다. Apple Clang으로 실행했으므로 별도 GNU GCC 설치는 필수가 아니다.

Debian/Ubuntu 계열에서는 다음 패키지를 설치한다.

```sh
sudo apt update
sudo apt install build-essential cmake git
```

## 2. 실습용 저장소 받기

자신의 연습 코드를 수정할 별도 폴더에서 다음 명령을 실행한다. 기존 작업 폴더가 있으면 다른 이름을 사용한다.

```sh
git clone --recurse-submodules https://github.com/yunju-baek/FreeRTOS-Tutorials-ko.git FreeRTOS-Tutorials-practice
cd FreeRTOS-Tutorials-practice
git rev-parse --show-toplevel
git submodule status --recursive
ls source/FreeRTOS-Kernel/tasks.c
```

이후 명령의 **저장소 루트**는 방금 들어온 `FreeRTOS-Tutorials-practice/`다. `source/`, `ko/`가 보이는 위치에서 시작한다. 커널은 submodule이므로 폴더 이름만 있고 내용이 비어 있으면 빌드할 수 없다.

이미 clone했지만 커널 파일이 없으면 저장소 루트에서 다음을 실행한다. 저장소가 지정한 commit을 내려받는 명령이며, 임의의 최신 커널로 갱신하는 명령은 아니다.

```sh
git submodule update --init --recursive
```

## 3. Linux ARM64의 stack 깊이 확인

일부 Linux ARM64 환경에서는 원문의 `configMINIMAL_STACK_SIZE`가 `PTHREAD_STACK_MIN`을 `unsigned short`로 변환하며 값이 잘린다. 확인한 환경에서는 `PTHREAD_STACK_MIN`이 131072였고 변환 결과가 0이었다. 이 경우 T01은 동작해도 task를 만드는 T02부터 `pthread_create: Invalid argument`로 종료될 수 있다.

이 환경에서는 **자신의 실습용 clone**에 제공된 호환 패치를 한 번 적용한다. 패치는 각 단원의 설정에서 해당 형 변환만 `size_t`로 바꾼다. 커널 소스와 예제의 학습 동작은 그대로다.

```sh
git apply --check ko/guides/patches/posix-stack-depth.patch
git apply ko/guides/patches/posix-stack-depth.patch
git diff -- source/tutorials/tutorial_2/include/FreeRTOSConfig.h
```

마지막 명령의 출력에서 형 변환 부분이 `unsigned short`에서 `size_t`로 바뀌었는지 확인한다. 이미 적용했거나 설정을 직접 수정했다면 `--check`가 실패할 수 있다. 이때는 강제 적용하지 말고 `git diff`로 현재 변경을 확인한다. 패치 없이 실행되는 환경에서는 이 단계를 건너뛴다.

여기서 stack 깊이는 `StackType_t`의 원소 수다. 이 POSIX port는 `StackType_t`를 `unsigned long`으로 정의하며, 위 64비트 환경에서는 원소 하나가 8바이트다. 이 값을 MCU의 바이트 단위 stack 크기로 그대로 옮기지 않는다.

## 4. 파일의 역할과 학습 순서

| 위치 | 역할 |
| --- | --- |
| `source/CMakeLists.txt` | `-DTUTORIAL=N`으로 단원을 선택하고 POSIX port와 함께 빌드 |
| `source/tutorials/tutorial_N/source/tutorial_N.c` | 자신이 읽거나 TODO를 채울 C 파일 |
| `source/tutorials/tutorial_N/include/FreeRTOSConfig.h` | 해당 단원의 우선순위·tick·hook 등 설정 |
| `ko/docs/tutorial_N/README.md` | 공식 튜토리얼의 한국어 번역 |
| `ko/guides/tutorial_N.md` | 개념·코드·실행 결과를 연결한 자습 해설 |
| `ko/guides/solutions/tutorial_N.c` | TODO가 있는 단원의 전체 참조 해답 |

T01·T06·T07·T08·T14·T21은 TODO가 없는 관찰 실습이다. 다른 단원은 먼저 TODO를 채운다. 미완성 코드가 컴파일되더라도 동작한다는 뜻은 아니다. 생성 결과를 검사하는 `configASSERT`에서 멈추거나, 아직 설정하지 않은 값으로 잘못 동작할 수 있다.

### task의 세 가지 실행 상태

- **Running**: 현재 실행하는 task다.
- **Ready**: 실행할 준비가 되어 있고 scheduler의 선택을 기다린다.
- **Blocked**: 시간 또는 이벤트를 기다리며 실행 대상으로 선택되지 않는 상태다.

대기가 끝나 Ready가 된 task가 곧바로 Running이 되는지는 우선순위와 스케줄링 조건에 달려 있다. `vTaskDelay()`나 수신 대기는 단순한 빈 반복문과 다르게 task를 Blocked 상태로 옮길 수 있다. 본 예제의 설정에서는 `INCLUDE_vTaskSuspend`가 1이므로, 해당 blocking API의 `portMAX_DELAY`를 무기한 대기에 사용한다. 다른 설정·API에 이 설명을 일괄 적용하지 않는다.

<a id="solution-workflow"></a>
## 5. 작성한 코드 보존과 해답 비교

다음은 T02 예시다. 다른 단원은 실제 파일의 번호를 바꾼다. 먼저 자신이 작성한 코드를 보존한다. 백업 파일이 이미 있으면 덮어쓰지 않고 다른 이름을 선택한다.

```sh
mkdir -p practice-backups
cp -n source/tutorials/tutorial_2/source/tutorial_2.c practice-backups/tutorial_2.c
diff -u source/tutorials/tutorial_2/source/tutorial_2.c ko/guides/solutions/tutorial_2.c
```

`diff`의 종료 코드 1은 두 파일에 차이가 있다는 뜻이다. TODO별 해설과 비교한 뒤 필요하면 자신의 실습 사본에만 참조 해답을 적용한다.

```sh
cp ko/guides/solutions/tutorial_2.c source/tutorials/tutorial_2/source/tutorial_2.c
```

자기 코드로 돌아가려면 다음 명령을 사용하고 다시 빌드한다.

```sh
cp practice-backups/tutorial_2.c source/tutorials/tutorial_2/source/tutorial_2.c
```

설정값을 바꾸는 탐구도 변경 전 파일을 같은 방식으로 보존한다. 학습 중 원래 코드와 설정의 차이는 `git diff -- source/tutorials/`로 확인할 수 있다.

## 6. 빌드·실행·종료

처음에는 TODO가 없는 T01로 환경을 확인한다. 저장소 루트에서 실행한다.

```sh
cmake -S source -B build/tutorial_1 -DTUTORIAL=1
cmake --build build/tutorial_1
./build/tutorial_1/freertos_example
```

`Tutorial 1 running...`이 반복되면 빌드와 기본 실행 경로가 연결된 것이다. T01은 아직 FreeRTOS task를 생성하지 않는다. 다음에는 T02의 TODO를 완성하거나 해답을 적용한 뒤 실행한다.

```sh
cmake -S source -B build/tutorial_2 -DTUTORIAL=2
cmake --build build/tutorial_2
./build/tutorial_2/freertos_example
```

`Tutorial 2 running...`의 반복으로 task와 scheduler가 동작하는지 확인한다. 단원마다 `build/tutorial_N/`을 사용하면 이전 단원의 실행 파일과 혼동을 줄일 수 있다. 코드를 고친 뒤에는 반드시 `cmake --build`를 다시 실행한다.

예제는 대체로 무한 루프이며 T01도 자동 종료하지 않는다. 몇 차례의 동작을 관찰한 뒤 `Ctrl+C`로 실습 프로세스를 중지한다. 이는 호스트 프로세스를 끝내는 방법이며, 실제 펌웨어에서 task와 자원을 정리하는 절차와는 구분한다.

출력은 주로 `fprintf(stderr, ...)`로 나온다. 로그를 저장하려면 stderr도 함께 받는다.

```sh
./build/tutorial_2/freertos_example > build/tutorial_2/run.log 2>&1
```

이때 화면에 출력이 없는 것은 리다이렉션 때문이다. 다른 터미널에서 같은 저장소 루트로 이동해 `tail -f build/tutorial_2/run.log`로 확인할 수 있다. 관찰 후 실행 터미널과 `tail` 터미널에서 각각 `Ctrl+C`를 누른다.

## 7. 증상으로 찾기

| 증상 | 확인할 위치·원인 후보 | 조치와 재확인 |
| --- | --- | --- |
| `cmake: command not found` | CMake 설치와 PATH | 1절을 따라 설치한 뒤 `cmake --version` 확인 |
| kernel 디렉터리·헤더를 찾지 못함 | 저장소 루트, submodule 내용, 실제 CMake 명령 | 2절의 초기화 후 `tasks.c` 존재를 확인하고 해당 단원을 다시 구성 |
| 다른 단원의 문구가 나옴 | `-DTUTORIAL=N`, 실행 파일 경로 | 해당 단원의 7절 명령으로 구성·빌드하고 같은 `build/tutorial_N`의 파일 실행 |
| T01은 되지만 T02부터 `pthread_create: Invalid argument` | 3절의 stack 깊이 형 변환, POSIX 환경 | 해당 조건이면 호환 패치 적용 후 다시 빌드; 계속 실패하면 도구·OS와 오류를 함께 확인 |
| 실행 직후 아무 출력 없이 멈춤 | TODO 누락, 실패한 생성 결과, `configASSERT` 위치 | 원문 TODO·해답과 비교. 이 설정의 assert는 메시지 출력 후 종료가 아니라 무한 루프이므로 마지막 호출과 반환값 확인 |
| 파일로 저장했는데 로그가 비어 있음 | stderr 수집 여부, 실제 실행 경로 | `2>&1`을 포함해 다시 수집하고 `run.log` 확인 |
| 출력 순서·간격이 예시와 조금 다름 | 해당 단원의 성공 조건과 허용 변동 | 값·관계·상태 변화가 맞는지 확인. 호스트의 출력 간격만으로 MCU 실시간 성능을 판단하지 않음 |

이후 [T01 자습 해설](tutorial_1.md)에서 첫 실행을 읽고, [학습 목차](README.md)를 따라 진행한다.
