#include <afx.h>
#include <iostream>
#include <fstream>
#include <shpa3api.h>
#include "MessageQueue.h"
#include "PhoneDial.h"
#include "TTS.h"

std::string TTS_Name[CH_NUM];
void WriteCTIErrorLog();

int CALLBACK DialCallBack(WORD wEvent, int nReference, DWORD dwParam, DWORD dwUser);
DWORD WINAPI dial(LPVOID lpParameter);

PhoneDial::PhoneDial()
{
	if(InitBoard()) {
		h_Message = CreateEvent(NULL, FALSE, FALSE, NULL);
		h_Dial = CreateThread(NULL, 0, dial, this, 0, NULL);
	}
}

PhoneDial::~PhoneDial()
{
	CloseHandle(h_Dial);
	CloseHandle(h_Message);
	m_list.clear();
	CloseBoard();
}

BOOL PhoneDial::InitBoard()
{
	if(SsmStartCti("../config/ShConfig.ini", "../config/ShIndex.ini") != 0) {
		WriteCTIErrorLog();
		return FALSE;
	}

	EVENT_SET_INFO EventMode;
	EventMode.dwWorkMode = EVENT_CALLBACK;
	EventMode.lpHandlerParam = DialCallBack;
	SsmSetEvent(0xFFFF, -1, TRUE, &EventMode);

	return TRUE;
}

BOOL PhoneDial::CloseBoard()
{
	if(SsmCloseCti() != 0) return FALSE;
	return TRUE;
}

BOOL PhoneDial::PickUp(int ch)
{
	if(SsmPickup(ch) != 0) {
		WriteCTIErrorLog();
		return FALSE;
	}
	return TRUE;
}

BOOL PhoneDial::Dial(int ch, const char* buf)
{
	int dResult = SsmAutoDial(ch, buf);
	if(dResult != 0) {
		WriteCTIErrorLog();
		return FALSE;
	}
	return TRUE;
}

BOOL PhoneDial::ChAllFree()
{
	for(int i = 0; i < CH_NUM; ++i) {
		if(SsmGetChType(i) != 0) return FALSE;
	}
	return TRUE;
}

int PhoneDial::SearchFreeCh()
{
	return SsmSearchIdleCallOutCh(1, 0);
}

void PhoneDial::AddPhoneMessage(const Message &message)
{
	m_list.push(message);
	SetEvent(h_Message);
}

DWORD WINAPI dial(LPVOID lpParameter)
{
	PhoneDial* dialler = (PhoneDial*)lpParameter;
	TTS tts;
	DWORD wait_result;

	while(true) {
		wait_result = WaitForSingleObject(dialler->h_Message, INFINITE);
		if(wait_result == WAIT_OBJECT_0) {
			while(!dialler->m_list.empty()) {
				//search free channel
				int ch = dialler->SearchFreeCh();
				std::cout << "Get Channel " << ch << std::endl;
				//no free channel
				if(ch < 0) continue;
				//get message
				Message message = dialler->m_list.front();
				//tts
				TTS_Name[ch] = tts.saveToWave(&message).c_str();
				//pick up
				if(!dialler->PickUp(ch))
					continue;
				//get phone number
				char TxDtmfBuf[10];
				strcpy(TxDtmfBuf, message.get_phone_number().c_str());
				if(dialler->Dial(ch, TxDtmfBuf)) {
					dialler->m_list.pop();
					cout << "dial" << endl;
				}
			}
			if(dialler->ChAllFree())
				ResetEvent(dialler->h_Message);
			
		}
	}
}

int CALLBACK DialCallBack(WORD wEvent, int nReference, DWORD dwParam, DWORD dwUser)
{
	switch(wEvent) {
		case E_CHG_ChState:
			for(int i = 0; i < CH_NUM; ++i) {
				switch(SsmGetChState(i)) {
				case 3: {
					if(SsmPlayFile(i, TTS_Name[i].c_str(), 1, 0, 100000000) != 0) {
						WriteCTIErrorLog();
					}
					break;
				}
				case 7:
					cout << "Put DOWN" << endl;
					SsmHangup(i);
					break;
				}
			}
			break;
		default:
			break;
	}
	return 1;
}

void WriteCTIErrorLog()
{
	char* log = new char[256];
	SsmGetLastErrMsg(log);
	ofstream in("out/cti_log.txt", ios::app);
	in << log << endl;
	in.flush();
	in.close();
	delete log;
	log = NULL;
}