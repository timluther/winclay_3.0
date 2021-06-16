
//TWIN standard dialogs code
#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <dos.h>

#include "graphics.h"
#include "gprim.h"
#include "strtable.h"
#include "views.h"
#include "logger.h"
#include "widgets.h"

#include "msmouse.h"
//#include "dosstuff.hpp"
#include "dialogs.h"

#define CMDchangecolournumboxesRGB 5000
#define CMDchangecolournumboxesHSV 5001
#define CMDchangecolourdisplaysRGB 5002
#define CMDchangecolourdisplaysHSV 5003
#define CMDsetred               5004
#define CMDsetgreen             5005
#define CMDsetblue              5006
#define CMDsethue               5007
#define CMDsetsaturation        5008
#define CMDsetvalue             5009
#define CMDok                   6000
#define CMDcancel               6001



#define getcolourINT16(res,r,g,b)\
_asm mov ecx,r \
_asm mov edx,g \
_asm mov eax,b \
_asm shr ax,3 \
_asm shr dx,2 \
_asm shl dx,5 \
_asm or ax,dx \
_asm shr cx,3 \
_asm shl cx,11 \
_asm or ax,cx \
_asm mov cx,ax \
_asm ror eax,16 \
_asm mov ax,cx \
_asm mov res,ax;


void shadevertical(trgba col1,trgba col2,UINT16 *base)
{
    INT32 r,g,b,rd,gd,bd,counter=255,sr,sg,sb;
    r=col1.r<<16;g=col1.g<<16;b=col1.b<<16;
    rd=((col2.r<<16)-r)/256;
    gd=((col2.g<<16)-g)/256;
    bd=((col2.b<<16)-b)/256;
    
    while (counter>0)
    {
		UINT16 col;
		sr=r>>16;sg=g>>16;sb=b>>16;

		getcolourINT16(col,sr,sg,sb);
        *base=col;
        r+=rd;
        g+=gd;
        b+=bd;                
        base+=256;
        counter--;
    }    
}

void backview::draw()
{
	if (bm)
		putbitmap(a.x,a.y,width()+1,height()+1,(trect*)bm,gadgets,0);
	else
	{
		t_fillcol=bgcol;
		bar(a.x,a.y,b.x,b.y);
	}
}

tgroupwithlabels::~tgroupwithlabels()
{
    strings.destroy();
}

void tgroupwithlabels::draw()
{
	tgroup::draw();
    labelelem *cs=(labelelem*)strings.first;
    while (cs!=NULL)
    {
        t_col=cs->colour;
        outtextxy(a.x+cs->pos.x,a.y+cs->pos.y,cs->label);
        cs=(labelelem *)cs->next;
    }
}

labelelem* tgroupwithlabels::addstring(char *label,INT32 x,INT32 y,UINT8 attribute,tcolour colour)
{
    labelelem *cs=new labelelem;
    cs->label=label;
    cs->pos.assign(x,y);
    cs->attribute=attribute;
    cs->colour=colour;
    strings.push((telem*)cs);
	return cs;
}

void tgroupwithlabels::drawstring(labelelem *label)
{	
	trect oldvp=SC.viewport;
	trect clip;
	//clip.rassign(a.x+label->pos.x,a.y+label->pos.y,a.x+label->pos.x+FNTgetwidth(label->label),a.y+label->pos.x+t_charset.ysize);//FNTgetheight(label->label)
	//SC.viewport.rintersect(clip);
	//drawview();
	SC.viewport=oldvp;
}

void tgroupwithlabels::setlabel(char *text,labelelem *label)
{
	label->label=text;
	drawstring(label);
}

colourview::colourview(INT32 x,INT32 y,colourselector *cs):tview(x,y,x+255+4,y+255+4)
{
    this->cs=cs;
    colourdisplay=TWINrendercontext->createsurface(256,256,PXFrgb565);    
    rendercolours();
}

colourview::~colourview()
{
    delete colourdisplay;
}


