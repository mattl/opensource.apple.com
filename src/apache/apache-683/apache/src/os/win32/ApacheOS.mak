# Microsoft Developer Studio Generated NMAKE File, Based on ApacheOS.dsp
!IF "$(CFG)" == ""
CFG=ApacheOS - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ApacheOS - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ApacheOS - Win32 Release" && "$(CFG)" != "ApacheOS - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ApacheOS.mak" CFG="ApacheOS - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ApacheOS - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ApacheOS - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ApacheOS - Win32 Release"

OUTDIR=.\LibR
INTDIR=.\LibR
# Begin Custom Macros
OutDir=.\LibR
# End Custom Macros

ALL : "$(OUTDIR)\ApacheOS.lib"


CLEAN :
	-@erase "$(INTDIR)\ApacheOS.idb"
	-@erase "$(INTDIR)\ApacheOS.pdb"
	-@erase "$(INTDIR)\os.obj"
	-@erase "$(OUTDIR)\ApacheOS.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\ApacheOS" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ApacheOS.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\ApacheOS.lib" 
LIB32_OBJS= \
	"$(INTDIR)\os.obj"

"$(OUTDIR)\ApacheOS.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ApacheOS - Win32 Debug"

OUTDIR=.\LibD
INTDIR=.\LibD
# Begin Custom Macros
OutDir=.\LibD
# End Custom Macros

ALL : "$(OUTDIR)\ApacheOS.lib"


CLEAN :
	-@erase "$(INTDIR)\ApacheOS.idb"
	-@erase "$(INTDIR)\ApacheOS.pdb"
	-@erase "$(INTDIR)\os.obj"
	-@erase "$(OUTDIR)\ApacheOS.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\ApacheOS" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ApacheOS.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\ApacheOS.lib" 
LIB32_OBJS= \
	"$(INTDIR)\os.obj"

"$(OUTDIR)\ApacheOS.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ApacheOS.dep")
!INCLUDE "ApacheOS.dep"
!ELSE 
!MESSAGE Warning: cannot find "ApacheOS.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ApacheOS - Win32 Release" || "$(CFG)" == "ApacheOS - Win32 Debug"
SOURCE=.\os.c

"$(INTDIR)\os.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

