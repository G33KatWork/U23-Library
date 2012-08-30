#include <stdint.h>

#include <RCC.h>
#include <System.h>
#include <LED.h>
#include <Accelerometer.h>

static void InitializeLEDFlow();
static void RunLEDFlow();
void Delay(uint32_t time);
static uint32_t sqrti(uint32_t n);

int main()
{
	InitializeSystem();
	SysTick_Config(HCLKFrequency()/100);
	InitializeLEDs();
	InitializeAccelerometer();

	InitializeLEDFlow();

	while(1)
	{
		RunLEDFlow();
		Delay(5);
	}
}

static int8_t zero[3];
static int32_t x=0,y=0;

static void InitializeLEDFlow()
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

	if(!PingAccelerometer()) for(;;) SetLEDs(0x05);

	//Read zero calibration data
	ReadRawAccelerometerData(zero);
}

static void RunLEDFlow()
{
	int8_t components[3];
	ReadRawAccelerometerData(components);

	int32_t dx=components[0]-zero[0];
	int32_t dy=components[1]-zero[1];
	int32_t r=sqrti(dx*dx+dy*dy);
	dx=(dx<<12)/r;
	dy=(dy<<12)/r;

	x+=r*25;

//	x+=components[0]-zero[0];
//	y+=components[1]-zero[1];

	int leds=0;
	leds|=(((x+dx)>>14)&1)<<1;
	leds|=(((x-dx)>>14)&1)<<3;
	leds|=(((x+dy)>>14)&1)<<0;
	leds|=(((x-dy)>>14)&1)<<2;

	SetLEDs(leds);
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
