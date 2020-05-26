/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "FreeRTOS.h"
#include "Task.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "bsp_key.h"

static TaskHandle_t AppTaskCreate_Handle;
static TaskHandle_t LED_Task_Handle;
static TaskHandle_t Hello_Task_Handle;
static TaskHandle_t KEY_Task_Handle;
/* Task statck*/
static StackType_t AppTaskCreate_Stack[128];
static StackType_t LED_Task_Stack[128];
static StackType_t Hello_Task_Stack[128];
static StackType_t KEY_Task_Stack[128];
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];

/* Task TCB*/
static StaticTask_t AppTaskCreate_TCB;
static StaticTask_t LED_Task_TCB;
static StaticTask_t Hello_Task_TCB;
static StaticTask_t KEY_Task_TCB;
static StaticTask_t Idle_Task_TCB;
static StaticTask_t Timer_Task_TCB;

/* Task priorities. */
#define App_Task_PRIORITY    2
#define LED_Task_PRIORITY    3
#define KEY_Task_PRIORITY    4
#define Hello_Task_PRIORITY  5
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void AppTaskCreate(void);
static void Hello_Task(void *pvParameters);
static void LED_Task(void *pvParameters);
static void KEY_Task(void *pvParameters);


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_LED_GPIO BOARD_USER_LED_GPIO
#define EXAMPLE_LED_GPIO_PIN BOARD_USER_LED_GPIO_PIN
#define EXAMPLE_DELAY_COUNT 8000000


void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
																		StackType_t **ppxTimerTaskStackBuffer, 
																		uint32_t *pulTimerTaskStackSize);

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
																	 StackType_t **ppxIdleTaskStackBuffer, 
																	 uint32_t *pulIdleTaskStackSize);
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void delay(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* The PIN status */
volatile bool g_pinSet = false;
extern volatile bool g_InputSignal;
/*******************************************************************************
 * Code
 ******************************************************************************/
void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < EXAMPLE_DELAY_COUNT; ++i)
    {
        __asm("NOP"); /* delay */
    }
}


