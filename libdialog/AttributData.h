

#ifndef __ATTRIBUTDATA__H__
#define __ATTRIBUTDATA__H__

class CAttribut;
class CDialogCatalogueData;

class CAttributData
{
public:

	CAttributData(HINSTANCE hInstance);
	CAttributData(){};
	~CAttributData(void);

	void SetParam(CDialogCatalogueData * cDialogCatalogueData)
	{
		m_cDialogCatalogueData = cDialogCatalogueData;
	}

	inline void SetSelectItem(int iValue)
	{
		m_iSelectItem = iValue;
	}

	/*
	inline void SetNumItem(int iValue)
	{
		m_iNumItem = iValue;
	}*/

	void InitAttributDialog(HWND &hDlg, const int &iDataType, int &iNumItem);
	void GetAttributValue(CAttribut &m_AttributTemp, HWND hDlg);

protected:

	int m_iSelectItem;
	//int m_iNumItem;
	HINSTANCE hInstanceLocal;
	CDialogCatalogueData * m_cDialogCatalogueData;
};

#endif