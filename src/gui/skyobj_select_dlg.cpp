////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// local headers
#include "../util/func.h"
#include "../sky/sky.h"
#include "../sky/catalog.h"
#include "../sky/catalog_stars.h"
#include "../sky/catalog_stars_names.h"
#include "../sky/catalog_dso.h"
#include "../sky/catalog_dso_ngc.h"
#include "../sky/catalog_dso_ic.h"
#include "../sky/catalog_dso_messier.h"
#include "../config/mainconfig.h"

// main header
#include "skyobj_select_dlg.h"

//		***** CLASS CSkyObjSelectDlg *****
////////////////////////////////////////////////////////////////////
// CSkyObjSelectDlg EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CSkyObjSelectDlg, wxDialog)
	EVT_CHOICE( wxID_SKY_OBJ_CAT_SELECT, CSkyObjSelectDlg::OnSelectType )
	EVT_CHOICE( wxID_SKY_OBJ_NAME_SELECT, CSkyObjSelectDlg::OnSelectName )
	EVT_TEXT( wxID_SKY_OBJ_NAME_ENTRY, CSkyObjSelectDlg::OnSetObjectId )
END_EVENT_TABLE()

// Constructor/Destructor
////////////////////////////////////////////////////////////////////
CSkyObjSelectDlg::CSkyObjSelectDlg( wxWindow *parent, const wxString& title ) : wxDialog(parent, -1,
                          title,
                          wxDefaultPosition,
                          wxDefaultSize,
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL)
{
	m_nSkyObjType = 0;
	m_nRaDeg = 0.0;
	m_nDecDeg = 0.0;

	// define object/source filter selection
	wxString vectObjFilter[3];
	vectObjFilter[0] = _T("All");
	vectObjFilter[1] = _T("Visible");
	// init selection for sky object sources
	wxString vectObjReference[10];
	vectObjReference[0] = _T("Star Common Name");
	vectObjReference[1] = _T("Dso Common Name");
	vectObjReference[2] = _T("Constellation Names");
	vectObjReference[3] = _T("Messier Id");
	vectObjReference[4] = _T("Ngc Id");
	vectObjReference[5] = _T("Ic Id");
	vectObjReference[6] = _T("Sao Id");

	// main sizer
	wxBoxSizer *pTopSizer = new wxBoxSizer( wxVERTICAL );

	/////////////////////////////
	// data sizer
	wxFlexGridSizer* dataSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
//	headSizer->AddGrowableCol( 1 );
	// data sizer A
	wxFlexGridSizer* dataSizerA = new wxFlexGridSizer( 5, 2, 5, 5 );
	dataSizer->Add( dataSizerA, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );
	// data static box
	wxStaticBox* pDataBox = new wxStaticBox( this, -1, _("Info"), wxPoint(-1,-1), wxSize(-1,-1) );
	wxStaticBoxSizer* sizerDataBox = new wxStaticBoxSizer( pDataBox, wxHORIZONTAL );
	dataSizer->Add( sizerDataBox, 1, wxGROW|wxEXPAND|wxTOP, 10 );
	// data sizer B
	wxFlexGridSizer* dataSizerB = new wxFlexGridSizer( 3, 2, 5, 5 );
	dataSizerB->AddGrowableCol( 1 );
	dataSizerB->AddGrowableCol( 0 );
	sizerDataBox->Add( dataSizerB, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );

	//////////
	// object cat select
	// :: object/source filter
	m_pObjFilter = new wxChoice( this, wxID_SKY_OBJ_FILTER_SELECT,
										wxDefaultPosition, wxSize(75,-1), 2, vectObjFilter,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pObjFilter->SetSelection( 0 );
	// :: object/source category
	m_pObjCatSelect = new wxChoice( this, wxID_SKY_OBJ_CAT_SELECT,
										wxDefaultPosition, wxSize(150,-1), 7, vectObjReference,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pObjCatSelect->SetSelection( 0 );
	// :: object/source name
	m_pObjectLabel = new wxStaticText( this, -1, _T("Object Name:") );
	m_pObjNameSelect = new wxChoice( this, wxID_SKY_OBJ_NAME_SELECT,
										wxDefaultPosition, wxSize(150,-1), 0, NULL,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pObjNameSelect->SetSelection( 0 );

	m_pObjectId = new wxTextCtrl( this, wxID_SKY_OBJ_NAME_ENTRY, _T(""), wxDefaultPosition, wxSize(150,-1) );
	m_pObjectId->Hide();
	// :: ra view
	m_pObjectRA = new wxStaticText( this, -1, _T("00:00:00.000") );
	m_pObjectRA->SetForegroundColour( *wxRED );
	// :: dec view
	m_pObjectDEC = new wxStaticText( this, -1, _T("+00:00:00.00") );
	m_pObjectDEC->SetForegroundColour( *wxRED );
	// :: mag view
	m_pObjectMAG = new wxStaticText( this, -1, _T("0.0000") );
	m_pObjectMAG->SetForegroundColour( *wxRED );

	///////////////////////////////
	// sizer layout
	// :: object filter
	dataSizerA->Add( new wxStaticText( this, -1, _T("Filter By:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	dataSizerA->Add( m_pObjFilter, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: object type
	dataSizerA->Add( new wxStaticText( this, -1, _T("Object Source:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	dataSizerA->Add( m_pObjCatSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: object name
	dataSizerA->Add( m_pObjectLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* dataSizerName = new wxFlexGridSizer( 1,3,0,0 );
	dataSizerName->Add( m_pObjNameSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	dataSizerName->Add( m_pObjectId, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	dataSizerA->Add( dataSizerName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: data view :: ra
	dataSizerB->Add( new wxStaticText( this, -1, _T("Right Ascension :")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	dataSizerB->Add( m_pObjectRA, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: data view :: dec
	dataSizerB->Add( new wxStaticText( this, -1, _T("Declination:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	dataSizerB->Add( m_pObjectDEC, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: data view :: mag
	dataSizerB->Add( new wxStaticText( this, -1, _T("Magnitude:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	dataSizerB->Add( m_pObjectMAG, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// add to main sizer
	pTopSizer->Add( dataSizer, 1, wxGROW | wxALL, 10 );
	// add buttons
	pTopSizer->Add( CreateButtonSizer(wxOK|wxCANCEL), 0, wxCENTRE|wxALL, 10 );

	// activate
	SetSizer( pTopSizer );
	SetAutoLayout(TRUE);
	pTopSizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyObjSelectDlg
// Purpose:	Delete my object
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CSkyObjSelectDlg::~CSkyObjSelectDlg( )
{
	delete( m_pObjFilter );
	delete( m_pObjCatSelect );
	delete( m_pObjectLabel );
	delete( m_pObjNameSelect );
	delete( m_pObjectId );
	delete( m_pObjectRA );
	delete( m_pObjectDEC );
	delete( m_pObjectMAG );
}

////////////////////////////////////////////////////////////////////
void CSkyObjSelectDlg::SetConfig( CSky* pSky )
{
	m_pSky = pSky;

	// default to zero ?
	SetObjectNameList( 0 );
	SetOrig( );

	Refresh( FALSE );
	Update( );
}

// GUI handlers
////////////////////////////////////////////////////////////////////
void CSkyObjSelectDlg::OnSelectType( wxCommandEvent& event )
{
	SetSelectType( );

	// reset layout
//	sizerOrigAPanel->Layout( );
	GetSizer()->Layout( );
	Fit( );
	Layout( );

	return;
}

////////////////////////////////////////////////////////////////////
void CSkyObjSelectDlg::SetSelectType( )
{
	int nSelect = m_pObjCatSelect->GetSelection();

	if( nSelect == SKYOBJ_TYPE_STAR_NAMES ||
		nSelect == SKYOBJ_TYPE_DSO_NAMES ||
		nSelect == SKYOBJ_TYPE_CONST_NAMES )
	{
		// hide id entry
		m_pObjectId->Hide();
		// show name entry
		m_pObjectLabel->SetLabel( wxT("Object Name:") );
		m_pObjNameSelect->Show();

		// and set the list of object in the next select
		SetObjectNameList( nSelect );

	} else if( nSelect == SKYOBJ_TYPE_MESSIER_ID ||
				nSelect == SKYOBJ_TYPE_NGC_ID ||
				nSelect == SKYOBJ_TYPE_IC_ID ||
				nSelect == SKYOBJ_TYPE_SAO_ID )
	{
		// show id entry
		m_pObjectLabel->SetLabel( wxT("Object Id:") );
		m_pObjectId->Show();
		// hide Name entry
		m_pObjNameSelect->Hide();
		//special case
		if( nSelect == SKYOBJ_TYPE_SAO_ID )
		{
			m_pSky->m_pCatalogStars->LoadCatalog( CATALOG_ID_SAO ); 
		}

		// set info box
		m_nRaDeg = 0.0; m_nDecDeg = 0.0;
		SetOrig( );
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CSkyObjSelectDlg::SetObjectNameList( int nType )
{
	int i = 0;

	if( nType == SKYOBJ_TYPE_DSO_NAMES )
	{
		wxArrayString vecObj;
		vecObj.Alloc( m_pSky->m_pCatalogDsoNames->m_nDsoName+1 );

		// populate list from catalog entries
		for( i=0; i<m_pSky->m_pCatalogDsoNames->m_nDsoName; i++ )
		{
			vecObj.Add( wxString(m_pSky->m_pCatalogDsoNames->m_vectDsoName[i].name,wxConvUTF8) );
			//m_pObjectName->Append( m_pCatalog->m_vectDsoName[i].name );
		}
		m_pObjNameSelect->Freeze( );
		m_pObjNameSelect->Clear( );
//		m_pObjectName->Disable();
		m_pObjNameSelect->Append( vecObj );
//		m_pObjectName->Enable();
		m_pObjNameSelect->Thaw( );
		m_nSkyObjType = SKYOBJ_TYPE_DSO_NAMES;

	} else if( nType == SKYOBJ_TYPE_STAR_NAMES )
	{
		wxArrayString vecObj;
		vecObj.Alloc( m_pSky->m_pCatalogStarNames->m_nStarName+1 );

		// populate star names list from catalog entries
		for( i=0; i<m_pSky->m_pCatalogStarNames->m_nStarName; i++ )
		{
			vecObj.Add( wxString(m_pSky->m_pCatalogStarNames->m_vectStarName[i].name,wxConvUTF8) );
		}
		m_pObjNameSelect->Freeze( );
		m_pObjNameSelect->Clear( );
		m_pObjNameSelect->Append( vecObj );
		m_pObjNameSelect->Thaw( );
		m_nSkyObjType = SKYOBJ_TYPE_STAR_NAMES;

	} else if( nType == SKYOBJ_TYPE_CONST_NAMES )
	{
		wxArrayString vecObj;
		vecObj.Alloc( m_pSky->m_pConstellations->m_nConstName+1 );

		// populate constelattions names list from catalog entries
		for( i=0; i<m_pSky->m_pConstellations->m_nConstName; i++ )
		{
			vecObj.Add( wxString(m_pSky->m_pConstellations->m_vectConstName[i].name,wxConvUTF8) );
		}
		m_pObjNameSelect->Freeze( );
		m_pObjNameSelect->Clear( );
		m_pObjNameSelect->Append( vecObj );
		m_pObjNameSelect->Thaw( );

		m_nSkyObjType = SKYOBJ_TYPE_CONST_NAMES;

	} 

	return;
}

// when an object was selected
////////////////////////////////////////////////////////////////////
void CSkyObjSelectDlg::OnSelectName( wxCommandEvent& pEvent )
{
	m_nRaDeg=0; 
	m_nDecDeg=0;

	int nSelect = pEvent.GetSelection( );

	if( m_nSkyObjType == SKYOBJ_TYPE_STAR_NAMES )
	{
		//m_nRaDeg = m_pCatalog->m_vectStarName[nSelect].ra;
		//m_nDecDeg = m_pCatalog->m_vectStarName[nSelect].dec;
		m_pSky->m_pCatalogStarNames->GetNameRaDec( nSelect, m_nRaDeg, m_nDecDeg );

	} else if( m_nSkyObjType == SKYOBJ_TYPE_DSO_NAMES )
	{
		m_pSky->m_pCatalogDsoNames->GetDsoNameRaDec( nSelect, m_nRaDeg, m_nDecDeg );

	} else if( m_nSkyObjType == SKYOBJ_TYPE_CONST_NAMES )
	{
		m_nRaDeg = m_pSky->m_pConstellations->m_vectConstName[nSelect].ra;
		m_nDecDeg = m_pSky->m_pConstellations->m_vectConstName[nSelect].dec;
	}

	// set info box
	SetOrig( );

	return;
}

////////////////////////////////////////////////////////////////////
void CSkyObjSelectDlg::OnSetObjectId( wxCommandEvent& pEvent )
{
	if( !m_pObjectId ) return;
	m_nSkyObjType = m_pObjCatSelect->GetSelection();

	unsigned long nCatNo = 0;
	if( !m_pObjectId->GetLineText(0).ToULong( &nCatNo ) ) return;

	if( m_nSkyObjType == SKYOBJ_TYPE_NGC_ID )
	{
		m_pSky->m_pCatalogDsoNgc->GetRaDec( nCatNo, m_nRaDeg, m_nDecDeg );

	} else if( m_nSkyObjType == SKYOBJ_TYPE_IC_ID )
	{
		m_pSky->m_pCatalogDsoIc->GetRaDec( nCatNo, m_nRaDeg, m_nDecDeg );

	} else if( m_nSkyObjType == SKYOBJ_TYPE_MESSIER_ID )
	{
		m_pSky->m_pCatalogDsoMessier->GetRaDec( nCatNo, m_nRaDeg, m_nDecDeg );
	
	} else if( m_nSkyObjType == SKYOBJ_TYPE_SAO_ID )
	{
		m_pSky->m_pCatalogStars->GetStarRaDec( nCatNo, m_nRaDeg, m_nDecDeg );
	}

	// and now set orig
	SetOrig( );

	return;
}

// todo: cleanup - use global conversion functions
////////////////////////////////////////////////////////////////////
void CSkyObjSelectDlg::SetOrig( )
{
	wxString strTmp;

	// get selected format 
	int nFormat = m_pMainConfig->m_nSkyCoordEntryFormat;
	// write the entries
	if( nFormat == UNITS_FORMAT_SEXAGESIMAL )
	{
		// set ra
		RaDegToSexagesimal( m_nRaDeg, strTmp );
		m_pObjectRA->SetLabel( strTmp );

		// set dec
		DecDegToSexagesimal( m_nDecDeg, strTmp );
		m_pObjectDEC->SetLabel( strTmp );

	} else
	{
		strTmp.Printf( wxT("%.12f"), m_nRaDeg );
		m_pObjectRA->SetLabel( strTmp );
		strTmp.Printf( wxT("%.8f"), m_nDecDeg );
		m_pObjectDEC->SetLabel( strTmp );
	}

	return;
}
