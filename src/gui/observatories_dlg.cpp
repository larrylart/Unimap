////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// wx
#include <wx/utils.h>

// local headers
#include "../util/folders.h"
#include "../config/mainconfig.h"
#include "dynamic_img_view.h"
#include "../observer/observatories.h"
#include "img_view_dlg.h"

// main header
#include "observatories_dlg.h"

// class:	CEarthmMapDlg
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CObservatoriesDlg, wxDialog )
//	EVT_CHOICE( wxID_OBS_SELECT_COUNTRY, CObservatoriesDlg::OnSelectCountry )
	EVT_BUTTON( wxID_OBS_CONTACT_BUTTON, CObservatoriesDlg::OnContact )
	EVT_BUTTON( wxID_OBS_INFO_WEBSITE_BUTTON, CObservatoriesDlg::OnViewWebSite )
	EVT_BUTTON( wxID_OBS_INFO_WEBREF_BUTTON, CObservatoriesDlg::OnViewWebRef )
	EVT_BUTTON( wxID_OBS_PHOTO_NEXT, CObservatoriesDlg::OnSetPhoto )
	EVT_BUTTON( wxID_OBS_PHOTO_BACK, CObservatoriesDlg::OnSetPhoto )
	EVT_BUTTON( wxID_OBS_PHOTO_VIEW, CObservatoriesDlg::OnPhotoViewFull )
	EVT_COMMAND( wxID_EVENT_UPDATE_OBS_LOCATION, wxEVT_CUSTOM_MESSAGE_UPDATE_EVENT, CObservatoriesDlg::OnUpdateObsLocation )
	EVT_COMMAND( wxID_EVENT_UPDATE_LOCATION, wxEVT_CUSTOM_MESSAGE_UPDATE_EVENT, CObservatoriesDlg::OnUpdateLocation )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CObservatoriesDlg
