////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/wxprec.h"

// local includes
#include "../config/mainconfig.h"

// main header
#include "view_objects_dlg.h"

BEGIN_EVENT_TABLE(CViewObjectsDlg, wxDialog)
	EVT_CHECKBOX( wxID_VIEW_ALL_OBJECTS, CViewObjectsDlg::OnViewHideAll )
	EVT_CHECKBOX( wxID_VIEW_HIDE_ALL_OBJECTS, CViewObjectsDlg::OnViewHideAll )

	// enable/disable objects/sources
	EVT_CHECKBOX( wxID_VIEW_OBJ_DETECTED, CViewObjectsDlg::OnViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_STARS, CViewObjectsDlg::OnViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_DSO, CViewObjectsDlg::OnViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_GAMMA_XRAY, CViewObjectsDlg::OnViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_RADIO, CViewObjectsDlg::OnViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_MSTARS, CViewObjectsDlg::OnViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_SUPERNOVAS, CViewObjectsDlg::OnViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_BLACKHOLES, CViewObjectsDlg::OnViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_EXO_PLANETS, CViewObjectsDlg::OnViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_ASTEROIDS, CViewObjectsDlg::OnViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_COMETS, CViewObjectsDlg::OnViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_SATELLITES, CViewObjectsDlg::OnViewObjects )

	// view all objects/sources
	EVT_CHECKBOX( wxID_VIEW_OBJ_DETECTED_ALL, CViewObjectsDlg::OnObjViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_STARS_ALL, CViewObjectsDlg::OnObjViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_DSO_ALL, CViewObjectsDlg::OnObjViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_GAMMA_XRAY_ALL, CViewObjectsDlg::OnObjViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_RADIO_ALL, CViewObjectsDlg::OnObjViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_MSTARS_ALL, CViewObjectsDlg::OnObjViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_SUPERNOVAS_ALL, CViewObjectsDlg::OnObjViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_BLACKHOLES_ALL, CViewObjectsDlg::OnObjViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_EXO_PLANETS_ALL, CViewObjectsDlg::OnObjViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_ASTEROIDS_ALL, CViewObjectsDlg::OnObjViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_COMETS_ALL, CViewObjectsDlg::OnObjViewObjects )
	EVT_CHECKBOX( wxID_VIEW_CAT_SATELLITES_ALL, CViewObjectsDlg::OnObjViewObjects )

	END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////
CViewObjectsDlg::CViewObjectsDlg( wxWindow *parent, CConfigMain* pMainConfig ) : wxDialog( parent, -1, 
							wxT("View Image Objects Setup"), wxDefaultPosition,
							wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	m_pMainConfig = pMainConfig;
	m_nObj = 0;

	// sort types
	wxString vectSortOrder[2] = { wxT("Asc"), wxT("Desc") };
	// sort fields
	wxString vectStarsSort[2] = { wxT("Mag") };

	// main dialog sizer
	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 1, 1, 5, 5 );

	// header sizer
	wxFlexGridSizer *sizerHeader = new wxFlexGridSizer( 1, 5, 5, 10 );
	topsizer->Add( sizerHeader, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxTOP, 10 );

	// entries panel sizer
	wxStaticBox* pObjSrcBox = new wxStaticBox( this, -1, wxT("Display Params"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pObjSrcSizer = new wxStaticBoxSizer( pObjSrcBox, wxVERTICAL );
	// the list
	sizerList = new wxFlexGridSizer( 5, 6, 5, 10 );
	pObjSrcSizer->Add( sizerList, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );

	//////////////////////////
	// elem: headers
	m_pViewAll = new wxCheckBox( this, wxID_VIEW_ALL_OBJECTS, wxT("View All") );
	m_pHideAll = new wxCheckBox( this, wxID_VIEW_HIDE_ALL_OBJECTS, wxT("Hide All") );
	m_pHideNoMatch = new wxCheckBox( this, wxID_VIEW_HIDE_NO_MATCH, wxT("Hide Not Matched") );

//	this->Freeze();
	// elem: detected = 0
	AddObjOptions( wxID_VIEW_OBJ_DETECTED, wxID_VIEW_OBJ_DETECTED_ALL, wxT("Detected Objects:"), vectStarsSort, 1 );
	// elem: catalog stars
	AddObjOptions( wxID_VIEW_CAT_STARS, wxID_VIEW_CAT_STARS_ALL, wxT("Catalog Stars:"), vectStarsSort, 1 );
	// elem: catalog dso
	AddObjOptions( wxID_VIEW_CAT_DSO, wxID_VIEW_CAT_DSO_ALL, wxT("Deep Sky Objects:"), vectStarsSort, 1 );
	// elem: catalog gamma/xray
	wxString vectGammaXraySort[] = { wxT("Flux") };
	AddObjOptions( wxID_VIEW_CAT_GAMMA_XRAY, wxID_VIEW_CAT_GAMMA_XRAY_ALL, wxT("Gamma/Xray:"), vectGammaXraySort, 1 );
	// elem: catalog radio sources
	wxString vectRadioSort[] = { wxT("Flux") };
	AddObjOptions( wxID_VIEW_CAT_RADIO, wxID_VIEW_CAT_RADIO_ALL, wxT("Radio Sources:"), vectRadioSort, 1 );
	// elem: catalog multiple stars
	AddObjOptions( wxID_VIEW_CAT_MSTARS, wxID_VIEW_CAT_MSTARS_ALL, wxT("Multiple Stars:"), vectStarsSort, 1 );
	// elem: catalog supernovas
	wxString vectSupernovasSort[] = { wxT("Mag"), wxT("Date") };
	AddObjOptions( wxID_VIEW_CAT_SUPERNOVAS, wxID_VIEW_CAT_SUPERNOVAS_ALL, wxT("Supernovas:"), vectStarsSort, 1 );
	// elem: catalog blackholes
	wxString vectBlackholesSort[] = { wxT("Mass") };
	AddObjOptions( wxID_VIEW_CAT_BLACKHOLES, wxID_VIEW_CAT_BLACKHOLES_ALL, wxT("Blackholes:"), vectBlackholesSort, 1 );
	// elem: catalog exo-planets
	wxString vectExoPlanetsSort[] = { wxT("Mass") };
	AddObjOptions( wxID_VIEW_CAT_EXO_PLANETS, wxID_VIEW_CAT_EXO_PLANETS_ALL, wxT("Exo-Planets:"), vectExoPlanetsSort, 1 );
	// elem: catalog asteroids
	wxString vectAsteroidsSort[] = { wxT("Diam") };
	AddObjOptions( wxID_VIEW_CAT_ASTEROIDS, wxID_VIEW_CAT_ASTEROIDS_ALL, wxT("Asteroids:"), vectAsteroidsSort, 1 );
	// elem: catalog comets
	wxString vectCometsSort[] = { wxT("Mass") };
	AddObjOptions( wxID_VIEW_CAT_COMETS, wxID_VIEW_CAT_COMETS_ALL, wxT("Comets:"), vectCometsSort, 1 );
	// elem: catalog satellites
	wxString vectSatSort[] = { wxT("Mass") };
	AddObjOptions( wxID_VIEW_CAT_SATELLITES, wxID_VIEW_CAT_SATELLITES_ALL, wxT("Satellites:"), vectSatSort, 1 );

//	this->Thaw();

	/////////////////////
	// :: header
	sizerHeader->Add( m_pViewAll, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
	sizerHeader->Add( m_pHideAll, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );
	sizerHeader->Add( m_pHideNoMatch, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxLEFT, 10 );

	// add to main sizer
	topsizer->Add( pObjSrcSizer, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW|wxALL, 5 );
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL ), 1, wxALIGN_CENTER| wxCENTRE | wxALL, 5 );

	// set config
	SetConfig( m_pMainConfig );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);

}

