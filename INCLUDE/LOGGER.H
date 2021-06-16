//Stubs for log generation
#ifndef _logger_h_
#define _logger_h_


extern char buf[255];
extern void (*logmessage)(void* data,const char *format,...);
void LOGinit(void (*logmessagefunc)(void* data,const char *format,...));

#endif
