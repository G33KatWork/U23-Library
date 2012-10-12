#include <platform/VGA.h>

#include <stdlib.h>

static void InitializeState(uint8_t *framebuffer,int pixelsperrow);
static void HSYNCHandler240();
static void HSYNCHandler200();
static void HSYNCHandler175();

static void InitializePixelDMA(int pixelclock);
static void DMACompleteHandler();
static inline void StartPixelDMA();
static inline void StopPixelDMA();

static HBlankInterruptFunction *HBlankInterruptHandler;
static VGAHSYNCInterruptHandler *HSyncHandler;

void InitializeVGAScreenMode240(uint8_t *framebuffer,int pixelsperrow,int pixelclock)
{
	HBlankInterruptHandler=NULL;
	InitializeVGAPort();
	InitializeState(framebuffer,pixelsperrow);
	InitializePixelDMA(pixelclock);
	InitializeVGAHorizontalSync31kHz(HSYNCHandler240);
}

void InitializeVGAScreenMode200(uint8_t *framebuffer,int pixelsperrow,int pixelclock)
{
	HBlankInterruptHandler=NULL;
	InitializeVGAPort();
	InitializeState(framebuffer,pixelsperrow);
	InitializePixelDMA(pixelclock);
	InitializeVGAHorizontalSync31kHz(HSYNCHandler200);
}

void InitializeVGAScreenMode175(uint8_t *framebuffer,int pixelsperrow,int pixelclock)
{
	HBlankInterruptHandler=NULL;
	InitializeVGAPort();
	InitializeState(framebuffer,pixelsperrow);
	InitializePixelDMA(pixelclock);
	InitializeVGAHorizontalSync31kHz(HSYNCHandler175);
}

void InitializeVGAPort()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//Enable peripheral clocks
	RCC_AHB1PeriphClockCmd(GPIO_DAC_PORT_CLK | GPIO_HSYNC_PORT_CLK | GPIO_VSYNC_PORT_CLK | RCC_AHB1ENR_DMA2EN, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM8EN | RCC_APB2ENR_TIM9EN | RCC_APB2ENR_SYSCFGEN, ENABLE);


	//Configure DAC Pins
	GPIO_InitStructure.GPIO_Pin = GPIO_DAC_PINS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIO_DAC_PORT, &GPIO_InitStructure);

	//DAC Output = Black
	GPIO_ResetBits(GPIO_DAC_PORT, GPIO_DAC_PINS);


	//Configure HSYNC pin
	GPIO_InitStructure.GPIO_Pin = GPIO_HSYNC_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIO_HSYNC_PORT, &GPIO_InitStructure);

	//Configure VSYNC pin
	GPIO_InitStructure.GPIO_Pin = GPIO_VSYNC_PIN;
	GPIO_Init(GPIO_VSYNC_PORT, &GPIO_InitStructure);

	//SYNC Pins = idle high
	RaiseVGAHSYNCLine();
	RaiseVGAVSYNCLine();
}

void InitializeVGAHorizontalSync31kHz(VGAHSYNCInterruptHandler *handler)
{
	HSyncHandler = handler;

	// Configure timer 9 as the HSYNC timer.
	TIM9->CR1=TIM_CR1_ARPE;
	TIM9->DIER=TIM_DIER_UIE|TIM_DIER_CC1IE|TIM_DIER_CC2IE; // Enable update, compare 1 and 2 interrupts.
	TIM9->CCMR1=0;
	TIM9->CCER=0;
	TIM9->PSC=0; // Prescaler = 1
	TIM9->ARR=5337; // 168 MHz / 31.4686 kHz = 5338.65504
	TIM9->CCR1=633; // 168 MHz * 3.77 microseconds = 633.36 - sync pulse end
	TIM9->CCR2=950; // 168 MHz * (3.77 + 1.89) microseconds = 950.88 - back porch end

	// Enable HSYNC timer interrupt and set highest priority.
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable HSYNC timer.
	TIM9->CR1|=TIM_CR1_CEN;
}

void SetHBlankInterruptHandler(HBlankInterruptFunction *handler)
{
	HBlankInterruptHandler=handler;
}


static uint32_t Line;
static volatile uint32_t Frame;
static uint32_t FrameBufferAddress;
static uint32_t CurrentLineAddress;
static uint32_t PixelsPerRow;

static void InitializeState(uint8_t *framebuffer,int pixelsperrow)
{
	// Initialize state.
	Line=0;
	Frame=0;
	FrameBufferAddress=(uint32_t)framebuffer;
	CurrentLineAddress=FrameBufferAddress;
	PixelsPerRow=pixelsperrow;
}

