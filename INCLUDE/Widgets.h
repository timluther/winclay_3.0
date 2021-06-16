#ifndef widgets_h
#define widgets_h

//window creation flags

#define windowdrawborder 1
#define windowdrawall 2

#define tabdrawborder 1
#define tabdrawselected 2
#define tabdrawchange 3
#define tabdrawall 4

#define CFxbar 1
#define CFybar 2
#define CFstatusbar 4
#define CFclosebutton 8
#define CFminimizebutton 16
#define CFclientedge 32
#define CFnoresize 64
#define CFshadow 128
//dropbox drawing modes
#define DBtopbit 3
#define DBtext 1
#define DBbutton 2
#define DBlist 4


//standard windows commands
#define CMDclosewindow 80
#define CMDmaximize 81
#define CMDminimizewindow 82
//commands from container classes
#define CMDxdeltachanged 90
#define CMDydeltachanged 91
#define CMDxdeltavalchanged 92
#define CMDydeltavalchanged 93
#define CMDxvalchanged 94
#define CMDyvalchanged 95
#define CMDvalchanged 96
#define CMDmakebarsinvisible 97
#define CMDmakebarsvisible 98
#define CMDchildunfocused 99
#define CMDdragselectstopped 100

//edit string commands
#define CMDeditstringaltered 131
#define CMDeditreturnpressed 132
//valuetype commands
#define CMDincval 110
#define CMDdecval 111
#define CMDselectedchanged   112
#define CMDitemclicked      113
#define CMDitemdclicked      114
#define CMDtoggledrop 115
#define CMDtabchanged 116
#define CMDcontainermodalended 120 //called when the modal mode of a container is ended by a click outside the view
#define CMDnumboxreleased 121
#define CMDdropboxclosed 122
#define CMDnumboxpressed 123
#define CMDdroppaletteclosed 124
#define CMDtoolbarreleased 125


#define LEselected 1
#define LEdisabled 1

#define listdrawall 1    //draws list, and sliderbars
#define listdrawchange 2  //draws the change in selection
#define listdrawselected 3 //just draws the selected item(s)
#define listdrawafter 4   //draw items after the 'drawafter' pointer
#define listdrawlist 5   //draws the whole list, but not the sliders or the border

#define menudrawall 1
#define menudrawchange 2



        
#define ETeditindent_y 3
#define ETeditindent_x 8
#define ETeditindex_x2 16

// Textbox attributes
#define TBnumeric 1  //is a number, no text shit allowed
#define TBfloat   3  //it's a float so one period allowed, and the exponential symbol
#define TBexternalstring 4




#define BMcheckboxcircle 5
#define BMcheckboxsquare 6
#define BMsquarebutton 7

//button attributes
#define BAdown 1  //don't change this whatever you do!
#define BAdrawonlyborder 2
#define BAstatechange 4
#define BAhighlightedborder 8
#define BAnobox 0x10
#define BAnooffset 0x20
#define BAxcentre 0x40
#define BAycentre 0x80
#define BAcentre BAxcentre|BAycentre

//button modes//
#define BMmask  0xFF00
#define BMclickaction 0x100
#define BMreleaseaction 0x200
#define BMperiodaction 0x400
#define BMtoggleaction 0x800
#define BMglobaltoggleaction 0x1000
#define BMbothtoggleaction 0x2000
#define BMmoveaction 0x4000
#define BMtogglebutton BMtoggleaction|BAnooffset|BAycentre|BAnobox



//Number sel attributes
#define NSdown 1  //don't change this whatever you do!
#define NSup 2
#define NSsliding 3
#define NSgetvalfromtext 4 //on next setrange, get the value from the text box before
						   //performing operations. The flag is then reset.

//slider attributes
#define SAxorient 1
#define SAmodalslide 2
#define SAdontdrawbuttons 4

//Toolbutton states
#define TBAdown          0x1       //button is down don't change this whatever you do!
#define TBAhighlighted   0x2    //button is highlighted, draw as appropriate
#define TBAseperator     0x4  //has no action, just a spacer
#define TBAdrawbox       0x8    //draw a box behind the image
#define TBAglobaltoggle  0x10    //if clicked, others with this flag will turn off
#define TBAtoggle        0x20    //local toggle button
#define TBAmoveaction    0x40    //move action button
#define TBAaltselect  0x80    //alternative selection style, an outer rectangle is drawn


//Menu states
#define MApopup 1
#define MAxorient 2
#define MAshadow 4
#define MAactive 8
//Menu item state
#define MIAchecked 1
#define MIA3dlook 2
#define MIAdisabled 4
#define MIAseperator 8


