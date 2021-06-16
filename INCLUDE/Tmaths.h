#ifndef _tmaths_h_
#define _tmaths_h_

#define FLT_HALF 0.5f
#define FLT_3 3.0f
#define FLT_2 2.0f  
#define FLT_1 1.0f
#define FLT_0 0.0f
  //((1+sqrt(5)) / 2)
#define GOLDENRATIO 1.618034f  
#define PI 3.14159265358979323846264338327950288419716939937510582097494459230781641f
#define HALFPI 1.570796326795f
#define TWOPI 6.28318530718f
#define FIXEDFLOATMAGIC 4503601774854144.000000f
#define FLT_CLOSETOTHRESHOLD 0.0001
#define fcloseto(A,B)  ((fabs(A-B)) < (FLT_CLOSETOTHRESHOLD)?(TRUE):(FALSE))
#define fcloseto2(A,B,T)  ((fabs(A-B)) < (T)?(TRUE):(FALSE))

class matrix4x4;

class fixedp16x16
{
    INT32 data;
    fixedp16x16(UINT32 val){data=val << 16;}
    fixedp16x16(float val){data=(int)val;}
    fixedp16x16(double val){data=(int)val;}        
    fixedp16x16 operator =(UINT32 other){data=other<<16;return *this;}
};    

template <class T> class tpointtemplate
{
    public:
    T x,y;
    tpointtemplate(){x=y=0;}
    tpointtemplate(T v){x=y=v;}
    tpointtemplate(T x_,T y_){x=x_;y=y_;}

    tpointtemplate &operator =(tpointtemplate &other){x=other.x;y=other.y;return *this;}    
    tpointtemplate &operator =(T val){x=y=val;return *this;}    
    void assign(T x1,T y1){x=x1;y=y1;};
    bool closeto(tpointtemplate *other,T t){return ((x>=other->x-t)&&(y>=other->y-t)&&(x<=other->x+t)&&(y<=other->y+t));}
	operator T *(){return &x;}
    tpointtemplate &operator +(tpointtemplate &other){static tpointtemplate r;r.x=x+other.x;r.y=y+other.y;return r;}
    tpointtemplate &operator +(T v){static tpointtemplate r;r.x=x+v;r.y=y+v;return r;}
    tpointtemplate &operator -(tpointtemplate &other){static tpointtemplate r;r.x=x-other.x;r.y=y-other.y;return r;}
    tpointtemplate &operator -(T v){static tpointtemplate r;r.x=x-v;r.y=y-v;return r;}
    tpointtemplate &operator *(tpointtemplate &other){static tpointtemplate r;r.x=x*other.x;r.y=y*other.y;return r;}
    tpointtemplate &operator *(T v){static tpointtemplate r;r.x=x*v;r.y=y*v;return r;}
    tpointtemplate &operator /(tpointtemplate &other){static tpointtemplate r;r.x=x/other.x;r.y=y/other.y;return r;}
    tpointtemplate &operator /(T v){static tpointtemplate r;r.x=x/v;r.y=y/v;return r;}
    tpointtemplate &operator +=(tpointtemplate &other){x+=other.x;y+=other.y;return *this;}
	tpointtemplate &operator ++(){x++;y++;return *this;}
	tpointtemplate &operator --(){x--;y--;return *this;}
    tpointtemplate &operator +=(T v){x+=v;y+=v;return *this;}
    tpointtemplate &operator -=(tpointtemplate &other){x-=other.x;y-=other.y;return *this;}
    tpointtemplate &operator -=(T v){x-=v;y-=v;return *this;}
    tpointtemplate &operator *=(tpointtemplate &other){x*=other.x;y*=other.y;return *this;}
    tpointtemplate &operator *=(T v){x*=v;y*=v;return *this;}
    tpointtemplate &operator /=(tpointtemplate &other){x/=other.x;y/=other.y;return *this;}
    tpointtemplate &operator /=(T v){x/=v;y/=v;return *this;}
    
   
    T findclosestpointscalar(tpointtemplate &v1,tpointtemplate &v2)
    {
       static tpointtemplate p1;
       p1=v2-v1;
       return (((x-v1.x)*p1.x) + ((y-v1.y)*p1.y))/p1.lengthsquared();
    }
    
    tpointtemplate &findclosestpoint(tpointtemplate &v1,tpointtemplate &v2)
    {
        static tpointtemplate r;
        static tpointtemplate p1;
        static T dot;
        p1=v2-v1;        
        dot=(((x-v1.x)*p1.x) + ((y-v1.y)*p1.y))/p1.lengthsquared();
        BOUND(dot,0,1);        
        
        r=v1+(p1 * dot);              
        return r;
    }

    T finddistancefromlinesquared(tpointtemplate &v1,tpointtemplate &v2)
    {
        static tpointtemplate r;
        r=(*this)-findclosestpoint(v1,v2);
        return r.lengthsquared();        
    }
    
    T dot(tpointtemplate &vec2) {return ((x*vec2.x) + (y*vec2.y));}
    
    bool operator==(tpointtemplate &other){return (x==other.x)&&(y==other.y);};
    bool operator==(T v){return (x==v)&&(y==v);};
    bool operator!=(tpointtemplate &other){return (x!=other.x)||(y!=other.y);};
    bool operator!=(T v){return (x!=v)||(y!=v);};
    bool aligned(tpointtemplate &other){return((x==other.x)||(y==other.y));};
    T lengthsquared(){return (T)(x*x) + (y*y);}
    T length(){return (T)sqrt((double)(x*x) + (y*y));}
    //turn into unit length vector
    void normalize()
    {
        T uli=(T)length();
        if (uli!=0)
        {            
            x/=uli;
            y/=uli;
        }            
    }
};


