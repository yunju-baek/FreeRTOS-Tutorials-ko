# Tutorial 15: event group을 통한 이벤트 관리 (Event Management using Event Groups)

이 단원에서는 여러 개의 복합적인 사건(이벤트)을 비트 플래그로 묶어서 관리하고, 여러 task에 이벤트를 동시에 전파(브로드캐스팅)할 수 있는 **event group(이벤트 그룹)**을 배운다.

---

## 1. 이번 실습에서 할 일

* **해결하려는 문제**: 큐나 세마포어는 "하나의 사건"만 다루거나 "단 하나의 task"만 깨울 수 있다. 여러 비트의 조합(AND/OR 조건)을 기다리고, 조건을 만족했을 때 대기 중이던 여러 task를 한꺼번에 깨우는 고급 동기화를 구현한다.
* **학습 목표**:
  1. 이벤트 그룹의 비트 플래그 구조(`EventBits_t`)와 큐/세마포어 대비 2대 차이점을 설명할 수 있다.
  2. `xEventGroupCreate()`, `xEventGroupSetBits()`, `xEventGroupWaitBits()` API를 활용해 생성, 비트 설정, 복합 대기를 구현한다.
  3. `xWaitForAllBits` 매개변수(`pdTRUE` = AND 조건, `pdFALSE` = OR 조건)와 `xClearOnExit`의 동작을 이해한다.

---

## 2. 시작 전 확인

### 2.1 C 언어 비트 연산 복습
* **비트 마스크 (Bitmask)**:
  * `EVENT_1_BIT ( 1UL << 0UL )` → `0b00000001` (10진수 1)
  * `EVENT_2_BIT ( 1UL << 1UL )` → `0b00000010` (10진수 2)
  * 비트 결합(OR): `( EVENT_1_BIT | EVENT_2_BIT )` → `0b00000011` (10진수 3)
  * 특정 비트 검사(AND): `if( ( value & EVENT_1_BIT ) != 0 )` → 1번 비트가 1인지 판별

### 2.2 확인할 소스 및 설정 파일
* 실습 코드: [`source/tutorials/tutorial_15/source/tutorial_15.c`](../../source/tutorials/tutorial_15/source/tutorial_15.c)
* 설정 파일: [`source/tutorials/tutorial_15/include/FreeRTOSConfig.h`](../../source/tutorials/tutorial_15/include/FreeRTOSConfig.h)
* 참조 해답: [`ko/guides/solutions/tutorial_15.c`](solutions/tutorial_15.c)

> **실습 형태**: `tutorial_15.c` 내부의 **`TODO 1`부터 `TODO 5`까지 5개의 빈칸을 완성하는 구현 실습**이다.

---

## 3. 개념 이해

### 3.1 큐·세마포어와 이벤트 그룹의 2대 결정적 차이

1. **복합 조건 대기 (AND / OR 조건)**:
   * 세마포어는 오직 1개의 신호만 기다릴 수 있다.
   * 이벤트 그룹은 "Event 1 또는 Event 2 중 아무거나 발생(OR)"하거나, "Event 1과 Event 2가 **둘 다 모두 발생(AND)**"할 때까지 Blocked 상태로 대기할 수 있다.
2. **다중 task 동시 기상 (브로드캐스팅)**:
   * 큐나 세마포어는 신호가 들어오면 대기 중이던 여러 task 중 **단 하나만** 깨운다 (토큰 소모).
   * 이벤트 그룹은 조건이 충족되면 **그 조건을 기다리던 모든 task를 동시에 전부 깨워준다(Unblock).**

```mermaid
flowchart TD
    subgraph Generator["이벤트 발생자 (prvEventGeneratorTask)"]
        G1["Event 1 발생! (비트 0 = 1)"]
        G2["Event 2 발생! (비트 1 = 1)"]
    end

    subgraph EventGroup["이벤트 그룹 [ 0b00000011 ]"]
        EG["두 비트가 모두 1이 됨! (AND 조건 충족)"]
    end

    subgraph Listeners["동시에 깨어나는 핸들러들"]
        H2["Handler 2 (우선순위 2)<br/>먼저 선점되어 둘 다 처리!"]
        H1["Handler 1 (우선순위 1)<br/>뒤이어 선점되어 둘 다 처리!"]
    end

    G1 --> EventGroup
    G2 --> EventGroup
    EventGroup -->|브로드캐스팅!| H2
    EventGroup -->|브로드캐스팅!| H1
```

