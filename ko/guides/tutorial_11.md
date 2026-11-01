# Tutorial 11: software timer

[학습 목차](README.md) · [공통 준비](getting-started.md) · [공식 튜토리얼 번역](../docs/tutorial_11/README.md) · [이전 단원](tutorial_10.md)

이 단원에서는 하드웨어 타이머 개수의 제약을 극복하고, 미래의 특정 시점이나 주기적인 작업을 예약 실행할 수 있게 해주는 **software timer(소프트웨어 타이머)**의 생성과 동작 원리를 배운다.

---

## 1. 이번 실습에서 할 일

* **해결하려는 문제**: 하드웨어 타이머 장치가 부족한 임베디드 MCU에서 수십 개의 독립된 타이머를 소프트웨어적으로 생성하고 주기적/단발성 이벤트를 처리한다.
* **학습 목표**:
  1. **단발 타이머(One-shot timer)**와 **자동 재장전 타이머(Auto-reload timer)**의 동작 차이를 설명할 수 있다.
  2. `xTimerCreate()`와 `xTimerStart()` API를 사용하여 두 타이머를 생성하고 구동한다.
  3. 타이머 콜백 함수가 실행되는 문맥(Timer Daemon Task)의 특성과 콜백 내부에서의 주의점(블로킹 금지)을 이해한다.

---

## 2. 시작 전 확인

### 2.1 C 언어 핵심 복습
* **콜백 함수 (Callback Function)**: 사용자가 직접 호출하는 것이 아니라, 특정 이벤트(타이머 만료, 패킷 도착 등)가 일어났을 때 시스템(커널)에 의해 거꾸로 호출되는 함수다.
* **타이머 식별자 (Timer ID)**: 여러 타이머가 동일한 콜백 함수를 공유할 때, 콜백 내부에서 `pvTimerGetTimerID( xTimer )`를 호출하여 어떤 타이머가 만료된 것인지 식별할 수 있다.

### 2.2 확인할 소스 및 설정 파일
* 실습 코드: [`source/tutorials/tutorial_11/source/tutorial_11.c`](../../source/tutorials/tutorial_11/source/tutorial_11.c)
* 설정 파일: [`source/tutorials/tutorial_11/include/FreeRTOSConfig.h`](../../source/tutorials/tutorial_11/include/FreeRTOSConfig.h)
  * `configUSE_TIMERS`가 `1`로 설정되어 있어야 소프트웨어 타이머 데몬 task가 활성화된다.
* 참조 해답: [`ko/guides/solutions/tutorial_11.c`](solutions/tutorial_11.c)

> **실습 형태**: `tutorial_11.c` 내부의 **`TODO 1`, `TODO 2`, `TODO 3`, `TODO 4`를 완성하는 구현 실습**이다.

---

## 3. 개념 이해

### 3.1 소프트웨어 타이머의 2가지 유형
1. **단발 타이머 (One-shot Timer)**:
   * 시작된 후 설정된 주기가 만료되면 콜백 함수를 **딱 1회만 실행**하고 멈춘다. (수동으로 다시 `xTimerStart`를 부르지 않는 한 재시작하지 않음).
   * 예: 통신 타임아웃 감시, 버튼 길게 누름 판정 등.
2. **자동 재장전 타이머 (Auto-reload Timer)**:
   * 주기가 만료되어 콜백을 실행한 뒤, 커널이 타이머를 **자동으로 다시 시작**시켜 주기적으로 콜백이 반복 실행된다.
   * 예: 1초마다 센서 값 읽기, 하트비트 LED 점멸 등.

```mermaid
flowchart TD
    subgraph OneShot["단발 타이머 (One-shot, 100ms)"]
        direction TB
        O1["시작 (xTimerStart)"] --> O2["100ms 대기"] --> O3["콜백 1회 실행"] --> O4["정지 (Dormant 상태)"]
    end

    subgraph AutoReload["자동 재장전 타이머 (Auto-reload, 1000ms)"]
        direction TB
        A1["시작 (xTimerStart)"] --> A2["1000ms 대기"] --> A3["콜백 실행"] --> A2
    end
```

