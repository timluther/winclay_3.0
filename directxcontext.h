#ifndef trendercontext_directx
#define trendercontext_directx

class trendercontext_svga:public trendercontext
{	
public:
	UINT32 driver; //software, opengl or directX
	tsurface primarysurface;
	
	virtual UINT32 create();
	virtual tsurface *createsurface(UINT32 x,UINT32 y,UINT32 w,UINT32 h,UINT32 pfd,char *data,tsurface *old,UINT32 flags);	
	virtual tsurface *createsurface(UINT32 x,UINT32 y,UINT32 pfd,UINT32 flags);
	virtual releasesurface(tsurface *surface);
	virtual tsurface *getprimary();
	virtual setinput(tsurface *surf);
	virtual setoutput(tsurface *surf);
	
	virtual UINT32 setmode(UINT32 x,UINT32 y,UINT32 pfd);
	virtual UINT32 getmode(UINT32 *x,UINT32 *y,UINT32 *pfd);
};


class trendercontext_directx:public trendercontext
{
public:
	HWND hwnd;
	IDirectDrawClipper *clipper;
	IDirectDraw7 *dd;
	RGNDATA2 cliplist;
	virtual UINT32 create();
	virtual tsurface *createsurface(UINT32 x,UINT32 y,UINT32 w,UINT32 h,UINT32 pfd,char *data,tsurface *old,UINT32 flags);	
	virtual bool releasesurface(tsurface *surface);
	virtual tsurface *getprimary();
	virtual INT32 setinput(tsurface *surf);
	virtual INT32 setoutput(tsurface *surf);	
	virtual UINT32 setmode(UINT32 x,UINT32 y,UINT32 pfd);	
	void sethwnd(HWND _hwnd){hwnd=_hwnd;};
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

#endif