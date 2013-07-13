#ifndef _MESSAGE_SENDER_H_
#define _MESSAGE_SENDER_H_

/***********************Class MessageSender************************
 * Description: The Module to send message to hardware
 * Use:			Get message from the queue then send
				it to respective hardware
 ******************************************************************/


#include "Message.h"
#include "MessageQueue.h"
#include "GSMOperation.h"

DWORD WINAPI sendthread(LPVOID lpParameter);
DWORD WINAPI resendthread(LPVOID lpParameter);

class MessageSender
{
	friend DWORD WINAPI sendthread(LPVOID lpParameter);
	friend DWORD WINAPI resendthread(LPVOID lpParameter);
public:
	MessageSender();
	~MessageSender();

	HANDLE Send();
	void AddMessage(const Message& message);
private:
	MessageQueue<Message> sender_queue;
	HANDLE h_Read, h_Write;
	HANDLE h_Resend, h_ResendThread;
};

#endif