#ifndef _basic3d_h_
#define _basic3d_h_


//bitmaks and enumerations use the format xxXXXXX)
//#define SHOW_NORMALS
#define camORTHXY 1
#define camORTHXZ 2
#define camORTHZY 3
#define cam3D     4

//render handles flags
#define rhVERTICES  0x1
#define rhEDGES     0x2
#define rhSURFACES  0x4
#define rhOBJECTS   0x8


//GLOBAL SCENE FLAGS
#define sfSPECULARHIGHLIGHTS    1
#define sfFOG                   14
#define sfLINEARFOG             2
#define sfCOSINEFOG             4
#define sfVOLUMEFOG             8
#define sfONLYSHOWSELECTEDNODES 16
#define sfCAMLOCK               32

#define sfSELECTINGVERTICES     64
#define sfSELECTINGEDGES        128
#define sfSELECTINGPOLYGONS     256
#define sfSELECTINGOBJECTS      512
#define sfSELECTING             1024

//Object types; An array of element masks (used for selection validifcation) relies on the order
//of these so it's not a good idea to change it.

#define otCAMERA	          0x1
#define otGEOM		          0x2
#define otINSTANCE            0x4
#define otLIGHT               0x8
#define otBOUNDINGBOX		  0x10
#define otCURSOR              0x20
#define otGRID				  0x40
#define otOCTTREE			  0x400
#define otPLANEPROJECTION     0x800
#define otSPHEREPROJECTION    0x1000
#define otCYLINDERPROJECTION  0x2000
#define otMETABALL		      0x4000
#define otCYLINDER		      0x8000
#define otBSPLINE             0x10000
#define otBPATCH              0x20000

//just used for file
#define otSCENE			0x80
#define otMATERIALBANK  0x100
#define otTEXTUREBANK   0x200
#define otEOF           0xFFFFF

//object flags
#define ofENABLED 0x1             //can recieve messages and collisions
#define ofVISIBLE 0x4             //object can be seen
#define ofHASOWNHEAP 0x8          //uses internal memory heap; good for objects that need to be recalculated quickly
#define ofSELECTED 0x10           //uses internal memory heap; good for objects that need to be recalculated quickly
#define ofEDITABLE 0x20           //internal object; modification restrictions apply
#define ofCURRENT 0x40            //is current object
#define ofAFFECTEDBYSELECTION 0x10000
#define ofHASSELECTEDVERTICES 0x20000
#define ofHASSELECTEDEDGES    0x40000
#define ofHASSELECTEDPOLYGONS 0x80000
#define ofCONTAINSCURRENTVERTEX 0x100000
#define ofCONTAINSCURRENTEDGE 0x200000
#define ofCONTAINSCURRENTPOLYGON 0x400000
#define ofAFFECTEDBYCURRENT  0x800000
//used for grids only
#define ofGRIDSNAP 0x1000000
//Object integrety
#define oiOK                   0x0

#define LTcont          0x1
#define LTpoint			0x2
#define LTdirectional   0x3

#define perspective 400

//for sphere clip check
#define scINSIDE 0  //all inside
#define scOUTSIDE 255   //all outside
#define scLEFT 1        //intersects left
#define scRIGHT 2
#define scUP 4
#define scDOWN 8
#define scFRONT 16
#define scBACK 32

//for cone and cylinder
//Marvelous, multipurpose, sequin studed **Creation Flags** 
#define cfANTICLOCKWISE 0x1
#define cfNOPOLYS 0x2
#define cfTOPPOLY 0x4
#define cfBOTTOMPOLY 0x8
#define cfSIDEPOLYS 0x10
#define cfALLPOLYS  0x20
//Needs a specific length array for the 'cmat' (mindrender material) flag, 2 for cone, 3 for cylinder 
#define cfCOLOURBYPART 0x40
//texture mucking about
#define cfWRAPTEXTURE 0x80
#define cfFLIPTEXTUREX 0x100
#define cfFLIPTEXTUREY 0x200
#define cfROTATETEXTURE90 0x400
#define cfROTATETEXTURE180 0x800
#define cfROTATETEXTURE270 0x1000
#define cfROTATETEXTURE360 0x2000
#define cfROTATETEXTUREMASK 0x4000
#define cfROTATETEXTURESHIFTRIGHT 0x8000
#define cfNOEDGES 0x10000


//polygon vertex attributes
#define pvaEDGEISANTICLOCKWISE 0x1  //the edge that is polygon vertex referes to is clockwise or not, relative to the polygon
#define pvaRESERVED 0x2

//polygon vertex integrity
#define pviOK 0x1
#define pviCLOCKWISEFLAGISINCORRECT 0x2
#define pviNOVERTEXREFERENCEINEDGE 0x3