//Tree view states
#define squaresize 3
#define TEselected 1
#define TEdisabled 2
#define TEcollapsed 4

//valuetype flags. These are the high order bits, low order flags
//are used by inherited classes.
#define VTfloat 0x1000000 
//valuetype return flags
#define VTvalchanged   0x1
#define VTminchanged   0x2
#define VTmaxchanged   0x4
#define VTrangechanged 0x6



typedef struct _stackelem
{
    UINT8 op;
    union
    {
       UINT32 data;
       UINT32 spacer;
       union
       {       
       trgbabase colour;
       UINT32 colourwrd;
       };
       tfont *font;
       char *hyperlink;
    };
}stackelem;

typedef struct _textstate
{
    public:
    UINT32 attribute;
    union
    {       
       trgbabase colour;
       UINT32 colourwrd;
    };
    tfont *font;        
}textstate;

class richtext:public tgroup
{
    public:
    //have a rectlist of hotspots
    //and a list of bitmaps    
    tpoint offset;
    tpoint c;      //current starting coordinates in relative units
    char *linestart; //start of current line    
    textstate state,topstate;
    stackelem stack[512],*stacktop;
    trgbabase backgroundcolour,linkcolour,defaulttextcolour;
    richtext(INT32 x,INT32 y,INT32 w,INT32 h,char *string_);
    void push(UINT8 op,UINT32 data);
    stackelem *pop();
    virtual void draw();
    virtual void handleevent(tevent *event);
    char *string;
    void setoffset(tpoint offset);
    void calculatexbounds(char *txt,bool changestate);
    void calculatestart();

    private:
      UINT32 xminbounds,xmaxbounds;
      char *nextbreak;
};


class tmenu;

class tmenuitem:public telem
{
    public:
    char text[20];
    tpoint size;
    UINT8 shortkey;
    tsurface *bitmap;
    UINT32 attrib;
    UINT32 command;
    tmenu *submenu;    
};

class tmenu:public tview
{
    public:
    tsurface *sideimage;
    tmenu *parentmenu;
    tmenuitem *sel,*oldsel;
    tlinkedlist data;
    UINT32 mattrib;
    UINT8 drawmode;
    tmenu(INT32 x,INT32 y,UINT32 attrib_=0,tsurface *sideimg=NULL);
    ~tmenu();
    tmenuitem *additem(char *text,UINT32 command,UINT32 attrib=0,tmenu *submenu=NULL,tsurface *bm=NULL,UINT8 shortkey_=0);
    bool removeitem(tmenuitem *item);
    void drawitem(INT32 xpos,INT32 ypos,INT32 w,INT32 h,tmenuitem *item);
    void selectitem(tmenuitem *item);
	bool insideitems(INT32 x,INT32 y);
    tmenuitem *getitematpos(tpoint p);
    bool setitem(tmenuitem *item,bool askey);
    bool opensubmenu(tmenuitem *item,bool askey);
    //closes any submenus in the menu then gets rid of itself
    tmenu *findopenleaf();
    tmenu *findroot();
    void closesubmenus();
    tmenuitem *getprevitem(tmenuitem *item);
    tmenuitem *getnextitem(tmenuitem *item);
    tpoint getitempos(tmenuitem *item);
    void colapse(UINT8 docommand,tmenu *submenu,tmenu *stopat);
    virtual void draw();
    virtual void handleevent(tevent *event);
    virtual bool setstate(UINT32 astate, bool enable);        
};


class ttoolbutton:public telem
{
    public:
    UINT32 command;
    UINT32 attribute;
    UINT32 image;
	char *text;
	UINT32 action(){return attribute & BMmask;}
    
    ttoolbutton(UINT32 at,UINT32 img,UINT32 cmnd,char *text);
};

class ttoolbar:public tview
{
    public:
    tlinkedlist buttons;    
    ttoolbutton *lasttoggledown,*modalbutton;  //the last global toggling button that was pressed
    tsurface *images;
    tpoint bsize;
    INT32 rows,cols; //calculated internally
    INT32 numimages;
    ttoolbar(INT32 x,INT32 y,INT32 w,INT32 h,tsurface *imgs,UINT32 numimgs,UINT32 btnx,UINT32 btny);
    void addbutton(UINT32 attribute,UINT32 image,UINT32 command,char *text);
    bool switchbutton(ttoolbutton *button);
    bool switchbuttonbyref(UINT32 buttonref);
    ttoolbutton *getbuttonat(UINT32 x,UINT32 y); //in screen coordinates
    
