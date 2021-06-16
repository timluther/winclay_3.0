//graphics primatives header file
//these are device independent functions that use the device dependent primatives
//defined in ggraph.hpp
//They are not classed by type, rather they are loosley connected useful graphics functions

#ifndef GPRIM_H
#define GPRIM_H

#define nodesize 2
#define nodesizedec1 nodesize-1
#define border 4
#define gadgetsize 6
#define halfgadget gadgetsize/2

extern tcolour highlight; //for icons e.t.c.//
extern tcolour textcol;   //text colour//
extern tcolour titletextcol; //for window titles//
extern tcolour scrollcol; //scroller background//
extern tcolour lightcol;  //highlight colour//
extern tcolour midcol;    //middle colour//
extern tcolour mid_darkcol;   //shadow colour//
extern tcolour mid_darkcol2;   //shadow colour//
extern tcolour darkcol;   //shadow colour//
extern tcolour backcol;   //background colour//
extern tcolour gridcol;    //grid colour//
extern tcolour selectcol;  //selection colour//
extern tcolour titlecol;   //titlebar colour//
extern tcolour disabledcol;
extern tcolour menuselcolback; 
extern tcolour menuselcolfore;
extern tcolour scrollhighlight;
extern tcolour textboxcol;

extern UINT8 circlepic[8];
extern UINT8 tickpic[8];
extern UINT8 closegadget[8];
extern UINT8 uparrow[8];
extern UINT8 downarrow[8];
extern UINT8 resizegadget[8];
extern UINT8 leftarrow[8];
extern UINT8 rightarrow[8];
extern UINT8 updownarrow[8];
extern trgbabase greypal[16];


void drawarrow(flt x1,flt y1,flt x2,flt y2,flt head,flt tail,flt width);
void captilebitmapy(INT32 x,INT32 y,INT32 y2,tsurface **bm,UINT32 useattrib=0);
void captilebitmapx(INT32 x,INT32 y,INT32 x2,tsurface **bm,UINT32 useattrib=0);
void captilebitmapy(INT32 x,INT32 y,INT32 y2,tsurface *bm1,tsurface *bm2,tsurface *bm3);
void captilebitmapx(INT32 x,INT32 y,INT32 x2,tsurface *bm1,tsurface *bm2,tsurface *bm3);
void tilebitmapx(INT32 x,INT32 y,INT32 x2,tsurface *bm,UINT32 useattrib=0);
void tilebitmapy(INT32 x,INT32 y,INT32 y2,tsurface *bm,UINT32 useattrib=0);
void outtextxy3d(INT32 x,INT32 y,char *txt);
void outtextxyu(INT32 x,INT32 y,char *txt,char underchar);
void outtextxyu3d(INT32 x,INT32 y,char *txt,char underchar);
void drawx(INT32 x,INT32 y);
void drawcursor(INT32 x,INT32 y);
void grid(INT32 x,INT32 y,INT32 gxsize,INT32 gysize,INT32 stpx,INT32 stpy);
void dotgrid(INT32 x,INT32 y,INT32 gxsize,INT32 gysize,INT32 stpx,INT32 stpy);
void boxgrid(INT32 x,INT32 y,INT32 gxsize,INT32 gysize,INT32 stpx,INT32 stpy);
void colourgrid2(INT32 x,INT32 y,INT32 gxsize,INT32 gysize,INT32 stpx,INT32 stpy);
void colourgrid(INT32 x,INT32 y,INT32 gxsize,INT32 gysize,INT32 stpx,INT32 stpy);
void outbox(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void outboxcol(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void inbox(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void inboxcol(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void inboxcolb(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void insetborder(INT32 x1,INT32 y1,INT32 x2,INT32 y2,char *txt);
void textcurs(INT32 x,INT32 y,INT32 y2);
void textsel(INT32 x,INT32 y,INT32 x2);
void textbox(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void textboxfilled(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void drawnode(INT32 x,INT32 y);
void hline3d(INT32 x1,INT32 x2,INT32 y1);
void draw3dborder(INT32 x1,INT32 y1,INT32 x2,INT32 y2,char inv);
void rectangle_rotated(INT32 x1,INT32 y1,INT32 x2,INT32 y2,INT32 cx,INT32 cy,INT32 angle); 
void thickrectangle(INT32 x1,INT32 y1,INT32 x2,INT32 y2,INT32 thickness);
void rectangle(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void rectangle(trect &b);
void drawbound(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void drawarrow(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
INT32 lineangle(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void drawborder(INT32 x1,INT32 y1,INT32 x2,INT32 y2,UINT8 borderwidth);
void label2(INT32 x,INT32 y,char *txt);
void label(INT32 x,INT32 y,char *txt);

inline void linef(flt x1,flt y1,flt x2,flt y2)
{
  line(float2int(x1),float2int(y1),float2int(x2),float2int(y2));  
}

inline void rectanglef(flt x1,flt y1,flt x2,flt y2)
{  
   rectangle(float2int(x1),float2int(y1),float2int(x2),float2int(y2));  
}



#endif
