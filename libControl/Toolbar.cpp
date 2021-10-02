// Toolbar.cpp: implementation of the CToolbar class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Toolbar.h"
#include <uxtheme.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolbar::CToolbar()
{
	himlHot = NULL;
}

CToolbar::~CToolbar()
{
	if(himlHot != NULL)
		ImageList_Destroy(himlHot);

	himlHot = NULL;
}

HWND CToolbar::CreateMenuToolbar(HWND hWndParent, HINSTANCE hInst, int idRessource)
{
	HWND hWndToolbar;
	TBBUTTON tbb[1]; 
	int iIndex = 0;
	tbb[iIndex].iBitmap = I_IMAGENONE; 
	tbb[iIndex].idCommand = ID_ACTION_EXTRAIREFICHIER; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	// Create a toolbar with three standard file bitmaps and one
	// separator.
	// There are 15 items in IDB_STD_SMALL_COLOR.  However, because this is a
	// standard system-defined bitmap, the parameter that specifies the number
	// of button images in the bitmap (nBitmaps) is ignored, so it is set
	// to 0.
	hWndToolbar = CreateToolbarEx (hWndParent, 
		WS_CHILD | WS_BORDER | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_LIST, 
		idRessource, 0, hInst, NULL, 
		tbb, ARRAYSIZE(tbb), 0, 0, 32, 34, sizeof (TBBUTTON));

	//SetWindowTheme(hWndToolbar,NULL,L"");

	return hWndToolbar;
}

// Also the code example uses a MACRO defined in the header file as
// follows: #define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))

// CreateAToolBar2 creates a toolbar and adds bitmaps to it.
// The function returns the handle to the toolbar if successful, 
// or NULL otherwise.
// The function uses HINST_COMMCTRL as the HINSTANCE and
// IDB_STD_SMALL_COLOR as the bitmap identifier. 
// hwndParent is the handle to the toolbar's parent window. 
HWND CToolbar::CreateToolBarWinzip(HWND hWndParent, HINSTANCE hInst)
{
	HWND hWndToolbar;

	TBBUTTON tbb[5]; 

	int iIndex = 0;

	tbb[iIndex].iBitmap = iIndex; 
	tbb[iIndex].idCommand = ID_ACTION_EXTRAIREFICHIER; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = iIndex; 
	tbb[iIndex].idCommand = ID_ACTION_AJOUTERFICHIER; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = iIndex; 
	tbb[iIndex].idCommand = ID_ACTION_SUPPRIMERFICHIER; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = iIndex; 
	tbb[iIndex].idCommand = ID_ACTION_VOIRFICHIER; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = iIndex; 
	tbb[iIndex].idCommand = ID_ACTION_TOUTSLECTIONNER; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	// Create a toolbar with three standard file bitmaps and one
	// separator.
	// There are 15 items in IDB_STD_SMALL_COLOR.  However, because this is a
	// standard system-defined bitmap, the parameter that specifies the number
	// of button images in the bitmap (nBitmaps) is ignored, so it is set
	// to 0.
	hWndToolbar = CreateToolbarEx (hWndParent, 
		WS_CHILD | WS_BORDER | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT, 
		ID_TOOLBAR, 0, hInst, NULL, 
		tbb, ARRAYSIZE(tbb), 0, 0, 32, 34, sizeof (TBBUTTON));

	HIMAGELIST himlHot3 = ImageList_LoadImage(hInst,MAKEINTRESOURCE(IDB_ICONEZIP),32,5,RGB(255,0,255),IMAGE_BITMAP,LR_CREATEDIBSECTION);
	SendMessage(hWndToolbar,TB_SETHOTIMAGELIST, 0, (LPARAM)himlHot3);

	HIMAGELIST himlHot2 = ImageList_LoadImage(hInst,MAKEINTRESOURCE(IDB_ICONEZIP),32,5,RGB(255,0,255),IMAGE_BITMAP,LR_CREATEDIBSECTION);
	SendMessage(hWndToolbar,TB_SETIMAGELIST, 0, (LPARAM)himlHot2);

	return hWndToolbar;
} 

// Also the code example uses a MACRO defined in the header file as
// follows: #define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))

