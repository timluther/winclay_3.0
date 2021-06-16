#include "graphics.h"
#include "trender.h"


#define COLHALFSHIFTBY 7
#define COLSHIFTBY 8
#define COLSHIFTBY5BIT 5
#define COLSHIFTBY6BIT 6
#define SHIFTBY 16
#define HALFSHIFTBY 15
#define MULBY 1<<SHIFTBY
#define HLINESIZE sizeof(thline)
#define THLINEOFFSET 1

//for 16bit
//0xF800,0x7E0,0x1F

#define PRESHIFTRMASK16bit /*0x3E0000 0x7C0000*/0xF80000
#define PRESHIFTGMASK16bit /*0x1f800 0x3F000*/0x7E000



textureref *t_textureref;
UINT32 t_tex_xshift;
UINT32 t_tex_xmask;
UINT32 t_tex_ymask;
tsurface *t_texture;
char *t_texturedataptr;

tpoint  t_texturesize;
float t_texturewidthscale;
float t_textureheightscale;


trender::trender()
{	
	c_material.diffuse=1;
	c_material.emmisive=
	c_material.ambient=
	c_material.specular=0;
	c_material.specular=0;
	c_material.st=0;
	viewport(0,0,SC.size.x-1,SC.size.y-1);
	matrixmode(tr_modelmatrix);
	loadidentity(&modelmat);
	loadidentity(&texturematrix);
	loadidentity(&invmodelmat);
	loadidentity(&projmat);
	loadidentity(&endmat);		
	
	attribute=TR_SMOOTH+TR_LIGHTING;
	setscanmode(attribute & 3); 	
	frustum(-1., 1., -1., 1., 1, 640.);
	setcurrentlight(0);
	lightcount=1;
	setlightpos(0,0.0,50,1);
	c_light->diffuse=c_light->specular=1;
	c_light->ambient=0;

	ascale=256*65535;
	bscale=31<<COLSHIFTBY;
	gscale=(63<<5)<<COLSHIFTBY;
	rscale=(31<<11)<<COLSHIFTBY;


	//frustum(float left, float right, float bottom,float top, float znear, float zfar )
}

void trender::setrendersurface(tsurface *dc)
{
	DC3D1=dc;
}

void trender::setzbuffersurface(tsurface *dc)
{
	DCZ=dc;
}



void trender::settexture(textureref *tex)
{    
	if (tex)
    {
		t_texture=tex->t;
	    t_texturedataptr=t_texture->scrptr;		

		
		//[(((xd.v>>7)&0x3FE00))+((xd.u>>15)&510 )]

		//this works for 16bit textures and handles all the shifting down
		//and masking. 
		//NB. Must also handle 8bit and 32bit!
		//16bit
		UINT32 ytopboundmask=((2 << tex->xshift)<<tex->yshift)-1;
		t_tex_xshift=15-tex->xshift;
		UINT32 invshift=16-t_tex_xshift;
		t_tex_ymask=(((1 << invshift)-1)<<invshift)&(ytopboundmask);
		t_tex_xmask=(1 << (tex->xshift+1))-2;
		//8bit
		/*
		t_tex_xshift=16-tex->xshift;
		UINT32 invshift=t_tex_xshift;
		t_tex_ymask=((1 << invshift)-1)<<invshift;
		t_tex_xmask=1 << (tex->xshift)-1;
		*/
		
	    t_texturesize=t_texture->size;
		t_textureheightscale=(t_texture->size.y-1)*65536.0f;
		t_texturewidthscale=(t_texture->size.x-1)*65536.0f;
	}
	else
		t_texture=NULL;
}

void trender::pers( float fovy, float aspect,float  zNear, float  zFar )
{
   float  xmin, xmax, ymin, ymax;

   ymax = zNear * (float)tan( fovy * PI / 360.0 );
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

   frustum( xmin, xmax, ymin, ymax, zNear, zFar );
}


