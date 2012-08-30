#ifndef __PIXELS_CONSTRUCTION_NO_ALPHA_H__
#define __PIXELS_CONSTRUCTION_NO_ALPHA_H__

#include "Functions.h"

#define RawRGB(r,g,b) ( ((r)<<PixelRedShift)|((g)<<PixelGreenShift)|((b)<<PixelBlueShift) )

#define UnclampedRGB(r,g,b) RawRGB( \
	(r)>>(8-PixelRedBits), \
	(g)>>(8-PixelGreenBits), \
	(b)>>(8-PixelBlueBits))

#define RGB(r,g,b) UnclampedRGB(Clamp(r,0,0xff),Clamp(g,0,0xff),Clamp(b,0,0xff))

#define RGBA(r,g,b,a) ( ((a)<0x80)?TransparentPixel: \
	UnclampedRGB(Clamp(r,0,0xff),Clamp(g,0,0xff),Clamp(b,0,0xff)) )

#endif
