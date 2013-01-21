#include <windows.h>
#include "../../Plugins.h"
#include "defines.h"

#define PLLinkage extern "C" __declspec (dllexport)

int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved){return TRUE;}

PLLinkage void GetPluginInfo(PLUGININFO* pPlInfo)
{
	pPlInfo->bPlaylist=TRUE;
	pPlInfo->bSetup=FALSE;
	pPlInfo->bSelfEx=FALSE;
}

PLLinkage int InitPlugin(INITPLUGIN* pInit){return 0;}

PLLinkage char* GetString(int nType, int nDiv)
{
	switch(nType)
	{
	case TYPE_FORMAT:
		switch(nDiv)
		{
		case DIV_HEADER:	return RET_FORMAT_HEADER; break;
		case DIV_MIDDLE:	return RET_FORMAT_MIDDLE; break;
		case DIV_END:		return RET_FORMAT_END; break;
		}
		break;
	}

	return NULL;
}
