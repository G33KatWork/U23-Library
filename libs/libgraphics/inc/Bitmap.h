#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <Pixels.h>

#include <stdlib.h>
#include <stdint.h>

typedef struct Bitmap
{
	int16_t width,height;
	int32_t bytesperrow;
	Pixel *pixels;
} Bitmap;

#define BytesPerRowForWidth(width) ( ((width)*PixelSize+3)&~3 )
#define NumberOfPixelsForWidthAndHeight(width,height) ( BytesPerRowForWidth((width))*(height)/PixelSize )
#define StartIntializingBitmapRow(row,w) [(row)*BytesPerRowForWidth(w)/PixelSize]=

void InitializeBitmap(Bitmap *self,int width,int height,int bytesperrow,Pixel *pixels);
void InitializeSubBitmap(Bitmap *self,Bitmap *parent,int x,int y,int width,int height);

void ClearBitmap(Bitmap *self);
void FillBitmap(Bitmap *self,uint32_t c);

//void ScrollBitmap(Bitmap *self,int dx,int dy);
//void ScrollRepeatingBitmap(Bitmap *self,int dx,int dy);

static inline size_t SizeOfBitmapDataWithWidthAndHeight(int width,int height)
{
	return BytesPerRowForWidth(width)*height;
}

static inline Pixel *BitmapRowPointer(Bitmap *self,int y)
{
	Pixel *row=(Pixel *)((char *)self->pixels+self->bytesperrow*y);
	return row;
}

static inline Pixel *BitmapPixelPointer(Bitmap *self,int x,int y)
{
	return &BitmapRowPointer(self,y)[x];
}

static inline const Pixel *ConstBitmapRowPointer(const Bitmap *self,int y)
{
	return BitmapRowPointer((Bitmap *)self,y);
}

static inline const Pixel *ConstBitmapPixelPointer(const Bitmap *self,int x,int y)
{
	return &ConstBitmapRowPointer(self,y)[x];
}

#endif

