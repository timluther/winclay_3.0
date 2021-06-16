#include "graphics.h"


/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L


typedef struct tagbmpfileheader
{    /* bmfh */
    UINT16    bfType;  //Specifies the type of file. This member must be BM. 
    UINT32    bfSize;  //Specifies the size of the file, in bytes.
    UINT16    bfReserved1;
    UINT16    bfReserved2;
    UINT32    bfOffBits; //Specifies the byte offset from the BITMAPFILEHEADER structure
} bmpfileheader;

typedef struct tagbmpinfoheader
{    /* bmih */
    UINT32   biSize;  //number of bytes in this structure
    INT32    biWidth; //Specifies the width of the bitmap, in pixels. 
    INT32    biHeight;
    UINT16   biPlanes;// number of planes, set to 1
    UINT16   biBitCount;//Specifies the number of bits per pixel. This value must be 1, 4, 8, or 24.
    UINT32   biCompression; //BI_RGB BI_RLE8 BI_RLE4
    UINT32   biSizeImage;  //Specifies the size, in bytes, of the image. It is valid to set this member to zero if the bitmap is in the BI_RGB format.
    INT32    biXPelsPerMeter;
    INT32    biYPelsPerMeter;
    INT32    biClrUsed;   //Specifies the number of color indexes in the color table
    UINT32   biClrImportant;
} bmpinfoheader;

typedef struct tagbmpinfo
{  /* bmi */    
    bmpinfoheader    bmiHeader;
    trgba               bmiColors[1];
} bmpinfo;


bmpfileheader fileheader;
bmpinfoheader bh;

trgba *palette;


/*void printbmpheader(INT32 x,INT32 y,BITMAPFILEHEADER &fh,bmpinfoheader &h)
{
    char buf[100];
    tcolour blue=getcolour(0,0,255);
    sprintf(buf,"Type: %i",fh.bfType);
    logmessage((void *)&blue,buf);
    sprintf(buf,"File Size: %i",fh.bfSize);
    logmessage((void *)&blue,buf);
    sprintf(buf,"Reserved bytes: %i %i",fh.bfReserved1,fh.bfReserved2);
    logmessage((void *)&blue,buf);
    sprintf(buf,"Offset to whatever: %i",fh.bfOffBits);
    logmessage((void *)&blue,buf);
    
    sprintf(buf,"Structuresize: %i",h.biSize);
    logmessage((void *)&blue,buf);
    sprintf(buf,"Width: %i",h.biWidth);
    logmessage((void *)&blue,buf);
    sprintf(buf,"Height: %i",h.biHeight);
    logmessage((void *)&blue,buf);
    sprintf(buf,"Number of planes %i",h.biPlanes);
    logmessage((void *)&blue,buf);
    sprintf(buf,"Bit count %i",h.biBitCount);
    logmessage((void *)&blue,buf);
    sprintf(buf,"Compression: %i",h.biCompression);
    logmessage((void *)&blue,buf);
    sprintf(buf,"Image size: %i",h.biSizeImage);
    logmessage((void *)&blue,buf);
    sprintf(buf,"Pels x: %i",h.biXPelsPerMeter);
    logmessage((void *)&blue,buf);
    sprintf(buf,"Pels y: %i",h.biYPelsPerMeter);
    logmessage((void *)&blue,buf);
    sprintf(buf,"colours used %i",h.biClrUsed);
    logmessage((void *)&blue,buf);
    sprintf(buf,"Important (MS?!?!) colours used %i",h.biClrImportant);                
    logmessage((void *)&blue,buf);
}*/

bool getbmpmetrics(FILE *f,INT32 &w,INT32 &h,UINT32 &bpp,tpixelformat &suggestpfd)
{    
    fseek(f,0,SEEK_SET);
    fread(&fileheader,14,1,f);   
    fread(&bh,sizeof(bmpinfoheader),1,f);
    if (fileheader.bfType!=0x4D42)
      return FALSE;

    w=bh.biWidth;
    h=bh.biHeight;
    bpp=bh.biBitCount;    
	switch (bpp)
	{
		case 4:      suggestpfd=PXFi8;break;
		case 8:      suggestpfd=PXFi8;break;
		case 16:     suggestpfd=PXFrgb565;break;		
		case 24:
		case 32:     suggestpfd=PXFrgba8888;break;
		default:	 return false;			
	};
    return TRUE;
}



