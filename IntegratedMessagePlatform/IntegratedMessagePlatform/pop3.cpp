/* Source file about POP3 interfaces.*/ 
#include "StdAfx.h"
#include "pop3.h"

CPop3::CPop3()
{
	WSocket::Init();
}

CPop3::~CPop3()
{
	WSocket::Clean();
}

int CPop3::Pop3Recv(char* buf, int len, int flags)
{
	int rs;
	int offset = 0;

	do 
	{
		if ( offset > len - 2 )
			return offset;

		rs = m_sock.Recv(buf + offset, len - offset, flags);
		if ( rs < 0  ) /* error occur */
			return -1;

		offset += rs;
		buf[offset] = '\0';
	} while ( strstr(buf, "\r\n.\r\n") == (char*)NULL );
	
	return offset;
}

bool CPop3::Create(const char* username, const char* userpwd, const char* svraddr, unsigned short port)
{
	strcpy(m_username, username);
	strcpy(m_userpwd, userpwd);
	strcpy(m_svraddr, svraddr);
	m_port = port;
	return true;
}

bool CPop3::Connect()
{
	// Create sock
	m_sock.Create(AF_INET, SOCK_STREAM, 0);

	// Parse domain
	char ipaddr[16];
	if ( WSocket::DnsParse(m_svraddr, ipaddr) != true )
		return false;

	// Connect
	if ( m_sock.Connect(ipaddr, m_port) != true )
		return false;

	// Recv POP3 server welcome message
	char buf[128];
	int rs = m_sock.Recv(buf, sizeof(buf), 0);
	if ( rs <= 0 || strncmp(buf, "+OK", 3) != 0 ) {
		return false;
	}

#ifdef _DEBUG
	buf[rs] = '\0';
	//printf("Recv POP3 Resp: %s", buf);
#endif

	return true;
}

bool CPop3::Login()
{
	/* Send USER command */
	char sendbuf[640];
	char recvbuf[6400];

	sprintf(sendbuf, "USER %s\r\n", m_username);
	m_sock.Send(sendbuf, strlen(sendbuf), 0);
	int rs = m_sock.Recv(recvbuf, sizeof(recvbuf), 0);
	if ( rs <= 0 || strncmp(recvbuf, "+OK", 3) != 0 ) {
		return false;
	}
	
#ifdef _DEBUG
	recvbuf[rs] = '\0';
	//printf("Recv USER Resp: %s", recvbuf);
#endif	
	
	/* Send PASS command */
	sprintf(sendbuf, "PASS %s\r\n", m_userpwd);
	m_sock.Send(sendbuf, strlen(sendbuf), 0);
	rs = m_sock.Recv(recvbuf, sizeof(recvbuf), 0);
	if ( rs <= 0 || strncmp(recvbuf, "+OK", 3) != 0 ) {
		return false;
	}
	
#ifdef _DEBUG
	recvbuf[rs] = '\0';
	//printf("Recv PASS Resp: %s", recvbuf);
#endif		

	return true;
}

bool CPop3::List(int& sum)
{
	/* Send LIST command */
	char sendbuf[128];
	char recvbuf[25600];

	sprintf(sendbuf, "LIST \r\n");
	m_sock.Send(sendbuf, strlen(sendbuf), 0);
	int rs = Pop3Recv(recvbuf, sizeof(recvbuf), 0);
	if ( rs <= 0 || strncmp(recvbuf, "+OK", 3) != 0 ) {
		return false;
	}
	recvbuf[rs] = '\0';

#ifdef _DEBUG
	//printf("Recv LIST Resp: %s", recvbuf);
#endif

	sum = GetMailSum(recvbuf);
	
	return true;	
}

bool CPop3::Fetch(char* buf, int num)
{
	char sendbuf[128];
	char recvbuf[10240];

	/* Send RETR command */
	sprintf(sendbuf, "RETR %d\r\n", num);
	m_sock.Send(sendbuf, strlen(sendbuf), 0);
	int rs = Pop3Recv(recvbuf, sizeof(recvbuf), 0);
	if ( rs <= 0) {
		return false;
	}
	recvbuf[rs] = '\0';

#ifdef _DEBUG
	//printf("Recv RETR Resp: %s", recvbuf);
#endif
	
	/* Save mail to buffer */
	char* p = strstr(recvbuf, "\r\n");
	memcpy(buf, p + 2, strlen(p + 2) - 3);
	buf[strlen(p + 2) -3] = '\0';

	return true;
}

bool CPop3::FetchEx(int num)
{
	int rs;
	FILE* fp;
	int flag = 0;
	unsigned int len;
	char filename[32];
	char sendbuf[128];
	char recvbuf[10240];

	/* Send RETR command */
	sprintf(sendbuf, "RETR %d\r\n", num);
	m_sock.Send(sendbuf, strlen(sendbuf), 0);
	
	do {
		rs = Pop3Recv(recvbuf, sizeof(recvbuf), 0);
		if ( rs < 0 ) {
			return false;
		}
	
		recvbuf[rs] = '\0';

		// Get mail subject
		if ( flag == 0 ) {
			GetSubject(filename, recvbuf);
			strcat(filename, ".eml");
			flag = 1;
			if ( (fp = fopen(filename, "wb")) == NULL )
					return false;
		}

#ifdef _DEBUG
		//printf("Recv RETR Resp: %s", recvbuf);
#endif

		len = strlen(recvbuf);
		if ( fwrite(recvbuf, 1, len, fp) != len ) {
			fclose(fp);
			return false;
		}
		fflush(fp);

	} while ( strstr(recvbuf, "\r\n.\r\n") == (char*)NULL );

	fclose(fp);
	return true;
}

bool CPop3::Quit()
{
	char sendbuf[128];
	char recvbuf[6400];

	/* Send QUIT command */
	sprintf(sendbuf, "QUIT\r\n");
	m_sock.Send(sendbuf, strlen(sendbuf), 0);
	int rs = m_sock.Recv(recvbuf, sizeof(recvbuf), 0);
	if ( rs <= 0 || strncmp(recvbuf, "+OK", 3) != 0 ) {
		return false;
	}
	
#ifdef _DEBUG
	recvbuf[rs] = '\0';
	//printf("Recv QUIT Resp: %s", recvbuf);
#endif	

	m_sock.Close();
	return true;
}

bool CPop3::GetSubject(char* subject, const char* buf)
{
	const char* p = strstr(buf, "Subject: ");
	if ( p == NULL )
		return false;

	p = p + 9;
	for (int i = 0; i < 32; i++) {

		if ( p[i] == '\r' || p[i] == '\n' ) {
			subject[i] = '\0';
			break;
		}
		subject[i] = p[i];
	}

	return true;
}

bool CPop3::WriteFile(const char* filename, const char* buffer, unsigned int len)
{
	FILE* fp;
		
	if ( (fp = fopen(filename, "wb")) == NULL ) 
		return false;
	
	if ( fwrite(buffer, len, 1, fp) != 1 ) {
		fclose(fp);
		return false;
	}
	
	fclose(fp);
	return true;
}

int CPop3::GetMailSum(const char* buf)
{
	int sum = 0;
	const char* p = strstr(buf, "\r\n");
	if ( p == NULL )
		return sum;

	p = strstr(p + 2, "\r\n");
	if ( p == NULL )
		return sum;
	
	while ( (p = strstr(p + 2, "\r\n")) != NULL ) {
		sum++;
	}

	return sum;
}