////////////////////////////////////////////////////////////////////
// CViewObjectsDlg
////////////////////////////////////////////////////////////////////
CViewObjectsDlg::~CViewObjectsDlg( ) 
{
	int i=0;

	// header
	delete( m_pViewAll );
	delete( m_pHideAll );
	delete( m_pHideNoMatch );

	// remove all sources props
	for( i=0; i<m_nObj; i++ )
	{
		delete( m_vectObj[i] );
		delete( m_vectLabel[i] );
		delete( m_vectLimit[i] );
		delete( m_vectSort[i] );
		delete( m_vectOrder[i] );
		delete( m_vectAll[i] );
	}
}

////////////////////////////////////////////////////////////////////
void CViewObjectsDlg::SetConfig( CConfigMain* pMainConfig )
{
	m_pMainConfig = pMainConfig;

	// set not match
	m_pHideNoMatch->SetValue( !(m_pMainConfig->m_bViewDetectedObjectsNoMatch) );

	this->Freeze();
	// set object sources
	SetObjSrc( 0, m_pMainConfig->m_nViewDetectedObjects );
	SetObjSrc( 1, m_pMainConfig->m_nViewCatalogStars );
	SetObjSrc( 2, m_pMainConfig->m_nViewCatalogDso );
	SetObjSrc( 3, m_pMainConfig->m_nViewCatalogXGamma );
	SetObjSrc( 4, m_pMainConfig->m_nViewCatalogRadio );
	SetObjSrc( 5, m_pMainConfig->m_nViewCatalogMStars );
	SetObjSrc( 6, m_pMainConfig->m_nViewCatalogSupernovas );
	SetObjSrc( 7, m_pMainConfig->m_nViewCatalogBlackholes );
	SetObjSrc( 8, m_pMainConfig->m_nViewCatalogExoplanets );
	SetObjSrc( 9, m_pMainConfig->m_nViewCatalogAsteroids );
	SetObjSrc( 10, m_pMainConfig->m_nViewCatalogComets );
	SetObjSrc( 11, m_pMainConfig->m_nViewCatalogAes );

	// set all show/hide
	if( m_pMainConfig->m_bViewObjectsType == 1 )
	{
		m_pViewAll->SetValue(1);
		m_pHideAll->SetValue(0);
		SetViewHideAll( wxID_VIEW_ALL_OBJECTS, 1 );

	} else if( m_pMainConfig->m_bViewObjectsType == 2 )
	{
		m_pViewAll->SetValue(0);
		m_pHideAll->SetValue(1);
		SetViewHideAll( wxID_VIEW_HIDE_ALL_OBJECTS, 1 );

	} else
	{
		m_pViewAll->SetValue(0);
		m_pHideAll->SetValue(0);
		SetViewHideAll( wxID_VIEW_ALL_OBJECTS, 0 );
	}

	this->Thaw();
}

////////////////////////////////////////////////////////////////////
void CViewObjectsDlg::SetObjSrc( int nId, int nVal )
{
	// if not
	if( nVal <= -2 )
	{
		m_vectObj[nId]->SetValue(0);
		m_vectAll[nId]->SetValue(0);
		// disable fields
		SetViewObjects( nId, 0 );

	} else if( nVal == -1 )
	{
		m_vectObj[nId]->SetValue(1);
		m_vectAll[nId]->SetValue(1);
		// enable fields
		SetViewObjects( nId, 1 );
		m_vectLimit[nId]->Disable();

	} else if( nVal >= 0 )
	{
		m_vectObj[nId]->SetValue(1);
		m_vectAll[nId]->SetValue(0);
		m_vectLimit[nId]->SetValue(nVal);
		// enable fields
		SetViewObjects( nId, 1 );
		m_vectLimit[nId]->Enable();
	}
}

