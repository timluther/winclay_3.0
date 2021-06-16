
#ifndef _gbasics_h_
#define _gbasics_h_


//colour weights
#define rwt 12 
#define gwt 26 
#define bwt 14

////graphic driver identifiers//////////////////////////////////////////////
#define STsoftware 0
#define STopengl   1  //Opengl routines
#define STdirectx  2  //Direct X routines

#define RFright   0x01
#define RFbellow  0x02
#define RFleft    0x04
#define RFabove    0x08
#define RFabove_right  RFabove+RFright
#define RFbellow_right RFbellow+RFright
#define RFabove_left   RFabove+RFleft
#define RFbellow_left  RFbellow+RFleft
#define RFmiddle       RFabove+RFbellow+RFright+RFleft

//Rectangle list return codes
#define RSlist 0
#define RSobscured 1
#define RSnooverlap 2



////COLOUR STRUCTURES//////////////////////////////////////////////////////////
typedef struct trgb_
{
	UINT8 r,g,b;
}trgb;


typedef struct trgbfloatbase
{
    float r,g,b;
}trgbfloatbase;

typedef struct trgbafloatbase
{
    float r,g,b,a;
}trgbafloatbase;

typedef struct trgbabase
{
    UINT8 r,g,b,a;
}trgbabase;


typedef struct trgbawordbase
{
    UINT16 r,g,b,a;
}trgbawordbase;

class trgba:public trgbabase
{
    public:
    trgba(){r=g=b=a=0;}
    trgba(UINT8 r_,UINT8 g_,UINT8 b_,UINT8 a_){r=r_;g=g_;b=b_;a=a_;}
    bool operator ==(trgba &other){return (r==other.r && g==other.g && b==other.b && a==other.a);}
    bool operator !=(trgba &other){return (r!=other.r || g!=other.g || b!=other.b || a!=other.a);}
    trgba &operator =(UINT8 val){r=b=g=a=val;return *this;}
    trgba &operator <<=(UINT8 val){r<<=val;g<<=val;b<<=val;a<<=val;return *this;}
    trgba &operator >>=(UINT8 val){r>>=val;g>>=val;b>>=val;a<<=val;return *this;}    
    UINT32 getdword(){return r|g<<8|b<<16|a<<24;}
    
    /*operator trgb <<(UINT8 val){trgb r;r.r=r<<val;r.g=g<<val;r.g=g<<val;return r;}    
    operator trgb >>(UINT8 val){trgb r;r.r=r>>val;r.g=g>>val;r.g=g>>val;return r;}*/
};      

class trgbaword:public trgbawordbase
{
    public:
    trgbaword(){r=g=b=a=0;}
    trgbaword(UINT16 r_,UINT16 g_,UINT16 b_,UINT16 a_){r=r_;g=g_;b=b_;a=a_;}
    bool operator ==(trgbaword &other){return (r==other.r && g==other.g && b==other.b && a==other.a);}
    bool operator !=(trgbaword &other){return (r!=other.r || g!=other.g || b!=other.b || a!=other.a);}
    bool operator ==(trgba &other){return (r==other.r && g==other.g && b==other.b && a==other.a);}
    bool operator !=(trgba &other){return (r!=other.r || g!=other.g || b!=other.b || a!=other.a);}
    trgbaword &operator =(UINT8 val){r=b=g=a=val;return *this;}
    trgbaword &operator <<=(UINT8 val){r<<=val;g<<=val;b<<=val;a<<=val;return *this;}
    trgbaword &operator >>=(UINT8 val){r>>=val;g>>=val;b>>=val;a<<=val;return *this;}
    /*operator trgb <<(UINT8 val){trgb r;r.r=r<<val;r.g=g<<val;r.g=g<<val;return r;}    
    operator trgb >>(UINT8 val){trgb r;r.r=r>>val;r.g=g>>val;r.g=g>>val;return r;}*/
};

class trgbfloat:public trgbfloatbase
{
    public:
    trgbfloat(){r=g=b=0.0f;}
    trgbfloat(float r_,float g_,float b_){r=r_;g=g_;b=b_;}    
    trgbfloat &operator =(trgba &other){r=other.r /255.0f;g=other.g /255.0f;b=other.b/255.0f;return *this;}
    bool operator ==(trgbfloat &other){return (r==other.r && g==other.g && b==other.b);}
    bool operator !=(trgbfloat &other){return (r!=other.r || g!=other.g || b!=other.b);}
    trgbfloat &operator =(float val){r=b=g=val;return *this;}
    trgbfloat &operator =(trgbabase &other){r=other.r/256.0f;g=other.g/256.0f;b=other.b/256.0f;return *this;}
};      


