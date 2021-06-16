
#include <math.h>
#include "stdafx.h"
#include "ttypes.h"
#include "tmaths.h"


float retfloat;
INT32 retint;
double flthalf=0.5;

//simple rotation routines
void simpleyrot(flt r,vector *v)
{
    flt cr=(flt)cos(r);    
    flt sr=(flt)sin(r);
    flt tx=(flt)v->x;
    v->x=v->x*cr + v->z*-sr;
    v->z=tx*sr +  v->z*cr;    
}

void simplexrot(float r,vector *v)
{
    flt cr=(flt)cos(r);    
    flt sr=(flt)sin(r);    
    flt ty=(flt)v->y;
    v->y=(v->y*cr) + (v->z*sr);
    v->z=(ty*-sr)  + (v->z*cr);
}    

void simplezrot(float r,vector *v)
{
    flt cr=(flt)cos(r);    
    flt sr=(flt)sin(r);    
    flt ty=(flt)v->y;
    v->y=(ty*cr) + (v->x*sr);
    v->x=(ty*-sr)  + (v->x*cr);
}    



vector planelineintersection(vector n,flt d,vector p1,vector p2)
{
        // returns the point where the line p1-p2 intersects the plane n&d
        vector dif  = p2-p1;
        flt dn= n^dif;
        flt  t = -(d+(n^p1) )/dn;
        return p1 + (dif*t);
}

bool concurrent(vector a,vector b)
{
        return(a.x==b.x && a.y==b.y && a.z==b.z);
}


//from hulio elasis
flt linear_interpolate(flt a, flt b, flt x)
{
      return  a*(1-x) + b*x;
}

// Cosine Interpolation: 
//This method gives a much smother curve than Linear
// Interpolation. It's clearly better and worth the effort if you can afford
//the very slight loss in speed.  
flt cosine_interpolate(flt a, flt b, flt x)
{
    //=a * (FLT_1- ((1.0-cos(x*PI))/2) + b* (1.0-cos(x*PI))/2)
    flt f = (FLT_1 - (flt)cos(x * 3.1415927)) * FLT_HALF;
    return  a*(FLT_1-f) + b*f;
}

flt intnoise(UINT32 x)                   
{
   x = (x<<13) ^ x;
   return (flt)( FLT_1 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);       
}

vector *vector::multmatrix(matrix4x4 *mat,vector *into)
{
    into->x=(x*mat->m00 +y*mat->m01 +z*mat->m02);
    into->y=(x*mat->m10 +y*mat->m11 +z*mat->m12);
    into->z=(x*mat->m20 +y*mat->m21 +z*mat->m22);
    return into;
}

vector *vector::multmatrixtrans(matrix4x4 *mat,vector *into)
{
    into->x=(x*mat->m00 +y*mat->m01 +z*mat->m02)+mat->p.x;
    into->y=(x*mat->m10 +y*mat->m11 +z*mat->m12)+mat->p.y;
    into->z=(x*mat->m20 +y*mat->m21 +z*mat->m22)+mat->p.z;
    return into;
}

vector *vector::multmatrixtrans_delta(matrix4x4 *mat,vector *into,flt delta)
{
    into->x=((x*mat->m00 +y*mat->m01 +z*mat->m02)+mat->p.x)* delta;
    into->y=((x*mat->m10 +y*mat->m11 +z*mat->m12)+mat->p.y)* delta;
    into->z=((x*mat->m20 +y*mat->m21 +z*mat->m22)+mat->p.z)* delta;
    return into;
}


vector *vector::multmatrixinv(matrix4x4 *mat,vector *into)
{
    into->x=(x*mat->m00 +y*mat->m10 +z*mat->m20);
    into->y=(x*mat->m01 +y*mat->m11 +z*mat->m21);
    into->z=(x*mat->m02 +y*mat->m12 +z*mat->m22);
    return into;
}

vector *vector::multmatrixtransinv(matrix4x4 *mat,vector *into)
{
    into->x=(x*mat->m00 +y*mat->m10 +z*mat->m20)-mat->p.x;
    into->y=(x*mat->m01 +y*mat->m11 +z*mat->m21)-mat->p.y;
    into->z=(x*mat->m02 +y*mat->m12 +z*mat->m22)-mat->p.z;
    return into;
}