    void calculatedimensions();//calculate rows and collums based on size of toolbar and the size of the buttons                     
	virtual void changebounds(trect &bounds);
    virtual void handleevent(tevent *event);
    virtual void draw();      
    virtual bool setstate(UINT32 astate, bool enable);        
};



class tbutton:public tview
{
  public:
  UINT32 dstate,style,hotkeycode;  
  UINT32 docommand;
  char *text;

  tsurface *surf;
  tblitrect *buttons;

  tbutton(INT32 x,INT32 y,INT32 w,INT32 h,UINT32 _dstate,UINT32 _docommand,UINT8 hotkey,char *txt=NULL,tsurface *surf_=NULL,tblitrect *buttons_=NULL);
  tbutton(INT32 x,INT32 y,UINT32 _dstate,UINT32 _docommand,UINT8 hotkey,tsurface *surf_,tblitrect *buttons_);
  tbutton(INT32 x,INT32 y,UINT32 _dstate,UINT32 _docommand,char *txt);
    
  virtual void handleevent(tevent *event);
  virtual void draw();      
  virtual bool setstate(UINT32 astate, bool enable);
  int  isdown(){return dstate & BAdown;}

  
  int ismouseover(){return state & SFmouseover;}
  int issel(){return state & SFselected;}
  int nobox(){return dstate & BAnobox;}
  int xcentre(){return dstate & BAxcentre;}
  int ycentre(){return dstate & BAycentre;}
  
  void setdown(){dstate|=BAdown;drawview();}
  void setup(){dstate&=~BAdown;drawview();}
  UINT32 action(){return dstate & BMmask;}
  
};

class tcollection:public tgroup
{
    public:
    tbutton *lastselected;
    tcollection(INT32,INT32,INT32,INT32);
    ~tcollection();
    virtual void draw();
    virtual void handleevent(tevent *event);
};

class fiunion
{
	public:
	union
	{
		UINT32 ival;
		float  fval;
	};

};

class valuedata
{
    public:
	union
	{
		struct {INT32 val,min,max;};
		struct {float fval,fmin,fmax;};
	};
	void seti(UINT32 val_,UINT32 min_,UINT32 max_){val=val_;min=min_;max=max_;}
	void setf(float val_,float min_,float max_){fval=val_;fmin=min_;fmax=max_;}

};

class valuetype:public tgroup
{
    public:
	UINT32 attrib;
    valuedata data;    
   
    valuetype(valuedata *data,UINT32 attrib_);
    UINT32 setvalmute(INT32 val_);
    UINT32 setval(INT32 val_);
	UINT32 setvalmutef(float val_);
    UINT32 setvalf(float val_);
	UINT32 setrangemute(valuedata *data);
    virtual UINT32 setrange(valuedata *data);
	
	void setmax();
	void setmin();
};


class slidertype:public valuetype
{
    public:
    INT32 s1,s2,offset;
    INT32 buttonsize;    
    
    UINT32 updatecommand;
    
    slidertype(INT32 x,INT32 y,INT32 l,valuedata *data,UINT32 attrib_,UINT32 updatecommand_=CMDvalchanged);

    virtual void handleevent(tevent *event);
    virtual void draw();
    virtual void changebounds(trect &bounds);
    virtual UINT32 setrange(valuedata *data);
    //private:
    void getsrect(trect &srect);
    void calcslidepos();
};


class edittextbox:public tview
{

    UINT8 tbattrib;
    UINT8 insertmode;
    INT32 curspos,curslen;
    //  start offset          string length
    INT32 off,xlen,slen,strmem,lastchar;
    INT32 cp1,cp2;     //cp1 & cp2 are the positions of the selection in pixels.
                                //lastchar is the index of the last character in the current visible field
    INT32 c1,c2; //cx are character index versions of cpx and lastchar is the index of the last visible character in the textbox
      
    public:
    char *txt;
    char *vistext;

    
    edittextbox(INT32 x,INT32 y,UINT8 attr,char *txt_,INT32 slen_,INT32 w);
    edittextbox(INT32 x,INT32 y,UINT8 attr,char *txt_,INT32 slen_,INT32 w,INT32 h);
    ~edittextbox();
    void calcinternalvars();
    //~edittextbox();
    virtual void draw();

    virtual void handleevent(tevent *event);
    virtual bool setstate(UINT32 astate, bool enable);
    char setcursorpos(INT32 pos,char followtail);
    void settext(char *string,bool selectall=false);
    //for numeric processing
    bool setval(UINT32 val);
    bool setvalfloat(float val);
    float getvalfloat();
    UINT32 getvalint();        
    void setinsertmode(UINT8 mode){insertmode=mode;}
    void selectall();
    