//edge attributes
#define eaSELECTED 0x1             //It's a selected edge and will be extruded
#define eaHIGHLIGHTED 0x2          //It's not selected but is part of a polygon selection
#define eaEXTRUSION 0x4          //It's not selected but is part of a polygon selection
#define eaENABLED 0x8              //turned on
#define eaAFFECTEDBYSELECTION 0x10 //Not selected but affected by a selection; i.e. it will be redrawn
#define eaVISIBLE 0x20  //Not selected but affected by a selection; i.e. it will be redrawn
#define eaNOTDRAWNYET 0x40
#define eaCLOCKWISESELECTION 0x80
#define eaDOUBLESIDEDEXTRUSION 0x100  //marks this edge as an interior edge (of a selection) that will be doublesided
#define eaDELETEME             0x200   //Flag this for deletion
#define eaEDGESHARED           0x400   //error checking flag, used to check if an edge occurs more than once in the same polygon
#define eaISOLDEDGE            0x800   //error checking flag, used to check if an edge occurs more than once in the same polygon
#define eaCURRENT              0x1000  //this is the current edge
#define eaREFERENCEDBYROUGEPOLYGONS 0x2000  //this is the current edge

//upper 24bits are used for tagging, thus you can only have 8bits for state
//and a total of 16million edges selected at any one time. Enough for anybody?
#define eaTAGMASK 0xFFFFFF00
#define eaTAGSHIFT 0x8

#define eaSELECTIONMASK (eaAFFECTEDBYSELECTION+eaSELECTED+eaHIGHLIGHTED)


//Edge integrety
#define eiOK                   0x0
#define eiVERTEXPARENTSDAMAGED 0x2
#define eiVERTEXNULLS          0x4
#define eiROUGEPOLYGONWARNING  0x8  //not an error but should be aware of it


//polygon attributes
#define paSHADED                    0x1
#define paTEXTURED                  0x2
#define paTRANSPARENT               0x4
#define paADDALPHA                  0x8
#define paTEXTINTERPOLATED          0x10 // 0=perspective texture, 1=interpolated texture
#define paOUTLINED                  0x20 // 0=polygon, 1=vector
#define paDOUBLESIDED               0x40 // 0=single sided, 1=double sided
#define paVISIBLE                   0x80 //it's visible
#define paNOSELECT                  0x100 // 0=selectable, 1=not selectable
#define paENVMAP                    0x200 // 0=standard UV texture, 1=environment mapping
#define paTRUECOLOUR                0x400 // 1=use poly->colour, 0=use light sourcing.
#define paNOSPECTRANS               0x8000
#define paTEXTURETRUE               0x10000

#define paSELECTED                  0x20000
#define paCLIPPEDPOLY               0x40000 // This is a temporary clipped poly; the next pointer points to the original polygon
#define paBACKFACING                0x80000 // This polygon failed the backface test
#define paHASSELECTEDVERTICES       0x100000  // Reserved flag: so that you don't have to check all the vertices each time; set when vertices are selected
#define paALLVERTICESSELECTED       0x200000 // Reserved flag: so that you don't have to check all the vertices each time; set when vertices are selected
#define paCURRENT                   0x400000 // Reserved flag to show that this is the current poly
#define paCONTAINSCURRENTVERTEX     0x800000 // Reserved flag to show that this is the current poly
#define paAFFECTEDBYCURRENTPOLYGON  0x1000000 // Reserved flag to show that this is the current poly
#define paAFFECTEDBYSELECTION       0x2000000 // This is either a selected polygon or connected to one
#define paENABLED                   0x4000000
#define paALLVERTICESVISIBLE        0x8000000
#define paDELETEME                  0x10000000
#define paEXTRUSION                 0x20000000
#define paEXTRUSIONCAP              0x40000000
#define paSELECTIONMASK         (paHASSELECTEDVERTICES+paALLVERTICESSELECTED+paSELECTED+paAFFECTEDBYSELECTION)
#define paSELECTEDVERTEXMASK    (paHASSELECTEDVERTICES+paALLVERTICESSELECTED)
#define paRESERVED_MASK         (paSELECTED+paCLIPPEDPOLY+paBACKFACING+paHASSELECTEDVERTICES+paALLVERTICESSELECTED+paCURRENT+paCONTAINSCURRENTVERTEX+paAFFECTEDBYSELECTION+paENABLED+paVISIBLE+paDELETEME)
#define paATTRIBUTE_MASK        (0xFFFFFFFF^paRESERVED)



//Polygon integrety
#define piOK                   0x0
#define piEDGESDAMAGED         0x1
#define piVERTEXPARENTSDAMAGED 0x2
#define piVERTEXNULLS          0x4
#define piEDGENULLS            0x8
#define piMISSINGEDGEREFERENCE 0x10
#define piSHAREDEDGEREFERENCE  0x20
#define piBADEDGEFLAG          0x40
#define piNOVERTEXREFERENCEINEDGE 0x80
#define piEDGEINWRONGPLACE     0x100


//only used in the EdgeConnect function
#define cmTOPOINT  1            //the first parameter is a single point  (as in a cone)                             
#define cmTOPOINTLIST 0         //both params define a list of verticies (as in a cylinder)



#define naSELECTEDMASK naAFFECTEDBYSELECTION+naREFERENCEDBYCURRENTPOLYGON+naSELECTED;

extern UINT32 elementselected[];
extern UINT32 elementenabled[];
extern UINT32 elementiscurrent[];

