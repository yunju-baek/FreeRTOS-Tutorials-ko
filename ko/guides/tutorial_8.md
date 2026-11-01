# Tutorial 8: 협력형 스케줄링 (Co-operative Scheduling)

[학습 목차](README.md) · [공통 준비](getting-started.md) · [공식 튜토리얼 번역](../docs/tutorial_8/README.md) · [이전 단원](tutorial_7.md)

이 단원에서는 선점 기능을 완전히 비활성화하는 **협력형 스케줄링(Co-operative Scheduling, `configUSE_PREEMPTION 0`)**의 원리와 특징을 배우고, 더 높은 우선순위 task가 생성되어도 현재 task가 양보하지 않으면 실행되지 못하는 현상을 직접 확인한다.

---

## 1. 이번 실습에서 할 일

* **해결하려는 문제**: 선점(Preemption)을 끈 이 예제에서 task가 실행권을 넘기는 규칙을 확인하고, 실시간 운영체제에서 선점형과 협력형의 트레이드오프를 이해한다.
* **학습 목표**:
  1. `configUSE_PREEMPTION 0`으로 설정된 협력형 스케줄러의 동작 메커니즘을 설명할 수 있다.
  2. 우선순위가 낮은 task가 실행 중에 **더 높은 우선순위 task를 동적으로 생성해도 선점되지 않고 계속 실행되는 과정**을 관찰한다.
  3. 협력형 스케줄링에서 문맥 전환이 발생하기 위해 필요한 2가지 조건(`taskYIELD()` 또는 Blocked 상태 진입)을 이해한다.

---

## 2. 시작 전 확인

### 2.1 개념 비교 복습
* **선점형 (T04, T06, T07)**: 더 높은 우선순위 task가 Ready가 되거나 시분할 조건이 맞으면 커널이 task를 전환한다.
* **협력형 (T08, 현재 단원)**: 높은 우선순위 task가 Ready가 되었다는 이유만으로 자동 선점하지 않는다. 이 예제에서는 실행 중인 task의 양보·대기가 필요하다.

### 2.2 확인할 소스 및 설정 파일
* 실습 코드: [`source/tutorials/tutorial_8/source/tutorial_8.c`](../../source/tutorials/tutorial_8/source/tutorial_8.c)
* 설정 파일: [`source/tutorials/tutorial_8/include/FreeRTOSConfig.h`](../../source/tutorials/tutorial_8/include/FreeRTOSConfig.h)

> **실습 형태**: 사전 완성된 코드를 통해 협력형 스케줄러의 동작을 분석하는 **관찰 및 비교 실습**이다.

---

## 3. 개념 이해

### 3.1 협력형 스케줄링이란?
협력형 스케줄링에서는 task가 필요한 시점에 실행권을 넘기도록 설계한다. 한 task가 계속 실행하면 다른 task의 처리가 지연된다. 인터럽트까지 꺼지는 것은 아니며 ISR의 명시적 전환 요청은 자동 선점 설정과 별도로 보아야 한다.

```mermaid
flowchart TD
    subgraph Cooperative["협력형 스케줄링 (configUSE_PREEMPTION = 0)"]
        direction TB
        C1["높은 우선순위 task가 Ready가 되어도 자동 선점하지 않음"]
        C2["이 예제에서 사용하는 task 전환 경로:"]
        C3["1. 스스로 taskYIELD()를 호출하여 양보할 때"]
        C4["2. 스스로 vTaskDelay()나 큐 대기 등으로 Blocked 상태에 들어갈 때"]
        C1 --> C2 --> C3
        C2 --> C4
    end
```

### 3.2 협력형 스케줄링의 장단점
* **장점**:
  * task 간 전환 지점을 파악하기 쉽다. 다만 인터럽트와 공유하는 데이터나 대기를 포함한 코드에는 여전히 보호가 필요하다.
  * 문맥 전환 횟수가 적어 시스템 오버헤드가 적다.
* **설계상 주의점**:
  * 높은 우선순위 task가 준비되어도 현재 task가 양보하지 않으면 응답이 지연된다. 실시간 요구를 만족하려면 양보까지 걸리는 최대 시간 등을 분석해야 하며, 이 출력 실습만으로 실시간 적합성을 판단할 수 없다.

---

## 4. 실행 흐름 읽기

`tutorial_8.c`에서는 매우 흥미로운 실험이 진행된다:
1. `main()`은 우선순위 `0`(`tskIDLE_PRIORITY`)인 **Task 1**만 생성하고 스케줄러를 시작한다.
2. 실행을 시작한 Task 1은 자신의 내부에서 **우선순위가 더 높은(`tskIDLE_PRIORITY + 1` = 우선순위 1) Task 2를 동적으로 생성**한다

