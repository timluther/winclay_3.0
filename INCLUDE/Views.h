
#ifndef views_h
#define views_h

//Windowing Basics header
/*Notes:
a.) Events must NOT be called in creation of objects.
b.) Make sure that the root object never tries to cal it's parent
Non-group objects must allways be owned by a group | procedures
at null pointers may be called resulting in a crash.
c.) ALL drawing operations nust be called in a view's 'draw' function
with possible case statments if only some things need to be drawn.
d.) make sure all possible command events are handled in some way,
avoiding misinterpertation(?!@#) of commands.
e.) The global 'root' must always be set using the command 'inittwin'
f.) The global 'global_select' is also important but it is handled solely
within the code of this unit.
*/

#define TWINmodalstacklimit	    20
//commands to container to sliders
#define CMDclose			    1

//sent to a view when it's interted into another view
#define CMDinitialupdate	    95


//window display attributes.. controls such things as the positioning of the title text on the window
#define TWtitletextafterindent  0x1
#define TWtitletextcentrex	    0x2    //plonks the text in the middle.. needs a custom skin!
#define TWtitletextcentrey	    0x4
#define TWblendedges		    0x8          //future implementation.. edges of window bitmaps are blended together

#define TSvisible			    0x1        //global visibility. If not set, nothing will be drawn!
#define PIDundefined			0xF000BAAA /*for views that don't want to be individuals*/

/*option flags*/
#define OFselectable		    0x0001 
#define OFnooverlap			    0x0002
#define OFaltkey                0x0004
#define OFxorient			    0x0008
#define OFpopup					0x0010
#define OFtopselect				0x0020
#define OFautoraise				0x0040
#define OFsoliddrag				0x0080
#define OFkeygrab				0x0100
#define OFclicktopselect		0x0200
#define OFparentresize			0x0400
#define OFtabcycleselect		0x0800
#define OFautofocus				0x1000
#define OFclientedge			0x2000
#define OFnoresize				0x4000
#define OFdisturable			0x8000
#define OFownerchangebounds		0x10000
#define OFtoplevelalien			0x20000
#define OFchunkyresize			0x40000


/*status flags*/
#define SFvisible				0x0001 
#define SFselected				0x0002 
#define SFfocused				0x0004 
#define SFdrawnonce				0x0008 
#define SFdisabled				0x0010 
#define SFmodal					0x0020 
#define SFexposed				0x0040 
#define SFmute					0x0080
#define SFreserved				0x0100
#define SFdisturbed				0x0200
#define SFmouseover				0x0400
#define SFdeaf					0x0800
#define SFwaitingforrelease		0x1000
#define SFenabled				0x2000
#define SFdragging				0x4000
#define SFresizing				0x8000


/*Grow mode constants*/
#define TGgrowx1				0x01 
#define TGgrowy1				0x02 
#define TGgrowx2				0x04 
#define TGgrowy2				0x08 
#define TGdragonx1				0x10   /*Only  equiv of the changed side will move, not it's opposite*/
#define TGdragony1				0x20
#define TGdragonx2				0x40   /*Only  equiv of the changed side will move, not it's opposite*/
#define TGdragony2				0x80 
#define TGdragx1				TGgrowx1+TGdragonx1 
#define TGdragy1				TGgrowy1+TGdragony1 
#define TGdragx2				TGgrowx2+TGdragonx2 
#define TGdragy2				TGgrowy2+TGdragony2 
#define TGrelative				0x40

#define TGgrowallx				TGgrowx1+TGgrowx2 
#define TGgrowally				TGgrowy1+TGgrowy2 
#define TGgrowall				TGgrowallx+TGgrowally 
#define TGdefault				0xDEFA000 

/*TGrowLoX   = right;
TGrowHiX   = left;
TGrowLoY   = bellow;
TGrowHiY   = above;
TGrowRel   = 0x10;*/