matrix4x4ptr LMrottrans(matrix4x4ptr m,flt rx,flt ry,flt rz,flt xt,flt yt,flt zt)
{
  flt cx=(flt)cos(rx);
  flt sx=(flt)sin(rx);
  flt cy=(flt)cos(ry);
  flt sy=(flt)sin(ry);
  flt cz=(flt)cos(rz);
  flt sz=(flt)sin(rz);

  m->m00=cy*cz;          m->m01=cy*sz;          m->m02=-sy;    
  m->m10=sx*sy*cz-cx*sz; m->m11=sx*sy*sz+cx*cz; m->m12=sx*cy;    
  m->m20=cx*sy*cz+sx*sz; m->m21=cx*sy*sz-sx*cz; m->m22=cx*cy;
  m->p.moveto(xt,yt,zt);
  return m;
}

matrix4x4ptr LMrottransinv(matrix4x4ptr m,flt rx,flt ry,flt rz,flt xt,flt yt,flt zt)
{
  /*flt cx=cos(rx);
  flt sx=sin(rx);
  flt cy=cos(ry);
  flt sy=sin(ry);
  flt cz=cos(rz);
  flt sz=sin(rz);
    
  m->m00=cy*cz;          m->m01=cy*sz;         m->m02=-sy;  
  m->m10=sx*sy*cz+cx*-sz;m->m11=sx*sy*sz+cx*cz;m->m12=sx*cy;
  m->m20=cx*sy*cz-sx*-sz;m->m21=cx*sy*sz-sx*cz;m->m22=cx*cy;*/
  
  matrix4x4 tmpmat;
  LMxrotation(m,rx);
  LMyrotation(&tmpmat,ry);
  LMconcatenatematrices(&tmpmat,m,m);
  LMzrotation(&tmpmat,rz);
  LMconcatenatematrices(&tmpmat,m,m);
  m->p.moveto(xt,yt,zt);    
  return m;
}

matrix4x4ptr LMrotatearound(matrix4x4ptr mat,flt rx,flt ry,flt rz,vector *vec)
{
  matrix4x4 tmpmat;
  matrix4x4 tmpmat2;
  
  LMtranslate(mat,-vec->x,-vec->y,-vec->z);  
  LMrottransinv(&tmpmat,rx,ry,rz,0,0,0);
  LMconcatenatematrices(mat,&tmpmat,&tmpmat2);
  LMtranslate(&tmpmat,vec->x,vec->y,vec->z);
  LMconcatenatematrices(&tmpmat2,&tmpmat,mat);
  return mat;
}


matrix4x4ptr LMtransrot(matrix4x4ptr mat,flt rx,flt ry,flt rz,flt xt,flt yt,flt zt)
{
  matrix4x4 tmpmat;

  LMtranslate(mat,xt,yt,zt);  
  LMxrotation(&tmpmat,rx);
  LMconcatenatematrices(&tmpmat,mat,mat);
  LMyrotation(&tmpmat,ry);
  LMconcatenatematrices(&tmpmat,mat,mat);
  LMzrotation(&tmpmat,rz);
  LMconcatenatematrices(&tmpmat,mat,mat);
  return mat;
}

void LMtranspose(matrix4x4ptr mat)
{
   matrix4x4 tmpmat;
   tmpmat=*mat;
   mat->m01=tmpmat.m10;
   mat->m02=tmpmat.m20;
   mat->m10=tmpmat.m01;
   mat->m12=tmpmat.m21;
   mat->m20=tmpmat.m02;
   mat->m21=tmpmat.m12;
}

void LMinvert(matrix4x4ptr mat)
{
   flt tmppx,tmppy;
   LMtranspose(mat);
   tmppx=mat->p.x;tmppy=mat->p.y;
   mat->p.x =(-mat->m00*tmppx)+(-mat->m01*tmppy)+(-mat->m02*mat->p.z);
   mat->p.y =(-mat->m10*tmppx)+(-mat->m11*tmppy)+(-mat->m12*mat->p.z);
   mat->p.z =(-mat->m20*tmppx)+(-mat->m21*tmppy)+(-mat->m22*mat->p.z);
}


