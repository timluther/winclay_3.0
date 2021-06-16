// test.cpp : Defines the entry point for the application.
//


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include <ddraw.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <zmouse.h>
#include "resource.h"
#include "fixedp.h"

#include "graphics.h"
#include "gprim.h"
#include "logger.h"
#include "strtable.h"
#include "trender.h"
#include "basic3d.h"
#include "..\scenegraph\3dfile.h"
#include "msmouse.h"
#include "views.h"
#include "widgets.h"
#include "dialogs.h"
#include "editscn.h"
#include "perlin.h"
#include "mstwin.h"
#include "io.h"

#include "trender.h"


HINSTANCE hInst;								// current instance
HWND g_hwnd;
tsurface *toolbm,*toolbm2,*objecticons,*fancycorner;


#define CMIAtitlemenustyle 0 //MIA3dlook

//transformation mode change events

#define CMDBASEdrawmode    2000
#define CMDselect		   (CMDBASEdrawmode+DMselect)
#define CMDcreatemesh      (CMDBASEdrawmode+DMcreatemesh)
#define CMDcreatespline    (CMDBASEdrawmode+DMcreatespline)
#define CMDcreatemetaballs (CMDBASEdrawmode+DMcreatemetaballs)
#define CMDcreateuv		   (CMDBASEdrawmode+DMcreateuv)

//mesh modes
#define CMDBASEmeshdrawmode 2010


#define CMDcube          (CMDBASEmeshdrawmode+MDMcube)
#define CMDsphere        (CMDBASEmeshdrawmode+MDMsphere)
#define CMDcone          (CMDBASEmeshdrawmode+MDMcone)
#define CMDcylinder      (CMDBASEmeshdrawmode+MDMcylinder)
#define CMDselipse       (CMDBASEmeshdrawmode+MDMselipse)
#define CMDsquare        (CMDBASEmeshdrawmode+MDMsquare)
#define CMDcircle        (CMDBASEmeshdrawmode+MDMcircle)
#define CMDcreatepolygon (CMDBASEmeshdrawmode+MDMpolygon)
#define CMDtext          (CMDBASEmeshdrawmode+MDMtext)
#define CMDgrid          (CMDBASEmeshdrawmode+MDMgrid)
#define CMDline          (CMDBASEmeshdrawmode+MDMline)
#define CMDdot           (CMDBASEmeshdrawmode+MDMdot)

//select modes
#define CMDBASEselectionlevel 2030
#define CMDvertex		 (CMDBASEselectionlevel +ETvertex)
#define CMDedge			 (CMDBASEselectionlevel +ETedge)
#define CMDpolygon		 (CMDBASEselectionlevel +ETpolygon)
#define CMDobject		 (CMDBASEselectionlevel +ETobject)
//selection functions
#define CMDBASEselectionfunction 2040
#define CMDstretch		 (CMDBASEselectionfunction +STstretch)
#define CMDrotate		 (CMDBASEselectionfunction +STrotate)
#define CMDscale		 (CMDBASEselectionfunction +STscale)
#define CMDshear		 (CMDBASEselectionfunction +STshear)
#define CMDtaper		 (CMDBASEselectionfunction +STtaper)
#define CMDtwist		 (CMDBASEselectionfunction +STtwist)
#define CMDaxisrotate	 (CMDBASEselectionfunction +STaxisrotate)
#define CMDaxistranslate (CMDBASEselectionfunction +STaxistranslate)
#define CMDaxisscale	 (CMDBASEselectionfunction +STaxisscale)
  
//drawing mode change events

#define CMDapplymaterialtoselectedpolygons  920
#define CMDapplytexturetoselectedpolygons   921
#define CMDapplyattributetoselectedpolygons 922
#define CMDselectbymaterial                 923
#define CMDselectbytexture                  924
#define CMDselectbyattribute                925

#define CMDleave							999


#define CMDmaterialchanged		 900


//these are temporay; more sophisticated functionality will make these redundant
#define CMDlight1				 1001
#define CMDlight2				 1002
#define CMDlight3				 1003
#define CMDlight4				 1004
#define CMDsnapgrid				 1005
#define CMDviewgrid				 1006
#define CMDlockcam				 1501
#define CMDrendertype			 1502
#define CMDtogspecular			 1503
#define CMDtogfog				 1504
#define CMDtogonlyshowselectednodes 1505
#define CMDeditfogcolour		 1506
#define CMDfogchanged			 1507
#define CMDtoggridchecker		 1530

//Material dialog change object events
#define CMDchangeobjectsphere	 1020
#define CMDchangeobjectcube		 1021
#define CMDchangeobjectcylinder  1022
#define CMDchangeobjectcone		 1023


//default object parameter events
#define CMDcircvalchanged		 1300
#define CMDspherexvalchanged	 1301
#define CMDsphereyvalchanged	 1302
#define CMDgridxvalchanged		 1303
#define CMDgridyvalchanged		 1304
#define CMDcylindervalchanged	 1305
#define CMDconevalchanged		 1306
#define CMDselipsexvalchanged	 1307
#define CMDselipseyvalchanged	 1308
#define CMDselipsep1changed		 1309
#define CMDselipsep2changed		 1310
#define CMDnumcopieschanged		 1311
#define CMDtoggleselectcopy		 1312
#define CMDtoggleselectextrude	 1313
#define CMDtoggleselectkeeppolys 1314
#define CMDnumericalextrusion	 1315
#define CMDtoggleextrusionmode	 1316

#define CMDscreendump			 1025
#define CMDemergencysave		 1027

#define CMDtogbase				 2000
#define CMDtogsmooth			 CMDtogbase+paSHADED
#define CMDtogenvmap			 CMDtogbase+paENVMAP
#define CMDtogdouble			 CMDtogbase+paDOUBLESIDED
#define CMDtogtransparency		 CMDtogbase+paTRANSPARENT
#define CMDtognospectrans		 CMDtogbase+paNOSPECTRANS
#define CMDtoginterpolate		 CMDtogbase+paTEXTINTERPOLATED
#define CMDtogwireframe			 CMDtogbase+paOUTLINED
#define CMDtogtruecolour		 CMDtogbase+paTRUECOLOUR


#define CMDcancel				 3000
#define CMDload3dobjectdialog	 3001
#define CMDsave3dobjectdialog	 3002
#define CMDappend3dobjectdialog	 3003
#define CMDloadtexturedialog	 3004
#define CMDload3dobject			 3005
#define CMDsave3dobject			 3006
#define CMDappend3dobject		 3007
#define CMDloadtexture			 3008
#define CMDnewscene				 3009


//for material editor, brings up colour selector
#define CMDeditambient			 4000
#define CMDeditdiffuse			 4001
#define CMDeditspecular			 4002
#define CMDeditemmisive			 4003

//commands that affect the selection 

#define CMDmergeselectedvertices 5001
#define CMDflipnormals			 5002
#define CMDdeleteselected		 5003
#define CMDmeshdivide		     5004
#define CMDtriangulateleft		 5005
#define CMDtriangulateright		 5006
#define CMDtriangulatebynormal   5007
#define CMDselectnoncoplainar    5008
#define CMDedgesplit             5009
#define CMDflipx                 5501
#define CMDflipy                 5502
#define CMDflipz                 5503
#define CMDlookatselection       5510
#define CMDshrinkselection       5511
#define CMDgrowselection         5512
#define CMDbuildocttree          5513
#define CMDsetsmooth             5600  //to 5616


#define PIDload3dobject			 100
#define PIDloadtexture			 101
#define PIDsave3dobject			 102



class logmessagestring:public telem
{
    public:
    tcolour colour;
    char string[255];    
};

class logmessagebox:public tview
{
    public:
    tlinkedlist *themessages;
    logmessagestring *top;
    logmessagebox(INT16 x,INT16 y,INT16 w,INT16 h,tlinkedlist *messages);
    ~logmessagebox();
    UINT32 offset,sel,maxlines;
    void update();
    virtual void draw();
    virtual void handleevent(tevent *event);    
};



class leftbar:public tgroup
{
    public:
    ttoolbar *functionbar;
    leftbar(UINT16 x,UINT16 y,UINT16 height);
    virtual void handleevent(tevent *event);
    virtual void draw();    
};

////////////////////////////////////////////////////////////////////////
//container

class container:public tgroupwithlabels
{
    public:
	backview *bv;
    void addbackview()
    {
        insert((bv=new backview(a.x,a.y,b.x,b.y,&TWINctrlbm.window_bg0)));		
    }
													
    container(UINT8 flags=0):tgroupwithlabels(0,0,1,1)
    {
		bv=NULL;
        if (flags)
          addbackview();
        else          
          options|=OFnooverlap;        
        
    }
    virtual void handleevent(tevent *event);
    
};

class subcontainer:public tgroupwithlabels
{
    public:
	backview *bv;
    void addbackview()
    {
        insert((bv=new backview(a.x+18,a.y+16,b.x,b.y,&TWINctrlbm.window_bg0)));		
    }
    subcontainer(trect *rect,UINT8 flags=0):tgroupwithlabels(rect->a.x,rect->a.y,rect->b.x,rect->b.y)
    {		
		bv=NULL;
        if (flags)
          addbackview();
        else          
          options|=OFnooverlap;        
        
    }
    virtual void handleevent(tevent *event);
	virtual void draw()
	{
		trect rect;
		rect.rassign(0,0,27,15);
		putbitmap(a.x,a.y,&rect,fancycorner,0);
		rect.a.x=60;rect.b.x=78;				
		putbitmap(a.x,a.y+16,&rect,fancycorner,0);
		rect.a.x=27;rect.b.x=59;				
		putbitmap(a.x+27,a.y,width()-18,16,&rect,fancycorner,0);
		rect.a.x=78;rect.b.x=95;				
		putbitmap(a.x,a.y+32,18,height()-32,&rect,fancycorner,0);
		tgroupwithlabels::draw();	
	}
    
};



class clay:public tgroup
{
    public:    
    tmenu *menu;
    container *selectctrl,*meshctrl,*splinectrl,*metaballctrl,*uvctrl;    
	subcontainer *currentselectcontainer;
	subcontainer *vertexselect,*polyselect,*edgeselect,*objectselect;
	subcontainer *currentmeshcontainer;
	subcontainer *cubectrl,*spherectrl,*conectrl,*cylinderctrl,*selipsectrl,*squarectrl,*circlectrl,*polyctrl,*textctrl,*gridctrl;
    dropbox *selectioneffect;	
	droppalette *selectionleveltb,*selectionfunctions,*meshmode,*splinemode,*metaballmode,*uvmode;
	labelelem *meshlabel;
    tbutton *bextruding;
    tabbox *drawfuncs;
    tgroup *desktop;
    view3d *renderview;
    edit3d *orthxy,*orthxz,*orthzy;
    numbox *numbericextrude;
    logmessagebox *messages;
    void renderall();
    clay(INT16 x,INT16 y,INT16 x2,INT16 y2);    
    virtual void handleevent(tevent *event);
    void updateuielements();
	subcontainer *getmeshcontainer(){return (&cubectrl)[T3Dcurrentscene()->meshdrawmode];}
	subcontainer *getselectcontainer(){return (&vertexselect)[T3Dcurrentscene()->selectionmode];}
    UINT32 deleteselected();
    UINT32 removeobjects(UINT32,UINT32);
	void setdrawmode(UINT32 mode);
	void setmeshdrawmode(UINT32 mode);
    void setselectmode(UINT32 mode);
	void setselectfunction(UINT32 mode);
	void setselecteffect(UINT32 mode);
};

clay *view;


class scrollmaterialview:public tview
{
    public:
    tcolour bg,fg;
    //UINT8 backgroundmode;
    t3dmesh *obj;
    teditscene *scene;
    tcamera *viewcam;
    UINT32 datasize;
    
    tsurface *scr[3];
    INT32 offset,sel,maxopts,idxoffset;
    UINT8 getoffsetfromsel();
    scrollmaterialview(INT32 x,INT32 y,teditscene *ltscene);
	void setsel(INT32 sel);
    void setoffset(INT32 offset);
    void calc3d(UINT32 calcmask);
    bool calc3dview(INT32 idx);
    void calcinternalvars();
    virtual void draw();    
    virtual void handleevent(tevent *event);
    virtual void changebounds(trect &bounds);
};



////////////////////////////////////////////////////////////////////////
//Preferences
//currently bellow to save my scrolling fingers..

////////////////////////////////////////////////////////////////////////
//material edit

class materialedit:public tgroup
{
    public:
    trgba editcolourrgb;
    material *mat;
    ttoolbar *objecttypes;
    tabbox *tabs;    
    tbutton *togsmooth;
    tbutton *togenvmap;
    tbutton *togdouble;
    tbutton *tognospectrans;
    tbutton *togtransparent;
    tbutton *toginterpolate;
    tbutton *togtruecolour;
    tbutton *togwireframe;    
    bordercontainer *colourcontainer;
    colourselector *editcolourselector;
    tbutton *colours[4];
    tbutton *applymaterial,*applytexture,*applyattribute,*applyall;
    tbutton *selectbymaterial,*selectbytexture,*selectbyattribute;
    tcolour buttonpalette[64];        
    numbox *transparency;
    numbox *specpower;
    numbox *specstr;
    listelem *texturelist;
    tlight *l;
    
    tscene *scene;
    scrollmaterialview *renderview;
    scrollcontainer *renderscroll;
    dropbox *textures,*materials;
    UINT8 objcount;
    UINT8 editcolour;    

    materialedit(INT16 x1,INT16 y1,INT16 x2,INT16 y2,material *mat_);
    ~materialedit();
    void calc3d(UINT8 mode);
    virtual void handleevent(tevent *event);
    void createtexturelist();
    void buildbuttonpalette(UINT32 ref,UINT32 r,UINT32 g,UINT32 b,bool setmat);
};

class preferences:public tgroup
{
    public:
    trgba editcolourrgb;
    bordercontainer *colourcontainer;
    colourselector *editcolourselector;
    edittextbox *texturepaths;
    tabbox *tabs;
    tcolour buttonpalette[16];
    tbutton *fogtoggle,*ok,*cancel,*fogcolour;
    numbox *nearfog;
    numbox *farfog;
    numbox *fogdensity;
    preferences(INT16 x,INT16 y);
    virtual void handleevent(tevent *event);
    void buildbuttonpalette(UINT8 r,UINT8 g,UINT8 b);
};


/////////////////////////////////////////////////////////////////
//Globals

tlinkedlist T3Dmessages;
tcolour COLred;
tcolour COLgreen;
tcolour COLblue;
tcolour COLmagenta;
tcolour COLcyan;
tcolour COLyellow;

char exitstring[512];
fileviewstruct merge3dobject;
fileviewstruct load3dobject;
fileviewstruct save3dobject;
fileviewstruct loadtexture;

//material *pmat;
treebox *treeview;
tsurface *sphere;
tlight *plight;
tlight *plight2;
tlight *plight3;
tlight *plight4;
materialedit *matedit;
preferences *prefs;
perlinview *perlin;
richtext *rtext;
UINT32 fin=0;
window *matwin;
window *prefwin;     
window *scenetree;
window *perlinwin;
window *containerwin;
matrix4x4 T3Dcmat;
material *firstmaterial;
edit3dcontainer *container3d;
octtree *T3Docttree;


    
    


////////////////////////////////////////////////////////////////////////////////////
//THE CODE

void setupcolours()
{
    COLred=getcolour2(255,0,0);
    COLgreen=getcolour2(0,255,0);
    COLblue=getcolour2(0,0,255);
    COLmagenta=getcolour2(255,0,255);
    COLcyan=getcolour2(0,255,255);
    COLyellow=getcolour2(255,255,0);
}

void T3Daddtexturefunc(textureref *tref)
{
    if (matedit)
      matedit->createtexturelist();

}

