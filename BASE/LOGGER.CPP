#include "logger.h"
#include <stdio.h>

char buf[255];

void defaultlogger(void *data,const char *format,...)
{
	printf(format);
}

void (*logmessage)(void* data,const char *format,...)=defaultlogger;

void LOGinit(void (*logmessagefunc)(void* data,const char *format,...))
{
    logmessage=logmessagefunc;    
}



 

