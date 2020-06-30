#pragma once

#include <iconefile.h>
class CSqlLib;

class CSqlIconeFile
{
public:
	CSqlIconeFile(void);
	~CSqlIconeFile(void);



	HRESULT LoadIconeFileFolder(IconeFileVector * iconeFileVector, LPCWSTR lpFolderName);
	HRESULT LoadIconeFileAlbum(IconeFileVector * iconeFileVector, LPCWSTR lpAlbumName);

	HRESULT SaveIconeFileFolder(IconeFileVector * iconeFileVector, LPCWSTR lpFolderName);
	HRESULT SaveIconeFileAlbum(IconeFileVector * iconeFileVector, LPCWSTR lpAlbumName);

private:

	CSqlLib * _sqlLib;

};
