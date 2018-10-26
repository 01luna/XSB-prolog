# Microsoft Developer Studio Generated NMAKE File, Based on cmain.dsp
!IF "$(CFG)" == ""
CFG=cmain - Win64
!MESSAGE No configuration specified. Defaulting to cmain - Win64.
!ENDIF 

!IF "$(CFG)" != "cmain - Win32" && "$(CFG)" != "cmain - Win64"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cmain.mak" CFG="cmain - Win64"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cmain - Win32" (based on "Win32 (x86) Console Application")
!MESSAGE "cmain - Win64" (based on "Win64 (x64) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cmain - Win32"

OUTDIR=.
INTDIR=.
# Begin Custom Macros
OutDir=.
# End Custom Macros

ALL : "$(OUTDIR)\cmain.exe"


CLEAN :
	-@erase "$(INTDIR)\cmain.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\cmain.exe"

CPP_PROJ=/nologo /W3 /EHsc /O2 /I "..\..\emu" /I "..\..\config\x86-pc-windows" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\cmain.pch" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cmain.bsc" 
BSC32_SBRS= \
	
LINKER=link.exe
LINKER_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib xsb.lib /nologo /subsystem:console /pdb:"$(OUTDIR)\cmain.pdb" /machine:I386 /out:"$(OUTDIR)\cmain.exe" /libpath:"..\..\config\x86-pc-windows\bin" 
LINKER_OBJS= \
	"$(INTDIR)\cmain.obj"

"$(OUTDIR)\cmain.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINKER_OBJS)
    $(LINKER) @<<
  $(LINKER_FLAGS) $(LINKER_OBJS)
<<

!ELSEIF  "$(CFG)" == "cmain - Win64"

OUTDIR=.
INTDIR=.
# Begin Custom Macros
OutDir=.
# End Custom Macros

ALL : "$(OUTDIR)\cmain.exe"


CLEAN :
	-@erase "$(INTDIR)\cmain.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\cmain.exe"
	-@erase "$(OUTDIR)\cmain.ilk"
	-@erase "$(OUTDIR)\cmain.pdb"

CPP_PROJ=/nologo /W3 /EHsc /O2 /I "..\..\emu" /I "..\..\config\x64-pc-windows" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\cmain.pch" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cmain.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINKER_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib xsb.lib /nologo /subsystem:console /pdb:"$(OUTDIR)\cmain.pdb" /machine:x64 /out:"$(OUTDIR)\cmain.exe" /libpath:"..\..\config\x64-pc-windows\bin" 
LINKER_OBJS= \
	"$(INTDIR)\cmain.obj"

"$(OUTDIR)\cmain.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINKER_OBJS)
    $(LINK32) @<<
  $(LINKER_FLAGS) $(LINKER_OBJS)
<<

!ENDIF 

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


!IF EXISTS("cmain.dep")
!INCLUDE "cmain.dep"
!ELSE 
!MESSAGE No external dependencies specified.
!ENDIF 


!IF "$(CFG)" == "cmain - Win32" || "$(CFG)" == "cmain - Win64"
SOURCE=cmain.c

"$(INTDIR)\cmain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

