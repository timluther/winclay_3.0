//TWINf basic widgets code
#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <conio.h>

#include "graphics.h"
#include "gprim.h"
#include "msmouse.h"
#include "views.h"
#include "logger.h"
#include "widgets.h"
#include "strtable.h"


#define papercolidx 12

char sliderblob[4]= {48,120,120,48};
char leftarrowb[8]= {8,24,56,120,56,24,8,0};
char rightarrowb[8]= {32,48,56,60,56,48,32,0};




/*void captilebitmapy(INT32 x,INT32 y,INT32 y2,tguirow &row,UINT32 e1,UINT32 e2,UINT32 e3)
{

    putbitmap(x,y,row.x,row[e1].y,row.w,row[e1].h,row.surf,row[e1].attrib);
    INT32 oldy1=SC.viewport.a.y;
    INT32 lasty;
    if (oldy1<y)
      SC.viewport.a.y=y;
    if (SC.viewport.b.y<SC.viewport.a.y) goto exit;
    lasty=y2-row[e2].h;
    if (y<y2)
    {
        y+=(*bm)->size.y;        
        bm++;
        tilebitmapy(x,y,lasty-1,row.x,row[e1].y,row.w,row[e1].h,row.surf,row[e1].attrib);
        bm++;
		putbitmap(x,lasty,row.x,row[e2].y,row.w,row[e2].h,row.surf,row[e2].attrib);
        
    }
exit:
    SC.viewport.a.y=oldy1;   
}


void captilebitmapx(INT32 x,INT32 y,INT32 x2,tguirow &row,UINT32 e1,UINT32 e2,UINT32 e3)
{
	putbitmap(x,y,row[e1].x,row.y,row[e1].w,row.h,row.surf,row[e1].attrib);

    putbitmap(x,y,*bm,useattrib);
    INT32 oldx1=SC.viewport.a.x;
    INT32 lastx;
    if (oldx1<x)
      SC.viewport.a.x=x;
    if (SC.viewport.b.x<SC.viewport.a.x) goto exit;
    lastx=x2-row[e2].w;
    if (x<x2)
    {
        x+=(*bm)->size.x;        
        bm++;
        tilebitmapx(x,y,lastx-1,row[e2].x,row.y,row[e2].w,row.h,row.surf,row[e2].attrib);
        bm++;
		putbitmap(lastx,y,row[e2].x,row.y,row[e2].w,row.h,row.surf,row[e2].attrib);
        
    }
exit:
    SC.viewport.a.x=oldx1;   
}

void tilebitmapx(INT32 x,INT32 y,INT32 x2,INT32 bmx,INT32 bmy,INT32 bmw,INT32 bmh,tsurface *bm,UINT32 attrib)
{
    INT32 oldx2=SC.viewport.b.x;
    if (SC.viewport.b.x>x2)
        SC.viewport.b.x=x2;
    else
        x2=SC.viewport.b.x;
    if (SC.viewport.b.x>SC.size.x-1) goto exit;
    while (x<=x2)
    {
      putbitmap(x,y,bmx,bmy,bmw,bmh,surf,attrib);
      x+=bmw;
    }
exit:
    SC.viewport.b.x=oldx2;
}

void tilebitmapy(INT32 x,INT32 y,INT32 y2,INT32 bmx,INT32 bmy,INT32 bmw,INT32 bmh,tsurface *bm,UINT32 useattrib)
{
    INT32 oldy2=SC.viewport.b.y;
    if (SC.viewport.b.y>y2)
       SC.viewport.b.y=y2;
    else
       y2=SC.viewport.b.y;
    if (SC.viewport.b.y>SC.size.y-1) goto exit;
    while (y<=y2)
    {
      putbitmap(x,y,bmx,bmy,bmw,bmh,surf,attrib);
      y+=bmh;
    }
exit:
    SC.viewport.b.y=oldy2;
}*/

void textboxb(INT32 x1,INT32 y1,INT32 x2,INT32 y2)
{
	drawborder(x1,y1,x2,y2,(tblitrect*)&TWINctrlbm.paper_c,(tblitrect*)&TWINctrlbm.paper_h,(tblitrect*)&TWINctrlbm.paper_v,gadgets);    
}


void textboxfilledb(INT32 x1,INT32 y1,INT32 x2,INT32 y2)
{
    drawfilledborder(x1,y1,x2,y2,(tblitrect*)&TWINctrlbm.paper_c,(tblitrect*)&TWINctrlbm.paper_h,(tblitrect*)&TWINctrlbm.paper_v,(tblitrect*)&TWINctrlbm.paper_bg,gadgets);
}
////////////////////////////////////////////////////////////////////////////////////////
// LIST BOX TYPE



#define rt_opcode 0x1
#define RTjustifymask   0x7
#define RTjustifyleft   0x1
#define RTjustifyright  0x2
#define RTjustifycentre 0x4
#define RTbold          0x8
#define RTitalic        0x10
#define RTunderlined    0x20
#define RTstrikethrough 0x40
#define RThyper         0x80
#define RTbulletmask    0x100+0x200+0x400
#define RTbullet1       0x100
#define RTbullet2       0x200
#define RTbullet3       0x400
#define RTwrapmoderect  0x800
#define RTwrapmodepixel 0x1000


#define opmask 128
#define op_pop opmask
#define op_statechange 1
#define op_colourchange 2
#define op_fontchange 3
#define op_hyperlink 4
#define op_pixelspacer opmask+1
#define op_break       opmask+2
#define op_insertimage opmask+3   
#define op_insertcontrol opmask+4




richtext::richtext(INT32 x,INT32 y,INT32 w,INT32 h,char *string_):tgroup(x,y,x+w,y+h)
{
    string=string_;
    stacktop=stack;
    offset=c=0;
    offset.y=27;
    state.colourwrd=0;
    state.font=&t_charset;
    state.attribute = RTjustifyleft;
    topstate=state;
    linkcolour.r=0;linkcolour.g=0;linkcolour.b=255;linkcolour.a=0;
    
    calculatestart();
}

void richtext::calculatexbounds(char *chr,bool changestate)
{
    UINT32 xlen=0,lastxlen=0;
    UINT32 breakfound=FALSE;
    tfontchar *ch;
    char *lastspace=NULL;
    //width of view now, later it'll be the width of the current textflow region...
    //once I work out what that is.
    char op;
    UINT32 code;
    UINT32 w=width();
    while (!*chr) chr++;    
    while (*chr && !breakfound)
    {
       if (*chr==rt_opcode)
       {
           chr++;op=*chr;chr++;           
           //check for state change with data
           if (op & opmask)
           {              
               switch (op)
               {    
                 case op_pixelspacer: xlen+=code;break;
                 case op_break: breakfound=TRUE;;break;
               }                 
           }else
           {
               code=(UINT32)*chr;
               chr+=4;
               if (changestate)
                  push(op,code);                                    
           }
           lastspace=chr;       
       }else
       {
           if (*chr==' ')
           {
              lastspace=chr;
              lastxlen=xlen;
           }
           ch=&t_charset.offsets[*chr];
           xlen+=FNTgetcharwidth(ch);
           if (xlen>w)
             breakfound=TRUE;
       }
       chr++;
    }
    if (lastspace && breakfound)
    {
      nextbreak=lastspace;
      xlen=lastxlen;      
    }else
      nextbreak=chr;
      
    //maybe not nessicary
    ch=&t_charset.offsets[*chr];
    //remove trailing spaces
    
    while (!*chr)
    {
      xlen-=FNTgetcharwidth(ch);
      chr--;
    }

    xminbounds=w-xlen;xmaxbounds=w;    
    switch (state.attribute & RTjustifymask)
    {
        
        case RTjustifyright:  xminbounds=w-xlen;xmaxbounds=w;break;
        case RTjustifycentre: w>>=1;xlen>>=1;xminbounds=w-xlen;xmaxbounds=w+xlen;break;
        default:   xminbounds=0;xmaxbounds=xlen;break;
    }
}

void richtext::handleevent(tevent *event)
{
    tview::handleevent(event);
}

void richtext::setoffset(tpoint offset_)
{
    offset=offset_;
}

void richtext::calculatestart()
{
    linestart=string;
    char *oldlinestart=string;
    INT32 ypos=-offset.y;
    c.y=ypos;
    while (ypos<0)
    {
        c.y=ypos;
        oldlinestart=linestart;
        calculatexbounds(linestart,TRUE);
        linestart=nextbreak;
        ypos+=t_charset.ysize+1;
    }
    linestart=oldlinestart;
    topstate=state;
}



void richtext::push(UINT8 op,UINT32 data)
{
   // if (stacktop>=&stack[512])
   //   return;
   // stacktop->op=op;
    //sprintf(buf,"Found data (code: %i %i)",op,data);
    //logmessage((void*)&stacktop->colour,buf);
    switch (op)
    {
        case op_statechange:
     //     stacktop->data=state.attribute;
          state.attribute=data;
        break;
        case op_colourchange:
       //   stacktop->colour=state.colour;
          state.colourwrd=data;
          t_col=getcolour2(state.colour.r,state.colour.g,state.colour.b);
          //logmessage((void*)&stacktop->colour,"found colour");
        break;
        case op_fontchange:
       //   stacktop->font=state.font;
          state.font=(tfont*)data;
        break;
        case op_hyperlink:
        //  state.attribute |= RThyper;
        //  stacktop->hyperlink=(char*)data;
          state.colour=linkcolour;
          t_col=getcolour2(state.colour.r,state.colour.g,state.colour.b);
          state.attribute |=RThyper;
          
        break;        
    }
    //stacktop++;    
}

stackelem *richtext::pop()
{
    return NULL;
    /*if (stacktop<=stack)
      return NULL;
    stackelem *retme=stacktop;
    
    switch (retme->op)
    {
        case op_statechange:
          state.attribute=stacktop->data;          
        break;
        case op_colourchange:
          state.colour=stacktop->colour;
          t_col=getcolour(state.colour.r,state.colour.g,state.colour.b);
        break;
        case op_fontchange:
          state.font=stacktop->font;
        break;
        case op_hyperlink:
          state.attribute &= ~RThyper;
        break;
    }

    stacktop++;
    return retme;*/
}

void richtext::draw()
{
    char *chr=linestart;
    tfontchar *ch;
    UINT8 op;
    UINT32 code;
    
    tpoint pixoff;
    stackelem *oldstacktop=stacktop;
    state=topstate;
    t_col=getcolour2(0,0,0);
    textboxfilledb(a.x,a.y,b.x,b.y);
    pixoff.y=a.y+c.y;
    
    
    while (*chr && (pixoff.y<b.y))
    {
       //get next line break and starting x position
       calculatexbounds(chr,FALSE);
       pixoff.x=a.x-offset.x+xminbounds;
       while (!*chr) chr++;
       while (chr<nextbreak)
       {
           if (*chr==rt_opcode)
           {
               chr++;
               op=*chr;
               chr++;               
               //check for state change with data
               if (op & opmask)
               {
                   switch (op)
                   {
                       case op_pop:pop();break;
                       case op_pixelspacer: pixoff.x+=code;break;       
                   }                   
               }else
               {
                   code=(UINT32)*chr;
                   chr+=4;
                   push(op,code);                                    
               }             
           }else
           {
               //check mode here           
               ch=&t_charset.offsets[*chr];
               drawbytes(pixoff.x,pixoff.y,ch->bytewidth,ch->height,ch->data);
               if (state.attribute & RTbold)
                 drawbytes(pixoff.x+1,pixoff.y,ch->bytewidth,ch->height,ch->data);
               if (state.attribute & RTunderlined)
                 hline(pixoff.x,pixoff.x+FNTgetcharwidth(ch),pixoff.y+ch->height);
               if (state.attribute & RTstrikethrough)
                 hline(pixoff.x,pixoff.x+FNTgetcharwidth(ch),pixoff.y+(ch->height>>1));                 
                 
               pixoff.x+=FNTgetcharwidth(ch);           
           }        
           chr++;
       }
       pixoff.y+=t_charset.ysize+1;
    }
    stacktop=oldstacktop;
}




void listelem::set(char *txt_,INT32 offset_,UINT32 attrib_,UINT32 contextattrib_,UINT8 image_)
{
    if (txt_)
        txt=txt_;
	else
	    txt=str_nodata;
    offset=offset_;
    contextattrib=contextattrib_;
    attrib=attrib_;
    image=image_;
}

UINT32 findlongesttext(listelembase *list,UINT32 nopts)
{    
    UINT32 len=0;
    UINT32 clen;
    while (nopts>0)
    {
        clen=FNTgetwidth(list->txt);        
        if (clen>len) len=clen;
        list++;
        nopts--;
    }
    return len;
};

listbox::listbox(INT32 x,INT32 y,INT32 w,INT32 h,listelem *info_,INT32 maxopts_,bool destroyold_)
:tview(x,y,x+w,y+h)
{
    options|= (OFtabcycleselect+OFnooverlap);
    images=NULL;
    imgwidth=0;
    rightclickfunc=NULL;
    destroyold=destroyold_;    
    makelist(maxopts_,info_,1);
    drawmode=listdrawall;
}

listbox::~listbox()
{
    kill_list();
}

void listbox::setitem(INT32 opt,char *txt,INT32 offset,UINT32 attrib,UINT32 contextattrib,UINT8 img)
{
    if (opt<maxopts)
       info[opt].set(txt,offset,attrib,contextattrib,img);                    
}
 
void listbox::makelist(INT32 nopts,listelem *list,char drawit)
{
    maxopts=nopts;
    if (nopts==0)
    {
       info=NULL;
    }else
    {
      if (list!=NULL)
        info=list;
      else
      {
        info=new listelem[maxopts+1];
		//for (int i=0;i<maxopts;i++)
		//	info[i].txt=str_nodata;        
      }
    }
    //memset(info,sizeof(listelem)*(maxopts+1),0);
	
    changed=FALSE;
    sel=off=oldsel=0;
    calcinternalvars();
    if (drawit)
       drawview();             
}


void listbox::calcinternalvars()
{
    valuedata valdata;
    maxylen=calcylen();
    INT32 temp=maxopts-maxylen;
    if (temp<0) temp=0;
    valdata.min=0;valdata.max=temp;valdata.val=off;
    //this could cause too many redraws as changebounds uses it also and
    //it invokes the scrollbar's setrange procedure
    //this then forces a redraw; need to find some way around this that's not too messy
    sendcommand(CMDydeltachanged,owner,&valdata);
    
}

INT32 listbox::calcylen()
{
   return MIN(1+((b.y-a.y-8) / t_charset.ysize),maxopts);
}

void listbox::changebounds(trect &bounds)
{
    INT32 oldheight=height(); 
    tview::changebounds(bounds);    
    if (height()!=oldheight)
    {
       twSTATE&=~TSvisible;
       calcinternalvars();
       twSTATE|=TSvisible;     
    }
}

void listbox::setlist(INT32 nopts,listelem *list,char drawit)
{
    kill_list(FALSE);
    makelist(nopts,list,drawit);        
}

void listbox::kill_list(char drawit)
{
    if ((maxopts>0)&&(destroyold))
       delete [] info;
    maxopts=0;
    if (drawit)
       drawview();
}

void listbox::setval(INT32 val_)
{
    INT32 oldoff;
    if (val_<0) val_=0;
    if (val_>maxopts-1) val_=maxopts-1;
    if ((sel!=val_) || (!changed))
    {
        changed=TRUE;        
        
        oldoff=off;
        oldsel=sel;
        sel=val_;
        if (sel>off+(maxylen-1))
            off=sel-(maxylen-1);
        if (sel<off)
            off=sel;
        if (off<0) off=0;
        if (oldoff!=off)
        {
            drawmode=listdrawlist;            
            sendcommand(CMDydeltavalchanged,owner,&off);                                    
        }else        
            drawmode=listdrawchange;
        
        drawview();
        drawmode=listdrawall; 
        evcallback(CMDselectedchanged);       
    }                       
}

void listbox::drawitem(INT32 itemno)
{
    INT32 yoff;
    INT32 xoff;
    if ((itemno>=off)&&(itemno<=maxopts))
    {

        yoff=a.y+TWINtextindent.a.x+(itemno-off)*t_charset.ysize;
        xoff=a.x+TWINtextindent.a.y;   
        t_fillcol=TWINpapercol;
        if (itemno==sel)
        {
            if ((state & SFselected+SFmouseover))
              t_fillcol=TWINpaperselcol; 
            t_col=TWINtextselcol;
        }else
           t_col=TWINtextcol;
        if (info[itemno].offset>0)
        {
            bar(xoff,yoff,xoff+info[itemno].offset-1,yoff+t_charset.ysize-1);
            xoff+=info[itemno].offset;
        }
        
        bar(xoff,yoff,b.x-TWINtextindent.b.x,yoff+t_charset.ysize-1);
        
        if ((info[itemno].image)&&(images))
        {
            UINT32 offset=((info[itemno].image-1)* imgwidth);
            trect oldvp=SC.viewport;    
            SC.viewport.rintersect(xoff,yoff,xoff+imgwidth-1,yoff+images->size.y-1);                 
            //putbitmap(xoff-offset,yoff,images,0xFFFFFFFF);
            SC.viewport=oldvp;   
            xoff+=imgwidth+4;
        }        
		
		outtextxy(xoff,yoff,info[itemno].txt);
		
			
    }
}
      
