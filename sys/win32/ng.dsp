# Microsoft Developer Studio Project File - Name="ng" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** 編集しないでください **

# TARGTYPE "Win32 (WCE x86em) Application" 0x0b01
# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (WCE MIPS) Application" 0x0a01
# TARGTYPE "Win32 (WCE SH) Application" 0x0901

CFG=ng - Win32 Debug NO UNICODE
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "ng.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "ng.mak" CFG="ng - Win32 Debug NO UNICODE"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "ng - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "ng - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE "ng - Win32 (WCE x86em) Release" ("Win32 (WCE x86em) Application" 用)
!MESSAGE "ng - Win32 (WCE x86em) Debug" ("Win32 (WCE x86em) Application" 用)
!MESSAGE "ng - Win32 (WCE MIPS) Release" ("Win32 (WCE MIPS) Application" 用)
!MESSAGE "ng - Win32 (WCE MIPS) Debug" ("Win32 (WCE MIPS) Application" 用)
!MESSAGE "ng - Win32 (WCE SH) Release" ("Win32 (WCE SH) Application" 用)
!MESSAGE "ng - Win32 (WCE SH) Debug" ("Win32 (WCE SH) Application" 用)
!MESSAGE "ng - Win32 Debug NO UNICODE" ("Win32 (x86) Application" 用)
!MESSAGE "ng - Win32 Release NO UNICODE" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/ng", MBAAAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "ng - Win32 Release"

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
F90=df.exe
# ADD BASE F90 /include:"Release/"
# ADD F90 /include:"Release/"
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "./sys/win32" /I "./sys/default" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "_UNICODE" /D "UNICODE" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
RSC=rc.exe
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib imm32.lib winmm.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE F90 /include:"Debug/"
# ADD F90 /include:"Debug/"
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "./sys/win32" /I "./sys/default" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "_UNICODE" /D "UNICODE" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
RSC=rc.exe
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib imm32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "x86emRel"
# PROP BASE Intermediate_Dir "x86emRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "x86emRel"
# PROP Intermediate_Dir "x86emRel"
# PROP Target_Dir ""
EMPFILE=empfile.exe
# ADD BASE EMPFILE -COPY
# ADD EMPFILE -COPY
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /subsystem:windows /machine:I386 /windowsce:emulation
# ADD LINK32 commctrl.lib coredll.lib /nologo /subsystem:windows /machine:I386 /windowsce:emulation
BSC32=bscmake.exe
# ADD BASE BSC32 /NOLOGO
# ADD BSC32 /NOLOGO
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
RSC=rc.exe
# ADD BASE RSC /l 0x411 /d UNDER_CE=$(CEVersion) /d "UNICODE" /d "_UNICODE" /d "WIN32" /d "STRICT" /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "_WIN32_WCE_EMULATION" /d "INTERNATIONAL" /d "USA" /d "INTLMSG_CODEPAGE" /d "NDEBUG"
# ADD RSC /l 0x411 /d UNDER_CE=$(CEVersion) /d "UNICODE" /d "_UNICODE" /d "WIN32" /d "STRICT" /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "_WIN32_WCE_EMULATION" /d "INTERNATIONAL" /d "USA" /d "INTLMSG_CODEPAGE" /d "NDEBUG"
CPP=cl.exe
# ADD BASE CPP /nologo /ML /W3 /O2 /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "STRICT" /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "_WIN32_WCE_EMULATION" /D "INTERNATIONAL" /D "USA" /D "INTLMSG_CODEPAGE" /D "NDEBUG" /D "x86" /D "i486" /D "_x86_" /YX /c
# ADD CPP /nologo /MT /W3 /O2 /I "./sys/win32" /I "./sys/default" /I "." /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "STRICT" /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "_WIN32_WCE_EMULATION" /D "INTERNATIONAL" /D "USA" /D "INTLMSG_CODEPAGE" /D "NDEBUG" /D "x86" /D "i486" /D "_x86_" /YX /c

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "x86emDbg"
# PROP BASE Intermediate_Dir "x86emDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "x86emDbg"
# PROP Intermediate_Dir "x86emDbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
EMPFILE=empfile.exe
# ADD BASE EMPFILE -COPY
# ADD EMPFILE -COPY
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /subsystem:windows /debug /machine:I386 /windowsce:emulation
# ADD LINK32 commctrl.lib coredll.lib /nologo /subsystem:windows /debug /machine:I386 /windowsce:emulation
BSC32=bscmake.exe
# ADD BASE BSC32 /NOLOGO
# ADD BSC32 /NOLOGO
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
RSC=rc.exe
# ADD BASE RSC /l 0x411 /d UNDER_CE=$(CEVersion) /d "UNICODE" /d "_UNICODE" /d "WIN32" /d "STRICT" /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "_WIN32_WCE_EMULATION" /d "INTERNATIONAL" /d "USA" /d "INTLMSG_CODEPAGE" /d "_DEBUG" /d "x86" /d "i486" /d "_x86_"
# ADD RSC /l 0x411 /d UNDER_CE=$(CEVersion) /d "UNICODE" /d "_UNICODE" /d "WIN32" /d "STRICT" /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "_WIN32_WCE_EMULATION" /d "INTERNATIONAL" /d "USA" /d "INTLMSG_CODEPAGE" /d "_DEBUG" /d "x86" /d "i486" /d "_x86_"
CPP=cl.exe
# ADD BASE CPP /nologo /MLd /W3 /Gm /Zi /Od /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "STRICT" /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "_WIN32_WCE_EMULATION" /D "INTERNATIONAL" /D "USA" /D "INTLMSG_CODEPAGE" /D "_DEBUG" /D "x86" /D "i486" /D "_x86_" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /Zi /Od /I "./sys/win32" /I "./sys/default" /I "." /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "STRICT" /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "_WIN32_WCE_EMULATION" /D "INTERNATIONAL" /D "USA" /D "INTLMSG_CODEPAGE" /D "_DEBUG" /D "x86" /D "i486" /D "_x86_" /YX /c

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WMIPSRel"
# PROP BASE Intermediate_Dir "WMIPSRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WMIPSRel"
# PROP Intermediate_Dir "WMIPSRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /machine:MIPS /subsystem:$(CESubsystem)
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib /nologo /machine:MIPS /subsystem:$(CESubsystem)
# SUBTRACT LINK32 /pdb:none /nodefaultlib
BSC32=bscmake.exe
# ADD BASE BSC32 /NOLOGO
# ADD BSC32 /NOLOGO
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
RSC=rc.exe
# ADD BASE RSC /l 0x411 /r /d "MIPS" /d "_MIPS_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "NDEBUG"
# ADD RSC /l 0x411 /r /d "MIPS" /d "_MIPS_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "NDEBUG"
CPP=clmips.exe
# ADD BASE CPP /nologo /ML /W3 /O2 /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "NDEBUG" /D "MIPS" /D "_MIPS_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /QMRWCE /c
# ADD CPP /nologo /MT /W3 /O2 /I "./sys/win32" /I "./sys/default" /I "." /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "NDEBUG" /D "MIPS" /D "_MIPS_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "STRICT" /YX /QMRWCE /c

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WMIPSDbg"
# PROP BASE Intermediate_Dir "WMIPSDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WMIPSDbg"
# PROP Intermediate_Dir "WMIPSDbg"
# PROP Target_Dir ""
PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /debug /machine:MIPS /subsystem:$(CESubsystem)
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib /nologo /debug /machine:MIPS /subsystem:$(CESubsystem)
# SUBTRACT LINK32 /pdb:none /nodefaultlib
BSC32=bscmake.exe
# ADD BASE BSC32 /NOLOGO
# ADD BSC32 /NOLOGO
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
RSC=rc.exe
# ADD BASE RSC /l 0x411 /r /d "MIPS" /d "_MIPS_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "DEBUG"
# ADD RSC /l 0x411 /r /d "MIPS" /d "_MIPS_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "DEBUG"
CPP=clmips.exe
# ADD BASE CPP /nologo /MLd /W3 /Zi /Od /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "MIPS" /D "_MIPS_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /QMRWCE /c
# ADD CPP /nologo /MTd /W3 /Zi /Od /I "./sys/win32" /I "./sys/default" /I "." /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "MIPS" /D "_MIPS_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "STRICT" /YX /QMRWCE /c

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WCESHRel"
# PROP BASE Intermediate_Dir "WCESHRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WCESHRel"
# PROP Intermediate_Dir "WCESHRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /machine:SH3 /subsystem:$(CESubsystem)
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib /nologo /machine:SH3 /subsystem:$(CESubsystem)
# SUBTRACT LINK32 /pdb:none /nodefaultlib
BSC32=bscmake.exe
# ADD BASE BSC32 /NOLOGO
# ADD BSC32 /NOLOGO
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
RSC=rc.exe
# ADD BASE RSC /l 0x411 /r /d "SHx" /d "SH3" /d "_SH3_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "NDEBUG"
# ADD RSC /l 0x411 /r /d "SHx" /d "SH3" /d "_SH3_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "NDEBUG"
CPP=shcl.exe
# ADD BASE CPP /nologo /ML /W3 /O2 /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "NDEBUG" /D "SHx" /D "SH3" /D "_SH3_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /c
# ADD CPP /nologo /MT /W3 /O2 /I "./sys/win32" /I "./sys/default" /I "." /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "NDEBUG" /D "SHx" /D "SH3" /D "_SH3_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "STRICT" /YX /c

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WCESHDbg"
# PROP BASE Intermediate_Dir "WCESHDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WCESHDbg"
# PROP Intermediate_Dir "WCESHDbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
PFILE=pfile.exe
# ADD BASE PFILE COPY
# ADD PFILE COPY
LINK32=link.exe
# ADD BASE LINK32 commctrl.lib coredll.lib /nologo /debug /machine:SH3 /subsystem:$(CESubsystem)
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 commctrl.lib coredll.lib /nologo /debug /machine:SH3 /subsystem:$(CESubsystem)
# SUBTRACT LINK32 /pdb:none /nodefaultlib
BSC32=bscmake.exe
# ADD BASE BSC32 /NOLOGO
# ADD BSC32 /NOLOGO
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
RSC=rc.exe
# ADD BASE RSC /l 0x411 /r /d "SHx" /d "SH3" /d "_SH3_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "DEBUG"
# ADD RSC /l 0x411 /r /d "SHx" /d "SH3" /d "_SH3_" /d UNDER_CE=$(CEVersion) /d _WIN32_WCE=$(CEVersion) /d "$(CEConfigName)" /d "UNICODE" /d "DEBUG"
CPP=shcl.exe
# ADD BASE CPP /nologo /MLd /W3 /Zi /Od /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "SHx" /D "SH3" /D "_SH3_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /YX /c
# ADD CPP /nologo /MTd /W3 /Zi /Od /I "./sys/win32" /I "./sys/default" /I "." /D _WIN32_WCE=$(CEVersion) /D "$(CEConfigName)" /D "DEBUG" /D "SHx" /D "SH3" /D "_SH3_" /D UNDER_CE=$(CEVersion) /D "UNICODE" /D "STRICT" /YX /c

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ng___Win"
# PROP BASE Intermediate_Dir "ng___Win"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "NOUNIDbg"
# PROP Intermediate_Dir "NOUNIDbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE F90 /include:"ng___Win/"
# ADD F90 /include:"NOUNIDbg/"
CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "sys/win32" /I "sys/default" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /D "_UNICODE" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "./sys/win32" /I "./sys/default" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STRICT" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
RSC=rc.exe
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib imm32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib imm32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ng___Wi0"
# PROP BASE Intermediate_Dir "ng___Wi0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "NOUNIRel"
# PROP Intermediate_Dir "NOUNIRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE F90 /include:"ng___Wi0/"
# ADD F90 /include:"NOUNIRel/"
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "sys/win32" /I "sys/default" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /D "_UNICODE" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "./sys/win32" /I "./sys/default" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STRICT" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
RSC=rc.exe
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib imm32.lib winmm.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib imm32.lib winmm.lib /nologo /subsystem:windows /machine:I386