void T3Daddmaterialfunc(material *mat)
{
    if (matedit)
    {
        matedit->renderview->calcinternalvars();
		matedit->calc3d(0);
        matedit->renderview->drawview();            
    }
}

/*
flt smoothednoise_1(flt x)
{
    return noise(x)/2  +  noise(x-1)/4  +  noise(x+1)/4;
}


flt interpolatednoise_1(flt x)
{
      INT32 int_x    = (int)x;
      frac_x = x - integer_x;

      flt v1 = smoothednoise1(int_x)
      flt v2 = smoothednoise1(int_x + 1)

      return Interpolate(v1 , v2 , frac_x)
}

*/

/*
function interpolate(flt v0, flt v1, flt v2, flt v3,flt x)
        P = (v3 - v2) - (v0 - v1)
        Q = (v0 - v1) - P
        R = v2 - v0
        S = v1

        return Px3 + Qx2 + Rx + S
  end of function*/
  
flt interpolate(flt a, flt b, flt x)
{
     flt ft = x * PI;
     flt f = (FLT_1 - (flt)cos(ft)) * .5f;
     return  a*(FLT_1-f) + b*f;
} 
       
/*
#define NUMOCTAVES 4

INT32 primes[NUMOCTAVES *3]={15731,789221,1376312589,15013,780061,1376313223,    15349,780613,1376313691, 15809,780851,1376313977  };
 

flt noise(INT32 x,INT32 y,INT32 idx)
{
  INT32 *pr=&primes[idx*3];  
  x = x + y * 57;
  x = (x<<13) ^ x;
  return ( 1.0 - ( (x * (x * x * pr[0]+ pr[1]) + pr[2]) & 0x7fffffff) / 1073741824.0);    
}


flt smoothednoise(flt x, flt y,INT32 idx)
{
    flt corners = ( noise(x-1, y-1,idx)+noise(x+1, y-1,idx)+noise(x-1, y+1,idx)+noise(x+1, y+1,idx) ) / 16.0;
    flt sides   = ( noise(x-1, y,idx)  +noise(x+1, y,idx)  +noise(x, y-1,idx)  +noise(x, y+1,idx) ) /  8.0;
    flt center  =  noise(x, y,idx) / 4.0;
    return corners + sides + center;
}

flt interpolatednoise(float x, float y,INT32 idx)
{
      INT32 int_x    = (INT32)x;
      INT32 frac_x = x - int_x;

      INT32 int_y    = (INT32)y;
      INT32 frac_y = y - int_y;
      
      flt v1 = smoothednoise(int_x,     int_y,idx);
      flt v2 = smoothednoise(int_x + 1, int_y,idx);
      flt v3 = smoothednoise(int_x,     int_y + 1,idx);
      flt v4 = smoothednoise(int_x + 1, int_y + 1,idx);

      flt i1 = interpolate(v1 , v2 , frac_x);
      flt i2 = interpolate(v3 , v4 , frac_x);

      return interpolate(i1 , i2 , frac_y);
}


flt perlinnoise_2d(flt x, flt y,flt xfreq,flt yfreq,flt p)
{
      flt total = 0;


      for (INT32 i=0;i<NUMOCTAVES;i++)
      {
          flt freqx = xfreq*i;
          flt freqy = yfreq*i;
          flt amplitude = p*i;
          total += interpolatednoise(x * xfreq, y * yfreq,i) * amplitude;
      }
      return total;
}

 

*/
void addstandardlights(teditscene *scene)
{
    plight=new tlight;
    plight3=new tlight;
	//plight->attribute &=~ofEDITABLE;
	//plight3->attribute &=~ofEDITABLE;
    /*plight2=new tlight;
    
    plight4=new tlight;
        
    strcpy(plight4->name,"Green");
    plight4->rotate(0,1,0);
    plight4->o.normalize();
    plight4->ambient=trgbfloat(0.0,0.0,0.0);
    plight4->diffuse=trgbfloat(0.0,0.3,0.0);    
    plight4->specular=trgbfloat(0.0,0.4,0.0);

    strcpy(plight2->name,"Cyan");
    plight2->o=vector(-1,0,0);
    plight2->o.normalize();      
    plight2->ambient=trgbfloat(0.0,0.0,0.0);    
    plight2->diffuse=trgbfloat(0.0,0.2,0.3);
    plight2->specular=trgbfloat(0.0,0.6,0.8);
*/
    strcpy(plight3->name,"Red Light");
    plight3->lighttype=LTpoint;
    plight3->o=vector(2,1,0);
    plight3->o.normalize();
    plight3->ambient=trgbfloat(0.001f,0.0f,0.0f);
    plight3->diffuse=trgbfloat(0.5f,0.0f,0.0f);    
    plight3->specular=trgbfloat(0.8f,0.0f,0.0f);

    strcpy(plight->name,"White Light");
    plight->lighttype=LTpoint;
    plight->rotate(0,0,0);
    plight->moveto(-60,-60,-60);
    
    plight->o.normalize();
    plight->ambient=trgbfloat(0.0f,0.0f,0.0f);
    plight->diffuse=trgbfloat(0.7f,0.7f,0.7f);
    plight->specular=trgbfloat(1.0f,1.0f,1.0f);
    
    scene->addobject(NULL,plight);
    scene->addobject(NULL,plight3);
    plight->enable(true);
    plight3->enable(true);
    /*plight->enable(TRUE);
    T3Dcurrentscene()->addobject(NULL,plight2);
    plight2->enable(FALSE);
    T3Dcurrentscene()->addobject(NULL,plight3);
    plight3->enable(FALSE);
    T3Dcurrentscene()->addobject(NULL,plight4);        
    plight4->enable(FALSE);*/
}   


void writeraw(char *filename,UINT16 w,UINT16 h,UINT16 *data)
{
    INT32 i;
    UINT8 r,g,b;
    FILE *f=fopen(filename,"wb");
    //fwrite( const void *__ptr, size_t __size, size_t __n, FILE *__fp );
    fwrite(&w,2,1,f);
    fwrite(&h,2,1,f);
    for (i=0;i<w*h;i++)      
    {
       UINT16 col=*data;
       data++;
       b=(col & 31)<<3;
       col>>=5;
       g=(col & 63)<<2;
       col>>=6;
       r=(col & 31)<<3;
       fwrite(&r,1,1,f);
       fwrite(&g,1,1,f);
       fwrite(&b,1,1,f);                      
    }
    fclose(f);
}

void writeraw2(char *filename,UINT16 w,UINT16 h,UINT16 *data)
{
    INT32 i;
    UINT8 r,g,b,a;
    FILE *f=fopen(filename,"wb");
    //fwrite( const void *__ptr, size_t __size, size_t __n, FILE *__fp );
    fwrite(&w,2,1,f);
    fwrite(&h,2,1,f);
    for (i=0;i<w*h;i++)      
    {
       UINT16 col=*data;
       if (col==0x0)
         a=0;
       else
         a=0xFF;         
       data++;
       b=(col & 31)<<3;
       col>>=5;
       g=(col & 63)<<2;
       col>>=6;
       r=(col & 31)<<3;
       fwrite(&r,1,1,f);
       fwrite(&g,1,1,f);
       fwrite(&b,1,1,f);
       fwrite(&a,1,1,f);       
    }
    fclose(f);
}



////////////////////////////////////////////////////////////////////////
//Container class
void subcontainer::handleevent(tevent *event)
{
   tgroup::handleevent(event);

   if (event->what==EVcommand)
   {       
       owner->owner->handleevent(event);
       //evcallback(event->command);       
   }
}




void container::handleevent(tevent *event)
{
   tgroup::handleevent(event);

   if (event->what==EVcommand)
   {       
       owner->owner->handleevent(event);
       //evcallback(event->command);       
   }
}




////////////////////////////////////////////////////////////////////////
//scrollmaterialview : Multiple renders in a single control; only 3 visible at a
//time         

#define SVCMselected 1
#define SVCMallvisible 2

#define SVDMselected 1
#define SVDMallvisible 2

#define svwidth 100
#define svheight 98

scrollmaterialview::scrollmaterialview(INT32 x,INT32 y,teditscene *ltscene):tview(x,y,x+100,y+200)
{
    INT32 w=svwidth-4 ;
    INT32 h=svheight-2 ;
    INT32 offs=w << 1;
    bg=getcolour2(20,80,120);
    fg=getcolour2(180,220,255);    
    //logmessage((void*)&textcol, "about to allocate screen data");
    for (int i=0;i<3;i++)
    {       
       scr[i]=TWINrendercontext->createsurface(w,h,PXFrgb565);       
    }
    viewcam= new tcamera();
    viewcam->camtype=cam3D;
    viewcam->moveto(0,0,-130);
    offset=sel=idxoffset=0;
    sel=T3Dmaterialbank.getindex((telem*)T3Dmaterial);//get current index.
    maxopts=T3Dmaterialbank.count;
    
    scene=ltscene;          
    logmessage((void*)&textcol,"created scroll3d view");
    //drawmode=SVDMallvisible;
    
}

void scrollmaterialview::calcinternalvars()
{
    valuedata valdata;
    maxopts=T3Dmaterialbank.count;    
    INT16 temp=(svheight*maxopts) - height();
    valdata.min=0;valdata.max=temp;valdata.val=offset;    
    sendcommand(CMDydeltachanged,owner,&valdata);    
}

void scrollmaterialview::changebounds(trect &bounds)
{
    tview::changebounds(bounds);
    calcinternalvars();
}

void scrollmaterialview::setoffset(INT32 offset_)
{
    if (offset!=offset_)
    {       
       INT32 off;              
       INT32 oldidxoffset=idxoffset;
       INT32 oldoffset=offset;       
       offset=offset_;
       idxoffset=offset/svheight;
       if (offset>oldoffset)
       {
           oldidxoffset++;
           off=a.y+(oldidxoffset*svheight)-oldoffset;              
           while (off<b.y)
           {
              off+=svheight;
              oldidxoffset++;
           }
           while (calc3dview(oldidxoffset++));
       }else if (offset<oldoffset)
       {
           oldidxoffset--;
           off=a.y+(svheight+(oldidxoffset*svheight))-oldoffset;              
           while (off>a.y)
           {
              off-=svheight;
              oldidxoffset--;
           }           
           while (calc3dview(oldidxoffset--));           
       }
           
       //do calculations for newly visible views
       drawview();
    }
    
}


	



/*void drawline(float x1,float y1,float x2,float y2,float w1,float w2)
{
	float nx,ny;
	float dx,dy;
	dx=x2-x1;
	dy=y2-y1;
	trivert tv[3];
	float length=(float)sqrt(dx*dx+dy*dy);
	if (length)
	{
	ny=(dx/length)*w1;
	nx=(dy/length)*w1;//length
	x1-=ny * 0.5f;
	y1-=nx * 0.5f;
	x2+=ny * 0.5f;
	y2+=nx * 0.5f;


	tv[0].x=x1-nx;
	tv[0].y=y1+ny;

	tv[1].x=x1+nx;
	tv[1].y=y1-ny;

	tv[2].x=x2-nx;
	tv[2].y=y2+ny;
	
	tri(&tv[0],&tv[1],&tv[2]);
	tv[0].x=x2+nx;
	tv[0].y=y2-ny;
	
	tri(&tv[0],&tv[1],&tv[2]);
	}
}*/

void scrollmaterialview::draw()
{

    INT16 idx=idxoffset;
    INT16 ypos=a.y- (offset % svheight);
    for (int i=0;i<3;i++)
    {        
		t_fillcol=(idx==sel)?getcolour(0x00FF0000):TWINtextcol;
        bar(a.x,ypos+2,a.x+1,ypos+svheight-1);
        bar(b.x-1,ypos+2,b.x,ypos+svheight-1);
             
        t_fillcol=((!idx && !sel)||((idx-1)==sel)||((idx)==sel))?getcolour(0x00FF0000):TWINtextcol;
        bar(a.x,ypos,b.x,ypos+1);
        bar(a.x,ypos+svheight,b.x,ypos+svheight+2);
                       
        putbitmap(a.x+2,ypos+2,scr[idx % 3]);
        idx++;
        ypos+=svheight;		
    }
	
}

void scrollmaterialview::calc3d(UINT32 calcmask)
{
	
    UINT8 idx=offset/svheight;
    if (calcmask==SVCMselected)	
      calc3dview(sel);
    else
    {       
      calc3dview(idx);
      calc3dview(idx+1);
      calc3dview(idx+2);
    }
}

bool scrollmaterialview::calc3dview(INT32 idx)
{	
    
    INT32 off=a.y+(idx*svheight)-offset;    
    if ((off+svheight>a.y)&&(off<(b.y)))
    {
        material *oldmat=T3Dmaterial;
        T3Dmaterial=(material *)T3Dmaterialbank.findelement(idx);
		if (T3Dmaterial)
		{            
		sprintf(buf,"%i -%s",idx,T3Dmaterial->name);        		
        idx %=3;
		scene->setdisplay(scr[idx]);
        if (scr[idx]->scrptr==NULL) return false;
        
    
        TWINrendercontext->setoutput(scr[idx]);
        t_fillcol=bg;
        t_col=fg;
        colourgrid2(0,0,25,25,5,5);
        scene->preprocess(viewcam);
        scene->render();
        t_col=getcolour2(255,255,255);
        outtextxy(2,2,buf);

        TWINrendercontext->setoutput(NULL);
        }else
		{
			idx %=3;
			TWINrendercontext->setoutput(scr[idx]);
			t_fillcol=getcolour2(128,128,128);
			cleardevice();
			TWINrendercontext->setoutput(NULL);
		}
        T3Dmaterial=oldmat;

        //tsoundoff();
        return true;
    }else
        return false;

}

void scrollmaterialview::handleevent(tevent *event)
{
    //static 
    tview::handleevent(event);
    
    if (state & SFmodal)
    {                      
        if ((MOUSEmoved)&&(mb==2)&&(obj))
        {                                    
            obj->rotate(obj->o.x+(flt)mdiff.y/100.0f,obj->o.y+(flt)mdiff.x/100.0f,obj->o.z);
            calc3d(SVCMselected);
            drawview();               
        }                 
        if (MOUSEreleased)        
            TWINendmodal();                       
    }else
    {        
        if (event->what&EVmousedown)
        {
            
                INT32 oldsel=sel;
                sel=((offset+(mp.y-a.y))/svheight);
                if (oldsel!=sel)
                {
                   T3Dmaterial=(material*)T3Dmaterialbank.findelement(sel);
                   evcallback(CMDmaterialchanged);
                   drawview();
                }
            
                TWINsetmodal(this);
            event->what=0;
        }else if (event->what == EVcommand)
        {
          switch (event->command)
          {          
          case CMDyvalchanged:
              
              setoffset(*(UINT32*)event->infoptr);
              
              break;                
          }          
        }else if (event->what & EVkeydown)
		{
			switch (scancode)
			{
				case insertkey:
					T3Dmaterial=T3Daddmaterial(trgbfloat(0.0f,0.0f,0.0f),trgbfloat(0.9f,0.9f,0.9f),trgbfloat(0.7f,0.7f,0.7f),trgbfloat(0.0f,0.0f,0.0f),0,0);
					break;
				case backspacekey:
					
					if (T3Dmaterialbank.count>3)
					{
						sel=MAX(0,sel-1);
						setoffset(sel);
						T3Ddeletematerial(T3Dmaterial);						
						T3Dmaterial=(material*)T3Dmaterialbank.findelement(sel);						
					}
					
					break;
			}
		}

		
    }
    
}


INT32 parselogfile(char *filename)
{
	strcpy(T3D_TEXTURE_PATHS,"{EXE_PATH}, {MODEL_PATH}, {EXE_PATH}objects, {EXE_PATH}images, c:\\3d\\des\\bitmaps\\");
	return true;
}

////////////////////////////////////////////////////////////////////////////////////
//Preferences

#define bheight 16

