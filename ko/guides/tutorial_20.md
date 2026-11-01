# Tutorial 20: message buffer

[학습 목차](README.md) · [공통 준비](getting-started.md) · [공식 튜토리얼 번역](../docs/tutorial_20/README.md) · [이전 단원](tutorial_19.md)

이 단원에서는 스트림 버퍼(Stream Buffer)를 기반으로 구축되어, 패킷 경계가 없는 단순 바이트 열이 아니라 **가변 길이의 이산 메시지(Discrete Message)**를 온전한 한 단위로 안전하게 주고받을 수 있는 **메시지 버퍼(Message Buffer)**의 동작 원리와 내부 구조를 배운다.

---

## 1. 이번 실습에서 할 일

* **해결하려는 문제**: 스트림 버퍼는 바이트를 쪼개어 읽을 수 있어 송신자가 보낸 패킷의 시작과 끝(경계)을 수신자가 직접 파싱해야 한다. 또한 일반 큐는 크기가 다른 다양한 길이의 메시지를 담기에 메모리 낭비가 심하다. 가변 길이의 메시지를 패킷 경계를 보존하면서 단일 송수신자 간에  전송한다.
* **학습 목표**:
  1. 메시지 버퍼(Message Buffer)가 내부적으로 스트림 버퍼를 어떻게 확장하여 메시지 경계를 보존하는지 이해한다.
  2. 메시지 버퍼의 `configMESSAGE_BUFFER_LENGTH_TYPE` 길이 헤더 메커니즘과 실제 버퍼 용량 계산법을 습득한다.
  3. `xMessageBufferCreate()`, `xMessageBufferSend()`, `xMessageBufferReceive()` API를 구현하고, 한 번에 하나의 완전한 메시지가 수신되는 과정을 관찰한다.

---

## 2. 시작 전 확인

### 2.1 C 언어 배열과 가변 길이 데이터
* `uint8_t ucMessage1[ 5 ]`: 5바이트 크기의 첫 번째 메시지 데이터다.
* `uint8_t ucMessage2[ 4 ]`: 4바이트 크기의 두 번째 메시지 데이터다.
* `uint8_t ucRxData[ 10 ]`: 수신자가 수신할 최대 10바이트 용량의 수신 버퍼다. 메시지 버퍼에서는 수신 버퍼의 크기가 유입되는 메시지 크기보다 반드시 크거나 같아야 한다.

### 2.2 확인할 소스 및 설정 파일
* 실습 코드: [`source/tutorials/tutorial_20/source/tutorial_20.c`](../../source/tutorials/tutorial_20/source/tutorial_20.c)
* 설정 파일: [`source/tutorials/tutorial_20/include/FreeRTOSConfig.h`](../../source/tutorials/tutorial_20/include/FreeRTOSConfig.h)
* 참조 해답: [`ko/guides/solutions/tutorial_20.c`](solutions/tutorial_20.c)

> **실습 형태**: `tutorial_20.c` 내부의 **`TODO 1`, `TODO 2`, `TODO 3`, `TODO 4`를 완성하는 구현 실습**이다.

---

## 3. 개념 이해

### 3.1 메시지 버퍼(Message Buffer)란?
메시지 버퍼는 FreeRTOS에서 **단일 송신자(Writer task 또는 ISR)와 단일 수신자(Reader task 또는 ISR)** 사이에서 서로 다른 크기의 가변 길이 메시지들을 온전한 패킷 단위로 전송하는 특수 버퍼다.

메시지 버퍼는 완전히 독립된 커널 객체가 아니라, **스트림 버퍼(Stream Buffer) 위에 얇게 얹혀진 상위 계층 래퍼(Wrapper)**다.

```mermaid
flowchart TD
    A["버퍼 비어 있음: 용량 20바이트"] --> B["메시지 1 보관: 헤더 8 + 데이터 5 = 13바이트"]
    B --> C["높은 우선순위 Reader가 메시지 1 수신: 버퍼 비움"]
    C --> D["메시지 2 보관: 헤더 8 + 데이터 4 = 12바이트"]
    D --> E["Reader가 메시지 2 수신: 버퍼 비움"]
```

현재 64비트 POSIX 환경의 흐름이다. 두 메시지를 20바이트 버퍼에 동시에 보관하는 그림이 아니다.

