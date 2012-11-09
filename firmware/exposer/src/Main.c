#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <System.h>
#include <serencode.h>

#include <stm32f4xx/stm32f4xx_gpio.h>
#include <stm32f4xx/misc.h>
#include <stm32f4xx/stm32f4xx_rcc.h>
#include <stm32f4xx/stm32f4xx_spi.h>
#include <stm32f4xx/stm32f4xx_dac.h>
#include <stm32f4xx/stm32f4xx_tim.h>

SPI_InitTypeDef  SPI_InitStructure;

static int IndexReached;
static int Initial;

/* SPIx Communication boards Interface */
#define SPIx                           SPI2
#define SPIx_CLK                       RCC_APB1Periph_SPI2
#define SPIx_CLK_INIT                  RCC_APB1PeriphClockCmd
#define SPIx_IRQn                      SPI2_IRQn
#define SPIx_IRQHANDLER                SPI2_IRQHandler

#define SPIx_SCK_PIN                   GPIO_Pin_13
#define SPIx_SCK_GPIO_PORT             GPIOB
#define SPIx_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define SPIx_SCK_SOURCE                GPIO_PinSource13
#define SPIx_SCK_AF                    GPIO_AF_SPI2

#define SPIx_MISO_PIN                  GPIO_Pin_14
#define SPIx_MISO_GPIO_PORT            GPIOB
#define SPIx_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define SPIx_MISO_SOURCE               GPIO_PinSource14
#define SPIx_MISO_AF                   GPIO_AF_SPI2

#define SPIx_MOSI_PIN                  GPIO_Pin_15
#define SPIx_MOSI_GPIO_PORT            GPIOB
#define SPIx_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define SPIx_MOSI_SOURCE               GPIO_PinSource15
#define SPIx_MOSI_AF                   GPIO_AF_SPI2

/* Controller parameters */
const int32_t K_CONTROL_MIN = 3400;
const int32_t K_CONTROL_MAX = 4000;
const int32_t K_CONTROL_CENTER = 3700; //(K_CONTROL_MAX + K_CONTROL_MIN) / 2;

const int32_t K_INTEGRAL_CLAMP = 3000;
const int32_t K_SET_SPEED = 608;


#define ADDR_FRAMEBUFFER ((uint8_t*)0x20000000)

const size_t K_DMABUFFER_SIZE = 1024;

/**
  * @brief  Configures the SPI Peripheral.
  * @param  None
  * @retval None
  */
static void SPI_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable the SPI clock */
  SPIx_CLK_INIT(SPIx_CLK, ENABLE);
  
  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(SPIx_SCK_GPIO_CLK | SPIx_MISO_GPIO_CLK | SPIx_MOSI_GPIO_CLK, ENABLE);

  /* SPI GPIO Configuration --------------------------------------------------*/
  /* Connect SPI pins to AF5 */  
  GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT, SPIx_SCK_SOURCE, SPIx_SCK_AF);
  GPIO_PinAFConfig(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_SOURCE, SPIx_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = SPIx_SCK_PIN;
  GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  SPIx_MOSI_PIN;
  GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStructure);
 
  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(SPIx);
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  
  /* Configure the Priority Group to 1 bit */                
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Configure the SPI interrupt priority */
  NVIC_InitStructure.NVIC_IRQChannel = SPIx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  //NVIC_Init(&NVIC_InitStructure);
  
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(SPIx, &SPI_InitStructure);
  
  SPI_Cmd(SPIx, ENABLE);
  DMA_Cmd(DMA1_Stream4, ENABLE);
}

