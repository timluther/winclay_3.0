//device specific rendering stuff
#ifndef ggraph_hpp
#define ggraph_hpp




typedef struct _RGNDATA2 
{ 
    DWORD dwSize; 
    DWORD iType; 
    DWORD nCount; 
    DWORD nRgnSize; 
    trect rcBound; 
    trect Buffer[255]; 
} RGNDATA2; 


class trendercontext
{	
public:
	UINT32 driver; //software, opengl or directX
	tsurface primarysurface;
	
	virtual UINT32 create();
	virtual tsurface *createsurface(UINT32 x,UINT32 y,UINT32 w,UINT32 h,UINT32 pfd,char *data,tsurface *old,UINT32 flags);
	virtual releasesurface(tsurface *surface);
	virtual tsurface *getprimary();
	virtual setinput(tsurface *surf);
	virtual setoutput(tsurface *surf);
	
	virtual UINT32 setmode(UINT32 x,UINT32 y,UINT32 pfd);
	virtual UINT32 getmode(UINT32 *x,UINT32 *y,UINT32 *pfd);


	tsurface *createsurface(UINT32 w,UINT32 h,UINT32 pfd,char *data=NULL,UINT32 flags=SFdoublewordaligned);
	tsurface *createsurface(tsurface *surf);
	tsurface *createsurface(UINT32 x,UINT32 y,UINT32 w,UINT32 h,tsurface *surf);
};

INT32 setoutput(tsurface *surf);
INT32 setinput(tsurface *surf);

/*

void releasedxbuffers(void)
{
    if (g_pDD != NULL)
    {
        if (g_pDDSPrimary)
        {
            g_pDDSPrimary->Release();
            g_pDDSPrimary = NULL;
        }
		if (clipper)
		{
			clipper->Release();
			clipper=NULL;
		}

        g_pDD->Release();
        g_pDD = NULL;
    }
}


HRESULT restoreall()
{
	
	r=g_pDDSPrimary->Restore();
	if (r==DDERR_WRONGMODE)
	{
		if (clipper)
		{
			clipper->Release();
			clipper=NULL;
			r=g_pDD->CreateClipper(0,&clipper,NULL);
			clipper->SetHWnd(0,scrhwnd);	

		}
		if (g_pDDSPrimary)
		{
			g_pDDSPrimary->Release();
			if (FAILED(r = g_pDD->CreateSurface(&ddsd, &g_pDDSPrimary, NULL)))
				return r;    	
		}
		
	}
	
	g_pDDSPrimary->SetClipper(clipper);

	return r;

}
*/

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
	virtual releasesurface(tsurface *surface);
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

void buildtables();

void closegraph();
//char initgraph(screenIDType &sID);
//input and output contexts. getsurface and getpixel are input context related functions.
/*tsurface *createsurface(UINT32 x,UINT32 y,UINT32 w,UINT32 h,UINT32 pfd,char *data,tsurface *old,UINT32 flags);
tsurface *createsurface(UINT32 w,UINT32 h,UINT32 pfd,char *data=NULL,UINT32 flags=ESFdoublewordaligned);
tsurface *createsurface(tsurface *surf);
tsurface *createsurface(UINT32 x,UINT32 y,UINT32 w,UINT32 h,tsurface *surf);
INT32 setinput(tsurface*sc=NULL);
INT32 setoutput(tsurface*sc=NULL);*/

extern tsurface SC,IC;

#endif