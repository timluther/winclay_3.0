#define shiftcol 13
#define shiftred 19
#define shiftgreen 18
#define shiftblue 19

//typedef union{    DWORD argb;    struct {        BYTE b,g,r,a;    };}

void svga32_putbmflat32(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset);
void svga32_putbmflat16(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset);
void svga32_putbmflat8(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset);
void svga32_putbmflat8ckey(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset);
void svga32_putbmflat8ckey(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset);
void svga32_putbmflat16(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset);
void svga32_putbmflat16ckey(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset);

#define fillhigh(r)\
_asm mov eax,r \
_asm mov ebx,eax \
_asm shl ebx,16 \
_asm or eax,ebx \
_asm mov r,eax; 

  
      
tcolour svga32_getpixel(PIXINT x,PIXINT y)
{   
   if (!IC.viewport.rcontains(x,y)) return 0;
   return *(tcolour*)getscanstart32(IC.scrptr,x,y,IC.offsd);
}    

void svga32_putpixel(register PIXINT x,register PIXINT y)
{   
   if ((x>=SC.viewport.a.x) && (x<=SC.viewport.b.x) && (y>=SC.viewport.a.y) && (y<=SC.viewport.b.y))   
	   *(UINT32*)getscanstart32(SC.scrptr,x,y,SC.offsd)=t_col;	            
}

#define _hline_32bit(adrs,length,col,op)\
_asm mov edi,adrs \
_asm mov ecx,length \
_asm mov eax,col \
_asm drawloop: \
_asm   op [edi],eax \
_asm   add edi,4 \
_asm   dec ecx \
_asm jnz drawloop;
  
void svga32_hline(PIXINT x1,PIXINT x2,PIXINT y)
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
	sadrs=getscanstart32(SC.scrptr,x1,y,SC.offsd);

	
    _hline_32bit(sadrs,xlen,t_col,mov);      
}

#define _vline_32bit(adrs,offs,cy,col,op)\
_asm mov edi,adrs \
_asm mov edx,offs \
_asm mov ecx,cy \
_asm mov eax,col \
_asm _yloop: \
_asm    op [edi],eax \
_asm    add edi,edx \
_asm    dec ecx \
_asm jnz _yloop;

void svga32_vline(PIXINT x,PIXINT y1,PIXINT y2)
{
    PIXINT ylen;
    if (y1>y2) swap(y2,y1);
    if ((x<SC.viewport.a.x)||(x>SC.viewport.b.x)) return;
    if (y1<SC.viewport.a.y) y1=SC.viewport.a.y;
    if (y2>SC.viewport.b.y) y2=SC.viewport.b.y;
    y2++;
    ylen=(y2-y1);
    if (ylen<=0) return;
    
    sadrs=getscanstart32(SC.scrptr,x,y1,SC.offsd);
    _vline_32bit(sadrs,SC.offsd,ylen,t_col,mov);    
}      

#define xmajorline_32bit(col) \
_asm mov eax,col \
_asm mov edx,addy \
_asm mov ebx,edx \
_asm shr ebx,1 \
_asm mov esi,linexdiff \
_asm mov edi,sadrs \
_asm stosd \
_asm xor ecx,ecx \
_asm xloop1: \
_asm   add bx,dx \
_asm   jnc nodraw \
_asm   rep stosd \
_asm   add edi,offs \
_asm nodraw: \
_asm inc ecx \
_asm dec esi \
_asm jnz xloop1;

#define ymajorline1_32bit(col)\
_asm mov eax,col \
_asm mov edx,addy \
_asm mov ebx,edx \
_asm shr ebx,1 \
_asm mov esi,offs \
_asm mov edi,sadrs \
_asm mov ecx,lineydiff \
_asm yloop1: \
_asm   mov [edi],eax \
_asm   add bx,dx \
_asm   jnc nooverflow1 \
_asm     sub edi,4 \
_asm   nooverflow1: \
_asm   sub edi,esi \
_asm   dec ecx \
_asm jnz yloop1;

