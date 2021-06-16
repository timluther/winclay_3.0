#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "graphics.h"
#include "gprim.h"
#include "trender.h"
#include "basic3d.h"
#include "..\scenegraph\3dfile.h"
#include "msmouse.h"
#include "views.h"
#include "widgets.h"
#include "dialogs.h"
#include "perlin.h"
#include "logger.h"
#include "editscn.h"


//////////////////////////////////////////////////////////////////////////////////////////
//Global Variables

bool T3Dcheckergrid;
UINT32 T3Dnumcopies;
UINT32 T3Dcopyattributes;
spheredata spherevals;
selipsedata selipsevals;
conedata conevals;
cylinderdata cylindervals;
griddata gridvals;
circledata circlevals;
textdata textvals;

/////////////////////////////////////////////////////////////////////////////////////////////
//Module Global Variables

//would be static only it relies on external function pointers; oo methods are slightly broken I suppose.
//globals are alright for this because with modal variables, we cannot have two
//edit3d views doing the same thing at the same time.
//all global variables pertaining to modal sections for the edit3d view are prefixed with a 'ed_'


UINT32 ed_waitingforrelease;
vector ed_p;
tpolygon *ed_polygon;
vertex *ed_polygonvertex;
INT16 ed_xdiff,ed_ydiff;   
tpoint ed_oldmp;
flt ed_cx,ed_cy,ed_newscale;
vector ed_oldp1;
vector ed_oldp2;
vector ed_p1;
UINT8 ed_scandown;
trect ed_tempbounds; //used in order to draw the temporay bounding box as it's being drawn
INT32 ed_boundcode;
tsurface *claytoolbm;
trectflt ed_sobjoldbounds;
teditscene *ed_scene;
t3dmesh *ed_obj;
bool ed_cornerstretch;
tboundingbox3d *ed_bounds;  //current bounds that are in use; set in setselectmode
vector ed_boundoldmin,ed_boundoldmax,ed_boundmin,ed_boundmax;
tpoint ed_addmin;
tpoint ed_addmax;
flt ed_scalefactor;

flt T3Dboundingboxrotation;
tpointflt T3D2drotationpivot;

void (*ed_modalfunc)(edit3d *ed,tevent *event)=NULL;
void (*T3Dextrusionmodestatchange)(UINT32 i)=NULL;

listelembase T3Dselectionfunctions[]=
{{"Stretch",0,STstretch,0,0},{"Rotate",0,STrotate,0,0},{"Centre Scale",0,STscale,0,0},{"Axis rotate",0,STaxisrotate,0,0},{"Shear",0,STshear,0,0},
{"Taper",0,STtaper,0,0},{"Twist",0,STtwist,0,0},{"Axis Translate",0,STaxistranslate,0,0},{"Axis scale",0,STaxisscale,0}};

UINT32 T3Dselectmasks[4]={naVISIBLE,0,paALLVERTICESVISIBLE,0};
elementfilter T3Dredrawmasks[4]={
                             {naSELECTED+naENABLED+naVISIBLE,eaAFFECTEDBYSELECTION+eaENABLED,paHASSELECTEDVERTICES+paENABLED,ofAFFECTEDBYSELECTION+ofENABLED},  //selected vertex list
                             {naEXTRUSION+naENABLED+naVISIBLE,eaEXTRUSION+eaENABLED,paEXTRUSION+paENABLED,ofAFFECTEDBYSELECTION+ofENABLED}, //selected polygon list {naAFFECTEDBYSELECTION+naENABLED+naVISIBLE,paAFFECTEDBYSELECTION+paENABLED,ofAFFECTEDBYSELECTION+ofENABLED}
                             {naENABLED+naVISIBLE,eaENABLED,paCONTAINSCURRENTVERTEX+paENABLED,ofENABLED},       //current vertex
                             {naENABLED+naVISIBLE,eaENABLED,paENABLED,ofENABLED} //current polygon            
                             };
listelembase T3Dselectionlevels[4]=
{{"Vertex",0,ETvertex,0,0},{"Edge",0,ETedge,0,0},{"Polygon",0,ETpolygon,0,0},{"Object",0,ETobject,0,0}};
listelembase T3Dselectioneffects[5]=
{{"New",0,bPUT,0,0},{"Add",0,bOR,0,0},{"Sub",0,bNOT,0,0},{"Xor",0,bXOR,0,0},{"And",0,bAND,0,0}};
listelembase T3Dselectiongeomerty[4]=
{{"Rectangle",0,0,0,0},{"Circle",0,0,0,0},{"Lasso",0,0,0,0},{"Polygon",0,0,0,0}};



//////////////////////////////////////////////////////////////////////////////
// XY projection routines.. will be condensed into single routine soon

matrix4x4ptr LMprojectxy1(matrix4x4ptr mat,vector *v,vector *c)
{


    flt d=((v->y*v->y)+(v->z*v->z));
    if (!d)
    {
        LMidentity(mat);    
        return mat;
    }
    d=(flt)sqrt(d);

    sprintf(buf,"denom %f ",d);
    logmessage((void*)&textcol,buf);
            
    flt sn=v->y /d;
    flt cs=v->z /d;
    /*flt angle1=acos(cs);    

    LMxrotation(mat,angle1);*/            
    /*flt x=v->x;
    /*sprintf(buf,"angle %f %f",angle1,angle2);
    logmessage((void*)&textcol,buf);
    sprintf(buf,"cs %f sn %f x %f y %f z %f",sn,cs,v->x,v->y,v->z);
    logmessage((void*)&textcol,buf);*/
    mat->p=0;
    mat->m00=1;mat->m01=    mat->m02=0;
    mat->m10=0;mat->m11=cs; mat->m12=-sn;
    mat->m20=0;mat->m21=sn;mat->m22=cs;    

    return mat;    
}

matrix4x4ptr LMprojectxy2(matrix4x4ptr mat,vector *v,vector *c)
{


    //if no projection in the x/z plane, no need to rotate
    flt denom=(v->x*v->x+v->z*v->z);
    if (!denom)
    {
        LMidentity(mat);    
        return mat;
    }
    flt d=((v->y*v->y)+(v->z*v->z));
    d=(flt)sqrt(d);
    flt x=v->x;
    sprintf(buf,"denom %f ",d);
    logmessage((void*)&textcol,buf);
    
    
    mat->p=0;
    mat->m00=d;  mat->m01=0;  mat->m02=-x;
    mat->m10=0;  mat->m11=1;  mat->m12=0;
    mat->m20=x; mat->m21=0;  mat->m22=d;    

    return mat;    
}

matrix4x4ptr LMprojectxy_1(matrix4x4ptr mat,vector *v,vector *c)
{
    matrix4x4 tmpmat,tmpmat2;
    LMtranslate(&tmpmat,-c->x,-c->y,-c->z);
    LMprojectxy1(mat,v,c);
    //LMidentity(mat);
    LMconcatenatematrices(mat,&tmpmat,&tmpmat2);
    LMprojectxy2(&tmpmat,v,c);
    LMconcatenatematrices(&tmpmat,&tmpmat2,mat);
    return mat;
}   


matrix4x4ptr LMinvprojectxy1(matrix4x4ptr mat,vector *v,vector *c)
{


    flt d=((v->y*v->y)+(v->z*v->z));
    if (!d)
    {
        LMidentity(mat);    
        return mat;
    }
    d=(flt)sqrt(d);
    
    //flt sn=v->y /sqrtd;
    flt cs=v->z /d;    

    flt theta=(flt)acos(cs);
    //flt theta=asin(sn);
    if (v->y > 0)
    {
      theta = 2*PI-theta;
    }

    LMxrotation(mat,-theta);
    

    
    /*flt denom=(v->x * v->x) +( v->y * v->y);
    

    /*sprintf(buf,"inv project xy 1 denom %f %f",denom,d);
    logmessage((void*)&textcol,buf);    
    
    if ((d==0.000) || (denom==0.000))
    {    
        LMidentity(mat);    
        return mat;
    }

    d=sqrt(d);
    
    flt cs= (v->z *denom)/d;
    flt sn= (v->y *denom)/d;        

    

    mat->p=0;
    mat->m00=1;mat->m01=    mat->m02=0;
    mat->m10=0;mat->m11=cs; mat->m12=sn;
    mat->m20=0;mat->m21=-sn;mat->m22=cs;    */
    
    return mat;
}

matrix4x4ptr LMinvprojectxy2(matrix4x4ptr mat,vector *v,vector *c)
{
    
    
    //if no projection in the x/z plane, no need to rotate
    flt denom=(v->x*v->x+v->z*v->z);
    if (!denom)
    {
        LMidentity(mat);    
        return mat;
    }
    flt d=((v->y*v->y)+(v->z*v->z));
    d=(flt)sqrt(d);
    flt x=v->x;
    sprintf(buf,"denom %f ",d);
    logmessage((void*)&textcol,buf);   
    
    mat->p=0;
    mat->m00=d;  mat->m01=0;  mat->m02=x;
    mat->m10=0;  mat->m11=1;  mat->m12=0;
    mat->m20=-x; mat->m21=0;  mat->m22=d;    
    return mat;
}


matrix4x4ptr LMinvprojectxy_1(matrix4x4ptr mat,vector *v,vector *c)
{
    matrix4x4 tmpmat,tmpmat2;
    LMinvprojectxy2(&tmpmat,v,c);
    LMinvprojectxy1(mat,v,c);
    //LMidentity(mat);
    LMconcatenatematrices(mat,&tmpmat,&tmpmat2);    
    LMtranslate(&tmpmat,c->x,c->y,c->z);
    LMconcatenatematrices(&tmpmat,&tmpmat2,mat);
    return mat;
}


matrix4x4ptr LMnormalscale(matrix4x4ptr mat,vector *v,vector *c,flt sx,flt sy,flt sz)
{
    matrix4x4 tmpmat,tmpmat2;            
    LMprojectxy_1(&tmpmat,v,c);
    LMscale(mat,sx,sy,sz);
    LMconcatenatematrices(mat,&tmpmat,&tmpmat2);
    *mat=tmpmat2;
    LMinvprojectxy_1(&tmpmat,v,c);
    LMconcatenatematrices(&tmpmat,&tmpmat2,mat);
    return mat;
}

//////////////////////////////////////////////////////////////////////////////////////////
//Global functions


void T3Drandheightgrid(griddata *gdata,vertex *first)
{
    //flt **heightarray=new flt*[gdata->xsegs*gdata->ysegs];
    //flt **cheight=heightarray;
    for (UINT32 y=0;y<gdata->ysegs;y++)
      for (UINT32 x=0;x<gdata->xsegs;x++)
      {   
		//first->data3d.y=PI*x*y;
        first->data3d.y=tperlinnoise_2d((flt)x,(flt)y,0.5f,0.5f,0.1f);
        first=(vertex*)first->next;
      }
    
    
}

void T3Dsetcurrentscene(teditscene *scene)
{
    ed_scene=scene;
}

teditscene *T3Dcurrentscene()
{
    return ed_scene;
}



//swap an edge pointer
void T3Deswap(tedge **e1,tedge **e2)
{
    tedge *t;
    t=*e1;*e1=*e2;*e2=t;
}

//set the background clear colour and fog colour
void T3Dsetbackgroundcolour(UINT32 r,UINT32 g,UINT32 b)
{
    ed_scene->fogcolour.r=r / 255.0f;
    ed_scene->fogcolour.g=g / 255.0f;
    ed_scene->fogcolour.b=b / 255.0f;    
}        



void T3Dgetinvertedobjectmatrix(matrix4x4 *invmat,tobject3d *obj)
{
    //should be single matrix operation, this is just to get it right
    matrix4x4 scaleinvmat;
    float sx=1.0f/obj->s.x;float sy=1.0f/obj->s.y;float sz=1.0f/obj->s.z;
    LMtranslate(invmat,-obj->p.x,-obj->p.y,-obj->p.z);
    LMrottrans(&scaleinvmat,obj->o.x,obj->o.y,obj->o.z,0,0,0);
    LMtranspose(&scaleinvmat);
    LMconcatenatematrices(&scaleinvmat,invmat,invmat);
    LMscale(&scaleinvmat,sx,sy,sz);                      
    LMconcatenatematrices(&scaleinvmat,invmat,invmat);            
}

void T3Dresetelementmasks()
{
    T3Dobjectprocessmask=T3Dobjectprocessmaskresult=ofENABLED;    
    T3Dpolygonprocessmask=T3Dpolygonprocessmaskresult=paENABLED;    
    T3Dvertexprocessmask=T3Dvertexprocessmaskresult=naENABLED+naVISIBLE;
    T3Dobjecttypemask=0xFFFFFFFF;    
}

bool m_chkrect(INT16 x1,INT16 y1,INT16 x2,INT16 y2,tpoint &mp)
{
    return ((mp.x>=x1)&&(mp.x<=x2)&&(mp.y>=y1)&&(mp.y<=y2));
}

bool m_chkrect(float x1,float y1,float x2,float y2,tpointflt &mp)
{
    return ((mp.x>=x1)&&(mp.x<=x2)&&(mp.y>=y1)&&(mp.y<=y2));
}


///////////////////////////////////////////////////////////////////////////////
//Selection callback functions


bool checkelembyattribute_and(tattribelem *v,UINT32 mask)
{
    return (v->attribute & mask)==mask;
}


bool checkelembyattribute_equality(tattribelem *v,UINT32 mask)
{
    return (v->attribute ==mask);
}

bool checkpolygonbytexture(tattribelem *v,UINT32 mask)
{
    return ((tpolygon *)v)->tex==(textureref *)mask;
}

bool checkpolygonbynoncoplainarity(tattribelem *v,UINT32 mask)
{
    return !((tpolygon *)v)->iscoplainar(0.00001f);
}

bool checkpolygonifedge(tattribelem *v,UINT32 mask)
{
    return ((tpolygon *)v)->checkedgeattributes(mask);
}

bool checkpolygonbymaterial(tattribelem *e,UINT32 mask)
{
    bool matfound=TRUE;
    tpolygon *p=(tpolygon *)e;
    tpolyvertex *v,*end;
    for (end=&(v=p->vlist)[p->npoints];v<end;v++)
        if (v->mat != (material *)mask)
           matfound=FALSE;
    return matfound;

}



//////////////////////////////////////////////////////////////////////////
//edit3d modal functions
//The edit3d class is going to be very large. I have split up it's modal operations into a set of functions
//in order to avoid case statements from hell and totally unreadable code
//each one takes a pointer to the calling edit3d class and the event that was recieved

void modal_waitformouse(edit3d *ed,tevent *event)
{
    //outtextxy(0,640,"Waiting for mouse");
    if (mb==0)
    {        
        ed->endmodal2();
    }
}

void modal_cursormove(edit3d *ed,tevent *event)
{
    if (mb==2)
    {
        if ((MOUSEmoved)&&(!ed_scandown))
        {
            ed_oldp1=ed_p1;
            ed->get3dpos(mp.x,mp.y,&ed_p1);
            ed_scene->grid->snapto(&ed_p1);            
            if (!ed_oldp1.closeto(ed_p1))
            {
               ed_scene->cursor->moveto(ed_p1.x,ed_p1.y,ed_p1.z);
               ed->evcallback(CMDscenealteredorth);
            }
        }
    }else
    {
        ed->resetdrawmode(CMsingleobj);
        ed->endmodal2();                                 
    }
}

void modal_drawingselectbox(edit3d *ed,tevent *event)
{ 
   if (MOUSEmoved)
   {
        ed_tempbounds.b.x=mp.x-ed->a.x;
        ed_tempbounds.b.y=mp.y-ed->a.y;
        ed->drawview();                  //draw the view, rectangle will be overlayed                  
   }
   if (mb==0)
   {
        ed->endmodal2();
        ed_tempbounds.sortelements();
        ed_scene->preprocess_select(&ed_tempbounds,&ed->scr->size,ed->viewcam,ed_tempbounds.a.x,ed_tempbounds.a.y);        
        ed_scene->clearextrusion();
        ed_scene->selectelements(ed_scene->selectionmode,ed_scene->selectioneffect,ofVISIBLE+ofENABLED,T3Dselectmasks[ed_scene->selectionmode],checkelembyattribute_and);
        ed_scene->slbox->sortelements();
        ed->evcallback(CMDscenealtered);
        //ed->evcallback(CMDselectionchanged);
   }
}


void modal_polygoncreate(edit3d *ed,tevent *event)
{    
    //search for or create vertex here
    if (!mb && MOUSEmoved)
    {
        ed_waitingforrelease=false;
        ed->get3dpos(mp.x,mp.y,&ed_p1);
        ed_scene->grid->snapto(&ed_p1);
        ed_polygonvertex->data3d=ed_p1;
        ed_obj->calculatenormals();
        ed->evcallback(CMDscenealtered);
    }else if (!ed_waitingforrelease)
    {
        if (mb==1)
        {
           if (ed_polygon->npoints<ed_maxpolyvertices)
           {
              logmessage((void*)&textcol, "add point");             
              ed_polygonvertex->data3d=ed_p1;        
              ed_polygonvertex = ed_obj->addvertex(ed_p1.x,ed_p1.y,ed_p1.z);
              T3Dsetpolygonvertex(ed_polygon,ed_polygonvertex,ed_polygon->npoints++,0);           
              ed_waitingforrelease=true;
           }
        }else if (mb>1)
        {
           ed_polygon=T3Dreallocpoly(ed_polygon,ed_polygon->npoints);
           ed_obj->calculatenormals();
           ed->resetdrawmode(CMsingleobj);
           ed->endmodal2();
           ed->sendcommand(CMDobjectcreated,ed->target,ed_obj);
        }
    }
    
}




void modal_objectcreate(edit3d *ed,tevent *event)
{
  //we're creating an object via scaling, this applies to all shapes that require it; polylines and curves use a different method
  if (mb==1)
  {
     if ((MOUSEmoved)&&(!ed_scandown))
     {
         ed_oldp1=ed_p;
         ed_xdiff=mp.x-ed_oldmp.x;
         ed_ydiff=mp.y-ed_oldmp.y;
         if (abs(ed_xdiff-ed_ydiff)<10) ed_ydiff=ed_xdiff;                                 
         ed->filter3dpos((ed_xdiff)/T3Dscale,(ed_ydiff) / T3Dscale,(ed_xdiff)/T3Dscale,&ed_p);//(mp.x-oldmp.x) /T3Dscale
         ed_scene->grid->snapto(&ed_p);
         if (!ed_oldp1.closeto(ed_p))
         {
             ed_obj->scale(ed_p.x,ed_p.y,ed_p.z);
             if (ed_cornerstretch)
             {
                 vector s=ed_obj->s/2;
                 switch (ed->viewcam->camtype)
                 {
                     case camORTHXY:
                     s.z=0.0f;break;
                     case camORTHXZ:
                     s.y=0.0f;s.z=-s.z;break;
                     case camORTHZY:
                     s.x=0.0f;s.z=-s.z;break;
                 }
                 ed_obj->moveto(ed_p1.x+s.x,ed_p1.y+s.y,ed_p1.z+s.z);
             }
             //ed_obj->calculatenormals();                        
             ed->evcallback(CMDscenealtered);
        }
     }
  }else
  {                    
     if (mb==0)
     {
        if (mp.x==ed_oldmp.x)                        
           ed_obj->scale(1,ed_obj->s.y,1);
        if (mp.y==ed_oldmp.y)                        
           ed_obj->scale(ed_obj->s.x,1,1);
        ed_obj->normalizescale();

        ed_obj->calculatenormals();                        
        ed->resetdrawmode(CMsingleobj);
                
        /*ed_scene->calc3dmode=CMsingleobj;    
        ed->evcallback(CMDscenealteredorth);                        
        ed_scene->singleobj=NULL;*/
        
        ed->endmodal2();
        ed->sendcommand(CMDobjectcreated,ed->target,ed_obj);

     }else if(mb==3)
     {                        
        ed_scene->calc3dmode=CMdrawall;    
        ed_scene->singleobj=NULL;                        
        ed_scene->removeobject(ed_obj);
        ed->evcallback(CMDscenealtered);
        ed_modalfunc=modal_waitformouse;
     }
     ed_obj=NULL;                  
   }                                     
}

void modal_uvobjectcreate(edit3d *ed,tevent *event)
{
  //we're creating an object via scaling, this applies to all shapes that require it; polylines and curves use a different method
  if (mb==1)
  {
     if ((MOUSEmoved)&&(!ed_scandown))
     {
         ed_oldp1=ed_p;
         ed_xdiff=mp.x-ed_oldmp.x;
         ed_ydiff=mp.y-ed_oldmp.y;
         if (abs(ed_xdiff-ed_ydiff)<10) ed_ydiff=ed_xdiff;                                 
         ed->filter3dpos((ed_xdiff)/T3Dscale,(ed_ydiff) / T3Dscale,(ed_xdiff)/T3Dscale,&ed_p);//(mp.x-oldmp.x) /T3Dscale
         ed_scene->grid->snapto(&ed_p);
         if (!ed_oldp1.closeto(ed_p))
         {
             ed_obj->scale(ed_p.x,ed_p.y,ed_p.z);
             if (ed_cornerstretch)
             {
                 vector s=ed_obj->s/2;
                 switch (ed->viewcam->camtype)
                 {
                     case camORTHXY:
                     s.z=0.0f;break;
                     case camORTHXZ:
                     s.y=0.0f;s.z=-s.z;break;
                     case camORTHZY:
                     s.x=0.0f;s.z=-s.z;break;
                 }
                 ed_obj->moveto(ed_p1.x+s.x,ed_p1.y+s.y,ed_p1.z+s.z);
             }
			 
			 ed_scene->applyuvobject((tplaneprojection*)ed_obj);
             //ed_obj->calculatenormals();                        
             ed->evcallback(CMDscenealtered);
        }
     }
  }else
  {                    
     if (mb==0)
     {
        if (mp.x==ed_oldmp.x)                        
           ed_obj->scale(1,ed_obj->s.y,1);
        if (mp.y==ed_oldmp.y)                        
           ed_obj->scale(ed_obj->s.x,1,1);                
        ed->resetdrawmode(CMsingleobj);                    
        ed->endmodal2();
        ed->sendcommand(CMDobjectcreated,ed->target,ed_obj);

     }else if(mb==3)
     {                        
        ed_scene->calc3dmode=CMdrawall;    
        ed_scene->singleobj=NULL;                        
        ed_scene->removeobject(ed_obj);
        ed->evcallback(CMDscenealtered);
        ed_modalfunc=modal_waitformouse;
     }
     ed_obj=NULL;                  
   }                                     
}


