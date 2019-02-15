////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/app.h>

// local headers
#include "../unimap.h"
#include "../worker.h"
//#include "../image/astroimage.h"
#include "../image/image_note.h"

// main header
#include "image_notes_dlg.h"

//extern CUnimap* g_pUnimap;
DECLARE_APP(CUnimap)

//		***** CLASS CImageNotesDlg *****
////////////////////////////////////////////////////////////////////
// CImageNotesDlg EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CImageNotesDlg, wxDialog)
	EVT_CHOICE( wxID_IMAGE_NOTE_TYPE, CImageNotesDlg::OnNoteType )
//	EVT_TEXT_ENTER( wxID_VIEW_FIND_OBJECT_ENTRY, CImageNotesDlg::OnDoFind )
	EVT_TOGGLEBUTTON( wxID_IMAGE_NOTE_RECORD, CImageNotesDlg::OnRecord )
	EVT_TOGGLEBUTTON( wxID_IMAGE_NOTE_PLAY, CImageNotesDlg::OnPlay )
	// sound engine thread notifications
	EVT_COMMAND( wxID_NOTES_DLG_ON_RECORDING_STOPPED, wxEVT_CUSTOM_MESSAGE_EVENT, CImageNotesDlg::OnSoundEngineAction )
	EVT_COMMAND( wxID_NOTES_DLG_ON_PLAYING_STOPPED, wxEVT_CUSTOM_MESSAGE_EVENT, CImageNotesDlg::OnSoundEngineAction )
END_EVENT_TABLE()

