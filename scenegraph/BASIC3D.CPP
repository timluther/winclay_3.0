#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "gl\gl.h"

#include "logger.h"
#include "graphics.h"
#include "views.h"

#include "trender.h"
#include "basic3d.h"
#include "gprim.h"
//#include "svga64k.hpp"


UINT32 elementselected[]={naSELECTED,eaSELECTED,paSELECTED,ofSELECTED};
UINT32 elementenabled[]={naENABLED,eaENABLED,paENABLED,ofENABLED};
UINT32 elementiscurrent[]={naCURRENT,eaCURRENT,paCURRENT,ofCURRENT};

char nodebm[5]={124,68,68,68,124}; //pattern for nodes
//
trender cmds;

tpointflt T3Dtxvecspool[255];

/*

  if (surface.normal.z > surface.normal.y && surface.normal.y > surface.normal.x)
  {
  for (each vertex i)
  {
  surface.lightmap[i].u = surface.vertex[i].x;
  surface.lightmap[i].v = surface.vertex[i].y;
  }
  }
  
	if (surface.normal.y > surface.normal.x && surfamal.x > surface.normal.z)
	{ce.nor
	for (each vertex i)
	{
	surface.lightmap[i].u = surface.vertex[i].z;
	surface.lightmap[i].v = surface.vertex[i].x;
	}
	}
	
	  if (surface.normal.x > surface.normal.z && surface.normal.z > surface.normal.y)
	  {
	  for (each vertex i)
	  {
	  surface.lightmap[i].u = surface.vertex[i].y;
	  surface.lightmap[i].v = surface.vertex[i].z;
	  }
	  }
	  
		
		  
*/


#if defined doublefloatpresicion
#define FLT_TWEAK 0.05
#define FLT_013 0.13
#else
#define FLT_TWEAK 0.05f
#define FLT_013 0.13f
#endif


char T3D_TEXTURE_PATHS[1024];
char TEXTURE_PATH[255];
char CURRENT_MODEL_PATH[255];

#define CLIP_PLANE_EPSILON  0.00000001f      //fudge factor for clipplanes
#define NUM_FRUSTUM_PLANES 6          //the amount of clip planes


tpolygon **T3Dvisiblepolygons,**T3Dcvisiblepoly,**T3Dlastpolygon; //visible polygon list
tedge **T3Dvisibleedges,**T3Dcvisibleedge;
UINT32 T3Dmaxedges,T3Dedgecount;
UINT32 T3Dmaxpolys,T3Dpolycount,T3Dselpolycount,T3Dtotalpolys;    //maximum polygon that can be displayed
UINT32 T3Dmaxpolyedges,T3Dmaxdrawnedges;
theapmem tempspace; //this will be used for clipped polygons and other stuff that's cleared down every frame.

//GLOBALS, mostly concearned with object creation, a bit like openGL.
//In other words, it's not badly coded, it's just a state machine!

char T3Dtempstr[256];
void (*T3Daddtexturecallback)(textureref *tref)=NULL;
void (*T3Daddmaterialcallback)(material *mat)=NULL;
tpoint T3Dsize;
UINT32 T3Dsceneattributes=sfSPECULARHIGHLIGHTS|sfONLYSHOWSELECTEDNODES;   //global scene attributes


//polycreation flags controlsetup parameters,attributes flags determine the properties
UINT32 T3Dpolycreationflags=cfANTICLOCKWISE;
UINT32 T3Ddefaultpolyattributes=paENABLED;
UINT32 T3Ddefaultedgeattributes=eaENABLED;
UINT32 T3Ddefaultvertexattributes=naENABLED;
UINT32 T3Ddefaultobjectattributes=ofENABLED|ofVISIBLE|ofEDITABLE;

tpolygon *T3Dselectedpoly=NULL; //currently selected poly
tpolygon *T3Dlastpolycreated=NULL;  //the last polygon that was created and added to an object
UINT32 T3Dobjecttypemask=0xFFFFFFFF;
UINT32 T3Dobjectprocessmask=ofENABLED; //if a bit test between the object attribute and this mask !=T3Dobjectprocessmaskresult, process the object
UINT32 T3Dobjectprocessmaskresult=ofENABLED;
UINT32 T3Dpolygonprocessmask=paENABLED; //if a bit test between the object attribute and this mask !=T3Dobjectprocessmaskresult, process the object
UINT32 T3Dpolygonprocessmaskresult=paENABLED;
UINT32 T3Dedgeprocessmask=eaENABLED; //if a bit test between the object attribute and this mask !=T3Dobjectprocessmaskresult, process the object
UINT32 T3Dedgeprocessmaskresult=eaENABLED;
UINT32 T3Dvertexprocessmask=naENABLED; //if a bit test between the object attribute and this mask !=T3Dobjectprocessmaskresult, process the object
UINT32 T3Dvertexprocessmaskresult=naENABLED;
UINT32 T3Dpolygonclipped;

tcolour T3DCOLprojector;
tcolour T3DCOLgridmajor;
tcolour T3DCOLgrid;
tcolour T3DCOLwire;        //colour of wireframe polygons
tcolour T3DCOLselectedwire;//colour of selected wireframe polygons
tcolour T3DCOLselectedwireedge;
tcolour T3DCOLextrusionbody;
tcolour T3DCOLextrusionedge;
tcolour T3DCOLboundingbox; //bounding box colour
tcolour T3DCOLcursor;      //cursor colour
tcolour T3DCOLnode;        //node colour
tcolour T3DCOLselectednode;//selected node colour
flt T3Dresistance=0.5f;    //air resistance
flt T3Dspecdp=FLT_0;        //internal, used in lighting calcs
vector T3Dlookv;           //the current view direction, valid after a call to tscene::preprocess 

vector T3Dbbcentreofrotation;
flt T3Dscale=FLT_1;          //Scaling factor in orthographic viws
UINT8 T3Dcliphint;         //Clipping hint; generated from the sphere/frustum test
matrix4x4 T3Dviewmat;      //current view matrix

matrix4x4 UNITMAT;         //unit matrix
material WHITEMATERIAL;    //standard white material
tpointflt T3Doffset;           //x/y integer offset into the centre of the screen
matrix4x4 *T3Dctm;         //Current transformation matrix
textureref *T3Dtexture=NULL;       //current texture
material *T3Dmaterial=NULL;     //current material
material *T3Dmaterialbg;   //current background material

tlinkedlist T3Dtexturebank;  //list of all textures
tlinkedlist T3Dmaterialbank; //list of all materials

flt FOV;                   //current field of vision, valid after a call to tscene::preprocess
tpointflt T3Dscreenscale,T3Dscreencentre; //2d scren scaling and centre
flt T3Dmaxscale;                            //biggest of x/y in screen scale
flt T3Dmaxscreenscaleinv;                   //FLT_1 / T3Dmaxscale. these are used to get the aspect ratio right

flt NEARZ;                                  //near clipping plane
flt FARZ;                                  //near clipping plane
tplane frustumplanes[6];                    //the frustum planes
trectflt T3Dviewport;
//NUM_FRUSTUM_PLANES

trgbfloat T3Dambientdefault=trgbfloat(0.1f,0.1f,0.1f);  //default ambient, specular and emmisive values
trgbfloat T3Dspeculardefault=trgbfloat(FLT_1,FLT_1,FLT_1);
trgbfloat T3Demmisivedefault=trgbfloat(FLT_0,FLT_0,FLT_0);

//_Packed




//////////////////////////////////////////////////////////////////////////////////////////
//Deletion callback functions

//unfortunatly, this needs globals.
//However, the removelots function is neat. it's faster than removing one element at a time
//and encapsulating the code inside the linkedlist class makes it smaller and neater.


void T3Dbuildplanecube(tplane *planes,vector *v1,vector *v2)
{
	planes[0].normal.moveto(1,0,0);
	planes[0].distance=v1->x;
	planes[1].normal.moveto(0,1,0);
	planes[1].distance=v1->y;
	
	planes[2].normal.moveto(-1,0,0);
	planes[2].distance=-v2->x;//v2->x;
	
	planes[3].normal.moveto(0,-1,0);
	planes[3].distance=-v2->y;//v2->y;
	planes[4].normal.moveto(0,0,1);
	planes[4].distance=v1->z;//55;
	planes[5].normal.moveto(0,0,-1);
	planes[5].distance=-v2->z;//-60;
}


UINT32 gbl_mask;

bool checkremovevertexfunc(telem *celem)
{
    return ((((vertex*)celem)->attribute & gbl_mask)==gbl_mask);
}


bool removevertexfunc(telem *celem)
{
    //((vertex*)celem)->deletereferences();
    
    
    //add celem to vertex restore list here
    return TRUE;
}

//edges
bool checkremoveedgefunc(telem *celem)
{
    return ((((tedge*)celem)->attribute & gbl_mask)==gbl_mask);
}

bool removeedgefunc(telem *celem)
{
    ((tedge*)celem)->deletereferences();
    
    //add celem to vertex restore list here
    return TRUE;
}

//polygons

bool checkremovepolygonfunc(telem *celem)
{
    return (((tpolygon*)celem)->attribute & gbl_mask)==gbl_mask;
}

bool removepolygonfunc(telem *celem)
{
    ((tpolygon*)celem)->removeedgereferences();
    //add celem to polygon restore list here
    
    return TRUE;
}


//////////////////////////////////////////
// Engine Initialization and shutdown


//this will do something slightly more useful...
UINT32 T3Dseterror(UINT32 error)
{
    return error;
}

UINT32 tpolyvertex::checkintegrity()
{
    if (!e->contains(v)) return pviNOVERTEXREFERENCEINEDGE;
    if (e->v[attribute & pvaEDGEISANTICLOCKWISE?1:0]!=v) return pviCLOCKWISEFLAGISINCORRECT;
    
    return eiOK;
}

vertex *T3Dcreatevertex(flt x,flt y,flt z,UINT8 *mem)
{
	vertex *newv=(mem!=NULL)?(vertex*)mem:(vertex*)malloc(sizeof(vertex));
	return newv;
}

tedge *T3Dcreateedge(vertex *v,vertex *v1,UINT8 *mem)
{
    tedge *newe=(mem!=NULL)?(tedge*)mem:(tedge*)malloc(sizeof(tedge));
    newe->v[0]=v;
    newe->v[1]=v1;
    if (!v->anedge)
		v->anedge=newe;    
    if (!v1->anedge)
		v1->anedge=newe;
    newe->writetag(0xFFFFFFFF);
    /*newe->edgeloopref=0;
    newe->edgeloopcount=0;*/    
    newe->pref[0]=newe->pref[1]=NULL;
    //
    newe->attribute=T3Ddefaultedgeattributes;
    return newe;
}



inline UINT32 T3Dpolymemsize(UINT32 npoints)
{
    return sizeof(tpolygon)+(sizeof(tpolyvertex)*(npoints-1));
}

tpolygon *T3Dcreatepolygon(UINT32 npoints,UINT8 *mem)
{    
    tpolygon *retpoly=(mem!=NULL)?(tpolygon*)mem:(tpolygon*)malloc(T3Dpolymemsize(npoints));    
	retpoly->parent=NULL;
    retpoly->tex=T3Dtexture;    
	retpoly->smoothgroup=0;
    retpoly->attribute=T3Ddefaultpolyattributes;
    retpoly->npoints=npoints;    
    return retpoly;        
}
//create polygon from another polygon; it is assumed to be a clipped polygon
tpolygon *T3Dcreatepolygon(tpolygon *inpoly,UINT32 npoints,UINT8 *mem)
{    
    tpolygon *retpoly=(mem!=NULL)?(tpolygon*)mem:(tpolygon*)malloc(T3Dpolymemsize(npoints));
	retpoly->parent=inpoly->parent;
    retpoly->next=(telem*)inpoly;
    retpoly->normal=inpoly->normal;
    //retpoly->tempnormal=inpoly->tempnormal;
    //retpoly->centre=inpoly->centre;
    retpoly->tex=inpoly->tex;    
    retpoly->attribute=inpoly->attribute|paCLIPPEDPOLY;
    retpoly->npoints=npoints;    
	retpoly->smoothgroup=0;
    return retpoly;        
}

//makes a copy of a polygon
tpolygon *T3Dcreatepolygon(tpolygon *inpoly,UINT8 *mem)
{
    UINT32 size=T3Dpolymemsize(inpoly->npoints);
    tpolygon *retpoly=(tpolygon*) ((mem!=NULL)?mem:malloc(size));
    memcpy(retpoly,inpoly,size);
    return retpoly;
}

tpolygon *T3Dreallocpoly(tpolygon *oldp,UINT32 npoints)
{
    tpolygon *p=(tpolygon*)realloc(oldp,T3Dpolymemsize(npoints));
    if (!p) return NULL;    
    //if realloc had to make a new pointer, we have to do this stuff
    if (p!=oldp)
    {
        logmessage( (void*)&textcol, "Realloc failed;pointer has been changed");
        if (T3Dselectedpoly==oldp)
			T3Dselectedpoly=p;
        //go through selected vertices and restate connections
        if ((p->attribute & paSELECTED)!=0)
        {
            //find entry in selected list and restate.
        }
        if (oldp->parent)
        {
            tpolygon *prev=(tpolygon*)p->parent->polygonlist.prev((telem*)oldp);
            if (prev)
				prev->next=p;
        }
    }
    return p;    
}


tpolygon *T3Dsetpolygonvertexuv(tpolygon *p,UINT32 ref,flt u,flt v)//,trgbfloat *col
{
    tpolyvertex *pv=&p->vlist[ref];
    pv->texv.x=u;
    pv->texv.y=v;
    return p;
}

tpolygon *T3Dsetpolygonvertex(tpolygon *p,vertex *v,UINT32 ref,UINT32 attribute,tedge *e)//,trgbfloat *col
{
    if (ref<p->npoints)
    {
		tpolyvertex *pv=&p->vlist[ref];
		tpolyvertex  *other=p->getvertexrefsafe(ref-1);
		
		pv->texv.x=pv->texv.y=FLT_0;
		pv->v=v;
		pv->e=e;       
		pv->attribute=attribute;     
		pv->mat=T3Dmaterial;
		if (e)
		{
			if (e->sameas(pv->v,other->v))
			{
				e->addreference(p);
				if (e->v[0]==v)
					pv->attribute &=~pvaEDGEISANTICLOCKWISE;
				else if (e->v[1]==v)
					pv->attribute |=pvaEDGEISANTICLOCKWISE;
				else
					return NULL;
			}else
			{
				printf("Error, wrong edge");
			}
		}
		return p;
    }else
		return NULL;
}

//this is the routine that is fucking the edges up...
/*tpolygon *T3Dsetpolygonvertex(tpolygon *p,tedge *e,UINT32 ref,UINT32 attribute)//,trgbfloat *col
{
if (ref<p->npoints)
{
tpolyvertex *pv=&p->vlist[ref];
tpolyvertex  *other=p->getvertexrefsafe(ref-1);
pv->texv.x=pv->texv.y=FLT_0;
vertex *v=p->vlist[ref].v=e->v[(attribute & pvaEDGEISANTICLOCKWISE)];
if (!e->sameas(pv->v,other->v))
{
printf("Error, wrong edge");
}
p->vlist[ref].e=e;       
p->vlist[ref].attribute=attribute;     
e->addreference(p);
p->vlist[ref].mat=T3Dmaterial;
return p;
}else
return NULL;
}*/

//returns false if point is less that 0
bool T3Dproject(vector *p3d,tpointflt *p2d,tcamera *cam)
{
    if (cam->camtype==cam3D)
    {
		
		flt temp=p3d->z;
		if (temp<NEARZ)
			return FALSE;
		//FOV can be put in the matrix...
		p2d->x=(float)((FOV*(p3d->x)/temp)+T3Doffset.x);
		p2d->y=(float)((FOV*(p3d->y)/temp)+T3Doffset.y);
    }else
    {       
		p2d->x=(float)(p3d->x);
		p2d->y=(float)(p3d->y);
    }
    return TRUE;
}



void T3Dsortbymaterial(INT32 l,INT32 r)
{
	if (r<0) return;
	tpolygon *c;
	tpolygon **lp,**rp,**base,**oldlp,**oldrp;
	material *lop3;
	//if (lop2<0) return;
	//  if (lop1>T3Dpolycount-1) return;
	
	lop3=T3Dvisiblepolygons[((l+r) >> 1) ]->vlist[0].mat;
	oldlp=lp=&T3Dvisiblepolygons[l];
	oldrp=rp=&T3Dvisiblepolygons[r];
	base=T3Dvisiblepolygons;
	
	while (lp<=rp)
	{
		while ((*lp)->vlist[0].mat>lop3)  {lp++;}
		while (lop3>(*rp)->vlist[0].mat)  {rp--;}
		if (lp<=rp)
		{
			c=*lp;*lp=*rp;*rp=c;
			lp++;rp--;         
		}      
	}
	if (oldlp<rp) T3Dsortbymaterial(l,(INT32)(rp-base));
	if (lp<oldrp) T3Dsortbymaterial((INT32)(lp-base),r);
}

void T3Dsort(INT32 l,INT32 r)
{  
	if (r<0) return;
	tpolygon *c;
	tpolygon **lp,**rp,**base,**oldlp,**oldrp;
	flt lop3;
	//if (lop2<0) return;
	//  if (lop1>T3Dpolycount-1) return;
	
	lop3=T3Dvisiblepolygons[((l+r) >> 1) ]->centre.z;
	oldlp=lp=&T3Dvisiblepolygons[l];
	oldrp=rp=&T3Dvisiblepolygons[r];
	base=T3Dvisiblepolygons;
	
	while (lp<=rp)
	{
		while ((*lp)->centre.z>lop3)  {lp++;}
		while (lop3>(*rp)->centre.z)  {rp--;}
		if (lp<=rp)
		{
			c=*lp;*lp=*rp;*rp=c;
			lp++;rp--;         
		}      
	}
	if (oldlp<rp) T3Dsort(l,(INT32)(rp-base));
	if (lp<oldrp) T3Dsort((INT32)(lp-base),r);
}




/*class vertex:public telem
{
public:

  UINT8 attribute; //attribute byte for latter use, not used in this proggie
  vector data3d;
  vector temp3d;
  vector normal;
  vector tempnormal;
  tpoint point2d;
  polygonref *plist;
  
    vertex *nxt(){return (vertex*)next;}
*/ 

//add a temporary vertexreference and new vertex to be clipped...
//this does not add a polygon reference to the new vertex; it is not needed.
//it also does not update the 'npoints' field of the polygon, it is assumed that this is setup correctly to start with.

tpolyvertex *T3Daddtempvertex(tpolygon *poly,tclipvert *vrefin,tpolyvertex *pv,UINT8 *mem)
{    
    //pv->texv=vrefin->v;
    pv->texv=vrefin->t;
    if (vrefin->oldv==NULL)
    {           
		vertex *newv=(vertex *)mem;       
		pv->v=newv;       
		newv->normal=vrefin->n;
		newv->attribute=T3Ddefaultvertexattributes+naVISIBLE+naTEMPORARY;
		newv->temp3d=vrefin->v;
		//because the new clipped vertex is in world space coords, just do this.
		flt temp=vrefin->v.z;    
		newv->point2d.x=(flt)((vrefin->v.x*FOV)/temp)+T3Doffset.x;
		newv->point2d.y=(flt)((vrefin->v.y*FOV)/temp)+T3Doffset.y;       
    }
    else
    {
		pv->v=vrefin->oldv->v;
    }
    return pv;
}


/////////////////////////////////////////////////////////////////////
// Set up a clip plane with the specified normal.1
/////////////////////////////////////////////////////////////////////


//Ax + By + Cz + D = 0
//where a,b and c are the normal x,y,z and distance

//Where A B C D are the co-efficients of the plane, and xyz is a point on the plane.
//Recalling that the equation of a ray is: 