matrix4x4ptr LMconcatenatematrices(register matrix4x4ptr m1,register matrix4x4ptr m2,matrix4x4ptr result)
{
  static matrix4x4 temp;
  //check the pointers, if they are the same, DO SOMETHING! 
  //if (m1==m2) return m1;
  if (m2==result)
  {
     temp=*m2;
     m2=&temp;
  }else if (m1==result)
  {
     temp=*m1;
     m1=&temp;
  }
  

  result->m00=(m1->m00*m2->m00 + m1->m01*m2->m10 + m1->m02*m2->m20);
  result->m10=(m1->m10*m2->m00 + m1->m11*m2->m10 + m1->m12*m2->m20);
  result->m20=(m1->m20*m2->m00 + m1->m21*m2->m10 + m1->m22*m2->m20);

  result->m01=(m1->m00*m2->m01 + m1->m01*m2->m11 + m1->m02*m2->m21);
  result->m11=(m1->m10*m2->m01 + m1->m11*m2->m11 + m1->m12*m2->m21);
  result->m21=(m1->m20*m2->m01 + m1->m21*m2->m11 + m1->m22*m2->m21);

  result->m02=(m1->m00*m2->m02 + m1->m01*m2->m12 + m1->m02*m2->m22);
  result->m12=(m1->m10*m2->m02 + m1->m11*m2->m12 + m1->m12*m2->m22);
  result->m22=(m1->m20*m2->m02 + m1->m21*m2->m12 + m1->m22*m2->m22);

  result->p.x=(m1->m00*m2->p.x + m1->m01*m2->p.y + m1->m02*m2->p.z)+m1->p.x;
  result->p.y=(m1->m10*m2->p.x + m1->m11*m2->p.y + m1->m12*m2->p.z)+m1->p.y;
  result->p.z=(m1->m20*m2->p.x + m1->m21*m2->p.y + m1->m22*m2->p.z)+m1->p.z;


  return result;
}

vector *LMmatrixtransformvec(register matrix4x4ptr mat,register vector *vec,vector *result)
{
    static vector tempvec;
    if (vec==result)
    {
        tempvec=*vec;
        vec=&tempvec;
    }
    result->x=(mat->m00*vec->x) + (mat->m10*vec->y) + (mat->m20*vec->z) +mat->p.x;
    result->y=(mat->m01*vec->x) + (mat->m11*vec->y) + (mat->m21*vec->z) +mat->p.y;
    result->z=(mat->m02*vec->x) + (mat->m12*vec->y) + (mat->m22*vec->z) +mat->p.z;
    
    return result;
}

matrix4x4ptr LMinversematrix(matrix4x4ptr mat)
{
      return mat;
}    

//[1 0 0 0]
//[0 1 0 0]
//[0 0 1 0]
//[0 0 0 1]

matrix4x4ptr LMidentity(matrix4x4ptr mat)
{
    
    mat->m01 = mat->m02 = 
    mat->m10 = mat->m12 = 
    mat->m20 = mat->m21 = 
    mat->p.x = mat->p.y = mat->p.z=0;
    
    //now the diagonal entries
    mat->m00=mat->m11=mat->m22=1;
    return mat;
}


//[1  0   0   0]
//[0  cr  sr  0]
//[0  -sr cr  0]
//[0  0   0   1]

matrix4x4ptr LMxrotation(register matrix4x4ptr mat,flt rotation)
{
    flt cr=(flt)cos(rotation);
    flt sr=(flt)sin(rotation);
    
	mat->m11=cr;     mat->m12=sr; 
    mat->m21=-sr;    mat->m22=cr; 
    
    //the rest are as in an identity matrix
    mat->m01 = mat->m02 =  mat->m10 = mat->m20 =
    mat->p.x = mat->p.y = mat->p.z=0;        
    //remaining identity diagonals (1's)
    mat->m00=1;
    return mat;
}


//[cr 0  -sr 0]
//[0  1  0   0]
//[sr 0  cr  0]
//[0  0  0   1]


