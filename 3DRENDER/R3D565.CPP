
void gradit(UINT32 col1,UINT32 col2,UINT32 v1,UINT32 v2)
{
  #define colshift 8
  PIXINT plus,p,step;  
  if (col2<col1) swap(col1,col2);
  step=1+(col2-col1);
  
  plus=((v2-v1)<< colshift)/ step;
    
  p=(v1<<colshift);
  for (;col1<col2;col1++)
  {
        UINT32 rb=(p>>colshift)&255;
        bound0to256_16.r[col1]=conv24to16.r[rb];
        bound0to256_16.g[col1]=conv24to16.g[rb];
        bound0to256_16.b[col1]=conv24to16.b[rb];  
        p+=plus;
  }  
}

//scan routines predefs
void sl_g(char *adrs,UINT32 count);
void sl_ga(char *adrs,UINT32 count);
void sl_gt16(char *adrs,UINT32 count);
void sl_gt16a(char *adrs,UINT32 count);
void sl_f(char *adrs,UINT32 count);
void sl_fa(char *adrs,UINT32 count);
void sl_ft16(char *adrs,UINT32 count);  
void sl_ft16a(char *adrs,UINT32 count);

void buildtables()
{
	UINT32 rsize=8-5;//8-SC.getredsize();
    UINT32 gsize=8-6;//8-SC.getgreensize();
    UINT32 bsize=8-5;//8-SC.getbluesize();
	UINT32 bshift=0;//SC.getredshift();
	UINT32 gshift=5;//SC.getgreenshift();
	UINT32 rshift=11;//SC.getblueshift();

    UINT32 rmax=(255>>rsize)<<rshift;
    UINT32 gmax=(255>>gsize)<<gshift;
    UINT32 bmax=(255>>bsize)<<bshift;    
    //setup rgb lookup table for 16/15 bit modes    
    //setup small rgb lookup table for 16/15 bit modes
	int i;
    for (i=0;i<65536;i++)
    {
       conv16to24[i].r=i>>rshift<<rsize;
       conv16to24[i].g=i>>gshift<<gsize;
       conv16to24[i].b=i>>bshift<<bsize;
       //conv16to24[i].a=0;//i>>SC.bpos<<bsize;              
    }
    //the alpha values are used for transparency
    for (i=0;i<256;i++)
    {
        for (UINT32 i2=0;i2<256;i2++)
        {
            conv16to24[(i<<8) + i2].a=(UINT8)((flt)i2*((flt)i/255.0f));
        }           
    }        
    
    for (i=0;i<256;i++)
    {
       bound0to256_16.r[i]=bound0to256_16.g[i]=bound0to256_16.b[i]=bound0to256[i]=0;       
       conv24to16.r[i]=(i>>rsize)<<rshift;
       conv24to16.g[i]=(i>>gsize)<<gshift;
       conv24to16.b[i]=(i>>bsize)<<bshift;
    }
    int ib;
    for (i=256;i<512;i++)       
    { 
       ib=(i-256);       
       bound0to256_16.r[i]=conv24to16.r[ib];
       bound0to256_16.g[i]=conv24to16.g[ib];
       bound0to256_16.b[i]=conv24to16.b[ib];
       bound0to256[i]=ib;
       conv24to16.r[i]=rmax;
       conv24to16.g[i]=gmax;
       conv24to16.b[i]=bmax;
    }
    
    for (i=512;i<1024;i++)
    {
       bound0to256[i]=255;
       bound0to256_16.r[i]=rmax;
       bound0to256_16.g[i]=gmax;
       bound0to256_16.b[i]=bmax;
    }
    gradit(0,198,0,0);
    gradit(198,340,0,100);
    gradit(340,560,100,255);    
}

////////////////////////////////////////////////////////////////////////////////////////////
//smooth shaded.. lots going on here to optimize.
//Aligned on dword boundaries
//Optimized upper word calculations
//probably more cylces can be shaved by better pentium instruction pairing


