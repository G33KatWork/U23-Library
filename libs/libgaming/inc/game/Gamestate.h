#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include <Drawing.h>

typedef struct Gamestate {
	void (*Init)(struct Gamestate* state);
	void (*Update)(uint32_t);
	void (*Draw)(Bitmap* surface);
	struct Gamestate *previousState;
} Gamestate;

#endif