// CreateAToolBar2 creates a toolbar and adds bitmaps to it.
// The function returns the handle to the toolbar if successful, 
// or NULL otherwise.
// The function uses HINST_COMMCTRL as the HINSTANCE and
// IDB_STD_SMALL_COLOR as the bitmap identifier. 
// hwndParent is the handle to the toolbar's parent window. 
HWND CToolbar::CreateToolBarMain(HWND hWndParent, HINSTANCE hInst)
{
	HWND hWndToolbar;

	TBBUTTON tbb[13]; 

	int iIndex = 0;

	tbb[iIndex].iBitmap = iIndex; 
	tbb[iIndex].idCommand = IDM_NEW; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = 0; 
	tbb[iIndex].idCommand = 0; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_SEP; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = iIndex - 1; 
	tbb[iIndex].idCommand = IDM_LARGEICON; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = iIndex - 1; 
	tbb[iIndex].idCommand = IDM_SMALLICON; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = iIndex - 1; 
	tbb[iIndex].idCommand = IDM_LISTVIEW; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = iIndex - 1; 
	tbb[iIndex].idCommand = IDM_REPORTVIEW; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = iIndex - 1; 
	tbb[iIndex].idCommand = IDM_THUMBMAIL; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = 0; 
	tbb[iIndex].idCommand = 0; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_SEP; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = iIndex  - 2; 
	tbb[iIndex].idCommand = IDM_EDITION_REPERTOIRESUP; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = iIndex - 2; 
	tbb[iIndex].idCommand = IDM_AFFICHAGE_IMAGE; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = iIndex-2; 
	tbb[iIndex].idCommand = IDM_OUTILS_TRAITEMENTIMAGE; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = iIndex - 2; 
	tbb[iIndex].idCommand = ID_OUTILS_AFFICHAGERECHERCHE; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

	iIndex++;

	tbb[iIndex].iBitmap = iIndex - 2; 
	tbb[iIndex].idCommand = ID_OUTILS_AFFICHAGEDOSSIER; 
	tbb[iIndex].fsState = TBSTATE_ENABLED; 
	tbb[iIndex].fsStyle = BTNS_BUTTON; 
	tbb[iIndex].dwData = 0; 
	tbb[iIndex].iString = 0;

 
	// Create a toolbar with three standard file bitmaps and one
	// separator.
	// There are 15 items in IDB_STD_SMALL_COLOR.  However, because this is a
	// standard system-defined bitmap, the parameter that specifies the number
	// of button images in the bitmap (nBitmaps) is ignored, so it is set
	// to 0.
	hWndToolbar = CreateToolbarEx (hWndParent, 
		WS_CHILD | WS_BORDER | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT, 
		ID_TOOLBAR, 0, hInst, NULL, 
		tbb, ARRAYSIZE(tbb), 0, 0, 30, 30, sizeof (TBBUTTON));

	HIMAGELIST himlHot3 = ImageList_LoadImage(hInst,MAKEINTRESOURCE(IDB_MAINHOT),30,13,RGB(255,0,255),IMAGE_BITMAP,LR_CREATEDIBSECTION);
	SendMessage(hWndToolbar,TB_SETHOTIMAGELIST, 0, (LPARAM)himlHot3);

	HIMAGELIST himlHot2 = ImageList_LoadImage(hInst,MAKEINTRESOURCE(IDB_MAINHOT),30,13,RGB(255,0,255),IMAGE_BITMAP,LR_CREATEDIBSECTION);
	SendMessage(hWndToolbar,TB_SETIMAGELIST, 0, (LPARAM)himlHot2);

	return hWndToolbar;
} 

// Also the code example uses a MACRO defined in the header file as
// follows: #define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))

