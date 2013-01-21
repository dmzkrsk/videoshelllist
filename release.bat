@echo off

if exist Install rmdir Install /s /q
if not exist Install\ mkdir Install\

copy VideoShellList.dll Install\

copy lib_release\fsys.dll Install\
copy lib_release\minfo.dll Install\
copy lib_release\reg.dll Install\

copy register.bat Install\
copy unregister.bat Install\

xcopy /y /s /e Plugins\*.vsp Install\Plugins\*.*

if not exist Install\src\ mkdir Install\src\

copy Plugin.cpp Install\src\
copy Plugin.h Install\src\
copy Settings.cpp Install\src\
copy Settings.h Install\src\
copy StdAfx.cpp Install\src\
copy StdAfx.h Install\src\
copy VideoShellList.cpp Install\src\
copy VideoShellList.h Install\src\
copy VSLExt.cpp Install\src\
copy VSLExt.h Install\src\

copy filesys.h Install\src\
copy MInfo.h Install\src\
copy Registry.h Install\src\

copy release.bat Install\src\

copy bp.bat Install\src\
copy plug.make Install\src\

copy VideoShellList_i.c Install\src\
copy VideoShellList_p.c Install\src\
copy dlldata.c Install\src\
copy Resource.h Install\src\

copy VideoShellList.def Install\src\
copy VideoShellList.clw Install\src\
copy VideoShellList.dsp Install\src\
copy VideoShellList.rc Install\src\
copy VideoShellList.tlb Install\src\
copy VideoShellList.dsw Install\src\
copy VideoShellList.idl Install\src\

copy VideoShellListps.def Install\src\
copy VideoShellListps.mk Install\src\

copy VSLExt.rgs Install\src\

copy PlugConst.h Install\src\
copy Plugins.h Install\src\

xcopy /y /s /e lib_debug\*.* Install\src\lib_debug\*.*
xcopy /y /s /e lib_release\*.* Install\src\lib_release\*.*
xcopy /y /s /e res\*.* Install\src\res\*.*

if exist Install\vsl_src.rar del Install\vsl_src.rar /f /q
pushd Install\src\
rar a -m5 -r -df ..\vsl_src.rar .\
popd
rmdir Install\src /s /q
upx Install\VideoShellList.dll
upx Install\Plugins\*.*

copy InstallSrc Install
"C:\Program Files\NSIS\makensis.exe" Install\vsl.nsi >install.log
rem if exist Install rmdir Install /s /q
