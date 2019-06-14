/**
  ******************************************************************************
  * @file    USBD_HID_CDC_cdc.c
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   This file provides the HID core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                HID Class  Description
  *          =================================================================== 
  *           This module manages the HID class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (HID) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - The Mouse protocol
  *             - Usage Page : Generic Desktop
  *             - Usage : Joystick
  *             - Collection : Application 
  *      
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *           
  *      
  *  @endverbatim
  *
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

/* Includes ------------------------------------------------------------------*/
#include "usbd_hid_cdc.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"


/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_HID 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_HID_CDC_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_HID_CDC_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_HID_CDC_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 




/** @defgroup USBD_HID_CDC_Private_FunctionPrototypes
  * @{
  */


static uint8_t  USBD_HID_CDC_Init (USBD_HandleTypeDef *pdev,
                               uint8_t cfgidx);

static uint8_t  USBD_HID_CDC_DeInit (USBD_HandleTypeDef *pdev,
                                 uint8_t cfgidx);

static uint8_t  USBD_HID_CDC_Setup (USBD_HandleTypeDef *pdev,
                                USBD_SetupReqTypedef *req);

static uint8_t  *USBD_HID_CDC_GetCfgDesc (uint16_t *length);

static uint8_t  *USBD_HID_CDC_GetDeviceQualifierDesc (uint16_t *length);

static uint8_t  USBD_HID_CDC_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_HID_CDC_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);
/**
  * @}
  */ 

/** @defgroup USBD_HID_CDC_Private_Variables
  * @{
  */ 
USBD_ClassTypeDef  USBD_HID_CDC =
{
  USBD_HID_CDC_Init,
  USBD_HID_CDC_DeInit,
  USBD_HID_CDC_Setup,
  NULL, /*EP0_TxSent*/  
  NULL, /*EP0_RxReady*/
  USBD_HID_CDC_DataIn, /*DataIn*/
  USBD_HID_CDC_DataOut, /*DataOut*/
  NULL, /*SOF */
  NULL,
  NULL,      
  USBD_HID_CDC_GetCfgDesc,
  USBD_HID_CDC_GetCfgDesc,
  USBD_HID_CDC_GetCfgDesc,
  USBD_HID_CDC_GetDeviceQualifierDesc,
};

/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_CDC_Desc[USB_HID_DESC_SIZE]  __ALIGN_END  =
{
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  HID_CUSTOM_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
};

