#include "stdafx.h"
#include <zmouse.h>
#include <ddraw.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "resource.h"
#include "graphics.h"
#include "gprim.h"
#include "msmouse.h"
#include "views.h"
#include "widgets.h"
#include "mstwin.h"


bool settingwindowssize=false;
class keyboard
{
public:
	UINT32 keycodes[8];

	void clearkey(UINT32 key)
	{
		keycodes[key >>5] &=~(1<<(key & 31));
	}
	void setkey(UINT32 key)
	{
		keycodes[key >>5] |=(1<<(key & 31));
	}
	UINT32 readkey(UINT32 key)
	{
		return keycodes[key>>5] & (1<<(key & 31));
	}
};

UINT16 scancodes[0x200];
char szWindowClass[]="IDC_MAINWIN";								// The window's class
HWND scrhwnd;
HHOOK g_mousehook;
HHOOK g_keybhook;

trendercontext_directx *dxdev;
//DDSURFACEDESC2 lockedscreen;
//DDSURFACEDESC2 ddsd;
HRESULT r;
// Foward declarations of functions included in this code module:
HRESULT             InitApp(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

mswindow::mswindow(INT16 x,INT16 y,INT16 w,INT16 h,char *txt,tview *cview,UINT16 createflags,HWND wnd):
window(x,y,w,h,txt,cview,createflags)
{
	options |=OFtoplevelalien;
	hwnd=wnd;
}

void mswindow::changebounds(trect &bounds)
{
	tgroup::changebounds(bounds);	
	//if ((state & SFsupresswindowsmessaging)==0)
	settingwindowssize=true;
	SetWindowPos(hwnd,HWND_TOP,a.x,a.y,width()+1,height()+1,0);
	settingwindowssize=false;
}

void mswindow::setbounds(trect &bounds)
{
	tgroup::changebounds(bounds);	
	
}


mswindow *mainwin=NULL;

//-----------------------------------------------------------------------------
// Name: ReleaseAllObjects()
// Desc: Finished with all objects we use; release them
//-----------------------------------------------------------------------------

/*typedef struct tagMSLLHOOKSTRUCT {
    POINT     pt;
    DWORD     mouseData;
    DWORD     flags;
    DWORD     time;
    ULONG_PTR dwExtraInfo;
} MSLLHOOKSTRUCT, *PMSLLHOOKSTRUCT;*/

/*SMleftshiftkey 2
SMleftctrlkey 4
SMleftaltkey 8
SMrightshiftkey 2
SMrightctrlkey 4
SMrightaltkey 8
VK_RSHIFT 
VK_LCONTROL VK_RCONTROL 
VK_LMENU VK_RMENU 
*/


void getkeyevents(MSG &msg,tevent &event)
{
	static UINT32 r;
	switch (msg.message)
	{
		case WM_KEYDOWN:				
			scancode=msg.wParam;
			//charcode=MapVirtualKey(wParam,2);
			//scancodes[wParam]=1;
		
			TranslateMessage(&msg);
			PeekMessage(&msg,msg.hwnd,0,0xFFFFFFF,PM_REMOVE);
			if (msg.message==WM_CHAR)
			  charcode=msg.wParam;
			else
			  charcode=0;
			event.what=EVkeydown;						
			break;				
		case WM_KEYUP:
			event.what=EVkeyup;
			//scancodes[wParam]=0;
			break;
	}
}

UINT32 KEYgetscan()
{
	UINT32 scan=0;
	if (GetAsyncKeyState(VK_SHIFT))
		scan|=SMleftshiftkey;
	if (GetAsyncKeyState(VK_CONTROL))
		scan|=SMleftctrlkey;
	if (GetAsyncKeyState(VK_MENU))
		scan|=SMleftaltkey;
	return scan;
}

UINT32 TIMEgettime()
{
	return GetTickCount();
}

 

LRESULT CALLBACK mousehookproc(int nCode,WPARAM wParam,LPARAM lParam)
{
	 MOUSEHOOKSTRUCT *mousedata;//==HC_ACTION
	 if (nCode>=0)  
	 {
		mousedata=(MOUSEHOOKSTRUCT*)lParam;
						
		switch (wParam)
		{
			case WM_MOUSEWHEEL:
				//ms_attrib|=MS_WHEEL;
				//mwheel=msg.wParam;
				//_asm sar mwheel,16						 		
				break;
			case WM_NCLBUTTONDBLCLK:
			case WM_LBUTTONDBLCLK:
				ms_attrib|=MS_DOUBLECLICKED;
				break;
			case WM_RBUTTONDOWN:		
				internalmb|=2;		
				SetCapture(mousedata->hwnd);
				break;
			case WM_LBUTTONDOWN:			
				
				internalmb|=1;
				SetCapture(mousedata->hwnd);
			break;
			case WM_RBUTTONUP:
				internalmb&=~2;
				ReleaseCapture();
			break;
			case WM_LBUTTONUP:
				internalmb&=~1;
				ReleaseCapture();
			break;
			case WM_MOUSEMOVE:				
				internalmp=*(tpoint*)&mousedata->pt;
				
				//internalmp.assign(LOWORD(msg.lParam),HIWORD(msg.lParam));			
				//ClientToScreen(msg.hwnd,(POINT*)&internalmp);
			break;
		}
		return 0;
	 }else if (nCode<0)
	 {
		return CallNextHookEx(  g_mousehook,nCode,wParam,lParam);
	 }
	 return 1;


}



trectlist *cliplistcallback(trect &bounds)
{
	trectlist *p=NULL,*first=NULL,*n;
	unsigned long count;
	dxdev->clipper->GetClipList(NULL,NULL,&count);
	trect b=bounds;
	++b.b;
			
	dxdev->clipper->GetClipList((RECT*)&b,(RGNDATA*)&dxdev->cliplist,&count);
	count=(count >> 4) -2;	

	if (count>0)
	{
		first=p=getrectmem();
		*p=dxdev->cliplist.Buffer[0];		
		--p->b;
		p->rintersect(bounds);
		for (unsigned long i=1;i<count;i++)
		{
			n=getrectmem();			
			p->next=n;
			*n=dxdev->cliplist.Buffer[i];		
			--n->b;
			n->rintersect(bounds);
			p=n;
		}
	
		p->next=NULL;
	}
	return first;

}

void lockcallback(trect *bounds)
{
	//bool lostit=dxdev->IsLost();
	//if (lostit)
	//restoreall();
		//if (FAILED(r=restoreall())) return;
	dxdev->lockprimary(bounds);	
}

void LastErrorBox(HWND hwnd)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf,0,NULL );
	MessageBox( hwnd, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
	LocalFree( lpMsgBuf );
}
void unlockcallback(trect *bounds)
{
	dxdev->unlockprimary(bounds);	
}

