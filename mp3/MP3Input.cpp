// MP3Input.cpp: implementation of the CMP3Input class.
//
//////////////////////////////////////////////////////////////////////

#include "MP3Input.h"
#include "Fft.h"
#include <strsafe.h>
#define FFT_SIZE	512
#define FFT_BAND	256
#define WAVE_BAND	256

Fft m_fft_rectangle(FFT_SIZE, Fft::rectangle);
Fft m_fft_hanning(FFT_SIZE, Fft::hanning);
Fft m_fft_hamming(FFT_SIZE, Fft::hamming);
Fft m_fft_blackman(FFT_SIZE, Fft::blackman);

CRITICAL_SECTION m_cs;						//äeéÌïœêî ìØä˙óp

HWND CMP3Input::m_HwndWindow;


typedef unsigned int (__stdcall *THREADFUNC)(void* pParam);

int		CMP3Input::m_playSec;					//ââëtåoâﬂéûä‘
int		CMP3Input::m_playSecCur;				//ââëtåoâﬂéûä‘(èoóÕóp)
int		CMP3Input::m_state;					//åªç›ÇÃèÛë‘
double	CMP3Input::m_playMs;					//ââëtíÜÇÃéûä‘ÅiÉ~ÉäïbÅj
int		CMP3Input::m_playSamples;				//ââëtÉTÉìÉvÉã
int		CMP3Input::m_firstSample;				//ÉTÉìÉvÉãÇÃà íu
BYTE*	CMP3Input::m_outputBuf_in;				//èoóÕÉoÉbÉtÉ@ÅiPCMÅj
int		CMP3Input::decodePos;					//çƒê∂éûä‘åvéZóp
BOOL	CMP3Input::callBackFlag;		//ÉRÅ[ÉãÉoÉbÉNä÷êîégópóLñ≥
int		CMP3Input::m_lvol = 100;
int		CMP3Input::m_rvol = 100;
int		CMP3Input::m_olvol = 100;
int		CMP3Input::m_orvol = 100;
int		CMP3Input::m_inputBytes;				//ÉfÉRÅ[ÉhÇµÇƒÇ¢Ç»Ç¢ÉoÉbÉtÉ@ÉTÉCÉY
BYTE*	CMP3Input::m_inputPtr;					//ì¸óÕÉ|ÉCÉìÉ^
BOOL	CMP3Input::m_waveFlag;					//ÉIÅ[ÉvÉìÉtÉ@ÉCÉãÇ™PCMÇ»ÇÁTRUE
BYTE*	CMP3Input::m_inputBuf;
int		CMP3Input::m_inputSize;
int		CMP3Input::m_errNo;
HANDLE	CMP3Input::m_hFile;					//ëŒè€ÇÃÉtÉ@ÉCÉã
DWORD	CMP3Input::dataSize;					//MPEG AudioÉfÅ[É^ÉTÉCÉY
int		CMP3Input::decFlag;					//ÉfÉRÅ[ÉhÉtÉâÉO 0:çƒê∂ 1:WaveèoóÕ 2:çƒê∂ÇµÇ»Ç™ÇÁWAVEèoóÕ
HANDLE	CMP3Input::m_hWaveFile;				//WAVEÉtÉ@ÉCÉã
int		CMP3Input::m_inWaveBufSize;
int		CMP3Input::m_readWaveDataSize;
MPEG_DECODE_INFO CMP3Input::m_decInfo;
int				CMP3Input::m_totalSec;					//ââëtëçéûä‘
HANDLE			CMP3Input::m_hThread;					//ÉfÉRÅ[ÉhópÉXÉåÉbÉh
DWORD				CMP3Input::m_threadID;					//ÉfÉRÅ[ÉhópÉXÉåÉbÉhID
int             CMP3Input::m_NEqualFrame;
INPUT_MSG_PROC	CMP3Input::m_pProc;					//ÉRÅ[ÉãÉoÉbÉNä÷êî
int				CMP3Input::m_userData;					//ÉRÅ[ÉãÉoÉbÉNä÷êîÉpÉâÉÅÅ[É^
int				CMP3Input::m_playFrames;				//ââëtÉtÉåÅ[ÉÄ
double			CMP3Input::m_playSpeed;
int				CMP3Input::m_reduction;
int				CMP3Input::m_bitRate;
int				CMP3Input::m_firstSync;				//ç≈èâÇÃìØä˙ÉwÉbÉ_à íu
double			CMP3Input::m_msPerFrame;				//1ÉtÉåÅ[ÉÄÇÃââëtéûä‘ÅiÉ~ÉäïbÅj
int				CMP3Input::m_inputBlock;			//ì¸óÕÉtÉåÅ[ÉÄêî
int				CMP3Input::m_outputBlock_in;		//èoóÕÉtÉåÅ[ÉÄêî
int				CMP3Input::m_outputBlock_in_tmp;		//èoóÕÉtÉåÅ[ÉÄêî
int				CMP3Input::m_inputSleep;			//ì¸óÕíºå„ÇÃÉXÉäÅ[Évéûä‘(É~Éäïb)
BOOL			CMP3Input::m_fadeOutFlag;
MPEG_DECODE_OPTION   CMP3Input::decodeOption;			//ÉfÉRÅ[ÉhÉGÉìÉWÉìÉIÉvÉVÉáÉì
BOOL			CMP3Input::m_openFlag;
BOOL			CMP3Input::m_playFlag;
BOOL			CMP3Input::m_timeFlag;			//TRUE:ââëtéûä‘Ç™ëçéûä‘âzÇµÇΩÇÁêÿÇËéÃÇƒ
BOOL			CMP3Input::m_seekFlag;			//ÉVÅ[ÉNíÜÇ»ÇÁ TRUE
BOOL			CMP3Input::m_stepFlag;
BOOL			CMP3Input::m_Lyrics3UseFlag;	//Lyrics3 èÓïÒÇÃégópëIë
BOOL			CMP3Input::m_KaraokeTagFlag;	//ÉJÉâÉIÉPÉ^ÉOégópëIë
BOOL			CMP3Input::m_ExistKaraokeTag;
int			CMP3Input::m_Lyrics3InsertPoint;	//0:ILYC 1:IID3 2:ñ≥Çµ


HANDLE CMP3Input::m_hSemaphore;
HWAVEOUT		CMP3Input::m_hWaveOut;					//ÉIÅ[ÉfÉBÉIèoóÕÉfÉoÉCÉX
int				CMP3Input::m_channels;
 int				CMP3Input::m_bitsPerSample;
 int				CMP3Input::m_blockSize;
 int				CMP3Input::m_BufferRedim;
 int				CMP3Input::m_outputBlock;
 int				CMP3Input::m_outputIndex;
 int				CMP3Input::m_outputFinished;
 BYTE**			CMP3Input::m_outputTable;
 int				CMP3Input::m_waveBlock;
 int				CMP3Input::m_waveIndex;				//WAVEHDR ÉCÉìÉfÉbÉNÉX
 WAVEHDR*		CMP3Input::m_waveTable;				//WAVEHDR äiî[ÉeÅ[ÉuÉã
 BYTE*			CMP3Input::m_outputPtr;
 int				CMP3Input::m_outputBytes;
 int				CMP3Input::m_outputSize;
 int				CMP3Input::m_pitch;				//ÉsÉbÉ`(%)
 int				CMP3Input::m_outputSleep;
 int				CMP3Input::m_lSoftVol;						//É\ÉtÉgÉEÉFÉAÉ{ÉäÉÖÅ[ÉÄ(ç∂)
 int				CMP3Input::m_rSoftVol;						//É\ÉtÉgÉEÉFÉAÉ{ÉäÉÖÅ[ÉÄ(âE)
 int				CMP3Input::m_lMaxSoftVol;			//ç≈ëÂÉ\ÉtÉgÉEÉFÉAÉ{ÉäÉÖÅ[ÉÄ(ç∂)
 int				CMP3Input::m_rMaxSoftVol;			//ç≈ëÂÉ\ÉtÉgÉEÉFÉAÉ{ÉäÉÖÅ[ÉÄ(âE)
 BOOL			CMP3Input::m_fadeIn;
 BOOL			CMP3Input::m_fadeOut;
 BOOL			CMP3Input::m_stepPitch;
 int				CMP3Input::m_deviceId;		//ÉfÉoÉCÉXID
 int				CMP3Input::m_fftWindow;

	const char *genre_tbl[256] = {
		"Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk", "Grunge", "Hip-Hop",
		"Jazz", "Metal", "New Age", "Oldies", "Other", "Pop", "R&B", "Rap",
		"Reggae", "Rock", "Techno", "Industrial", "Alternative", "Ska", "Death Metal", "Pranks",
		"Soundtrack", "Euro-tecno", "Ambient", "Trip-Hop", "Vocal", "Juzz+Funk", "Fusion", "Trance",
		"Classical", "Instrumental", "Acid", "House", "Game", "Sound Clip", "Gospel", "Noise",
		"AlternRock", "Bass", "Soul", "Punk", "Space", "Meditative", "Instrumental Pop", "Instrumental Rock",
		"Ethnic", "Gothic", "Darkwave", "Techno-Industrial", "Electronic", "Pop-Folk", "Eurodance", "Dream",
		"Southern Rock", "Comedy", "Cult", "Gangsta", "Top 40", "Christian Rap", "Pop/Funk", "Jungle",

		"Native American", "Cabaret", "New Wave", "Psychadelic", "Rave", "Showtunes", "Trailer", "Lo-Fi",
		"Tribal", "Acid Punk", "Acid Jazz", "Polka", "Retro", "Musical", "Rock & Roll", "Hard Rock",
		"Folk", "Folk/Rock", "National Folk", "Swing", "Fast-Fusion", "Bedob", "Latin", "Revival",
		"Celtic", "Bluegrass", "Avantagarde", "Gothic Rock", "Progressive Rock", "Psychadelic Rock", "Symphonic Rock", "Slow Rock",
		"Big band", "Chorus", "Easy Listening", "Acoustic", "Humour", "SpeechChanson", "Chanson", "Opera",
		"Chamber Music", "Sonata", "Symphony", "Booty Bass", "Primus", "Porn Groove", "Satire", "Slow Jam",
		"Club", "Tango", "Samba", "Folklore", "Ballad", "Power Ballad", "Rhytmic Soul", "Freestyle",
		"Duet", "Punk Rock", "Drum Solo", "Acapella", "Euro-House", "Dance Hall", "Goa", "Dram & Bass",

		"Club-House", "Hardcore", "Terror", "Indie", "BritPop", "Negerpunk", "Polsk Punk", "Beat",
		"Chiristian Gangsta Rap", "Heavy Metal", "Black Metal", "Crossover", "Contemporary Christian", "Christian Rock", "Merengue", "Salsa",
		"Thrash Metal", "Anime", "JPop", "Synthpop", "", "", "", "",
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",

		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		"", "", "", "", "", "", "", "",
		"", "", "", "Seiyu(J)", "Tecno Ambient(J)", "Moemoe(J)", "Tokusatsu(J)", "Anime(J)"

	};

	static const double ms_table[3][3] =
	{
		// Layer1
		{8.707483f, 8.0f, 12.0f},
		// Layer2
		{26.12245f, 24.0f, 36.0f},
		// Layer3
		{26.12245f, 24.0f, 36.0f},
	};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef unsigned int (__stdcall *THREADFUNC)(void* pParam);

void __stdcall CMP3Input::_NullProc(int msg, int param) 
{
}

void CMP3Input::input_setWaveOutDeviceId(int id){
	output_setWaveOutDeviceId(id);
}

void CMP3Input::input_setLyricsTime(int flag){
	m_lyricsTotalFlag = flag;
}

void CMP3Input::input_setFftWindow(int window)
{
	output_setFftWindow(window);
}

void CMP3Input::input_setKaraokeUse(int useKaraoke)
{
	if ( useKaraoke == 1 ) {
		m_KaraokeTagFlag = TRUE;
	} else {
		m_KaraokeTagFlag = FALSE;
	}
}

BOOL CMP3Input::input_getListInfoEX(const char* pszName, LIST_INFO_EX* lpListInfo)
{
	HANDLE			hFileMpeg;				//Mp3ÉtÉ@ÉCÉã
	BOOL			result;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileMpeg = ::CreateFile(pszName, GENERIC_READ, FILE_SHARE_READ + FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileMpeg == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	result = getListInfoEX(hFileMpeg, lpListInfo);
	::CloseHandle(hFileMpeg);

	return result;
}

BOOL CMP3Input::input_setListInfoEX(const char* pszName, LIST_INFO_EX* lpListInfo)
{
	HANDLE			hFileMpeg;				//Mp3ÉtÉ@ÉCÉã
	BOOL			result;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileMpeg = ::CreateFile(pszName, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileMpeg == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	result = setListInfoEX(hFileMpeg, lpListInfo);
	::CloseHandle(hFileMpeg);

	return result;
}

BOOL CMP3Input::input_clearLyrics()
{
	if (m_state != STATE_STOP) {
		//í‚é~íÜà»äOÇÕé¿çsïsâ¬
		m_errNo = ERR_NOT_STATE_STOP;
		return FALSE;
	}

	m_ExistKaraokeTag = FALSE;
	m_lyricsCount = 0;
	m_lyricsLineNo = 0;
	m_lyricsCurrent = 0;
	::memset(m_lyrics, 0, sizeof(m_lyrics));
	return TRUE;

}

BOOL CMP3Input::input_readLyrics3Data()
{
	if (m_hFile == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_NOT_OPEN;
		return FALSE;
	}

	return input_readLyrics3(m_hFile);
}

void CMP3Input::input_setLyrics3Use(int useLyrics3)
{
	if ( useLyrics3 == 1 ) {
		m_Lyrics3UseFlag = TRUE;
	} else {
		m_Lyrics3UseFlag = FALSE;
	}
}

void CMP3Input::input_setLyrics3InsField(int field)
{
	if ( field < 0 || field > 2 ) {
		m_Lyrics3InsertPoint = 0;
	} else {
		m_Lyrics3InsertPoint = field;
	}
}

int CMP3Input::input_getSilentFrames(const char* pszName)
{
	HANDLE			hFileMpeg;				//É^ÉOï“èWÉtÉ@ÉCÉã
	BOOL			result;
	int				fSync;					//ìØä˙ÉwÉbÉ_ÇÃà íu
	DWORD			dSize;					//MPEG AudioÇÃÉfÅ[É^ÉTÉCÉY
	BYTE			buf[10240];
	//BYTE			buf[2048];
	DWORD			bytesRead;
	MPEG_DECODE_INFO dInfo;
	LIST_INFO		ListInfo;
	DWORD			fSize;
	int				headSize;
	int				frameSize;
	int				dCnt; 
	int				sCnt = 0;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return -1;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileMpeg = ::CreateFile(pszName, GENERIC_READ, FILE_SHARE_READ + FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileMpeg == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return -1;
	}

	fSync = 0;
	dSize = GetFileSize(hFileMpeg, NULL);
	fSize = dSize;

	SetFilePointer(hFileMpeg, 0, NULL, FILE_BEGIN);
	result = ReadFile(hFileMpeg, buf, 4, &bytesRead, NULL);
	if (!result || bytesRead != 4) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(hFileMpeg);
		return -1;
	}

	//RIFFå`éÆÅAID3 Tag v2ÇÉ`ÉFÉbÉN
	if (memcmp(buf, "RIFF", 4) == 0) 
	{
		readRIFF(hFileMpeg, &ListInfo, &fSync, &dSize);
		if (RiffStatus != 0) {
			::CloseHandle(hFileMpeg);
			m_errNo = ERR_UNKNOWN_FILE;
			return -1;
		} else {
			if (readID3v2(hFileMpeg, &fSync)) {
				dSize -= fSync;
			}
		}
	}
	else if (memcmp(buf, "ID3", 3) == 0) {
		if (readID3v2(hFileMpeg, &fSync)) {
			dSize -= fSync;
		}
	}
	//Tag ñ≥ÇµÉfÅ[É^
	else if (buf[0] == 0x00) {
			fSync = 128;
	}
	else {
		fSync = 0;
	}

	for(;;){

		//ìØä˙ÉwÉbÉ_éÊìæèÄîı
		::SetFilePointer(hFileMpeg, fSync, NULL, FILE_BEGIN);
		result = ::ReadFile(hFileMpeg, buf, sizeof(buf), &bytesRead, NULL);
		if (!result || bytesRead == 0) {
			::CloseHandle(hFileMpeg);
			m_errNo = ERR_MP3_FILE_READ;
			return -1;
		}
		//ìØä˙ÉwÉbÉ_ÇíTÇ∑
		int sync;
		if (!::mp3FindSync(buf, bytesRead, &sync)) {
			::CloseHandle(hFileMpeg);
			m_errNo = ERR_FRAME_HEADER_NOT_FOUND;
			return -1;
		}
		fSync += sync;

		//ìØä˙ÉwÉbÉ_à íuéÊìæ
		::SetFilePointer(hFileMpeg, fSync, NULL, FILE_BEGIN);
		result = ::ReadFile(hFileMpeg, buf, sizeof(buf), &bytesRead, NULL);
		if (!result || bytesRead == 0) {
			::CloseHandle(hFileMpeg);
			m_errNo = ERR_MP3_FILE_READ;
			return -1;
		}
		
		//ìØä˙ÉwÉbÉ_éÊìæ
		if (!::mp3GetDecodeInfo(buf, bytesRead, &dInfo, 0)) {
			m_errNo = ERR_FRAME_HEADER_READ;
			::CloseHandle(hFileMpeg);
			return -1;
		}

		//ÉwÉbÉ_ÉTÉCÉYéÊìæ
		headSize = 4;
		if ( dInfo.header.error_prot == 1 ) headSize = headSize + 2;
		if ( dInfo.header.layer == 3 ) {
			if ( dInfo.header.version == 1 ) {
				headSize = headSize + 17 * dInfo.channels;
			} else {
				headSize = headSize + 9 * dInfo.channels;
			}
		}

		//ÉtÉåÅ[ÉÄÉTÉCÉYéÊìæ
//		if ( dInfo.skipSize != 0 ){
//			frameSize = dInfo.skipSize;
//		} else {
			if ( dInfo.header.padding == 0 ) {
				frameSize = dInfo.minInputSize;
			} else {
				frameSize = dInfo.maxInputSize;
			}
//		}

		sCnt++;
		if ( sCnt == 1 ) debugNo = dInfo.skipSize;
		if ( !(dInfo.skipSize != 0 && sCnt == 1) ) {
			for ( dCnt = headSize; dCnt < frameSize; dCnt++ ) {
				if ( buf[dCnt] != 0xFF ) {
					::CloseHandle(hFileMpeg);
					return sCnt - 1;
				}
			}
		}
		fSync = fSync + frameSize;
	}

	::CloseHandle(hFileMpeg);
	return -1;

}

void CMP3Input::input_setOverTime(int on)
{
	if ( on == 0 ) {
		m_timeFlag = FALSE;
	} else {
		m_timeFlag = TRUE;
	}
}


int CMP3Input::input_getLastErrorNo()
{
	return m_errNo;
}

void CMP3Input::input_setErrNo(int no)
{
	m_errNo = no;
}

void CMP3Input::input_fadeOut()
{
	output_getVolume(&m_olvol,&m_orvol);
	m_lvol = m_olvol;
	m_rvol = m_orvol;
	m_fadeOutFlag = true;
}

void CMP3Input::input_setWindow(const HWND &hWnd)
{
	m_HwndWindow = hWnd;
}


void CMP3Input::input_fadeOutVolume()
{
	if ( m_fadeOutFlag ){
		if ( m_lvol > 0 )m_lvol--;
		if ( m_rvol > 0 )m_rvol--;
		output_setVolume(m_lvol, m_rvol);

		if (m_lvol == 0 && m_rvol == 0) {
			m_lvol=m_olvol;
			m_rvol=m_orvol;
			m_fadeOutFlag = false;
			output_stop();
		}
	}
}

int CMP3Input::input_getDecodeOutputSize()
{
	return (int)output_sampleToBytes(m_firstSample);
}

int CMP3Input::input_getTotalOutputSize()
{
	return (int)output_sampleToBytes(input_getTotalSamples());
}


void CMP3Input::input_setFadeIn(int on)
{
	output_setFadeIn(on);
}

void CMP3Input::input_setFadeOut(int on)
{
	output_setFadeOut(on);
}


BOOL CMP3Input::input_reload()
{
	if ( m_state == STATE_STOP ) {
		m_errNo = ERR_STATE_STOP;
		return FALSE;
	}

	int w_state = m_state;
	int curSample = output_getCurrentSample() + m_firstSample;

	input_stop();
	input_play();
	input_seekBySample(curSample);
	if ( w_state == STATE_PAUSE ) 
		input_pause();
	return TRUE;
}

BOOL CMP3Input::input_setStepPitch(int pitch, int frames)
{
	//WinNT ébíËëŒçÙ
	if ( m_platForm == 1 ) {
		m_errNo = ERR_PLATFORM_NT;
		return FALSE;
	}

	if ( pitch < 1 || frames < 1)
	{
		m_errNo = ERR_INVALID_VALUE;
		return FALSE;
	}
	int w_state = m_state;
	int curSample = output_getCurrentSample() + m_firstSample;

	if ( pitch == 1 ) {
		m_outputBlock_in = m_outputBlock_in_tmp;
		m_stepFlag = FALSE;
	} else {
		if ( m_stepFlag == FALSE ) m_outputBlock_in_tmp = m_outputBlock_in; 
		m_outputBlock_in = frames;
		m_stepFlag = TRUE;
	}

	output_setStepPitch(pitch);
	if ( m_state != STATE_STOP) {
		input_stop();
		input_play();
		input_seekBySample(curSample);
		if ( w_state == STATE_PAUSE )
			input_pause();
	}
	return TRUE;
}

int CMP3Input::input_getStepPitch()
{
	return output_getStepPitch();
}

BOOL CMP3Input::input_setPitch(int pitch)
{
	if ( pitch < 20 )
	{
		m_errNo = ERR_INVALID_VALUE;
		return FALSE;
	}
	output_setPitch(pitch);
	int w_state = m_state;
	int curSample = output_getCurrentSample() + m_firstSample;
	if ( m_state != STATE_STOP) {
		input_stop();
		input_play();
		input_seekBySample(curSample);
		if ( w_state == STATE_PAUSE )
			input_pause();
	}
	return TRUE;
}

int CMP3Input::input_getPitch()
{
	return output_getPitch();
}

void CMP3Input::input_callBackTime()
{
	if ( callBackFlag == TRUE ) {
		m_playSamples = output_getCurrentSample();
		m_playSec = (int)(input_sampleToSec(m_playSamples + m_firstSample) * m_playSpeed);
		if ( m_timeFlag && m_playSec > m_totalSec ) m_playSec = m_totalSec;
		m_pProc(INPUT_MSG_PLAY, m_playSec);
	}
}

long CMP3Input::input_getWinampPlayMs()
{
	long winampMs;
	if ( m_decInfo.frequency == 0 || m_totalSec == 0 ) {
		return 0;
	}

	//î{ë¨ââëtëŒâû
	double playMs;
	int playSamples;
	playSamples = output_getCurrentSample();
	playMs = (((float)playSamples + (float)m_firstSample) * (float)1000 / (float)m_decInfo.frequency) / (float)1000;
	playMs = playMs * m_playSpeed;
	playMs = playMs * (float)((float)m_winampTotalSec/(float)m_totalSec);
	winampMs = (long)(playMs * 1000);
	if (winampMs > (m_winampTotalSec * 1000) ) winampMs = m_winampTotalSec * 1000;
	return winampMs;
}

int CMP3Input::input_getWinampTotalSec()
{
	return m_winampTotalSec;
}

int CMP3Input::input_getPlayBitRate()
{
	if ( m_waveFlag == FALSE ) {
		return m_bitRate / 1000;
	} else {
		return m_bitRate;
	}
}

int CMP3Input::input_getPlayFlames()
{
	m_playFrames = 0;

	//ÉtÉåÅ[ÉÄêîéÊìæ
	if ( m_decInfo.outputSize > 0 ){
		if ( decFlag == 1 ){	
			m_playFrames = (int)(((m_firstSample + input_bytesToSample(decodePos)) * m_decInfo.bitsPerSample * m_decInfo.channels / 8) / m_decInfo.outputSize);
		} else {
			m_playFrames = (int)(output_sampleToBytes(output_getCurrentSample() + m_firstSample) / m_decInfo.outputSize);
		}
	}
	return m_playFrames;
}

long CMP3Input::input_getPlaySamples()
{
	if ( decFlag == 1 ) {
		return (m_firstSample + input_bytesToSample(decodePos));
	}
	return output_getCurrentSample() + m_firstSample;
}

int CMP3Input::input_getTotalSamples()
{
	return input_bytesToSample(m_decInfo.frames * m_decInfo.outputSize);
}

int CMP3Input::getFileType(const char* pszName)
{
	HANDLE			hFileType;				//É^ÉOï“èWÉtÉ@ÉCÉã
	BYTE			buf[1024];
	BOOL			result;
	DWORD			bytesRead;
	int				fType = 0;
	RiffChunk		riff;
	CommonChunk		common;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		return -1;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	//hFileType = ::CreateFile(pszName, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ,
	hFileType = ::CreateFile(pszName, GENERIC_READ , FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileType == INVALID_HANDLE_VALUE) {
		return -1;
	}

	//RIFFå`éÆÅAID3 Tag v2ÇÉ`ÉFÉbÉN
	SetFilePointer(hFileType, 0, NULL, FILE_BEGIN);
	result = ReadFile(hFileType, buf, 4, &bytesRead, NULL);
	if (!result || bytesRead != 4) {
		::CloseHandle(hFileType);
		return -1;
	}
	if (memcmp(buf, "RIFF", 4) == 0) {
		//ÉtÉ@ÉCÉãÇÃì™Ç…ñﬂÇËÅARIFFÉ`ÉÉÉìÉNÇì«Ç›çûÇﬁ
		::SetFilePointer(hFileType, 0, NULL, FILE_BEGIN);
		result = ::ReadFile(hFileType, &riff, sizeof(riff), &bytesRead, NULL);
		if (!result || bytesRead != sizeof(riff)) {
			::CloseHandle(hFileType);
			return -1;
		}
		if (memcmp(riff.formType, "WAVE", 4) == 0) {
			fType += 1;
		}
		if (memcmp(riff.formType, "RMP3", 4) == 0) {
			fType += 2;
		}
		//dataÉ`ÉÉÉìÉNÇíTÇ∑
		for (;;) {
			result = ::ReadFile(hFileType, &common, sizeof(common), &bytesRead, NULL);
			if (!result || bytesRead != sizeof(common)) {
				::CloseHandle(hFileType);
				return -1;
			}

			if (::memcmp(common.chunkID, "data", 4) == 0) {
				//dataÉ`ÉÉÉìÉNî≠å©
				break;
			}
			else {
				//éüÇÃÉ`ÉÉÉìÉNÇ‹Ç≈ì«Ç›îÚÇŒÇ∑
				::SetFilePointer(hFileType, common.chunkSize, NULL, FILE_CURRENT);
			}
		}
		result = ReadFile(hFileType, buf, 4, &bytesRead, NULL);
		if (!result || bytesRead != 4) {
			::CloseHandle(hFileType);
			return -1;
		}
		if (memcmp(buf, "ID3", 3) == 0) {
			fType += 4;
		}
	}
	else if (memcmp(buf, "ID3", 3) == 0) {
		fType += 4;
	}
	else if (buf[0] == 0x00) {
		fType += 8;
	}

	SetFilePointer(hFileType, -(long)128, NULL, FILE_END);
	result = ReadFile(hFileType, buf, 128, &bytesRead, NULL);
	if (!result || bytesRead != 128) {
		::CloseHandle(hFileType);
		return -1;
	}
	if (memcmp(buf, "TAG", 3) == 0) {
		fType += 16;
	}

	//LYRICS3ë∂ç›É`ÉFÉbÉN
	DWORD lysize;
	if (chkLYRICS3(hFileType, &lysize)) {
		fType += 32;
	}

	::CloseHandle(hFileType);
	return fType;

}


BOOL CMP3Input::input_cutMacBinary(const char* pszName)
{
	BYTE*			fBuf;
	HANDLE			hFileMac;				//É^ÉOï“èWÉtÉ@ÉCÉã
	BYTE			buf[1024];
	BOOL			result;
	DWORD			bytesRead;
	DWORD			bytesWrite;
	DWORD			fSize;
	DWORD           target = 0;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileMac = ::CreateFile(pszName, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileMac == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//É}ÉbÉNÉoÉCÉiÉäÉ`ÉFÉbÉN
	SetFilePointer(hFileMac, 0, NULL, FILE_BEGIN);
	result = ReadFile(hFileMac, buf, 4, &bytesRead, NULL);
	if (!result || bytesRead != 4) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(hFileMac);
		return FALSE;
	}
	if (buf[0] != 0x00) {
		::CloseHandle(hFileMac);
		return TRUE;
	}

	fSize = GetFileSize(hFileMac, NULL) - 128;

	fBuf = (BYTE*)::malloc(fSize);
	if (fBuf == NULL){
		m_errNo = ERR_MALLOC;
		::CloseHandle(hFileMac);
		return FALSE;
	}

	SetFilePointer(hFileMac, 128, NULL, FILE_BEGIN);
	result = ::ReadFile(hFileMac, fBuf, fSize, &bytesRead, NULL);
	if (!result || bytesRead != fSize) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(hFileMac);
		return FALSE;
	}

	for (target = fSize - 1; target > 0 && fBuf[target - 1] == 0; target --) {}

	::SetFilePointer(hFileMac, 0, NULL, FILE_BEGIN);
	result = ::WriteFile(hFileMac, fBuf, target, &bytesWrite, NULL);
	if (!result || bytesWrite != target) {
		m_errNo = ERR_MP3_FILE_WRITE;
		::CloseHandle(hFileMac);
		return FALSE;
	}

	::SetEndOfFile(hFileMac);
	::CloseHandle(hFileMac);
	::free(fBuf);
	return TRUE;

}


