#include "usbd_composite.h"
#include "usbd_desc.h"
#include "usbd_hid.h"
#include "usbd_cdc.h"
#include "usbd_ctlreq.h"

static uint8_t USBD_Composite_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx);

static uint8_t USBD_Composite_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx);

static uint8_t USBD_Composite_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static uint8_t USBD_Composite_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t USBD_Composite_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t USBD_Composite_EP0_RxReady (USBD_HandleTypeDef *pdev);

static uint8_t *USBD_Composite_GetFSCfgDesc (uint16_t *length);

static uint8_t *USBD_Composite_GetHSCfgDesc (uint16_t *length);

static uint8_t *USBD_Composite_GetOtherSpeedCfgDesc (uint16_t *length);

static uint8_t *USBD_Composite_GetOtherSpeedCfgDesc (uint16_t *length);

static uint8_t *USBD_Composite_GetDeviceQualifierDescriptor (uint16_t *length);

USBD_ClassTypeDef USBD_Composite = 
{
  USBD_Composite_Init,
  USBD_Composite_DeInit,
  USBD_Composite_Setup,
  NULL, //TODO
  USBD_Composite_EP0_RxReady,
  USBD_Composite_DataIn,
  USBD_Composite_DataOut,
  NULL, //TODO
  NULL, //TODO
  NULL, //TODO
  USBD_Composite_GetHSCfgDesc,  
  USBD_Composite_GetFSCfgDesc,    
  USBD_Composite_GetOtherSpeedCfgDesc, 
  USBD_Composite_GetDeviceQualifierDescriptor,
};

static int classes;

static USBD_ClassTypeDef *USBD_Classes[MAX_CLASSES];

int in_endpoint_to_class[MAX_ENDPOINTS];

int out_endpoint_to_class[MAX_ENDPOINTS];

const uint8_t *config_descriptor;

static uint16_t descriptor_size;

void USBD_Composite_Set_Descriptor(const uint8_t *descriptor, uint16_t size) {
    config_descriptor = descriptor;
    descriptor_size = size;
}

void USBD_Composite_Set_Classes(USBD_ClassTypeDef *class0, USBD_ClassTypeDef *class1) {
    USBD_Classes[0] = class0;
    USBD_Classes[1] = class1;
    classes = 2;
}

static uint8_t USBD_Composite_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
  for(int cls = 0; cls < classes; cls++) {
      if (USBD_Classes[cls]->Init(pdev, cfgidx) != USBD_OK) {
          return USBD_FAIL;
      }
  }
  
  return USBD_OK;
}

static uint8_t  USBD_Composite_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
  for(int cls = 0; cls < classes; cls++) {
      if (USBD_Classes[cls]->DeInit(pdev, cfgidx) != USBD_OK) {
          return USBD_FAIL;
      }
  }
  
  return USBD_OK;
}

static uint8_t USBD_Composite_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
  switch (req->bmRequest & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_CLASS :
      return USBD_Classes[req->wIndex]->Setup(pdev, req);
    
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest) {
          
        case USB_REQ_GET_DESCRIPTOR :
          for(int cls = 0; cls < classes; cls++) {
            if (USBD_Classes[cls]->Setup(pdev, req) != USBD_OK) {
              return USBD_FAIL;
            }
          }
        
        break;
        
        case USB_REQ_GET_INTERFACE :
        case USB_REQ_SET_INTERFACE :
        return USBD_Classes[req->wIndex]->Setup(pdev, req);
      }
  }
  return USBD_OK;
}

static uint8_t USBD_Composite_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum) {
  int class_index;
  
  class_index = in_endpoint_to_class[epnum];

  return USBD_Classes[class_index]->DataIn(pdev, epnum);
}

static uint8_t USBD_Composite_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum) {      
  int class_index;
  
  class_index = out_endpoint_to_class[epnum];
  
  return USBD_Classes[class_index]->DataOut(pdev, epnum);
  
}

static uint8_t USBD_Composite_EP0_RxReady (USBD_HandleTypeDef *pdev) {
  for(int cls = 0; cls < classes; cls++) {
    if (USBD_Classes[cls]->EP0_RxReady != NULL) {
        if (USBD_Classes[cls]->EP0_RxReady(pdev) != USBD_OK) {
        return USBD_FAIL;
      }
    }
  }
  return USBD_OK;
}

static uint8_t  *USBD_Composite_GetFSCfgDesc (uint16_t *length) {
  *length = descriptor_size;
  return config_descriptor;
}

static uint8_t  *USBD_Composite_GetHSCfgDesc (uint16_t *length) {
  *length = descriptor_size;
  return config_descriptor;
}

static uint8_t  *USBD_Composite_GetOtherSpeedCfgDesc (uint16_t *length) {
  *length = descriptor_size;
  return config_descriptor;
}

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_Composite_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

uint8_t  *USBD_Composite_GetDeviceQualifierDescriptor (uint16_t *length) {
  *length = sizeof (USBD_Composite_DeviceQualifierDesc);
  return USBD_Composite_DeviceQualifierDesc;
}
