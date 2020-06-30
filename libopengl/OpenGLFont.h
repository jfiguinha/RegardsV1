// OpenGLFont.h: interface for the COpenGLFont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLFONT_H__8B591CF5_03DA_4A11_BC3C_3FBB66233D58__INCLUDED_)
#define AFX_OPENGLFONT_H__8B591CF5_03DA_4A11_BC3C_3FBB66233D58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <gl\gl.h>								// Header File For The OpenGL32 Library
#include <gl\glu.h>								// Header File For The GLu32 Library
#include <gl\glaux.h>								// Header File For The GLaux Library

#define GLUT_BITMAP_HELVETICA_12	((void*)7)

typedef struct {
  const GLsizei width;
  const GLsizei height;
  const GLfloat xorig;
  const GLfloat yorig;
  const GLfloat advance;
  const GLubyte *bitmap;
} BitmapCharRec, *BitmapCharPtr;

typedef struct {
  const char *name;
  const int num_chars;
  const int first;
  const BitmapCharRec * const *ch;
} BitmapFontRec, *BitmapFontPtr;

typedef void *GLUTbitmapFont;

class COpenGLFont  
{
public:



	COpenGLFont();
	virtual ~COpenGLFont();

	static void glutBitmapCharacter(GLUTbitmapFont font, int c);

};

#endif // !defined(AFX_OPENGLFONT_H__8B591CF5_03DA_4A11_BC3C_3FBB66233D58__INCLUDED_)