//8000

//Edge integrety
#define niOK                   0x0

  
#if defined doublefloatpresicion
  #define boundingboxrangex2 26.0
  #define boundingboxrange 13.0
  #define bndctrlsize 8.0
  #define bndindent 4.0

  
#else
  #define boundingboxrangex2 26.0f
  #define boundingboxrange 13.0f
  #define bndctrlsize 8.0f
  #define bndindent 4.0f

  
#endif

//predefs
class tpolygon;
class t3dmesh;
class tpolyvertex;
class tclipvert;
class tedge;
class material;
//material, embeded with next pointer
class tattribelem:public telem
{
    public:
    UINT32 attribute;
    t3dmesh *parent;
};

#define MATERIALENTRYSIZE 60
#pragma pack(1)
typedef  struct _materialentry
{
    float ar,ag,ab,
           dr,dg,db,
           sr,sg,sb,
           er,eg,eb;    
    float st;
    float t;
    UINT32 sp;
}materialentry;
#pragma pack()

enum t3dtypes
{
	t3dint,
	t3dreal,
	t3dmatrix4x4,
	t3dmaterial,
	t3dvector,
	t3dquaternion,
	t3dmeshcache
};

class basenode
{
public:
	UINT32 attrib;
	virtual UINT32 numinputs();
	virtual UINT32 numoutputs();
	virtual t3dtypes inputtype(UINT32 i);
	virtual t3dtypes outputtype(UINT32 i);
	virtual void  *getoutput(UINT32 i);
	virtual void  *getinput(UINT32 i);	
};



static t3dtypes transform_intypes[]={t3dvector,t3dvector,t3dvector};
static t3dtypes transform_outtypes[]={t3dmatrix4x4};

class transform: public basenode
{

public:

	vector *in_o,*in_p,*in_s;	

	vector int_o,int_p,int_s;
	matrix4x4 int_mat;
	virtual t3dtypes inputtype(UINT32 i){return transform_intypes[i];}
	virtual t3dtypes outputtype(UINT32 i){return transform_outtypes[i];}
	virtual UINT32 numinputs(){return 3;}
	virtual UINT32 numoutputs(){return 1;}
	virtual void  *getinput(UINT32 i){return (void*)(&in_o)[i];}
	virtual void  *getoutput(UINT32 i){return (void*)&int_mat;}
};


typedef enum
{
	BMput=1,
	BMmodulate=2,
	BMadd=3,
	BMsub=4,
	BMalphablend=5,
	BMasint=0xFFFFFFF
}blendmodeenum;

typedef enum
{
	LMemmisive=0,
	LMambient=1,
	LMdiffuse=2,
	LMspecular=3,
	LMasint=0xFFFFFFF
}lightmodelenum;

typedef enum 
{
	UVobjectuv=0,
	UVspheremap=1,
	UVeyeplaneortho=2,
	UVeyeplanepers=3,
	UVreflect=4,
	UVxyplane=5,
	UVxzplane=6,
	UVyzplane=7,
	UVasint=0xFFFFFFFF
}uvmodetype;

#define tmlUSESTEXTURE 1
#define tmlCOLOUR      2

class tmateriallayer:public telem
{
	public:	
	UINT32 attribute;
	UINT32 cachedrendermode;
	textureref *tex;
	uvmodetype uvmode;
	lightmodelenum lightmodel;
	blendmodeenum blendmode; 
	trgbafloat col;
};

class tlayermaterial:public basenode
{
	tlinkedlist layerlist;
	UINT32 attribute;
	


	virtual t3dtypes inputtype(UINT32 i){return transform_intypes[i];}
	virtual t3dtypes outputtype(UINT32 i){return transform_outtypes[i];}
	virtual UINT32 numinputs(){return 3;}
	virtual UINT32 numoutputs(){return 1;}
	virtual void  *getinput(UINT32 i){return (void*)NULL;}
	virtual void  *getoutput(UINT32 i){return (void*)NULL;}
};

class fogplane: public basenode
{
	vector normal,tnormal;
	float min,max;	

};


#define OCTTREE_DEPTH 10
#define OCTTREE_POLYDEPTH 5

    

class material:public telem
{
    public:	
    trgbfloat ambient,diffuse,specular,emmisive;
    
    flt st; 
    flt transparency;
	char name[63];
	UINT8 specpower;
	textureref *tex1;
	//textureref *tex2;
	//textureref *tex3;	

		
    bool operator ==(material &other){return ((ambient==other.ambient)&&
                                          (diffuse==other.diffuse)&&
                                          (specular==other.specular)&&
                                          (emmisive==other.emmisive)&&
                                          (specpower==other.specpower)&&
                                          (transparency==other.transparency));}
    bool operator !=(material &other){return ((ambient!=other.ambient)||
                                          (diffuse!=other.diffuse)||
                                          (specular!=other.specular)||
                                          (emmisive!=other.emmisive)||
                                          (specpower!=other.specpower)||
                                          (transparency!=other.transparency));}
    material *nextm(){return (material*)next;}                                              
};