void trender::ortho(float l,float r,float b,float t,float n,float f)
{	float r_l=r-l;
	float t_b=t-b;
	float f_n=f-n;
	
	projmat.m00=2/(r_l)  ; projmat.m01=0;            projmat.m02=0;         projmat.m03=-((r+l)/(r_l));
	projmat.m10=0        ; projmat.m11=2/(t_b)      ;projmat.m12=0;         projmat.m13=-((t+b)/(t_b));
	projmat.m20=0        ; projmat.m21=0;            projmat.m22=-2/(f_n);  projmat.m23=-((f+n)/(f_n));
	projmat.m30=0        ; projmat.m31=0;            projmat.m32=0;         projmat.m33=-1;
	
}

void trender::frustum(float l, float r, float b,float t, float n, float f )
{	
	float n2=n*2;
	float r_l=r-l;
	float t_b=t-b;
	float f_n=f-n;
	projmat.m00=n2/r_l;    projmat.m01=0;           projmat.m02=(r+l)/r_l; projmat.m03=0;
	projmat.m10=0        ; projmat.m11=n2 / t_b;    projmat.m12=(t+b)/t_b; projmat.m13=0;
	projmat.m20=0        ; projmat.m21=0;           projmat.m22=(f+n)/f_n; projmat.m23=(f*n2)/f_n;
	projmat.m30=0        ; projmat.m31=0;           projmat.m32=-1;        projmat.m33=0;
}


//end surface, vertex, postion and various other primative related functions will rely on
//function pointers later, set when you select the surface type

void trender::surfacetype(TR_surfacetype type)
{
	
	
}

void trender::beginsurface()
{
	//combine matrices here??
	c_vertex=v_buff;
	nsides=0;
}

void trender::beginscene()
{
	multmatrix2(&projmat,&modelmat,&endmat);	
}


void trender::endscene()
{
}


void projectv(vector4 &pv,vector &v,matrix &m)
{
	pv.x=v.x*m.m00 +v.y*m.m01 +v.z*m.m02 + m.m03;
	pv.y=v.x*m.m10 +v.y*m.m11 +v.z*m.m12 + m.m13;
	pv.z=v.x*m.m20 +v.y*m.m21 +v.z*m.m22 + m.m23;
	pv.w=v.x*m.m30 +v.y*m.m31 +v.z*m.m32 + m.m33;
	//m30,m31,m32 are not currently used for anything
	//..pv.w=v.z*m.m32;//?+mat.m33;
}

void projectv(vector4 &pv,vector4 &v,matrix &m)
{
	pv.x=v.x*m.m00 +v.y*m.m01 +v.z*m.m02 + v.w*m.m03;
	pv.y=v.x*m.m10 +v.y*m.m11 +v.z*m.m12 + v.w*m.m13;
	pv.z=v.x*m.m20 +v.y*m.m21 +v.z*m.m22 + v.w*m.m23;
	pv.w=v.x*m.m30 +v.y*m.m31 +v.z*m.m32 + v.w*m.m33;
	//m30,m31,m32 are not currently used for anything
	//..pv.w=v.z*m.m32;//?+mat.m33;
}

void projectv(vector4 &pv,float x,float y,float z,float w,matrix &m)
{
	pv.x=x*m.m00 +y*m.m01 +z*m.m02 + w*m.m03;
	pv.y=x*m.m10 +y*m.m11 +z*m.m12 + w*m.m13;
	pv.z=x*m.m20 +y*m.m21 +z*m.m22 + w*m.m23;
	pv.w=x*m.m30 +y*m.m31 +z*m.m32 + w*m.m33;
	//m30,m31,m32 are not currently used for anything
	//..pv.w=v.z*m.m32;//?+mat.m33;
}


void trender::setcurrentlight(INT32 lightno)
{
	c_light=&lights[lightno];
}

void trender::setlighttype(INT32 ltype)
{
	c_light->attribute = ltype;
}

void trender::setlightdiffuse(trgbafloat *c)
{
	c_light->diffuse=*c;
}


void trender::setlightambient(trgbafloat *c)
{
	c_light->ambient=*c;
}

void trender::setlightspecular(trgbafloat *c)
{
	c_light->specular=*c;
}




void trender::setlightpos(vector4 *p)
{
	projectv(c_light->p,*p,modelmat);
}

