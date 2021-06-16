========================================================================
       WIN32 APPLICATION : winclay
========================================================================

This is the initial port of Clayworks 3.0 for DOS in to windows. 
The structure of the code follows the original Pascal DOS 2.* series quite closely still
and the project still holds on to its software rendering roots, requiring DirectDraw to 
render the UI. This was pretty much the end of the road for my software rendering engine in C++,
although I did revive it in Java and wrote a number of games using that.
This version runs quite poorly on new versions of windows, as these do not support DirectDraw's style 
of direct framebuffer access very well and as a result, the UI redraw speed is a bit sluggish.
The engine renders in 565 16bit colour mode so in order to get the bundled exe to work correctly, 
try right clicking on the exe, select 'properties' then 'App Compatability' and then select 'Reduce Colour Mode > 16bit(65536) colour'

This is the version I maintained whilst working at A.I. Cube in Tokyo, the company responsible for the engine
behind 'Gitaroo man'. This version of Clayworks can export scenefiles to the Xeios engine.
