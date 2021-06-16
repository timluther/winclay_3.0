#ifndef _ttypes_h_
#define _ttypes_h_

#define _WIN_VER_

#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define NEITHERTRUENORFALSE -1
#define MAX_ENVSTRINGS 32

//logical modes
#define bPUT 0   //just write the bits, no logical operation
#define bAND 1
#define bOR  2
#define bXOR 3
#define bNOT 4





//#define doublefloatpresicion
#if defined doublefloatpresicion
   #define flt double  
#else
   #define flt float
#endif

//The standard integer type for defining pixel values
#define PIXINT INT32
#define MIN_INT16 -32767
#define MAX_INT16 32767
#define MIN_INT32 -2147483648
#define MAX_INT32 2147483648


#define RFright  0x01
#define RFbellow 0x02
#define RFleft   0x04
#define RFabove  0x08

#define RFabove_right  RFabove+RFright
#define RFbellow_right RFbellow+RFright
#define RFabove_left   RFabove+RFleft
#define RFbellow_left  RFbellow+RFleft
#define RFmiddle       RFabove+RFbellow+RFright+RFleft     

#define SMleftshiftkey 2
#define SMleftctrlkey 4
#define SMleftaltkey 8
#define SMrightshiftkey 2
#define SMrightctrlkey 4
#define SMrightaltkey 8

//#ifdef _WIN_VER_
#define tabkey 0x9  
#define insertkey 0x2D
#define rightkey 0x27
#define rightkeyctrl 0x116 
#define leftkey 0x25 
#define leftkeyctrl 115 
#define upkey 0x26 
#define downkey 0x28 
#define pageupkey 0x21  
#define pagedownkey 0x22
#define homekey 0x24 
#define delkey 0x2E 
#define endkey 0x23 
#define backspacekey 0x8 
#define escapekey 0x27 
#define returnkey 13

/*#else
#define tabkey 9  
#define insertkey 0
#define rightkey 77
#define rightkeyctrl 116 
#define leftkey 75 
#define leftkeyctrl 115 
#define upkey 72 
#define downkey 80 
#define pageupkey 73 
#define pagedownkey 81 
#define homekey 71 
#define delkey 83 
#define endkey 79 
#define backspacekey 8 
#define escapekey 0x1B 
#define returnkey 0x0D
#endif*/

#define stringsize 20
typedef unsigned char UINT8;
typedef signed char INT8;
typedef unsigned short UINT16 ;
typedef signed short INT16;
typedef unsigned int UINT32  ;
typedef signed int INT32 ;
//typedef unsigned int UINT64;
//typedef int INT64;


//typedef char[stringsize] stdstring;
//typedef UINT8 bool;
typedef double *doubleptr;
typedef float *floatptr;
typedef UINT8 *UINT8ptr;
typedef INT8 *INT8ptr;
typedef UINT16 *UINT16ptr;
typedef INT16 *INT16ptr;
typedef UINT32 *UINT32ptr;
typedef INT32 *INT32ptr;

extern void lowcase(char *str);
extern void upcase(char *str);
extern char lowcase(char str);
extern char upcase(char str);

class ttreenode
{
    public:
	union
	{
		ttreenode *next,*left;
	};
	union
	{
		ttreenode *child,*right;
	};
	ttreenode *parent;
	bool isleaf(){return (((UINT32)left+(UINT32)right)==NULL);}
};



class ttree
{
public:
    UINT32 count,size;
    ttreenode* root;
};

//Basic Linked list type



class telem
{
    public:
    telem *next;    
};

typedef telem *telemptr;



class telemdouble
{
    public:
    telemdouble *next,*prev;
};

typedef telemdouble *telemdoubleptr;



typedef bool (*linkcriteriafunction)(telem *elem);
typedef bool (*linkcleanupfunction)(telem *elem);



class tlistbase
{
public:
	UINT32 count;
	void *first,*last;

	virtual void *next(void *data);
	virtual void *prev(void *data);
	void *gethead(){return first;}
	void *gettail(){return last;}
};

/*ass tarray:public tlistbase
{
};*/


class tlinkedlist
{
public:
    UINT32 count;
    UINT32 size;
    telem *first,*last;
    
    void init(UINT16 size);
    void destroy(void (*cleanup)(telem *element)=NULL);
    void reset(){first=last=NULL;count=0;}
    telem *remove(telem *popped,void (*cleanup)(telem *element)=NULL);
    telem *remove(telem *prev,telem *popped,void (*cleanup)(telem *element)=NULL);
    int removelots(linkcriteriafunction lfunc,linkcleanupfunction cfunc=NULL);
    telem *push(telem *newelement);
    telem *addtofront(telem *newelement);
    telem *insert(telem *afterme,telem *newelement);
    