matrix4x4ptr LMyrotation(register matrix4x4ptr mat,flt rotation)
{
 
    flt cr=(flt)cos(rotation);
    flt sr=(flt)sin(rotation);
    mat->m00=cr;  mat->m02=sr;
    mat->m20=-sr;  mat->m22=cr;
   
    //the rest are as in an identity matrix
    mat->m01 = mat->m10 = mat->m12 = mat->m21 =       
    mat->p.x = mat->p.y = mat->p.z=0;             
    //remaining identity diagonals (1's)
    mat->m11 =1;
    return mat;
}

//[cr  sr  0  0]
//[-sr cr  0  0]
//[0   0   1  0]
//[0   0   0  1]

matrix4x4ptr LMzrotation(register matrix4x4ptr mat,flt rotation)
{
    flt cr=(flt)cos(rotation);
    flt sr=(flt)sin(rotation);
    
    mat->m00=cr;  mat->m01=sr; 
    mat->m10=-sr; mat->m11=cr; 
   
    //the rest are as in an identity matrix
    mat->m02 = mat->m12 = mat->m20 = 
    mat->m21 = mat->p.x = mat->p.y = mat->p.z=0; 
    //remaining identity diagonals (1's)
    mat->m22 = 1;
    return mat;
}
matrix4x4ptr LMxyzrotation(register matrix4x4ptr m,flt xrotation,flt yrotation,flt zrotation)
{
    flt cx=(flt)cos(xrotation);
    flt sx=(flt)sin(xrotation);
    flt cy=(flt)cos(yrotation);
    flt sy=(flt)sin(yrotation);
    flt cz=(flt)cos(zrotation);
    flt sz=(flt)sin(zrotation);

    m->m00=cy*cz;          m->m01=cy*sz;         m->m02=-sy;   
    m->m10=sx*sy*cz-cx*sz; m->m11=sx*sy*sz+cx*cz;m->m12=sx*cy;
    m->m20=cx*sy*cz+sx*sz; m->m21=cx*sy*sz-sx*cz;m->m22=cx*cy;

    m->p=0;        
    return m;
}

matrix4x4ptr LMzyxrotation(register matrix4x4ptr m,flt xrotation,flt yrotation,flt zrotation)
{
    flt cx=(flt)cos(xrotation);
    flt sx=(flt)sin(xrotation);
    flt cy=(flt)cos(yrotation);
    flt sy=(flt)sin(yrotation);
    flt cz=(flt)cos(zrotation);
    flt sz=(flt)sin(zrotation);
    
    m->m00=cy*cz;          m->m01=cy*sz;         m->m02=-sy;  
    m->m10=sx*sy*cz+cx*-sz;m->m11=sx*sy*sz+cx*cz;m->m12=sx*cy;
    m->m20=cx*sy*cz-sx*-sz;m->m21=cx*sy*sz-sx*cz;m->m22=cx*cy;
    m->p.moveto(0,0,0);    
    return m;
}    

matrix4x4ptr LMaddscale(register matrix4x4ptr m,register matrix4x4ptr mr,flt scx,flt scy,flt scz)
{
    mr->m00=m->m00*scx; mr->m01=m->m01*scy; mr->m02=m->m02*scz;
    mr->m10=m->m10*scx; mr->m11=m->m11*scy; mr->m12=m->m12*scz;
    mr->m20=m->m20*scx; mr->m21=m->m21*scy; mr->m22=m->m22*scz;
    mr->p=m->p;
    return mr;
}

matrix4x4ptr LMrst(register matrix4x4ptr m,flt xrotation,flt yrotation,flt zrotation,flt scx,flt scy,flt scz,flt tx,flt ty,flt tz)
{
    flt cx=(flt)cos(xrotation);
    flt sx=(flt)sin(xrotation);
    flt cy=(flt)cos(yrotation);
    flt sy=(flt)sin(yrotation);
    flt cz=(flt)cos(zrotation);
    flt sz=(flt)sin(zrotation);

    m->m00=cy*cz*scx;          m->m01=cy*sz*scy;         m->m02=-sy*scz;  
    m->m10=sx*sy*cz+cx*-sz*scx;m->m11=sx*sy*sz+cx*cz*scy;m->m12=sx*cy*scz;
    m->m20=cx*sy*cz-sx*-sz*scx;m->m21=cx*sy*sz-sx*cz*scy;m->m22=cx*cy*scz;
    m->p.moveto(tx,ty,tz);    
    
    return m;
}


