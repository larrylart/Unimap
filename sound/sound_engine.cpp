
//system headers
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <sys/timeb.h>

// WX includes
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif
#include <wx/regex.h>
//#include <wx/sound.h>

// audiere player
//#include "../lib/audiere/src/audiere.h"

// local headers
#include "sound_player.h"
#include "../worker.h"
#include "../gui/image_notes_dlg.h"

// main header
#include "sound_engine.h"

#pragma comment(lib,"winmm.lib")
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#pragma comment(lib,"audiered.lib")
//#else
//#pragma comment(lib,"audiere.lib")
//#endif

////////////////////////////////////////////////////////////////////
// Method:		RecordCallback
// Purpose:		destroy my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CALLBACK waveInProc( HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 )
{
	WAVEHDR *pHdr=NULL;
	switch(uMsg)
	{
		case WIM_CLOSE:
		{
			break;
		}

		case WIM_DATA:
		{
			CSoundEngine *pInst = (CSoundEngine*) dwInstance;
			pInst->ProcessHeader( (WAVEHDR*) dwParam1 );
		}
		break;

		case WIM_OPEN:
			break;

		default:
			break;
	}
}


////////////////////////////////////////////////////////////////////
// Constructor/Destructor
////////////////////////////////////////////////////////////////////
CSoundEngine::CSoundEngine( const wxString& file, int devid, int formatid ) : CWorker( NULL, NULL )
{
	m_bRun=FALSE;
//	m_hThread=NULL;
	m_hWaveIn=NULL;
	ZeroMemory(&m_stWFEX,sizeof(WAVEFORMATEX));
	ZeroMemory(m_stWHDR,MAX_BUFFERS*sizeof(WAVEHDR));

	m_vectDevices.clear();
	m_nUsedDevice = devid;
	m_vectFormats.clear();
	m_nUsedFormat = formatid;

	m_strSoundFile = file;
	m_pSoundPlayer = NULL;

	// init flags
	m_bIsPlaying = 0;

	InitThread( );

	// get devices .. not needed ?
	GetDevices( );
}

////////////////////////////////////////////////////////////////////
// Method:		Destructor
// Class:		CSoundRecorder
// Purpose:		destroy my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
CSoundEngine::~CSoundEngine()
{	
	// clean stuff
	if( m_pSoundPlayer ) 
	{
		//if( m_pSoundPlayer->isPlaying() ) m_pSoundPlayer->stop();
		m_pSoundPlayer->Delete();
		m_pSoundPlayer = NULL;
	}
	m_vectDevices.clear();
	m_vectFormats.clear();
}

////////////////////////////////////////////////////////////////////
int CSoundEngine::InitThread( )
{	
//	GetDlgItem(ID_REC)->SetWindowText("&Stop");
//	m_hThread=CreateThread(NULL,0,ThFunc,(LPVOID)this,0,NULL);

	// create thread
	if( Create() != wxTHREAD_NO_ERROR )
	{
//		m_pLogger->Add( "ERROR :: worker thread initialization failure\n" );
	} else
	{
//		m_pLogger->Add( "INFO :: thread initialization ok\n" );
	}

	// start the thread
	if( Run() != wxTHREAD_NO_ERROR )
	{
//		m_pLogger->Add( "ERROR :: worker thread running failure\n" );
	} else
	{
//		m_pLogger->Add( "INFO :: worker thread running ok\n" );
	}


	return( 1 );
}

////////////////////////////////////////////////////////////////////
unsigned int CSoundEngine::GetDevices( )
{
//	CComboBox *pBox=(CComboBox*)GetDlgItem(IDC_DEVICES);
	unsigned int nDevices, nC1;
	WAVEINCAPS stWIC={0};
	MMRESULT mRes;

//	pBox->ResetContent();

	nDevices = waveInGetNumDevs();

	for( nC1=0; nC1<nDevices; ++nC1 )
	{
		ZeroMemory(&stWIC,sizeof(WAVEINCAPS));
		mRes=waveInGetDevCaps( nC1, &stWIC, sizeof(WAVEINCAPS) );
		if( mRes==0 )
		{
//			pBox->AddString(stWIC.szPname);
			wxString strDev = stWIC.szPname;
			m_vectDevices.push_back( strDev );

		} else
		{
//			StoreError(mRes,TRUE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
		}
	}

	if( nDevices > 0 ) SetDevice( 0 );

	return( nDevices );
}

