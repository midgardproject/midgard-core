# Microsoft Developer Studio Project File - Name="lib_midgard" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=lib_midgard - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "lib_midgard.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "lib_midgard.mak" CFG="lib_midgard - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "lib_midgard - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "lib_midgard - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "lib_midgard"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lib_midgard - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\tmp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIB_MIDGARD_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\\" /I "..\midgard" /I "..\..\build\expat\lib" /I "..\..\build\glib" /I "..\..\build\glib\glib" /I "..\..\build\iconv\include" /I "..\..\build\libmysql" /D "HAVE_MIDGARD_SITEGROUPS" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIB_MIDGARD_EXPORTS" /YX /FD /D G_LOG_DOMAIN=\"midgard-lib\" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libexpat.lib glib-2.0.lib iconv.lib libmysql.lib ws2_32.lib /nologo /dll /machine:I386 /libpath:"..\..\build\expat\lib\Release" /libpath:"..\..\build\glib\glib" /libpath:"..\..\build\iconv\lib" /libpath:"..\..\build\libmysql\Release" /opt:noref
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "lib_midgard - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\tmp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIB_MIDGARD_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\midgard" /I "..\..\build\expat\lib" /I "..\..\build\glib" /I "..\..\build\glib\glib" /I "..\..\build\iconv\include" /I "..\..\build\libmysql" /D "HAVE_MIDGARD_SITEGROUPS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIB_MIDGARD_EXPORTS" /YX /FD /GZ /D G_LOG_DOMAIN=\"midgard-lib\" /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libexpat.lib glib-2.0.lib iconv.lib libmysql.lib ws2_32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\build\expat\lib\Debug" /libpath:"..\..\build\glib\glib" /libpath:"..\..\build\iconv\lib" /libpath:"..\..\build\libmysql\Debug" /opt:noref
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "lib_midgard - Win32 Release"
# Name "lib_midgard - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\access.c
# End Source File
# Begin Source File

SOURCE=..\src\authfailure.c
# End Source File
# Begin Source File

SOURCE=..\src\filetemplate.c
# End Source File
# Begin Source File

SOURCE=.\flock.c
# End Source File
# Begin Source File

SOURCE=..\src\fmt_russian.c
# End Source File
# Begin Source File

SOURCE=..\src\format.c
# End Source File
# Begin Source File

SOURCE=..\src\iconvxml.c
# End Source File
# Begin Source File

SOURCE=.\lib_midgard.def
# End Source File
# Begin Source File

SOURCE=..\src\md5.c
# End Source File
# Begin Source File

SOURCE=..\src\midgard.c
# End Source File
# Begin Source File

SOURCE=..\src\pageresolve.c
# End Source File
# Begin Source File

SOURCE=..\src\preparse.c
# End Source File
# Begin Source File

SOURCE=..\src\repligard.c
# End Source File
# Begin Source File

SOURCE=..\src\tablenames.c
# End Source File
# Begin Source File

SOURCE=..\src\tree.c
# End Source File
# Begin Source File

SOURCE=..\src\tree_core.c
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\midgard\authfailure.h
# End Source File
# Begin Source File

SOURCE=..\src\defaults.h
# End Source File
# Begin Source File

SOURCE=.\flock.h
# End Source File
# Begin Source File

SOURCE=..\src\fmt_russian.h
# End Source File
# Begin Source File

SOURCE=..\midgard\internal.h
# End Source File
# Begin Source File

SOURCE=..\src\md5.h
# End Source File
# Begin Source File

SOURCE=..\midgard\midgard.h
# End Source File
# Begin Source File

SOURCE=..\midgard\pageresolve.h
# End Source File
# Begin Source File

SOURCE=..\midgard\parser.h
# End Source File
# Begin Source File

SOURCE=..\midgard\parsers.h
# End Source File
# Begin Source File

SOURCE=..\midgard\tablenames.h
# End Source File
# Begin Source File

SOURCE=.\win95nt.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\lib_midgard.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# End Target
# End Project