    UINT32 getindex(telem *elem);
    bool isempty(){return !count;}
   
    telem *findelement(INT32 count);
    telem *pop(telem *popped,void (*cleanup)(telem *element)=NULL);
    telem *prev(telem *elem);
    telem *prevwrap(telem *elem);
    telem *nextwrap(telem *elem);
    telem *getlast(){return last;};
    telem *getnext_check(telem *elem){return elem?elem->next:first;};
    void repairlast();
};

//simple list stuff, no tlinkedlist structure. Less flexible, less storage space.

telem *SLadd(telem **prevelem,telem *newelem);
void SLremove(telem *prev,telem *removeelem);
int SLremovelots(telem** first,linkcriteriafunction lfunc,linkcleanupfunction cfunc);
telem *SLprev(telem *first,telem *current);
telem *SLfindlast(telem *first);
void SLaddtoend(telem **first,telem *newe);
void SLdeletelist(telem *first);
void SLreverselist(telem *first);
UINT32 SLgetlength(telem *first);
telem *SLcountfrom(telem *start,UINT32 count);

class theapmem
{
    public:
    char *base,*curr;
    UINT32 size,currindex;

    char *create(UINT32 size_);
    void destroy();
    void reset();
    char *grab(UINT32 size);
    void *grabandsetpointer(void *indata);
    void *getnextpointer();
    char *ungrab(UINT32 size);
	operator char *() {return base;}
};

//Tree Functions
ttreenode *TTtraversetonext(ttreenode *currnode,ttreenode *stopat);
void TTinittree(ttree *tree,UINT32 size);
ttreenode *TTcreatettreenode(ttree *tree);
ttreenode *TTaddchildtonode(ttree *tree,ttreenode *parent,ttreenode *child);
ttreenode *TTgetprevnode(ttree *tree,ttreenode *node); //get previous node on the level of 'node'
ttreenode *TTgetlastnode(ttreenode *node); //get last node entry on the level of 'node'
void TTdestroytree(ttreenode *tree);
UINT32 TTgetdepth(ttreenode *node);
void TTremovenode(ttree *tree,ttreenode *node);

class envvar
{
public:
	char envname[64];
	char *data;
	UINT32 datalen;
};

extern int envcount;
extern envvar envstrings[MAX_ENVSTRINGS];



typedef void* voidptr ;

#define MAKE_ID(A,B,C,D)        (((UINT32)A) | ((UINT32)(B)<<8) | ((UINT32)(C)<<16) | ((UINT32)(D)<<24))
#define NOT(X)                  ((X)==0)
#define ABS(X)                  ((X) < 0 ? -(X) : X)
#define MIN(A,B)                ((A) < (B) ? (A) : (B))
#define MAX(A,B)                ((A) > (B) ? (A) : (B))
#define BOUND(X,MI,MA)  {if(X<MI)X=MI;if(X>MA)X=MA;}




#define SETBIT(val,mask,setreset){(setreset?val|mask:val&~mask);}

inline UINT32 SETBITBYMODE(UINT32 val,UINT32 mask,UINT32 mode)
{
    switch (mode)
    {
        case bNOT:val &=~ mask;break;
        case bXOR:val ^= mask;break;
        case bOR:
        case bPUT:val |= mask;break;                                      
    }
    return val;
}

#define swap(a,b){(a)^=(b);(b)^=(a);(a)^=(b);}
#define swap_cast(a,b){(*(UINT32*)&a)^=(*(UINT32*)&b);(*(UINT32*)&b)^=(*(UINT32*)&a);(*(UINT32*)&a)^=(*(UINT32*)&b);}

//#define swap(a,b) (a)=(a)^(b);(b)=(b)^(a);(a)=(a)^(b)

/*inline void swap(UINT32 &a, UINT32 &b){UINT32 t;t=a;a=b;b=t;}
inline void swap(UINT16 &a, UINT16 &b){UINT32 t;t=a;a=b;b=t;}
inline void swap(UINT8 &a, UINT8 &b){UINT32 t;t=a;a=b;b=t;}
inline void swap(INT32 &a, INT32 &b){INT32 t;t=a;a=b;b=t;}
inline void swap(INT16 &a, INT16 &b){INT32 t;t=a;a=b;b=t;}
inline void swap(INT8 &a, INT8 &b){INT32 t;t=a;a=b;b=t;}
inline void swap(voidptr &a, voidptr &b){voidptr t;t=a;a=b;b=t;}*/



