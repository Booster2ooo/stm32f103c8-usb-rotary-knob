/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "tim.h"
#include "usbd_hid_cdc.h"
#include "usbd_cdc_if.h"
#include "command_handler.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern USBD_HandleTypeDef hUsbDeviceFS;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osMailQDef(rotaryMailQ, 16, RotaryMail);
osMailQId rotaryMailQ;
osMailQDef(commandMailQ, 16, CommandMail);
osMailQId commandMailQ;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId blinkTaskHandle;
osThreadId commandTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void DisableTimer();
void Blink();
void Switch_Rising(uint32_t rotary_id);
void Switch_Falling(uint32_t rotary_id);
void Clockwise(uint32_t rotary_id);
void Counter_Clockwise(uint32_t rotary_id);
void Act(RotaryActionConfig action_config);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartBlinkTask(void const * argument);
void StartCommandHandlerTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

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

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  rotaryMailQ = osMailCreate(osMailQ(rotaryMailQ), NULL);
  if (rotaryMailQ == NULL)
  {
    Error_Handler();
  }
  commandMailQ = osMailCreate(osMailQ(commandMailQ), NULL);
  if (commandMailQ == NULL)
  {
    Error_Handler();
  }
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of blinkTask */
  osThreadDef(blinkTask, StartBlinkTask, osPriorityIdle, 0, 128);
  blinkTaskHandle = osThreadCreate(osThread(blinkTask), NULL);

  /* definition and creation of commandTask */
  osThreadDef(commandTask, StartCommandHandlerTask, osPriorityIdle, 0, 128);
  commandTaskHandle = osThreadCreate(osThread(commandTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  if ((CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)==CoreDebug_DHCSR_C_DEBUGEN_Msk) {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  //reenumerate
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_Delay(500);
  }

  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN StartDefaultTask */
  Init_Rotary(
    &rotary1, 
    ROTARYID1,
    RCLK_GPIO_Port,
    RCLK_Pin,
    RDT_GPIO_Port,
    RDT_Pin,
    RSW_GPIO_Port,
    RSW_Pin
  );
  Init_Rotary(
    &rotary2,
    ROTARYID2,
    RCLK2_GPIO_Port,
    RCLK2_Pin,
    RDT2_GPIO_Port,
    RDT2_Pin,
    RSW2_GPIO_Port,
    RSW2_Pin
  );
  HAL_TIM_Base_Start_IT(&htim2);
  DisableTimer();

  Init_Mediakeyboard();
  Init_Command_Handler();

  RotaryMail *rptr;
  osEvent event;
  /* Infinite loop */
  for(;;)
  {
    event = osMailGet(rotaryMailQ, osWaitForever);
    if(event.status == osErrorParameter)
    {
      Error_Handler();
    }
    else if (event.status == osEventMail)
    {
      rptr = event.value.p;
      if (rptr->EventCode & ebPRESSED)
      {
        DisableTimer();
        Switch_Rising(rptr->Rotary->Id);
        Blink();
      }
      else if (rptr->EventCode & ebRELEASED)
      {
        DisableTimer();
        Switch_Falling(rptr->Rotary->Id);
        Blink();
      }
      else if (rptr->EventCode & ebCLOCKWISE)
      {
        Clockwise(rptr->Rotary->Id);
        Blink();
      }
      else if (rptr->EventCode & ebCOUNTERCLOCKWISE)
      {
        Counter_Clockwise(rptr->Rotary->Id);
        Blink();
      }
      osMailFree(rotaryMailQ, rptr);
    }
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartBlinkTask */
/**
* @brief Function implementing the blinkTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBlinkTask */
void StartBlinkTask(void const * argument)
{
  /* USER CODE BEGIN StartBlinkTask */
  /* Infinite loop */
  for(;;)
  {
    osEvent event = osSignalWait(0, osWaitForever);
    if (event.status & 0x80)
    {
      Error_Handler();
    }
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    osDelay(50);
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  }
  /* USER CODE END StartBlinkTask */
}

/* USER CODE BEGIN Header_StartCommandHandlerTask */
/**
* @brief Function implementing the commandTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCommandHandlerTask */
void StartCommandHandlerTask(void const * argument)
{
  /* USER CODE BEGIN StartCommandHandlerTask */
  /* Infinite loop */
  CommandMail *rptr;
  osEvent event;
  /* Infinite loop */
  for(;;)
  {
    event = osMailGet(commandMailQ, osWaitForever);
    if(event.status == osErrorParameter)
    {
      Error_Handler();
    }
    else if (event.status == osEventMail)
    {
      rptr = event.value.p;
      if (!rptr->EventCode)
      {
        printf("\r\n>> [Error] \"%s\" unrecognized.\r\n", rptr->Command);
      }
      else if (rptr->EventCode & ebCONNECTED)
      {
        printf(">> Hello\r\n");
      }
      if (rptr->EventCode & ebWAITING)
      {
        utx(&rptr->LastValue, 1);
        HAL_Delay(1);
      }
      else if (rptr->EventCode & ebCOMMAND)
      {
        printf("\r\n>> [OK] \"%s\" command accepted\r\n", rptr->Command);
      }
      else if (rptr->EventCode & ebOVERFLOWN)
      {
        printf("\r\n>> [Error] \"%s\" too long.\r\n", rptr->Command);
      }
      osMailFree(commandMailQ, rptr);
    }
  }
  /* USER CODE END StartCommandHandlerTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void DisableTimer()
{
  __HAL_TIM_DISABLE(&htim2);
  htim2.Instance->EGR = TIM_EGR_UG; // reset counter and prescaler
}

void Blink()
{
  if (osSignalSet(blinkTaskHandle, 0x1) != osOK)
  {
    Error_Handler();
  }
}

void Switch_Rising(uint32_t rotary_id)
{
  printf("[%ld] Switch released (rising)\r\n", rotary_id / 0x10);
  Act(rotary_configs[rotary_id / 0x10].Released);
}

void Switch_Falling(uint32_t rotary_id)
{
  printf("[%ld] Switch pressed (falling)\r\n", rotary_id /0x10);
  Act(rotary_configs[rotary_id / 0x10].Pressed);
}

void Clockwise(uint32_t rotary_id)
{
  printf("[%ld] Clockwise\r\n", rotary_id / 0x10);
  Act(rotary_configs[rotary_id / 0x10].Clockwise);
}

void Counter_Clockwise(uint32_t rotary_id)
{
  printf("[%ld] Counter-Clockwise\r\n", rotary_id / 0x10);
  Act(rotary_configs[rotary_id / 0x10].Counterclockwise);
}

void Act(RotaryActionConfig action_config)
{
  if (action_config.Mode == KEYBOARD_MODE)
  {
    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&action_config.Keyboard, sizeof(KeyboardHIDTypeDef));
    HAL_Delay(30);
    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&keyboardHID, sizeof(KeyboardHIDTypeDef));
  }
  else if(action_config.Mode == MEDIA_MODE)
  {
    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&action_config.Media, sizeof(MediaHIDTypeDef));
    HAL_Delay(30);
    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&mediaHID, sizeof(MediaHIDTypeDef));
  }
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