void listbox::handleevent(tevent *event)
{
    if (maxopts<=0) return;
    INT32 oldoff,listi;
    tview::handleevent(event);
    if ((state & SFmodal)!=0)
    {
        setval(off+(mp.y-a.y-TWINtextindent.a.y) / t_charset.ysize);
        if (MOUSEreleased)
        {
            evcallback(CMDdragselectstopped);
            TWINendmodal();
        }
    }else    
    {
		if ((event->what & EVmousewheel))
		{
			setval(sel-getsign(mwheel));
			event->what=0;
		}
			
		if (event->what & EVmousedoubleclicked)					
                evcallback(CMDitemdclicked);              
        
        if ((event->what & EVmousepressed)!=0)
        {
            if ((mb==2) && (rightclickfunc))
              rightclickfunc(this,event);              
            else
            {
              TWINsetmodal(this);                          
            }
            event->what=0;                       
        }           
      if (event->what == EVcommand)
      switch (event->command)
      {
          case CMDyvalchanged:
              oldoff=off;
              off=*(UINT32*)event->infoptr;
              if (oldoff!=off)
              {
                  drawmode=listdrawlist;
                  drawview();
                  drawmode=listdrawall; 
              }
              break;                
      }  
   }
   if (event->what & EVkeyboard)
        {
           switch (scancode)
           {
               case upkey:setval(sel-1);event->what=0;break;
               case downkey:setval(sel+1);event->what=0;break;
               case pageupkey:setval(0);event->what=0;break;
               case pagedownkey:setval(maxopts);event->what=0;break;        
           }        
           switch (charcode)
           {
              case returnkey:              
                 evcallback(CMDitemdclicked);
                 event->what=0;
                 break;
              default:
                 if ((charcode>0x21))
                 {                     
                     if ((info[sel].txt[0]==charcode)&&(sel<maxopts-1))
                       if (info[sel+1].txt[0]==charcode)
                       {
                         setval(sel+1);
                         event->what=0;
                         return;
                       }                                          
                     listi=0;                     
                     while ((listi<maxopts)&&(info[listi++].txt[0]!=charcode));                                                 
                     if (listi<maxopts)
                         setval(listi);
                     event->what=0;
                 }              
           }
      } 
}


void listbox::draw()
{
    INT32 i;
    trect ir=trect(a.x+TWINtextindent.a.x,a.y+TWINtextindent.a.y,b.x-TWINtextindent.b.x,b.y-TWINtextindent.b.y);
    if (drawmode==listdrawall)
    {        
        textboxb(a.x,a.y,b.x,b.y);                
        //thickrectangle(ir.a.x,ir.a.y,ir.b.x,ir.b.y,2);
    }
    
    trect oldvp=SC.viewport;    
    SC.viewport.rintersect(ir);
    t_fillcol=TWINpapercol;        
    if (maxopts==0)
    {
       t_col=TWINtextcol;       
       bar(ir.a.x,ir.a.y,ir.b.x,ir.b.y);              
       outtextxy(a.x+4,a.y+4,"this view is empty");
    }else 
    switch (drawmode)
    {
      case listdrawall:
      case listdrawlist:

          i=a.y+TWINtextindent.a.y+(maxylen*t_charset.ysize);
          if (i<=ir.b.y)                        
              bar(ir.a.x,i,ir.b.x,ir.b.y);              
                          
          for (i=off;i<off+maxylen;i++)
              drawitem(i);
          
          
        break;
      case listdrawchange:
          drawitem(oldsel);      
      case listdrawselected:
          drawitem(sel);
      break;
    }
    SC.viewport=oldvp;        
}

bool listbox::setstate(UINT32 astate, bool enable)
{
    if (tview::setstate(astate,enable))
    {        
        if ((maxopts!=0)&&(astate & SFselected+SFmouseover))
        {
            drawmode=listdrawselected;
            drawview();
            drawmode=listdrawall; 
        }        
        return 1;
    }else
        return 0;    
}

inline void swapb(INT8 &a, INT8 &b){INT32 t;t=a;a=b;b=t;}


bool listbox::find(INT32 from,char *txt)
{
	if (info)
    for (INT32 i=from;i<maxopts;i++)    
		if (!strcmp(info[i].txt,txt)) return true;
    
    return false;
}
void listbox::sortlist(INT32 from,INT32 to)
{

  if (info)
  if (from<maxopts)
  for (INT32 i1=from;i1<maxopts;i1++)
    for (INT32 i2=i1;i2<maxopts;i2++)
    {    
       if (strgreater(info[i2].txt,info[i1].txt)==0)
          fswapi(&info[i2],&info[i1],sizeof(listelem));
    }
 }






////////////////////////////////////////////////////////////////////////////////////////
// TREEBOX TYPE

treebox::treebox(INT32 x,INT32 y,INT32 w,INT32 h,ttree *tree_,gettreeitemdatafunc gettreeitemdata_):tview(x,y,x+w,y+h)
{
    currsel=NULL;
    tree=tree_;
    gettreeitemdata=gettreeitemdata_;
    options|= OFtabcycleselect+OFnooverlap;
    drawmode=listdrawall;
    height=t_charset.ysize;
    irect.rassign(a.x+4,a.y+4,b.x-4,b.y-4);
    off=0;
}

treebox::~treebox()
{
    TTdestroytree(tree->root);    
}

void treebox::changebounds(trect &bounds)
{
  INT32 oldheight=tview::height();
  tview::changebounds(bounds);  
  irect.rassign(a.x+4,a.y+4,b.x-4,b.y-4);

  if (tview::height()!=oldheight)
  {
      twSTATE&=~TSvisible;
      setbarrange();
      twSTATE|=TSvisible;
  }
}

UINT32 *treebox::getattrib(ttreenode *node)
{
    void *dummy;
    UINT32 *attrib;
    gettreeitemdata(node,(char **)&dummy,(tsurface **)&dummy,&attrib);
    return attrib;
}


void treebox::clearsel()
{
    oldsel=currsel=NULL;
    drawview();
}

void treebox::setsel(ttreenode *newelem)
{
    if ((newelem!=currsel))
    {
        INT32 ypos,maxvisopts,oldoff;
        oldoff=off;
        if (newelem)
        {
          ypos=getypos(newelem);
          maxvisopts=irect.height()/height;          
          if (ypos<0)                
            off+=ypos;
          else if (ypos>maxvisopts)
            off+=(ypos-maxvisopts);
          if (off<0) off=0;
        }
        oldsel=currsel;
        currsel=newelem;
        if (oldoff!=off)
        {
            drawmode=listdrawlist;
            sendcommand(CMDydeltavalchanged,owner,&off);            
        }else
            drawmode=listdrawchange;
        
        drawview();
        drawmode=listdrawall;
        evcallback(CMDselectedchanged);
    }
}

/*tree_elem *treebox::addelem(tree_elem *parent,tree_elem *newelem,char drawit)
{    
    (tree_elem*)TTaddchildtonode(&tree,(ttreenode*)parent,(ttreenode*)newelem);
    if (drawit)
    {        
        drawmode=listdrawlist;
        drawview();
        drawmode=listdrawall;
    }
    setbarrange();
    return newelem;
}

tree_elem *treebox::addelem(tree_elem *parent,char *txt,tsurface *pic,UINT8 tattrib,void *data,char drawit)
{
    ref=new tree_elem(txt,pic,tattrib,data);    
    return addelem(parent,ref,drawit);
}


{
    TTdestroytree(node);
    if (node==(tree_elem*)tree.root)
      tree.root=NULL;
    if (drawit)
       drawview();
}*/

INT32 treebox::getypos(ttreenode *celem)
{
    INT32 ypos=-off;

    ttreenode *ccelem=tree->root;
    while ((ccelem!=celem))
    {
        if (ccelem==NULL) return -1;
        ccelem=getnextitem(ccelem);
        ypos++;     
    }   
    return ypos;
}


void treebox::update()
{
    calcylen();
    drawview();
}

INT32 treebox::calcylen()
{
    INT32 count=0;
    ttreenode *celem=tree->root;
    while (celem!=NULL)
    {        
        celem=getnextitem(celem);        
        count++;     
    }
    return count;
}

void treebox::setbarrange()
{        
    INT32 temp=calcylen()-(irect.height()/height);
        
    if (temp<0) temp=0;
    BOUND(off,0,temp);
    valuedata valdata;
    valdata.min=0;valdata.max=temp;valdata.val=off;
    sendcommand(CMDydeltachanged,owner,&valdata);
}

void treebox::drawitem(ttreenode *elem,INT32 ypos,bool justtext)
{
    if (!elem) return;
    tsurface *pic;char *txt;UINT32 *attrib;
    gettreeitemdata(elem,&txt,&pic,&attrib);
    t_fillcol=TWINpapercol;    
    t_col=TWINtextcol;
    INT32 hlf=height >> 1;
    INT32 textlen=FNTgetwidth(txt);
    INT32 xpos,i;
    INT32 depth=TTgetdepth(elem->parent);
    
    ttreenode *parent=elem->parent;

    xpos=irect.a.x+(depth << 4);  
    if (!justtext)
    {        
        if ( (currsel==elem)&&((state & SFselected)!=0) )
        {
            bar(irect.a.x,ypos,xpos+32,ypos+height-1);
            bar(xpos+textlen+34,ypos,irect.b.x,ypos+height-1);
            t_fillcol=TWINpaperselcol;            
            bar(xpos+33,ypos,xpos+textlen+33,ypos+height-1);        
        }else
            bar(irect.a.x,ypos,irect.b.x,ypos+height-1);
        for (i=0;i < depth;i++)
        {
            if (parent->next!=NULL)
                vline(xpos-8,ypos,ypos+height-1);
            xpos-=16;
            parent=parent->parent;
        }
    
        xpos=irect.a.x+(depth << 4);        
    
        if (elem->child==NULL)
        {
            if (elem!=tree->root)
                vline(xpos+8,ypos,ypos+hlf);
            hline(xpos+9,xpos+24,ypos+hlf);
            if (elem->next!=NULL)
                vline(xpos+8,ypos+hlf,ypos+height-1);
        }
        else        
        {
            if (elem!=tree->root)
                vline(xpos+8,ypos,ypos+hlf-squaresize);
            rectangle(xpos+8-squaresize,ypos+hlf-squaresize,xpos+8+squaresize,ypos+hlf+squaresize);
            hline(xpos+10-squaresize,xpos+6+squaresize,ypos+hlf);
            if ((*attrib & TEcollapsed)!=0)
            {
                vline(xpos+8,ypos+hlf-(squaresize-2),ypos+hlf+(squaresize-2));
                hline(xpos+hlf+squaresize,xpos+24,ypos+hlf);
            }
            else
            {
                vline(xpos+24,ypos+hlf,ypos+height-1);       //linking down to next
                hline(xpos+hlf+squaresize,xpos+23,ypos+hlf); //horizontal link
            }
            if (elem->next!=NULL)
                vline(xpos+8,ypos+hlf+squaresize,ypos+height-1);                        
        }
        //if (pic!=NULL)                        
        //    putbitmap(xpos+16,ypos+hlf-8,pic,0xFFFFFFFF);
    }else
    {
        if ( (currsel==elem)&&((state & SFselected)!=0) )        
            t_fillcol=TWINpaperselcol;
        bar(xpos+33,ypos,xpos+textlen+33,ypos+height-1);
    }
    xpos=irect.a.x+(depth << 4)+16;
    if (currsel==elem)
        t_col=TWINtextselcol;        
    
    outtextxy(xpos+18,ypos+(hlf-(t_charset.ytextsize >> 1)),txt);
}

void treebox::draw()
{
    INT32 ypos;
    ttreenode *celem;
    trect oldvp=SC.viewport;

    if (drawmode==listdrawall)
    {
        textboxb(a.x,a.y,b.x,b.y);
        t_col=TWINpapercol;
        //thickrectangle(a.x+2,a.y+2,b.x-2,b.y-2,2);        
    }
    
    SC.viewport.rintersect(irect);
    
    switch (drawmode)
    {
    case listdrawchange:               
        drawitem(oldsel,irect.a.y+getypos(oldsel)*height,TRUE);                
    case listdrawselected:        
        drawitem(currsel,irect.a.y+getypos(currsel)*height,TRUE);        
        break;
    case listdrawafter:               
    case listdrawall:
    case listdrawlist:
        if (drawmode==listdrawafter)
        {
            celem=ref;       
            ypos=irect.a.y+(getypos(ref)*height);
        }else
        {
            ypos=irect.a.y;
            celem=tree->root;    
            INT32 i;
            for (i=0;i<off;i++)
               celem=getnextitem(celem);
        }    
        while ((celem!=NULL)&&(ypos<irect.b.y))
        {
            drawitem(celem,ypos);
            celem=getnextitem(celem);
            ypos+=height;     
        }
        if (ypos<irect.b.y)
        {
            t_fillcol=TWINpapercol;    
            bar(irect.a.x,ypos,irect.b.x,irect.b.y);
        }       
        break;
    }
    SC.viewport=oldvp;
}

ttreenode *treebox::getprevitem(ttreenode *celem)
{
    ttreenode  *ccelem=tree->root;
    ttreenode  *retelem=NULL;
    while ((ccelem!=celem)&&(ccelem!=NULL))
    {
       retelem=ccelem;
       ccelem=getnextitem(ccelem);
    }
    return retelem;
}

ttreenode  *treebox::getnextitem(ttreenode  *celem)
{
    
    
    if ((celem->child!=NULL)&&((*getattrib(celem) & TEcollapsed)==0))        
        celem=celem->child;        
    else if (celem->next!=NULL)        
        celem=celem->next;        
    else if (celem->parent!=NULL)        
        celem=celem->parent->next;                     
    else        
        celem=NULL;
    return celem;
}            

ttreenode  *treebox::getitem(INT32 y)
{
    INT32 ypos=-off;
    ttreenode  *celem=tree->root;
    while (celem!=NULL)
    {
        if (y==ypos) return celem;
        celem=getnextitem(celem);        
        ypos++;     
    }
    return NULL;
}

         
void treebox::handleevent(tevent *event)
{
  INT32 yval;
  INT32 oldoff;
  tview::handleevent(event);
  ttreenode  *sel;
  if ((state & SFmodal)!=0)
  {
      yval=(mp.y-irect.a.y) / height;
      sel=getitem(yval);
      if (sel!=0)
      setsel(sel);
      if (MOUSEreleased)
          TWINendmodal();
      event->what=0;
  }else
  if (tree->root!=NULL)
  {      
      if ((event->what & EVmousepressed)!=0)
      {          
            yval=(mp.y-irect.a.y) / height;
            sel=getitem(yval);
            if (sel!=NULL)
            {         
              if (sel->child!=NULL)
              {
                  //calculate the little rectangle for expanding, contracting and check it against the mouse position
                  INT32 depth=TTgetdepth(sel);
                  INT32 xpos=irect.a.x+((depth-1) << 4)+8;
                  INT32 ypos=irect.a.y+(yval*height)+(height >> 1);    
                  trect rect;               
                  rect.rassign(xpos-squaresize,ypos-squaresize,xpos+squaresize,ypos+squaresize);
                  if (rect.rcontains(mp.x,mp.y))
                  {
                       *getattrib(sel)^=TEcollapsed;
                       ref=sel;                                              
                       drawmode=listdrawafter;
                       drawview();
                       drawmode=listdrawall;                       
                       setbarrange();                       
                  }else
                     setsel(sel);
                  TWINsetmodal(this);
              }else
              {              
                  setsel(sel);
                  TWINsetmodal(this);
              }
              
                //TWINsetmodal(this);            
              if (MOUSEdoubleclicked)            
                  evcallback(CMDitemdclicked);
              event->what=0;          
        }
     }else
     if (event->what == EVkeydown)
     {
         switch (scancode)
         {
             case upkey:setsel(getprevitem(currsel));event->what=0;break;
             case downkey:setsel(getnextitem(currsel));event->what=0;break;
             case pageupkey:setsel(tree->root);event->what=0;break;
             case pagedownkey:;break;        
         }        
         switch (charcode)
         {
            case returnkey:              
               evcallback(CMDitemdclicked);
               event->what=0;
               break;
         }
     }else if (event->what == EVcommand)
     switch (event->command)
     {
         case CMDyvalchanged:
             oldoff=off;
             off=*(INT32*)event->infoptr;
             if (oldoff!=off)
             {
                 drawmode=listdrawlist;
                 drawview();
                 drawmode=listdrawall; 
             }
             break;                
     }        
  }
  
}