/* USB device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_CDC_CfgDesc[USB_HID_CONFIG_DESC_SIZE]  __ALIGN_END =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
  USB_HID_CONFIG_DESC_SIZE,
  /* wTotalLength: Bytes returned */
  0x00,
  0x03,         /*bNumInterfaces: 3 interfaces*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing the configuration*/
  0xE0,         /*bmAttributes: bus powered and Support Remote Wake-up */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

  /*Interface Association Descriptor*/
  /* 09 */
  0x08,         /*length*/
  0x0B,         /*Interface association descriptor type*/
  0x00,         /*The first associated interface */
  0x02,         /*Number of continuous associated interface*/
  0x02,         /*bInterfaceClass of the first interface*/
  0x02,         /*bInterfaceSubclass of the first interface*/
  0x01,         /*bInterfaceProtocol of the first interface*/
  0x00,         /*Interface string index*/

  /************** Descriptor of CDC Control interface *************/
  /* 17 */
  0x09,         /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
  /* Interface descriptor type */
  0x00,         /* bInterfaceNumber: Number of Interface */
  0x00,         /* bAlternateSetting: Alternate setting */
  0x01,         /* bNumEndpoints: One endpoint used */
  0x02,         /* bInterfaceClass: Communication Interface Class */
  0x02,         /* bInterfaceSubClass: Abstract Control Model */
  0x00,         /* bInterfaceProtocol: */
  0x00,         /* iInterface: */

  /*Header Functional Descriptor*/
  /* 26 */
  0x05,         /* bLength: Endpoint Descriptor size */
  0x24,         /* bDescriptorType: CS_INTERFACE */
  0x00,         /* bDescriptorSubtype: Header Func Desc */
  0x10,         /* bcdCDC: spec release number */
  0x01,

  /*Call Management Functional Descriptor*/
  /* 31 */
  0x05,         /* bFunctionLength */
  0x24,         /* bDescriptorType: CS_INTERFACE */
  0x01,         /* bDescriptorSubtype: Call Management Func Desc */
  0x00,         /* bmCapabilities: D0+D1 */
  0x00,         /* bDataInterface: 0 */

  /*ACM Functional Descriptor*/
  /* 36 */
  0x04,         /* bFunctionLength */
  0x24,         /* bDescriptorType: CS_INTERFACE */
  0x02,         /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,         /* bmCapabilities */

  /*Union Functional Descriptor*/
  /* 40 */
  0x05,          /* bFunctionLength */
  0x24,          /* bDescriptorType: CS_INTERFACE */
  0x06,          /* bDescriptorSubtype: Union func desc */
  0x00,          /* bMasterInterface: Communication class interface */
  0x01,          /* bSlaveInterface0: Data Class Interface */

  /*Endpoint 2 Descriptor*/
  /* 45 */
  0x07,         /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
  CDC_EPCMD_ADDR,   /* bEndpointAddress */
  0x03,         /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SIZE), /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SIZE),
  0x10,         /* bInterval: */
  /************** Descriptor of CDC Control interface *************/

  /************** Descriptor of CDC Data interface ****************/
  /* 52 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
  0x01,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x02,         /*bNumEndpoints: Two endpoints used*/
  0x0A,         /*bInterfaceClass: CDC*/
  0x00,         /*bInterfaceSubClass:*/
  0x00,         /*bInterfaceProtocol:*/
  0x00,         /*iInterface:*/

  /******************** Descriptor of CDC endpoint ****************/
  /* 61 */
  0x07,         /*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT,/* bDescriptorType: Endpoint */
  CDC_EP0IN_ADDR,/*bEndpointAddress:*/
  0x02,         /*bmAttributes: Bulk, 0x03 = interrupt */
  LOBYTE(VCOM_DATA_SIZE), /*wMaxPacketSize:*/
  HIBYTE(VCOM_DATA_SIZE),
  0x00,          /*bInterval: ignore for Bulk transfer*/

  /******************** Descriptor of CDC endpoint ****************/
  /* 68 */
  0x07,         /*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT,/*bDescriptorType: Endpoint*/
  CDC_EP1OUT_ADDR,/*bEndpointAddress:*/
  0x02,         /*bmAttributes: Bulk*/
  LOBYTE(VCOM_DATA_SIZE), /*wMaxPacketSize:*/
  HIBYTE(VCOM_DATA_SIZE),
  0x00,          /*bInterval*/
  /******************** Descriptor of CDC endpoint *****************/

  /************** Descriptor of CDC interface **********************/

