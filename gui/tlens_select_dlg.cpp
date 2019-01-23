
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/wxprec.h"

// local include
#include "../observer/observer.h"
#include "../camera/camera.h"
#include "../telescope/telescope.h"

// main header
#include "tlens_select_dlg.h"

// CTLensSelectDlg EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CTLensSelectDlg, wxDialog )
	EVT_CHOICE( wxID_TLENS_SELECT_SOURCE, CTLensSelectDlg::OnSourceType )
	// main lens
	EVT_CHOICE( wxID_MAIN_LENS_SELECT_TYPE, CTLensSelectDlg::OnSelectMainLens )
	EVT_CHOICE( wxID_MAIN_LENS_SELECT_OPTICS, CTLensSelectDlg::OnSelectMainLens )
	EVT_CHOICE( wxID_MAIN_LENS_SELECT_BRAND, CTLensSelectDlg::OnSelectMainLens )
	EVT_CHOICE( wxID_MAIN_LENS_SELECT_NAME, CTLensSelectDlg::OnSelectMainLens )
	// barlow lens
	EVT_CHECKBOX( wxID_CHECK_USE_BARLOW_LENS, CTLensSelectDlg::OnUseSystemPart )
	EVT_CHOICE( wxID_BARLOW_LENS_SELECT_BRAND, CTLensSelectDlg::OnSelectOtherLens )
	EVT_CHOICE( wxID_BARLOW_LENS_SELECT_NAME, CTLensSelectDlg::OnSelectOtherLens )
	// focal reducer
	EVT_CHECKBOX( wxID_CHECK_USE_FOCAL_REDUCER, CTLensSelectDlg::OnUseSystemPart )
	EVT_CHOICE( wxID_FOCAL_REDUCER_SELECT_BRAND, CTLensSelectDlg::OnSelectOtherLens )
	EVT_CHOICE( wxID_FOCAL_REDUCER_SELECT_BRAND, CTLensSelectDlg::OnSelectOtherLens )
	// eyepiece
	EVT_CHECKBOX( wxID_CHECK_USE_EYEPIECE, CTLensSelectDlg::OnUseSystemPart )
	EVT_CHOICE( wxID_EYEPIECE_SELECT_TYPE, CTLensSelectDlg::OnSelectOtherLens )
	EVT_CHOICE( wxID_EYEPIECE_SELECT_BRAND, CTLensSelectDlg::OnSelectOtherLens )
	EVT_CHOICE( wxID_EYEPIECE_SELECT_NAME, CTLensSelectDlg::OnSelectOtherLens )
	// camera lens
	EVT_CHECKBOX( wxID_CHECK_USE_CAMERA_LENS, CTLensSelectDlg::OnUseSystemPart )
	EVT_CHOICE( wxID_CAMERA_LENS_SELECT_TYPE, CTLensSelectDlg::OnSelectOtherLens )
	EVT_CHOICE( wxID_CAMERA_LENS_SELECT_BRAND, CTLensSelectDlg::OnSelectOtherLens )
	EVT_CHOICE( wxID_CAMERA_LENS_SELECT_NAME, CTLensSelectDlg::OnSelectOtherLens )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CTLensSelectDlg
// Purpose:	Initialize object
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CTLensSelectDlg::CTLensSelectDlg( wxWindow *parent, wxString title, int nType ) 
						: wxDialog( parent, -1,
                          title, wxDefaultPosition,
                          wxDefaultSize, //wxSize( 350, 310 ),
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	m_nDlgType = nType;
	m_nSourceId = 0;
	// reset lesn system
	memset( &m_rLensSetup, 0, sizeof( DefTLensSetup ) );

	// define type of sources
	wxString vectSourceType[3];
	vectSourceType[0] = wxT("All Defined");
	vectSourceType[1] = wxT("What I Own");
	// define types of main lenses
	wxString vectLensType[2];
	vectLensType[0] = wxT("Telescope");
	vectLensType[1] = wxT("Camera Lens");

	// main dialog sizer
	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 3, 1, 0, 0 );

	int n_row = 2;
