#include <stdint.h>
#include <stdio.h>

#include <stm32f4xx/stm32f4xx.h>

#include <SystemInit.h>
#include <USART.h>
#include <LED.h>

#include <sdcard/sdcard.h>

void Delay(uint32_t time);
void hexdump(uint8_t* start, size_t len);

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

	printf("Getting SD Card info\r\n");
	SD_CardInfo sd_info;
	err = SD_GetCardInfo(&sd_info);

	if(err != SD_OK)
	{
		printf("ERR: %X\r\n", err);
		return 0;
	}

	printf("SD Capacity: %X - SD Blocksize: %X\r\n", sd_info.CardCapacity, sd_info.CardBlockSize);

	printf("Reading first 512B\r\n");
	uint8_t buffer[512];
	err = SD_ReadBlock(buffer, 0, 512);

	if(err != SD_OK)
	{
		printf("ERR: %X\r\n", err);
		return 0;
	}

	hexdump(buffer, sizeof(buffer));

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

void hexdump(uint8_t* start, size_t len)
{
	for(unsigned int i = 0; i < (len / 0x10); i++)
    {
    	printf("%08X: ", (uint32_t)start);
        
        char* ptr = (char*)start;
        for(int j = 0; j < 0x10; j++)
        	printf("%02X ", *ptr++);
        
        ptr = (char*)start;
        printf("\t");
        for(int j = 0; j < 0x10; j++)
        {
            char c = *ptr++;
            if(c < 0x20 || c > 0x7e)
                printf(".");
            else
                printf("%c", c);
        }
        
        printf("\r\n");
        start += 0x10;
    }
}
