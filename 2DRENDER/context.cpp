#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <ddraw.h>
#include <math.h>

#include "ttypes.h"
#include "tmaths.h"
#include "gbasics.h"
#include "surface.h"
#include "context.h"
#include "directxcontext.h"
#include "raster2d.h"
#include "svga64k.h"
#include "strtable.h"

bool GFX_INITIALIZED=0;

tsurface SC_BASE,SC,IC;  //input and output contexts
DDSURFACEDESC2 lockedscreen;

void tpalette::setentries(UINT32 start,UINT32 count,trgba *data)
{
	getcolourfunc getcol=t_getcolourfunc(SC_BASE.pfd);
	trgba *c=&rgbadata[start];
	tcolour *tc=&nativedata[start];
	memcpy(c,data,count<<2);
	if ((flags & PFnativergba)==0)
		while (count--)
		{
			*tc=getcol(*(UINT32*)c);
			tc++;
			c++;
		}
}

void tpalette::getentries(UINT32 start,UINT32 count,trgba *data)
{
	memcpy(data,&rgbadata[start],count);

}

void tpalette::calclookuptable()
{
}

void tpalette::setentry(UINT32 idx,trgba data)
{
	rgbadata[idx]=data;
	nativedata[idx]=t_getcolourfunc(SC_BASE.pfd)(data.getdword());
}

trgba tpalette::getentry(UINT32 idx)
{
	return rgbadata[idx];
    
}



inline tsurface *createsurface_sw(UINT32 x,UINT32 y,UINT32 w,UINT32 h,tpixelformat pfd,char *data,tsurface *old,UINT32 flags)
{
	UINT32* offsets;
    UINT32 i;
	tsurface *nsurf;
	nsurf=new tsurface();	
	
	nsurf->flags=flags;
	nsurf->pfd=pfd;
	nsurf->size.assign(w,h);
	nsurf->viewport.rassign(0,0,w-1,h-1);		
    nsurf->driver=0;	
	UINT32 offsd=nsurf->offsd=nsurf->getstride(w);
    if ( (flags & SFwordaligned) &&(offsd & 3))
        nsurf->offsd+=(4-(nsurf->offsd & 3));  //4 byte align
    else if ( (flags & SFdoublewordaligned) & (offsd & 7))
        nsurf->offsd+=(8-(offsd & 7));  //8 byte align;			                                                 
	nsurf->palette=NULL;//palette_;
	if (data)
	{		
		nsurf->scrptr = data;    
		nsurf->scrptr = nsurf->getpointerat(x,y);
	}
	else
	{	
		nsurf->flags |= SFownsdata;		
		nsurf->scrptr = new char[nsurf->pagesize()];
		if (!nsurf->scrptr)
		{
			delete nsurf;
			return NULL;
		}
	}

	if (old)
	{
		UINT32 bytewidth=MIN(nsurf->offsd,old->getstride(w));
		char *src=old->getpointerat(x,y);
		char *dest=nsurf->scrptr;
		if (src!=dest)
		{
			//do proper colour convertion in here
			for (i=0;i<h;i++)
			{
				memcpy(dest,src,bytewidth);
				dest+=nsurf->offsd;
				src+=old->offsd;
			}
		}
	}
	//get rid of this... soon
            
    
	return nsurf;
}

bool releasesurface_sw(tsurface *surface)
{
	if (surface)	
	{
		if (surface->flags & SFownsdata)
		{
			delete [] surface->scrptr;
			return true;
		}		

	}
	return false;
}