void trender::setlightpos(float x,float y,float z,float w)
{
	projectv(c_light->p,x,y,z,w,modelmat);
}



void trender::setvertex(trvertex *v)
{
	//calculate colours
	*((vector*)&c_vertex->nx)=v->normal;
	intensity(c_vertex,&v->data3d);
	if ((v->attribute & naDRAWNALREADY)==0)
	{
		projectv(v->proj4d,v->data3d,endmat);				
	}
	*((vector4*)c_vertex)=v->proj4d;
	//clipme here?

	//divide by w
	flt w_recip=1/c_vertex->w;
	c_vertex->x=(c_vertex->x*w_recip*screen_scale.x)+screen_offset.x;
	c_vertex->y=(c_vertex->y*w_recip*screen_scale.y)+screen_offset.y;
	c_vertex->z*=w_recip;
	c_vertex ++;
}

void trender::colour(float r,float g,float b,float a)
{
	c_material.diffuse.r=r*rscale;
	c_material.diffuse.g=g*gscale;
	c_material.diffuse.b=b*bscale;
	c_material.diffuse.a=a*ascale;
}

void trender::normal(float x,float y,float z)
{
	c_normal.x=x;
	c_normal.y=y;
	c_normal.z=z;
}

void trender::position(float x,float y,float z)
{	
	*((vector*)&c_vertex->nx)=c_normal;
	vector p(x,y,z);
	if (attribute & TR_LIGHTING)
		intensity(c_vertex,&p);	
	else
		*((trgbafloat*)&c_vertex->r)=c_material.diffuse;
	projectv(*((vector4*)c_vertex),p,endmat);						
	//clipme here?

	//divide by w
	flt w_recip=1/c_vertex->w;
	
	//
	c_vertex->x=(c_vertex->x*w_recip*screen_scale.x)+screen_offset.x;
	c_vertex->y=(c_vertex->y*w_recip*screen_scale.y)+screen_offset.y;
	c_vertex->z*=w_recip;	
	c_vertex ++;
	
}

void trender::endsurface()
{
	//and here's where we render or store in a sorting list	
	nsides=c_vertex-v_buff;
	UINT32 id=0;
	if (nsides>3)
    {
        for (;id<(nsides >> 1)-1;id++)
        {				
            tri(&v_buff[id],&v_buff[id+1],&v_buff[nsides-id-1]);
            tri(&v_buff[id+1],&v_buff[nsides-id-2],&v_buff[nsides-id-1]);                            
        }                                                               
    }
	if ((nsides & 1)==1) 
	{
		tri(&v_buff[id],&v_buff[id+1],&v_buff[id+2]);
	}
}


void loadidentity(matrix *m)
{
	m->m00=m->m11=m->m22=m->m33=1;

	m->m01=m->m02=m->m03=
	m->m10=m->m12=m->m13=
	m->m20=m->m21=m->m23=
	m->m30=m->m31=m->m32=0;
}

void translate2(float x,float y,float z,matrix *m)
{
/*	matrix tmat;
	loadidentity(&tmat);
	tmat.m03=x;
	tmat.m13=y;
	tmat.m23=z;

	matrix temp=*m;
	multmatrix2(m,&tmat,m);*/
	//float m03,m13,m23;


	m->m03=(m->m00*x + m->m01*y + m->m02*z + m->m03);
	m->m13=(m->m10*x + m->m11*y + m->m12*z + m->m13);
	m->m23=(m->m20*x + m->m21*y + m->m22*z + m->m23);
	m->m33=(m->m30*x + m->m31*y + m->m32*z + m->m33);

	/*m->m03=(m->m00*x + m->m01*y + m->m02*z + m->m03);
	m->m13=(m->m10*x + m->m11*y + m->m12*z + m->m13);
	m->m23=(m->m20*x + m->m21*y + m->m22*z + m->m23);
	m->m33=(m->m30*x + m->m31*y + m->m22*z + m->m33);*/
}

