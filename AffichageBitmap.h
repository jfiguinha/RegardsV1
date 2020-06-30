// AffichageBitmap.h: interface for the CAffichageBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AFFICHAGEBITMAP_H__CD02453E_28B9_4D90_9574_D0CE394159AD__INCLUDED_)
#define AFX_AFFICHAGEBITMAP_H__CD02453E_28B9_4D90_9574_D0CE394159AD__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000



class CAffichageBitmapWindow;

class CAffichageBitmap  
{

public:

	CAffichageBitmap();
	virtual ~CAffichageBitmap();
	int CreateScreenWindow(HWND hParent, HINSTANCE hInstance,int nCmdShow, const char * cFileName, const int iTrie = 0);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool GetExplorer();
	void SetExplorer(bool bExplorer);

private:

	int CreateBitmapWindow(int nCmdShow);

	
	CAffichageBitmapWindow * pimpl_;
	HWND hWnd;
	TCHAR szWindowClass[MAX_LOADSTRING];
	TCHAR szTitle[MAX_LOADSTRING];
	

};

#endif // !defined(AFX_AFFICHAGEBITMAP_H__CD02453E_28B9_4D90_9574_D0CE394159AD__INCLUDED_)
