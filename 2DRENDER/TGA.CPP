#include "graphics.h"

char footer[]="TRUEVISION-XFILE";

#define tga_nodata       0
#define tga_indexed	     1
#define tga_truecolour   2
#define tga_greyscale    3
#define tga_rlindexed    9
#define tga_rltruecolour 10
#define tga_rlgreyscale  11


typedef void (*tgascanlinefunc)(FILE *f,INT32 y,INT32 width);


  
void read_greyscale_scanline_rl(FILE *f,INT32 y,INT32 width)
{

}


#pragma pack(1)

typedef struct tgaheader_
{
	UINT8  idlength;
	UINT8  cmaptype;
	UINT8  imagetype;
	UINT16 cmapoffset;
	UINT16 cmapsize;
	UINT8  cmapbpp;

	UINT16 xorigin;
	UINT16 yorigin;
	UINT16 width;
	UINT16 height;
	UINT8  bpp;
	UINT8  imgdesc;	
}tgaheader;

#pragma pack()

tgaheader tgahdr;
char imageinfo[256];		

inline trgba getrgbcolour(int data,int datasize)
{
	if (datasize==2)	
		data=((data << 3)& 0xff)|((data << 8)& 0xff00)|((data << 13)& 0xff0000)|((data << 22)& 0xff000000);			
	
	SWAPRGB(data);
	
	return *(trgba*)&data;
}


bool gettgametrics(FILE *f,UINT32 &w,UINT32 &h,UINT32 &bpp,tpixelformat &suggestpfd)
{
	
	fread(&tgahdr,18,1,f);
	if (tgahdr.idlength)
	    fread(imageinfo,tgahdr.idlength,1,f);
	bpp=tgahdr.bpp;
	w=tgahdr.width;
	h=tgahdr.height;
	switch (bpp)
	{
		case 4:      suggestpfd=PXFi8;break;
		case 8:      suggestpfd=PXFi8;break;
		case 16:     suggestpfd=PXFrgb565;break;		
		case 24:
		case 32:     suggestpfd=PXFrgba8888;break;
			
	};

	return true;
}


#define dopixel() \
    data=0;\
    fread(&data,datasize,1,f);\
    if (cmapped) \
		pdata=lcmaprgb[data];\
	else \
	    pdata=getrgbcolour(data,datasize); \
    t_col=getcolour(*(UINT32*)&pdata);



inline void putpix(INT32 x,INT32 y,INT32 attrib,INT32 w,INT32 h)
{
	if (attrib & 4)
		x=w-x;
	if ((attrib & 5)==0)
		y=h-y;
	putpixel(x,y);
}

bool loadtgadata(FILE *f)
{
	INT32 i;	
	UINT32 datasize;
	trgba pdata;
	UINT32 data;
	bool cmapped;
	
	bool compressed=tgahdr.imagetype>=9;
	cmapped=((tgahdr.imagetype==9 || tgahdr.imagetype==1) && (tgahdr.cmaptype));
	
	datasize=tgahdr.bpp;
	UINT32 cmapdatasize=tgahdr.cmapbpp;

	if (datasize==15)
		datasize++;
	if (cmapdatasize==15)
		datasize++;

	datasize>>=3;
	cmapdatasize>>=3;

	if (tgahdr.cmaptype)		
		for (i=0;i<tgahdr.cmapsize;i++)
		{
			fread(&data,cmapdatasize,1,f);
			lcmaprgb[i]=getrgbcolour(data,cmapdatasize);
		}

	buildimagecolourpalettes(&SC,lcmaprgb);

	
	for (int y=0;y<tgahdr.height;y++)
	{
		INT32 x=0;
		while (x<tgahdr.width)
		{
			if (compressed)
			{
				UINT8 ctype;				
				fread(&ctype,1,1,f);
				if (ctype & 128)
				{
					dopixel();
					ctype = 1+(ctype & 127);					
					for (i=0;i<ctype;i++)										
						putpix(x++,y,tgahdr.imgdesc,tgahdr.width,tgahdr.height);
						
					
				}else
				{
					ctype = 1+(ctype & 127);
					for (i=0;i<ctype;i++)
					{															
						dopixel();						
						putpix(x++,y,tgahdr.imgdesc,tgahdr.width,tgahdr.height);
					}
				}


			}else
			{
				dopixel();				
				putpix(x++,y,tgahdr.imgdesc,tgahdr.width,tgahdr.height);
			}
		}
	}
	return true;
}

tsurface* readtga(trendercontext *ctx,FILE *f,tpixelformat pfd,UINT32 attrib)
{	
	tpixelformat suggestpfd;
	UINT32 w,h,bpp;
	gettgametrics(f,w,h,bpp,suggestpfd);

	if (pfd==PXFbestguess)
		pfd=suggestpfd;
	
	tsurface *nsurf=ctx->createsurface(w,h,pfd,NULL,attrib);

	if ((attrib & SFremaped)==0)
		if (nsurf->isindexed())
			nsurf->setpalette(ctx->createpalette(nsurf->getpalettesize(),PFlookup));	
		
	ctx->setoutput(nsurf);
	loadtgadata(f);
	
	ctx->setoutput(NULL);
	return nsurf;
}

void writetga(char *filename,tsurface *bm)
{
}