//Origin + t*Direction

flt clipscale(tplane *p,vector *v1,vector *v2)
{
    vector dir=*v2-*v1;    
    return (flt)-(p->normal.x*v1->x + p->normal.x*v1->y + p->normal.x*v1->z + p->distance)/
		(p->normal.x*dir.x + p->normal.y*dir.y + p->normal.y*dir.z);
}          

void T3Dsetclipplane(vector &normal,tplane &plane)
{
    // Rotate the plane normal into worldspace
    plane.normal=normal;
    //vector zero=vector(0,0,0);
    //plane.distance =dot(zero,plane.normal)+(flt)CLIP_PLANE_EPSILON;
    plane.distance= (flt)CLIP_PLANE_EPSILON;
}

/*bool insideviewvol(tcamera *cam,vector &v)
{
flt FOV2=cam->FOV*2;
flt sw=(flt)(SC.size.x+1)*v.z;
flt sh=(flt)(SC.size.y+1)*v.z;

  return ((v.z>cam->NEARZ)&&(v.z<cam->FARZ)
  &&(v.x<=(sw/FOV2))&&(v.x>=(-sw/FOV2))&&
  (v.y<=(sh/FOV2))&&(v.y>=(-sh/FOV2)));
} */   

/////////////////////////////////////////////////////////////////////
// Set up the planes of the frustum, in worldspace coordinates.
/////////////////////////////////////////////////////////////////////
void T3Dsetupfrustum()
{
    flt angle, s, c;
    vector normal;
    angle = (flt)atan(2.0 * (FOV/(SC.viewport.width()+0.5)) );
    s = (flt)sin(angle);
    c = (flt)cos(angle);
    // Left clip plane
    normal.x = s;
    normal.y = 0;
    normal.z = c;
    T3Dsetclipplane(normal, frustumplanes[0]);
	
    // Right clip plane
    normal.x = -s;
    T3Dsetclipplane(normal, frustumplanes[1]);
	
    angle = (flt)atan(2.0 * (FOV/ (SC.viewport.height()+0.5)));
    s = (flt)sin(angle);
    c = (flt)cos(angle);
	
    // Bottom clip plane
    normal.x = 0;
    normal.y = s;
    normal.z = c;
    T3Dsetclipplane(normal, frustumplanes[2]);
	
    // Top clip plane    
    normal.y = -s;
    T3Dsetclipplane(normal, frustumplanes[3]);
	
    // Front clip plane*/
    frustumplanes[4].normal.x = 0;
    frustumplanes[4].normal.y = 0;
    frustumplanes[4].normal.z = 1;
    frustumplanes[4].distance = NEARZ;
	
    // back clip plane*/
    frustumplanes[5].normal.x = 0;
    frustumplanes[5].normal.y = 0;
    frustumplanes[5].normal.z = -1;
    frustumplanes[5].distance = -FARZ;
}


void T3Dsetupfrustumviewp()
{
    flt angle,temp;
    vector normal;
	
    ///////////////////////////////////////////////////////
    // X
    temp=T3Dscreencentre.x-SC.viewport.b.x;
    angle = (flt)atan(FOV/temp);
    // Left clip plane
    normal.y = 0;
    if (temp<0)
    {
		normal.x = (flt)sin(angle);
		normal.z = (flt)cos(angle);
    }else
    {
		normal.x = (flt)-sin(angle);
		normal.z = (flt)-cos(angle);
    }
    T3Dsetclipplane(normal, frustumplanes[0]);
	
    //Right clipplane
    temp=T3Dscreencentre.x-SC.viewport.a.x;
	
    angle = (flt)atan(FOV/temp);
    if (temp<0)
    {
		normal.x = (flt)-sin(angle);
		normal.z = (flt)-cos(angle);
    }else
    {
		normal.x = (flt)sin(angle);
		normal.z = (flt)cos(angle);
    }
    T3Dsetclipplane(normal, frustumplanes[1]);
    //frustumplanes[1].distance=-frustumplanes[1].distance;
	
    
	
    ///////////////////////////////////////////////////////
    // Y
    //bottom clipplane
    temp=T3Dscreencentre.y-SC.viewport.b.y;
    angle = (flt)atan(FOV/temp);
    normal.x = 0;
    if (temp<0)
    {
        normal.y = (flt)sin(angle);
        normal.z = (flt)cos(angle);
    }else
    {
        normal.y = (flt)-sin(angle);
        normal.z = (flt)-cos(angle);
    }
    
    T3Dsetclipplane(normal, frustumplanes[2]);
    //frustumplanes[2].distance=frustumplanes[2].distance;
    temp=T3Dscreencentre.y-SC.viewport.a.y;
    
    angle = (flt)atan(FOV/temp);
    // Top clip plane
    if (temp<0)
    {
        normal.y = (flt)-sin(angle);
        normal.z = (flt)-cos(angle);
    }else
    {
        normal.y = (flt)sin(angle);
        normal.z = (flt)cos(angle);
    }
    T3Dsetclipplane(normal, frustumplanes[3]);
    //frustumplanes[3].distance=-frustumplanes[3].distance;
	
    ///////////////////////////////////////////////////////
    // Z
    // Front clip plane      
    
    frustumplanes[4].normal.x = 0;
    frustumplanes[4].normal.y = 0;
    frustumplanes[4].normal.z = 1;
    frustumplanes[4].distance = NEARZ;
	
    frustumplanes[5].normal.x = 0;
    frustumplanes[5].normal.y = 0;
    frustumplanes[5].normal.z = -1;
    frustumplanes[5].distance = -FARZ;
}


UINT8 T3Dsphereclipped(vector *c,flt radius)
{
	
    UINT8 cliphint=0;
    for (UINT32 i=0 ; i<(NUM_FRUSTUM_PLANES);i++)//
    {
        flt dist=frustumplanes[i].distance-dot(*c, frustumplanes[i].normal);
        if ((dist>radius))   
            return scOUTSIDE;
        else if ((dist>-radius))
            cliphint|=(1<<i);
    }
    return cliphint;    
}

static int T3Dcliptoplane(tclippoly *pin, tplane *pplane, tclippoly *pout)
{
    static int     curin, nextin;
    static flt curdot, nextdot, scale,temp;
    static tclipvert *pinvert,*poutvert,*nxtvert,*end;
	
    //inside/outside test
    pout->npoints=0;
    nxtvert=pin->p;
    poutvert=pout->p;
    //distance of plane algo basically
    pinvert=(end=&nxtvert[pin->npoints])-1;
    curdot = dot(pinvert->v, pplane->normal);
    curin = (curdot >= pplane->distance);
	
    for (;nxtvert<end;pinvert=nxtvert++)
    {
		
        // Keep the current vertex if it's inside the plane        // add a new vertex here ref
        if (curin)
        {
            *poutvert=*pinvert;
            poutvert++;
            pout->npoints++;                   			
        }else		
            poutvert->oldv=NULL;		   
		
        //inside/outside test
        nextdot = dot(nxtvert->v, pplane->normal);
        nextin = (nextdot >= pplane->distance);        
        // Add a clipped vertex if one end of the current edge is
        // inside the plane and the other is outside
        if (curin != nextin)
        {
            //add new vertex and reference to temp poly       
            temp=nextdot-curdot;
            //get scale value from dot product minus distance of plane from origin
            scale = (pplane->distance-curdot) / temp;            
            poutvert->v.x= pinvert->v.x + ((nxtvert->v.x - pinvert->v.x) * scale);
            poutvert->v.y= pinvert->v.y + ((nxtvert->v.y - pinvert->v.y) * scale);
            poutvert->v.z= pinvert->v.z + ((nxtvert->v.z - pinvert->v.z) * scale);
            poutvert->n.x= pinvert->n.x + ((nxtvert->n.x - pinvert->n.x) * scale);
            poutvert->n.y= pinvert->n.y + ((nxtvert->n.y - pinvert->n.y) * scale);
            poutvert->n.z= pinvert->n.z + ((nxtvert->n.z - pinvert->n.z) * scale);
            poutvert->c.r= pinvert->c.r + ((nxtvert->c.r - pinvert->c.r) * scale);
            poutvert->c.g= pinvert->c.g + ((nxtvert->c.g - pinvert->c.g) * scale);
            poutvert->c.b= pinvert->c.b + ((nxtvert->c.b - pinvert->c.b) * scale);
            poutvert->c.a= pinvert->c.a + ((nxtvert->c.a - pinvert->c.a) * scale);
            poutvert->t.x= (float)pinvert->t.x + ((nxtvert->t.x - pinvert->t.x) * (float)scale);
            poutvert->t.y= (float)pinvert->t.y + ((nxtvert->t.y - pinvert->t.y) * (float)scale);            
            poutvert->oldv=NULL;
            poutvert++;
            pout->npoints++;
            T3Dpolygonclipped=true;
        }
		
        curdot = nextdot;
        curin = nextin;
        //get next vertex 
        pinvert++;
    }
	
    if (pout->npoints < 3)
        return 0;
	
    return 1;
	
}

tclippoly T3Dclippolys[2];  //perhaps I should have a method of rendering these things raw? Who am I asking?
tclippoly *T3Dppoly;


static tpolygon *T3Dcliptofrustum(tpolygon *pin)
{
    static int         i, curpoly;
    static tpolygon   *newpoly;    
    static tpolyvertex *poutvertref,*end;
    static vertex *poutvert;
    static tclipvert *clipv;    
	static UINT32 polysize;
	static UINT8 *mem;
	curpoly=0;
    T3Dppoly = &T3Dclippolys[1];
    //load the normal polygon into the clip structure    
    clipv=T3Dppoly->p;
    T3Dppoly->npoints=pin->npoints;
    T3Dpolygonclipped=false;
    for (end=&(poutvertref = pin->vlist)[pin->npoints];poutvertref<end;poutvertref++)
    {
        poutvert = poutvertref->v;        
        clipv->v=poutvert->temp3d;
        //clipv->n=poutvert->tempnormal;
        clipv->e=poutvertref->e;
        clipv->t.x=poutvertref->texv.x;
        clipv->t.y=poutvertref->texv.y;
		clipv->c=poutvertref->outcolour;
        clipv->oldv=poutvertref;
        clipv++;        
    }
    
    for (i=0 ; i<(NUM_FRUSTUM_PLANES);i++)
    {
        if ((T3Dcliphint & (1<<i))!=0)
        {
			if (!T3Dcliptoplane(T3Dppoly,
				&frustumplanes[i],
				&T3Dclippolys[curpoly & 1]))                
				return NULL;        
			T3Dppoly = &T3Dclippolys[curpoly];
			curpoly ^= 1;
        }
    }
    if (!T3Dpolygonclipped)
		return(pin);
	
	//Now set the data for the new temporary polygon
    clipv=T3Dppoly->p;
    polysize=T3Dpolymemsize(T3Dppoly->npoints);
    mem=(UINT8*)tempspace.grab(polysize+(T3Dppoly->npoints*sizeof(vertex)));
    if (!mem) return NULL;
    newpoly=T3Dcreatepolygon(pin,T3Dppoly->npoints,mem);
    vector newcentre(FLT_0);
    newpoly->next=(telem *)pin;
    mem+=polysize;
    if (newpoly)
    {
        for (i=0;i<T3Dppoly->npoints;i++)
        {
            T3Daddtempvertex(newpoly,clipv,&newpoly->vlist[i],mem);
            newcentre+=clipv->v;
			newpoly->vlist[i].attribute=0;//pin->vlist[i].attribute;
			newpoly->vlist[i].mat=NULL;
			newpoly->vlist[i].outcolour=clipv->c;
			if (clipv->e)
			{
				clipv->e->attribute |=eaVISIBLE; //this edge is visible            
				newpoly->vlist[i].e=clipv->e;
			}else
				newpoly->vlist[i].e=NULL;
			
			
            clipv++;
            mem+=sizeof(vertex);
        }        
        newpoly->centre=newcentre;
    }
    return newpoly;        
}


vector *tplane::intersect(vector *res,vector *v1,vector *v2)
{
	
	float dot1,dot2;	
	dot1 = dot(*v1, normal);
	dot2 = dot(*v2, normal);
	
	if ((dot1>=distance)!=(dot2>=distance))
	{
		float temp=dot2-dot1;
		float scale = (distance-dot1) / temp;            
		*res=*v1+((*v2-*v1)*scale);
		return res;
	}else
		return v1;
}


UINT32 tplane::classifypoly(tpolygon *p)
{
    UINT32 retval=0;
    tpolyvertex *v,*end;
    flt currdot;
    
    for (end=&(v=p->vlist)[p->npoints];v<end;v++)
    {
        currdot = dot(v->v->data3d, normal);
        if (currdot>distance)
			retval |= pcBEHIND;
        else if(currdot<distance)
			retval |= pcINFRONT;
    }
    return retval;
}


UINT32 T3Dgettextureindex(textureref *tex)
{
    UINT32 count=0;
    textureref *ctex=(textureref*)T3Dtexturebank.first;
    while (ctex!=NULL)
    {
        if (ctex==tex)
			return count;
        count++;
        ctex=(textureref*)ctex->next;
    }
    return 0;
}

UINT32 T3Dgetmaterialindex(material *mat)
{
    UINT32 count=0;
    material *cmat=(material*)T3Dmaterialbank.first;
    while (cmat!=NULL)
    {
        if (cmat==mat)
			return count;
        count++;
        cmat=(material*)cmat->next;
    }
    return 0;
}

void T3Ddeletematerial(material *mat)
{
	T3Dmaterialbank.remove((telem*)mat);
	T3Daddmaterialcallback(NULL);
}

material *T3Daddmaterialifnotfound(material *mat,bool makenew)
{
    material *mref=(material*)T3Dmaterialbank.first;
	mat->name[0]=0;
    while (mref!=NULL)
    {
        if (*mref==*mat)                    
            return mref;        
        mref=mref->nextm();
    }
    material *nmat;
    if (makenew)
    {
		nmat=new material;
		*nmat=*mat;
    }else    
		nmat=mat;
	
    T3Dmaterialbank.push((telemptr)nmat);
    if (T3Daddmaterialcallback)
		T3Daddmaterialcallback(nmat);
    return nmat;
}

material *T3Dfindmaterialbyname(char *name)
{
	material *mref=(material*)T3Dmaterialbank.first;
	while (mref)
	{
		if (!strcmp(name,mref->name))
			return mref;
		mref=(material*)mref->next;
	}
	return NULL;
}

material *T3Daddmaterial()
{
    material *nmat=new material();
	nmat->ambient=
    nmat->diffuse=
    nmat->specular=
    nmat->emmisive=trgbfloat(0,0,0);
    nmat->specpower=0;
    nmat->st=0;
    nmat->transparency=0;        
	nmat->name[0]=0;
	nmat->tex1=NULL;
	T3Dmaterialbank.push((telemptr)nmat);
    if (T3Daddmaterialcallback)
		T3Daddmaterialcallback(nmat);
    return nmat;
}


material *T3Daddmaterial(materialentry *mat)
{
    material lmat;
    lmat.ambient=trgbfloat(mat->ar,mat->ag,mat->ab);
    lmat.diffuse=trgbfloat(mat->dr,mat->dg,mat->db);
    lmat.specular=trgbfloat(mat->sr,mat->sg,mat->sb);
    lmat.emmisive=trgbfloat(mat->er,mat->eg,mat->eb);
    lmat.specpower=mat->sp;
    lmat.st=mat->st;
    lmat.transparency=mat->t;        
	lmat.tex1=NULL;
    return T3Daddmaterialifnotfound(&lmat,TRUE);
}

material *T3Daddmaterial(trgbfloat a,trgbfloat d,trgbfloat s,trgbfloat e,UINT8 specpower,flt t)
{
    material lmat;    
    lmat.next=NULL;
    lmat.st=FLT_013;
    lmat.ambient=a;
    lmat.diffuse=d;
    lmat.specular=s;
    lmat.emmisive=e;
    lmat.specpower=specpower;
    lmat.transparency=t;       
	lmat.tex1=NULL;
    return T3Daddmaterialifnotfound(&lmat,TRUE);    
}         

material *T3Daddmaterial(trgbfloat &d,UINT8 specpower,flt t)
{       
    material lmat;
	lmat.next=NULL;
    lmat.ambient=T3Dambientdefault;
    lmat.diffuse=d;
    lmat.st=0.5;
    lmat.specular=T3Dspeculardefault;
    lmat.emmisive=T3Demmisivedefault;
    lmat.specpower=specpower;
    lmat.transparency=t;    
	lmat.tex1=NULL;
    return T3Daddmaterialifnotfound(&lmat,TRUE);
}

material *T3Daddmaterial(material *mat)
{
    return T3Daddmaterialifnotfound(mat,FALSE);
}

UINT32 getshift(UINT32 val)
{
	
	for (UINT32 i=0;i<31;i++)					
		if (val<(1 << (i+1)))
			return i;		
	return 1;
	
}

textureref *T3Daddtexture(tsurface *pic,char *name)
{
	if (!pic) return NULL;
    textureref *tref=(textureref*)T3Dtexturebank.first;        
    while (tref!=NULL)
    {
		if (!strcmp(name,tref->name))        
			return tref;        
		tref=tref->nextt();
	}
    tref=new textureref;
    if ((!tref)||(!pic)) return NULL;    
    strcpy(tref->name,name);        
	T3Dsettexture(tref,pic);
	
    T3Dtexturebank.push((telemptr)tref);
    if (T3Daddtexturecallback)
		T3Daddtexturecallback(tref);    
	
    return tref;
}    

void T3Dsettexture(textureref *pic)
{
    T3Dtexture=pic;
    (pic!=NULL)?T3Ddefaultpolyattributes|=paTEXTURED:T3Ddefaultpolyattributes&=~paTEXTURED;
    //SETBIT(T3Ddefaultpolyattributes,paTEXTURED,pic!=NULL);    
}

tsurface *T3Dsettexture(textureref *tref,tsurface *pic)
{
	
	tref->t=pic;
	if (pic)
	{
		tref->xshift=getshift(pic->size.x);
		tref->yshift=getshift(pic->size.y);
		tpoint nsize;
		nsize.assign(1<<tref->xshift,1<<tref->yshift);

		if (nsize!=pic->size)
		{	
			tref->t=TWINrendercontext->createsurface2(pic,nsize.x,nsize.y,pic->pfd,NULL);
			TWINrendercontext->releasesurface(pic);
		}
		else
			tref->t=pic;
		
	}
	return pic;
}


textureref *T3Dgettextureref(UINT32 idx)
{
    if (idx==0) return NULL;
    idx--;
    textureref *retref=(textureref*)T3Dtexturebank.first;
    while ((retref!=NULL)&&(idx!=0))
    {
        retref=retref->nextt();
        idx--;        
    }
    return retref;
}




void T3Dprintstats(INT16 x,INT16 y)
{
	
    sprintf(buf,"Polycount %i/%i",T3Dpolycount,T3Dtotalpolys);
    outtextxy(x,y,buf);
    sprintf(buf,"clip status %i %i %i %i %i %i",T3Dcliphint & 1,T3Dcliphint & 2,T3Dcliphint & 4,T3Dcliphint & 8,T3Dcliphint & 16,T3Dcliphint & 32);\
		outtextxy(x,y+=20,buf);
}    

