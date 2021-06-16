//header for all the routines that load and save 3d files
//These load scenes
bool T3Dloadnewclay(char *filename,tscene *scene);

//These load single objects
bool T3Dloadmr(char *filename,tscene *scene);
bool T3Dloadlw(char *filename,tscene *scene);
bool T3Dloadvrml(char *filename,tscene *scene);
bool T3Dloadclay(char *filename,tscene *scene);
bool T3Dloadmd3(char *filename,tscene *scene);
bool T3Dloaddxf(char *filename,tscene *scene);
bool T3Dload3ds(char *filename,tscene *scene);
bool T3Dloadplg(char *filename,tscene *scene);
bool T3Dloadoff(char *filename,tscene *scene);

//These save scenes 
bool T3Dsavenewclay(char *filename,tscene *scene);
bool T3Dsavejava(char *filename,tscene *scene);

//These save single objects
bool T3Dsavemr(char *filename,t3dmesh *obj);
bool T3Dsavelw(char *filename,t3dmesh *obj);
bool T3Dsavevrml(char *filename,t3dmesh *obj);
bool T3Dsaveclay(char *filename,t3dmesh *obj);
bool T3Dsavedxf(char *filename,t3dmesh *obj);
bool T3Dsave3ds(char *filename,t3dmesh *obj);
bool T3Dsavexeios(char *filename,tscene *scene);


