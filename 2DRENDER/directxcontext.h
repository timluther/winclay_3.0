#ifndef trendercontext_directx_h_
#define trendercontext_directx_h_



class trendercontext_directx:public trendercontext
{
public:
	HWND hwnd;
	IDirectDrawClipper *clipper;
	IDirectDraw7 *dd;
	RGNDATA2 cliplist;
	trendercontext_directx(){dd=NULL;clipper=NULL;hwnd=NULL;}
	LPDIRECTDRAWSURFACE7 getprimarydd(){return (LPDIRECTDRAWSURFACE7)primarysurface.devdata;}
	virtual UINT32 create(void *initdata);
	virtual tpalette *createpalette(UINT32 size,UINT32 flags);
	virtual bool releasesurface(tsurface *surface);
	virtual bool releasepalette(tpalette *palette);
	virtual tsurface *getprimary();
	//virtual bool setinput(tsurface *surf);
	//virtual bool setoutput(tsurface *surf);	
	virtual UINT32 setmode(UINT32 x,UINT32 y,tpixelformat pfd);	
	virtual tsurface *createsurface(UINT32 x,UINT32 y,UINT32 w,UINT32 h,tpixelformat pfd,char *data,tsurface *old,UINT32 flags);
	virtual void getmode(UINT32 *x,UINT32 *y,tpixelformat *pfd);
	void sethwnd(HWND _hwnd)
	{
		hwnd=_hwnd;
		if (clipper)
			clipper->SetHWnd(0,hwnd);		
	};
	INT32 initgraph();
	UINT32 lockprimary(trect *bounds);
	UINT32 unlockprimary(trect *bounds);
	
	UINT32 createclipper()
	{
		HRESULT r=dd->CreateClipper(0,&clipper,NULL);	
		clipper->SetHWnd(0,hwnd);	        
		((IDirectDrawSurface7*)primarysurface.devdata)->SetClipper(clipper);	
		return (UINT32)r;
	}
};

class trendercontext_svga:public trendercontext
{	
public:		
	virtual UINT32 create(void *initdata);
	virtual tsurface *createsurface(UINT32 x,UINT32 y,UINT32 w,UINT32 h,UINT32 pfd,char *data,tsurface *old,UINT32 flags);	
	virtual tsurface *createsurface(UINT32 x,UINT32 y,UINT32 pfd,UINT32 flags);
	virtual bool releasesurface(tsurface *surface);
	virtual tsurface *getprimary();
	virtual tpalette *createpalette(UINT32 flags);	
	virtual bool releasepalette(tpalette *palette);
	virtual bool setinput(tsurface *surf);
	virtual bool setoutput(tsurface *surf);
	
	virtual UINT32 setmode(UINT32 x,UINT32 y,UINT32 pfd);
	virtual void  getmode(UINT32 *x,UINT32 *y,UINT32 *pfd);
};



#endif