void colourview::draw()
{
    //t_nativepalette=windowcols;
    //drawclientedge(a.x+2,a.y+2,b.x-2,b.y-2,&w_u);
    putbitmap(a.x+2,a.y+2,colourdisplay);
    t_col=getcolour2(255-cs->col.r,255-cs->col.g,255-cs->col.b);
    switch (cs->colourmode)
    {
      case cmRED:
      xp=cs->col.b;yp=cs->col.g;
      break;
      case cmGREEN:
      xp=cs->col.b;yp=cs->col.r;
      break;
      case cmBLUE:
      xp=cs->col.r;yp=cs->col.g;
      case cmHUE:
      xp=cs->s;yp=cs->v;
      break;
      case cmSATURATION:
      xp=cs->h;yp=cs->v;
      break;
      case cmVALUE:
      xp=cs->h;yp=cs->s;
    } 
      
    circle(a.x+xp,a.y+(255-yp),3);             
}


void colourview::rendercolours()
{
    UINT32 r,g,b,h,s,v;
    UINT16 *scrptr=(UINT16*)colourdisplay->scrptr;      
	if (!scrptr) return;
    trgba ccol=cs->col;
    trgba fadecol;
    TWINrendercontext->setoutput(colourdisplay);
    UINT32 xi,yi;
    s=cs->s;
    v=cs->v;
    h=cs->h;        
    fadecol=trgba(0,0,0,0);
    switch (cs->colourmode)
    {
        
      case cmRED:
        r=ccol.r;
        g=255;
        for (xi=0;xi<256;xi++)
        {
           b=0;         
           for (yi=0;yi<256;yi++)         
		   {
			   UINT16 col;
               getcolourINT16(col,r,g,b);
			   b++;
			   *(scrptr++)=col;
		   }
           g--;
        }
      break;
      case cmGREEN:
        g=ccol.g;
        r=255;
        for (yi=0;yi<256;yi++)
        {
           b=0;         
           for (xi=0;xi<256;xi++)            
		   {
			   INT16 col;
               getcolourINT16(col,r,g,b);
			   b++;
			   *(scrptr++)=col;
		   }
           r--;
        }
      break;
      case cmBLUE:
        b=ccol.b;
        g=255;
        for (xi=0;xi<256;xi++)
        {
           r=0;         
           for (yi=0;yi<256;yi++)         
		   {
			   INT16 col;
               getcolourINT16(col,r,g,b);
			   r++;
			   *(scrptr++)=col;               
		   }
           g--;
        }
        break;              
        case cmHUE:
        s=0;
        for (xi=0;xi<256;xi++)
        {           
           COLsethsv(&ccol,h,s++,255);
           shadevertical(ccol,fadecol,scrptr);
           scrptr++;                      
        }
        
        break;
        case cmSATURATION:        
        h=0;
        for (xi=0;xi<256;xi++)
        {           
           COLsethsv(&ccol,h++,s,255);
           shadevertical(ccol,fadecol,scrptr);
           scrptr++;                      
        }
        break;
        case cmVALUE:
        COLsethsv(&fadecol,0,0,v);
        h=0;
        for (xi=0;xi<256;xi++)
        {           
           COLsethsv(&ccol,h++,255,v);
           shadevertical(ccol,fadecol,scrptr);
           scrptr++;                      
        }

        break;
    }    
    TWINrendercontext->setoutput(NULL);    
}

void colourview::handleevent(tevent *event)
{
    if (state&SFmodal)
    {
      if (mb!=0)
      {
      xp=(UINT8)mp.x-a.x;
      yp=(UINT8)255-(mp.y-a.y);
      trgba ccol=cs->col;
      
      switch (cs->colourmode)
      {    
        case cmRED:
          ccol.g=yp;ccol.b=xp;
        break;
        case cmGREEN:
          ccol.r=yp;ccol.b=xp;                
        break;
        case cmBLUE:
          ccol.g=yp;ccol.r=xp;
        break;              
        case cmHUE:
          cs->s=xp;cs->v=yp;
          break;
        case cmSATURATION:
          cs->h=xp;cs->v=yp;
          break;
        case cmVALUE:
          cs->h=xp;cs->s=yp;
          break;
      }
      if (cs->colourmode & cmRGBMASK)
         COLgethsv(&ccol,cs->h,cs->s,cs->v);
      else
         COLsethsv(&ccol,cs->h,cs->s,cs->v);          
      
      if (cs->col!=ccol)
      {
        cs->col=ccol;
        cs->redrawviews(1);
      }
      }else
      {
        TWINendmodal();
		MOUSEsetbounds(0,0,SC.size.x-1,SC.size.y-1);
        evcallback(CMDcolourschangedrelease);
      }
    }else
    if ((event->what&EVmousepressed))    
    {
      TWINsetmodal(this);
	  MOUSEsetbounds(a.x,a.y,b.x-2,b.y-2);
      event->what=0;
    }
    
}



