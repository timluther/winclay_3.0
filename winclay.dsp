# Microsoft Developer Studio Project File - Name="winclay" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=winclay - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "winclay.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "winclay.mak" CFG="winclay - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "winclay - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "winclay - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "winclay - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "d:\code\winclay\include" /I "c:\prog\c\winclay\include" /I "include" /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "d:\code\winclay\include" /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib ddraw.lib dxguid.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "winclay - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "winclay___Win32_Debug"
# PROP BASE Intermediate_Dir "winclay___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "d:\code\winclay\include" /I "c:\prog\c\winclay\include" /I "include" /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "d:\code\winclay\include\\" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib ddraw.lib dxguid.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "winclay - Win32 Release"
# Name "winclay - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "TWIN"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Twin\Dialogs.cpp
# End Source File
# Begin Source File

SOURCE=.\Include\Dialogs.h
# End Source File
# Begin Source File

SOURCE=.\Twin\Views.cpp
# End Source File
# Begin Source File

SOURCE=.\Include\Views.h
# End Source File
# Begin Source File

SOURCE=.\Twin\Widgets.cpp
# End Source File
# Begin Source File

SOURCE=.\Include\Widgets.h
# End Source File
# End Group
# Begin Group "2d render"

# PROP Default_Filter ""
# Begin Group "headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\2DRENDER\context.h
# End Source File
# Begin Source File

SOURCE=.\2DRENDER\directxcontext.h
# End Source File
# Begin Source File

SOURCE=.\2drender\font.h
# End Source File
# Begin Source File

SOURCE=.\2DRENDER\Gbasics.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\graphics.h
# End Source File
# Begin Source File

SOURCE=.\2drender\raster2d.h
# End Source File
# Begin Source File

SOURCE=.\2DRENDER\surface.h
# End Source File
# Begin Source File

SOURCE=.\2DRENDER\Svga64k.h
# End Source File
# End Group
# Begin Group "image loaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\2drender\BMP.CPP
# End Source File
# Begin Source File

SOURCE=.\2drender\COLOUR.CPP
# End Source File
# Begin Source File

SOURCE=.\2DRENDER\Imgloadr.cpp
# End Source File
# Begin Source File

SOURCE=.\2DRENDER\Imgloadr.h
# End Source File
# Begin Source File

SOURCE=.\2DRENDER\imx.cpp
# End Source File
# Begin Source File

SOURCE=.\2DRENDER\JPG.CPP
# End Source File
# Begin Source File

SOURCE=.\2drender\PCX256.CPP
# End Source File
# Begin Source File

SOURCE=.\2DRENDER\TGA.CPP
# End Source File
# End Group
# Begin Source File

SOURCE=.\2DRENDER\colourconv.cpp
# End Source File
# Begin Source File

SOURCE=.\2drender\context.cpp
# End Source File
# Begin Source File

SOURCE=.\2drender\font.cpp
# End Source File
# Begin Source File

SOURCE=.\2drender\Gbasics.cpp
# End Source File
# Begin Source File

SOURCE=.\2drender\raster2d.cpp
# End Source File
# Begin Source File

SOURCE=.\2DRENDER\surface.cpp
# End Source File
# Begin Source File

SOURCE=.\2DRENDER\SVGA24.CPP
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\2DRENDER\SVGA256.CPP
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\2drender\SVGA32.CPP
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\2drender\Svga64k.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\2DRENDER\tables.cpp
# End Source File
# End Group
# Begin Group "core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IO\io.cpp
# End Source File
# Begin Source File

SOURCE=.\base\logger.cpp
# End Source File
# Begin Source File

SOURCE=.\base\TMATHS.CPP
# End Source File
# Begin Source File

SOURCE=.\base\TTYPES.CPP
# End Source File
# Begin Source File

SOURCE=.\Include\Ttypes.h
# End Source File
# End Group
# Begin Group "scenegraph"

# PROP Default_Filter ""
# Begin Group "3d render"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\3drender\R3d565.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\3drender\Trender.cpp
# End Source File
# Begin Source File

SOURCE=.\include\TRENDER.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\scenegraph\3DFILE.CPP
# End Source File
# Begin Source File

SOURCE=.\scenegraph\3DFILE.H
# End Source File
# Begin Source File

SOURCE=.\scenegraph\BASIC3D.CPP
# End Source File
# Begin Source File

SOURCE=.\Include\Basic3d.h
# End Source File
# Begin Source File

SOURCE=.\Editscn.cpp
# End Source File
# Begin Source File

SOURCE=.\Include\Editscn.h
# End Source File
# End Group
# Begin Group "clayworks"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=.\2DRENDER\GPRIM.CPP
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\IO\MSMOUSE.CPP
# End Source File
# Begin Source File

SOURCE=.\mstwin.cpp
# End Source File
# Begin Source File

SOURCE=.\Perlin.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\Strtable.cpp
# End Source File
# Begin Source File

SOURCE=.\winclay.rc
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\small.ico
# End Source File
# Begin Source File

SOURCE=.\winclay.ico
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Include\Dosstuff.h
# End Source File
# Begin Source File

SOURCE=.\Include\Dskmemio.h
# End Source File
# Begin Source File

SOURCE=.\Include\Gprim.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\io.h
# End Source File
# Begin Source File

SOURCE=.\Include\Logger.h
# End Source File
# Begin Source File

SOURCE=.\Include\Msmouse.h
# End Source File
# Begin Source File

SOURCE=.\Include\Mstwin.h
# End Source File
# Begin Source File

SOURCE=.\Include\Pcx256.h
# End Source File
# Begin Source File

SOURCE=.\Include\Perlin.h
# End Source File
# Begin Source File

SOURCE=.\Include\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Include\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Include\Stdinc.h
# End Source File
# Begin Source File

SOURCE=.\Include\Strtable.h
# End Source File
# Begin Source File

SOURCE=.\Include\Tmaths.h
# End Source File
# Begin Source File

SOURCE=.\Include\Topengl.h
# End Source File
# Begin Source File

SOURCE=.\Include\Winclay.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