tsurface *trendercontext::createsurface2(tsurface *source,UINT32 w,UINT32 h,tpixelformat pfd,char *data,UINT32 flags)
{
	tsurface old_ic=IC;
	tsurface old_sc=SC;
	
	UINT32 n_samples;					
	trgba c;
	tpoint srcsize=source->size;
			
	tsurface *nsurf=createsurface(0,0,w,h,pfd,data,NULL,flags);	
	if (!nsurf)
		return NULL;
	
	setinput(source);
	setoutput(nsurf);
	getrgbafunc getrgba=t_getrgbafunc(source->pfd); //
	getcolourfunc getcol=t_getcolourfunc(pfd);
	
	UINT32 xi,yi,xi2,yi2;
	UINT32 w_aspect, h_aspect;
	w_aspect= (srcsize.x << 16)/ nsurf->size.x;
	h_aspect= (srcsize.y << 16)/ nsurf->size.y;
	UINT32 top, bottom, left, right;
	top=0;
	bottom=h_aspect;
	UINT32 x1,x2,y1,y2;
	UINT32 cr,cg,cb,ca;

	if ((w==source->size.x)&&(h==source->size.y))
	{
		for( yi=0;yi<h; yi++ )	
		{
			for( xi=0; xi<w; xi++ )
			{	
				t_col = getcol(getrgba(getpixel( xi, yi )));					
				putpixel(xi,yi);
			}	
		}
	}
	else
	for( yi=0;yi<h; yi++ )	
	{
		y1=top>>16;y2=bottom>>16;
		left=0;	
		right=w_aspect;
		for( xi=0; xi<w; xi++ )
		{			
			cr=cg=cb=ca=0;			
			n_samples = 0;			
			x1=left>>16;x2=right>>16;			

			for( yi2=y1; yi2<=y2; yi2++ )			
				for( xi2=x1; xi2<=x2; xi2++ )
				{			
					t_col = getpixel( xi2, yi2 );					
					c=getrgba(t_col);					
					cr += c.r;
					cg += c.g;
					cb += c.b;
					ca += c.a;
					n_samples++;
				}
			
			left+=w_aspect;
			right+=w_aspect;
			
			if (n_samples)
			{
				c.r=cr/n_samples;
				c.g=cg/n_samples;
				c.b=cb/n_samples;
				c.a=cr/n_samples;
			}else
			{
				c.r=cr;c.g=cg;c.b=cb;c.a=ca;
			}

			t_col=getcol(c.getdword());
			putpixel(xi,yi);			
		}
		top+=h_aspect;
		bottom+=h_aspect;
	}
	
	setinput(&old_ic);
	setoutput(&old_sc);

	
	return nsurf;
}

bool trendercontext::releasesurface(tsurface *surface)
{
	return releasesurface_sw(surface);
}

tsurface *trendercontext::createsurface(UINT32 x,UINT32 y,UINT32 w,UINT32 h,tpixelformat pfd,char *data,tsurface *old,UINT32 flags)
{
	return createsurface_sw(x,y,w,h,pfd,data,old,flags);
}

tsurface *trendercontext::createsurface(UINT32 w,UINT32 h,tpixelformat pfd,char *data,UINT32 flags)
{
	return createsurface(0,0,w,h,pfd,data,NULL,flags);
}

tsurface *trendercontext::createsurface(UINT32 x,UINT32 y,UINT32 w,UINT32 h,tsurface *surf)
{	
	tsurface *nsurf=new tsurface();
	*nsurf=*surf;
	nsurf->size.assign(w,h);
	nsurf->scrptr=nsurf->getpointerat(x,y);
	nsurf->viewport.rassign(0,0,w-1,h-1);
	return nsurf;
}
/*

tsurface *trendercontext::createsurface(UINT32 x,UINT32 y,UINT32 w,UINT32 h,tsurface *surf)
{	
	return createsurface(x,y,w,h,surf->pfd,surf->scrptr,surf,surf->flags);	
}*/

tsurface *trendercontext::createsurface(tsurface *surf)
{
	return createsurface(0,0,surf->size.x,surf->size.y,surf->pfd,NULL,surf,surf->flags);
}

tpalette *trendercontext::createpalette(UINT32 size,UINT32 flags)
{
	tpalette *palette=new tpalette();
	palette->flags=flags;
	palette->size=size;		
	palette->rgbadata = new trgba[size];
	if (SC_BASE.pfd==PXFrgba8888)
	{
		flags|=PFnativergba;
		palette->nativedata=(tcolour*)palette->rgbadata;
	}else
		palette->nativedata= new tcolour[size];
	if (flags & PFlookup)
		palette->rgblookup=new UINT8[65536];
	return palette;
}


bool trendercontext::releasepalette(tpalette *palette)
{
	if (palette->nativedata)
		delete [] palette->nativedata;
	if (palette->rgbadata)
		delete [] palette->rgbadata;
	if (palette->nativedata)
		delete [] palette->nativedata;
	return true;
}