UINT32 T3Dinit(UINT32 polyblocksize,UINT32 tempspacesize)
{
    T3Dmaxpolys=polyblocksize;
    T3Dmaxedges=polyblocksize<<2;
    T3Dpolycount=T3Dtotalpolys=0;
    T3Dedgecount=0;
    T3Dvisiblepolygons=new tpolygon*[polyblocksize];
    T3Dvisibleedges=new tedge*[T3Dmaxedges];
    if (!T3Dvisiblepolygons) return T3Dseterror(0);
    //strcpy(TEXTURE_PATH,getenv("VRBITMAPS"));
    /*if (TEXTURE_PATH[0]==0)
	strcpy(TEXTURE_PATH,EXE_PATH);   */
    //if (TEXTURE_PATH[strlen(TEXTURE_PATH)-1]!='\\')
    //   strcat(TEXTURE_PATH,"\\");
    T3Dcvisiblepoly=T3Dvisiblepolygons;
	
    T3Dlastpolygon=T3Dvisiblepolygons+(polyblocksize<<2);
	
    T3Dtexturebank.init(sizeof(textureref));
    T3Dmaterialbank.init(sizeof(material));
    
    WHITEMATERIAL.ambient=trgbfloat(FLT_0,FLT_0,FLT_0);
    WHITEMATERIAL.diffuse=trgbfloat(FLT_1,FLT_1,FLT_1);
    WHITEMATERIAL.specular=trgbfloat(FLT_1,FLT_1,FLT_1);
    WHITEMATERIAL.specpower=2;
    WHITEMATERIAL.st=0.5;
    WHITEMATERIAL.emmisive=trgbfloat(FLT_0,FLT_0,FLT_0);
    WHITEMATERIAL.transparency=FLT_0;
	
    if (GFX_INITIALIZED)
    {
		T3DCOLprojector=getcolour2(0,255,100);
		T3DCOLwire=getcolour2(200,200,255);
		T3DCOLselectedwireedge=getcolour2(0,255,255);
		T3DCOLselectedwire=getcolour2(255,255,50);
		T3DCOLboundingbox=getcolour2(255,0,0);
		T3DCOLcursor=getcolour2(255,0,0);    
		T3DCOLnode=getcolour2(250,250,255);
		T3DCOLselectednode=getcolour2(255,255,0);
		T3DCOLgrid=getcolour2(50,140,140);
		T3DCOLgridmajor=getcolour2(70,180,180);
		T3DCOLextrusionbody=getcolour2(100,150,255);
		T3DCOLextrusionedge=getcolour2(0,100,255);    
    }      
    T3Dtexture=NULL;
    T3Dmaterial=&WHITEMATERIAL;
	
    if (!tempspace.create(tempspacesize))return 0;
    return 1;
}

void destroytexturecontents(telem *tex)
{
    delete (((textureref*)tex)->t);
}    



//You should also sort the polygons on insert (it's faster) by several modes:
//Nearest Z (best case for zbuffer)
//Minz and Maxz; make a judgement based on these two values, polys with larger
//spans should go first
//And none; Don't sort polygon list



inline tpolygon *T3Daddvisiblepolygon(tpolygon *poly)
{
    if ((T3Dcvisiblepoly+1)>T3Dlastpolygon) return NULL;    
    *(T3Dcvisiblepoly++)=poly;
    
    T3Dpolycount++;
    if (poly->attribute & paSELECTED)
		T3Dselpolycount++;          
    return poly;
}




inline tpolygon *T3Daddvisibleedges(tpolygon *poly)
{
    tpolyvertex *vr=poly->vlist;
    tpolyvertex *end=&vr[poly->npoints];
    tedge *e;
    
    UINT32 attribute;
    for (;vr<end;vr++)
    {
        e=vr->e;
        if (e)
        {
			attribute=e->attribute;             
			if (((attribute & T3Dedgeprocessmask)==T3Dedgeprocessmaskresult)&& (attribute & eaNOTDRAWNYET) )
			{
				e->attribute &=~eaNOTDRAWNYET;
				*(T3Dcvisibleedge++)=e;
				T3Dedgecount++;
			}
        }
    }
    return poly;
}


void T3Dresetbuffers()
{
    T3Dpolycount=T3Dtotalpolys=T3Dselpolycount=0;
    T3Dmaxpolyedges=T3Dmaxdrawnedges=0;
    T3Dedgecount=0;
    tempspace.reset();
    T3Dcvisiblepoly=T3Dvisiblepolygons;
    T3Dcvisibleedge=T3Dvisibleedges;    
}

void T3Dshutdown()
{
    delete T3Dvisiblepolygons;
	delete T3Dvisibleedges;
	
    T3Dtexturebank.destroy(destroytexturecontents);
    T3Dmaterialbank.destroy();    
    tempspace.destroy();
}


bool tpolygon::isinside(tplane *planes,int count)
{
	int code=0;
	int ccode=0;
	for (int i=0;i<count;i++)
	{
		ccode=planes[i].classifypoly(this);
		if (ccode==pcINFRONT)
			return false;
		
	}
	return true;

}

void tpolygon::getedgenormal(tpolyvertex *e,vector *v)
{	
	vertex **ev=e->e->v;
	vector v1=ev[1]->data3d-ev[0]->data3d;
	v1.normalize();	
	cross(*v,v1,normal);	
	if (e->attribute & pvaEDGEISANTICLOCKWISE )
		*v=-*v;
}



void tpolygon::getedgeplane(tplane *plane,tpolyvertex *e)
{	
	getedgenormal(e,&plane->normal);
	plane->distance=dot(plane->normal,e->e->v[0]->data3d);	
	
	
}

void tpolygon::getplane(tplane *plane)
{
    plane->normal=normal;
    plane->distance=dot(normal,centre); //the shortest distance between the origin and the plane    
}

bool tpolygon::iscoplainar(flt threshold)
{
    tpolyvertex *v,*end;
    v=vlist;
    flt distance=dot(normal,v->v->data3d);
    v++;
    for (end=&vlist[npoints];v<end;v++)    
        if (!fcloseto2(distance,dot(normal,v->v->data3d),threshold)) return FALSE;    
		return TRUE;
}


UINT32 tpolygon::fixedgeflags()
{
    tpolyvertex *v,*end;
    UINT32 v1,v2,v3;
    for (end=&(v=vlist)[npoints];v<end;v++)
    {
        if (v->e->v[0]==v->v)
            v->attribute &=~pvaEDGEISANTICLOCKWISE;
        else if (v->e->v[1]==v->v)
            v->attribute |=pvaEDGEISANTICLOCKWISE;
        else
        {
            v1=findvertexidx(v->v);
            v2=findvertexidx(v->e->v[0]);
            v3=findvertexidx(v->e->v[1]);
            sprintf(buf,"expected to find vertex: %i but the edge references %i and %i",v1,v2,v3);
            logmessage((void*)&T3DCOLboundingbox,buf);          
        }    
    }
    return 1;
}


vertex *tpolygon::findclosestvertex(flt x,flt y)
{
    flt length=10000000,len2;
    vertex *rv=NULL;
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
    {
        tpointflt p=v->v->point2d;
        p.x-=x;
        p.y-=y;
		
        len2=p.lengthsquared();
        if (len2<length)
        {
			length=len2;
			rv=v->v;
        }
    }
    return rv;
	
}

tedge *tpolygon::findclosestedge(flt x,flt y)
{
    flt length=10000000,len2;
    tedge *re=NULL;
    tpolyvertex *v,*end;
    tpointflt p((flt)x,(flt)y);
    for (end=&(v=vlist)[npoints];v<end;v++)
    {
        tpointflt p1=v->e->v[0]->point2d;
        tpointflt p2=v->e->v[1]->point2d;
        
        len2=p.finddistancefromlinesquared(p1,p2);
        if (len2<length)
        {
			length=len2;
			re=v->e;
        }
    }
    return re;
}


UINT32 tpolygon::countedgeswith(UINT32 mask)
{
    UINT32 count=0;
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)    
        if (v->e->attribute & mask) count++;
		return count;
}

UINT32 tpolygon::countverticeswith(UINT32 mask)
{
    UINT32 count=0;
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)    
        if (v->v->attribute & mask) count++;
		return count;
}

void tpolygon::copyoldvertices()
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->v->old=v->v->data3d;                      
}


void tpolygon::calccentre()
{
    tpolyvertex *v,*end;
    centre=0;
    for (end=&(v=vlist)[npoints];v<end;v++)    
        centre+=v->v->data3d;
    centre/=(flt)npoints;
}


bool tpolygon::removeedgereferences()
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)    
        v->e->deletereferences(this);            
    return TRUE;
}

UINT32 tpolygon::checkintegrity()
{
    UINT32 status=piOK;
    
    tpolyvertex *v,*end,*last;
    v=vlist;
    t3dmesh *parent=v->v->parent;
    UINT32 edgereport;
    
    last=&v[npoints-1];
    for (end=&v[npoints];v<end;v++)
    {
        if (v->e->attribute & eaEDGESHARED)        
			status|=piSHAREDEDGEREFERENCE;        
		
        if ((v->e->v[0]!=v->v) && (v->e->v[1]!=v->v))        
			status|=piNOVERTEXREFERENCEINEDGE;
		
        if (v->e->v[(v->attribute & pvaEDGEISANTICLOCKWISE)?1:0]!=v->v)        
			status|=piBADEDGEFLAG;                       
		
        if (v->v->parent!=parent)        
			status|=piVERTEXPARENTSDAMAGED;        
        
        edgereport=v->e->checkintegrity();
        if (!(edgereport==0 || edgereport==eiROUGEPOLYGONWARNING))        
			status|=piEDGESDAMAGED;        
		
        if (!v->e->findpolygonref(this))        
			status|=piMISSINGEDGEREFERENCE;               
		
		if (!v->e->sameas(v->v,last->v))		
			status |= piEDGEINWRONGPLACE;
		
        v->e->attribute |= eaEDGESHARED;
		last=v;
    }
	
    for (v=vlist;v<end;v++)
        v->e->attribute &=~eaEDGESHARED;
	
    return status;
}


void getslopes(vector *a,vector *b,float *m1,float *m2)
{
	*m1=(b->y-a->y)/(b->x-a->x);
	*m2=(b->z-a->z)/(b->x-a->x);
}

void tpolygon::calcpolynormal()
{
	
	vector *a,*b,*c=NULL;
	a=&vlist[0].v->data3d;
	b=&vlist[1].v->data3d;
	float m1,m2;
	float n1,n2;
	getslopes(a,b,&m1,&m2);
	//find next no-colinear point
	if (npoints<=3)
	{		
		calcnormal(*a,*b,vlist[2].v->data3d,normal);	
	}else
	{
		for (UINT32 i=2;i<npoints;i++)
		{
			getslopes(b,&vlist[i].v->data3d,&n1,&n2);
			if ((n1!=m1) || (n2!=m2))
			{
				c=&vlist[i].v->data3d;
				break;
			}
		}
		if (c)			
			calcnormal(*a,*b,*c,normal);
	}
			
}

void tpolygon::reverse()
{
	
    tpolyvertex swapme;
    UINT32 last=npoints-1;
	UINT32 i;
	
	
    for (i=0;i< npoints>>1;i++)
    {
        swapme=vlist[i];
        vlist[i]=vlist[last];
		
        vlist[last]=swapme;
		last--;	
    }
	
	swapme.e=vlist[npoints-1].e;
	for (i=npoints-1;i>0 ;i--)     	
		vlist[i].e=vlist[i-1].e;    
	vlist[0].e=swapme.e;
	
	//reverse flags
	for (i=0;i< npoints;i++)   
	{
		vlist[i].attribute &= ~pvaEDGEISANTICLOCKWISE;
		if (vlist[i].e)
			if (vlist[i].v!=vlist[i].e->v[0])
				vlist[i].attribute |= pvaEDGEISANTICLOCKWISE;
			
	}
	/*static tedge *edges[256];
    //this code works fine but is slow and clumsy
	for (i=0;i< npoints;i++)
	{
	edges[i]=vlist[i].e;
	vlist[i].attribute &=~pvaEDGEISANTICLOCKWISE;
	}
	
	  t3dmesh *obj=this->parent;
	  UINT32 reversed;
	  tpolyvertex *pv,*lastpv;
	  pv=vlist;
	  lastpv=&pv[npoints-1];
	  //search edge list for correct edge. Search may not be nessicary but do it for now
	  for (i=0;i<npoints;i++)	
	  {
	  if (!(pv->e=obj->edgesearch(pv->v,lastpv->v,&reversed,edges,npoints)))
	  return NULL;
	  //pv->e->addreference(newp);
	  if (reversed)
	  pv->attribute |= pvaEDGEISANTICLOCKWISE;			
	  lastpv=pv;
	  pv++;
}*/
    normal.x=-normal.x;
    normal.y=-normal.y;
    normal.z=-normal.z;       
}


void tpolygon::setuv(tpointfloat *texv)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++,texv++)
        v->texv=*texv;
}

bool tpolygon::contains(vertex *cv)
{
    tpolyvertex *v;
    for (tpolyvertex *end=&(v=vlist)[npoints];v<end;v++)        
        if (v->v==cv) return TRUE;    
		return FALSE;
}


bool tpolygon::checkedgeattributes(UINT32 attr)
{
    tpolyvertex *v;
    for (tpolyvertex *end=&(v=vlist)[npoints];v<end;v++)    
		if (v->e->attribute & attr) return TRUE;
		return FALSE;    
}


bool tpolygon::checkvertexattributes(UINT32 attr)
{
    tpolyvertex *v;
    for (tpolyvertex *end=&(v=vlist)[npoints];v<end;v++)    
		if (v->v->attribute & attr) return TRUE;
		return FALSE;    
}
//returns the number of vertices that matched the criteria
UINT32 tpolygon::checkallvertexattributes(UINT32 attr)
{
    UINT32 count=0;
    tpolyvertex *v;
    for (tpolyvertex *end=&(v=vlist)[npoints];v<end;v++)    
		if (v->v->attribute & attr) count++;
		return count;    
}

tpolyvertex *tpolygon::findedge(tedge *e)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
		if (v->e==e) return v;
		return NULL;
}

INT32 tpolygon::findedgeidx(tedge *e)
{
    for (UINT32 i=0;i<npoints;i++)
		if (vlist[i].e==e) return i;
		return -1;
}

INT32 tpolygon::findvertexidx(vertex *v)
{
    for (UINT32 i=0;i<npoints;i++)
		if (vlist[i].v==v) return i;
		return -1;
}


void tpolygon::setmaterial(material *mat,UINT32 mask)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
		if (v->v->attribute & mask)
			v->mat=mat;
}


void tpolygon::or_vertexrefattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->attribute |=attr;
}
void tpolygon::and_vertexrefattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->attribute &=attr;
}

void tpolygon::xor_vertexrefattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->attribute ^=attr;
}
void tpolygon::or_vertexattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->v->attribute |=attr;
}
void tpolygon::and_vertexattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->v->attribute &=attr;
}

void tpolygon::xor_vertexattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->v->attribute ^=attr;
}

void tpolygon::or_edgeattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->e->attribute |=attr;
}


void tpolygon::and_edgeattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->e->attribute &=attr;
}

void tpolygon::xor_edgeattributes(UINT32 attr)
{
    tpolyvertex *v,*end;
    for (end=&(v=vlist)[npoints];v<end;v++)
        v->e->attribute ^=attr;
}

bool tpolygon::allverticesclipped()
{
    tpolyvertex *v=vlist;
    for (int i=npoints;i>0;i--,v++)    
        if ((v->v->attribute & naVISIBLE)!=0) return FALSE;
		return TRUE;
}


bool tpolygon::someverticesclipped()
{
    tpolyvertex *v=vlist;
    for (int i=npoints;i>0;i--,v++)    
        if ((v->v->attribute & naVISIBLE)!=0) return TRUE;
		return FALSE;
}

bool tpolygon::noverticesclipped()
{
    tpolyvertex *v=vlist;
    for (int i=npoints;i>0;i--,v++)    
        if ((v->v->attribute & naVISIBLE)==0) return FALSE;
		return TRUE;
}


bool tpolygon::containsedge(vertex *from,vertex *to)
{
    tpolyvertex *v=vlist;
    tpolyvertex *vj,*end;
    vj=end=&vlist[npoints];
    vj--;
    while (v<end)
    {
        if ( (vj->v==from && v->v==to) || (vj->v==to && v->v==from))
			return true;
        vj=v;
        v++;        
    }
    return false;
}


//////////////////////////////////////////
// Basic 3d object

tobject3d::tobject3d()
{        
    //setup for tree stuff  
    next = parent = child = NULL;
    attribute=T3Ddefaultobjectattributes;    
    unitpos();
    //mass=FLT_1;
    //maxspeed=3FLT_0;
    //minspeed=-3FLT_0;
    min=max=0;
    radius=0;
}

tobject3d::~tobject3d()
{
    ;    
}


char *tobject3d::setname(char *name_)
{
    strcpy(name,name_);
    return name;
}

//speed, 
#define MAXSPEED 300

void tobject3d::updatedatafull()
{
	//updatedata();
	tobject3d *currobj=(tobject3d*)child;  
	while (currobj!=NULL)
	{
		currobj->updatedatafull();
		currobj=(tobject3d*)currobj->next;    
	}
}


void tobject3d::updatedata()
{
	vector thrust,diff;
	flt decby=mass*T3Dresistance;
	vector dp=vector(a,0,0);
	dp.multmatrix(&localmat,&thrust);
	d+=thrust;
	spd=d.length();
	if (spd>0)
	{
		spd-=decby;
		if (spd<0) spd=0;
	}
	else
	{
		spd+=decby;
		if (spd>0) spd=0;
	}
	BOUND(spd,minspeed,maxspeed);
	d.normalize();
	d*=spd;
	
	localmat.p+=d;
	p=localmat.p;
}


//when parents change, must call another function to traverse down the heirarchy
//also looks down the heirarchy to sort out any misbehaving children.
//this should be called whenever the localmat matrix is changed!

trectflt *tobject3d::getbounds(matrix4x4 *cmat,tcamera*cam)
{
    return &bounds;
}

void tobject3d::calcworldmat()
{
	if (parent)
	{
		LMconcatenatematrices(&tparent()->localmat,&localmat,&worldmat);
		LMconcatenatematrices(&tparent()->localmattr,&localmattr,&worldmattr);
		//LMconcatenatematrices(&tparent()->localmattr,&localmattr,&localmattr);   
	}else
	{
		worldmat=localmat;
		worldmattr=localmattr;
	}
	tobject3d *childloop=tchild();
	while (childloop!=NULL)
	{
		tchild()->calcworldmat();
		childloop=childloop->tnext();
	}    
	
}


void tobject3d::lookat(vector pt,flt twist)
{        
    flt r, theta, fee;
    pt-=p;
    r = pt.length();
    if (r==0) return;
    //the following code works for rottrans() rather than the inverted versiom
    //This is specifically for camera transforms;
    //Y rotation first feels more intuative.
    fee = (flt)atan2 (pt.y,pt.z);             
    simplexrot((float)-fee,&pt);
    theta =(flt)-atan2(pt.x, pt.z);    
	
    /*theta =-atan2 (pt.x, pt.z);    
    simpleyrot(-theta,&pt);
    fee = atan2 (pt.y,pt.z);             */
    
    rotate(fee,theta,o.z);
}

void tobject3d::lookat2(vector pt,flt twist)
{        
    flt r, theta, fee;
    pt-=p;
    r = pt.length();
    if (r==0) return;
    //the following code works for rottrans() rather than the inverted versiom
    //This is specifically for camera transforms;
    //Y rotation first feels more intuative.
	
    theta =-(flt)atan2 (pt.x, pt.z);    
    simpleyrot(-theta,&pt);
    fee = (flt)atan2 (pt.y,pt.z);
	
    o.moveto(fee,theta,0);    
    LMrottrans(&localmattr,o.x,o.y,o.z,p.x,p.y,p.z);
    localmat=localmattr;
    calcworldmat();
	
}



/*void tobject3d::lookat2(vector pt,flt twist)
{
matrix4x4 zmat;
vector oldp=pt;
flt r, theta, fee;//, ct, st, cf, sf;
// vector zero length, collapse the world!
pt-=p;
r = pt.length();
if (r==0) return;

  
    fee = atan2 (pt.y,pt.z);             
    doxrot(-theta,&pt);
    theta = -atan2 (pt.x, pt.z);
    
	  
		/*sprintf(buf,"ydiff:%0.4f zdiff:%0.4f",pt.y,pt.z);
		label(0,0,buf);
		sprintf(buf,"theta :%0.4f fee:%0.4f",theta,fee);
label(0,20,buf);*/

