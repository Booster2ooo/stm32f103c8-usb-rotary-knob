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
#include "rotary.h"
#include "usbd_hid_cdc.h"
#include "usbd_cdc_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern USBD_HandleTypeDef hUsbDeviceFS;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// USB media codes
#define USB_HID_SCAN_NEXT 0x01
#define USB_HID_SCAN_PREV 0x02
#define USB_HID_STOP      0x04
#define USB_HID_EJECT     0x08
#define USB_HID_PAUSE     0x10
#define USB_HID_MUTE      0x20
#define USB_HID_VOL_UP    0x40
#define USB_HID_VOL_DEC   0x80

// USB keyboard codes
#define USB_HID_MODIFIER_LEFT_CTRL   0x01
#define USB_HID_MODIFIER_LEFT_SHIFT  0x02
#define USB_HID_MODIFIER_LEFT_ALT    0x04
#define USB_HID_MODIFIER_LEFT_GUI    0x08 // (Win/Apple/Meta)
#define USB_HID_MODIFIER_RIGHT_CTRL  0x10
#define USB_HID_MODIFIER_RIGHT_SHIFT 0x20
#define USB_HID_MODIFIER_RIGHT_ALT   0x40
#define USB_HID_MODIFIER_RIGHT_GUI   0x80
#define USB_HID_KEY_L     0x0F
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osMailQDef(rotaryMailQ, 16, RotaryMail);
osMailQId rotaryMailQ;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId blinkTaskHandle;

// HID Keyboard report
typedef struct {
		uint8_t id;
		uint8_t modifiers;
		uint8_t key1;
		uint8_t key2;
		uint8_t key3;
} keyboardHID_t;
keyboardHID_t keyboardHID = {0};
// HID Media report
typedef struct {
	uint8_t id;
	uint8_t keys;
} mediaHID_t;
mediaHID_t mediaHID = {0};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void DisableTimer();
void Blink();
void Switch_Rising(uint32_t rotary_id);
void Switch_Falling(uint32_t rotary_id);
void Clockwise(uint32_t rotary_id);
void Counter_Clockwise(uint32_t rotary_id);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartBlinkTask(void const * argument);

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
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of blinkTask */
  osThreadDef(blinkTask, StartBlinkTask, osPriorityIdle, 0, 128);
  blinkTaskHandle = osThreadCreate(osThread(blinkTask), NULL);

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
	if ((CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)==CoreDebug_DHCSR_C_DEBUGEN_Msk) {
		/* init code for USB_DEVICE */
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

  // Define report ids
  keyboardHID.id = 1;
  mediaHID.id = 2;

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
}

void Switch_Falling(uint32_t rotary_id)
{
  printf("[%ld] Switch pressed (falling)\r\n", rotary_id /0x10);
}

void Clockwise(uint32_t rotary_id)
{
  printf("[%ld] Clockwise\r\n", rotary_id / 0x10);
	keyboardHID.key1 = USB_HID_KEY_L;
  mediaHID.keys = USB_HID_VOL_UP;
  USBD_HID_SendReport(&hUsbDeviceFS, &mediaHID, sizeof(mediaHID_t));
  HAL_Delay(30);
  mediaHID.keys = 0;
  USBD_HID_SendReport(&hUsbDeviceFS, &mediaHID, sizeof(mediaHID_t));
}

void Counter_Clockwise(uint32_t rotary_id)
{
  printf("[%ld] Counter-Clockwise\r\n", rotary_id / 0x10);
  mediaHID.keys = USB_HID_VOL_DEC;
  USBD_HID_SendReport(&hUsbDeviceFS, &mediaHID, sizeof(mediaHID_t));
  HAL_Delay(30);
  mediaHID.keys = 0;
  USBD_HID_SendReport(&hUsbDeviceFS, &mediaHID, sizeof(mediaHID_t));
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
