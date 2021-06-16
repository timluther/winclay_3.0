
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <ddraw.h>

#include "ttypes.h"
#include "tmaths.h"
#include "gbasics.h"
#include "svga64k.h"
#include "ggraph.h"
//#include "vga256.h"
//#include "svga16.h"
//#include "svga32k.h"
//#include "vesainfo.h"
//#include "topengl.h"
#include "strtable.h"

char GFX_INITIALIZED=0;


DDSURFACEDESC2 lockedscreen;



UINT32 tdirectx::create()
{	
	HRESULT r;
	driver=STdirectx;	
	if (FAILED(r = DirectDrawCreateEx(NULL, (VOID**)&dd, IID_IDirectDraw7, NULL)))    
        return r;	
    if (FAILED(r = dd->SetCooperativeLevel(hwnd, DDSCL_NORMAL))) //exclusive?(DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN):
		return r;	
	
	lockedscreen.dwSize=sizeof(lockedscreen);
	lockedscreen.dwFlags=DDSD_ALL;

	cliplist.dwSize=sizeof(RGNDATAHEADER);
	cliplist.iType=RDH_RECTANGLES;
	cliplist.nRgnSize=0;

    return (UINT32) r;
}




/*tsurface *trenderdevice::createsurface(UINT32 x,UINT32 y,UINT32 pfd,tsurface *old,UINT32 flags)
{
	return createsurface(x,y,pfd,old,flags);
}*/

DDSURFACEDESC2 ddsd;

UINT32 GetNumberOfBits( UINT32 dwMask )
{
    UINT32 wBits = 0;
    while( dwMask )
    {
        dwMask = dwMask & ( dwMask - 1 );  
        wBits++;
    }
    return wBits;
}


void convertscreendata(tsurface &twin,DDSURFACEDESC2 &win)
{
	twin.scrptr=(char *)win.lpSurface;
	twin.offsd=win.lPitch;
	twin.size.x=win.dwWidth;
	twin.size.y=win.dwHeight;
	twin.viewport.rassign(0,0,twin.size.x-1,twin.size.y-1);		
	UINT32 rbitcount=GetNumberOfBits(win.ddpfPixelFormat.dwRBitMask);
	UINT32 gbitcount=GetNumberOfBits(win.ddpfPixelFormat.dwGBitMask);
	UINT32 bbitcount=GetNumberOfBits(win.ddpfPixelFormat.dwBBitMask);

	
	surfacedescriptor *sd=surfacedescriptions;
	//twin.pagesize=twin.size.y*twin.offsd;
	//try to work out pixel format from masks
	twin.pfd=0xFFFFFFFF;
	for (INT32 i=0;i<10;i++)
	{
		if (sd->rmask==win.ddpfPixelFormat.dwRBitMask &&
			sd->gmask==win.ddpfPixelFormat.dwGBitMask &&
			sd->bmask==win.ddpfPixelFormat.dwBBitMask &&
			sd->amask==win.ddpfPixelFormat.dwRGBAlphaBitMask )
			  break;

		sd++;
	}
	twin.pfd=i;									
}


tsurface *tdirectx::getprimary()
{
	HRESULT r;
	ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	ddsd.dwBackBufferCount = 0;
	IDirectDrawSurface7	*surf;
    if (FAILED(r = dd->CreateSurface(&ddsd, &surf, NULL)))
		return NULL;    	
	primarysurface.devdata=(void*)surf;	
	if (SUCCEEDED(r=surf->Lock(NULL,&lockedscreen,0,NULL)))			    //DDLOCK_WAIT
	{
		convertscreendata(primarysurface,lockedscreen);
		surf->Unlock(NULL);
	}else
	{

		return NULL;
	}
	cliplist.rcBound.rassign(0,0,primarysurface.size.x,primarysurface.size.y);
	SC_BASE=primarysurface;
	UINT32 *offsets=(UINT32*)primarysurface.startoffsets= new UINT32[primarysurface.size.y];     
	if (primarysurface.startoffsets)
	{
		offsets=primarysurface.startoffsets;	
		for (INT32 i=0;i<primarysurface.size.y;i++)    
			*offsets++=(UINT32)primarysurface.scrptr+(i*primarysurface.offsd);                   
	}
	return &primarysurface;
}

