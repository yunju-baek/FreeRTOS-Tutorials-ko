/*
 * FreeRTOS-Tutorials (참조 완성 해답)
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: MIT-0
 *
 * 이 파일은 Tutorial 13의 모든 TODO가 완성된 참조 코드입니다.
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
 * @brief Mutex used in this program.
 */
static SemaphoreHandle_t xMutex = NULL;
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
                                       tskIDLE_PRIORITY,
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
     * TODO 1 - Create a mutex using xSemaphoreCreateMutex API.
     */
    xMutex = xSemaphoreCreateMutex();

    configASSERT( xMutex != NULL );

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
    BaseType_t xSemaphoreGiveResult = pdFAIL;

    /* Silence warning about unused parameters. */
    ( void ) pvParams;

    for( ;; )
    {
        /*
         * TODO 2 - Take the mutex using xSemaphoreTake API.
         */
        xSemaphoreTake( xMutex, portMAX_DELAY );

        {
            fprintf( stderr, "Task 1 is running...\r\n" );
        }
        /*
         * TODO 3 - Give the mutex using xSemaphoreGive API.
         */
        xSemaphoreGiveResult = xSemaphoreGive( xMutex );

        configASSERT( xSemaphoreGiveResult == pdPASS );

        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
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
        /*
         * TODO 4 - Take the mutex using xSemaphoreTake API.
         */
        xSemaphoreTake( xMutex, portMAX_DELAY );

        {
            fprintf( stderr, "Task 2 is running...\r\n" );
        }
        /*
         * TODO 5 - Give the mutex using xSemaphoreGive API.
         */
        xSemaphoreGiveResult = xSemaphoreGive( xMutex );

        configASSERT( xSemaphoreGiveResult == pdPASS );

        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}
/*-----------------------------------------------------------*/