////////////////////////////////////////////////////////////////////
void CViewObjectsDlg::AddObjOptions( int nId, int nIdAll, const wxString label, const wxString* vectSort, int nSort )
{
	// sort types
	wxString vectSortOrder[2] = { wxT("Asc"), wxT("Desc") };

	// create elements
	m_vectObj[m_nObj] = new wxCheckBox( this, nId, wxT("") );
	m_vectLabel[m_nObj] = new wxStaticText( this, -1, label );
	m_vectLimit[m_nObj] = new wxSpinCtrl( this, -1, wxT("50"), wxDefaultPosition, wxSize(60,-1), wxSP_ARROW_KEYS, 0, 60000, 50 );
	m_vectSort[m_nObj] = new wxChoice( this, -1, wxDefaultPosition, wxSize(50,-1), 1, vectSort );
	m_vectSort[m_nObj]->SetSelection( 0 );
	m_vectOrder[m_nObj] = new wxChoice( this, -1, wxDefaultPosition, wxSize(50,-1), 2, vectSortOrder );
	m_vectOrder[m_nObj]->SetSelection( 0 );
	m_vectAll[m_nObj] = new wxCheckBox( this, nIdAll, wxT("All") );

	// add to sizer
	sizerList->Add( m_vectObj[m_nObj], 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
	sizerList->Add( m_vectLabel[m_nObj], 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
	sizerList->Add( m_vectLimit[m_nObj], 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerList->Add( m_vectSort[m_nObj], 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerList->Add( m_vectOrder[m_nObj], 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
	sizerList->Add( m_vectAll[m_nObj], 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );

	m_nObj++;
}

////////////////////////////////////////////////////////////////////
void CViewObjectsDlg::OnViewHideAll( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	int bState = pEvent.IsChecked();

	SetViewHideAll( nId, bState );
}

////////////////////////////////////////////////////////////////////
void CViewObjectsDlg::SetViewHideAll( int nId, int bState )
{

	SetStateAll( !bState );

	if( !bState )
	{
		m_pHideNoMatch->Enable();
		if( nId == wxID_VIEW_ALL_OBJECTS )
			m_pHideAll->Enable();
		else if( nId == wxID_VIEW_HIDE_ALL_OBJECTS )
			m_pViewAll->Enable();
	
	} else
	{
		m_pHideNoMatch->Disable();
		if( nId == wxID_VIEW_ALL_OBJECTS )
			m_pHideAll->Disable();
		else if( nId == wxID_VIEW_HIDE_ALL_OBJECTS )
			m_pViewAll->Disable();
	}
}

////////////////////////////////////////////////////////////////////
void CViewObjectsDlg::OnViewObjects( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	int bState = pEvent.IsChecked();

	// get what to be 
	if( nId == wxID_VIEW_OBJ_DETECTED )
		nId = VIEW_OBJ_DETECT;
	else if( nId == wxID_VIEW_CAT_STARS )
		nId = VIEW_OBJ_CAT_STARS;
	else if( nId == wxID_VIEW_CAT_DSO )
		nId = VIEW_OBJ_CAT_DSO;
	else if( nId == wxID_VIEW_CAT_GAMMA_XRAY )
		nId = VIEW_OBJ_CAT_GAMMA_XRAY;
	else if( nId == wxID_VIEW_CAT_RADIO )
		nId = VIEW_OBJ_CAT_RADIO;
	else if( nId == wxID_VIEW_CAT_MSTARS )
		nId = VIEW_OBJ_CAT_MSTARS;
	else if( nId == wxID_VIEW_CAT_SUPERNOVAS )
		nId = VIEW_OBJ_CAT_SUPERNOVAS;
	else if( nId == wxID_VIEW_CAT_BLACKHOLES )
		nId = VIEW_OBJ_CAT_BLACKHOLES;
	else if( nId == wxID_VIEW_CAT_EXO_PLANETS )
		nId = VIEW_OBJ_CAT_EXO_PLANETS;
	else if( nId == wxID_VIEW_CAT_ASTEROIDS )
		nId = VIEW_OBJ_CAT_ASTEROIDS;
	else if( nId == wxID_VIEW_CAT_COMETS )
		nId = VIEW_OBJ_CAT_COMETS;
	else if( nId == wxID_VIEW_CAT_SATELLITES )
		nId = VIEW_OBJ_CAT_SATELLITES;

	SetViewObjects( nId, bState );
}

////////////////////////////////////////////////////////////////////
void CViewObjectsDlg::SetViewObjects( int nId, int bState )
{
	// enable/disable
	if( bState )
	{
		m_vectLabel[nId]->Enable();

		// case checked all
		if( m_vectAll[nId]->IsChecked() )
			m_vectLimit[nId]->Disable();
		else
			m_vectLimit[nId]->Enable();

		m_vectSort[nId]->Enable();
		m_vectOrder[nId]->Enable();
		m_vectAll[nId]->Enable();

	} else
	{
		m_vectLabel[nId]->Disable();
		m_vectLimit[nId]->Disable();
		m_vectSort[nId]->Disable();
		m_vectOrder[nId]->Disable();
		m_vectAll[nId]->Disable();
	}

}

////////////////////////////////////////////////////////////////////
void CViewObjectsDlg::OnObjViewObjects( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	int bState = pEvent.IsChecked();

	// get what to be 
	if( nId == wxID_VIEW_OBJ_DETECTED_ALL )
		nId = VIEW_OBJ_DETECT;
	else if( nId == wxID_VIEW_CAT_STARS_ALL )
		nId = VIEW_OBJ_CAT_STARS;
	else if( nId == wxID_VIEW_CAT_DSO_ALL )
		nId = VIEW_OBJ_CAT_DSO;
	else if( nId == wxID_VIEW_CAT_GAMMA_XRAY_ALL )
		nId = VIEW_OBJ_CAT_GAMMA_XRAY;
	else if( nId == wxID_VIEW_CAT_RADIO_ALL )
		nId = VIEW_OBJ_CAT_RADIO;
	else if( nId == wxID_VIEW_CAT_MSTARS_ALL )
		nId = VIEW_OBJ_CAT_MSTARS;
	else if( nId == wxID_VIEW_CAT_SUPERNOVAS_ALL )
		nId = VIEW_OBJ_CAT_SUPERNOVAS;
	else if( nId == wxID_VIEW_CAT_BLACKHOLES_ALL )
		nId = VIEW_OBJ_CAT_BLACKHOLES;
	else if( nId == wxID_VIEW_CAT_EXO_PLANETS_ALL )
		nId = VIEW_OBJ_CAT_EXO_PLANETS;
	else if( nId == wxID_VIEW_CAT_ASTEROIDS_ALL )
		nId = VIEW_OBJ_CAT_ASTEROIDS;
	else if( nId == wxID_VIEW_CAT_COMETS_ALL )
		nId = VIEW_OBJ_CAT_COMETS;
	else if( nId == wxID_VIEW_CAT_SATELLITES_ALL )
		nId = VIEW_OBJ_CAT_SATELLITES;

	// enable/disable
	if( !bState )
		m_vectLimit[nId]->Enable();
	else
		m_vectLimit[nId]->Disable();
}

////////////////////////////////////////////////////////////////////
void CViewObjectsDlg::SetStateAll( int bState )
{
	int i=0;

	for( i=0; i<m_nObj; i++ )
	{
		if( bState )
		{
			m_vectObj[i]->Enable();
			// if checked - also enable
			if( m_vectObj[i]->IsChecked() )
			{
				m_vectLabel[i]->Enable();
				if( !m_vectAll[i]->IsChecked() ) m_vectLimit[i]->Enable();
				m_vectSort[i]->Enable();
				m_vectOrder[i]->Enable();
				m_vectAll[i]->Enable();
			}

		} else
		{
			m_vectObj[i]->Disable();
			m_vectLabel[i]->Disable();
			m_vectLimit[i]->Disable();
			m_vectSort[i]->Disable();
			m_vectOrder[i]->Disable();
			m_vectAll[i]->Disable();
		}
	}
}

////////////////////////////////////////////////////////////////////
int CViewObjectsDlg::GetObjSrcVal( int nId )
{
	int nVal = -2;

	if( m_vectObj[nId]->IsChecked() )
	{
		if( !m_vectAll[nId]->IsChecked() )
			nVal = m_vectLimit[nId]->GetValue();
		else
			nVal = -1;

	} else
	{
		nVal = -2;
	}
	return( nVal );
}

/***************************** OLD MENU ************************

	////////////////////
	// :: VIEW :: detected stars filter submenu
	wxMenu* pDetectStarsSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pDetectStarsSubMenu->AppendRadioItem( wxID_MENU_VIEW_DETECT_NONE, wxT("None"));
	pDetectStarsSubMenu->AppendRadioItem( wxID_MENU_VIEW_DETECT_50, wxT("50"));
	pDetectStarsSubMenu->AppendRadioItem( wxID_MENU_VIEW_DETECT_100, wxT("100"));
	pDetectStarsSubMenu->AppendRadioItem( wxID_MENU_VIEW_DETECT_200, wxT("200"));
	pDetectStarsSubMenu->AppendRadioItem( wxID_MENU_VIEW_DETECT_ALL, wxT("All"));
	pDetectStarsSubMenu->AppendSeparator( );
	pDetectStarsSubMenu->AppendCheckItem( wxID_MENU_VIEW_DETECT_NO_MATCH, wxT("No Match") , wxT("Show Not Matched") );
	pDetectStarsSubMenu->Check( wxID_MENU_VIEW_DETECT_NO_MATCH,  1 );
	pDetectStarsSubMenu->Check( wxID_MENU_VIEW_DETECT_50, 1 );	
	pViewMenu->Append( wxID_MENU_VIEW_DETECTED, wxT("Detected Objects"), pDetectStarsSubMenu );

	////////////////////
	// :: VIEW :: catalog stars filter submenu
	wxMenu* pCatStarsSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pCatStarsSubMenu->AppendRadioItem( wxID_MENU_VIEW_CATALOG_STARS_NONE, wxT("None"));
	pCatStarsSubMenu->AppendRadioItem( wxID_MENU_VIEW_CATALOG_STARS_50, wxT("50"));
	pCatStarsSubMenu->AppendRadioItem( wxID_MENU_VIEW_CATALOG_STARS_100, wxT("100"));
	pCatStarsSubMenu->AppendRadioItem( wxID_MENU_VIEW_CATALOG_STARS_200, wxT("200"));
	pCatStarsSubMenu->AppendRadioItem( wxID_MENU_VIEW_CATALOG_STARS_ALL, wxT("All"));
	pCatStarsSubMenu->Check( wxID_MENU_VIEW_CATALOG_STARS_50, 1 );	
	pViewMenu->Append( wxID_MENU_VIEW_CATALOG_STARS, wxT("Catalog Stars"), pCatStarsSubMenu );

	////////////////////
	// :: VIEW :: dso objects filter submenu
	wxMenu* pDsoObjectsSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pDsoObjectsSubMenu->AppendRadioItem( wxID_MENU_VIEW_DSO_OBJECTS_NONE, wxT("None"));
	pDsoObjectsSubMenu->AppendRadioItem( wxID_MENU_VIEW_DSO_OBJECTS_50, wxT("50"));
	pDsoObjectsSubMenu->AppendRadioItem( wxID_MENU_VIEW_DSO_OBJECTS_100, wxT("100"));
	pDsoObjectsSubMenu->AppendRadioItem( wxID_MENU_VIEW_DSO_OBJECTS_200, wxT("200"));
	pDsoObjectsSubMenu->AppendRadioItem( wxID_MENU_VIEW_DSO_OBJECTS_ALL, wxT("All"));
	pDsoObjectsSubMenu->Check( wxID_MENU_VIEW_DSO_OBJECTS_50, 1 );	
	pViewMenu->Append( wxID_MENU_VIEW_DSO_OBJECTS, wxT("DSO Objects"), pDsoObjectsSubMenu );

	////////////////////
	// :: VIEW :: gamma/x-ray filter submenu
	wxMenu* pXGammaObjectsSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pXGammaObjectsSubMenu->AppendRadioItem( wxID_MENU_VIEW_GAMMA_XRAY_NONE, wxT("None"));
	pXGammaObjectsSubMenu->AppendRadioItem( wxID_MENU_VIEW_GAMMA_XRAY_50, wxT("50"));
	pXGammaObjectsSubMenu->AppendRadioItem( wxID_MENU_VIEW_GAMMA_XRAY_100, wxT("100"));
	pXGammaObjectsSubMenu->AppendRadioItem( wxID_MENU_VIEW_GAMMA_XRAY_200, wxT("200"));
	pXGammaObjectsSubMenu->AppendRadioItem( wxID_MENU_VIEW_GAMMA_XRAY_ALL, wxT("All"));
	pXGammaObjectsSubMenu->Check( wxID_MENU_VIEW_GAMMA_XRAY_50, 1 );	
	pViewMenu->Append( wxID_MENU_VIEW_GAMMA_XRAY, wxT("Gamma/X-Ray"), pXGammaObjectsSubMenu );

	////////////////////
	// :: VIEW :: radio sources filter submenu
	wxMenu* pRadioObjectsSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pRadioObjectsSubMenu->AppendRadioItem( wxID_MENU_VIEW_RADIO_SOURCES_NONE, wxT("None"));
	pRadioObjectsSubMenu->AppendRadioItem( wxID_MENU_VIEW_RADIO_SOURCES_50, wxT("50"));
	pRadioObjectsSubMenu->AppendRadioItem( wxID_MENU_VIEW_RADIO_SOURCES_100, wxT("100"));
	pRadioObjectsSubMenu->AppendRadioItem( wxID_MENU_VIEW_RADIO_SOURCES_200, wxT("200"));
	pRadioObjectsSubMenu->AppendRadioItem( wxID_MENU_VIEW_RADIO_SOURCES_ALL, wxT("All"));
	pRadioObjectsSubMenu->Check( wxID_MENU_VIEW_RADIO_SOURCES_50, 1 );	
	pViewMenu->Append( wxID_MENU_VIEW_RADIO_SOURCES, wxT("Radio Sources"), pRadioObjectsSubMenu );

	////////////////////
	// :: VIEW :: multiple/double filter submenu
	wxMenu* pMStarsSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pMStarsSubMenu->AppendRadioItem( wxID_MENU_VIEW_MULTIPLE_STARS_NONE, wxT("None"));
	pMStarsSubMenu->AppendRadioItem( wxID_MENU_VIEW_MULTIPLE_STARS_50, wxT("50"));
	pMStarsSubMenu->AppendRadioItem( wxID_MENU_VIEW_MULTIPLE_STARS_100, wxT("100"));
	pMStarsSubMenu->AppendRadioItem( wxID_MENU_VIEW_MULTIPLE_STARS_200, wxT("200"));
	pMStarsSubMenu->AppendRadioItem( wxID_MENU_VIEW_MULTIPLE_STARS_ALL, wxT("All"));
	pMStarsSubMenu->Check( wxID_MENU_VIEW_MULTIPLE_STARS_50, 1 );	
	pViewMenu->Append( wxID_MENU_VIEW_MULTIPLE_STARS, wxT("Multiple Stars"), pMStarsSubMenu );

	////////////////////
	// :: VIEW :: supernovas filter submenu
	wxMenu* pSupernovasSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pSupernovasSubMenu->AppendRadioItem( wxID_MENU_VIEW_SUPERNOVAS_NONE, wxT("None"));
	pSupernovasSubMenu->AppendRadioItem( wxID_MENU_VIEW_SUPERNOVAS_50, wxT("50"));
	pSupernovasSubMenu->AppendRadioItem( wxID_MENU_VIEW_SUPERNOVAS_100, wxT("100"));
	pSupernovasSubMenu->AppendRadioItem( wxID_MENU_VIEW_SUPERNOVAS_200, wxT("200"));
	pSupernovasSubMenu->AppendRadioItem( wxID_MENU_VIEW_SUPERNOVAS_ALL, wxT("All"));
	pSupernovasSubMenu->Check( wxID_MENU_VIEW_SUPERNOVAS_50, 1 );	
	pViewMenu->Append( wxID_MENU_VIEW_SUPERNOVAS, wxT("Supernovas"), pSupernovasSubMenu );

	////////////////////
	// :: VIEW :: blackholes filter submenu
	wxMenu* pBlackholesSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pBlackholesSubMenu->AppendRadioItem( wxID_MENU_VIEW_BLACKHOLES_NONE, wxT("None"));
	pBlackholesSubMenu->AppendRadioItem( wxID_MENU_VIEW_BLACKHOLES_50, wxT("50"));
	pBlackholesSubMenu->AppendRadioItem( wxID_MENU_VIEW_BLACKHOLES_100, wxT("100"));
	pBlackholesSubMenu->AppendRadioItem( wxID_MENU_VIEW_BLACKHOLES_200, wxT("200"));
	pBlackholesSubMenu->AppendRadioItem( wxID_MENU_VIEW_BLACKHOLES_ALL, wxT("All"));
	pBlackholesSubMenu->Check( wxID_MENU_VIEW_BLACKHOLES_50, 1 );	
	pViewMenu->Append( wxID_MENU_VIEW_BLACKHOLES, wxT("Blackholes"), pBlackholesSubMenu );

	////////////////////
	// :: VIEW :: exo-planets filter submenu
	wxMenu* pExoPlanetsSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pExoPlanetsSubMenu->AppendRadioItem( wxID_MENU_VIEW_EXO_PLANETS_NONE, wxT("None"));
	pExoPlanetsSubMenu->AppendRadioItem( wxID_MENU_VIEW_EXO_PLANETS_50, wxT("50"));
	pExoPlanetsSubMenu->AppendRadioItem( wxID_MENU_VIEW_EXO_PLANETS_100, wxT("100"));
	pExoPlanetsSubMenu->AppendRadioItem( wxID_MENU_VIEW_EXO_PLANETS_200, wxT("200"));
	pExoPlanetsSubMenu->AppendRadioItem( wxID_MENU_VIEW_EXO_PLANETS_ALL, wxT("All"));
	pExoPlanetsSubMenu->Check( wxID_MENU_VIEW_EXO_PLANETS_50, 1 );	
	pViewMenu->Append( wxID_MENU_VIEW_EXO_PLANETS, wxT("Exo-Planets"), pExoPlanetsSubMenu );

	////////////////////
	// :: VIEW :: asteroids filter submenu
	wxMenu* pAsteroidsSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pAsteroidsSubMenu->AppendRadioItem( wxID_MENU_VIEW_ASTEROIDS_NONE, wxT("None"));
	pAsteroidsSubMenu->AppendRadioItem( wxID_MENU_VIEW_ASTEROIDS_50, wxT("50"));
	pAsteroidsSubMenu->AppendRadioItem( wxID_MENU_VIEW_ASTEROIDS_100, wxT("100"));
	pAsteroidsSubMenu->AppendRadioItem( wxID_MENU_VIEW_ASTEROIDS_200, wxT("200"));
	pAsteroidsSubMenu->AppendRadioItem( wxID_MENU_VIEW_ASTEROIDS_ALL, wxT("All"));
	pAsteroidsSubMenu->Check( wxID_MENU_VIEW_ASTEROIDS_50, 1 );	
	pViewMenu->Append( wxID_MENU_VIEW_ASTEROIDS, wxT("Asteroids"), pAsteroidsSubMenu );

	////////////////////
	// :: VIEW :: comets filter submenu
	wxMenu* pCometsSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pCometsSubMenu->AppendRadioItem( wxID_MENU_VIEW_COMETS_NONE, wxT("None"));
	pCometsSubMenu->AppendRadioItem( wxID_MENU_VIEW_COMETS_50, wxT("50"));
	pCometsSubMenu->AppendRadioItem( wxID_MENU_VIEW_COMETS_100, wxT("100"));
	pCometsSubMenu->AppendRadioItem( wxID_MENU_VIEW_COMETS_200, wxT("200"));
	pCometsSubMenu->AppendRadioItem( wxID_MENU_VIEW_COMETS_ALL, wxT("All"));
	pCometsSubMenu->Check( wxID_MENU_VIEW_COMETS_50, 1 );	
	pViewMenu->Append( wxID_MENU_VIEW_COMETS, wxT("Comets"), pCometsSubMenu );

	////////////////////
	// :: VIEW :: AES filter submenu
	wxMenu* pAESSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pAESSubMenu->AppendRadioItem( wxID_MENU_VIEW_SATELLITES_NONE, wxT("None"));
	pAESSubMenu->AppendRadioItem( wxID_MENU_VIEW_SATELLITES_50, wxT("50"));
	pAESSubMenu->AppendRadioItem( wxID_MENU_VIEW_SATELLITES_100, wxT("100"));
	pAESSubMenu->AppendRadioItem( wxID_MENU_VIEW_SATELLITES_200, wxT("200"));
	pAESSubMenu->AppendRadioItem( wxID_MENU_VIEW_SATELLITES_ALL, wxT("All"));
	pAESSubMenu->Check( wxID_MENU_VIEW_SATELLITES_50, 1 );	
	pViewMenu->Append( wxID_MENU_VIEW_SATELLITES, wxT("Satellites"), pAESSubMenu );

*/

/***************************
////////////////////////////////////////////////////////////////////
// Method:		OnViewObjects
// Class:		CGUIFrame
// Purpose:		set the number of objects by type to be showen
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnViewObjects( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();

	if( nId >= wxID_MENU_VIEW_DETECTED && nId <= wxID_MENU_VIEW_DETECT_CUSTOM )
		m_pImgView->SetStars( m_pAstroImage, GetViewObjects( nId ) );
	else if( nId >= wxID_MENU_VIEW_CATALOG_STARS && nId <= wxID_MENU_VIEW_CATALOG_STARS_ALL )
		m_pImgView->SetCatStars( m_pAstroImage, GetViewObjects( nId ) );
	else if( nId >= wxID_MENU_VIEW_DSO_OBJECTS && nId <= wxID_MENU_VIEW_DSO_OBJECTS_ALL )
		m_pImgView->SetDsoObjects( m_pAstroImage, GetViewObjects( nId ) );
	else if( nId >= wxID_MENU_VIEW_GAMMA_XRAY && nId <= wxID_MENU_VIEW_GAMMA_XRAY_ALL )
		m_pImgView->SetXGammaSources( m_pAstroImage, GetViewObjects( nId ) );
	else if( nId >= wxID_MENU_VIEW_RADIO_SOURCES && nId <= wxID_MENU_VIEW_RADIO_SOURCES_ALL )
		m_pImgView->SetRadioSources( m_pAstroImage, GetViewObjects( nId ) );
	else if( nId >= wxID_MENU_VIEW_MULTIPLE_STARS && nId <= wxID_MENU_VIEW_MULTIPLE_STARS_ALL )
		m_pImgView->SetMStars( m_pAstroImage, GetViewObjects( nId ) );
	else if( nId >= wxID_MENU_VIEW_SUPERNOVAS && nId <= wxID_MENU_VIEW_SUPERNOVAS_ALL )
		m_pImgView->SetSupernovas( m_pAstroImage, GetViewObjects( nId ) );
	else if( nId >= wxID_MENU_VIEW_BLACKHOLES && nId <= wxID_MENU_VIEW_BLACKHOLES_ALL )
		m_pImgView->SetBlackholes( m_pAstroImage, GetViewObjects( nId ) );
	else if( nId >= wxID_MENU_VIEW_EXO_PLANETS && nId <= wxID_MENU_VIEW_EXO_PLANETS_ALL )
		m_pImgView->SetExoplanets( m_pAstroImage, GetViewObjects( nId ) );
	else if( nId >= wxID_MENU_VIEW_ASTEROIDS && nId <= wxID_MENU_VIEW_ASTEROIDS_ALL )
		m_pImgView->SetAsteroids( m_pAstroImage, GetViewObjects( nId ) );
	else if( nId >= wxID_MENU_VIEW_COMETS && nId <= wxID_MENU_VIEW_COMETS_ALL )
		m_pImgView->SetComets( m_pAstroImage, GetViewObjects( nId ) );
	else if( nId >= wxID_MENU_VIEW_SATELLITES && nId <= wxID_MENU_VIEW_SATELLITES_ALL )
		m_pImgView->SetAes( m_pAstroImage, GetViewObjects( nId ) );

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnViewDetectNoMatch( wxCommandEvent& pEvent )
{
	if( pEvent.IsChecked( ) )
		m_pMainConfig->m_bViewDetectedObjectsNoMatch = 1;
	else
		m_pMainConfig->m_bViewDetectedObjectsNoMatch = 0;

	// re-display objects
	m_pImgView->SetStars( m_pAstroImage, m_pMainConfig->m_nViewDetectedObjects );

	return;
}

*/

/**********************************

	/////////////////////////////////////
	// :: VIEW :: set view detected objects menu
	if( m_pMainConfig->m_nViewDetectedObjects == 0 )
		pViewMenu->Check( wxID_MENU_VIEW_DETECT_NONE, 1 );
	else if( m_pMainConfig->m_nViewDetectedObjects == 50 )
		pViewMenu->Check( wxID_MENU_VIEW_DETECT_50, 1 );
	else if( m_pMainConfig->m_nViewDetectedObjects == 100 )
		pViewMenu->Check( wxID_MENU_VIEW_DETECT_100, 1 );
	else if( m_pMainConfig->m_nViewDetectedObjects == 200 )
		pViewMenu->Check( wxID_MENU_VIEW_DETECT_200, 1 );
	else if( m_pMainConfig->m_nViewDetectedObjects == -1 )
		pViewMenu->Check( wxID_MENU_VIEW_DETECT_ALL, 1 );
	// :: VIEW :: set view catalog stars menu
	if( m_pMainConfig->m_bViewDetectedObjectsNoMatch == 0 )
		pViewMenu->Check( wxID_MENU_VIEW_DETECT_NO_MATCH, 0 );
	else
		pViewMenu->Check( wxID_MENU_VIEW_DETECT_NO_MATCH, 1 );
	// :: VIEW :: set view catalog stars menu
	if( m_pMainConfig->m_nViewCatalogStars == 0 )
		pViewMenu->Check( wxID_MENU_VIEW_CATALOG_STARS_NONE, 1 );
	else if( m_pMainConfig->m_nViewCatalogStars == 50 )
		pViewMenu->Check( wxID_MENU_VIEW_CATALOG_STARS_50, 1 );
	else if( m_pMainConfig->m_nViewCatalogStars == 100 )
		pViewMenu->Check( wxID_MENU_VIEW_CATALOG_STARS_100, 1 );
	else if( m_pMainConfig->m_nViewCatalogStars == 200 )
		pViewMenu->Check( wxID_MENU_VIEW_CATALOG_STARS_200, 1 );
	else if( m_pMainConfig->m_nViewCatalogStars == -1 )
		pViewMenu->Check( wxID_MENU_VIEW_CATALOG_STARS_ALL, 1 );
	// :: VIEW :: set view catalog DSO menu
	if( m_pMainConfig->m_nViewCatalogDso == 0 )
		pViewMenu->Check( wxID_MENU_VIEW_DSO_OBJECTS_NONE, 1 );
	else if( m_pMainConfig->m_nViewCatalogDso == 50 )
		pViewMenu->Check( wxID_MENU_VIEW_DSO_OBJECTS_50, 1 );
	else if( m_pMainConfig->m_nViewCatalogDso == 100 )
		pViewMenu->Check( wxID_MENU_VIEW_DSO_OBJECTS_100, 1 );
	else if( m_pMainConfig->m_nViewCatalogDso == 200 )
		pViewMenu->Check( wxID_MENU_VIEW_DSO_OBJECTS_200, 1 );
	else if( m_pMainConfig->m_nViewCatalogDso == -1 )
		pViewMenu->Check( wxID_MENU_VIEW_DSO_OBJECTS_ALL, 1 );
	// :: VIEW :: set view catalog XGAMMA menu
	if( m_pMainConfig->m_nViewCatalogXGamma == 0 )
		pViewMenu->Check( wxID_MENU_VIEW_GAMMA_XRAY_NONE, 1 );
	else if( m_pMainConfig->m_nViewCatalogXGamma == 50 )
		pViewMenu->Check( wxID_MENU_VIEW_GAMMA_XRAY_50, 1 );
	else if( m_pMainConfig->m_nViewCatalogXGamma == 100 )
		pViewMenu->Check( wxID_MENU_VIEW_GAMMA_XRAY_100, 1 );
	else if( m_pMainConfig->m_nViewCatalogXGamma == 200 )
		pViewMenu->Check( wxID_MENU_VIEW_GAMMA_XRAY_200, 1 );
	else if( m_pMainConfig->m_nViewCatalogXGamma == -1 )
		pViewMenu->Check( wxID_MENU_VIEW_GAMMA_XRAY_ALL, 1 );
	// :: VIEW :: set view catalog RADIO menu
	if( m_pMainConfig->m_nViewCatalogRadio == 0 )
		pViewMenu->Check( wxID_MENU_VIEW_RADIO_SOURCES_NONE, 1 );
	else if( m_pMainConfig->m_nViewCatalogRadio == 50 )
		pViewMenu->Check( wxID_MENU_VIEW_RADIO_SOURCES_50, 1 );
	else if( m_pMainConfig->m_nViewCatalogRadio == 100 )
		pViewMenu->Check( wxID_MENU_VIEW_RADIO_SOURCES_100, 1 );
	else if( m_pMainConfig->m_nViewCatalogRadio == 200 )
		pViewMenu->Check( wxID_MENU_VIEW_RADIO_SOURCES_200, 1 );
	else if( m_pMainConfig->m_nViewCatalogRadio == -1 )
		pViewMenu->Check( wxID_MENU_VIEW_RADIO_SOURCES_ALL, 1 );
	// :: VIEW :: set view catalog MULTIPLE STARS menu
	if( m_pMainConfig->m_nViewCatalogMStars == 0 )
		pViewMenu->Check( wxID_MENU_VIEW_MULTIPLE_STARS_NONE, 1 );
	else if( m_pMainConfig->m_nViewCatalogMStars == 50 )
		pViewMenu->Check( wxID_MENU_VIEW_MULTIPLE_STARS_50, 1 );
	else if( m_pMainConfig->m_nViewCatalogMStars == 100 )
		pViewMenu->Check( wxID_MENU_VIEW_MULTIPLE_STARS_100, 1 );
	else if( m_pMainConfig->m_nViewCatalogMStars == 200 )
		pViewMenu->Check( wxID_MENU_VIEW_MULTIPLE_STARS_200, 1 );
	else if( m_pMainConfig->m_nViewCatalogMStars == -1 )
		pViewMenu->Check( wxID_MENU_VIEW_MULTIPLE_STARS_ALL, 1 );
	// :: VIEW :: set view catalog SUPERNOVAS menu
	if( m_pMainConfig->m_nViewCatalogSupernovas == 0 )
		pViewMenu->Check( wxID_MENU_VIEW_SUPERNOVAS_NONE, 1 );
	else if( m_pMainConfig->m_nViewCatalogSupernovas == 50 )
		pViewMenu->Check( wxID_MENU_VIEW_SUPERNOVAS_50, 1 );
	else if( m_pMainConfig->m_nViewCatalogSupernovas == 100 )
		pViewMenu->Check( wxID_MENU_VIEW_SUPERNOVAS_100, 1 );
	else if( m_pMainConfig->m_nViewCatalogSupernovas == 200 )
		pViewMenu->Check( wxID_MENU_VIEW_SUPERNOVAS_200, 1 );
	else if( m_pMainConfig->m_nViewCatalogSupernovas == -1 )
		pViewMenu->Check( wxID_MENU_VIEW_SUPERNOVAS_ALL, 1 );
	// :: VIEW :: set view catalog BLACKHOLES menu
	if( m_pMainConfig->m_nViewCatalogBlackholes == 0 )
		pViewMenu->Check( wxID_MENU_VIEW_BLACKHOLES_NONE, 1 );
	else if( m_pMainConfig->m_nViewCatalogBlackholes == 50 )
		pViewMenu->Check( wxID_MENU_VIEW_BLACKHOLES_50, 1 );
	else if( m_pMainConfig->m_nViewCatalogBlackholes == 100 )
		pViewMenu->Check( wxID_MENU_VIEW_BLACKHOLES_100, 1 );
	else if( m_pMainConfig->m_nViewCatalogBlackholes == 200 )
		pViewMenu->Check( wxID_MENU_VIEW_BLACKHOLES_200, 1 );
	else if( m_pMainConfig->m_nViewCatalogBlackholes == -1 )
		pViewMenu->Check( wxID_MENU_VIEW_BLACKHOLES_ALL, 1 );
	// :: VIEW :: set view catalog EXO PLANETS menu
	if( m_pMainConfig->m_nViewCatalogExoplanets == 0 )
		pViewMenu->Check( wxID_MENU_VIEW_EXO_PLANETS_NONE, 1 );
	else if( m_pMainConfig->m_nViewCatalogExoplanets == 50 )
		pViewMenu->Check( wxID_MENU_VIEW_EXO_PLANETS_50, 1 );
	else if( m_pMainConfig->m_nViewCatalogExoplanets == 100 )
		pViewMenu->Check( wxID_MENU_VIEW_EXO_PLANETS_100, 1 );
	else if( m_pMainConfig->m_nViewCatalogExoplanets == 200 )
		pViewMenu->Check( wxID_MENU_VIEW_EXO_PLANETS_200, 1 );
	else if( m_pMainConfig->m_nViewCatalogExoplanets == -1 )
		pViewMenu->Check( wxID_MENU_VIEW_EXO_PLANETS_ALL, 1 );
	// :: VIEW :: set view catalog ASTEROIDS menu
	if( m_pMainConfig->m_nViewCatalogAsteroids == 0 )
		pViewMenu->Check( wxID_MENU_VIEW_ASTEROIDS_NONE, 1 );
	else if( m_pMainConfig->m_nViewCatalogAsteroids == 50 )
		pViewMenu->Check( wxID_MENU_VIEW_ASTEROIDS_50, 1 );
	else if( m_pMainConfig->m_nViewCatalogAsteroids == 100 )
		pViewMenu->Check( wxID_MENU_VIEW_ASTEROIDS_100, 1 );
	else if( m_pMainConfig->m_nViewCatalogAsteroids == 200 )
		pViewMenu->Check( wxID_MENU_VIEW_ASTEROIDS_200, 1 );
	else if( m_pMainConfig->m_nViewCatalogAsteroids == -1 )
		pViewMenu->Check( wxID_MENU_VIEW_ASTEROIDS_ALL, 1 );
	// :: VIEW :: set view catalog COMETS menu
	if( m_pMainConfig->m_nViewCatalogComets == 0 )
		pViewMenu->Check( wxID_MENU_VIEW_COMETS_NONE, 1 );
	else if( m_pMainConfig->m_nViewCatalogComets == 50 )
		pViewMenu->Check( wxID_MENU_VIEW_COMETS_50, 1 );
	else if( m_pMainConfig->m_nViewCatalogComets == 100 )
		pViewMenu->Check( wxID_MENU_VIEW_COMETS_100, 1 );
	else if( m_pMainConfig->m_nViewCatalogComets == 200 )
		pViewMenu->Check( wxID_MENU_VIEW_COMETS_200, 1 );
	else if( m_pMainConfig->m_nViewCatalogComets == -1 )
		pViewMenu->Check( wxID_MENU_VIEW_COMETS_ALL, 1 );
	// :: VIEW :: set view catalog ARTIFICIAL EARTH SATELLITES menu
	if( m_pMainConfig->m_nViewCatalogAes == 0 )
		pViewMenu->Check( wxID_MENU_VIEW_SATELLITES_NONE, 1 );
	else if( m_pMainConfig->m_nViewCatalogAes == 50 )
		pViewMenu->Check( wxID_MENU_VIEW_SATELLITES_50, 1 );
	else if( m_pMainConfig->m_nViewCatalogAes == 100 )
		pViewMenu->Check( wxID_MENU_VIEW_SATELLITES_100, 1 );
	else if( m_pMainConfig->m_nViewCatalogAes == 200 )
		pViewMenu->Check( wxID_MENU_VIEW_SATELLITES_200, 1 );
	else if( m_pMainConfig->m_nViewCatalogAes == -1 )
		pViewMenu->Check( wxID_MENU_VIEW_SATELLITES_ALL, 1 );

*/

/*

////////////////////////////////////////////////////////////////////
// Method:		GetViewObjects
// Class:		CGUIFrame
// Purpose:		Get how many object to view from the menu view
// Input:		menu id originating
// Output:		no of stars to view -1 = all
////////////////////////////////////////////////////////////////////
int CGUIFrame::GetViewObjects( int nMenuId )
{
	// 1 = menu index for vie
	wxMenu* pMenu = GetMenuBar()->GetMenu( 1 );

	int nObjectsView = 0;

	if( nMenuId >= wxID_MENU_VIEW_DETECTED && nMenuId <= wxID_MENU_VIEW_DETECT_CUSTOM )
	{
		if( pMenu->IsChecked( wxID_MENU_VIEW_DETECT_NONE ) )
			nObjectsView = 0;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_DETECT_50 ) )
			nObjectsView = 50;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_DETECT_100 ) )
			nObjectsView = 100;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_DETECT_200 ) )
			nObjectsView = 200;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_DETECT_ALL ) )
			nObjectsView = -1;
		m_pMainConfig->m_nViewDetectedObjects = nObjectsView;

	} else if( nMenuId >= wxID_MENU_VIEW_CATALOG_STARS && nMenuId <= wxID_MENU_VIEW_CATALOG_STARS_ALL )
	{
		if( pMenu->IsChecked( wxID_MENU_VIEW_CATALOG_STARS_NONE ) )
			nObjectsView = 0;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_CATALOG_STARS_50 ) )
			nObjectsView = 50;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_CATALOG_STARS_100 ) )
			nObjectsView = 100;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_CATALOG_STARS_200 ) )
			nObjectsView = 200;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_CATALOG_STARS_ALL ) )
			nObjectsView = -1;
		m_pMainConfig->m_nViewCatalogStars = nObjectsView;

	} else if( nMenuId >= wxID_MENU_VIEW_DSO_OBJECTS && nMenuId <= wxID_MENU_VIEW_DSO_OBJECTS_ALL )
	{
		if( pMenu->IsChecked( wxID_MENU_VIEW_DSO_OBJECTS_NONE ) )
			nObjectsView = 0;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_DSO_OBJECTS_50 ) )
			nObjectsView = 50;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_DSO_OBJECTS_100 ) )
			nObjectsView = 100;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_DSO_OBJECTS_200 ) )
			nObjectsView = 200;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_DSO_OBJECTS_ALL ) )
			nObjectsView = -1;
		m_pMainConfig->m_nViewCatalogDso = nObjectsView;
	
	} else if( nMenuId >= wxID_MENU_VIEW_GAMMA_XRAY && nMenuId <= wxID_MENU_VIEW_GAMMA_XRAY_ALL )
	{
		if( pMenu->IsChecked( wxID_MENU_VIEW_GAMMA_XRAY_NONE ) )
			nObjectsView = 0;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_GAMMA_XRAY_50 ) )
			nObjectsView = 50;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_GAMMA_XRAY_100 ) )
			nObjectsView = 100;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_GAMMA_XRAY_200 ) )
			nObjectsView = 200;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_GAMMA_XRAY_ALL ) )
			nObjectsView = -1;
		m_pMainConfig->m_nViewCatalogXGamma = nObjectsView;

	} else if( nMenuId >= wxID_MENU_VIEW_RADIO_SOURCES && nMenuId <= wxID_MENU_VIEW_RADIO_SOURCES_ALL )
	{
		if( pMenu->IsChecked( wxID_MENU_VIEW_RADIO_SOURCES_NONE ) )
			nObjectsView = 0;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_RADIO_SOURCES_50 ) )
			nObjectsView = 50;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_RADIO_SOURCES_100 ) )
			nObjectsView = 100;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_RADIO_SOURCES_200 ) )
			nObjectsView = 200;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_RADIO_SOURCES_ALL ) )
			nObjectsView = -1;
		m_pMainConfig->m_nViewCatalogRadio = nObjectsView;

	} else if( nMenuId >= wxID_MENU_VIEW_MULTIPLE_STARS && nMenuId <= wxID_MENU_VIEW_MULTIPLE_STARS_ALL )
	{
		if( pMenu->IsChecked( wxID_MENU_VIEW_MULTIPLE_STARS_NONE ) )
			nObjectsView = 0;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_MULTIPLE_STARS_50 ) )
			nObjectsView = 50;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_MULTIPLE_STARS_100 ) )
			nObjectsView = 100;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_MULTIPLE_STARS_200 ) )
			nObjectsView = 200;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_MULTIPLE_STARS_ALL ) )
			nObjectsView = -1;
		m_pMainConfig->m_nViewCatalogMStars = nObjectsView;

	} else if( nMenuId >= wxID_MENU_VIEW_SUPERNOVAS && nMenuId <= wxID_MENU_VIEW_SUPERNOVAS_ALL )
	{
		if( pMenu->IsChecked( wxID_MENU_VIEW_SUPERNOVAS_NONE ) )
			nObjectsView = 0;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_SUPERNOVAS_50 ) )
			nObjectsView = 50;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_SUPERNOVAS_100 ) )
			nObjectsView = 100;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_SUPERNOVAS_200 ) )
			nObjectsView = 200;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_SUPERNOVAS_ALL ) )
			nObjectsView = -1;
		m_pMainConfig->m_nViewCatalogSupernovas = nObjectsView;

	} else if( nMenuId >= wxID_MENU_VIEW_BLACKHOLES && nMenuId <= wxID_MENU_VIEW_BLACKHOLES_ALL )
	{
		if( pMenu->IsChecked( wxID_MENU_VIEW_BLACKHOLES_NONE ) )
			nObjectsView = 0;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_BLACKHOLES_50 ) )
			nObjectsView = 50;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_BLACKHOLES_100 ) )
			nObjectsView = 100;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_BLACKHOLES_200 ) )
			nObjectsView = 200;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_BLACKHOLES_ALL ) )
			nObjectsView = -1;
		m_pMainConfig->m_nViewCatalogBlackholes = nObjectsView;

	} else if( nMenuId >= wxID_MENU_VIEW_EXO_PLANETS && nMenuId <= wxID_MENU_VIEW_EXO_PLANETS_ALL )
	{
		if( pMenu->IsChecked( wxID_MENU_VIEW_EXO_PLANETS_NONE ) )
			nObjectsView = 0;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_EXO_PLANETS_50 ) )
			nObjectsView = 50;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_EXO_PLANETS_100 ) )
			nObjectsView = 100;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_EXO_PLANETS_200 ) )
			nObjectsView = 200;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_EXO_PLANETS_ALL ) )
			nObjectsView = -1;
		m_pMainConfig->m_nViewCatalogExoplanets = nObjectsView;

	} else if( nMenuId >= wxID_MENU_VIEW_ASTEROIDS && nMenuId <= wxID_MENU_VIEW_ASTEROIDS_ALL )
	{
		if( pMenu->IsChecked( wxID_MENU_VIEW_ASTEROIDS_NONE ) )
			nObjectsView = 0;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_ASTEROIDS_50 ) )
			nObjectsView = 50;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_ASTEROIDS_100 ) )
			nObjectsView = 100;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_ASTEROIDS_200 ) )
			nObjectsView = 200;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_ASTEROIDS_ALL ) )
			nObjectsView = -1;
		m_pMainConfig->m_nViewCatalogAsteroids = nObjectsView;

	} else if( nMenuId >= wxID_MENU_VIEW_COMETS && nMenuId <= wxID_MENU_VIEW_COMETS_ALL )
	{
		if( pMenu->IsChecked( wxID_MENU_VIEW_COMETS_NONE ) )
			nObjectsView = 0;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_COMETS_50 ) )
			nObjectsView = 50;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_COMETS_100 ) )
			nObjectsView = 100;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_COMETS_200 ) )
			nObjectsView = 200;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_COMETS_ALL ) )
			nObjectsView = -1;
		m_pMainConfig->m_nViewCatalogComets = nObjectsView;

	} else if( nMenuId >= wxID_MENU_VIEW_SATELLITES && nMenuId <= wxID_MENU_VIEW_SATELLITES_ALL )
	{
		if( pMenu->IsChecked( wxID_MENU_VIEW_SATELLITES_NONE ) )
			nObjectsView = 0;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_SATELLITES_50 ) )
			nObjectsView = 50;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_SATELLITES_100 ) )
			nObjectsView = 100;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_SATELLITES_200 ) )
			nObjectsView = 200;
		else if( pMenu->IsChecked( wxID_MENU_VIEW_SATELLITES_ALL ) )
			nObjectsView = -1;
		m_pMainConfig->m_nViewCatalogAes = nObjectsView;
	}

	return( nObjectsView );
}

*/
