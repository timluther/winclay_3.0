#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "graphics.h"
#include "gprim.h"
#include "logger.h"
#include "strtable.h"
#include "trender.h"
#include "basic3d.h"
#include "3dfile.h"
#include "views.h"
#include "io.h"

char *parsestring(char *in,char *out)
{
	char envname[32];
	int i;
	char *outend;
	outend=out;
	while (*in==' ')
		in++;
	while (*in!=',' && *in!=';')
	{
		if (*in=='{')
		{
			in++;
			if (*in==0) return NULL;
			i=0;
			while (*in != '}')
			{
				envname[i]=*in;
				i++;
				in++;
				if (*in==0) return NULL;
			}
			envname[i]=0;
			in++;
			if (*in==0) return NULL;
			//for now, brute force search. Environment variables will be 
			//sorted in the future, allowing for a more elegant seach.
			for (i=0;i<envcount;i++)
			{
				if (!strcmp(envname,envstrings[i].envname))
				{
					strcat(out,envstrings[i].data);
					outend+=strlen(envstrings[i].data);
					//messy way to break
					i=envcount;
				}
			}
			
		}
		if ((*in != ';')&&(*in != ','))
		{
			if (*in != ' ')
			{
				*outend=*in;
				outend++;
			}		
			in++;
		}
		if (*in==0) return NULL;
	}
	in++;
	if (*in==0) return NULL;
	if (*outend!='\\')
	{
		*outend='\\';
		outend++;
	}
	*outend=0;	
	return in;
}

void alterslashes(char *name)
{
	while (*name)
	{
		if (*name=='/')
			*name='\\';
		name++;
	}
	
}

tsurface *searchreadimage(trendercontext *ctx,char *name,UINT32 pfd)
{
	static char cachedpath[256]="";
	static char fullname[256];
	static char pathname[256];
	char *cenv=T3D_TEXTURE_PATHS;	
	char *filename;
	bool exists=false;
	tsurface *tex=NULL;  
	
	strcpy(fullname,name);			
	alterslashes(fullname);
	if (!fexist(fullname))
	{
		filename=getname(name);
		if (cachedpath[0]!=0)
		{
			sprintf(fullname,"%s%s",cachedpath,filename);		
			alterslashes(fullname);
			if (fexist(fullname))							
				exists=true;							
		}		
		while ((!exists)&&(cenv=parsestring(cenv,pathname)))
		{
			
			sprintf(fullname,"%s%s",pathname,filename);		
			alterslashes(fullname);
			if (fexist(fullname))
			{
				getpathanddrive(pathname,cachedpath);
				exists=true;				
			}
		}		          		
	}else
		exists=true;
	if (exists)
	{
		tex=readimage(TWINrendercontext,fullname,PXFrgb565,SFtexture);
		T3Daddtexture(tex,filename);            
	}
	return tex;
}


UINT32 gettricount(t3dmesh *obj)
{
    UINT32 count=0;
    INT32 vcount;
    tpolygon *p=obj->getpolygonhead();
    while (p!=NULL)
    {
        vcount=p->npoints-2;
        if (vcount>0)
			count+=vcount;
        p=p->nxt();
		
    }
    return count;
}

class polyblock
{
public:
	int start;
	material *mat;
	textureref *tex;
};


typedef struct MD3Header_
{
	char ID[4];         //id of file, always "IDP3" 
	int  Version;       //i suspect this is a version number always 15 	
	char FileName[68];  //sometimes left Blank...65 chars, 32bit aligned == 68 chars                     
	int  BoneFrame_num; //number of BoneFrames
	int  Tag_num;       //number of 'tags' per BoneFrame
	int  Mesh_num;      //number of meshes/skins 
	int  MaxSkin_num;   //maximum number of unique skins used in md3 file 						
	int  HeaderLength;  //always equal to the length of this header 
	int  Tag_Start;     //starting position of tag-structures                         
	int  Tag_End;       //ending position of tag-structures/starting position of mesh-structures 
	int  FileSize;      //size of file     
}MD3Header;

typedef struct MD3Tag_
{
	char Name[64];    //name of 'tag' as it's usually called in the md3 files try to see it as a sub-mesh/seperate mesh-part. 				                               
	vector p;   //the direction the tag is facing relative to the rest of the model
	matrix3x3 o;   //3x3 matrix, the orientation
}MD3Tag;


typedef struct MD3BoneFrame_
{
	float  Mins[3];
	float  Maxs[3];
	vector p;
	float  scale;
	char   Creator[16]; 
}MD3BoneFrame;

typedef struct MD3Mesh_
{
	char ID[4];          //id, must be IDP3 
	char Name[68];       //name of mesh 
	
	int  MeshFrame_num;  //number of meshframes 
	//in mesh 
	int  Skin_num;       //number of skins in mesh 
	int  Vertex_num;     //number of vertices 
	int  Triangle_num;   //number of Triangles 
	int  Triangle_Start; //starting position of relative to start of mesh header
	
	int  HeaderSize;     //size of header 
	int  TexVec_Start;   //starting position of texvector data, relative to start of Mesh_Header 
	int  Vertex_Start;   //starting position of  vertex data,relative to start of Mesh_Header 
	int  MeshSize;       //size of mesh 
}MD3Mesh;

typedef struct MD3Skin_
{
	char Name[68];
}MD3Skin;

typedef struct MD3Triangle_
{
	int p[3];
}MD3Triangle;

typedef struct MD3uv_
{
	float u,v;
}MD3uv;

typedef struct MD3vertex_
{
	INT16 x,y,z;
	UINT8 env_u,env_v;
}MD3vertex;




bool T3Dloadmd3(char *filename,tscene *scene)
{
	MD3Header	 hdr;
	MD3Tag		 tag;
	MD3BoneFrame boneframe;
	MD3Mesh		 mesh;
	MD3vertex    vertex;
	MD3Triangle  triangle;
	MD3uv		 *uvlist;
	MD3Skin      skin;
	t3dmesh		 *tmesh;	
	float		 scale=0.1f;
	FILE *f;
	int i,j;
    f=fopen(filename,"rb");
    if (!f)
    {        //set ERR_LEVEL
        return NULL;
    }    
	
	getpathanddrive(filename,CURRENT_MODEL_PATH);
    fread(&hdr,sizeof(hdr),1,f);    
	
	if (hdr.Tag_End!=hdr.Tag_Start) 
	{
		int count=hdr.Tag_num * hdr.BoneFrame_num;
		for (i=0;i<count;i++)
		{
			fread(&tag,sizeof(tag),1,f);    
		}
	}
	for (i=0;i<hdr.BoneFrame_num;i++)
	{
		fread(&boneframe,sizeof(boneframe),1,f);    
	}
	for (i=0;i<hdr.Mesh_num;i++)
	{
		int oldpos=ftell(f);
		fread(&mesh,sizeof(mesh),1,f);    
		for (j=0;j<mesh.Skin_num;j++)
		{
			fread(&skin,sizeof(skin),1,f);    
		}
		char *ext=getextension(skin.Name);
		if (ext)
		{	
			tsurface *tex=searchreadimage(TWINrendercontext,skin.Name,PXFrgb565);									
			T3Dsettexture(T3Daddtexture(tex,skin.Name));			
			
		}		
		
		fseek(f,oldpos+mesh.Vertex_Start,SEEK_SET);
		tmesh=new t3dmesh();
		
		for (j=0;j<mesh.Vertex_num;j++)
		{
			fread(&vertex,sizeof(vertex),1,f);    
			tmesh->addvertex(vertex.x*scale,vertex.y*scale,vertex.z*scale);
		}
		
		uvlist= new MD3uv[mesh.Vertex_num];
		
		fseek(f,oldpos+mesh.TexVec_Start,SEEK_SET);
		fread(uvlist,sizeof(MD3uv)*mesh.Vertex_num,1,f);    		
		
		fseek(f,oldpos+mesh.Triangle_Start,SEEK_SET);
		for (j=0;j<mesh.Triangle_num;j++)
		{
			fread(&triangle,sizeof(triangle),1,f);    
			tpolygon *tpoly=tmesh->addpoly(3);          
			
            T3Dsetpolygonvertex(tpoly,tmesh->getvertex(triangle.p[0]),0,0);               
			T3Dsetpolygonvertexuv(tpoly,0,uvlist[triangle.p[0]].u,uvlist[triangle.p[0]].v);
			T3Dsetpolygonvertex(tpoly,tmesh->getvertex(triangle.p[1]),1,0);               
			T3Dsetpolygonvertexuv(tpoly,1,uvlist[triangle.p[1]].u,uvlist[triangle.p[1]].v);
			T3Dsetpolygonvertex(tpoly,tmesh->getvertex(triangle.p[2]),2,0);               
			T3Dsetpolygonvertexuv(tpoly,2,uvlist[triangle.p[2]].u,uvlist[triangle.p[2]].v);
			tmesh->addvertex(vertex.x*scale,vertex.y*scale,vertex.z*scale);
		}		
		
		delete [] uvlist;
		scene->addobject(NULL,tmesh);
		//get skins				
		
		
		
		fseek(f,oldpos+mesh.MeshSize,SEEK_SET);
		tmesh->buildedges();         
		tmesh->calculatenormals();
		
	}
	fclose(f);
	return true;
	
	
}


