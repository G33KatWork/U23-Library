#include <game/Game.h>
#include <stdio.h>

int ChangeState(Gamestate *state) {
	if(!state->Update || !state->Draw) {
		fprintf(stderr, "PANIC: Update and/or Draw function pointer is NULL\r\n");
		return -1;
	}
	if(state->previousState != NULL) {
		fprintf(stderr, "This Gamestate is already in the Stack\r\n");
		return -1;
	}
	state->previousState = TheGame->currentState;
	TheGame->currentState = state;
}

void ExitState(void) {
	Gamestate* state = TheGame->currentState;
	if (state->previousState)
	{
		TheGame->currentState = state->previousState;
		state->previousState = NULL;
	}
}