BOOL CMP3Input::input_changeWav(const char* pszName)
{
	BOOL			Result;
	HANDLE			hFileMP3;				//É^ÉOï“èWÉtÉ@ÉCÉã
	BYTE			buf[1024];
	BOOL			result;
	DWORD			bytesRead;
	DWORD			bytesWrite;
	DWORD			fSize;
	DWORD			dSize;
	DWORD			siSize;
	DWORD           target = 0;
	BYTE			tag[128];
	int				tagFlag = 0;
	int				lyFlag = 0;
	BYTE*			lyBuf;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileMP3 = ::CreateFile(pszName, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileMP3 == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	fSize = ::GetFileSize(hFileMP3, NULL);
	dSize = fSize;
	siSize = 298 - 8;

	//RIFFå`éÆÅAID3 Tag v2ÇÉ`ÉFÉbÉN
	SetFilePointer(hFileMP3, 0, NULL, FILE_BEGIN);
	result = ReadFile(hFileMP3, buf, 4, &bytesRead, NULL);
	if (!result || bytesRead != 4) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(hFileMP3);
		return FALSE;
	}
	if (memcmp(buf, "RIFF", 4) == 0) {
		m_errNo = ERR_NON_RIFF;
		::CloseHandle(hFileMP3);
		return FALSE;
	}
/*
	else if (memcmp(buf, "ID3", 3) == 0) {
		::CloseHandle(hFileMP3);
		return FALSE;
	}
*/
	else if (buf[0] == 0x00) {
		m_errNo = ERR_MAC_BIN;
		::CloseHandle(hFileMP3);
		return FALSE;
	}

	SetFilePointer(hFileMP3, -(long)128, NULL, FILE_END);
	result = ReadFile(hFileMP3, tag, 128, &bytesRead, NULL);
	if (!result || bytesRead != 128) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(hFileMP3);
		return FALSE;
	}
	if (memcmp(tag, "TAG", 3) == 0) {
		dSize -= 128;
		//siSize += 8 + 128;
		fSize -= 128;
		tagFlag = 1;
	}

	//LYRICS3ë∂ç›É`ÉFÉbÉN
	DWORD lysize = 0;			//é¿ÉTÉCÉY
	DWORD lyfsize;			//ÉtÉBÅ[ÉãÉhÉTÉCÉY
	DWORD lysp = 0;			//ÉXÉ^Å[Égà íu
	if (chkLYRICS3(hFileMP3, &lysize)) {
		if (m_Lyrics3InsertPoint == 0 ) {
			siSize = siSize + lysize - 10;
		} else if ( m_Lyrics3InsertPoint == 1 ) {
			siSize = siSize + lysize;
		}
		lyfsize = lysize;
		dSize -= lysize;
		fSize -= lysize;
		lyFlag = 1;

		if ( m_Lyrics3InsertPoint != 2 ) {
			//LYRICS3 äÔêîÉ`ÉFÉbÉN
			if ((lysize % 2) == 1) {
				lyfsize++;
				siSize++;
				lysp = 1;
				lyBuf = (BYTE*)::malloc(lyfsize);
				lyBuf[0] = 0x00;
			} else {
				lyBuf = (BYTE*)::malloc(lysize);
			}

			if (lyBuf == NULL){
				m_errNo = ERR_MALLOC;
				::CloseHandle(hFileMP3);
				::free(lyBuf);
				return FALSE;
			}

			//É|ÉCÉìÉ^à⁄ìÆ
			if ( tagFlag == 1 ){
				SetFilePointer(hFileMP3, -(long)(128+lysize), NULL, FILE_END);
			} else {
				SetFilePointer(hFileMP3, -(long)(lysize), NULL, FILE_END);
			}

			result = ReadFile(hFileMP3, &lyBuf[lysp], lysize, &bytesRead, NULL);
			if (!result || bytesRead != lysize) {
				m_errNo = ERR_MP3_FILE_READ;
				::CloseHandle(hFileMP3);
				::free(lyBuf);
				return FALSE;
			}
			lysize = lyfsize;
		}
	}

	fSize = fSize + 12 + 38 + 4 + siSize + 12;  //fSize + Riff + frm + data + siSize + List

	::memset(&buf, 0, sizeof(buf));
	::memcpy(&buf[0], "RIFF", 4);
	buf[4] = (BYTE)((fSize - 8) & 0x000000ff);
	buf[5] = (BYTE)(((fSize - 8) & 0x0000ffff) >> 8);
	buf[6] = (BYTE)(((fSize - 8) & 0x00ffffff) >> 16);
	buf[7] = (BYTE)(((fSize - 8) & 0xffffffff) >> 24);
    ::memcpy(&buf[8], "WAVE", 4);
    ::memcpy(&buf[12], "fmt ", 4);
	target = 16;
	buf[target] = 0x1e;
	buf[++target] = 0x00;
	buf[++target] = 0x00;
	buf[++target] = 0x00;
	buf[++target] = 0x55;
	buf[++target] = 0x00;
	buf[++target] = 0x02;
	buf[++target] = 0x00;
	buf[++target] = 0x44;
	buf[++target] = 0xac;
	buf[++target] = 0x00;
	buf[++target] = 0x00;
	buf[++target] = 0x80;
	buf[++target] = 0x3e;
	buf[++target] = 0x00;
	buf[++target] = 0x00;
	buf[++target] = 0x01;
	buf[++target] = 0x00;
	buf[++target] = 0x00;
	buf[++target] = 0x00;
	buf[++target] = 0x0c;
	buf[++target] = 0x00;
	buf[++target] = 0x01;
	buf[++target] = 0x00;
	buf[++target] = 0x02;
	buf[++target] = 0x00;
	buf[++target] = 0x00;
	buf[++target] = 0x00;
	buf[++target] = 0xa1;
	buf[++target] = 0x01;
	buf[++target] = 0x01;
	buf[++target] = 0x00;
	buf[++target] = 0x71;
	buf[++target] = 0x05;
	target++;
	::memcpy(&buf[target], "data", 4);
	target += 4;
	buf[target] = (BYTE)(dSize & 0x000000ff);
	buf[++target] = (BYTE)((dSize & 0x0000ffff) >> 8);
	buf[++target] = (BYTE)((dSize & 0x00ffffff) >> 16);
	buf[++target] = (BYTE)((dSize & 0xffffffff) >> 24);
	target++;

	Result = insertFileDataBegin(hFileMP3, (BYTE*)buf, target);

	::memset(&buf, 0, sizeof(buf));
	::memcpy(&buf[0], "LIST", 4);
	buf[4] = (BYTE)(siSize & 0x000000ff);
	buf[5] = (BYTE)((siSize & 0x0000ffff) >> 8);
	buf[6] = (BYTE)((siSize & 0x00ffffff) >> 16);
	buf[7] = (BYTE)((siSize & 0xffffffff) >> 24);
    ::memcpy(&buf[8], "INFO", 4);

	target = 12;
    ::memcpy(&buf[target], "INAM", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "IART", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "IPRD", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "ICMT", 4);
	buf[target + 4] = 1;
    
	target += 10;
	::memcpy(&buf[target], "IGNR", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "ICRD", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "ICOP", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "IENG", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "ISRC", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "ISFT", 4);
	buf[target + 4] = 0x0b;
	::memcpy(&buf[target + 8], "xmMP3", 11);

	target += 20;
    ::memcpy(&buf[target], "IKEY", 4);
	buf[target + 4] = 1;
    
	target += 10;
	::memcpy(&buf[target], "ITCH", 4);
	buf[target + 4] = 1;
    
	target += 10;
    ::memcpy(&buf[target], "ICMS", 4);
	buf[target + 4] = 1;

	target += 10;
	::memcpy(&buf[target], "ILYC", 4);

	if ( lyFlag == 0 || m_Lyrics3InsertPoint == 2 ) {
		buf[target + 4] = 1;

		target += 10;
		::memcpy(&buf[target], "IID3", 4);
		buf[target + 4] = 0x80;

		target += 8;

		if ( tagFlag == 0 ) {
			int Cnt;
			::memcpy(&buf[target], "TAG", 3);
			for (Cnt = 3; Cnt < 127; Cnt++){
				::memcpy(&buf[target+Cnt], " ", 1);
			}
			buf[target+127] = 0xFA;
			SetFilePointer(hFileMP3, -(long)(lysize - lysp), NULL, FILE_END);
		} else {
			::memcpy(&buf[target], tag, 128);
			SetFilePointer(hFileMP3, -(long)(128 + lysize - lysp), NULL, FILE_END);
		}

		target += 128;
		result = ::WriteFile(hFileMP3, &buf, target, &bytesWrite, NULL);
		if (!result || bytesWrite != target) {
			m_errNo = ERR_MP3_FILE_WRITE;
			::CloseHandle(hFileMP3);
			::free(lyBuf);
			return FALSE;
		}
		::SetEndOfFile(hFileMP3);

	} else {
		if ( m_Lyrics3InsertPoint == 1 ) {
			buf[target + 4] = 1;

			target += 10;
			::memcpy(&buf[target], "IID3", 4);
		}

		buf[target + 4] = (BYTE)((lysize + 128) & 0x000000ff);
		buf[target + 5] = (BYTE)(((lysize + 128) & 0x0000ffff) >> 8);
		buf[target + 6] = (BYTE)(((lysize + 128) & 0x00ffffff) >> 16);
		buf[target + 7] = (BYTE)(((lysize + 128) & 0xffffffff) >> 24);

		target += 8;

		long seekPoint = lysize;
		if ( tagFlag == 1 ) {
			seekPoint += 128;
		}
		SetFilePointer(hFileMP3, -(long)(seekPoint - lysp), NULL, FILE_END);

		//SIÉtÉBÅ[ÉãÉh ë}ì¸
		result = ::WriteFile(hFileMP3, &buf, target, &bytesWrite, NULL);
		if (!result || bytesWrite != target) {
			m_errNo = ERR_MP3_FILE_WRITE;
			::CloseHandle(hFileMP3);
			::free(lyBuf);
			return FALSE;
		}

		//LYRICS3 ë}ì¸
		if ( lysize != 0 ) {
			result = ::WriteFile(hFileMP3, lyBuf, lysize, &bytesWrite, NULL);
			if (!result || bytesWrite != lysize) {
				m_errNo = ERR_MP3_FILE_WRITE;
				::CloseHandle(hFileMP3);
				::free(lyBuf);
				return FALSE;
			}
		}

		//ID3Tag v1 ë}ì¸
		if ( tagFlag == 0 ) {
			int Cnt;
			::memcpy(&buf[0], "TAG", 3);
			for (Cnt = 3; Cnt < 127; Cnt++){
				::memcpy(&buf[Cnt], " ", 1);
			}
			buf[127] = 0xFA;
		} else {
			::memcpy(&buf[0], tag, 128);
		}
		result = ::WriteFile(hFileMP3, &buf, 128, &bytesWrite, NULL);
		if (!result || bytesWrite != 128) {
			m_errNo = ERR_MP3_FILE_WRITE;
			::CloseHandle(hFileMP3);
			::free(lyBuf);
			return FALSE;
		}

	}

	::CloseHandle(hFileMP3);
	::free(lyBuf);
	return Result;

}


BOOL CMP3Input::input_changeMp3(const char* pszName)
{
	HANDLE			hFileList;				//É^ÉOï“èWÉtÉ@ÉCÉã
	BYTE*			fBuf;
	BYTE			buf[1024];
	BOOL			result;
	DWORD			bytesRead;
	DWORD			bytesWrite;
	DWORD			fSync;					//ìØä˙ÉwÉbÉ_ÇÃà íu
	DWORD			fLast;					//ÉfÅ[É^É`ÉÉÉìÉNÇÃç≈å„
	DWORD			dSize;
	DWORD           target = 0;
	BYTE			tag[128];
	int				tagFlag = 0;
	RiffChunk		riff;
	CommonChunk		common;
	int				lyFlag = 0;
	BYTE*			lyBuf;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileList = ::CreateFile(pszName, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileList == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	fSync = 0;
	dSize = GetFileSize(hFileList, NULL);

	//RIFFå`éÆÅAID3 Tag v2ÇÉ`ÉFÉbÉN
	SetFilePointer(hFileList, 0, NULL, FILE_BEGIN);
	result = ReadFile(hFileList, buf, 4, &bytesRead, NULL);
	if (memcmp(buf, "RIFF", 4) != 0) {
		m_errNo = ERR_NON_RIFF;
		::CloseHandle(hFileList);
		return FALSE;
	} else {
		//ÉtÉ@ÉCÉãÇÃì™Ç…ñﬂÇËÅARIFFÉ`ÉÉÉìÉNÇì«Ç›çûÇﬁ
		::SetFilePointer(hFileList, 0, NULL, FILE_BEGIN);
		result = ::ReadFile(hFileList, &riff, sizeof(riff), &bytesRead, NULL);
		if (!result || bytesRead != sizeof(riff)) {
			m_errNo = ERR_MP3_FILE_READ;
			::CloseHandle(hFileList);
			return FALSE;
		}
		fSync = sizeof(riff);
		//dataÉ`ÉÉÉìÉNÇíTÇ∑
		for (;;) {
			result = ::ReadFile(hFileList, &common, sizeof(common), &bytesRead, NULL);
			if (!result || bytesRead != sizeof(common)) {
				m_errNo = ERR_MP3_FILE_READ;
				::CloseHandle(hFileList);
				return FALSE;
			}

			if (::memcmp(common.chunkID, "data", 4) == 0) {
				//dataÉ`ÉÉÉìÉNî≠å©
				break;
			}
			else {
				//éüÇÃÉ`ÉÉÉìÉNÇ‹Ç≈ì«Ç›îÚÇŒÇ∑
				fSync = ::SetFilePointer(hFileList, common.chunkSize, NULL, FILE_CURRENT);
			}
		}

		//É`ÉÉÉìÉNÉTÉCÉYÉ`ÉFÉbÉN
		if (common.chunkSize < dSize) {
			dSize = common.chunkSize;
		}

		fSync += bytesRead;
		fLast = fSync + dSize;
	}

	fBuf = (BYTE*)::malloc(dSize);
	if (fBuf == NULL){
		::CloseHandle(hFileList);
		::free(fBuf);
		m_errNo = ERR_MALLOC;
		return FALSE;
	}

	//ID3Tag v1 É`ÉFÉbÉN
	SetFilePointer(hFileList, -(long)128, NULL, FILE_END);
	result = ReadFile(hFileList, tag, 128, &bytesRead, NULL);
	if (!result || bytesRead != 128) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(hFileList);
		::free(fBuf);
		return FALSE;
	}
	if (memcmp(tag, "TAG", 3) == 0) {
		tagFlag = 1;
	}

	//LYRICS3ë∂ç›É`ÉFÉbÉN
	DWORD lysize = 0;			//é¿ÉTÉCÉY
	if (chkLYRICS3(hFileList, &lysize)) {
		lyFlag = 1;

		lyBuf = (BYTE*)::malloc(lysize);
		if (lyBuf == NULL){
			m_errNo = ERR_MALLOC;
			::CloseHandle(hFileList);
			::free(lyBuf);
			::free(fBuf);
			return FALSE;
		}

		if ( tagFlag == 1 ){
			SetFilePointer(hFileList, -(long)(128+lysize), NULL, FILE_END);
		} else {
			SetFilePointer(hFileList, -(long)(lysize), NULL, FILE_END);
		}

		result = ReadFile(hFileList, lyBuf, lysize, &bytesRead, NULL);
		if (!result || bytesRead != lysize) {
			m_errNo = ERR_MP3_FILE_READ;
			::CloseHandle(hFileList);
			::free(lyBuf);
			return FALSE;
		}

	}

	SetFilePointer(hFileList, fSync, NULL, FILE_BEGIN);
	result = ::ReadFile(hFileList, fBuf, dSize, &bytesRead, NULL);
	if (!result || bytesRead != dSize) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(hFileList);
		::free(lyBuf);
		::free(fBuf);
		return FALSE;
	}

	::SetFilePointer(hFileList, 0, NULL, FILE_BEGIN);
	result = ::WriteFile(hFileList, fBuf, dSize, &bytesWrite, NULL);
	if (!result || bytesWrite != dSize) {
		m_errNo = ERR_MP3_FILE_WRITE;
		::CloseHandle(hFileList);
		::free(lyBuf);
		::free(fBuf);
		return FALSE;
	}

	if ( lyFlag == 1 ) {
		result = ::WriteFile(hFileList, lyBuf, lysize, &bytesWrite, NULL);
		if (!result || bytesWrite != lysize) {
			m_errNo = ERR_MP3_FILE_WRITE;
			::CloseHandle(hFileList);
			::free(lyBuf);
			::free(fBuf);
			return FALSE;
		}
	}

	if ( tagFlag == 1 ) {
		result = ::WriteFile(hFileList, &tag, 128, &bytesWrite, NULL);
		if (!result || bytesWrite != 128) {
			m_errNo = ERR_MP3_FILE_WRITE;
			::CloseHandle(hFileList);
			::free(lyBuf);
			::free(fBuf);
			return FALSE;
		}
	}

	::SetEndOfFile(hFileList);
	::CloseHandle(hFileList);
	::free(lyBuf);
	::free(fBuf);
	return TRUE;
}


BOOL CMP3Input::setListInfo(const char* pszName, LIST_INFO* pListInfo)
{
	HANDLE			hFileList;				//É^ÉOï“èWÉtÉ@ÉCÉã
	BYTE			buf[102400];
	//BYTE			buf[2048];
	BOOL			result;
	DWORD			bytesRead;
	DWORD			bytesWrite;
	DWORD			fSync;					//ìØä˙ÉwÉbÉ_ÇÃà íu
	DWORD			fLast;					//ÉfÅ[É^É`ÉÉÉìÉNÇÃç≈å„
	DWORD			fSize;
	DWORD			dSize;
	DWORD			listSize;
	DWORD           target = 0;
	BYTE			tag[128];
	int				tagFlag = 0;
	RiffChunk		riff;
	CommonChunk		common;
	int				lyFlag = 0;
	BYTE*			lyBuf;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileList = ::CreateFile(pszName, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileList == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	fSync = 0;
	dSize = GetFileSize(hFileList, NULL);

	//RIFFå`éÆÅAID3 Tag v2ÇÉ`ÉFÉbÉN
	SetFilePointer(hFileList, 0, NULL, FILE_BEGIN);
	result = ReadFile(hFileList, buf, 4, &bytesRead, NULL);
	if (memcmp(buf, "RIFF", 4) != 0) {
		::CloseHandle(hFileList);
		m_errNo = ERR_NON_RIFF;
		return FALSE;
	} else {
		//ÉtÉ@ÉCÉãÇÃì™Ç…ñﬂÇËÅARIFFÉ`ÉÉÉìÉNÇì«Ç›çûÇﬁ
		::SetFilePointer(hFileList, 0, NULL, FILE_BEGIN);
		result = ::ReadFile(hFileList, &riff, sizeof(riff), &bytesRead, NULL);
		if (!result || bytesRead != sizeof(riff)) {
			::CloseHandle(hFileList);
			m_errNo = ERR_MP3_FILE_READ;
			return FALSE;
		}
		fSync = sizeof(riff);
		//dataÉ`ÉÉÉìÉNÇíTÇ∑
		for (;;) {
			result = ::ReadFile(hFileList, &common, sizeof(common), &bytesRead, NULL);
			if (!result || bytesRead != sizeof(common)) {
				m_errNo = ERR_MP3_FILE_READ;
				::CloseHandle(hFileList);
				return FALSE;
			}

			if (::memcmp(common.chunkID, "data", 4) == 0) {
				//dataÉ`ÉÉÉìÉNî≠å©
				break;
			}
			else {
				//éüÇÃÉ`ÉÉÉìÉNÇ‹Ç≈ì«Ç›îÚÇŒÇ∑
				fSync = ::SetFilePointer(hFileList, common.chunkSize, NULL, FILE_CURRENT);
			}
		}

		//É`ÉÉÉìÉNÉTÉCÉYÉ`ÉFÉbÉN
		if (common.chunkSize < dSize) {
			dSize = common.chunkSize;
		}

		fSync += bytesRead;
		fLast = fSync + dSize;
	}

	//ID3Tag v1É`ÉFÉbÉN
	SetFilePointer(hFileList, -(long)128, NULL, FILE_END);
	result = ReadFile(hFileList, tag, 128, &bytesRead, NULL);
	if (!result || bytesRead != 128) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(hFileList);
		return FALSE;
	}
	if (memcmp(tag, "TAG", 3) == 0) {
		tagFlag = 1;
	}

	//LYRICS3ë∂ç›É`ÉFÉbÉN
	DWORD lysize = 0;			//é¿ÉTÉCÉY
	DWORD lyfsize;			//ÉtÉBÅ[ÉãÉhÉTÉCÉY
	DWORD lysp = 0;			//ÉXÉ^Å[Égà íu
	if (chkLYRICS3(hFileList, &lysize)) {
		lyFlag = 1;
		lyfsize = lysize;

		//LYRICS3 äÔêîÉ`ÉFÉbÉN
		if ((lysize % 2) == 1) {
			lyfsize++;
			lysp = 1;
			lyBuf = (BYTE*)::malloc(lyfsize);
			lyBuf[0] = 0x00;
		} else {
			lyBuf = (BYTE*)::malloc(lysize);
		}

		if (lyBuf == NULL){
			m_errNo = ERR_MALLOC;
			::CloseHandle(hFileList);
			::free(lyBuf);
			return FALSE;
		}

		//É|ÉCÉìÉ^à⁄ìÆ
		if ( tagFlag == 1 ){
			SetFilePointer(hFileList, -(long)(128+lysize), NULL, FILE_END);
		} else {
			SetFilePointer(hFileList, -(long)(lysize), NULL, FILE_END);
		}

		result = ReadFile(hFileList, &lyBuf[lysp], lysize, &bytesRead, NULL);
		if (!result || bytesRead != lysize) {
			m_errNo = ERR_MP3_FILE_READ;
			::CloseHandle(hFileList);
			::free(lyBuf);
			return FALSE;
		}
		lysize = lyfsize;
	}

	//ÉfÅ[É^ÉTÉCÉYäÔêîëŒçÙ
	if ( (dSize % 2) == 1 ){
		buf[0] = 0x00;
		SetFilePointer(hFileList, fLast, NULL, FILE_BEGIN);
		result = ::WriteFile(hFileList, &buf, 1, &bytesWrite, NULL);
		if (!result || bytesWrite != 1) {
			m_errNo = ERR_MP3_FILE_WRITE;
			::CloseHandle(hFileList);
			::free(lyBuf);
			return FALSE;
		}
		fLast++;
	}

	::memset(&buf, 0, sizeof(buf));

	target = 12;
	listSize = ::strlen(pListInfo->INAM) + 1;
	for (listSize = INFO_NAME_SIZE; listSize > 0 && (pListInfo->INAM[listSize - 1] == ' ' ||
	                                                 pListInfo->INAM[listSize - 1] == '\0') ; listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "INAM", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->INAM, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = INFO_NAME_SIZE; listSize > 0 && (pListInfo->IART[listSize - 1] == ' ' ||
	                                                 pListInfo->IART[listSize - 1] == '\0') ; listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "IART", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->IART, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = INFO_NAME_SIZE; listSize > 0 && (pListInfo->IPRD[listSize - 1] == ' ' ||
	                                                 pListInfo->IPRD[listSize - 1] == '\0') ; listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "IPRD", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->IPRD, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = INFO_NAME_SIZE; listSize > 0 && (pListInfo->ICMT[listSize - 1] == ' ' || 
	                                                 pListInfo->ICMT[listSize - 1] == '\0') ; listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ICMT", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->ICMT, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = INFO_NAME_SIZE; listSize > 0 && (pListInfo->IGNR[listSize - 1] == ' ' || 
	                                                 pListInfo->IGNR[listSize - 1] == '\0') ; listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "IGNR", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->IGNR, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = INFO_NAME_SIZE; listSize > 0 && (pListInfo->ICRD[listSize - 1] == ' ' ||
	                                                 pListInfo->ICRD[listSize - 1] == '\0') ; listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ICRD", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->ICRD, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = INFO_NAME_SIZE; listSize > 0 && (pListInfo->ICOP[listSize - 1] == ' ' ||
	                                                 pListInfo->ICOP[listSize - 1] == '\0') ; listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ICOP", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->ICOP, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = INFO_NAME_SIZE; listSize > 0 && (pListInfo->IENG[listSize - 1] == ' ' ||
	                                                 pListInfo->IENG[listSize - 1] == '\0') ; listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "IENG", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->IENG, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = INFO_NAME_SIZE; listSize > 0 && (pListInfo->ISRC[listSize - 1] == ' ' ||
	                                                 pListInfo->ISRC[listSize - 1] == '\0') ; listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ISRC", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->ISRC, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = INFO_NAME_SIZE; listSize > 0 && (pListInfo->ISFT[listSize - 1] == ' ' ||
	                                                 pListInfo->ISFT[listSize - 1] == '\0') ; listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ISFT", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->ISFT, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = INFO_NAME_SIZE; listSize > 0 && (pListInfo->IKEY[listSize - 1] == ' ' ||
	                                                 pListInfo->IKEY[listSize - 1] == '\0') ; listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "IKEY", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->IKEY, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = INFO_NAME_SIZE; listSize > 0 && (pListInfo->ITCH[listSize - 1] == ' ' ||
	                                                 pListInfo->ITCH[listSize - 1] == '\0') ; listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ITCH", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->ITCH, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = INFO_NAME_SIZE; listSize > 0 && (pListInfo->ICMS[listSize - 1] == ' ' ||
	                                                 pListInfo->ICMS[listSize - 1] == '\0') ; listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ICMS", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->ICMS, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = INFO_NAME_SIZE; listSize > 0 && (pListInfo->ILYC[listSize - 1] == ' ' ||
	                                                 pListInfo->ILYC[listSize - 1] == '\0') ; listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ILYC", 4);

	if ( lyFlag == 0 || m_Lyrics3InsertPoint == 2 ) {
		buf[target + 4] = (BYTE)(listSize & 0x000000ff);
		buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
		buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
		buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
		::memcpy(&buf[target + 8], pListInfo->ILYC, listSize - 1);
		target += 8 + listSize;
		if ((listSize % 2) == 1 ) target += 1;

		::memcpy(&buf[target], "IID3", 4);
		buf[target + 4] = 0x80;

		target += 8;

		if ( tagFlag == 0 ) {
			int Cnt;
			::memcpy(&buf[target], "TAG", 3);
			for (Cnt = 3; Cnt < 127; Cnt++){
				::memcpy(&buf[target+Cnt], " ", 1);
			}
			buf[target+127] = 0xFA;
		} else {
			::memcpy(&buf[target], tag, 128);
		}

		target += 128;
	} else {
		if ( m_Lyrics3InsertPoint == 1 ) {
			buf[target + 4] = (BYTE)(listSize & 0x000000ff);
			buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
			buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
			buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
			::memcpy(&buf[target + 8], pListInfo->ILYC, listSize - 1);
			target += 8 + listSize;
			if ((listSize % 2) == 1 ) target += 1;

			::memcpy(&buf[target], "IID3", 4);
		}

		buf[target + 4] = (BYTE)((lysize + 128) & 0x000000ff);
		buf[target + 5] = (BYTE)(((lysize + 128) & 0x0000ffff) >> 8);
		buf[target + 6] = (BYTE)(((lysize + 128) & 0x00ffffff) >> 16);
		buf[target + 7] = (BYTE)(((lysize + 128) & 0xffffffff) >> 24);

		target += 8 + lysize + 128;
	}

	::memcpy(&buf[0], "LIST", 4);
	buf[4] = (BYTE)((target - 8) & 0x000000ff);
	buf[5] = (BYTE)(((target - 8) & 0x0000ffff) >> 8);
	buf[6] = (BYTE)(((target - 8) & 0x00ffffff) >> 16);
	buf[7] = (BYTE)(((target - 8) & 0xffffffff) >> 24);
    ::memcpy(&buf[8], "INFO", 4);

	SetFilePointer(hFileList, fLast, NULL, FILE_BEGIN);
	result = ::WriteFile(hFileList, &buf, target, &bytesWrite, NULL);
	if (!result || bytesWrite != target) {
		m_errNo = ERR_MP3_FILE_WRITE;
		::CloseHandle(hFileList);
		::free(lyBuf);
		return FALSE;
	}
	::SetEndOfFile(hFileList);

	if ( lyFlag != 0 && m_Lyrics3InsertPoint != 2 ) {
		SetFilePointer(hFileList, -(long)(lysize + 128), NULL, FILE_END);
		result = ::WriteFile(hFileList, lyBuf, lysize, &bytesWrite, NULL);
		if (!result || bytesWrite != lysize) {
			m_errNo = ERR_MP3_FILE_WRITE;
			::CloseHandle(hFileList);
			::free(lyBuf);
			return FALSE;
		}

		if ( tagFlag == 0 ) {
			int Cnt;
			::memcpy(&buf[0], "TAG", 3);
			for (Cnt = 3; Cnt < 127; Cnt++){
				::memcpy(&buf[target+Cnt], " ", 1);
			}
			buf[127] = 0xFA;
		} else {
			::memcpy(&buf[0], tag, 128);
		}

		result = ::WriteFile(hFileList, &buf, 128, &bytesWrite, NULL);
		if (!result || bytesWrite != 128) {
			m_errNo = ERR_MP3_FILE_WRITE;
			::CloseHandle(hFileList);
			::free(lyBuf);
			return FALSE;
		}

		::SetEndOfFile(hFileList);
	}

	fSize = fLast + target;

	::memset(&buf, 0, sizeof(buf));
	buf[0] = (BYTE)((fSize - 8) & 0x000000ff);
	buf[1] = (BYTE)(((fSize - 8) & 0x0000ffff) >> 8);
	buf[2] = (BYTE)(((fSize - 8) & 0x00ffffff) >> 16);
	buf[3] = (BYTE)(((fSize - 8) & 0xffffffff) >> 24);

	SetFilePointer(hFileList, 4, NULL, FILE_BEGIN);
	result = ::WriteFile(hFileList, &buf, 4, &bytesWrite, NULL);
	if (!result || bytesWrite != 4) {
		m_errNo = ERR_MP3_FILE_WRITE;
		::CloseHandle(hFileList);
		::free(lyBuf);
		return FALSE;
	}

	::CloseHandle(hFileList);
	::free(lyBuf);
	return TRUE;

}

BOOL CMP3Input::input_changeRmp(const char* pszName)
{
	BOOL			Result;
	HANDLE			hFileMP3;				//É^ÉOï“èWÉtÉ@ÉCÉã
	BYTE			buf[1024];
	BOOL			result;
	DWORD			bytesRead;
	DWORD			bytesWrite;
	DWORD			fSize;
	DWORD			dSize;
	DWORD			siSize;
	DWORD           target = 0;
	BYTE			tag[128];
	int				tagFlag = 0;
	int				lyFlag = 0;
	BYTE*			lyBuf;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileMP3 = ::CreateFile(pszName, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileMP3 == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	fSize = ::GetFileSize(hFileMP3, NULL);
	dSize = fSize;
	siSize = 298 - 8;

	//RIFFå`éÆÅAID3 Tag v2ÇÉ`ÉFÉbÉN
	SetFilePointer(hFileMP3, 0, NULL, FILE_BEGIN);
	result = ReadFile(hFileMP3, buf, 4, &bytesRead, NULL);
	if (!result || bytesRead != 4) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(hFileMP3);
		return FALSE;
	}
	if (memcmp(buf, "RIFF", 4) == 0) {
		m_errNo = ERR_RIFF;
		::CloseHandle(hFileMP3);
		return FALSE;
	}
/*
	else if (memcmp(buf, "ID3", 3) == 0) {
		::CloseHandle(hFileMP3);
		return FALSE;
	}
*/
	else if (buf[0] == 0x00) {
		m_errNo = ERR_MAC_BIN;
		::CloseHandle(hFileMP3);
		return FALSE;
	}

	SetFilePointer(hFileMP3, -(long)128, NULL, FILE_END);
	result = ReadFile(hFileMP3, tag, 128, &bytesRead, NULL);
	if (!result || bytesRead != 128) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(hFileMP3);
		return FALSE;
	}
	if (memcmp(tag, "TAG", 3) == 0) {
		dSize -= 128;
		//siSize += 8 + 128;
		fSize -= 128;
		tagFlag = 1;
	}

	//LYRICS3ë∂ç›É`ÉFÉbÉN
	DWORD lysize = 0;			//é¿ÉTÉCÉY
	DWORD lyfsize;			//ÉtÉBÅ[ÉãÉhÉTÉCÉY
	DWORD lysp = 0;			//ÉXÉ^Å[Égà íu
	if (chkLYRICS3(hFileMP3, &lysize)) {
		if (m_Lyrics3InsertPoint == 0 ) {
			siSize = siSize + lysize - 10;
		} else if ( m_Lyrics3InsertPoint == 1 ) {
			siSize = siSize + lysize;
		}
		lyfsize = lysize;
		dSize -= lysize;
		fSize -= lysize;
		lyFlag = 1;

		if ( m_Lyrics3InsertPoint != 2 ) {
			//LYRICS3 äÔêîÉ`ÉFÉbÉN
			if ((lysize % 2) == 1) {
				lyfsize++;
				siSize++;
				lysp = 1;
				lyBuf = (BYTE*)::malloc(lyfsize);
				lyBuf[0] = 0x00;
			} else {
				lyBuf = (BYTE*)::malloc(lysize);
			}

			if (lyBuf == NULL){
				m_errNo = ERR_MALLOC;
				::CloseHandle(hFileMP3);
				::free(lyBuf);
				return FALSE;
			}

			//É|ÉCÉìÉ^à⁄ìÆ
			if ( tagFlag == 1 ){
				SetFilePointer(hFileMP3, -(long)(128+lysize), NULL, FILE_END);
			} else {
				SetFilePointer(hFileMP3, -(long)(lysize), NULL, FILE_END);
			}

			result = ReadFile(hFileMP3, &lyBuf[lysp], lysize, &bytesRead, NULL);
			if (!result || bytesRead != lysize) {
				m_errNo = ERR_MP3_FILE_READ;
				::CloseHandle(hFileMP3);
				::free(lyBuf);
				return FALSE;
			}
			lysize = lyfsize;
		}
	}

	fSize = fSize + 12 + 4 + siSize + 12;  //fSize + Riff + data + siSize + List

	::memset(&buf, 0, sizeof(buf));
	::memcpy(&buf[0], "RIFF", 4);
	buf[4] = (BYTE)((fSize - 8) & 0x000000ff);
	buf[5] = (BYTE)(((fSize - 8) & 0x0000ffff) >> 8);
	buf[6] = (BYTE)(((fSize - 8) & 0x00ffffff) >> 16);
	buf[7] = (BYTE)(((fSize - 8) & 0xffffffff) >> 24);
    ::memcpy(&buf[8], "RMP3", 4);
	::memcpy(&buf[12], "data", 4);
	buf[16] = (BYTE)(dSize & 0x000000ff);
	buf[17] = (BYTE)((dSize & 0x0000ffff) >> 8);
	buf[18] = (BYTE)((dSize & 0x00ffffff) >> 16);
	buf[19] = (BYTE)((dSize & 0xffffffff) >> 24);

	Result = insertFileDataBegin(hFileMP3, (BYTE*)buf, 20);

	::memset(&buf, 0, sizeof(buf));
	::memcpy(&buf[0], "LIST", 4);
	buf[4] = (BYTE)(siSize & 0x000000ff);
	buf[5] = (BYTE)((siSize & 0x0000ffff) >> 8);
	buf[6] = (BYTE)((siSize & 0x00ffffff) >> 16);
	buf[7] = (BYTE)((siSize & 0xffffffff) >> 24);
    ::memcpy(&buf[8], "INFO", 4);

	target = 12;
    ::memcpy(&buf[target], "INAM", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "IART", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "IPRD", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "ICMT", 4);
	buf[target + 4] = 1;
    
	target += 10;
	::memcpy(&buf[target], "IGNR", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "ICRD", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "ICOP", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "IENG", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "ISRC", 4);
	buf[target + 4] = 1;

	target += 10;
    ::memcpy(&buf[target], "ISFT", 4);
	buf[target + 4] = 0x0b;
	::memcpy(&buf[target + 8], "xmMP3", 11);

	target += 20;
    ::memcpy(&buf[target], "IKEY", 4);
	buf[target + 4] = 1;
    
	target += 10;
	::memcpy(&buf[target], "ITCH", 4);
	buf[target + 4] = 1;
    
	target += 10;
    ::memcpy(&buf[target], "ICMS", 4);
	buf[target + 4] = 1;

	target += 10;
	::memcpy(&buf[target], "ILYC", 4);

	if ( lyFlag == 0 || m_Lyrics3InsertPoint == 2 ) {
		buf[target + 4] = 1;

		target += 10;
		::memcpy(&buf[target], "IID3", 4);
		buf[target + 4] = 0x80;

		target += 8;

		if ( tagFlag == 0 ) {
			int Cnt;
			::memcpy(&buf[target], "TAG", 3);
			for (Cnt = 3; Cnt < 127; Cnt++){
				::memcpy(&buf[target+Cnt], " ", 1);
			}
			buf[target+127] = 0xFA;
			SetFilePointer(hFileMP3, -(long)(lysize - lysp), NULL, FILE_END);
		} else {
			::memcpy(&buf[target], tag, 128);
			SetFilePointer(hFileMP3, -(long)(128 + lysize - lysp), NULL, FILE_END);
		}

		target += 128;
		result = ::WriteFile(hFileMP3, &buf, target, &bytesWrite, NULL);
		if (!result || bytesWrite != target) {
			m_errNo = ERR_MP3_FILE_WRITE;
			::CloseHandle(hFileMP3);
			::free(lyBuf);
			return FALSE;
		}
		::SetEndOfFile(hFileMP3);

	} else {
		if ( m_Lyrics3InsertPoint == 1 ) {
			buf[target + 4] = 1;

			target += 10;
			::memcpy(&buf[target], "IID3", 4);
		}

		buf[target + 4] = (BYTE)((lysize + 128) & 0x000000ff);
		buf[target + 5] = (BYTE)(((lysize + 128) & 0x0000ffff) >> 8);
		buf[target + 6] = (BYTE)(((lysize + 128) & 0x00ffffff) >> 16);
		buf[target + 7] = (BYTE)(((lysize + 128) & 0xffffffff) >> 24);

		target += 8;

		long seekPoint = lysize - lysp;
		if ( tagFlag == 1 ) {
			seekPoint += 128;
		}
		SetFilePointer(hFileMP3, -seekPoint, NULL, FILE_END);

		//SIÉtÉBÅ[ÉãÉh ë}ì¸
		result = ::WriteFile(hFileMP3, &buf, target, &bytesWrite, NULL);
		if (!result || bytesWrite != target) {
			m_errNo = ERR_MP3_FILE_WRITE;
			::CloseHandle(hFileMP3);
			::free(lyBuf);
			return FALSE;
		}

		//LYRICS3 ë}ì¸
		if ( lysize != 0 ) {
			result = ::WriteFile(hFileMP3, lyBuf, lysize, &bytesWrite, NULL);
			if (!result || bytesWrite != lysize) {
				m_errNo = ERR_MP3_FILE_WRITE;
				::CloseHandle(hFileMP3);
				::free(lyBuf);
				return FALSE;
			}
		}

		//ID3Tag v1 ë}ì¸
		if ( tagFlag == 0 ) {
			int Cnt;
			::memcpy(&buf[0], "TAG", 3);
			for (Cnt = 3; Cnt < 127; Cnt++){
				::memcpy(&buf[Cnt], " ", 1);
			}
			buf[127] = 0xFA;
		} else {
			::memcpy(&buf[0], tag, 128);
		}
		result = ::WriteFile(hFileMP3, &buf, 128, &bytesWrite, NULL);
		if (!result || bytesWrite != 128) {
			m_errNo = ERR_MP3_FILE_WRITE;
			::CloseHandle(hFileMP3);
			::free(lyBuf);
			return FALSE;
		}
	}

	::CloseHandle(hFileMP3);
	::free(lyBuf);
	return Result;

}