verticalcolourview::verticalcolourview(INT32 x,INT32 y,colourselector *cs):tview(x,y,x+16+4,y+255+4)
{  
   y=0;
   this->cs=cs;
}


//it's just gouraud shading you divet
void verticalcolourview::draw()
{
    UINT8 delta=255;

    trgba ccol=cs->col;
    UINT32 yi;
    INT32 cx=a.x+2;
    INT32 cx2=b.x-2;
    INT32 cy=a.y+2;
    
    //drawclientedge(cx,cy,cx2,b.y-2,&w_u);
    switch (cs->colourmode)
    {       
      case cmRED:
        yp=ccol.r;
        for (yi=0;yi<256;yi++)
        {          
           t_col=getcolour2(delta--,ccol.g,ccol.b);
           hline(cx,cx2,cy+yi);
        }
      break;
      case cmGREEN:
        yp=ccol.g;
        for (yi=0;yi<256;yi++)
        {          
           t_col=getcolour2(ccol.r,delta--,ccol.b);
           hline(cx,cx2,cy+yi);
        }
        break;
      case cmBLUE:
        yp=ccol.b;
        for (yi=0;yi<256;yi++)
        {          
           t_col=getcolour2(ccol.r,ccol.g,delta--);
           hline(cx,cx2,cy+yi);
        }
        break;            
        case cmHUE:
        yp=cs->h;
        for (yi=0;yi<256;yi++)
        {
           COLsethsv(&ccol,delta--,255,255);
           t_col=getcolour2(ccol.r,ccol.g,ccol.b);
           hline(cx,cx2,cy+yi);
        }
        break;
        case cmSATURATION:
        yp=cs->s;
        for (yi=0;yi<256;yi++)
        {
           COLsethsv(&ccol,cs->h,delta--,cs->v);
           t_col=getcolour2(ccol.r,ccol.g,ccol.b);
           hline(cx,cx2,cy+yi);
        }

        break;
        case cmVALUE:
        yp=cs->v;
        for (yi=0;yi<256;yi++)
        {
           COLsethsv(&ccol,cs->h,cs->s,delta--);
           t_col=getcolour2(ccol.r,ccol.g,ccol.b);
           hline(cx,cx2,cy+yi);
        }
        break;
    }
    t_col=getcolour2(255-ccol.r,255-ccol.g,255-ccol.b);
    hline(cx,cx2,cy+(255-yp));   
}

void verticalcolourview::handleevent(tevent *event)
{
    if (state&SFmodal)
    {
      if (mb!=0)
      {
      yp=255-(mp.y-a.y);
      trgba ccol=cs->col;
         
      switch (cs->colourmode)
      {    
        case cmRED:ccol.r=yp;break;
        case cmGREEN:ccol.g=yp;break;
        case cmBLUE:ccol.b=yp;break;              
        case cmHUE:cs->h=yp;break;
        case cmSATURATION:cs->s=yp;break;
        case cmVALUE:cs->v=yp;break;
      }
      if (cs->colourmode & cmRGBMASK)
         COLgethsv(&ccol,cs->h,cs->s,cs->v);
      else
         COLsethsv(&ccol,cs->h,cs->s,cs->v);                
      
      if (cs->col!=ccol)
      {
        cs->col=ccol;
        cs->redrawviews(2);
      }
      }else
      {
        TWINendmodal();
        evcallback(CMDcolourschangedrelease);
		MOUSEsetbounds(0,0,SC.size.x-1,SC.size.y-1);
      }
    }else
    if ((event->what&EVmousepressed))
    {
	  MOUSEsetbounds(a.x,a.y,b.x,b.y-3);
	  
	  
	  
      TWINsetmodal(this);
      event->what=0;
    }
    
}