### 3.2 스트림 버퍼(T19) vs 메시지 버퍼(T20)
| 비교 항목 | 스트림 버퍼 (Stream Buffer, T19) | 메시지 버퍼 (Message Buffer, T20) |
| :--- | :--- | :--- |
| **데이터 형태** | 연속된 바이트 스트림 (경계 없음) | **이산 메시지 (Discrete Message, 경계 보존)** |
| **내부 헤더** | 헤더 없음 (순수 페이로드만 저장) | 메시지마다 **`sizeof(configMESSAGE_BUFFER_LENGTH_TYPE)` 길이 헤더** 저장 |
| **수신 동작** | 버퍼에 있는 바이트 중 요청한 만큼(일부) 읽기 가능 | **반드시 메시지 1개 전체를 온전히 수신** (부분 수신 불가) |
| **수신 버퍼 크기 부족 시** | 가능한 만큼 잘라서 반환 | **0 반환하고 데이터를 버퍼에 그대로 남겨둠** |
| **트리거 레벨** | 사용자가 설정한 바이트 수에 도달하면 알림 | 메시지 1개가 완전히 저장되는 순간 자동 알림 |

### 3.3 버퍼 용량 계산

길이 헤더 크기는 `sizeof(configMESSAGE_BUFFER_LENGTH_TYPE)`다. 이 설정은 별도로 길이형을 바꾸지 않으므로 기본형 `size_t`를 쓰며, 검증한 64비트 POSIX 환경에서는 8바이트다.

* 첫 메시지: 데이터 5 + 헤더 8 = 13바이트.
* 두 번째 메시지: 데이터 4 + 헤더 8 = 12바이트.
* 두 개를 동시에 보관하려면 25바이트이므로 원본의 20바이트 버퍼에는 함께 들어가지 않는다.

그래도 예제가 동작하는 이유는 Reader의 우선순위가 더 높아 첫 메시지를 수신하고 비운 뒤 Writer가 두 번째를 보내기 때문이다. 버퍼를 설계할 때는 한 메시지가 들어가는가와 여러 메시지를 동시에 보관할 수 있는가를 따로 계산한다.

---

## 4. 실행 흐름 읽기

실습 코드에서는 Reader task(우선순위 1)가 Writer task(우선순위 0)보다 우선순위가 높다.

```mermaid
sequenceDiagram
    autonumber
    actor Main as main()
    participant Reader as Reader Task (우선순위 1)
    participant Buffer as Message Buffer (Size: 20)
    participant Writer as Writer Task (우선순위 0)

    Main->>Reader: xTaskCreate (우선순위 1)
    Main->>Writer: xTaskCreate (우선순위 0)
    Main->>Buffer: xMessageBufferCreate(20)
    Main->>Reader: 스케줄러 시작 -> 높은 우선순위 Reader 실행

    Reader->>Buffer: xMessageBufferReceive(최대 10바이트, portMAX_DELAY)
    Note over Reader,Buffer: 메시지가 전혀 없음 -> Reader 즉시 Blocked 대기 진입

    Writer->>Writer: "Sending first message..." 출력
    Writer->>Buffer: xMessageBufferSend(5바이트 ucMessage1)
    Note over Buffer: [헤더:5] + [데이터 5바이트] 기록 완료!<br/>온전한 메시지가 도착했으므로 Reader 즉시 Unblock

    Note over Reader,Writer: Reader(우선순위 1)가 Writer를 선점
    Reader->>Buffer: 첫 번째 메시지 5바이트 온전히 인출
    Note over Reader: "Received message of 5 length... 0x1~0x5" 출력
    Reader->>Buffer: 다음 루프: xMessageBufferReceive(최대 10바이트)
    Note over Buffer: 버퍼가 비었으므로 Reader 다시 Blocked 대기 진입

    Writer->>Writer: "Sending second message..." 출력
    Writer->>Buffer: xMessageBufferSend(4바이트 ucMessage2)
    Note over Buffer: [헤더:4] + [데이터 4바이트] 기록 완료!<br/>Reader 다시 즉시 Unblock

    Note over Reader,Writer: Reader가 다시 Writer를 선점
    Reader->>Buffer: 두 번째 메시지 4바이트 온전히 인출
    Note over Reader: "Received message of 4 length... 0xA~0xD" 출력
    Reader->>Buffer: 다음 루프: xMessageBufferReceive() -> Blocked 대기

    Writer->>Writer: "Finished sending both the messages." 출력 후 1초 지연
```

---

## 5. 코드와 API 이해

