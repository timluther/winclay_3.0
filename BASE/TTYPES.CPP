#include "stdafx.h"
#include "ttypes.h"
#include <stdlib.h>
#include <string.h>



//////////////////////////////////////////////////////////////////////////////////////////
// RANDOM MISC STUFF
//


int round(float val){
  if ((int)(val+0.5)>(int)val)
    return ((int)(val)+1);  
  return ((int)val);
}

//////////////////////////////////////
//string operations

char strgreater(char *a,char *b)
{
    while ((*a)&&(*b))
    {
        if (*a>*b) return 1;
        if (*a<*b) return 0;        
        a++;b++;
    }
    return 2;
}

char *copyfilter(char *tgt,char *src)
{
    char *oldtgt=tgt;
    //while ((*src!=0)&&(*src!='*'))
    //    src++;
    while ((*src!=0)&&(*src!=' '))    
        *(tgt++)=*(src++);
    *tgt=0;
    return oldtgt;
      
}

char *truncatepathto(char *path,char *dir)
{
    char *opath=path;
    char *dirscan=dir;
    while (*path!=0)
    {
        if (*dirscan==0)
        {            
            path++;//avoid nulling the forward slash
            if (*path==0) return NULL;
            *path=0;//null after forward slash
            return opath; //exit with directory
        }else if (*dirscan!=*path)
        {
            dirscan=dir;
        }else
            dirscan++;
        path++;                                        
    }
    return opath;
}

char *setdepth(char *dir,UINT32 depth)
{
    char *olddir=dir;
    UINT32 count=0;
    while ((*dir != 0)&&(depth>0))
      if (*(dir++)=='\\') depth--;
    *dir=0;
    return olddir;
}


UINT32 getdepth(char *dir)
{
    UINT32 count=0;
    while (*dir != 0)    
      if (*(dir++)=='\\') count++;
    return count;
}

char *getdir(char *dir,char *dest)
{    
    while (*dir != '\\')
    {
      if (*dir=='\0') return NULL;
      *dest=*dir;
      dir++;
      dest++;
    }
    *dest='\0';
    return dir+1;
}

bool strcontains(char *str,char c)
{
    while (*str!=0)   
        if (*(str++)==c)
          return true;              
    return false;
}



void fswapi(void *a,void *b,UINT32 len)
{
    if (len==0) return;
    _asm
    {
        mov edi,a
        mov esi,b
        mov ecx,len     
        sloop:
          mov al,[edi]
          mov ah,[esi]
          mov [edi],ah
          mov [esi],al
          inc edi
          inc esi
          dec ecx
        jnz sloop
    }
}




//////////////////////////////////////////////////////////////////////////////////////////
// LINKED LIST STUFF
//


void tlinkedlist::init(UINT16 size_)
{
  count=0;
  first=NULL;
  last=NULL;
  size=size_;
} 

//should not use free
void tlinkedlist::destroy(void (*cleanup)(telem *element))
{
  telem *next,*curr;
  curr=first;
  while (curr!=NULL)
  {
      next=curr->next;
      if (cleanup)
        cleanup(curr);
//      free(curr);
      curr=next;  
  }
  first=NULL;
  last=NULL;
  count=0;
}

void tlinkedlist::repairlast()
{
    telem *celem=first;
    if (celem)
    while (celem->next)
      celem=celem->next;
    last=celem;
}

int tlinkedlist::removelots(linkcriteriafunction lfunc,linkcleanupfunction cfunc)
{
    if (!lfunc) return 0;
	int rcount=0;
    telem *ce,*prev;        
    ce=first;
    prev=NULL;
    while (ce!=NULL)
    {                                           
          while (ce && lfunc(ce))
          {             
             if (cfunc)
               cfunc(ce);
             ce=ce->next;
             count--;                   
			 rcount++;
          }
          if (prev)                               
            prev->next=ce;                    
          else
            first=ce;            
          prev=ce;
          if (ce)
             ce=ce->next;                                   
     }
     repairlast();
     return rcount;
}


telem *tlinkedlist::remove(telem *previous,telem *popped,void (*cleanup)(telem *element))
{
    if (!popped) return NULL;
    if (popped!=first)
    {      
       previous->next=popped->next;
       if (last==popped)
          last=previous;
    }else
       first=popped->next;
    if (cleanup)    
      cleanup(popped);    
    count--;
    return popped;
}

telem *tlinkedlist::remove(telem *popped,void (*cleanup)(telem *element))
{
    if (!popped) return NULL;
    if (popped!=first)
    {      
       telem *previous=prev(popped);
       previous->next=popped->next;
       if (last==popped)
          last=previous;
    }else
       first=popped->next;
    if (cleanup)    
      cleanup(popped);
    count--;
    return popped;    
}

telem *tlinkedlist::prevwrap(telem *elem)
{    
    if (elem==first) return last;    
    telem *currelem=first;    
    while ( (currelem != NULL )&&(currelem->next != elem))
        currelem=currelem->next;        
    return currelem;
}