//  /************** Descriptor of Joystick interface ****************/
//  /* 75 */
//  0x09,         /*bLength: Interface Descriptor size*/
//  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
//  0x02,         /*bInterfaceNumber: Number of Interface*/
//  0x00,         /*bAlternateSetting: Alternate setting*/
//  0x02,         /*bNumEndpoints*/
//  0x03,         /*bInterfaceClass: HID*/
//  0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
//  0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
//  0,            /*iInterface: Index of string descriptor*/
//  /******************** Descriptor of Joystick HID ********************/
//  /* 84 */
//  0x09,         /*bLength: HID Descriptor size*/
//  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
//  0x11,         /*bcdHID: HID Class Spec release number: 1.11*/
//  0x01,
//  0x00,         /*bCountryCode: Hardware target country*/
//  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
//  0x22,         /*bDescriptorType*/
//  HID_CUSTOM_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
//  0x00,
//  /******************** Descriptor of Joystick IN endpoint ********************/
//  /* 93 */
//  0x07,          /*bLength: Endpoint Descriptor size*/
//  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
//
//  HID_EP0IN_ADDR,/*bEndpointAddress: Endpoint Address (IN)*/
//  0x03,          /*bmAttributes: Interrupt endpoint*/
//  HID_EP0IN_SIZE, /*wMaxPacketSize: 1 Byte max */
//  0x00,
//  HID_FS_BINTERVAL,/*bInterval: Polling Interval */
//  /************** Descriptor of Joystick interface ****************/
//
//  /******************** Descriptor of Joystick OUT endpoint ********************/
//  /* 100 */
//  0x07,          /*bLength: Endpoint Descriptor size*/
//  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
//
//  HID_EP1OUT_ADDR,/*bEndpointAddress: Endpoint Address (IN)*/
//  0x03,          /*bmAttributes: Interrupt endpoint*/
//  HID_EP1OUT_SIZE, /*wMaxPacketSize: 1 Byte max */
//  0x00,
//  HID_FS_BINTERVAL,/*bInterval: Polling Interval */
//  /************** Descriptor of Joystick interface ****************/
//
//  /* 107 */

  /************** Descriptor of MeidaKeyboard interface ****************/
  /* 75 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
  0x02,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x01,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: HID*/
  0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x01,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=custom*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of MeidaKeyboard HID ********************/
  /* 84 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  HID_CUSTOM_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
  /******************** Descriptor of MeidaKeyboard endpoint ********************/
  /* 93 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
  HID_EP0IN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_EP0IN_SIZE, /*wMaxPacketSize: 4 Byte max */
  0x00,
  HID_FS_BINTERVAL,          /*bInterval: Polling Interval (10 ms)*/
  /* 100 */
} ;

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_CDC_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC]  __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0xEF,   /* bDeviceClass (0xEF - Misc), was: 0x00, */
  0x02,   /* bDeviceSubClass (common) */
  0x01,   /* bDeviceProtocol (IAD),  was: 0x00, */
  0x08,   /* bMaxPacketSize0 */
  0x83,
  0x40,
  0x01,
  0x00,
};

__ALIGN_BEGIN static uint8_t HID_CUSTOM_ReportDesc[HID_CUSTOM_REPORT_DESC_SIZE]  __ALIGN_END = {
  // 78 bytes
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x06,        // Usage (Keyboard)
  0xA1, 0x01,        // Collection (Application)
  0x85, 0x01,        //   Report ID (1)
  0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x08,        //   Report Count (8)
  0x19, 0xE0,        //   Usage Minimum (0xE0)
  0x29, 0xE7,        //   Usage Maximum (0xE7)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x03,        //   Report Count (3)
  0x75, 0x08,        //   Report Size (8)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x64,        //   Logical Maximum (100)
  0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
  0x19, 0x00,        //   Usage Minimum (0x00)
//  0x29, 0x65,        //   Usage Maximum (0x65)
  0x29, 0xe7,        //   Usage Maximum (0xe7)
  0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              // End Collection
  0x05, 0x0C,        // Usage Page (Consumer)
  0x09, 0x01,        // Usage (Consumer Control)
  0xA1, 0x01,        // Collection (Application)
  0x85, 0x02,        //   Report ID (2)
  0x05, 0x0C,        //   Usage Page (Consumer)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x08,        //   Report Count (8)
  0x09, 0xB5,        //   Usage (Scan Next Track)
  0x09, 0xB6,        //   Usage (Scan Previous Track)
  0x09, 0xB7,        //   Usage (Stop)
  0x09, 0xB8,        //   Usage (Eject)
  0x09, 0xCD,        //   Usage (Play/Pause)
  0x09, 0xE2,        //   Usage (Mute)
  0x09, 0xE9,        //   Usage (Volume Increment)
  0x09, 0xEA,        //   Usage (Volume Decrement)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              // End Collection
};

