#ifndef __VGA_H__
#define __VGA_H__

#include <stm32f4xx/stm32f4xx.h>
#include <stdint.h>

#define GPIO_DAC_PINS		(GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15)
#define GPIO_DAC_PORT		(GPIOE)
#define GPIO_DAC_DMADESTREG	(((uint32_t)&GPIOE->ODR)+1)
#define GPIO_HSYNC_PIN		(GPIO_Pin_6)
#define GPIO_HSYNC_PORT		(GPIOE)
#define GPIO_VSYNC_PIN		(GPIO_Pin_7)
#define GPIO_VSYNC_PORT		(GPIOE)

#define GPIO_DAC_PORT_CLK	RCC_AHB1Periph_GPIOE
#define GPIO_HSYNC_PORT_CLK	RCC_AHB1Periph_GPIOE
#define GPIO_VSYNC_PORT_CLK	RCC_AHB1Periph_GPIOE

typedef void HBlankInterruptFunction(void);
typedef void VGAHSYNCInterruptHandler(void);

void InitializeVGAScreenMode240(uint8_t *framebuffer,int pixelsperrow,int pixelclock);
void InitializeVGAScreenMode200(uint8_t *framebuffer,int pixelsperrow,int pixelclock);
void InitializeVGAScreenMode175(uint8_t *framebuffer,int pixelsperrow,int pixelclock);

void WaitVBL();
void SetFrameBuffer(uint8_t *framebuffer); // Only safe to call in the vertical blank!

static inline void IntializeVGAScreenMode320x240(uint8_t *framebuffer)
{
	InitializeVGAScreenMode240(framebuffer,320,13);
}

static inline void IntializeVGAScreenMode320x200(uint8_t *framebuffer)
{
	InitializeVGAScreenMode200(framebuffer,320,13);
}

static inline void IntializeVGAScreenMode320x175(uint8_t *framebuffer)
{
	InitializeVGAScreenMode175(framebuffer,320,13);
}



// Functions for implementing your own video driving code.

void InitializeVGAPort();
void InitializeVGAHorizontalSync31kHz(VGAHSYNCInterruptHandler *handler);

#define VGAHorizontalSyncStartInterrupt 1 // Overflow interrupt
#define VGAHorizontalSyncEndInterrupt 2 // Output compare 1 interrupt
#define VGAVideoStartInterrupt 4 // Output compare 2 interrupt

static inline uint32_t VGAHorizontalSyncInterruptType()
{
	uint32_t sr=TIM9->SR;
	TIM9->SR=0;
	return sr;
}

static inline void RaiseVGAHSYNCLine() { GPIO_SetBits(GPIO_HSYNC_PORT, GPIO_HSYNC_PIN); }
static inline void LowerVGAHSYNCLine() { GPIO_ResetBits(GPIO_HSYNC_PORT, GPIO_HSYNC_PIN); }
static inline void RaiseVGAVSYNCLine() { GPIO_SetBits(GPIO_VSYNC_PORT, GPIO_VSYNC_PIN); }
static inline void LowerVGAVSYNCLine() { GPIO_ResetBits(GPIO_VSYNC_PORT, GPIO_VSYNC_PIN); }

#endif
