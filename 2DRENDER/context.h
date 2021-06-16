//device specific rendering stuff
#ifndef ggraph_hpp
#define ggraph_hpp




typedef struct _RGNDATA2 
{ 
    UINT32 dwSize; 
    UINT32 iType; 
    UINT32 nCount; 
    UINT32 nRgnSize; 
    trect rcBound; 
    trect Buffer[255]; 
} RGNDATA2; 


class trendercontext
{	
public:
	UINT32 driver; //software, opengl or directX
	tsurface primarysurface;
	trendercontext(){driver=0;}
	virtual UINT32 create(void *initdata){return 1;};
	virtual tsurface *createsurface(UINT32 x,UINT32 y,UINT32 w,UINT32 h,tpixelformat pfd,char *data,tsurface *old,UINT32 flags);
	virtual bool releasesurface(tsurface *surface);
	virtual bool releasepalette(tpalette *palette);
	virtual tsurface *getprimary();
	virtual bool setinput(tsurface *surf);
	virtual bool setoutput(tsurface *surf);	
	virtual UINT32 setmode(UINT32 x,UINT32 y,tpixelformat pfd){return 0;};
	virtual void getmode(UINT32 *x,UINT32 *y,tpixelformat *pfd){};
	virtual tpalette *createpalette(UINT32 size,UINT32 flags);
	UINT32 initgraph();
	tsurface *convertsurface(UINT32 pfd,tsurface *insurface,tsurface *outsurface);
	tsurface *resizesurface(tsurface *surf,INT32 x,INT32 y,bool resample);
	tsurface *createsurface2(tsurface *source,UINT32 w,UINT32 h,tpixelformat pfd,char *data=NULL,UINT32 flags=SFdoublewordaligned);
	tsurface *createsurface(UINT32 w,UINT32 h,tpixelformat pfd,char *data=NULL,UINT32 flags=SFdoublewordaligned);
	tsurface *createsurface(tsurface *surf);
	tsurface *createsurface(UINT32 x,UINT32 y,UINT32 w,UINT32 h,tsurface *surf);
};

extern tsurface SC,IC;

#endif