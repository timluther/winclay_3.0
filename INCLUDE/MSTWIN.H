// Foward declarations of functions included in this code module:


class mswindow:public window
{
public:
	HWND hwnd;
	mswindow(INT16 x,INT16 y,INT16 w,INT16 h,char *txt,tview *cview,UINT16 createflags,HWND wnd);

	//virtual void handleevent(tevent *event);
	//virtual void draw();
	virtual void changebounds(trect &bounds);
	void setbounds(trect &bounds);
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND createmainwindow(INT32 xsize,INT32 ysize,HINSTANCE hInstance);
void unlockcallback(trect *bounds);
void lockcallback(trect *bounds);
trectlist *cliplistcallback(trect &bounds);
void cleanupwindowsstructures();
void getkeyevents(MSG &msg,tevent &event);

extern mswindow *mainwin;