class tpolygonref:public telem
{
    public:
    tedge *e;  //the edge that 'owns' this reference
    tpolygon *p;
    bool operator ==(const tpolygonref &other){return (e==other.e && p==other.p);}
    tpolygonref(tpolygon *p_,tedge *e_){p=p_;e=e_;}
};

class vertex:public tattribelem
{
    public:
    vector data3d;
	vector4 proj4d;    
    vector normal;        
    vector old;       //used to avoid compound errors when moving a selection.. might be a way around it using selection 'objects'
	vector temp3d;		//this will go
    UINT32 tag;
    tpointflt point2d; //this will become a 4d projected vector
    UINT32 num;
    //tpolygonref *prefs;
    
    tedge *anedge;
    //tclipvert *clipv;
    //void writetag(UINT32 tg){ *((UINT32*)&temp3d.x)=tag;}
    //UINT32 readtag(){ return *((UINT32*)&temp3d.x);}
    void writetag(UINT32 tg){ tag=tg;}
    UINT32 readtag(){ return tag;}        
    vertex *nxt();   
};

class polycyclevars
{
    public:
    UINT32 polycycleindex;
    tpolygonref *curr,*prev;
};

class tedge:public tattribelem
{
    public:
    
    vertex *v[2];
    //tpolygonref *prefs;
    UINT32 tag;
    tpolygon *pref[2];
	//temporary
	vertex *newv;
    
    /*UINT32 edgeloopref,edgeloopcount;*/
    
    tedge(vertex *v1,vertex *v2);
    bool deletereference(polycyclevars *pcv);
    bool deletereferences();
    bool deletereferences(UINT32 attribute);
    bool deletereferences(tpolygon *poly);
	bool replacereference(tpolygon *oldpoly,tpolygon *newpoly);
    tpolygon *addreference(tpolygon *poly);    
    void writetag(UINT32 tag_){tag=tag_;}
    UINT32 readtag(){ return tag;}
    UINT32 checkintegrity();
    UINT32 selectedvertexcount();
	bool isboundary();
    bool sharesvertexwith(tedge *e);
    bool contains(vertex *v);
    UINT32 sameas(tedge *e);
    UINT32 sameas(vertex *v1,vertex *v2);
    int isclockwise();

    UINT32 countreferences(UINT32 attribute=0);    
    tpolygon **startprefcycle(polycyclevars *pcv);
    tpolygon **startprefcycleatrougelist(polycyclevars *pcv);
    tpolygon **getnextpref(polycyclevars *pcv);
    bool findpolygonref(tpolygon *p);

    private:
    bool reclaimfromrougelist();
};



class tclipvert
{
    public:
    vector v,n;
    //tclipvert *v,*v1;
    tedge *e;
    tpointflt t;
    trgbafloatbase c;
    tpolyvertex *oldv; //if the old one isn't outside, don't clip it!
};

/*class tclipedge
{
    public:
    
};*/

class tpolyvertex
{
    public:
    UINT32 attribute;
    tpointflt texv;
	vector normal;
    vertex *v; //vertex at this point
    tedge* e;   //flag in attribute determines if this edge is clockwise or not
    material *mat;
    trgbafloatbase outcolour; //uses up too much memory... :(
    UINT32 checkintegrity();
};




class tplane
{
    public:
    flt distance;
    vector normal;
    UINT32 classifypoly(tpolygon *poly);
	vector *intersect(vector *res,vector *v1,vector *v2);
};



class tclippoly
{
    public:
    UINT8 npoints;
    tclipvert p[256];
};


//offset to tempcentre is important!
class tpolygon:public tattribelem
{
    public:
    vector normal;    
    vector centre;
    #ifdef SHOW_NORMALS
    
    tpointflt c2d,n2d;
    #endif
    
    textureref *tex;
    //material *mat;    
    UINT32 npoints;    
    tpolyvertex vlist[1];
    UINT32 tag;
	UINT8  smoothgroup;
   
	bool isinside(tplane *planes,int count);	
    void setuv(tpointfloat *texv);
    void calcpolynormal();
	void getedgeplane(tplane *plane,tpolyvertex *e);
	void getedgenormal(tpolyvertex *e,vector *v);
    void calccentre();
    void reverse();
    bool removeedgereferences();
    bool iscoplainar(flt threshold);	
    void getplane(tplane *plane);
    void or_vertexattributes(UINT32 attr);//on/off/toggle
    void and_vertexattributes(UINT32 attr);//on/off/toggle
    void xor_vertexattributes(UINT32 attr);//on/off/toggle
    void or_vertexrefattributes(UINT32 attr);//on/off/toggle
    void and_vertexrefattributes(UINT32 attr);//on/off/toggle    
    void xor_vertexrefattributes(UINT32 attr);//on/off/toggle
    void or_edgeattributes(UINT32 attr);
    void and_edgeattributes(UINT32 attr);
    void xor_edgeattributes(UINT32 attr);
    void setmaterial(material *mat,UINT32 mask=0xFFFFFFFF);
    UINT32 countedgeswith(UINT32 mask);
    UINT32 countverticeswith(UINT32 mask);
    UINT32 fixedgeflags();
    bool checkvertexattributes(UINT32 attr);
    UINT32 checkallvertexattributes(UINT32 attr);//returns the amount of vertices who's attributes matched the critera
    bool checkedgeattributes(UINT32 attr);
    tpolyvertex *findedge(tedge *e);
    INT32 findedgeidx(tedge *e);
    INT32 findvertexidx(vertex *v);
    bool contains(vertex *v);    
    bool allverticesclipped();
    bool someverticesclipped();
    bool noverticesclipped();
    vertex *findclosestvertex(flt x,flt y);
    tedge *findclosestedge(flt x,flt y);
	tedge *getedgesafe(INT32 i) 
	{
		i=i%npoints;
		if (i<0) i=npoints+i;		
		return vlist[i].e;
	}
	vertex *getvertexsafe(INT32 i) {return vlist[i % npoints].v;}
	tpolyvertex *getvertexrefsafe(UINT32 i) {return &vlist[i % npoints];}
    