//	wxStaticBoxSizer* sizerOutputBox;
	wxFlexGridSizer *sizerOutputData;
	m_pFocalVal = NULL;
	m_pFocalRatioVal = NULL;
	wxPanel* panelOut;
	if( nType == 1 )
	{
		n_row = 3;
		//////////////
		// output box sizer
		//wxStaticBox* pOuputBox = new wxStaticBox( this, -1, wxT("Output"), wxPoint(-1,-1), wxDefaultSize );
		//sizerOutputBox = new wxStaticBoxSizer( pOuputBox, wxHORIZONTAL );
		sizerOutputData = new wxFlexGridSizer( 1, 4, 0, 5 );
		panelOut = new wxPanel( this, -1 );
		panelOut->SetBackgroundColour( *wxBLACK );
		panelOut->SetForegroundColour( *wxRED );
		panelOut->SetSizer( sizerOutputData );
		//sizerOutputBox->Add( sizerOutputData, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTER );

		///////////
		// elemensts
		m_pFocalVal = new wxTextCtrl( panelOut, -1, wxT("0.0"), wxDefaultPosition, wxSize(100,-1), wxTE_READONLY );
		m_pFocalVal->SetBackgroundColour( *wxLIGHT_GREY );
//		m_pFocalVal->SetForegroundColour( *wxRED );
		m_pFocalRatioVal = new wxTextCtrl( panelOut, -1, wxT("0.0"), wxDefaultPosition, wxSize(100,-1), wxTE_READONLY );
		m_pFocalRatioVal->SetBackgroundColour( *wxLIGHT_GREY );
//		m_pFocalVal->SetForegroundColour( *wxRED );
	}

	// select source sizer
	wxFlexGridSizer *sizerSource = new wxFlexGridSizer( 1, n_row, 0, 5 );
	if( nType == 0 ) sizerSource->AddGrowableCol( 0 );
	sizerSource->AddGrowableCol( 1 );
	topsizer->Add( sizerSource, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_BOTTOM|wxALL|wxEXPAND, 10 );

	// hardware static box/sizer
	wxStaticBox* pSetupBox = new wxStaticBox( this, -1, wxT("Lens System Setup"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerSetupBox = new wxStaticBoxSizer( pSetupBox, wxHORIZONTAL );
	// hardware data sizer
	wxFlexGridSizer *sizerSetupData = new wxFlexGridSizer( 4, 2, 3, 10 );
	sizerSetupBox->Add( sizerSetupData, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 10 );
	topsizer->Add( sizerSetupBox, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 10 );
	
	///////////////
	// Source setup elements
	m_pSourceSelect = new wxChoice( this, wxID_TLENS_SELECT_SOURCE, 
										wxDefaultPosition, wxSize(-1,-1), 2, 
										vectSourceType, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSourceSelect->SetSelection(0);

	////////////////
	// Main Lens elements
	m_pLensLabel = new wxStaticText( this, -1, wxT("Main Lens System:") );
	m_pLensTypeSelect = new wxChoice( this, wxID_MAIN_LENS_SELECT_TYPE, wxDefaultPosition, wxSize(-1,-1), 2, 
										vectLensType, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pLensTypeSelect->SetSelection(0);
	m_pLensOpticsSelect = new wxChoice( this, wxID_MAIN_LENS_SELECT_OPTICS, wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pLensBrandSelect = new wxChoice( this, wxID_MAIN_LENS_SELECT_BRAND, wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pLensNameSelect= new wxChoice( this, wxID_MAIN_LENS_SELECT_NAME, wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	////////////////
	// Barlow Lens elements
	m_pBarlowLensLabel = new wxCheckBox( this, wxID_CHECK_USE_BARLOW_LENS, wxT("Barlow Lens:") );
	m_pBarlowLensBrandSelect = new wxChoice( this, wxID_BARLOW_LENS_SELECT_BRAND, wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pBarlowLensNameSelect = new wxChoice( this, wxID_BARLOW_LENS_SELECT_NAME, wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	////////////////
	// Focal reducer elements
	m_pFocalReducerLabel = new wxCheckBox( this, wxID_CHECK_USE_FOCAL_REDUCER, wxT("Focal Reducer:") );
	m_pFocalReducerBrandSelect = new wxChoice( this, wxID_FOCAL_REDUCER_SELECT_BRAND, wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pFocalReducerNameSelect = new wxChoice( this, wxID_FOCAL_REDUCER_SELECT_NAME, wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	////////////////
	// Eyepiece elements
	m_pEyepieceLabel = new wxCheckBox( this, wxID_CHECK_USE_EYEPIECE, wxT("Eyepiece:") );
	m_pEyepieceTypeSelect = new wxChoice( this, wxID_EYEPIECE_SELECT_TYPE, wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pEyepieceBrandSelect = new wxChoice( this, wxID_EYEPIECE_SELECT_BRAND, wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pEyepieceNameSelect = new wxChoice( this, wxID_EYEPIECE_SELECT_NAME, wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	////////////////
	// Camera Lens elements
	m_pCameraLensLabel = new wxCheckBox( this, wxID_CHECK_USE_CAMERA_LENS, wxT("Camera Lens:") );
	m_pCameraLensTypeSelect = new wxChoice( this, wxID_CAMERA_LENS_SELECT_TYPE, wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCameraLensBrandSelect = new wxChoice( this, wxID_CAMERA_LENS_SELECT_BRAND, wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCameraLensNameSelect = new wxChoice( this, wxID_CAMERA_LENS_SELECT_NAME, wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////////
	// LAYOUT OF ELEMENTS IN SIZERS
	// :: SOURCE
	sizerSource->Add( new wxStaticText( this, -1, wxT("Filter Select From:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerSource->Add( m_pSourceSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	if( nType == 1 ) 
	{
		sizerSource->Add( panelOut, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND );

		sizerOutputData->Add( new wxStaticText( panelOut, -1, wxT("Focal Length:") ), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
		sizerOutputData->Add( m_pFocalVal, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );

		sizerOutputData->Add( new wxStaticText( panelOut, -1, wxT("Focal Ratio:") ), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL );
		sizerOutputData->Add( m_pFocalRatioVal, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	}

	///////////////////////////
	// :: MAIN LENS
	sizerSetupData->Add( m_pLensLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerSetupDataMainLens = new wxFlexGridSizer( 1, 4, 0, 5 );
	sizerSetupDataMainLens->Add( m_pLensTypeSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupDataMainLens->Add( m_pLensOpticsSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupDataMainLens->Add( m_pLensBrandSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupDataMainLens->Add( m_pLensNameSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupData->Add( sizerSetupDataMainLens, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	///////////////////////////
	// :: BARLOW LENS
	sizerSetupData->Add( m_pBarlowLensLabel, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerSetupDataBarlowLens = new wxFlexGridSizer( 1, 2, 0, 5 );
	sizerSetupDataBarlowLens->Add( m_pBarlowLensBrandSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupDataBarlowLens->Add( m_pBarlowLensNameSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupData->Add( sizerSetupDataBarlowLens, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	///////////////////////////
	// :: FOCAL REDUCER
	sizerSetupData->Add( m_pFocalReducerLabel, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerSetupDataFocalReducer = new wxFlexGridSizer( 1, 2, 0, 5 );
	sizerSetupDataFocalReducer->Add( m_pFocalReducerBrandSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupDataFocalReducer->Add( m_pFocalReducerNameSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupData->Add( sizerSetupDataFocalReducer, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	///////////////////////////
	// :: EYEPIECE
	sizerSetupData->Add( m_pEyepieceLabel, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerSetupDataEyepiece = new wxFlexGridSizer( 1, 3, 0, 5 );
	sizerSetupDataEyepiece->Add( m_pEyepieceTypeSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupDataEyepiece->Add( m_pEyepieceBrandSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupDataEyepiece->Add( m_pEyepieceNameSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupData->Add( sizerSetupDataEyepiece, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	///////////////////////////
	// :: CAMERA LENS
	sizerSetupData->Add( m_pCameraLensLabel, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerSetupDataCamLens = new wxFlexGridSizer( 1, 3, 0, 5 );
	sizerSetupDataCamLens->Add( m_pCameraLensTypeSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupDataCamLens->Add( m_pCameraLensBrandSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupDataCamLens->Add( m_pCameraLensNameSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupData->Add( sizerSetupDataCamLens, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// add bottom buttons 
	if( nType == 0 ) topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL ), 1, wxALIGN_CENTER|wxCENTRE|wxBOTTOM|wxLEFT|wxRIGHT, 10 );

	// set default states
	SetBalowLensState( 0, 0 );
	SetFocalReducerState( 0, 0 );
	SetEyepieceState( 0, 0 );
	SetCameraLensState( 0, 0 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
	//topsizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CTLensSelectDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CTLensSelectDlg::~CTLensSelectDlg( ) 
{
	if( m_nDlgType == 1 ) 
	{
		delete( m_pFocalVal );
		delete( m_pFocalRatioVal );
	}

	delete( m_pSourceSelect );
	// main lens
	delete( m_pLensLabel );
	delete( m_pLensTypeSelect );
	delete( m_pLensOpticsSelect );
	delete( m_pLensBrandSelect );
	delete( m_pLensNameSelect );
	// barlow lens
	delete( m_pBarlowLensLabel );
	delete( m_pBarlowLensBrandSelect );
	delete( m_pBarlowLensNameSelect );
	// focal reducer
	delete( m_pFocalReducerLabel );
	delete( m_pFocalReducerBrandSelect );
	delete( m_pFocalReducerNameSelect );
	// eyepiece
	delete( m_pEyepieceLabel );
	delete( m_pEyepieceTypeSelect );
	delete( m_pEyepieceBrandSelect );
	delete( m_pEyepieceNameSelect );
	// camera lens
	delete( m_pCameraLensLabel );
	delete( m_pCameraLensTypeSelect );
	delete( m_pCameraLensBrandSelect );
	delete( m_pCameraLensNameSelect );
}

////////////////////////////////////////////////////////////////////
void CTLensSelectDlg::SetData( CObserver* pObserver, CCamera* pCamera, CTelescope* pTelescope, int nSourceId, DefTLensSetup& rLensSetup )
{

	m_pObserver = pObserver;
	m_pCamera = pCamera;
	m_pTelescope = pTelescope;

	// source id
	m_nSourceId = nSourceId;

	// copy lens setup localy
	memcpy( &m_rLensSetup, &rLensSetup, sizeof( DefTLensSetup ) );

	m_pSourceSelect->SetSelection( m_nSourceId );
	m_pLensTypeSelect->SetSelection( m_rLensSetup.nLensType );
	// set other lens uses
	m_pBarlowLensLabel->SetValue( m_rLensSetup.bBarlow );
	m_pFocalReducerLabel->SetValue( m_rLensSetup.bFocalReducer );
	m_pEyepieceLabel->SetValue( m_rLensSetup.bEyepiece );
	m_pCameraLensLabel->SetValue( m_rLensSetup.bCameraLens );

	SetSetupData( );

	// set check states
	SetBalowLensState( 0, m_rLensSetup.bBarlow );
	SetFocalReducerState( 0, m_rLensSetup.bFocalReducer );
	SetEyepieceState( 0, m_rLensSetup.bEyepiece );
	SetCameraLensState( 0, m_rLensSetup.bCameraLens );

	/////////
	// if calc type
	if( m_nDlgType == 1 ) SetFocalValues( );

	GetSizer()->Layout();
	Layout();
	Fit( );
	Refresh( false );
}

////////////////////////////////////////////////////////////////////
void CTLensSelectDlg::SetSetupData( )
{
	int i=0;
	// if to select from all defines
	if( m_nSourceId == 0 )
	{
		// main lens
		m_pLensOpticsSelect->Show();
		m_pLensBrandSelect->Show();
		m_pBarlowLensBrandSelect->Show();
		m_pFocalReducerBrandSelect->Show();
		m_pEyepieceTypeSelect->Show();
		m_pEyepieceBrandSelect->Show();
		m_pCameraLensTypeSelect->Show();
		m_pCameraLensBrandSelect->Show();

		////////////
		// MAIN LENS
		if( m_rLensSetup.nLensType == 1 )
		{
			m_pCamera->LoadHardwareDef( CAMERA_HARDWARE_LENS, -1, m_rLensSetup.nLensBrand );
			m_pCamera->SetGuiSelect( CAMERA_HARDWARE_LENS, m_pLensOpticsSelect, m_pLensBrandSelect, m_pLensNameSelect, 
									m_rLensSetup.nLensOptics, m_rLensSetup.nLensBrand, m_rLensSetup.nLensName,
									m_vectLensBrandId, m_vectLensNameId );
			// hide other options
			SetBalowLensState( 1, 0 );
			SetFocalReducerState( 1, 0 );
			SetEyepieceState( 1, 0 );
			SetCameraLensState( 1, 0 );

		} else if( m_rLensSetup.nLensType == 0 )
		{
			m_pTelescope->LoadHardwareDef( TELESCOPE_HARDWARE_MAIN_SCOPE, -1, m_rLensSetup.nLensBrand );
			m_pTelescope->SetGuiSelect( TELESCOPE_HARDWARE_MAIN_SCOPE, m_pLensOpticsSelect, m_pLensBrandSelect, m_pLensNameSelect, 
										m_rLensSetup.nLensOptics, m_rLensSetup.nLensBrand, m_rLensSetup.nLensName,
										m_vectLensBrandId, m_vectLensNameId );
			// show other options
			SetBalowLensState( 1, 1 );
			SetFocalReducerState( 1, 1 );
			SetEyepieceState( 1, 1 );
			SetCameraLensState( 1, 1 );
		}

		////////////
		// BARLOW LENS
		m_pTelescope->LoadHardwareDef( TELESCOPE_HARDWARE_BARLOW_LENS, m_rLensSetup.nLensBrand );
		m_pTelescope->SetGuiSelect( TELESCOPE_HARDWARE_BARLOW_LENS, NULL, m_pBarlowLensBrandSelect, m_pBarlowLensNameSelect, 
									0, m_rLensSetup.nBarlowLensBrand, m_rLensSetup.nBarlowLensName,
									m_vectBarlowLensBrandId, m_vectBarlowLensNameId );

		////////////
		// FOCAL REDUCER
		m_pTelescope->LoadHardwareDef( TELESCOPE_HARDWARE_FOCAL_REDUCER, m_rLensSetup.nLensBrand );
		m_pTelescope->SetGuiSelect( TELESCOPE_HARDWARE_FOCAL_REDUCER, NULL, m_pFocalReducerBrandSelect, m_pFocalReducerNameSelect, 
									0, m_rLensSetup.nFocalReducerBrand, m_rLensSetup.nFocalReducerName,
									m_vectFocalReducerBrandId, m_vectFocalReducerNameId );

		////////////
		// EYEPIECE
		m_pTelescope->LoadHardwareDef( TELESCOPE_HARDWARE_EYEPIECE, m_rLensSetup.nLensBrand );
		m_pTelescope->SetGuiSelect( TELESCOPE_HARDWARE_EYEPIECE, m_pEyepieceTypeSelect, m_pEyepieceBrandSelect, 
									m_pEyepieceNameSelect, m_rLensSetup.nEyepieceType, m_rLensSetup.nEyepieceBrand, 
									m_rLensSetup.nEyepieceName, m_vectEyepieceBrandId, m_vectEyepieceNameId );

		////////////
		// CAMERA LENS
		m_pCamera->LoadHardwareDef( CAMERA_HARDWARE_LENS, -1, m_rLensSetup.nCameraLensBrand );
		m_pCamera->SetGuiSelect( CAMERA_HARDWARE_LENS, m_pCameraLensTypeSelect, m_pCameraLensBrandSelect, m_pCameraLensNameSelect, 
									m_rLensSetup.nCameraLensOptics, m_rLensSetup.nCameraLensBrand, 
									m_rLensSetup.nCameraLensName, m_vectCameraLensBrandId, m_vectCameraLensNameId );

	// or if to select from own hardware
	} else if( m_nSourceId == 1 )
	{
		// main lens
		m_pLensOpticsSelect->Hide();
		m_pLensBrandSelect->Hide();
		m_pBarlowLensBrandSelect->Hide();
		m_pFocalReducerBrandSelect->Hide();
		m_pEyepieceTypeSelect->Hide();
		m_pEyepieceBrandSelect->Hide();
		m_pCameraLensTypeSelect->Hide();
		m_pCameraLensBrandSelect->Hide();

		////////////
		// MAIN LENS
		if( m_rLensSetup.nLensType == 1 )
		{
			// set cameras
			m_pLensNameSelect->Clear();
			if( m_pObserver->m_pHardware->m_vectCameraLens.size() > 0 )
			{
				//m_pLensNameSelect->Enable();
				for( i=0; i<m_pObserver->m_pHardware->m_vectCameraLens.size(); i++ ) 
					m_pLensNameSelect->Append( m_pObserver->m_pHardware->GetCameraLensFullName( i ) );
				m_pLensNameSelect->SetSelection( m_rLensSetup.nLensNameOwn );
			} else
				m_pLensNameSelect->Disable();

		}  else if( m_rLensSetup.nLensType == 0 )
		{
			// set telescope
			m_pLensNameSelect->Clear();
			if( m_pObserver->m_pHardware->m_vectTelescope.size() > 0 )
			{
				//m_pLensNameSelect->Enable();
				for( i=0; i<m_pObserver->m_pHardware->m_vectTelescope.size(); i++ ) 
					m_pLensNameSelect->Append( m_pObserver->m_pHardware->GetTelescopeFullName( i ) );
				m_pLensNameSelect->SetSelection( m_rLensSetup.nLensNameOwn );
			} else
				m_pLensNameSelect->Disable();
		}

		////////////
		// BARLOW LENS
		m_pBarlowLensNameSelect->Clear();
		if( m_pObserver->m_pHardware->m_vectBarlowLens.size() > 0 )
		{
			//m_pBarlowLensNameSelect->Enable();
			for( i=0; i<m_pObserver->m_pHardware->m_vectBarlowLens.size(); i++ ) 
				m_pBarlowLensNameSelect->Append( m_pObserver->m_pHardware->GetBarlowLensFullName( i ) );
			m_pBarlowLensNameSelect->SetSelection( m_rLensSetup.nBarlowLensNameOwn );
		} else
			m_pBarlowLensNameSelect->Disable();

		////////////
		// FOCAL REDUCER
		m_pFocalReducerNameSelect->Clear();
		if( m_pObserver->m_pHardware->m_vectFocalReducer.size() > 0 )
		{
			//m_pFocalReducerNameSelect->Enable();
			for( i=0; i<m_pObserver->m_pHardware->m_vectFocalReducer.size(); i++ ) 
				m_pFocalReducerNameSelect->Append( m_pObserver->m_pHardware->GetFocalReducerFullName( i ) );
			m_pFocalReducerNameSelect->SetSelection( m_rLensSetup.nFocalReducerNameOwn );
		} else
			m_pFocalReducerNameSelect->Disable();

		////////////
		// EYEPIECE
		m_pEyepieceNameSelect->Clear();
		if( m_pObserver->m_pHardware->m_vectEyepiece.size() > 0 )
		{
			//m_pEyepieceNameSelect->Enable();
			for( i=0; i<m_pObserver->m_pHardware->m_vectEyepiece.size(); i++ ) 
				m_pEyepieceNameSelect->Append( m_pObserver->m_pHardware->GetEyepieceFullName( i ) );
			m_pEyepieceNameSelect->SetSelection( m_rLensSetup.nEyepieceNameOwn );
		} else
			m_pEyepieceNameSelect->Disable();

		///////////////////
		// CAMERA LENS
		m_pCameraLensNameSelect->Clear();
		if( m_pObserver->m_pHardware->m_vectCameraLens.size() > 0 )
		{
			//m_pCameraLensNameSelect->Enable();
			for( i=0; i<m_pObserver->m_pHardware->m_vectCameraLens.size(); i++ ) 
				m_pCameraLensNameSelect->Append( m_pObserver->m_pHardware->GetCameraLensFullName( i ) );
			m_pCameraLensNameSelect->SetSelection( m_rLensSetup.nCameraLensNameOwn );
		} else
			m_pCameraLensNameSelect->Disable();

	}
}

////////////////////////////////////////////////////////////////////
// Method:	OnSelectMainLens
////////////////////////////////////////////////////////////////////
void CTLensSelectDlg::OnSelectMainLens( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	int nBrandId = GetLensBrandId();

	if( nId == wxID_MAIN_LENS_SELECT_TYPE || nId == wxID_MAIN_LENS_SELECT_OPTICS )
	{
		// get my hardware type
		m_rLensSetup.nLensType = m_pLensTypeSelect->GetSelection();
		// get lens sub type
		m_rLensSetup.nLensOptics = m_pLensOpticsSelect->GetSelection();
		m_rLensSetup.nLensBrand = GetLensBrandId();
		m_rLensSetup.nLensName = 0;

//		m_rLensSetup.nLensType = m_pLensTypeSelect->GetSelection();
		SetSetupData( );

	} else if( nId == wxID_MAIN_LENS_SELECT_BRAND )
	{
		if( m_rLensSetup.nLensType == 0 )
		{
			m_pTelescope->LoadHardwareIniByBrandId( TELESCOPE_HARDWARE_MAIN_SCOPE, nBrandId );
			m_pTelescope->SetGuiNameSelect( TELESCOPE_HARDWARE_MAIN_SCOPE, m_pLensNameSelect, 
											m_rLensSetup.nLensOptics, nBrandId, 0, m_vectLensNameId );

		} else if( m_rLensSetup.nLensType == 1 )
		{
			m_pCamera->LoadHardwareIniByBrandId( CAMERA_HARDWARE_LENS, nBrandId );
			m_pCamera->SetGuiNameSelect( CAMERA_HARDWARE_LENS, m_pLensNameSelect, 
										m_rLensSetup.nLensOptics, nBrandId, 0, m_vectLensNameId );
		}
	}

	/////////
	// if calc type
	if( m_nDlgType == 1 ) SetFocalValues( );


	GetSizer()->Layout();
	Layout();
	Fit( );
	Refresh( false );
}

////////////////////////////////////////////////////////////////////
// Method:	OnSelectOtherLens
////////////////////////////////////////////////////////////////////
void CTLensSelectDlg::OnSelectOtherLens( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
//	int nBrandId=0; 

	if( nId == wxID_BARLOW_LENS_SELECT_BRAND )
	{
		m_rLensSetup.nBarlowLensBrand = GetBarlowLensBrandId();
		m_pTelescope->LoadHardwareIniByBrandId( TELESCOPE_HARDWARE_BARLOW_LENS, m_rLensSetup.nBarlowLensBrand );
		m_pTelescope->SetGuiNameSelect( TELESCOPE_HARDWARE_BARLOW_LENS, m_pBarlowLensNameSelect, 0, 
										m_rLensSetup.nBarlowLensBrand, 0, m_vectBarlowLensNameId );

	} else if( nId == wxID_FOCAL_REDUCER_SELECT_BRAND )
	{
		m_rLensSetup.nFocalReducerBrand = GetFocalReducerBrandId();
		m_pTelescope->LoadHardwareIniByBrandId( TELESCOPE_HARDWARE_FOCAL_REDUCER, m_rLensSetup.nFocalReducerBrand );
		m_pTelescope->SetGuiNameSelect( TELESCOPE_HARDWARE_FOCAL_REDUCER, m_pFocalReducerNameSelect, 0, 
										m_rLensSetup.nFocalReducerBrand, 0, m_vectFocalReducerNameId );

	} else if( nId == wxID_EYEPIECE_SELECT_TYPE )
	{
		m_rLensSetup.nEyepieceType = m_pEyepieceTypeSelect->GetSelection();
		m_rLensSetup.nEyepieceBrand = GetEyepieceBrandId();
		m_rLensSetup.nEyepieceName = 0;

		m_pTelescope->SetGuiSelect( TELESCOPE_HARDWARE_EYEPIECE, NULL, m_pEyepieceBrandSelect, m_pEyepieceNameSelect, 
									m_rLensSetup.nEyepieceType, m_rLensSetup.nEyepieceBrand, 
										m_rLensSetup.nEyepieceName, m_vectEyepieceBrandId, m_vectEyepieceNameId );

	} else if( nId == wxID_EYEPIECE_SELECT_BRAND )
	{
		m_rLensSetup.nEyepieceBrand = GetEyepieceBrandId();
		m_pTelescope->LoadHardwareIniByBrandId( TELESCOPE_HARDWARE_EYEPIECE, m_rLensSetup.nEyepieceBrand );
		m_pTelescope->SetGuiNameSelect( TELESCOPE_HARDWARE_EYEPIECE, m_pEyepieceNameSelect, m_rLensSetup.nEyepieceType, 
										m_rLensSetup.nEyepieceBrand, 0, m_vectEyepieceNameId );

	} else if( nId == wxID_CAMERA_LENS_SELECT_TYPE )
	{
		m_rLensSetup.nCameraLensOptics = m_pCameraLensTypeSelect->GetSelection();
		m_rLensSetup.nCameraLensBrand = GetEyepieceBrandId();
		m_rLensSetup.nCameraLensName = 0;

		m_pCamera->SetGuiSelect( CAMERA_HARDWARE_LENS, NULL, m_pCameraLensBrandSelect, m_pCameraLensNameSelect, 
									m_rLensSetup.nCameraLensOptics, m_rLensSetup.nCameraLensBrand, 
										m_rLensSetup.nCameraLensName, m_vectCameraLensBrandId, m_vectCameraLensNameId );

	} else if( nId == wxID_CAMERA_LENS_SELECT_BRAND )
	{
		m_rLensSetup.nCameraLensBrand = GetCameraLensBrandId();
		m_pCamera->LoadHardwareIniByBrandId( CAMERA_HARDWARE_LENS, m_rLensSetup.nCameraLensBrand );
		m_pCamera->SetGuiNameSelect( CAMERA_HARDWARE_LENS, m_pCameraLensNameSelect, m_rLensSetup.nCameraLensOptics, 
										m_rLensSetup.nCameraLensBrand, m_rLensSetup.nCameraLensName, m_vectCameraLensNameId );
	}

	/////////
	// if calc type
	if( m_nDlgType == 1 ) SetFocalValues( );

	GetSizer()->Layout();
	Layout();
	Fit( );
	Refresh( false );
}

////////////////////////////////////////////////////////////////////
// Method:	OnSourceType
////////////////////////////////////////////////////////////////////
void CTLensSelectDlg::OnSourceType( wxCommandEvent& pEvent )
{
	int nSourceId = m_pSourceSelect->GetSelection();
	// check if the same as prev source
	if( m_nSourceId == nSourceId ) return;


	// keep last set
	if( m_nSourceId == 0 )
	{
		m_rLensSetup.nLensType = m_pLensTypeSelect->GetSelection();
		m_rLensSetup.nLensOptics = m_pLensOpticsSelect->GetSelection();
		m_rLensSetup.nLensBrand = GetLensBrandId();
		m_rLensSetup.nLensName = GetLensNameId();

		m_rLensSetup.nBarlowLensName = GetBarlowLensNameId();
		m_rLensSetup.nFocalReducerName = GetFocalReducerNameId();
		m_rLensSetup.nEyepieceName = GetEyepieceNameId();
		m_rLensSetup.nCameraLensName = GetCameraLensNameId();

	} else if( m_nSourceId == 1 )
	{
		m_rLensSetup.nLensNameOwn = m_pLensNameSelect->GetSelection();
		m_rLensSetup.nBarlowLensNameOwn = m_pBarlowLensNameSelect->GetSelection();
		m_rLensSetup.nFocalReducerNameOwn = m_pFocalReducerNameSelect->GetSelection();
		m_rLensSetup.nEyepieceNameOwn = m_pEyepieceNameSelect->GetSelection();
		m_rLensSetup.nCameraLensNameOwn = m_pCameraLensNameSelect->GetSelection();
	}

	// set to current source
	m_nSourceId = nSourceId;

	// set hardware data
	SetSetupData( );

	/////////
	// if calc type
	if( m_nDlgType == 1 ) SetFocalValues( );

	GetSizer()->Layout();
	Layout();
	Fit( );
	Refresh( false );

	return;
}

////////////////////////////////////////////////////////////////////
void CTLensSelectDlg::OnUseSystemPart( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	bool bValue = pEvent.IsChecked();

	// check by field type
	if( nId == wxID_CHECK_USE_BARLOW_LENS )
		SetBalowLensState( 0, bValue );
	else if( nId == wxID_CHECK_USE_FOCAL_REDUCER )
		SetFocalReducerState( 0, bValue );
	else if( nId == wxID_CHECK_USE_EYEPIECE )
		SetEyepieceState( 0, bValue );
	else if( nId == wxID_CHECK_USE_CAMERA_LENS )
		SetCameraLensState( 0, bValue );

	/////////
	// if calc type
	if( m_nDlgType == 1 ) SetFocalValues( );
}

////////////////////////////////////////////////////////////////////
void CTLensSelectDlg::SetBalowLensState( int nType, bool bValue )
{
	if( nType == 0 )
	{
		if( bValue )
		{
			m_pBarlowLensBrandSelect->Enable();
			m_pBarlowLensNameSelect->Enable();

		} else
		{
			m_pBarlowLensBrandSelect->Disable();
			m_pBarlowLensNameSelect->Disable();
		}

	} else if( nType == 1 )
	{
		if( bValue )
		{
			m_pBarlowLensLabel->Show();
			m_pBarlowLensBrandSelect->Show();
			m_pBarlowLensNameSelect->Show();

		} else
		{
			m_pBarlowLensLabel->Hide();
			m_pBarlowLensBrandSelect->Hide();
			m_pBarlowLensNameSelect->Hide();
		}
	}
}

////////////////////////////////////////////////////////////////////
void CTLensSelectDlg::SetFocalReducerState( int nType, bool bValue )
{
	if( nType == 0 )
	{
		if( bValue )
		{
			m_pFocalReducerBrandSelect->Enable();
			m_pFocalReducerNameSelect->Enable();

		} else
		{
			m_pFocalReducerBrandSelect->Disable();
			m_pFocalReducerNameSelect->Disable();
		}

	} else if( nType == 1 )
	{
		if( bValue )
		{
			m_pFocalReducerLabel->Show();
			m_pFocalReducerBrandSelect->Show();
			m_pFocalReducerNameSelect->Show();

		} else
		{
			m_pFocalReducerLabel->Hide();
			m_pFocalReducerBrandSelect->Hide();
			m_pFocalReducerNameSelect->Hide();
		}
	}
}

////////////////////////////////////////////////////////////////////
void CTLensSelectDlg::SetEyepieceState( int nType, bool bValue )
{
	if( nType == 0 )
	{
		if( bValue )
		{
			m_pEyepieceTypeSelect->Enable();
			m_pEyepieceBrandSelect->Enable();
			m_pEyepieceNameSelect->Enable();

		} else
		{
			m_pEyepieceTypeSelect->Disable();
			m_pEyepieceBrandSelect->Disable();
			m_pEyepieceNameSelect->Disable();
		}

	} else if( nType == 1 )
	{
		if( bValue )
		{
			m_pEyepieceLabel->Show();
			m_pEyepieceTypeSelect->Show();
			m_pEyepieceBrandSelect->Show();
			m_pEyepieceNameSelect->Show();

		} else
		{
			m_pEyepieceLabel->Hide();
			m_pEyepieceTypeSelect->Hide();
			m_pEyepieceBrandSelect->Hide();
			m_pEyepieceNameSelect->Hide();
		}
	}
}

////////////////////////////////////////////////////////////////////
void CTLensSelectDlg::SetCameraLensState( int nType, bool bValue )
{
	if( nType == 0 )
	{
		if( bValue )
		{
			m_pCameraLensTypeSelect->Enable();
			m_pCameraLensBrandSelect->Enable();
			m_pCameraLensNameSelect->Enable();

		} else
		{
			m_pCameraLensTypeSelect->Disable();
			m_pCameraLensBrandSelect->Disable();
			m_pCameraLensNameSelect->Disable();
		}

	} else if( nType == 1 )
	{
		if( bValue )
		{
			m_pCameraLensLabel->Show();
			m_pCameraLensTypeSelect->Show();
			m_pCameraLensBrandSelect->Show();
			m_pCameraLensNameSelect->Show();

		} else
		{
			m_pCameraLensLabel->Hide();
			m_pCameraLensTypeSelect->Hide();
			m_pCameraLensBrandSelect->Hide();
			m_pCameraLensNameSelect->Hide();
		}
	}
}

////////////////////////////////////////////////////////////////////
void CTLensSelectDlg::GetAllFields( )
{
	//m_nSourceId = pSelectTLens->m_nSourceId;
	m_rLensSetup.nLensType = m_pLensTypeSelect->GetSelection();

	// just fetch everything - do selection in calc later
	// Main lens
	m_rLensSetup.nLensBrand = m_pLensBrandSelect->GetSelection();
	// barlow
	m_rLensSetup.bBarlow = m_pBarlowLensLabel->GetValue();
	m_rLensSetup.nBarlowLensBrand = m_pBarlowLensBrandSelect->GetSelection();
	// focal reducer
	m_rLensSetup.bFocalReducer = m_pFocalReducerLabel->GetValue();
	m_rLensSetup.nFocalReducerBrand = m_pFocalReducerBrandSelect->GetSelection();	
	// eyepiece
	m_rLensSetup.bEyepiece = m_pEyepieceLabel->GetValue();
	m_rLensSetup.nEyepieceBrand = m_pEyepieceBrandSelect->GetSelection();		
	// camera lens
	m_rLensSetup.bCameraLens = m_pCameraLensLabel->GetValue();
	m_rLensSetup.nCameraLensBrand = m_pCameraLensBrandSelect->GetSelection();

	if( m_nSourceId == 0 )
	{
		m_rLensSetup.nLensName = GetLensNameId();
		m_rLensSetup.nBarlowLensName = GetBarlowLensNameId();
		m_rLensSetup.nFocalReducerName = GetFocalReducerNameId();
		m_rLensSetup.nEyepieceName = GetEyepieceNameId();
		m_rLensSetup.nCameraLensName = GetCameraLensNameId();
		/////////
		// now set the own stuff if used 
		m_rLensSetup.nLensNameOwn = m_rLensSetup.nLensNameOwn;
		m_rLensSetup.nBarlowLensNameOwn = m_rLensSetup.nBarlowLensNameOwn;
		m_rLensSetup.nFocalReducerNameOwn = m_rLensSetup.nFocalReducerNameOwn;
		m_rLensSetup.nEyepieceNameOwn = m_rLensSetup.nEyepieceNameOwn;
		m_rLensSetup.nCameraLensNameOwn = m_rLensSetup.nCameraLensNameOwn;

	} else if( m_nSourceId == 1 )
	{
		m_rLensSetup.nLensNameOwn = m_pLensNameSelect->GetSelection();
		m_rLensSetup.nBarlowLensNameOwn = m_pBarlowLensNameSelect->GetSelection();
		m_rLensSetup.nFocalReducerNameOwn = m_pFocalReducerNameSelect->GetSelection();
		m_rLensSetup.nEyepieceNameOwn = m_pEyepieceNameSelect->GetSelection();
		m_rLensSetup.nCameraLensNameOwn = m_pCameraLensNameSelect->GetSelection();
		/////////
		// now set the own stuff if used 
		m_rLensSetup.nLensName = m_rLensSetup.nLensName;
		m_rLensSetup.nBarlowLensName = m_rLensSetup.nBarlowLensName;
		m_rLensSetup.nFocalReducerName = m_rLensSetup.nFocalReducerName;
		m_rLensSetup.nEyepieceName = m_rLensSetup.nEyepieceName;
		m_rLensSetup.nCameraLensName = m_rLensSetup.nCameraLensName;
	}
}

////////////////////////////////////////////////////////////////////
double CTLensSelectDlg::CalculateFocalLength( DefTLensSetup& rLensSetup )
{
	double nTelescopeFL=0.0, nEyepieceFL=0.0, nCameraLensFL=0.0,
			nTelescopeMag=0.0, nSystemFL=0.0;

	// get all the data first
	GetAllFields( );

	// if telescope
	if( m_rLensSetup.nLensType == 0 && m_pTelescope->IsTelescopeDef(rLensSetup.nLensBrand,rLensSetup.nLensName) )
	{
		// get telescope focal
		if( m_nSourceId == 0 )
			nTelescopeFL = m_pTelescope->m_vectTelescopeBrandDef[rLensSetup.nLensBrand].vectData[rLensSetup.nLensName].focal_length;
		else if( m_nSourceId == 1 )
			nTelescopeFL = m_pObserver->m_pHardware->m_vectTelescope[rLensSetup.nLensNameOwn].focal_length;

		// get if barlow
		if( rLensSetup.bBarlow )
		{
			// multiply focal length by barlow lens factor
			if( m_nSourceId == 0 )
				nTelescopeFL *=  (double) m_pTelescope->m_vectBarlowLensBrandDef[rLensSetup.nBarlowLensBrand].vectData[rLensSetup.nBarlowLensName].magnification;
			else if( m_nSourceId == 1 )
				nTelescopeFL *= (double) m_pObserver->m_pHardware->m_vectBarlowLens[rLensSetup.nBarlowLensNameOwn].magnification;
		}
		// get if focal reducer
		if( rLensSetup.bFocalReducer )
		{
			// multiply focal length by focal reducer factor
			if( m_nSourceId == 0 )
				nTelescopeFL *=  (double) m_pTelescope->m_vectFocalReducerBrandDef[rLensSetup.nFocalReducerBrand].vectData[rLensSetup.nFocalReducerName].focal_ratio;
			else if( m_nSourceId == 1 )
				nTelescopeFL *= (double) m_pObserver->m_pHardware->m_vectFocalReducer[rLensSetup.nFocalReducerNameOwn].focal_ratio;
		}
		// USE EYEPIECE
		if( rLensSetup.bEyepiece )
		{
			// mag factor = D/FE - 1 - should be 1
			if( m_nSourceId == 0 && m_pTelescope->IsEyepieceDef(rLensSetup.nEyepieceBrand,rLensSetup.nEyepieceName) )
				nEyepieceFL = m_pTelescope->m_vectEyepieceBrandDef[rLensSetup.nEyepieceBrand].vectData[rLensSetup.nEyepieceName].focal_length;
			else if( m_nSourceId == 1 && m_pObserver->m_pHardware->IsEyepiece(rLensSetup.nEyepieceNameOwn) )
				nEyepieceFL = (double) m_pObserver->m_pHardware->m_vectEyepiece[rLensSetup.nEyepieceNameOwn].focal_length;
		}
		// HAS CAMERA LENS
		if( rLensSetup.bCameraLens && m_pCamera->IsCameraLensDef(rLensSetup.nCameraLensBrand,rLensSetup.nCameraLensName) )
		{
			if( m_nSourceId == 0 && m_pCamera->IsCameraLensDef(rLensSetup.nCameraLensBrand,rLensSetup.nCameraLensName) )
				nCameraLensFL = m_pCamera->m_vectCameraLensBrandDef[rLensSetup.nCameraLensBrand].vectData[rLensSetup.nCameraLensName].focal_length;
			else if( m_nSourceId == 1 && m_pObserver->m_pHardware->IsCameraLens(rLensSetup.nCameraLensNameOwn) )
				nCameraLensFL = (double) m_pObserver->m_pHardware->m_vectCameraLens[rLensSetup.nCameraLensNameOwn].focal_length;
		}

		//////////////////
		// SPECIAL TO ADD :: check if camera has lens
		//if( camera has lens )
		//{
		//	nCameraWithLensFR ? = ...
		//}

		//////////////////////////
		// calculate focal length
		if( rLensSetup.bEyepiece && ( rLensSetup.bCameraLens /*|| camera has lens*/ ) )
		{
			// if camera with lens and lens - adjust by camera lens adaptor factor
			//if( rLensSetup.bCameraLens && camera_has_lens )
			//{
			//	nCameraLensFL *= nCameraWithLensFR ??
			//}

			// calculate system focal length
			nTelescopeMag = (nTelescopeFL / nEyepieceFL);
			nSystemFL = (nTelescopeMag * nCameraLensFL);

		// else if just eyepiece in prime focus ?
		} else if( rLensSetup.bEyepiece )
		{
			// to do here - specifiy that distace in the TLens dialog setup
			double nDistEyepieceToSensor = nEyepieceFL;
			double nMagFact = (nDistEyepieceToSensor/nEyepieceFL-1.0)*nTelescopeFL;
			if( nMagFact > 0 )
				nSystemFL = nMagFact*nTelescopeFL;
			else
				nSystemFL = nTelescopeFL;

		// else if just prime focus
		} else
		{
			nSystemFL = nTelescopeFL;
		}

	//////////////////
	// camera lens
	} else if( m_rLensSetup.nLensType == 1 && m_pCamera->IsCameraLensDef(rLensSetup.nLensBrand,rLensSetup.nLensName) )
	{
		// system focal length in this case is the camera lens focal length
		if( m_nSourceId == 0 && m_pCamera->IsCameraLensDef(rLensSetup.nLensBrand,rLensSetup.nLensName) )
			nTelescopeFL = m_pCamera->m_vectCameraLensBrandDef[rLensSetup.nLensBrand].vectData[rLensSetup.nLensName].focal_length;
		else if( m_nSourceId == 1 && m_pObserver->m_pHardware->IsCameraLens(rLensSetup.nLensNameOwn) )
			nTelescopeFL = (double) m_pObserver->m_pHardware->m_vectCameraLens[rLensSetup.nLensNameOwn].focal_length;

		//////////////////
		// SPECIAL TO ADD :: check if camera has lens
		//if( camera has lens )
		//{
		//	nCameraWithLensFR ? = ...
		//}

		//////////////////////////
		// calculate focal length
		//if( camera has lens )
		//{
		//} else 
		//{
			nSystemFL = nTelescopeFL;
		//}
	}


/*	// calculate focal (afocal)
	// also see: http://www.saao.ac.za/~wpk/exposure.html
   lnTelescopeMag = (lnTelescopeFL / lnEyepieceFL);
   lnSystemFL = (lnTelescopeMag * lnCameraFL);
   lnFRatio = (lnSystemFL / lnAperture);
   lnExposure = (lnFRatio * lnFRatio) / (lnK * lnISO);

   //////////////////////////////////
	// prime focus with a barlow lens - http://www.astrosurf.com/legault/focal.html
	// Amplification = D/Fb + 1 - where D = distance between lens and focal plan, 
	// Fb = barlow lens focal length - SystemFocal = Amplification*Telescope_focal_length;
	// - I can use the mag factor written on the barlow like 2x or 3x 
*/

	return( nSystemFL );
}

////////////////////////////////////////////////////////////////////
void CTLensSelectDlg::SetFocalValues( )
{
	wxString strTmp=wxT("");

	// calculate system focal length
	double nFocal = CalculateFocalLength( m_rLensSetup );
	// set focal length
	strTmp.Printf( wxT("%.2lf"), nFocal );
	m_pFocalVal->ChangeValue( strTmp );

	// get telescope focal
	double nAperture=0.0;
	if( m_nSourceId == 0 && m_pTelescope->IsTelescopeDef(m_rLensSetup.nLensBrand,m_rLensSetup.nLensName) )
		nAperture = m_pTelescope->m_vectTelescopeBrandDef[m_rLensSetup.nLensBrand].vectData[m_rLensSetup.nLensName].aperture;
	else if( m_nSourceId == 1 && m_pObserver->m_pHardware->IsTelescope(m_rLensSetup.nLensNameOwn) )
		nAperture = m_pObserver->m_pHardware->m_vectTelescope[m_rLensSetup.nLensNameOwn].aperture;

	// calculate focal ratio - speed
	if( nAperture > 0.0 )
	{
		double nFocalRatio = nFocal/nAperture;
		strTmp.Printf( wxT("%.2lf"), nFocalRatio );
		m_pFocalRatioVal->ChangeValue( strTmp );
	}
}

////////////////////////////////////////////////////////////////////
int CTLensSelectDlg::GetLensBrandId( )
{
	int lb_id = m_pLensBrandSelect->GetSelection();
	if( m_vectLensBrandId.size() && lb_id >= 0 && lb_id < m_vectLensBrandId.size() ) 
		return( m_vectLensBrandId.at(lb_id) ); 
	else 
		return(0);
}

////////////////////////////////////////////////////////////////////
int CTLensSelectDlg::GetLensNameId( )
{
	int ln_id = m_pLensNameSelect->GetSelection();
	if( m_vectLensNameId.size() && ln_id >= 0 && ln_id < m_vectLensNameId.size() ) 		
		return( m_vectLensNameId.at(ln_id) );
	else 
		return(0);
}

////////////////////////////////////////////////////////////////////
int CTLensSelectDlg::GetEyepieceBrandId()
{ 
	int eb_id = m_pEyepieceBrandSelect->GetSelection();
	if( m_vectEyepieceBrandId.size() && eb_id >=0 && eb_id < m_vectEyepieceBrandId.size() ) 
		return( m_vectEyepieceBrandId.at(eb_id) ); 
	else 
		return(0); 
}

////////////////////////////////////////////////////////////////////
int CTLensSelectDlg::GetEyepieceNameId()
{ 
	int en_id = m_pEyepieceNameSelect->GetSelection();
	if( m_vectEyepieceNameId.size() && en_id >= 0 && en_id < m_vectEyepieceNameId.size() ) 
		return( m_vectEyepieceNameId.at(en_id) ); 
	else 
		return(0); 
}

////////////////////////////////////////////////////////////////////
int CTLensSelectDlg::GetCameraLensBrandId()
{ 
	int id = m_pCameraLensBrandSelect->GetSelection();
	if( m_vectCameraLensBrandId.size() && id >= 0 && id < m_vectCameraLensBrandId.size() ) 
		return( m_vectCameraLensBrandId.at(id) ); 
	else 
		return(0); 
}

////////////////////////////////////////////////////////////////////
int CTLensSelectDlg::GetCameraLensNameId()
{ 
	int id = m_pCameraLensNameSelect->GetSelection();
	if( m_vectCameraLensNameId.size() && id >= 0 && id < m_vectCameraLensNameId.size() ) 
		return( m_vectCameraLensNameId.at(id) ); 
	else 
		return(0); 
}
