#include <stdint.h>

#include <RCC.h>
#include <System.h>
#include <LED.h>
#include <Accelerometer.h>
#include <Audio.h>

static void InitializeAnglephone();
static void AudioCallback(void *context, int buffer);
void Delay(uint32_t time);
static uint32_t sqrti(uint32_t n);

int main()
{
	InitializeSystem();
	SysTick_Config(HCLKFrequency()/100);
	InitializeLEDs();

	InitializeAccelerometer();
	InitializeAnglephone();

	InitializeAudio(Audio22050HzSettings);
	SetAudioVolume(0x60);

	PlayAudioWithCallback(AudioCallback, 0);

	while(1)
	{
		SetLEDs(0x5);
		Delay(100);
		SetLEDs(0xA);
		Delay(100);
	}
}

static int8_t zero[3];
static int32_t x=0;

static void InitializeAnglephone()
{
	SetLEDs(0x0f);

	SetAccelerometerMainConfig(
		LIS302DL_LOWPOWERMODE_ACTIVE|
		LIS302DL_DATARATE_100|
		LIS302DL_XYZ_ENABLE|
		LIS302DL_FULLSCALE_2_3|
		LIS302DL_SELFTEST_NORMAL);

	//Wait one second for data to stabilize
	Delay(100);

	SetAccelerometerFilterConfig(
		LIS302DL_FILTEREDDATASELECTION_BYPASSED|
    	LIS302DL_HIGHPASSFILTER_LEVEL_1|
    	LIS302DL_HIGHPASSFILTERINTERRUPT_1_2);

	SetLEDs(0);

	if (!PingAccelerometer()) for(;;) SetLEDs(0x05);

	//Read zero calibration data
	ReadRawAccelerometerData(zero);
}

#define GET_AUDIO_BUFFER(i) ((int16_t*)(0x2001fa00 + 0x200 * (!!i)))

static void AudioCallback(void *context, int buffer)
{
	int8_t components[3];
	int32_t dx;
	static uint32_t phase = 0;
	uint32_t phasediff;
	int16_t *samples = GET_AUDIO_BUFFER(buffer);

	ReadRawAccelerometerData(components);

	dx = (components[0]-zero[0]) + 0x3f;

	phasediff = dx * 0x9;

	for(int i = 0; i < 128; i++)
	{
		phase += phasediff;
		if (!(phase & 0x8000))
			samples[2*i+0] = samples[2*i+1] = 32767;
		else
			samples[2*i+0] = samples[2*i+1] = -32768;
	}

	ProvideAudioBuffer(samples, 256);
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

static uint32_t sqrti(uint32_t n)
{
	uint32_t s,t;

	#define sqrtBit(k) \
	t = s+(1UL<<(k-1)); t <<= k+1; if (n >= t) { n -= t; s |= 1UL<<k; }

	s=0;
	if(n>=1<<30) { n-=1<<30; s=1<<15; }
	sqrtBit(14); sqrtBit(13); sqrtBit(12); sqrtBit(11); sqrtBit(10);
	sqrtBit(9); sqrtBit(8); sqrtBit(7); sqrtBit(6); sqrtBit(5);
	sqrtBit(4); sqrtBit(3); sqrtBit(2); sqrtBit(1);
	if(n>s<<1) s|=1;

	#undef sqrtBit

	return s;
}
