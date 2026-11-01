# Tutorial 14: 우선순위 역전과 우선순위 상속 (Priority Inversion and Priority Inheritance)

이 단원에서는 공유 자원을 동기화할 때 발생할 수 있는 가장 치명적인 실시간 시스템 결함인 **우선순위 역전(Priority Inversion)**과, FreeRTOS 뮤텍스가 이를 방어하기 위해 제공하는 **우선순위 상속(Priority Inheritance)** 메커니즘을 관찰한다.

---

## 1. 이번 실습에서 할 일

* **해결하려는 문제**: 낮은 우선순위 task가 점유한 뮤텍스 때문에 높은 우선순위 task가 실행을 차단당하는 역전 현상의 위험성을 이해한다.
* **학습 목표**:
  1. 우선순위 역전(Priority Inversion)과 유계 없는 역전(Unbounded Priority Inversion)의 발생 경로를 설명할 수 있다.
  2. FreeRTOS 뮤텍스에 내장된 **우선순위 상속(Priority Inheritance)**에 의해 하위 task의 우선순위가 일시적으로 승격(`0 → 1`)되는 과정을 직접 확인한다.
  3. `uxTaskPriorityGet()` API를 통해 task의 현재 유효 우선순위를 동적으로 모니터링한다.

---

## 2. 시작 전 확인

### 2.1 개념 복습
* **우선순위 역전 (Priority Inversion)**: 높은 우선순위 task(H)가 실행되어야 함에도 불구하고, 낮은 우선순위 task(L)가 쥔 자원을 기다리느라 엉뚱한 중간 우선순위 task(M)가 먼저 실행되어 시스템의 우선순위 질서가 뒤집히는 현상이다.
* **유명한 역사적 사례**: 1997년 NASA의 화성 탐사선 패스파인더(Mars Pathfinder)가 착륙 후 원인 모를 시스템 재부팅을 반복했던 사건의 원인이 바로 이 우선순위 역전 버그였다.

### 2.2 확인할 소스 및 설정 파일
* 실습 코드: [`source/tutorials/tutorial_14/source/tutorial_14.c`](../../source/tutorials/tutorial_14/source/tutorial_14.c)
* 설정 파일: [`source/tutorials/tutorial_14/include/FreeRTOSConfig.h`](../../source/tutorials/tutorial_14/include/FreeRTOSConfig.h)

> **실습 형태**: 이 단원은 코드가 사전에 완성되어 있는 **관찰 및 동적 상태 분석 실습**이다.

---

## 3. 개념 이해

### 3.1 악명 높은 3단계 우선순위 역전 시나리오
태스크 3개가 있다고 가정해 보자:
* **H (High)**: 긴급 제어 작업 (우선순위 3)
* **M (Medium)**: 긴 연산 작업 (우선순위 2)
* **L (Low)**: 센서 로깅 작업 (우선순위 1)

```mermaid
sequenceDiagram
    autonumber
    participant L as Low Task (우선순위 1)
    participant M as Medium Task (우선순위 2)
    participant H as High Task (우선순위 3)
    participant Res as 공유 자원 (Mutex)

    L->>Res: 1. Low가 자원 획득 (잠금)
    Note over H: High가 깨어나서 자원을 요구함
    H->>Res: 2. 자원이 잠겨있어 High가 Blocked 대기!
    Note over M: 이때 자원과 무관한 Medium Task가 깨어남!
    M->>M: 3. Medium이 Low보다 우선순위가 높으므로 Low를 밀어내고 CPU 장악!
    Note over H,L: Low가 CPU를 못 받으니 자원을 반납 못함!<br/>결과적으로 가장 시급한 High가 무관한 Medium 때문에 무한정 멈춤!<br/>(Unbounded Priority Inversion 발생!)
```

### 3.2 해결책: 우선순위 상속 (Priority Inheritance)
FreeRTOS의 Mutex는 이 문제를 방어하기 위해 **우선순위 상속** 알고리즘을 기본 내장하고 있다:

> **"높은 우선순위 task(H)가 자원을 기다리며 Blocked 상태에 빠지면, 해당 자원을 쥐고 있는 낮은 우선순위 task(L)의 우선순위를 일시적으로 H의 우선순위로 끌어올린다!"**

* **효과**: Low Task의 우선순위가 순간적으로 High와 같아지므로, 중간 우선순위인 Medium Task가 새치기(선점)하지 못한다. Low Task가 방해받지 않고 빠르게 자원을 쓰고 반납하게 되며, 반납 즉시 원래 우선순위로 복귀하고 High Task가 자원을 획득한다.

---

## 4. 실행 흐름 읽기

`tutorial_14.c`에서는 상속 동작을 증명하기 위해 Low Task가 자신의 우선순위를 계속 출력한다:

```mermaid
sequenceDiagram
    autonumber
    participant High as High Task (우선순위 1)
    participant Mutex as xMutex
    participant Low as Low Task (우선순위 0)

    Note over High: High는 시작하자마자 vTaskDelay(1초)로 잠듦
    Low->>Mutex: 1. Low가 xMutex 획득 성공 (소유)
    Low->>Low: 2. "Task priority is 0" 출력 (기본 우선순위 0 확인)
    Low->>Low: 3. 1초 지연 진입
    
    Note over High: 1초 후 High Task가 깨어남!
    High->>Mutex: 4. High가 xMutex 획득 시도 → Low가 쥐고 있으므로 Blocked 대기!
    Note over Mutex,Low: 커널이 즉시 Low의 우선순위를 0에서 1로 강제 승격! (우선순위 상속)
    
    Note over Low: 1초 후 Low Task가 깨어나 다음 루프 실행
    Low->>Low: 5. "Task priority is 1" 출력! (상속받아 승격된 우선순위 증명)
```

