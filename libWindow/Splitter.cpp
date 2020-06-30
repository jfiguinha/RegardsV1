// Splitter.cpp: implementation of the CWndSplitter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Splitter.h"


#define SPLIT_HORIZ 1
#define SPLIT_VERT 2

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWndSplitter::CWndSplitter()
{
	m_bHorizontal = 1;
	bSplitterMoving = FALSE;
	dwSplitterPosOld = dwSplitterPos = 130;
	hCursor = NULL;
}

CWndSplitter::~CWndSplitter()
{
	if(hCursor != NULL)
		DeleteObject(hCursor);
}

void CWndSplitter::SetSplitPos(DWORD dwSplitterPos)
{
	this->dwSplitterPos = dwSplitterPos;
}


void CWndSplitter::SetSplitWindow(HWND hWnd1, HWND hWnd2)
{
	this->hWnd1 = hWnd1;
	this->hWnd2 = hWnd2;
}

int CWndSplitter::LButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	dwSplitterPosOld = -1;
	SetCursor(hCursor);
	bSplitterMoving = TRUE;
	SetCapture(hWnd);
	return 0;
}

int CWndSplitter::LButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(bSplitterMoving)
	{
		ReleaseCapture();
		HDC hdc = GetWindowDC(hWnd);
		if(m_bHorizontal == 1)
			DrawXorBar(hdc, 0,dwSplitterPosOld, rect.right,4);
		else
			DrawXorBar(hdc, dwSplitterPosOld, 0, 4,rect.bottom);
		ReleaseDC(hWnd, hdc);
		bSplitterMoving = FALSE;
		OnSize(m_hWnd,WM_SIZE, 0, MAKELPARAM(rect.right, rect.bottom));
	}
	//RedrawSplitter();
	return 0;
}

int CWndSplitter::MouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	

	if(m_bHorizontal == 1)
	{
		SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENS)));

		if (HIWORD(lParam) > 10) // do not allow above this mark
		{
			//SetCursor(hCursor);
			if ((wParam == MK_LBUTTON) && bSplitterMoving)
			{
				
				if (HIWORD(lParam) > rect.bottom - 10)
					return 0;

				dwSplitterPos = HIWORD(lParam);
				
				//SendMessage(hWnd, WM_SIZE, 0, MAKELPARAM(rect.right, rect.bottom));

				DrawBar();
			}
		}
	}
	else
	{
		SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEWE)));

		if (LOWORD(lParam) > 10) // do not allow above this mark
		{
			//SetCursor(hCursor);
			if ((wParam == MK_LBUTTON) && bSplitterMoving)
			{
				if (LOWORD(lParam) > rect.right - 10)
					return 0;

				dwSplitterPos = LOWORD(lParam);
				DrawBar();
			}
		}
	}

	return 0;
}

int CWndSplitter::Size(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/* Adjust the children's size and position */

	iRight = LOWORD(lParam);
	iBottom = HIWORD(lParam);

	GetClientRect(hWnd, &rect);

	rect.right = iRight;
	rect.bottom = iBottom;

	RedrawSplitter();

	return 0;
}

void CWndSplitter::SetHorizontalSplit(const bool &m_bTrue)
{
	m_bHorizontal = m_bTrue;
}

LRESULT CWndSplitter::OnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Sens du partage
	if(m_bHorizontal)
		hCursor = (HCURSOR)LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS));
	else
		hCursor = (HCURSOR)LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE));

	return 0;
}

LRESULT CWndSplitter::OnPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint( hWnd, &ps );
	SendMessage(hWnd1,uMsg,wParam,lParam);
	SendMessage(hWnd2,uMsg,wParam,lParam);
	EndPaint( hWnd, &ps );
	ReleaseDC(hWnd,hDC);

	return 0;
}