////////////////////////////////////////////////////////////////////
int CSoundEngine::OpenRecordingDevice( )
{
	if( m_vectDevices.size() <= 0 ) return( 0 );

	int nState = 0;
	int nT1=0;
	wxString csT1;
	double dT1=0.0;
	MMRESULT mRes=0;
//	CComboBox *pDevices=(CComboBox*)GetDlgItem(IDC_DEVICES);
//	CComboBox *pFormats=(CComboBox*)GetDlgItem(IDC_FORMATS);

	nT1 = m_nUsedFormat;
	csT1 = wxString(m_vectFormats[nT1].name,wxConvUTF8);

// larry: here put my dev from properties
//	pFormats->GetLBText(nT1,csT1);
	sscanf((PCHAR)(LPCTSTR)csT1,"%lf",&dT1);

	dT1=dT1*1000;
	m_stWFEX.nSamplesPerSec=(int)dT1;
	csT1=csT1.Right(csT1.Length()-csT1.Find(',')-1);
	csT1.Trim();
	if(csT1.Find(wxT("mono"))!=-1)
		m_stWFEX.nChannels=1;
	if(csT1.Find(wxT("stereo"))!=-1)
		m_stWFEX.nChannels=2;
	csT1=csT1.Right(csT1.Length()-csT1.Find(',')-1);
	csT1.Trim();
	sscanf( (PCHAR)(LPCTSTR)csT1, "%d", &m_stWFEX.wBitsPerSample );
	m_stWFEX.wFormatTag = WAVE_FORMAT_PCM;
	m_stWFEX.nBlockAlign = m_stWFEX.nChannels*m_stWFEX.wBitsPerSample/8;
	m_stWFEX.nAvgBytesPerSec = m_stWFEX.nSamplesPerSec*m_stWFEX.nBlockAlign;
	m_stWFEX.cbSize = sizeof(WAVEFORMATEX);

// larry: here put the curent selection from devices
	mRes = waveInOpen( &m_hWaveIn, m_nUsedDevice, &m_stWFEX, (DWORD_PTR)waveInProc, (DWORD_PTR)this, CALLBACK_FUNCTION );

	if( mRes != MMSYSERR_NOERROR )
	{
		return( 0 );
//		StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
//		throw m_csErrorText;
	}

// larry :: here insert my file name
//	GetDlgItem(IDC_FILENAME)->GetWindowText(csT1);
	csT1 = m_strSoundFile;

	ZeroMemory(&m_stmmIF,sizeof(MMIOINFO));
	DeleteFile((LPCTSTR)csT1);
	// to check: check here if conversion is ok
	m_hOPFile=mmioOpen((LPWSTR)(LPCTSTR)csT1,&m_stmmIF,MMIO_WRITE | MMIO_CREATE);
	if( m_hOPFile==NULL )
		throw "Can not open file...";

	ZeroMemory(&m_stckOutRIFF,sizeof(MMCKINFO));
	m_stckOutRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E'); 
	mRes=mmioCreateChunk(m_hOPFile, &m_stckOutRIFF, MMIO_CREATERIFF);
	if(mRes!=MMSYSERR_NOERROR)
	{
//		StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
//		throw m_csErrorText;
	}
	ZeroMemory(&m_stckOut,sizeof(MMCKINFO));
	m_stckOut.ckid = mmioFOURCC('f', 'm', 't', ' ');
	m_stckOut.cksize = sizeof(m_stWFEX);
	mRes=mmioCreateChunk(m_hOPFile, &m_stckOut, 0);
	if(mRes!=MMSYSERR_NOERROR)
	{
//		StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
//		throw m_csErrorText;
	}
	nT1=mmioWrite(m_hOPFile, (HPSTR) &m_stWFEX, sizeof(m_stWFEX));
	if(nT1!=sizeof(m_stWFEX))
	{
//		m_csErrorText.Format("Can not write Wave Header..File: %s ,Line Number:%d",__FILE__,__LINE__);
//		throw m_csErrorText;
	}
	mRes=mmioAscend(m_hOPFile, &m_stckOut, 0);
	if(mRes!=MMSYSERR_NOERROR)
	{
//		StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
//		throw m_csErrorText;
	}
	m_stckOut.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mRes=mmioCreateChunk(m_hOPFile, &m_stckOut, 0);
	if(mRes!=MMSYSERR_NOERROR)
	{
//		StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
//		throw m_csErrorText;
	}

	return( nState );
}

