

#ifndef __TRAITEMENTINSTANCE__H__
#define __TRAITEMENTINSTANCE__H__

#include <vector>

class CTraitementInstance
{
public:
	CTraitementInstance(void);
	~CTraitementInstance(void);

	HWND hWnd;
	HANDLE hThreadTraitement;
	DWORD dwThreadId;
};

typedef std::vector<CTraitementInstance> TraitementInstanceVector;

#endif