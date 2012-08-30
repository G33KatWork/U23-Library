#include "BitmapLoader.h"
#include "BinaryLoader.h"
#include "PNG.h"

#include <stdio.h>

static void *PixelFunc(void *destination,uint8_t r,uint8_t g,uint8_t b,uint8_t a,int x,int y);

UnpackedBitmap *AllocAndLoadBitmap(const char *filename)
{
	size_t length;
	void *bytes=AllocAndLoadFile(filename,&length);
	if(!bytes) return NULL;

	PNGLoader loader;
	InitializePNGLoader(&loader,bytes,length);

	if(!LoadPNGHeader(&loader))
	{
		fprintf(stderr,"Not a valid PNG file.\n");
		free(bytes);
		return NULL;
	}

	UnpackedBitmap *self=malloc(sizeof(UnpackedBitmap)+
	loader.width*loader.height*sizeof(UnpackedPixel));
	if(!self) return NULL;

	self->width=loader.width;
	self->height=loader.height;

	if(!LoadPNGImageData(&loader,self->pixels,sizeof(UnpackedPixel)*loader.width,PixelFunc))
	{
		fprintf(stderr,"Failed while loading PNG data.\n");
		free(bytes);
		free(self);
		return NULL;
	}

	free(bytes);

	return self;
}

static void *PixelFunc(void *destination,uint8_t r,uint8_t g,uint8_t b,uint8_t a,int x,int y)
{
	UnpackedPixel *pixel=destination;

	pixel->r=r;
	pixel->g=g;
	pixel->b=b;
	pixel->a=a;
	return pixel+1;
}




UnpackedBitmap *AllocSubBitmap(const UnpackedBitmap *bitmap,int x,int y,int w,int h)
{
	UnpackedBitmap *self=malloc(sizeof(UnpackedBitmap)+w*h*sizeof(UnpackedPixel));
	if(!self) return NULL;

	self->width=w;
	self->height=h;

	UnpackedPixel *ptr=self->pixels;

	for(int dy=0;dy<h;dy++)
	for(int dx=0;dx<w;dx++)
	{
		*ptr++=UnpackedPixelAt(bitmap,x+dx,y+dy);
	}

	return self;
}



bool IsBitmapTransparent(const UnpackedBitmap *self)
{
	return IsPartialBitmapTransparent(self,0,0,self->width,self->height);
}

bool IsPartialBitmapTransparent(const UnpackedBitmap *self,int x,int y,int w,int h)
{
	for(int dy=0;dy<h;dy++)
	for(int dx=0;dx<w;dx++)
	if(IsUnpackedPixelTransparentAt(self,x+dx,y+dy)) return true;

	return false;
}

bool IsBitmapInvisible(const UnpackedBitmap *self)
{
	return IsPartialBitmapInvisible(self,0,0,self->width,self->height);
}

bool IsPartialBitmapInvisible(const UnpackedBitmap *self,int x,int y,int w,int h)
{
	for(int dy=0;dy<h;dy++)
	for(int dx=0;dx<w;dx++)
	if(!IsUnpackedPixelTransparentAt(self,x+dx,y+dy)) return false;

	return true;
}
