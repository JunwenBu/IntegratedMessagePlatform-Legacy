#include <afx.h>

#include <iostream>
#include <fstream>
#include "MessageFiller.h"
#include "MessageSender.h"
#include "GSMReader.h"
#include "MailReader.h"
#include "DBInterface.h"
using namespace std;

//#define DEBUG_PRINT
#define HANDLE_NUM 4

DWORD WINAPI read_message_from_gsm(LPVOID lpParameter);
DWORD WINAPI read_message_from_mail(LPVOID lpParameter);
DWORD WINAPI resend(LPVOID lpParameter);

#ifdef DEBUG_PRINT
DWORD WINAPI Send_write(LPVOID lpParameter);
ofstream o_test("out/input.txt");
#endif

int main()
{

	HANDLE* p_Handles = new HANDLE[HANDLE_NUM];

	MessageSender* sender = new MessageSender();
	p_Handles[0] = sender->Send();
	MessageFiller* filler = new MessageFiller(sender);
	p_Handles[1] = filler->Start();

#ifdef DEBUG_PRINT
	for(int i = 0; i < 100; ++i) {
		CreateThread(NULL, 0, Send_write, filler, 0, NULL);
	}
#endif

	MailReader* mail_reader = new MailReader(filler);
	GSMReader* gsm_reader = new GSMReader(filler);
	
	p_Handles[2] = mail_reader->Start();
	p_Handles[3] = gsm_reader->Start();
	WaitForMultipleObjects(HANDLE_NUM, p_Handles, TRUE, INFINITE);
	for(int i = 0; i < HANDLE_NUM; ++i)
		CloseHandle(p_Handles[i]);
}

#ifdef DEBUG_PRINT
DWORD WINAPI Send_write(LPVOID lpParameter)
{
	MessageFiller* filler = (MessageFiller*)lpParameter;
	while(true) {
		SimpleMessage message;
		filler->AddMessage(message);
		o_test << message.ID << std::endl;
		Sleep(rand()%100);
	}
}
#endif