!ENDIF 

# Begin Target

# Name "ng - Win32 Release"
# Name "ng - Win32 Debug"
# Name "ng - Win32 (WCE x86em) Release"
# Name "ng - Win32 (WCE x86em) Debug"
# Name "ng - Win32 (WCE MIPS) Release"
# Name "ng - Win32 (WCE MIPS) Debug"
# Name "ng - Win32 (WCE SH) Release"
# Name "ng - Win32 (WCE SH) Debug"
# Name "ng - Win32 Debug NO UNICODE"
# Name "ng - Win32 Release NO UNICODE"
# Begin Source File

SOURCE=.\sys\win32\appicon.ico
# End Source File
# Begin Source File

SOURCE=.\autosave.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\basic.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_BASIC=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_BASIC=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_BASIC=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\buffer.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_BUFFE=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_BUFFE=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_BUFFE=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sys\win32\buttons.bmp
# End Source File
# Begin Source File

SOURCE=.\sys\win32\cefep.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sys\win32\cefep.h
# End Source File
# Begin Source File

SOURCE=.\sys\default\chrdef.h
# End Source File
# Begin Source File

SOURCE=.\sys\default\cinfo.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_CINFO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_CINFO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_CINFO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cmode.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_CMODE=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_CMODE=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_CMODE=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\complt.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_COMPL=\
	".\complt.h"\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_COMPL=\
	".\complt.h"\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_COMPL=\
	".\complt.h"\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\complt.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=.\def.h
