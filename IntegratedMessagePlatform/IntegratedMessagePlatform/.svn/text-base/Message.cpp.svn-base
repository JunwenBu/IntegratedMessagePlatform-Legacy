#include "Message.h"

using namespace std;

Message::Message()
{
	SimpleMessage::SimpleMessage();
	MID = rand() % 100;
	user_type_pos = 0;
}

Message::Message(const Message& message)
{
	SimpleMessage::GID = message.GID;
	SimpleMessage::name = message.name;
	SimpleMessage::data = message.data;
	cellphone = message.cellphone;
	phone_number = message.phone_number;
	mail_addr = message.mail_addr;
	user_type_pos = message.user_type_pos;
	times = message.times;
	MID = message.MID;
	
	int count = sizeof(user_choice) / sizeof(type);
	for(int i = 0; i < count; ++i)
		user_choice[i] = message.user_choice[i];
}

Message::Message(const SimpleMessage& s_message)
{
	SimpleMessage::GID = s_message.GID;
	SimpleMessage::name = s_message.name;
	SimpleMessage::data = s_message.data;
	user_type_pos = 0;
	times = 0;
}

Message::~Message()
{

}

string Message::get_cellphone() const
{
//	std::string tmp(cellphone);
	return cellphone;
}

string Message::get_phone_number() const
{
	//std::string tmp(phone_number);
	return phone_number;
}

string Message::get_mail_addr() const
{
	//std::string tmp(mail_addr);
	return mail_addr;
}

type Message::get_type() const
{
	return user_choice[user_type_pos];
}

void Message::next_choice()
{
	++user_type_pos;
}

int Message::get_times() const
{
	return times;
}

int Message::get_mid() const
{
	return MID;
}

void Message::set_cellphone(const string m_cellphone)
{
	cellphone = m_cellphone;;
}

void Message::set_phone_number(const string m_phone_number)
{
	phone_number = m_phone_number;
}

void Message::set_mail_addr(const string m_mail_addr)
{
	mail_addr = m_mail_addr;
}

void Message::set_type_choice(const type choice[CHOICE_NUM])
{
	for(int i = 0; i < CHOICE_NUM; ++i)
		user_choice[i] = choice[i];
}

void Message::set_mid(const int mid)
{
	MID = mid;
}

void Message::inc_times()
{
	++times;
}