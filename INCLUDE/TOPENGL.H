#ifndef _opengl_h_
#define _opengl_h_


#include "gbasics.h"

void opengl_close();
//char opengl_setmode(screenIDType &sid);
tcolour opengl_getcolour(UINT32 r,UINT32 g,UINT32 b);
tcolour opengl_getpixel(PIXINT x,PIXINT y);
trgba opengl_getrgbpixel(PIXINT x,PIXINT y);
void opengl_putpixel(PIXINT x,PIXINT y);
void opengl_hline(PIXINT x1,PIXINT x2,PIXINT y);
void opengl_vline(PIXINT x,PIXINT y1,PIXINT y2);
void opengl_line(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2);
void opengl_bar(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2);
void opengl_scanmode(UINT8 mode);
void opengl_tri(trivert *v0,trivert *v1,trivert *v2);
void opengl_putbitmap(PIXINT x,PIXINT y,PIXINT sx,PIXINT sy,tsurface *pic,UINT32 attrib);
void opengl_getbitmap(PIXINT x,PIXINT y,tsurface *pic);
void opengl_circle(PIXINT cx,PIXINT cy,PIXINT radius);
void opengl_outtextxy(PIXINT cx,PIXINT cy,char *txt,PIXINT slen);
void opengl_drawbytes(PIXINT x,PIXINT y,UINT8 w,UINT8 h,char *bytes);
void opengl_cleardevice(void);
#endif