# End Source File
# Begin Source File

SOURCE=.\dir.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_DIR_C=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_DIR_C=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_DIR_C=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dired.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_DIRED=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_DIRED=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_DIRED=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\display.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_DISPL=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_DISPL=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_DISPL=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\echo.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_ECHO_=\
	".\complt.h"\
	".\config.h"\
	".\def.h"\
	".\key.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_ECHO_=\
	".\complt.h"\
	".\config.h"\
	".\def.h"\
	".\key.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_ECHO_=\
	".\complt.h"\
	".\config.h"\
	".\def.h"\
	".\key.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\extend.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_EXTEN=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\key.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_EXTEN=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\key.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_EXTEN=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\key.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\file.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_FILE_=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_FILE_=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_FILE_=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sys\win32\fileio.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_FILEI=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_FILEI=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_FILEI=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\help.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_HELP_=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\key.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_HELP_=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\key.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_HELP_=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\key.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jump.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kanji.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_KANJI=\
	".\config.h"\
	".\def.h"\
	".\kinit.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_KANJI=\
	".\config.h"\
	".\def.h"\
	".\kinit.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_KANJI=\
	".\config.h"\
	".\def.h"\
	".\kinit.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kbd.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_KBD_C=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\key.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_KBD_C=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\key.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_KBD_C=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\key.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kbd.h
