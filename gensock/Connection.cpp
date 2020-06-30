// Connection.cpp: implementation of the CConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "Connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SOCKET_BUFFER_SIZE	512

class CConnectionPrv
{
	public:

		CConnectionPrv()
		{
		};

		~CConnectionPrv()
		{
		};

		int TestIsANumber(const char * string)
		{
			while (*string) 
			{
				if (!isdigit (*string)) 
				{
				  return (0);
				}
				string++;
			}
			return (1);
		}

		SOCKET	the_socket;
		char in_buffer[SOCKET_BUFFER_SIZE];
		char out_buffer[SOCKET_BUFFER_SIZE];
		UINT in_index;
		UINT out_index;
		UINT in_buffer_total;
		UINT out_buffer_total;
		UINT last_winsock_error;
		HANDLE	owner_task;
		fd_set	fds;
		timeval	timeout;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnection::CConnection()
{
	pimpl_ = new CConnectionPrv();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CConnection::~CConnection()
{
	delete pimpl_;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

SOCKET CConnection::get_socket() 
{ 
	return (pimpl_->the_socket); 
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

HANDLE CConnection::get_owner_task()
{ 
	return (pimpl_->owner_task); 
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int	CConnection::get_buffer(const int &wait)
{
	int retval;
	int bytes_read = 0;
	unsigned long ready_to_read = 0;

	// Use select to see if data is waiting...

	FD_ZERO (&pimpl_->fds);
	FD_SET  (pimpl_->the_socket, &pimpl_->fds);

	// if wait is set, we are polling, return immediately
	if (wait) 
	{
		pimpl_->timeout.tv_sec = 0;
	}
	else 
	{
		pimpl_->timeout.tv_sec = 30;
	}

	if ((retval = select (0, &pimpl_->fds, NULL, NULL, &pimpl_->timeout)) == SOCKET_ERROR) 
	{
		char what_error[256];
		int error_code = WSAGetLastError();

		if (error_code == WSAEINPROGRESS && wait) 
		{
			return (WAIT_A_BIT);
		}

		//wsprintf (what_error, "connection::get_buffer() unexpected error from select: %d",error_code);
		//complain (what_error);
	}

	// if we don't want to wait
	if (!retval && wait) 
	{
		return (WAIT_A_BIT);
	}

	// we have data waiting...
	bytes_read = recv(pimpl_->the_socket, pimpl_->in_buffer, SOCKET_BUFFER_SIZE, 0);

	// just in case.

	if (bytes_read == 0) 
	{
		// connection terminated (semi-) gracefully by the other side
		return (ERR_NOT_CONNECTED);
	}

	if (bytes_read == SOCKET_ERROR) 
	{
		char what_error[256];

		int ws_error = WSAGetLastError();
		switch (ws_error) 
		{
			  // all these indicate loss of connection (are there more?)
			case WSAENOTCONN:
			case WSAENETDOWN:
			case WSAENETUNREACH:
			case WSAENETRESET:
			case WSAECONNABORTED:
			case WSAECONNRESET:
			  return (ERR_NOT_CONNECTED);
			  break;

			case WSAEWOULDBLOCK:
			  return (WAIT_A_BIT);
			  break;

			default:
			  //wsprintf (what_error, "connection::get_buffer() unexpected error: %d", ws_error);
			  //complain (what_error);
				break;
		}
	}

	// reset buffer indices.
	pimpl_->in_buffer_total = bytes_read;
	pimpl_->in_index = 0;
	return (0);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int	CConnection::close(void)
{
	if (closesocket(pimpl_->the_socket) == SOCKET_ERROR)
		return (ERR_CLOSING);
	else
		return (0);
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

int	CConnection::getachar(const int &wait, char * ch)
{
	int retval;

	if (pimpl_->in_index >= pimpl_->in_buffer_total) 
	{
		if ((retval = get_buffer(wait)))
			return (retval);
	}

	*ch = pimpl_->in_buffer[pimpl_->in_index++];
	return (0);
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

int	CConnection::put_data(const char * data,const ULONG &length)
{
  int num_sent;
  int retval;
  ULONG lLocalLength = length;

  FD_ZERO (&pimpl_->fds);
  FD_SET  (pimpl_->the_socket, &pimpl_->fds);

  pimpl_->timeout.tv_sec = 30;

	while (lLocalLength > 0) 
	{
		if ((retval = select (0, NULL, &pimpl_->fds, NULL, &pimpl_->timeout)) == SOCKET_ERROR) 
		{
			char what_error[256];
			int error_code = WSAGetLastError();

			/*
			wsprintf (what_error, "connection::put_data() unexpected error from select: %d",error_code);
			complain (what_error);*/
		}

		num_sent = send (pimpl_->the_socket, data,
				 lLocalLength > 1024 ? 1024 : (int)length,
				 0);

		if (num_sent == SOCKET_ERROR) 
		{
			char what_error[256];
			int ws_error = WSAGetLastError();
			switch (ws_error) 
			{
				// this is the only error we really expect to see.
				case WSAENOTCONN:
					return (ERR_NOT_CONNECTED);
					break;

				// seems that we can still get a block
				case WSAEWOULDBLOCK:
				case WSAEINPROGRESS:
					break;

				default:
					/*
					wsprintf (what_error,
					  "connection::put_data() unexpected error from send(): %d",
					  ws_error);
					complain (what_error);*/
					return (ERR_SENDING_DATA);
			}
		}	
		else 
		{
			lLocalLength -= num_sent;
			data += num_sent;
		}
	}

  return (0);
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

int	CConnection::put_data_buffered(const char * data,const ULONG &length)
{
	unsigned int sorta_sent = 0;
	int retval;
	ULONG m_lLocalLength = length;

	while (m_lLocalLength) 
	{
		if ((pimpl_->out_index + m_lLocalLength) < SOCKET_BUFFER_SIZE) 
		{
			// we won't overflow, simply copy into the buffer
			memcpy (pimpl_->out_buffer + pimpl_->out_index, data, (size_t) m_lLocalLength);
			pimpl_->out_index += (unsigned int) m_lLocalLength;
			m_lLocalLength = 0;
		}
		else 
		{
			unsigned int orphaned_chunk = SOCKET_BUFFER_SIZE - pimpl_->out_index;
			// we will overflow, handle it
			memcpy (pimpl_->out_buffer + pimpl_->out_index, data, orphaned_chunk);
			// send this buffer...
			if ((retval = put_data (pimpl_->out_buffer, SOCKET_BUFFER_SIZE))) 
			{
				return (retval);
			}
			m_lLocalLength -= orphaned_chunk;
			pimpl_->out_index = 0;
			data += orphaned_chunk;
		}
	}

	return (0);
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

int	CConnection::put_data_flush()
{
	int retval;

	if ((retval = put_data (pimpl_->out_buffer, pimpl_->out_index)))
		return (retval);
	else
		pimpl_->out_index = 0;

	return(0);
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

int CConnection::get_connected(const char * hostname,const char * service)
{
	hostent *	hostentry;
	servent *	serventry;
	unsigned long ip_address;
	sockaddr_in sa_in;
	int			our_port;
	int			not = 0;
	int			retval, err_code;
	unsigned long ioctl_blocking = 1;
	char			message[512];

	// if the ctor couldn't get a buffer
	if (!pimpl_->in_buffer || !pimpl_->out_buffer)
		return (ERR_CANT_MALLOC);

	// --------------------------------------------------
	// resolve the service name
	//

	// If they've specified a number, just use it.
	if (pimpl_->TestIsANumber(service)) 
	{
		char * tail;
		our_port = (int)strtol(service, &tail, 10);
		if (tail == service) 
		{
			return (ERR_CANT_RESOLVE_SERVICE);
		} 
		else 
		{
			our_port = htons (our_port);
		}
	} 
	else 
	{
		// we have a name, we must resolve it.
		serventry = getservbyname (service, (LPSTR)"tcp");

		if (serventry)
			our_port = serventry->s_port;
		else 
		{
			retval = WSAGetLastError();
			// Chicago beta is throwing a WSANO_RECOVERY here...
			if ((retval == WSANO_DATA) || (retval == WSANO_RECOVERY)) 
			{
				return (ERR_CANT_RESOLVE_SERVICE);
			} 
			else 
			{
				return (retval - 5000);
			}
		}
	}

	// --------------------------------------------------
	// resolve the hostname/ipaddress
	//

	if ((ip_address = inet_addr (hostname)) != INADDR_NONE) 
	{
		sa_in.sin_addr.s_addr = ip_address;
	}
	else 
	{
		if ((hostentry = gethostbyname(hostname)) == NULL) 
		{
		  return (ERR_CANT_RESOLVE_HOSTNAME);
		}
		sa_in.sin_addr.s_addr = *(long far *)hostentry->h_addr;
	}


	// --------------------------------------------------
	// get a socket
	//

	if ((pimpl_->the_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
	{
		return (ERR_CANT_GET_SOCKET);
	}

	sa_in.sin_family = AF_INET;
	sa_in.sin_port = our_port;

	// set socket options.  DONTLINGER will give us a more graceful disconnect

	setsockopt(pimpl_->the_socket, SOL_SOCKET, SO_DONTLINGER, (char *) &not, sizeof(not));

	// get a connection

	if ((retval = connect (pimpl_->the_socket,
			 (struct sockaddr *)&sa_in,
			 sizeof(struct sockaddr_in))==SOCKET_ERROR)) 
	{
		switch ((err_code = WSAGetLastError())) 
		{
			  /* twiddle our thumbs until the connect succeeds */
			case WSAEWOULDBLOCK:
			  break;
			case WSAECONNREFUSED:
			  return (ERR_CONNECTION_REFUSED);
			  break;
			default:
			  //wsprintf(message, "unexpected error %d from winsock", err_code);
			  //complain(message);
			  return (ERR_CANT_CONNECT);
			  break;
		}
	}

	pimpl_->owner_task = GetModuleHandle(NULL);

	// Make this a non-blocking socket
	ioctlsocket (pimpl_->the_socket, FIONBIO, &ioctl_blocking);

	// make the FD_SET and timeout structures for later operations...

	FD_ZERO (&pimpl_->fds);
	FD_SET  (pimpl_->the_socket, &pimpl_->fds);

	// normal timeout, can be changed by the wait option.
	pimpl_->timeout.tv_sec = 30;
	pimpl_->timeout.tv_usec = 0;

	return (0);
}