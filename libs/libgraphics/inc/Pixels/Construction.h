#ifndef __PIXELS_CONSTRUCTION_H__
#define __PIXELS_CONSTRUCTION_H__

#include "Functions.h"

#define RawRGB(r,g,b) ( ((r)<<PixelRedShift)|((g)<<PixelGreenShift)|((b)<<PixelBlueShift)|BitMaskAt(PixelAlphaShift,PixelAlphaBits) )
#define RawRGBA(r,g,b,a) ( ((r)<<PixelRedShift)|((g)<<PixelGreenShift)|((b)<<PixelBlueShift)|((a)<<PixelAlphaShift) )

#define UnclampedRGB(r,g,b) RawRGB( \
	(r)>>(8-PixelRedBits), \
	(g)>>(8-PixelGreenBits), \
	(b)>>(8-PixelBlueBits))

#define UnclampedRGBA(r,g,b,a) RawRGBA( \
	(r)>>(8-PixelRedBits), \
	(g)>>(8-PixelGreenBits), \
	(b)>>(8-PixelBlueBits), \
	(a)>>(8-PixelAlphaBits))

#define RGB(r,g,b) UnclampedRGB(Clamp(r,0,0xff),Clamp(g,0,0xff),Clamp(b,0,0xff))

#define RGBA(r,g,b,a) UnclampedRGBA(Clamp(r,0,0xff),Clamp(g,0,0xff),Clamp(b,0,0xff),Clamp(a,0,0xff))

#endif
