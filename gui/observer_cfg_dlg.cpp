
// wx includes
#include "wx/notebook.h"
#include <wx/calctrl.h>
#include <wx/datetime.h>
#include <wx/filedlg.h>
#include <wx/image.h>

// local headers
#include "../util/func.h"
#include "location_dialog.h"
#include "../land/geoip.h"
#include "simple_common_dlg.h"
#include "../observer/observer.h"
#include "../telescope/telescope.h"
#include "../camera/camera.h"
#include "hardware_add_dlg.h"
#include "../unimap_worker.h"

// main header
#include "observer_cfg_dlg.h"

// CMainConfigView EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CObserverCfgDlg, wxDialog )
	EVT_NOTEBOOK_PAGE_CHANGED( -1, CObserverCfgDlg::OnNotebookTab )
	EVT_BUTTON( wxID_PROFILE_PHOTO_SET_BUTTON, CObserverCfgDlg::OnProfileSetPhoto )
	// address
	EVT_CHOICE( wxID_OBSERVER_CFG_COUNTRY, CObserverCfgDlg::OnSelectAddrLoc )
	EVT_CHOICE( wxID_OBSERVER_CFG_PROVINCE, CObserverCfgDlg::OnSelectAddrLoc )
	EVT_CHOICE( wxID_OBSERVER_CFG_CITY, CObserverCfgDlg::OnSelectAddrLoc )
	EVT_BUTTON( wxID_OBSERVER_CFG_CONTACT_MAP, CObserverCfgDlg::OnEartMapLocation )
	EVT_BUTTON( wxID_OBSERVER_CFG_CONTACT_GEOIP, CObserverCfgDlg::OnGeoIpLocation )
	// contact
	EVT_CHOICE( wxID_PROFILE_CONTACT_MESSENGER_SELECT, CObserverCfgDlg::OnSelectIMessType )
	// sites
	EVT_CHOICE( wxID_OBSERVER_CFG_SITES_NAME_SELECT, CObserverCfgDlg::OnSelectSite )
	EVT_BUTTON( wxID_OBSERVER_CFG_SITES_NAME_ADD, CObserverCfgDlg::OnSitesAdd )
	EVT_BUTTON( wxID_OBSERVER_CFG_SITES_NAME_DELETE, CObserverCfgDlg::OnSiteDelete )
	EVT_BUTTON( wxID_OBSERVER_CFG_SITES_NAME_RENAME, CObserverCfgDlg::OnSiteRename )
	EVT_CHOICE( wxID_OBSERVER_CFG_SITE_COUNTRY, CObserverCfgDlg::OnSelectAddrLoc )
	EVT_CHOICE( wxID_OBSERVER_CFG_SITE_PROVINCE, CObserverCfgDlg::OnSelectAddrLoc )
	EVT_CHOICE( wxID_OBSERVER_CFG_SITE_CITY, CObserverCfgDlg::OnSelectAddrLoc )
	EVT_BUTTON( wxID_OBSERVER_CFG_SITE_MAP, CObserverCfgDlg::OnEartMapLocation )
	EVT_BUTTON( wxID_OBSERVER_CFG_SITE_GEOIP, CObserverCfgDlg::OnGeoIpLocation )
	// hardware
	EVT_CHOICE( wxID_OBSERVER_CFG_HARDWARE_NAME_TYPE, CObserverCfgDlg::OnSelectHardwareType )
	EVT_CHOICE( wxID_OBSERVER_CFG_HARDWARE_NAME_SELECT, CObserverCfgDlg::OnSelectHardwareName )
	EVT_BUTTON( wxID_OBSERVER_CFG_HARDWARE_NAME_ADD, CObserverCfgDlg::OnHardwareAdd )
	EVT_BUTTON( wxID_OBSERVER_CFG_HARDWARE_NAME_DELETE, CObserverCfgDlg::OnHardwareDelete )
	// setups
	EVT_CHOICE( wxID_OBSERVER_CFG_SETUPS_NAME_SELECT, CObserverCfgDlg::OnSelectSetupName )
	EVT_BUTTON( wxID_OBSERVER_CFG_SETUPS_NAME_ADD, CObserverCfgDlg::OnSetupAdd )
	EVT_BUTTON( wxID_OBSERVER_CFG_SETUPS_NAME_DELETE, CObserverCfgDlg::OnSetupDelete )
	EVT_BUTTON( wxID_OBSERVER_CFG_SETUP_NAME_RENAME, CObserverCfgDlg::OnSetupRename )
	// validate observer data on ok
	EVT_BUTTON( wxID_OK, CObserverCfgDlg::OnValidate )
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
CObserverCfgDlg::CObserverCfgDlg(wxWindow *parent) : wxDialog(parent, -1,
                          wxT("Observer Setup"), wxDefaultPosition, wxSize( 760, 440 ),
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	// init some
	m_pEarth = NULL;
	m_strProfilePhotoFile = wxT("");

	wxString vectStrMessengerLabels[6];
	vectStrMessengerLabels[0] = wxT("Yahoo");
	vectStrMessengerLabels[1] = wxT("Skype");
	vectStrMessengerLabels[2] = wxT("GTalk");
	vectStrMessengerLabels[3] = wxT("Msn");
	vectStrMessengerLabels[4] = wxT("Aol");
	vectStrMessengerLabels[5] = wxT("Icq");
	// site names
	wxString vectSiteNameSelect[2];
	vectSiteNameSelect[0] = wxT("Default");

	// main dialog sizer
	wxFlexGridSizer *pTopSizer = new wxFlexGridSizer( 2, 1, 5, 5 );

	// building configure as a note book
	wxNotebook* pNotebook = new wxNotebook( this, -1, wxPoint(-1,-1), wxSize( -1, -1 ) );  //wxSize( 300, 240 ) ); 
	// PANEL :: PROFILE 0
	wxPanel* pPanelProfile = new wxPanel( pNotebook );
	// PANEL :: ADDRESS 1
	wxPanel* pPanelAddress = new wxPanel( pNotebook );
	// PANEL :: CONTACT 2
	wxPanel* pPanelContact = new wxPanel( pNotebook );
	// PANEL :: SITES 3
	wxPanel* pPanelSites = new wxPanel( pNotebook );
	// PANEL :: HARDWARE 4
	wxPanel* pPanelHardware = new wxPanel( pNotebook );
	// PANEL :: SETUPS 5
	wxPanel* pPanelSetups = new wxPanel( pNotebook );
	// PANEL :: DEFAULTS 6
	wxPanel* pPanelDefaults = new wxPanel( pNotebook );

	////////////
	// SIZER :: PROFILE
	wxFlexGridSizer* sizerProfile = new wxFlexGridSizer( 2, 1, 5, 5 );
	sizerProfile->AddGrowableCol( 1 );
	sizerProfile->AddGrowableCol( 0 );
	wxBoxSizer* sizerProfilePage = new wxBoxSizer( wxHORIZONTAL );
	sizerProfilePage->Add( sizerProfile, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxALL, 10 );
	// sub sizers
	wxFlexGridSizer* sizerProfileHead = new wxFlexGridSizer( 1, 2, 5, 5 );
//	sizerProfileHead->AddGrowableCol( 0 );
	wxFlexGridSizer* sizerProfileHeadA = new wxFlexGridSizer( 4, 2, 5, 5 );
	sizerProfileHeadA->AddGrowableCol( 1 );
	sizerProfileHeadA->AddGrowableCol( 0 );
	wxFlexGridSizer* sizerProfileHeadB = new wxFlexGridSizer( 2, 1, 5, 5 );
	sizerProfileHeadB->AddGrowableRow( 0 );
	sizerProfileHead->Add( sizerProfileHeadA, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP );
	sizerProfileHead->Add( sizerProfileHeadB, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP );
	// sub-sizer :: tail
	wxFlexGridSizer* sizerProfileTail = new wxFlexGridSizer( 3, 2, 5, 5 );
	sizerProfileHeadA->AddGrowableCol( 1 );
	sizerProfileHeadA->AddGrowableCol( 0 );
	sizerProfile->Add( sizerProfileHead, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerProfile->Add( sizerProfileTail, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	////////////
	// SIZER :: ADDRESS
	wxFlexGridSizer* sizerAddress = new wxFlexGridSizer( 8, 2, 5, 5 );
	sizerAddress->AddGrowableCol( 1 );
	sizerAddress->AddGrowableCol( 0 );
	wxBoxSizer* sizerAddressPage = new wxBoxSizer( wxHORIZONTAL );
	sizerAddressPage->Add( sizerAddress, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxALL, 10 );

	////////////
	// SIZER :: CONTACT
	wxFlexGridSizer* sizerContact = new wxFlexGridSizer( 8, 2, 5, 5 );
	sizerContact->AddGrowableCol( 1 );
	sizerContact->AddGrowableCol( 0 );
	wxBoxSizer* sizerContactPage = new wxBoxSizer( wxHORIZONTAL );
	sizerContactPage->Add( sizerContact, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxALL, 10 );

	////////////
	// SIZER :: SITES
	wxFlexGridSizer* sizerSites = new wxFlexGridSizer( 2, 1, 5, 5 );
	sizerSites->AddGrowableCol( 1 );
	sizerSites->AddGrowableCol( 0 );
	wxBoxSizer* sizerSitesPage = new wxBoxSizer( wxHORIZONTAL );
	sizerSitesPage->Add( sizerSites, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxALL, 10 );
	// sub sizers
	// head - box 
	wxStaticBox* pSitesHeadBox = new wxStaticBox( pPanelSites, -1, wxT("Add/Edit Sites"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pSitesHeadBoxSizer = new wxStaticBoxSizer( pSitesHeadBox, wxVERTICAL );
	wxFlexGridSizer* sizerSitesHead = new wxFlexGridSizer( 1, 5, 5, 5 );
	sizerSitesHead->AddGrowableCol( 1 );
	sizerSites->Add( pSitesHeadBoxSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxEXPAND|wxGROW );
	pSitesHeadBoxSizer->Add( sizerSitesHead, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// tail - box 
	wxStaticBox* pSitesTailBox = new wxStaticBox( pPanelSites, -1, wxT("Site Location"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pSitesTailBoxSizer = new wxStaticBoxSizer( pSitesTailBox, wxVERTICAL );
	// data sizer
	wxFlexGridSizer* sizerSitesTail = new wxFlexGridSizer( 4, 2, 5, 5 );
	sizerSitesTail->AddGrowableCol( 1 );
	sizerSitesTail->AddGrowableCol( 0 );
	sizerSites->Add( pSitesTailBoxSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxEXPAND|wxGROW );
	pSitesTailBoxSizer->Add( sizerSitesTail, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	////////////
	// SIZER :: HARDWARE
	wxFlexGridSizer* sizerHardware = new wxFlexGridSizer( 2, 1, 5, 5 );
	sizerHardware->AddGrowableCol( 1 );
	sizerHardware->AddGrowableCol( 0 );
	sizerHardware->AddGrowableRow( 1 );
	wxBoxSizer* sizerHardwarePage = new wxBoxSizer( wxHORIZONTAL );
	sizerHardwarePage->Add( sizerHardware, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 10 );
	// sub sizers
	// head - box 
	wxStaticBox* pHardwareHeadBox = new wxStaticBox( pPanelHardware, -1, wxT("Add/Edit Hardware"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pHardwareHeadBoxSizer = new wxStaticBoxSizer( pHardwareHeadBox, wxVERTICAL );
	wxFlexGridSizer* sizerHardwareHead = new wxFlexGridSizer( 1, 5, 5, 5 );
	sizerHardwareHead->AddGrowableCol( 1 );
	sizerHardware->Add( pHardwareHeadBoxSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxEXPAND|wxGROW );
	pHardwareHeadBoxSizer->Add( sizerHardwareHead, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// tail - box 
	wxStaticBox* pHardwareTailBox = new wxStaticBox( pPanelHardware, -1, wxT("Properties"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pHardwareTailBoxSizer = new wxStaticBoxSizer( pHardwareTailBox, wxVERTICAL );
	// data sizer
	sizerHardwareTail = new wxFlexGridSizer( 4, 2, 5, 5 );
	sizerHardwareTail->AddGrowableCol( 1 );
	sizerHardwareTail->AddGrowableCol( 0 );
	sizerHardware->Add( pHardwareTailBoxSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );
	pHardwareTailBoxSizer->Add( sizerHardwareTail, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	////////////
	// SIZER :: SETUPS
	wxFlexGridSizer* sizerSetups = new wxFlexGridSizer( 2, 1, 5, 5 );
	sizerSetups->AddGrowableCol( 1 );
	sizerSetups->AddGrowableCol( 0 );
	sizerSetups->AddGrowableRow( 1 );
	wxBoxSizer* sizerSetupsPage = new wxBoxSizer( wxHORIZONTAL );
	sizerSetupsPage->Add( sizerSetups, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 10 );
	// sub sizers
	// head - box 
	wxStaticBox* pSetupsHeadBox = new wxStaticBox( pPanelSetups, -1, wxT("Add/Edit Setups"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pSetupsHeadBoxSizer = new wxStaticBoxSizer( pSetupsHeadBox, wxVERTICAL );
	wxFlexGridSizer* sizerSetupsHead = new wxFlexGridSizer( 1, 5, 5, 5 );
	sizerSetupsHead->AddGrowableCol( 1 );
	sizerSetups->Add( pSetupsHeadBoxSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxEXPAND|wxGROW );
	pSetupsHeadBoxSizer->Add( sizerSetupsHead, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// tail - box 
	wxStaticBox* pSetupsTailBox = new wxStaticBox( pPanelSetups, -1, wxT("Properties"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pSetupsTailBoxSizer = new wxStaticBoxSizer( pSetupsTailBox, wxVERTICAL );
	// data sizer
	wxFlexGridSizer* sizerSetupsTail = new wxFlexGridSizer( 4, 2, 5, 5 );
	sizerSetupsTail->AddGrowableCol( 1 );
	sizerSetupsTail->AddGrowableCol( 0 );
	sizerSetups->Add( pSetupsTailBoxSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );
	pSetupsTailBoxSizer->Add( sizerSetupsTail, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	////////////
	// SIZER :: DEFAULTS
	wxFlexGridSizer* sizerDefaults = new wxFlexGridSizer( 6, 2, 5, 5 );
	sizerDefaults->AddGrowableCol( 1 );
	sizerDefaults->AddGrowableCol( 0 );
	wxBoxSizer* sizerDefaultsPage = new wxBoxSizer( wxHORIZONTAL );
	sizerDefaultsPage->Add( sizerDefaults, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxALL, 10 );

	/////////////////////////
	// GUI OBJECTS :: PROFILE
	m_pFirstName = new wxTextCtrl( pPanelProfile, -1, wxT(""), wxDefaultPosition, wxSize(200,-1) );
	m_pMidName = new wxTextCtrl( pPanelProfile, -1, wxT(""), wxDefaultPosition, wxSize(150,-1) );
	m_pLastName = new wxTextCtrl( pPanelProfile, -1, wxT(""), wxDefaultPosition, wxSize(200,-1) );
	m_pNickName = new wxTextCtrl( pPanelProfile, -1, wxT(""), wxDefaultPosition, wxSize(200,-1) );
	m_pBirthday = new wxDatePickerCtrl( pPanelProfile, wxID_PROFILE_BIRTHDAY, wxDefaultDateTime,
									wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN );
	m_pProfession = new wxTextCtrl( pPanelProfile, -1, wxT(""), wxDefaultPosition, wxSize(320,-1) );
	m_pHobbies = new wxTextCtrl( pPanelProfile, -1, wxT(""), wxDefaultPosition, wxSize(320,-1) );
	m_pWebsite = new wxTextCtrl( pPanelProfile, -1, wxT(""), wxDefaultPosition, wxSize(320,-1) );
	// profile photo
	wxBitmap bmpProfilePhoto( wxT("./resources/nophoto_s.gif"), wxBITMAP_TYPE_ANY );
	m_pProfilePhoto = new wxStaticBitmap( pPanelProfile, -1, bmpProfilePhoto, wxDefaultPosition, wxSize( 110, 115 ) );
	// load photo
	m_pSetPhotoButton = new wxButton( pPanelProfile, wxID_PROFILE_PHOTO_SET_BUTTON, wxT("Set Photo"), wxPoint(-1, -1), wxSize(-1,-1) );

	// GUI OBJECTS :: ADDRESS
	m_pCntAddrStreet = new wxTextCtrl( pPanelAddress, -1, wxT(""), wxDefaultPosition, wxSize(300,-1) );
	m_pCntZipCode = new wxTextCtrl( pPanelAddress, -1, wxT(""), wxDefaultPosition, wxSize(120,-1) );
	m_pCntCity = new wxChoice( pPanelAddress, wxID_OBSERVER_CFG_CITY, wxDefaultPosition, wxSize(160,-1) );
	m_pCntProvince = new wxChoice( pPanelAddress, wxID_OBSERVER_CFG_PROVINCE, wxDefaultPosition, wxSize(120,-1) );
	m_pCntCountry = new wxChoice( pPanelAddress, wxID_OBSERVER_CFG_COUNTRY , wxDefaultPosition, wxSize(200,-1) );
	m_pCntLatitude = new wxTextCtrl( pPanelAddress, wxID_OBSERVER_CFG_LATITUDE, wxT(""), wxDefaultPosition, wxSize(80,-1) );
	m_pCntLongitude = new wxTextCtrl( pPanelAddress, wxID_OBSERVER_CFG_LONGITUDE, wxT(""), wxDefaultPosition, wxSize(80,-1) );
	m_pCntAltitude = new wxTextCtrl( pPanelAddress, wxID_OBSERVER_CFG_ALTITUDE, wxT(""), wxDefaultPosition, wxSize(60,-1) );

	// GUI OBJECTS :: CONTACT
	m_pMobile = new wxTextCtrl( pPanelContact, -1, wxT(""), wxDefaultPosition, wxSize(200,-1) );
	m_pPhone = new wxTextCtrl( pPanelContact, -1, wxT(""), wxDefaultPosition, wxSize(200,-1) );
	m_pFax = new wxTextCtrl( pPanelContact, -1, wxT(""), wxDefaultPosition, wxSize(200,-1) );
	m_pEmail = new wxTextCtrl( pPanelContact, -1, wxT(""), wxDefaultPosition, wxSize(200,-1) );
	m_pMessenger = new wxTextCtrl( pPanelContact, -1, wxT(""), wxDefaultPosition, wxSize(135,-1) );
	m_pMessengerType = new wxChoice( pPanelContact, wxID_PROFILE_CONTACT_MESSENGER_SELECT, 
										wxDefaultPosition, wxSize(-1,-1), 6, 
										vectStrMessengerLabels, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pMessengerType->SetSelection( 0 );

	// GUI OBJECTS :: SITES
	m_pSiteNameSelect = new wxChoice( pPanelSites, wxID_OBSERVER_CFG_SITES_NAME_SELECT,
										wxDefaultPosition, wxSize(200,-1), 1, vectSiteNameSelect,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSiteNameSelect->SetSelection( 0 );
	m_pSiteNameButtonAdd = new wxButton( pPanelSites, wxID_OBSERVER_CFG_SITES_NAME_ADD, wxT("Add"), wxPoint(-1, -1), wxSize(30,-1) );
	m_pSiteNameButtonDelete = new wxButton( pPanelSites, wxID_OBSERVER_CFG_SITES_NAME_DELETE, wxT("Delete"), wxPoint(-1, -1), wxSize(43,-1) );
	wxButton* pSiteNameButtonRename = new wxButton( pPanelSites, wxID_OBSERVER_CFG_SITES_NAME_RENAME, wxT("Rename"), wxPoint(-1, -1), wxSize(50,-1) );
	// :: sites :: location
	m_pSiteCity = new wxChoice( pPanelSites, wxID_OBSERVER_CFG_SITE_CITY, wxDefaultPosition, wxSize(160,-1) );
	m_pSiteProvince = new wxChoice( pPanelSites, wxID_OBSERVER_CFG_SITE_PROVINCE, wxDefaultPosition, wxSize(120,-1) );
	m_pSiteCountry = new wxChoice( pPanelSites, wxID_OBSERVER_CFG_SITE_COUNTRY , wxDefaultPosition, wxSize(200,-1) );
	m_pSiteLatitude = new wxTextCtrl( pPanelSites, wxID_OBSERVER_CFG_SITE_LATITUDE, wxT(""), wxDefaultPosition, wxSize(80,-1) );
	m_pSiteLongitude = new wxTextCtrl( pPanelSites, wxID_OBSERVER_CFG_SITE_LONGITUDE, wxT(""), wxDefaultPosition, wxSize(80,-1) );
	m_pSiteAltitude = new wxTextCtrl( pPanelSites, wxID_OBSERVER_CFG_SITE_ALTITUDE, wxT(""), wxDefaultPosition, wxSize(80,-1) );
	m_pSiteNotes = new wxTextCtrl( pPanelSites, wxID_OBSERVER_CFG_SITE_NOTES, wxT(""), wxDefaultPosition, wxSize(250,-1) );

	// GUI OBJECTS :: HARDWARE
	m_pHardwareNameType = new wxChoice( pPanelHardware, wxID_OBSERVER_CFG_HARDWARE_NAME_TYPE,
										wxDefaultPosition, wxSize(100,-1), 0, NULL,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pHardwareNameType->SetSelection( 0 );
	m_pHardwareNameSelect = new wxChoice( pPanelHardware, wxID_OBSERVER_CFG_HARDWARE_NAME_SELECT,
										wxDefaultPosition, wxSize(150,-1), 0, NULL,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pHardwareNameSelect->Disable();
	m_pHardwareNameButtonAdd = new wxButton( pPanelHardware, wxID_OBSERVER_CFG_HARDWARE_NAME_ADD, wxT("Add"), wxPoint(-1, -1), wxSize(30,-1) );
	m_pHardwareNameButtonDelete = new wxButton( pPanelHardware, wxID_OBSERVER_CFG_HARDWARE_NAME_DELETE, wxT("Delete"), wxPoint(-1, -1), wxSize(43,-1) );
	// labels
	m_pHardwareLabel1 = new wxStaticText( pPanelHardware, -1, wxT("Field 1:"));
	m_pHardwareLabel2 = new wxStaticText( pPanelHardware, -1, wxT("Field 2:"));
	m_pHardwareLabel3 = new wxStaticText( pPanelHardware, -1, wxT("Field 3:"));
	m_pHardwareLabel4 = new wxStaticText( pPanelHardware, -1, wxT("Field 4:"));
	m_pHardwareLabel5 = new wxStaticText( pPanelHardware, -1, wxT("Field 5:"));
	// text entry 
	m_pHardwareEntry1 = new wxTextCtrl( pPanelHardware, -1, wxT(""), wxDefaultPosition, wxSize(150,-1) );
	m_pHardwareEntry2 = new wxTextCtrl( pPanelHardware, -1, wxT(""), wxDefaultPosition, wxSize(150,-1) );
	m_pHardwareEntry3 = new wxTextCtrl( pPanelHardware, -1, wxT(""), wxDefaultPosition, wxSize(150,-1) );
	m_pHardwareEntry4 = new wxTextCtrl( pPanelHardware, -1, wxT(""), wxDefaultPosition, wxSize(150,-1) );
	// select fields
	m_pHardwareSelect1 = new wxChoice( pPanelHardware, -1, wxDefaultPosition, wxSize(150,-1), 0, NULL,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// GUI OBJECTS :: SETUPS
	m_pSetupNameSelect = new wxChoice( pPanelSetups, wxID_OBSERVER_CFG_SETUPS_NAME_SELECT,
										wxDefaultPosition, wxSize(150,-1), 0, NULL,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSetupNameSelect->Disable();
	m_pSetupNameButtonAdd = new wxButton( pPanelSetups, wxID_OBSERVER_CFG_SETUPS_NAME_ADD, wxT("Add"), wxPoint(-1, -1), wxSize(30,-1) );
	m_pSetupNameButtonDelete = new wxButton( pPanelSetups, wxID_OBSERVER_CFG_SETUPS_NAME_DELETE, wxT("Delete"), wxPoint(-1, -1), wxSize(43,-1) );
	wxButton* pSetupNameButtonRename = new wxButton( pPanelSetups, wxID_OBSERVER_CFG_SETUP_NAME_RENAME, wxT("Rename"), wxPoint(-1, -1), wxSize(50,-1) );
	m_pSetupType = new wxChoice( pPanelSetups, -1, wxDefaultPosition, wxSize(150,-1), g_nSetupsTypes, g_vectSetupsTypes,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSetupType->SetSelection(0);
	m_pSetupTelescope = new wxChoice( pPanelSetups, -1, wxDefaultPosition, wxSize(150,-1), 0, NULL,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSetupMount = new wxChoice( pPanelSetups, -1, wxDefaultPosition, wxSize(150,-1), 0, NULL,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSetupCamera = new wxChoice( pPanelSetups, -1, wxDefaultPosition, wxSize(150,-1), 0, NULL,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSetupFilter = new wxChoice( pPanelSetups, -1, wxDefaultPosition, wxSize(150,-1), 0, NULL,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSetupFocuser = new wxChoice( pPanelSetups, -1, wxDefaultPosition, wxSize(150,-1), 0, NULL,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// GUI OBJECTS :: DEFAULTS
	m_pDefaultSite = new wxChoice( pPanelDefaults, -1, wxDefaultPosition, wxSize(200,-1), 0, NULL );
	m_pDefaultSetup = new wxChoice( pPanelDefaults, -1, wxDefaultPosition, wxSize(200,-1), 0, NULL );
	m_pDefaultTelescope = new wxChoice( pPanelDefaults, -1, wxDefaultPosition, wxSize(200,-1), 0, NULL );
	m_pDefaultCamera = new wxChoice( pPanelDefaults, -1, wxDefaultPosition, wxSize(200,-1), 0, NULL );
	m_pDefaultMount = new wxChoice( pPanelDefaults, -1, wxDefaultPosition, wxSize(200,-1), 0, NULL );
	m_pDefaultFocuser = new wxChoice( pPanelDefaults, -1, wxDefaultPosition, wxSize(200,-1), 0, NULL );

	/////////////////////////////////////////
	// populate PROFILE panel
	// :: first name
	sizerProfileHeadA->Add( new wxStaticText( pPanelProfile, -1, wxT("First Name:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerProfileHeadA->Add( m_pFirstName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: mid name
	sizerProfileHeadA->Add( new wxStaticText( pPanelProfile, -1, wxT("Middle Name:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerProfileHeadA->Add( m_pMidName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: Last name
	sizerProfileHeadA->Add( new wxStaticText( pPanelProfile, -1, wxT("Last Name:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerProfileHeadA->Add( m_pLastName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: nick name
	sizerProfileHeadA->Add( new wxStaticText( pPanelProfile, -1, wxT("Nick Name:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerProfileHeadA->Add( m_pNickName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: birthday
	sizerProfileHeadA->Add( new wxStaticText( pPanelProfile, -1, wxT("Birthday:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerProfileHeadA->Add( m_pBirthday, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: photo image/button
	sizerProfileHeadB->Add( m_pProfilePhoto, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerProfileHeadB->Add( m_pSetPhotoButton, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	/////
	// :: profession
	sizerProfileTail->Add( new wxStaticText( pPanelProfile, -1, wxT("Profession:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerProfileTail->Add( m_pProfession, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: hobbies
	sizerProfileTail->Add( new wxStaticText( pPanelProfile, -1, wxT("Hobbies:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerProfileTail->Add( m_pHobbies, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: website
	sizerProfileTail->Add( new wxStaticText( pPanelProfile, -1, wxT("Website:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerProfileTail->Add( m_pWebsite, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////////////////
	// populate ADDRESS panel
	// :: street/apt/no
	sizerAddress->Add( new wxStaticText( pPanelAddress, -1, wxT("Street/Apt/No:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerAddress->Add( m_pCntAddrStreet, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: city
	sizerAddress->Add( new wxStaticText( pPanelAddress, -1, wxT("City:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerAddressCProv = new wxFlexGridSizer( 1, 4, 5, 5 );
	sizerAddressCProv->AddGrowableCol( 1 );
	wxButton* pCntMapButton = new wxButton( pPanelAddress, wxID_OBSERVER_CFG_CONTACT_MAP, wxT("Map"), wxPoint( -1, -1 ), wxSize( 35, -1 ) );
	wxButton* pCntGeoIpButton = new wxButton( pPanelAddress, wxID_OBSERVER_CFG_CONTACT_GEOIP, wxT("GeoIp"), wxPoint( -1, -1 ), wxSize( 40, -1 ) );
	wxButton* pCntGpsButton = new wxButton( pPanelAddress, wxID_OBSERVER_CFG_CONTACT_GPS, wxT("GPS"), wxPoint( -1, -1 ), wxSize( 35, -1 ) );
	pCntGpsButton->Disable();
	sizerAddressCProv->Add( m_pCntCity, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerAddressCProv->Add( pCntMapButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerAddressCProv->Add( pCntGeoIpButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerAddressCProv->Add( pCntGpsButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerAddress->Add( sizerAddressCProv, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: province/state
	sizerAddress->Add( new wxStaticText( pPanelAddress, -1, wxT("Province(State):")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerAddress->Add( m_pCntProvince, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: country
	sizerAddress->Add( new wxStaticText( pPanelAddress, -1, wxT("Country:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerAddress->Add( m_pCntCountry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: zipcode
	sizerAddress->Add( new wxStaticText( pPanelAddress, -1, wxT("Zip Code:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerAddress->Add( m_pCntZipCode, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: Latitude
	sizerAddress->Add( new wxStaticText( pPanelAddress, -1, wxT("Latitude(°N):")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerAddress->Add( m_pCntLatitude, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: Longitude
	sizerAddress->Add( new wxStaticText( pPanelAddress, -1, wxT("Longitude(°E):")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerAddress->Add( m_pCntLongitude, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: Altitude
	sizerAddress->Add( new wxStaticText( pPanelAddress, -1, wxT("Altitude(m):")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerAddress->Add( m_pCntAltitude, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////////////////
	// populate CONTACT panel
	// :: mobile
	sizerContact->Add( new wxStaticText( pPanelContact, -1, wxT("Mobile:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerContact->Add( m_pMobile, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: phone
	sizerContact->Add( new wxStaticText( pPanelContact, -1, wxT("Phone:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerContact->Add( m_pPhone, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: fax
	sizerContact->Add( new wxStaticText( pPanelContact, -1, wxT("Fax:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerContact->Add( m_pFax, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: email
	sizerContact->Add( new wxStaticText( pPanelContact, -1, wxT("Email:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerContact->Add( m_pEmail, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: messenger
	sizerContact->Add( new wxStaticText( pPanelContact, -1, wxT("Messenger:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerContactMess = new wxFlexGridSizer( 1, 2, 5, 5 );
	sizerContactMess->AddGrowableCol( 1 );
	sizerContactMess->Add( m_pMessengerType, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerContactMess->Add( m_pMessenger, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerContact->Add( sizerContactMess, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////////////////
	// populate SITES panel
	// :: site name
	sizerSitesHead->Add( new wxStaticText( pPanelSites, -1, wxT("Site:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerSitesHead->Add( m_pSiteNameSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSitesHead->Add( m_pSiteNameButtonAdd, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSitesHead->Add( m_pSiteNameButtonDelete, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSitesHead->Add( pSiteNameButtonRename, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: city
	sizerSitesTail->Add( new wxStaticText( pPanelSites, -1, wxT("City:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerSitesCProv = new wxFlexGridSizer( 1, 4, 5, 5 );
	sizerSitesCProv->AddGrowableCol( 1 );
	wxButton* pSiteMapButton = new wxButton( pPanelSites, wxID_OBSERVER_CFG_SITE_MAP, wxT("Map"), wxPoint( -1, -1 ), wxSize( 35, -1 ) );
	wxButton* pSiteGeoIpButton = new wxButton( pPanelSites, wxID_OBSERVER_CFG_SITE_GEOIP, wxT("GeoIp"), wxPoint( -1, -1 ), wxSize( 40, -1 ) );
	wxButton* pSiteGpsButton = new wxButton( pPanelSites, wxID_OBSERVER_CFG_SITE_GPS, wxT("GPS"), wxPoint( -1, -1 ), wxSize( 35, -1 ) );
	pSiteGpsButton->Disable();
	sizerSitesCProv->Add( m_pSiteCity, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSitesCProv->Add( pSiteMapButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSitesCProv->Add( pSiteGeoIpButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSitesCProv->Add( pSiteGpsButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSitesTail->Add( sizerSitesCProv, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: province(state)
	sizerSitesTail->Add( new wxStaticText( pPanelSites, -1, wxT("Province(State):")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerSitesTail->Add( m_pSiteProvince, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: country/zipcode
	sizerSitesTail->Add( new wxStaticText( pPanelSites, -1, wxT("Country:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerSitesTail->Add( m_pSiteCountry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: Latitude
	sizerSitesTail->Add( new wxStaticText( pPanelSites, -1, wxT("Lat(°N)/Lon(°E):")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerSitesLLA = new wxFlexGridSizer( 1, 2, 5, 5 );
	sizerSitesLLA->AddGrowableCol( 1 );
	sizerSitesLLA->Add( m_pSiteLatitude, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSitesLLA->Add( m_pSiteLongitude, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSitesTail->Add( sizerSitesLLA, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: Altitude
	sizerSitesTail->Add( new wxStaticText( pPanelSites, -1, wxT("Altitude(m):")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerSitesTail->Add( m_pSiteAltitude, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: Comments
	sizerSitesTail->Add( new wxStaticText( pPanelSites, -1, wxT("Notes:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerSitesTail->Add( m_pSiteNotes, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////////////////
	// populate HARDWARE panel
	// :: site name
	sizerHardwareHead->Add( new wxStaticText( pPanelHardware, -1, wxT("Name:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerHardwareHead->Add( m_pHardwareNameType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerHardwareHead->Add( m_pHardwareNameSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerHardwareHead->Add( m_pHardwareNameButtonAdd, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerHardwareHead->Add( m_pHardwareNameButtonDelete, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: select 1
	sizerHardwareTail->Add( m_pHardwareLabel1, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerHardwareTail->Add( m_pHardwareSelect1, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: field 1
	sizerHardwareTail->Add( m_pHardwareLabel2, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerHardwareTail->Add( m_pHardwareEntry1, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: field 2
	sizerHardwareTail->Add( m_pHardwareLabel3, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerHardwareTail->Add( m_pHardwareEntry2, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: field 3
	sizerHardwareTail->Add( m_pHardwareLabel4, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerHardwareTail->Add( m_pHardwareEntry3, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: field 4
	sizerHardwareTail->Add( m_pHardwareLabel5, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerHardwareTail->Add( m_pHardwareEntry4, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////////////////
	// populate SETUPS panel
	// :: setup name
	sizerSetupsHead->Add( new wxStaticText( pPanelSetups, -1, wxT("Setup:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerSetupsHead->Add( m_pSetupNameSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupsHead->Add( m_pSetupNameButtonAdd, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupsHead->Add( m_pSetupNameButtonDelete, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerSetupsHead->Add( pSetupNameButtonRename, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: type
	sizerSetupsTail->Add( new wxStaticText( pPanelSetups, -1, wxT("Type:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerSetupsTail->Add( m_pSetupType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: telescope
	sizerSetupsTail->Add( new wxStaticText( pPanelSetups, -1, wxT("Telescope:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerSetupsTail->Add( m_pSetupTelescope, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: mount
	sizerSetupsTail->Add( new wxStaticText( pPanelSetups, -1, wxT("Mount:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerSetupsTail->Add( m_pSetupMount, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: camera
	sizerSetupsTail->Add( new wxStaticText( pPanelSetups, -1, wxT("Camera:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerSetupsTail->Add( m_pSetupCamera, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: filter
	sizerSetupsTail->Add( new wxStaticText( pPanelSetups, -1, wxT("Filter:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerSetupsTail->Add( m_pSetupFilter, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: focuser
	sizerSetupsTail->Add( new wxStaticText( pPanelSetups, -1, wxT("Focuser:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerSetupsTail->Add( m_pSetupFocuser, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////////////////
	// populate DEFAULTS panel
	// :: sites
	sizerDefaults->Add( new wxStaticText( pPanelDefaults, -1, wxT("Site:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerDefaults->Add( m_pDefaultSite, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: setups
	sizerDefaults->Add( new wxStaticText( pPanelDefaults, -1, wxT("Setup:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerDefaults->Add( m_pDefaultSetup, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: telescope
	sizerDefaults->Add( new wxStaticText( pPanelDefaults, -1, wxT("Telescope:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerDefaults->Add( m_pDefaultTelescope, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: camera
	sizerDefaults->Add( new wxStaticText( pPanelDefaults, -1, wxT("Camera:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerDefaults->Add( m_pDefaultCamera, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: mount
	sizerDefaults->Add( new wxStaticText( pPanelDefaults, -1, wxT("Mount:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerDefaults->Add( m_pDefaultMount, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: focuser
	sizerDefaults->Add( new wxStaticText( pPanelDefaults, -1, wxT("Focuser:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerDefaults->Add( m_pDefaultFocuser, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// add sizer/panels to notebook
	pPanelProfile->SetSizer( sizerProfilePage );
	pNotebook->AddPage( pPanelProfile, wxT("Profile") );
	pPanelAddress->SetSizer( sizerAddressPage );
	pNotebook->AddPage( pPanelAddress, wxT("Address") );
	pPanelContact->SetSizer( sizerContactPage );
	pNotebook->AddPage( pPanelContact, wxT("Contact") );
	pPanelSites->SetSizer( sizerSitesPage );
	pNotebook->AddPage( pPanelSites, wxT("Sites") );
	pPanelHardware->SetSizer( sizerHardwarePage );
	pNotebook->AddPage( pPanelHardware, wxT("Hardware") );
	pPanelSetups->SetSizer( sizerSetupsPage );
	pNotebook->AddPage( pPanelSetups, wxT("Setups") );
	pPanelDefaults->SetSizer( sizerDefaultsPage );
	pNotebook->AddPage( pPanelDefaults, wxT("Defaults") );

	pTopSizer->Add( pNotebook, 0, wxGROW | wxALL|wxEXPAND, 10 );
	pTopSizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10 );

	pNotebook->Layout();
	SetSizer(pTopSizer);
	SetAutoLayout(TRUE);
	pTopSizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
CObserverCfgDlg::~CObserverCfgDlg( ) 
{
	// profile
	delete( m_pFirstName );
	delete( m_pMidName );
	delete( m_pLastName );
	delete( m_pNickName );
	delete( m_pBirthday );
	delete( m_pProfession );
	delete( m_pHobbies );
	delete( m_pWebsite );
	delete( m_pProfilePhoto );
	delete( m_pSetPhotoButton );
	// address
	delete( m_pCntAddrStreet );
	delete( m_pCntZipCode );
	delete( m_pCntCity );
	delete( m_pCntProvince );
	delete( m_pCntCountry );
	delete( m_pCntLatitude );
	delete( m_pCntLongitude );
	delete( m_pCntAltitude );
	// contact
	delete( m_pMobile );
	delete( m_pPhone );
	delete( m_pFax );
	delete( m_pEmail );
	delete( m_pMessengerType );
	delete( m_pMessenger );
	// sites
	delete( m_pSiteNameSelect );
	delete( m_pSiteNameButtonAdd );
	delete( m_pSiteNameButtonDelete );
	delete( m_pSiteCity );
	delete( m_pSiteProvince );
	delete( m_pSiteCountry );
	delete( m_pSiteLatitude );
	delete( m_pSiteLongitude );
	delete( m_pSiteAltitude );
	delete( m_pSiteNotes );
	////////////////
	// hardware
	delete( m_pHardwareNameType );
	delete( m_pHardwareNameSelect );
	delete( m_pHardwareNameButtonAdd );
	delete( m_pHardwareNameButtonDelete );
	// hardware :: labels
	delete( m_pHardwareLabel1 );
	delete( m_pHardwareLabel2 );
	delete( m_pHardwareLabel3 );
	delete( m_pHardwareLabel4 );
	delete( m_pHardwareLabel5 );
	// hardware :: free fields
	delete( m_pHardwareEntry1 );
	delete( m_pHardwareEntry2 );
	delete( m_pHardwareEntry3 );
	delete( m_pHardwareEntry4 );
	// select fiedls
	delete( m_pHardwareSelect1 );
	///////////////
	// setups
	delete( m_pSetupNameSelect );
	delete( m_pSetupNameButtonAdd );
	delete( m_pSetupNameButtonDelete );
	delete( m_pSetupType );
	delete( m_pSetupTelescope );
	delete( m_pSetupMount );
	delete( m_pSetupCamera );
	delete( m_pSetupFilter );
	delete( m_pSetupFocuser );
	// defaults
	delete( m_pDefaultSite );
	delete( m_pDefaultSetup );
	delete( m_pDefaultTelescope );
	delete( m_pDefaultCamera );
	delete( m_pDefaultMount );
	delete( m_pDefaultFocuser );
	
	// delete other local objects
	if( m_pHardware ) delete( m_pHardware );
	if( m_pEarth != NULL ) delete( m_pEarth );
	if( m_vectSite != NULL )
	{ 
		for(int i=0; i<m_nSite; i++) delete( m_vectSite[i] );
		free( m_vectSite );
	}
	m_nSite = 0;
}

// Method:	SetConfig
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::SetConfig( CObserver* pObserver, CCamera* pCamera, 
									CTelescope* pTelescope, CUnimapWorker* pUnimapWorker )
{
	m_pUnimapWorker = pUnimapWorker;

	wxString strTmp;
	int i=0;

	m_pObserver = pObserver;
	m_pCamera = pCamera;
	m_pTelescope = pTelescope;
	// set hardware type
	m_pHardwareNameType->Clear();
	for( i=0; i<9; i++ ) m_pHardwareNameType->Append( g_vectHardwareTypes[i].type );
	m_pHardwareNameType->SetSelection(0);

	///////////////////
	// PROFILE
	m_pFirstName->SetValue( m_pObserver->m_strFirstName );
	m_pMidName->SetValue( m_pObserver->m_strMiddleName );
	m_pLastName->SetValue( m_pObserver->m_strLastName );
	m_pNickName->SetValue( m_pObserver->m_strNickName );
	m_pProfession->SetValue( m_pObserver->m_strProfession );
	m_pHobbies->SetValue( m_pObserver->m_srtHobbies );
	m_pWebsite->SetValue( m_pObserver->m_strWebsite );
	// :: birth day
	if( m_pObserver->m_nBirthday != 0.0 ) m_pBirthday->SetValue( wxDateTime( m_pObserver->m_nBirthday ) );
	// :: photo
	m_strProfilePhotoFile = m_pObserver->m_strPhotoFile;
	if( m_strProfilePhotoFile.Length() )
	{
		wxImage imgProfilePhoto;
		if( imgProfilePhoto.LoadFile( m_strProfilePhotoFile, wxBITMAP_TYPE_ANY ) )
			m_pProfilePhoto->SetBitmap( wxBitmap( imgProfilePhoto.Rescale( 110, 115 ) ) );
	}
	// PROFILE :: ADDRESS
	m_pCntAddrStreet->SetValue( m_pObserver->m_strAddress );
	m_pCntZipCode->SetValue( m_pObserver->m_strZipCode );
	// latitude
	strTmp.Printf( wxT("%.6f"), m_pObserver->m_nLatitude );
	m_pCntLatitude->SetLabel( strTmp );
	// longitude
	strTmp.Printf( wxT("%.6f"), m_pObserver->m_nLongitude );
	m_pCntLongitude->SetLabel( strTmp );
	// altitude
	strTmp.Printf( wxT("%.2f"), m_pObserver->m_nAltitude );
	m_pCntAltitude->SetLabel( strTmp );
	// PROFILE :: CONTACT
	m_pMobile->SetValue( m_pObserver->m_strMobile );
	m_pPhone->SetValue( m_pObserver->m_strPhone );
	m_pFax->SetValue( m_pObserver->m_strFax );
	m_pEmail->SetValue( m_pObserver->m_strEmail );
	// set messenger by type - first default yahoo
	m_vectMessenger[0] = m_pObserver->m_strMessengerYahoo;
	m_vectMessenger[1] = m_pObserver->m_strMessengerSkype;
	m_vectMessenger[2] = m_pObserver->m_strMessengerGTalk;
	m_vectMessenger[3] = m_pObserver->m_strMessengerMsn;
	m_vectMessenger[4] = m_pObserver->m_strMessengerAol;
	m_vectMessenger[5] = m_pObserver->m_strMessengerIcq;
	m_nSelectedMessenger = 0;
	m_pMessenger->SetValue( m_vectMessenger[m_nSelectedMessenger] );

	//////////////////
	// SITES
	m_nSite = m_pObserver->m_nSite;
	if( m_nSite > 0 )
	{
		m_pSiteNameSelect->Clear();
		m_pSiteNameSelect->Freeze();
		// allocate
		m_vectSite = (CObserverSite**) calloc( m_nSite, sizeof(CObserverSite*) );
		// set local vector 
		for( i=0; i<m_nSite; i++ )
		{
			m_vectSite[i] =  new CObserverSite();

			m_vectSite[i]->m_strSiteName = m_pObserver->m_vectSite[i]->m_strSiteName;
			m_vectSite[i]->m_strCity = m_pObserver->m_vectSite[i]->m_strCity;
			m_vectSite[i]->m_strProvince = m_pObserver->m_vectSite[i]->m_strProvince;
			m_vectSite[i]->m_strCountry = m_pObserver->m_vectSite[i]->m_strCountry;
			m_vectSite[i]->m_nLatitude = m_pObserver->m_vectSite[i]->m_nLatitude;
			m_vectSite[i]->m_nLongitude = m_pObserver->m_vectSite[i]->m_nLongitude;
			m_vectSite[i]->m_nAltitude = m_pObserver->m_vectSite[i]->m_nAltitude;
			m_vectSite[i]->m_strSiteNotes = m_pObserver->m_vectSite[i]->m_strSiteNotes;
			// append name to select
			m_pSiteNameSelect->Append( m_vectSite[i]->m_strSiteName );
		}
		m_pSiteNameSelect->SetSelection( 0 );
		m_pSiteNameSelect->Thaw();

		///////
		// latitude
		strTmp.Printf( wxT("%.6f"), m_vectSite[0]->m_nLatitude );
		m_pSiteLatitude->SetLabel( strTmp );
		// longitude
		strTmp.Printf( wxT("%.6f"), m_vectSite[0]->m_nLongitude );
		m_pSiteLongitude->SetLabel( strTmp );
		// altitude
		strTmp.Printf( wxT("%.2f"), m_vectSite[0]->m_nAltitude );
		m_pSiteAltitude->SetLabel( strTmp );
		// notes
		m_pSiteNotes->SetLabel( m_vectSite[0]->m_strSiteNotes );

	} else
	{
		// default 1 site = Default
		m_nSite = 1;
		m_vectSite = (CObserverSite**) calloc( m_nSite, sizeof(CObserverSite*) );
		for( i=0; i<m_nSite; i++ ) m_vectSite[i] =  new CObserverSite();

		m_vectSite[0]->m_strSiteName = m_pSiteNameSelect->GetLabelText();
	}
	// set default to zero
	m_nSelectSite = 0;

	//////////////////////////
	// HARDWARE/SETUPS
	m_pHardware = new CObserverHardware();
	m_pObserver->m_pHardware->CopyTo( m_pHardware );
	// here we select the hardware type
	SelectHardwareType( 0 );
	SelectHardwareName( );

	/////////////
	// SETUPS
	if( m_pHardware->m_vectSetup.size() > 0 )
	{
		m_pSetupNameSelect->Enable();
		m_pSetupNameSelect->Clear();
		m_pSetupNameSelect->Freeze();
		for( i=0; i<m_pHardware->m_vectSetup.size(); i++ )
			m_pSetupNameSelect->Append( m_pHardware->m_vectSetup[i].name );
		m_pSetupNameSelect->SetSelection(0);
		m_pSetupNameSelect->Thaw();
		m_nSelectedSetup = 0;
		GetSetupSelected( 0 );

	} else
	{
		m_pSetupNameSelect->Clear();
		m_pSetupNameSelect->Disable();
		m_nSelectedSetup = -1;
	}

	/////////////////////
	// DEFAULTS
	GetDefaults( );

	return;
}

// Method:	OnProfileSetPhoto
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnProfileSetPhoto( wxCommandEvent& pEvent )
{
	wxFileDialog fileDlg( this, wxT("Select Observer Profile Photo"), wxT("./"), wxT(""), wxT(DEFAULT_LOAD_IMAGE_FILE_TYPE),
							wxFD_DEFAULT_STYLE| wxFD_FILE_MUST_EXIST );

	if( fileDlg.ShowModal() == wxID_OK )
	{
		// get file name
		wxString strPhotoFile = fileDlg.GetPath( );
		m_strProfilePhotoFile = strPhotoFile;
		if( m_strProfilePhotoFile.Length() )
		{
			wxImage imgProfilePhoto;
			imgProfilePhoto.LoadFile( strPhotoFile, wxBITMAP_TYPE_ANY );
			m_pProfilePhoto->SetBitmap( wxBitmap( imgProfilePhoto.Rescale( 110, 115 ) ) );

			Update( );
			Refresh( false );
		}
	}

	return;
}

// Method:	OnNotebookTab
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnNotebookTab( wxNotebookEvent& pEvent )
{
	int nId = pEvent.GetSelection();

	// if site tabs - load earth locations
	if( nId == 1 || nId == 3 ) 
	{
		// here initialize my location map obj
		if( !m_pEarth ) 
		{
			m_pEarth = new CLocationEarth( NULL );
//			m_pEarth->LoadLocations( );

			// set address location
			SetLocByName( m_pObserver->m_strCountry, m_pObserver->m_strProvince, 
						m_pObserver->m_strCity, m_pCntCountry, m_pCntProvince, m_pCntCity, 0 );

			////////////////////
			// sites panel
			if( m_nSite > 0 )
			{
				int nSiteId = m_pSiteNameSelect->GetSelection();
				// set address location
				if( m_pObserver->m_nSite <= 0 )
					SetLocByName( m_vectSite[nSiteId]->m_strCountry, m_vectSite[nSiteId]->m_strProvince, 
							m_vectSite[nSiteId]->m_strCity, m_pSiteCountry, m_pSiteProvince, m_pSiteCity, 1 );
				else
					SetLocByName( m_vectSite[nSiteId]->m_strCountry, m_vectSite[nSiteId]->m_strProvince, 
							m_vectSite[nSiteId]->m_strCity, m_pSiteCountry, m_pSiteProvince, m_pSiteCity, 0 );

			} else
			{
				//m_pEarth->SetCountriesWidget( m_pSiteCountry, nCountryId );
				//m_pEarth->SetRegionsWidget( m_pSiteProvince, nCountryId, nRegionId );
				//m_pEarth->SetCitiesWidget( m_pSiteCity, nCountryId, nRegionId, nCityId );
				// only first time - when no default set
				//if( m_pObserver->m_nSite <= 0 ) SetCity( 1 );
			}
		}

	// else if setup tab - reload selects
	} else if( nId == 5 )
	{
		GetSetupSelected( m_nSelectedSetup );

	// if defaults
	} else if( nId == 6 )
	{
		GetDefaults( );
	}
}

// Method:	SetLocationByString
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::SetLocByName( wxString& strCountry, wxString& strProvince, wxString& strCity, 
									wxChoice* pCountry, wxChoice* pProvince, wxChoice* pCity, int bCity )
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

	m_pEarth->SetCountriesWidget( pCountry, nCountryId );
	m_pEarth->SetRegionsWidget( pProvince, nCountryId, nRegionId );
	m_pEarth->SetCitiesWidget( pCity, nCountryId, nRegionId, nCityId );

	if( bCity ) SetCity( 1 );

	return;
}

// Method:	OnSelectIMessType
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnSelectIMessType( wxCommandEvent& pEvent )
{
	m_vectMessenger[m_nSelectedMessenger] = m_pMessenger->GetLineText(0).Trim(0).Trim(1);

	m_nSelectedMessenger = m_pMessengerType->GetSelection();
	m_pMessenger->SetValue( m_vectMessenger[m_nSelectedMessenger] );

	return;
}

// Method:	OnSelectAddrLoc
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnSelectAddrLoc( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	int nCountryId=0, nRegionId=0;
	if( nId == wxID_OBSERVER_CFG_COUNTRY || nId == wxID_OBSERVER_CFG_PROVINCE )
	{
		nCountryId = m_pCntCountry->GetSelection( );
		nRegionId = m_pCntProvince->GetSelection( );

	} else if( nId == wxID_OBSERVER_CFG_SITE_COUNTRY || nId == wxID_OBSERVER_CFG_SITE_PROVINCE )
	{
		nCountryId = m_pSiteCountry->GetSelection( );
		nRegionId = m_pSiteProvince->GetSelection( );
	}

	// by id
	if( nId == wxID_OBSERVER_CFG_COUNTRY )
	{
		m_pEarth->SetRegionsWidget( m_pCntProvince, nCountryId, 0 );
		m_pEarth->SetCitiesWidget( m_pCntCity, nCountryId, 0, 0 );
		SetCity( 0 );

	} else if( nId == wxID_OBSERVER_CFG_SITE_COUNTRY )
	{
		m_pEarth->SetRegionsWidget( m_pSiteProvince, nCountryId, 0 );
		m_pEarth->SetCitiesWidget( m_pSiteCity, nCountryId, 0, 0 );
		SetCity( 1 );

	} else if( nId == wxID_OBSERVER_CFG_PROVINCE )
	{
		m_pEarth->SetCitiesWidget( m_pCntCity, nCountryId, nRegionId, 0 );
		SetCity( 0 );

	} else if( nId == wxID_OBSERVER_CFG_SITE_PROVINCE )
	{
		m_pEarth->SetCitiesWidget( m_pSiteCity, nCountryId, nRegionId, 0 );
		SetCity( 1 );

	} else if( nId == wxID_OBSERVER_CFG_CITY )
	{
		SetCity( 0 );

	} else if( nId == wxID_OBSERVER_CFG_SITE_CITY )
	{
		SetCity( 1 );
	} 

	return;
}

// Method:	SetCity
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::SetCity( int nType )
{
	wxString strTmp;

	wxTextCtrl* pLatitude;
	wxTextCtrl* pLongitude;
	wxTextCtrl* pAltitude;
	wxChoice*	pCity;
	wxChoice*	pProvince;
	wxChoice*	pCountry;

	if( nType == 0 )
	{
		pCity = m_pCntCity;
		pProvince = m_pCntProvince;
		pCountry = m_pCntCountry;
		pLatitude = m_pCntLatitude;
		pLongitude = m_pCntLongitude;
		pAltitude = m_pCntAltitude;

	} else if( nType == 1 )
	{
		pCity = m_pSiteCity;
		pProvince = m_pSiteProvince;
		pCountry = m_pSiteCountry;
		pLatitude = m_pSiteLatitude;
		pLongitude = m_pSiteLongitude;
		pAltitude = m_pSiteAltitude;

	} else
		return;

	int nCountryId = pCountry->GetSelection( );
	int nRegionId = pProvince->GetSelection( );
	int nCityId = pCity->GetSelection( );

	if( nCountryId < 0 || nRegionId < 0 || nCityId < 0 ) return;

	// :: LATITUDE :: is NORTH to SOUTH 90 to -90 
	double lat = m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].lat;
	strTmp.Printf( wxT("%.6f"), lat );
	pLatitude->SetValue( strTmp );

	// :: LONGITUDE :: is WEST - EAST ... -180 to 180
	double lon = m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].lon;
	strTmp.Printf( wxT("%.6f"), lon );
	pLongitude->SetValue( strTmp );

	// :: ALTITUDE
	strTmp.Printf( wxT("%.4f"), m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].alt );
	pAltitude->SetValue( strTmp );
}

////////////////////////////////////////////////////////////////////
// Method:	OnEartMapLocation
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnEartMapLocation( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();

	wxTextCtrl* pLatitude;
	wxTextCtrl* pLongitude;
	wxTextCtrl* pAltitude;
	wxChoice*	pCity;
	wxChoice*	pProvince;
	wxChoice*	pCountry;
	// by type
	if( nId == wxID_OBSERVER_CFG_CONTACT_MAP )
	{
		pCity = m_pCntCity;
		pProvince = m_pCntProvince;
		pCountry = m_pCntCountry;
		pLatitude = m_pCntLatitude;
		pLongitude = m_pCntLongitude;
		pAltitude = m_pCntAltitude;

	} else if( nId == wxID_OBSERVER_CFG_SITE_MAP )
	{
		pCity = m_pSiteCity;
		pProvince = m_pSiteProvince;
		pCountry = m_pSiteCountry;
		pLatitude = m_pSiteLatitude;
		pLongitude = m_pSiteLongitude;
		pAltitude = m_pSiteAltitude;

	} else
		return;

	int nCountryId = pCountry->GetSelection( );
	int nRegionId = pProvince->GetSelection( );
	int nCityId = pCity->GetSelection( );

	CEarthmMapDlg* pMapDlg = new CEarthmMapDlg( this, wxT("Earth Map - Set Your Location") );
	pMapDlg->SetEarth( m_pEarth );
	pMapDlg->LoadData( this, m_pUnimapWorker ) ;
	pMapDlg->SetConfig( NULL, nCountryId, nRegionId, nCityId );
	// set over manually - latitude/longitude and altitude
	pMapDlg->SetCustomPositionFromString( pLatitude->GetLineText(0),
											pLongitude->GetLineText(0),
											pAltitude->GetLineText(0) );

	if( pMapDlg->ShowModal() == wxID_OK )
	{
		int nCountryId = pMapDlg->m_pCountry->GetSelection();
		int nRegionId = pMapDlg->m_pRegion->GetSelection();
		int nCityId = pMapDlg->m_pCity->GetSelection();

		pCountry->SetSelection( nCountryId );
		m_pEarth->SetRegionsWidget( pProvince, nCountryId, nRegionId );
		m_pEarth->SetCitiesWidget( pCity, nCountryId, nRegionId, nCityId );

		// also get latitude/longitude and altitude
		pLatitude->SetValue( pMapDlg->m_pLatitude->GetLineText(0) );
		pLongitude->SetValue( pMapDlg->m_pLongitude->GetLineText(0) );
		pAltitude->SetValue( pMapDlg->m_pAltitude->GetLineText(0) );

	}

	delete( pMapDlg );
}

////////////////////////////////////////////////////////////////////
// Method:	OnGeoIpLocation
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnGeoIpLocation( wxCommandEvent& pEvent )
{
	wxString strCountry;
	wxString strCity;
	double nLat=0.0, nLon=0.0;
	wxString strTmp;

	int nId = pEvent.GetId();

	wxTextCtrl* pLatitude;
	wxTextCtrl* pLongitude;
	wxTextCtrl* pAltitude;
	wxChoice*	pCity;
	wxChoice*	pProvince;
	wxChoice*	pCountry;
	// by type
	if( nId == wxID_OBSERVER_CFG_CONTACT_GEOIP )
	{
		pCity = m_pCntCity;
		pProvince = m_pCntProvince;
		pCountry = m_pCntCountry;
		pLatitude = m_pCntLatitude;
		pLongitude = m_pCntLongitude;
		pAltitude = m_pCntAltitude;

	} else if( nId == wxID_OBSERVER_CFG_SITE_GEOIP )
	{
		pCity = m_pSiteCity;
		pProvince = m_pSiteProvince;
		pCountry = m_pSiteCountry;
		pLatitude = m_pSiteLatitude;
		pLongitude = m_pSiteLongitude;
		pAltitude = m_pSiteAltitude;

	} else
		return;

	CGeoIp ip_locator;
	ip_locator.GetOwnIp( strCountry, strCity, nLat, nLon );

	// :: set latitude :: is NORTH to SOUTH 90 to -90 
	strTmp.Printf( wxT("%.6f"), nLat );
	pLatitude->SetValue( strTmp );

	// :: set longitude :: is WEST - EAST ... -180 to 180
	strTmp.Printf( wxT("%.6f"), nLon );
	pLongitude->SetValue( strTmp );

	// :: set country/city
	int bSolved = 0;
	int nRegionId=0, nCityId=0;
	int nCountryId = m_pEarth->FindCountryByName( strCountry );
	if( nCountryId >= 0 )
	{
		pCountry->SetSelection( nCountryId );
		if( m_pEarth->FindRegionCityByName( strCity, nCountryId, nRegionId, nCityId ) )	bSolved = 1;
	}
	// if not solved - find closest to coord
	if( !bSolved )
	{
		if( m_pEarth->FindClosestCity( nLat, nLon, 10.0, nCountryId, nRegionId, nCityId ) ) bSolved = 1;
	}
	// finally if found
	if( bSolved )
	{
		pCountry->SetSelection( nCountryId );
		m_pEarth->SetRegionsWidget( pProvince, nCountryId, nRegionId );
		m_pEarth->SetCitiesWidget( pCity, nCountryId, nRegionId, nCityId );

		strTmp.Printf( wxT("%.4f"), m_pEarth->m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[nCityId].alt );
		pAltitude->SetValue( strTmp );
	}
}

// Method:	OnSelectSite
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnSelectSite( wxCommandEvent& pEvent )
{
	char strTmp[255];

	StoreSelectedSite( );
	// get current selection
	m_nSelectSite = m_pSiteNameSelect->GetSelection();
	// set location fields
	SetLocByName( m_vectSite[m_nSelectSite]->m_strCountry, m_vectSite[m_nSelectSite]->m_strProvince, 
			m_vectSite[m_nSelectSite]->m_strCity, m_pSiteCountry, m_pSiteProvince, m_pSiteCity, 1 );

	// site notes
	m_pSiteNotes->SetValue( m_vectSite[m_nSelectSite]->m_strSiteNotes );

	return;
}

////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::StoreSelectedSite( )
{
	//debug
//	wxString strSiteName = m_pSiteNameSelect->GetLabelText();
//
//	if( m_nSelectSite == 0 && strSiteName.CmpNoCase( "Default" ) != 0 )
//		return;

	// :: store last site selected
//	strcpy( m_vectSite[m_nSelectSite].m_strSiteName, m_pSiteNameSelect->GetLabelText() );
	m_vectSite[m_nSelectSite]->m_strCity = m_pSiteCity->GetLabelText();
	m_vectSite[m_nSelectSite]->m_strProvince = m_pSiteProvince->GetLabelText();
	m_vectSite[m_nSelectSite]->m_strCountry = m_pSiteCountry->GetLabelText();
	m_pSiteLatitude->GetLineText(0).Trim(0).Trim(1).ToDouble( &(m_vectSite[m_nSelectSite]->m_nLatitude) );
	m_pSiteLongitude->GetLineText(0).Trim(0).Trim(1).ToDouble( &(m_vectSite[m_nSelectSite]->m_nLongitude) );
	m_pSiteAltitude->GetLineText(0).Trim(0).Trim(1).ToDouble( &(m_vectSite[m_nSelectSite]->m_nAltitude) );
	m_vectSite[m_nSelectSite]->m_strSiteNotes = m_pSiteNotes->GetLineText(0).Trim(0).Trim(1);

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnSitesNewUse
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnSitesAdd( wxCommandEvent& pEvent )
{
	CSimpleEntryAddDlg *pAddProfile = new CSimpleEntryAddDlg( this, wxT("Add a new site"), wxT("Site Name:") );
	if ( pAddProfile->ShowModal( ) == wxID_OK )
	{
		// check if entry
		wxString strProfileName = pAddProfile->m_pName->GetLineText(0);
		if( strProfileName != wxT("") )
		{
			// reallocate
			if( m_vectSite == NULL )
				m_vectSite = (CObserverSite**) calloc( 1, sizeof(CObserverSite*) );
			else
				m_vectSite = (CObserverSite**) _recalloc( m_vectSite, m_nSite+1, sizeof(CObserverSite*) );

			// add a site 
			m_vectSite[m_nSite] = new CObserverSite();
			m_vectSite[m_nSite]->m_strSiteName = strProfileName;
			m_nSite++;

			// gui
			StoreSelectedSite( );
			m_pSiteNameSelect->Append( strProfileName );
			m_pSiteNameSelect->SetSelection( m_pSiteNameSelect->GetCount()-1 );
			// get current selection
			m_nSelectSite = m_pSiteNameSelect->GetSelection();

			// set location fields
			SetLocByName( m_pCntCountry->GetLabelText(), 
							m_pCntProvince->GetLabelText(), 
							m_pCntCity->GetLabelText(), 
							m_pSiteCountry, m_pSiteProvince, m_pSiteCity, 1 );

		}
	}

	delete( pAddProfile );

	return;
}

////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnSiteDelete( wxCommandEvent& pEvent )
{
	// pop up general warning before delete
	int nRet = wxMessageBox( wxT("Delete Site?"), wxT("Confirm"), wxYES_NO, this );
	if( nRet != wxYES ) return;

	int nSiteId = m_pSiteNameSelect->GetSelection();
	if( nSiteId < 0 ) return;

	// call to delete site from vector
	delete( m_vectSite[nSiteId] );
	m_nSite--;
	for( int i=nSiteId; i<m_nSite; i++ ) m_vectSite[i] = m_vectSite[i+1];
	m_vectSite[m_nSite] = NULL;

	// also remove from selection
	m_pSiteNameSelect->Delete( nSiteId );

	// get next selected
	if( m_nSite == 0 )
		m_nSelectSite = -1;
	else if( nSiteId >= m_nSite )
		m_nSelectSite = nSiteId-1;
	else
		m_nSelectSite = nSiteId;
	// select or disable
	if( m_nSelectSite < 0 )
	{
		m_pSiteNameSelect->Disable();
		m_pSiteNotes->SetValue( wxT("") );

	} else
	{
		m_pSiteNameSelect->SetSelection(m_nSelectSite);
		// set location fields
		SetLocByName( m_vectSite[m_nSelectSite]->m_strCountry, m_vectSite[m_nSelectSite]->m_strProvince, 
				m_vectSite[m_nSelectSite]->m_strCity, m_pSiteCountry, m_pSiteProvince, m_pSiteCity, 1 );

		// site notes
		m_pSiteNotes->SetValue( m_vectSite[m_nSelectSite]->m_strSiteNotes );
	}
}

////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnSiteRename( wxCommandEvent& pEvent )
{
	int nSiteId = m_pSiteNameSelect->GetSelection();

	CSimpleEntryAddDlg dlg( this, wxT("Rename Site"), wxT("Site Name:") );
	dlg.m_pName->ChangeValue( m_pSiteNameSelect->GetStringSelection() );
	dlg.m_pName->SetSelection( -1, -1 );
	if( dlg.ShowModal( ) == wxID_OK )
	{
		m_vectSite[nSiteId]->m_strSiteName = dlg.m_pName->GetLineText(0);
		m_pSiteNameSelect->SetString( nSiteId, m_vectSite[nSiteId]->m_strSiteName );
		m_pSiteNameSelect->SetSelection(nSiteId);
	}
}

// Method:	OnSelectHardwareType
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnSelectHardwareType( wxCommandEvent& pEvent )
{
	// get current selection
	int nType = m_pHardwareNameType->GetSelection();
	// set gui for current selection
	SelectHardwareType( nType );
	SelectHardwareName( );

	return;
}

// Method:	ResetAllHardwareFields
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::ResetAllHardwareFields( )
{
	m_pHardwareEntry1->SetValue( wxT("") );
	m_pHardwareEntry2->SetValue( wxT("") );
	m_pHardwareEntry3->SetValue( wxT("") );
	m_pHardwareEntry4->SetValue( wxT("") );
	m_pHardwareSelect1->SetSelection( 0 );
}

// Method:	OnSelectHardwareName
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnSelectHardwareName( wxCommandEvent& pEvent )
{
	SelectHardwareName( );
}

// Method:	SelectHardwareName
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::SelectHardwareName( )
{
	wxString strTmp;
	// get current selection
	int nType = m_pHardwareNameType->GetSelection();
	int nName = m_pHardwareNameSelect->GetSelection();

	// populate fields with data for current hardware
	// 0 :: camera
	if( nType == HARDWARE_TYPE_CAMERA )
	{
		if( m_pHardware->m_vectCamera.size() > 0 )
		{
			// type
			m_pHardwareSelect1->SetSelection( m_pHardware->m_vectCamera[nName].type );
			// brad/manufacturer
			m_pHardwareEntry1->SetValue( m_pHardware->m_vectCamera[nName].brand );
			// sensor width
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectCamera[nName].sensor_width );
			m_pHardwareEntry2->SetValue( strTmp );
			// sensor height
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectCamera[nName].sensor_height );
			m_pHardwareEntry3->SetValue( strTmp );
			// sensor pixel size
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectCamera[nName].sensor_psize );
			m_pHardwareEntry4->SetValue( strTmp );

		} else
			ResetAllHardwareFields( );

	// 1 :: telescope
	} if( nType == HARDWARE_TYPE_TELESCOPE )
	{
		if( m_pHardware->m_vectTelescope.size() > 0 )
		{
			// brad/manufacturer
			m_pHardwareEntry1->SetValue( m_pHardware->m_vectTelescope[nName].brand );
			// focal lenght
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectTelescope[nName].focal_length );
			m_pHardwareEntry2->SetValue( strTmp );
			// clear aperure
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectTelescope[nName].aperture );
			m_pHardwareEntry3->SetValue( strTmp );
			// controller
			m_pHardwareEntry4->SetValue( m_pHardware->m_vectTelescope[nName].controller );

		} else
			ResetAllHardwareFields( );

	} else if( nType == HARDWARE_TYPE_CAMERA_LENS )
	{
		if( m_pHardware->m_vectCameraLens.size() > 0 )
		{
			// brad/manufacturer
			m_pHardwareEntry1->SetValue( m_pHardware->m_vectCameraLens[nName].brand );
			// focal lenght
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectCameraLens[nName].focal_length );
			m_pHardwareEntry2->SetValue( strTmp );
			// aperture
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectCameraLens[nName].aperture );
			m_pHardwareEntry3->SetValue( strTmp );
			// lens view angle
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectCameraLens[nName].view_angle );
			m_pHardwareEntry4->SetValue( strTmp );

		} else
			ResetAllHardwareFields( );

	} else if( nType == HARDWARE_TYPE_EYEPIECE )
	{
		if( m_pHardware->m_vectEyepiece.size() > 0 )
		{
			// brad/manufacturer
			m_pHardwareEntry1->SetValue( m_pHardware->m_vectEyepiece[nName].brand );
			// focal lenght
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectEyepiece[nName].focal_length );
			m_pHardwareEntry2->SetValue( strTmp );
			// elements
			strTmp.Printf( wxT("%d"), m_pHardware->m_vectEyepiece[nName].elements );
			m_pHardwareEntry3->SetValue( strTmp );
			// barrel
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectEyepiece[nName].barrel );
			m_pHardwareEntry4->SetValue( strTmp );

		} else
			ResetAllHardwareFields( );

	} else if( nType == HARDWARE_TYPE_BARLOW_LENS )
	{
		if( m_pHardware->m_vectBarlowLens.size() > 0 )
		{
			// brad/manufacturer
			m_pHardwareEntry1->SetValue( m_pHardware->m_vectBarlowLens[nName].brand );
			// magnification
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectBarlowLens[nName].magnification );
			m_pHardwareEntry2->SetValue( strTmp );
			// elements
			strTmp.Printf( wxT("%d"), m_pHardware->m_vectBarlowLens[nName].elements );
			m_pHardwareEntry3->SetValue( strTmp );
			// barrel
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectBarlowLens[nName].barrel );
			m_pHardwareEntry4->SetValue( strTmp );

		} else
			ResetAllHardwareFields( );

	} else if( nType == HARDWARE_TYPE_FOCAL_REDUCER )
	{
		if( m_pHardware->m_vectFocalReducer.size() > 0 )
		{
			// brad/manufacturer
			m_pHardwareEntry1->SetValue( m_pHardware->m_vectFocalReducer[nName].brand );
			// focal ratio
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectFocalReducer[nName].focal_ratio );
			m_pHardwareEntry2->SetValue( strTmp );
			// elements
			strTmp.Printf( wxT("%d"), m_pHardware->m_vectFocalReducer[nName].elements );
			m_pHardwareEntry3->SetValue( strTmp );
			// barrel
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectFocalReducer[nName].barrel );
			m_pHardwareEntry4->SetValue( strTmp );

		} else
			ResetAllHardwareFields( );

	} else if( nType == HARDWARE_TYPE_LMOUNT )
	{
		if( m_pHardware->m_vectLMount.size() > 0 )
		{
			// brad/manufacturer
			m_pHardwareEntry1->SetValue( m_pHardware->m_vectLMount[nName].brand );
			// controller
			m_pHardwareEntry2->SetValue( m_pHardware->m_vectLMount[nName].controller );
			// capacity
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectLMount[nName].capacity );
			m_pHardwareEntry3->SetValue( strTmp );
			// PEM
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectLMount[nName].pem );
			m_pHardwareEntry4->SetValue( strTmp );

		} else
			ResetAllHardwareFields( );

	} else if( nType == HARDWARE_TYPE_FOCUSER )
	{
		if( m_pHardware->m_vectFocuser.size() > 0 )
		{
			// brad/manufacturer
			m_pHardwareEntry1->SetValue( m_pHardware->m_vectFocuser[nName].brand );
			// controller
			m_pHardwareEntry2->SetValue( m_pHardware->m_vectFocuser[nName].controller );
			// drawtube
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectFocuser[nName].drawtube );
			m_pHardwareEntry3->SetValue( strTmp );
			// weight
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectFocuser[nName].weight );
			m_pHardwareEntry4->SetValue( strTmp );

		} else
			ResetAllHardwareFields( );

	} else if( nType == HARDWARE_TYPE_TFILTER )
	{
		if( m_pHardware->m_vectTFilter.size() > 0 )
		{
			// brad/manufacturer
			m_pHardwareEntry1->SetValue( m_pHardware->m_vectTFilter[nName].brand );
			// clear aperture
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectTFilter[nName].clear_aperture );
			m_pHardwareEntry3->SetValue( strTmp );
			// bandpass
			m_pHardwareEntry2->SetValue( m_pHardware->m_vectTFilter[nName].band_pass );
			// transmision
			strTmp.Printf( wxT("%.4f"), m_pHardware->m_vectTFilter[nName].transmission );
			m_pHardwareEntry4->SetValue( strTmp );

		} else
			ResetAllHardwareFields( );
	}
}

// Method:	SelectHardwareType
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::SelectHardwareType( int nType, int nSelect )
{
	int i=0;

	// 0 :: camera
	if( nType == HARDWARE_TYPE_CAMERA )
	{
		// set cameras
		m_pHardwareNameSelect->Clear();
		if( m_pHardware->m_vectCamera.size() > 0 )
		{
			m_pHardwareNameSelect->Enable();
			for( i=0; i<m_pHardware->m_vectCamera.size(); i++ ) 
				m_pHardwareNameSelect->Append( m_pHardware->GetCameraFullName( i ) );
			m_pHardwareNameSelect->SetSelection( nSelect );
		} else
			m_pHardwareNameSelect->Disable();

		// :: camera type
		m_pHardwareLabel1->Show();
		m_pHardwareSelect1->Show();
		m_pHardwareLabel1->SetLabel( wxT("Camera Type:") );
		m_pHardwareSelect1->SetSize( 100, -1 );
		m_pHardwareSelect1->Clear();
		for( i=0; i<g_nCameraTypes; i++ ) m_pHardwareSelect1->Append( g_vectCameraTypes[i].type_label );
		m_pHardwareSelect1->SetSelection( 0 );
		// :: Manufacturer
		m_pHardwareLabel2->SetLabel( wxT("Manufacturer:") );
		// :: Sensor Width
		m_pHardwareLabel3->SetLabel( wxT("Sensor Width:") );
		// :: Sensor Height
		m_pHardwareLabel4->SetLabel( wxT("Sensor Height:") );
		// :: Pixel Density
		m_pHardwareLabel5->SetLabel( wxT("Pixel Size:") );

	// 1 :: telescope
	} else if( nType == HARDWARE_TYPE_TELESCOPE )
	{
		// set telescopes
		m_pHardwareNameSelect->Clear();
		if( m_pHardware->m_vectTelescope.size() > 0 )
		{
			m_pHardwareNameSelect->Enable();
			for( i=0; i<m_pHardware->m_vectTelescope.size(); i++ ) 
				m_pHardwareNameSelect->Append( m_pHardware->GetTelescopeFullName( i ) );
			m_pHardwareNameSelect->SetSelection( nSelect );
		} else
			m_pHardwareNameSelect->Disable();

		// :: telescope type/Optical Design
		m_pHardwareLabel1->Show();
		m_pHardwareSelect1->Show();
		m_pHardwareLabel1->SetLabel( wxT("Optical Design:") );
		m_pHardwareSelect1->Clear();
		for( i=0; i<g_nTelescopeTypes; i++ ) m_pHardwareSelect1->Append( g_vectTelescopeTypes[i].type_label );
		m_pHardwareSelect1->SetSelection( 0 );
		// :: Manufacturer
		m_pHardwareLabel2->SetLabel( wxT("Manufacturer:") );
		// :: Focal Lenght
		m_pHardwareLabel3->SetLabel( wxT("Focal Length:") );
		// :: Clear Aperture
		m_pHardwareLabel4->SetLabel( wxT("Clear Aperture:") );
		// :: Controller
		m_pHardwareLabel5->SetLabel( wxT("Controller:") );


	// 2 :: Camera Lens
	} else if( nType == HARDWARE_TYPE_CAMERA_LENS )
	{
		// set cameras
		m_pHardwareNameSelect->Clear();
		if( m_pHardware->m_vectCameraLens.size() > 0 )
		{
			m_pHardwareNameSelect->Enable();
			for( i=0; i<m_pHardware->m_vectCameraLens.size(); i++ ) 
				m_pHardwareNameSelect->Append( m_pHardware->GetCameraLensFullName( i ) );
			m_pHardwareNameSelect->SetSelection( nSelect );
		} else
			m_pHardwareNameSelect->Disable();

		// :: camera lens type
		m_pHardwareLabel1->Show();
		m_pHardwareSelect1->Show();
		m_pHardwareLabel1->SetLabel( wxT("Lens Type:") );
		m_pHardwareSelect1->Clear();
		for( i=0; i<g_nCameraLensTypes; i++ ) m_pHardwareSelect1->Append( g_vectCameraLensTypes[i].type_label );
		m_pHardwareSelect1->SetSelection( 0 );
		m_pHardwareLabel3->SetLabel( wxT("Focal Length:") );
		m_pHardwareLabel4->SetLabel( wxT("Aperture:") );
		m_pHardwareLabel5->SetLabel( wxT("Diagonal Angle of view:") );

	} else if( nType == HARDWARE_TYPE_EYEPIECE )
	{
		// set eyepiece
		m_pHardwareNameSelect->Clear();
		if( m_pHardware->m_vectEyepiece.size() > 0 )
		{
			m_pHardwareNameSelect->Enable();
			for( i=0; i<m_pHardware->m_vectEyepiece.size(); i++ ) 
				m_pHardwareNameSelect->Append( m_pHardware->GetEyepieceFullName( i ) );
			m_pHardwareNameSelect->SetSelection( nSelect );
		} else
			m_pHardwareNameSelect->Disable();

		// :: Telescope eye-piece
		m_pHardwareLabel1->Show();
		m_pHardwareSelect1->Show();
		m_pHardwareLabel1->SetLabel( wxT("Eyepiece Type:") );
		m_pHardwareSelect1->Clear();
		for( i=0; i<g_nTelescopeEyepieceTypes; i++ ) m_pHardwareSelect1->Append( g_vectTelescopeEyepieceTypes[i].type_label );
		m_pHardwareSelect1->SetSelection( 0 );
		m_pHardwareLabel3->SetLabel( wxT("Focal Length:") );
		m_pHardwareLabel4->SetLabel( wxT("Elements:") );
		m_pHardwareLabel5->SetLabel( wxT("Barrel:") );

	} else if( nType == HARDWARE_TYPE_BARLOW_LENS )
	{
		// set barlow lens
		m_pHardwareNameSelect->Clear();
		if( m_pHardware->m_vectBarlowLens.size() > 0 )
		{
			m_pHardwareNameSelect->Enable();
			for( i=0; i<m_pHardware->m_vectBarlowLens.size(); i++ ) 
				m_pHardwareNameSelect->Append( m_pHardware->GetBarlowLensFullName( i ) );
			m_pHardwareNameSelect->SetSelection( nSelect );
		} else
			m_pHardwareNameSelect->Disable();

		// :: Telescope barlow lens
		m_pHardwareLabel1->Show();
		m_pHardwareSelect1->Show();
		m_pHardwareSelect1->Clear();
		m_pHardwareLabel1->Hide();
		m_pHardwareSelect1->Hide();
		m_pHardwareLabel3->SetLabel( wxT("Magnification:") );
		m_pHardwareLabel4->SetLabel( wxT("Elements:") );
		m_pHardwareLabel5->SetLabel( wxT("Barrel:") );

	} else if( nType == HARDWARE_TYPE_FOCAL_REDUCER )
	{
		// set barlow lens
		m_pHardwareNameSelect->Clear();
		if( m_pHardware->m_vectFocalReducer.size() > 0 )
		{
			m_pHardwareNameSelect->Enable();
			for( i=0; i<m_pHardware->m_vectFocalReducer.size(); i++ ) 
				m_pHardwareNameSelect->Append( m_pHardware->GetFocalReducerFullName( i ) );
			m_pHardwareNameSelect->SetSelection( nSelect );
		} else
			m_pHardwareNameSelect->Disable();

		// :: Telescope focal reducer
		m_pHardwareLabel1->Show();
		m_pHardwareSelect1->Show();
		m_pHardwareSelect1->Clear();
		m_pHardwareLabel1->Hide();
		m_pHardwareSelect1->Hide();
		m_pHardwareLabel3->SetLabel( wxT("Focal Factor:") );
		m_pHardwareLabel4->SetLabel( wxT("Elements:") );
		m_pHardwareLabel5->SetLabel( wxT("Barrel:") );

	} else if( nType == HARDWARE_TYPE_LMOUNT )
	{
		// set telescopes
		m_pHardwareNameSelect->Clear();
		if( m_pHardware->m_vectLMount.size() > 0 )
		{
			m_pHardwareNameSelect->Enable();
			for( i=0; i<m_pHardware->m_vectLMount.size(); i++ ) 
				m_pHardwareNameSelect->Append( m_pHardware->GetLMountFullName( i ) );
			m_pHardwareNameSelect->SetSelection( nSelect );
		} else
			m_pHardwareNameSelect->Disable();

		// :: Telescope mount
		m_pHardwareLabel1->Show();
		m_pHardwareSelect1->Show();
		m_pHardwareLabel1->SetLabel( wxT("Type:") );
		m_pHardwareSelect1->Clear();
		for( i=0; i<g_nTelescopeMountTypes; i++ ) m_pHardwareSelect1->Append( g_vectTelescopeMountTypes[i].type_label );
		m_pHardwareSelect1->SetSelection( 0 );
		m_pHardwareLabel3->SetLabel( wxT("Controller:") );
		m_pHardwareLabel4->SetLabel( wxT("Capacity(kg):") );
		m_pHardwareLabel5->SetLabel( wxT("Periodic Error Maximum:") );

	} else if( nType == HARDWARE_TYPE_FOCUSER )
	{
		// set telescopes
		m_pHardwareNameSelect->Clear();
		if( m_pHardware->m_vectFocuser.size() > 0 )
		{
			m_pHardwareNameSelect->Enable();
			for( i=0; i<m_pHardware->m_vectFocuser.size(); i++ ) 
				m_pHardwareNameSelect->Append( m_pHardware->GetFocuserFullName( i ) );
			m_pHardwareNameSelect->SetSelection( nSelect );
		} else
			m_pHardwareNameSelect->Disable();

		// :: Telescope focuser
		m_pHardwareLabel1->Show();
		m_pHardwareSelect1->Show();
		m_pHardwareLabel1->SetLabel( wxT("Type:") );
		m_pHardwareSelect1->Clear();
		for( i=0; i<g_nTelescopeFocuserTypes; i++ ) m_pHardwareSelect1->Append( g_vectTelescopeFocuserTypes[i].type_label );
		m_pHardwareSelect1->SetSelection( 0 );
		m_pHardwareLabel3->SetLabel( wxT("Controller:") );
		m_pHardwareLabel4->SetLabel( wxT("Drawtube(mm):") );
		m_pHardwareLabel5->SetLabel( wxT("Weight(g):") );

	} else if( nType == HARDWARE_TYPE_TFILTER )
	{
		// set telescopes
		m_pHardwareNameSelect->Clear();
		if( m_pHardware->m_vectTFilter.size() > 0 )
		{
			m_pHardwareNameSelect->Enable();
			for( i=0; i<m_pHardware->m_vectTFilter.size(); i++ ) 
				m_pHardwareNameSelect->Append( m_pHardware->GetTFilterFullName( i ) );
			m_pHardwareNameSelect->SetSelection( nSelect );
		} else
			m_pHardwareNameSelect->Disable();

		// :: telescope filters
		m_pHardwareLabel1->Show();
		m_pHardwareSelect1->Show();
		m_pHardwareLabel1->SetLabel( wxT("Type:") );
		m_pHardwareSelect1->Clear();
		for( i=0; i<g_nTelescopeFilterTypes; i++ ) m_pHardwareSelect1->Append( g_vectTelescopeFilterTypes[i].type_label );
		m_pHardwareSelect1->SetSelection( 0 );
		m_pHardwareLabel3->SetLabel( wxT("Clear Aperture(mm):") );
		m_pHardwareLabel4->SetLabel( wxT("Bandpass(nm):") );
		m_pHardwareLabel5->SetLabel( wxT("Transmission(%):") );
	}

	sizerHardwareTail->Layout();
	Update();
	Refresh( false );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnHardwareAdd
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnHardwareAdd( wxCommandEvent& pEvent )
{
	int nType = m_pHardwareNameType->GetSelection();

	// 0 :: camera
	if( nType == HARDWARE_TYPE_CAMERA )
	{
		CHardwareAddDlg *pAddCamera = new CHardwareAddDlg( this, wxT("Add Camera") );
		pAddCamera->SetData( m_pCamera, CAMERA_HARDWARE_MAIN_BODY );
		if( pAddCamera->ShowModal( ) == wxID_OK )
		{
			int nNewHard=0;
			// get camera name
			if( pAddCamera->m_nNameType == 0 && pAddCamera->m_nBrandType == 0 )
			{
				int nBrand = pAddCamera->GetBrandId();
				int nName = pAddCamera->GetNameId();
				if( m_pCamera->IsCameraDef( nBrand, nName ) )
					nNewHard = m_pHardware->AddCamera( m_pCamera->m_vectCameraBrandDef[nBrand].vectData[nName] );

			} else 
			{
				DefCamera cam;
				// brand 
				if( pAddCamera->m_nBrandType == 1 ) 
					cam.brand = pAddCamera->m_pBrandEntry->GetLineText(0);
				else
					cam.brand = pAddCamera->m_pBrandSelect->GetLabelText();
				// camera name
				if( pAddCamera->m_nNameType == 1 ) 
					cam.name = pAddCamera->m_pNameEntry->GetLineText(0);
				else
					cam.name = pAddCamera->m_pNameSelect->GetLabelText();
				// other init
				cam.sensor_width = 0.0;
				cam.sensor_height = 0.0;
				cam.sensor_psize = 0.0;
				// add hardware
				nNewHard = m_pHardware->AddCamera( cam );
			}
			// here we select the hardware type
			SelectHardwareType( nType, nNewHard );
			SelectHardwareName( );
		}
		delete( pAddCamera );

	// 1 :: telescope
	} else if( nType == HARDWARE_TYPE_TELESCOPE )
	{
		CHardwareAddDlg *pAddTelescope = new CHardwareAddDlg( this, wxT("Add Telescope") );
		pAddTelescope->SetData( m_pTelescope, TELESCOPE_HARDWARE_MAIN_SCOPE );
		if( pAddTelescope->ShowModal( ) == wxID_OK )
		{
			int nNewHard=0;
			// if a default
			if( pAddTelescope->m_nNameType == 0 && pAddTelescope->m_nBrandType == 0 )
			{
				int nBrand = pAddTelescope->GetBrandId();
				int nName = pAddTelescope->GetNameId();
				if( m_pTelescope->IsTelescopeDef( nBrand, nName ) )
					nNewHard = m_pHardware->AddTelescope( m_pTelescope->m_vectTelescopeBrandDef[nBrand].vectData[nName] );

			} else 
			{
				DefTelescope tel;
				// brand 
				if( pAddTelescope->m_nBrandType == 1 ) 
					tel.brand = pAddTelescope->m_pBrandEntry->GetLineText(0);
				else
					tel.brand = pAddTelescope->m_pBrandSelect->GetLabelText();
				// name
				if( pAddTelescope->m_nNameType == 1 ) 
					tel.name = pAddTelescope->m_pNameEntry->GetLineText(0);
				else
					tel.name = pAddTelescope->m_pNameSelect->GetLabelText();
				// other init
				tel.focal_length = 0.0;
				tel.aperture = 0.0;
				tel.controller = wxT( "" );
				// add hardware
				nNewHard = m_pHardware->AddTelescope( tel );
			}
			// here we select the hardware type
			SelectHardwareType( nType, nNewHard );
			SelectHardwareName( );
		}
		delete( pAddTelescope );

	} else if( nType == HARDWARE_TYPE_CAMERA_LENS )
	{
		CHardwareAddDlg *pAddCamLens = new CHardwareAddDlg( this, wxT("Add Camera Lens") );
		pAddCamLens->SetData( m_pCamera, CAMERA_HARDWARE_LENS );
		if( pAddCamLens->ShowModal( ) == wxID_OK )
		{
			int nNewHard=0;
			// get camera name
			if( pAddCamLens->m_nNameType == 0 && pAddCamLens->m_nBrandType == 0 )
			{
				int nBrand = pAddCamLens->GetBrandId();
				int nName = pAddCamLens->GetNameId();
				if( m_pCamera->IsCameraLensDef( nBrand, nName ) )
					nNewHard = m_pHardware->AddCameraLens( m_pCamera->m_vectCameraLensBrandDef[nBrand].vectData[nName] );

			} else 
			{
				DefCameraLens cam_lens;
				// brand 
				if( pAddCamLens->m_nBrandType == 1 ) 
					cam_lens.brand = pAddCamLens->m_pBrandEntry->GetLineText(0);
				else
					cam_lens.brand = pAddCamLens->m_pBrandSelect->GetLabelText();
				// camera name
				if( pAddCamLens->m_nNameType == 1 ) 
					cam_lens.name = pAddCamLens->m_pNameEntry->GetLineText(0);
				else
					cam_lens.name = pAddCamLens->m_pNameSelect->GetLabelText();
				// other init
				cam_lens.focal_length = 0.0;
				cam_lens.aperture = 0.0;
				cam_lens.view_angle = 0.0;
				// add hardware
				nNewHard = m_pHardware->AddCameraLens( cam_lens );
			}
			// here we select the hardware type
			SelectHardwareType( nType, nNewHard );
			SelectHardwareName( );
		}
		delete( pAddCamLens );

	} else if( nType == HARDWARE_TYPE_EYEPIECE )
	{
		CHardwareAddDlg *pAddEyepiece = new CHardwareAddDlg( this, wxT("Add Eyepiece") );
		pAddEyepiece->SetData( m_pTelescope, TELESCOPE_HARDWARE_EYEPIECE );
		if( pAddEyepiece->ShowModal( ) == wxID_OK )
		{
			int nNewHard=0;
			// if a default
			if( pAddEyepiece->m_nNameType == 0 && pAddEyepiece->m_nBrandType == 0 )
			{
				int nBrand = pAddEyepiece->GetBrandId();
				int nName = pAddEyepiece->GetNameId();
				nNewHard = m_pHardware->AddEyepiece( m_pTelescope->m_vectEyepieceBrandDef[nBrand].vectData[nName] );

			} else 
			{
				DefEyepiece eyepiece;
				// brand 
				if( pAddEyepiece->m_nBrandType == 1 ) 
					eyepiece.brand = pAddEyepiece->m_pBrandEntry->GetLineText(0);
				else
					eyepiece.brand = pAddEyepiece->m_pBrandSelect->GetLabelText();
				// name
				if( pAddEyepiece->m_nNameType == 1 ) 
					eyepiece.name = pAddEyepiece->m_pNameEntry->GetLineText(0);
				else
					eyepiece.name = pAddEyepiece->m_pNameSelect->GetLabelText();
				// other init
				eyepiece.focal_length = 0.0;
				eyepiece.elements = 0;
				eyepiece.barrel = 0.0;
				// add hardware
				nNewHard = m_pHardware->AddEyepiece( eyepiece );
			}
			// here we select the hardware type
			SelectHardwareType( nType, nNewHard );
			SelectHardwareName( );
		}
		delete( pAddEyepiece );

	} else if( nType == HARDWARE_TYPE_BARLOW_LENS )
	{
		CHardwareAddDlg *pAddBarlowLens = new CHardwareAddDlg( this, wxT("Add Barlow Lens") );
		pAddBarlowLens->SetData( m_pTelescope, TELESCOPE_HARDWARE_BARLOW_LENS );
		if( pAddBarlowLens->ShowModal( ) == wxID_OK )
		{
			int nNewHard=0;
			// if a default
			if( pAddBarlowLens->m_nNameType == 0 && pAddBarlowLens->m_nBrandType == 0 )
			{
				int nBrand = pAddBarlowLens->GetBrandId();
				int nName = pAddBarlowLens->GetNameId();
				nNewHard = m_pHardware->AddBarlowLens( m_pTelescope->m_vectBarlowLensBrandDef[nBrand].vectData[nName] );

			} else 
			{
				DefBarlowLens barlow_lens;
				// brand 
				if( pAddBarlowLens->m_nBrandType == 1 ) 
					barlow_lens.brand = pAddBarlowLens->m_pBrandEntry->GetLineText(0);
				else
					barlow_lens.brand = pAddBarlowLens->m_pBrandSelect->GetLabelText();
				// name
				if( pAddBarlowLens->m_nNameType == 1 ) 
					barlow_lens.name = pAddBarlowLens->m_pNameEntry->GetLineText(0);
				else
					barlow_lens.name = pAddBarlowLens->m_pNameSelect->GetLabelText();
				// other init
				barlow_lens.magnification = 0.0;
				barlow_lens.elements = 0;
				barlow_lens.barrel = 0;
				// add hardware
				nNewHard = m_pHardware->AddBarlowLens( barlow_lens );
			}
			// here we select the hardware type
			SelectHardwareType( nType, nNewHard );
			SelectHardwareName( );
		}
		delete( pAddBarlowLens );

	} else if( nType == HARDWARE_TYPE_FOCAL_REDUCER )
	{
		CHardwareAddDlg *pAddFocalReducer = new CHardwareAddDlg( this, wxT("Add Focal Reducer") );
		pAddFocalReducer->SetData( m_pTelescope, TELESCOPE_HARDWARE_FOCAL_REDUCER );
		if( pAddFocalReducer->ShowModal( ) == wxID_OK )
		{
			int nNewHard=0;
			// if a default
			if( pAddFocalReducer->m_nNameType == 0 && pAddFocalReducer->m_nBrandType == 0 )
			{
				int nBrand = pAddFocalReducer->GetBrandId();
				int nName = pAddFocalReducer->GetNameId();
				nNewHard = m_pHardware->AddFocalReducer( m_pTelescope->m_vectFocalReducerBrandDef[nBrand].vectData[nName] );

			} else 
			{
				DefFocalReducer focal_reducer;
				// brand 
				if( pAddFocalReducer->m_nBrandType == 1 ) 
					focal_reducer.brand = pAddFocalReducer->m_pBrandEntry->GetLineText(0);
				else
					focal_reducer.brand = pAddFocalReducer->m_pBrandSelect->GetLabelText();
				// name
				if( pAddFocalReducer->m_nNameType == 1 ) 
					focal_reducer.name = pAddFocalReducer->m_pNameEntry->GetLineText(0);
				else
					focal_reducer.name = pAddFocalReducer->m_pNameSelect->GetLabelText();
				// other init
				focal_reducer.focal_ratio = 0.0;
				focal_reducer.elements = 0;
				focal_reducer.barrel = 0;
				// add hardware
				nNewHard = m_pHardware->AddFocalReducer( focal_reducer );
			}
			// here we select the hardware type
			SelectHardwareType( nType, nNewHard );
			SelectHardwareName( );
		}
		delete( pAddFocalReducer );

	} else if( nType == HARDWARE_TYPE_LMOUNT )
	{
		CHardwareAddDlg *pAddLMount = new CHardwareAddDlg( this, wxT("Add Telescope Mount") );
		pAddLMount->SetData( m_pTelescope, TELESCOPE_HARDWARE_LMOUNT );
		if( pAddLMount->ShowModal( ) == wxID_OK )
		{
			int nNewHard=0;
			// if a default
			if( pAddLMount->m_nNameType == 0 && pAddLMount->m_nBrandType == 0 )
			{
				int nBrand = pAddLMount->GetBrandId();
				int nName = pAddLMount->GetNameId();
				nNewHard = m_pHardware->AddLMount( m_pTelescope->m_vectLMountBrandDef[nBrand].vectData[nName] );

			} else 
			{
				DefLMount mount;
				// brand 
				if( pAddLMount->m_nBrandType == 1 ) 
					mount.brand = pAddLMount->m_pBrandEntry->GetLineText(0);
				else
					mount.brand = pAddLMount->m_pBrandSelect->GetLabelText();
				// name
				if( pAddLMount->m_nNameType == 1 ) 
					mount.name = pAddLMount->m_pNameEntry->GetLineText(0);
				else
					mount.name = pAddLMount->m_pNameSelect->GetLabelText();
				// other init
				mount.capacity = 0.0;
				mount.pem = 0;
				mount.controller = wxT( "" );
				// add hardware
				nNewHard = m_pHardware->AddLMount( mount );
			}
			// here we select the hardware type
			SelectHardwareType( nType, nNewHard );
			SelectHardwareName( );
		}
		delete( pAddLMount );

	} else if( nType == HARDWARE_TYPE_FOCUSER )
	{
		CHardwareAddDlg *pAddFocuser = new CHardwareAddDlg( this, wxT("Add Focuser") );
		pAddFocuser->SetData( m_pTelescope, TELESCOPE_HARDWARE_FOCUSER );
		if( pAddFocuser->ShowModal( ) == wxID_OK )
		{
			int nNewHard=0;
			// if a default
			if( pAddFocuser->m_nNameType == 0 && pAddFocuser->m_nBrandType == 0 )
			{
				int nBrand = pAddFocuser->GetBrandId();
				int nName = pAddFocuser->GetNameId();
				nNewHard = m_pHardware->AddFocuser( m_pTelescope->m_vectFocuserBrandDef[nBrand].vectData[nName] );

			} else 
			{
				DefFocuser focuser;
				// brand 
				if( pAddFocuser->m_nBrandType == 1 ) 
					focuser.brand = pAddFocuser->m_pBrandEntry->GetLineText(0);
				else
					focuser.brand = pAddFocuser->m_pBrandSelect->GetLabelText();
				// name
				if( pAddFocuser->m_nNameType == 1 ) 
					focuser.name = pAddFocuser->m_pNameEntry->GetLineText(0);
				else
					focuser.name = pAddFocuser->m_pNameSelect->GetLabelText();
				// other init
				focuser.drawtube = 0.0;
				focuser.weight = 0;
				focuser.controller = wxT("");
				// add hardware
				nNewHard = m_pHardware->AddFocuser( focuser );
			}
			// here we select the hardware type
			SelectHardwareType( nType, nNewHard );
			SelectHardwareName( );
		}
		delete( pAddFocuser );

	} else if( nType == HARDWARE_TYPE_TFILTER )
	{
		CHardwareAddDlg *pAddFilter = new CHardwareAddDlg( this, wxT("Add Filter") );
		pAddFilter->SetData( m_pTelescope, TELESCOPE_HARDWARE_TFILTER );
		if( pAddFilter->ShowModal( ) == wxID_OK )
		{
			int nNewHard=0;
			// if a default
			if( pAddFilter->m_nNameType == 0 && pAddFilter->m_nBrandType == 0 )
			{
				int nBrand = pAddFilter->GetBrandId();
				int nName = pAddFilter->GetNameId();
				nNewHard = m_pHardware->AddTFilter( m_pTelescope->m_vectTFilterBrandDef[nBrand].vectData[nName] );

			} else 
			{
				DefTFilter filter;
				// brand 
				if( pAddFilter->m_nBrandType == 1 ) 
					filter.brand = pAddFilter->m_pBrandEntry->GetLineText(0);
				else
					filter.brand = pAddFilter->m_pBrandSelect->GetLabelText();
				// name
				if( pAddFilter->m_nNameType == 1 ) 
					filter.name = pAddFilter->m_pNameEntry->GetLineText(0);
				else
					filter.name = pAddFilter->m_pNameSelect->GetLabelText();
				// other init
				filter.clear_aperture = 0.0;
				filter.transmission = 0;
				filter.band_pass = wxT("");
				// add hardware
				nNewHard = m_pHardware->AddTFilter( filter );
			}
			// here we select the hardware type
			SelectHardwareType( nType, nNewHard );
			SelectHardwareName( );
		}
		delete( pAddFilter );
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnHardwareDelete( wxCommandEvent& pEvent )
{
	int nType = m_pHardwareNameType->GetSelection();
	int nHardId = m_pHardwareNameSelect->GetSelection();
	int nNewId = -1;

	// pop up general warning before delete
	int nRet = wxMessageBox( wxT("Delete Hardware?"), wxT("Confirm"), wxYES_NO, this );
	if( nRet != wxYES ) return;

	// delete by hardware type
	if( nType == HARDWARE_TYPE_CAMERA )
		nNewId = m_pHardware->DeleteCamera( nHardId );
	else if( nType == HARDWARE_TYPE_CAMERA_LENS )
		nNewId = m_pHardware->DeleteCameraLens( nHardId );
	else if( nType == HARDWARE_TYPE_TELESCOPE )
		nNewId = m_pHardware->DeleteTelescope( nHardId );
	else if( nType == HARDWARE_TYPE_EYEPIECE )
		nNewId = m_pHardware->DeleteEyepiece( nHardId );
	else if( nType == HARDWARE_TYPE_BARLOW_LENS )
		nNewId = m_pHardware->DeleteBarlowLens( nHardId );
	else if( nType == HARDWARE_TYPE_FOCAL_REDUCER )
		nNewId = m_pHardware->DeleteFocalReducer( nHardId );
	else if( nType == HARDWARE_TYPE_LMOUNT )
		nNewId = m_pHardware->DeleteLMount( nHardId );
	else if( nType == HARDWARE_TYPE_FOCUSER )
		nNewId = m_pHardware->DeleteFocuser( nHardId );
	else if( nType == HARDWARE_TYPE_TFILTER )
		nNewId = m_pHardware->DeleteTFilter( nHardId );

	// now delete from control
	m_pHardwareNameSelect->Delete( nHardId );
//	nHardId--;
	if( nNewId < 0 ) 
		m_pHardwareNameSelect->Disable();
	else
	{
		m_pHardwareNameSelect->SetSelection( nNewId );
		SelectHardwareName();
	}
}

// Method:	OnSelectSetupName
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnSelectSetupName( wxCommandEvent& pEvent )
{
	StoreSelectedSetup( );
	m_nSelectedSetup = m_pSetupNameSelect->GetSelection();
	GetSetupSelected( m_nSelectedSetup );
}

////////////////////////////////////////////////////////////////////
// Method:	OnSetupAdd
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnSetupAdd( wxCommandEvent& pEvent )
{
	CSimpleEntryAddDlg *pAddSetup = new CSimpleEntryAddDlg( this, wxT("Add Setup"), wxT("Setup Name:") );

	if( pAddSetup->ShowModal( ) == wxID_OK )
	{
		DefHarwareSetup setup;

		setup.name = pAddSetup->m_pName->GetLineText(0);
		setup.type = 0;
		setup.camera = 0;
		setup.telescope = 0;
		setup.mount = 0;
		setup.filter = 0;
		setup.focuser = 0;

		// add hardware
		int nSetup = m_pHardware->AddSetup( setup );
		// add to gui
		m_pSetupNameSelect->Append( setup.name );
		m_pSetupNameSelect->Enable();
		m_pSetupNameSelect->SetSelection( nSetup );
		// set field of setup
		StoreSelectedSetup( );
		m_nSelectedSetup = nSetup;
		GetSetupSelected( nSetup );
	}
	delete( pAddSetup );
}

////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnSetupDelete( wxCommandEvent& pEvent )
{
	// pop up general warning before delete
	int nRet = wxMessageBox( wxT("Delete Setup?"), wxT("Confirm"), wxYES_NO, this );
	if( nRet != wxYES ) return;

	int nSetupId = m_pSetupNameSelect->GetSelection();
	if( nSetupId < 0 ) return;

	// call to delete site from vector
	int nNewId = m_pHardware->DeleteSetup( nSetupId );
	m_pSetupNameSelect->Delete( nSetupId );
	if( nNewId < 0 ) 
		m_pSetupNameSelect->Disable();
	else
		m_pSetupNameSelect->SetSelection(nNewId);

	m_nSelectedSetup = m_pSetupNameSelect->GetSelection();
	GetSetupSelected( m_nSelectedSetup );
}

////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnSetupRename( wxCommandEvent& pEvent )
{
	int nSetupId = m_pSetupNameSelect->GetSelection();

	CSimpleEntryAddDlg dlg( this, wxT("Rename Setup"), wxT("Setup Name:") );
	dlg.m_pName->ChangeValue( m_pSetupNameSelect->GetStringSelection() );
	dlg.m_pName->SetSelection( -1, -1 );
	if( dlg.ShowModal( ) == wxID_OK )
	{
		m_pHardware->m_vectSetup[nSetupId].name = dlg.m_pName->GetLineText(0);
		m_pSetupNameSelect->SetString( nSetupId, m_pHardware->m_vectSetup[nSetupId].name );
		m_pSetupNameSelect->SetSelection(nSetupId);
	}
}

////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::GetSetupSelected( int nSetup )
{
	int i=0;
	// if  no setup set then get current selected
	if( nSetup < 0 ) nSetup = m_pSetupNameSelect->GetSelection();
	if( nSetup < 0 ) 
	{
		m_pSetupType->SetSelection(-1);
		m_pSetupType->Disable();
		m_pSetupTelescope->Clear();
		m_pSetupTelescope->Disable();
		m_pSetupCamera->Clear();
		m_pSetupCamera->Disable();
		m_pSetupMount->Clear();
		m_pSetupMount->Disable();
		m_pSetupFilter->Clear();
		m_pSetupFilter->Disable();
		m_pSetupFocuser->Clear();
		m_pSetupFocuser->Disable();
		return;
	}

	// enable all
	m_pSetupType->Enable();
	m_pSetupTelescope->Enable();
	m_pSetupCamera->Enable();
	m_pSetupMount->Enable();
	m_pSetupFilter->Enable();
	m_pSetupFocuser->Enable();

	// set setup type
	m_pSetupType->SetSelection( m_pHardware->m_vectSetup[nSetup].type );

	///////////////
	// populate telescopes
	m_pSetupTelescope->Clear();
	m_pSetupTelescope->Append( wxT("None") );
	for( i=0; i<m_pHardware->m_vectTelescope.size(); i++ ) m_pSetupTelescope->Append( m_pHardware->GetTelescopeFullName( i ) );
	m_pSetupTelescope->SetSelection( m_pHardware->m_vectSetup[nSetup].telescope );

	///////////////
	// populate cameras
	m_pSetupCamera->Clear();
	m_pSetupCamera->Append( wxT("None") );
	for( i=0; i<m_pHardware->m_vectCamera.size(); i++ ) m_pSetupCamera->Append( m_pHardware->GetCameraFullName( i ) );
	m_pSetupCamera->SetSelection( m_pHardware->m_vectSetup[nSetup].camera );

	///////////////
	// populate mounts
	m_pSetupMount->Clear();
	m_pSetupMount->Append( wxT("None") );
	for( i=0; i<m_pHardware->m_vectLMount.size(); i++ ) m_pSetupMount->Append( m_pHardware->GetLMountFullName( i ) );
	m_pSetupMount->SetSelection( m_pHardware->m_vectSetup[nSetup].mount );

	///////////////
	// populate filter
	m_pSetupFilter->Clear();
	m_pSetupFilter->Append( wxT("None") );
	for( i=0; i<m_pHardware->m_vectTFilter.size(); i++ ) m_pSetupFilter->Append( m_pHardware->GetTFilterFullName( i ) );
	m_pSetupFilter->SetSelection( m_pHardware->m_vectSetup[nSetup].filter );

	///////////////
	// populate focuser
	m_pSetupFocuser->Clear();
	m_pSetupFocuser->Append( wxT("None") );
	for( i=0; i<m_pHardware->m_vectFocuser.size(); i++ ) m_pSetupFocuser->Append( m_pHardware->GetFocuserFullName( i ) );
	m_pSetupFocuser->SetSelection( m_pHardware->m_vectSetup[nSetup].focuser );

}

////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::StoreSelectedSetup( )
{
	if( m_nSelectedSetup < 0 || m_pHardware->m_vectSetup.size() <= 0 ) return;

	m_pHardware->m_vectSetup[m_nSelectedSetup].type = m_pSetupType->GetSelection();
	m_pHardware->m_vectSetup[m_nSelectedSetup].telescope = m_pSetupTelescope->GetSelection();
	m_pHardware->m_vectSetup[m_nSelectedSetup].mount = m_pSetupMount->GetSelection();
	m_pHardware->m_vectSetup[m_nSelectedSetup].camera = m_pSetupCamera->GetSelection();
	m_pHardware->m_vectSetup[m_nSelectedSetup].filter = m_pSetupFilter->GetSelection();
	m_pHardware->m_vectSetup[m_nSelectedSetup].focuser = m_pSetupFocuser->GetSelection();
}

////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::GetDefaults( )
{
	int i=0;

	//////////////////
	// SITES
	if( m_nSite > 0 )
	{
		m_pDefaultSite->Enable();
		m_pDefaultSite->Clear();
		m_pDefaultSite->Freeze();
		// set local vector 
		for( i=0; i<m_nSite; i++ )
		{
			// append name to select
			m_pDefaultSite->Append( m_vectSite[i]->m_strSiteName );
		}
		m_pDefaultSite->SetSelection( m_pObserver->m_nDefaultSite );
		m_pDefaultSite->Thaw();

	} else
	{
		m_pDefaultSite->Clear();
		m_pDefaultSite->Disable();
	}

	/////////////
	// SETUPS
	if( m_pHardware->m_vectSetup.size() > 0 )
	{
		m_pDefaultSetup->Enable();
		m_pDefaultSetup->Clear();
		m_pDefaultSetup->Freeze();
		for( i=0; i<m_pHardware->m_vectSetup.size(); i++ )
			m_pDefaultSetup->Append( m_pHardware->m_vectSetup[i].name );
		m_pDefaultSetup->SetSelection( m_pObserver->m_nDefaultSetup );
		m_pDefaultSetup->Thaw();

	} else
	{
		m_pDefaultSetup->Clear();
		m_pDefaultSetup->Disable();
	}

	///////////////
	// populate telescopes
	m_pDefaultTelescope->Clear();
	for( i=0; i<m_pHardware->m_vectTelescope.size(); i++ ) m_pDefaultTelescope->Append( m_pHardware->GetTelescopeFullName( i ) );
	m_pDefaultTelescope->SetSelection( m_pObserver->m_nDefaultTelescope );

	///////////////
	// populate cameras
	m_pDefaultCamera->Clear();
	for( i=0; i<m_pHardware->m_vectCamera.size(); i++ ) m_pDefaultCamera->Append( m_pHardware->GetCameraFullName( i ) );
	m_pDefaultCamera->SetSelection( m_pObserver->m_nDefaultCamera );

	///////////////
	// populate mounts
	m_pDefaultMount->Clear();
	for( i=0; i<m_pHardware->m_vectLMount.size(); i++ ) m_pDefaultMount->Append( m_pHardware->GetLMountFullName( i ) );
	m_pDefaultMount->SetSelection( m_pObserver->m_nDefaultMount );

	///////////////
	// populate focuser
	m_pDefaultFocuser->Clear();
	for( i=0; i<m_pHardware->m_vectFocuser.size(); i++ ) m_pDefaultFocuser->Append( m_pHardware->GetFocuserFullName( i ) );
	m_pDefaultFocuser->SetSelection( m_pObserver->m_nDefaultFocuser );
}

////////////////////////////////////////////////////////////////////
// Method:	OnValidate
// Class:	CObserverCfgDlg
// Purpose:	validate my data on ok
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CObserverCfgDlg::OnValidate( wxCommandEvent& pEvent )
{
	/////////////////////////////////
	// first(or last?) we do final data population
	// :: store last selected IM type
	m_vectMessenger[m_nSelectedMessenger] = m_pMessenger->GetLineText(0).Trim(0).Trim(1);
	// :: store last site selected
	StoreSelectedSite( );
	// :: store last selected setup
	StoreSelectedSetup( );

/*
	wxString strMsg = "";
	wxString strTmp = "";
	wxRegEx reName( "^[a-zA-Z0-9_\\ \\+\\-\\.\\B[:punct:]]+$" );
	wxRegEx reStar( "[*,]+" );
	int bErrorFound = 0;
	long nTmp = 0;
	int bFoundNumber = 0;

	// common checks
	// check hardware name
	strTmp = m_pDeviceNameEntry->GetLineText(0);
	if( !reName.Matches( strTmp ) || reStar.Matches( strTmp ) )
	{
		if( strTmp.IsEmpty() )
			strMsg += "ERROR :: please enter hardware name.\n";
		else
			strMsg += "ERROR :: illegal characters found in software name.\n";
		bErrorFound = 1;
	}
	// check on delay
	strTmp = m_pOnDelayEntry->GetLineText(0);
	bFoundNumber = strTmp.ToLong( &nTmp );
	if( !bFoundNumber || nTmp < 0 )
	{
		if( strTmp.IsEmpty() )
			strMsg += "ERROR :: please enter ON delay.\n";
		else
			strMsg += "ERROR :: ON delay should be a positive integer.\n";
		bErrorFound = 1;
	}
	// check off delay
	strTmp = m_pOffDelayEntry->GetLineText(0);
	bFoundNumber = strTmp.ToLong( &nTmp );
	if( !bFoundNumber || nTmp < 0 )
	{
		if( strTmp.IsEmpty() )
			strMsg += "ERROR :: please enter OFF delay.\n";
		else
			strMsg += "ERROR :: OFF delay should be a positive integer.\n";
		bErrorFound = 1;
	}

	//////////////////////////////////////////////
	// now, in the end, check to see if there was an error
	if( bErrorFound == 1 )
	{
		wxMessageBox( strMsg, wxT("Data Error!"), wxOK|wxICON_ERROR );
	} else
		event.Skip();
*/
	pEvent.Skip();
	return;
}
