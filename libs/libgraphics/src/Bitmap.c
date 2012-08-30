#include <Bitmap.h>
#include <Clipping.h>

#include <string.h>

void InitializeBitmap(Bitmap *self,int width,int height,int bytesperrow,Pixel *pixels)
{
	self->width=width;
	self->height=height;
	self->bytesperrow=bytesperrow;
	self->pixels=pixels;
}

void InitializeSubBitmap(Bitmap *self,Bitmap *parent,int x,int y,int width,int height)
{
	if(ClipRectangle(&x,&y,&width,&height,parent->width,parent->height))
	{
		InitializeBitmap(self,width,height,parent->bytesperrow,
		BitmapPixelPointer(parent,x,y));
	}
	else
	{
		InitializeBitmap(self,0,0,0,NULL);
	}
}

void ClearBitmap(Bitmap *self)
{
	memset(self->pixels,0,self->bytesperrow*self->height);
}

void FillBitmap(Bitmap *self,uint32_t c)
{
	if(sizeof(Pixel)==4)
	{
		for(int y=0;y<self->height;y++)
		{
			uint32_t *row=(uint32_t *)BitmapRowPointer(self,y);
			for(int x=0;x<self->width;x++) row[x]=c;
		}
	}
	else if(sizeof(Pixel)==2)
	{
		for(int y=0;y<self->height;y++)
		{
			uint32_t *row=(uint32_t *)BitmapRowPointer(self,y);
			for(int x=0;x<self->width/2;x++) row[x]=(c<<16)|c;

			if(self->width&1)
			{
				uint16_t *row16=(uint16_t *)row;
				row16[self->width-1]=c;
			}
		}
	}
	else if(sizeof(Pixel)==1)
	{
		for(int y=0;y<self->height;y++)
		{
			uint32_t *row=(uint32_t *)BitmapRowPointer(self,y);
			memset(row,c,self->width);
		}
	}
}

