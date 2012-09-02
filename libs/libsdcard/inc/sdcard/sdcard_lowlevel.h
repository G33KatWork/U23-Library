#ifndef _SDCARD_LOWLEVEL_H_
#define _SDCARD_LOWLEVEL_H_

#include <stdint.h>

void SD_LowLevel_Init(void);
void SD_LowLevel_DeInit(void);
void SD_LowLevel_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
void SD_LowLevel_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize);

#endif
