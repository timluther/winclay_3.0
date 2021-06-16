//////windowing basics code 
#include "stdafx.h"
#include "graphics.h"
#include "logger.h"
#include "msmouse.h"
#include "views.h"

twingfx TWINctrlbm;
tscrollgfx TWINctrlbmv;
INT32 TWINbarminsize;
trendercontext *TWINrendercontext;
rectlistcallbacktype buildcliprects=NULL;
void (*lockcallback)(trect *bounds)=NULL;
void (*unlockcallback)(trect *bounds)=NULL;
UINT32 lockcount=0;
  
/*----****IMPORTANT GLOBALS****----*/
//all of these values control the size of graphics used in the widgets (most of this is actually used in widgets.h)
//the naming convention (roughly) is an abrieviated form of the widget in question (w for window, sb for scroll bar ect)
//and then an underbar folled by an abrieviated description
//w is used for width, h for height, l for left, m for middle, r for right, c for corner (normally a tpoint varialbe containing X and Y size).
//btn means button
trect TWINtextindent,TWINclientindent,TWINwindowindent,TWINmenuindent,TWINpopupmenuindent,TWINtabindent,TWINborderindent;
char RESOURCE_PATH[128];
char charcode;
char scancode;



INT32		TWINmodalstackhead;
UINT32		TWINattributes;
UINT32		twSTATE;
UINT32		twDISPLAYATTRIB;
UINT32		TWINdefaultgrowmode=TGgrowall;
tcolour		TWINtextcol,TWINpapercol,TWINpaperselcol,TWINtextselcol,TWINtextdisabledcol,TWINtextdisabledhighlightcol;
tcolour		buttoncols[5];
tpalette	*TWINpalette;

tview		*TWINmodalstack[TWINmodalstacklimit];
tview		*terminal_mouserecipient=NULL;
tview		*modal_view=NULL;
tview		*global_selected=NULL;
tview		*last_mouseover=NULL;
tgroup		*root=NULL;
tgroup		*popup_target=NULL;
tsurface	*m_pointer,
			*m_waiting,
			*m_pointup,
			*m_pointdown,
			*m_pointleft,
			*m_pointright,
			*m_rotate,
			*m_hand;
			
tsurface	*fileicons;



/************************====--- TObject ---====************************/
tobject::tobject(){ }
tobject::~tobject(){ }

/************************====--- tview ---====************************/
/*make is for view whose dimensions are determined latter -they are minimal
 constructrs*/
tview::tview()
{
  growmode=TWINdefaultgrowmode;
  target=NULL;
  owner=NULL;
  next=firstchild=NULL;
  pid=PIDundefined;
  state=SFenabled;  
  options=OFselectable;
}

tview::tview(INT32 x1,INT32 y1,INT32 x2,INT32 y2)
{
  a.x=x1; 
  a.y=y1;
  b.x=x2; 
  b.y=y2;
  pid=PIDundefined;
  
  growmode=TWINdefaultgrowmode;
  target=NULL;
  owner=NULL;
  next=firstchild=NULL;
  state=SFvisible+SFenabled;
  options=OFselectable+OFsoliddrag;
}

tview::~tview()
{
    if (modal_view==this)
       TWINendmodal();   
	if (global_selected==this)
		global_selected=NULL;
	if (last_mouseover==this)
		last_mouseover=NULL;
	if (terminal_mouserecipient==this)
		terminal_mouserecipient=NULL;		
	
}

void tview::tmove(INT32 x1,INT32 y1)
{  
  trect tmp_loader;
  getbounds(&tmp_loader);
  tmp_loader.rmove(x1,y1);
  reposview(tmp_loader);
}

void tview::tmoveto(INT32 x1,INT32 y1)
{  
  trect tmp_loader;
  getbounds(&tmp_loader);
  tmp_loader.rpos(x1,y1);  
  reposview(tmp_loader);
}

/*        Procedure Tview_.calcbounds(change:trect);
var dragx,dragy:boolean;
    tmp_loader:trect;
Begin
  getbounds(tmp_loader);
  with tmp_loader do
  if change.empty then
    rmove(change.x1,change.y1)
  else begin
    dragX:=growmode and TGdragonX <> 0;
    dragY:=growmode and TGdragonY <> 0;
    if growmode and TGgrowx1<>0 then begin
      inc(x1,change.x1);if dragx then inc(x2,change.x1);
    end;
    if growmode and TGgrowy1<>0 then begin
      inc(y1,change.y1);if dragy then inc(y2,change.y1);
    end;
    if growmode and TGgrowx2<>0 then begin
      inc(x2,change.x2);if dragx then inc(x1,change.x2);
    end;
    if growmode and TGgrowy2<>0 then begin
      inc(y2,change.y2);if dragy then inc(y1,change.y2);
    end;
  end;
  changebounds(tmp_loader);
end;*/
        
