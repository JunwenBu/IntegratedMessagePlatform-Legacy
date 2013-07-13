#include "TTS.h"

TTS::TTS(){
	init();
}

TTS::~TTS(){}

HRESULT TTS::init()
{
	CoInitialize(NULL);
	hr = m_cpVoice.CoCreateInstance( CLSID_SpVoice );
	return hr;
}

string TTS::saveToWave(Message *sm){
	CComPtr<ISpStream> cpWavStream;
	CComPtr<ISpStreamFormat> cpOldStream;

	USES_CONVERSION;

	WCHAR szWTextString[1024] ;
	wcscpy(szWTextString, T2W(sm->get_data().c_str()));

	TCHAR szFileName[256];
	TCHAR tmp[30];
	_tcscpy(szFileName, "tts/");
	_tcscat(szFileName, itoa(sm->get_gid(), tmp, 10));
	_tcscat(szFileName, ".wav");

	WCHAR m_szWFileName[256];
	wcscpy(m_szWFileName, T2W(szFileName));	

	CSpStreamFormat OriginalFmt;
	hr = m_cpVoice->GetOutputStream( &cpOldStream );
	if (hr == S_OK)
	{
		hr = OriginalFmt.AssignFormat(cpOldStream);
	}
	else
	{
		hr = E_FAIL;
	}

	// User SAPI helper function in sphelper.h to create a wav file
	if (SUCCEEDED(hr))
	{
		hr = SPBindToFile(m_szWFileName, SPFM_CREATE_ALWAYS, &cpWavStream, &OriginalFmt.FormatId(), OriginalFmt.WaveFormatExPtr()); 
	}
	if (SUCCEEDED(hr))
	{
		// Set the voice's output to the wav file instead of the speakers
		hr = m_cpVoice->SetOutput(cpWavStream, TRUE);
	}

    if (SUCCEEDED(hr))
    {
	    // Do the Speak
		hr = m_cpVoice->Speak( szWTextString, SPF_ASYNC, 0 );
    }

	// Set output back to original stream
	// Wait until the speak is finished if saving to a wav file so that
	// the smart pointer cpWavStream doesn't get released before its
	// finished writing to the wav.
	m_cpVoice->WaitUntilDone(INFINITE);
	cpWavStream.Release();
	
    // Reset output
	m_cpVoice->SetOutput(cpOldStream, FALSE);

	string out(szFileName);

	return out;
}