//pythag: a =sqrt( b*b + c*c )
//theta=sin(opp/adj)
flt getangle(INT16 ax,INT16 ay,INT16 bx,INT16 by)
{
    ax-=bx;
    ay-=by;
    
    return (flt)atan(ay/ax);
}

matrix4x4ptr LMaxisrotation(register matrix4x4ptr mat,flt x,flt y,flt z,flt rotation)
{
    flt c=(flt)cos(rotation),s=(flt)sin(rotation),t=1-c,
    txz=t*x*z,tyz=t*y*z,txy=t*x*y,
    sx=s*x,sy=s*y,sz=s*z;
       
    mat->m00=t*x*x+c;   mat->m01=txy+sz;    mat->m02=txz-sy;
    mat->m10=txy-sz;    mat->m11=t*y*y+c;   mat->m12=tyz+sx;
    mat->m20=txz+sy;    mat->m21=tyz-sx;    mat->m22=t*z*z+c;
    
    mat->p.x=mat->p.y=mat->p.z=0;
    return mat;
}    
    

        


//[x 0 0 0]
//[0 y 0 0]
//[0 0 z 0]
//[0 0 0 1]

matrix4x4ptr LMscale(register matrix4x4ptr mat,flt xscale,flt yscale,flt zscale)
{  
   
   mat->m01 = mat->m02 = 
   mat->m10 = mat->m12 = 
   mat->m20 = mat->m21 = 
   mat->p.x = mat->p.y = mat->p.z=0;
    
    //now the diagonal entries
   mat->m00 = xscale;
   mat->m11 = yscale; 
   mat->m22 = zscale;

   return mat;
}


/*  multby:=oldmax;
  multby.sub(oldmin.x,oldmin.y,oldmin.z);
  with multby do begin
    if x=0 then x:=1;
    x:=(newmax.x-newmin.x)/x;
    if y=0 then y:=1;
    y:=(newmax.y-newmin.y)/y;
    if z=0 then z:=1;
    z:=(newmax.z-newmin.z)/z;
  end;
  with mat^ do begin
    m00:=multby.x; m01:=0;        m02:=0;
    m10:=0;        m11:=multby.y; m12:=0;
    m20:=0;        m21:=0;        m22:=multby.z;
    p.x:=(-oldmin.x)*multby.x+newmin.x;
    p.y:=(-oldmin.y)*multby.y+newmin.y;
    p.z:=(-oldmin.z)*multby.z+newmin.z;
*/
matrix4x4ptr LMstretch(register matrix4x4ptr mat,vector *oldmin,vector *oldmax,vector *newmin,vector *newmax)
{
   
   vector multby;
   matrix4x4 tmpmat;
   multby.x=oldmax->x-oldmin->x;
   multby.y=oldmax->y-oldmin->y;
   multby.z=oldmax->z-oldmin->z;
   
   if (multby.x==0) multby.x=1;
   multby.x=(newmax->x-newmin->x)/multby.x;
   if (multby.y==0) multby.y=1;
   multby.y=(newmax->y-newmin->y)/multby.y;
   if (multby.z==0) multby.z=1;
   multby.z=(newmax->z-newmin->z)/multby.z;   
  
   mat->m00=multby.x; mat->m01=0;        mat->m02=0;
   mat->m10=0;        mat->m11=multby.y; mat->m12=0;
   mat->m20=0;        mat->m21=0;        mat->m22=multby.z;
   mat->p.x=(-oldmin->x)*multby.x+newmin->x;
   mat->p.y=(-oldmin->y)*multby.y+newmin->y;
   mat->p.z=(-oldmin->z)*multby.z+newmin->z;
   return mat;
};


//[1 0 0 0]
//[0 1 0 0]
//[0 0 1 0]
//[x y z 1]

