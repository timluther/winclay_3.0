#ifndef _vga256_h_
#define _vga256_h_

//256 colour flat mode graphics library; No 64k pages.
void vga256_close();
char vga256_setmode(screenIDType &sid);
void vga256_putpixel(INT32 x,INT32 y);
tcolour vga256_getpixel(INT32 x,INT32 y);
void vga256_hline(INT32 x1,INT32 x2,INT32 y);
void vga256_vline(INT32 x,INT32 y1,INT32 y2);
void vga256_line(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void vga256_bar(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void vga256_convexpolygon(gpoint *points,UINT16 count);
void vga256_setpolymode(UINT8 mode);
void vga256_putbitmap(INT32 x,INT32 y,tbitmap *pic,char useglobalattribs);
void vga256_circle(INT32 cx,INT32 cy,INT32 radius);
void vga256_outtextxy(INT32 cx,INT32 cy,char *txt,INT32 slen);
void vga256_drawbytes(INT32 x,INT32 y,UINT8 w,UINT8 h,char *bytes);
void vga256_cleardevice(void);
tcolour vga256_getcolour(UINT8 r,UINT8 g,UINT8 b);

#endif
