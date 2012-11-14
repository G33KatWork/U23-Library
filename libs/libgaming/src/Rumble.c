#include <platform/Rumble.h>

void InitializeRumble()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(GPIO_RUMBLE_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_RUMBLE_PIN;

	GPIO_Init(GPIO_RUMBLE_PORT, &GPIO_InitStructure);
}

void EnableRumble()
{
	GPIO_SetBits(GPIO_RUMBLE_PORT, GPIO_RUMBLE_PIN);
}

void DisableRumble()
{
	GPIO_ResetBits(GPIO_RUMBLE_PORT, GPIO_RUMBLE_PIN);
}
