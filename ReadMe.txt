========================================================================
       WIN32 APPLICATION : winclay
========================================================================

This is the initial port of Clayworks 3.0 for DOS in to windows. 
The structure of the code follows the original Pascal DOS 2.* series quite closely still
and the project still holds on to its software rendering roots, requiring DirectDraw to 
render the UI, whilst the 3D windows use Direct3D.
As a result, this version runs poorly on new versions of windows, as these do not support
DirectDraw's style of direct framebuffer access. 

This is the version I maintained whilst working at A.I. Cube in Tokyo, the company responsible for the engine
behind 'Gitaroo man'. This version of Clayworks can export scenefiles to the Xeios engine.