---

## 4. 실행 흐름 읽기

```mermaid
sequenceDiagram
    autonumber
    participant Gen as Generator (우선순위 0)
    participant EG as Event Group
    participant H2 as Handler 2 (우선순위 2)
    participant H1 as Handler 1 (우선순위 1)

    Note over H1,H2: Handler 1, 2 둘 다 (Event 1 AND Event 2) 대기 진입 (Blocked)
    Gen->>EG: Event 1 비트 설정!
    Note over EG: 아직 Event 2가 없으므로 Handler들은 계속 대기
    Gen->>Gen: 500ms 지연
    Gen->>EG: Event 2 비트 설정! (드디어 둘 다 세팅됨!)
    Note over EG,H2: AND 조건이 충족되자 대기하던 모든 task가 깨어남!<br/>우선순위가 가장 높은 Handler 2가 먼저 실행!
    H2->>H2: Handler 2: Event 1, Event 2 처리 완료
    Note over H2,H1: Handler 2가 다시 대기하러 들어가자 차순위인 Handler 1 실행!
    H1->>H1: Handler 1: Event 1, Event 2 처리 완료
```

---

## 5. 코드와 API 이해

### 5.1 `xEventGroupCreate()` API
```c
EventGroupHandle_t xEventGroupCreate( void );
```
* 이벤트 그룹을 생성한다. 32비트 아키텍처에서는 총 24개의 비트(상위 8비트는 커널 제어용)를 사용자 플래그로 쓸 수 있다. 초기값은 모든 비트가 0이다.

### 5.2 `xEventGroupSetBits()` API
```c
EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup,
                                const EventBits_t uxBitsToSet );
```
* 지정한 비트를 `1`로 세팅한다. (기존 비트들은 그대로 유지되고 `uxBitsToSet` 비트들만 1로 켜짐).

### 5.3 `xEventGroupWaitBits()` API
```c
EventBits_t xEventGroupWaitBits( EventGroupHandle_t xEventGroup,
                                 const EventBits_t uxBitsToWaitFor,
                                 const BaseType_t xClearOnExit,
                                 const BaseType_t xWaitForAllBits,
                                 TickType_t xTicksToWait );
```
* `uxBitsToWaitFor`: 기다릴 비트들의 OR 마스크 (`EVENT_1_BIT | EVENT_2_BIT`).
* `xClearOnExit`: `pdTRUE`로 주면 조건을 충족하여 깨어날 때 해당 비트들을 자동으로 0으로 클리어해 준다.
* `xWaitForAllBits`:
  * `pdTRUE` (AND): 지정한 모든 비트가 다 1이 되어야 깨어남.
  * `pdFALSE` (OR): 지정한 비트 중 하나라도 1이 되면 깨어남.
* **반환값**: 대기가 풀린 순간의 이벤트 그룹 전체 비트 값.

---

## 6. 직접 구현 또는 관찰 준비

`source/tutorials/tutorial_15/source/tutorial_15.c` 파일을 열고 5개의 TODO를 완성한다.

### 6.1 TODO 1: 이벤트 그룹 생성 (`main` 함수)
87~90행: `xEventGroupCreate()`를 호출하여 `xEventGroup`에 대입한다.

<details>
<summary>TODO 1 해답 코드 보기</summary>

```c
    xEventGroup = xEventGroupCreate();
```
</details>

### 6.2 TODO 2 & 3: 이벤트 비트 세팅 (`prvEventGeneratorTask`)
118~139행: `EVENT_1_BIT`와 `EVENT_2_BIT`를 순차적으로 세팅한다.

<details>
<summary>TODO 2 & 3 해답 코드 보기</summary>

```c
        /* TODO 2 */
        xEventGroupSetBits( xEventGroup, EVENT_1_BIT );

        vTaskDelay( pdMS_TO_TICKS( 500 ) );

        /* TODO 3 */
        xEventGroupSetBits( xEventGroup, EVENT_2_BIT );
```
</details>

### 6.3 TODO 4 & 5: 이벤트 복합 대기 (`prvEventHandlerTask1 & 2`)
158~168행, 193~203행: 두 핸들러 task 모두 AND 조건(`pdTRUE`)과 자동 클리어(`pdTRUE`)로 대기한다.

<details>
<summary>TODO 4 & 5 해답 코드 보기</summary>

