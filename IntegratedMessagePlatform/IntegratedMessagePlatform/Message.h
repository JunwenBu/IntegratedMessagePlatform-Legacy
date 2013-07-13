#ifndef _MESSAGE_H_
#define _MESSAGE_H_

/***********************Class Message*******************************
 * Description: The standart message format extends SimpleMessage
 *******************************************************************/

#include "SimpleMessage.h"

#define CHOICE_NUM 3
#define MESSAGE_CHOICE 1
#define PHONE_CHOICE 2
#define MAIL_CHOICE 3

typedef int type;

class Message : public SimpleMessage
{
private:
	string cellphone;
	string phone_number;
	string mail_addr;
	type user_choice[CHOICE_NUM];
	int MID;
	int user_type_pos;
	int times;
public:
	Message();
	Message(const Message& message);
	Message(const SimpleMessage& s_message);
	~Message();

	virtual string get_cellphone() const;
	virtual string get_phone_number() const;
	virtual string get_mail_addr() const;
	virtual type get_type() const;
	virtual void next_choice();
	virtual int get_times() const;
	virtual int get_mid() const;

	virtual void set_cellphone(const string m_cellphone);
	virtual void set_phone_number(const string m_phone_number);
	virtual void set_mail_addr(const string m_mail_addr);
	virtual void set_type_choice(const type choice[CHOICE_NUM]);
	virtual void set_mid(const int mid);

	void inc_times();
};

#endif