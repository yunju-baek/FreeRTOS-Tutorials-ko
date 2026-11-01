/*
 * FreeRTOS-Tutorials (참조 완성 해답)
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: MIT-0
 *
 * 이 파일은 Tutorial 3의 모든 TODO가 완성된 참조 코드입니다.
 */

/* Standard includes. */
#include <stdio.h>
#include <unistd.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief Function that implement FreeRTOS task.
 */
static void prvTaskFunction( void * pvParams );
/*-----------------------------------------------------------*/

/**
 * @brief TCB and stack buffers for the task.
 */
static StaticTask_t xTaskTcbBuffer;
static StackType_t xTaskStackBuffer[ configMINIMAL_STACK_SIZE ];
/*-----------------------------------------------------------*/

/**
 * @brief Tutorial entry point.
 */
int main( void )
{
    TaskHandle_t xCreatedTaskHandle = NULL;

    /* TODO 1 - Create a FreeRTOS task using xTaskCreateStatic API which
     * uses prvTaskFunction as the task function.
     */
    xCreatedTaskHandle = xTaskCreateStatic( prvTaskFunction,
                                            "Task1",
                                            configMINIMAL_STACK_SIZE,
                                            NULL,
                                            tskIDLE_PRIORITY,
                                            &( xTaskStackBuffer[ 0 ] ),
                                            &( xTaskTcbBuffer ) );

    configASSERT( xCreatedTaskHandle != NULL );

    /* TODO 2 - Call vTaskStartScheduler to start the scheduler. */
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
    /* Silence warning about unused parameters. */
    ( void ) pvParams;

    for( ;; )
    {
        fprintf( stderr, "Tutorial 3 running...\r\n" );

        /* Pause for a second. */
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}
/*-----------------------------------------------------------*/