telem *tlinkedlist::nextwrap(telem *elem)
{   
    if (elem==last) return first;
    if (elem==NULL) return NULL;
    return elem->next;    
}

telem *tlinkedlist::prev(telem *elem)
{
    telem *currelem=first;
    while ( (currelem != NULL )&&(currelem->next != elem))
        currelem=currelem->next;
    return currelem;
}

telem *tlinkedlist::pop(telem *popped,void (*cleanup)(telem *element))
{   
    telem *poppedelem;
    if (count<=0) return NULL;  
    poppedelem=last;
    if (popped)
        memcpy(popped,poppedelem,size);
    last = prev(last);
    if (last) 
        last->next = NULL;
    count--;
    if (count==0)
        first=last=NULL;
    if (poppedelem)
    {
        if (cleanup)
          cleanup(poppedelem);
        free(poppedelem);
    }
    return popped;  
}

telem *tlinkedlist::insert(telem *afterme,telem *newelement)
{
    //special case, the only time when first and last are NULL is in an empty list    
    if (!first)
    {
        first=last=newelement;
        first->next=last->next=NULL;
        return newelement;
    }
    //Is after me the last item? Then set it up
    if (afterme==last)
    {
        last->next=newelement;
        last=newelement;
        last->next=NULL;
        
    }else //normal case, it's at a random list location
    {
        newelement->next=afterme->next;
        afterme->next=newelement;        
    }
    count++;
    return newelement;    
}

telem *tlinkedlist::addtofront(telem *newelement)
{
    newelement->next=first;
    first=newelement;
    if (last==NULL)    
        last=newelement;
    count++;
    return newelement;
}


telem *tlinkedlist::push(telem *newelement)
{    
    if (count==0)    //first==NULL
        first=newelement;
    if (last!=NULL)
       last->next=newelement;
    
    newelement->next=NULL;
    last=newelement;    
    count++;
    return newelement;
}

telem *tlinkedlist::findelement(INT32 count)
{
    telem *c=first;
    while ((c!=NULL)&&(count>0))
    {
      c=c->next;
      count--;
    }
    return c;
}

UINT32 tlinkedlist::getindex(telem *elem)
{
    telem *search=first;
    UINT32 counter=0;
    while (search!=NULL)
    {
        if (search==elem) return counter;
        counter++;
        search=search->next;
    }
    return 0xFFFFFFFF;//not in the list
}




//////////////////////////////////////////////////////////////////////////////////////////
// BASIC elemENT STUFF INCLUDING MININAL 'SimpleList' ROUTINES
//


telem *SLcreateelement(long size)
{
    telem *newelem;
    newelem=(telem*)malloc(size);
    if (!newelem) return NULL;
    newelem->next=NULL;
    return newelem;
}

telem *SLadd(telem** prevelem,telem* newelem)
{
    /*if (*prevelem)    
      (*prevelem)->next=newelem;      
    else    
      *prevelem=newelem;    
    newelem->next=NULL;*/
    newelem->next=*prevelem;
    *prevelem=newelem;
    return newelem;
}



//Prev is acceptable as a paramter because you might have stored it elsewhere.
//if not the use SLPrev(first,current) to get the value.    
void SLremove(telem *prev,telem *relem)
{
    if (!relem) return;
    prev=relem->next;
    
    free(relem);
}

int SLremovelots(telem** first,linkcriteriafunction lfunc,linkcleanupfunction cfunc)
{
    if (!lfunc) return 0;
	int count=0;
    telem *ce,*prev;        
    ce=*first;
    prev=NULL;
    while (ce!=NULL)
    {                                           
          while (ce && lfunc(ce))
          {             
             if (cfunc)
               cfunc(ce);
			 count++;
             ce=ce->next;                              
          }
          if (prev)                               
            prev->next=ce;                    
          else
            *first=ce;            
          prev=ce;
          if (ce)
             ce=ce->next;                                   
     }
     return count;
}

    
telem *SLprev(telem *first,telem *current)
{
    if (first==NULL) return NULL;
    if (first==current) return NULL;
    while (first->next != current)
    {
        
        first=first->next;
        if (first==NULL) return NULL;
    }
    return first;    
}


void SLaddtoend(telem **first,telem *newe)
{
    if (*first)
       SLfindlast(*first)->next=newe;            
    else
       *first=newe;    
        
    newe->next=NULL;
}



telem *SLfindlast(telem *first)
{
    while (first->next!=NULL)  //find end of list        
        first=first->next;
    return first;
}

void SLdeletelist(telem *first)
{
    telem *next,*curr;
    curr=first;
    while (curr!=NULL)
    {
       next=curr->next;
       free(curr);
       curr=next;  
    }
}    


UINT32 SLgetlength(telem *first)
{
    UINT32 count=0;
    while (first!=NULL)
    {
       first=first->next;
       count++;
    }
    return count;    
}    

     //doesn't work :(               
telem *SLcountfrom(telem *start,UINT32 count)
{
    while ((start!=NULL)&&(count>0))
    {
       start=start->next;
       count--;     
    }
    return start;
}        
       


