// Tooltip.cpp: implementation of the CTooltip class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tooltip.h"


#define X_OFFSET 15
#define Y_OFFSET X_OFFSET

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTooltip::CTooltip()
{
    INITCOMMONCONTROLSEX iccex; 

    /* INITIALIZE COMMON CONTROLS */
    iccex.dwSize = sizeof(iccex);
    iccex.dwICC  = ICC_BAR_CLASSES | ICC_COOL_CLASSES;
    InitCommonControlsEx(&iccex);

	StringCchCopy(m_szLibelle,255,"Fermer");

	m_hWnd = NULL;
}

CTooltip::~CTooltip()
{
	if(m_hWnd != NULL)
		DestroyWindow(m_hWnd);
}

int CTooltip::CreateEx(HWND hParent,DWORD dExStyle,DWORD dWndStyle,RECT rc)
{

    
    unsigned int uid = 0;       // for ti initialization
    

    // Load the ToolTip class from the DLL.
    // Create the ToolTip control.
    m_hWnd = CreateWindow(TOOLTIPS_CLASS, TEXT(""),
                          WS_POPUP,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, (HMENU)NULL, hInstance,
                          NULL);

    // Prepare TOOLINFO structure for use as tracking ToolTip.
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
    ti.hwnd   = hParent;
    ti.uId    = (UINT)hParent;
    ti.hinst  = hInstance;
    ti.lpszText  = m_szLibelle;
    ti.rect.left = ti.rect.top = ti.rect.bottom = ti.rect.right = 0; 
    
    // Add the tool to the control, displaying an error if needed.
    if(!SendMessage(m_hWnd,TTM_ADDTOOL,0,(LPARAM)&ti)){
        return NULL;
    }
    

    return 0;    

}

/////////////////////////////////////////////////////////////////////////////////
//Modification du libelle
/////////////////////////////////////////////////////////////////////////////////

int CTooltip::SetLibelle(const char * m_szLibelle)
{
	StringCchCopy(this->m_szLibelle,255,m_szLibelle);

    ti.lpszText = this->m_szLibelle;
    
    // Add the tool to the control, displaying an error if needed.
    SendMessage(m_hWnd,TTM_UPDATETIPTEXT,0,(LPARAM)&ti);

	return 0;
}

int CTooltip::TrackPosition()
{
	if(g_bIsVisible)
	{
		POINT point;
		GetCursorPos(&point);
		SendMessage(m_hWnd,TTM_TRACKPOSITION, 0, (LPARAM)MAKELPARAM(point.x + X_OFFSET, point.y + Y_OFFSET));
	}

	return 0;
}

int CTooltip::TrackActive(const bool &m_bShow)
{
    SendMessage(m_hWnd,TTM_TRACKACTIVATE,(WPARAM)m_bShow,(LPARAM)&ti);
    g_bIsVisible = m_bShow;

	return 0;
}

