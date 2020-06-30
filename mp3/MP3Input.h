// MP3Input.h: interface for the CMP3Input class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MP3INPUT_H__8613D81E_3FF4_4CCE_A5CA_762E30A4386B__INCLUDED_)
#define AFX_MP3INPUT_H__8613D81E_3FF4_4CCE_A5CA_762E30A4386B__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include <windows.h>
#include <process.h>	//_beginthreadex, _endthreadex
#include <string.h>		//memmove
#include <stdio.h>		//fgets
#include "mp3dec.h"
#include <Mmreg.h>
#include <Mmsystem.h>

#define DEFAULT_BUFFER_SIZE	4096
#define INFO_NAME_SIZE		128
#define INPUT_MSG_ERROR			0
#define INPUT_MSG_STOP			1
#define INPUT_MSG_PLAY			2
#define INPUT_MSG_PAUSE			3
#define INPUT_MSG_PLAYDONE		4
#define INPUT_MSG_BITRATE		5
#define INPUT_MSG_WAVE_LEFT		6
#define INPUT_MSG_WAVE_RIGHT	7
#define LIST_SIZE		2048
#define ALLOW_DATA_SIZE		268435455

#define IDC_POS 230
#define IDC_FINFICHIER 240

typedef void (__stdcall *INPUT_MSG_PROC)(int msg, int param);

class Exception
{
public:
	int		code;

	Exception(int c) : code(c) {}
};

class MMException
{
public:
	int		code;

	MMException(int c) : code(c) {}
};

class CMP3Input  
{

public:

	CMP3Input();
	virtual ~CMP3Input();

	struct ID3Tagv1 {
		BYTE	tag[3];
		BYTE	trackName[30];
		BYTE	artistName[30];
		BYTE	albumName[30];
		BYTE	year[4];
		BYTE	comment[30];
		BYTE	genre;
	};//128�o�C�g

	struct ID3Tagv2 {
		BYTE	id3[3];
		BYTE	version;
		BYTE	revision;
		BYTE	flags;
		BYTE	size[4];
	};

	struct RiffChunk {
		BYTE	chunkID[4];		//'RIFF'
		DWORD	chunkSize;
		BYTE	formType[4];	//'WAVE','RMP3'
	};

	struct CommonChunk {
		BYTE	chunkID[4];
		DWORD	chunkSize;	
	};

	struct RMPInfo {
		BYTE	infoID[4];
		DWORD	length;
		BYTE	data[1];
	};


	struct InputInfo
	{
		char	szTrackName[INFO_NAME_SIZE];	//�Ȗ�
		char	szArtistName[INFO_NAME_SIZE];	//�A�[�e�B�X�g��
		int		channels;		//�`�����l����
		int		bitRate;		//�r�b�g���[�g
		int		samplingRate;	//�T���v�����O���[�g
		int		totalSec;		//�����t����
	};


	struct TAG_INFO
	{
		char	szTrackName[INFO_NAME_SIZE];	//�Ȗ�
		char	szArtistName[INFO_NAME_SIZE];	//�A�[�e�B�X�g��
		char    szAlbumName[INFO_NAME_SIZE];	//�A���o����
		char	szYear[5];						//�����[�X�N��
		char	szComment[INFO_NAME_SIZE];		//�R�����g������
		int		genre;							//�W������
		char    szGanreName[INFO_NAME_SIZE];	//�W����������
	};

	struct TAG_INFO_11
	{
		char	szTrackName[INFO_NAME_SIZE];	//�Ȗ�
		char	szArtistName[INFO_NAME_SIZE];	//�A�[�e�B�X�g��
		char    szAlbumName[INFO_NAME_SIZE];	//�A���o����
		char	szYear[5];			//�����[�X�N��
		char	szComment[INFO_NAME_SIZE];		//�R�����g������
		int		genre;				//�W������
		char    szGanreName[INFO_NAME_SIZE];	//�W����������
		int		trackNo;			//�g���b�N�ԍ� -1 = v1.0
	};

