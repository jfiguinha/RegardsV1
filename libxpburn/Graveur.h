// Graveur.h: interface for the CGraveur class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAVEUR_H__AB6C9F44_F5A9_4635_96EA_6831C16F0306__INCLUDED_)
#define AFX_GRAVEUR_H__AB6C9F44_F5A9_4635_96EA_6831C16F0306__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <IconeFile.h>

class IDiscMasterProgressEventsImpl;

class CGraveur  
{
public:
	CGraveur();
	virtual ~CGraveur();
	int GraverFichierRep(const string &m_stgDirectory, HWND hWnd);
	int GraverFichier(const string &m_stgFilename, HWND hWnd);
	int EraseCD(const bool &m_bMethod, HWND hWnd);
	int GraverFichierAlbum(IconeFileVector &m_IconeFileVector, const string &m_stgAlbumName, HWND hWnd);
	int EjectCD();
};

#endif // !defined(AFX_GRAVEUR_H__AB6C9F44_F5A9_4635_96EA_6831C16F0306__INCLUDED_)
