////////////////////////////////////////////////////////////////////////////////////
//DEFINES

//These will be enumerations

#define DMselect          0
#define DMcreatemesh      1
#define DMcreatespline    2
#define DMcreatemetaballs 3
#define DMcreateuv        4

#define MDMcube           0
#define MDMsphere         1
#define MDMcone           2
#define MDMcylinder       3
#define MDMselipse        4
#define MDMsquare         5
#define MDMcircle         6
#define MDMpolygon        7
#define MDMtext           8
#define MDMline			  9
#define MDMdot			  10
#define MDMgrid           12

//element types, used in selection. T3Dselectmode uses these.
#define ETvertex		  0  //for geom meshes and things with vertex like handles like nurb curves and surfaces
#define ETedge			  1 //for geom meshes only
#define ETpolygon         2 //for geom meshes only
#define ETobject          3  //all selectable objects (lights, meshes, nurb objects, blobs, cameras ect)

//select functions
#define STstretch		  0
#define STrotate		  1
#define STscale			  2
#define STshear			  3
#define STtaper			  4
#define STtwist			  5
#define STaxisrotate	  6
#define STaxistranslate   7
#define STaxisscale		  8

#define CMDextrusionended        1005
#define CMDextrusionstarted      1006
#define CMDobjectdeleted         1007
#define CMDobjectcreated         1008
#define CMDcalc3d                1009
#define CMDorthoscalechanged     1010
#define CMDscenealtered          1011            //everything's redrawn
#define CMDscenealteredorth      1012        //just orthographic views are redrawn
#define CMDundraw                1013                  //scene is redrawn without object specified in infoptr
#define CMDcurrentelementchanged 1014

//drawing modes for tscene calc3dmode
#define CMdrawall 0             //draw all objects
#define CMsingleobj 1           //just draw selected object
#define CMlayeredsingleobj 2    //draw background and layer single object ontop
#define CMlayeredsingleobjclip 3//as above only clipped to bounds
#define CMdrawallclip 4         //draw all objects, clipped to bounds   
#define CMselectedonly 5        //only draw selected
#define CMlayeredselectedonly 6 //draw background and layer selected elements on top
#define CMinvselectedonly 7     //draw everything that isn't selected

//redraw levels
//These are the values for tscene::redrawlevel that index an array of vertex, polygon and object attribute filters defined bellow.
//combined with the tscene::calc3dmode variable, these provide a very high level of control over what vertices, polygons
//and objects will be processed and redrawn at a given time, increasing the speed of processing and reducing flicker.
//NB these values are only used if the current calc3dmode is either CMinvselectedonly, CMselectedonly or CMlayeredselectedonly:       
#define RDLselection 0  //same damned thing
#define RDLextrusion 1           
#define RDLcurrentvertex 2
#define RDLcurrentpolygon 3
#define RDLcurrentedge 4

//copying attributes. This defines what will happen when a selection is transformed
//if this is zero, it will just be transformed. Otherwise it will either be extruded or copied.
//if the STRNSkeepoldpolys flag is set, the polygons (if applicable) at the base of the extrusion
//will remain in place, otherwise they will be removed from the list and placed on the undo stack 
#define STRNScopy 1
#define STRNSextrude 2
#define STRNSkeepoldpolys 4

//extra extrude definitions
#define EXTRDjustedges 1  //Does not copy interior polygons

//Globals

extern flt T3Dboundingboxrotation;
extern tpointflt T3D2drotationpivot;

////////////////////////////////////////////////////////////////////////////////////
//CLASS DEFINITIONS


class tedgeloop
{
    public:
    UINT32 type;
    tedge **start;
    UINT32 count;
};



typedef struct undostruct
{
    UINT32 command;
    UINT32 datacount;
    UINT32 selectmode; //select mode at time of command    
    union
    {
        matrix4x4 *matrix;
        vertex *vertexlist;
        tpolygon *polygonlist;
        tobject3d *objectlist;
    };    
}undostruct;