// CreateAToolBar2 creates a toolbar and adds bitmaps to it.
// The function returns the handle to the toolbar if successful, 
// or NULL otherwise.
// The function uses HINST_COMMCTRL as the HINSTANCE and
// IDB_STD_SMALL_COLOR as the bitmap identifier. 
// hwndParent is the handle to the toolbar's parent window. 
HWND CToolbar::CreateToolBarCatalogue(HWND hWndParent, HINSTANCE hInst)
{
	HWND hWndToolbar;

	TBBUTTON tbb[9]; 

	tbb[0].iBitmap = 0; 
	tbb[0].idCommand = IDM_LARGEICON; 
	tbb[0].fsState = TBSTATE_ENABLED; 
	tbb[0].fsStyle = BTNS_BUTTON; 
	tbb[0].dwData = 0; 
	tbb[0].iString = 0;

	tbb[1].iBitmap = 1; 
	tbb[1].idCommand = IDM_SMALLICON; 
	tbb[1].fsState = TBSTATE_ENABLED; 
	tbb[1].fsStyle = BTNS_BUTTON; 
	tbb[1].dwData = 0; 
	tbb[1].iString = 0;

	tbb[2].iBitmap = 2; 
	tbb[2].idCommand = IDM_LISTVIEW; 
	tbb[2].fsState = TBSTATE_ENABLED; 
	tbb[2].fsStyle = BTNS_BUTTON; 
	tbb[2].dwData = 0; 
	tbb[2].iString = 0;

	tbb[3].iBitmap = 3; 
	tbb[3].idCommand = IDM_REPORTVIEW; 
	tbb[3].fsState = TBSTATE_ENABLED; 
	tbb[3].fsStyle = BTNS_BUTTON; 
	tbb[3].dwData = 0; 
	tbb[3].iString = 0;

	tbb[4].iBitmap = 4; 
	tbb[4].idCommand = IDM_THUMBMAIL; 
	tbb[4].fsState = TBSTATE_ENABLED; 
	tbb[4].fsStyle = BTNS_BUTTON; 
	tbb[4].dwData = 0; 
	tbb[4].iString = 0;


	// Create a toolbar with three standard file bitmaps and one
	// separator.
	// There are 15 items in IDB_STD_SMALL_COLOR.  However, because this is a
	// standard system-defined bitmap, the parameter that specifies the number
	// of button images in the bitmap (nBitmaps) is ignored, so it is set
	// to 0.
	hWndToolbar = CreateToolbarEx (hWndParent, 
		WS_CHILD | WS_BORDER | WS_VISIBLE | TBSTYLE_TOOLTIPS |  TBSTYLE_FLAT, 
		ID_TOOLBAR, 5, hInst, NULL, 
		tbb, ARRAYSIZE(tbb), 0, 0, 30, 20, sizeof (TBBUTTON));


	// Create the image list, himlHot.
	himlHot = ImageList_Create(30,30,ILC_COLOR8,8,1);
	// Load a bitmap from a resource file and adds the images to the
	// image list. The bitmap contains four images.
	HBITMAP hBitmapHot = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MAINHOT));
	ImageList_Add(himlHot, hBitmapHot, NULL);


	SendMessage(hWndToolbar, TB_SETHOTIMAGELIST, 0, (LPARAM)himlHot);

	DeleteObject(hBitmapHot);


	return hWndToolbar;
} 

// Also the code example uses a MACRO defined in the header file as
// follows: #define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))

