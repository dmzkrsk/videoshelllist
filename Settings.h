#define PROP_ARTIST			483001
#define PROP_TITLE			483002
#define PROP_TYPE			483003
#define PROP_FILESIZE		483004
#define PROP_VWIDTH			483005
#define PROP_VHEIGHT		483006
#define PROP_VFRAMERATE		483007
#define PROP_VBITRATE		483008
#define PROP_VCODEC			483009
#define PROP_VFOURCC		483010
#define PROP_ACODEC			483011
#define PROP_ACHANNELS		483012
#define PROP_ASAMPLING		483013
#define PROP_ABITRATE		483014

#define PROPSIZE 14

#define MAX_EXTLIST 512
#define DEFAULT_EXTENSIONS "avi,dat,mpeg,mpg,asf,asx,wmv,ram,rm,mov,qt,m2v,m1v,vob"

typedef struct
{
	int nID;
	char* szTitle;
}Properties;

extern Properties proplist[];

class CSettings
{
public:
	int		nListSaveAt;
	char*	szListSaveAt;
	int		sizeListSaveAt;

	int		nListName;
	char*	szListName;
	int		sizeListName;

	int*	pnExporting;
	int		sizeExporting;

	int		nSheet;

	char	szSaveFile[MAX_PATH];
	char	szExtensions[MAX_EXTLIST];

	bool	bOpenAfterAll;
public:
	BOOL IsExported(int nVal);
	void Load();
	BOOL Dump();

	CSettings(char* szFile);
	~CSettings();
private:
	void Clear();
};