void preferences::buildbuttonpalette(UINT8 r,UINT8 g,UINT8 b)
{   
  // COLgrad(buttonpalette,0,8,trgba(0,0,0,0),trgba(r,g,b,0));
  // COLgrad(buttonpalette,8,14,trgba(r,g,b,0),trgba(255,255,255,0));
}   



preferences::preferences(INT16 x,INT16 y):tgroup(x,y,x+320,y+200)
{
    tabs=new tabbox(a.x+5,a.y+5,b.x-5,b.y-35);
    container *sceneprefs=new container();
	container *paths=new container(1);

	
    editcolourselector=new colourselector(0,0,&editcolourrgb);
    colourcontainer=new bordercontainer(0,0,editcolourselector);
    
    TWINdefaultgrowmode=TGdragy1+TGdragx1;
    UINT16 flags=BAnooffset|BAycentre|BAnobox|BMtoggleaction;
	
	texturepaths = new edittextbox(5,20,TBexternalstring ,T3D_TEXTURE_PATHS,1024,300,18);    
    tbutton *camlocktoggle= new tbutton(200,10,flags|(T3Dsceneattributes & sfCAMLOCK),CMDlockcam,"lock camera");
	valuedata data;
	fiunion stepsize;
	stepsize.ival=1;
	data.min=0;
	data.max=20000;
	data.val=float2int(T3Dcurrentscene()->fogminz);

    fogtoggle= new tbutton(5,10,flags|(T3Dsceneattributes & sfFOG),CMDtogfog,"Fog Enable");
    nearfog= new numbox(5,35,70,stepsize,CMDvalchanged,&data,0);
	data.val=float2int(T3Dcurrentscene()->fogmaxz);
    farfog= new numbox(5,70,70,stepsize,CMDvalchanged,&data,0);
	data.fmax=1;
	data.fval=T3Dcurrentscene()->fogdensity;
	stepsize.fval=0.01f;

    fogdensity= new numbox(5,105,45,stepsize,CMDvalchanged,&data,VTfloat);    
    fogcolour=new tbutton(5,140,40,20,BMtoggleaction,CMDeditfogcolour,13);
    TWINdefaultgrowmode=TGdragy2+TGdragx2;
	
    ok=new tbutton(b.x-60,b.y-30,45,25,BMreleaseaction|BAxcentre+BAycentre,0,(char)13,"Ok");
    cancel=new tbutton(b.x-60,b.y-30,45,25,BMreleaseaction|BAxcentre+BAycentre,0,(char)13,"Ok");
	paths->insertrel(texturepaths,10002);
	
	paths->addstring("Texture paths (comma seperated)",5,5,0,textcol);		

    sceneprefs->insertrel(fogtoggle);
    sceneprefs->insertrel(camlocktoggle);
    
    sceneprefs->insertrel(nearfog);
    sceneprefs->insertrel(farfog);
    sceneprefs->insertrel(fogdensity);
    sceneprefs->insertrel(fogcolour);
	sceneprefs->addbackview();
    
    tabs->addtabcontrol(sceneprefs,"Scene");
	tabs->addtabcontrol(paths,"Paths");

    insert(new backview(a.x,a.y,b.x,b.y,&TWINctrlbm.window_bg0));
    insert(tabs);
    insert(ok);
    insert(cancel);
    buildbuttonpalette(float2int(T3Dcurrentscene()->fogcolour.r*255.0f),
					   float2int(T3Dcurrentscene()->fogcolour.g*255.0f),
					   float2int(T3Dcurrentscene()->fogcolour.b*255.0f));
    TWINdefaultgrowmode=TGgrowall;    
}


void preferences::handleevent(tevent *event)
{
    tgroup::handleevent(event);
    bool redrawscene=false;
	valuedata data;
	data.min=0;
    if (event->what==EVcommand)
    {
        switch (event->command)
        {

        case CMDvalchanged:

          T3Dcurrentscene()->setfog((flt)nearfog->data.val,(flt)farfog->data.val,fogdensity->data.fval);
		  view->renderview->viewcam->FARZ=farfog->data.val+100;
		  data.val=float2int(nearfog->data.val);
		  data.max=float2int(farfog->data.val);
          nearfog->setrangemute(&data);
		  data.min=data.val;
		  data.max=0x0FFFFFFF;
		  data.val=float2int(farfog->data.val);
          farfog->setrangemute(&data);          
          //check for overlap
          redrawscene=true;
          
          break;
        case CMDeditfogcolour:          
          editcolourselector->mute();
          editcolourselector->setcolour(T3Dcurrentscene()->fogcolour.r*255,T3Dcurrentscene()->fogcolour.g*255,T3Dcurrentscene()->fogcolour.b*255);
          editcolourselector->unmute();
          colourcontainer->tmoveto(fogcolour->b.x,fogcolour->a.y);
          TWINgetpopuptarget()->insert(colourcontainer);
          colourcontainer->target=this;
          colourcontainer->drawview();
          TWINsetmodal(colourcontainer);                             
          break;
        case CMDcolourschanged:
          editcolourrgb=((colourselector*)event->infoptr)->col;
          buildbuttonpalette(editcolourrgb.r,editcolourrgb.g,editcolourrgb.b);          
          fogcolour->drawview();          
          T3Dsetbackgroundcolour(editcolourrgb.r,editcolourrgb.g,editcolourrgb.b);
          redrawscene=true;
          break;            
        case CMDtogfog:
          T3Dsceneattributes ^= sfFOG;
          redrawscene=true;
          break;
        case CMDcontainermodalended:
          colourcontainer->remove();
          if (fogcolour->isdown())
             fogcolour->setup();
          break;
        case CMDlockcam:
          view->renderview->lockcam^=1;break;       
        }
        
    }
    if (redrawscene)
    {
        view->renderview->calc3d();
        view->renderview->drawview();
    }       
}

////////////////////////////////////////////////////////////////////////////////////
//Material edit

materialedit::materialedit(INT16 x1,INT16 y1,INT16 x2,INT16 y2,material *mat_):tgroup(x1,y1,x2,y2)
{
    
    scene=new tscene();
    scene->setfog(3000,3000,1);

    editcolour=0;
    l=new tlight();
    l->rotate(-HALFPI,-HALFPI,-HALFPI);
    l->o.normalize();
    //l->o=vector(-1500,-1000,-500);
    
    l->ambient=trgbfloat(0.10f,0.10f,0.10f);
    l->diffuse=trgbfloat(0.7f,0.7f,0.7f);
    l->specular=trgbfloat(1.0f,1.0f,1.0f);
    
    editcolourselector=new colourselector(0,0,&editcolourrgb);
    colourcontainer=new bordercontainer(0,0,editcolourselector);
    
    TWINdefaultgrowmode=TGdragy1+TGdragx1;

    logmessage((void*)&textcol,"Creating viewer window");
    
    renderview=new scrollmaterialview(0,0,(teditscene*)scene);
    renderscroll=new scrollcontainer(10,10,109+TWINctrlbm.scroll.getbuttonwidth(),200,renderview,CFybar);

    mat=mat_;
    T3Dmaterial=NULL;
    T3Dtexture=NULL;
    T3Ddefaultpolyattributes=paENABLED;//paSHADED;//|paTRANSPARENT;
    objcount=0;
    T3Dpolycreationflags|=cfWRAPTEXTURE;    

    renderview->obj=new t3dmesh;    
    mksphere(renderview->obj,0,0,0,10,10,10,PI,30,30);    
    
    renderview->obj->calculatenormals();         
    T3Dmaterial=mat;
    scene->addobject(NULL,renderview->obj);
    scene->addobject(NULL,l);
	        
    renderview->obj->moveto(0,0,0);
    

    
    //logmessage((void*)&textcol,"Done");    
    //an error was occuring here, unknown reason

    tabs=new tabbox(a.x+140,a.y+10,b.x-3,a.y+200);    
    container *matc=new container(1);
    container *texc=new container(1);
    container *attribc=new container();

    textures=new dropbox(10,120,10,NULL,0);
    createtexturelist();
    
    UINT16 ypos=0;
    int i;
    trgbfloat *col=&mat->ambient;
    
    for (i=0;i<4;i++)
    {
       buildbuttonpalette(i,col->r*255.0f,col->g*255.0f,col->b*255.0f,false);
       
       colours[i]=new tbutton(5,ypos,40,20,BMtoggleaction,CMDeditambient+i,13);
       matc->insertrel(colours[i]);
       ypos+=35;
       col++;
    }
    ypos=0;
	fiunion stepsize;
	stepsize.ival=1;
	valuedata data;
	data.min=0;
	data.max=255;
	data.val=0;
	matc->addstring("Spec Power",65,ypos,0,TWINtextcol);
	specpower=new numbox(65,ypos+=35,55,stepsize,CMDvalchanged,&data,0);    
	data.fmax=1.0;
	stepsize.fval=0.01;
    transparency=new numbox(65,ypos+=35,55,stepsize,CMDvalchanged,&data,VTfloat);        
	specstr=new numbox(65,ypos+=35,55,stepsize,CMDvalchanged,&data,VTfloat);    
    
    
    ypos=4;    
    UINT16 flags=BAnooffset|BAycentre|BAnobox|BMtoggleaction;
    togsmooth = new tbutton(10,ypos,flags,CMDtogsmooth,"Smooth shaded");    
    togenvmap = new tbutton(10,ypos+=bheight,flags|(T3Ddefaultpolyattributes & paENVMAP),CMDtogenvmap,"Environment Mapped UV");    
    togdouble = new tbutton(10,ypos+=bheight,flags|(T3Ddefaultpolyattributes & paDOUBLESIDED),CMDtogdouble,"Double sided");    
    togtransparent= new tbutton(10,ypos+=bheight,flags|(T3Ddefaultpolyattributes & paTRANSPARENT),CMDtogtransparency,"Transparency");    
    tognospectrans = new tbutton(10,ypos+=bheight,flags|(T3Ddefaultpolyattributes & paNOSPECTRANS),CMDtognospectrans,"Transparent hightlights");    
    toginterpolate = new tbutton(10,ypos+=bheight,flags|(T3Ddefaultpolyattributes & paTEXTINTERPOLATED),CMDtoginterpolate,"Interpolated Textures");
    togwireframe = new tbutton(10,ypos+=bheight,flags|(T3Ddefaultpolyattributes & paOUTLINED),CMDtogwireframe,"Wire frame");
    togtruecolour = new tbutton(10,ypos+=bheight,flags|(T3Ddefaultpolyattributes & paTRUECOLOUR),CMDtogtruecolour ,"Solid colour");
	flags=BAycentre|BAxcentre|BMreleaseaction;
    applymaterial=new tbutton(170,135,60,20,flags,CMDapplymaterialtoselectedpolygons,13,"Apply");
    applymaterial->target=this;
    selectbymaterial=new tbutton(100,135,60,20,flags,CMDselectbymaterial,13,"Select");
    selectbymaterial->target=this;
    applytexture=new tbutton(170,135,60,20,flags,CMDapplytexturetoselectedpolygons,13,"Apply");
    applytexture->target=this;
    selectbytexture=new tbutton(100,135,60,20,flags,CMDselectbytexture,13,"Select");
    selectbytexture->target=this;    
    applyattribute=new tbutton(170,135,60,20,flags,CMDapplyattributetoselectedpolygons,13,"Apply");
    applyattribute->target=this;
    selectbyattribute=new tbutton(100,135,60,20,flags,CMDselectbyattribute,13,"Select");
    selectbyattribute->target=this;
    //applymaterial=new tbutton(100,130,60,20,NULL,0,BMclickaction,CMDapplymaterialtoselectedpolygons);  ,                
    insert(new backview(x1,y1,x2,y2,&TWINctrlbm.window_bg0));
    insertrel(renderscroll);
    objecttypes=new ttoolbar(a.y+10,renderscroll->b.y+2,(24*4),23,toolbm2,8,24,24);
    objecttypes->addbutton(TBAglobaltoggle ,1,CMDchangeobjectsphere,"Sphere");
    objecttypes->addbutton(TBAglobaltoggle ,3,CMDchangeobjectcube,"Cube");
    objecttypes->addbutton(TBAglobaltoggle ,6,CMDchangeobjectcylinder,"Cylinder");
    objecttypes->addbutton(TBAglobaltoggle ,5,CMDchangeobjectcone,"Cone");    
    
    insert(objecttypes);
    insert(tabs);
    transparency->setval(mat->transparency * 255);
    specpower->setval(mat->specpower);
    specstr->setval(mat->st*255);                
    
    matc->insertrel(transparency);
    matc->insertrel(specpower);
    matc->insertrel(specstr);
    matc->insertrel(applymaterial);
    matc->insertrel(selectbymaterial);
       
    texc->insertrel(textures);
    texc->insertrel(applytexture);
    texc->insertrel(new tbutton(170,105,60,20,BMreleaseaction|BAycentre|BAxcentre,CMDloadtexturedialog,13,"Load"));
    texc->insertrel(selectbytexture);
    
    attribc->insertrel(togsmooth);
    attribc->insertrel(togenvmap);
    attribc->insertrel(togdouble);
    attribc->insertrel(togtransparent);
    attribc->insertrel(tognospectrans);
    attribc->insertrel(toginterpolate);
    attribc->insertrel(togwireframe);
    attribc->insertrel(togtruecolour);
    attribc->insertrel(applyattribute);
    attribc->insertrel(selectbyattribute);
    attribc->addbackview();
    
    tabs->addtabcontrol(matc,"Material");
    tabs->addtabcontrol(texc,"Texture");
    tabs->addtabcontrol(attribc,"Attribute");
    renderview->calc3d(SVCMallvisible );

    TWINdefaultgrowmode=TGgrowall;
    logmessage((void*)&textcol,"creation of material dialog successful");
    
}

materialedit::~materialedit()
{
    delete scene;
    delete texturelist;
}

void materialedit::buildbuttonpalette(UINT32 ref,UINT32 r,UINT32 g,UINT32 b,bool setmat)
{
   UINT8 start=ref <<4;
   //COLgrad(buttonpalette,start,start+8,trgba(0,0,0,0),trgba(r,g,b,0));
   //COLgrad(buttonpalette,start+8,start+14,trgba(r,g,b,0),trgba(255,255,255,0));
   
   if (setmat)
   {
      trgbfloat *col=&mat->ambient;
      col[ref]=trgbfloat(r/255.0f,g/255.0f,b/255.0f);      
   }
}



void materialedit::createtexturelist()
{
    UINT32 i;
    textures->list->makelist(T3Dtexturebank.count+1,NULL,0);
    textureref *texref=(textureref*)T3Dtexturebank.first;
    textures->list->setitem(0,str_none,0,0,0,0);
    i=1;
    while (texref!=NULL)
    {
        textures->list->setitem(i++,texref->name,0,0,0,0);
        texref=texref->nextt();
    }
    textures->list->drawview();
}

void materialedit::calc3d(UINT8 mode)
{
    if (renderview)
    {
         renderview->calc3d(mode);
         renderview->drawview();
    }
}        