bool treebox::setstate(UINT32 astate, bool enable)
{
    //check for difference
    if (tview::setstate(astate,enable))
    {
        if ((astate & SFselected)!=0)
        {
            drawmode=listdrawselected;
            drawview();
            drawmode=listdrawall;
        }
        return 1;
    }else
        return 0;
}


tbutton::tbutton(INT32 x,INT32 y,UINT32 _dstate,UINT32 _docommand,UINT8 hotkey,tsurface *surf_,tblitrect *buttons_)
:tview(x,y,x+buttons_[0].w-1,y+buttons_[0].h-1)
{
	options|=OFkeygrab;			
	hotkeycode=hotkey;  
    docommand=_docommand;	
	surf=surf_;
	buttons=buttons_;	
	dstate=_dstate|BAnobox|BAnooffset;
	text=NULL;
}
	

 
tbutton::tbutton(INT32 x,INT32 y,INT32 w,INT32 h,UINT32 _dstate,UINT32 _docommand,UINT8 hotkey,char *txt,tsurface *surf_,tblitrect *buttons_)
:tview(x,y,x+w-1,y+h-1)
{
	options|=OFkeygrab|OFautoraise;		
	hotkeycode=hotkey;  
    docommand=_docommand;
	surf=surf_;
	buttons=buttons_;
	dstate=_dstate;
	text=txt;
}

tbutton::tbutton(INT32 x,INT32 y,UINT32 _dstate,UINT32 _docommand,char *txt)
:tview(x,y,x+TWINctrlbm.checkbox.b0.w+FNTgetwidth(txt)+4,y+TWINctrlbm.checkbox.b0.h-1)
{
	options|=OFkeygrab|OFautoraise;		
	hotkeycode=0x13;  
    docommand=_docommand;
	surf=gadgets;
	buttons=&TWINctrlbm.checkbox.b0;
	dstate=_dstate;
	text=txt;
}
 
#define threshold 1

void tbutton::handleevent(tevent *event)
{
  char inviewpc;
  static UINT32 mcount;

  tview::handleevent(event);

  if ((state & SFmodal)!=0)
  {
      inviewpc=inview(mp.x,mp.y);

      if (action()==BMperiodaction)
      {
          if ((TIMEgettime()-mcount)>threshold)
          {
             mcount=TIMEgettime();
             evcallback(docommand);
          }
      }else if (action()==BMmoveaction)
	  {
		  if (MOUSEmoved)
		  {
			  evcallback(docommand);
		  }
	  }
         
      if ((MOUSEreleased)||(!inviewpc))
      {
         
         if ((action()!=BMtoggleaction)&&(action()!=BMglobaltoggleaction))
         {
           if ((inviewpc)&&(action()!=BMperiodaction))
               evcallback(docommand);
         
           dstate^= BAdown;
           drawview();
         }  
         TWINendmodal();
      }
                            
  } else   
  if ((event->what & EVmousepressed)!=0)
  {
      if ((action()==BMglobaltoggleaction)&&(dstate&BAdown))
      {
          event->what=0;
          return;
      }
      
        dstate^=BAdown;
        drawview();
      
        // outtextxy(200,200,itoa(rect_count,buf,10));
        switch (action()) 
		{
          case BMglobaltoggleaction:
          case BMtoggleaction:
            evcallback(docommand);
		  case BMmoveaction:
		  case BMreleaseaction:
            TWINsetmodal(this);                
          break;		  
          case BMclickaction:
            evcallback(docommand);
            dstate^= BAdown;
            drawview();
          break;                      
          case BMperiodaction:
            evcallback(docommand);          
            mcount=TIMEgettime()+threshold;          
            TWINsetmodal(this);          
          
          break;
        
        } //} of switch
         
      event->what=0;
      
    } else if (event->what & EVkeydown)
      if  ( (state & SFselected) && (charcode==13) )
      {
        if ((action() & BMmask)==BMtoggleaction)
        {
          dstate^= BAdown;
          drawview();
        }
        evcallback(docommand);
        event->what=0;
      }  
}

void tbutton::draw()
{
  tpoint pos;
  
  UINT8 downoffset=((dstate & BAnooffset)==0)?isdown():0;
    
  if (((dstate & BAdrawonlyborder)==0))
  {
     if (!nobox())
     {        
        if (isdown())
           inboxcol(a.x,a.y,b.x,b.y);        
        else
           outboxcol(a.x,a.y,b.x,b.y);
     }
     t_col=TWINtextcol;
     
     UINT32 fntwidth=(text!=NULL)?FNTgetwidth(text)>>1:0;     
     UINT32 bmwidth=(surf!=NULL)?(buttons[0].w>> 1):0;
     if ((fntwidth!=0)&&(bmwidth!=0))fntwidth++;

     pos.x=downoffset+a.x;
     if (xcentre())                           
         pos.x+=halfx()-(fntwidth+bmwidth);                                
                    
     if (surf!=NULL)
     {                      
          tblitrect *cbm;
          if (isdown())
              cbm=&buttons[2];
          else if (ismouseover())
              cbm=&buttons[1];
          else
              cbm=&buttons[0];
          pos.y=downoffset+a.y;
          if (ycentre())
              pos.y+=halfy()-(cbm[0].h>>1); 
                    
          putbitmap(pos.x,pos.y,(trect*)cbm,surf,cbm->attrib);
          pos.x+=cbm->w+1;
      }
      
      if (text)
      {
          pos.y=downoffset+a.y;
          if (ycentre())
              pos.y+=halfy()-(t_charset.ytextsize>>1);
          outtextxy(pos.x,pos.y,text);
      }
  }
}

bool tbutton::setstate(UINT32 astate,bool enable)
{
  if (tview::setstate(astate,enable))
  {
      if ((astate & (SFselected+SFmouseover)) )
      {
          if ((dstate & BAnobox)==0)
              dstate|=BAdrawonlyborder;
          drawview();
          dstate&=~BAdrawonlyborder;
      }
      return 1;
  }else
      return 0;   
}

////////////////////////////////////////////////////////////////////////////////////////
// BUTTON COLLECTION; used as radio buttons


tcollection::tcollection(INT32 x1,INT32 y1,INT32 x2,INT32 y2):tgroup(x1,y1,x2,y2)
{
    options |= OFnooverlap;
    lastselected=NULL;
}

tcollection::~tcollection()
{
  
}

void tcollection::handleevent(tevent *event)
{
    tgroup::handleevent(event);
    if (event->what==EVcommand)
    {
        if (event->infoptr!=lastselected)
        {
            if (lastselected!=NULL)
            {
                lastselected->dstate &= ~BAdown;
                lastselected->drawview();                
            }
            lastselected=(tbutton*)event->infoptr;                            
        }     
        evcallback(event->command);
    }
}

void tcollection::draw()
{
    t_fillcol=0;
    bar(a.x,a.y,b.x,b.y);
    tgroup::draw();
}


////////////////////////////////////////////////////////////////////////////////////////
// VALUE TYPE

  
valuetype::valuetype(valuedata *data,UINT32 attrib_):tgroup()
{    
    valuetype::setrange(data);
	attrib=attrib_;
}    

UINT32 valuetype::setval(INT32 val_)
{
	valuedata ndata=data;
	ndata.val=val_;
    return setrange(&ndata);   
}

UINT32 valuetype::setvalmute(INT32 val_)
{
    state |= SFmute;
	valuedata ndata=data;
	ndata.val=val_;	
    UINT32 retval=setrange(&ndata);
    state &= ~SFmute;
    return retval;
}

UINT32 valuetype::setvalf(float val_)
{
	valuedata ndata=data;
	ndata.fval=val_;	
    return setrange(&ndata);    
}

UINT32 valuetype::setvalmutef(float val_)
{
    state |= SFmute;
	valuedata ndata=data;
	ndata.fval=val_;	
    UINT32 retval=setrange(&ndata);
    state &= ~SFmute;
    return retval;
}

UINT32 valuetype::setrangemute(valuedata *data_)
{
	state |= SFmute;
	UINT32 retval=setrange(data_);
	state &= ~SFmute;
	return retval;
}

UINT32 valuetype::setrange(valuedata *data_)
{
	UINT32 retval=0;
	if (attrib & VTfloat)
	{

		BOUND(data_->fval,data_->fmin,data_->fmax);
		if (data_->fval!=data.fval)
			retval=VTvalchanged;
		if (data_->fmin!=data.fmin)
			retval|=VTminchanged;
		if (data_->fmax!=data.fmax)
			retval|=VTmaxchanged;
		
	}else
	{
		BOUND(data_->val,data_->min,data_->max);
		if (data_->val!=data.val)
			retval=VTvalchanged;
		if (data_->min!=data.min)
			retval|=VTminchanged;
		if (data_->max!=data.max)
			retval|=VTmaxchanged;
		
	}
	if (retval!=0)
		data=*data_;
	return true;//retval;
}    



////////////////////////////////////////////////////////////////////////////////////////
// SLIDER TYPE

slidertype::slidertype(INT32 x,INT32 y,INT32 l,valuedata *data,UINT32 attrib_,UINT32 updatecommand_):valuetype(data,attrib_)
{   
   options|=OFnooverlap+OFkeygrab+OFautoraise;
   updatecommand=updatecommand_;
   
   if ((attrib_ && SAxorient)!=0)
   {
       growmode=TGgrowallx+TGdragy1;            
       
       rassign(x,y,x+l,y+TWINctrlbm.scroll.getbuttonheight()-1);
       
       insert(new tbutton(a.x,a.y,BMperiodaction,CMDdecval,leftkey,gadgets,&TWINctrlbm.scroll.left0));
       firstchild->growmode=TGdragx1|TGgrowally;       
       insert(new tbutton(b.x-TWINctrlbm.scroll.getbuttonwidth(),y,BMperiodaction,CMDincval,rightkey,gadgets,&TWINctrlbm.scroll.right0));
       firstchild->growmode=TGdragx2|TGgrowally;       
   }else
   {       
       growmode=TGgrowally+TGdragx1;
       
       rassign(x,y,x+TWINctrlbm.scroll.getbuttonwidth()-1,y+l);
       
       insert(new tbutton(a.x,a.y,BMperiodaction,CMDdecval,upkey,gadgets,&TWINctrlbm.scroll.up0));
       firstchild->growmode=TGdragy1|TGgrowallx;
       insert(new tbutton(a.x,b.y-TWINctrlbm.scroll.getbuttonheight(),BMperiodaction,CMDincval,downkey,gadgets,&TWINctrlbm.scroll.down0));
       firstchild->growmode=TGdragy2|TGgrowallx;       
   }
   firstchild->options&=~(OFselectable);
   lastchild->options&=~(OFselectable);
   calcslidepos();
}


void slidertype::handleevent(tevent *event)
{
    INT32 diff;
    INT32 range;
    INT32 incby;
    trect srect;
            
    tpoint p;
    attrib&SAxorient?p.assign(a.x+TWINctrlbm.scroll.getbuttonwidth(),b.x-TWINctrlbm.scroll.getbuttonwidth()):p.assign(a.y+TWINctrlbm.scroll.getbuttonheight(),b.y-TWINctrlbm.scroll.getbuttonheight());
        
    if ((state & SFmodal)!=0)
    {
        if (MOUSEreleased)
        {
            MOUSEsetbounds(0,0,SC.size.x-1,SC.size.y-1);
            attrib&=~SAmodalslide;
            TWINendmodal();
            return;
        }
            
        if ((attrib & SAmodalslide)!=0) //we're on a modal slide
        {
            if (event->what & (EVmousemove))
            {
                diff=(p.y-p.x)-TWINbarminsize;                    
                if (diff==0)
                   diff=1;
                range=data.max-data.min;
                if ((attrib & SAxorient)!=0)
                    incby=(mp.x-p.x)-offset;
                else
                    incby=(mp.y-p.x)-offset;
                if (range>=diff)
                    incby=(incby * range) /diff;
                incby+=data.min;
                setval(incby);                
                
                event->what=0;
            }
        }
        else  //mouse not in slider, jump up or down
        {
            getsrect(srect);
            if ((mp.x>srect.b.x)||(mp.y>srect.b.y))    
                 setval(data.val+2);
            else if ((mp.x<srect.a.x)||(mp.y<srect.a.y))    
                 setval(data.val-2);
            else
                 event->what=0;
        }              
    }else
    {    
    tgroup::handleevent(event);
    if (event->what==0) return;
    if ((event->what & (EVmousepressed))!=0)
    {                   
        getsrect(srect);
        if (mb>0)
        {
            if (srect.rcontains(mp.x,mp.y))
            {
                attrib |= SAmodalslide;
                if ((attrib & SAxorient)!=0)
                {
                    offset=mp.x-srect.a.x;         
                    MOUSEsetbounds(p.x+offset,mp.y,p.y-(s2-s1-offset)+1,mp.y);              
                }
                else
                {
                    offset=mp.y-srect.a.y;         
                    MOUSEsetbounds(mp.x,p.x+offset,mp.x,p.y-(s2-s1-offset)+1);
              
                }                              
            }
            TWINsetmodal(this);
        }        
        event->what=0;      
   }else if ((event->what & EVcommand) != 0)
   {
      incby=1;
      switch (event->command)
      {
          case CMDincval:setval(data.val+incby);break;
          case CMDdecval:setval(data.val-incby);break;
      }
   }else if ((event->what==EVkeydown) && (scancode!=0))
   {
      event->what=0;
      if ((attrib & SAxorient)!=0)      
      switch (scancode)
      {
          case rightkeyctrl: 
          case endkey:      scancode=pagedownkey;
          case leftkeyctrl: scancode=pageupkey;break;
          case rightkey:    scancode=downkey;break;
          case leftkey:     scancode=upkey;break;
      }      
      switch (scancode)
      {
          case pageupkey:   setval(data.min);break;
          case pagedownkey: setval(data.max);break;
          case upkey:       setval(data.val-1);break;
          case downkey:     setval(data.val+1);break;
          default:
          event->what=EVkeydown;
      }               
   }
   }
}

UINT32 slidertype::setrange(valuedata *data_)
{
    trect oldvp,tmp;


    UINT32 retval=valuetype::setrange(data_);
    if (retval)
    {
        if ((state & SFvisible)!=0)
        {            
            oldvp=SC.viewport;
            getsrect(SC.viewport);            
            calcslidepos();
            getsrect(tmp);
            SC.viewport.runion(tmp);
            SC.viewport.rgrow(3,3);
            SC.viewport.rintersect(oldvp);
            attrib |=SAdontdrawbuttons;
            drawview();
            attrib &=~SAdontdrawbuttons;
            SC.viewport=oldvp;
        }else
            calcslidepos();
        // out message to parent}
        sendcommand(updatecommand,owner,&data.val);
    }
	return retval;
}
/*
	UINT32 w2=(1+x2-x)-(c[0].w+c[2].w);
	putbitmap(x,y,(trect*)&c[0],image,c[0].attrib);
	putbitmap(x+c[0].w,y,w2,c[1].h,(trect*)&c[1],image,c[1].attrib);
	putbitmap(x2-c[2].w,y,(trect*)&c[2],image,c[2].attrib);
}

void captilebitmapy(INT32 x,INT32 y,INT32 y2,tblitrect *c,tsurface *image)
{
	UINT32 h2=(1+y2-y)-(c[0].h+c[2].h);
	putbitmap(x,y,(trect*)&c[0],image,c[0].attrib);
	putbitmap(x,y+c[0].h,c[1].w,h2,(trect*)&c[1],image,c[1].attrib);
	putbitmap(x,y2-c[2].h,(trect*)&c[2],image,c[2].attrib);
*/