### 5.1 `xMessageBufferCreate`
```c
MessageBufferHandle_t xMessageBufferCreate( size_t xBufferSizeBytes );
```
* **역할**: 지정된 바이트 크기의 메시지 버퍼를 동적으로 생성한다.
* **`xBufferSizeBytes`**: 버퍼가 보관할 수 있는 총 바이트 용량이다.
  > **주의**: 이 크기에는 각 메시지의 **길이 헤더 공간**이 반드시 포함되어야 한다.
* **반환값**: 성공 시 버퍼 핸들(`MessageBufferHandle_t`), 힙 부족 시 `NULL`.

### 5.2 `xMessageBufferReceive`
```c
size_t xMessageBufferReceive( MessageBufferHandle_t xMessageBuffer,
                              void * pvRxData,
                              size_t xBufferLengthBytes,
                              TickType_t xTicksToWait );
```
* **역할**: 메시지 버퍼로부터 다음 대기 중인 이산 메시지 **1개를 통째로** 꺼내어 `pvRxData`에 복사한다.
* **동작 특성**:
  * 메시지가 없고 대기 시간이 양수이면 지정한 범위에서 Blocked로 기다린다. `0`이면 즉시 0을 반환한다. 이 예제 설정에서 `portMAX_DELAY`는 무기한 대기다.
  * **중요 (버퍼 크기 검증)**: 수신자가 전달한 `xBufferLengthBytes`가 도착한 메시지의 실제 길이보다 작으면 데이터를 읽지 않고 `0`을 반환하며 메시지는 버퍼에 그대로 보존된다.
* **반환값**: 성공 시 수신한 메시지의 실제 바이트 길이(`size_t`), 실패/타임아웃 시 `0`.

### 5.3 `xMessageBufferSend`
```c
size_t xMessageBufferSend( MessageBufferHandle_t xMessageBuffer,
                           const void * pvTxData,
                           size_t xDataLengthBytes,
                           TickType_t xTicksToWait );
```
* **역할**: `pvTxData`의 데이터를 `xDataLengthBytes` 길이의 메시지로 포장하여 메시지 버퍼에 기록한다. 내부적으로 앞에 `sizeof(configMESSAGE_BUFFER_LENGTH_TYPE)`바이트 길이 헤더가 붙는다.
* **반환값**: 성공 시 페이로드 바이트 수(`xDataLengthBytes`), 실패 시 `0`이다. 메시지와 헤더가 버퍼 전체 용량보다 크면 `portMAX_DELAY`를 지정해도 들어갈 수 없으므로 대기 없이 실패할 수 있다.

---

## 6. 직접 구현 또는 관찰 준비