/*if (pt.z>=0)      
fee   = asin (pt.y/r);
else
fee   = 0;//acos (pt.y/r);*/

//fee = -asin (pt.y/r);    

//rotate(-fee,-theta,twist);

//rotate(theta,fee,twist);
/*ct = cos (theta);
st = sin (theta);
cf = cos (fee);
sf = sin (fee);

  localmat.m00 = -st;  localmat.m01 = -ct*cf ;  localmat.m02 = ct*sf;
  localmat.m10 = ct;  localmat.m11 = -st*cf;  localmat.m12 = st*sf; 
  localmat.m20 = FLT_0;  localmat.m21 = sf;  localmat.m22 =  cf;*
  
    LMzrotation(&zmat,twist);
    o.x=fee;
    o.y=theta;
    o.z=twist;
    
LMconcatenatematrices(&localmat,&zmat,&localmat);    */



void tobject3d::do3d(matrix4x4 *cmat,matrix4x4 *cmattr,tcamera *cam,trender *cmds)
{    
    
}


//whereas do3d just calculates the 3d for that object,
//this wrapper method with traverse down the heirarchy.
//NOTE: the root node must be traversed in an external loop
//as this only processes the children of a given node
void tobject3d::do3dfull(matrix4x4 *cmat ,matrix4x4 *cmattr,tcamera *cam,trender *cmds)
{
	//updatedata();
	//loadup data into the temporary matrix
	//LMconcatenatematrices(cmat,&localmat,&worldmat);
	
	//then use this to calculate 2d coordinates, clipping and so on.
	if (type & T3Dobjecttypemask)
		if ((attribute & T3Dobjectprocessmask)==T3Dobjectprocessmaskresult)//
		{
			matrix4x4 tmpmat;
			matrix4x4 tmpmattr;
			//preconcatinate cameraspace matrix to objects world matrix
			LMconcatenatematrices(cmat,&worldmat,&tmpmat);
			LMconcatenatematrices(cmattr,&worldmattr,&tmpmattr);   
			do3d(&tmpmat,&tmpmattr,cam,cmds);
			tobject3d *currobj=(tobject3d*)child;
			while (currobj!=NULL)
			{
				currobj->do3dfull(cmat,cmattr,cam,cmds);
				currobj=(tobject3d*)currobj->next;    
			}
		}
		
		//LMtranspose(&tmpmat);  
}

//as with the above function this wrapper method with traverse down the heirarchy
//instead for the UI components of 3d objects (polygons are handled seperately although
//they can also be considered UI elements when selecting by polygons in the 3d view.
//sometime paradigms aren't as neat as they should be :(
//NOTE: the root node must be traversed in an external loop
//as this only processes the children of a given node

void tobject3d::renderhandlesfull(UINT32 attr,tcamera *cam)
{
	if (type & T3Dobjecttypemask)
		if ((attribute & T3Dobjectprocessmask)==T3Dobjectprocessmaskresult)
		{
			renderhandles(attr,cam);
			tobject3d *currobj=(tobject3d*)child;  
			while (currobj!=NULL)
			{
				currobj->renderhandlesfull(attr,cam);
				currobj=(tobject3d*)currobj->next;    
			}
		}
}





void tobject3d::unitpos()
{
    //spd=a=FLT_0;
    o=p=vector(FLT_0,FLT_0,FLT_0);
    s=vector(FLT_1,FLT_1,FLT_1);    
    LMidentity(&localmat);
}

void tobject3d::rotate(flt x,flt y,flt z)
{
    o.moveto(x,y,z);        
    LMrottrans(&localmattr,o.x,o.y,o.z,p.x,p.y,p.z);
    LMaddscale(&localmattr,&localmat,s.x,s.y,s.z);     
    calcworldmat();
}

void tobject3d::scale(flt x,flt y,flt z)
{
    s.moveto(x,y,z);
    LMrottrans(&localmattr,o.x,o.y,o.z,p.x,p.y,p.z);
    LMaddscale(&localmattr,&localmat,s.x,s.y,s.z);     
    calcworldmat();
}

void tobject3d::moveto(flt x,flt y,flt z)
{
    p.moveto(x,y,z);
    LMrottrans(&localmattr,o.x,o.y,o.z,p.x,p.y,p.z);
    LMaddscale(&localmattr,&localmat,s.x,s.y,s.z);     
    calcworldmat();
}

void tobject3d::oncollision()
{
}

//used for rendering nodes on an object mesh, the object centre (perhaps) and other user interactable parts
void tobject3d::renderhandles(UINT32 attrib,tcamera *cam)
{
    
}

//////////////////////////////////////////
// Geometry

void cleanupvertex(telem *v)
{
    free((vertex*)v);
}

void cleanupedge(telem *e)
{
    ((tedge *)e)->deletereferences((UINT32)0);
    free((tedge*)e);
}

void cleanuppolygon(telem *p)
{
    free((tpolygon*)p);
}



t3dmesh::t3dmesh()
{
    sprintf(name,"geom");
    type=otGEOM;
    attribute |=ofEDITABLE;
    vertexlist.init(sizeof(vertex));
    edgelist.init(sizeof(tedge));
    polygonlist.init(sizeof(tpolygon));
    rougepolygonrefs.init(sizeof(tpolygonref));
    
}

t3dmesh::~t3dmesh()
{
	destroy();  
}

void t3dmesh::destroy()
{
	//this must be called before cleanuppolygon
	
	logmessage((void*)&textcol,"destroying mesh");
	//edgelist.destroy(cleanupedge);   
	vertexlist.destroy(cleanupvertex);
	polygonlist.destroy(cleanuppolygon);   
}

void calc2dbound(tpoint *p,vector v,tpoint *min2d,tpoint *max2d,matrix4x4 *m)
{
    p->x=float2int((v.x*m->m00 +v.y*m->m01 +v.z*m->m02) +m->p.x);
    p->y=float2int((v.x*m->m10 +v.y*m->m11 +v.z*m->m12) +m->p.y);
    if (p->x<min2d->x) min2d->x=p->x;
    if (p->y<min2d->y) min2d->y=p->y;
    if (p->x>max2d->x) max2d->x=p->x;
    if (p->y>max2d->y) max2d->y=p->y;
	
}

void calc2d(tpoint *p,vector *v,matrix4x4 *m)
{
    p->x=float2int((v->x*m->m00 +v->y*m->m01 +v->z*m->m02) +m->p.x);
    p->y=float2int((v->x*m->m10 +v->y*m->m11 +v->z*m->m12) +m->p.y);
}

void t3dmesh::normalizescale()
{
    vertex *v=getvertexhead();
    matrix4x4 scalemat;
    LMscale(&scalemat,s.x,s.y,s.z);
    while (v)
    {
        v->temp3d=v->data3d;
        v->temp3d.multmatrix(&scalemat,&v->data3d);
        v=(vertex*)v->next;
    }
    scale(1,1,1);
}


//calculate objects radius and 2d bounds
trectflt *t3dmesh::getbounds(matrix4x4 *cmat,tcamera* cam)
{
    UINT32 intrad;
    flt bs=0;
    vector trad;
    if (bs<fabs(s.x)) bs=(flt)fabs(s.x);
    if (bs<fabs(s.y)) bs=(flt)fabs(s.y);
    if (bs<fabs(s.z)) bs=(flt)fabs(s.z);
    
	
    if (cam->camtype==cam3D)
    {
        flt temp=cmat->p.z;
        if (temp<NEARZ)          
			temp=NEARZ;
        //FOV can be put in the matrix...
        p2d.x=((FOV*(cmat->p.x)/temp)+T3Doffset.x);
        p2d.y=((FOV*(cmat->p.y)/temp)+T3Doffset.y);
        intrad=float2int((tempradius=radius*bs));
        //intrad=FOV*(tempradius)/temp;
		
    }else
    {
        p2d.x=(cmat->p.x);
        p2d.y=(cmat->p.y);
        intrad=float2int((tempradius=T3Dscale*radius*bs));       
    }
    intrad+=4;  //selection nodes cause it to go over the bounds slightly; this compensates.
    bounds.rassign(p2d.x-intrad,p2d.y-intrad,p2d.x+intrad,p2d.y+intrad);
    return &bounds;      
}




/*                               V = Any_vertex - Eye_position
N = Polygon normal

  If(N.V < 0) back_facing triangle
  Else front_facing triangle
  
	Note these vectors do not need to be normalised as we are only interested in the sign of the dot
	product, not the magnitude. It's always worked for me.
*/

void t3dmesh::clearvertexandedgeflags()
{
	vertex *v=(vertex*)vertexlist.first;  
	tedge *e=(tedge *)edgelist.first;
	while (v!=NULL)
	{
		v->attribute &=~(naPERSPECTIVECALCED+naVISIBLE);
		v=v->nxt();
    }
    while (e!=NULL)
    {
		e->attribute |= eaNOTDRAWNYET;
		e=(tedge*)e->next;
    }
}

void t3dmesh::dumppolygons()
{
	tpolygon *p=getpolygonhead();
	while (p)
	{		
		T3Daddvisiblepolygon(p);		
		p=(tpolygon*)p->next;
	}
}


static void calccolours(tscene *scene,tpolyvertex *vr,vertex *v,vector *lookv,tpolygon *pl)
{
    static vector normal;	
	static UINT32 at;							
	static float tempt;
	static material *mat;
	at=pl->attribute;
	
	mat=(vr->mat)?vr->mat:T3Dmaterial;
	if (at & paSHADED)
		normal=(at & paBACKFACING)?-v->normal:v->normal;
    else
		normal=(at & paBACKFACING)?-pl->normal:pl->normal;
    scene->intensity(normal,v->data3d,*lookv,*mat,vr->outcolour);                    
    if (at & paTRANSPARENT)
    {
		tempt=(float)mat->transparency-(T3Dspecdp*1.5f);
        BOUND(tempt,FLT_0,mat->transparency);
		vr->outcolour.a=tempt;
    }
	vr->outcolour.a*=cmds.ascale;
	vr->outcolour.r*=cmds.rscale;
	vr->outcolour.g*=cmds.gscale;
	vr->outcolour.b*=cmds.bscale;
}

void t3dmesh::do3d(matrix4x4 *cmat,matrix4x4 *cmattr,tcamera *cam,trender *cmds)
{
	static UINT32 n_clipped; //counter for clipped verticies.  
	static flt temp;
	
	static tclipvert *cvlist;
	//INT16 dx1,dy1,dx2,dy2;
	static tpolygon *pl;
	static tpolygon *wpl;//working poly  
	static tpolyvertex *vr,*end;
	static tpoint p1,p2,p3;  
	static vertex *v; 
	static vector p;
	static vector normal;	
	
	static vector data3d;
	static matrix4x4 cmatfov;
	static vector lv,rv;
	static UINT32 facing ;
	
	//this rotates the camera normal so that it is in un-transformed object space
	//the dot product of this and the untransformed polygon normals are then used for backface removal
	//using 3 muls and 3 adds. Possibly more expensive than the 2 adds, 2 subs and 2 muls in the usual
	//method but then there's less setup code and less complexity so it's probably faster (and less buggy!)
	//learned of a new method using the viewers eye POSITION, must try it out.
	//also, backface culling could be performed before the vertices are transformed!
	
	normal=cam->p-worldmattr.p;	
	normal.multmatrixinv(&worldmattr,&lv);
	rv=lv;	
	rv.normalize();
	pl=(tpolygon*)polygonlist.first;
	scene->projectlights(&worldmattr);
	
	//test object bounds against tcamera view fustrum in here
	T3Dtotalpolys+=polygonlist.count;                                      
	getbounds(cmat,cam);  
	attribute &=~ofVISIBLE; //clear the flag, in case it was set the last time around   	
	p=cam->p;    
	cmatfov=*cmat;
	
	if (cam->camtype==cam3D)
	{      
		T3Dcliphint=T3Dsphereclipped(&cmat->p,tempradius);
		if (T3Dcliphint!=scOUTSIDE)
		{
			attribute |=ofVISIBLE;
			clearvertexandedgeflags();
			while (pl!=NULL)
			{
				//if ((pl->attribute & T3Dpolygonprocessmask)==T3Dpolygonprocessmaskresult)
				{
					pl->attribute &= ~(paBACKFACING+paVISIBLE);                           
					
					//needs to be scaled
					vector vp=lv-(pl->vlist->v->data3d*s);
					facing = (dot(pl->normal,vp)>FLT_0);
					
					if ((facing)||(pl->attribute & paDOUBLESIDED))
					{
						if (!facing)
							pl->attribute |=paBACKFACING;                                        
						
						wpl=NULL;
						n_clipped=0;                  
						pl->centre=FLT_0;                                    
						vr=pl->vlist;
						//pl->normal.multmatrix(cmattr,&pl->tempnormal);                 
						T3Dppoly=&T3Dclippolys[1];
						cvlist=T3Dppoly->p;
						UINT32 at=pl->attribute;
						
						for (end=&(vr=pl->vlist)[pl->npoints];vr<end;vr++,cvlist++)
						{
							v=vr->v;
							vr->e->attribute &=~(eaVISIBLE);                      
							
							if ((v->attribute & naPERSPECTIVECALCED)==0)
							{
								//v->normal.multmatrix(cmattr,&v->tempnormal);
								v->data3d.multmatrixtrans(cmat,&v->temp3d);
								
								temp=v->temp3d.z;
								if (temp>NEARZ)
								{
									temp=FLT_1/temp *FOV;
									v->point2d.x=(v->temp3d.x*temp)+T3Doffset.x;
									v->point2d.y=(v->temp3d.y*temp)+T3Doffset.y;
									if ((T3Dcliphint==scINSIDE)||(T3Dviewport.rcontains(v->point2d)))
										v->attribute |= naVISIBLE;                             
									else
									{
										v->attribute &= ~naVISIBLE;
										n_clipped++;
									}
								}else
								{
									v->attribute &= ~naVISIBLE;
									n_clipped++;
								}
								v->attribute|=naPERSPECTIVECALCED;
							}else if ((v->attribute & naVISIBLE)==0)
								n_clipped++;
							//calculate colours
							calccolours(scene,vr,v,&rv, pl);                            
							pl->centre+=v->temp3d;                                                              
						}                  
						
						if ((T3Dcliphint==scINSIDE))//||(n_clipped==0))
						{            
							wpl=pl;
							pl->attribute |=paALLVERTICESVISIBLE;
						}else                     
							wpl=T3Dcliptofrustum(pl);				  
						
						if (wpl!=NULL)              
						{                      
							pl->attribute|=paVISIBLE;
							wpl->attribute|=paVISIBLE;                                            
							wpl->centre/=(flt)wpl->npoints;                      
							
#ifdef SHOW_NORMALS                      
							temp=FLT_1/wpl->centre.z*FOV;
							wpl->c2d.x=(wpl->centre.x*temp)+T3Doffset.x;
							wpl->c2d.y=(wpl->centre.y*temp)+T3Doffset.y;                                                     
							
							centre=wpl->centre+(wpl->normal*4);
							temp=FLT_1/centre.z*FOV;
							
							wpl->n2d.x=(centre.x*temp)+T3Doffset.x;
							wpl->n2d.y=(centre.y*temp)+T3Doffset.y;                                                     
#endif                      
							T3Daddvisiblepolygon(wpl);
							
							T3Daddvisibleedges(wpl);
						}
					}
				}
				pl=pl->nxt();
			}
		}
	}else
	{     
		if (T3Dviewport.roverlap(bounds))
		{
			attribute |=ofVISIBLE;
			clearvertexandedgeflags();
			bool allinside=bounds.rinside(T3Dviewport);  //Only check for clipping if bounds overlaps the edges of Sc.viewport         
			trectflt prect;
			v=(vertex*)vertexlist.first;
			while (v!=NULL)
			{
				if ((v->attribute & naPERSPECTIVECALCED)==0)
				{
					data3d=v->data3d;
					v->point2d.x=(data3d.x*cmatfov.m00 +data3d.y*cmatfov.m01 +data3d.z*cmatfov.m02) +cmatfov.p.x;
					v->point2d.y=(data3d.x*cmatfov.m10 +data3d.y*cmatfov.m11 +data3d.z*cmatfov.m12) +cmatfov.p.y;
					if (allinside)                         
						v->attribute |= naVISIBLE;                             
					else
					{
						if ((T3Dviewport.rcontains(v->point2d)))                                                                               
							v->attribute |= naVISIBLE;                             
						else                            
							v->attribute &= ~naVISIBLE;                                                           
					}
					v->attribute|=naPERSPECTIVECALCED;
				} 
				v=v->nxt();    
			}
			while (pl!=NULL)
			{
				pl->attribute &=~paVISIBLE;                     
				if ((pl->attribute & T3Dpolygonprocessmask)==T3Dpolygonprocessmaskresult)
				{
					prect.rassign(32000,32000,-32000,-32000);                                                   
					//calculate 3d coordinates
					pl->attribute|=paALLVERTICESVISIBLE;
					for (end=&(vr=pl->vlist)[pl->npoints];vr<end;vr++)
					{
						v=vr->v;
						if (!allinside)
						{                          
							tpointflt p=v->point2d;                                     
							if (p.x<prect.a.x)
								prect.a.x=p.x;
							else if (p.x>prect.b.x)
								prect.b.x=p.x;                        
							if (p.y<prect.a.y)
								prect.a.y=p.y;
							else if (p.y>prect.b.y)
								prect.b.y=p.y;
							if ((v->attribute & naVISIBLE)==0)
								pl->attribute&=~paALLVERTICESVISIBLE;
						} 
					}
					
					if ((allinside)||(T3Dviewport.roverlap(prect)))
					{
						pl->attribute |=paVISIBLE;                                         
						T3Daddvisiblepolygon(pl);
						T3Daddvisibleedges(pl);                     
					}
				}
				pl=pl->nxt();
			}
		}else     
			pl=NULL;           
	}                                         
	
}
//draws the nodes, object centre etc.
void t3dmesh::rendermouseover(UINT32 level,void *element,tpointfloat *offset,tcamera *cam)
{
	tpolyvertex *vr,*end,*pv;
	tedge *ce;

	tpolygon *ply,*ply2;
	tpointfloat p,p2;
	vector normal,lv,vp;
	bool drawit;
	bool facing;
	if (element)
	switch (level)
	{
		case 0:
			p=((vertex*)element)->point2d+*offset;
	        line(float2int(p.x-5),float2int(p.y-5),float2int(p.x+5),float2int(p.y+5));
		    line(float2int(p.x+5),float2int(p.y-5),float2int(p.x-5),float2int(p.y+5));        			
			break;
		case 1:
			p=((tedge*)element)->v[0]->point2d+*offset;
			p2=((tedge*)element)->v[1]->point2d+*offset;            
			line (float2int(p.x),float2int(p.y),float2int(p2.x),float2int(p2.y));
			break;
		case 2:
			ply=(tpolygon*)element;				
			vr=ply->vlist;       
			pv=(end=&vr[ply->npoints])-1;                                              
			for (;vr<end;pv=vr++)
			{                        
	            p=vr->v->point2d+*offset;
		        p2=pv->v->point2d+*offset;
			    line(float2int(p.x),float2int(p.y),float2int(p2.x),float2int(p2.y));
			}
			break;
		case 3:
			ce=getedgehead();
			normal=cam->p-worldmattr.p;	
			normal.multmatrixinv(&worldmattr,&lv);			
			//lv.normalize();
			
			while (ce)
			{	
				ply=ce->pref[0];
				ply2=ce->pref[1];
				drawit=false;
				if (!ply2)
				{
					vp=lv-(ply->vlist->v->data3d);
					if ((dot(ply->normal,vp)>FLT_0))
						drawit=true;
				}else if (!ply)
				{
					vp=lv-(ply2->vlist->v->data3d);
					if ((dot(ply2->normal,vp)>FLT_0))					
						drawit=true;
				}else
				{
					vp=lv-(ply->vlist->v->data3d);
					facing = (dot(ply->normal,vp)>FLT_0);
					lv-(ply2->vlist->v->data3d);
					if (facing!=(dot(ply2->normal,vp)>FLT_0))
						drawit=true;										
				}
				if (drawit)
					line(ce->v[0]->point2d.x+offset->x,ce->v[0]->point2d.y+offset->y,
						 ce->v[1]->point2d.x+offset->x,ce->v[1]->point2d.y+offset->y);					
								
				ce=(tedge*)ce->next;
			}
		break;
	}

}

