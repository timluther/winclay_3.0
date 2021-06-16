#define shiftcol 13
#define shiftred 19
#define shiftgreen 18
#define shiftblue 19

void putbmflat16(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
void putbmflat8(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
void putbmflat8ckey(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
void putbmflat8addalpha(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
//void putbmflat8subalpha(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
void putbmflat16ckey(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
//void putbmflat16addalpha(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
//void putbmflat16subalpha(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
//void putbmflat8blendalpha(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);
//void putbmflat16blendalpha(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset);

pixmapfunc renderpixmap=putbmflat16;

#define fillhigh(r)\
_asm mov eax,r \
_asm mov ebx,eax \
_asm shl ebx,16 \
_asm or eax,ebx \
_asm mov r,eax; 



#define _getcolour(res,r,g,b) \
_asm mov ecx,r \
_asm mov edx,g \
_asm mov eax,b \
_asm shr ax,3 \
_asm shr dx,2 \
_asm shl dx,5 \
_asm or ax,dx \
_asm shr cx,3 \
_asm shl cx,11 \
_asm or ax,cx \
_asm mov cx,ax \
_asm ror eax,16 \
_asm mov ax,cx \
_asm mov res,eax;

//*((unsigned short*)outpix)=((pix <<8)& 0xf800)|((pix >> 5)&0x7e0);//|((pix >>) & 0xf800);//|;//;//;//((pix << 7)& 0x7e0);//|((pix << 13)& 0xf800);	   */

#define _getrgb(res,col) \
_asm mov eax,col \
_asm mov bl,al \
_asm shl ebx,19 \
_asm shr ax,3 \
_asm mov bh,al \
_asm shr ax,5 \
_asm or bl,al \
_asm mov res,ebx;

#define _getcolourint16(res,r,g,b)\
_asm mov ecx,r \
_asm mov edx,g \
_asm mov eax,b \
_asm shr ax,3 \
_asm shr dx,2 \
_asm shl dx,5 \
_asm or ax,dx \
_asm shr cx,3 \
_asm shl cx,11 \
_asm or ax,cx \
_asm mov cx,ax \
_asm ror eax,16 \
_asm mov ax,cx \
_asm mov res,ax;

#define _getcolourint16ptr(res,r,g,b)\
_asm mov ecx,r \
_asm mov edx,g \
_asm mov eax,b \
_asm shr ax,3 \
_asm shr dx,2 \
_asm shl dx,5 \
_asm or ax,dx \
_asm shr cx,3 \
_asm shl cx,11 \
_asm or ax,cx \
_asm mov cx,ax \
_asm ror eax,16 \
_asm mov ax,cx \
_asm mov ebx,res \
_asm mov [ebx],ax;
  
tcolour svga16_getpixel(PIXINT x,PIXINT y)
{   
   if (!IC.viewport.rcontains(x,y)) return 0;
   return *(UINT16*)getscanstart16(IC.scrptr,x,y,IC.offsd);
}    

void svga16_putpixel(register PIXINT x,register PIXINT y)
{   
   if ((x>=SC.viewport.a.x) && (x<=SC.viewport.b.x) && (y>=SC.viewport.a.y) && (y<=SC.viewport.b.y))   
	   *(UINT16*)getscanstart16(SC.scrptr,x,y,SC.offsd)=t_col;	            
}

#define _hline(adrs,length,col,op)\
_asm mov edi,adrs \
_asm mov ecx,length \
_asm mov eax,col \
_asm drawloop: \
_asm   op [edi],ax \
_asm   add edi,2 \
_asm   dec ecx \
_asm jnz drawloop;
  
void svga16_hline(INT32 x1,INT32 x2,INT32 y)
{
    INT32 xlen;    
    if (x1>x2) swap(x2,x1);
    if ((y<SC.viewport.a.y)||(y>SC.viewport.b.y)) return;
    if (x1<SC.viewport.a.x) x1=SC.viewport.a.x;
    if (x2>SC.viewport.b.x) x2=SC.viewport.b.x;
    x2++;	
    xlen=x2-x1;
	fillhigh(t_col);
	
    if (xlen<=0) return;  	
	sadrs=getscanstart16(SC.scrptr,x1,y,SC.offsd);

	
    _hline(sadrs,xlen,t_col,mov);      
}

#define _vline(adrs,offs,cy,col,op)\
_asm mov edi,adrs \
_asm mov edx,offs \
_asm mov ecx,cy \
_asm mov eax,col \
_asm _yloop: \
_asm    op [edi],ax \
_asm    add edi,edx \
_asm    dec ecx \
_asm jnz _yloop;

void svga16_vline(INT32 x,INT32 y1,INT32 y2)
{
    INT32 ylen;
    if (y1>y2) swap(y2,y1);
    if ((x<SC.viewport.a.x)||(x>SC.viewport.b.x)) return;
    if (y1<SC.viewport.a.y) y1=SC.viewport.a.y;
    if (y2>SC.viewport.b.y) y2=SC.viewport.b.y;
    y2++;
    ylen=(y2-y1);
    if (ylen<=0) return;
    
    sadrs=getscanstart16(SC.scrptr,x,y1,SC.offsd);
    _vline(sadrs,SC.offsd,ylen,t_col,mov);    
}      

#define xmajorline(col) \
_asm mov eax,col \
_asm mov edx,addy \
_asm mov ebx,edx \
_asm shr ebx,1 \
_asm mov esi,linexdiff \
_asm mov edi,sadrs \
_asm stosw \
_asm xor ecx,ecx \
_asm xloop1: \
_asm   add bx,dx \
_asm   jnc nodraw \
_asm   rep stosw \
_asm   add edi,offs \
_asm nodraw: \
_asm inc ecx \
_asm dec esi \
_asm jnz xloop1;

#define ymajorline1(col)\
_asm mov eax,col \
_asm mov edx,addy \
_asm mov ebx,edx \
_asm shr ebx,1 \
_asm mov esi,offs \
_asm mov edi,sadrs \
_asm mov ecx,lineydiff \
_asm yloop1: \
_asm   mov [edi],ax \
_asm   add bx,dx \
_asm   jnc nooverflow1 \
_asm     sub edi,2 \
_asm   nooverflow1: \
_asm   sub edi,esi \
_asm   dec ecx \
_asm jnz yloop1;

#define ymajorline2(col)\
_asm mov eax,col \
_asm mov edx,addy \
_asm mov ebx,edx \
_asm shr ebx,1 \
_asm mov esi,offs \
_asm mov edi,sadrs \
_asm mov ecx,lineydiff \
_asm yloop2: \
_asm   mov [edi],ax \
_asm   add bx,dx \
_asm   jnc nooverflow2 \
_asm     add edi,2 \
_asm   nooverflow2: \
_asm   add edi,esi \
_asm   dec ecx \
_asm jnz yloop2;

#define vert_or_diagline(col) \
_asm mov eax,col \
_asm mov esi,offs \
_asm mov edi,sadrs \
_asm mov ecx,linexdiff \
_asm yloop3: \
_asm   mov [edi],ax \
_asm   add edi,esi \
_asm dec ecx \
_asm jnz yloop3;
   
        
void svga16_line(INT32 x1,INT32 y1,INT32 x2,INT32 y2)
{
    
    if (y2==y1) {hline(x1,x2,y1);return;}
    if (!SC.viewport.clipline(x1,y1,x2,y2)) return;
	fillhigh(t_col);
    INT32 offsd=SC.offsd;
    linexdiff=abs(x2-x1);
    lineydiff=abs(y2-y1);
    lineydiff++;
    linexdiff++;      
    if (linexdiff>lineydiff)
    {        
        if (x1>x2)
        {        
            swap(x1,x2);
            swap(y1,y2);
        }    
		sadrs=getscanstart16(SC.scrptr,x1,y1,SC.offsd);        
        
        if (y2>y1)
           offs=offsd;
        else
           offs=-offsd;
                           
        addy=(lineydiff << 16)/ linexdiff;
        xmajorline(t_col);              
    } else if (lineydiff>linexdiff)
    {
        
                      
        if (y1>y2)
        {
            swap(x1,x2);
            swap(y1,y2);
        }
        sadrs=getscanstart16(SC.scrptr,x1,y1,SC.offsd);
        addy=(linexdiff << 16)/ lineydiff;
     
        if (x1>x2)
        {
            offs=-offsd;
            ymajorline1(t_col);               
        } else
        {            
            offs=offsd;
            ymajorline2(t_col);
        }
    }   
    else
    {
    //diagonals +vertical
        if (y1>y2)
        {
            swap(x1,x2);
            swap(y1,y2);
        }
        sadrs=getscanstart16(SC.scrptr,x1,y1,SC.offsd);
        if (x1==x2)
            offs=SC.offsd;
        else if (x2>x1)
            offs=SC.offsd+2;
        else 
            offs=SC.offsd-2;
        vert_or_diagline(t_col);                
    }
}
 

//this routine draws a bar onto the screen. It uses doubleword string writes and
//doubleword aligns the data before writting. Thanks for that one Martin!
//maybe the inner loop could be done with the adc rep stosw trick

#define _bar(adrs,length,cy)\


void svga16_bar(INT32 x1,INT32 y1,INT32 x2,INT32 y2)
{
    INT32 xlen;    
    if (y1>y2) swap(y2,y1);
    if (x1>x2) swap(x2,x1);
    
    if ((x1>SC.viewport.b.x)||(x2<SC.viewport.a.x)||
       (y1>SC.viewport.b.y)||(y2<SC.viewport.a.y))return;
        
    if (x1<SC.viewport.a.x) x1=SC.viewport.a.x;
    if (x2>SC.viewport.b.x) x2=SC.viewport.b.x;
    if (y1<SC.viewport.a.y) y1=SC.viewport.a.y;
    if (y2>SC.viewport.b.y) y2=SC.viewport.b.y;
    
	sadrs=getscanstart16(SC.scrptr,x1,y1,SC.offsd);	    
    xlen=1+x2-x1;
    y2-=(y1-1);
    temp32=SC.offsd-(xlen+xlen);
	fillhigh(t_fillcol);
    _asm
	{
		mov edi,sadrs    
		mov edx,xlen
		mov esi,y2    
		mov eax,dword ptr t_fillcol 
		xor ecx,ecx     
		mov ebx,edi     
		and bx,2        
		shr bl,1        
		sub dx,bx       
		push ebp        
		mov ebp,temp32  
		shr edx,1       
		setc bh         
		yloop:          
			mov cl,bl     
			rep stosw     
			mov cx,dx     
			rep stosd     
			mov cl,bh     
			rep stosw     
			add edi,ebp   
			dec esi       
		jnz yloop       
		pop ebp    
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
//Set the drawing mode for the polygons.. only implemented in this driver as of 12/1/99


pixmapfunc pixfuncs[256]=
{
	//normalput xorput andput orput addput subput blendput chromaput ndef ndef ndef ndef ndef ndef ndef ndef ndef 
	  NULL,	      NULL,  NULL,NULL, NULL,  NULL,	NULL,	  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFi1
	  NULL,       NULL,  NULL,NULL, NULL,  NULL,    NULL,     NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFi4
   	  putbmflat8, NULL,  NULL,NULL, NULL,NULL,NULL,putbmflat8ckey,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFi8
	  NULL,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFpi4
	  NULL,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFpi8
	  putbmflat16, NULL,  NULL,NULL, NULL,NULL,NULL,putbmflat16ckey,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFr5g6b5
	  NULL,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFr5g5b5 	  
	  NULL,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFr5g5b5a1
	  NULL,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFr8g8b8
	  NULL,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFr8g8b8a8
	  NULL,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //PXFr4g4b4a4
	  NULL,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //Not defined
	  NULL,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //Not defined
	  NULL,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //Not defined
	  NULL,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //Not defined
	  NULL,		  NULL,  NULL,NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,  //Not defined
};


pixmapfunc svga16_pixmapmode(UINT32 mode)
{
	if (mode!=t_currentpixmapmode)
	{
		t_currentpixmapmode=mode;
		renderpixmap=pixfuncs[mode];		
		if (!renderpixmap)
			renderpixmap=putbmflat16;
	}		
	return renderpixmap;
}



//draw a triangle. Note that the input variable in trivert are floats to preserve sub-pixel accuracy
//tried it with fixed points all the way through but it always overflows :(
//prehaps I was doing something stupid but floats are fast too these days and they are only used in setup
//I'm now using my own ceil functions and float2fixed conversions; visibly much faster; those watcom ceils
//were really causing problems.

/*calcslopeprecalced(dyred,r,yred,-diffr0_r2);

ic.dyred=   float2int(((v1->r-v0->r)*invheight)     
imul(ic.yred,prestep,ic.dyred));
ic.yred   += float2int(v0->r);*/


/*ic.dxl = float2fixed(((v2->x-v0->x)*invheight));      
ic.xl  = float2fixed(v0->x)+ imul(prestep,ic.dxl);

ic.dxr = float2fixed(((v2->x-v0->x)/height));      
ic.xr=   float2fixed(v0->x)+ imul(prestep,ic.dxr);

ic.dxr = float2fixed(((v1->x-v0->x)/height));      
ic.xr  = float2fixed(v0->x)+ imul(prestep,ic.dxr);      

ic.dxl = float2fixed(((v1->x-v0->x)*invheight));      
ic.xl  = float2fixed(v0->x)+ imul(prestep,ic.dxl);

calcslopecoordinates(dxl,x,xl,v2);	  
*/
////////////////////////////////////////////////////////////////////////////////////////////
//Bitmap pasting routines

#define _putbmflat16(xlen,ylen,bmoffset)\
_asm mov ecx,xlen \
_asm mov edx,ylen \
_asm mov ebx,SC.offsd \
_asm mov eax,bmoffset \
_asm mov esi,padrs \
_asm mov edi,sadrs \
_asm test edi,2 \
_asm setnz byte ptr leadingbyte \
_asm sub ecx,leadingbyte \
_asm mov temp32,ecx \
_asm add ecx,leadingbyte \
_asm add ecx,ecx \
_asm sub ebx,ecx \
_asm sub eax,ecx \
_asm xor ecx,ecx \
_asm yloop: \
_asm   mov cl,byte ptr leadingbyte \
_asm   rep movsw \
_asm   mov ecx,temp32 \
_asm   shr ecx,1 \
_asm   rep movsd \
_asm   adc cl,cl \
_asm   rep movsw \
_asm   add edi,ebx \
_asm   add esi,eax \
_asm dec dx \
_asm jnz yloop;

void putbmflat16(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
{
	//if (xlen<=0 || ylen<=0) return;
	padrs=getscanstart16(tpadrs,x,y,bmoffset);
	_putbmflat16(xlen,ylen,bmoffset);
}
//see bottom of file for annotated version of the above

//uses native palette
void putbmflat8(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
{
  INT32 xlop;
  padrs=getscanstart8(tpadrs,x,y,bmoffset);
  bmoffset-=xlen;
  temp32=SC.offsd-(xlen+xlen);  
  while (ylen>0)
  {                  
      for (xlop=0;xlop<xlen;xlop++)
      {   
          *(UINT16*)sadrs=t_nativebitmappalette[*padrs];          
          padrs++;
          sadrs+=2;                               
      }
      padrs+=bmoffset;
      sadrs+=temp32;                                 
      ylen--;
  }
}

void putbmflat8ckey(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
{
  //outtextxy(0,0,"This is a fookking string");
  INT32 xlop;
  padrs=getscanstart8(tpadrs,x,y,bmoffset);
  bmoffset-=xlen;
  temp32=SC.offsd-(xlen+xlen);
  UINT8 inchar;  
  while (ylen>0)
  {                  
      for (xlop=0;xlop<xlen;xlop++)
      {
          inchar=*padrs;
          if (inchar!=t_chroma)
          {              
              *(UINT16*)sadrs=t_nativebitmappalette[inchar];
          }                        
          sadrs+=2;
          padrs++;            
      }
      padrs+=bmoffset;
      sadrs+=temp32;                                 
      ylen--;
  }
}

//uses 8bpc palette
void putbmflat8addalpha(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
{
   INT32 xlop;
   trgba col;
   trgba scol;
   padrs=getscanstart8(tpadrs,x,y,bmoffset);
   bmoffset-=(xlen);
   temp32=SC.offsd-(xlen+xlen);
   while (ylen>0)
   {                            
        for (xlop=0;xlop<xlen;xlop++)
        {

            /*scol=conv16to24[*(UINT16*)sadrs];
            col=t_bitmappalette[*(UINT8*)padrs];
			scol.r+=col.r;
			scol.g+=col.g;
			scol.b+=col.b;
            
			_getcolourint16ptr(sadrs,r,g,b)\
            *(UINT16*)sadrs=conv24to16.r[scol.r+col.r]|
               conv24to16.g[scol.g+col.g]|               
               conv24to16.b[scol.b+col.b];
            sadrs+=2;        
            padrs++;*/
                                   
        }
        padrs+=bmoffset;
        sadrs+=temp32;                                 
        ylen--;
   }
}

void putbmflat8subalpha(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
{
   INT32 xlop;
   trgba col;
   trgba scol;
   padrs=getscanstart8(tpadrs,x,y,bmoffset);
   char *tsadrs=sadrs;
   bmoffset-=(xlen);
   temp32=SC.offsd-(xlen+xlen);
   while (ylen>0)
   {                            
        for (xlop=0;xlop<xlen;xlop++)
        {
            /*scol=conv16to24[*(UINT16*)tsadrs];     
            col=t_bitmappalette[*(UINT8*)tpadrs];     
            *(UINT16*)sadrs=
               bound0to256_16.r[256+scol.r-col.r]|
               bound0to256_16.g[256+scol.g-col.g]|
               bound0to256_16.b[256+scol.b-col.b];
            padrs++;
            tsadrs+=2;                               */
        }
        padrs+=bmoffset;
        tsadrs+=temp32;                                 
        ylen--;
   }
}


void putbmflat16ckey(INT32 x,INT32 y,INT32 xlen,INT32 ylen,UINT32 bmoffset)
{
  INT32 xlop;
  padrs=getscanstart16(tpadrs,x,y,bmoffset);
  char *lsadrs=sadrs;
  bmoffset-=(xlen+xlen);
  temp32=SC.offsd-(xlen+xlen);  
  UINT16 p;
  t_chroma &=0x0000FFFF;
  while (ylen>0)
  {
      for (xlop=0;xlop<xlen;xlop++)
      {
          p=*(UINT16*)padrs;
          if (p!=t_chroma) //
             *(UINT16*)lsadrs=p;
          padrs+=2;
          lsadrs+=2;                               
      }
      padrs+=bmoffset;
      lsadrs+=temp32;
      ylen--;
  }
}



void svga16_putbitmapfast(INT32 x,INT32 y,trect *srect,tsurface *pic,UINT32 attrib)
{
	INT32 x2,y2,xlen,ylen;     
	INT32 pic_x,pic_y;
	if (!srect) srect=&pic->viewport;
	pic_x=srect->a.x;
	pic_y=srect->a.y;		
    UINT32 picoffset=pic->offsd;	
    x2=x+(srect->b.x-pic_x);//pic->size.x-1;
    y2=y+(srect->b.y-pic_y);//pic->size.y-1;
    
    if (x<SC.viewport.a.x)
    {
        if (x2<SC.viewport.a.x) return;
        pic_x+=SC.viewport.a.x-x; 
        x=SC.viewport.a.x;
    }
    if (y<SC.viewport.a.y)
    {
        if (y2<SC.viewport.a.y)return;        
        pic_y+=SC.viewport.a.y-y; 
        y=SC.viewport.a.y;
    }    
    if (x2>SC.viewport.b.x)
    {
        if (x>SC.viewport.b.x) return;
        x2=SC.viewport.b.x;
    }
    if (y2>SC.viewport.b.y)
    {
        if (y>SC.viewport.b.y) return;        
        y2=SC.viewport.b.y;
    }

    xlen=x2-x+1;
    ylen=y2-y+1;
    if (xlen<=0 || ylen <=0) return;
    sadrs=getscanstart16(SC.scrptr,x,y,SC.offsd);
	tpadrs=pic->scrptr;   
	if (pic->palette)
		t_nativebitmappalette=pic->palette->nativedata; 
	t_chroma=pic->chroma;
	svga16_pixmapmode((pic->pfd << 4)  + attrib);	

    renderpixmap(pic_x,pic_y,xlen,ylen,picoffset);                
}


void svga16_putbitmap(INT32 x,INT32 y,INT32 dw,INT32 dh,trect *srect,tsurface *pic,UINT32 attrib)
{
	if (pic->size.x==1 && pic->size.y==1)
	{
		t_fillcol=*(UINT16*)pic->scrptr;
		bar(x,y,dw,dh);
		return;
	}
    INT32 x2,y2,end_pw,end_ph,cylen,cxlen,cx,yclip,xclip,picoffset;     
	INT32 pic_x,pic_y,pic_w,pic_h,cpic_x,cpic_y;
	pic_x=srect->a.x;
	pic_y=srect->a.y;
	pic_w=1+srect->b.x-pic_x;
	pic_h=1+srect->b.y-pic_y;
    	
    x2=x+dw-1;
    y2=y+dh-1;
    
    if (x<SC.viewport.a.x)
    {
        if (x2<SC.viewport.a.x) return;
        cpic_x=SC.viewport.a.x-x; 
        x=SC.viewport.a.x;
    }else
		cpic_x=0;
    if (y<SC.viewport.a.y)
    {
        if (y2<SC.viewport.a.y)return;        
        cpic_y=SC.viewport.a.y-y; 
        y=SC.viewport.a.y;
    }else
		cpic_y=0; 
    if (x2>SC.viewport.b.x)
    {
        if (x>SC.viewport.b.x) return;
        x2=SC.viewport.b.x;
    }
    if (y2>SC.viewport.b.y)
    {
        if (y>SC.viewport.b.y) return;        
        y2=SC.viewport.b.y;
    }
	

    //xlen=x2-x + 1;ylen=y2-y + 1;
	if ((x2-x)<0 || (y2-y)<0) return;				
	picoffset=pic->offsd;
	tpadrs=pic->scrptr;    
	
	svga16_pixmapmode((pic->pfd << 4)  + attrib);	
	if (pic->palette)
		t_nativebitmappalette=pic->palette->nativedata; 
	t_chroma=pic->chroma;
	cpic_x-=((cpic_x) / pic_w) *pic_w;
	cpic_y-=((cpic_y) / pic_h) *pic_h;
	
	//first row
	INT32 endx=x2-pic_w;
	INT32 endy=y2-pic_h;	

	cylen=pic_h-cpic_y;		
	if ((yclip=(y2-(y+cylen-1)))<0)
		cylen+=yclip;

	cxlen=pic_w-cpic_x;
	cpic_x+=pic_x;
	cpic_y+=pic_y;

	if ((xclip=(x2-(x+cxlen-1)))<=0)
	{
		cxlen+=xclip;
		sadrs=getscanstart16(SC.scrptr,x,y,SC.offsd);
		renderpixmap(cpic_x,cpic_y,cxlen,cylen,picoffset);		
		
		if (yclip>0)		
		{
			y+=cylen;
			while (y<=endy)
			{	
				sadrs=getscanstart16(SC.scrptr,x,y,SC.offsd);	
				renderpixmap(cpic_x,pic_y,cxlen,pic_h,picoffset);
				y+=pic_h;				
			}		
			sadrs=getscanstart16(SC.scrptr,x,y,SC.offsd);	
			if ((end_ph=(1+y2-y))>0)								
				renderpixmap(cpic_x,pic_y,cxlen,end_ph,picoffset);
		}		
	}else
	{		
		cx=x;		
		sadrs=getscanstart16(SC.scrptr,cx,y,SC.offsd);	
		renderpixmap(cpic_x,cpic_y,cxlen,cylen,picoffset);		
		cx+=cxlen;						
		while (cx<=endx)
		{
			sadrs=getscanstart16(SC.scrptr,cx,y,SC.offsd);	
			renderpixmap(pic_x,cpic_y,pic_w,cylen,picoffset);			
			cx+=pic_w;			
		}						
		if ((end_pw=(1+x2-cx))>0)				
		{		
			sadrs=getscanstart16(SC.scrptr,cx,y,SC.offsd);	
			renderpixmap(pic_x,cpic_y,end_pw,cylen,picoffset);
		}
		if (yclip>0)
		{
			y+=cylen;
			while (y<=endy)
			{			
				cx=x;
				sadrs=getscanstart16(SC.scrptr,cx,y,SC.offsd);	
				renderpixmap(cpic_x,pic_y,cxlen,pic_h,picoffset);
				cx+=cxlen;									
				while (cx<=endx)
				{
					sadrs=getscanstart16(SC.scrptr,cx,y,SC.offsd);	
					renderpixmap(pic_x,pic_y,pic_w,pic_h,picoffset);
					cx+=pic_w;					
				}							
				
				if (end_pw>0)										
				{
					sadrs=getscanstart16(SC.scrptr,cx,y,SC.offsd);	
					renderpixmap(pic_x,pic_y,end_pw,pic_h,picoffset);
				}
				y+=pic_h;
			}

			if ((end_ph=(1+y2-y))>0)	
			{
				cx=x;
				sadrs=getscanstart16(SC.scrptr,cx,y,SC.offsd);	
				renderpixmap(cpic_x,pic_y,cxlen,end_ph,picoffset);					
				cx+=cxlen;									
				while (cx<=endx)
				{
					sadrs=getscanstart16(SC.scrptr,cx,y,SC.offsd);
					renderpixmap(pic_x,pic_y,pic_w,end_ph,picoffset);
					cx+=pic_w;					
				}				
				if (end_pw>0)				
				{		
					sadrs=getscanstart16(SC.scrptr,cx,y,SC.offsd);	
					renderpixmap(pic_x,pic_y,end_pw,end_ph,picoffset);								
				}
			}
		}
	}
}


#define drawbytes_1bit32(adrs,data,xlen,ylen,offs,mask,col)\
_asm mov edi,dword ptr adrs \
_asm mov esi,data \
_asm mov cl,byte ptr xlen \
_asm mov ch,byte ptr ylen \
_asm mov edx,dword ptr offs \
_asm mov ah,byte ptr mask \
_asm mov ebx,dword ptr col \
_asm mov byte ptr temp32,cl \
_asm _yloop: \
_asm    mov cl,byte ptr temp32 \
_asm   _xloop: \
_asm     mov al,[esi] \
_asm     and al,ah \
_asm     test al,128 \
_asm     jz _nodraw1 \
_asm       mov [edi],bx \
_asm     _nodraw1: \
_asm     test al,64 \
_asm     jz _nodraw2 \
_asm       mov [edi+2],bx \
_asm     _nodraw2: \
_asm     test al,32 \
_asm     jz _nodraw3 \
_asm       mov [edi+4],bx \
_asm     _nodraw3: \
_asm     test al,16 \
_asm     jz _nodraw4 \
_asm       mov [edi+6],bx \
_asm     _nodraw4: \
_asm     test al,8 \
_asm     jz _nodraw5 \
_asm       mov [edi+8],bx \
_asm     _nodraw5: \
_asm     test al,4 \
_asm     jz _nodraw6 \
_asm       mov [edi+10],bx \
_asm     _nodraw6: \
_asm     test al,2 \
_asm     jz _nodraw7 \
_asm       mov [edi+12],bx \
_asm     _nodraw7: \
_asm     test al,1 \
_asm     jz _nodraw8 \
_asm       mov [edi+14],bx \
_asm     _nodraw8: \
_asm     inc esi \
_asm     add edi,edx \
_asm     dec cl \
_asm   jnz _xloop \
_asm   dec ch \
_asm   jnz _yloop;

#define drawbytes_1bit232(adrs,data,xlen,ylen,offs,mask,col)\
_asm mov edi,dword ptr adrs \
_asm mov esi,dword ptr data \
_asm mov cl,byte ptr xlen \
_asm mov ch,byte ptr ylen \
_asm mov edx,dword ptr offs \
_asm mov byte ptrmask, ah \
_asm mov bx,word ptr col \
_asm mov byte ptr temp32,cl \
_asm _yloop: \
_asm    mov cl,byte ptr temp32 \
_asm    xor edx,edx \
_asm   _xloop: \
_asm     mov al,[esi] \
_asm     and al,ah \
_asm     mov [edi],bx \
_asm     mov [edi+2],bx \
_asm     mov [edi+4],bx \
_asm     mov [edi+6],bx \
_asm     mov [edi+8],bx \
_asm     mov [edi+10],bx \
_asm     mov [edi+12],bx \
_asm     mov [edi+14],bx \
_asm     inc esi \
_asm     add edi,edx \
_asm     dec cl \
_asm   jnz _xloop \
_asm   dec ch \
_asm   jnz _yloop;
   
void svga16_circle(INT32 cx,INT32 cy,INT32 radius)
{
     trect circrect=trect(cx-radius,cy-radius,cx+radius,cy+radius);
     if (!circrect.rinside(SC.viewport)) return;
     register UINT16 col=t_col;
     UINT32 halfoffsd=SC.offsd >>1;
     INT32 x,y,d,deltaE,deltaSE;
     INT32 yoff;
     INT32 xoff;


     x=xoff=0;
     y=radius;
	 	 
     yoff=(y>>1)*SC.offsd;
     d=1-radius;
     deltaE=3;
     deltaSE=5-(radius<<1);
     
     UINT8 *base=(UINT8*)getscanstart16(SC.scrptr,cx,cy,SC.offsd);
     
     *(UINT16*)( ((UINT16*)base)+y)=col;
     *(UINT16*)( ((UINT16*)base)-y)=col;               
     *(UINT16*)( ((UINT16*)base)+yoff)=col;     
     *(UINT16*)( ((UINT16*)base)-yoff)=col;     

     while(y>x)
     {
         if (d<0)
         {
              d+=deltaE;
              deltaE+=2;
              deltaSE+=2;
              x++;
              xoff+=halfoffsd;
         }
         else
         {
              d+=deltaSE;
              deltaE+=2;
              deltaSE+=4;
              x++;
              y--;
              xoff+=halfoffsd;
              yoff-=halfoffsd;
         }         
         *(UINT16*)( ((UINT16*)base)+xoff+y)=col;
         *(UINT16*)( ((UINT16*)base)+xoff-y)=col;
         *(UINT16*)( ((UINT16*)base)-xoff-y)=col;
         *(UINT16*)( ((UINT16*)base)-xoff+y)=col;
                  
         *(UINT16*)( ((UINT16*)base)+yoff+x)=col;
         *(UINT16*)( ((UINT16*)base)+yoff-x)=col;
         *(UINT16*)( ((UINT16*)base)-yoff-x)=col;
         *(UINT16*)( ((UINT16*)base)-yoff+x)=col;
         
        }
}

void svga16_clearouttextxy(INT32 cx,INT32 cy,char *txt,INT32 slen)
{
    //char *adrs;    
	register tfontchar *ch;        

    INT32 schar=0;

    if (slen<0)
       slen=strlen(txt);

    while (cx<SC.viewport.a.x)
    {
        
        cx+=FNTgetcharwidth(*txt);
        txt++;
        schar++;
        
    }
    if (schar!=0)
    {
        schar--;
        txt--;
        cx-=FNTgetcharwidth(*txt);
    }        

    while (cx<=SC.viewport.b.x)
    {
        if (schar>slen) return;
        ch=&t_charset.offsets[*txt];
        drawbytes(cx,cy,ch->bytewidth,ch->height,ch->data);
        cx+=FNTgetcharwidth(ch);
        txt++;
        schar++;
    }        
}


void svga16_outtextxy(INT32 cx,INT32 cy,char *txt,INT32 slen)
{
    //char *adrs;
    register tfontchar *ch;        
    INT32 schar=0;
    if (cx>SC.viewport.b.x) return;
    if (slen<0)
       slen=strlen(txt);

    while (cx<SC.viewport.a.x)
    {        
        cx+=FNTgetcharwidth(*txt);
        txt++;
        schar++;        
    }
    if (schar!=0)
    {
        schar--;
        txt--;
        cx-=FNTgetcharwidth(*txt);
    }        

    while (cx<=SC.viewport.b.x)
    {
        if (schar>slen) return;
        ch=&t_charset.offsets[*txt];
        drawbytes(cx,cy,ch->bytewidth,ch->height,ch->data);
        cx+=FNTgetcharwidth(ch);
        txt++;
        schar++;
    }        
}

void svga16_drawbytes(INT32 x,INT32 y,UINT8 w,UINT8 h,char *databytes)
{
    INT32 y2,x2;
    char *adrs;
    INT32 y_clip=0;
    unsigned char mask;
    
    x2=x+(w<<3);
    y2=y+h;
    if ((y>SC.viewport.b.y)||(y2<=SC.viewport.a.y)||(x>SC.viewport.b.x)||(x2<=SC.viewport.a.x))
      return;

    if (y<SC.viewport.a.y)
    {
        y_clip=abs(SC.viewport.a.y-y);
        y=SC.viewport.a.y;
        databytes+=y_clip*t_charset.xbytesize;
        h-=y_clip;
    }


    if (y2>SC.viewport.b.y)
    {
        y_clip=(y2-SC.viewport.b.y)-1;        
        h-=y_clip;
    }    
    
    mask=0xff;
    if (x<SC.viewport.a.x)
        mask&=(0xff>>(SC.viewport.a.x-x));
    if (x2>SC.viewport.b.x)                                
        mask&=(0xff<<(x2-SC.viewport.b.x-1));

    adrs=getscanstart16(SC.scrptr,x,y,SC.offsd);
	fillhigh(t_col);
    drawbytes_1bit32(adrs,databytes,w,h,SC.offsd,mask,t_col);            
}

#define _cleardevice(screen,pagesize,col)\
_asm mov edi,screen \
_asm mov ecx,pagesize \
_asm mov ax,word ptr col \
_asm mov bx,ax \
_asm shl eax,16 \
_asm mov ax,bx \
_asm shr ecx,2 \
_asm rep stosd;
       
void svga16_cleardevice(void)
{
	UINT32 pagesize=SC.pagesize();
    _cleardevice(SC.scrptr,pagesize,t_fillcol);
}


