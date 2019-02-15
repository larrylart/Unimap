////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// local headers
#include "../config/mainconfig.h"
#include "dynamic_img_view.h"

// main header
#include "light_pollution_dlg.h"
#include "../observer/observer.h"
#include "../observer/observer_site.h"
#include "../unimap_worker.h"

// class:	CEarthmMapDlg
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CLightPollutionDlg, wxDialog )
	EVT_CHOICE( wxID_LIGHT_POLLUTION_SITE_SELECT, CLightPollutionDlg::OnChangeSite )
	EVT_CHOICE( wxID_SELECT_COUNTRY, CLightPollutionDlg::OnSelectCountry )
	EVT_CHOICE( wxID_SELECT_REGION, CLightPollutionDlg::OnSelectRegion )
	EVT_CHOICE( wxID_SELECT_CITY, CLightPollutionDlg::OnSelectCity )
	EVT_COMMAND( wxID_EVENT_UPDATE_LOCATION, wxEVT_CUSTOM_MESSAGE_UPDATE_EVENT, CLightPollutionDlg::OnUpdateLocation )
END_EVENT_TABLE()

// Constructor/Destructor
////////////////////////////////////////////////////////////////////
CLightPollutionDlg::CLightPollutionDlg( wxWindow *parent, const wxString& strTitle, CUnimapWorker* pUnimapWorker )
               : wxDialog(parent, -1, strTitle, wxDefaultPosition, wxSize( 360, 440 ), wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	// create my earth object here
	//m_pEarth = NULL;
	m_pUnimapWorker = pUnimapWorker;
	m_pEarth = new CLocationEarth( m_pUnimapWorker );

	// main dialog sizer
	wxFlexGridSizer *pTopSizer = new wxFlexGridSizer( 2, 1, 5, 5 );

	////////////////
	// SITE/LOC SIZER
	wxFlexGridSizer* sizerSiteLoc = new wxFlexGridSizer( 1, 4, 5, 10 );
	sizerSiteLoc->AddGrowableCol( 0 );
	sizerSiteLoc->AddGrowableCol( 1 );
	sizerSiteLoc->AddGrowableCol( 3 );
	pTopSizer->Add( sizerSiteLoc, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP, 10 );

	// other data sizers
	wxFlexGridSizer *dataCSizer = new wxFlexGridSizer( 1, 2, 2, 5 );
	wxFlexGridSizer *dataASizer = new wxFlexGridSizer( 1, 4, 2, 5 );
	wxFlexGridSizer *dataBSizer = new wxFlexGridSizer( 1, 6, 2, 5 );

	//////////////////////////
	// create elements
	m_pSiteSelect = new wxChoice( this, wxID_LIGHT_POLLUTION_SITE_SELECT, wxPoint(-1, -1), wxSize(160,-1), 0, NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pLightPollutionLevel = new wxStaticText( this, -1, wxT("Unknown"));
	m_pLightPollutionLevel->SetForegroundColour( *wxRED );

	m_pCountry = new wxChoice( this, wxID_SELECT_COUNTRY );
	m_pRegion = new wxChoice( this, wxID_SELECT_REGION, wxDefaultPosition, wxSize(140,-1) );
	m_pCity = new wxChoice( this, wxID_SELECT_CITY, wxDefaultPosition, wxSize(170,-1) );
	m_pLatitude = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(60,-1) );
	m_pLongitude = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(60,-1) );
	m_pAltitude = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(60,-1) );

	m_pMapView = new CEarthMapView( this, wxDefaultPosition, wxSize( 500, 300 ), 
									unMakeAppPath(wxT("data/earth/light_pollution/earth_lp_map.gif")), 0 );
	m_pMapView->m_pEarth = m_pEarth;
	pTopSizer->AddGrowableRow( 0 );
	pTopSizer->Add( m_pMapView, 0, wxGROW | wxALL|wxEXPAND, 10 );

	////////////
	// data layout
	// :: site
	sizerSiteLoc->Add( new wxStaticText( this, -1, wxT("Site:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerSiteLoc->Add( m_pSiteSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: LP Level
	sizerSiteLoc->Add( new wxStaticText( this, -1, wxT("Light Pollution Level:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerSiteLoc->Add( m_pLightPollutionLevel, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );

	// :: Country
	dataCSizer->Add( new wxStaticText( this, -1, wxT("Country:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataCSizer->Add( m_pCountry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: City
	dataASizer->Add( new wxStaticText( this, -1, wxT("Provice/State:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataASizer->Add( m_pRegion, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: City
	dataASizer->Add( new wxStaticText( this, -1, wxT("City:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataASizer->Add( m_pCity, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: latitude
	dataBSizer->Add( new wxStaticText( this, -1, wxT("Latitude:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataBSizer->Add( m_pLatitude, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: longitude
	dataBSizer->Add( new wxStaticText( this, -1, wxT("Longitude:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataBSizer->Add( m_pLongitude, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: altitude
	dataBSizer->Add( new wxStaticText( this, -1, wxT("Altitude:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	dataBSizer->Add( m_pAltitude, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );

	pTopSizer->Add( dataCSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 1 );
	pTopSizer->Add( dataASizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 1 );
	pTopSizer->Add( dataBSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 5 );

//	pTopSizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10 );

	SetSizer(pTopSizer);
	SetAutoLayout(TRUE);
	pTopSizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
CLightPollutionDlg::~CLightPollutionDlg( ) 
{
	if( m_pEarth ) delete( m_pEarth );
	
	delete( m_pSiteSelect );
	delete( m_pLightPollutionLevel );
	delete( m_pCountry );
	delete( m_pRegion );
	delete( m_pCity );
	delete( m_pLatitude );
	delete( m_pLongitude );
	delete( m_pAltitude );
	if( m_pMapView ) delete( m_pMapView );
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CLightPollutionDlg
// Purpose:	Set and get my reference from the config object
// Input:	pointer to main config
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CLightPollutionDlg::SetConfig( CObserver* pObserver, int siteid )
{
	int i=0;

	m_pObserver = pObserver;
	m_nSiteId = siteid;

	// populate sites
	m_pSiteSelect->Clear();
	for( i=0; i<m_pObserver->m_nSite; i++ ) m_pSiteSelect->Append( m_pObserver->m_vectSite[i]->m_strSiteName );
	m_pSiteSelect->SetSelection(m_nSiteId);
//	SetSite( m_nSiteId );
}

////////////////////////////////////////////////////////////////////
void CLightPollutionDlg::LoadData( )
{
	// load earth locations
	m_pEarth->LoadAllCountries();
//	SetEarth( m_pEarth );

	// load image
	m_pMapView->LoadMap();

	// set my site id
	SetSite( m_nSiteId );
}

////////////////////////////////////////////////////////////////////
void CLightPollutionDlg::SetSite( int site )
{
	wxString strTmp;

	m_pSiteSelect->SetSelection( site );

//	m_nCurrentSite = site;
	m_pSite = m_pObserver->m_vectSite[site];
//	m_pWeather = m_pSite->m_pWeather;

	// set title
	strTmp.Printf( wxT("%s/%s - Lat/Lon: %.2lf %s/%.2lf %s - Light Pollution Map"),
			m_pSite->m_strCity, m_pSite->m_strCountry, m_pSite->m_nLatitude, wxT("°N"), m_pSite->m_nLongitude, wxT("°E") );
	SetTitle( strTmp );

	SetLocByName( m_pSite->m_strCountry, m_pSite->m_strProvince, m_pSite->m_strCity, 1 );
	// set over manually - latitude/longitude and altitude
	SetCustomPosition( m_pSite->m_nLatitude, m_pSite->m_nLongitude, m_pSite->m_nAltitude );

	// set light pollution level
	double level = m_pEarth->GetLPLevelFromImage( m_pMapView->m_pImage, m_pSite->m_nLatitude, m_pSite->m_nLongitude );
	m_pEarth->GetLPLevelAsString( level, strTmp );
	m_pLightPollutionLevel->SetLabel( strTmp );
}

////////////////////////////////////////////////////////////////////
// Method:	SetCountries
// Class:	CLightPollutionDlg
// Purpose:	set countries
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
void CLightPollutionDlg::SetCountries( int nCountryId )
{
	int i = 0;

	m_pEarth->SetCountriesWidget( m_pCountry, nCountryId );
}

////////////////////////////////////////////////////////////////////
// Method:	SetRegions
// Class:	CLightPollutionDlg
// Purpose:	set regions for current country
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
void CLightPollutionDlg::SetRegions( int nSelect )
{
	int i = 0;
	int nCountryId = m_pCountry->GetSelection( );

	m_pEarth->SetRegionsWidget( m_pRegion, nCountryId, nSelect );
}

////////////////////////////////////////////////////////////////////
// Method:	SetCities
// Class:	CLightPollutionDlg
// Purpose:	set cities for current country/region
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
void CLightPollutionDlg::SetCities( int nSelect )
{
	int i = 0;
	int nCountryId = m_pCountry->GetSelection( );
	int nRegionId = m_pRegion->GetSelection( );

	m_pEarth->SetCitiesWidget( m_pCity, nCountryId, nRegionId, nSelect );
}

////////////////////////////////////////////////////////////////////
void CLightPollutionDlg::OnSelectCountry( wxCommandEvent& pEvent )
{
	SetRegions( );
	SetCities( );
	SetCity( );
	return;
}

////////////////////////////////////////////////////////////////////
void CLightPollutionDlg::OnSelectRegion( wxCommandEvent& pEvent )
{
	SetCities( );
	SetCity( );
	return;
}

////////////////////////////////////////////////////////////////////
void CLightPollutionDlg::OnSelectCity( wxCommandEvent& pEvent )
{
	SetCity( );
	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetCity
// Class:	CLightPollutionDlg
// Purpose:	set current city info
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
void CLightPollutionDlg::SetCity( )
{
	wxString strTmp;

	int nCountryId = m_pCountry->GetSelection( );
	int nRegionId = m_pRegion->GetSelection( );
	int nCityId = m_pCity->GetSelection( );

	if( nCountryId < 0 || nRegionId < 0 || nCityId < 0 ) return;

	////////////////////////
	// :: LATITUDE :: is NORTH to SOUTH 90 to -90 
	double lat = m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].lat;
	strTmp.Printf( wxT("%.6f"), lat );
	m_pLatitude->SetValue( strTmp );

	////////////////////////
	// :: LONGITUDE :: is WEST - EAST ... -180 to 180
	double lon = m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].lon;
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
	m_pMapView->m_strLocName = wxString(m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].name,wxConvUTF8);;
	m_pMapView->bIsLocSet = 1;
	// :: also set target
	m_pMapView->m_nMousePicX = m_pMapView->m_nLocX;
	m_pMapView->m_nMousePicY = m_pMapView->m_nLocY;
	m_pMapView->m_bMainTarget = 2;

	// zoom to position
/*	m_pMapView->SetZoom( m_pMapView->m_nMousePicX, m_pMapView->m_nMousePicY );
*/
	// set light pollution level
	double level = m_pEarth->GetLPLevelFromImage( m_pMapView->m_pImage, lat, lon );
	m_pEarth->GetLPLevelAsString( level, strTmp );
	m_pLightPollutionLevel->SetLabel( strTmp );

	m_pMapView->Refresh( false );

}

////////////////////////////////////////////////////////////////////
void CLightPollutionDlg::OnUpdateLocation( wxCommandEvent& pEvent )
{
	wxString strTmp;

	m_pCountry->SetSelection( m_pMapView->m_nCountryId );
	SetRegions( m_pMapView->m_nRegionId );
	SetCities( m_pMapView->m_nCityId );

	// now set target lat/log	
	// :: LATITUDE :: is NORTH to SOUTH 90 to -90 
	double lat = m_pMapView->m_nTargetLat;
	strTmp.Printf( wxT("%.6f"), lat );
	m_pLatitude->SetValue( strTmp );

	// :: LONGITUDE :: is WEST - EAST ... -180 to 180
	double lon = m_pMapView->m_nTargetLon;
	strTmp.Printf( wxT("%.6f"), lon );
	m_pLongitude->SetValue( strTmp );

	strTmp.Printf( wxT("%.4f"), m_pEarth->m_vectCountry[m_pMapView->m_nCountryId].vectRegion[m_pMapView->m_nRegionId].vectCity[m_pMapView->m_nCityId].alt );
	m_pAltitude->SetValue( strTmp );

	// zoom to position
/*	m_pMapView->SetZoom( m_pMapView->m_nMousePicX, m_pMapView->m_nMousePicY );
*/
	// set light pollution level
	double level = m_pEarth->GetLPLevelFromImage( m_pMapView->m_pImage, lat, lon );
	m_pEarth->GetLPLevelAsString( level, strTmp );
	m_pLightPollutionLevel->SetLabel( strTmp );

	m_pMapView->Refresh( false );
}

////////////////////////////////////////////////////////////////////
// Method:	SetCustomPositionFromString
// Class:	CLightPollutionDlg
// Purpose:	set custom position from strings
// Input:	lat/log/alt as wxstrings
// Output:	status
////////////////////////////////////////////////////////////////////
void CLightPollutionDlg::SetCustomPositionFromString( wxString& strLat, wxString& strLon, wxString& strAlt )
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
void CLightPollutionDlg::SetCustomPosition( double lat, double lon, double alt )
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

	// try to zoom in on location
	m_pMapView->SetZoom( m_pMapView->m_nMousePicX, m_pMapView->m_nMousePicY );
	m_pMapView->Refresh( false );
}

// Method:	SetLocByName
////////////////////////////////////////////////////////////////////
void CLightPollutionDlg::SetLocByName( wxString& strCountry, wxString& strProvince, wxString& strCity, int bCity )
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

////////////////////////////////////////////////////////////////////
void CLightPollutionDlg::OnChangeSite( wxCommandEvent& pEvent )
{
	int nId = m_pSiteSelect->GetSelection();

	SetSite( nId );

}
