////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// local headers
#include "../camera/camera_canoneos.h"

// main header
#include "canoneos_setup_dlg.h"

//		***** CLASS CCanonEosSetupDlg *****
////////////////////////////////////////////////////////////////////
// CCanonEosSetupDlg EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CCanonEosSetupDlg, wxDialog)
//	EVT_TEXT_ENTER( wxID_VIEW_FIND_OBJECT_ENTRY, CCanonEosSetupDlg::OnDoFind )
//	EVT_BUTTON( wxID_VIEW_FIND_OBJECT_BUTTON, CCanonEosSetupDlg::OnDoFind )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CCanonEosSetupDlg
// Purpose:	Initialize add devicedialog
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCanonEosSetupDlg::CCanonEosSetupDlg( wxWindow *parent, const wxString& title ) : wxDialog(parent, -1,
                          title,
                          wxDefaultPosition,
                          wxDefaultSize,
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL)
{

	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 2,1,0,0 );
	topsizer->AddGrowableRow( 1 );

	/////////////////////////////
	// basic panel sizer - layout of elements
	wxFlexGridSizer* sizerData = new wxFlexGridSizer( 5,2,0,0 );
	sizerData->AddGrowableCol( 1 );

	////////////////
	// create my data objects
	m_pTVSelect = new wxChoice( this, -1, wxDefaultPosition, wxSize(150,-1), 0, NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pTVSelect->SetSelection( 0 );
	m_pISOSelect = new wxChoice( this, -1, wxDefaultPosition, wxSize(150,-1), 0, NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pISOSelect->SetSelection( 0 );
	m_pIMQSelect = new wxChoice( this, -1, wxDefaultPosition, wxSize(150,-1), 0, NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pIMQSelect->SetSelection( 0 );
	m_pAVSelect = new wxChoice( this, -1, wxDefaultPosition, wxSize(150,-1), 0, NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pAVSelect->SetSelection( 0 );
	m_pAEMSelect = new wxChoice( this, -1, wxDefaultPosition, wxSize(150,-1), 0, NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pAEMSelect->SetSelection( 0 );
	m_pMMSelect = new wxChoice( this, -1, wxDefaultPosition, wxSize(150,-1), 0, NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pMMSelect->SetSelection( 0 );

	///////////////////////////////////
	// :: TV
	sizerData->Add( new wxStaticText( this, -1, wxT("TV:") ), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerData->Add( m_pTVSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: ISO 
	sizerData->Add( new wxStaticText( this, -1, wxT("ISO:") ), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerData->Add( m_pISOSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: AV 
	sizerData->Add( new wxStaticText( this, -1, wxT("AV:") ), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerData->Add( m_pAVSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: AEM 
	sizerData->Add( new wxStaticText( this, -1, wxT("AEMode:") ), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerData->Add( m_pAEMSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: IMQ 
	sizerData->Add( new wxStaticText( this, -1, wxT("Image Quality:") ), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerData->Add( m_pIMQSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: Metering Mode 
	sizerData->Add( new wxStaticText( this, -1, wxT("Metering Mode:") ), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerData->Add( m_pMMSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// add to main sizer
	topsizer->Add( sizerData, 1, wxGROW | wxALL, 10 );
	// create buttons
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CCanonEosSetupDlg
// Purpose:	Delete my object
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CCanonEosSetupDlg::~CCanonEosSetupDlg( )
{
	m_vectTvPropertyTable.empty();
	m_vectIsoPropertyTable.empty();
	m_vectImqPropertyTable.empty();
	m_vectAvPropertyTable.empty();
	m_vectAemPropertyTable.empty();
	m_vectMmPropertyTable.empty();

	delete( m_pTVSelect );
	delete( m_pISOSelect );
	delete( m_pIMQSelect );
	delete( m_pAVSelect );
	delete( m_pAEMSelect );
	delete( m_pMMSelect );
}	

////////////////////////////////////////////////////////////////////
// Method:		SetConfig
// Class:		CCanonEosSetupDlg
// Purpose:		set config
// Input:		catalog id
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CCanonEosSetupDlg::SetConfig( CCameraCanonEos* pCamera )
{
	if( !pCamera ) return;

	InitTV();
	InitISO();
	InitIMQ();
	InitAV();
	InitAEM();
	InitMM();

	// short call to get all properties 
//	pCamera->GetPropertyCommand( kEdsPropID_Unknown );

	// :: TV :: EXPOSURE LENGTH
	SetupProperty( m_pTVSelect, m_vectMapTvSelect, m_vectTvPropertyTable, &(pCamera->m_pModel->getTvDesc()) );
	SelectProperty( m_pTVSelect, m_vectMapTvSelect, pCamera->m_pModel->getTv() );
	// :: ISO
	SetupProperty( m_pISOSelect, m_vectMapIsoSelect, m_vectIsoPropertyTable, &(pCamera->m_pModel->getIsoDesc()) );
	SelectProperty( m_pISOSelect, m_vectMapIsoSelect, pCamera->m_pModel->getIso() );
	// :: IMAGE QUALITY
	SetupProperty( m_pIMQSelect, m_vectMapImqSelect, m_vectImqPropertyTable, &(pCamera->m_pModel->getImageQualityDesc()) );
	SelectProperty( m_pIMQSelect, m_vectMapImqSelect, pCamera->m_pModel->getImageQuality() );
	// :: AV
	SetupProperty( m_pAVSelect, m_vectMapAvSelect, m_vectAvPropertyTable, &(pCamera->m_pModel->getAvDesc()) );
	SelectProperty( m_pAVSelect, m_vectMapAvSelect, pCamera->m_pModel->getAv() );
	// :: AEM
	SetupProperty( m_pAEMSelect, m_vectMapAemSelect, m_vectAemPropertyTable, &(pCamera->m_pModel->getAEModeDesc()) );
	SelectProperty( m_pAEMSelect, m_vectMapAemSelect, pCamera->m_pModel->getAEMode() );
	// :: Metering Mode
	SetupProperty( m_pMMSelect, m_vectMapMmSelect, m_vectMmPropertyTable, &(pCamera->m_pModel->getMeteringModeDesc()) );
	SelectProperty( m_pMMSelect, m_vectMapMmSelect, pCamera->m_pModel->getMeteringMode() );

}

////////////////////////////////////////////////////////////////////
void CCanonEosSetupDlg::InitTV( )
{
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x04,"Bulb"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x0c,"Bulb"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x10,"30\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x13,"25\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x14,"20\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x15,"20\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x18,"15\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x1B,"13\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x1C,"10\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x1D,"10\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x20,"8\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x23,"6\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x24,"6\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x25,"5\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x28,"4\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x2B,"3\"2"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x2C,"3\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x2D,"2\"5"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x30,"2\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x33,"1\"6"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x34,"1\"5"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x35,"1\"3"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x38,"1\""));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x3B,"0\"8"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x3C,"0\"7"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x3D,"0\"6"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x40,"0\"5"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x43,"0\"4"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x44,"0\"3"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x45,"0\"3"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x48,"4"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x4B,"5"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x4C,"6"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x4D,"6"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x50,"8"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x53,"10"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x54,"10"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x55,"13"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x58,"15"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x5B,"20"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x5C,"20"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x5D,"25"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x60,"30"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x63,"40"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x64,"45"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x65,"50"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x68,"60"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x6B,"80"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x6C,"90"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x6D,"100"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x70,"125"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x73,"160"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x74,"180"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x75,"200"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x78,"250"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x7B,"320"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x7C,"350"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x7D,"400"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x80,"500"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x83,"640"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x84,"750"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x85,"800"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x88,"1000"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x8B,"1250"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x8C,"1500"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x8D,"1600"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x90,"2000"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x93,"2500"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x94,"3000"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x95,"3200"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x98,"4000"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x9B,"5000"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x9C,"6000"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x9D,"6400"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0xA0,"8000"));
	m_vectTvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0xffffffff,"Unknown"));
}

////////////////////////////////////////////////////////////////////
void CCanonEosSetupDlg::InitISO( )
{
	// Map of value and display name
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00,"Auto"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x28,"6"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x30,"12"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x38,"25"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x40,"50"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x48,"100"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x4b,"125"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x4d,"160"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x50,"200"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x53,"250"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x55,"320"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x58,"400"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x5b,"500"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x5d,"640"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x60,"800"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x63,"1000"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x65,"1250"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x68,"1600"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x70,"3200"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x78,"6400"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x80,"12800"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x88,"25600"));
	m_vectIsoPropertyTable.insert( std::pair<EdsUInt32, const char *>(0xffffffff,"Unknown"));
}

////////////////////////////////////////////////////////////////////
void CCanonEosSetupDlg::InitIMQ( )
{
	// PTP Camera
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00640f0f, "RAW"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02640f0f, "Small RAW"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00640013, "RAW + Large Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00640113, "RAW + Middle Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00640213, "RAW + Small Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00640012, "RAW + Large Normal Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00640112, "RAW + Middle Normal Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00640212, "RAW + Small Normal Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00130f0f, "Large Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x01130f0f, "Middle Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02130f0f, "Small Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00120f0f, "Large Normal Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x01120f0f, "Middle Normal Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02120f0f, "Small Normal Jpeg"));

	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00640010, "RAW + Large Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00640510, "RAW + Middle1 Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00640610, "RAW + Middle2 Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00640210, "RAW + Small Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00100f0f, "Large Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x05100f0f, "Middle1 Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x06100f0f, "Middle2 Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02100f0f, "Small Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02640010, "Small RAW + Large Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02640510, "Small RAW + Middle1 Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02640610, "Small RAW + Middle2 Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02640210, "Small RAW + Small Jpeg"));

	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x01640f0f, "Small RAW1"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x01640013, "Small RAW1 + Large Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x01640113, "Small RAW1 + Middle Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x01640213, "Small RAW1 + Small Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x01640012, "Small RAW1 + Large Normal Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x01640112, "Small RAW1 + Middle Normal Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x01640212, "Small RAW1 + Small Normal Jpeg"));

	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02640013, "Small RAW + Large Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02640113, "Small RAW + Middle Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02640213, "Small RAW + Small Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02640012, "Small RAW + Large Normal Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02640112, "Small RAW + Middle Normal Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02640212, "Small RAW + Small Normal Jpeg"));

	// Legacy Camera
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00240000, "RAW"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00240013, "RAW + Large Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00240113, "RAW + Middle Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00240213, "RAW + Small Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00240012, "RAW + Large Normal Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00240112, "RAW + Middle Normal Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00240212, "RAW + Small Normal Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00130000, "Large Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x01130000, "Middle Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02130000, "Small Fine Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00120000, "Large Normal Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x01120000, "Middle Normal Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x02120000, "Small Normal Jpeg"));

	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x002f000f, "RAW"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x002f001f, "RAW + Large Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x002f051f, "RAW + Middle1 Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x002f061f, "RAW + Middle2 Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x002f021f, "RAW + Small Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x001f000f, "Large Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x051f000f, "Middle1 Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x061f000f, "Middle2 Jpeg"));
	m_vectImqPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x021f000f, "Small Jpeg"));
}

////////////////////////////////////////////////////////////////////
void CCanonEosSetupDlg::InitAV( )
{
	// Map of value and display name
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x00,"00"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x08,"1"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x0B,"1.1"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x0C,"1.2"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x0D,"1.2"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x10,"1.4"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x13,"1.6"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x14,"1.8"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x15,"1.8"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x18,"2"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x1B,"2.2"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x1C,"2.5"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x1D,"2.5"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x20,"2.8"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x23,"3.2"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x24,"3.5"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x25,"3.5"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x28,"4"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x2B,"4.5"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x2C,"4.5"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x2D,"5.0"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x30,"5.6"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x33,"6.3"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x34,"6.7"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x35,"7.1"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x38,"8"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x3B,"9"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x3C,"9.5"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x3D,"10"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x40,"11"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x43,"13"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x44,"13"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x45,"14"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x48,"16"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x4B,"18"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x4C,"19"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x4D,"20"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x50,"22"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x53,"25"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x54,"27"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x55,"29"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x58,"32"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x5B,"36"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x5C,"38"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x5D,"40"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x60,"45"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x63,"51"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x64,"54"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x65,"57"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x68,"64"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x6B,"72"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x6C,"76"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x6D,"80"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0x70,"91"));
	m_vectAvPropertyTable.insert( std::pair<EdsUInt32, const char *>(0xffffffff,"Unknown"));
}

////////////////////////////////////////////////////////////////////
void CCanonEosSetupDlg::InitAEM( )
{
	// Map of value and display name
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(0,"P"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(1,"Tv"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(2,"Av"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(3,"M"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(4,"Bulb"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(5,"A-DEP"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(6,"DEP"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(7,"C"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(8,"Lock"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(9,"GreenMode"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(10,"Night Portrait"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(11,"Sports"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(13,"LandScape"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(14,"Close Up"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(15,"No Strobo"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(12,"Portrait"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(19,"Creative Auto"));
	m_vectAemPropertyTable.insert( std::pair<EdsUInt32, const char *>(0xffffffff,"Unknown"));
}

////////////////////////////////////////////////////////////////////
void CCanonEosSetupDlg::InitMM( )
{
	// Map of value and display name
	m_vectMmPropertyTable.insert( std::pair<EdsUInt32, const char *>(1,"Spot"));
	m_vectMmPropertyTable.insert( std::pair<EdsUInt32, const char *>(3,"Evaluative"));
	m_vectMmPropertyTable.insert( std::pair<EdsUInt32, const char *>(4,"Partial"));
	m_vectMmPropertyTable.insert( std::pair<EdsUInt32, const char *>(5,"Center-Weighted Average"));
	m_vectMmPropertyTable.insert( std::pair<EdsUInt32, const char *>(0xffffffff,"Unknown"));
}

////////////////////////////////////////////////////////////////////
void CCanonEosSetupDlg::SetupProperty( wxChoice* pSelect, long* vectMap, 
									std::map<EdsUInt32, const char *>& vectPropertyTable, 
									const EdsPropertyDesc* desc )
{
	pSelect->Clear();
	// check if empty
	if( desc->numElements <= 0 )
	{
		pSelect->Disable();
		return;
	}

	int cnt=0;
	for(int i = 0; i < desc->numElements; i++)
	{
		// The character string corresponding to data is acquired.
		std::map<EdsUInt32, const char *>::iterator itr = vectPropertyTable.find((EdsUInt32)desc->propDesc[i]);

		// Create list of combo box
		if (itr != vectPropertyTable.end())
		{
			// Insert string
			pSelect->Append( wxString(itr->second,wxConvUTF8) );
			// Set select map
			vectMap[cnt]=itr->first;
			cnt++;
		}
	}
}

////////////////////////////////////////////////////////////////////
void CCanonEosSetupDlg::SelectProperty( wxChoice* pSelect, long* vectMap, EdsUInt32 value )
{
	int nSelected=0, i=0;

	for( i=0; i<pSelect->GetCount(); i++ )
	{
		if( vectMap[i] == value ) 
		{
			nSelected = i;
			break;
		}
	}

	pSelect->SetSelection( nSelected );
}

////////////////////////////////////////////////////////////////////
EdsUInt32 CCanonEosSetupDlg::GetProperty( wxChoice* pSelect, long* vectMap )
{
	int nSelected = pSelect->GetSelection();
	if( nSelected < 0 ) return( -1 );

	return( vectMap[nSelected] );
}

////////////////////////////////////////////////////////////////////
// Method:		OnDoFind
// Class:		CCanonEosSetupDlg
// Purpose:		when find button pressed
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
/*void CCanonEosSetupDlg::OnDoFind( wxCommandEvent& event )
{
	EndModal( wxID_OK );

	return;
}
*/
