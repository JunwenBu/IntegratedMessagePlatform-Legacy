#include "SimpleMessage.h"

//#define DEBUG_RAND

#ifdef DEBUG_RAND
static int count = 0;
#endif

SimpleMessage::SimpleMessage()
{
	#ifdef DEBUG_RAND
	ID = ++count;
	#else
	GID = rand()%100;
	#endif
}

SimpleMessage::SimpleMessage(const SimpleMessage &message)
{
	GID = message.GID;
	name = message.name;
	data = message.data;
}

SimpleMessage::~SimpleMessage()
{

}

int SimpleMessage::get_gid()
{
	return GID;
}

std::string SimpleMessage::get_name()
{
	return name;
}

std::string SimpleMessage::get_data()
{
	return data;
}