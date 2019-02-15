////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/wxprec.h"

// local includes
#include "../image/astroimage.h"
#include "../image/imagegroup.h"
#include "../online/unimap_online.h"

// main header
#include "user_feedback_dlg.h"

BEGIN_EVENT_TABLE(CUserFeedbackDlg, wxDialog)
//	EVT_BUTTON( wxID_EXPORT_FILE_SET_BUTTON, CUserFeedbackDlg::OnFileSet )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////
CUserFeedbackDlg::CUserFeedbackDlg( wxWindow *parent, wxString title, CUnimapOnline* pOnline ) 
						: wxDialog( parent, -1,
                          title, wxDefaultPosition,
                          wxDefaultSize, //wxSize( 350, 310 ),
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	m_pUnimapOnline = pOnline;

	// types
	wxString vectTypes[4] = { wxT("Bug Report"), wxT("Improvement"), wxT("New Function"), wxT("Any Other") };
	// sections
	wxString vectSection[10]; 
	vectSection[0] = wxT("Detection");
	vectSection[1] = wxT("Hints");
	vectSection[2] = wxT("Matching");
	vectSection[3] = wxT("Seeing");
	vectSection[4] = wxT("Instruments");

	// main dialog sizer
	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 1, 1, 5, 5 );

	// entries panel sizer
//	wxFlexGridSizer *sizerEntryPanel = new wxFlexGridSizer( 1, 2, 5, 10 );

	// define gui elements
	m_pType = new wxChoice( this, -1, wxDefaultPosition, wxSize(150,-1), 4, vectTypes );
	m_pType->SetSelection(0);
	m_pSection = new wxChoice( this, -1, wxDefaultPosition, wxSize(120,-1), 5, vectSection );
	m_pSection->SetSelection(0);
	m_pTitle = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(300,-1) );
	m_pBody = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(350,100), wxTE_MULTILINE|wxTE_RICH|wxTE_RICH2 );
	m_pSubmit = new wxButton( this, wxID_USER_FEEDBACK_SUBMIT, wxT("Submit"), wxPoint(-1, -1), wxSize(60,-1) );
	m_pCancel = new wxButton( this, wxID_USER_FEEDBACK_CANCEL, wxT("Cancel"), wxPoint(-1, -1), wxSize(60,-1) );
	SetAffirmativeId( wxID_USER_FEEDBACK_SUBMIT );
	SetEscapeId( wxID_USER_FEEDBACK_CANCEL );

	// :: header
	wxFlexGridSizer *sizerHead = new wxFlexGridSizer( 1, 4, 5, 0 );
	sizerHead->SetFlexibleDirection(wxHORIZONTAL);
	// types
	sizerHead->Add(new wxStaticText( this, -1, wxT("Type:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );
	sizerHead->Add( m_pType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// sections
	sizerHead->Add(new wxStaticText( this, -1, wxT("Section:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	sizerHead->Add( m_pSection, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	topsizer->Add( sizerHead, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW|wxLEFT|wxRIGHT|wxTOP, 5 );

	// :: subject
	wxFlexGridSizer *sizerTitle = new wxFlexGridSizer( 1, 2, 5, 0 );
	sizerTitle->AddGrowableCol(1);
	sizerTitle->Add(new wxStaticText( this, -1, wxT("Subject:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	sizerTitle->Add( m_pTitle, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxEXPAND|wxLEFT, 5 );
	topsizer->Add( sizerTitle, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxLEFT|wxRIGHT, 5 );

	// :: body
	topsizer->Add( m_pBody, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND|wxLEFT|wxRIGHT, 5 );

	// :: buttons
	wxFlexGridSizer *sizerButtons = new wxFlexGridSizer( 1, 2, 5, 0 );
//	sizerButtons->SetFlexibleDirection(wxHORIZONTAL);
	sizerButtons->AddGrowableCol(0);
	sizerButtons->AddGrowableCol(1);
	sizerButtons->Add( m_pSubmit, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerButtons->Add( m_pCancel, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	topsizer->Add( sizerButtons, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxBOTTOM, 5 );

//	topsizer->Add( sizerEntryPanel, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW | wxALL, 3 );
//	topsizer->Add((wxOK | wxCANCEL ), 1, wxALIGN_CENTER| wxCENTRE | wxALL, 5 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);

}

////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////
CUserFeedbackDlg::~CUserFeedbackDlg( ) 
{
	delete( m_pType );
	delete( m_pSection );
	delete( m_pTitle );
	delete( m_pBody );
	delete( m_pSubmit );
	delete( m_pCancel );
}


////////////////////////////////////////////////////////////////////
//void CUserFeedbackDlg::Onsss( wxCommandEvent& pEvent )
//{
//}
