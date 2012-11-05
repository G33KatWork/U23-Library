#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <stm32f4xx/stm32f4xx.h>

#include <SystemInit.h>
#include <USART.h>
#include <LED.h>

void Delay(uint32_t time);

//!!! FPU is enabled in SystemInit.c !!!

int main()
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);

	InitializeLEDs();
	MyUSART_Init();

	setvbuf(stdout, 0, _IONBF, 0);
	printf("Hello!\r\n");

	float a = 0.0f;
	a += 0.1f;

	float b = 32.74538f;

	printf("a: %f, b: %f\r\n", a, b);

	while(1)
	{
		a += b;
		printf("Float: %f\r\n", a);

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

void HardFault_Handler(uint32_t* hardfault_args)
{
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;

	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);

	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);

	printf ("\r\n\r\n[Hard fault handler]\r\n");
	printf ("R0 = %x\r\n", stacked_r0);
	printf ("R1 = %x\r\n", stacked_r1);
	printf ("R2 = %x\r\n", stacked_r2);
	printf ("R3 = %x\r\n", stacked_r3);
	printf ("R12 = %x\r\n", stacked_r12);
	printf ("LR [R14] = %x  subroutine call return address\r\n", stacked_lr);
	printf ("PC [R15] = %x  program counter\r\n", stacked_pc);
	printf ("PSR = %x\r\n", stacked_psr);
	printf ("BFAR = %x\r\n", (*((volatile unsigned long *)(0xE000ED38))));
	printf ("CFSR = %x\r\n", (*((volatile unsigned long *)(0xE000ED28))));
	printf ("HFSR = %x\r\n", (*((volatile unsigned long *)(0xE000ED2C))));
	printf ("DFSR = %x\r\n", (*((volatile unsigned long *)(0xE000ED30))));
	printf ("AFSR = %x\r\n", (*((volatile unsigned long *)(0xE000ED3C))));
	printf ("SCB_SHCSR = %x\r\n", SCB->SHCSR);

	while (1);
}

void NMI_Handler() {while(1);}
void MemManage_Handler() {while(1);}
void BusFault_Handler() {while(1);}
void UsageFault_Handler() {while(1);}
