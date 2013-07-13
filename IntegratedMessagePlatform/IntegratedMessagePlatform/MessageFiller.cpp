#include <afx.h>
#include <fstream>
#include <vector>
#include "MessageFiller.h"
using namespace std;

//#define DEBUG_PRINT

DWORD WINAPI recvthread(LPVOID lpParameter);

#ifdef DEBUG_PRINT
ofstream or_filler("out/Filler_Read.txt");
ofstream ow_filler("out/Filler_Write.txt");
#endif

MessageFiller::MessageFiller(MessageSender* m_sender)
{
	sender = m_sender;
	h_Read = CreateEvent(NULL, FALSE, TRUE, NULL);
	h_Write = CreateEvent(NULL, TRUE, FALSE, NULL);
}

MessageFiller::~MessageFiller()
{
	free(sender);
	sender = NULL;

	CloseHandle(h_Read);
	CloseHandle(h_Write);

	#ifdef DEBUG_PRINT
	or_filler.close();
	ow_filler.close();
	#endif
}
//Call from listening thread and add it to filler queue
void MessageFiller::AddMessage(const SimpleMessage &s_message)
{
	//non-signify write event
	//command read thread to stop after its recent action
	ResetEvent(h_Write);
	//wait events
	DWORD wait_result = WaitForSingleObject(h_Read, INFINITE);
	if(wait_result == WAIT_OBJECT_0) {
		//add to queue
		recver_queue.push(s_message);
		#ifdef DEBUG_PRINT
		ow_filler << "Recv Thread: Write Message = " << s_message.ID << std::endl;
		#endif
		//notify events
		SetEvent(h_Write);
		SetEvent(h_Read);
	}
}

/* judge if a receiver is a team */
BOOL MessageFiller::isTeam(char* receiver){
	vector<string*> vec;
	char buf[60];
	strcpy(buf, "team_name='");
	strcat(buf, receiver);
	strcat(buf, "'");
	::getDataLikeSql("team", "team_name", buf, 1, vec);
	if (vec.empty())
		return FALSE;
	else 
		return TRUE;
}

/* return count of members in a group */
int MessageFiller::getMembersCount(char* receiver){
	vector<string*> vec;
	char buf[60];

	strcpy(buf, "team_name='");
	strcat(buf, receiver);
	strcat(buf, "'");

	getDataLikeSql("team", "team_name", buf, 1, vec);

	return vec.size();
}

int MessageFiller::generateMID(int user_id){
	srand((unsigned)time(NULL));
	return user_id * 1000 + rand() % 1000;
}

//link to database and get all infomation needed
//then add it to sender queue
bool MessageFiller::Fill(const SimpleMessage &s_message)
{
	while(!msc.connect("root","root",dataBase)){}

	Message message(s_message);
	//connect database to fill message

	vector<string*> vec;
	char criteria[160];
	char receiver[160];
	strcpy(receiver, message.get_name().c_str());
	if (isTeam(receiver)){
		int count = getMembersCount(receiver);
		strcpy(criteria, "team_name='");
		strcat(criteria, receiver);
		strcat(criteria, "'");
		getDataLikeSql("team", "user_id", criteria, 1, vec);
		for (int i = 0; i < count; i++){

			vector<string*> vec1;
			char criteria1[160];
			strcpy(criteria1, "user_id=");

			strcat(criteria1, getByColumn(i, 0, vec));
			getDataLikeSql("person", "cellphone, phone, email, type1, type2, type3", criteria1, 6, vec1);

			message.set_cellphone(getByColumn(0, 0, vec1));
			message.set_phone_number(getByColumn(0, 1, vec1));
			message.set_mail_addr(getByColumn(0, 2, vec1));
			type choice[3] = {atoi(getByColumn(0, 3, vec1)), atoi(getByColumn(0, 4, vec1)), atoi(getByColumn(0, 5, vec1))};
			message.set_type_choice(choice);
			message.set_mid(generateMID(atoi(getByColumn(i, 0, vec))));

			//add to sender queue
			sender->AddMessage(message);
		}
	}
	else {
		strcpy(criteria, "name='");
		strcat(criteria, receiver);
		strcat(criteria, "'");
		getDataLikeSql("person", "user_id", criteria, 1, vec);
		//transfer ID int user_id = xxx;

		vector<string*> vec1;
		char criteria1[160];
		strcpy(criteria1, "user_id=");
		strcat(criteria1, getByColumn(0, 0, vec));
		getDataLikeSql("person", "cellphone, phone, email, type1, type2, type3", criteria, 6, vec1);

		message.set_cellphone(getByColumn(0, 0, vec1));
		message.set_phone_number(getByColumn(0, 1, vec1));
		message.set_mail_addr(getByColumn(0, 2, vec1));
		type choice[3] = {atoi(getByColumn(0, 3, vec1)), atoi(getByColumn(0, 4, vec1)), atoi(getByColumn(0, 5, vec1))};
		message.set_type_choice(choice);
		message.set_mid(generateMID(atoi(getByColumn(0, 0, vec))));

		//add to sender queue
		sender->AddMessage(message);
	}

	msc.close();
	return true;
}
//create read thread
HANDLE MessageFiller::Start()
{
	#ifdef DEBUG_PRINT
	printf("Recv Thread Create\n");
	#endif

	HANDLE handle = CreateThread(NULL, 0, recvthread, this, 0, NULL);
	return handle;
}
//read thread
DWORD WINAPI recvthread(LPVOID lpParameter)
{
	MessageFiller* filler = (MessageFiller*)lpParameter;

	HANDLE h_Events[2];
	h_Events[0] = filler->h_Read;
	h_Events[1] = filler->h_Write;
	DWORD wait_result;

	while(true) {
		//wait events
		wait_result = WaitForMultipleObjects(2, h_Events, TRUE, INFINITE);
		if(wait_result == WAIT_OBJECT_0) {
			//check if queue is empty
			while(!filler->recver_queue.empty()) {
				#ifdef DEBUG_PRINT
				or_filler << "Recv Thread: Read Message = " << filler->recver_queue.front().ID << std::endl;;
				#endif
				//fill the message
				filler->Fill(filler->recver_queue.front());
				//remove the message from queue
				filler->recver_queue.pop();
			}
		
			#ifdef DEBUG_PRINT
			printf("Recv Thread ready to sleep\n");
			#endif
			//queue empty then sleep itself if queue is empty
			ResetEvent(h_Events[1]);
			
			//notify write events
			SetEvent(h_Events[0]);
		}
	}
	//free pointer
	if(filler != NULL) {
		free(filler);
		filler = NULL;
	}

	return 0;
}