/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "sensor_types.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Task_SensorRead */
osThreadId_t Task_SensorReadHandle;
const osThreadAttr_t Task_SensorRead_attributes = {
  .name = "Task_SensorRead",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_Control */
osThreadId_t Task_ControlHandle;
const osThreadAttr_t Task_Control_attributes = {
  .name = "Task_Control",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_Comm */
osThreadId_t Task_CommHandle;
const osThreadAttr_t Task_Comm_attributes = {
  .name = "Task_Comm",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for Queue_SensorData */
osMessageQueueId_t Queue_SensorDataHandle;
const osMessageQueueAttr_t Queue_SensorData_attributes = {
  .name = "Queue_SensorData"
};
/* Definitions for Queue_BluetoothRx */
osMessageQueueId_t Queue_BluetoothRxHandle;
const osMessageQueueAttr_t Queue_BluetoothRx_attributes = {
  .name = "Queue_BluetoothRx"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTask_SensorRead(void *argument);
void StartTask_Control(void *argument);
void StartTask_Comm(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of Queue_SensorData */
  Queue_SensorDataHandle = osMessageQueueNew (1, sizeof(AllSensorData), &Queue_SensorData_attributes);

  /* creation of Queue_BluetoothRx */
  Queue_BluetoothRxHandle = osMessageQueueNew (128, 1, &Queue_BluetoothRx_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_SensorRead */
  Task_SensorReadHandle = osThreadNew(StartTask_SensorRead, NULL, &Task_SensorRead_attributes);

  /* creation of Task_Control */
  Task_ControlHandle = osThreadNew(StartTask_Control, NULL, &Task_Control_attributes);

  /* creation of Task_Comm */
  Task_CommHandle = osThreadNew(StartTask_Comm, NULL, &Task_Comm_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTask_SensorRead */
/**
* @brief Function implementing the Task_SensorRead thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_SensorRead */
__weak void StartTask_SensorRead(void *argument)
{
  /* USER CODE BEGIN StartTask_SensorRead */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask_SensorRead */
}

/* USER CODE BEGIN Header_StartTask_Control */
/**
* @brief Function implementing the Task_Control thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_Control */
__weak void StartTask_Control(void *argument)
{
  /* USER CODE BEGIN StartTask_Control */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask_Control */
}

/* USER CODE BEGIN Header_StartTask_Comm */
/**
* @brief Function implementing the Task_Comm thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_Comm */
__weak void StartTask_Comm(void *argument)
{
  /* USER CODE BEGIN StartTask_Comm */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask_Comm */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