//can either be applied to result or mask
class elementfilter
{
    public:
    UINT32 vertex,edge,polygon,object; 
};




extern listelembase T3Dselectionfunctions[];
extern listelembase T3Dselectionlevels[4];
extern listelembase T3Dselectioneffects[5];
extern listelembase T3Dselectiongeomerty[4];


//Index is T3Dselectmode of type etXXXXXX
extern UINT32 T3Dselectmasks[4];
extern elementfilter T3Dredrawmasks[4];


//bounding system is a little pants, needs some work
//Bounds are determined from scene->singleobj



////////////////////////////////////////////////////////////////////////////////////////
//TYPE SPECIFICATION

class teditscene;

typedef union extraobjectdata
{
    struct sphere
    {
        UINT32 xsegs,ysegs;
    };
    struct superelipse
    {
        UINT32 xsegs,ysegs;
        flt p1,p2;
    };
    struct cylinder
    {
        UINT32 xsegs,ysegs;
    };
    struct grid
    {
        UINT32 xsegs,ysegs;
        UINT32 funcnumber,arg1,arg2,arg3;        
    };
    struct circle
    {
        UINT32 ysegs;        
    };
    struct cone
    {
        UINT32 xsegs,ysegs;
    };
    struct text3d
    {
        char *text;
        UINT32 fontref;
    };
} extraobjectdata;

class spheredata
{
    public:
    UINT32 xsegs,ysegs;
};

class selipsedata
{
    public:
    UINT32 xsegs,ysegs;
    flt p1,p2;
};


class conedata
{
    public:
    UINT32 xsegs,ysegs;
};

class cylinderdata
{
    public:
    UINT32 xsegs,ysegs;
};

class griddata
{
    public:
    UINT32 xsegs,ysegs;
    UINT32 funcnumber,arg1,arg2,arg3;        
};


class circledata
{
    public:
    UINT32 segs;
};

class textdata
{
    public:
    char txt[255];
};

class edit3dcontainer;
class view3d;
class edit3d;


class textrusion
{
    public:
    UINT32 attribute;
    vector p;
    axisrotation o;
    flt s;
    flt l;
    matrix4x4 tmat;    
    vertex **vlist;
    tedge **elist;
    tpolygon **plist;
    tedge **ecnxlist;
    tpolygon **pcnxlist;
    UINT32 sv_count,se_count,sp_count,cnxe_count,cnxp_count;
    //textrusion(UINT32 vc,UINT32 ec,UINT32 pc,vertex **vl,tedge **el,tpolygon **pl,UINT32 attr);
    //INT32 create(teditscene *scn,UINT32 attr);
    void copyselection(teditscene *scn);

    vertex *addvref(vertex* v);
    tedge *adderef(tedge* e);
    tpolygon *addpref(tpolygon* p);
    tedge *addecnxref(tedge* e);
    tpolygon *addpcnxref(tpolygon* p);    
};

class tboundingbox3d;
class tplaneprojection;

class teditscene:public tscene
{
    public:
    UINT32 drawmode;    
	UINT32 meshdrawmode;    

	UINT32 selectionmode;			//the current selection mode; do3d functions need to be aware of this.
	UINT32 selectionfunction;   //the type of manipulation; e.g. stretch/translate, rotate, twist. etc.
	UINT32 selectioneffect;     //The effect that a new selection will have on the current selection


    UINT32 redrawlevel;  //indexs an array of attribute filters for vertices, polygons and objects
    UINT32 extrusioncount;  //number of extrusions
    UINT32 lastextrusion;
    UINT32 sv_count,se_count,sp_count,so_count;
    UINT32 ev_count;                     //edge vertex count
    UINT32 av_count,ae_count,ap_count;   //affected (but not selected) counts
    UINT32 ce_count;                     //contained edge count; edges that are part of selection
                                         //but are not selected (i.e. they should be copied; these
                                         //are tagged onto the selected edge list just after the
                                         //edge that are actually selected)                                       
    UINT32 el_count; //edge loop count
    //these are the extrusion conection counts, for the polygons and edges that connect different extrusion 'hubs'
    UINT32 vext_count;
    UINT32 eext_count;
    UINT32 pext_count;