#define ymajorline2_32bit(col)\
_asm mov eax,col \
_asm mov edx,addy \
_asm mov ebx,edx \
_asm shr ebx,1 \
_asm mov esi,offs \
_asm mov edi,sadrs \
_asm mov ecx,lineydiff \
_asm yloop2: \
_asm   mov [edi],eax \
_asm   add bx,dx \
_asm   jnc nooverflow2 \
_asm     add edi,4 \
_asm   nooverflow2: \
_asm   add edi,esi \
_asm   dec ecx \
_asm jnz yloop2;

#define vert_or_diagline_32bit(col) \
_asm mov eax,col \
_asm mov esi,offs \
_asm mov edi,sadrs \
_asm mov ecx,linexdiff \
_asm yloop3: \
_asm   mov [edi],eax \
_asm   add edi,esi \
_asm dec ecx \
_asm jnz yloop3;
   
        
void svga32_line(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2)
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
		sadrs=getscanstart32(SC.scrptr,x1,y1,SC.offsd);        
        
        if (y2>y1)
           offs=offsd;
        else
           offs=-offsd;
                           
        addy=(lineydiff << 16)/ linexdiff;
        xmajorline_32bit(t_col);              
    } else if (lineydiff>linexdiff)
    {
        
                      
        if (y1>y2)
        {
            swap(x1,x2);
            swap(y1,y2);
        }
        sadrs=getscanstart32(SC.scrptr,x1,y1,SC.offsd);        
        addy=(linexdiff << 16)/ lineydiff;
     
        if (x1>x2)
        {
            offs=-offsd;
            ymajorline1_32bit(t_col);               
        } else
        {            
            offs=offsd;
            ymajorline2_32bit(t_col);
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
        sadrs=getscanstart32(SC.scrptr,x1,y1,SC.offsd);        
        if (x1==x2)
            offs=SC.offsd;
        else if (x2>x1)
            offs=SC.offsd+4;
        else 
            offs=SC.offsd-4;
        vert_or_diagline_32bit(t_col);                
    }
}
 

//this routine draws a bar onto the screen. It uses doubleword string writes and
//doubleword aligns the data before writting. Thanks for that one Martin!
//maybe the inner loop could be done with the adc rep stosw trick

#define _bar_32bit(adrs,length,cy)\
_asm mov edi,adrs    \
_asm mov edx,length  \
_asm mov esi,cy      \
_asm mov eax,dword ptr t_fillcol \
_asm mov ebx,temp32  \
_asm yloop:          \
_asm   mov ecx,edx   \
_asm   rep stosd     \
_asm   add edi,ebx   \
_asm   dec esi       \
_asm jnz yloop       \

void svga32_bar(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2)
{
    PIXINT xlen;    
    if (y1>y2) swap(y2,y1);
    if (x1>x2) swap(x2,x1);
    
    if ((x1>SC.viewport.b.x)||(x2<SC.viewport.a.x)||
       (y1>SC.viewport.b.y)||(y2<SC.viewport.a.y))return;
        
    if (x1<SC.viewport.a.x) x1=SC.viewport.a.x;
    if (x2>SC.viewport.b.x) x2=SC.viewport.b.x;
    if (y1<SC.viewport.a.y) y1=SC.viewport.a.y;
    if (y2>SC.viewport.b.y) y2=SC.viewport.b.y;
    
	sadrs=getscanstart32(SC.scrptr,x1,y1,SC.offsd);
    
    xlen=1+x2-x1;
    y2-=(y1-1);
    temp32=SC.offsd-(xlen+xlen);
    _bar_32bit(sadrs,xlen,y2);      
}



////////////////////////////////////////////////////////////////////////////////////////////
//Set the drawing mode for the polygons.. only implemented in this driver as of 12/1/99

pixmapfunc svga32_pixmapmode(UINT32 mode)
{
	if (mode!=t_currentpixmapmode)
	{
		t_currentpixmapmode=mode;
		switch(mode)
		{		
			/*case BMflat8:		
			renderpixmap=svga32_putbmflat8;        
			break;    
			case (BMflat8|subputsl8):        
			renderpixmap=svga32_putbmflat8subalpha;
			break;    			
			case (BMflat8|chromaputsl8):        
			renderpixmap=svga32_putbmflat8ckey;
			break;
			case (BMflat8|addputsl8):        
			renderpixmap=svga32_putbmflat8addalpha;
			break;    
			case BMflat16:        
			renderpixmap=svga32_putbmflat16;        
			break;
			case (BMflat16|chromaputsl8):        
			renderpixmap=svga32_putbmflat16ckey;
			break;
			case (BMflat16|addputsl8):        
			renderpixmap=svga32_putbmflat16addalpha;
			break;
			case (BMflat16|subputsl8):        
			renderpixmap=svga32_putbmflat16subalpha;
			break;
			case (BMflat16|blendputsl8):        
			renderpixmap=svga32_putbmflat16blendalpha;
			break;*/
		}
	}
	return renderpixmap;
}
////////////////////////////////////////////////////////////////////////////////////////////
//Bitmap pasting routines

