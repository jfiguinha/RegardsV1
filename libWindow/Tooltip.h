// Tooltip.h: interface for the CTooltip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLTIP_H__DDA7DC30_87AB_4020_A074_AF9B2FFB7B19__INCLUDED_)
#define AFX_TOOLTIP_H__DDA7DC30_87AB_4020_A074_AF9B2FFB7B19__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "LocalWindow.h"

class CTooltip : public CLocalWindow  
{
public:
	CTooltip();
	virtual ~CTooltip();
	int CreateEx(HWND hParent,DWORD dExStyle,DWORD dWndStyle,RECT rc);
	int SetLibelle(const char * m_szLibelle);
	int TrackActive(const bool &m_bShow);
	int TrackPosition();

private:

	char m_szLibelle[255];
	TOOLINFO ti;
	bool g_bIsVisible;
};

#endif // !defined(AFX_TOOLTIP_H__DDA7DC30_87AB_4020_A074_AF9B2FFB7B19__INCLUDED_)
