//Sydney Honyouti
//myecho.cpp: This file will do four things, accept a socket service name or a socket number, accept a 
//			  DNS host name or an IP address
//Due Date: 2/4/22

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <string>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

const int MAXBUF = 128;
const int MAXNAME = 80;

/*------------------------------------------------------------------
 * connectsock - allocate & connect a client socket using TCP or UDP
 *------------------------------------------------------------------
 */
SOCKET connectsock(char* host, char* service, char* protocol)
{ // Parameter List:
	// [IN] host: String containing remote host's IP address (eg. "10.30.201.110")
	// [IN] service : String containing remote port number (eg. "7")
	// [IN] protocol: String containing protocol type (either "tcp" or "udp")
	// Return value : SOCKET handle of connected socket.
	// NOTE: This function requires winsock2.h header file and wsock32.lib library file.
	struct sockaddr_in client; // an Intenet endpoint address
	int status;    // status of connection attempt
	SOCKET s;    // socket descriptor
	servent* pse;
	hostent* phe; //pointer to a host information

	/* Allocate and connect socket - which type depends upon protocol */
	if (_stricmp(protocol, "tcp") == 0) // Create a TCP socket
	{
		s = socket(AF_INET, SOCK_STREAM, 0);
		if (s < 0 || s == INVALID_SOCKET)
		{
			cout << "Cannot create socket" << endl;
			return INVALID_SOCKET;
		}
		memset(&client, 0, sizeof(client)); // zero the client struct
		client.sin_family = AF_INET; // Set the address family

		//if they send service = "echo"
		//get the number using getservbyname function
		pse = getservbyname(service, protocol);	 //either give us the corresponding port number or it'll be null
		if (pse != NULL) {
			client.sin_port = (u_short)pse->s_port;
		}
		else {
			//an port number <= 0 is invalid
			short port = atoi(service);
			if (port > 0)
				client.sin_port = htons((u_short)atoi(service)); // Set the port number
			else
				cout << "Invalid service request.";
		}

		long ihost = inet_addr(host);
		if (ihost == INADDR_NONE) {
			phe = gethostbyname(host);
			//host is the domain name and now have the IP address
			if (phe != NULL) {
				memmove(&ihost, phe->h_addr, phe->h_length);
			}
			else {
				cout << "Invalid host. " << endl;
				return INVALID_SOCKET;
			}
		}
			
		client.sin_addr.s_addr = ihost; // Set remote IP address
		/* Connect the socket */
		status = connect(s, (LPSOCKADDR)&client, sizeof(SOCKADDR));
		if (status == SOCKET_ERROR)
		{
			cout << "Remote host/service not found - or connection refused" << endl;
			return INVALID_SOCKET;
		}
	}
	else if (_stricmp(protocol, "udp") == 0) // Create a UDP socket
	{
		s = socket(AF_INET, SOCK_DGRAM, 0);
		if (s < 0 || s == INVALID_SOCKET)
		{
			cout << "Cannot create socket" << endl;
			return INVALID_SOCKET;
		}
	}
	else
	{
		cout << "Invalid Protocol" << endl; //I give up! 
		return INVALID_SOCKET;
	}
	return s;
}

//command line arguements
//argc = number of command line arguments (including the actual program)
//argv = holds the arguments(first argument is at argv[0]

	int main(int argc, char* argv[]) {

		char msg[MAXBUF] = "";
		char buf[MAXBUF] = "";
		char host[MAXNAME] = "";
		char service[MAXNAME] = "";
		char protocol[4] = "";
		//length of the message/ how many bytes did you send and/or recieve
		int len;
		SOCKET s;

		//WSAStartup - must always initialize the Windows Sockets Library
		WORD wVersionRequired = 0x0101;
		WSADATA wsaData;
		int err = WSAStartup(wVersionRequired, &wsaData);
		if (err) {
			cout << "Unable to intitialize the Windows Socket Library" << endl;
			return -1;
		}
		
		//socket, connect - done by connectsock
		//need to set the host, service, protocol

		strcpy_s(protocol, "tcp");
		bool oneArg = false, twoArg = false, moreArg = false;
		
		if (argc == 1) {
			//output usage statement
			strcpy_s(host, "10.30.201.110");
			strcpy_s(service, "7");
			
			oneArg = true;
		}
		if (argc == 2) {
			//second arguement will specify the host
			strcpy_s(host, argv[1]);

			twoArg = true;
		}
		else {
			strcpy_s(host, "10.30.201.110");
		}
		if (argc == 3) {
			//third argument will specify the service
			strcpy_s(service, argv[2]);

			moreArg = true;
		}
		else {
			strcpy_s(service, "7");
		}
		s = connectsock(host, service, protocol);


		if (s != INVALID_SOCKET) {
			string User_Input_Message;

			if (oneArg == true) {
				cout << "Usage: myecho server_address " << service << endl;
			}

			else if (twoArg == true || moreArg == true) {

				do {
					//Setting up the user input array so you can send multiple lines
					//of text (one at a time)
					cout << "Message? ";
					getline(cin, User_Input_Message);
					if (User_Input_Message == "Quit" || User_Input_Message == "QUIT" || User_Input_Message == "quit") {
						break;
					}

					char* UserInput = new char[User_Input_Message.length() + 1];;
					strcpy(UserInput, User_Input_Message.c_str());
					strcpy_s(msg, UserInput);

					//number of bytes sent
					len = send(s, msg, strlen(msg), 0);
					cout << "Number of bytes sent: " << len << endl;

					//strcpy_s(msg, "This is a sample message with two lines!\nDoes it work?");
					len = recv(s, buf, MAXBUF, 0);
					cout << endl;
					if (len > 0) {
						cout << "Received message: ";
						for (int i = 0; i < len; i++) {
							cout << buf[i];
						}

					}
					cout << endl;
					cout << "Number of bytes received: " << len << endl;

					cout << endl;

				} while (User_Input_Message != "QUIT" || User_Input_Message != "quit");

			}
			//close
			closesocket(s);

		}
		//WSACleanup - this is going to shut down the windowsSocket library
		WSACleanup();
	}