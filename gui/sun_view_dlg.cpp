
// local headers
#include "../config/mainconfig.h"
#include "dynamic_img_view.h"
#include "waitdialog.h"
#include "../unimap_worker.h"
// sources
#include "../sky/sun_fetch.h"

// main header
#include "sun_view_dlg.h"

// class:	CSunViewDlg
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CSunViewDlg, wxDialog )
//	EVT_CHOICE( wxID_SUN_VIEW_IMAGE_SOURCE, CSunViewDlg::OnSelectSunImgSource )
	EVT_CHOICE( wxID_SUN_VIEW_IMAGE_BAND, CSunViewDlg::OnSelectObsBandType )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSunViewDlg
// Purpose:	Initialize my dialog
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CSunViewDlg::CSunViewDlg( wxWindow *parent, const wxString& strTitle, CUnimapWorker* pUnimapWorker )
               : wxDialog(parent, -1, strTitle, wxDefaultPosition, wxSize( 360, 440 ), wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	m_pUnimapWorker = pUnimapWorker;
	m_nSourceType = 0;
	m_strImgFile = wxT("");
	m_nImgDateTime = 0.0;
	m_bIsRemoteImage = 0;

	// sun image source
/*	wxString vectSunImgSource[4];
	vectSunImgSource[0] = "soho.easc.int";
	vectSunImgSource[1] = "soho.nasa.gov";
	vectSunImgSource[2] = "xrt.cfa.harvard.edu";
*/	// Obs Band Type
	wxString vectObsBandType[11];
	vectObsBandType[0] = wxT("EIT 171");
	vectObsBandType[1] = wxT("EIT 195");
	vectObsBandType[2] = wxT("EIT 284");
	vectObsBandType[3] = wxT("EIT 304");
	vectObsBandType[4] = wxT("MDI Continuum");
	vectObsBandType[5] = wxT("MDI Magnetogram");
	vectObsBandType[6] = wxT("LASCO C2");
	vectObsBandType[7] = wxT("LASCO C3");
	vectObsBandType[8] = wxT("X-Ray");
	vectObsBandType[9] = wxT("Last Event B-Field");

	// main dialog sizer
	wxFlexGridSizer *pTopSizer = new wxFlexGridSizer( 3, 1, 5, 5 );
	pTopSizer->AddGrowableRow( 0 );
	pTopSizer->AddGrowableRow( 2 );
	// select sizers
	wxFlexGridSizer* dataImgDateTime = new wxFlexGridSizer( 1, 2, 2, 5 );
	wxFlexGridSizer* dataVisType = new wxFlexGridSizer( 1, 2, 2, 5 );

	// create gui objects
	m_pImgView = new CDynamicImgView( this, wxDefaultPosition, wxSize( 400, 400 ) );
	m_pImgDateTime = new wxStaticText( this, -1, wxT(""));
	m_pImgDateTime->SetForegroundColour( *wxRED );
/*	m_pSunImgSource = new wxChoice( this, wxID_SUN_VIEW_IMAGE_SOURCE,
										wxDefaultPosition,
										wxSize(-1,-1), 3, vectSunImgSource, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSunImgSource->SetSelection( 0 );
*/
	m_pObsBandType = new wxChoice( this, wxID_SUN_VIEW_IMAGE_BAND,
										wxDefaultPosition,
										wxSize(-1,-1), 10, vectObsBandType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pObsBandType->SetSelection( 0 );

	////////////
	// data layout
	// :: band
	dataVisType->Add( new wxStaticText( this, -1, wxT("Band:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataVisType->Add( m_pObsBandType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: Image date/time
	dataImgDateTime->Add( new wxStaticText( this, -1, wxT("Sun Image Date/Time:")), 1, wxALIGN_RIGHT | wxALIGN_TOP | wxALL, 2  );
	dataImgDateTime->Add( m_pImgDateTime, 1, wxALIGN_LEFT | wxALIGN_TOP | wxALL, 2  );
/*	// :: source
	dataVisType->Add( new wxStaticText( this, -1, _T("Source:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataVisType->Add( m_pSunImgSource, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
*/
	// :: band
//	dataVisType->Add( new wxStaticText( this, -1, wxT("Band:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
//	dataVisType->Add( m_pObsBandType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// add to main sizer
	pTopSizer->Add( dataVisType, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxTOP|wxBOTTOM, 5 );
	pTopSizer->Add( m_pImgView, 0, wxGROW|wxEXPAND|wxLEFT|wxRIGHT, 10 );
	pTopSizer->Add( dataImgDateTime, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxBOTTOM, 5 );

//	pTopSizer->Add( CreateButtonSizer(wxOK), 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10 );

	SetSizer(pTopSizer);
	SetAutoLayout(TRUE);
	pTopSizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSunViewDlg
// Purpose:	destroy my dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CSunViewDlg::~CSunViewDlg( ) 
{
	if( m_pImgView ) delete( m_pImgView );

	delete( m_pImgDateTime );
//	delete( m_pSunImgSource );
	delete( m_pObsBandType );
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CSunViewDlg
// Purpose:	Set and get my reference from the config object
// Input:	pointer to main config
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CSunViewDlg::SetConfig( CConfigMain* pMainConfig )
{
//	m_nSourceType = m_pSunImgSource->GetSelection();
//	SetOptions( );

	m_nSourceType = m_pObsBandType->GetSelection();
//	int bFound = FetchRemoteImage( );
}

////////////////////////////////////////////////////////////////////
int CSunViewDlg::FetchRemoteImage( )
{
	m_strImgFile = wxT("");
	m_nImgDateTime = 0.0;
	m_bIsRemoteImage = 0;
	
	// check by source type
	if( m_nSourceType >= 0 && m_nSourceType < 8 )
		m_bIsRemoteImage = GetLatest_SunSohoImage( 0, m_nSourceType, m_strImgFile, m_nImgDateTime );
	else if( m_nSourceType == 8 )
		m_bIsRemoteImage = GetLatest_SunXrtImage( 0, m_strImgFile, m_nImgDateTime );
	else if( m_nSourceType == 9 )
		m_bIsRemoteImage = GetLatest_SunEventImageLmsal( 0, m_strImgFile, m_nImgDateTime );

	return( m_bIsRemoteImage );
}

////////////////////////////////////////////////////////////////////
int CSunViewDlg::FetchRemoteImageDlg()
{
	///////////
	// prepare action command
	DefCmdAction act;
	act.id = THREAD_ACTION_WEATHER_SUN_VIEW_IMAGE;
	act.vectObj[0] = (void*) this;

	/////////////////
	// show wait dialog
	CWaitDialog waitDialog( this, wxT("Wait..."), wxT("Fetching Remote Sun Image..."), 0, m_pUnimapWorker, &act );
	waitDialog.ShowModal();

	if( m_bIsRemoteImage ) 
		SetRemoteImage( );
	else
		wxMessageBox( wxT("No Sun Image Avaialable!"), wxT("Error Fetching Image"), wxOK|wxICON_ERROR );

	return( m_bIsRemoteImage );
}

////////////////////////////////////////////////////////////////////
void CSunViewDlg::SetRemoteImage( )
{
	if( m_bIsRemoteImage )
	{
		// load image from file
		m_pImgView->SetImageFromFile( m_strImgFile );
		// set image date time
		wxDateTime imgDateTime( m_nImgDateTime );
		m_pImgDateTime->SetLabel( imgDateTime.Format( wxT("%d/%m/%Y %H:%M:%S") ) );
	}
}

////////////////////////////////////////////////////////////////////
void CSunViewDlg::OnSelectObsBandType( wxCommandEvent& pEvent )
{
	int nSourceType = m_nSourceType;
	m_nSourceType = m_pObsBandType->GetSelection();

	if( FetchRemoteImageDlg( ) )
	{
		Refresh( false );

	} else
	{
		m_nSourceType = nSourceType;
		m_pObsBandType->SetSelection(m_nSourceType);
	}

	return;
}

