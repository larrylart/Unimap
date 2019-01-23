
// local headers
#include "../config/mainconfig.h"
#include "dynamic_img_view.h"
#include "waitdialog.h"
#include "../unimap.h"
#include "../unimap_worker.h"
// sources
#include "../weather/satimg_fetch.h"

// main header
#include "weather_satvis_dlg.h"

// class:	CEarthmMapDlg
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CWeatherVisDlg, wxDialog )
	EVT_CHOICE( wxID_SAT_IMAGE_SOURCE, CWeatherVisDlg::OnSelectSatSource )
	EVT_CHOICE( wxID_SAT_IMAGE_VIEW_ZONE, CWeatherVisDlg::OnSelectVisZone )
	EVT_CHOICE( wxID_SAT_IMAGE_VIS_TYPE, CWeatherVisDlg::OnSelectVisZone )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CWeatherVisDlg
// Purpose:	Initialize my dialog
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CWeatherVisDlg::CWeatherVisDlg( wxWindow *parent, const wxString& strTitle, CUnimapWorker* pUnimapWorker )
               : wxDialog(parent, -1, strTitle, wxDefaultPosition, wxSize( 360, 440 ), wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	m_pUnimapWorker = pUnimapWorker;

	m_nSourceType = 0;
	m_nImgDateTime = 0.0;
	m_strImgFile = wxT("");
	m_bIsRemoteImage = 0;

	// sat source
	wxString vectSatSource[4];
	vectSatSource[0] = wxT("EuroMeteo.com");
	vectSatSource[1] = wxT("MeteoAM.it");
	vectSatSource[2] = wxT("fvalk.com");
	// zone
	wxString vectViewZone[2];
	vectViewZone[0] = wxT("Auto(Lat/Lon)");
	// sat band
	wxString vectVisType[4];
	vectVisType[0] = wxT("Visible");
	vectVisType[1] = wxT("Infrared");
	vectVisType[2] = wxT("Water Vapour");

	// main dialog sizer
	wxFlexGridSizer *pTopSizer = new wxFlexGridSizer( 3, 1, 5, 5 );
	pTopSizer->AddGrowableRow( 0 );
	pTopSizer->AddGrowableRow( 2 );
	// select sizers
	wxFlexGridSizer* dataImgDateTime = new wxFlexGridSizer( 1, 2, 2, 5 );
	wxFlexGridSizer* dataVisType = new wxFlexGridSizer( 1, 6, 2, 5 );

	// create gui objects
	m_pImgView = new CDynamicImgView( this, wxDefaultPosition, wxSize( 540, 380 ) );
	m_pImgDateTime = new wxStaticText( this, -1, wxT(""));
	m_pImgDateTime->SetForegroundColour( *wxRED );
	m_pSatSource = new wxChoice( this, wxID_SAT_IMAGE_SOURCE,
										wxDefaultPosition,
										wxSize(-1,-1), 3, vectSatSource, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSatSource->SetSelection( 0 );
	// :: view zone - ie asia, europe, etc
	m_pViewZone = new wxChoice( this, wxID_SAT_IMAGE_VIEW_ZONE,
										wxDefaultPosition,
										wxSize(200,-1), 1, vectViewZone, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pViewZone->SetSelection( 0 );
	m_pVisType = new wxChoice( this, wxID_SAT_IMAGE_VIS_TYPE,
										wxDefaultPosition,
										wxSize(-1,-1), 3, vectVisType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pVisType->SetSelection( 0 );

	////////////
	// data layout
	pTopSizer->Add( dataVisType, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxTOP, 5 );
	// :: Image date/time
	dataImgDateTime->Add( new wxStaticText( this, -1, wxT("Satellite Image Date/Time:")), 1, wxALIGN_RIGHT | wxALIGN_TOP | wxALL, 2  );
	dataImgDateTime->Add( m_pImgDateTime, 1, wxALIGN_LEFT | wxALIGN_TOP | wxALL, 2  );
	// :: source
	dataVisType->Add( new wxStaticText( this, -1, wxT("Source:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataVisType->Add( m_pSatSource, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: zone
	dataVisType->Add( new wxStaticText( this, -1, wxT("Zone:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataVisType->Add( m_pViewZone, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: band
	dataVisType->Add( new wxStaticText( this, -1, wxT("Band:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataVisType->Add( m_pVisType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// add to main sizer
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
// Class:	CWeatherVisDlg
// Purpose:	destroy my dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CWeatherVisDlg::~CWeatherVisDlg( ) 
{
	if( m_pImgView ) delete( m_pImgView );

	delete( m_pImgDateTime );
	delete( m_pSatSource );
	delete( m_pViewZone );
	delete( m_pVisType );
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CWeatherVisDlg
// Purpose:	Set and get my reference from the config object
// Input:	pointer to main config
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CWeatherVisDlg::SetConfig( CConfigMain* pMainConfig )
{
//	int bFound=0, nOpt=0;

	m_pSatSource->SetSelection( 0 );
	m_nSourceType = 0;
	SetOptions( );

	m_nVisType = m_pVisType->GetSelection();
	m_nZoneId = m_pViewZone->GetSelection();

/*	///////////////////////////
	// this is to get try to get the option which works
	// it starts at option zero and less then 3
	while( !bFound && nOpt < 3 )
	{
		m_pSatSource->SetSelection( nOpt );
		m_nSourceType = nOpt;
		SetOptions( );

		m_nVisType = m_pVisType->GetSelection();
		m_nZoneId = m_pViewZone->GetSelection();

		bFound = FetchRemoteImage( );

		// if not found increment counter
		if( !bFound )
			nOpt++;
		else
			break;
	}
*/
	return(1);
}

////////////////////////////////////////////////////////////////////
void CWeatherVisDlg::SetOptions( )
{
	int i=0, nZones=0;
	wxString* vectZones=NULL;

	// get data by source type
	// :: eumeteosat.com 
	if( m_nSourceType == 0 )
	{
		vectZones = vectZonesEeumetsat;
		nZones = nZonesEeumetsat;

	// :: meteoam.it
	} else if( m_nSourceType == 1 )
	{
		vectZones = vectZonesMeteoamIt;
		nZones = nZonesMeteoamIt;

	// :: fvalk.com
	} else if( m_nSourceType == 2 )
	{
		vectZones = vectZonesFvalk;
		nZones = nZonesFvalk;
	}

	m_pViewZone->Freeze();
	m_pViewZone->Clear();
	m_pViewZone->Append( wxT("Auto(Lat/Lon)") );
	// fill in the zones
	for( i=0; i<nZones; i++ )
	{
		m_pViewZone->Append( vectZones[i] );
	}

	m_pViewZone->Thaw();
	m_pViewZone->SetSelection( 0 );

	// enable/disable by source type
	// :: meteoam.it
	if( m_nSourceType == 0 )
	{
		m_pVisType->Enable();
		m_pViewZone->Enable();
		//m_pViewZone->SetSelection( 1 );
	
	// :: fvalk.com
	} else if( m_nSourceType == 1 )
	{
		m_pVisType->Disable();
		m_pViewZone->Enable();

	// :: eumeteosat.com
	} else if( m_nSourceType == 2 )
	{
		m_pVisType->Disable();
		m_pViewZone->Enable();
	}

	return;
}

////////////////////////////////////////////////////////////////////
int CWeatherVisDlg::FetchRemoteImage( )
{
	m_nImgDateTime = 0.0;
	m_strImgFile = wxT("");
	m_bIsRemoteImage = 0;

	// check by source type
	if( m_nSourceType == 0 )
		m_bIsRemoteImage = Get_LatestWeatherSatImage_EuroMeteo( 0, m_nZoneId, 0.0, 0.0, m_strImgFile, m_nImgDateTime );
	else if( m_nSourceType == 1 )
		m_bIsRemoteImage = Get_LatestWeatherSatImage_MeteoamIt( m_nVisType, m_strImgFile, m_nImgDateTime );
	else if( m_nSourceType == 2 )
		m_bIsRemoteImage = Get_LatestWeatherSatImage_fvalk( m_nZoneId, 0, 0, m_strImgFile, m_nImgDateTime );		

	return( m_bIsRemoteImage );
}

////////////////////////////////////////////////////////////////////
int CWeatherVisDlg::FetchRemoteImageDlg( )
{
	///////////
	// prepare action command
	DefCmdAction act;
	act.id = THREAD_ACTION_WEATHER_SATELLITE_IMAGE;
	act.vectObj[0] = (void*) this;

	/////////////////
	// show wait dialog
	CWaitDialog waitDialog( this, wxT("Wait..."), wxT("Fetch Remote Satellite Image..."), 0, m_pUnimapWorker, &act );
	waitDialog.ShowModal();

	if( m_bIsRemoteImage ) 
		SetRemoteImage( );
	else
		wxMessageBox( wxT("No Satellite Image Avaialable!"), wxT("Error Fetching Image"), wxOK|wxICON_ERROR );

	return( m_bIsRemoteImage );
}

////////////////////////////////////////////////////////////////////
void CWeatherVisDlg::SetRemoteImage( )
{
	// if found ...
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
void CWeatherVisDlg::OnSelectSatSource( wxCommandEvent& pEvent )
{
	int nPrevSource = m_nSourceType;
	m_nSourceType = m_pSatSource->GetSelection();

	SetOptions( );

	if( FetchRemoteImageDlg( ) )
	{
		Refresh( false );

	} else
	{
		m_nSourceType = nPrevSource;
		m_pSatSource->SetSelection(m_nSourceType);
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CWeatherVisDlg::OnSelectVisZone( wxCommandEvent& pEvent )
{
	int nVisType = m_nVisType;
	int nZoneId = m_nZoneId;
	m_nVisType = m_pVisType->GetSelection();
	m_nZoneId = m_pViewZone->GetSelection();

	if( FetchRemoteImageDlg( ) )
	{
		Refresh( false );

	} else
	{
		m_nVisType = nVisType;
		m_nZoneId = nZoneId;
	}

	return;
}