    void copyoldvertices();
    bool containsedge(vertex *from,vertex *to);
    UINT32 checkintegrity();
    tpolygon *nxt(){return (tpolygon*)next;}
};

class tscene;
class tcamera;

class tobject3d:public ttreenode
{
    public:
    char name[32];
    //tpoint cube[8];
    tscene *scene; //parent scene
    UINT32 attribute;
    UINT32 type;
    UINT32 treeattrib;
    vector p,o,s;        //position, orientation, scale. These are in local space, used to reconstruct the matricies
    tpointflt p2d;           //2d projection of centre
    vector min,max;       //defines bounding cube
    flt radius,tempradius;  //maximum radius of the object and it's temporary projection
    trectflt bounds;//objects 2d projected bounds -- VOLATILE
    vector d; //direction
    flt a,spd,mass,maxspeed,minspeed;             //acceleration
    matrix4x4 localmat,worldmat,localmattr,worldmattr; //local space matrix and world space matrix. camera space matrix is not stored.
    //localmattr does not include scaling (for use with normals)
    //the above data is essentially repeated in here in matrix form, both are included for ease of access.
    //perhaps this is all a little too big...
    // generates a 2d clipping rectangle using the current projection;
    char *setname(char *name_);
    void updatedatafull();
    virtual void updatedata();
    virtual trectflt *getbounds(matrix4x4 *,tcamera*);    
    void updatefull();
    void do3dfull(matrix4x4 *cmat ,matrix4x4 *cmattr,tcamera *cam,trender *cmds);
    
    void unitpos();
    void lookat(vector pt,flt twist);
    void lookat2(vector pt,flt twist);
    
    void calcworldmat();  //calculate world space matrix from parent

    virtual void do3d(matrix4x4 *,matrix4x4 *,tcamera*,trender *cmds);
    //virtual void updatedata();  //do AI stuff in here, hapenns inside do3d
    virtual void rotate(flt x,flt y,flt z);
    virtual void scale(flt x,flt y,flt z);
    virtual void moveto(flt x,flt y,flt z);
    void move(flt x,flt y,flt z){moveto(p.x+x,p.y+y,p.z+z);};
    virtual void oncollision();
    virtual void renderhandles(UINT32 attrib,tcamera *cam);
	virtual void rendermouseover(UINT32 level,void *element,tpointfloat *offset,tcamera *cam){;}
	void renderhandlesfull(UINT32 attrib,tcamera *cam);
    
    void enable(bool onoff){(onoff)?attribute|=ofENABLED:attribute&=~ofENABLED;}
    tobject3d *tparent(){return (tobject3d*)parent;}
    tobject3d *tnext(){return (tobject3d*)next;}
    tobject3d *tchild(){return (tobject3d*)child;}    
    
    tobject3d();
    virtual ~tobject3d();        
};

class t3dmesh:public tobject3d
{   
    public:    
    tlinkedlist vertexlist;    
    tlinkedlist polygonlist;
    tlinkedlist edgelist;
    tlinkedlist rougepolygonrefs;

    //nasty little fix for merged vertices.. more complex than anticipated.
    void *tempdata;
    //theapmem memblock;//this is not always needed, some object however need to clear down their memory quickly
    
    t3dmesh();
    virtual ~t3dmesh();
    //presentation methods        
    //  virtual void updatedata();  //do AI stuff in here
	void dumppolygons();
    void normalizescale();
    void destroy();
    virtual trectflt *getbounds(matrix4x4 *,tcamera*);
    virtual void do3d(matrix4x4 *,matrix4x4 *,tcamera*,trender *cmds);
    virtual void renderhandles(UINT32,tcamera *cam);
	virtual void rendermouseover(UINT32 level,void *element,tpointfloat *offset,tcamera *cam);
    void clearvertexandedgeflags();
    void calculatenormals(UINT32 polymask=0xFFFFFFFF,UINT32 vertexmask=0xFFFFFFFF);
    //object construction methods
    void settexture(textureref *,UINT8 selectedonly=1);
    void setmaterial(material *,UINT8 selectedonly=1);
    void setpolyattributes(UINT32 ,UINT8 selectedonly=1);
    void setvertexattributes(UINT32 ,UINT8 selectedonly=1);
	int  removestrandedelements();
	int  removevertices(int mask);
	int  removeedges(int mask);
	int  removepolygons(int mask);
    void and_vertexattributes(UINT32 attribute);
    void or_vertexattributes(UINT32 attribute);
    void and_edgeattributes(UINT32 attribute);
    void or_edgeattributes(UINT32 attribute);
    
