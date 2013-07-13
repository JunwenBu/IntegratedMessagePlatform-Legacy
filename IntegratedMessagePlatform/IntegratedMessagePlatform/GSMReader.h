#ifndef _GSM_READER_H_
#define _GSM_READER_H_

/***********************Class GSMReader****************************
 * Description: The Interface for GSM Module
 * Use:			Convert GSM message to standard message
				Add to MessageFiller queue
 ******************************************************************/

#include "GSMOperation.h"
#include "MessageFiller.h"

class GSMReader{
	//GSM Module Listening Thread
	friend DWORD WINAPI read_message_from_gsm(LPVOID lpParameter);
public:
	GSMReader(MessageFiller* filler);
	~GSMReader();

	HANDLE Start();

	void convertToSimpleMessage(SM_PARAM* pMsg, char* receiver);
	void getReceiver(SM_PARAM* pMsg, char *receiver);
	BOOL isTeam(char* receiver);
	int getMembersCount(char* receiver);
	void clearBuf(char* buf);
	
private:
	MessageFiller* m_filler;
};

#endif