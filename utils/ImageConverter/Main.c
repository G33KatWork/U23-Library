#include "BitmapLoader.h"
#include "RLELoader.h"

#include <stdio.h>
#include <strings.h>
#include <ctype.h>

static void ProcessBitmap(UnpackedBitmap *bitmap,const char *name,int numargs,const char **args);
static void ProcessRLEBitmap(UnpackedBitmap *bitmap,const char *name,int numargs,const char **args);
static void ProcessRLEBitmapArray(UnpackedBitmap *bitmap,const char *name,int numargs,const char **args);
static void ProcessAdaptiveBitmap(UnpackedBitmap *bitmap,const char *name,int numargs,const char **args);
static void ProcessAdaptiveBitmapArray(UnpackedBitmap *bitmap,const char *name,int numargs,const char **args);

static void TestRectangle(int x,int y,int width,int height,const UnpackedBitmap *bitmap);

static void PrintPartialBitmapInitializer(UnpackedBitmap *bitmap,int x,int y,int w,int h,const char *name,int n,const char *indent);
static void PrintPartialBitmapPixelArray(UnpackedBitmap *bitmap,int x,int y,int w,int h,const char *name,int n);
static void PrintRLEStructDefinition(UnpackedRLEBitmap *rle);
static void PrintRLEBitmapInitializer(UnpackedRLEBitmap *rle,const char *indent);
static void PrintAdaptiveBitmapPointerFromPartialBitmap(UnpackedBitmap *bitmap,int x,int y,int w,int h,const char *name,int n,const char *indent);

static void RunFilter(UnpackedBitmap *bitmap,const char *filter);

int main(int argc,const char **argv)
{
	if(argc<4)
	{
		fprintf(stderr,"Usage: %s file.png Bitmap name [x y width height] [filter]\n",argv[0]);
		fprintf(stderr,"       %s file.png RLEBitmap name [x y width height] [filter]\n",argv[0]);
		fprintf(stderr,"       %s file.png RLEBitmapArray name framewidth frameheight [x y width height] [filter]\n",argv[0]);
		fprintf(stderr,"       %s file.png AdaptiveBitmap name [x y width height] [filter]\n",argv[0]);
		fprintf(stderr,"       %s file.png AdaptiveBitmapArray name framewidth frameheight [x y width height] [filter]\n",argv[0]);
		exit(1);
	}

	const char *image=argv[1];
	const char *type=argv[2];
	const char *name=argv[3];

	UnpackedBitmap *bitmap=AllocAndLoadBitmap(image);
	if(!bitmap)
	{
		fprintf(stderr,"Failed to open PNG file \"%s\".\n",image);
		exit(1);
	}
//	ColourTransform=atoi(argv[3]);

	if(strcasecmp(type,"Bitmap")==0)
	{
		ProcessBitmap(bitmap,name,argc-4,&argv[4]);
	}
	else if(strcasecmp(type,"RLEBitmap")==0)
	{
		ProcessRLEBitmap(bitmap,name,argc-4,&argv[4]);
	}
	else if(strcasecmp(type,"RLEBitmapArray")==0)
	{
		ProcessRLEBitmapArray(bitmap,name,argc-4,&argv[4]);
	}
	else if(strcasecmp(type,"AdaptiveBitmap")==0)
	{
		ProcessAdaptiveBitmap(bitmap,name,argc-4,&argv[4]);
	}
	else if(strcasecmp(type,"AdaptiveBitmapArray")==0)
	{
		ProcessAdaptiveBitmapArray(bitmap,name,argc-4,&argv[4]);
	}
	else
	{
		fprintf(stderr,"Unknown type \"%s\".\n",type);
		exit(1);
	}
}



static void ProcessBitmap(UnpackedBitmap *bitmap,const char *name,int numargs,const char **args)
{
	int x,y,w,h;
	if(numargs<4)
	{
		x=0;
		y=0;
		w=bitmap->width;
		h=bitmap->height;

		if(numargs>0) RunFilter(bitmap,args[0]);
	}
	else
	{
		x=atoi(args[0]);
		y=atoi(args[1]);
		w=atoi(args[2]);
		h=atoi(args[3]);

		TestRectangle(x,y,w,h,bitmap);

		if(numargs>4) RunFilter(bitmap,args[4]);
	}

	PrintPartialBitmapPixelArray(bitmap,x,y,w,h,name,0);

	printf("const Bitmap %s=",name);
	PrintPartialBitmapInitializer(bitmap,x,y,w,h,name,0,"");
	printf(";\n");
}