// Purpose:	Initialize my eart location dialog
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CObservatoriesDlg::CObservatoriesDlg( wxWindow *parent, const wxString& strTitle )
               : wxDialog(parent, -1, strTitle, wxDefaultPosition, wxSize( 360, 440 ), wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL|wxRESIZE_BORDER )
{
	m_pObservatories = NULL;
	// create my earth object here
//	m_pEarth = new CLocationEarth();

	// predefined selects
	wxString strSelectKind[3];
	strSelectKind[0] = wxT("Professional");
	strSelectKind[1] = wxT("Amateur");
	strSelectKind[2] = wxT("All");
	// type
	wxString strSelectType[12];
	strSelectType[0] = wxT("All");
	strSelectType[1] = wxT("Ground based observatory");
	strSelectType[2] = wxT("Solar observatory");
	strSelectType[3] = wxT("Radio observatory");
	strSelectType[4] = wxT("Microwave observatory");
	strSelectType[5] = wxT("Cosmic-ray observatory");
	strSelectType[6] = wxT("Neutrino observatory");
	strSelectType[7] = wxT("Gravitational observatory");
	strSelectType[8] = wxT("Antimatter observatory");
	strSelectType[9] = wxT("Airborne observatory");
	strSelectType[10] = wxT("Space observatory");

	// top dialog sizer
	wxFlexGridSizer *pTopSizer = new wxFlexGridSizer( 2, 1, 5, 5 );
	pTopSizer->AddGrowableRow( 1 );
	pTopSizer->AddGrowableCol( 0 );

	/////////////
	// filter sizer
	wxFlexGridSizer *sizerFilter = new wxFlexGridSizer( 1, 6, 5, 5 );
	sizerFilter->AddGrowableCol( 0 );
//	sizerFilter->AddGrowableCol( 2 );
	sizerFilter->AddGrowableCol( 3 );
	pTopSizer->Add( sizerFilter, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	//////////////
	// main sizer
	wxFlexGridSizer *pMainSizer = new wxFlexGridSizer( 1, 2, 5, 5 );
	pMainSizer->AddGrowableCol( 0 );
	pMainSizer->AddGrowableRow( 0 );
	pTopSizer->Add( pMainSizer, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

	////////////////
	// main A sizer
	wxFlexGridSizer *pMainASizer = new wxFlexGridSizer( 3, 1, 5, 5 );
	pMainASizer->AddGrowableRow( 0 );
	pMainASizer->AddGrowableCol( 0 );
	pMainSizer->Add( pMainASizer, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );

	////////////////////
	// Map view sizer
	wxStaticBox* pMapBox = new wxStaticBox( this, -1, wxT("Map View"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerMapBox = new wxStaticBoxSizer( pMapBox, wxHORIZONTAL );
	pMainASizer->Add( sizerMapBox, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );

	////////////////////
	// Web Act Sizer
	wxFlexGridSizer *pActSizer = new wxFlexGridSizer( 1, 5, 5, 5 );
	//pActSizer->AddGrowableCol( 0 );
	//pActSizer->AddGrowableCol( 1 );
	pActSizer->AddGrowableCol( 2 );
	pMainASizer->Add( pActSizer, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );

	////////////////////
	// Decription view sizer
	wxStaticBox* pDescBox = new wxStaticBox( this, -1, wxT("Description"), wxPoint(-1,70), wxDefaultSize );
	wxStaticBoxSizer* sizerDescBox = new wxStaticBoxSizer( pDescBox, wxHORIZONTAL );
	pMainASizer->Add( sizerDescBox, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );

	////////////////
	// main B sizer
	wxFlexGridSizer *pMainBSizer = new wxFlexGridSizer( 3, 1, 5, 5 );
	pMainBSizer->AddGrowableRow( 1 );
	pMainSizer->Add( pMainBSizer, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );

	////////////////////
	// Photo view sizer
	wxStaticBox* pPhotoBox = new wxStaticBox( this, -1, wxT("Photo"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerPhotoBox = new wxStaticBoxSizer( pPhotoBox, wxHORIZONTAL );
	pMainBSizer->Add( sizerPhotoBox, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );
	// photo data
	wxFlexGridSizer *sizerPhotoData = new wxFlexGridSizer( 3, 1, 0, 0 );
	pMainBSizer->AddGrowableRow( 1 );
	pMainBSizer->AddGrowableCol( 0 );
	sizerPhotoBox->Add( sizerPhotoData, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );
	// photo Buttons
	wxFlexGridSizer *sizerPhotoButtons = new wxFlexGridSizer( 1, 3, 0, 0 );
	sizerPhotoButtons->AddGrowableCol( 0 );
	sizerPhotoButtons->AddGrowableCol( 2 );
	sizerPhotoData->Add( sizerPhotoButtons, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxBOTTOM, 3 );

	////////////////////
	// Instruments sizer
	wxStaticBox* pInstBox = new wxStaticBox( this, -1, wxT("Instruments"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerInstBox = new wxStaticBoxSizer( pInstBox, wxHORIZONTAL );
	pMainBSizer->Add( sizerInstBox, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );

	////////////////////
	// Info sizer
	wxStaticBox* pInfoBox = new wxStaticBox( this, -1, wxT("Info"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerInfoBox = new wxStaticBoxSizer( pInfoBox, wxVERTICAL );
	pMainBSizer->Add( sizerInfoBox, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );
	// data sizer
	wxFlexGridSizer *sizerInfoData = new wxFlexGridSizer( 4, 2, 2, 2 );
	sizerInfoBox->Add( sizerInfoData, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );

	////////////////////
	// GUI ELEMENTS
	// :: kind
	m_pFilterKind = new wxChoice( this, wxID_OBS_FILTER_SELECT_KIND, wxPoint(-1, -1), wxSize(90,-1), 
									3, strSelectKind, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pFilterKind->SetSelection( 0 );
	// :: type
	m_pFilterType = new wxChoice( this, wxID_OBS_FILTER_SELECT_TYPE, wxPoint(-1, -1), wxSize(170,-1), 
									11, strSelectType, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pFilterType->SetSelection( 0 );
	// :: find
	m_pFindEntry = new wxTextCtrl( this, wxID_OBS_FILTER_FIND_ENTRY, wxT(""), wxDefaultPosition, wxSize(150,-1) );
	m_pFindButton = new wxButton( this, wxID_OBS_FILTER_FIND_BUTTON, wxT("Find"), wxPoint(-1, -1), wxSize(40,-1) );
	// :: map
	m_pMapView = new CEarthMapView( this, wxDefaultPosition, wxSize( 500, 250 ) );
	m_pMapView->m_pEarth = m_pEarth;
	m_pMapView->m_nLDClickType = 1;
	// :: details
	m_pDescText = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(-1,150), wxTE_MULTILINE|wxTE_RICH|wxTE_RICH2|wxTE_READONLY );

	// photo view
	wxBitmap bmpPreview( 250, 200 ); //( "./resources/nophoto_s.gif", wxBITMAP_TYPE_ANY );
	m_pPhoto = new wxStaticBitmap( this, -1, bmpPreview, wxDefaultPosition, wxSize( 250, 200 ) );
	m_pPhotoText = new wxStaticText( this, -1, wxT("- photo details -"));
	// photo buttons
	m_pPhotoBack = new wxButton( this, wxID_OBS_PHOTO_BACK, wxT("<< Back"), wxPoint(-1, -1), wxSize(60,17) );
	m_pPhotoNext = new wxButton( this, wxID_OBS_PHOTO_NEXT, wxT("Next >>"), wxPoint(-1, -1), wxSize(60,17) );
	m_pPhotoView = new wxButton( this, wxID_OBS_PHOTO_VIEW, wxT("View"), wxPoint(-1, -1), wxSize(40,17) );

	// instrumnets
	m_pInstruments = new wxListBox( this, -1, wxDefaultPosition, wxSize(-1,80), 0, NULL, wxLB_HSCROLL|wxLB_SINGLE|wxLB_HSCROLL|wxLB_ALWAYS_SB );

	// info :: Organization
	m_pOrgLabel = new wxStaticText( this, -1, wxT("Organization:"));
	m_pOrgValue = new wxStaticText( this, -1, wxT("none"), wxDefaultPosition, wxDefaultSize );
	m_pOrgValue->SetForegroundColour( *wxRED );
	// info :: Code
	m_pCodeLabel = new wxStaticText( this, -1, wxT("Code:"));
	m_pCodeValue = new wxStaticText( this, -1, wxT("none"));
	m_pCodeValue->SetForegroundColour( *wxRED );
	// info :: Weather
	m_pWeatherLabel = new wxStaticText( this, -1, wxT("Weather:"));
	m_pWeatherValue = new wxStaticText( this, -1, wxT("none"));
	m_pWeatherValue->SetForegroundColour( *wxRED );
	// info :: Built
	m_pBuiltLabel = new wxStaticText( this, -1, wxT("Built:"));
	m_pBuiltValue = new wxStaticText( this, -1, wxT("none"));
	m_pBuiltValue->SetForegroundColour( *wxRED );

	// info :: contact
	m_pContactButton = new wxButton( this, wxID_OBS_CONTACT_BUTTON, wxT("Contact"), wxPoint(-1, -1), wxSize(60,-1) );
	m_pContactAddr = new wxChoice( this, -1, wxPoint(-1, -1), wxSize(100,-1), 
									0, NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pContactAddr->SetSelection( 0 );

	// info :: website
	m_pWebSiteButton = new wxButton( this, wxID_OBS_INFO_WEBSITE_BUTTON, wxT("View Website"), wxPoint(-1, -1), wxSize(80,-1) );
//	m_pWebSiteText = new wxStaticText( this, -1, wxT("none"));
	// info :: webRef
	m_pWebRefButton = new wxButton( this, wxID_OBS_INFO_WEBREF_BUTTON, wxT("Web Refence"), wxPoint(-1, -1), wxSize(80,-1) );
	m_pWebRefUrl = new wxChoice( this, -1, wxPoint(-1, -1), wxSize(120,-1), 
									0, NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pWebRefUrl->SetSelection( 0 );

	////////////
	// layout
	// :: kind/show
	sizerFilter->Add( new wxStaticText( this, -1, wxT("Show:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerFilter->Add( m_pFilterKind, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: type
	sizerFilter->Add( new wxStaticText( this, -1, wxT("Type:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxLEFT, 20 );
	sizerFilter->Add( m_pFilterType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: find
	sizerFilter->Add( m_pFindEntry, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerFilter->Add( m_pFindButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );

	// :: map view
	sizerMapBox->Add( m_pMapView, 1, wxGROW |wxEXPAND );

	// :: desc
	sizerDescBox->Add( m_pDescText, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW|wxEXPAND );

	// photo buttons
	sizerPhotoButtons->Add( m_pPhotoBack, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerPhotoButtons->Add( m_pPhotoView, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5  );
	sizerPhotoButtons->Add( m_pPhotoNext, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// photo image
	sizerPhotoData->Add( m_pPhoto, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );
	sizerPhotoData->Add( m_pPhotoText, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxTOP, 2  );

	// instruments
	sizerInstBox->Add( m_pInstruments, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 2  );

	// info :: Organization
	sizerInfoData->Add( m_pOrgLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerInfoData->Add( m_pOrgValue, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// info :: Code
	sizerInfoData->Add( m_pCodeLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerInfoData->Add( m_pCodeValue, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// info :: Clear Nights
	sizerInfoData->Add( m_pWeatherLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerInfoData->Add( m_pWeatherValue, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// info :: Built
	sizerInfoData->Add( m_pBuiltLabel, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerInfoData->Add( m_pBuiltValue, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// info :: Contact
	pActSizer->Add( m_pContactButton, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	pActSizer->Add( m_pContactAddr, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// info :: website
	pActSizer->Add( m_pWebSiteButton, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	//pActSizer->Add( m_pWebSiteText, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// info :: webref
	pActSizer->Add( m_pWebRefButton, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	pActSizer->Add( m_pWebRefUrl, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );

	SetSizer(pTopSizer);
	SetAutoLayout(TRUE);
	pTopSizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CObservatoriesDlg
// Purpose:	destroy my dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CObservatoriesDlg::~CObservatoriesDlg( ) 
{
	delete( m_pFilterKind );
	delete( m_pFilterType );
	delete( m_pFindEntry );
	delete( m_pFindButton );
	delete( m_pDescText );
	delete( m_pPhotoBack );
	delete( m_pPhotoNext );
	delete( m_pPhotoView );
	delete( m_pPhoto );
	delete( m_pPhotoText );
	delete( m_pInstruments );
	// info
	delete( m_pOrgLabel );
	delete( m_pOrgValue );
	delete( m_pCodeLabel );
	delete( m_pCodeValue );
	delete( m_pWeatherLabel );
	delete( m_pWeatherValue );
	delete( m_pBuiltLabel );
	delete( m_pBuiltValue );
	// contact
	delete( m_pContactButton );
	delete( m_pContactAddr );
	// web site
	delete( m_pWebSiteButton );
//	delete( m_pWebSiteText );
	// web ref
	delete( m_pWebRefButton );
	delete( m_pWebRefUrl );

	if( m_pMapView ) delete( m_pMapView );
	if( m_pObservatories ) delete( m_pObservatories );
//	delete( m_pEarth );
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CObservatoriesDlg
// Purpose:	Set and get my reference from the config object
// Input:	pointer to main config
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::SetConfig( )
{
//	m_pCountry->Clear();
//	m_pEarth->LoadLocations( );
	
//	LoadData( );

	int nIdDetails = -1;
//	int is_selected = 0;
	int max_instr = 0;

	for( int i=0; i<m_pObservatories->m_vectObservatory.size(); i++ )
	{
		// get teh best with details
		if( m_pObservatories->m_vectObservatory[i].has_details )
		{
			if( m_pObservatories->m_vectObservatory[i].vectPhoto.size() > max_instr )
			{
				nIdDetails = i;
				max_instr = m_pObservatories->m_vectObservatory[i].vectPhoto.size();
			}

		}

		m_pMapView->AddMark( m_pObservatories->m_vectObservatory[i].lat, 
								m_pObservatories->m_vectObservatory[i].lon,
								m_pObservatories->m_vectObservatory[i].name,
								m_pObservatories->m_vectObservatory[i].has_details, 0 );
	}
	// set selected
	m_pMapView->m_vectMarks[nIdDetails].is_selected = 1;


	// set target observatory
	if( nIdDetails >= 0 ) SetObservatory( nIdDetails );

	GetSizer()->Layout();
	Layout();
	Fit();
	Update();
	Refresh( FALSE );

//	SetCountries( nCountryId );
//	SetRegions( nRegionId );
//	SetCities( nCityId );
//	SetCity( );
}

////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::LoadData( )
{
	m_pObservatories = new CObservatories();
}

////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::SetObservatoryPhoto( int nId, int nPhotoId )
{
	if( nPhotoId >= m_pObservatories->m_vectObservatory[nId].vectPhoto.size() ) return;

	wxString strFile;

	strFile.Printf( unMakeAppPath(wxT("data/observatory/%s")), m_pObservatories->m_vectObservatory[nId].vectPhoto[nPhotoId].file );
	wxImage imgPhoto( strFile, wxBITMAP_TYPE_ANY );
	int w = imgPhoto.GetWidth();
	int h = imgPhoto.GetHeight();
	if( w >= h )
	{
		double r = (double) 250.0/w;
		m_pPhoto->SetBitmap( wxBitmap( imgPhoto.Rescale(250, (int)h*r) ) );

	} else
	{
		double r = (double) 200.0/h;
		m_pPhoto->SetBitmap( wxBitmap( imgPhoto.Rescale((int)w*r,200) ) );
	}

	// set text detail
	wxString strPhotoDesc = m_pObservatories->m_vectObservatory[nId].vectPhoto[nPhotoId].desc.Mid(0,125);
	if( !strPhotoDesc.IsEmpty() ) strPhotoDesc += wxT("...");
	m_pPhotoText->SetLabel( strPhotoDesc );
	m_pPhotoText->Wrap( 250 );
}

////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::SetObservatory( int nId )
{
	char strTmp[255];
	int i=0;

	m_nSelectedObs = nId;
	double lat = m_pObservatories->m_vectObservatory[nId].lat;
	double lon = m_pObservatories->m_vectObservatory[nId].lon;

	//////////////////////
	// set photo
	if( m_pObservatories->m_vectObservatory[nId].vectPhoto.size() )
	{
		SetObservatoryPhoto( nId, 0 );
		m_nCurrentPhoto = 0;

		m_pPhotoBack->Disable();
		m_pPhotoView->Enable();
		m_pPhotoNext->Enable();

	} else
	{
		// disable all buttons
		m_pPhotoBack->Disable();
		m_pPhotoNext->Disable();
		m_pPhotoView->Disable();
		// set empty picture
		wxBitmap bmpPhoto( 250, 200 );
		m_pPhoto->SetBitmap( bmpPhoto );
		// photo details
		m_pPhotoText->SetLabel( wxT("- photo details -") );
	}

	// set instruments
	m_pInstruments->Clear();
	if( m_pObservatories->m_vectObservatory[nId].vectInstr.size() )
	{
		m_pInstruments->Enable();
		for( i=0; i<m_pObservatories->m_vectObservatory[nId].vectInstr.size(); i++ )
		{
			m_pInstruments->Append( m_pObservatories->m_vectObservatory[nId].vectInstr[i].name );
		}
		m_pInstruments->SetSelection( 0 );

	} else
	{
		m_pInstruments->Disable();
	}

	// set description
	m_pDescText->SetValue( m_pObservatories->m_vectObservatory[nId].desc );

	// set contact
	m_pContactAddr->Clear();
	if( !m_pObservatories->m_vectObservatory[nId].email.IsEmpty() ||
		!m_pObservatories->m_vectObservatory[nId].phone.IsEmpty() || 
		!m_pObservatories->m_vectObservatory[nId].fax.IsEmpty() )
	{
		m_pContactButton->Enable();
		m_pContactAddr->Enable();

		if( !m_pObservatories->m_vectObservatory[nId].email.IsEmpty() ) m_pContactAddr->Append( m_pObservatories->m_vectObservatory[nId].email );
		if( !m_pObservatories->m_vectObservatory[nId].phone.IsEmpty() ) m_pContactAddr->Append( m_pObservatories->m_vectObservatory[nId].phone );
		if( !m_pObservatories->m_vectObservatory[nId].fax.IsEmpty() ) m_pContactAddr->Append( m_pObservatories->m_vectObservatory[nId].fax );
		m_pContactAddr->SetSelection( 0 );

	} else
	{
		m_pContactButton->Disable();
		m_pContactAddr->Disable();
	}

	// set website
	if( !m_pObservatories->m_vectObservatory[nId].web.IsEmpty() )
		m_pWebSiteButton->Enable();
	else
		m_pWebSiteButton->Disable();

	// set web references
	m_pWebRefUrl->Clear();
	if( m_pObservatories->m_vectObservatory[nId].vectWebRef.size() )
	{
		m_pWebRefButton->Enable();
		m_pWebRefUrl->Enable();
		
		for( i=0; i<m_pObservatories->m_vectObservatory[nId].vectWebRef.size(); i++ )
		{
			m_pWebRefUrl->Append( m_pObservatories->m_vectObservatory[nId].vectWebRef[i].name );
		}
		m_pWebRefUrl->SetSelection( 0 );

	} else
	{
		m_pWebRefButton->Disable();
		m_pWebRefUrl->Disable();
	}

	/////
	// set info data
	// :: observatory 
	if( !m_pObservatories->m_vectObservatory[nId].organization.IsEmpty() ) 
	{
		m_pOrgValue->SetLabel( m_pObservatories->m_vectObservatory[nId].organization );
		m_pOrgValue->Wrap( 180 );

	} else
	{
		m_pOrgValue->SetLabel( wxT("none") );
	}
	// :: code
	if( !m_pObservatories->m_vectObservatory[nId].code.IsEmpty() )
	{
		m_pCodeValue->SetLabel( m_pObservatories->m_vectObservatory[nId].code );

	} else
	{
		m_pCodeValue->SetLabel( wxT("none") );
	}
	// :: weather
	if( !m_pObservatories->m_vectObservatory[nId].weather.IsEmpty() )
	{
		m_pWeatherValue->SetLabel( m_pObservatories->m_vectObservatory[nId].weather );

	} else
	{
		m_pWeatherValue->SetLabel( wxT("none") );
	}
	// :: built
	if( !m_pObservatories->m_vectObservatory[nId].built.IsEmpty() )
	{
		m_pBuiltValue->SetLabel( m_pObservatories->m_vectObservatory[nId].built );

	} else
	{
		m_pBuiltValue->SetLabel( wxT("none") );
	}

	///////////////
	// set my city on the map
	m_pMapView->m_nLocX = (double) ((lon+180.)/360.*m_pMapView->m_nOrigPicWidth);
	m_pMapView->m_nLocY = (double) ((lat-90.)/-180.*m_pMapView->m_nOrigPicHeight);
	//strcpy( m_pMapView->m_strLocName, m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].name );
	m_pMapView->m_strLocName = m_pObservatories->m_vectObservatory[nId].name;
	m_pMapView->bIsLocSet = 1;

	// :: also set target
	m_pMapView->m_nMousePicX = m_pMapView->m_nLocX;
	m_pMapView->m_nMousePicY = m_pMapView->m_nLocY;
	m_pMapView->m_bMainTarget = 2;

	m_pMapView->Refresh( false );
}

////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::OnSetPhoto( wxCommandEvent& pEvent )
{
	int nActId = pEvent.GetId();
	int nNoPhoto = m_pObservatories->m_vectObservatory[m_nSelectedObs].vectPhoto.size();

	if( nActId == wxID_OBS_PHOTO_NEXT  && m_nCurrentPhoto < nNoPhoto )
	{
		m_nCurrentPhoto++;
		if( m_nCurrentPhoto > 0 ) m_pPhotoBack->Enable();
		if( m_nCurrentPhoto >= nNoPhoto-1 ) m_pPhotoNext->Disable();

	} else if( nActId == wxID_OBS_PHOTO_BACK  && m_nCurrentPhoto > 0 )
	{
		m_nCurrentPhoto--;
		if( m_nCurrentPhoto < nNoPhoto ) m_pPhotoNext->Enable();
		if( m_nCurrentPhoto <=0 ) m_pPhotoBack->Disable();
	}

	SetObservatoryPhoto( m_nSelectedObs, m_nCurrentPhoto );

	GetSizer()->Layout();
	Layout();
	Fit();
	Update();
	Refresh( FALSE );

}

// Method:	OnPhotoViewFull
////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::OnPhotoViewFull( wxCommandEvent& pEvent )
{
	wxString strFile;
	wxString strTitle;

	strFile.Printf( unMakeAppPath(wxT("data/observatory/%s")), m_pObservatories->m_vectObservatory[m_nSelectedObs].vectPhoto[m_nCurrentPhoto].file );

	if( wxFileExists( strFile ) )
	{
		strTitle.Printf( wxT("Image view :: %s"), m_pObservatories->m_vectObservatory[m_nSelectedObs].vectPhoto[m_nCurrentPhoto].file );
		CImageViewDlg *pDlg = new CImageViewDlg( this, strTitle );
		pDlg->SetImageFromFile( strFile );

		if ( pDlg->ShowModal( ) == wxID_OK )
		{
			// do something here ? - maybe marked as viewed ?
		}
		delete( pDlg );
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::OnViewWebRef( wxCommandEvent& pEvent )
{
	int nId = m_pWebRefUrl->GetSelection();
	if( nId < 0 ) return;

	wxLaunchDefaultBrowser( m_pObservatories->m_vectObservatory[m_nSelectedObs].vectWebRef[nId].link );
}

////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::OnViewWebSite( wxCommandEvent& pEvent )
{
	wxLaunchDefaultBrowser( m_pObservatories->m_vectObservatory[m_nSelectedObs].web );
}

////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::OnContact( wxCommandEvent& pEvent )
{
	char strCmd[255];

	sprintf( strCmd, "start mailto:%s", m_pObservatories->m_vectObservatory[m_nSelectedObs].email );
	system( strCmd );
}

////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::OnUpdateObsLocation( wxCommandEvent& pEvent )
{
	if( m_pMapView->m_nSelectedMark >=0 ) SetObservatory( m_pMapView->m_nSelectedMark );

	GetSizer()->Layout();
	Layout();
	Fit();
	Update();
	Refresh( FALSE );
}

////////////////////////////////////////////////////////////////////
// Method:	SetCountries
// Class:	CObservatoriesDlg
// Purpose:	set countries
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::SetCountries( int nCountryId )
{
	//m_pEarth->SetCountriesWidget( m_pCountry, nCountryId );
}

////////////////////////////////////////////////////////////////////
// Method:	SetRegions
// Class:	CObservatoriesDlg
// Purpose:	set regions for current country
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::SetRegions( int nSelect )
{
//	int nCountryId = m_pCountry->GetSelection( );
//	m_pEarth->SetRegionsWidget( m_pRegion, nCountryId, nSelect );
}

////////////////////////////////////////////////////////////////////
// Method:	SetCities
// Class:	CObservatoriesDlg
// Purpose:	set cities for current country/region
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::SetCities( int nSelect )
{
//	int nCountryId = m_pCountry->GetSelection( );
//	int nRegionId = m_pRegion->GetSelection( );

//	m_pEarth->SetCitiesWidget( m_pCity, nCountryId, nRegionId, nSelect );
}

////////////////////////////////////////////////////////////////////
// Method:	SetCity
// Class:	CObservatoriesDlg
// Purpose:	set current city info
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::SetCity( )
{
	char strTmp[255];
	int nCountryId=0, nRegionId=0, nCityId=0;

//	nCountryId = m_pCountry->GetSelection( );
//	nRegionId = m_pRegion->GetSelection( );
//	nCityId = m_pCity->GetSelection( );

	if( nCountryId < 0 || nRegionId < 0 || nCityId < 0 ) return;

	////////////////////////
	// :: LATITUDE :: is NORTH to SOUTH 90 to -90 
	double lat = m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].lat;
//	sprintf( strTmp, "%.6f", lat );
//	m_pLatitude->SetValue( strTmp );

	////////////////////////
	// :: LONGITUDE :: is WEST - EAST ... -180 to 180
	double lon = m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].lon;
//	sprintf( strTmp, "%.6f", lon );
//	m_pLongitude->SetValue( strTmp );

	/////////////////////
	// :: ALTITUDE
	sprintf( strTmp, "%.4f", m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].alt );
//	m_pAltitude->SetValue( strTmp );

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
void CObservatoriesDlg::OnUpdateLocation( wxCommandEvent& pEvent )
{
	char strTmp[255];

//	m_pCountry->SetSelection( m_pMapView->m_nCountryId );
//	SetRegions( m_pMapView->m_nRegionId );
//	SetCities( m_pMapView->m_nCityId );

	// now set target lat/log	
	// :: LATITUDE :: is NORTH to SOUTH 90 to -90 
	double lat = m_pMapView->m_nTargetLat;
//	sprintf( strTmp, "%.6f", lat );
//	m_pLatitude->SetValue( strTmp );

	// :: LONGITUDE :: is WEST - EAST ... -180 to 180
	double lon = m_pMapView->m_nTargetLon;
//	sprintf( strTmp, "%.6f", lon );
//	m_pLongitude->SetValue( strTmp );

	sprintf( strTmp, "%.4f", m_pEarth->m_vectCountry[m_pMapView->m_nCountryId].vectRegion[m_pMapView->m_nRegionId].vectCity[m_pMapView->m_nCityId].alt );
//	m_pAltitude->SetValue( strTmp );
}

////////////////////////////////////////////////////////////////////
// Method:	SetCustomPositionFromString
// Class:	CObservatoriesDlg
// Purpose:	set custom position from strings
// Input:	lat/log/alt as wxstrings
// Output:	status
////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::SetCustomPositionFromString( wxString& strLat, wxString& strLon, wxString& strAlt )
{
	// we first compare
/*	if( strLat.CmpNoCase( m_pLatitude->GetLineText(0) ) == 0 &&
		strLon.CmpNoCase( m_pLongitude->GetLineText(0) ) == 0 )
	{
		// check if to update altitude
		if( strAlt.CmpNoCase( m_pAltitude->GetValue() ) != 0 ) m_pAltitude->SetValue( strAlt );
		return;
	}
*/
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
void CObservatoriesDlg::SetCustomPosition( double lat, double lon, double alt )
{
	char strTmp[255];

	// first we just set the strings
/*	sprintf( strTmp, "%.6lf", lat );
	m_pLatitude->SetValue( strTmp );
	sprintf( strTmp, "%.6lf", lon );
	m_pLongitude->SetValue( strTmp );
	sprintf( strTmp, "%.2lf", alt );
	m_pAltitude->SetValue( strTmp );
*/
	m_pMapView->m_nMousePicX = (double) ((lon+180.)/360.*m_pMapView->m_nOrigPicWidth);
	m_pMapView->m_nMousePicY = (double) ((lat-90.)/-180.*m_pMapView->m_nOrigPicHeight);
	m_pMapView->m_bMainTarget = 2;

	m_pMapView->Refresh( false );
}

// Method:	SetLocByName
////////////////////////////////////////////////////////////////////
void CObservatoriesDlg::SetLocByName( wxString& strCountry, wxString& strProvince, wxString& strCity, int bCity )
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

//	m_pEarth->SetCountriesWidget( m_pCountry, nCountryId );
//	m_pEarth->SetRegionsWidget( m_pRegion, nCountryId, nRegionId );
//	m_pEarth->SetCitiesWidget( m_pCity, nCountryId, nRegionId, nCityId );

	if( bCity ) SetCity( );

	return;
}
