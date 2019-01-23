
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// local headers
#include "../util/func.h"
#include "../sky/catalog.h"
#include "../sky/sky.h"
#include "../sky/catalog_stars.h"
#include "../sky/catalog_dso.h"
#include "../sky/catalog_radio.h"
#include "../sky/catalog_xgamma.h"
#include "../sky/catalog_dso.h"
#include "../sky/catalog_dso_names.h"
#include "../sky/catalog_supernovas.h"
#include "../sky/catalog_blackholes.h"
#include "../sky/catalog_mstars.h"
#include "../sky/catalog_exoplanets.h"
#include "../sky/catalog_asteroids.h"
#include "../sky/catalog_comets.h"
#include "../sky/catalog_aes.h"

// main header
#include "catalog_view.h"

//		***** CLASS CCatalogViewDlg *****
////////////////////////////////////////////////////////////////////
// CCatalogView EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CCatalogViewDlg, wxDialog)
//	EVT_TEXT_ENTER( wxID_VIEW_FIND_OBJECT_ENTRY, CCatalogViewDlg::OnDoFind )
//	EVT_BUTTON( wxID_VIEW_FIND_OBJECT_BUTTON, CCatalogViewDlg::OnDoFind )
	// :: page count
	EVT_CHOICE( wxID_CATALOG_VIEW_PAGE_SIZE, CCatalogViewDlg::OnPageSetup )
	// :: navigation
	EVT_BUTTON( wxID_CATALOG_VIEW_PAGE_BACK, CCatalogViewDlg::OnNavPage )
	EVT_BUTTON( wxID_CATALOG_VIEW_PAGE_NEXT, CCatalogViewDlg::OnNavPage )
	EVT_CHOICE( wxID_CATALOG_VIEW_SELECT_PAGE_NO, CCatalogViewDlg::OnNavPage )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CCatalogViewDlg