tsurface *tdirectx::createsurface(UINT32 x,UINT32 y,UINT32 pfd,tsurface *old,UINT32 flags)
{
	return createsurface(x,y,pfd,old,flags);
	/*tsurface *nsurf;
	nsurf=new tsurface();	
	
	ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;    */
}




UINT32 tdirectx::lockprimary(trect *bounds)
{		
	return (UINT32)((IDirectDrawSurface7*)primarysurface.devdata)->Lock((RECT*)bounds,&lockedscreen,DDLOCK_WAIT,NULL);
}

UINT32 tdirectx::unlockprimary(trect *bounds)
{
	return (UINT32)((IDirectDrawSurface7*)primarysurface.devdata)->Unlock((RECT*)bounds);
}

INT32 tdirectx::setinput(tsurface *surf)
{
	
	if (surf->pfd!=IC.pfd)
	switch (surf->pfd)	
	{
		case PXFr5g5b5:
		case PXFr5g6b5:
		case PXFr5g5b5a1:			
			inputgetpixel=svga64k_getpixel;
			inputgetrgb=svga64k_getrgb;
			inputgetcolour=svga64k_getcolour;	
			break;
	}
	IC=(surf)?*surf:primarysurface;		
	return 1;
}

INT32 tdirectx::setoutput(tsurface *surf)
{
	if (surf->pfd!=SC.pfd)
	switch (surf->pfd)
	{
		case PXFr5g5b5:
		case PXFr5g6b5:
		case PXFr5g5b5a1:		
			getrgb=svga64k_getrgb;
			getpixel=svga64k_getpixel;
			getcolour=svga64k_getcolour;        
			//scan setup routines
			setpixmapmode=svga64k_pixmapmode;
			setscanmode=svga64k_scanmode;		        
			//drawing routines
			putpixel=svga64k_putpixel;
			hline=svga64k_hline;
			vline=svga64k_vline;
			line=svga64k_line;
			bar=svga64k_bar;        
			tri=svga64k_tri;        
			putbitmap_base=svga64k_putbitmap;        
			putbitmapfast_base=svga64k_putbitmapfast;        
			circle=svga64k_circle;
			outtextxy_base=svga64k_outtextxy; 
			drawbytes=svga64k_drawbytes;
			cleardevice=svga64k_cleardevice;                        
			break;
	}
	SC=(surf)?*surf:primarysurface;		
	return 1;
}

UINT32 tdirectx::setmode(UINT32 x,UINT32 y,UINT32 pfd)
{
	return (dd->SetDisplayMode(x, y, surfacedescriptions[pfd].bpp, 0, 0));	
}
  
INT32 tdirectx::initgraph()
{
      
    ERR_LEVEL=ERR_NOERROR; 
        
    //SC.colourdepth=sID.colourdepth;    
    
    t_currentscanmode=0;
	t_currentpixmapmode=0;
  
    setinput(&primarysurface);
    setoutput(&primarysurface);
    

    /*COLinit(SC.colourdepth);   
       
    if (!setmode(sID))
    {
        printf("Error encountered initializing graphics driver:\n%s\n",ERR_STRINGS[ERR_LEVEL]);
        ERR_LEVEL=0;
        return 0;
    }*/
	
    t_col=getcolour(255,255,255);
    t_fillcol=getcolour(0,0,0);
    t_chroma=t_fillcol;
    //t_writemode=
    t_linepattern=0xff;
    t_palette=NULL;
    t_nativepalette=NULL;    
	buildtables();
    
    SC_BASE=SC;
    IC=SC;
    GFX_INITIALIZED=1;    
    return 1;
}

void _setmode(UINT16 mode)
{
	_asm {int 0x10};
}

void closegraph()
{
    delete SC.startoffsets;
    closemode();
    COLdone();
    GFX_INITIALIZED=0;    
    _setmode(0x3);
}


void waitvbl()
{
    _asm
    {
		mov dx,0x3da
        runloop:
           in al,dx
           test al,08h
        jz runloop
    }
}