// Method:	Constructor/Destructor
////////////////////////////////////////////////////////////////////
CImageNotesDlg::CImageNotesDlg( wxWindow *parent, const wxString& title, int type ) : wxDialog(parent, -1,
                          title,
                          wxDefaultPosition,
                          wxSize(300,200),
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL|wxRESIZE_BORDER )
{
	m_nDlgMode = type;

	m_bIsRecording = 0;
	m_bIsPlaying = 0;

	// define note types
	wxString vectNoteType[3];
	vectNoteType[0] = wxT("Text");
	vectNoteType[1] = wxT("Audio");

	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	/////////////////////////////
	// basic panel sizer - layout of elements
	wxFlexGridSizer* mainSizer = new wxFlexGridSizer( 3, 1, 0, 0 );
	mainSizer->AddGrowableCol( 1 );
	wxFlexGridSizer* headSizer = new wxFlexGridSizer( 1, 4, 0, 0 );
	mainSizer->Add( headSizer, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );

	///////////////////////////
	// CREATE GUI OBJECTS
	m_pNoteType = new wxChoice( this, wxID_IMAGE_NOTE_TYPE, wxDefaultPosition, wxSize(70,-1), 2, 
										vectNoteType, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pNoteType->SetSelection(0);
	m_pRecord = new wxToggleButton( this, wxID_IMAGE_NOTE_RECORD, wxT("Record"), wxPoint(-1, -1), wxSize(60,-1) );
	m_pPlay = new wxToggleButton( this, wxID_IMAGE_NOTE_PLAY, wxT("Play"), wxPoint(-1, -1), wxSize(60,-1) );

	// by type
	if( m_nDlgMode == 2 || m_nDlgMode == 3 )
	{
		// GUI :: NOTE TITLE
		m_pNoteTitle = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(350,-1), wxTE_READONLY );
		// GUI :: NOTE BODY   
		m_pNoteBody = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(350,100), wxTE_MULTILINE|wxTE_RICH|wxTE_RICH2|wxTE_READONLY );
	} else
	{
		// GUI :: NOTE TITLE
		m_pNoteTitle = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(350,-1) );
		// GUI :: NOTE BODY 
		m_pNoteBody = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(350,100), wxTE_MULTILINE|wxTE_RICH|wxTE_RICH2 );
	}

	///////////////////////////////////
	// action note types and buttons
	headSizer->Add( new wxStaticText( this, -1, wxT("Type:") ), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	headSizer->Add( m_pNoteType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	headSizer->Add( m_pRecord, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	headSizer->Add( m_pPlay, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );

	// set note title
	mainSizer->Add( m_pNoteTitle, 1, wxGROW|wxALL, 5 );
	// set note body
	mainSizer->Add( m_pNoteBody, 1, wxGROW|wxALL, 5 );

	// add to main sizer
	topsizer->Add( mainSizer, 1, wxGROW|wxALL, 5 );
	// add main buttons
	if( m_nDlgMode == 2 || m_nDlgMode == 3 )
		topsizer->Add( CreateButtonSizer(wxOK), 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxALL, 10 );
	else
		topsizer->Add( CreateButtonSizer(wxOK|wxCANCEL), 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxALL, 10 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Fit();
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
CImageNotesDlg::~CImageNotesDlg( )
{
	// stop sound engine if in use by me
	if( m_bIsPlaying || m_bIsRecording ) 
	{
		if( m_bIsRecording ) m_pNote->RecordAudioNote_Stop( NULL );
		if( m_bIsPlaying ) m_pNote->PlayAudioNote_Stop( NULL );
		Sleep( 1000 );
		wxGetApp().StopSoundEngine( );
	}

	delete( m_pNoteType );
	delete( m_pRecord );
	delete( m_pPlay );
	delete( m_pNoteTitle );
	delete( m_pNoteBody );
}

// Method:		SetConfig
////////////////////////////////////////////////////////////////////
void CImageNotesDlg::SetConfig( CImageNote* pNote )
{
	m_pNote = pNote;

	// set type of note
	m_pNoteType->SetSelection( pNote->m_nType );
	SetNoteType( pNote->m_nType );

	// if edit=1 or view=2,3 - set text message of the note
	if( m_nDlgMode == 1 || m_nDlgMode == 2 || m_nDlgMode == 3 )
	{
		m_pNoteTitle->SetValue( pNote->m_strTitle );
		m_pNoteBody->SetValue( pNote->m_strMessage );
	}

	// if audio and type = 3 play
	if( pNote->m_nType == 1 && m_nDlgMode == 3 )
	{
		m_pPlay->SetValue(1);
		pNote->PlayAudioNote_Start( this->GetEventHandler() );
	}

	GetSizer()->Layout( );
	Layout( );
	Fit();

	Refresh( FALSE );
	Update( );
}

// Method:		OnRecord
////////////////////////////////////////////////////////////////////
void CImageNotesDlg::OnRecord( wxCommandEvent& pEvent )
{

	if( pEvent.IsChecked() )
	{
		// start note audio recording
		m_bIsRecording = m_pNote->RecordAudioNote_Start();

	} else
	{
		// stop note audio recordng
		if( m_bIsRecording )
		{
			m_pNote->RecordAudioNote_Stop( this->GetEventHandler() );
			m_bIsRecording = 0;
		}
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CImageNotesDlg::OnSoundEngineAction( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	// stop sound engine - use this once ??
	wxGetApp().StopSoundEngine( );
	// untoggle play button
	if( nId == wxID_NOTES_DLG_ON_PLAYING_STOPPED ) m_pPlay->SetValue(0);
}

// Method:		OnPlay
////////////////////////////////////////////////////////////////////
void CImageNotesDlg::OnPlay( wxCommandEvent& pEvent )
{
	if( pEvent.IsChecked() )
	{
		// start note audio playing
		m_bIsPlaying = m_pNote->PlayAudioNote_Start( this->GetEventHandler() );

	} else
	{
		// stop note audio recordng
		if( m_bIsPlaying )
		{
			m_pNote->PlayAudioNote_Stop( this->GetEventHandler() );
			m_bIsPlaying = 0;
		}
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CImageNotesDlg::OnNoteType( wxCommandEvent& pEvent )
{
	int nId = m_pNoteType->GetSelection();
	SetNoteType( nId );

	GetSizer()->Layout( );
	Layout( );
	Fit();

	Refresh( FALSE );
	Update( );
}

////////////////////////////////////////////////////////////////////
void CImageNotesDlg::SetNoteType( int nType )
{
	// :: if note type text = 0
	if( nType == 0 )
	{
		m_pRecord->Hide();
		m_pPlay->Hide();

	// :: if note type text = 1
	} else if( nType == 1 )
	{
		m_pRecord->Show();
		m_pPlay->Show();
	}
}
