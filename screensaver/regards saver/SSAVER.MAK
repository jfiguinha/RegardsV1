# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=SSAVER32 - Win32 Release
!MESSAGE No configuration specified.  Defaulting to SSAVER32 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "SSAVER32 - Win32 Release" && "$(CFG)" !=\
 "SSAVER32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "SSaver.MAK" CFG="SSAVER32 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SSAVER32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SSAVER32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "SSAVER32 - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "SSAVER32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "WinRel"
OUTDIR=.
INTDIR=.\WinRel

ALL : "$(OUTDIR)\WinRel\Simple Saver.SCR"

CLEAN : 
	-@erase "$(INTDIR)\DOFRAME.OBJ"
	-@erase "$(INTDIR)\ssaver.obj"
	-@erase "$(INTDIR)\ssaver.res"
	-@erase "$(OUTDIR)\WinRel\Simple Saver.SCR"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /G3 /W3 /Ox /Oi /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_WINNT" /FR /YX /c
# ADD CPP /nologo /G3 /W3 /O1 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_WINNT" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /G3 /ML /W3 /O1 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_X86_" /D "_WINNT" /Fp"$(INTDIR)/SSaver.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/ssaver.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SSaver.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 scrnsave.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /entry:"main" /subsystem:windows /machine:IX86
# ADD LINK32 scrnsave.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:IX86 /out:"WinRel\Simple Saver.SCR"
LINK32_FLAGS=scrnsave.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Simple Saver.pdb" /machine:IX86 /def:".\SSaver.DEF"\
 /out:"$(OUTDIR)/WinRel\Simple Saver.SCR" 
DEF_FILE= \
	".\SSaver.DEF"
LINK32_OBJS= \
	"$(INTDIR)\DOFRAME.OBJ" \
	"$(INTDIR)\ssaver.obj" \
	"$(INTDIR)\ssaver.res"

"$(OUTDIR)\WinRel\Simple Saver.SCR" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SSAVER32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)\Simple Saver.SCR"

CLEAN : 
	-@erase "$(INTDIR)\DOFRAME.OBJ"
	-@erase "$(INTDIR)\ssaver.obj"
	-@erase "$(INTDIR)\ssaver.res"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Simple Saver.ilk"
	-@erase "$(OUTDIR)\Simple Saver.pdb"
	-@erase "$(OUTDIR)\Simple Saver.SCR"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /Zp4 /W3 /Zi /O2 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "_WINNT" /FR /YX /c
# ADD CPP /nologo /Zp4 /W3 /Gm /Zi /O2 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_X86_" /D "_WINNT" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /Zp4 /MLd /W3 /Gm /Zi /O2 /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_X86_" /D "_WINNT" /Fp"$(INTDIR)/SSaver.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/ssaver.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SSaver.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 scrnsave.lib olecli32.lib olesvr32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /entry:"main" /subsystem:windows /debug /machine:IX86
# ADD LINK32 scrnsave.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:IX86 /out:"WinDebug\Simple Saver.SCR"
LINK32_FLAGS=scrnsave.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Simple Saver.pdb" /debug /machine:IX86 /def:".\SSaver.DEF"\
 /out:"$(OUTDIR)/Simple Saver.SCR" 
DEF_FILE= \
	".\SSaver.DEF"
LINK32_OBJS= \
	"$(INTDIR)\DOFRAME.OBJ" \
	"$(INTDIR)\ssaver.obj" \
	"$(INTDIR)\ssaver.res"

"$(OUTDIR)\Simple Saver.SCR" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "SSAVER32 - Win32 Release"
# Name "SSAVER32 - Win32 Debug"

!IF  "$(CFG)" == "SSAVER32 - Win32 Release"

!ELSEIF  "$(CFG)" == "SSAVER32 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ssaver.c
DEP_CPP_SSAVE=\
	".\doframe.h"\
	

"$(INTDIR)\ssaver.obj" : $(SOURCE) $(DEP_CPP_SSAVE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ssaver.RC
DEP_RSC_SSAVER=\
	".\icon1.ico"\
	

"$(INTDIR)\ssaver.res" : $(SOURCE) $(DEP_RSC_SSAVER) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\DOFRAME.C

"$(INTDIR)\DOFRAME.OBJ" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\SSaver.DEF

!IF  "$(CFG)" == "SSAVER32 - Win32 Release"

!ELSEIF  "$(CFG)" == "SSAVER32 - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