/**
  * @}
  */ 

/** @defgroup USBD_HID_CDC_Private_Functions
  * @{
  */ 

/**
  * @brief  USBD_HID_CDC_Init
  *         Initialize the HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_HID_CDC_Init (USBD_HandleTypeDef *pdev,
                               uint8_t cfgidx)
{
  uint8_t ret = 0;
  
  /* Open EP IN */
  USBD_LL_OpenEP(pdev,
                 HID_EP0IN_ADDR,
                 USBD_EP_TYPE_INTR,
                 HID_EP0IN_SIZE);
  
  /* Open EP OUT */
  /* USBD_LL_OpenEP(pdev,
                 HID_EP1OUT_ADDR,
                 USBD_EP_TYPE_INTR,
                 HID_EP1OUT_SIZE);
  */

  /* Open EP IN */
  USBD_LL_OpenEP(pdev,
                 CDC_EP0IN_ADDR,
                 USBD_EP_TYPE_BULK,
                 CDC_EP0IN_SIZE);

  /* Open EP OUT */
  USBD_LL_OpenEP(pdev,
                 CDC_EP1OUT_ADDR,
                 USBD_EP_TYPE_BULK,
                 CDC_EP1OUT_SIZE);

  /* Open Command IN EP */
  USBD_LL_OpenEP(pdev,
                   CDC_EPCMD_ADDR,
                   USBD_EP_TYPE_INTR,
                   CDC_CMD_PACKET_SIZE);

  pdev->pClassData = USBD_malloc(sizeof (USBD_HID_CDC_HandleTypeDef));
  
  if(pdev->pClassData == NULL)
  {
    ret = 1; 
  }
  else
  {
    ((USBD_HID_CDC_HandleTypeDef *)pdev->pClassData)->state = HID_IDLE;

    /* Init  physical Interface components */
    ((USBD_CDC_ItfTypeDef *)pdev->pUserData)->Init();

    /* Init Xfer states */
    ((USBD_HID_CDC_HandleTypeDef *)pdev->pClassData)->CDCTxState = 0;
    ((USBD_HID_CDC_HandleTypeDef *)pdev->pClassData)->CDCRxState = 0;

    ((USBD_HID_CDC_HandleTypeDef *)pdev->pClassData)->HIDRxState = 0;

    /* Prepare Out endpoint to receive next packet */
    USBD_LL_PrepareReceive(pdev,
        CDC_EP1OUT_ADDR,
        ((USBD_HID_CDC_HandleTypeDef *)pdev->pClassData)->CDCRxBuffer,
            CDC_EP1OUT_SIZE);

    /*USBD_LL_PrepareReceive(pdev,
        HID_EP1OUT_ADDR,
          ((USBD_HID_CDC_HandleTypeDef *)pdev->pClassData)->HIDRxBuffer,
            HID_EP1OUT_SIZE);
   */
  }

  return ret;
}

/**
  * @brief  USBD_HID_CDC_DeInit
  *         DeInitialize the HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_HID_CDC_DeInit (USBD_HandleTypeDef *pdev,
                                 uint8_t cfgidx)
{
  /* Close HID EPs */
  USBD_LL_CloseEP(pdev, HID_EP0IN_ADDR);
  //USBD_LL_CloseEP(pdev, HID_EP1OUT_ADDR);
  USBD_LL_CloseEP(pdev, CDC_EP0IN_ADDR);
  USBD_LL_CloseEP(pdev, CDC_EP1OUT_ADDR);
  
  /* Open Command IN EP */
  USBD_LL_CloseEP(pdev, CDC_EPCMD_ADDR);

  /* FRee allocated memory */
  if(pdev->pClassData != NULL)
  {
  ((USBD_CDC_ItfTypeDef *)pdev->pUserData)->DeInit();
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  } 
  
  return USBD_OK;
}