//////////////////////////////////////////////////////////////////////////////////////////
//Object editing callback functions


void modal_resizebounds(edit3d *ed,tevent *event)
{    
    vector dtl,dbr;
    vector tl,br;
    vector addminv,addmaxv;
    if (mb!=0)
    {
       
       if ((MOUSEmoved)&&(!ed_scandown))    
       {
          
          if (ed_boundcode & RFleft)
          {
              ed_addmin.x+=mdiff.x;
              if (KEYgetscan() & SMleftaltkey ) ed_addmax.x-=mdiff.x;
          }
          if ((ed_boundcode & RFabove) != 0)
          {
              ed_addmin.y+=mdiff.y;          
              if (KEYgetscan() & SMleftaltkey ) ed_addmax.y-=mdiff.y;
          }
          if ((ed_boundcode & RFright) != 0)
          {
              ed_addmax.x+=mdiff.x;          
              if (KEYgetscan() & SMleftaltkey ) ed_addmin.x-=mdiff.x;
          }
          if ((ed_boundcode & RFbellow) != 0)
          {
              ed_addmax.y+=mdiff.y;          
              if (KEYgetscan() & SMleftaltkey ) ed_addmin.y-=mdiff.y;
          }

          ed_oldp1=ed_boundmin;
          ed_oldp2=ed_boundmax;                 
          
          ed->get3dvec(ed_addmin.x,ed_addmin.y,&addminv);
          ed->get3dvec(ed_addmax.x,ed_addmax.y,&addmaxv);
          float temp=addmaxv.z;addmaxv.z=addminv.z;addminv.z=temp;
          
          matrix4x4 tmat;          
          
          ed_scene->grid->snapto(&addminv);
          ed_scene->grid->snapto(&addmaxv);
          
          ed_boundmin=ed_boundoldmin+addminv;
          //build extrusion matrices
          if (ed_boundcode==RFmiddle)
          {
             ed_boundmax=ed_boundoldmax+addminv;
             if (!ed_scene->extrusioncount)
                 LMtranslate(&tmat,addminv.x,addminv.y,addminv.z);
             else
             {
                 
                 tl=dtl=addminv/(flt)T3Dnumcopies;             
                 for (UINT32 i=0;i<ed_scene->extrusioncount;i++)
                 {                     
                     LMtranslate(&ed_scene->extrusionlist[i].tmat,tl.x,tl.y,tl.z);
                     tl+=dtl;                 
                 }
             }
          }
          else
          {
             ed_boundmax=ed_boundoldmax+addmaxv;             
             if (!ed_scene->extrusioncount)
             {             
                LMstretch(&tmat,&ed_boundoldmin,&ed_boundoldmax,&ed_boundmin,&ed_boundmax);
             }else
             {
                
                dtl=addminv/(flt)T3Dnumcopies;
                dbr=addmaxv/(flt)T3Dnumcopies;
                tl=ed_boundoldmin + dtl;
                br=ed_boundoldmax + dbr;
                for (UINT32 i=0;i<ed_scene->extrusioncount;i++)
                {
                     LMstretch(&ed_scene->extrusionlist[i].tmat,&ed_boundoldmin,&ed_boundoldmax,&tl,&br);                     
                     tl+=dtl;
                     br+=dbr;
                }                                
             }
             
          }
          if (!(ed_oldp1.closeto(ed_boundmin) && ed_oldp2.closeto(ed_boundmax)))
          {
             ed_bounds->setbounds(&ed_boundmin,&ed_boundmax);
             if (!ed_scene->extrusioncount)
             {
                ed_scene->transformselectedvertices(&tmat);
                ed_scene->calculatenormals(ofAFFECTEDBYSELECTION,paAFFECTEDBYSELECTION,naAFFECTEDBYSELECTION);             
             }else
             {
               ed_scene->transformextrusion();
               ed_scene->calculatenormals(ofAFFECTEDBYSELECTION,paEXTRUSION,naEXTRUSION);        
             }
             //ed_scene->calcselvertexbounds();
             
             ed->evcallback(CMDscenealtered);          			 
			 
          }
          
       }
       
    }else
    {        
        ed_bounds->sortelements();            
        ed->endmodal2();
        ed_scene->calcselvertexbounds();
        ed->resetdrawmode(CMselectedonly);
    }
}

void modal_axistranslatebounds(edit3d *ed,tevent *event)
{    
    vector tl,dtl;
    if (mb!=0)
    {
       
       if ((MOUSEmoved)&&(!ed_scandown))    
       {
          ed_addmin.x+=mdiff.x;
          
          
          
          ed_oldp1=ed_boundmin;
          ed_oldp2=ed_boundmax;                           
          
          matrix4x4 tmat;          
          flt temp=ed_scene->grid->snapto(ed_addmin.x/T3Dscale);

          ed_p=ed_scene->selectionnormal * temp;
          
          ed_boundmin=ed_boundoldmin+ed_p;
          //build extrusion matrices
          ed_boundmax=ed_boundoldmax+ed_p;
          if (!ed_scene->extrusioncount)
              LMtranslate(&tmat,ed_p.x,ed_p.y,ed_p.z);
          else
          {
                 
              tl=dtl=ed_p/(flt)T3Dnumcopies;             
              for (UINT32 i=0;i<ed_scene->extrusioncount;i++)
              {                     
                   LMtranslate(&ed_scene->extrusionlist[i].tmat,tl.x,tl.y,tl.z);
                   tl+=dtl;                 
              }            
          }
          if (!(ed_oldp1.closeto(ed_boundmin)))
          {
             ed_bounds->setbounds(&ed_boundmin,&ed_boundmax);
             if (!ed_scene->extrusioncount)
             {
                ed_scene->transformselectedvertices(&tmat);
                ed_scene->calculatenormals(ofAFFECTEDBYSELECTION,paAFFECTEDBYSELECTION,naAFFECTEDBYSELECTION);             
             }else
             {
               ed_scene->transformextrusion();
               ed_scene->calculatenormals(ofAFFECTEDBYSELECTION,paEXTRUSION,naEXTRUSION);        
             }
             //ed_scene->calcselvertexbounds();
             
             ed->evcallback(CMDscenealtered);          
          }
          
       }
       
    }else
    {        
        ed_bounds->sortelements();            
        ed->endmodal2();
        ed_scene->calcselvertexbounds();
        ed->resetdrawmode(CMselectedonly);
    }
}


void modal_axisscalebounds(edit3d *ed,tevent *event)
{    
    flt tl,dtl;
    if (mb!=0)
    {
       
       if ((MOUSEmoved)&&(!ed_scandown))    
       {
          vector v=ed_scene->selectionnormal;
          vector c=ed_scene->selectioncentre;
          ed_scalefactor+=mdiff.x;
          if (ed_scalefactor!=0)
          {
          
          ed_oldp1=ed_boundmin;
          ed_oldp2=ed_boundmax;                           
          
          matrix4x4 tmat;
          flt temp;
          if (ed_scalefactor<0)
          {
             temp=ed_scalefactor;
             BOUND(temp,-200.0,0.0);
             temp=-((200.0f+temp)/-200.0f);
          }else
          {
             temp=1+(ed_scalefactor/200.0f);
          }

             //ed_scene->grid->snapto(ed_addmin.x/100.0);//T3Dscale
          ed_boundmin=ed_boundoldmin+ed_p;
          //build extrusion matrices
          ed_boundmax=ed_boundoldmax+ed_p;
          if (!ed_scene->extrusioncount)
              LMnormalscale(&tmat,&v,&c,temp,temp,1);
          else
          {  
              tl=dtl=temp/T3Dnumcopies;             
              for (UINT32 i=0;i<ed_scene->extrusioncount;i++)
              {                     

                   LMnormalscale(&ed_scene->extrusionlist[i].tmat,&v,&c,tl,tl,1);
                   tl+=dtl;                 
              }            
          }
          //if (!(ed_oldp1.closeto(ed_boundmin)))
          {
             ed_bounds->setbounds(&ed_boundmin,&ed_boundmax);
             if (!ed_scene->extrusioncount)
             {
                ed_scene->transformselectedvertices(&tmat);
                ed_scene->calculatenormals(ofAFFECTEDBYSELECTION,paAFFECTEDBYSELECTION,naAFFECTEDBYSELECTION);             
             }else
             {
               ed_scene->transformextrusion();
               ed_scene->calculatenormals(ofAFFECTEDBYSELECTION,paEXTRUSION,naEXTRUSION);        
             }
             //ed_scene->calcselvertexbounds();
             
             ed->evcallback(CMDscenealtered);          
          }
          }
       }
       
    }else
    {        
        ed_bounds->sortelements();            
        ed->endmodal2();
        ed_scene->calcselvertexbounds();
        ed->resetdrawmode(CMselectedonly);
    }
}


void modal_axisrotatebounds(edit3d *ed,tevent *event)
{
    flt dr,cr;
    vector r;    
    
    if (mb!=NULL)
    {
    if ((MOUSEmoved)&&(!ed_scandown))    
    {
      T3Dboundingboxrotation+=mdiff.x/100.0f;
      ed_bounds->moveto(ed_scene->cursor->p.x,ed_scene->cursor->p.y,ed_scene->cursor->p.z);
      T3Dbbcentreofrotation=ed_scene->cursor->p;
      ed->filter3dpos(0,0,T3Dboundingboxrotation,&r);
      
      ed_bounds->rotate(r.x,r.y,r.z);

      matrix4x4 tmat;
      //vector p=-ed_scene->cursor->p;
      if (!ed_scene->extrusioncount)                   
          LMaxisrotation(&tmat,ed_scene->selectionnormal.x,ed_scene->selectionnormal.y,ed_scene->selectionnormal.z,T3Dboundingboxrotation);
      else
      {
          cr=dr=T3Dboundingboxrotation/T3Dnumcopies;
          textrusion *ext;
          //vector oldv(0,0,0);
          //vector p1;
          for (UINT32 i=0;i<ed_scene->extrusioncount;i++)
          {
                ext=&ed_scene->extrusionlist[i];
                LMaxisrotation(&ext->tmat,ed_scene->selectionnormal.x,ed_scene->selectionnormal.y,ed_scene->selectionnormal.z,cr);
                cr+=dr;
          }                                
      }
      
      if (!ed_scene->extrusioncount)
      {
          ed_scene->transformselectedvertices(&tmat);
          ed_scene->calculatenormals(ofAFFECTEDBYSELECTION,paAFFECTEDBYSELECTION,naAFFECTEDBYSELECTION);        
      }else
      {
          ed_scene->transformextrusion();          
          ed_scene->calculatenormals(ofAFFECTEDBYSELECTION,paEXTRUSION,naEXTRUSION);        
      } 
      //ed_scene->calcselvertexbounds();
      ed->evcallback(CMDscenealtered);
      
    }
    }else
    {
        T3Dboundingboxrotation=0.0f;
        T3Dbbcentreofrotation=vector(0,0,0);
        ed_bounds->moveto(0,0,0);
        ed_bounds->rotate(0,0,0);
                
        
        ed_bounds->sortelements();            
        ed->endmodal2();
        ed_scene->calcselvertexbounds();
        ed->resetdrawmode(CMselectedonly);
    }
}  


void modal_rotatebounds(edit3d *ed,tevent *event)
{
    vector r,dr,cr;
    if (mb!=NULL)
    {
    if ((MOUSEmoved)&&(!ed_scandown))    
    {
      T3Dboundingboxrotation+=(mdiff.x/(TWOPI*20));
      ed_bounds->moveto(ed_scene->cursor->p.x,ed_scene->cursor->p.y,ed_scene->cursor->p.z);
      T3Dbbcentreofrotation=ed_scene->cursor->p;
      ed->filter3dpos(0,0,T3Dboundingboxrotation,&r);
      ed_bounds->rotate(r.x,r.y,r.z);   
      matrix4x4 tmat;
      vector p=-ed_scene->cursor->p;
      if (!ed_scene->extrusioncount)                   
          LMrotatearound(&tmat,r.x,r.y,r.z,&p);
      else
      {
          cr=dr=r/(flt)T3Dnumcopies;
          textrusion *ext;
          //vector oldv(0,0,0);
          //vector p1;
          for (UINT32 i=0;i<ed_scene->extrusioncount;i++)
          {
                ext=&ed_scene->extrusionlist[i];
                LMrotatearound(&ext->tmat,cr.x,cr.y,cr.z,&p);               
                cr+=dr;
          }                                
      }
      
      if (!ed_scene->extrusioncount)
      {
          ed_scene->transformselectedvertices(&tmat);
          ed_scene->calculatenormals(ofAFFECTEDBYSELECTION,paAFFECTEDBYSELECTION,naAFFECTEDBYSELECTION);        
      }else
      {
          ed_scene->transformextrusion();          
          ed_scene->calculatenormals(ofAFFECTEDBYSELECTION,paEXTRUSION,naEXTRUSION);        
      } 
      //ed_scene->calcselvertexbounds();
      ed->evcallback(CMDscenealtered);             
    }
    }else
    {
        T3Dboundingboxrotation=0.0f;
        T3Dbbcentreofrotation=vector(0,0,0);
        ed_bounds->moveto(0,0,0);
        ed_bounds->rotate(0,0,0);
                
        
        ed_bounds->sortelements();            
        ed->endmodal2();
        ed_scene->calcselvertexbounds();
        ed->resetdrawmode(CMselectedonly);
    }
}  
 
//////////////////////////////////////////////////////////////////////////////////////////
//Textrusion

vertex *textrusion::addvref(vertex* v)
{
    ed_scene->vext_count++;
    sv_count++;
    ed_scene->svext_list.grabandsetpointer(v);
    return v;
}

tedge *textrusion::adderef(tedge *e)
{
    ed_scene->eext_count++;
    se_count++;
    ed_scene->seext_list.grabandsetpointer(e);
    return e;
    
}

tpolygon *textrusion::addpref(tpolygon *p)
{
    ed_scene->pext_count++;
    sp_count++;
    ed_scene->spext_list.grabandsetpointer(p);
    return p;
}


tedge *textrusion::addecnxref(tedge *e)
{
    ed_scene->eext_count++;
    cnxe_count++;
    ed_scene->seext_list.grabandsetpointer(e);
    return e;
}

tpolygon *textrusion::addpcnxref(tpolygon* p)
{
    ed_scene->pext_count++;
    cnxp_count++;
    ed_scene->spext_list.grabandsetpointer(p);
    return p;
}


void textrusion::copyselection(teditscene *scn)
{
    memcpy(vlist,scn->sv_list.base,scn->sv_count);
    memcpy(elist,scn->se_list.base,scn->sv_count);
    memcpy(plist,scn->sp_list.base,scn->sv_count);  
}


////////////////////////////////////////////////////////////////////////
//Teditscene

teditscene::teditscene():tscene()
{
	selectionmode=ETvertex;
    selectionfunction=STstretch;
	meshdrawmode=MDMcube;
	selectioneffect=bPUT;
    calc3dmode=CMdrawall;
    redrawlevel=RDLselection; //this is only used if we are drawing (or not drawing) the selected elements only
    T3Ddefaultobjectattributes &=~ofEDITABLE;
    grid=new tgrid3d(1.5,1.5,1.5);
    strcpy(grid->name,"THE GRID");
    cursor=new tcursor3d();
    extrusionlength=10;    
    extrusioncount=0;
    lastextrusion=0;
    sv_list.create(20000<<2);        
    sp_list.create(20000<<2);
    se_list.create(20000<<2);
    so_list.create(20000<<2);

    svext_list.create(10000<<2);
    seext_list.create(10000<<2);
    spext_list.create(10000<<2);

    extrusioncount=lastextrusion=
    sv_count=se_count=sp_count=so_count=
    ev_count=
    av_count=ae_count=ap_count=
    ce_count=
    el_count=
    vext_count=eext_count=pext_count=0;
       

    ed_bounds=slbox=new tboundingbox3d(&sv_count);
    setselectmode(ETvertex);        
    
    addobject(NULL,grid);
    addobject(NULL,cursor);
    addobject(NULL,slbox);

    T3Ddefaultobjectattributes |=ofEDITABLE;
    curr.clear();
    underreticle.clear();
    singleobj=NULL;
    cmat=NULL;
    ctex=NULL;
    drawmode=DMselect;
    //create editing polygon
    /*for (int i=0;i<ed_maxpolyvertices;i++)
    {
        ed_polygon.vlist[i]=&ed_polygonvertices[i];
    }*/
}

teditscene::~teditscene()
{
  sv_list.destroy();
}

void teditscene::or_elementattributes(tattribelem**v,UINT32 mask,UINT32 count)
{
    tattribelem**cv,**end;
    for (end=&(cv=v)[count];cv<end;cv++)
        (*cv)->attribute |= mask;
}

void teditscene::and_elementattributes(tattribelem**v,UINT32 mask,UINT32 count)
{
    tattribelem**cv,**end;
    for (end=&(cv=v)[count];cv<end;cv++)
        (*cv)->attribute &= mask;
}

void teditscene::xor_elementattributes(tattribelem**v,UINT32 mask,UINT32 count)
{
    tattribelem**cv,**end;
    for (end=&(cv=v)[count];cv<end;cv++)
        (*cv)->attribute ^= mask;
}

void teditscene::startextrusion()
{
    redrawlevel=RDLextrusion;
    if (T3Dextrusionmodestatchange)
      T3Dextrusionmodestatchange(1);
    
    //view->bextruding->setdown();
    UINT32 attribute;
    
    if (!(T3Dcopyattributes & STRNSkeepoldpolys))
    {
        or_elementattributes((tattribelem**)sp_list.base,paDELETEME,sp_count);
        logmessage((void*)&textcol, "or'ed attributes");
        attribute=EXTRDjustedges ;                                                                           
    }else
        attribute=0;
    if (T3Dcopyattributes & STRNScopy)                     
        copyselpolygons(T3Dnumcopies,0);    
    else if (T3Dcopyattributes & STRNSextrude)                     
        extrudeselpolygons(T3Dnumcopies,attribute);
                     
    applyextrusionattributes();
    //This removes the base of the extrusion, the previous selection
        //should only remove 'standalone' vertices
    if (!(T3Dcopyattributes & STRNSkeepoldpolys))
    {
        
		removepolygons(ofHASSELECTEDPOLYGONS,paDELETEME);
        removestrandedelements(ofHASSELECTEDPOLYGONS);
		
    }
    //calculatenormals(ofHASSELECTEDVERTICES,paAFFECTEDBYSELECTION,naAFFECTEDBYSELECTION);     
    selectelements(ETvertex,bPUT,ofAFFECTEDBYSELECTION,naEXTRUSIONCAP,checkelembyattribute_and);
    calculatenormals(ofHASSELECTEDVERTICES,paAFFECTEDBYSELECTION,naAFFECTEDBYSELECTION);
    calcselvertexbounds();
}

void teditscene::clearextrusion()
{
    if (T3Dextrusionmodestatchange)
      T3Dextrusionmodestatchange(0);    
    
    if (extrusioncount)
    {
        //view->bextruding->setup();
        redrawlevel=RDLselection;        
        //since the whole lists are in one continuous block, just apply these all together   
        and_elementattributes((tattribelem**)extrusionlist[0].vlist,~(naEXTRUSION+naEXTRUSIONCAP),vext_count);
        and_elementattributes((tattribelem**)extrusionlist[0].elist,~eaEXTRUSION,eext_count);
        and_elementattributes((tattribelem**)extrusionlist[0].plist,~(paEXTRUSION+paEXTRUSIONCAP),pext_count);
        //reset counters and heaps
        vext_count=eext_count=pext_count=0;
        svext_list.reset();
        seext_list.reset();
        spext_list.reset();
        extrusioncount=0;
        lastextrusion=0;
    }
}

void teditscene::applynumericextrusionmatrix()
{
    vector dp=(selectionnormal*extrusionlength);
    dp/=(flt)extrusioncount;
    vector p=dp;
   
    for (UINT32 i=0;i<extrusioncount;i++)
    {                     
         LMtranslate(&extrusionlist[i].tmat,p.x,p.y,p.z);
         p+=dp;                 
    }
    //calcselvertexbounds();
    /*dp=(selectionnormal*extrusionlength);
    slbox->moveto(p.x,p.y,p.z);*/
    
}

void teditscene::applyextrusionattributes()
{
    if (extrusioncount)
    {
    or_elementattributes((tattribelem**)extrusionlist[extrusioncount-1].plist,paEXTRUSIONCAP,sp_count);
    or_elementattributes((tattribelem**)extrusionlist[extrusioncount-1].vlist,naEXTRUSIONCAP,sv_count);

    /*for (UINT32 i=0;i<extrusioncount-1;i++)
    {
        
        if ((T3Dcopyattributes & STRNSkeepoldpolys))
          or_elementattributes((tattribelem**)extrusionlist[i].plist,paENABLED,sp_count);
        else
          and_elementattributes((tattribelem**)extrusionlist[i].plist,~paENABLED,sp_count);          
    }*/
    }
}

void teditscene::clearcurrentelements ()
{
    ed_scene->curr.clear();
}


