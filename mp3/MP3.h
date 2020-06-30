// MP3.h: interface for the CMP3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MP3_H__4059B60A_25E4_4A6D_BF59_6AE71C43D9C9__INCLUDED_)
#define AFX_MP3_H__4059B60A_25E4_4A6D_BF59_6AE71C43D9C9__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "string.h"
#include <windows.h>
#include <malloc.h>
#include "MP3Input.h"

#define LIST_SIZE_VB		512

class CMP3  
{
public:

	struct LIST_INFO_EX_VB
	{
		char	INAM[LIST_SIZE_VB];	//曲名
		char	IART[LIST_SIZE_VB];	//アーティスト名
		char    IPRD[LIST_SIZE_VB];	//製品名
		char	ICMT[LIST_SIZE_VB];	//コメント文字列
		char	ICRD[LIST_SIZE_VB];	//リリース年号
		char	IGNR[LIST_SIZE_VB];	//ジャンル名
		char    ICOP[LIST_SIZE_VB];	//著作権
		char    IENG[LIST_SIZE_VB];	//エンジニア
		char    ISRC[LIST_SIZE_VB];	//ソース
		char    ISFT[LIST_SIZE_VB];	//ソフトウェア
		char    IKEY[LIST_SIZE_VB];	//キーワード
		char    ITCH[LIST_SIZE_VB];	//技術者
		char    ICMS[LIST_SIZE_VB];	//コミッション
		char	IMED[LIST_SIZE_VB];	//中間 
		char	ISBJ[LIST_SIZE_VB];	//タイトル
		char	IMP3[LIST_SIZE_VB];	//MP3情報
		char    ILYC[LIST_SIZE_VB];	//歌詞
	};

	struct LYRICS3_INFO_VB
	{
		char	IND_LYR;		//LYRフィールドの有無 '0':無し '1':有り
		char	IND_TIMETAG;	//タイムタグの有無 '0':無し '1':有り
		char	AUT[250];		//作詞／作曲者名
		char	EAL[250];		//拡張アルバム名
		char	EAR[250];		//拡張アーティスト名
		char	ETT[250];		//拡張トラックタイトル
	};

	CMP3();
	virtual ~CMP3();

