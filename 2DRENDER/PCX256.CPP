#include "graphics.h"


#define headerchar 10
#define no_bytes_back 769
#define check256val 12
#define chkbits 0xC0
#define maskbits 0x3f


typedef struct pcx_header
{
    UINT8 header;
    UINT8 version;
    UINT8 encoding;
    UINT8 bpp;
    trect16 window;
    tpoint16 resolution;
    trgb cmap[16];
    UINT8 reserved;
    UINT8 numplanes;
    UINT16 offset;
    UINT16 PaletteInfo;
    tpoint16 screensize;
    UINT8 filler[54];        
}pcx_header;

pcx_header header;


void printpcxheader(INT32 x,INT32 y,pcx_header &h)
{
    char buf[100];
    sprintf(buf,"Headerbyte: %i",h.header);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Version: %i",h.version);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Encoding: %i",h.encoding);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"bpp: %i",h.bpp);
    outtextxy(x,y+=18,buf);    
    sprintf(buf,"window: width:%i height:%i",h.window.width(),h.window.height());
    outtextxy(x,y+=18,buf);    
    sprintf(buf,"resolution: x:%i y:%i",h.resolution.x,h.resolution.y);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"planes: %i",h.numplanes);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"offset: %i",h.offset);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"Paletteinfo: %i",h.PaletteInfo);
    outtextxy(x,y+=18,buf);
    sprintf(buf,"screensize: x: %i y: %i",h.screensize.x,h.screensize.y);            
    outtextxy(x,y+=18,buf);
    int i;
    for (i=0;i<16;i++)
    {
        sprintf(buf,"Pal %1: r:%i g:%i b:%i",i,header.cmap[i].r,header.cmap[i].g,header.cmap[i].b);
        outtextxy(x,y+=18,buf);
    }                    
}



UINT8 *loadscanline(UINT8 *scanptr,UINT8 val,UINT8 bitplane,UINT32 count)
{

    bitplane=1 << bitplane;
    
    while (count>0)
    {        
        if (val & 0x80)
            *scanptr|=bitplane;
        scanptr++;
        if (val & 0x40)
            *scanptr|=bitplane;
        scanptr++;            
        if (val & 0x20)
            *scanptr|=bitplane;
        scanptr++;            
        if (val & 0x10)
            *scanptr|=bitplane;
        scanptr++;            
        if (val & 0x8)
            *scanptr|=bitplane;
        scanptr++;            
        if (val & 0x4)
            *scanptr|=bitplane;
        scanptr++;            
        if (val & 0x2)
            *scanptr|=bitplane;
        scanptr++;            
        if (val & 0x1)
            *scanptr|=bitplane;
        scanptr++;            
        count--;
    }
    return scanptr;
    
}

tpoint startpos;


//SEEK_SET, SEEK_END, SEEK_CUR
  /*if (pos.y==0)
                    {
                        t_col=lcmap[255];
                        sprintf(buf,"%i",chk);
                        
                        outtextxy(fontpos,300,buf);
                        fontpos+=strlen(buf)<<3;
                    }*/

bool getpcxmetrics(FILE *f,INT32 &w,INT32 &h,UINT32 &bpp,tpixelformat &suggestpfd)
{
    fread(&header,128,1,f);
    w=header.window.width()+1;
    h=header.window.height()+1;
    bpp=header.bpp*header.numplanes;  
	/*if ((bpp!=1)||(bpp!=4)||(bpp!=8)||(bpp!=24)||(bpp!=32))
		return FALSE;*/
	switch (bpp)
	{
		case 4:      suggestpfd=PXFi8;break;
		case 8:      suggestpfd=PXFi8;break;
		case 16:     suggestpfd=PXFrgb565;break;		
		case 24:
		case 32:     suggestpfd=PXFrgba8888;break;
		default:	return false;
	};
    return TRUE;
}