////////////////////////////////////////////////////////////////////
VOID CSoundEngine::CloseDevice()
{
	MMRESULT mRes=0;
	
	if(m_hWaveIn)
	{
		UnPrepareBuffers();
		mRes=waveInClose(m_hWaveIn);
	}

	if(m_hOPFile)
	{
		mRes=mmioAscend(m_hOPFile, &m_stckOut, 0);
		if(mRes!=MMSYSERR_NOERROR)
		{
//			StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
		}
		mRes=mmioAscend(m_hOPFile, &m_stckOutRIFF, 0);
		if(mRes!=MMSYSERR_NOERROR)
		{
//			StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
		}
		mmioClose(m_hOPFile,0);
		m_hOPFile=NULL;
	}
	m_hWaveIn=NULL;
}

////////////////////////////////////////////////////////////////////
void CSoundEngine::ProcessHeader( WAVEHDR * pHdr )
{
	MMRESULT mRes=0;

//	TRACE("%d",pHdr->dwUser);
	if(WHDR_DONE==(WHDR_DONE &pHdr->dwFlags))
	{
		mmioWrite(m_hOPFile,pHdr->lpData,pHdr->dwBytesRecorded);
		mRes=waveInAddBuffer(m_hWaveIn,pHdr,sizeof(WAVEHDR));

//		if(mRes!=0) StoreError(mRes,TRUE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
	}
}

////////////////////////////////////////////////////////////////////
VOID CSoundEngine::PrepareBuffers( )
{
	MMRESULT mRes=0;
	int nT1=0;

	for(nT1=0;nT1<MAX_BUFFERS;++nT1)
	{
		m_stWHDR[nT1].lpData=(LPSTR)HeapAlloc(GetProcessHeap(),8,m_stWFEX.nAvgBytesPerSec);
		m_stWHDR[nT1].dwBufferLength=m_stWFEX.nAvgBytesPerSec;
		m_stWHDR[nT1].dwUser=nT1;
		mRes = waveInPrepareHeader( m_hWaveIn, &m_stWHDR[nT1], sizeof(WAVEHDR) );
		if(mRes!=0)
		{
//			StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
//			throw m_csErrorText;
		}
		mRes = waveInAddBuffer( m_hWaveIn, &m_stWHDR[nT1], sizeof(WAVEHDR) );
		if(mRes!=0)
		{
//			StoreError(mRes,FALSE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
//			throw m_csErrorText;
		}
	}
}