void materialedit::handleevent(tevent *event)
{
    trgbfloat *col;
    int i;
    UINT32 updatescene=0;
    tgroup::handleevent(event);
    if ((state & SFdeaf)==0)    
    if (event->what & EVcommand)
    {
        switch (event->command)
        {
            case CMDselectedchanged:
               T3Dsettexture(T3Dgettextureref(textures->list->sel));
               renderview->obj->settexture(T3Dtexture,1);
               calc3d(SVCMallvisible);
            case CMDmaterialchanged:
              deafen();
              mat=T3Dmaterial;
              col=&mat->ambient;
              for (i=0;i<4;i++)
              {
                 buildbuttonpalette(i,col->r*255.0f,col->g*255.0f,col->b*255.0f,false);
                 colours[i]->drawview();
                 col++;
              }              
              transparency->setval(mat->transparency * 255);
              specpower->setval(mat->specpower);
              specstr->setval(mat->st*255);
              undeafen();
              break;
            case CMDeditambient:                              
            case CMDeditdiffuse:               
            case CMDeditspecular:               
            case CMDeditemmisive:                              
               editcolour=event->command-CMDeditambient;
               if (colours[editcolour]->isdown())
               {
                   col=&mat->ambient;                              
                   editcolourselector->mute();
                   editcolourselector->setcolour(col[editcolour].r*255,col[editcolour].g*255,col[editcolour].b*255);
                   editcolourselector->unmute();
                   colourcontainer->tmoveto(colours[editcolour]->b.x,colours[editcolour]->a.y);
                   TWINgetpopuptarget()->insert(colourcontainer);
                   colourcontainer->target=this;
                   colourcontainer->drawview();
                   TWINsetmodal(colourcontainer);
                   for (int i=0;i<4;i++)                   
                       if ((i!=editcolour)&&(colours[i]->isdown()))                       
                           colours[i]->setup();                                                                  
                }
               break;            
            case CMDcontainermodalended:
               colourcontainer->remove();
               if (colours[editcolour]->isdown())
                   colours[editcolour]->setup();               
               break;
            case CMDcolourschanged:
               editcolourrgb=((colourselector*)event->infoptr)->col;
               buildbuttonpalette(editcolour,editcolourrgb.r,editcolourrgb.g,editcolourrgb.b,true);               
               colours[editcolour]->drawview();
               calc3d(SVCMselected);               
               break;
            case CMDnumboxreleased:
            case CMDcolourschangedrelease:
               updatescene=1;
               break;
            case CMDvalchanged:
               mat->specpower=specpower->data.val;
               mat->transparency=transparency->data.fval;
               mat->st=specstr->data.fval;
               calc3d(SVCMselected);
            break;
            case CMDapplymaterialtoselectedpolygons:T3Dcurrentscene()->setsp_material(T3Dmaterial);updatescene=1;  break;
            case CMDapplytexturetoselectedpolygons:T3Dcurrentscene()->setsp_texture(T3Dtexture);updatescene=1;   break;
            case CMDapplyattributetoselectedpolygons:T3Dcurrentscene()->setsp_attribute(T3Ddefaultpolyattributes,bPUT);updatescene=1;   break;
            case CMDselectbyattribute:T3Dcurrentscene()->selectelements(ETpolygon,T3Dcurrentscene()->selectioneffect,ofENABLED,T3Ddefaultpolyattributes,checkelembyattribute_and);updatescene=3;break;
            case CMDselectbytexture:T3Dcurrentscene()->selectelements(ETpolygon,T3Dcurrentscene()->selectioneffect,ofENABLED,(UINT32)T3Dtexture,checkpolygonbytexture);updatescene=3;break;
            case CMDselectbymaterial:T3Dcurrentscene()->selectelements(ETpolygon,T3Dcurrentscene()->selectioneffect,ofENABLED,(UINT32)T3Dmaterial,checkpolygonbymaterial);updatescene=3;break;
            case CMDchangeobjectsphere:
            case CMDchangeobjectcube:
            case CMDchangeobjectcylinder:
            case CMDchangeobjectcone:            
            objcount=event->command-CMDchangeobjectsphere;
            //t3dmesh needs updated remove procedures
            //obj->removevertex(0);
            renderview->obj->destroy();  
            T3Dmaterial=NULL;
            T3Dsettexture(T3Dgettextureref(textures->list->sel));
            
            switch(objcount)
            {
               case 0:T3Dpolycreationflags|=cfWRAPTEXTURE;mksphere(renderview->obj,0,0,0,10,10,10,0,30,30);break;
               case 1:T3Dpolycreationflags&=~cfWRAPTEXTURE;mkcube(renderview->obj,-5.5,-5.5,-5.5,5.5,5.5,5.5);break;
               case 2:T3Dpolycreationflags|=cfWRAPTEXTURE;mkcylinder(renderview->obj,0,0,-5,5,10,10,0,22);break;
               case 3:T3Dpolycreationflags|=cfWRAPTEXTURE;mkcone(renderview->obj,0,0,-5,5,10,10,0,22);break;
            }
            T3Dmaterial=mat;
            renderview->obj->calculatenormals();
            calc3d(SVCMallvisible);
            break;            
            
            default:
            if ((event->command>=CMDtogbase)&&(event->command<=CMDtogbase+4096))
            {
                T3Ddefaultpolyattributes^=(event->command-CMDtogbase);
                renderview->obj->setpolyattributes(T3Ddefaultpolyattributes,0);
                calc3d(SVCMallvisible);
            }
            break;
            case CMDloadtexturedialog:TWINgetpopuptarget()->add_and_draw(new window(0,20,380,260,"Load Texture",new fileview(0,0,&loadtexture),CFnoresize),VPcentre,PIDloadtexture);break;                    
        }
		if (view)
		{
        if (updatescene & 1)
        {
            view->renderview->calc3d();
            view->renderview->drawview();
        }
        if (updatescene & 2)
        {
            view->orthxy->calc3d();
            view->orthxy->drawview();
            view->orthxz->calc3d();
            view->orthxz->drawview();
            view->orthzy->calc3d();
            view->orthzy->drawview();            
        }
		}
        
    }        

}


leftbar::leftbar(UINT16 x,UINT16 y,UINT16 height):tgroup(x,y,x+95,y+height)
{
    functionbar=new ttoolbar(a.x,a.y+85,(24*4),(24*3)-1,toolbm,17,24,24);
    functionbar->addbutton(TBAtoggle+((T3Dcurrentscene()->grid->attribute & ofGRIDSNAP)!=0),16,CMDsnapgrid,"Grid Snap");
    functionbar->addbutton(TBAtoggle+((T3Dcurrentscene()->grid->attribute & ofVISIBLE)!=0),5,CMDviewgrid,"Show Grid");
    functionbar->addbutton(0,6,0,"Undo");  //undo
    functionbar->addbutton(0,7,0,"Redo");  //redo
    
    functionbar->addbutton(TBAtoggle ,8,CMDtogonlyshowselectednodes,"Show selected nodes");    
    functionbar->addbutton(TBAglobaltoggle ,9,0,NULL);
    functionbar->addbutton(TBAglobaltoggle ,8,0,NULL);    
    functionbar->addbutton(TBAtoggle+1,15,CMDlight1,NULL);
    functionbar->addbutton(TBAtoggle+1,15,CMDlight2,NULL);    
    functionbar->addbutton(0,14,CMDemergencysave,"Backup");
    functionbar->addbutton(0,13,CMDscreendump,"Capture screen");
    functionbar->addbutton(0,9,0,NULL);
    
    
    growmode=TGdragonx1 | TGdragony1 ;
    insert(functionbar);
	
}

void leftbar::handleevent(tevent *event)
{
    tgroup::handleevent(event);
    if (event->what==EVcommand)
    {
        owner->handleevent(event);
    }
}

void leftbar::draw()
{
    tgroup::draw();    
   
    outboxcol(a.x,a.y,b.x,functionbar->a.y-1);
    inboxcol(a.x,functionbar->b.y+1,b.x,b.y);//outbox inbox
    
}


tmenu *createmainmenu()
{
	tmenu *menu=new tmenu(0,0,MAxorient);
    tmenu *filemenu=new tmenu(0,0,MApopup);
    tmenu *editmenu=new tmenu(0,0,MApopup);
    tmenu *windowmenu=new tmenu(0,0,MApopup);
    tmenu *polymenu=new tmenu(0,0,MApopup);
    tmenu *subpolymenu=new tmenu(0,0,MApopup);
    tmenu *subselectmenu=new tmenu(0,0,MApopup);
	tmenu *subtriangulatemenu=new tmenu(0,0,MApopup);
	tmenu *smoothgroupmenu=new tmenu(0,0,MApopup);

    menu->additem("File",0,CMIAtitlemenustyle,filemenu,NULL,1);
    menu->additem("Edit",0,CMIAtitlemenustyle,editmenu,NULL,1);
    menu->additem("Selected",0,CMIAtitlemenustyle,polymenu,NULL,1);
    menu->additem("Window",0,CMIAtitlemenustyle,windowmenu,NULL,1);
    menu->b.x=SC.size.x-1;
    filemenu->additem("New",CMDnewscene,0,NULL,NULL,1);
    filemenu->additem("Open..",CMDload3dobjectdialog,0,NULL,NULL,1);
    filemenu->additem("Save",CMDsave3dobjectdialog,0,NULL,NULL,1);
    filemenu->additem("Save as..",CMDsave3dobjectdialog,0,NULL,NULL,2);
    filemenu->additem("Merge..",CMDappend3dobjectdialog,0,NULL,NULL,1);
    filemenu->additem(NULL,0,MIAseperator);	
    filemenu->additem(exitstring,CMDleave,0,NULL,NULL,2);    
    editmenu->additem("Copy",0,MIAdisabled);
    editmenu->additem("Paste",0,MIAdisabled);
    windowmenu->additem("Tile",0,MIAdisabled);
    windowmenu->additem("Cascade",0,MIAdisabled);
    windowmenu->additem("Close",0,MIAdisabled);
    windowmenu->additem("Close all",0,MIAdisabled);
    windowmenu->additem("Previous (F6)",0,MIAdisabled);
    windowmenu->additem("Next (F7)",0,MIAdisabled);
    subpolymenu->additem("Double sided ",0);
    subpolymenu->additem("Transparency ",0);
    subpolymenu->additem("Outlined ",0);
    subpolymenu->additem("Specular trans ",0);    

	subtriangulatemenu->additem("Left",CMDtriangulateright);
	subtriangulatemenu->additem("Right",CMDtriangulateleft);
	subtriangulatemenu->additem("By Normal",CMDtriangulatebynormal);

	smoothgroupmenu->additem("0",CMDsetsmooth+0);
	smoothgroupmenu->additem("1",CMDsetsmooth+1);
	smoothgroupmenu->additem("2",CMDsetsmooth+2);
	smoothgroupmenu->additem("3",CMDsetsmooth+3);
	smoothgroupmenu->additem("4",CMDsetsmooth+4);
	smoothgroupmenu->additem("5",CMDsetsmooth+5);
    smoothgroupmenu->additem("6",CMDsetsmooth+6);
	smoothgroupmenu->additem("7",CMDsetsmooth+7);
	smoothgroupmenu->additem("8",CMDsetsmooth+8);
	smoothgroupmenu->additem("9",CMDsetsmooth+9);
	smoothgroupmenu->additem("10",CMDsetsmooth+10);
	smoothgroupmenu->additem("11",CMDsetsmooth+11);
	smoothgroupmenu->additem("12",CMDsetsmooth+12);
	smoothgroupmenu->additem("13",CMDsetsmooth+13);
	smoothgroupmenu->additem("14",CMDsetsmooth+14);
	smoothgroupmenu->additem("15",CMDsetsmooth+15);
    smoothgroupmenu->additem("16",CMDsetsmooth+16);
	
    
    polymenu->additem("Delete",CMDdeleteselected);
    polymenu->additem("Set attribute ",0,0,subpolymenu);
    polymenu->additem("Select by...  ",0,0,subselectmenu);
    polymenu->additem("Windowize",0);
    polymenu->additem("Merge",CMDmergeselectedvertices);
    polymenu->additem("Flip normals",CMDflipnormals);
    polymenu->additem("Poly divide",CMDmeshdivide);
    polymenu->additem("Edge split",CMDedgesplit );
    polymenu->additem("Triangulate",0,0,subtriangulatemenu);
	polymenu->additem("Shrink Selection",CMDshrinkselection);
    polymenu->additem("Grow Selection",CMDgrowselection);	
    polymenu->additem("Look at Selection",CMDlookatselection);    
	polymenu->additem("Build Octtree",CMDbuildocttree);    
	polymenu->additem("Set Smooth Group",0,0,smoothgroupmenu );

    subselectmenu->additem("Non-coplainarity  ",CMDselectnoncoplainar);
    subselectmenu->additem("Current Material  ",CMDselectbymaterial);
    subselectmenu->additem("Current Texture   ",CMDselectbytexture);
    subselectmenu->additem("Current Attribute ",CMDselectbyattribute);   
	return menu;
}



