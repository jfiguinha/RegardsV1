// Files.h : Declaration of the CFiles


#include "file.h"
#include <vector>

using namespace std;

#define CENTRALDIRSIZE 22

/////////////////////////////////////////////////////////////////////////////
// CFiles
class CFiles
{
public:
	CFiles()
	{
		m_thisDisk       = 0;
		m_diskWithCD     = 0;
		m_diskEntriesNo  = 0;
		m_entriesNumber  = 0;
		m_size           = 0;
		m_offset         = 0;
		m_commentSize    = 0;
		m_changed        = false;
		m_bytesBeforeZip = 0;
	}


/////////////////////////////////////////////////////////////////////////////
//Archives
//////////////////////////////////////////////////////////////////////////////

public : //METHOD
	long getReadOnly(bool *pVal);
	//long Close();
	long getName (string *pVal);
	long Create(string aName);
	long Open(string aName);

private:
	string m_name;
	CFileHandle m_archiveFile;
	bool m_readOnly;

/////////////////////////////////////////////////////////////////////////////////


// IFiles
public:
	CFichierZip ExtractFile(long m_lngItem);
	long getNbFiles();
	long AddFileByName( string fileName,  short level,  bool fullPath,  bool refresh);
	long getComment(string *pVal);
	long putComment( string newVal);
	//long Create(long aFileHandle);
	long Close();

	long Add(CFichierZip file);
	long Remove( int nIndex);

private:
	char m_signature[4];	//		end of central dir signature    4 bytes  (0x06054b50)
	WORD m_thisDisk;		//		number of this disk             2 bytes
	WORD m_diskWithCD;		//		number of the disk with the  
							//		start of the central directory	2 bytes
	WORD m_diskEntriesNo;	//		total number of entries in
							//		the central dir on this disk    2 bytes
	WORD m_entriesNumber;	//		total number of entries in
							//		the central dir                 2 bytes
	DWORD m_size;			//		size of the central directory   4 bytes
	DWORD m_offset;			//		offset of start of central
							//		directory with respect to
							//		the starting disk number        4 bytes
	WORD m_commentSize;		//		zipfile comment length          2 bytes
	string m_comment;

	CFileHandle m_zipFile;

	static char m_gszSignature[];
	static const int m_bufferSize;

	DWORD m_bytesBeforeZip;
	bool m_changed; // Indicates if the file needs to be stored (file added or removed).

	//Implementation du vecteur de fichier
	
	typedef vector<CFichierZip> VecFile;
	VecFile m_Vector;

	typedef VecFile::iterator VecFileIt;


private:	// Methods
	DWORD locate(void);
	void write(void);
	void setChanged(bool newChanged);
	short search(string name);
	long Read(long aFileHandle);
};