//extern void fswapi(void *a,void *b,UINT32 len);
//string operations
void fswapi(void *a,void *b,UINT32 len);
bool strcontains(char *str,char c);
char *getdir(char *dir,char *dest);
UINT32 getdepth(char *dir);
char *setdepth(char *dir,UINT32 depth);
char *truncatepathto(char *path,char *dir);
char *copyfilter(char *tgt,char *src);
char strgreater(char *a,char *b);

#define UNZERO(A){if (A==0) A=1;}
#define SWAPCHECK(a,b){if (a>b) SWAP(a,b);}
#define getsign(a) ((a>=0)?1:-1)

void setenvstring(char *name,char *data);
void sortenvlist();




/*
Q17. How do I convert Euclidean to Polar coordinates?
-----------------------------------------------------

  With a Euclidean coordinate (x y z), the goal is to convert it into a
  Polar coordinate (latitude, longitude, distance).

  The conversion is as follows:

    -----------------------------------------
    distance  = sqrt( x*x + y*y + z*z )

    x        /= distance;
    y        /= distance;
    z        /= distance;

    xz_dist   = sqrt( x*x + z*z )
    latitude  = atan2( xz_dist, y ) * RADIANS
    longitude = atan2( x,       z ) * RADIANS
    -----------------------------------------


Q18. How do I convert Polar to Euclidean coordinates?
-----------------------------------------------------

  With a Polar coordinate (latitude, longitude, distance), the goal is to
  convert it into a Euclidean coordinate (x y z).

  The conversion is as follows:

    -----------------------------------------
    x = cos( latitude ) . sin( longitude )
    y = sin( latitude )
    z = cos( latitude ) . cos( longitude )
    -----------------------------------------


Q19. How do I convert between rotation systems?
-----------------------------------------------

  Altogether, there are around five different ways in which a rotation
  in three dimensions can be represented:

  These include:

    Euler angles
    Matrices
    Quaternions
    Rotation Axis/Angle
    Spherical Rotation Angles

  Euler angles define each rotation as a combination of rotations in
  each of the three main X, Y and Z axii. Euler angles must be converted
  to matrices before any vertex transformations can be performed.

  Matrices define each rotation as an 2D array of values which are
  multiplied with an array of vertices. They can either be 3x3 or 4x4
  in size.

  Quaternions define each rotation in 4 dimension instead of three. As a
  result they require four floating point values. Quaternions must be
  connverted to matrices before any vertex transformation can be
  performed.

  Rotation Axis/Angles define each rotation as a unit vector along with
  a specifiede rotation angle along that axis. Rotation Axis/Angle pairs
  must be converted into Quaternions before finally being converted into
  a rotation matrix.

  Spherical angles define each rotation in terms of latitude/longitude
  and a rotation angle. They must be converted into a rotation matrix
  through the use of AxisAngle and Quaternion coordinates.


  The conversion paths are as follows:

         +--------------+     +--------------+     +--------------+
         |    Matrix    |<===>|  Quaternion  |<===>|   AxisAngle  |
         +--------------+     +--------------+     +--------------+
                /\                                        /\
                ||                                        ||
                \/                                        \/
         +--------------+                          +--------------+
         | Euler angles |                          |  Spherical   |
         +--------------+                          +--------------+

  A comparision of the various representations is presented below:

    +--------------+------------+---------------------------------------+
    | Data type    | Size       | Usage                                 |
    +--------------+------------+---------------------------------------+
    | Euler Angles | FLOAT * 3  | Must be converted to a matrix         |
    |              |            | Gimbal lock can occur                 |
    +--------------+------------+---------------------------------------+
    | Matrix       | FLOAT * 9  | Can be used directly for vertex       |
    |              | FLOAT * 16 | transformation                        |
    |              |            | 3x3 matrices can be used for rotation |
    |              |            | and scaling only.                     |
    |              |            | 4x4 matrices can be used for both     |
    |              |            | translation and perspective           |
    +--------------+------------+---------------------------------------+
    | Quaternion   | FLOAT * 4  | Must be converted to a matrix         |
    |              |            | No danger of Gimbal lock              |
    +--------------+------------+---------------------------------------+
    | Axis Angle   | FLOAT * 4  | Must be converted to a matrix through |
    |              |            | the use of Quaternions.               |
    |              |            | No danger of Gimbal lock              |
    +--------------+------------+---------------------------------------+
    | Spherical    | FLOAT * 3  | Must be converted to a matrix through |
    |              |            | the use of AxisAngle and Quaternion   |
    |              |            | representations.                      |
    |              |            | No danger of Gimbal lock              |
    +--------------+------------+---------------------------------------+



  */

#endif