/*Event constants*/
#define EVmousedown				0x0001  /* Mouse button depressed */
#define EVmouseup				0x0002  /* Mouse button released  */
#define EVmousemove				0x0004  /* Mouse changed location */
#define EVmousedrag				0x0008  /* Periodic event while mouse button held down*/
#define EVmousepressed			EVmousedown+EVmousedrag 
#define EVkeydown				0x0010  /* Key pressed    */
#define EVkeyup					0x0020  /* Key released */
#define EVscankeydown			0x0040  /* Key released */
#define EVcommand				0x0080  /* Command event  */
#define EVmousewheel			0x0100
#define EVmousedoubleclicked	0x0200


#define EVnothing				0x0000  /* Event already handled*/
#define EVmouse					0x030F  /* Mouse event   */
#define EVkeyboard				0x0070  /* Keyboard event*/
#define EVmessage				0xFF00  /* Message (command, broadcast, | user-defined) event*/
#define EVall					0xFFFF 

/*add_and_draw constants*/
#define VPcentre				1
#define VPbottomright			2 
#define VPtopright				3 
#define VPbottomleft			4 
#define VPtopleft				5 
#define VPindent				3  

typedef trectlist *(*rectlistcallbacktype) (trect &r);
class tview;
class tgroup;

//32 bytes
struct tevent
{
	UINT32 what;
	
	union
	{     
		struct 
		{
			tpointbase  mpos;
			UINT32		mbuttons;
		};
		struct
		{			
			UINT32		scankey; 
			char		key;
		};
		struct
		{
			UINT32 command;
			UINT32 data;
			union
			{
				void		*infoptr;
				tview		*infotview;
				tgroup		*infotgroup; 
				UINT32		infoUINT32;
				INT32		infoINT32;				
				float		infofloat;
				char		*infotxt;
			};
		};
	};
	tevent(UINT32 wha,UINT32 com,void *data){what=wha;command=com;infoptr=data;}
	tevent(){;}
};


class tobject
{
	tobject();
	virtual ~tobject();
};

class tview:public trect
{   
public:
	UINT32 pid;  
	UINT32 options,state;
	tview *next,*firstchild;
	tgroup *owner;
	tview *target;
	UINT8 growmode;
	