	struct MPEG_INFO
	{
		int		version;		//MPEG�̃o�[�W����
		int		layer;			//MPEG�̃��C��
		int		crcDisable;		//�G���[�ی�
		int		extension;		//�l���
		int		mode;			//�`�����l�����[�h
		int		copyright;		//���쌠
		int		original;		//�I���W�i��
		int		emphasis;		//�G���t�H�V�X		

		int		channels;		//�o�̓`�����l����
		int		bitRate;		//�r�b�g���[�g
		int		samplingRate;	//�T���v�����O���[�g
		int		fileSize;		//�t�@�C���T�C�Y
		int		flames;			//�t���[����
		int		totalSec;		//�������t����
	};

	struct LYRICS_WORK
	{
		long	sec;
		int		lineno;
		int		point;
		BOOL	tagFlag;
		char	lyrics[INFO_NAME_SIZE];
	};

	struct LYRICS_INFO
	{
		long	sec;
		char	LyricsNext2[INFO_NAME_SIZE];
		char	LyricsNext1[INFO_NAME_SIZE];
		char	LyricsCurrent[INFO_NAME_SIZE];
		char	LyricsPrev1[INFO_NAME_SIZE];
		char	LyricsPrev2[INFO_NAME_SIZE];
	};

	struct LYRICS_INFO2
	{
		long	sec;
		int		lineno;
		int		point;
		int		length;
		char	LyricsNext2[INFO_NAME_SIZE];
		char	LyricsNext1[INFO_NAME_SIZE];
		char	LyricsCurrent[INFO_NAME_SIZE];
		char	LyricsCurrentBegin[INFO_NAME_SIZE];
		char	LyricsCurrentLyrics[INFO_NAME_SIZE];
		char	LyricsCurrentAll[INFO_NAME_SIZE];
		char	LyricsPrev1[INFO_NAME_SIZE];
		char	LyricsPrev2[INFO_NAME_SIZE];
	};

	struct DEC_OPTION{
		int		reduction;		//�T���v�����O 0:1/1 1:1/2 2:1/4 (Default = 0)
		int		convert;		//�`�����l�� 0:�X�e���I 1:���m����(Default = 0)
		int		freqLimit;		//���g��
	};

	struct xmMP3_OPTION
	{
		int inputBlock;			//���̓t���[����
		int outputBlock;		//�o�̓t���[����
		int inputSleep;			//���͒���̃X���[�v����(�~���b)
		int outputSleep;		//�o�͒���̃X���[�v����(�~���b)
	};

	struct OutputInfo
	{
		int		channels;			//�`���l����
		int		bitsPerSample;		//�r�b�g��/1�T���v��
		int		frequency;
		int		bufferSize;
	};


	struct LIST_INFO
	{
		char	INAM[INFO_NAME_SIZE];	//�Ȗ�
		char	IART[INFO_NAME_SIZE];	//�A�[�e�B�X�g��
		char    IPRD[INFO_NAME_SIZE];	//���i��
		char	ICMT[INFO_NAME_SIZE];	//�R�����g������
		char	ICRD[INFO_NAME_SIZE];	//�����[�X�N��
		char	IGNR[INFO_NAME_SIZE];	//�W��������
		char    ICOP[INFO_NAME_SIZE];	//���쌠
		char    IENG[INFO_NAME_SIZE];	//�G���W�j�A
		char    ISRC[INFO_NAME_SIZE];	//�\�[�X
		char    ISFT[INFO_NAME_SIZE];	//�\�t�g�E�F�A
		char    IKEY[INFO_NAME_SIZE];	//�L�[���[�h
		char    ITCH[INFO_NAME_SIZE];	//�Z�p��
		char    ILYC[INFO_NAME_SIZE];	//�̎�
		char    ICMS[INFO_NAME_SIZE];	//�R�~�b�V����
	};


