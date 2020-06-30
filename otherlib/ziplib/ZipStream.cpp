// ZipStream.cpp: implementation of the CZipStream class.
//
//////////////////////////////////////////////////////////////////////


#include "ZipStream.h"

#ifndef DEF_MEM_LEVEL
#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int CZipStream::m_bufferSize = 65535;

CZipStream::CZipStream(DWORD method, DWORD crc32, DWORD uncompressedSize, DWORD compressedSize)
 : m_buffer(m_bufferSize)
{
	init(method, crc32, uncompressedSize, compressedSize);
}

CZipStream::CZipStream(DWORD newMethod) : m_buffer(m_bufferSize)
{
	init(newMethod, 0, 0, 0);
}

void CZipStream::init(DWORD method, DWORD crc32, DWORD uncompressedSize, DWORD compressedSize)
{
	m_stream.zalloc    = (alloc_func) myalloc;
	m_stream.zfree     = (free_func) myfree;
	m_crc32            = crc32;
	m_compressedSize   = compressedSize;
	m_uncompressedSize = uncompressedSize;
	m_method           = method;
}

CZipStream::~CZipStream()
{
}

int CZipStream::extract(CFileHandle &theZipFile, CExtractTarget &aTarget)
{
	inflateInit2(&m_stream, -MAX_WBITS);
	
	m_stream.total_out = 0;
	m_stream.avail_in  = 0;
	m_stream.opaque    = 0;

	DWORD comprLeft = m_compressedSize;
	DWORD uncomprLeft = m_uncompressedSize;
	DWORD crc32File = m_crc32;
	DWORD readTotalCount;
	DWORD readCount;

	CBuffer writeBuffer(m_bufferSize);

	do
	{
		readTotalCount = 0;
		readCount = 0;

		m_stream.next_out  = (unsigned char *)(char*) writeBuffer;
		m_stream.avail_out = writeBuffer.getSize() > uncomprLeft ? uncomprLeft : writeBuffer.getSize();

		while (m_stream.avail_out > 0 )
		{
			if (    ( m_stream.avail_in == 0 )
				 && ( comprLeft          > 0 ))
			{
				DWORD bytesToRead = ( comprLeft < m_bufferSize ) ? comprLeft : m_buffer.getSize();
				if ( bytesToRead == 0 )
					break;

				readCount = theZipFile.read(m_buffer, bytesToRead);

				comprLeft -= bytesToRead;

				m_stream.next_in = (unsigned char *)(char*) m_buffer;
				m_stream.avail_in = bytesToRead;
			}

			DWORD toCopy;
			if ( m_method == 0 )
			{
				toCopy = m_stream.avail_out < m_stream.avail_in ? m_stream.avail_out : m_stream.avail_in;
				memcpy(m_stream.next_out, m_stream.next_in, toCopy);
				crc32File = crc32(crc32File, m_stream.next_out, toCopy);
				
				uncomprLeft -= toCopy;
				m_stream.avail_in -= toCopy;
				m_stream.avail_out -= toCopy;
				m_stream.next_out += toCopy;
				m_stream.next_in += toCopy;
				m_stream.total_out += toCopy;
				readTotalCount += toCopy;
			}
			else
			{
				DWORD total = m_stream.total_out;
				unsigned char *oldBuffer = m_stream.next_out;
				int err = inflate(&m_stream, Z_SYNC_FLUSH);
				toCopy = m_stream.total_out - total;
				crc32File = crc32(crc32File, oldBuffer, toCopy);

				uncomprLeft -= toCopy;
				readTotalCount += toCopy;

				if (    err != Z_OK
					 && err != Z_STREAM_END )
					return err;
			}
		} // End While

		aTarget.write(writeBuffer, readTotalCount);
	} 
	while (uncomprLeft); // readTotalCount == m_bufferSize);

	inflateEnd(&m_stream);

	return Z_OK;
}

int CZipStream::compress(CFileHandle &aZipFile, CFileHandle &anInputFile, int aLevel)
{
	DWORD comprLeft = 0;

	CBuffer readBuffer(m_bufferSize);
	m_stream.next_in = 0;
	m_stream.next_out = (unsigned char *)(char*) m_buffer;
	m_stream.avail_out = m_bufferSize;
    m_stream.total_in = 0;
    m_stream.total_out = 0;
    m_stream.opaque = 0;

	m_crc32 = 0;
	
    int err = deflateInit2(&m_stream, aLevel, Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);
//	ATLTRACE("deflateInit2 err: %d\n", err);

	if ( err == Z_OK )
	{
		DWORD readCount = 0;
		do
		{
			readCount = anInputFile.read(readBuffer, readBuffer.getSize());

			if ( readCount )
			{
				m_crc32 = crc32(m_crc32, (unsigned char*)(char *) readBuffer, readCount);
				m_stream.next_in = (unsigned char*)(char*) readBuffer;
				m_stream.avail_in = readCount;

				while (m_stream.avail_in > 0)
				{
					if (m_stream.avail_out == 0)
					{
						if ( aZipFile.write(m_buffer, comprLeft) != comprLeft )
							return Z_STREAM_ERROR;

						comprLeft = 0;
						m_stream.avail_out = m_bufferSize;
						m_stream.next_out = (unsigned char*)(char *) m_buffer;
					}
					
					if (m_method == Z_DEFLATED)
					{
						DWORD uTotal = m_stream.total_out;
						int err = deflate(&m_stream,  Z_NO_FLUSH);
						if ( err != Z_OK )
							break;
						comprLeft += m_stream.total_out - uTotal;
					}
					else
					{
						DWORD uToCopy = (m_stream.avail_in < m_stream.avail_out) 
							? m_stream.avail_in : m_stream.avail_out;
						
						memcpy(m_stream.next_out, m_stream.next_in, uToCopy);
						
						m_stream.avail_in -= uToCopy;
						m_stream.avail_out -= uToCopy;
						m_stream.next_in += uToCopy;
						m_stream.next_out += uToCopy;
						m_stream.total_in += uToCopy;
						m_stream.total_out += uToCopy;
						comprLeft += uToCopy;
					}
				}
			}
		}
		while(readCount == m_bufferSize);
	}

    m_stream.avail_in = 0;
    if (m_method == Z_DEFLATED)
	{
        while (err == Z_OK)
		{
			if (m_stream.avail_out == 0)
			{
				if ( aZipFile.write(m_buffer, comprLeft) != comprLeft )
					return Z_STREAM_ERROR;
				comprLeft = 0;
				m_stream.avail_out = m_bufferSize;
				m_stream.next_out = (unsigned char*) (char *) m_buffer;
			}
			DWORD uTotal = m_stream.total_out;
			err = deflate(&m_stream, Z_FINISH);
			comprLeft += m_stream.total_out - uTotal;
		}
	}		
		
	if (comprLeft > 0)
	{
		if ( aZipFile.write(m_buffer, comprLeft) != comprLeft )
			return Z_STREAM_ERROR;

	}
	
	if (m_method == Z_DEFLATED)
	{
		err = deflateEnd(&m_stream);
	}
	
	m_compressedSize = m_stream.total_out;
	m_uncompressedSize = m_stream.total_in;

	if (err == Z_STREAM_END)
		err = Z_OK;
		
	return err;
}
