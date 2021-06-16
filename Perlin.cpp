//TWINf basic widgets code
#include "stdafx.h"
#include <stdlib.h>

#include "graphics.h"
#include "msmouse.h"
#include "views.h"
#include "widgets.h"
#include "dialogs.h"
#include "gprim.h"
#include "perlin.h"



flt minnoise,maxnoise;
 
#define NUMOCTAVES 4

INT32 primes[]={15731,789221,1376312589,15013,780061,1376313223,    15349,780613,1376313691, 15809,780851,1376313977  };
 

flt noise(INT32 x,INT32 y,INT32 idx)
{
  INT32 *pr=&primes[idx*3];  
  x = x + y * 57;
  x = (x<<13) ^ x;
  return ( FLT_1- ( (x * (x * x * pr[0]+ pr[1]) + pr[2]) & 0x7fffffff) / 1073741824.0f);    
}


flt smoothednoise(INT32 x, INT32 y,INT32 idx)
{
    flt corners = ( noise(x-1, y-1,idx)+noise(x+1, y-1,idx)+noise(x-1, y+1,idx)+noise(x+1, y+1,idx) ) / 16.0f;
    flt  sides   = ( noise(x-1, y,idx)  +noise(x+1, y,idx)  +noise(x, y-1,idx)  +noise(x, y+1,idx) ) / 8.0f;
    flt  center  =  noise(x, y,idx) / 4.0f;
    return corners + sides + center;
}

flt interpolateit(flt a, flt b, flt x)
{     
     x = (1 - (flt)cos(x * PI)) * FLT_HALF;
     return  a*(FLT_1-x) + b*x;
}        

flt interpolatednoise(flt x, flt y,INT32 idx)
{
      int int_x    = (INT32)x;
      flt frac_x = x - int_x;

      int int_y    = (INT32)y;
      flt  frac_y = y - int_y;
      
      flt v1 = smoothednoise(int_x,     int_y,idx);
      flt v2 = smoothednoise(int_x + 1, int_y,idx);
      flt v3 = smoothednoise(int_x,     int_y + 1,idx);
      flt v4 = smoothednoise(int_x + 1, int_y + 1,idx);

      flt i1 = interpolateit(v1 , v2 , frac_x);
      flt i2 = interpolateit(v3 , v4 , frac_x);

      return interpolateit(i1 , i2 , frac_y);
}


flt tperlinnoise_2d(flt x, flt y,flt xfreq,flt yfreq,flt p)
{
      flt total = 0;


      for (INT32 i=0;i<NUMOCTAVES;i++)
      {
          flt freqx = xfreq*(i+1);
          flt freqy = yfreq*(i+1);
          flt amplitude = p*(i+1);
          total += interpolatednoise(x * xfreq, y * yfreq,i) * amplitude;
      }
      return total;
}



perlinview::perlinview(INT32 x,INT32 y):tgroup(x,y,x+300,y+400)
{
	tpalette *pal=TWINrendercontext->createpalette(256,0);
    for (UINT32 i=0;i<256;i++)
      pal->setentry(i,trgba(i,i,i,0));

    tsurface *img=TWINrendercontext->createsurface(256,256,PXFrgb565);
	img->palette=pal;
	image=new imageview(20,20,img);
    heightfield=new flt[256*256];
    
    TWINrendercontext->setoutput(img);
    for (INT32 xi=0;xi<img->size.x;xi++)
      for (INT32 yi=0;yi<img->size.y;yi++)
      {
          t_col=yi;
          putpixel(xi,yi);
      }
    TWINrendercontext->setoutput(NULL);
    drawmode=0;
    UINT16 yof=10;    
    insert(new backview(a.x,a.y,b.x,b.y,&TWINctrlbm.window_bg0));
    TWINdefaultgrowmode=TGdragy1+TGdragx1;
	fiunion stepsize;
	stepsize.ival=1;
	valuedata data;
	data.min=0;
	data.max=1000;
	data.val=10;
    freqx=(numbox*)insertrel(new numbox(280,yof+=35,50,stepsize,CMDperlinchanged,&data,0));
    freqy=(numbox*)insertrel(new numbox(280,yof+=35,50,stepsize,CMDperlinchanged,&data,0));
    persistance=(numbox*)insertrel(new numbox(280,yof+=35,50,stepsize,CMDperlinchanged,&data,0));
    xoff=(numbox*)insertrel(new numbox(280,yof+=35,50,stepsize,CMDperlinchanged,&data,0));
    yoff=(numbox*)insertrel(new numbox(280,yof+=35,50,stepsize,CMDperlinchanged,&data,0));
    insertrel(new tbutton(280,yof+=35,45,25,BMreleaseaction+BAxcentre+BAycentre,CMDredrawperlin,13,"Do it"));
	
	insertrel(image);
    TWINdefaultgrowmode=TGgrowall;    
}

void perlinview::updatepattern()
{
    flt fx,fy,p,xf,yf;
    fx=freqx->data.val /10.0f;
    fy=freqy->data.val /10.0f;
    p=persistance->data.val /10.0f;
    xf=xoff->data.val /10.0f;
    yf=yoff->data.val /10.0f;
	tpoint size=image->img->size;
    TWINrendercontext->setoutput(image->img);
    minnoise=10000000000;
    maxnoise=-1000000000;
    flt *ch=heightfield;
    for (INT32 x=0;x<size.x;x++)
      for (INT32 y=0;y<size.y;y++)
      {
          flt nse=tperlinnoise_2d((x+xf),(y+yf),fx,fy,p);
          if (nse<minnoise) minnoise=nse;
          if (nse>maxnoise) maxnoise=nse;
          *ch=nse;
          ch++;
      }
    ch=heightfield;
    flt range=maxnoise-minnoise;
    for (x=0;x<size.x;x++)
      for (INT32 y=0;y<size.y;y++)
      {
          *ch=(*ch+minnoise) / range;
		  UINT8 val=float2int(*ch*255.0f) ;
          t_col=getcolour(BUILDRGBA(val,val,val,0));
          putpixel(x,y);
          ch++;          
      }



    TWINrendercontext->setoutput(NULL);
}

void perlinview::draw()
{
    if (!drawmode)
      tgroup::draw();
    //putbitmap(a.x+20,a.y+20,image);
    //sprintf(buf,"min:%f,max:%f",minnoise,maxnoise);
    //t_col=textcol;
    //outtextxy(a.x+20,a.y+5,buf);

}

void perlinview::handleevent(tevent *event)
{
    tgroup::handleevent(event);
    if (event->what==EVcommand)
    {
        if (event->command==CMDredrawperlin)
        {
        updatepattern();        
        image->drawview();        
        }
    }
}
