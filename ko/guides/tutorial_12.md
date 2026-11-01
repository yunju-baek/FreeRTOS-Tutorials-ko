# Tutorial 12: semaphore (Semaphore)

이 단원에서는 특정 이벤트의 발생을 알리고 두 task 간의 실행 타이밍을 맞추는 **이진 세마포어(Binary Semaphore)**의 신호 동기화(Synchronization) 메커니즘을 배운다.

---

## 1. 이번 실습에서 할 일

* **해결하려는 문제**: 데이터 전송 없이 "어떤 사건이 일어났다!"는 신호(Flag)만을 전달하여, 대기 중이던 task를 즉시 깨워 실행시키는 동기화 기법을 익힌다.
* **학습 목표**:
  1. 이진 세마포어(Binary Semaphore)의 생성(`xSemaphoreCreateBinary`), 획득(`Take`), 반환/신호(`Give`) 동작을 구현한다.
  2. 세마포어가 비어있을 때 상위 task가 Blocked 상태로 대기하고, 하위 task가 신호를 보낼 때 즉시 선점되어 깨어나는 과정을 관찰한다.
  3. 세마포어(동기화/신호)와 뮤텍스(자원 보호/소유권)의 개념적 차이를 미리 이해한다.

---

## 2. 시작 전 확인

### 2.1 개념 복습
* **세마포어(Semaphore)의 어원**: 철도 신호기(깃발 신호)에서 유래했다. 깃발이 올라가 있으면(자원 있음 또는 이벤트 발생) 지나갈 수 있고, 깃발이 내려가 있으면(자원 없음) 멈춰서 기다려야 한다.
* **이진 세마포어 (Binary Semaphore)**: 세마포어 내부의 카운트 값이 오직 `0` 또는 `1`만 가질 수 있는 세마포어다.
* **소유권 없음(No Ownership)**: 세마포어는 신호 전달용이므로, **A task가 Take(대기)하고 B task가 Give(신호 발송)**할 수 있다. (소유한 자만 반납할 수 있는 Mutex와의 결정적 차이점!)

### 2.2 확인할 소스 및 설정 파일
* 실습 코드: [`source/tutorials/tutorial_12/source/tutorial_12.c`](../../source/tutorials/tutorial_12/source/tutorial_12.c)
* 설정 파일: [`source/tutorials/tutorial_12/include/FreeRTOSConfig.h`](../../source/tutorials/tutorial_12/include/FreeRTOSConfig.h)
* 참조 해답: [`ko/guides/solutions/tutorial_12.c`](solutions/tutorial_12.c)

> **실습 형태**: `tutorial_12.c` 내부의 **`TODO 1`, `TODO 2`, `TODO 3`을 완성하는 구현 실습**이다.

---

## 3. 개념 이해

### 3.1 이진 세마포어의 신호 전달 흐름

```mermaid
flowchart LR
    subgraph S["세마포어 내부 카운트"]
        C["Count: 0 (비어있음)"]
    end

    subgraph T1["Task 1 (우선순위 1, 소비자)"]
        Take["xSemaphoreTake(..., portMAX_DELAY)<br/>카운트가 0이므로 Blocked 대기!"]
    end

    subgraph T2["Task 2 (우선순위 0, 생산자)"]
        Give["xSemaphoreGive()<br/>신호 발송! (Count 0 → 1)"]
    end

    Take -.대기.-> C
    Give -.신호 주입.-> C
    C -->|카운트가 1이 되는 순간 즉시 깨움!| Take
```

1. `xSemaphoreCreateBinary()`로 세마포어를 생성하면 초기 상태는 **비어있는 상태(Count = 0)**다.
2. 높은 우선순위의 Task 1이 먼저 실행되어 `xSemaphoreTake()`를 호출하지만, 카운트가 0이므로 즉시 **Blocked 상태**로 들어간다.
3. 이제 낮은 우선순위의 Task 2가 실행되어 어떤 작업을 마친 뒤 `xSemaphoreGive()`를 호출한다.
4. 세마포어에 신호가 들어오는 순간, 커널은 대기 중이던 높은 우선순위의 Task 1을 깨우고, **Task 1이 Task 2를 즉시 선점(Preemption)**하여 실행된다!

