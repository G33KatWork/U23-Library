#ifndef __BITMAP_LOADER_H__
#define __BITMAP_LOADER_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct UnpackedPixel
{
	uint8_t r,g,b,a;
} UnpackedPixel;

typedef struct UnpackedBitmap
{
	int width,height;
	UnpackedPixel pixels[0];
} UnpackedBitmap;

UnpackedBitmap *AllocAndLoadBitmap(const char *filename);
UnpackedBitmap *AllocSubBitmap(const UnpackedBitmap *bitmap,int x,int y,int w,int h);

bool IsBitmapTransparent(const UnpackedBitmap *self);
bool IsPartialBitmapTransparent(const UnpackedBitmap *self,int x,int y,int w,int h);
bool IsBitmapInvisible(const UnpackedBitmap *self);
bool IsPartialBitmapInvisible(const UnpackedBitmap *self,int x,int y,int w,int h);

static inline UnpackedPixel UnpackedPixelAt(const UnpackedBitmap *bitmap,int x,int y)
{
	return bitmap->pixels[x+y*bitmap->width];
}

static inline bool IsUnpackedPixelTransparentAt(const UnpackedBitmap *bitmap,int x,int y)
{
	return UnpackedPixelAt(bitmap,x,y).a<128;
}

#endif