bool trendercontext::setinput(tsurface *surf)
{
	bool different;
	if (surf)
	{
		SC_BASE.viewport=IC.viewport;
		different=IC.pfd!=surf->pfd;
		IC=*surf;
	}
	else		
	{
		different=IC.pfd!=SC_BASE.pfd;
		IC=SC_BASE;
	}
	if (different)
	{
	switch (IC.getbitspp())	
	{
		case 32:getpixel=svga32_getpixel;break;
		case 24:getpixel=svga24_getpixel;break;
		case 16:getpixel=svga16_getpixel;break;
		case 8:getpixel=svgai8_getpixel;break;
		case 4:getpixel=svgai8_getpixel;break;
		case 2:getpixel=svgai8_getpixel;break;
		case 1:getpixel=svgai8_getpixel;break;
	}
	switch (IC.pfd)
	{
		case PXFi1:
		case PXFi4:
		case PXFi8:	
			getrgba=cci_getrgba;
			break;
		case PXFrgb888:	
			getrgba=cc888_getrgba;        			
			break;
		case PXFrgba8888:	
			getrgba=cc8888_getrgba;        			
			break;
		case PXFrgba4444:	
			getrgba=cc4444_getrgba;			
			break;
		case PXFrgba5551:	
			getrgba=cc5551_getrgba;        
			break;
		case PXFrgb565:	
			getrgba=cc565_getrgba;			
			break;	
		case PXFgrey16:
			getrgba=ccgrey16_getrgba;
		break;
	}
	}
	return TRUE;
}

bool trendercontext::setoutput(tsurface *surf)
{
	bool different;
	if (surf)
	{
		SC_BASE.viewport=SC.viewport;
		different=SC.pfd!=surf->pfd;
		SC=*surf;
	}else		
	{
		different=SC.pfd!=SC_BASE.pfd;
		SC=SC_BASE;
	}
	if (different)
	switch (SC.getbitspp())
	{
		case 32:
			//drawing routines			
			putpixel=svga32_putpixel;
			hline=svga32_hline;			
			vline=svga32_vline;
			line=svga32_line;
			bar=svga32_bar;
			putbitmap_base=svga32_putbitmap;
			putbitmapfast_base=svga32_putbitmapfast;
			circle=svga32_circle;
			outtextxy_base=svga32_outtextxy;
			drawbytes=svga32_drawbytes;
			cleardevice=svga32_cleardevice;
			break;
		case 24:
			putpixel=svga24_putpixel;
			/*hline=svga24_hline;
			vline=svga24_vline;
			line=svga24_line;
			bar=svga24_bar;
			putbitmap_base=svga24_putbitmap;
			putbitmapfast_base=svga24_putbitmapfast;
			circle=svga24_circle;
			outtextxy_base=svga24_outtextxy;
			drawbytes=svga24_drawbytes;
			cleardevice=svga24_cleardevice;*/
			break;
		case 16:
			//drawing routines			
			putpixel=svga16_putpixel;
			hline=svga16_hline;
			vline=svga16_vline;
			line=svga16_line;
			bar=svga16_bar;
			putbitmap_base=svga16_putbitmap;
			putbitmapfast_base=svga16_putbitmapfast;
			circle=svga16_circle;
			outtextxy_base=svga16_outtextxy;
			drawbytes=svga16_drawbytes;
			cleardevice=svga16_cleardevice;
		break;
		case 1:
		case 4:
		case 8:
			putpixel=svgai8_putpixel;
			hline=svgai8_hline;		                  					
			/*vline=svgai8_vline;
			line=svgai8_line;
			bar=svgai8_bar;
			putbitmap_base=svgai8_putbitmap;
			putbitmapfast_base=svgai8_putbitmapfast;
			circle=svgai8_circle;
			outtextxy_base=svgai8_outtextxy;
			drawbytes=svgai8_drawbytes;
			cleardevice=svgai8_cleardevice;*/
			//drawing routines			
			
			break;				
	}
	switch (SC.pfd)
	{
		case PXFi1:
		case PXFi4:
		case PXFi8:
			getcolour=cci_getcolour;						
			break;
		case PXFrgb888:
			getcolour=cc888_getcolour;        			
			break;
		case PXFrgba8888:
			getcolour=cc8888_getcolour;        			
			break;
		case PXFrgba4444:
			getcolour=cc4444_getcolour;						
			break;
		case PXFrgba5551:
			getcolour=cc5551_getcolour;			
			break;
		case PXFrgb565:
			getcolour=cc565_getcolour;     			
			break;				
		case PXFgrey16:
			getcolour=ccgrey16_getcolour;
		break;
	}
	
	return TRUE;
}