bool teditscene::deleteselected(UINT32 selectmode)
{
   switch (selectmode)
   {
      case ETvertex:
         if (sv_count!=0)
         {
             removepolygons(ofHASSELECTEDVERTICES,paHASSELECTEDVERTICES);
             removeverticies(ofHASSELECTEDVERTICES,naSELECTED);                     
			 removestrandedelements(ofHASSELECTEDVERTICES);
             sv_count=0;
             sv_list.reset();
             return TRUE;
         }                                                                    
         break;
      case ETpolygon:
         if (sp_count!=0)
         {
             removepolygons(ofHASSELECTEDPOLYGONS,paSELECTED);
             //should only remove 'standalone' vertices
             removestrandedelements(ofHASSELECTEDPOLYGONS);
             sp_count=0;
             sp_list.reset();
             return TRUE;
         }
         break;
      case ETobject:                   
             removeobjects(0xFFFFFFFF,ofSELECTED+ofEDITABLE);                          
             return TRUE;
         //break;      
   }
   clearextrusion();
   clearselection();
   clearcurrentelements();
   return FALSE;
}

void teditscene::flipselectedpolygons()
{
    tpolygon **cp,**end;        
    for (end=&(cp=(tpolygon **)sp_list.base)[sp_count];cp<end;cp++)
    {
      (*cp)->reverse();
    }
    calculatenormals(ofAFFECTEDBYSELECTION,paAFFECTEDBYSELECTION,naAFFECTEDBYSELECTION);             
}



//this version checks to see if any edges reference this; if none do, it'll get deleted
//regardless of it's attribute

UINT32 teditscene::removestrandedelements(UINT32 omask)
{    
    t3dmesh *obj=(t3dmesh*)scenegraph.root;    
    int count=0;   
    
    //should recurse
    while (obj!=NULL)
    {
        if ((obj->type==otGEOM) && ((obj->attribute & omask)==omask))
        {
			count+=obj->removestrandedelements();
        }  
        obj=(t3dmesh*)obj->next;
    }
	clearcurrentelements();
    return count;
}

UINT32 teditscene::removeedges(UINT32 omask,UINT32 emask)
{    
    t3dmesh *obj=(t3dmesh*)scenegraph.root;    
    int count=0;    
    //should recurse
    while (obj!=NULL)
    {
        if ((obj->type==otGEOM)&&((obj->attribute & omask)==omask))        
			count+=obj->removeedges(emask);            
        obj=(t3dmesh*)obj->next;
    }
    return count;
}

UINT32 teditscene::removeverticies(UINT32 omask,UINT32 vmask)
{    
    t3dmesh *obj=(t3dmesh*)scenegraph.root;    
    int count=0;
    //should recurse
    while (obj!=NULL)
    {
        if ((obj->type==otGEOM)&&((obj->attribute & omask)==omask))        
			count+=obj->removevertices(vmask);
            
        obj=(t3dmesh*)obj->next;
    }
    return count;
}


UINT32 teditscene::removepolygons(UINT32 omask,UINT32 pmask)
{   
    t3dmesh *obj=(t3dmesh*)scenegraph.root;
	int count=0;
    //should recurse
    while (obj!=NULL)
    {
        if ((obj->type==otGEOM)&&((obj->attribute & omask)==omask))        
			count+=obj->removepolygons(pmask);
            
        obj=(t3dmesh*)obj->next;
    }
    return count;
}

UINT32 teditscene::removeobjects(UINT32 otmask,UINT32 omask)
{
    tobject3d *obj=(tobject3d*)scenegraph.root;
    tobject3d *next;
    UINT32 rcount=0;
    //should recurse

    while (obj!=NULL)
    {
        next=(tobject3d*)obj->next;
        logmessage((void *)&textcol,obj->name);
        
        if ((obj->type & otmask)&&((obj->attribute & omask)==omask))
        {
            //add obj to restore list here
            if (curr.object==obj)
               clearcurrentelements();
            if (singleobj==obj)
               singleobj=NULL;            
            removeobject(obj);
            logmessage((void *)&textcol,"deleted");
            rcount++;
        }
        obj=next;
    }
    return rcount;
    //should now go through selected polygon list and check for enteries with the same
    //flags, removing where necissary    
}



//copies old polygon (for possible undo later) and removes duplicated vertices
//if all vertices are removed, the polygon is flagged for deletion
UINT32 tidyuppolygon(tpolygon *p)
{
    tpolyvertex *cpv,*oldcpv,*endpv,*npv;
    INT32 npoints=0;
    //this will be placed on a stack to restore the polygons; it knows which polygon to replace from the next field 
    tpolygon *sp=T3Dcreatepolygon(p,NULL);
	sp->parent=p->parent;
    sp->next=(telem*)p;
    npv=p->vlist;
    endpv=&(cpv=sp->vlist)[sp->npoints];
    p->and_vertexrefattributes(~pvaRESERVED);
    while (cpv<endpv)
    {
        oldcpv=cpv;
        oldcpv++;
        if ((cpv->attribute & pvaRESERVED)==0)
        while (oldcpv<endpv)
        {
            if (oldcpv->v == cpv->v)            
              cpv->attribute |=pvaRESERVED;                          
            oldcpv++;                        
        }        
        if ((cpv->attribute & pvaRESERVED)==0)
        {
          *npv=*cpv;
          npv->attribute=0;
          npv++;
          npoints++;
        }
        cpv++;
    }
    p->npoints=npoints;
    //see which ones were removed. If not, tidy up vertex references    
    if (npoints<=2)
    {
        p->attribute |=paDELETEME;
        return 1;
    }
    
    return 0;          
}

UINT32 teditscene::tidyuppolygons(UINT32 omask,UINT32 pmask)
{
    UINT32 deletecount=0;
    tobject3d *obj;
    tpolygon *p;
    obj=(tobject3d*)scenegraph.root;    
    while (obj!=NULL)
    {
        if (obj->type==otGEOM &&((obj->attribute & omask)==omask))
        {
            p=(tpolygon*)((t3dmesh*)obj)->polygonlist.first;
            while (p!=NULL)
            {
                if ((p->attribute & pmask)==pmask)
                  deletecount+=tidyuppolygon(p);
                p=p->nxt();                
            }
        }
        obj=(tobject3d*)obj->next;
    }
    return deletecount;
}

void teditscene::getaverageselpos(vector *pos)
{
	pos->moveto(0,0,0);
	vertex **cv=(vertex**)sv_list.base;
	vertex **end=&cv[sv_count];
	for (;cv<end;cv++)	
		*pos+=(*cv)->data3d;
	*pos /=(flt)sv_count;	
}


//will merge vertices depending on attribute; can then be used to merge polygons
bool teditscene::mergeselectedvertices(UINT8 posmode)
{
    
    tpolygon *p;
    tpolyvertex *cpv,*endpv,*prev;
    vertex *cv;
    t3dmesh *obj;
    matrix4x4 invmat;    
    UINT32 ecount=0;
    UINT32 reversed;    
    if (sv_count<=1) return FALSE;
    vertex *v=*(vertex**)sv_list.base;
    obj=(t3dmesh*)scenegraph.root;   
	vector spos;
	getaverageselpos(&spos);
	T3Dctm=NULL;

    while (obj!=NULL)
    {                     
        T3Dgetinvertedobjectmatrix(&invmat,obj);
        if (obj->type==otGEOM && (obj->attribute & ofHASSELECTEDVERTICES) && (cv=obj->getvertexhead()))
        {   
			cv=obj->addvertex(spos.x,spos.y,spos.z);			
			
			tedge *e=(tedge*)((t3dmesh*)obj)->edgelist.first;
			while (e!=NULL)
			{
				
				if ((e->v[0]->attribute & naSELECTED) || (e->v[1]->attribute & naSELECTED))				
					e->attribute|=eaDELETEME;								
				e=(tedge*)e->next;
			}
          
			p=(tpolygon*)((t3dmesh*)obj)->polygonlist.first;
			while (p!=NULL)
			{                
				for (endpv=&(cpv=p->vlist)[p->npoints];cpv<endpv;cpv++)
	            {
					if (cpv->v->attribute & naSELECTED)
			        cpv->v=cv;
				}  
				p=p->nxt();
			}
        }
        obj=(t3dmesh*)obj->next;
    }    
    removeverticies(ofHASSELECTEDVERTICES,naSELECTED);
    removeedges(ofHASSELECTEDVERTICES,eaDELETEME);
    UINT32 tidycount=tidyuppolygons(ofHASSELECTEDVERTICES,paHASSELECTEDVERTICES);
    if (tidycount>0)
    {      
      removepolygons(ofHASSELECTEDVERTICES,paDELETEME);
      sprintf(buf,"%i polygons flaged for deletion",tidycount);
      logmessage((void*)&textcol, buf);
    }
    
    obj=(t3dmesh*)scenegraph.root;    
    while (obj!=NULL)
    {                     
        T3Dgetinvertedobjectmatrix(&invmat,obj);
        if (obj->type==otGEOM && (obj->attribute & ofHASSELECTEDVERTICES))
        {
            p=(tpolygon*)((t3dmesh*)obj)->polygonlist.first;
            while (p!=NULL)
            {
               endpv=&(cpv=p->vlist)[p->npoints];
               prev=&cpv[p->npoints-1];
               for (;cpv<endpv;cpv++)
               {                   
                   if (cpv->e->attribute & eaDELETEME)
                   {
                       cpv->e->deletereferences();                       
                       if (!(cpv->e=obj->edgesearch(cpv->v,prev->v,&reversed)))
						   cpv->e=obj->addedge(cpv->v,prev->v);

                       cpv->e->addreference(p);
                       if (cpv->e->v[1]==cpv->v)
                           cpv->attribute|=pvaEDGEISANTICLOCKWISE;
                       else if (cpv->e->v[0]!=cpv->v)
                       {                           
                           logmessage((void*)&textcol, "Whoops, not good");
                       }
                         
                       //=newedges[ecount]
                   }
                   prev=cpv;
               }
               p=p->nxt();
            }           
        }
        
        obj=(t3dmesh*)obj->next;
    }
    
    //now add polygons references to the resultant merged vertex
    //major typecasting stuff.. I will tye my brain in knots if this needs debuging
    calculatenormals(0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF);
    selectelements(ETpolygon,bOR,ofHASSELECTEDPOLYGONS,paSELECTED,checkelembyattribute_and);    
    //getch();
    return TRUE;    
}

void teditscene::triangulate(INT32 bmode)
{
    t3dmesh *obj;
    tpolygon *cp;//,*newpoly;    
    //tedge *newe1,*newe2,*bottome,*ce;

	tedge *edges[3];
    tpolygon **cpoly=(tpolygon **)sp_list.base;
    tpolyvertex *vlist;
    //these could be done using a restartable block
    //make new polygons from old ones, double the vregs and place old vrefs at even indices
    for (tpolygon **endp=&cpoly[sp_count];cpoly<endp;cpoly++)
    {        
		tpolygon *newp;
        cp=*cpoly;        
        obj=cp->parent;
        vlist=cp->vlist;
        INT32 nsides=cp->npoints;
        //UINT32 reversed;
        //UINT32 found;
        INT32 id;		
		int left=0,right=0;
		int mode=0;
		int dmode=bmode & 3;
		if (dmode < 2)
			mode=dmode;
      

        if (nsides>3)
        {

            for (id=0;id<nsides;id++)
				vlist[id].e->deletereferences(cp); 	           
		    tedge *elist=(tedge*)obj->edgelist.last;

			if (dmode>=2)							
				mode=(cp->normal.x>0)|(cp->normal.y<0)|(cp->normal.z>0);

			if (mode)
			{
				edges[0]=cp->vlist[0].e;
				left=nsides-1;
				right=1;

			}else
			{
				edges[1]=cp->vlist[0].e;
				left=nsides-2;
				right=0;
			}

	
			//classify polygon via normal here and perfrom one of two triangulizations			
			for (id=0;id<(nsides-3);id++)
	        {				
				if (mode)
				{
					edges[2]=vlist[right].e;					
					edges[1]=obj->addedge(vlist[left].v,vlist[right].v);					
					left--;											
				}
				else
				{
					edges[2]=vlist[left+1].e;						
					edges[0]=obj->addedge(vlist[left].v,vlist[right].v);					
					right++;															
				}
				newp=obj->addpoly(NULL,edges,3);															
				mode^=1;
	        }
		    if (mode)
            {
				edges[1]=vlist[left].e;				
				edges[2]=vlist[right].e;												
				
            }else
			{
				edges[2]=vlist[left+1].e;				
				edges[0]=vlist[right+1].e;												
				
			}
			obj->addpoly(NULL,edges,3);	
            //logmessage((void *)&textcol,"created edges");
            cp->attribute |=paDELETEME;
        }
        
    }
    logmessage((void *)&textcol,"Done");
    removepolygons(ofHASSELECTEDVERTICES,paDELETEME);
    calculatenormals(ofHASSELECTEDPOLYGONS,0xFFFFFFFF,0xFFFFFFFF);    
}

// the initial meshdivide routine will split all the edges, creating new polygons
void teditscene::meshdivide()
{
    tpolygon *npoly,*cp;
    tpolygon **cpoly=(tpolygon **)sp_list.base;
    tedge *tope,*bottome,*righte,*lefte,*edges[4];
    t3dmesh *obj;
    vertex *newv;
    tpolyvertex *vlist;
	T3Dctm=NULL;


    edgesplit();
    
    for (tpolygon **endp=&cpoly[sp_count];cpoly<endp;cpoly++)
    {
        cp=*cpoly;
		INT32 npoints=cp->npoints;
		if (npoints>=6 && ((npoints & 1)==0))
		{
			vlist=cp->vlist;
			cp->attribute |= paDELETEME;
			cp->calccentre();
			obj=cp->parent;
			newv=obj->addvertex(cp->centre.x,cp->centre.y,cp->centre.z);
			tope=obj->addedge(vlist[2].v,newv);
			righte=obj->addedge(vlist[4].v,newv);
			bottome=obj->addedge(vlist[6].v,newv);
			lefte=obj->addedge(vlist[0].v,newv);

			edges[3]=vlist[1].e;
			edges[2]=vlist[2].e;
			edges[1]=tope;
			edges[0]=lefte;
			npoly=obj->addpoly(NULL,edges,4);        
			npoly->calcpolynormal();

			edges[3]=vlist[3].e;
			edges[2]=vlist[4].e;
			edges[1]=righte;
			edges[0]=tope;
			npoly=obj->addpoly(NULL,edges,4);        
			npoly->calcpolynormal();

			edges[3]=vlist[5].e;
			edges[2]=vlist[6].e;
			edges[1]=bottome;
			edges[0]=righte;
			npoly=obj->addpoly(NULL,edges,4);        
			npoly->calcpolynormal();

			edges[3]=vlist[7].e;
			edges[2]=vlist[0].e;
			edges[1]=lefte;
			edges[0]=bottome;
			npoly=obj->addpoly(NULL,edges,4);        
			npoly->calcpolynormal();
		}
    
    }
    removepolygons(ofHASSELECTEDVERTICES,paDELETEME);
    
}

void teditscene::edgesplit()
{
    UINT32 tag=0;
	int i;
    vector pos;
    tpolyvertex *oldpv,*newpv;

    t3dmesh *obj;
    tedge *newe1,*newe2;
    tpolygon *newpoly,*cp;
    vertex *newv;
    tedge *ce;
    tpolygon **cpoly=(tpolygon **)sp_list.base;
    UINT32 count=0;    
	//splitting edges
    tedge **cedge=(tedge **)se_list.base;
    for (tedge **ende=&cedge[ce_count];cedge<ende;cedge++) 
    {
        //sprintf(buf,"getting next edge: %i",count);
		//build new vertices and edges
        count++;
        ce=*cedge;
        ce->attribute |=eaDELETEME;
        obj=ce->parent;
        pos=ce->v[0]->data3d+((ce->v[1]->data3d-ce->v[0]->data3d)/2.0);                        
        newv=obj->addvertex(pos.x,pos.y,pos.z);
        newv->normal=ce->v[0]->normal+((ce->v[1]->normal-ce->v[0]->normal)/2.0);                        
        newe1=obj->addedge(ce->v[0],newv);
        newe2=obj->addedge(newv,ce->v[1]);        
		//set tag to this new edge pair in old edge
		ce->tag=(UINT32)newe1;        
		ce->newv=newv;
    }
	cpoly=(tpolygon **)sp_list.base;
	for (tpolygon **endp=&cpoly[sp_count+ap_count];cpoly<endp;cpoly++)
    {        
        cp=*cpoly;
        obj=cp->parent;
        newpoly=NULL;
		//if it's selected, all edges are split
        if  (cp->attribute & paSELECTED)
          newpoly=obj->addpoly(cp->npoints << 1);
        else //otherwise, count them
        {
          count=cp->countedgeswith(eaSELECTED+eaHIGHLIGHTED);
          if (count)
            newpoly=obj->addpoly(cp->npoints+count);        
        }
        if (newpoly)
        {
			newpoly->normal=cp->normal;
			newpoly->centre=cp->centre;
			newpoly->tex=cp->tex;
			newpoly->attribute=cp->attribute;			
			newpv=&newpoly->vlist[newpoly->npoints-1];
			count=0;
			//for (tpolyvertex *end=&(oldpv=cp->vlist)[cp->npoints];oldpv<end;oldpv++)
			oldpv=cp->vlist;
			tedge *olde;

			for (i=cp->npoints-1;i>=0;i--)
			//for (i=0;i<cp->npoints;i++)
			{						
				*newpv=oldpv[i];
				olde=newpv->e;
				//olde=cp->gETedgesafe(i+1);
				//is this highlighted? If so, add two new points and edge references
				if ((olde->attribute & (eaSELECTED+eaHIGHLIGHTED) ))
				{            
					
					if (oldpv[i].attribute & pvaEDGEISANTICLOCKWISE)
					{
						newpv->e=(tedge*)((tedge*)olde->tag)->next;
						newpv->e->addreference(newpoly);
						//newpv->v=olde->newv;
						newpv--;
						*newpv=oldpv[i];
						newpv->e=(tedge*)olde->tag;																			
						newpv->v=olde->newv;
						//						
					}else
					{																										
						newpv->e=(tedge*)olde->tag;
						newpv->e->addreference(newpoly);
						newpv--;
						*newpv=oldpv[i];						
						
						newpv->e=(tedge*)((tedge*)olde->tag)->next;
						newpv->v=olde->newv;//newpv->e->v[0];
					}	
					newpv->e->addreference(newpoly);
				}else
				{      					
					newpv->e->attribute |= eaISOLDEDGE;		
					newpv->e->attribute &=~eaDELETEME;
					newpv->e->replacereference(cp,newpoly);		
					
				}								
				newpv--;               				
			}
			cp->attribute |= paDELETEME;
			*cpoly=newpoly;
        }
    }
    removeedges(ofHASSELECTEDVERTICES,eaDELETEME);
    removepolygons(ofHASSELECTEDVERTICES,paDELETEME);            
    logmessage((void*)&textcol,"done edge split");              
}
    
void teditscene::setsp_material(material *mat)
{
    tpolygon **cp,**end;
    if (selectionmode==ETpolygon)
    for (end=&(cp=(tpolygon **)sp_list.base)[sp_count];cp<end;cp++)      
      (*cp)->setmaterial(mat);
    else
    for (end=&(cp=(tpolygon **)sp_list.base)[sp_count+ap_count];cp<end;cp++)      
      (*cp)->setmaterial(mat,naSELECTED);
    
}
  
void teditscene::setsp_texture(textureref *tex)
{
    tpolygon **cp,**end;        
    for (end=&(cp=(tpolygon **)sp_list.base)[sp_count];cp<end;cp++)
    {
      (*cp)->tex=tex;
      (*cp)->attribute|=paTEXTURED;
    }
}

void teditscene::setsp_attribute(UINT32 attrib,UINT8 mode)
{
    tpolygon **cp,**end,*p;

    for (end=&(cp=(tpolygon **)sp_list.base)[sp_count];cp<end;cp++)
    {
        p=*cp;
        switch (mode)
        {
          case bPUT:
            p->attribute&=paRESERVED_MASK+paTEXTURED;
            p->attribute|=attrib;
            break;
          case bXOR:
            p->attribute|=attrib;
            break;
          case bNOT:
            p->attribute&=(0xFFFFFFFF^attrib);
            break;
          case bAND:
            p->attribute&=(attrib);
            break;
        }        
    }
}


void teditscene::setdrawmode(UINT32 mode)
{
	drawmode=mode;
}

void teditscene::setmeshdrawmode(UINT32 mode)
{
	meshdrawmode=mode;
}

void teditscene::setselectfunction(UINT32 mode)
{
	selectionfunction=mode;
}

void teditscene::setselecteffect(UINT32 mode)
{
	selectioneffect=mode;
}

void teditscene::setselectmode(UINT32 mode)
{
   selectionmode=mode;
}

vertex *teditscene::setcurrentvertex(vertex *cv)
{
    /*t3dmesh *obj=cv->parent;
    vertex *v=(vertex *)obj->vertexlist.first;
    tpolygon *p=(tpolygon*)obj->polygonlist.first;
    while (v!=NULL)
    {
        v->attribute &=~naAFFECTEDBYCURRENTVERTEX;
        v=v->nxt();
    }
    while (p!=NULL)
    {        
        if (p->contains(cv))
        {
            p->attribute |= paCONTAINSCURRENTVERTEX;
            p->or_vertexattributes(naAFFECTEDBYCURRENTVERTEX);
        }else
            p->attribute &= ~paCONTAINSCURRENTVERTEX;
        p=p->nxt();
    }

    cv->attribute |= naCURRENT;
    return (cvertex=cv);*/
    return NULL;
}

void teditscene::renderhandles(tobject3d *obj,tcamera *cam)
{
	UINT32 attrib=0;
	switch (selectionmode)
	{
		case ETvertex:attrib=rhVERTICES;break;
		case ETobject:attrib=rhOBJECTS;break;
	}
	obj->renderhandlesfull(attrib,cam);

}

