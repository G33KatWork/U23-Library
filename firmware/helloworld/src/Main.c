#include <stdint.h>

#include <RCC.h>
#include <System.h>
#include <LED.h>

void Delay(uint32_t time);

int main()
{
	InitializeSystem();
	SysTick_Config(HCLKFrequency()/100);
	InitializeLEDs();

	while(1)
	{
		SetLEDs(0x5);
		Delay(100);
		SetLEDs(0xA);
		Delay(100);
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
