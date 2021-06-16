#include "graphics.h"

#pragma pack(1)
typedef enum
{
// every mode the PS2 supports...
	imxIndex4,
	imxIndex8,
	imxRGBA5551,
	imxRGB888,
	imxRGBA8888,
	imxGrayscale16b,				// 16-bit grayscale bitmap data	
    imxNoFormat,
	imxAsInt		=0xFFFFFFFF
} imxFormat;

typedef enum
{
	imxClut16,
	imxClut256,
	imxBitmap,		// data stored in this chunk depends on imxFormat type
	imxEnd,
	imxUserData,			// data stored here is left to the user application to parse
	imxChnkAsInt	=0xFFFFFFFF
} imxChunkType;


typedef struct
{
	UINT32		header;				// "\0XMI" (bytes: 49 4D 58 00)
	UINT8		md5[16];
	UINT32		xsize, ysize;
	imxFormat	format;
} imxHeader;

typedef struct
{
	imxChunkType	type;
	UINT32			len;
} imxChunk;


int calcimxfilesize(tsurface *p)
{
	int size=sizeof(imxHeader);
	
	switch (p->pfd)
	{		
		case PXFi4:size+=(16<<2)+8;break;
		case PXFi8:size+=(256<<2)+8;break;
	}
	size+=(p->getstride(p->size.x)* p->size.y)+8+8; //8 for header, 8 for end
	return size;
}

imxHeader hdr;
bool getimxmetrics(FILE *f,INT32 &w,INT32 &h,UINT32 &bpp,tpixelformat &suggestpfd)
{	
	fread(&hdr,1,sizeof(imxHeader),f);
	w=hdr.xsize;
	h=hdr.ysize;
	switch (hdr.format)
	{
		case imxIndex4:			bpp=4;      suggestpfd=PXFi8;break;
		case imxIndex8:			bpp=8;      suggestpfd=PXFi8;break;
		case imxRGBA5551:		bpp=16;     suggestpfd=PXFrgb565;break;
		case imxGrayscale16b:	bpp=16;		suggestpfd=PXFgrey16;break;
		case imxRGB888:			bpp=24;		suggestpfd=PXFrgb888;break;
		case imxRGBA8888:		bpp=32;     suggestpfd=PXFrgba8888;break;
		case imxNoFormat:		bpp=8;		suggestpfd=PXFnoformat;break;
		default :
			return false;
	}
	return true;
}


//trgba lcmaprgb[256];
//tcolour lcmap[256];

void convertpalette(int count)
{
	while (count--)
	{
		lcmaprgb[count].swaprgb();
		if (SC.isindexed()) //||remapped
			lcmap[count]=count;
		else
		    lcmap[count]=getcolour(*(UINT32*)&lcmaprgb[count]);		
	}
}

UINT32 gettotalchunksizes(FILE *f)
{
	imxChunk chnk;
	chnk.type=imxChnkAsInt;
	UINT32 fpos=ftell(f);
	UINT32 count=0;
	while (chnk.type!=imxEnd)
	{
		fread(&chnk,8,1,f);
		count+=chnk.len+8;
		fseek(f,chnk.len,SEEK_CUR);
	}
	fseek(f,fpos,SEEK_SET);
	return count;
}

