#ifndef _MESSAGE_RECVER_H_
#define _MESSAGE_RECVER_H_

#include "Message.h"
#include "MessageQueue.h"

class MessageRecver
{
public:
	MessageRecver();
	~MessageRecver();
	bool AddMessage(const SimpleMessage& s_message);
private:
	MessageQueue<SimpleMessage> recver_queue;
};

#endif
