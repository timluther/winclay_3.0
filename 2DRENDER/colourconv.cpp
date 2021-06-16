#include "graphics.h"

//colour weights
#define rwt 12 
#define gwt 26 
#define bwt 14

UINT32 cc8888_getrgba(tcolour rgba)
{    		
    return rgba;
}       

tcolour cc888_getrgba(tcolour rgba)
{    		
    return rgba & 0x00FFFFFF;
}       


UINT32 cci_getrgba(tcolour rgba)
{    		
	if (IC.palette)
		return IC.palette->rgbadata[rgba & 255].getdword();
	else
		return 0;
}     

UINT32 cc5551_getrgba(tcolour rgba)
{    		
    return ((rgba << 17)& 0xFF000000)|((rgba << 9)& 0x00FF0000)|((rgba << 6)& 0x0000FF00)|((rgba << 3)& 0x000000FF);
}       

UINT32 cc4444_getrgba(tcolour rgba)
{    		
    return ((rgba << 20)& 0xFF000000)|((rgba << 16)& 0x00FF0000)|((rgba << 8)& 0x0000FF00)|((rgba << 4)& 0x000000FF);
}       

UINT32 cc565_getrgba(tcolour rgba)
{    		
    return ((rgba << 8)& 0x00FF0000)|((rgba << 5)& 0x0000FF00)|((rgba << 3)& 0x000000FF);
}       

UINT32 ccgrey16_getrgba(tcolour rgba)
{
	return (rgba >> 8)|(rgba & 0x0000FF00)|((rgba << 8)& 0x00FF0000);
}

tcolour cci_getcolour(UINT32 rgba)
{    		
	UINT32 currcol;  
	INT32 diff,match;
	tcolour retcol;
	retcol=0;
	currcol=0;
	match=32000;
	UINT32 r,g,b;
	r=(rgba >> 16) & 0x000000FF;
	g=(rgba >> 8) & 0x000000FF;
	b=(rgba) & 0x000000FF;
	
	if (!SC.palette) return retcol;  
	trgba *pal=SC.palette->rgbadata;
	while (currcol<SC.palette->size)
	{ 
		if ((pal[currcol].r==r)&&(pal[currcol].g==g)&&(pal[currcol].b==b))
		{
			retcol=currcol;
			return retcol;
		}
		diff=abs(g-pal[currcol].g)*gwt;
		if (diff<match)
		{
			diff+=(abs(r-pal[currcol].r)*rwt+abs(b-pal[currcol].b)*bwt);
			if (diff<match)
			{        
				match=diff;
				retcol=currcol;
			}
		}    
		currcol++;
	}
	return retcol;  
	
}       


tcolour cc8888_getcolour(UINT32 rgba)
{    		
    return rgba;
}       

tcolour cc888_getcolour(UINT32 rgba)
{    		
    return rgba & 0x00FFFFFF;
}       

tcolour cc565_getcolour(UINT32 argb)
{	
	return ((argb >> 8)& 0xf800)| ((argb >> 5 )& 0x7e0)| (( argb >> 3) & 0x1f);	
}

tcolour cc4444_getcolour(UINT32 argb)
{	
	return ((argb>> 16)&0xF000)|((argb>> 12)&0xF00)|((argb>> 8)&0xF0)|((argb>> 4)&0xF);
}

tcolour cc5551_getcolour(UINT32 argb)
{	
	return ((argb>> 16)&0x1000)|((argb>> 9)&0x7C00)|((argb>> 6)&0x3E0)|((argb>> 3)&0x1F);
}

tcolour ccgrey16_getcolour(UINT32 rgba)
{    		
    return ((rgba & 0x000000FF)+((rgba & 0x0000FF00)>>8)+((rgba & 0x00FF0000)>>16))<<6;

}       


//return ((rgba>> 3)& 0x1f)| ((rgba>> 5)& 0x7e0)|((rgba>> 8)& 0xf800);	
//return ((rgba << 8)& 0xf800)| ((rgba >> 5 )& 0x7e0)| (( rgba >> 19) & 0x1f);	


/*
tcolour bgr565_getcolour(UINT32 rgba)
{		
	return ((rgba >> 16)&0xf800)|((rgba >> 13)& 0x7e0)|((rgba>> 11)& 0x1f);
	
}*/

void rgb565_convertscanline(char *out,char *in,UINT32 count,UINT32 pfd)
{
	UINT32 c;
	tcolour *pal=SC.palette->nativedata;
	switch (pfd)
	{
		case PXFi1		:
			while (count)
			{
				c=*(UINT8*)in;
				*((UINT16*)out)=pal[c & 1];
				c>>=1;
				out+=2;
				*((UINT16*)out)=pal[c & 1];
				c>>=1;
				out+=2;
				*((UINT16*)out)=pal[c & 1];
				c>>=1;
				out+=2;
				*((UINT16*)out)=pal[c & 1];
				c>>=1;
				out+=2;
				*((UINT16*)out)=pal[c & 1];
				c>>=1;
				out+=2;
				*((UINT16*)out)=pal[c & 1];
				c>>=1;
				out+=2;
				*((UINT16*)out)=pal[c & 1];
				c>>=1;
				out+=2;
				*((UINT16*)out)=pal[c];				
				out+=2;				
				in++;
				count-=8;				
			}						
			break;
		case PXFi4		:
			while (count)
			{
				c=*(UINT8*)in;
				*((UINT16*)out)=pal[c & 3];
				out+=2;
				*((UINT16*)out)=pal[c >> 3];
				out+=2;				
				in++;
				count-=2;				
			}
			break;
		case PXFpi8	    :
			while (count--)
			{
				*((UINT16*)out)=pal[(*(UINT8*)in) & 0xff];
				in++;
				out+=2;
			}
			break;
		case PXFrgba5551:
			while (count--)
			{
				c=*(UINT16*)in;				
				*((UINT16*)out)=((c << 1)& 0xfFE0)|(c & 0x1f);
				in+=2;
				out+=2;
			};
			break;
		case PXFrgba4444:
			while (count--)
			{
				c=*(UINT16*)in;
				*((UINT16*)out)=((c << 4)& 0xf000)|((c<<3)&0x780)|((c<<1)&0x1f);
				in+=2;
				out+=2;
			};
			break;
		case PXFrgb888  :
		case PXFrgba8888:
			while (count--)
			{
				c=*(UINT32*)in;
				*((UINT16*)out)=((c >> 8)& 0xf800)| ((c >> 5 )& 0x7e0)| ((c >> 3) & 0x1f);
				in+=4;
				out+=2;
			};
			break;
		case PXFrgb565  :memcpy(out,in,count<<1);break;					
	}
}