int main(void)
{
    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
	  BSP_KeyInit();  
		PRINTF("\r\n");
		PRINTF("************* RT1050 FreeRTOS *************\r\n");
		PRINTF("CPU:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_CpuClk));
		PRINTF("AHB:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_AhbClk));
		PRINTF("SEMC:            %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SemcClk));
		PRINTF("SYSPLL:          %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllClk));
		PRINTF("SYSPLLPFD0:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd0Clk));
		PRINTF("SYSPLLPFD1:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd1Clk));
		PRINTF("SYSPLLPFD2:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk));
		PRINTF("SYSPLLPFD3:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk)); 
		
	
	  if(xTaskCreate((TaskFunction_t)AppTaskCreate,
									"AppTaskCreate",
		               128,
		               NULL,
									 App_Task_PRIORITY,
		               NULL)!=pdPASS)
		{
			PRINTF("Task creation failed!.\r\n");
      while (1);
		}
		vTaskStartScheduler();
    for (;;);  
}
/*************************** task function ************************************/
static void AppTaskCreate(void)
{
	taskENTER_CRITICAL();
	LED_Task_Handle = xTaskCreateStatic((TaskFunction_t)LED_Task,
																			(const char*)"LED_Task",
																		  128,
																			NULL,
																			LED_Task_PRIORITY,
																			LED_Task_Stack,
																			&LED_Task_TCB);																	
	if(NULL != LED_Task_Handle)
		PRINTF("LED_Task Create OK\r\n");
	else
		PRINTF("LED_Task Create Failed\r\n");		
	Hello_Task_Handle = xTaskCreateStatic((TaskFunction_t)Hello_Task,
																			(const char*)"Hello_Task",
																		  128,
																			NULL,
																			Hello_Task_PRIORITY,
																			Hello_Task_Stack,
																			&Hello_Task_TCB);																					
	if(NULL != Hello_Task_Handle)
		PRINTF("Hello_Task Create OK\r\n");
	else
		PRINTF("Hello_Task Create Failed\r\n");	
	KEY_Task_Handle = xTaskCreateStatic((TaskFunction_t)KEY_Task,
																			(const char*)"KEY_Task",
																		  128,
																			NULL,
																			KEY_Task_PRIORITY,
																			KEY_Task_Stack,
																			&KEY_Task_TCB);																					
	if(NULL != KEY_Task_Handle)
		PRINTF("KEY_Task Create OK\r\n");
	else
		PRINTF("KEY_Task Create Failed\r\n");	
	
	vTaskDelete(AppTaskCreate_Handle); 
  taskEXIT_CRITICAL();  
	//vTaskStartScheduler();	
}
static void Hello_Task(void *pvParameters)
{
    for (;;)
    {
        PRINTF("Hello world.\r\n");
        vTaskDelay(1000);
    }
}

static void LED_Task(void *pvParameters)
{
	gpio_pin_config_t led_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
	GPIO_PinInit(EXAMPLE_LED_GPIO, EXAMPLE_LED_GPIO_PIN, &led_config);
	for(;;)
	{
		if (g_pinSet)
		{
			GPIO_PinWrite(EXAMPLE_LED_GPIO, EXAMPLE_LED_GPIO_PIN, 0U);
			g_pinSet = false;
		}
		else
		{
			GPIO_PinWrite(EXAMPLE_LED_GPIO, EXAMPLE_LED_GPIO_PIN, 1U);
			g_pinSet = true;
		}
		vTaskDelay(500);
	}
}



static void KEY_Task(void *pvParameters)
{
	static unsigned char key_count  =0;
	for(;;)
	{
		if (g_InputSignal)
		{
			vTaskDelay(20);
			if (1 == GPIO_PinRead(EXAMPLE_SW_GPIO, EXAMPLE_SW_GPIO_PIN))
			{
					PRINTF("%s is turned on.\r\n", EXAMPLE_SW_NAME);
			}
			/* Reset state of switch. */
			g_InputSignal = false;
			key_count++;
			if(key_count%2)
			{
				vTaskSuspend(LED_Task_Handle);
				PRINTF("Suspend LED_Task\r\n");
			}
			else
			{
				vTaskResume(LED_Task_Handle);
				PRINTF("Resume LED_Task\r\n");
			}
		}
		vTaskDelay(10);
	}
}

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
																	 StackType_t **ppxIdleTaskStackBuffer, 
																	 uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer=&Idle_Task_TCB;
	*ppxIdleTaskStackBuffer=Idle_Task_Stack;
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;
}
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
									StackType_t **ppxTimerTaskStackBuffer, 
									uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer=&Timer_Task_TCB;
	*ppxTimerTaskStackBuffer=Timer_Task_Stack;
	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;
}

/*!
 * @brief Main function
 */
//int main(void)
//{
//    /* Define the init structure for the output LED pin*/
//    gpio_pin_config_t led_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};

//    /* Board pin, clock, debug console init */
//    BOARD_ConfigMPU();
//    BOARD_InitPins();
//    BOARD_BootClockRUN();
//    BOARD_InitDebugConsole();

//    /* Print a note to terminal. */
//    PRINTF("\r\n GPIO Driver example\r\n");
//    PRINTF("\r\n The LED is blinking.\r\n");

//    /* Init output LED GPIO. */
//    GPIO_PinInit(EXAMPLE_LED_GPIO, EXAMPLE_LED_GPIO_PIN, &led_config);

//    while (1)
//    {
//        SDK_DelayAtLeastUs(100000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
//#if (defined(FSL_FEATURE_IGPIO_HAS_DR_TOGGLE) && (FSL_FEATURE_IGPIO_HAS_DR_TOGGLE == 1))
//        GPIO_PortToggle(EXAMPLE_LED_GPIO, 1u << EXAMPLE_LED_GPIO_PIN);
//#else
//        if (g_pinSet)
//        {
//            GPIO_PinWrite(EXAMPLE_LED_GPIO, EXAMPLE_LED_GPIO_PIN, 0U);
//            g_pinSet = false;
//        }
//        else
//        {
//            GPIO_PinWrite(EXAMPLE_LED_GPIO, EXAMPLE_LED_GPIO_PIN, 1U);
//            g_pinSet = true;
//        }
//#endif /* FSL_FEATURE_IGPIO_HAS_DR_TOGGLE */
//    }
//}
