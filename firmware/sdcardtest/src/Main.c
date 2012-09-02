#include <stdint.h>
#include <stdio.h>

#include <stm32f4xx/stm32f4xx.h>

#include <SystemInit.h>
#include <USART.h>
#include <LED.h>
//#include <SDCARD.h>

#include <sdcard/sdcard.h>

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

	printf("Initilizing SD Card\r\n");
	SD_Error err = SD_Init();
	
	if(err != SD_OK)
	{
		printf("ERR: %X\r\n", err);
		return 0;
	}

	printf("Switching to 1b mode\r\n");
	err = SD_EnableWideBusOperation(SDIO_BusWide_1b);

	if(err != SD_OK)
	{
		printf("ERR: %X\r\n", err);
		return 0;
	}

	printf("Getting SD Card status\r\n");
	SD_CardStatus sd_status;
	err = SD_GetCardStatus(&sd_status);

	if(err != SD_OK)
	{
		printf("ERR: %X\r\n", err);
		return 0;
	}

	printf("SD Card Status:\r\n"
		   "\tDAT_BUS_WIDTH: %X\r\n"
		   "\tSECURED_MODE: %X\r\n"
		   "\tSD_CARD_TYPE: %X\r\n"
		   "\tSIZE_OF_PROTECTED_AREA: %X\r\n"
		   "\tSPEED_CLASS: %X\r\n"
		   "\tPERFORMANCE_MOVE: %X\r\n"
		   "\tAU_SIZE: %X\r\n"
		   "\tERASE_SIZE: %X\r\n"
		   "\tERASE_TIMEOUT: %X\r\n"
		   "\tERASE_OFFSET: %X\r\n",
		   sd_status.DAT_BUS_WIDTH,
		   sd_status.SECURED_MODE,
		   sd_status.SD_CARD_TYPE,
		   sd_status.SIZE_OF_PROTECTED_AREA,
		   sd_status.SPEED_CLASS,
		   sd_status.PERFORMANCE_MOVE,
		   sd_status.AU_SIZE,
		   sd_status.ERASE_SIZE,
		   sd_status.ERASE_TIMEOUT,
		   sd_status.ERASE_OFFSET
	);

	// InitializeSDCard();
	// SD_LowLevel_Init();
	// SD_Init();

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