void tview::calcbounds(trect &change)
{
  trect tmp_loader;
  getbounds(&tmp_loader);
  if (change.rempty())
    tmp_loader.rmove(change.a.x,change.a.y);
  else
  {
    if ((growmode & (TGgrowx1|TGdragonx1))!=0)
    {
      tmp_loader.a.x+=change.a.x;
      if ((growmode & TGdragonx1)!=0) tmp_loader.b.x+=change.a.x;
    }
    if ((growmode & (TGgrowx2|TGdragonx2))!=0)
    {
      tmp_loader.b.x+=change.b.x;
      if ((growmode & TGdragonx2)!=0) tmp_loader.a.x+=change.b.x;
    }    
    if ((growmode & (TGgrowy1|TGdragony1))!=0)
    {
      tmp_loader.a.y+=change.a.y;
      if ((growmode & TGdragony1)!=0) tmp_loader.b.y+=change.a.y;
    }    
    if ((growmode & (TGgrowy2|TGdragony2))!=0)
    {
      tmp_loader.b.y+=change.b.y;
      if ((growmode & TGdragony2)!=0) tmp_loader.a.y+=change.b.y;
    }
  }
  changebounds(tmp_loader);
}


void tview::reposview(trect &bounds)
{
  trect old,oldvp;
  tview *currview;   
  getbounds(&old);
  if (old==bounds) return;
  if (old.width()==bounds.width() && old.height()==bounds.height())
	  state |= SFdragging;	
  else
	  state |= SFresizing;

  changebounds(bounds);
  if ((state & SFvisible)!=0)
  {
  MOUSEoff();
  ms_visible=-128;  
  if (  !((options & OFtoplevelalien)&&(state & (SFdragging+SFresizing))) )
    drawview();
  if (owner!=NULL)
  {
    oldvp=SC.viewport;
    old.rintersect(SC.viewport);
    SC.viewport=old;
    currview=next;
    while (currview!=NULL)
    {
      currview->drawview();
      currview=currview->next;
    }
    SC.viewport=oldvp;
  }
  
  ms_visible=1;
  MOUSEon();
  state &= ~(SFdragging+SFresizing);
  }
}

void tview::changebounds(trect &bounds)
{
    rassign(bounds.a.x,bounds.a.y,bounds.b.x,bounds.b.y);
}

void tview::select()
{
  char sf;
  tview *selview;
  if ((global_selected==this)||(owner==NULL)) return;
  sf=SFselected+SFfocused;
  if ( ((state & SFselected)==0)&&((options & OFselectable)!=0)&&(owner!=NULL))
  {    
    selview=this;
    //traverse up tree, marking owners so that they don't get defocused
    while (selview!=NULL)
    {      
      selview->state|=SFreserved;
      selview=selview->owner;      
    }  
    //traverse tree, deselecting owners*/         
    while (global_selected!=NULL)
    {
       if ((global_selected->state & SFreserved)==0)
           global_selected->setstate(sf,FALSE);
       global_selected=global_selected->owner;
    }
    global_selected=this;
    selview=owner;
    /*traverse up tree, selecting owners and reseting reserved flag*/
    while (selview!=NULL)
    {
        selview->state&=~SFreserved;            
        selview->setstate(SFfocused,TRUE);        
        selview=selview->owner;      
    }
    state&=~SFreserved;    
    setstate(sf,TRUE);
    if ((options & OFtopselect)!=0) 
       putinfrontof(owner->firstchild,true);        
  }
}


bool tview::setstate(UINT32 astate,bool enable)
{    
    UINT32 oldstate=state & ~SFreserved;
    (enable==1)?state|=astate:state &= astate^0xFFFF;//));
    return ((state&~SFreserved)!=oldstate);
}


void tview::drawtransparent()
{
	TWINrendercontext->setoutput(scratchpad);
	//mess with SC.scrptr
	//render background into scratchpad

	draw();
	TWINrendercontext->setoutput(NULL);
	putbitmap(a.x,a.y,scratchpad);
}

void tview::drawview()
{
    trectlist *p,*oldlop,*plop,*newp;//,*prevrect;
    trect oldvp;
    tgroup  *parent;
    tview *currview,*stopat;
    UINT32 rectidx;
    char *rectpos;
    if ((roverlap(SC.viewport))&&(state & SFvisible)&&(twSTATE & TSvisible)&&(b.x>a.x)&&(b.y>a.y))
    {
        //this part does the basic clipping and also ensures that the view is not
        //drawn if any of it's parents are invisible or if the view is
        //not part of the main windows heirarchy (i.e. if ultimate parent is not 'root')
        rectidx=rectbuffer.currindex;
        rectpos=rectbuffer.curr;
        oldvp=SC.viewport;
        SC.viewport.rintersect(*this);
                //stopat=root;
        if (this!=root)
        {
            parent=owner;        
            while (parent!=NULL)
            {
                if ((parent->state & SFvisible)==0) goto end;           
                SC.viewport.rintersect(*parent);
                stopat=(tview*)parent;
                parent=parent->owner;
            }        
            if (stopat!=(tview*)root)goto end;
        }
		
		if (buildcliprects)
		{
		
		    if (!(p=buildcliprects(SC.viewport)))			  
			  goto end;
		}
		else
		{
           p=getrectmem();
           *p=SC.viewport;
		   p->next=NULL;
		}	   
        
        parent=owner;
        stopat=this;

        while (parent!=NULL)
        {
            currview=parent->firstchild;
            if ((parent->options & OFnooverlap) == 0)
            while ((currview!=stopat)&&(p!=NULL))
            {
                /*traverse current rectangle list*/
                plop=p;
                
                while (plop!=NULL)
                {
                                                          
                    newp=plop->cutfrom(*currview);                    
                    switch (rectlist_status)
                    {
                        /*replace current rectangle with the rectangles cut away from it*/
                        
                        case RSlist:
                            lastrect->next=plop->next;
                            plop->a=newp->a;
                            plop->b=newp->b;                                                        
                            plop->next=newp->next;                            
                            plop=lastrect->next;
                            /*should check for 'concatablity' by 'ere*/
                            break;                                           
                        case RSobscured:
                            newp=plop->next;
                            if (plop==p)
                                p=newp;
                            else  //get previous and setup it's 'next' ptr;
                            {
                                oldlop=p;
                                while ((oldlop!=NULL)&&(oldlop->next!=plop))
                                    oldlop=oldlop->next;
                                oldlop->next=newp;
                                
                            }
                            plop=newp;
                            break;
                        /* leave it alone if nothing touches it*/
                        default:                            
                            plop=plop->next;
                            break;
                    }
                }
                currview=currview->next;
            }
            stopat=parent;
            parent=parent->owner;
        }    
		
        if (p!=NULL)
        {
            /*draw loop*/
            plop=p;
            MOUSEoff();
			//trect lrect=SC.viewport;
			//lrect.b;
			TWINlock(NULL);//
            while (plop!=NULL)
            {
                SC.viewport=*plop;				
                draw();				                
                state=state | SFdrawnonce;
                plop=plop->next;
            }
			TWINunlock(NULL);//

            MOUSEon();          
            state^=SFdrawnonce;           
        }
		
end:        

        SC.viewport=oldvp;          
        rectbuffer.currindex=rectidx;
        rectbuffer.curr=rectpos;
    }
}


