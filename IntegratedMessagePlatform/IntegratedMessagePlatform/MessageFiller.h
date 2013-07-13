#ifndef _MESSAGE_FILLER_H_
#define _MESSAGE_FILLER_H_

/***********************Class MessageFiller************************
 * Description: The Schedule Module of message
 * Use:			The interface for GSM and Mail module to add
				Link to database and get information ready
				Add to MessageSender queue
 ******************************************************************/

#include "MessageSender.h"
#include "MessageQueue.h"

class MessageFiller
{
	friend DWORD WINAPI recvthread(LPVOID lpParameter);
public:
	MessageFiller(MessageSender* m_sender);
	~MessageFiller();

	void AddMessage(const SimpleMessage& s_message);
	HANDLE Start();
private:
	MessageSender* sender;
	MessageQueue<SimpleMessage> recver_queue;
	
	HANDLE h_Read, h_Write;
	
	BOOL isTeam(char* receiver);
	int getMembersCount(char* receiver);
	int generateMID(int user_id);

	bool Fill(const SimpleMessage& s_message);
};

#endif