void teditscene::renderhandles(tcamera *cam)
{
	UINT32 attrib=0;
	switch (selectionmode)
	{
		case ETvertex:attrib=rhVERTICES;break;
		case ETobject:attrib=rhOBJECTS;break;
	}
    
    tobject3d *obj=(tobject3d*)scenegraph.root;
    while (obj!=NULL)
    {
        obj->renderhandlesfull(attrib,cam);
        obj=(tobject3d*)obj->next;
    }
}


void teditscene::calcselvertexbounds()
{
    if (sv_count==0) return;
    vector min,max,t,nt,rad;
    min.moveto(1000000,1000000,1000000);
    max.moveto(-1000000,-1000000,-1000000);
    selectionnormal.moveto(0,0,0);    
	vector tempnormal;

    vertex **cv=(vertex**)sv_list.base,**endv,*v;
    endv=&cv[sv_count];
    

    if (sp_count)
    {
       tpolygon **cp=(tpolygon**)sp_list.base,**endp;
       endp=&cp[sp_count];
       tpolygon *poly;
    
       for (;cp<endp;cp++)
       {
          poly=*cp;
          poly->normal.multmatrix(&poly->vlist[0].v->parent->worldmat,&tempnormal);        
          selectionnormal+=tempnormal;
       }
    }else
    {
                
        for (;cv<endv;cv++)
        {
           v=*cv;
           v->normal.multmatrix(&v->parent->worldmat,&tempnormal);        
           selectionnormal+=tempnormal;
        }  
        
        
    }
    cv=(vertex**)sv_list.base;
    //selectionnormal/=sp_count; 
    for (;cv<endv;cv++)
    {
        v=*cv;
        v->data3d.multmatrixtrans(&v->parent->worldmat,&t);        
                
        if (t.x>max.x) max.x=t.x;
        if (t.y>max.y) max.y=t.y;
        if (t.z>max.z) max.z=t.z;
                            
        if (t.x<min.x) min.x=t.x;
        if (t.y<min.y) min.y=t.y;
        if (t.z<min.z) min.z=t.z;

        //v->normal.multmatrix(&v->parent->worldmattr,&v->tempnormal);
        //selectionnormal+=v->normal;//v->tempnormal;
    }
    //selectionnormal/=sv_count;
    selectionnormal.normalize();
    sprintf(buf,"Selection normal: %f %f %f",selectionnormal.x,selectionnormal.y,selectionnormal.z);
    logmessage((void*)&T3DCOLboundingbox,buf);
    slbox->setbounds(&min,&max);
    selectioncentre=slbox->p;
    slbox->normal=selectionnormal;   
}

void teditscene::calculatenormals(UINT32 objectmask,UINT32 polymask,UINT32 vertexmask)
{
    tobject3d *obj=(tobject3d*)scenegraph.root;
    while (obj!=NULL)
    {
        if ((obj->type==otGEOM)&&(obj->attribute & objectmask))
          ((t3dmesh*)obj)->calculatenormals(polymask,vertexmask);
        obj=(tobject3d*)obj->next;
    }
}

void teditscene::resetrendermode()
{
}

void vectormult(vertex *vert,vector *src,matrix4x4 *mat)
{
    src->multmatrixinv(mat,&vert->data3d);
}


textrusion *teditscene::addextrusion(UINT32 attr)
{
    textrusion *ext=&extrusionlist[extrusioncount];
    ext->attribute=attr;
    ext->sv_count=ext->se_count=ext->sp_count=ext->cnxe_count=ext->cnxp_count=0;

    ext->vlist=(vertex**)svext_list.getnextpointer();
    ext->elist=(tedge**)seext_list.getnextpointer();
    ext->plist=(tpolygon**)spext_list.getnextpointer();
    //ecnxlist;
    //pcnxlist;
    extrusioncount++;
    return ext;
    /*p=0;
    o=0;
    s=0;    */
}



bool teditscene::copyselpolygons(UINT32 nsteps,UINT32 attribute)
{ 
    UINT32 report;
    UINT32 interior;
    t3dmesh *obj=(t3dmesh*)scenegraph.root;
    tpolygon *newp;
    UINT32 ctag,ctag2;
    UINT32 tag=0;
    vertex *newv;
    vector vec;
    tedge *newe;
    
    tpolyvertex *pv; 
    textrusion *ext;    
    //cmat=imat=*tmat / count;
    while (nsteps>0)
    {
        ext=addextrusion(0);
        //logmessage((void*)&T3DCOLboundingbox,"Added extrusion");
        vertex **vlist=ext->vlist;    
        tedge **elist=ext->elist;
        tpolygon **plist=ext->plist;
        interior=((attribute & EXTRDjustedges) && (nsteps>1));
        vertex **cv=(vertex**)sv_list.base,*v,**endv;
        endv=&cv[interior?ev_count:sv_count];        //vlist=(vertex **)T3Dvisiblepolygons;
        tedge **ce=(tedge**)se_list.base,*e,**ende;
        ende=&ce[interior?se_count:ce_count];
        tpolygon **cp=(tpolygon**)sp_list.base,*p,**end;
        end=&cp[interior?0:sp_count];      
        
        tag=0;
        
        //vertex **vlist=(vertex **)T3Dvisiblepolygons;
        //tedge **elist=T3Dvisibleedges;    
        for (;cv<endv;cv++)
        {
            v=*cv;
            v->attribute&=~naSELECTED;
            v->writetag(tag);                               
            vec=v->data3d;
            //sprintf(buf,"X:%f Y:%f Z:%f",vec.x,vec.y,vec.z);
            //logmessage((void*)&T3DCOLboundingbox,buf); 
            newv=ext->addvref(v->parent->addvertex(vec.x,vec.y,vec.z));
            newv->attribute &=~ (naSELECTED|naAFFECTEDBYSELECTION);
            newv->attribute |= naEXTRUSION;
            newv->data3d=vec;
            newv->normal=v->normal;            
            tag++;
        }
        tag=0;
        
        for (;ce<ende;ce++)
        {
            e=*ce;
            t3dmesh *parent=e->parent;
            if (parent)
            {
                e->writetag(tag);
                e->attribute &=~eaSELECTIONMASK;
                ctag=e->v[0]->readtag();
                ctag2=e->v[1]->readtag();
                newe=ext->adderef(parent->addedge(vlist[ctag],vlist[ctag2],NULL));
                //copies over flags, including 'clockwise selection' flag which is needed for correct extrusion                
                newe->attribute=e->attribute | eaEXTRUSION;
                newe->attribute &=~(eaSELECTED|eaAFFECTEDBYSELECTION);
                if ((report=elist[tag]->checkintegrity()))
                    switch (report)
                    {
                        case eiVERTEXPARENTSDAMAGED :logmessage((void*)&T3DCOLboundingbox,"Edges parents are inconsistant");break;
                        case eiVERTEXNULLS          :logmessage((void*)&T3DCOLboundingbox,"Null vertex found on edge");break;
                    }
                tag++;
            }else
                logmessage((void*)&T3DCOLboundingbox,"No parent"); 
        }
        
        tag=0;
        for (;cp<end;cp++)
        {
            p=*cp;
            newp=ext->addpref(p->parent->addpoly(p->npoints,NULL));           			
            newp->tex=p->tex;
            newp->attribute=p->attribute;
            p->attribute&=~paSELECTIONMASK;
            newp->attribute = (p->attribute & ~paDELETEME)| paEXTRUSION;            
            pv=p->vlist;
            for (UINT32 i=0;i<p->npoints;i++)
            {            
                //ctag=pv->v->readtag();
                ctag2=pv->e->readtag();          
                if (ctag2<ce_count)
                {
					T3Dsetpolygonvertex(newp,elist[ctag2]->v[(pv->attribute & pvaEDGEISANTICLOCKWISE)?1:0],i,pv->attribute ,elist[ctag2]);
					elist[ctag2]->addreference(newp);                    
                    T3Dsetpolygonvertexuv(newp,i,pv->texv.x,pv->texv.y);
                    
                    newp->vlist[i].mat=p->vlist[i].mat;                                        

                }else
                    logmessage((void*)&T3DCOLboundingbox,"Polygon ref out of bounds");                
                pv++;
            }                                                            
            tag++;
        }
        nsteps--;        
    }    
    sprintf(buf,"Extrusion count:%i",extrusioncount);
    logmessage((void*)&T3DCOLboundingbox,buf); 
    //calculatenormals(ofHASSELECTEDPOLYGONS,0xFFFFFFFF,0xFFFFFFFF);    
    return TRUE;
}

void teditscene::extrudeselpolygons(UINT32 nsteps,UINT32 attribute)
{
   copyselpolygons(nsteps,attribute);
   extrudeedgelists((tedge**)se_list.base,extrusionlist[0].elist,se_count,&extrusionlist[0],attribute);          
   for (UINT32 i=0;i<nsteps-1;i++)   
       extrudeedgelists(extrusionlist[i].elist,extrusionlist[i+1].elist,se_count,&extrusionlist[i],attribute);
   logmessage((void*)&T3DCOLboundingbox,"finished extrusion"); 
}

tpolygon **teditscene::extrudeedgelists(tedge **elist1,tedge **elist2,INT32 nsteps,textrusion *ext,UINT32 attributes)
{
#define LEFT 0
#define TOP 1
#define RIGHT 2
#define BOTTOM 3

    tpointflt texv[4];
    texv[0].assign(0.0f,0.0f);
    texv[1].assign(1.0f,0.0f);
    texv[2].assign(1.0f,1.0f);
    texv[2].assign(0.0f,1.0f);
    //UINT32 found;
    UINT32 count=0;
    ext->ecnxlist=(tedge**)seext_list.getnextpointer();
    ext->pcnxlist=(tpolygon**)spext_list.getnextpointer();

    if (nsteps>0)
    {
       UINT32 oldcfflags=T3Dpolycreationflags;
       UINT32 oldatflags=T3Ddefaultpolyattributes;
       UINT32 oldeatflags=T3Ddefaultedgeattributes;
	   tedge *edges[4];       
       tpolygon *p;
       T3Ddefaultedgeattributes |= eaAFFECTEDBYSELECTION | eaEXTRUSION;
	   vertex *v1,*v2;

       for (INT32 i=0;i<nsteps;i++)
       {
          
          edges[LEFT]=elist1[i];		  
          edges[RIGHT]=elist2[i];
          
          T3Dpolycreationflags =oldcfflags;
          T3Ddefaultpolyattributes= oldatflags | paEXTRUSION;
          if (edges[LEFT]->attribute & eaCLOCKWISESELECTION)          
            T3Dpolycreationflags =oldcfflags | cfANTICLOCKWISE;
          else
            T3Dpolycreationflags =oldcfflags  &~cfANTICLOCKWISE;
          if (edges[LEFT]->attribute & eaDOUBLESIDEDEXTRUSION)
            T3Ddefaultpolyattributes|=paDOUBLESIDED;                    
          
          t3dmesh* obj=edges[LEFT]->parent;
		  UINT32 reversed;

          logmessage((void*)&textcol,"adding edges");  
		  v1=edges[LEFT]->v[0];v2=edges[RIGHT]->v[0];

          if (!(edges[TOP]=obj->edgesearch(v1,v2,&reversed,ext->ecnxlist,count)))
		  {
			  edges[TOP]=obj->addedge(v1,v2);
			  ext->addecnxref(edges[TOP]);count++;
			  
		  }          
		  v1=edges[LEFT]->v[1];v2=edges[RIGHT]->v[1];          
          if (!(edges[BOTTOM]=obj->edgesearch(v1,v2,&reversed,ext->ecnxlist,count)))
		  {
			  edges[BOTTOM]=obj->addedge(v1,v2);
			  ext->addecnxref(edges[BOTTOM]);count++;			  
		  }
                    
		  p=obj->addpoly(NULL,edges,4);
		  p->setuv(texv);
          
                             
          ext->addpcnxref(p);          
       }
       T3Dpolycreationflags =oldcfflags;
       T3Ddefaultpolyattributes=oldatflags;
       T3Ddefaultedgeattributes=oldeatflags;
       return NULL;
    }
    return NULL;    
}

UINT8 elemcheckarray[7]={0,ETobject,ETvertex+ETedge+ETpolygon+ETobject,ETobject,ETobject,0,0};

UINT32 tagcounter;
UINT32 tagpolygon(tpolygon *p)
{
    if (p->tag==0)
    {
       p->tag=tagcounter;
       tpolyvertex *v,*end;
       polycyclevars pcv;
        
       for (end=&(v=p->vlist)[p->npoints];v<end;v++)
       {
           
           tpolygon *refpoly=*v->e->startprefcycle(&pcv);                    
                 
           while (refpoly!=NULL)
           {
              
              if ((refpoly!=p) && (refpoly->tag==0) && (refpoly->attribute & paSELECTED) )
              {
                  tagpolygon(refpoly);
              }
           
              refpoly=*v->e->getnextpref(&pcv);              
           }           
        }
        return 1;
    }else
      return 0;
    
}

void teditscene::tagselpolys()
{
    /*UINT32 loopcounter;
    tagcounter=0;
    tpolygon **inbuf,**outbuf,**pbuf,**end,*swapme;

    tpolygonref *pref;    
    for (end=&(inbuf=(tpolygon **)sp_list.base)[sp_count];inbuf<end;inbuf++)
    {
       (*inbuf)->tag=tagcounter;
       tagcounter++;
    }
    pbuf=outbuf=inbuf=(tpolygon **)sp_list.base;
    tagcounter=1;
           
    tpolygon *p2;
    //first, select polygons on inside of ring
    for (UINT32 loopi=0;loopi<el_count;loopi++)
    {
        loopcounter=0;
        for (UINT32 i=0;i<el_list[loopi].count;i++)
        {
            pref=el_list[loopi].start[i]->prefs;
            while (pref!=NULL)
            {
                p2=pref->p;
                if (p2->attribute & paSELECTED)
                {
                    //sort to start of list, increment counter
                    p2->attribute &=~paSELECTED;
                    //pbuffer[loopcounter]=p2;
                    swapme=*outbuf;
                    *outbuf=p2;   //pbuf[p2->tag];
                    if (pbuf[p2->tag]!=p2)
                        logmessage((void*)&textcol,"tags don't match");
                    pbuf[p2->tag]=swapme;
                    swapme->tag=p2->tag;     
                    p2->tag=tagcounter;
                    outbuf++;
                    loopcounter++;
                }              
                pref=(tpolygonref*)pref->next;
            }         
        }
        //sprintf(buf,"Initial interior polygons%i",loopcounter);
        //logmessage((void*)&textcol,buf);            
        //for each polygon, 
        if (loopcounter>2)
        while(loopcounter)
        {        
            end=&inbuf[loopcounter];
            loopcounter=0;
            for (;inbuf<end;inbuf++)
            {             
                tpolyvertex *v,*endv;
                //go through it's edges and then ...
                for (endv=&(v=(*inbuf)->vlist)[(*inbuf)->npoints];v<endv;v++)
                {
                    pref=v->e->prefs;
                    while (pref!=NULL)
                    {              
                        p2=pref->p;
                        pref=(tpolygonref*)pref->next;
                        //if a polygon is selected, move it to the outbuffer (via a swap)
                        if (p2->attribute & paSELECTED)
                        {
                            //sort to start of list, increment counter
                            p2->attribute &=~paSELECTED;
                            if (pbuf[p2->tag]!=p2)
                                logmessage((void*)&textcol,"tags don't match");
                            //swap them
                            swapme=*outbuf;
                            *outbuf=p2;   
                            pbuf[p2->tag]=swapme;
                            swapme->tag=p2->tag;                          
                            //apply new tag                         
                            p2->tag=tagcounter;
                            //go to next polygon in out buffer
                            outbuf++;
                            //count polygons in this ring
                            loopcounter++;
                            //goto foundpoly;
                        }                 
                    }                          
                 }
                 foundpoly:;
                 //sprintf(buf,"Interior polygons%i",loopcounter);
                 //logmessage((void*)&textcol,buf);
         
            }
            
        }
        tagcounter++;        
    
    }
    
    /*for (end=&(cp=(tpolygon **)sp_list.base)[sp_count];cp<end;cp++)
    {
       if (tagpolygon(*cp))
         tagcounter++;
    }
    or_elementattributes((tattribelem**)sp_list.base,paSELECTED,sp_count);*/
}

//minimal preprocess for selection
void teditscene::preprocess_select(trect *cliprect,tpoint *screensize,tcamera *viewcam,INT32 offx,INT32 offy)
{
    trect oldvp=SC.viewport;
    tpoint oldsize=SC.size;
    SC.viewport=*cliprect;
    SC.size=*screensize;
    preprocess(viewcam,offx,offy);
    SC.viewport=oldvp;
    SC.size=oldsize;      
}

void sortforedgeloops(teditscene *scene)
{

    UINT32 i;
    vertex **vlist=(vertex**)scene->sv_list.base;
    for (i=0;i<scene->sv_count;i++)        
        vlist[i]->num=i;    
    //scene->el_count=0;
    UINT32 el_count=0;
    UINT32 destcount=0;
    UINT32 count=scene->se_count;    
    tedge *ne;    
    tedge **dlist=(tedge **)scene->se_list.base;        
    UINT32 j,loopcount;
    tedge **start=&dlist[0];
    loopcount=1;
    bool edgefound=false;

    UINT32 swapi=1;
    for (j=0;j<count;j++)
    {        
        ne=dlist[j];
        //ne->edgeloopref=el_count;
        //ne->edgeloopcount=loopcount-1;
        
        swapi=i=j+1;        
        edgefound=false;        
        while (i<count && edgefound==false)
        {            
            if (dlist[i]->sharesvertexwith(ne))
            {
              //eswap(&dlist[i],&dlist[j+1]);
              T3Deswap(&dlist[i],&dlist[swapi]);
              //swapi++;
              loopcount++;              
              edgefound=true;
            }                                
           i++;            
        }          
//        foundedge:
        if (!edgefound)
        {                
           scene->el_list[el_count].start=start;
           scene->el_list[el_count].count=loopcount;
           el_count++;
           start=&dlist[j];
           loopcount=1;
        }
        
    }
    scene->el_count=el_count;
}

void sortedgevertices(teditscene *scene)
{
    vertex *swapv;
    vertex **svlist=(vertex**)scene->sv_list.base;
    for (UINT32 i=0;i<scene->sv_count;i++)
      for (UINT32 j=i;j<scene->sv_count;j++)
        if ((svlist[i]->attribute & naINCLUDEDINEDGE)<(svlist[j]->attribute & naINCLUDEDINEDGE))
        {
          swapv=svlist[i];svlist[i]=svlist[j];svlist[j]=swapv;
        }    
}


void teditscene::find_edges(tedge *ce,bool connectifvertsselected)
{
    while (ce!=NULL)
    {
        polycyclevars pcv;
        tpolygon *refpoly=*ce->startprefcycle(&pcv);                    
        
        UINT32 selcount=0;
        UINT32 polycount=0;
                                
        while (refpoly!=NULL)
        {
             if (refpoly->attribute & paSELECTED)
                 selcount++;
             polycount++;
             refpoly=*ce->getnextpref(&pcv);                              
        }
        if (selcount>0)
           ce->attribute |= eaHIGHLIGHTED;
        else
           ce->attribute &= ~eaHIGHLIGHTED;
                      
        if ((selcount>0 && (polycount==1 || selcount<polycount))   || ((ce->selectedvertexcount()==2 && selcount==0)&& connectifvertsselected))
        {
            ce->attribute &=~(eaCLOCKWISESELECTION+eaDOUBLESIDEDEXTRUSION);
            switch (ce->isclockwise())
            {
               case TRUE:ce->attribute |=eaCLOCKWISESELECTION;break;            
               case NEITHERTRUENORFALSE:ce->attribute |=eaDOUBLESIDEDEXTRUSION;break;
            }

            ce->attribute |= eaSELECTED;
            ce->v[0]->attribute |= naINCLUDEDINEDGE;
            ce->v[1]->attribute |= naINCLUDEDINEDGE;
            se_list.grabandsetpointer(ce);
            se_count++;
        }
        else
           ce->attribute &= ~eaSELECTED;
        ce=(tedge *)ce->next;                                
    }        
}


//to be called after elements have been selected and affected items have been calculated
void teditscene::add_affected_elements_references()
{
    tobject3d **cobj,**cobjend,*obj;

    vertex *cv;
    tedge *ce;
    tpolygon *cp;
    
    ce_count=se_count;
    for (cobjend=(tobject3d**)&(cobj=(tobject3d**)so_list.base)[so_count];cobj<cobjend;cobj++)      
    {
        //for now, it'll only be of type 'geom' but will include other types later
        //for selection of beizer nodes and surfaces amoungst other things
        obj=*cobj;
        switch (obj->type)
        {                    
            case otGEOM:            
                cv=(vertex*) (((t3dmesh*)obj)->vertexlist.first);
                while (cv)
                {
                    if (cv->attribute & naAFFECTEDBYSELECTION  && ((cv->attribute & naSELECTED)==0))
                    {
                        sv_list.grabandsetpointer(cv);
                        av_count++;
                    }
                    if (cv->attribute & naINCLUDEDINEDGE)
                        ev_count++;
                    cv=(vertex*)cv->next;
                }
                ce=(tedge*) (((t3dmesh*)obj)->edgelist.first);
                //contained edges
                while (ce)
                {
                    if (ce->attribute & eaHIGHLIGHTED && ((ce->attribute & eaSELECTED)==0))                    
                    {
                        se_list.grabandsetpointer(ce);
                        ce_count++;
                    }
                    ce=(tedge*)ce->next;
                }
                
                sprintf(buf,"Contained edge count: %i selected edge count: %i",ce_count,se_count);
                logmessage((void*)&textcol,buf);
    
    
                //affected edges
                ce=(tedge*) (((t3dmesh*)obj)->edgelist.first);
                while (ce)
                {
                    if (ce->attribute & eaAFFECTEDBYSELECTION && ((ce->attribute & eaSELECTED+eaHIGHLIGHTED)==0))
                    {
                        se_list.grabandsetpointer(ce);
                        ae_count++;
                    }
                    ce=(tedge*)ce->next;
                }
                cp=(tpolygon*) (((t3dmesh*)obj)->polygonlist.first);
                while (cp)
                {
                    if (cp->attribute & paAFFECTEDBYSELECTION && ((cp->attribute & paSELECTED)==0))
                    {
                        sp_list.grabandsetpointer(cp);
                        ap_count++;
                    }
                    cp=(tpolygon*)cp->next;
                }
            break;
        }
    }
    
    /*sprintf(buf,"Edge vertices:%i",ev_count);
    logmessage((void*)&COLblue,buf);     
    sprintf(buf,"Sel Vertices:%i",sv_count);
    logmessage((void*)&COLblue,buf);
    sprintf(buf,"Sel Edges:%i",se_count);
    logmessage((void*)&COLblue,buf);
    sprintf(buf,"Contained Edges:%i",ce_count);
    logmessage((void*)&COLblue,buf);
    sprintf(buf,"Sel Polygons:%i",sp_count);
    logmessage((void*)&COLblue,buf);             */
    sprintf(buf,"Affected Polygons:%i",ap_count);
    logmessage((void*)&textcol,buf);             
}

