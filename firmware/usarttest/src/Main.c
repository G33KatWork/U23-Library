#include <stdint.h>
#include <stm32f4xx/stm32f4xx.h>

#include <SystemInit.h>

void InitLEDs(void);
static inline void SetLEDs(int leds);
void InitUSART(void);
void USART_SendString(char* s);
void USART_SendChar(char c);
uint8_t USART_ReceiveChar(char* c);
void Delay(uint32_t time);

int main()
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
	
	InitLEDs();
	InitUSART();

	USART_SendString("Hello!\r\n");

	char c;
	while(1)
	{
		if(USART_ReceiveChar(&c))
			USART_SendChar(c);

		SetLEDs(c);
	}
}

void InitUSART()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;

	//Enable GPIOD clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	//Enable USART2 clock
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  	//Enable Alternate functions
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

	//FIXME: check pin settings, overcurrent led lights up on transmission!

	//PD5 = TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//PD6 = RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//Configure USART2 clocks
	USART_ClockStructInit(&USART_ClockInitStructure);
	USART_ClockInit(USART2, &USART_ClockInitStructure);

	// 9600 8n1, no flowcontrol
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);
}

void USART_SendString(char* s)
{
	while (*s)
	{
		USART_SendChar(*s);
		s++;
	}
}

void USART_SendChar(char c)
{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	USART_SendData(USART2, (uint16_t)c);
}

uint8_t USART_ReceiveChar(char* c)
{
	if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)
	{
		*c = (char)USART_ReceiveData(USART2);
		return 1;
	}
	else
		return 0;
}

void InitLEDs()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  
	/* Enable the GPIO_LED Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/* Configure the GPIO_LED pins */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

static inline void SetLEDs(int leds)
{
	uint16_t val=GPIOD->ODR;
	val&=~(0x0f<<12);
	val|=leds<<12;
	GPIOD->ODR=val;
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
