// MP3.cpp: implementation of the CMP3 class.
//
//////////////////////////////////////////////////////////////////////

#include "MP3.h"
#include <time.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMP3::CMP3()
{

}

CMP3::~CMP3()
{

}

/*/
 int   xmMP3_getEncodeState(int* readSize, int* encodeSize)
{
	return enc_getEncodeState(readSize, encodeSize);
}

 BOOL  xmMP3_encodeOpen(const char* pszWaveName, WAVE_FORM* pWaveForm)
{
	return enc_open(pszWaveName, pWaveForm);
}

 BOOL  xmMP3_encodeStart(const char* pszMp3Name)
{
	return enc_start(pszMp3Name);
}

 BOOL  xmMP3_encodeStop()
{
	return enc_stop();
} /*/

BOOL CMP3::xmMP3_getWaveData(CMP3Input::WAVE_DATA* pWaveData)
{
	//return input_getWaveData(pWaveData);
	return FALSE;
}

 BOOL  CMP3::xmMP3_setLyricsFile(const char* pszLyricsName)
{
	return m_CMP3Input.input_setLyricsFile(pszLyricsName);
}

 BOOL  CMP3::xmMP3_getLyrics(CMP3Input::LYRICS_INFO* pLyricsInfo)
{
	return m_CMP3Input.input_getLyrics(pLyricsInfo);
}

 void  CMP3::xmMP3_getSpectrum(int* pSpecL, int* pSpecR)
{
	m_CMP3Input.input_getSpectrum(pSpecL, pSpecR);
	return;
}

void  CMP3::xmMP3_getWave(int* pWaveL, int* pWaveR)
{
	m_CMP3Input.input_getWave(pWaveL, pWaveR);
}

 BOOL  CMP3::xmMP3_getFileLyrics3Info(const char* pszName, CMP3Input::LYRICS3_INFO* pLyrics3Info)
{
	return m_CMP3Input.input_getFileLyrics3Info(pszName, pLyrics3Info);
}

 BOOL  CMP3::xmMP3_getFileLyrics3InfoVB(const char* pszName, LYRICS3_INFO_VB* pLyrics3InfoVb, char* lyrData, char* InfData, char* ImgData)
{
	CMP3Input::LYRICS3_INFO lyrics3Info;

	::memset(pLyrics3InfoVb, 0, sizeof(LYRICS3_INFO_VB));
	::memset(lyrData, 0, 60000);
	::memset(lyrData, 0, 60000);
	::memset(lyrData, 0, 60000);

	if (!(m_CMP3Input.input_getFileLyrics3Info(pszName, &lyrics3Info))) {
		return FALSE;
	} else {
		::memcpy(lyrData, lyrics3Info.LYR, 60000);
		::memcpy(InfData, lyrics3Info.INF, 60000);
		::memcpy(ImgData, lyrics3Info.IMG, 60000);

		pLyrics3InfoVb->IND_LYR = lyrics3Info.IND_LYR;
		pLyrics3InfoVb->IND_TIMETAG = lyrics3Info.IND_TIMETAG;
		::memcpy(pLyrics3InfoVb->AUT, lyrics3Info.AUT, 250);
		::memcpy(pLyrics3InfoVb->EAL, lyrics3Info.EAL, 250);
		::memcpy(pLyrics3InfoVb->EAR, lyrics3Info.EAR, 250);
		::memcpy(pLyrics3InfoVb->ETT, lyrics3Info.ETT, 250);
		return TRUE;
	}
}

 void  CMP3::xmMP3_setLyrics3Use(int useLyrics3)
{
	m_CMP3Input.input_setLyrics3Use(useLyrics3);
}

 BOOL  CMP3::xmMP3_readLyrics3Data()
{
	return m_CMP3Input.input_readLyrics3Data();
}

 BOOL  CMP3::xmMP3_clearLyrics()
{
	return m_CMP3Input.input_clearLyrics();
}

 void  CMP3::xmMP3_setKaraokeUse(int useKaraoke)
{
	m_CMP3Input.input_setKaraokeUse(useKaraoke);
}

 BOOL  CMP3::xmMP3_getLyrics2(CMP3Input::LYRICS_INFO2* pLyricsInfo)
{
	return m_CMP3Input.input_getLyrics2(pLyricsInfo);
}

 void  CMP3::xmMP3_setLyricsTime(int flag)
{
	m_CMP3Input.input_setLyricsTime(flag);
}

 void  CMP3::xmMP3_startAnalyze()
{
	//input_startAnalyze();
}

 void  CMP3::xmMP3_stopAnalyze()
{
	//input_stopAnalyze();
}

 void  CMP3::xmMP3_startAnalyzeThread()
{
	//input_startAnalyzeThread();
}