void sl_g(char *adrs,UINT32 count)
{
 

	//this code works but the colours need to be in 5:6:5 mode	
	_asm
	{
		mov ecx,count;
		mov esi,adrs
		mov ebx,xd.r
		mov edx,xd.g
		mov edi,xd.b
		push ebp
		xloop:
			mov eax,ebx
			and eax,PRESHIFTRMASK16bit 
			mov ebp,edx
			and ebp,PRESHIFTGMASK16bit 
			or  eax,ebp
			or  eax,edi
			shr eax,COLSHIFTBY
			mov [esi],ax
			add ebx,xd.dr
			add edx,xd.dg
			add edi,xd.db		
			add esi,2
		loop xloop
		pop ebp
		  
	}
/*
    while (count>0)
	{
		*(UINT16*)adrs=((xd.r & PRESHIFTRMASK16bit )|(xd.g & PRESHIFTGMASK16bit )|(xd.b))>>COLSHIFTBY;
		//*(UINT16*)adrs=((xd.r )|(xd.g )|(xd.b))>>COLSHIFTBY;
        xd.r+=xd.dr;
        xd.g+=xd.dg;
        xd.b+=xd.db;
        adrs+=2;
        count--;                         
    }*/
}

#define TRANS50_16bit 0xF7DE
#define TRANS25_16bit 0xE79C
#define TRANS15_16bit 0xC718
#define TRANS10_16bit 0x8610
#define TRANS5_16bit 0x200



void sl_ga(char *adrs,UINT32 count)
{
	UINT16 tex,scr;
	//this will look up into a small table each pixel to get the masks for different
	//levels of transparency. I think I can do 0 25 50 & 100.
	//There should be a check for no global changes in alpha as this will be quite common and
	//will be a valuable optimization. Besides, gouraud shading over 4 values is going to look 
	//like pants.
	
    while (count>0)
	{		
		scr=(*(UINT16*)adrs) & TRANS25_16bit;
		tex=(((xd.r & PRESHIFTRMASK16bit )|(xd.g & PRESHIFTGMASK16bit )|(xd.b))>>COLSHIFTBY) & TRANS50_16bit;
		*(UINT16*)adrs=((scr+tex)>>1); 
        xd.r+=xd.dr;
        xd.g+=xd.dg;
        xd.b+=xd.db;
		xd.a+=xd.da;
        adrs+=2;
        count--;                         
    }
}

inline UINT32 get16bittexel()
{
_asm mov ecx,t_tex_xshift; 
_asm mov esi,dword ptr xd+40; 
_asm shr esi,cl; 
_asm and esi,t_tex_ymask; 
_asm add esi,t_texturedataptr; 
_asm mov ebx,dword ptr xd+32; 
_asm shr ebx,15; 
_asm and ebx,t_tex_xmask; 
_asm mov bx,[esi+ebx]; 
_asm mov eax,ebx;
}
							
        
//masks for 256 x 256 16bit texture
//[(((xd.v>>7)&0x3FE00))+((xd.u>>15)&0x1FE )]

//I want to be able to generate code for rasterizing by using building
//blocks of machine code slotted together.
//This will also include replacing delta values with constants to save
//registers as these do not change over a triangle's length.

void sl_t16(char *adrs,UINT32 count)
{
    _asm
    {
      mov edx,adrs
      mov ecx,count
      push ebp                  
      mov ebp,dword ptr xd+40 //get v
      mov eax,dword ptr xd+32 //get u
      mov esi,t_texturedataptr//point to bitmap offsets                
      xloop:
          //get texel          
          mov edi,ebp            //load up u
          shr edi,7  
		  and edi,0x3FE00         //shift to integer range and clip
          add ebp,dword ptr xd+44 //add dv to v

		  mov ebx,eax			    //load up u
          add eax,dword ptr xd+36  //add du to u
          shr ebx,15
		  and ebx,0x1FE          //shift to integer range and clip

          mov bx,[edi+ebx]     //get pixel address
          mov [edx],bx
          add edx,2
          dec ecx                    
      jnz xloop          
      pop ebp
    }
}