```mermaid
sequenceDiagram
    autonumber
    participant Sched as 협력형 스케줄러 (configUSE_PREEMPTION = 0)
    participant T1 as Task 1 (우선순위 0)
    participant T2 as Task 2 (우선순위 1, 상위 우선순위)

    Sched->>T1: Task 1 실행 시작 (Running)
    T1->>Sched: xTaskCreate(Task 2, 우선순위 1) 호출
    Note over Sched,T2: 선점형이라면 Task 2가 생기자마자 Task 1이 선점당했어야 함!<br/>하지만 협력형이므로 Task 2는 Ready 상태로 대기만 함
    loop 무한 반복 (양보 없는 busy wait)
        T1->>T1: "Tutorial 8 task 1 running..." 계속 출력
        Note over T1: Task 1이 양보하지 않으므로 CPU를 독점함
    end
    Note over T2: Task 2는 우선순위가 더 높은데도 실행 기회를 전혀 얻지 못함
```

---

## 5. 코드와 API 이해

### 5.1 `FreeRTOSConfig.h` 핵심 설정
```c
#define configUSE_PREEMPTION    0
```
* 선점 기능을 끈다.
* `configUSE_PREEMPTION`이 `0`이면 `configUSE_TIME_SLICING` 설정은 아무런 의미를 갖지 못한다 (선점할 수 없으므로 time slice로 자를 수도 없음).

### 5.2 `tutorial_8.c` 핵심 코드 분석
```c
static void prvTask1Function( void * pvParams )
{
    uint64_t i;
    BaseType_t xTaskCreationResult = pdFAIL;

    /* 우선순위가 더 높은 Task 2 생성 (우선순위: tskIDLE_PRIORITY + 1) */
    xTaskCreationResult = xTaskCreate( prvTask2Function,
                                       "Task2",
                                       configMINIMAL_STACK_SIZE,
                                       NULL,
                                       tskIDLE_PRIORITY + 1,
                                       NULL );
    configASSERT( xTaskCreationResult == pdPASS );

    for( ;; )
    {
        fprintf( stderr, "Tutorial 8 task 1 running...\r\n" );

        for( i = 0; i < 100000000; i++ )
        {
            /* 양보 없이 계속 시간 끄는 busy wait */
        }
    }
}
```

* Task 2가 성공적으로 생성되어 Ready 상태가 되었음에도 불구하고, Task 1이 `taskYIELD()`나 `vTaskDelay()`를 호출하지 않기 때문에 제어권이 넘어가지 않는다.

---

## 6. 직접 구현 또는 관찰 준비

* **실행 전 예측 질문**:
  * Task 2는 우선순위 1이고, Task 1은 우선순위 0이다.
  * 보통의 RTOS 상식으로는 상위 우선순위인 Task 2가 실행되어야 마땅하다.
  * 과연 `task 2 running...` 로그가 한 줄이라도 출력될 것인가?

---

## 7. 빌드와 실행

[공통 준비](getting-started.md)의 학습용 사본 루트에서 다음 명령을 실행한다:

```bash
# 1. CMake 빌드 구성 (Tutorial 8 지정)
cmake -S source -B build/tutorial_8 -DTUTORIAL=8

# 2. 컴파일 및 빌드
cmake --build build/tutorial_8

# 3. 바이너리 실행
./build/tutorial_8/freertos_example
```

* **종료 방법**: 터미널에서 **`Ctrl + C`**를 누른다.

---

## 8. 결과 해석

### 8.1 출력 예시

높은 우선순위 Task 2가 생성되어도 원본에서는 Task 1만 진행한다. 출력 속도는 고정하지 않는다.

```text
Tutorial 8 task 1 running...
Tutorial 8 task 1 running...
Tutorial 8 task 1 running...
Tutorial 8 task 1 running...
Tutorial 8 task 1 running...
```

### 8.2 판정 기준 및 해석
* **결과의 특징**: 우선순위가 더 높은 Task 2가 생성되었음에도 불구하고, 화면에는 오직 **`Tutorial 8 task 1 running...`만 계속해서 출력**된다
* **이유**:
  * 협력형 스케줄링(`configUSE_PREEMPTION 0`)에서는 아무리 높은 우선순위 task가 Ready 상태가 되어도, 현재 Running 상태인 task를 자동으로 선점하지 않는다.
  * Task 1이 양보 없이 busy-wait 루프를 돌고 있으므로 Task 2는 Ready 큐에 묶인 채 대기만 하게 된다.

