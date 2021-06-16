//Alphanumeric character set handling code. T.Lewis 1996

#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ttypes.h"
#include "tmaths.h"
#include "gbasics.h"
#include "font.h"

//cattrib flags

tfont t_charset;

UINT16 FNTgetcharsize(tfontchar *ch)
{ 
  return (ch->height*ch->bytewidth);
}

UINT16 FNTgetcharstructsize(tfontchar *ch)
{
  return (sizeof(tfontchar)+(ch->height*ch->bytewidth)-1);
}

tfontchar *FNTcreatechar(UINT8 h,UINT8 w)
{
    tfontchar *ch;
   
    ch=new tfontchar[h*(w>>3)];
    ch->height=h;
    ch->width=w;
    ch->bytewidth=w >> 3;
    return ch;
}



UINT32 FNTgetwidth(char *txt)
{
  if (!txt) return 0;
  UINT32 totalwidth=0;
  if ((t_charset.cattrib & CTproportional)==0)
    return t_charset.xsize*strlen(txt);
    
  while (*txt!=0)    
    totalwidth+=t_charset.offsets[*(txt++)].width;

  
  
  return totalwidth;
}

UINT32 FNTgetwidthlength(char *txt,UINT32 length)
{  
  if ((t_charset.cattrib & CTproportional)==0)  
    return t_charset.xsize*MIN(strlen(txt),length);
    
  UINT32 totalwidth=0;    
  while ((*txt!=0)&&(length>0))
  {
    totalwidth+=t_charset.offsets[*txt].width;
    txt++;
    length--;
  }
  return totalwidth;
}

UINT32 FNTgetheight(char *txt)
{
  UINT32 totalheight=0,currmaxheight=0;
  while (txt)
  {
    if (currmaxheight<t_charset.offsets[*txt].height)
      currmaxheight=t_charset.offsets[*txt].height;
    if ((txt==NULL)||(*txt==returnkey))
    totalheight+=currmaxheight;
    txt++;
  }
  return totalheight;
}

INT16 FNTgetindexfrompos(char *txt,int x)
{
  INT16 currlen=0;
  UINT8 count=0;
  while (x>currlen)
  {
    currlen+=t_charset.offsets[*txt].width;   
    txt++;
    count++;
    if (*txt==0) return count;
  }
  return count; 
}

INT16 FNTgetindexbeforepos(char *txt,int x)
{
  INT16 currlen=0;
  UINT8 count=0;
  while (x>currlen)
  {
    currlen+=t_charset.offsets[*txt].width;   
    txt++;
    count++;
    if (*txt==0) return count;
  }
  return count; 
}


void FNTproportionalize(tfont *ch)
{
  char *currbyte;  
  charStruct *currch;
  char bytelop,bitlop,currbit;
  UINT32 i;

  currch=ch->offsets;
  ch->cattrib=CTproportional;
  for (i=0;i<255;i++)
  {
    currch->width=0;
    currbyte=currch->data; //currch->heights
    for (bytelop=0;bytelop<16;bytelop++)
    {
    
      currbit=0;
      bitlop=7;
      while (bitlop>0)
      {            
        if ((*currbyte & (1 << bitlop))!=0)
          currbit=bitlop^7;
        bitlop--;
      }
      if (currbit>currch->width)
          currch->width=currbit;

      currbyte++;
    }
    currch->width+=2;
    currch++;
  }  
  ch->offsets[32].width=6;
    
}

tfont *FNTload(char *filename, tfont *chlist)
{
  if (!chlist) return NULL;
  FILE *f;
  char height;
  UINT16 i;
  if (!(f=fopen(filename,"rb")))return NULL;
  height=fgetc(f);
  //fgets(&height,1,f);
  //fseek(f,1);
  height=16;
  chlist->ysize=height;
  chlist->ytextsize=height-3;
  chlist->xbytesize=1;
  chlist->xsize=8;
    
  for (i=0;i<256;i++)
  {
      //load data into buffer and proportionalize here   
      //ch=chlist->offsets[i]=createchar(8,height);
      chlist->offsets[i].width=8;
      chlist->offsets[i].bytewidth=1;
      chlist->offsets[i].height=height;
      fread(chlist->offsets[i].data,height,1,f);
      //fgets(chlist->offsets[i].data,height,f);
  }

  //blockread(f,ch.typeface,256*scans,norecs);
  FNTproportionalize(chlist);
  fclose(f);
  return chlist;
}