matrix4x4ptr LMtranslate(register matrix4x4ptr mat,flt xtrans,flt ytrans,flt ztrans)
{
   
   mat->p.moveto(xtrans,ytrans,ztrans);
   
   mat->m01 = mat->m02 =
   mat->m10 = mat->m12 =
   mat->m20 = mat->m21 = 0;    
    //now the diagonal entries
   mat->m00 = mat->m11 = mat->m22 =1;
   return mat;
}

matrix4x4ptr LMprojectxy(matrix4x4ptr mat,vector *v,vector *c)
{
    matrix4x4 xrot;
    
    flt d=(flt)sqrt((v->y*v->y)+(v->z*v->z));
    //next you take the dot product of (0,0,v->z) and (0,v->y,v->z)
    //and divide it by v->z * d
    //the result is equivelent to v->z/d
    //this produces the cosine of the angle and the sine is v->y/d
    //This is used to rotate the axis around the y plane
    //When later rotating around the z plane, the cosine is d and the sine is -v->x
    //neato
    flt sn=v->y /d;
    flt cs=v->z /d;
    flt x=v->x;
    mat->p=*c;
    mat->m00=1;mat->m01=    mat->m02=0;
    mat->m10=0;mat->m11=cs; mat->m12=sn;
    mat->m20=0;mat->m21=-sn;mat->m22=cs;
    

    xrot.m00=d;  xrot.m01=0;  xrot.m02=x;
    xrot.m10=0;  xrot.m11=1;  xrot.m12=0;
    xrot.m20=-x; xrot.m21=0;  xrot.m22=d;
    xrot.p=0;
/*    LMconcatenatematrices(&xrot,mat,mat);

    //*mat=xrot;
    mat->m00=d;        mat->m01=0;   mat->m02=x;
    mat->m10=0;        mat->m11=cs;  mat->m12=sn*d;
    mat->m20=cs*-x;    mat->m21=-sn; mat->m22=cs*d;
    mat->p=0;//-*c;*/
  
    
    return mat;
    
}

matrix4x4ptr LMinvprojectxy(matrix4x4ptr mat,vector *v,vector *c)
{
    
    flt d=(flt)sqrt((v->y*v->y)+(v->z*v->z));
    flt denom=(v->x * v->x) +( v->y * v->y);
    flt sn= (v->y *d )/denom;
    flt cs= (v->z *d )/denom;
    matrix4x4 xrot;
    
    denom=((v->x * v->x) + (d*d));
    flt x=v->x/denom;
    d=d/denom;

    mat->p=*c;
    mat->m00=1;mat->m01=    mat->m02=0;
    mat->m10=0;mat->m11=cs; mat->m12=sn;
    mat->m20=0;mat->m21=-sn;mat->m22=cs;

    xrot.m00=d;  xrot.m01=0;  xrot.m02=x;
    xrot.m10=0;  xrot.m11=1;  xrot.m12=0;
    xrot.m20=-x; xrot.m21=0;  xrot.m22=d;
    xrot.p=0;
    LMconcatenatematrices(mat,&xrot,mat);

    return mat;
}



/*matrix4x4ptr LMaxisrotation(register matrix4x4ptr mat,float x,)
{
    
    
    xz=x*z,yz=y*z,xy=x*y,    
    
    if ((x==0)&&(y==0)&&(z==0)){
       LMidentity(mat);
       return mat;
    }

   
    mat->m00=x*x;   mat->m01=xy; mat->m02=xz;
    mat->m10=xy;    mat->m11=y*y;    mat->m12=yz;
    mat->m20=xz;    mat->m21=yz;     mat->m22=z*z;
    
    mat->p.x=mat->p.y=mat->p.z=0;
    return mat;
} */   




void calcnormal(vector p1,vector &p2,vector p3,vector & pn)
{
    //make magnitude vectors, P2 being the base}
    p3-=p2;
    p1-=p2;

    pn.x=(p1.y*p3.z) - (p1.z*p3.y);
    pn.y=(p1.z*p3.x) - (p1.x*p3.z);
    pn.z=(p1.x*p3.y) - (p1.y*p3.x);
    pn.normalize();
}

