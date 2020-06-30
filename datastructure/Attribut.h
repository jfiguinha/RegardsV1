

#ifndef __ATTRIBUT__H__
#define __ATTRIBUT__H__

#include <vector>

class CAttribut
{
public:
	CAttribut(void);
	~CAttribut(void);

	int iNumAttribut;
	char cLibelle[MAX_PATH];
};

typedef std::vector<CAttribut> AttributVector;

#endif