static void ProcessRLEBitmap(UnpackedBitmap *bitmap,const char *name,int numargs,const char **args)
{
	UnpackedRLEBitmap *rle;
	if(numargs<4)
	{
		if(numargs>0) RunFilter(bitmap,args[0]);

		rle=AllocRLEBitmapFromBitmap(bitmap);
	}
	else
	{
		int x=atoi(args[0]);
		int y=atoi(args[1]);
		int width=atoi(args[2]);
		int height=atoi(args[3]);

		TestRectangle(x,y,width,height,bitmap);

		if(numargs>4) RunFilter(bitmap,args[4]);

		rle=AllocRLEBitmapFromPartialBitmap(bitmap,x,y,width,height);
	}
	if(!rle) exit(1);

	PrintRLEStructDefinition(rle);
	printf(" %s=\n",name);
	PrintRLEBitmapInitializer(rle,"");
	printf(";\n");

	free(rle);
}

static void ProcessRLEBitmapArray(UnpackedBitmap *bitmap,const char *name,int numargs,const char **args)
{
	int fw=atoi(args[0]);
	int fh=atoi(args[1]);

	if(fw<=0)
	{
		fprintf(stderr,"Invalid frame width.\n");
		exit(1);
	}

	if(fh<=0)
	{
		fprintf(stderr,"Invalid frame height.\n");
		exit(1);
	}

	int x,y,width,height;
	if(numargs<6)
	{
		x=0;
		y=0;
		width=bitmap->width;
		height=bitmap->height;
		if(numargs>2) RunFilter(bitmap,args[2]);
	}
	else
	{
		x=atoi(args[2]);
		y=atoi(args[3]);
		width=atoi(args[4]);
		height=atoi(args[5]);

		TestRectangle(x,y,width,height,bitmap);

		if(numargs>6) RunFilter(bitmap,args[6]);
	}

	int numcols=width/fw;
	int numrows=height/fh;

	printf("const RLEBitmap * const %s[%d]=\n",name,numcols*numrows);
	printf("{\n");

	for(int row=0;row<numrows;row++)
	for(int col=0;col<numcols;col++)
	{
		int fx=x+col*fw;
		int fy=y+row*fh;

		UnpackedRLEBitmap *rle=AllocRLEBitmapFromPartialBitmap(bitmap,fx,fy,fw,fh);
		if(!rle) exit(1);

		printf("\t(const RLEBitmap *)&(");
		PrintRLEStructDefinition(rle);
		printf(") ");
		PrintRLEBitmapInitializer(rle,"\t");
		printf(",\n");

		free(rle);
	}

	printf("};\n");
}




static void ProcessAdaptiveBitmap(UnpackedBitmap *bitmap,const char *name,int numargs,const char **args)
{
	int x,y,w,h;
	if(numargs<4)
	{
		x=0;
		y=0;
		w=bitmap->width;
		h=bitmap->height;

		if(numargs>0) RunFilter(bitmap,args[0]);
	}
	else
	{
		x=atoi(args[0]);
		y=atoi(args[1]);
		w=atoi(args[2]);
		h=atoi(args[3]);

		TestRectangle(x,y,w,h,bitmap);

		if(numargs>4) RunFilter(bitmap,args[4]);
	}

	if(!IsPartialBitmapTransparent(bitmap,x,y,w,h)) PrintPartialBitmapPixelArray(bitmap,x,y,w,h,name,0);

	printf("const AdaptiveBitmap * const %s=",name);
	PrintAdaptiveBitmapPointerFromPartialBitmap(bitmap,x,y,w,h,name,0,"");
	printf(";\n");
}