void teditscene::do_vertex_selection_finalization(t3dmesh *obj)
{
    //now set the relevent polygon and vertex flags and select certain polygons  
    tpolygon *cp;
    
    cp=(tpolygon*)obj->polygonlist.first;                            
    while (cp!=NULL)
    {                                
         UINT32 nselected=cp->checkallvertexattributes(naSELECTED);
         //all vertices selected so the polygon is selected too                                
         if (nselected==cp->npoints)
         {
             cp->attribute |= (paALLVERTICESSELECTED+paHASSELECTEDVERTICES+paAFFECTEDBYSELECTION+paSELECTED);
             cp->or_vertexattributes(naAFFECTEDBYSELECTION+naREFERENCEDBYSELECTEDPOLYGON);
             obj->attribute|=ofHASSELECTEDPOLYGONS;
             sp_list.grabandsetpointer(cp);
             sp_count++;             
         }
         else
         {
             cp->attribute &=~(paALLVERTICESSELECTED+paHASSELECTEDVERTICES+paAFFECTEDBYSELECTION+paSELECTED);
             //if some of the vertices were selected, do this
             if (nselected)
             {
                 cp->attribute |=paHASSELECTEDVERTICES+paAFFECTEDBYSELECTION;
                 cp->or_vertexattributes(naAFFECTEDBYSELECTION);
             }
         }                                                              
         cp=cp->nxt();
     }
}

void teditscene::do_polygon_selection_finalization(t3dmesh *obj)
{
    //check to see if any vertices in the polygon list have been
    //referenced by this selection; if so, set all of them to be affected
    //bt this selection; 
    //The same goes for the polygon; clear the polyflag if none touch it
    tpolygon *cp=(tpolygon*) (obj->polygonlist.first);
    while (cp!=NULL)
    {
         //if any vertices in this polygon are referenced by a selected polygon,
         //Set their attributes so that they are affected by the selection
         if (cp->checkvertexattributes(naREFERENCEDBYSELECTEDPOLYGON))
         {
             cp->or_vertexattributes(naAFFECTEDBYSELECTION);
             cp->attribute |= paAFFECTEDBYSELECTION+paHASSELECTEDVERTICES;
         }                                     
         cp=cp->nxt();
    }                                
    //add selected vertices to selected vertex list
    vertex *cv=(vertex*) (((t3dmesh*)obj)->vertexlist.first); 
    while (cv!=NULL)
    {
         if (cv->attribute & naSELECTED)
         {
             sv_list.grabandsetpointer(cv);
             sv_count++;
         }                                                                                
         cv=cv->nxt();
    }                                    
}




UINT32 teditscene::selectelement(UINT32 elemtype,UINT32 boolmode,tattribelem *elem)
{
     if (!elem) return 0;
	 UINT32 *attribute,*pattribute;
	 if (elemtype==ETobject)
	 {
		 pattribute=attribute=&((tobject3d*)elem)->attribute;
	 }else
	 {
		 pattribute=&elem->parent->attribute;
		 attribute=&elem->attribute;
	 }
     *attribute |= elementiscurrent[elemtype];        
     *pattribute |=ofCONTAINSCURRENTPOLYGON;
     ed_scene->clearextrusion();
     ed_scene->selectelements(elemtype,boolmode,ofVISIBLE+ofENABLED+ofCONTAINSCURRENTPOLYGON,elementiscurrent[elemtype]|elementenabled[elemtype],checkelembyattribute_and);
     *attribute &= ~elementiscurrent[elemtype];        
     *pattribute &=~ofCONTAINSCURRENTPOLYGON;
     return 1;
}
                      
//This function requires the scene to be preprocessed according to a certain view first
//it selects all the elements that could be seen the last time the scene's view was calculated
//This routine also handles all the manipulation of the selection lists and the flags pertaining
//to selection and the current vertex, polygon or object.
//If there are any problems with this (and the managment of all that can be a can of worms), the answer should lie in here
//Selecting of edges is handled by the above routine, 'do edge stuff'
UINT32 teditscene::selectelements(UINT32 elemtype,UINT32 boolmode,UINT32 omask,UINT32 emask,bool (*checkelemfunc)(tattribelem *e,UINT32 mask))
{
    //go through object list, clip bounds against viewmat
    //If object is of relevent type and bounds intersect, check each element.
    //update SVL/SEL/SPL/SOL as appropriate
    //return number of elements selected
    tobject3d *obj=(tobject3d*)scenegraph.root;    
    vertex *cv;
    tpolygon *cp;
    tedge *ce;
    //s=selected, a=affected v=vertex e=edge p=polygon
    //this may be silly, I may have to ensure that older data is preserved...    
    //clear down previously selected/affected vertex flags
    switch (elemtype)
    {
       case ETvertex:                        
         and_elementattributes((tattribelem **)sv_list.base,~(naINCLUDEDINEDGE+naREFERENCEDBYSELECTEDPOLYGON+naAFFECTEDBYSELECTION),av_count+sv_count);
         break;
       case ETpolygon:
         and_elementattributes((tattribelem **)sv_list.base,~(naINCLUDEDINEDGE+naREFERENCEDBYSELECTEDPOLYGON+naAFFECTEDBYSELECTION+naSELECTED),av_count+sv_count);
         break;
    }

    sp_count=se_count=sv_count=so_count=ap_count=ae_count=av_count=ev_count=0;        
    sv_list.reset();
    se_list.reset();
    sp_list.reset();
    so_list.reset();
    logmessage((void*)&textcol,"reset lists");
    if (checkelemfunc)
    while (obj!=NULL)
    {
        //check to see if this type contains the sort of element we're dealing with
        UINT32 type=obj->type;
        //if ((elemcheckarray[type]&elemtype)!=0)
        {            
            UINT32 globalvis=((obj->attribute & omask)==omask);            
            {                
                switch (type)
                {                    
                    case otGEOM:                                                                        
                        switch (elemtype)
                        {
                        case ETvertex:
                             //if it's not visible and it contains no selected vertices already we won't do any processing.
                             if (!globalvis && (obj->attribute & (ofHASSELECTEDVERTICES+ofAFFECTEDBYSELECTION))==0) break;
                             cv=(vertex*) (((t3dmesh*)obj)->vertexlist.first);
                             obj->attribute &=~(ofHASSELECTEDVERTICES+ofAFFECTEDBYSELECTION);
                             //((t3dmesh*)obj)->and_vertexattributes(~(naINCLUDEDINEDGE+naREFERENCEDBYSELECTEDPOLYGON+naAFFECTEDBYSELECTION));
                             //select visible vertices 
                             while (cv!=NULL)
                             {
                                if ( checkelemfunc((tattribelem*)cv,emask) && (globalvis))                                
                                    cv->attribute=SETBITBYMODE(cv->attribute,naSELECTED,boolmode);
                                else if (boolmode==bPUT || boolmode==bAND )
                                    cv->attribute &=~naSELECTED;                                
                                //if the selected attribute is set, add to the selection list
                                if (cv->attribute & naSELECTED)
                                {                                                                                                            
                                    sv_list.grabandsetpointer(cv);
                                    sv_count++;                                                                        
                                }                                                   
                                cv=cv->nxt();
                            }
                            //if any vertices were selected, add this object to the selected object list
                            if (sv_count)
                            {
                                 obj->attribute|=ofAFFECTEDBYSELECTION+ofHASSELECTEDVERTICES;                                     
                                 so_list.grabandsetpointer(obj);
                                 so_count++;
                            }
                            do_vertex_selection_finalization((t3dmesh*)obj);
                            find_edges((tedge*)(((t3dmesh*)obj)->edgelist.first),true);
                            
                        break;
                        case ETedge:
                            if (!globalvis && (obj->attribute & (ofHASSELECTEDEDGES+ofAFFECTEDBYSELECTION)==0)) break;
                            ce=(tedge*) (((t3dmesh*)obj)->edgelist.first);
                             obj->attribute &=~(ofHASSELECTEDEDGES+ofAFFECTEDBYSELECTION);
                             //((t3dmesh*)obj)->and_vertexattributes(~(naINCLUDEDINEDGE+naREFERENCEDBYSELECTEDPOLYGON+naAFFECTEDBYSELECTION));
                             //select visible vertices 
                             while (ce!=NULL)
                             {
                                if ( checkelemfunc((tattribelem*)ce,emask) && (globalvis))                                
                                    ce->attribute=SETBITBYMODE(ce->attribute,eaSELECTED,boolmode);
                                else if (boolmode==bPUT || boolmode==bAND )
                                    ce->attribute &=~eaSELECTED;                                
                                //if the selected attribute is set, add to the selection list
                                if (cv->attribute & eaSELECTED)
                                {                                                                                                            
                                    se_list.grabandsetpointer(ce);
                                    se_count++;                                                                        
                                }                                                   
                                ce=(tedge*)ce->next;
                            }
                            //if any vertices were selected, add this object to the selected object list
                            if (sv_count)
                            {
                                 obj->attribute|=ofAFFECTEDBYSELECTION+ofHASSELECTEDVERTICES;                                     
                                 so_list.grabandsetpointer(obj);
                                 so_count++;
                            }                            
                            do_vertex_selection_finalization((t3dmesh*)obj);                        
                        break;                       
                        case ETpolygon:
                            //if it's not visible and it won't be affected, don't do any processing.
                            if (!globalvis && (obj->attribute & (ofHASSELECTEDPOLYGONS+ofAFFECTEDBYSELECTION)==0)) break;
                            obj->attribute&=~(ofHASSELECTEDPOLYGONS+ofAFFECTEDBYSELECTION);
                            //clear down vertex flags pertaining to selected polygons
                            //((t3dmesh*)obj)->and_vertexattributes(~(naINCLUDEDINEDGE+naREFERENCEDBYSELECTEDPOLYGON+naAFFECTEDBYSELECTION+naSELECTED));
                            cp=(tpolygon*) (((t3dmesh*)obj)->polygonlist.first);                                                         
                            while (cp!=NULL)
                            {
                                tpolyvertex *v=cp->vlist;
                                //clear down all flags pertaining to selection, save the paSELECTED flag itself
                                UINT32 result=checkelemfunc((tattribelem*)cp,emask);
                                cp->attribute &=~(paALLVERTICESSELECTED+paHASSELECTEDVERTICES+paAFFECTEDBYSELECTION);
                                //add, remove or toggle polygons from selected list
                                if ( result && (globalvis))                                                                
                                    cp->attribute=SETBITBYMODE(cp->attribute,paSELECTED+paAFFECTEDBYSELECTION+paHASSELECTEDVERTICES+paALLVERTICESSELECTED,boolmode);
                                else if (boolmode==bPUT || boolmode==bAND )                                    
                                    cp->attribute &=~paSELECTED;
                                //check if it's selected after all that...
                                if (cp->attribute & paSELECTED)
                                {
                                    //set flags in object as it's now affected by the selection                                    
                                    sp_list.grabandsetpointer(cp);
                                    obj->attribute|=ofAFFECTEDBYSELECTION+ofHASSELECTEDPOLYGONS+ofHASSELECTEDVERTICES;
                                    sp_count++;                                    
                                    //selects the verticies here
                                    cp->or_vertexattributes(naREFERENCEDBYSELECTEDPOLYGON+naSELECTED);
                                }                      
                                cp=cp->nxt();
                            }
                            //if any polygons were selected, add this object to the selected object list
                            if (sp_count)
                            {                                                                                                                 
                                so_list.grabandsetpointer(obj);
                                so_count++;                                       
                            }
                            if (globalvis)
                                do_polygon_selection_finalization((t3dmesh*)obj);    
                                                        
                            find_edges((tedge*)(((t3dmesh*)obj)->edgelist.first),false);
                        break;    
                    }
                    
                    break;                                          
                }                        
                //process this object and it's children
            }
        }
        obj=(tobject3d*)obj->next;
    }

    
    
                                 
    sortedgevertices(this);
    UINT32 count=0;
    tpolygon **cpoly=(tpolygon **)sp_list.base;

    
    
    sprintf(buf,"Edgeloops found:%i",el_count);
    logmessage((void *)&textcol,buf);
    //tagselpolys();
    /*sortforedgeloops(this);
    UINT32 count=0;
    tpolygon **cpoly=(tpolygon **)sp_list.base;

    for (tpolygon **endp=&cpoly[sp_count];cpoly<endp;cpoly++)
    {
        cp=*cpoly;
        sprintf(buf,"Current polygon:%i",count++);
        logmessage((void*)&textcol,buf);          
        cp->fixedgeflags();
    }*/
    //don't panic, just goes through the object list
    //adds affected elements to the end of the selected lists
    add_affected_elements_references();
    calcselvertexbounds();
    //could be done in one go
    return sv_count;
}

void teditscene::clearvertextags(vertex **v,UINT32 count)
{
    vertex **cvref,**end;
    cvref=v;
    for (end=&cvref[count];cvref<end;cvref++)           
        (*cvref)->writetag(0);    
}

void teditscene::prepareforpervertextransformation()
{
    vertex **cvref,**end;
    UINT32 count;
    
    //for (end=&cvref[sv_count*(extrusioncount+1)];cvref<end;cvref++)
    if (extrusioncount)
    {
        cvref=(vertex**)svext_list.base;
        count=vext_count;
    }else
    {
        cvref=(vertex**)sv_list.base;
        count=sv_count;        
    }
    for (end=&cvref[count];cvref<end;cvref++)
    {
         (*cvref)->old=(*cvref)->data3d;
         (*cvref)->writetag(0);
    }
    logmessage((void*)&T3DCOLboundingbox,"copied old");        
}

void teditscene::transformextrusion()
{
    for (UINT32 i=0;i<extrusioncount;i++)
    {
        //sprintf(buf,"transforming by matrix:%i/%i",i,extrusioncount);
        //logmessage((void*)&T3DCOLboundingbox,buf); 
        transformvertexlist(&extrusionlist[i].tmat,extrusionlist[i].vlist,extrusionlist[i].sv_count);
    }       
}



void teditscene::transformvertexlist(matrix4x4 *tmat,vertex **list,UINT32 count)
{
    vertex **cvref=list,*cv;//
    UINT32 i;    
    matrix4x4 scaleinvmat,invmat;
    
    LMidentity(&scaleinvmat);       
    tobject3d *obj=NULL;
    for (i=0;i<count;i++)
    {
        cv=*cvref;
        
        if (cv->parent!=obj)
        {
            obj=cv->parent;
            //contruct matrix; hopefully I can get this down to two function calls :)
            //sorting the vertexlist by parent object would also be a good idea
            T3Dgetinvertedobjectmatrix(&invmat,obj);
            LMconcatenatematrices(tmat,&obj->worldmat,&scaleinvmat);
            LMconcatenatematrices(&invmat,&scaleinvmat,&scaleinvmat);
            
        }
        //cv->data3d=vector(0,0,0);
        cv->old.multmatrixtrans(&scaleinvmat,&cv->data3d);
        cvref++;
    }        
}

void teditscene::prepareforperpolygontransformation()
{
    prepareforpervertextransformation();    
    tpolygon *cp,**cpref;
    UINT32 count;
    if (extrusioncount)
    {
        cpref=(tpolygon**)svext_list.base;
        count=pext_count;
    }else
    {
        cpref=(tpolygon**)sv_list.base;
        count=sp_count;        
    }

    for (tpolygon **last=&(cpref[count]);cpref<last;cpref++)
    {
        cp=*cpref;
        tpolyvertex *v,*end;
        for (end=&(v=cp->vlist)[cp->npoints];v<end;v++)
           v->v->tag++;
        cp->calccentre();//maybe not needed   
    }
}

void teditscene::transformpolygonlist(matrix4x4 *tmat,tpolygon **list,UINT32 count)
{
    //first count polygon references for each vertex
    
    matrix4x4 tmat1,tmat2,result;
    tpolygon **last=&list[count];
    tpolygon **cpref=last;
    tpolygon *cp;
    tpolyvertex *v,*end;    
    for (;cpref<last;cpref++)
    {
        cp=*cpref;
        
        //should be a function here
        LMprojectxy_1(&tmat1,&cp->normal,&cp->centre);
        LMconcatenatematrices(tmat,&tmat1,&tmat2);
        LMinvprojectxy_1(&tmat1,&cp->normal,&cp->centre);
        LMconcatenatematrices(&tmat1,&tmat2,&result);

        for (end=&(v=cp->vlist)[cp->npoints];v<end;v++)
        {
            //divide power of this by number of references
            vector temp;
            v->v->old.multmatrixtrans_delta(&result,&temp,1.0f/(flt)v->v->tag);
            v->v->data3d+=temp;
        }                                           
    }    
}


void teditscene::transformselectedpolygons(matrix4x4 *tmat)
{
    
    vertex **cvref,**end;    
    cvref=(vertex**)sv_list.base;
    //the effect of the translation is addative so zero the 3d values
    for (end=&cvref[sv_count];cvref<end;cvref++)
    {
         if ((*cvref)->attribute & naREFERENCEDBYSELECTEDPOLYGON)
           (*cvref)->data3d=0;
    }
            
    transformpolygonlist(tmat,(tpolygon**)sp_list.base,sp_count);
    
}

void teditscene::transformextrusionperpolygon()
{
}


void teditscene::projectselectionxy()
{
   matrix4x4 tmat;
   prepareforpervertextransformation();
   selectionnormal.normalize();
   LMnormalscale(&tmat,&selectionnormal,&selectioncentre,1,1,1.0);//);
   transformselectedvertices(&tmat);                       
   calculatenormals(ofHASSELECTEDVERTICES,paHASSELECTEDVERTICES,0xFFFFFFFF);
}


void projectnode(t3dmesh *obj,matrix4x4 *mat)
{
	
	matrix4x4 tmpmat;
	
	if (obj->type==otGEOM && (obj->attribute & ofHASSELECTEDVERTICES))
	{		 
		
		LMconcatenatematrices(mat,&obj->worldmat,&tmpmat);
		vertex *v=obj->getvertexhead();
		tpolygon *p=obj->getpolygonhead();
		while (v)
		{
			v->point2d.x=((v->data3d.x*tmpmat.m00 +v->data3d.y*tmpmat.m01 +v->data3d.z*tmpmat.m02)) +tmpmat.p.x;
			v->point2d.y=((v->data3d.x*tmpmat.m10 +v->data3d.y*tmpmat.m11 +v->data3d.z*tmpmat.m12)) +tmpmat.p.y;
			//v->point2d.z=((v->data3d.x*tmpmat.m20 +v->data3d.y*tmpmat.m21 +v->data3d.z*tmpmat.m22)) +tmpmat.p.z;
			float w=tmpmat.m33;
			v->point2d.x/=w;
			v->point2d.y/=w;
			v=(vertex*)v->next;
		}
		while (p)
		{
			if (p->attribute & paSELECTED)
			{
				tpolyvertex *v,*end;
				for (end=&(v=p->vlist)[p->npoints];v<end;v++)
					v->texv=v->v->point2d;
				
			}
			p=(tpolygon*)p->next;
		}
	}
	

		
	//tobject3d *currobj=(tobject3d*)child;
	/*while (currobj!=NULL)
	{
		currobj->do3dfull(cmat,cmattr,cam,cmds);
		currobj=(tobject3d*)currobj->next;    
	}*/
}

void ortho(matrix4x4 &projmat,float l,float r,float b,float t,float n,float f)
{	
	float r_l=r-l;
	float t_b=t-b;
	float f_n=f-n;
	
	projmat.m00=2/(r_l)  ; projmat.m01=0;            projmat.m02=0;         projmat.p.x=-((r+l)/(r_l))+0.5f;
	projmat.m10=0        ; projmat.m11=2/(t_b)      ;projmat.m12=0;         projmat.p.y=-((t+b)/(t_b))+0.5f;
	projmat.m20=0        ; projmat.m21=0;            projmat.m22=-2/(f_n);  projmat.p.z=-((f+n)/(f_n))+0.5f;
	projmat.m03=0        ; projmat.m13=0;            projmat.m23=0;         projmat.m33=-1;
	
}

void teditscene::applyuvobject(tplaneprojection *obj)
{
	vector p,s;
	t3dmesh *tobj=(t3dmesh*)scenegraph.root;
	matrix4x4 mat1,mat2,mat3;
	//LMxyzrotation(&mat1,-obj->o.x,-obj->o.y,-obj->o.z);	
	//obj->p.multmatrixinv(&mat1,&p);
	//obj->s.multmatrixinv(&mat1,&s);
	p=obj->p;
	s=obj->s;
	LMxyzrotation(&mat1,obj->o.x,obj->o.y,obj->o.z);	
	ortho(mat2,p.x-s.x,p.x+s.x,p.y-s.y,p.y+s.y,0,1);
	LMconcatenatematrices(&mat2,&mat1,&mat3);
	
	//LMaddscale(&mat1,&mat2,1.0f/obj->s.x,1.0f/obj->s.y,1.0f/obj->s.z);     		
	
	while (tobj)
	{
		projectnode(tobj,&mat3);
		tobj=(t3dmesh*)tobj->next;
	}
}