colourselector::colourselector(INT32 x,INT32 y,trgba *col):tgroup(x,y,x+376,y+280)
{
    this->col=*col;
    INT32 yp=15;
	fiunion stepsize;
	stepsize.ival=1;
    tcollection *buttons=new tcollection(350,yp,430,310);
    TWINdefaultgrowmode=TGdragy1+TGdragx1;

    colourmode=cmRED;
    cview=new colourview(10,5,this);
    cviewv=new verticalcolourview(276,5,this);   
    
    //numbox(x,y,INT32 w,UINT32 updatecommand_,INT32 minval_,INT32 maxval_,INT32 val_=NULL);
	valuedata data;
	data.min=0;
	data.max=255;
	data.val=this->col.r;
    nb_r=new numbox(300,yp,45,stepsize,CMDchangecolourdisplaysRGB,&data,0);    
    nb_r->target=this;    
    data.val=this->col.g;
    nb_g=new numbox(300,yp+=37,45,stepsize,CMDchangecolourdisplaysRGB,&data,0);
    nb_g->target=this;
	data.val=this->col.b;
    nb_b=new numbox(300,yp+=37,45,stepsize,CMDchangecolourdisplaysRGB,&data,0);
    nb_b->target=this;
	data.val=this->col.r;
    nb_h=new numbox(300,yp+=37,45,stepsize,CMDchangecolourdisplaysHSV,&data,0);
    nb_h->target=this;
	data.val=this->col.g;
    nb_s=new numbox(300,yp+=37,45,stepsize,CMDchangecolourdisplaysHSV,&data,0);
    nb_s->target=this;
	data.val=this->col.b;
    nb_v=new numbox(300,yp+=37,45,stepsize,CMDchangecolourdisplaysHSV,&data,0);
    nb_v->target=this;


	
    ok=new tbutton(a.x+300,yp+=37,45,25,BMreleaseaction+BAxcentre+BAycentre,0,13,"Ok");
    //cancel=new tbutton(b.x-60,b.y-30,50,25,windowcols,1,BMreleaseaction,dirinfo->cancelcommand,13,BAxcentre+BAycentre,"Cancel");
    
    
    
    insert(new backview(x,x,x+430,y+320,&TWINctrlbm.window_bg0));
    insert(ok);
    UINT32 flags=BAnooffset|BAycentre|BAnobox|BMglobaltoggleaction;
    yp=0;
    buttons->insertrel(new tbutton(0,yp,flags,CMDsetred,""));
    buttons->insertrel(new tbutton(0,yp+=37,flags,CMDsetgreen,""));
    buttons->insertrel(new tbutton(0,yp+=37,flags,CMDsetblue,""));
    buttons->insertrel(new tbutton(0,yp+=37,flags,CMDsethue,""));
    buttons->insertrel(new tbutton(0,yp+=37,flags,CMDsetsaturation,""));
    buttons->insertrel(new tbutton(0,yp+=37,flags,CMDsetvalue,""));
    
    insertrel(buttons);
    insertrel(cviewv);
    insertrel(cview);
      
    insertrel(nb_r);
    insertrel(nb_g);
    insertrel(nb_b);
    insertrel(nb_h);
    insertrel(nb_s);
    insertrel(nb_v);
    TWINdefaultgrowmode=TGgrowall;
}

void colourselector::redrawviews(UINT8 what)
{
    //if (what & 1)
    cviewv->drawview();
    
    if (what & 2)
    {
       cview->rendercolours();       
    }
    cview->drawview();
    if ((what & 4)==0)
    {
      nb_r->setvalmute(col.r);
      nb_g->setvalmute(col.g);
      nb_b->setvalmute(col.b);
      nb_h->setvalmute(h);
      nb_s->setvalmute(s);
      nb_v->setvalmute(v);
    }
    evcallback(CMDcolourschanged);
          
}

