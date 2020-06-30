
#include "3ds.h"
#define MAX_TEXTURES 8

#ifndef __3DS__H__
#define __3DS__H__

class C3ds
{
public:
	C3ds(void);
	~C3ds(void);

	int DrawObject(const float &m_fRed,	const float & m_fGreen,	const float & m_fBlue);
	bool Load3dsFile(const char *mFile,float &distance);

private:
	data3ds_t	* model;
};

#endif