// CreateAToolBar2 creates a toolbar and adds bitmaps to it.
// The function returns the handle to the toolbar if successful, 
// or NULL otherwise.
// The function uses HINST_COMMCTRL as the HINSTANCE and
// IDB_STD_SMALL_COLOR as the bitmap identifier. 
// hwndParent is the handle to the toolbar's parent window. 
HWND CToolbar::CreateToolBarPicture(HWND hWndParent, HINSTANCE hInst)
{
	HWND hWndToolbar;
	TBBUTTON tbb[13]; 

	tbb[0].iBitmap = 0; 
	tbb[0].idCommand = IDM_IMAGES_PRCDENTE; 
	tbb[0].fsState = TBSTATE_ENABLED; 
	tbb[0].fsStyle = BTNS_BUTTON; 
	tbb[0].dwData = 0; 
	tbb[0].iString = 0;

	tbb[1].iBitmap = 1; 
	tbb[1].idCommand = IDM_IMAGES_SUIVANTE; 
	tbb[1].fsState = TBSTATE_ENABLED; 
	tbb[1].fsStyle = BTNS_BUTTON; 
	tbb[1].dwData = 0; 
	tbb[1].iString = 0;

	tbb[2].iBitmap = 2; 
	tbb[2].idCommand = ID_IMAGES_VISUALISATION_PREMIREIMAGE; 
	tbb[2].fsState = TBSTATE_ENABLED; 
	tbb[2].fsStyle = BTNS_BUTTON; 
	tbb[2].dwData = 0; 
	tbb[2].iString = 0;

	tbb[3].iBitmap = 3; 
	tbb[3].idCommand = ID_IMAGES_VISUALISATION_DERNIREIMAGE; 
	tbb[3].fsState = TBSTATE_ENABLED; 
	tbb[3].fsStyle = BTNS_BUTTON; 
	tbb[3].dwData = 0; 
	tbb[3].iString = 0;

	tbb[4].iBitmap = 0; 
	tbb[4].idCommand = 0; 
	tbb[4].fsState = TBSTATE_ENABLED; 
	tbb[4].fsStyle = BTNS_SEP; 
	tbb[4].dwData = 0; 
	tbb[4].iString = 0;

	tbb[5].iBitmap = 4; 
	tbb[5].idCommand = IDM_FICHIER_REVERT; 
	tbb[5].fsState = TBSTATE_ENABLED; 
	tbb[5].fsStyle = BTNS_BUTTON; 
	tbb[5].dwData = 0; 
	tbb[5].iString = 0;

	tbb[6].iBitmap = 5; 
	tbb[6].idCommand = IDM_IMAGES_VISUALISATION_PLAINECRAN; 
	tbb[6].fsState = TBSTATE_ENABLED; 
	tbb[6].fsStyle = BTNS_BUTTON; 
	tbb[6].dwData = 0; 
	tbb[6].iString = 0;

	tbb[7].iBitmap = 6; 
	tbb[7].idCommand = IDM_IMAGES_ROTATEIMAGE_90CW; 
	tbb[7].fsState = TBSTATE_ENABLED; 
	tbb[7].fsStyle = BTNS_BUTTON; 
	tbb[7].dwData = 0; 
	tbb[7].iString = 0;

	tbb[8].iBitmap = 7; 
	tbb[8].idCommand = IDM_IMAGES_ROTATEIMAGE_90CCW; 
	tbb[8].fsState = TBSTATE_ENABLED; 
	tbb[8].fsStyle = BTNS_BUTTON; 
	tbb[8].dwData = 0; 
	tbb[8].iString = 0;

	tbb[9].iBitmap = 8; 
	tbb[9].idCommand = IDM_IMAGES_ROTATEIMAGE_FLIPHORIZONTAL;
	tbb[9].fsState = TBSTATE_ENABLED; 
	tbb[9].fsStyle = BTNS_BUTTON; 
	tbb[9].dwData = 0; 
	tbb[9].iString = 0;

	tbb[10].iBitmap = 9; 
	tbb[10].idCommand = IDM_IMAGES_ROTATEIMAGE_FLIPVERTICAL; 
	tbb[10].fsState = TBSTATE_ENABLED; 
	tbb[10].fsStyle = BTNS_BUTTON; 
	tbb[10].dwData = 0; 
	tbb[10].iString = 0;

	tbb[11].iBitmap = 10; 
	tbb[11].idCommand = ID_IMAGE_BOUGERIMAGE; 
	tbb[11].fsState = TBSTATE_ENABLED; 
	tbb[11].fsStyle = BTNS_BUTTON; 
	tbb[11].dwData = 0; 
	tbb[11].iString = 0;

	tbb[12].iBitmap = 11; 
	tbb[12].idCommand = ID_IMAGE_ZOOMERIMAGE; 
	tbb[12].fsState = TBSTATE_ENABLED; 
	tbb[12].fsStyle = BTNS_BUTTON; 
	tbb[12].dwData = 0; 
	tbb[12].iString = 0;


	// Create a toolbar with three standard file bitmaps and one
	// separator.
	// There are 15 items in IDB_STD_SMALL_COLOR.  However, because this is a
	// standard system-defined bitmap, the parameter that specifies the number
	// of button images in the bitmap (nBitmaps) is ignored, so it is set
	// to 0.
	hWndToolbar = CreateToolbarEx (hWndParent, 
		WS_CHILD | WS_BORDER | WS_VISIBLE | TBSTYLE_TOOLTIPS |  TBSTYLE_FLAT, 
		ID_TOOLBAR, 0, hInst, NULL, 
		tbb, ARRAYSIZE(tbb), 0, 0, 30, 30, sizeof (TBBUTTON));

	HIMAGELIST himlHot3 = ImageList_LoadImage(hInst,MAKEINTRESOURCE(IDB_PICTURE),30,12,RGB(255,0,255),IMAGE_BITMAP,LR_CREATEDIBSECTION);
	SendMessage(hWndToolbar,TB_SETHOTIMAGELIST, 0, (LPARAM)himlHot3);
	
	HIMAGELIST himlHot2 = ImageList_LoadImage(hInst,MAKEINTRESOURCE(IDB_PICTURE),30,12,RGB(255,0,255),IMAGE_BITMAP,LR_CREATEDIBSECTION);
	SendMessage(hWndToolbar,TB_SETIMAGELIST, 0, (LPARAM)himlHot2);

	/*

	// Create the image list, himlHot.
	himlHot = ImageList_Create(30,30,ILC_COLOR8,11,1);
	// Load a bitmap from a resource file and adds the images to the
	// image list. The bitmap contains four images.
	HBITMAP hBitmapHot = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PICTURE));
	ImageList_Add(himlHot, hBitmapHot, NULL);
	DeleteObject(hBitmapHot);*/

	//SendMessage(hWndToolbar, TB_SETHOTIMAGELIST, 0, (LPARAM)himlHot);

	return hWndToolbar;
} 