	tview(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
	tview();
	virtual ~tview();
	/*this is the one you use if you don't want to redraw the view!*/
	virtual void changebounds(trect &bounds);
	virtual bool setstate(UINT32 astate,bool enable );
	virtual void draw()=0;   
	virtual void handleevent(tevent *event);
	virtual void initialupdate();
	
	bool isvisible(){return (state & SFvisible)!=0;}
	bool deaf(){return ((state & SFdeaf)!=0);}
	void activate(){state |=SFvisible;state &=~(SFdisabled+SFmute+SFdeaf);}
	void deactivate(){state &=~SFvisible;state |=(SFdisabled+SFmute+SFdeaf);}
	void disable(){state |=SFdisabled;}
	void enable(){state &=~SFdisabled;}
	void makeinvisible(){state&=~SFvisible;}
	void makevisible(){state|=SFvisible;}
	void unmute(){state &=~SFmute;}
	void mute(){state |=SFmute;}
	void undeafen(){state &=~SFdeaf;}
	void deafen(){state |=SFdeaf;}
	void drawtransparent();
	
	void setbounds(INT32 x1,INT32 y1,INT32 x2,INT32 y2){rassign(x1,y1,x2,y2);};
	void setbounds(trect &bounds){rassign(bounds.a.x,bounds.a.y,bounds.b.x,bounds.b.y);};
	void getbounds(trect *bounds){bounds->a=a;bounds->b=b;};
	void tmoveto(INT32 x1,INT32 y1);
	void tmove(INT32 x1,INT32 y1);
	void tgrow(INT32 x1,INT32 y1);
	void calcbounds(trect &change);
	void limitmouse(trect &bounds);
	void reposview(trect &bounds);
	void select();
	//void dragview(char mode,trect limits,INT32 minx,INT32 miny,INT32 maxx,INT32 maxy);
	void drawview();
	void doneevent(tevent *event);
	void evcallback(UINT32 command);
	void evcallback2(UINT32 command,UINT32 code);
	void sendcommand(tview *destination,tevent *sendevent);
	
	void sendcommand(UINT32 command,tview *destination,void *data);
	void whilemousein();
	void remove();
	tview *prev();
	void putinfrontof(tview *view,bool redraw);
	char exposed();
	char inview(INT32 x,INT32 y);
};


class tgroup:public tview
{    
public:
	tview *lastchild;
	tgroup(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
	tgroup();
	virtual ~tgroup();
	
	virtual void changebounds(trect &bounds);
	virtual void draw(); 
	virtual void handleevent(tevent *event);
	tview* insert(tview *view,UINT32 id=0,UINT32 growmode=TGdefault);
	tview* insertrel(tview *view,UINT32 id=0,UINT32 growmode=TGdefault);
	tview *swapinto(tview *view,tview *swapped);
	char add_and_draw(tview *view,char where,UINT32 pid);
	bool ischild(tview *view);
	
};

class windowgfx
{
public:
    //also, for non client edge windows, a new set of "filler" gadgets is needed
    //topleft topright bottom left bottom right
    //corner gadgets, corner gadgets for buttons and interior corner gadgets (for client edges). All same size
    tsurface *br,*tr,*bl,*tl,*rb,*mb,*lb,*br0,*br1,*itl,*ibl,*itr,*ibr;
    //connector gadgets, exterior and interior client edge. all horizontal, left and right rotated 90degrees
    //all same size, for now
    tsurface *l,*r,*t,*b,*il,*ir,*it,*ib;
    //text area (spacer - text main -spacer) and filler gadgets.
    tsurface *title[6];
    void freeall();
};

class tblitrectcollection
{
public:		
	void set(UINT32 count,tsurface *surf);
	operator tblitrect*(){return (tblitrect*)this;}	
	
};

class thorizborders:public tblitrectcollection
{
public:
	tblitrect t,b;
};

class tcorners:public tblitrectcollection
{
public:
	tblitrect tl,tr,bl,br;
};

class tvertborders:public tblitrectcollection
{
public:
	tblitrect l,r;
};

class twindowborders:public tblitrectcollection
{
public:
	tcorners w;
	tblitrect lbbord,mbbord,rbbord,resize0,resize1;
	tcorners ce;
	tblitrect title[6];			
	INT32 getheight(){return lbbord.h;}
};

class ttabgfx:public tblitrectcollection
{
public:
	tblitrect lb,lt1,mt1,rt1,lt0,mt0,rt0,rc,spacer,rb;
	INT32 getheight(){return lb.h;}
};

class tnumboxgfx:public tblitrectcollection
{
public:
	tblitrect down0,down1,up0,up1;
	INT32 getwidth(){return down0.w;}
	INT32 getheight(){return down0.h+up0.h+4;}
	INT32 getbuttonheight(){return down0.h;}
	
};

class tdropboxgfx:public tblitrectcollection
{
public:
	tblitrect down1,down0;
	INT32 getbuttonwidth(){return down0.w;}
	INT32 getheight(){return down0.h;}
	
};
class tcheckboxgfx:public tblitrectcollection
{
public:
	tblitrect b0,b1,b2;
};

class tshadowgfx:public tblitrectcollection
{
public:
	tblitrect l,m,r,t;
};

class twindowicons:public tblitrectcollection
{
public:
	tblitrect close0,close1,close2,min0,min1,min2,max0,max1,max2,unmax0,unmax1,unmax2;	
};

class tscrollgfx:public tblitrectcollection
{
public:
	tblitrect bgleft,bgmid,bgright,barleft,barmid,barright,barshad;	
	INT32 getscrolllength(){return bgleft.w+bgmid.w+bgright.w+barleft.w+barmid.w+barright.w+barshad.w;};
	INT32 getscrollheight(){return bgleft.h;};
};

class tscrollbargfx:public tscrollgfx
{
public:
	tblitrect right0,right1,right2,down0,down1,down2,left0,left1,left2,up0,up1,up2;		
	INT32 getbuttonwidth(){return left0.w;}
	INT32 getbuttonheight(){return left0.h;}
};

class twingfx:public tblitrectcollection
{
public:
	twindowborders window0,window1;
	thorizborders window1_h,window0_h;
	thorizborders ce1_h,ce0_h;
	twindowicons windowicons;
	tscrollbargfx scroll;
	ttabgfx tab;
	tnumboxgfx numbox;
	tshadowgfx shadow;	
	tdropboxgfx dropbox;
	tcheckboxgfx checkbox;
	