static void HSYNCHandler240()
{
	switch(VGAHorizontalSyncInterruptType())
	{
		case VGAHorizontalSyncStartInterrupt:
			LowerVGAHSYNCLine();
		break;

		case VGAHorizontalSyncEndInterrupt:
			RaiseVGAHSYNCLine();
		break;

		case VGAVideoStartInterrupt:
			if(Line<480)
			{
				StartPixelDMA();
				if(Line&1) CurrentLineAddress+=PixelsPerRow;
			}
			else if(Line==480)
			{
				Frame++;
				// TODO: VBlank interrupt.
			}
			else if(Line==490)
			{
				LowerVGAVSYNCLine();
			}
			else if(Line==492)
			{
				RaiseVGAVSYNCLine();
			}
			else if(Line==524)
			{
				Line=-1;
				CurrentLineAddress=FrameBufferAddress;
			}
			Line++;
		break;
	}
}

static void HSYNCHandler200()
{
	switch(VGAHorizontalSyncInterruptType())
	{
		case VGAHorizontalSyncStartInterrupt:
			LowerVGAHSYNCLine();
		break;

		case VGAHorizontalSyncEndInterrupt:
			RaiseVGAHSYNCLine();
		break;

		case VGAVideoStartInterrupt:
			if(Line<400)
			{
				StartPixelDMA();
				if(Line&1) CurrentLineAddress+=PixelsPerRow;
			}
			else if(Line==400)
			{
				Frame++;
				// TODO: VBlank interrupt.
			}
			else if(Line==412)
			{
				RaiseVGAVSYNCLine();
			}
			else if(Line==414)
			{
				LowerVGAVSYNCLine();
			}
			else if(Line==448)
			{
				Line=-1;
				CurrentLineAddress=FrameBufferAddress;
			}
			Line++;
		break;
	}
}

static void HSYNCHandler175()
{
	switch(VGAHorizontalSyncInterruptType())
	{
		case VGAHorizontalSyncStartInterrupt:
			RaiseVGAHSYNCLine();
		break;

		case VGAHorizontalSyncEndInterrupt:
			LowerVGAHSYNCLine();
		break;

		case VGAVideoStartInterrupt:
			if(Line<350)
			{
				StartPixelDMA();
				if(Line&1) CurrentLineAddress+=PixelsPerRow;
			}
			else if(Line==350)
			{
				Frame++;
				// TODO: VBlank interrupt.
			}
			else if(Line==387)
			{
				LowerVGAVSYNCLine();
			}
			else if(Line==389)
			{		
				RaiseVGAVSYNCLine();
			}
			else if(Line==448)
			{
				Line=-1;
				CurrentLineAddress=FrameBufferAddress;
			}
			Line++;
		break;
	}
}



static void InitializePixelDMA(int pixelclock)
{
	// Configure timer 8 as the pixel clock.
	TIM8->CR1=TIM_CR1_ARPE;
	TIM8->DIER=TIM_DIER_UDE; // Enable update DMA request.
	TIM8->PSC=0; // Prescaler = 1
	TIM8->ARR=pixelclock-1; // TODO: Should this be -1?

	// DMA2 stream 1 channel 7 is triggered by timer 8.
	// Stop it and configure interrupts.
	DMA2_Stream1->CR&=~DMA_SxCR_EN;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void DMA2_Stream1_IRQHandler()
{
	GPIO_ResetBits(GPIO_DAC_PORT, GPIO_DAC_PINS); // Set signal to black.
	DMA2->LIFCR|=DMA_LIFCR_CTCIF1; // Clear interrupt flag.
	
	StopPixelDMA();

	if(HBlankInterruptHandler) HBlankInterruptHandler();
}

void TIM1_BRK_TIM9_IRQHandler()
{
	HSyncHandler();
}

static inline void StartPixelDMA()
{
	// Visible line. Configure and enable pixel DMA.
	DMA2_Stream1->CR=(7*DMA_SxCR_CHSEL_0)| // Channel 7
	(3*DMA_SxCR_PL_0)| // Priority 3
	(0*DMA_SxCR_PSIZE_0)| // PSIZE = 8 bit
	(0*DMA_SxCR_MSIZE_0)| // MSIZE = 8 bit
	DMA_SxCR_MINC| // Increase memory address
	(1*DMA_SxCR_DIR_0)| // Memory to peripheral
	DMA_SxCR_TCIE; // Transfer complete interrupt
	DMA2_Stream1->NDTR=PixelsPerRow;
	DMA2_Stream1->PAR=GPIO_DAC_DMADESTREG;
	DMA2_Stream1->M0AR=CurrentLineAddress;
	DMA2_Stream1->CR|=DMA_SxCR_EN;

	// Start pixel clock.
	TIM8->CR1|=TIM_CR1_CEN;
}

static inline void StopPixelDMA()
{
	TIM8->CR1&=~TIM_CR1_CEN; // Stop pixel clock.
	DMA2_Stream1->CR=0; // Disable pixel DMA.
}



void WaitVBL()
{
	uint32_t currframe=Frame;
	while(Frame==currframe);
}

void SetFrameBuffer(uint8_t *framebuffer)
{
	FrameBufferAddress=CurrentLineAddress=(uint32_t)framebuffer;
}