	struct LIST_INFO_EX
	{
		char	INAM[LIST_SIZE];	//�Ȗ�
		char	IART[LIST_SIZE];	//�A�[�e�B�X�g��
		char    IPRD[LIST_SIZE];	//���i��
		char	ICMT[LIST_SIZE];	//�R�����g������
		char	ICRD[LIST_SIZE];	//�����[�X�N��
		char	IGNR[LIST_SIZE];	//�W��������
		char    ICOP[LIST_SIZE];	//���쌠
		char    IENG[LIST_SIZE];	//�G���W�j�A
		char    ISRC[LIST_SIZE];	//�\�[�X
		char    ISFT[LIST_SIZE];	//�\�t�g�E�F�A
		char    IKEY[LIST_SIZE];	//�L�[���[�h
		char    ITCH[LIST_SIZE];	//�Z�p��
		char    ICMS[LIST_SIZE];	//�R�~�b�V����
		char	IMED[LIST_SIZE];	//���� 
		char	ISBJ[LIST_SIZE];	//�^�C�g��
		char	IMP3[LIST_SIZE];	//MP3���
		char    ILYC[LIST_SIZE];	//�̎�
	};

	struct WAVE_DATA
	{
		int		channels;
		int		bitsPerSample;
		int		left;
		int		right;
	};

	struct LYRICS3_INFO
	{
		char	IND_LYR;		//LYR�t�B�[���h�̗L�� '0':���� '1':�L��
		char	IND_TIMETAG;	//�^�C���^�O�̗L�� '0':���� '1':�L��
		char	LYR[60000];		//�̎��t�B�[���h
		char	INF[60000];		//���t�B�[���h
		char	AUT[250];		//�쎌�^��ȎҖ�
		char	EAL[250];		//�g���A���o����
		char	EAR[250];		//�g���A�[�e�B�X�g��
		char	ETT[250];		//�g���g���b�N�^�C�g��
		char	IMG[60000];		//�C���[�W�t�@�C���ւ̃����N
	};




	enum {
		rectangle	= 0,
		hanning		= 1,
		hamming		= 2,
		blackman	= 3,
	};

	//�f�R�[�h�X���b�h�p���b�Z�[�W
	enum {
		EV_STOP		= (WM_APP + 0),
		EV_PAUSE	= (WM_APP + 2),
		EV_RESTART	= (WM_APP + 3),
		EV_SEEK		= (WM_APP + 4),
		EV_EQUALIZER= (WM_APP + 5),
	};

	//���݂̏��
	//getState�֐��Ŏ擾�ł���
	enum {
		STATE_STOP	= 0,
		STATE_PLAY	= 1,
		STATE_PAUSE	= 2,
		STATE_SEEK	= 3,
	};


	//�G���[�X�e�[�^�X
	enum {
		ERR_MP3_FILE_OPEN	= 1,
		ERR_MP3_FILE_NOT_OPEN	= 2,
		ERR_MP3_FILE_READ	= 3,
		ERR_MP3_FILE_WRITE	= 4,
		ERR_WAV_FILE_OPEN	= 5,
		ERR_WAV_FORMAT	= 6,
		ERR_ENCODE_FILE_OPEN	= 7,
		ERR_LYRICS_FILE_OPEN	= 8,
		ERR_LYRICS_NON_DATA	= 9,
		ERR_FRAME_HEADER_NOT_FOUND = 10,
		ERR_FRAME_HEADER_READ = 11,
		ERR_STATE_STOP	= 12,
		ERR_NOT_STATE_STOP	= 13,
		ERR_NOT_STATE_PLAY	= 14,
		ERR_STATE_NON_ENCODE = 15,
		ERR_PLAY = 16,
		ERR_STOP = 17,
		ERR_INVALID_VALUE	= 18,
		ERR_MALLOC	= 19,
		ERR_NON_RIFF = 20,
		ERR_RIFF = 21,
		ERR_NOT_MP3 = 22,
		ERR_MAC_BIN = 23,
		ERR_UNKNOWN_FILE = 24,
		ERR_OPEN_OUT_DEVICE = 25,
		ERR_DECODE = 26,
		ERR_DECODE_THREAD = 27,
		ERR_ENCODE_THREAD = 28,
		ERR_CREATE_EVENT = 29,
		ERR_CODEC_NOT_FOUND = 30,
		ERR_WAVE_TABLE_NOT_FOUND = 31,
		ERR_ACM_OPEN = 32,
		ERR_PLATFORM_NT = 33,
	};