    //when selections are divided up into topological regions, each one will have it's own centre and normal
    flt extrusionlength;    
    vector selectionnormal;
    vector selectioncentre;
    
    theapmem sv_list;  //selected vertices list
    theapmem se_list;  //selected edge list
    theapmem sp_list;  //selected polygon list
    theapmem so_list;  //selected object list

    theapmem svext_list;  //extruded vertices list
    theapmem seext_list;  //extruded  edge list
    theapmem spext_list;  //extruded  polygon list
    
    tedgeloop el_list[6000];  //edge loop list, references a sorted se_list

    textrusion extrusionlist[256];
    tboundingbox3d *slbox;    
    UINT8 calc3dmode;
    
    tcursor3d *cursor;
    material *cmat;
    textureref *ctex;
   // edit3dcontainer *views;
    
    elementstruct underreticle;
    tobject3d *singleobj;

    UINT32 extvcount(UINT32 lev){return (lev<(extrusioncount-1))?ev_count:sv_count;};
    UINT32 extecount(UINT32 lev){return (lev<(extrusioncount-1))?se_count:ce_count;};
    UINT32 extpcount(UINT32 lev){return (lev<(extrusioncount-1))?0:sp_count;};

	void renderhandles(tcamera *cam); //Mainly draws extra UI elements of objects in the scene. 
	void renderhandles(tobject3d *obj,tcamera *cam);
	
    void tagselpolys();

	void setdrawmode(UINT32 mode);
	void setmeshdrawmode(UINT32 mode);
    void setselectmode(UINT32 mode);
	void setselectfunction(UINT32 mode);
	void setselecteffect(UINT32 mode);

    void vectormult(vertex *vert,vector *src,matrix4x4 *mat);
    void preprocess_select(trect *cliprect,tpoint *screensize,tcamera *viewcam,INT32 offx=0,INT32 offy=0);

    //these should be one routine when the lists are unified    
    void calcselvertexbounds();
    void prepareforpervertextransformation();
    void prepareforperpolygontransformation();
    void transformvertexlist(matrix4x4 *tmat,vertex **list,UINT32 count);
    void transformpolygonlist(matrix4x4 *tmat,tpolygon **list,UINT32 count);    
    void or_elementattributes(tattribelem **v,UINT32 mask,UINT32 count);
    void and_elementattributes(tattribelem **v,UINT32 mask,UINT32 count);
    void xor_elementattributes(tattribelem **v,UINT32 mask,UINT32 count);

    void clearvertextags(vertex **v,UINT32 count);


    
    void transformselectedobjects(matrix4x4 *tmat);    
    void calculatenormals(UINT32 objectmask,UINT32 polymask,UINT32 vertexmask);
    UINT32 removestrandedelements(UINT32 omask);
    UINT32 removeverticies(UINT32 omask,UINT32 vmask);
    UINT32 removeedges(UINT32 omask,UINT32 vmask);
    UINT32 removepolygons(UINT32 omask,UINT32 vmask);
    UINT32 removeobjects(UINT32 otmask,UINT32 omask);
    bool deleteselected(UINT32 mode);
    void flipselectedpolygons();

	//highlighted elements, clear em
    void clearcurrentelements();
	
    vertex *setcurrentvertex(vertex *v);
    
    UINT32 selectelements(UINT32 elemtype,UINT32 boolmode,UINT32 omask,UINT32 emask,bool (*checkelemfunc)(tattribelem *e,UINT32 mask));
    UINT32 selectelement(UINT32 elemtype,UINT32 boolmode,tattribelem *elem);
    void clearselection(){selectelements(ETpolygon,bAND,0x0,0x0,NULL);};    
    