/*the always overridden standard handling procedure*/
void tview::handleevent(tevent *event)
{
  if (event->what & EVmouse)
  {
      terminal_mouserecipient=this;	  
  }else
  if (event->what==EVcommand)
  {
      switch (event->command)
      {
      case CMDclose:remove();event->what=0;break;
      }
  }
}


//called when inserted
void tview::initialupdate()
{
}

void tview::doneevent(tevent *event)
{
  event->what=0;
}

void tview::evcallback(UINT32 command)
{  
    sendcommand(command,target,this);
}

void tview::evcallback2(UINT32 command,UINT32 code)
{  
    sendcommand(command,target,(void*)code);
}


void tview::sendcommand(tview *destination,tevent *sendevent)
{
    if ((!destination)||(state & SFmute)) return;    
      destination->handleevent(sendevent);
}


void tview::sendcommand(UINT32 command,tview *destination,void *data)
{
  if ((destination==NULL)||(state & SFmute)) return;
  tevent sendevent;
  sendevent.what=EVcommand;
  sendevent.infoptr=data;
  sendevent.command=command;
  destination->handleevent(&sendevent);
}

void tview::limitmouse(trect &bounds)
{
  MOUSEsetbounds(bounds.a.x-(a.x-mp.x),bounds.a.y-(a.y-mp.y),bounds.b.x-(b.x-mp.x),bounds.b.y-(b.y-mp.y));
}

tview *tview::prev()
{
  tview *currview;
  if (owner==NULL) return NULL;
  currview=owner->firstchild;
  if (currview==this) return currview;
  while (currview->next!=this)
  {
    if (currview==NULL)return NULL;
    currview=currview->next;    
  }
  return currview;
}

void tview::putinfrontof(tview *view,bool redraw)
{
  if (owner==NULL) return;  
  if ((owner->lastchild==owner->firstchild)||(this==owner->firstchild)) return;
  prev()->next=next;  
  next=owner->firstchild;
  owner->firstchild=this;
  if ((redraw)&&((state & SFvisible)!=0))
  {
      MOUSEoff();
      ms_visible=-128;
      drawview();
      ms_visible=1;
      MOUSEon();
  }
  //next=owner->firstchild;
  //owner->firstchild=this;
  //draw();
  /*tview *currview;  
  trect oldvp;
  trectlist *plist,*currp;
  char nooverlaps;
    
  if (view!=this)
  {
    if ((owner!=NULL)&&(this!=owner->firstchild))
    {
      rectbuffer.reset();
      currview=owner->firstchild;
      plist=getrectmem();
      nooverlaps=TRUE;
      currp=plist;
      while (currview!=this)
      {
        if (currview->roverlap(*this))
        {
          nooverlaps=FALSE;
          currp->rassign(currview->a.x,currview->a.y,currview->b.x,currview->b.y);
          currp->rintersect(SC.screenport);
          if (currview->next!=this)
          {
            currp->next=getrectmem();            
            currp=currp->next;
          }
        }
        currview=currview->next;
      }
      currp->next=NULL;
    }

    //rearange the list
    if (view->owner->firstchild==view)
        view->owner->firstchild=this; 
    else if (view->owner->firstchild=this)
        view->owner->firstchild=next;
    prev()->next=next;    
    view->prev()->next=this;
    next=view;
    if ((state & SFvisible)!=0)
    {
        oldvp=SC.viewport;
        
        
        //draw 'self' in only the areas that have been revealed
        if (owner->firstchild==this) select();
        if (nooverlaps)
            currp=NULL;
        else
            currp=plist;
        while (currp!=NULL)
        {
            SC.viewport=*currp;
            drawview();
            currp=currp->next;
        }        
        
        //write on views, if any, that will obscure 'self'
        currview=owner->firstchild;
        SC.viewport=*this;
        while ((currview!=this)&& (currview!=NULL))
        {
            currview->drawview();
            currview=currview->next;
        }
        SC.viewport=oldvp;
    }
    rectbuffer.reset(); 
  }  */
}

