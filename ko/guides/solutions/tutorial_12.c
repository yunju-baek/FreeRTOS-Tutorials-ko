/*
 * FreeRTOS-Tutorials (참조 완성 해답)
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: MIT-0
 *
 * 이 파일은 Tutorial 12의 모든 TODO가 완성된 참조 코드입니다.
 */

/* Standard includes. */
#include <stdio.h>
#include <unistd.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/**
 * @brief Functions that implement FreeRTOS tasks.
 */
static void prvTask1( void * pvParams );
static void prvTask2( void * pvParams );
/*-----------------------------------------------------------*/

/**
 * @brief Semaphore used in this program.
 */
static SemaphoreHandle_t xSemaphore = NULL;
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
                                       NULL );
    configASSERT( xTaskCreationResult == pdPASS );

    xTaskCreationResult = xTaskCreate( prvTask2,
                                       "Task2",
                                       configMINIMAL_STACK_SIZE,
                                       NULL,
                                       tskIDLE_PRIORITY,
                                       NULL );
    configASSERT( xTaskCreationResult == pdPASS );

    /*
     * TODO 1 - Create a binary semaphore using xSemaphoreCreateBinary API.
     */
    xSemaphore = xSemaphoreCreateBinary();

    configASSERT( xSemaphore != NULL );

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
        /*
         * TODO 2 - Take the semaphore using the xSemaphoreTake API.
         */
        xSemaphoreTake( xSemaphore, portMAX_DELAY );

        fprintf( stderr, "Task 1 is running...\r\n" );
    }

}
/*-----------------------------------------------------------*/

static void prvTask2( void * pvParams )
{
    BaseType_t xSemaphoreGiveResult = pdFAIL;

    /* Silence warning about unused parameters. */
    ( void ) pvParams;

    for( ;; )
    {
        fprintf( stderr, "Task 2 - signalling Task 1...\r\n" );

        /*
         * TODO 3 - Give the semaphore using the xSemaphoreGive API.
         */
        xSemaphoreGiveResult = xSemaphoreGive( xSemaphore );

        configASSERT( xSemaphoreGiveResult == pdPASS );

        fprintf( stderr, "Task 2 is running...\r\n" );

        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}
/*-----------------------------------------------------------*/
