/*
 * FreeRTOS-Tutorials (참조 완성 해답)
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: MIT-0
 *
 * 이 파일은 Tutorial 16의 모든 TODO가 완성된 참조 코드입니다.
 */

/* Standard includes. */
#include <stdio.h>
#include <unistd.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

/**
 * @brief Definitions of event bits in the event group.
 */
#define TASK_1_BIT ( 1UL << 0UL )
#define TASK_2_BIT ( 1UL << 1UL )
#define TASK_3_BIT ( 1UL << 2UL )
/*-----------------------------------------------------------*/

/**
 * @brief Functions that implement FreeRTOS tasks.
 */
static void prvTask1( void * pvParams );
static void prvTask2( void * pvParams );
static void prvTask3( void * pvParams );
/*-----------------------------------------------------------*/

/**
 * @brief Event group used in this program.
 */
static EventGroupHandle_t xEventGroup = NULL;
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

    xTaskCreationResult = xTaskCreate( prvTask3,
                                       "Task3",
                                       configMINIMAL_STACK_SIZE,
                                       NULL,
                                       tskIDLE_PRIORITY,
                                       NULL );
    configASSERT( xTaskCreationResult == pdPASS );

    /*
     * TODO 1 - Create event group using xEventGroupCreate API.
     */
    xEventGroup = xEventGroupCreate();

    configASSERT( xEventGroup != NULL );

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
        fprintf( stderr, "Task 1 reached sync point.\r\n" );

        /*
         * TODO 2 - Synchronize using xEventGroupSync API.
         */
        xEventGroupSync( xEventGroup,
                         TASK_1_BIT,
                         ( TASK_1_BIT | TASK_2_BIT | TASK_3_BIT ),
                         portMAX_DELAY );

        fprintf( stderr, "Task 1 exited sync point.\r\n" );

        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}
/*-----------------------------------------------------------*/

static void prvTask2( void * pvParams )
{
    /* Silence warning about unused parameters. */
    ( void ) pvParams;

    for( ;; )
    {
        fprintf( stderr, "Task 2 reached sync point.\r\n" );

        /*
         * TODO 3 - Synchronize using xEventGroupSync API.
         */
        xEventGroupSync( xEventGroup,
                         TASK_2_BIT,
                         ( TASK_1_BIT | TASK_2_BIT | TASK_3_BIT ),
                         portMAX_DELAY );

        fprintf( stderr, "Task 2 exited sync point.\r\n" );

        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}
/*-----------------------------------------------------------*/

static void prvTask3( void * pvParams )
{
    /* Silence warning about unused parameters. */
    ( void ) pvParams;

    for( ;; )
    {
        fprintf( stderr, "Task 3 reached sync point.\r\n" );

        /*
         * TODO 4 - Synchronize using xEventGroupSync API.
         */
        xEventGroupSync( xEventGroup,
                         TASK_3_BIT,
                         ( TASK_1_BIT | TASK_2_BIT | TASK_3_BIT ),
                         portMAX_DELAY );

        fprintf( stderr, "Task 3 exited sync point.\r\n" );

        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}
/*-----------------------------------------------------------*/