### 3.2 타이머 콜백은 누가 실행하는가? (Timer Service Task)
FreeRTOS의 소프트웨어 타이머 콜백은 인터럽트 핸들러가 실행하는 것이 아니라, 커널이 내부적으로 띄우는 **`prvTimerTask`(Timer Service / Daemon Task)**라는 전용 task가 실행한다.
* **매우 중요한 제약 사항**:
  * 타이머 콜백 함수 내부에서 `vTaskDelay()`를 호출하거나, 데이터가 올 때까지 무한정 기다리는 `xQueueReceive(..., portMAX_DELAY)` 같은 **블로킹 API를 절대 호출해서는 안 된다**
  * 타이머 콜백이 잠들면 Timer Daemon Task 전체가 멈추어 시스템의 다른 모든 소프트웨어 타이머 동작이 일제히 마비되기 때문이다.

---

## 4. 실행 흐름 읽기

```mermaid
sequenceDiagram
    autonumber
    participant Main as main()
    participant Daemon as Timer Daemon Task
    participant CB as prvTimerCallback

    Main->>Main: AutoTimer (1000ms) & OneTimer (100ms) 생성
    Main->>Main: 두 타이머 모두 Start 요청 (TODO 3, 4)
    Main->>Daemon: vTaskStartScheduler() 호출로 스케줄러 시작

    Note over Daemon: 100ms 경과 (OneTimer 만료)
    Daemon->>CB: prvTimerCallback(OneTimer) 실행
    CB->>CB: "Callback for Timer ID 0x2 running..." (1회 출력 뒤 비활성, 다시 시작 가능)

    Note over Daemon: 1000ms 경과 (AutoTimer 만료)
    Daemon->>CB: prvTimerCallback(AutoTimer) 실행
    CB->>CB: "Callback for Timer ID 0x1 running..." 출력

    Note over Daemon: 다시 1000ms 경과 (AutoTimer 자동 재장전)
    Daemon->>CB: prvTimerCallback(AutoTimer) 재실행
```

---

## 5. 코드와 API 이해

### 5.1 `xTimerCreate()` API
```c
TimerHandle_t xTimerCreate( const char * const pcTimerName,
                            const TickType_t xTimerPeriodInTicks,
                            const UBaseType_t uxAutoReload,
                            void * const pvTimerID,
                            TimerCallbackFunction_t pxCallbackFunction );
```
* `pcTimerName`: 디버깅용 타이머 이름 문자열.
* `xTimerPeriodInTicks`: 타이머 만료 주기 (`pdMS_TO_TICKS( ms )`).
* `uxAutoReload`: `pdTRUE`이면 자동 재장전, `pdFALSE`이면 단발 타이머.
* `pvTimerID`: 타이머 식별용 ID 값 (포인터 형태).
* `pxCallbackFunction`: 만료 시 실행할 콜백 함수 포인터.

### 5.2 `xTimerStart()` API
```c
BaseType_t xTimerStart( TimerHandle_t xTimer, TickType_t xTicksToWait );
```
* 타이머 서비스 task에 시작 명령을 보낸다. `pdPASS`는 명령 큐에 요청을 넣었다는 뜻이며 콜백 실행이나 활성화 완료를 뜻하지 않는다. 이 예제는 스케줄러 시작 전에 명령을 보내므로 시작 뒤 서비스 task가 처리한다.
* `xTicksToWait`: 명령 큐가 가득 찼을 때의 최대 대기 tick 수다. 이 예제의 `0`은 대기 없이 시도한다. 타이머 주기와는 다른 인자다.

---

## 6. 직접 구현 또는 관찰 준비

