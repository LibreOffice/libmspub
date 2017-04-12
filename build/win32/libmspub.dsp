# Microsoft Developer Studio Project File - Name="libmspub" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libmspub - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libmspub.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libmspub.mak" CFG="libmspub - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libmspub - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libmspub - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libmspub - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /I "libwpd-0.9" /D "WIN32" /D "NDEBUG" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "libwpd-0.9" /D "NDEBUG" /D "WIN32" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\lib\libmspub-0.0.lib"

!ELSEIF  "$(CFG)" == "libmspub - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "libwpd-0.9" /D "WIN32" /D "_DEBUG" /D "DEBUG" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /GR /GX /ZI /Od /I "libwpd-0.9" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\lib\libmspub-0.0.lib"

!ENDIF 

# Begin Target

# Name "libmspub - Win32 Release"
# Name "libmspub - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\lib\ColorReference.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\Dash.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\Fill.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\libmspub_utils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBCollector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBDocument.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBParser2k.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBParser97.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBSVGGenerator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\PolygonUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\Shadow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\ShapeGroupElement.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\VectorTransformation2D.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\inc\libmspub\libmspub.h
# End Source File
# Begin Source File

SOURCE=..\..\inc\libmspub\MSPUBDocument.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\Arrow.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\BorderArtInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\ColorReference.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\Coordinate.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\Dash.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\EmbeddedFontInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\EscherContainerType.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\EscherFieldIds.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\Fill.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\FillType.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\libmspub_utils.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\Line.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\ListInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\Margins.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBBlockID.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBBlockType.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBCollector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBConstants.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBContentChunkType.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBParser.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBParser2k.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBParser97.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBSVGGenerator.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\MSPUBTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\NumberingDelimiter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\NumberingType.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\PolygonUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\Shadow.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\ShapeFlags.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\ShapeGroupElement.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\ShapeInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\Shapes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\ShapeType.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\TableInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\VectorTransformation2D.h
# End Source File
# Begin Source File

SOURCE=..\..\src\lib\VerticalAlign.h
# End Source File
# End Group
# End Target
# End Project
