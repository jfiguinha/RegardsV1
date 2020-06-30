// File.h : Declaration of the CFile

#include "ZipStream.h"
#include <strsafe.h>
#include <string>
using namespace std;

#define FILEHEADERSIZE	46
#define LOCALFILEHEADERSIZE	30
#define VERSIONMADEBY 20

/////////////////////////////////////////////////////////////////////////////
// CFile

class CFichierZip
{

public:

	CFichierZip()
	{
		m_compressLevel  = 1;
		m_commentSize    = 0;
		m_method		 = 0;
		m_comprSize		 = 0;
		m_uncomprSize	 = 0;
		m_fileNameSize 	 = 0;
		m_extraFieldSize = 0;
		m_diskStart      = 0;
		m_internalAttr   = 0;
		m_externalAttr   = 0;
		m_bytesBeforeZip = 0;
		m_fullPath       = false;
		m_index          = -1; // means not in zip.
	}

	//Constructeur de copie

	CFichierZip(const CFichierZip & Source)
	{
		m_name = Source.m_name ;
		m_comment = Source.m_comment;
		StringCchCopy(m_signature,4,Source.m_signature);
		m_versionMadeBy = Source.m_versionMadeBy;
		m_versionNeeded = Source.m_versionNeeded;
		m_flag = Source.m_flag;		
		m_method = Source.m_method;		
		m_modTime = Source.m_modTime;		
		m_modDate = Source.m_modDate;		
		m_crc32= Source.m_crc32;		
		m_comprSize = Source.m_comprSize;	
		m_uncomprSize = Source.m_uncomprSize;
		m_fileNameSize = Source.m_fileNameSize;
		m_extraFieldSize = Source.m_extraFieldSize;
		m_commentSize = Source.m_commentSize;	
		m_diskStart = Source.m_diskStart;	
		m_internalAttr = Source.m_internalAttr;	
		m_externalAttr = Source.m_externalAttr;	
		m_offset = Source.m_offset;			
		m_extraField = Source.m_extraField;
		m_zipFile = Source.m_zipFile;
		m_bytesBeforeZip = Source.m_bytesBeforeZip;
		m_compressLevel = Source.m_compressLevel;
		m_fullPath = Source.m_fullPath;
		m_index = Source.m_index;
	}

public:

#ifdef NETSCAPE
#else
	long getModificationDate(DATE *pVal);
#endif
	//long putModificationDate(WORD newVal);
	//long ExtractToStream( IStream *aStream);
	long ExtractToString( string *strResult);
	long getFileNameInZip( string *pVal);
	long getIndex( short *pVal);
	long Shift( long Count);
	long Remove(bool lastFile,  long *Removed);
	long getFullPath(bool *pVal);
	long putFullPath(bool newVal);
	long getComment( string *pVal);
	long putComment( string pVal);
	long WriteHeader(short *aSize);
	long getCrc32( long *pVal);
	long getIsDirectory(bool *pVal);
	long getLevel( short *pVal);
	long putLevel( short newVal);
	long getUncompressedSize( long *pVal);
	long getCompressedSize( long *pVal);
	long Write( long aZipFileHandle,  long theBytesBeforeZip,  short anIndex);
	long getDirectory(bool *pVal);
	long Extract( string aPath,  string *pFileName);
	long Close();
	long Read( long aFileHandle,  long theBytesBeforeZip,  short anIndex);
	long getName(string * pVal);
	long putName(string pVal);

	inline long getSize(void) 
	{
		return FILEHEADERSIZE + m_extraFieldSize + m_fileNameSize + m_commentSize;
	}

private:
	
	string m_name;
	string m_comment;

	char m_signature[4];	//         central file header signature   4 bytes  (0x02014b50)
	WORD m_versionMadeBy;	//         version made by                 2 bytes
	WORD m_versionNeeded;	//         version needed to extract       2 bytes
	WORD m_flag;			//         general purpose bit flag        2 bytes
	WORD m_method;			//         compression method              2 bytes
	WORD m_modTime;			//         last mod file time              2 bytes
	WORD m_modDate;			//         last mod file date              2 bytes
	DWORD m_crc32;			//         crc-32                          4 bytes
	DWORD m_comprSize;		//         compressed size                 4 bytes
	DWORD m_uncomprSize;	//         uncompressed size               4 bytes
	WORD m_fileNameSize;	//         filename length                 2 bytes
	WORD m_extraFieldSize;	//         extra field length              2 bytes
	WORD m_commentSize;		//         file comment length             2 bytes
	WORD m_diskStart;		//         disk number start               2 bytes
	WORD m_internalAttr;	//         internal file attributes        2 bytes
	DWORD m_externalAttr;	//         external file attributes        4 bytes
	DWORD m_offset;			//         relative offset of local header 4 bytes
	CBuffer m_extraField;

	CFileHandle m_zipFile;
	int m_bytesBeforeZip;

	int m_compressLevel;
	static char m_gszSignature[];
	static char m_gszLocalSignature[];
	bool m_fullPath;
	short m_index;

private: // Methods

	bool locateFile(void);
	bool checkCrcAndSizes(char *pBuf);

	bool checkDirectory(string directory);
};


