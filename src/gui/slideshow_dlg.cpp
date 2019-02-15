////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system
#include <math.h>
#include <stdio.h>
#include <vector>

// wx
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/wxprec.h"

// local includes
#include "../image/imagegroup.h"
#include "../image/imagedb.h"
#include "../config/mainconfig.h"

// main header
#include "slideshow_dlg.h"

////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////
CSlideShowDlg::CSlideShowDlg( wxWindow *parent, wxString title, CImageDb* pImageDb, CConfigMain* pMainConfig ) 
								: wxDialog( parent, -1, title, wxDefaultPosition,
									wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL )
{
	m_pImageDb = pImageDb;

	wxString vectOrder[3] = { wxT("Sequential"), wxT("Shuffle") };

	// main dialog sizer
	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 1, 1, 5, 5 );

	// entries panel sizer
	wxFlexGridSizer *sizerEntryPanel = new wxFlexGridSizer( 1, 2, 5, 10 );

	// define gui elements
	// :: sources of images
	if( m_pImageDb->m_pCurrentGroup->m_vectStack.size() > 0 )
	{
		wxString vectSources[4] = { wxT("Group"), wxT("Stack"), wxT("All") };
		m_pSources = new wxChoice( this, -1, wxDefaultPosition, wxSize(120,-1), 3, vectSources );

	} else
	{
		wxString vectSources[3] = { wxT("Group"), wxT("All") };
		m_pSources = new wxChoice( this, -1, wxDefaultPosition, wxSize(120,-1), 2, vectSources );
	}
	m_pSources->SetSelection(0);
	// :: order of display
	m_pOrder = new wxChoice( this, -1, wxDefaultPosition, wxSize(120,-1), 2, vectOrder );
	m_pOrder->SetSelection(0);
	// :: image show time
	m_pShowTime = new wxSpinCtrl( this, -1, wxEmptyString, wxDefaultPosition,
									wxSize(60,-1), wxSP_ARROW_KEYS, 0, 60000, 5 );
	// :: repeat - continous play
	m_pRepeat = new wxCheckBox( this, -1, wxT("") );
	// :: full screen
	m_pFullScreen = new wxCheckBox( this, -1, wxT("") );

	// :: sources of images
	sizerEntryPanel->Add(new wxStaticText( this, -1, wxT("Use Images From:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pSources, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: play order
	sizerEntryPanel->Add(new wxStaticText( this, -1, wxT("Play Order:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pOrder, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: image show time
	sizerEntryPanel->Add(new wxStaticText( this, -1, wxT("Display Time(s):") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pShowTime, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: repeat - continous play
	sizerEntryPanel->Add(new wxStaticText( this, -1, wxT("Continuously Play:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pRepeat, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: full screen
	sizerEntryPanel->Add(new wxStaticText( this, -1, wxT("Show Full Screen:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pFullScreen, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	topsizer->Add( sizerEntryPanel, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW | wxALL, 5 );
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL ), 1, wxALIGN_CENTER| wxCENTRE | wxALL, 5 );

	// check config
	if( pMainConfig ) SetConfig( pMainConfig );

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
CSlideShowDlg::~CSlideShowDlg( ) 
{
	delete( m_pSources );
	delete( m_pOrder );
	delete( m_pShowTime );
	delete( m_pRepeat );
	delete( m_pFullScreen );
}

////////////////////////////////////////////////////////////////////
void CSlideShowDlg::SetConfig( CConfigMain* pMainConfig )
{
	m_pMainConfig = pMainConfig;
	// set locals
	m_pSources->SetSelection( m_pMainConfig->m_nSlideShowSources );
	m_pOrder->SetSelection( m_pMainConfig->m_nSlideShowOrder );
	m_pShowTime->SetValue( m_pMainConfig->m_nSlideShowDelay );
	m_pRepeat->SetValue( m_pMainConfig->m_nSlideShowRepeat );
	m_pFullScreen->SetValue( m_pMainConfig->m_nSlideShowFullScreen );
}