void trender::viewport(INT32 x,INT32 y,INT32 x2,INT32 y2)
{
	vp.rassign(x,y,x2,y2);
	flt aspect=(x2-x)/(flt)(y2-y);
	screen_scale.x=(x2-x)*0.5f;
	screen_scale.y=(y2-y)*0.5f;

	screen_offset.x=screen_scale.x;
	screen_offset.y=screen_scale.y;
	screen_scale.y*=aspect;
}

void trender::rotatex(float r)
{
}

void trender::rotatey(float r)
{
	flt cr=(flt)cos(r);
	flt sr=(flt)sin(r);

    float m01=c_matrix->m01;
    float m11=c_matrix->m11;
    float m21=c_matrix->m21;
    float m31=c_matrix->m31;

    c_matrix->m01=(m01*cr + c_matrix->m02*-sr );
    c_matrix->m11=(m11*cr + c_matrix->m12*-sr );
    c_matrix->m21=(m21*cr + c_matrix->m22*-sr );
    c_matrix->m31=(m31*cr + c_matrix->m32*-sr );

    c_matrix->m02=(m01*sr + c_matrix->m02*cr );
    c_matrix->m12=(m11*sr + c_matrix->m12*cr );
    c_matrix->m22=(m21*sr + c_matrix->m22*cr );
    c_matrix->m32=(m31*sr + c_matrix->m32*cr );
}

void trender::rotatez(float r)
{
    flt cr=(flt)cos(r);
    flt sr=(flt)sin(r);
    
	//copy corruptable values
	flt m00=c_matrix->m00;
	flt m10=c_matrix->m10;
	flt m20=c_matrix->m20;
	flt m30=c_matrix->m30;

	c_matrix->m00=(m00*cr + c_matrix->m01*-sr);
	c_matrix->m10=(m10*cr + c_matrix->m11*-sr);
	c_matrix->m20=(m20*cr + c_matrix->m21*-sr);
	c_matrix->m30=(m30*cr + c_matrix->m31*-sr);

	c_matrix->m01=(m00*sr + c_matrix->m01*cr);
	c_matrix->m11=(m10*sr + c_matrix->m11*cr);
	c_matrix->m21=(m20*sr + c_matrix->m21*cr);
	c_matrix->m31=(m30*sr + c_matrix->m31*cr);
}

void trender::rotate(float x,float y,float z,float a)
{
}
void trender::translate(float x,float y,float z)
{
	translate2(x,y,z,c_matrix);
}

void trender::scale(float x,float y,float z)
{

}
void trender::identity()
{
	loadidentity(c_matrix);
}

//	matrix4x4ptr LMconcatenatematrices(register matrix4x4ptr m1,register matrix4x4ptr m2,matrix4x4ptr result)


void multmatrix3(matrix *m1, matrix *m2, matrix *r)
{
   int i,j;
   for(i=0; i<4; i++)
      for(j=0; j<4; j++)
         r->set(i,j,m1->m(i,0)*m2->m(0,j)+
                    m1->m(i,1)*m2->m(1,j)+
                    m1->m(i,2)*m2->m(2,j)+
                    m1->m(i,3)*m2->m(3,j));
}

