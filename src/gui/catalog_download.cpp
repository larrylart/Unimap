////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// local headers
#include "../config/mainconfig.h"
#include "../sky/catalog_types.h"
#include "../sky/sky.h"
#include "../sky/catalog.h"
#include "../sky/catalog_stars.h"
#include "../sky/catalog_dso.h"

// main header
#include "catalog_download.h"

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CCatalogDownloadDlg
// Purpose:	Initialize add devicedialog
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCatalogDownloadDlg::CCatalogDownloadDlg( wxWindow *parent, CatalogDef* pCatalog, const wxString& title ) : wxDialog(parent, -1,
								title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	// remote locations
	wxString vectStrCatDownloadLocation[10];
	wxString vectStrCatLocalFormat[10];
	int i=0;

	m_nMapSourceId=0;
	// init catalog remote locations list
	for( i=0; i<pCatalog->m_nSourcesFull; i++ )
	{
		if( pCatalog->m_vectSourcesFull[i] == CATALOG_DOWNLOAD_SOURCE_VIZIER )
			vectStrCatDownloadLocation[i] = wxT("Vizier");
		else if( pCatalog->m_vectSourcesFull[i] == CATALOG_DOWNLOAD_SOURCE_UNIMAP )
			vectStrCatDownloadLocation[i] = wxT("Unimap");
		else if( pCatalog->m_vectSourcesFull[i] == CATALOG_DOWNLOAD_SOURCE_LEDA )
			vectStrCatDownloadLocation[i] = wxT("Leda");
		else if( pCatalog->m_vectSourcesFull[i] == CATALOG_DOWNLOAD_SOURCE_EXOPLANET_EU )
			vectStrCatDownloadLocation[i] = wxT("Exoplanet.EU");
		else if( pCatalog->m_vectSourcesFull[i] == CATALOG_DOWNLOAD_SOURCE_CELESTRAK )
			vectStrCatDownloadLocation[i] = wxT("Celestrak.Com");

		m_vectMapSourceId[i] = pCatalog->m_vectSourcesFull[i];
		m_nMapSourceId++;
	}

	// set local format types
	vectStrCatLocalFormat[0] = wxT("Binary");
	vectStrCatLocalFormat[1] = wxT("ASCII");
	vectStrCatLocalFormat[2] = wxT("Original");

	// main sizer
	wxBoxSizer *pTopSizer = new wxBoxSizer( wxVERTICAL );
	wxFlexGridSizer* sizerTable = new wxFlexGridSizer( 3, 2, 5, 10 );

	// elements - name
	m_pCatalogName = new wxStaticText( this, -1, wxString(pCatalog->m_strName,wxConvUTF8) );
	m_pCatalogName->SetForegroundColour( *wxRED );
	// full source location
	m_pCatalogRemoteLocation = new wxChoice( this, wxID_CATALOG_REMOTE_LOCATIONS,
										wxDefaultPosition, wxSize(-1,-1), pCatalog->m_nSourcesFull, 
										vectStrCatDownloadLocation, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCatalogRemoteLocation->SetSelection( 0 );
	// local file format - binary, ascii, original, etc
	m_pCatlogLocalFormat = new wxChoice( this, -1,
										wxDefaultPosition, wxSize(-1,-1), 3, 
										vectStrCatLocalFormat, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCatlogLocalFormat->SetSelection( 0 );

	///////////////////////////////////
	// Populate STARS panel
	// :: if to load remote or local
	sizerTable->Add( new wxStaticText(this, -1, _T("Catalog Name:")), 0,
							wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerTable->Add( m_pCatalogName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: download  location
	sizerTable->Add( new wxStaticText(this, -1, _T("Download Location:")), 0,
							wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerTable->Add( m_pCatalogRemoteLocation, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: local format
	sizerTable->Add( new wxStaticText(this, -1, _T("Local Format:")), 0,
							wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerTable->Add( m_pCatlogLocalFormat, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );


	pTopSizer->Add( sizerTable, 1, wxEXPAND | wxALL, 10 );
	pTopSizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxCENTRE | wxALL, 10 );

	// activate
	SetSizer( pTopSizer );
	SetAutoLayout(TRUE);
	pTopSizer->SetSizeHints(this);	
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CCatalogDownloadDlg
// Purpose:	Delete my object
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCatalogDownloadDlg::~CCatalogDownloadDlg( )
{
	delete( m_pCatalogName );
	delete( m_pCatalogRemoteLocation );
	delete( m_pCatlogLocalFormat );
}

////////////////////////////////////////////////////////////////////
// Method:	GetSourceId
// Class:	CCatalogDownloadDlg
// Purpose:	return from map between select box and real source id
// Input:	mothing
// Output:	catalog download source id
////////////////////////////////////////////////////////////////////
int CCatalogDownloadDlg::GetSourceId( )
{
	int nSelectId = m_pCatalogRemoteLocation->GetSelection( );
	return( m_vectMapSourceId[nSelectId] );
}