	BOOL SetReadHaedA(int NEqualFrame);

	


	//MP3Input

	void input_setWaveOutDeviceId(int id);
	//LYRICS3
	void input_setWindow(const HWND &hWnd);

	void input_setLyricsTime(int flag);
	BOOL input_clearLyrics();
	BOOL input_readLyrics3Data();
	void input_setLyrics3Use(int useLyrics3);
	BOOL input_readLyrics3(HANDLE FileHandle);
	BOOL input_getLyrics3Data(HANDLE hFileHandle, LYRICS3_INFO* lpLyrics3Info);
	BOOL input_getFileLyrics3Info(const char* pszName, LYRICS3_INFO* lpLyrics3Info);
	BOOL chkLYRICS3(HANDLE FileHandle, DWORD* dSize);
	void input_setLyrics3InsField(int field);
	BOOL input_setFileLyrics3Info(const char* pszName, LYRICS3_INFO* lpLyrics3Info, int setFlag);
	int input_checkTimeTag(char* tagData, long* msec);
	int input_getTimeLyrics(char* tagData, long* msec, int* lyPoint, int* lyLen, int* nextPoint);
	BOOL input_getLyrics2(LYRICS_INFO2* pLyricsInfo);
	BOOL input_getListInfoEX(const char* pszName, LIST_INFO_EX* lpListInfo);
	BOOL input_setListInfoEX(const char* pszName, LIST_INFO_EX* lpListInfo);
	BOOL getFileTagInfoEX(const char* pszName, TAG_INFO_11* pTagInfo);
	BOOL setTagInfo11(const char* pszName, TAG_INFO_11* pTagInfo,int tagSet, int tagAdd);
	void input_setKaraokeUse(int useKaraoke);
	void input_setFftWindow(int window);
	BOOL input_playDecodeWave(const char* pszWaveName);
	int input_getSilentFrames(const char* pszName);
	int input_getWaveOutSupport();
	void input_setSoftVolume(int left, int right);
	void input_getSoftVolume(int* left, int* right);
	void input_setOverTime(int on);
	void input_setErrNo(int no);
	static int input_getDecodeOutputSize();
	static int input_getTotalOutputSize();
	void input_fadeOut();
	void input_setFadeIn(int on);
	void input_setFadeOut(int on);
	static void input_fadeOutVolume();
	BOOL input_reload();
	BOOL input_setStepPitch(int pitch, int frames);
	int input_getStepPitch();
	BOOL input_setPitch(int pitch);
	int input_getPitch();
	static void input_callBackTime();
	int input_getPlayFlames();
	long input_getPlaySamples();
	static int input_getTotalSamples();
	void input_startAnalyze();
	void input_stopAnalyze();
	void input_startAnalyzeThread();
	void input_stopAnalyzeThread();
	BOOL input_decodeWave(const char* pszWaveName);
	long input_getWinampPlayMs();
	int input_getWinampTotalSec();
	int input_getPlayBitRate();
	int getFileType(const char* pszName);
	BOOL input_cutMacBinary(const char* pszName);
	BOOL input_changeMp3(const char* pszName);
	BOOL input_changeWav(const char* pszName);
	BOOL input_changeRmp(const char* pszName);
	BOOL insertFileDataBegin(HANDLE hFileHandle, BYTE* lpData, DWORD dSize);
	BOOL setListInfo(const char* pszName, LIST_INFO* pListInfo);
	BOOL getListInfo(LIST_INFO* pListInfo);
	BOOL getFileTagInfo(const char* pszName, TAG_INFO* pTagInfo);
	BOOL getFileInfo(const char* pszName, TAG_INFO* pTagInfo, MPEG_INFO* pMpegInfo);
	BOOL getFileInfo2(const char* pszName, TAG_INFO* pTagInfo, MPEG_INFO* pMpegInfo, LIST_INFO* pListInfo);
	BOOL getFileInfoMtr(const char* pszName, TAG_INFO* pTagInfo, MPEG_INFO* pMpegInfo, LIST_INFO* pListInfo,int* pFileType);
	long input_debug();
	void input_setDebug(long no);
	BOOL input_GetGenre(TAG_INFO* pTagInfo);
	BOOL input_SetxmMP3Option(xmMP3_OPTION* pxmMP3Option);
	void input_GetxmMP3Option(xmMP3_OPTION* pxmMP3Option);
	BOOL input_SetDecodeOption(DEC_OPTION* pDecOption);
	void input_GetDecodeOption(DEC_OPTION* pDecOption);
	BOOL getTagInfo(TAG_INFO* pTagInfo);
	BOOL setTagInfo(const char* pszName, TAG_INFO* pTagInfo,int tagSet, int tagAdd);
	BOOL getMpegInfo(MPEG_INFO* pMpegInfo);
	int getTime();


