// Buffer.h: interface for the CBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "windows.h"
#include <string>

using namespace std;

class CExtractTarget; 
class CFileHandle;  
class CBuffer; 
class CFileTarget;
class CStringTarget;

/**
* Buffer class.
*/
class CBuffer  
{
public:
	CBuffer()
	{
		m_buffer = NULL;
		m_size = 0;
	}
	CBuffer(DWORD theSize)
	{
		m_size = theSize;
		m_buffer = ( theSize > 0 ) ? new char[theSize] : NULL;
	}
	virtual ~CBuffer()
	{
		if ( m_buffer != NULL )
			delete[] m_buffer;
	}
	inline DWORD getSize(void)
	{
		return m_size;
	}
	inline void allocate(DWORD theSize)
	{
		if ( m_buffer != NULL )
		{
			delete[] m_buffer;
		}
		m_size = theSize;
		m_buffer = ( theSize > 0 ) ? new char[theSize] : NULL;
	}
	inline bool isAllocated(void)
	{
		return (m_buffer != NULL);
	}

	inline operator char*()
	{
		return m_buffer;
	}

	inline void setByte(int aPosition, char aValue)
	{
		if (    isAllocated()
			 && aPosition >= 0
			 && aPosition < m_size )
		{
			m_buffer[aPosition] = aValue;
		}
	}

private:
	char *m_buffer;
	DWORD m_size;
};


class CExtractTarget  
{
public:
	CExtractTarget() {}
	virtual void write(CBuffer &aBuffer, const DWORD aSize) = 0;
	virtual ~CExtractTarget() {}

};

class CStringTarget : public CExtractTarget  
{
public:
	CStringTarget(){};
	virtual ~CStringTarget(){};

	void write(CBuffer &aBuffer, const DWORD aSize)
	{
	//	USES_CONVERSION;
		m_string += aBuffer;
	};

	string getString(void) const { return m_string; }
private:
	string m_string;
};


/**
* A helper class for using a HANDLE for files
*/
class CFileHandle  
{
public:
	
	CFileHandle() : m_fileHandle(NULL), m_attached(false)
	{
	}

	CFileHandle(CFileHandle &aCopy)
	{
		m_fileHandle = aCopy.m_fileHandle;
		m_attached = true;
	}

	/**
	* Checks if the filehandle is valid
	* @return true when then filehandle is valid, false when it is not.
	*/
	inline bool isValid()
	{
		return (    m_fileHandle != NULL 
			     && m_fileHandle != INVALID_HANDLE_VALUE); 
	} 

	/**
	* Attaches a CFileHandle. A filehandle that is attached will not be closed when destroyed
	* @param aFileHandle CFileHandle
	*/
	inline void attach(const CFileHandle &aFileHandle)
	{
		m_fileHandle = aFileHandle.m_fileHandle;
		m_attached = true;
	}

	inline void attach(const HANDLE &aFileHandle)
	{
		m_fileHandle = aFileHandle;
		m_attached = true;
	}

	inline void attach(const long &aFileHandle)
	{
		m_fileHandle = reinterpret_cast<HANDLE>(aFileHandle);
		m_attached = true;
	}

	/**
	* Detaches a CFileHandle.
	*/
	inline void detach(void)
	{
		if ( m_attached )
		{
			m_fileHandle = NULL;
			m_attached = false;
		}
	}

	/**
	* Returns the filesize
	* @return DWORD The size of the file
	*/
	inline DWORD getSize()
	{
		return GetFileSize(m_fileHandle, NULL);
	}

	/**
	* Sets the size of the file
	* @param newSize DWORD The new size of the file
	*/
	inline void setSize(DWORD newSize)
	{
		seek(newSize);
		SetEndOfFile(m_fileHandle);
	}

	/**
	* Returns the current position in the file 
	* @return long The current position
	*/
	inline long getPosition(void)
	{
		DWORD position = SetFilePointer(m_fileHandle, 0, NULL, FILE_CURRENT);
		return position;
	}
	/**
	* Opens a file. When a file was already open, it will be closed first.
	* @param aName cstring The name of the file
	* @param dwCreateOption DWORD See for more information to CreateFile
	* @param dwDesiredAccess DWORD See for more information to CreateFile
	* @param dwShareMode DWORD See for more information to CreateFile
	* @return bool When open succeeded the method returns true
	*/
	inline bool open(string aName, 
					 DWORD dwCreateOption      = OPEN_EXISTING,
					 DWORD dwDesiredAccess     = GENERIC_READ | GENERIC_WRITE,
					 DWORD dwShareMode         = FILE_SHARE_READ,
					 DWORD dwFlagAndAttributes = FILE_ATTRIBUTE_NORMAL)
	{
		if ( ! m_attached )
			close();

		m_fileHandle = CreateFile(aName.c_str(), dwDesiredAccess, dwShareMode,
								  NULL, dwCreateOption, dwFlagAndAttributes, NULL);
		m_attached = false;
		return isValid();
	}

	/**
	* Closes the file.
	*/
	inline void close(void)
	{
		if ( isValid() )
		{
			CloseHandle(m_fileHandle);
			m_fileHandle = NULL;
		}
	}

	inline operator long()
	{
		return (long) m_fileHandle; 
	}

	/**
	* Reads aCount bytes from the file
	* @param aBuffer void* The buffer for receiving the bytes. Make sure it's large enough.
	* @param aCount int Number of bytes to read from the file
	* @return Number of bytes actually read. -1 if an error occured.
	*/
	inline DWORD read(void* aBuffer, DWORD aCount)
	{
		DWORD readCount = -1;
		if ( isValid() )
		{
			ReadFile(m_fileHandle, aBuffer, aCount, &readCount, NULL);
		}
		return readCount;
	}

	/**
	* Writes the buffer to the file.
	* @param aBuffer void* The buffer to write
	* @param aCount DWORD The number of bytes of the buffer to write
	* @return DWORD The number of bytes actually written to the file
	*/
	inline DWORD write(void *aBuffer, DWORD aCount)
	{
		DWORD writeCount = -1;
		if ( isValid() )
		{
			WriteFile(m_fileHandle, aBuffer, aCount, &writeCount, NULL);
		}
		return writeCount;
	}
	/**
	* Sets the filepointer
	* @param anOffset DWORD
	* @param aStart DWORD
	*/
	inline bool seek(DWORD anOffset, DWORD aStart = FILE_BEGIN)
	{
//		ATLTRACE("Seek %ld , from %ld\n", anOffset, aStart);
		if ( SetFilePointer(m_fileHandle, anOffset, NULL, aStart) != 0xFFFFFFFF )
			return true;
		else
		{
			//ATLTRACE("Invalid seek ! OffSet: %ld, Start: %d", anOffset, aStart);
			return false;
		}
	}

	inline virtual ~CFileHandle() 
	{
		if ( ! m_attached )
			close();
	}

private:
	HANDLE m_fileHandle;
	bool m_attached;
};

class CFileTarget : public CExtractTarget  
{
public:
	CFileTarget(const string &fileName, const DWORD anExternalAttr)
	{
		m_file.open(fileName.c_str(), CREATE_ALWAYS, GENERIC_WRITE, 0, anExternalAttr);
	};

	void write(CBuffer &aBuffer, const DWORD aSize)
	{
		m_file.write(aBuffer, aSize);
	};

	inline bool isValid() { return m_file.isValid(); }
	virtual ~CFileTarget(){};
private:
	CFileHandle m_file;
};

