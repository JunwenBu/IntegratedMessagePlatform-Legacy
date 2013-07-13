#ifndef _SIMPLE_MESSAGE_H_
#define _SIMPLE_MESSAGE_H_

/***********************Class SimpleMessage*************************
 * Description: The first stage format of message
 *******************************************************************/

#include <string>
using namespace std;

class SimpleMessage
{
public:
	int GID;
	std::string name;
	std::string data;

	SimpleMessage();
	SimpleMessage(const SimpleMessage& message);
	~SimpleMessage();

	virtual int get_gid();
	virtual std::string get_name();
	virtual std::string get_data();
};

#endif
