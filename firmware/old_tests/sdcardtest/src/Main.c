#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <stm32f4xx/stm32f4xx.h>

#include <SystemInit.h>
#include <USART.h>
#include <LED.h>

#include <sdcard/sdcard.h>

void Delay(uint32_t time);
void hexdump(void* start, size_t len);

int main()
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);

	InitializeLEDs();
	MyUSART_Init();

	setvbuf(stdout, 0, _IONBF, 0);
	printf("Hello!\r\n");

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = SD_SDIO_DMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	printf("Initializing SD Card\r\n");
	SD_Error err = SD_Init();

	if(err != SD_OK)
	{
		printf("ERR: %X\r\n", err);
		return 0;
	}


	printf("Getting SD Card info\r\n");
	SD_CardInfo sd_info;
	err = SD_GetCardInfo(&sd_info);

	if(err != SD_OK)
	{
		printf("ERR: %X\r\n", err);
		return 0;
	}
	printf("SD Capacity: %X - SD Blocksize: %X\r\n", sd_info.CardCapacity, sd_info.CardBlockSize);


	/*printf("Select/Deselect SD Card\r\n");
	err = SD_SelectDeselect((uint32_t) (sd_info.RCA << 16));

	if(err != SD_OK)
	{
		printf("ERR: %X\r\n", err);
		return 0;
	}*/

	printf("Switching to 4b mode\r\n");
	err = SD_EnableWideBusOperation(SDIO_BusWide_4b);

	if(err != SD_OK)
	{
		printf("ERR: %X\r\n", err);
		return 0;
	}

	// printf("Reading first 512B\r\n");
	// uint32_t buffer[512/4];
	// //err = SD_ReadMultiBlocks(0, buffer, 512, 1);
	// err = SD_ReadBlock(0, buffer, 512);

	// if(err != SD_OK)
	// {
	// 	printf("ERR: %X\r\n", err);
	// 	return 0;
	// }

	// hexdump(buffer, sizeof(buffer));


	// printf("Erasing first 512B\r\n");
	// err = SD_Erase(0, 512);

	// if(err != SD_OK)
	// {
	// 	printf("ERR: %X\r\n", err);
	// 	return 0;
	// }


	// for(int i = 0; i < 512/4; i++)
	// 	buffer[i]++;

	// printf("Writing first 512B\r\n");
	// //err = SD_WriteMultiBlocks(0, buffer, 512, 1);
	// err = SD_WriteBlock(0, buffer, 512);

	// if(err != SD_OK)
	// {
	// 	printf("ERR: %X\r\n", err);
	// 	return 0;
	// }


	printf("Reading first two sectors\r\n");
	uint8_t buffer2[4*512];
	err = SD_ReadMultiBlocks(buffer2, 0, 512, 4);

	if(err != SD_OK)
	{
		printf("ERR: %X\r\n", err);
		return 0;
	}

	hexdump(buffer2, sizeof(buffer2));

	memset(buffer2, 0xAA, sizeof(buffer2));

	printf("Writing first two blocks\r\n");
	err = SD_WriteMultiBlocks(buffer2, 0, 512, 4);

	if(err != SD_OK)
	{
		printf("ERR: %X\r\n", err);
		return 0;
	}


	printf("Done\r\n");
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

void hexdump(void* start, size_t len)
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

void SDIO_IRQHandler(void)
{
	SD_ProcessIRQSrc();
}

void SD_SDIO_DMA_IRQHANDLER(void)
{
	SD_ProcessDMAIRQ();
}
