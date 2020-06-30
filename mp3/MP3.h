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
		char	INAM[LIST_SIZE_VB];	//�Ȗ�
		char	IART[LIST_SIZE_VB];	//�A�[�e�B�X�g��
		char    IPRD[LIST_SIZE_VB];	//���i��
		char	ICMT[LIST_SIZE_VB];	//�R�����g������
		char	ICRD[LIST_SIZE_VB];	//�����[�X�N��
		char	IGNR[LIST_SIZE_VB];	//�W��������
		char    ICOP[LIST_SIZE_VB];	//���쌠
		char    IENG[LIST_SIZE_VB];	//�G���W�j�A
		char    ISRC[LIST_SIZE_VB];	//�\�[�X
		char    ISFT[LIST_SIZE_VB];	//�\�t�g�E�F�A
		char    IKEY[LIST_SIZE_VB];	//�L�[���[�h
		char    ITCH[LIST_SIZE_VB];	//�Z�p��
		char    ICMS[LIST_SIZE_VB];	//�R�~�b�V����
		char	IMED[LIST_SIZE_VB];	//���� 
		char	ISBJ[LIST_SIZE_VB];	//�^�C�g��
		char	IMP3[LIST_SIZE_VB];	//MP3���
		char    ILYC[LIST_SIZE_VB];	//�̎�
	};

	struct LYRICS3_INFO_VB
	{
		char	IND_LYR;		//LYR�t�B�[���h�̗L�� '0':���� '1':�L��
		char	IND_TIMETAG;	//�^�C���^�O�̗L�� '0':���� '1':�L��
		char	AUT[250];		//�쎌�^��ȎҖ�
		char	EAL[250];		//�g���A���o����
		char	EAR[250];		//�g���A�[�e�B�X�g��
		char	ETT[250];		//�g���g���b�N�^�C�g��
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