HWND CToolbar::CreateToolBarTraitement(HWND hWndParent, HINSTANCE hInst)
{
	HWND hWndToolbar;
	TBBUTTON tbb[7]; 

	tbb[0].iBitmap = 0; 
	tbb[0].idCommand = ID_IMAGE_BOUGERIMAGE; 
	tbb[0].fsState = TBSTATE_ENABLED; 
	tbb[0].fsStyle = BTNS_BUTTON; 
	tbb[0].dwData = 0; 
	tbb[0].iString = 0;

	tbb[1].iBitmap = 1; 
	tbb[1].idCommand = ID_IMAGE_COUPERIMAGE; 
	tbb[1].fsState = TBSTATE_ENABLED; 
	tbb[1].fsStyle = BTNS_BUTTON; 
	tbb[1].dwData = 0; 
	tbb[1].iString = 0;

	tbb[2].iBitmap = 2; 
	tbb[2].idCommand = ID_IMAGE_ZOOMERIMAGE; 
	tbb[2].fsState = TBSTATE_ENABLED; 
	tbb[2].fsStyle = BTNS_BUTTON; 
	tbb[2].dwData = 0; 
	tbb[2].iString = 0;

	tbb[3].iBitmap = 3; 
	tbb[3].idCommand = IDM_IMAGES_ROTATEIMAGE_90CW; 
	tbb[3].fsState = TBSTATE_ENABLED; 
	tbb[3].fsStyle = BTNS_BUTTON; 
	tbb[3].dwData = 0; 
	tbb[3].iString = 0;

	tbb[4].iBitmap = 4; 
	tbb[4].idCommand = IDM_IMAGES_ROTATEIMAGE_90CCW; 
	tbb[4].fsState = TBSTATE_ENABLED; 
	tbb[4].fsStyle = BTNS_BUTTON; 
	tbb[4].dwData = 0; 
	tbb[4].iString = 0;

	tbb[5].iBitmap = 5; 
	tbb[5].idCommand = IDM_IMAGES_ROTATEIMAGE_FLIPHORIZONTAL;
	tbb[5].fsState = TBSTATE_ENABLED; 
	tbb[5].fsStyle = BTNS_BUTTON; 
	tbb[5].dwData = 0; 
	tbb[5].iString = 0;

	tbb[6].iBitmap = 6; 
	tbb[6].idCommand = IDM_IMAGES_ROTATEIMAGE_FLIPVERTICAL; 
	tbb[6].fsState = TBSTATE_ENABLED; 
	tbb[6].fsStyle = BTNS_BUTTON; 
	tbb[6].dwData = 0; 
	tbb[6].iString = 0;

	// Create a toolbar with three standard file bitmaps and one
	// separator.
	// There are 15 items in IDB_STD_SMALL_COLOR.  However, because this is a
	// standard system-defined bitmap, the parameter that specifies the number
	// of button images in the bitmap (nBitmaps) is ignored, so it is set
	// to 0.
	hWndToolbar = CreateToolbarEx (hWndParent, 
		WS_CHILD | WS_BORDER | WS_VISIBLE | TBSTYLE_TOOLTIPS |  TBSTYLE_FLAT, 
		ID_TOOLBAR, 0, hInst, NULL, 
		tbb, ARRAYSIZE(tbb), 0, 0, 30, 30, sizeof (TBBUTTON));


	HIMAGELIST himlHot3 = ImageList_LoadImage(hInst,MAKEINTRESOURCE(IDB_TRAITEMENTMENU),30,7,RGB(255,0,255),IMAGE_BITMAP,LR_CREATEDIBSECTION);
	SendMessage(hWndToolbar,TB_SETHOTIMAGELIST, 0, (LPARAM)himlHot3);
	
	HIMAGELIST himlHot2 = ImageList_LoadImage(hInst,MAKEINTRESOURCE(IDB_TRAITEMENTMENU),30,7,RGB(255,0,255),IMAGE_BITMAP,LR_CREATEDIBSECTION);
	SendMessage(hWndToolbar,TB_SETIMAGELIST, 0, (LPARAM)himlHot2);

	//SendMessage(hWndToolbar, TB_SETHOTIMAGELIST, 0, (LPARAM)himlHot);
	return hWndToolbar;
} 