char tview::inview(INT32 x,INT32 y)
{
  tgroup *parent=owner;
  tview *stopat=this,*currview;
  
  if (!rcontains(x,y)) return FALSE;
  
  while (parent!=NULL)
  {
      currview=parent->firstchild;
      if (((parent->options & OFnooverlap) == 0) )
      while (currview!=stopat)
      {
          if ((currview->rcontains(x,y))|(! parent->rcontains(x,y)))
              return (FALSE);
          currview=currview->next;
      }
      stopat=parent;
      parent=parent->owner;
  }
  return (TRUE);  
}

char tview::exposed()
{
    tgroup *parent=owner;
    tview *currview=this,*stopat;
    //stopat=root
    while (parent!=NULL)
    {
        currview=parent->firstchild;
        if ((parent->options & OFnooverlap) == 0 )
        while (currview!=stopat)
        {
            if (stopat->rinside(*currview))
                return (FALSE);
            currview=currview->next;
        }
        stopat=parent;
        parent=parent->owner;
    }
    return(TRUE);
}

void tview::remove()
{
    if (owner==NULL) return;
    trect oldvp;
    tview *currview;
    oldvp=SC.viewport;
    SC.viewport=*this;
    SC.viewport.rintersect(0,0,SC.size.x,SC.size.y);
    currview=prev();
    
    if (currview)
      currview->next=next;
    if (owner->firstchild==this)
        owner->firstchild=next;
    if (owner->lastchild==this)
        owner->lastchild=currview;
        
    root->select();        
    currview=next;
    while (currview!=NULL)
    {
         currview->drawview();
         currview=currview->next;
    }
    target=NULL;
    owner=NULL;
    state&=~SFvisible;
    //currview->drawview();
    SC.viewport=oldvp;    
} 

void tview::whilemousein(){
  while (inview(mp.x,mp.y) && (mb!=0));
    //mpos();
}

/************************====--- tgroup_ ---====************************/

tgroup::tgroup(INT32 x1,INT32 y1,INT32 x2,INT32 y2):tview(x1,y1,x2,y2)
{
  lastchild=NULL;  
}

tgroup::tgroup():tview()
{
  lastchild=NULL;  
}

void tgroup::handleevent(tevent *event)
{
      tview *currview;
      
      tview::handleevent(event);
      if ((event->what & EVmouse)!=0)
      {
          currview=firstchild;
          while ((event->what!=0)&&(currview!=NULL))
          {
              if ((currview->inview(mp.x,mp.y) && !currview->deaf() ) )              
                  currview->handleevent(event);              
              currview=currview->next;
          }                      
      }
      else if ((event->what & EVkeyboard)!=0)
      {          
          currview=firstchild;
          while ((currview!=NULL)&&(event->what!=0))
          {
              if ((currview!=global_selected)&&(currview->options & OFkeygrab!=0)&& (!currview->deaf()))
                 currview->handleevent(event);                 
              currview=currview->next;
          }             
      }
}

tview *tgroup::swapinto(tview *view,tview *swapped)
{
	if (swapped->owner!=this)
		return NULL;
	if (swapped!=firstchild)
	{
		tview *prev=swapped->prev();	
		if (prev)
			prev->next=view;		
	}else
		firstchild=view;	
	if (swapped==lastchild)		
		lastchild=view;				
	view->next=swapped->next;
	view->owner=this;
	view->target=swapped->target;
	view->state|=SFvisible;
	view->changebounds(*swapped);
	return view;
}

tview* tgroup::insertrel(tview *view,UINT32 id,UINT32 growmode)
{
  //relative insert
  trect tmp=trect(view->a.x+a.x,view->a.y+a.y,view->b.x+a.x,view->b.y+a.y);
  view->changebounds(tmp);
  return insert(view,id,growmode);  
}

tview* tgroup::insert(tview *view,UINT32 id,UINT32 growmode)
{   
  if (lastchild==NULL)
  { 
      lastchild=view;
      firstchild=view;            
  } else
  {
      view->next=firstchild;
      firstchild=view;
  }
  if (growmode!=TGdefault)
	  view->growmode=growmode;  
  view->pid=id;
  lastchild->next=NULL;
  view->target=(tview*)this;
  view->owner=this;  
  view->state|=SFvisible;
  view->initialupdate();
  return view;  
}

/*
procedure Tgroup_.insert(view:tview);
begin
  if last=nil then begin
    last:=view;
    first:=view;
  end else begin
    view^.next:=first;
    first:=view;
  end;
  last^.next:=nil;
  view^.owner:=@self;
  {state:=state and not sfvisible;
  view^.select;
  state:=state or sfvisible;}
end;
*/

char tgroup::add_and_draw(tview *view,char where,UINT32 pid)
{
  trect tmprect;
  tview *currview=firstchild;
  
  while (currview!=NULL)
  {
      if ((currview->pid!=PIDundefined)&&(currview->pid==pid))
      {
          currview->putinfrontof(firstchild,true);
          delete view;
          return false;
      }
      currview=currview->next;
  }
  insert(view);
  view->pid=pid;
  tmprect=*view;
  switch (where)
  {
      case VPcentre:
          tmprect.rpos(a.x+halfx()-view->halfx(), a.y+halfy()-view->halfy());
          break;
      case VPbottomright:
          tmprect.rpos(b.x-(view->b.x-view->a.x)-VPindent,b.y-(view->b.y-view->a.y)-VPindent);
          break;
      case VPtopright:
          tmprect.rpos(b.x-(view->b.x-view->a.x)-VPindent,a.y+VPindent);
          break;
  }
  if (where!=0) view->changebounds(tmprect);
    view->drawview();
  return(TRUE);
}