////////////////////////////////////////////////////////////////////
VOID CSoundEngine::UnPrepareBuffers( )
{
	MMRESULT mRes=0;
	int nT1=0;

	if( m_hWaveIn )
	{
		mRes = waveInStop( m_hWaveIn );
		for( nT1=0; nT1<3; ++nT1 )
		{
			if(m_stWHDR[nT1].lpData)
			{
				mRes=waveInUnprepareHeader(m_hWaveIn,&m_stWHDR[nT1],sizeof(WAVEHDR));
				HeapFree(GetProcessHeap(),0,m_stWHDR[nT1].lpData);
				ZeroMemory(&m_stWHDR[nT1],sizeof(WAVEHDR));
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
void CSoundEngine::SetDevice( int nId )
{
//	CComboBox *pDevices=(CComboBox*)GetDlgItem(IDC_DEVICES);
//	CComboBox *pFormats=(CComboBox*)GetDlgItem(IDC_FORMATS);
	int nSel=0;
	WAVEINCAPS stWIC={0};
	MMRESULT mRes;

//	SetStatus("Querying device informations...");
//	pFormats->ResetContent();
	nSel = nId; //pDevices->GetCurSel();
	if( nSel >= 0 )
	{
		ZeroMemory( &stWIC, sizeof(WAVEINCAPS) );
		mRes = waveInGetDevCaps( nSel, &stWIC, sizeof(WAVEINCAPS) );
		if( mRes == 0 )
		{
			if(WAVE_FORMAT_1M08==(stWIC.dwFormats&WAVE_FORMAT_1M08))
				AddFormat( "11.025 kHz, mono, 8-bit", WAVE_FORMAT_1M08 );
			if(WAVE_FORMAT_1M16==(stWIC.dwFormats&WAVE_FORMAT_1M16))
				AddFormat( "11.025 kHz, mono, 16-bit",WAVE_FORMAT_1M16 );
			if(WAVE_FORMAT_1S08==(stWIC.dwFormats&WAVE_FORMAT_1S08))
				AddFormat( "11.025 kHz, stereo, 8-bit",WAVE_FORMAT_1S08 );
			if(WAVE_FORMAT_1S16==(stWIC.dwFormats&WAVE_FORMAT_1S16))
				AddFormat( "11.025 kHz, stereo, 16-bit",WAVE_FORMAT_1S16 );
			if(WAVE_FORMAT_2M08==(stWIC.dwFormats&WAVE_FORMAT_2M08))
				AddFormat( "22.05 kHz, mono, 8-bit",WAVE_FORMAT_2M08 );
			if(WAVE_FORMAT_2M16==(stWIC.dwFormats&WAVE_FORMAT_2M16))
				AddFormat( "22.05 kHz, mono, 16-bit",WAVE_FORMAT_2M16 );
			if(WAVE_FORMAT_2S08==(stWIC.dwFormats&WAVE_FORMAT_2S08))
				AddFormat( "22.05 kHz, stereo, 8-bit",WAVE_FORMAT_2S08 );
			if(WAVE_FORMAT_2S16==(stWIC.dwFormats&WAVE_FORMAT_2S16))
				AddFormat( "22.05 kHz, stereo, 16-bit",WAVE_FORMAT_2S16 );
			if(WAVE_FORMAT_4M08==(stWIC.dwFormats&WAVE_FORMAT_4M08))
				AddFormat( "44.1 kHz, mono, 8-bit",WAVE_FORMAT_4M08 );
			if(WAVE_FORMAT_4M16==(stWIC.dwFormats&WAVE_FORMAT_4M16))
				AddFormat( "44.1 kHz, mono, 16-bit",WAVE_FORMAT_4M16 );
			if(WAVE_FORMAT_4S08==(stWIC.dwFormats&WAVE_FORMAT_4S08))
				AddFormat( "44.1 kHz, stereo, 8-bit",WAVE_FORMAT_4S08 );
			if(WAVE_FORMAT_4S16==(stWIC.dwFormats&WAVE_FORMAT_4S16))
				AddFormat( "44.1 kHz, stereo, 16-bit",WAVE_FORMAT_4S16 );
			if(WAVE_FORMAT_96M08==(stWIC.dwFormats&WAVE_FORMAT_96M08))
				AddFormat( "96 kHz, mono, 8-bit",WAVE_FORMAT_96M08 );
			if(WAVE_FORMAT_96S08==(stWIC.dwFormats&WAVE_FORMAT_96S08))
				AddFormat( "96 kHz, stereo, 8-bit",WAVE_FORMAT_96S08 );
			if(WAVE_FORMAT_96M16==(stWIC.dwFormats&WAVE_FORMAT_96M16))
				AddFormat( "96 kHz, mono, 16-bit",WAVE_FORMAT_96M16 );
			if(WAVE_FORMAT_96S16==(stWIC.dwFormats&WAVE_FORMAT_96S16))
				AddFormat( "96 kHz, stereo, 16-bit",WAVE_FORMAT_96S16 );
			//if(pFormats->GetCount())
			//	pFormats->SetCurSel(0);

		} else
		{
//			StoreError(mRes,TRUE,"File: %s ,Line Number:%d",__FILE__,__LINE__);
		}
	}
//	SetStatus("Waiting to start...");

}

////////////////////////////////////////////////////////////////////
// Sound engine commands
////////////////////////////////////////////////////////////////////
void CSoundEngine::StartRecording( )
{
	DefCmdAction cmd;
	cmd.id = THREAD_SOUNDENGINE_ACTION_RECORD_START;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
void CSoundEngine::StopRecording( wxEvtHandler* handler )
{	
	DefCmdAction cmd;
	cmd.id = THREAD_SOUNDENGINE_ACTION_RECORD_STOP;
	cmd.handler = handler;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
void CSoundEngine::StartPlaying( wxEvtHandler* handler )
{
	DefCmdAction cmd;
	cmd.id = THREAD_SOUNDENGINE_ACTION_PLAY_START;
	cmd.handler = handler;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
void CSoundEngine::StopPlaying( wxEvtHandler* handler )
{	
	DefCmdAction cmd;
	cmd.id = THREAD_SOUNDENGINE_ACTION_PLAY_STOP;
	cmd.handler = handler;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
// CHECK FOR COMMANDS
////////////////////////////////////////////////////////////////////
int CSoundEngine::CheckForCommands( )
{
	////////////////////////////////////
	// custom section
	if( m_bIsPlaying )
	{
		// check if my sound file is done playing
		if( !m_pSoundPlayer->isPlaying() )
		{
			if( m_DlgHandler )
				SendEventType( m_DlgHandler, wxID_NOTES_DLG_ON_PLAYING_STOPPED ); 
			m_bIsPlaying = 0;
			// close thread
			m_pSoundPlayer->Stop();
			wxThread::Sleep( 200 );
			m_pSoundPlayer->Delete( );
			m_pSoundPlayer = NULL;
		}
	}

	///////////////
	// NOW GO ON WITH THE QUEUE
	if( m_vectActionQueue.size() == 0 ) return( 0 );
	char strMsg[255];

	m_nActionCmd = m_vectActionQueue.front().id;

	////////////////////////
	// check actions
	// :: START RECORDING
	if( m_nActionCmd == THREAD_SOUNDENGINE_ACTION_RECORD_START )
	{
		MMRESULT mRes;

		OpenRecordingDevice();
		PrepareBuffers();
		mRes = waveInStart( m_hWaveIn );
		// check error
		if( mRes != 0 )
		{ 
			// log error
		}

	// :: STOP RECORDING
	} else if( m_nActionCmd == THREAD_SOUNDENGINE_ACTION_RECORD_STOP )
	{
		CloseDevice();

		if( m_vectActionQueue[0].handler )
			SendEventType( m_vectActionQueue[0].handler, wxID_NOTES_DLG_ON_RECORDING_STOPPED ); 

	// :: START PLAYING
	} else if( m_nActionCmd == THREAD_SOUNDENGINE_ACTION_PLAY_START )
	{
		if( !m_pSoundPlayer ) m_pSoundPlayer = new CSoundPlayer( m_strSoundFile );

		m_pSoundPlayer->Play();
		m_bIsPlaying = 1;
		m_DlgHandler = m_vectActionQueue[0].handler;

	// :: STOP PLAYING
	} else if( m_nActionCmd == THREAD_SOUNDENGINE_ACTION_PLAY_STOP )
	{
		if( m_pSoundPlayer ) 
		{
			m_pSoundPlayer->Stop();
			wxThread::Sleep( 200 );
			m_pSoundPlayer->Delete( );
			m_pSoundPlayer = NULL;
		}
		m_bIsPlaying = 0;
		m_DlgHandler = NULL;

		if( m_vectActionQueue[0].handler )
			SendEventType( m_vectActionQueue[0].handler, wxID_NOTES_DLG_ON_PLAYING_STOPPED ); 
	}

	// reset flag
	m_bCurrentTaskSetToBreak = 0;
	// set detect action flag to none
	m_nActionCmd = THREAD_ACTION_NONE;
	m_vectActionQueue.erase( m_vectActionQueue.begin() );

	return( 0 );
}