전체 참조 해답은 [tutorial_11.c](solutions/tutorial_11.c)에 있다. 먼저 TODO별 힌트를 따라 작성하고, [자기 코드 보존·해답 적용·복구 절차](getting-started.md#solution-workflow)에 따라 비교한다.

`source/tutorials/tutorial_11/source/tutorial_11.c` 파일을 열고 4개의 TODO를 채운다.

### 6.1 TODO 1: Auto-reload 타이머 생성 (1000ms)
49~60행 주석: 이름 `"AutoTimer"`, 주기 `pdMS_TO_TICKS( 1000 )`, `pdTRUE`, ID `(void*)1`, 콜백 `prvTimerCallback`.

* **TODO 1 힌트 1**: 자동 재장전 여부와 주기를 독립된 인자로 지정한다.
* **TODO 1 힌트 2**: `main`에서 AutoTimer는 1000ms를 tick으로 변환하고 재장전을 켠다. ID 1과 기존 콜백을 연결한다.

<details>
<summary>TODO 1 해답 코드 보기</summary>

```c
    xAutoReloadTimer = xTimerCreate( "AutoTimer",
                                     pdMS_TO_TICKS( 1000 ),
                                     pdTRUE,
                                     ( void * ) 1,
                                     prvTimerCallback );
```

**이유**: 자동 재장전 여부와 주기를 독립된 인자로 지정한다.
</details>

### 6.2 TODO 2: One-shot 타이머 생성 (100ms)
65~76행 주석: 이름 `"OneTimer"`, 주기 `pdMS_TO_TICKS( 100 )`, `pdFALSE`, ID `(void*)2`, 콜백 `prvTimerCallback`.

* **TODO 2 힌트 1**: 단발 타이머는 만료 뒤 자동으로 다시 시작하지 않는다.
* **TODO 2 힌트 2**: OneTimer의 주기는 100ms이며 재장전 인자는 거짓이다. ID 2와 같은 콜백을 연결한다.

<details>
<summary>TODO 2 해답 코드 보기</summary>

```c
    xOneShotTimer = xTimerCreate( "OneTimer",
                                  pdMS_TO_TICKS( 100 ),
                                  pdFALSE,
                                  ( void * ) 2,
                                  prvTimerCallback );
```

**이유**: 단발 타이머는 만료 뒤 자동으로 다시 시작하지 않는다.
</details>

### 6.3 TODO 3 & 4: 두 타이머 시작
81~100행 주석: `xTimerStart()`로 각각 구동한다.

* **TODO 3 힌트 1**: 생성만 한 타이머에는 시작 명령이 필요하다.
* **TODO 3 힌트 2**: `main`의 TODO 3에서 `xAutoReloadTimer` 핸들과 대기 시간 0을 사용하고 결과를 검사할 변수에 저장한다.
* **TODO 4 힌트 1**: 두 타이머는 각각 시작 명령을 받아야 한다.
* **TODO 4 힌트 2**: TODO 4에서는 단발 타이머 핸들 `xOneShotTimer`를 사용한다. 반환 성공은 명령 큐에 넣었다는 뜻이다.

<details>
<summary>TODO 3 & 4 해답 코드 보기</summary>

```c
    xTimerStartResult = xTimerStart( xAutoReloadTimer, 0 );
    configASSERT( xTimerStartResult == pdPASS );

    xTimerStartResult = xTimerStart( xOneShotTimer, 0 );
    configASSERT( xTimerStartResult == pdPASS );
```

**이유**: 생성만 한 타이머에는 시작 명령이 필요하다. 두 타이머는 각각 시작 명령을 받아야 한다.
</details>

> 전체 완성 코드는 [`ko/guides/solutions/tutorial_11.c`](solutions/tutorial_11.c)에서 확인할 수 있다.

---

## 7. 빌드와 실행

[공통 준비](getting-started.md)의 학습용 사본 루트에서 다음 명령을 실행한다:

```bash
# 1. CMake 빌드 구성 (Tutorial 11 지정)
cmake -S source -B build/tutorial_11 -DTUTORIAL=11

# 2. 컴파일 및 빌드
cmake --build build/tutorial_11

# 3. 바이너리 실행
./build/tutorial_11/freertos_example
```

* **종료 방법**: 터미널에서 **`Ctrl + C`**를 누른다.

---

## 8. 결과 해석

### 8.1 출력 예시

단발 ID는 한 번, 자동 재장전 ID는 반복해서 나타난다. 정확한 실행 시점과 포인터 표기는 달라질 수 있다.

```text
Callback for Timer ID 0x2 running...
Callback for Timer ID 0x1 running...
Callback for Timer ID 0x1 running...
Callback for Timer ID 0x1 running...
Callback for Timer ID 0x1 running...
```

### 8.2 판정 기준 및 해석
* **결과의 특징**:
  1. 프로그램이 시작되고 아주 잠깐(100ms) 후 `Timer ID 0x2`(One-shot)가 **단 한 번** 출력된다.
  2. 그 후 1초(1000ms)마다 `Timer ID 0x1`(Auto-reload)이 **주기적으로 반복** 출력된다.
* **검증**: 단발 타이머가 정확히 1회 실행 후 저절로 비활성화되었으며, 자동 재장전 타이머는 계속 반복되는지 확인한다. 콜백의 실제 시점과 `%p` 표기는 호스트 부하·구현에 따라 달라질 수 있다.

---

## 9. 막혔을 때 (트러블슈팅)

| 증상 | 원인 | 해결 방법 |
| :--- | :--- | :--- |
| `0x2`가 출력되지 않음 | TODO 4의 `xTimerStart(xOneShotTimer, 0)` 누락 | TODO 4 시작 코드 호출 여부 확인 |
| `0x2`도 약 100ms마다 반복됨 | TODO 2에서 `pdFALSE` 대신 `pdTRUE`를 넘겼음 | 단발 타이머의 3번째 인자가 `pdFALSE`인지 확인 |

---

## 10. 혼자 확인하기 (자가 점검)

### 질문 1 (주의사항)
소프트웨어 타이머 콜백 함수 내부에서 `vTaskDelay(pdMS_TO_TICKS(500))`를 호출하면 왜 다른 타이머의 처리를 지연시키는가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: 타이머 콜백을 실행하는 주체는 시스템의 유일한 **Timer Daemon Task**이기 때문이다.
* **이유**: 콜백이 500ms 동안 잠들면 데몬 task 자체가 멈추므로, 그동안 다른 타이머의 만료와 명령을 처리할 수 없게 된다. 오래 걸리는 일은 별도 task에 전달한다.
</details>

### 질문 2 (식별자 활용)
콜백 함수가 단 하나뿐인데 어떻게 `0x1`과 `0x2`를 구별하여 출력할 수 있었는가?

<details>
<summary>정답 및 해설 확인</summary>

* **정답**: `pvTimerGetTimerID( xTimer )` API 덕분이다.
* **원리**: 콜백 함수의 인자로 넘어온 타이머 핸들(`xTimer`)을 통해 타이머 제어 구조체(`Timer_t`)에 등록해 두었던 `pvTimerID` 값을 꺼내왔기 때문이다.
</details>

### 질문 3 (한 조건 변형과 실행 추적)
`main()`의 OneTimer 생성에서 주기만 `pdMS_TO_TICKS(100)`에서 `pdMS_TO_TICKS(200)`으로 바꾼다.

실행 전에 바뀔 결과와 유지될 조건을 각각 적는다. 자기 풀이와 변경 전 파일은 [공통 보존·복구 절차](getting-started.md#solution-workflow)로 남긴다. 한 조건만 수정하고 `cmake --build build/tutorial_11`으로 다시 빌드한 뒤 실행해 예측과 비교한다. 아래 해설은 변형의 예상 결과이며, 직접 얻은 로그와 구분한다.

<details>
<summary>예상 결과와 해설 확인</summary>

단발 콜백은 더 늦게 한 번 실행하고 AutoTimer는 계속 반복한다. 주기 변경과 재장전 여부 변경을 구분한다. 정확한 200ms의 벽시계 실행은 요구하지 않는다.
</details>

**복구와 재확인**: 변경한 한 값(추가한 코드라면 그 한 줄)을 원래대로 돌리고 같은 단원을 다시 빌드·실행한다. 원래 조건의 8절 결과와 비교한다. 설명이 어려우면 [관련 개념 다시 읽기](#5-코드와-api-이해)로 돌아간다.

---

## 11. 다음 학습

소프트웨어 타이머를 통해 주기적 작업을 관리하는 법을 배웠다.
이제 **[Tutorial 12: semaphore](tutorial_12.md)**로 넘어가서, task와 task, 또는 인터럽트와 task 간에 특정 이벤트의 발생을 알리고 동기화하는 **이진 세마포어(Binary Semaphore)**를 배운다.

* **한국어 커널북 심화 읽기**:
  * [6장: software timer 관리 (Software Timer Management)](https://github.com/yunju-baek/FreeRTOS-Kernel-Book-ko/blob/27d03c61cd2a6cf5a6fa6333082218ef706f5b89/ko/ch06.md)
