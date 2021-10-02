

#ifndef __PARAMETER__H__
#define __PARAMETER__H__

#include <string>
using namespace std;

class CParameter
{
public:
	CParameter(void);
	~CParameter(void);

	static void SetNbScreen(const int &iNbScreen);
	static int GetNbScreen();
	static int GetInfosScreen(int iNumScreen,RECT &rectScreen);
	static void SetInfosScreen(const int &iNumScreen,RECT &rectScreen);

	static long GetGpGpu();
	static void SetGpGpu(const bool &m_bValue);

	//Option pour la navigation
	int GetOptionAffichage(int &m_bHauteQualite,bool &m_bAutoSize, bool &bRawFile);
	int SetOptionAffichage(const int &m_bHauteQualite,const bool &m_bAutoSize,const bool &bRawFile);
	
	//Option pour l'exploreur
	int GetOptionExploration(int &m_bHauteQualite,bool &m_bImage, char * m_szDirectory,const int &iDirectorySize, int &iModeAffichage);
	int SetOptionExploration(const int &m_bHauteQualite,const bool &m_bImage,const char * m_szDirectory,const int &iModeAffichage);

	//Option pour le traitement
	void SetOptionTraitement(int iFiltre);
	int GetOptionTraitement();

	//Option pour les miniatures
	int GetOptionMiniature(int &iLargeur,int &iHauteur);
	int SetOptionMiniature(const int &iLargeur, const int &iHauteur);

	//Option pour les miniatures
	int GetSaveOptionMiniature(int &bSave);
	int SetSaveOptionMiniature(const int &bSave);
	string GetThumbnailDirectory();


	string GetMKisofsFile();
	string GetCDRecordFile();

	//Information concernant le stockage du fichier des albums
	int GetOptionAlbumDir(char * m_szDirectory,const int &iDirectorySize);
	int SetOptionAlbumDir(char * m_szDirectory);

	int SetOptionDirectory(const char * m_szDirectory);

	string GetBurningDirectory();

	long GetStartDiaporama();
	void SetStartDiaporama(const bool &m_bValue);

	static int GetOptionDiaporama(int &iSequence,int &iDelai, bool &bAutoSize, int &bQualite,bool &bFullscreen, bool &b3D, int &i3DQuality);
	static int SetOptionDiaporama(const int &iSequence,const int &iDelai, const bool &bAutoSize, const int &bQualite,const bool &bFullscreen,const bool &b3D,const int &i3DQuality);

	int SetOptionAffichage3D(const int &iFiltre);
	int GetOptionAffichage3D();

	static int SaveWindowPlacement(const char * RegistryKey,HWND hWnd);
	static int LoadWindowPlacement(const char * RegistryKey,HWND hWnd);
	static int GetWindowPlacement(const char * RegistryKey,RECT &rectScreen);

	static int SetNbThread(int &iNbThread);
	static int GetNbThread(int &iNbThread);


private:

	int GetDefaultAlbumDir(char * m_szDirectory,const int &iDirectorySize);

};

#endif