# End Source File
# Begin Source File

SOURCE=.\key.h
# End Source File
# Begin Source File

SOURCE=.\keymap.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_KEYMA=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	
NODEP_CPP_KEYMA=\
	".\amiga_maps.c"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_KEYMA=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	
NODEP_CPP_KEYMA=\
	".\amiga_maps.c"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_KEYMA=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	
NODEP_CPP_KEYMA=\
	".\amiga_maps.c"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kinit.h
# End Source File
# Begin Source File

SOURCE=.\kinsoku.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_KINSO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_KINSO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_KINSO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\line.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_LINE_=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_LINE_=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_LINE_=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\macro.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_MACRO=\
	".\config.h"\
	".\def.h"\
	".\key.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_MACRO=\
	".\config.h"\
	".\def.h"\
	".\key.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_MACRO=\
	".\config.h"\
	".\def.h"\
	".\key.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\macro.h
# End Source File
# Begin Source File

SOURCE=.\main.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_MAIN_=\
	".\config.h"\
	".\def.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_MAIN_=\
	".\config.h"\
	".\def.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_MAIN_=\
	".\config.h"\
	".\def.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\match.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_MATCH=\
	".\config.h"\
	".\def.h"\
	".\key.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_MATCH=\
	".\config.h"\
	".\def.h"\
	".\key.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_MATCH=\
	".\config.h"\
	".\def.h"\
	".\key.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\modes.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_MODES=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_MODES=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_MODES=\
	".\config.h"\
	".\def.h"\
	".\kbd.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sys\win32\newres.h
# End Source File
# Begin Source File

