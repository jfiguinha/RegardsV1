// Scroll.cpp: implementation of the CScroll class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Scroll.h"
#include "objet.h"

class CScrollPrv
{
public:
	CScrollPrv()
	{
		xCurrentScroll = 0;
		yCurrentScroll = 0;

	};
	~CScrollPrv(){};

	HWND hwndScrollHorz, hwndScrollVert;
	// These variables are required for horizontal scrolling. 
 
	int xMinScroll;       // minimum horizontal scroll value 
	int xCurrentScroll;   // current horizontal scroll value 
	int xMaxScroll;       // maximum horizontal scroll value 
 
	// These variables are required for vertical scrolling. 
 
	int yMinScroll;       // minimum vertical scroll value 
	int yCurrentScroll;   // current vertical scroll value 
	int yMaxScroll;       // maximum vertical scroll value 

	int xMaxHwndScroll;
	int yMaxHwndScroll;

	SCROLLINFO si; 

};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScroll::CScroll()
{
	pimpl_ = new CScrollPrv();
	if (pimpl_ == NULL)
	{
		CObjet::ErreurInformation();
	}
}

CScroll::~CScroll()
{
	delete pimpl_;
}

void CScroll::CreateScrollWindow(HWND hWndParent, HINSTANCE hInst)
{
   pimpl_->hwndScrollHorz = CreateWindowEx( 
		0L,                          // no extended styles 
		"SCROLLBAR",                 // scroll bar control class 
		(LPSTR) NULL,                // text for window title bar 
		WS_VISIBLE | WS_CHILD | SBS_HORZ | SBS_RIGHTALIGN,         // scroll bar styles 
		0,                           // horizontal position 
		0,                           // vertical position 
		200,                         // width of the scroll bar 
		CW_USEDEFAULT,               // default height 
		hWndParent,                   // handle to main window 
		(HMENU) NULL,           // no menu for a scroll bar 
		hInstance,                  // instance owning this window 
		(LPVOID) NULL           // pointer not needed 
	); 

   pimpl_->hwndScrollVert = CreateWindowEx( 
		0L,                          // no extended styles 
		"SCROLLBAR",                 // scroll bar control class 
		(LPSTR) NULL,                // text for window title bar 
		WS_VISIBLE | WS_CHILD | SBS_VERT | SBS_BOTTOMALIGN,         // scroll bar styles 
		0,                           // horizontal position 
		0,                           // vertical position 
		200,                         // width of the scroll bar 
		CW_USEDEFAULT,               // default height 
		hWndParent,                   // handle to main window 
		(HMENU) NULL,           // no menu for a scroll bar 
		hInstance,                  // instance owning this window 
		(LPVOID) NULL           // pointer not needed 
	); 

}

void CScroll::Initialize()
{
	// Initialize the horizontal scrolling variables. 

	pimpl_->xMinScroll = 0; 
	pimpl_->xCurrentScroll = 0; 
	pimpl_->xMaxScroll = 0; 

	// Initialize the vertical scrolling variables. 

	pimpl_->yMinScroll = 0; 
	pimpl_->yCurrentScroll = 0; 
	pimpl_->yMaxScroll = 0; 

}



void CScroll::Size(HWND &hWnd,const int &xNewSize, const int &yNewSize)
{

	if(pimpl_->xCurrentScroll != 0)
		pimpl_->xCurrentScroll = ((float)pimpl_->xCurrentScroll / (float)pimpl_->xMaxHwndScroll) * max(pimpl_->xMaxScroll-xNewSize,0);

	if(pimpl_->yCurrentScroll != 0)
		pimpl_->yCurrentScroll = ((float)pimpl_->yCurrentScroll / (float)pimpl_->yMaxHwndScroll) * max(pimpl_->yMaxScroll-yNewSize,0);


	pimpl_->xMaxHwndScroll = max(pimpl_->xMaxScroll-xNewSize,0); 

	pimpl_->si.cbSize = sizeof(pimpl_->si); 
	pimpl_->si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
	pimpl_->si.nMin   = pimpl_->xMinScroll; 
	pimpl_->si.nMax   = pimpl_->xMaxScroll; 
	pimpl_->si.nPage  = xNewSize; 
	pimpl_->si.nPos   = pimpl_->xCurrentScroll; 
	SetScrollInfo(hWnd, SB_HORZ, &pimpl_->si, TRUE); 

	// The vertical scrolling range is defined by 
	// (bitmap_height) - (client_height). The current vertical 
	// scroll value remains within the vertical scrolling range. 

	pimpl_->yMaxHwndScroll = max(pimpl_->yMaxScroll-yNewSize,0); 

	pimpl_->si.cbSize = sizeof(pimpl_->si); 
	pimpl_->si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
	pimpl_->si.nMin   = pimpl_->yMinScroll; 
	pimpl_->si.nMax   = pimpl_->yMaxScroll;
	pimpl_->si.nPage  = yNewSize; 
	pimpl_->si.nPos   = pimpl_->yCurrentScroll; 

	SetScrollInfo(hWnd, SB_VERT, &pimpl_->si, TRUE);
}

void CScroll::DefineSize(const int &xMax, const int &yMax, const int &xMin, const int &yMin)
{
	pimpl_->xMaxScroll = xMax;
	pimpl_->yMaxScroll = yMax;
	pimpl_->xMinScroll = xMin;
	pimpl_->yMinScroll = yMin;

}