bool readimxdata(FILE *f)
{
	imxChunk chnk;
	chnk.len=0;
	chnk.type=imxClut16;
	bool foundbitmap=false;
	int i=0;
	UINT32 x,y;
	tcolour pdata;
	pdata=0;
	bool haspalette=((hdr.format==imxIndex4)||(hdr.format==imxIndex8));
	//if (!feof(f)) return false;
	while (chnk.type!=imxEnd)
	{
		fread(&chnk,8,1,f);
		if (feof(f)) return true;
		switch (chnk.type)
		{
			case imxClut16:
				fread(lcmaprgb,16<<2,1,f);				
				convertpalette(16);								
				buildimagecolourpalettes(&SC,lcmaprgb);
				break;
			case imxClut256:
				fread(lcmaprgb,256<<2,1,f);
				convertpalette(256);
				buildimagecolourpalettes(&SC,lcmaprgb);
				break;
			case imxBitmap:
				if (!foundbitmap)
				{
				for (y=0;y<hdr.ysize;y++)
					switch (hdr.format)
					{
						case imxIndex4:
							for (x=0;x<(hdr.xsize>>1);x+=2)
							{	
								fread(&pdata,1,1,f);
								t_col=lcmap[pdata & 3];
								putpixel(x,y);
								t_col=lcmap[pdata >>4];
								putpixel(x+1,y);								
							}							
							
							break;
						case imxIndex8:
							for (x=0;x<hdr.xsize;x+=1)
							{	
								fread(&pdata,1,1,f);
								t_col=lcmap[pdata];
								putpixel(x,y);
							}
							break;

						case imxGrayscale16b:
							for (x=0;x<hdr.xsize;x+=1)
							{	
								fread(&pdata,2,1,f);
								//pdata=(pdata >> 8)|(pdata & 0x0000FF00)|((pdata << 8)& 0x00FF0000);
								t_col=pdata;//getcolour(pdata);
								putpixel(x,y);
							}

							break;
						case imxRGBA5551:
							for (x=0;x<hdr.xsize;x+=1)
							{	
								fread(&pdata,2,1,f);
								pdata=((pdata << 17)& 0xFF000000)|((pdata << 9)& 0x00FF0000)|((pdata << 6)& 0x0000FF00)|((pdata << 3)& 0x000000FF);
								pdata=SWAPRGB(pdata);
								//fix brightness
								if (pdata & 0x80000)
									pdata |= 0x30000;
								if (pdata & 0x800)
									pdata |= 0x300;
								if (pdata & 0x8)
									pdata |= 0x3;

								t_col=getcolour(pdata);
								putpixel(x,y);
							}
							break;
						case imxRGB888:
							for (x=0;x<hdr.xsize;x+=1)
							{	
								pdata=0;
								fread(&pdata,3,1,f);
								//pdata=(pdata>> 8) ;
								pdata=SWAPRGB(pdata);
								t_col=getcolour(pdata);
								putpixel(x,y);
							}
							break;
						case imxRGBA8888:
							for (x=0;x<hdr.xsize;x+=1)
							{	
								fread(&pdata,4,1,f);
								//different byte ordering so swap it
								pdata=SWAPRGB(pdata);
								t_col=getcolour(pdata);
								putpixel(x,y);
							}
							break;						
					}					
				}else
					fseek(f,chnk.len,SEEK_CUR);
				foundbitmap=true;
				break;
			default:
				fseek(f,chnk.len,SEEK_CUR);
		}
	}
	return true;
	
}

tsurface* readimx(trendercontext *ctx,FILE *f,tpixelformat pfd,UINT32 attrib)
{
	tsurface oldsc;
    tpoint wsize;
    tsurface *bm;
    UINT32 bpp;
	tpixelformat suggestpfd;
    if (!getimxmetrics(f,wsize.x,wsize.y,bpp,suggestpfd))
		return NULL;
	if (suggestpfd==PXFnoformat)
	{
		UINT32 size=gettotalchunksizes(f);
		bm=ctx->createsurface(size,1,PXFnoformat,NULL,0);		
		if (!bm)
			return false;
		fread(bm->scrptr,size,1,f);		
		return bm;
	}
	if (pfd==PXFbestguess)
		pfd=suggestpfd;
    bm=ctx->createsurface(wsize.x,wsize.y,pfd,NULL,attrib);
	if ((attrib & SFremaped)==0)
		if (bm->isindexed())
			bm->setpalette(ctx->createpalette(bm->getpalettesize(),PFlookup));
			
    oldsc=SC;          
    ctx->setoutput(bm);    
    if (!readimxdata(f)) return NULL;    	
    ctx->setoutput(&oldsc);	   
    return bm;
}