class trgbafloat:public trgbafloatbase
{
    public:
    trgbafloat(){r=g=b=a=0.0f;}
    trgbafloat(float r_,float g_,float b_,float a_){r=r_;g=g_;b=b_;a=a_;}    
	
    bool operator ==(trgbafloat &other){return (r==other.r && g==other.g && b==other.b && a==other.a);}
    bool operator !=(trgbafloat &other){return (r!=other.r || g!=other.g || b!=other.b && a==other.a);}
    trgbafloat &operator =(float val){r=b=g=a=val;return *this;}    
};      


class tpointbase 
{
public:
        INT32 x,y;
};

//This is the union which t_col and t_fillcol typed as.
//possibility of extra fields for alpha values.

typedef struct tcolour
{
    union
    {
        struct { UINT8 c8; };                                                      
        struct { UINT16 c16;};
        trgbabase c32;
        UINT32 c32val;
    };
}tcolour;

class trectlist;

//the rectbuffer heap
void destroyplist(trectlist *p);       
trectlist* getrectmem();
trectlist* ungetrectmem();

//rectlist global data
extern theapmem rectbuffer;
extern UINT8 rectlist_status;
extern UINT32 rect_count;
extern trectlist *lastrect;

inline void swapb(UINT8 &a, UINT8 &b){UINT8 t;t=a;a=b;b=t;}

