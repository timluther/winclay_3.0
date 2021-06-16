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

class tdirectx:public trenderdevice
{
public:
	HWND hwnd;
	IDirectDrawClipper *clipper;
	IDirectDraw7 *dd;
	RGNDATA2 cliplist;
	virtual UINT32 create();
	virtual tsurface *createsurface(UINT32 x,UINT32 y,UINT32 pfd,tsurface *old,UINT32 flags);
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
#endif