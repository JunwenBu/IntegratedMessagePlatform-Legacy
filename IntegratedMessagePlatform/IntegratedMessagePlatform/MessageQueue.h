#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_
/***********************Class MessageQueue*************************
 * Description: A thread-safe queue extends stl list
 *				Use Critical_Section to implement synchroniztion
 ******************************************************************/
#include <list>
#include <afx.h>

using namespace::std;

template<class T>
class MessageQueue : public list<T>
{
public:
	MessageQueue();
	~MessageQueue();
	
	void push(const T& data);
	void pop();
	T front();
	bool empty();
	int size();

private:
	CRITICAL_SECTION cs_queue;
	//The MessageQueue Lock
	//Use the same Critical_Section
	class MQLock
	{
	public:
		//Enter Critical_Section when Constructor
		MQLock(CRITICAL_SECTION &cs_queue);
		//Leave Critical_Section when Destructor
		~MQLock();
	private:
		CRITICAL_SECTION &cs;
	};
};

template <class T>
MessageQueue<T>::MessageQueue()
{
	::InitializeCriticalSection(&cs_queue);
	list<T>::list(100);
}

template <class T>
MessageQueue<T>::~MessageQueue()
{
	::DeleteCriticalSection(&cs_queue);
}

template <class T>
void MessageQueue<T>::push(const T &data)
{
	MQLock lock(cs_queue);
	list<T>::push_back(data);
}

template <class T>
void MessageQueue<T>::pop()
{
	MQLock lock(cs_queue);
	list<T>::pop_front();
}

template <class T>
T MessageQueue<T>::front()

{
	MQLock lock(cs_queue);
	return list<T>::front();
}

template <class T>
bool MessageQueue<T>::empty()
{
	MQLock lock(cs_queue);
	return list<T>::empty();
}

template <class T>
int MessageQueue<T>::size()
{
	MQLock lock(cs_queue);
	return list<T>::size();
}

template <class T>
MessageQueue<T>::MQLock::MQLock(CRITICAL_SECTION &cs_queue) : cs(cs_queue)
{
	::EnterCriticalSection(&cs);
}

template <class T>
MessageQueue<T>::MQLock::~MQLock()
{
	::LeaveCriticalSection(&cs);
}

#endif