template <class T> class trecttemplate
{
    public:
    tpointtemplate<T> a,b;
    trecttemplate(){a.x=b.y=0;}
    trecttemplate(tpointtemplate<T> a_,tpointtemplate<T> b_){a=a_;b=b_;}
    trecttemplate(T x,T y){a.x=a.y=0;b.x=x-1;b.y=y-1;}
    trecttemplate(T  x1,T y1,T x2,T y2){a.x=x1;a.y=y1;b.x=x2;b.y=y2;}    
    
    void rpos(T x1, T y1){T w=width();T h=height();a.x=x1;a.y=y1;b.x=x1+w;b.y=y1+h;};    
    void rmove(T x1, T y1){a.x += x1;a.y += y1;b.x += x1;b.y += y1;};
    void rgrow(T x1, T y1){a.x -= x1;a.y -= y1;b.x += x1;b.y += y1;}       
    bool rcontains(T x1, T y1){return (x1>=a.x) && (y1>=a.y) && (x1<=b.x) && (y1<=b.y);};
    bool rcontains(tpointtemplate<T>  &p){return (p.x>=a.x) && (p.y>=a.y) && (p.x<=b.x) && (p.y<=b.y);};
    bool rinside (trecttemplate &r){  return (a.x>=r.a.x) && (b.x<=r.b.x) && (a.y>=r.a.y) && (b.y<=r.b.y);};    
    bool roverlap(trecttemplate &r){return ( (r.b.x>=a.x) && (r.a.x<=b.x) && (r.b.y>=a.y) && (r.a.y<=b.y) );};
    void zero(){a.x=a.y=b.x=b.y=0;}
    UINT8 rempty(){return (a==b);}
    T height(){return b.y-a.y;}
    T width(){return b.x-a.x;}
    T halfx(){return width() / 2;}
    T halfy(){return height() /2;}
    
    trecttemplate  &operator =(trecttemplate &other){a.x=other.a.x;a.y=other.a.y;b.x=other.b.x;b.y=other.b.y;return *this;}    
    trecttemplate &operator +(tpointtemplate<T> &other){static trecttemplate r;r.a=a+other;r.b=b+other;return r;}
    trecttemplate &operator -(tpointtemplate<T> &other){static trecttemplate r;r.a=a-other;r.b=b-other;return r;}
    trecttemplate &operator +=(tpointtemplate<T>&other){a+=other;b+=other;return *this;}	
    trecttemplate &operator -=(tpointtemplate<T>&other){a-=other;b-=other;return *this;}

    trecttemplate &operator =(tpointtemplate<T> &other){a=0;b.x=other.x-1;b.y=other.y-1;return *this;}
    bool operator==(trecttemplate &other){return (a==other.a) && (b==other.b);}
    bool operator!=(trecttemplate &other){return (a!=other.a) | (b!=other.b);}

    void rassign(T x1,T y1, T x2, T y2)
    {
        if (x2>=x1) {a.x=x1;b.x=x2;} else {a.x=x2;b.x=x1;}
        if (y2>=y1) {a.y=y1;b.y=y2;} else {a.y=y2;b.y=y1;}
    }

    void rassign(T x2, T y2)
    {
        a.x=a.y=0;
        b.x=x2-1;
        b.y=y2-1;
    }
    
    void sortelements()
    {
       T temp;
       if ((temp=a.x)>b.x) {a.x=b.x;b.x=temp;}
       if ((temp=a.y)>b.y) {a.y=b.y;b.y=temp;}
    }

    void runion(trecttemplate &r)
    {   
        if (r.b.x>b.x) b.x=r.b.x;
        if (r.b.y>b.y) b.y=r.b.y;
        if (r.a.x<a.x) a.x=r.a.x;
        if (r.a.y<a.y) a.y=r.a.y;
    }                      

    bool rintersect(trecttemplate &r)
    {
        if ((r.b.x>=a.x) && (r.a.x<=b.x) && (r.b.y>=a.y) && (r.a.y<=b.y)) 
        {
           if ((r.a.x>=a.x) && (r.a.x<=b.x)) a.x=r.a.x;
           if ((r.a.y>=a.y) && (r.a.y<=b.y)) a.y=r.a.y;
           if ((r.b.x>=a.x) && (r.b.x<=b.x)) b.x=r.b.x;
           if ((r.b.y>=a.y) && (r.b.y<=b.y)) b.y=r.b.y;
           return TRUE;
       }else
       {
          zero(); 
          return FALSE;
       }    
   }

   bool rintersect(T x1,T y1,T x2,T y2)
   {
      if ((x2>=a.x) && (x1<=b.x) && (y2>=a.y) && (y1<=b.y)) 
      {
          if ((x1>=a.x) && (x1<=b.x)) a.x=x1;
          if ((y1>=a.y) && (y1<=b.y)) a.y=y1;
          if ((x2>=a.x) && (x2<=b.x)) b.x=x2;
          if ((y2>=a.y) && (y2<=b.y)) b.y=y2;
          return TRUE;
      }else
      {
          zero(); 
          return FALSE;
      }
   }

   bool rintersect(T x,T y)
   {
      if ((x>a.x) && (0<=b.x) && (y>a.y) && (0<=b.y)) 
      {
          if ((0>=a.x) && (0<=b.x)) a.x=0;
          if ((0>=a.y) && (0<=b.y)) a.y=0;
          if ((a.x<x) && (x<b.x)) b.x=x-1;
          if ((a.y<y) && (y<b.y)) b.y=y-1;
          return TRUE;
      }else
      {
          zero(); 
          return FALSE;
      }
   }

   
   bool rconcatable(trecttemplate &r)
   {
       return (((r.b.x>=a.x-1) && (r.a.x<=b.x+1) && (r.a.y==a.y) && (r.b.y==b.y))||((r.a.x==a.x)&&(r.b.x==b.x)&&(r.b.y>=a.y-1)&&(r.a.y<=b.y+1)));
   }

   inline char outcode(T &x1,T &y1)
   {
      return  ((x1>b.x))+     //right  =$01}
               ((y1>b.y)<< 1)+ //bellow =$02}
               ((x1<a.x)<< 2)+ //left   =$04}
               ((y1<a.y)<< 3); //above  =$08}
   }


   
//template <class INT32> bool trecttemplate<INT32>::clipline(INT32 &xa,INT32 &ya,INT32 &xb,INT32 &yb)
   bool clipline(T &xa,T &ya,T &xb,T &yb)
   {   
     unsigned char  code0,code1;
     T swapme;
     while (1)
         {
         code0=outcode(xb,yb);
                 code1=outcode(xa,ya);            
    if ((code0 & code1) !=0)
      return(FALSE);//Trivial reject}
    else {
      if (!(code0 | code1)!=0)
         return(TRUE); //Trivial accept}
      else if (code0==0){
        swap(code1,code0);
        swapme=xb;xb=xa;xa=swapme;
        swapme=yb;yb=ya;ya=swapme;        
      }
      if ((code0 & RFbellow)!=0)
      {
        xb+=((xa-xb)*(b.y-yb)/(ya-yb));
        yb=b.y;
      }else if ((code0 & RFabove)!=0)
      {
        xb+=((xa-xb)*(a.y-yb)/(ya-yb));
        yb=a.y;
      }else if ((code0 & RFright)!=0)
      {
        yb+=((ya-yb)*(b.x-xb)/(xa-xb));
        xb=b.x;
      }else if ((code0 & RFleft)!=0)
      {
        yb+=((ya-yb)*(a.x-xb)/(xa-xb));
        xb=a.x;
      }
    }
  }
}





//trectlist *cutfrom(trecttemplate &r);

//template <class T> trectlist* trecttemplate<T>::cutfrom(trecttemplate r)

trectlist *cutfrom(trecttemplate r)
{
    trectlist *newlist;          
    if (!(r.rintersect(*this)))
    {
        rect_count=1;
        lastrect=getrectmem();
        lastrect->rassign(a.x,a.y,b.x,b.y);
        rectlist_status=RSnooverlap;
        lastrect->next=NULL;
        return lastrect;
    }
    rect_count=0;
    lastrect=NULL;
    if (*this==r)
    {
        rectlist_status=RSobscured;
        return NULL;
    }    
    newlist=lastrect=(trectlist*)rectbuffer.curr;
    
    if (a.y<r.a.y)
    {
         lastrect=lastrect->next=getrectmem();
         rect_count++;
         lastrect->rassign(a.x,a.y,b.x,r.a.y-1);
    };
    if (a.x<r.a.x)
    {
         lastrect=lastrect->next=getrectmem();
         rect_count++;
         lastrect->rassign(a.x,r.a.y,r.a.x-1,r.b.y);
    }
        
    if (b.x>r.b.x)
    {
        lastrect=lastrect->next=getrectmem();
        rect_count++;
        lastrect->rassign(r.b.x+1,r.a.y,b.x,r.b.y);
    }
    if (b.y>r.b.y)
    {
        lastrect=lastrect->next=getrectmem();
        rect_count++;
        lastrect->rassign(a.x,r.b.y+1,b.x,b.y);
    }
    
    if (rect_count>0)
    {
        rectlist_status=RSlist;
        lastrect->next=NULL;
        return newlist;
    }else
    {
        rectlist_status=RSobscured;
        return NULL;        
    }        
    return NULL;
}

   
   
};

     
typedef trecttemplate<float> trectfloat;
typedef trecttemplate<double> trectdouble;
typedef trecttemplate<flt> trectflt;
typedef trecttemplate<INT16> trect16;
typedef trecttemplate<INT32> trect;


