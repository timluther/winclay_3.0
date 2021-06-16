//This is the start of my openGL style rendering library.
//I will probably move the software rasterizer into this for tighter coupling but I'm
//not sure yet. This will clear up the difference between the scene graph and the rendering
//functions and also provide easier access to hardware acceleration as it is based around
//those styles of libraries, albiet with a few clayworks friendly extras (which will be optimized
//in the software version. Otherwise, let the hardware work it out).
#ifndef trender_h_
#define trender_h_

#define MAXVERTS 256

#define TR_SMOOTH    1
#define TR_TEXTURE   2 
#define TR_LIGHTING  4

//node attributes
#define naVISIBLE 0x1 //clip or not?
#define naDRAWNALREADY 0x2 //For efficient drawing. Clear after each draw.
#define naCLEARMASK 0x2
#define naTEMPORARY 0x4 //it is a volatile, temporary vertex
#define naPERSPECTIVECALCED  0x8 //perspective has already been calculated, no need to do it again
#define naSELECTED 0x10 //if set, this vertex is selected
#define naCURRENT 0x20 //if set, this vertex is the current vertex
#define naAFFECTEDBYSELECTION         0x40 //if set, it's not selected but it's normal will need updating
#define naREFERENCEDBYSELECTEDPOLYGON 0x80 //This vertex is inside a selected vertex
#define naREFERENCEDBYCURRENTPOLYGON 0x200 //a selected polygon references this vertex
#define naAFFECTEDBYCURRENT 0x400  //a polygon conected to current vertex contains this vertex
#define naTRANSFORMCALCED 0x1000
#define naENABLED 0x2000
#define naDELETEME 0x4000
#define naINCLUDEDINEDGE 0x8000
#define naEXTRUSION    0x10000
#define naEXTRUSIONCAP 0x20000

#define FLT_LOWBOUND 0.01f

class trvertex
{
    public:
	trvertex *next;
	UINT32 attribute;
    vector data3d;
	vector4 proj4d;    
    vector normal;        
};

typedef void (*drawscanlinefunc) (char *adrs,UINT32 count);

class trivert
{
    public:
    float x,y,z,w,r,g,b,a,u,v,nx,ny,nz;
    void assign(float x_,float y_,float z_){x=x_;y=y_;z=z_;};
    void assign(float x_,float y_){x=x_;y=y_;};
    void normal(float x_,float y_,float z_){nx=x_;ny=y_;nz=z_;};

    void colour(float r_,float g_,float b_,float a_){r=r_*65536.0f;g=g_*65536.0f;b=b_;a=a_*65536.0f;};
    void uv(float u_,float v_){u=u_*65536.0f;v=v_*65536.0f;};
};


class trlight
{
public:
	UINT32 attribute;
	vector4 p;
	trgbafloat ambient,diffuse,specular;
};

class trmaterial
{
public:
    trgbafloat ambient,diffuse,specular,emmisive;
    UINT32 specpower;
    flt st; //shine strength.. replacing the '2.0f' of r'=(2.0f*n'*dp)-o' maybe incorrect
    flt transparency;
};

class matrix
{
    public:
    flt m00,m01,m02,m03,
        m10,m11,m12,m13,
        m20,m21,m22,m23,
		m30,m31,m32,m33;
	operator flt *() {return &m00;}

	vector *r1(){return (vector *)&m00;}
	vector *r2(){return (vector *)&m10;}
	vector *r3(){return (vector *)&m20;}
	vector *r4(){return (vector *)&m30;}
	vector *p(){return (vector *)&m30;}
	void set(UINT32 i,UINT32 j,flt v){((float*)&m00)[i+(j<<2)]=v;}
	flt m(UINT32 i,UINT32 j){return ((float*)&m00)[i+(j<<2)];}

    matrix(){m01=m02=m03=m10=m12=m13=m20=m21=m23=m30=m31=m32=0;m00=m11=m22=m33=1;}
    matrix &operator *(flt s)
    {
        static matrix r;flt *f=&r.m00;                              
        for (UINT32 i=0;i<16;i++,f++)
           *f*=s;
        return r;
    }
    matrix &operator /(flt s)
    {
        static matrix r;flt *f=&r.m00;
        s=FLT_1/s;
        for (UINT32 i=0;i<16;i++,f++)
           *f*=s;
        return r;
   }
   /*void addtrans(const vector &v){p.x+=v.x; p.y+=v.y; p.z+=v.z; }
   void settrans(const vector &v){p.x= v.x; p.y= v.y; p.z= v.z; }
   void loadtrans(vector *v){v->x=p.x;v->y=p.y;v->z=p.z;}*/
};


enum TR_surfacetype 
{
    tr_polygon,
	tr_tristrip,
	tr_trifan,
	tr_linlist,
	tr_lineloop
};

//lighting should be perfomed in world space (model matrix)
//clipping will be performed after the projection matrix is set (in -1..1 heomogenous coordinates ala J. Blinn)

enum TR_matrixmode
{
	tr_modelmatrix=0,
	tr_texturematrix=1,
	tr_projmatrix=3
};


class textureref:public telem
{
    public:
    tsurface *t;	
	UINT32 xshift,yshift;
    char name[64];

    textureref *nextt(){return (textureref*)next;}
};    


