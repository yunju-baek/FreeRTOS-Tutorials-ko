/*
 * FreeRTOS-Tutorials (참조 완성 해답)
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: MIT-0
 *
 * 이 파일은 Tutorial 17의 모든 TODO가 완성된 참조 코드입니다.
 */

/* Standard includes. */
#include <stdio.h>
#include <unistd.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief Functions that implement FreeRTOS tasks.
 */
static void prvTask1( void * pvParams );
static void prvTask2( void * pvParams );
/*-----------------------------------------------------------*/

/**
 * @brief Task 1 handle used to notify it.
 */
static TaskHandle_t xTask1Handle = NULL;
/*-----------------------------------------------------------*/

/**
 * @brief Tutorial entry point.
 */
int main( void )
{
    BaseType_t xTaskCreationResult = pdFAIL;

    xTaskCreationResult = xTaskCreate( prvTask1,
                                       "Task1",
                                       configMINIMAL_STACK_SIZE,
                                       NULL,
                                       tskIDLE_PRIORITY + 1,
                                       &( xTask1Handle ) );
    configASSERT( xTaskCreationResult == pdPASS );

    xTaskCreationResult = xTaskCreate( prvTask2,
                                       "Task2",
                                       configMINIMAL_STACK_SIZE,
                                       NULL,
                                       tskIDLE_PRIORITY,
                                       NULL );
    configASSERT( xTaskCreationResult == pdPASS );

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

static void prvTask1( void * pvParams )
{
    /* Silence warning about unused parameters. */
    ( void ) pvParams;

    for( ;; )
    {
        fprintf( stderr, "Task 1: Waiting for notification...\r\n" );

        /*
         * TODO 1 - Wait for a task notification using ulTaskNotifyTake API.
         */
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

        fprintf( stderr, "Task 1: Received notification...\r\n" );
    }
}
/*-----------------------------------------------------------*/

static void prvTask2( void * pvParams )
{
    /* Silence warning about unused parameters. */
    ( void ) pvParams;

    for( ;; )
    {
        fprintf( stderr, "Task 2: Notifying task 1...\r\n" );

        /*
         * TODO 2 - Notify task 1 using xTaskNotifyGive API.
         */
        xTaskNotifyGive( xTask1Handle );

        fprintf( stderr, "Task 2: Notified task 1...\r\n" );

        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}
/*-----------------------------------------------------------*/
