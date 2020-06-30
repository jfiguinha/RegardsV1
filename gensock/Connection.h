// Connection.h: interface for the CConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTION_H__AA5EED7F_B172_4AE4_9455_8963B44A2478__INCLUDED_)
#define AFX_CONNECTION_H__AA5EED7F_B172_4AE4_9455_8963B44A2478__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <winsock.h>

#define ERR_CANT_MALLOC			4001
#define ERR_SENDING_DATA		4002
#define ERR_INITIALIZING		4003
#define ERR_VER_NOT_SUPPORTED		4004
#define ERR_EINVAL			4005
#define ERR_SYS_NOT_READY		4006
#define ERR_CANT_RESOLVE_HOSTNAME	4007
#define ERR_CANT_GET_SOCKET		4008
#define ERR_READING_SOCKET		4009
#define ERR_NOT_A_SOCKET		4010
#define ERR_BUSY			4011
#define ERR_CLOSING			4012
#define WAIT_A_BIT			4013
#define ERR_CANT_RESOLVE_SERVICE	4014
#define ERR_CANT_CONNECT		4015
#define ERR_NOT_CONNECTED		4016
#define ERR_CONNECTION_REFUSED		4017

#define ERR_NO_ERROR_CODE		-5000	/* this is returned by misbehaving stacks that
						 * fail, but don't set an error code
						 */
class CConnectionPrv;

class CConnection  
{
public:
	CConnection();
	virtual ~CConnection();


	SOCKET get_socket();

	HANDLE get_owner_task();

	int	get_buffer(const int &wait);
	int	close ();
	int	getachar (const int &wait, char * ch);
	int	put_data (const char * data,const ULONG &length);
	int	put_data_buffered (const char * data,const ULONG &length);
	int	put_data_flush ();
	int get_connected(const char * hostname,const char * service);

private:
	
	CConnectionPrv * pimpl_;

};

#endif // !defined(AFX_CONNECTION_H__AA5EED7F_B172_4AE4_9455_8963B44A2478__INCLUDED_)
