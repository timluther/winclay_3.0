//msmouse unit

//uses gbasics,tgraph;
#include "stdafx.h"
#include "graphics.h"
#include "msmouse.h"




#define COM1INTR 0x0C
#define COM1PORT 0x3F8

//*****Globals*****

trect oldvp;
tsurface *mouseimage=NULL;
tsurface *mouseunderarea=NULL;
tcolour black;
tpoint hotspot;
trect mrect,ms_bounds;
UINT32 ms_counter;
UINT32 ms_dcounter;
UINT32 ms_lcounter;

signed int ms_visible=0;
char oldvis=FALSE;
INT32 mwheel,moldwheel,internalmwheel;
tpoint mp,moldp,mdiff,internalmdiff,internalmp(0,0);
UINT32 mb,moldb,internalmb;
//UINT32 mscan,moldscan;

tpoint msize;
//change to single attribute
UINT32 ms_attrib;


void MOUSEsetpos(INT16 x,INT16 y)
{

}

bool MOUSEreset()
{  

	return TRUE;
  
}


//extern void mouseon();



void MOUSEsetshape(INT16 x,INT16 y,tsurface *bm)
{

}

void MOUSEoff()
{
    
}

void MOUSEon()
{

}

    
void MOUSEsetsense(INT16 x,INT16 y,INT16 dspeed)
{

}

void MOUSEsetbounds(trect *r)
{
    ms_bounds=*r;
	ClipCursor((RECT*)r);   // pointer to structure with rectangle);
}

void MOUSEsetbounds(INT32 x1,INT32 y1,INT32 x2,INT32 y2)
{
	ms_bounds.rassign(x1,y1,x2,y2);
	ClipCursor((RECT*)&ms_bounds);   // pointer to structure with rectangle);

}

void MOUSEhardwarepoll()
{
}

void MOUSEupdatepos(INT32 x,INT32 y)
{
	moldp=mp;
	mp.assign(x,y);
	mdiff=mp-moldp;
	mrect.a=mp;
	mrect.b.x=mp.x+msize.x;
	mrect.b.y=mp.y+msize.y;
	ms_attrib &=~MS_MOVED;
	if (moldp!=mp) ms_attrib|=MS_MOVED;      
	
	
}

void MOUSEupdatebutton(INT32 b)
{
	moldb=mb;
	mb=b;
	
}



//****Globals****
extern signed int ms_visible;
extern char oldvis;
extern tpoint mp,moldp,mdiff,internalmp;
extern UINT32 mb,moldb,internalmb;
extern UINT32 mscan,moldscan;
extern INT32 mwheel,moldwheel,internalmwheel;


extern tpoint msize;
extern UINT32 ms_attrib;

void MOUSEclearflags()
{
	ms_attrib =0;//&=~(MS_MOVED + MS_PRESSED + MS_RELEASED + MS_CLICKED + MS_DOUBLECLICKED + MS_WHEEL);
}

void MOUSEpoll()
{
  mp=internalmp;
  mdiff=mp-moldp;    
  mrect.a=mp;
  mrect.b.x=mp.x+msize.x;
  mrect.b.y=mp.y+msize.y;        
  if (moldp!=mp) ms_attrib|=MS_MOVED;
  moldp=mp;

  /*if ((ms_moved)&&(ms_visible))
  {
      oldvp=SC.viewport;
      SC.viewport.rassign(SC.size.x,SC.size.y);
      if (mouseunderarea)
      {
         putsurface(moldp.x,moldp.y,mouseunderarea);
         getsurface(mp.x,mp.y,mouseunderarea);          
      }
      if (mouseimage)
          putsurface(mp.x,mp.y,mouseimage);
      SC.viewport=oldvp;
  }*/  
    
  mb=internalmb;
  if ((moldb!=0)&&(mb==0)) ms_attrib|=MS_RELEASED;
  if ((moldb!=mb)&&(mb!=0)) ms_attrib|=MS_PRESSED;  
  moldb=mb;

  /*if (MOUSEpressed) ms_counter=TIMEgettime()+clickwait;  
  if ((MOUSEreleased)&&(TIMEgettime()<ms_counter)) ms_attrib|=MS_CLICKED;
  if ((TIMEgettime()<ms_dcounter)) ms_attrib|=MS_DOUBLECLICKED;//&&(oldmouserect.rcontains(mp.x,mp.y))
  if (MOUSEclicked) ms_dcounter=TIMEgettime()+dclickwait;      */
}

bool  MOUSEinit(tsurface *mouseimage_)
{
    black=0;
    ms_counter=clickwait;
    ms_dcounter=dclickwait;
    ms_lcounter=linger_for;
  
    mouseimage=mouseimage_;
    mp=moldp=mdiff=0;
    mb=moldb=0;
    msize=mouseimage->size;
    //mouseunderarea=createsurface(msize.x,msize.y,SC.pfd);
    //set the mouse bounds to huge numbers; effectivley bypasses the system.
    
    bool reset=MOUSEreset();

    return reset;    
}

void MOUSEshutdown()
{
    if (mouseimage)
      delete mouseimage;
    if (mouseunderarea)
      delete mouseunderarea;
}

void MOUSEstoprepeat()
{
    while (mb!=0) MOUSEpoll();
}

INT32 MOUSEchkicon(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
    if ((mp.x>=x1) && (mp.y>=y1) && (mp.x<=x1+x2) && (mp.y<=y1+y2))
       return mb;
    else
       return -1;
}

INT32 MOUSEchkouticon(INT16 x1,INT16 y1,INT16 x2,INT16 y2)
{
    if ((mp.x<x1) | (mp.y<y1) | (mp.x>x1+x2) | (mp.y>y1+y2))    
       return mb;
    else
       return -1;
}

void MOUSEstoprepeatwait(UINT16 wait)
{
    /*UINT16 start=*timer,count=0;
    while ((mb!=0) && (count<wait))
    {
        MOUSEpoll();
        count=*timer-start;
    }*/
}

