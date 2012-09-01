#include <stdint.h>
#include <stdio.h>

#include <stm32f4xx/stm32f4xx.h>

#include <SystemInit.h>
#include <USART.h>
#include <LED.h>

void Delay(uint32_t time);

int main()
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
	
	InitializeLEDs();
	MyUSART_Init();

	setvbuf(stdout, 0, _IONBF, 0);
	printf("Hello!\r\n");

	char c;
	while(1)
	{
		if(USART_ReceiveChar(&c))
			USART_SendChar(c);

		SetLEDs(c);
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