void multmatrix2(matrix *m1,matrix *m2,matrix *result)
{  
  //check the pointers, if they are the same, DO SOMETHING! 
  //if (m1==m2) return m1;  


  result->m00=(m1->m00*m2->m00 + m1->m01*m2->m10 + m1->m02*m2->m20 + m1->m03*m2->m30);
  result->m10=(m1->m10*m2->m00 + m1->m11*m2->m10 + m1->m12*m2->m20 + m1->m13*m2->m30);
  result->m20=(m1->m20*m2->m00 + m1->m21*m2->m10 + m1->m22*m2->m20 + m1->m23*m2->m30);
  result->m30=(m1->m30*m2->m00 + m1->m31*m2->m10 + m1->m32*m2->m20 + m1->m33*m2->m30);

  result->m01=(m1->m00*m2->m01 + m1->m01*m2->m11 + m1->m02*m2->m21 + m1->m03*m2->m31);
  result->m11=(m1->m10*m2->m01 + m1->m11*m2->m11 + m1->m12*m2->m21 + m1->m13*m2->m31);
  result->m21=(m1->m20*m2->m01 + m1->m21*m2->m11 + m1->m22*m2->m21 + m1->m23*m2->m31);
  result->m31=(m1->m30*m2->m01 + m1->m31*m2->m11 + m1->m32*m2->m21 + m1->m33*m2->m31);

  result->m02=(m1->m00*m2->m02 + m1->m01*m2->m12 + m1->m02*m2->m22 + m1->m03*m2->m32);
  result->m12=(m1->m10*m2->m02 + m1->m11*m2->m12 + m1->m12*m2->m22 + m1->m13*m2->m32);
  result->m22=(m1->m20*m2->m02 + m1->m21*m2->m12 + m1->m22*m2->m22 + m1->m23*m2->m32);
  result->m32=(m1->m30*m2->m02 + m1->m31*m2->m12 + m1->m32*m2->m22 + m1->m33*m2->m32);

  result->m03=(m1->m00*m2->m03 + m1->m01*m2->m13 + m1->m02*m2->m23 + m1->m03*m2->m33);
  result->m13=(m1->m10*m2->m03 + m1->m11*m2->m13 + m1->m12*m2->m23 + m1->m13*m2->m33);
  result->m23=(m1->m20*m2->m03 + m1->m21*m2->m13 + m1->m22*m2->m23 + m1->m23*m2->m33);
  result->m33=(m1->m30*m2->m03 + m1->m31*m2->m13 + m1->m32*m2->m23 + m1->m33*m2->m33);

}

void trender::multmatrix(matrix *m2)
{
  static matrix m1=*c_matrix;
  multmatrix2(&m1,m2,c_matrix);
}

void trender::intensity(trivert *v,vector *p)
{
    flt specdot=FLT_0;
    trlight *l;
    flt dp;    
    vector o,r;        
    v->r=0;v->g=0;v->b=0;v->a=0;
  
    for (UINT32 i=0;i<lightcount;i++)
    {
		 l=&lights[i];        
		 if (l->p.w==0)
         		o=*(vector*)&l->p;								
		 else
		 {
				o=(*(vector*)&l->p)-*p;                
				o.normalize();		
		 }        
         dp=dot((vector*)&v->nx,&o);                                 
         specdot=FLT_0;
         if (dp>FLT_0) //any light here?
         {            
			//calculate reflection vector			

			float dp2=dp*2;

			r.x= (dp2* v->nx)-o.x;
			r.y= (dp2* v->ny)-o.y;
			r.z= (dp2* v->nz)-o.z;
			specdot=dot(r,objspace_lookv);						
			if ((specdot>FLT_0))							
				specdot=ipow(specdot,c_material.specpower)*c_material.st;								
			else
				specdot=FLT_0;
            
            v->r+=l->ambient.r+(l->ambient.r*c_material.ambient.r)+(l->diffuse.r*c_material.diffuse.r*(float)dp)+((l->specular.r*c_material.specular.r)*(float)specdot);
            v->g+=l->ambient.g+(l->ambient.g*c_material.ambient.g)+(l->diffuse.g*c_material.diffuse.g*(float)dp)+((l->specular.g*c_material.specular.g)*(float)specdot);
            v->b+=l->ambient.b+(l->ambient.b*c_material.ambient.b)+(l->diffuse.b*c_material.diffuse.b*(float)dp)+((l->specular.b*c_material.specular.b)*(float)specdot);
			//v->a=????
         }else
         {               
             v->r+=l->ambient.r+(l->ambient.r*c_material.ambient.r);
             v->g+=l->ambient.g+(l->ambient.g*c_material.ambient.g);
             v->b+=l->ambient.b+(l->ambient.b*c_material.ambient.b);        
         }        
    }
   
    BOUND(v->r,FLT_LOWBOUND,FLT_1);
    BOUND(v->g,FLT_LOWBOUND,FLT_1);
    BOUND(v->b,FLT_LOWBOUND,FLT_1);
	v->r*=rscale;
	v->g*=gscale;
	v->b*=bscale;
	v->a*=ascale;
}