void TIM_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  /* GPIOE clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  
  /* TIM1 channel 2 pin (PE.11) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Connect TIM pins to AF2 */
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);
  
  /* Enable the TIM1 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}

void DMA_Config() {
  RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_DMA1, ENABLE);

  DMA_Stream_TypeDef DMA_Stream_InitStruct;

  DMA_InitTypeDef DMA_InitStruct;


  DMA_Stream_TypeDef *SPI_TX_DMA_STREAM = DMA1_Stream4;

  DMA_StructInit(&DMA_InitStruct);
  DMA_DeInit(SPI_TX_DMA_STREAM);
  DMA_InitStruct.DMA_PeripheralBaseAddr = SPI2_BASE + 0x0C;
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStruct.DMA_Channel = DMA_Channel_0;
  DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;  
  DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)ADDR_FRAMEBUFFER;
  DMA_InitStruct.DMA_BufferSize = 2;
  DMA_Init(SPI_TX_DMA_STREAM, &DMA_InitStruct);      
  DMA_Cmd(SPI_TX_DMA_STREAM, ENABLE); 
}

volatile int EnableClosedLoop = 0;

int main()
{
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);		
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

  MyUSART_Init();

  GPIO_InitTypeDef GPIO_InitStructure;
	
  //memset(ADDR_FRAMEBUFFER, 0xAA, K_DMABUFFER_SIZE);

	/* DMA1 clock and GPIOA clock enable (to be used with DAC) */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	
  DAC_DeInit();
	
  /* DAC channel 1 (DAC_OUT1 = PA.4) configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  DAC_InitTypeDef  DAC_InitStructure;
  
  /* DAC channel1 Configuration */
  // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  //  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //  

  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  
  /* Enable DAC Channel1 */
  DAC_Cmd(DAC_Channel_1, ENABLE);
  DAC_Cmd(DAC_Channel_2, ENABLE);
  DAC_SetChannel1Data(DAC_Align_12b_R, 3700);
  DAC_SetChannel2Data(DAC_Align_12b_R, 0);

  while(1);

  // while(1) {
  //   Delay(100);
  //   printf("Channel1 %d\r\n", DAC_GetDataOutputValue(DAC_Channel_1));
  //   printf("Channel2 %d\r\n", DAC_GetDataOutputValue(DAC_Channel_2));
  // };

  //TIM_ICInitTypeDef  TIM_ICInitStructure;
  /* TIM1 Configuration */
  //TIM_Config();

  /* TIM1 configuration: Input Capture mode ---------------------
     The external signal is connected to TIM1 CH2 pin (PE.11)  
     The Rising edge is used as active edge,
     The TIM1 CCR2 is used to compute the frequency value 
  ------------------------------------------------------------ */

  /*TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM1, &TIM_ICInitStructure);
  
  TIM_PrescalerConfig(TIM1, 512, TIM_PSCReloadMode_Immediate);
  
  // TIM enable counter
  TIM_Cmd(TIM1, ENABLE);

  //Enable the CC2 Interrupt Request 
  TIM_ITConfig(TIM1, TIM_IT_CC2, ENABLE);*/

    
    /* while(1) {
        for(int i=0; i<4096; i++) {
            DAC_SetChannel1Data(DAC_Align_12b_R, i);
            for(int j=0; j<100; j++);
        }
    } */
	
	/* EnableAHB1PeripheralClock(RCC_AHB1ENR_GPIOBEN);

	
	while(1) {
	    GPIOB->ODR |= 0x8000;
	    Delay(1);
        GPIOB->ODR &= ~0x8000;
        Delay(1);
	} */
	
 //  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	// //SetGPIOInputMode(GPIOB,(1<<0));
	// //SetGPIONoPullResistor(GPIOB,(1<<0));

 //  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
 //  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
 //  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 //  GPIO_Init(GPIOB, &GPIO_InitStructure);

	SPI_Config();
  DMA_Config();

	Delay(700);
	
	EnableClosedLoop = 1;
  IndexReached = 0;

  while(1);
  
  while(1) {
    while(1) {
      while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
      SPI_I2S_SendData(SPIx, 0xFF);
    }

    while(!IndexReached) {
      while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
      SPI_I2S_SendData(SPIx, 0xFF);
    }

    for (int i = 0; i < 500; ++i)
    {
      while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
      SPI_I2S_SendData(SPIx, 0xFF);
    }

    for (int i = 0; i < 100; ++i)
    {
      while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
      SPI_I2S_SendData(SPIx, 0x00);
    }

    for (int i = 0; i < 600; ++i)
    {
      for (int i = 0; i < 2; ++i)
      {
        while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
        SPI_I2S_SendData(SPIx, 0xFF);
      }
      for (int i = 0; i < 2; ++i)
      {
        while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
        SPI_I2S_SendData(SPIx, 0x00);
      }
    }
    IndexReached = 0;

  }
}

volatile int CaptureNumber = 0;
volatile uint16_t IC3ReadValue1, IC3ReadValue2;

volatile int32_t integral = 0;

void TIM1_CC_IRQHandler(void)
{
  int Capture, TIM1Freq;
  


  if(TIM_GetITStatus(TIM1, TIM_IT_CC2) == SET) 
  {
    /* Clear TIM1 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
    if(CaptureNumber == 0)
    {
      /* Get the Input Capture value */
      IC3ReadValue1 = TIM_GetCapture2(TIM1);
      CaptureNumber = 1;
    }
    else if(CaptureNumber == 1)
    {
      /* Get the Input Capture value */
      IC3ReadValue2 = TIM_GetCapture2(TIM1); 
      
      /* Capture computation */
      if (IC3ReadValue2 > IC3ReadValue1)
      {
        Capture = (IC3ReadValue2 - IC3ReadValue1); 
      }
      else if (IC3ReadValue2 < IC3ReadValue1)
      {
        Capture = ((0xFFFF - IC3ReadValue1) + IC3ReadValue2); 
      }
      else
      {
        Capture = 0;
      }
      /* Frequency computation */ 
      // TIM1Freq = (uint32_t) SystemCoreClock / Capture;
      CaptureNumber = 0;
      
      int speed = (uint32_t) 1000000 / Capture;
      
      int32_t control = K_CONTROL_CENTER - (speed - K_SET_SPEED)*4 - integral/8;
      if(control > K_CONTROL_MAX) control = K_CONTROL_MAX;
      if(control < K_CONTROL_MIN) control = K_CONTROL_MIN;
      
      integral += (speed - K_SET_SPEED);
      
      if(integral > K_INTEGRAL_CLAMP) integral = K_INTEGRAL_CLAMP;
      if(integral < -K_INTEGRAL_CLAMP) integral = -K_INTEGRAL_CLAMP;
      
      if(EnableClosedLoop)
        DAC_SetChannel1Data(DAC_Align_12b_R, control);

      se_start_frame(10);
      se_puti16(speed);
      se_puti16(control);
      se_puti16(DAC_GetDataOutputValue(DAC_Channel_1));
      se_puti32(integral);

    }
  }

  IndexReached = 1;
  /* for(int i=15; i>=0; i--) {
    GPIOB->ODR |= (Capture & (1<<i) ? 3 : 1);
    GPIOB->ODR &= ~(3);
  } */
}