#define _svga32_putbmflat16(xlen,ylen,offset,bmoffset)\
_asm mov ecx,xlen \
_asm mov edx,ylen \
_asm mov ebx,offset \
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


void svga32_putbmflat16(UINT32 xlen,UINT32 ylen,UINT32 offs,UINT32 bmoffset)
{
	_svga32_putbmflat16(xlen,ylen,offs,bmoffset);
}
//see bottom of file for annotated version of the above

//uses native palette
void svga32_putbmflat8(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset)
{
  UINT32 xlop;
  bmoffset-=xlen;
  offset-=(xlen+xlen);  
  while (ylen>0)
  {                  
      for (xlop=0;xlop<xlen;xlop++)
      {   
          *(UINT16*)sadrs=t_nativebitmappalette[*padrs];          
          padrs++;
          sadrs+=2;                               
      }
      padrs+=bmoffset;
      sadrs+=offset;                                 
      ylen--;
  }
}

void svga32_putbmflat32(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset)
{
  UINT32 xlop;
  bmoffset-=(xlen+xlen);
  offset-=(xlen+xlen);  
  while (ylen>0)
  {                  
      for (xlop=0;xlop<xlen;xlop++)
      {   
          *(UINT32*)sadrs=*(UINT32*)padrs;          
          padrs+=4;
          sadrs+=4;                               
      }
      padrs+=bmoffset;
      sadrs+=offset;                                 
      ylen--;
  }
}


void svga32_putbmflat8ckey(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset)
{
  UINT32 xlop;
  bmoffset-=xlen;
  offset-=(xlen<<2);
  UINT32 inchar;  
  while (ylen>0)
  {                  
      for (xlop=0;xlop<xlen;xlop++)
      {
          inchar=t_nativebitmappalette[*padrs];
          if (inchar!=t_chroma)
          {              
              *(UINT32*)sadrs=inchar;
          }                                  
          padrs++;            
		  sadrs+=4;
      }
      padrs+=bmoffset;
      sadrs+=offset;                                 
      ylen--;
  }
}


void svga32_putbmflat16ckey(UINT32 xlen,UINT32 ylen,UINT32 offset,UINT32 bmoffset)
{
  UINT32 xlop;
  bmoffset-=(xlen+xlen);
  offset-=(xlen<<2);  
  UINT32 pixin;
  while (ylen>0)
  {
      for (xlop=0;xlop<xlen;xlop++)
      {
		  //do colour convertion and stippling
          pixin=*(UINT16*)padrs;
          if (pixin!=t_chroma)
             *(UINT32*)sadrs=pixin;
          padrs+=2;
          sadrs+=4;                               
      }
      padrs+=bmoffset;
      sadrs+=offset;
      ylen--;
  }
}

void svga32_putbitmapfast(INT32 x,INT32 y,trect *srect,tsurface *pic,UINT32 attrib)
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
    sadrs=getscanstart32(SC.scrptr,x,y,SC.offsd);
	tpadrs=pic->scrptr;    
    t_nativebitmappalette=pic->palette->nativedata; 
	t_chroma=pic->chroma;
	
	svga32_pixmapmode((pic->pfd << 4)  + attrib);	

    renderpixmap(pic_x,pic_y,xlen,ylen,picoffset);                
}


