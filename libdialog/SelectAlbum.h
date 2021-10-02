

#ifndef __SELECTALBUM__H__
#define __SELECTALBUM__H__
#include <albumdata.h>

#include "LocalDialog.h"

class CSelectAlbum : public CLocalDialog
{
public:
	CSelectAlbum(void);
	~CSelectAlbum(void);

	void SetParam(AlbumDataVector * vectorAlbum)
	{
		m_VectorAlbum = vectorAlbum;
	}

	int GetReturnValue()
	{
		return m_iReturnValue;
	}

	const char * GetAlbumName()
	{
		return m_stgAlbumName.c_str();
	}

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	AlbumDataVector * m_VectorAlbum;
	int m_iReturnValue;
	string m_stgAlbumName;
};

#endif