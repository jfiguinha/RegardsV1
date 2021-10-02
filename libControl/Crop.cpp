// Crop.cpp: implementation of the CCrop class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Crop.h"
#include "resource.h"
#include "objet.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCrop::CCrop()
{
	ptSelection = new POINT[4];
	iSelect = 0;
}

CCrop::~CCrop()
{
	if (ptSelection != NULL)
		delete[] ptSelection;

	ptSelection = NULL;
}

void CCrop::Dessiner(HDC hDC, const long &m_lHScroll, const long &m_lVScroll)
{
	//Ajout d'un rectangle pour tester le crop
	//Trouver le point central

	//Point 0 : NO
	//Point 1 : NE
	//Point 2 : SE
	//Point 3 : SO


	RECT rcTemp;

	rcTemp.left = (long)(ptSelection[0].x * m_fRatioValue) + m_rcAffichage.left - m_lHScroll;
	rcTemp.right = (long)(ptSelection[1].x * m_fRatioValue) + m_rcAffichage.left - m_lHScroll;
	rcTemp.top = (long)(ptSelection[0].y * m_fRatioValue) + m_rcAffichage.top - m_lVScroll;
	rcTemp.bottom = (long)(ptSelection[3].y * m_fRatioValue) + m_rcAffichage.top - m_lVScroll;

	DessinerRectangleVide(hDC,1,rcTemp,RGB(0,0,255));


	RECT rcTemp2;

	//Dessin de tous les petits rectangles

	rcTemp2.left = rcTemp.left - 4;
	rcTemp2.right = rcTemp.left + 4;
	rcTemp2.top = rcTemp.top - 4;
	rcTemp2.bottom = rcTemp.top + 4;

	DessinerRectangleVide(hDC,1,rcTemp2,RGB(0,0,255));

	//Dessin de tous les petits rectangles

	rcTemp2.left = rcTemp.right - 4;
	rcTemp2.right = rcTemp.right + 4;
	rcTemp2.top = rcTemp.top - 4;
	rcTemp2.bottom = rcTemp.top + 4;

	DessinerRectangleVide(hDC,1,rcTemp2,RGB(0,0,255));

	//Dessin de tous les petits rectangles

	rcTemp2.left = rcTemp.right - 4;
	rcTemp2.right = rcTemp.right + 4;
	rcTemp2.top = rcTemp.bottom - 4;
	rcTemp2.bottom = rcTemp.bottom + 4;

	DessinerRectangleVide(hDC,1,rcTemp2,RGB(0,0,255));

	//Dessin de tous les petits rectangles

	rcTemp2.left = rcTemp.left - 4;
	rcTemp2.right = rcTemp.left + 4;
	rcTemp2.top = rcTemp.bottom - 4;
	rcTemp2.bottom = rcTemp.bottom + 4;

	DessinerRectangleVide(hDC,1,rcTemp2,RGB(0,0,255));

	//Dessin de tous les petits rectangles

	rcTemp2.left = (rcTemp.right - rcTemp.left) / 2 + rcTemp.left - 4;
	rcTemp2.right = (rcTemp.right - rcTemp.left) / 2 + rcTemp.left + 4;
	rcTemp2.top = rcTemp.bottom - 4;
	rcTemp2.bottom = rcTemp.bottom + 4;

	DessinerRectangleVide(hDC,1,rcTemp2,RGB(0,0,255));

	//Dessin de tous les petits rectangles

	rcTemp2.left = (rcTemp.right - rcTemp.left) / 2 + rcTemp.left - 4;
	rcTemp2.right = (rcTemp.right - rcTemp.left) / 2 + rcTemp.left + 4;
	rcTemp2.top = rcTemp.top - 4;
	rcTemp2.bottom = rcTemp.top + 4;

	DessinerRectangleVide(hDC,1,rcTemp2,RGB(0,0,255));

	//Dessin de tous les petits rectangles

	rcTemp2.left = rcTemp.left - 4;
	rcTemp2.right = rcTemp.left + 4;
	rcTemp2.top = (rcTemp.top - rcTemp.bottom) / 2 + rcTemp.bottom - 4;
	rcTemp2.bottom = (rcTemp.top - rcTemp.bottom) / 2 + rcTemp.bottom + 4;

	DessinerRectangleVide(hDC,1,rcTemp2,RGB(0,0,255));

	//Dessin de tous les petits rectangles

	rcTemp2.left = rcTemp.right - 4;
	rcTemp2.right = rcTemp.right + 4;
	rcTemp2.top = (rcTemp.top - rcTemp.bottom) / 2 + rcTemp.bottom - 4;
	rcTemp2.bottom = (rcTemp.top - rcTemp.bottom) / 2 + rcTemp.bottom + 4;

	DessinerRectangleVide(hDC,1,rcTemp2,RGB(0,0,255));
}

