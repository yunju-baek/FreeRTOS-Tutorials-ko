# Tutorial 10: 인터럽트와 task 간의 queue 통신 (Using Queues for Communication between Interrupts and Tasks)

[학습 목차](README.md) · [공통 준비](getting-started.md) · [공식 튜토리얼 번역](../docs/tutorial_10/README.md) · [이전 단원](tutorial_9.md)

이 단원에서는 하드웨어 인터럽트 서비스 루틴(ISR)에서 발생한 데이터를 task로 안전하게 전달하는 **`FromISR` 전용 API**와 **지연된 인터럽트 처리(Deferred Interrupt Processing)**의 기초를 실습한다.

---

## 1. 이번 실습에서 할 일

* **해결하려는 문제**: ISR에서는 task처럼 Blocked로 기다릴 수 없다. 큐 송신의 ISR 전용 API를 선택하고 호출 가능 조건을 지킨다.
* **학습 목표**:
  1. 일반 FreeRTOS API와 `...FromISR` 접미사가 붙은 ISR 전용 API의 차이를 명확히 설명할 수 있다.
  2. Tick Hook 인터럽트 핸들러에서 `xQueueSendFromISR()`을 호출하여 큐로 데이터를 전송한다.
  3. `pxHigherPriorityTaskWoken` 매개변수와 `portYIELD_FROM_ISR()` 매크로의 동작 원리를 이해한다.

---

## 2. 시작 전 확인

### 2.1 임베디드 인터럽트 핵심 복습
* **인터럽트 문맥 (Interrupt Context)**: 하드웨어 이벤트(타이머, UART 수신, GPIO 핀 변화 등)가 발생하면 CPU는 현재 실행 중이던 task를 즉시 멈추고 인터럽트 벡터로 점프하여 ISR을 실행한다.
* 이 실습의 tick hook은 POSIX 포트가 모사한 인터럽트 경로에서 실행된다. 실제 MCU의 인터럽트 우선순위·마스킹 규칙은 해당 포트 문서로 확인한다.

**ISR의 규칙**:
  1. **절대 잠들지 말 것 (Never Block)**: ISR은 자신만의 독립된 task 스택이나 TCB가 없으므로 `vTaskDelay()`를 부르거나 큐가 비워질 때까지 기다릴 수 없다.
  2. **최대한 빨리 끝낼 것 (Short and Fast)**: ISR이 길어지면 다른 중요한 하드웨어 인터럽트를 놓치거나 시스템 실시간성이 붕괴된다.

### 2.2 확인할 소스 및 설정 파일
* 실습 코드: [`source/tutorials/tutorial_10/source/tutorial_10.c`](../../source/tutorials/tutorial_10/source/tutorial_10.c)
* 설정 파일: [`source/tutorials/tutorial_10/include/FreeRTOSConfig.h`](../../source/tutorials/tutorial_10/include/FreeRTOSConfig.h)
  * `configUSE_TICK_HOOK`이 `1`로 켜져 있어 매 1ms tick 인터럽트마다 `vApplicationTickHook()` 함수가 호출된다.
* 참조 해답: [`ko/guides/solutions/tutorial_10.c`](solutions/tutorial_10.c)

> **실습 형태**: 이 단원은 `tutorial_10.c` 내부의 **`TODO 1`, `TODO 2`, `TODO 3`을 완성하는 구현 실습**이다.

---

## 3. 개념 이해

### 3.1 왜 큐 송신에 ISR 전용 API가 필요한가?
`xQueueSend()`의 3번째 인자는 `xTicksToWait`(대기 시간)이다. 만약 큐가 꽉 차 있으면 커널은 호출자를 Blocked 상태로 만들어 재우려고 시도한다.
하지만 **ISR은 운영체제의 스케줄링 대상(task)이 아니므로 재울 수 없다** task용 큐 API를 ISR에서 쓰면 커널 상태를 손상시키거나 assertion으로 멈출 수 있다. 모든 API를 이름만으로 판단하지 말고 ISR 사용 가능 여부를 각 API 계약에서 확인한다.
따라서 FreeRTOS는 ISR에서 호출할 수 있는 함수들을 별도로 분리하고 이름 끝에 **`FromISR`**을 붙여 두었다.