typedef tpointtemplate<INT16> tpoint16;
typedef tpointtemplate<INT32> tpoint;
typedef tpointtemplate<INT32> tpoint32;
typedef tpointtemplate<float> tpointfloat;
typedef tpointtemplate<double> tpointdouble;
typedef tpointtemplate<flt> tpointflt;

class vector
{
    public:
    
    flt x,y,z;
    
    vector(flt x_,flt y_,flt z_){x=x_;y=y_;z=z_;}
    vector(flt v){x=y=z=v;}
    vector(){x=y=z=FLT_0;}
    void moveto(flt x_,flt y_,flt z_){x=x_;y=y_;z=z_;}
	operator flt *() {return &x;}
    
    vector &operator +=(const vector &other){x+=other.x;y+=other.y;z+=other.z;return *this;}
    vector &operator -=(const vector &other){x-=other.x;y-=other.y;z-=other.z;return *this;}
    vector &operator =(flt s){x=y=z=s;return *this;}
    vector &operator -=(flt s){x-=s;y-=s;z-=s;return *this;}
    vector &operator +=(flt s){x+=s;y+=s;z+=s;return *this;}
    vector &operator /=(flt s){s=FLT_1/s;x*=s;y*=s;z*=s;return *this;}
    vector &operator *=(flt s){x*=s;y*=s;z*=s;return *this;}    
    flt  operator^(const vector &v2) {return (flt)x*v2.x + y*v2.y + z*v2.z;}

    bool closeto(const vector &v2){return (fcloseto(x,v2.x) && fcloseto(y,v2.y) && fcloseto(z,v2.z));}
    bool operator ==(const vector &other){return (x==other.x && y==other.y && z==other.z);}
    bool operator !=(const vector &other){return (x!=other.x || y!=other.y || z!=other.z);}
    bool operator ==(flt val){return (x==val && y==val && z==val);}
    bool operator !=(flt val){return (x!=val && y!=val && z!=val);}
    
    vector &operator +(const vector &other){static vector r;r.x=x+other.x;r.y=y+other.y;r.z=z+other.z; return r;}
    vector &operator -(const vector &other){static vector r;r.x=x-other.x;r.y=y-other.y;r.z=z-other.z; return r;}
    vector &operator *(const vector &other){static vector r;r.x=x*other.x;r.y=y*other.y;r.z=z*other.z;return r;}
    vector &operator /(const vector &other){static vector r;r.x=x/other.x;r.y=y/other.y;r.z=z/other.z;return r;}
    vector &operator -(flt other){static vector r;r.x=x-other;r.y=y-other;r.z=z-other; return r;}
    vector &operator +(flt other){static vector r;r.x=x+other;r.y=y+other;r.z=z+other; return r;}
    vector &operator -(){static vector r;r.x=-x;r.y=-y;r.z=-z; return r;}
    vector &operator /(flt s){static vector r;s=FLT_1/s;r.x=x*s;r.y=y*s;r.z=z*s;return r;}
    
    vector &operator *(flt s){static vector r;r.x=x*s;r.y=y*s;r.z=z*s;return r;}
    
    //vector treated as magnitude vector from 0,0,0 to x,y,z
    flt magnitude(){return (x*x) + (y*y) + (z*z);}
    flt length(){return (flt)sqrt(magnitude());}
    
