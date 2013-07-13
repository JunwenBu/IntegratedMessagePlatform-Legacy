/* Definitions for POP3 interfaces.*/
#ifndef _HEGANG_POP3_H_
#define _HEGANG_POP3_H_
#include "wsocket.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "Message.h"

class CPop3 {

public:
	CPop3();
	~CPop3();

	bool Create(const char* username, const char* userpwd, const char* svraddr, 
		unsigned short port = 110);// Init POP3 attributes 
	bool Connect();// Connect POP3 server
	bool Login();// Login server
	bool List(int& sum);// List mail
	bool Fetch(char* buf, int num = 1);// Fetch mail
	bool FetchEx(int num = 1);
	bool Quit();// Quit
	// Get subject 
	bool GetSubject(char* subject, const char* buf);
	// Write file
	static bool WriteFile(const char* filename, const char* buffer, unsigned int len);

protected:
	int GetMailSum(const char* buf);

	WSocket m_sock;
	char m_username[32];
	char m_userpwd[32];
	char m_svraddr[32];
	unsigned short m_port;

private:
	int Pop3Recv(char* buf, int len, int flags = 0);

};

#endif

