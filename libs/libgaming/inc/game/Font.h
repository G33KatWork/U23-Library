#ifndef __FONT_H__
#define __FONT_H__

#include <Bitmap.h>
#include <RLEBitmap.h>

extern const RLEBitmap * const fontblack16[256];

extern const RLEBitmap * const fontblack8[256];

extern const RLEBitmap * const fontwhite16[256];

extern const RLEBitmap * const fontwhite8[256];

void setFont(const RLEBitmap * const * font);
void DrawText(Bitmap* dest, char *text, int length, int x, int y);

#endif
