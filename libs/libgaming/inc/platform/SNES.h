#ifndef _SNES_CTRL_H_
#define _SNES_CTRL_H_

#include <stdint.h>

typedef union {
	struct {
		uint16_t Reserved : 4;
		uint16_t R        : 1;
		uint16_t L        : 1;
		uint16_t X        : 1;
		uint16_t A        : 1;
		uint16_t Right    : 1;
		uint16_t Left     : 1;
		uint16_t Down     : 1;
		uint16_t Up       : 1;
		uint16_t Start    : 1;
		uint16_t Select   : 1;
		uint16_t Y        : 1;
		uint16_t B        : 1;
	} buttons;
	uint16_t raw;
} snes_button_state_t;

void InitializeSnesController();
snes_button_state_t GetControllerState();
void HandleSnesTimerIRQ();

#endif
