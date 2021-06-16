#include <string.h>
#include <math.h>
#include "ttypes.h"
#include "tmaths.h"
#include "gbasics.h"
#include "surface.h"

char *surface_textdescriptions[14]=
{"1 bit indexed",
 "4 bit indexed",
 "8 bit indexed",
 "4 bit plainar indexed",
 "8 bit plainar indexed",
 "16 bit (565) truecolour",
 "24 bit truecolour",
 "16 bit (5551) truecolour",
 "32 bit truecolour",
 "16 bit (4444) truecolour",
 "8 bit greyscale",
 "16 bit greyscale",
 "32 bit greyscale",
 "Unformated"};

surfacedescriptor surfacedescriptions[14]=
{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,SDFindexed,1},  //PXFi1 
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,SDFindexed,16},	//PXFi4 
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,8,SDFindexed,256},	//PXFi8 
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,SDFindexed,16},	//PXFpi4 
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,8,SDFindexed,256},	//PXFpi8 
{0xF800,0x7E0,0x1F,0,0,10,5,0,0,0,5,6,5,0,0,2,16,SDFtruecolour,0}, //PXFr5g6b5 
{0xFF0000,0xFF00,0xFF,0,0,16,8,0,0,0,8,8,8,0,0,3,24,SDFtruecolour,0},	////PXFr8g8b8 
{0x7C00,0x3E0,0x1F,0x1000,0,10,5,0,15,0,5,5,5,1,0,2,16,SDFtruecolour|SDFalphachannel,0}, //PXFr5g5b5a1
{0xFF0000,0xFF00,0xFF,0xFF000000,0,16,8,0,24,0,8,8,8,8,0,3,32,SDFtruecolour+SDFalphachannel,0},	////PXFr8g8b8a8 
{0xF00,0xF0,0xF,0xF000,0,8,4,0,12,0,4,4,4,4,0,2,16,SDFtruecolour|SDFalphachannel,0},	//PXFr4g4b4a4 					
{0x0,0x0,0x0,0x0,0,0,0,0,0,0,0,0,0,0,0,1,8,SDFgreyscale,0},	 //PXFgrey8
{0x0,0x0,0x0,0x0,0,0,0,0,0,0,0,0,0,0,0,2,16,SDFgreyscale,0}, //PXFgrey16
{0x0,0x0,0x0,0x0,0,0,0,0,0,0,0,0,0,0,0,4,32,SDFgreyscale,0}, //PXFgrey32
{0x0,0x0,0x0,0x0,0,0,0,0,0,0,0,0,0,0,0,1,8,0,0}				 //noformat
};

void tsurface::rotate90()
{
    UINT32 bpp=getbpp();
    UINT32 addy=size.y*bpp;
	UINT32 psize=pagesize();
    
    char *newdata=new char[psize];
    char *src,*dst=newdata;
    
    for (INT32 y=0;y<size.y;y++)
    {
      char* olddst=dst;
      src=scrptr+(y*offsd);
      for (INT32 x=0;x<size.x;x++)
      {
          memmove(dst,src,bpp);
          //*dst=*src;
          dst+=addy;
          src+=bpp;          
      }
      dst=olddst;
      dst+=bpp;
    }
    swap(size.y,size.x);
	viewport.rassign(0,0,size.x-1,size.y-1);
    memcpy(scrptr,newdata,psize);
	delete newdata;
    offsd=size.x*bpp;    
}



void tsurface::setpalette(tpalette *pal)
{
	if (flags & SFprimary)	
		pal->flags|=PFsystem;	
	palette=pal;
}