static void ProcessAdaptiveBitmapArray(UnpackedBitmap *bitmap,const char *name,int numargs,const char **args)
{
	int fw=atoi(args[0]);
	int fh=atoi(args[1]);

	if(fw<=0)
	{
		fprintf(stderr,"Invalid frame width.\n");
		exit(1);
	}

	if(fh<=0)
	{
		fprintf(stderr,"Invalid frame height.\n");
		exit(1);
	}

	int x,y,width,height;
	if(numargs<6)
	{
		x=0;
		y=0;
		width=bitmap->width;
		height=bitmap->height;
		if(numargs>2) RunFilter(bitmap,args[2]);
	}
	else
	{
		x=atoi(args[2]);
		y=atoi(args[3]);
		width=atoi(args[4]);
		height=atoi(args[5]);

		TestRectangle(x,y,width,height,bitmap);

		if(numargs>6) RunFilter(bitmap,args[6]);
	}

	int numcols=width/fw;
	int numrows=height/fh;

	for(int row=0;row<numrows;row++)
	for(int col=0;col<numcols;col++)
	{
		int n=col+row*numcols;
		int fx=x+col*fw;
		int fy=y+row*fh;
		if(!IsPartialBitmapTransparent(bitmap,fx,fy,fw,fh)) PrintPartialBitmapPixelArray(bitmap,fx,fy,fw,fh,name,n);
	}

	printf("const AdaptiveBitmap * const %s[%d]=\n",name,numcols*numrows);
	printf("{\n");

	for(int row=0;row<numrows;row++)
	for(int col=0;col<numcols;col++)
	{
		int n=col+row*numcols;
		int fx=x+col*fw;
		int fy=y+row*fh;
		PrintAdaptiveBitmapPointerFromPartialBitmap(bitmap,fx,fy,fw,fh,name,n,"\t");
		printf(",\n");
	}

	printf("};\n");
}




static void TestRectangle(int x,int y,int width,int height,const UnpackedBitmap *bitmap)
{
	if(x<0||y<0||x+width>bitmap->width||y+height>bitmap->height)
	{
		fprintf(stderr,"The specified dimensions do not fit inside the image.\n");
		exit(1);
	}

	if(width<=0)
	{
		fprintf(stderr,"Invalid width.\n");
		exit(1);
	}

	if(height<=0)
	{
		fprintf(stderr,"Invalid height.\n");
		exit(1);
	}
}




static void PrintPartialBitmapInitializer(UnpackedBitmap *bitmap,int x,int y,int w,int h,const char *name,int n,const char *indent)
{
	printf("{ %d,%d,BytesPerRowForWidth(%d),(Pixel *)%s_%d }",w,h,h,name,n);

	// Thanks to gcc bug #37303, we can't use the below code which rolls
	// the pixel array into the initializer.
/*	bool transparent=IsPartialBitmapTransparent(bitmap,x,y,w,h);

	printf("{\n");

	printf("%s\t%d,%d,BytesPerRowForWidth(%d),\n",indent,w,h,h);
	printf("%s\t(const Pixel[NumberOfPixelsForWidthAndHeight(%d,%d)]) {",indent,w,h);

	for(int dy=0;dy<h;dy++)
	{
		printf("\n");
		printf("%s\t\tStartIntializingBitmapRow(%d,%d)",indent,dy,w);

		for(int dx=0;dx<w;dx++)
		{
			if(dx%4==0) printf("\n%s\t\t",indent);

			UnpackedPixel p=UnpackedPixelAt(bitmap,x+dx,y+dy);
			if(transparent) printf("RGBA(0x%02x,0x%02x,0x%02x,0x%02x),",p.r,p.g,p.b,p.a);
			else printf("RGB(0x%02x,0x%02x,0x%02x),",p.r,p.g,p.b);
		}
	}

	printf("\n");
	printf("%s\t}\n",indent);
	printf("%s}",indent);*/
}

static void PrintPartialBitmapPixelArray(UnpackedBitmap *bitmap,int x,int y,int w,int h,const char *name,int n)
{
	bool transparent=IsPartialBitmapTransparent(bitmap,x,y,w,h);

	printf("static const Pixel %s_%d[NumberOfPixelsForWidthAndHeight(%d,%d)]=\n",name,n,w,h);
	printf("{");

	for(int dy=0;dy<h;dy++)
	{
		printf("\n");
		printf("\tStartIntializingBitmapRow(%d,%d)",dy,w);

		for(int dx=0;dx<w;dx++)
		{
			if(dx%4==0) printf("\n\t");

			UnpackedPixel p=UnpackedPixelAt(bitmap,x+dx,y+dy);
			if(transparent) printf("RGBA(0x%02x,0x%02x,0x%02x,0x%02x),",p.r,p.g,p.b,p.a);
			else printf("RGB(0x%02x,0x%02x,0x%02x),",p.r,p.g,p.b);
		}
	}

	printf("\n");
	printf("};\n");
}