HWND CToolbar::CreateRebar(HWND hwndOwner, HINSTANCE g_hinst, const int &iNumToolbar)
{
   REBARINFO     rbi;
   REBARBANDINFO rbBand;
   //RECT          rc;
   HWND   /*hwndCB,*/ hwndTB, hwndRB;
   DWORD  dwBtnSize;
   INITCOMMONCONTROLSEX icex;

   icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
   icex.dwICC   = ICC_COOL_CLASSES|ICC_BAR_CLASSES;
   InitCommonControlsEx(&icex);

   hwndRB = CreateWindowEx(WS_EX_TOOLWINDOW,
                           REBARCLASSNAME,
                           NULL,
                           WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|
                           WS_CLIPCHILDREN|RBS_VARHEIGHT|CCS_NODIVIDER,
                           0,0,0,0,
                           hwndOwner,
                           NULL,
                           g_hinst,
                           NULL);
   if(!hwndRB)
      return NULL;

   SendMessage(hwndRB, RB_SETTEXTCOLOR,0,(LPARAM)(COLORREF)(0,0,0));


   // Initialize and send the REBARINFO structure.
   rbi.cbSize = sizeof(REBARINFO);  // Required when using this struct.
   rbi.fMask  = 0;
   rbi.himl   = (HIMAGELIST)NULL;
   if(!SendMessage(hwndRB, RB_SETBARINFO, 0, (LPARAM)&rbi))
      return NULL;

   // Initialize structure members that both bands will share.
   rbBand.cbSize = sizeof(REBARBANDINFO);  // Required
   rbBand.fMask  = RBBIM_COLORS | RBBIM_TEXT | 
                   RBBIM_STYLE | RBBIM_CHILD  | RBBIM_CHILDSIZE | 
                   RBBIM_SIZE;

   rbBand.fStyle = RBBS_CHILDEDGE;
   //rbBand.hbmBack= LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_BACKGRND));   
   rbBand.clrFore = RGB(0,0,0);
   rbBand.clrBack = GetSysColor(COLOR_BTNFACE);

	//CreateSolidBrush(RGB( 230, 230, 230))
   /*
   // Create the combo box control to be added.
   hwndCB = CreateComboBox(hwndRB, g_hinst);
   
   // Set values unique to the band with the combo box.
   GetWindowRect(hwndCB, &rc);
   rbBand.lpText     = "Combo Box";
   rbBand.hwndChild  = hwndCB;
   rbBand.cxMinChild = 0;
   rbBand.cyMinChild = rc.bottom - rc.top;
   rbBand.cx         = 200;

   // Add the band that has the combo box.
   SendMessage(hwndRB, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);*/

   // Create the toolbar control to be added.
   //hwndTB = CreateToolbar(hwndOwner, g_hinst);

   switch(iNumToolbar)
   {
		case 1:
			hwndTB = CreateToolBarMain(hwndOwner, g_hinst);
			break;

		case 2:
			hwndTB = CreateToolBarPicture(hwndOwner, g_hinst);
			break;

		case 3:
			hwndTB = CreateToolBarCatalogue(hwndOwner, g_hinst);
			break;

		case 4:
			hwndTB = CreateToolBarWinzip(hwndOwner, g_hinst);
			break;

		case 5:
			hwndTB = CreateToolBarTraitement(hwndOwner, g_hinst);
			break;

   }

   // Get the height of the toolbar.
   dwBtnSize = SendMessage(hwndTB, TB_GETBUTTONSIZE, 0,0);

   // Set values unique to the band with the toolbar.
   rbBand.lpText     = (LPSTR)"Tool Bar";
   rbBand.hwndChild  = hwndTB;
   rbBand.cxMinChild = 0;
   rbBand.cyMinChild = HIWORD(dwBtnSize);
   rbBand.cx         = 250;

   // Add the band that has the toolbar.
   SendMessage(hwndRB, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);


   return (hwndRB);

}
