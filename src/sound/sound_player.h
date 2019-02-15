////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _SOUND_PLAYER_H
#define _SOUND_PLAYER_H

// system includes
#include <vector>
#include <MMSystem.h>

// WX includes
#include "wx/wxprec.h"
#include "wx/thread.h"
#include <wx/process.h>

// defines
#define SOUNDSAMPLES 1000
// other defines
#define BLOCK_SIZE  8192    
#define BLOCK_COUNT 20    

class CSoundPlayer : public wxThread
{
// methods
public:
	CSoundPlayer( wxString strSoundFile, int iHertz=8000 );
	~CSoundPlayer();

	// thread entry point
	virtual void *Entry();
	virtual void OnExit();
	int InitThread( );

	// method to try to get the wav's file format
	bool SetFormatByFile( wxString file );

	// action methods
	int Play();
	void Stop(){ m_bContinuePlaying = 0; CloseAudio( ); }
	int isPlaying(){ return( m_bIsPlaying ); }

	////////////////////
	// other way of playing
	void CloseAudio( );
	void writeAudio(HWAVEOUT hWaveOut, LPSTR data, int size);
	WAVEHDR* allocateBlocks(int size, int count);
	void freeBlocks(WAVEHDR* blockArray);

// data
public:
	// the saund file to be played
	wxString m_strSoundFile;

	// flags
	bool m_bContinuePlaying;
	bool m_bIsPlaying;

	///////////////
	// other way of playing
	WAVEHDR*	waveBlocks;
	int			waveCurrentBlock;
	// device handle 
    HWAVEOUT hWaveOut;
	// file handle 
    HANDLE   hFile;

// PLAYER DATA
protected:
	WAVEFORMATEX m_WaveFormatEx;

};

#endif

