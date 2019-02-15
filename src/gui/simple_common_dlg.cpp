////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/wxprec.h"
#include <wx/spinctrl.h>

// main header
#include "simple_common_dlg.h"

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSimpleEntryAddDlg
// Purpose:	Initialize object
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CSimpleEntryAddDlg::CSimpleEntryAddDlg( wxWindow *parent, wxString title, wxString label, int nType ) 
						: wxDialog( parent, -1,
                          title, wxDefaultPosition,
                          wxDefaultSize, //wxSize( 350, 310 ),
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	m_nType = nType;
	m_pName = NULL;
	m_pNameSelect = NULL;
	m_pNameSpin = NULL;

	// main dialog sizer
	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	//////////////////////////////////
	// Mouse panel sizer
	wxFlexGridSizer *sizerEntryPanel = new wxFlexGridSizer( 1, 2, 5, 10 );
	
	sizerEntryPanel->Add(new wxStaticText( this, -1, label ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );

	if( m_nType == 0 )
	{
		m_pName = new wxTextCtrl( this, -1, _T(""), wxDefaultPosition, wxSize(140,-1) );
		sizerEntryPanel->Add( m_pName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	} else if( m_nType == 1 )
	{
		m_pNameSelect = new wxChoice( this, -1, wxDefaultPosition, wxSize(120,-1) );
		sizerEntryPanel->Add( m_pNameSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	} else if( m_nType == 2 )
	{
		m_pNameSpin = new wxSpinCtrl( this, -1, wxT("0"), wxDefaultPosition, wxSize(70,-1), wxSP_ARROW_KEYS, 0, 60000, 0 );
		sizerEntryPanel->Add( m_pNameSpin, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	}


	topsizer->Add( sizerEntryPanel, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW | wxALL, 3 );
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL ), 1, wxALIGN_CENTER| wxCENTRE | wxALL, 5 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);

}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSimpleEntryAddDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CSimpleEntryAddDlg::~CSimpleEntryAddDlg( ) 
{
	if( m_pName ) delete( m_pName );
	if( m_pNameSelect ) delete( m_pNameSelect );
	if( m_pNameSpin ) delete( m_pNameSpin );
}