---

## 9. 막혔을 때 (트러블슈팅)

| 증상 | 원인 | 해결 방법 |
| :--- | :--- | :--- |
| `task 2`가 출력되는 현상 | `FreeRTOSConfig.h`에서 `configUSE_PREEMPTION`이 1로 되어 있음 | `source/tutorials/tutorial_8/include/FreeRTOSConfig.h`의 설정 값 확인 |
| 이전 단원의 문자열이 나옴 | 다른 실행 경로·구성 가능성 | 7절의 `build/tutorial_8` 경로와 `-DTUTORIAL=8`을 확인하고 다시 구성·빌드 |

---

## 10. 혼자 확인하기 (자가 점검)

### 질문 1 (핵심 개념)
협력형 스케줄링(`configUSE_PREEMPTION 0`) 환경에서 task 코드에서 문맥 전환을 요청하는 대표적인 2가지 경우는 무엇인가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**:
  1. 실행 중인 task가 스스로 `taskYIELD()`를 호출하여 다른 task에게 실행권을 양보할 때.
  2. 실행 중인 task가 `vTaskDelay()`를 호출하거나, 비어 있는 큐에서 데이터를 읽으려 하거나, 이미 획득된 세마포어를 얻으려 하여 스스로 **Blocked 상태**로 진입할 때.
</details>

### 질문 2 (조건 변경과 결과 예측)
`prvTask1Function`의 로그 출력 바로 다음에 `taskYIELD();` 한 줄만 추가한다.

실행 전에 어떤 결과가 달라지고 어떤 동작이 그대로 유지될지 적는다. 직접 작성한 코드와 수정 전 파일은 [공통 보존·복구 절차](getting-started.md#solution-workflow)에 따라 보관한다. 위에서 지정한 조건 하나만 바꾸고 `cmake --build build/tutorial_8` 명령으로 다시 빌드한다. 실행 결과를 예측과 비교한 뒤 아래 해설을 확인한다. 해설은 예상 결과를 설명한 것이므로 직접 얻은 로그와 구분한다.

<details>
<summary>예상 결과와 해설 확인</summary>

첫 양보 뒤 높은 우선순위 Task 2가 실행한다. Task 2에도 대기·양보가 없으므로 이후 계속 실행하며 Task 1과 번갈아 출력하지 않는다.
</details>

**복구와 재확인**: `prvTask1Function`의 로그 출력 뒤에 추가한 `taskYIELD();` 호출을 삭제한다. 같은 단원을 다시 빌드하고 실행하여 [8절의 결과](#8-결과-해석)와 비교한다. 결과를 설명하기 어려우면 [관련 개념 다시 읽기](#3-개념-이해)로 돌아간다.

### 질문 3 (스케줄링 3종 알고리즘 총정리 표)
지금까지 배운 T06, T07, T08의 스케줄링 방식을 비교 표로 정리하라.

<details>
<summary>정답 및 해설 확인</summary>

| 구분 | T06 (선점 + 시분할) | T07 (선점 + 시분할 없음) | T08 (협력형) |
| :--- | :---: | :---: | :---: |
| **선점 활성 여부** | O (`1`) | O (`1`) | **X (`0`)** |
| **시분할 활성 여부**| O (`1`) | **X (`0`)** | 해당 없음 |
| **상위 task가 Ready가 됨** | 스케줄링 가능한 시점에 선점 | 스케줄링 가능한 시점에 선점 | 그 이유만으로 자동 선점하지 않음 |
| **동일 우선순위 task** | tick 기준으로 Ready task를 교대 | tick만으로 자동 교대하지 않음 | 이 예제에서는 자발적 양보·대기로 전환 |
</details>

---

## 11. 다음 학습

지금까지 task 생성, 메모리 할당, 우선순위와 스케줄링 설정을 비교했다.
이제 단일 task의 실행을 넘어, **서로 다른 task 간에 데이터를 주고받는 통신(Communication)과 동기화(Synchronization)**의 세계로 진입한다.

다음 단계인 **[Tutorial 9: queue를 통한 task 간 통신 (Using Queues for Inter-Task Communication)](tutorial_9.md)**으로 넘어가서, 큐의 생성과 데이터 전송을 배운다.

* **한국어 커널북 심화 읽기**:
  * [4.12.5장: 협력형 스케줄링 (Co-operative Scheduling)](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch04.md)
