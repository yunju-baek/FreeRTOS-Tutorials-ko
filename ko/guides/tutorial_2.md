# Tutorial 2: task 생성 (Task Creation)

[학습 목차](README.md) · [공통 준비](getting-started.md) · [공식 튜토리얼 번역](../docs/tutorial_2/README.md) · [이전 단원](tutorial_1.md)

이 단원에서는 FreeRTOS의 가장 기본적인 실행 단위인 **task(태스크)**를 동적으로 생성하고, 커널의 **scheduler(스케줄러)**를 구동하여 task가 독립적으로 실행되는 과정을 직접 구현한다.

---

## 1. 이번 실습에서 할 일

* **해결하려는 문제**: 단일 루프(`main`) 구조를 벗어나, 운영체제가 관리하는 독립된 실행 흐름인 task를 생성하고 스케줄러에게 CPU 제어권을 넘긴다.
* **학습 목표**:
  1. FreeRTOS task 함수의 프로토타입 규칙과 무한 루프 구조를 설명할 수 있다.
  2. `xTaskCreate()` API의 6가지 매개변수와 반환값(`pdPASS`/`errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY`)의 의미를 이해하고 올바르게 호출한다.
  3. `vTaskStartScheduler()`를 호출하여 커널 멀티태스킹을 시작하고, `vTaskDelay()`를 통한 상태 전이(Running → Blocked)를 관찰한다.

---

## 2. 시작 전 확인

### 2.1 C 언어 핵심 복습
* **함수 포인터 (Function Pointer)**: 함수도 메모리의 특정 주소(코드 영역)에 위치한다. 함수의 이름은 곧 그 함수가 시작되는 메모리 주소를 뜻한다. `xTaskCreate()`의 첫 번째 인자로 함수 이름(`prvTaskFunction`)을 넘기는 것은 커널에게 "이 주소에서 task 코드를 시작하라"고 알려주는 함수 포인터 전달이다.
* **`void *` 포인터 (Generic Pointer)**: 어떠한 데이터 타입의 주소라도 가리킬 수 있는 만능 포인터다. task 생성 시 매개변수나 인자로 넘길 데이터가 없다면 `NULL`을 전달한다.

### 2.2 확인할 소스 및 설정 파일
* 실습 코드: [`source/tutorials/tutorial_2/source/tutorial_2.c`](../../source/tutorials/tutorial_2/source/tutorial_2.c)
* 설정 파일: [`source/tutorials/tutorial_2/include/FreeRTOSConfig.h`](../../source/tutorials/tutorial_2/include/FreeRTOSConfig.h)
* 참조 해답: [`ko/guides/solutions/tutorial_2.c`](solutions/tutorial_2.c)

> **실습 형태**: 이 단원은 `tutorial_2.c` 파일 내부의 **`TODO 1`과 `TODO 2`를 직접 완성하는 구현 실습**이다.

---

## 3. 개념 이해

### 3.1 task(태스크)란 무엇인가?
일반 C 프로그램에서는 `main()` 함수에서 호출된 함수가 작업을 마치고 `return`해야 다음 줄이 실행된다. 하지만 RTOS의 **task**는 마치 자신 혼자만 CPU를 독점해서 사용하는 것처럼 동작하는 **독립적인 소형 프로그램(스레드)**이다.

**Running**은 현재 실행 중, **Ready**는 실행 가능하지만 선택을 기다리는 상태, **Blocked**는 시간이나 이벤트를 기다리는 상태다. 이 단일 코어 모델에서는 한 번에 하나의 task만 Running이다. 대기 조건이 풀리면 먼저 Ready가 되고, 스케줄러가 선택해야 Running이 된다.

각 task는 실행을 중단했다가 이어가기 위해 다음 두 요소를 독립적으로 갖는다:
1. **스택 (Stack)**: 해당 task 내부에서 선언된 지역 변수, 함수 호출 기록(복귀 주소), CPU 레지스터 값을 임시 저장하는 메모리 공간.
2. **TCB (Task Control Block)**: task의 이름, 우선순위, 현재 상태(Ready, Blocked 등), 스택 포인터의 위치 등 커널이 task를 관리하기 위한 메타데이터 구조체.

