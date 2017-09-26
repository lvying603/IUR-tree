# Microsoft Developer Studio Project File - Name="IR2_Tree" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=IR2_Tree - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IR2_Tree.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IR2_Tree.mak" CFG="IR2_Tree - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IR2_Tree - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "IR2_Tree - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IR2_Tree - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "IR2_Tree - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "IR2_Tree - Win32 Release"
# Name "IR2_Tree - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\addinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Delete.cpp
# End Source File
# Begin Source File

SOURCE=.\infoadd.cpp
# End Source File
# Begin Source File

SOURCE=.\IR2_Tree.cpp
# End Source File
# Begin Source File

SOURCE=.\IR2_Tree.rc
# End Source File
# Begin Source File

SOURCE=.\IR2_TreeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MEM_IUR_Tree.cpp
# End Source File
# Begin Source File

SOURCE=.\MEM_ODE_CIUR.cpp
# End Source File
# Begin Source File

SOURCE=.\ODE_CIUR1.cpp
# End Source File
# Begin Source File

SOURCE=.\ODE_TE.cpp
# End Source File
# Begin Source File

SOURCE=.\query.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TE_CIUR.cpp
# End Source File
# Begin Source File

SOURCE=.\tesebutton.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\addinfo.h
# End Source File
# Begin Source File

SOURCE=.\Delete.h
# End Source File
# Begin Source File

SOURCE=.\infoadd.h
# End Source File
# Begin Source File

SOURCE=.\IR2_Tree.h
# End Source File
# Begin Source File

SOURCE=.\IR2_TreeDlg.h
# End Source File
# Begin Source File

SOURCE=.\IR2tree.h
# End Source File
# Begin Source File

SOURCE=.\MEM_IUR_Tree.h
# End Source File
# Begin Source File

SOURCE=.\MEM_ODE_CIUR.h
# End Source File
# Begin Source File

SOURCE=.\ODE_CIUR.H
# End Source File
# Begin Source File

SOURCE=.\ODE_CIUR1.h
# End Source File
# Begin Source File

SOURCE=.\ODE_TE.h
# End Source File
# Begin Source File

SOURCE=.\query.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TE_CIUR.h
# End Source File
# Begin Source File

SOURCE=.\tesebutton.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap7.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bupu1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bupu3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cx2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\down.bmp
# End Source File
# Begin Source File

SOURCE=.\res\eagle.bmp
# End Source File
# Begin Source File

SOURCE=.\res\eagle2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\IR2_Tree.ico
# End Source File
# Begin Source File

SOURCE=.\res\IR2_Tree.rc2
# End Source File
# Begin Source File

SOURCE=.\res\pubu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\query.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
