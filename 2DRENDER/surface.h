#ifndef surface_h_
#define surface_h_

////PALETTE OBJECT FLAGS//////////////////////////////////////////////////////////

#define PFindirection 1  //palette isn't a palette, it's an indirection table
#define PFlookup      2  //palette has a 64k look up table for fast rgb colour matching (alpha is ignored)
#define PFsystem      4 //palette is the system palette
#define PFnativergba  8 //native palette is rgba too, points to rgbadata palette


////SCREEN DISPLAY INFORMATION//////////////////////////////////////////////////////////

//Note: surfaces may be created by one of several different implementations of rendercontexts
//(see context.cpp and context.h) so they do not contain any constructor or destuctor information
//as the device in question may have it's own way of allocating and deallocating resources
//Therefore, surfaces are created and destroyed using the trendercontext::createsurface and
//trendercontext::releasesurface functions, which are virtual and change depending on the device.

//most significant bits are always alpha

typedef enum
{
	PXFi1		=0,     //1bit indexed
	PXFi4		=1,	 //4bit indexed
	PXFi8		=2,	 //8bit indexed
	PXFpi4		=3,   //4bit indexed plainar
	PXFpi8		=4,   //8bit indexed plainar
	PXFrgb565	=5,   //16bit truecolour
	PXFrgb888	=6,   //24bit truecolour 
	PXFrgba5551 =7,  //15bit truecolour + 1bit alpha //should be argb or bgra
	PXFrgba8888 =8,  //32bit truecolour + 8bit alpha
	PXFrgba4444 =9, //12bit truecolour + 4bit alpha	
	PXFgrey8    =10, //8bit greyscale	
	PXFgrey16   =11, //16bit greyscale
	PXFgrey32   =12, //32bit greyscale
	PXFnoformat =13, //bpp is 8, just non formated data. Height, width and flag formats are invalid
	PXFzbuf16   =14, //16bit zbuffer, rendering should alais to greyscale
	PXFzbuf32   =15, //32bit zbuffer, rendering should alais to greyscale
	PXFbestguess=0xFF,  //Primary surface or bitmap type
	PXFasint    =0xFFFFFFFF //make sure it's 32bit
}tpixelformat;

//surface description flags
#define SDFindexed	    0x1
#define SDFtruecolour   0x2
#define SDFgreyscale	0x4
#define SDFplainar      0x8
#define SDFalphachannel 0x16

//for loading in data
#define IMGLDdontloadpalette 0x10
#define IMGLDdontremap		 0x20  //when loading a picture, the system will try and use 'getcolour' to form a 
                           //native colour palette which in indexed modes causes remaping to take place
                           //if you want to use the orginal palette and just index into it as the file does
                           //then use this flag; Note, it only applies to indexed palettes.
                           //it has no effect in direct colour models

//surface creation flags
#define SFwordaligned		 0x1   //data aligned on word per scanline
#define SFdoublewordaligned  0x2   //data aligned on doubleword per scanline
#define SFownsdata			 0x4   //data allocated by createsurface routine
#define SFprimary			 0x8   //image is primary surface. 
#define SFremaped	         0x10  //image has been remaped to primary surface. Can't recalculate colours
#define SFtexture            0x20  //image is a texture; currently, no effect in the software version but in directx, this indicates that the surface will be created with the DDSCAPS_TEXTURE flag
#define SFvideomem			 0x40  //image is a texture; currently, no effect in the software version but in directx, this indicates that the surface will be created with the DDSCAPS_TEXTURE flag
#define SF3ddevice			 0x80  //the surface created will be used for 3d redering


typedef struct surfacedescriptor_
{
	UINT32 rmask,gmask,bmask,amask,zmask;
	UINT32 rshift,gshift,bshift,ashift,zshift;
	UINT32 rsize,gsize,bsize,asize,zsize;
	UINT32 bpp; //bytes per pixel
	UINT32 bitspp; //bits per pixel
	UINT32 flags;	
	UINT32 palsize;
	UINT32* operator *(){return &rmask;}
} surfacedescriptor;

extern surfacedescriptor surfacedescriptions[14];
extern char *surface_textdescriptions[14];

class tpalette
{
	public:
	UINT32 size;
	UINT32 flags;
	trgba *rgbadata;	  //argb data
	tcolour *nativedata;  //stored in the format of the primary surface. 
	UINT8 *rgblookup;	  //1555 format lookup table to speed up rgb lookups
	void *devdata;		  //pointer to device specific palette info.	

	void setentries(UINT32 start,UINT32 count,trgba *data);
	void getentries(UINT32 start,UINT32 count,trgba *data);
	void setentry(UINT32 idx,trgba data);
	trgba getentry(UINT32 idx);
	void calclookuptable();
};

class tsurface
{
    public:    
    UINT32 driver;  //software or hardware
	tpixelformat pfd;		//pixelformat descriptor
	UINT32 flags;	//flags

	char *scrptr;   //pointer to screen data
	void *devdata;	//device specific pointer, such as a pointer to a ddraw surface
	UINT32 offsd;	//stride of the image, in bytes
	tpoint size;	//size of the image in pixels	
	tpalette *palette;
	trect viewport;     //viewport,all clipping is performed through this    
	tcolour chroma;		//chroma key for blits

	//if it's a directx object, this should call the directx routines to do this



	void setpalette(tpalette *palette);
	void getsurfacedescription(surfacedescriptor *d){*d=surfacedescriptions[pfd];}
	surfacedescriptor *getsurfacedescription(){return &surfacedescriptions[pfd];}

	UINT32 pagesize(){return offsd*size.y;}
	
	UINT32 getbpp(){return surfacedescriptions[pfd].bpp;}
	UINT32 getbitspp(){return surfacedescriptions[pfd].bitspp;}
	UINT32 getredmask(){return surfacedescriptions[pfd].rmask;}
	UINT32 getgreenmask(){return surfacedescriptions[pfd].gmask;}
	UINT32 getbluemask(){return surfacedescriptions[pfd].bmask;}
	UINT32 getalphamask(){return surfacedescriptions[pfd].amask;}
	UINT32 getredshift(){return surfacedescriptions[pfd].rshift;}
	UINT32 getgreenshift(){return surfacedescriptions[pfd].gshift;}
	UINT32 getblueshift(){return surfacedescriptions[pfd].bshift;}
	UINT32 getalphashift(){return surfacedescriptions[pfd].ashift;}
	UINT32 getredsize(){return surfacedescriptions[pfd].rsize;}
	UINT32 getgreensize(){return surfacedescriptions[pfd].gsize;}
	UINT32 getbluesize(){return surfacedescriptions[pfd].bsize;}
	UINT32 getalphasize(){return surfacedescriptions[pfd].asize;}
	UINT32 getpalettesize(){return surfacedescriptions[pfd].palsize;}
	UINT32 getstride(INT32 len){ return (7+(getbitspp()*len))>>3;}
	UINT32 hasalpha(){return (surfacedescriptions[pfd].flags & SDFalphachannel);}
	UINT32 isindexed(){return (surfacedescriptions[pfd].flags & SDFindexed);}
	UINT32 isgreyscale(){return (surfacedescriptions[pfd].flags & SDFgreyscale);}
	char *gettextdescription(){return surface_textdescriptions[pfd];}
	char *getpointerat(INT32 x,INT32 y){return scrptr+((7+(getbitspp()*x))>>3)+(offsd*y);}

	void flipx(trect &rect);
	void flipy(trect &rect);	
	
	void rotate(float angle);
	void rotate90();
};




#endif