tgroup::~tgroup()
{
  tview *nextview,*curr;   
  
  curr=firstchild;
  if (curr!=NULL )
  while (1)
  {
      nextview=curr->next;
	  delete curr;
      
      curr=nextview;
	  if(!curr) break;
  }
  firstchild=NULL;
  //tview::~tview(); 
}

void tgroup::draw()
{
    if (firstchild!=NULL)
    {
        trect oldvp;
        tview *currview=firstchild;
		if ((options & OFnooverlap) != 0)
        {           
			while (currview!=NULL)
			{            
				if ((currview->state & SFvisible)&&(currview->roverlap(SC.viewport)))
				{
					oldvp=SC.viewport;
					SC.viewport.rintersect(*currview);
					currview->draw();
					SC.viewport=oldvp;                    					
				}    
				currview=currview->next;
			}			            
        }else
		{
			while (currview!=NULL)
			{
				currview->drawview();
				currview=currview->next;
			}
		}
    }
}

void tgroup::changebounds(trect &bounds)
{

  trect diff;
  tview *currview ;
  /*get the difference between the old rectangle && the new*/
  diff.a=bounds.a-a;
  diff.b=bounds.b-b;
  setbounds(bounds);

  currview=firstchild;
  /*options=options | ofparentresize; */
  while (currview!=NULL)
  {
      currview->calcbounds(diff);
      currview=currview->next;
  }
  /*options=options xor ofparentresize;*/
}

bool tgroup::ischild(tview *view)
{
    tview *v=firstchild;
    while (v!=NULL)
    {
       if (v==view)      
          return true;
       v=v->next;
    }
    return false;
}

//loose functions. Should really put a TWIN prefix or something on these things

bool TWINtabkeybehaviour(tgroup *view,tevent *event)
{
    if (event->what & EVkeydown)
    {
        if (scancode==tabkey)
        {
          if ((global_selected==NULL)||(global_selected->next==NULL)||(!view->ischild(global_selected)))
            view->firstchild->select();
          else          
            global_selected->next->select();
          return true;                  
        }        
    }
    return false;
}

/*Core key && mouse event handling*/

void TWINfinalizemouseevent(tevent &mevent)
{	  	  
      if (terminal_mouserecipient)
      {
          //if applicable, select terminal recipient and and focus it's ancestors
          if (((terminal_mouserecipient->options & OFautoraise)!=0)||(mb!=0))
             terminal_mouserecipient->select();
          //set mouseover flag for all ancestors of terminal recipient
          if ((terminal_mouserecipient->state & SFmouseover)==0)
          {
            tview *loopback=terminal_mouserecipient;
            //switch on mouseover flags and reseverd flags       
            while (loopback)
            {               
               loopback->setstate(SFmouseover+SFreserved,true);
               loopback=loopback->owner;
            }
            //switch off all flags that were not just set
            while (last_mouseover)
            { 
               if ((last_mouseover->state & SFreserved)==0)               
                   last_mouseover->setstate(SFmouseover,false);                                
               last_mouseover=last_mouseover->owner;
            }
            loopback=terminal_mouserecipient;
            //clear reserved flags
            while (loopback)
            {
               loopback->state&=~SFreserved;
               loopback=loopback->owner;
            }
            last_mouseover=terminal_mouserecipient;
          }
      }
}


void TWINfinalizekeyevent(tevent &keyevent)
{
	if (global_selected!=NULL)
    {
        tview *currview=global_selected;
        while ((keyevent.what!=0)&&(currview!=NULL))
        {
            currview->handleevent(&keyevent);
            //if (keyevent.what==0)
            //  rectangle(currview->a.x,currview->a.y,currview->b.x,currview->b.y);
            currview=currview->owner;
        }
    }else
        root->handleevent(&keyevent);
}

void TWINeventmpos()
{
  terminal_mouserecipient=NULL;
  UINT32 what=0;
  tevent mevent;
  MOUSEpoll();
  if (MOUSEwheel)  
	  what+=EVmousewheel; 
  if (MOUSEdoubleclicked)
	  what+=EVmousedoubleclicked;
   
  if (MOUSEreleased) 
      what+=EVmouseup;
  else
  {
      if (mb!=0) 
          what+=EVmousedrag;
      if (MOUSEpressed)  
          what+=EVmousedown;
  }
  if (MOUSEmoved)
      what+=EVmousemove;
            
  if (what!=0)
  {
	  mevent.what=what;
	  if (modal_view!=NULL)
         modal_view->handleevent(&mevent);
      else  
         root->handleevent(&mevent);
	  TWINfinalizemouseevent(mevent);      
  }              
}




void TWINeventkey()
{
  tevent keyevent;
  tview *currview;
  if (kbhit())
  {
    charcode=getch();
    if (charcode==0) 
        scancode=getch();
    else
        scancode=0;
    itoa(scancode,buf,10);
    
    //label(0,SC.size.y-20,buf);
    
    keyevent.what=EVkeydown;
    /*if (modal_view!=NULL)
        modal_view->handleevent(&keyevent);
    else */
    if (global_selected!=NULL)
    {
        currview=global_selected;
        while ((keyevent.what!=0)&&(currview!=NULL))
        {
            currview->handleevent(&keyevent);
            //if (keyevent.what==0)
            //  rectangle(currview->a.x,currview->a.y,currview->b.x,currview->b.y);
            currview=currview->owner;
        }
    }else
        root->handleevent(&keyevent);
  }
}