bool T3Dsavexeios(char *filename,tscene *scene)
{
	polyblock meshblocks[255];
	material *cmat;
	int meshblockcount;
	FILE *f=fopen(filename,"wa");
    if (!f)
    {        //set ERR_LEVEL
        return NULL;
    }
	UINT32 i=0;
    t3dmesh *obj=(t3dmesh*)scene->scenegraph.root;    
	fprintf(f,"XGAv1.00\n");
	int ocount=0;
	while (obj!=NULL)
	{
		if (obj->type==otGEOM)
		{
			fprintf(f,"xgDagTransform %s_%i;\n",obj->name,ocount);
			fprintf(f,"xgBgMatrix %s_%i$matrix;\n",obj->name,ocount);
			fprintf(f,"xgDagMesh %s_%i$mesh0;\n",obj->name,ocount);
			fprintf(f,"xgBgGeometry %s_%i$geom;\n\n",obj->name,ocount);
			ocount++;
		}
		obj=(t3dmesh*)obj->next;
	}

	fprintf(f,"\n");
	textureref *tex=(textureref*)T3Dtexturebank.first;
	
	while (tex)
	{
		fprintf(f,"xgTexture %s;\n",tex->name);
		tex=(textureref*)tex->next;
	}
	fprintf(f,"\n");
	
	material *mat=(material*)T3Dmaterialbank.first;
	while (mat)
	{
		fprintf(f,"xgMaterial scene$mat%i;\n",i);
		i++;
		mat=(material*)mat->next;
	}
	fprintf(f,"\n");

	tex=(textureref*)T3Dtexturebank.first;	
	while (tex)
	{
		fprintf(f,"xgTexture %s\n{",tex->name);
		fprintf(f,"    url= \"%s\";\n",tex->name);
		fprintf(f,"    mipmap_depth= 127;\n",tex->name);
		fprintf(f,"}\n");
		tex=(textureref*)tex->next;
	}
	fprintf(f,"\n");
	
	mat=(material*)T3Dmaterialbank.first;
	i=0;
	while (mat)
	{
		fprintf(f,"xgMaterial scene$mat%i\n",i);
		fprintf(f,"{\n",i);
		fprintf(f,"    shadingType=1;\n",i);
		fprintf(f,"    diffuse=%f %f %f %f ;\n",mat->diffuse.r,mat->diffuse.g,mat->diffuse.b,1.0f);
		fprintf(f,"    specular=%f %f %f %f ;\n",mat->specular.r,mat->specular.g,mat->specular.b,mat->st);
		fprintf(f,"}\n");
		i++;
		mat=(material*)mat->next;
	}

	
		
	obj=(t3dmesh*)scene->scenegraph.root;    
	i=0;
	ocount=0;
	while (obj!=NULL)
	{
		if (obj->type==otGEOM)
		{	
			
			//transform node
			fprintf(f,"xgDagTransform %s_%i\n{\n",obj->name,ocount);
			fprintf(f,"    inputMatrix = %s_%i$matrix.outputMatrix;",obj->name,ocount);			
			fprintf(f,"\n}\n\n");
			
			//matrix node
			fprintf(f,"xgBgMatrix %s_%i$matrix\n{\n",obj->name,ocount);
			fprintf(f,"    position=%f %f %f;\n",obj->p.x,obj->p.y,obj->p.z);
			fprintf(f,"    rotation=%f %f %f 1.0f;\n",0.0f,0.0f,0.0f);//obj->o.x,obj->o.y,obj->o.z
			fprintf(f,"    scale=%f %f %f;\n",1.0f,1.0f,1.0f);
			fprintf(f,"\n}\n\n");
			
			//geometery node
			
			
			fprintf(f,"xgBgGeometry %s_%i$geom\n{\n",obj->name,ocount);
			fprintf(f,"    vertices=\n");
			fprintf(f,"       %i %i\n",11,obj->vertexlist.count);	
			
			tpolygon *p=obj->getpolygonhead();
			vertex *v=obj->getvertexhead();
			while (p)
			{
				tpolyvertex *v,*end;        
				for (end=&(v=p->vlist)[p->npoints];v<end;v++)
					v->v->point2d=v->texv;
				p=(tpolygon*)p->next;
			}
			i=0;
			while (v)
			{
				fprintf(f,"      %f %f %f 1.0 %f %f %f %f %f\n",v->data3d.x,v->data3d.y,v->data3d.z,v->normal.x,v->normal.y,v->normal.z,v->point2d.x,v->point2d.y);
				v->tag=i;
				i++;
				v=(vertex*)v->next;
			}
			fprintf(f,"      ;\n");
			fprintf(f,"}\n\n");
			
			
			//mesh node
			
			T3Dresetbuffers();
			obj->dumppolygons();
			
			T3Dsortbymaterial(0,T3Dpolycount-1);
			
			cmat=NULL;
			tex=NULL;
			meshblockcount=0;
			
			for (i=0;i<T3Dpolycount;i++)
			{
				if ((cmat!=T3Dvisiblepolygons[i]->vlist[0].mat)||(tex!=T3Dvisiblepolygons[i]->tex))
				{					
					cmat=T3Dvisiblepolygons[i]->vlist[0].mat;
					tex=T3Dvisiblepolygons[i]->tex;
					meshblocks[meshblockcount].start=i;
					meshblocks[meshblockcount].mat=cmat;
					meshblocks[meshblockcount].tex=tex;
					meshblockcount++;					
				}
			}
			meshblocks[meshblockcount].start=i;
			obj->tempdata=(void*)meshblockcount;
			
			
			for (int mc=0;mc<meshblockcount;mc++)
			{
				polyblock *cb=&meshblocks[mc];
				polyblock *nb=&meshblocks[mc+1];
				
				UINT32 ecount=0;
				for (i=cb->start;i<nb->start;i++)
				{
					ecount+=T3Dvisiblepolygons[i]->npoints+1;
				}				
				
				fprintf(f,"xgDagMesh %s_%i$mesh%i\n{\n",obj->name,ocount,mc);
				fprintf(f,"    primType  = 0;\n");
				fprintf(f,"    primCount = %i;\n",nb->start-cb->start);
				fprintf(f,"    primData = \n");
				
				fprintf (f,"    %i\n",ecount);		
				for (int ib=cb->start;ib<nb->start;ib++)
				{
					p=T3Dvisiblepolygons[ib];
					fprintf (f,"    %i ",p->npoints);
					for (i=0;i<p->npoints;i++)
						fprintf (f,"%i ",p->vlist[i].v->tag);
					fprintf(f,"\n");
					
				}
				fprintf(f,";\n\n");
				fprintf(f,"    inputGeometry = %s_%i$geom.outputGeometry;\n",obj->name,ocount);
				fprintf(f,"    inputMaterial = scene$mat%i.outputMaterial;\n",T3Dgetmaterialindex(cb->mat));
				fprintf(f,"}\n\n");
			}
			ocount++;
		}
		obj=(t3dmesh*)obj->next;		
	}
	
	obj=(t3dmesh*)scene->scenegraph.root;    
	
	fprintf(f,"dag\n{\n");
	ocount=0;
	while (obj!=NULL)
	{
		if (obj->type==otGEOM)
		{	
			fprintf(f,"    %s_%i\n",obj->name,ocount);
			fprintf(f,"    [\n");
			int co=(int)obj->tempdata;
			for (i=0;i<co;i++)
				fprintf(f,"	       %s_%i$mesh%i\n",obj->name,ocount,i);
			fprintf(f,"    ]\n");
			ocount++;
		}
		obj=(t3dmesh*)obj->next;
		
	}
	fprintf(f,"}\n");
	fclose(f);
    return TRUE;
	
}