int CWndSplitter::RedrawSplitter()
{
	if(!::IsWindowVisible(hWnd1) && !::IsWindowVisible(hWnd2))
		return 0;

	if(iRight == 0 || iBottom == 0)
		return 0;


	if(m_bHorizontal == 1)
	{
		/* 
		   If window is shrunk so that splitter now lies outside the 
		   window boundaries, move the splitter within the window.
		*/
		if (iBottom < dwSplitterPos)
			dwSplitterPos = iBottom - 10;


		//Test si les fenêtres sont visibles ou non

		if(::IsWindowVisible(hWnd1) && ::IsWindowVisible(hWnd2))
		{
			MoveWindow(hWnd1, 0, 0, iRight, dwSplitterPos - 1, TRUE);
			MoveWindow(hWnd2, 0, dwSplitterPos+2, iRight , iBottom - dwSplitterPos - 2, TRUE);
		}
		else
		{
			if(::IsWindowVisible(hWnd1))
				MoveWindow(hWnd1, 0, 0, iRight, iBottom, TRUE);
			else
				MoveWindow(hWnd2, 0, 0, iRight, iBottom, TRUE);
		}
	}
	else
	{
		/* 
		   If window is shrunk so that splitter now lies outside the 
		   window boundaries, move the splitter within the window.
		*/
		if (iRight < dwSplitterPos)
			dwSplitterPos = iRight - 10;

		if(::IsWindowVisible(hWnd1) && ::IsWindowVisible(hWnd2))
		{
			MoveWindow(hWnd1, 0, 0, dwSplitterPos - 1, iBottom, TRUE);
			MoveWindow(hWnd2, dwSplitterPos+2, 0, iRight - dwSplitterPos - 2,iBottom, TRUE);
		}
		else
		{
			if(::IsWindowVisible(hWnd1))
				MoveWindow(hWnd1, 0, 0, iRight, iBottom, TRUE);
			else
				MoveWindow(hWnd2, 0, 0, iRight, iBottom, TRUE);
		}

	}

	return 0;
}

LRESULT CWndSplitter::OnSize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return Size(hWnd,uMsg,wParam,lParam);
}

LRESULT CWndSplitter::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return MouseMove(hWnd,uMsg,wParam,lParam);
}

LRESULT CWndSplitter::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return LButtonDown(hWnd,uMsg,wParam,lParam);
}

LRESULT CWndSplitter::OnLButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return LButtonUp(hWnd,uMsg,wParam,lParam);
}

void CWndSplitter::DrawXorBar(HDC hdc, int x1, int y1, int width, int height)
{
	HBRUSH  hbr, hbrushOld;

	WORD _dotPatternBmp[8] = 
	{ 
		0x00aa, 0x0055, 0x00aa, 0x0055, 
		0x00aa, 0x0055, 0x00aa, 0x0055
	};

	HBITMAP hbm = CreateBitmap(8, 8, 1, 1, _dotPatternBmp);

	hbr = CreatePatternBrush(hbm);
	
	SetBrushOrgEx(hdc, x1, y1, 0);
	hbrushOld = (HBRUSH)SelectObject(hdc, hbr);
	
	PatBlt(hdc, x1, y1, width, height, PATINVERT);
	
	SelectObject(hdc, hbrushOld);
	
	DeleteObject(hbr);

	DeleteObject(hbm);

}

void CWndSplitter::DrawBar()
{
	HDC hdc;

	if(m_bHorizontal)
	{
		if(dwSplitterPos < 0) 
			dwSplitterPos = 0;

		if(dwSplitterPos > rect.bottom) 
			dwSplitterPos = rect.bottom;

		SetCapture(m_hWnd);

		hdc = GetWindowDC(m_hWnd);

		if(dwSplitterPosOld != -1)
			DrawXorBar(hdc, 0,dwSplitterPosOld, rect.right,4);
		DrawXorBar(hdc, 0,dwSplitterPos, rect.right,4);

		ReleaseDC(m_hWnd, hdc);

		
	}
	else
	{
		if(dwSplitterPos < 0) 
			dwSplitterPos = 0;

		if(dwSplitterPos > rect.right) 
			dwSplitterPos = rect.right;

		SetCapture(m_hWnd);

		hdc = GetWindowDC(m_hWnd);
		
		if(dwSplitterPosOld != -1)
			DrawXorBar(hdc, dwSplitterPosOld, 0, 4,rect.bottom);
		DrawXorBar(hdc, dwSplitterPos, 0, 4,rect.bottom);

		ReleaseDC(m_hWnd, hdc);
		
	}

	dwSplitterPosOld = dwSplitterPos;
}