//////////////////////////////////////////////////////////////////////////////////////////
// TREE STUFF
//

void TTinittree(ttree *tree, UINT32 size)
{
    tree->count=0;
    tree->root=NULL;
    tree->size=size;
}

//gets previous node at the same depth as 'node'
ttreenode *TTgetprevnode(ttree *tree,ttreenode *node)
{
  ttreenode *currnode;
    
  if ((node == NULL)) return NULL;  
  if (node->parent!=NULL)
  {
     if (node==node->parent->child) return NULL;
     currnode=node->parent->child;
  }
  else
     currnode=tree->root;
     
  while ( (currnode != NULL )&&(currnode->next != node))
    currnode=currnode->next;
  return currnode;
}



ttreenode *TTgetlastnode(ttreenode *node)
{     
  while ((node->next != NULL))
    node=node->next;
  return node;
}

ttreenode *TTcreatettreenode(ttree *tree)
{
  ttreenode *newnode;
  newnode=(ttreenode*)malloc(tree->size);
  if (!newnode) return NULL;
  newnode->next = newnode->parent = newnode->child = NULL; 
  return newnode;
}

UINT32 TTgetdepth(ttreenode *node)
{
    UINT32 depth=0;
    while (node!=NULL)
    {
        depth++;
        node=node->parent;
    }
    return depth;
}

ttreenode *TTcreatettreenodeexpsize(int size)
{
  ttreenode *newnode;
  newnode=(ttreenode*)malloc(size);
  if (!newnode) return NULL;
  newnode->next = newnode->parent = newnode->child = NULL; 
  return newnode;
}

//To add siblings, repeatedly add a child to the same parent, to add to the root, specifiy parent as NULL

ttreenode *TTaddchildtonode(ttree *tree,ttreenode *parent,ttreenode *child)
{  
  tree->count++;
  if (parent==NULL)
  {                      //if adding to the root of the tree, do this
    if (tree->root==NULL)
      tree->root=child;
    else       
      TTgetlastnode(tree->root)->next=child;    
  } else if (parent->child == NULL)     //if not the root than do this.
    parent->child=child;
  else 
    TTgetlastnode(parent->child)->next=child;   
    
  child->next=NULL; 
  child->parent=parent;
  return child;
}

// Traverses in this order: Child, Sibling then Parent. (returns NULL if the root has been reached)
ttreenode *TTtraversetonext(ttreenode *currnode,ttreenode *stopat)
{
  if (currnode->child!=NULL)
      currnode=currnode->child;
  else if (currnode->next != NULL)
  {
      //traverse horizontaly or get the hell out.
      if (currnode==stopat) return NULL; 
      currnode=currnode->next;
  }    
  else
  {                                           //traverse upwards (if parent is last in it's list
      while (TRUE)
      {                                  //this code checks for that and go to it's parent (if 'stopat' is reached, terminate)
          currnode=currnode->parent;
          if (!currnode) return NULL;
          if (currnode==stopat) return NULL;
      
          if (currnode->next!=NULL)
          {
              currnode=currnode->next;
              break; 
          }        
      }  
  }
  return(currnode);
}

void TTremovenode(ttree *tree,ttreenode *node)
{
    if (node==NULL) return;
    ttreenode *prev=TTgetprevnode(tree,node);
    if (prev)
      prev->next=node->next;
    else if (node->parent!=NULL)
      node->parent->child=node->next;
    else if (node==tree->root)
      tree->root=node->next;
	tree->count--;
}

void TTdestroytree(ttreenode *node)
{
    if (node)
    {
    if (node->child!=NULL)
      TTdestroytree(node->child);
    if (node->next!=NULL)
      TTdestroytree(node->next);
    delete node;
    }
}

//////////////////////////////////////////////////
//theapmem object

char *theapmem::create(UINT32 size_)
{    
    base = new char[size_];
    size=(UINT32)(base+size_);
    curr=base;
    return base;
}

void theapmem::reset()
{
    curr=base;
}

void theapmem::destroy()
{
    if (base)
      delete base;
}

void *theapmem::grabandsetpointer(void *indata)
{
   void **data=(void **)grab(4);
   if (data)
   {
     *data=indata;
     return (void*)data;
   }else
     return NULL;
     
}

void *theapmem::getnextpointer()
{
    return curr;
}

char *theapmem::grab(UINT32 size_)
{
   char *retptr=curr;
   curr+=size_;   
   return ((UINT32)curr<=size)?retptr:NULL;
}

char *theapmem::ungrab(UINT32 size_)
{
     curr-=size_;      
     return curr;
} 


int envcount=0;
envvar envstrings[MAX_ENVSTRINGS];

void setenvstring(char *name,char *data)
{
	if (envcount<MAX_ENVSTRINGS)
	{
		strcpy(envstrings[envcount].envname,name);
		envstrings[envcount].data=data;
		envstrings[envcount].datalen=strlen(data);
		envcount++;
	}
}

void sortenvlist()
{
}