bool T3Dsavejava(char *filename,tscene *scene)
{
    //UINT16 minorv=0,majorv=1;
    //UINT32 blockpos,endpos,codeword,blocksize;   
    material *oldmat;
    UINT32 oldattribute;
    //ttexture *oldtex;
    vertex *v;
    tpolygon *p;
    tpolyvertex *pv,*end,*pvr;
    UINT32 counter;
    UINT32 tricount;
	
    FILE *f=fopen(filename,"wa");
    if (!f)
    {        //set ERR_LEVEL
        return NULL;
    }
    t3dmesh *obj=(t3dmesh*)scene->scenegraph.root;
    
    while (obj!=NULL)
    {
        if (obj->type==otGEOM)
        {
			tricount=0;
			counter=0;
			v=(vertex*)obj->vertexlist.first;
			p=(tpolygon*)obj->polygonlist.first;
			fprintf(f,"class %s extends t3dmesh\n",obj->name);
			fprintf(f,"%s()\n{\n",obj->name);
			fprintf(f,"    super(%i,%i);\n",obj->vertexlist.count,gettricount(obj));\
				while (v!=NULL)
				{
					//should and this with a mask;
					fprintf(f,"    addvertex(%ff,%ff,%ff);\n",v->data3d.x,v->data3d.y,v->data3d.z);
					//T3Dwritevector3f(f,&v->data3d);
					v->writetag(counter++);
					v=v->nxt();        
				}
				oldmat=NULL;
				//oldtex=NULL;
				oldattribute=0xFFFFFFFF;
				while (p!=NULL)
				{
					UINT32 npoints=p->npoints;            
					if (p->attribute !=oldattribute)
					{
						fprintf(f,"    t3dmesh.cattribute=0");
						if (p->attribute & paDOUBLESIDED)
							fprintf(f,"|ttriangle.TAdoublesided");
						if (p->attribute & paSHADED)
							fprintf(f,"|ttriangle.TAsmooth");
						fprintf(f,";\n");
					}
					/*if (p->mat!=oldmat)
					{
					if (p->mat->diffuse!=oldmat->diffuse)                
					fprintf(f,"    t3dmesh.cred=%ff;t3dmesh.cgreen=%ff;t3dmesh.cblue=%ff;\n",p->mat->diffuse.r,p->mat->diffuse.g,p->mat->diffuse.b);
					if (p->mat->emmisive!=oldmat->emmisive)                
					fprintf(f,"    t3dmesh.cered=%ff;t3dmesh.cegreen=%ff;t3dmesh.ceblue=%ff;\n",p->mat->emmisive.r,p->mat->emmisive.g,p->mat->emmisive.b);
				}*/
					
				/*id=0;
				if (count>3)
				{
				for (;id<(count >> 1)-1;id++)
				{                                               
				trianglebuffer[polycount++]=
				addtemptriangle(pclipv[id],pclipv[id+1],pclipv[count-id-1],ct);
				trianglebuffer[polycount++]=
				addtemptriangle(pclipv[id+1],pclipv[count-id-2],pclipv[count-id-1],ct);                                                                 
				}                                                               
				}                                                               
				if ((count & 1)==1) 
					trianglebuffer[polycount++]=addtemptriangle(pclipv[id],pclipv[id+1],pclipv[id+2],ct); */
					pv=p->vlist;
					end=&pv[(npoints >>1)-1];
					pvr=&pv[npoints-1];
					if (npoints>3)
						for (;pv<end;pv++,pvr--)
						{                   
							fprintf(f,"    addquad(%i,%i,%i,%i);\n",pv->v->readtag(),(pv+1)->v->readtag(),(pvr-1)->v->readtag(),pvr->v->readtag());
							tricount+=2;
						}
						if (npoints & 1)
						{
							fprintf(f,"    addtriangle(%i,%i,%i);\n",pv->v->readtag(),(pv+1)->v->readtag(),pvr->v->readtag());
							tricount++;
						}
						//oldmat=p->mat;
						oldattribute=p->attribute;
						p=p->nxt();
				}
				
				fprintf(f,"    }//tricount=%i\n",tricount);
				fprintf(f,"}\n");
        }
        obj=(t3dmesh*)obj->next;
    }
    fclose(f);
    return TRUE;
}


/*B_flags: 

  bit 0 : tiled flag 
  
	0 is not tiled 
	
	  1 is tiled
	  
		bit 1 : animated flag 
		
		  0 = static texture 
		  
			1 = animated texture
			*/
			
			char *getpossiblelabel(char *txt,FILE *f)
			{
				fscanf(f,"%s",txt);
				char ch[2]="\0";
                
				if (txt[0]=='{')   //surface has a label
				{        
					if (!strcontains(txt,'}'))
						while (ch[0]!='}')
						{
							ch[0]=fgetc(f);
							//strcat(txt,ch);          
						}
						return txt;
				}
				else return NULL;               
			}
			
			
			
#define MRTEXTURE_TILED 0x1
#define MRTEXTURE_ANIM 0x2
#define MRTEXTURE_TRUECOL 0x1
			
#define MRPOLY_SHADED 0x1
#define MRPOLY_TINTED 0x2
#define MRPOLY_WIREFRAME 0x4
#define MRPOLY_DOUBLESIDED 0x8
#define MRPOLY_RESERVED 0x10
#define MRPOLY_TEXTUREBACKGROUND 0x20 //transparent or seethrough
#define MRPOLY_TEXTURETYPE 0x40 //transparent or seethrough
#define MRPOLY_INVISIBLE 0x80 //visible or not
#define MRPOLY_NOTSELECTABLE 0x100 
#define MRPOLY_TINTTYPE 0x200 //hash or not (redundant)
#define MRPOLY_TINTPHASE 0x400 //tintphase(redundant)
#define MRPOLY_ENVMAP 0x800  //env mapping
#define MRPOLY_NOPHONG 0x1000 //Phong
#define MRPOLY_TRUECOLOUR 0x2000 //true colour
			
			bool T3Dloadmr(char *filename,tscene *scene)
			{    
				UINT8 *textureflagsa=NULL;
				UINT8 ctexflaga=0;        
				textureref **texturetable=NULL;
				char T3Dtempstr[256];
				int polyrefs[256];
				tpointflt tvecs[64];
				
				float tfloat;
				float tvecx,tvecy,tvecz;
				tpolygon *tpoly;
				trgba tcol;
				trgbfloat tcolfloat;    
				int maxbsp,maxtextures,dummy,flags,maxvecs,maxpolys,maxsurfs,maxpolypoints,i,i2,i3,hastexture;    
				UINT32 polyflags;
				UINT32 oldpolyflags=T3Ddefaultpolyattributes;
				FILE *f;
				getpathanddrive(filename,CURRENT_MODEL_PATH);
				//logmessage( (void*)&textcol,"inside loadmr");
				f=fopen(filename,"rb");
				
				if (!f)
				{        //set ERR_LEVEL
					return NULL;
				}            
				fscanf(f,"%s",T3Dtempstr);
				if (!strcmp(T3Dtempstr,"#MINDRENDER"))
				{    
					fscanf(f,"%f",&tfloat);
					fscanf(f,"%i",&maxbsp);
				}else
				{
					return false;
					//sscanf(T3Dtempstr,"%i",&maxbsp);
				}          
				if (maxbsp<0) return false;    
				for (i=0;i<maxbsp;i++)
				{
					fscanf(f,"%s",T3Dtempstr);                
					fscanf(f,"%s",T3Dtempstr);                               
				}    
				fscanf(f,"%i",&maxtextures);
				if (maxtextures<0) return NULL;
				
				//logmessage( (void*)&textcol,"getting textures");
				
				if (maxtextures>0)
				{        
					texturetable=new textureref*[maxtextures+4];
					textureflagsa=new UINT8[maxtextures+4];
					for (i=0;i<maxtextures;i++)
					{
						fscanf(f,"%s",T3Dtempstr);
						strcat(T3Dtempstr,".bmp");
						tsurface *tex=NULL;              
						tex=searchreadimage(TWINrendercontext,T3Dtempstr,PXFrgb565);                       
						texturetable[i]=T3Daddtexture(tex,T3Dtempstr);         
						
						//load texture
						fscanf(f,"%i",&textureflagsa[i]);
						
						fscanf(f,"%i",&dummy);
						fscanf(f,"%i",&flags);
						
						
						fscanf(f,"%i",&dummy);fscanf(f,"%i",&dummy);
						
						if (flags & MRTEXTURE_ANIM)
						{
							fscanf(f,"%i",&dummy);fscanf(f,"%i",&dummy);
							fscanf(f,"%i",&dummy);fscanf(f,"%i",&dummy);
						}                          
						
					}        
				}    
				//sprintf(buf,"maxtextures: %i",T3Dtexturebank.count);
				//logmessage( (void*)&textcol,buf);
				
				
				//5.0 volts 1 amp...what kind of random comment is this??
				
				fscanf(f,"%i",&dummy);
				fscanf(f,"%i",&dummy);
				fscanf(f,"%i",&dummy);
				fscanf(f,"%i",&dummy);
				fscanf(f,"%i",&maxvecs);
				
				//logmessage( (void*)&textcol,"allocating object");
				
				t3dmesh *newobj=new t3dmesh();
				if (maxvecs<0) return NULL;
				if (maxvecs>0)
					for (i=0;i<maxvecs;i++)
					{        
						fscanf(f,"%s",&T3Dtempstr);
						if (T3Dtempstr[0]=='i')  //previous vertex has intensity value
						{            
							fscanf(f,"%f",&tfloat);
							fscanf(f,"%f",&tvecx);
						}           
						else
							sscanf(T3Dtempstr,"%f",&tvecx);                     
						
						fscanf(f,"%f",&tvecy);
						fscanf(f,"%f",&tvecz);
						newobj->addvertex(tvecx,-tvecy,tvecz);
					}
					fscanf(f,"%i",&maxsurfs);
					if (maxsurfs<0) return NULL;  
					sprintf(buf,"No. surfaces: %i",maxsurfs);
					logmessage( (void*)&textcol,buf);
					
					
					
					for (i2=0;i2<maxsurfs;i2++)
					{
						//check for label (if any) here        
						if (getpossiblelabel(T3Dtempstr,f))
						{
							logmessage(  (void*)&textcol,T3Dtempstr);
							
							fscanf(f,"%i",&dummy);
						}else
							sscanf(T3Dtempstr,"%i",&dummy); //node atachment        
						fscanf(f,"%i",&dummy); //phong
						fscanf(f,"%i",&dummy); //render type
						fscanf(f,"%i",&maxpolys);
						if (maxpolys<0) return NULL;  
						sprintf(buf,"No. polygons for surface %i:%i",i2,maxpolys);
						logmessage( (void*)&textcol,buf);
						
						for (i=0;i<maxpolys;i++)
						{                        
							T3Ddefaultpolyattributes=paENABLED;            
							if (getpossiblelabel(T3Dtempstr,f))
							{
								logmessage( (void*)&textcol,T3Dtempstr);                
								
								fscanf(f,"%i",&hastexture);                                
							}else
								sscanf(T3Dtempstr,"%i",&hastexture);
							
							if (hastexture>0)
							{
								if (maxtextures)
								{
									T3Dsettexture(texturetable[hastexture-1]);
									ctexflaga=textureflagsa[hastexture-1];                                      
								}
								fscanf(f,"%i",&dummy);  //don't know what this first param is yet
							}else
							{
								T3Dsettexture(NULL);
								ctexflaga=0;
							}
							fscanf(f,"%u",&polyflags);  //polygon flags
							
							if(polyflags&0x400) T3Ddefaultpolyattributes|=paENVMAP;
							if(polyflags&MRPOLY_TINTED) T3Ddefaultpolyattributes|=paTRANSPARENT;
							if(polyflags&MRPOLY_SHADED) T3Ddefaultpolyattributes|=paSHADED;
							if(polyflags&MRPOLY_TRUECOLOUR) T3Ddefaultpolyattributes|=paTRUECOLOUR;
							if(polyflags&MRPOLY_DOUBLESIDED) T3Ddefaultpolyattributes|=paDOUBLESIDED;
							
							if(ctexflaga&MRTEXTURE_TRUECOL) T3Ddefaultpolyattributes|=paTEXTURETRUE;
							
							
							fscanf(f,"%i %i %i ",&tcol.r,&tcol.g,&tcol.b);  //colour
							tcolfloat.r=tcol.r/255.0f;
							tcolfloat.g=tcol.g/255.0f;
							tcolfloat.b=tcol.b/255.0f;            
							T3Dmaterial=T3Daddmaterial(tcolfloat,8,((T3Ddefaultpolyattributes&paTRANSPARENT)!=0)?0.5f:FLT_0);
							T3Dmaterial->st=FLT_1;
							
							fscanf(f,"%i",&maxpolypoints);     //max polygon points
							if (maxpolypoints<0) return NULL;  
							tpoly=newobj->addpoly(maxpolypoints);            
							for (i3=0;i3<maxpolypoints;i3++)  //read in vertex references            
							{
								fscanf(f,"%i",&polyrefs[i3]);                  
							}
							if (hastexture>0)                 //read in texture coords
								for (i3=0;i3<maxpolypoints;i3++)
								{
									tpointfloat *tv=&tvecs[maxpolypoints-i3-1];
									fscanf(f,"%f %f",&tv->x,&tv->y);
									if (T3Dtexture)
									{
										tv->x/=T3Dtexture->t->size.x;
										tv->y/=T3Dtexture->t->size.y;
									}
									BOUND(tv->x,FLT_0,FLT_1);
									BOUND(tv->y,FLT_0,FLT_1);
								}
								
								for (i3=0;i3<maxpolypoints;i3++)
								{				
									T3Dsetpolygonvertex(tpoly,newobj->getvertex(polyrefs[i3]),maxpolypoints-i3-1,0);               
								}
								tpoly->setuv(tvecs);                            
								
						}        
					}
					newobj->buildedges();         
					newobj->calculatenormals();
					scene->addobject(NULL,newobj);
					if (maxtextures>0)
					{
						free(texturetable);
						free(textureflagsa);
					}
					//logmessage( (void*)&textcol, (void*)&textcol,"done");
					fclose(f);    
					T3Ddefaultpolyattributes=oldpolyflags;
					return newobj!=0;    
}