    void inserttext(INT32 pos,char *txt_,INT32 length);
    void deletetext(INT32 pos,INT32 length);
    
};


class numbox:public valuetype
{
    public:
	fiunion stepsize;
    edittextbox *edit;
    UINT32 updatecommand;
    
    numbox(INT32 x,INT32 y,INT32 w,fiunion stepsize,UINT32 updatecommand_,valuedata *data,UINT32 attrib);
    ~numbox();
    virtual UINT32 setrange(valuedata *data);
    virtual void handleevent(tevent *event);
    virtual void draw();
	void dec();
	void inc();
    
};

typedef struct listelembase_
{
    char *txt;    
    UINT32 attrib;
    UINT32 contextattrib;
    INT32 offset;
    UINT8 image;
}listelembase;

class listelem:public listelembase
{
    public:	
    void set(char *txt_,INT32 offset_,UINT32 attrib_,UINT32 contextattrib_,UINT8 image_);
    
};

class scrollcontainer:public tgroup
{
    public:
    UINT32 attrib;
    tview *view;
    slidertype *xbar,*ybar;
    scrollcontainer(INT32 x1,INT32 y1,INT32 x2,INT32 y2,tview *view_,UINT32 attrib=3);
    virtual void draw();      
    
    virtual void handleevent(tevent *event);    
};


class listbox:public tview
{
  public:
  INT32 maxopts,maxylen,oldsel,sel,off;
  UINT8 drawmode;
  bool destroyold,changed;
  tsurface *images;
  INT32 imgwidth;
  void (*rightclickfunc)(listbox *view,tevent *event);  
  listelem *info;

  listbox(INT32 x,INT32 y,INT32 w,INT32 h,listelem *info_,INT32 maxopts_,bool destroyold_);
  ~listbox();
  listelem *getsel(){return &info[sel];}
  char *getseltext(){return info[sel].txt;}
  UINT32 getselimageindex(){return info[sel].image;}
  UINT32 getselattrib(){return info[sel].attrib;}
  UINT32 getselcontextattrib(){return info[sel].contextattrib;}
    
  void makelist(INT32 nopts,listelem *list,char drawit=TRUE);
  void setitem(INT32 opt,char *txt,INT32 offset,UINT32 attrib,UINT32 contextattrib,UINT8 img);
  void setlist(INT32 nopts,listelem *list,char drawit=TRUE);
  void kill_list(char drawit=TRUE);   
  void setval(INT32 val_);
  void drawitem(INT32 itemno);
  bool find(INT32 from,char *txt);
  void sortlist(INT32 from,INT32 to);
  INT32 calcylen();
  void changebounds(trect &bounds);
  void calcinternalvars();
  virtual void handleevent(tevent *event);
  virtual void draw();      
  virtual bool setstate(UINT32 astate, bool enable);
};

//{*===================================*}
//{*        -*Tree box  type*-         *}
//{*===================================*}

typedef void (*gettreeitemdatafunc)(ttreenode *item,char **txt,tsurface **pic,UINT32 **col);

class treebox:public tview
{
    public:
    ttree *tree;
    ttreenode *oldsel,*currsel,*ref;
    UINT8 drawmode;
    INT32 height;
    trect irect;
    INT32 off;
    gettreeitemdatafunc gettreeitemdata;
    void update();
    UINT32 *getattrib(ttreenode *node);
    treebox(INT32 x,INT32 y,INT32 w,INT32 h,ttree *tree,gettreeitemdatafunc gettreeitemdata_);
    ~treebox();
    INT32 calcylen();
    void killtree(ttreenode *node,char drawit=TRUE);    
    void setbarrange();
    ttreenode *getnextitem(ttreenode *celem);
    ttreenode *getprevitem(ttreenode *celem);
    void setsel(ttreenode *newelem);
    void clearsel();
    void drawitem(ttreenode *elem,INT32 ypos,bool justtext=FALSE);
    ttreenode *getitem(INT32 y);
    INT32 getypos(ttreenode *elem);    
    virtual void changebounds(trect &bounds);
    virtual void draw();
    virtual void handleevent(tevent *event);
    virtual bool setstate(UINT32 astate, bool enable);
};

class tabelement:public telem
{
    public:
    char name[32];
    tview *view;
    tsurface *d,*u;
};

