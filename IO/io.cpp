//io helper functions
#include "stdafx.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ttypes.h"
#include "io.h"


char lowcase(char str)
{
    if ((str>='a')&&(str<='z'))
       return str-32;
    else
       return 0;
}

char upcase(char str)
{
    if ((str>='a')&&(str<='z'))
       return str-32;
    else
       return 0;
}

void upcase(char *str)
{
    while (*str!=0)
    {
        if ((*str>='a')&&(*str<='z'))
           *str-=32;
        str++;
    }
}

void lowcase(char *str)
{
    while (*str!=0)
    {
        if ((*str>='A')&&(*str<='Z'))
           *str+=32;
        str++;
    }
}

char *getpathanddrive(char *fullpath,char *justpath)
{
    char dummy[255];
	char path[255];
    //char dir[255];
    _splitpath(fullpath,justpath ,path, dummy, dummy);
    strcat(justpath,path);
    return justpath;        
}


char *getpath(char *fullpath,char *justpath)
{
    char dummy[255];
    //char dir[255];
    _splitpath(fullpath,dummy,justpath , dummy, dummy);
        
    return justpath;        
}

char *getextension( char *name )
{
	char *nameEnd = name + strlen( name ) - 1;

	while( nameEnd != name )
	{
		if( *nameEnd == '.' )
			break;
		nameEnd--;
	}
	if (nameEnd==name) return NULL;
	return nameEnd;
}

char *getname( char *name )
{
	char *nameEnd = name + strlen( name ) - 1;

	while( nameEnd != name )
	{
		if(( *nameEnd == '\\' )||( *nameEnd == '/' ))
			break;
		nameEnd--;
	}	
	if (nameEnd!=name)
		nameEnd++;
	return nameEnd;
}

char *makepath(char *outtext,char *intext)
{
    if (outtext==NULL)
      outtext=intext;
    if (outtext!=intext)
      strcpy(outtext,intext);
      

      
    char *cchar=outtext;
    char *lastslash=NULL;
    while (*cchar!=0)
    {
        if (*cchar=='\\')
            lastslash=cchar+1;
        cchar++;
    }
    if (lastslash!=NULL)
       *lastslash=0;
    return outtext;
}                

bool fexist(char *filename)
{

    WIN32_FIND_DATA findstruct;
	int retval=(int)FindFirstFile(filename,&findstruct);
	return (retval!=-1);		    
}

UINT64 fsize(char *filename)
{
	WIN32_FIND_DATA findstruct;
	int retval=(int)FindFirstFile(filename,&findstruct);

	if (retval==-1) return -1;
	return (findstruct.nFileSizeHigh * (MAXDWORD+1)) + findstruct.nFileSizeLow;	
}

UINT32 fsize(FILE *f)
{
	UINT32 fpos=ftell(f);
	fseek(f,0,SEEK_END);
	UINT32 retme=ftell(f);
	fseek(f,fpos,SEEK_SET);
	return retme;

}