UINT32 intel_mot(UINT32 a)
{	
	_asm 
	{ 
		mov eax,a;
		bswap eax;
		mov a,eax;
	};
	return a;
}

float intel_motflt(float *a)
{	
	_asm 
	{ 
		mov ebx,a
			mov eax,[ebx];
		bswap eax;
		mov [ebx],eax;
	};
	return *a;
}

UINT16 intel_mot16(UINT16 a)
{	
	_asm 
	{ 
		mov ax,a;
		ror ax,8;
		mov a,ax;
	};
	return a;
}


#define PASREALSIZE 6
#define CLAYPOLYSIZE 11
#define CLAYJOINSIZE 4

#pragma pack(1)
typedef struct _pasreal
{
	UINT8 d[6];
}pasreal;
#pragma pack()
/*
typedef _Packed struct pasreal
{
UINT16 t[3];
}pasreal;
*/

#pragma pack(1)
typedef struct _claypoly
{
	UINT8 numsides;
	UINT16 p[4];
	UINT8 colour;
	UINT8 attrib;
}claypoly;
#pragma pack()

//[4]

#pragma pack(1)
typedef struct _clayjoin
{
    UINT16 f,t;
}clayjoin;
#pragma pack()
/*
extern void fldz();
#pragma aux fldz=\
"fldz"

  extern void fld(long double *f);
  #pragma aux fld=\
  "fld [edi]"\
  parm[edi]
  
	
	  //#pragma warn -rvl
	  
		double real2double(UINT16 r[3])
		{
		union {
		UINT16 w[5];
		long double e;
		} ld;
		//fldz();
		if ( (r[0]&0xff)==0 )
		return FLT_0;
		
		  ld.w[4] = (r[0]&0x00ff)+(r[2]&0x8000)+0x3f7e;
		  ld.w[3] = r[2]|0x8000;
		  ld.w[2] = r[1];
		  ld.w[1] = r[0]&0xff00;
		  ld.w[0] = 0;
		  fld(&ld.e);   
		  return ld.e;
		  }
		  
			//<iretd> if 0 < e <= 255, then v = (-1)^s * 2^(e-129) * (1.f)
			double real2double(pasreal &r)
			{
			//UINT32 frac=intel_mot(r.frac);
			//power(-1,r.sign & 0x1)*power(2,r.exp-129)*
			if (r.sign &0x1)
			return pow(2,r.exp-129)*(FLT_1+(flt)r.frac/0xFFFFFFFF);
			return -(pow(2,r.exp-129)*(FLT_1+(flt)r.frac/0xFFFFFFFF));
			
			  }
			  
				
				  /*
				  
					Below is some code cut out of the TD log, if your C compiler supports in-line
					code, it might be useful...
					
					  Input of the round and trunc functions is a 6-byte real in dx:bx:ax.
					  (s-mant-exp), the return a longint in dx:ax.
					  
						FYI a Borland 6 byte real looks like this, and this info is in the manual..
						
						  5  4  3  2  1  0    {byte order, exponet is lowest byte!}
						  1     39      8    {width in bits}
						  +-+------------+---+
						  |s|    f       | e | (f has an implied msb of 1)
						  +-+------------+---+
						  msb   lsb m   l
						  
							The value n of the number is determined by
							
							  - if e = 0, then n = 0
							  - if 0 < e <= 255, then n = (-1)^s * 2^(e - 129) * (1.f)
*/
float real2float(pasreal r)
{
	union {
		UINT32 clearme;
		UINT8 m[4];
		float f;
	} ld;
	if (r.d[0]==0) return FLT_0;
	ld.clearme=0;
	r.d[0]-=2;
	ld.m[1] = r.d[4];
	ld.m[0] = r.d[3];
	ld.m[2] = r.d[5]&0x7F;
	if ((r.d[0] & 1)!=0)
		ld.m[2] |= 0x80;   
	
	ld.m[3] = (r.d[0]&0xFE) >> 1;
	ld.m[3] |= (r.d[5]&0x80);
	return ld.f;
}




/*char T3Dsignature[4]={'C','W','3','D'};*/
//UINT32 T3Dsignature=0x43573344;

UINT32 T3Dsignature=0x44335743;


//just used for file
#define otSCENE 0x80


void T3Dwritefloat(FILE *f,float fltval)
{
    static float fval;
    fval=(float)fltval;
    fwrite(&fval,4,1,f);
}

inline UINT32 T3DwriteUINT8(FILE *f,UINT8 *n)
{
    fwrite(n,1,1,f);
    return 1;    
}

inline UINT32 T3DwriteUINT32(FILE *f,UINT32 *n)
{
    fwrite(n,4,1,f);
    return 4;
}

inline UINT32 T3DwriteUINT16(FILE *f,UINT16 *n)
{
    fwrite(n,4,1,f);
    return 4;
}


UINT32 T3Dwritecolour3f(FILE *f,trgbfloat *col)
{
    T3Dwritefloat(f,col->r);
    T3Dwritefloat(f,col->g);
    T3Dwritefloat(f,col->b);
    return 12;
}

inline bool T3Dwritetexturebank(FILE *f)
{
	textureref *ref;
	UINT32 temp=otTEXTUREBANK;
	UINT32 texturecount=T3Dtexturebank.count;
	T3DwriteUINT32(f,&temp);
	temp=8+(texturecount * 32);
	T3DwriteUINT32(f,&temp);
	T3DwriteUINT32(f,&texturecount);
	ref=(textureref*)T3Dtexturebank.first;
	
	while (ref)
	{
		fwrite(ref->name,32,1,f);		
		ref=(textureref*)ref->next;
	}
	return true;
}

inline UINT32 T3Dwritematerialbank(FILE *f)
{
    material *cmat;
    UINT32 temp=otMATERIALBANK;
    UINT32 materialcount=T3Dmaterialbank.count;
    T3DwriteUINT32(f,&temp);
    temp=8+(materialcount * 57);
    T3DwriteUINT32(f,&temp);
    T3DwriteUINT32(f,&materialcount);
    cmat=(material*)T3Dmaterialbank.first;
	
    while (cmat)
    {
        T3Dwritecolour3f(f,&cmat->ambient);
        T3Dwritecolour3f(f,&cmat->diffuse);
        T3Dwritecolour3f(f,&cmat->specular);
        T3Dwritecolour3f(f,&cmat->emmisive);        
        T3Dwritefloat(f,cmat->st);
        T3Dwritefloat(f,cmat->transparency);
        UINT32 specp=cmat->specpower;
        T3DwriteUINT32(f,&specp);
        cmat=(material*)cmat->next;
    }
    return TRUE;
}



