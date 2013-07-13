#include "GSMReader.h"

DWORD WINAPI read_message_from_gsm(LPVOID lpParameter);

GSMReader::GSMReader(MessageFiller* filler)
{
	m_filler =  filler;
}

GSMReader::~GSMReader()
{
	if(m_filler != NULL) {
		free(m_filler);
		m_filler = NULL;
	}
}

HANDLE GSMReader::Start()
{
	HANDLE handle = CreateThread(NULL, 0, read_message_from_gsm, this, 0, NULL);
	OpenComm1("COM3", 115200, 8, 4, 1);
	OpenComm2("COM4", 9600, 8, 4, 1);
	gsmInit();
	return handle;
}

void GSMReader::convertToSimpleMessage(SM_PARAM* pMsg, char* receiver){
	SimpleMessage msg;
	srand((unsigned)time(NULL));
	msg.GID = rand() % 1000;
	string recv(receiver);
	msg.name = recv;
	msg.data = pMsg->TP_UD;
	m_filler->AddMessage(msg);

}

void GSMReader::getReceiver(SM_PARAM* pMsg, char *receiver){
	char *ptr = pMsg->TP_UD;
	char *start = pMsg->TP_UD;
	int count = 0;
	char buf[161];
	if (pMsg->TP_DCS == GSM_7BIT){
		while (*ptr != '\0'){
			if (*ptr == (char)0x3a){
				strncpy(receiver, pMsg->TP_UD, count);
				strncpy(pMsg->TP_UD, pMsg->TP_UD+count+1, strlen(pMsg->TP_UD));
				receiver[count] = '\0';
				return;
			}
			ptr+=1;
			count+=1;
		}
	}
	else {
		while (*ptr != '\0'){
			if ((unsigned char)*ptr > 0xa0){
				if (*ptr == (char)0xa3 && *(ptr+1) == (char)0xba){
					//found!
					strncpy(receiver, pMsg->TP_UD, count);
					strncpy(pMsg->TP_UD, pMsg->TP_UD+count+2, strlen(pMsg->TP_UD));
					receiver[count] = '\0';
					return;
				}
				ptr+=2;
				count+=2;
			}
			else {
				if (*ptr == (char)0x3a){
					strncpy(receiver, pMsg->TP_UD, count);
					strncpy(pMsg->TP_UD, pMsg->TP_UD+count+1, strlen(pMsg->TP_UD));
					receiver[count] = '\0';
					return;
				}
				ptr+=1;
				count+=1;
			}
		}
	}
}

void GSMReader::clearBuf(char* buf)
{
	for (int i = 0; i < 1024; i++)
		buf[i] = '\0';
	return;
}

//listening thread
DWORD WINAPI read_message_from_gsm(LPVOID lpParameter)
{
	GSMReader* reader = (GSMReader*)lpParameter;
	char buf[1024];
	int nLength = 0;
	char *ptr;
	int index = 0;
	while(true) {
		Sleep(3000);

		//here listen to gsm
		//convert to simple message
		//add to filler queue
		nLength = ReadComm1(buf, 1024);
		if (nLength != 0 && strncmp(buf, "+CMTI:", 6) != 0){
			ptr = buf;
			SM_PARAM *pMsg = new SM_PARAM();
			// if receive the msg
			ptr += 14;
			sscanf(ptr, "%d", &index);
			pMsg->index = index;
			bool flag = gsmGetMessage(pMsg, index);					//Transfer to pMsg Structure
			if (flag == false) 
				continue;

			char receiver[20];
			printf("-------------------------------------------------\n");
			printf("Receive the short message£¡\n");
			reader->getReceiver(pMsg, receiver);					//Get Receiver
			printf("Receiver:%s\n", receiver);
			reader->convertToSimpleMessage(pMsg, receiver);			//Transfer to SimpleMessage and add to filler queue
			printf("Content:%s\n", pMsg->TP_UD);
			reader->clearBuf(buf);
		}
		else {
			printf("Haven't received new message.\n");
		}
	}

	return 0;	
}
