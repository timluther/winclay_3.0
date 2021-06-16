#ifndef IMAGELOADER_H_
#define IMAGELOADER_H_
//image file loading routines
//future extentions could include threaded loading

extern trgba lcmaprgb[256];
extern tcolour lcmap[256];


#define TGAEXT 0x4147542E  //tga
#define PCXEXT 0x5843502e  //pcx
#define JPGEXT 0x47504A2e  //jpg
#define BMPEXT 0x504D422E  //bmp
#define LBMEXT 0x4D424C2E  //lbm
#define IFFEXT 0x4646492E  //iff
#define GIFEXT 0x4649472E  //gif
#define IMXEXT 0x584D492E  //imx


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
bool getpcxmetrics(FILE *f,UINT32 &w,UINT32 &h,UINT32 &bpp,tpixelformat &suggestpfd);
tsurface* readpcx(trendercontext *ctx,FILE *f,tpixelformat pfd,UINT32 attrib=SFdoublewordaligned);
bool writepcx(FILE *f,tsurface *bm);
//void printpcxheader(INT16 x,INT16 y,pcx_header &h);
//bmp specific
bool getbmpmetrics(FILE *f,UINT32 &w,UINT32 &h,UINT32 &bpp,tpixelformat &suggestpfd);
tsurface* readbmp(trendercontext *ctx,FILE *f,tpixelformat pfd,UINT32 attrib=SFdoublewordaligned);
bool writebmp(FILE *f,tsurface *bm);
//void printbmpheader(INT16 x,INT16 y,BITMAPFILEHEADER &fhs,BITMAPINFO &h);
//gif specific
bool getgifmetrics(FILE *f,UINT32 &w,UINT32 &h,UINT32 &bpp,tpixelformat &suggestpfd);
tsurface* readgif(trendercontext *ctx,FILE *f,tpixelformat pfd,UINT32 attrib=SFdoublewordaligned);
bool writegif(FILE *f,tsurface *bm);
//void printgifheader(INT16 x,INT16 y,BITMAPFILEHEADER &fhs,BITMAPINFO &h);
bool getarchmetrics(FILE *f,UINT32 &w,UINT32 &h,UINT32 &bpp,tpixelformat &suggestpfd);
tsurface* readarch(trendercontext *ctx,FILE *f,tpixelformat pfd,UINT32 attrib=SFdoublewordaligned);
bool loadarchdata(FILE *f,char dopalette);
//tga
bool gettgametrics(FILE *f,UINT32 &w,UINT32 &h,UINT32 &bpp,tpixelformat &suggestpfd);
tsurface* readtga(trendercontext *ctx,FILE *f,tpixelformat pfd,UINT32 attrib=SFdoublewordaligned);
bool writetga(FILE *f,tsurface *bm);
//jpg
bool getjpgmetrics(FILE *f,UINT32 &w,UINT32 &h,UINT32 &bpp,tpixelformat &suggestpfd);
tsurface* readjpg(trendercontext *ctx,FILE *f,tpixelformat pfd,UINT32 attrib=SFdoublewordaligned);
bool writejpg(FILE *f,tsurface *bm);
//imx
bool getimxmetrics(FILE *f,UINT32 &w,UINT32 &h,UINT32 &bpp,tpixelformat &suggestpfd);
tsurface* readimx(trendercontext *ctx,FILE *f,tpixelformat pfd,UINT32 attrib=SFdoublewordaligned);
bool writeimx(FILE *f,tsurface *bm);
int calcimxfilesize(tsurface *p);
//generic
//for extracting palette information; returns null if not applicable (i.e. truecolour images)
trgba *getimage_rgbpalette();
tcolour *getimage_nativepalette();
//generic image load; uses the extention as a hint but if the check on the header fails
//it should check the header against other known types before eventually failing
tsurface* readimage(trendercontext *ctx,char *filename,tpixelformat pfd,UINT32 attrib=SFdoublewordaligned);

//stores the colour encoding and decoding routines 
extern getcolourfunc (*lastgetcolour)(UINT32 rgba);
extern getrgbafunc (*lastgetrgba)(tcolour colour);
void buildimagecolourpalettes(tsurface *img,trgba *pal);
bool writeimage(char *filename,tsurface *image,UINT32 type);
#endif