UINT32 T3Dwritevector3f(FILE *f,vector *v)
{
    T3Dwritefloat(f,v->x);
    T3Dwritefloat(f,v->y);
    T3Dwritefloat(f,v->z);
    return 12;
}

#define T3Dcolour3fsize (12)
#define T3Dbasic3dobjectsize (32+16)
#define T3Dlightsize (T3Dbasic3dobjectsize+1+(T3Dcolour3fsize*3)+4+4)

/*
trgbfloat ambient,diffuse,specular,emmisive;
UINT8 specpower;
flt st; //shine strength.. replacing the '2.0f' of r'=(2.0f*n'*dp)-o' maybe incorrect
flt transparency;
*/

UINT32 T3Dwritebasic3dobject(FILE *f,tobject3d *obj)
{
    fwrite(obj->name,32,1,f);
    T3DwriteUINT32(f,&obj->attribute);
    T3Dwritevector3f(f,&obj->p);
    T3Dwritevector3f(f,&obj->o);
    T3Dwritevector3f(f,&obj->s);
    return TRUE;
}


inline UINT32 T3Dwritemesh(FILE *f,t3dmesh *mesh)
{        
    T3Dwritebasic3dobject(f,(tobject3d *)mesh);
    UINT32 vcount=mesh->vertexlist.count;
    UINT32 pcount=mesh->polygonlist.count;
	
    vertex *v=(vertex *)mesh->vertexlist.first;
    tpolygon *p=(tpolygon *)mesh->polygonlist.first;
    
    UINT32 counter=0;
	
    T3DwriteUINT32(f,&vcount);
    while (v!=NULL)
    {
        //should and this with a mask;
        T3DwriteUINT32(f,&v->attribute);
        T3Dwritevector3f(f,&v->data3d);
        v->writetag(counter++);
        v=v->nxt();        
    }
    counter=0;
    fwrite(&pcount,4,1,f);
    while (p!=NULL)
    {
        UINT32 npoints=p->npoints;
        UINT32 vref;
		
        T3DwriteUINT32(f,&p->attribute);
        T3DwriteUINT32(f,&npoints);
        counter=T3Dgettextureindex(p->tex);
        tpolyvertex *pv=p->vlist;
        tpolyvertex *end=&pv[npoints];
        T3DwriteUINT32(f,&counter);               
        
        for (;pv<end;pv++)
        {
            T3DwriteUINT32(f,&pv->attribute);
            //write material index; this will be pervertex lighting soon so might as well plan ahead
            counter=T3Dgetmaterialindex(pv->mat);
            T3DwriteUINT32(f,&counter);
            T3Dwritefloat(f,pv->texv.x);
            T3Dwritefloat(f,pv->texv.y);
            vref=pv->v->readtag();
            T3DwriteUINT32(f,&vref);                                                                               
        }                
        p=p->nxt();
    }
    return TRUE;    
}



inline UINT32 T3Dwritelight(FILE *f,tlight *light)
{  
    T3Dwritebasic3dobject(f,(tobject3d *)light);
    T3DwriteUINT8(f,&light->lighttype);
    T3Dwritecolour3f(f,&light->diffuse);
    T3Dwritecolour3f(f,&light->ambient);
    T3Dwritecolour3f(f,&light->specular);
    T3Dwritefloat(f,light->conesize);
    T3Dwritefloat(f,light->intensity);
    return TRUE;
}

bool T3Dsavenewclay(char *filename,tscene *scene)
{
    UINT32 codeword;
    UINT32 blocksize;
    UINT16 minorv=0;
    UINT16 majorv=1;
    UINT32 blockpos,endpos;
	
    FILE *f=fopen(filename,"wb");
    if (!f)
    {        //set ERR_LEVEL
        return NULL;
    }
    tobject3d *obj=(tobject3d*)scene->scenegraph.root;
    fwrite(&T3Dsignature,4,1,f);
    fwrite(&majorv,2,1,f);
    fwrite(&minorv,2,1,f);
    //start of sceneprefs block
    codeword=otSCENE;
    fwrite(&codeword,4,1,f);
    blocksize=12+T3Dcolour3fsize;
    T3DwriteUINT32(f,&blocksize);
    T3DwriteUINT32(f,&T3Dsceneattributes);    
    T3Dwritefloat(f,scene->fogminz);
    T3Dwritefloat(f,scene->fogmaxz);   
    T3Dwritecolour3f(f,&scene->fogcolour);
    //end of scene block
    T3Dwritematerialbank(f);
	T3Dwritetexturebank(f);
    while (obj!=NULL)
    {        
        codeword=obj->type;
        if ((codeword & (otGEOM+otLIGHT))!=0)
        {
			T3DwriteUINT32(f,&codeword);
			//remember here.... 
			blockpos=ftell(f);
			T3DwriteUINT32(f,&blocksize);
			
			switch (codeword)
			{
			case otGEOM:T3Dwritemesh(f,(t3dmesh *)obj);break;
			case otLIGHT:T3Dwritelight(f,(tlight *)obj);break;       
			}
			endpos=ftell(f);          
			blocksize=endpos-blockpos-4;
			
			fseek(f,blockpos,SEEK_SET);
			T3DwriteUINT32(f,&blocksize);        
			fseek(f,endpos,SEEK_SET);
        }                    
        obj=(tobject3d*)obj->next;
    };
    codeword=otEOF;
    T3DwriteUINT32(f,&codeword);
    fclose(f);
    return TRUE;
}



flt T3Dreadfloat(FILE *f,float *fltval)
{
    static float fval;
    fread(&fval,4,1,f);
    *fltval=fval;
    return fval;
}


flt T3Dreadfloat(FILE *f,double *fltval)
{
    static float fval;
    fread(&fval,4,1,f);
    *fltval=fval;
    return fval;
}

inline UINT32 T3DreadUINT8(FILE *f,UINT8 *n)
{
    fread(n,1,1,f);
    return 1;    
}

inline UINT32 T3DreadUINT32(FILE *f,UINT32 *n)
{
    fread(n,4,1,f);
    return 4;
}

inline UINT32 T3DreadINT32(FILE *f,INT32 *n)
{
    fread(n,4,1,f);
    return 4;
}


UINT32 T3Dreadcolour3f(FILE *f,trgbfloat *col)
{
    float val[3];
    fread(val,12,1,f);
    col->r=val[0];
    col->g=val[1];
    col->b=val[2];
    return 12;
}

UINT32 T3Dreadvector3f(FILE *f,vector *v)
{
    float val[3];
    fread(val,12,1,f);
    v->x=val[0];
    v->y=val[1];
    v->z=val[2];
    return 12;
}

UINT32 T3Dreadbasicobject(FILE *f,tobject3d *obj)
{    
    fread(obj->name,32,1,f);
    fread(&obj->attribute,4,1,f);
    obj->attribute=T3Ddefaultobjectattributes;
    
    T3Dreadvector3f(f,&obj->p);
    T3Dreadvector3f(f,&obj->o);
    T3Dreadvector3f(f,&obj->s);
    return TRUE;
}

void logcolour(trgbfloat *col)
{
    sprintf(buf,"Red: %f Green: %f Blue %f",col->r,col->g,col->b);
    logmessage( (void*)&textcol,buf);
}    

tlight *T3Dreadlight(FILE *f)
{
    tlight *light=new tlight();
    T3Dreadbasicobject(f,(tobject3d*)light);  
    T3DreadUINT8(f,&light->lighttype);
    T3Dreadcolour3f(f,&light->diffuse);
    T3Dreadcolour3f(f,&light->ambient);
    T3Dreadcolour3f(f,&light->specular);
    T3Dreadfloat(f,&light->conesize);
    T3Dreadfloat(f,&light->intensity);
    //logcolour(&light->diffuse);
    //logcolour(&light->ambient);
    //logcolour(&light->specular);            
    return light;
}



void T3Dreadsceneattributes(FILE *f,tscene* scene)
{
    float fmin,fmax;//,fdensity;
    T3DreadUINT32(f,&T3Dsceneattributes);    
    fmin=T3Dreadfloat(f,&fmin);
    fmax=T3Dreadfloat(f,&fmax);
    scene->setfog(fmin,fmax,1);
	
    T3Dreadcolour3f(f,&scene->fogcolour);        
}


material **objectmatbank;
UINT32 materialcount=0;
bool T3Dreadmaterialbank(FILE *f)
{
	
    materialentry cmat;
        
	
    fread(&materialcount,4,1,f);
    material **cmatref=objectmatbank=new material*[materialcount];
    
    sprintf(buf,"material count:%i sizeof block %i",materialcount,MATERIALENTRYSIZE*materialcount);
    logmessage( (void*)&textcol,buf);
    
    for (int i=0;i<materialcount;i++)
    {
		fread(&cmat,MATERIALENTRYSIZE,1,f);
		sprintf(buf,"material data: %.03f %.03f %.03f %.03f %.03f %.03f %.03f %.03f %.03f %.03f %.03f %.03f %i %.03f %f",cmat.ar,cmat.ag,cmat.ab,cmat.dr,cmat.dg,cmat.db,cmat.sr,cmat.sg,cmat.sb,cmat.er,cmat.eg,cmat.eb,cmat.sp,cmat.st,cmat.t);
		logmessage( (void*)&textcol,buf);                                                        
		//add material
		
		*cmatref=T3Daddmaterial(&cmat);
		
		sprintf(buf,"material count:%i",materialcount);
		logmessage( (void*)&textcol,buf);
		cmatref++;       		
    }
    return TRUE;
}

