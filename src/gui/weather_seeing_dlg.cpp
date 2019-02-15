////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// wx includes
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/datetime.h>

// local headers
#include "grid_data_view.h"
#include "../weather/weather.h"
#include "../weather/meteo_blue.h"
#include "../weather/clear_sky_chart.h"
#include "../observer/observer.h"
#include "../observer/observer_site.h"
#include "../unimap_worker.h"
#include "location_dialog.h"
#include "../land/geoip.h"
#include "waitdialog.h"

// main header
#include "weather_seeing_dlg.h"

BEGIN_EVENT_TABLE(CWeatherSeeingDlg, wxDialog)
	EVT_RADIOBUTTON( wxID_WEATHER_SEEING_POS_TYPE_A, CWeatherSeeingDlg::OnPosType )
	EVT_RADIOBUTTON( wxID_WEATHER_SEEING_POS_TYPE_B, CWeatherSeeingDlg::OnPosType )
	EVT_TEXT( wxID_WEATHER_SEEING_SET_LAT, CWeatherSeeingDlg::OnSetLatLon )
	EVT_TEXT( wxID_WEATHER_SEEING_SET_LON, CWeatherSeeingDlg::OnSetLatLon )
	EVT_CHOICE( wxID_WEATHER_SEEING_SITE_SELECT, CWeatherSeeingDlg::OnChangeSite )
	EVT_BUTTON( wxID_WEATHER_SEEING_BUTTON_GET_DATA, CWeatherSeeingDlg::OnGetCustomData )
	// pos custom
	EVT_BUTTON( wxID_WEATHER_SEEING_BUTTON_MAP, CWeatherSeeingDlg::OnEartMapLocation )
	EVT_BUTTON( wxID_WEATHER_SEEING_BUTTON_GEOIP, CWeatherSeeingDlg::OnGeoIpLocation )
	// navigation
	EVT_CHOICE( wxID_WEATHER_SEEING_SOURCE_SELECT, CWeatherSeeingDlg::OnChangeSource )
	EVT_BUTTON( wxID_WEATHER_SEEING_PREV_DATE, CWeatherSeeingDlg::OnChangePredDate )
	EVT_CHOICE( wxID_WEATHER_SEEING_DATE_SELECT, CWeatherSeeingDlg::OnChangePredDate )
	EVT_BUTTON( wxID_WEATHER_SEEING_NEXT_DATE, CWeatherSeeingDlg::OnChangePredDate )
