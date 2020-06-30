//Commented by X-MaD 30/08/01 
/*/ #include "..\\include\\MP3Enc.h"
typedef unsigned int (__stdcall *THREADFUNC)(void* pParam);

void initEnc()
{
	m_pFormat = NULL;
	m_hSrcFile = INVALID_HANDLE_VALUE;
	m_hDstFile = INVALID_HANDLE_VALUE;
	m_encState = STATE_NON_ENCODE;
}

void freeEnc()
{
	::free(m_pFormat);
	enc_close();
}

BOOL CALLBACK ACMDriverProc(HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport)
{
	_ACMDRIVERTABLE* padt = (_ACMDRIVERTABLE*)dwInstance;
	MMRESULT		mmr;
	ACMDRIVERDETAILS add;

	if (!(fdwSupport & ACMDRIVERDETAILS_SUPPORTF_CODEC)) {
		return true;
	}
	add.cbStruct = sizeof(add);
	mmr = acmDriverDetails(hadid, &add, 0L);

	padt->pTable = (_ACMDRIVER*)realloc(padt->pTable, 
			(padt->count + 1) * sizeof(_ACMDRIVER));
	padt->pTable[padt->count].hadid = hadid;
	padt->pTable[padt->count].add = add;
	padt->count ++;
	return true;
}

BOOL CALLBACK ACMFormatProc(HACMDRIVERID hadid, ACMFORMATDETAILS* pafd, DWORD dwInstance, DWORD fdwSupport)
{
	_ACMFORMATTABLE* paft = (_ACMFORMATTABLE*)dwInstance;

	paft->pTable = (_ACMFORMAT*)realloc(paft->pTable, 
			(paft->count + 1) * sizeof(_ACMFORMAT));
	paft->pTable[paft->count].pwfx = (WAVEFORMATEX*)malloc(pafd->cbwfx);
	memcpy(paft->pTable[paft->count].pwfx, pafd->pwfx, pafd->cbwfx);
	strcpy(paft->pTable[paft->count].szFormat, pafd->szFormat);
	paft->count ++;
	return true;
}

//コーデックテーブル作成
_ACMDRIVERTABLE* CreateCodecTable()
{
	_ACMDRIVERTABLE* padt;
	MMRESULT		mmr;

	padt = (_ACMDRIVERTABLE*)malloc(sizeof(_ACMDRIVERTABLE));
	padt->count = 0;
	padt->pTable = NULL;
	mmr = acmDriverEnum(ACMDriverProc, (DWORD)padt, ACM_DRIVERENUMF_NOLOCAL);
	return padt;
}

BOOL DestroyCodecTable(_ACMDRIVERTABLE* padt)
{
	free(padt->pTable);
	free(padt);
	return true;
}

//WAVEフォーマットテーブル作成
_ACMFORMATTABLE* CreateFormatTable(HACMDRIVER had, WORD formatTag)
{
	_ACMFORMATTABLE* paft;
	MMRESULT		mmr;
	DWORD			formatSize;
	ACMFORMATDETAILS afd;

	paft = (_ACMFORMATTABLE*)malloc(sizeof(_ACMFORMATTABLE));
	paft->count = 0;
	paft->pTable = NULL;
	mmr = acmMetrics((HACMOBJ)had, ACM_METRIC_MAX_SIZE_FORMAT, &formatSize);

	memset(&afd, 0, sizeof(afd));
	afd.cbStruct = sizeof(afd);
	afd.pwfx = (WAVEFORMATEX*)malloc(formatSize);
	afd.pwfx->wFormatTag = formatTag;
	afd.cbwfx = formatSize;
	afd.dwFormatTag = formatTag;
	mmr = acmFormatEnum(had, &afd, ACMFormatProc,
			(DWORD)paft, ACM_FORMATENUMF_WFORMATTAG);
	free(afd.pwfx);
	return paft;
}

//コーデックテーブル破棄
BOOL DestroyFormatTable(_ACMFORMATTABLE* paft)
{
	for (int i = 0; i < paft->count; i ++) {
		free(paft->pTable[i].pwfx);
	}
	free(paft->pTable);
	free(paft);
	return true;
}

int enc_getEncodeState(int* readSize, int* encodeSize)
{
	*readSize = m_readed;
	*encodeSize = m_encoded;
	return m_encState;
}

BOOL enc_open(const char* pszWaveName,  WAVE_FORM* pWaveForm)
{
	WAVEFORMATEX*	pwfxSrc;

	if ( m_hSrcFile != INVALID_HANDLE_VALUE || m_hDstFile != INVALID_HANDLE_VALUE ){
		::input_setErrNo(ERR_ENCODE_FILE_OPEN);
		return FALSE;
	}

	//ファイルオープンチェック
	if (pszWaveName[0] == 0) {
		//何も入力されなかった場合は終了
		::input_setErrNo(ERR_WAV_FILE_OPEN);
		return FALSE;
	}

	//WAVEファイルオープン
	m_hSrcFile = ::CreateFile(pszWaveName, GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hSrcFile == INVALID_HANDLE_VALUE) {
		input_setDebug(4);
		::input_setErrNo(ERR_WAV_FILE_OPEN);
		return FALSE;
	}

	//WAVEフォーマット取得
	pwfxSrc = enc_getFormat();
	if ( pwfxSrc == NULL ) {
		enc_close();
		::input_setErrNo(ERR_WAV_FORMAT);
		return FALSE;
	}

	pWaveForm->channels = pwfxSrc->nChannels;
	pWaveForm->bitsPerSample = pwfxSrc->wBitsPerSample;
	pWaveForm->samplingRate = pwfxSrc->nSamplesPerSec;
	pWaveForm->dataSize = m_dataSize;

	m_enc_channels = pwfxSrc->nChannels;
	m_enc_samplingRate = pwfxSrc->nSamplesPerSec;

	return TRUE;
}

BOOL enc_start(const char* pszMp3Name)
{
	_ACMDRIVERTABLE* padt;
	_ACMFORMATTABLE* paft;
	WAVEFORMATEX*	pwfxSrc;
	WAVEFORMATEX*	pwfxDst;
	int i;

	if ( m_hSrcFile == INVALID_HANDLE_VALUE || m_hDstFile != INVALID_HANDLE_VALUE ){
		enc_close();
		::input_setErrNo(ERR_ENCODE_FILE_OPEN);
		return FALSE;
	}

	//ファイルオープンチェック
	if (pszMp3Name[0] == 0) {
		//何も入力されなかった場合は終了
		enc_close();
		::input_setErrNo(ERR_MP3_FILE_OPEN);
		return FALSE;
	}

	//コーデックテーブル作成
	padt = CreateCodecTable();

	//コーデック一覧表示
	for (i = 0; i < padt->count; i ++) {
		if (::strcmp(padt->pTable[i].add.szLongName, "Fraunhofer IIS MPEG Layer-3 Codec (professional)") == 0){
			break;
		}
	}

	if ( i == padt->count ){
		//コーデックテーブル破棄
		DestroyCodecTable(padt);
		enc_close();
		::input_setErrNo(ERR_CODEC_NOT_FOUND);
		return FALSE;
	}

	//指定コーデックオープン
	m_mmr = acmDriverOpen(&m_had, padt->pTable[i].hadid, 0L);

	//コーデックテーブル破棄
	DestroyCodecTable(padt);

	//WAVEフォーマットテーブル作成
	paft = CreateFormatTable(m_had, 0x55);

	if (paft->count == 0) {
		//WAVEフォーマットテーブル破棄
		DestroyFormatTable(paft);
		enc_close();
		::input_setErrNo(ERR_WAVE_TABLE_NOT_FOUND);
		return FALSE;
	}

	//WAVEフォーマット取得
	pwfxSrc = enc_getFormat();
	if (  m_enc_channels == 2 && m_enc_samplingRate == 48000 ){
		pwfxDst = paft->pTable[0].pwfx;
	} else if (  m_enc_channels == 2 && m_enc_samplingRate == 44100 ){
		pwfxDst = paft->pTable[3].pwfx;
	} else if (  m_enc_channels == 2 && m_enc_samplingRate == 32000 ){
		pwfxDst = paft->pTable[6].pwfx;
	} else if (  m_enc_channels == 2 && m_enc_samplingRate == 24000 ){
		pwfxDst = paft->pTable[8].pwfx;
	} else if (  m_enc_channels == 2 && m_enc_samplingRate == 22050 ){
		pwfxDst = paft->pTable[11].pwfx;
	} else if (  m_enc_channels == 2 && m_enc_samplingRate == 16000 ){
		pwfxDst = paft->pTable[14].pwfx;
	} else if (  m_enc_channels == 2 && m_enc_samplingRate == 12000 ){
		pwfxDst = paft->pTable[16].pwfx;
	} else if (  m_enc_channels == 2 && m_enc_samplingRate == 11025 ){
		pwfxDst = paft->pTable[20].pwfx;
	} else if (  m_enc_channels == 2 && m_enc_samplingRate == 8000 ){
		pwfxDst = paft->pTable[24].pwfx;
	} else if (  m_enc_channels == 1 && m_enc_samplingRate == 48000 ){
		pwfxDst = paft->pTable[28].pwfx;
	} else if (  m_enc_channels == 1 && m_enc_samplingRate == 44100 ){
		pwfxDst = paft->pTable[31].pwfx;
	} else if (  m_enc_channels == 1 && m_enc_samplingRate == 32000 ){
		pwfxDst = paft->pTable[34].pwfx;
	} else if (  m_enc_channels == 1 && m_enc_samplingRate == 24000 ){
		pwfxDst = paft->pTable[37].pwfx;
	} else if (  m_enc_channels == 1 && m_enc_samplingRate == 22050 ){
		pwfxDst = paft->pTable[39].pwfx;
	} else if (  m_enc_channels == 1 && m_enc_samplingRate == 16000 ){
		pwfxDst = paft->pTable[41].pwfx;
	} else if (  m_enc_channels == 1 && m_enc_samplingRate == 12000 ){
		pwfxDst = paft->pTable[46].pwfx;
	} else if (  m_enc_channels == 1 && m_enc_samplingRate == 11025 ){
		pwfxDst = paft->pTable[50].pwfx;
	} else if (  m_enc_channels == 1 && m_enc_samplingRate == 8000 ){
		pwfxDst = paft->pTable[54].pwfx;
	}

	//MP3ファイルオープン
	m_hDstFile = ::CreateFile(pszMp3Name, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hDstFile == INVALID_HANDLE_VALUE) {
		enc_close();
		::input_setErrNo(ERR_MP3_FILE_OPEN);
		return FALSE;
	}

	//コーデックストリームオープン
	m_mmr = acmStreamOpen(&m_has, m_had,
				pwfxSrc, pwfxDst, NULL, 0, 0, 0);
	if (m_mmr != MMSYSERR_NOERROR) {
		DestroyFormatTable(paft);
		m_mmr = acmDriverClose(m_had, 0);
		enc_close();
		::input_setErrNo(ERR_ACM_OPEN);
		return FALSE;
	}
	//WAVEフォーマットテーブル破棄
	DestroyFormatTable(paft);

	//ACMヘッダ設定
	memset(&m_ash, 0, sizeof(m_ash));
	m_ash.cbStruct		= sizeof(m_ash);
	m_ash.pbSrc			= m_srcBlock;
	m_ash.cbSrcLength		= sizeof(m_srcBlock);
	m_ash.pbDst			= m_dstBlock;
	m_ash.cbDstLength		= sizeof(m_dstBlock);
	m_mmr = acmStreamPrepareHeader(m_has, &m_ash, 0);
	
	//デコード用スレッド開始
	m_hEncThread = (HANDLE)::_beginthreadex(NULL, 0, 
			(THREADFUNC)enc_ThreadProc, NULL, 0, (unsigned int*)&m_encThreadID);
	if (m_hEncThread == NULL) {
		enc_close();
		::input_setErrNo(ERR_ENCODE_THREAD);
		return FALSE;
	}

	//enc_encode();
	return TRUE;
}

int enc_ThreadProc()
{
	return enc_encode();
}

int enc_encode()
{
	m_encoded = 0;
	m_readed = 0;
	bool done = false;
	m_ash.cbSrcLengthUsed = sizeof(m_srcBlock);

	m_encState = STATE_ENCODE;

	//エンコードループ
	while (!done) {
		//残サイズ取得
		int remains = sizeof(m_srcBlock) - m_ash.cbSrcLengthUsed;

		//ソースより未エンコードデータ取得
		memmove(m_srcBlock, &m_srcBlock[m_ash.cbSrcLengthUsed], remains);
		int bytes = enc_read(&m_srcBlock[remains], sizeof(m_srcBlock) - remains);
		m_readed += bytes;

		//読み込みデータが無くなった時点で終了
		if (bytes == 0) done = true;
		
		//エンコード完了サイズ取得
		m_ash.cbSrcLength	= bytes + remains;

		//エンコード
		m_mmr = acmStreamConvert(m_has, &m_ash, ACM_STREAMCONVERTF_BLOCKALIGN);
		if (m_mmr != 0) {
			break;
		}

		//エンコード結果書き込み
		enc_write(m_dstBlock, m_ash.cbDstLengthUsed);

		//エンコード完了サイズ取得
		m_encoded += m_ash.cbDstLengthUsed;

		//if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		//	input_handleEvent(msg);
		//}
		if (m_encState == STATE_NON_ENCODE) {
			done = true;
		}

	}

	//終了処理
	m_encThreadID = 0;
	m_encoded = 0;
	m_readed = 0;
	m_mmr = acmStreamUnprepareHeader(m_has, &m_ash, 0);
	m_mmr = acmStreamClose(m_has, 0);
	m_mmr = acmDriverClose(m_had, 0);
	m_encState = STATE_NON_ENCODE;
	::CloseHandle(m_hEncThread);
	enc_close();
	return 0;
}

BOOL enc_stop()
{
	if ( m_encThreadID != 0 && m_encState == STATE_ENCODE ) {
		m_encState = STATE_NON_ENCODE;
//		if (::PostThreadMessage(m_encThreadID, EV_ENC_STOP, 0, 0)) {
//			::WaitForSingleObject(m_hEncThread, 500);
//		}
	} else {
		::input_setErrNo(ERR_STATE_NON_ENCODE);
		return FALSE;
	}
	return TRUE;
}

//WAVEフォーマット取得
WAVEFORMATEX* enc_getFormat()
{
	RiffChunk	riff;
	CommonChunk common;

	enc_seek(0, FILE_BEGIN);

	enc_read(&riff, sizeof(riff));

	if (memcmp(riff.chunkID, "RIFF", 4) != 0) {
		input_setDebug(7);
		return NULL;
	}
	if (memcmp(riff.formatType, "WAVE", 4) != 0) {
		input_setDebug(8);
		return NULL;
	}

	//find data chunk
	for (;;) {
		enc_read(&common, sizeof(common));
		if (memcmp(common.chunkID, "data", 4) == 0) {
			m_dataSize = common.chunkSize;
			break;
		}
		else if (memcmp(common.chunkID, "fmt ", 4) == 0) {
			int size = common.chunkSize;
			m_pFormat = (BYTE*)::realloc(m_pFormat, size);
			enc_read(m_pFormat, size);
		}
		else {
			enc_seek(common.chunkSize, FILE_CURRENT);
		}
	}
	return (WAVEFORMATEX*)m_pFormat;
}


//ファイルクローズ
void enc_close()
{
	if (m_hSrcFile != INVALID_HANDLE_VALUE) {
		::CloseHandle(m_hSrcFile);
		m_hSrcFile = INVALID_HANDLE_VALUE;
	}
	if (m_hDstFile != INVALID_HANDLE_VALUE) {
		::CloseHandle(m_hDstFile);
		m_hDstFile = INVALID_HANDLE_VALUE;
	}
}

//ファイル読込
int enc_read(void* pData, int length)
{
	int readBytes;
	if (!::ReadFile(m_hSrcFile, pData, length, (DWORD*)&readBytes, NULL)) {
		return 0;//exception
	}
	return readBytes;
}

//ファイル書き込み
int enc_write(void* pData, int length)
{
	int writeBytes;
	if (!::WriteFile(m_hDstFile, pData, length, (DWORD*)&writeBytes, NULL)) {
		return 0;//exception
	}
	return writeBytes;
}

//ファイルシーク
void enc_seek(int pos, int method)
{
	::SetFilePointer(m_hSrcFile, pos, NULL, method);
} /*/