textureref **texlist;
UINT32 texturecount;

inline bool T3Dreadtexturebank(FILE *f)
{
	char name[32];
	tsurface *tex=NULL;
	
	
	fread(&texturecount,4,1,f);
	if (texturecount)
	{
		texlist= new textureref*[texturecount];
		for (UINT32 i=0;i<texturecount;i++)
		{
			fread(name,32,1,f);
			tex=searchreadimage(TWINrendercontext,name,PXFrgb565);		
			texlist[i]=T3Daddtexture(tex,name);					
		}
	}
	return true;
}



t3dmesh *T3Dreadmesh(FILE *f)
{
    vector v;
    vertex *cv;
    INT32 pcount,vcount;
    UINT32 attribute,npoints,texref,matref;
    t3dmesh *mesh=new t3dmesh();
    mesh->attribute |= ofEDITABLE;
    
    sprintf(buf,"created mesh");
    T3Dreadbasicobject(f,(tobject3d*)mesh);
    mesh->type=otGEOM;
    T3DreadINT32(f,&vcount);
    tpolygon *cpoly;
	
    sprintf(buf,"vertex count: %i",vcount);
    logmessage( (void*)&textcol,buf);
    if (vcount<0) return NULL;
    while (vcount>0)
    {
        //should and this with a mask;
        T3DreadUINT32(f,&attribute);
        attribute &=~naSELECTEDMASK;
        T3Dreadvector3f(f,&v);
        mesh->addvertex(v.x,v.y,v.z)->attribute =attribute | naVISIBLE+naENABLED;        
        vcount--;
    }
    T3DreadINT32(f,&pcount);
    sprintf(buf,"polygon count: %i",pcount);
    logmessage( (void*)&textcol,buf);    
    
    while (pcount>0)
    {
        UINT32 attribute;
        T3DreadUINT32(f,&attribute);//T3Ddefaultpolyattributes
        //T3Ddefaultpolyattributes |= paVISIBLE | paENABLED;
        T3DreadUINT32(f,&npoints);        
        T3DreadUINT32(f,&texref);
		
        cpoly=mesh->addpoly(npoints,NULL);
        cpoly->attribute=attribute;
		if (texlist && texref<texturecount)		
			cpoly->tex=texlist[texref];
		else
			cpoly->tex=NULL;
        for (UINT32 i=0;i<npoints;i++)
        {
            T3DreadUINT32(f,&attribute);            
            T3DreadUINT32(f,&matref);
			if (matref>materialcount)
				matref=0;
			float u,v;
            T3Dreadfloat(f,&u);
            T3Dreadfloat(f,&v);            
            T3DreadINT32(f,&vcount);
            cv=mesh->getvertex(vcount);
            //if (cv)             
			T3Dsetpolygonvertex(cpoly,cv,i,0,NULL);
			T3Dsetpolygonvertexuv(cpoly,i,u,v);
			
            //else
			// {
			//   logmessage((void*)&textcol,"Vertex is not there!");
			//   return NULL;
			// }
            cpoly->vlist[i].mat=objectmatbank[matref];                        
        }                
        pcount--;
    }
	
    
    logmessage( (void*)&textcol,"Building edges...");
    mesh->buildedges();
	mesh->calculatenormals();
	
    logmessage( (void*)&textcol,"Success");
    return mesh;    
}


void addstandardlight(tscene *scene)
{
    tlight *plight=new tlight;
    strcpy(plight->name,"White");
    plight->lighttype=LTpoint;
    plight->rotate(0,0,0);
    plight->moveto(-60,-60,-60);
    
    plight->ambient=trgbfloat(FLT_0,FLT_0,FLT_0);
    plight->diffuse=trgbfloat(0.7f,0.7f,0.7f);
    plight->specular=trgbfloat(FLT_1,FLT_1,FLT_1);
	
    scene->addobject(NULL,plight);
    plight->enable(TRUE);
}


bool T3Dloadnewclay(char *filename,tscene *scene)
{
    tlight *l;
    t3dmesh *m;    
    UINT32 codeword;
    UINT32 blocksize;
    UINT32 sig;
    UINT8 majorv,minorv;
    FILE *f;
    bool ret=TRUE;
	UINT32 mcount=0;
	texlist=NULL;
	getpathanddrive(filename,CURRENT_MODEL_PATH);
    f=fopen(filename,"rb");
    objectmatbank=NULL;
    if (!f) return NULL;
    fread(&sig,4,1,f);
	texlist=NULL;
    itoa((UINT32)sig,buf,16);
    logmessage( (void*)&textcol,buf);
    if (sig!=T3Dsignature)
    {        
        logmessage( (void*)&textcol,"Signature was not correct, trying old format");                
        if (T3Dloadclay(filename,scene))
        {
			
            ret=TRUE;            
            logmessage( (void*)&textcol,"Load sucessful");
        }else
        {
            ret=FALSE;
            logmessage( (void*)&textcol,"Load failed");
        }
        return ret;
    }
	
    fread(&majorv,2,1,f);
    fread(&minorv,2,1,f);
    sprintf(buf,"Version: %i.%i",majorv,minorv);
    logmessage( (void*)&textcol,buf);    
    fread(&codeword,4,1,f);
    if (codeword==otSCENE)
		logmessage( (void*)&textcol,"Found Scene tag");
    else
    {
		logmessage( (void*)&textcol,"Scene tag not found");
		ret=FALSE;
		goto exitgracefully;
    }
    fread(&blocksize,4,1,f);
	
    T3Dreadsceneattributes(f,scene);
    //fseek(f,blocksize,SEEK_CUR);
    
    fread(&codeword,4,1,f);
    if (codeword==otMATERIALBANK)
		logmessage( (void*)&textcol,"Found Material bank");
    else
    {
		logmessage( (void*)&textcol,"Material bank not found");
		ret=FALSE;
		goto exitgracefully;
    }
    fread(&blocksize,4,1,f);
    sprintf(buf,"material bank block size:%i",blocksize);    
    T3Dreadmaterialbank(f);
    //fseek(f,blocksize,SEEK_CUR);
	
	
    while (codeword!=otEOF)
    {
		fread(&codeword,4,1,f);
		fread(&blocksize,4,1,f);
		switch (codeword)
		{
		case otLIGHT:
			logmessage( (void*)&textcol,"found light");
			
			l=T3Dreadlight(f);
			if (l)
				scene->addobject(NULL,l);
			break;
		case otGEOM:					
			//if (mcount==0)
			//{
			logmessage( (void*)&textcol,"found mesh");                    
			m=T3Dreadmesh(f);       
			if (m)
			{
				m->calculatenormals();        
				scene->addobject(NULL,m);
			}else
			{
				ret=false;
				goto exitgracefully;
			}

			
			mcount++;
			break;
		case otTEXTUREBANK:
			logmessage( (void*)&textcol,"found texture bank");                    
			T3Dreadtexturebank(f);
			break;
		default:
			logmessage( (void*)&textcol,"Tag not recognized");
			sprintf(buf,"Tag code: %i",codeword);
			logmessage( (void*)&textcol,buf);
			ret=true;
			goto exitgracefully;
		}              
		
    }
    
exitgracefully:
	if (texlist)
		delete [] texlist;
    if (objectmatbank)
		free(objectmatbank);
    fclose(f);
	
    return ret;    
    
}

#define CLAYvisible 128
#define CLAYabsolute 32
#define CLAYconnected 16
#define CLAYoutlined 8
#define CLAYsmooth 2
#define CLAYdoublesided 1


trgbabase pal16[]={{0,0,0,0},
{24,24,32,0},{50,50,70,0},{70,70,82,0},{118,119,123,0},
{130,130,133,0},{150,150,155,0},{180,180,180,0},{23,24,80,0},
{89,96,140,0},{120,230,255,0},{0,140,0,0},{255,240,0,0},
{240,190,8,0},{130,0,0,0},{240,240,220,0}};