END_EVENT_TABLE()
// Method:	Constructor/Destructor
////////////////////////////////////////////////////////////////////
CWeatherSeeingDlg::CWeatherSeeingDlg( wxWindow *parent, const wxString& title ) : wxDialog(parent, -1,
							title, wxDefaultPosition, wxDefaultSize,
							wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	m_pCustomWeather = NULL;
	m_pWaitDlg = NULL;

	wxString vectSources[3];
	vectSources[0] = wxT("Meteo Blue");
	vectSources[1] = wxT("Clear Sky");

	// top sizer
	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	/////////////////////////////
	// main sizer
	wxFlexGridSizer* mainSizer = new wxFlexGridSizer( 3, 1, 5, 5 );
	mainSizer->AddGrowableRow( 1 );
	mainSizer->AddGrowableCol( 0 );
	////////////////
	// SITE/LOC SIZER
	wxFlexGridSizer* sizerPosData = new wxFlexGridSizer( 2, 2, 5, 10 );
	sizerPosData->AddGrowableCol( 0 );
	sizerPosData->AddGrowableCol( 1 );
	mainSizer->Add( sizerPosData, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );
	/////////////////
	// Navigation box sizer
//	wxStaticBox* pNavBox = new wxStaticBox( this, -1, "Source/Date Selector", wxPoint(-1,-1), wxDefaultSize );
//	wxStaticBoxSizer* sizerNavBox = new wxStaticBoxSizer( pNavBox, wxHORIZONTAL );
//	mainSizer->Add( sizerNavBox, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );
	// data sizer
//	wxFlexGridSizer* sizerNavData = new wxFlexGridSizer( 1, 5, 0, 10 );
	//sizerNavData->SetFlexibleDirection( wxVERTICAL );
//	sizerNavData->AddGrowableCol( 0 );
//	sizerNavData->AddGrowableCol( 1 );
//	sizerNavData->AddGrowableCol( 4 );
//	mainSizer->Add( sizerNavData, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );
	////////////////////
	// Data view sizer
	wxStaticBox* pDataBox = new wxStaticBox( this, -1, wxT("Data View"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerDataBox = new wxStaticBoxSizer( pDataBox, wxHORIZONTAL );
	mainSizer->Add( sizerDataBox, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );

	/////////////////////////
	// GUI ELEMNTS INIT
	// :: navigation controls
	m_pPosTypeA = new wxRadioButton( this,  wxID_WEATHER_SEEING_POS_TYPE_A, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_pSiteLabel = new wxStaticText( this, -1, wxT("Site:"));
	m_pSiteSelect = new wxChoice( this, wxID_WEATHER_SEEING_SITE_SELECT, wxPoint(-1, -1), wxSize(160,-1), 0, NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: pos type addr
	m_pPosTypeB = new wxRadioButton( this,  wxID_WEATHER_SEEING_POS_TYPE_B, _T("") );
	m_pLatLonLabel = new wxStaticText( this, -1, wxT("Lat(°N)/Lon(°E):"));
	m_pLatValue = new wxTextCtrl( this, wxID_WEATHER_SEEING_SET_LAT, wxT(""), wxDefaultPosition, wxSize(50,-1) );
	m_pLonValue = new wxTextCtrl( this, wxID_WEATHER_SEEING_SET_LON, wxT(""), wxDefaultPosition, wxSize(50,-1) );
	m_pMapButton = new wxButton( this, wxID_WEATHER_SEEING_BUTTON_MAP, wxT("Map"), wxPoint(-1, -1), wxSize(30,-1) );
	m_pGeoIpButton = new wxButton( this, wxID_WEATHER_SEEING_BUTTON_GEOIP, wxT("GeoIp"), wxPoint(-1, -1), wxSize(40,-1) );
	m_pGpsButton = new wxButton( this, wxID_WEATHER_SEEING_BUTTON_GPS, wxT("Gps"), wxPoint(-1, -1), wxSize(30,-1) );
	m_pGetDataButton = new wxButton( this, wxID_WEATHER_SEEING_BUTTON_GET_DATA, wxT("Get Data"), wxPoint(-1, -1), wxSize(55,-1) );
	m_pGpsButton->Disable();
	// head line 2
	m_pSourceSelect = new wxChoice( this, wxID_WEATHER_SEEING_SOURCE_SELECT, wxPoint(-1, -1), wxSize(90,-1), 2, vectSources, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSourceSelect->SetSelection( 0 );
	m_pPrevDateButton = new wxButton( this, wxID_WEATHER_SEEING_PREV_DATE, wxT("<<< Back"), wxPoint(-1, -1), wxSize(70,-1) );
	m_pDateSelect = new wxChoice( this, wxID_WEATHER_SEEING_DATE_SELECT, wxPoint(-1, -1), wxSize(150,-1), 0, NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pDateSelect->SetSelection( 0 );
	m_pNextDateButton = new wxButton( this, wxID_WEATHER_SEEING_NEXT_DATE, wxT("Next >>>"), wxPoint(-1, -1), wxSize(70,-1) );
	// source rights
	m_pSourceRights = new wxStaticText( this, -1, wxT("- Data Source Rights -"));

	////////////////
	// new grid view
	m_pDataView = new CGridDataView( this, wxDefaultPosition, wxSize( 720, 210 ) );

//	m_pDataView->InitGrid( 24, 11, 0, 100 );
	// init as default to meteo blue
//	InitMeteoBlueDataTable( );

	//////////////////////////
	// LAYOUT
	// :: site select
	wxFlexGridSizer* sizerPosAData = new wxFlexGridSizer( 1, 3, 0, 5 );
	sizerPosAData->Add( m_pPosTypeA, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerPosAData->Add( m_pSiteLabel, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerPosAData->Add( m_pSiteSelect, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerPosData->Add( sizerPosAData, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL  );
	// :: coord select
	wxFlexGridSizer* sizerPosBData = new wxFlexGridSizer( 1, 8, 0, 5 );
	sizerPosBData->Add( m_pPosTypeB, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerPosBData->Add( m_pLatLonLabel, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerPosBData->Add( m_pLatValue, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerPosBData->Add( m_pLonValue, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	// buttons
	sizerPosBData->Add( m_pMapButton, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerPosBData->Add( m_pGeoIpButton, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerPosBData->Add( m_pGpsButton, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerPosBData->Add( m_pGetDataButton, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	sizerPosData->Add( sizerPosBData, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL  );
	
	// :: source
	wxFlexGridSizer* sizerNavAData = new wxFlexGridSizer( 1, 2, 0, 5 );
	sizerNavAData->Add( new wxStaticText( this, -1, wxT("Source:")), 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL  );
	sizerNavAData->Add( m_pSourceSelect, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerPosData->Add( sizerNavAData, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL  );
	// :: nav buttons
	wxFlexGridSizer* sizerNavBData = new wxFlexGridSizer( 1, 3, 0, 5 );
	sizerNavBData->Add( m_pPrevDateButton, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	sizerNavBData->Add( m_pDateSelect, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
	sizerNavBData->Add( m_pNextDateButton, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerPosData->Add( sizerNavBData, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL  );

	// :: seeing data view table
	sizerDataBox->Add( m_pDataView, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );

	// :: data source rights
	mainSizer->Add( m_pSourceRights, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxTOP, 3 );

	// add main sizer
	topsizer->Add( mainSizer, 1, wxGROW|wxALL, 5 );
	// add buttons ok
//	topsizer->Add( CreateButtonSizer(wxOK), 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxALL, 10 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
	Layout();
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
CWeatherSeeingDlg::~CWeatherSeeingDlg( )
{
	if( m_pCustomWeather ) delete( m_pCustomWeather );
	if( m_pWaitDlg ) delete( m_pWaitDlg );

	delete( m_pPosTypeA );
	delete( m_pPosTypeB );
	delete( m_pLatLonLabel );
	delete( m_pLatValue );
	delete( m_pLonValue );
	delete( m_pMapButton );
	delete( m_pGeoIpButton );
	delete( m_pGpsButton );
	delete( m_pGetDataButton );
	delete( m_pSiteLabel );
	delete( m_pSiteSelect );
	delete( m_pSourceSelect );
	delete( m_pPrevDateButton );
	delete( m_pNextDateButton );
	delete( m_pDateSelect );
	delete( m_pDataView );
	delete( m_pSourceRights );
}

////////////////////////////////////////////////////////////////////
void CWeatherSeeingDlg::SetConfig( CObserver* pObserver, int siteid, int engine, CUnimapWorker* pUnimapWorker )
{
	int i=0;
	m_pObserver = pObserver;
	m_pUnimapWorker = pUnimapWorker;
	m_nCurrentEngine = engine;
	m_nPosType = 0;

	// populate sites
	m_pSiteSelect->Clear();
	for( i=0; i<m_pObserver->m_nSite; i++ ) m_pSiteSelect->Append( m_pObserver->m_vectSite[i]->m_strSiteName );
	
	// init/set by engine
	SetSite( siteid );

	// set default position
	SetPosType( 0 );
}

////////////////////////////////////////////////////////////////////
void CWeatherSeeingDlg::SetSite( int site )
{
	wxString strTmp;

	m_pSiteSelect->SetSelection( site );

	m_nCurrentSite = site;
	m_pSite = m_pObserver->m_vectSite[m_nCurrentSite];
	m_pWeather = m_pSite->m_pWeather;

	// set title
	strTmp.Printf( wxT("%s/%s - Lat/Lon: %.2lf %s/%.2lf %s - Seeing Prediction"),
			m_pSite->m_strCity, m_pSite->m_strCountry, m_pSite->m_nLatitude, wxT("°N"), m_pSite->m_nLongitude, wxT("°E") );
	SetTitle( strTmp );
	// set custom fields
	strTmp.Printf( wxT("%.4lf"), m_pSite->m_nLatitude );
	m_pLatValue->ChangeValue( strTmp );
	strTmp.Printf( wxT("%.4lf"), m_pSite->m_nLongitude );
	m_pLonValue->ChangeValue( strTmp );

	// set local lat/lon
	m_nLat = m_pSite->m_nLatitude;
	m_nLon = m_pSite->m_nLongitude;

	// init/set by engine
	SetSource( -1 );
}

////////////////////////////////////////////////////////////////////
void CWeatherSeeingDlg::InitMeteoBlueDataTable( )
{
	int i=0;
	wxString strTmp=wxT("");

	wxColor col_col(255,255,255);

	// set col label values
	for( i=0; i<24; i++ )
	{
		strTmp.Printf( wxT("%.2d"), i );
		m_pDataView->SetColLabel( i, strTmp, col_col );
	}
	// set row values
	m_pDataView->SetRowLabel( 0, wxT("High Clouds (%)"), col_col );
	m_pDataView->SetRowLabel( 1, wxT("Mid Clouds (%)"), col_col );
	m_pDataView->SetRowLabel( 2, wxT("Low Clouds (%)"), col_col );
	m_pDataView->SetRowLabel( 3, wxT("Seeing Index 1"), col_col );
	m_pDataView->SetRowLabel( 4, wxT("Seeing Index 2"), col_col );
	m_pDataView->SetRowLabel( 5, wxT("Seeing arcSec"), col_col );
	m_pDataView->SetRowLabel( 6, wxT("Jet stream (m/s)"), col_col  );
	m_pDataView->SetRowLabel( 7, wxT("2m Rel. Hum. (%)"), col_col );
	m_pDataView->SetRowLabel( 8, wxT("Bad Layers Top (km)"), col_col );
	m_pDataView->SetRowLabel( 9, wxT("Bad Layers Bot (km)"), col_col  );
	m_pDataView->SetRowLabel( 10, wxT("Bad Layers K/100m"), col_col );
	
	// set select values
	SetSelectDateRange( );
	// set data
	SetPredictionData( 0 );

	// set source rights
	m_pSourceRights->SetLabel( wxT("Created with data from: meteoblue - numerical weather prediction") );
}

////////////////////////////////////////////////////////////////////
void CWeatherSeeingDlg::SetSelectDateRange( )
{
	CWeather* pWeather=NULL;

	// get by pos type
	if( m_nPosType == 0 )
		pWeather = m_pSite->m_pWeather;
	else if( m_nPosType == 1 )
		pWeather = m_pCustomWeather;

	// add select date
	int i=0;
	m_pDateSelect->Clear();
	for( i=0; i<pWeather->m_nSeeingPredict; i++ )
	{
		wxDateTime date( pWeather->m_vectSeeingPredict[i].date );
		m_pDateSelect->Append( date.Format( wxT("%A, %d/%m/%Y") ) );
	}
	m_pDateSelect->SetSelection( 0 );
}

////////////////////////////////////////////////////////////////////
void CWeatherSeeingDlg::SetPredictionData( int nDayId )
{
	CWeather* pWeather=NULL;

	// get by pos type
	if( m_nPosType == 0 )
		pWeather = m_pSite->m_pWeather;
	else if( m_nPosType == 1 )
		pWeather = m_pCustomWeather;

	int i=0;

	// activate deactivate buttons
	if( pWeather->m_nSeeingPredict < 2 )
	{
		m_pPrevDateButton->Disable();
		m_pNextDateButton->Disable();

	} else if( nDayId == 0 )
	{
		m_pPrevDateButton->Disable();
		m_pNextDateButton->Enable();

	} else if( nDayId == pWeather->m_nSeeingPredict-1 )
	{
		m_pNextDateButton->Disable();
		m_pPrevDateButton->Enable();

	} else
	{
		m_pPrevDateButton->Enable();
		m_pNextDateButton->Enable();

	}

	// populate
	unsigned char red=0, green=0, blue=0;
	wxString str=wxT("");
	for( i=0; i<24; i++ )
	{
		for( int r=0; r<11; r++ )
		{
			// :: high clouds
			red = pWeather->m_vectSeeingPredict[nDayId].hour[i].red[r];
			green = pWeather->m_vectSeeingPredict[nDayId].hour[i].green[r];
			blue = pWeather->m_vectSeeingPredict[nDayId].hour[i].blue[r];

			// set by type
			if( r == 0 )
				str.Printf( wxT("%d"), pWeather->m_vectSeeingPredict[nDayId].hour[i].high_clouds );
			else if( r == 1 )
				str.Printf( wxT("%d"), pWeather->m_vectSeeingPredict[nDayId].hour[i].mid_clouds );
			else if( r == 2 )
				str.Printf( wxT("%d"), pWeather->m_vectSeeingPredict[nDayId].hour[i].low_clouds );
			else if( r == 3 )
				str.Printf( wxT("%d"), pWeather->m_vectSeeingPredict[nDayId].hour[i].seeing_id_1 );
			else if( r == 4 )
				str.Printf( wxT("%d"), pWeather->m_vectSeeingPredict[nDayId].hour[i].seeing_id_2 );
			else if( r == 5 )
				str.Printf( wxT("%.2lf"), pWeather->m_vectSeeingPredict[nDayId].hour[i].seeing_arcsec );
			else if( r == 6 )
				str.Printf( wxT("%.1lf"), pWeather->m_vectSeeingPredict[nDayId].hour[i].jet_stream );
			else if( r == 7 )
				str.Printf( wxT("%d"), pWeather->m_vectSeeingPredict[nDayId].hour[i].rel_hum_2m );
			else if( r == 8 )
				str.Printf( wxT("%.2lf"), pWeather->m_vectSeeingPredict[nDayId].hour[i].bad_layers_top );
			else if( r == 9 )
				str.Printf( wxT("%.2lf"), pWeather->m_vectSeeingPredict[nDayId].hour[i].bad_layers_bot );
			else if( r == 10 )
				str.Printf( wxT("%.2lf"), pWeather->m_vectSeeingPredict[nDayId].hour[i].bad_layers_k );

			// set cell text
			m_pDataView->SetCellLabel( r, i, str, wxColor(red,green,blue) );
		}
	}

	Refresh( false );
}

////////////////////////////////////////////////////////////////////
void CWeatherSeeingDlg::OnChangePredDate( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	int nDayId = m_pDateSelect->GetSelection();
	int nItems = m_pDateSelect->GetCount();

	if( nId == wxID_WEATHER_SEEING_NEXT_DATE && nDayId < nItems-1 )
	{
		nDayId++;
		m_pDateSelect->SetSelection( nDayId );

	} else if( nId == wxID_WEATHER_SEEING_PREV_DATE && nDayId > 0  )
	{
		nDayId--;
		m_pDateSelect->SetSelection( nDayId );
	}

	// set dat for current selection
	SetPredictionData( nDayId );
}

////////////////////////////////////////////////////////////////////
void CWeatherSeeingDlg::OnPosType( wxCommandEvent& pEvent )
{
	int nType = pEvent.GetId();

	if( nType == wxID_WEATHER_SEEING_POS_TYPE_A )
		SetPosType( 0 );
	else if( nType == wxID_WEATHER_SEEING_POS_TYPE_B )
		SetPosType( 1 );

}

////////////////////////////////////////////////////////////////////
void CWeatherSeeingDlg::SetPosType( int type )
{
	if( type == 0 )
	{
		m_nPosType = 0;

		// set current lat/lon
		m_nLat = m_pSite->m_nLatitude;
		m_nLon = m_pSite->m_nLongitude;

		// enable 
		//m_pPosTypeA->Enable();
		m_pSiteLabel->Enable();
		m_pSiteSelect->Enable();
		// disable
		//m_pPosTypeB->Disable();
		m_pLatLonLabel->Disable();
		m_pLatValue->Disable();
		m_pLonValue->Disable();
		m_pMapButton->Disable();
		m_pGeoIpButton->Disable();
		m_pGpsButton->Disable();
		m_pGetDataButton->Disable();

	} else
	{
		m_nPosType = 1;
		if( !m_pCustomWeather ) m_pCustomWeather = new CWeather();

		// set current lat/lon from text entries
		m_nLat = wxAtof( m_pLatValue->GetLineText(0) );
		m_nLon = wxAtof( m_pLonValue->GetLineText(0) );

		// enable
		//m_pPosTypeB->Enable();
		m_pLatLonLabel->Enable();
		m_pLatValue->Enable();
		m_pLonValue->Enable();
		m_pMapButton->Enable();
		m_pGeoIpButton->Enable();
		//m_pGpsButton->Enable();
		m_pGetDataButton->Enable();
		//disable
		//m_pPosTypeA->Disable();
		m_pSiteLabel->Disable();
		m_pSiteSelect->Disable();
	}
}

////////////////////////////////////////////////////////////////////
void CWeatherSeeingDlg::OnSetLatLon( wxCommandEvent& pEvent )
{
	wxString strTmp;

	// set current lat/lon from text entries
	wxString strLat = m_pLatValue->GetLineText(0);
	m_nLat = wxAtof( strLat );
	wxString strLon = m_pLonValue->GetLineText(0);
	m_nLon = wxAtof( strLon );
	// enable/disable data button
	if( strLat.IsEmpty() || strLon.IsEmpty() )
		m_pGetDataButton->Disable();
	else
		m_pGetDataButton->Enable();

	strTmp.Printf( wxT("Lat/Lon: %.2lf %s/%.2lf %s - Seeing Prediction"), m_nLat, wxT("°N"), m_nLon, wxT("°E") );
	SetTitle( strTmp );
}

////////////////////////////////////////////////////////////////////
void CWeatherSeeingDlg::OnChangeSite( wxCommandEvent& pEvent )
{
	int nId = m_pSiteSelect->GetSelection();

	SetSite( nId );
}

////////////////////////////////////////////////////////////////////
void CWeatherSeeingDlg::OnGetCustomData( wxCommandEvent& pEvent )
{
	// set current lat/lon from text entries
	m_nLat = wxAtof( m_pLatValue->GetLineText(0) );
	m_nLon = wxAtof( m_pLonValue->GetLineText(0) );

	// init/set by engine
	SetSource( -1, 1 );
}

////////////////////////////////////////////////////////////////////
void CWeatherSeeingDlg::OnEartMapLocation( wxCommandEvent& pEvent )
{
	wxString strTmp;

	CLocationEarth* pEarth = new CLocationEarth( NULL );
//	pEarth->LoadAllCountries( );

	CEarthmMapDlg* pMapDlg = new CEarthmMapDlg( this, _("Earth Map - Set Your Location") );
	pMapDlg->SetEarth( pEarth );
	pMapDlg->LoadData( this, m_pUnimapWorker );
	//pMapDlg->SetConfig( NULL, nCountryId, nRegionId, nCityId );
	// set address location
	pMapDlg->SetLocByName( m_pSite->m_strCountry, m_pSite->m_strProvince, m_pSite->m_strCity, 1 );

	// set over manually - latitude/longitude and altitude
	wxString strAlt = wxT("0.0");
	wxString strLat = m_pLatValue->GetLineText(0);
	wxString strLon = m_pLonValue->GetLineText(0);
	pMapDlg->SetCustomPositionFromString( strLat, strLon, strAlt );

	if( pMapDlg->ShowModal() == wxID_OK )
	{
		// also get/set latitude/longitude and altitude
		wxString strLat = pMapDlg->m_pLatitude->GetLineText(0);
		m_pLatValue->SetValue( strLat );
		wxString strLon = pMapDlg->m_pLongitude->GetLineText(0);
		m_pLonValue->SetValue( strLon );

		// set current lat/lon
		m_nLat = wxAtof( strLat );
		m_nLon = wxAtof( strLon );

		// set title
		strTmp.Printf( wxT("%s/%s - Lat/Lon: %.2lf %s/%.2lf %s - Seeing Prediction"),
			pMapDlg->m_pCity->GetStringSelection(), pMapDlg->m_pCountry->GetStringSelection(), m_nLat, wxT("°N"), m_nLon, wxT("°E") );
		SetTitle( strTmp );

	}
	delete( pMapDlg );
	delete( pEarth );
}

////////////////////////////////////////////////////////////////////
void CWeatherSeeingDlg::OnGeoIpLocation( wxCommandEvent& pEvent )
{
	wxString strCountry;
	wxString strCity;
	double nLat, nLon;
	wxString strTmp;

	CGeoIp ip_locator;
	ip_locator.GetOwnIp( strCountry, strCity, nLat, nLon );

	// :: set latitude :: is NORTH to SOUTH 90 to -90 
	strTmp.Printf( wxT("%.4f"), nLat );
	m_pLatValue->SetValue( strTmp );

	// :: set longitude :: is WEST - EAST ... -180 to 180
	strTmp.Printf( wxT("%.4f"), nLon );
	m_pLonValue->SetValue( strTmp );

	// set current lat/lon
	m_nLat = nLat;
	m_nLon = nLon;

	// set title
	strTmp.Printf( wxT("%s/%s - Lat/Lon: %.2lf %s/%.2lf %s - Seeing Prediction"), 
								strCity, strCountry, m_nLat, wxT("°N"), m_nLon, wxT("°E") );
	SetTitle( strTmp );

}


////////////////////////////////////////////////////////////////////
void CWeatherSeeingDlg::OnChangeSource( wxCommandEvent& pEvent )
{
	int nId = m_pSourceSelect->GetSelection();

	SetSource( nId );
}

////////////////////////////////////////////////////////////////////
void CWeatherSeeingDlg::SetSource( int source, int type )
{
	wxString strTmp;

	////////////////////////////////////
	// check if to do the update
	// :: Get best seeing prediction for this site
	int best_engine = m_pWeather->GetBestSeeingPredictEngine( m_nLat, m_nLon );
	// treat exception = 1 (clear sky doesn't do outside it's limits)
	if( source == 1 && best_engine == 0 )
	{
		strTmp.Printf( wxT("Clear Sky Chart not available for lat/lon: %.4lf/%.4lf"), m_nLat, m_nLon );
		wxMessageBox( strTmp, wxT("Data not available"), wxOK, this );
		m_pSourceSelect->SetSelection( 0 );
		m_nCurrentEngine = 0;
		return;
	}
	// if -1 has been passed
	if( source == -1 )
		m_nCurrentEngine = best_engine;
	else
		m_nCurrentEngine = source;
	// set selection of engine
	m_pSourceSelect->SetSelection( m_nCurrentEngine );

	///////////////
	// check if weather for current site is to update
	if( m_pWeather->IsSeeing_ToUpdate( m_nLat, m_nLon, m_nCurrentEngine ) )
	{
		///////////
		// prepare action command
		DefCmdAction act;

		// by type
		if( type == 0 )
		{
			act.id = THREAD_ACTION_WEATHER_SEEING_PREDICT_GET;
			// site id
			act.vectInt[0] = m_nCurrentSite;
			// engine id
			act.vectInt[1] = m_nCurrentEngine;

		} else if( type == 1 )
		{
			act.id = THREAD_ACTION_WEATHER_SEEING_PREDICT_GET_CUSTOM;
			// lat/lon
			act.vectFloat[0] = m_nLat;
			act.vectFloat[1] = m_nLon;
			// engine id
			act.vectInt[0] = m_nCurrentEngine;

		}

		/////////////////
		// show wait dialog
		if( m_pWaitDlg ) delete( m_pWaitDlg );
		m_pWaitDlg = new CWaitDialog( this, wxT("Fetch Seeing Prediction Data"),
								wxT("Updating weather seeing prediction data..."), 0, m_pUnimapWorker, &act );
		m_pWaitDlg->ShowModal();
		delete( m_pWaitDlg );
		m_pWaitDlg = NULL;

		// now if type 1 get what was loaded from local
		if( type == 1 ) 
		{
			m_pCustomWeather->GetSeeingPrediction( m_nLat, m_nLon, m_nCurrentEngine );
		}

	} else
	{
		// call to get current seeing prediction - local cache
		m_pSite->GetSeeingPrediction( m_nCurrentEngine );
	}

	// :: if meteoblue
	if( m_nCurrentEngine == 0 )
	{
		// enable/clear buttons/selector
		m_pPrevDateButton->Enable();
		m_pNextDateButton->Enable();
		m_pDateSelect->Clear();
		m_pDateSelect->Enable();

		// init my grid
		m_pDataView->InitGrid( 24, 11, 0, 100 );
		// init as default to meteo blue
		InitMeteoBlueDataTable( );

	// :: if clear sky chart
	} else if( m_nCurrentEngine == 1 )
	{
		// test :: local call 
//		CClearSkyChart src;
//		src.GetPredictionData( NULL, 45.0, 85.0 ); 

		// call to get current seeing prediction - local cache
//		m_pSite->GetSeeingPrediction( 1 );

		// draw on the grid brute image
		if( type == 0 ) 
			m_pDataView->SetImageFromFile( m_pSite->m_pWeather->m_strSeeingPredictImg );
		else if( type == 1 ) 
			m_pDataView->SetImageFromFile( m_pCustomWeather->m_strSeeingPredictImg );

		// disable/clear buttons/selector
		m_pPrevDateButton->Disable();
		m_pNextDateButton->Disable();
		m_pDateSelect->Clear();
		m_pDateSelect->Disable();
		// set source rights
		m_pSourceRights->SetLabel( wxT("© A. Danco - Created with data from: Environment Canada") );

	}
}