bool TWINismodal(tview *view)
{
    
    INT32 i=TWINmodalstackhead;
    while (i>0)    
        if (TWINmodalstack[i--]==view) return true;
    return false;        
}

void TWINsetmodal(tview *view)
{
    if (TWINmodalstackhead<TWINmodalstacklimit-1)
      TWINmodalstackhead++; 
    TWINmodalstack[TWINmodalstackhead]=view;        
    modal_view=view;
    view->state|=SFmodal;    
}

void TWINendmodal()
{
    TWINmodalstackhead--;    
    if (TWINmodalstackhead<0)
      TWINmodalstackhead=0;
	if (modal_view)
    modal_view->state &= (SFmodal ^ 0xff);
    modal_view=TWINmodalstack[TWINmodalstackhead];
}

void TWINendallmodals()
{
    while (TWINmodalstackhead>=0)
    {
        modal_view->state &= (SFmodal ^ 0xff);
        modal_view=TWINmodalstack[TWINmodalstackhead];
        TWINmodalstackhead--;    
    }
}
    
tsurface *getresourceimage(char *imagename,tpixelformat mode)
{
    char dir[255];
    strcpy(dir,RESOURCE_PATH);
    strcat(dir,imagename);
    return readimage(TWINrendercontext,dir,mode,SFdoublewordaligned);    
}

tsurface *gadgets,*vgadgets,*scratchpad;

void windowgfx::freeall()
{
    UINT8 i;
    tsurface **cbm=&tl;
    
    for (i=0;i<28;i++)
    {
       delete *cbm;
       cbm++;
    }       
}   

INT32 getnextypix(INT32 ypix,tcolour t_chroma)
{

	tcolour c;
	c=t_chroma;
	while (c==t_chroma)
	{
		ypix++;
		c=getpixel(0,ypix);		
		if (ypix>=IC.size.y) return -1;
	}
	return ypix;
}


INT32 getnextxpix(INT32 xpix,INT32 ypix,tcolour t_chroma)
{	
	tcolour c;
	c=t_chroma;
	while (c==t_chroma)
	{		
		xpix++;
		c=getpixel(xpix,ypix);
		if (xpix>=IC.size.x) return -1;
	}
	return xpix;
}
 

void adjustrectdata(tblitrect *rectlist,tcolour t_chroma)
{	
	rectlist->hotspot.assign(0xFFFFFFF0,0xFFFFFFF0);
	tcolour col;
	INT32 scantrans=-1;
	for (INT32 yi=rectlist->a.y;yi<rectlist->b.y;yi++)
	{

		for (INT32 xi=rectlist->a.x;xi<rectlist->b.x;xi++)
		{
			col=getpixel(xi,yi);
			if (col==t_chroma)
			{
				rectlist->attrib |= chromaput;
				if (xi<rectlist->hotspot.x)
					rectlist->hotspot.x=xi;
				if (yi<rectlist->hotspot.y)
					rectlist->hotspot.y=yi;
			}
		}
	}
}

void analyiseimage(tsurface *image,tblitrect *rectlist,INT32 count)
{
	INT32 ypix,nexty=0;
	INT32 xpix,nextx=0;
	
	ypix=1;
	putbitmap(0,0,image);
	TWINrendercontext->setinput(image);

	tcolour t_chroma=getpixel(0,0);
	image->chroma=t_chroma;
	//bar(0,0,300,300);
	INT32 acount=0;
	t_col=getcolour2(0xFF,0xFF,0xFF);
	
	while ((nexty=getnextypix(ypix,t_chroma))!=-1)
	{
		xpix=1;
		while ((nextx=getnextxpix(xpix,ypix,t_chroma))!=-1)
		{
			rectlist[acount].assign(xpix,ypix+1,nextx-1,nexty-1);						
				

			//rectangle(*(trect*)&rectlist[acount]);
			acount++;	
			if (acount>=count)
			{
				acount=count-1;
				t_col=getcolour2(0xFF,0xFF,0);
			}
			xpix=nextx;
		}

		ypix=nexty;
	}
	
	for (INT32 i=0;i<acount;i++)
	{
		 adjustrectdata(&rectlist[i],t_chroma);
		//get transparency info
		//rectangle(*(trect*)&rectlist[i]);
	}			
	TWINrendercontext->setinput(NULL);
}


void captilebitmapx(INT32 x,INT32 y,INT32 x2,tblitrect *c,tsurface *image)
{
	UINT32 w2=(1+x2-x)-(c[0].w+c[2].w)-1;
	putbitmap(x,y,(trect*)&c[0],image,c[0].attrib);
	putbitmap(x+c[0].w,y,w2,c[1].h,(trect*)&c[1],image,c[1].attrib);
	putbitmap(x2-c[2].w,y,(trect*)&c[2],image,c[2].attrib);
}

void captilebitmapy(INT32 x,INT32 y,INT32 y2,tblitrect *c,tsurface *image)
{
	UINT32 h2=(1+y2-y)-(c[0].h+c[2].h)-1;
	putbitmap(x,y,(trect*)&c[0],image,c[0].attrib);
	putbitmap(x,y+c[0].h,c[1].w,h2,(trect*)&c[1],image,c[1].attrib);
	putbitmap(x,y2-c[2].h,(trect*)&c[2],image,c[2].attrib);
}
					
