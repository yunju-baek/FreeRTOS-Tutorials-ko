# Tutorial 9: queue를 통한 task 간 통신 (Using Queues for Inter-Task Communication)

[학습 목차](README.md) · [공통 준비](getting-started.md) · [공식 튜토리얼 번역](../docs/tutorial_9/README.md) · [이전 단원](tutorial_8.md)

이 단원에서는 FreeRTOS에서 task 간에 데이터를 안전하게 주고받는 가장 핵심적인 메커니즘인 **Queue(큐)**의 동작 원리와 생성, 송수신 API 사용법을 실습한다.

---

## 1. 이번 실습에서 할 일

* **해결하려는 문제**: 전역 변수를 무방비하게 공유할 때 발생하는 경쟁 상태(Race Condition)를 방지하고, task 간에 스레드 안전(Thread-safe)한 FIFO 방식으로 데이터를 전달한다.
* **학습 목표**:
  1. `xQueueCreate()` API를 사용하여 고정 크기 항목을 저장하는 큐를 생성할 수 있다.
  2. `xQueueSend()`와 `xQueueReceive()`를 이용해 송신자 task와 수신자 task 간에 데이터를 송수신한다.
  3. FreeRTOS 큐의 **값 복사(Pass by Copy)** 방식과, 큐가 비어있을 때의 **Blocked 대기 메커니즘**을 설명할 수 있다.

---

## 2. 시작 전 확인

### 2.1 C 언어 핵심 복습
* **`sizeof` 연산자**: 특정 데이터 타입이나 변수가 메모리에서 차지하는 바이트(Byte) 크기를 계산한다. 큐를 만들 때 각 항목의 크기를 `sizeof( UBaseType_t )`와 같이 지정한다.
* **주소 연산자 `&`**: 변수의 메모리 주소를 얻는다. 큐에 데이터를 넣거나 꺼낼 때, 변수 자체가 아니라 **데이터가 위치한 주소(포인터)**를 커널에 넘겨주어야 커널이 그 메모리에서 큐 버퍼로 바이트 단위 복사를 수행한다.

### 2.2 확인할 소스 및 설정 파일
* 실습 코드: [`source/tutorials/tutorial_9/source/tutorial_9.c`](../../source/tutorials/tutorial_9/source/tutorial_9.c)
* 설정 파일: [`source/tutorials/tutorial_9/include/FreeRTOSConfig.h`](../../source/tutorials/tutorial_9/include/FreeRTOSConfig.h)
* 참조 해답: [`ko/guides/solutions/tutorial_9.c`](solutions/tutorial_9.c)

> **실습 형태**: 이 단원은 `tutorial_9.c` 내부의 **`TODO 1`, `TODO 2`, `TODO 3`을 완성하는 구현 실습**이다.

---

## 3. 개념 이해

### 3.1 왜 전역 변수 대신 Queue를 쓰는가?
여러 task가 아무런 보호 장치 없이 전역 변수를 읽고 쓰면, 한 task가 값을 절반만 썼을 때 다른 task가 선점하여 값을 읽어가는 데이터 오염이 발생한다.
FreeRTOS 큐는 여러 task의 송수신을 커널이 조정한다. 다만 큐에 넣는 원본 데이터를 여러 주체가 동시에 수정하거나 포인터가 가리키는 객체의 수명이 끝나는 문제까지 해결해 주지는 않는다.

```mermaid
flowchart LR
    subgraph Sender["송신 태스크 (Sender)"]
        S1["uxValueToSend = 10"]
    end

    subgraph QueueRAM["FreeRTOS 큐 버퍼 (FIFO 링 버퍼)"]
        direction LR
        Q1["[ 10 ]"] --> Q2["[ 빈칸 ]"] --> Q3["[ 빈칸 ]"]
    end

    subgraph Receiver["수신 태스크 (Receiver)"]
        R1["uxReceivedValue = 10"]
    end

    S1 -->|xQueueSend: 값 복사| Q1
    Q1 -->|xQueueReceive: 값 복사| R1
```

### 3.2 큐의 2대 핵심 특성

