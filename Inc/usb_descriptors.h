#ifndef __USB_DESCRIPTORS_H
#define __USB_DESCRIPTORS_H

#include "usbd_composite.h"
#include "usbd_desc.h"
#include "usbd_hid.h"
#include "usbd_cdc.h"
#include "usbd_ctlreq.h"

#define COMPOSITE_CDC_HID_DESCRIPTOR_SIZE 92

extern uint8_t COMPOSITE_CDC_HID_DESCRIPTOR[COMPOSITE_CDC_HID_DESCRIPTOR_SIZE];

#endif