/**
  * @brief  USBD_HID_CDC_Setup
  *         Handle the HID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_HID_CDC_Setup (USBD_HandleTypeDef *pdev,
                                USBD_SetupReqTypedef *req)
{
  uint16_t len = 0;
  uint8_t  *pbuf = NULL;

  USBD_HID_CDC_HandleTypeDef     *hhid = (USBD_HID_CDC_HandleTypeDef*) pdev->pClassData;
  

  /*if (req) {
    uint8_t dbg[255];

    uint8_t ll = sprintf(dbg,
        "\r\nreq->bmRequest=0x%x\r\nreq->bRequest=0x%x\r\nreq->wValue=0x%x\r\nreq->wIndex=0x%x\r\nreq->wLength=0x%x\r\n",
        req->bmRequest,
        req->bRequest,
        req->wValue,
        req->wIndex,
        req->wLength
        );
    utx (dbg, ll);
  }*/

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {


  case USB_REQ_TYPE_CLASS :
      if ((((req->bmRequest & 0x0f) == 1) && (req->wIndex == 2)) ||
         (((req->bmRequest & 0x0f) == 2) && ((req->wIndex == HID_EP0IN_ADDR)))) {

    switch (req->bRequest)
    {
    case HID_REQ_SET_PROTOCOL:
      hhid->Protocol = (uint8_t)(req->wValue);
      break;

    case HID_REQ_GET_PROTOCOL:
      USBD_CtlSendData (pdev,
              (uint8_t *)&hhid->Protocol,
              1);
      break;

    case HID_REQ_SET_IDLE:
      hhid->IdleState = (uint8_t)(req->wValue >> 8);
      break;

    case HID_REQ_GET_IDLE:
      USBD_CtlSendData (pdev,
              (uint8_t *)&hhid->IdleState,
              1);
      break;

    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL;
    }
    } else if (req->wLength)
    {
      if (req->bmRequest & 0x80)
      {
        ((USBD_CDC_ItfTypeDef *)pdev->pUserData)->Control(req->bRequest,
                                                          (uint8_t *)hhid->CDCdata,
                                                          req->wLength);
          USBD_CtlSendData (pdev,
                            (uint8_t *)hhid->CDCdata,
                            req->wLength);
      }
      else
      {
        hhid->CDCCmdOpCode = req->bRequest;
        hhid->CDCCmdLength = req->wLength;

        USBD_CtlPrepareRx (pdev,
                           (uint8_t *)hhid->CDCdata,
                           req->wLength);
      }
      
    }
    else
    {
      ((USBD_CDC_ItfTypeDef *)pdev->pUserData)->Control(req->bRequest,
                                                        (uint8_t*)req,
                                                        0);
    }
    break;
    
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_DESCRIPTOR: 
      if( req->wValue >> 8 == HID_REPORT_DESC)
      {
        len = MIN(HID_CUSTOM_REPORT_DESC_SIZE, req->wLength);
        pbuf = HID_CUSTOM_ReportDesc;
      }
      else if( req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
      {
        pbuf = USBD_HID_CDC_Desc;
        len = MIN(USB_HID_DESC_SIZE, req->wLength);
      }
      
//      {
//          uint8_t dbg[512];
//          uint16_t ll = 0;
//
//          for (int i=0; i<len; i++)
//            ll += sprintf(&dbg[ll], " %02x", pbuf[i]);
//
//          ll += sprintf(&dbg[ll], "\r\n");
//          utx (dbg, ll);
//        }

      USBD_CtlSendData (pdev, 
                        pbuf,
                        len);
      
      break;
      
    case USB_REQ_GET_INTERFACE :
      if ((uint8_t)(req->wValue) == HID_EP0IN_ADDR)
      {
        USBD_CtlSendData (pdev,
                        (uint8_t *)&hhid->HIDAltSetting, 1);
      } else {
        USBD_CtlSendData (pdev,
                    (uint8_t *)&hhid->CDCAltSetting, 1);
    }
      break;
      
    case USB_REQ_SET_INTERFACE :
      if ((uint8_t)(req->wValue) == HID_EP0IN_ADDR)
      {
        hhid->HIDAltSetting = (uint8_t)(req->wValue);
      } else {
        hhid->CDCAltSetting = (uint8_t)(req->wValue);
    }
      break;
    }
  }
  return USBD_OK;
}