clay::clay(INT16 x,INT16 y,INT16 x2,INT16 y2):tgroup(x,y,x2,y2)
{
	char exitstring[]="Exit..";
    INT16 xpos;
    trect desktop;
    tpoint halfpoint;
    tpoint windowsize;
	
	fiunion stepsize;
	stepsize.ival=1;
	valuedata data;
	data.min=3;
	data.max=100;


    insert(new backview(96,y,x2,y2,getcolour2(5,30,100)));       
    
    selipsevals.xsegs=selipsevals.ysegs=spherevals.xsegs=spherevals.ysegs=circlevals.segs=conevals.xsegs=cylindervals.xsegs=gridvals.xsegs=gridvals.ysegs=20;   
    selipsevals.p1=selipsevals.p2=1;
	menu=createmainmenu();
    //drawing functions tab boxes
    drawfuncs=new tabbox(96,menu->b.y+1,SC.size.x-2,menu->b.y+95);
	drawfuncs->growmode=growmode=TGdragonx1 | TGdragony1 |TGgrowx2 ;
    
    drawfuncs->addtabcontrol(selectctrl=new container(0),"Select",CMDselect);
	drawfuncs->addtabcontrol(meshctrl=new container(0),"Meshes",CMDcreatemesh);
	drawfuncs->addtabcontrol(splinectrl=new container(0),"Splines",CMDcreatespline);
	drawfuncs->addtabcontrol(metaballctrl=new container(0),"Metaballs",CMDcreatemetaballs);
	drawfuncs->addtabcontrol(uvctrl=new container(0),"UV application",CMDcreateuv);

	trect subrect;
	subrect.rassign(160,2,drawfuncs->b.x-2,drawfuncs->b.y-2);
	currentselectcontainer=vertexselect=new subcontainer(&subrect,0);
	polyselect=new subcontainer(&subrect,0);
	edgeselect=new subcontainer(&subrect,0);
	objectselect=new subcontainer(&subrect,0);
	subrect.rassign(70,2,drawfuncs->b.x-2,drawfuncs->b.y-2);
	currentmeshcontainer=cubectrl=new subcontainer(&subrect,0);
	squarectrl=new subcontainer(&subrect,0);
	spherectrl=new subcontainer(&subrect,0);
	circlectrl=new subcontainer(&subrect,0);
	polyctrl=new subcontainer(&subrect,0);	
	selipsectrl=new subcontainer(&subrect,0);
	cylinderctrl=new subcontainer(&subrect,0);
    conectrl=new subcontainer(&subrect,0);    
    textctrl=new subcontainer(&subrect,0);
	gridctrl=new subcontainer(&subrect,0); //do the same thing in square
    
    numbox* newbox;
    insertrel(menu);    
    
    insertrel(drawfuncs);

	TWINdefaultgrowmode=TGdragonx1| TGdragony1;

   	
    xpos=10;
	meshmode=new droppalette(xpos,15,objecticons,19,36,36);	
	meshmode->addbutton(TBAglobaltoggle+TBAaltselect,0,CMDcube,"Cube");
	meshmode->addbutton(TBAglobaltoggle+TBAaltselect,1,CMDsphere,"Sphere");
	meshmode->addbutton(TBAglobaltoggle+TBAaltselect,2,CMDcone,"Cone");
	meshmode->addbutton(TBAglobaltoggle+TBAaltselect,3,CMDcylinder,"Cylinder");
	meshmode->addbutton(TBAglobaltoggle+TBAaltselect,8,CMDselipse,"Super Elipse");
	meshmode->addbutton(TBAglobaltoggle+TBAaltselect,4,CMDsquare,"Square");
	meshmode->addbutton(TBAglobaltoggle+TBAaltselect,5,CMDcircle,"Circle");
	meshmode->addbutton(TBAglobaltoggle+TBAaltselect,6,CMDcreatepolygon,"Polygon");
	meshmode->addbutton(TBAglobaltoggle+TBAaltselect,7,CMDtext,"Text");
	meshmode->setsel(0);

	selectionleveltb=new droppalette(xpos,15,objecticons,19,36,36);	
	selectionleveltb->addbutton(TBAglobaltoggle+TBAaltselect,15,CMDvertex,"Vertex");
	selectionleveltb->addbutton(TBAglobaltoggle+TBAaltselect,16,CMDedge,"Edge");
	selectionleveltb->addbutton(TBAglobaltoggle+TBAaltselect,17,CMDpolygon,"Polygon");
	selectionleveltb->addbutton(TBAglobaltoggle+TBAaltselect,18,CMDobject,"Object");	
	selectionleveltb->setsel(0);

	xpos+=40;

	selectionfunctions=new droppalette(xpos,15,objecticons,19,36,36);
	selectionfunctions->addbutton(TBAglobaltoggle+TBAaltselect,9,CMDstretch,"Translate/Stretch");
	selectionfunctions->addbutton(TBAglobaltoggle+TBAaltselect,10,CMDrotate,"Rotate");
	selectionfunctions->addbutton(TBAglobaltoggle+TBAaltselect,0,CMDscale,"Uniform Scale");
	selectionfunctions->addbutton(TBAglobaltoggle+TBAaltselect,11,CMDshear,"Shear");	
	selectionfunctions->addbutton(TBAglobaltoggle+TBAaltselect,0,CMDtaper,"Taper");
	selectionfunctions->addbutton(TBAglobaltoggle+TBAaltselect,14,CMDtwist,"Twist");		
	selectionfunctions->addbutton(TBAglobaltoggle+TBAaltselect,0,CMDaxisrotate,"Rotate around Axis");
	selectionfunctions->addbutton(TBAglobaltoggle+TBAaltselect,0,CMDaxistranslate,"Translate along Axis");	
	selectionfunctions->addbutton(TBAglobaltoggle+TBAaltselect,0,CMDaxisscale,"Scale by Axis");
	
	selectionfunctions->setsel(0);

	UINT32 len;    
    xpos+=40;

    len=findlongesttext(T3Dselectioneffects,5)+8+TWINctrlbm.dropbox.getbuttonwidth();
    selectioneffect=new dropbox(xpos,xpos+len,15,(listelem*)T3Dselectioneffects,5);
    xpos+=len+5;
    //"Put text in here"
    
	selectctrl->insertrel(selectionleveltb);
	selectctrl->insertrel(selectionfunctions);
	selectctrl->insertrel(currentselectcontainer);
	selectctrl->insertrel(selectioneffect);
	selectctrl->addbackview();
    //selectctrl->insertrel(selectionfunction);
    
	xpos=10;
    vertexselect->insertrel(new tbutton(xpos,10,BMtogglebutton,CMDtoggleselectcopy,"Copy"));    
    vertexselect->insertrel(new tbutton(xpos,25,BMtogglebutton,CMDtoggleselectextrude,"Extrude"));
    vertexselect->insertrel(new tbutton(xpos,40,BMtogglebutton,CMDtoggleselectkeeppolys,"Keep polys"));
    xpos+=105;    
	data.seti(1,1,100);
    vertexselect->insertrel(new numbox(xpos,25,45,stepsize,CMDnumcopieschanged,&data,0));
    vertexselect->insertrel(bextruding=new tbutton(xpos+55,10,60,20,BMtoggleaction|BAxcentre|BAycentre,CMDtoggleextrusionmode,13,"Extrude"));    
	data.setf(0,-10000000,10000000);
    vertexselect->insertrel(numbericextrude=new numbox(xpos+55,35,45,stepsize,CMDnumericalextrusion,&data,VTfloat));   

	

	meshctrl->insertrel(meshmode);
	meshctrl->insertrel(currentmeshcontainer);
	meshctrl->addbackview();
	meshctrl->addstring("Type:",9,0,0,TWINtextcol);
	meshlabel=meshctrl->addstring(meshmode->getseltext(),9,52,0,TWINtextcol);

	data.seti(1,1,100);

	cubectrl->addbackview();
	textctrl->insertrel(new edittextbox(20,25,0,EXE_PATH,512,200,18));
    textctrl->addbackview();    
    polyctrl->addbackview();

	data.seti(20,3,100);

    circlectrl->insertrel(new numbox(20,25,45,stepsize,CMDcircvalchanged,&data,0));
	circlectrl->addstring("Sides",20,10,0,TWINtextcol);        
    circlectrl->addbackview();

    spherectrl->insertrel(new numbox(20,25,45,stepsize,CMDspherexvalchanged,&data,0));
    spherectrl->insertrel(new numbox(80,25,45,stepsize,CMDsphereyvalchanged,&data,0));
    spherectrl->addbackview();
	spherectrl->addstring("X Sides",20,10,0,TWINtextcol);    
	spherectrl->addstring("Y Sides",80,10,0,TWINtextcol);    
	
    cylinderctrl->insertrel(new numbox(20,25,45,stepsize,CMDcylindervalchanged,&data,0));
	cylinderctrl->insertrel(new numbox(80,25,45,stepsize,CMDcylindervalchanged,&data,0));
    cylinderctrl->addbackview();
	cylinderctrl->addstring("X Sides",20,10,0,TWINtextcol);
	cylinderctrl->addstring("Y Sides",80,10,0,TWINtextcol);

    conectrl->insertrel(new numbox(20,25,45,stepsize,CMDconevalchanged,&data,0));
	conectrl->addstring("Sides",20,10,0,TWINtextcol);    
    conectrl->addbackview();

	data.seti(1,1,100);
	
    squarectrl->insertrel(new numbox(20,25,45,stepsize,CMDgridxvalchanged,&data,0));
    squarectrl->insertrel(new numbox(80,25,45,stepsize,CMDgridyvalchanged,&data,0));
    squarectrl->insertrel(new tbutton(150,25,BMtogglebutton|T3Dcheckergrid,CMDtoggridchecker,"Checkered"));
    squarectrl->addbackview();
	squarectrl->addstring("X Sides",20,10,0,TWINtextcol);
	squarectrl->addstring("Y Sides",80,10,0,TWINtextcol);

	data.seti(10,3,100);
    selipsectrl->insertrel(new numbox(20,25,45,stepsize,CMDselipsexvalchanged,&data,0));
    selipsectrl->insertrel(new numbox(80,25,45,stepsize,CMDselipseyvalchanged,&data,0));


	stepsize.fval=0.01;		
	data.setf(0.5,0,4.0);
	
    selipsectrl->insertrel(new numbox(140,25,55,stepsize,CMDselipsep1changed,&data,VTfloat));
    selipsectrl->insertrel(new numbox(204,25,55,stepsize,CMDselipsep2changed,&data,VTfloat));
    selipsectrl->addbackview();
	selipsectrl->addstring("X Sides",20,10,0,TWINtextcol);
	selipsectrl->addstring("Y Sides",80,10,0,TWINtextcol);
	selipsectrl->addstring("Theta",140,10,0,TWINtextcol);
	selipsectrl->addstring("Omega",210,10,0,TWINtextcol); 


	metaballctrl->addbackview();
	splinectrl->addbackview();
	uvctrl->addbackview();
    
	TWINdefaultgrowmode=TGgrowall;
    
    insert(new leftbar(0,menu->b.y+1,y2));      
    desktop.rassign(drawfuncs->a.x+2,drawfuncs->b.y+2,SC.size.x-2,SC.size.y-2);
    halfpoint.assign(desktop.a.x+desktop.halfx(),desktop.a.y+desktop.halfy());
    windowsize.assign(desktop.halfx()-2,desktop.halfy()-2);

    messages=new logmessagebox(0,0,1,1,&T3Dmessages);
    //insertrel(new window(desktop.a.x,desktop.a.y,windowsize.x,windowsize.y,"Log",messages,CFclientedge|CFybar));    
    orthxy=orthxz=orthzy=NULL;
	renderview=NULL;
    orthxy=new edit3d(0,0,SC.size.x,SC.size.y,T3Dcurrentscene(),camORTHXY,NULL);
    orthxz=new edit3d(0,0,SC.size.x,SC.size.y,T3Dcurrentscene(),camORTHXZ,NULL);
    orthzy=new edit3d(0,0,SC.size.x,SC.size.y,T3Dcurrentscene(),camORTHZY,NULL);    
    renderview=new view3d(0,0,SC.size.x,SC.size.y,T3Dcurrentscene(),NULL);
    renderview->viewcam->setname("3d camera");
    renderview->viewcam->moveto(0,0,-100);
    T3Dcurrentscene()->addobject(NULL,renderview->viewcam);		
    renderview->viewcam->attribute &= ~ofEDITABLE;
	
    //renderview->viewcam->camtype=camORTHXY;
	
    container3d=new edit3dcontainer(0,0,400,400);	
	container3d->setviews(orthxy,orthzy,orthxz,renderview);
	containerwin=new window(100,100,500,500,"The Scene",container3d,0);
	orthxy->target=this;
	orthxz->target=this;
	orthzy->target=this;
	renderview->target=this;
    
    insert(containerwin);
	
	/*window *windowof3d;
    if (orthxy)
    {
       windowof3d=new window(desktop.a.x,desktop.a.y,windowsize.x,windowsize.y,"XYview",orthxy,CFclientedge|CFxbar|CFybar);
       insertrel(windowof3d);
    }

    if (orthzy)
    {                        
       windowof3d=new window(halfpoint.x,desktop.a.y,windowsize.x,windowsize.y,"ZYview",orthzy,CFclientedge|CFxbar|CFybar);
       insertrel(windowof3d);
    }
    if (orthxz)
    {
       windowof3d=new window(desktop.a.x,halfpoint.y,windowsize.x,windowsize.y,"XZview",orthxz,CFclientedge|CFxbar|CFybar);
       insertrel(windowof3d);
    }
    if (renderview)
    {
       //renderview->bg=getcolour2(0,0,30);
       windowof3d=new window(halfpoint.x,halfpoint.y,windowsize.x,windowsize.y,"3d view",renderview,CFclientedge|CFxbar|CFybar);
       insertrel(windowof3d);
    } */
	
    logmessage((void*)&textcol,"Created rendering window");    
    
}

UINT32 clay::deleteselected()
{    
      
    UINT32 ret=T3Dcurrentscene()->deleteselected(T3Dcurrentscene()->selectionmode);
    if (T3Dcurrentscene()->selectionmode==ETobject)
      treeview->clearsel();
    return ret;
}

UINT32 clay::removeobjects(UINT32 typemask,UINT32 attributemask)
{
    //tobject3d *obj=(tobject3d*)treeview->currsel;
    //if ((obj->type & typemask)&&(obj->attribute & attributemask))
    
    T3Dcurrentscene()->removeobjects(typemask,attributemask);
    treeview->clearsel();
    return 1;
}

void clay::renderall()
{
   renderview->calc3d();
   renderview->drawview();
  // if (changed==2)
  // {
       orthxy->calc3d();
       orthxy->drawview();
       orthzy->calc3d();
       orthzy->drawview();
       orthxz->calc3d();
       orthxz->drawview();
  // }
}

void clay::updateuielements()
{
	teditscene *scene=T3Dcurrentscene();
    sendcommand(CMDvalchanged,prefs,NULL);
    prefs->nearfog->setval(float2int(scene->fogminz));
    prefs->farfog->setval(float2int(scene->fogmaxz));
    prefs->buildbuttonpalette(float2int(scene->fogcolour.r*255.0f),
							  float2int(scene->fogcolour.g*255.0f),
							  float2int(scene->fogcolour.b*255.0f));
    T3Dsetbackgroundcolour(float2int(scene->fogcolour.r*255.0f),
						   float2int(scene->fogcolour.g*255.0f),
						   float2int(scene->fogcolour.b*255.0f));
    //sendcommand(CMDcolourschanged,prefs,NULL);
    /*setbit(camlocktoggle)
    setbit(fogtoggle->dstate )*/
}


char *getfilename(char *path,char *dest)
{
   char dummy[128];
   _splitpath(path, dummy,dummy,dest,dummy);
   return dest;
}

//handle the convertion from CMD id's to standard enumerations
//

void clay::setselectfunction(UINT32 mode)
{
	T3Dcurrentscene()->setselectfunction(mode-CMDBASEselectionfunction);
	//set selection function subcontainer stuff here
}

void clay::setselectmode(UINT32 mode )
{	
	T3Dcurrentscene()->setselectmode(mode-CMDBASEselectionlevel);
	//change selection subcontainer here
	subcontainer* newcontainer=getselectcontainer();
	selectctrl->swapinto(newcontainer,currentselectcontainer);	    
	currentselectcontainer=newcontainer;
	currentselectcontainer->drawview();
}

void clay::setdrawmode(UINT32 mode)
{
	T3Dcurrentscene()->setdrawmode(mode-CMDBASEdrawmode);		
}


void clay::setmeshdrawmode(UINT32 mode)
{
	T3Dcurrentscene()->setmeshdrawmode(mode-CMDBASEmeshdrawmode);
	//change mesh creation subcontainer here
	subcontainer* newcontainer=getmeshcontainer();
	meshctrl->swapinto(newcontainer,currentmeshcontainer);	    
	meshctrl->setlabel(meshmode->getseltext(),meshlabel);
		
	currentmeshcontainer=newcontainer;
	currentmeshcontainer->drawview();
}



