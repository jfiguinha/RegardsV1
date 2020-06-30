// Crop.h: interface for the CCrop class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CROP_H__1DECE239_FD14_440E_8C9B_6812FFFAAE0C__INCLUDED_)
#define AFX_CROP_H__1DECE239_FD14_440E_8C9B_6812FFFAAE0C__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include "Dessin.h"

class CCrop : public CDessin  
{
public:
	void GetPos(RECT &rc);

	void InitPoint(const long &m_lx, const long &m_ly, const long &m_lHScroll, const long &m_lVScroll);
	void MouseMove(const long &xNewSize, const long &yNewSize, const long &m_lHScroll, const long &m_lVScroll);
	CCrop();
	virtual ~CCrop();
	void Dessiner(HDC hdc, const long &m_lHScroll, const long &m_lVScroll);
	void Selection(const int &yNewSize, const int &xNewSize);

private:
	POINT * ptSelection;
	int iSelect;

	void VerifierValiditerPoint(POINT &pt,const bool &m_bX);

};

#endif // !defined(AFX_CROP_H__1DECE239_FD14_440E_8C9B_6812FFFAAE0C__INCLUDED_)
