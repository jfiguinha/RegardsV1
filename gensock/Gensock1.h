// Gensock1.h: interface for the CGensock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENSOCK1_H__157CB77B_E320_4733_AC7C_29418D0E8049__INCLUDED_)
#define AFX_GENSOCK1_H__157CB77B_E320_4733_AC7C_29418D0E8049__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Connection.h"
#include <vector>

typedef std::vector<CConnection> ConnectionVecteur;

class CGensock  
{
public:
	CGensock();
	virtual ~CGensock();

	int init_winsock(void);
	void deinit_winsock(void);
	int gensock_connect (char * hostname, char * service);
	int	gensock_getchar(int wait, char * ch);
	int	gensock_put_data(char * data, unsigned long length);
	int gensock_close();
	int	gensock_gethostname(char * name, int namelen);
	int	gensock_put_data_buffered (char * data, unsigned long length);
	int	gensock_put_data_flush();

private:

	CConnection * m_cConnection;
	ConnectionVecteur m_LocalConnectionVecteur;

	int network_initialized;

};

#endif // !defined(AFX_GENSOCK1_H__157CB77B_E320_4733_AC7C_29418D0E8049__INCLUDED_)