	tvertborders menu_v;
	tblitrect menu_bg0,menu_bg1;
	tvertborders window1_v,window0_v;
	tblitrect window_bg0;
	tvertborders ce0_v;	
	tblitrect paper_bg;
	tvertborders paper_v;
	tblitrect button_bg;
	tvertborders buttonu_v,buttond_v;	
	
	tblitrect shadowr;	  
	tcorners paper_c;
	thorizborders paper_h;
	tcorners popupmenu_c;
	thorizborders popupmenu_h;
	tcorners titlemenu_c;
	thorizborders titlemenu_h;
	tcorners buttonu_c;
	thorizborders buttonu_h;
	tcorners buttond_c;
	thorizborders buttond_h;					
	tblitrect filler[20];
};

extern	twingfx		TWINctrlbm;
extern	tscrollgfx  TWINctrlbmv;
extern	INT32		TWINbarminsize;
extern  tpalette	*TWINpalette;
extern  UINT32		twSTATE;


/*----****IMPORTANT GLOBALS****----*/

//the modal stack is needed because views belonging to a modal dialog
//may be selected but on release, the modal behaviour of the dialog
//is still desired; therefore, this is nessiciary;
extern	tview		*TWINmodalstack[TWINmodalstacklimit];

extern	tsurface	*gadgets,*vgadgets,*scratchpad;
extern	tsurface	*fileicons;
extern	trect		TWINtextindent;/*,
					TWINclientindent,
					TWINwindowindent,
					TWINmenuindent,
					TWINpopupmenuindent,
					TWINtabindent,
					TWINborderindent;*/
extern	char		charcode;
extern	char		scancode;
extern	theapmem	disturbedlist;
extern	UINT32		TWINdefaultgrowmode;
extern	tview		*modal_view;               
extern	tview		*global_selected;
extern	tgroup		*root;
extern	INT32		TWINmodalstackhead;
extern  tcolour		TWINtextcol,
					TWINpapercol,
					TWINpaperselcol,
					TWINtextselcol,
					TWINtextdisabledcol,
					TWINtextdisabledhighlightcol;



void    TWINlock(trect *bounds);
void    TWINsetlockcallbacks(void (*lock)(trect *bounds),void (*unlock)(trect *bounds));
void    TWINsetcliprectcallback(rectlistcallbacktype cliprects);
void    TWINunlock(trect *bounds);
void    TWINsetroot(tgroup *root_);
tgroup *TWINgetroot();
void	TWINclose();
bool	TWINinit(char *rpath);
void	TWINeventmpos();
void	TWINfinalizemouseevent(tevent &mevent);
void	TWINfinalizekeyevent(tevent &keyevent);
void	TWINeventkey();
bool	TWINismodal(tview *view);
tgroup *TWINgetpopuptarget();
tgroup *TWINsetpopuptarget(tgroup *target);
void	TWINsetcontext(trendercontext *ctx);
void	TWINsetmodal(tview *view);
void	TWINendmodal();
void	TWINendallmodals();
extern	bool TWINtabkeybehaviour(tgroup *view,tevent *event);
extern	trendercontext *TWINrendercontext;

void	drawfilledborder(INT32 x1,INT32 y1,INT32 x2,INT32 y2,tblitrect *c,tblitrect *h,tblitrect *v,tblitrect *mid,tsurface *base);
void	drawborder(INT32 x1,INT32 y1,INT32 x2,INT32 y2,tblitrect *c,tblitrect *h,tblitrect *v,tsurface *surf);
void	captilebitmapy(INT32 x,INT32 y,INT32 y2,tblitrect *c,tsurface *image);
void	captilebitmapx(INT32 x,INT32 y,INT32 x2,tblitrect *c,tsurface *image);


#endif