void CScroll::Hscroll(HWND hWnd,int Param,int Param2, int &fScroll)
{
	int xDelta;     // xDelta = new_pos - current_pos  
	int xNewPos;    // new position 
	int yDelta = 0; 

	switch (Param) 
	{ 
		// User clicked the shaft left of the scroll box. 

		case SB_PAGEUP: 
			xNewPos = pimpl_->xCurrentScroll - 50; 
			break; 

		// User clicked the shaft right of the scroll box. 

		case SB_PAGEDOWN: 
			xNewPos = pimpl_->xCurrentScroll + 50; 
			break; 

		// User clicked the left arrow. 

		case SB_LINEUP: 
			xNewPos = pimpl_->xCurrentScroll - 5; 
			break; 

		// User clicked the right arrow. 

		case SB_LINEDOWN: 
			xNewPos = pimpl_->xCurrentScroll + 5; 
			break; 

		// User dragged the scroll box. 

		case SB_THUMBPOSITION: 
			xNewPos = Param2; 
			break; 

		default: 
			xNewPos = pimpl_->xCurrentScroll; 
	} 

	// New position must be between 0 and the screen width. 

	xNewPos = max(0, xNewPos); 
	xNewPos = min(pimpl_->xMaxHwndScroll, xNewPos); 

	// If the current position does not change, do not scroll.

	if (xNewPos == pimpl_->xCurrentScroll) 
		return; 

	// Set the scroll flag to TRUE. 

	fScroll = TRUE; 

	// Determine the amount scrolled (in pixels). 

	xDelta = xNewPos - pimpl_->xCurrentScroll; 

	// Reset the current scroll position. 

	pimpl_->xCurrentScroll = xNewPos; 

	// Scroll the window. (The system repaints most of the 
	// client area when ScrollWindowEx is called; however, it is 
	// necessary to call UpdateWindow in order to repaint the 
	// rectangle of pixels that were invalidated.) 

	ScrollWindowEx(hWnd, -xDelta, -yDelta, (CONST RECT *) NULL, 
		(CONST RECT *) NULL, (HRGN) NULL, (LPRECT) NULL, 
		SW_INVALIDATE); 
	UpdateWindow(hWnd); 

	// Reset the scroll bar. 

	pimpl_->si.cbSize = sizeof(pimpl_->si); 
	pimpl_->si.fMask  = SIF_POS; 
	pimpl_->si.nPos   = pimpl_->xCurrentScroll; 
	SetScrollInfo(hWnd, SB_HORZ, &pimpl_->si, TRUE);
	InvalidateRect (hWnd, NULL, TRUE) ;
}

void CScroll::Vscroll(HWND hWnd,int Param,int Param2, int &fScroll)
{
	int xDelta = 0; 
	int yDelta;     // yDelta = new_pos - current_pos 
	int yNewPos;    // new position 

	switch (Param) 
	{ 
		// User clicked the shaft above the scroll box. 

		case SB_PAGEUP: 
			yNewPos = pimpl_->yCurrentScroll - 50; 
			break; 

		// User clicked the shaft below the scroll box. 

		case SB_PAGEDOWN: 
			yNewPos = pimpl_->yCurrentScroll + 50; 
			break; 

		// User clicked the top arrow. 

		case SB_LINEUP: 
			yNewPos = pimpl_->yCurrentScroll - 5; 
			break; 

		// User clicked the bottom arrow. 

		case SB_LINEDOWN: 
			yNewPos = pimpl_->yCurrentScroll + 5; 
			break; 

		// User dragged the scroll box. 

		case SB_THUMBPOSITION: 
			yNewPos = Param2; 
			break; 

		default: 
			yNewPos = pimpl_->yCurrentScroll; 
	} 

	// New position must be between 0 and the screen height. 

	yNewPos = max(0, yNewPos); 
	yNewPos = min(pimpl_->yMaxHwndScroll, yNewPos); 

	// If the current position does not change, do not scroll.

	if (yNewPos == pimpl_->yCurrentScroll) 
		return;

	// Set the scroll flag to TRUE. 

	fScroll = TRUE; 

	// Determine the amount scrolled (in pixels). 

	yDelta = yNewPos - pimpl_->yCurrentScroll; 

	// Reset the current scroll position. 

	pimpl_->yCurrentScroll = yNewPos; 

	// Scroll the window. (The system repaints most of the 
	// client area when ScrollWindowEx is called; however, it is 
	// necessary to call UpdateWindow in order to repaint the 
	// rectangle of pixels that were invalidated.) 

	ScrollWindowEx(hWnd, -xDelta, -yDelta, (CONST RECT *) NULL, 
		(CONST RECT *) NULL, (HRGN) NULL, (LPRECT) NULL, 
		SW_INVALIDATE); 
	UpdateWindow(hWnd); 

	// Reset the scroll bar. 

	pimpl_->si.cbSize = sizeof(pimpl_->si); 
	pimpl_->si.fMask  = SIF_POS; 
	pimpl_->si.nPos   = pimpl_->yCurrentScroll; 
	SetScrollInfo(hWnd, SB_VERT, &pimpl_->si, TRUE); 
	InvalidateRect (hWnd, NULL, TRUE) ;
}

int CScroll::GetCurrentHScroll()
{
	return (pimpl_->xCurrentScroll < 0 ? 0 : pimpl_->xCurrentScroll);
}

int CScroll::GetCurrentVScroll()
{
	return (pimpl_->yCurrentScroll < 0 ? 0 : pimpl_->yCurrentScroll);
}

void CScroll::DisabledScrollBar()
{
	DestroyWindow(pimpl_->hwndScrollHorz);
	DestroyWindow(pimpl_->hwndScrollVert);
}