void CCrop::Selection(const int &y, const int &x)
{
	//Changement de souris
	//Test par rapport au x et y
	//Différent cas

	long yNewSize, xNewSize;

	yNewSize = (long)((y - m_rcAffichage.top) / m_fRatioValue);
	xNewSize = (long)((x - m_rcAffichage.left) / m_fRatioValue);

	iSelect = 0;

	if((xNewSize > (ptSelection[0].x - 4) && xNewSize < (ptSelection[0].x + 4)))
		iSelect = 1;

	if((xNewSize > (ptSelection[1].x - 4) && xNewSize < (ptSelection[1].x + 4)))
		iSelect = 2;

	if((yNewSize > (ptSelection[0].y - 4) && yNewSize < (ptSelection[0].y + 4)))
	{
		switch(iSelect)
		{
			case 0:
				iSelect = 3;
				break;
			case 1:
				iSelect = 4;
				break;
			case 2:
				iSelect = 5;
				break;
		}
	}


	if((yNewSize > (ptSelection[2].y - 4) && yNewSize < (ptSelection[2].y + 4)))
	{
		switch(iSelect)
		{
			case 0:
				iSelect = 6;
				break;
			case 1:
				iSelect = 7;
				break;
			case 2:
				iSelect = 8;
				break;
		}
	}


	switch(iSelect)
	{
		case 2:
		case 1:
			SetCursor (LoadCursor (NULL, IDC_SIZEWE));
			break;
		case 3:
		case 6:
			SetCursor (LoadCursor (NULL, IDC_SIZENS));
			break;
		case 8:
		case 4:
			SetCursor (LoadCursor (NULL, IDC_SIZENWSE));
			break;
		case 7:
		case 5:
			SetCursor (LoadCursor (NULL, IDC_SIZENESW));
			break;
		default:
			SetCursor (LoadCursor (hInstance, MAKEINTRESOURCE(IDC_CURSELECT))) ;

	}
}

void CCrop::VerifierValiditerPoint(POINT &pt,const bool &m_bX)
{
	if(m_bX)
	{
		if(pt.x > (m_rcAffichage.right - m_rcAffichage.left))
			pt.x = m_rcAffichage.right - m_rcAffichage.left;

		pt.x = (long)(pt.x / m_fRatioValue);

		if(pt.x < 0)
			pt.x = 0;
	}
	else
	{
		if(pt.y > (m_rcAffichage.bottom - m_rcAffichage.top))
			pt.y = (m_rcAffichage.bottom - m_rcAffichage.top);

		pt.y = (long)(pt.y / m_fRatioValue);

		if(pt.y < 0)
			pt.y = 0;

	}
}