void t3dmesh::renderhandles(UINT32 attrib,tcamera *cam)
{
    static tpointflt c;
    t_col=T3DCOLselectednode;
    if (cam->camtype!=cam3D)        
		circle(float2int(p2d.x),float2int(p2d.y),3);
    
    static vertex *v;
	v=(vertex*)vertexlist.first;
        
    if (attrib & rhVERTICES)
		if (attribute & ofVISIBLE) //anything visible?
		{
			//do check on attrib (ie, render all nodes or render only selected ones)
			while (v!=NULL)
			{
				if ((v->attribute & T3Dvertexprocessmask)==T3Dvertexprocessmaskresult)
				{
					if (v->attribute & naSELECTED) //individual check
					{
						if (v->attribute & naINCLUDEDINEDGE)
							t_col=T3DCOLboundingbox;
						else
							t_col=T3DCOLselectednode;
						c.x=v->point2d.x;
						c.y=v->point2d.y;
						drawbytes(float2int(c.x-FLT_3),float2int(c.y-FLT_3),1,5,nodebm);
					}else if ((T3Dsceneattributes & sfONLYSHOWSELECTEDNODES)==0)
					{
						t_col=T3DCOLnode;
						c.x=v->point2d.x;
						c.y=v->point2d.y;
						drawbytes(float2int(c.x-FLT_3),float2int(c.y-FLT_3),1,5,nodebm);
					}					
				}
				v=v->nxt();
			}
		}
	if (attrib & rhOBJECTS)
	{
		//draw object outline
		if (attribute & ofSELECTED)
		{
		}
	}

}

void t3dmesh::settexture(textureref *tex,UINT8 selectedonly)
{
    tpolygon *cpoly;
    cpoly=(tpolygon*)polygonlist.first;
    while (cpoly!=NULL)
    {
        //if (((cpoly->attribute&paSELECTED)!=0)||(selectedonly==0))
        cpoly->tex=tex;
        (tex!=NULL)?cpoly->attribute|=paTEXTURED:cpoly->attribute&=~paTEXTURED;
        cpoly=cpoly->nxt();
    }    
}

void t3dmesh::setmaterial(material *mat,UINT8 selectedonly)
{
    tpolygon *cpoly;
    cpoly=(tpolygon*)polygonlist.first;
    while (cpoly!=NULL)
    {
        if (((cpoly->attribute&paSELECTED)!=0)||(selectedonly==0))
			cpoly->setmaterial(mat);
        cpoly=cpoly->nxt();
    }
}

void t3dmesh::setpolyattributes(UINT32 attrib,UINT8 selectedonly)
{
    tpolygon *cpoly;
    cpoly=(tpolygon*)polygonlist.first;
    while (cpoly!=NULL)
    {
        if (((cpoly->attribute&paSELECTED)!=0)||(selectedonly==0))
			cpoly->attribute=attrib;
        cpoly=cpoly->nxt();
    }
}

void t3dmesh::setvertexattributes(UINT32 attrib,UINT8 selectedonly)
{
    vertex *cvertex=(vertex*)vertexlist.first;
    while (cvertex!=NULL)
    {
        if (((cvertex->attribute&naSELECTED)!=0)||(selectedonly==0))
			cvertex->attribute=attrib;
        cvertex=(vertex*)cvertex->next;
    }    
}

void t3dmesh::and_vertexattributes(UINT32 attribute)
{
    vertex *cvertex=(vertex*)vertexlist.first;
    while (cvertex!=NULL)
    {        
        cvertex->attribute&=attribute;
        cvertex=(vertex*)cvertex->next;
    }
}

void t3dmesh::or_vertexattributes(UINT32 attribute)
{
    vertex *cvertex=(vertex*)vertexlist.first;
    while (cvertex!=NULL)
    {        
        cvertex->attribute|=attribute;
        cvertex=(vertex*)cvertex->next;
    }
}

void t3dmesh::and_edgeattributes(UINT32 attribute)
{
    tedge *cedge=(tedge *)edgelist.first;
    while (cedge!=NULL)
    {        
        cedge->attribute&=attribute;
        cedge=(tedge*)cedge->next;
    }
}

void t3dmesh::or_edgeattributes(UINT32 attribute)
{
    tedge *cedge=(tedge *)edgelist.first;
    while (cedge!=NULL)
    {        
        cedge->attribute|=attribute;
        cedge=(tedge*)cedge->next;
    }
}


void t3dmesh::calculatenormals(UINT32 polymask,UINT32 vertexmask)
{    
    tpolygon *cpoly=(tpolygon*)polygonlist.first;
    
    vertex *cvertex=(vertex*)vertexlist.first;
    
    vector n=vector(0,0,0);
    
    while (cvertex)
    {
        if ((cvertex->attribute & vertexmask)!=0)
            cvertex->normal=n;
        cvertex=cvertex->nxt();
    }
    while (cpoly)
    {
        if ((cpoly->attribute & polymask)!=0)
        {
			cpoly->calcpolynormal();
			for (UINT32 i=0;i<cpoly->npoints;i++)
			{
				cpoly->vlist[i].normal=cpoly->normal;

				
                vertex *v=cpoly->vlist[i].v;
                if (v->attribute & vertexmask)                                
					v->normal+=cpoly->normal;
			}
        }
        cpoly=cpoly->nxt();        
    }
    vector calced;
    min.moveto(30000,30000,30000);
    max.moveto(-30000,-30000,-30000);    
    radius=0;
    flt tmp;
    cvertex=(vertex*)vertexlist.first;
    while (cvertex)
    {
        calced=cvertex->data3d;               
        if (min.x>calced.x) min.x=calced.x;
        if (min.y>calced.y) min.y=calced.y;
        if (min.z>calced.z) min.z=calced.z;
        if (max.x<calced.x) max.x=calced.x;
        if (max.y<calced.y) max.y=calced.y;
        if (max.z<calced.z) max.z=calced.z;
        
        tmp=calced.magnitude();
        if (radius<tmp)
            radius=tmp;                
        if (cvertex->attribute & vertexmask)                
            cvertex->normal.normalize();
		
        cvertex=cvertex->nxt();
    }
    //the almighty fudge factor
    
    radius=(flt)sqrt(radius);
    radius+=radius*FLT_TWEAK;
    /*vector absmax=vector(MAX(abs(min.x),max.x),
	MAX(abs(min.y),max.y),
	MAX(abs(min.z),max.z));
    radius=absmax.length();*/
    
    //radius*=2.0;
	
}

//////////////////////////////////////////////////////////////////////////////////////
//TVertex


vertex *vertex::nxt()
{
    if (this==NULL)
    {
        logmessage( (void*)&textcol, "bad call to next pointer");
        return NULL;
    }
    return (vertex*)next;
}


//////////////////////////////////////////////////////////////////////////////////////
//Tedge

UINT32 tedge::checkintegrity()
{
    if (!v[0] || !v[1]) return eiVERTEXNULLS;
    if (v[0]->parent!=v[1]->parent) return eiVERTEXPARENTSDAMAGED;
    if (attribute & eaREFERENCEDBYROUGEPOLYGONS) return eiROUGEPOLYGONWARNING;
    return eiOK;
}

bool tedge::sharesvertexwith(tedge *e)
{
    //if (e==this) return FALSE;
    if ((v[0]==e->v[0]) || (v[0]==e->v[1])|| (v[1]==e->v[0]) || (v[1]==e->v[1])) return TRUE;
    //if (v[0]->num==e->v[0]->num || v[0]->num==e->v[1]->num || v[1]->num==e->v[1]->num) return TRUE;
    return FALSE;
}

UINT32 tedge::sameas(tedge *e)
{
    if (e->v[0]==v[0] && e->v[1]==v[1]) return 1;
    if (e->v[1]==v[0] && e->v[0]==v[1]) return 2;
    return 0;
}

UINT32 tedge::sameas(vertex *v1,vertex *v2)
{
    if (v1==v[0] && v2==v[1]) return 1;
    if (v2==v[0] && v1==v[1]) return 2;
    return 0;
}

bool tedge::isboundary()
{ 
	return ((!pref[0] || !pref[1])||( (pref[0]->attribute & paBACKFACING) != (pref[1]->attribute & paBACKFACING)) );
}

tpolygon *NULLPOLYPTR=NULL;

UINT32 tedge::countreferences(UINT32 attribute)
{
    polycyclevars pcv;
    UINT32 count=0;
    tpolygon *p=*startprefcycle(&pcv);
    while (p)
    {
        if ((p->attribute & attribute)==attribute)
			count++;
        p=*getnextpref(&pcv);
    }
    return count;    
}

tpolygon **tedge::startprefcycle(polycyclevars *pcv)
{
    pcv->curr=pcv->prev=NULL;
    
    if (pref[0])
    {
		pcv->polycycleindex=0;
		return &pref[0];      
    }
    
    if (pref[1])
    {
		pcv->polycycleindex=1;
		return &pref[1];      
    }
    //logmessage((void*)&textcol,"starting ref cycle at rouge list");
    return startprefcycleatrougelist(pcv);    
}

tpolygon **tedge::startprefcycleatrougelist(polycyclevars *pcv)
{
    if (attribute & eaREFERENCEDBYROUGEPOLYGONS)
    {
        pcv->polycycleindex=2; //out of range, going to rouge list 
        pcv->prev=NULL;
        pcv->curr=(tpolygonref *)parent->rougepolygonrefs.first;
        if (!pcv->curr) return &NULLPOLYPTR;
        while (pcv->curr->e!=this)
        {
			pcv->prev=pcv->curr;
			(pcv->curr=(tpolygonref*)pcv->curr->next);
			if (!pcv->curr) return &NULLPOLYPTR;       
        }
        return &pcv->curr->p;
    }
    return &NULLPOLYPTR;
}


tpolygon **tedge::getnextpref(polycyclevars *pcv)
{
    if (pcv->polycycleindex<2)
    {
        pcv->polycycleindex++;
        if (pcv->polycycleindex<2)
			return &pref[pcv->polycycleindex];
        else
        {
			//logmessage((void*)&textcol,"ran out of normal vertices, trying rouge list");
			return startprefcycleatrougelist(pcv);
        }
    }
    while (pcv->curr)
    {
		pcv->prev=pcv->curr;
		(pcv->curr=(tpolygonref*)pcv->curr->next);
		if (pcv->curr)
			if (pcv->curr->e==this) return &pcv->curr->p;
    }
    return &NULLPOLYPTR;
	
}

bool tedge::deletereference(polycyclevars *pcv)
{
    if (pcv->polycycleindex<2)
    {
        pref[pcv->polycycleindex]=NULL;
        return TRUE;
    }
    tpolygonref *delme=(tpolygonref*)(parent->rougepolygonrefs.remove((telem*)pcv->prev,(telem*)pcv->curr));
    delete delme;    
    pcv->curr=(tpolygonref*)pcv->prev;
    return TRUE;
}

bool tedge::findpolygonref(tpolygon *p)
{
    polycyclevars pcv;
    tpolygon *pc=*startprefcycle(&pcv);
    while (pc)
    {
        if (pc==p) return TRUE;
        pc=*getnextpref(&pcv);
    }
    return FALSE;
}

bool tedge::reclaimfromrougelist()
{
    polycyclevars pcv;
    //printf (buf,"trying to remove rouge polygons from %i",this);
    //logmessage((void*)&textcol,buf);
    tpolygon *pc=*startprefcycleatrougelist(&pcv);
    
    
    if (pc)
    {                                
        if (!pref[0])
        {
			pref[0]=pc;           
			deletereference(&pcv);                   
			pc=*getnextpref(&pcv);
        }
        //pc=startprefcycleatrougelist(&pcv);
        
        if (!pref[1] && pc)
        {
			pref[1]=pc;       
			deletereference(&pcv);
			
        }                             
        
        //pc=startprefcycleatrougelist(&pcv);
        
        if (!getnextpref(&pcv))
        {
			attribute &=~eaREFERENCEDBYROUGEPOLYGONS;
			//logmessage((void*)&textcol,"No more rouge polys, whoohoo");
        }
        
        return TRUE;
    }//else
	//  logmessage((void*)&textcol,"failed to find any");
    
    return FALSE;
}

int tedge::isclockwise()
{
	
	polycyclevars pcv;
	tpolygon *currp=*startprefcycle(&pcv);
	while (currp!=NULL)
	{
		if (currp->attribute & paSELECTED)
		{
			tpolyvertex *vp,*end,*pv;
			vp=currp->vlist;
			pv=&vp[currp->npoints-1];
			for (end=&vp[currp->npoints];vp<end;pv=vp++)
			{
				if (pv->v==v[0] && vp->v==v[1]) return TRUE;
				if (pv->v==v[1] && vp->v==v[0]) return FALSE;               
				/*if (v->e==this)               
				return (v->attribute & pvaEDGEISANTICLOCKWISE)?FALSE:TRUE;                 */
			}
		}
		currp=*getnextpref(&pcv);       
	}
	return NEITHERTRUENORFALSE;
}

bool tedge::contains(vertex *cv)
{
    return (v[0]==cv || v[1]==cv);
}

UINT32 tedge::selectedvertexcount()
{
    UINT32 retcount=0;
    if (v[0]->attribute & naSELECTED) retcount++;
    if (v[1]->attribute & naSELECTED) retcount++;
    return retcount;
}

tpolygon *tedge::addreference(tpolygon *p)
{
    tpolygonref *pr;
    
    if (!p || pref[0]==p || pref[1]==p) return NULL;
    if (pref[0]==NULL)
    {
		pref[0]=p;return p;
    }
    if (pref[1]==NULL)
    {
		pref[1]=p;return p;
    }
	
    t3dmesh *parnt=parent;
    if (attribute & eaREFERENCEDBYROUGEPOLYGONS)
    {
		pr=(tpolygonref*)parnt->rougepolygonrefs.first;
		while (pr)
		{
			if ((pr->p==p) && (pr->e==this)) return NULL;
			pr=(tpolygonref*)pr->next;        
		}
    }
    
    attribute |= eaREFERENCEDBYROUGEPOLYGONS;
    logmessage((void *)&textcol,"Got a rouge polygon");    
    pr=new tpolygonref(p,this);
    logmessage((void *)&textcol,"created new reference");
	
    
    
    parnt->rougepolygonrefs.addtofront((telem*)pr);
    logmessage((void *)&textcol,"added polygon ref to rouge list polygon");
	
    return p;
    
}


bool tedge::deletereferences()
{
    polycyclevars pcv;
    tpolygon *p=*startprefcycle(&pcv);    
    if (!p) return FALSE;
    logmessage((void*)&textcol,"deleting references");
    
    while (p!=NULL)
    {
        deletereference(&pcv);
        p=*getnextpref(&pcv);
    }
    reclaimfromrougelist();
    return TRUE;
}

bool tedge::deletereferences(UINT32 attribute)
{
    polycyclevars pcv;
    tpolygon *p=*startprefcycle(&pcv);
    bool anyfound=FALSE;
    
    while (p!=NULL)
    {
        if ((p->attribute & attribute)==attribute)
        {
			deletereference(&pcv);
			anyfound=TRUE;
        }
        p=*getnextpref(&pcv);
    }
    reclaimfromrougelist();
    return anyfound;           
}

bool tedge::replacereference(tpolygon *oldpoly,tpolygon *newpoly)
{
	polycyclevars pcv;
    tpolygon **p=startprefcycle(&pcv);
    
    while (p!=NULL)
    {
        if (*p==oldpoly)
        {
			*p=newpoly;
			return TRUE;
        }
        p=getnextpref(&pcv);
    }
	
    return FALSE;
}

bool tedge::deletereferences(tpolygon *cp)
{
    polycyclevars pcv;
    tpolygon *p=*startprefcycle(&pcv);
    
    while (p!=NULL)
    {
        if (p==cp)
        {
			if (pcv.polycycleindex>=2)
				logmessage((void*)&textcol,"deleted rouge reference");
			deletereference(&pcv);
			reclaimfromrougelist();
			return TRUE;
        }
        p=*getnextpref(&pcv);
    }
	
    return FALSE;
}


vertex *t3dmesh::getvertex(UINT32 idx)
{
    vertex *cvertex=(vertex*)vertexlist.first;
    if (idx>=vertexlist.count) return cvertex;
    while (idx>0 && cvertex!=NULL)
    {        
        cvertex=cvertex->nxt();
        idx--;
    }
    return cvertex;     
}


tpolygon *t3dmesh::getpoly(UINT32 idx)
{
    tpolygon *cpoly=(tpolygon*)polygonlist.first;
    if (idx>=polygonlist.count) return cpoly;
    while (idx>0 && cpoly!=NULL)
    {        
        cpoly=(tpolygon*)cpoly->next;
        idx--;
    }
    return cpoly;     
}

UINT32 vertcount=0; 

vertex *t3dmesh::addvertex(flt x1,flt y1,flt z1,vertex *addvertex)
{    
    vector calced(x1,y1,z1);        
    addvertex->parent=this;
    addvertex->attribute=T3Ddefaultvertexattributes;
    addvertex->anedge=NULL;
    addvertex->num=vertcount++;
    //if (T3Dctm)
    //   LMmatrixtransformvec(T3Dctm,&calced,&addvertex->data3d);
    //else
	addvertex->data3d=calced;
    vertexlist.push(addvertex);    
    return addvertex; //was successfull, return the obj
}

vertex *t3dmesh::addvertex(flt x1,flt y1,flt z1)
{
    //should not use new; has to be freed using 'free'
    vertex *addvertex=T3Dcreatevertex(x1,y1,z1,NULL);
    vector calced(x1,y1,z1);        
    addvertex->parent=this;
    addvertex->attribute=T3Ddefaultvertexattributes;
    addvertex->anedge=NULL;
    addvertex->num=vertcount++;
    if (T3Dctm)
		LMmatrixtransformvec(T3Dctm,&calced,&addvertex->data3d);
    else
		addvertex->data3d=calced;
    vertexlist.push(addvertex);    
    return addvertex; //was successfull, return the obj
}

//cutdown quick version
vertex *t3dmesh::addvertexsimp(flt x1,flt y1,flt z1,vertex *addvertex)
{    
    addvertex->data3d.moveto(x1,y1,z1);      
    addvertex->parent=this;
    addvertex->num=vertcount++;
    addvertex->anedge=NULL;
    addvertex->attribute=T3Ddefaultvertexattributes;           
    vertexlist.push(addvertex);    
    return addvertex; //was successfull, return the obj
}


void t3dmesh::buildedges()
{
    tpolygon *p=getpolygonhead();
    UINT32 reversed;
    UINT32 i=0,j;
    while (p!=NULL)
    {        
        tpolyvertex *v,*end,*oldv;
        oldv=end=&(v=p->vlist)[p->npoints];                
        oldv--;
        j=0;
        for (;v<end;v++)
        {
            if (!v->e)
            {
				v->e=edgesearch(v->v,oldv->v,&reversed);		
				if (!v->e)
					v->e=addedge(v->v,oldv->v);
				if (reversed==TRUE)
					v->attribute |=pvaEDGEISANTICLOCKWISE;
				
				v->e->addreference(p);
				//sprintf(buf,"working on vertex:%i %i %i %f",i,j,(UINT32)v,v->v->data3d.x);            
				//logmessage((void*)&textcol,buf);
				
				v->v->anedge=v->e;
            }            
            oldv=v;
        }
        i++;
        p=(tpolygon*)p->next;
    }
}

