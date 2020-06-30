// Gensock1.cpp: implementation of the CGensock class.
//
//////////////////////////////////////////////////////////////////////

#include "Gensock1.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGensock::CGensock()
{
	m_cConnection = new CConnection();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CGensock::~CGensock()
{
	m_LocalConnectionVecteur.clear();
	delete m_cConnection;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int CGensock::init_winsock(void)
{
	int retval;
	WSADATA winsock_data;
	WORD version_required = 0x0101; /* Version 1.1 */

	retval = WSAStartup (version_required, &winsock_data);

	switch (retval) 
	{
		case 0:
			/* successful */
			break;

		case WSASYSNOTREADY:
			return (ERR_SYS_NOT_READY);
			break;

		case WSAEINVAL:
			return (ERR_EINVAL);
			break;

		case WSAVERNOTSUPPORTED:
			return (ERR_VER_NOT_SUPPORTED);
			break;
	}

	network_initialized = 1;
	return (0);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int CGensock::gensock_connect (char * hostname, char * service)
{
	int retval;

	if (!m_cConnection)
		return (ERR_INITIALIZING);

	if (m_LocalConnectionVecteur.size() < 1)
		init_winsock();

	m_LocalConnectionVecteur.push_back(*m_cConnection);

	if ((retval = m_cConnection->get_connected(hostname, service))) 
	{
		gensock_close();
		return (retval);
	}

	return (0);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CGensock::deinit_winsock(void)
{
  network_initialized = 0;
  WSACleanup();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int CGensock::gensock_close()
{
	int retval;

	if (!m_cConnection)
		return (ERR_NOT_A_SOCKET);

	if ((retval = m_cConnection->close()))
		return (retval);

	m_LocalConnectionVecteur.erase(m_cConnection);

	if (m_LocalConnectionVecteur.size() < 1) 
	{
		deinit_winsock();
	}

  return (0);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int	CGensock::gensock_getchar(int wait, char * ch)
{
	int retval = 0;

	if (!m_cConnection)
		return (ERR_NOT_A_SOCKET);

	if ((retval = m_cConnection->getachar(wait, ch)))
		return (retval);
	else
		return (0);

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int	CGensock::gensock_put_data(char * data, unsigned long length)
{
	int retval = 0;

	if (!m_cConnection)
		return (ERR_NOT_A_SOCKET);

	if ((retval = m_cConnection->put_data(data, length)))
		return (retval);

	return (0);

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int	CGensock::gensock_gethostname(char * name, int namelen)
{
	int retval;
	if ((retval = gethostname(name, namelen))) 
		return (retval - 5000);
	else 
		return (0);

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int	CGensock::gensock_put_data_buffered (char * data, unsigned long length)
{
	int retval = 0;

	if (!m_cConnection)
		return (ERR_NOT_A_SOCKET);

	if ((retval = m_cConnection->put_data_buffered(data, length)))
		return (retval);

	return (0);

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
	
int	CGensock::gensock_put_data_flush()
{
	int retval = 0;

	if (!m_cConnection)
		return (ERR_NOT_A_SOCKET);

	if ((retval = m_cConnection->put_data_flush() ))
		return (retval);

	return (0);
}