void svga32_putbitmap(INT32 x,INT32 y,INT32 dw,INT32 dh,trect *srect,tsurface *pic,UINT32 attrib)
{
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
	t_chroma=pic->chroma;
	
	svga32_pixmapmode((pic->pfd << 4)  + attrib);	
	t_nativebitmappalette=pic->palette->nativedata; 
	
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
		sadrs=getscanstart32(SC.scrptr,x,y,SC.offsd);
		renderpixmap(cpic_x,cpic_y,cxlen,cylen,picoffset);		
		
		if (yclip>0)		
		{
			y+=cylen;
			while (y<=endy)
			{	
				sadrs=getscanstart32(SC.scrptr,x,y,SC.offsd);	
				renderpixmap(cpic_x,pic_y,cxlen,pic_h,picoffset);
				y+=pic_h;				
			}		
			sadrs=getscanstart32(SC.scrptr,x,y,SC.offsd);	
			if ((end_ph=(1+y2-y))>0)								
				renderpixmap(cpic_x,pic_y,cxlen,end_ph,picoffset);
		}		
	}else
	{		
		cx=x;		
		sadrs=getscanstart32(SC.scrptr,cx,y,SC.offsd);	
		renderpixmap(cpic_x,cpic_y,cxlen,cylen,picoffset);		
		cx+=cxlen;						
		while (cx<=endx)
		{
			sadrs=getscanstart32(SC.scrptr,cx,y,SC.offsd);	
			renderpixmap(pic_x,cpic_y,pic_w,cylen,picoffset);			
			cx+=pic_w;			
		}						
		if ((end_pw=(1+x2-cx))>0)				
		{		
			sadrs=getscanstart32(SC.scrptr,cx,y,SC.offsd);	
			renderpixmap(pic_x,cpic_y,end_pw,cylen,picoffset);
		}
		if (yclip>0)
		{
			y+=cylen;
			while (y<=endy)
			{			
				cx=x;
				sadrs=getscanstart32(SC.scrptr,cx,y,SC.offsd);	
				renderpixmap(cpic_x,pic_y,cxlen,pic_h,picoffset);
				cx+=cxlen;									
				while (cx<=endx)
				{
					sadrs=getscanstart32(SC.scrptr,cx,y,SC.offsd);	
					renderpixmap(pic_x,pic_y,pic_w,pic_h,picoffset);
					cx+=pic_w;					
				}							
				
				if (end_pw>0)										
				{
					sadrs=getscanstart32(SC.scrptr,cx,y,SC.offsd);	
					renderpixmap(pic_x,pic_y,end_pw,pic_h,picoffset);
				}
				y+=pic_h;
			}

			if ((end_ph=(1+y2-y))>0)	
			{
				cx=x;
				sadrs=getscanstart32(SC.scrptr,cx,y,SC.offsd);	
				renderpixmap(cpic_x,pic_y,cxlen,end_ph,picoffset);					
				cx+=cxlen;									
				while (cx<=endx)
				{
					sadrs=getscanstart32(SC.scrptr,cx,y,SC.offsd);
					renderpixmap(pic_x,pic_y,pic_w,end_ph,picoffset);
					cx+=pic_w;					
				}				
				if (end_pw>0)				
				{		
					sadrs=getscanstart32(SC.scrptr,cx,y,SC.offsd);	
					renderpixmap(pic_x,pic_y,end_pw,end_ph,picoffset);								
				}
			}
		}
	}
}


#define drawbytes_1bit(adrs,data,xlen,ylen,offs,mask,col)\
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
_asm       mov [edi],ebx \
_asm     _nodraw1: \
_asm     test al,64 \
_asm     jz _nodraw2 \
_asm       mov [edi+4],ebx \
_asm     _nodraw2: \
_asm     test al,32 \
_asm     jz _nodraw3 \
_asm       mov [edi+8],ebx \
_asm     _nodraw3: \
_asm     test al,16 \
_asm     jz _nodraw4 \
_asm       mov [edi+12],ebx \
_asm     _nodraw4: \
_asm     test al,8 \
_asm     jz _nodraw5 \
_asm       mov [edi+16],ebx \
_asm     _nodraw5: \
_asm     test al,4 \
_asm     jz _nodraw6 \
_asm       mov [edi+20],ebx \
_asm     _nodraw6: \
_asm     test al,2 \
_asm     jz _nodraw7 \
_asm       mov [edi+24],ebx \
_asm     _nodraw7: \
_asm     test al,1 \
_asm     jz _nodraw8 \
_asm       mov [edi+28],ebx \
_asm     _nodraw8: \
_asm     inc esi \
_asm     add edi,edx \
_asm     dec cl \
_asm   jnz _xloop \
_asm   dec ch \
_asm   jnz _yloop;



