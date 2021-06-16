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

inline trgba getrgbcolour(int data,int datasize)
{
	if (datasize==2)	
		data=((data << 3)& 0xff)|((data << 8)& 0xff00)|((data << 13)& 0xff0000)|((data << 22)& 0xff000000);		
	
	return *(trgba*)&data;
}



bool gettgametrics(char *filename,UINT32 &w,UINT32 &h,UINT32 &bpp)
{
	return true;
}

#define dopixel() data=0;fread(&data,datasize,1,f);if (cmapped) pdata=lcmaprgb[data];else pdata=getrgbcolour(data,datasize);t_col=getcolour(pdata.b,pdata.g,pdata.r);



inline void putpix(INT32 x,INT32 y,INT32 attrib,INT32 w,INT32 h)
{
	if (attrib & 4)
		x=w-x;
	if (attrib & 5)
		y=h-y;
	putpixel(x,y);
}

tsurface* readtga(trendercontext *ctx,char *filename,UINT32 pfd,UINT32 attrib)
{
	INT32 i;
	tgaheader hdr;
	FILE *f;
	UINT32 datasize;
	trgba pdata;
	UINT32 data;
	bool cmapped;

	char imageinfo[256];	
	if (!(f=fopen(filename,"rb"))) return NULL;
	fread(&hdr,18,1,f);
	if (hdr.idlength)
	    fread(imageinfo,hdr.idlength,1,f);

	bool compressed=hdr.imagetype>=9;
	cmapped=((hdr.imagetype==9 || hdr.imagetype==1) && (hdr.cmaptype));
	
	datasize=hdr.bpp;
	UINT32 cmapdatasize=hdr.cmapbpp;

	if (datasize==15)
		datasize++;
	if (cmapdatasize==15)
		datasize++;

	datasize>>=3;
	cmapdatasize>>=3;
	
	tsurface *nsurf=ctx->createsurface(hdr.width,hdr.height,pfd,NULL,attrib);
		
	ctx->setoutput(nsurf);
	if (hdr.cmaptype)		
		for (i=0;i<hdr.cmapsize;i++)
		{
			fread(&data,cmapdatasize,1,f);
			lcmaprgb[i]=getrgbcolour(data,cmapdatasize);
		}

	
	for (int y=0;y<hdr.height;y++)
	{
		INT32 x=0;
		while (x<hdr.width)
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
						putpix(x++,y,hdr.imgdesc,hdr.width,hdr.height);
						
					
				}else
				{
					ctype = 1+(ctype & 127);
					for (i=0;i<ctype;i++)
					{															
						dopixel();						
						putpix(x++,y,hdr.imgdesc,hdr.width,hdr.height);
					}
				}


			}else
			{
				dopixel();				
				putpix(x++,y,hdr.imgdesc,hdr.width,hdr.height);
			}
		}
	}
	ctx->setoutput(NULL);
	return nsurf;
}

void writetga(char *filename,tsurface *bm)
{
}