    bool copyselpolygons(UINT32 nsteps,UINT32 attributes);
    void extrudeselpolygons(UINT32 nsteps,UINT32 attributes); //tmat is a pointer to an array of matricies
    tpolygon **extrudeedgelists(tedge **elist1,tedge **elist2,INT32 nsteps,textrusion *ext,UINT32 attributes);
    bool mergeselectedvertices(UINT8 posmode);
    void meshdivide();
    void edgesplit();
    
    void triangulate(INT32 bmode);
    
    UINT32 tidyuppolygons(UINT32 omask,UINT32 pmask);
    textrusion *addextrusion(UINT32 attr);

    void applynumericextrusionmatrix();
    void applyextrusionattributes();
    
	void getaverageselpos(vector *pos);
    void clearextrusion();
    void startextrusion();
    void transformselectedvertices(matrix4x4 *tmat)
    {
        transformvertexlist(tmat,(vertex**)sv_list.base,sv_count);
    }

    void transformselectedpolygons(matrix4x4 *tmat);
    void transformextrusionperpolygon();    
    void projectselectionxy();       
    void transformextrusion();
            
    void resetrendermode();
    teditscene();
    ~teditscene();
    
    UINT32 undo();
    void setsp_material(material *mat);
    void setsp_texture(textureref *tex);
    void setsp_attribute(UINT32 attrib,UINT8 mode);    
    UINT32 alterscene(UINT16 command,void *param1,UINT32 param2);

	void applyuvobject(tplaneprojection *obj);
    private:
    void do_polygon_selection_finalization(t3dmesh *obj);
    void do_vertex_selection_finalization(t3dmesh *obj);
    void add_affected_elements_references();
    void find_edges(tedge *ce,bool connectifvertsselected);
};


class tboundingbox3d:public tobject3d
{
    public:
    //trect bounds;
    vertex vlist[8];
    tpolygon plist[6];
    //direction of the selection
    vector normal;    
    tpointflt norm2d;
    UINT32 *count;//pointer to the element count for this object
    tboundingbox3d(UINT32 *count);
    void setbounds(vector *_min,vector *_max);  //set these and centres here
    virtual trectflt *getbounds(matrix4x4 *,tcamera*);
    virtual void renderhandles(UINT32 attrib,tcamera *cam);
    virtual void do3d(matrix4x4 *,matrix4x4 *,tcamera*,trender *cmds);
    void sortelements();
};


class tplaneprojection:public tobject3d
{
public:
	tplane plane;
	tpointfloat p2d[4];
	tpointfloat scale,centre;
	tplaneprojection();
	virtual trectflt *getbounds(matrix4x4 *,tcamera*);
    virtual void renderhandles(UINT32 attrib,tcamera *cam);
    virtual void do3d(matrix4x4 *,matrix4x4 *,tcamera*,trender *cmds);
};


//abstract class for 3d views
class basic3dview:public tview
{
    public:        
    tcolour bg,fg;    
    teditscene *scene;
    tcamera *viewcam;
    UINT32 datasize;            
    tsurface *scr;    
    //tobject3d *obj;
    basic3dview(INT16 x,INT16 y,INT16 x2,INT16 y2,teditscene *ltscene,char *scrdata);
	~basic3dview();
    void resetdrawmode(UINT32 restorecommand);//sets the drawing mode for this scene to draw all object, it also renders the current object into the scene.
    void setsingleobjectdrawmode(tobject3d *obj);//draws the scene without drawing the specified object and then sets it up to layer that object on top
    void setselectdrawmode(tobject3d *obj);//draws the scene without drawing the specified object and then sets it up to layer that object on top
    virtual void calc3d(){;}
};

//these are used so that the edit window knows how to handle
//modal events (n.b. E3Ddrawingselectbox is also used in the draw method)