void clay::handleevent(tevent *event)
{
    fileviewstruct *fvs=&merge3dobject;
    tsurface *tex=NULL;
	teditscene *scene=T3Dcurrentscene();
    UINT8 changed=0;
    flt infoflt;   
    if (event->what&EVcommand)
    {
        INT32 ival=*(UINT32*)event->infoptr;
		switch (event->command)
        {
		//redrawing commands		

		case CMDscenealtered:changed=3;break;
        case CMDscenealteredorth:changed=2;break;
        case CMDorthoscalechanged:
			infoflt=*(flt*)event->infoptr;          
			orthxy->setscaleoffsets(T3Dscale,infoflt);
			orthxz->setscaleoffsets(T3Dscale,infoflt);
			orthzy->setscaleoffsets(T3Dscale,infoflt);
			T3Dscale=infoflt;
			changed=2;
			break;
        case CMDcalc3d:           
			orthxy->calc3d();
			orthxz->calc3d();
			orthzy->calc3d();           
			break;
        case CMDundraw:
			ival=((tobject3d*)event->infoptr)->attribute;
			((tobject3d*)event->infoptr)->attribute&=~ofENABLED;           
			orthxy->calc3d();
			orthxz->calc3d();
			orthzy->calc3d();
			((tobject3d*)event->infoptr)->attribute=ival;           
			break;     
			
		//selection commands		
		case CMDcurrentelementchanged:
			treeview->setsel((ttreenode*)scene->curr.object);
			break;
		case CMDselectedchanged:
			if (scene->curr.object)
				scene->curr.object->attribute &=~ofSELECTED;                    
			scene->curr.object= (tobject3d*)treeview->currsel;
			if (scene->curr.object)
				scene->curr.object->attribute |=ofSELECTED;                    			
			break;
		case CMDdropboxclosed:
			if (event->infoptr==selectioneffect)
				scene->selectioneffect=selectioneffect->getselcontextattrib();
			break;
		case CMDvertex:
		case CMDedge:
		case CMDpolygon:
		case CMDobject:
			setselectmode(event->command);changed=3;			
			break;
		case CMDstretch:		 
		case CMDrotate:		 
		case CMDscale:		 		 
		case CMDshear:		 			
		case CMDtaper:		 		 
		case CMDtwist:		 		 
		case CMDaxisrotate:		 	 
		case CMDaxistranslate:		  
		case CMDaxisscale:		 	 
			setselectfunction(event->command);changed=3;
		case CMDtoggleextrusionmode:
			//if (scene->extrusioncount)
			if (bextruding->isdown())
			{
				scene->startextrusion();
				scene->prepareforpervertextransformation();              
				scene->applynumericextrusionmatrix();
				scene->transformextrusion();
				scene->calculatenormals(ofAFFECTEDBYSELECTION,paEXTRUSION,naEXTRUSION);        
				changed=3;				
			}else
			{
				scene->clearextrusion();
			}
			break;
		case CMDnumericalextrusion:
			if (scene->extrusioncount)
			{
				scene->extrusionlength=ival / 10.0f;
				scene->applynumericextrusionmatrix();
				scene->transformextrusion();
				scene->calculatenormals(ofAFFECTEDBYSELECTION,paEXTRUSION,naEXTRUSION);          
				changed=3;
			}
			break;
			//copy mode attribute events        
		case CMDtoggleselectcopy:T3Dcopyattributes ^=STRNScopy;break;
		case CMDtoggleselectextrude:T3Dcopyattributes ^=STRNSextrude;break;
		case CMDtoggleselectkeeppolys:T3Dcopyattributes ^=STRNSkeepoldpolys;scene->applyextrusionattributes();changed=3;break;
		case CMDnumcopieschanged:T3Dnumcopies=ival;break;		           					

		//commands to change the selection		
        case CMDselectbyattribute:scene->selectelements(ETpolygon,scene->selectioneffect,ofENABLED,T3Ddefaultpolyattributes,checkelembyattribute_and);changed=3;break;
        case CMDselectbytexture:scene->selectelements(ETpolygon,scene->selectioneffect,ofENABLED,(UINT32)T3Dtexture,checkpolygonbytexture);changed=3;break;
        case CMDselectbymaterial:scene->selectelements(ETpolygon,scene->selectioneffect,ofENABLED,(UINT32)T3Dmaterial,checkpolygonbymaterial);changed=3;break;
        case CMDselectnoncoplainar:scene->selectelements(ETpolygon,scene->selectioneffect,ofENABLED,(UINT32)T3Dmaterial,checkpolygonbynoncoplainarity);changed=3;break;        
		case CMDshrinkselection:scene->selectelements(ETpolygon,bNOT,ofAFFECTEDBYSELECTION,eaSELECTED,checkpolygonifedge);changed=3;break;
        case CMDgrowselection: scene->selectelements(ETpolygon,scene->selectioneffect,ofAFFECTEDBYSELECTION,paAFFECTEDBYSELECTION,checkelembyattribute_and);changed=3;break;        

		//commands that act upon the current selection
		case CMDmergeselectedvertices:scene->mergeselectedvertices(1);changed=3;break;
        case CMDmeshdivide:scene->meshdivide();changed=3;break;
        case CMDedgesplit:scene->edgesplit();changed=3;break;        
        case CMDtriangulateleft:scene->triangulate(0);changed=3;break;
		case CMDtriangulateright:scene->triangulate(1);changed=3;break;
		case CMDtriangulatebynormal:scene->triangulate(2);changed=3;break;
        case CMDflipnormals:scene->flipselectedpolygons();changed=3;break;
        case CMDdeleteselected:if (deleteselected()) changed=3;break;		
		//mesh object commands

		case CMDtoggridchecker:T3Dcheckergrid^=TRUE;break;
		case CMDcircvalchanged:circlevals.segs=ival;break;
		case CMDspherexvalchanged:spherevals.xsegs=ival;break;
		case CMDsphereyvalchanged:spherevals.ysegs=ival;break;
		case CMDselipsexvalchanged:selipsevals.xsegs=ival;break;
		case CMDselipseyvalchanged:selipsevals.ysegs=ival;break;
		case CMDselipsep1changed:selipsevals.p1=*(flt*)&ival;break;
		case CMDselipsep2changed:selipsevals.p2=*(flt*)&ival;break;
		case CMDgridxvalchanged:gridvals.xsegs=ival;break;
		case CMDgridyvalchanged:gridvals.ysegs=ival;break;
		case CMDcylindervalchanged:cylindervals.xsegs=ival;break;
		case CMDconevalchanged:conevals.xsegs=ival;break;
		case CMDline:
		case CMDdot:
		case CMDcube:
		case CMDcylinder:
		case CMDsphere:
		case CMDgrid:
		case CMDtext:
		case CMDcreatepolygon:
		case CMDcone:
		case CMDsquare:
		case CMDcircle:
		case CMDselipse:
			setmeshdrawmode(event->command);
			break;

		//object creation/removal notices
		case CMDobjectcreated:
			treeview->update();
			break;
        case CMDobjectdeleted:            
            if (treeview->currsel==event->infoptr)
				treeview->setsel(NULL);
            break;
		//misc commands
				
        case CMDleave:PostQuitMessage(0);break;        
        case CMDscreendump:
			writeraw("\\dump.raw",SC.size.x,SC.size.y,(UINT16*)SC.scrptr);break;
			
		case CMDbuildocttree :
			if ((scene->curr.object)&&(scene->curr.object->type==otGEOM))
		 	    T3Docttree->build((t3dmesh*)scene->curr.object);						
			break;
        case CMDlookatselection:
			scene->projectselectionxy();
			changed=3;break;
        case CMDlight1:plight->attribute^=ofENABLED;changed=1;break;
        case CMDlight2:plight2->attribute^=ofENABLED;changed=1;break;
        case CMDlight3:plight3->attribute^=ofENABLED;changed=1;break;
        case CMDlight4:plight4->attribute^=ofENABLED;changed=1;break;

		//tabbar response
		case CMDtabchanged:setdrawmode(((tabbox*)event->infotview)->getseltabviewid());			
			break;
		//left toolbar functions
			
        case CMDsnapgrid:scene->grid->attribute^=ofGRIDSNAP;break;
        case CMDviewgrid:scene->grid->attribute^=ofVISIBLE;changed=3;break;
        case CMDrendertype:renderview->rendertype^=1;break;
        case CMDtogspecular:T3Dsceneattributes^=sfSPECULARHIGHLIGHTS;changed=1;break;
        case CMDtogfog:T3Dsceneattributes^=sfFOG;changed=1;break;        
        case CMDtogonlyshowselectednodes:T3Dsceneattributes^=sfONLYSHOWSELECTEDNODES;changed=2;break;

		//Scene management, loading and saving of scenes and scene elements
			                
        case CMDnewscene:           
			removeobjects(0xFFFFFFFF,ofEDITABLE);
			scene->clearextrusion();
			scene->clearselection();
			addstandardlights(scene);
			treeview->update();
			changed=3;break;                
        case CMDappend3dobjectdialog:add_and_draw(new window(0,20,380,260,"Merge 3d object into scene",new fileview(0,0,&merge3dobject),CFnoresize),VPcentre,PIDsave3dobject);break;
        case CMDsave3dobjectdialog:add_and_draw(new window(0,20,380,260,"Save 3d Object",new fileview(0,0,&save3dobject),CFnoresize),VPcentre,PIDsave3dobject);break;
        case CMDload3dobjectdialog:add_and_draw(new window(0,20,380,260,"Load 3d Object",new fileview(0,0,&load3dobject),CFnoresize),VPcentre,PIDload3dobject);break;
        case CMDemergencysave:
			T3Dsavenewclay("backup.3d",scene);
			logmessage((void*)&textcol,"Saved Clayworks backup file:");
			break;
        case CMDsave3dobject:									
			switch (save3dobject.filterindex)
			{                                  
				case 0:T3Dsavenewclay(save3dobject.fullpath,scene);
					logmessage((void*)&textcol,"Saved Clayworks file %s:",save3dobject.fullpath);
					break;
				case 1:T3Dsavexeios(save3dobject.fullpath,scene);
					logmessage((void*)&textcol,"Saved Xeios Scene file %s:",save3dobject.fullpath);                 
					break;
				case 2:T3Dsavejava(save3dobject.fullpath,scene);
					logmessage((void*)&textcol,"Saved ClayJava source file %s:",save3dobject.fullpath);
					break;
			}			
			break;
		case CMDloadtexture:
			//maybe put an image type 'suggestion' in here, based on the result of the filter index
			tex=(tsurface*)readimage(TWINrendercontext,loadtexture.fullpath,PXFrgb565);
			char tempbuf[8];
			if (tex)
				T3Daddtexture(tex,getname(loadtexture.fullpath));                                     
			break;
					
		case CMDload3dobject:
			removeobjects(0xFFFFFFFF,ofEDITABLE);
			fvs=&load3dobject;
		case CMDappend3dobject:
			t3dmesh *obj=NULL;           
			scene->clearextrusion();
			scene->clearselection();
			bool success=false;
				
			switch (fvs->filterindex)
			{
				case 0:success=T3Dloadnewclay(fvs->fullpath,scene);break;                                 
				case 1:success=T3Dloadmr(fvs->fullpath,scene);break;
				case 2:success=T3Dloadlw(fvs->fullpath,scene);break;
				case 3:success=T3Dloadmd3(fvs->fullpath,scene);break;
				case 4:success=T3Dload3ds(fvs->fullpath,scene);break;
			}
			if (!success)		      
			{
				removeobjects(0xFFFFFFFF,ofEDITABLE);
				scene->clearextrusion();
				scene->clearselection();
				addstandardlights(scene);				
			}
			
			treeview->update();               
			changed=3;           
			if (scene->tlights.count==0)
				addstandardlights(scene);	  
			updateuielements();     		   
			break;          
        }
        
        event->what=0;
    }else if (event->what&EVkeyboard)
    {
        switch (charcode)
        {
		case '+':
			scene->selectelements(ETpolygon,scene->selectioneffect,ofAFFECTEDBYSELECTION,paAFFECTEDBYSELECTION,checkelembyattribute_and);
			changed=3;
			break;
		case '-':
			scene->selectelements(ETpolygon,bNOT,ofAFFECTEDBYSELECTION,eaSELECTED,checkpolygonifedge);
			changed=3;
			break;
		case 0:
			switch (scancode)
			{
			case delkey:
				if (deleteselected())changed=3;             
				break;
			}
			break;
        }
        event->what=0;
    } 
	
    /*if(*scan & SMleftaltkey))
    {
	//handle shortcuts in here        
	menu->handleevent(event);        
	tsoundoff();
    }*/
    if (changed)
	{
		if ((changed&1)&&(renderview)&&(renderview->isvisible()))
		{
			renderview->calc3d();
			renderview->drawview();
		}            
		if (changed&2)
		{
			if ((orthxy)&&(orthxy->isvisible()))
			{                            
				orthxy->calc3d();
				orthxy->drawview();
			}
			if ((orthzy)&&(orthzy->isvisible()))
			{                                           
				orthzy->calc3d();
				orthzy->drawview();
			}
			if ((orthxz)&&(orthxz->isvisible()))
			{                                           
				orthxz->calc3d();
				orthxz->drawview();
			}
		}           
	}
	
	
	
    if (event->what!=0)
		tgroup::handleevent(event);
    
	
}


bool mouseinstalled;



logmessagebox::logmessagebox(INT16 x,INT16 y,INT16 w,INT16 h,tlinkedlist *messages):tview(x,y,w,h)
{
    offset=sel=0;
    themessages=messages;
}

logmessagebox::~logmessagebox()
{
    //tlinkedlist.destroy();
}

void logmessagebox::update()
{
    INT16 addsize=t_charset.ysize+2;
    valuedata valdata;
    maxlines=themessages->count-(height()/addsize);
    offset=maxlines;
    valdata.min=0;valdata.max=maxlines;valdata.val=offset;
    //this could cause too many redraws as changebounds uses it also and
    //it invokes the scrollbar's setrange procedure
    //this then forces a redraw; need to find some way around this that's not too messy
    sendcommand(CMDydeltachanged,owner,&valdata);
    drawview();
}

void logmessagebox::draw()
{
    INT16 addsize=t_charset.ysize+2;
    textboxfilledb(a.x,a.y,b.x,b.y);
    tcolour oldcolour=t_col;
    

    logmessagestring *celem=(logmessagestring*)themessages->first;
    INT16 ypos=(b.y-addsize) + (maxlines-offset) * addsize;
    while (celem!=NULL)
    {
        t_col=celem->colour;
        outtextxy(a.x+2,ypos,celem->string);
        ypos-=addsize;
        celem=(logmessagestring*)celem->next;
    }
    t_col=oldcolour;
}
    
void logmessagebox::handleevent(tevent *event)
{
    tview::handleevent(event);
    if (event->what == EVcommand)
      switch (event->command)
      {
          case CMDyvalchanged:
              UINT32 oldoff=offset;
              offset=*(UINT32*)event->infoptr;
              if (oldoff!=offset)                                
                  drawview();                                
              break;                
      }
}


void setcolours()
{
    textcol=getcolour2(5,5,5);   //text colour//
    titletextcol=getcolour2(200,200,200); //for window titles//
    scrollcol=getcolour2(120,100,130); //scroller background//
    scrollhighlight=getcolour2(140,120,150); //scroller background//
    lightcol=getcolour2(160,160,170);  //highlight colour//
    midcol=getcolour2(138,138,138);    //middle colour//
    mid_darkcol=getcolour2(100,100,100);   //shadow colour//
    mid_darkcol2=getcolour2(60,60,60);   //shadow colour//
    darkcol=getcolour2(20,20,20);   //shadow colour//
    backcol=getcolour2(50,20,20);   //background colour//
    gridcol=getcolour2(50,200,40);    //grid colour//
    selectcol=getcolour2(200,40,40);  //selection colour//
    titlecol=getcolour2(50,200,40);   //titlebar colour//
    disabledcol=getcolour2(50,200,40);
    menuselcolback=getcolour2(100,120,150); 
    menuselcolfore=getcolour2(255,255,255);
    textboxcol=getcolour2(190,190,190);
    highlight=getcolour2(255,255,255); //for icons e.t.c.//
}



void printlight(tlight *l,UINT16 x,UINT16 y)
{    
    sprintf(buf,"ambient r:%.2f g:%.2f b:%.2f ",
                l->ambient.r,
                l->ambient.g,
                l->ambient.b);
                                
    outtextxy(x,y,buf);
    y+=15;
    sprintf(buf,"diffuse r:%.2f g:%.2f b:%.2f",
                l->diffuse.r,
                l->diffuse.g,
                l->diffuse.b);

    outtextxy(x,y,buf);
    y+=15;                
                
    sprintf(buf,"specular r:&.2f g:%.2f b:%.2f",
                l->specular.r,
                l->specular.g,
                l->specular.b);

    outtextxy(x,y,buf);
    y+=15;

    sprintf(buf,"Orientation x:%.2f y:%.2f z:%.2f",
                l->o.x,
                l->o.y,
                l->o.z);

    outtextxy(x,y,buf);
    y+=15;

    sprintf(buf,"Position x:%.2f y:%.2f z:%.2f",
                l->p.x,
                l->p.y,
                l->p.z);

    outtextxy(x,y,buf);
    y+=15;    
}    

void gettreedatafromobj(ttreenode *node,char **txt,tsurface **pic,UINT32 **attrib)
{
    UINT32 type = ((tobject3d*)node)->type;
    *txt=(char*)&((tobject3d*)node)->name;
    *attrib=&((tobject3d*)node)->treeattrib;
    *pic=sphere;    
}

void logmessagefunc2(void *data,const char *format,...)
{
    /*static ypos=0;
    logmessagestring* msg=new logmessagestring;
    strcpy(msg->string,format);
    msg->colour=*(tcolour *)data;
    T3Dmessages.addtofront((telem *)msg);
    if (view)
    {        
        view->messages->update();
    }
    else if (GFX_INITIALIZED)
    {
       t_col=*(tcolour *)&data;
       t_fillcol=getcolour2(0,0,0);
       outtextxy(0,ypos+=t_charset.ysize,msg->string);
    }else
    {
       printf(msg->string);
       printf("\n");       
    } */       
}