getcolourfunc t_getcolourfunc(tpixelformat pfd)
{
	switch (pfd)
	{
		case PXFi1:
		case PXFi4:
		case PXFi8:
			return cci_getcolour;						
		case PXFrgb888:
			return cc888_getcolour;        						
		case PXFrgba8888:
			return cc8888_getcolour;        						
		case PXFrgba4444:
			return cc4444_getcolour;			
		case PXFrgba5551:
			return cc5551_getcolour;        			
		case PXFrgb565:
			return cc565_getcolour; 
		case PXFgrey16:
			return ccgrey16_getcolour;
			
		break;
	}
	return NULL;
}

getrgbafunc t_getrgbafunc(tpixelformat pfd)
{
	switch (pfd)
	{
		case PXFi1:
		case PXFi4:
		case PXFi8:
			return cci_getrgba;						
		case PXFrgb888:
			return cc888_getrgba;        						
		case PXFrgba8888:
			return cc8888_getrgba;        						
		case PXFrgba4444:
			return cc4444_getrgba;			
		case PXFrgba5551:
			return cc5551_getrgba;        			
		case PXFrgb565:
			return cc565_getrgba;   
		case PXFgrey16:
			return ccgrey16_getrgba;
		//scan setup routines			
		break;
	}
	return NULL;
}

tsurface *trendercontext::getprimary()
{
	primarysurface.pfd=PXFrgba8888;	
	return &primarysurface;
}
  
UINT32 trendercontext::initgraph()
{        
	SC.pfd=IC.pfd=PXFasint;	
    setinput(&primarysurface);
    setoutput(&primarysurface);
        
    t_col=getcolour(0xFFFFFFFF);
    t_fillcol=getcolour(0);

   
    SC_BASE=SC;
    IC=SC;
    GFX_INITIALIZED=1;    
    return 1;
}

tsurface *trendercontext::resizesurface(tsurface *surface,INT32 xlen,INT32 ylen,bool resample)
{
	tpixelformat pfd;
	pfd=surface->pfd;
	tsurface *newsurf;
	
	if (resample)
		newsurf=createsurface2(surface,xlen,ylen,pfd);
	else
		newsurf=createsurface(xlen,ylen,pfd);
	releasesurface(surface);
	return newsurf;       
}



DDSURFACEDESC2 ddsd;

UINT32 trendercontext_directx::create(void *initdata)
{	
	HRESULT r;
	//driver=;	
	hwnd=(HWND)initdata;
	if (FAILED(r = DirectDrawCreateEx(NULL, (VOID**)&dd, IID_IDirectDraw7, NULL)))    
        return r;	
    if (FAILED(r = dd->SetCooperativeLevel(hwnd, DDSCL_NORMAL))) //exclusive?(DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN):
		return r;	
	clipper=NULL;
	lockedscreen.dwSize=sizeof(lockedscreen);
	lockedscreen.dwFlags=DDSD_ALL;

	cliplist.dwSize=sizeof(RGNDATAHEADER);
	cliplist.iType=RDH_RECTANGLES;
	cliplist.nRgnSize=0;

    return (UINT32) r;
}


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
	twin.pfd=PXFasint;
	for (INT32 i=0;i<10;i++)
	{
		if (sd->rmask==win.ddpfPixelFormat.dwRBitMask &&
			sd->gmask==win.ddpfPixelFormat.dwGBitMask &&
			sd->bmask==win.ddpfPixelFormat.dwBBitMask)
			
			//&& sd->amask==win.ddpfPixelFormat.dwRGBAlphaBitMask )
			  break;

		sd++;
	}
	twin.pfd=(tpixelformat)i;									
}


void trendercontext_directx::getmode(UINT32 *x,UINT32 *y,tpixelformat *pfd)
{
	*x=SC.size.x;
	*y=SC.size.y;
	*pfd=SC.pfd;
}

tsurface *trendercontext_directx::getprimary()
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
	
	
	
	primarysurface.flags|=SFprimary;
	SC_BASE=primarysurface;
	return &primarysurface;
}
	