static void PrintRLEStructDefinition(UnpackedRLEBitmap *rle)
{
	int numpixels=0,numlengths=0,numends=0;

	for(int i=0;i<rle->numcodes;i++)
	{
		if(rle->codes[i].islength)
		{
			if(rle->codes[i].isend) numends++;
			else numlengths++;
		}
		else
		{
			numpixels++;
		}
	}

	printf("const struct { RLEBitmap rle; Pixel codes[RLECodeCount(%d,%d,%d)]; }",numpixels,numlengths,numends);
}

static void PrintRLEBitmapInitializer(UnpackedRLEBitmap *rle,const char *indent)
{
	printf("{\n");

	printf("%s\t{ %d,%d },\n",indent,rle->width,rle->height);
	printf("%s\t{",indent);

	for(int i=0;i<rle->numcodes;i++)
	{
		if(i%4==0) printf("\n%s\t\t",indent);

		if(rle->codes[i].islength)
		{
			if(rle->codes[i].isend) printf("RLEEndCode(0x%02x),   ",rle->codes[i].value.lengths.empty);
			else printf("RLECode(0x%02x,0x%02x), ",rle->codes[i].value.lengths.empty,rle->codes[i].value.lengths.filled);
		}
		else
		{
			printf("RGB(0x%02x,0x%02x,0x%02x),",
			rle->codes[i].value.pixel.r,rle->codes[i].value.pixel.g,rle->codes[i].value.pixel.b);
		}
	}

	printf("\n");
	printf("%s\t}\n",indent);
	printf("%s}",indent);
}

static void PrintAdaptiveBitmapPointerFromPartialBitmap(UnpackedBitmap *bitmap,int x,int y,int w,int h,const char *name,int n,const char *indent)
{
	if(IsPartialBitmapInvisible(bitmap,x,y,w,h))
	{
		printf("\tNULL");
	}
	else if(IsPartialBitmapTransparent(bitmap,x,y,w,h))
	{
		UnpackedRLEBitmap *rle=AllocRLEBitmapFromPartialBitmap(bitmap,x,y,w,h);
		if(!rle) exit(1);

		printf("\tStartRLEBitmapToAdaptiveBitmapCast (const RLEBitmap *)&(");
		PrintRLEStructDefinition(rle);
		printf(") ");
		PrintRLEBitmapInitializer(rle,indent);
		printf(" EndRLEBitmapToAdaptiveBitmapCast");

		free(rle);
	}
	else
	{
		printf("\tStartBitmapToAdaptiveBitmapCast &(const Bitmap)");
		PrintPartialBitmapInitializer(bitmap,x,y,w,h,name,n,indent);
		printf(" EndBitmapToAdaptiveBitmapCast");
	}
}





static int PixelChannel(UnpackedPixel p,char channel)
{
	if(channel=='r') return p.r;
	else if(channel=='g') return p.g;
	else if(channel=='b') return p.b;
	else if(channel=='a') return p.a;
	else if(channel=='0') return 0;
	else if(channel=='1') return 0xff;
	else return 0;
}

static void RunFilter(UnpackedBitmap *bitmap,const char *filter)
{
	if(strncasecmp(filter,"SwapChannels=",13)==0)
	{
		if(strlen(filter)<17) return;

		char rchannel=tolower(filter[13]);
		char gchannel=tolower(filter[14]);
		char bchannel=tolower(filter[15]);
		char achannel=tolower(filter[16]);

		for(int i=0;i<bitmap->width*bitmap->height;i++)
		{
			UnpackedPixel p=bitmap->pixels[i];
			bitmap->pixels[i].r=PixelChannel(p,rchannel);
			bitmap->pixels[i].g=PixelChannel(p,gchannel);
			bitmap->pixels[i].b=PixelChannel(p,bchannel);
			bitmap->pixels[i].a=PixelChannel(p,achannel);
		}
	}
	else
	{
		fprintf(stderr,"Unknown filter \"%s\".\n",filter);
		fprintf(stderr,"Supported filters:\n");
		fprintf(stderr,"* SwapChannels=rgba\n");
	}
}
