#ifndef _svga16_h_
#define _svga16_h_

tcolour cc8888_getcolour(UINT32 rgba);
tcolour cc888_getcolour(UINT32 rgba);
tcolour cc5551_getcolour(UINT32 rgba);
tcolour cc4444_getcolour(UINT32 rgba);
tcolour cc565_getcolour(UINT32 rgba);
tcolour cci_getcolour(UINT32 rgba);
tcolour ccgrey16_getcolour(UINT32 rgba);

UINT32 cc8888_getrgba(tcolour rgba);
UINT32 cc888_getrgba(tcolour rgba);
UINT32 cc5551_getrgba(tcolour rgba);
UINT32 cc4444_getrgba(tcolour rgba);
UINT32 cc565_getrgba(tcolour rgba);
UINT32 cci_getrgba(tcolour rgba);
UINT32 ccgrey16_getrgba(tcolour rgba);


pixmapfunc svga16_pixmapmode(UINT32 mode);
tcolour svga16_getpixel(INT32 x,INT32 y);
void svga16_putpixel(INT32 x,INT32 y);
void svga16_hline(INT32 x1,INT32 x2,INT32 y);
void svga16_vline(INT32 x,INT32 y1,INT32 y2);
void svga16_line(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void svga16_bar(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void svga16_putbitmap(INT32 x,INT32 y,INT32 dw,INT32 dh,trect *srect,tsurface *pic,UINT32 attrib);
void svga16_putbitmapfast(INT32 x,INT32 y,trect *srect,tsurface *pic,UINT32 attrib);
void svga16_circle(INT32 cx,INT32 cy,INT32 radius);
void svga16_outtextxy(INT32 cx,INT32 cy,char *txt,INT32 slen);
void svga16_drawbytes(INT32 x,INT32 y,UINT8 w,UINT8 h,char *bytes);
void svga16_cleardevice(void);

pixmapfunc svga24_pixmapmode(UINT32 mode);
tcolour svga24_getpixel(INT32 x,INT32 y);
void svga24_putpixel(INT32 x,INT32 y);
void svga24_hline(INT32 x1,INT32 x2,INT32 y);
void svga24_vline(INT32 x,INT32 y1,INT32 y2);
void svga24_line(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void svga24_bar(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void svga24_putbitmap(INT32 x,INT32 y,INT32 dw,INT32 dh,trect *srect,tsurface *pic,UINT32 attrib);
void svga24_putbitmapfast(INT32 x,INT32 y,trect *srect,tsurface *pic,UINT32 attrib);
void svga24_circle(INT32 cx,INT32 cy,INT32 radius);
void svga24_outtextxy(INT32 cx,INT32 cy,char *txt,INT32 slen);
void svga24_drawbytes(INT32 x,INT32 y,UINT8 w,UINT8 h,char *bytes);
void svga24_cleardevice(void);


pixmapfunc svga32_pixmapmode(UINT32 mode);
tcolour svga32_getpixel(INT32 x,INT32 y);
void svga32_inputputpixel(INT32 x,INT32 y);
void svga32_putpixel(INT32 x,INT32 y);
void svga32_hline(INT32 x1,INT32 x2,INT32 y);
void svga32_vline(INT32 x,INT32 y1,INT32 y2);
void svga32_line(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void svga32_bar(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void svga32_putbitmap(INT32 x,INT32 y,INT32 dw,INT32 dh,trect *srect,tsurface *pic,UINT32 attrib);
void svga32_putbitmapfast(INT32 x,INT32 y,trect *srect,tsurface *pic,UINT32 attrib);
void svga32_circle(INT32 cx,INT32 cy,INT32 radius);
void svga32_outtextxy(INT32 cx,INT32 cy,char *txt,INT32 slen);
void svga32_drawbytes(INT32 x,INT32 y,UINT8 w,UINT8 h,char *bytes);
void svga32_cleardevice(void);

pixmapfunc svgai8_pixmapmode(UINT32 mode);
tcolour svgai8_getpixel(INT32 x,INT32 y);
void svgai8_putpixel(INT32 x,INT32 y);
void svgai8_hline(INT32 x1,INT32 x2,INT32 y);
void svgai8_vline(INT32 x,INT32 y1,INT32 y2);
void svgai8_line(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void svgai8_bar(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void svgai8_putbitmap(INT32 x,INT32 y,INT32 dw,INT32 dh,trect *srect,tsurface *pic,UINT32 attrib);
void svgai8_putbitmapfast(INT32 x,INT32 y,trect *srect,tsurface *pic,UINT32 attrib);
void svgai8_circle(INT32 cx,INT32 cy,INT32 radius);
void svgai8_outtextxy(INT32 cx,INT32 cy,char *txt,INT32 slen);
void svgai8_drawbytes(INT32 x,INT32 y,UINT8 w,UINT8 h,char *bytes);
void svgai8_cleardevice(void);

#endif