HWND createmainwindow(INT32 xsize,INT32 ysize,HINSTANCE hInstance)
{
	HWND                        hWnd;
    WNDCLASS                    wc;
    
    // Set up and register window class
    wc.style = CS_PARENTDC|CS_OWNDC |CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(wc.hInstance, (LPCTSTR)IDI_SMALL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground =NULL;// (HBRUSH )GetStockObject(BLACK_BRUSH); //NULL;//
    wc.lpszMenuName = NULL;//(LPCSTR)IDC_test;
    wc.lpszClassName = szWindowClass;
    RegisterClass(&wc);
	
	//hWnd = CreateWindow(szTitle, szWindowClass, 0,CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
    //hWnd = CreateWindow(szTitle, szWindowClass, WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	//
    hWnd = CreateWindowEx(0, szWindowClass,"Clayworks",WS_POPUP,0,0,xsize,ysize,NULL,NULL,hInstance,NULL);
	//GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN)
	scrhwnd=hWnd;
	dxdev = new trendercontext_directx();
	TWINsetcontext(dxdev);
	
	if (FAILED(dxdev->create((void*)hWnd))) return NULL;
	if (FAILED(dxdev->getprimary())) return NULL;
	if (FAILED(dxdev->createclipper())) return NULL;
	
	if (!dxdev->initgraph()) return NULL;

	g_mousehook=SetWindowsHookEx(WH_MOUSE, mousehookproc,NULL,GetCurrentThreadId() );    
	
	if (!g_mousehook)
	{
		LastErrorBox(hWnd);
		
	} 

    return hWnd;
}

/*
void TWINeventkey()
{
  tevent keyevent;
  tview *currview;
  if (kbhit())
  {
    charcode=getch();
    if (charcode==0) 
        scancode=getch();
    else
        scancode=0;
    itoa(scancode,buf,10);
    
    label(0,SC.size.y-20,buf);
    
    keyevent.what=EVkeydown;
    if (global_selected!=NULL)
    {
        currview=global_selected;
        while ((keyevent.what!=0)&&(currview!=NULL))
        {
            currview->handleevent(&keyevent);
            //if (keyevent.what==0)
            //  rectangle(currview->a.x,currview->a.y,currview->b.x,currview->b.y);
            currview=currview->owner;
        }
    }else
        root->handleevent(&keyevent);
  }
}*/

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	
	//PAINTSTRUCT ps;
		
	tpoint size;
	trect bounds,oldvp;
	tpoint movep;
	

	switch (message) 
	{
		case WM_ACTIVATEAPP:
			SetFocus(hWnd);
			//buildcontextfromsurface(g_pDDSPrimary,&SC);
			break;

		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{			
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
			}
			break;	
		case WM_MOUSEMOVE:
		case WM_NCPAINT:
		case WM_ERASEBKGND:			
			return 0;
		case WM_PAINT:			
			if (mainwin)
			{
				oldvp=SC.viewport;
				if (GetUpdateRect(hWnd,(RECT*)&SC.viewport,FALSE))
				{
					SC.viewport.a+=mainwin->a;
					SC.viewport.b+=mainwin->a-1;
					if ((mainwin->state & SFresizing)==0)
						mainwin->drawview();
					
					bounds.rassign(0,0,mainwin->width()+1,mainwin->height()+1);//mainwin->width(),mainwin->height());
					ValidateRect(hWnd,(RECT*)&bounds);//			
				}
				SC.viewport=oldvp;
			}
			
			return FALSE;
		case WM_SIZE:
			//need to still handle this somehow
			size.assign(LOWORD(lParam),HIWORD(lParam));
			/*GetWindowRect(hWnd,(RECT*)&bounds);	
			--bounds.b;*/
			
			if (mainwin && !settingwindowssize)
			{			   
			   mainwin->setbounds(bounds);						   
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
				
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }		   

   return 0;
}

void cleanupwindowsstructures()
{
	UnhookWindowsHookEx(g_mousehook);
}