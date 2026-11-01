/*
 * FreeRTOS-Tutorials (참조 완성 해답)
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: MIT-0
 *
 * 이 파일은 Tutorial 4의 모든 TODO가 완성된 참조 코드입니다.
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
static void prvHighPriorityTaskFunction( void * pvParams );
static void prvLowPriorityTaskFunction( void * pvParams );
/*-----------------------------------------------------------*/

/**
 * @brief Tutorial entry point.
 */
int main( void )
{
    BaseType_t xTaskCreationResult = pdFAIL;

    /* TODO 1 - Create a FreeRTOS task using xTaskCreate API which
     * uses prvHighPriorityTaskFunction as the task function.
     */
    xTaskCreationResult = xTaskCreate( prvHighPriorityTaskFunction,
                                       "High",
                                       configMINIMAL_STACK_SIZE,
                                       NULL,
                                       1,
                                       NULL );
    configASSERT( xTaskCreationResult == pdPASS );

    /* TODO 2 - Create a FreeRTOS task using xTaskCreate API which
     * uses prvLowPriorityTaskFunction as the task function.
     */
    xTaskCreationResult = xTaskCreate( prvLowPriorityTaskFunction,
                                       "Low",
                                       configMINIMAL_STACK_SIZE,
                                       NULL,
                                       0,
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

static void prvHighPriorityTaskFunction( void * pvParams )
{
    uint64_t i;

    /* Silence warning about unused parameters. */
    ( void ) pvParams;

    for( ;; )
    {
        fprintf( stderr, "Tutorial 4 high priority task running...\r\n" );

        for( i = 0; i < 1000000000; i++ )
        {
            /* This loop is just a very crude delay implementation. */
        }
    }
}
/*-----------------------------------------------------------*/

static void prvLowPriorityTaskFunction( void * pvParams )
{
    uint64_t i;

    /* Silence warning about unused parameters. */
    ( void ) pvParams;

    for( ;; )
    {
        fprintf( stderr, "Tutorial 4 low priority task running...\r\n" );

        for( i = 0; i < 1000000000; i++ )
        {
            /* This loop is just a very crude delay implementation. */
        }
    }
}
/*-----------------------------------------------------------*/
