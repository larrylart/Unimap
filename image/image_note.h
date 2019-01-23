
#ifndef _IMAGE_NOTE_H
#define _IMAGE_NOTE_H

// wx 
#include "wx/wxprec.h"
#include <wx/datetime.h>

// external classes
class CSoundEngine;
class CAstroImage;

class CImageNote
{
// methods
public:
	CImageNote( CAstroImage* pAstroImage );
	~CImageNote();
	
	// some handlers
	void CopyFrom( CImageNote& note );

	// audio-notes handlers 
	int RecordAudioNote_Start();
	int RecordAudioNote_Stop( wxEvtHandler* handler );
	int PlayAudioNote_Start( wxEvtHandler* handler );
	int PlayAudioNote_Stop( wxEvtHandler* handler );

// data
public:
	// external objects
	CAstroImage*	m_pAstroImage;
	CSoundEngine*	m_pSoundEngine;

	// note id (generated from main astro aimeg list)
	int m_nId;
	// type 0=text, 1=audio, 2=both
	int m_nType;
	// date/time
	wxDateTime m_rDateTime;
	// title
	wxString m_strTitle;
	// message body
	wxString m_strMessage;
	// sound file
	wxString m_strSoundFile;
	// also keep the sound path
	wxString m_strSoundPath;
	// position
	double x;
	double y;
};

#endif