//loads PCX file into current device context
bool loadpcxdata(FILE *f,char dopalette)
{
    UINT16 i;
    UINT8 *tempscan,*cbyte,planei;    
    UINT8 runlength;
    UINT8 chk;
    INT16 palsize=0;
    UINT32 scanwidth;
    UINT16 subtotal;
    tpoint wsize,pos;
               
    fseek(f,sizeof(header),SEEK_CUR);    
    scanwidth=header.offset*header.numplanes;
    wsize.x=header.window.width()+1;
    wsize.y=header.window.height()+1;
          
    //load up palette
        
        if (header.bpp<=4)
        {
            palsize=16;            
            for (i=0;i<16;i++)
                memcpy(&lcmaprgb[i],&header.cmap[i],3);                
        }else
        {
            fseek(f,-no_bytes_back,SEEK_END);
            chk=fgetc(f);
            if (chk==check256val)
            {       
                palsize=256;          
                for (i=0;i<256;i++)
                {
                    fread(&lcmaprgb[i],sizeof(trgb),1,f);					
                    lcmaprgb[i].a=0;
                }
            }
            fseek(f,128,SEEK_SET);        
        }
    
    //(lcmaprgb[i].r<<16)|(lcmaprgb[i].g<<8)|(lcmaprgb[i].b)
    for (i=0;i<256;i++)
    {						
        if (dopalette)
		{
			lcmaprgb[i].swaprgb();
			if (SC.isindexed())
				lcmap[i]=i;
			else
				lcmap[i]=getcolour(*(UINT32*)&lcmaprgb[i]);
        }  
        else
          lcmap[i]=i;
    }
	buildimagecolourpalettes(&SC,lcmaprgb);
             
    pos=startpos;
        
    tempscan=new UINT8[scanwidth<<1];
    runlength=0;
        
    for (pos.y=0;pos.y<wsize.y;pos.y++)
    {
        memset(tempscan,0,scanwidth<<1);
        subtotal=0;
        cbyte=tempscan;                            
        
        while (subtotal<scanwidth) 
        {                    
            //if ((chk=fgetc(f))==EOF)return NULL;                    
			chk=fgetc(f);
            if(chkbits==(chk & chkbits))
            {                        
                runlength=chk&maskbits;                        
				chk=fgetc(f);
                //if ((chk=fgetc(f))==EOF)return NULL;                                                
            }else                                                                  
                runlength=1;
            //if ((subtotal+runlength)>scanwidth)  return NULL; //error in file, scanlength too long
                                        
            memset(cbyte,chk,runlength);
            cbyte+=runlength;
            subtotal+=runlength;                    
        }                                                                               
        
        UINT32 o=header.offset;
        UINT8 bppi;
        UINT8 colindex;
        UINT32 scanpos;
        //decode scanline from nbpp to 8bpp
            
        for (scanpos=0;scanpos<header.offset;scanpos++)
        {
            if (header.bpp==8)
            {
                if (pos.x<wsize.x)  //clip padding
                    if (header.numplanes==3)                    
                        t_col=getcolour(
                        tempscan[scanpos]<<16|
						(tempscan[scanpos+header.offset]<<8)|
                        (tempscan[scanpos+header.offset+header.offset]));
                        
                    else                                            
                        t_col=lcmap[tempscan[scanpos]];                        
                    
                    
                    putpixel(pos.x,pos.y);
                pos.x++;
            }else
            if (header.bpp==1)
            {                    
                UINT8 cmask=128;
                for (bppi=0;bppi<8;bppi++)
                {
                    if (pos.x<wsize.x)  //clip padding
                    {
                        cmask=128 >> bppi;
                        colindex=0;
                        for (planei=0;planei<header.numplanes;planei++)                        
                            if (tempscan[scanpos+(header.offset*planei)]&cmask)
                               colindex|=1 << planei;
                        t_col=lcmap[colindex];
                        
                        putpixel(pos.x,pos.y);                        
                    }
                    pos.x++;                            
                }                    
            }
            
        }
        pos.x=0;                                 
    }
    delete tempscan;    
    return TRUE;

}



tsurface* readpcx(trendercontext *ctx,FILE *f,tpixelformat pfd,UINT32 attrib)
{
    tsurface oldsc;
    tpoint wsize;
    tsurface *bm;
    UINT32 bpp;
	tpixelformat suggestpfd;
    if (!getpcxmetrics(f,wsize.x,wsize.y,bpp,suggestpfd))return NULL;    
	if (pfd==PXFbestguess)	
		pfd=suggestpfd;

	
    bm=ctx->createsurface(wsize.x,wsize.y,pfd,NULL,attrib);

	if ((attrib & SFremaped)==0)
		if (bm->isindexed())
			bm->setpalette(ctx->createpalette(bm->getpalettesize(),PFlookup));	
    oldsc=SC;          
    ctx->setoutput(bm);    
    if (!loadpcxdata(f,(attrib & IMGLDdontloadpalette)==0)) return NULL;    
	t_col=getcolour(0x00000000);
    ctx->setoutput(&oldsc);	   
    return bm;
}    
    