---

## 4. 실행 흐름 읽기

```mermaid
sequenceDiagram
    autonumber
    participant Sched as 스케줄러
    participant T1 as Task 1 (우선순위 1)
    participant Sem as Binary Semaphore
    participant T2 as Task 2 (우선순위 0)

    Sched->>T1: 우선순위가 높은 Task 1 먼저 실행
    T1->>Sem: xSemaphoreTake(portMAX_DELAY) 호출 (TODO 2)
    Note over T1,Sem: 세마포어가 비어있으므로 Task 1은 Blocked 대기 진입!
    Sched->>T2: CPU가 Task 2로 넘어감
    T2->>T2: "Task 2 - signalling Task 1..." 출력
    T2->>Sem: xSemaphoreGive() 호출 (TODO 3)
    Note over Sem,T1: 세마포어가 채워지며 상위 task인 Task 1이 즉시 깨어남!
    Note over T1,T2: Task 1이 Task 2를 선점(Preempt)하여 즉시 실행!
    T1->>T1: "Task 1 is running..." 출력
    T1->>Sem: 다음 xSemaphoreTake() 호출 → 다시 Blocked 대기!
    Note over T1,T2: Task 1이 다시 잠들었으므로 CPU가 Task 2로 복귀
    T2->>T2: "Task 2 is running..." 출력 후 1000ms 지연 진입
```

---

## 5. 코드와 API 이해

### 5.1 `xSemaphoreCreateBinary()` API
```c
SemaphoreHandle_t xSemaphoreCreateBinary( void );
```
* 이진 세마포어를 동적으로 생성한다.
* 초기 카운트는 `0`이다. (생성 직후 바로 Take하면 무조건 블록됨).

### 5.2 `xSemaphoreTake()` API
```c
BaseType_t xSemaphoreTake( SemaphoreHandle_t xSemaphore, TickType_t xBlockTime );
```
* 세마포어의 카운트를 1 감소시키려고 시도한다. 카운트가 0이면 `xBlockTime` 동안 대기한다.

### 5.3 `xSemaphoreGive()` API
```c
BaseType_t xSemaphoreGive( SemaphoreHandle_t xSemaphore );
```
* 세마포어의 카운트를 1 증가시킨다. 대기 중이던 task가 있으면 즉시 깨어난다.

---

## 6. 직접 구현 또는 관찰 준비

`source/tutorials/tutorial_12/source/tutorial_12.c` 파일을 열고 세 개의 TODO를 완성한다.

### 6.1 TODO 1: 이진 세마포어 생성 (`main` 함수)
71~74행 주석: `xSemaphoreCreateBinary()`를 호출하여 `xSemaphore`에 대입한다.

<details>
<summary>TODO 1 해답 코드 보기</summary>

```c
    xSemaphore = xSemaphoreCreateBinary();
```
</details>

### 6.2 TODO 2: 세마포어 획득/대기 (`prvTask1`)
100~105행 주석: `portMAX_DELAY`로 세마포어를 대기한다.

<details>
<summary>TODO 2 해답 코드 보기</summary>

```c
        xSemaphoreTake( xSemaphore, portMAX_DELAY );
```
</details>

### 6.3 TODO 3: 세마포어 신호 전달 (`prvTask2`)
125~131행 주석: `xSemaphoreGive()`를 호출하여 반환값을 `xSemaphoreGiveResult`에 대입한다.

<details>
<summary>TODO 3 해답 코드 보기</summary>

```c
        xSemaphoreGiveResult = xSemaphoreGive( xSemaphore );
```
</details>

> 전체 완성 코드는 [`ko/guides/solutions/tutorial_12.c`](solutions/tutorial_12.c)에서 확인할 수 있다.

---

## 7. 빌드와 실행

저장소 루트에서 다음 명령을 실행한다:

