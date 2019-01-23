
// local headers
#include "dynamic_img_view.h"

// main header
#include "img_view_dlg.h"

// class:	CImageViewDlg
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CImageViewDlg, wxDialog )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CImageViewDlg
// Purpose:	Initialize my dialog
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CImageViewDlg::CImageViewDlg( wxWindow *parent, const wxString& strTitle )
               : wxDialog(parent, -1, strTitle, wxDefaultPosition, wxSize( 640, 480 ), wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL )
{

	// main dialog sizer
	wxFlexGridSizer *pTopSizer = new wxFlexGridSizer( 1, 1, 0, 0 );
	pTopSizer->AddGrowableRow( 0 );

	// create gui objects
	m_pImgView = new CDynamicImgView( this, wxDefaultPosition, wxSize( 640, 480 ) );

	// add to main sizer
	pTopSizer->Add( m_pImgView, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW|wxEXPAND|wxALL, 5 );

//	pTopSizer->Add( CreateButtonSizer(wxOK), 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10 );

	SetSizer(pTopSizer);
	SetAutoLayout(TRUE);
	pTopSizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CImageViewDlg
// Purpose:	destroy my dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CImageViewDlg::~CImageViewDlg( ) 
{
	if( m_pImgView ) delete( m_pImgView );
}

// Method:	CImageViewDlg
////////////////////////////////////////////////////////////////////
void CImageViewDlg::SetImageFromFile( const wxString& strImgFile ) 
{
	// load image from file
	m_pImgView->SetImageFromFile( strImgFile );

	Refresh( false );
}