class edit3d:public basic3dview
{
    public:    
    edit3d(INT16 x,INT16 y,INT16 x2,INT16 y2,teditscene *ltscene,UINT8 axis_,char *scrdata);    
    ~edit3d();
    virtual void draw();
    void filter3dpos(flt x,flt y,flt z,vector *p);
    virtual void handleevent(tevent *event);
    void *handleselect(tevent *event,tobject3d *obj);
    virtual void changebounds(trect &bounds);
    virtual void initialupdate();
    virtual void calc3d();
    //needs acess to the real camera matrix
    void get2dpos(vector *p3d,vector *p2d);
    void get3dpos(INT16 x,INT16 y,vector *p);  //deals with a point, takes the window's position into account
    void get3dvec(INT16 x,INT16 y,vector *p);  //deals with a magnitude vector; convertes a 2d magnitude to a 3d one
    void changeoffsets(INT32 x,INT32 y);
    void setoffsets(flt  x,flt y);
    void dostretch(bool corner);
	void douvstretch(bool corner);
    void startpolygon();
    void endmodal2();
    void startmodal(void (*modalfunc)(edit3d *ed,tevent *event));
    void setscaleoffsets(flt oldscale,flt newscale);
};

class view3d:public basic3dview
{
    public:
    UINT32 xoff,yoff;    
    char lockcam;
    char rendertype;    
    view3d(INT16 x,INT16 y,INT16 x2,INT16 y2,teditscene *ltscene,char *scrdata);
    ~view3d();
    virtual void draw();
    virtual void handleevent(tevent *event);
    virtual void changebounds(trect &bounds);
    virtual void initialupdate();
    virtual void calc3d();    
};

#define E3DShoriz 1
#define E3DSvert 2


class containertoolbar:public tgroup
{
public:	
	ttoolbar *toolbar;
	tbutton *zoombutton;
	containertoolbar(INT32 x,INT32 y,INT32 x2,INT32 y2);
	virtual void handleevent(tevent *event);
};


class edit3dcontainer:public tgroup
{
public:
	INT32 zoomref;
	containertoolbar *tb[4];	
	tview *views[4];
	tpointfloat ratiocentre;
    tpoint quadcentre;    
    
    UINT8 *scr;
	UINT32 smode;
    edit3dcontainer(UINT32 x,UINT32 y,UINT32 w,UINT32 h);
	char *getpointer(INT32 ref);
    virtual void draw();
    virtual void changebounds(trect &bounds);    
	virtual void edit3dcontainer::handleevent(tevent *event);
	void setviews(tview *v1,tview *v2,tview *v3,tview *v4);
	void setcentre(INT32 x,INT32 y);
	void resizechildren();
};

////////////////////////////////////////////////////////////////////////////////////
//GLOBAL VARIABLES

extern bool T3Dcheckergrid;
extern UINT32 T3Dnumcopies;
extern UINT32 T3Dcopyattributes;
extern spheredata spherevals;
extern selipsedata selipsevals;
extern conedata conevals;
extern cylinderdata cylindervals;
extern griddata gridvals;
extern circledata circlevals;
extern textdata textvals;

#define ed_maxpolyvertices 256

extern void (*ed_modalfunc)(edit3d *ed,tevent *event);
extern void (*T3Dextrusionmodestatchange)(UINT32 i);

////////////////////////////////////////////////////////////////////////////////////
//FUNCTION PREDEFS


bool checkelembyattribute_and(tattribelem *v,UINT32 mask);
bool checkelembyattribute_equality(tattribelem *v,UINT32 mask);
bool checkpolygonbytexture(tattribelem *v,UINT32 mask);
bool checkpolygonbymaterial(tattribelem *e,UINT32 mask);
bool checkpolygonbynoncoplainarity(tattribelem *v,UINT32 mask);
bool checkpolygonifedge(tattribelem *v,UINT32 mask);


void T3Dsetbackgroundcolour(UINT32 r,UINT32 g,UINT32 b);
void T3Dsetorthoscale(flt scale);
void T3Dgetinvertedobjectmatrix(matrix4x4 *invmat,tobject3d *obj);
void T3Daddtexturefunc(textureref *tref);
void T3Daddmaterialfunc(material *mat);
void T3Dresetelementmasks();
void T3Dsetcurrentscene(teditscene *scene);
teditscene *T3Dcurrentscene();

extern tsurface *claytoolbm;

