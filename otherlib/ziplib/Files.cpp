// Files.cpp : Implementation of CFiles

#include "Files.h"

/////////////////////////////////////////////////////////////////////////////
// CFiles
char CFiles::m_gszSignature[] = {0x50, 0x4b, 0x05, 0x06};
const int CFiles::m_bufferSize = 32768;

long CFiles::Add(CFichierZip file)
{
	// TODO: Add your implementation code here

	if ( m_zipFile.isValid() )
	{
		if ( m_Vector.size() == USHRT_MAX )
			return 1;

		string name;
		file.getFileNameInZip(&name);
		short index = search(name);
		if ( index != -1 )
			Remove(index);

		setChanged(true);

		HRESULT hr = file.Write(m_zipFile, m_bytesBeforeZip, m_Vector.size());
		if ( SUCCEEDED(hr) )
		{
			m_Vector.push_back(file);
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 1;
	}
}

long CFiles::Read(long aFileHandle)
{
	m_zipFile.attach(aFileHandle);

	DWORD readCount = 0;
	CBuffer buffer(CENTRALDIRSIZE);
	DWORD centerDirPos = locate();

	m_zipFile.seek(centerDirPos);

	if ( (readCount = m_zipFile.read(buffer, CENTRALDIRSIZE)) == -1 )
		return 1;

	if ( readCount == 0 )
		return 0;

	if ( readCount < CENTRALDIRSIZE )
		return 1;

	memcpy(&m_signature    , buffer, 4);
	memcpy(&m_thisDisk     , buffer + 4, 2);
	memcpy(&m_diskWithCD   , buffer + 6, 2);
	memcpy(&m_diskEntriesNo, buffer + 8, 2);
	memcpy(&m_entriesNumber, buffer + 10, 2);
	memcpy(&m_size         , buffer + 12, 4);
	memcpy(&m_offset       , buffer + 16, 4);
	memcpy(&m_commentSize  , buffer + 20, 2);

	if ( centerDirPos < m_offset + m_size )
	{
		return 1;
	}

	if ( m_commentSize > 0)
	{
		//USES_CONVERSION;
		CBuffer commentBuffer(m_commentSize + 1);
		readCount = m_zipFile.read(commentBuffer, commentBuffer.getSize());
		commentBuffer.setByte(readCount, '\0');
		m_comment = commentBuffer;
	}

	m_bytesBeforeZip = centerDirPos - m_size - m_offset;

	m_zipFile.seek(m_offset + m_bytesBeforeZip);
	
	//Vider le vecteur

	m_Vector.erase(m_Vector.begin(),m_Vector.end());

	for (int i = 0; i < m_entriesNumber; i++)
	{

		CFichierZip file;
		if ( file.Read((long) m_zipFile, m_bytesBeforeZip, i) != S_OK )
			return 1;
		
		m_Vector.push_back(file);
			
	}

	m_changed = false;

	return 0;
}

DWORD CFiles::locate(void)
{
	// maximum size of end of central dir record
	long maxRecordSize = 0xffff + CENTRALDIRSIZE;
	DWORD fileSize = m_zipFile.getSize();

	if ((DWORD) maxRecordSize > fileSize)
		maxRecordSize = fileSize;

	CBuffer buffer(m_bufferSize);

	long posInFile = 0;
	int read = 0;
	// backward reading
	while (posInFile < maxRecordSize)
	{
		posInFile = read + m_bufferSize;
		if (posInFile > maxRecordSize)
			posInFile = maxRecordSize;

		DWORD toRead = posInFile - read;
		DWORD readCount = 0;

		m_zipFile.seek(-posInFile, FILE_END);
		readCount = m_zipFile.read(buffer, toRead);
		// search from the very last bytes to prevent an error if inside archive 
		// there are packed other arhives
		for (int i = toRead - 4; i >=0 ; i--)
			if (!memcmp((char*)buffer + i, m_gszSignature, 4))	
				return fileSize - (posInFile - i);

		read += toRead - 3;

	}
	return 0;
}

long CFiles::Remove(int nIndex)
{
	if ( m_zipFile.isValid() )
	{
		if ( nIndex < 0 || nIndex >= m_Vector.size() )
			return 1;

		VecFileIt it = m_Vector.begin() + nIndex;
		setChanged(true);

		//CFile * filePrivate;

		//CComQIPtr<IFilePrivate> filePrivate(*it);

		bool lastFile = (nIndex == m_Vector.size()) ? true : false;
		long removedBytes = 0;

		m_Vector[nIndex].Remove(lastFile, &removedBytes);
		
		//filePrivate->Remove(lastFile, &removedBytes);

		m_Vector.erase(m_Vector.begin() + nIndex);
		
		if ( removedBytes > 0 )
		{
			for(it = m_Vector.begin() + nIndex; it !=  m_Vector.end(); it++)
			{
				//filePrivate = *it;
				it->Shift(removedBytes);
			}
		}
	}
	return 0;
}

long CFiles::Close()
{
	// TODO: Add your implementation code here
	setChanged(false);

	m_zipFile.detach();

	for(VecFileIt it = m_Vector.begin(); it != m_Vector.end(); it++)
	{
		//CComQIPtr<IFilePrivate> filePrivate(*it);
		//filePrivate->Close();
		it->Close();
	}
	m_Vector.clear();
	
	//m_Iterator = m_Vector.begin();


	//Archives

	if ( m_archiveFile.isValid() ) 
	{
		m_archiveFile.close();
		m_readOnly = false;
	}
	return 0;
}

void CFiles::setChanged(bool newChanged)
{
	// truncate the file if changed becomes true, write the file when false
	if ( newChanged == m_changed )
		return;

	m_changed = newChanged;
	if ( m_changed )
		m_zipFile.setSize(m_bytesBeforeZip + m_offset);
	else
		write();
}

void CFiles::write(void)
{
	//USES_CONVERSION;
	if ( m_comment.size() > USHRT_MAX )
	{
		m_comment.resize(USHRT_MAX);
	}
	m_commentSize = m_comment.size();
	m_entriesNumber = m_Vector.size();
	m_size = 0;

	m_thisDisk = 0;
	m_diskEntriesNo = m_entriesNumber;
	m_diskWithCD = 0;

	m_zipFile.seek(0, FILE_END);
	m_offset = m_zipFile.getPosition() - m_bytesBeforeZip;

	for(VecFileIt it = m_Vector.begin(); it != m_Vector.end(); it++)
	{
		//CComQIPtr<IFilePrivate> filePrivate(*it);
		short size = 0;
		it->WriteHeader(&size);
		m_size += size;
	}
	
	CBuffer buffer(CENTRALDIRSIZE + m_commentSize);
	memcpy(buffer, m_gszSignature, 4);
	memcpy(buffer + 4, &m_thisDisk, 2);
	memcpy(buffer + 6, &m_diskWithCD, 2);
	memcpy(buffer + 8, &m_diskEntriesNo, 2);
	memcpy(buffer + 10, &m_entriesNumber, 2);
	memcpy(buffer + 12, &m_size, 4);
	memcpy(buffer + 16, &m_offset, 4);
	memcpy(buffer + 20, &m_commentSize, 2);
	memcpy(buffer + 22, m_comment.c_str(), m_commentSize);

	m_zipFile.write(buffer, buffer.getSize());
}

/*
long CFiles::Create(long aFileHandle)
{
	m_zipFile.attach(aFileHandle);
	setChanged(true);
	return 0;
}
*/

long CFiles::getName(string *pVal)
{
	// TODO: Add your implementation code here

	*pVal = m_name;
	return 0;
}

long CFiles::Create(string aName)
{
	// TODO: Add your implementation code here
	//USES_CONVERSION;

	if ( m_archiveFile.isValid() )
	{
		Close();
	}

	m_name = aName;
	m_archiveFile.open(m_name, CREATE_ALWAYS);
	if ( ! m_archiveFile.isValid() )
	{
		m_name = "";
		return 1;
	}
	else
	{
		//m_files = new CFiles();
		//m_files->Create((long) m_archiveFile);
		m_zipFile.attach((long) m_archiveFile);
		setChanged(true);
	}
	return 0;
}

long CFiles::getComment(string *pVal)
{
	*pVal = m_comment.c_str();
	return 0;
}

long CFiles::putComment(string newVal)
{
	setChanged(true);
	m_comment = newVal;
	return 0;
}

long CFiles::AddFileByName(string fileName, short level, bool fullPath, bool refresh)
{
	// TODO: Add your implementation code here
	
	string name = fileName;

	if ( m_zipFile.isValid() )
	{
		WIN32_FIND_DATA findFileData;

		HANDLE handleFind = FindFirstFile(fileName.c_str(), &findFileData);
		if ( handleFind == INVALID_HANDLE_VALUE )
		{
			string strErr = "Could not find file " + name;
			return 1;
		}
			
		do
		{
			string stringName = name;

			bool addAllowed = true;

			short index = search(stringName);
			if ( index != -1 )
			{
				if ( refresh == true)
				{
					Remove(index);
				}
				else
				{
					addAllowed = false;
				}
			}

			if ( addAllowed )
			{
				CFichierZip pfile;
				//CComObject<CFile> *pfile;
				//CComObject<CFile>::CreateInstance(&pfile);

				pfile.putName(stringName);
				pfile.putLevel(level);
				pfile.putFullPath(fullPath);

				HRESULT hr = Add(pfile);
				if (FAILED(hr))
				{
					return hr;
				}
			}
		}
		while( FindNextFile(handleFind, &findFileData) );

		FindClose(handleFind);

		return 0;
	}
	else
	{
		return 1;
	}
}

short CFiles::search(string name)
{
	short index = -1;
	for(VecFileIt it = m_Vector.begin(); it !=  m_Vector.end(); it++)
	{
		//CComQIPtr<IFile> file(*it);
		string fileSearchName;
		it->getName(&fileSearchName);
		if ( fileSearchName == name )
		{
			it->getIndex(&index);
			break;
		}
	}
	return index;
}

long CFiles::getReadOnly(bool *pVal)
{
	// TODO: Add your implementation code here

	*pVal = m_readOnly;
	return 0;
}

long CFiles::Open(string aName)
{
	// TODO: Add your implementation code here

	if ( m_archiveFile.isValid() )
	{
		Close();
	}

	// Check for ReadOnly
	DWORD fileAttr = GetFileAttributes(aName.c_str());
	if (    fileAttr != 0xFFFFFFFF 
		 && ((fileAttr & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY ) )
	{
		m_readOnly = true;
		m_archiveFile.open(aName, OPEN_EXISTING, GENERIC_READ);
	}
	else
		m_archiveFile.open(aName);

	if ( !m_archiveFile.isValid() )
	{
		m_name = "";
		return 1;
	}
	else
	{
		m_name = aName;
		if (Read((long) m_archiveFile) == 0 )
			return 0;
		else
			return 1;
	}
}

long CFiles::getNbFiles()
{
	return m_Vector.size();
}

CFichierZip CFiles::ExtractFile(long m_lngItem)
{
	return m_Vector[m_lngItem];
}
