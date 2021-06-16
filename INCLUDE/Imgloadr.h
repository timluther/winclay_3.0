#ifndef IMAGELOADER_H_
#define IMAGELOADER_H_
#include "gbasics.h"

//image file loading routines
//future extentions could include threaded loading

extern trgba lcmaprgb[256];
extern tcolour lcmap[256];

extern tpoint startpos;

extern UINT32 arch_palbase;

#define CTnone 0
#define CTrunlength 1
#define CTlzw 2
#define CTjpg 3
#define CTmixed 4


typedef struct imageinfoheader
{
    UINT32 type; //type of file
    tpoint size;
    trect cliprect;
    UINT8 colourmode;
    UINT16 colourdepth;   //The amount of palette colour available with this mode}            
    UINT8 rsize,gsize,bsize,rpos,gpos,bpos,maskr,maskg,maskb; //for truecolour modes
    char infostr[512];   //could contain possible author information
    char compression; //compression type
    UINT32 flags; //extra flags
}imageinfoheader;

//pcx specific
bool getpcxmetrics(char *filename,UINT32 &w,UINT32 &h,UINT32 &bpp);
tsurface* readpcx(char *filename,UINT32 pfd,UINT32 attrib=BMFdoublewordalign);
void writepcx(char *filename,tsurface *bm);
//void printpcxheader(INT16 x,INT16 y,pcx_header &h);
//bmp specific
bool getbmpmetrics(char *filename,UINT32 &w,UINT32 &h,UINT32 &bpp);
tsurface* readbmp(char *filename,UINT32 pfd,UINT32 attrib=BMFdoublewordalign);
void writebmp(char *filename,tsurface *bm);
//void printbmpheader(INT16 x,INT16 y,BITMAPFILEHEADER &fhs,BITMAPINFO &h);
//gif specific
bool getgifmetrics(char *filename,UINT32 &w,UINT32 &h,UINT32 &bpp);
tsurface* readgif(char *filename,UINT32 pfd,UINT32 attrib=BMFdoublewordalign);
void writegif(char *filename,tsurface *bm);
//void printgifheader(INT16 x,INT16 y,BITMAPFILEHEADER &fhs,BITMAPINFO &h);

bool getarchmetrics(char *filename,UINT32 &w,UINT32 &h,UINT32 &bpp);
tsurface* readarch(char *filename,UINT32 pfd,UINT32 attrib=BMFdoublewordalign);
bool loadarchdata(char *filename,char dopalette);
//generic
//for extracting palette information; returns null if not applicable (i.e. truecolour images)
trgba *getimage_rgbpalette();
tcolour *getimage_nativepalette();
//generic image load; uses the extention as a hint but if the check on the header fails
//it should check the header against other known types before eventually failing
tsurface* readimage(char *filename,UINT8 type,UINT8 drawmode=normalput,UINT8 attrib=BMFdoublewordalign);



#endif
