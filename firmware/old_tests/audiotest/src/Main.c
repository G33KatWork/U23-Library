#include <stdint.h>

#include <RCC.h>
#include <System.h>
#include <LED.h>
#include <Audio.h>

#include <BitBin.h>

void Delay(uint32_t time);

static void AudioCallback(void *context,int buffer);
int16_t *buffers[2]={ (int16_t *)0x2001fa00,(int16_t *)0x2001fc00 };
extern BitBinNote *channels[8];

int main()
{
	InitializeSystem();
	SysTick_Config(HCLKFrequency()/100);
	InitializeLEDs();

	BitBinSong song;
	InitializeBitBinSong(&song,BitBin22kTable,8,channels);

	InitializeAudio(Audio22050HzSettings);
	PlayAudioWithCallback(AudioCallback,&song);

	while(1)
	{
		SetLEDs(0x5);
		Delay(100);
		SetLEDs(0xA);
		Delay(100);
	}
}

static void AudioCallback(void *context,int buffer)
{
	BitBinSong *song=context;

	int16_t *samples=buffers[buffer];
	RenderBitBinSamples(song,128,samples);
	for(int i=128;i>=0;i--)
	{
		samples[2*i+0]=samples[i];
		samples[2*i+1]=samples[i];
	}

	ProvideAudioBuffer(samples,256);
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
