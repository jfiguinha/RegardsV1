// DialogFiltreEffet.h: interface for the CDialogFiltreEffet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOGFILTREEFFET_H__E87C7F29_7EA5_4460_9BB3_D46F5C62CA6C__INCLUDED_)
#define AFX_DIALOGFILTREEFFET_H__E87C7F29_7EA5_4460_9BB3_D46F5C62CA6C__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CIBitmap;

class CFiltreEffet
{
public:
	CFiltreEffet();
	~CFiltreEffet();

	CIBitmap * m_ciBitmap;
	int NumFiltre;
	long * m_lParam;
	COLORREF colorref;
	int iNbParam;
	int iAffichage;
	int iHandlePrgBar;
	BOOL m_bGpGpu;
	HWND hDlg;
};


#endif // !defined(AFX_DIALOGFILTREEFFET_H__E87C7F29_7EA5_4460_9BB3_D46F5C62CA6C__INCLUDED_)

