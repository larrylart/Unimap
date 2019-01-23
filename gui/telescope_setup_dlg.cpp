
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// local headers
#include "../observer/observer.h"
#include "../observer/observer_hardware.h"
#include "../telescope/telescope.h"
#include "../camera/camera.h"
#include "observer_cfg_dlg.h"
#include "../unimap_worker.h"

// main header
#include "telescope_setup_dlg.h"

//		***** CLASS CTelescopeSetupDlg *****
////////////////////////////////////////////////////////////////////
// CTelescopeSetupDlg EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CTelescopeSetupDlg, wxDialog)
	EVT_BUTTON( wxID_TELESCOPE_SETUP_SITE_ADD, CTelescopeSetupDlg::OnSiteAdd )
END_EVENT_TABLE()

// Constructor/Destructor
////////////////////////////////////////////////////////////////////
CTelescopeSetupDlg::CTelescopeSetupDlg( wxWindow *parent, CTelescope* pTelescope, const wxString& title ) : wxDialog(parent, -1,
                          title,
                          wxDefaultPosition,
                          wxDefaultSize,
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL)
{
	m_pTelescope = pTelescope;
	// mount types
	wxString vectMountType[3];
	vectMountType[0] = wxT("Altazimuth");
	vectMountType[1] = wxT("Equatorial");

	// create main sizer
	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	/////////////////////////////
	// data sizer
	wxFlexGridSizer* sizerData = new wxFlexGridSizer( 5, 1, 5, 5 );
//	headSizer->AddGrowableCol( 1 );
	// data sizer A
	wxFlexGridSizer* sizerDataA = new wxFlexGridSizer( 5, 2, 5, 5 );
	sizerData->Add( sizerDataA, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );
	// data sizer B
	wxFlexGridSizer* sizerDataB = new wxFlexGridSizer( 5, 1, 5, 5 );
	sizerData->Add( sizerDataB, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );

	////////////////
	// GUI create
	// :: mount
	m_pMountTypeSelect = new wxChoice( this, -1,
										wxDefaultPosition, wxSize(100,-1), 2, vectMountType,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pMountTypeSelect->SetSelection( 0 );
	// :: site
	m_pSiteNameSelect = new wxChoice( this, -1,
										wxDefaultPosition, wxSize(200,-1), 0, NULL,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSiteNameSelect->SetSelection( 0 );
	m_pSiteSetButton = new wxButton( this, wxID_TELESCOPE_SETUP_SITE_ADD, wxT("Add"), wxPoint(-1,-1), wxSize(30,-1) );
	// :: date
	m_pDate = new wxDatePickerCtrl( this, -1, wxDefaultDateTime,
									wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN );
	// :: time
	m_pTime = new wxTextCtrl( this, -1, wxT(""),
									wxDefaultPosition, wxSize(60,-1) );
	// :: orientation left-right up-down
	m_pReverseLeftRight = new wxCheckBox( this, -1, wxT("Reverse Left-Right (East-West)") );
	m_pReverseUpDown = new wxCheckBox( this, -1, wxT("Reverse Up-Down (North-South)") );
	// :: chowrd wrap
	m_pCordWrap = new wxCheckBox( this, -1, wxT("Do Cord Un-Wrap Movement") );
	// :: user high precision 
	m_pHighPrecision = new wxCheckBox( this, -1, wxT("Use High Precision Pointing") );
	// :: select what camera to user for guiding
	m_pGuideCamSelect = new wxChoice( this, -1, wxDefaultPosition, wxSize(200,-1), 0, NULL,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pGuideCamSelect->SetSelection( 0 );

	///////////////////////////////////
	// data sizer layout
	// :: mount
	sizerDataA->Add( new wxStaticText( this, -1, wxT("Mount:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerDataA->Add( m_pMountTypeSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: site
	sizerDataA->Add( new wxStaticText( this, -1, wxT("Site:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	wxFlexGridSizer* sizerDataSite = new wxFlexGridSizer( 1,2,0,0 );
	sizerDataSite->Add( m_pSiteNameSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerDataSite->Add( m_pSiteSetButton, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5  );
	sizerDataA->Add( sizerDataSite, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: date-time
	sizerDataA->Add( new wxStaticText( this, -1, wxT("Date/Time:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	wxFlexGridSizer* sizerDataDateTime = new wxFlexGridSizer( 1,3,0,0 );
	sizerDataDateTime->Add( m_pDate, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerDataDateTime->Add( m_pTime, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerDataA->Add( sizerDataDateTime, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: guide camera
	sizerDataA->Add( new wxStaticText( this, -1, wxT("Guide:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerDataA->Add( m_pGuideCamSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// flags
	sizerDataB->Add( m_pReverseLeftRight, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerDataB->Add( m_pReverseUpDown, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerDataB->Add( m_pCordWrap, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerDataB->Add( m_pHighPrecision, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// add to main sizer
	topsizer->Add( sizerData, 1, wxGROW | wxALL, 10 );
	// add buttons
	topsizer->Add( CreateButtonSizer(wxOK|wxCANCEL), 0, wxCENTRE|wxALL, 10 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	topsizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
CTelescopeSetupDlg::~CTelescopeSetupDlg( )
{
	delete( m_pMountTypeSelect );
	delete( m_pSiteNameSelect );
	delete( m_pSiteSetButton );
	delete( m_pDate );
	delete( m_pTime );
	delete( m_pReverseLeftRight );
	delete( m_pReverseUpDown );
	delete( m_pCordWrap );
	delete( m_pHighPrecision );
	delete( m_pGuideCamSelect );
}


////////////////////////////////////////////////////////////////////
void CTelescopeSetupDlg::SetConfig( CObserver* pObserver, CCamera* pCamera, CUnimapWorker* pUnimapWorker )
{
	m_pCamera = pCamera;
	m_pObserver = pObserver;
	m_pUnimapWorker = pUnimapWorker;
	// vars
	int i=0;

	//////////////////////////
	// sites
	if( m_pObserver->m_nSite > 0 )
	{
		m_pSiteNameSelect->Clear();
		m_pSiteNameSelect->Freeze();
		// set Select options
		for( i=0; i<m_pObserver->m_nSite; i++ )
		{
			// append name to select
			m_pSiteNameSelect->Append( wxString(m_pObserver->m_vectSite[i]->m_strSiteName,wxConvUTF8) );
		}
		m_pSiteNameSelect->SetSelection( m_pObserver->m_nDefaultSite );
		m_pSiteNameSelect->Thaw();

	} else
		m_pSiteNameSelect->Disable();

	///////////////////
	// set current date/time
	wxDateTime now;
	now.SetToCurrent();
	m_pDate->SetValue( now );
	m_pTime->SetLabel( now.FormatTime() );

	/////////////////////////
	// if camera
	if( m_pCamera )
	{
		// set camera
		m_pGuideCamSelect->Clear();
		//m_vectCamDevices.clear();
		for( i=0; i<m_pCamera->m_vectHWDevices.size(); i++ )
		{
			if( m_pCamera->m_vectHWDevices[i].connected )
			{
				//m_vectCamDevices.push_back( m_pCamera->m_vectHWDevices[i] );
				m_pGuideCamSelect->Append( m_pCamera->m_vectHWDevices[i].strName );
			}
		}
		// set default selection and type of panel?
		m_pGuideCamSelect->SetSelection(0);

	} else
		m_pGuideCamSelect->Disable();

	Refresh( FALSE );
	Update( );
}

// GUI handlers
////////////////////////////////////////////////////////////////////
void CTelescopeSetupDlg::OnSiteAdd( wxCommandEvent& event )
{
	CObserverCfgDlg* pDlg = new CObserverCfgDlg( this );
	pDlg->SetConfig( m_pObserver, m_pCamera, m_pTelescope, m_pUnimapWorker );
	// show
	if( pDlg->ShowModal() == wxID_OK )
	{
		// update options here
	}
	delete( pDlg );

	return;
}

