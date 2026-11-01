# Tutorial 13: mutex (Mutex)

이 단원에서는 공유 자원(Shared Resource)에 여러 task가 동시에 접근할 때 발생하는 데이터 오염을 막고, 오직 한 번에 하나의 task만 안전하게 접근하도록 보장하는 **Mutex(뮤텍스, 상호 배제)**를 배운다.

---

## 1. 이번 실습에서 할 일

* **해결하려는 문제**: 프린터, 직렬 통신 포트, 공유 메모리 등 하나의 task만 독점해야 하는 자원에 여러 task가 동시 접근하는 충돌을 방지한다.
* **학습 목표**:
  1. 상호 배제(Mutual Exclusion)의 개념과 Mutex의 역할을 설명할 수 있다.
  2. `xSemaphoreCreateMutex()`로 뮤텍스를 생성하고, `Take`와 `Give`의 대칭적 호출 구조를 구현한다.
  3. 세마포어(신호 전달, 소유권 없음)와 뮤텍스(자원 보호, **소유권 있음**)의 결정적 차이를 명확히 이해한다.

---

## 2. 시작 전 확인

### 2.1 개념 복습
* **임계 구역 (Critical Section)**: 공유 자원에 접근하는 코드 영역으로, 반드시 한 번에 하나의 실행 흐름만 진입해야 하는 보호 구역이다.
* **소유권 (Ownership)**:
  * Mutex를 `Take`(잠금)한 task는 해당 Mutex의 **소유자(Owner)**가 된다.
  * **자신이 잠근 Mutex는 반드시 자기 자신만이 `Give`(잠금 해제)할 수 있다.** (다른 task가 엉뚱하게 가로채서 잠금을 푸는 행위는 금지된다.)

### 2.2 확인할 소스 및 설정 파일
* 실습 코드: [`source/tutorials/tutorial_13/source/tutorial_13.c`](../../source/tutorials/tutorial_13/source/tutorial_13.c)
* 설정 파일: [`source/tutorials/tutorial_13/include/FreeRTOSConfig.h`](../../source/tutorials/tutorial_13/include/FreeRTOSConfig.h)
  * `configUSE_MUTEXES`가 `1`로 설정되어 있어야 한다.
* 참조 해답: [`ko/guides/solutions/tutorial_13.c`](solutions/tutorial_13.c)

> **실습 형태**: `tutorial_13.c` 내부의 **`TODO 1`부터 `TODO 5`까지 5개의 빈칸을 완성하는 구현 실습**이다.

---

## 3. 개념 이해

### 3.1 세마포어 vs 뮤텍스 비교 (혼동 종결)

| 비교 항목 | 이진 세마포어 (Binary Semaphore) | 뮤텍스 (Mutex) |
| :--- | :--- | :--- |
| **주요 목적** | **이벤트 신호 전달 및 동기화** ("작업 끝났다!") | **공유 자원 상호 배제** ("내가 쓰는 중이니 건들지 마!") |
| **소유권 (Ownership)**| **없음** (A task가 기다리고 B task가 신호 발송 가능) | **있음** (A task가 잠갔으면 오직 A task만 풀어야 함) |
| **초기 상태** | 통상 **비어있음 (0)** (신호 기다림) | **사용 가능 (1)** (누구든 먼저 와서 잠글 수 있음) |
| **우선순위 상속** | 없음 (우선순위 역전 버그에 취약) | **내장됨** (우선순위 역전 문제 자동 완화) |

```mermaid
flowchart LR
    subgraph MutexPattern["뮤텍스의 표준 사용 패턴"]
        M1["1. xSemaphoreTake(xMutex) - 자원 잠금/소유"] --> M2["2. 임계 구역 진입 (공유 자원 독점 사용)"]
        M2 --> M3["3. xSemaphoreGive(xMutex) - 자원 잠금 해제/반납"]
    end
```

---

## 4. 실행 흐름 읽기

```mermaid
sequenceDiagram
    autonumber
    participant T1 as Task 1
    participant Mutex as xMutex (자원 잠금)
    participant T2 as Task 2

    Note over Mutex: 초기 상태: 사용 가능 (Available)
    T1->>Mutex: xSemaphoreTake() 성공 (소유권 획득)
    Note over T1: Task 1이 임계 구역 실행 중...
    T2->>Mutex: xSemaphoreTake() 시도!
    Note over T2,Mutex: 이미 Task 1이 잠갔으므로 Task 2는 Blocked 대기!
    T1->>Mutex: xSemaphoreGive() 호출 (자원 반납)
    Note over Mutex,T2: 반납되는 즉시 대기하던 Task 2가 소유권을 얻고 실행!
    Note over T2: Task 2가 임계 구역 실행...
    T2->>Mutex: xSemaphoreGive() 반납
```

---

## 5. 코드와 API 이해

### 5.1 `xSemaphoreCreateMutex()` API
```c
SemaphoreHandle_t xSemaphoreCreateMutex( void );
```
* 뮤텍스를 생성한다. 초기 상태는 **사용 가능(Available, 토큰이 들어있는 상태)**이다.
* 따라서 가장 먼저 `Take`를 호출하는 task는 블록되지 않고 즉시 통과한다.

### 5.2 `Take`와 `Give`의 대칭성
```c
xSemaphoreTake( xMutex, portMAX_DELAY ); /* 1. 진입 전 잠금 */
{
    /* 2. 임계 구역 (공유 자원 접근) */
    fprintf( stderr, "Task 1 is running...\r\n" );
}
xSemaphoreGive( xMutex );                /* 3. 진출 후 반납 */
```
* 자원을 다 썼으면 지체 없이 `Give`를 해주어야 다른 task가 실행될 수 있다.