bool loadbmpdata(FILE *f)
{    
    UINT8 datain[4];
    UINT8 dummy;
    trgba temprgb;

    INT32 i,xi,padding,yi,xcount;    
    fseek(f,sizeof(bmpinfoheader)+14,SEEK_SET);    
    padding=sizeof(bmpinfoheader)-bh.biSize;
    //printbmpheader(0,0,fileheader,bh);
    
    if (padding>0)
    for (i=0;i<padding;i++)
       fread(&dummy,1,1,f);
          
    if (bh.biClrUsed>0)
    {       
       fread(lcmaprgb,sizeof(RGBQUAD)*bh.biClrUsed,1,f);
	   if (SC.isindexed())	   	   
		   for (i=0;i<bh.biClrUsed;i++)
               lcmap[i]=i;       
	   else
		   for (i=0;i<bh.biClrUsed;i++)
			   lcmap[i]=getcolour(lcmaprgb[i].getdword());                      
	   buildimagecolourpalettes(&SC,lcmaprgb);
    }       

    padding=bh.biWidth-((bh.biWidth >> 2)<<2);    
    switch (bh.biBitCount)
    {
        case 1:break;
        case 4:
        for (yi=0;yi<bh.biHeight;yi++)
        {
            xcount=0;
            for (xi=0;xi<bh.biWidth >> 1;xi++)
            {
                fread(datain,1,1,f);
                t_col=lcmap[datain[0] & 15];
                putpixel(xcount++,yi);
                t_col=lcmap[(datain[0]>>4) & 15];
                putpixel(xcount++,bh.biHeight-yi-1);                                    
            }
            if (padding)
               fread(datain,4-padding,1,f);
        }
        break;
        case 8:
        for (yi=0;yi<bh.biHeight;yi++)
        {
            for (xi=0;xi<bh.biWidth;xi++)
            {
                fread(datain,1,1,f);
                t_col=lcmap[datain[0]];
                putpixel(xi,bh.biHeight-yi-1);                                    
            }
            if (padding)
               fread(datain,4-padding,1,f);
               //padding
        }
        break;
        case 24:
        for (yi=0;yi<bh.biHeight;yi++)
        {
            for (xi=0;xi<bh.biWidth;xi++)
            {
                fread(datain,3,1,f);
                t_col=getcolour(*(INT32*)datain);;//BUILDARGB(0,datain[2],datain[1],datain[0])
                putpixel(xi,bh.biHeight-yi-1);                                    
            }
            if (padding)
               fread(datain,padding,1,f);
        }
        break;        
    }    
    return TRUE;
}


/*
FILE *f;    
    
    f=fopen(filename,"rb");
    if (!f) return FALSE;    
    */

tsurface* readbmp(trendercontext *ctx,FILE *f,tpixelformat pfd,UINT32 attrib)
{
    tpoint size;
    UINT32 bpp;
    tsurface *bm;
    tsurface oldsc;
    oldsc=SC;
	tpixelformat suggestpfd;

    if (!getbmpmetrics(f,size.x,size.y,bpp,suggestpfd))return NULL;
	if (pfd==PXFbestguess)
		pfd=suggestpfd;
	
	
    bm=ctx->createsurface(size.x,size.y,pfd,NULL,attrib);

	if ((attrib & SFremaped)==0)
		if (bm->isindexed())
			bm->setpalette(ctx->createpalette(bm->getpalettesize(),PFlookup));	
    
    ctx->setoutput(bm);   

    loadbmpdata(f);
    ctx->setoutput(&oldsc);
  
    return bm;        
}    