void drawborder(INT32 x1,INT32 y1,INT32 x2,INT32 y2,tblitrect *c,tblitrect *h,tblitrect *v,tsurface *base)
{
	INT32 w1,h1,w2,h2;
	w1=w2=1+x2-x1;
	h1=h2=1+y2-y1;
	w1-=c[0].w+c[1].w;
	w2-=c[2].w+c[3].w;
	h1-=c[0].h+c[2].h;
	h2-=c[1].h+c[3].h;
	x2++;y2++;
	putbitmap(x1,y1,(trect*)&c[0],base,c[0].attrib);
	putbitmap(x2-c[1].w,y1,(trect*)&c[1],base,c[1].attrib);
	putbitmap(x1,y2-c[2].h,(trect*)&c[2],base,c[2].attrib);
	putbitmap(x2-c[3].w,y2-c[3].h,(trect*)&c[3],base,c[3].attrib);
	
	putbitmap(x1+c[0].w,y1,w1,h[0].h,(trect*)&h[0],base,h[0].attrib);
	putbitmap(x1+c[2].w,y2-h[1].h,w2,h[1].h,(trect*)&h[1],base,h[1].attrib);
	
	putbitmap(x1,y1+c[0].h,v[0].w,h1,(trect*)&v[0],base,v[0].attrib);
	putbitmap(x2-v[1].w,y1+c[1].h,v[1].w,h2,(trect*)&v[1],base,v[1].attrib);
	//putbitmap(x2-v[1].w,y2-h[1].h,w2,h[1].h,(trect*)&h[1],base,h[1].attrib);
}


void drawfilledborder(INT32 x1,INT32 y1,INT32 x2,INT32 y2,tblitrect *c,tblitrect *h,tblitrect *v,tblitrect *mid,tsurface *base)
{
	INT32 w1,h1,w2,h2;
	w1=w2=1+x2-x1;
	h1=h2=1+y2-y1;
	w1-=c[0].w+c[1].w;
	w2-=c[2].w+c[3].w;
	h1-=c[0].h+c[2].h;
	h2-=c[1].h+c[3].h;
	x2++;y2++;
	putbitmap(x1,y1,(trect*)&c[0],base,c[0].attrib);
	putbitmap(x2-c[1].w,y1,(trect*)&c[1],base,c[1].attrib);
	putbitmap(x1,y2-c[2].h,(trect*)&c[2],base,c[2].attrib);
	putbitmap(x2-c[3].w,y2-c[3].h,(trect*)&c[3],base,c[3].attrib);
	
	putbitmap(x1+c[0].w,y1,w1,h[0].h,(trect*)&h[0],base,h[0].attrib);
	putbitmap(x1+c[2].w,y2-h[1].h,w2,h[1].h,(trect*)&h[1],base,h[1].attrib);
	
	putbitmap(x1,y1+c[0].h,v[0].w,h1,(trect*)&v[0],base,v[0].attrib);
	putbitmap(x2-v[1].w,y1+c[1].h,v[1].w,h2,(trect*)&v[1],base,v[1].attrib);

	putbitmap(x1+v[0].w,y1+h[0].h,(x2-x1)-v[1].w-v[0].w,(y2-y1)-h[1].h-h[0].h,(trect*)mid,base,mid->attrib);


	//putbitmap(x2-v[1].w,y2-h[1].h,w2,h[1].h,(trect*)&h[1],base,h[1].attrib);
}

void outboxcol(INT32 x1,INT32 y1,INT32 x2,INT32 y2)
{
   drawfilledborder(x1,y1,x2,y2,(tblitrect*)&TWINctrlbm.buttonu_c,(tblitrect*)&TWINctrlbm.buttonu_h,(tblitrect*)&TWINctrlbm.buttonu_v,(tblitrect*)&TWINctrlbm.window_bg0,gadgets);
}

void outbox(INT32 x1,INT32 y1,INT32 x2,INT32 y2)
{
   drawfilledborder(x1,y1,x2,y2,(tblitrect*)&TWINctrlbm.buttonu_c,(tblitrect*)&TWINctrlbm.buttonu_h,(tblitrect*)&TWINctrlbm.buttonu_v,(tblitrect*)&TWINctrlbm.window_bg0,gadgets);
}

