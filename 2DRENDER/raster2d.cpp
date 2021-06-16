//low level defines for 2d drawing operations
#include <string.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ttypes.h"
#include "tmaths.h"
#include "gbasics.h"
#include "surface.h"
#include "context.h"
#include "raster2d.h"
#include "font.h"

#define MISC_ADDR 0x3C2
#define MISC_READ_ADDR 0x3CC
#define ATTR_ADDR 0x3c0
#define ATTR_DATA_ADDR 0x3c1
#define SEQU_ADDR 0x3c4
#define SEQU_DATA_ADDR 0x3c5
#define CRTC_ADDR 0x3D4
#define CRTC_DATA_ADDR 0x3d5
#define GRPH_ADDR 0x3CE
#define GRPH_DATA_ADDR 0x3CF
#define STATUS_ADDR 0x3da
#define noclear 128
#define allplanes 3841

//Register masks
#define data_rotate 0x3
#define chainfour 0x8
#define oddeven 0x4
#define exetendedmem 0x2
#define alphaon 0x1

#define normalputsl8 0x00
#define xorputsl8 0x100
#define andputsl8 0x200
#define orputsl8 0x400
#define addputsl8 0x800
#define subputsl8 0x1000
#define blendputsl8 0x2000
#define chromaputsl8 0x4000



////////////////////////////////////////////////////////////////////////////////////////
//Function type declarations


UINT32 t_maxcolours;
tcolour t_fillcol,t_col,t_chroma;
trgba t_colrgba;
trgba *t_palette;
UINT32 t_alpha;


//render modes
//should be pointer...
//for fill patterns

//char maskbyte

UINT8 singlepixelbitmasks[8]={128,64,32,16,8,4,2,1};
UINT8 left_pixelbitmasks[8]={255,127,63,31,15,7,3,1};
UINT8 right_pixelbitmasks[8]={0,128,192,224,240,248,252,254};

//Graphics function pointers
getcolourfunc t_getcolourfunc(UINT32 pfd);
getrgbafunc t_getrgbafunc(UINT32 pfd);

pixmapfunc (*setpixmapmode)(UINT32 mode);
void (*closemode)(void);
void (*putpixel)(PIXINT x,PIXINT y);
tcolour (*getpixel)(PIXINT x,PIXINT y);
getcolourfunc getcolour;
//get rgba code (as integer, see trgba definition) from colour
getrgbafunc getrgba;
void (*convertscanline)(char *out,char *in,UINT32 count,UINT32 pfd);
void (*hline)(PIXINT x1,PIXINT x2,PIXINT y);
void (*vline)(PIXINT x,PIXINT y1,PIXINT y2);
void (*line)(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2);
void (*bar)(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2);
void (*putbitmap_base)(PIXINT x,PIXINT y,PIXINT dw,PIXINT dh,trect *srect,tsurface *pic,UINT32 attrib);
void (*putbitmapfast_base)(PIXINT x,PIXINT y,trect *srect,tsurface *pic,UINT32 attrib);
void (*circle)(PIXINT cx,PIXINT cy,PIXINT radius);
void (*outtextxy_base)(PIXINT cx,PIXINT cy,char *txt,PIXINT slen);
void (*drawbytes)(PIXINT x,PIXINT y,UINT8 w,UINT8 h,char *bytes);
void (*cleardevice)(void);

UINT8 add_table_6bit[128];
UINT8 add_table_5bit[64];
UINT8 add_table_4bit[32];
UINT8 sub_table_6bit[128];
UINT8 sub_table_5bit[64];
UINT8 sub_table_4bit[32];

#define setscanstart1(adrs,bitoffs,x,y,offset)\
adrs=(char*)(offset*y+x<<3);\
bitoffs=x & 7;

//these are used in the linedrawing routines
INT32 addy;
INT32 offs;
INT32 linexdiff,lineydiff;
UINT32 temp32;
UINT32 leadingbyte;

UINT32 t_currentpixmapmode=PXFrgb565;

char *sadrs;
char *padrs;
char *tpadrs;

#define getscanstart8(sadrs,x,y,offset)\
((char*)((offset*y)+x+sadrs))

#define getscanstart16(sadrs,x,y,offset)\
((char*)((offset*y)+x+x+sadrs))

#define getscanstart24(sadrs,x,y,offset)\
((char*)((offset*y)+x+x+x+sadrs))

#define getscanstart32(sadrs,x,y,offset)\
((char*)((offset*y)+(x<<2)+sadrs))

//set to the internal palette pointers inside a surface by the putbitmap* routines
trgba *t_bitmappalette;
tcolour *t_nativebitmappalette;

//include raster implementation sources here
#include "SVGA24.CPP"
#include "SVGA64k.CPP"
#include "SVGA32.CPP"
#include "SVGA256.CPP"
