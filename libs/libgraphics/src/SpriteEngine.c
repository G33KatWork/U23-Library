#include<Drawing.h>
#include<SpriteEngine.h>

list *spriteList = &(list) { };


void Sprite_draw(Sprite *s, Bitmap *surface, int xo, int yo)
{
  DrawRLEBitmap(surface, s->bitmap, xo + s->x, yo + s->y);
}

void SpriteEngine_draw(Bitmap *surface, int xo, int yo)
{
  list_el *i = spriteList->head;
  while (i)
  {
    Sprite_draw((Sprite*) i->val, surface, xo, yo);
    i = i->next;
  }
}