void inboxcol(INT32 x1,INT32 y1,INT32 x2,INT32 y2)
{
   drawfilledborder(x1,y1,x2,y2,(tblitrect*)&TWINctrlbm.buttond_c,(tblitrect*)&TWINctrlbm.buttond_h,(tblitrect*)&TWINctrlbm.buttond_v,(tblitrect*)&TWINctrlbm.window_bg0,gadgets);
}

  
bool TWINinit(char *rpath)
{
    int i;	
    strcpy(RESOURCE_PATH,rpath);
	setenvstring("RESOURCE_PATH",RESOURCE_PATH);
	TWINpalette=TWINrendercontext->createpalette(256,0);
    TWINmodalstackhead=0;
    TWINmodalstack[0]=NULL;
	
	UINT32 count=sizeof(TWINctrlbm)>> 4;
    
    if (!(gadgets=getresourceimage("gadgets.pcx",PXFrgb565))) return FALSE;//BMFdontloadpalette|    
		
	TWINrendercontext->setinput(gadgets);
	UINT32 ncol;
	for (i=0;i<48;i++)    
	{
		//get colour in native format of the bitmap
		tcolour c=getpixel(i,0);
		ncol=getrgba(c);
		TWINpalette->setentry(i,*(trgba*)&ncol);		
	}

	TWINtextcol=TWINpalette->nativedata[1];
	TWINtextselcol=TWINpalette->nativedata[2];
	TWINtextdisabledcol=TWINpalette->nativedata[3];
	TWINtextdisabledhighlightcol=TWINpalette->nativedata[4];
	TWINpapercol=TWINpalette->nativedata[5];
	TWINpaperselcol=TWINpalette->nativedata[6];
	
	TWINrendercontext->setinput(NULL);
	analyiseimage(gadgets,TWINctrlbm,count);	    
	vgadgets=TWINrendercontext->createsurface(TWINctrlbm.scroll.getscrolllength(),TWINctrlbm.scroll.getscrollheight(),PXFrgb565);
	TWINrendercontext->setoutput(vgadgets);
	INT32 x=0;
	for (i=0;i<7;i++)
	{
		putbitmap(x,0,(trect*)&TWINctrlbm.scroll[i],gadgets);
		TWINctrlbmv[i].assign(0,x,TWINctrlbm.scroll.getscrollheight(),x+TWINctrlbm.scroll[i].w-1);
		TWINctrlbmv[i].attrib=TWINctrlbm[i].attrib;	
		x+=TWINctrlbm.scroll[i].w;
	}	
	TWINrendercontext->setoutput(NULL);
	vgadgets->rotate90();    
	vgadgets->chroma=gadgets->chroma;
	scratchpad=TWINrendercontext->createsurface(256,256,SC.pfd);
	
	TWINbarminsize=TWINctrlbm.scroll.barleft.w+TWINctrlbm.scroll.barright.w;
	TWINctrlbm.shadowr.attrib=subput;
	TWINctrlbm.shadow.l.attrib=subput;
	TWINctrlbm.shadow.r.attrib=subput;
	TWINctrlbm.shadow.t.attrib=subput;
	TWINctrlbm.shadow.m.attrib=subput;

	TWINtextindent.rassign(TWINctrlbm.paper_v.l.w,TWINctrlbm.paper_h.t.h,TWINctrlbm.paper_v.r.w,TWINctrlbm.paper_h.b.h);
	TWINwindowindent.rassign(TWINctrlbm.window0_v.l.w,TWINctrlbm.window0.lbbord.h,TWINctrlbm.window0_v.r.w,TWINctrlbm.window0_h.b.h);
	TWINborderindent.rassign(TWINctrlbm.window0_v.l.w,TWINctrlbm.window0_h.t.h,TWINctrlbm.window0_v.r.w,TWINctrlbm.window0_h.b.h);
	TWINclientindent.rassign(TWINctrlbm.ce0_v.l.w,TWINctrlbm.ce0_h.t.h,TWINctrlbm.ce0_v.r.w,TWINctrlbm.ce0_h.b.h);
	TWINmenuindent.rassign(TWINctrlbm.menu_v.l.w,TWINctrlbm.titlemenu_h.t.h,TWINctrlbm.menu_v.r.w,TWINctrlbm.titlemenu_h.b.h);		
	TWINpopupmenuindent.rassign(TWINctrlbm.menu_v.l.w,TWINctrlbm.titlemenu_h.t.h,TWINctrlbm.menu_v.r.w,TWINctrlbm.titlemenu_h.b.h);		
	TWINtabindent.rassign(TWINctrlbm.window0_v.l.w,TWINctrlbm.tab.getheight(),TWINctrlbm.window0_v.r.w,TWINctrlbm.window0_h.b.h);

    rectbuffer.create(12*1000); //10k rect buffer
    twSTATE=  TSvisible;    
    return TRUE;
}



void TWINsetroot(tgroup *root_)
{
    root=root_;
    root->target=NULL;
    root->owner=NULL;
    
    global_selected=NULL;
    root->state|=SFvisible;
    root->select();
}

void TWINclose()
{       
    delete root;
    rectbuffer.destroy();
    MOUSEoff();
    MOUSEshutdown();
        
	TWINrendercontext->releasesurface(gadgets);
	TWINrendercontext->releasesurface(vgadgets);
	TWINrendercontext->releasesurface(scratchpad);
	TWINrendercontext->releasepalette(TWINpalette);
}



void TWINsetcliprectcallback(rectlistcallbacktype cliprects)
{
	buildcliprects=cliprects;
}

tgroup *TWINsetpopuptarget(tgroup *target)
{
	return (popup_target=target);
}

tgroup *TWINgetroot()
{
	return root;
}

tgroup *TWINgetpopuptarget()
{
	if (popup_target)
		return popup_target;
	return root;	
}


void TWINsetlockcallbacks(void (*lock)(trect *bounds),void (*unlock)(trect *bounds))
{
	lockcallback=lock;
	unlockcallback=unlock;
}

void TWINlock(trect *bounds)
{
	if (lockcallback && !lockcount)
		lockcallback(bounds);
	lockcount++;
}

void TWINunlock(trect *bounds)
{
	lockcount--;
	if (unlockcallback && !lockcount)
		unlockcallback(bounds);	
}

void TWINsetcontext(trendercontext *ctx)
{
	TWINrendercontext=ctx;
}