bool T3Dloadclay(char *filename,tscene *scene)
{
	
    logmessage( (void*)&textcol,"attempting to load clayworks file");
    
    tpointfloat texvec;
    UINT16 maxvecs,maxlines,maxpolys,i;
    UINT8 vi;    
    pasreal pvec;//pasreal pvec;
    UINT32 oldpolyflags=T3Ddefaultpolyattributes;
	
	
    tpolygon *tpoly;
    claypoly ppoly;
    vector ivec;
    FILE *f;
    f=fopen(filename,"rb");
    if (!f)
    {        //set ERR_LEVEL
        return NULL;
    }
    
    fread(&maxvecs,2,1,f);
    fread(&maxlines,2,1,f);
    fread(&maxpolys,2,1,f);
    sprintf(buf,"vertex count:%i,edge count:%i,polygon count:%i",maxvecs,maxlines,maxpolys);
    logmessage((void*)&textcol,buf);
    /*if (fgetsize(filename)!=6+(maxvecs*PASREALSIZE*3)+(maxlines*CLAYJOINSIZE)+(maxpolys*CLAYPOLYSIZE))
    {
	logmessage((void*)&textcol,"Filesize check failed");
	return NULL;
}*/
    t3dmesh *obj=new t3dmesh();            
    for (i=0;i<maxvecs;i++)
    {
        fread(&pvec,PASREALSIZE,1,f);
        ivec.x=real2float(pvec);
        fread(&pvec,PASREALSIZE,1,f);
        ivec.y=real2float(pvec);
        fread(&pvec,PASREALSIZE,1,f);
        ivec.z=real2float(pvec);
		
        obj->addvertex(ivec.x,ivec.y,ivec.z);
    }
	
    fseek(f,CLAYJOINSIZE*maxlines,SEEK_CUR);
    
    for (i=0;i<maxpolys;i++)
    {
        fread(&ppoly,1,CLAYPOLYSIZE,f);
        trgbfloat col;
        col.r=(float)pal16[ppoly.colour & 15].r/255.0f;
        col.g=(float)pal16[ppoly.colour & 15].g/255.0f;
        col.b=(float)pal16[ppoly.colour & 15].b/255.0f;
		
        T3Dmaterial=T3Daddmaterial(col,2,FLT_0);
        T3Ddefaultpolyattributes=paENABLED;
        if((ppoly.attrib&CLAYsmooth)!=0) T3Ddefaultpolyattributes|=paSHADED;
        if((ppoly.attrib&CLAYabsolute)!=0) T3Ddefaultpolyattributes|=paTRUECOLOUR;
        if((ppoly.attrib&CLAYdoublesided)!=0) T3Ddefaultpolyattributes|=paDOUBLESIDED;
        tpoly=obj->addpoly(ppoly.numsides);
		
        UINT8 *p=&ppoly.numsides;
        p++;
        UINT32 vidx;
        for (vi=0;vi<ppoly.numsides;vi++)
        {
            vidx=*((UINT16*)p);             
            if (vidx>obj->vertexlist.count)
            {
                sprintf(buf,"Vertex index check failed;%i",vidx);
                logmessage( (void*)&textcol,buf);                
                return NULL;
            }
            p+=2;            
            T3Dsetpolygonvertex(tpoly,obj->getvertex(vidx),vi,0);
            
        }
    }
    fclose(f);
    obj->buildedges();         
	obj->calculatenormals();
	scene->addobject(NULL,obj);
    T3Ddefaultpolyattributes=oldpolyflags;
    return obj!=0;
}


#define IFFNOTYPE 0
#define IFFFORM 1297239878
#define IFFLWOB 1112495948
#define IFFPNTS 1398034000
#define IFFSRFS 1397117523
#define IFFPOLS 1397509968
#define IFFSURF 1179899815

#define IFFCOLR 1380732739
#define IFFFLAG 1195461702
#define IFFLUMI 1229804876
#define IFFDIFF 1179011396
#define IFFSPEC 1128616019
#define IFFREFL 1279673682
#define IFFTRAN 1312903764
#define IFFGLOS 1397705799
#define IFFRIMG 1196247378
#define IFFRSAN 1312904018
#define IFFRIND 1145981266
#define IFFEDDE 1162101829
#define IFFSMAN 1312902483

/*
char IFF_LUMI[4]={'L','U','M','I'};
char IFF_DIFF[4]={'D','I','F','F'};
char IFF_SPEC[4]={'S','P','E','C'};
char IFF_REFL[4]={'R','E','F','L'};
char IFF_TRAN[4]={'T','R','A','N'};
char IFF_GLOS[4]={'G','L','O','S'};
char IFF_RIMG[4]={'R','I','M','G'};
char IFF_RSAN[4]={'R','S','A','N'};
char IFF_RIND[4]={'R','I','N','D'};
char IFF_EDGE[4]={'E','D','D','E'};
char IFF_SMAN[4]={'S','M','A','N'};    

  //    CTEX, DTEX, STEX, RTEX, TTEX, and BTEX
  //      TIMG TFLG TSIZ, TCTR, TFAL, and TVEL TCLR TVAL TFRQ TSP0, TSP1, and TSP2
  
	/*
	extern int      fgetpos( FILE *__fp, fpos_t *__pos );
	extern int      fseek( FILE *__fp, long int __T3Doffset, int __whence );
	extern int      fsetpos( FILE *__fp, const fpos_t *__pos );
	*/
	
	
	/*WARNING, HAS MANY BUGS, THIS IS NOT STABLE!!!!!!!!!*/
	
	bool T3Dloadlw(char *filename,tscene *scene)
	{
		fpos_t oldpos;
		UINT32 chunk;
		float cfloatx,cfloaty,cfloatz;
		tpointfloat texvec;
		UINT32 chunksize;
		INT16 cint16,numppoints=0;
		INT32 numpoints=0,i;//numpolys,numsurfs;
		t3dmesh *obj=NULL;
		tpolygon *tpoly;
		UINT32 oldpolyflags=T3Ddefaultpolyattributes;
		
		FILE *f=fopen(filename,"rb");
		if (!f)
		{        //set ERR_LEVEL
			return NULL;
		}
		obj=new t3dmesh();         
		while (!feof(f))
		{
			fread(&chunk,4,1,f);
			
			
			
			fread(&chunksize,4,1,f);
			chunksize=intel_mot(chunksize);                
			switch (chunk)
			{
			case IFFFORM:
				fread(&chunk,4,1,f);                     
				
				if (chunk!=IFFLWOB)
				{
					return NULL;
				}
				
				break;
			case IFFSRFS:
				fseek(f,chunksize,SEEK_CUR);
				break;
				
			case IFFPOLS:           
				//fseek(f,chunksize,SEEK_CUR);
				while (chunksize>0)
				{
					T3Ddefaultpolyattributes=paENABLED;                                                          
					fread(&numppoints,2,1,f);
					
					numppoints=intel_mot16(numppoints);
					if (numppoints>256 && numppoints>=3)
					{
						delete obj;
						obj=NULL;
						goto exitgracefully;
					}
					tpoly=obj->addpoly(numppoints);    
					
					chunksize-=2;
					for (i=0;i<numppoints;i++)
					{
                        fread(&cint16,2,1,f);
                        cint16=intel_mot16(cint16);
                        if (cint16>(INT16)obj->vertexlist.count)
                        {
                            delete obj;
							obj=NULL;
                            goto exitgracefully;
                        }
                        T3Dsetpolygonvertex(tpoly,obj->getvertex(cint16),numppoints-i-1,0);
                        T3Dsetpolygonvertexuv(tpoly,numppoints-i-1,texvec.x,texvec.y);
                        
                        chunksize-=2;
					}
					
					fread(&cint16,2,1,f);
					cint16=intel_mot16(cint16);                   
					chunksize-=2;
					//tpoly->mat=&WHITEMATERIAL;
					if (cint16<0)
					{                       
						fread(&cint16,2,1,f);
						cint16=intel_mot16(cint16);
						chunksize-=2;
						cint16=abs(cint16);
					}
				}
				fsetpos( f,&oldpos);
				fseek(f,chunksize,SEEK_CUR);
				goto exitgracefully;
                break;                                
				
			case IFFPNTS:
				if (obj==NULL) return NULL;               
				
				numpoints=chunksize/12;
				
				
				
				fgetpos( f, &oldpos);
				
				for (i=0;i<numpoints;i++)
				{
					fread(&cfloatx,4,1,f);
					intel_motflt(&cfloatx);
					fread(&cfloaty,4,1,f);
					intel_motflt(&cfloaty);
					fread(&cfloatz,4,1,f);
					intel_motflt(&cfloatz);                   
					obj->addvertex(cfloatx,-cfloaty,cfloatz);
				}
				fsetpos( f,&oldpos);
				fseek(f,chunksize,SEEK_CUR);
				break;
			default:
				//delete obj;
				//return NULL;
				fseek(f,chunksize,SEEK_CUR);
				goto exitgracefully;
				break;
			}
			
			
    }
exitgracefully:
	if (obj)
	{
		obj->buildedges();         
		obj->calculatenormals();
		scene->addobject(NULL,obj);
	}
    T3Ddefaultpolyattributes=oldpolyflags;
    fclose(f);
    return obj!=NULL;
	
}    

