#include <fatfs/diskio.h>
#include <sdcard/sdcard.h>
#include <stm32f4xx/stm32f4xx.h>
#include <stdio.h>

#define BlockSize            512 /* Block Size in Bytes */

static SD_CardInfo SDCardInfo;

// void hexdump(void* start, size_t len);

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	//printf("Disk init: %x\r\n", drv);
	SD_Error Status;

	if(drv==0)
	{
		//FIXME: DMA stuff - fix later...
		// NVIC_InitTypeDef NVIC_InitStructure;
		// NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
		// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		// NVIC_Init(&NVIC_InitStructure);
		// NVIC_InitStructure.NVIC_IRQChannel = SD_SDIO_DMA_IRQn;
		// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		// NVIC_Init(&NVIC_InitStructure);

		Status = SD_Init();

		if(Status != SD_OK)
			return STA_NODISK;	//assume no disk if initialization failed
		else
		{
			Status = SD_GetCardInfo(&SDCardInfo);

			if(Status != SD_OK)
				return STA_NOINIT;

			Status = SD_EnableWideBusOperation(SDIO_BusWide_4b);

			if(Status != SD_OK)
				return STA_NOINIT;

			return 0;
		}
	}
	else
		return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */
DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	DSTATUS Stat = STA_NOINIT;

	switch (drv)
	{
	case 0:
		if(SD_GetStatus() == 0)
			Stat &= ~STA_NOINIT;

		break;
	}

	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	//printf("Disk read: Drive %x, Sector: %x, Count: %x\r\n", drv, sector, count);
	SD_Error Status;
	//uint32_t timeout = 1000000;

	if (!count) return RES_PARERR;

	if(drv==0)
	{
		//DMA
		// Status = SD_ReadMultiBlocks(buff, (uint32_t)(sector*512), BlockSize, count);
		// Status = SD_WaitReadOperation();

		// while(SD_GetStatus() != SD_TRANSFER_OK)
		// {
		// 	if(timeout-- == 0)
		// 	{
		// 		printf("SDIO Timeout\r\n");
		// 		return RES_ERROR;
		// 	}
		// }

		//Polling
		for(int i = 0; i < count; i++)
		{
			Status = SD_ReadBlock((BYTE*)(((uintptr_t)buff)+BlockSize*i), (uint32_t)(sector*BlockSize), BlockSize);

			while(SD_GetStatus() != SD_TRANSFER_OK);

			if(Status != SD_OK)
				return RES_ERROR;
		}

		if(Status == SD_OK)
			return RES_OK;
		else
			return RES_ERROR;
	}
	else
		return RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	//printf("Disk write: Drive %x, Sector: %x, Count: %x\r\n", drv, sector, count);
	SD_Error Status;
	//uint32_t timeout = 1000000;

	if (!count) return RES_PARERR;

	if(drv==0)
	{
		//DMA
		// Status = SD_WriteMultiBlocks((BYTE*)buff, (uint32_t)(sector*512), BlockSize, count);
		// Status = SD_WaitWriteOperation();

		// while(SD_GetStatus() != SD_TRANSFER_OK)
		// {
		// 	if(timeout-- == 0)
		// 	{
		// 		printf("SDIO Timeout\r\n");
		// 		return RES_ERROR;
		// 	}
		// }

		for(int i = 0; i < count; i++)
		{
			Status = SD_WriteBlock((BYTE*)(((uintptr_t)buff)+BlockSize*i), (uint32_t)(sector*BlockSize), BlockSize);

			while(SD_GetStatus() != SD_TRANSFER_OK);

			if(Status != SD_OK)
				return RES_ERROR;
		}

		if(Status == SD_OK)
			return RES_OK;
		else
			return RES_ERROR;
	}
	else
		return RES_ERROR;
}
#endif /* _READONLY */

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	//printf("Disk ioctl: %x, Disk: %x\r\n", ctrl, drv);
	u32 x, y, z;
	DRESULT res;

	if (drv==0)
	{
		switch(ctrl)
		{
			case CTRL_SYNC:
				res = RES_OK;
				break;

			case GET_BLOCK_SIZE:
				*(WORD*)buff = BlockSize;
				res = RES_OK;
				break;

			case GET_SECTOR_COUNT:
				if (SD_GetCardInfo(&SDCardInfo)==SD_OK)
				{
					*(DWORD*)buff = SDCardInfo.CardCapacity / 512;
					res = RES_OK;
				}
				else
					res = RES_ERROR ;
				break;

			default:
				res = RES_PARERR;
		}

		return res;
	}
	else
		return RES_ERROR;
}


/*-----------------------------------------------------------------------*/
/* User defined to give a current time to fatfs module          */
/* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31)     */
/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2)           */
/*-----------------------------------------------------------------------*/
DWORD get_fattime (void)
{
#if 0
	struct tm t;
	DWORD date;

	t = Time_GetCalendarTime();
	t.tm_year -= 1980;
	t.tm_mon++;
	t.tm_sec /= 2;

	date = 0;
	date = (t.tm_year << 25) | (t.tm_mon<<21) | (t.tm_mday<<16)|\
			(t.tm_hour<<11) | (t.tm_min<<5) | (t.tm_sec);

	return date;
#else
	return 0;
#endif
}

void SDIO_IRQHandler(void)
{
	SD_ProcessIRQSrc();
}

void SD_SDIO_DMA_IRQHANDLER(void)
{
	SD_ProcessDMAIRQ();
}

// void hexdump(void* start, size_t len)
// {
// 	for(unsigned int i = 0; i < (len / 0x10); i++)
// 	{
// 		printf("%08X: ", (uint32_t)start);

// 		char* ptr = (char*)start;
// 		for(int j = 0; j < 0x10; j++)
// 			printf("%02X ", *ptr++);

// 		ptr = (char*)start;
// 		printf("\t");
// 		for(int j = 0; j < 0x10; j++)
// 		{
// 			char c = *ptr++;
// 			if(c < 0x20 || c > 0x7e)
// 				printf(".");
// 			else
// 				printf("%c", c);
// 		}

// 		printf("\r\n");
// 		start += 0x10;
// 	}
// }