void slidertype::draw()
{
    
    if ((attrib & SAdontdrawbuttons)==0)
        tgroup::draw();
    char c=0;
	INT32 pw;
    INT32 old2,old1,b1,b2;    
    tpoint p;
    if ((attrib & SAxorient)!=0)
    {
        old1=SC.viewport.a.x;
        old2=SC.viewport.b.x;
        p.assign(a.x+TWINctrlbm.scroll.getbuttonwidth(),b.x-TWINctrlbm.scroll.getbuttonwidth()-1);
		pw=p.y-p.x;
        b1=p.x+s1;
        b2=p.x+s2-1;
        if (old2>b1)
          SC.viewport.b.x=b1;      
		
		captilebitmapx(p.x,a.y,p.y,&TWINctrlbm.scroll.bgleft,gadgets);
		
        SC.viewport.b.x=(old2>p.y)?p.y:old2;		
		
		/*UINT32 w2=(1+x2-x)-(c[0].w+c[2].w);
		putbitmap(x,y,(trect*)&c[0],image,c[0].attrib);
		putbitmap(x+c[0].w,y,w2,c[1].h,(trect*)&c[1],image,c[1].attrib);
		putbitmap(x2-c[2].w,y,(trect*)&c[2],image,c[2].attrib);        
        captilebitmapx(b1,a.y,b2,&TWINctrlbm.scroll.barleft,gadgets);          */
		if (TWINctrlbm.scroll.barleft.attrib!=0)
		{
			scratchpad->viewport.b.assign(TWINctrlbm.scroll.barleft.w-1,TWINctrlbm.scroll.barleft.h-1);			
			TWINrendercontext->setoutput(scratchpad);						
			captilebitmapx(-s1,0,-s1+pw,&TWINctrlbm.scroll.bgleft,gadgets);
			putbitmap(0,0,(trect*)&TWINctrlbm.scroll.barleft,gadgets,TWINctrlbm.scroll.barleft.attrib);
			TWINrendercontext->setoutput(NULL);
			putbitmap(b1,a.y,&scratchpad->viewport,scratchpad);
		}else		
			putbitmap(b1,a.y,(trect*)&TWINctrlbm.scroll.barleft,gadgets,TWINctrlbm.scroll.barleft.attrib);

		if (TWINctrlbm.scroll.barright.attrib!=0)
		{
			scratchpad->viewport.b.assign(TWINctrlbm.scroll.barright.w-1,TWINctrlbm.scroll.barright.h-1);			
			TWINrendercontext->setoutput(scratchpad);
			captilebitmapx(-s2+TWINctrlbm.scroll.barright.w,0,-s2+TWINctrlbm.scroll.barright.w+pw+2,&TWINctrlbm.scroll.bgleft,gadgets);
			putbitmap(0,0,(trect*)&TWINctrlbm.scroll.barright,gadgets,TWINctrlbm.scroll.barright.attrib);
			TWINrendercontext->setoutput(NULL);
			putbitmap(b2-TWINctrlbm.scroll.barright.w,a.y,&scratchpad->viewport,scratchpad);
			
		}else
			putbitmap(b2-TWINctrlbm.scroll.barright.w,a.y,(trect*)&TWINctrlbm.scroll.barright,gadgets,TWINctrlbm.scroll.barright.attrib);
		
		putbitmap(b1+TWINctrlbm.scroll.barleft.w,a.y,(b2-b1)-(TWINctrlbm.scroll.barleft.w+TWINctrlbm.scroll.barright.w),TWINctrlbm.scroll.barleft.h,(trect*)&TWINctrlbm.scroll.barmid,gadgets,TWINctrlbm.scroll.barmid.attrib);		
		
		//putbitmap(b2,a.y,(trect*)&TWINctrlbm.scroll.barshad,gadgets,TWINctrlbm.scroll.barshad.attrib);		
        
        if (old1<b2)        
          SC.viewport.a.x=b2;//+TWINctrlbm.scroll.barshad.w;          
		captilebitmapx(p.x,a.y,p.y+1,&TWINctrlbm.scroll.bgleft,gadgets);
        
        SC.viewport.b.x=old2;
        SC.viewport.a.x=old1;
        
    } else
    {
        
        old1=SC.viewport.a.y;
        old2=SC.viewport.b.y;
        p.assign(a.y+TWINctrlbm.scroll.getbuttonheight(),b.y-TWINctrlbm.scroll.getbuttonheight()-1);
		pw=p.y-p.x;
        b1=p.x+s1;
        b2=p.x+s2-1;
        if (old2>b1)
          SC.viewport.b.y=b1;        
        captilebitmapy(a.x,p.x,p.y,&TWINctrlbmv.bgleft,vgadgets);
        SC.viewport.b.y=(old2>p.y)?p.y:old2;

		//UINT32 h2=(1+y2-y)-(c[0].h+c[2].h);

		if (TWINctrlbmv.barleft.attrib!=0)
		{
			scratchpad->viewport.b.assign(TWINctrlbmv.barleft.w-1,TWINctrlbmv.barleft.h-1);			
			TWINrendercontext->setoutput(scratchpad);						
			captilebitmapy(0,-s1,-s1+pw,&TWINctrlbmv.bgleft,vgadgets);
			putbitmap(0,0,(trect*)&TWINctrlbmv.barleft,vgadgets,TWINctrlbmv.barleft.attrib);
			TWINrendercontext->setoutput(NULL);
			putbitmap(a.x,b1,&scratchpad->viewport,scratchpad);
		}else	
			putbitmap(a.x,b1,(trect*)&TWINctrlbmv.barleft,vgadgets,TWINctrlbmv.barleft.attrib);
		
		if (TWINctrlbmv.barright.attrib!=0)
		{
			scratchpad->viewport.b.assign(TWINctrlbmv.barright.w-1,TWINctrlbmv.barright.h-1);			
			
			TWINrendercontext->setoutput(scratchpad);		
			captilebitmapy(0,-s2+TWINctrlbmv.barright.h,-s2+TWINctrlbmv.barright.h+pw+2,&TWINctrlbmv.bgleft,vgadgets);
			putbitmap(0,0,(trect*)&TWINctrlbmv.barright,vgadgets,TWINctrlbmv.barright.attrib);
			TWINrendercontext->setoutput(NULL);
			putbitmap(a.x,b2-TWINctrlbmv.barright.h,&scratchpad->viewport,scratchpad);
			
		}else
			putbitmap(a.x,b2-TWINctrlbmv.barright.h,(trect*)&TWINctrlbmv.barright,vgadgets,TWINctrlbmv.barright.attrib);			
		
				
		putbitmap(a.x,b1+TWINctrlbmv.barleft.h,TWINctrlbmv.barleft.w,(b2-b1)-(TWINctrlbmv.barleft.h+TWINctrlbmv.barright.h),(trect*)&TWINctrlbmv.barmid,vgadgets,TWINctrlbmv.barmid.attrib);

		//captilebitmapy(a.x,b1,b2,&TWINctrlbmv.barleft,vgadgets);          
		//putbitmap(a.x,b2,(trect*)&TWINctrlbmv.barshad,vgadgets,TWINctrlbmv.barshad.attrib);		
                                
        if (old1<b2)        
          SC.viewport.a.y=b2;          
		captilebitmapy(a.x,p.x,p.y+1,&TWINctrlbmv.bgleft,vgadgets);
        //captilebitmapy(a.x,p.x,p.y+1,sb_basey);                  
        SC.viewport.b.y=old2;
        SC.viewport.a.y=old1;
    }          
}


void slidertype::calcslidepos()
{
  INT32 diff,valdiff,size;
  UINT32 calc;

  //calculate the size of the bar in pixels...
  tpoint p;
  attrib&SAxorient?p.assign(a.x+TWINctrlbm.scroll.getbuttonwidth(),b.x-TWINctrlbm.scroll.getbuttonwidth()+1):p.assign(a.y+TWINctrlbm.scroll.getbuttonheight(),b.y-TWINctrlbm.scroll.getbuttonheight()+1);

  diff=(p.y-p.x);
  //calculate the range of the bar...      
  valdiff=data.max-data.min;
  //check if we're smaller than the minimum size...  
  if (valdiff<(diff-TWINbarminsize)) 
      size=diff-valdiff;
  else
      size=TWINbarminsize;  //default to minimum size....
  if (valdiff!=0)
  {
      calc=data.val-data.min;
      s1=(calc*(diff-size))  / valdiff;
  } else
  {
      s1=0;
      size=diff;
  }
  s2=s1+size;
}



void slidertype::changebounds(trect &bounds)
{
  tgroup::changebounds(bounds);
  calcslidepos();
}

void slidertype::getsrect(trect &srect)
{
  if ((attrib & SAxorient)!=0) 
      srect.rassign(a.x+TWINctrlbm.scroll.getbuttonwidth()+s1,a.y,a.x+TWINctrlbm.scroll.getbuttonwidth()+s2,b.y);
  else
      srect.rassign(a.x,a.y+TWINctrlbm.scroll.getbuttonheight()+s1,b.x,a.y+TWINctrlbm.scroll.getbuttonheight()+s2);
}


////////////////////////////////////////////////////////////////////////////////////////
// NUMBOX TYPE


numbox::numbox(INT32 x,INT32 y,INT32 w,fiunion stepsize_,UINT32 updatecommand_,valuedata *data_,UINT32 attrib_):valuetype(data_,attrib_)
{ 
   UINT32 hgt=TWINctrlbm.numbox.down0.h+2+TWINctrlbm.numbox.up0.h-1;
   stepsize=stepsize_;
   updatecommand=updatecommand_;
   rassign(x,y,x+w,y+hgt);
   options|=OFnooverlap+OFkeygrab;
   //create a textbox 
   insert(edit=new edittextbox(x,y,(attrib & VTfloat)?TBfloat:TBnumeric,NULL,255,w-TWINctrlbm.numbox.up0.w,hgt));
   if (attrib & VTfloat)
	  edit->setvalfloat(data.fval);
   else
	  edit->setval(data.val);
   
}

void numbox::dec()
{
	valuedata ndata=data;
	if (attrib & VTfloat)
		ndata.fval-=stepsize.fval;
	else
		ndata.val-=stepsize.ival;
	setrange(&ndata);
}

void numbox::inc()
{
	valuedata ndata=data;
	if (attrib & VTfloat)
		ndata.fval+=stepsize.fval;
	else
		ndata.val+=stepsize.ival;
	setrange(&ndata);

}

void valuetype::setmin()
{
	data.val=data.min;
	
}

void valuetype::setmax()
{
	data.val=data.max;	
}




numbox::~numbox()
{
}

void numbox::handleevent(tevent *event)
{
    static UINT32 mcount;
    if ((state & SFmodal)!=0)
    {
        if (MOUSEreleased)
        {
            attrib&=~(NSsliding|NSdown|NSup);
            drawview();
            TWINendmodal();
            evcallback(CMDnumboxreleased);
            return;
        }
        if ((attrib & NSsliding)==0)
        {
           if (abs(mdiff.y)>3)
           {
               attrib|=NSsliding;
               drawview();
           }
        }
		if (attrib & NSsliding)
		{
			if (attrib & VTfloat)
				setvalf(data.fval-((float)mdiff.y*stepsize.fval));
			else
				setval(data.val-(mdiff.y*stepsize.ival));
        
		}else
        if (attrib & NSdown) //we're on a modal slide
        {
            if ((TIMEgettime()-mcount)>threshold)
            {
              mcount=TIMEgettime();
			  dec();
              
            }
        }  
        else if (attrib & NSup)
        {
            if ((TIMEgettime()-mcount)>threshold)
            {
              mcount=TIMEgettime();
			  inc();              
            }
        }                                                                        
    }else
    {    
    tgroup::handleevent(event);
    if (event->what==0) return;    
    if ((event->what & (EVmousepressed))!=0)
    {                   
        if (mb>0)
        {
            mcount=TIMEgettime()+threshold;
            if (mp.x>b.x-TWINctrlbm.numbox.getwidth())
            {
                if (mp.y<(a.y+TWINctrlbm.numbox.getbuttonheight()-1))
                {
                  inc();                 
                  attrib|=NSup;
                }else if (mp.y>(1+b.y-TWINctrlbm.numbox.getbuttonheight()))
                {                  
                  dec();                  
                  attrib|=NSdown;                  
                }else                             
                  attrib|=NSsliding;
                drawview();
            }
            evcallback(CMDnumboxpressed);
            TWINsetmodal(this);
        }        
        event->what=0;      
   }else if (event->what == EVkeydown)
   {
       event->what=0;

       switch (scancode)
       {
           case upkey:inc();break;
           case downkey:dec();break;
           case pageupkey:setmax();break;
           case pagedownkey:setmin();break;        
       }                 
   }else if ((event->what & EVcommand) != 0)
   {
       switch (event->command)
       {
         case CMDeditreturnpressed:
		 if (attrib & VTfloat)					 
			 setvalf(edit->getvalfloat());         
		 else		 			 
			 setval(edit->getvalint());         		          
         break;
		 case CMDeditstringaltered:
			 attrib |= NSgetvalfromtext;
			 break;
       }
   }
   }
   
}

void numbox::draw()
{
    edit->drawview();
    UINT32 ypos=a.y;
	tblitrect *br=(attrib & NSup)?&TWINctrlbm.numbox.up1:&TWINctrlbm.numbox.up0;
	INT32 bw=TWINctrlbm.numbox.getwidth()-1;
    putbitmap(b.x-bw,ypos,(trect*)br,gadgets,br->attrib);
	ypos+=TWINctrlbm.numbox.getbuttonheight();    
	
    //putbitmap(b.x-(ns_btnw-1),ypos,ns_gfx[4+((attrib & NSsliding)!=0)]);
	ypos+=2;//ns_spacerh;        
	br=(attrib & NSdown)?&TWINctrlbm.numbox.down1:&TWINctrlbm.numbox.down0;
    putbitmap(b.x-bw,ypos,(trect*)br,gadgets,br->attrib);

    //draw the buttons
    //
}


UINT32 numbox::setrange(valuedata *data_)
{
         
	if (attrib & NSgetvalfromtext)
	{

		if (attrib & VTfloat)					 
		{
			data_->fval=edit->getvalfloat()+(data_->fval-data.fval);         
		}else		 			 
		{
			data_->val=edit->getvalint()+(data_->val-data.val);         		          
		}
		attrib &=~NSgetvalfromtext;        
	}
    UINT32 retval=valuetype::setrange(data_);    
    if (retval)
    {
        sendcommand(updatecommand,owner,(void*)&data.val);
		if (attrib & VTfloat)
			edit->setvalfloat(data.fval);
		else
			edit->setval(data.val);
    }
	return retval;
}
    

////////////////////////////////////////////////////////////////////////////////////////
// EDITTEXT TYPE

edittextbox::edittextbox(INT32 x,INT32 y,UINT8 attr,char *txt_,INT32 slen_,INT32 w,INT32 h):
                           tview(x,y,x+w,y+h)
{
  tbattrib=attr;
  strmem=slen_;
  xlen=w;
  if (tbattrib & TBexternalstring)
  {
	  txt=txt_;
      slen=MIN((INT32)strlen(txt_),strmem);
  }else
  {
	txt=new char[strmem+1];
	memset(txt,0,strmem+1);
	if (txt_)
	{
		  slen=MIN((INT32)strlen(txt_),strmem);
		  strcpy(txt,txt_);
	} else
		  slen=0;
  }
    
  insertmode=1;  
  vistext=txt;
  curspos=curslen=c1=c2=cp1=cp2=off=0;
  lastchar=FNTgetindexfrompos(vistext,xlen-ETeditindex_x2);    //Get the closest character index to 'xlen'  
  options|=OFtabcycleselect;
  //OFautoraise+ is annoying
}

edittextbox::edittextbox(INT32 x,INT32 y,UINT8 attr,char *txt_,INT32 slen_,INT32 w)
{
    edittextbox(x,y,attr,txt_,slen_,w,t_charset.ysize+ETeditindent_y);    
}

edittextbox::~edittextbox()
{
	if (txt && ((tbattrib & TBexternalstring)==0))
		delete [] txt;
}

void edittextbox::settext(char *txt_,bool selectall)
{
  if (strcmp(txt_,txt))
  {    
      slen=MIN((INT32)strlen(txt_),strmem);
      if (selectall)
      {
        //curspos=slen;
		curspos=0;
        curslen=slen;
      }else
      {
        //if (curspos>slen)  curspos=slen;
		curspos=0;
        curslen=curspos;
      }
      memcpy(txt,txt_,slen);
      txt[slen]=0;
      calcinternalvars();
      drawview();
  }  
}

void edittextbox::selectall()
{
    curspos=slen;
    curslen=0;
    calcinternalvars();
    drawview();
}

void edittextbox::calcinternalvars()
{
    vistext=&txt[off];
    lastchar=MIN(slen,FNTgetindexfrompos(vistext,xlen-ETeditindex_x2));    //Get the closest character index to 'xlen'    

    c1=MIN(curspos,curslen)-off;
    c2=MAX(curspos,curslen)-off;
    BOUND(c1,0,lastchar);  //'clip' c1 and c2 to the visible region of characters
    BOUND(c2,0,lastchar);     
    lastchar--;           
    cp1=FNTgetwidthlength(vistext,c1);  //Get the pixel x position of 'curslen'
    cp2=FNTgetwidthlength(vistext,c2);  //Get the pixel x position of 'curslen'
    //txt[slen+1]=0;
}    

