//imgloadr.cpp generic interface for image loading
#include "graphics.h"
#include "io.h"


trgba lcmaprgb[256];
tcolour lcmap[256];

getcolourfunc (*lastgetcolour)(UINT32 rgba);
getrgbafunc (*lastgetrgba)(tcolour colour);


void buildimagecolourpalettes(tsurface *img,trgba *pal)
{
	if (img->palette)
		img->palette->setentries(0,img->palette->size,pal);	
}


trgba *getimage_rgbpalette()
{
    return lcmaprgb;
}

tcolour *getimage_nativepalette()
{
    return lcmap;
}



tsurface* readimage(trendercontext *ctx,char *filename,tpixelformat pfd,UINT32 attrib)
{
	char *extp=getextension(filename);
	tsurface *retme=NULL;
	if (extp)
	{
		char ext[5];
		strcpy(ext,extp);
		upcase(ext);
		
		FILE *f=fopen(filename,"rb");
		if (f)
		{
			fseek(f,0,SEEK_SET);		
			switch (*(UINT32*)ext)
			{
				case TGAEXT:retme=readtga(ctx,f,pfd,attrib);break;
				case PCXEXT:retme=readpcx(ctx,f,pfd,attrib);break;			
				case JPGEXT:retme=readjpg(ctx,f,pfd,attrib);break;		
				case BMPEXT:retme=readbmp(ctx,f,pfd,attrib);break;
				case IMXEXT:retme=readimx(ctx,f,pfd,attrib);break;
			}
			fclose(f);
		}
	}
	return retme;
}

bool writeimage(char *filename,tsurface *image,UINT32 type)
{
	//char *extp=getextension(filename);
	//if (extp)
	//{
		/*char ext[5];
		strcpy(ext,extp);
		upcase(ext);*/
		FILE *f=fopen(filename,"wb");
		bool retme=false;
		if (f)
		{
			fseek(f,0,SEEK_SET);		
			switch (type)
			{
				//casse TGAEXT:retme=writetga(f,image);			
				//case PCXEXT:retme=writepcx(f,image);			
				//case JPGEXT:retme=writejpg(f,image);			
				//case BMPEXT:retme=writebmp(f,image);
				case IMXEXT:retme=writeimx(f,image);break;
			}
			
		}
		fclose(f);
	//}
	return retme;
}


//09032009219