void colourselector::draw()
{
    tgroup::draw();
    t_col=textcol;
    outtextxy(nb_r->a.x+4,nb_r->a.y-14,"Red");
    outtextxy(nb_g->a.x+4,nb_g->a.y-14,"Green");
    outtextxy(nb_b->a.x+4,nb_b->a.y-14,"Blue");
    outtextxy(nb_h->a.x+4,nb_h->a.y-14,"Hue");
    outtextxy(nb_s->a.x+4,nb_s->a.y-14,"Saturation");
    outtextxy(nb_v->a.x+4,nb_v->a.y-14,"Value");
}

void colourselector::handleevent(tevent *event)
{
    tgroup::handleevent(event);
    if (event->what==EVcommand)
    {
        switch (event->command)
        {
          case CMDsetred:colourmode=cmRED;redrawviews(7);break;
          case CMDsetgreen:colourmode=cmGREEN;redrawviews(7);break;
          case CMDsetblue:colourmode=cmBLUE;redrawviews(7);break;
          case CMDsethue:colourmode=cmHUE;redrawviews(7);break;
          case CMDsetsaturation:colourmode=cmSATURATION;redrawviews(7);break;
          case CMDsetvalue:colourmode=cmVALUE;redrawviews(7);break;           
          
          case CMDchangecolourdisplaysRGB:
          col.r=nb_r->data.val;
          col.g=nb_g->data.val;
          col.b=nb_b->data.val;
          COLgethsv(&col,h,s,v);
          nb_h->setvalmute(h);
          nb_s->setvalmute(s);
          nb_v->setvalmute(v);                    
          redrawviews(7);break;
          case CMDchangecolourdisplaysHSV:
          h=nb_h->data.val;
          s=nb_s->data.val;
          v=nb_v->data.val;
          COLsethsv(&col,h,s,v);
          nb_r->setvalmute(col.r);
          nb_g->setvalmute(col.g);
          nb_b->setvalmute(col.b);                    
          redrawviews(7);break;
          case CMDcolourschangedrelease:
          case CMDnumboxreleased:
            evcallback(CMDcolourschangedrelease);
            break;            
          
        }
    }
}


void colourselector::setcolour(UINT8 r,UINT8 g,UINT8 b)
{
    col.r=r;col.g=g;col.b=b;
    redrawviews(3);
}


#ifdef _WIN_VER_
WIN32_FIND_DATA findstruct;
#else
find_t findstruct;
#endif

/*class fileviewstruct
{
    char fullpath[255];
    UINT32 filterindex,filtercount;    
    UINT32 fileindex;
    listelem *presetfilters;            
};    

class fileview:public tgroup
{
    public:
    listbox *directories;
    listbox *files;
    edittextbox *pathname;
    tbutton *ok,*cancel;
    dropbox *filters;
    fileviewstruct *datasave;
        
    char drive[4];
    char path[128];
    char name[8];
    char ext[4];    
                   
    fileview(UINT32 x,UINT32 y,fileviewstruct *dirinfo);
    virtual void handleevent(tevent *event);
    void draw();
    private:
    void getfilelisting();
    void getdirectorylisting();
    UINT32 getcomponents(char *pathname);
    
};
*/


UINT32 filevalid(char *path)
{
    //find_t findstruct;
    //return _dos_findfirst(path,0xFF,&findstruct);
	return 0;
}


