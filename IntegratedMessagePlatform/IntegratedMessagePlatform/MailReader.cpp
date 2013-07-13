#include "MailReader.h"
DWORD WINAPI read_message_from_mail(LPVOID lpParameter);

MailReader::MailReader(MessageFiller* filler)
{
	m_filler = filler;
}

MailReader::~MailReader()
{
	if(m_filler != NULL) {
		free(m_filler);
		m_filler = NULL;
	}
}

HANDLE MailReader::Start()
{
	HANDLE handle = CreateThread(NULL, 0, read_message_from_mail, this, 0, NULL);
	return handle;
}

void MailReader::initPop3(CPop3 pop,const char* username, const char* userpwd, const char* svraddr,
	int& s ,unsigned short port)
{
	pop.Create(username,userpwd,svraddr,110);
	pop.Connect();
	pop.Login();
	pop.List(s);	
}

void MailReader::getPart(char *whole, char *txt,int i,CPop3 pop)
{
	pop.Fetch(whole,i);
	const char* part=strstr(whole,"Date: ");
	if(part==NULL)
		return;
	int j=0;
	for(j=0;part[j]!='\0';j++)
	{
		txt[j]=part[j];
	}
	txt[j]='\0';
}

/* get information about the sender, the info get has more content than From */
void MailReader::getFrom(char* part,char* from)
{
	const char* fr=strstr(part,"From: ");
	fr+=6;
	if(fr==NULL)
		return;
	int j=0;
	for(j=0;fr[j]!='\0'&&j<strlen(from);j++)
	{
		from[j]=fr[j];
	}
	from[j]='\0';
}

/* to chech whether the sender is a sjtu user. */
void MailReader::checkFrom(char*from,char* twe)
{
	char* mailtype=strstr(from,"@");
	mailtype+=1;
	if(mailtype==NULL)
		return;
	int j=0;
	for(j=0;mailtype[j]!='\0';j++)
	{
		if(mailtype[j]=='.')
		{
			twe[j]='\0';
			break;
		}
		twe[j]=mailtype[j];
	}
}

void MailReader::getSubject(char* part, char* subject)
{
	const char* sub=strstr(part,"Subject: ");
	sub+=9;
	if(sub==NULL)
		return;
	int j=0;
	for(j=0;sub[j]!='\0';j++)
	{
		if(sub[j]=='\r'||sub[j]=='\n')
		{
			subject[j]='\0';
			break;
		}
		if(sub[j]=='<'||sub[j]=='>')
		{
			subject[j]='\0';
		}
		subject[j]=sub[j];
	}
	subject[j]='\0';
}

bool MailReader::checkSubject(char *subject)
{	
	const char* sub=strstr(subject,":");
	if(sub==NULL)
		return false;
	else return true;

}

/* use both getmoreContent and getContent to get the real content of the mail.*/
void MailReader::getmoreContent(char*part,char*cont)
{
	const char* mC=strstr(part,"Content-Transfer-Encoding: ");
	mC+=27;
	if(mC==NULL)
		return;
	int j=0;
	for(j=0;mC[j]!='\0';j++)
	{
		cont[j]=mC[j];
	}
	cont[j]='\0';
}

void MailReader::getContent(char* part, char* content)
{	
	char uA[100];
	const char* userA=strstr(part,": ");
	if(userA==NULL)
		return;
	int i=0;
	for(i=0;userA[i]!='\0';i++)
	{
		if(userA[i]=='\r'||userA[i]=='\n')
		{
			uA[i]='\0';
			break;
		}
		uA[i]=userA[i];
	}
	const char* con=strstr(part, ": ");
	con+=strlen(uA)+4;
	strncpy(content,con,strlen(con)-4);
	content[strlen(con)-4]='\0';
}

void MailReader::getRealSubject(char* subject,char* realSub)
{
	const char* sub=strstr(subject,":");
	sub+=1;
	if(sub==NULL)
		return;
	int j=0;
	for(;sub[j]!='\0';j++)
	{
		if(sub[j]=='\r'||sub[j]=='\n')
		{
			realSub[j]='\0';
			break;
		}
		realSub[j]=sub[j];
	}
	realSub[j]='\0';
}

void MailReader::getReceiver(char* subject, char *receiver)
{
	const char* realsub=strstr(subject,":");
	if(realsub==NULL)
		return;
	int i=strlen(subject)-strlen(realsub);
	strncpy(receiver,subject,i);
	receiver[i]='\0';
}

void MailReader::convertToSimplemessage(char *content,char* receiver)
{	
	SimpleMessage msg;
	// next two lines product a random GID
	srand((unsigned)time(NULL));
	msg.GID = rand() % 1000;
	string recv(receiver);
	string cont(content);
	msg.name = recv;
	msg.data = cont;
	cout<<endl;
	cout<<"in processing ..."<<endl;
	m_filler->AddMessage(msg);
}


//listening thread
DWORD WINAPI read_message_from_mail(LPVOID lpParameter)
{
	MailReader* reader = (MailReader*)lpParameter;
	static int sum;
	static int count;
	CPop3 pop;
	//These two lines to ensure that only new mails will be checked.
	reader->initPop3(pop,"wjbuunen","test","pop3.sjtu.edu.cn",sum,110);
	pop.Quit();

	while(true) 
	{

		//here listen to mail server
		CPop3 pop3;
		//Init CPop3 pop3
		pop3.Create("wjbuunen","test","pop3.sjtu.edu.cn",110);
		pop3.Connect();
		pop3.Login();
		pop3.List(count);
		if ( count == sum )
			printf("No new Mails\n");
		while(sum<count)
		{	
			sum++;
			char buf[2048];
			char txt[1024];//part
			reader->getPart(buf,txt,sum,pop3);
			char from[64];
			/* get sender's address */
			for(int t=0;t<100;t++)from[t]='0';
			from[63]='\0';
			reader->getFrom(buf,from);
			/* if the mail is from a sjtu user */
			char twelve[5];
			for(int t=0;t<5;t++)twelve[t]='0';
			twelve[4]='\0';
			reader->checkFrom(from,twelve);
			if(strcmp(twelve,"sjtu")==0)
			{
				char subject[45];
				reader->getSubject(txt,subject);
				if(reader->checkSubject(subject))
				{			
					cout<<"-------------------------------------------------"<<endl;
					cout<<"Received new Mail£¡"<<endl;
					cout<<endl;
					char realsub[30];
					reader->getRealSubject(subject,realsub);
					char receiver[20];
					reader->getReceiver(subject,receiver);
					//cout<<"subject: "<<subject<<endl;
					cout<<"Receiver:"<<receiver<<endl;
					cout<<endl;
					char morecontent[1024];
					char content[1024];
					reader->getmoreContent(txt,morecontent);
					reader->getContent(morecontent,content);
					cout<<"Content:"<<endl;
					cout<<endl;
					cout<<content<<endl;
					cout<<endl;
					//convert to simplemessage and add to filler queue
					reader->convertToSimplemessage(content,receiver);
				}
			}
			else
			{
				printf("not sjtu mail\n");
			}	
		}
		pop3.Quit();
		Sleep(3000);
	}

	return 0;	
}