//r=0
//dr=4
//g=8
//dg=12
//b=16
//db=20
//a=24
//da=28
//u=32
//du=36
//v=40
//dv=44

//this will use 16bit range numbers in the tables to reduce cache misses
//unscientific pentium optimizations :/

void sl_gt16(char *adrs,UINT32 count)
{
    
    UINT32 pix;    
    while (count>0)
    {    
		pix=get16bittexel();                      
        *(UINT16*)adrs= ((((xd.r >> COLSHIFTBY) * (pix & 0xF800))>>16) & 0xF800)|
						((((xd.g >> COLSHIFTBY) * (pix & 0x7E0))>>11) & 0x7E0)|					  
						((((xd.b >> COLSHIFTBY) * (pix & 0x1F)>>5)) & 0x1F);
		//modulate c2.t1
        /*cr=(xd.r >> COLHALFSHIFTBY);//&0x1F000;
        cg=(xd.g >> COLHALFSHIFTBY);//)&0xFC0
        cb=(xd.b >> COLHALFSHIFTBY);
                              
        *(UINT16*)adrs= (((cr * (pix & 0x7C00))>>17) & 0xF800)|
						(((cg * (pix & 0x7E0))>>12) & 0x7E0)|					  
						(((cb * (pix & 0x1F)>>6)) & 0x1F);*/
		//cheap c50.t50
		/*UINT32 p=((xd.r & 0x3E0000)|(xd.g & 0x1f800)|(xd.b))>>COLSHIFTBY;
		p&=(0xF7DE);
		pix&=(0xF7DE);
		*(UINT16*)adrs=(pix+p) >> 1;
		//cheap c50.t25
		UINT32 p=((xd.r & 0x3E0000)|(xd.g & 0x1f800)|(xd.b))>>COLSHIFTBY;
		p&=(0xF7DE);
		pix&=(0xE79C);
		*(UINT16*)adrs=(pix+p) >> 1;
		*/			    					   	    
        xd.r+=xd.dr;
        xd.g+=xd.dg;
        xd.b+=xd.db;     
        xd.u+=xd.du;
        xd.v+=xd.dv;
        adrs+=2;
        count--;
    }
}



void sl_gt16a(char *adrs,UINT32 count)
{
    

    UINT32 pix;
	UINT16 tex,scr;
    
    while (count>0)
    {    
		scr=(*(UINT16*)adrs) & TRANS25_16bit;				
		pix=get16bittexel();
		tex=(((((xd.r >> COLSHIFTBY) * (pix & 0xF800))>>16) & 0xF800)|
			((((xd.g >> COLSHIFTBY) * (pix & 0x7E0))>>11) & 0x7E0)|					  
			((((xd.b >> COLSHIFTBY) * (pix & 0x1F)>>5)) & 0x1F))& TRANS50_16bit;
        *(UINT16*)adrs=((scr+tex)>>1); 
			    					   	    
        xd.r+=xd.dr;
        xd.g+=xd.dg;
        xd.b+=xd.db;     
        xd.a+=xd.da;     
		xd.u+=xd.du;
        xd.v+=xd.dv;
        adrs+=2;
        count--;
    }
}



//the following routines not used in rendering


void sl_t16a(char *adrs,UINT32 count)
{

}

void sl_f(char *adrs,UINT32 count)
{
    _asm
    {
        mov edi,adrs
        mov ecx,count
        mov bx,word ptr t_col
        mov eax,ebx
        shl eax,16
        mov ax,bx
        test edi,1
        jz noshift           //align that data
          mov [edi],ax
          add edi,2
          dec ecx
        noshift:
        shr ecx,1
        rep stosd            //blast out some dwords
        adc ecx,ecx          //did we get a trailing word?        
        rep stosw            //if so,draw it.
    }    
}