```mermaid
flowchart TD
    subgraph ISR["인터럽트 문맥 (vApplicationTickHook)"]
        I1["xQueueSendFromISR() 호출<br/>(대기 시간 인자 없음, 즉시 시도)"] --> I2["데이터를 큐에 넣음"]
        I2 --> I3["중단된 task보다 높은 우선순위가 Ready가 되었는가?<br/>그 조건일 때만 플래그가 pdTRUE"]
        I3 --> I4["portYIELD_FROM_ISR() 호출<br/>(플래그가 참일 때 포트에 전환 요청)"]
    end

    subgraph Task["태스크 문맥 (prvReceiverTask)"]
        T1["xQueueReceive(..., portMAX_DELAY)<br/>(데이터 올 때까지 Blocked 대기)"]
    end

    I2 -.큐 버퍼.-> T1
```

### 3.2 `pxHigherPriorityTaskWoken`과 조건부 전환

1. ISR 진입 때 플래그를 `pdFALSE`로 초기화한다. 큐 전송이 대기 task를 Ready로 만들 수 있다.
2. 그 task의 우선순위가 ISR에 의해 중단된 task보다 높을 때 API가 플래그를 `pdTRUE`로 설정한다. 단순히 큐에 값을 넣었다고 항상 참이 되지는 않는다.
3. `portYIELD_FROM_ISR( xHighPriorityTaskWoken )`로 조건부 전환을 요청한다. 이 과정에도 실행 시간이 들므로 지연이 0이 되는 것은 아니다.

이 예제의 Receiver는 우선순위 0으로 Idle task와 같다. 따라서 `pdTRUE`와 즉시 선점을 관찰의 필수 조건으로 삼지 않는다. Receiver는 Ready가 된 뒤 tick의 스케줄링 등으로 선택되어 실행될 수 있다.

---

## 4. 실행 흐름 읽기

```mermaid
sequenceDiagram
    autonumber
    participant HW as POSIX 포트의 Tick 처리
    participant ISR as vApplicationTickHook (ISR)
    participant Q as FreeRTOS Queue
    participant R as Receiver Task

    Note over R: Receiver는 큐가 비어 Blocked 상태로 대기 중
    HW->>ISR: 1000번째 Tick 인터럽트 발생
    ISR->>Q: xQueueSendFromISR(uxCounter) 호출 (TODO 3)
    Note over Q: Receiver를 Ready로 전환<br/>우선순위 조건에 따라 플래그 설정
    ISR->>HW: portYIELD_FROM_ISR() 호출
    Note over HW,R: 조건부 yield 요청과 tick 스케줄링을 거쳐<br/>Receiver가 선택되면 실행
    Q->>R: xQueueReceive() 성공
    R->>R: "Value received from the queue: 1000" 출력
    R->>Q: 다음 xQueueReceive() 호출 (다시 Blocked 대기)
```

---

## 5. 코드와 API 이해

### 5.1 `xQueueSendFromISR()` API 상세 분석
```c
BaseType_t xQueueSendFromISR( QueueHandle_t xQueue,
                              const void * pvItemToQueue,
                              BaseType_t * const pxHigherPriorityTaskWoken );
```
* 일반 `xQueueSend`와 달리 `xTicksToWait` 인자가 아예 없다 (기다리지 않고 즉시 시도).
* `pxHigherPriorityTaskWoken`: 이 큐 전송으로 인해 더 높은 우선순위 task가 깨어났는지 여부를 커널이 기록해 주는 출력용 포인터 변수다 (`&( xHighPriorityTaskWoken )`).
* **반환값**: 성공 시 `pdPASS`, 큐가 꽉 차서 실패하면 즉시 `errQUEUE_FULL`을 반환한다.

### 5.2 `portYIELD_FROM_ISR()` 매크로
* ISR의 맨 마지막에서 호출한다.
* 플래그가 `pdTRUE`이면 포트에 문맥 전환을 요청한다. 이 POSIX 포트의 매크로는 조건부 `vPortYield()`를 사용한다. Cortex-M의 PendSV 설명을 이 PC 실습에 그대로 적용하지 않는다.

---

## 6. 직접 구현 또는 관찰 준비