다음은 일반적인 MCU의 task별 TCB·스택 관계를 단순화한 개념도다. 이 POSIX 실습의 실제 실행 문맥은 호스트 스레드로 구현되므로 배열 안에 모든 호스트 레지스터·지역 변수가 저장된다고 해석하지 않는다.

```mermaid
flowchart LR
    subgraph RAM["FreeRTOS 힙 메모리 (Heap)"]
        subgraph T1["Task 1 전용 메모리"]
            TCB1["TCB (제어 블록)<br/>• 우선순위: 0<br/>• 상태: Ready"]
            STK1["Stack (독립 스택)<br/>• 지역 변수<br/>• 레지스터 저장소"]
        end
        subgraph T2["Task 2 전용 메모리"]
            TCB2["TCB (제어 블록)<br/>• 우선순위: 1<br/>• 상태: Running"]
            STK2["Stack (독립 스택)<br/>• 지역 변수<br/>• 레지스터 저장소"]
        end
    end
```

### 3.2 task 함수가 절대 return하면 안 되는 이유
FreeRTOS task 함수는 반드시 다음과 같은 표준 형태를 가져야 한다:

```c
void TaskFunction( void *pvParameters )
{
    /* 1. 작업 초기화 (1회 실행) */

    /* 2. 무한 루프 내에서 작업 반복 */
    for( ;; )
    {
        /* 실제 task 로직 */
        vTaskDelay( pdMS_TO_TICKS( 1000 ) ); /* task를 대기 상태로 전환 */
    }

    /* 절대 루프를 빠져나와 함수 끝에 도달하면 안 됨! */
    /* 만약 task를 종료해야 한다면 반드시 vTaskDelete( NULL ); 을 호출해야 함 */
}
```

task 함수에서 일반 함수처럼 반환하는 것은 FreeRTOS의 사용 계약에 맞지 않는다. 이 POSIX 포트는 task가 반환하면 assertion으로 중단한다. 실패 형태는 포트마다 다르므로 항상 HardFault가 발생한다고 단정할 수 없다. 반복 작업은 무한 루프로 작성하고, 종료해야 한다면 삭제 기능을 활성화한 뒤 `vTaskDelete(NULL)`을 사용한다.

---

## 4. 실행 흐름 읽기

`main()` 함수가 시작되어 task가 생성되고 스케줄러가 구동되는 전체 타임라인은 다음과 같다:

```mermaid
sequenceDiagram
    autonumber
    participant Main as main() 함수
    participant Kernel as FreeRTOS 커널 (Scheduler)
    participant Task as prvTaskFunction

    Main->>Kernel: xTaskCreate() 호출 (TODO 1)
    Kernel->>Kernel: 힙에서 TCB 및 스택 할당 후 Ready 리스트에 등록
    Kernel-->>Main: pdPASS 반환
    Main->>Kernel: vTaskStartScheduler() 호출 (TODO 2)
    Note over Main: 이 예제의 정상 실행에서는 main으로 복귀하지 않음

    Kernel->>Task: 최초 문맥 전환 (Task 실행 시작)
    loop 매 1초마다 반복
        Task->>Task: "Tutorial 2 running..." 출력
        Task->>Kernel: vTaskDelay(1000ms) 호출
        Note over Task,Kernel: Task는 Running → Blocked 상태로 전환<br/>커널은 CPU를 Idle Task 등에 양보
        Kernel-->>Task: 지연 tick 경과로 Ready 전환, 스케줄러가 선택하면 실행
    end
```

---

## 5. 코드와 API 이해

### 5.1 `xTaskCreate()` API 상세 분석
task를 동적으로 생성하는 가장 핵심적인 함수다:

```c
BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                       const char * const pcName,
                       const configSTACK_DEPTH_TYPE usStackDepth,
                       void * const pvParameters,
                       UBaseType_t uxPriority,
                       TaskHandle_t * const pxCreatedTask );
```