////////////////////////////////////////////////////////////////////////
//Container for views

/*class edit3dcontainer:public window
{
    tpoint quadcentre;
    edit3d *xy,*yz,*xz;
    view3d *pers;
    UINT8 *scr;
    edit3dcontainer(UINT32 x,UINT32 y,UINT32 w,UINT32 h);
    virtual void draw();
    virtual void changebounds(trect &bounds);    
};*/

#define CMDzoomwindow 1000
#define CMDmoveoffsets 1001

containertoolbar::containertoolbar(INT32 x,INT32 y,INT32 x2,INT32 y2):tgroup(x,y,x2,y2)
{
	insert(new backview(x,y,x2,y2,&TWINctrlbm.window_bg0));	    
	insert(toolbar=new ttoolbar(x2-70,y+2,(3<<4),15,claytoolbm,4,16,16),PIDundefined,TGdragony1|TGdragonx2);
	toolbar->addbutton(TBAmoveaction,0,CMDmoveoffsets,"Move View");
    toolbar->addbutton(TBAmoveaction,3,1002,"Zoom View");
	toolbar->addbutton(TBAmoveaction,5,1003,"Rotate View");     
	insert(zoombutton=new tbutton(x2-TWINctrlbm.windowicons.max0.w,y+2,BMreleaseaction,CMDzoomwindow,0x13,gadgets,&TWINctrlbm.windowicons.max0),PIDundefined,TGdragony1|TGdragonx2);	
}

void containertoolbar::handleevent(tevent *event)
{
	
	if (event->what==EVcommand)
	{
		switch (event->command)
		{
		  case CMDzoomwindow:evcallback2(CMDzoomwindow,pid);break;
		  case CMDmoveoffsets:evcallback2(CMDmoveoffsets,pid);break;
		}
	}
	if (event->what)
	  tgroup::handleevent(event);
}


edit3dcontainer::edit3dcontainer(UINT32 x,UINT32 y,UINT32 w,UINT32 h):tgroup(x,y,x+w,y+h)
{
	ratiocentre.assign(0.5,0.5);
	quadcentre.x=width()*float2int(ratiocentre.x);
	quadcentre.y=height()*float2int(ratiocentre.y);
	options|=OFtopselect|OFnooverlap;
	zoomref=-1;

	for (INT32 i=0;i<4;i++)
	{
		tb[i]=new containertoolbar(0,0,0,0);
		insert(tb[i],i);	
		views[i]=NULL;
	}
	
}

void edit3dcontainer::setcentre(INT32 x,INT32 y)
{	
	BOUND(x,40,width()-20);
	BOUND(y,40,height()-40);	
	ratiocentre.x=x/(flt)width();
	ratiocentre.y=y/(flt)height();
	quadcentre.x=x;
	quadcentre.y=y;
}

void edit3dcontainer::setviews(tview *v1,tview *v2,tview *v3,tview *v4)
{
	insert(views[0]=v1);
	insert(views[1]=v2);
	insert(views[2]=v3);
	insert(views[3]=v4);
}


#define cadd 3
void edit3dcontainer::changebounds(trect &bounds)
{
	tview::changebounds(bounds);
	quadcentre.x=float2int(width()*ratiocentre.x);
	quadcentre.y=float2int(height()*ratiocentre.y);	
	resizechildren();
	
}

void edit3dcontainer::resizechildren()
{
	trect bounds;
	tpoint qc=quadcentre+a;
	trect indent;
	indent.rassign(TWINctrlbm.ce0_v.l.w,TWINctrlbm.ce0_h.t.h,TWINctrlbm.ce0_v.r.w,TWINctrlbm.ce0_h.b.h);
	if (zoomref==-1)
	{
	
			
		if (views[0])
		{
			bounds.rassign(a.x+indent.a.x+20,a.y+indent.a.y+20,qc.x-indent.b.x,qc.y-indent.b.y);
			views[0]->changebounds(bounds);
		}
		bounds.rassign(a.x+20,a.y,qc.x-1,a.y+19);	
		tb[0]->changebounds(bounds);

		if (views[1])
		{
			bounds.rassign(qc.x+indent.a.x,a.y+indent.a.y+20,b.x-indent.b.x,qc.y-indent.b.y);
			views[1]->changebounds(bounds);
		}

		bounds.rassign(qc.x,a.y,b.x,a.y+19);	
		tb[1]->changebounds(bounds);
	
		if (views[2])
		{
			bounds.rassign(a.x+indent.a.x+20,qc.y+indent.a.y+20,qc.x-indent.b.x,b.y-indent.b.y);
			views[2]->changebounds(bounds);
		}
		bounds.rassign(a.x+20,qc.y,qc.x-1,qc.y+20);	
		tb[2]->changebounds(bounds);

		if (views[3])
		{
			bounds.rassign(qc.x+indent.a.x,qc.y+indent.a.y+20,b.x-indent.b.x,b.y-indent.b.y);
			views[3]->changebounds(bounds);	
		}
		bounds.rassign(qc.x,qc.y,b.x,qc.y+20);	
		tb[3]->changebounds(bounds);
	}else
	{
		bounds.rassign(a.x+2+20,a.y+2+20,b.x-2,b.y-2);
		views[zoomref]->changebounds(bounds);
		bounds.rassign(a.x,a.y,b.x,a.y+19);	
		tb[zoomref]->changebounds(bounds);
	}


}

inline void drawclientedge(tview *v)
{
	drawclientedge(v->a.x,v->a.y,v->b.x,v->b.y,TWINctrlbm.window0.ce,TWINctrlbm.ce0_h,TWINctrlbm.ce0_v,gadgets);
}


void edit3dcontainer::draw()
{
	putbitmap(a.x,a.y,a.x,a.y,a.x+19,b.y,(trect*)&TWINctrlbm.window_bg0,gadgets);
	if (zoomref==-1)
	{
		t_col=TWINpalette->nativedata[8];
		for (INT32 i=0;i<4;i++)
			drawclientedge(views[i]);	    

	}else
	{
		drawclientedge(views[zoomref]);
	}
	tgroup::draw();
	
	
}

void edit3dcontainer::handleevent(tevent *event)
{				
	if (event->what & EVmouse)
	{		
		if (state & SFmodal)
		{
			if (mb)
			{					
				if (MOUSEmoved)
				{
					if (smode & E3DShoriz)
						quadcentre.x+=mdiff.x;
					if (smode & E3DSvert)
						quadcentre.y+=mdiff.y;
					setcentre(quadcentre.x,quadcentre.y);
					resizechildren();				
					drawview();
				}				
			}else
				TWINendmodal();
		}else
		{
			tgroup::handleevent(event);
			if (event->what && mb)
			{
				tpoint qc=quadcentre+a;
				smode=0;
				if ((mp.x>views[0]->b.x) && (mp.x<views[1]->a.x))
					smode|=E3DShoriz;
				if ((mp.y>views[0]->b.y) && (mp.y<views[2]->a.y))
					smode|=E3DSvert;
				if (smode!=0)
					TWINsetmodal(this);								
					event->what=0;
	
			}
		}					
	}else if (event->what==EVcommand)
	{
		INT32 zcommand=event->infoUINT32;
		INT32 i;
		switch (event->command)
		{		
			case CMDzoomwindow:
				zoomref=(zoomref>=0)?-1:zcommand;				
				for (i=0;i<4;i++)
				{					
					if (zoomref>=0 && i!=zoomref)
					{
						views[i]->deactivate();
						tb[i]->deactivate();
					}else
					{
						views[i]->activate();
						tb[i]->activate();
					}				
				}
				if (zoomref>-1)
				{
					views[zoomref]->putinfrontof(owner->firstchild,true);        
					tb[zoomref]->putinfrontof(owner->firstchild,true);        
				}
				resizechildren();
				drawview();
				break;
			case CMDmoveoffsets :
				((edit3d*)views[zcommand])->changeoffsets(mdiff.x,mdiff.y);
				((edit3d*)views[zcommand])->calc3d();
				views[zcommand]->drawview();
				break;
		}
					
	}
	tgroup::handleevent(event);
	
	
}



////////////////////////////////////////////////////////////////////////
//basic3dview abstract class

basic3dview::basic3dview(INT16 x,INT16 y,INT16 x2,INT16 y2,teditscene *ltscene,char *scrdata):tview(x,y,x2,y2)
{
    logmessage((void*)&textcol,"Entered basic 3d view constructor");
    INT32 w=width();
    INT32 h=height();
    INT32 offs=w << 1;
    bg=getcolour2(20,80,120);
    fg=getcolour2(180,220,255);
    datasize=offs*(h+1); 
    logmessage((void*)&textcol, "about to allocate screen data");
    if (scrdata==NULL)
    {
        scrdata=new char[datasize];
    }
    scr=TWINrendercontext->createsurface(w,h,PXFrgb565,scrdata,SF3ddevice);        	
    scene=ltscene;
}

basic3dview::~basic3dview()
{
	delete [] scr->scrptr;
}



void basic3dview::resetdrawmode(UINT32 restorecommand)
{    
     scene->calc3dmode=restorecommand;
     evcallback(CMDscenealteredorth);
     scene->calc3dmode=CMdrawall;    
     scene->singleobj=NULL;            
     //endmodal();
}

void basic3dview::setsingleobjectdrawmode(tobject3d *obj)
{
     if (!obj) return;
     scene->calc3dmode=CMdrawallclip;                
     scene->singleobj=obj;
     sendcommand(CMDundraw,target,obj);                            
     scene->calc3dmode=CMlayeredsingleobj;
}

void basic3dview::setselectdrawmode(tobject3d *obj)
{
     if (!obj) return;
     if (scene->calc3dmode==CMinvselectedonly)
     {
         /*scene->calc3dmode=CMselectedonly;
         evcallback(CMDcalc3d);
         logmessage((void*)&textcol,"drawing selection again");*/
     }else
         scene->calc3dmode=CMinvselectedonly;     
     scene->singleobj=obj;//if dealing with multiple element selection, will be boundingbox. otherwise, the object containing the single element to be modified     
     evcallback(CMDcalc3d);     
     scene->calc3dmode=CMlayeredselectedonly;
}

//////////////////////////////////////////////////////////////////////////
//edit3d


void edit3d::startpolygon()
{
    scene->addobject(NULL,ed_obj);
    ed_obj->moveto(0,0,0);
    scene->grid->snapto(&ed_p1);
    scene->singleobj=ed_obj;
        
    ed_polygon=ed_obj->addpoly(ed_maxpolyvertices,NULL);
    ed_polygon->npoints=0;    
    ed_polygonvertex=ed_obj->addvertex(ed_p1.x,ed_p1.y,ed_p1.z);
    T3Dsetpolygonvertex(ed_polygon,ed_polygonvertex,ed_polygon->npoints++,0);//pvaNOTADDEDTOPLIST
    ed_polygonvertex=ed_obj->addvertex(ed_p1.x,ed_p1.y,ed_p1.z);
    T3Dsetpolygonvertex(ed_polygon,ed_polygonvertex,ed_polygon->npoints++,0);//pvaNOTADDEDTOPLIST
    scene->calc3dmode=CMlayeredsingleobj;    
    evcallback(CMDscenealtered);
    startmodal(modal_polygoncreate);
    //ed_waitingforrelease=true;    
}

void edit3d::dostretch(bool corner)
{
    ed_cornerstretch=corner;
    scene->addobject(NULL,ed_obj);
    scene->grid->snapto(&ed_p1);

    filter3dpos((mp.x-ed_oldmp.x)/T3Dscale,(mp.y-ed_oldmp.y) / T3Dscale,(mp.x-ed_oldmp.x)/T3Dscale,&ed_p);//(mp.x-oldmp.x) / T3Dscale    
    scene->grid->snapto(&ed_p);
        
    ed_obj->scale(ed_p.x,ed_p.y,ed_p.z);    
    if (!corner)
       ed_obj->moveto(ed_p1.x,ed_p1.y,ed_p1.z);
    else
    {
       vector s=ed_obj->s/2;
       switch (viewcam->camtype)
       {
           case camORTHXY:
           s.z=0.0f;break;
           case camORTHXZ:
           s.y=0.0f;s.z=-s.z;break;
           case camORTHZY:
           s.x=0.0f;s.z=-s.z;break;
       }
       ed_obj->moveto(ed_p1.x+s.x,ed_p1.y+s.y,ed_p1.z+s.z);
    }
    ed_obj->calculatenormals();
    scene->singleobj=ed_obj;
    scene->calc3dmode=CMlayeredsingleobj;    
    evcallback(CMDscenealtered);
    startmodal(modal_objectcreate);
}

void edit3d::douvstretch(bool corner)
{
    ed_cornerstretch=corner;
    scene->addobject(NULL,ed_obj);
    scene->grid->snapto(&ed_p1);

    filter3dpos((mp.x-ed_oldmp.x)/T3Dscale,(mp.y-ed_oldmp.y) / T3Dscale,(mp.x-ed_oldmp.x)/T3Dscale,&ed_p);//(mp.x-oldmp.x) / T3Dscale    
    scene->grid->snapto(&ed_p);
        
    ed_obj->scale(ed_p.x,ed_p.y,ed_p.z);    
    if (!corner)
       ed_obj->moveto(ed_p1.x,ed_p1.y,ed_p1.z);
    else
    {
       vector s=ed_obj->s/2;
       switch (viewcam->camtype)
       {
           case camORTHXY:
           s.z=0.0f;break;
           case camORTHXZ:
           s.y=0.0f;s.z=-s.z;break;
           case camORTHZY:
           s.x=0.0f;s.z=-s.z;break;
       }
       ed_obj->moveto(ed_p1.x+s.x,ed_p1.y+s.y,ed_p1.z+s.z);
    }    
    scene->singleobj=ed_obj;
    scene->calc3dmode=CMlayeredsingleobj;    
    evcallback(CMDscenealtered);
    startmodal(modal_uvobjectcreate);
}


UINT32 checkresizebounds(trectfloat &bounds,tpointfloat &m)
{
	UINT32 ed_boundcode=0;
	UINT32 halfx=float2int(bounds.a.x+bounds.halfx());
	UINT32 halfy=float2int((INT32)bounds.a.y+bounds.halfy());

	if (m_chkrect(bounds.a.x,bounds.a.y,bounds.a.x+bndctrlsize,bounds.a.y+bndctrlsize,m)) ed_boundcode=RFabove_left; else
    if (m_chkrect(bounds.b.x-bndctrlsize,bounds.a.y,bounds.b.x,bounds.a.y+bndctrlsize,m)) ed_boundcode=RFabove_right; else
    if (m_chkrect(bounds.a.x,bounds.b.y-bndctrlsize,bounds.a.x+bndctrlsize,bounds.b.y,m)) ed_boundcode=RFbellow_left; else
    if (m_chkrect(bounds.b.x-bndctrlsize,bounds.b.y-bndctrlsize,bounds.b.x,bounds.b.y,m)) ed_boundcode=RFbellow_right; else              
    if (m_chkrect(halfx-bndindent,bounds.a.y,halfx+bndindent,bounds.a.y+bndctrlsize,m)) ed_boundcode=RFabove; else 
    if (m_chkrect(halfx-bndindent,bounds.b.y-bndctrlsize,halfx+bndindent,bounds.b.y,m)) ed_boundcode=RFbellow; else             
    if (m_chkrect(bounds.a.x,halfy-bndindent,bounds.a.x+bndctrlsize,halfy+bndindent,m)) ed_boundcode=RFleft; else              
    if (m_chkrect(bounds.b.x-bndctrlsize,halfy-bndindent,bounds.b.x,halfy+bndindent,m)) ed_boundcode=RFright; else                             
    if (bounds.rcontains(m)) ed_boundcode=RFmiddle;//move
	return ed_boundcode;
}

void *edit3d::handleselect(tevent *event,tobject3d *obj)
{
    vertex *cv;   
    //tpolygon *cp;
    vector c;
    trectflt bounds;    
    tpointflt m;
    m.x=(flt)mp.x-a.x;
	m.y=(flt)mp.y-a.y;
    bool noelementsselected=TRUE;
    while ((obj!=NULL)&&(noelementsselected))
    {
        if (obj->type==otBOUNDINGBOX)
        {
            sprintf(buf,"bounds: %f,%f,%f,%f",obj->bounds.a.x,obj->bounds.a.y,obj->bounds.b.x,obj->bounds.b.y);
            logmessage((void*)&textcol,buf);
        }
        
        bounds=obj->bounds;
        //    logmessage((void*)&textcol,"Got a bounding box");
        
        bounds.sortelements();    
        if ((obj->attribute & ofENABLED) &&(bounds.rcontains(m)))
        {
            switch (obj->type)
            {
            case otGEOM:
              cv=(vertex*)((t3dmesh *)obj)->vertexlist.first;
              while (cv!=NULL)
              {
                  if (cv->point2d.closeto(&m,2.0f))
                  {
                      scene->setcurrentvertex(cv);
                      noelementsselected=FALSE;
                      matrix4x4 scaleinvmat,invmat,transmat;
                      float sx=1.0f/obj->s.x;float sy=1.0f/obj->s.y;float sz=1.0f/obj->s.z;
                      LMtranslate(&invmat,-obj->p.x,-obj->p.y,-obj->p.z);
                      LMrottrans(&scaleinvmat,obj->o.x,obj->o.y,obj->o.z,0,0,0);
                      LMtranspose(&scaleinvmat);
                      LMconcatenatematrices(&scaleinvmat,&invmat,&invmat);                      
                      LMscale(&scaleinvmat,sx,sy,sz);                      
                      LMconcatenatematrices(&scaleinvmat,&invmat,&invmat);                           
    
                      setsingleobjectdrawmode(obj);
                      
                      cv->old=cv->data3d;
                      int tx=0,ty=0;
                      /*while (mb!=0)
                      {
                          MOUSEpoll();
                          if (MOUSEmoved)
                          {
                                                      
                          get3dvec(tx+=mdiff.x,ty+=mdiff.y,&c);
                          vector r;
                          LMtranslate(&transmat,c.x,c.y,c.z);
                          LMconcatenatematrices(&transmat,&obj->worldmat,&scaleinvmat);
                          LMconcatenatematrices(&invmat,&scaleinvmat,&scaleinvmat);
                                                    
                          cv->old.multmatrixtrans(&scaleinvmat,&cv->data3d);
                          ((t3dmesh*)obj)->calculatenormals(0xFFFFFFFF,0xFFFFFFFF);
                          evcallback(CMDscenealtered);
                          
                          }
                      }
                      resetdrawmode(CMsingleobj);                      */
                      break;
                  }
                  cv=cv->nxt();
              }
            break;
            case otCAMERA:break;
            case otLIGHT:break;
            case otBOUNDINGBOX:
            
            if (*ed_bounds->count>0)
            {  
                 logmessage((void*)&textcol,"Got a bounding box");  
                 if ((T3Dcopyattributes & (STRNScopy+STRNSextrude))&& ed_scene->extrusioncount==0)
                 {                     
                     scene->startextrusion();
                 }                  
                 scene->prepareforpervertextransformation();
                 switch (scene->selectionfunction)
                 {
                    case STaxistranslate:
                        
                        setselectdrawmode(obj);//RDLselectedvertices
                        noelementsselected=FALSE;
                        startmodal(modal_axistranslatebounds);                            
                        ed_p=0;
                        ed_addmin.x=0;
                        ed_boundmin=ed_boundoldmin=((tboundingbox3d*)obj)->min+obj->p;
                        ed_boundmax=ed_boundoldmax=((tboundingbox3d*)obj)->max+obj->p;                  
                        break;
                    case STstretch:
                        ed_boundcode=0;
                        bounds=((tboundingbox3d*)obj)->bounds;                        
						ed_boundcode=checkresizebounds(bounds,m);
                                              
                                                                                  
              
                        if (ed_boundcode)
                        {                                                                                       
                            setselectdrawmode(obj);//RDLselectedvertices
                            noelementsselected=FALSE;
                            startmodal(modal_resizebounds);                            
                            ed_addmin=ed_addmax=0;
                            ed_boundmin=ed_boundoldmin=((tboundingbox3d*)obj)->min+obj->p;
                            ed_boundmax=ed_boundoldmax=((tboundingbox3d*)obj)->max+obj->p;                  
                        }
                        break;
                  case STrotate:
                      T3Dboundingboxrotation=0.0f;
                      scene->getbounds(viewcam,scene->cursor,0,0);
                      T3D2drotationpivot=scene->cursor->p2d+tpointflt((flt)a.x,(flt)a.y);
                      T3D2drotationpivot.x-=(SC.size.x >>1 );
                      T3D2drotationpivot.y-=(SC.size.y >>1 );

                      T3D2drotationpivot.x+=((b.x-a.x) >>1 );
                      T3D2drotationpivot.y+=((b.y-a.y) >>1 );                                            
                      setselectdrawmode(obj);//RDLselectedvertices
                      noelementsselected=FALSE;
                      startmodal(modal_rotatebounds);
                      
                      break;
                 case STaxisrotate:
                      T3Dboundingboxrotation=0.0f;
                                            
                      scene->getbounds(viewcam,scene->cursor,0,0);
                      T3D2drotationpivot=scene->cursor->p2d+tpointflt((flt)a.x,(flt)a.y);
                      T3D2drotationpivot.x-=(SC.size.x >>1 );
                      T3D2drotationpivot.y-=(SC.size.y >>1 );

                      T3D2drotationpivot.x+=((b.x-a.x) >>1 );
                      T3D2drotationpivot.y+=((b.y-a.y) >>1 );                                            
                      setselectdrawmode(obj);//RDLselectedvertices
                      noelementsselected=FALSE;
                      startmodal(modal_axisrotatebounds);
                      
                      break;
                 case STaxisscale:
                    setselectdrawmode(obj);//RDLselectedvertices
                    noelementsselected=FALSE;
                    startmodal(modal_axisscalebounds);                            
                    ed_scalefactor=0;                    
                   break;                 
                 }

             }
             break;
          }
        }                
        obj=(tobject3d*)obj->next;
    }
    if (noelementsselected)
    {
        ed_tempbounds.a.x=mp.x-a.x;
        ed_tempbounds.a.y=mp.y-a.y;        
        startmodal(modal_drawingselectbox);   
    }
    
    //if (obj->child)
    //    handleselect(event,(tobject3d*)obj->child);
    return (void*)ed_modalfunc;
}

