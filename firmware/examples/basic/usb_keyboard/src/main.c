#include <System.h>

#include <stdio.h>
#include <string.h>

#include <usbd_hid_core.h>
#include <USB_DeviceEvents.h>
#include <USB_DeviceDescriptor.h>
#include <USB_HID_Constants.h>

#include <usb/usb_dcd_int.h>
#include <usbd/usbd_core.h>


void getNextChar(void);
uint8_t *USBD_HID_GetStruct (uint8_t key0, uint8_t modifier);

USB_OTG_CORE_HANDLE  USB_OTG_dev;

void main()
{
	EnableDebugOutput(DEBUG_USART);

	USBD_Init(&USB_OTG_dev,
	               USB_OTG_FS_CORE_ID,
	               &USR_desc,
	               &USBD_HID_cb,
	               &USR_cb);

	while(1) {
		getNextChar();
	}
}

void getNextChar(void)
{

	char input = 0;
	USART_ReceiveChar(&input);
	//printf("%c", input);
	uint8_t key0, modifier;
	usbHidKeyCodeFromAsciiChar(input, &key0, &modifier);

	uint8_t *buf = USBD_HID_GetStruct(key0, modifier);
	USBD_HID_SendReport (&USB_OTG_dev, 
	                                 buf,
	                                 8);
	buf = USBD_HID_GetStruct(0, 0);
	USBD_HID_SendReport (&USB_OTG_dev, 
	                                 buf,
	                                 8);
}

void OTG_FS_IRQHandler(void)
{
    USBD_OTG_ISR_Handler(&USB_OTG_dev);
}

uint8_t *USBD_HID_GetStruct (uint8_t key0, uint8_t modifier)
{
  static uint8_t HID_Buffer[8];

  HID_Buffer[0] = modifier;
  HID_Buffer[1] = 0;
  HID_Buffer[2] = key0;
  HID_Buffer[3] = 0;
  HID_Buffer[4] = 0;
  HID_Buffer[5] = 0;
  HID_Buffer[6] = 0;
  HID_Buffer[7] = 0;

  return HID_Buffer;
}

