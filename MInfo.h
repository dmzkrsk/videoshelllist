#ifndef MINFOLinkage
#define MINFOLinkage __declspec (dllimport)
#endif

#include <comdef.h>
#include <vfw.h>
#include <VFWMSGS.H>
#include <uuids.h>

#ifndef DWORD_PTR
	#define DWORD_PTR DWORD
#endif
#ifndef LONG_PTR
	#define LONG_PTR LONG
#endif

#define GUID_NOT_DETECTED "Unknown"
#define ERROR_LEVEL_NAME "Unknown"
#define ERROR_PROFILE_NAME "Unknown"

#define CODEC_NOT_DEFINED "No Information"

#define ERROR_SOURCE_NOT_DV 8500
#define ERROR_SOURCE_NOT_MPEG 8501
#define ERROR_SOURCE_NOT_MPEG2 8502
#define ERROR_SOURCE_NOT_VIDEO 8503
#define ERROR_SOURCE_NOT_VIDEO2 8504
#define ERROR_SOURCE_NOT_WAVE 8505
#define ERROR_SOURCE_NOT_ORDINARY_VIDEO 8506

#define STREAM_TYPE_ERROR 956799
#define STREAM_TYPE_DV 956800
#define STREAM_TYPE_MPEG1 956801
#define STREAM_TYPE_MPEG2 956802
#define STREAM_TYPE_VIDEO1 956803
#define STREAM_TYPE_VIDEO2 956804
#define STREAM_TYPE_WAVE 956805

#define STREAM_NAME_ERROR "Unknown"
#define STREAM_NAME_DV "Digital Video"
#define STREAM_NAME_MPEG1 "MPEG-1"
#define STREAM_NAME_MPEG2 "MPEG-2"
#define STREAM_NAME_VIDEO1 "Video Type 1"
#define STREAM_NAME_VIDEO2 "Video Type 2"
#define STREAM_NAME_WAVE "Audio Wave"

#include <QEdit.h>
#include <amvideo.h>
#include <Dvdmedia.h>

typedef struct
{
	char* szFourCC;
	char* szDescription;
}FOURCCINFOSTRUCT;

typedef struct
{
	DWORD dwCode;
	char* szDescription;
}AUDIOINFOSTRUCT ;

extern FOURCCINFOSTRUCT fccisCodecs[];
extern AUDIOINFOSTRUCT aisCodecs[];

class MINFOLinkage CStreamInfo : public AM_MEDIA_TYPE 
{
private:
	BOOL TypeNone() const {return formattype==FORMAT_None || formattype==GUID_NULL;};
	BOOL TypeDV() const {return formattype==FORMAT_DvInfo;};
	BOOL TypeMpeg1() const {return formattype==FORMAT_MPEGVideo;};
	BOOL TypeMpeg2() const {return formattype==FORMAT_MPEG2Video;};
	BOOL TypeVideo1() const {return formattype==FORMAT_VideoInfo;};
	BOOL TypeVideo2() const {return formattype==FORMAT_VideoInfo2;};
	BOOL TypeWave() const {return formattype==FORMAT_WaveFormatEx;};

	BITMAPINFOHEADER GetBMIH(DWORD *dwError);
public:
	long lStream;
	static CString GUIDescription(GUID guid);
	static CString FourCCToText(DWORD FourCC);
	static DWORD TextToFOURCC(CString sText);
	static CString GetFourCCDescription(CString FourCC);
	static CString GetAudioDescription(DWORD dwCodec);

	GUID GetMajorType() const {return majortype;};
	GUID GetSubType() const {return subtype;};

	int GetStreamType();
	CString GetStreamName();

	LONG GetFrameWidth();
	LONG GetFrameHeight();

	DWORD GetFrameCompression();
	CString GetFourCCCodec();

	DWORD GetDVAudioSource();
	DWORD GetDVAudioSource1();
	DWORD GetDVAudioControl();
	DWORD GetDVAudioControl1();
	DWORD GetDVVideoSource();
	DWORD GetDVVideoControl();

	BOOL IsMpeg2Interlaced();
	BOOL IsMpeg2OneFieldPerSample();
	BOOL IsMpeg2FirstFieldFirst();
	BOOL IsMpeg2NeverFirstField();
	BOOL IsMpeg2NeverSecondField();
	BOOL IsMpeg2FieldsRegular();
	BOOL IsMpeg2FieldsIrregular();
	BOOL IsMpeg2Bob();
	BOOL IsMpeg2BobAndWave();
	BOOL IsMpeg2Weave();
	BOOL IsMpeg2CopyProtected();
	BOOL IsMpeg2Pad4x3();
	BOOL IsMpeg2Pad16x9();
	POINT GetMpeg2AspectRatio();
	DWORD GetMpeg2Profile();
	DWORD GetMpeg2Level();
//	CString GetMpeg2ProfileName();
//	CString GetMpeg2LevelName();
	BOOL IsMpeg2Letterboxed();
	BOOL IsMpeg2PanScan();
	BOOL IsMpeg2CCDInGOP1();
	BOOL IsMpeg2CCDInGOP2();
	BOOL IsMpeg2Film();
	BOOL IsMpeg2AnalogLetterboxed();
	BOOL IsMpeg2DSSUserData();
	BOOL IsMpeg2DVBUserData();
	BOOL IsMpeg227MHzTimeBase();
	BOOL IsMpeg2AnalogWidescreen();

	BOOL IsVideo2Interlaced();
	BOOL IsVideo2OneFieldPerSample();
	BOOL IsVideo2FirstFieldFirst();
	BOOL IsVideo2NeverFirstField();
	BOOL IsVideo2NeverSecondField();
	BOOL IsVideo2FieldsRegular();
	BOOL IsVideo2FieldsIrregular();
	BOOL IsVideo2Bob();
	BOOL IsVideo2BobAndWave();
	BOOL IsVideo2Weave();
	BOOL IsVideo2CopyProtected();
	BOOL IsVideo2Pad4x3();
	BOOL IsVideo2Pad16x9();
	POINT GetVideo2AspectRatio();

	WORD GetAudioFormat();
	GUID GetAudioFormatGUID();
	WORD GetAudioChannels();
	DWORD GetAudioSamplesPerSecond();
	DWORD GetAudioBPS();

};

class MINFOLinkage CMediaInfo
{
private:
	bool bOpened;
	IMediaDet* pMediaInfo;
	CString sFilename;
private:
	static BOOL GetScreenshot(CString sSourceFile, double StreamTime, CString sDestFile, long width, long height);
public:
	CSize GetVideoDimensions();
	static BOOL GetFullScreenshot(CString sSourceFile, double StreamTime, CString sDestFile);
	static BOOL GetScaledScreenshot(CString sSourceFile, double StreamTime, CString sDestFile, double scale);
	static BOOL FitScreenshotWidth(CString sSourceFile, double StreamTime, CString sDestFile, long width);
	static BOOL FitScreenshotHeight(CString sSourceFile, double StreamTime, CString sDestFile, long height);
	CMediaInfo();
	~CMediaInfo();

	BOOL ReadFile(CString sFilename);
//	CString GetFilename();
	long GetStreamsCount();
	double GetStreamLength(long nStream);
	double GetFramerate(long nStream);
	BOOL IsFrameRateSpecified(long nStream);
	BOOL GetStreamInfo(long nStream, CStreamInfo* csiInfo);

	double GetStreamBPS(long nStream);
	LONG GetStreamSize(long nStream);
};