void edit3d::setscaleoffsets(flt oldscale,flt newscale)
{    
    ed_cx=viewcam->p.x/oldscale;
    ed_cy=viewcam->p.y/oldscale;                   
    setoffsets((ed_cx*newscale),(ed_cy*newscale));
}
                      
void edit3d::handleevent(tevent *event)
{
    
    tview::handleevent(event);
    UINT8 oldscandown=ed_scandown;
    ed_scandown=(KEYgetscan() & (SMleftctrlkey+SMleftshiftkey));
    UINT32 what=event->what;
    //do modal
	
    if (state & SFmodal)
    {
        UINT8 oldcalcmode=scene->calc3dmode;
        scene->calc3dmode=CMdrawall;               
		/*if (what & EVmousewheel)
			{
				ed_newscale=T3Dscale+(event->infoINT32*0.1f)*(MAX(T3Dscale,0.005f));			
				if (ed_newscale<=0.00001f) ed_newscale=0.00001f;
				sendcommand(CMDorthoscalechanged,target,&ed_newscale);
			}*/
        if (ed_scandown)
        {
            //this section controls the zooming and panning behaviour; should work regardless of the current mode
            //so that the user can pan and scale whilst creating objects or manipulating the scene
			
            if (MOUSEmoved)
            {                            
                if (KEYgetscan() & SMleftctrlkey)
                {
                    ed_newscale=T3Dscale+(mdiff.y*0.005f)*(MAX(T3Dscale,0.005f));
                    if (ed_newscale<=0.00001f) ed_newscale=0.00001f;
                    sendcommand(CMDorthoscalechanged,target,&ed_newscale);
                }else
                if (KEYgetscan() & SMleftshiftkey)
                {                   
                    changeoffsets(mdiff.x,mdiff.y);
                    ed_oldmp+=mdiff;
                    calc3d();                    
                    drawview();                   
                }                        
            }
        }else
        {
           if (ed_modalfunc==NULL)
           {
               endmodal2();               
           }
           else if ((oldcalcmode==CMlayeredselectedonly)&&(oldscandown))//get back to previous state
           {
               setselectdrawmode(scene->singleobj);
           }
           else if ((scene->singleobj!=NULL)&&(oldscandown))
           {                 
               sendcommand(CMDundraw,target,scene->singleobj);
           }
        }
        scene->calc3dmode=oldcalcmode;
        if (ed_modalfunc)		
            ed_modalfunc(this,event);                               
    }else
    {
    //SHOULD NOT HAVE THIS HERE-THIS IS A FUDGE AS THE MODE IS NOT BEING RESET CORRECTLY
    scene->calc3dmode=CMdrawall;

    if (what & EVmouse)
    {
		if (what & EVmousewheel)
		{
			ed_newscale=T3Dscale+(mwheel*0.001f)*(MAX(T3Dscale,0.005f));			
            if (ed_newscale<=0.00001f) ed_newscale=0.00001f;
            sendcommand(CMDorthoscalechanged,target,&ed_newscale);
		}
        //get creation matrix here
        if ((mb==0)&&(ed_scandown))
        {
            ed_oldmp=mp;                        
            startmodal(NULL);
        }
		
                
        if (what&EVmousedown)
        {
                        
            if (mb==2)
            {
                setsingleobjectdrawmode(scene->cursor);                
                get3dpos(mp.x,mp.y,&ed_p1);
                scene->grid->snapto(&ed_p1);
                scene->cursor->moveto(ed_p1.x,ed_p1.y,ed_p1.z);
                evcallback(CMDscenealteredorth);
                startmodal(modal_cursormove);   
            }
            else if (mb==1)
            {
                get3dpos(mp.x,mp.y,&ed_p1);    
                ed_oldmp=mp;     
                
                T3Dctm=&viewcam->localmattr; 
				switch (scene->drawmode)
				{
				case DMselect:
					//here you should just check around the 4 pixel area of the mouse pointer
					scene->preprocess_select(&scr->viewport,&scr->size,viewcam);                       
					handleselect(event,(tobject3d*)scene->scenegraph.root);                                              
					break;
				case DMcreatemesh:
					switch (scene->meshdrawmode)
					{                                  						
					case MDMcube:                                              
						ed_obj=new t3dmesh();
						mkcube(ed_obj,-0.5,-0.5,-0.5,0.5,0.5,0.5);                       
						dostretch(true);
						break;
					case MDMsquare:
						/*ed_obj=new t3dmesh();
						mksquare(ed_obj,-0.5,-0.5,0.5,0.5,0);                       
						dostretch(true);                                                                                
						break;*/
					case MDMgrid:
						ed_obj=new t3dmesh();
						mkgrid(ed_obj,-0.5,-0.5,0.5,0.5,0,gridvals.xsegs,gridvals.ysegs,T3Dcheckergrid);
						//T3Drandheightgrid(&gridvals,(vertex*)ed_obj->vertexlist.first);
						
						dostretch(true);                                                                                
						break;
					case MDMcircle:
						ed_obj=new t3dmesh();
						mkcircle(ed_obj,0,0,0,1,1,0,circlevals.segs);                       
						dostretch(false);                                                                                
						break;                       
					case MDMsphere:               
						ed_obj=new t3dmesh();
						mksphere(ed_obj,0,0,0,1,1,1,PI,spherevals.xsegs,spherevals.ysegs);                      
						dostretch(false);
						break;
					case MDMselipse:               
						ed_obj=new t3dmesh();
						mksuperelipse(ed_obj,0,0,0,1,1,1,PI,selipsevals.xsegs,selipsevals.ysegs,selipsevals.p1/10.0f,selipsevals.p2/10.0f);                                              
						dostretch(false);
						break;                   
					case MDMcylinder:
						ed_obj=new t3dmesh();
						mkcylinder(ed_obj,0,0,-0.5,0.5,1,1,PI,cylindervals.xsegs);                       
						dostretch(false);                                                                                
						break;
					case MDMcone:
						ed_obj=new t3dmesh();
						mkcone(ed_obj,0,0,-0.5,0.5,1,1,PI,conevals.xsegs);                       
						dostretch(false);                                                                                
						break;
					case MDMpolygon:
						ed_obj=new t3dmesh(); //to be deleted if polygon grabs another vertex on an object                       
						startpolygon();
						break;
					}
					T3Dctm=NULL;
					break;
				case DMcreatespline:
					break;
				//case CMDcreatemetaball:
			//		break;
				case DMcreateuv:
					ed_obj=(t3dmesh*)new tplaneprojection();					
					ed_obj->rotate(viewcam->o.x,viewcam->o.y,viewcam->o.z);
					douvstretch(true);
					break;
				}
                
            }
        }
        
        event->what=0;
    }else
    if (what==EVcommand)
    switch (event->command)
    {
        case CMDxvalchanged:
             viewcam->p.x=-(flt)*(INT32*)event->infoptr;
             calc3d();
             drawview();
             break;
        case CMDyvalchanged:                     
             viewcam->p.y=-(flt)*(INT32*)event->infoptr;
             calc3d();
             drawview();
             break;
    }
    }
}


edit3d::edit3d(INT16 x,INT16 y,INT16 x2,INT16 y2,teditscene *ltscene,UINT8 axis_,char *scrdata):
basic3dview(x,y,x2,y2,ltscene,scrdata)
{
    viewcam= new tcamera();    
    viewcam->camtype=axis_;
    viewcam->p.moveto(0,0,0);
    
    switch (axis_)
    {
        case camORTHXY:viewcam->rotate(0,0,0);break;
        case camORTHZY:viewcam->rotate(0,-HALFPI,0);break;        
        case camORTHXZ:viewcam->rotate(-HALFPI,0,0);break;
    }    
    calc3d();
}

edit3d::~edit3d()
{
    delete viewcam;
}

void edit3d::startmodal(void (*modalfunc)(edit3d *ed,tevent *event))
{
    TWINsetmodal(this);
    ed_modalfunc=modalfunc;
    ed_scene=scene;    
}

void edit3d::endmodal2()
{
    TWINendmodal();
    ed_modalfunc=NULL;
    scene->calc3dmode=CMdrawall;
}

void setviewport()
{
    SC.viewport.a.x=float2int(T3Dviewport.a.x);
    SC.viewport.a.y=float2int(T3Dviewport.a.y);
    SC.viewport.b.x=float2int(T3Dviewport.b.x);
    SC.viewport.b.y=float2int(T3Dviewport.b.y);
    SC.viewport.rintersect(SC.size.x,SC.size.y);
}


void edit3d::calc3d()
{
    scene->setdisplay(scr);
	if (state & SFvisible)
	{
    t_fillcol=bg;
    t_col=fg;
    if (scr->scrptr==NULL) return;
    tobject3d *obj=scene->singleobj;
    TWINrendercontext->setoutput(scr);

	T3Dviewport.a.x=(flt)SC.viewport.a.x;
	T3Dviewport.a.y=(flt)SC.viewport.a.y;
    T3Dviewport.b.x=(flt)SC.viewport.b.x;
	T3Dviewport.b.y=(flt)SC.viewport.b.y;
    
    //set standard element masks
    T3Dresetelementmasks();
    UINT32 calc3dmode=scene->calc3dmode;
    UINT32 redrawlevel=scene->redrawlevel;

    switch (calc3dmode)
    {
        case CMdrawallclip:
          if (!obj) goto exitgracefully;
          ed_sobjoldbounds=*scene->getbounds(viewcam,obj);//
          T3Dviewport.rintersect(ed_sobjoldbounds);
          setviewport();
        case CMdrawall:
          bar(0,0,SC.size.x,SC.size.y);
          //cleardevice();
          scene->preprocess(viewcam);
          if ((T3Dobjecttypemask & otGRID)&&(scene->grid))
             scene->grid->drawgrid();          
          scene->renderwire();
          scene->renderhandles(viewcam);

          break;
        case CMlayeredsingleobjclip:
          ed_sobjoldbounds=*scene->getbounds(viewcam,obj,a.x,a.y);//
          T3Dviewport.rintersect(ed_sobjoldbounds);                    
        case CMlayeredsingleobj:        
          //sobjoldbounds=obj->bounds;
          setviewport();
          scene->preprocess_singleobject(viewcam,obj,a.x,a.y); //drawing is done in draw at the moment.. probably going to use another scratch image to get rid of the flicker
                    
          break;
        case CMsingleobj:
          if (!obj) goto exitgracefully;
          setviewport();
          scene->preprocess_singleobject(viewcam,obj);
          scene->renderwire();
          scene->renderhandles(obj,viewcam);
          break;
        case CMinvselectedonly:
          bar(0,0,SC.size.x,SC.size.y);
        case CMselectedonly:          
        case CMlayeredselectedonly:
          //setup so that it only draws selected vertices and the polygons that reference them
          if (calc3dmode!=CMinvselectedonly)
          {
              T3Dobjecttypemask=otBOUNDINGBOX+otGEOM;
              T3Dobjectprocessmask=T3Dredrawmasks[redrawlevel].object;          
              T3Dobjectprocessmaskresult=T3Dredrawmasks[redrawlevel].object;          
              T3Dpolygonprocessmaskresult=T3Dredrawmasks[redrawlevel].polygon;
              T3Dvertexprocessmaskresult=T3Dredrawmasks[redrawlevel].vertex;
          }else
              T3Dobjecttypemask=0xFFFFFFFF^otBOUNDINGBOX;
                     
          T3Dpolygonprocessmask=T3Dredrawmasks[redrawlevel].polygon;          
          T3Dvertexprocessmask=T3Dredrawmasks[redrawlevel].vertex;
          tpoint offset;
          offset.x=offset.y=0;
          if (calc3dmode==CMlayeredselectedonly)
             offset=a;                        
          //cleardevice();
          scene->preprocess(viewcam,offset.x,offset.y);
          if (calc3dmode==CMinvselectedonly)
          if ((T3Dobjecttypemask & otGRID)&&(scene->grid))
             scene->grid->drawgrid();
          
          
          if (scene->calc3dmode!=CMlayeredselectedonly)
          {                          
             scene->renderwire();
             scene->renderhandles(viewcam);
          }
          
          break;
    }
exitgracefully:
    T3Dobjectprocessmask=ofENABLED;
    T3Dobjectprocessmaskresult=ofENABLED;    
    T3Dobjecttypemask=0xFFFFFFFF;
    
    TWINrendercontext->setoutput(NULL);
	}
    //halfandsmooth(scr);   
}



void edit3d::draw()
{

    if (scr->scrptr==NULL) return;
    tobject3d *obj=scene->singleobj;
    
    switch (scene->calc3dmode)
    {    
        case CMlayeredselectedonly:    
            T3Dobjectprocessmask=T3Dobjectprocessmaskresult=ofAFFECTEDBYSELECTION+ofENABLED;          
            T3Dpolygonprocessmask=T3Dpolygonprocessmaskresult=paHASSELECTEDVERTICES+paENABLED;          
            T3Dvertexprocessmask=T3Dvertexprocessmaskresult=naSELECTED+naVISIBLE+naENABLED;
            break;
        default:
            T3Dobjectprocessmask=T3Dobjectprocessmaskresult=ofENABLED;          
            T3Dpolygonprocessmask=T3Dpolygonprocessmaskresult=paENABLED;          
            T3Dvertexprocessmask=T3Dvertexprocessmaskresult=naENABLED+naVISIBLE;             
    }
    switch (scene->calc3dmode)
    {
       case CMlayeredsingleobj:
       case CMlayeredsingleobjclip:
       case CMlayeredselectedonly:
         //should clip to bounds here
         //ed_sobjoldbounds+=a;
         //SC.viewport.rintersect(ed_sobjoldbounds);
         putbitmap(a.x,a.y,scr);
         scene->renderwire();
         if (scene->calc3dmode!=CMlayeredselectedonly)
             scene->renderhandles(obj,viewcam);
         else
             scene->renderhandles(viewcam);
         //draw guide circle
         if ((state & SFmodal)&&(ed_modalfunc==modal_objectcreate)&&(ed_xdiff==ed_ydiff))
         {
              t_col=getcolour2(255,255,0);
              circle(ed_oldmp.x+ed_xdiff,ed_oldmp.y+ed_ydiff,5);
         }
       break;
       default:
          putbitmap(a.x,a.y,scr);
    }
    //draw selection box, if applicable
    if ((state & SFmodal)&&(ed_modalfunc==modal_drawingselectbox))
    {
        t_col=T3DCOLboundingbox;
        rectangle(a.x+ed_tempbounds.a.x,a.y+ed_tempbounds.a.y,a.x+ed_tempbounds.b.x,a.y+ed_tempbounds.b.y);
    }
    t_fillcol=bg;
    if (a.x+scr->size.x<b.x)
      bar(a.x+scr->size.x,a.y,b.x,b.y);
    if (a.y+scr->size.y<b.y)
      bar(a.x,a.y+scr->size.y,a.x+scr->size.x-1,b.y);
    //reset
    T3Dobjectprocessmask=ofENABLED;
    T3Dobjectprocessmaskresult=ofENABLED;    
    T3Dobjecttypemask=0xFFFFFFFF;
}

void edit3d::changebounds(trect &bounds)
{
    if ((bounds.width()!=width())||(bounds.height()!=height()))
    {
       UINT32 nwidth=bounds.width()+1;
       UINT32 nheight=bounds.height()+1;
        
       if (((nwidth+nwidth)*nheight) <=(INT32)datasize)         
		   scr=TWINrendercontext->resizesurface(scr,nwidth,nheight,false);        
       
       calc3d();
    }
    tview::changebounds(bounds);    
}

void edit3d::get2dpos(vector *p3d,vector *p2d)
{
    //build matrix
        
}

void edit3d::filter3dpos(flt x,flt y,flt z,vector *p)
{
    switch (viewcam->camtype)
    {
    case camORTHXY:
      p->moveto(x,y,z);break;
    case camORTHXZ:
      p->moveto(x,z,y);break;
    case camORTHZY:
      p->moveto(z,y,x);break;
    }    
}




void edit3d::get3dpos(INT16 x,INT16 y,vector *p)
{
    flt posx=(x-a.x-halfx()-viewcam->p.x)/T3Dscale;
    flt posy=(y-a.y-halfy()-viewcam->p.y)/T3Dscale;
    vector c=scene->cursor->p;
    switch (viewcam->camtype)
    {
    case camORTHXY:
      p->moveto(posx,posy,c.z);break;
    case camORTHXZ:
      p->moveto(posx,c.y,-posy);break;
    case camORTHZY:
      p->moveto(c.x,posy,-posx);break;
    }                
}

void edit3d::get3dvec(INT16 x,INT16 y,vector *p)
{
    flt posx=x/T3Dscale;
    flt posy=y/T3Dscale;
    
    switch (viewcam->camtype)
    {
    case camORTHXY:
      p->moveto(posx,posy,0.0f);break;
    case camORTHXZ:
      p->moveto(posx,0.0f,-posy);break;
    case camORTHZY:
      p->moveto(0.0f,posy,-posx);break;
    }                
}



//called when inserted; usefull especially for setting the
//slider paramaters on a containers parent
void edit3d::initialupdate()
{
    valuedata valdata;
    valdata.min=-500;valdata.max=500;valdata.val=0;    
    sendcommand(CMDxdeltachanged,target,&valdata);
    sendcommand(CMDydeltachanged,target,&valdata);
}

void edit3d::setoffsets(flt x,flt y)
{
    UINT32 val;    
    viewcam->p.x=x;
    viewcam->p.y=y;
    val=float2int(-viewcam->p.x);
    sendcommand(CMDxdeltavalchanged,target,&val);
    val=float2int(-viewcam->p.y);
    sendcommand(CMDydeltavalchanged,target,&val);        
}

void edit3d::changeoffsets(INT32 x,INT32 y)
{
    UINT32 val;    
    viewcam->p.x+=x;
    viewcam->p.y+=y;
    val=float2int(-viewcam->p.x);
    sendcommand(CMDxdeltavalchanged,target,&val);
    val=float2int(-viewcam->p.y);
    sendcommand(CMDydeltavalchanged,target,&val);        
}


////////////////////////////////////////////////////////////////////////
//view3d
view3d::view3d(INT16 x,INT16 y,INT16 x2,INT16 y2,teditscene *ltscene,char *scrdata):
basic3dview(x,y,x2,y2,ltscene,scrdata)
{
    logmessage((void*)&textcol, "Entered View3d constructor");
    lockcam=0;
    rendertype=0;
    //bg=getcolour2(0,0,20);
    
    viewcam= new tcamera();
    viewcam->camtype=cam3D;
    viewcam->moveto(0,0,0);    
    
    calc3d();
    
}

view3d::~view3d()
{
}



void view3d::calc3d()
{
	scene->setdisplay(scr);
	if (state & SFvisible)
	{
    if (scr->scrptr==NULL) return;
    TWINrendercontext->setoutput(scr);
    t_fillcol=getcolour2(float2int(scene->fogcolour.r*255.0f),
								  float2int(scene->fogcolour.g*255.0f),
								  float2int(scene->fogcolour.b*255.0f));
    t_col=fg;

    cleardevice();
        
    if ((scene->curr.object)&&(lockcam))
        viewcam->lookat(scene->curr.object->p,PI/2);
    T3Dedgeprocessmask=T3Dedgeprocessmaskresult=eaHIGHLIGHTED;
    scene->preprocess(viewcam);
    T3Dedgeprocessmask=T3Dedgeprocessmaskresult=eaENABLED;
        
    t_col=getcolour2(255,255,255);
    //getch();
    scene->render();
    scene->renderwire();
    scene->renderhandles(viewcam);
    TWINrendercontext->setoutput(NULL);
	}
}    

void printerror(char *error,INT32 x,INT32 *yc)
{
	outtextxy(x,*yc+=15,error);
}

