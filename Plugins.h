#define MAX_PNAME 256
#define MAX_PTITLE 256
#define MAX_PMESSAGE 1024
#define MAX_PSTATUS 256
#define MAX_EXT 256

typedef enum {modeSetup=0, modeAction=1} PLUGINMODE;

typedef struct
{
	BOOL bPlaylist;
	BOOL bSetup;
	BOOL bSelfEx;
} PLUGININFO;

typedef int	(*SAVERSETPARAM_DWORD)(int, char*, DWORD);
typedef int	(*SAVERSETPARAM_CHAR)(int, char*, char*);
typedef DWORD	(*SAVERGETPARAM_DWORD)(int, char*, DWORD);
typedef void	(*SAVERGETPARAM_CHAR)(int, char*, char**, int*, char*);

typedef struct
{
	SAVERSETPARAM_DWORD	SetDwrParam;
	SAVERSETPARAM_CHAR	SetStrParam;
	SAVERGETPARAM_DWORD	GetDwrParam;
	SAVERGETPARAM_CHAR	GetStrParam;
}SAVER;

typedef struct
{
	SAVER* pSaver;
	PLUGINMODE mode;
	int nPluginID;

	DLGPROC SetupDlgProc;
}INITPLUGIN;

typedef void (*PLUGINGETINFO)(PLUGININFO*);
typedef int (*PLUGININITPLUGIN)(INITPLUGIN*);
typedef char* (*PLUGINGETSTRING)(int, int);

#define TYPE_FORMAT	0
#define TYPE_LINE	1
#define TYPE_TABLE	2

#define DIV_HEADER	0
#define DIV_MIDDLE	1
#define DIV_END		2
