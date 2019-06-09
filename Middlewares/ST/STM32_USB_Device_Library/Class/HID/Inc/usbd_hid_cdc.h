/**
  ******************************************************************************
  * @file    usbd_hid.h
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   Header file for the USBD_HID_CDC_core.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_HID_H
#define __USB_HID_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_HID
  * @brief This file is the Header file for usbd_hid.c
  * @{
  */ 


/** @defgroup USBD_HID_CDC_Exported_Defines
  * @{
  */ 

#define CDC_EPCMD_ADDR				   0x81
#define CDC_EP0IN_ADDR                 0x82	/* data IN  */
#define HID_EP0IN_ADDR                 0x83

#define CDC_EP1OUT_ADDR				   0x01 /* data OUT */
#define HID_EP1OUT_ADDR                0x02

#define HID_EP0IN_SIZE                 0x0a
#define HID_EP1OUT_SIZE                0x40

#define VCOM_DATA_SIZE	   			   USB_FS_MAX_PACKET_SIZE
#define CDC_EP0IN_SIZE                 VCOM_DATA_SIZE
#define CDC_EP1OUT_SIZE                VCOM_DATA_SIZE

#define CDC_CMD_PACKET_SIZE            8  /* Control Endpoint Packet size */

//#define USB_HID_CONFIG_DESC_SIZE       107
#define USB_HID_CONFIG_DESC_SIZE       100
#define USB_HID_DESC_SIZE              9
#define HID_BBOX_REPORT_DESC_SIZE      86
#define HID_CUSTOM_REPORT_DESC_SIZE    78

#define HID_DESCRIPTOR_TYPE            0x21
#define HID_REPORT_DESC                0x22

#define HID_HS_BINTERVAL               0x05
#define HID_FS_BINTERVAL               0x05
#define HID_POLLING_INTERVAL           0x05

#define HID_REQ_SET_PROTOCOL           0x0B
#define HID_REQ_GET_PROTOCOL           0x03

#define HID_REQ_SET_IDLE               0x0A
#define HID_REQ_GET_IDLE               0x02

#define HID_REQ_SET_REPORT             0x09
#define HID_REQ_GET_REPORT             0x01

/*---------------------------------------------------------------------*/
/*  CDC definitions                                                    */
/*---------------------------------------------------------------------*/
#define CDC_SEND_ENCAPSULATED_COMMAND  0x00
#define CDC_GET_ENCAPSULATED_RESPONSE  0x01
#define CDC_SET_COMM_FEATURE           0x02
#define CDC_GET_COMM_FEATURE           0x03
#define CDC_CLEAR_COMM_FEATURE         0x04
#define CDC_SET_LINE_CODING            0x20
#define CDC_GET_LINE_CODING            0x21
#define CDC_SET_CONTROL_LINE_STATE     0x22
#define CDC_SEND_BREAK                 0x23

/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */
typedef enum
{
  HID_IDLE = 0,
  HID_BUSY,
}
HID_StateTypeDef; 

typedef struct _USBD_CDC_Itf
{
  int8_t (* Init)          (void);
  int8_t (* DeInit)        (void);
  int8_t (* Control)       (uint8_t, uint8_t * , uint16_t);
  int8_t (* Receive)       (uint8_t *, uint32_t *);

}USBD_CDC_ItfTypeDef;

typedef struct
{
  /* HID fields */
  uint32_t             Protocol;   
  uint32_t             IdleState;  
  uint32_t             HIDAltSetting;
  HID_StateTypeDef     state;

  uint8_t              *HIDRxBuffer;
  uint32_t             HIDRxLength;
  __IO uint32_t        HIDRxState;

  /* CDC fields */
  uint32_t			   CDCAltSetting;
  uint32_t             CDCdata[VCOM_DATA_SIZE/4];      /* Force 32bits alignment */
  uint8_t              CDCCmdOpCode;
  uint8_t              CDCCmdLength;
  uint8_t              *CDCRxBuffer;
  uint8_t              *CDCTxBuffer;
  uint32_t             CDCRxLength;
  uint32_t             CDCTxLength;

  __IO uint32_t        CDCTxState;
  __IO uint32_t        CDCRxState;
}
USBD_HID_CDC_HandleTypeDef;
/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 

extern USBD_ClassTypeDef  USBD_HID_CDC;
#define USBD_HID_CDC_CLASS    &USBD_HID_CDC
/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */ 
uint8_t USBD_HID_SendReport (USBD_HandleTypeDef *pdev,
                                 uint8_t *report,
                                 uint16_t len);

uint32_t USBD_HID_CDC_GetPollingInterval (USBD_HandleTypeDef *pdev);

uint8_t  USBD_CDC_RegisterInterface  (USBD_HandleTypeDef   *pdev,
                                      USBD_CDC_ItfTypeDef *fops);

uint8_t  USBD_CDC_SetTxBuffer        (USBD_HandleTypeDef   *pdev,
                                      uint8_t  *pbuff,
                                      uint16_t length);

uint8_t  USBD_CDC_SetRxBuffer        (USBD_HandleTypeDef   *pdev,
                                      uint8_t  *pbuff);

uint8_t  USBD_CDC_ReceivePacket      (USBD_HandleTypeDef *pdev);

uint8_t  USBD_CDC_TransmitPacket     (USBD_HandleTypeDef *pdev);

/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif  /* __USB_HID_H */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