---

## 6. 직접 구현 또는 관찰 준비

`source/tutorials/tutorial_13/source/tutorial_13.c` 파일을 열고 5개의 TODO를 완성한다.

### 6.1 TODO 1: 뮤텍스 생성 (`main` 함수)
71~74행: `xSemaphoreCreateMutex()`를 호출하여 `xMutex`에 대입한다.

<details>
<summary>TODO 1 해답 코드 보기</summary>

```c
    xMutex = xSemaphoreCreateMutex();
```
</details>

### 6.2 TODO 2 & 3: Task 1의 Take와 Give
101~120행: 임계 구역 전후로 `xSemaphoreTake`와 `xSemaphoreGive`를 작성한다.

<details>
<summary>TODO 2 & 3 해답 코드 보기</summary>

```c
        xSemaphoreTake( xMutex, portMAX_DELAY );

        {
            fprintf( stderr, "Task 1 is running...\r\n" );
        }

        xSemaphoreGiveResult = xSemaphoreGive( xMutex );
```
</details>

### 6.3 TODO 4 & 5: Task 2의 Take와 Give
138~157행: Task 2에도 동일하게 뮤텍스 획득과 반납을 작성한다.

<details>
<summary>TODO 4 & 5 해답 코드 보기</summary>

```c
        xSemaphoreTake( xMutex, portMAX_DELAY );

        {
            fprintf( stderr, "Task 2 is running...\r\n" );
        }

        xSemaphoreGiveResult = xSemaphoreGive( xMutex );
```
</details>

> 전체 완성 코드는 [`ko/guides/solutions/tutorial_13.c`](solutions/tutorial_13.c)에서 확인할 수 있다.

---

## 7. 빌드와 실행

저장소 루트에서 다음 명령을 실행한다:

```bash
# 1. CMake 빌드 구성 (Tutorial 13 지정)
cmake -B build -S source -DTUTORIAL=13

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
Task 1 is running...
Task 2 is running...
Task 1 is running...
Task 2 is running...
```

### 8.2 판정 기준 및 해석
* 두 task가 1초 지연(`vTaskDelay(1000)`)을 두고 번갈아 가며 한 번씩 실행된다.
* 만약 하나의 task가 뮤텍스를 쥐고 오랫동안 놓아주지 않는다면, 다른 task는 그 시간 동안 `xSemaphoreTake`에서 안전하게 대기 상태(Blocked)를 유지한다.

---

## 9. 막혔을 때 (트러블슈팅)

| 증상 | 원인 | 해결 방법 |
| :--- | :--- | :--- |
| `assertion "xSemaphoreGiveResult == pdPASS" failed` | 잠그지 않은 뮤텍스를 풀려고 시도했거나 소유자가 아님 | `Take`와 `Give` 쌍이 정확히 일치하는지 확인 |
| `Task 1`만 한 번 출력되고 멈춤 | `xSemaphoreGive` 호출이 누락되어 자원이 영원히 잠김 | 임계 구역 직후 반드시 `Give`를 호출했는지 확인 |

---

## 10. 혼자 확인하기 (자가 점검)

### 질문 1 (비교 분석)
Tutorial 12의 이진 세마포어와 Tutorial 13의 뮤텍스는 둘 다 `xSemaphoreTake()`와 `xSemaphoreGive()` 함수를 쓰는데, 생성 직후의 **초기 카운트 값**은 어떻게 다른가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**:
  * **이진 세마포어**: 초기 카운트가 **`0` (비어있음)**이다. 신호가 발생할 때까지 먼저 기다려야 하기 때문이다.
  * **뮤텍스**: 초기 카운트가 **`1` (사용 가능)**이다. 처음 접근하는 task가 즉시 자원을 획득하고 들어갈 수 있어야 하기 때문이다.
</details>

### 질문 2 (소유권 원칙)
Task A가 뮤텍스를 획득하여 자원을 사용 중인데, 인터럽트 핸들러(ISR)나 다른 Task B에서 `xSemaphoreGive( xMutex );`를 강제로 호출하면 어떻게 되는가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: 에러가 반환되거나 커널의 assert에 걸린다.
* **이유**: 뮤텍스는 **소유권(Ownership)**이 있는 객체이므로, 오직 그 뮤텍스를 획득한 당사자 task만이 반납할 수 있다. 특히 ISR은 task가 아니므로 뮤텍스를 소유할 수 없어 ISR 내부에서는 뮤텍스 API 호출이 전면 금지된다.
</details>

---

## 11. 다음 학습

뮤텍스를 쓰다 보면 실시간 시스템에서 가장 위험한 버그 중 하나인 **우선순위 역전(Priority Inversion)** 문제와 마주치게 된다.  
화성 탐사선 패스파인더(Mars Pathfinder)를 위기에 빠뜨렸던 바로 그 유명한 버그다!

이제 **[Tutorial 14: 우선순위 역전과 우선순위 상속 (Priority Inversion and Priority Inheritance)](tutorial_14.md)**으로 넘어가서, FreeRTOS 뮤텍스가 어떻게 이 문제를 스스로 해결하는지 그 메커니즘을 파헤쳐 보자!

* **한국어 커널북 심화 읽기**:
  * [8.3장: mutex (상호 배제)](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch08.md)