fileview::fileview(UINT32 x,UINT32 y,fileviewstruct *dirinfo):tgroup(x,y,x+380,y+240)
{
    TWINdefaultgrowmode=TGdragy2+TGdragx2;
	filenamebuffer.create(1024*20);
	directorynamebuffer.create(1024*20);

    ok=new tbutton(b.x-120,b.y-30,50,25,BMreleaseaction|BAxcentre|BAycentre,dirinfo->okcommand,13,"Ok");
    cancel=new tbutton(b.x-60,b.y-30,50,25,BMreleaseaction|BAxcentre|BAycentre,dirinfo->cancelcommand,13,"Cancel");
    TWINdefaultgrowmode=TGdragy1+TGdragx1;
    drawmode=DTdrawall;        
    pathname=new edittextbox(10+FNTgetwidth(str_file),5,0,NULL,255,135,20);    
    files=new listbox(0,0,100,110,NULL,0,true);
    directories=new listbox(0,0,100,110,NULL,0,true);
    directories->images=fileicons;
    directories->imgwidth=23;
    datasave=dirinfo;    
    
    filters=new dropbox(a.x+5,a.x+170,b.y-30,dirinfo->presetfilters,dirinfo->filtercount);    
    //dropbox->setsel(firinfo->filterindex);
    //filters->list->sel=dirinfo->filterindex;
    filters->list->setval(dirinfo->filterindex);
    copyfilter(searchfilter,filters->list->getseltext());    
    insert(new backview(a.x,a.y,b.x,b.y,&TWINctrlbm.window_bg0));
    getcomponents(dirinfo->fullpath);
    pathname->settext(name);
    pathname->inserttext(strlen(name),ext,strlen(ext));
            
    INT32 ypos=30;
    insertrel(pathname);
    insertrel(new scrollcontainer(180,ypos,355,ypos+150,directories,2));
    insertrel(new scrollcontainer(5,ypos,175,ypos+150,files,2));
    insert(filters);
    
        
    insert(ok);
    insert(cancel);
    getfilelisting();
    getdirectorylisting();            
    TWINdefaultgrowmode=TGgrowall;
}

char *fileview::grabstringmem(theapmem *buf,char *txt,char *extratext)
{	
	char *newbuf;
	UINT32 len=1+strlen(txt);
	if (extratext)
		len+=strlen(extratext);
	newbuf=(char*)buf->grab(len);
	strcpy(newbuf,txt);
	if (extratext)
		strcat(newbuf,extratext);
	
	return newbuf;
}

fileview::~fileview()
{
	filenamebuffer.destroy();
	directorynamebuffer.destroy();
}

bool fileview::isfilter()
{
    return ( strcontains(name,'*') || strcontains(ext,'*') );    
}



INT32 getfilecount(char *fullpath,UINT32 attrib)
{
    UINT32 count=0;
	HANDLE filehandle;
	INT32 retcode;
	filehandle=FindFirstFile(fullpath,&findstruct);	
	
	if (filehandle==INVALID_HANDLE_VALUE)
		return -1;
	retcode=TRUE;
	while (retcode)
	{
		if ((findstruct.dwFileAttributes & attrib) && (findstruct.cFileName[0]!='.'))
			count++;
		retcode=FindNextFile(filehandle,&findstruct);
	}
	FindClose(filehandle);
    /*
    UINT32 doserror=_dos_findfirst(path,attrib,&findstruct);
    while (doserror==0)
    {
        if ((attrib&findstruct.attrib) && (findstruct.name[0]!='.') )
             count++;
        doserror=_dos_findnext(&findstruct);                   
    }*/
    return count;
}

UINT32 fileview::getcomponents(char *pathname)
{
    char tdrive[4];
    char tpath[128];
    char tname[9];
    char text[5];
    UINT32 result=0;
    
    _splitpath( pathname, tdrive,tpath,tname,text);        
    /*strcpy(ttestpath,tdrive);
    strcat(ttestpath,tpath);
    strcat(ttestpath,"*.*");*/
    //if (!filevalid("c:\*.*"))return 128;
        
    //do further analysis in here
    if ((tdrive[0]!=0)&&(strcmp(drive,tdrive))  )
    {
      result|=FLdrivechange;
      strcpy(drive,tdrive);
    }
    if ((tpath[0]!=0)&&(strcmp(path,tpath))  )
    {
      result|=FLpathchange;
      if (!strcmp(tpath,"\\..\\" ))
        setdepth(tpath,getdepth(tpath)-1);
      else if (tpath[0]=='\\')
        strcpy(path,tpath);        
      else 
        strcat(path,tpath);
        
    }
    if ((tname[0]!=0)&&(strcmp(name,tname))  )
    {
      strupr(tname);
      if (directories->find(dirdepth,tname))
      {
        strcat(path,tname);
        strcat(path,"\\");
        result|=FLpathchange;
      }else
      {            
        result|=FLnamechange;
        strcpy(name,tname);
      }
      
    }
    if ((text[0]!=0)&&(strcmp(ext,text))  )
    {
        result|=FLextchange;
        strcpy(ext,text);
    }
    if(isfilter())
    {        
        strcpy(searchfilter,name);
        strcat(searchfilter,ext);
    }
    drawmode=DTdrawtext;
    drawview();
    drawmode=DTdrawall;
        
    return result;     
}