class trectlist:public trect
{
  public:
  void operator=(trect &other){a=other.a;b=other.b;}
  trectlist *next;
};

typedef struct gpoint
{
    INT16 x,y;
    UINT8 r,g,b,a;
    UINT16 u,v;
    UINT32 z;
}gpoint;


#define CTproportional 1 //use this for monospaced fonts
#define CTreversed 2     //fontdata is in reversed format
#define CTpixelpacked 3   //data is in byte per pixel format

////FONT STRUCTURES//////////////////////////////////////////////////////////


class tblitrect :public trect
{
	public:
	tpoint hotspot;
	UINT32 attrib;
	INT32 w,h;   //precalced width and height
	void assign(INT32 x,INT32 y,INT32 x2,INT32 y2)
	{
		a.assign(x,y);
		b.assign(x2,y2);
		w=width()+1;
		h=height()+1;
		attrib=0;
		hotspot.assign(0,0);
	}
};


typedef void (*drawscanlinefunc) (char *adrs,UINT32 count);



//COLOUR & PALETTE HANDLING ROUTINES////////////////////////////////////////////////
//these need some work...

void COLdone();
char COLinit(UINT16 _maxcolours);
void COLsetrgb(UINT8 colno,UINT8 red,UINT8 green,UINT8 blue);
INT8 COLallocatergb(UINT8 rc,UINT8 gc,UINT8 bc);
void COLsetpal(trgba *pal,UINT8 start,UINT8 len);
void COLgethardwarepal(trgba *pal);
void COLsethardwarepal(trgba *pal,UINT16 start, UINT16 len);
void COLgetrgb(UINT8 colno,UINT8 *red,UINT8 *green,UINT8 *blue);
void COLsethsv(trgba *col,float h,float s,float v);
void COLgethsv(trgba *col,UINT8 &hi,UINT8 &si,UINT8 &vi);
void COLswaprgb(trgba *col1,trgba *col2);
void COLswapcol(UINT8 col1,UINT8 col2);
void COLcopycol(UINT8 col1,UINT8 col2);
void COLgrad(trgba *pal,UINT16 col1,UINT16 col2);
void COLgrad(tcolour *pal,UINT16 col1,UINT16 col2,trgba tcol1,trgba tcol2);
tpointfloat *zrotatepoint(tpointfloat *in,tpointfloat *out,flt r);
tpointfloat *zrotatepointaround(tpointfloat in,tpointfloat *out,flt r,tpointfloat *around);
extern trgba *colourmap;

#endif

