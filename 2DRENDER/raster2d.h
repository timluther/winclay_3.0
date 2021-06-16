#ifndef imagepacker_h_
#define imagepacker_h_


////raster ops 
#define normalput 0x00
#define xorput 0x01
#define andput 0x02
#define orput 0x03
#define addput 0x4
#define subput 0x5
#define blendput 0x6
#define chromaput 0x7

typedef tcolour (*getpixelfunc)(INT32 x,INT32 y);
typedef void (*putpixelfunc)(INT32 x,INT32 y);
typedef UINT32  (*getrgbafunc)   (tcolour colour);
typedef tcolour (*getcolourfunc) (UINT32 trgba);
typedef void (*pixmapfunc) (INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
getcolourfunc t_getcolourfunc(tpixelformat pfd);
getrgbafunc t_getrgbafunc(tpixelformat pfd);

////////////////////////////////////////////////////////////////////
//Global functions

extern getpixelfunc getpixel;
extern putpixelfunc putpixel;
extern pixmapfunc (*setpixmapmode)(UINT32 mode);
//get colour from rgba code (as integer, see trgba definition)
extern getcolourfunc getcolour;
//get rgba code (as integer, see trgba definition) from colour
extern getrgbafunc getrgba;

extern void (*hline)(INT32 x1,INT32 x2,INT32 y);
extern void (*vline)(INT32 x,INT32 y1,INT32 y2);
extern void (*line)(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
extern void (*bar)(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
extern void (*putbitmap_base)(INT32 x,INT32 y,INT32 dw,INT32 dh,trect *srect,tsurface *pic,UINT32 attrib);
extern void (*putbitmapfast_base)(INT32 x,INT32 y,trect *srect,tsurface *pic,UINT32 attrib);
extern void (*circle)(INT32 cx,INT32 cy,INT32 radius);
extern void (*outtextxy_base)(INT32 cx,INT32 cy,char *txt,INT32 length);
extern void (*drawbytes)(INT32 x,INT32 y,UINT8 w,UINT8 h,char *bytes);
extern void (*cleardevice)(void);


extern bool GFX_INITIALIZED;

//screen disply types

//The two main diplay contexts, output and input.
//Current Colours & writemode 
extern UINT32 t_maxcolours;
extern tcolour t_fillcol,t_col;
extern UINT32 t_linepattern;
extern trgba t_colrgba;
extern UINT32 t_alpha;
extern UINT32 t_currentpixmapmode;
extern UINT32 t_writemode;

void waitvbl();

inline void putbitmap(INT32 x,INT32 y,tsurface *pic,UINT32 attrib=0)
{
    putbitmapfast_base(x,y,NULL,pic,attrib);
}

inline void putbitmap(INT32 x,INT32 y,trect *rect,tsurface *pic,UINT32 attrib=0)
{
    putbitmapfast_base(x,y,rect,pic,attrib);
}


inline void putbitmap(INT32 x,INT32 y,INT32 dw,INT32 dh,trect *srect,tsurface *pic,UINT32 attrib=0)
{
    putbitmap_base(x,y,dw,dh,srect,pic,attrib);
}

inline void putbitmap(INT32 px,INT32 py,INT32 x1,INT32 y1,INT32 x2,INT32 y2,trect *srect,tsurface *pic,UINT32 attrib=0)
{
	trect temp=SC.viewport;
	SC.viewport.rintersect(x1,y1,x2,y2);
    putbitmap_base(px,py,SC.size.x,SC.size.y,srect,pic,attrib);
	SC.viewport=temp;
}

inline void outtextxy(INT32 cx,INT32 cy,char *txt)
{
    outtextxy_base(cx,cy,txt,-1);
}

inline void outtextxy(INT32 cx,INT32 cy,char *txt,INT16 len)
{
    outtextxy_base(cx,cy,txt,len);
}    

inline tcolour getcolour2(UINT8 r,UINT8 g,UINT8 b)
{
	return getcolour(BUILDARGB(0,r,g,b));
}
#endif