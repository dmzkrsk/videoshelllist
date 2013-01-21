
VideoShellListps.dll: dlldata.obj VideoShellList_p.obj VideoShellList_i.obj
	link /dll /out:VideoShellListps.dll /def:VideoShellListps.def /entry:DllMain dlldata.obj VideoShellList_p.obj VideoShellList_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del VideoShellListps.dll
	@del VideoShellListps.lib
	@del VideoShellListps.exp
	@del dlldata.obj
	@del VideoShellList_p.obj
	@del VideoShellList_i.obj
