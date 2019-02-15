////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// local headers
#include "../config/mainconfig.h"
#include "dynamic_img_view.h"
#include "waitdialog.h"
#include "../unimap_worker.h"

// main header
#include "location_dialog.h"

// class:	CEarthmMapDlg
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CEarthmMapDlg, wxDialog )
//	EVT_INIT_DIALOG( CEarthmMapDlg::OnInitDlg )
	EVT_CHOICE( wxID_SELECT_COUNTRY, CEarthmMapDlg::OnSelectCountry )
	EVT_CHOICE( wxID_SELECT_REGION, CEarthmMapDlg::OnSelectRegion )
	EVT_CHOICE( wxID_SELECT_CITY, CEarthmMapDlg::OnSelectCity )
	EVT_COMMAND( wxID_EVENT_UPDATE_LOCATION, wxEVT_CUSTOM_MESSAGE_UPDATE_EVENT, CEarthmMapDlg::OnUpdateLocation )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CEarthmMapDlg
// Purpose:	Initialize my eart location dialog
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CEarthmMapDlg::CEarthmMapDlg( wxWindow *parent, const wxString& strTitle )
               : wxDialog(parent, -1, strTitle, wxDefaultPosition, wxSize( 360, 440 ), wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	// create my earth object here
//	m_pEarth = new CLocationEarth();

	// main dialog sizer
	wxFlexGridSizer *pTopSizer = new wxFlexGridSizer( 2, 1, 5, 5 );

	// create elements
	m_pCountry = new wxChoice( this, wxID_SELECT_COUNTRY );
	m_pRegion = new wxChoice( this, wxID_SELECT_REGION, wxDefaultPosition, wxSize(140,-1) );
	m_pCity = new wxChoice( this, wxID_SELECT_CITY, wxDefaultPosition, wxSize(170,-1) );
	m_pLatitude = new wxTextCtrl( this, -1, _T(""), wxDefaultPosition, wxSize(60,-1) );
	m_pLongitude = new wxTextCtrl( this, -1, _T(""), wxDefaultPosition, wxSize(60,-1) );
	m_pAltitude = new wxTextCtrl( this, -1, _T(""), wxDefaultPosition, wxSize(60,-1) );

	wxFlexGridSizer *dataCSizer = new wxFlexGridSizer( 1, 2, 2, 5 );
	wxFlexGridSizer *dataASizer = new wxFlexGridSizer( 1, 4, 2, 5 );
	wxFlexGridSizer *dataBSizer = new wxFlexGridSizer( 1, 6, 2, 5 );

	m_pMapView = new CEarthMapView( this, wxDefaultPosition, wxSize( 500, 300 ), wxT(""), 0 );
	m_pMapView->m_pEarth = m_pEarth;
	pTopSizer->AddGrowableRow( 0 );
	pTopSizer->Add( m_pMapView, 0, wxGROW | wxALL|wxEXPAND, 10 );

	////////////
	// data layout
	// :: Country
	dataCSizer->Add( new wxStaticText( this, -1, _T("Country:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataCSizer->Add( m_pCountry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: City
	dataASizer->Add( new wxStaticText( this, -1, _T("Provice/State:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataASizer->Add( m_pRegion, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: City
	dataASizer->Add( new wxStaticText( this, -1, _T("City:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataASizer->Add( m_pCity, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: latitude
	dataBSizer->Add( new wxStaticText( this, -1, _T("Latitude:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataBSizer->Add( m_pLatitude, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: longitude
	dataBSizer->Add( new wxStaticText( this, -1, _T("Longitude:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataBSizer->Add( m_pLongitude, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: altitude
	dataBSizer->Add( new wxStaticText( this, -1, _T("Altitude:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataBSizer->Add( m_pAltitude, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );

	pTopSizer->Add( dataCSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 1 );
	pTopSizer->Add( dataASizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 1 );
	pTopSizer->Add( dataBSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 5 );

	pTopSizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10 );

	SetSizer(pTopSizer);
	SetAutoLayout(TRUE);
	pTopSizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CEarthmMapDlg
// Purpose:	destroy my dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CEarthmMapDlg::~CEarthmMapDlg( ) 
{
	delete( m_pCountry );
	delete( m_pRegion );
	delete( m_pCity );
	delete( m_pLatitude );
	delete( m_pLongitude );
	delete( m_pAltitude );
	if( m_pMapView ) delete( m_pMapView );
//	delete( m_pEarth );
}

////////////////////////////////////////////////////////////////////
void CEarthmMapDlg::LoadData( wxWindow* parent, CUnimapWorker* pWorker )
{
	if( pWorker )
	{
		DefCmdAction act;
		act.id = THREAD_ACTION_LOAD_EARTH_MAP_DATA;
		act.vectObj[0] = (void*) m_pEarth;
		act.vectObj[1] = (void*) m_pMapView;

		CWaitDialog waitDialog( parent, wxT("Load Earth Map Data"), 
						wxT("Loading Earth Locations..."), 0, pWorker, &act );
		waitDialog.ShowModal();
	}
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CEarthmMapDlg
// Purpose:	Set and get my reference from the config object
// Input:	pointer to main config
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CEarthmMapDlg::SetConfig( CConfigMain* pMainConfig, int nCountryId, int nRegionId, int nCityId )
{
//	m_pCountry->Clear();
//	m_pEarth->LoadLocations( );
	
	SetCountries( nCountryId );
	SetRegions( nRegionId );
	SetCities( nCityId );
	SetCity( );
}

////////////////////////////////////////////////////////////////////
// Method:	SetCountries
// Class:	CEarthmMapDlg
// Purpose:	set countries
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
void CEarthmMapDlg::SetCountries( int nCountryId )
{
	int i = 0;

	m_pEarth->SetCountriesWidget( m_pCountry, nCountryId );
/*
	m_pCountry->Freeze( );
	m_pCountry->Clear();
	for( i=0; i<m_pEarth->m_nCountry; i++ )
	{
		if( m_pEarth->m_vectCountry[i].nRegion != 0 && m_pEarth->m_vectCountry[i].vectRegion != NULL )
			m_pCountry->Append( m_pEarth->m_vectCountry[i].name );
	}
	m_pCountry->Thaw( );
	// select first
	m_pCountry->SetSelection( 0 );
*/
}

////////////////////////////////////////////////////////////////////
// Method:	SetRegions
// Class:	CEarthmMapDlg
// Purpose:	set regions for current country
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
void CEarthmMapDlg::SetRegions( int nSelect )
{
	int i = 0;
	int nCountryId = m_pCountry->GetSelection( );

	m_pEarth->SetRegionsWidget( m_pRegion, nCountryId, nSelect );

/*	m_pRegion->Freeze( );
	m_pRegion->Clear();
	for( i=0; i<m_pEarth->m_vectCountry[nCountryId].nRegion; i++ )
	{
		m_pRegion->Append( m_pEarth->m_vectCountry[nCountryId].vectRegion[i].name );
	}
	m_pRegion->Thaw( );
	// select first
	m_pRegion->SetSelection( nSelect );
*/
}

////////////////////////////////////////////////////////////////////
// Method:	SetCities
// Class:	CEarthmMapDlg
// Purpose:	set cities for current country/region
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
void CEarthmMapDlg::SetCities( int nSelect )
{
	int i = 0;
	int nCountryId = m_pCountry->GetSelection( );
	int nRegionId = m_pRegion->GetSelection( );

	m_pEarth->SetCitiesWidget( m_pCity, nCountryId, nRegionId, nSelect );

/*	m_pCity->Freeze( );
	m_pCity->Clear();
	for( i=0; i<m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].nCity; i++ )
	{
		m_pCity->Append( m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[i].name );
	}
	m_pCity->Thaw( );
	// select first
	m_pCity->SetSelection( nSelect );
*/
}

////////////////////////////////////////////////////////////////////
void CEarthmMapDlg::OnSelectCountry( wxCommandEvent& pEvent )
{
	SetRegions( );
	SetCities( );
	SetCity( );
	return;
}

////////////////////////////////////////////////////////////////////
void CEarthmMapDlg::OnSelectRegion( wxCommandEvent& pEvent )
{
	SetCities( );
	SetCity( );
	return;
}

////////////////////////////////////////////////////////////////////
void CEarthmMapDlg::OnSelectCity( wxCommandEvent& pEvent )
{
	SetCity( );
	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetCity
// Class:	CEarthmMapDlg
// Purpose:	set current city info
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
void CEarthmMapDlg::SetCity( )
{
	wxString strTmp;

	int nCountryId = m_pCountry->GetSelection( );
	int nRegionId = m_pRegion->GetSelection( );
	int nCityId = m_pCity->GetSelection( );

	if( nCountryId < 0 || nRegionId < 0 || nCityId < 0 ) return;

	////////////////////////
	// :: LATITUDE :: is NORTH to SOUTH 90 to -90 
	double lat = m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].lat;
/*	if( lat >= 0 )
		sprintf( strTmp, "%.4fN", fabs(lat) );
	else
		sprintf( strTmp, "%.4fS", fabs(lat) );
*/
	strTmp.Printf( wxT("%.6f"), lat );
	m_pLatitude->SetValue( strTmp );

	////////////////////////
	// :: LONGITUDE :: is WEST - EAST ... -180 to 180
	double lon = m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].lon;
/*	if( lon >= 0 )
		sprintf( strTmp, "%.4fE", fabs(lon) );
	else
		sprintf( strTmp, "%.4fW", fabs(lon) );
*/
	strTmp.Printf( wxT("%.6f"), lon );
	m_pLongitude->SetValue( strTmp );

	/////////////////////
	// :: ALTITUDE
	strTmp.Printf( wxT("%.4f"), m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].alt );
	m_pAltitude->SetValue( strTmp );

	///////////////
	// set my city on the map
	m_pMapView->m_nLocX = (double) ((lon+180.)/360.*m_pMapView->m_nOrigPicWidth);
	m_pMapView->m_nLocY = (double) ((lat-90.)/-180.*m_pMapView->m_nOrigPicHeight);
	m_pMapView->m_strLocName = wxString(m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].name,wxConvUTF8);
	m_pMapView->bIsLocSet = 1;
	// :: also set target
	m_pMapView->m_nMousePicX = m_pMapView->m_nLocX;
	m_pMapView->m_nMousePicY = m_pMapView->m_nLocY;
	m_pMapView->m_bMainTarget = 2;

	m_pMapView->Refresh( false );
}

////////////////////////////////////////////////////////////////////
void CEarthmMapDlg::OnUpdateLocation( wxCommandEvent& pEvent )
{
	wxString strTmp;

	m_pCountry->SetSelection( m_pMapView->m_nCountryId );
	SetRegions( m_pMapView->m_nRegionId );
	SetCities( m_pMapView->m_nCityId );

	// now set target lat/log	
	// :: LATITUDE :: is NORTH to SOUTH 90 to -90 
	double lat = m_pMapView->m_nTargetLat;
/*	if( lat >= 0 )
		sprintf( strTmp, "%.4fN", fabs(lat) );
	else
		sprintf( strTmp, "%.4fS", fabs(lat) );
*/
	strTmp.Printf( wxT("%.6f"), lat );
	m_pLatitude->SetValue( strTmp );

	// :: LONGITUDE :: is WEST - EAST ... -180 to 180
	double lon = m_pMapView->m_nTargetLon;
/*	if( lon >= 0 )
		sprintf( strTmp, "%.4fE", fabs(lon) );
	else
		sprintf( strTmp, "%.4fW", fabs(lon) );
*/
	strTmp.Printf( wxT("%.6f"), lon );
	m_pLongitude->SetValue( strTmp );

	strTmp.Printf( wxT("%.4f"), m_pEarth->m_vectCountry[m_pMapView->m_nCountryId].vectRegion[m_pMapView->m_nRegionId].vectCity[m_pMapView->m_nCityId].alt );
	m_pAltitude->SetValue( strTmp );

}

////////////////////////////////////////////////////////////////////
// Method:	SetCustomPositionFromString
// Class:	CEarthmMapDlg
// Purpose:	set custom position from strings
// Input:	lat/log/alt as wxstrings
// Output:	status
////////////////////////////////////////////////////////////////////
void CEarthmMapDlg::SetCustomPositionFromString( wxString& strLat, wxString& strLon, wxString& strAlt )
{
	// we first compare
	if( strLat.CmpNoCase( m_pLatitude->GetLineText(0) ) == 0 &&
		strLon.CmpNoCase( m_pLongitude->GetLineText(0) ) == 0 )
	{
		// check if to update altitude
		if( strAlt.CmpNoCase( m_pAltitude->GetValue() ) != 0 ) m_pAltitude->SetValue( strAlt );
		return;
	}

	double lat=0.0, lon=0.0, alt=0.0;
	char strTmp[255], cho=0;
	int len=0, bError=0;

	// :: LATITUDE :: is NORTH to SOUTH 90.0 to -90.0
	strcpy( strTmp, strLat.ToAscii() );
	len = strlen(strTmp);
	if( len > 0 )
	{
		cho = strTmp[len-1];
		if( cho == 'N' || cho == 'S' ) 
		{
			strTmp[len-1] = 0;
			if( cho == 'N' )
				lat = atof( strTmp );
			else if( cho == 'S' )
				lat = -atof( strTmp );
		} else
			lat = atof( strTmp );
	}

	// :: LONGITUDE :: is WEST - EAST ... -180 to 180
	strcpy( strTmp, strLon.ToAscii() );
	len = strlen(strTmp);
	if( len > 0 )
	{
		cho = strTmp[len-1];
		if( cho == 'E' || cho == 'W' )
		{
			strTmp[len-1] = 0;
			if( cho == 'E' )
				lon = atof( strTmp );
			else if( cho == 'W' )
				lon = -atof( strTmp );

		} else
			lon = atof( strTmp );
	}
	// get alt
	alt = atof( strAlt.ToAscii() );

	// :: now set target
	SetCustomPosition( lat, lon, alt );
}

////////////////////////////////////////////////////////////////////
void CEarthmMapDlg::SetCustomPosition( double lat, double lon, double alt )
{
	wxString strTmp;

	// first we just set the strings
	strTmp.Printf( wxT("%.6lf"), lat );
	m_pLatitude->SetValue( strTmp );
	strTmp.Printf( wxT("%.6lf"), lon );
	m_pLongitude->SetValue( strTmp );
	strTmp.Printf( wxT("%.2lf"), alt );
	m_pAltitude->SetValue( strTmp );

	m_pMapView->m_nMousePicX = (double) ((lon+180.)/360.*m_pMapView->m_nOrigPicWidth);
	m_pMapView->m_nMousePicY = (double) ((lat-90.)/-180.*m_pMapView->m_nOrigPicHeight);
	m_pMapView->m_bMainTarget = 2;

	m_pMapView->Refresh( false );
}

// Method:	SetLocByName
////////////////////////////////////////////////////////////////////
void CEarthmMapDlg::SetLocByName( wxString& strCountry, wxString& strProvince, wxString& strCity, int bCity )
{
	// get here ids from astro image pointer
	int nCountryId = m_pEarth->FindCountryByName( strCountry );
	int nRegionId = 0;
	int nCityId = 0;
	if( nCountryId >= 0 )
	{
		nRegionId = m_pEarth->FindRegionByName( nCountryId, strProvince );
		if( nRegionId >= 0 )
		{
			nCityId = m_pEarth->FindCityByName( nCountryId, nRegionId, strCity );
			if( nCityId < 0 ) nCityId = 0;
		} else
			nRegionId = 0;
	} else
		nCountryId = 0;

	m_pEarth->SetCountriesWidget( m_pCountry, nCountryId );
	m_pEarth->SetRegionsWidget( m_pRegion, nCountryId, nRegionId );
	m_pEarth->SetCitiesWidget( m_pCity, nCountryId, nRegionId, nCityId );

	if( bCity ) SetCity( );

	return;
}