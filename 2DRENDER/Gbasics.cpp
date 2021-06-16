//Graphical basics unit -defines points, rectangles, etc.


#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <math.h>
#include "ttypes.h"
#include "tmaths.h"
#include "gbasics.h"

//rectangle list stuff
theapmem rectbuffer;
UINT8 rectlist_status;
UINT32 rect_count;
trectlist *lastrect;

//Current Colours & writemode }
//there is a distinction between the remaped palette and the actual palette
//in all modes but 32 and 24 bit colour, the 8 channel rgb palettes are different
//and some functions need to know the rgb colours, rather than the 'native' mode
//colours.. a function will be provided to load up a palette from 8chan to native

///////////////////////////////////////////////////////////////////////////
//Trecttemplate



void destroyplist(trectlist *p)
{
    trectlist *curr_p,*next_p;
    curr_p=p;
    while (curr_p!=NULL)
    {
        next_p=curr_p->next;
        delete curr_p;
        curr_p=next_p;
    }
}

trectlist* getrectmem()
{
    return (trectlist*)rectbuffer.grab(sizeof(trectlist));
}



trectlist* ungetrectmem()
{
    return (trectlist*)rectbuffer.ungrab(sizeof(trectlist));
}