char edittextbox::setcursorpos(INT32 pos,char followtail)
{
    INT32 oldoff,newcurslen;  
    //trect oldvp;
    BOUND(pos,0,slen);
    if (!followtail)
       newcurslen=pos;
    else
       newcurslen=curslen;
    BOUND(newcurslen,0,slen);   
  
    if ((pos!=curspos)||(newcurslen!=curslen))
    {
        curspos=pos;
        curslen=newcurslen;         
        oldoff=off;                                          //store old offset
        //do a limited calculation of internal variables so that we can check to see if the cursor outside of the view
        vistext=&txt[off];
        cp1=FNTgetwidthlength(vistext,curspos-off);  //Get the pixel x position of 'curpos'    
        lastchar=FNTgetindexfrompos(vistext,xlen-ETeditindex_x2);    //Get the closest character index to 'xlen'           
        if (curspos<off)                                         //Is the cursor to the left of the textboxb? 
            off=curspos;                                          // recaluate the character offset so that is just inside to the left
        else if ((cp1>=(xlen-ETeditindex_x2)))                                       //Is the cursor to the right of the textboxb?      
            off=curspos-lastchar;                                      //If so  recaluate the char offset so that it is just inside to right
            
        calcinternalvars();       
        if (oldoff!=off)                                    //If the offset has changed  we need to redraw everthing        
            drawview();
        else                  
            drawview();                                               
        return 1;
    }
        
    return 0;
}
/*

void edittextype::update;
{
static lasttime:word=0;
char onoff=0;

char oldmode;

  if (*timmer-lasttime>100)
  {
    onoff=onoff xor true;
    oldmode=drawmode;
    drawmode=

  }
}
*/

void edittextbox::draw()
{
  textboxb(a.x,a.y,b.x,b.y);      
  t_fillcol=TWINpapercol;
  t_col=TWINtextcol;        
  
  INT32 dx=a.x+ETeditindent_x,dy=a.y+ETeditindent_y;    
  if (c1!=c2)
  {                
      if ((state & SFselected)!=0)
      {
          bar(a.x+TWINtextindent.a.x,a.y+TWINtextindent.a.y,dx+cp1,b.y-TWINtextindent.b.y);
          bar(dx+cp2,a.y+TWINtextindent.a.y,b.x-TWINtextindent.b.x,b.y-TWINtextindent.b.y);
          t_fillcol=TWINpaperselcol;
          bar(dx+cp1,a.y+TWINtextindent.a.y,dx+cp2,b.y-TWINtextindent.b.y);
          
      }else
          bar(a.x+TWINtextindent.a.x,a.y+TWINtextindent.a.x,b.x-TWINtextindent.b.x,b.y-TWINtextindent.b.y);

      INT32 templen;
      if (c1>0)
          outtextxy(dx,dy,vistext,c1);                        //draw the first portion of non-selected text
      templen=MIN(slen,lastchar)-c2;
      if(templen>=0)      
          outtextxy(dx+cp2,dy,&vistext[c2],templen);          //the trailing portion of non selected text
      
      if ((templen=MIN(c2-1,lastchar)-c1)>=0)
      {
          t_col=TWINtextselcol;
          outtextxy(dx+cp1,dy,&vistext[c1],templen);          //draw the selected text.
      }    
  } else
  {
      bar(a.x+TWINtextindent.a.x,a.y+TWINtextindent.a.y,b.x-TWINtextindent.b.x,b.y-TWINtextindent.b.y);  
      outtextxy(dx,dy,vistext,lastchar);
  }              
  
  
  if ((state & SFselected)!=0)
  {
      t_col=TWINtextcol;
      int cpos;
      if (curspos>curslen)
        cpos=cp2;
      else
        cpos=cp1;  
      textcurs(dx+cpos,dy,height()-5);
  }
 
  /*diagnositcs
  SC.viewport=SC.screenport;

  char tmp[10];
  bar(10,10,50,80);

  outtextxy(10,10,itoa(cp1,tmp,10));
  outtextxy(10,20,itoa(cp2,tmp,10));
  outtextxy(10,30,itoa(c1,tmp,10));
  outtextxy(10,40,itoa(c2,tmp,10));
  outtextxy(10,50,itoa(off,tmp,10));
  outtextxy(10,60,itoa(lastchar,tmp,10));
  */
  
}

void edittextbox::handleevent(tevent *event)
{
    char followtail=0;
    char *vistext;
    tview::handleevent(event);

    if ((state & SFmodal)!=0)
    {
        vistext=txt;//&txt[off];  
        if (MOUSEreleased)
        {
            TWINendmodal();
            return;
        }    
        if (mp.x<a.x+ETeditindent_x)
            setcursorpos(curspos-1,1);
        else if(mp.x>b.x-ETeditindent_x)
            setcursorpos(curspos+1,1);
        else          
            setcursorpos(off+FNTgetindexfrompos(vistext,mp.x-ETeditindent_x-a.x),1);
    }else    
    if ((event->what & EVmousepressed)!=0)
    {
        vistext=txt;//&txt[off];  
        setcursorpos(off+FNTgetindexfrompos(vistext,mp.x-ETeditindent_x-a.x),0);
        TWINsetmodal(this);
   
        event->what=0;
    }
  
  
    if (event->what==EVkeydown)
    {
      event->what=0;
      switch (charcode)
      {        
        case backspacekey:        
          if ((curspos==curslen)&&(curspos>0))
             deletetext(curspos-1,1);
          else          
             deletetext(MIN(curspos,curslen),abs(curslen-curspos));                       
        break;
        case escapekey:break;;
        case returnkey:
            evcallback(CMDeditreturnpressed);           
            break;
        default:
            //do checking in here for numeric or other specialized text cases.
            //if (curspos!=curslen)
            //   deletetext(MIN(curspos,curslen),abs(curslen-curspos));                       
            if (tbattrib & TBnumeric)
            {
                if ((((charcode>='0' && charcode<='9')|| charcode=='-' || charcode=='+'))||
				   ((tbattrib & TBfloat)&& charcode=='.'))
                  inserttext(curspos,&charcode,1);
		
            }else if (charcode>30)        
                inserttext(curspos,&charcode,1);                          
        break;

      }
      //followtail=(*scan & SMleftshiftkey)!=0;
      if (charcode==0)
      {
        event->what=0;
        switch (scancode)
        {
          case homekey:
          case leftkeyctrl:
                setcursorpos(0,followtail);break;
          case endkey:
          case rightkeyctrl:
                setcursorpos(slen,followtail);break;
          case rightkey:
                setcursorpos(curspos+1,followtail);break;
          case leftkey:
                setcursorpos(curspos-1,followtail);break;
          case delkey:
             if (curspos==curslen)
             {
                UINT8 oldslen=slen;
                deletetext(curspos,1);
                if (oldslen!=0)
                  drawview();
             }
             else 
                deletetext(MIN(curspos,curslen),abs(curslen-curspos));                                              
          default: event->what=EVkeydown;break;//;
        }
      }
    }
}

/*
extern void *memchr( const void *__s, int __c, size_t __n );
extern int   memcmp( const void *__s1, const void *__s2, size_t __n );
extern void *memcpy( void *__s1, const void *__s2, size_t __n );
extern void *memmove( void *__s1, const void *__s2, size_t __n );
extern void *memset( void *__s, int __c, size_t __n );
extern char *strcat( char *__s1, const char *__s2 );
extern char *strchr( const char *__s, int __c );
extern int strcmp( const char *__s1, const char *__s2 );
extern int strcoll( const char *__s1, const char *__s2 );
extern size_t strxfrm( char *__s1, const char *__s2, size_t __n );
extern char *strcpy( char *__s1, const char *__s2 );
extern size_t strcspn( const char *__s1, const char *__s2 );
extern char *strerror( int __errnum );
extern size_t strlen( const char *__s );
extern char *strncat( char *__s1, const char *__s2, size_t __n );
extern int strncmp( const char *__s1, const char *__s2, size_t __n );
extern char *strncpy( char *__s1, const char *__s2, size_t __n );
extern char *strpbrk( const char *__s1, const char *__s2 );
extern char *strrchr( const char *__s, int __c );
extern size_t strspn( const char *__s1, const char *__s2 );
extern char *strstr( const char *__s1, const char *__s2 );
extern char *strtok( char *__s1, const char *__s2 );

*/
bool edittextbox::setstate(UINT32 astate,bool enable)
{
  if (tview::setstate(astate,enable))
  {
      if ((astate & SFselected) != 0 )
         drawview();
      return 1;
  }else
      return 0;    
}
    
void edittextbox::inserttext(INT32 pos,char *txt_,INT32 length)
{    
	if (curspos!=curslen)
	{
	    deletetext(MIN(curspos,curslen),abs(curslen-curspos));
		pos=curspos;
	}
    length=MIN(length,pos+strmem);
    pos=MIN(pos,slen);
    if (insertmode==0)
    {
        memcpy(&txt[pos],txt_,length);        
    }else
    {
        INT32 length_to_end=MAX(0,slen-pos);
        char *charbuf=new char[length_to_end];
        
        memcpy(charbuf,&txt[pos],length_to_end);                
        memcpy(&txt[pos+length],charbuf,length_to_end);
        delete [] charbuf;
        memcpy(&txt[pos],txt_,length);      
    }
    //if (curspos==slen)
    //{    
    slen=MIN(slen+length,strmem);
    setcursorpos(pos+length,0);
    evcallback(CMDeditstringaltered);
    //} 
        
    /*memcpy(txt_,&newstr[pos],length);
    memcpy(&txt[pos],&newstr[pos+length],slen-pos);*/
    
    //slen+=length;       
}

bool edittextbox::setval(UINT32 val)
{
    //if ((tbattrib & TBnumeric)!=0) return FALSE;
    UINT32 oldval=getvalint();
    
    memset(txt,0,strmem);
    itoa(val,txt,10);
    vistext=txt; 
    slen=strlen(txt);
	curspos=0;
	curslen=slen;
    calcinternalvars();    
    lastchar=slen-1;    
    if (oldval!=val)
      drawview();
    return TRUE;
}


bool edittextbox::setvalfloat(float val)
{
    //if ((tbattrib & TBnumeric)!=0) return FALSE;
    float oldval=getvalfloat();
    
    memset(txt,0,strmem);
    sprintf(txt,"%.2f",val);
    vistext=txt; 
    slen=strlen(txt);
	curspos=0;
	curslen=slen;
    calcinternalvars();    
    lastchar=slen-1;    
    if (oldval!=val)
      drawview();
    return TRUE;
}


UINT32 edittextbox::getvalint()
{
    return atoi(txt);
}

float edittextbox::getvalfloat()
{
    return (float)atof(txt);
}

    
void edittextbox::deletetext(INT32 pos,INT32 length)
{
    //when you've done some sort of clipboard implement it in here
    if (pos+length>slen) return;
    length=MIN(length,pos+strmem);
    INT32 length_to_end=MAX(0,slen-pos);
    memcpy(&txt[pos],&txt[pos+length],length_to_end);
    slen-=length;
    if (slen<0) slen=0;
    txt[slen]=0;       
    setcursorpos(pos,0);
        //drawview();
    evcallback(CMDeditstringaltered);
}

////////////////////////////////////////////////////////////////////////////////////////
// TOOL BAR
// Easy to use toolbar; uses a single image for the buttons
// 


ttoolbutton::ttoolbutton(UINT32 at,UINT32 img,UINT32 cmnd,char *txt)
{
    attribute=at;
    image=img;
    command=cmnd;
	text=txt;
}

ttoolbar::ttoolbar(INT32 x,INT32 y,INT32 w,INT32 h,tsurface *imgs,UINT32 numimgs,UINT32 btnx,UINT32 btny):
tview(x,y,x+w,y+h)
{
    numimages=numimgs;
    images=imgs;
    
    bsize.x=MAX(1,btnx);bsize.y=MAX(1,btny);
    lasttoggledown=modalbutton=NULL;
    buttons.init(sizeof(ttoolbutton));
    calculatedimensions();
}

void ttoolbar::addbutton(UINT32 attribute,UINT32 image,UINT32 command,char *text)
{
    ttoolbutton *btn=new ttoolbutton(attribute,image,command,text);
    if ((attribute & (TBAglobaltoggle+TBAdown))==(TBAglobaltoggle+TBAdown))
    {
        if (lasttoggledown!=NULL)
           lasttoggledown->attribute &=~TBAdown;        
        lasttoggledown=btn;
    }
    
    buttons.push(btn);    
    calculatedimensions();
}

void ttoolbar::changebounds(trect &bounds)
{
	tview::changebounds(bounds);
	calculatedimensions();

}

bool ttoolbar::switchbuttonbyref(UINT32 buttonref)
{
    return switchbutton((ttoolbutton*)buttons.findelement(buttonref));
}

bool ttoolbar::switchbutton(ttoolbutton *button)
{    
    if (button==NULL) return FALSE;    
    if (button->attribute & TBAglobaltoggle)
    {
        if (lasttoggledown!=NULL)
           lasttoggledown->attribute &=~TBAdown;
        button->attribute |=TBAdown;
        lasttoggledown=button;
        evcallback(button->command);
    }else if (button->attribute & TBAtoggle)
    {
       button->attribute^=TBAdown;
       evcallback(button->command);
    }else
    {
       button->attribute^=TBAdown;
       modalbutton=button;       
    } 

    return TRUE;
}

ttoolbutton *ttoolbar::getbuttonat(UINT32 x,UINT32 y)
{
    x-=a.x;y-=a.y;
    INT32 count=((x/bsize.x)+((y/bsize.y)*cols));
    telem *btn=buttons.first;
    while ((--count>=0)&&(btn!=NULL))
      btn=btn->next;
    return (ttoolbutton *) btn;               
}
    
void ttoolbar::calculatedimensions()
{
    
    cols=MIN((INT32)buttons.count,(width()/bsize.x));
    if (cols==0)
       rows=0;
    else
       rows=(buttons.count/cols);
    //cols=3;rows=3;
}

//
void ttoolbar::draw()
{
    INT32 currx,curry=a.y,ix,iy;
    trect oldvp=SC.viewport;
    ttoolbutton *btn=(ttoolbutton *)buttons.first;
    trect irect;
	
    for (iy=0;iy<rows;iy++)
    {
        currx=a.x;
        for (ix=0;ix<cols;ix++)
        {
            if (btn)
            {
            UINT8 st=btn->attribute;
            //if ((btn->attribute & TBAseperator)==0)
            {
			   if (st & TBAaltselect)
			   {
				   irect.a.assign(btn->image*bsize.x,0);
				   irect.b.assign(irect.a.x+bsize.x-1,bsize.y-1);
				   putbitmap(currx,curry,&irect,images,0);
				   if (st & TBAdown)
				   {
					   t_col=TWINpaperselcol;					   
					   rectangle(currx,curry,currx+bsize.x-1,curry+bsize.y-1);
				   }
			   }else
			   {
				   irect.a.assign((btn->image+((st & TBAdown)*numimages))*bsize.x,0);
				   irect.b.assign(irect.a.x+bsize.x-1,bsize.y-1);
				   putbitmap(currx,curry,&irect,images,0);				   
			   }                              
            }
            
            btn=(ttoolbutton*)btn->next;
            }
            currx+=bsize.x;            
        }
        curry+=bsize.y;
        
    }
    
}

void ttoolbar::handleevent(tevent *event)
{
  tview::handleevent(event);
  if (state & SFmodal)
  {
	  if (modalbutton)
	  {
			//inviewpc=inview(mp.x,mp.y);      
			if ((MOUSEmoved)&&(modalbutton->attribute & TBAmoveaction))			
				evcallback(modalbutton->command);		 
			         
			if (MOUSEreleased)
			{
				if (modalbutton->attribute & TBAmoveaction!=0)
				{
					evcallback(modalbutton->command);					
				}
				modalbutton->attribute&=~TBAdown;
				modalbutton=NULL;
				drawview();     
			}
         
	  }
	  if (MOUSEreleased)
	  {
		  TWINendmodal();
		  evcallback(CMDtoolbarreleased);
	  }

  
  } else   

  if (event->what & EVmousepressed)
  {
        if (switchbutton(getbuttonat(mp.x,mp.y)))
        {
	        drawview();
		    TWINsetmodal(this);
        }
        event->what=0;
  }
}

