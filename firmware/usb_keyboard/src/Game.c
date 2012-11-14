#include <game/Game.h>
#include <game/Debug.h>
#include <platform/USART.h>

#include <stdio.h>
#include <string.h>

#include <usbd_hid_core.h>
#include <USB_DeviceEvents.h>
#include <USB_DeviceDescriptor.h>
#include <USB_HID_Constants.h>

#include <usb/usb_dcd_int.h>
#include <usbd/usbd_core.h>

void Init(struct Gamestate*);
void OnEnter(struct Gamestate* state);
void OnLeave(struct Gamestate* state);
void Update(uint32_t);
void Draw(Bitmap *);

uint8_t *USBD_HID_GetStruct (uint8_t key0, uint8_t modifier);

Gamestate InitState = { Init, OnEnter, OnLeave, Update, Draw };
Game* TheGame = &(Game) {&InitState};

USB_OTG_CORE_HANDLE  USB_OTG_dev;

void Init(struct Gamestate* state)
{
	EnableDebugOutput(DEBUG_USART);

	USBD_Init(&USB_OTG_dev,
	               USB_OTG_FS_CORE_ID,
	               &USR_desc,
	               &USBD_HID_cb,
	               &USR_cb);
	// USB_SETUP_REQ req;
	// req.bmRequest = USB_REQ_TYPE_VENDOR;
	// req.bRequest = USB_REQ_SET_VALUE;
	// req.wValue = 
	// req.wIndex
	// req.wLength
}

void OnEnter(struct Gamestate* state)
{
}

void OnLeave(struct Gamestate* state)
{
}

void Update(uint32_t a)
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

void Draw(Bitmap *b)
{
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

