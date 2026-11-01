/*
 * FreeRTOS-Tutorials (참조 완성 해답)
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: MIT-0
 *
 * 이 파일은 Tutorial 10의 모든 TODO가 완성된 참조 코드입니다.
 */

/* Standard includes. */
#include <stdio.h>
#include <unistd.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/**
 * @brief Function that implement FreeRTOS task.
 */
static void prvReceiverTask( void * pvParams );
/*-----------------------------------------------------------*/

/**
 * @brief Queue used in the program.
 */
static QueueHandle_t xQueue;

/**
 * @brief Counter used in the program.
 */
static UBaseType_t uxCounter = 0;
/*-----------------------------------------------------------*/

/**
 * @brief Tutorial entry point.
 */
int main( void )
{
    BaseType_t xTaskCreationResult = pdFAIL;

    xTaskCreationResult = xTaskCreate( prvReceiverTask,
                                       "Receiver",
                                       configMINIMAL_STACK_SIZE,
                                       NULL,
                                       tskIDLE_PRIORITY,
                                       NULL );
    configASSERT( xTaskCreationResult == pdPASS );

    /* TODO 1 - Create a queue capable of holding 5 UBaseType_t using
     * xQueueCreate API.
     */
    xQueue = xQueueCreate( 5, sizeof( UBaseType_t ) );

    configASSERT( xQueue != NULL );

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

static void prvReceiverTask( void * pvParams )
{
    UBaseType_t uxReceivedValue;

    /* Silence warning about unused parameters. */
    ( void ) pvParams;

    for( ;; )
    {
        /* TODO 2 - Receive from the queue using xQueueReceive API. */
        xQueueReceive( xQueue, &( uxReceivedValue ), portMAX_DELAY );

        fprintf( stderr, "Value received from the queue: %lu\r\n", uxReceivedValue );
        fprintf( stderr, "Number of items in the queue: %lu.\r\n", uxQueueMessagesWaiting( xQueue ) );
    }
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
    BaseType_t xQueueSendResult = pdFAIL;
    BaseType_t xHighPriorityTaskWoken = pdFALSE;

    uxCounter++;

    if( ( uxCounter % 1000 ) == 0 )
    {
        /* TODO 3 - Send uxCounter to the queue using xQueueSendFromISR API. */
        xQueueSendResult = xQueueSendFromISR( xQueue,
                                              &( uxCounter ),
                                              &( xHighPriorityTaskWoken ) );

        configASSERT( xQueueSendResult == pdPASS );
    }

    portYIELD_FROM_ISR( xHighPriorityTaskWoken );
}
/*-----------------------------------------------------------*/
