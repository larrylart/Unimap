////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _SOUND_ENGINE_H
#define _SOUND_ENGINE_H

// system includes
#include <vector>
#include <MMSystem.h>

// WX includes
#include "wx/wxprec.h"
#include "wx/thread.h"
#include <wx/process.h>
//#include <wx/sound.h>

// audiere player
//#include "../lib/audiere/src/audiere.h"

// defines
#define MAX_BUFFERS	3

// sound format struct
typedef struct
{
	char name[255];
	long id;

} DefSoundFormat;

//////////////
// define actions - start with 1 0 = none already defined
#define THREAD_SOUNDENGINE_ACTION_RECORD_START		1
#define THREAD_SOUNDENGINE_ACTION_RECORD_STOP		2
#define THREAD_SOUNDENGINE_ACTION_PLAY_START		3
#define THREAD_SOUNDENGINE_ACTION_PLAY_STOP			4

// local headers
#include "../worker.h"

// external classes
class CSoundPlayer;

// used namespaces
using namespace std;
//using namespace audiere;

// class : CSoundEngine
class CSoundEngine : public CWorker
{
// methods
public:
	CSoundEngine( const wxString& file, int devid=0, int formatid=0 );
	~CSoundEngine();

	// thread functions
	int InitThread( );
	int CheckForCommands( );

	// actions
	void StartRecording( );
	void StopRecording( wxEvtHandler* handler );
	void StartPlaying( wxEvtHandler* handler );
	void StopPlaying( wxEvtHandler* handler );

public:
	// file to output the sound
	wxString m_strSoundFile;

	// devices
	vector<wxString> m_vectDevices;
	int m_nUsedDevice;
	// formats
	vector<DefSoundFormat> m_vectFormats;
	int m_nUsedFormat;

	// flags
	int m_bIsPlaying;
	wxEvtHandler* m_DlgHandler;

	// debug
//	wxString m_csErrorText;

	//////////////////////////////
	// sound player data
// wx sound player
//	wxSound*	m_pSoundPlayer;
// audiere library
//	OutputStreamPtr	m_pSoundPlayer;
	CSoundPlayer* m_pSoundPlayer;

	//////////////////////////////
	// sound recordind data
	BOOL m_bRun;
	HWAVEIN m_hWaveIn;
	WAVEFORMATEX m_stWFEX;
	WAVEHDR m_stWHDR[MAX_BUFFERS];

	HMMIO m_hOPFile;
	MMIOINFO m_stmmIF;
	MMCKINFO m_stckOut,m_stckOutRIFF; 

public:
	// todo: have this how ??? - not used here ?
	unsigned int GetDevices( );

private:

	int OpenRecordingDevice( );
	void CloseDevice( );
	void SetDevice( int nId );

public:
	void ProcessHeader( WAVEHDR * pHdr );

private:
	void PrepareBuffers();
	void UnPrepareBuffers();

	void AddFormat( const char* name, long id ){ DefSoundFormat tt; strcpy( tt.name, name ); tt.id = id; m_vectFormats.push_back( tt ); }

};

#endif
