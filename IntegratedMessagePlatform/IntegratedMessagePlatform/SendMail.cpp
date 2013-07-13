#include "SendMail.h"

void sendMail(Message msg)
{
	Smtp mail;

	if(mail.GetLastError() != CSMTP_NO_ERROR)
	{
	printf("Unable to initialise winsock2.\n");
		return;
	}
	mail.SetSMTPServer("smtp.sjtu.edu.cn",25);
	mail.SetLogin("junwenbu");
	mail.SetPassword("test");
	mail.SetSenderMail("junwenbu@sjtu.edu.cn");
	mail.SetSubject("Subject");
	mail.AddRecipient(msg.get_mail_addr().c_str());
	mail.SetMessageBody(msg.data.c_str());
	if( mail.Send() )
	{
		printf("Mail has been sent out.\n");
		cout<<"-------------------------------------------------"<<endl;
	}
	else
	{
		printf("%s\n",GetErrorText(mail.GetLastError()));
		printf("Mail sending fail!\n");
		cout<<"-------------------------------------------------"<<endl;
	}
}