bool writeimx(FILE *f,tsurface *bm)
{
	imxChunk chnk;	
	hdr.header=0x00584D49;
	//bung that random data back there!
	if (bm->pfd==PXFnoformat)
	{
		hdr.xsize=0;
		hdr.ysize=0;
		hdr.format=imxNoFormat;
		fwrite(&hdr,sizeof(hdr),1,f);
		fwrite(bm->scrptr,bm->offsd,1,f);
		return true;
	}
	hdr.xsize=bm->size.x;
	hdr.ysize=bm->size.y;
	switch (bm->pfd)
	{		
		case PXFi4:			hdr.format=imxIndex4;break;
		case PXFi8:			hdr.format=imxIndex8;break;
		case PXFrgb565:		hdr.format=imxRGBA5551;break;
		case PXFrgba5551:	hdr.format=imxRGBA5551;break;
		case PXFrgb888:		hdr.format=imxRGB888;break;
		case PXFrgba8888:	hdr.format=imxRGBA8888;break;
		case PXFgrey16:		hdr.format=imxGrayscale16b;break;
		
		//case PXFr4g4b4a4:
		default:return false;		//if it's not one of these formats, it isn't an imx.. ,maybe I should convert to the closest format?
	}
	fwrite(&hdr,sizeof(hdr),1,f);

	if (bm->isindexed() && bm->palette)
	{
		
		
		chnk.len=bm->palette->size <<2;
		switch (bm->pfd)
		{
			case PXFi8:chnk.type=imxClut256;break;
			case PXFi4:chnk.type=imxClut16;break;
		}
		fwrite(&chnk,8,1,f);
		trgba wcol,*rcol=bm->palette->rgbadata;
		for (UINT32 i=0;i<bm->palette->size;i++)
		{
			wcol=*rcol;
			wcol.swaprgb();
			fwrite(&wcol,4,1,f);	
			rcol++;
		}
	}

	chnk.type=imxBitmap;
	int bmstride=bm->getstride(hdr.xsize);
	chnk.len=bmstride*hdr.ysize;
	fwrite(&chnk,8,1,f);
	UINT32 pdata;
	UINT32 x;
	for (UINT32 y=0;y<hdr.ysize;y++)
	{
		char *idata=bm->getpointerat(0,y);
		switch (bm->pfd)
		{
			case PXFi8:				
			case PXFi4:								
			case PXFgrey16:								
				fwrite(idata,bmstride,1,f);
				idata+=bmstride;
			break;
			case PXFrgb565:
			for (x=0;x<hdr.xsize;x++)
			{				
				pdata=*(UINT16*)idata;				
				pdata=((pdata >> 11) & 0x1F)|((pdata << 10) & 0x7C00)|((pdata >> 1)& 0x3E0);
				fwrite(&pdata,2,1,f);
				idata+=2;
			}
			break;
			case PXFrgba5551:
			for (x=0;x<hdr.xsize;x++)
			{				
				//{0x7C00,0x3E0,0x1F
				pdata=*(UINT16*)idata;								
				pdata=((pdata >> 10) & 0x1F)|((pdata << 10) & 0x7C00)|(pdata & 0x3E0);
				fwrite(&pdata,2,1,f);
				idata+=2;
			}
			break;
			case PXFrgb888:
			for (x=0;x<hdr.xsize;x+=1)
			{	
				pdata=*(UINT32*)idata;								
				pdata=SWAPRGB(pdata);
				fwrite(&pdata,3,1,f);				
				idata+=3;
			}
			break;
			case PXFrgba8888:
			for (x=0;x<hdr.xsize;x++)
			{	
				pdata=*(UINT32*)idata;				
				pdata=SWAPRGB(pdata);
				fwrite(&pdata,4,1,f);			
				idata+=4;
			}
			
			break;						
		}		
	}
	chnk.type=imxEnd;
	chnk.len=0;
	fwrite(&chnk,8,1,f);
	return true;
}