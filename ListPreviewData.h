

#ifndef __LISTPREVIEWDATA__H__
#define __LISTPREVIEWDATA__H__

class CStatusBar;
class CParameter;
#include <IconeFile.h>

class CListPreviewData
{
public:
	CListPreviewData(void);
	~CListPreviewData(void);


	void CreateHImageList(IconeFileVector &iconeFileVector);
	void CreateFolderIcone();
	int ChargeThumbmail(const char * Localisation,IconeFileVector &iconeFileVector);
	int SaveThumbmail(const char * Localisation,IconeFileVector &iconeFileVector);
	int ChargeDatabaseFolder(const char * cDirectory,const char * Localisation,IconeFileVector &iconeFileVector);
	int SauverDatabaseFolder(const char * cDirectory,IconeFileVector &iconeFileVector);



	int GetFileDataBase()
	{
		return iFileDatabase;
	}

	void SetFileDataBase(int iValue)
	{
		iFileDatabase = iValue;;
	}

	int GetAffichageVolee()
	{
		return iAffichageAlaVolee;
	}

	void SetAffichageVolee(int iValue)
	{
		iAffichageAlaVolee = iValue;
	}

	void UpdateData();

	int GetHauteur()
	{
		return iHauteur;
	}

	int GetLargeur()
	{
		return iLargeur;
	}

	int GetNumFolder()
	{
		return iNumFolder;
	}

	void SetThumbnailSize(const int &iWidth, const int &iHeight);

	HIMAGELIST hImgThumbmail;

private:

	int ChargeData(const wchar_t * FilePath, const wchar_t * FolderPath, const wchar_t * m_chStorageName, const wchar_t * m_chStreamName, const int &iTypeData,IconeFileVector &iconeFileVector);
	int SaveData(const wchar_t * cFilePath, const wchar_t * FolderPath, const wchar_t * m_chStorageName, const wchar_t * m_chStreamName, const int &iTypeData,IconeFileVector &iconeFileVector);

	bool m_bLoadData;
	int iLargeur;
	int iHauteur;
	int iMarge;
	int iAffichageAlaVolee;
	int iFileDatabase;
	int iNumFolder;
	CParameter * m_cParameter;
};

#endif