    tpolygon *getpolygonhead(){return (tpolygon *)polygonlist.first;}
    tedge *getedgehead(){return (tedge*)edgelist.first;}
    vertex *getvertexhead(){return (vertex *)vertexlist.first;}

    void buildedges();

    vertex *getvertex(UINT32);
    vertex *addvertex(flt,flt,flt);
    vertex *addvertex(flt,flt,flt,vertex *addvertex);//quick version, does not transform with CTM
    vertex *addvertexsimp(flt x1,flt y1,flt z1,vertex *addvertex);
    vertex *addvertex(vector*v){return addvertex(v->x,v->y,v->z);}
    tedge  *addedge(vertex *v,vertex *v1,UINT8 *mem=NULL);    
    tedge *edgesearch(vertex *v,vertex *v1,UINT32 *reversed,tedge **list,INT32 count);	
    tedge *edgesearch(vertex *v,vertex *v1,UINT32 *reversed,tedge *e=NULL, INT32 count=0xFFFFFFFF);
    tedge *addedgeloop(vertex *startv,UINT32 count,UINT8 *mem=NULL);
	tedge *completeedge(tedge *e1,tedge *e2);		        				

	tpolygon*  addpoly(vertex *v,tedge *e,INT32 count,UINT8 *mem=NULL);	
	tpolygon*  addpoly(vertex **v,tedge **e,INT32 count,UINT8 *mem=NULL);
	//creates empty polygon
	tpolygon*  addpoly(UINT32 count,UINT8 *mem=NULL);   
	tpolygon *getpoly(UINT32);

	void edgeconnect(tedge*,tedge*,tpointflt*,UINT32);
    void edgeconnect(tedge*,vertex*,tpointflt*,UINT32);

        
};

class octtreenode
{
public:
	octtreenode *nodes[8];
	tpolygon **plist;	
	UINT32 pcount;
	UINT32 depth;
	vector v1,v2;

	octtreenode();
	
};

class octtree: public tobject3d
{
public:
	octtreenode *root;
	tpolygon *plist[2000];	
	UINT32 totalpolys;

	void build(t3dmesh *obj);
	octtreenode *buildnode(t3dmesh *obj,vector *v1,vector *v2,int depth,tpolygon **polylist);
	octtree();
	virtual void renderhandles(UINT32 attrib,tcamera *cam);
};


#define pcCOINCIDENT 0
#define pcBEHIND 1
#define pcINFRONT 2
#define pcSPANNING 3


class tcamera:public tobject3d
{
    public:
    //define the view fustrum;
    flt FOV;
    flt NEARZ,FARZ,NEARX,NEARY;
    
    UINT8 camtype;
    tcamera();
    virtual trectflt *getbounds(matrix4x4 *,tcamera*);
    tcamera(vector centre_,vector orientation_,flt FOV_);

    virtual ~tcamera();
    virtual void renderhandles(UINT32 attrib,tcamera *cam);
    virtual void rotate(flt x,flt y,flt z);
    virtual void scale(flt x,flt y,flt z);
    virtual void moveto(flt x,flt y,flt z);
    virtual void do3d(matrix4x4 *,matrix4x4 *,tcamera*,trender *cmds);
};


//typedef enum tlighttype{ltCONE=1,ltSPOT=2,ltINF=3}

class tlight:public tobject3d
{
    public:
    UINT8 lighttype;
    trgbfloat diffuse,ambient,specular;
    flt conesize,intensity;
    vector no,wp;  //projected rotation and worldposition
        
    virtual trectflt *getbounds(matrix4x4 *,tcamera*);
    virtual void do3d(matrix4x4 *,matrix4x4 *,tcamera*,trender *cmds);
    virtual void renderhandles(UINT32 attrib,tcamera *cam);
    tlight(vector p,vector o,UINT8 type,trgbfloat *colvals);    
    tlight();    
    virtual ~tlight();    
};



class tlightref:public telem
{
    public:
    tlight *l;
    tlightref(tlight *l_){l=l_;};
    tlightref(){l=NULL;};    
};    

//this class is the 3d cursor, used for many modelling tasks.
//pivotal Clayworks UI concept
class tcursor3d:public tobject3d
{
    public:
    vector wp;
    tcursor3d();
    virtual trectflt *getbounds(matrix4x4 *,tcamera*);
    virtual void renderhandles(UINT32 attrib,tcamera *cam);
    virtual void do3d(matrix4x4 *,matrix4x4 *,tcamera*,trender *cmds);
};