tpointflt *RotateTextureVecs(tpointflt *txvecs,UINT16 count,UINT16 angle)
{
	
    UINT16 i;
    UINT16 iv;
    tpointflt temppnt;
    if (!txvecs) return NULL;
    for (i=0; i<angle;i++)
    {
        temppnt=txvecs[count-1];
        for (iv=0;iv<count-1;iv++)
            txvecs[iv+1]=txvecs[iv];
        txvecs[0]=temppnt;
    }
    return txvecs;
}

tpointflt *FlipTextureVecsX(tpointflt *txvecs,UINT16 count, textureref *tex)
{
    int i;
    if ((!txvecs)||(!tex)) return NULL;
    for (i=0;i<count;i++)
		txvecs[i].x=tex->t->size.x-txvecs[i].x;
    return txvecs;  
}    


tpointflt *FlipTextureVecsY(tpointflt *txvecs,int count, textureref *tex)
{
    int i;
    if ((!txvecs)||(!tex)) return NULL;
    for (i=0;i<count;i++)
		txvecs[i].y=tex->t->size.y-txvecs[i].y;
    return txvecs;  
}            

void T3Dapplytexturemanip(tpointflt *txvecs,int count,textureref *tex,int T3Dpolycreationflags)
{
    if (T3Dpolycreationflags & cfFLIPTEXTUREX)
        FlipTextureVecsX(txvecs,count,tex);
    if (T3Dpolycreationflags & cfFLIPTEXTUREY)
        FlipTextureVecsY(txvecs,count,tex);
    if (T3Dpolycreationflags & cfROTATETEXTUREMASK)
        RotateTextureVecs(txvecs,count,(T3Dpolycreationflags & cfROTATETEXTUREMASK)>>cfROTATETEXTURESHIFTRIGHT);
}




//this adds a vertex reference to the polygon list and also add the polygon to the vertex's reference list.
//UV values are also loaded

tedge *t3dmesh::addedge(vertex *v,vertex *v1,UINT8 *mem)
{
    
    tedge *newe=T3Dcreateedge(v,v1,mem);
	newe->parent=this;
	
    /*if (edgelist.count==0)    //first==NULL
	edgelist.first=newe;
    getch();
    sprintf(buf,"p:%i",edgelist.last);
    logmessage((void *)&textcol,buf);               
    if (edgelist.last!=NULL)
    {
	sprintf(buf,"x:%f y:%f",((tedge *) edgelist.last)->v[0]->data3d.x,((tedge *) edgelist.last)->v[0]->data3d.y);
	logmessage((void *)&textcol,buf);                           
	edgelist.last->next=newe;
    }    
    newe->next=NULL;
    edgelist.last=newe;    
    edgelist.count++;
    logmessage((void *)&textcol,"done");                   
    getch();
	
	  if (edgelist.last!=NULL)
	  {
	  sprintf(buf,"x:%f y:%f",((tedge *) edgelist.last)->v[0]->data3d.x,((tedge *) edgelist.last)->v[0]->data3d.y);
	  logmessage((void *)&textcol,buf);                           
	  edgelist.last->next=newe;
	  }
	  
		sprintf(buf,"p:%i",edgelist.last);
    logmessage((void *)&textcol,buf);*/
	
    edgelist.push(newe);    
	
    
    
    return newe;
}
/*
tedge *t3dmesh::addedgesearch(vertex *v,vertex *v1,UINT8 *mem,tedge **list,UINT32 count)
{
tedge *newe=T3Dcreateedge(v,v1,mem);
edgelist.push(newe);
return newe;
}*/

tedge *t3dmesh::completeedge(tedge *e1,tedge *e2)
{
	if (e1->v[0]==e2->v[1])
		return addedge(e1->v[1],e1->v[0]);
	if (e1->v[1]==e2->v[0])
		return addedge(e1->v[0],e1->v[1]);
	if (e1->v[0]==e2->v[0])
		return addedge(e1->v[1],e1->v[1]);
	if (e1->v[1]==e2->v[1])
		return addedge(e1->v[0],e1->v[0]);
	return NULL;	
}

tedge *t3dmesh::edgesearch(vertex *v,vertex *v1,UINT32 *reversed,tedge *newe,INT32 count)
{
    *reversed=0;    
	if (!newe)
		newe=getedgehead();	
	if (!newe) return NULL;
    while (count)
    {
        if (newe->v[0]==v && newe->v[1]==v1) return newe;
        if (newe->v[1]==v && newe->v[0]==v1) {*reversed=pvaEDGEISANTICLOCKWISE;return newe;};        
        newe=(tedge *)newe->next;
		if (!newe) return NULL;
		count--;		
    }                       
    return NULL;
}

tedge *t3dmesh::edgesearch(vertex *v,vertex *v1,UINT32 *reversed,tedge **list,INT32 count)
{
    *reversed=0;
    tedge *newe;    
    for (int i=0;i<count;i++)
    {
        newe=list[i];
        if (newe->v[0]==v && newe->v[1]==v1) return newe;
        if (newe->v[1]==v && newe->v[0]==v1) {*reversed=pvaEDGEISANTICLOCKWISE;return newe;};                
    }    
    return NULL;
}

tedge *t3dmesh::addedgeloop(vertex *v,UINT32 count,UINT8 *mem)
{
    
    if (count<2 || v==NULL) return NULL;
    UINT32 i;
    tedge *retedge;
    vertex *nextv=(vertex *)v->next;
    if (!nextv) return NULL;
    vertex *firstv=v;    
    count--;
    retedge=(tedge*)edgelist.getlast();
    for(i=0;i<count;i++)
    {
        addedge(v,nextv,mem);
        if (mem) mem+=sizeof(tedge);        
        v=nextv;        
        nextv=nextv->nxt();        
    }
    addedge(v,firstv,mem);    
    return (tedge*)edgelist.getnext_check(retedge);
}

tpolygon* t3dmesh::addpoly(UINT32 count,UINT8 *mem)
{
    tpolygon *newp=T3Dcreatepolygon(count,mem);
	newp->parent=this;
    T3Dlastpolycreated=newp;
    polygonlist.push(newp);
    return newp;      
}    



void printvertex(vertex *vert,int i)
{
/*if (vert==NULL)        
//outconsole("It's a null");
else
    {*/
	char txt[255];
	sprintf(txt,"pos no:%i= %d %d %d",i,vert->data3d.x,vert->data3d.y,vert->data3d.z);
	//outconsole(txt);
    //}  
}




bool getvertices(tedge **e,vertex **v,INT32 count)
{	
	for (int i=0;i<count;i++)
	{
		tedge *e1,*e2;
		e1=e[i];e2=e[(i+1)%count];
		if ((e1->v[0]==e2->v[0])||(e1->v[0]==e2->v[1]))
			v[i]=e1->v[1];
		else
			if ((e1->v[1]==e2->v[0])||(e1->v[1]==e2->v[1]))
				v[i]=e1->v[0];
			else
				return false; //edges do not form continuous loop
	}	
	return true;
}

//for now, this assumes that e1 and e2 are connectable; i.e. the polygon created between them will
//be clockwise
tpolygon *t3dmesh::addpoly(vertex **v,tedge **e,INT32 count,UINT8 *mem)
{
	static vertex *vertices[256];
	//what a friendly routine, if the vertices aren't there, it finds them for you. joy.
	if (!v)
	{
		v=vertices;
		if (!getvertices(e,v,count))
			return NULL;
	}
	tpolyvertex *lastpv,*pv;
    tpolygon *newp=T3Dcreatepolygon(count,mem);
	newp->parent=this;
    UINT32 reversed=0;
	int i;
	if (T3Dpolycreationflags & cfANTICLOCKWISE)
		for (i=0;i<count;i++)	
			T3Dsetpolygonvertex(newp,v[i],count-i-1,0,NULL);
		else for (i=0;i<count;i++)	
			T3Dsetpolygonvertex(newp,v[i],i,0,NULL);
	pv=newp->vlist;
	lastpv=&pv[count-1];
	//search edge list for correct edge. Search may not be nessicary but do it for now
	for (i=0;i<count;i++)	
	{
		if (!(pv->e=edgesearch(pv->v,lastpv->v,&reversed,e,count)))
			return NULL;
		pv->e->addreference(newp);
		if (reversed)
			pv->attribute |= pvaEDGEISANTICLOCKWISE;			
		lastpv=pv;
		pv++;
	}
	polygonlist.push(newp);
	return newp;       
}

tpolygon *t3dmesh::addpoly(vertex *v,tedge *e,INT32 count,UINT8 *mem)
{
	tpolyvertex *lastpv,*pv;
	vertex *cv;	
    tpolygon *newp=T3Dcreatepolygon(count,mem);
	newp->parent=this;
    UINT32 reversed=0;
	int i;
	cv=v;
	if (T3Dpolycreationflags & cfANTICLOCKWISE)	
		for (i=0;i<count;i++)
		{
			T3Dsetpolygonvertex(newp,cv,count-i-1,0,NULL);
			cv=(vertex*)cv->next;
		}
	else	
		for (i=0;i<count;i++)
		{
			T3Dsetpolygonvertex(newp,cv,i,0,NULL);
			cv=(vertex*)cv->next;
		}
			
	pv=newp->vlist;
	lastpv=&pv[count-1];
	//search edge list for correct edge. Search may not be nessicary but do it for now	
	for (i=0;i<count;i++)	
	{
		if (!(pv->e=edgesearch(pv->v,lastpv->v,&reversed,e,count)))
			return NULL;
		pv->e->addreference(newp);
		if (reversed)
			pv->attribute |= pvaEDGEISANTICLOCKWISE;			
		lastpv=pv;
		pv++;
	}
			
	polygonlist.push(newp);
	
	return newp;       
}


int t3dmesh::removestrandedelements()
{
	int count=0;
	or_vertexattributes(naDELETEME);
    or_edgeattributes(eaDELETEME);
    tpolygon *cp=getpolygonhead();
    //de-or those elements that are referenced
    while (cp!=NULL)
    {
		cp->and_vertexattributes(~naDELETEME);
        cp->and_edgeattributes(~eaDELETEME);            
        cp=(tpolygon *)cp->next;
    }
    //wipe out the elements still tagged for deletion.
    gbl_mask=naDELETEME;              
    count+=vertexlist.removelots(checkremovevertexfunc,removevertexfunc);
    gbl_mask=eaDELETEME;
    count+=edgelist.removelots(checkremoveedgefunc,removeedgefunc);
	return count;
}

int t3dmesh::removevertices(int mask)
{
	gbl_mask=mask;
	return vertexlist.removelots(checkremovevertexfunc,removevertexfunc);	
}

int t3dmesh::removeedges(int mask)
{
	gbl_mask=mask;
	return edgelist.removelots(checkremoveedgefunc,removeedgefunc);                   
}

int t3dmesh::removepolygons(int mask)
{
	gbl_mask=mask;
	return polygonlist.removelots(checkremovepolygonfunc,removepolygonfunc);
}

//pass 'singleedgeconnect' an array, rather than specific edges
//These should be given in order and said function should determine the vertices
//that make up the polygon and later apply the edges.

void t3dmesh::edgeconnect(tedge *start1,tedge *start2,tpointflt *between,UINT32 edgecount)
{    
    UINT32 i;
    UINT8 ti;
	tedge *edges[4];
	edges[1]=start1;
	edges[3]=start2;
    
    flt addby;
    if ((start1==NULL)||(start2==NULL))return;
    
    tedge *firsttopedge;
	//*topedge=NULL,*bottomedge=NULL,
	
	
    addby=((T3Dpolycreationflags & cfWRAPTEXTURE)!=0)?FLT_1 / (flt)edgecount:FLT_1;
    
    T3Dtxvecspool[0].assign(0,between->x);
    T3Dtxvecspool[3].assign(addby,between->x);
    T3Dtxvecspool[2].assign(addby,between->y);
    T3Dtxvecspool[1].assign(0,between->y);
	
    firsttopedge=edges[2]=addedge(start1->v[0],start2->v[0]);   
	tpolygon *poly;
    for (i=0;i<edgecount-1;i++)
    {
        
        edges[0]=addedge(edges[1]->v[1],edges[3]->v[1]);    
		poly=addpoly(NULL,edges,4);		
		//return;
        poly->setuv(T3Dtxvecspool);    
        //topedge=bottomedge;
		edges[2]=edges[0];
        edges[1]=(tedge *)edges[1]->next;
        edges[3]=(tedge *)edges[3]->next;
        if (T3Dpolycreationflags & cfWRAPTEXTURE) 
			for(ti=0;ti<4;ti++)
				T3Dtxvecspool[ti].x+=addby;                                     
    }
	edges[0]=firsttopedge;    	            
    (addpoly(NULL,edges,4))->setuv(T3Dtxvecspool);
    //logmessage((void*)&textcol,"done last");
    
}

void t3dmesh::edgeconnect(tedge *start1,vertex *v,tpointflt *between,UINT32 edgecount)
{    
    UINT32 i;
    UINT8 ti;
    tedge *edges[3];
	edges[1]=start1;
    flt addby;
    flt txw=FLT_1,txh=FLT_1;
    if ((start1==NULL)||(v==NULL))return;    
    tedge *firsttopedge;    
    addby=((T3Dpolycreationflags & cfWRAPTEXTURE)!=0)?FLT_1 / (flt)edgecount:FLT_1;    
    T3Dtxvecspool[0].assign(0,between->x);
    //txvecs[3].assign(addby,between->x);
    T3Dtxvecspool[2].assign(addby,between->y);
    T3Dtxvecspool[1].assign(0,between->y);    
    firsttopedge=edges[0]=addedge(start1->v[0],v);    
	tpolygon *poly;
    for (i=0;i<edgecount-1;i++)
    {
        edges[2]=addedge(edges[1]->v[1],v);    
		poly=addpoly(NULL,edges,3);		
		poly->setuv(T3Dtxvecspool);
        //(singleedgeconnect(start1,topedge,bottomedge))->setuv(T3Dtxvecspool);
        edges[0]=edges[2];
        edges[1]=(tedge *)edges[1]->next;
        if (T3Dpolycreationflags & cfWRAPTEXTURE) 
			for(ti=1;ti<3;ti++)
				T3Dtxvecspool[ti].x+=addby;                                     
    }
	edges[2]=firsttopedge;
	poly=addpoly(NULL,edges,3);		
	poly->setuv(T3Dtxvecspool);
	
	
    //(singleedgeconnect(start1,bottomedge,firsttopedge))->setuv(T3Dtxvecspool);    
}

tedge *mksquare(t3dmesh *obj,flt px1,flt py1,flt px2,flt py2,flt pz)
{
    vertex *firstvertex;
	
    firstvertex=obj->addvertex(px1,py1,pz);
    obj->addvertex(px2,py1,pz);
    obj->addvertex(px2,py2,pz);
    obj->addvertex(px1,py2,pz);
    T3Dtxvecspool[0].x=FLT_0;  T3Dtxvecspool[0].y=FLT_0;
    T3Dtxvecspool[1].x=FLT_1;  T3Dtxvecspool[1].y=FLT_0;
    T3Dtxvecspool[2].x=FLT_1;  T3Dtxvecspool[2].y=FLT_1;
    T3Dtxvecspool[3].x=FLT_0;  T3Dtxvecspool[3].y=FLT_1;
    tedge *e;tpolygon *p;
    
    e=obj->addedgeloop(firstvertex,4);
	
    if (!(T3Dpolycreationflags & cfNOPOLYS))
    {               
        p=obj->addpoly(firstvertex,e,4);
        p->setuv(T3Dtxvecspool);        
    }
	
    return e;    
}

tedge *mkcircle(t3dmesh *obj,flt cx,flt cy,flt cz,flt rx,flt ry,flt angle,int edgecount)
{
    return mksuperelipse2d(obj,cx,cy,cz,rx,ry,angle,edgecount,FLT_1,FLT_1);
}




vertex *mkcube(t3dmesh *obj,flt x1, flt y1,flt z1,flt x2, flt y2,flt z2)
{
	UINT32 OLDCF=T3Dpolycreationflags;
	tpointflt between;   
	tedge *e1,*e2;
	between.x=0;between.y=1;
	if (z1<z2) T3Dpolycreationflags ^= cfANTICLOCKWISE;
	
	e1=mksquare(obj,x1,y1,x2,y2,z1);   
	T3Dpolycreationflags ^= cfANTICLOCKWISE | cfFLIPTEXTUREY;
	e2=mksquare(obj,x1,y1,x2,y2,z2);
	T3Dpolycreationflags ^= cfANTICLOCKWISE | cfFLIPTEXTUREY;   
	obj->edgeconnect(e1,e2,&between,4);   
	T3Dpolycreationflags=OLDCF;
	
	return NULL;
}                                       


vertex *mkcylinder(t3dmesh *obj,flt x1, flt y1,flt z1,flt z2,flt rx, flt ry, flt angle,int edgecount)                                     
{
	UINT32 OLDCF=T3Dpolycreationflags;
	
	tpointflt between(FLT_0,FLT_1);
	
	tedge *e1,*e2;
	
	if (z1<z2) T3Dpolycreationflags ^= cfANTICLOCKWISE;
	T3Dpolycreationflags|=cfNOPOLYS;   
	e1=mkcircle(obj,x1,y1,z1,rx,ry,angle,edgecount);
	e2=mkcircle(obj,x1,y1,z2,rx,ry,angle,edgecount);
	T3Dpolycreationflags ^=cfNOPOLYS;   
	obj->edgeconnect(e1,e2,&between,edgecount);   
	mkcircle(obj,x1,y1,z1,rx,ry,angle,edgecount);
	T3Dpolycreationflags ^=cfANTICLOCKWISE|cfFLIPTEXTUREX;   
	mkcircle(obj,x1,y1,z2,rx,ry,angle,edgecount);
	T3Dpolycreationflags = OLDCF; //restore polyflags
	return NULL;
}

vertex *mkring(t3dmesh *obj,flt x1, flt y1,flt z1,flt rx, flt ry,flt rx2, flt ry2,flt angle,int edgecount)                                     
{
	UINT32 OLDCF=T3Dpolycreationflags;   
	tpointflt between(FLT_0,FLT_1);
	tedge *e1,*e2;
	
	T3Dpolycreationflags|=cfNOPOLYS;
	T3Dpolycreationflags|=cfFLIPTEXTUREY|cfFLIPTEXTUREX;   
	e1=mkcircle(obj,x1,y1,z1,rx,ry,angle,edgecount);      
	e2=mkcircle(obj,x1,y1,z1,rx2,ry2,angle,edgecount);   
	T3Dpolycreationflags&=~cfNOPOLYS;
	obj->edgeconnect(e1,e2,&between,edgecount);
	T3Dpolycreationflags = OLDCF; //restore polyflags
	return NULL;
}



vertex *mksphere(t3dmesh *obj,flt x1, flt y1,flt z1,flt rx, flt ry, flt rz, flt angle,int radial,int laterial)
{
    return mksuperelipse(obj,x1,y1,z1,rx,ry,rz,angle,radial,laterial,FLT_1,FLT_1);
}

flt safepow(flt val,flt power)
{
    if (power==FLT_0) return val;
    if (val<0)
		return (flt)-pow(-val,power);    
    return (flt)pow(val,power);
}