BOOL CMP3Input::insertFileDataBegin(HANDLE hFileHandle, BYTE* lpData, DWORD dSize)
{
	BYTE*			fBuf;
	BOOL			result;
	DWORD			fSize;
	DWORD			bytesRead;
	DWORD			bytesWrite;

	//ÉtÉ@ÉCÉãñ¢ÉIÅ[ÉvÉì
	if (hFileHandle == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	fSize = ::GetFileSize(hFileHandle, NULL);
	fBuf = (BYTE*)::malloc(fSize);
	if (fBuf == NULL){
		m_errNo = ERR_MALLOC;
		return FALSE;
	}
	
	SetFilePointer(hFileHandle, 0, NULL, FILE_BEGIN);
	result = ReadFile(hFileHandle, fBuf, fSize, &bytesRead, NULL);
	if (!result || bytesRead != fSize) {
		//::CloseHandle(hFileHandle);
		m_errNo = ERR_MP3_FILE_READ;
		return FALSE;
	}

	::SetFilePointer(hFileHandle, 0, NULL, FILE_BEGIN);
	result = ::WriteFile(hFileHandle, lpData, dSize, &bytesWrite, NULL);
	if (!result || bytesWrite != dSize) {
		//::CloseHandle(hFileHandle);
		m_errNo = ERR_MP3_FILE_WRITE;
		return FALSE;
	}

	result = ::WriteFile(hFileHandle, fBuf, fSize, &bytesWrite, NULL);
	if (!result || bytesWrite != fSize) {
		//::CloseHandle(hFileHandle);
		m_errNo = ERR_MP3_FILE_WRITE;
		return FALSE;
	}

	::SetEndOfFile(hFileHandle);
	//::CloseHandle(hFileHandle);
	::free(fBuf);
	return TRUE;

}

BOOL CMP3Input::getListInfo(LIST_INFO* pListInfo)
{
	//ÉtÉ@ÉCÉãñ¢ÉIÅ[ÉvÉì
	if (m_hFile == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	::memset(pListInfo, 0, sizeof(LIST_INFO));

	//ÉäÉXÉgèÓïÒÉZÉbÉg
	::memcpy(pListInfo->IART,  listInfo.IART,  sizeof(listInfo.IART));
	::memcpy(pListInfo->ICMS,  listInfo.ICMS,  sizeof(listInfo.ICMS));
	::memcpy(pListInfo->ICMT,  listInfo.ICMT,  sizeof(listInfo.ICMT));
	::memcpy(pListInfo->ICOP,  listInfo.ICOP,  sizeof(listInfo.ICOP));
	::memcpy(pListInfo->ICRD,  listInfo.ICRD,  sizeof(listInfo.ICRD));
	::memcpy(pListInfo->IENG,  listInfo.IENG,  sizeof(listInfo.IENG));
	::memcpy(pListInfo->IGNR,  listInfo.IGNR,  sizeof(listInfo.IGNR));
	::memcpy(pListInfo->IKEY,  listInfo.IKEY,  sizeof(listInfo.IKEY));
	::memcpy(pListInfo->ILYC,  listInfo.ILYC,  sizeof(listInfo.ILYC));
	::memcpy(pListInfo->INAM,  listInfo.INAM,  sizeof(listInfo.INAM));
	::memcpy(pListInfo->IPRD,  listInfo.IPRD,  sizeof(listInfo.IPRD));
	::memcpy(pListInfo->ISFT,  listInfo.ISFT,  sizeof(listInfo.ISFT));
	::memcpy(pListInfo->ISRC,  listInfo.ISRC,  sizeof(listInfo.ISRC));
	::memcpy(pListInfo->ITCH,  listInfo.ITCH,  sizeof(listInfo.ITCH));
	
	return TRUE;
}

BOOL CMP3Input::getFileTagInfo(const char* pszName, TAG_INFO* pTagInfo)
{
	HANDLE			hFileMpeg;				//É^ÉOï“èWÉtÉ@ÉCÉã

	::memset(pTagInfo, 0, sizeof(TAG_INFO));

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileMpeg = ::CreateFile(pszName, GENERIC_READ, FILE_SHARE_READ + FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileMpeg == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ID3 Tagv1É`ÉFÉbÉN
	readID3v1(hFileMpeg, pTagInfo);

	CloseHandle(hFileMpeg);
	return TRUE;
}

BOOL CMP3Input::getFileTagInfoEX(const char* pszName, TAG_INFO_11* pTagInfo)
{
	HANDLE			hFileMpeg;				//É^ÉOï“èWÉtÉ@ÉCÉã

	memset(pTagInfo, 0, sizeof(TAG_INFO_11));

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileMpeg = ::CreateFile(pszName, GENERIC_READ, FILE_SHARE_READ + FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileMpeg == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ID3 Tagv1É`ÉFÉbÉN
	readID3v11(hFileMpeg, pTagInfo);

	::CloseHandle(hFileMpeg);
	return TRUE;
}

BOOL CMP3Input::getFileInfo(const char* pszName, TAG_INFO* pTagInfo, MPEG_INFO* pMpegInfo)
{
	HANDLE			hFileMpeg;				//É^ÉOï“èWÉtÉ@ÉCÉã
	BOOL			result;
	int				fSync;					//ìØä˙ÉwÉbÉ_ÇÃà íu
	DWORD			dSize;					//MPEG AudioÇÃÉfÅ[É^ÉTÉCÉY
	BYTE			buf[1024];
	DWORD			bytesRead;
	MPEG_DECODE_INFO dInfo;
	LIST_INFO		ListInfo;
	DWORD			fSize;
	int				padding;

	//èâä˙âª
	::memset(pTagInfo, 0, sizeof(TAG_INFO));
	::memset(pMpegInfo, 0, sizeof(MPEG_INFO));

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileMpeg = ::CreateFile(pszName, GENERIC_READ, FILE_SHARE_READ + FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileMpeg == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	fSync = 0;
	dSize = GetFileSize(hFileMpeg, NULL);
	fSize = dSize;

	SetFilePointer(hFileMpeg, 0, NULL, FILE_BEGIN);
	result = ReadFile(hFileMpeg, buf, 4, &bytesRead, NULL);
	if (!result || bytesRead != 4) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(hFileMpeg);
		return FALSE;
	}

	//RIFFå`éÆÅAID3 Tag v2ÇÉ`ÉFÉbÉN
	if (memcmp(buf, "RIFF", 4) == 0) {
		readRIFF(hFileMpeg, &ListInfo, &fSync, &dSize);
		if (RiffStatus != 0) {
			::CloseHandle(hFileMpeg);
			m_errNo = ERR_UNKNOWN_FILE;
			return FALSE;
		} else {
			if (readID3v2(hFileMpeg, &fSync)) {
				dSize -= fSync;
			}
		}
	}
	else if (memcmp(buf, "ID3", 3) == 0) {
		if (readID3v2(hFileMpeg, &fSync)) {
			dSize -= fSync;
		}
	}
	//Tag ñ≥ÇµÉfÅ[É^
	else if (buf[0] == 0x00) {
			fSync = 128;
	}
	else {
		fSync = 0;
	}

	//ìØä˙ÉwÉbÉ_éÊìæèÄîı
	::SetFilePointer(hFileMpeg, fSync, NULL, FILE_BEGIN);
	result = ::ReadFile(hFileMpeg, buf, sizeof(buf), &bytesRead, NULL);
	if (!result || bytesRead == 0) {
		::CloseHandle(hFileMpeg);
		m_errNo = ERR_MP3_FILE_READ;
		return FALSE;
	}
	//ìØä˙ÉwÉbÉ_ÇíTÇ∑
	int sync;
	if (!::mp3FindSync(buf, bytesRead, &sync)) {
		::CloseHandle(hFileMpeg);
		m_errNo = ERR_FRAME_HEADER_NOT_FOUND;
		return FALSE;
	}
	fSync += sync;

	//ìØä˙ÉwÉbÉ_à íuéÊìæ
	::SetFilePointer(hFileMpeg, fSync, NULL, FILE_BEGIN);
	result = ::ReadFile(hFileMpeg, buf, sizeof(buf), &bytesRead, NULL);
	if (!result || bytesRead == 0) {
		::CloseHandle(hFileMpeg);
		m_errNo = ERR_MP3_FILE_READ;
		return FALSE;
	}
	
	//ìØä˙ÉwÉbÉ_éÊìæ
	if (!::mp3GetDecodeInfo(buf, bytesRead, &dInfo, 0)) {
		m_errNo = ERR_FRAME_HEADER_READ;
		::CloseHandle(hFileMpeg);
		return FALSE;
	}

	//ID3 Tagv1É`ÉFÉbÉN
	if (readID3v1(hFileMpeg, pTagInfo)) {
		if (fSize == dSize) 
			dSize -= 128;
	}

	//LYRICS3ë∂ç›É`ÉFÉbÉN
	DWORD lysize;
	if (chkLYRICS3(hFileMpeg, &lysize)) {
		dSize -= lysize;
	}

	//MPEGèÓïÒÉRÉsÅ[
	pMpegInfo->version = dInfo.header.version;
	pMpegInfo->layer = dInfo.header.layer;
	pMpegInfo->crcDisable = (dInfo.header.error_prot) ? 0 : 1;
	pMpegInfo->mode = dInfo.header.mode;
	pMpegInfo->extension = dInfo.header.mode_ext;
	pMpegInfo->copyright = dInfo.header.copyright;
	pMpegInfo->original = dInfo.header.original;
	pMpegInfo->emphasis = dInfo.header.emphasis;
	pMpegInfo->channels = dInfo.channels;
	pMpegInfo->bitRate = dInfo.bitRate / 1000;
	pMpegInfo->fileSize = fSize;
	pMpegInfo->samplingRate = dInfo.frequency;
	padding = dInfo.header.padding;

	if (dInfo.frames == 0) {
		dInfo.frames = dSize / dInfo.minInputSize;
	}
	pMpegInfo->flames = dInfo.frames;
	int samples;
	int bytes = dInfo.frames * dInfo.outputSize;
	if ( bytes > ALLOW_DATA_SIZE ) {
		samples = (bytes / dInfo.bitsPerSample / dInfo.channels * 8);
	} else {
		samples = (bytes * 8 / dInfo.bitsPerSample / dInfo.channels);
	}

	pMpegInfo->totalSec = samples / dInfo.frequency;

	::CloseHandle(hFileMpeg);
	return TRUE;

}

BOOL CMP3Input::getFileInfoMtr(const char* pszName, TAG_INFO* pTagInfo, MPEG_INFO* pMpegInfo, LIST_INFO* pListInfo,int* pFileType)
{
	HANDLE			hFileMpeg;				//É^ÉOï“èWÉtÉ@ÉCÉã
	BOOL			result;
	int				fSync;					//ìØä˙ÉwÉbÉ_ÇÃà íu
	DWORD			dSize;					//MPEG AudioÇÃÉfÅ[É^ÉTÉCÉY
	BYTE			buf[1024];
	DWORD			bytesRead;
	MPEG_DECODE_INFO dInfo;
	DWORD			fSize;
	int				padding;

	//èâä˙âª
	::memset(pTagInfo, 0, sizeof(TAG_INFO));
	::memset(pMpegInfo, 0, sizeof(MPEG_INFO));
	::memset(pListInfo, 0, sizeof(LIST_INFO));
	m_FileType = 0;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileMpeg = ::CreateFile(pszName, GENERIC_READ, FILE_SHARE_READ + FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileMpeg == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	fSync = 0;
	dSize = GetFileSize(hFileMpeg, NULL);
	fSize = dSize;

	SetFilePointer(hFileMpeg, 0, NULL, FILE_BEGIN);
	result = ReadFile(hFileMpeg, buf, 4, &bytesRead, NULL);
	if (!result || bytesRead != 4) {
		::CloseHandle(hFileMpeg);
		m_errNo = ERR_MP3_FILE_READ;
		return FALSE;
	}

	//RIFFå`éÆÅAID3 Tag v2ÇÉ`ÉFÉbÉN
	if (memcmp(buf, "RIFF", 4) == 0) {
		readRIFF(hFileMpeg, pListInfo, &fSync, &dSize);
		if (RiffStatus != 0) {
			::CloseHandle(hFileMpeg);
			m_errNo = ERR_UNKNOWN_FILE;
			return FALSE;
		} else {
			if (readID3v2(hFileMpeg, &fSync)) {
				dSize -= fSync;
			}
		}
	}
	else if (memcmp(buf, "ID3", 3) == 0) {
		if (readID3v2(hFileMpeg, &fSync)) {
			dSize -= fSync;
		}
	}
	//Tag ñ≥ÇµÉfÅ[É^
	else if (buf[0] == 0x00) {
			m_FileType += 8;
			fSync = 128;
	}
	else {
		fSync = 0;
	}

	//ìØä˙ÉwÉbÉ_éÊìæèÄîı
	::SetFilePointer(hFileMpeg, fSync, NULL, FILE_BEGIN);
	result = ::ReadFile(hFileMpeg, buf, sizeof(buf), &bytesRead, NULL);
	if (!result || bytesRead == 0) {
		::CloseHandle(hFileMpeg);
		m_errNo = ERR_MP3_FILE_READ;
		return FALSE;
	}
	//ìØä˙ÉwÉbÉ_ÇíTÇ∑
	int sync;
	if (!::mp3FindSync(buf, bytesRead, &sync)) {
		::CloseHandle(hFileMpeg);
		m_errNo = ERR_FRAME_HEADER_NOT_FOUND;
		return FALSE;
	}
	fSync += sync;

	//ìØä˙ÉwÉbÉ_à íuéÊìæ
	::SetFilePointer(hFileMpeg, fSync, NULL, FILE_BEGIN);
	result = ::ReadFile(hFileMpeg, buf, sizeof(buf), &bytesRead, NULL);
	if (!result || bytesRead == 0) {
		::CloseHandle(hFileMpeg);
		m_errNo = ERR_MP3_FILE_READ;
		return FALSE;
	}

	//ìØä˙ÉwÉbÉ_éÊìæ
	if (!::mp3GetDecodeInfo(buf, bytesRead, &dInfo, 0)) {
		m_errNo = ERR_FRAME_HEADER_READ;
		::CloseHandle(hFileMpeg);
		return FALSE;
	}

	//ID3 Tagv1É`ÉFÉbÉN
	if (readID3v1(hFileMpeg, pTagInfo)) {
		if (fSize == dSize) 
			//m_FileType += 16;
			dSize -= 128;
	}

	//LYRICS3ë∂ç›É`ÉFÉbÉN
	DWORD lysize;
	if (chkLYRICS3(hFileMpeg, &lysize)) {
		dSize -= lysize;
	}

	*pFileType = m_FileType;

	//MPEGèÓïÒÉRÉsÅ[
	pMpegInfo->version = dInfo.header.version;
	pMpegInfo->layer = dInfo.header.layer;
	pMpegInfo->crcDisable = (dInfo.header.error_prot) ? 0 : 1;
	pMpegInfo->mode = dInfo.header.mode;
	pMpegInfo->extension = dInfo.header.mode_ext;
	pMpegInfo->copyright = dInfo.header.copyright;
	pMpegInfo->original = dInfo.header.original;
	pMpegInfo->emphasis = dInfo.header.emphasis;
	pMpegInfo->channels = dInfo.channels;
	pMpegInfo->bitRate = dInfo.bitRate / 1000;
	pMpegInfo->fileSize = fSize;
	pMpegInfo->samplingRate = dInfo.frequency;
	padding = dInfo.header.padding;

	if (dInfo.frames == 0) {
		dInfo.frames = dSize / dInfo.minInputSize;
	}
	pMpegInfo->flames = dInfo.frames;
	int samples;
	int bytes = dInfo.frames * dInfo.outputSize;
	if ( bytes > ALLOW_DATA_SIZE ) {
		samples = (bytes / dInfo.bitsPerSample / dInfo.channels * 8);
	} else {
		samples = (bytes * 8 / dInfo.bitsPerSample / dInfo.channels);
	}

	pMpegInfo->totalSec = samples / dInfo.frequency;

	::CloseHandle(hFileMpeg);
	return TRUE;
}


BOOL CMP3Input::getFileInfo2(const char* pszName, TAG_INFO* pTagInfo, MPEG_INFO* pMpegInfo, LIST_INFO* pListInfo)
{
	HANDLE			hFileMpeg;				//É^ÉOï“èWÉtÉ@ÉCÉã
	BOOL			result;
	int				fSync;					//ìØä˙ÉwÉbÉ_ÇÃà íu
	DWORD			dSize;					//MPEG AudioÇÃÉfÅ[É^ÉTÉCÉY
	BYTE			buf[1024];
	DWORD			bytesRead;
	MPEG_DECODE_INFO dInfo;
	DWORD			fSize;
	int				padding;

	//èâä˙âª
	::memset(pTagInfo, 0, sizeof(TAG_INFO));
	::memset(pMpegInfo, 0, sizeof(MPEG_INFO));
	::memset(pListInfo, 0, sizeof(LIST_INFO));

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileMpeg = ::CreateFile(pszName, GENERIC_READ, FILE_SHARE_READ + FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileMpeg == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	fSync = 0;
	dSize = GetFileSize(hFileMpeg, NULL);
	fSize = dSize;

	SetFilePointer(hFileMpeg, 0, NULL, FILE_BEGIN);
	result = ReadFile(hFileMpeg, buf, 4, &bytesRead, NULL);
	if (!result || bytesRead != 4) {
		::CloseHandle(hFileMpeg);
		m_errNo = ERR_MP3_FILE_READ;
		return FALSE;
	}

	//RIFFå`éÆÅAID3 Tag v2ÇÉ`ÉFÉbÉN
	if (memcmp(buf, "RIFF", 4) == 0) {
		readRIFF(hFileMpeg, pListInfo, &fSync, &dSize);
		if (RiffStatus != 0 ) {
			::CloseHandle(hFileMpeg);
			m_errNo = ERR_UNKNOWN_FILE;
			return FALSE;
		} else {
			if (readID3v2(hFileMpeg, &fSync)) {
				dSize -= fSync;
			}
		}
	}
	else if (memcmp(buf, "ID3", 3) == 0) {
		if (readID3v2(hFileMpeg, &fSync)) {
			dSize -= fSync;
		}
	}
	//Tag ñ≥ÇµÉfÅ[É^
	else if (buf[0] == 0x00) {
			fSync = 128;
	}
	else {
		fSync = 0;
	}
	//ìØä˙ÉwÉbÉ_éÊìæèÄîı
	::SetFilePointer(hFileMpeg, fSync, NULL, FILE_BEGIN);
	result = ::ReadFile(hFileMpeg, buf, sizeof(buf), &bytesRead, NULL);
	if (!result || bytesRead == 0) {
		::CloseHandle(hFileMpeg);
		m_errNo = ERR_MP3_FILE_READ;
		return FALSE;
	}

	//ìØä˙ÉwÉbÉ_ÇíTÇ∑
	int sync;
	if (!::mp3FindSync(buf, bytesRead, &sync)) {
		::CloseHandle(hFileMpeg);
		m_errNo = ERR_FRAME_HEADER_NOT_FOUND;
		return FALSE;
	}
	fSync += sync;

	//ìØä˙ÉwÉbÉ_à íuéÊìæ
	::SetFilePointer(hFileMpeg, fSync, NULL, FILE_BEGIN);
	result = ::ReadFile(hFileMpeg, buf, sizeof(buf), &bytesRead, NULL);
	if (!result || bytesRead == 0) {
		::CloseHandle(hFileMpeg);
		m_errNo = ERR_MP3_FILE_READ;
		return FALSE;
	}

	//ìØä˙ÉwÉbÉ_éÊìæ
	if (!::mp3GetDecodeInfo(buf, bytesRead, &dInfo, 0)) {
		::CloseHandle(hFileMpeg);
		m_errNo = ERR_FRAME_HEADER_READ;
		return FALSE;
	}

	//ID3 Tagv1É`ÉFÉbÉN
	if (readID3v1(hFileMpeg, pTagInfo)) {
		if (fSize == dSize) 
			dSize -= 128;
	}

	//LYRICS3ë∂ç›É`ÉFÉbÉN
	DWORD lysize;
	if (chkLYRICS3(hFileMpeg, &lysize)) {
		dSize -= lysize;
	}

	//MPEGèÓïÒÉRÉsÅ[
	pMpegInfo->version = dInfo.header.version;
	pMpegInfo->layer = dInfo.header.layer;
	pMpegInfo->crcDisable = (dInfo.header.error_prot) ? 0 : 1;
	pMpegInfo->mode = dInfo.header.mode;
	pMpegInfo->extension = dInfo.header.mode_ext;
	pMpegInfo->copyright = dInfo.header.copyright;
	pMpegInfo->original = dInfo.header.original;
	pMpegInfo->emphasis = dInfo.header.emphasis;
	pMpegInfo->channels = dInfo.channels;
	pMpegInfo->bitRate = dInfo.bitRate / 1000;
	pMpegInfo->fileSize = fSize;
	pMpegInfo->samplingRate = dInfo.frequency;
	padding = dInfo.header.padding;

	if (dInfo.frames == 0) {
		dInfo.frames = dSize / dInfo.minInputSize;
	}

	pMpegInfo->flames = dInfo.frames;
	int samples;
	int bytes = dInfo.frames * dInfo.outputSize;
	if ( bytes > ALLOW_DATA_SIZE ) {
		samples = (bytes / dInfo.bitsPerSample / dInfo.channels * 8);
	} else {
		samples = (bytes * 8 / dInfo.bitsPerSample / dInfo.channels);
	}

	pMpegInfo->totalSec = samples / dInfo.frequency;

	::CloseHandle(hFileMpeg);
	return TRUE;

}

BOOL CMP3Input::setTagInfo(const char* pszName, TAG_INFO* pTagInfo,int tagSet, int tagAdd)
{
	HANDLE			hFileTag;				//É^ÉOï“èWÉtÉ@ÉCÉã
	ID3Tagv1		tag;
	ID3Tagv1		wtag;
	BOOL			result;
	DWORD			bytesRead;
	DWORD			bytesWrite;
	BYTE			buf[1024];
	LIST_INFO		ListInfo;
	int				fSync;					//ìØä˙ÉwÉbÉ_ÇÃà íu
	DWORD			dSize;					//MPEG AudioÇÃÉfÅ[É^ÉTÉCÉY
	int				siFlag = 0;
	DWORD           target = 0;
	DWORD			fSize;
	DWORD			lSize;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileTag = ::CreateFile(pszName, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileTag == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	::memcpy(wtag.tag, "TAG",  3);
	::memcpy(wtag.albumName,  pTagInfo->szAlbumName,  30);
	::memcpy(wtag.artistName,  pTagInfo->szArtistName,  30);
	::memcpy(wtag.comment,  pTagInfo->szComment,  30);
	::memcpy(wtag.trackName,  pTagInfo->szTrackName,  30);
	::memcpy(wtag.year,  pTagInfo->szYear,  4);
	if ( pTagInfo->genre < 0 || pTagInfo->genre > 255 ) {
		pTagInfo->genre = 200;
	}
	wtag.genre = pTagInfo->genre;

	//RIFFå`éÆÉ`ÉFÉbÉN
	SetFilePointer(hFileTag, 0, NULL, FILE_BEGIN);
	result = ReadFile(hFileTag, buf, 4, &bytesRead, NULL);
	if (!result || bytesRead != 4) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(hFileTag);
		return FALSE;
	}
	if (memcmp(buf, "RIFF", 4) == 0) {
		readRIFF(hFileTag, &ListInfo, &fSync, &dSize);
		if ( m_listChunkPoint > 0 ) {
			siFlag = 1;
		} else {
			siFlag = 2;
		}
	}

	//ÉtÉ@ÉCÉãÇÃå„ÇÎÇ©ÇÁ128ÉoÉCÉgÇì«Ç›çûÇﬁ
	::SetFilePointer(hFileTag, -(long)sizeof(tag), NULL, FILE_END);
	result = ::ReadFile(hFileTag, &tag, sizeof(tag), &bytesRead, NULL);
	if (!result || bytesRead != sizeof(tag)) {
		::CloseHandle(hFileTag);
		m_errNo = ERR_MP3_FILE_READ;
		return FALSE;
	}
	if (::memcmp(tag.tag, "TAG", 3) != 0 && tagAdd == 0 ) {
		//ID3Tag v1 Ç™ñ≥Ç≠É^ÉOí«â¡ã÷é~
		::CloseHandle(hFileTag);
		m_errNo = ERR_MP3_FILE_WRITE;
		return FALSE;
	} else if (::memcmp(tag.tag, "TAG", 3) != 0 && tagAdd == 1 ) {
		//ID3Tag v1 Ç™ñ≥Ç≠É^ÉOí«â¡ãñâ¬
		if ( siFlag == 0 ) {
			//í èÌÇÃMP3ÉtÉ@ÉCÉã
			//ÉtÉ@ÉCÉãÇÃç≈å„Ç…èëÇ´çûÇﬁ
			::SetFilePointer(hFileTag, 0, NULL, FILE_END);
			result = ::WriteFile(hFileTag, &wtag, sizeof(wtag), &bytesWrite, NULL);
			if (!result || bytesWrite != sizeof(wtag)) {
				::CloseHandle(hFileTag);
				m_errNo = ERR_MP3_FILE_WRITE;
				return FALSE;
			}
		} else if ( siFlag == 1 ) {
			//SIÉtÉBÅ[ÉãÉhÇ™Ç†ÇÈÇ™IID3ÉtÉBÅ[ÉãÉhÇ™Ç»Ç¢
			//IID3ÉtÉBÅ[ÉãÉhí«â¡
			::memcpy(&buf[target], "IID3", 4);
			buf[target + 4] = 0x80;
			target += 8;
			::memcpy(&buf[target], &wtag, 128);
			target += 128;
			SetFilePointer(hFileTag, 0, NULL, FILE_END);
			result = ::WriteFile(hFileTag, &buf, target, &bytesWrite, NULL);
			if (!result || bytesWrite != target) {
				::CloseHandle(hFileTag);
				m_errNo = ERR_MP3_FILE_WRITE;
				return FALSE;
			}
			::SetEndOfFile(hFileTag);
			fSize = ::GetFileSize(m_hFile, NULL) - 8;
			lSize = m_listChunkSize + target;

			//LISTÉ`ÉÉÉìÉNÉTÉCÉYïœçX
			buf[0] = (BYTE)(fSize & 0x000000ff);
			buf[1] = (BYTE)((fSize & 0x0000ffff) >> 8);
			buf[2] = (BYTE)((fSize & 0x00ffffff) >> 16);
			buf[3] = (BYTE)((fSize & 0xffffffff) >> 24);
			SetFilePointer(hFileTag, m_listChunkPoint + 4, NULL, FILE_BEGIN);
			result = ::WriteFile(hFileTag, &buf, 4, &bytesWrite, NULL);
			if (!result || bytesWrite != 4) {
				::CloseHandle(hFileTag);
				m_errNo = ERR_MP3_FILE_WRITE;
				return FALSE;
			}

			//DATAÉ`ÉÉÉìÉNÉTÉCÉYïœçX
			buf[0] = (BYTE)(lSize & 0x000000ff);
			buf[1] = (BYTE)((lSize & 0x0000ffff) >> 8);
			buf[2] = (BYTE)((lSize & 0x00ffffff) >> 16);
			buf[3] = (BYTE)((lSize & 0xffffffff) >> 24);
			SetFilePointer(hFileTag,  4, NULL, FILE_BEGIN);
			result = ::WriteFile(hFileTag, &buf, 4, &bytesWrite, NULL);
			if (!result || bytesWrite != 4) {
				::CloseHandle(hFileTag);
				m_errNo = ERR_MP3_FILE_WRITE;
				return FALSE;
			}

		} else if ( siFlag == 2 ) {
			//SIÉtÉBÅ[ÉãÉhÇ™Ç»Ç¢
			//LISTèÓïÒèâä˙âª
			::memset(&ListInfo, ' ', sizeof(LIST_INFO));

			//LISTÉ`ÉÉÉìÉNí«â¡
			::CloseHandle(hFileTag);
			if (!setListInfo(pszName, &ListInfo)) {
				return FALSE;
			}

			//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
			hFileTag = ::CreateFile(pszName, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFileTag == INVALID_HANDLE_VALUE) {
				m_errNo = ERR_MP3_FILE_OPEN;
				return FALSE;
			}

			//ÉtÉ@ÉCÉãÇÃå„ÇÎÇ©ÇÁ128ÉoÉCÉgÇ…èëÇ´çûÇﬁ
			::SetFilePointer(hFileTag, -(long)sizeof(wtag), NULL, FILE_END);
			result = ::WriteFile(hFileTag, &wtag, sizeof(wtag), &bytesWrite, NULL);
			if (!result || bytesWrite != sizeof(wtag)) {
				::CloseHandle(hFileTag);
				m_errNo = ERR_MP3_FILE_WRITE;
				return FALSE;
			}
		}
	} else {
		//ID3Tag v1 óLÇË
		//ÉtÉ@ÉCÉãÇÃå„ÇÎÇ©ÇÁ128ÉoÉCÉgÇ…èëÇ´çûÇﬁ
		::SetFilePointer(hFileTag, -(long)sizeof(wtag), NULL, FILE_END);
		result = ::WriteFile(hFileTag, &wtag, sizeof(wtag), &bytesWrite, NULL);
		if (!result || bytesWrite != sizeof(wtag)) {
			::CloseHandle(hFileTag);
			m_errNo = ERR_MP3_FILE_WRITE;
			return FALSE;
		}
	}

	if ( tagSet == 1 ) {
		//É^ÉOèÓïÒÉZÉbÉg
		strcpy_sID3v1((char*)tagInfo.szTrackName, pTagInfo->szTrackName);
		strcpy_sID3v1((char*)tagInfo.szArtistName, pTagInfo->szArtistName);
		strcpy_sID3v1((char*)tagInfo.szAlbumName, pTagInfo->szAlbumName);
		::memcpy(tagInfo.szYear, pTagInfo->szYear, 5);
		strcpy_sID3v1((char*)tagInfo.szComment, pTagInfo->szComment);
		tagInfo.genre = pTagInfo->genre;
	}

	::CloseHandle(hFileTag);
	return TRUE;
}

//å„ÇÎÇ…Ç†ÇÈÉXÉyÅ[ÉXÇêÿÇÈ
void CMP3Input::strcpy_sID3v1(char* pszDest, const char* pszSrc)
{
	::memcpy(pszDest,  pszSrc,  30);
	char* p1 = pszDest;
	char* p2 = pszDest + 29;
	for (; p2 > p1 && *p2 == ' '; p2 --) {}
	*(p2 + 1) = 0;
}

BOOL CMP3Input::setTagInfoEX(const char* pszName, TAG_INFO_11* pTagInfo,int tagSet, int tagAdd)
{
	HANDLE			hFileTag;				//É^ÉOï“èWÉtÉ@ÉCÉã
	ID3Tagv1		tag;
	ID3Tagv1		wtag;
	BOOL			result;
	DWORD			bytesRead;
	DWORD			bytesWrite;
	BYTE			buf[1024];
	LIST_INFO		ListInfo;
	int				fSync;					//ìØä˙ÉwÉbÉ_ÇÃà íu
	DWORD			dSize;					//MPEG AudioÇÃÉfÅ[É^ÉTÉCÉY
	int				siFlag = 0;
	DWORD           target = 0;
	DWORD			fSize;
	DWORD			lSize;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileTag = ::CreateFile(pszName, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileTag == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	::memcpy(wtag.tag, "TAG",  3);
	::memcpy(wtag.albumName,  pTagInfo->szAlbumName,  30);
	::memcpy(wtag.artistName,  pTagInfo->szArtistName,  30);

	if ( pTagInfo->trackNo >= 0 && pTagInfo->trackNo <= 255 ) {
		pTagInfo->szComment[28] = 0;
		pTagInfo->szComment[29] = pTagInfo->trackNo;
	}

    ::memcpy(wtag.comment,  pTagInfo->szComment,  30);
	::memcpy(wtag.trackName,  pTagInfo->szTrackName,  30);
	::memcpy(wtag.year,  pTagInfo->szYear,  4);
	if ( pTagInfo->genre < 0 || pTagInfo->genre > 255 ) {
		pTagInfo->genre = 200;
	}
	wtag.genre = pTagInfo->genre;

	//RIFFå`éÆÉ`ÉFÉbÉN
	SetFilePointer(hFileTag, 0, NULL, FILE_BEGIN);
	result = ReadFile(hFileTag, buf, 4, &bytesRead, NULL);
	if (!result || bytesRead != 4) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(hFileTag);
		return FALSE;
	}
	if (memcmp(buf, "RIFF", 4) == 0) {
		readRIFF(hFileTag, &ListInfo, &fSync, &dSize);
		if ( m_listChunkPoint > 0 ) {
			siFlag = 1;
		} else {
			siFlag = 2;
		}
	}

	//ÉtÉ@ÉCÉãÇÃå„ÇÎÇ©ÇÁ128ÉoÉCÉgÇì«Ç›çûÇﬁ
	::SetFilePointer(hFileTag, -(long)sizeof(tag), NULL, FILE_END);
	result = ::ReadFile(hFileTag, &tag, sizeof(tag), &bytesRead, NULL);
	if (!result || bytesRead != sizeof(tag)) {
		::CloseHandle(hFileTag);
		m_errNo = ERR_MP3_FILE_READ;
		return FALSE;
	}
	if (::memcmp(tag.tag, "TAG", 3) != 0 && tagAdd == 0 ) {
		//ID3Tag v1 Ç™ñ≥Ç≠É^ÉOí«â¡ã÷é~
		::CloseHandle(hFileTag);
		m_errNo = ERR_MP3_FILE_WRITE;
		return FALSE;
	} else if (::memcmp(tag.tag, "TAG", 3) != 0 && tagAdd == 1 ) {
		//ID3Tag v1 Ç™ñ≥Ç≠É^ÉOí«â¡ãñâ¬
		if ( siFlag == 0 ) {
			//í èÌÇÃMP3ÉtÉ@ÉCÉã
			//ÉtÉ@ÉCÉãÇÃç≈å„Ç…èëÇ´çûÇﬁ
			::SetFilePointer(hFileTag, 0, NULL, FILE_END);
			result = ::WriteFile(hFileTag, &wtag, sizeof(wtag), &bytesWrite, NULL);
			if (!result || bytesWrite != sizeof(wtag)) {
				::CloseHandle(hFileTag);
				m_errNo = ERR_MP3_FILE_WRITE;
				return FALSE;
			}
		} else if ( siFlag == 1 ) {
			//SIÉtÉBÅ[ÉãÉhÇ™Ç†ÇÈÇ™IID3ÉtÉBÅ[ÉãÉhÇ™Ç»Ç¢
			//IID3ÉtÉBÅ[ÉãÉhí«â¡
			::memcpy(&buf[target], "IID3", 4);
			buf[target + 4] = 0x80;
			target += 8;
			::memcpy(&buf[target], &wtag, 128);
			target += 128;
			SetFilePointer(hFileTag, 0, NULL, FILE_END);
			result = ::WriteFile(hFileTag, &buf, target, &bytesWrite, NULL);
			if (!result || bytesWrite != target) {
				::CloseHandle(hFileTag);
				m_errNo = ERR_MP3_FILE_WRITE;
				return FALSE;
			}
			::SetEndOfFile(hFileTag);
			fSize = ::GetFileSize(m_hFile, NULL) - 8;
			lSize = m_listChunkSize + target;

			//LISTÉ`ÉÉÉìÉNÉTÉCÉYïœçX
			buf[0] = (BYTE)(fSize & 0x000000ff);
			buf[1] = (BYTE)((fSize & 0x0000ffff) >> 8);
			buf[2] = (BYTE)((fSize & 0x00ffffff) >> 16);
			buf[3] = (BYTE)((fSize & 0xffffffff) >> 24);
			SetFilePointer(hFileTag, m_listChunkPoint + 4, NULL, FILE_BEGIN);
			result = ::WriteFile(hFileTag, &buf, 4, &bytesWrite, NULL);
			if (!result || bytesWrite != 4) {
				::CloseHandle(hFileTag);
				m_errNo = ERR_MP3_FILE_WRITE;
				return FALSE;
			}

			//DATAÉ`ÉÉÉìÉNÉTÉCÉYïœçX
			buf[0] = (BYTE)(lSize & 0x000000ff);
			buf[1] = (BYTE)((lSize & 0x0000ffff) >> 8);
			buf[2] = (BYTE)((lSize & 0x00ffffff) >> 16);
			buf[3] = (BYTE)((lSize & 0xffffffff) >> 24);
			SetFilePointer(hFileTag,  4, NULL, FILE_BEGIN);
			result = ::WriteFile(hFileTag, &buf, 4, &bytesWrite, NULL);
			if (!result || bytesWrite != 4) {
				::CloseHandle(hFileTag);
				m_errNo = ERR_MP3_FILE_WRITE;
				return FALSE;
			}

		} else if ( siFlag == 2 ) {
			//SIÉtÉBÅ[ÉãÉhÇ™Ç»Ç¢
			//LISTèÓïÒèâä˙âª
			::memset(&ListInfo, ' ', sizeof(LIST_INFO));

			//LISTÉ`ÉÉÉìÉNí«â¡
			::CloseHandle(hFileTag);
			if (!setListInfo(pszName, &ListInfo)) {
				return FALSE;
			}

			//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
			hFileTag = ::CreateFile(pszName, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFileTag == INVALID_HANDLE_VALUE) {
				m_errNo = ERR_MP3_FILE_OPEN;
				return FALSE;
			}

			//ÉtÉ@ÉCÉãÇÃå„ÇÎÇ©ÇÁ128ÉoÉCÉgÇ…èëÇ´çûÇﬁ
			::SetFilePointer(hFileTag, -(long)sizeof(wtag), NULL, FILE_END);
			result = ::WriteFile(hFileTag, &wtag, sizeof(wtag), &bytesWrite, NULL);
			if (!result || bytesWrite != sizeof(wtag)) {
				::CloseHandle(hFileTag);
				m_errNo = ERR_MP3_FILE_WRITE;
				return FALSE;
			}
		}
	} else {
		//ID3Tag v1 óLÇË
		//ÉtÉ@ÉCÉãÇÃå„ÇÎÇ©ÇÁ128ÉoÉCÉgÇ…èëÇ´çûÇﬁ
		::SetFilePointer(hFileTag, -(long)sizeof(wtag), NULL, FILE_END);
		result = ::WriteFile(hFileTag, &wtag, sizeof(wtag), &bytesWrite, NULL);
		if (!result || bytesWrite != sizeof(wtag)) {
			::CloseHandle(hFileTag);
			m_errNo = ERR_MP3_FILE_WRITE;
			return FALSE;
		}
	}

	if ( tagSet == 1 ) {
		//É^ÉOèÓïÒÉZÉbÉg
		strcpy_sID3v1((char*)tagInfo.szTrackName, pTagInfo->szTrackName);
		strcpy_sID3v1((char*)tagInfo.szArtistName, pTagInfo->szArtistName);
		strcpy_sID3v1((char*)tagInfo.szAlbumName, pTagInfo->szAlbumName);
		::memcpy(tagInfo.szYear, pTagInfo->szYear, 5);
		strcpy_sID3v1((char*)tagInfo.szComment, pTagInfo->szComment);
		tagInfo.genre = pTagInfo->genre;
	}

	::CloseHandle(hFileTag);
	return TRUE;
}


long CMP3Input::input_debug()
{
	return debugNo;
}

void CMP3Input::input_setDebug(long no)
{
	debugNo = no;
}


BOOL CMP3Input::input_GetGenre(TAG_INFO* pTagInfo)
{
	int gno;

	gno = pTagInfo->genre;

	::memset(pTagInfo, 0, sizeof(TAG_INFO));

	pTagInfo->genre = gno;
	if ( gno < 0 || gno > 255 ) {
		m_errNo = ERR_INVALID_VALUE;
		return FALSE;
	}

	::memcpy(pTagInfo->szGanreName,  genre_tbl[gno],  strlen(genre_tbl[gno]));

	return TRUE;
}

BOOL CMP3Input::input_SetxmMP3Option(xmMP3_OPTION* pxmMP3Option)
{

	if (m_state != STATE_STOP) {
		//í‚é~íÜÇÃÇ›èàóùÇ∑ÇÈ
		m_errNo = ERR_NOT_STATE_STOP;
		return FALSE;
	}

	//ÉIÉvÉVÉáÉìílÉ`ÉFÉbÉN
	if (pxmMP3Option->inputBlock <= 0) {
		m_errNo = ERR_INVALID_VALUE;
		return FALSE;
	}

	if (pxmMP3Option->outputBlock <= 0) {
		m_errNo = ERR_INVALID_VALUE;
		return FALSE;
	}

	if (pxmMP3Option->inputSleep < 0) {
		m_errNo = ERR_INVALID_VALUE;
		return FALSE;
	}

	if (pxmMP3Option->outputSleep < 0) {
		m_errNo = ERR_INVALID_VALUE;
		return FALSE;
	}

	m_inputBlock	= pxmMP3Option->inputBlock;
	m_outputBlock_in	= pxmMP3Option->outputBlock;
	m_outputBlock_in_tmp = m_outputBlock_in;
	m_inputSleep	= pxmMP3Option->inputSleep;
	//m_outputSleep	= pxmMP3Option->outputSleep;

	output_SetxmMP3Option(pxmMP3Option);

	return TRUE;

}

void CMP3Input::input_GetxmMP3Option(xmMP3_OPTION* pxmMP3Option){
	pxmMP3Option->inputBlock = m_inputBlock;
	pxmMP3Option->outputBlock = m_outputBlock_in;
	pxmMP3Option->inputSleep = m_inputSleep;
	//pxmMP3Option->outputSleep = m_outputSleep;
	output_GetxmMP3Option(pxmMP3Option);
	return;
}

BOOL CMP3Input::input_SetDecodeOption(DEC_OPTION* pDecOption)
{
	double curRed;
	double Red;

	if (m_state != STATE_STOP) {
		//í‚é~íÜÇÃÇ›èàóùÇ∑ÇÈ
		m_errNo = ERR_NOT_STATE_STOP;
		return FALSE;
	}

	//ÉIÉvÉVÉáÉìílÉ`ÉFÉbÉN
	if (pDecOption->reduction != 0 &&
	    pDecOption->reduction != 1 &&
		pDecOption->reduction != 2) {
		m_errNo = ERR_INVALID_VALUE;
		return FALSE;
	}

	if (pDecOption->convert != 0 &&
	    pDecOption->convert != 1) {
		m_errNo = ERR_INVALID_VALUE;
		return FALSE;
	}

	if (pDecOption->freqLimit <= 0) {
		m_errNo = ERR_INVALID_VALUE;
		return FALSE;
	}

	decodeOption.reduction = pDecOption->reduction;
	decodeOption.convert = pDecOption->convert;
	decodeOption.freqLimit = pDecOption->freqLimit;

	if ( pDecOption->reduction == 0 ) {
		curRed = 4;
	} else if ( pDecOption->reduction == 1 ) {
		curRed = 2;
	} else {
		curRed = 1;
	}

	if ( m_reduction == 0 ) {
		Red = 4;
	} else if ( m_reduction == 1 ) {
		Red = 2;
	} else {
		Red = 1;
	}
	m_playSpeed = Red / curRed;

	return ::mp3SetDecodeOption(&decodeOption);
}

void CMP3Input::input_GetDecodeOption(DEC_OPTION* pDecOption)
{
	mp3GetDecodeOption(&decodeOption);

	pDecOption->reduction = decodeOption.reduction;
	pDecOption->convert = decodeOption.convert;
	pDecOption->freqLimit = decodeOption.freqLimit;

	return;
}

/******************** RIFF.cpp ************************/
BOOL CMP3Input::readRIFF(HANDLE FileHandle, LIST_INFO* lplistInfo, int* fSync, DWORD* dSize)
{
	RiffChunk	riff;
	CommonChunk	common;
	BOOL		result;
	DWORD		bytesRead;
	DWORD       wkDataSize;
	DWORD       wkDataPoint;

	RiffStatus = 0;
	m_listChunkPoint = 0;
	m_listChunkSize = 0;
	m_listIID3Point = 0;
	m_listILYCPoint = 0;

	//ÉtÉ@ÉCÉãÇÃì™Ç…ñﬂÇËÅARIFFÉ`ÉÉÉìÉNÇì«Ç›çûÇﬁ
	::SetFilePointer(FileHandle, 0, NULL, FILE_BEGIN);
	result = ::ReadFile(FileHandle, &riff, sizeof(riff), &bytesRead, NULL);
	if (!result || bytesRead != sizeof(riff)) {
		return FALSE;
	}
	if (::memcmp(riff.chunkID, "RIFF", 4) != 0) {
		return FALSE;
	}

	//FileType
	if (memcmp(riff.formType, "WAVE", 4) == 0) {
		m_FileType += 1;
	}
	if (memcmp(riff.formType, "RMP3", 4) == 0) {
		m_FileType += 2;
	}

	*fSync = sizeof(riff);

	//dataÉ`ÉÉÉìÉNÇíTÇ∑
	for (;;) {
		result = ::ReadFile(FileHandle, &common, sizeof(common), &bytesRead, NULL);
		if (!result || bytesRead != sizeof(common)) {
			return FALSE;
		}

		if (::memcmp(common.chunkID, "fmt ", 4) == 0) {
			//Mpegà»äOÇÃÉtÉHÅ[É}ÉbÉgÉ`ÉFÉbÉN
			//::ReadFile(FileHandle, &m_waveFormat, common.chunkSize, &bytesRead, NULL);
			::ReadFile(FileHandle, &m_waveFormat, sizeof(WAVEFORMATEX), &bytesRead, NULL);
			if (!result || bytesRead != sizeof(WAVEFORMATEX)) {
				RiffStatus = -1;
				return FALSE;
			}
			//ÉtÉHÅ[É}ÉbÉgTagÉ`ÉFÉbÉN
			if (m_waveFormat.wFormatTag == 85 || m_waveFormat.wFormatTag == 80) {
				//MPEG Audio ÉfÅ[É^
				//common.chunkSize = 0;  //É`ÉÉÉìÉNÇì«Ç›îÚÇŒÇ≥Ç»Ç¢
			} else if (m_waveFormat.wFormatTag == 1) {
				//ÉäÉAÉãÇoÇbÇlÉfÅ[É^
				RiffStatus = 1;
				//common.chunkSize = 0;  //É`ÉÉÉìÉNÇì«Ç›îÚÇŒÇ≥Ç»Ç¢
			} else {
				//ÇªÇÃëºÇÃå`éÆ
				RiffStatus = -1;
				return FALSE; 
			}
			::SetFilePointer(FileHandle, -(long)sizeof(WAVEFORMATEX), NULL, FILE_CURRENT);
/*
			if (common.chunkSize != 0x1E && common.chunkSize != 0x20) {
				if (common.chunkSize == 0x10 || common.chunkSize == 0x12) {
					::ReadFile(FileHandle, &m_waveFormat, common.chunkSize, &bytesRead, NULL);
					if (!result || bytesRead != common.chunkSize) {
						RiffStatus = -1;
						return FALSE;
					}
					RiffStatus = 1;
					common.chunkSize = 0;  //É`ÉÉÉìÉNÇì«Ç›îÚÇŒÇ≥Ç»Ç¢
					//return TRUE;
				} else {
					RiffStatus = -1;
					return FALSE; 
				}
			}
*/
		}
		
		if (::memcmp(common.chunkID, "data", 4) == 0) {
			//dataÉ`ÉÉÉìÉNî≠å©
			break;
		}
		else {
			//éüÇÃÉ`ÉÉÉìÉNÇ‹Ç≈ì«Ç›îÚÇŒÇ∑
			*fSync = ::SetFilePointer(FileHandle, common.chunkSize, NULL, FILE_CURRENT);
		}
	}

	//É`ÉÉÉìÉNÉTÉCÉYÉ`ÉFÉbÉN
	wkDataSize = *dSize;
	if (common.chunkSize < wkDataSize) {
		*dSize = common.chunkSize;
	}

	*fSync += bytesRead;

	wkDataPoint = *fSync + common.chunkSize;

	//RIFF MP3 LISTÉ`ÉÉÉìÉNÇíTÇ∑
	if ( (common.chunkSize % 2) == 1 ) common.chunkSize++;
	if (::SetFilePointer(FileHandle, common.chunkSize, NULL, FILE_CURRENT) == 0xFFFFFFFF && GetLastError() != NO_ERROR ){
		return TRUE;
	};
	wkDataPoint += common.chunkSize;
	for (;;) {
		result = ::ReadFile(FileHandle, &common, sizeof(common), &bytesRead, NULL);
		wkDataPoint += sizeof(common);
		if (!result || bytesRead != sizeof(common)) {
			return TRUE;
		}
		if (memcmp(common.chunkID, "LIST", 4) == 0) {
			m_listChunkPoint = wkDataPoint;
			m_listChunkSize = common.chunkSize;
			//LISTÉ`ÉÉÉìÉNî≠å©
			break;
		}
		else {
			//éüÇÃÉ`ÉÉÉìÉNÇ‹Ç≈ì«Ç›îÚÇŒÇ∑
			::SetFilePointer(FileHandle, common.chunkSize, NULL, FILE_CURRENT);
			wkDataPoint += common.chunkSize;
		}
	}
	BYTE* pRMPList = (BYTE*)::malloc(common.chunkSize);
	if (pRMPList == NULL) {
		return TRUE;
	}
	BYTE listID[4];
	result = ::ReadFile(FileHandle, listID, 4, &bytesRead, NULL);
	wkDataPoint += 4;
	if (!result || bytesRead != 4) {
		::free(pRMPList);
		return TRUE;
	}
	if (::memcmp(listID, "INFO", 4) != 0) {
		//INFOà»äOÇÕñ≥éã
		::free(pRMPList);
		return TRUE;
	}
	RMPInfo* pRMPInfo = (RMPInfo*)::malloc(sizeof(RMPInfo));
	if (pRMPInfo == NULL) {
		::free(pRMPList);
		return TRUE;
	}
	DWORD remains = common.chunkSize - 4;
	DWORD baseSize = sizeof(RMPInfo) - sizeof(pRMPInfo->data);
	int dumy;
	while (remains >= sizeof(RMPInfo)) {
		result = ::ReadFile(FileHandle, pRMPInfo, baseSize, &bytesRead, NULL);
		wkDataPoint += baseSize;
		if (!result || bytesRead != baseSize) {
			break;
		}
		dumy = 0;
		if ((pRMPInfo->length % 2) == 1 ) dumy = 1;

		pRMPInfo = (RMPInfo*)::realloc(pRMPInfo, baseSize + pRMPInfo->length + dumy);
		if (pRMPInfo == NULL) {
			break;
		}
		result = ::ReadFile(FileHandle, pRMPInfo->data, pRMPInfo->length + dumy, &bytesRead, NULL);
		wkDataPoint += pRMPInfo->length + dumy;
		if (!result || bytesRead != pRMPInfo->length + dumy) {
			break;
		}
		loadRMPInfo(lplistInfo, pRMPInfo);
		//IID3,ILYC à íuï€ë∂
		if ( ::memcmp(pRMPInfo->infoID, "IID3", 4) == 0 ) {
			m_listIID3Point = wkDataPoint;
		} else if ( ::memcmp(pRMPInfo->infoID, "ILYC", 4) == 0 ) {
			m_listILYCPoint = wkDataPoint;
			m_listILYCSize = pRMPInfo->length;
		}
	
		remains -= baseSize + pRMPInfo->length + dumy;
	}
	::free(pRMPList);
	::free(pRMPInfo);
	return TRUE;
}

int CMP3Input::getTime()
{
	if (m_decInfo.frequency == 0 || m_decInfo.frequency == 0) return 0;

	if ( decFlag == 1 ){
		return input_sampleToSec(m_firstSample + input_bytesToSample(decodePos));
	}

	double playMs;
	int playSamples;
	playSamples = output_getCurrentSample();
	//î{ë¨ââëtëŒâû
	playMs = (((float)playSamples + (float)m_firstSample) * (float)1000 / (float)m_decInfo.frequency) / (float)1000;
	playMs = playMs * m_playSpeed;
	m_playSec = (int)playMs;
	if (m_timeFlag && m_playSec > m_totalSec) m_playSec = m_totalSec;
	return m_playSec;
}

BOOL CMP3Input::getTagInfo(TAG_INFO* pTagInfo)
{
	//ÉtÉ@ÉCÉãñ¢ÉIÅ[ÉvÉì
	if (m_hFile == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_NOT_OPEN;
		return FALSE;
	}

	::memset(pTagInfo, 0, sizeof(TAG_INFO));

	//É^ÉOèÓïÒÉZÉbÉg
	strcpy_sID3v1(pTagInfo->szTrackName,  (char*)tagInfo.szTrackName);
	strcpy_sID3v1(pTagInfo->szArtistName, (char*)tagInfo.szArtistName);
	strcpy_sID3v1(pTagInfo->szAlbumName, (char*)tagInfo.szAlbumName);
	::memcpy(pTagInfo->szYear,  tagInfo.szYear,  5);
	strcpy_sID3v1(pTagInfo->szComment, (char*)tagInfo.szComment);
	pTagInfo->genre = tagInfo.genre;
	::memcpy(pTagInfo->szGanreName,  genre_tbl[tagInfo.genre],  strlen(genre_tbl[tagInfo.genre]));
	
	return TRUE;
}

BOOL CMP3Input::getMpegInfo(MPEG_INFO* pMpegInfo)
{
	int				padding;

	//ÉtÉ@ÉCÉãñ¢ÉIÅ[ÉvÉì
	if (m_hFile == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_NOT_OPEN;
		return FALSE;
	}

	pMpegInfo->version = m_decInfo.header.version;
	pMpegInfo->layer = m_decInfo.header.layer;
	pMpegInfo->crcDisable = (m_decInfo.header.error_prot) ? 0 : 1;
	pMpegInfo->mode = m_decInfo.header.mode;
	pMpegInfo->extension = m_decInfo.header.mode_ext;
	pMpegInfo->copyright = m_decInfo.header.copyright;
	pMpegInfo->original = m_decInfo.header.original;
	pMpegInfo->emphasis = m_decInfo.header.emphasis;
	pMpegInfo->channels = m_decInfo.channels;
	pMpegInfo->bitRate = m_decInfo.bitRate / 1000;
	pMpegInfo->fileSize = fileSize;
	pMpegInfo->samplingRate = m_decInfo.frequency;
	padding = m_decInfo.header.padding;

	if (m_decInfo.frames == 0) {
		m_decInfo.frames = dataSize / m_decInfo.minInputSize;
	}
	pMpegInfo->flames = m_decInfo.frames;
	int samples = input_bytesToSample(m_decInfo.frames * m_decInfo.outputSize);
	pMpegInfo->totalSec = input_sampleToSec(samples);

	return TRUE;
}


BOOL CMP3Input::initDec()
{

	::mp3DecodeInit();

	OSVERSIONINFO	obsInfo;
	obsInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&obsInfo);
	if ( obsInfo.dwPlatformId == VER_PLATFORM_WIN32_NT ) {
		m_platForm = 1;
	} else {
		m_platForm = 0;
	}
	
	m_pProc			= _NullProc;
	m_hFile			= INVALID_HANDLE_VALUE;

	m_hThread		= NULL;
	m_threadID		= 0;
	m_state			= STATE_STOP;
	m_inputBuf		= NULL;
	m_outputBuf_in	= NULL;

	callBackFlag	= FALSE;

	debugNo = 0;

	initOutput();
	//initEnc();
	mp3SetEqualizer(NULL);
	return TRUE;
}

BOOL CMP3Input::freeDec()
{
	::Sleep(50);
	callBackFlag = FALSE;
	m_pProc = _NullProc;

	if ( m_playFlag ) input_stop();
	input_close();

	if ( m_playFlag ) output_stop();
	if ( m_playFlag ) output_close();
	freeOutput();
	//freeEnc();

	::free(m_inputBuf);
	::free(m_outputBuf_in);
	::Sleep(50);
	return TRUE;
}

BOOL CMP3Input::input_open(const char* pszName, InputInfo* pInfo)
{
	//Sleep(100);
	
	if (pszName[0] == 0) {
	
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	
	input_close();
	m_waveFlag= FALSE;

	
	m_hFile = ::CreateFile(pszName, GENERIC_READ, FILE_SHARE_READ + FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	m_openFlag = TRUE;

	m_firstSync = 0;
	dataSize = ::GetFileSize(m_hFile, NULL);
	fileSize = dataSize;

	::memset(pInfo, 0, sizeof(InputInfo));

	::memset(tagInfo.szAlbumName, 0, sizeof(tagInfo.szAlbumName));
	::memset(tagInfo.szArtistName, 0, sizeof(tagInfo.szArtistName));
	::memset(tagInfo.szTrackName, 0, sizeof(tagInfo.szTrackName));
	::memset(tagInfo.szYear, 0, sizeof(tagInfo.szYear));
	tagInfo.genre = 0;
	::memset(tagInfo.szGanreName, 0, sizeof(tagInfo.szGanreName));
	::memset(tagInfo.szComment, 0, sizeof(tagInfo.szComment));

	::memset(listInfo.IART, 0, sizeof(listInfo.IART));
	::memset(listInfo.ICMS, 0, sizeof(listInfo.ICMS));
	::memset(listInfo.ICMT, 0, sizeof(listInfo.ICMT));
	::memset(listInfo.ICOP, 0, sizeof(listInfo.ICOP));
	::memset(listInfo.ICRD, 0, sizeof(listInfo.ICRD));
	::memset(listInfo.IENG, 0, sizeof(listInfo.IENG));
	::memset(listInfo.IGNR, 0, sizeof(listInfo.IGNR));
	::memset(listInfo.IKEY, 0, sizeof(listInfo.IKEY));
	::memset(listInfo.ILYC, 0, sizeof(listInfo.ILYC));
	::memset(listInfo.INAM, 0, sizeof(listInfo.INAM));
	::memset(listInfo.IPRD, 0, sizeof(listInfo.IPRD));
	::memset(listInfo.ISFT, 0, sizeof(listInfo.ISFT));
	::memset(listInfo.ISRC, 0, sizeof(listInfo.ISRC));
	::memset(listInfo.ITCH, 0, sizeof(listInfo.ITCH));

	if (readID3v1(m_hFile, &tagInfo)) {
		dataSize -= 128;
		strcpy_sID3v1(pInfo->szTrackName,  (char*)tagInfo.szTrackName);
		strcpy_sID3v1(pInfo->szArtistName, (char*)tagInfo.szArtistName);
	}

	//LYRICS3
	DWORD lysize;
	if (chkLYRICS3(m_hFile, &lysize)) {
		dataSize -= lysize;
		if ( m_Lyrics3UseFlag ) {
			input_readLyrics3(m_hFile);
		}
	}

	readRIFF(m_hFile, &listInfo, &m_firstSync, &dataSize);
	if (RiffStatus == -1) {
		//MPEG,WAVE
		input_close();
		m_errNo = ERR_UNKNOWN_FILE;
		return FALSE;
	} else if (RiffStatus == 1) {
		//WAVE
		m_decInfo.bitsPerSample = m_waveFormat.wBitsPerSample;
		m_decInfo.channels = m_waveFormat.nChannels;
		m_decInfo.dataSize = dataSize;
		m_decInfo.frequency = m_waveFormat.nSamplesPerSec;
		m_decInfo.bitRate = m_waveFormat.wBitsPerSample;		
		m_bitRate = m_decInfo.bitRate;							
		m_playSpeed = 1;
		m_firstSample = 0;
		m_decInfo.outputSize = DEFAULT_BUFFER_SIZE;
		if ( m_decInfo.bitRate == 8 ) m_decInfo.outputSize = m_decInfo.outputSize / 4;
		m_decInfo.frames = (int)(m_decInfo.dataSize / m_decInfo.outputSize);

		StringCchCopy(pInfo->szTrackName,20,  "WAVE_FORMAT_PCM");
		StringCchCopy(pInfo->szArtistName,20, "WAVE_FORMAT_PCM");
		pInfo->bitRate = m_waveFormat.wBitsPerSample;
		pInfo->channels = m_waveFormat.nChannels;
		pInfo->samplingRate = m_waveFormat.nSamplesPerSec;
		pInfo->totalSec = input_sampleToSec(input_bytesToSample(dataSize));
		m_totalSec = pInfo->totalSec;
		m_winampTotalSec = pInfo->totalSec;
		m_waveFlag= TRUE;
		return TRUE;
	}

	if (readID3v2(m_hFile, &m_firstSync)) {
		dataSize -= m_firstSync;
	}


	BOOL	result;
	DWORD	bytesRead;
	BYTE	buf[10240];
	//BYTE	buf[2048];
	int		sync;

	//Tag
	
	if (m_firstSync == 0) {
		::SetFilePointer(m_hFile, m_firstSync, NULL, FILE_BEGIN);
		result = ::ReadFile(m_hFile, buf, 4, &bytesRead, NULL);
		if (!result || bytesRead != 4) {
			input_close();
			m_errNo = ERR_MP3_FILE_READ;
			return FALSE;
		}
		if (buf[0] == 0x00){
			m_firstSync = 128;
		}
	}

//X-MaD 02/08/01
	int InputBytes = sizeof(buf);
	long FileSize = GetFileSize(m_hFile, NULL);
	long NextBufRead = m_firstSync;
	int nvolte = 0;
	int Bitrate = 0;
	int Freq = 0;
	int firstSync = 0;
	bool primavolta = true;


	for (;;) {

		::SetFilePointer(m_hFile, NextBufRead, NULL, FILE_BEGIN);
		result = ::ReadFile(m_hFile, buf, InputBytes, &bytesRead, NULL);
		if (!result || bytesRead == 0) {
			m_errNo = ERR_MP3_FILE_READ;
			input_close();
			return FALSE;
		}
		
		
		if (!::mp3FindSync(buf, InputBytes, &sync)) {
			input_close();
			m_errNo = ERR_FRAME_HEADER_NOT_FOUND;
			return FALSE;
		}

		firstSync = NextBufRead + sync;

		::SetFilePointer(m_hFile, firstSync, NULL, FILE_BEGIN);
		result = ::ReadFile(m_hFile, buf, InputBytes, &bytesRead, NULL);
		if (!result || bytesRead == 0) {
			input_close();
			m_errNo = ERR_MP3_FILE_READ;
			return FALSE;
		}



		if (!::mp3GetDecodeInfo(buf, bytesRead, &m_decInfo, 1)) {
			//nvolte = 0;
			//primavolta = true;
			NextBufRead = firstSync + 1;
		} 
		else {
			if (primavolta) {	
					NextBufRead = firstSync;
					m_firstSync = firstSync;
					primavolta = false;
			}
			if ((Bitrate == m_decInfo.bitRate && Freq == m_decInfo.frequency) || nvolte == 0) {
				if (firstSync == 0) {
					goto Exit;
				} else {
					nvolte = nvolte + 1;
				}
			} else {
				NextBufRead = firstSync;
				if (nvolte == 1) {
					primavolta = true;
				}
				nvolte = 0;
			}
			Bitrate = m_decInfo.bitRate;
			Freq = m_decInfo.frequency;
			if (nvolte >= m_NEqualFrame) {
				goto Exit;
			} else {
				NextBufRead = NextBufRead + 1;
			}
		}

	}

Exit:
//X-MaD 02/08/01

	m_firstSample = 0;

	pInfo->channels = m_decInfo.channels;
	pInfo->bitRate = m_decInfo.bitRate / 1000;
	m_bitRate = pInfo->bitRate;
	pInfo->samplingRate = m_decInfo.frequency;

	if (m_decInfo.frames == 0) {
		m_decInfo.frames = dataSize / m_decInfo.minInputSize;
	}
	int samples = input_bytesToSample(m_decInfo.frames * m_decInfo.outputSize);
	pInfo->totalSec = input_sampleToSec(samples);
	m_totalSec = pInfo->totalSec;
	
	//winamp
	double msPerFrame = ms_table[m_decInfo.header.layer-1][m_decInfo.header.fr_index];

	m_winampTotalSec = (m_decInfo.frames * (int)(msPerFrame) / 1000);

	mp3GetDecodeOption(&decodeOption);
	m_reduction = decodeOption.reduction;
	m_playSpeed = 1;

	return TRUE;
}

//X-MaD 26/03/02

BOOL CMP3Input::SetReadHaedA(int NEqualFrame)
{
	BOOL Flag = FALSE;
	if (NEqualFrame < 1024 && NEqualFrame > 0)
	{			
		m_NEqualFrame = NEqualFrame;
		Flag = TRUE;
	}
	return Flag;
}
//X-MaD 26/03/02

BOOL CMP3Input::input_close()
{
	::Sleep(100);
	if (m_hFile == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_NOT_OPEN;
		return FALSE;
	}

	if (m_state != STATE_STOP) {
		input_stop();
	}

	::CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;

	return TRUE;
}

int CMP3Input::input_read(void* pData, int length)
{
	int bytesRead;
	if (!::ReadFile(m_hFile, pData, length, (DWORD*)&bytesRead, NULL)) {
		return 0;//exception
	}
	return bytesRead;
}

BOOL CMP3Input::input_setCallback(INPUT_MSG_PROC pProc)
{
	if (m_state != STATE_STOP) {
		//í‚é~íÜà»äOÇÕê›íËïsâ¬
		m_errNo = ERR_NOT_STATE_STOP;
		return FALSE;
	}
	m_pProc = pProc;
	callBackFlag = TRUE;
	return TRUE;
}

BOOL CMP3Input::input_startCallback()
{
	callBackFlag = TRUE;
	return TRUE;
}

BOOL CMP3Input::input_stopCallback()
{
	callBackFlag = FALSE;
	return TRUE;
}

int CMP3Input::input_getState()
{
	return m_state;
}

BOOL CMP3Input::input_play()
{
	//Sleep(100);
	if (m_state != STATE_STOP || m_hThread != NULL ) {
		//í‚é~íÜà»äOÇÕé¿çsïsâ¬
		m_errNo = ERR_NOT_STATE_STOP;
		return FALSE;
	}

	decFlag = 0;
	m_fadeOutFlag = false;
	m_playFlag = TRUE;
	m_readWaveDataSize = 0;

	//ÉfÉRÅ[ÉhèÄîı
	//decodePos		= 0;		//éûä‘éÊìæóp

	::SetFilePointer(m_hFile, m_firstSync, NULL, FILE_BEGIN);
	if ( m_waveFlag == FALSE ){
		m_inputSize		= m_decInfo.maxInputSize * m_inputBlock;
		m_inputBuf		= (BYTE*)::realloc(m_inputBuf, m_inputSize);
		m_inputPtr		= m_inputBuf;
		m_inputBytes	= input_read(m_inputPtr, m_inputSize);
		if (m_inputBytes == 0) {
			m_errNo = ERR_INVALID_VALUE;
			return FALSE;
		}
	}

	//èoóÕÉfÉoÉCÉXÇäJÇ≠
	OutputInfo	outputInfo;
	outputInfo.channels			= m_decInfo.channels;
	outputInfo.bitsPerSample	= m_decInfo.bitsPerSample;
	outputInfo.frequency		= m_decInfo.frequency;

	if ( m_waveFlag == FALSE ){
		outputInfo.bufferSize		= m_decInfo.outputSize * m_outputBlock_in;
	} else {
		m_outWaveBufSize = m_decInfo.outputSize * m_outputBlock_in_wav;
		m_inWaveBufSize = m_decInfo.outputSize;
		outputInfo.bufferSize		= m_outWaveBufSize;
	}

	if (!output_open(NULL, outputInfo)) {
		m_errNo = ERR_OPEN_OUT_DEVICE;
		return FALSE;
	}

	//m_outputBuf_in= (BYTE*)::realloc(m_outputBuf_in, m_decInfo.outputSize * m_outputBlock_in);
	m_outputBuf_in= (BYTE*)::realloc(m_outputBuf_in, outputInfo.bufferSize);

	if ( m_waveFlag == FALSE ){
		if (!::mp3DecodeStart(m_inputPtr, m_inputBytes)) {
			output_close();
			m_errNo = ERR_DECODE;
			return FALSE;
		}
		m_inputPtr += m_decInfo.skipSize;
		m_inputBytes -= m_decInfo.skipSize;
	}

	//ÉfÉRÅ[ÉhópÉXÉåÉbÉhäJén
	//m_hThread = (HANDLE)_beginthreadex(NULL, 0, (THREADFUNC)input_ThreadProc, NULL, 0, (unsigned int*)&m_threadID);
	m_hThread = CreateThread(NULL, 0, input_ThreadProc, NULL, 0, &m_threadID);
	if (m_hThread == NULL) {
		output_close();
		m_errNo = ERR_DECODE_THREAD;
		return FALSE;
	}

	//èÛë‘É`ÉFÉbÉN
	int Cnt;
	for ( Cnt = 0; Cnt < 100; Cnt++){
		::Sleep(10);
		if ( m_state == STATE_PLAY ) {
			break;
		} else if ( Cnt == 99 ){
			m_errNo = ERR_PLAY;
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CMP3Input::input_decodeWave(const char* pszWaveName)
{
	BYTE			buf[1024];
	BOOL			result;
	DWORD			bytesWrite;

	if (m_state != STATE_STOP) {
		//í‚é~íÜà»äOÇÕé¿çsïsâ¬
		m_errNo = ERR_NOT_STATE_STOP;
		return FALSE;
	}

	decFlag = 1;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	m_hWaveFile = ::CreateFile(pszWaveName, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hWaveFile == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_WAV_FILE_OPEN;
		return FALSE;
	}

	SetFilePointer(m_hWaveFile, 0, NULL, FILE_BEGIN);
	StringCchCopy((char*)buf,1024,"RIFF");
	result = ::WriteFile(m_hWaveFile, &buf, 4, &bytesWrite, NULL);
	::memset(&buf, 0, 4);
	result = ::WriteFile(m_hWaveFile, &buf, 4, &bytesWrite, NULL);
	StringCchCopy((char*)buf,1024,"WAVEfmt ");
	result = ::WriteFile(m_hWaveFile, &buf, 8, &bytesWrite, NULL);

	//É`ÉÉÉìÉNÉTÉCÉY
	buf[0] = 0x10;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	result = ::WriteFile(m_hWaveFile, &buf, 4, &bytesWrite, NULL);

	//ÉtÉHÅ[É}ÉbÉgÇhÇc
	buf[0] = 0x01;
	buf[1] = 0x00;
	result = ::WriteFile(m_hWaveFile, &buf, 2, &bytesWrite, NULL);

	//É`ÉÉÉìÉlÉãêî
	buf[0] = m_decInfo.channels;
	buf[1] = 0x00;
	result = ::WriteFile(m_hWaveFile, &buf, 2, &bytesWrite, NULL);

	//ÉTÉìÉvÉäÉìÉOÉåÅ[Ég
	DWORD nSamplesPerSec = m_decInfo.frequency;
	result = ::WriteFile(m_hWaveFile, &nSamplesPerSec, 4, &bytesWrite, NULL);

	//ì]ëóÉåÅ[Ég
	WORD  nBlockAlign = (m_decInfo.channels * m_decInfo.bitsPerSample / 8);
	DWORD nAvgBytesPerSec = (nBlockAlign * m_decInfo.frequency);
	result = ::WriteFile(m_hWaveFile, &nAvgBytesPerSec, 4, &bytesWrite, NULL);

	//ÉuÉçÉbÉNÉTÉCÉYêî
	result = ::WriteFile(m_hWaveFile, &nBlockAlign, 2, &bytesWrite, NULL);

	//ó éqâªÉrÉbÉgêî
	buf[0] = m_decInfo.bitsPerSample;
	buf[1] = 0x00;
	result = ::WriteFile(m_hWaveFile, &buf, 2, &bytesWrite, NULL);

	StringCchCopy((char*)buf,1024,"data");
	result = ::WriteFile(m_hWaveFile, &buf, 4, &bytesWrite, NULL);
	::memset(&buf, 0, 4);
	result = ::WriteFile(m_hWaveFile, &buf, 4, &bytesWrite, NULL);

	//ÉfÉRÅ[ÉhèÄîı
	//decodePos		= 0;		//éûä‘éÊìæóp
	::SetFilePointer(m_hFile, m_firstSync, NULL, FILE_BEGIN);
	m_inputSize		= m_decInfo.maxInputSize * m_inputBlock;
	m_inputBuf		= (BYTE*)::realloc(m_inputBuf, m_inputSize);
	m_inputPtr		= m_inputBuf;
	m_inputBytes	= input_read(m_inputPtr, m_inputSize);

	if (m_inputBytes == 0) {
		m_errNo = ERR_INVALID_VALUE;
		return FALSE;
	}

	//èoóÕÉfÉoÉCÉXÇäJÇ≠
/*
	OutputInfo	outputInfo;
	outputInfo.channels			= m_decInfo.channels;
	outputInfo.bitsPerSample	= m_decInfo.bitsPerSample;
	outputInfo.frequency		= m_decInfo.frequency;
	outputInfo.bufferSize		= m_decInfo.maxOutputSize * m_outputBlock;

	if (!output_open(NULL, outputInfo)) {
		return FALSE;
	}
*/
	m_outputBuf_in= (BYTE*)::realloc(m_outputBuf_in, m_decInfo.outputSize);

	if (!::mp3DecodeStart(m_inputPtr, m_inputBytes)) {
		output_close();
		m_errNo = ERR_DECODE;
		return FALSE;
	}

	m_inputPtr += m_decInfo.skipSize;
	m_inputBytes -= m_decInfo.skipSize;

	//ÉfÉRÅ[ÉhópÉXÉåÉbÉhäJén
	//m_hThread = (HANDLE)_beginthreadex(NULL, 0, (THREADFUNC)input_ThreadProc, NULL, 0, (unsigned int*)&m_threadID);
	
	m_hThread = (HANDLE)CreateThread(NULL, 0, input_ThreadProc, NULL, 0, &m_threadID);
	if (m_hThread == NULL) {
		output_close();
		m_errNo = ERR_DECODE_THREAD;
		return FALSE;
	}

	//èÛë‘É`ÉFÉbÉN
	int Cnt;
	for ( Cnt = 0; Cnt < 100; Cnt++){
		::Sleep(10);
		if ( m_state == STATE_PLAY ) {
			break;
		} else if ( Cnt == 99 ){
			m_errNo = ERR_PLAY;
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CMP3Input::input_playDecodeWave(const char* pszWaveName)
{
	BYTE			buf[1024];
	BOOL			result;
	DWORD			bytesWrite;

	if (m_state != STATE_STOP) {
		//í‚é~íÜà»äOÇÕé¿çsïsâ¬
		m_errNo = ERR_NOT_STATE_STOP;
		return FALSE;
	}

	//WAVEÉtÉ@ÉCÉã
	if ( m_waveFlag == TRUE ){
		//m_errNo = ERR_NOT_STATE_STOP;
		return FALSE;
	}

	decFlag = 2;
	m_fadeOutFlag = false;
	m_playFlag = TRUE;
	m_readWaveDataSize = 0;

	::SetFilePointer(m_hFile, m_firstSync, NULL, FILE_BEGIN);
	m_inputSize		= m_decInfo.maxInputSize * m_inputBlock;
	m_inputBuf		= (BYTE*)::realloc(m_inputBuf, m_inputSize);
	m_inputPtr		= m_inputBuf;
	m_inputBytes	= input_read(m_inputPtr, m_inputSize);
	if (m_inputBytes == 0) {
		m_errNo = ERR_INVALID_VALUE;
		return FALSE;
	}

	//èoóÕÉfÉoÉCÉXÇäJÇ≠
	OutputInfo	outputInfo;
	outputInfo.channels			= m_decInfo.channels;
	outputInfo.bitsPerSample	= m_decInfo.bitsPerSample;
	outputInfo.frequency		= m_decInfo.frequency;

	outputInfo.bufferSize		= m_decInfo.outputSize * m_outputBlock_in;

	if (!output_open(NULL, outputInfo)) {
		m_errNo = ERR_OPEN_OUT_DEVICE;
		return FALSE;
	}

	m_outputBuf_in= (BYTE*)::realloc(m_outputBuf_in, outputInfo.bufferSize);

	if (!::mp3DecodeStart(m_inputPtr, m_inputBytes)) {
		output_close();
		m_errNo = ERR_DECODE;
		return FALSE;
	}
	m_inputPtr += m_decInfo.skipSize;
	m_inputBytes -= m_decInfo.skipSize;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	m_hWaveFile = ::CreateFile(pszWaveName, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hWaveFile == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_WAV_FILE_OPEN;
		return FALSE;
	}

	SetFilePointer(m_hWaveFile, 0, NULL, FILE_BEGIN);
	StringCchCopy((char*)buf,1024,"RIFF");
	result = ::WriteFile(m_hWaveFile, &buf, 4, &bytesWrite, NULL);
	::memset(&buf, 0, 4);
	result = ::WriteFile(m_hWaveFile, &buf, 4, &bytesWrite, NULL);
	StringCchCopy((char*)buf,1024,"WAVEfmt ");
	result = ::WriteFile(m_hWaveFile, &buf, 8, &bytesWrite, NULL);

	//É`ÉÉÉìÉNÉTÉCÉY
	buf[0] = 0x10;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	result = ::WriteFile(m_hWaveFile, &buf, 4, &bytesWrite, NULL);

	//ÉtÉHÅ[É}ÉbÉgÇhÇc
	buf[0] = 0x01;
	buf[1] = 0x00;
	result = ::WriteFile(m_hWaveFile, &buf, 2, &bytesWrite, NULL);

	//É`ÉÉÉìÉlÉãêî
	buf[0] = m_decInfo.channels;
	buf[1] = 0x00;
	result = ::WriteFile(m_hWaveFile, &buf, 2, &bytesWrite, NULL);

	//ÉTÉìÉvÉäÉìÉOÉåÅ[Ég
	DWORD nSamplesPerSec = m_decInfo.frequency;
	result = ::WriteFile(m_hWaveFile, &nSamplesPerSec, 4, &bytesWrite, NULL);

	//ì]ëóÉåÅ[Ég
	WORD  nBlockAlign = (m_decInfo.channels * m_decInfo.bitsPerSample / 8);
	DWORD nAvgBytesPerSec = (nBlockAlign * m_decInfo.frequency);
	result = ::WriteFile(m_hWaveFile, &nAvgBytesPerSec, 4, &bytesWrite, NULL);

	//ÉuÉçÉbÉNÉTÉCÉYêî
	result = ::WriteFile(m_hWaveFile, &nBlockAlign, 2, &bytesWrite, NULL);

	//ó éqâªÉrÉbÉgêî
	buf[0] = m_decInfo.bitsPerSample;
	buf[1] = 0x00;
	result = ::WriteFile(m_hWaveFile, &buf, 2, &bytesWrite, NULL);

	StringCchCopy((char*)buf,1024,"data");
	result = ::WriteFile(m_hWaveFile, &buf, 4, &bytesWrite, NULL);
	::memset(&buf, 0, 4);
	result = ::WriteFile(m_hWaveFile, &buf, 4, &bytesWrite, NULL);

	//ÉfÉRÅ[ÉhópÉXÉåÉbÉhäJén
	//m_hThread = (HANDLE)::_beginthreadex(NULL, 0, 
	//		(THREADFUNC)input_ThreadProc, NULL, 0, (unsigned int*)&m_threadID);

	m_hThread = (HANDLE)CreateThread(NULL, 0, 
			input_ThreadProc, NULL, 0, &m_threadID);

	if (m_hThread == NULL) {
		output_close();
		m_errNo = ERR_DECODE_THREAD;
		return FALSE;
	}

	//èÛë‘É`ÉFÉbÉN
	int Cnt;
	for ( Cnt = 0; Cnt < 100; Cnt++){
		::Sleep(10);
		if ( m_state == STATE_PLAY ) {
			break;
		} else if ( Cnt == 99 ){
			m_errNo = ERR_PLAY;
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CMP3Input::input_stop()
{
	if (m_state == STATE_STOP) {
		//í‚é~íÜÇÕèàóùÇµÇ»Ç¢
		m_errNo = ERR_STATE_STOP;
		return TRUE;
	}

	if ( m_threadID == 0 ) {
		output_stop();
	//	output_close();
	}
	if (::PostThreadMessage(m_threadID, EV_STOP, 0, 0)) {
		::WaitForSingleObject(m_hThread, 500);
	}

	m_playSec = 0;
	m_playSecCur = 0;
	m_playMs = 0;
	m_firstSample = 0;

	//èÛë‘É`ÉFÉbÉN
	int Cnt;
	for ( Cnt = 0; Cnt < 100; Cnt++){
		if ( m_state == STATE_STOP ) {
			break;
		} else if ( Cnt == 99 ){
			m_errNo = ERR_STOP;
			return FALSE;
		}
		::Sleep(10);
	}

	return TRUE;
}

BOOL CMP3Input::input_pause()
{
	int w_state = m_state;
	if (w_state == STATE_PAUSE ||
			w_state == STATE_STOP) {
		//àÍéûí‚é~íÜÅAí‚é~íÜÇÕèàóùÇµÇ»Ç¢
		m_errNo = ERR_NOT_STATE_PLAY;
		return TRUE;
	}

	if ( m_threadID == 0 ) {
		input_actionPause();
		return TRUE;
	} else {
		return input_sendEvent(EV_PAUSE, 0);
	}
}

BOOL CMP3Input::input_restart()
{
	if (m_state != STATE_PAUSE) {
		//àÍéûí‚é~íÜà»äOÇÕèàóùÇµÇ»Ç¢
		return TRUE;
	}

	if ( m_threadID == 0 ) {
		input_actionRestart();
		return TRUE;
	} else {
		return input_sendEvent(EV_RESTART, 0);
	}
}

void CMP3Input::input_setEqualizer(int* pTable)
{
	if (m_threadID) {
		input_sendEvent(EV_EQUALIZER, (int)pTable);
	}
	else {
		mp3SetEqualizer(pTable);
	}
}


void CMP3Input::input_seekByFrame(int frame)
{
	m_seekFlag = TRUE;
	int sample = input_bytesToSample(frame * m_decInfo.outputSize);
	input_seekBySample(sample);
}

void CMP3Input::input_seekBySample(int sample)
{
	int w_state = m_state;
	m_seekFlag = TRUE;
	if ( m_threadID == 0 && m_state != STATE_STOP) {
		input_stop();
		//::Sleep(100);
		input_play();
		if ( w_state == STATE_PAUSE )
			input_pause();
		//::Sleep(100);
		//input_actionSeek(sample);
	}
	//} else {
	//if ( m_threadID != 0 )
		input_sendEvent(EV_SEEK, sample);
	//}
	output_softVolumeMax();
}

void CMP3Input::input_seekBySec(int sec)
{
	m_seekFlag = TRUE;
	int sample = input_secToSample((int)(sec / m_playSpeed));
	input_seekBySample(sample);
}

int CMP3Input::input_bytesToSample(int bytes)
{
	if (m_decInfo.bitsPerSample != 0 && m_decInfo.channels != 0){
		if ( bytes > ALLOW_DATA_SIZE ) {
			return (bytes / m_decInfo.bitsPerSample / m_decInfo.channels * 8);
		} else {
			return (bytes * 8 / m_decInfo.bitsPerSample / m_decInfo.channels);
		}
	} else {
		return 0;
	}
}

int CMP3Input::input_secToSample(int sec)
{
	return (sec * m_decInfo.frequency);
}

int CMP3Input::input_sampleToSec(int sample)
{
	if ( m_decInfo.frequency == 0 ) return 0;
	return (sample / m_decInfo.frequency);
}

long CMP3Input::input_sampleToMs(int sample)
{
	if ( m_decInfo.frequency == 0 ) return 0;
	return (sample * 1000 / m_decInfo.frequency);
}

BOOL CMP3Input::input_skipNextSync()
{
	int sync;
	m_inputPtr   ++;
	m_inputBytes --;
	for (;;) {
		if (::mp3FindSync(m_inputPtr, m_inputBytes, &sync)) {
			m_inputPtr += sync;
			m_inputBytes -= sync;
			return true;
		}
		::memmove(m_inputBuf, m_inputPtr, m_inputBytes);
		int bytesRead = input_read(&m_inputBuf[m_inputBytes], m_inputSize - m_inputBytes);
		if (bytesRead == 0) {
			m_errNo = ERR_MP3_FILE_READ;
			return false;//no more data
		}
		m_inputPtr = m_inputBuf;
		m_inputBytes += bytesRead;
	}
}

/*
BOOL input_seek(int pos)
{
	int w_state = m_state;
	if (w_state != STATE_PLAY &&
			w_state != STATE_PAUSE) {
		return FALSE;
	}

	if ( m_threadID == 0 ) {
		int sample = input_secToSample(pos);
		input_actionSeek(sample);
		return TRUE;
	} else {
		return input_sendEvent(EV_SEEK, pos);
	}

	return TRUE;
}
*/

BOOL CMP3Input::input_setVolume(int left, int right)
{
	return output_setVolume(left, right);
}

BOOL CMP3Input::input_getVolume(int* left, int* right)
{
	return output_getVolume(left, right);
}

int CMP3Input::input_getWaveOutSupport()
{
    int support = output_getWaveOutSupport();
    if (support & WAVECAPS_LRVOLUME) {
        //ç∂âEÉnÅ[ÉhÉ{ÉäÉÖÅ[ÉÄê›íËâ¬î
		return 2;
    }
    else if (support & WAVECAPS_VOLUME) {
        //ÉnÅ[ÉhÉ{ÉäÉÖÅ[ÉÄê›íËâ¬î\Åiç∂âEí≤êÆïsâ¬Åj
		return 1;
    }
    else {
        //ÉnÅ[ÉhÉ{ÉäÉÖÅ[ÉÄê›íËïsâ¬î
		return 0;
    }
}


void CMP3Input::input_setSoftVolume(int left, int right)
{
	output_setSoftVolume(left, right);
}

void CMP3Input::input_getSoftVolume(int* left, int* right)
{
	output_getSoftVolume(left, right);
}

void CMP3Input::input_getSpectrum(int* pSpecL, int* pSpecR)
{
	output_getSpectrum(output_getCurrentSample(), pSpecL, pSpecR);
	return;
}

void CMP3Input::input_getWave(int* pWaveL, int* pWaveR)
{
	int sample = output_getCurrentSample();
	if ( sample ) output_getWave(sample, pWaveL, pWaveR);
	return;
}

BOOL CMP3Input::input_sendEvent(int code, int param)
{
	HANDLE hEvent = ::CreateEvent(NULL, false, false, NULL);
	if (hEvent == NULL) {
		m_errNo = ERR_CREATE_EVENT;
		return FALSE;
	}
	if (::PostThreadMessage(m_threadID, code, (WPARAM)hEvent, param)) {
		::WaitForSingleObject(hEvent, 500);
	}
	::CloseHandle(hEvent);
	return TRUE;
}

unsigned long __stdcall CMP3Input::input_ThreadProc(void * lParam)
{
	return input_decode();
}

int CMP3Input::input_decode()
{
	MPEG_DECODE_PARAM param;
	MSG		msg;
	DWORD	readBytes;
	int		oldSec;
	int		waitLCount = 0;
	int		waitRCount = 0;

	BOOL			result;
	DWORD			bytesWrite;
	BOOL m_bTerminer = false;

	oldSec = 0;
	m_state = STATE_PLAY;
	m_playSec = 0;
	m_playSecCur = 0;
	m_playMs = 0;
	m_playSamples = 0;
	m_firstSample = 0;
	decodePos = 0;
	param.outputBuf	= m_outputBuf_in;

	if ( callBackFlag == TRUE )
		m_pProc(INPUT_MSG_PLAY, m_playSec);
	
	for (;;) {
		for (;;) {
			if ( m_waveFlag == FALSE ){
				param.inputBuf	= m_inputPtr;
				param.inputSize	= m_inputBytes;

				if (!::mp3DecodeFrame(&param)) {
					int err = ::mp3GetLastError();
					if (err == MP3_ERROR_OUT_OF_BUFFER) {
						break;//read next buffer
					}
					//invalid frame !!
					if ( callBackFlag == TRUE )
							m_pProc(INPUT_MSG_ERROR, 0);
					if (!input_skipNextSync()) {
						goto exit;
					}
					continue;
				}

				m_bitRate = param.bitRate;
				//decodePos    += param.inputSize;		//éûä‘éÊìæóp
				m_inputPtr   += param.inputSize;
				m_inputBytes -= param.inputSize;
				//m_playMs     += m_msPerFrame;
			} else {
				//WAVEÉtÉ@ÉCÉã
				if ( (int)dataSize >= m_readWaveDataSize ) {
					param.outputSize = m_inWaveBufSize;
					m_readWaveDataSize = m_readWaveDataSize + param.outputSize;
					if ( (int)dataSize < m_readWaveDataSize ) param.outputSize = m_readWaveDataSize - dataSize;
					if ( m_bitRate == 8 ) {
						::memset(param.outputBuf, 128, param.outputSize);
					} else {
						::memset(param.outputBuf, 0, param.outputSize);
					}
					readBytes = input_read(param.outputBuf, param.outputSize);
					if (readBytes == 0) 
					{
						m_bTerminer = 1;
						goto exit;//no more data
					}
				} 
				else 
				{
					m_bTerminer = 1;
					goto exit;
				}
			}

			//ÉfÉRÅ[Éh or WAVEÉtÉ@ÉCÉãèoóÕ
			if (decFlag == 0 || decFlag == 2) 
			{
				if (output_write(param.outputBuf, param.outputSize))
				{
					m_playSamples = output_getCurrentSample();
					//m_playMs = (((float)m_playSamples + (float)m_firstSample) * (float)1000 / (float)m_decInfo.frequency) / (float)1000;
					m_playSec = (int)(input_sampleToSec(m_playSamples + m_firstSample) * m_playSpeed);

					

					if (m_playSec != oldSec)
					{
						SendMessage(m_HwndWindow,WM_COMMAND,MAKEWPARAM(IDC_POS,0),(LPARAM)m_playSec);
						if ( callBackFlag == TRUE )
							m_pProc(INPUT_MSG_PLAY, m_playSec);
						oldSec = m_playSec;
					}
				}
			}
			if (decFlag == 1 || decFlag == 2) {
				decodePos += param.outputSize;
				result = ::WriteFile(m_hWaveFile, param.outputBuf, param.outputSize, &bytesWrite, NULL);
				m_playSamples = output_getCurrentSample();
				//m_playMs = (((float)m_playSamples + (float)m_firstSample) * (float)1000 / (float)m_decInfo.frequency) / (float)1000;
				m_playSec = (int)(input_sampleToSec(m_playSamples + m_firstSample) * m_playSpeed);
				if (m_playSec != oldSec) {
					if ( callBackFlag == TRUE )
						m_pProc(INPUT_MSG_PLAY, m_playSec);
					oldSec = m_playSec;
				}
			}

			if ( m_fadeOutFlag ){
				waitLCount++;
				waitRCount++;
/*
				if ( m_waveFlag ){
					if ( m_lvol > 5 ) {
						m_lvol = m_lvol - 5;
					} else if (m_lvol > 0) {
						m_lvol--;
					}

					if ( m_rvol > 5 ) {
						m_rvol = m_rvol - 5;
					} else if (m_rvol > 0) {
						m_rvol--;
					}

					waitLCount=0;
					waitRCount=0;
				} else {
*/
					if ( (m_lvol > 50 && waitLCount == 2) ||
						 (m_lvol > 10 && waitLCount == 3) ||
						 (m_lvol > 5  && waitLCount == 5) ||
						 (m_lvol > 0  && waitLCount == 10) ) {
						m_lvol--;
						waitLCount=0;
					}

					if ( (m_rvol > 50 && waitRCount == 2) ||
						 (m_rvol > 10 && waitRCount == 3) ||
						 (m_rvol > 5  && waitRCount == 5) ||
						 (m_rvol > 0  && waitRCount == 10) ) {
						m_rvol--;
						waitRCount=0;
//					}
				}
				output_setVolume(m_lvol, m_rvol);

				if (m_lvol == 0 && m_rvol == 0) {
					m_lvol=m_olvol;
					m_rvol=m_orvol;
					m_fadeOutFlag = false;
					output_stop();
					goto exit;
				}
			}
			
			if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				input_handleEvent(msg);
			}
			while (m_state == STATE_PAUSE) {
				if (::GetMessage(&msg, NULL, 0, 0)) {
					input_handleEvent(msg);
				}
			}
			if (m_state == STATE_STOP) 
			{
				goto exit;
			}
		}
		if ( m_waveFlag == FALSE ){
			::memmove(m_inputBuf, m_inputPtr, m_inputBytes);
			readBytes = input_read(&m_inputBuf[m_inputBytes], m_inputSize - m_inputBytes);
			if (readBytes == 0) 
			{
				m_bTerminer = 1;
				break;//no more data
			}
			m_inputPtr = m_inputBuf;
			m_inputBytes += readBytes;
			::Sleep(m_inputSleep);
		}
	}
	
exit:

	m_threadID = 0;

	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		HANDLE hEvent = (HANDLE)msg.wParam;
		if (hEvent != NULL) {
			::SetEvent(hEvent);
		}
	}
	::CloseHandle(m_hThread);
	m_hThread = NULL;
	if (m_state != STATE_STOP) 
	{
		input_actionStop(TRUE);
		if ( callBackFlag == TRUE )
			m_pProc(INPUT_MSG_PLAYDONE, 0);

		m_bTerminer = true;
	}

	m_playSec = 0;
	m_playSecCur = 0;
	m_playMs = 0;
	m_playSamples = 0;
	m_firstSample = 0;

	if (decFlag == 1 || decFlag == 2) {
		::SetEndOfFile(m_hWaveFile);
		DWORD cSize = ::GetFileSize(m_hWaveFile, NULL) - 8;
		SetFilePointer(m_hWaveFile, 4, NULL, FILE_BEGIN);
		result = ::WriteFile(m_hWaveFile, &cSize, 4, &bytesWrite, NULL);

		DWORD dSize = cSize - 36;
		SetFilePointer(m_hWaveFile, 40, NULL, FILE_BEGIN);
		result = ::WriteFile(m_hWaveFile, &dSize, 4, &bytesWrite, NULL);

		::CloseHandle(m_hWaveFile);
	}

	if(m_bTerminer)
		SendMessage(m_HwndWindow,WM_COMMAND,MAKEWPARAM(IDC_FINFICHIER,0),(LPARAM)m_playSec);


	return 0;
}

void CMP3Input::input_handleEvent(const MSG& msg)
{
	switch (msg.message) {
	case EV_STOP:
		input_actionStop(FALSE);
		break;
	case EV_PAUSE:
		input_actionPause();
		m_state = STATE_PAUSE;
		break;
	case EV_RESTART:
		input_actionRestart();
		m_state = STATE_PLAY;
		break;
	case EV_SEEK:
		input_actionSeek(msg.lParam);
		break;
	case EV_EQUALIZER:
		mp3SetEqualizer((int*)msg.lParam);
		break;
	}
	HANDLE hEvent = (HANDLE)msg.wParam;
	if (hEvent != NULL) {
		::SetEvent(hEvent);
	}

}

void CMP3Input::input_actionStop(BOOL wait)
{
	if (!wait) {
		//ã≠êßí‚é~
		output_stop();
	}
	output_close();
	if (wait) {
		//âï˙
		output_stop();
	}
	m_state = STATE_STOP;
	if ( callBackFlag == TRUE && !wait)
		m_pProc(INPUT_MSG_STOP, 0);
}

void CMP3Input::input_actionPause()
{
	if (m_state == STATE_STOP) {
		return;//ignore
	}
	if (m_state == STATE_PLAY) {
		output_pause();
	}
	m_state = STATE_PAUSE;
	if ( callBackFlag == TRUE )
		m_pProc(INPUT_MSG_PAUSE, 0);
}

void CMP3Input::input_actionRestart()
{
	if (m_state == STATE_PAUSE) {
		output_restart();
	}
	m_state = STATE_PLAY;
	if ( callBackFlag == TRUE )
		m_pProc(INPUT_MSG_PLAY, m_playSec);

}

void CMP3Input::input_actionSeek(int seekSample)
{

//	input_actionSpeedSeek(seekSample);
//	return;
	MPEG_DECODE_PARAM param;
	int bytesRead, outputBytes;

	output_stop();
	m_state = STATE_SEEK;
	::SetFilePointer(m_hFile, m_firstSync, NULL, FILE_BEGIN);
	
	m_playSecCur = 0;
	m_playMs = 0;
	m_inputBytes = 0;
	m_playSamples = 0;
	outputBytes = 0;
	param.outputBuf	= m_outputBuf_in;
	m_readWaveDataSize = 0; 

	if ( m_waveFlag == FALSE){
		::mp3MuteStart(&param);
		for (;;) {
			::memmove(m_inputBuf, m_inputPtr, m_inputBytes);
			bytesRead = input_read(&m_inputBuf[m_inputBytes], m_inputSize - m_inputBytes);
			if (bytesRead == 0) break;//no more data
			m_inputPtr = m_inputBuf;
			m_inputBytes += bytesRead;

			for (;;) {
				param.inputBuf	= m_inputPtr;
				param.inputSize	= m_inputBytes;
				if (!::mp3DecodeFrame(&param)) {
					int err = ::mp3GetLastError();
					if (err == MP3_ERROR_OUT_OF_BUFFER) {
						break;//read next buffer
					}
					//invalid frame !!
					if ( callBackFlag == TRUE )
						m_pProc(INPUT_MSG_ERROR, 0);
					if (!input_skipNextSync()) {
						goto exit;
					}
					continue;
				}

				m_inputPtr   += param.inputSize;
				m_inputBytes -= param.inputSize;
				outputBytes += param.outputSize;
				m_firstSample = input_bytesToSample(outputBytes);
				if (m_firstSample >= seekSample) {
					m_state = STATE_PLAY;
					goto exit;
				}
			}
		}
	} else {
		m_firstSample = seekSample;
		decodePos = m_firstSample;
		outputBytes = (int)output_sampleToBytes(m_firstSample);
		::SetFilePointer(m_hFile, outputBytes, NULL, FILE_CURRENT);
		m_readWaveDataSize = m_readWaveDataSize + outputBytes;
	}

exit:
	if ( m_waveFlag == FALSE ) 
		::mp3MuteEnd(&param);
	m_seekFlag = FALSE;

//	if ( callBackFlag == TRUE )
//		m_pProc(INPUT_MSG_PLAY, m_playSec);

}

/*
long CMP3Input::input_sampleToMp3(int Sample) 
{

	int Sec;
	int xBit;
	Sec = input_sampleToSec(Sample);
	xBit = Sec * m_bitRate;
	return xBit / 8;

}*/

/*
__int64 CMP3Input::myFileSeek (HANDLE hf, __int64 distance, DWORD MoveMethod)
{
   LARGE_INTEGER li;

   li.QuadPart = distance;

   li.LowPart = SetFilePointer (hf, li.LowPart, &li.HighPart, MoveMethod);

	if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
   {
      li.QuadPart = -1;
   }

   return li.QuadPart;
}*/

/*/
void input_actionSpeedSeek(int seekSample)
{
	MPEG_DECODE_PARAM param;
	DWORD bytesRead, outputBytes;
	long NextBufRead;

	output_stop();
	m_state = STATE_SEEK;
	NextBufRead = input_sampleToMp3(seekSample);
	//NextBufRead = 71680000;
	//::SetFilePointer(m_hFile, m_firstSync + NextBufRead, NULL, FILE_BEGIN);
		

//X-MaD 02/08/01
	BYTE buf[2048];
	int sync;
	long result;
	MPEG_DECODE_INFO decInfo;
	bool PORCODIO = false;
	int InputBytes = sizeof(buf);
	while (!PORCODIO) {

	
	::SetFilePointer(m_hFile, NextBufRead, NULL, FILE_BEGIN);
	result = ::ReadFile(m_hFile, buf, InputBytes, &bytesRead, NULL);
	if (!result || bytesRead == 0) {
		m_errNo = ERR_MP3_FILE_READ;
		input_close();
		return;
	}
	
	
	if (!::mp3FindSync(buf, InputBytes, &sync)) {
		input_close();
		m_errNo = ERR_FRAME_HEADER_NOT_FOUND;
		return;
	}

	NextBufRead += sync;

	::SetFilePointer(m_hFile, NextBufRead, NULL, FILE_BEGIN);
	result = ::ReadFile(m_hFile, buf, sizeof(buf), &bytesRead, NULL);
	if (!result || bytesRead == 0) {
		input_close();
		m_errNo = ERR_MP3_FILE_READ;
		return;
	}


	if (!::mp3GetDecodeInfo(buf, bytesRead, &decInfo, 1)) {
		m_errNo = ERR_FRAME_HEADER_READ;
		NextBufRead += InputBytes;
	}
	else {
		PORCODIO = true;
	}

	}

::SetFilePointer(m_hFile, NextBufRead, NULL, FILE_BEGIN);
//X-MaD 02/08/01

	m_playSecCur = 0;
	m_playMs = 0;
	m_inputBytes = 0;
	m_playSamples = 0;
	outputBytes = 0;
	param.outputBuf	= m_outputBuf_in;
	m_readWaveDataSize = 0; 

	if ( m_waveFlag == FALSE){
		::mp3MuteStart(&param);
		for (;;) {
			::memmove(m_inputBuf, m_inputPtr, m_inputBytes);
			bytesRead = input_read(&m_inputBuf[m_inputBytes], m_inputSize - m_inputBytes);
			if (bytesRead == 0) break;//no more data
			m_inputPtr = m_inputBuf;
			m_inputBytes += bytesRead;

			for (;;) {
				param.inputBuf	= m_inputPtr;
				param.inputSize	= m_inputBytes;
				if (!::mp3DecodeFrame(&param)) {
					int err = ::mp3GetLastError();
					if (err == MP3_ERROR_OUT_OF_BUFFER) {
						break;//read next buffer
					}
					//invalid frame !!
					if ( callBackFlag == TRUE )
						m_pProc(INPUT_MSG_ERROR, 0);
					if (!input_skipNextSync()) {
						goto exit;
					}
					continue;
				}

				m_inputPtr   += param.inputSize;
				m_inputBytes -= param.inputSize;
				outputBytes += param.outputSize;
				m_firstSample = input_bytesToSample(outputBytes);
				if (m_firstSample >= seekSample) {
					m_state = STATE_PLAY;
					goto exit;
				}
			}
		}
	} else {
		m_firstSample = seekSample;
		decodePos = m_firstSample;
		outputBytes = (int)output_sampleToBytes(m_firstSample);
		::SetFilePointer(m_hFile, outputBytes, NULL, FILE_CURRENT);
		m_readWaveDataSize = m_readWaveDataSize + outputBytes;
	}

exit:
	if ( m_waveFlag == FALSE ) 
		::mp3MuteEnd(&param);
	m_seekFlag = FALSE;

//	if ( callBackFlag == TRUE )
//		m_pProc(INPUT_MSG_PLAY, m_playSec);

}/*/

/******************** ID3Tagv1.cpp ************************/
BOOL CMP3Input::readID3v1(HANDLE FileHandle, TAG_INFO* lpTagInfo)
{
	ID3Tagv1	tag;
	BOOL		result;
	DWORD		bytesRead;

	//ÉtÉ@ÉCÉãÇÃå„ÇÎÇ©ÇÁ128ÉoÉCÉgÇì«Ç›çûÇﬁ
	::SetFilePointer(FileHandle, -(long)sizeof(tag), NULL, FILE_END);
	result = ::ReadFile(FileHandle, &tag, sizeof(tag), &bytesRead, NULL);
	if (!result || bytesRead != sizeof(tag)) {
		return FALSE;
	}
	if (::memcmp(tag.tag, "TAG", 3) != 0) {
		return FALSE;
	}
	m_FileType += 16;

	//É^ÉOèÓïÒï€ë∂
	strcpy_sID3v1(lpTagInfo->szTrackName,  (char*)tag.trackName);
	strcpy_sID3v1(lpTagInfo->szArtistName, (char*)tag.artistName);
	strcpy_sID3v1(lpTagInfo->szAlbumName, (char*)tag.albumName);
	strcpy_sID3v1(lpTagInfo->szComment, (char*)tag.comment);
	::memcpy(lpTagInfo->szYear,  tag.year,  4);
	lpTagInfo->genre = tag.genre;
	::memcpy(lpTagInfo->szGanreName,  genre_tbl[tag.genre],  strlen(genre_tbl[tag.genre]));
	return TRUE;
}

BOOL CMP3Input::readID3v11(HANDLE FileHandle, TAG_INFO_11* lpTagInfo)
{
	ID3Tagv1	tag;
	BOOL		result;
	DWORD		bytesRead;

	//ÉtÉ@ÉCÉãÇÃå„ÇÎÇ©ÇÁ128ÉoÉCÉgÇì«Ç›çûÇﬁ
	::SetFilePointer(FileHandle, -(long)sizeof(tag), NULL, FILE_END);
	result = ::ReadFile(FileHandle, &tag, sizeof(tag), &bytesRead, NULL);
	if (!result || bytesRead != sizeof(tag)) {
		return FALSE;
	}
	if (::memcmp(tag.tag, "TAG", 3) != 0) {
		return FALSE;
	}
	m_FileType += 16;

	//É^ÉOèÓïÒï€ë∂
	strcpy_sID3v1(lpTagInfo->szTrackName,  (char*)tag.trackName);
	strcpy_sID3v1(lpTagInfo->szArtistName, (char*)tag.artistName);
	strcpy_sID3v1(lpTagInfo->szAlbumName, (char*)tag.albumName);
	strcpy_sID3v1(lpTagInfo->szComment, (char*)tag.comment);
	if ( lpTagInfo->szComment[28] == 0 ) {
		lpTagInfo->trackNo = (BYTE)lpTagInfo->szComment[29];
		lpTagInfo->szComment[29] = 0;
	} else {
		lpTagInfo->trackNo = -1;
	}
	::memcpy(lpTagInfo->szYear,  tag.year,  4);
	lpTagInfo->genre = tag.genre;
	::memcpy(lpTagInfo->szGanreName,  genre_tbl[tag.genre],  strlen(genre_tbl[tag.genre]));
	return TRUE;
}




/******************** ID3Tagv2.cpp ************************/
BOOL CMP3Input::readID3v2(HANDLE FileHandle,int* fSync)
{
	ID3Tagv2	tag;
	BOOL		result;
	DWORD		bytesRead;
	BYTE		temp[4];
	int			v3size;

	//ÉtÉ@ÉCÉãÇÃì™Ç…ñﬂÇËÅAÉ^ÉOÇÃÉwÉbÉ_Çì«Ç›çûÇﬁ
	::SetFilePointer(FileHandle, *fSync, NULL, FILE_BEGIN);
	result = ::ReadFile(FileHandle, &tag, sizeof(tag), &bytesRead, NULL);
	if (!result || bytesRead != sizeof(tag)) {
		return FALSE;
	}
	if (::memcmp(tag.id3, "ID3", 3) != 0) {
		return FALSE;
	}
	m_FileType += 4;

	//28ÉrÉbÉgÇÃêîílÇïœä∑
	temp[3] =   tag.size[3]             | ((tag.size[2] & 1) << 7);
	temp[2] = ((tag.size[2] >> 1) & 63) | ((tag.size[1] & 3) << 6);
	temp[1] = ((tag.size[1] >> 2) & 31) | ((tag.size[0] & 7) << 5);
	temp[0] = ((tag.size[0] >> 3) & 15);
	v3size = temp[3] | ((DWORD)temp[2] << 8) | 
			((DWORD)temp[1] << 16) | ((DWORD)temp[0] << 24) + 10;
	*fSync = *fSync + v3size;
	return TRUE;
}


#define CHECK(x)	(::memcmp(pRMPInfo->infoID, x, 4) == 0)
#define SET(x, y)	strcpy_sRMP(x, (char*)pRMPInfo->data, pRMPInfo->length, y)

void CMP3Input::loadRMPInfo(LIST_INFO* lplistInfo, RMPInfo* pRMPInfo)
{
	if (CHECK("INAM")) {		//track name
		SET(lplistInfo->INAM, INFO_NAME_SIZE);
	}
	else if (CHECK("IART")) {	//artist name
		SET(lplistInfo->IART, INFO_NAME_SIZE);
	}
	else if (CHECK("IPRD")) {
		SET(lplistInfo->IPRD, INFO_NAME_SIZE);
	}
	else if (CHECK("ICMT")) {
		SET(lplistInfo->ICMT, INFO_NAME_SIZE);
	}
	else if (CHECK("ICRD")) {
		SET(lplistInfo->ICRD, INFO_NAME_SIZE);
	}
	else if (CHECK("IGNR")) {
		SET(lplistInfo->IGNR, INFO_NAME_SIZE);
	}
	else if (CHECK("ICOP")) {
		SET(lplistInfo->ICOP, INFO_NAME_SIZE);
	}
	else if (CHECK("IENG")) {
		SET(lplistInfo->IENG, INFO_NAME_SIZE);
	}
	else if (CHECK("ISRC")) {
		SET(lplistInfo->ISRC, INFO_NAME_SIZE);
	}
	else if (CHECK("ISFT")) {
		SET(lplistInfo->ISFT, INFO_NAME_SIZE);
	}
	else if (CHECK("IKEY")) {
		SET(lplistInfo->IKEY, INFO_NAME_SIZE);
	}
	else if (CHECK("ITCH")) {
		SET(lplistInfo->ITCH, INFO_NAME_SIZE);
	}
	else if (CHECK("ILYC")) {
		SET(lplistInfo->ILYC, INFO_NAME_SIZE);
	}
	else if (CHECK("ICMS")) {
		SET(lplistInfo->ICMS, INFO_NAME_SIZE);
	}
}

void CMP3Input::strcpy_sRMP(char* pszDest, const char* pszSrc, int length, int maxSize)
{
	if (length >= maxSize) {
		length = maxSize - 1;
		::memcpy(pszDest, pszSrc, length);
		pszDest[maxSize - 1] = 0;
	}
	else {
		::memcpy(pszDest, pszSrc, length);
		pszDest[length] = 0;
	}
}

BOOL CMP3Input::chkLYRICS3(HANDLE FileHandle, DWORD* dSize)
{
	BOOL		result;
	DWORD		bytesRead;
	BYTE        buf[128];
	char        lysize[7];
	int			lyPoint = 9;

	*dSize = 0;

	//ÉtÉ@ÉCÉãÇÃå„ÇÎÇ©ÇÁ128ÉoÉCÉgÇì«Ç›çûÇﬁ
	::SetFilePointer(FileHandle, -128, NULL, FILE_END);
	result = ::ReadFile(FileHandle, &buf, 128, &bytesRead, NULL);
	if (!result || bytesRead != 128) {
		return FALSE;
	}
	if (::memcmp(buf, "TAG", 3) == 0) {
		//ID3Tag v1 Ç™ë∂ç›Ç∑ÇÈ
		lyPoint += 128;
	}

	//LYRICS3 ÇÃóLñ≥É`ÉFÉbÉN
	::SetFilePointer(FileHandle, -lyPoint, NULL, FILE_END);
	result = ::ReadFile(FileHandle, &buf, 9, &bytesRead, NULL);
	if (!result || (int)bytesRead != 9) {
		return FALSE;
	}
	if (::memcmp(buf, "LYRICS200", 9) != 0) {
		return FALSE;
	} else {
		lyPoint += 6;
		::SetFilePointer(FileHandle, -lyPoint, NULL, FILE_END);
		result = ::ReadFile(FileHandle, &buf, 6, &bytesRead, NULL);
		if (!result || (int)bytesRead != 6) {
			return FALSE;
		}
		::memcpy(lysize, buf, 6);
		lysize[6] = '\0';
		*dSize = atoi(lysize) + 15;
		m_FileType += 32;
		return TRUE;
	}
}

BOOL CMP3Input::input_setLyricsFile(const char* pszLyricsName)
{
	FILE*		m_LyricsFile;				//âÃéåÉtÉ@ÉCÉã
	char		buf[1024];
	long		sec = 0;
	int			lyPoint;
	int			lySize;
	char*		pbuf;
	int			nextTagPoint;
	int			readDataSize;
	int			result;

	if (m_state != STATE_STOP) {
		m_errNo = ERR_NOT_STATE_STOP;
		return FALSE;
	}

	//èâä˙âª
	input_clearLyrics();

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszLyricsName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		return TRUE;
	}

	errno_t err;
	err = fopen_s(&m_LyricsFile, pszLyricsName, "r");

	if(err != 0)
	{
		m_errNo = ERR_LYRICS_FILE_OPEN;
		return FALSE;
	}

	if ( m_KaraokeTagFlag ) {
		readDataSize = 1024;
	} else {
		readDataSize = 128;
	}
	while(fgets(buf, readDataSize, m_LyricsFile) != NULL){
		buf[readDataSize - 1] = '\0';
		m_liricsLinePointMax[m_lyricsLineNo] = 0;
		result = input_getTimeLyrics(buf, &sec, &lyPoint, &lySize, &nextTagPoint);
		if ( result == -1 ){
			if ( m_lyricsCount > 0 ) {
				//ëOÇÃÉ^ÉCÉÄÉ^ÉOÇÉZÉbÉgÅiëOçsÇÃë±Ç´Åj
				m_lyrics[m_lyricsCount].tagFlag = FALSE;
				m_lyrics[m_lyricsCount].sec = m_lyrics[m_lyricsCount-1].sec;
				::memcpy(&m_lyrics[m_lyricsCount].lyrics, &buf[lyPoint], lySize);
				m_lyrics[m_lyricsCount].lineno = m_lyricsLineNo;
				m_liricsLinePointMax[m_lyricsLineNo]++;
				m_lyrics[m_lyricsCount].point = m_liricsLinePointMax[m_lyricsLineNo];
				m_lyricsCount++;
				m_lyricsLineNo++;
				if ( m_lyricsCount == 1000 ){
					::fclose(m_LyricsFile);
					return TRUE;
				}
			}
		} else {
			pbuf = buf;
			for(;;){
				m_lyrics[m_lyricsCount].tagFlag = TRUE;
				m_lyrics[m_lyricsCount].sec = sec;
				::memcpy(&m_lyrics[m_lyricsCount].lyrics, &pbuf[lyPoint], lySize);
				m_lyrics[m_lyricsCount].lineno = m_lyricsLineNo;
				m_liricsLinePointMax[m_lyricsLineNo]++;
				m_lyrics[m_lyricsCount].point = m_liricsLinePointMax[m_lyricsLineNo];
				m_lyricsCount++;
				if ( m_lyricsCount == 1000 ){
					::fclose(m_LyricsFile);
					return TRUE;
				}
				if ( result == 0 ) {
					m_lyricsLineNo++;
					break;
				}
				pbuf += nextTagPoint;
				result = input_getTimeLyrics(pbuf, &sec, &lyPoint, &lySize, &nextTagPoint);
			}
		}
	}

	::fclose(m_LyricsFile);
	return TRUE;

}

BOOL CMP3Input::input_setFileLyrics3Info(const char* pszName, LYRICS3_INFO* lpLyrics3Info, int setFlag)
{
	HANDLE			hFileLylics3;			//LYRICS3ï“èWÉtÉ@ÉCÉã
	BYTE			tag[128];
	BOOL			result;
	DWORD			bytesRead;
	DWORD			bytesWrite;
	BYTE			buf[302000];
	int				siFlag = 0;
	DWORD           target = 0;
	int				tagFlag = 0;
	int				riffFlag = 0;
	int				lyFlag = 0;
	int				insPoint = 0;
	DWORD			lySize = 0;			//é¿ÉTÉCÉY
	DWORD			fieldSize;
	char			fieldSizeChar[6];
	int				fieldFigure;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileLylics3 = ::CreateFile(pszName, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileLylics3 == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//RIFFå`éÆÉ`ÉFÉbÉN
	SetFilePointer(hFileLylics3, 0, NULL, FILE_BEGIN);
	result = ReadFile(hFileLylics3, buf, 4, &bytesRead, NULL);
	if (!result || bytesRead != 4) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(hFileLylics3);
		return FALSE;
	}
	if (memcmp(buf, "RIFF", 4) == 0) {
		riffFlag = 1;
	}

	//ID3Tagv1 éÊìæ
	SetFilePointer(hFileLylics3, -(long)128, NULL, FILE_END);
	result = ReadFile(hFileLylics3, tag, 128, &bytesRead, NULL);
	if (!result || bytesRead != 128) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(hFileLylics3);
		return FALSE;
	}
	if (memcmp(tag, "TAG", 3) == 0) {
		tagFlag = 1;
		insPoint = 128;
	}

	//LYRICS3ë∂ç›É`ÉFÉbÉN
	if (chkLYRICS3(hFileLylics3, &lySize)) {
		lyFlag = 1;
		insPoint += lySize;
	}

	::memset(buf, '0', sizeof(buf));
	target=0;

	if ( setFlag == 1 ) {
		//LYRICS3äiî[
		::memcpy(&buf[target], "LYRICSBEGIN", 11);
		target += 11;
		::memcpy(&buf[target], "IND00002", 8);
		target += 8;
		buf[target] = lpLyrics3Info->IND_LYR;
		buf[target + 1] = lpLyrics3Info->IND_TIMETAG;
		target += 2;

		::memcpy(&buf[target], "LYR", 3);
		target += 3;
		for (fieldSize = 60000; fieldSize > 0 && (lpLyrics3Info->LYR[fieldSize - 1] == ' ' ||
			                                      lpLyrics3Info->LYR[fieldSize - 1] == '\0') ; fieldSize --) {}
		::_ltoa_s(fieldSize, fieldSizeChar, 6, 10);
		debugNo=fieldSize;
		fieldFigure = ::strlen(fieldSizeChar);
		if ( fieldFigure < 5 ) target += (5 - fieldFigure);
		::memcpy(&buf[target], fieldSizeChar, fieldFigure);
		target += fieldFigure;
		::memcpy(&buf[target], lpLyrics3Info->LYR, fieldSize);
		target += fieldSize;

		::memcpy(&buf[target], "INF", 3);
		target += 3;
		for (fieldSize = 60000; fieldSize > 0 && (lpLyrics3Info->INF[fieldSize - 1] == ' ' ||
			                                      lpLyrics3Info->INF[fieldSize - 1] == '\0') ; fieldSize --) {}
		::_ltoa_s(fieldSize, fieldSizeChar, 6, 10);
		fieldFigure = ::strlen(fieldSizeChar);
		if ( fieldFigure < 5 ) target += (5 - fieldFigure);
		::memcpy(&buf[target], fieldSizeChar, fieldFigure);
		target += fieldFigure;
		::memcpy(&buf[target], lpLyrics3Info->INF, fieldSize);
		target += fieldSize;

		::memcpy(&buf[target], "AUT", 3);
		target += 3;
		for (fieldSize = 250; fieldSize > 0 && (lpLyrics3Info->AUT[fieldSize - 1] == ' ' ||
			                                    lpLyrics3Info->AUT[fieldSize - 1] == '\0') ; fieldSize --) {}
		::_ltoa_s(fieldSize, fieldSizeChar, 6, 10);
		fieldFigure = ::strlen(fieldSizeChar);
		if ( fieldFigure < 5 ) target += (5 - fieldFigure);
		::memcpy(&buf[target], fieldSizeChar, fieldFigure);
		target += fieldFigure;
		::memcpy(&buf[target], lpLyrics3Info->AUT, fieldSize);
		target += fieldSize;

		::memcpy(&buf[target], "EAL", 3);
		target += 3;
		for (fieldSize = 250; fieldSize > 0 && (lpLyrics3Info->EAL[fieldSize - 1] == ' ' ||
			                                    lpLyrics3Info->EAL[fieldSize - 1] == '\0') ; fieldSize --) {}
		::_ltoa_s(fieldSize, fieldSizeChar, 6, 10);
		fieldFigure = ::strlen(fieldSizeChar);
		if ( fieldFigure < 5 ) target += (5 - fieldFigure);
		::memcpy(&buf[target], fieldSizeChar, fieldFigure);
		target += fieldFigure;
		::memcpy(&buf[target], lpLyrics3Info->EAL, fieldSize);
		target += fieldSize;

		::memcpy(&buf[target], "EAR", 3);
		target += 3;
		for (fieldSize = 250; fieldSize > 0 && (lpLyrics3Info->EAR[fieldSize - 1] == ' ' ||
			                                    lpLyrics3Info->EAR[fieldSize - 1] == '\0') ; fieldSize --) {}
		::_ltoa_s(fieldSize, fieldSizeChar, 6, 10);
		fieldFigure = ::strlen(fieldSizeChar);
		if ( fieldFigure < 5 ) target += (5 - fieldFigure);
		::memcpy(&buf[target], fieldSizeChar, fieldFigure);
		target += fieldFigure;
		::memcpy(&buf[target], lpLyrics3Info->EAR, fieldSize);
		target += fieldSize;

		::memcpy(&buf[target], "ETT", 3);
		target += 3;
		for (fieldSize = 250; fieldSize > 0 && (lpLyrics3Info->ETT[fieldSize - 1] == ' ' ||
			                                    lpLyrics3Info->ETT[fieldSize - 1] == '\0') ; fieldSize --) {}
		::_ltoa_s(fieldSize, fieldSizeChar, 6, 10);
		fieldFigure = ::strlen(fieldSizeChar);
		if (fieldFigure > 0) target += (5 - fieldFigure);
		::memcpy(&buf[target], fieldSizeChar, fieldFigure);
		target += fieldFigure;
		::memcpy(&buf[target], lpLyrics3Info->ETT, fieldSize);
		target += fieldSize;

		::memcpy(&buf[target], "IMG", 3);
		target += 3;
		for (fieldSize = 60000; fieldSize > 0 && (lpLyrics3Info->IMG[fieldSize - 1] == ' ' ||
			                                      lpLyrics3Info->IMG[fieldSize - 1] == '\0') ; fieldSize --) {}
		::_ltoa_s(fieldSize, fieldSizeChar, 6, 10);
		fieldFigure = ::strlen(fieldSizeChar);
		if (fieldFigure > 0) target += (5 - fieldFigure);
		::memcpy(&buf[target], fieldSizeChar, fieldFigure);
		target += fieldFigure;
		::memcpy(&buf[target], lpLyrics3Info->IMG, fieldSize);
		target += fieldSize;

		::_ltoa_s(fieldSize, fieldSizeChar, 6, 10);
		fieldFigure = ::strlen(fieldSizeChar);
		if (fieldFigure > 0) target += (6 - fieldFigure);
		::memcpy(&buf[target], fieldSizeChar, fieldFigure);
		target += fieldFigure;

		::memcpy(&buf[target], "LYRICS200", 9);
		target += 9;
	}

	if ( tagFlag == 1 ) {
		::memcpy(&buf[target], tag, 128);
	} else {
		int Cnt;
		::memcpy(&buf[target], "TAG", 3);
		for (Cnt = 3; Cnt < 127; Cnt++){
			::memcpy(&buf[target+Cnt], " ", 1);
		}
		buf[target+127] = 0xFA;
	}
	target +=128;

	//LYRICS3èëÇ´çûÇ›
	::SetFilePointer(hFileLylics3, -insPoint, NULL, FILE_END);
	result = ::WriteFile(hFileLylics3, &buf, target, &bytesWrite, NULL);
	if (!result || bytesWrite != target) {
		::CloseHandle(hFileLylics3);
		m_errNo = ERR_MP3_FILE_WRITE;
		return FALSE;
	}
	::SetEndOfFile(hFileLylics3);

	//SIÉtÉBÅ[ÉãÉhèCïú
	if ( riffFlag == 1 ) {
		LIST_INFO_EX ListInfo;
		if ( !getListInfoEX(hFileLylics3, &ListInfo) ) {
			::CloseHandle(hFileLylics3);
			return FALSE;
		}
		if ( !setListInfoEX(hFileLylics3, &ListInfo) ) {
			::CloseHandle(hFileLylics3);
			return FALSE;
		}
	}

	::CloseHandle(hFileLylics3);
	return TRUE;

}

BOOL CMP3Input::input_getFileLyrics3Info(const char* pszName, LYRICS3_INFO* lpLyrics3Info)
{
	HANDLE			hFileMpeg;				//Mp3ÉtÉ@ÉCÉã
	BOOL			result;

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÉ`ÉFÉbÉN
	if (pszName[0] == 0) {
		//âΩÇ‡ì¸óÕÇ≥ÇÍÇ»Ç©Ç¡ÇΩèÍçáÇÕèIóπ
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	//ÉtÉ@ÉCÉãÉIÅ[ÉvÉì
	hFileMpeg = ::CreateFile(pszName, GENERIC_READ, FILE_SHARE_READ + FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileMpeg == INVALID_HANDLE_VALUE) {
		m_errNo = ERR_MP3_FILE_OPEN;
		return FALSE;
	}

	result = input_getLyrics3Data(hFileMpeg, lpLyrics3Info);
	::CloseHandle(hFileMpeg);

	return result;
}

BOOL CMP3Input::input_getLyrics3Data(HANDLE FileHandle, LYRICS3_INFO* lpLyrics3Info)
{
	DWORD		bytesRead;
	BOOL		result;
	BYTE		tag[128];
	BYTE		id[3];
	char		fSizeData[6];
	int			fSize;
	BYTE*		lyBuf;
	DWORD		lysize = 0;			//é¿ÉTÉCÉY
	int			target = 0;
	int			tagFlag = 0;

	//èâä˙âª
	::memset(lpLyrics3Info, 0, sizeof(LYRICS3_INFO));

	//ID3Tag v1 É`ÉFÉbÉN
	SetFilePointer(FileHandle, -(long)128, NULL, FILE_END);
	result = ReadFile(FileHandle, tag, 128, &bytesRead, NULL);
	if (!result || bytesRead != 128) {
		m_errNo = ERR_MP3_FILE_READ;
		return FALSE;
	}
	if (memcmp(tag, "TAG", 3) == 0) {
		tagFlag = 1;
	}

	//LYRICS3ë∂ç›É`ÉFÉbÉNÅïéÊìæ
	result = chkLYRICS3(FileHandle, &lysize);
	if (!result || lysize == 0) {
		return FALSE;	//LYRICS3 ÉfÅ[É^ñ≥Çµ
	}
		
	//ÉoÉbÉtÉ@ämï€
	lyBuf = (BYTE*)::malloc(lysize);
	if (lyBuf == NULL){
		m_errNo = ERR_MALLOC;
		::free(lyBuf);
		return FALSE;
	}

	//LYRICS3êÊì™à íuÇ÷É|ÉCÉìÉ^à⁄ìÆ
	if ( tagFlag == 1 ){
		SetFilePointer(FileHandle, -(long)(128+lysize), NULL, FILE_END);
	} else {
		SetFilePointer(FileHandle, -(long)(lysize), NULL, FILE_END);
	}

	lysize -= 15;	//ÉTÉCÉYÇ∆LYRICS200ï™ÇÃÉoÉCÉgêîÇÉ}ÉCÉiÉX

	//LYRICS3 ì«Ç›çûÇ›
	result = ReadFile(FileHandle, lyBuf, lysize, &bytesRead, NULL);
	if (!result || bytesRead != lysize) {
		m_errNo = ERR_MP3_FILE_READ;
		::free(lyBuf);
		return FALSE;
	}

	//LYRICS3ÉXÉ^Å[Égà íuÉ`ÉFÉbÉN
	if (memcmp(lyBuf, "LYRICSBEGIN", 11) != 0) {
		return FALSE;
	}

	target += 11;	//"LYRICSBEGIN" ï™à⁄ìÆ

	//ÉfÅ[É^âêÕ
	do {
		::memcpy(id, &lyBuf[target], 3);
		::memcpy(fSizeData, &lyBuf[target+3], 5);
		fSizeData[5] = '\0';
		fSize = atoi(fSizeData);
		target += 8;
		if (::memcmp(id, "IND", 3) == 0) {
			if (fSize > 0) {
				lpLyrics3Info->IND_LYR = lyBuf[target];
			}
			if (fSize > 1) {
				lpLyrics3Info->IND_TIMETAG = lyBuf[target];
			}
		} else if (::memcmp(id, "LYR", 3) == 0) {
			if (fSize > 0)
				::memcpy(lpLyrics3Info->LYR, &lyBuf[target], fSize);
		} else if (::memcmp(id, "INF", 3) == 0) {
			if (fSize > 0)
				::memcpy(lpLyrics3Info->INF, &lyBuf[target], fSize);
		} else if (::memcmp(id, "AUT", 3) == 0) {
			if (fSize > 0)
				::memcpy(lpLyrics3Info->AUT, &lyBuf[target], fSize);
		} else if (::memcmp(id, "EAL", 3) == 0) {
			if (fSize > 0)
				::memcpy(lpLyrics3Info->EAL, &lyBuf[target], fSize);
		} else if (::memcmp(id, "EAR", 3) == 0) {
			if (fSize > 0)
				::memcpy(lpLyrics3Info->EAR, &lyBuf[target], fSize);
		} else if (::memcmp(id, "ETT", 3) == 0) {
			if (fSize > 0)
				::memcpy(lpLyrics3Info->ETT, &lyBuf[target], fSize);
		} else if (::memcmp(id, "IMG", 3) == 0) {
			if (fSize > 0)
				::memcpy(lpLyrics3Info->IMG, &lyBuf[target], fSize);
		} else {
			//::free(lyBuf);
			//return FALSE;
		} 
		target += fSize;
	} while ( target < (int)lysize );

	::free(lyBuf);
	return TRUE;

}

BOOL CMP3Input::input_readLyrics3(HANDLE FileHandle)
{
	char		buf[1024];
	long		sec = 0;
//	char		mm[3];
//	char		ss[3];
//	char        ms[4];
//	char*		lyrics;
//	int			dataEnd;
//	int			targetLy;
	int			lyPoint;
	int			lySize;
	char*		pbuf;
	int			nextTagPoint;
	int			readDataSize;
	int			result;

	LYRICS3_INFO Lyrics3Info;
	int lyricsSize = 0;
	int startPoint = 0;
	int lineSize = 0;
	int inputSize = 0;		//Max 128
	int target = 0;


	if (m_state != STATE_STOP) {
		//í‚é~íÜà»äOÇÕé¿çsïsâ¬
		m_errNo = ERR_NOT_STATE_STOP;
		return FALSE;
	}

	m_lyricsCount = 0;
	m_lyricsCurrent = 0;
	::memset(m_lyrics, 0, sizeof(m_lyrics));

	//LYRICS3 ÉfÅ[É^ì«Ç›çûÇ›
	if (!(input_getLyrics3Data(FileHandle, &Lyrics3Info))) {
		return FALSE;
	}

	lyricsSize = ::strlen(Lyrics3Info.LYR);
	if (lyricsSize == 0) {
		return FALSE;
	}

	if ( m_KaraokeTagFlag ) {
		readDataSize = 1024;
	} else {
		readDataSize = 128;
	}

	//âÃéåâêÕ
	do {
		if (Lyrics3Info.LYR[target] == '\r' || Lyrics3Info.LYR[target] == '\n' || (target+1) == lyricsSize ) {
			//â¸çsÇQÉoÉCÉgñ⁄É`ÉFÉbÉN(ÉtÉ@ÉCÉãññîˆÇ≈ÇÕçsÇÌÇ»Ç¢)
			if ((target+1) < lyricsSize) {
				if (Lyrics3Info.LYR[target+1] == '\r' || Lyrics3Info.LYR[target+1] == '\n'){
					target++;
				}
			}
			target++;
			inputSize = lineSize;
			if ( inputSize > readDataSize ) inputSize = readDataSize;
			::memset(buf, 0, sizeof(buf));
			::memcpy(buf, &Lyrics3Info.LYR[startPoint], inputSize);

			buf[readDataSize - 1] = '\0';
			m_liricsLinePointMax[m_lyricsLineNo] = 0;
			result = input_getTimeLyrics(buf, &sec, &lyPoint, &lySize, &nextTagPoint);
			if ( result == -1 ){
				if ( m_lyricsCount > 0 ) {
					//ëOÇÃÉ^ÉCÉÄÉ^ÉOÇÉZÉbÉgÅiëOçsÇÃë±Ç´Åj
					m_lyrics[m_lyricsCount].tagFlag = FALSE;
					m_lyrics[m_lyricsCount].sec = m_lyrics[m_lyricsCount-1].sec;
					::memcpy(&m_lyrics[m_lyricsCount].lyrics, &buf[lyPoint], lySize);
					m_lyrics[m_lyricsCount].lineno = m_lyricsLineNo;
					m_liricsLinePointMax[m_lyricsLineNo]++;
					m_lyrics[m_lyricsCount].point = m_liricsLinePointMax[m_lyricsLineNo];
					m_lyricsCount++;
					m_lyricsLineNo++;
					if ( m_lyricsCount == 1000 ){
						return TRUE;
					}
				}
			} else {
				pbuf = buf;
				for(;;){
					m_lyrics[m_lyricsCount].tagFlag = TRUE;
					m_lyrics[m_lyricsCount].sec = sec;
					::memcpy(&m_lyrics[m_lyricsCount].lyrics, &pbuf[lyPoint], lySize);
					m_lyrics[m_lyricsCount].lineno = m_lyricsLineNo;
					m_liricsLinePointMax[m_lyricsLineNo]++;
					m_lyrics[m_lyricsCount].point = m_liricsLinePointMax[m_lyricsLineNo];
					m_lyricsCount++;
					if ( m_lyricsCount == 1000 ){
						return TRUE;
					}
					if ( result == 0 ) {
						m_lyricsLineNo++;
						break;
					}
					pbuf += nextTagPoint;
					result = input_getTimeLyrics(pbuf, &sec, &lyPoint, &lySize, &nextTagPoint);
				}
			}

			startPoint = target;
			lineSize = 0;
		} else {
			lineSize++;
			target++;
		}
	} while (target < lyricsSize);

	return TRUE;

}

int CMP3Input::input_checkTimeTag(char* tagData, long* msec)
{
	char		mm[3];
	char		ss[3];
	char        ms[4];
	int			dataLength;
	int			target;
	int			lyricsPoint;

	::memset(mm, 0, sizeof(mm));
	::memset(ss, 0, sizeof(ss));
	lyricsPoint = 0;
	target = 0;

	if ( tagData[target] == '\0' ) {
		return -1;
	} else if ( tagData[target] != '[' ) {
		return 0;
	}

	//ï™É`ÉFÉbÉN
	if ( tagData[target + 1] == '\0' ) {
		return -1;
	} else if ( tagData[target + 1] < 0x30 || tagData[target + 1] > 0x39 ) {
		return 0;
	} else if ( tagData[target + 2] == '\0' ) {
		return -1;
	} else if ( tagData[target + 2] == ':' ) {
		dataLength = 1;
	} else if ( tagData[target + 3] == '\0' ) {
		return -1;
	} else if ( tagData[target + 2] >= 0x30 && tagData[target + 2] <= 0x39 && tagData[target + 3] == ':') {
		dataLength = 2;
	} else {
		return 0;
	}
	::memcpy(mm, &tagData[target + 1], dataLength);
	target = 1 + dataLength;

	//ïbÉ`ÉFÉbÉN
	if ( tagData[target + 1] == '\0' ) {
		return -1;
	} else if ( tagData[target + 1] < 0x30 || tagData[target + 1] > 0x39 ) {
		return 0;
	} else if ( tagData[target + 2] == '\0' ) {
		return -1;
	} else if ( tagData[target + 2] == ':' || tagData[target + 2] == ']') {
		dataLength = 1;
	} else if ( tagData[target + 3] == '\0' ) {
		return -1;
	} else if ( tagData[target + 2] >= 0x30 && tagData[target + 2] <= 0x39 
		   && ( tagData[target + 3] == ':' || tagData[target + 3] == ']' )) {
		dataLength = 2;
	} else {
		return 0;
	}
	::memcpy(ss, &tagData[target + 1], dataLength);
	*msec = (atoi(mm) * 60 + atoi(ss)) * 1000;

	target += 1 + dataLength;
	lyricsPoint = target + 1;

	//É~ÉäïbÉ`ÉFÉbÉN
	if ( tagData[target] == ':' ) {
		dataLength = 0;
		if ( tagData[target + 1] == '\0' ) {
			return -1;
		} else if ( tagData[target + 1] < 0x30 || tagData[target + 1] > 0x39 ) {
			return 0;
		} else if ( tagData[target + 2] == '\0' ) {
			return -1;
		} else if ( tagData[target + 2] == ']' ) {
			dataLength = 1;
			lyricsPoint = target + 3;
		} else if ( tagData[target + 2] < 0x30 || tagData[target + 2] > 0x39 ) {
			return 0;
		} else if ( tagData[target + 3] == '\0' ) {
			return -1;
		} else if ( tagData[target + 3] == ']' ) {
			dataLength = 2;
			lyricsPoint = target + 4;
		} else if ( tagData[target + 3] < 0x30 || tagData[target + 3] > 0x39 ) {
			return FALSE;
		} else if ( tagData[target + 4] == '\0' ) {
			return -1;
		} else if ( tagData[target + 4] == ']' ) {
			dataLength = 3;
			lyricsPoint = target + 5;
		} else {
			return 0;
		}
		
		ms[0] = '0';
		ms[1] = '0';
		ms[2] = '0';
		ms[3] = '\0';
		::memcpy(ms, &tagData[target + 1], dataLength);
		*msec = *msec + atoi(ms);
	}
	return lyricsPoint;
}

int CMP3Input::input_getTimeLyrics(char* tagData, long* msec, int* lyPoint, int* lyLen, int* nextPoint)
{
	int			target;
	int			result;
	long		sec;
	char*		lyrics;

	target = 0;
	*msec = 0;
	*lyLen = 0;
	*lyPoint = 0;
	*nextPoint = 0;

	if ( tagData[target] == '\0' ) {
		return -1;
	}
	
	//É^ÉCÉÄÉ^ÉOÉ`ÉFÉbÉN
	result = input_checkTimeTag(&tagData[target], &sec);
	if ( result <= 0 ) {
		for(;;){
			if ( tagData[target] == '\0' ){
				*lyLen = target;
				return -1;
			} else if ( tagData[target] == '[' && m_KaraokeTagFlag ) {
				result = input_checkTimeTag(&tagData[target], &sec);
				if ( result > 0 ) {
					m_ExistKaraokeTag = TRUE;
					*lyLen = target;
					*nextPoint = target;
					return 1;
				}
			}
			target++;
		}
	} else {
		target = 0;
		*lyPoint = result;
		*nextPoint = result;
		lyrics = tagData + result;
		*msec = sec;
		for(;;){
			if ( lyrics[target] == '\0' ){
				*lyLen = target;
				return 0;
			} else if ( lyrics[target] == '['  && m_KaraokeTagFlag ) {
				result = input_checkTimeTag(&lyrics[target], &sec);
				if ( result > 0 ) {
					m_ExistKaraokeTag = TRUE;
					*lyLen = target;
					*nextPoint += target;
					return 1;
				}
			}
			target++;
		}
	}
	return -1;
}

BOOL CMP3Input::input_getLyrics(LYRICS_INFO* pLyricsInfo)
{
	::memset(pLyricsInfo, 0, sizeof(LYRICS_INFO));

	if (m_lyricsCount == 0 || m_decInfo.frequency == 0) {
		m_errNo = ERR_LYRICS_NON_DATA;
		return FALSE;
	}

	//î{ë¨ââëtëŒâû
	double playMs;
	int playSamples;
	int lyricsTotalTime;
	if (m_lyricsTotalFlag == 0){
		lyricsTotalTime = m_winampTotalSec;
	} else {
		lyricsTotalTime = m_totalSec;
	}
	playSamples = output_getCurrentSample();
	playMs = (((float)playSamples + (float)m_firstSample) * (float)1000 / (float)m_decInfo.frequency) / (float)1000;
	playMs = playMs * m_playSpeed;

	//âÃéåëŒâû
	int		lyCnt;
	if ( m_lyricsCount > 0 ){
		for (lyCnt = 0; lyCnt < m_lyricsCount; lyCnt++){
			if ( (m_lyrics[lyCnt].sec) > ((float)playMs) * (float)((float)lyricsTotalTime/(float)m_totalSec) * 1000){
				m_lyricsCurrent = lyCnt - 1;

				//ãÛçsÉJÉbÉg
				while ( !m_lyrics[m_lyricsCurrent].tagFlag ) {
					m_lyricsCurrent--;
					if ( m_lyricsCurrent < 0 ) {
						m_errNo = ERR_LYRICS_NON_DATA;
						return FALSE;
					}
				}

				 if (m_lyrics[m_lyricsCount -1].sec <= m_playSec) m_lyricsCurrent = m_lyricsCount -1;
				 if ( m_lyricsCurrent < 0 ) m_lyricsCurrent = 0;
				break;
			 }
		 }
		 if ( lyCnt == m_lyricsCount ) m_lyricsCurrent = m_lyricsCount -1;
	}

	if ( m_lyricsCurrent < m_lyricsCount ) {
		if (m_lyricsCurrent < m_lyricsCount -2)
			::memcpy(pLyricsInfo->LyricsNext2, m_lyrics[m_lyricsCurrent+2].lyrics,  strlen(m_lyrics[m_lyricsCurrent+2].lyrics));
		if (m_lyricsCurrent < m_lyricsCount -1)
			::memcpy(pLyricsInfo->LyricsNext1, m_lyrics[m_lyricsCurrent+1].lyrics,  strlen(m_lyrics[m_lyricsCurrent+1].lyrics));
		::memcpy(pLyricsInfo->LyricsCurrent, m_lyrics[m_lyricsCurrent].lyrics,  strlen(m_lyrics[m_lyricsCurrent].lyrics));
		if (m_lyricsCurrent > 0)
			::memcpy(pLyricsInfo->LyricsPrev1, m_lyrics[m_lyricsCurrent-1].lyrics,  strlen(m_lyrics[m_lyricsCurrent-1].lyrics));
		if (m_lyricsCurrent > 1)
			::memcpy(pLyricsInfo->LyricsPrev2, m_lyrics[m_lyricsCurrent-2].lyrics,  strlen(m_lyrics[m_lyricsCurrent-2].lyrics));
		pLyricsInfo->sec = m_lyrics[m_lyricsCurrent].sec;
	} else {
		m_errNo = ERR_LYRICS_NON_DATA;
		return FALSE;
	}
	return TRUE;
}

BOOL CMP3Input::input_getLyrics2(LYRICS_INFO2* pLyricsInfo)
{
	::memset(pLyricsInfo, 0, sizeof(LYRICS_INFO));

	if (m_lyricsCount == 0 || m_decInfo.frequency == 0) {
		m_errNo = ERR_LYRICS_NON_DATA;
		return FALSE;
	}

	//î{ë¨ââëtëŒâû
	double playMs;
	int playSamples;
	int lyricsTotalTime;
	if (m_lyricsTotalFlag == 0){
		lyricsTotalTime = m_winampTotalSec;
	} else {
		lyricsTotalTime = m_totalSec;
	}
	playSamples = output_getCurrentSample();
	playMs = (((float)playSamples + (float)m_firstSample) * (float)1000 / (float)m_decInfo.frequency) / (float)1000;
	playMs = playMs * m_playSpeed;

	//âÃéåëŒâû
	int		lyCnt;
	int		target;
	int		curLine;
	int		curPoint;
	long	curSec;
	long	length = 0;
	char	bufNext2[128] = "\0";
	char	bufNext1[128] = "\0";
	char	bufCurrent[128] = "\0";
	char	bufCurrentBegin[128] = "\0";
	char	bufCurrentLyrics[128] = "\0";
	char	bufCurrentAll[128] = "\0";
	char	bufPrev1[128] = "\0";
	char	bufPrev2[128] = "\0";
	//BOOL	found;
	if ( m_lyricsCount > 0 ){
		 for (lyCnt = 0; lyCnt < m_lyricsCount; lyCnt++){
			 //found = FALSE;
			 //if ( m_lyrics[lyCnt].point == 1 ) {
				 //for (lineCnt = 0; lineCnt < m_liricsLinePointMax[m_lyrics[lyCnt].lineno]; lineCnt++) {
					 //target = lyCnt+lineCnt;
					 target = lyCnt;
					 if ( (m_lyrics[target].sec) > ((float)playMs) * (float)((float)lyricsTotalTime/(float)m_totalSec) * 1000){
						m_lyricsCurrent = target - 1;
						while ( !m_lyrics[m_lyricsCurrent].tagFlag ) {
							m_lyricsCurrent--;
							if ( m_lyricsCurrent < 0 ) {
								m_errNo = ERR_LYRICS_NON_DATA;
								return FALSE;
							}
						}

						if (m_lyrics[m_lyricsCount -1].sec <= m_playSec) m_lyricsCurrent = m_lyricsCount -1;
						if ( m_lyricsCurrent < 0 ) m_lyricsCurrent = 0;
						//found = TRUE;
						break;
					 }
				 //}
				 //if ( found ) break;
				 //lyCnt += lineCnt-1;
			 //}
		 }
		 if ( lyCnt == m_lyricsCount ) m_lyricsCurrent = m_lyricsCount -1;
	}
	curLine = m_lyrics[m_lyricsCurrent].lineno;
	curPoint = m_lyrics[m_lyricsCurrent].point;
	curSec = m_lyrics[m_lyricsCurrent].sec;

	StringCchCat(bufCurrent,128, m_lyrics[m_lyricsCurrent].lyrics );
	for(lyCnt = 0; lyCnt < m_lyricsCount; lyCnt++){
		if ( m_lyrics[lyCnt].sec > curSec && length == 0 ){
			length = curSec - m_lyrics[lyCnt].sec;
		}

		if ( m_lyrics[lyCnt].lineno == curLine - 2 ){
			StringCchCat(bufPrev2,128, m_lyrics[lyCnt].lyrics );
		} else
		if ( m_lyrics[lyCnt].lineno == curLine - 1 ){
			StringCchCat(bufPrev1,128, m_lyrics[lyCnt].lyrics );
		} else
		if ( m_lyrics[lyCnt].lineno == curLine ){
			StringCchCat(bufCurrentAll,128, m_lyrics[lyCnt].lyrics );
			if ( m_lyrics[lyCnt].point <= curPoint - 1 ){
				StringCchCat(bufCurrentBegin,128, m_lyrics[lyCnt].lyrics );
			}
			if ( m_lyrics[lyCnt].point <= curPoint){
				StringCchCat(bufCurrentLyrics,128, m_lyrics[lyCnt].lyrics );
			}
		} else
		if ( m_lyrics[lyCnt].lineno == curLine + 1 ){
			StringCchCat(bufNext1,128, m_lyrics[lyCnt].lyrics );
		} else
		if ( m_lyrics[lyCnt].lineno == curLine + 2 ){
			StringCchCat(bufNext2,128, m_lyrics[lyCnt].lyrics );
		}
	}

	if ( m_lyricsCurrent < m_lyricsCount ) {
		::memcpy(pLyricsInfo->LyricsNext2, bufNext2,  strlen(bufNext2));
		::memcpy(pLyricsInfo->LyricsNext1, bufNext1,  strlen(bufNext1));
		::memcpy(pLyricsInfo->LyricsCurrent, bufCurrent,  strlen(bufCurrent));
		::memcpy(pLyricsInfo->LyricsCurrentBegin, bufCurrentBegin,  strlen(bufCurrentBegin));
		::memcpy(pLyricsInfo->LyricsCurrentLyrics, bufCurrentLyrics,  strlen(bufCurrentLyrics));
		::memcpy(pLyricsInfo->LyricsCurrentAll, bufCurrentAll,  strlen(bufCurrentAll));
		::memcpy(pLyricsInfo->LyricsPrev1, bufPrev1,  strlen(bufPrev1));
		::memcpy(pLyricsInfo->LyricsPrev2, bufPrev2,  strlen(bufPrev2));
		pLyricsInfo->sec = m_lyrics[m_lyricsCurrent].sec;
		pLyricsInfo->lineno = curLine;
		pLyricsInfo->point = curPoint;
		pLyricsInfo->length = length;
	} else {
		m_errNo = ERR_LYRICS_NON_DATA;
		return FALSE;
	}

	return TRUE;
}

BOOL CMP3Input::getListInfoEX(HANDLE FileHandle, LIST_INFO_EX* lplistInfo)
{
	RiffChunk	riff;
	CommonChunk	common;
	BOOL		result;
	DWORD		bytesRead;

	//èâä˙âª
	::memset(lplistInfo, 0, sizeof(LIST_INFO_EX));

	//ÉtÉ@ÉCÉãÇÃì™Ç…ñﬂÇËÅARIFFÉ`ÉÉÉìÉNÇì«Ç›çûÇﬁ
	::SetFilePointer(FileHandle, 0, NULL, FILE_BEGIN);
	result = ::ReadFile(FileHandle, &riff, sizeof(riff), &bytesRead, NULL);
	if (!result || bytesRead != sizeof(riff)) {
		return FALSE;
	}
	if (::memcmp(riff.chunkID, "RIFF", 4) != 0) {
		return FALSE;
	}

	//dataÉ`ÉÉÉìÉNÇíTÇ∑
	for (;;) {
		result = ::ReadFile(FileHandle, &common, sizeof(common), &bytesRead, NULL);
		if (!result || bytesRead != sizeof(common)) {
			return FALSE;
		}

		if (::memcmp(common.chunkID, "fmt ", 4) == 0) {
			//Mpegà»äOÇÃÉtÉHÅ[É}ÉbÉgÉ`ÉFÉbÉN
			::ReadFile(FileHandle, &m_waveFormat, sizeof(WAVEFORMATEX), &bytesRead, NULL);
			if (!result || bytesRead != sizeof(WAVEFORMATEX)) {
				return FALSE;
			}
			//ÉtÉHÅ[É}ÉbÉgTagÉ`ÉFÉbÉN
			if (m_waveFormat.wFormatTag != 85 && m_waveFormat.wFormatTag != 80) {
				//ÇªÇÃëºÇÃå`éÆ
				return FALSE; 
			}
			::SetFilePointer(FileHandle, -(long)sizeof(WAVEFORMATEX), NULL, FILE_CURRENT);
		}
		
		if (::memcmp(common.chunkID, "data", 4) == 0) {
			//dataÉ`ÉÉÉìÉNî≠å©
			break;
		}
		else {
			//éüÇÃÉ`ÉÉÉìÉNÇ‹Ç≈ì«Ç›îÚÇŒÇ∑
			::SetFilePointer(FileHandle, common.chunkSize, NULL, FILE_CURRENT);
		}
	}

	//RIFF MP3 LISTÉ`ÉÉÉìÉNÇíTÇ∑
	if ( (common.chunkSize % 2) == 1 ) common.chunkSize++;
	if (::SetFilePointer(FileHandle, common.chunkSize, NULL, FILE_CURRENT) == 0xFFFFFFFF && GetLastError() != NO_ERROR ){
		return TRUE;
	};
	for (;;) {
		result = ::ReadFile(FileHandle, &common, sizeof(common), &bytesRead, NULL);
		if (!result || bytesRead != sizeof(common)) {
			return TRUE;
		}
		if (memcmp(common.chunkID, "LIST", 4) == 0) {
			//LISTÉ`ÉÉÉìÉNî≠å©
			break;
		}
		else {
			//éüÇÃÉ`ÉÉÉìÉNÇ‹Ç≈ì«Ç›îÚÇŒÇ∑
			::SetFilePointer(FileHandle, common.chunkSize, NULL, FILE_CURRENT);
		}
	}
	BYTE* pRMPList = (BYTE*)::malloc(common.chunkSize);
	if (pRMPList == NULL) {
		return TRUE;
	}
	BYTE listID[4];
	result = ::ReadFile(FileHandle, listID, 4, &bytesRead, NULL);
	if (!result || bytesRead != 4) {
		::free(pRMPList);
		return TRUE;
	}
	if (::memcmp(listID, "INFO", 4) != 0) {
		//INFOà»äOÇÕñ≥éã
		::free(pRMPList);
		return TRUE;
	}
	RMPInfo* pRMPInfo = (RMPInfo*)::malloc(sizeof(RMPInfo));
	if (pRMPInfo == NULL) {
		::free(pRMPList);
		return TRUE;
	}
	DWORD remains = common.chunkSize - 4;
	DWORD baseSize = sizeof(RMPInfo) - sizeof(pRMPInfo->data);
	int dumy;
	while (remains >= sizeof(RMPInfo)) {
		result = ::ReadFile(FileHandle, pRMPInfo, baseSize, &bytesRead, NULL);
		if (!result || bytesRead != baseSize) {
			break;
		}
		dumy = 0;
		if ((pRMPInfo->length % 2) == 1 ) dumy = 1;

		pRMPInfo = (RMPInfo*)::realloc(pRMPInfo, baseSize + pRMPInfo->length + dumy);
		if (pRMPInfo == NULL) {
			break;
		}
		result = ::ReadFile(FileHandle, pRMPInfo->data, pRMPInfo->length + dumy, &bytesRead, NULL);
		if (!result || bytesRead != pRMPInfo->length + dumy) {
			break;
		}
		loadRMPInfoEX(lplistInfo, pRMPInfo);
	
		remains -= baseSize + pRMPInfo->length + dumy;
	}
	::free(pRMPList);
	::free(pRMPInfo);
	return TRUE;
}

void CMP3Input::loadRMPInfoEX(LIST_INFO_EX* lplistInfo, RMPInfo* pRMPInfo)
{
	if (CHECK("INAM")) {		//track name
		SET(lplistInfo->INAM, LIST_SIZE);
	}
	else if (CHECK("IART")) {	//artist name
		SET(lplistInfo->IART, LIST_SIZE);
	}
	else if (CHECK("IPRD")) {
		SET(lplistInfo->IPRD, LIST_SIZE);
	}
	else if (CHECK("ICMT")) {
		SET(lplistInfo->ICMT, LIST_SIZE);
	}
	else if (CHECK("ICRD")) {
		SET(lplistInfo->ICRD, LIST_SIZE);
	}
	else if (CHECK("IGNR")) {
		SET(lplistInfo->IGNR, LIST_SIZE);
	}
	else if (CHECK("ICOP")) {
		SET(lplistInfo->ICOP, LIST_SIZE);
	}
	else if (CHECK("IENG")) {
		SET(lplistInfo->IENG, LIST_SIZE);
	}
	else if (CHECK("ISRC")) {
		SET(lplistInfo->ISRC, LIST_SIZE);
	}
	else if (CHECK("ISFT")) {
		SET(lplistInfo->ISFT, LIST_SIZE);
	}
	else if (CHECK("IKEY")) {
		SET(lplistInfo->IKEY, LIST_SIZE);
	}
	else if (CHECK("ITCH")) {
		SET(lplistInfo->ITCH, LIST_SIZE);
	}
	else if (CHECK("ICMS")) {
		SET(lplistInfo->ICMS, LIST_SIZE);
	}
	else if (CHECK("IMED")) {
		SET(lplistInfo->IMED, LIST_SIZE);
	}
	else if (CHECK("ISBJ")) {
		SET(lplistInfo->ISBJ, LIST_SIZE);
	}
	else if (CHECK("IMP3")) {
		SET(lplistInfo->IMP3, LIST_SIZE);
	}
	else if (CHECK("ILYC")) {
		SET(lplistInfo->ILYC, LIST_SIZE);
	}
}

BOOL CMP3Input::setListInfoEX(HANDLE FileHandle, LIST_INFO_EX* pListInfo)
{
	BYTE			buf[400000];
	BOOL			result;
	DWORD			bytesRead;
	DWORD			bytesWrite;
	DWORD			fSync;					//ìØä˙ÉwÉbÉ_ÇÃà íu
	DWORD			fLast;					//ÉfÅ[É^É`ÉÉÉìÉNÇÃç≈å„
	DWORD			fSize;
	DWORD			dSize;
	DWORD			listSize;
	DWORD           target = 0;
	BYTE			tag[128];
	int				tagFlag = 0;
	RiffChunk		riff;
	CommonChunk		common;
	int				lyFlag = 0;
	BYTE*			lyBuf;

	fSync = 0;
	dSize = GetFileSize(FileHandle, NULL);

	//RIFFå`éÆÅAID3 Tag v2ÇÉ`ÉFÉbÉN
	SetFilePointer(FileHandle, 0, NULL, FILE_BEGIN);
	result = ReadFile(FileHandle, buf, 4, &bytesRead, NULL);
	if (memcmp(buf, "RIFF", 4) != 0) {
		::CloseHandle(FileHandle);
		m_errNo = ERR_NON_RIFF;
		return FALSE;
	} else {
		//ÉtÉ@ÉCÉãÇÃì™Ç…ñﬂÇËÅARIFFÉ`ÉÉÉìÉNÇì«Ç›çûÇﬁ
		::SetFilePointer(FileHandle, 0, NULL, FILE_BEGIN);
		result = ::ReadFile(FileHandle, &riff, sizeof(riff), &bytesRead, NULL);
		if (!result || bytesRead != sizeof(riff)) {
			::CloseHandle(FileHandle);
			m_errNo = ERR_MP3_FILE_READ;
			return FALSE;
		}
		fSync = sizeof(riff);
		//dataÉ`ÉÉÉìÉNÇíTÇ∑
		for (;;) {
			result = ::ReadFile(FileHandle, &common, sizeof(common), &bytesRead, NULL);
			if (!result || bytesRead != sizeof(common)) {
				m_errNo = ERR_MP3_FILE_READ;
				::CloseHandle(FileHandle);
				return FALSE;
			}

			if (::memcmp(common.chunkID, "data", 4) == 0) {
				//dataÉ`ÉÉÉìÉNî≠å©
				break;
			}
			else {
				//éüÇÃÉ`ÉÉÉìÉNÇ‹Ç≈ì«Ç›îÚÇŒÇ∑
				fSync = ::SetFilePointer(FileHandle, common.chunkSize, NULL, FILE_CURRENT);
			}
		}

		//É`ÉÉÉìÉNÉTÉCÉYÉ`ÉFÉbÉN
		if (common.chunkSize < dSize) {
			dSize = common.chunkSize;
		}

		fSync += bytesRead;
		fLast = fSync + dSize;
	}

	//ID3Tag v1É`ÉFÉbÉN
	SetFilePointer(FileHandle, -(long)128, NULL, FILE_END);
	result = ReadFile(FileHandle, tag, 128, &bytesRead, NULL);
	if (!result || bytesRead != 128) {
		m_errNo = ERR_MP3_FILE_READ;
		::CloseHandle(FileHandle);
		return FALSE;
	}
	if (memcmp(tag, "TAG", 3) == 0) {
		tagFlag = 1;
	}

	//LYRICS3ë∂ç›É`ÉFÉbÉN
	DWORD lysize = 0;			//é¿ÉTÉCÉY
	DWORD lyfsize;			//ÉtÉBÅ[ÉãÉhÉTÉCÉY
	DWORD lysp = 0;			//ÉXÉ^Å[Égà íu
	if (chkLYRICS3(FileHandle, &lysize)) {
		lyFlag = 1;
		lyfsize = lysize;

		//LYRICS3 äÔêîÉ`ÉFÉbÉN
		if ((lysize % 2) == 1) {
			lyfsize++;
			lysp = 1;
			lyBuf = (BYTE*)::malloc(lyfsize);
			lyBuf[0] = 0x00;
		} else {
			lyBuf = (BYTE*)::malloc(lysize);
		}

		if (lyBuf == NULL){
			m_errNo = ERR_MALLOC;
			::CloseHandle(FileHandle);
			::free(lyBuf);
			return FALSE;
		}

		//É|ÉCÉìÉ^à⁄ìÆ
		if ( tagFlag == 1 ){
			SetFilePointer(FileHandle, -(long)(128+lysize), NULL, FILE_END);
		} else {
			SetFilePointer(FileHandle, -(long)(lysize), NULL, FILE_END);
		}

		result = ReadFile(FileHandle, &lyBuf[lysp], lysize, &bytesRead, NULL);
		if (!result || bytesRead != lysize) {
			m_errNo = ERR_MP3_FILE_READ;
			::CloseHandle(FileHandle);
			::free(lyBuf);
			return FALSE;
		}
		lysize = lyfsize;
	}

	//ÉfÅ[É^ÉTÉCÉYäÔêîëŒçÙ
	if ( (dSize % 2) == 1 ){
		buf[0] = 0x00;
		SetFilePointer(FileHandle, fLast, NULL, FILE_BEGIN);
		result = ::WriteFile(FileHandle, &buf, 1, &bytesWrite, NULL);
		if (!result || bytesWrite != 1) {
			m_errNo = ERR_MP3_FILE_WRITE;
			::CloseHandle(FileHandle);
			if ( lyFlag == 1 ) ::free(lyBuf);
			return FALSE;
		}
		fLast++;
	}

	::memset(&buf, 0, sizeof(buf));

	target = 12;
	listSize = ::strlen(pListInfo->INAM) + 1;
	for (listSize = LIST_SIZE; listSize > 0 && 
		(pListInfo->INAM[listSize - 1] == ' ' || 
		 pListInfo->INAM[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "INAM", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->INAM, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 && 
		(pListInfo->IART[listSize - 1] == ' ' ||
		 pListInfo->IART[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "IART", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->IART, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 &&
		(pListInfo->IPRD[listSize - 1] == ' ' ||
		 pListInfo->IPRD[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "IPRD", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->IPRD, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 &&
		(pListInfo->ICMT[listSize - 1] == ' ' ||
		 pListInfo->ICMT[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ICMT", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->ICMT, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 &&
		(pListInfo->IGNR[listSize - 1] == ' ' ||
		 pListInfo->IGNR[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "IGNR", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->IGNR, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 &&
		(pListInfo->ICRD[listSize - 1] == ' ' ||
		 pListInfo->ICRD[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ICRD", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->ICRD, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 &&
		(pListInfo->ICOP[listSize - 1] == ' ' ||
		 pListInfo->ICOP[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ICOP", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->ICOP, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 &&
		(pListInfo->IENG[listSize - 1] == ' ' ||
		 pListInfo->IENG[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "IENG", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->IENG, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 &&
		(pListInfo->ISRC[listSize - 1] == ' ' ||
		 pListInfo->ISRC[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ISRC", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->ISRC, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 &&
		(pListInfo->ISFT[listSize - 1] == ' ' ||
		 pListInfo->ISFT[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ISFT", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->ISFT, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 &&
		(pListInfo->IKEY[listSize - 1] == ' ' ||
		 pListInfo->IKEY[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "IKEY", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->IKEY, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 &&
		(pListInfo->ITCH[listSize - 1] == ' ' ||
		 pListInfo->ITCH[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ITCH", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->ITCH, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 &&
		(pListInfo->ICMS[listSize - 1] == ' ' ||
		 pListInfo->ICMS[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ICMS", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->ICMS, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 &&
		(pListInfo->IMED[listSize - 1] == ' ' ||
		 pListInfo->IMED[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "IMED", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->IMED, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 &&
		(pListInfo->ISBJ[listSize - 1] == ' ' ||
		 pListInfo->ISBJ[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ISBJ", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->ISBJ, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 &&
		(pListInfo->IMP3[listSize - 1] == ' ' ||
		 pListInfo->IMP3[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "IMP3", 4);
	buf[target + 4] = (BYTE)(listSize & 0x000000ff);
	buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
	buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
	buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
    ::memcpy(&buf[target + 8], pListInfo->IMP3, listSize - 1);
	target += 8 + listSize;
	if ((listSize % 2) == 1 ) target += 1;

	for (listSize = LIST_SIZE; listSize > 0 &&
		(pListInfo->ILYC[listSize - 1] == ' ' ||
		 pListInfo->ILYC[listSize - 1] == '\0'); listSize --) {}
	listSize++;
    ::memcpy(&buf[target], "ILYC", 4);

	if ( lyFlag == 0 || m_Lyrics3InsertPoint == 2 ) {
		buf[target + 4] = (BYTE)(listSize & 0x000000ff);
		buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
		buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
		buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
		::memcpy(&buf[target + 8], pListInfo->ILYC, listSize - 1);
		target += 8 + listSize;
		if ((listSize % 2) == 1 ) target += 1;

		::memcpy(&buf[target], "IID3", 4);
		buf[target + 4] = 0x80;

		target += 8;

		if ( tagFlag == 0 ) {
			int Cnt;
			::memcpy(&buf[target], "TAG", 3);
			for (Cnt = 3; Cnt < 127; Cnt++){
				::memcpy(&buf[target+Cnt], " ", 1);
			}
			buf[target+127] = 0xFA;
		} else {
			::memcpy(&buf[target], tag, 128);
		}

		target += 128;
	} else {
		if ( m_Lyrics3InsertPoint == 1 ) {
			buf[target + 4] = (BYTE)(listSize & 0x000000ff);
			buf[target + 5] = (BYTE)((listSize & 0x0000ffff) >> 8);
			buf[target + 6] = (BYTE)((listSize & 0x00ffffff) >> 16);
			buf[target + 7] = (BYTE)((listSize & 0xffffffff) >> 24);
			::memcpy(&buf[target + 8], pListInfo->ILYC, listSize - 1);
			target += 8 + listSize;
			if ((listSize % 2) == 1 ) target += 1;

			::memcpy(&buf[target], "IID3", 4);
		}

		buf[target + 4] = (BYTE)((lysize + 128) & 0x000000ff);
		buf[target + 5] = (BYTE)(((lysize + 128) & 0x0000ffff) >> 8);
		buf[target + 6] = (BYTE)(((lysize + 128) & 0x00ffffff) >> 16);
		buf[target + 7] = (BYTE)(((lysize + 128) & 0xffffffff) >> 24);

		target += 8 + lysize + 128;
	}

	::memcpy(&buf[0], "LIST", 4);
	buf[4] = (BYTE)((target - 8) & 0x000000ff);
	buf[5] = (BYTE)(((target - 8) & 0x0000ffff) >> 8);
	buf[6] = (BYTE)(((target - 8) & 0x00ffffff) >> 16);
	buf[7] = (BYTE)(((target - 8) & 0xffffffff) >> 24);
    ::memcpy(&buf[8], "INFO", 4);

	SetFilePointer(FileHandle, fLast, NULL, FILE_BEGIN);
	result = ::WriteFile(FileHandle, &buf, target, &bytesWrite, NULL);
	if (!result || bytesWrite != target) {
		m_errNo = ERR_MP3_FILE_WRITE;
		::CloseHandle(FileHandle);
		if ( lyFlag == 1 ) ::free(lyBuf);
		return FALSE;
	}
	::SetEndOfFile(FileHandle);

	if ( lyFlag != 0 && m_Lyrics3InsertPoint != 2 ) {
		SetFilePointer(FileHandle, -(long)(lysize + 128), NULL, FILE_END);
		result = ::WriteFile(FileHandle, lyBuf, lysize, &bytesWrite, NULL);
		if (!result || bytesWrite != lysize) {
			m_errNo = ERR_MP3_FILE_WRITE;
			::CloseHandle(FileHandle);
			if ( lyFlag == 1 ) ::free(lyBuf);
			return FALSE;
		}

		if ( tagFlag == 0 ) {
			int Cnt;
			::memcpy(&buf[0], "TAG", 3);
			for (Cnt = 3; Cnt < 127; Cnt++){
				::memcpy(&buf[target+Cnt], " ", 1);
			}
			buf[127] = 0xFA;
		} else {
			::memcpy(&buf[0], tag, 128);
		}

		result = ::WriteFile(FileHandle, &buf, 128, &bytesWrite, NULL);
		if (!result || bytesWrite != 128) {
			m_errNo = ERR_MP3_FILE_WRITE;
			::CloseHandle(FileHandle);
			if ( lyFlag == 1 ) ::free(lyBuf);
			return FALSE;
		}

		::SetEndOfFile(FileHandle);
	}

	fSize = fLast + target;

	::memset(&buf, 0, sizeof(buf));
	buf[0] = (BYTE)((fSize - 8) & 0x000000ff);
	buf[1] = (BYTE)(((fSize - 8) & 0x0000ffff) >> 8);
	buf[2] = (BYTE)(((fSize - 8) & 0x00ffffff) >> 16);
	buf[3] = (BYTE)(((fSize - 8) & 0xffffffff) >> 24);

	SetFilePointer(FileHandle, 4, NULL, FILE_BEGIN);
	result = ::WriteFile(FileHandle, &buf, 4, &bytesWrite, NULL);
	if (!result || bytesWrite != 4) {
		m_errNo = ERR_MP3_FILE_WRITE;
		::CloseHandle(FileHandle);
		if ( lyFlag == 1 ) ::free(lyBuf);
		return FALSE;
	}

	::CloseHandle(FileHandle);
	if ( lyFlag == 1 ) ::free(lyBuf);
	return TRUE;

}

CMP3Input::CMP3Input()
{
	m_NEqualFrame = 128;
	m_inputBlock = 30;			//ì¸óÕÉtÉåÅ[ÉÄêî
	m_outputBlock_in = 20;		//èoóÕÉtÉåÅ[ÉÄêî
	m_outputBlock_in_tmp = 20;		//èoóÕÉtÉåÅ[ÉÄêî
	m_inputSleep = 5;			//ì¸óÕíºå„ÇÃÉXÉäÅ[Évéûä‘(É~Éäïb)
	m_fadeOutFlag = false;
	callBackFlag = FALSE;		//ÉRÅ[ÉãÉoÉbÉNä÷êîégópóLñ≥
	m_lyricsCount = 0;			//É^ÉOêî
	m_lyricsLineNo = 0;			//âÃéåçsêî
	m_lyricsTotalFlag = 0;		//âÃéåï\é¶éûÇ…égópÇ∑ÇÈéûä‘(0:Winampå›ä∑ 1:ïWèÄéûä‘)
	m_outputBlock_in_wav = 5;		//èoóÕÉtÉåÅ[ÉÄêî
	m_openFlag = FALSE;
	m_playFlag = FALSE;
	m_timeFlag = TRUE;			
	m_seekFlag = FALSE;			
	m_stepFlag = FALSE;
	m_Lyrics3UseFlag = FALSE;	
	m_KaraokeTagFlag = FALSE;	
	m_ExistKaraokeTag = FALSE;
	m_Lyrics3InsertPoint = 0;
	m_blockSize = 1;
	m_BufferRedim = 1;
	m_pitch = 100;				//ÉsÉbÉ`(%)
	m_outputSleep = 0;
	m_lMaxSoftVol = 100;			//ç≈ëÂÉ\ÉtÉgÉEÉFÉAÉ{ÉäÉÖÅ[ÉÄ(ç∂)
	m_rMaxSoftVol = 100;			//ç≈ëÂÉ\ÉtÉgÉEÉFÉAÉ{ÉäÉÖÅ[ÉÄ(âE)
	m_deviceId = WAVE_MAPPER;		//ÉfÉoÉCÉXID
	m_fftWindow = hanning;
	m_lvol = 100;
	m_rvol = 100;
}

CMP3Input::~CMP3Input()
{

}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


void CMP3Input::output_setWaveOutDeviceId(int id){
	m_deviceId = id;
}

void CMP3Input::output_setSoftVolume(int left, int right)
{
	m_lMaxSoftVol = left;
	if ( left > 100 ) m_lMaxSoftVol = 100;
	if ( left < 0 ) m_lMaxSoftVol = 0;

	m_rMaxSoftVol = right;
	if ( right > 100 ) m_rMaxSoftVol = 100;
	if ( right < 0 ) m_rMaxSoftVol = 0;

	m_lSoftVol = m_lMaxSoftVol;
	m_rSoftVol = m_rMaxSoftVol;
}

void CMP3Input::output_getSoftVolume(int* left, int* right)
{
	*left = m_lSoftVol;
	*right = m_rSoftVol;
}

void CMP3Input::output_setStepPitch(int pitch)
{
	m_stepPitch = pitch;
}

int CMP3Input::output_getStepPitch()
{
	return m_stepPitch;
}

void CMP3Input::output_setPitch(int pitch)
{
	m_pitch = pitch;
}

int CMP3Input::output_getPitch()
{
	return m_pitch;
}

/*
void output_startAnalyzeThread()
{
	m_analyzeThread	= true;
}

void output_stopAnalyzeThread()
{
	m_analyzeThread	= false;
}

void output_startAnalyze()
{
	m_analyze		= true;
}

void output_stopAnalyze()
{
	m_analyze		= false;
}
*/

BOOL CMP3Input::output_SetxmMP3Option(xmMP3_OPTION* pxmMP3Option)
{
//	m_waveCount = pxmMP3Option->outputBlock;
	m_outputSleep = pxmMP3Option->outputSleep;
	return TRUE;
}

void CMP3Input::output_GetxmMP3Option(xmMP3_OPTION* pxmMP3Option)
{
//	pxmMP3Option->outputBlock = m_waveCount;
//	pxmMP3Option->outputBlock = 0;
	pxmMP3Option->outputSleep = m_outputSleep;
	return;
}

BOOL CMP3Input::initOutput()
{
	m_hWaveOut		= NULL;
	m_hSemaphore	= NULL;

	m_lSoftVol		= 100;
	m_rSoftVol		= 100;
	m_fadeIn		= false;
	m_fadeOut		= false;
	m_stepPitch		= 1;

	m_waveIndex		= 0;
	m_waveBlock		= 32;
	//m_waveBlock		= 128;
	m_waveTable		= (WAVEHDR*)::malloc(m_waveBlock * sizeof(WAVEHDR));
	::memset(m_waveTable, 0, m_waveBlock * sizeof(WAVEHDR));

	m_outputIndex	= 0;
	m_outputFinished= 0;
	m_outputBlock	= 80;
	//m_outputBlock	= 128;
	m_outputTable	= (BYTE**)::malloc(m_outputBlock * sizeof(BYTE**));
	for (int i = 0; i < m_outputBlock; i ++) {
		m_outputTable[i] = NULL;
	}
	
	InitializeCriticalSection(&m_cs);

	return TRUE;
}

BOOL CMP3Input::freeOutput()
{
	output_stop();
	output_close();
	for (int i = 0; i < m_waveBlock; i ++) {
		WAVEHDR* pWaveHdr = &m_waveTable[i];
		::free(pWaveHdr->lpData);
	}
	::free(m_waveTable);
	for (int i = 0; i < m_outputBlock; i ++) {
		::free(m_outputTable[i]);
	}
	::free(m_outputTable);
	::DeleteCriticalSection(&m_cs);

	return TRUE;
}

//X-MaD 30/08/01

BOOL CMP3Input::SetBufferRedim(int BufferRedimSize)
{
	BOOL Flag = FALSE;
	if (BufferRedimSize < 11 && BufferRedimSize > 0)
	{			
		m_BufferRedim = BufferRedimSize;
		Flag = TRUE;
	}
	return Flag;
}
//X-MaD 30/08/01

BOOL CMP3Input::output_open(const char* pszName, OutputInfo& info)
{
	output_stop();
	output_close();

	//ÉZÉ}ÉtÉHçÏê¨
	m_hSemaphore = ::CreateSemaphore(NULL, m_waveBlock, m_waveBlock, NULL);
	if (m_hSemaphore == NULL) {
		//throw Exception(::GetLastError());
		return FALSE;
	}

	//èoóÕÉfÉoÉCÉXÉIÅ[ÉvÉì
	WAVEFORMATEX wfex;	
	wfex.wFormatTag      = WAVE_FORMAT_PCM;
	wfex.wBitsPerSample  = info.bitsPerSample;
	wfex.nChannels       = info.channels;
	wfex.nSamplesPerSec  = info.frequency * m_pitch / 100;
	wfex.nBlockAlign     = wfex.nChannels * wfex.wBitsPerSample / 8;
	wfex.nAvgBytesPerSec = wfex.nBlockAlign * info.frequency * m_pitch / 100;
	wfex.cbSize          = 0;

	MMRESULT mmresult = ::waveOutOpen(&m_hWaveOut, 
//			WAVE_MAPPER, 
			m_deviceId, 
			&wfex,
			(int)WaveProc, 
			0,
			WAVE_ALLOWSYNC | CALLBACK_FUNCTION);

	if (mmresult != MMSYSERR_NOERROR) {
		::CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;

		//throw MMException(mmresult);
		return FALSE;
	}
	m_bitsPerSample = info.bitsPerSample;
	m_channels = info.channels;
	m_blockSize	= info.bufferSize;
	m_outputSize = 0;

	if ( m_fadeIn ) {
		m_lSoftVol = 0;
		m_rSoftVol = 0;
	} else {
		m_lSoftVol = 100;
		m_rSoftVol = 100;
	}

	m_outputIndex	= 0;
//X-MaD 30/08/01
	m_blockSize = m_blockSize / (11 - m_BufferRedim);
	if (m_blockSize < 5120) {
		m_blockSize = 5120;
	}

//X-MaD 30/08/01
	for (int i = 0; i < m_outputBlock; i ++) {
		m_outputTable[i] = (BYTE*)::realloc(m_outputTable[i], m_blockSize);
		memset(m_outputTable[i], 0, m_blockSize);
	}
	m_outputFinished= 0;
	m_outputBytes	= 0;
	m_outputPtr		= m_outputTable[0];

	m_waveIndex	= 0;
	for (int i = 0; i < m_waveBlock; i ++) {
		WAVEHDR* pWaveHdr = &m_waveTable[i];
		pWaveHdr->lpData = (char*)::realloc(pWaveHdr->lpData, m_blockSize);
		pWaveHdr->dwBufferLength = m_blockSize;
		::waveOutPrepareHeader(m_hWaveOut, pWaveHdr, sizeof(WAVEHDR));
	}

	return TRUE;

}

BOOL CMP3Input::output_close()
{
	if (m_hWaveOut == NULL) {
		return FALSE;
	}
	if (m_outputBytes) {
		int reserve = m_blockSize - m_outputBytes;
		::memset(m_outputPtr, 0, reserve);
		output_playBlock();
	}

	for (int i = 0; i < m_waveBlock; i ++) {
		WAVEHDR* pWaveHdr = &m_waveTable[m_waveIndex ++];
		m_waveIndex %= m_waveBlock;
		while (::waveOutUnprepareHeader(m_hWaveOut,
				pWaveHdr, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING) {
			input_callBackTime();
			input_fadeOutVolume();
			::Sleep(60);
		}
	}

	output_stop();	//âï˙èàóù
	while (::waveOutClose(m_hWaveOut) != MMSYSERR_NOERROR) {
		::Sleep(100);
	}
	::CloseHandle(m_hSemaphore);
	m_hWaveOut = NULL;
	m_hSemaphore = NULL;

	return TRUE;
}

int CMP3Input::output_write(const void* pData, int length)
{
	int reserve = m_blockSize - m_outputBytes;
	//x-mad 03/02/02
	if (reserve < 0) { 
		m_blockSize = m_outputBytes + 1024;
		reserve = 1024;
	}
	//x-mad 03/02/02
	if (reserve <= length) {
		::memcpy(m_outputPtr, pData, reserve);
		output_playBlock();
		length -= reserve;
		if (length >= 0) {
			::memcpy(m_outputPtr, ((BYTE*)pData + reserve), length);
			m_outputPtr += length;
			m_outputBytes += length;
		}
		return m_blockSize;
	}
	else {
		::memcpy(m_outputPtr, pData, length);
		m_outputPtr += length;
		m_outputBytes += length;
		return 0;
	}
}

BOOL CMP3Input::output_stop()
{
	MMRESULT mmresult = ::waveOutReset(m_hWaveOut);
	if (mmresult != MMSYSERR_NOERROR) {
		return FALSE;
	}
	m_outputIndex = 0;
	m_outputFinished = 0;
	m_outputPtr = m_outputTable[0];
	m_outputBytes = 0;
	m_outputSize = 0;

	return TRUE;
}

BOOL CMP3Input::output_pause()
{
	MMRESULT mmresult = ::waveOutPause(m_hWaveOut);
	if (mmresult != MMSYSERR_NOERROR) {
		return FALSE;
	}

	return TRUE;
}

BOOL CMP3Input::output_restart()
{
	MMRESULT mmresult = ::waveOutRestart(m_hWaveOut);
	if (mmresult != MMSYSERR_NOERROR) {
		return FALSE;
	}

	return TRUE;
}

BOOL CMP3Input::output_setVolume(int left, int right)
{
	int maxVol	 = 0xFFFF;//É{ÉäÉÖÅ[ÉÄÇÃç≈ëÂíl
	int leftVol	 = left * maxVol / 100;
	int rightVol = right * maxVol / 100;

	//Ç∑Ç◊ÇƒÇÃèoóÕÉfÉoÉCÉXÇ…É{ÉäÉÖÅ[ÉÄÇê›íË
	MMRESULT mmresult;
/*
	int j = ::waveOutGetNumDevs();
	for (int i = 0; i < j; i ++) {
		mmresult = ::waveOutSetVolume(m_hWaveOut, 
				(DWORD)MAKELONG(leftVol, rightVol));
	}
*/
	if ( m_hWaveOut != NULL ) {
		mmresult = ::waveOutSetVolume(m_hWaveOut, (DWORD)MAKELONG(leftVol, rightVol));
	} else {
//		mmresult = ::waveOutSetVolume((HWAVEOUT)WAVE_MAPPER, (DWORD)MAKELONG(leftVol, rightVol));
		mmresult = ::waveOutSetVolume((HWAVEOUT)m_deviceId, (DWORD)MAKELONG(leftVol, rightVol));
	}

	return TRUE;
}


BOOL CMP3Input::output_getVolume(int* left, int* right)
{
	//Ç∑Ç◊ÇƒÇÃèoóÕÉfÉoÉCÉXÇ…É{ÉäÉÖÅ[ÉÄÇê›íË
	MMRESULT mmresult;
	DWORD  dwVolume;

/*
	int j = ::waveOutGetNumDevs();
	for (int i = 0; i < j; i ++) {
		mmresult = ::waveOutGetVolume(m_hWaveOut, &dwVolume);
	}
*/
	if ( m_hWaveOut != NULL ) {
		mmresult = ::waveOutGetVolume(m_hWaveOut, &dwVolume);
	} else {
//		mmresult = ::waveOutGetVolume((HWAVEOUT)WAVE_MAPPER, &dwVolume);
		mmresult = ::waveOutGetVolume((HWAVEOUT)m_deviceId, &dwVolume);
	}

	UINT maxVol	  = 0xFFFF;//É{ÉäÉÖÅ[ÉÄÇÃç≈ëÂíl
	UINT leftVol  = (UINT)(dwVolume & 0x0000FFFF);
	UINT rightVol = (UINT)(dwVolume >> 16);
	
	*left = leftVol * 100 / maxVol;
	*right = rightVol * 100 / maxVol;

	return TRUE;
}

int CMP3Input::output_getWaveOutSupport()
{
    MMRESULT mmresult;
    WAVEOUTCAPS woc;

//    mmresult = waveOutGetDevCaps(WAVE_MAPPER, &woc, sizeof(woc));
    mmresult = waveOutGetDevCaps(m_deviceId, &woc, sizeof(woc));
	input_setDebug(woc.dwSupport);
    if (mmresult != 0) { return 0; }
    return woc.dwSupport;
}

long CMP3Input::output_getCurrentSample()
{
	MMRESULT mmresult;
	MMTIME mmtime;
	long Sample = 0;
	
	mmtime.wType = TIME_SAMPLES;
	//mmtime.wType = TIME_BYTES;
	mmresult = ::waveOutGetPosition(m_hWaveOut, &mmtime, sizeof(mmtime));
	if (mmresult != MMSYSERR_NOERROR) {
		return 0;
	}
	Sample = mmtime.u.sample * m_stepPitch;
	//Sample = mmtime.u.cb * m_stepPitch;
	return Sample;
}




void CMP3Input::output_setFftWindow(int window)
{
	if ( window == rectangle ){
		m_fftWindow = rectangle;
	} else if ( window == hamming ) {
		m_fftWindow = hamming;
	} else if ( window == blackman ) {
		m_fftWindow = blackman;
	} else {
		m_fftWindow = hanning;
	}
}

void CMP3Input::fftCopy(int pos, short s)
{
	if ( m_fftWindow == rectangle ){
		m_fft_rectangle.copy(pos, s);
	} else if ( m_fftWindow == hamming ) {
		m_fft_hamming.copy(pos, s);
	} else if ( m_fftWindow == blackman ) {
		m_fft_blackman.copy(pos, s);
	} else {
		m_fft_hanning.copy(pos, s);
	}
}

void CMP3Input::fftTransform(int* spec)
{
	if ( m_fftWindow == rectangle ){
		m_fft_rectangle.transform(spec);
	} else if ( m_fftWindow == hamming ) {
		m_fft_hamming.transform(spec);
	} else if ( m_fftWindow == blackman ) {
		m_fft_blackman.transform(spec);
	} else {
		m_fft_hanning.transform(spec);
	}
}

int CMP3Input::output_getWave(int sample, int* pWaveL, int* pWaveR)
{
	BYTE waveBuf[WAVE_BAND * sizeof(short) * 2];
	int waveBytes = WAVE_BAND * sizeof(short) * m_channels;
	//sample = 267841652;
	int bytes = output_sampleToBytes(sample);
	int minBytes, maxBytes, lost;
	int index, pos;

	::EnterCriticalSection(&m_cs);
	lost = m_outputFinished - m_outputBlock;
	if (lost < 0) {
		minBytes = waveBytes;
		maxBytes = minBytes + m_outputIndex * m_blockSize - waveBytes;
		if (bytes < minBytes || bytes > maxBytes) {
			::LeaveCriticalSection(&m_cs);
			::memset(pWaveL, 0, WAVE_BAND*sizeof(int));
			::memset(pWaveR, 0, WAVE_BAND*sizeof(int));
			return 0;
		}
		index = (bytes / m_blockSize) % m_outputBlock;
		pos = bytes % m_blockSize;
	}
	else {
		minBytes = lost * m_blockSize;
		maxBytes = minBytes + m_outputBlock * m_blockSize - waveBytes;
		if (bytes < minBytes) {
			::LeaveCriticalSection(&m_cs);
			::memset(pWaveL, 0, WAVE_BAND*sizeof(int));
			::memset(pWaveR, 0, WAVE_BAND*sizeof(int));
			return 0;
		}
		if (bytes > maxBytes) {
			bytes = maxBytes;
		}
		bytes -= minBytes;
		int add = bytes / m_blockSize;
		index = (m_outputIndex + add) % m_outputBlock;
		pos = bytes % m_blockSize;
	}
	BYTE* p1 = m_outputTable[index];
	int actual = m_blockSize - pos;

	if (actual < waveBytes) {
		memcpy(waveBuf, &p1[pos], actual);
		index = (index + 1) % m_outputBlock;
		p1 = m_outputTable[index];
		memcpy(&waveBuf[actual], p1, waveBytes - actual);
	}
	else {
		memcpy(waveBuf, &p1[pos], waveBytes);
	}
	::LeaveCriticalSection(&m_cs);

	short* p2 = (short*)waveBuf;
	for (int i = 0; i < WAVE_BAND; i ++) {
		if ( m_bitsPerSample == 8 && *p2 != 0 ) *p2 = *p2 * 256 - 32768;	//8bit
		pWaveL[i] = *p2 * 100 / 32767;
		//pWaveL[i] = *p2;
		p2 += m_channels;
	}

	if (m_channels == 2) {
		p2 = (short*)waveBuf;
		p2 ++;
		for (int i = 0; i < WAVE_BAND; i ++) {
			if ( m_bitsPerSample == 8 && *p2 != 0 ) *p2 = *p2 * 256 - 32768;	//8bit
			pWaveR[i] = *p2 * 100 / 32767;
			//pWaveR[i] = *p2;
			p2 += m_channels;
		}
	}
	else {
		::memset(pWaveR, 0, WAVE_BAND*sizeof(int));
	}
	return output_bytesToSample(waveBytes);
}

int CMP3Input::output_getSpectrum(int sample, int* pSpecL, int* pSpecR)
{
	BYTE fftBuf[FFT_SIZE * sizeof(short) * 2];
	int fftBytes = FFT_SIZE * sizeof(short) * m_channels;
	int bytes = output_sampleToBytes(sample);
	int minBytes, maxBytes, lost;
	int index, pos;

	::EnterCriticalSection(&m_cs);
	lost = m_outputFinished - m_outputBlock;
	if (lost < 0) {
		minBytes = fftBytes;
		maxBytes = minBytes + m_outputIndex * m_blockSize - fftBytes;
		if (bytes < minBytes || bytes > maxBytes) {
			::LeaveCriticalSection(&m_cs);
			::memset(pSpecL, 0, FFT_BAND*sizeof(int));
			::memset(pSpecR, 0, FFT_BAND*sizeof(int));
			return 0;
		}
		index = (bytes / m_blockSize) % m_outputBlock;
		pos = bytes % m_blockSize;
	}
	else {
		minBytes = lost * m_blockSize;
		maxBytes = minBytes + m_outputBlock * m_blockSize - fftBytes;
		if (bytes < minBytes) {
			::LeaveCriticalSection(&m_cs);
			::memset(pSpecL, 0, FFT_BAND*sizeof(int));
			::memset(pSpecR, 0, FFT_BAND*sizeof(int));
			return 0;
		}
		if (bytes > maxBytes) {
			bytes = maxBytes;
		}
		bytes -= minBytes;
		int add = bytes / m_blockSize;
		index = (m_outputIndex + add) % m_outputBlock;
		pos = bytes % m_blockSize;
	}
	
	BYTE* p1 = m_outputTable[index];
	int actual = m_blockSize - pos;

	if (actual < fftBytes) {
		memcpy(fftBuf, &p1[pos], actual);
		index = (index + 1) % m_outputBlock;
		p1 = m_outputTable[index];
		memcpy(&fftBuf[actual], p1, fftBytes - actual);
	}
	else {
		memcpy(fftBuf, &p1[pos], fftBytes);
	}
	::LeaveCriticalSection(&m_cs);

	short* p2 = (short*)fftBuf;
	for (int i = 0; i < FFT_SIZE; i ++) {
		if ( m_bitsPerSample == 8 && *p2 != 0 ) *p2 = *p2 * 256 - 32768;	//8bit
		//m_fft.copy(i, *p2);
		fftCopy(i, *p2);
		p2 += m_channels;
	}
	//m_fft.transform(pSpecL);
	fftTransform(pSpecL);

	if (m_channels == 2) {
		p2 = (short*)fftBuf;
		p2 ++;
		for (int i = 0; i < FFT_SIZE; i ++) {
			if ( m_bitsPerSample == 8 && *p2 != 0 ) *p2 = *p2 * 256 - 32768;	//8bit
			//m_fft.copy(i, *p2);
			fftCopy(i, *p2);
			p2 += m_channels;
		}
		//m_fft.transform(pSpecR);
		fftTransform(pSpecR);
	}
	else {
		::memset(pSpecR, 0, FFT_BAND*sizeof(int));
	}
	return output_bytesToSample(fftBytes);
}

int CMP3Input::output_bytesToSample(int bytes)
{
	if ( bytes > ALLOW_DATA_SIZE ) {
		return (bytes / m_bitsPerSample / m_channels * 8);
	} else {
		return (bytes * 8 / m_bitsPerSample / m_channels);
	}
}

int CMP3Input::output_sampleToBytes(int sample)
{
	unsigned long Temp = 0;
	Temp = sample * m_bitsPerSample;
	Temp = Temp * m_channels;
	return (int)(Temp / 8);
}

void CMP3Input::output_playBlock()
{
	BOOL fadeOutFlg = FALSE;
	::WaitForSingleObject(m_hSemaphore, INFINITE);
	WAVEHDR* pWaveHdr = &m_waveTable[m_waveIndex];

	::memcpy(pWaveHdr->lpData, m_outputTable[m_outputIndex], m_blockSize);
	m_outputSize = m_outputSize + pWaveHdr->dwBufferLength;
	//ÉtÉFÅ[ÉhÉAÉEÉg
	if ( m_fadeOut ) {
		int outDecSize = m_outputSize + input_getDecodeOutputSize();
		int outTolSize = input_getTotalOutputSize();
		UINT outRemSize = outTolSize - outDecSize;
		if ( outRemSize <  (pWaveHdr->dwBufferLength * 23) ){
			fadeOutFlg = TRUE;
			if ( m_lSoftVol > 50 ) {
				m_lSoftVol = m_lSoftVol - 10;
			} else if ( m_lSoftVol > 10 ) {
				m_lSoftVol = m_lSoftVol - 5;
			} else if ( m_lSoftVol > 2 ) {
				m_lSoftVol = m_lSoftVol - 2;
			} else if ( m_lSoftVol > 0 ) {
				m_lSoftVol = m_lSoftVol--;
			}
			if ( m_lSoftVol < 0 ) m_lSoftVol = 0;

			if ( m_rSoftVol > 50 ) {
				m_rSoftVol = m_rSoftVol - 10;
			} else if ( m_rSoftVol > 10 ) {
				m_rSoftVol = m_rSoftVol - 5;
			} else if ( m_rSoftVol > 2 ) {
				m_rSoftVol = m_rSoftVol - 2;
			} else if ( m_rSoftVol > 0 ) {
				m_rSoftVol = m_rSoftVol--;
			}
			if ( m_rSoftVol < 0 ) m_rSoftVol = 0;
		}
	}

	//ÉtÉFÅ[ÉhÉCÉì
	if ( m_fadeIn && fadeOutFlg == FALSE ) {
		int outStart = input_getDecodeOutputSize();
		if ( outStart == 0 ) {
			if ( m_lSoftVol < 10 ) {
				m_lSoftVol++;
			} else if ( m_lSoftVol < 50 ) {
				m_lSoftVol = m_lSoftVol + 5;
			} else if ( m_lSoftVol < 100 ) {
				m_lSoftVol = m_lSoftVol + 10;
			}
			if ( m_lSoftVol > m_lMaxSoftVol ) m_lSoftVol = m_lMaxSoftVol;

			if ( m_rSoftVol < 10 ) {
				m_rSoftVol++;
			} else if ( m_rSoftVol < 50 ) {
				m_rSoftVol = m_rSoftVol + 5;
			} else if ( m_rSoftVol < 100 ) {
				m_rSoftVol = m_rSoftVol + 10;
			}
			if ( m_rSoftVol > m_rMaxSoftVol ) m_rSoftVol = m_rMaxSoftVol;
		}
	}

	if ( m_lSoftVol != 100 && m_rSoftVol != 100 ) {
		if ( m_bitsPerSample == 16 ){
			output_chgSoftVol( (short*) pWaveHdr->lpData, pWaveHdr->dwBufferLength );
		} else {
			output_chgSoftVol8( (unsigned char*) pWaveHdr->lpData, pWaveHdr->dwBufferLength );
		}
	}

	//output_chgStepSpeed( (short*) pWaveHdr->lpData, pWaveHdr->dwBufferLength );
	if ( m_stepPitch == 1 ) {
		::waveOutWrite(m_hWaveOut, pWaveHdr, sizeof(WAVEHDR));
	} else {
		//WinNT Ç≈ñ‚ëËÇ†ÇË
		int tmpSize = pWaveHdr->dwBufferLength;
		pWaveHdr->dwBufferLength = pWaveHdr->dwBufferLength / m_stepPitch;
		::waveOutWrite(m_hWaveOut, pWaveHdr, sizeof(WAVEHDR));
		pWaveHdr->dwBufferLength = tmpSize;
	}

	m_waveIndex = (m_waveIndex + 1) % m_waveBlock;

	EnterCriticalSection(&m_cs);
	m_outputFinished ++;
	m_outputIndex = (m_outputIndex + 1) % m_outputBlock;
	m_outputPtr = m_outputTable[m_outputIndex];
	m_outputBytes = 0;
	::LeaveCriticalSection(&m_cs);
	if (m_outputFinished % 4 == 0) Sleep(m_outputSleep);
}

//ÉRÅ[ÉãÉoÉbÉNä÷êî
void CALLBACK CMP3Input::WaveProc(HWAVEOUT hWaveOut, UINT msg,
		DWORD instance, DWORD param1, DWORD param2)
{
	switch (msg) {
	case WOM_DONE:
		{
			ReleaseSemaphore(m_hSemaphore, 1, NULL);
		}
		break;
	}
}


//WAVEëÄçÏ
void CMP3Input::output_softVolumeMax()
{
	m_lSoftVol = m_lMaxSoftVol;
	m_rSoftVol = m_rMaxSoftVol;
}

void CMP3Input::output_setFadeIn(int on)
{
	if ( on == 0 ) {
		m_fadeIn = false;
		m_lSoftVol = m_lMaxSoftVol;
		m_rSoftVol = m_rMaxSoftVol;
	} else {
		m_fadeIn = true;
	}
}

void CMP3Input::output_setFadeOut(int on)
{
	if ( on == 0 ) {
		m_fadeOut = false;
		m_lSoftVol = m_lMaxSoftVol;
		m_rSoftVol = m_rMaxSoftVol;
	} else {
		m_fadeOut = true;
	}
}

void CMP3Input::output_chgSoftVol( short *buf, int size )
{
	int i,tmp;

	for( i = 0; i < (int)(size / sizeof(short)); i++ ) {
		if ( m_channels == 2 && (i % 2) == 1) {
			tmp = (int)*buf * m_rSoftVol / 100;
		} else {
			tmp = (int)*buf * m_lSoftVol / 100;
		}
		if ( tmp > 32767 ) {
			*buf = 32767;
		} else if ( tmp < -32768 ) {
			*buf = -32768;
		} else {
			*buf = (short)tmp;
		}
		buf++;
	}
}

void CMP3Input::output_chgSoftVol8( unsigned char *buf, int size )
{
	int i,tmp;

	for( i = 0; i < size; i++ )	{
		if ( m_channels == 2 && (i % 2) == 1) {
			tmp = (int)*buf * m_rSoftVol / 100;
		} else {
			tmp = (int)*buf * m_lSoftVol / 100;
		}
		tmp = tmp - 128;
		if ( tmp > 255 ) {
			*buf = 255;
		} else if ( tmp < 0 ) {
			*buf = 0;
		} else {
			*buf = (unsigned char)tmp;
		}
		buf++;
	}
}


//ÉeÉXÉgíÜ
void CMP3Input::output_chgStepSpeed( short *buf, int size )
{
	int i;

	if ( m_channels == 2 ) {
		short *tmp1 = buf;
		short *tmp2 = buf;
		tmp1 = buf+2*(int) (size / (sizeof(short) * 2)/4);
		tmp2 = buf+3*(int) (size / (sizeof(short) * 2)/4);
		for( i = 0; i < (int) (size / (sizeof(short) * 2)/4); i++ )
		{
			*tmp1 = *tmp2;
			tmp1++;
			tmp2++;
			*tmp1 = *tmp2;
			tmp1++;
			tmp2++;
		}		
	}
}

