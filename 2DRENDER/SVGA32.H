#ifndef _svga32_h_
#define _svga32_h_


#include "gbasics.hpp"


//256 colour flat mode graphics library; No 64k pages.
void svga32_close();
char svga32_setmode(screenIDType &sid);
drawscanlinefunc svga32_scanmode(UINT8 mode);
pixmapfunc svga32_pixmapmode(UINT8 mode);

tcolour svga32_getcolour(UINT32 r,UINT32 g,UINT32 b);
tcolour svga32_getpixel(PIXINT x,PIXINT y);
trgba svga32_getrgbpixel(PIXINT x,PIXINT y);
void svga32_putpixel(PIXINT x,PIXINT y);
void svga32_hline(PIXINT x1,PIXINT x2,PIXINT y);
void svga32_vline(PIXINT x,PIXINT y1,PIXINT y2);
void svga32_line(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2);
void svga32_bar(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2);
void svga32_tri(trivert *v0,trivert *v1,trivert *v2);
void svga32_putbitmap(PIXINT x,PIXINT y,tbitmap *pic,char useglobalattrib);
void svga32_getbitmap(PIXINT x,PIXINT y,tbitmap *pic);
void svga32_circle(PIXINT cx,PIXINT cy,PIXINT radius);
void svga32_outtextxy(PIXINT cx,PIXINT cy,char *txt,PIXINT slen);
void svga32_drawbytes(PIXINT x,PIXINT y,UINT8 w,UINT8 h,char *bytes);
void svga32_cleardevice(void);
#endif