bool ttoolbar::setstate(UINT32 astate,bool enable)
{
  //return (tview::setstate(astate,enable));
  if (tview::setstate(astate,enable))
  {      
      /*if ((astate & SFfocused) != 0 )
      {
         drawmode=tabdrawborder;
         drawview();
         drawmode=tabdrawall;
      }*/
      return 1;
  }else
      return 0;
}


////////////////////////////////////////////////////////////////////////////////////////
//DROP PALETTE
//USE: This widget looks like a button but when clicked it shows a ttoolbar class
//	   Similar in functionality to the dropbox



droppalette::droppalette(INT32 x,INT32 y,tsurface *imgs,UINT32 numimgs,UINT32 btnx,UINT32 btny):tview(x,y,x+btnx-1,y+btny-1)
{
	list=new ttoolbar(0,0,1,1,imgs,numimgs,btnx,btny);
	list->target=this;
	drawmode=DBtopbit;
}

void droppalette::draw()
{
	ttoolbutton *btn=getsel();
	trect irect;
	if (btn)
	{
	
	irect.a.assign(btn->image*list->bsize.x,0);
	irect.b.assign(irect.a.x+list->bsize.x,list->bsize.y);
    t_col=getcolour(0x0);
	hline(b.x-4,b.x,b.y);
	vline(b.x,b.y-4,b.y);
	line(b.x-4,b.y,b.x,b.y-4);	
	t_col=getcolour(0xFFFFFFFF);	 
	putbitmap(a.x,a.y,&irect,list->images,0);
	}
}


void droppalette::toggledrop()
{    
    drawmode &=DBlist;
    drawmode^=DBlist;
    trect newbounds;
    if (drawmode & DBlist)
    {
        TWINgetpopuptarget()->insert(list);        
        list->target=this;
        list->tmoveto(a.x,b.y);
        list->draw();
        list->state |=SFwaitingforrelease;
        list->select();
        //TWINsetmodal(list);        
    }
    else
    {   
        list->remove();
        evcallback(CMDdroppaletteclosed);
    }
    drawview();
    drawmode|=DBtopbit;    
}

droppalette::~droppalette()
{    
    if ((drawmode & DBlist)==0)    
      list->remove();
    delete list;
}

void droppalette::changebounds(trect &bounds)
{
    tview::changebounds(bounds);
    if ((drawmode & DBlist)!=0)
        list->tmoveto(a.x,b.y);               
}

void droppalette::handleevent(tevent *event)
{
    tview::handleevent(event);
	UINT8 olddrawmode;
    if (event->what&EVmousedown)
    {
        droppalette::toggledrop();
		event->what=0;
    }else
    if (event->what & EVcommand)
    {
        switch (event->command)
        {        
        case CMDtoolbarreleased :        
            toggledrop();
            break;
        case CMDselectedchanged:            
            olddrawmode=drawmode;
            drawmode=DBtext;
            drawview();
            drawmode=olddrawmode;
            evcallback(CMDselectedchanged);
            break;    
		default:
			sendcommand(target,event);
        }
    }if (event->what & EVkeyboard)
    {
        list->handleevent(event);
    }
}




////////////////////////////////////////////////////////////////////////////////////////
// DROP BOX
// USE: this widget provides the functionality of a listbox but takes up much less space
//      with the use of shadowing (soon) it will also look pretty cool

dropbox::dropbox(INT32 x1,INT32 x2,INT32 y1,listelem *info_,INT32 maxopts_):tview(x1,y1,x2,y1+TWINctrlbm.dropbox.down0.h-1)
{	
    growmode=TGdragx1|TGdragy1;
    drawmode=DBtopbit;
    options |= OFnooverlap;    
    list=new listbox(0,0,1,1,info_,maxopts_,FALSE);
    UINT32 numvals=maxopts_;
    BOUND(numvals,4,20);
    
    container=new scrollcontainer(x1,y1+TWINctrlbm.dropbox.getheight(),x2,y1+TWINctrlbm.dropbox.getheight()+4+(t_charset.ysize*numvals),list,CFybar+CFshadow);    
    container->growmode=TGdragonx2|TGdragony1;
    list->target=this;  
}



void dropbox::draw()
{
     textboxfilledb(a.x,a.y,b.x-TWINctrlbm.dropbox.getbuttonwidth(),a.y+TWINctrlbm.dropbox.getheight()-1);                      
     t_col=TWINtextcol;
     outtextxy(a.x+4,a.y+3,list->info[list->sel].txt);
	 tblitrect *bm=(drawmode & DBlist)?&TWINctrlbm.dropbox.down0:&TWINctrlbm.dropbox.down1;
     putbitmap(b.x-TWINctrlbm.dropbox.getbuttonwidth()+1,a.y,(trect*)bm,gadgets,bm->attrib);	 
}

dropbox::~dropbox()
{    
    if ((drawmode & DBlist)==0)    
      container->remove();
    delete container;
}

void dropbox::changebounds(trect &bounds)
{
    tview::changebounds(bounds);
    if ((drawmode & DBlist)!=0)
        container->tmoveto(a.x,b.y+1);               
}

void dropbox::handleevent(tevent *event)
{
    tview::handleevent(event);
    if (event->what&EVmousedown)
    {
        dropbox::toggledrop();
		event->what=0;
    }else
    if (event->what & EVcommand)
    {
        switch (event->command)
        {
        case CMDdragselectstopped:
            TWINendmodal();
        case CMDcontainermodalended:        
            toggledrop();
            break;
        case CMDselectedchanged:            
            UINT8 olddrawmode=drawmode;
            drawmode=DBtext;
            drawview();
            drawmode=olddrawmode;
            evcallback(CMDselectedchanged);
            break;                   
        }
    }if (event->what & EVkeyboard)
    {
        container->handleevent(event);
    }
}

void dropbox::toggledrop()
{    
    drawmode &=DBlist;
    drawmode^=DBlist;
    trect newbounds;
    if (drawmode & DBlist)
    {
        TWINgetpopuptarget()->insert(container);        
        container->target=this;
        container->tmoveto(a.x,b.y+1);
        container->draw();
        container->state |=SFwaitingforrelease;
        list->select();
        TWINsetmodal(container);        
    }
    else
    {   
        container->remove();
        evcallback(CMDdropboxclosed);
    }
    drawview();
    drawmode|=DBtopbit;    
}



////////////////////////////////////////////////////////////////////////////////////////
// TAB BOX
// USE: Creates a tabbed selection view. Useful for conserving space and logically seperating
// values



tabbox::tabbox(INT32 x1,INT32 y1,INT32 x2,INT32 y2):tgroup(x1,y1,x2,y2)
{
    drawmode=tabdrawall;
    rootelem=NULL;
    seltab=NULL;
}

tabbox::~tabbox()
{
    SLdeletelist((telem*)rootelem);    
}

void tabbox::addtabcontrol(tview *view,char *name,UINT32 id)
{
    seltab= new tabelement;
    strcpy(seltab->name,name);
    seltab->view=view;    
    SLaddtoend((telem**)&rootelem,(telem*)seltab);
    trect viewrect=trect(a.x+TWINctrlbm.window0_v.l.w,a.y+TWINctrlbm.tab.getheight(),b.x-TWINctrlbm.window0_v.r.w,b.y-TWINctrlbm.window0_h.b.h);
    /*if ((createflags & CFclientedge)!=0)
    {
       viewrect.rgrow(-2,-2);
       options|=OFclientedge;
    }
    if (view)
    {*/
        insert(view,id);
        view->makeinvisible();
        view->growmode=TGgrowall;
        view->changebounds(viewrect);
        view->makevisible();
    //}
    
}


    
void tabbox::drawtab(tabelement *ctab,INT32 x1,INT32 x2)
{
    if (!ctab)return;
    UINT32 sel=(seltab==ctab);    
    tblitrect *tabgfx=(sel)?&TWINctrlbm.tab.lt1:&TWINctrlbm.tab.lt0;		    
	captilebitmapx(x1,a.y,x2,(tblitrect*)tabgfx,gadgets);		
    t_col=sel?TWINtextselcol:TWINtextcol;    
    outtextxy(x1+13,a.y+3,ctab->name);
}

void tabbox::draw()
{    
    tabelement *ctab=rootelem;        
    INT32 x1,x2,selx1,selx2;
    INT32 tx1,tx2,ty1,ty2;
	b+=1;
    putbitmap(a.x,a.y,(trect*)&TWINctrlbm.tab.lb,gadgets,TWINctrlbm.tab.lb.attrib);
    x1=a.x+TWINctrlbm.tab.lb.w;    
    while (ctab!=NULL)
    {        
        x2=x1+FNTgetwidth(ctab->name)+24;
        if ((drawmode!=tabdrawselected)||(ctab==seltab))
        drawtab(ctab,x1,x2);                        
        if (ctab==seltab)
        {
            selx1=x1;
            selx2=x2-2;
        }
        ctab=(tabelement*)ctab->next;
        x1=x2;
    }          
    //shadowy bit    
    ty1=a.y+22;ty2=b.y-22;
    tx1=a.x+22;tx2=b.x-22;
    if (x2<b.x)    
        captilebitmapx(x2,a.y,b.x,&TWINctrlbm.tab.rc,gadgets);           
    
    
	INT32 w2=1+width()-(TWINctrlbm.window0.w.br.w+TWINctrlbm.window0.w.bl.w);
	putbitmap(a.x,b.y-TWINctrlbm.window0.w.bl.h,(trect*)&TWINctrlbm.window0.w.bl,gadgets,TWINctrlbm.window0.w.bl.attrib);
	putbitmap(b.x-TWINctrlbm.window0.w.br.w,b.y-TWINctrlbm.window0.w.br.h,(trect*)&TWINctrlbm.window0.w.br,gadgets,TWINctrlbm.window0.w.br.attrib);		
	putbitmap(a.x+TWINctrlbm.window0.w.bl.w,b.y-TWINctrlbm.window0_h.b.h,w2,TWINctrlbm.window0_h.b.h,(trect*)&TWINctrlbm.window0_h.b,gadgets,TWINctrlbm.window0_h.b.attrib);
	INT32 h=height()-TWINctrlbm.tab.getheight();	
	putbitmap(a.x,a.y+TWINctrlbm.tab.getheight(),TWINctrlbm.window0_v.l.w,h-TWINctrlbm.window0.ce.bl.h,(trect*)&TWINctrlbm.window0_v.l,gadgets,TWINctrlbm.window0_v.l.attrib);
	putbitmap(b.x-TWINctrlbm.window0_v.r.w,a.y+TWINctrlbm.tab.getheight(),TWINctrlbm.window0_v.l.w,h-TWINctrlbm.window0.ce.br.h,(trect*)&TWINctrlbm.window0_v.r,gadgets,TWINctrlbm.window0_v.r.attrib);
	/*putbitmap(x2-v[1].w,y1+c->rbbord.h,v[1].w,h2,(trect*)&v[1],base,v[1].attrib);*/
	b-=1;
	
    if (drawmode==tabdrawall)
    if (seltab!=NULL)
      seltab->view->drawview();
}

void tabbox::handleevent(tevent *event)
{    
    tview::handleevent(event);
    if (event->what&EVmousedown)
    {
        if (mp.y<a.y+20)
        {
            tabelement *ctab=rootelem;        
            INT32 x1,x2;
            x1=a.x;
            UINT8 found=0;
            while ((ctab!=NULL)&&(found==0))
            {                
                x2=x1+FNTgetwidth(ctab->name)+24;
                found=((mp.x>=x1)&&(mp.x<=x2));
                if (!found)
                ctab=(tabelement*)ctab->next;
                x1=x2;                
            }
            if ((ctab!=NULL)&&(ctab!=seltab))
            {               
               seltab=ctab;
               tview *cview=ctab->view;
               if (cview!=firstchild)
               {
               cview->prev()->next=cview->next;  
               cview->next=firstchild;
               firstchild=cview;
               drawview();
               evcallback(CMDtabchanged);
               }
            }
            event->what=0;
        }
    }
    
    if (seltab!=NULL)
        seltab->view->handleevent(event);
}

bool tabbox::setstate(UINT32 astate,bool enable)
{
  //return (tview::setstate(astate,enable));
  if (tview::setstate(astate,enable))
  {      
      /*if ((astate & SFfocused) != 0 )
      {
         drawmode=tabdrawborder;
         drawview();
         drawmode=tabdrawall;
      }*/
      return 1;
  }else
      return 0;
}





////////////////////////////////////////////////////////////////////////////////////////
// WINDOW
// USE: this widget is the container class for all things that move around on the desktop
//      It resizes and changes colour when focused. Various flags control it's starting state
//      including if it's got scrollbars or not and which buttons it has added: 
//      an optional menu across the top and flags for no resizing (for dialog boxes)



/*void drawborder(INT32 x1,INT32 y1,INT32 x2,INT32 y2,tblitrect *c,tblitrect *h,tblitrect *v);
void captilebitmapy(INT32 x,INT32 y,INT32 y2,tblitrect *c,tsurface *image);
void captilebitmapx(INT32 x,INT32 y,INT32 x2,tblitrect *c,tsurface *image);*/

