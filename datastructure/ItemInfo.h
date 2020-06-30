

#ifndef __ITEMINFO__H__
#define __ITEMINFO__H__

#include <shlobj.h>

class CItemInfo
{
public:
	CItemInfo(void);
	~CItemInfo(void);

	CItemInfo(CItemInfo * p2);

	bool operator == (const CItemInfo &p2) const;

	char * GetParseName()
	{
		return m_szParseName;
	}

	void SetParseName(const char * cValue)
	{
		if(cValue != NULL)
		{
			int m_iSize = strlen(cValue)+1;
			if(m_szParseName != NULL)
				delete[] m_szParseName;
		
			m_szParseName = new char[m_iSize];
			StringCchCopy(m_szParseName,m_iSize,cValue);
		}
		else
		{
			if(m_szParseName != NULL)
				delete[] m_szParseName;

			m_szParseName = NULL;
		}
	}

	char * GetFileName()
	{
		return m_szFilename;
	}

	void SetFileName(const char * cValue)
	{
		if(cValue != NULL)
		{
			int m_iSize = strlen(cValue)+1;
			if(m_szFilename != NULL)
				delete[] m_szFilename;
		
			m_szFilename = new char[m_iSize];
			StringCchCopy(m_szFilename,m_iSize,cValue);
		}
		else
		{
			if(m_szFilename != NULL)
				delete[] m_szFilename;

			m_szFilename = NULL;
		}
	}

	void SetIPos(int iValue)
	{
		m_iPos = iValue;
	}

	int GetIPos()
	{
		return m_iPos;
	}

	void SetImage(int iValue)
	{
		iImage = iValue;
	}

	int GetImage()
	{
		return iImage;
	}

	void SetTypeDonne(int iValue)
	{
		m_iTypeDonne = iValue;
	}

	int GetTypeDonne()
	{
		return m_iTypeDonne;
	}

	void SetPidlFQ(LPITEMIDLIST lpItem);
	void SetPidlRel(LPITEMIDLIST lpItem);

	LPITEMIDLIST GetPidlFQ()
	{
		return pidlFQ;
	}

	LPITEMIDLIST GetPidlRel()
	{
		return pidlRel;
	}

	void SetShellFolder(LPSHELLFOLDER lpShellFolder)
	{
		pParentFolder = lpShellFolder;
	}

	LPSHELLFOLDER GetShellFolder()
	{
		return pParentFolder;
	}

	char * GetSize()
	{
		return m_lSize;
	}

	void SetSize(const char * cValue)
	{
		StringCchCopy(m_lSize,20,cValue);
	}

	char * GetDimension()
	{
		return m_lDimension;
	}

	void SetDimension(const char * cValue)
	{
		StringCchCopy(m_lDimension,20,cValue);
	}


	char * GetDate()
	{
		return m_lDate;
	}

	void SetDate(const char * cValue)
	{
		StringCchCopy(m_lDate,20,cValue);
	}


private:

	LPSHELLFOLDER  pParentFolder;
	LPITEMIDLIST   pidlFQ;
	LPITEMIDLIST   pidlRel;
	char * m_szParseName;
	char * m_szFilename;
	char m_lSize[20];
	char m_lDate[20];
	char m_lDimension[20];
	int iImage;
	int m_iPos;
	int m_iTypeDonne;
};


#endif