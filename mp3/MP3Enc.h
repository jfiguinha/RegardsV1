//Commented by X-MaD 30/08/01 
/*/ 
Def Export
  xmMP3_getEncodeState
  xmMP3_encodeOpen
  xmMP3_encodeStart
  xmMP3_encodeStop


#include <windows.h>
#include <stdio.h>
#include <mmreg.h>
#include <msacm.h>
#include <process.h>

#pragma comment(lib, "msacm32.lib")

#pragma pack(1)
typedef struct {
	ACMDRIVERDETAILS	add;
	HACMDRIVERID		hadid;
} _ACMDRIVER;

typedef struct {
	int				count;
	_ACMDRIVER*		pTable;
} _ACMDRIVERTABLE;

typedef struct {
    char            szFormat[ACMFORMATDETAILS_FORMAT_CHARS];
	WAVEFORMATEX*	pwfx;
} _ACMFORMAT;

typedef struct {
	int				count;
	_ACMFORMAT*		pTable;
} _ACMFORMATTABLE;
#pragma pack()

struct RiffChunk
{
	BYTE	chunkID[4];
	DWORD	chunkSize;
	BYTE	formatType[4];
};

struct CommonChunk
{
	BYTE	chunkID[4];
	DWORD	chunkSize;	
};

struct WAVE_FORM
{
	int		channels;			//チャネル数
	int		bitsPerSample;		//ビット数/1サンプル
	int		samplingRate;
	int		dataSize;			//ファイルサイズ
};

HANDLE				m_hEncThread;					//エンコード用スレッド
int					m_encThreadID;					//エンコード用スレッドID

BYTE*				m_pFormat;
DWORD				m_dataSize;
HANDLE				m_hSrcFile;
HANDLE				m_hDstFile;
BYTE				m_srcBlock[8192];
BYTE				m_dstBlock[2048];
ACMSTREAMHEADER		m_ash;
MMRESULT			m_mmr;
HACMDRIVER			m_had;
HACMSTREAM			m_has;
int					m_encoded = 0;
int					m_readed = 0;
int					m_encState;

int					m_enc_channels;
int					m_enc_samplingRate;

void initEnc();
void freeEnc();
void enc_close();
int  enc_read(void* pData, int length);
int  enc_write(void* pData, int length);
void enc_seek(int pos, int method);
int  enc_getEncodeState(int* readSize, int* encodeSize);
BOOL enc_open(const char* pszWaveName, WAVE_FORM* pWaveForm);
BOOL enc_start(const char* pszMp3Name);
BOOL enc_stop();
int  enc_encode();
int  enc_ThreadProc();
WAVEFORMATEX* enc_getFormat();

BOOL CALLBACK ACMDriverProc(HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport);
BOOL CALLBACK ACMFormatProc(HACMDRIVERID hadid, ACMFORMATDETAILS* pafd, DWORD dwInstance, DWORD fdwSupport);
_ACMDRIVERTABLE* CreateCodecTable();
BOOL DestroyCodecTable(_ACMDRIVERTABLE* padt);
_ACMFORMATTABLE* CreateFormatTable(HACMDRIVER had, WORD formatTag);
BOOL DestroyFormatTable(_ACMFORMATTABLE* paft);

//MP3Input
void input_setDebug(long no);
void input_setErrNo(int no);

//エラーステータス
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
};


//現在の状態
enum {
	STATE_NON_ENCODE	= 0,
	STATE_ENCODE		= 1,
};

//エンコードスレッド用メッセージ
enum {
	EV_ENC_STOP		= (WM_APP + 0),
}; /*/