| 매개변수 | 예제에서 사용한 값 | 상세 의미 및 설정 이유 |
| :--- | :--- | :--- |
| `pxTaskCode` | `prvTaskFunction` | task로 실행할 C 함수의 시작 주소 (함수 포인터). |
| `pcName` | `"Task"` | 디버깅 및 트레이스 시 식별할 문자열 이름 (커널 동작에는 영향 없음). |
| `usStackDepth` | `configMINIMAL_STACK_SIZE` | 스택 요소 수. 메모리 크기는 `usStackDepth * sizeof(StackType_t)`다. 현재 64비트 POSIX 환경의 요소는 8바이트이며 호스트 스레드의 최소 스택 조건은 별도로 적용된다. |
| `pvParameters` | `NULL` | task 함수에 넘겨줄 인자 포인터. 이번 실습에서는 전달할 데이터가 없으므로 `NULL`. |
| `uxPriority` | `tskIDLE_PRIORITY` | task의 우선순위. `0`은 유휴(Idle) task와 동일한 최하위 우선순위임. |
| `pxCreatedTask` | `NULL` | 생성된 task를 추후 제어(삭제, 우선순위 변경 등)하기 위한 핸들(포인터)을 받을 변수. 제어가 필요 없으면 `NULL`. |

* **반환값**:
  * `pdPASS` (1): task 생성 성공 (힙 메모리 할당 완료).
  * `errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY` (-1): 힙 메모리가 부족하여 TCB나 스택을 할당하지 못함.

### 5.2 `vTaskStartScheduler()` API
* 커널 scheduler를 시작한다. 내부적으로 시스템 틱(Tick) 타이머 인터럽트를 켜고, 생성된 task 중 가장 우선순위가 높은 task의 문맥을 복원하여 실행을 시작한다.
* **중요한 특성**: 이 예제는 스케줄러를 종료하지 않으므로 정상 실행 중에는 다음 줄로 돌아오지 않는다. 시작 실패나 스케줄러 종료 지원 여부는 포트의 동작과 구분한다.

### 5.3 `vTaskDelay()`와 `pdMS_TO_TICKS()`
* `vTaskDelay( xTicksToDelay )`: 양수 인자를 주면 호출한 task를 지정한 tick 동안 Blocked로 만든다. 그동안 다른 Ready task가 실행될 수 있다. 지연이 끝나도 즉시 실행을 보장하지 않으며, `0`은 대기 시간 없이 실행권을 양보하는 경우다.
* `pdMS_TO_TICKS( ms )`: 밀리초(ms) 단위의 시간을 커널 tick 수로 변환해 주는 매크로다. 시스템의 `configTICK_RATE_HZ`가 1000(1초에 1000틱 = 1틱당 1ms)인 경우 `pdMS_TO_TICKS(1000)`은 1000 tick이 된다.

---

## 6. 직접 구현 또는 관찰 준비