void  CMP3::xmMP3_stopAnalyzeThread()
{
	//input_stopAnalyzeThread();
}

void  CMP3::xmMP3_setWindow(const HWND &hWnd)
{
	m_CMP3Input.input_setWindow(hWnd);
}

BOOL  CMP3::xmMP3_callback(INPUT_MSG_PROC pProc)
{
	return m_CMP3Input.input_setCallback(pProc);
}

 BOOL  CMP3::xmMP3_startCallback()
{
	return m_CMP3Input.input_startCallback();
}

 BOOL  CMP3::xmMP3_stopCallback()
{
	return m_CMP3Input.input_stopCallback();
}

 BOOL  CMP3::xmMP3_setFileLyrics3Info(const char* pszName, CMP3Input::LYRICS3_INFO* pLyrics3Info)
{
	return m_CMP3Input.input_setFileLyrics3Info(pszName, pLyrics3Info, 1);
}

 BOOL  CMP3::xmMP3_delFileLyrics3Info(const char* pszName)
{
	CMP3Input::LYRICS3_INFO Lyrics3Info;
	return m_CMP3Input.input_setFileLyrics3Info(pszName, &Lyrics3Info, 0);
}

 BOOL  CMP3::xmMP3_setFileLyrics3InfoVB(const char* pszName, LYRICS3_INFO_VB* pLyrics3InfoVb, char* lyrData, char* InfData, char* ImgData)
{
	CMP3Input::LYRICS3_INFO lyrics3Info;

	::memcpy(lyrics3Info.LYR, lyrData, 60000);
	::memcpy(lyrics3Info.INF, InfData, 60000);
	::memcpy(lyrics3Info.IMG, ImgData, 60000);

	lyrics3Info.IND_LYR = pLyrics3InfoVb->IND_LYR;
	lyrics3Info.IND_TIMETAG = pLyrics3InfoVb->IND_TIMETAG;

	::memcpy(lyrics3Info.AUT, pLyrics3InfoVb->AUT, 250);
	::memcpy(lyrics3Info.EAL, pLyrics3InfoVb->EAL, 250);
	::memcpy(lyrics3Info.EAR, pLyrics3InfoVb->EAR, 250);
	::memcpy(lyrics3Info.ETT, pLyrics3InfoVb->ETT, 250);

	return m_CMP3Input.input_setFileLyrics3Info(pszName, &lyrics3Info, 1);
}

 void  CMP3::xmMP3_setLyrics3InsField(int field)
{
	m_CMP3Input.input_setLyrics3InsField(field);
}

 BOOL  CMP3::xmMP3_playDecodeWave(const char* pszWaveName)
{
	return m_CMP3Input.input_playDecodeWave(pszWaveName);
}

 BOOL  CMP3::xmMP3_decodeWave(const char* pszWaveName)
{
	return m_CMP3Input.input_decodeWave(pszWaveName);
}

 BOOL  CMP3::xmMP3_changeWav(const char* pszName)
{
	return m_CMP3Input.input_changeWav(pszName);
}

 BOOL  CMP3::xmMP3_changeMp3(const char* pszName)
{
	return m_CMP3Input.input_changeMp3(pszName);
}

 BOOL  CMP3::xmMP3_changeRmp(const char* pszName)
{
	return m_CMP3Input.input_changeRmp(pszName);
}

 BOOL  CMP3::xmMP3_cutMacBinary(const char* pszName)
{
	return m_CMP3Input.input_cutMacBinary(pszName);
}

 BOOL  CMP3::xmMP3_setTagInfo(const char* pszName, CMP3Input::TAG_INFO* pTagInfo,int tagSet, int tagAdd)
{
	return m_CMP3Input.setTagInfo(pszName, pTagInfo, tagSet, tagAdd);
}

 BOOL  CMP3::xmMP3_setListInfo(const char* pszName, CMP3Input::LIST_INFO* pListInfo)
{
	return m_CMP3Input.setListInfo(pszName, pListInfo);
}

 BOOL  CMP3::xmMP3_setListInfoEX(const char* pszName, CMP3Input::LIST_INFO_EX* pListInfo)
{
	return m_CMP3Input.input_setListInfoEX(pszName, pListInfo);
}

 BOOL  CMP3::xmMP3_setListInfoExVB(const char* pszName, LIST_INFO_EX_VB* pListInfo)
{
	CMP3Input::LIST_INFO_EX lListInfo;
	::memset(&lListInfo, ' ', sizeof(CMP3Input::LIST_INFO_EX));

	::memcpy(lListInfo.INAM, pListInfo->INAM, LIST_SIZE_VB);
	::memcpy(lListInfo.IART, pListInfo->IART, LIST_SIZE_VB);
	::memcpy(lListInfo.IPRD, pListInfo->IPRD, LIST_SIZE_VB);
	::memcpy(lListInfo.ICMT, pListInfo->ICMT, LIST_SIZE_VB);
	::memcpy(lListInfo.ICRD, pListInfo->ICRD, LIST_SIZE_VB);
	::memcpy(lListInfo.IGNR, pListInfo->IGNR, LIST_SIZE_VB);
	::memcpy(lListInfo.ICOP, pListInfo->ICOP, LIST_SIZE_VB);
	::memcpy(lListInfo.IENG, pListInfo->IENG, LIST_SIZE_VB);
	::memcpy(lListInfo.ISRC, pListInfo->ISRC, LIST_SIZE_VB);
	::memcpy(lListInfo.ISFT, pListInfo->ISFT, LIST_SIZE_VB);
	::memcpy(lListInfo.IKEY, pListInfo->IKEY, LIST_SIZE_VB);
	::memcpy(lListInfo.ITCH, pListInfo->ITCH, LIST_SIZE_VB);
	::memcpy(lListInfo.ICMS, pListInfo->ICMS, LIST_SIZE_VB);
	::memcpy(lListInfo.IMED, pListInfo->IMED, LIST_SIZE_VB);
	::memcpy(lListInfo.ISBJ, pListInfo->ISBJ, LIST_SIZE_VB);
	::memcpy(lListInfo.IMP3, pListInfo->IMP3, LIST_SIZE_VB);
	::memcpy(lListInfo.ILYC, pListInfo->ILYC, LIST_SIZE_VB);

	return m_CMP3Input.input_setListInfoEX(pszName, &lListInfo);
}

 BOOL  CMP3::xmMP3_setTagInfoEX(const char* pszName, CMP3Input::TAG_INFO_11* pTagInfo,int tagSet, int tagAdd)
{
	return m_CMP3Input.setTagInfoEX(pszName, pTagInfo, tagSet, tagAdd);
}

