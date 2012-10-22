#ifndef __RLE_LOADER_H__
#define __RLE_LOADER_H__

#include "BitmapLoader.h"

#include <stdlib.h>
#include <stdbool.h>

typedef struct UnpackedRLECode
{
	bool islength:1;
	bool isend:1;
	union
	{
		UnpackedPixel pixel;
		struct
		{
			uint16_t empty,filled;
		} lengths;
	} value;
} UnpackedRLECode;

typedef struct UnpackedRLEBitmap
{
	int width,height,numcodes;
	UnpackedRLECode codes[0];
} UnpackedRLEBitmap;

UnpackedRLEBitmap *AllocRLEBitmapFromBitmap(const UnpackedBitmap *bitmap);
UnpackedRLEBitmap *AllocRLEBitmapFromPartialBitmap(const UnpackedBitmap *bitmap,int x0,int y0,int width,int height);

#endif
