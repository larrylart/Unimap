////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _IMAGE_NOTES_DLG_H
#define _IMAGE_NOTES_DLG_H

#include "wx/wxprec.h"
#include <wx/listctrl.h>
#include <wx/tglbtn.h>

// gui ids
enum
{
	wxID_IMAGE_NOTE_TYPE		= 11600,
	wxID_IMAGE_NOTE_RECORD,
	wxID_IMAGE_NOTE_PLAY, 
	// notifications
	wxID_NOTES_DLG_ON_RECORDING_STOPPED,
	wxID_NOTES_DLG_ON_PLAYING_STOPPED
};

// external classes
//class CAstroImage;
class CSoundEngine;
class CImageNote;

// class:	CImageNotesDlg
///////////////////////////////////////////////////////
class CImageNotesDlg : public wxDialog
{
// methods
public:
    CImageNotesDlg( wxWindow *parent, const wxString& title, int type=0 );
	~CImageNotesDlg( );

	void SetConfig( CImageNote* pNote );

/////////////
// ::DATA
public:
	int m_nDlgMode;
	// local note object
	CImageNote* m_pNote;

	int m_nNoteId;
	wxString m_strSoundFile;
	wxString m_strSoundPath;

//	CAstroImage*	m_pAstroImage;

	// flags for recording/playing
	int	m_bIsRecording;
	int m_bIsPlaying;

	wxChoice*			m_pNoteType;
	wxToggleButton*		m_pRecord;
	wxToggleButton*		m_pPlay;

	wxTextCtrl*		m_pNoteTitle;
	wxTextCtrl*		m_pNoteBody;

private:
	void OnNoteType( wxCommandEvent& pEvent );
	void SetNoteType( int nType );
	void OnRecord( wxCommandEvent& pEvent );
	void OnPlay( wxCommandEvent& pEvent );
	// sound engine thread notification handlers
	void OnSoundEngineAction( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};


#endif
