# Microsoft Developer Studio Generated NMAKE File, Based on mod_vhost_alias.dsp
!IF "$(CFG)" == ""
CFG=mod_vhost_alias - Win32 Release
!MESSAGE No configuration specified. Defaulting to mod_vhost_alias - Win32\
 Release.
!ENDIF 

!IF "$(CFG)" != "mod_vhost_alias - Win32 Release" && "$(CFG)" !=\
 "mod_vhost_alias - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mod_vhost_alias.mak" CFG="mod_vhost_alias - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mod_vhost_alias - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mod_vhost_alias - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "mod_vhost_alias - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\mod_vhost_alias.so"

!ELSE 

ALL : "ApacheCore - Win32 Release" "$(OUTDIR)\mod_vhost_alias.so"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ApacheCore - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\mod_vhost_alias.idb"
	-@erase "$(INTDIR)\mod_vhost_alias.obj"
	-@erase "$(OUTDIR)\mod_vhost_alias.exp"
	-@erase "$(OUTDIR)\mod_vhost_alias.lib"
	-@erase "$(OUTDIR)\mod_vhost_alias.map"
	-@erase "$(OUTDIR)\mod_vhost_alias.so"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /O2 /I "..\..\include" /I "..\..\os\win32" /D "NDEBUG"\
 /D "WIN32" /D "_WINDOWS" /D "SHARED_MODULE" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\mod_vhost_alias" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mod_vhost_alias.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\mod_vhost_alias.pdb" /map:"$(INTDIR)\mod_vhost_alias.map"\
 /machine:I386 /out:"$(OUTDIR)\mod_vhost_alias.so"\
 /implib:"$(OUTDIR)\mod_vhost_alias.lib" /base:@"BaseAddr.ref",mod_vhost_alias 
LINK32_OBJS= \
	"$(INTDIR)\mod_vhost_alias.obj" \
	"..\..\Release\ApacheCore.lib"

"$(OUTDIR)\mod_vhost_alias.so" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mod_vhost_alias - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\mod_vhost_alias.so"

!ELSE 

ALL : "ApacheCore - Win32 Debug" "$(OUTDIR)\mod_vhost_alias.so"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ApacheCore - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\mod_vhost_alias.idb"
	-@erase "$(INTDIR)\mod_vhost_alias.obj"
	-@erase "$(OUTDIR)\mod_vhost_alias.exp"
	-@erase "$(OUTDIR)\mod_vhost_alias.lib"
	-@erase "$(OUTDIR)\mod_vhost_alias.map"
	-@erase "$(OUTDIR)\mod_vhost_alias.pdb"
	-@erase "$(OUTDIR)\mod_vhost_alias.so"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /I "..\..\include" /I "..\..\os\win32" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "SHARED_MODULE" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\mod_vhost_alias" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mod_vhost_alias.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\mod_vhost_alias.pdb" /map:"$(INTDIR)\mod_vhost_alias.map"\
 /debug /machine:I386 /out:"$(OUTDIR)\mod_vhost_alias.so"\
 /implib:"$(OUTDIR)\mod_vhost_alias.lib" /base:@"BaseAddr.ref",mod_vhost_alias 
LINK32_OBJS= \
	"$(INTDIR)\mod_vhost_alias.obj" \
	"..\..\Debug\ApacheCore.lib"

"$(OUTDIR)\mod_vhost_alias.so" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "mod_vhost_alias - Win32 Release" || "$(CFG)" ==\
 "mod_vhost_alias - Win32 Debug"

!IF  "$(CFG)" == "mod_vhost_alias - Win32 Release"

"ApacheCore - Win32 Release" : 
   cd "..\../..\src"
   $(MAKE) /$(MAKEFLAGS) /F ".\ApacheCore.mak" CFG="ApacheCore - Win32 Release"\
 
   cd ".\os\win32"

"ApacheCore - Win32 ReleaseCLEAN" : 
   cd "..\../..\src"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F ".\ApacheCore.mak"\
 CFG="ApacheCore - Win32 Release" RECURSE=1 
   cd ".\os\win32"

!ELSEIF  "$(CFG)" == "mod_vhost_alias - Win32 Debug"

"ApacheCore - Win32 Debug" : 
   cd "..\../..\src"
   $(MAKE) /$(MAKEFLAGS) /F ".\ApacheCore.mak" CFG="ApacheCore - Win32 Debug" 
   cd ".\os\win32"

"ApacheCore - Win32 DebugCLEAN" : 
   cd "..\../..\src"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F ".\ApacheCore.mak"\
 CFG="ApacheCore - Win32 Debug" RECURSE=1 
   cd ".\os\win32"

!ENDIF 

SOURCE=..\..\modules\standard\mod_vhost_alias.c
DEP_CPP_MOD_V=\
	"..\..\include\ap.h"\
	"..\..\include\ap_alloc.h"\
	"..\..\include\ap_config.h"\
	"..\..\include\ap_ctype.h"\
	"..\..\include\ap_ebcdic.h"\
	"..\..\include\ap_mmn.h"\
	"..\..\include\buff.h"\
	"..\..\include\hsregex.h"\
	"..\..\include\http_config.h"\
	"..\..\include\http_core.h"\
	"..\..\include\httpd.h"\
	"..\..\include\util_uri.h"\
	".\os.h"\
	".\readdir.h"\
	
NODEP_CPP_MOD_V=\
	"..\..\include\ap_config_auto.h"\
	"..\..\include\sfio.h"\
	

"$(INTDIR)\mod_vhost_alias.obj" : $(SOURCE) $(DEP_CPP_MOD_V) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