전체 참조 해답은 [tutorial_20.c](solutions/tutorial_20.c)에 있다. 먼저 TODO별 힌트를 따라 작성하고, [자기 코드 보존·해답 적용·복구 절차](getting-started.md#solution-workflow)에 따라 비교한다.

실습 파일: [`source/tutorials/tutorial_20/source/tutorial_20.c`](../../source/tutorials/tutorial_20/source/tutorial_20.c)

### 6.1 TODO 1: 메시지 버퍼 생성
`main()` 함수 내부에서 20바이트 크기의 메시지 버퍼를 생성한다.

* **TODO 1 힌트 1**: 생성 용량에는 데이터와 길이 헤더가 함께 들어간다.
* **TODO 1 힌트 2**: 원본 흐름의 순차 수신을 유지하며 `main`에서 용량 20을 지정하고 `xMessageBuffer`에 저장한다.

<details>
<summary>TODO 1 해답 코드 보기</summary>

```c
    /*
     * TODO 1 - Create a message buffer of size 20 using xMessageBufferCreate API.
     */
    xMessageBuffer = xMessageBufferCreate( 20 );
```

**이유**: 생성 용량에는 데이터와 길이 헤더가 함께 들어간다.
</details>

### 6.2 TODO 2: 메시지 수신 구현
`prvReaderTask()` 함수 내부에서 최대 10바이트 크기의 메시지를 수신한다.

* **TODO 2 힌트 1**: 메시지는 일부만 읽을 수 없으므로 최대 메시지 길이 이상인 수신 공간을 제공한다.
* **TODO 2 힌트 2**: Reader에서 `ucRxData` 시작 주소와 용량 10을 전달하고 반환 페이로드 길이를 `uxReceivedBytes`에 저장한다.

<details>
<summary>TODO 2 해답 코드 보기</summary>

```c
    /*
     * TODO 2 - Read a message from the message buffer using xMessageBufferReceive API.
     */
    uxReceivedBytes = xMessageBufferReceive( xMessageBuffer,
                                            &( ucRxData[ 0 ] ),
                                            10,
                                            portMAX_DELAY );
```

**이유**: 메시지는 일부만 읽을 수 없으므로 최대 메시지 길이 이상인 수신 공간을 제공한다.
</details>

### 6.3 TODO 3: 첫 번째 메시지(5바이트) 송신
`prvWriterTask()` 함수 내부에서 `ucMessage1` (5바이트)을 송신한다.

* **TODO 3 힌트 1**: 첫 배열을 하나의 메시지로 보낸다. 헤더는 API가 기록한다.
* **TODO 3 힌트 2**: Writer에서 `ucMessage1[0]`의 주소와 페이로드 길이 5를 넘긴다. 길이에 헤더 8을 직접 더하지 않는다.

<details>
<summary>TODO 3 해답 코드 보기</summary>

```c
    /*
     * TODO 3 - Send the first message to the message buffer using xMessageBufferSend API.
     */
    uxSentBytes = xMessageBufferSend( xMessageBuffer,
                                     &( ucMessage1[ 0 ] ),
                                     5,
                                     portMAX_DELAY );
```

**이유**: 첫 배열을 하나의 메시지로 보낸다. 헤더는 API가 기록한다.
</details>

### 6.4 TODO 4: 두 번째 메시지(4바이트) 송신
`prvWriterTask()` 함수 내부에서 `ucMessage2` (4바이트)를 송신한다.

* **TODO 4 힌트 1**: 두 번째 배열도 별도 메시지 경계를 가진다.
* **TODO 4 힌트 2**: Writer에서 `ucMessage2[0]`의 주소와 페이로드 길이 4를 넘기고 `uxSentBytes`에 결과를 저장한다.

<details>
<summary>TODO 4 해답 코드 보기</summary>

```c
    /*
     * TODO 4 - Send the second message to the message buffer using xMessageBufferSend API.
     */
    uxSentBytes = xMessageBufferSend( xMessageBuffer,
                                     &( ucMessage2[ 0 ] ),
                                     4,
                                     portMAX_DELAY );
```

**이유**: 두 번째 배열도 별도 메시지 경계를 가진다.
</details>

---

## 7. 빌드와 실행

[공통 준비](getting-started.md)의 학습용 사본 루트에서 다음 명령을 실행한다:

```bash
# 1. CMake 빌드 구성 (Tutorial 20 지정)
cmake -S source -B build/tutorial_20 -DTUTORIAL=20

# 2. 컴파일 및 빌드
cmake --build build/tutorial_20

# 3. 바이너리 실행
./build/tutorial_20/freertos_example
```

* **종료 방법**: 터미널에서 **`Ctrl + C`**를 누른다.

---

## 8. 결과 해석

### 8.1 출력 예시

5바이트와 4바이트 메시지가 각각 온전히 수신된다. 원본은 첫 수신 뒤 두 번째를 보내며 정확한 벽시계 간격은 고정하지 않는다.

```text
Sending first message...
Received message of 5 length...
0x1 0x2 0x3 0x4 0x5
Sending second message...
Received message of 4 length...
0xA 0xB 0xC 0xD
Finished sending both the messages.
Sending first message...
Received message of 5 length...
0x1 0x2 0x3 0x4 0x5
Sending second message...
Received message of 4 length...
0xA 0xB 0xC 0xD
Finished sending both the messages.
```

### 8.2 판정 기준 및 단계별 상세 분석
1. **메시지 단위의 즉각 수신**:
   * Writer가 `Sending first message...`를 출력하고 5바이트를 전송하자마자 완전한 메시지가 버퍼에 채워졌으므로 Reader가 즉시 깨어난다.
   * 스트림 버퍼(T19)와 달리 사용자가 트리거 레벨을 수동 계산할 필요 없이, **하나의 메시지가 완성되는 순간 Reader가 깨어난다.**
2. **메시지 경계의 보존**:
   * 첫 번째 메시지는 길이 5(`0x1 0x2 0x3 0x4 0x5`), 두 번째 메시지는 길이 4(`0xA 0xB 0xC 0xD`)로 정확히 분리되어 수신된다.
   * 두 메시지가 섞이거나 일부만 잘려 나오는 일 없이 독립된 메시지으로 전달됨을 확인한다.

---

## 9. 막혔을 때 (트러블슈팅)

| 증상 | 원인 | 해결 방법 |
| :--- | :--- | :--- |
| `xMessageBufferReceive`가 데이터를 안 읽고 계속 0을 반환함 | 수신 버퍼 `xBufferLengthBytes`가 메시지 크기보다 작음 | 수신 버퍼 크기를 유입되는 최대 메시지 크기 이상으로 늘림 |
| 송신 뒤 assertion으로 멈춤 | 반환된 바이트 수가 요청과 다를 수 있음 | `uxSentBytes`와 헤더 포함 크기를 확인. 현재 64비트 환경의 길이 헤더는 8바이트이며 assertion 메시지는 자동 출력되지 않음 |
| 컴파일 에러: `MessageBufferHandle_t undeclared` | 헤더 파일 누락 | `#include "message_buffer.h"`가 포함되어 있는지 확인 |

---

## 10. 혼자 확인하기 (자가 점검)

### 질문 1 (버퍼 용량 계산)
10바이트 메시지 2개를 동시에 보관하려고 한다. 길이 헤더가 4바이트인 구성과 이 실습처럼 8바이트인 구성에서 `xMessageBufferCreate()`의 최소 용량은 각각 얼마인가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: 각각 `2 * (10 + 4) = 28`바이트, `2 * (10 + 8) = 36`바이트다.
* **해설**: 기준은 CPU 이름이 아니라 실제 `sizeof(configMESSAGE_BUFFER_LENGTH_TYPE)`다. 동적 생성 API는 내부 구현에 필요한 추가 1바이트를 처리하므로 위 요청 용량에 사용자가 또 1을 더하지 않는다.
</details>

### 질문 2 (스트림 버퍼와의 결정적 차이)
수신 버퍼 크기가 3바이트인 Reader task가 5바이트짜리 데이터가 들어있는 버퍼를 읽으려고 할 때, 스트림 버퍼(`xStreamBufferReceive`)와 메시지 버퍼(`xMessageBufferReceive`)의 동작 차이는 무엇인가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: 스트림 버퍼는 3바이트를 먼저 잘라서 읽고 성공(3 반환)하지만, 메시지 버퍼는 메시지가 손상되는 것을 방지하기 위해 단 1바이트도 읽지 않고 0을 반환하며 메시지를 버퍼에 그대로 남겨둔다.
</details>

### 질문 3 (한 조건 변형과 실행 추적)
`main()`의 생성 용량만 `20`에서 `25`로 바꾼다.

실행 전에 바뀔 결과와 유지될 조건을 각각 적는다. 자기 풀이와 변경 전 파일은 [공통 보존·복구 절차](getting-started.md#solution-workflow)로 남긴다. 한 조건만 수정하고 `cmake --build build/tutorial_20`으로 다시 빌드한 뒤 실행해 예측과 비교한다. 아래 해설은 변형의 예상 결과이며, 직접 얻은 로그와 구분한다.

<details>
<summary>예상 결과와 해설 확인</summary>

현재 8바이트 헤더 기준 두 메시지를 함께 보관할 수 있는 용량이 된다. 그러나 원본의 높은 Reader 우선순위가 유지되어 출력은 여전히 첫 송신·수신 다음 두 번째 송신·수신 순서일 것으로 예상한다. 용량 증가는 두 메시지 동시 적재를 강제하지 않는다.
</details>

**복구와 재확인**: 변경한 한 값(추가한 코드라면 그 한 줄)을 원래대로 돌리고 같은 단원을 다시 빌드·실행한다. 원래 조건의 8절 결과와 비교한다. 설명이 어려우면 [관련 개념 다시 읽기](#3-개념-이해)로 돌아간다.

---

## 11. 다음 학습

메시지 경계의 보존과 헤더를 포함한 용량 계산을 살펴보았다.

이제 FreeRTOS 튜토리얼 21개의 마지막 단원인 **[Tutorial 21: 자원 관리 종합 비교 (Resource Management)](tutorial_21.md)**으로 넘어가서, FreeRTOS가 제공하는 3가지 자원 보호 기법인 **스케줄러 일시 중지(`vTaskSuspendAll`), 상호 배제 뮤텍스(`Mutex`), 임계 구역(`taskENTER_CRITICAL`)**의 적용 범위와 인터럽트 보호 능력을 총체적으로 비교 정리한다.

* **공식 FreeRTOS 심화 문서**:
  * [FreeRTOS Official Message Buffer API Reference](https://www.freertos.org/RTOS-message-buffer-API.html)
  * *(참고: Message Buffer는 Stream Buffer 기반의 고급 추상화 계층으로 공식 웹 기술문서를 참조한다.)*
