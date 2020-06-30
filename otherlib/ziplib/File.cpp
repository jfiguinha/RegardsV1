// File.cpp : Implementation of CFile


#include "File.h"
#include <algorithm>
#include <direct.h>

#define BUFFER_SIZE 16384

/////////////////////////////////////////////////////////////////////////////
// CFile

char CFichierZip::m_gszSignature[] = {0x50, 0x4b, 0x01, 0x02};
char CFichierZip::m_gszLocalSignature[] = {0x50, 0x4b, 0x03, 0x04};

long CFichierZip::getName(string *pVal)
{
	string nameConvert = m_name;
	std::replace(nameConvert.begin(), nameConvert.end(), '/', '\\');
	*pVal = nameConvert;
	return 0;
}

long CFichierZip::Read(long aFileHandle, long theBytesBeforeZip, short anIndex)
{
	m_zipFile.attach(aFileHandle);
	m_index = anIndex;

	m_bytesBeforeZip = theBytesBeforeZip;

	CBuffer buffer(FILEHEADERSIZE);
	DWORD readCount = m_zipFile.read(buffer, FILEHEADERSIZE);

	memcpy(&m_signature,		buffer, 4);
	memcpy(&m_versionMadeBy,	buffer + 4, 2);
	memcpy(&m_versionNeeded,	buffer + 6, 2);
	memcpy(&m_flag,				buffer + 8, 2);
	memcpy(&m_method,			buffer + 10, 2);
	memcpy(&m_modTime,			buffer + 12, 2);
	memcpy(&m_modDate,			buffer + 14, 2);
	memcpy(&m_crc32,			buffer + 16, 4);
	memcpy(&m_comprSize,		buffer + 20, 4);
	memcpy(&m_uncomprSize,		buffer + 24, 4);
	memcpy(&m_fileNameSize,		buffer + 28, 2);
	memcpy(&m_extraFieldSize,	buffer + 30, 2);
	memcpy(&m_commentSize,		buffer + 32, 2);
	memcpy(&m_diskStart,		buffer + 34, 2);
	memcpy(&m_internalAttr,		buffer + 36, 2);
	memcpy(&m_externalAttr,		buffer + 38, 4);
	memcpy(&m_offset,			buffer + 42, 4);

	if (memcmp(m_signature, m_gszSignature, 4) != 0)
	{
		Close();
		return 1;
	}

	CBuffer readName(m_fileNameSize + 1);
	readCount = m_zipFile.read(readName, m_fileNameSize);
	if ( readCount != -1 )
	{
		readName.setByte(readCount, '\0');
		//m_name = A2T(readName);
		m_name = readName;
	}

	if ( m_extraFieldSize > 0 )
	{
		m_extraField.allocate(m_extraFieldSize);
		readCount = m_zipFile.read(m_extraField, m_extraFieldSize);
	}

	if ( m_commentSize > 0 )
	{
		CBuffer comment(m_commentSize + 1);
		readCount = m_zipFile.read(comment, m_commentSize);
		if ( readCount != -1 )
		{
			comment.setByte(readCount, '\0');
			//m_comment = A2T(comment);
			m_comment = comment;
		}
	}

	//ATLTRACE("Current Position %ld for %s\n", m_zipFile.getPosition(), m_name.c_str());

	m_fullPath = true; // Default for extraction

	return 0;
}

long CFichierZip::Close()
{
	m_zipFile.detach();
	m_index = -1;
	return 0;
}

