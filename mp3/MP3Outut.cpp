#include "MP3Output.h"
#include "Fft.h"
typedef unsigned int (__stdcall *THREADFUNC)(void* pParam);

void output_setWaveOutDeviceId(int id){
	m_deviceId = id;
}

void output_setSoftVolume(int left, int right)
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

void output_getSoftVolume(int* left, int* right)
{
	*left = m_lSoftVol;
	*right = m_rSoftVol;
}

void output_setStepPitch(int pitch)
{
	m_stepPitch = pitch;
}

int output_getStepPitch()
{
	return m_stepPitch;
}

void output_setPitch(int pitch)
{
	m_pitch = pitch;
}

int output_getPitch()
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

BOOL output_SetxmMP3Option(xmMP3_OPTION* pxmMP3Option)
{
//	m_waveCount = pxmMP3Option->outputBlock;
	m_outputSleep = pxmMP3Option->outputSleep;
	return TRUE;
}

void output_GetxmMP3Option(xmMP3_OPTION* pxmMP3Option)
{
//	pxmMP3Option->outputBlock = m_waveCount;
//	pxmMP3Option->outputBlock = 0;
	pxmMP3Option->outputSleep = m_outputSleep;
	return;
}

BOOL initOutput()
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
	m_outputTable	= (byte**)::malloc(m_outputBlock * sizeof(byte**));
	for (int i = 0; i < m_outputBlock; i ++) {
		m_outputTable[i] = NULL;
	}
	::InitializeCriticalSection(&m_cs);

	return TRUE;
}

BOOL freeOutput()
{
	output_stop();
	output_close();
	for (int i = 0; i < m_waveBlock; i ++) {
		WAVEHDR* pWaveHdr = &m_waveTable[i];
		::free(pWaveHdr->lpData);
	}
	::free(m_waveTable);
	for (i = 0; i < m_outputBlock; i ++) {
		::free(m_outputTable[i]);
	}
	::free(m_outputTable);
	::DeleteCriticalSection(&m_cs);

	return TRUE;
}

//X-MaD 30/08/01
BOOL SetBufferRedim(int BufferRedimSize)
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

BOOL output_open(const char* pszName, OutputInfo& info)
{
	output_stop();
	output_close();

	//セマフォ作成
	m_hSemaphore = ::CreateSemaphore(NULL, m_waveBlock, m_waveBlock, NULL);
	if (m_hSemaphore == NULL) {
		//throw Exception(::GetLastError());
		return FALSE;
	}

	//出力デバイスオープン
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
		/*
		int mmErrNo = mmresult;
		char work[1000];
		char mmErrMsg[1000];
		mmresult = ::waveOutGetErrorText(mmErrNo, work, 1000);
		::sprintf(mmErrMsg, "ErrorNo : %d\n  %s", mmErrNo, work);
		::MessageBox(NULL, mmErrMsg, "xmMP3", MB_OK);
		*/
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
		m_outputTable[i] = (byte*)::realloc(m_outputTable[i], m_blockSize);
		memset(m_outputTable[i], 0, m_blockSize);
	}
	m_outputFinished= 0;
	m_outputBytes	= 0;
	m_outputPtr		= m_outputTable[0];

	m_waveIndex	= 0;
	for (i = 0; i < m_waveBlock; i ++) {
		WAVEHDR* pWaveHdr = &m_waveTable[i];
		pWaveHdr->lpData = (char*)::realloc(pWaveHdr->lpData, m_blockSize);
		pWaveHdr->dwBufferLength = m_blockSize;
		::waveOutPrepareHeader(m_hWaveOut, pWaveHdr, sizeof(WAVEHDR));
	}

	return TRUE;

}

BOOL output_close()
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
			::input_callBackTime();
			::input_fadeOutVolume();
			::Sleep(60);
		}
	}

	output_stop();	//解放処理
	while (::waveOutClose(m_hWaveOut) != MMSYSERR_NOERROR) {
		::Sleep(100);
	}
	::CloseHandle(m_hSemaphore);
	m_hWaveOut = NULL;
	m_hSemaphore = NULL;

	return TRUE;
}

int output_write(const void* pData, int length)
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
			::memcpy(m_outputPtr, ((byte*)pData + reserve), length);
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

BOOL output_stop()
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

BOOL output_pause()
{
	MMRESULT mmresult = ::waveOutPause(m_hWaveOut);
	if (mmresult != MMSYSERR_NOERROR) {
		return FALSE;
	}

	return TRUE;
}

BOOL output_restart()
{
	MMRESULT mmresult = ::waveOutRestart(m_hWaveOut);
	if (mmresult != MMSYSERR_NOERROR) {
		return FALSE;
	}

	return TRUE;
}