drawscanlinefunc (*setscanmode)(UINT32 mode);



//The purpose of this file is to try and collect the messy, shared global variables of the 
//low level graphics drivers.
//Hopefully I can reduce this mostly to defines in the future.

typedef struct rgb512
{
    UINT32 r[512];
    UINT32 g[512];
    UINT32 b[512];    
}rgb512;

typedef struct rgb1024
{
    UINT32 r[1024];
    UINT32 g[1024];
    UINT32 b[1024];    
}rgb1024;

//these are global in here so they can be used in asm routines without hassle

//current maximum resolution, up it when better display h/w comes out :)

//temporary storage double word
UINT32 tempint;
//used for offsets into transparency table
UINT32 pa1,pa2;
INT32 wcx;      //width of polyscanline.. maybe <0 so HAS to be signed!
tcolour pix;   //for texel
tcolour pixin; //for screen read

//these perform colour clipping and convertion from 24 to 16bit colour
rgb1024 bound0to256_16;
rgb512 conv24to16;
trgba conv16to24[65536];
UINT32 bound0to256[1024];

//tri scanning structures
/*struct coordinterp_
{
      
      INT32 xl,dxl,xr,dxr,yred,dyred,ygreen,dygreen,yblue,dyblue,ya,dya,yu,dyu,yv,dyv,yz,dyz;      
}ic;*/

typedef struct coordinterp_
{    
      INT32 xl,dxl,xr,dxr,yred,dyred,ygreen,dygreen,yblue,dyblue,ya,dya,yu,dyu,yv,dyv,yz,dyz;      
}coordinterp;

typedef struct xdeltas_
{
      INT32 r,dr,g,dg,b,db,a,da,u,du,v,dv,z,dz;      
}xdeltas;

coordinterp ic;
xdeltas xd;

#include "r3d565.cpp"


UINT32 trender::initialize(trendercontext *ctx)
{
	buildtables();
	return 1;
}

void trender::setscanmode(UINT32 mode)
{
	if (mode!=currentscanmode)
	{
		currentscanmode=mode;
		switch(mode)
		{
			case 0:drawscanline=sl_f;break;
			case 1:drawscanline=sl_g;break;
			case 2:drawscanline=sl_ft16;break;
			case 3:drawscanline=sl_gt16;break;
			case 4:drawscanline=sl_fa;break;
			case 5:drawscanline=sl_ga;break;
			case 6:drawscanline=sl_ft16a;break;
			case 7:drawscanline=sl_gt16a;break;        
		}
	}	
}

#define calcslopecoordinates(diff,val,startval,v)\
(ic.diff=float2fixed((v->val-v0->val)*invheight);\
imul(ic.startval,prestep,ic.diff);\
ic.startval += float2fixed(v0->val);)

#define calcslopecoordinates2()\
ic.dxr=float2fixed((v2->x-v0->x)*invheight);\
imul(ic.xr,prestep,ic.dxr);\
ic.xr+=float2fixed(v0->x);\

#define calcslope(diff,val,start,v)\
ic.diff=float2int((v->val-v0->val)*invheight);\
imul(ic.start,prestep,ic.diff);\
ic.start+=float2int(v0->val);

#define calcslopeprecalced(diff,val,start,precalced)\
ic.diff=float2int(precalced*invheight);\
imul(ic.start,prestep,ic.diff);\
ic.start+=float2int(v0->val);


inline int getoutcodes(vector4 *v,float *bc)
{
	int code=0;
	if ((bc[0]=v->x)>0)		 code |= 1;
	if ((bc[1]=v->x-v->w)>0) code |= 2;
	if ((bc[2]=v->y)>0)		 code |= 4;
	if ((bc[3]=v->y-v->w)>0) code |= 8;
	if ((bc[4]=v->z)>0)		 code |= 16;
	if ((bc[5]=v->z-v->w)>0) code |= 32;
	return code;
}