    //turn into unit length vector
    void normalize()
    {
        flt uli=(flt)sqrt((x*x) + (y*y) + (z*z));
        if (uli!=0)
        {
            uli=FLT_1/uli;     
            x*=uli;y*=uli;z*=uli;
        }            
    }
    vector *multmatrix(matrix4x4 *mat,vector *into);
    vector *multmatrixinv(matrix4x4 *mat,vector *into);
    vector *multmatrixtrans(matrix4x4 *mat,vector *into);
    vector *multmatrixtransinv(matrix4x4 *mat,vector *into);

    vector *multmatrixtrans_delta(matrix4x4 *mat,vector *into,flt delta);
};

typedef vector vector3;

//will be used for rotation interpolation, need convertion routines to and from other rotation systems
class quaternion
{
	flt a,b,c,d;
};

class axisrotation:public vector
{
    public:
    axisrotation(flt x_,flt y_,flt z_,flt t_){x=x_;y=y_;z=z_;t=t_;};
    axisrotation(){x=y=z=t=0;};
    flt t;
};

//will be used in projection and clipping
class vector4:public vector
{
    public:
    vector4(flt x_,flt y_,flt z_,flt w_){x=x_;y=y_;z=z_;w=w_;};
    vector4(){x=y=z=w=0;};
    flt w;
};

typedef matrix4x4 *matrix4x4ptr;

enum matrixConcatType {POSTCONCAT,PRECONCAT,REPLACE};

class matrix4x4
{
    public:
    flt m00,m01,m02,m03,
        m10,m11,m12,m13,
        m20,m21,m22,m23;
	vector p;
	flt m33;
		
	
	operator flt *() {return &m00;}
	
	//vector *p(){return (vector*)&m30;}

    matrix4x4(){m01=m02=m10=m12=m20=m21=p.x=p.y=p.z=0;m00=m11=m22=m33=1;}
    matrix4x4 &operator *(flt s)
    {
        static matrix4x4 r;flt *f=&r.m00;                              
        for (UINT32 i=0;i<12;i++,f++)
           *f*=s;
        return r;
    }
    matrix4x4 &operator /(flt s)
    {
        static matrix4x4 r;flt *f=&r.m00;
        s=FLT_1/s;
        for (UINT32 i=0;i<12;i++,f++)
           *f*=s;
        return r;
   }
   /*void addtrans(const vector &v){p.x+=v.x; p.y+=v.y; p.z+=v.z; }
   void settrans(const vector &v){p.x= v.x; p.y= v.y; p.z= v.z; }
   void loadtrans(vector *v){v->x=p.x;v->y=p.y;v->z=p.z;}*/
};

class matrix3x3
{
    public:
    flt m00,m01,m02,
        m10,m11,m12,
        m20,m21,m22;
};


vector planelineintersection(vector *n,flt d,vector *p1,vector *p2);
bool concurrent(vector a,vector b);

vector *LMmatrixtransformvec(matrix4x4ptr mat,vector * vec,vector * result);

//matrix m2 is preconcatinated to matrix m1.. counterintuative :(
matrix4x4ptr LMconcatenatematrices(register matrix4x4ptr m1,register matrix4x4ptr m2,matrix4x4ptr result);
void LMtranspose(matrix4x4ptr mat);
void LMinvert(matrix4x4ptr mat);
matrix4x4ptr LMrottrans(matrix4x4ptr mat,flt rx,flt ry,flt rz,flt xt,flt yt,flt zt);
matrix4x4ptr LMtransrot(matrix4x4ptr mat,flt rx,flt ry,flt rz,flt xt,flt yt,flt zt);
matrix4x4ptr LMrottransinv(matrix4x4ptr mat,flt rx,flt ry,flt rz,flt xt,flt yt,flt zt);
matrix4x4ptr LMidentity(matrix4x4ptr mat);
matrix4x4ptr LMxrotation(register matrix4x4ptr mat,flt rotation);
matrix4x4ptr LMyrotation(register matrix4x4ptr mat,flt rotation);
matrix4x4ptr LMzrotation(register matrix4x4ptr mat,flt rotation);
matrix4x4ptr LMrst(register matrix4x4ptr m,flt xrotation,flt yrotation,flt zrotation,flt scx,flt scy,flt scz,flt tx,flt ty,flt tz);
matrix4x4ptr LMaddscale(register matrix4x4ptr m,register matrix4x4ptr mr,flt scx,flt scy,flt scz);
matrix4x4ptr LMxyzrotation(register matrix4x4ptr mat,flt xrotation,flt yrotation,flt zrotation);
matrix4x4ptr LMzyxrotation(register matrix4x4ptr mat,flt xrotation,flt yrotation,flt zrotation);
matrix4x4ptr LMscale(register matrix4x4ptr mat,flt xscale,flt yscale,flt zscale);
matrix4x4ptr LMtranslate(register matrix4x4ptr mat,flt xtrans,flt ytrans,flt ztrans);
matrix4x4ptr LMaxisrotation(register matrix4x4ptr mat,flt x,flt y,flt z,flt rotation);
matrix4x4ptr LMstretch(register matrix4x4ptr mat,vector *oldmin,vector *oldmax,vector *newmin,vector *newmax);
matrix4x4ptr LMrotatearound(matrix4x4ptr mat,flt rx,flt ry,flt rz,vector *vec);
matrix4x4ptr LMprojectxy(matrix4x4ptr mat,vector *vec,vector *centre);
matrix4x4ptr LMinvprojectxy(matrix4x4ptr mat,vector *vec,vector *centre);