#define drawbytes_1bit2(adrs,data,xlen,ylen,offs,mask,col)\
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
_asm     mov [edi],ebx \
_asm     mov [edi+4],ebx \
_asm     mov [edi+8],ebx \
_asm     mov [edi+12],ebx \
_asm     mov [edi+16],ebx \
_asm     mov [edi+20],ebx \
_asm     mov [edi+24],ebx \
_asm     mov [edi+28],ebx \
_asm     inc esi \
_asm     add edi,edx \
_asm     dec cl \
_asm   jnz _xloop \
_asm   dec ch \
_asm   jnz _yloop;
   
void svga32_circle(INT32 cx,INT32 cy,INT32 radius)
{
     trect circrect=trect(cx-radius,cy-radius,cx+radius,cy+radius);
     if (!circrect.rinside(SC.viewport)) return;
     register UINT32 col=t_col;
     UINT32 halfoffsd=SC.offsd >>1;
     INT32 x,y,d,deltaE,deltaSE;
     INT32 yoff;
     INT32 xoff;


     x=xoff=0;
     y=radius;
     yoff=(y*SC.offsd) >> 1;
     d=1-radius;
     deltaE=3;
     deltaSE=5-(radius<<1);
     
     UINT8 *base=(UINT8*)getscanstart32(SC.scrptr,cx,cy,SC.offsd);
     
     /**(UINT16*)(basey+cx+y)=t_col.c16;
     *(UINT16*)(basey+yoff+cx)=t_col.c16;
     *(UINT16*)(basey+yoff-cx)=t_col.c16;
     *(UINT16*)(basey+cx-y)=t_col.c16;
     *(UINT16*)(basey+cx-y)=t_col.c16;
     *(UINT16*)(basey-yoff+cx)=t_col.c16;
     *(UINT16*)(basey-yoff-cx)=t_col.c16;
     *(UINT16*)(basey-cx+y)=t_col.c16;*/
     
     *(UINT32*)( ((UINT32*)base)+y)=col;
     *(UINT32*)( ((UINT32*)base)-y)=col;
     *(UINT32*)( ((UINT32*)base)-y)=col;
     *(UINT32*)( ((UINT32*)base)+y)=col;     
     *(UINT32*)( ((UINT32*)base)+yoff)=col;     
     *(UINT32*)( ((UINT32*)base)-yoff)=col;     

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
         *(UINT32*)( ((UINT32*)base)+xoff+y)=col;
         *(UINT32*)( ((UINT32*)base)+xoff-y)=col;
         *(UINT32*)( ((UINT32*)base)-xoff-y)=col;
         *(UINT32*)( ((UINT32*)base)-xoff+y)=col;
                  
         *(UINT32*)( ((UINT32*)base)+yoff+x)=col;
         *(UINT32*)( ((UINT32*)base)+yoff-x)=col;
         *(UINT32*)( ((UINT32*)base)-yoff-x)=col;
         *(UINT32*)( ((UINT32*)base)-yoff+x)=col;
         
        }
}

void svga32_clearouttextxy(PIXINT cx,PIXINT cy,char *txt,PIXINT slen)
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


void svga32_outtextxy(PIXINT cx,PIXINT cy,char *txt,PIXINT slen)
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

void svga32_drawbytes(PIXINT x,PIXINT y,UINT8 w,UINT8 h,char *databytes)
{
    PIXINT y2,x2;
    
    PIXINT y_clip=0;
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
	

	sadrs=getscanstart32(SC.scrptr,x,y,SC.offsd);   
	fillhigh(t_col);
    drawbytes_1bit(sadrs,databytes,w,h,SC.offsd,mask,t_col);            
}

#define _cleardevice32(screen,pagesize,col)\
_asm mov edi,screen \
_asm mov ecx,pagesize \
_asm mov eax,col \
_asm shr ecx,2 \
_asm rep stosd;
       
void svga32_cleardevice(void)
{
    UINT32 pagesize=SC.pagesize();
    _cleardevice32(SC.scrptr,pagesize,t_fillcol);
}


