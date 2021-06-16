#include "stdafx.h"
#include <string.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "gl\gl.h"
#include "ttypes.h"
#include "tmaths.h"
#include "gbasics.h"
#include "topengl.h"

tcolour retme;   
/*char opengl_setmode(screenIDType &sid)
{        
	return 0;
} */   

tcolour opengl_getcolour(UINT32 r,UINT32 g,UINT32 b)
{       
	
	return retme;
}       
   
tcolour opengl_getpixel(PIXINT x,PIXINT y)
{  
	
	return retme;
}    

trgba opengl_getrgbpixel(PIXINT x,PIXINT y)
{   
	trgba retme;
	return retme;
}        

void opengl_putpixel(register PIXINT x,register PIXINT y)
{

}               

    
void opengl_hline(PIXINT x1,PIXINT x2,PIXINT y)
{

}


void opengl_vline(PIXINT x,PIXINT y1,PIXINT y2)
{

}
    
        
        
void opengl_line(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2)
{

}

void opengl_bar(PIXINT x1,PIXINT y1,PIXINT x2,PIXINT y2)
{

}



void opengl_tri(trivert *v0,trivert *v1,trivert *v2)     
{	
	glBegin( GL_TRIANGLES);
	   
       glNormal3f(v0->nx,v0->ny,v0->nz);
	   glColor3f(v0->r,v0->g,v0->b);
	   glVertex3f(v0->x,v0->y,-v0->z);	   
	   	
	   glNormal3f(v1->nx,v1->ny,v1->nz);
	   glColor3f(v1->r,v1->g,v1->b);
	   glVertex3f(v1->x,v1->y,-v1->z);	    
	   
	   glNormal3f(v2->nx,v2->ny,v2->nz);
	   glColor3f(v2->r,v2->g,v2->b);
	   glVertex3f(v2->x,v2->y,-v2->z);	 

	glEnd();
}


void opengl_convexpolygon(trivert *points,UINT32 count)
{

}


void opengl_putbitmap(PIXINT x,PIXINT y,PIXINT sx,PIXINT sy,tsurface *pic,UINT32 attrib)
{


          
}



void opengl_circle(INT32 cx,INT32 cy,INT32 radius)
{

}

void opengl_clearouttextxy(PIXINT cx,PIXINT cy,char *txt,PIXINT slen)
{

}


void opengl_outtextxy(PIXINT cx,PIXINT cy,char *txt,PIXINT slen)
{


}




void opengl_drawbytes(PIXINT x,PIXINT y,UINT8 w,UINT8 h,char *bytes)
{

}

    
    
void opengl_cleardevice(void)
{

}