char *geterror(UINT32 error,char *str)
{
	switch (error)
	{
		case DD_OK :break;
		case DDERR_INCOMPATIBLEPRIMARY :break;
		case DDERR_INVALIDCAPS  :break;
		case DDERR_INVALIDOBJECT  :break;
		case DDERR_INVALIDPARAMS   :break;
		case DDERR_INVALIDPIXELFORMAT    :break;
		case DDERR_NOALPHAHW    :break;		
		case DDERR_NOCOOPERATIVELEVELSET    :break;	
		case DDERR_NODIRECTDRAWHW     :break;	
		case DDERR_NOEMULATION :break;
		case DDERR_NOEXCLUSIVEMODE  :break;
		case DDERR_NOFLIPHW   :break;
		case DDERR_NOMIPMAPHW    :break;
		case DDERR_NOZBUFFERHW    :break;
		case DDERR_OUTOFMEMORY     :break;
		case DDERR_OUTOFVIDEOMEMORY     :break;
		case DDERR_PRIMARYSURFACEALREADYEXISTS   :break;
		case DDERR_UNSUPPORTEDMODE   :break;
	}	
	return str;
}



tpalette *trendercontext_directx::createpalette(UINT32 size, UINT32 flags)
{
	tpalette *palette=trendercontext::createpalette(size,flags);	
	return palette;
}

tsurface *trendercontext_directx::createsurface(UINT32 x,UINT32 y,UINT32 w,UINT32 h,tpixelformat pfd,char *data,tsurface *old,UINT32 flags)
{
	//return createsurface_sw(x,y,w,h,pfd,data,old,flags);	
	tsurface *nsurf;
	nsurf=new tsurface();	
	nsurf->pfd=pfd;
	nsurf->palette=NULL;
	
	ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH|DDSD_PIXELFORMAT ;// |DDSD_PIXELFORMAT ;
	
	if (flags & SFtexture)
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
	else
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	if ((flags & SFvideomem)==0)
	{
		 ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
	}	 
	if (flags & SF3ddevice)
		ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE ;
	
	ddsd.dwHeight=h;
	ddsd.dwWidth=w;	
		
	ddsd.ddpfPixelFormat.dwSize=sizeof(DDPIXELFORMAT);
	ddsd.ddpfPixelFormat.dwFlags=DDPF_RGB;
	//if (nsurf->hasalpha())
	//	ddsd.ddpfPixelFormat.dwFlags|=DDPF_ALPHAPIXELS;
	ddsd.ddpfPixelFormat.dwRGBBitCount=nsurf->getbitspp();
	ddsd.ddpfPixelFormat.dwRBitMask=nsurf->getredmask();
	ddsd.ddpfPixelFormat.dwGBitMask=nsurf->getgreenmask();
	ddsd.ddpfPixelFormat.dwBBitMask=nsurf->getbluemask();
	ddsd.ddpfPixelFormat.dwRGBAlphaBitMask=nsurf->getalphamask();	  
    
	IDirectDrawSurface7	*surf;
	HRESULT r;
	r=dd->CreateSurface(&ddsd, &surf, NULL);

	nsurf->devdata=surf;
	geterror(r,NULL);	
	if ((surf)&&(SUCCEEDED(r=surf->Lock(NULL,&lockedscreen,0,NULL))))			    //DDLOCK_WAIT
	{
		convertscreendata(*nsurf,lockedscreen);
		surf->Unlock(NULL);
	}else
	{
		return NULL;
	}
	
	
	
	//nsurf->scrptr=

	return nsurf;   

}

bool trendercontext_directx::releasesurface(tsurface *surface)
{
	//((IDirectDrawSurface7*)surface->devdata)->Release(surface->devdata);
	//return releasesurface_sw(surface);
	delete surface;
	return true;
}

bool trendercontext_directx::releasepalette(tpalette *palette)
{
	
	return trendercontext::releasepalette(palette);
}



UINT32 trendercontext_directx::lockprimary(trect *bounds)
{		
	return (UINT32)((IDirectDrawSurface7*)primarysurface.devdata)->Lock((RECT*)bounds,&lockedscreen,DDLOCK_WAIT,NULL);
}

UINT32 trendercontext_directx::unlockprimary(trect *bounds)
{
	return (UINT32)((IDirectDrawSurface7*)primarysurface.devdata)->Unlock((RECT*)bounds);
}



UINT32 trendercontext_directx::setmode(UINT32 x,UINT32 y,tpixelformat pfd)
{
	return (dd->SetDisplayMode(x, y, surfacedescriptions[pfd].bpp, 0, 0));	
}
  
INT32 trendercontext_directx::initgraph()
{
          
	return trendercontext::initgraph();
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