class tabbox: public tgroup
{
    public:
    UINT8 drawmode;
    tabelement *rootelem,*seltab;    
    tabbox(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
    ~tabbox();
    tview *getseltabview(){return seltab->view;}
    UINT32 getseltabviewid(){return seltab->view->pid;}
    void drawtab(tabelement *ctab,INT32 x1,INT32 x2);
    virtual void draw();
    virtual void handleevent(tevent *event);
    //virtual void changebounds(trect &bounds);
    void addtabcontrol(tview *view,char *name,UINT32 id=PIDundefined);
    virtual bool setstate(UINT32 state, bool enable);
};   
    


/*
bordertype=^bordertype_;
bordertype_=object(tview_)
  size:byte;
  Constructor create(xa,ya,xb,yb:integer;size_:byte);
  Procedure draw;virtual;
end;

*/



/*class ttoolbutton:public telem
{
    public:
    UINT32 command;
    UINT32 attribute;
    UINT32 image;
	char text[32];
	UINT32 action(){return attribute & BMmask;}
    
    ttoolbutton(UINT32 at,UINT32 img,UINT32 cmnd,char *text);
};*/
class droppalette:public tview
{	
public:
	ttoolbar *list;
	UINT8 drawmode;
	droppalette(INT32 x,INT32 y,tsurface *imgs,UINT32 numimgs,UINT32 btnx,UINT32 btny);
    ~droppalette();
    virtual void handleevent(tevent *event);
    virtual void draw();
    virtual void changebounds(trect &bounds);
    ttoolbutton *getsel(){return list->lasttoggledown;}
    char *getseltext(){return list->lasttoggledown->text;}
	void setsel(UINT32 i)
	{
		list->switchbuttonbyref(i);
	}
    UINT32 getselimageindex(){return list->lasttoggledown->image;}
    UINT32 getselattrib(){return list->lasttoggledown->attribute;}
    UINT32 getselcommand(){return list->lasttoggledown->command;}
	void addbutton(UINT32 attribute,UINT32 image,UINT32 command,char *text)
	{
		list->addbutton(attribute|TBAglobaltoggle,image,command,text);
		trect irect;
		irect.rassign(0,0,list->bsize.x,list->bsize.y*list->buttons.count);
		list->changebounds(irect);

	}
    void toggledrop();
};


class dropbox:public tview
{
    public:    
    //edittextbox text;
    tbutton *dropbutton;
    listbox *list;
    scrollcontainer *container;
	UINT8 drawmode;
    dropbox(INT32 x1,INT32 x2,INT32 y1,listelem *info_,INT32 maxopts_);
    ~dropbox();
    virtual void handleevent(tevent *event);
    virtual void draw();
    virtual void changebounds(trect &bounds);
    listelem *getsel(){return list->getsel();}
    char *getseltext(){return list->getseltext();}
    UINT32 getselimageindex(){return list->getselimageindex();}
    UINT32 getselattrib(){return list->getselattrib();}
    UINT32 getselcontextattrib(){return list->getselcontextattrib();}
    void toggledrop();
};

class window:public tgroup
{
    public:    
    UINT8 drawmode,resizemode;
    char *text;
    tbutton *close,*min,*max;
    slidertype *xbar,*ybar;
    tview *view;   //should know how to handle events from xbar and ybar
    window(INT32 x,INT32 y,INT32 w,INT32 h,char *txt,tview *cview,UINT32 createflags);
	UINT32 getresizemode(INT32 x,INT32 y);
    

    
    virtual void draw();
    virtual void handleevent(tevent *event);
    virtual bool setstate(UINT32 state, bool enable);
};

class bordercontainer:public tgroup
{
    public:
    bordercontainer(INT32 x,INT32 y,INT32 w,INT32 h,tview *inner);
    bordercontainer(INT32 x,INT32 y,tview *inner);        
    virtual void draw();
    virtual void handleevent(tevent *event);
};


////////////////////////////////////////////
//external function declarations

//extern void drawclientedge(INT32 x1,INT32 y1,INT32 x2,INT32 y2,windowgfx *wg);
extern void drawclientedge(INT32 x1,INT32 y1,INT32 x2,INT32 y2,tblitrect *c,tblitrect *h,tblitrect *v,tsurface *base);
extern void drawouteredge(INT32 x1,INT32 y1,INT32 x2,INT32 y2,windowgfx *wg);
extern UINT32 findlongesttext(listelembase *list,UINT32 nopts);
void textboxb(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void textboxfilledb(INT32 x1,INT32 y1,INT32 x2,INT32 y2);


#endif