long CFichierZip::Extract(string aPath, string *pFileName)
{
	//USES_CONVERSION;
	string path = aPath;
	string errorMsg;

	if ( ! m_zipFile.isValid() )
		return 1;

	DWORD attr = GetFileAttributes(path.c_str());
	if ( attr == 0xFFFFFFFF )
	{
		errorMsg = "Invalid path:\n";
		errorMsg += path;
		return 1;
	}

	if ( (attr & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
	{
		errorMsg = "Invalid path:\n";
		errorMsg += path;
		return 1;
	}

	if ( *path.rbegin() != TEXT('\\') )
		path += '\\';

	string target = path;

	bool directory;
	getDirectory(&directory);

	if ( directory == true)
	{
		target += m_name;
		CreateDirectory(target.c_str(), NULL);
	}
	else
	{
		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_DIR];
		TCHAR fname[_MAX_FNAME];
		TCHAR ext[_MAX_EXT];

		_splitpath_s(m_name.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname,_MAX_FNAME, ext,_MAX_EXT);

		if ( m_fullPath == true )
		{
			target += dir;
			std::replace(target.begin(), target.end(), '/', '\\');
			if ( ! checkDirectory(target) )
			{
				errorMsg = "Can't create directory:\n";
				errorMsg += target;
				return 1;
			}
		}

		target += fname;
		target += ext;

		CFileTarget file(target, m_externalAttr);
		if ( !file.isValid() )
		{
			errorMsg = "Unable to create the file:\n";
			errorMsg += target;
			return 1;
		}

		// Locate the pointer to the compressed file
		if ( ! locateFile() )
		{
			errorMsg = "Can't find the file in the ziparchive:\n";
			errorMsg += target;
			return 1;
		}

		CZipStream stream(m_method, 0, m_uncomprSize, m_comprSize);
		if ( stream.extract(m_zipFile, file) != Z_OK )
		{
			errorMsg = "Error occurred during extract:\n";
			errorMsg += target;
			return 1;
		}
		else
			*pFileName = target;
			
		//CComstring(target.c_str()).CopyTo(pFileName);
	}
	return 0;
}

long CFichierZip::getDirectory(bool *pVal)
{
	// Returns true when this file is a directory.
	*pVal = false;
	if ( ! m_zipFile.isValid() )
	{
		return 1;
	}
	else
	{
		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_DIR];
		TCHAR fname[_MAX_FNAME];
		TCHAR ext[_MAX_EXT];

		_splitpath_s(m_name.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname,_MAX_FNAME, ext,_MAX_EXT);

		//TCHAR fname[_MAX_FNAME];
		//_splitpath(m_name.c_str(), NULL, NULL, fname, NULL);
		if ( strlen(fname) == 0 )
			*pVal = true;
	}

	return 0;
}

bool CFichierZip::locateFile()
{
	DWORD readCount;
	
	CBuffer buffer(LOCALFILEHEADERSIZE);

	m_zipFile.seek(m_bytesBeforeZip + m_offset);

	if ( (readCount = m_zipFile.read(buffer, LOCALFILEHEADERSIZE)) == -1 )
		return false;
	if (memcmp(buffer, m_gszLocalSignature, 4) != 0)
		return false;

	bool bIsDataDescr = (((WORD)*(buffer + 6)) & 8) != 0;

	if (   ( memcmp(buffer + 8, &m_method, 2)        != 0          )
		|| ( m_method 
		&& ( m_method                                != Z_DEFLATED ))
		|| ( memcmp(buffer + 26, &m_fileNameSize, 2) != 0          ))
		return false;

	if ( !bIsDataDescr )
		if ( !checkCrcAndSizes(buffer + 14) )
			return false;

	WORD iLocExtrFieldSize = (WORD)*(buffer + 28);
	m_zipFile.seek(m_fileNameSize + iLocExtrFieldSize, FILE_CURRENT);

	return true;
}

bool CFichierZip::checkCrcAndSizes(char *pBuf)
{
	return    (memcmp(pBuf, &m_crc32, 4)           == 0)
		   && (memcmp(pBuf + 4, &m_comprSize, 4)   == 0)
		   && (memcmp(pBuf + 8, &m_uncomprSize, 4) == 0);
}