extern tsurface *t_texture;
extern char *t_texturedataptr;
extern UINT32 *tr_current_texture_startoffsets;
extern tpoint  t_texturesize;
extern float t_texturewidthscale;
extern float t_textureheightscale;


class trender_base
{
public:	
	
};

class trender:public trender_base
{
	public:
	float ascale,bscale,gscale,rscale;
	trivert v_buff[MAXVERTS];
	trlight lights[10];
	trender();
	
	matrix modelmat,texturematrix,invmodelmat,projmat,endmat;
	void beginscene();
		
	void endscene();
	void pers( float fovy, float aspect,float  zNear, float  zFar );
	void frustum(float left, float right, float bottom,float top, float znear, float zfar );
	void ortho(float left,float right,float bottom,float top,float near,float far);

	void surfacetype(TR_surfacetype type);
	void beginsurface();
		void setvertex(trvertex *v);
		void colour(float r,float g,float b,float a);
		void normal(float x,float y,float z);
		void position(float x,float y,float z);
	void endsurface();

	void setcurrentlight(INT32 lightno);
	void setlighttype(INT32 ltype);
	
	void setlightdiffuse(trgbafloat *c);
	void setlightambient(trgbafloat *c);
	void setlightspecular(trgbafloat *c);

	
	void setlightpos(float x,float y,float z,float w);
	void setlightpos(vector4 *p);
	


	void matrixmode(TR_matrixmode mode){c_matrix=&((matrix*)&modelmat)[(INT32)mode];}
	void viewport(INT32 x,INT32 y,INT32 x2,INT32 y2); //pixel coordinates viewport
	void rotatex(float r);
	void rotatey(float r);
	void rotatez(float r);
	void rotate(float x,float y,float z,float a);
	void translate(float x,float y,float z);
	void scale(float x,float y,float z);
	void identity();
	void multmatrix(matrix *mat);
	void loadmatrix(matrix *mat){*c_matrix=*mat;}
	void intensity(trivert *v,vector *p);
	void enable(UINT32 mode){attribute |=mode;}
	void disable(UINT32 mode){attribute &=~mode;}
	void setrendersurface(tsurface *dc);
	void setzbuffersurface(tsurface *dc);
	void setscanmode(UINT32 i);

	matrix *c_matrix;
	
	void line(trivert *v0,trivert *v1);
	void tri(trivert *v0,trivert *v1,trivert *v2);
	void settexture(textureref *tex);
	static UINT32 initialize(trendercontext *ctx);
private:
	UINT32 currentscanmode;	
	drawscanlinefunc drawscanline;
	tsurface *DC3D1,*DC3D2,*DCZ;
	trlight *c_light;

	vector objspace_lookv;
	UINT32 lightcount;
	tpointfloat screen_offset,screen_scale;
	UINT32 nsides;
	UINT32 attribute;
	trect vp;
	trivert *c_vertex;	
	trmaterial c_material;
	vector c_normal;

	tpoint texturesize;
	float texturewidthscale;
	float textureheightscale;
};

class trender_dx:public trender_base
{
public:
	static IDirect3D7 *d3d;
	static IDirect3DDevice7 *d3ddev;
	void beginscene();
		
	void endscene();
	/*void pers( float fovy, float aspect,float  zNear, float  zFar );
	void frustum(float left, float right, float bottom,float top, float znear, float zfar );
	void ortho(float left,float right,float bottom,float top,float near,float far);

	void surfacetype(TR_surfacetype type);
	void beginsurface();
		void setvertex(trvertex *v);
		void colour(float r,float g,float b,float a);
		void normal(float x,float y,float z);
		void position(float x,float y,float z);
	void endsurface();

	void setcurrentlight(INT32 lightno);
	void setlighttype(INT32 ltype);
	
	void setlightdiffuse(trgbafloat *c);
	void setlightambient(trgbafloat *c);
	void setlightspecular(trgbafloat *c);

	
	void setlightpos(float x,float y,float z,float w);
	void setlightpos(vector4 *p);
	


	void matrixmode(TR_matrixmode mode){c_matrix=&((matrix*)&modelmat)[(INT32)mode];}
	void viewport(INT32 x,INT32 y,INT32 x2,INT32 y2); //pixel coordinates viewport
	void rotatex(float r);
	void rotatey(float r);
	void rotatez(float r);
	void rotate(float x,float y,float z,float a);
	void translate(float x,float y,float z);
	void scale(float x,float y,float z);
	void identity();
	void multmatrix(matrix *mat);
	void loadmatrix(matrix *mat){*c_matrix=*mat;}
	void intensity(trivert *v,vector *p);
	void enable(UINT32 mode){attribute |=mode;}
	void disable(UINT32 mode){attribute &=~mode;}
	void setrendersurface(tsurface *dc);
	void setzbuffersurface(tsurface *dc);
	void setscanmode(UINT32 i);
	void line(trivert *v0,trivert *v1);
	void tri(trivert *v0,trivert *v1,trivert *v2);
	void settexture(textureref *tex);*/
	static UINT32 initialize(trendercontext *ctx);
};


void multmatrix2(matrix *m1,matrix *m2,matrix *result);
void loadidentity(matrix *m);
void translate(float x,float y,float z,matrix *m);

#endif