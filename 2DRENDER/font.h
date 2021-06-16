#ifndef _font_h_
#define _font_h_
typedef struct tfontchar
{
  UINT8 height,width,bytewidth;
  char data[32];
}charStruct;

typedef struct tfont
{
  //font data is structured like this:
  //char Height,char width,char bytewidth and then the bit data.
  //Use the macros to access individual bits
  tfontchar offsets[256];  //pointer offsets to chardata in 'fontdata'
  UINT8 xbytesize;
  UINT8 ysize;              //maximum y size
  UINT8 ytextsize;           //extender y size
  
  UINT8 xsize;              
 
  UINT8 cattrib;       //attribute byte
  union {
    UINT8 numplanes;   //how many bitplanes for the data
    UINT8 numbits;     //or how many bits per pixel
  };
  

}tfont;

extern tfont t_charset;


////FONT ROUTINE DEFINTIONS//////////////////////////////////////////////////////////

inline UINT8 FNTgetcharwidth(charStruct *ch)
{
    return (t_charset.cattrib & CTproportional!=0)?ch->width:t_charset.xsize;
}

inline UINT8 FNTgetcharwidth(char ch)
{
    return (t_charset.cattrib & CTproportional!=0)?t_charset.offsets[ch].width:t_charset.xsize;
}    

void FNTnotfont(tfontchar *ch);
void FNTrorfont(tfontchar *ch);
void FNTrodfont(tfontchar *ch);
void FNTrolfont(tfontchar *ch);
void FNTroufont(tfontchar *ch);
void FNTflipxfont(tfontchar *ch);
void FNTflipyfont(tfontchar *ch);
void FNTsavefontlist(char *filename,tfont* chlist);
tfont *FNTload(char *filename,tfont* chlist);
void FNTproportionalize(tfont* chlist);
UINT32 FNTgetwidth(char *txt);
UINT32 FNTgetwidthlength(char *txt,UINT32 pos);
UINT32 FNTgetheight(char *txt);
INT16 FNTgetindexfrompos(char *txt,int x);

#endif