// Writes a NEW file to the zip
long CFichierZip::Write(long aZipFileHandle, long theBytesBeforeZip, short anIndex)
{
	//USES_CONVERSION;

	if ( m_zipFile.isValid() )
		return 1;

	m_index = anIndex;
	string fullFileName = m_name; // Needed for opening the file

	// Code written by Frederic St-Laurent
	// Get the file attributes
	m_internalAttr = GetFileAttributes( fullFileName.c_str() );
	// Get the file last write time
	WIN32_FIND_DATA FindData;
	FindFirstFile( fullFileName.c_str(), &FindData );
	FileTimeToDosDateTime(&FindData.ftLastWriteTime, &m_modDate, &m_modTime);
	// End Code written by Frederic St-Laurent

	// Replace all \ by / in the filename

	std::replace(m_name.begin(), m_name.end(), '\\', '/');

	memcpy(m_signature, m_gszSignature, 4);
	m_versionMadeBy = VERSIONMADEBY;
	m_versionNeeded = 20;
	m_method = Z_DEFLATED;
	m_flag = 0;

	switch(m_compressLevel)
	{
	case 1:
		m_flag |= 6;
		break;
	case 2:
		m_flag |= 4;
		break;
	case 8:
	case 9:
		m_flag |= 2;
		break;
	}
	m_crc32 = 0;
	m_uncomprSize = 0;
	m_comprSize = 0;

	string name;
	getFileNameInZip(&name);
	m_name = name;

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	
	string strStoredName;

	// Split the file name into parts
	_splitpath_s(fullFileName.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname,_MAX_FNAME, ext,_MAX_EXT);
	
	// Store the full file path if we need to
	if (m_fullPath == true)
	{
		strStoredName = dir;
		strStoredName += fname;
		strStoredName += ext;
	}
	else
	{
		strStoredName = fname;
		strStoredName += ext;
	}

	if ( strStoredName.size() > USHRT_MAX )
		return 1;

	m_fileNameSize = strStoredName.size();

	m_extraFieldSize = m_extraField.getSize();

	if (m_comment.size() > USHRT_MAX)
		m_comment.resize(USHRT_MAX);
	m_commentSize = m_comment.size();

	DWORD localSize = LOCALFILEHEADERSIZE + m_extraFieldSize + m_fileNameSize;
	CBuffer buffer(localSize);
	memcpy(buffer, m_gszLocalSignature, 4);
	memcpy(buffer + 4, &m_versionNeeded, 2);
	memcpy(buffer + 6, &m_flag, 2);
	memcpy(buffer + 8, &m_method, 2);
	memcpy(buffer + 10, &m_modTime, 2);
	memcpy(buffer + 12, &m_modDate, 2);
	memcpy(buffer + 14, &m_crc32, 4);
	memcpy(buffer + 18, &m_comprSize, 4);
	memcpy(buffer + 22, &m_uncomprSize, 4);
	memcpy(buffer + 26, &m_fileNameSize, 2);
	memcpy(buffer + 28, &m_extraFieldSize, 2);
	memcpy(buffer + 30, strStoredName.c_str(), m_fileNameSize);
	memcpy(buffer + 30 + m_fileNameSize, m_extraField, m_extraFieldSize);

	m_zipFile.attach(aZipFileHandle);

	m_zipFile.seek(0, FILE_END);
	m_zipFile.write(buffer, buffer.getSize());

	m_offset = m_zipFile.getPosition() - localSize;

	CZipStream stream(m_method);



	if ((m_internalAttr & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
	{

	}
	else
	{
		CFileHandle inputFile;
		inputFile.open(fullFileName.c_str(), OPEN_EXISTING, GENERIC_READ);
		if ( inputFile.isValid() )
		{
			if ( stream.compress(m_zipFile, inputFile, m_compressLevel) != Z_OK )
			{
				return 1;
			}
		}
		else
		{
			string errorMsg = "Can't open file:\n";
			errorMsg += fullFileName;
			return 1;
		}
	}

	m_crc32 = stream.getCrc32();
	m_comprSize = stream.getCompressedSize();
	m_uncomprSize = stream.getUncompressedSize();

	buffer.allocate(12);
	memcpy(buffer, &m_crc32, 4);
	memcpy(buffer + 4, &m_comprSize, 4);
	memcpy(buffer + 8, &m_uncomprSize, 4);

	m_zipFile.seek(m_offset + 14);

	m_offset -= m_bytesBeforeZip;
	m_zipFile.write(buffer, 12);

	return 0;
}

long CFichierZip::getCompressedSize(long *pVal)
{
	// TODO: Add your implementation code here

	*pVal = m_comprSize;

	return 0;
}

long CFichierZip::getUncompressedSize(long *pVal)
{
	// TODO: Add your implementation code here

	*pVal = m_uncomprSize;

	return 0;
}


long CFichierZip::getLevel(short *pVal)
{
	// TODO: Add your implementation code here
	*pVal = (short) m_compressLevel;
	return 0;
}

long CFichierZip::putLevel(short newVal)
{
	// TODO: Add your implementation code here
	
	if ( ! m_zipFile.isValid() )
	{
		m_compressLevel = newVal;
		return 0;
	}
	else
	{
		return S_FALSE;
	}
}

long CFichierZip::getIsDirectory(bool *pVal)
{
	// TODO: Add your implementation code here
	
	*pVal = ((GetFileAttributes(m_name.c_str()) & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
	return 0;
}

long CFichierZip::getCrc32(long *pVal)
{
	// TODO: Add your implementation code here

	*pVal = m_crc32;
	return 0;
}

long CFichierZip::WriteHeader(short *aSize)
{
	//USES_CONVERSION;
	*aSize = getSize();
	CBuffer buffer(*aSize);
	memcpy(buffer, m_gszSignature, 4);
	memcpy(buffer + 4, &m_versionMadeBy, 2);
	memcpy(buffer + 6, &m_versionNeeded, 2);
	memcpy(buffer + 8, &m_flag, 2);
	memcpy(buffer + 10, &m_method, 2);
	memcpy(buffer + 12, &m_modTime, 2);
	memcpy(buffer + 14, &m_modDate, 2);
	memcpy(buffer + 16, &m_crc32, 4);
	memcpy(buffer + 20, &m_comprSize, 4);
	memcpy(buffer + 24, &m_uncomprSize, 4);
	memcpy(buffer + 28, &m_fileNameSize, 2);
	memcpy(buffer + 30, &m_extraFieldSize, 2);
	memcpy(buffer + 32, &m_commentSize, 2);
	memcpy(buffer + 34, &m_diskStart, 2);
	memcpy(buffer + 36, &m_internalAttr, 2);
	memcpy(buffer + 38, &m_externalAttr, 4);
	memcpy(buffer + 42, &m_offset, 4);
	memcpy(buffer + 46, m_name.c_str(), m_fileNameSize);

	if (m_extraFieldSize)
		memcpy(buffer + 46 + m_fileNameSize, m_extraField, m_extraFieldSize);

	if (m_commentSize)
	{
	memcpy(buffer + 46 + m_fileNameSize + m_extraFieldSize, m_comment.c_str(), m_commentSize);
	}
	m_zipFile.write(buffer, *aSize);

	return 0;
}

long CFichierZip::getComment(string *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_comment;
	//CComstring(m_comment.c_str()).CopyTo(pVal);
	return 0;
}

long CFichierZip::getFullPath(bool *pVal)
{
	// TODO: Add your implementation code here

	*pVal = m_fullPath;
	return 0;
}

long CFichierZip::putFullPath(bool newVal)
{
	// TODO: Add your implementation code here

	m_fullPath = newVal;
	return 0;
}

long CFichierZip::putComment(string pVal)
{
	//USES_CONVERSION;
	m_comment = pVal;
	return 0;
}

long CFichierZip::putName(string pVal)
{
	//USES_CONVERSION;
	m_name = pVal;
	return 0;
}

long CFichierZip::Remove(bool LastFile, long *Removed)
{
	// TODO: Add your implementation code here

	*Removed = 0;
	if ( m_zipFile.isValid() )
	{
		if ( LastFile == true )
		{
			m_zipFile.setSize(m_offset + m_bytesBeforeZip);
		}
		else
		{
			DWORD start = m_offset + m_bytesBeforeZip;
			DWORD end = m_offset + m_bytesBeforeZip + m_comprSize;
			DWORD bytesToCopy = m_zipFile.getSize() - end;

			CBuffer buffer(BUFFER_SIZE);
			if ( bytesToCopy > BUFFER_SIZE )
			{
				bytesToCopy = BUFFER_SIZE;
			}
			
			DWORD totalWritten = 0;
			DWORD readCount = 0;

			do
			{
				m_zipFile.seek(end + totalWritten);
				readCount = m_zipFile.read(buffer, bytesToCopy);
				if ( readCount > 0 )
				{
					m_zipFile.seek(start + totalWritten);
					m_zipFile.write(buffer, readCount);
				}
				totalWritten += readCount;
			}
			while(readCount == bytesToCopy);

			*Removed = end - start;
			m_zipFile.setSize(m_zipFile.getSize() - *Removed);
		}
		Close();
	}

	return 0;
}

long CFichierZip::Shift(long Count)
{
	// TODO: Add your implementation code here
	m_offset -= Count;
	m_index--;
	return 0;
}

long CFichierZip::getIndex(short *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_index;
	return 0;
}

long CFichierZip::getFileNameInZip(string *pVal)
{
	// TODO: Add your implementation code here
	//USES_CONVERSION;

	string name;

	char fileName[_MAX_FNAME];
	char ext[_MAX_EXT];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];

	_splitpath_s(m_name.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fileName,_MAX_FNAME, ext,_MAX_EXT);
	
	if ( m_fullPath == true )
	{
		name = drive;
		name += dir;
	}

	name += fileName;
	name += ext;

	*pVal = name;

	return 0;
}

long CFichierZip::ExtractToString(string *strResult)
{
	// TODO: Add your implementation code here
	bool directory;
	getDirectory(&directory);

	if ( directory == true )
		return 1;

	if ( ! locateFile() )
		return 1;

	CZipStream stream(m_method, 0, m_uncomprSize, m_comprSize);
	CStringTarget target;
	if ( stream.extract(m_zipFile, target) != Z_OK )
		return 1;
	else
		*strResult = target.getString();

	return 0;
}

/*
long CFichierZip::ExtractToStream(IStream *aStream)
{
	// TODO: Add your implementation code here
	bool directory;
	getDirectory(&directory);

	if ( directory == true )
		return 1;

	if ( ! locateFile() )
		return 1;

	CZipStream stream(m_method, 0, m_uncomprSize, m_comprSize);
	CStreamTarget target;
	target.setStream(aStream);
	if ( stream.extract(m_zipFile, target) != Z_OK )
		return 1;

	return 0;
}*/
#ifdef NETSCAPE
#else
long CFichierZip::getModificationDate(DATE * pVal)
{
	// TODO: Add your implementation code here
	//DosDateTimeToVariantTime(m_modDate, m_modTime, pVal);

	DosDateTimeToVariantTime(m_modDate, m_modTime, pVal);

	/*
	pVal = m_modDate;
	pVal += " ";
	pVal += m_modTime;*/

	return 0;
}
#endif

/*
long CFichierZip::putModificationDate(WORD newVal)
{
	// TODO: Add your implementation code here
	VariantTimeToDosDateTime(newVal, &m_modDate, &m_modTime);
	return 0;
}*/

bool CFichierZip::checkDirectory(string directory)
{
	string dir(directory);
	int pos = dir.find_last_of('\\');

	if ( pos != string::npos )
		dir.erase(dir.begin() + pos, dir.end());
	else
		return true;

	TCHAR curPath[_MAX_PATH];   /* Get the current working directory: */
	if (!_getcwd(curPath, _MAX_PATH))
		return false;
	if (! _chdir(dir.c_str()) )	// retruns 0 if error
		return true;
	_chdir(curPath);

	if ( ! checkDirectory(dir) )
		return false;

	//ATLTRACE("Directory: %s\n", dir.c_str());
	if ( ! CreateDirectory(dir.c_str(), NULL) )
		return false;

	return true;
}