class tgrid3d:public tobject3d
{
    public:
    flt tempgridx,gridx,tempgridy,gridy,gridz;    
    vector p;

    tgrid3d();
    UINT32 snapto(vector *v);
    flt snapto(flt f);
    tgrid3d(float gridspacex,float gridspacey,float gridspacez);
    virtual trectflt *getbounds(matrix4x4 *,tcamera*);
    void drawgrid();
    virtual void do3d(matrix4x4 *,matrix4x4 *,tcamera*,trender *cmds);
};

//This class represents a bounding box
//The editscene class contains at least 3 of these objects 
//this class needs major revision; It should be aware of extrusion levels some how and
//provide manipulation tools for those

typedef struct _elementstruct
{
    union
    {
        struct
        {
            vertex *vertex;
            tedge *edge;
            tpolygon *polygon;
            tobject3d *object;
        };
        tattribelem *elements[4];
    };
    bool operator !=(const _elementstruct &other){return (vertex!=other.vertex || edge!=other.edge || polygon!=other.polygon || object!=other.object);}
    void clear(){vertex=NULL;edge=NULL;polygon=NULL;object=NULL;};
    bool anynotnull(){return vertex || edge || polygon || object;};
}elementstruct;



class tscene
{
    public:
   
    UINT32 polyattribmask;//attribute
    ttree scenegraph;
    tlinkedlist tlights;
    vector cursor;
    tgrid3d *grid;
    flt fogminz;            //near fog boundry
    flt fogmaxz;            //far fog boundry
    flt fogdepth;           //depth of fog
    flt fogdensity;
    trgbfloat fogcolour;
    void setfog(flt,flt,flt);    
    
    tobject3d *addobject(tobject3d* parentobject,tobject3d* newobject);    
    void removeobject(tobject3d* object);
	elementstruct curr;
    
    tscene();
    virtual ~tscene();       
    //calculation for the rgb intensity of a point at position p and normal n
    void intensity(vector &n,vector &p,vector &v,material &m,trgbafloatbase &result);
    //perform clipping and other stuff in here
    //retrieves the bounding box for an object with minimal calculation invloved
    void setupview(tcamera *cam,UINT16 offx=0,UINT16 offy=0);
    trectflt *getbounds(tcamera *cam,tobject3d *obj,UINT16 offx=0,UINT16 offy=0);
    void preprocess(tcamera *cam,UINT16 offx=0,UINT16 offy=0);
    void preprocess_singleobject(tcamera *cam,tobject3d *obj,UINT16 offx=0,UINT16 offy=0);
    //perform rasterization into the current display context
    void render();     //full on dogs bollocks render
    void renderwire(); //tiddley wires; should use funky hash table thing to determine in lines have already been drawn
    void projectlights(matrix4x4 *mat);
	void setdisplay(tsurface *surf);
};    

//GLOBALS, mostly concerned with object creation, a bit like openGL.
//In other words, it's not badly coded, it's just a state machine!
extern UINT32 T3Dmaxpolyedges,T3Dmaxdrawnedges;
extern tcolour T3DCOLprojector;
extern tcolour T3DCOLboundingbox;
extern tcolour T3DCOLselectedwire;
extern tcolour T3DCOLwire;
extern tcolour T3DCOLnode;
extern UINT32 T3Dsceneattributes,T3Dpolycreationflags,T3Ddefaultpolyattributes,T3Ddefaultobjectattributes;
extern UINT32 T3Ddefaultedgeattributes,T3Ddefaultvertexattributes;
extern flt T3Dscale;
extern tpolygon *T3Dselectedpoly;
extern tlinkedlist T3Dtexturebank;
extern tlinkedlist T3Dmaterialbank;
extern matrix4x4 *T3Dctm;
extern textureref *T3Dtexture;
extern material *T3Dmaterial;
extern UINT32 T3Dpolycount;
extern vector T3Dlookv;
extern tpointflt T3Doffset;
extern vector T3Dbbcentreofrotation;
extern matrix4x4 T3Dviewmat;
extern flt T3Dresistance;
extern UINT32 T3Dobjecttypemask;
extern UINT32 T3Dobjectprocessmask;
extern UINT32 T3Dobjectprocessmaskresult;
extern UINT32 T3Dedgeprocessmask;
extern UINT32 T3Dedgeprocessmaskresult;
extern UINT32 T3Dpolygonprocessmask;
extern UINT32 T3Dpolygonprocessmaskresult;
extern UINT32 T3Dvertexprocessmask; //if a bit test between the object attribute and this mask !=T3Dobjectprocessmaskresult, process the object
extern UINT32 T3Dvertexprocessmaskresult;
extern tpolygon **T3Dvisiblepolygons;
extern UINT32 T3Dpolygonclipped;
extern tedge **T3Dvisibleedges;
//this is where the 'safe' edge adding routines starts looking for
//previously added edges

//VARIABLES DECLARED IN CLAYCP.CPP
extern tpointflt T3Dscreenscale,T3Dscreencentre; 
extern trectflt T3Dviewport;