BOOL  CMP3::xmMP3_getFileInfoMtr(const char* pszName, CMP3Input::TAG_INFO* pTagInfo, CMP3Input::MPEG_INFO* pMpegInfo, CMP3Input::LIST_INFO* pListInfo, int* pFileType)
{
	return m_CMP3Input.getFileInfoMtr(pszName, pTagInfo, pMpegInfo, pListInfo, pFileType);
}

 BOOL  CMP3::xmMP3_getFileInfo2(const char* pszName, CMP3Input::TAG_INFO* pTagInfo, CMP3Input::MPEG_INFO* pMpegInfo, CMP3Input::LIST_INFO* pListInfo)
{
	return m_CMP3Input.getFileInfo2(pszName, pTagInfo, pMpegInfo, pListInfo);
}

 BOOL  CMP3::xmMP3_getFileInfo(const char* pszName, CMP3Input::TAG_INFO* pTagInfo, CMP3Input::MPEG_INFO* pMpegInfo)
{
	return m_CMP3Input.getFileInfo(pszName, pTagInfo, pMpegInfo);
}

 BOOL  CMP3::xmMP3_getFileTagInfo(const char* pszName, CMP3Input::TAG_INFO* pTagInfo)
{
	return m_CMP3Input.getFileTagInfo(pszName, pTagInfo);
}

 BOOL  CMP3::xmMP3_getListInfo(CMP3Input::LIST_INFO* pListInfo)
{
	return m_CMP3Input.getListInfo(pListInfo);
}

 BOOL  CMP3::xmMP3_getMpegInfo(CMP3Input::MPEG_INFO* pMpegInfo)
{
	return m_CMP3Input.getMpegInfo(pMpegInfo);
}

 BOOL  CMP3::xmMP3_getTagInfo(CMP3Input::TAG_INFO* pTagInfo)
{
	return m_CMP3Input.getTagInfo(pTagInfo);
}

 int  CMP3::xmMP3_getFileType(const char* pszName)
{
	return m_CMP3Input.getFileType(pszName);
}

 BOOL  CMP3::xmMP3_getGenre(CMP3Input::TAG_INFO* pTagInfo)
{
	return m_CMP3Input.input_GetGenre(pTagInfo);
}

 long  CMP3::xmMP3_getWinampPlayMs()
{
	return m_CMP3Input.input_getWinampPlayMs();
}

 int  CMP3::xmMP3_getWinampTotalSec()
{
	return m_CMP3Input.input_getWinampTotalSec();
}

 int  CMP3::xmMP3_getPlayBitRate()
{
	return m_CMP3Input.input_getPlayBitRate();
}

 int  CMP3::xmMP3_getLastErrorNo()
{
	return m_CMP3Input.input_getLastErrorNo();
}

 int  CMP3::xmMP3_getSilentFrames(const char* pszName)
{
	return m_CMP3Input.input_getSilentFrames(pszName);
}

 BOOL  CMP3::xmMP3_getListInfoEX(const char* pszName, CMP3Input::LIST_INFO_EX* pListInfo)
{
	return m_CMP3Input.input_getListInfoEX(pszName, pListInfo);
}

 BOOL  CMP3::xmMP3_getListInfoExVB(const char* pszName, LIST_INFO_EX_VB* pListInfo)
{
	CMP3Input::LIST_INFO_EX lListInfo;

	::memset(pListInfo, 0, sizeof(LIST_INFO_EX_VB));

	if (!(m_CMP3Input.input_getListInfoEX(pszName, &lListInfo))) {
		return FALSE;
	} else {
		::memcpy(pListInfo->INAM, lListInfo.INAM, LIST_SIZE_VB);
		::memcpy(pListInfo->IART, lListInfo.IART, LIST_SIZE_VB);
		::memcpy(pListInfo->IPRD, lListInfo.IPRD, LIST_SIZE_VB);
		::memcpy(pListInfo->ICMT, lListInfo.ICMT, LIST_SIZE_VB);
		::memcpy(pListInfo->ICRD, lListInfo.ICRD, LIST_SIZE_VB);
		::memcpy(pListInfo->IGNR, lListInfo.IGNR, LIST_SIZE_VB);
		::memcpy(pListInfo->ICOP, lListInfo.ICOP, LIST_SIZE_VB);
		::memcpy(pListInfo->IENG, lListInfo.IENG, LIST_SIZE_VB);
		::memcpy(pListInfo->ISRC, lListInfo.ISRC, LIST_SIZE_VB);
		::memcpy(pListInfo->ISFT, lListInfo.ISFT, LIST_SIZE_VB);
		::memcpy(pListInfo->IKEY, lListInfo.IKEY, LIST_SIZE_VB);
		::memcpy(pListInfo->ITCH, lListInfo.ITCH, LIST_SIZE_VB);
		::memcpy(pListInfo->ICMS, lListInfo.ICMS, LIST_SIZE_VB);
		::memcpy(pListInfo->IMED, lListInfo.IMED, LIST_SIZE_VB);
		::memcpy(pListInfo->ISBJ, lListInfo.ISBJ, LIST_SIZE_VB);
		::memcpy(pListInfo->IMP3, lListInfo.IMP3, LIST_SIZE_VB);
		::memcpy(pListInfo->ILYC, lListInfo.ILYC, LIST_SIZE_VB);
		return TRUE;
	}
}

