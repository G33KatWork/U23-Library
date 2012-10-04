#include <stdint.h>
#include <stdio.h>

#include <stm32f4xx/stm32f4xx.h>

#include <usb/usb_dcd_int.h>
#include <usbd/usbd_core.h>

#include <SystemInit.h>

#include <USB_DeviceDescriptor.h>
#include <USB_DeviceEvents.h>
#include <USB_Device.h>

#include <LED.h>
#include <USART.h>

void Delay(uint32_t time);

USB_OTG_CORE_HANDLE  USB_OTG_dev;

int main()
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
	
	InitializeLEDs();
	MyUSART_Init();

	setvbuf(stdout, 0, _IONBF, 0);
	printf("Hello!\r\n");

	USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_DEV_cb, &USR_cb);

	while(1)
	{
		SetLEDs(0x5);
		Delay(50);
		SetLEDs(0xA);
		Delay(50);
	}
}


volatile uint32_t SysTickCounter=0;

void Delay(uint32_t time)
{
	uint32_t end=SysTickCounter+time;
	while(SysTickCounter!=end);
}

void SysTick_Handler()
{  
	SysTickCounter++;
}

void OTG_FS_IRQHandler(void)
{
    USBD_OTG_ISR_Handler(&USB_OTG_dev);
}
