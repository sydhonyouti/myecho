#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
using namespace std;

/*------------------------------------------------------------------
 * connectsock - allocate & connect a client socket using TCP or UDP
 *------------------------------------------------------------------
 */
SOCKET connectsock (char *host, char *service, char *protocol)
{	// Parameter List:
	// [IN] host    : String containing remote host's IP address (eg. "10.30.201.110")
	// [IN] service : String containing remote port number (eg. "7")
	// [IN] protocol: String containing protocol type (either "tcp" or "udp")
	// Return value : SOCKET handle of connected socket.
	//	NOTE: This function requires winsock2.h header file and wsock32.lib library file.

	struct sockaddr_in client; // an Intenet endpoint address
	int status;				   // status of connection attempt
	SOCKET s;				   // socket descriptor

	/* Allocate and connect socket - which type depends upon protocol */
	if (_stricmp(protocol, "tcp") == 0)			// Create a TCP socket
	{	s = socket(AF_INET, SOCK_STREAM, 0);
		if (s < 0 || s == INVALID_SOCKET)
		{	cout << "Cannot create socket" << endl;
			return INVALID_SOCKET;
		}
		
		memset(&client, 0, sizeof(client));					// zero the client struct

		client.sin_family = AF_INET;						// Set the address family
		client.sin_port = htons( (u_short) atoi(service));	// Set the port number
		client.sin_addr.s_addr = inet_addr(host);			// Set remote IP address

		/* Connect the socket */
		status = connect(s, (LPSOCKADDR) &client, sizeof(SOCKADDR));
		if (status == SOCKET_ERROR) 
		{	cout << "Remote host/service not found - or connection refused" << endl;
			return INVALID_SOCKET;
		}
	}
	else if (_stricmp(protocol,"udp") == 0)			// Create a UDP socket
	{	s = socket(AF_INET, SOCK_DGRAM,  0);
		if (s < 0 || s == INVALID_SOCKET)
		{	cout << "Cannot create socket" << endl;
			return INVALID_SOCKET;
		}
	}
	else
	{	cout << "Invalid Protocol" << endl;			//I give up! 
		return INVALID_SOCKET;
	}		
	return s;
}