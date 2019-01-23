
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// local headers
#include "../online/unimap_online.h"

// main header
#include "upload_custom_dlg.h"

//		***** CLASS CUploadCustomDlg *****
////////////////////////////////////////////////////////////////////
// CUploadCustomDlg EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CUploadCustomDlg, wxDialog)
//	EVT_TEXT_ENTER( wxID_VIEW_FIND_OBJECT_ENTRY, CUploadCustomDlg::OnDoFind )
//	EVT_BUTTON( wxID_VIEW_FIND_OBJECT_BUTTON, CUploadCustomDlg::OnDoFind )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CUploadCustomDlg
// Purpose:	Initialize add devicedialog
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CUploadCustomDlg::CUploadCustomDlg( wxWindow *parent, const wxString& title ) : wxDialog(parent, -1,
                          title,
                          wxDefaultPosition,
                          wxDefaultSize,
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL)
{

	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 2,1,0,0 );
	topsizer->AddGrowableRow( 1 );


	/////////////////////////////
	// basic panel sizer - layout of elements
	wxFlexGridSizer* headSizer = new wxFlexGridSizer( 1,3,0,0 );
	headSizer->AddGrowableCol( 1 );
//	wxFlexGridSizer* bodySizer = new wxFlexGridSizer( 1,3,0,0 );

	// create my data objects
//	m_pPrevButton = new wxButton( this, wxID_CATALOG_VIEW_PAGE_BACK, "<< Back", wxPoint(-1, -1), wxSize(60,-1) );
//	m_pNextButton = new wxButton( this, wxID_CATALOG_VIEW_PAGE_NEXT, "Next >>", wxPoint(-1, -1), wxSize(60,-1) );

	// the tables
//	m_pCatalogTable = new wxListCtrl( this, -1,
//			wxPoint(-1,-1), wxSize(-1,-1), wxLC_REPORT|wxLC_HRULES|wxLC_VRULES );
	

	///////////////////////////////////
	// head command 
	headSizer->Add( new wxStaticText( this, -1, wxT("Not yet implemented!") ), 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );
//	headSizer->Add( m_pNextButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// add to main sizer
	topsizer->Add( headSizer, 1, wxGROW | wxALL|wxALIGN_CENTER, 10 );

	// set table
//	topsizer->Add( m_pCatalogTable, 1, wxGROW | wxALL, 10 );

	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CUploadCustomDlg
// Purpose:	Delete my object
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CUploadCustomDlg::~CUploadCustomDlg( )
{
//	delete( m_pPrevButton );
//	delete( m_pNextButton );
//	delete( m_pCatalogTable );
}

////////////////////////////////////////////////////////////////////
// Method:		SetConfig
// Class:		CUploadCustomDlg
// Purpose:		set local configuration
// Input:		pointer to unimap online object
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CUploadCustomDlg::SetConfig( CUnimapOnline* pUnimapOnline )
{
	m_pUnimapOnline = pUnimapOnline;

	Refresh( FALSE );
	Update( );
}

////////////////////////////////////////////////////////////////////
// Method:		OnAddToUploadList
// Class:		CUploadCustomDlg
// Purpose:		when find button pressed
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
//void CUploadCustomDlg::OnAddToUploadList( wxCommandEvent& event )
//{
//
//	return;
//}
