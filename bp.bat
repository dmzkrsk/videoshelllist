@echo off
echo ==Build plugins batch file==
if not exist plug.make (
	echo Makefile not found
	exit /b
)
copy plug.make PluginsSrc\%1 >nul
pushd PluginsSrc\%1
if exist .\Release\ rmdir .\Release\ /s /q
nmake /f plug.make cfg="Release" plugin="%1" NO_EXTERNAL_DEPS="1" /nologo /c
if exist .\Release\ rmdir Release\ /s /q
del plug.make
popd
if exist PluginsSrc\%1\%1.vsp move PluginsSrc\%1\%1.vsp Plugins\
