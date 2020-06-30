

#ifndef __ALBUMDATA__H__
#define __ALBUMDATA__H__

#include <vector>
#include "iconefile.h"


class CAlbumData
{
public:
	CAlbumData(void);
	~CAlbumData(void);

	char m_szAlbumName[255];
	int iNbFiles;

#ifndef SQLSERVERCE
	IconeFileVector m_IconeFileVector;
#endif

};

typedef std::vector<CAlbumData> AlbumDataVector;


#endif