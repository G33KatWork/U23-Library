#include "RLELoader.h"
#include "BitmapLoader.h"

#include <stdio.h>

static size_t SizeOfRLEBitmapFromPartialBitmap(const UnpackedBitmap *bitmap,int x0,int y0,int width,int height);
static void InitializeRLEBitmapFromPartialBitmap(UnpackedRLEBitmap *self,const UnpackedBitmap *bitmap,int x0,int y0,int width,int height);

UnpackedRLEBitmap *AllocRLEBitmapFromBitmap(const UnpackedBitmap *bitmap)
{
	return AllocRLEBitmapFromPartialBitmap(bitmap,0,0,bitmap->width,bitmap->height);
}

UnpackedRLEBitmap *AllocRLEBitmapFromPartialBitmap(const UnpackedBitmap *bitmap,int x0,int y0,int width,int height)
{
	size_t size=SizeOfRLEBitmapFromPartialBitmap(bitmap,0,0,bitmap->width,bitmap->height);
	UnpackedRLEBitmap *self=malloc(size);
	if(!self)
	{
		fprintf(stderr,"Out of memory.\n");
		exit(1);
	}

	InitializeRLEBitmapFromPartialBitmap(self,bitmap,x0,y0,width,height);

	return self;
}





static size_t SizeOfRLEBitmapFromPartialBitmap(const UnpackedBitmap *bitmap,int x0,int y0,int width,int height)
{
	int numcodes=0;

	for(int y=0;y<height;y++)
	{
		int x=0;
		while(x<width)
		{
			int emptystart=x;
			while(x<width && x-emptystart<0xff)
			{
				if(!IsUnpackedPixelTransparentAt(bitmap,x+x0,y+y0)) break;
				x++;
			}
			//int empty=x-emptystart;

			int filledstart=x;
			while(x<width && x-filledstart<0xff)
			{
				if(IsUnpackedPixelTransparentAt(bitmap,x+x0,y+y0)) break;
				x++;
			}
			int filled=x-filledstart;

			numcodes+=1+filled;
		}
	}

	return sizeof(UnpackedRLEBitmap)+numcodes*sizeof(UnpackedRLECode);
}

static void InitializeRLEBitmapFromPartialBitmap(UnpackedRLEBitmap *self,const UnpackedBitmap *bitmap,int x0,int y0,int width,int height)
{
	self->width=width;
	self->height=height;
	self->numcodes=0;
	UnpackedRLECode *ptr=self->codes;

	for(int y=0;y<height;y++)
	{
		int x=0;
		while(x<width)
		{
			int emptystart=x;
			while(x<width && x-emptystart<0xff)
			{
				if(!IsUnpackedPixelTransparentAt(bitmap,x+x0,y+y0)) break;
				x++;
			}
			int empty=x-emptystart;

			int filledstart=x;
			while(x<width && x-filledstart<0xff)
			{
				if(IsUnpackedPixelTransparentAt(bitmap,x+x0,y+y0)) break;
				x++;
			}
			int filled=x-filledstart;

			ptr->islength=true;
			ptr->isend=x>=width && filled==0;
			ptr->value.lengths.empty=empty;
			ptr->value.lengths.filled=filled;
			ptr++;

			for(int i=0;i<filled;i++)
			{
				UnpackedPixel p=UnpackedPixelAt(bitmap,filledstart+x0+i,y+y0);
				ptr->islength=false;
				ptr->value.pixel=p;
				ptr++;
			}

			self->numcodes+=1+filled;
		}
	}
}