	BOOL initDec();
	BOOL freeDec();
	BOOL input_setCallback(INPUT_MSG_PROC pProc);
	BOOL input_startCallback();
	BOOL input_stopCallback();
	BOOL input_open(const char* pszName, InputInfo* pInfo);
	BOOL input_close();
	static int input_read(void* pData, int length);
	int input_getState();
	BOOL input_play();
	BOOL input_stop();
	BOOL input_pause();
	BOOL input_restart();
	//BOOL input_seek(int pos);
	void input_seekBySec(int sec);
	void input_seekByFrame(int frame);
	void input_seekBySample(int sample);
	void input_setEqualizer(int* pTable);
	static int input_bytesToSample(int bytes);
	static int input_secToSample(int sec);
	static int input_sampleToSec(int sample);
	long input_sampleToMs(int sample);
	static BOOL input_skipNextSync();
	BOOL input_setVolume(int left, int right);
	BOOL input_getVolume(int* left, int* right);
	void input_getSpectrum(int* pSpecL, int* pSpecR);
	void input_getWave(int* pWaveL, int* pWaveR);
	BOOL input_sendEvent(int code, int param);
	
	static int input_decode();

	static void input_handleEvent(const MSG& msg);
	static void input_actionStop(BOOL wait);
	static void input_actionPause();
	static void input_actionRestart();
	static void input_actionSeek(int sample);

	BOOL input_setLyricsFile(const char* pszLyricsName);
	BOOL input_getLyrics(LYRICS_INFO* pLyricsInfo);


	//IDTagv1
	static void strcpy_sID3v1(char* pszDest, const char* pszSrc);
	BOOL readID3v1(HANDLE FileHandle, TAG_INFO* lpTagInfo);
	BOOL readID3v11(HANDLE FileHandle, TAG_INFO_11* lpTagInfo);

	//IDTagv2
	BOOL readID3v2(HANDLE FileHandle,int* fSync);

	//RIFF
	BOOL readRIFFInfo(InputInfo* pInfo);
	BOOL readRIFF(HANDLE FileHandle, LIST_INFO* lplistInfo, int* fSync, DWORD* dSize);
	static void loadRMPInfo(LIST_INFO* pInfo, RMPInfo* pRMPInfo);
	static void strcpy_sRMP(char* pszDest, const char* pszSrc, int length, int maxSize);

	BOOL getListInfoEX(HANDLE FileHandle, LIST_INFO_EX* lplistInfo);
	static void loadRMPInfoEX(LIST_INFO_EX* lplistInfo, RMPInfo* pRMPInfo);
	BOOL setListInfoEX(HANDLE FileHandle, LIST_INFO_EX* pListInfo);

	int input_getLastErrorNo();

	static void __stdcall _NullProc(int msg, int param) ;
	static unsigned long __stdcall input_ThreadProc(void * lParam);

	//Output Fonction