---

## 5. 코드와 API 이해

`source/tutorials/tutorial_14/source/tutorial_14.c`의 핵심 코드:

### 5.1 High Task
```c
static void prvHighPriorityTask( void * pvParams )
{
    for( ;; )
    {
        /* 1. Low Task가 먼저 뮤텍스를 잡을 수 있도록 1초 잠듦 */
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );

        /* 2. 깨어나서 뮤텍스 요구 → Low가 쥐고 있으므로 Blocked 됨! */
        xSemaphoreTake( xMutex, portMAX_DELAY );
    }
}
```

### 5.2 Low Task
```c
static void prvLowPriorityTask( void * pvParams )
{
    /* 1. 시작하자마자 뮤텍스를 먼저 획득 */
    xSemaphoreTake( xMutex, portMAX_DELAY );

    for( ;; )
    {
        /* 2. 자신의 현재 유효 우선순위를 출력 */
        fprintf( stderr, "Task priority is %lu\r\n", uxTaskPriorityGet( NULL ) );

        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}
```
* `uxTaskPriorityGet( NULL )`: `NULL`을 넘기면 현재 호출 중인 자기 자신 task의 우선순위를 반환한다.

---

## 6. 직접 구현 또는 관찰 준비

* **실행 전 예측 질문**:
  1. Low Task의 첫 번째 출력은 어떤 우선순위 숫자가 찍힐까?
  2. High Task가 뮤텍스를 얻으려고 시도하여 대기 상태에 들어간 뒤, Low Task의 두 번째 출력은 어떤 숫자가 찍힐까?

---

## 7. 빌드와 실행

저장소 루트에서 다음 명령을 실행한다:

```bash
# 1. CMake 빌드 구성 (Tutorial 14 지정)
cmake -B build -S source -DTUTORIAL=14

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
Task priority is 0
Task priority is 1
Task priority is 1
Task priority is 1
Task priority is 1
```

### 8.2 출력 결과 정밀 분석
* **첫 번째 줄 (`Task priority is 0`)**:
  * High Task가 아직 `vTaskDelay(1000)`로 잠들어 있는 동안, Low Task는 자신의 본래 기본 우선순위인 `0`으로 실행되었다.
* **두 번째 줄 이후 (`Task priority is 1`)**:
  * 1초 뒤 High Task가 깨어나 `xMutex`를 잡으려다 실패하여 Blocked 상태로 대기하자, **커널이 Low Task의 우선순위를 High의 우선순위인 `1`로 즉시 승격(상속)**시켰다!
  * 따라서 `uxTaskPriorityGet(NULL)` 결과가 `1`로 바뀌어 출력되는 것이다.

---

## 9. 막혔을 때 (트러블슈팅)

| 증상 | 원인 | 해결 방법 |
| :--- | :--- | :--- |
| 계속 `0`만 찍히는 현상 | High Task가 뮤텍스를 잡지 못함 | `tutorial_14.c` 원본 코드가 수정되지 않았는지 확인 |
| 이전 빌드 결과가 실행됨 | CMake 캐시 미갱신 | `rm -rf build` 후 다시 빌드 |

---

## 10. 혼자 확인하기 (자가 점검)

### 질문 1 (개념)
우선순위 상속(Priority Inheritance)이 상호 배제 뮤텍스(Mutex)에는 내장되어 있지만, 이진 세마포어(Binary Semaphore)에는 내장될 수 없는 이유는 무엇인가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: 이진 세마포어에는 **소유권(Ownership)의 개념이 없기 때문**이다.
* **이유**: 세마포어는 "누가 신호를 쥐고 있는지" 커널이 알 수 없고 알 필요도 없다. 반면 뮤텍스는 어떤 task가 `Take`했는지 TCB 소유자 포인터가 명확히 기록되므로, 상위 task가 대기할 때 "어느 task의 우선순위를 올려주어야 하는지" 대상을 정확히 특정할 수 있다.
</details>

### 질문 2 (한계 인식)
우선순위 상속은 우선순위 역전 문제를 "완전히 없애주는 것"인가, 아니면 "피해를 최소화하는 것"인가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: **피해를 최소화하는 완화책(Mitigation)**이다.
* **이유**: 하위 task가 자원을 반납할 때까지 상위 task가 기다려야 한다는 본질적인 '역전' 자체는 자원 독점 규칙상 피할 수 없다. 우선순위 상속은 무관한 중간 우선순위(Medium) task들의 새치기를 차단하여, 역전 상태가 지속되는 시간을 '하위 task가 자원을 다 쓰고 반납하는 최소 시간'으로 묶어두는(Bounded) 역할을 한다.
</details>

---

## 11. 다음 학습

큐, 타이머, 세마포어, 뮤텍스, 우선순위 상속까지 동기화와 자원 관리의 핵심을 완벽히 정복했다!  
하지만 지금까지의 동기화는 "단 하나의 사건(Event)"만 다루었다. 만약 "여러 개의 복합 조건이 동시에 만족되었을 때(예: 배터리 정상 AND SD카드 장착 AND 네트워크 연결)" task를 깨우고 싶다면 어떻게 해야 할까?

이제 **[Tutorial 15: event group을 통한 이벤트 관리 (Event Management using Event Groups)](tutorial_15.md)**으로 넘어가서, 24개 이상의 비트 플래그를 한 번에 다루는 **Event Group**을 배워보자!

* **한국어 커널북 심화 읽기**:
  * [8.3.2장: 우선순위 역전 (Priority Inversion)](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch08.md)
  * [8.3.3장: 우선순위 상속 (Priority Inheritance)](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch08.md)
