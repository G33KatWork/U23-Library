#ifndef COLLISION_H
#define COLLISION_H

#include <stdbool.h>
#include <Clipping.h>
#include <RLEBitmap.h>

// Check for collisions
//  BB     = BoundingBox
//  Sprite = Pixelwise

static inline bool Collision_BB_BB(int b1X, int b1Y, int b1sizeX, int b1sizeY, int b2X, int b2Y, int b2sizeX, int b2sizeY)
{
  return !IsCompletelyOutsideAbs(b1X, b1Y, b1sizeX, b1sizeY, b2X, b2Y, b2sizeX, b2sizeY);
}

// Coordinates in whole pixels
bool Collision_BB_Sprite(int bX, int bY, int bsizeX, int bsizeY, int spriteX, int spriteY, const RLEBitmap *sprite);
bool Collision_Sprite_Sprite(int s1X, int s1Y, const RLEBitmap *s1, int s2X, int s2Y, const RLEBitmap *s2);

#endif