void extrudingstatechangefunc(UINT32 i)
{
    if (i)
       view->bextruding->setdown();
    else
       view->bextruding->setup();
}
     


void logmessagefunc(void *data,const char *format,...)
{
    /*static ypos=0;
    logmessagestring* msg=new logmessagestring;
    strcpy(msg->string,format);
    msg->colour=*(tcolour *)data;
    T3Dmessages.addtofront((telem *)msg);
    if (GFX_INITIALIZED)
    {
       t_col=*(tcolour *)&data;
       t_fillcol=getcolour2(0,0,0);
       outtextxy(0,ypos+=t_charset.ysize,msg->string);
    }else
    {
       printf(msg->string);
       printf("\n");       
    } */       
}

static void setupmaterials()
{
	trgbfloat black;    
    black=trgbfloat(0.0f,0.0f,0.0f);
	
    firstmaterial=T3Daddmaterial(black,trgbfloat(1.0f,0.2f,0.2f),trgbfloat(0.5f,0.2f,1.0f),black,0,0);
    T3Daddmaterial(black,trgbfloat(0.2f,1.0f,0.2f),trgbfloat(0.3f,0.7f,0.7f),black,0,0);
    T3Daddmaterial(black,trgbfloat(0.2f,0.2f,1.0f),trgbfloat(0.5f,1.0f,1.4f),black,0,0);
    T3Daddmaterial(black,trgbfloat(0.2f,0.6f,0.2f),trgbfloat(0.5f,0.2f,1.0f),black,0,0);
    T3Daddmaterial(black,trgbfloat(0.6f,1.0f,1.0f),trgbfloat(0.3f,0.7f,0.7f),black,0,0);
    T3Daddmaterial(black,trgbfloat(1.0f,0.2f,1.0f),trgbfloat(0.5f,1.0f,1.4f),black,0,0);   
    T3Dmaterial=T3Daddmaterial(black,trgbfloat(0.9f,0.9f,0.9f),trgbfloat(0.7f,0.7f,0.7f),black,0,0);
}

char txt_md3quake[]="*.md3 Quake III model";
char txt_3dclayworks[]="*.3d Clayworks";
char txt_odtmindformer[]="*.odt Mindformer";
char txt_lwolightwave[]="*.lwo Lightwave";
char txt_3ds[]="*.3ds 3d studio";
char txt_xeios[]="*.xg Xeios scenefile";

static void setupfilefilters()
{
	strcpy(merge3dobject.fullpath,EXE_PATH);
    strcat(merge3dobject.fullpath,"*.3d");
    merge3dobject.filtercount=5;
    merge3dobject.presetfilters=new listelem[merge3dobject.filtercount];
    merge3dobject.presetfilters[0].set(txt_3dclayworks,0,0,0,0);
    merge3dobject.presetfilters[1].set(txt_odtmindformer,0,0,0,0);
    merge3dobject.presetfilters[2].set(txt_lwolightwave,0,0,0,0);
	merge3dobject.presetfilters[3].set(txt_md3quake,0,0,0,0);
	merge3dobject.presetfilters[4].set(txt_3ds,0,0,0,0);
    merge3dobject.filterindex=0;
    merge3dobject.type=FDTopen;
    merge3dobject.fileindex=0;
    merge3dobject.okcommand=CMDappend3dobject;
    merge3dobject.cancelcommand=CMDcancel;
        
    strcpy(load3dobject.fullpath,EXE_PATH);
    strcat(load3dobject.fullpath,"*.3d");
    load3dobject.filtercount=5;
    load3dobject.presetfilters=new listelem[load3dobject.filtercount];
    load3dobject.presetfilters[0].set(txt_3dclayworks,0,0,0,0);
    load3dobject.presetfilters[1].set(txt_odtmindformer,0,0,0,0);
    load3dobject.presetfilters[2].set(txt_lwolightwave,0,0,0,0);
	load3dobject.presetfilters[3].set(txt_md3quake,0,0,0,0);
	load3dobject.presetfilters[4].set(txt_3ds,0,0,0,0);
    load3dobject.filterindex=0;
    load3dobject.type=FDTopen;
    load3dobject.fileindex=0;
    load3dobject.okcommand=CMDload3dobject;
    load3dobject.cancelcommand=CMDcancel;


    strcpy(save3dobject.fullpath,EXE_PATH);
    strcat(save3dobject.fullpath,"*.3d");
    save3dobject.filtercount=2;
    save3dobject.presetfilters=new listelem[save3dobject.filtercount];
    save3dobject.presetfilters[0].set(txt_3dclayworks,0,0,0,0);
	save3dobject.presetfilters[1].set(txt_xeios,0,0,0,0);
    save3dobject.filterindex=0;
    save3dobject.type=FDTsave;
    save3dobject.fileindex=0;
    save3dobject.okcommand=CMDsave3dobject;
    save3dobject.cancelcommand=CMDcancel;

	strcpy(loadtexture.fullpath,EXE_PATH);
    strcat(loadtexture.fullpath,"*.pcx");
    
    loadtexture.filtercount=3;
    loadtexture.presetfilters=new listelem[loadtexture.filtercount];
    loadtexture.presetfilters[0].set("*.bmp Microsoft",0,0,0,0);
    loadtexture.presetfilters[1].set("*.pcx Zsoft",0,0,0,0);
	loadtexture.presetfilters[2].set("*.tga TargaVison",0,0,0,0);
    loadtexture.filterindex=1;
    loadtexture.type=FDTopen;
    loadtexture.fileindex=0;
    loadtexture.okcommand=CMDloadtexture;
    loadtexture.cancelcommand=CMDcancel;
}

static void setupscene()
{
	T3Dnumcopies=1;                
    T3Dinit(100000,1024*512);                
    T3Dctm=&T3Dcmat;    
    LMidentity(T3Dctm);    

    T3Dpolycreationflags|=cfWRAPTEXTURE;    
    //_splitpath(argv[1],dummy,dummy,dummy,ext);
    //lowcase(ext);
    T3Dsetcurrentscene(new teditscene);
    T3Dcurrentscene()->setfog(400,3000,1);
    T3Dcurrentscene()->fogcolour=trgbfloat(20/255.0f,80/255.0f,120/255.0f);
    T3Dsceneattributes|=sfFOG;
	T3Dcurrentscene()->curr.object=(tobject3d*)T3Dcurrentscene()->scenegraph.root;                  	    
	T3Daddtexturecallback=T3Daddtexturefunc;
    T3Daddmaterialcallback=T3Daddmaterialfunc;    

    addstandardlights(T3Dcurrentscene());
	setupmaterials();                     

}

static void loadclaybitmaps()
{
	sprintf(buf,"%simages\\sphere.pcx",EXE_PATH);
    sphere=readimage(TWINrendercontext,buf,SC.pfd,chromaput);
	sprintf(buf,"%simages\\icons.pcx",EXE_PATH);
	TWINrendercontext->setinput(objecticons);
    objecticons=readimage(TWINrendercontext,buf,SC.pfd,normalput);
	objecticons->chroma=getpixel(0,0);
	TWINrendercontext->setinput(NULL);
    sprintf(buf,"%simages\\toolbar1.pcx",EXE_PATH);
    toolbm=readimage(TWINrendercontext,buf,SC.pfd,normalput);
    sprintf(buf,"%simages\\toolbar1.pcx",EXE_PATH);
    toolbm2=readimage(TWINrendercontext,buf,SC.pfd,normalput);
	sprintf(buf,"%simages\\claybitmaps.pcx",EXE_PATH);
	claytoolbm=readimage(TWINrendercontext,buf,SC.pfd,normalput);
	sprintf(buf,"%simages\\fcorner.pcx",EXE_PATH);
	fancycorner=readimage(TWINrendercontext,buf,SC.pfd,normalput);
}

static void addandinsertdialogs()
{	
    
    rtext=new richtext(100,100,300,200,"This is simply a test, \1\1\255\255\255\255 \
just to see if I've got all this stuff worked out. I'm not sure how far I'll go with this but \1\2\255\0\0\0 I think \
that this view control \1\1\0\0\0\64 will be really rather flexible\1\2\0\0\0\0, pretty kick ass altogether.\
However, there are many issues to be resolved, such as how to allow the text to flow in a flexible and fast way \
and yet still display html pages in a manner that is consitant with the browsers currently on the market such as \
Netscape communicator/navigator, opera and microsoft explorer amoungst others. Since this browser will not need to \
facilitate some of the more advanced web browsing features, such as java and javascript, I can concentrate on the \
speedy formating of text and placement of controls and images. I hope to be able to allow text to wrap around an image \
based on the form of the image; for that, flow zones need to be aware of the adjacent flow obsticles and also know a \
little bit about their nature. This should be rather exciting if I can pull it off.");    
    perlin=new perlinview(0,0);    
	prefs=new preferences(0,0);
	treeview=new treebox(340,70,200,4+(6*t_charset.ysize),&T3Dcurrentscene()->scenegraph,gettreedatafromobj);         
    matedit=new materialedit(0,0,392,255,T3Dmaterial);                	

    perlinwin=new window(100,100,400,350,"Perlin noise",perlin,CFnoresize);
    prefwin=new window(500,120,330,260,"Preferences",prefs,CFnoresize);    
    scenetree=new window(500,120,300,300,"Scene",treeview,CFybar);    
    matwin=new window(228,120,400,255,"Edit Material",matedit,CFnoresize);        
	
    
	view->insert(scenetree);
	view->insert(matwin);    
	view->insert(prefwin);
    view->insert(perlinwin);    
    view->insert(new window(500,120,300,300,"Info browser",rtext,CFybar+CFxbar));	    
}



class recttester:public tview
{
public:
	struct
	{
	trect cliprect;
	trect blitrect;
	};
	tpoint bmsize;
	tpoint *p;
	tsurface *surf;

	recttester(INT32 x1,INT32 y1,INT32 x2,INT32 y2);

	virtual void draw();
	virtual void handleevent(tevent *event);
};

void drawbitmaprect(PIXINT x,PIXINT y,PIXINT dw,PIXINT dh,tpoint *bmsize,trect *cliprect)
{
    PIXINT x2,y2,end_pw,end_ph,cylen,cxlen,cx;     
	PIXINT xclip,yclip;
	PIXINT pic_x,pic_y,pic_w,pic_h,cpic_x,cpic_y;
	pic_x=0;//srect->a.x;
	pic_y=0;//srect->a.y;
	pic_w=bmsize->x;//1+srect->b.x-pic_x;
	pic_h=bmsize->y;//1+srect->b.y-pic_y;	
    

    x2=x+dw-1;
    y2=y+dh-1;
    
    if (x<cliprect->a.x)
    {
        if (x2<cliprect->a.x) return;
        cpic_x=cliprect->a.x-x; 
        x=cliprect->a.x;
    }else
		cpic_x=0;
    if (y<cliprect->a.y)
    {
        if (y2<cliprect->a.y)return;        
        cpic_y=cliprect->a.y-y; 
        y=cliprect->a.y;
    }else
		cpic_y=0; 
    if (x2>cliprect->b.x)
    {
        if (x>cliprect->b.x) return;
        x2=cliprect->b.x;
    }
    if (y2>cliprect->b.y)
    {
        if (y>cliprect->b.y) return;        
        y2=cliprect->b.y;
    }
	rectangle(x,y,x2,y2);

    //xlen=x2-x + 1;ylen=y2-y + 1;
	if ((x2-x)<0 || (y2-y)<0) return;				
	
	cpic_x-=((cpic_x) / pic_w) *pic_w;
	cpic_y-=((cpic_y) / pic_h) *pic_h;

	

	
	INT32 blue=0;
	
		
		//first row
	INT32 endx=x2-pic_w;
	INT32 endy=y2-pic_h;	

	cylen=pic_h-cpic_y;		
	if ((yclip=(y2-(y+cylen)))<0)
		cylen+=yclip;
	

	cxlen=pic_w-cpic_x;
	if ((xclip=(x2-(x+cxlen)))<0)
	{
		t_fillcol=getcolour2(0xFF,blue,0x0);		
		cxlen+=xclip;
		bar(x,y,x+cxlen,y+cylen);
		y+=cylen;
		if (yclip>0)
		{

			while (y<endy)
			{			
				t_fillcol=getcolour2(0xFF,0xAA,0x0);		
				
				bar(x,y,x+cxlen,y+pic_h);			
				y+=pic_h;
			}
			blue=255;
			if ((end_ph=(y2-y))>0)	
			{
					
				t_fillcol=getcolour2(0xFF,blue,0x0);		
				
				bar(x,y,x+cxlen,y+end_ph);							
			}

		}
	}else
	{		
		cx=x;
		t_fillcol=getcolour2(0xFF,0xAA,blue);		
		bar(cx,y,x+cxlen,y+cylen);
		cx+=cxlen;		
		//sadrs+=cxlen+cxlen;		
		t_fillcol=getcolour2(0xDD,0xFF,blue);
		while (cx<endx)
		{
			bar(cx,y,cx+pic_w,y+cylen);
			cx+=pic_w;
			//sadrs+=pic_w+pic_w;			
		}
		
		
		t_fillcol=getcolour2(0xAA,0xFF,blue);
		if ((end_pw=(x2-cx))>0)				
		{		
			bar(cx,y,cx+end_pw,y+cylen);			
		}

		if (yclip>0)
		{

			y+=cylen;
		
		
			blue+=128;
			while (y<endy)
			{			
				t_fillcol=getcolour2(0xFF,0xAA,blue);		
				cx=x;
				bar(cx,y,x+cxlen,y+pic_h);			
				cx+=cxlen;					
				t_fillcol=getcolour2(0xDD,0xFF,blue);
				while (cx<endx)
				{
					bar(cx,y,cx+pic_w,y+pic_h);
					cx+=pic_w;
					//sadrs+=pic_w+pic_w;			
				}
			
				t_fillcol=getcolour2(0xAA,0xFF,blue);
				if (end_pw>0)				
				{		
					bar(cx,y,cx+end_pw,y+pic_h);			
				}
				y+=pic_h;
			}

			if ((end_ph=(y2-y))>0)	
			{
		
				blue+=127;
				t_fillcol=getcolour2(0xFF,0xAA,blue);		
				cx=x;
				bar(cx,y,x+cxlen,y+end_ph);			
				cx+=cxlen;					
				t_fillcol=getcolour2(0xDD,0xFF,blue);
				while (cx<endx)
				{
					bar(cx,y,cx+pic_w,y+end_ph);
					cx+=pic_w;
					//sadrs+=pic_w+pic_w;			
				}
				t_fillcol=getcolour2(0xAA,0xFF,blue);
				if (end_pw>0)				
				{		
					bar(cx,y,cx+end_pw,y+end_ph);			
				}
			}
		}
	}
}

recttester::recttester(INT32 x1,INT32 y1,INT32 x2,INT32 y2):tview(x1,y1,x2,y2)
{
	cliprect.rassign(70,70,300,300);
	bmsize.assign(50,50);
	blitrect.rassign(40,40,500,500);
	surf=TWINrendercontext->createsurface(SC.size.x,SC.size.y,SC.pfd);
	p=NULL;
}

void drawunclippedrects(INT32 x,INT32 y,INT32 dw,INT32 dh,tpoint bmsize)
{
	for (int yi=0;yi<dh/bmsize.y;yi++)
		for (int xi=0;xi<dw/bmsize.x;xi++)
		{
			rectangle(x+(xi*bmsize.x),y+(yi*bmsize.y),x+((xi+1)*bmsize.x),y+((yi+1)*bmsize.y));
		}	
}