void simpleyrot(flt r,vector *v);
void simplexrot(flt r,vector *v);
flt safepow(flt val,flt pow);
flt genpower (flt number, flt exponent);
flt ipow(flt val,int power);
flt dot(vector &vec1,vector &vec2);
flt dot(vector *vec1,vector *vec2);
flt findclosestpoint(vector &v1,vector &v2,vector &v3,vector &out);
vector *cross(vector &rvec1,vector &vec1,vector &vec2);
void reflection(vector &vec1,vector &vec2,vector &rvec);
flt distance(vector &vec1,vector &vec2);
void calcnormal(vector p1,vector &p2,vector p3,vector & pn);
tpointfloat *zrotatepoint(tpointfloat *in,tpointfloat *out,flt r);
tpointfloat *zrotatepointaround(tpointfloat in,tpointfloat *out,flt r,tpointfloat *around);

//fixed point macros & slope calculations


inline int float2int(float f)
{
   return (int)f;
}

inline int float2fixed(float f)
{
  return (int)(f*65536.0f);
}

inline INT32 fdiv(INT32 a,INT32 b)
{
    return (a/b);    
}

//these two are used by the triangle scanning routine
#define imul(r,a,b) _asm mov eax ,a _asm mov edx,b _asm imul edx _asm shrd eax,edx,16 _asm mov r,eax ;
#define iceil(r,a) _asm mov eax,a _asm add eax,0xFFFF _asm shr eax,16 _asm mov r,eax;
#define idiv(r,a,b) _asm mov eax,a _asm mov edx,b _asm mov edx,eax _asm sar edx,16 _asm shl eax,16 _asm idiv ebx _asm mov r,eax ;
#define iwholeint(r,a) _asm mov eax,a _asm add eax,0xFFFF _asm and eax,0xFFFF0000 _asm mov r,eax;
#define iinterror(r,a) _asm mov eax,a _asm mov eax,eax _asm add eax,0xFFFF _asm and eax,0xFFFF0000 _asm _asm sub eax,ebx _asm mov r,eax;
#define idivs(r,a,b) _asm mov eax,a _asm mov edx,b _asm cdq _asm idiv ebx _asm mov r,eax;
#define imuls(r,a,b) _asm mov eax,a _asm mov edx,b _asm imul edx _asm mov r,eax;
#define satadd(r,a,b) _asm mov eax,a _asm mov edx,b _asm add al,dl _asm sbb,cl,cl _asm or al,cl _asm mov r,eax;
#define satsub(r,a,b) _asm mov eax,a _asm mov edx,b _asm sub al,dl _asm mov cl,255 _asm adc cl,0 _asm and al,cl _asm mov r,eax;


extern float retfloat;
extern INT32 retint;
extern double flthalf;

inline float fceil(const float input)
{   
   _asm
   {   
       fld     input
       fadd    qword ptr flthalf
       frndint        
       fstp    retfloat
   }
 
   return retfloat;
}

inline float fgetscanlines(const float s1,const float s2)
{
 
   _asm
   {   
       fld     s1
       fadd    qword ptr flthalf
       frndint
       fld     s2
       fadd    qword ptr flthalf
       frndint
       fsubp   st(1),st
       fstp    retfloat
   }
   return retfloat;
}


inline float SUB_PIX(float input)
{
	//=ceil(input)-input;//float2int(input)-input; 
    _asm
    {   
        fld     input
        fadd    qword ptr flthalf
        frndint        
        fld     input        
        fsubp   st(1), st
        fstp    retfloat
    }    
    return retfloat;       
}

inline INT32 imul2(INT32 a,INT32 b)
{	
	imul(retint,a,b);
	return retint;
}

inline INT32 iceil2(INT32 a)

{	
	iceil(retint,a);
	return retint;
}




#endif
