# Microsoft Developer Studio Project File - Name="SDL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SDL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SDL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SDL.mak" CFG="SDL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SDL - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SDL - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SDL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /w /W0 /GX /O2 /I "..\..\..\extlib\src\SDL-1.2.13\include" /I "." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D DECLSPEC="" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SDL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\extlib\src\SDL-1.2.13\include" /I "." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D DECLSPEC="" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "SDL - Win32 Release"
# Name "SDL - Win32 Debug"
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\include\MSD_video.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\include\SDL.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\include\SDL_common.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\include\SDL_image.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\include\SDL_main.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\include\SDL_syswm.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\include\SDL_ttf.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\include\SDLevent.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\include\SDLvideo.h"
# End Source File
# End Group
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\src\MSD_blit.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\src\MSD_blit_0.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\src\MSD_blit_1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\src\MSD_blit_A.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\src\MSD_blit_N.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\src\MSD_bmp.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\src\MSD_pixels.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\src\MSD_rwops.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\src\MSD_stretch.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\src\MSD_surface.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\src\SDL.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\src\SDL_image.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\src\SDL_ttf.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\src\SDLevent.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\extlib\src\SDL-1.2.13\src\SDLvideo.c"
# End Source File
# End Group
# End Target
# End Project
