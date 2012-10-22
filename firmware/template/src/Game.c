#include <game/Game.h>
#include <game/Debug.h>
#include <game/Audio.h>

#include <stdio.h>
#include <string.h>

void Init(struct Gamestate*);
void OnEnter(struct Gamestate* state);
void OnLeave(struct Gamestate* state);
void Update(uint32_t);
void Draw(Bitmap *);

Gamestate InitState = { Init, OnEnter, OnLeave, Update, Draw };
Game* TheGame = &(Game) {&InitState};

void Init(struct Gamestate* state)
{
}

void OnEnter(struct Gamestate* state)
{
}

void OnLeave(struct Gamestate* state)
{
}

void Update(uint32_t a)
{
}

void Draw(Bitmap *b)
{
}