extern char TEXTURE_PATH[255];
extern char T3D_TEXTURE_PATHS[1024];
extern char TEXTURE_PATH[255];
extern char CURRENT_MODEL_PATH[255];


//Global library calls
bool T3Dproject(vector *p3d,tpointflt *p2d,tcamera *cam);
void T3Dbuildplanecube(tplane *planes,vector *v1,vector *v2);
UINT32 T3Dinit(UINT32 polyblocksize,UINT32 tempspacesize);
void T3Dprintstats(INT16 x,INT16 y);
void T3Dresetbuffers();
void T3Dshutdown();
tsurface *T3Dsettexture(textureref *tref,tsurface *pic);
void T3Dsettexture(textureref *pic);
textureref *T3Dgettextureref(UINT32 idx);

material *T3Dfindmaterialbyname(char *name);
void     T3Ddeletematerial(material *mat);
material *T3Daddmaterial();
material *T3Daddmaterial(materialentry *mat);
material *T3Daddmaterial(trgbfloat a,trgbfloat d,trgbfloat s,trgbfloat e,UINT8 specpower,flt t);
material *T3Daddmaterial(trgbfloat &d,UINT8 specpower,flt t);
material *T3Daddmaterial(material *mat);
material *T3Daddmaterialifnotfound(material *mat,bool makenew);
textureref *T3Daddtexture(tsurface *pic,char *name);
//textureref *T3Daddtexture(textureref *tref);
extern void (*T3Daddtexturecallback)(textureref *tref);
extern void (*T3Daddmaterialcallback)(material *mat);

extern tedge *T3Dcreateedge(vertex *v,vertex *v1,UINT8 *mem);
extern tpolygon *T3Dcreatepolygon(tpolygon *inpoly,UINT8 *mem);
extern tpolygon *T3Dcreatepolygon(UINT32 npoints,UINT8 *mem);
extern tpolygon *T3Dreallocpoly(tpolygon *p,UINT32 npoints);

extern tpolygon *T3Daddvertextopolygon(tpolygon *p,vertex *v,tpointflt *texv);
extern tpolygon *T3Dremovevertexfrompolygon(tpolygon *p,vertex *v);
extern tpolygon *T3Dsetpolygonvertex(tpolygon *p,vertex *v,UINT32 ref,UINT32 attribute,tedge *e=NULL);
//only use this if all information is known
//extern tpolygon *T3Dsetpolygonvertex(tpolygon *p,tedge *e,UINT32 ref,UINT32 attribute);
extern tpolygon *T3Dsetpolygonvertexuv(tpolygon *p,UINT32 ref,flt u,flt v);
extern UINT32 T3Dgettextureindex(textureref *tex);
extern UINT32 T3Dgetmaterialindex(material *mat);


//delete callbacks
bool removevertexfunc(telem *celem);
bool removeedgefunc(telem *celem);
bool removepolygonfunc(telem *celem);
//checks
bool checkremovevertexfunc(telem *celem);
bool checkremoveedgefunc(telem *celem);
bool checkremovepolygonfunc(telem *celem);




//these functions add primatives to the basic shape
//as I want a "parametric object representations until otherwise screwed" scheme, more work may need to be done
//on the placement of these routines.
tedge *mksquare(t3dmesh*,flt,flt,flt,flt,flt);
tedge *mkcircle(t3dmesh*,flt cx,flt cy,flt cz,flt rx,flt ry,flt angle,int edges);
tedge *mksuperelipse2d(t3dmesh*,flt cx,flt cy,flt cz,flt rx,flt ry,flt cval1,int edges,flt n1,flt n2);                                                                                                          
vertex *mkcube(t3dmesh*,flt x1, flt y1,flt z1,flt x2, flt y2,flt z2);
vertex *mkcylinder(t3dmesh*,flt x1, flt y1,flt z1,flt z2,flt rx, flt ry, flt angle,int edgecount);
vertex *mkring(t3dmesh*,flt x1, flt y1,flt z1,flt rx, flt ry,flt rx2, flt ry2,flt angle,int edgecount);                                     
vertex *mksphere(t3dmesh*,flt x1, flt y1,flt z1,flt rx, flt ry, flt rz, flt angle,int radial,int laterial);
//super quadratics are sphereoid like shapes
//only their method of creation uses a few more paramters (powers of sin and cos, parameters n1 and n2)
//used to create more interesting shapes
//note that superelipse2d works like the mkcircle command only the 'angle' parameter controls the depth 
                                   
vertex *mksuperelipse(t3dmesh*,flt x1, flt y1,flt z1,flt rx, flt ry, flt rz, flt angle,int radial,int laterial,flt n1,flt n2);
vertex *mkcone(t3dmesh*,flt x1, flt y1,flt z1,flt z2,flt rx, flt ry,flt angle,int edgecount);
vertex *mkgrid(t3dmesh *obj,flt x1,flt y1,flt x2,flt y2,flt z,int gridx,int gridy,int checkergrid);


void cleanupvertex(telem *v);
void cleanupedge(telem *v);
void cleanuppolygon(telem *v);

void T3Dsortbymaterial(INT32 l,INT32 r);


#endif
