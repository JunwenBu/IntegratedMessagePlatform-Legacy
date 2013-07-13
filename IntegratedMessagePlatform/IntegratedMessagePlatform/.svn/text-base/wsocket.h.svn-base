/* Definitions for socket class.*/ 

#ifndef _HEGANG_WSOCKET_H_
#define _HEGANG_WSOCKET_H_

#include <winsock.h>
#include "StdAfx.h"
#include <stdio.h>
#include <iostream>
using namespace std;

typedef int				socklen_t;


class WSocket {

public:
	WSocket(SOCKET sock = INVALID_SOCKET);
	~WSocket();

	// Create socket
	bool Create(int af, int type, int protocol = 0);

	// Connect socket
	bool Connect(const char* ip, unsigned short port);

	// Bind socket
	bool Bind(unsigned short port);

	// Listen socket
	bool Listen(int backlog = 5); 

	// Accept socket
	bool Accept(WSocket& s, char* fromip = NULL);

	// Send socket
	int Send(const char* buf, int len, int flags = 0);

	// Recv socket
	int Recv(char* buf, int len, int flags = 0);
	
	// Close socket
	int Close();

	// Get errno
	int GetError();

	// Init winsock DLL 
	static int Init();	
	// Clean winsock DLL
	static int Clean();

	// Domain parse
	static bool DnsParse(const char* domain, char* ip);

	WSocket& operator = (SOCKET s);

	operator SOCKET ();

protected:
	SOCKET m_sock;

};

#endif