// Purpose:	Initialize add devicedialog
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCatalogViewDlg::CCatalogViewDlg( wxWindow *parent, CSky* pSky, const wxString& title ) : wxDialog(parent, -1,
                          title,
                          wxDefaultPosition,
                          wxSize(500,300),
                          wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL|wxRESIZE_BORDER )
{
	m_pSky = pSky;
	// init some vars
	m_nCatCols = 0;
	m_nCatRowsPerPage = DEFAULT_NO_OF_CAT_ROWS_PER_PAGE;
	// local vars
	wxString vectStrCatFindColsType[2];
	vectStrCatFindColsType[0] = wxT("All");
	wxString vectStrCatPageNo[2];
	vectStrCatPageNo[0] = wxT("1-100");
	wxString vectStrCatPageSize[5];
	vectStrCatPageSize[0] = wxT("100");
	vectStrCatPageSize[1] = wxT("500");
	vectStrCatPageSize[2] = wxT("1000");
	vectStrCatPageSize[3] = wxT("5000");
	vectStrCatPageSize[4] = wxT("All");

	// main sizer
	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 2,1,0,0 );
	topsizer->AddGrowableRow( 1 );
	topsizer->AddGrowableCol( 0 );

	/////////////////////////////
	// basic panel sizer - layout of elements
	wxFlexGridSizer* headSizer = new wxFlexGridSizer( 1,3,0,0 );
	headSizer->AddGrowableCol( 1 );
	// :: in head sizers
	wxFlexGridSizer* headFindSizer = new wxFlexGridSizer( 1,3,0,0 );
	wxFlexGridSizer* headPageCountSizer = new wxFlexGridSizer( 1,2,0,0 );
	wxFlexGridSizer* headPageNavSizer = new wxFlexGridSizer( 1,3,0,0 );
	// add to main header sizers
	headSizer->Add( headFindSizer, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	headSizer->Add( headPageCountSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );
	headSizer->Add( headPageNavSizer, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );

	///////////////////////////////////////////
	// create my data objects
	// :: find rows items
	m_pFindRowsButton = new wxButton( this, wxID_CATALOG_VIEW_FIND_ROWS_BUTTON, wxT("Find"), wxPoint(-1, -1), wxSize(60,-1) );
	m_pFindTextEntry = new wxTextCtrl( this, wxID_CATALOG_VIEW_FIND_ROWS_ENTRY, wxT(""), wxDefaultPosition, wxSize(90,-1) );
	m_pFindCols = new wxChoice( this, wxID_CATALOG_VIEW_FIND_COLS_TYPE,
										wxDefaultPosition, wxSize(100,-1), 1, 
										vectStrCatFindColsType, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pFindCols->SetSelection( 0 );
	// :: page size
	m_pCatalogPageSize = new wxChoice( this, wxID_CATALOG_VIEW_PAGE_SIZE,
										wxDefaultPosition, wxSize(60,-1), 5, 
										vectStrCatPageSize, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCatalogPageSize->SetSelection( 0 );
	// :: navigation items
	m_pPrevButton = new wxButton( this, wxID_CATALOG_VIEW_PAGE_BACK, wxT("<< Back"), wxPoint(-1, -1), wxSize(60,-1) );
	m_pPrevButton->Disable();
	m_pNextButton = new wxButton( this, wxID_CATALOG_VIEW_PAGE_NEXT, wxT("Next >>"), wxPoint(-1, -1), wxSize(60,-1) );
	m_pCatalogPage = new wxChoice( this, wxID_CATALOG_VIEW_SELECT_PAGE_NO,
										wxDefaultPosition, wxSize(110,-1), 1, 
										vectStrCatPageNo, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCatalogPage->SetSelection( 0 );

	// the tables
	m_pCatalogTable = new wxListCtrl( this, -1,
			wxPoint(-1,-1), wxSize(700,300), wxLC_REPORT|wxLC_HRULES|wxLC_VRULES );
	

	///////////////////////////////////
	// head sizers
	// :: find
	headFindSizer->Add( m_pFindRowsButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	headFindSizer->Add( m_pFindTextEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5  );
	headFindSizer->Add( m_pFindCols, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: page size
	headPageCountSizer->Add( new wxStaticText( this, -1, wxT("Page Size:")), 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxRIGHT, 5  );
	headPageCountSizer->Add( m_pCatalogPageSize, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: navigation
	headPageNavSizer->Add( m_pPrevButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	headPageNavSizer->Add( m_pCatalogPage, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL |wxLEFT|wxRIGHT, 5  );
	headPageNavSizer->Add( m_pNextButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// add to main sizer
	topsizer->Add( headSizer, 1, wxGROW | wxALL, 5 );

	// set table
	topsizer->Add( m_pCatalogTable, 1, wxEXPAND| wxGROW | wxALL, 4 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CCatalogViewDlg
// Purpose:	Delete my object
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCatalogViewDlg::~CCatalogViewDlg( )
{
	delete( m_pFindRowsButton );
	delete( m_pFindTextEntry );
	delete( m_pFindCols );
	delete( m_pCatalogPageSize );
	delete( m_pPrevButton );
	delete( m_pNextButton );
	delete( m_pCatalogPage );
	delete( m_pCatalogTable );
}

////////////////////////////////////////////////////////////////////
// Method:		SetCatalogTable
// Class:		CCatalogViewDlg
// Purpose:		set catalog table
// Input:		catalog id
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CCatalogViewDlg::SetCatalogTable( int nCatDefId, int nCatId )
{	
	m_nCatRows=0;

	///////////////////////////////////////
	// GET BY TYPE
	if( nCatId == CATALOG_ID_EXOPLANET_EU )
	{
		// set profile - todo :: maybe I should move this in main select catalog ???
		m_pSky->m_pCatalogExoplanets->SetProfile( m_pSky->m_vectCatalogDef[nCatDefId] );

		m_pSky->m_pCatalogExoplanets->Load( nCatId  );
		m_nCatRows = m_pSky->m_pCatalogExoplanets->m_nData;
		// set colummn definition
		m_vectHeader = vectExoPlanet;
		m_nCatCols = 11;

	} else if( nCatId == CATALOG_ID_SAO || nCatId == CATALOG_ID_HIPPARCOS ||
				nCatId == CATALOG_ID_TYCHO_2 || nCatId == CATALOG_ID_2MASS || 
				nCatId == CATALOG_ID_GSC || nCatId == CATALOG_ID_USNO_B1 )
	{
		m_pSky->m_pCatalogStars->SetProfile( m_pSky->m_vectCatalogDef[nCatDefId] );
		// load
		m_pSky->m_pCatalogStars->LoadCatalog( nCatId  );
		m_nCatRows = m_pSky->m_pCatalogStars->m_nStar;
		// set colummn definition
		m_vectHeader = vectStars;
		m_nCatCols = 4;

	} else if( nCatId == CATALOG_ID_PGC || nCatId == CATALOG_ID_HYPERLEDA ||
				nCatId == CATALOG_ID_UGC || nCatId == CATALOG_ID_MCG ||
				nCatId == CATALOG_ID_MGC || nCatId == CATALOG_ID_PGC_ )
	{
		m_pSky->m_pCatalogDso->SetProfile( m_pSky->m_vectCatalogDef[nCatDefId] );
		// load
		m_pSky->m_pCatalogDso->Load( nCatId  );
		m_nCatRows = m_pSky->m_pCatalogDso->m_nDso;
		// set colummn definition
		m_vectHeader = vectHeadDso;
		m_nCatCols = 7;

	} else if( nCatId == CATALOG_ID_SPECFIND || nCatId == CATALOG_ID_VLSS ||
				nCatId == CATALOG_ID_NVSS || nCatId == CATALOG_ID_MSL )
	{
		m_pSky->m_pCatalogRadio->SetProfile( m_pSky->m_vectCatalogDef[nCatDefId] );
		// load
		m_pSky->m_pCatalogRadio->Load( nCatId  );
		m_nCatRows = m_pSky->m_pCatalogRadio->m_nData;
		// set colummn definition
		m_vectHeader = vectHeadRadio;
		m_nCatCols = 10;

	} else if( nCatId == CATALOG_ID_ROSAT || nCatId == CATALOG_ID_BATSE ||
				nCatId == CATALOG_ID_INTEGRAL || nCatId == CATALOG_ID_BEPPOSAX_GRBM ||
				nCatId == CATALOG_ID_2XMMi || nCatId == CATALOG_ID_HEAO_A1 )
	{
		m_pSky->m_pCatalogXGamma->SetProfile( m_pSky->m_vectCatalogDef[nCatDefId] );
		// load
		m_pSky->m_pCatalogXGamma->Load( nCatId  );
		m_nCatRows = m_pSky->m_pCatalogXGamma->m_nData;
		// set colummn definition
		m_vectHeader = vectHeadXGamma;
		m_nCatCols = 28;

	} else if( nCatId == CATALOG_ID_ASC || nCatId == CATALOG_ID_SSC )
	{
		m_pSky->m_pCatalogSupernovas->SetProfile( m_pSky->m_vectCatalogDef[nCatDefId] );
		// load
		m_pSky->m_pCatalogSupernovas->Load( nCatId  );
		m_nCatRows = m_pSky->m_pCatalogSupernovas->m_nData;
		// set colummn definition
		m_vectHeader = vectHeadSupernovas;
		m_nCatCols = 16;

	} else if( nCatId == CATALOG_ID_SDSS_DR5 || nCatId == CATALOG_ID_RXE_AGN )
	{
		m_pSky->m_pCatalogBlackholes->SetProfile( m_pSky->m_vectCatalogDef[nCatDefId] );
		// load
		m_pSky->m_pCatalogBlackholes->Load( nCatId  );
		m_nCatRows = m_pSky->m_pCatalogBlackholes->m_nData;
		// set colummn definition
		m_vectHeader = vectHeadBlackholes;
		m_nCatCols = 13;

	} else if( nCatId == CATALOG_ID_WDS || nCatId == CATALOG_ID_CCDM )
	{
		m_pSky->m_pCatalogMStars->SetProfile( m_pSky->m_vectCatalogDef[nCatDefId] );
		// load
		m_pSky->m_pCatalogMStars->Load( nCatId  );
		m_nCatRows = m_pSky->m_pCatalogMStars->m_nData;
		// set colummn definition
		m_vectHeader = vectHeadMStars;
		m_nCatCols = 22;

	} else if( nCatId == CATALOG_ID_OMP )
	{
		m_pSky->m_pCatalogAsteroids->SetProfile( m_pSky->m_vectCatalogDef[nCatDefId] );
		// load
		m_pSky->m_pCatalogAsteroids->Load( nCatId  );
		m_nCatRows = m_pSky->m_pCatalogAsteroids->m_nData;
		// set colummn definition
		m_vectHeader = vectHeadAST;
		m_nCatCols = 22;

	} else if( nCatId == CATALOG_ID_COCOM )
	{
		m_pSky->m_pCatalogComets->SetProfile( m_pSky->m_vectCatalogDef[nCatDefId] );
		// load
		m_pSky->m_pCatalogComets->Load( nCatId  );
		m_nCatRows = m_pSky->m_pCatalogComets->m_nData;
		// set colummn definition
		m_vectHeader = vectHeadCOM;
		m_nCatCols = 15;

	} else if( nCatId == CATALOG_ID_CELESTRAK )
	{
		m_pSky->m_pCatalogAes->SetProfile( m_pSky->m_vectCatalogDef[nCatDefId] );
		// load
		m_pSky->m_pCatalogAes->Load( nCatId  );
		m_nCatRows = m_pSky->m_pCatalogAes->m_nData;
		// set colummn definition
		m_vectHeader = vectHeadAES;
		m_nCatCols = 21;

	} else
		return;

	// se locals
	m_nCatId = nCatId;
	m_nCatViewPage = 0;

	// set page numbers
	m_nCatPages = (int) round((double) m_nCatRows/(double) m_nCatRowsPerPage + 0.499999);

	// init catalog page
	InitCatalogTablePage( );
	// set first page
	SetCatalogTablePage( );

	Refresh( FALSE );
	Update( );
}

////////////////////////////////////////////////////////////////////
void CCatalogViewDlg::InitCatalogTablePage( )
{
	int i = 0, j = 0;
	wxString strData;

	m_pCatalogPage->Clear();
	m_pCatalogPage->Freeze();
	for( i=0; i<m_nCatPages; i++ )
	{
		unsigned long nEnd = (i+1)*m_nCatRowsPerPage;
		if( nEnd > m_nCatRows ) nEnd = m_nCatRows;
		strData.Printf( wxT("%d-%d"), i*m_nCatRowsPerPage+1, nEnd );
		m_pCatalogPage->Append( strData );
	}
	m_pCatalogPage->SetSelection( 0 );
	m_pCatalogPage->Thaw();


	// clean up
	m_pCatalogTable->ClearAll();

	// create header
	m_pCatalogTable->InsertColumn( 1, wxT("Id"), wxLIST_FORMAT_CENTRE, 40 );
	m_pFindCols->Freeze();
	m_pFindCols->Append( wxT("Id") );
	for( i=1; i<=m_nCatCols; i++ )
	{
		m_pCatalogTable->InsertColumn( i+1, wxString(m_vectHeader[i-1].label,wxConvUTF8), wxLIST_FORMAT_CENTRE, m_vectHeader[i-1].size );
		m_pFindCols->Append( wxString(m_vectHeader[i-1].label,wxConvUTF8) );
	}
	m_pFindCols->Thaw();

	// add data - for every column
	m_pCatalogTable->Freeze();
	for( i=0; i<m_nCatRowsPerPage; i++ )
	{
		long nIndex = m_pCatalogTable->InsertItem( 55551+i, wxT("") );
	}
	m_pCatalogTable->Thaw();
}

////////////////////////////////////////////////////////////////////
void CCatalogViewDlg::SetCatalogTablePage( )
{
	int i=0, j=0;
	long nIndex=0;
	wxString strData;

	// add data - for every column
	m_pCatalogTable->Freeze();
	for( i=0; i<m_nCatRowsPerPage; i++ )
	{
		nIndex = i;
		// check if before end
		unsigned long nCol = m_nCatViewPage*m_nCatRowsPerPage+i;
		if( nCol < m_nCatRows )
		{
			// set the id
			strData.Printf( wxT("%d"), m_nCatViewPage*m_nCatRowsPerPage+i+1 );
			m_pCatalogTable->SetItem( nIndex, 0, strData );

			for( j=1; j<=m_nCatCols; j++ )
			{
				//char str_data[255];
				if( GetCatalogColumnRowDataAsStr( m_nCatId, j-1, nCol, strData ) )
					m_pCatalogTable->SetItem( nIndex, j, strData );
				else
					m_pCatalogTable->SetItem( nIndex, j, wxT("-") );

			}

		} else
		{
			for( j=0; j<=m_nCatCols; j++ ) m_pCatalogTable->SetItem( nIndex, j, wxT("") );
		}
	}
	m_pCatalogTable->Thaw();

	return;
}

////////////////////////////////////////////////////////////////////
int CCatalogViewDlg::GetCatalogColumnRowDataAsStr( int nCatId, int col, int row, wxString& strData )
{
//	wxString strWxData = wxT("");

	// get by type
	if( nCatId == CATALOG_ID_EXOPLANET_EU )
	{
		if( row >  m_pSky->m_pCatalogExoplanets->m_nData ) return( 0 );
		// get each column by type
		if( col == 0 )
			strData.Printf( wxT("%s"), m_pSky->m_pCatalogExoplanets->m_vectData[row].cat_name );
		else if( col == 1 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogExoplanets->m_vectData[row].mass );
		else if( col == 2 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogExoplanets->m_vectData[row].radius );
		else if( col == 3 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogExoplanets->m_vectData[row].period );
		else if( col == 4 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogExoplanets->m_vectData[row].sm_axis );
		else if( col == 5 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogExoplanets->m_vectData[row].eccentricity );
		else if( col == 6 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogExoplanets->m_vectData[row].inclination );
		else if( col == 7 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogExoplanets->m_vectData[row].ang_dist );
		else if( col == 8 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogExoplanets->m_vectData[row].discovery_year );
		else if( col == 9 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogExoplanets->m_vectData[row].ra );
		else if( col == 10 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogExoplanets->m_vectData[row].dec );
		else
			return( 0 );

	} else if( nCatId == CATALOG_ID_SAO || nCatId == CATALOG_ID_HIPPARCOS ||
				nCatId == CATALOG_ID_TYCHO_2 || nCatId == CATALOG_ID_2MASS || 
				nCatId == CATALOG_ID_GSC || nCatId == CATALOG_ID_USNO_B1 )
	{
		if( row >  m_pSky->m_pCatalogStars->m_nStar ) return( 0 );

		if( col == 0 )
			m_pSky->m_pCatalogStars->GetSimpleCatName( m_pSky->m_pCatalogStars->m_vectStar[row], strData );
		else if( col == 1 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogStars->m_vectStar[row].ra );
		else if( col == 2 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogStars->m_vectStar[row].dec );
		else if( col == 3 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogStars->m_vectStar[row].mag );
		else
			return( 0 );

	} else if( nCatId == CATALOG_ID_PGC || nCatId == CATALOG_ID_HYPERLEDA ||
				nCatId == CATALOG_ID_UGC || nCatId == CATALOG_ID_MCG ||
				nCatId == CATALOG_ID_MGC || nCatId == CATALOG_ID_PGC_ )
	{
		if( row >  m_pSky->m_pCatalogDso->m_nDso ) return( 0 );

		if( col == 0 )
			m_pSky->m_pCatalogDsoNames->GetDsoSimpleCatName( m_pSky->m_pCatalogDso->m_vectDso[row], 0, strData );
		else if( col == 1 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogDso->m_vectDso[row].ra );
		else if( col == 2 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogDso->m_vectDso[row].dec );
		else if( col == 3 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogDso->m_vectDso[row].mag );
		else if( col == 4 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogDso->m_vectDso[row].a );
		else if( col == 5 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogDso->m_vectDso[row].b );
		else if( col == 6 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogDso->m_vectDso[row].pa );
		else
			return( 0 );

	///////////////////////////////////////
	// RADIO
	} else if( nCatId == CATALOG_ID_SPECFIND || nCatId == CATALOG_ID_VLSS ||
				nCatId == CATALOG_ID_NVSS || nCatId == CATALOG_ID_MSL )
	{
		if( row >  m_pSky->m_pCatalogRadio->m_nData ) return( 0 );

		if( col == 0 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogRadio->m_vectData[row].cat_no );
		else if( col == 1 )
			strData.Printf( wxT("%hs"), m_pSky->m_pCatalogRadio->m_vectData[row].cat_name );
		else if( col == 2 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogRadio->m_vectData[row].ra );
		else if( col == 3 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogRadio->m_vectData[row].dec );
		else if( col == 4 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogRadio->m_vectData[row].frequency );
		else if( col == 5 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogRadio->m_vectData[row].flux_density );
		else if( col == 6 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogRadio->m_vectData[row].i_flux_density );
		else if( col == 7 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogRadio->m_vectData[row].no_spec_points );
		else if( col == 8 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogRadio->m_vectData[row].spec_slope );
		else if( col == 9 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogRadio->m_vectData[row].spec_abscissa );
		else
			return( 0 );

	//////////////////////////////////////
	// GAMMA/X-RAY
	} else if( nCatId == CATALOG_ID_ROSAT || nCatId == CATALOG_ID_BATSE ||
				nCatId == CATALOG_ID_INTEGRAL || nCatId == CATALOG_ID_BEPPOSAX_GRBM ||
				nCatId == CATALOG_ID_2XMMi || nCatId == CATALOG_ID_HEAO_A1 )
	{
		if( row >  m_pSky->m_pCatalogXGamma->m_nData ) return( 0 );

		if( col == 0 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogXGamma->m_vectData[row].cat_no );
		else if( col == 1 )
			strData.Printf( wxT("%hs"), m_pSky->m_pCatalogXGamma->m_vectData[row].cat_name );
		else if( col == 2 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogXGamma->m_vectData[row].ra );
		else if( col == 3 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogXGamma->m_vectData[row].dec );
		else if( col == 4 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogXGamma->m_vectData[row].count );
		else if( col == 5 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogXGamma->m_vectData[row].bg_count );
		else if( col == 6 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogXGamma->m_vectData[row].exp_time );
		else if( col == 7 )
			strData.Printf( wxT("%e"), m_pSky->m_pCatalogXGamma->m_vectData[row].flux );
		else if( col == 8 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogXGamma->m_vectData[row].start_time );
		else if( col == 9 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogXGamma->m_vectData[row].duration );
		else if( col == 10 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogXGamma->m_vectData[row].burst_date_time );
		else if( col == 11 )
			strData.Printf( wxT("%e"), m_pSky->m_pCatalogXGamma->m_vectData[row].flux_band_1 );
		else if( col == 12 )
			strData.Printf( wxT("%e"), m_pSky->m_pCatalogXGamma->m_vectData[row].flux_band_2 );
		else if( col == 13 )
			strData.Printf( wxT("%e"), m_pSky->m_pCatalogXGamma->m_vectData[row].flux_band_3 );
		else if( col == 14 )
			strData.Printf( wxT("%e"), m_pSky->m_pCatalogXGamma->m_vectData[row].flux_band_4 );
		else if( col == 15 )
			strData.Printf( wxT("%e"), m_pSky->m_pCatalogXGamma->m_vectData[row].flux_band_5 );
		else if( col == 16 )
			strData.Printf( wxT("%e"), m_pSky->m_pCatalogXGamma->m_vectData[row].flux_band_6 );
		else if( col == 17 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogXGamma->m_vectData[row].count_band_1 );
		else if( col == 18 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogXGamma->m_vectData[row].count_band_2 );
		else if( col == 19 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogXGamma->m_vectData[row].count_band_3 );
		else if( col == 20 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogXGamma->m_vectData[row].count_band_4 );
		else if( col == 21 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogXGamma->m_vectData[row].source_type );
		else if( col == 22 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogXGamma->m_vectData[row].time_det );
		else if( col == 23 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogXGamma->m_vectData[row].interval_no );
		else if( col == 24 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogXGamma->m_vectData[row].fluence );
		else if( col == 25 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogXGamma->m_vectData[row].peak_flux );
		else if( col == 26 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogXGamma->m_vectData[row].gamma );
		else if( col == 27 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogXGamma->m_vectData[row].no_detections );
		else
			return( 0 );

	///////////////////////////
	// SUPERNOVAS
	} else if( nCatId == CATALOG_ID_ASC || nCatId == CATALOG_ID_SSC )
	{
		if( row >  m_pSky->m_pCatalogSupernovas->m_nData ) return( 0 );

		if( col == 0 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogSupernovas->m_vectData[row].cat_no );
		else if( col == 1 )
			strData.Printf( wxT("%s"), m_pSky->m_pCatalogSupernovas->m_vectData[row].cat_name );
		else if( col == 2 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogSupernovas->m_vectData[row].ra );
		else if( col == 3 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogSupernovas->m_vectData[row].dec );
		else if( col == 4 )
			strData.Printf( wxT("%s"), wxDateTime( m_pSky->m_pCatalogSupernovas->m_vectData[row].date_max ).Format( wxT("%d/%m/%Y") ) );
		else if( col == 5 )
			strData.Printf( wxT("%s"), wxDateTime( m_pSky->m_pCatalogSupernovas->m_vectData[row].date_discovery ).Format( wxT("%d/%m/%Y") ) );
		else if( col == 6 )
			strData.Printf( wxT("%s"), m_pSky->m_pCatalogSupernovas->m_vectData[row].discoverer );
		else if( col == 7 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogSupernovas->m_vectData[row].mag );
		else if( col == 8 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogSupernovas->m_vectData[row].max_mag );
		else if( col == 9 )
			strData.Printf( wxT("%s"), m_pSky->m_pCatalogSupernovas->m_vectData[row].sn_type );
		else if( col == 10 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogSupernovas->m_vectData[row].gal_center_offset_x );
		else if( col == 11 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogSupernovas->m_vectData[row].gal_center_offset_y );
		else if( col == 12 )
			strData.Printf( wxT("%s"), m_pSky->m_pCatalogSupernovas->m_vectData[row].parent_galaxy );
		else if( col == 13 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogSupernovas->m_vectData[row].disc_method );
		else if( col == 14 )
			strData.Printf( wxT("%s"), m_pSky->m_pCatalogSupernovas->m_vectData[row].prog_code );
		else if( col == 15 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogSupernovas->m_vectData[row].unconfirmed );
		else
			return( 0 );

	///////////////////////////////////////
	// BLACK HOLES
	} else if( nCatId == CATALOG_ID_SDSS_DR5 || nCatId == CATALOG_ID_RXE_AGN )
	{
		if( row >  m_pSky->m_pCatalogBlackholes->m_nData ) return( 0 );

		if( col == 0 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogBlackholes->m_vectData[row].cat_no );
		else if( col == 1 )
			strData.Printf( wxT("%s"), m_pSky->m_pCatalogBlackholes->m_vectData[row].cat_name );
		else if( col == 2 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogBlackholes->m_vectData[row].ra );
		else if( col == 3 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogBlackholes->m_vectData[row].dec );
		else if( col == 4 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogBlackholes->m_vectData[row].mass );
		else if( col == 5 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogBlackholes->m_vectData[row].mass_hbeta );
		else if( col == 6 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogBlackholes->m_vectData[row].mass_mgii );
		else if( col == 7 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogBlackholes->m_vectData[row].mass_civ );
		else if( col == 8 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogBlackholes->m_vectData[row].luminosity );
		else if( col == 9 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogBlackholes->m_vectData[row].radio_luminosity );
		else if( col == 10 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogBlackholes->m_vectData[row].xray_luminosity );
		else if( col == 11 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogBlackholes->m_vectData[row].redshift );
		else if( col == 12 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogBlackholes->m_vectData[row].target_type );
		else
			return( 0 );

	///////////////////////////////////////
	// MULTIPLE/DOUBLE STARS
	} else if( nCatId == CATALOG_ID_WDS || nCatId == CATALOG_ID_CCDM )
	{
		if( row >  m_pSky->m_pCatalogMStars->m_nData ) return( 0 );

		if( col == 0 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogMStars->m_vectData[row].cat_no );
		else if( col == 1 )
			strData.Printf( wxT("%hs"), m_pSky->m_pCatalogMStars->m_vectData[row].cat_name );
		else if( col == 2 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogMStars->m_vectData[row].ra );
		else if( col == 3 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogMStars->m_vectData[row].dec );
		else if( col == 4 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogMStars->m_vectData[row].mag );
		else if( col == 5 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogMStars->m_vectData[row].mag2 );
		else if( col == 6 )
			strData.Printf( wxT("%s"), m_pSky->m_pCatalogMStars->m_vectData[row].comp );
		else if( col == 7 )
			strData.Printf( wxT("%hs"), m_pSky->m_pCatalogMStars->m_vectData[row].spectral_type );
		else if( col == 8 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogMStars->m_vectData[row].pos_ang );
		else if( col == 9 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogMStars->m_vectData[row].pos_ang2 );
		else if( col == 10 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogMStars->m_vectData[row].pm_ra );
		else if( col == 11 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogMStars->m_vectData[row].pm_dec );
		else if( col == 12 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogMStars->m_vectData[row].pm_ra2 );
		else if( col == 13 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogMStars->m_vectData[row].pm_dec2 );
		else if( col == 14 )
			strData.Printf( wxT("%hs"), m_pSky->m_pCatalogMStars->m_vectData[row].pm_note );
		else if( col == 15 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogMStars->m_vectData[row].sep );
		else if( col == 16 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogMStars->m_vectData[row].sep2 );
		else if( col == 17 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogMStars->m_vectData[row].nobs );
		else if( col == 18 )
			strData.Printf( wxT("%s"), wxDateTime( m_pSky->m_pCatalogMStars->m_vectData[row].obs_date ).Format( wxT("%d/%m/%Y") ) );
		else if( col == 19 )
			strData.Printf( wxT("%s"), wxDateTime( m_pSky->m_pCatalogMStars->m_vectData[row].obs_date2 ).Format( wxT("%d/%m/%Y") ) );
		else if( col == 20 )
			strData.Printf( wxT("%hs"), m_pSky->m_pCatalogMStars->m_vectData[row].discoverer );
		else if( col == 21 )
			strData.Printf( wxT("%hs"), m_pSky->m_pCatalogMStars->m_vectData[row].notes );
		else
			return( 0 );

	///////////////////////////////////////
	// ASTEROIDS
	} else if( nCatId == CATALOG_ID_OMP )
	{
		if( row >  m_pSky->m_pCatalogAsteroids->m_nData ) return( 0 );

		if( col == 0 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogAsteroids->m_vectData[row].cat_no );
		else if( col == 1 )
			strData.Printf( wxT("%hs"), m_pSky->m_pCatalogAsteroids->m_vectData[row].cat_name );
		else if( col == 2 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAsteroids->m_vectData[row].ra );
		else if( col == 3 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAsteroids->m_vectData[row].dec );
		else if( col == 4 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogAsteroids->m_vectData[row].ast_no );
		else if( col == 5 )
			strData.Printf( wxT("%hs"), m_pSky->m_pCatalogAsteroids->m_vectData[row].comp_name );
		else if( col == 6 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAsteroids->m_vectData[row].abs_mag_h );
		else if( col == 7 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAsteroids->m_vectData[row].slope_mag );
		else if( col == 8 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAsteroids->m_vectData[row].color_index );
		else if( col == 9 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAsteroids->m_vectData[row].iras_diam );
		else if( col == 10 )
			strData.Printf( wxT("%hs"), m_pSky->m_pCatalogAsteroids->m_vectData[row].iras_class );
		else if( col == 11 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogAsteroids->m_vectData[row].no_arc );
		else if( col == 12 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogAsteroids->m_vectData[row].no_obs );
		else if( col == 13 )
			strData.Printf( wxT("%s"), wxDateTime( m_pSky->m_pCatalogAsteroids->m_vectData[row].epoch ).Format( wxT("%d/%m/%Y") ) );
		else if( col == 14 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAsteroids->m_vectData[row].mean_anomaly );
		else if( col == 15 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAsteroids->m_vectData[row].arg_perihelion );
		else if( col == 16 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAsteroids->m_vectData[row].lon_asc_node );
		else if( col == 17 )
			strData.Printf( wxT("%s"), wxDateTime( m_pSky->m_pCatalogAsteroids->m_vectData[row].date_orb_comp ).Format( wxT("%d/%m/%Y") ) );
		else if( col == 18 )
			strData.Printf( wxT("%s"), wxDateTime( m_pSky->m_pCatalogAsteroids->m_vectData[row].update_date ).Format( wxT("%d/%m/%Y") ) );
		else if( col == 19 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAsteroids->m_vectData[row].inclination );
		else if( col == 20 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAsteroids->m_vectData[row].eccentricity );
		else if( col == 21 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAsteroids->m_vectData[row].smaj_axis );
		else
			return( 0 );

	///////////////////////////////////////
	// COMETS
	} else if( nCatId == CATALOG_ID_COCOM )
	{
		if( row >  m_pSky->m_pCatalogComets->m_nData ) return( 0 );

		if( col == 0 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogComets->m_vectData[row].cat_no );
		else if( col == 1 )
			strData.Printf( wxT("%s"), m_pSky->m_pCatalogComets->m_vectData[row].cat_name );
		else if( col == 2 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogComets->m_vectData[row].ra );
		else if( col == 3 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogComets->m_vectData[row].dec );
		else if( col == 4 )
			strData.Printf( wxT("%s"), wxDateTime( m_pSky->m_pCatalogComets->m_vectData[row].note_update_date ).Format( wxT("%d/%m/%Y") ) );
		else if( col == 5 )
			strData.Printf( wxT("%s"), m_pSky->m_pCatalogComets->m_vectData[row].iau_code );
		else if( col == 6 )
			strData.Printf( wxT("%s"), m_pSky->m_pCatalogComets->m_vectData[row].comp_name );
		else if( col == 7 )
			strData.Printf( wxT("%s"), wxDateTime( m_pSky->m_pCatalogComets->m_vectData[row].epoch_comp ).Format( wxT("%d/%m/%Y") ) );
		else if( col == 8 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogComets->m_vectData[row].rel_effect );
		else if( col == 9 )
			strData.Printf( wxT("%s"), wxDateTime( m_pSky->m_pCatalogComets->m_vectData[row].perihelion_date ).Format( wxT("%d/%m/%Y") ) );
		else if( col == 10 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogComets->m_vectData[row].perihelion_distance );
		else if( col == 11 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogComets->m_vectData[row].arg_perihelion );
		else if( col == 12 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogComets->m_vectData[row].lon_orbital_node );
		else if( col == 13 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogComets->m_vectData[row].inclination );
		else if( col == 14 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogComets->m_vectData[row].eccentricity );
		else
			return( 0 );

	///////////////////////////////////////
	// ASTEROIDS
	} else if( nCatId == CATALOG_ID_CELESTRAK )
	{
		if( row >  m_pSky->m_pCatalogAes->m_nData ) return( 0 );

		if( col == 0 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogAes->m_vectData[row].cat_no );
		else if( col == 1 )
			strData.Printf( wxT("%hs"), m_pSky->m_pCatalogAes->m_vectData[row].cat_name );
		else if( col == 2 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogAes->m_vectData[row].type_of_orbit );
		else if( col == 3 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogAes->m_vectData[row].obj_type );
		else if( col == 4 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogAes->m_vectData[row].vectSatType[0] );
		else if( col == 5 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogAes->m_vectData[row].norad_cat_no );
		else if( col == 6 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogAes->m_vectData[row].security_class );
		else if( col == 7 )
			strData.Printf( wxT("%d-%d-%hs"), m_pSky->m_pCatalogAes->m_vectData[row].int_id_year, 
										m_pSky->m_pCatalogAes->m_vectData[row].int_id_launch_no, 
										m_pSky->m_pCatalogAes->m_vectData[row].int_id_launch_piece );
		else if( col == 8 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogAes->m_vectData[row].ephemeris_type );
		else if( col == 9 )
			strData.Printf( wxT("%s"), wxDateTime( m_pSky->m_pCatalogAes->m_vectData[row].vectTLERec[0].epoch_time ).Format( wxT("%d/%m/%Y") ) );
		else if( col == 10 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAes->m_vectData[row].vectTLERec[0].orbit_decay_rate );
		else if( col == 11 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAes->m_vectData[row].vectTLERec[0].mean_motion_second_time_deriv );
		else if( col == 12 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAes->m_vectData[row].vectTLERec[0].object_drag_coeff );
		else if( col == 13 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogAes->m_vectData[row].vectTLERec[0].element_number );
		else if( col == 14 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAes->m_vectData[row].vectTLERec[0].inclination );
		else if( col == 15 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAes->m_vectData[row].vectTLERec[0].asc_node_ra );
		else if( col == 16 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAes->m_vectData[row].vectTLERec[0].eccentricity );
		else if( col == 17 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAes->m_vectData[row].vectTLERec[0].arg_of_perigee );
		else if( col == 18 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAes->m_vectData[row].vectTLERec[0].mean_anomaly );
		else if( col == 19 )
			strData.Printf( wxT("%lf"), m_pSky->m_pCatalogAes->m_vectData[row].vectTLERec[0].mean_motion );
		else if( col == 20 )
			strData.Printf( wxT("%d"), m_pSky->m_pCatalogAes->m_vectData[row].vectTLERec[0].orb_no_epoch );
		else
			return( 0 );

	} else
		return( 0 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		OnDoFind
// Class:		CCatalogViewDlg
// Purpose:		when find button pressed
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CCatalogViewDlg::OnDoFind( wxCommandEvent& pEvent )
{
	EndModal( wxID_OK );

	return;
}

////////////////////////////////////////////////////////////////////
void CCatalogViewDlg::OnPageSetup( wxCommandEvent& pEvent )
{
	int nId = m_pCatalogPageSize->GetSelection();
	// calculate row starting at the current page
	unsigned long nRowStart = m_nCatViewPage*m_nCatRowsPerPage+1;

	if( nId == 0 && m_nCatRowsPerPage != 100 )
		m_nCatRowsPerPage = 100;
	else if( nId == 1 && m_nCatRowsPerPage != 500 )
		m_nCatRowsPerPage = 500;
	else if( nId == 2 && m_nCatRowsPerPage != 1000 )
		m_nCatRowsPerPage = 1000;
	else if( nId == 3 && m_nCatRowsPerPage != 5000 )
		m_nCatRowsPerPage = 5000;
	else if( nId == 4 && m_nCatRowsPerPage != m_nCatRows )
		m_nCatRowsPerPage = m_nCatRows;
	else
		return;

	// check if over the size of catalog
	if( m_nCatRowsPerPage > m_nCatRows ) m_nCatRowsPerPage = m_nCatRows;
	// check if to start at the same page 
	if( nRowStart <= m_nCatRowsPerPage ) 
		m_nCatViewPage = 0;
	else
		m_nCatViewPage = (int) round((double) nRowStart/(double) m_nCatRowsPerPage + 0.499999);

	// set page numbers
	m_nCatPages = (int) round((double) m_nCatRows/(double) m_nCatRowsPerPage + 0.499999);

	// init catalog page
	InitCatalogTablePage( );
	// set page
	SetNavPage( -1 );

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
void CCatalogViewDlg::OnNavPage( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();

	if( nId == wxID_CATALOG_VIEW_PAGE_NEXT )
		m_nCatViewPage++;
	else if( nId == wxID_CATALOG_VIEW_PAGE_BACK )
		m_nCatViewPage--;
	else if( nId == wxID_CATALOG_VIEW_SELECT_PAGE_NO )
		m_nCatViewPage = m_pCatalogPage->GetSelection();

	// set nav page
	SetNavPage( nId );

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
void CCatalogViewDlg::SetNavPage( int nId )
{
	// set page select
	if( nId != wxID_CATALOG_VIEW_SELECT_PAGE_NO ) m_pCatalogPage->SetSelection( m_nCatViewPage );

	SetCatalogTablePage( );

	// enable/disable back button
	if( m_nCatViewPage == 0 )
		m_pPrevButton->Disable();
	else 
		m_pPrevButton->Enable();

	// enable/disable next button
	if( m_nCatViewPage >= m_nCatPages-1 ) 
		m_pNextButton->Disable();
	else
		m_pNextButton->Enable();

	return;
}