tedge *mksuperelipse2d(t3dmesh *obj,flt cx,flt cy,flt cz,flt rx,flt ry,flt cval1,int edgecount,flt n1,flt n2)
{
    if (edgecount<3) return NULL;
    //logmessage( (void*)&textcol,"entered mkcircle");
    
    int i;
    flt txw=2*PI,txh=2*PI;
    vertex *firstvertex,*cv;
	
    flt cval2=-PI;
    flt delta2= (2*PI)/edgecount;
    //logmessage( (void*)&textcol,"entering main loop mkcircle");
    firstvertex=(vertex*)obj->vertexlist.getlast();    
    for (i=0;i<edgecount;i++)
    {
		
        if ((T3Dpolycreationflags & cfNOPOLYS)==0)
        {   
			T3Dtxvecspool[i].x=((flt)sin(cval2)/txw)+FLT_HALF;BOUND(T3Dtxvecspool[i].x,FLT_0,FLT_1);
			T3Dtxvecspool[i].y=((flt)cos(cval2)/txh)+FLT_HALF;BOUND(T3Dtxvecspool[i].y,FLT_0,FLT_1);           
        }
        
        flt pwrcos1=safepow((flt)cos(cval1),n1);
        cv=obj->addvertex(cx+pwrcos1*safepow((flt)cos(cval2),n2)*rx,cy+pwrcos1*safepow((flt)sin(cval2),n2)*ry,cz);
        cval2+=delta2;
        //logmessage( (void*)&textcol,"added vertex");         
    }
    firstvertex=(vertex*)obj->vertexlist.getnext_check(firstvertex);
    tedge *e;tpolygon *p;
    e=obj->addedgeloop(firstvertex,edgecount);
    if ((T3Dpolycreationflags & cfNOPOLYS)==0)
    {
        p=obj->addpoly(firstvertex,e,edgecount);
        p->setuv(T3Dtxvecspool);                
    }//logmessage( (void*)&textcol,"Done");                 
    return e;                               
}

vertex *mksuperelipse(t3dmesh *obj,flt x1, flt y1,flt z1,flt rx, flt ry, flt rz, flt angle,int radial,int laterial,flt n1,flt n2)
{  
	UINT32 OLDCF=T3Dpolycreationflags;
	tedge *e1,*e2=NULL;
	vertex *v;
	flt addby,cval,delta,zpos;
	
	int i;
	laterial--;
	addby=(FLT_1 / (flt)laterial);
	tpointflt between(0,addby);
	
	delta= (PI)/laterial;
	cval=-PI/2.0f;   
	zpos=z1 +(safepow((flt)sin(cval),n1) *rz);
	//logmessage( (void*)&textcol,"entered mksphere");
	
	v=obj->addvertex(x1,y1,zpos);
	cval+=delta;       
	
	T3Dpolycreationflags |= cfNOPOLYS;
	T3Dpolycreationflags ^= cfANTICLOCKWISE;   
	
	for (i=1;i<laterial;i++)
	{
		zpos=z1 + (safepow((flt)sin(cval),n1) *rz);
		e1=e2;       
		e2=mksuperelipse2d(obj,x1,y1,zpos,rx,ry,cval,radial,n1,n2);
		
		if (i==1)
			obj->edgeconnect(e2,v,&between,radial);       
		if (i>1)
			obj->edgeconnect(e1,e2,&between,radial);
		between.x+=addby;
		between.y+=addby; 
		cval+=delta;       
	}
	zpos=z1 + (safepow((flt)sin(cval),n1) *rz);
	v=obj->addvertex(x1,y1,zpos);
	T3Dpolycreationflags=OLDCF; 
	obj->edgeconnect(e2,v,&between,radial); 
	
	//logmessage( (void*)&textcol,"done\n");       
	return NULL;    
}


vertex *mkgrid(t3dmesh *obj,flt x1,flt y1,flt x2,flt y2,flt z,int gridx,int gridy,int checkergrid)
{
    //tedge *tope=NULL,*bottome=NULL,*righte=NULL,*lefte=NULL;
	tedge *edges[4];
    T3Dpolycreationflags ^=cfANTICLOCKWISE;
    material *mats[2];
    mats[0]=T3Dmaterial;
    if (T3Dmaterial->nextm())
		mats[1]=T3Dmaterial->nextm();
    else
		mats[1]=(material *)T3Dmaterialbank.first;
    
    flt xdiff=x2-x1,ydiff=y2-y1;
    tpointflt step;
    vertex *firstrow=(vertex*)obj->vertexlist.last,*lastv;
    vertex *secondrow=NULL,*v=NULL;
    int yi,xi,ti;
    
    for (yi=0;yi<=gridy;yi++)
    {
        step.y=yi/(float)(gridy);
        lastv=NULL;
        for (xi=0;xi<=gridx;xi++)
        {            
            step.x=xi/(float)(gridx);                                                           
            v=obj->addvertex(x1+(xdiff*step.x),y1+(ydiff*step.y),z);
            if (lastv)
				edges[0]=obj->addedge(lastv,v);
            if (!xi)
            {
                if (!yi)              
					firstrow=v;                               
                else if (yi==1)            
					secondrow=v;
            }else if (xi==1)
            {
                if (!yi)              
					edges[1]=edges[0];                               
                else if (yi==1)            
					edges[3]=edges[0];
            }
            lastv=v;                        
        }        
    }
    gridx--;
    gridy--;
    
    step.x=2.0f/gridx;
    step.y=FLT_1/gridy;
    
    T3Dtxvecspool[0].y=step.y;    
    T3Dtxvecspool[1].y=step.y;    
    T3Dtxvecspool[2].y=FLT_0;
    T3Dtxvecspool[3].y=FLT_0;    
	
    vertex *firstvertex=firstrow;
    if (!firstrow) return NULL;
    for (yi=0;yi<=gridy;yi++)
    {
        T3Dtxvecspool[0].x=FLT_0;
		T3Dtxvecspool[1].x=step.x;
		T3Dtxvecspool[2].x=step.x;
		T3Dtxvecspool[3].x=FLT_0;
        edges[0]=obj->addedge(firstrow,secondrow);
		
        int index=yi & 1;
        for (xi=0;xi<=gridx;xi++)
        {
            if (checkergrid)
				T3Dmaterial=mats[index];
            edges[2]=obj->addedge(firstrow->nxt(),secondrow->nxt());
			tpolygon *p=obj->addpoly(NULL,edges,4,NULL);
			p->setuv(T3Dtxvecspool);
			
            //obj->singleedgeconnect(lefte,righte,tope,bottome)->setuv(T3Dtxvecspool);            
            index ^=1;            
            firstrow =firstrow ->nxt();
            secondrow=secondrow->nxt();
            edges[1]=(tedge*)edges[1]->next;
            edges[3]=(tedge*)edges[3]->next;
            
            for (ti=0;ti<4;ti++)
                T3Dtxvecspool[ti].x+=step.x;
            edges[0]=edges[2];
        }
        for (ti=0;ti<4;ti++)
			T3Dtxvecspool[ti].y+=step.y;
        firstrow =firstrow ->nxt();
        secondrow=secondrow->nxt();
    }
    T3Dpolycreationflags ^=cfANTICLOCKWISE;
    return firstvertex;    
}     


/*vertex *t3dmesh::mktorus(vector centre,flt rx1,flt ry1,flt rx2,flt ry2,flt angle,int lat,int lon)
{
int lati
for (lati=0;lati<lat;lati++)
for (loni=0;loni<lon;loni++)
{
}
} */          

vertex *mkcone(t3dmesh *obj,flt x1, flt y1,flt z1,flt z2,flt rx, flt ry,flt angle,int edgecount)
{
	UINT32 OLDCF=T3Dpolycreationflags;
	tedge *e;vertex *v;
	tpointflt between(FLT_0,FLT_1);
	if (z2>z1) T3Dpolycreationflags ^= cfANTICLOCKWISE;
	v=obj->addvertex(x1,y1,z1);
	
	e=mkcircle(obj,x1,y1,z2,rx,ry,angle,edgecount);   
	T3Dpolycreationflags^=cfFLIPTEXTUREX+cfANTICLOCKWISE;
	T3Dpolycreationflags=OLDCF;
	obj->edgeconnect(e,v,&between,edgecount);
	//mkcircle(x1,y1,z2,rx,ry,angle,edgecount);
	
	return NULL;
}




//////////////////////////////////////////
// tcamera

tcamera::tcamera(vector p_,vector o_,flt fov)
{
    camtype=cam3D;
    p=p_;
    o=o_;
    type=otCAMERA;
    FOV=fov;
    NEARZ=0.1f;
    FARZ=150;    
}


tcamera::tcamera()
{
    camtype=cam3D;
    type=otCAMERA;
    FOV=500;
    NEARZ=0.1f;
    FARZ=1600;    
}

tcamera::~tcamera()
{
}

void tcamera::do3d(matrix4x4 *cmat,matrix4x4 *cmattr,tcamera *cam,trender *cmds)
{
    getbounds(cmat,cam);
    T3Dviewport.roverlap(bounds)?attribute|=ofVISIBLE:attribute &=~ofVISIBLE;
}


//draws a graphical representation of the camera
void tcamera::renderhandles(UINT32 attrib,tcamera *cam)
{
    if (cam->camtype!=cam3D)
    {
		t_col=getcolour2(255,255,255);
		tpoint p2di(float2int(p2d.x),float2int(p2d.y));    
		hline(p2di.x-5,p2di.x+5,p2di.y);
		vline(p2di.x,p2di.y-5,p2di.y+5);    
    }
}

void tcamera::rotate(flt x,flt y,flt z)
{
    o.moveto(x,y,z);    
    LMrottransinv(&localmattr,o.x,o.y,o.z,p.x,p.y,p.z);
    localmat=localmattr;
    calcworldmat();
}

void tcamera::scale(flt x,flt y,flt z)
{    
}


void tcamera::moveto(flt x,flt y,flt z)
{
    p.moveto(x,y,z);
    localmattr.p=p;
    
    localmat=localmattr;
    calcworldmat();
}

trectflt *tcamera::getbounds(matrix4x4 *cmat,tcamera* cam)
{
    vector wp=cmat->p;            
    T3Dproject(&wp,&p2d,cam)?attribute |=ofVISIBLE:attribute &=~ofVISIBLE;
    //p2d.x=(wp.x*cmat->m00 +wp.y*cmat->m01 +wp.z*cmat->m02) +cmat->p.x;
    //p2d.y=(wp.x*cmat->m10 +wp.y*cmat->m11 +wp.z*cmat->m12) +cmat->p.y;
    tempradius=FLT_1;
    bounds.rassign(p2d.x-6,p2d.y-6,p2d.x+6,p2d.y+6);
    return &bounds;
}




//////////////////////////////////////////
// tlights


tlight::tlight()
{
    type=otLIGHT;
    diffuse=trgbfloat(FLT_1,FLT_1,FLT_1);
    ambient=trgbfloat(FLT_0,FLT_0,FLT_0);
    specular=trgbfloat(FLT_1,FLT_1,FLT_1);
    
    lighttype=LTdirectional;
    conesize=FLT_1;
    intensity=FLT_1;        
}


tlight::~tlight()
{
}

void tlight::do3d(matrix4x4 *cmat,matrix4x4 *cmattr,tcamera*cam,trender *cmds)
{
    //rotate temporary tlight rotation
    o.multmatrix(cmat,&no);
    no.normalize();
    getbounds(cmat,cam);
    T3Dviewport.roverlap(bounds)?attribute|=ofVISIBLE:attribute &=~ofVISIBLE;
}



trectflt *tlight::getbounds(matrix4x4 *cmat,tcamera*cam)
{
    wp=cmat->p;    
    T3Dproject(&wp,&p2d,cam)?attribute |=ofVISIBLE:attribute &=~ofVISIBLE;
    tempradius=FLT_1;
    bounds.rassign(p2d.x-6,p2d.y-6,p2d.x+6,p2d.y+6);
    return &bounds;
}

//draws a graphical representation of the light
void tlight::renderhandles(UINT32 attrib,tcamera *cam)
{
    t_col=getcolour2(float2int(diffuse.r*255.0f),float2int(diffuse.g*255.0f),float2int(diffuse.b*255.0f));
    tpoint p2di(float2int(p2d.x),float2int(p2d.y));    
	
    hline(p2di.x-5,p2di.x+5,p2di.y);
    vline(p2di.x,p2di.y-5,p2di.y+5);
    line(p2di.x-5,p2di.y-5,p2di.x+5,p2di.y+5);
    line(p2di.x+5,p2di.y-5,p2di.x-5,p2di.y+5);    
}

//////////////////////////////////////////
// bounding box


tgrid3d::tgrid3d():tobject3d()
{
    attribute |= ofGRIDSNAP;
    type=otGRID;
    gridx=gridy=gridz=1.5;
    strcpy(name,"grid3d");
}    


tgrid3d::tgrid3d(float gridspacex,float gridspacey,float gridspacez):tobject3d()
{
    type=otGRID;
    gridx=gridspacex;
    gridy=gridspacey;
    gridz=gridspacez;
}

trectflt *tgrid3d::getbounds(matrix4x4 *cmat,tcamera*cam)
{
    return &T3Dviewport;
    
}

flt closestmultiple(flt f,flt s)
{
    static INT32 i_f,i_s;
	i_f=float2int(f*10000.0f);
    i_s=float2int(s*10000.0f);
    i_f %= i_s;   
    return f-i_f / 10000.0f;            
}

UINT32 tgrid3d::snapto(vector *v)
{
    if (attribute & ofGRIDSNAP)
    {
		vector old=*v;
		
		v->x=closestmultiple(v->x,gridx);
		v->y=closestmultiple(v->y,gridy);
		v->z=closestmultiple(v->z,gridz);
		return (old!=*v);
    }
    return TRUE;
}

flt tgrid3d::snapto(flt f)
{
    if (attribute & ofGRIDSNAP)        
		f=closestmultiple(f,gridx);
    
    return f;
    
	
}


void tgrid3d::do3d(matrix4x4 *cmat,matrix4x4 *cmattr,tcamera*cam,trender *cmds)
{    
    p=cmat->p;
    if (cam->camtype!=cam3D)
    {
		switch (cam->camtype)
		{
        case camORTHXY:
			tempgridx=gridx*T3Dscale;
			tempgridy=gridy*T3Dscale;
			break;
        case camORTHXZ:
			tempgridx=gridx*T3Dscale;
			tempgridy=gridz*T3Dscale;
			break;
        case camORTHZY:
			tempgridx=gridz*T3Dscale;
			tempgridy=gridy*T3Dscale;
			break;
		}
		
    }
}

void tgrid3d::drawgrid()
{
    static flt px,py;
    
    t_col=T3DCOLgridmajor;
    hline(float2int(T3Dviewport.a.x),float2int(T3Dviewport.b.x),float2int(p.y));
    vline(float2int(p.x),float2int(T3Dviewport.a.y),float2int(T3Dviewport.b.y));
    if (attribute & ofVISIBLE)
    {    
		t_col=T3DCOLgrid;
		
		if (tempgridx>=FLT_1)
		{
			px=p.x;
			while (px>=T3Dviewport.a.x)
				vline(float2int(px-=tempgridx),SC.viewport.a.y,SC.viewport.b.y);                    
			px=p.x;
			while (px<=T3Dviewport.b.x)      
				vline(float2int(px+=tempgridx),SC.viewport.a.y,SC.viewport.b.y);                
		}
		if (tempgridy>=FLT_1)
		{ 
			py=p.y;
			while (py>=SC.viewport.a.y)
				hline(SC.viewport.a.x,SC.viewport.b.x,float2int(py-=tempgridy));
			py=p.y;
			while (py<=SC.viewport.b.y)        
				hline(SC.viewport.a.x,SC.viewport.b.x,float2int(py+=tempgridy));                    
		}
    }
}



//////////////////////////////////////////
// bounding box

/*class tboundingbox3d:public tobject3d
{
trect bounds;
virtual void renderhandles(UINT8 attrib);    
virtual void do3d(matrix4x4 *,tcamera*);  
};*/


//////////////////////////////////////////
// The 3d cursor

tcursor3d::tcursor3d()
{
    strcpy(name,"3dcursor");
    type=otCURSOR;           
}

void tcursor3d::renderhandles(UINT32 attrib,tcamera *cam)
{    
    t_fillcol=T3DCOLcursor;
    tpoint p2di(float2int(p2d.x),float2int(p2d.y));    
    bar(p2di.x-6,p2di.y-1,p2di.x+6,p2di.y+1);
    bar(p2di.x-1,p2di.y-6,p2di.x+1,p2di.y+6);  
}

void tcursor3d::do3d(matrix4x4 *cmat,matrix4x4 *cmattr,tcamera *cam,trender *cmds)
{        
    getbounds(cmat,cam);    
    T3Dviewport.roverlap(bounds)?attribute|=ofVISIBLE:attribute &=~ofVISIBLE;
}

trectflt *tcursor3d::getbounds(matrix4x4 *cmat,tcamera*cam)
{
    wp=cmat->p;
    T3Dproject(&wp,&p2d,cam);
    bounds.rassign(p2d.x-6,p2d.y-6,p2d.x+6,p2d.y+6);
    tempradius=6.0f;
    return &bounds;
}

//////////////////////////////////////////
// The Scene

tscene::tscene()
{
    //since this is all OO, the size could be anything so pass 0    
    TTinittree(&scenegraph,0);
    tlights.init(sizeof(tlightref));
    polyattribmask=0xFFFFFFFF;    
}

tscene::~tscene()
{
    TTdestroytree(scenegraph.root);
    tlights.destroy();
}

//process the scene for a full redraw
void tscene::setupview(tcamera *cam,UINT16 offx,UINT16 offy)
{
    matrix4x4 tempmat;    
    
    T3Doffset.x=(flt)((SC.size.x >> 1)+offx);
    T3Doffset.y=(flt)((SC.size.y >> 1)+offy);
    
    SC.viewport.a.x+=offx;
    SC.viewport.a.y+=offy;
    SC.viewport.b.x+=offx;
    SC.viewport.b.y+=offy;
	T3Dviewport.a.x=(flt)SC.viewport.a.x;
	T3Dviewport.a.y=(flt)SC.viewport.a.y;
	T3Dviewport.b.x=(flt)SC.viewport.b.x;
	T3Dviewport.b.y=(flt)SC.viewport.b.y;
    
    
    if (cam->camtype==cam3D)
    {
        FOV=cam->FOV;
        NEARZ=cam->NEARZ;
        FARZ=cam->FARZ;
        T3Dscreenscale.x=SC.size.x / FOV;
        T3Dscreenscale.y=SC.size.y / FOV;
        T3Dmaxscale = MAX(T3Dscreenscale.x, T3Dscreenscale.y);
        T3Dmaxscreenscaleinv = FLT_1 / T3Dmaxscale;
        T3Dscreencentre.x= SC.size.x  / FLT_2 - FLT_HALF;
        T3Dscreencentre.y= SC.size.y  / FLT_2 - FLT_HALF;       
        T3Dsetupfrustumviewp();      
		
		vector v1(5,5,60);
		vector v2(10,10,100);
	//	T3Dbuildplanecube(frustumplanes,&v1,&v2);
        vector uvec=vector(0,0,-1);
        T3Dviewmat=cam->localmat;
        uvec.multmatrix(&T3Dviewmat,&T3Dlookv);
        
        LMinvert(&T3Dviewmat);
        
    }else
    {
        //stored orthographic camera stuff doesn't affect it at the moment.. calculating each frame.. eeks.
        //T3Dviewmat=cam->mat;
        //0.5 sorts out weird inaccuracy shit    
        LMtranslate(&T3Dviewmat,cam->p.x+T3Doffset.x+FLT_HALF,cam->p.y+T3Doffset.y+FLT_HALF,0);
        LMscale(&tempmat,T3Dscale,T3Dscale,T3Dscale);
        LMconcatenatematrices(&T3Dviewmat,&tempmat,&T3Dviewmat);
        switch (cam->camtype)
        {
		case camORTHXY:LMidentity(&tempmat);break;
		case camORTHZY:LMyrotation(&tempmat,-HALFPI);break;
		case camORTHXZ:LMxrotation(&tempmat,-HALFPI);break;
        }        
        LMconcatenatematrices(&T3Dviewmat,&tempmat,&T3Dviewmat);  
    }
}