void fileview::getfilelisting()
{
    //_A_DRIVE 0x8 ? updated io.h
    #define _A_DRIVE 0x8
    char fullpath[128];
	UINT32 attrib;
	HANDLE filehandle;
	INT32 retcode;

      
    sprintf(fullpath,"%s%s%s",drive,path,searchfilter);
   
    //attrib=0xFF^_A_SUBDIR+_A_DRIVE        ;  //everything but subdirectories 
	attrib=0xFFFFFFFF ^ (FILE_ATTRIBUTE_DIRECTORY + FILE_ATTRIBUTE_HIDDEN);
    INT32 count=0;
    files->makelist((getfilecount(fullpath,attrib)),NULL,false);
	filenamebuffer.reset();
	//windows version
	filehandle=FindFirstFile(fullpath,&findstruct);
	retcode=TRUE;
	while (retcode)
	{
		if (findstruct.dwFileAttributes & attrib)
			files->setitem(count++,grabstringmem(&filenamebuffer,findstruct.cFileName,NULL),0,0,findstruct.dwFileAttributes,0);
		retcode=FindNextFile(filehandle,&findstruct);
	}
	FindClose(filehandle);

    /*
	//dos version	
    retcode=_dos_findfirst(fullpath,attrib,&findstruct);    
    while (retcode==0)
    {
        if ((attrib&findstruct.attrib)&&(findstruct.name[0]!='.'))        
            files->setitem(count++,findstruct.name,0,0,findstruct.attrib,0);
        retcode=_dos_findnext(&findstruct);                   
    }*/
    files->sortlist(0,count-2);
    files->calcinternalvars();
    files->drawview();
}


void fileview::getdirectorylisting()
{
    #define _A_DRIVE 0x8
    char fullpath[128];
	char dirbuf[128];
    
    INT32 xoffset=0;        
	INT32 count;
    sprintf(fullpath,"%s%s*.*",drive,path);
    
    UINT32 attrib=FILE_ATTRIBUTE_DIRECTORY;
	HANDLE filehandle;
	INT32 retcode;
	//=_A_SUBDIR;  //just get directories
    
    //count depth in here
    dirdepth=getdepth(fullpath);
	count=getfilecount(fullpath,attrib);
	if (count>-1)
	{
		
		directories->makelist(count+dirdepth,NULL,false);		
		directorynamebuffer.reset();
		//find_t findstruct;    
		count=0;
		//generate nested directory list items
		directories->setitem(count,grabstringmem(&directorynamebuffer,drive,"\\"),xoffset+=4,0,0,2);			
		count++;
		char *dir=path+1;
		while (TRUE)
		{          
			dir= getdir(dir,dirbuf);			
			if (!dir) break;
			directories->setitem(count,grabstringmem(&directorynamebuffer,dirbuf,NULL),xoffset+=4,0,0,2);							
			count++;    			
		}	
		
		
		filehandle=FindFirstFile(fullpath,&findstruct);
		retcode=TRUE;
		while (retcode)
		{
			if ((findstruct.dwFileAttributes & attrib)&&(findstruct.cFileName[0]!='.'))
				directories->setitem(count++,grabstringmem(&directorynamebuffer,findstruct.cFileName,NULL),xoffset,0,findstruct.dwFileAttributes,1);
			
			retcode=FindNextFile(filehandle,&findstruct);
		}
		FindClose(filehandle);
		directories->sortlist(dirdepth+1,0);
		directories->calcinternalvars();
		directories->drawview();
	}                            
    /*UINT32 doserror=_dos_findfirst(fullpath,attrib,&findstruct);
    while (doserror==0)
    {
        if ((attrib&findstruct.attrib)&&(findstruct.name[0]!='.'))        
          directories->setitem(count++,findstruct.name,xoffset,0,findstruct.attrib,1);
        doserror=_dos_findnext(&findstruct);                   
    }*/
    
}