1. **값 복사 전송 (Pass by Copy)**:
   * 포인터만 달랑 전달하는 것이 아니라, 전달하려는 데이터의 바이트들을 큐 내부 메모리로 **직접 복사**해 넣는다.
   * **장점**: 송신자가 지역 변수를 큐에 넣고 그 변수가 사라지거나 값을 나중에 바꾸더라도, 이미 큐 안에 복사된 데이터는 안전하게 보존된다.
   * *(단, 대용량 버퍼는 복사 오버헤드가 크므로 포인터 자체를 큐 항목으로 만들어 전달하는 기법을 쓴다.)*

2. **블로킹 대기 (Blocking on Queues)**:
   * **수신 대기**: 큐가 비어 있고 양수 대기 시간을 지정했다면, 수신자는 CPU를 쓰지 않는 **Blocked 상태**로 들어간다. 나중에 송신자가 데이터를 큐에 넣는 순간 커널이 대기 중인 수신자를 Ready로 만든다. 실행은 스케줄러가 선택한 뒤에 이루어진다.
   * **송신 대기**: 큐가 가득 차면 지정한 시간 범위에서 공간을 기다린다. 대기 시간이 0이면 즉시 실패를 반환한다.

---

## 4. 실행 흐름 읽기

```mermaid
sequenceDiagram
    autonumber
    participant S as Sender (우선순위 1)
    participant Q as FreeRTOS Queue
    participant R as Receiver (우선순위 0)

    Note over S,R: 시작 시 우선순위가 높은 Sender가 먼저 실행됨
    Note over S: Sender가 실행됨 (우선순위가 더 높음)
    S->>Q: xQueueSend(1) 호출 (TODO 2)
    Note over Q: 첫 송신 값이 큐에 보관됨<br/>이후 주기에는 대기 중인 Receiver를 Ready로 전환
    S->>S: vTaskDelay(1000ms) 호출 (Blocked 전이)
    Note over S,R: Sender가 잠들자 CPU가 Receiver에게 넘어감
    Q->>R: xQueueReceive() 성공 (값 1 꺼냄)
    R->>R: "Value received: 1" 출력
    R->>Q: 다음 xQueueReceive() 호출 → 큐가 비었으므로 다시 Blocked 대기
```

---

## 5. 코드와 API 이해

### 5.1 `xQueueCreate()` API
```c
QueueHandle_t xQueueCreate( UBaseType_t uxQueueLength, UBaseType_t uxItemSize );
```
* `uxQueueLength`: 큐에 동시에 담을 수 있는 최대 항목 수 (이번 실습: `5`).
* `uxItemSize`: 항목 1개가 차지하는 바이트 수 (`sizeof( UBaseType_t )`).
* **반환값**: 성공 시 `QueueHandle_t`, 힙 메모리 부족 시 `NULL`.

### 5.2 `xQueueSend()` API
```c
BaseType_t xQueueSend( QueueHandle_t xQueue,
                       const void * pvItemToQueue,
                       TickType_t xTicksToWait );
```
* `xQueue`: 대상 큐 핸들.
* `pvItemToQueue`: 큐로 복사해 넣을 데이터의 **메모리 주소 포인터** (`&( uxValueToSend )`).
* `xTicksToWait`: 큐가 꽉 찼을 때 대기할 시간. `0`이면 즉시 시도하고 유한 값이면 그 tick 범위까지 기다린다. 이 예제처럼 `INCLUDE_vTaskSuspend=1`일 때 `portMAX_DELAY`는 무기한 대기다.
* **반환값**: 성공 시 `pdPASS`, 타임아웃 만료 시 `errQUEUE_FULL`.

### 5.3 `xQueueReceive()` API
```c
BaseType_t xQueueReceive( QueueHandle_t xQueue,
                          void * pvBuffer,
                          TickType_t xTicksToWait );
```
* `pvBuffer`: 큐에서 꺼낸 데이터를 복사받을 **수신 버퍼 주소 포인터** (`&( uxReceivedValue )`).
* `xTicksToWait`: 큐가 비어있을 때 대기할 시간 (`portMAX_DELAY`).

---

## 6. 직접 구현 또는 관찰 준비