void sl_fa(char *adrs,UINT32 count)
{
    static UINT32 cr,cg,cb;        
    pa1=xd.a;
    pa2=(255-pa1)<<8;
    static UINT32 sr,sg,sb;

    cr=conv16to24[xd.r+pa2].a;
    cg=conv16to24[xd.g+pa2].a;
    cb=conv16to24[xd.b+pa2].a;
    
    pa1<<=8;                    
    while (count>0)    
    {
        pixin=*(UINT16*)adrs;
        sb=(pixin << 3) & 255;        
        sg=(pixin >> 3) & 255;
        sr=(pixin >> 8) & 255;
        
        
        *(UINT16*)adrs=
            conv24to16.r[conv16to24[sr+pa1].a+cr]|
            conv24to16.g[conv16to24[sg+pa1].a+cg]|
            conv24to16.b[conv16to24[sb+pa1].a+cb];
        adrs+=2;
        count--;                                                  
    }
}

void sl_ft16(char *adrs,UINT32 count)
{
    static UINT32 pix;
    static UINT32 r,g,b;        
    UINT32 cr=xd.r << 1 ;
    UINT32 cg=xd.g << 1;
    UINT32 cb=xd.b << 1;        
    
    while (count>0)
    {
        //get texel into rgb
        _asm
        {
            mov ecx,t_tex_xshift			//point to bitmap offsets     			
            mov esi,dword ptr xd+40         //load up integer part of v                
            shr esi,cl                              
			and esi,t_tex_ymask			
			add esi,t_texturedataptr
            mov ebx,dword ptr xd+32         //get integer part of u                			
            shr ebx,15
			and ebx,t_tex_xmask
            mov bx,[esi+ebx]              //get pixel address            			
            mov pix,ebx            			
        }
        
        b=(pix<< 3) & 255;        
        g=(pix>> 3) & 255;
        r=(pix>> 8) & 255;
        *((UINT16*)adrs)=bound0to256_16.r[cr+r]|bound0to256_16.g[cg+g]|bound0to256_16.b[cb+b];
        
        xd.u+=xd.du;
        xd.v+=xd.dv;
        adrs+=2;
        count--;
    }
}

      
void sl_ft16a(char *adrs,UINT32 count)
{
    static UINT32 pix;
    static UINT32 r,g,b;
    static UINT32 sr,sg,sb;

    pa1=xd.a;
    pa2=(255-pa1)<<8;
    pa1<<=8;
    
    UINT32 cr=xd.r << 1 ;
    UINT32 cg=xd.g << 1;
    UINT32 cb=xd.b << 1;

    while (count>0)
    {
        //get texel into rgb
        _asm
        {
            mov ecx,t_tex_xshift			//point to bitmap offsets     			
            mov esi,dword ptr xd+40         //load up integer part of v                
            shr esi,cl                              
			and esi,t_tex_ymask			
			add esi,t_texturedataptr
            mov ebx,dword ptr xd+32         //get integer part of u                			
            shr ebx,15
			and ebx,t_tex_xmask
            mov bx,[esi+ebx]              //get pixel address            			
            mov pix,ebx            			
        }
        //get blended rgb texel
        b=bound0to256[cb+((pix<< 3) & 255)];        
        g=bound0to256[cg+((pix>> 3) & 255)];
        r=bound0to256[cr+((pix>> 8) & 255)];
        //get screen pixel into rgb
        pixin=*(UINT16*)adrs;
        sb=(pixin << 3) & 255;        
        sg=(pixin >> 3) & 255;
        sr=(pixin >> 8) & 255;
                
        *(UINT16*)adrs=
            conv24to16.r[conv16to24[sr+pa1].a+conv16to24[r+pa2].a]|
            conv24to16.g[conv16to24[sg+pa1].a+conv16to24[g+pa2].a]|
            conv24to16.b[conv16to24[sb+pa1].a+conv16to24[b+pa2].a];                      
        xd.u+=xd.du;
        xd.v+=xd.dv;
        adrs+=2;
        count--;
    }
}
