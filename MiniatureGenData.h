

#ifndef __MINIATUREGENDATA__H__
#define __MINIATUREGENDATA__H__

class CItemInfo;

class CMiniatureGenData
{
public:
	CMiniatureGenData(void);
	~CMiniatureGenData(void);

	void SetItemInfo(CItemInfo * pItem);
	CItemInfo * GetItemInfo();

	void SetNumItemInfo(int iNumItem)
	{
		m_iNumItem = iNumItem;
	}

	int GetNumItemInfo()
	{
		return m_iNumItem;
	}

	void SetFilename(string stgFilename)
	{
		m_stgFilename = stgFilename;
	}

	string GetFilename()
	{
		return m_stgFilename;
	}

	void SetIconeSize(int iHauteur, int iLargeur)
	{
		m_iHauteur = iHauteur;
		m_iLargeur = iLargeur;
	}

	HBITMAP GetBitmapResult()
	{
		return hBitmap;
	}

	void SetBitmapResult(HBITMAP hBitmapResult)
	{
		hBitmap = (HBITMAP)CopyImage(hBitmapResult, IMAGE_BITMAP, 0, 0, 0);
	}

	void SetThreadId(DWORD dwThreadID)
	{
		m_dwThreadID = dwThreadID;
	}

	DWORD GetThreadId()
	{
		return m_dwThreadID;
	}

	int GetLargeur()
	{
		return m_iLargeur;
	}

	int GetHauteur()
	{
		return m_iHauteur;
	}

	bool GetIfFolder()
	{
		return m_bFolder;
	}

	void SetIfFolder(bool bValue)
	{
		m_bFolder = bValue;
	}

private:

	DWORD m_dwThreadID;
	CItemInfo * m_pItem;

	int m_iNumItem;
	string m_stgFilename;
	int m_iHauteur;
	int m_iLargeur;
	HBITMAP hBitmap;
	bool m_bStatus;
	bool m_bFolder;

};

#endif