void CCrop::MouseMove(const long &xNewSize, const long &yNewSize, const long &m_lHScroll, const long &m_lVScroll)
{
	//Dessiner le rectangle
	switch(iSelect)
	{
		case 0:
			{
				ptSelection[1].x = (xNewSize + m_lHScroll) - m_rcAffichage.left;
				ptSelection[1].y = ptSelection[0].y;

				ptSelection[2].x = (xNewSize + m_lHScroll) - m_rcAffichage.left;
				ptSelection[2].y = (yNewSize + m_lVScroll) - m_rcAffichage.top;

				ptSelection[3].x = ptSelection[0].x;
				ptSelection[3].y = (yNewSize + m_lVScroll) - m_rcAffichage.top;

				VerifierValiditerPoint(ptSelection[1],true);
				VerifierValiditerPoint(ptSelection[2],true);
				VerifierValiditerPoint(ptSelection[2],false);
				VerifierValiditerPoint(ptSelection[3],false);

				SetCursor (LoadCursor (hInstance, MAKEINTRESOURCE(IDC_CURSELECT))) ;

			}
			break;

		case 1:
			{
				ptSelection[0].x = (xNewSize + m_lHScroll) - m_rcAffichage.left;
				ptSelection[3].x = (xNewSize + m_lHScroll) - m_rcAffichage.left;

				VerifierValiditerPoint(ptSelection[0],true);
				VerifierValiditerPoint(ptSelection[3],true);

				SetCursor (LoadCursor (NULL, IDC_SIZEWE));
			}
			break;

		case 2:
			{
				ptSelection[1].x = (xNewSize + m_lHScroll) - m_rcAffichage.left;
				ptSelection[2].x = (xNewSize + m_lHScroll) - m_rcAffichage.left;
				SetCursor (LoadCursor (NULL, IDC_SIZEWE));

				VerifierValiditerPoint(ptSelection[1],true);
				VerifierValiditerPoint(ptSelection[2],true);

			}
			break;

		case 3:
			{
				ptSelection[0].y = (yNewSize + m_lVScroll) - m_rcAffichage.top;
				ptSelection[1].y = (yNewSize + m_lVScroll) - m_rcAffichage.top;

				VerifierValiditerPoint(ptSelection[0],false);
				VerifierValiditerPoint(ptSelection[1],false);

				SetCursor (LoadCursor (NULL, IDC_SIZENS));
			}
			break;

		case 6:
			{
				ptSelection[2].y = (yNewSize + m_lVScroll) - m_rcAffichage.top;
				ptSelection[3].y = (yNewSize + m_lVScroll) - m_rcAffichage.top;

				VerifierValiditerPoint(ptSelection[2],false);
				VerifierValiditerPoint(ptSelection[3],false);


				SetCursor (LoadCursor (NULL, IDC_SIZENS));
			}
			break;

		case 4:
			{
				//NO - SE
				//Point bougean 0 NO
				//Point 1 bouge en x  NE
				//Point 2 ne bouge pas SE
				//Point 3 bouge en y SO

				ptSelection[0].x = (xNewSize + m_lHScroll) - m_rcAffichage.left;
				ptSelection[0].y = (yNewSize + m_lVScroll) - m_rcAffichage.top;
				ptSelection[1].y = (yNewSize + m_lVScroll) - m_rcAffichage.top;
				ptSelection[3].x = (xNewSize + m_lHScroll) - m_rcAffichage.left;

				SetCursor (LoadCursor (NULL, IDC_SIZENWSE));

				VerifierValiditerPoint(ptSelection[0],true);
				VerifierValiditerPoint(ptSelection[0],false);
				VerifierValiditerPoint(ptSelection[1],false);
				VerifierValiditerPoint(ptSelection[3],true);

			}
			break;

		case 5:
			{
				//NE - SO
				//Point bougean 3 
				//Point 0 bouge en y et en x
				//Point 1 ne bouge pas
				//Point 2 bouge en x et en y

				ptSelection[0].y = (yNewSize + m_lVScroll) - m_rcAffichage.top;
				ptSelection[1].x = (xNewSize + m_lHScroll) - m_rcAffichage.left;
				ptSelection[1].y = (yNewSize + m_lVScroll) - m_rcAffichage.top;
				ptSelection[2].x = (xNewSize + m_lHScroll) - m_rcAffichage.left;

				SetCursor (LoadCursor (NULL, IDC_SIZENESW));

				VerifierValiditerPoint(ptSelection[0],false);
				VerifierValiditerPoint(ptSelection[1],true);
				VerifierValiditerPoint(ptSelection[1],false);
				VerifierValiditerPoint(ptSelection[2],true);



			}
			break;

		case 7:
			{
				//SO - NE
				ptSelection[0].x = (xNewSize + m_lHScroll) - m_rcAffichage.left;
				ptSelection[3].x = (xNewSize + m_lHScroll) - m_rcAffichage.left;
				ptSelection[3].y = (yNewSize + m_lVScroll) - m_rcAffichage.top;
				ptSelection[2].y = (yNewSize + m_lVScroll) - m_rcAffichage.top;

				SetCursor (LoadCursor (NULL, IDC_SIZENESW));

				VerifierValiditerPoint(ptSelection[0],true);
				VerifierValiditerPoint(ptSelection[3],true);
				VerifierValiditerPoint(ptSelection[3],false);
				VerifierValiditerPoint(ptSelection[2],false);


			}
			break;

		case 8:
			{
				//SE - NO
				ptSelection[1].x = (xNewSize + m_lHScroll) - m_rcAffichage.left;
				ptSelection[2].x = (xNewSize + m_lHScroll) - m_rcAffichage.left;
				ptSelection[2].y = (yNewSize + m_lVScroll) - m_rcAffichage.top;
				ptSelection[3].y = (yNewSize + m_lVScroll) - m_rcAffichage.top;
				SetCursor (LoadCursor (NULL, IDC_SIZENWSE));

				VerifierValiditerPoint(ptSelection[1],true);
				VerifierValiditerPoint(ptSelection[2],true);
				VerifierValiditerPoint(ptSelection[2],false);
				VerifierValiditerPoint(ptSelection[3],false);



			}
			break;
	}


}

void CCrop::InitPoint(const long &m_lx, const long &m_ly, const long &m_lHScroll, const long &m_lVScroll)
{
	int x, y;
	if(iSelect == 0)
	{
		x = (m_lx + m_lHScroll) - m_rcAffichage.left;
		y = (m_ly + m_lVScroll) - m_rcAffichage.top;

		if(x > (m_rcAffichage.right - m_rcAffichage.left))
			x = (m_rcAffichage.right - m_rcAffichage.left);

		if(x < 0)
			x = 0;

		if(y > (m_rcAffichage.bottom - m_rcAffichage.top))
			y = (m_rcAffichage.bottom - m_rcAffichage.top);

		if(y < 0)
			y = 0;

		for(int i = 0;i < 4; i++)
		{
			ptSelection[i].x = (long)(x / m_fRatioValue);
			ptSelection[i].y = (long)(y / m_fRatioValue);
		}
	}
}

void CCrop::GetPos(RECT &rc)
{
	rc.left = ptSelection[0].x;
	rc.right = ptSelection[1].x;
	rc.top = ptSelection[0].y;
	rc.bottom = ptSelection[3].y;
}
