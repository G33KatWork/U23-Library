#include <Collision.h>

bool Collision_BB_Sprite(int bX, int bY, int bsizeX, int bsizeY, int spriteX, int spriteY, const RLEBitmap *sprite)
{
  if(!Collision_BB_BB(
        bX,
        bY,
        bsizeX,
        bsizeY,
        spriteX,
        spriteY,
        sprite->width,
        sprite->height))
      return false;

  RLEBitmapSpanIterator iterator;
  InitializeRLEBitmapSpanIterator(&iterator, sprite);
  ClipRLEBitmapSpanIteratorRowsByValue(&iterator, bsizeY, bY - spriteY);

  while(NextRLEBitmapSpan(&iterator))
  {
    int x=RLEBitmapSpanStart(&iterator);
    int length=RLEBitmapSpanLength(&iterator);
    if (length != 0 && !IsSpanCompletelyOutsideAbs(x + spriteX, length, bX, bsizeX))
      return true;
  }

  return false;
}

bool Collision_Sprite_Sprite(int s1X, int s1Y, const RLEBitmap *s1, int s2X, int s2Y, const RLEBitmap *s2)
{
  if(!Collision_BB_BB(
        s1X,
        s1Y,
        s1->width,
        s1->height,
        s2X,
        s2Y,
        s2->width,
        s2->height))
      return false;

  RLEBitmapSpanIterator it1;
  InitializeRLEBitmapSpanIterator(&it1, s1);
  ClipRLEBitmapSpanIteratorRowsByValue(&it1, s2->height, s1Y - s2Y);
  RLEBitmapSpanIterator it2;
  InitializeRLEBitmapSpanIterator(&it2, s2);
  ClipRLEBitmapSpanIteratorRowsByValue(&it2, s1->height, s2Y - s1Y);

  while (true)
  {
    if (!NextRLEBitmapSpan(&it1)) return false;
    if (!NextRLEBitmapSpan(&it2)) return false;
    while (RLEBitmapRow(&it1) + s1Y < RLEBitmapRow(&it2) + s2Y) if (!NextRLEBitmapSpan(&it1)) return false;
    while (RLEBitmapRow(&it1) + s1Y > RLEBitmapRow(&it2) + s2Y) if (!NextRLEBitmapSpan(&it2)) return false;
    if (RLEBitmapSpanLength(&it2) != 0 && RLEBitmapSpanLength(&it2) != 0 &&
        !IsSpanCompletelyOutsideAbs(
          RLEBitmapSpanStart(&it1) + s1X,
          RLEBitmapSpanLength(&it1),
          RLEBitmapSpanStart(&it2) + s2X,
          RLEBitmapSpanLength(&it2) ))
      return true;
  }
}
