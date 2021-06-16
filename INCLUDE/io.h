#ifndef io_h_
#define io_h_


char lowcase(char str);
char upcase(char str);
void upcase(char *str);
void lowcase(char *str);
char *getpathanddrive(char *fullpath,char *justpath);
char *getname(char *fullpath);
char *getpath(char *fullpath,char *justpath);
char *getextension( char *name );
char *makepath(char *outtext,char *intext);
bool fexist(char *filename);
UINT64 fsize(char *filename);
UINT32 fsize(FILE *f);

#endif