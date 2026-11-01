/*
 * FreeRTOS-Tutorials (참조 완성 해답)
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: MIT-0
 *
 * 이 파일은 Tutorial 9의 모든 TODO가 완성된 참조 코드입니다.
 */

/* Standard includes. */
#include <stdio.h>
#include <unistd.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/**
 * @brief Functions that implement FreeRTOS tasks.
 */
static void prvSenderTask( void * pvParams );
static void prvReceiverTask( void * pvParams );
/*-----------------------------------------------------------*/

/**
 * @brief Queue used in the program.
 */
static QueueHandle_t xQueue;
/*-----------------------------------------------------------*/

/**
 * @brief Tutorial entry point.
 */
int main( void )
{
    BaseType_t xTaskCreationResult = pdFAIL;

    xTaskCreationResult = xTaskCreate( prvSenderTask,
                                       "Sender",
                                       configMINIMAL_STACK_SIZE,
                                       NULL,
                                       tskIDLE_PRIORITY + 1,
                                       NULL );
    configASSERT( xTaskCreationResult == pdPASS );

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

static void prvSenderTask( void * pvParams )
{
    BaseType_t xQueueSendResult;
    UBaseType_t uxValueToSend = 0;

    /* Silence warning about unused parameters. */
    ( void ) pvParams;

    for( ;; )
    {
        uxValueToSend++;

        /* TODO 2 - Send uxValueToSend to the queue using xQueueSend API. */
        xQueueSendResult = xQueueSend( xQueue, &( uxValueToSend ), portMAX_DELAY );

        configASSERT( xQueueSendResult == pdPASS );

        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}
/*-----------------------------------------------------------*/

static void prvReceiverTask( void * pvParams )
{
    UBaseType_t uxReceivedValue;

    /* Silence warning about unused parameters. */
    ( void ) pvParams;

    for( ;; )
    {
        /* TODO 3 - Receive from the queue using xQueueReceive API. */
        xQueueReceive( xQueue, &( uxReceivedValue ), portMAX_DELAY );

        fprintf( stderr, "Value received from the queue: %lu\r\n", uxReceivedValue );
        fprintf( stderr, "Number of items in the queue: %lu.\r\n", uxQueueMessagesWaiting( xQueue ) );
    }
}
/*-----------------------------------------------------------*/
