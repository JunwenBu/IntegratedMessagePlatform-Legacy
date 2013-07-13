#ifndef _SEND_MAIL_H_
#define _SEND_MAIL_H_

#include "Smtp.h"
#include <cstring>
#include <string>
#include <vector>
#include <conio.h>
#include <iostream>
#include "Message.h"
#include "DBInterface.h"

using namespace std;

void sendMail(Message msg);
#endif