전체 참조 해답은 [tutorial_9.c](solutions/tutorial_9.c)에 있다. 먼저 TODO별 힌트를 따라 작성하고, [자기 코드 보존·해답 적용·복구 절차](getting-started.md#solution-workflow)에 따라 비교한다.

`source/tutorials/tutorial_9/source/tutorial_9.c` 파일을 열고 세 개의 TODO를 완성한다.

### 6.1 TODO 1: 큐 생성 (`main` 함수)
70~78행 주석 부분을 찾는다:
* 항목 수 5개, 크기는 `sizeof( UBaseType_t )`

* **TODO 1 힌트 1**: 항목의 개수와 항목 하나의 바이트 크기를 구별한다.
* **TODO 1 힌트 2**: `main`에서 길이 5와 `sizeof(UBaseType_t)`를 전달하고 반환 핸들을 `xQueue`에 저장한다.

<details>
<summary>TODO 1 해답 코드 보기</summary>

```c
    xQueue = xQueueCreate( 5, sizeof( UBaseType_t ) );
```

**이유**: 항목의 개수와 항목 하나의 바이트 크기를 구별한다.
</details>

### 6.2 TODO 2: 데이터 송신 (`prvSenderTask`)
108~116행 주석 부분을 찾는다:
* `uxValueToSend` 변수의 주소를 전달하고 대기 시간은 `portMAX_DELAY`로 설정한다.

* **TODO 2 힌트 1**: 송신 API는 인자가 가리키는 데이터의 내용을 큐에 복사한다.
* **TODO 2 힌트 2**: `prvSenderTask`에서 `uxValueToSend`의 주소와 설정상 무기한 대기를 전달하고 결과를 `xQueueSendResult`에 저장한다.

<details>
<summary>TODO 2 해답 코드 보기</summary>

```c
        xQueueSendResult = xQueueSend( xQueue, &( uxValueToSend ), portMAX_DELAY );
```

**이유**: 송신 API는 인자가 가리키는 데이터의 내용을 큐에 복사한다.
</details>

### 6.3 TODO 3: 데이터 수신 (`prvReceiverTask`)
134~141행 주석 부분을 찾는다:
* `uxReceivedValue` 변수의 주소로 데이터를 복사받는다.

* **TODO 3 힌트 1**: 수신 API에는 데이터를 쓸 수 있는 변수의 주소가 필요하다.
* **TODO 3 힌트 2**: `prvReceiverTask`에서 `uxReceivedValue`의 주소를 전달한다. 빈 큐에서는 `portMAX_DELAY`로 기다린다.

<details>
<summary>TODO 3 해답 코드 보기</summary>

```c
        xQueueReceive( xQueue, &( uxReceivedValue ), portMAX_DELAY );
```

**이유**: 수신 API에는 데이터를 쓸 수 있는 변수의 주소가 필요하다.
</details>

> 전체 완성 코드는 [`ko/guides/solutions/tutorial_9.c`](solutions/tutorial_9.c)에서 확인할 수 있다.

---

## 7. 빌드와 실행

[공통 준비](getting-started.md)의 학습용 사본 루트에서 다음 명령을 실행한다:

```bash
# 1. CMake 빌드 구성 (Tutorial 9 지정)
cmake -S source -B build/tutorial_9 -DTUTORIAL=9

# 2. 컴파일 및 빌드
cmake --build build/tutorial_9

# 3. 바이너리 실행
./build/tutorial_9/freertos_example
```

* **종료 방법**: 터미널에서 **`Ctrl + C`**를 누른다.

---

## 8. 결과 해석

### 8.1 출력 예시

수신 값의 FIFO 순서를 확인한다. 아래 큐 잔여 수는 해당 조회 시점의 값이며 다른 송신 주기에서도 항상 0인 것은 아니다.

```text
Value received from the queue: 1
Number of items in the queue: 0.
Value received from the queue: 2
Number of items in the queue: 0.
Value received from the queue: 3
Number of items in the queue: 0.
```

### 8.2 판정 기준 및 해석
* **결과의 특징**: 1초마다 송신자가 값을 1씩 증가시키며 보내고, 수신자가 선택되면 그 값을 받아 출력한다.
* `Number of items in the queue: 0`: 수신자가 데이터를 꺼내가자마자 큐에는 남아있는 항목 수가 0개가 됨을 보여준다.

---

## 9. 막혔을 때 (트러블슈팅)

| 증상 | 원인 | 해결 방법 |
| :--- | :--- | :--- |
| 출력 없이 멈춤 | 큐 생성·송신 결과 검사에서 assertion 가능성 | 큐 핸들과 TODO의 반환값 대입을 확인. 이 설정의 assertion은 별도 메시지가 없음 |
| 송신 뒤 진행하지 않음 | 큐가 가득 차서 대기하거나 생성·송신에 실패했을 수 있음 | 수신 task의 생성 및 수신 호출, 송신 반환값을 확인 |
| 이상한 큰 숫자가 출력됨 | 수신 버퍼 포인터에 `&`를 붙이지 않고 쓰레기 주소에 씀 | `&( uxReceivedValue )` 주소 전달 확인 |

---

## 10. 혼자 확인하기 (자가 점검)

### 질문 1 (원리)
FreeRTOS 큐가 데이터를 전송할 때 "Pass by Copy" 방식을 기본으로 채택한 가장 큰 안전상의 이유는 무엇인가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: 송신 task가 전송 직후 해당 변수를 덮어쓰거나, 송신 task의 스코프를 벗어나 메모리가 해제되더라도 수신자가 안전하게 독립된 복사본을 읽을 수 있도록 보장하기 위함이다.
</details>

### 질문 2 (상태 전이)
수신 task(`prvReceiverTask`)의 무한 루프에는 `vTaskDelay()`가 전혀 없는데도, 왜 CPU를 100% 점유하지 않고 1초에 한 번만 실행되는가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: `xQueueReceive(..., portMAX_DELAY)` 때문이다.
* **원리**: 큐가 비어 있으면 `xQueueReceive()` 내부에서 수신 task를 **Blocked 상태**로 전환시켜 CPU를 즉시 양보한다. 이후 송신 task가 1초 뒤 데이터를 큐에 넣을 때까지 수신 task는 Blocked로 대기하여 스케줄러의 실행 대상에서 빠진다.
</details>

### 질문 3 (조건 변경과 결과 예측)
`prvSenderTask`의 `pdMS_TO_TICKS(1000)`만 `pdMS_TO_TICKS(500)`으로 바꾼다.

실행 전에 어떤 결과가 달라지고 어떤 동작이 그대로 유지될지 적는다. 직접 작성한 코드와 수정 전 파일은 [공통 보존·복구 절차](getting-started.md#solution-workflow)에 따라 보관한다. 위에서 지정한 조건 하나만 바꾸고 `cmake --build build/tutorial_9` 명령으로 다시 빌드한다. 실행 결과를 예측과 비교한 뒤 아래 해설을 확인한다. 해설은 예상 결과를 설명한 것이므로 직접 얻은 로그와 구분한다.

<details>
<summary>예상 결과와 해설 확인</summary>

수신 빈도가 늘어나도 FIFO 값은 1, 2, 3 순으로 유지될 것으로 예상한다. 정확한 벽시계 간격보다 값의 순서를 확인한다.
</details>

**복구와 재확인**: `prvSenderTask`의 `pdMS_TO_TICKS(500)`을 `pdMS_TO_TICKS(1000)`으로 되돌린다. 같은 단원을 다시 빌드하고 실행하여 [8절의 결과](#8-결과-해석)와 비교한다. 결과를 설명하기 어려우면 [관련 개념 다시 읽기](#5-코드와-api-이해)로 돌아간다.

---

## 11. 다음 학습

지금까지는 두 task 사이의 큐 통신을 배웠다.
그런데 만약 데이터를 보내는 주체가 일반 task가 아니라 **하드웨어 인터럽트(ISR)**라면 어떻게 해야 할까? ISR에서도 `xQueueSend()`를 그대로 부르면 될까?

이제 **[Tutorial 10: 인터럽트와 task 간의 queue 통신 (Using Queues for Communication between Interrupts and Tasks)](tutorial_10.md)**으로 넘어가서 ISR 전용 API인 `FromISR`의 엄격한 규칙을 배운다.

* **한국어 커널북 심화 읽기**:
  * [5장: queue 관리 (Queue Management)](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch05.md)