void drawclientedge(INT32 x1,INT32 y1,INT32 x2,INT32 y2,tblitrect *c,tblitrect *h,tblitrect *v,tsurface *base)
{
	x1-=TWINctrlbm.ce0_v.l.w;
	x2+=TWINctrlbm.ce0_v.r.w;
	y1-=TWINctrlbm.ce0_h.t.h;
	y2+=TWINctrlbm.ce0_h.t.h;
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



void drawborders(INT32 x1,INT32 y1,INT32 x2,INT32 y2,char *txt,UINT32 options,twindowborders *c,tblitrect *h,tblitrect *v,tsurface *base)
{
	INT32 w1,h1,w2,h2;
	tblitrect *br,*bl;
	if (options)
		br=&c->resize0;
	else
		br=&c->w.br;//resize0;
	bl=&c->w.bl;
	w1=w2=1+x2-x1;
	h1=h2=1+y2-y1;
	w1-=c->lbbord.w+c->rbbord.w;
	w2-=bl->w+br->w;
	h1-=c->lbbord.h+bl->h;
	//h2-=br->h+c->rbbord.w;
	h2=h1;
	x2++;y2++;
	INT32 txtwidth=FNTgetwidth(txt)+10;
	INT32 xp;

	putbitmap(x1,y1,(trect*)&c->lbbord,base,c->lbbord.attrib);
	putbitmap(x2-c->rbbord.w,y1,(trect*)&c->rbbord,base,c->rbbord.attrib);
	//lower
	putbitmap(x1,y2-bl->h,(trect*)bl,base,bl->attrib);
	putbitmap(x2-br->w,y2-br->h,(trect*)br,base,br->attrib);		
	putbitmap(x1+bl->w,y2-h[1].h,w2,h[1].h,(trect*)&h[1],base,h[1].attrib);
	
	putbitmap(x1,y1+c->lbbord.h,v[0].w,h1,(trect*)&v[0],base,v[0].attrib);
	putbitmap(x2-v[1].w,y1+c->rbbord.h,v[1].w,h2,(trect*)&v[1],base,v[1].attrib);
	
	xp=x1+c->lbbord.w-1;
	captilebitmapx(xp,y1,xp+txtwidth,&c->title[0],gadgets);
	t_col=TWINtextcol;
	outtextxy(xp+5,y1+5,txt);
	xp+=txtwidth;
	captilebitmapx(xp,y1,x2-c->rbbord.w,&c->title[3],gadgets);
	//putbitmap(x2-v[1].w,y2-h[1].h,w2,h[1].h,(trect*)&h[1],base,h[1].attrib);*/
}


window::window(INT32 x,INT32 y,INT32 w,INT32 h,char *txt,tview *cview,UINT32 createflags):tgroup(x,y,x+w,y+h)
{
	valuedata data;
	data.max=data.min=data.val=0;
    text=txt;
    trect viewrect(x+TWINctrlbm.window0_v.l.w,y+TWINctrlbm.window0.lbbord.h,w+x-TWINctrlbm.window0_v.r.w,h+y-TWINctrlbm.window0_h.b.h);    
    drawmode=windowdrawall;
    options|=OFtopselect|OFnooverlap;
	growmode=TGdragonx1 | TGdragony1 ;
    
    if (createflags & CFxbar)
    {
        viewrect.b.y-=(TWINctrlbm.scroll.getscrollheight()-1);
        xbar=new slidertype(viewrect.a.x,viewrect.b.y,1+viewrect.width()-TWINctrlbm.scroll.getbuttonwidth(),&data,SAxorient,CMDxvalchanged);
        xbar->growmode=TGdragony2|TGgrowallx;
        viewrect.b.y--;
        insert(xbar);        
    }else
        xbar=NULL;
        
    if (createflags & CFybar)
    {
        viewrect.b.x-=(TWINctrlbm.scroll.down0.w-1);
        ybar=new slidertype(viewrect.b.x,viewrect.a.y,1+viewrect.height(),&data,0,CMDyvalchanged);
        ybar->growmode=TGdragonx2|TGgrowally;
        viewrect.b.x--;
        insert(ybar);
    }else
        ybar=NULL;
	if (xbar && ybar)
		options|=OFchunkyresize;

	//TWINctrlbm.window0.lbbord.hotspot.x
    close=new tbutton(a.x+4,a.y+5,BMreleaseaction,CMDclose,0,gadgets,&TWINctrlbm.windowicons.close0);
    close->growmode=TGdragonx1|TGdragony1|TGgrowx1|TGgrowy1;
    //close->options|=OFautoraise;

    min=new tbutton(b.x-24,a.y+5,BMreleaseaction,CMDminimizewindow,0,gadgets,&TWINctrlbm.windowicons.min0);
	if ((createflags & CFclientedge)!=0)
    {
       viewrect.a.x+=TWINctrlbm.ce0_v.l.w;
	   viewrect.b.x-=TWINctrlbm.ce0_v.r.w;
	   viewrect.a.y+=TWINctrlbm.ce0_h.t.h;
	   viewrect.b.y-=TWINctrlbm.ce0_h.b.h;


       options|=OFclientedge;
    }
    min->growmode=TGdragonx2|TGdragony1|TGgrowx2|TGgrowy1;
    view=cview;
    if ((createflags & CFnoresize)!=0) options|=OFnoresize;     
    if (view)
    {        
        cview->makeinvisible();
        cview->growmode=TGgrowall;
        cview->changebounds(viewrect);
        cview->makevisible();
        insert(cview);
    }
    insert(min);
    insert(close);
    //min->options|=OFautoraise;
}    
//windows draw their borders in a special way...
/*
void drawvedge(UINT32 x1,UINT32 y1,UINT32 y2,tcolour c1,tcolour c2,tcolour c3,tcolour c4)
{
    t_col=c1;vline(x1,y1,y2);
    t_col=c2;vline(x1+1,y1,y2);
    t_col=c3;vline(x1+2,y1,y2);
    t_col=c4;vline(x1+3,y1,y2);   
}

void drawhedge(UINT32 x1,UINT32 y1,UINT32 y2,tcolour c1,tcolour c2,tcolour c3,tcolour c4)
{
    t_col=c1;hline(x1,x2,y1);
    t_col=c2;hline(x1,x2,y1+1);
    t_col=c3;hline(x1,x2,y1+2);
    t_col=c4;hline(x1,x2,y1+3);   
}
*/


void window::draw()
{
    twindowborders *wb;
	thorizborders *whb;
	tvertborders *wvb;
	thorizborders *cehb;
	tvertborders *cevb;
	
	cehb=&TWINctrlbm.ce0_h;
	cevb=&TWINctrlbm.ce0_v;
    if (state & (SFfocused+SFmouseover))
    {      
	  wb=&TWINctrlbm.window1;
	  whb=&TWINctrlbm.window1_h;
	  wvb=&TWINctrlbm.window1_v;      
	  
    }else
    {      
	  wb=&TWINctrlbm.window0;
	  whb=&TWINctrlbm.window0_h;
	  wvb=&TWINctrlbm.window0_v;      
	  
    }


	drawborders(a.x,a.y,b.x,b.y,text,options & OFchunkyresize,wb,(tblitrect*)whb,(tblitrect*)wvb,gadgets);
    //drawborders(a.x,a.y,b.x,b.y,wg,text,options,((xbar==NULL)||(ybar==NULL)));
    if ((options & OFclientedge))
		drawclientedge(view->a.x,view->a.y,view->b.x,view->b.y,(tblitrect*)wb->ce,(tblitrect*)cehb,(tblitrect*)cevb,gadgets);      
      
    if (drawmode==windowdrawall)
      tgroup::draw();
}

bool window::setstate(UINT32 astate,bool enable)
{
  //return (tview::setstate(astate,enable));
  bool ison=((state & (SFfocused+SFmouseover))!=0);
  if (tview::setstate(astate,enable))
  {
      
      if (ison!=((state & (SFfocused+SFmouseover))!=0)) 
      {
         drawmode=windowdrawborder;
         drawview();
         drawmode=windowdrawall;
      }
      return 1;
  }else
      return 0;
}

UINT32 window::getresizemode(INT32 x,INT32 y)
{
	//TWINctrlbm.window0.lbbord.h
	trect tr=*(trect*)view;
	INT32 tb=a.y+TWINctrlbm.window0_h.t.h;
	//trect tr(a.x+TWINctrlbm.window0_v.l.w,a.y+TWINctrlbm.window0_h.t.h,b.x-TWINctrlbm.window0_v.r.w,b.y-TWINctrlbm.window0_h.b.h);
	resizemode=0;
	if (!tr.rcontains(x,y))
	{
		if (x<tr.a.x) 		
			resizemode=TGgrowx1;			
		else if (x>tr.b.x) 		
			resizemode=TGgrowx2;					
		if (resizemode)
		{
			if (y<tb+20) 
				resizemode|=TGgrowy1;
			else if (y>tr.b.y-20) 
				resizemode|=TGgrowy2;
		}
		if (y<tb)
			resizemode=TGgrowy1;
		else if (y>tr.b.y) 		
			resizemode=TGgrowy2;		
		if (resizemode)
		{
			if (x<tr.a.x+20) 
				resizemode|=TGgrowx1;
			else if (x>tr.b.x-20) 
				resizemode|=TGgrowx2;
		}		
		if (!resizemode || options & OFnoresize)
			resizemode=TGgrowall;
		
	}
	
	/*else if (y<a.y+TWINctrlbm.window0.lbbord.h)
		resizemode=TGgrowall;*/
	return resizemode;
}

void window::handleevent(tevent *event)
{
#define MINWINX 100
#define MINWINY 100

    if ( ((state & SFmodal)!=0)&&((event->what & EVmouse)!=0)  )
    {
        
        if (resizemode==TGgrowall)        
            tmove(mdiff.x,mdiff.y);
        else
        {
        trect tmp_loader;
        getbounds(&tmp_loader);
        if ((resizemode&TGgrowx1)!=0)
        {
          tmp_loader.a.x+=mdiff.x;
          if (tmp_loader.width()<MINWINY) tmp_loader.a.x=tmp_loader.b.x-MINWINX;
        }
        if ((resizemode&TGgrowx2)!=0)
        {
          tmp_loader.b.x+=mdiff.x;          
          if (tmp_loader.width()<MINWINX) tmp_loader.b.x=tmp_loader.a.x+MINWINX;
        }
        if ((resizemode&TGgrowy1)!=0)
        {
          tmp_loader.a.y+=mdiff.y;
          if (tmp_loader.height()<MINWINY) tmp_loader.a.y=tmp_loader.b.y-MINWINY;
        }
        if ((resizemode&TGgrowy2)!=0)
        {
          tmp_loader.b.y+=mdiff.y;
          if (tmp_loader.height()<MINWINY) tmp_loader.b.y=tmp_loader.a.y+MINWINY;
        }
        reposview(tmp_loader);
        }
        if (MOUSEreleased)        
		{
            TWINendmodal();           
			MOUSEsetbounds(SC.viewport.a.x,SC.viewport.a.y,SC.viewport.b.x,SC.viewport.b.y);
		}
        
    }else
    {

    tgroup::handleevent(event);
    if ((event->what&EVmousedown)!=0)
    {
        
		resizemode=getresizemode(mp.x,mp.y);
		if (this!=TWINgetroot())
		{
		trect limits=*(trect*)TWINgetpopuptarget();
		MOUSEsetbounds(limits.a.x,limits.a.y,limits.b.x,limits.b.y);
		}
        TWINsetmodal(this);
        event->what=0;
    }else if(event->what==EVcommand)
    {
        valuedata *data=(valuedata*)event->infoptr;
        event->what=0;
        switch(event->command)
        {
            case CMDxvalchanged:sendcommand(CMDxvalchanged,view,data);break;
            case CMDyvalchanged:sendcommand(CMDyvalchanged,view,data);break;
            case CMDxdeltavalchanged:if (xbar!=NULL){xbar->mute();xbar->setval(data->val);xbar->unmute();};break;
            case CMDydeltavalchanged:if (ybar!=NULL){ybar->mute();ybar->setval(data->val);ybar->unmute();};break;
            case CMDxdeltachanged:if (xbar!=NULL){xbar->mute();xbar->setrange(data);xbar->unmute();};break;
            case CMDydeltachanged:if (ybar!=NULL){ybar->mute();ybar->setrange(data);ybar->unmute();};break;
            default:
                 event->what=EVcommand;
                 sendcommand(event->command,owner,data);
        }       
    }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////
// MENU

tmenu::tmenu(INT32 x,INT32 y,UINT32 attrib_,tsurface *sideimg):tview(x,y,x+7,y+7)
{    
    options|=OFselectable;
    mattrib=attrib_;
    sideimage=sideimg;
    sel=oldsel=NULL;
    parentmenu=NULL;
    data.init(sizeof(tmenuitem));
    drawmode=menudrawall;
	growmode=TGgrowallx | TGdragony1 ;
    if ((mattrib & MAxorient)==0)
	{
      b.x+=20;            	  
	}
    else
	{
      b.y+=20;	  
	}
    if (mattrib & MApopup)
        b+=16;          
}

tmenu::~tmenu()
{
    data.destroy(NULL);
}

tmenu *tmenu::findroot()
{
    tmenu *parent=parentmenu;
    if (!parent) return NULL;
    while (parent->parentmenu!=NULL)        
        parent=parent->parentmenu;
    return parent;      
}

tmenuitem *tmenu::additem(char *text,UINT32 command,UINT32 iattrib,tmenu *submenu,tsurface *bm,UINT8 shortkey_)
{
    tmenuitem *newitem = new tmenuitem;
    tpoint size;
    if (!newitem) return NULL;
	if (text)
      strcpy(newitem->text,text);
	else
	  newitem->text[0]=0;
    newitem->command=command;
    newitem->attrib=iattrib;
    newitem->bitmap=bm;
    newitem->shortkey=shortkey_;
    newitem->submenu=submenu;
    
    
    if (submenu!=NULL)
      submenu->parentmenu=this;
    
    data.push((telem*)newitem);

    if ((newitem->attrib & MIAseperator)!=0)
       size.assign(8,4);
    else if (newitem->bitmap==NULL)
       size.assign(8+FNTgetwidth(newitem->text),4+t_charset.ysize);
    else    
       size.assign(12+newitem->bitmap->size.x+FNTgetwidth(newitem->text),4+MAX(newitem->bitmap->size.y,t_charset.ysize));
          
    newitem->size=size;
    INT32 extra=(mattrib & MApopup)?16:0;
    if (mattrib & MAxorient)
    {
        b.x+=size.x+1;
        if ((height()-extra)<size.y)
          b.y=a.y+size.y+extra;
    }else
    {
        if (submenu)
          extra+=24;
        b.y+=size.y+1;
        if ((width()-extra)<size.x)
          b.x=a.x+size.x+extra;
    }
    return newitem;
}

void tmenu::drawitem(INT32 xpos,INT32 ypos,INT32 w,INT32 h,tmenuitem *item)
{
    if (!item) return;
    UINT32 iattrib=item->attrib;    
    tsurface *bitmap=item->bitmap;
    bool selected=((sel==item));//&&((state & SFfocused)!=0)
    if (iattrib & MIA3dlook)
    {       
       if (selected)
       {          
          inboxcol(xpos,ypos,xpos+w,ypos+h);
          xpos++;ypos++;
       }else
          outboxcol(xpos,ypos,xpos+w,ypos+h);
    }
    else
    {
	   trect *bg;
       if (selected)
       {
          
		  bg=(trect*)&TWINctrlbm.menu_bg1;
          t_col=TWINtextselcol;
       }
       else
       {          
		  bg=(trect*)&TWINctrlbm.menu_bg0;
          t_col=TWINtextcol;
       }
	   trect oldsc=SC.viewport;	   
	   SC.viewport.rintersect(xpos,ypos,xpos+w,ypos+h);
	   putbitmap(a.x+TWINctrlbm.menu_v.l.w,a.y+TWINctrlbm.titlemenu_h.t.h,width(),height(),bg,gadgets);
	   SC.viewport=oldsc;
       
    }
    h>>=1;
    INT32 tempy=ypos+ (h-(t_charset.ytextsize >> 1));
    if (iattrib & MIAseperator)
    {
		t_col=TWINtextdisabledhighlightcol;        
        hline(xpos+5,xpos+w-7,ypos+h+1);
        t_col=TWINtextdisabledcol;
        hline(xpos+4,xpos+w-8,ypos+h);
        return;        
    }
    UINT32 x2=xpos+w;
    if (bitmap!=NULL)
    {
        putbitmap(xpos+4,ypos+(h-(bitmap->size.y>>1)),bitmap,0);
        xpos+=bitmap->size.x+4;
    }
    
    if (iattrib & MIAdisabled)
    {        
		t_col=TWINtextdisabledhighlightcol;
		outtextxyu(xpos+5,tempy+2,item->text,item->shortkey);
		t_col=TWINtextdisabledcol;
		outtextxyu(xpos+4,tempy+1,item->text,item->shortkey);        		
    }else
    {
       outtextxyu(xpos+4,tempy+1,item->text,item->shortkey);
       //outtextxy(xpos+4,tempy+1,item->text);
    }
    //the 18 is 16 for the shadow, 2 for an offset.. replace by variables later.
    //if ( !(mattrib & MAxorient) && item->submenu )              
    //    putbitmap(b.x-18-b_right0->size.x,ypos,b_right0,0);

    /*
    if ((item->submenu)&&((mattrib & MAxorient)==0))
      outtextxy(x2-8,tempy,">",1);
    */
}

tpoint tmenu::getitempos(tmenuitem *item)
{
    tpoint retval(0,0);
    tmenuitem *citem=(tmenuitem*)data.first;
    while (citem!=NULL)
    {
       if (citem==item)
          return retval;
        retval.x+=citem->size.x;
        retval.y+=citem->size.y;
        
        citem=(tmenuitem *)citem->next;
    }
    return retval;
}

tmenuitem *tmenu::getitematpos(tpoint p)
{
    INT32 pos=4;
    p-=a;
    tmenuitem *item=(tmenuitem *)data.first;
    while (item!=NULL)
    {
       if ((mattrib & MAxorient)!=0)
       {
           pos+=item->size.x+1;
           if (p.x<=pos) return item;                                
       }else
       {
           pos+=item->size.y+1;
           if (p.y<=pos) return item;                                
       }
       item=(tmenuitem *)item->next;
    }
    return NULL;
        
}


void tmenu::draw()
{
    tmenuitem *item=(tmenuitem*)data.first;
    INT32 xpos=a.x+TWINctrlbm.menu_v.l.w,ypos=a.y+TWINctrlbm.titlemenu_h.t.h;
    INT32 w=width(),temp;
	INT32 h=height();
    if (mattrib & MApopup)
    {
      w-=TWINctrlbm.shadowr.w;
      h-=TWINctrlbm.shadow.m.h;
    }

    if (drawmode==menudrawall)       
		drawborder(a.x,a.y,a.x+w,a.y+h,(tblitrect*)&TWINctrlbm.titlemenu_c,(tblitrect*)&TWINctrlbm.titlemenu_h,(tblitrect*)&TWINctrlbm.menu_v,gadgets);    
    w-=TWINctrlbm.menu_v.l.w+TWINctrlbm.menu_v.r.w;
	h-=TWINctrlbm.titlemenu_h.t.h+TWINctrlbm.titlemenu_h.b.h;
    //draw border
    while (item!=NULL)
    {
        bool drawit=( (drawmode==menudrawall)||
                       ( (item==sel)||(item==oldsel) )
                     );
        if ((mattrib & MAxorient)!=0)
        {
           temp=item->size.x;
           if (drawit)
              drawitem(xpos,ypos,temp,h,item);
           xpos+=temp+1;
        }
        else
        {
            temp=item->size.y;
            if (drawit)
               drawitem(xpos,ypos,w,temp,item);
            ypos+=temp+1;
        }        
        item=(tmenuitem*)item->next;
    }
             
    //fill in the gaps if there are any
    
    if (drawmode==menudrawall)
    {
        if (mattrib & MApopup)
        {
           captilebitmapx(a.x,b.y-TWINctrlbm.shadow.m.h,b.x,&TWINctrlbm.shadow.l,gadgets);
           putbitmap(b.x-TWINctrlbm.shadowr.w,a.y,(trect*)&TWINctrlbm.shadow.t,gadgets,TWINctrlbm.shadow.t.attrib);//);
           putbitmap(b.x-TWINctrlbm.shadowr.w,a.y+TWINctrlbm.shadow.t.h,TWINctrlbm.shadowr.w,height()-(TWINctrlbm.shadow.t.h+TWINctrlbm.shadow.r.h),(trect*)&TWINctrlbm.shadowr,gadgets,TWINctrlbm.shadowr.attrib);           
        }
		trect *bg=(trect*)&TWINctrlbm.menu_bg0;
		trect oldrect=SC.viewport;
        if (mattrib & MAxorient)
        {
            temp=a.x+w+TWINctrlbm.menu_v.l.w;
            if (xpos<temp)
			{
				SC.viewport.rintersect(xpos,ypos,temp,ypos+h);
				putbitmap(a.x,a.y,width(),height(),bg,gadgets);
			}
              
        }else
        {
			temp=a.y+h+TWINctrlbm.titlemenu_h.t.h;                               
            if (ypos<temp)
			{
			  SC.viewport.rintersect(xpos,ypos,xpos+w,temp);
              putbitmap(a.x,a.y,width(),height(),bg,gadgets);
			}
        }
		SC.viewport=oldrect;
    }
    

}

//should only set/unset modal in one place.. hmmm
//combination of keypresses and mouse moves causes it to get stuck
//and then a crash

bool tmenu::opensubmenu(tmenuitem *item,bool askey)
{
    if (!item) return FALSE;
    tmenu *submenu=item->submenu;
    tpoint itempos;
    UINT8 notopen;
    if (submenu)
    {                
        itempos=getitempos(sel);
        notopen=(submenu->owner==NULL);        
        if (notopen)
        {
           if ((mattrib & MAxorient)!=0)               
               submenu->tmoveto(a.x+4+itempos.x,b.y+1);
           else
               submenu->tmoveto(b.x+1,a.y+4+itempos.y);                                  
           TWINgetpopuptarget()->insert(submenu);           
		   submenu->target=target;
        }
        
        submenu->mattrib|=MAactive;           
        if (askey)
        {
            if (!TWINismodal(submenu)) TWINsetmodal(submenu);        
            submenu->oldsel=submenu->sel;
            submenu->sel=(tmenuitem*)submenu->data.first;
            if (!notopen)//just to force a redraw
            {
                 submenu->drawmode=menudrawchange;            
                 notopen=TRUE;
            }            
        }
        if (notopen)        
            submenu->drawview();                
        submenu->drawmode=menudrawall;
        submenu->select();
        return TRUE;
     }
     return FALSE;
}

bool tmenu::setitem(tmenuitem *item,bool askey)
{
    if ((item!=NULL)&&(item!=sel)&&((item->attrib & MIAseperator+MIAdisabled)==0))
    {           
        
        if (sel!=NULL)                    
          closesubmenus();                                
        oldsel=sel;
        sel=item;
        drawmode=menudrawchange;                      
        drawview();
        drawmode=menudrawall;
        if (sel!=NULL)
        {        
           if ((mattrib & MApopup) && askey)
             return TRUE;
           opensubmenu(sel,askey);           
        }
        return TRUE;
    }else
        return FALSE;
}

tmenuitem *tmenu::getnextitem(tmenuitem *item)
{
    tmenuitem *nextitem;
    if (item==NULL)
       nextitem=(tmenuitem *)data.first;
    else
       nextitem=(tmenuitem*)data.nextwrap(item);
    while ((nextitem->attrib & MIAseperator)!=0)    
        nextitem=(tmenuitem*)data.nextwrap(nextitem);
    setitem(nextitem,TRUE);
    return nextitem;    
}

tmenuitem *tmenu::getprevitem(tmenuitem *item)
{
    tmenuitem *previtem;
    if (item==NULL)
       previtem=(tmenuitem *)data.first;
    else
       previtem=(tmenuitem*)data.prevwrap(item);
    while ((previtem->attrib & MIAseperator)!=0)    
        previtem=(tmenuitem*)data.prevwrap(previtem);
    setitem(previtem,TRUE);
    return previtem;    
}

bool tmenu::insideitems(INT32 x,INT32 y)
{
	if (!rcontains(x,y)) return false;

	trect chkme;
	chkme.a=a;
	tmenuitem *item=(tmenuitem*)data.first;
		
	if (mattrib & MAxorient)
	{
		chkme.b.y=b.y;
		chkme.b.x=a.x+TWINctrlbm.menu_v.l.w;
		while (item!=NULL)
		{
			chkme.b.x+=item->size.x+1;        
			item=(tmenuitem*)item->next;
		}
	}else
	{
		chkme.b.x=b.x;
		chkme.b.y=a.y+TWINctrlbm.titlemenu_h.t.h;
		while (item!=NULL)
		{
			chkme.b.y+=item->size.y+1;        
			item=(tmenuitem*)item->next;
		}
	}
	return chkme.rcontains(x,y);
}

void tmenu::handleevent(tevent *event)
{
   tmenu *menu;
   tmenuitem *item;
   bool docommand=true;

   tview::handleevent(event);
   if (event->what & EVkeydown)
       {
           /*t_col=getcolour(255,0,0);
           rectangle(a.x,a.y,b.x,b.y);
           t_col=getcolour(255,255,0);
           if (global_selected)
             rectangle(global_selected->a.x+1,global_selected->a.y+1,global_selected->b.x-1,global_selected->b.y-1);
           */  
           if (mattrib & MAxorient)
           switch (scancode)
           {
               case upkey:scancode=leftkey;break;
               case downkey:scancode=rightkey;break;
               case leftkey:scancode=upkey;break;
               case rightkey:scancode=downkey;break;
           }                                                                          
           switch (scancode)
           {
               case upkey:
                   getprevitem(sel);break;
               case downkey:
                   getnextitem(sel);break;
               case rightkey:
                  if (sel->submenu!=NULL)//&&((mattrib & MApopup)!=0)                  
                      opensubmenu(sel,TRUE);                      
                  else if (parentmenu)
                  {                      
                      parentmenu->getnextitem(parentmenu->sel);
                      parentmenu->select();
                  }
                  break;
                  
               case leftkey:
                  if (parentmenu)
                  {
                    parentmenu->getprevitem(parentmenu->sel);
                    parentmenu->select();
                    
                  }
               break;              
           }
           if (charcode==13 && sel->submenu==NULL)  //if it has a submenu, don't collapse
               colapse(TRUE,this,NULL);
           else if (charcode!=0)
           {
               item=(tmenuitem*)data.first;
               //charcode=upcase(charcode);
               UINT8 shortkey;
               while (item!=NULL)               
               {
                   shortkey=item->shortkey;
                   if (shortkey!=0)
                   if (item->text[shortkey-1]==charcode)
                   {
                     setitem(item,TRUE);
                     break;            
                   }
                   item=(tmenuitem*)item->next;
               }
           }                                     
           
       }else if (state & SFmodal)
       {
           /*t_col=getcolour(0,0,255);
           rectangle(a.x,a.y,b.x,b.y);
           t_col=getcolour(255,0,255);*/
           //if (mattrib & MAactive)
           {
               
               mattrib|=MAactive;
               
               if (insideitems(mp.x,mp.y))
               {
                  item=getitematpos(mp);
                  if (item)               
                    setitem(item,FALSE);
                  else if (parentmenu && (mb!=0)&&((mattrib & MApopup)==0))     //if you click outside the button area, it closes the submenus
                    parentmenu->closesubmenus();
               }else
               {
                   if (sel)
                   {
                       menu=sel->submenu;
                       if ((menu)&&(menu->insideitems(mp.x,mp.y)))
                          menu->handleevent(event);
                   }
                   menu=parentmenu;
                   if ((menu)&&(menu->insideitems(mp.x,mp.y)))                                         
                      menu->handleevent(event);
                   else
					  docommand=false;                                  					  
               }			   
			   if (MOUSEreleased && (!sel || (sel && !sel->submenu)))  //if it has a submenu, don't collapse               
                   colapse(docommand,this,NULL);                                 				  
			   /*else 
				  if (MOUSEreleased)
					 colapse(false,this,NULL);                                 */

           }           
       }else
       if ((event->what & EVmouse)!=0)
       {
           //if mouse is released and menu control hasn't changed, mode changes; check for click rather than release.
           if (((mattrib & MAactive)||(mb!=0))&& getitematpos(mp))
           {
               if (!TWINismodal(this))
                 TWINsetmodal(this);

               /*t_col=getcolour(0,255,0);
               rectangle(a.x,a.y,b.x,b.y);
               t_col=getcolour(0,255,255);*/
               mattrib|=MAactive;                              
           }           
       }
       event->what=0;    
}

//find open leaf node; has to be a leaf node!
tmenu* tmenu::findopenleaf()
{
    tmenu *selmenu=sel->submenu;
    
    while (selmenu!=NULL)
    {
      if (selmenu->sel && selmenu->sel->submenu && (selmenu->sel->submenu->mattrib & MAactive))      
          selmenu=selmenu->sel->submenu;
      else
          return selmenu;          
    }
    return selmenu;    
}


//go down and close
void tmenu::closesubmenus()
{    
    colapse(FALSE,findopenleaf(),this);
}

//go up and close

void tmenu::colapse(UINT8 docommand,tmenu *submenu,tmenu *stopat)
{            
       while (submenu!=NULL && submenu!=stopat)  //clear down menus and call command
       {
           if (docommand && submenu->sel)
             evcallback(submenu->sel->command);
           submenu->oldsel=submenu->sel;
           submenu->sel=NULL;
           
           if ((submenu->mattrib & MApopup)!=0)                          
               submenu->remove();           
           else
           {               
               submenu->drawmode=menudrawchange;
               submenu->drawview();
               submenu->drawmode=menudrawall;
           }
           submenu->mattrib &=~MAactive;  //have to click to get it back
           //should be called in reverse order so that all modal selections are cleared
           if (submenu->state & SFmodal)
             TWINendmodal();
           submenu=submenu->parentmenu;
       }       
}           


bool tmenu::setstate(UINT32 astate, bool enable)
{
    
    if (!enable)
    {
        //do higlighting stuff in here
    }
    bool retval=tview::setstate(astate,enable);
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SCROLL CONTAINER

scrollcontainer::scrollcontainer(INT32 x1,INT32 y1,INT32 x2,INT32 y2,tview *view_,UINT32 attrib_):
tgroup(x1,y1,x2,y2)
{
	valuedata data;
	data.min=data.max=data.val=0;
    //add scroll bars
    //resize view
    trect viewrect(x1,y1,x2,y2);
    attrib=attrib_;
    if (attrib & CFxbar)
    {
        viewrect.b.y-=(TWINctrlbm.scroll.getbuttonheight()-1);
        xbar=new slidertype(a.x,viewrect.b.y,width(),&data,SAxorient,CMDxvalchanged);
        xbar->growmode=TGdragony2|TGgrowallx;
        viewrect.b.y--;
        insert(xbar);    
    }else
        xbar=NULL;
    if (attrib & CFybar)
    {
        viewrect.b.x-=(TWINctrlbm.scroll.getbuttonwidth()-1);
        ybar=new slidertype(viewrect.b.x,a.y,height()+1,&data,0,CMDyvalchanged);
        ybar->growmode=TGdragonx2|TGgrowally;
        viewrect.b.x--;  
        insert(ybar);        
    }else
        ybar=NULL;
    view=view_;
    if (view)
    {        
        view->growmode=TGgrowall;
        insert(view);        
        view->changebounds(viewrect);        
    }
    if (attrib & CFshadow)
    {
        b.x+=16;
        b.y+=16;
    }
    //insert view (calls initialupdate to view) allowing it to call back and resize the scroll bars    
}

/*scrollcontainer(INT32 x1,INT32 y1,tview *view_,UINT32 attrib)::
tgroup(x1,y1,view_->width()+(attrib&CFxbar
{
    //add scroll bars
    //resize view
    trect viewrect(x1,y1,x2,y2);
    if ((attrib&CFxbar))
    {
        viewrect.b.y-=(sb_btn.y-1);
        xbar=new slidertype(a.x,viewrect.b.y,width(),0,0,SAxorient,CMDxvalchanged,NULL);
        xbar->growmode=TGdragony2|TGgrowallx;
        viewrect.b.y--;
        insert(xbar);    
    }else
        xbar=NULL;
    if ((attrib&CFybar)!=0)
    {
        viewrect.b.x-=(sb_btn.x-1);
        ybar=new slidertype(viewrect.b.x,a.y,height()+1,0,0,0,CMDyvalchanged,NULL);
        ybar->growmode=TGdragonx2|TGgrowally;
        viewrect.b.x--;  
        insert(ybar);        
    }else
        ybar=NULL;
    view=view_;
    if (view)
    {        
        view->growmode=TGgrowall;
        insert(view);        
        view->changebounds(viewrect);        
    }
    //insert view (calls initialupdate to view) allowing it to call back and resize the scroll bars    
}
*/
void scrollcontainer::handleevent(tevent *event)
{
    tgroup::handleevent(event);
    if (event->what & (EVmouse + EVkeyboard))
    {
        if (state & SFmodal)
        {
            if (state & SFwaitingforrelease)
            {
                if (mb==0) state &=~SFwaitingforrelease;
            }else
            if ((mb!=0) && (!inview(mp.x,mp.y)))
            {
                TWINendmodal();
                evcallback(CMDcontainermodalended);                
            }                
            else
                tgroup::handleevent(event);              
        }else
            tgroup::handleevent(event);
    }else if (event->what==EVcommand)        
    {
        valuedata *data=(valuedata*)event->infoptr;
        event->what=0;
        switch(event->command)
        {            
            case CMDxvalchanged:sendcommand(CMDxvalchanged,view,data);break;
            case CMDyvalchanged:sendcommand(CMDyvalchanged,view,data);break;
            case CMDxdeltavalchanged:if (xbar!=NULL){xbar->mute();xbar->setval(data->val);xbar->unmute();};break;
            case CMDydeltavalchanged:if (ybar!=NULL){ybar->mute();ybar->setval(data->val);ybar->unmute();};break;
            case CMDxdeltachanged:if (xbar!=NULL){xbar->mute();xbar->setrange(data);xbar->unmute();};break;
            case CMDydeltachanged:if (ybar!=NULL){ybar->mute();ybar->setrange(data);ybar->unmute();};break;
     /*     case CMDmakebarsinvisible:if (ybar!=NULL) ybar->makeinvisible();if (xbar!=NULL) xbar->makeinvisible();break;
            case CMDmakebarsvisible:if (ybar!=NULL) ybar->makevisible();if (xbar!=NULL) xbar->makevisible();break;
       */     
            default:
                 event->what=EVcommand;
                 target->handleevent(event);    
        }
    }
}

void scrollcontainer::draw()
{
    tgroup::draw();
    if (attrib & CFshadow)
    {
       /*captilebitmapx(a.x,b.y-16,b.x-16,TWINBM_shadow,0xFFFFFFFF);
       putbitmap(b.x-16,b.y-16,TWINBM_shadow[3],0xFFFFFFFF);
       captilebitmapy(b.x-16,a.y,b.y-16,&TWINBM_shadow[4],0xFFFFFFFF);*/
    }
}


bordercontainer::bordercontainer(INT32 x,INT32 y,INT32 w,INT32 h,tview *inner):tgroup(x,y,x+w,y+h)
{
    trect be;
    be.rassign(a.x+2,a.y+2,b.x-2,b.y-2);
    inner->changebounds(be);
    insert(inner);
}

bordercontainer::bordercontainer(INT32 x,INT32 y,tview *inner):tgroup(x,y,inner->width()+4,inner->height()+4)
{
    trect be;
    be.rassign(a.x+2,a.y+2,b.x-2,b.y-2);
    inner->changebounds(be);
    insert(inner);
}

void bordercontainer::draw()
{
    //windowgfx *wg;
    
    /*if (state & (SFfocused+SFmouseover))          
      wg=&w_s;
    else          
      wg=&w_u;    */
    
    //drawouteredge(a.x+2,a.y+2,b.x-2,b.y-2,wg);
    tgroup::draw();
}

void bordercontainer::handleevent(tevent *event)
{
    if (event->what & EVmouse + EVkeyboard)
    {
        if (state & SFmodal)
        {
            
            if ((mb!=0) && (!inview(mp.x,mp.y)))
            {
                TWINendmodal();
                evcallback(CMDcontainermodalended);
            }                
            else
                tgroup::handleevent(event);
              
         }else
            tgroup::handleevent(event);
    }else if (event->what==EVcommand)
         target->handleevent(event);
}


