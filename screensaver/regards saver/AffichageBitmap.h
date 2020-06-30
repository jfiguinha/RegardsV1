// AffichageBitmap.h: interface for the CAffichageBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AFFICHAGEBITMAP_H__C47E9F10_76C0_4BAD_8DE1_2A5E30AAB326__INCLUDED_)
#define AFX_AFFICHAGEBITMAP_H__C47E9F10_76C0_4BAD_8DE1_2A5E30AAB326__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

class CAffichageBitmap  
{
public:

	CAffichageBitmap();
	virtual ~CAffichageBitmap();

	void CAffichageBitmap::AffichageBitmap(HWND hWnd,HBITMAP hBitmap,const char * szFileName);

private:

	int nTailleAffichageWidth;
	int nTailleAffichageHeight;
	void CAffichageBitmap::DeterminePos(RECT rc, int &left, int &top);
	void CAffichageBitmap::CompleteBitmap(HDC hdc, RECT rc, int left, int top, int xValue, int yValue);
	void CAffichageBitmap::ShowBitmap(HDC hdc, int left,int top, int xValue, int yValue, HBITMAP hBitmap);



};

#endif // !defined(AFX_AFFICHAGEBITMAP_H__C47E9F10_76C0_4BAD_8DE1_2A5E30AAB326__INCLUDED_)