int cliplist(trivert *vin,trivert *vout,UINT32 count)
{
	float bcodes[12];
	float *bc0,*bc1;
	int outcount=0;
	bc0=bcodes;
	bc1=&bcodes[6];
	int code0,code1;
	UINT32 i;
	code0=getoutcodes((vector4*)&vin[0].x,bc0);
	if (!code0)
	{
		*vout=*vin;
		vout++;
	}

	for (i=1;i<count;i++);
	{		
		code1=getoutcodes((vector4*)&vin[i].x,bc1);
		if (code0 & code1)
			if (code0 | code1)
			{
				*vout=vin[i];
				vout++;
			}
			else
			{
				int code=code0 | code1;
				float delta0=0;
				float delta1=1;
				int mask=0x1;
				for (int i=0;i<6;i++)
				{
					float delta=bc0[i]/(bc0[i] - bc1[i]);
					if (code0 & mask)
						delta0= MAX(delta0,delta);
					else
						delta0= MAX(delta1,delta);
					if (delta1 < delta0)
						goto nontrivreject;
					mask <<=1;
				}
				if (code0!=0)
				{
					//vin[i].x=
					//perform clipping

				}
				if (code1!=0)
				{
					//perform clipping

				}				
nontrivreject:;
			}				
	}


}


void trender::line(trivert *v0,trivert *v1)
{
}