void recttester::draw()
{
	trect vp=SC.viewport;
	TWINrendercontext->setoutput(surf);
	t_fillcol=getcolour2(0xFF,0xFF,0xFF);
	textboxfilledb(0,0,width(),height());
	
	if (p)
		circle(p->x,p->y,5);
	
	t_col=getcolour2(0xFF,0,0);
	rectangle(cliprect);
	t_col=getcolour2(0xFF,0,0xFF);
	rectangle(blitrect);
	t_col=getcolour2(0,0xFF,0);
	drawunclippedrects(blitrect.a.x,blitrect.a.y,blitrect.width(),blitrect.height(),bmsize);
	t_col=getcolour2(0,0,0xFF);
	drawbitmaprect(blitrect.a.x,blitrect.a.y,blitrect.width(),blitrect.height(),&bmsize,&cliprect);
	TWINrendercontext->setoutput(NULL);
	SC.viewport=vp;
	putbitmap(a.x,a.y,surf);
	
}

bool closeto(tpoint *p,tpoint *p2)
{
	return ((p->x>=p2->x-5) && (p->y>=p2->y-5) && (p->x<=p2->x+5) && (p->y<=p2->y+5));
}


void recttester::handleevent(tevent *event)
{
	
	tview::handleevent(event);
	UINT32 what=event->what;
	event->what=0;

	if (what&EVmouse)
	{
		if (state & SFmodal)
		{
			if (mb!=0 && (MOUSEmoved))
			{
				*p+=mdiff;
				drawview();
			}else
				TWINendmodal();

		}else
		{
		
		
			tpoint smp=mp-a;
			p=&cliprect.a;
			for (int i=0;i<8;i++)
			{
				if (closeto(&smp,p))
				{
					if (mb!=0)
						TWINsetmodal(this);
					drawview();
					
					return;
				}				
				p++;
			}
			p=NULL;
		}		
	}	
}

#define scalar_mask	  7
#define scalar_int8   1
#define scalar_int16  2
#define scalar_int32  3
#define scalar_float  4
#define scalar_double  5
#define scaler_pointer 8
//program break
#define op_ret  0
//i = immediate varialbe
//p = indirect/pointer to variable
//c = constant value

//32 bit integer math
#define op_iadd_i  1
#define op_iadd_p  2
#define op_iadd_c  3
#define op_isub_i  4
#define op_isub_p  5
#define op_isub_c  6
#define op_imul  7
#define op_idiv  8
#define op_imod  9
#define op_ipow  10
#define op_iroot 11
//floating point math
#define op_fadd  12
#define op_fsub  13
#define op_fmul  14
#define op_fdiv  15
#define op_fpow  16
#define op_froot 17
#define op_sin  18
//bitwise
#define op_and  19
#define op_or   20
#define op_xor  21
#define op_shl  21
//jumps
#define op_jmp  22  //unconditional
#define op_jmpge 23 //>=
#define op_jmple 24 //<=
#define op_jmpe  25 //==
#define op_jmpg  26 //>
#define op_jmpl  27 //<
#define op_jmpz  28 //==0
//comparisons
#define op_cmp   29
#define op_test  30

class vm;
typedef UINT32 (*opcode) (vm *m);


class vm
{
	UINT32 state;
	union 
	{
		struct
		{
			UINT32 ebp;
			UINT32 esp;
			UINT32 edi;
			UINT32 esi;
			UINT32 eax;
			UINT32 ebx;
			UINT32 ecx;
			UINT32 edx;			
			float f0;
			float f1;
			float f2;
			float f3;			
		};
		UINT32 heap[256];
	};
	UINT32 *stack;
	UINT32 *code;
	UINT32 *pc;
	UINT32 run();
	vm(UINT32 stacksize,UINT32 *code_);
};

vm::vm(UINT32 stacksize,UINT32 *code_)
{
	stack=new UINT32[stacksize];
	code=code_;
	pc=code;
	state=0;
	ZeroMemory(heap,256<<2);
}

UINT32 vm::run()
{
	while (*pc!=op_ret)
	{

	}
	return eax;
}


class variabletype
{
	UINT32 type;
};


class mathnode:public ttreenode
{
public:
	UINT32 opcode;
	
	union
	{		
		double val;
		variabletype *pval;
	};
};


typedef float (*mathfunc) (float x);
typedef void (*mathfunc2) (float t,float *x,float *y);


void gridf(float x,float y,float w,float h,float gxsize,float gysize)
{  
  register INT16 lop,x2,y2;
  float i;
  x2=x+w;
  y2=y+h;
  for (i=1;i<=x2;i+=gxsize)
    vline(i,y-1,y2+y-1);
  for (i=1;i<=y2;i+=gysize)
    hline(x+1,x2+x+1,i);
}

#define step 0.005

extern flt safepow(flt val,flt power);



void circlefunc(float t,float *x1,float *y1)
{
	*x1=t*t*t;
	*y1=t*3*t*t;
	
	//return pow(x*0.01)*20;
}


float pfunc3(float x)
{
	return (500*x)-((320*x*x)-(x*x*x)); 
	//return pow(x*0.01)*20;
}


float pfunc2(float x)
{
	return x*x;
	//return pow(x*0.01)*20;
}

float golden()
{
	return (1+sqrt(5))/2;
}



class functype:public telem
{
public:
	tcolour col;
	mathfunc2 func;
	functype(mathfunc2 func_,tcolour col_)
	{
		func=func_;
		col=col_;
	}
};

class viewmathfunc:public tview
{
public:
	tpointfloat off,scale;
	tsurface *surf;
	trender cmds;
	tlinkedlist mathlist;
	void addfunc(mathfunc2 func,tcolour col)
	{
		mathlist.push((telem*)new functype(func,col));
	}

	void calcfunc();
	viewmathfunc();
	virtual void draw();
	virtual void changebounds(trect &bounds)
	{
		UINT32 c=( (bounds.height()!=height()) || (bounds.width()!=width()));
		tview::changebounds(bounds);
		if (c)
		calcfunc();
		
	}
};


viewmathfunc::viewmathfunc():tview(0,0,1,1)
{
	off.assign(0,0);
	scale.assign(1,1);
	mathlist.init(sizeof(functype));
	surf=TWINrendercontext->createsurface(SC.size.x,SC.size.y,SC.pfd);
}

void drawsquare(float x1,float y1,float x2,float y2,float z,trender *cmds)
{
	cmds->normal(0,0,1);
	cmds->colour(0.0f,0,0,0);
	cmds->position(x1,y1,z);
	

	cmds->colour(1.0f,1.0f,0,0);
	cmds->position(x2,y1,z);

	cmds->colour(0.0f,1.0f,1,0);
	cmds->position(x2,y2,z);

	cmds->colour(0.0f,0.0f,1,0);
	cmds->position(x1,y2,z);
}


void drawcube(float x1,float y1,float z1,float x2,float y2,float z2,trender *cmds)
{
	drawsquare(x1,y1,x2,y2,z1,cmds);
	drawsquare(x1,y1,x2,y2,z2,cmds);
}


void viewmathfunc::calcfunc()
{
	TWINrendercontext->setoutput(surf);
	float range;
	tpointfloat p1,p2,of;
	of.x=width()>>1;
	of.y=height()>>1;
	range=2*PI;

	//drawgrid(0,0,10,10,surf->size.x/10,surf->size.y/10,NULL,1);
	
	t_fillcol=getcolour2(255,255,255);
	cleardevice();



	t_col=getcolour2(200,200,200);
	gridf(0,0,width(),height(),15.0,15.0);
	t_col=getcolour2(0,0,0);
	hline(0,width(),of.y);
	vline(of.x,0,height());
	
	/*functype *func=(functype*)mathlist.first;
	while (func!=NULL)
	{
		mathfunc2 mfunc=func->func;
		t_col=func->col;
		for (float t=0;t<range;t+=step)
		{
			mfunc(t,&p1.x,&p1.y);
			p1*=scale;			
			p1+=of;
			mfunc(t+step,&p2.x,&p2.y);
			p2*=scale;
			p2+=of;
			line(float2int(p1.x),float2int(p1.y),float2int(p2.x),float2int(p2.y));
		}
		func=(functype *)func->next;
	}*/
	
	t_col=getcolour2(255,0,0);
	double say1=(step*step)*2;//step/2.5;//step*step*step;
	double sc1=0,sc2=0,sc3=0;
//*step
	
	/*for (float x=0;x<ox;x+=step)
	{
		sc1+=say1;
		sc2+=sc1;						
		putpixel(x+ox,(sc2*scale.y)+oy);
			
	}*/
	cmds.identity();
	//cmds.disable(TR_LIGHTING);
	cmds.translate(0,0,5);
	cmds.viewport(0,0,width(),height());
	
	cmds.rotatez(2*PI / 10);
	cmds.rotatey(2*PI / 20);
	cmds.setlightpos(0,0.0,50,1);
	//cmds.translate(2.5,0,5);
	//cmds.matrixmode(tr_projmatrix);
	//cmds.c_matrix=&cmds.projmat;
	//cmds.translate(0,0,0);
	cmds.setrendersurface(surf);
	cmds.beginscene();
	cmds.beginsurface();
	drawsquare(-1,-1,1,1,1,&cmds);
	cmds.endsurface();
	cmds.endscene();

	TWINrendercontext->setoutput(NULL);
}

void viewmathfunc::draw()
{
	putbitmap(a.x,a.y,surf);
	vector v1,v2;
	v1.moveto(0,1,0);
	v2.moveto(1,1,0);
	v2.normalize();

	float val=dot(v1,v2);
	sprintf(buf,"val:%f",val);
	outtextxy(a.x,a.y,buf);
}

/*
void settexture2(tsurface *tex)
{
    texture=tex;
	if (tex)
    {
	    t_texturedataptr=tex->scrptr;
		t_texturestartoffsets=tex->startoffsets;
	    t_texturesize=tex->size;
		t_textureheightscale=(tex->size.y-1)*65536.0f;
		t_texturewidthscale=(tex->size.x-1)*65536.0f;
	}
}

*/
int PASCAL WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{ 	
	MSG msg;
	HRESULT r;
	
	if (!(g_hwnd=createmainwindow(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),hInstance)))			
	{
		MessageBox(g_hwnd, "Failed to initialize graphics. \n Please ensure that you have DirectX version 7 or greater installed.","Graphics subsystem Error", MB_OK);
		return FALSE;	
	}

/*	if (FAILED(r=createdxbuffers(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),g_hwnd,FALSE)))
	{
		MessageBox(g_hwnd, "Problem creating directx buffers", "Error", MB_OK);
		return FALSE;
	}

	SC.startoffsets= new UINT32[2400];

	if (FAILED(r=buildcontextfromsurface(g_pDDSPrimary ,&SC)))
	{
		MessageBox(g_hwnd, "Failed to create TWIN device context", "Error", MB_OK);
		return FALSE;
	}*/
	
	
	/*IC=SC_BASE=SC;	
	setinput(&SC);
	t_chroma.c32val=0;*/


	//makepath(EXE_PATH,argv[0]);
	GetCurrentDirectory(255,EXE_PATH);
	strcat(EXE_PATH,"\\");

	setenvstring("EXE_PATH",EXE_PATH);
	setenvstring("MODEL_PATH",CURRENT_MODEL_PATH);
		
	//strcpy(EXE_PATH,"d:\\code\\test\\");
    sprintf(buf,"%sfonts\\sansf16b.fnt",EXE_PATH);    	        

	FNTload(buf,&t_charset);							
	sprintf(buf,"%simages\\",EXE_PATH);    	        
	TWINinit(buf);
	parselogfile(NULL);
	/* (!))
	{
		//MessageBox(g_hwnd, "Could not initialize TWIN", "Error", MB_OK);
		return FALSE;
	}*/
	    
	trender::initialize(TWINrendercontext);
    T3Dmessages.init(sizeof(logmessagestring));
    T3Dextrusionmodestatchange=extrudingstatechangefunc;        
    LOGinit(logmessagefunc);    
	setupcolours();

    logmessage((void*)&T3DCOLboundingbox,"Starting Clayworks 3.0w for Windows");
    logmessage((void*)&T3DCOLboundingbox,"Make sure you are in fullscreen mode and then press a key");
	
	setupscene();
    loadclaybitmaps();                         
	setupfilefilters();
	//create main clayworks object

	view=new clay(0,0,SC.size.x,SC.size.y);                
	//add floating dialogs
	//tgroup *test=new tgroup();
	
	//test->insert(new tbutton(0,0,40,40,1,0,13,"Hi",NULL,NULL));

    
	T3Docttree=new octtree();	
	T3Dcurrentscene()->addobject(NULL,T3Docttree);
		
	addandinsertdialogs();    
	viewmathfunc *mathview=new viewmathfunc();
	mathview->addfunc(circlefunc,getcolour2(0,160,100));
	//mathview->addfunc(pfunc2,getcolour2(0,160,0));
	//mathview->addfunc(pfunc3,getcolour2(0,160,100));
	//mathview->addfunc(pfunc3,getcolour2(0,0,160));
	mathview->calcfunc();
	view->insert(new window(100,100,400,350,"math func",mathview,CFclientedge));
	
	recttester *test=new recttester(0,0,400,400);
    //create main container class	
	//tbutton *button=new tbutton(50,50,40,40,1+BAcentre ,0,13,"Hi",NULL,NULL);
    mainwin=new mswindow(0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),"ClayWorks",view,CFclientedge,g_hwnd);//CFybar+CFxbar+
	/*tmenu *mainmenu=createmainmenu();
	mainwin->insertrel(mainmenu);	
	trect bounds;
	bounds.rassign(mainwin->a.x+TWINctrlbm.ce0_v.l.w,mainwin->a.y+TWINctrlbm.window0.lbbord.h,mainwin->b.x-TWINctrlbm.window0_v.r.w-TWINctrlbm.scroll.bgmid.h,mainwin->a.y+TWINctrlbm.window0.lbbord.h+mainmenu->height());
	mainmenu->changebounds(bounds);
	mainwin->view->getbounds(&bounds);
	bounds.a.y+=mainmenu->height();	
	mainwin->view->changebounds(bounds);
	
	mainwin->xbar->setrange(0,100,40);
	mainwin->ybar->setrange(0,100,40);*/
		
	//set as root and setup callback functions
	TWINsetroot(mainwin);

	//TWINsetpopuptarget(test);
	TWINsetpopuptarget(view);
	TWINsetlockcallbacks(lockcallback,unlockcallback);
	TWINsetcliprectcallback(cliplistcallback);
	root->drawview();
    LOGinit(logmessagefunc2);    

	ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);
    SetFocus(g_hwnd);
	//tsurface *tgapic=readtga(TWINrendercontext,"d:\\code\\winclay\\images\\torso.tga",SC.pfd,0);
	//putbitmap(10,10,tgapic);
	/*setscanmode(6);
	t_colrgba.r=128;
	t_colrgba.g=128;
	t_colrgba.b=128;
	
	tri(&tv[0],&tv[1],&tv[2]);
	putbitmap(0,0,tex);*/
	// Main message loop:
	tevent event;
    while (TRUE)
    {			
		//ms_wheel=0;
		event.what=0;
		if (!GetMessage(&msg, NULL, 0, 0))
			return msg.wParam;		
		MOUSEclearflags();
		getkeyevents(msg,event);
		//getmouseandkeyevents(msg,event);
		if (event.what)
		{
			if ((event.what == EVkeydown) & (!KEYgetscan() || charcode))
			   TWINfinalizekeyevent(event);		
			//putbitmap(0,0,vgadgets,0);
		}				
		TWINeventmpos();
		DispatchMessage(&msg);
	}
	
	//releasedxbuffers();
	cleanupwindowsstructures();
	shutdown:
	
	TWINrendercontext->setoutput(NULL);
	T3Dshutdown();
	//closegraph();
  
	logmessage((void*)&T3DCOLboundingbox,exitstring);
	logmessage((void*)&textcol,"Thank you for using this early beta of Clayworks.");
	logmessage((void*)&textcol,"Please report any bugs you may have found to luther2001@hotmail.com");
	logmessage((void*)&textcol,"Press any key to continue");
	return TRUE;
}