/**
  * @brief  USBD_HID_CDC_SendReport
  *         Send HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t USBD_HID_SendReport     (USBD_HandleTypeDef  *pdev,
                                 uint8_t *report,
                                 uint16_t len)
{
  USBD_HID_CDC_HandleTypeDef     *hhid = (USBD_HID_CDC_HandleTypeDef*)pdev->pClassData;
  
  if (pdev->dev_state == USBD_STATE_CONFIGURED )
  {
    if(hhid->state == HID_IDLE)
    {
      hhid->state = HID_BUSY;

      USBD_LL_Transmit (pdev, 
                        HID_EP0IN_ADDR,
                        report,
                        len);
      return USBD_OK;
    }
  }
  return USBD_FAIL;
}

/**
  * @brief  USBD_HID_CDC_GetPollingInterval
  *         return polling interval from endpoint descriptor
  * @param  pdev: device instance
  * @retval polling interval
  */
uint32_t USBD_HID_CDC_GetPollingInterval (USBD_HandleTypeDef *pdev)
{
  uint32_t polling_interval = 0;

  /* HIGH-speed endpoints */
  if(pdev->dev_speed == USBD_SPEED_HIGH)
  {
   /* Sets the data transfer polling interval for high speed transfers. 
    Values between 1..16 are allowed. Values correspond to interval 
    of 2 ^ (bInterval-1). This option (8 ms, corresponds to HID_HS_BINTERVAL */
    polling_interval = (((1 <<(HID_HS_BINTERVAL - 1)))/8);
  }
  else   /* LOW and FULL-speed endpoints */
  {
    /* Sets the data transfer polling interval for low and full 
    speed transfers */
    polling_interval =  HID_FS_BINTERVAL;
  }
  
  return ((uint32_t)(polling_interval));
}

/**
  * @brief  USBD_HID_CDC_GetCfgDesc
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_HID_CDC_GetCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_HID_CDC_CfgDesc);
  return USBD_HID_CDC_CfgDesc;
}


/**
  * @brief  USBD_HID_CDC_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_HID_CDC_DataIn (USBD_HandleTypeDef *pdev,
                              uint8_t epnum)
{
  USBD_HID_CDC_HandleTypeDef *h = (USBD_HID_CDC_HandleTypeDef *)pdev->pClassData;
  
  /* Ensure that the FIFO is empty before a new transfer, this condition could 
  be caused by  a new transfer before the end of the previous transfer */
  if (h != NULL) {

  epnum |= 0x80;
  if (epnum == HID_EP0IN_ADDR) {
    h->state = HID_IDLE;
  } else {
    h->CDCTxState = 0;
  }

  return USBD_OK;
  }

  return USBD_FAIL;
}

/**
  * @brief  USBD_HID_CDC_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */

static uint8_t  USBD_HID_CDC_DataOut (USBD_HandleTypeDef *pdev,
                uint8_t epnum)
{
  USBD_HID_CDC_HandleTypeDef *h = (USBD_HID_CDC_HandleTypeDef *)pdev->pClassData;

    /* USB data will be immediately processed, this allow next USB traffic being
  NAKed till the end of the application Xfer */
  if(h != NULL && epnum != HID_EP1OUT_ADDR)
  {
    h->CDCRxLength = USBD_LL_GetRxDataSize (pdev, epnum);
      ((USBD_CDC_ItfTypeDef *)pdev->pUserData)->Receive(h->CDCRxBuffer, &h->CDCRxLength);
      USBD_LL_PrepareReceive(pdev, CDC_EP1OUT_ADDR, h->CDCRxBuffer, CDC_EP1OUT_SIZE);

      return USBD_OK;
  }
  // else
  // {
  //   h->HIDRxLength = USBD_LL_GetRxDataSize (pdev, epnum);
  //     USBD_LL_PrepareReceive(pdev, HID_EP1OUT_ADDR, h->HIDRxBuffer, HID_EP1OUT_SIZE);

  //     return USBD_OK;
  // }

    return USBD_FAIL;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
static uint8_t  *USBD_HID_CDC_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_HID_CDC_DeviceQualifierDesc);
  return USBD_HID_CDC_DeviceQualifierDesc;
}


/**
* @brief  USBD_CDC_RegisterInterface
  * @param  pdev: device instance
  * @param  fops: CD  Interface callback
  * @retval status
  */
uint8_t  USBD_CDC_RegisterInterface  (USBD_HandleTypeDef   *pdev,
                                      USBD_CDC_ItfTypeDef *fops)
{
  uint8_t  ret = USBD_FAIL;

  if(fops != NULL)
  {
    pdev->pUserData= fops;
    ret = USBD_OK;
  }

  return ret;
}

/**
  * @brief  USBD_CDC_SetTxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Tx Buffer
  * @retval status
  */
uint8_t  USBD_CDC_SetTxBuffer  (USBD_HandleTypeDef   *pdev,
                                uint8_t  *pbuff,
                                uint16_t length)
{
  USBD_HID_CDC_HandleTypeDef   *h = (USBD_HID_CDC_HandleTypeDef*) pdev->pClassData;

  h->CDCTxBuffer = pbuff;
  h->CDCTxLength = length;

  return USBD_OK;
}


/**
  * @brief  USBD_CDC_SetRxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Rx Buffer
  * @retval status
  */
uint8_t  USBD_CDC_SetRxBuffer  (USBD_HandleTypeDef   *pdev,
                                   uint8_t  *pbuff)
{
  USBD_HID_CDC_HandleTypeDef   *h = (USBD_HID_CDC_HandleTypeDef*) pdev->pClassData;

  h->CDCRxBuffer = pbuff;

  return USBD_OK;
}

/**
  * @brief  USBD_CDC_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
uint8_t  USBD_CDC_TransmitPacket(USBD_HandleTypeDef *pdev)
{
  USBD_HID_CDC_HandleTypeDef   *h = (USBD_HID_CDC_HandleTypeDef*) pdev->pClassData;

  if(pdev->pClassData != NULL)
  {
    if(h->CDCTxState == 0)
    {
      /* Tx Transfer in progress */
      h->CDCTxState = 1;

      /* Transmit next packet */
      USBD_LL_Transmit(pdev, CDC_EP0IN_ADDR, h->CDCTxBuffer, h->CDCTxLength);
      return USBD_OK;
    }

    return USBD_BUSY;
  }

  return USBD_FAIL;
}


/**
  * @brief  USBD_CDC_ReceivePacket
  *         prepare OUT Endpoint for reception
  * @param  pdev: device instance
  * @retval status
  */
uint8_t  USBD_CDC_ReceivePacket(USBD_HandleTypeDef *pdev)
{
  USBD_HID_CDC_HandleTypeDef   *h = (USBD_HID_CDC_HandleTypeDef*) pdev->pClassData;

  /* Suspend or Resume USB Out process */
  if(pdev->pClassData != NULL)
  {
      /* Prepare Out endpoint to receive next packet */
      USBD_LL_PrepareReceive(pdev, CDC_EP1OUT_ADDR, h->CDCRxBuffer, VCOM_DATA_SIZE);

      return USBD_OK;
  }

  return USBD_FAIL;
}

/**
  * @}
  */ 


/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
