/*
 * FreeRTOS-Tutorials (참조 완성 해답)
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: MIT-0
 *
 * 이 파일은 Tutorial 18의 모든 TODO가 완성된 참조 코드입니다.
 */

/* Standard includes. */
#include <stdio.h>
#include <unistd.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief Definitions of event bits in the task notification.
 */
#define EVENT_1_BIT ( 1UL << 0UL )
#define EVENT_2_BIT ( 1UL << 1UL )
/*-----------------------------------------------------------*/

/**
 * @brief Functions that implement FreeRTOS tasks.
 */
static void prvEventGeneratorTask( void * pvParams );
static void prvEventHandlerTask( void * pvParams );
/*-----------------------------------------------------------*/

/**
 * @brief Handler task handle used to notify it.
 */
static TaskHandle_t xEventHandlerTaskHandle = NULL;
/*-----------------------------------------------------------*/

/**
 * @brief Tutorial entry point.
 */
int main( void )
{
    BaseType_t xTaskCreationResult = pdFAIL;

    xTaskCreationResult = xTaskCreate( prvEventGeneratorTask,
                                       "Generator",
                                       configMINIMAL_STACK_SIZE,
                                       NULL,
                                       tskIDLE_PRIORITY,
                                       NULL );
    configASSERT( xTaskCreationResult == pdPASS );

    xTaskCreationResult = xTaskCreate( prvEventHandlerTask,
                                       "Handler",
                                       configMINIMAL_STACK_SIZE,
                                       NULL,
                                       tskIDLE_PRIORITY + 1,
                                       &( xEventHandlerTaskHandle ) );
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

static void prvEventGeneratorTask( void * pvParams )
{
    /* Silence warning about unused parameters. */
    ( void ) pvParams;

    for( ;; )
    {
        fprintf( stderr, "Generating event 1...\r\n" );

        /*
         * TODO 1 - Notify the event handler task about event 1 using
         * xTaskNotify API.
         */
        xTaskNotify( xEventHandlerTaskHandle, EVENT_1_BIT, eSetBits );

        vTaskDelay( pdMS_TO_TICKS( 500 ) );

        fprintf( stderr, "Generating event 2...\r\n" );

        /*
         * TODO 2 - Notify the event handler task about event 2 using
         * xTaskNotify API.
         */
        xTaskNotify( xEventHandlerTaskHandle, EVENT_2_BIT, eSetBits );

        vTaskDelay( pdMS_TO_TICKS( 500 ) );

        fprintf( stderr, "Generated both events...\r\n" );
    }
}
/*-----------------------------------------------------------*/

static void prvEventHandlerTask( void * pvParams )
{
    uint32_t ulNotifiedValue;

    /* Silence warning about unused parameters. */
    ( void ) pvParams;

    for( ;; )
    {
        /*
         * TODO 3 - Wait for a task notification using xTaskNotifyWait API.
         */
        xTaskNotifyWait( 0, ~0, &( ulNotifiedValue ), portMAX_DELAY );

        if( ( ulNotifiedValue & EVENT_1_BIT ) != 0 )
        {
            fprintf( stderr, "Handling event 1...\r\n" );
        }
        if( ( ulNotifiedValue & EVENT_2_BIT ) != 0 )
        {
            fprintf( stderr, "Handling event 2...\r\n" );
        }
    }
}
/*-----------------------------------------------------------*/
