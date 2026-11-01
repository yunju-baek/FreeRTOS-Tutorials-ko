/*
 * FreeRTOS-Tutorials (참조 완성 해답)
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: MIT-0
 *
 * 이 파일은 Tutorial 11의 모든 TODO가 완성된 참조 코드입니다.
 */

/* Standard includes. */
#include <stdio.h>
#include <unistd.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/**
 * @brief Timer callback function.
 */
static void prvTimerCallback( TimerHandle_t xTimer );
/*-----------------------------------------------------------*/

/**
 * @brief Tutorial entry point.
 */
int main( void )
{
    BaseType_t xTimerStartResult = pdFAIL;
    TimerHandle_t xAutoReloadTimer, xOneShotTimer;

    /*
     * TODO 1 - Create an auto-reload timer with 1000 ms period using
     * xTimerCreate API.
     */
    xAutoReloadTimer = xTimerCreate( "AutoTimer",
                                     pdMS_TO_TICKS( 1000 ),
                                     pdTRUE,
                                     ( void * ) 1,
                                     prvTimerCallback );
    configASSERT( xAutoReloadTimer != NULL );

    /*
     * TODO 2 - Create a one-shot timer with 100 ms period using xTimerCreate
     * API.
     */
    xOneShotTimer = xTimerCreate( "OneTimer",
                                  pdMS_TO_TICKS( 100 ),
                                  pdFALSE,
                                  ( void * ) 2,
                                  prvTimerCallback );
    configASSERT( xOneShotTimer != NULL );

    /*
     * TODO 3 - Start the auto-reload timer using xTimerStart API.
     */
    xTimerStartResult = xTimerStart( xAutoReloadTimer, 0 );
    configASSERT( xTimerStartResult == pdPASS );

    /*
     * TODO 4 - Start the one-shot timer using xTimerStart API.
     */
    xTimerStartResult = xTimerStart( xOneShotTimer, 0 );
    configASSERT( xTimerStartResult == pdPASS );

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* Should not reach here. */
    for( ;; )
    {

    }

    /* Just to make the compiler happy. */
    return 0;
}
/*-----------------------------------------------------------*/

static void prvTimerCallback( TimerHandle_t xTimer )
{
    fprintf( stderr, "Callback for Timer ID %p running...\r\n", pvTimerGetTimerID( xTimer ) );
}
/*-----------------------------------------------------------*/