	void output_setWaveOutDeviceId(int id);
	void output_setFftWindow(int window);
	int output_getWaveOutSupport();
	void output_setSoftVolume(int left, int right);
	void output_getSoftVolume(int* left, int* right);
	void output_setStepPitch(int pitch);
	int output_getStepPitch();
	void output_setPitch(int pitch);
	int output_getPitch();
	BOOL output_SetxmMP3Option(xmMP3_OPTION* pxmMP3Option);
	void output_GetxmMP3Option(xmMP3_OPTION* pxmMP3Option);
	static BOOL initOutput();
	static BOOL freeOutput();
	static BOOL output_open(const char* pszName, OutputInfo& info);
	static BOOL output_close();
	static int output_write(const void* pData, int length);
	static BOOL output_stop();
	static BOOL output_pause();
	static BOOL output_restart();
	static BOOL output_setVolume(int left, int right);
	static BOOL output_getVolume(int* left, int* right);
	static long output_getCurrentSample();
	static int output_getWave(int sample, int* pWaveL, int* pWaveR);
	static int output_getSpectrum(int sample, int* pSpecL, int* pSpecR);
	static int output_sampleToBytes(int sample);
	void output_setFadeIn(int on);
	void output_setFadeOut(int on);
	void output_softVolumeMax();

	static int output_bytesToSample(int bytes);
	static void output_playBlock();

	static void CALLBACK WaveProc(HWAVEOUT hWaveOut, UINT msg, DWORD instance, DWORD param1, DWORD param2);

	//WAVE����
	static void output_chgSoftVol( short *buf, int size );
	static void output_chgSoftVol8( unsigned char *buf, int size );
	static void output_chgStepSpeed( short *buf, int size );

	BOOL SetBufferRedim(int BufferRedimSize);
	static void fftCopy(int pos, short s);
	static void fftTransform(int* spec);

	//MP3Enc
	void initEnc();
	void freeEnc();

	void input_actionSpeedSeek(int seekSample);

	BOOL setTagInfoEX(const char* pszName, TAG_INFO_11* pTagInfo,int tagSet, int tagAdd);



private:
	static int m_errNo;
	static HANDLE	m_hFile;					//�Ώۂ̃t�@�C��
	
	static int		m_playSec;					//���t�o�ߎ���
	static int		m_playSecCur;				//���t�o�ߎ���(�o�͗p)
	static int		m_state;					//���݂̏��
	static double	m_playMs;					//���t���̎��ԁi�~���b�j
	static int		m_playSamples;				//���t�T���v��
	static int		m_firstSample;				//�T���v���̈ʒu
	static BYTE*	m_outputBuf_in;				//�o�̓o�b�t�@�iPCM�j
	static int		decodePos;					//�Đ����Ԍv�Z�p
	static BOOL		callBackFlag;		//�R�[���o�b�N�֐��g�p�L��

	static int		m_lvol;
	static int		m_rvol;
	static int		m_olvol;
	static int		m_orvol;
	static int		m_inputBytes;				//�f�R�[�h���Ă��Ȃ��o�b�t�@�T�C�Y
	static BYTE*	m_inputPtr;					//���̓|�C���^
	static BOOL		m_waveFlag;					//�I�[�v���t�@�C����PCM�Ȃ�TRUE
	static BYTE*	m_inputBuf;					//���̓o�b�t�@�iMPEG Audio�j
	static int		m_inputSize;				//���̓o�b�t�@�T�C�Y
	static MPEG_DECODE_INFO m_decInfo;
	static DWORD			dataSize;					//MPEG Audio�f�[�^�T�C�Y
	static int				decFlag;					//�f�R�[�h�t���O 0:�Đ� 1:Wave�o�� 2:�Đ����Ȃ���WAVE�o��
	static HANDLE			m_hWaveFile;				//WAVE�t�@�C��
	static int		m_inWaveBufSize;
	static int		m_readWaveDataSize;


	static int				m_totalSec;					//���t������
	static HANDLE			m_hThread;					//�f�R�[�h�p�X���b�h
	static DWORD				m_threadID;					//�f�R�[�h�p�X���b�hID
	static int             m_NEqualFrame;
	static INPUT_MSG_PROC	m_pProc;					//�R�[���o�b�N�֐�
	static int				m_userData;					//�R�[���o�b�N�֐��p�����[�^
	static int				m_playFrames;				//���t�t���[��
	static double			m_playSpeed;
	static int				m_reduction;
	static int				m_bitRate;
	static int				m_firstSync;				//�ŏ��̓����w�b�_�ʒu
	static double			m_msPerFrame;				//1�t���[���̉��t���ԁi�~���b�j
	static int				m_inputBlock;			//���̓t���[����
	static int				m_outputBlock_in;		//�o�̓t���[����
	static int				m_outputBlock_in_tmp;		//�o�̓t���[����
	static int				m_inputSleep;			//���͒���̃X���[�v����(�~���b)
	static BOOL			m_fadeOutFlag;
	static MPEG_DECODE_OPTION   decodeOption;			//�f�R�[�h�G���W���I�v�V����


	TAG_INFO		tagInfo;					//�^�O���
	int				fileSize;					//MPEG �t�@�C���T�C�Y
	LIST_INFO		listInfo;					//���X�g���
	int				m_FileType;					//̧�َ��
	LYRICS_WORK		m_lyrics[1000];				//�̎����
	int				m_liricsLinePointMax[1000];	//�s�̃^�O��
	int				m_lyricsCount;			//�^�O��
	int				m_lyricsLineNo;			//�̎��s��
	int				m_lyricsCurrent;
	int				m_winampTotalSec;
	int				m_lyricsTotalFlag;		//�̎��\�����Ɏg�p���鎞��(0:Winamp�݊� 1:�W������)
	long            debugNo;					//�f�o�b�O�p
	int				m_platForm;						//1:WinNT 0:���̑�
	int				m_listChunkPoint;
	int				m_listChunkSize;
	int				m_listIID3Point;
	int				m_listILYCPoint;
	int				m_listILYCSize;
	int				m_outputBlock_in_wav;		//�o�̓t���[����
	int            RiffStatus;					//MpegCheck
	WAVEFORMATEX	m_waveFormat;				//WAVE�t�H�[�}�b�g
	int				m_outWaveBufSize;
	
	
	static BOOL			m_openFlag;
	static BOOL			m_playFlag;
	static BOOL			m_timeFlag;			//TRUE:���t���Ԃ������ԉz������؂�̂�
	static BOOL			m_seekFlag;			//�V�[�N���Ȃ� TRUE
	static BOOL			m_stepFlag;
	static BOOL			m_Lyrics3UseFlag;	//Lyrics3 ���̎g�p�I��
	static BOOL			m_KaraokeTagFlag;	//�J���I�P�^�O�g�p�I��
	static BOOL			m_ExistKaraokeTag;
	static int				m_Lyrics3InsertPoint;	//0:ILYC 1:IID3 2:����


	//Output


	static HWAVEOUT		m_hWaveOut;					//�I�[�f�B�I�o�̓f�o�C�X
	static HANDLE	m_hSemaphore;				//�f�[�^�u���b�N�p�Z�}�t�H
	static int				m_channels;
	static int				m_bitsPerSample;
	static int				m_blockSize;
	static int				m_BufferRedim;
	static int				m_outputBlock;
	static int				m_outputIndex;
	static int				m_outputFinished;
	static BYTE**			m_outputTable;
	static int				m_waveBlock;
	static int				m_waveIndex;				//WAVEHDR �C���f�b�N�X
	static WAVEHDR*		m_waveTable;				//WAVEHDR �i�[�e�[�u��
	static BYTE*			m_outputPtr;
	static int				m_outputBytes;
	static int				m_outputSize;
	static int				m_pitch;				//�s�b�`(%)
	static int				m_outputSleep;
	static int				m_lSoftVol;						//�\�t�g�E�F�A�{�����[��(��)
	static int				m_rSoftVol;						//�\�t�g�E�F�A�{�����[��(�E)
	static int				m_lMaxSoftVol;			//�ő�\�t�g�E�F�A�{�����[��(��)
	static int				m_rMaxSoftVol;			//�ő�\�t�g�E�F�A�{�����[��(�E)
	static BOOL			m_fadeIn;
	static BOOL			m_fadeOut;
	static BOOL			m_stepPitch;
	static int				m_deviceId;		//�f�o�C�XID
	static int				m_fftWindow;

	static HWND				m_HwndWindow;
};

#endif // !defined(AFX_MP3INPUT_H__8613D81E_3FF4_4CCE_A5CA_762E30A4386B__INCLUDED_)