void fileview::exitifok()
{
    if ((files->getselcontextattrib()==_A_SYSTEM)&&(datasave->type==FDTsave))
    {
        //messagebox("This is a system file, and overwriting it is potentially dangerous! Are you sure you want to do this?");
    }
    strcpy(datasave->fullpath,drive);
    strcat(datasave->fullpath,path);
    strcat(datasave->fullpath,name);
    strcat(datasave->fullpath,ext);
    logmessage((void*)&textcol,datasave->fullpath);
    datasave->filterindex=filters->list->sel;
    datasave->fileindex=files->sel;                 
    evcallback(datasave->okcommand);
    global_selected=root;
    owner->remove();
    delete owner;             

}

void fileview::handleevent(tevent *event)
{
    tgroup::handleevent(event);
    UINT32 result;
    char nameext[13];
    
    TWINtabkeybehaviour(this,event);
    if (event->what==EVcommand)
    {
        switch (event->command)
        {
            case CMDeditreturnpressed:
              
              result=getcomponents(pathname->txt);
              logmessage((void*)&textcol,drive);
              logmessage((void*)&textcol,path);
              logmessage((void*)&textcol,name);
              logmessage((void*)&textcol,ext);              
              
              if (!isfilter())
              {
                 logmessage((void*)&textcol," exiting with the pathname");                  
                 exitifok();
                 return;
              }
              if (result==128) return;
              //if (result & FLchangefiles)
              if (result!=0)
              {
                 getfilelisting();               
              } 
            
              strcpy(nameext,name);
              strcat(nameext,ext);            
              pathname->settext(nameext,1);
              if (result &(FLpathchange+FLdrivechange))
              {
                
                getdirectorylisting();            
                //redraw directory name            
              }                
            
            break;
            case CMDselectedchanged:
              if (event->infotview==files)
              {
                  pathname->settext(files->info[files->sel].txt,1);
                  getcomponents(pathname->txt);
              }else
              if (event->infotview==filters)
              {                  
                  getcomponents(copyfilter(searchfilter,filters->list->getseltext()));
                  pathname->settext(searchfilter);                  
                  getfilelisting();
              }
              break;
            case CMDitemdclicked:
              if (event->infotview==files)
              {
                  exitifok();
              }                        
              else if (event->infotview==directories)
              {
                  listelem *sel=directories->getsel();
                  if (directories->sel==0)
                    strcpy(path,"\\");
                  else if(sel->image==2)
                  {
                      if (!truncatepathto(path,sel->txt))break;
                  }else
                  {
                    strcat(path,sel->txt);
                    strcat(path,"\\");
                  }
                  getdirectorylisting();
                  getfilelisting();
              }
              break;
           default:
             if (event->command==datasave->okcommand)
             {
               result=getcomponents(pathname->txt);
               logmessage((void*)&textcol,drive);
               logmessage((void*)&textcol,path);
               logmessage((void*)&textcol,name);
               logmessage((void*)&textcol,ext);              
              
               if (!isfilter())
               {
                 logmessage((void*)&textcol," exiting with the pathname");                  
                 exitifok();
                 return;
               }                          
             }else if(event->command==datasave->cancelcommand)
             {
               evcallback(datasave->cancelcommand);
               owner->remove();
               delete owner;
             }
        }
    }
    
}

void fileview::draw()
{    
    t_fillcol=TWINpalette->nativedata[8];
    INT32 dirwidth=directories->a.x+8+FNTgetwidth(str_directories);
    INT32 ypos=pathname->a.y+4;
    if (drawmode==DTdrawall)
    {
      tgroup::draw();
      t_col=textcol;
      outtextxyu(a.x+5,ypos,str_file,1);    
      outtextxyu(directories->a.x,ypos,str_directories,1);
      outtextxyu(a.x+5,filters->a.y-20,"File Types",7);
    }else
      bar(dirwidth,ypos,b.x,ypos+t_charset.ysize);
    t_col=textcol;
    outtextxy(dirwidth,ypos,drive);
    dirwidth+=FNTgetwidth(drive);
    outtextxy(dirwidth,ypos,path);
    dirwidth+=FNTgetwidth(path);
    outtextxy(dirwidth,ypos,name);
    //dirwidth+=FNTgetwidth(name);   
    //outtextxy(dirwidth,ypos,ext);
}
