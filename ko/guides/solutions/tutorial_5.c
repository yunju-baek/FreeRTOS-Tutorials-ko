/*
 * FreeRTOS-Tutorials (참조 완성 해답)
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: MIT-0
 *
 * 이 파일은 Tutorial 5의 모든 TODO가 완성된 참조 코드입니다.
 */

/* Standard includes. */
#include <stdio.h>
#include <unistd.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief Function that implement FreeRTOS tasks.
 */
static void prvTaskFunction( void * pvParams );
/*-----------------------------------------------------------*/

/**
 * @brief Tutorial entry point.
 */
int main( void )
{
    BaseType_t xTaskCreationResult = pdFAIL;

    /* TODO 1 - Create a FreeRTOS task using xTaskCreate API which
     * uses prvTaskFunction as the task function.
     */
    xTaskCreationResult = xTaskCreate( prvTaskFunction,
                                       "Task1",
                                       configMINIMAL_STACK_SIZE,
                                       ( void * ) 1,
                                       tskIDLE_PRIORITY,
                                       NULL );
    configASSERT( xTaskCreationResult == pdPASS );

    /* TODO 2 - Create a FreeRTOS task using xTaskCreate API which
     * uses prvTaskFunction as the task function.
     */
    xTaskCreationResult = xTaskCreate( prvTaskFunction,
                                       "Task2",
                                       configMINIMAL_STACK_SIZE,
                                       ( void * ) 2,
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

static void prvTaskFunction( void * pvParams )
{
    for( ;; )
    {
        fprintf( stderr, "Tutorial 5 task %p running...\r\n", pvParams );

        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}
/*-----------------------------------------------------------*/
