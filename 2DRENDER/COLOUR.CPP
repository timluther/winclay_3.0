//WATCOM C VGA colour library
#include <stdlib.h>
#include <math.h>
#include "ttypes.h"
#include "tmaths.h"
#include "gbasics.h"
#include "surface.h"
#include "context.h"
#include "raster2d.h"

//colour weights
#define rwt 12 
#define gwt 26 
#define bwt 14



void _setrgb(UINT8 colno,UINT8 r,UINT8 g,UINT8 b)
{
	_asm
    {
		mov dx,3c8h
		out dx,al
		inc dx
		mov al,ah
		out dx,al
		mov al,bl
		out dx,al
		mov al,bh
		out dx,al
    }
}


void COLsetrgb(UINT8 colno,UINT8 red,UINT8 green,UINT8 blue)
{
    _setrgb(colno,red,green,blue);  
}


UINT32 COLfindclosest(trgba c,tpalette *pal)
{
	UINT32 currcol;
	UINT32 matchindex;
	bool colfound;
	INT32 diff,match;
	trgba *ccol,*end;
	ccol=pal->rgbadata;
	end=&ccol[pal->size];
	currcol=0;
	match=0x0FFFFFF;
	colfound=FALSE;
	
	while (ccol<end)
	{ 
		if (c==*ccol)
			return currcol;
		
		diff=c.g-ccol->g*gwt;
		if (diff<match)
		{
			diff+=(c.r-ccol->r*rwt + c.b-ccol->b);
			if (diff<match)
			{        
				match=diff;
				matchindex=currcol;
			}
		}		
		currcol++;
		ccol++;
	}
	return matchindex;
	
	
}


void COLgethardwarepal(trgba *pal)
{
	_asm
	{
		mov esi,pal
		mov cl,256
		mov dx,0x3C7
		mov al,0
		out dx,al
		mov dx,0x3C9
		inc dx 
	_colloop:
		in al,dx
		shl al,2
		mov [esi],al
		in al,dx
		shl al,2 
		mov [esi+1],al
		in al,dx
		shl al,2 
		mov [esi+2],al
		add edi,4
		dec cx
		jnz _colloop
	}    
}

void COLsethardwarepal(trgba *pal,UINT16 start, UINT16 len)
{
	//pal+=start;
	_asm
	{
		mov esi,pal
		mov bx,start
		mov cx,len
		inc cx
		sub cx,bx
		mov dx,0x3C6
		mov al,255
		out dx,al
		inc dx
			
_colloop:
		mov dx,0x3C8
		mov al,bl
		out dx,al
		inc dx
		mov al,[esi]
		shr al,2
		out dx,al 
		mov al,[esi+1]
		shr al,2
		out dx,al 
		mov al,[esi+2]
		shr al,2
		out dx,al 
		add esi,4
		inc bx
		dec cx
		jnz _colloop
	}
}   


void COLsethsv(trgba *col,float h,float s,float v)
{
	float r,g,b,f,q,p,t;  int i;
	h*=6;
	s/=255;
	v/=255;
	if (s==0)
	{
		r=v;g=v;b=v;
	}else
	{
		if (h>=1530) 
			h=0;
		h/=255;
		i=(INT16)h;
		f=h-i;
		p=v*(1-s);
		q=v*(1-(s*f));
		t=v*(1-(s*(1-f)));
		//with 1/6 is Hue in?}
		switch (i)
		{
			case 0:r=v;g=t;b=p;break;
			case 1:r=q;g=v;b=p;break;
			case 2:r=p;g=v;b=t;break;
			case 3:r=p;g=q;b=v;break;
			case 4:r=t;g=p;b=v;break;
			case 5:r=v;g=p;b=q;break;
		}  
	}
	*col=trgba((INT32)(r*255),(INT32)(g*255),(INT32)(b*255),0);
}

#define inv256 0.003921568627451f

void COLgethsv(trgba *col,UINT8 &hi,UINT8 &si,UINT8 &vi)
{
	float max,min,diff,r_dist,g_dist,b_dist,r,g,b,h,s,v;;		
	
	r=col->r*inv256; g=col->g*inv256; b=col->b*inv256;
	if (r>g) max=r; else max=g;
	if (b>max) max=b;
	if (r<g) min=r; else min=g;
	if (b<min) min=b;
	diff=max-min;
	v=max;	
	if (!max)	
		s=h=0.0f;	
	else 
	{
		s=diff/max;
		diff=1.0f/diff;
		r_dist=(max - r)*diff;
		g_dist=(max - g)*diff;
		b_dist=(max - b)*diff;
		if (r==max) 
			h=b_dist-g_dist;
		else if (g==max)
			h=2+r_dist-b_dist;
		else if (b==max) 
			h=4+g_dist-r_dist;
		h=h * 255;
		if (h<0) h+=1530;
	}
	hi=(UINT8)(h / 255);
	si=(UINT8)(s * 255);
	vi=(UINT8)(v * 255);
}


void COLgrad(trgba *pal,UINT32 col1,UINT32 col2)
{
#define colshift 8
	INT32 rplus,bplus,gplus,aplus,rp,gp,bp,ap,step;  
	if (col2<col1) swap(col1,col2);
	step=1+(col2-col1);
	
	rplus=((pal[col2].r-pal[col1].r)<< colshift)/ step;
	gplus=((pal[col2].g-pal[col1].g)<< colshift)/ step;
	bplus=((pal[col2].b-pal[col1].b)<< colshift)/ step;
	aplus=((pal[col2].a-pal[col1].a)<< colshift)/ step;
    
	rp=(pal[col1].r<<colshift);
	gp=(pal[col1].g<<colshift);
	bp=(pal[col1].b<<colshift);
	ap=(pal[col1].a<<colshift);  
	for (;col1<col2;col1++)
	{
		pal[col1].r=rp>>colshift;
		pal[col1].g=gp>>colshift;
		pal[col1].b=bp>>colshift;
		pal[col1].a=ap>>colshift;                        
		rp+=rplus; gp+=gplus; bp+=bplus;ap+=aplus;
	}
	
}