void view3d::draw()
{
    
	INT32 i;
	tobject3d *co;	
	tpolygon *cp;
	tpointflt p,np;
    if (scr->scrptr==NULL) return;
	tedge *ce;
    tpointflt fa((float)a.x,(float)a.y);
    INT32 yc=a.y-10;
    putbitmap(a.x,a.y,scr);
	t_fillcol=getcolour2(float2int(scene->fogcolour.r*255.0f),
								  float2int(scene->fogcolour.g*255.0f),
								  float2int(scene->fogcolour.b*255.0f));    
    if (a.x+scr->size.x<b.x)
      bar(a.x+scr->size.x,a.y,b.x,b.y);
    if (a.y+scr->size.y<b.y)
      bar(a.x,a.y+scr->size.y,a.x+scr->size.x,b.y);
    t_col=T3DCOLboundingbox;
	co=scene->underreticle.object;
	if (co)
	if (scene->drawmode==DMselect)
	{
		switch (scene->selectionmode)
		{
			case ETvertex:
			co->rendermouseover(scene->selectionmode,scene->underreticle.vertex,&fa,viewcam);
			break;
			case ETedge:
			ce=scene->underreticle.edge;
			if (ce)
			{
				co->rendermouseover(scene->selectionmode,ce,&fa,viewcam);
				switch (scene->underreticle.edge->checkintegrity())
				{
	                case eiVERTEXPARENTSDAMAGED :sprintf(buf,"Vertex parents inconsistant");break;
		            case eiVERTEXNULLS          :sprintf(buf,"One or more vertex is Null");break;
			        case eiROUGEPOLYGONWARNING  :sprintf(buf,"Warning: edge has more than 2 polygon references");break;
				    case eiOK:
					default:sprintf(buf,"Edge integrity is fine");break;
	            }
		        outtextxy(a.x,yc+=15,buf);
				sprintf(buf,"Poly verts: %0X,%0X",ce->pref[0],ce->pref[1]);
				outtextxy(a.x,yc+=15,buf);
				
			}
			break;
			case ETpolygon:
			if (scene->underreticle.polygon && (scene->underreticle.polygon->attribute & paVISIBLE))
			{         				
				cp=scene->underreticle.polygon;
				co->rendermouseover(scene->selectionmode,cp,&fa,viewcam);
				t_col=T3DCOLwire;
				//T3Dpolycount
				itoa(scene->sp_count,buf,10);
				outtextxy(a.x,yc+=15,buf);
				i=cp->checkintegrity();
				if (!i)
					printerror("Polygon integrity ok",a.x,&yc);
				else
				{
					if (i & piEDGESDAMAGED)								
						printerror("Polygon edges damaged",a.x,&yc);
					if (i & piVERTEXPARENTSDAMAGED)
						printerror("Vertex parent error",a.x,&yc);
					if (i & piMISSINGEDGEREFERENCE)
						printerror("missing edge back-reference",a.x,&yc);
					if (i & piSHAREDEDGEREFERENCE)
						printerror("Shared edge reference",a.x,&yc);
					if (i & piBADEDGEFLAG)
						printerror("Bad edge flag",a.x,&yc);
					if (i & piNOVERTEXREFERENCEINEDGE)
						printerror("Edge does not reference vertex at all!",a.x,&yc);
					if (i & piEDGEINWRONGPLACE)
						printerror("Edge is in the wrong place!",a.x,&yc);
				}
				
				sprintf(buf,"Tag:%X",cp);
				outtextxy(a.x,yc+=15,buf);
				sprintf(buf,"Poly verts: %0X,%0X,%0X",cp->vlist[0].v,cp->vlist[1].v,cp->vlist[2].v);
				outtextxy(a.x,yc+=15,buf);
				if (cp->npoints==3)
				{
					sprintf(buf,"Poly Edges: %0X-%0X,%0X-%0X,%0X-%0X",cp->vlist[0].e->v[0],cp->vlist[0].e->v[1],
														cp->vlist[1].e->v[0],cp->vlist[1].e->v[1],
														cp->vlist[2].e->v[0],cp->vlist[2].e->v[1]);
				}else
				{
					sprintf(buf,"Poly Edges: %0X-%0X,%0X-%0X,%0X-%0X,%0X-%0X",
							cp->vlist[0].e->v[0],cp->vlist[0].e->v[1],
							cp->vlist[1].e->v[0],cp->vlist[1].e->v[1],
							cp->vlist[2].e->v[0],cp->vlist[2].e->v[1],
							cp->vlist[3].e->v[0],cp->vlist[3].e->v[1]);
				}
					
				outtextxy(a.x,yc+=15,buf);
				sprintf(buf,"No. Sides: %i",cp->npoints);
				outtextxy(a.x,yc+=15,buf);
				outtextxy(a.x,yc+=15,buf);								
				break;
				case ETobject:															
					co->rendermouseover(scene->selectionmode,co,&fa,viewcam);
				break;
					
			}
		}
		buf[0]=0;
		switch (scene->calc3dmode)
		{
			case CMinvselectedonly:
			case CMselectedonly:          
			case CMlayeredselectedonly:
       
			switch (scene->redrawlevel)
			{
				case RDLselection:sprintf(buf,"RDLselection");break;
				case RDLextrusion:sprintf(buf,"RDLextrusion");break;             
			}                 
			break;       
		}
		outtextxy(a.x,yc+=15,buf);              
	}
}

void view3d::changebounds(trect &bounds)
{
    if ((bounds.width()!=width())||(bounds.height()!=height()))
    {
       UINT16 nwidth=bounds.width()+1;
       UINT16 nheight=bounds.height()+1;
        
       if (((nwidth+nwidth)*nheight) <=(INT32)datasize)         
           scr=TWINrendercontext->resizesurface(scr,nwidth,nheight,false);
       calc3d();
    }
    tview::changebounds(bounds);    
}

matrix4x4ptr LMrotatearound(matrix4x4ptr mat,flt rx,flt ry,vector *vec)
{
  matrix4x4 tmpmat;
  matrix4x4 tmpmat2;
  
  LMtranslate(&tmpmat,-vec->x,-vec->y,-vec->z);  
  LMrottrans(&tmpmat2,rx,ry,0,0,0,0);  
  LMconcatenatematrices(&tmpmat2,&tmpmat,mat);  
  return mat;
}


void view3d::handleevent(tevent *event)
{
    static UINT32 scanmodal=0;
    static UINT32 selectmodal=0;
    static vector dist;
    elementstruct oldelems;
    matrix4x4 rmat;
    vector moveme;
    vector p,p2;
    static flt theta,fee;
    moveme.x=moveme.y=moveme.z=0;
    tview::handleevent(event);
    UINT32 scandown=(KEYgetscan() & (SMleftctrlkey+SMleftshiftkey));
    tobject3d *obj=scene->curr.object;
    if (state & SFmodal)
    {
        if (selectmodal &&(event->what & EVmouse))
        {
           tpoint ap=mp-a;
           ed_tempbounds.rassign(ap.x,ap.y,ap.x+1,ap.y+1);
           //ed_tempbounds.rassign(50,50,scrdc.size.x-20,scrdc.size.y-20);
           scene->preprocess_select(&ed_tempbounds,&scr->size,viewcam);
           oldelems=scene->curr;
           if (T3Dpolycount>0)
           {                                      
              scene->curr.polygon=T3Dvisiblepolygons[T3Dpolycount-1];              
			  scene->curr.object=scene->curr.polygon->parent;
              if (scene->curr.polygon->attribute & paCLIPPEDPOLY)                   
                 scene->curr.polygon=scene->curr.polygon->nxt();
                               
              scene->curr.vertex=scene->curr.polygon->findclosestvertex((flt)ap.x,(flt)ap.y);
              scene->curr.vertex->attribute |= naVISIBLE;
              scene->underreticle=scene->curr;
              
              if (scene->curr.elements[scene->selectionmode]!=oldelems.elements[scene->selectionmode])
              {
                   scene->selectelement(scene->selectionmode,scene->selectioneffect,(tattribelem*)scene->curr.elements[scene->selectionmode]);
                   evcallback(CMDscenealtered);
				   evcallback(CMDcurrentelementchanged);				   
              }                             
			  
            }else
            {
                 scene->curr.clear();
                 if (scene->curr.elements[scene->selectionmode]!=oldelems.elements[scene->selectionmode])
                   drawview();               
            }
        }else
        if (MOUSEmoved)
        {            
            switch (mb)
            {            
            case 1:
                
                if ((viewcam->camtype==cam3D)&&(obj))
                {
                   vector dist=obj->p-viewcam->p;                   
                   float temp=dist.length();;
                   float FOV=viewcam->FOV;                   
                   
                   moveme.x=(temp*mdiff.x)/FOV;
                   moveme.y=(temp*mdiff.y)/FOV;
                   if (!lockcam)
                     moveme.multmatrix(&viewcam->worldmat,&p);
                   else
                     p=moveme;
                   obj->moveto(obj->p.x+p.x,obj->p.y+p.y,obj->p.z+p.z);                   
                   scene->calcselvertexbounds();    
                   evcallback(CMDscenealtered);
                }
                break;
                
            case 2:
                if ((viewcam->camtype==cam3D)&&(obj))
                {                   
                   moveme.z=(flt)mdiff.y;
                   if (!lockcam)
                     moveme.multmatrix(&viewcam->worldmat,&p);
                   else
                     p=moveme;
                   obj->moveto(obj->p.x+p.x,obj->p.y+p.y,obj->p.z+p.z);
                   if (obj->attribute&ofHASSELECTEDVERTICES)
                      scene->calcselvertexbounds();                                         
                   evcallback(CMDscenealtered);
                }
                break;
            case 3:if (obj)
                {
                    obj->rotate(obj->o.x+(flt)mdiff.y/100.0f,obj->o.y+(flt)mdiff.x/100.0f,obj->o.z);                    
                    scene->calcselvertexbounds();                       
                    evcallback(CMDscenealtered);
                }
                break;
            case 0:                
                if (scandown)
                {
                   
                   if ((KEYgetscan() & (SMleftctrlkey + SMleftshiftkey))==(SMleftctrlkey + SMleftshiftkey))
                   {
                       p=viewcam->p-scene->cursor->p;
                       flt r=p.length();
                       dist=vector(0,0,-r);
                                                                                       
                       fee=viewcam->o.x-mdiff.y/50.0f;                       
                       theta=viewcam->o.y-mdiff.x/50.0f;
                       //as dist is only facing in the z axis, these two lines could
                       //be made far simpler                     
                       LMrotatearound(&rmat,fee,theta,0,&scene->cursor->p);//theta
                       dist.multmatrix(&rmat,&viewcam->p);
                       viewcam->p+=scene->cursor->p;
                       viewcam->rotate(fee,theta,0);                                                   
                   }else
                   {
                       if (KEYgetscan() & SMleftctrlkey)
                       {                   
                           moveme.z=(flt)mdiff.y;                                            
                       }else
                       if (KEYgetscan() & SMleftshiftkey)
                       {
                           moveme.x=(flt)-mdiff.x;
                           moveme.y=(flt)-mdiff.y;                      
                       }

                       moveme.multmatrix(&viewcam->localmat,&p);
                       viewcam->moveto(viewcam->p.x+p.x,viewcam->p.y+p.y,viewcam->p.z+p.z);                       
                   }
                   evcallback(CMDscenealtered);
                }
                break;
            }
            
        }
        if (scanmodal)
        {           
           if (!scandown)
           {
               TWINendmodal();
               resetdrawmode(CMsingleobj);
           }
        }else                          
        if ((MOUSEreleased))
        {
        
            TWINendmodal();            
            resetdrawmode(selectmodal?CMselectedonly:CMsingleobj);
        }
        
            
    }else
    {        
        if ((event->what&EVmousedown)||(scandown))
        {
            scanmodal=scandown;
            selectmodal=0;
            if (!scanmodal)
            {
              //about to go into modal selection. 
              if (scene->underreticle.anynotnull())
              {
                  setselectdrawmode(scene->slbox);//RDLselectedvertices                  
                  selectmodal=1;
                  scene->curr.clear();
              }
              else 
                  setsingleobjectdrawmode(scene->curr.object);                                
            }
            else
            {              
              setsingleobjectdrawmode(viewcam);
            }            
            TWINsetmodal(this);
        }
        else if (MOUSEmoved)
        {     
            //if ((scene->selectionmode!=ETobject))
            {
               tpoint ap=mp-a;
               ed_tempbounds.rassign(ap.x,ap.y,ap.x+1,ap.y+1);
               
               //ed_tempbounds.rassign(50,50,scrdc.size.x-20,scrdc.size.y-20);
               scene->preprocess_select(&ed_tempbounds,&scr->size,viewcam);
               oldelems=scene->underreticle;
               if (T3Dpolycount>0)
               {          				   
                   scene->underreticle.polygon=T3Dvisiblepolygons[T3Dpolycount-1];
				   scene->underreticle.object=scene->underreticle.polygon->parent;
                   if (scene->underreticle.polygon->attribute & paCLIPPEDPOLY)                   
                       scene->underreticle.polygon=scene->underreticle.polygon->nxt();                   
                   scene->underreticle.vertex=scene->underreticle.polygon->findclosestvertex((flt)ap.x,(flt)ap.y);
                   scene->underreticle.edge=scene->underreticle.polygon->findclosestedge((flt)ap.x,(flt)ap.y);
				   //scene->preprocess_select(&SC.viewport,&SC.size,viewcam);
				   //scene->preprocess_singleobject(viewcam,scene->underreticle.object,0,0);
               }else
               {
                   scene->underreticle.clear();
               }
               
               if (scene->underreticle.elements[scene->selectionmode]!=oldelems.elements[scene->selectionmode])
                   drawview();               
                                                                                     
            }
        }
        event->what=0;
    }
}

//called when inserted; usefull especially for setting the
//slider paramaters on a containers parent
void view3d::initialupdate()
{
    valuedata valdata;
    valdata.min=0;valdata.max=100;valdata.val=0;    
    sendcommand(CMDxdeltachanged,target,&valdata);
}



/////////////////////////////////////////////////////////////////////////////////////////
// uv projector: plane


tplaneprojection::tplaneprojection()
{
	type=otPLANEPROJECTION;
	sprintf(name,"Plainar UV projection");
}



trectflt *tplaneprojection::getbounds(matrix4x4 *cmat,tcamera*cam)
{
    return &T3Dviewport;
}

//trectflt *tplaneprojection::getbounds(matrix4x4 *,tcamera*);
void tplaneprojection::renderhandles(UINT32 attrib,tcamera *cam)
{
	t_col=T3DCOLprojector;
	line(float2int(p2d[0].x),float2int(p2d[0].y),float2int(p2d[1].x),float2int(p2d[1].y));
	line(float2int(p2d[1].x),float2int(p2d[1].y),float2int(p2d[2].x),float2int(p2d[2].y));
	line(float2int(p2d[2].x),float2int(p2d[2].y),float2int(p2d[3].x),float2int(p2d[3].y));
	line(float2int(p2d[3].x),float2int(p2d[3].y),float2int(p2d[0].x),float2int(p2d[0].y));	
}


void tplaneprojection::do3d(matrix4x4 *cmat,matrix4x4 *cmattr,tcamera *cam,trender *cmds)
{
	vector v[4],t;
	v[0]=vector(-0.5,-0.5,0);	
	v[1]=vector(0.5,-0.5,0);	
	v[2]=vector(0.5,0.5,0);
	v[3]=vector(-0.5,0.5,0);
	for (int i=0;i<4;i++)
	{
		v[i].multmatrixtrans(cmat,&t);
		T3Dproject(&t,&p2d[i],cam);
	}		
}


/////////////////////////////////////////////////////////////////////////////////////////
// bounding box 3d


tboundingbox3d::tboundingbox3d(UINT32 *count_)
{
    count=count_;
    sprintf(name,"boundingbox");
    type=otBOUNDINGBOX;
    attribute|=ofAFFECTEDBYSELECTION;
}


void tboundingbox3d::do3d(matrix4x4 *cmat,matrix4x4 *cmattr,tcamera *cam,trender *cmds)
{
	vector temp3d;
	vertex *cv,*end=&vlist[8];
	getbounds(cmat,cam);
	//if visible
	if (((teditscene*)scene)->selectionfunction)
	{
		vector minb,maxb;
		//-boundingboxrange
		//+boundingboxrange
		vlist[0].data3d=minb=(min)-T3Dbbcentreofrotation;
		vlist[6].data3d=maxb=(max)-T3Dbbcentreofrotation;
		vlist[1].data3d.moveto(maxb.x,minb.y,minb.z);
		vlist[2].data3d.moveto(maxb.x,maxb.y,minb.z);
		vlist[3].data3d.moveto(minb.x,maxb.y,minb.z);
		vlist[4].data3d.moveto(minb.x,minb.y,maxb.z);
		vlist[5].data3d.moveto(maxb.x,minb.y,maxb.z);       
		vlist[7].data3d.moveto(minb.x,maxb.y,maxb.z);                 
		for (cv=vlist;cv<end;cv++)
		{
			cv->data3d.multmatrixtrans(cmat,&cv->temp3d);
			T3Dproject(&cv->temp3d,&cv->point2d,cam);
		}
	}else
	{
		//handled by bounds code in get bounds
		/*min.multmatrixtrans(cmat,&temp3d);
		T3Dproject(&temp3d,&minmax2d.a,cam);
		max.multmatrixtrans(cmat,&temp3d);
		T3Dproject(&temp3d,&minmax2d.b,cam);       */
	}
	vector norm;
    p2d.x=(cmat->p.x);
    p2d.y=(cmat->p.y);
    normal.multmatrix(cmattr,&norm);
    norm.normalize();
    norm*=30*T3Dscale;
    //T3Dproject(&p,&p2d,cam);
    //p2d.x=((p.x*cmat->m00 +p.y*cmat->m01 +p.z*cmat->m02) +cmat->p.x);
    //p2d.y=((p.x*cmat->m10 +p.y*cmat->m11 +p.z*cmat->m12) +cmat->p.y);
    norm2d.x=norm.x;
    norm2d.y=norm.y;
}

//sets bounds and centres them
void tboundingbox3d::setbounds(vector *_min,vector *_max)
{
    min=*_min;
    max=*_max;        
    vector c=min+((max-min) /2 );
    min-=c;
    max-=c;
    moveto(c.x,c.y,c.z);
    radius=MAX((flt)fabs(max.length()),(flt)fabs(max.length()));
}

void tboundingbox3d::renderhandles(UINT32 attrib,tcamera *cam)
{
    t_col=T3DCOLboundingbox;
    trectflt b;
    flt half;
    tpointflt t,b1,b2,b3,b4;
	
    if (cam->camtype!=cam3D)        
		if ((*count>0)&&((attribute& ofENABLED)!=0))
		{
			switch (((teditscene*)scene)->selectionfunction)
			{
			case STstretch:
				
				b=bounds;       
				if (b.a.x+boundingboxrangex2 >b.b.x) {b.b.x=bounds.a.x+boundingboxrangex2 ;b.a.x=bounds.b.x-boundingboxrangex2;}
				if (b.a.y+boundingboxrangex2 >b.b.y) {b.b.y=bounds.a.y+boundingboxrangex2 ;b.a.y=bounds.b.y-boundingboxrangex2;}
				
				rectanglef(b.a.x+bndindent,b.a.y+bndindent,b.b.x-bndindent,b.b.y-bndindent);
				rectanglef(b.a.x,b.a.y,b.a.x+bndctrlsize,b.a.y+bndctrlsize);
				rectanglef(b.b.x-bndctrlsize,b.a.y,b.b.x,b.a.y+bndctrlsize);
				rectanglef(b.a.x,b.b.y-bndctrlsize,b.a.x+bndctrlsize,b.b.y);
				rectanglef(b.b.x-bndctrlsize,b.b.y-bndctrlsize,b.b.x,b.b.y);
				half=b.a.x+b.halfx();
				rectanglef(half-bndindent,b.a.y,half+bndindent,b.a.y+bndctrlsize);
				rectanglef(half-bndindent,b.b.y-bndctrlsize,half+bndindent,b.b.y);
				half=b.a.y+b.halfy();
				rectanglef(b.a.x,half-bndindent,b.a.x+bndctrlsize,half+bndindent);    
				rectanglef(b.b.x-bndctrlsize,half-bndindent,b.b.x,half+bndindent);
				break;
			case STrotate:
				linef(vlist[0].point2d.x,vlist[0].point2d.y,vlist[1].point2d.x,vlist[1].point2d.y);
				linef(vlist[1].point2d.x,vlist[1].point2d.y,vlist[2].point2d.x,vlist[2].point2d.y);
				linef(vlist[2].point2d.x,vlist[2].point2d.y,vlist[3].point2d.x,vlist[3].point2d.y);
				linef(vlist[3].point2d.x,vlist[3].point2d.y,vlist[0].point2d.x,vlist[0].point2d.y);
				
				linef(vlist[4].point2d.x,vlist[4].point2d.y,vlist[5].point2d.x,vlist[5].point2d.y);
				linef(vlist[5].point2d.x,vlist[5].point2d.y,vlist[6].point2d.x,vlist[6].point2d.y);
				linef(vlist[6].point2d.x,vlist[6].point2d.y,vlist[7].point2d.x,vlist[7].point2d.y);
				linef(vlist[7].point2d.x,vlist[7].point2d.y,vlist[4].point2d.x,vlist[4].point2d.y);
				
				linef(vlist[0].point2d.x,vlist[0].point2d.y,vlist[4].point2d.x,vlist[4].point2d.y);
				linef(vlist[1].point2d.x,vlist[1].point2d.y,vlist[5].point2d.x,vlist[5].point2d.y);
				linef(vlist[2].point2d.x,vlist[2].point2d.y,vlist[6].point2d.x,vlist[6].point2d.y);
				linef(vlist[3].point2d.x,vlist[3].point2d.y,vlist[7].point2d.x,vlist[7].point2d.y);                                
			}
			drawarrow(p2d.x,p2d.y,p2d.x+norm2d.x,p2d.y+norm2d.y,FLT_1,0.8f,4.3f);
			circle((PIXINT)p2d.x,(PIXINT)p2d.y,(PIXINT)tempradius);  
		}    
}


trectflt *tboundingbox3d::getbounds(matrix4x4 *cmat,tcamera*cam)
{
    tempradius=radius*T3Dscale;
    bounds.a.x=((min.x*cmat->m00 +min.y*cmat->m01 +max.z*cmat->m02) +cmat->p.x);
    bounds.a.y=((min.x*cmat->m10 +min.y*cmat->m11 +max.z*cmat->m12) +cmat->p.y);
    bounds.b.x=((max.x*cmat->m00 +max.y*cmat->m01 +min.z*cmat->m02) +cmat->p.x);
    bounds.b.y=((max.x*cmat->m10 +max.y*cmat->m11 +min.z*cmat->m12) +cmat->p.y);
    bounds.sortelements();       
	bounds.a-=boundingboxrange;
	bounds.b+=boundingboxrange;
    return &bounds;
}

void tboundingbox3d::sortelements()
{
	flt temp;
	if ((temp=min.x)>max.x){min.x=max.x;max.x=temp;}
	if ((temp=min.y)>max.y){min.y=max.y;max.y=temp;}
	if ((temp=min.z)>max.z){min.z=max.z;max.z=temp;}
}


