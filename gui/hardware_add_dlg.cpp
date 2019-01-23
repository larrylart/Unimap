
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/wxprec.h"

// local include
#include "../observer/observer_hardware.h"
#include "../camera/camera.h"
#include "../telescope/telescope.h"

// main header
#include "hardware_add_dlg.h"

// CCameraAddDlg EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CHardwareAddDlg, wxDialog )
	EVT_CHOICE( wxID_HARDWARE_SELECT_TYPE, CHardwareAddDlg::OnSelectHardware )
	EVT_CHOICE( wxID_HARDWARE_SELECT_BRAND, CHardwareAddDlg::OnSelectHardware )
	EVT_CHOICE( wxID_HARDWARE_SELECT_CAMERA, CHardwareAddDlg::OnSelectHardware )
	EVT_BUTTON( wxID_HARDWARE_OTHER_BRAND, CHardwareAddDlg::OnActionType )
	EVT_BUTTON( wxID_HARDWARE_OTHER_NAME, CHardwareAddDlg::OnActionType )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CHardwareAddDlg
// Purpose:	Initialize object
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CHardwareAddDlg::CHardwareAddDlg( wxWindow *parent, wxString title ) 
						: wxDialog( parent, -1,
                          title, wxDefaultPosition,
                          wxDefaultSize, //wxSize( 350, 310 ),
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	m_nType = 0;
	m_nBrandType = 0;
	m_nNameType = 0;
	// clear vector ids
	m_vectBrandId.clear();
	m_vectNameId.clear();

	// main dialog sizer
	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 2, 1, 10, 10 );

	//////////////////////////////////
	// Mouse panel sizer
	wxFlexGridSizer *sizerEntryPanel = new wxFlexGridSizer( 1, 2, 5, 10 );
	
	m_pTypeLabel = new wxStaticText( this, -1, wxT("Type:") );
	m_pTypeSelect = new wxChoice( this, wxID_HARDWARE_SELECT_TYPE, 
										wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pBrandSelect = new wxChoice( this, wxID_HARDWARE_SELECT_BRAND, 
										wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pNameSelect= new wxChoice( this, wxID_HARDWARE_SELECT_CAMERA, 
										wxDefaultPosition, wxSize(-1,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// text entries
	m_pBrandEntry = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(150,-1), wxTE_NOHIDESEL );
	m_pNameEntry = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(150,-1), wxTE_NOHIDESEL );
	m_pBrandEntry->Hide();
	m_pNameEntry->Hide();
	// buttons
	m_pBrandButton = new wxButton( this, wxID_HARDWARE_OTHER_BRAND, wxT("Other"), wxPoint(-1, -1), wxSize(40,-1) );
	m_pNameButton = new wxButton( this, wxID_HARDWARE_OTHER_NAME, wxT("Other"), wxPoint(-1, -1), wxSize(40,-1) );

	// :: type
	sizerEntryPanel->Add( m_pTypeLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pTypeSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: brand
	sizerEntryPanel->Add(new wxStaticText( this, -1, wxT("Brand:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer *sizerEntryBrand = new wxFlexGridSizer( 1, 3, 5, 10 );
	sizerEntryBrand->Add( m_pBrandSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerEntryBrand->Add( m_pBrandEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerEntryBrand->Add( m_pBrandButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( sizerEntryBrand, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: name
	sizerEntryPanel->Add(new wxStaticText( this, -1, wxT("Name:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer *sizerEntryName = new wxFlexGridSizer( 1, 3, 5, 10 );
	sizerEntryName->Add( m_pNameSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerEntryName->Add( m_pNameEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerEntryName->Add( m_pNameButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( sizerEntryName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	topsizer->Add( sizerEntryPanel, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW |wxEXPAND|wxTOP|wxLEFT|wxRIGHT, 10 );
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
// Class:	CHardwareAddDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CHardwareAddDlg::~CHardwareAddDlg( ) 
{
	delete( m_pTypeLabel );
	delete( m_pTypeSelect );
	delete( m_pBrandSelect );
	delete( m_pNameSelect );
	delete( m_pBrandEntry );
	delete( m_pNameEntry );
	delete( m_pBrandButton );
	delete( m_pNameButton );
}

////////////////////////////////////////////////////////////////////
void CHardwareAddDlg::SetData( CCamera* pCamera, int nHType )
{
	// set main object type(either camera=0 or telescope=1)
	m_nObjectType = 0;
	// set hardware kind ie camera=digital/video/etc
	m_nHType = nHType;
	// get reference to object telescope
	m_pCamera = pCamera;

	// other defaults
	m_nType = 0;
	m_nBrand = 0;
	m_nName = 0;

	// load camera hardware
	m_pCamera->LoadHardwareDef( nHType, -1, 0 );
	m_pCamera->SetGuiSelect( nHType, m_pTypeSelect, m_pBrandSelect, m_pNameSelect, m_nType, m_nBrand, m_nName, m_vectBrandId, m_vectNameId );

	GetSizer()->Layout();
	Layout();
	Fit( );
	Refresh( false );
}

//////////////////////////////////////////////////////////////////// - check here reverse main with type
void CHardwareAddDlg::SetData( CTelescope* pTelescope, int nHType )
{
	// set main object type(either camera=0 or telescope=1)
	m_nObjectType = 1;
	// set main hardware type
//	m_nMainType = nMainType
	// set hardware kind ie camera=digital/video/etc
	m_nHType = nHType;
	// get reference to object telescope
	m_pTelescope = pTelescope;

	// other defaults
	m_nType = 0;
	m_nBrand = 0;
	m_nName = 0;

	// load telescope hardware
	m_pTelescope->LoadHardwareDef( nHType, -1, 0 );
	m_pTelescope->SetGuiSelect( nHType, m_pTypeSelect, m_pBrandSelect, m_pNameSelect, m_nType, m_nBrand, m_nName, m_vectBrandId, m_vectNameId );

	// for the case of barllow lens and focal reducer - hide type
	if( nHType == TELESCOPE_HARDWARE_BARLOW_LENS || 
		nHType == TELESCOPE_HARDWARE_FOCAL_REDUCER )
	{
		m_pTypeLabel->Hide();
		m_pTypeSelect->Hide();
	}

	GetSizer()->Layout();
	Layout();
	Fit( );
	Refresh( false );
}

////////////////////////////////////////////////////////////////////
// Method:	OnSelectHardware
////////////////////////////////////////////////////////////////////
void CHardwareAddDlg::OnSelectHardware( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	int nBrandId = GetBrandId();

	if( nId == wxID_HARDWARE_SELECT_TYPE )
	{
		// get my hardware type
		m_nType = m_pTypeSelect->GetSelection();
		m_nBrand=0;
		m_nName=0;

		// if object type camera=0 or telescope=1
		if( m_nObjectType == 0 )
			m_pCamera->SetGuiSelect( m_nHType, m_pTypeSelect, m_pBrandSelect, m_pNameSelect, m_nType, m_nBrand, m_nName, m_vectBrandId, m_vectNameId );
		else if( m_nObjectType == 1 )
			m_pTelescope->SetGuiSelect( m_nHType, m_pTypeSelect, m_pBrandSelect, m_pNameSelect, m_nType, m_nBrand, m_nName, m_vectBrandId, m_vectNameId );

	} else if( nId == wxID_HARDWARE_SELECT_BRAND )
	{
		m_nBrand = nBrandId;
		m_nName=0;

		// if object type camera=0 or telescope=1
		if( m_nObjectType == 0 )
		{
			m_pCamera->LoadHardwareIniByBrandId( m_nHType, nBrandId );
			//m_pCamera->SetGuiSelect( m_nHType, m_pTypeSelect, m_pBrandSelect, m_pNameSelect, m_nType, m_nBrand, m_nName, m_vectBrandId, m_vectNameId );
			m_pCamera->SetGuiNameSelect( m_nHType, m_pNameSelect, m_nType, m_nBrand, m_nName, m_vectNameId );

		} else if( m_nObjectType == 1 )
		{
			m_pTelescope->LoadHardwareIniByBrandId( m_nHType, nBrandId );
			//m_pTelescope->SetGuiSelect( m_nHType, m_pTypeSelect, m_pBrandSelect, m_pNameSelect, m_nType, m_nBrand, m_nName, m_vectBrandId, m_vectNameId );
			m_pTelescope->SetGuiNameSelect( m_nHType, m_pNameSelect, m_nType, m_nBrand, m_nName, m_vectNameId );
		}

/*
		if( m_nType == HARDWARE_TYPE_CAMERA )
		{
			m_pCamera->LoadHardwareIniByBrandId( CAMERA_HARDWARE_MAIN_BODY, nBrandId );
			m_pCamera->SetGuiNameSelect( CAMERA_HARDWARE_MAIN_BODY, m_pNameSelect, 0, nBrandId, 0, m_vectNameId );

		} else if( m_nType == HARDWARE_TYPE_CAMERA_LENS )
		{
			m_pCamera->LoadHardwareIniByBrandId( CAMERA_HARDWARE_LENS, nBrandId );
			m_pCamera->SetGuiNameSelect( CAMERA_HARDWARE_LENS, m_pNameSelect, 0, nBrandId, 0, m_vectNameId );

		} else if( m_nType == HARDWARE_TYPE_TELESCOPE )
		{
			m_pTelescope->LoadHardwareIniByBrandId( TELESCOPE_HARDWARE_MAIN_SCOPE, nBrandId );
			m_pTelescope->SetGuiNameSelect( TELESCOPE_HARDWARE_MAIN_SCOPE, m_pNameSelect, nBrandId, 0 );

		} else if( m_nType == HARDWARE_TYPE_BARLOW_LENS )
		{
			m_pTelescope->LoadHardwareIniByBrandId( TELESCOPE_HARDWARE_BARLOW_LENS, nBrandId );
			m_pTelescope->SetGuiNameSelect( TELESCOPE_HARDWARE_BARLOW_LENS, m_pNameSelect, nBrandId, 0 );

		} else if( m_nType == HARDWARE_TYPE_FOCAL_REDUCER )
		{
			m_pTelescope->LoadHardwareIniByBrandId( TELESCOPE_HARDWARE_FOCAL_REDUCER, nBrandId );
			m_pTelescope->SetGuiNameSelect( TELESCOPE_HARDWARE_FOCAL_REDUCER, m_pNameSelect, nBrandId, 0 );

		} else if( m_nType == HARDWARE_TYPE_EYEPIECE )
		{
			m_pTelescope->LoadHardwareIniByBrandId( TELESCOPE_HARDWARE_EYEPIECE, nBrandId );
			m_pTelescope->SetGuiNameSelect( TELESCOPE_HARDWARE_EYEPIECE, m_pNameSelect, nBrandId, 0 );

		} else if( m_nType == HARDWARE_TYPE_LMOUNT )
		{
			m_pTelescope->LoadHardwareIniByBrandId( TELESCOPE_HARDWARE_LMOUNT, nBrandId );
			m_pTelescope->SetGuiNameSelect( TELESCOPE_HARDWARE_LMOUNT, m_pNameSelect, nBrandId, 0 );

		} else if( m_nType == HARDWARE_TYPE_FOCUSER )
		{
			m_pTelescope->LoadHardwareIniByBrandId( TELESCOPE_HARDWARE_FOCUSER, nBrandId );
			m_pTelescope->SetGuiNameSelect( TELESCOPE_HARDWARE_FOCUSER, m_pNameSelect, nBrandId, 0 );

		} else if( m_nType == HARDWARE_TYPE_TFILTER )
		{
			m_pTelescope->LoadHardwareIniByBrandId( TELESCOPE_HARDWARE_TFILTER, nBrandId );
			m_pTelescope->SetGuiNameSelect( TELESCOPE_HARDWARE_TFILTER, m_pNameSelect, nBrandId, 0 );

		}
*/
	}

	GetSizer()->Layout();
	Layout();
	Fit( );
	Refresh( false );
}

////////////////////////////////////////////////////////////////////
// Method:	OnActionType
////////////////////////////////////////////////////////////////////
void CHardwareAddDlg::OnActionType( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();

	if( nId == wxID_HARDWARE_OTHER_BRAND )
	{
		// switch to entry type = 1
		if( m_nBrandType == 0 )
		{
			m_pBrandSelect->Hide();
			m_pBrandEntry->Show();
			m_pBrandEntry->SetFocus();
			m_pBrandButton->SetLabel( wxT("Select") );
			m_nBrandType = 1;

		} else if( m_nBrandType == 1 )
		{
			m_pBrandSelect->Show();
			m_pBrandEntry->Hide();
			m_pBrandSelect->SetFocus();
			m_pBrandButton->SetLabel( wxT("Other") );
			m_nBrandType = 0;
		}

	} else if( nId == wxID_HARDWARE_OTHER_NAME )
	{
		// switch to entry type = 1
		if( m_nNameType == 0 )
		{
			m_pNameSelect->Hide();
			m_pNameEntry->Show();
			m_pNameEntry->SetFocus();
			m_pNameButton->SetLabel( wxT("Select") );
			m_nNameType = 1;

		} else if( m_nNameType == 1 )
		{
			m_pNameSelect->Show();
			m_pNameEntry->Hide();
			m_pNameSelect->SetFocus();
			m_pNameButton->SetLabel( wxT("Other") );
			m_nNameType = 0;
		}
	}

	GetSizer()->Layout();
	Layout();
	Fit( );
	Refresh( false );

	return;
}
