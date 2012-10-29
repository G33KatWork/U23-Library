#include <game/Game.h>
#include <game/Debug.h>
#include <game/Filesystem.h>
#include <game/Font.h>

#include <stdio.h>
#include <string.h>

#include <ethernet/eth_bsp.h>
#include <ethernet/stm32f4x7_eth.h>

#define MAC_ADDR0   0x3c
#define MAC_ADDR1   0x97
#define MAC_ADDR2   0x0e
#define MAC_ADDR3   0x12
#define MAC_ADDR4   0x34
#define MAC_ADDR5   0x56

void Init(struct Gamestate*);
void Update(uint32_t);
void Draw(Bitmap* surface);
void hexdump(void* start, size_t len);

Gamestate InitState = { Init, NULL, NULL, Update, Draw };
Game* TheGame = &(Game) {&InitState};

extern ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB], DMATxDscrTab[ETH_TXBUFNB];
extern uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE];
extern uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE]; 

/* Global pointer for last received frame infos */
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;

void Init(struct Gamestate* state)
{
	EnableDebugOutput(DEBUG_USART);
	printf("Init\r\n");
	
	printf("Attempting Ethernet initialization\r\n");
	ETH_BSP_Config();
	printf("Ethernet initialized\r\n");

	printf("Setting MAC address\r\n");
	uint8_t macaddr[] = {MAC_ADDR0, MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5};
	ETH_MACAddressConfig(ETH_MAC_Address0, macaddr);
	printf("MAC set\r\n");

	/* Initialize Rx Descriptors list: Chain Mode  */
	printf("Initializing DMA RX Desc chain\r\n");
  	ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
  	printf("DMA RX Desc chain initialized\r\n");

  	/* Initialize Tx Descriptors list: Chain Mode  */
  	printf("Initializing DMA TX Desc chain\r\n");
  	ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
  	printf("DMA TX Desc chain initialized\r\n");

  	#ifdef CHECKSUM_BY_HARDWARE
	  /* Enable the TCP, UDP and ICMP checksum insertion for the Tx frames */
	  for(i=0; i<ETH_TXBUFNB; i++)
	  {
	    ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
	  }
	#endif

  	/* Enable Ethernet Rx interrrupt */
  { 
    for(int i=0; i<ETH_RXBUFNB; i++)
    {
      ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
    }
  }

  	printf("Starting ethernet\r\n");
  	ETH_Start();
  	printf("Ethernet started\r\n");
}

void Update(uint32_t delta)
{
	if(ETH_CheckFrameReceived())
	{
		__IO ETH_DMADESCTypeDef *DMARxNextDesc;

		printf("Received ethernet frame!\r\n");

		FrameTypeDef frame = ETH_Get_Received_Frame();
		hexdump((void*)frame.buffer, frame.length);

		/* Release descriptors to DMA */
		  /* Check if frame with multiple DMA buffer segments */
		  if (DMA_RX_FRAME_infos->Seg_Count > 1)
		  {
		    DMARxNextDesc = DMA_RX_FRAME_infos->FS_Rx_Desc;
		  }
		  else
		  {
		    DMARxNextDesc = frame.descriptor;
		  }
		  
		  /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
		  for (int i=0; i<DMA_RX_FRAME_infos->Seg_Count; i++)
		  {  
		    DMARxNextDesc->Status = ETH_DMARxDesc_OWN;
		    DMARxNextDesc = (ETH_DMADESCTypeDef *)(DMARxNextDesc->Buffer2NextDescAddr);
		  }
		  
		  /* Clear Segment_Count */
		  DMA_RX_FRAME_infos->Seg_Count =0;
		  
		  /* When Rx Buffer unavailable flag is set: clear it and resume reception */
		  if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)  
		  {
		    /* Clear RBUS ETHERNET DMA flag */
		    ETH->DMASR = ETH_DMASR_RBUS;
		    /* Resume DMA reception */
		    ETH->DMARPDR = 0;
		  }
	}
}

void Draw(Bitmap* surface)
{
	ClearBitmap(surface);
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
