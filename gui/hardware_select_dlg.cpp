
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/wxprec.h"

// local include
#include "../observer/observer.h"
#include "../camera/camera.h"
#include "../telescope/telescope.h"

// main header
#include "hardware_select_dlg.h"

// CCameraAddDlg EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CHardwareSelectDlg, wxDialog )
	EVT_CHOICE( wxID_HARDWARE_SELECT_SOURCE, CHardwareSelectDlg::OnSourceType )
	EVT_CHOICE( wxID_HARDWARE_SELECT_TYPE, CHardwareSelectDlg::OnSelectHardware )
	EVT_CHOICE( wxID_HARDWARE_SELECT_BRAND, CHardwareSelectDlg::OnSelectHardware )
	EVT_CHOICE( wxID_HARDWARE_SELECT_NAME, CHardwareSelectDlg::OnSelectHardware )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CHardwareSelectDlg
// Purpose:	Initialize object
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CHardwareSelectDlg::CHardwareSelectDlg( wxWindow *parent, wxString title, wxString label ) 
						: wxDialog( parent, -1,
                          title, wxDefaultPosition,
                          wxDefaultSize, //wxSize( 350, 310 ),
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	m_nHardwareType = 0;
	m_nSourceId = 0;
	m_nType = 0;
	m_nBrand = 0;
	m_nName = 0;
	m_nNameOwn = 0;

	// define type of sources
	wxString vectSourceType[3];
	vectSourceType[0] = wxT("All Defined");
	vectSourceType[1] = wxT("What I Own");

	// main dialog sizer
	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 3, 1, 10, 10 );

	// select source sizer
	wxFlexGridSizer *sizerSource = new wxFlexGridSizer( 1, 2, 0, 5 );
	topsizer->Add( sizerSource, 1, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_BOTTOM|wxTOP, 10 );
	// hardware static box/sizer
	wxStaticBox* pHardwareBox = new wxStaticBox( this, -1, label, wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerHardwareBox = new wxStaticBoxSizer( pHardwareBox, wxHORIZONTAL );
	// hardware data sizer
	wxFlexGridSizer *sizerHardwareData = new wxFlexGridSizer( 3, 2, 3, 10 );
	sizerHardwareBox->Add( sizerHardwareData, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 10 );
	topsizer->Add( sizerHardwareBox, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 10 );
	
	m_pSourceSelect = new wxChoice( this, wxID_HARDWARE_SELECT_SOURCE, 
										wxDefaultPosition, wxSize(-1,-1), 2, 
										vectSourceType, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSourceSelect->SetSelection(0);
	m_pTypeSelect = new wxChoice( this, wxID_HARDWARE_SELECT_TYPE, 
										wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pBrandSelect = new wxChoice( this, wxID_HARDWARE_SELECT_BRAND, 
										wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pNameSelect= new wxChoice( this, wxID_HARDWARE_SELECT_NAME, 
										wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pTypeBrandLabel = new wxStaticText( this, -1, wxT("Type/Brand:") );

	// :: source
	sizerSource->Add( new wxStaticText( this, -1, wxT("Filter Select From:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerSource->Add( m_pSourceSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: type/brand
	sizerHardwareData->Add( m_pTypeBrandLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer *sizerHardwareTypeBrand = new wxFlexGridSizer( 1, 2, 0, 5 );
	sizerHardwareTypeBrand->Add( m_pTypeSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerHardwareTypeBrand->Add( m_pBrandSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerHardwareData->Add( sizerHardwareTypeBrand, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: name
	sizerHardwareData->Add(new wxStaticText( this, -1, wxT("Name:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerHardwareData->Add( m_pNameSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

//	topsizer->Add( sizerEntryPanel, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW |wxEXPAND|wxTOP|wxLEFT|wxRIGHT, 10 );
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL ), 1, wxALIGN_CENTER| wxCENTRE | wxALL, 5 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
	//topsizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CHardwareSelectDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CHardwareSelectDlg::~CHardwareSelectDlg( ) 
{
	delete( m_pSourceSelect );
	delete( m_pTypeBrandLabel );
	delete( m_pTypeSelect );
	delete( m_pBrandSelect );
	delete( m_pNameSelect );
}

////////////////////////////////////////////////////////////////////
void CHardwareSelectDlg::SetData( CObserver* pObserver, CCamera* pCamera, 
									int nHType, int nSourceId, int nType, int nBrand, int nName )
{
	m_nHardwareType = HARDWARE_TYPE_CAMERA;
	m_pObserver = pObserver;
	m_pCamera = pCamera;
	// ids
	m_nSourceId = nSourceId;
	m_nHType = nHType;
	m_nType = nType;
	m_nBrand = nBrand;

	if( m_nSourceId == 0 )
		m_nName = nName;
	else if( m_nSourceId == 1 )
		m_nNameOwn = nName;

	m_pSourceSelect->SetSelection( m_nSourceId );
	SetHardwareData( nHType );

	GetSizer()->Layout();
	Layout();
	Fit( );
	Refresh( false );
}

////////////////////////////////////////////////////////////////////
void CHardwareSelectDlg::SetHardwareData( int nHType )
{
	int i=0;
	// if to select from all defines
	if( m_nSourceId == 0 )
	{
		m_pTypeBrandLabel->Show();
		m_pTypeSelect->Show();
		m_pBrandSelect->Show();

		if( m_nHardwareType == HARDWARE_TYPE_CAMERA )
		{
			m_pCamera->LoadHardwareDef( nHType, -1, m_nBrand );
			m_pCamera->SetGuiSelect( nHType, m_pTypeSelect, m_pBrandSelect, m_pNameSelect, 
										m_nType, m_nBrand, m_nName, m_vectBrandId, m_vectNameId );
			//m_pBrandSelect->SetSelection( GetBrandSelectFromId( m_nBrand ) );

		} else if( m_nHardwareType == HARDWARE_TYPE_TELESCOPE )
		{
			m_pTelescope->LoadHardwareDef( TELESCOPE_HARDWARE_MAIN_SCOPE, -1, 0 );
			m_pTelescope->SetGuiSelect( TELESCOPE_HARDWARE_MAIN_SCOPE, m_pTypeSelect, m_pBrandSelect, 
											m_pNameSelect, 0, 0, 0, m_vectBrandId, m_vectNameId );
		}

	// or if to select from own hardware
	} else if( m_nSourceId == 1 )
	{
		m_pTypeBrandLabel->Hide();
		m_pTypeSelect->Hide();
		m_pBrandSelect->Hide();

		if( m_nHardwareType == HARDWARE_TYPE_CAMERA )
		{
			// set cameras
			m_pNameSelect->Clear();
			if( m_pObserver->m_pHardware->m_vectCamera.size() > 0 )
			{
				m_pNameSelect->Enable();
				for( i=0; i<m_pObserver->m_pHardware->m_vectCamera.size(); i++ ) 
					m_pNameSelect->Append( m_pObserver->m_pHardware->GetCameraFullName( i ) );
				m_pNameSelect->SetSelection( m_nNameOwn );
			} else
				m_pNameSelect->Disable();

		} 
	}
}

////////////////////////////////////////////////////////////////////
void CHardwareSelectDlg::SetData( CObserver* pObserver, CTelescope* pTelescope, int nHType )
{
	m_nHardwareType = HARDWARE_TYPE_TELESCOPE;
	m_pObserver = pObserver;
	m_pTelescope = pTelescope;

	SetHardwareData( 0 );

	GetSizer()->Layout();
	Layout();
	Fit( );
	Refresh( false );
}

////////////////////////////////////////////////////////////////////
// Method:	OnSelectHardware
////////////////////////////////////////////////////////////////////
void CHardwareSelectDlg::OnSelectHardware( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
//	int nBrandId = GetBrandId();

	if( nId == wxID_HARDWARE_SELECT_TYPE )
	{
		// get my hardware type
		m_nType = m_pTypeSelect->GetSelection();
		m_nBrand=GetBrandId();
		m_nName=0;

		// if object type camera=0 or telescope=1
		if( m_nSourceId == 0 )
			m_pCamera->SetGuiSelect( m_nHType, NULL, m_pBrandSelect, m_pNameSelect, m_nType, m_nBrand, m_nName, m_vectBrandId, m_vectNameId );
		else if( m_nSourceId == 1 )
			m_pTelescope->SetGuiSelect( m_nHType, NULL, m_pBrandSelect, m_pNameSelect, m_nType, m_nBrand, m_nName, m_vectBrandId, m_vectNameId );

	} else if( nId == wxID_HARDWARE_SELECT_BRAND )
	{
		m_nBrand = GetBrandId();
		m_nName=0;

		// if object type camera=0 or telescope=1
		if( m_nSourceId == 0 )
		{
			m_pCamera->LoadHardwareIniByBrandId( m_nHType, m_nBrand );
			//m_pCamera->SetGuiSelect( m_nHType, m_pTypeSelect, m_pBrandSelect, m_pNameSelect, m_nType, m_nBrand, m_nName, m_vectBrandId, m_vectNameId );
			m_pCamera->SetGuiNameSelect( m_nHType, m_pNameSelect, m_nType, m_nBrand, m_nName, m_vectNameId );

		} else if( m_nSourceId == 1 )
		{
			m_pTelescope->LoadHardwareIniByBrandId( m_nHType, m_nBrand );
			//m_pTelescope->SetGuiSelect( m_nHType, m_pTypeSelect, m_pBrandSelect, m_pNameSelect, m_nType, m_nBrand, m_nName, m_vectBrandId, m_vectNameId );
			m_pTelescope->SetGuiNameSelect( m_nHType, m_pNameSelect, m_nType, m_nBrand, m_nName, m_vectNameId );
		}

/*		if( m_nHardwareType == HARDWARE_TYPE_CAMERA )
		{
			m_pCamera->LoadHardwareIniByBrandId( CAMERA_HARDWARE_MAIN_BODY, nBrandId );
			m_pCamera->SetGuiNameSelect( CAMERA_HARDWARE_MAIN_BODY, m_pNameSelect, 0, nBrandId, 0, m_vectNameId );

		} else if( m_nHardwareType == HARDWARE_TYPE_TELESCOPE )
		{
			m_pTelescope->LoadHardwareIniByBrandId( TELESCOPE_HARDWARE_MAIN_SCOPE, nBrandId );
			m_pTelescope->SetGuiNameSelect( TELESCOPE_HARDWARE_MAIN_SCOPE, m_pNameSelect, nBrandId, 0 );

		} else if( m_nHardwareType == HARDWARE_TYPE_CAMERA_LENS )
		{
			m_pCamera->LoadHardwareIniByBrandId( CAMERA_HARDWARE_LENS, nBrandId );
			m_pCamera->SetGuiNameSelect( CAMERA_HARDWARE_LENS, m_pNameSelect, 0, nBrandId, 0, m_vectNameId );
		}
*/
	}

	GetSizer()->Layout();
	Layout();
	Fit( );
	Refresh( false );
}

////////////////////////////////////////////////////////////////////
// Method:	OnSourceType
////////////////////////////////////////////////////////////////////
void CHardwareSelectDlg::OnSourceType( wxCommandEvent& pEvent )
{
	int nSourceId = m_pSourceSelect->GetSelection();
	// check if the same as prev source
	if( m_nSourceId == nSourceId ) return;

	// keep last set
	if( m_nSourceId == 0 )
	{
		m_nType = m_pTypeSelect->GetSelection();
		m_nBrand = m_pBrandSelect->GetSelection();
		m_nName = m_pNameSelect->GetSelection();
		// fix brand no selection - todo: in future trace why and set properly
		if( m_nBrand < 0 ) m_nBrand = 0;

	} else if( m_nSourceId == 1 )
		m_nNameOwn = m_pNameSelect->GetSelection();

	// set to current source
	m_nSourceId = nSourceId;

	// set hardware data
	SetHardwareData( 0 );

	GetSizer()->Layout();
	Layout();
	Fit( );
	Refresh( false );

	return;
}