BOOL output_setVolume(int left, int right)
{
	int maxVol	 = 0xFFFF;//ボリュームの最大値
	int leftVol	 = left * maxVol / 100;
	int rightVol = right * maxVol / 100;

	//すべての出力デバイスにボリュームを設定
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


BOOL output_getVolume(int* left, int* right)
{
	//すべての出力デバイスにボリュームを設定
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

	UINT maxVol	  = 0xFFFF;//ボリュームの最大値
	UINT leftVol  = (UINT)(dwVolume & 0x0000FFFF);
	UINT rightVol = (UINT)(dwVolume >> 16);
	
	*left = leftVol * 100 / maxVol;
	*right = rightVol * 100 / maxVol;

	return TRUE;
}

int output_getWaveOutSupport()
{
    MMRESULT mmresult;
    WAVEOUTCAPS woc;

//    mmresult = waveOutGetDevCaps(WAVE_MAPPER, &woc, sizeof(woc));
    mmresult = waveOutGetDevCaps(m_deviceId, &woc, sizeof(woc));
	input_setDebug(woc.dwSupport);
    if (mmresult != 0) { return 0; }
    return woc.dwSupport;
}

long output_getCurrentSample()
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

#define FFT_SIZE	512
#define FFT_BAND	256
#define WAVE_BAND	256

Fft m_fft_rectangle(FFT_SIZE, Fft::rectangle);
Fft m_fft_hanning(FFT_SIZE, Fft::hanning);
Fft m_fft_hamming(FFT_SIZE, Fft::hamming);
Fft m_fft_blackman(FFT_SIZE, Fft::blackman);


void output_setFftWindow(int window)
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

void fftCopy(int pos, short s)
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

void fftTransform(int* spec)
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

int output_getWave(int sample, int* pWaveL, int* pWaveR)
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
	byte* p1 = m_outputTable[index];
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
		for (i = 0; i < WAVE_BAND; i ++) {
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

int output_getSpectrum(int sample, int* pSpecL, int* pSpecR)
{
	byte fftBuf[FFT_SIZE * sizeof(short) * 2];
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
	
	byte* p1 = m_outputTable[index];
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
		for (i = 0; i < FFT_SIZE; i ++) {
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

int output_bytesToSample(int bytes)
{
	if ( bytes > ALLOW_DATA_SIZE ) {
		return (bytes / m_bitsPerSample / m_channels * 8);
	} else {
		return (bytes * 8 / m_bitsPerSample / m_channels);
	}
}

int output_sampleToBytes(int sample)
{
	unsigned long Temp = 0;
	Temp = sample * m_bitsPerSample;
	Temp = Temp * m_channels;
	return (int)(Temp / 8);
}

void output_playBlock()
{
	BOOL fadeOutFlg = FALSE;
	::WaitForSingleObject(m_hSemaphore, INFINITE);
	WAVEHDR* pWaveHdr = &m_waveTable[m_waveIndex];

	::memcpy(pWaveHdr->lpData, m_outputTable[m_outputIndex], m_blockSize);
	m_outputSize = m_outputSize + pWaveHdr->dwBufferLength;
	//フェードアウト
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

	//フェードイン
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
		//WinNT で問題あり
		int tmpSize = pWaveHdr->dwBufferLength;
		pWaveHdr->dwBufferLength = pWaveHdr->dwBufferLength / m_stepPitch;
		::waveOutWrite(m_hWaveOut, pWaveHdr, sizeof(WAVEHDR));
		pWaveHdr->dwBufferLength = tmpSize;
	}

	m_waveIndex = (m_waveIndex + 1) % m_waveBlock;

	::EnterCriticalSection(&m_cs);
	m_outputFinished ++;
	m_outputIndex = (m_outputIndex + 1) % m_outputBlock;
	m_outputPtr = m_outputTable[m_outputIndex];
	m_outputBytes = 0;
	::LeaveCriticalSection(&m_cs);
	if (m_outputFinished % 4 == 0) Sleep(m_outputSleep);
}

//コールバック関数
void CALLBACK WaveProc(HWAVEOUT hWaveOut, UINT msg,
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


//WAVE操作
void output_softVolumeMax()
{
	m_lSoftVol = m_lMaxSoftVol;
	m_rSoftVol = m_rMaxSoftVol;
}

void output_setFadeIn(int on)
{
	if ( on == 0 ) {
		m_fadeIn = false;
		m_lSoftVol = m_lMaxSoftVol;
		m_rSoftVol = m_rMaxSoftVol;
	} else {
		m_fadeIn = true;
	}
}

void output_setFadeOut(int on)
{
	if ( on == 0 ) {
		m_fadeOut = false;
		m_lSoftVol = m_lMaxSoftVol;
		m_rSoftVol = m_rMaxSoftVol;
	} else {
		m_fadeOut = true;
	}
}

void output_chgSoftVol( short *buf, int size )
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

void output_chgSoftVol8( unsigned char *buf, int size )
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


//テスト中
void output_chgStepSpeed( short *buf, int size )
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
