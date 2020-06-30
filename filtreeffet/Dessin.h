// Dessin.h: interface for the CDessin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DESSIN_H__C251A552_F4E5_4E27_AF65_1CE40C430CA3__INCLUDED_)
#define AFX_DESSIN_H__C251A552_F4E5_4E27_AF65_1CE40C430CA3__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

class CDessin  
{
public:
	void SetRatio(const float &m_fValue);
	void SetMaxPosition(const RECT &m_rcPicture);
	CDessin();
	virtual ~CDessin();
	static void DrawARectangle(HDC hdc, const RECT &rc, const COLORREF &rgb) ;
	static void DessinerRectangleVide(HDC hdc,const int &iTaille, const RECT &rc, const COLORREF &rgb);
	void DessinerCarre(HDC &destDC,const int &iLargeur, const int &iHauteur, const int & iMarge, const int &iPosX = 0, const int &iPosY = 0, const COLORREF &rgb = RGB(0,0,0));

	virtual void Dessiner(HDC hdc, const long &m_lHScroll, const long &m_lVScroll){};
	virtual void Selection(const int &yNewSize, const int &xNewSize){};
	virtual void MouseMove(const long &m_lx, const long &m_ly, const long &m_lHScroll, const long &m_lVScroll){};
	virtual void InitPoint(const long &m_lx, const long &m_ly, const long &m_lHScroll, const long &m_lVScroll){};
	virtual void GetPos(RECT &rc){};

protected:
	RECT m_rcAffichage;
	float m_fRatioValue;

};	

#endif // !defined(AFX_DESSIN_H__C251A552_F4E5_4E27_AF65_1CE40C430CA3__INCLUDED_)