전체 참조 해답은 [tutorial_2.c](solutions/tutorial_2.c)에 있다. 먼저 TODO별 힌트를 따라 작성하고, [자기 코드 보존·해답 적용·복구 절차](getting-started.md#solution-workflow)에 따라 비교한다.

`source/tutorials/tutorial_2/source/tutorial_2.c` 파일을 열고 두 개의 TODO를 채워 넣는다.

### 6.1 TODO 1: `xTaskCreate` 호출하기
`tutorial_2.c`의 46~58행 주석 부분을 찾는다:


* **TODO 1 힌트 1**: task를 생성해도 스케줄러가 시작되기 전에는 실행되지 않는다. 생성 결과를 먼저 검사한다.
* **TODO 1 힌트 2**: 기존 함수 `prvTaskFunction`과 준비된 스택 상수를 사용하고 결과를 `xTaskCreationResult`에 저장한다. 출력용 핸들은 필요 없다.

<details>
<summary>TODO 1 해답 코드 보기</summary>

```c
    xTaskCreationResult = xTaskCreate( prvTaskFunction,
                                       "Task",
                                       configMINIMAL_STACK_SIZE,
                                       NULL,
                                       tskIDLE_PRIORITY,
                                       NULL );
```

**이유**: task를 생성해도 스케줄러가 시작되기 전에는 실행되지 않는다. 생성 결과를 먼저 검사한다.
</details>

### 6.2 TODO 2: `vTaskStartScheduler` 호출하기
`tutorial_2.c`의 62행 주석 부분을 찾는다:


* **TODO 2 힌트 1**: 생성된 task를 실행 대상으로 선택하는 스케줄러를 시작한다.
* **TODO 2 힌트 2**: 생성 결과 검사 뒤, 끝의 무한 루프 앞에 인자 없는 `vTaskStartScheduler` 호출을 둔다.

<details>
<summary>TODO 2 해답 코드 보기</summary>

```c
    vTaskStartScheduler();
```

**이유**: 생성된 task를 실행 대상으로 선택하는 스케줄러를 시작한다.
</details>

> 작성한 내용이 맞는지 전체 소스를 비교하고 싶다면 [`ko/guides/solutions/tutorial_2.c`](solutions/tutorial_2.c)를 열어 확인할 수 있다.

---

## 7. 빌드와 실행

[공통 준비](getting-started.md)의 학습용 사본 루트에서 다음 명령을 실행하여 빌드하고 실행한다:

```bash
# 1. CMake 빌드 구성 (Tutorial 2 타깃 지정)
cmake -S source -B build/tutorial_2 -DTUTORIAL=2

# 2. 컴파일 및 빌드
cmake --build build/tutorial_2

# 3. 실행
./build/tutorial_2/freertos_example
```

* **종료 방법**: 터미널에서 **`Ctrl + C`**를 누른다.

---

## 8. 결과 해석

### 8.1 출력 예시

task의 반복 문자열을 확인한다. 1000ms 지연은 출력 간격을 정확히 고정하지 않는다.

```text
Tutorial 2 running...
Tutorial 2 running...
Tutorial 2 running...
Tutorial 2 running...
```

### 8.2 출력 해석 및 판정 기준
* **반드시 만족할 조건**: 약 1초(`vTaskDelay(pdMS_TO_TICKS(1000))`) 간격으로 `Tutorial 2 running...` 로그가 계속해서 출력되어야 한다.
* **동작 검증**: 이 문자열은 `main()` 함수가 출력한 것이 아니라, 우리가 생성한 `prvTaskFunction` task가 스케줄러에 의해 선택되어 출력한 것이다.
* **실패 증상**:
  * 프로그램이 아무것도 출력하지 않고 즉시 멈추거나 종료된다면 `TODO 2`의 스케줄러 시작 호출이 누락되었을 가능성이 높다.
  * 이 프로젝트의 `configASSERT`는 오류 메시지를 출력하지 않고 멈춘다. 정지했다고 바로 힙 부족으로 단정하지 말고 TODO 대입문과 생성 결과, 공통 준비의 POSIX 스택 진단을 확인한다.

---

## 9. 막혔을 때 (트러블슈팅)

| 증상 | 원인 | 해결 방법 |
| :--- | :--- | :--- |
| 출력 없이 멈춤 | TODO 누락, 생성 실패 또는 assertion 가능성 | `xTaskCreationResult` 대입·값과 인자를 확인하고 공통 준비의 호스트 스택 오류 항목을 확인 |
| 아무 출력이 없거나 실행이 끝남 | 스케줄러 호출 누락 또는 호스트 오류 가능성 | TODO 2를 확인하고 stderr를 읽는다. 원본에는 끝의 무한 루프가 있으므로 호출 누락만으로 정상 종료한다고 보지 않음 |
| 컴파일 에러: `undefined reference to prvTaskFunction` | 함수명 철자 오타 | 첫 번째 인자가 `prvTaskFunction`과 대소문자까지 정확히 일치하는지 확인 |

---

## 10. 혼자 확인하기 (자가 점검)

### 질문 1 (개념)
`xTaskCreate()`의 세 번째 인자인 `usStackDepth`에 숫자 `1024`를 넘기면 이 task에 할당되는 스택 메모리는 총 1024바이트인가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: 아니다. `1024 * sizeof(StackType_t)`바이트다.
* **이유**: 현재 64비트 POSIX 포트의 `StackType_t`는 8바이트이므로 8192바이트다. 요소가 4바이트인 MCU 포트라면 4096바이트다. 이 계산은 FreeRTOS 스택 배열의 크기이며 TCB와 호스트 스레드 관리 메모리는 별도다.
</details>

### 질문 2 (실행 추적)
`tutorial_2.c` 코드 64~68행에는 다음과 같은 코드가 있다:
```c
vTaskStartScheduler();

/* Should not reach here. */
for( ;; )
{
}
```
`vTaskStartScheduler()` 아래에 있는 이 `for(;;)` 루프는 정상적인 실행 상황에서 실행되는가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: 이 예제의 정상 실행에서는 실행되지 않는다.
* **이유**: 스케줄러가 task 실행을 계속 관리하며 예제는 스케줄러 종료를 요청하지 않는다. MCU의 스택 전환 방식과 POSIX의 스레드 기반 구현을 동일한 기계어 동작으로 설명해서는 안 된다.
</details>

### 질문 3 (비교)
Tutorial 1의 `sleep(1)`과 Tutorial 2의 `vTaskDelay(pdMS_TO_TICKS(1000))`의 가장 결정적인 시스템적 차이는 무엇인가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: 대기를 관리하는 스케줄러가 다르다.
* `sleep(1)`은 호출한 호스트 스레드를 대기시킨다. `vTaskDelay()`는 FreeRTOS task를 Blocked로 만들어 커널이 다른 Ready task를 선택하게 한다. 둘 다 바쁜 대기 루프와는 다르다.
</details>

### 질문 4 (조건 변경과 결과 예측)
`prvTaskFunction`의 지연값을 1000ms에서 200ms로 바꾼다.

실행 전에 어떤 결과가 달라지고 어떤 동작이 그대로 유지될지 적는다. 직접 작성한 코드와 수정 전 파일은 [공통 보존·복구 절차](getting-started.md#solution-workflow)에 따라 보관한다. 위에서 지정한 조건 하나만 바꾸고 `cmake --build build/tutorial_2` 명령으로 다시 빌드한다. 실행 결과를 예측과 비교한 뒤 아래 해설을 확인한다. 해설은 예상 결과를 설명한 것이므로 직접 얻은 로그와 구분한다.

<details>
<summary>예상 결과와 해설 확인</summary>

출력이 더 자주 나타난다. 지연 뒤 Ready가 되는 것과 곧바로 Running이 되는 것은 다르므로 정확히 초당 5줄을 조건으로 삼지 않는다.
</details>

**복구와 재확인**: `prvTaskFunction`의 지연 시간을 200ms에서 1000ms로 되돌린다. 같은 단원을 다시 빌드하고 실행하여 [8절의 결과](#8-결과-해석)와 비교한다. 결과를 설명하기 어려우면 [관련 개념 다시 읽기](#5-코드와-api-이해)로 돌아간다.

---

## 11. 다음 학습

`xTaskCreate()`는 매우 편리하지만, 힙 메모리를 동적으로 할당하므로 임베디드 안전 필수(Safety-critical) 시스템에서는 힙 고갈이나 메모리 단편화(Fragmentation) 위험이 따를 수 있다.

이제 **[Tutorial 3: 정적 메모리 할당 (Static Allocation)](tutorial_3.md)**으로 넘어가서, task의 TCB와 스택 버퍼를 직접 제공하는 정적 할당 기법을 배운다.

* **한국어 커널북 심화 읽기**:
  * [4.2장: task 함수 형태](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch04.md)
  * [4.4장: task 생성 (xTaskCreate)](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch04.md)