void tscene::setfog(flt min,flt max,flt density)
{
    fogminz=min;
    if (max<min) max=min;
    fogmaxz=max;
    fogdepth=max-min;
    fogdensity=density;
}


void tscene::projectlights(matrix4x4 *mat)
{
	tlightref *lr=(tlightref*)tlights.first;

	while (lr)
	{
		tlight *l=lr->l;
		vector p=l->p-mat->p;
		p.multmatrixinv(mat,&l->wp);		
		l->o.multmatrixinv(mat,&l->no);		
		lr=(tlightref*)lr->next;
	}
}



void tscene::setdisplay(tsurface *surf)
{
	cmds.setrendersurface(surf);
}


void tscene::preprocess(tcamera *cam,UINT16 offx,UINT16 offy)
{
    //static tcamera *oldcam=NULL;
    
    matrix4x4 tempmat;
    tobject3d *root=(tobject3d*)scenegraph.root;
    T3Dresetbuffers();              
    setupview(cam,offx,offy);   
    //oldcam=cam;
    tobject3d *cobj=root;    
    //matrix4x4 startmat;
    //LMunit(&startmat);
    while (cobj!=NULL)
    {        
        cobj->do3dfull(&T3Dviewmat,&T3Dviewmat,cam,&cmds);
        cobj=(tobject3d*)cobj->next;
    }
	if (T3Dpolycount>0)
		if (cam->camtype==cam3D)
			T3Dsort(0,T3Dpolycount-1);
	
}


void tscene::preprocess_singleobject(tcamera *cam,tobject3d *obj,UINT16 offx,UINT16 offy)
{

	T3Dresetbuffers();              
	setupview(cam,offx,offy);
	obj->do3dfull(&T3Dviewmat,&T3Dviewmat,cam,&cmds);
	if (cam->camtype==cam3D)
		T3Dsort(0,T3Dpolycount-1);

}


//process scene for a single object and it's children
trectflt *tscene::getbounds(tcamera *cam,tobject3d *obj,UINT16 offx,UINT16 offy)
{
    matrix4x4 tmpmat;
    //preconcatinate cameraspace matrix to objects world matrix
    setupview(cam,offx,offy);
    LMconcatenatematrices(&T3Dviewmat,&obj->worldmat,&tmpmat);
    
    return obj->getbounds(&tmpmat,cam);    
}

void tscene::removeobject(tobject3d* object)
{
    tlightref *lref;
    switch (object->type)
    {
	case otLIGHT:
		//search for light reference, remove it
		lref=(tlightref*)tlights.first;
		while ((lref!=NULL)&&(lref->l!=(tlight*)object))
			lref=(tlightref*)lref->next;
		if (lref)
		{
			tlights.remove(lref);
		}
		break;
	case otGEOM:
        logmessage((void *)&textcol,"removing object");
        break;
    }    
    TTremovenode(&scenegraph,(ttreenode*)object);
    delete object;    
}

tobject3d *tscene::addobject(tobject3d* parentobject,tobject3d* newobject)
{
    switch (newobject->type)
    {
	case otLIGHT:
		tlightref *lr=new tlightref((tlight*)newobject);
		tlights.push((telemptr)lr);
		break;
    }
    newobject->scene=this;
    TTaddchildtonode(&scenegraph,(ttreenode*)parentobject,(ttreenode*)newobject);
    //force matrix update
    newobject->moveto(newobject->p.x,newobject->p.y,newobject->p.z);
	
	//    newobject->calcworldmat();
    return newobject;
}

INT16 intypos;
void tscene::intensity(vector &n,vector &p,vector &v,material &m,trgbafloatbase &result)
{
    tlightref *lr;
    flt specdot=FLT_0;
    tlight *l;
    flt dp;
    result.r=result.g=result.b=FLT_0;
    vector o,r;    
    lr=(tlightref*)tlights.first;
    T3Dspecdp=FLT_0;    
    result.r=0;
    result.g=0;
    result.b=0;
	
    
    if ((T3Dsceneattributes & sfFOG)!=0)           
	if (p.z>fogmaxz)
	{
	result.r=fogcolour.r;
	result.g=fogcolour.g;
	result.b=fogcolour.b;
	return;
	}        
    
    while (lr!=NULL)
    {
        l=lr->l;
        if ((l->attribute&ofENABLED)!=0)
        {
            switch (l->lighttype)
            {
			case LTdirectional:
				o=l->no;
				
				break;
			case LTpoint:
				o=l->wp-p;                
				o.normalize();
				break;               
            };
            //n.normalize();
            dp=dot(n,o);            
            
            //R'= 2N'(N'.L')-L
            specdot=FLT_0;
            if (dp>FLT_0) //any light here?
            {            
                if ((T3Dsceneattributes & sfSPECULARHIGHLIGHTS)!=0)
                {
					//calculate reflection vector
					r.x=(2.0f*n.x*dp)-o.x;
					r.y=(2.0f*n.y*dp)-o.y;
					r.z=(2.0f*n.z*dp)-o.z;                                        
					specdot=dot(r,v);
					//if vector is in world space, only this is needed
					//specdot=-((2.0f*n.z*dp)-o.z);
					if ((specdot>FLT_0))
					{
						
						if (m.specpower==0)
							specdot*=m.st;
						else                                            
							specdot=ipow(specdot,m.specpower)*m.st;
						
						T3Dspecdp+=specdot;                     
					}else
						specdot=FLT_0;
                }                           
                result.r+=l->ambient.r+(l->ambient.r*m.ambient.r)+(l->diffuse.r*m.diffuse.r*(float)dp)+((l->specular.r*m.specular.r)*(float)specdot);
                result.g+=l->ambient.g+(l->ambient.g*m.ambient.g)+(l->diffuse.g*m.diffuse.g*(float)dp)+((l->specular.g*m.specular.g)*(float)specdot);
                result.b+=l->ambient.b+(l->ambient.b*m.ambient.b)+(l->diffuse.b*m.diffuse.b*(float)dp)+((l->specular.b*m.specular.b)*(float)specdot);
            }else
            {
                //if no light, just add ambient values
                result.r+=l->ambient.r+(l->ambient.r*m.ambient.r);
                result.g+=l->ambient.g+(l->ambient.g*m.ambient.g);
                result.b+=l->ambient.b+(l->ambient.b*m.ambient.b);
            }
        }
        lr=(tlightref*)lr->next;
    }
    //put fogging in here
    if ((T3Dsceneattributes & sfFOG)!=0)
    {        
        if (p.z>fogminz)
        {
            float temp=(float)(p.z-fogminz)/fogdepth;
            BOUND(temp,FLT_0,FLT_1);
            float invtemp=(float)FLT_1-temp;
            result.r=(float)(result.r*invtemp)+(fogcolour.r*temp);
            result.g=(float)(result.g*invtemp)+(fogcolour.g*temp);
            result.b=(float)(result.b*invtemp)+(fogcolour.b*temp);
            temp*=fogdensity;
            invtemp=(float)FLT_1-temp;
            result.r+=m.emmisive.r*invtemp;
            result.g+=m.emmisive.g*invtemp;
            result.b+=m.emmisive.b*invtemp;            
        }
        else
        {
            result.r+=m.emmisive.r;
            result.g+=m.emmisive.g;
            result.b+=m.emmisive.b;
        }
    }
    BOUND(result.r,0.07f,0.99f);
    BOUND(result.g,0.07f,0.99f);
    BOUND(result.b,0.07f,0.99f);   
	
}

void tscene::renderwire()
{
	
	
	
    static UINT32 i;
    
    static int colours[]={T3DCOLwire,T3DCOLselectedwireedge,
		T3DCOLselectedwire,T3DCOLselectedwireedge,
		T3DCOLextrusionbody,T3DCOLselectedwireedge,
		T3DCOLselectedwire,T3DCOLselectedwireedge,                           
	};
    static tedge **ce;    
    static tpointflt p,np;
    static tedge *e;    
	ce=T3Dvisibleedges;
    
    for (i=0;i<T3Dedgecount;i++) 
    {
        e=*ce;
        
        t_col=colours[e->attribute & 3];
        p=e->v[0]->point2d;
        np=e->v[1]->point2d;
		
        /*if (e->attribute & eaSELECTED)
        {
		drawarrow(p.x,p.y,np.x,np.y,0.5,0.4,5);
	}else*/
		line(float2int(p.x),float2int(p.y),float2int(np.x),float2int(np.y)); 
        
        ce++;
    }
    
}


void tri2(trivert *v0,trivert *v1,trivert *v2)     
{	
	
	   
	glNormal3f(v0->nx,v0->ny,v0->nz);
	   glColor3f(v0->r,v0->g,v0->b);
	   glVertex3f(v0->x,v0->y,-v0->z);	   
	   
	   glNormal3f(v1->nx,v1->ny,v1->nz);
	   glColor3f(v1->r,v1->g,v1->b);
	   glVertex3f(v1->x,v1->y,-v1->z);	    
	   
	   glNormal3f(v2->nx,v2->ny,v2->nz);
	   glColor3f(v2->r,v2->g,v2->b);
	   glVertex3f(v2->x,v2->y,-v2->z);	 	   
}


void tscene::render()
{
    static UINT32 i,iv;
    static UINT32 at;
    static INT32 addby;
    static UINT32 nsides;
    static vector normal;
    static t3dmesh *obj;
    static tpolyvertex *vr;
    static tpoint p,np;
    static vertex *vp;
    static tpolygon **cpolyref;//(polygon*)obj->polygonlist.first;
    static tpolygon *cpoly;    
    static float tr;	
    static trivert points[256],*cpoint;  
    cpolyref=T3Dvisiblepolygons;
	//glBegin(GL_TRIANGLES);
	obj=(t3dmesh *)scenegraph.root;
    for (i=0;i<T3Dpolycount;i++)
    {        
        cpoly=*cpolyref;        
        at=cpoly->attribute & polyattribmask;        
        nsides=cpoly->npoints;                                
        cpoint=points;
        cmds.settexture(cpoly->tex);                //
        //transparency failsafe.
        if (at & paBACKFACING)
        {
			vr=vr;
			vr=&cpoly->vlist[nsides-1];
			addby=-1;
        }else
        {
			vr=&cpoly->vlist[0];
			addby=1;
        }        
		
        if (!t_texture)
			at &=~paTEXTURED;
		else
			at |= paTEXTURED;
		
        //set up point details
        if (at & paTEXTURETRUE)
        {
            t_colrgba.r=t_colrgba.g=t_colrgba.b=128;
            at&=~paSHADED;
            if (at & paTRANSPARENT)                            
                t_colrgba.a=float2int(tr);                        
				}/*else
				 if (at & paTRUECOLOUR)
				 {
				 trgbfloat d=mat->diffuse;
				 t_colrgba.r=float2int(d.r*255);t_colrgba.g=float2int(d.g*255);t_colrgba.b=float2int(d.b*255);
				 t_col=getcolour2(t_colrgba.r,t_colrgba.g,t_colrgba.b);
				 at&=~paSHADED;
				 if (at & paTRANSPARENT)                            
				 t_colrgba.a=float2int(tr);            
	}  */    
        //at&=~paTRANSPARENT;
        for (iv=0;iv<nsides;iv++)
        {
            vp=vr->v;                        
            cpoint->x=(float)vp->point2d.x;
            cpoint->y=(float)vp->point2d.y;         
			//cpoint->x=(float)vp->temp3d.x;
            //cpoint->y=(float)vp->temp3d.y;         
			//cpoint->z=(float)vp->temp3d.z;
			cpoint->nx=(float)vp->normal.x;
            cpoint->ny=(float)vp->normal.y;         
			cpoint->nz=(float)vp->normal.z;                                  
            cpoint->r=vr->outcolour.r;
			cpoint->g=vr->outcolour.g;
			cpoint->b=vr->outcolour.b;
			cpoint->a=vr->outcolour.a;
            if (at & paTEXTURED)
            {
                if (at & paENVMAP)
                {
                    vector vec=vp->normal;
                    vector vec2=T3Dlookv;
                    vec.x=-vec.x;
                    vec2.y=-vec2.y;
                    reflection(vec2,vec,normal);
					//tempnormal=vec;
                    cpoint->u = (float)MAX(0.01f,((0.5f + normal.x*0.5f))*(t_texturewidthscale));
                    cpoint->v = (float)MAX(0.01f,((0.5f + normal.y*0.5f))*(t_textureheightscale));
                }else
                {
                    /*BOUND(vr->texv.x,0.01f,FLT_1);
                    BOUND(vr->texv.y,0.01f,FLT_1);*/
                    cpoint->u=vr->texv.x*(t_texturewidthscale);
                    cpoint->v=vr->texv.y*(t_textureheightscale);
                }
            }
            //if zbuffer...            
            cpoint++;
            vr+=addby;            
            //vr++;
        }
        at |= paSHADED;
        //set render mode
        cmds.setscanmode(at & 15);
        //and render...
        UINT32 id=0;
		
		
        if (nsides>3)
        {
            for (;id<(nsides >> 1)-1;id++)
            {
				
                cmds.tri(&points[id],&points[id+1],&points[nsides-id-1]);
                cmds.tri(&points[id+1],&points[nsides-id-2],&points[nsides-id-1]);                            
            }                                                               
        }                                                               
        if ((nsides & 1)==1) 
		{
			cmds.tri(&points[id],&points[id+1],&points[id+2]);
		}
		
		//convexpolygon(points,nsides);                        
        //Draw surface normals*/
#ifdef SHOW_NORMALS
        line(cpoly->c2d.x,cpoly->c2d.y,
			cpoly->n2d.x,cpoly->n2d.y);
#endif
        cpolyref++;
    } 
	//glEnd();
}

////////////////////////////////////////////////////////////////////////////
//Octtree



octtree::octtree()
{
	root=NULL;
	totalpolys=0;
	type=otOCTTREE;
	sprintf(name,"Oct tree");

}

octtreenode *octtree::buildnode(t3dmesh *obj,vector *v1,vector *v2,int depth,tpolygon **polylist)
{
	static tplane planes[6];
	octtreenode *root=NULL;
	T3Dbuildplanecube(planes,v1,v2);
	tpolygon *p=obj->getpolygonhead();
	int count=0;	
	

	while (p!=NULL)
	{
		if (p->isinside(planes,6))
		{
			count++;
			//if (depth==OCTTREE_DEPTH)
			//	polylist[count]=p;			
		}	
		p=(tpolygon*)p->next;
	}
	//count=1;
	if (count)
	{
		root=new octtreenode();
		root->pcount=count;
		root->plist=polylist;
		root->depth=depth;
		totalpolys+=count;
		root->v1=*v1;
		root->v2=*v2;
		vector h=(*v2-*v1) / 2;
		vector nv1,nv2;
		if ((count>OCTTREE_POLYDEPTH)&&(depth!=OCTTREE_DEPTH))
		{
			nv1.z=v1->z;			
			nv2.z=v1->z+h.z;

			nv1.y=v1->y;
			nv2.y=v1->y+h.y;
			nv1.x=v1->x;
			nv2.x=v1->x+h.x;
			root->nodes[0]=buildnode(obj,&nv1,&nv2,depth+1,&polylist[totalpolys]);		
			nv1.x+=h.x;
			nv2.x=v2->x;
			root->nodes[1]=buildnode(obj,&nv1,&nv2,depth+1,&polylist[totalpolys]);		

			nv1.y+=h.y;nv2.y=v2->y;
			nv1.x=v1->x;
			nv2.x=v1->x+h.x;
			root->nodes[2]=buildnode(obj,&nv1,&nv2,depth+1,&polylist[totalpolys]);		
			nv1.x+=h.x;
			nv2.x=v2->x;
			root->nodes[3]=buildnode(obj,&nv1,&nv2,depth+1,&polylist[totalpolys]);		

			nv1.z+=h.z;			
			nv2.z=v2->z;

			nv1.y=v1->y;
			nv2.y=v1->y+h.y;
			nv1.x=v1->x;
			nv2.x=v1->x+h.x;
			root->nodes[4]=buildnode(obj,&nv1,&nv2,depth+1,&polylist[totalpolys]);		
			nv1.x+=h.x;
			nv2.x=v2->x;
			root->nodes[5]=buildnode(obj,&nv1,&nv2,depth+1,&polylist[totalpolys]);		

			nv1.y+=h.y;nv2.y=v2->y;
			nv1.x=v1->x;
			nv2.x=v1->x+h.x;
			root->nodes[6]=buildnode(obj,&nv1,&nv2,depth+1,&polylist[totalpolys]);		
			nv1.x+=h.x;
			nv2.x=v2->x;
			root->nodes[7]=buildnode(obj,&nv1,&nv2,depth+1,&polylist[totalpolys]);		
			
		}
		return root;
	}else
		return NULL;
}

void octtree::build(t3dmesh *obj)
{
	min=obj->min;
	max=obj->max;
	root=buildnode(obj,&min,&max,0,plist);
}

octtreenode::octtreenode()
{
	for (int i=0;i<8;i++)	
		nodes[i]=NULL;			
	
	plist=NULL;
	pcount=0;
}

void rendercube(vector &v1,vector &v2,matrix4x4 *mat)
{
	static vector p[8]; //points
	static vector tp[8]; //transformed points

	p[0]=v1;
	p[1].moveto(v2.x,v1.y,v1.z);
	p[2].moveto(v2.x,v2.y,v1.z);
	p[3].moveto(v1.x,v2.y,v1.z);

	p[4].moveto(v1.x,v1.y,v2.z);
	p[5].moveto(v2.x,v1.y,v2.z);
	p[6]=v2;
	p[7].moveto(v1.x,v2.y,v2.z);

	for (int i=0;i<8;i++)
	{		
		p[i].multmatrixtrans(mat,&tp[i]);		
		float temp=FLT_1/tp[i].z*FOV;
		tp[i].x=(tp[i].x*temp)+T3Doffset.x;
		tp[i].y=(tp[i].y*temp)+T3Doffset.y;
	}

	/*line(tp[0].x,tp[0].y,tp[1].x,tp[1].y);
	line(tp[1].x,tp[1].y,tp[2].x,tp[2].y);
	line(tp[2].x,tp[2].y,tp[3].x,tp[3].y);
	line(tp[3].x,tp[3].y,tp[0].x,tp[0].y);

	line(tp[4].x,tp[4].y,tp[5].x,tp[5].y);
	line(tp[5].x,tp[5].y,tp[6].x,tp[6].y);
	line(tp[6].x,tp[6].y,tp[7].x,tp[7].y);
	line(tp[7].x,tp[7].y,tp[4].x,tp[4].y);
	
	line(tp[0].x,tp[0].y,tp[4].x,tp[4].y);
	line(tp[1].x,tp[1].y,tp[5].x,tp[5].y);
	line(tp[2].x,tp[2].y,tp[6].x,tp[6].y);
	line(tp[3].x,tp[3].y,tp[7].x,tp[7].y);	*/
}

void renderoctnode(octtreenode *root)
{
	if (!root) return;
	bool haschildren=false;
	int i;
	for (i=0;i<8;i++)
		if (root->nodes[i])
			haschildren=true;
	if (!haschildren)
		rendercube(root->v1,root->v2,&T3Dviewmat);	

	for (i=0;i<8;i++)
		renderoctnode(root->nodes[i]);
	
}


void octtree::renderhandles(UINT32 attrib,tcamera *cam)
{	

	octtreenode *cnode=root;
	renderoctnode(root);
	
}