	void  xmMP3_setWindow(const HWND &hWnd);
	BOOL xmMP3_getWaveData(CMP3Input::WAVE_DATA* pWaveData);
	BOOL  xmMP3_setLyricsFile(const char* pszLyricsName);
	BOOL  xmMP3_getLyrics(CMP3Input::LYRICS_INFO* pLyricsInfo);
	void  xmMP3_getSpectrum(int* pSpecL, int* pSpecR);
	void  xmMP3_getWave(int* pWaveL, int* pWaveR);
	BOOL  xmMP3_getFileLyrics3Info(const char* pszName, CMP3Input::LYRICS3_INFO* pLyrics3Info);
	BOOL  xmMP3_getFileLyrics3InfoVB(const char* pszName, LYRICS3_INFO_VB* pLyrics3InfoVb, char* lyrData, char* InfData, char* ImgData);
	void  xmMP3_setLyrics3Use(int useLyrics3);
	BOOL  xmMP3_readLyrics3Data();
	BOOL  xmMP3_clearLyrics();
	void  xmMP3_setKaraokeUse(int useKaraoke);
	BOOL  xmMP3_getLyrics2(CMP3Input::LYRICS_INFO2* pLyricsInfo);
	void  xmMP3_setLyricsTime(int flag);
	void  xmMP3_startAnalyze();
	void  xmMP3_stopAnalyze();
	void  xmMP3_startAnalyzeThread();
	void  xmMP3_stopAnalyzeThread();
	BOOL  xmMP3_callback(INPUT_MSG_PROC pProc);
	BOOL  xmMP3_startCallback();
	BOOL  xmMP3_stopCallback();
	BOOL  xmMP3_setFileLyrics3Info(const char* pszName, CMP3Input::LYRICS3_INFO* pLyrics3Info);
	BOOL  xmMP3_delFileLyrics3Info(const char* pszName);
	BOOL  xmMP3_setFileLyrics3InfoVB(const char* pszName, LYRICS3_INFO_VB* pLyrics3InfoVb, char* lyrData, char* InfData, char* ImgData);
	void  xmMP3_setLyrics3InsField(int field);
	BOOL  xmMP3_playDecodeWave(const char* pszWaveName);
	BOOL  xmMP3_decodeWave(const char* pszWaveName);
	BOOL  xmMP3_changeWav(const char* pszName);
	BOOL  xmMP3_changeMp3(const char* pszName);
	BOOL  xmMP3_changeRmp(const char* pszName);
	BOOL  xmMP3_cutMacBinary(const char* pszName);
	BOOL  xmMP3_setTagInfo(const char* pszName, CMP3Input::TAG_INFO* pTagInfo,int tagSet, int tagAdd);
	BOOL  xmMP3_setListInfo(const char* pszName, CMP3Input::LIST_INFO* pListInfo);
	BOOL  xmMP3_setListInfoEX(const char* pszName, CMP3Input::LIST_INFO_EX* pListInfo);
	BOOL  xmMP3_setListInfoExVB(const char* pszName, LIST_INFO_EX_VB* pListInfo);
	BOOL  xmMP3_setTagInfoEX(const char* pszName, CMP3Input::TAG_INFO_11* pTagInfo,int tagSet, int tagAdd);
	BOOL  xmMP3_getFileInfoMtr(const char* pszName, CMP3Input::TAG_INFO* pTagInfo, CMP3Input::MPEG_INFO* pMpegInfo, CMP3Input::LIST_INFO* pListInfo, int* pFileType);
	BOOL  xmMP3_getFileInfo2(const char* pszName, CMP3Input::TAG_INFO* pTagInfo, CMP3Input::MPEG_INFO* pMpegInfo, CMP3Input::LIST_INFO* pListInfo);
	BOOL  xmMP3_getFileInfo(const char* pszName, CMP3Input::TAG_INFO* pTagInfo, CMP3Input::MPEG_INFO* pMpegInfo);
	BOOL  xmMP3_getFileTagInfo(const char* pszName, CMP3Input::TAG_INFO* pTagInfo);
	BOOL  xmMP3_getListInfo(CMP3Input::LIST_INFO* pListInfo);
	BOOL  xmMP3_getMpegInfo(CMP3Input::MPEG_INFO* pMpegInfo);
	BOOL  xmMP3_getTagInfo(CMP3Input::TAG_INFO* pTagInfo);
	int  xmMP3_getFileType(const char* pszName);
	BOOL  xmMP3_getGenre(CMP3Input::TAG_INFO* pTagInfo);
	long  xmMP3_getWinampPlayMs();
	int  xmMP3_getWinampTotalSec();
	int  xmMP3_getPlayBitRate();
	int  xmMP3_getLastErrorNo();
	int  xmMP3_getSilentFrames(const char* pszName);
	BOOL  xmMP3_getListInfoEX(const char* pszName, CMP3Input::LIST_INFO_EX* pListInfo);
	BOOL  xmMP3_getListInfoExVB(const char* pszName, LIST_INFO_EX_VB* pListInfo);
	BOOL  xmMP3_getFileTagInfoEX(const char* pszName, CMP3Input::TAG_INFO_11* pTagInfo);
	int  xmMP3_getVersion();
	BOOL  xmMP3_setxmMP3Option(CMP3Input::xmMP3_OPTION* pxmMP3Option);
	void  xmMP3_getxmMP3Option(CMP3Input::xmMP3_OPTION* pxmMP3Option);
	BOOL  xmMP3_setDecodeOption(CMP3Input::DEC_OPTION* pDecOption);
	void  xmMP3_getDecodeOption(CMP3Input::DEC_OPTION* pDecOption);
	long  xmMP3_debug();
	void  xmMP3_setEqualizer(int* pTable);
	void  xmMP3_setFftWindow(int window);
	void  xmMP3_setWaveOutDeviceId(int id);
	BOOL  xmMP3_setStepPitch(int pitch, int frames);
	int  xmMP3_getStepPitch();
	BOOL  xmMP3_reload();
	BOOL  xmMP3_setPitch(int pitch);
	int  xmMP3_getPitch();
	BOOL  xmMP3_init(int flag);
	BOOL  xmMP3_free();
	BOOL  xmMP3_open(const char* pszName, CMP3Input::InputInfo* pInfo);
	BOOL  xmMP3_close();
	int  xmMP3_getState(int* sec);
	BOOL  xmMP3_play();
	BOOL  xmMP3_SetBufferSize(int BufferRedimSize);
	BOOL  xmMP3_SetReadHaedA(int NEqualFrame);
	BOOL  xmMP3_stop();
	BOOL  xmMP3_pause();
	BOOL  xmMP3_restart();
	BOOL  xmMP3_seek(int seekSec);
	BOOL  xmMP3_setVolume(int left, int right);
	BOOL  xmMP3_getVolume(int* left, int* right);
	int  xmMP3_getWaveOutSupport();
	void  xmMP3_setSoftVolume(int left, int right);
	void  xmMP3_getSoftVolume(int* left, int* right);
	int  xmMP3_getPlayFlames();
	BOOL  xmMP3_setPlayFlames(int flames);
	long  xmMP3_getPlaySamples();
	int  xmMP3_getTotalSamples();
	BOOL  xmMP3_setPlaySamples(int sample);
	void  xmMP3_setFadeIn(int on);
	void  xmMP3_setFadeOut(int on);
	void  xmMP3_fadeOut();
	void  xmMP3_setOverTime(int on);

private:

	CMP3Input m_CMP3Input;

};

#endif // !defined(AFX_MP3_H__4059B60A_25E4_4A6D_BF59_6AE71C43D9C9__INCLUDED_)