//Generalized power function by Prof. Timo Salmi *)
flt genpower (flt number, flt exponent )
{
    return (flt)exp(exponent*log(number));
/*     if (exponent == 0.0f)
       return 1.0f
     else if (number == FLT_0)
       return 0.0f
     else if (abs(exponent*ln(abs(number))) > 87.498 then
       begin writeln ('Overflow in GENPOWFN expression'); halt; end
     else if (number > FLT_0)
       
     else if ((number < FLT_0) && (frac(exponent) == FLT_0))
       if Odd(Round(exponent)) then
         genpowfn := -GENPOWFN (-number, exponent)
       else
         genpowfn :=  GENPOWFN (-number, exponent)
     else
       begin writeln ('Invalid GENPOWFN expression'); halt; end;
   end;*/
}


flt ipow(flt val,int power)
{
    //return exp((flt)power*log(val));
    //if (power==1) return 1.0f;
    UINT8 n=power<0;
    flt rval=val;
    power=ABS(power);    
    while (power>1)
    {
         rval*=val;        
         power--;
    }
    if (n)
      return FLT_1/rval;
    return rval;                
}    
    
        




flt findclosestpoint(vector &v1,vector &v2,vector &v3,vector &out)
{
    vector p1=v2-v1;
    vector p2=v3-v1;
    flt u=dot(p1,p2)/p1.magnitude();
    out=v1+(p1*u);
    return u;
    //flt u= ((v3.x-v1.x)*p2.x + (v3.y-v1.y)*p2.y + (v3.z-v1.z)*p2.z) / p2.length2();
    //out=v1+(u*v2);
    //return u;
}



flt dot(vector &vec1,vector &vec2)
{
    return  ((vec1.x*vec2.x) +
              (vec1.y*vec2.y) +
              (vec1.z*vec2.z));
}


flt dot(vector *vec1,vector *vec2)
{
    return  ((vec1->x*vec2->x) +
             (vec1->y*vec2->y) +
             (vec1->z*vec2->z));
}




/*[P3 - P1 - u(P2 - P1)] dot (P2 - P1) = 0 
Solving this gives the value of u

u = (x3-x1)*(x2-x1)+(y3-y1)*(y2-y1) / length2(p2-p1);

Substituting this into the equation of the line gives the point of intersection (x,y) of the tangent as 

x = x1 + u (x2 - x1)
y = y1 + u (y2 - y1) */



vector *cross(vector &rvec,vector &vec1,vector &vec2)
{
    rvec.x=(vec1.y*vec2.z) - (vec1.z*vec2.y);
    rvec.y=(vec1.z*vec2.x) - (vec1.x*vec2.z);
    rvec.z=(vec1.x*vec2.y) - (vec1.y*vec2.x);
    return &rvec;
}    

void reflection(vector &vec1,vector &vec2,vector &rvec)
{
    flt dp=dot(vec1,vec2);

    rvec.x=(2.0f*vec1.x*dp)-vec2.x;
    rvec.y=(2.0f*vec1.y*dp)-vec2.y;
    rvec.z=(2.0f*vec1.z*dp)-vec2.z;

/*    rvec.x=vec1.x - 2*dp*vec2.x;
    rvec.y=vec1.y - 2*dp*vec2.y;
    rvec.z=vec1.z - 2*dp*vec2.z;        */
}
    
flt distance(vector &vec1,vector &vec2)
{
    vector diff=(vec1-vec2);    
    return (flt)sqrt((diff.x*diff.x)+(diff.y*diff.y)+(diff.z*diff.z)); 
}    


tpointfloat *zrotatepoint(tpointfloat *in,tpointfloat *out,flt r)
{    
    flt cr=(flt)cos(r);
    flt sr=(flt)sin(r);
    out->x=(float)(in->x*cr + in->y*sr);
    out->y=(float)(in->x*-sr+ in->y*cr);
    return out;
}                

tpointfloat *zrotatepointaround(tpointfloat in,tpointfloat *out,flt r,tpointfloat *around)
{    
    flt cr=(flt)cos(r);
    flt sr=(flt)sin(r);
    in-=*around;
    out->x=(float)(in.x*cr + in.y*sr);
    out->y=(float)(in.x*-sr+ in.y*cr);
    *out+=*around;
    return out;
}                


