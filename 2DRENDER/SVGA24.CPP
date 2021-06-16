//typedef union{    DWORD argb;    struct {        BYTE b,g,r,a;    };}

#define fillhigh(r)\
_asm mov eax,r \
_asm mov ebx,eax \
_asm shl ebx,16 \
_asm or eax,ebx \
_asm mov r,eax; 
    
tcolour svga24_getpixel(PIXINT x,PIXINT y)
{   
   if (!IC.viewport.rcontains(x,y)) return 0;
   return *(tcolour*)getscanstart24(IC.scrptr,x,y,IC.offsd) & 0x00FFFFFF;
}    

void svga24_putpixel(register PIXINT x,register PIXINT y)
{   
   if ((x>=SC.viewport.a.x) && (x<=SC.viewport.b.x) && (y>=SC.viewport.a.y) && (y<=SC.viewport.b.y))   
   {
	   UINT32 *sadrs=(UINT32*)getscanstart24(SC.scrptr,x,y,SC.offsd);
	   *sadrs= (*sadrs & 0xFF000000)|(t_col & 0x00FFFFFF);	         
   }
}


#define _hline_24bit(adrs,length,col,op)\
_asm mov edi,adrs \
_asm mov ecx,length \
_asm mov eax,col \
_asm drawloop: \
_asm   op [edi],eax \
_asm   add edi,4 \
_asm   dec ecx \
_asm jnz drawloop;
  
void svga24_hline(PIXINT x1,PIXINT x2,PIXINT y)
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
	sadrs=getscanstart24(SC.scrptr,x1,y,SC.offsd);

	
    _hline_24bit(sadrs,xlen,t_col,mov);      
}