```c
        xEventBitsValue = xEventGroupWaitBits( xEventGroup,
                                               ( EVENT_1_BIT | EVENT_2_BIT ),
                                               pdTRUE,
                                               pdTRUE,
                                               portMAX_DELAY );
```
</details>

> 전체 완성 코드는 [`ko/guides/solutions/tutorial_15.c`](solutions/tutorial_15.c)에서 확인할 수 있다.

---

## 7. 빌드와 실행

저장소 루트에서 다음 명령을 실행한다:

```bash
# 1. CMake 빌드 구성 (Tutorial 15 지정)
cmake -B build -S source -DTUTORIAL=15

# 2. 컴파일 및 빌드
cmake --build build

# 3. 바이너리 실행
./build/freertos_example
```

* **종료 방법**: 터미널에서 **`Ctrl + C`**를 누른다.

---

## 8. 결과 해석

### 8.1 정상 실행 출력 화면
```text
Generating event 1...
Generating event 2...
Handler Task 2: Handling  event 1...
Handler Task 2: Handling  event 2...
Handler Task 1: Handling  event 1...
Handler Task 1: Handling  event 2...
Generated both the events...
```

### 8.2 출력 순서 정밀 분석
* `Generating event 1...` 직후에는 Handler들이 전혀 깨어나지 않는다! (AND 조건 미충족)
* 500ms 뒤 `Generating event 2...`가 호출되는 순간, **우선순위가 가장 높은 Handler Task 2가 먼저 깨어나 Event 1, 2를 처리**하고, 이어서 **Handler Task 1도 깨어나 Event 1, 2를 모두 처리**한다.
* 단 한 번의 이벤트 세팅으로 두 핸들러 task가 브로드캐스팅되어 모두 깨어났음을 증명한다.

---

## 9. 막혔을 때 (트러블슈팅)

| 증상 | 원인 | 해결 방법 |
| :--- | :--- | :--- |
| Event 1만 발생했는데 핸들러가 깨어남 | `xWaitForAllBits`에 `pdTRUE` 대신 `pdFALSE`(OR)를 전달했음 | 4번째 인자를 `pdTRUE`로 수정 |
| 한 핸들러만 깨어나고 다른 핸들러는 안 깨어남 | `xEventGroupWaitBits` 인자 불일치 | 두 핸들러의 대기 인자가 동일한지 확인 |

---

## 10. 혼자 확인하기 (자가 점검)

### 질문 1 (개념)
`xEventGroupWaitBits()`의 4번째 인자 `xWaitForAllBits`를 `pdFALSE`로 바꾸면 핸들러 task들은 언제 깨어나는가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: Event 1 또는 Event 2 중 **어느 하나라도 먼저 세팅되는 순간(OR 조건)** 즉시 깨어난다.
* **설명**: `pdFALSE`는 지정한 비트 마스크 중 단 하나라도 1이 되면 대기를 해제하라는 의미이기 때문이다.
</details>

### 질문 2 (세마포어와의 브로드캐스팅 비교)
만약 이 예제를 이벤트 그룹 대신 이진 세마포어로 구현하여 세마포어를 한 번 Give 했다면, Handler 1과 Handler 2가 모두 깨어날 수 있는가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: 절대 불가능하다.
* **이유**: 세마포어는 신호를 1회 획득(Take)하면 카운트가 0으로 소모되므로, 우선순위가 더 높은 Handler 2 혼자만 깨어나고 Handler 1은 계속 잠들어 있게 된다. 이벤트 그룹만이 모든 대기 task를 동시에 깨우는 브로드캐스팅을 지원한다.
</details>

---

## 11. 다음 학습

이벤트 그룹을 통한 이벤트 관리와 브로드캐스팅을 배웠다.  
이벤트 그룹의 또 다른 강력한 기능은, 여러 task가 각자의 초기화나 연산을 마친 뒤 **"모든 task가 약속 장소에 다 모일 때까지 아무도 출발하지 않는" 랑데부(장벽, Barrier) 동기화**다.

이제 **[Tutorial 16: event group을 통한 task 동기화 (Task Synchronization using Event Groups)](tutorial_16.md)**으로 넘어가서 `xEventGroupSync()`의 신비를 체험해 보자!

* **한국어 커널북 심화 읽기**:
  * [9.2장: event group의 특성](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch09.md)
  * [9.3장: event group을 이용한 이벤트 관리](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch09.md)
