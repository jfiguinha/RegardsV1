#ifndef __CLOUDBOX__H__
#define __CLOUDBOX__H__

#include "../libWindow/LocalDialog.h"

class CIBitmap;

class CCloudsBox : public CLocalDialog
{
public:
	CCloudsBox(HINSTANCE hInstance);
	~CCloudsBox(void);


	inline int GetReturnValue()
	{
		return m_iReturnValue;
	}

	inline COLORREF GetRGBcolor1()
	{
		return m_RGBcolor1;
	}

	inline COLORREF GetRGBcolor2()
	{
		return m_RGBcolor2;
	}

	inline float GetAmplitude()
	{
		return m_Amplitude;
	}

	inline float GetFrequence()
	{
		return m_Frequence;
	}

	inline int GetOctave()
	{
		return m_Octave;
	}

	CIBitmap * GetBitmap()
	{
		return m_ciBitmapTestBitmap;
	}

private:

	LRESULT OnInitDialog(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHScroll(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnActivate(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDblClick(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CCloudsBox::CallColorPicker(HWND hDlg,COLORREF &clValue);

	CIBitmap * m_ciBitmapTestBitmap;
	CIBitmap * m_ciBitmapColorBox1;
	CIBitmap * m_ciBitmapColorBox2;

	COLORREF m_RGBcolor1;
	COLORREF m_RGBcolor2;

	//CIBitmap * m_Source;

	float m_Amplitude;
	float m_Frequence;
	int m_Octave;

	int m_iAmplitude;
	int m_iFrequence;

	int m_iReturnValue;

	HINSTANCE m_hInstance;

};

#endif