BOOL  CMP3::xmMP3_getFileTagInfoEX(const char* pszName, CMP3Input::TAG_INFO_11* pTagInfo)
{
	return m_CMP3Input.getFileTagInfoEX(pszName, pTagInfo);
}

int  CMP3::xmMP3_getVersion()
{
	int Version = 167;
	return Version;
}

 BOOL  CMP3::xmMP3_setxmMP3Option(CMP3Input::xmMP3_OPTION* pxmMP3Option)
{
	return m_CMP3Input.input_SetxmMP3Option(pxmMP3Option);
}

 void  CMP3::xmMP3_getxmMP3Option(CMP3Input::xmMP3_OPTION* pxmMP3Option)
{
	m_CMP3Input.input_GetxmMP3Option(pxmMP3Option);
}

 BOOL  CMP3::xmMP3_setDecodeOption(CMP3Input::DEC_OPTION* pDecOption)
{
	return m_CMP3Input.input_SetDecodeOption(pDecOption);
}

 void  CMP3::xmMP3_getDecodeOption(CMP3Input::DEC_OPTION* pDecOption)
{
	m_CMP3Input.input_GetDecodeOption(pDecOption);
}

 long  CMP3::xmMP3_debug()
{
	return m_CMP3Input.input_debug();
}

 void  CMP3::xmMP3_setEqualizer(int* pTable)
{
	m_CMP3Input.input_setEqualizer(pTable);
}

 void  CMP3::xmMP3_setFftWindow(int window)
{
	m_CMP3Input.input_setFftWindow(window);
}

 void  CMP3::xmMP3_setWaveOutDeviceId(int id){
	m_CMP3Input.input_setWaveOutDeviceId(id);
}

 BOOL  CMP3::xmMP3_setStepPitch(int pitch, int frames)
{
	return m_CMP3Input.input_setStepPitch(pitch, frames);
}

 int  CMP3::xmMP3_getStepPitch()
{
	return m_CMP3Input.input_getStepPitch();
}

 BOOL  CMP3::xmMP3_reload()
{
	return m_CMP3Input.input_reload();
}

 BOOL  CMP3::xmMP3_setPitch(int pitch)
{
	return m_CMP3Input.input_setPitch(pitch);
}

 int  CMP3::xmMP3_getPitch()
{
	return m_CMP3Input.input_getPitch();
}

 BOOL  CMP3::xmMP3_init(int flag)
{
	return m_CMP3Input.initDec();
}

 BOOL  CMP3::xmMP3_free()
{
	return m_CMP3Input.freeDec();
} 
 BOOL  CMP3::xmMP3_open(const char* pszName, CMP3Input::InputInfo* pInfo)
{
	int flag = 0;
	return m_CMP3Input.input_open(pszName, pInfo);
}

 BOOL  CMP3::xmMP3_close()
{
	return m_CMP3Input.input_close();
}

 int  CMP3::xmMP3_getState(int* sec)
{
	*sec = m_CMP3Input.getTime();
	return m_CMP3Input.input_getState();
}