SOURCE=.\sys\win32\ng.rc

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\paragraph.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_PARAG=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_PARAG=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_PARAG=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_RANDO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_RANDO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_RANDO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\re_search.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_RE_SE=\
	".\config.h"\
	".\def.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	
NODEP_CPP_RE_SE=\
	".\regex.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_RE_SE=\
	".\config.h"\
	".\def.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	
NODEP_CPP_RE_SE=\
	".\regex.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_RE_SE=\
	".\config.h"\
	".\def.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	
NODEP_CPP_RE_SE=\
	".\regex.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sys\win32\rebar.bmp
# End Source File
# Begin Source File

SOURCE=.\regex.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_REGEX=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	
NODEP_CPP_REGEX=\
	".\buffer.h"\
	".\lisp.h"\
	".\proto.h"\
	".\regex.h"\
	".\syntax.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_REGEX=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	
NODEP_CPP_REGEX=\
	".\buffer.h"\
	".\lisp.h"\
	".\proto.h"\
	".\regex.h"\
	".\syntax.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_REGEX=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	
NODEP_CPP_REGEX=\
	".\buffer.h"\
	".\lisp.h"\
	".\proto.h"\
	".\regex.h"\
	".\syntax.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\regex.h
# End Source File
# Begin Source File

SOURCE=.\region.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_REGIO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_REGIO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_REGIO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sys\win32\resource.h
# End Source File
# Begin Source File

SOURCE=.\search.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_SEARC=\
	".\config.h"\
	".\def.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_SEARC=\
	".\config.h"\
	".\def.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_SEARC=\
	".\config.h"\
	".\def.h"\
	".\macro.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\shell.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_SHELL=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_SHELL=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_SHELL=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\skg.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sys\win32\spawn.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_SPAWN=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_SPAWN=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_SPAWN=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sys\win32\sysdef.h
# End Source File
# Begin Source File

SOURCE=.\sys\win32\tools.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_TOOLS=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_TOOLS=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_TOOLS=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sys\win32\tools.h
# End Source File
# Begin Source File

SOURCE=.\sys\win32\tty.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_TTY_C=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	".\sys\win32\winmain.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_TTY_C=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	".\sys\win32\winmain.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_TTY_C=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	".\sys\win32\winmain.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sys\win32\ttyctrl.cpp

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_TTYCT=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	".\sys\win32\ttyctrl.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_TTYCT=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	".\sys\win32\ttyctrl.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_TTYCT=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	".\sys\win32\ttyctrl.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sys\win32\ttyctrl.h
# End Source File
# Begin Source File

SOURCE=.\sys\default\ttydef.h
# End Source File
# Begin Source File

SOURCE=.\sys\win32\ttyio.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_TTYIO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	".\sys\win32\winmain.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_TTYIO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	".\sys\win32\winmain.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_TTYIO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	".\sys\win32\winmain.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\undo.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\undo.h
# End Source File
# Begin Source File

SOURCE=.\version.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_VERSI=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_VERSI=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_VERSI=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\window.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_WINDO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_WINDO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_WINDO=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sys\win32\winmain.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_WINMA=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	".\sys\win32\ttyctrl.h"\
	".\sys\win32\winmain.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_WINMA=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	".\sys\win32\ttyctrl.h"\
	".\sys\win32\winmain.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_WINMA=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	".\sys\win32\tools.h"\
	".\sys\win32\ttyctrl.h"\
	".\sys\win32\winmain.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sys\win32\winmain.h
# End Source File
# Begin Source File

SOURCE=.\word.c

!IF  "$(CFG)" == "ng - Win32 Release"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Release"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE x86em) Debug"

DEP_CPP_WORD_=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Release"

DEP_CPP_WORD_=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE MIPS) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Release"

DEP_CPP_WORD_=\
	".\config.h"\
	".\def.h"\
	".\sys\default\chrdef.h"\
	".\sys\default\ttydef.h"\
	".\sys\win32\sysdef.h"\
	

!ELSEIF  "$(CFG)" == "ng - Win32 (WCE SH) Debug"

!ELSEIF  "$(CFG)" == "ng - Win32 Debug NO UNICODE"

!ELSEIF  "$(CFG)" == "ng - Win32 Release NO UNICODE"

!ENDIF 

# End Source File
# End Target
# End Project