/*

  struct CVertex
  {
  float x,y,z;
  };
  
	struct CPolygon
	{
    int verts[3];
	};
	
	  struct CModel
	  {
	  CVertex* verts;
	  int numverts;
	  CPolygon* polys;
	  int numpolys;
	  };
	  
		
		  
			CModel g_model;
			*/
			
			// Load3DS
			//
			// Load a .3ds file into memory for parsing
			//
			long EatChunk(char* buffer,tscene *scene,UINT32 fsize);
			
			bool T3Dload3ds(char *filename,tscene *scene)
			{
				FILE		*f;				//file handle
				bool        doneloading = false;
				UINT32		filelength;
				char*       memfile;        // file loaded into memory
				
				// load entire file into memory (easier to deal with)
				f=fopen(filename,"rb");
				if (!f) return false;
				filelength=fsize(f);
				getpathanddrive(filename,CURRENT_MODEL_PATH);
				memfile = new char[filelength];
				fread(memfile,filelength,1,f);
				// parse it
				EatChunk(memfile,scene,filelength);
				delete [] memfile;
				return true;
			}
			
			// EatChunk
			//
			//  This function recursively handles chunks
			// in a .3ds file.  When the function exits,
			// the return value (i) should be the length
			// of the current chunk.  Right now we only
			// create one model (the first one listed in
			// the file) and read in verts and polys only.
			// To grab other info from the file, create
			// a new case label in the switch statement
			// for the chunk type you want to react to.
			// 
			
			int GetPercentage(char *buffer,UINT16 id,UINT8 *val)
			{
				switch (id)
				{
				case 0x0030 : *val=(*(UINT16*)buffer)*2.56;return 2;
				case 0x0031 : *val=(*(float*)buffer)*256.0f;return 4;
				}
				return 0;
			}
			
			int GetPercentage(char *buffer,UINT16 id,float *val)
			{
				switch (id)
				{
				case 0x0030 : *val=(*(UINT16*)buffer);*val/=100.0f;return 2;
				case 0x0031 : *val=(*(float*)buffer);return 4;
				}
				return 0;
			}
			
			int GetString(char *buffer,char *outbuffer)
			{
				int i=0;
				while (buffer[i])
				{
					if (outbuffer)			
						outbuffer[i]=buffer[i];		
					i++;				
				}		
				outbuffer[i]=0;
				i++;
				return i;
				
			}
			
			int GetColour(char *buffer,UINT32 i,trgbfloat *col)
			{
				return 3;
			}
			
			int GetColour(char *buffer,UINT32 i,trgba *col)
			{
				return 3;
			}
			
#pragma pack(1)
			typedef struct chnk3ds_
			{
				UINT16 id;
				UINT32 len;
			}chnk3ds;
#pragma pack()
			
			void setobjuvvalues(t3dmesh *obj)
			{
				tpolygon *p=(tpolygon*)obj->polygonlist.first;
				while (p)
				{
					for (int i=0;i<p->npoints;i++)	
					{
						p->vlist[i].texv.x=p->vlist[i].v->point2d.x;
						p->vlist[i].texv.y=1-p->vlist[i].v->point2d.y;
					}		
					p=(tpolygon*)p->next;
				}
			}
			
			long EatChunk(char* buffer,tscene *scene,UINT32 fsize)
			{
				matrix4x4 mat;
				LMxrotation(&mat,HALFPI);
				T3Dctm=&mat;
				textureref *texref=NULL;	
				
				vertex **vertexlist=NULL;
				tpolygon **polygonlist=NULL;
				chnk3ds chnk;
				UINT32     i = 0;  // index into current chunk
				UINT32	   j;
				UINT16 i1,i2,i3;
				
				trgbfloat *ccol=NULL;
				UINT32 nverts,npolys;
				trgba col;
				vector v;
				UINT32 idbase=T3Dmaterialbank.count;
				
				t3dmesh *obj=NULL;
				material *cmat=NULL;
				tpolygon *cpoly;
				char matsearchname[64];
				char texname[64];
				UINT32 oldattrib=T3Ddefaultpolyattributes;
				T3Ddefaultpolyattributes|=paSHADED;
				tsurface *tex;
				
				while (i<fsize)
				{
					chnk=*(chnk3ds*)&buffer[i];    
					i+=6;
					chnk.len-=6;
					switch (chnk.id)
					{
					case 0x4D4D:    // main file
									/*while ((*(short*)(buffer+i) != 0x3D3D) &&
									(*(short*)(buffer+i) != 0xB000))
						i += 2;*/
						break;
					case 0xAFFF:
						
						break;
					case 0xA000:		
						texref=NULL;
						cmat=T3Daddmaterial();
						i+=GetString(&buffer[i],cmat->name);						
						break;
					case 0xA010:
						ccol=&cmat->ambient;
						break;
					case 0xA020:
						ccol=&cmat->diffuse;
						break;
					case 0xA030:
						ccol=&cmat->specular;
						break;
					case 0xA040:
						chnk=*(chnk3ds*)&buffer[i];    
						i+=6;
						i+= GetPercentage(&buffer[i],chnk.id,&cmat->st);			
						break;
					case 0xA041:
						chnk=*(chnk3ds*)&buffer[i];    
						i+=6;
						i+= GetPercentage(&buffer[i],chnk.id,&cmat->specpower);
						cmat->specpower>>=1;
						break;
					case 0xA200://texture
						
						break;
						
						//case 0xA201:
						//break;
					case 0xA300:			
						
						i+=GetString(&buffer[i],texname);			
						tex=searchreadimage(TWINrendercontext,texref->name,PXFrgb565);				
						if (tex)
						{
							texref=T3Daddtexture(tex,texname);
							if (cmat)
								cmat->tex1=texref;
						}			
						break;
						
					case 0x0013: 
					case 0x0010: //Rgb (float)
						if (ccol)			
							*ccol=*(trgbfloat*)&buffer[i];				
						
						i+=12;
						break;
					case 0x0011: //Rgb (byte)			
					case 0x0012: //Rgb (byte) gamma corrected
						if (ccol)
						{
							col=*(trgba*)&buffer[i];
							ccol->r=col.b;				
							ccol->g=col.g;				
							ccol->b=col.r;								
							ccol->r/=255.0f;
							ccol->g/=255.0f;
							ccol->b/=255.0f;
							BOUND(ccol->r,0,1);
							BOUND(ccol->g,0,1);
							BOUND(ccol->b,0,1);
						}
						i+=3;
						break;
						
						
						/*maps
						case 0xA360: //RGB Luma/Alpha tint 1
						break;
						case 0xA362: //RGB Luma/Alpha tint 2
						break;
						case 0xA364: //RGB tint R
						break;
						case 0xA366: //RGB tint G
						break;
						case 0xA368: //RGB tint B
						break;*/
						
					case 0x3D3D:    // editor data
						break;
					case 0x4000:    // object description
						j=0;
						if (obj)			
							setobjuvvalues(obj);
						cmat=NULL;
						texref=NULL;
						obj=new t3dmesh ();
						scene->addobject(NULL,obj);	
						i+=GetString(&buffer[i],obj->name);
						break;
					case 0x4140:
						nverts=*(UINT16*)(buffer+i);
						i+=2;
						for (j=0;j<nverts;j++)
						{
							v=*(vector*)(buffer+i);
							i+=8;
							if (vertexlist)				
								vertexlist[j]->point2d=*(tpointfloat*)&v;
							
						}
						break;
					case 0x4100:    // triangular polygon list
						
						break;
					case 0x4160:			
						//skip axis, for now
						v=*(vector*)&buffer[i];i+=12;
						v=*(vector*)&buffer[i];i+=12;
						v=*(vector*)&buffer[i];i+=12;
						v=*(vector*)&buffer[i];i+=12;
						//obj->moveto(v.x,v.y,v.z);
						
						break;
						
					case 0x4110:    // vertex list
						if (!obj->vertexlist.first)
						{
							nverts= *(short*)(buffer+i);
							if (vertexlist)
								delete [] vertexlist;
							vertexlist=new vertex*[nverts];
							i+=2;            
							for (j=0;j<nverts;j++)
							{
								vertexlist[j]=obj->addvertex(*(float*)&buffer[i],*(float*)&buffer[i+4],*(float*)&buffer[i+8]);                
								i+=12;
							}
						}
						else
							i+=chnk.len;
						break;
					case 0x4120:
						if (obj && (!obj->polygonlist.first))
						{
							npolys = *(short*)(buffer+i);
							if (polygonlist)
								delete [] polygonlist;
							polygonlist=new tpolygon*[npolys];
							
							i+=2;        
							if (polygonlist)
								for (j=0;j<npolys;j++)
								{
									polygonlist[j]=cpoly=obj->addpoly(3);	
									
									i1=*(UINT16*)&buffer[i];
									i2=*(UINT16*)&buffer[i+2];
									i3=*(UINT16*)&buffer[i+4];
									T3Dsetpolygonvertex(cpoly,vertexlist[i1],2,0);					
									T3Dsetpolygonvertex(cpoly,vertexlist[i2],1,0);					
									T3Dsetpolygonvertex(cpoly,vertexlist[i3],0,0);
									
									i+=6;
									i+=2;   // skip face info
								}
								
								obj->buildedges();         
								obj->calculatenormals();
						}else
							i+=chnk.len;
						
						
						break;
					case 0x4130:
						j=0;
						
						i+=GetString(&buffer[i],matsearchname);
						
						T3Dmaterial=T3Dfindmaterialbyname(matsearchname);			
						npolys = *(short*)(buffer+i);
						i+=2;
						if (!T3Dmaterial) 
							T3Dmaterial=(material*)T3Dmaterialbank.first;
						
						while (npolys)
						{
							i1=*(UINT16*)&buffer[i];
							i+=2;
							polygonlist[i1]->setmaterial(T3Dmaterial);
							if (T3Dmaterial->tex1)
								polygonlist[i1]->tex=T3Dmaterial->tex1;
							npolys--;
						}
						
						break;
						
					default:
						i+=chnk.len;			
						break;
		}
	}
	if (vertexlist)
		delete [] vertexlist;
	if (polygonlist)
		delete [] polygonlist;    
	if (obj)
		setobjuvvalues(obj);
	UINT32 T3Ddefaultpolyattributes=oldattrib;
    return true;
}



