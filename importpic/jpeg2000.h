// J2K.h: interface for the CJ2K class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_J2K_H__66E2761C_7895_4630_984D_0CBAED03C40D__INCLUDED_)
#define AFX_J2K_H__66E2761C_7895_4630_984D_0CBAED03C40D__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CIBitmap;

#include "jasper\jasper.h"

class CxFile
{
public:
	CxFile(void) { };
	virtual ~CxFile() { };

	virtual bool	Close() = 0;
	virtual size_t	Read(void *buffer, size_t size, size_t count) = 0;
	virtual size_t	Write(const void *buffer, size_t size, size_t count) = 0;
	virtual bool	Seek(long offset, int origin) = 0;
	virtual long	Tell() = 0;
	virtual long	Size() = 0;
	virtual bool	Flush() = 0;
	virtual bool	Eof() = 0;
	virtual long	Error() = 0;
	virtual bool	PutC(unsigned char c)
		{
		// Default implementation
		size_t nWrote = Write(&c, 1, 1);
		return (bool)(nWrote == 1);
		}
	virtual long	GetC() = 0;
};

class CxIOFile : public CxFile
	{
public:
	CxIOFile(FILE* fp = NULL)
	{
		m_fp = fp;
		m_bCloseFile = (bool)(fp==0);
	}

	~CxIOFile()
	{
		Close();
	}
//////////////////////////////////////////////////////////
	bool Open(const char *filename, const char *mode)
	{
		if (m_fp) 
			return false;	// Can't re-open without closing first

		errno_t err;
		err  = fopen_s(&m_fp, filename, mode);

		if (err != 0) 
			return false;

		m_bCloseFile = true;

		return true;
	}
//////////////////////////////////////////////////////////
	virtual bool Close()
	{
		int iErr = 0;
		if ( (m_fp) && (m_bCloseFile) ){ 
			iErr = fclose(m_fp);
			m_fp = NULL;
		}
		return (bool)(iErr==0);
	}
//////////////////////////////////////////////////////////
	virtual size_t	Read(void *buffer, size_t size, size_t count)
	{
		if (!m_fp) return 0;
		return fread(buffer, size, count, m_fp);
	}
//////////////////////////////////////////////////////////
	virtual size_t	Write(const void *buffer, size_t size, size_t count)
	{
		if (!m_fp) return 0;
		return fwrite(buffer, size, count, m_fp);
	}
//////////////////////////////////////////////////////////
	virtual bool Seek(long offset, int origin)
	{
		if (!m_fp) return false;
		return (bool)(fseek(m_fp, offset, origin) == 0);
	}
//////////////////////////////////////////////////////////
	virtual long Tell()
	{
		if (!m_fp) return 0;
		return ftell(m_fp);
	}
//////////////////////////////////////////////////////////
	virtual long	Size()
	{
		if (!m_fp) return -1;
		long pos,size;
		pos = ftell(m_fp);
		fseek(m_fp, 0, SEEK_END);
		size = ftell(m_fp);
		fseek(m_fp, pos,SEEK_SET);
		return size;
	}
//////////////////////////////////////////////////////////
	virtual bool	Flush()
	{
		if (!m_fp) return false;
		return (bool)(fflush(m_fp) == 0);
	}
//////////////////////////////////////////////////////////
	virtual bool	Eof()
	{
		if (!m_fp) return true;
		return (bool)(feof(m_fp) != 0);
	}
//////////////////////////////////////////////////////////
	virtual long	Error()
	{
		if (!m_fp) return -1;
		return ferror(m_fp);
	}
//////////////////////////////////////////////////////////
	virtual bool PutC(unsigned char c)
	{
		if (!m_fp) return false;
		return (bool)(fputc(c, m_fp) == c);
	}
//////////////////////////////////////////////////////////
	virtual long	GetC()
	{
		if (!m_fp) return EOF;
		return getc(m_fp);
	}
//////////////////////////////////////////////////////////
protected:
	FILE *m_fp;
	bool m_bCloseFile;
	};

class CJ2K  
{
public:

	CJ2K();
	virtual ~CJ2K();

	int ReadJ2K(CIBitmap * m_ciBitmap,const char * szFileName);
	void GetDimensions(const char *fileName, UINT *width, UINT *height);

protected:

	class CxFileJas
	{
	public:
		CxFileJas(CxIOFile* pFile,jas_stream_t *stream)
		{
			if (stream->obj_) jas_free(stream->obj_);
			stream->obj_ = pFile;
			stream->ops_->close_ = JasClose;
			stream->ops_->read_  = JasRead;
			stream->ops_->seek_  = JasSeek;
			stream->ops_->write_ = JasWrite;
		}
		static int JasRead(jas_stream_obj_t *obj, char *buf, int cnt)
		{		return ((CxIOFile*)obj)->Read(buf,1,cnt); }
		static int JasWrite(jas_stream_obj_t *obj, char *buf, int cnt)
		{		return ((CxIOFile*)obj)->Write(buf,1,cnt); }
		static long JasSeek(jas_stream_obj_t *obj, long offset, int origin)
		{		return ((CxIOFile*)obj)->Seek(offset,origin); }
		static int JasClose(jas_stream_obj_t *obj)
		{		return ((CxIOFile*)obj)->Close(); }
	};
};

#endif // !defined(AFX_J2K_H__66E2761C_7895_4630_984D_0CBAED03C40D__INCLUDED_)