```bash
# 1. CMake 빌드 구성 (Tutorial 12 지정)
cmake -B build -S source -DTUTORIAL=12

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
Task 2 - signalling Task 1...
Task 1 is running...
Task 2 is running...
Task 2 - signalling Task 1...
Task 1 is running...
Task 2 is running...
```

### 8.2 출력 순서 정밀 분석 (매우 중요!)
출력 순서를 주의 깊게 보자:
1. `Task 2 - signalling Task 1...`
2. **`Task 1 is running...`** (Task 2가 아직 안 끝났는데 새치기!)
3. `Task 2 is running...`

* **왜 이런 순서가 나오는가?**:
  * Task 2의 코드 순서는 분명히 `signalling` 출력 → `xSemaphoreGive` → `Task 2 is running...` 출력이다.
  * 그런데 `xSemaphoreGive`가 불리는 바로 그 순간, 대기하던 **우선순위가 더 높은 Task 1이 즉시 깨어나 Task 2의 실행권을 빼앗아버리기(선점) 때문**이다!
  * 따라서 Task 1의 출력이 먼저 나오고, Task 1이 다시 대기하러 들어간 뒤에야 비로소 Task 2의 남은 한 줄(`Task 2 is running...`)이 출력된다.

---

## 9. 막혔을 때 (트러블슈팅)

| 증상 | 원인 | 해결 방법 |
| :--- | :--- | :--- |
| `Task 1 is running...`이 아예 안 나옴 | TODO 3에서 `xSemaphoreGive` 호출 누락 | Task 2에서 신호를 정상 발송하고 있는지 확인 |
| `assertion "xSemaphore != NULL" failed` | TODO 1에서 세마포어 생성 누락 | `xSemaphore = xSemaphoreCreateBinary();` 확인 |

---

## 10. 혼자 확인하기 (자가 점검)

### 질문 1 (동작 추적)
로그에서 `Task 1 is running...`이 `Task 2 is running...`보다 **먼저** 출력되는 이유를 선점(Preemption)의 관점에서 설명하라.

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: Task 1(우선순위 1)이 Task 2(우선순위 0)보다 우선순위가 더 높기 때문이다.
* **설명**: Task 2가 `xSemaphoreGive()`를 호출하여 세마포어를 채우는 순간, 커널은 대기 중이던 상위 우선순위 Task 1을 즉시 Ready로 깨운다. 선점형 스케줄러는 실행 가능한 상위 task가 나타나면 현재 하위 task를 즉시 중단시키고 상위 task에게 CPU를 넘기므로, Task 2의 다음 줄이 실행되기 전에 Task 1이 먼저 실행되는 것이다.
</details>

### 질문 2 (세마포어와 큐의 관계)
FreeRTOS 내부 구현 관점에서, 이진 세마포어는 사실상 어떤 형태의 큐와 동일한가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: **길이가 1이고 항목 크기가 0바이트인 큐 (`xQueueCreate( 1, 0 )`)**와 동일하다.
* **설명**: 세마포어는 실제 데이터를 주고받을 필요 없이 "신호가 채워졌는가(1), 비어있는가(0)"라는 상태 플래그만 관리하면 되기 때문이다.
</details>

---

## 11. 다음 학습

세마포어를 통한 동기화를 배웠다.  
그런데 세마포어를 "공유 자원 보호(상호 배제)" 목적으로 쓰면 어떻게 될까? 누가 잠갔는지 소유권이 없어 엉뚱한 task가 신호를 날릴 수도 있고, 악명 높은 '우선순위 역전' 버그에 취약해진다.

이제 **[Tutorial 13: mutex (Mutex)](tutorial_13.md)**으로 넘어가서, 공유 자원 독점 접근을 위해 고안된 **소유권(Ownership)**을 가진 동기화 객체인 **Mutex(뮤텍스)**를 배워보자!

* **한국어 커널북 심화 읽기**:
  * [7.4장: binary semaphore를 통한 동기화](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch07.md)
