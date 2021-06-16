//msmouse header file
#ifndef MSMOUSE_H
#define MSMOUSE_H

#define clickwait 3
#define dclickwait 6
#define dclick_xthreshold 2
#define dclick_ythreshold 2
#define linger_for 400

#define MS_PRESSED 0x1
#define MS_RELEASED 0x2
#define MS_CLICKED 0x4
#define MS_DOUBLECLICKED 0x8
#define MS_MOVED 0x10
#define MS_LINGER 0x20
#define MS_VISIBLE 0x40
#define MS_WRAPMODE 0x80
#define MS_WHEEL 0x100

//****Globals****
extern INT32 ms_visible;
extern char oldvis;
extern tpoint mp,moldp,mdiff,internalmp;
extern UINT32 mb,moldb,internalmb;
//extern UINT32 mscan,moldscan;
extern INT32 mwheel,moldwheel,internalmwheel;


extern tpoint msize;
extern UINT32 ms_attrib;

#define MOUSEpressed			  (ms_attrib & MS_PRESSED)
#define MOUSEreleased	  (ms_attrib & MS_RELEASED)
#define MOUSEclicked		  (ms_attrib & MS_CLICKED)
#define MOUSEdoubleclicked (ms_attrib & MS_DOUBLECLICKED)
#define MOUSEmoved		  (ms_attrib & MS_MOVED)
#define MOUSElingering	  (ms_attrib & MS_LINGER)
#define MOUSEvisible		  (ms_attrib & MS_VISIBLE)
#define MOUSEwrapmodeon    (ms_attrib & MS_WRAPMODE)
#define MOUSEwheel    (ms_attrib & MS_WHEEL)


INT32 MOUSEchkouticon(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
INT32 MOUSEchkicon(INT16 x1,INT16 y1,INT16 x2,INT16 y2);
bool MOUSEreset();
bool MOUSEinit(tsurface* bm);
void MOUSEupdatepos(INT32 x,INT32 y);
void MOUSEupdatebutton(INT32 b);
void MOUSEon();
void MOUSEoff();
void MOUSEstoprepeat();
void MOUSEstoprepeatwait(UINT16 wait);

void MOUSEshutdown();
void MOUSEpoll();
void MOUSEsetbounds(trect *rect);
void MOUSEsetbounds(INT32 x1,INT32 y1,INT32 x2,INT32 y2);
void MOUSEsetpos(INT16 x1, INT16 y1);
void MOUSEsetshape(INT16 x,INT16 y,UINT16 *mask);
void MOUSEsetmickeys(INT16 x,INT16 y);
void MOUSEsetsense(INT16 x,INT16 y,INT16 dspeed);
void MOUSEclearflags();
UINT32 KEYgetscan();
UINT32 TIMEgettime();
//var counter,Dcounter,Lcounter:word;

#endif 
