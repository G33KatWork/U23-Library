#ifndef SPRITEENGINE_H
#define SPRITEENGINE_H

#include <RLEBitmap.h>
#include <list.h>


// Implements sprites, simply Bitmaps with associated coordinates
typedef struct Sprite {
  const RLEBitmap *bitmap;
  int x, y;
} Sprite;


void SpriteEngine_draw(Bitmap *surface, int xo, int yo);
void Sprite_draw(Sprite *s, Bitmap *surface, int xo, int yo);

extern list *spriteList;

#endif