void trender::tri(trivert *v0,trivert *v1,trivert *v2)     
{       
   //long dxr,dxl,xl,xr;
   
   static UINT32 ti;
   static UINT32 scanlines;
   static float v0yceil;
   static float height,invheight;
   static float d,id;
   static int prestep;  
   static INT32 soffs,soffsend;
   static float diffy1_y2,diffy0_y2,diffr1_r2,diffr0_r2,diffg1_g2,diffg0_g2,diffb1_b2,diffb0_b2;
   static trivert *tempv;
   register UINT32 offsd=DC3D1->offsd;
   
   //sort
   if (v0->y>v2->y) {tempv=v0;v0=v2;v2=tempv;}           
   if (v0->y>v1->y) {tempv=v0;v0=v1;v1=tempv;}           
   if (v1->y>v2->y) {tempv=v1;v1=v2;v2=tempv;}
   
   //scanlines=v2->y-v0->y;

   diffy1_y2=v1->y-v2->y;
   diffy0_y2=v0->y-v2->y;

   d = (v0->x - v2->x) * diffy1_y2 - (v1->x - v2->x) * diffy0_y2;
   if (d==0.0)
      return;

   height=v2->y-v0->y;        //mov di,yc;sub di,ya
   
   diffr1_r2=v1->r-v2->r;
   diffr0_r2=v0->r-v2->r;

   diffg1_g2=v1->g-v2->g;
   diffg0_g2=v0->g-v2->g;

   diffb1_b2=v1->b-v2->b;
   diffb0_b2=v0->b-v2->b;
   
   id= 1.0f/d;   
   v0yceil=fceil(v0->y);
                   
   UINT16 *sbase=(UINT16*)DC3D1->scrptr+(int)(v0yceil*(offsd>>1));
   //should do this in asm
   //prestep = float2fixed((ceil(v0->y)-v0->y));
   prestep = float2fixed(v0yceil-v0->y);
   xd.dr=float2int((( diffr0_r2*diffy1_y2)-(diffr1_r2*diffy0_y2))*id);
   xd.dg=float2int((( diffg0_g2*diffy1_y2)-(diffg1_g2*diffy0_y2))*id);
   xd.db=float2int((( diffb0_b2*diffy1_y2)-(diffb1_b2*diffy0_y2))*id);
   xd.da=float2int((( (v0->a-v2->a)*diffy1_y2)-((v1->a-v2->a)*diffy0_y2))*id);
   xd.du=float2int((( (v0->u-v2->u)*diffy1_y2)-((v1->u-v2->u)*diffy0_y2))*id);
   xd.dv=float2int((( (v0->v-v2->v)*diffy1_y2)-((v1->v-v2->v)*diffy0_y2))*id);   
   invheight = 1.0f/height;

   if (d>0)                
   {               
      ic.xl  =    float2fixed(v0->x)+ imul2(prestep,(ic.dxl = float2fixed(((v2->x-v0->x)*invheight))));
	  ic.yred   = float2int(v0->r)+ imul2(prestep,(ic.dyred=   float2int(-diffr0_r2*invheight)));
      ic.ygreen = float2int(v0->g)+ imul2(prestep,(ic.dygreen= float2int(-diffg0_g2*invheight)));
      ic.yblue=   float2int(v0->b)+ imul2(prestep,(ic.dyblue=  float2int(-diffb0_b2*invheight)));
      ic.ya   =   float2int(v0->a)+ imul2(prestep,(ic.dya =    float2int((v2->a-v0->a)*invheight)));
      ic.yu   =   float2int(v0->u)+ imul2(prestep,(ic.dyu=     float2int((v2->u-v0->u)*invheight)));
      ic.yv   =   float2int(v0->v)+ imul2(prestep,(ic.dyv=     float2int((v2->v-v0->v)*invheight)));

   }else           
   {	  	        	  	  
	  ic.xr=float2fixed(v0->x)+imul2(prestep,(ic.dxr=float2fixed((v2->x-v0->x)*invheight)));	        
   }    
                                       
   for (ti=0;ti<2;ti++)
   {       
       //calculate difference. changes at midpoint, hence the outer loop. much better than checking each scanline
       height=(v1->y-v0->y);
       scanlines=(INT32)fgetscanlines(v1->y,v0->y);//ceil(v1->y)-ceil(v0->y);
       
              
       if (height>0)
       {
       //sprintf(buf,"Scanline count:%i",scanlines);
       //outtextxy(10,yc+=16,buf);
          prestep = float2fixed(SUB_PIX(v0->y));
          //prestep = float2fixed((ceil(v0->y)-v0->y));
		  invheight = 1.0f/height;
          if (d>0)                
          {                                                                     
             ic.dxr = float2fixed(((v1->x-v0->x)*invheight));      
             ic.xr  = float2fixed(v0->x)+ imul2(prestep,ic.dxr);      
          }else           
          {
             
             ic.dxl    = float2fixed(((v1->x-v0->x)*invheight));      
             ic.xl     = float2fixed(v0->x)+ imul2(prestep,ic.dxl);
     
             ic.yred   = float2int(v0->r)+ imul2(prestep,(ic.dyred=   float2int(((v1->r-v0->r)*invheight))));
             ic.ygreen = float2int(v0->g)+ imul2(prestep,(ic.dygreen= float2int(((v1->g-v0->g)*invheight))));
             ic.yblue  = float2int(v0->b)+ imul2(prestep,(ic.dyblue=  float2int(((v1->b-v0->b)*invheight))));
             ic.ya   =   float2int(v0->a)+ imul2(prestep,(ic.dya =    float2int(((v1->a-v0->a)*invheight))));
             ic.yu     = float2int(v0->u)+ imul2(prestep,(ic.dyu=     float2int(((v1->u-v0->u)*invheight))));
             ic.yv     = float2int(v0->v)+ imul2(prestep,(ic.dyv=     float2int(((v1->v-v0->v)*invheight))));
          }
          while (scanlines>0)
          {                                               
              soffs=iceil2(ic.xl);
              soffsend=iceil2(ic.xr)-soffs;
              ic.xl+=ic.dxl;
              ic.xr+=ic.dxr;
              //pixel plotting loop;
              xd.r=ic.yred;
              ic.yred+=ic.dyred;
              xd.g=ic.ygreen;
              ic.ygreen+=ic.dygreen;
              xd.b=ic.yblue;
              ic.yblue+=ic.dyblue;              
              xd.a=ic.ya;
              ic.ya+=ic.dya;
              xd.u=ic.yu;
              ic.yu+=ic.dyu;
              xd.v=ic.yv;
              ic.yv+=ic.dyv;              
              //drawscanline			  
              if (soffsend >0)
                 drawscanline((char*)sbase+(soffs << 1),soffsend);           
              
              sbase+=offsd >> 1;
              scanlines--;
          }
     }                                
     //shift down to next set of vals
     v0=v1;
     v1=v2;
     }
}
