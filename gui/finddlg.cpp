
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// local headers

// main header
#include "finddlg.h"

//		***** CLASS CFindObjectDialog *****
////////////////////////////////////////////////////////////////////
// CFindObjectDialog EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CFindObjectDialog, wxDialog)
	EVT_TEXT_ENTER( wxID_VIEW_FIND_OBJECT_ENTRY, CFindObjectDialog::OnDoFind )
	EVT_BUTTON( wxID_VIEW_FIND_OBJECT_BUTTON, CFindObjectDialog::OnDoFind )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CFindObjectDialog
// Purpose:	Initialize add devicedialog
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CFindObjectDialog::CFindObjectDialog( wxWindow *parent, const wxString& title ) : wxDialog(parent, -1,
                          title,
                          wxDefaultPosition,
                          wxDefaultSize,
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL)
{

	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	// add basic config panel
	wxPanel* pMainPanel = new wxPanel( this, -1 );

	/////////////////////////////
	// basic panel sizer - layout of elements
	wxFlexGridSizer *nameSizer = new wxFlexGridSizer( 1,3,0,0 );
	nameSizer->AddGrowableCol( 1 );

	// create my data objects
	m_pFindObjectEntry = new wxTextCtrl( pMainPanel, wxID_VIEW_FIND_OBJECT_ENTRY, wxT(""),
									wxDefaultPosition, wxSize(150,-1), wxTE_PROCESS_ENTER );
	m_pFindObjectButton = new wxButton( pMainPanel, wxID_VIEW_FIND_OBJECT_BUTTON, wxT("Find"),
					wxPoint(-1, -1), wxSize(60,-1) );

	///////////////////////////////////
	// group name entry
	nameSizer->Add(new wxStaticText(pMainPanel, -1, wxT("Object Name:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	nameSizer->Add( m_pFindObjectEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	nameSizer->Add( m_pFindObjectButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// calculate sizers
	topsizer->Add( pMainPanel, 1, wxGROW | wxALL, 10 );

	pMainPanel->SetSizer( nameSizer );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CFindObjectDialog
// Purpose:	Delete my object
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CFindObjectDialog::~CFindObjectDialog( )
{
	delete( m_pFindObjectEntry );
	delete( m_pFindObjectButton );
}

////////////////////////////////////////////////////////////////////
// Method:		OnDoFind
// Class:		CFindObjectDialog
// Purpose:		when find button pressed
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CFindObjectDialog::OnDoFind( wxCommandEvent& event )
{
	EndModal( wxID_OK );

	return;
}