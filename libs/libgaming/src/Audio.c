#include <stm32f4xx/stm32f4xx.h>
#include <game/Audio.h>

#include <stdlib.h>

static void WriteRegister(uint8_t RegisterAddr, uint8_t RegisterValue);

static _Bool audio_initialized = 0;

static AudioCallbackFunction *CallbackFunction;
static void *CallbackContext;
static int16_t * volatile NextBufferSamples;
static volatile int NextBufferLength;
static volatile int BufferNumber;
static volatile _Bool DMARunning;

void InitializeAudio(void)
{
	// Intitialize state.
	CallbackFunction = NULL;
	CallbackContext = NULL;
	NextBufferSamples = NULL;
	NextBufferLength = 0;
	BufferNumber = 0;
	DMARunning = 0;


	// Turn on peripherals.
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
			RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD |
			RCC_AHB1Periph_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1 | RCC_APB1Periph_SPI3,
			ENABLE);

	// Configure reset pin.
	GPIO_Init(GPIOD, &(GPIO_InitTypeDef){
		.GPIO_Pin = GPIO_Pin_4,
		.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL,
		.GPIO_Speed = GPIO_Speed_50MHz
	});

	// Configure I2C SCL and SDA pins.
	GPIO_Init(GPIOB, &(GPIO_InitTypeDef){
		.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_OType = GPIO_OType_OD,
		.GPIO_PuPd = GPIO_PuPd_NOPULL,
		.GPIO_Speed = GPIO_Speed_50MHz
	});
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);

	// Configure I2S MCK, SCK, SD pins.
	GPIO_Init(GPIOC, &(GPIO_InitTypeDef){
		.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_12,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL,
		.GPIO_Speed = GPIO_Speed_50MHz
	});
	GPIO_PinAFConfig(GPIOC, GPIO_Pin_7, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_Pin_10, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_Pin_12, GPIO_AF_SPI3);

	// Configure I2S WS pin.
	GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
		.GPIO_Pin = GPIO_Pin_4,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_NOPULL,
		.GPIO_Speed = GPIO_Speed_50MHz
	});
	GPIO_PinAFConfig(GPIOA, GPIO_Pin_4, GPIO_AF_SPI3);

	// Reset the codec.
	GPIO_ResetBits(GPIOD, GPIO_Pin_4);
	for(volatile int i = 0; i < 0x4fff; i++) __asm__ volatile("nop");
	GPIO_SetBits(GPIOD, GPIO_Pin_4);

	// Reset I2C.
	I2C_DeInit(I2C1);

	// Configure I2C.
	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &(I2C_InitTypeDef){
		.I2C_ClockSpeed = 100000,
		.I2C_Mode = I2C_Mode_I2C,
		.I2C_DutyCycle = I2C_DutyCycle_2,
		.I2C_OwnAddress1 = 0x33,
		.I2C_Ack = I2C_Ack_Enable,
		.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit
	});

	// Configure codec.
	WriteRegister(0x02, 0x01); // Keep codec powered off.
	WriteRegister(0x04, 0xaf); // SPK always off and HP always on.

	WriteRegister(0x05, 0x81); // Clock configuration: Auto detection.
	WriteRegister(0x06, 0x04); // Set slave mode and Philips audio standard.

	SetAudioVolume(0xff);

	// Power on the codec.
	WriteRegister(0x02, 0x9e);

	// Configure codec for fast shutdown.
	WriteRegister(0x0a, 0x00); // Disable the analog soft ramp.
	WriteRegister(0x0e, 0x04); // Disable the digital soft ramp.

	WriteRegister(0x27, 0x00); // Disable the limiter attack level.
	WriteRegister(0x1f, 0x0f); // Adjust bass and treble levels.

	WriteRegister(0x1a, 0x0a); // Adjust PCM volume level.
	WriteRegister(0x1b, 0x0a);

	// Reset I2S.
	//SPI_I2S_DeInit(SPI3);

	I2S_Init(SPI3, &(I2S_InitTypeDef){
		.I2S_Mode = I2S_Mode_MasterTx,
		.I2S_Standard = I2S_Standard_Phillips,
		.I2S_DataFormat = I2S_DataFormat_16b,
		.I2S_MCLKOutput = I2S_MCLKOutput_Enable,
		.I2S_AudioFreq = I2S_AudioFreq_22k,
		.I2S_CPOL = I2S_CPOL_Low
	});
	I2S_Cmd(SPI3, ENABLE);

	audio_initialized = 1;
}

void DeinitializeAudio(void)
{
	audio_initialized = 0;

}

_Bool IsAudioInitialized(void)
{
	return audio_initialized;
}

void OutputAudioSample(int16_t sample)
{
	if (!audio_initialized) return;
	while(!SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI3, sample);
}

static void WriteRegister(uint8_t RegisterAddr, uint8_t RegisterValue)
{
	/* While the bus is busy */
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

	/* Start the config sequence */
	I2C_GenerateSTART(I2C1, ENABLE);

	/* Test on EV5 and clear it */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	/* Transmit the slave address and enable writing operation */
	I2C_Send7bitAddress(I2C1, 0x94, I2C_Direction_Transmitter);

	/* Test on EV6 and clear it */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	/* Transmit the first address for write operation */
	I2C_SendData(I2C1, RegisterAddr);

	/* Test on EV8 and clear it */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));

	/* Prepare the register value to be sent */
	I2C_SendData(I2C1, RegisterValue);

	/* Wait till all data have been physically transferred on the bus */
	while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF));

	/* End the configuration sequence */
	I2C_GenerateSTOP(I2C1, ENABLE);
}

void SetAudioVolume(int volume)
{
	WriteRegister(0x20, (volume + 0x19) & 0xff);
	WriteRegister(0x21, (volume + 0x19) & 0xff);
}
