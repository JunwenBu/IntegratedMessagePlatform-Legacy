#ifndef _TTS_H_
#define _TTS_H_

#include "Message.h"
#include <sphelper.h>

class TTS{

public:
	TTS();
	~TTS();
	HRESULT init();
	string saveToWave(Message *sm);

private:
	CComPtr<ISpVoice> m_cpVoice;
	HRESULT hr;
};

#endif