#ifndef __USB_DEVICE_H_
#define __USB_DEVICE_H_

#include  <usbd/usbd_ioreq.h>


#define USB_DEV_CONFIG_DESC_SIZ       18

#define USB_REQ_SET_VALUE              1
#define USB_REQ_GET_VALUE              2

extern USBD_Class_cb_TypeDef  USBD_DEV_cb;

#endif  // __USB_DEVICE_H_
