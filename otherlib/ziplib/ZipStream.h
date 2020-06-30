// ZipStream.h: interface for the CZipStream class.
//
//////////////////////////////////////////////////////////////////////

#define ZLIB_DLL

#include "zlib.h"
#include "Buffer.h"

class CZipStream  
{
public:
	CZipStream(DWORD newMethod, DWORD crc32, DWORD uncompressedSize, DWORD compressedSize);
	CZipStream(DWORD newMethod);
	virtual ~CZipStream();

	static void* myalloc(void* opaque, UINT items, UINT size)
	{
		return new char[items * size];
	}
	static void myfree(void* opaque, void* address)
	{
		delete[] address;
	}

	int extract(CFileHandle &theZipFile, CExtractTarget &aTarget);
	int compress(CFileHandle &aZipFile, CFileHandle &anInputFile, int aLevel);

	inline DWORD getCrc32(void) { return m_crc32; }
	inline DWORD getCompressedSize(void) { return m_compressedSize; }
	inline DWORD getUncompressedSize(void) { return m_uncompressedSize; }

private:
	void init(DWORD method, DWORD crc32, DWORD uncompressedSize, DWORD compressedSize);

	z_stream m_stream;
	CBuffer m_buffer;
	static int m_bufferSize;
	DWORD m_compressedSize;
	DWORD m_uncompressedSize;
	DWORD m_crc32;
	DWORD m_method;
};


