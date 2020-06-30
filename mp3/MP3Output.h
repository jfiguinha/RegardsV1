#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <Mmsystem.h>


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


#ifdef OutputInfo
#else

struct OutputInfo
{
	int		channels;			//チャネル数
	int		bitsPerSample;		//ビット数/1サンプル
	int		frequency;
	int		bufferSize;
};

#define OutputInfo

#endif

#ifdef xmMP3_OPTION
#else

struct xmMP3_OPTION
{
	int inputBlock;			//入力フレーム数
	int outputBlock;		//出力フレーム数
	int inputSleep;			//入力直後のスリープ時間(ミリ秒)
	int outputSleep;		//出力直後のスリープ時間(ミリ秒)
};

#define xmMP3_OPTION

#endif

HWAVEOUT		m_hWaveOut;					//オーディオ出力デバイス
//HANDLE			m_out_hThread;
//int				m_out_threadID;

HANDLE			m_hSemaphore;				//データブロック用セマフォ
CRITICAL_SECTION m_cs;						//各種変数 同期用

int				m_channels;
int				m_bitsPerSample;
int				m_blockSize = 1;
int				m_BufferRedim = 1;

int				m_outputBlock;
int				m_outputIndex;
int				m_outputFinished;
BYTE**			m_outputTable;

//boolean			m_analyze = false;
//boolean			m_analyzeThread = false;
//int				m_specTableL[256];
//int				m_specTableR[256];

int				m_waveBlock;
int				m_waveIndex;				//WAVEHDR インデックス
//int				m_waveStack;				//未再生 WAVEHDR 数
//int				m_waveCount = 16;				//WAVEHDR 配列数
WAVEHDR*		m_waveTable;				//WAVEHDR 格納テーブル

BYTE*			m_outputPtr;
int				m_outputBytes;
int				m_outputSize;
int				m_pitch = 100;				//ピッチ(%)

/*
BYTE*		m_outputBuf;
BYTE*		m_outputPtr;
int			m_outputSize;
int			m_outputBytes;
*/
int				m_outputSleep = 0;

int				m_lSoftVol;						//ソフトウェアボリューム(左)
int				m_rSoftVol;						//ソフトウェアボリューム(右)
int				m_lMaxSoftVol = 100;			//最大ソフトウェアボリューム(左)
int				m_rMaxSoftVol = 100;			//最大ソフトウェアボリューム(右)
BOOL			m_fadeIn;
BOOL			m_fadeOut;

BOOL			m_stepPitch;

int				m_deviceId = WAVE_MAPPER;		//デバイスID

enum {
	rectangle	= 0,
	hanning		= 1,
	hamming		= 2,
	blackman	= 3,
};

int				m_fftWindow = hanning;

#define ALLOW_DATA_SIZE		268435455

//MP3Output
/*
void output_startAnalyze();
void output_stopAnalyze();
void output_startAnalyzeThread();
void output_stopAnalyzeThread();
*/

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

BOOL initOutput();
BOOL freeOutput();
//BOOL SetBufferRedim(int BufferRedimSize);
BOOL output_open(const char* pszName, OutputInfo& info);
BOOL output_close();
int output_write(const void* pData, int length);
BOOL output_stop();
BOOL output_pause();
BOOL output_restart();
BOOL output_setVolume(int left, int right);
BOOL output_getVolume(int* left, int* right);

//void output_getSpectrum(int* pSpecL, int* pSpecR);
long output_getCurrentSample();
int output_getWave(int sample, int* pWaveL, int* pWaveR);
int output_getSpectrum(int sample, int* pSpecL, int* pSpecR);

//int output_ThreadProc();
//void output_analyze();

int output_bytesToSample(int bytes);
int output_sampleToBytes(int sample);
void output_playBlock();

void CALLBACK WaveProc(HWAVEOUT hWaveOut, UINT msg,
		DWORD instance, DWORD param1, DWORD param2);

//WAVE操作
void output_softVolumeMax();
void output_chgSoftVol( short *buf, int size );
void output_chgSoftVol8( unsigned char *buf, int size );
void output_setFadeIn(int on);
void output_setFadeOut(int on);
void output_chgStepSpeed( short *buf, int size );

//inputMP3
void input_callBackTime();
int input_getDecodeOutputSize();
int input_getTotalOutputSize();
void input_fadeOutVolume();
void input_setDebug(long no);

void fftCopy(int pos, short s);
void fftTransform(int* spec);