전체 참조 해답은 [tutorial_10.c](solutions/tutorial_10.c)에 있다. 먼저 TODO별 힌트를 따라 작성하고, [자기 코드 보존·해답 적용·복구 절차](getting-started.md#solution-workflow)에 따라 비교한다.

`source/tutorials/tutorial_10/source/tutorial_10.c` 파일을 열고 세 개의 TODO를 완성한다.

### 6.1 TODO 1: 큐 생성 (`main` 함수)
66~74행 주석: 크기 5, 항목 크기 `sizeof( UBaseType_t )`의 큐를 생성한다.

* **TODO 1 힌트 1**: ISR과 수신 task가 같은 큐를 사용하도록 먼저 객체를 만든다.
* **TODO 1 힌트 2**: `main`에서 길이 5, 항목 크기 `sizeof(UBaseType_t)`를 전달하고 핸들을 저장한다.

<details>
<summary>TODO 1 해답 코드 보기</summary>

```c
    xQueue = xQueueCreate( 5, sizeof( UBaseType_t ) );
```

**이유**: ISR과 수신 task가 같은 큐를 사용하도록 먼저 객체를 만든다.
</details>

### 6.2 TODO 2: task에서 큐 수신 (`prvReceiverTask`)
101~108행 주석: 큐에서 데이터를 무기한 대기하여 꺼낸다.

* **TODO 2 힌트 1**: task 문맥의 수신자는 데이터가 없을 때 Blocked로 기다릴 수 있다.
* **TODO 2 힌트 2**: `prvReceiverTask`에서 `uxReceivedValue`의 주소와 `portMAX_DELAY`를 사용한다.

<details>
<summary>TODO 2 해답 코드 보기</summary>

```c
        xQueueReceive( xQueue, &( uxReceivedValue ), portMAX_DELAY );
```

**이유**: task 문맥의 수신자는 데이터가 없을 때 Blocked로 기다릴 수 있다.
</details>

### 6.3 TODO 3: ISR에서 큐 송신 (`vApplicationTickHook`)
125~133행 주석 부분을 찾는다:
* **주의**: 일반 `xQueueSend`가 아니라 반드시 **`xQueueSendFromISR`**을 호출해야 한다

* **TODO 3 힌트 1**: tick hook에서는 대기할 수 없으므로 ISR용 송신 함수를 사용한다.
* **TODO 3 힌트 2**: `vApplicationTickHook`의 1000 tick 조건 안에서 카운터 주소와 `xHighPriorityTaskWoken` 주소를 넘긴다. 기존 yield 요청은 그대로 둔다.

<details>
<summary>TODO 3 해답 코드 보기</summary>

```c
        xQueueSendResult = xQueueSendFromISR( xQueue,
                                              &( uxCounter ),
                                              &( xHighPriorityTaskWoken ) );
```

**이유**: tick hook에서는 대기할 수 없으므로 ISR용 송신 함수를 사용한다.
</details>

> 전체 완성 코드는 [`ko/guides/solutions/tutorial_10.c`](solutions/tutorial_10.c)에서 확인할 수 있다.

---

## 7. 빌드와 실행

[공통 준비](getting-started.md)의 학습용 사본 루트에서 다음 명령을 실행한다:

```bash
# 1. CMake 빌드 구성 (Tutorial 10 지정)
cmake -S source -B build/tutorial_10 -DTUTORIAL=10

# 2. 컴파일 및 빌드
cmake --build build/tutorial_10

# 3. 바이너리 실행
./build/tutorial_10/freertos_example
```

* **종료 방법**: 터미널에서 **`Ctrl + C`**를 누른다.

---

## 8. 결과 해석

### 8.1 출력 예시

수신 값이 1000 tick 단위로 늘어나는지 확인한다. 정확한 벽시계 간격이나 yield 플래그의 참을 요구하지 않는다.

```text
Value received from the queue: 1000
Number of items in the queue: 0.
Value received from the queue: 2000
Number of items in the queue: 0.
Value received from the queue: 3000
Number of items in the queue: 0.
```

### 8.2 판정 기준 및 해석
* **결과의 특징**: tick hook 1000회마다 전송하므로 수신 값은 1000씩 증가한다.
* **원리 확인**: 설정상 명목 tick 간격은 1ms이며 `vApplicationTickHook`은 tick 처리 때 호출된다. `if( ( uxCounter % 1000 ) == 0 )` 조건은 1000 tick마다 참이 된다. 수신 값이 1000씩 늘어나는지를 확인한다. 호스트 벽시계 간격이나 ISR에서 출력까지의 지연이 정확히 고정된다는 뜻은 아니다.

---

## 9. 막혔을 때 (트러블슈팅)

| 증상 | 원인 | 해결 방법 |
| :--- | :--- | :--- |
| 컴파일 경고 또는 크래시 발생 | TODO 3에서 `xQueueSendFromISR` 대신 일반 `xQueueSend`를 호출했음 | 반드시 `xQueueSendFromISR`로 교체 |
| 출력이 1000배 빠르게 쏟아짐 | `% 1000 == 0` 조건을 누락했음 | 1000틱마다 송신하는 if 조건 블록 안에서 호출했는지 확인 |

---

## 10. 혼자 확인하기 (자가 점검)

### 질문 1 (핵심 규칙)
`xQueueSendFromISR()` 함수에는 일반 `xQueueSend()` 함수에 있는 `xTicksToWait` 매개변수가 왜 존재하지 않는가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: 인터럽트 서비스 루틴(ISR)은 절대 Blocked 상태(대기 상태)로 들어갈 수 없기 때문이다.
* **이유**: ISR은 커널이 Blocked로 전환할 수 있는 task가 아니므로 빈자리가 날 때까지 기다릴 수 없다. 즉시 쓸 수 없으면 에러를 내고 바로 리턴해야 한다.
</details>

### 질문 2 (지연된 인터럽트 처리 패턴)
실무에서 고속 통신(예: UART 패킷 수신) 인터럽트가 발생했을 때, 복잡한 데이터 파싱과 파일 저장을 ISR 내부에서 직접 하지 않고, 큐를 통해 별도의 task로 넘겨서 처리하는 설계 패턴을 무엇이라고 부르는가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: **인터럽트 처리의 지연 수행 (Deferred Interrupt Processing)**.
* **이유**: ISR은 최소한의 데이터만 큐에 넣고 즉시 복귀하여 다른 인터럽트를 놓치지 않게 하고, 시간이 오래 걸리는 무거운 연산은 일반 task가 CPU를 받아 여유롭게 처리하게 만들기 위함이다.
</details>

### 질문 3 (조건 변경과 결과 예측)
`vApplicationTickHook`의 조건에서 나머지 연산의 기준만 `% 1000`에서 `% 500`으로 바꾼다.

실행 전에 어떤 결과가 달라지고 어떤 동작이 그대로 유지될지 적는다. 직접 작성한 코드와 수정 전 파일은 [공통 보존·복구 절차](getting-started.md#solution-workflow)에 따라 보관한다. 위에서 지정한 조건 하나만 바꾸고 `cmake --build build/tutorial_10` 명령으로 다시 빌드한다. 실행 결과를 예측과 비교한 뒤 아래 해설을 확인한다. 해설은 예상 결과를 설명한 것이므로 직접 얻은 로그와 구분한다.

<details>
<summary>예상 결과와 해설 확인</summary>

수신 값이 500, 1000, 1500 순으로 증가할 것으로 예상한다. ISR용 API와 조건부 yield 플래그는 그대로이며 수신마다 플래그가 참일 필요는 없다.
</details>

**복구와 재확인**: `vApplicationTickHook`의 나머지 연산 조건을 `% 500`에서 `% 1000`으로 되돌린다. 같은 단원을 다시 빌드하고 실행하여 [8절의 결과](#8-결과-해석)와 비교한다. 결과를 설명하기 어려우면 [관련 개념 다시 읽기](#3-개념-이해)로 돌아간다.

---

## 11. 다음 학습

인터럽트 경로에서 데이터를 보내고 task에서 처리하는 방식을 살펴보았다.
이제 **[Tutorial 11: software timer](tutorial_11.md)**로 넘어가서, 하드웨어 타이머 개수가 부족할 때 메모리와 처리 여유 범위에서 여러 타이머를 관리하는 타이머 서비스 task와 콜백 함수를 배운다.

* **한국어 커널북 심화 읽기**:
  * [7.2장: FromISR API의 규칙](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch07.md)
  * [7.7장: 인터럽트와 큐 통신](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch07.md)