BOOL  CMP3::xmMP3_play()
{
	return m_CMP3Input.input_play();
}


BOOL  CMP3::xmMP3_SetBufferSize(int BufferRedimSize)
{
	return m_CMP3Input.SetBufferRedim(BufferRedimSize);
}

BOOL  CMP3::xmMP3_SetReadHaedA(int NEqualFrame)
{
	return m_CMP3Input.SetReadHaedA(NEqualFrame);
}

 BOOL  CMP3::xmMP3_stop()
{
	return m_CMP3Input.input_stop();
}

 BOOL  CMP3::xmMP3_pause()
{
	return m_CMP3Input.input_pause();
}

 BOOL  CMP3::xmMP3_restart()
{
	return m_CMP3Input.input_restart();
}

 BOOL  CMP3::xmMP3_seek(int seekSec)
{
	m_CMP3Input.input_seekBySec(seekSec);
	return TRUE;
}

 BOOL  CMP3::xmMP3_setVolume(int left, int right)
{
	return m_CMP3Input.input_setVolume(left, right);
}

 BOOL  CMP3::xmMP3_getVolume(int* left, int* right)
{
	return m_CMP3Input.input_getVolume(left, right);
}

 int  CMP3::xmMP3_getWaveOutSupport()
{
	return m_CMP3Input.input_getWaveOutSupport();
}

 void  CMP3::xmMP3_setSoftVolume(int left, int right)
{
	m_CMP3Input.input_setSoftVolume(left, right);
}

 void  CMP3::xmMP3_getSoftVolume(int* left, int* right)
{
	m_CMP3Input.input_getSoftVolume(left, right);
}

 int  CMP3::xmMP3_getPlayFlames()
{
	return m_CMP3Input.input_getPlayFlames();
}

 BOOL  CMP3::xmMP3_setPlayFlames(int flames)
{
	m_CMP3Input.input_seekByFrame(flames);
	return TRUE;
}

 long  CMP3::xmMP3_getPlaySamples()
{
	return m_CMP3Input.input_getPlaySamples();
}

 int  CMP3::xmMP3_getTotalSamples()
{
	return m_CMP3Input.input_getTotalSamples();
}

 BOOL  CMP3::xmMP3_setPlaySamples(int sample)
{
	m_CMP3Input.input_seekBySample(sample);
	return TRUE;
}

 void  CMP3::xmMP3_setFadeIn(int on)
{
	m_CMP3Input.input_setFadeIn(on);
}

 void  CMP3::xmMP3_setFadeOut(int on)
{
	m_CMP3Input.input_setFadeOut(on);
}

 void  CMP3::xmMP3_fadeOut()
{
	m_CMP3Input.input_fadeOut();
}

 void  CMP3::xmMP3_setOverTime(int on)
{
	m_CMP3Input.input_setOverTime(on);
}