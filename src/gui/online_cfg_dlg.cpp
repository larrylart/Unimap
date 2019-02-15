////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// wx includes
#include "wx/notebook.h"
#include <wx/msgdlg.h>

// local headers
#include "../util/func.h"
#include "../online/unimap_online.h"
#include "../weather/meteo_blue.h"

// main header
#include "online_cfg_dlg.h"

//extern DefOnlineAccounts g_vectServiceAccounts[];

// COnlineCfgDlg EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( COnlineCfgDlg, wxDialog )
	// account testing and creation
	EVT_BUTTON( wxID_ONLINE_CFG_TEST_ACCOUNT, COnlineCfgDlg::OnCreateTestAccount )
	EVT_BUTTON( wxID_ONLINE_CFG_CREATE_ACCOUNT, COnlineCfgDlg::OnCreateTestAccount )
	// other services
	EVT_CHOICE( wxID_ONLINE_SERVICE_NAME, COnlineCfgDlg::OnSelectService )
	EVT_BUTTON( wxID_ONLINE_CFG_TEST_SERVICE, COnlineCfgDlg::OnTestService )
	EVT_BUTTON( wxID_ONLINE_CFG_REGISTER_SERVICE, COnlineCfgDlg::OnRegisterService )
	// validate observer data on ok
	EVT_BUTTON( wxID_OK, COnlineCfgDlg::OnValidate )
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
COnlineCfgDlg::COnlineCfgDlg(wxWindow *parent) : wxDialog(parent, -1,
                          wxT("Unimap Online Setup"), wxDefaultPosition, wxSize( 760, 440 ),
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	int i=0;
	m_nServiceId = 0;
	// empty fields
	for( i=0; i<10; i++ )
	{
		m_vectServiceUsername[i] = wxT("");
		strcpy( m_vectServicePassword[i], "" );
	}

	// main dialog sizer
	wxFlexGridSizer *pTopSizer = new wxFlexGridSizer( 2, 1, 5, 5 );

	// building configure as a note book
	wxNotebook* pNotebook = new wxNotebook( this, -1, wxPoint(-1,-1), wxSize( 300, 240 ) ); 
	// PROFILE PANEL
	wxPanel* pPanelProfile = new wxPanel( pNotebook );
	// OTHER PANEL
	pPanelOther = new wxPanel( pNotebook );

	////////////
	// PROFILE SIZER
	wxFlexGridSizer* sizerProfile = new wxFlexGridSizer( 3, 2, 5, 5 );
	sizerProfile->AddGrowableCol( 1 );
	sizerProfile->AddGrowableCol( 0 );
	wxBoxSizer* sizerProfilePage = new wxBoxSizer( wxHORIZONTAL );
	sizerProfilePage->Add( sizerProfile, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	////////////
	// OTHER SIZER
	wxFlexGridSizer* sizerOther = new wxFlexGridSizer( 3, 2, 5, 5 );
	sizerOther->AddGrowableCol( 1 );
	sizerOther->AddGrowableCol( 0 );
	wxFlexGridSizer* sizerOtherMain = new wxFlexGridSizer( 3, 1, 5, 5 );
	sizerOtherMain->AddGrowableRow( 0 );
	sizerOtherMain->AddGrowableCol( 0 );
	sizerOtherMain->Add( sizerOther, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	wxBoxSizer* sizerOtherPage = new wxBoxSizer( wxHORIZONTAL );
	sizerOtherPage->Add( sizerOtherMain, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	/////////////////////////
	// GUI OBJECTS :: PROFILE
	m_pUsername = new wxTextCtrl( pPanelProfile, -1, wxT(""), wxDefaultPosition, wxSize(100,-1) );
	m_pPassword = new wxTextCtrl( pPanelProfile, -1, wxT(""), wxDefaultPosition, wxSize(100,-1), wxTE_PASSWORD );
	m_pPasswordConfirm = new wxTextCtrl( pPanelProfile, -1, wxT(""), wxDefaultPosition, wxSize(100,-1), wxTE_PASSWORD );
	m_pEmail = new wxTextCtrl( pPanelProfile, -1, wxT(""), wxDefaultPosition, wxSize(150,-1) );
	m_pNickname = new wxTextCtrl( pPanelProfile, -1, wxT(""), wxDefaultPosition, wxSize(100,-1) );
	m_pTestAccount = new wxButton( pPanelProfile, wxID_ONLINE_CFG_TEST_ACCOUNT, wxT("Test Account"), wxPoint(-1, -1), wxSize(-1,-1) );
	m_pCreateAccount = new wxButton( pPanelProfile, wxID_ONLINE_CFG_CREATE_ACCOUNT, wxT("Create Account"), wxPoint(-1, -1), wxSize(-1,-1) );

	///////////////////////
	// GUI :: OTHERS
	m_pServiceName = new wxChoice( pPanelOther, wxID_ONLINE_SERVICE_NAME,
										wxDefaultPosition, wxSize(-1,-1), 0, NULL,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pServiceName->SetSelection(0);
	m_pServiceUsername = new wxTextCtrl( pPanelOther, -1, wxT(""), wxDefaultPosition, wxSize(100,-1) );
	m_pServicePassword = new wxTextCtrl( pPanelOther, -1, wxT(""), wxDefaultPosition, wxSize(100,-1), wxTE_PASSWORD );
	m_pTestService = new wxButton( pPanelOther, wxID_ONLINE_CFG_TEST_SERVICE, wxT("Verify"), wxPoint(-1, -1), wxSize(-1,-1) );
	m_pRegisterService = new wxButton( pPanelOther, wxID_ONLINE_CFG_REGISTER_SERVICE, wxT("Register"), wxPoint(-1, -1), wxSize(-1,-1) );
	m_pServiceNote = new wxStaticText( pPanelOther, -1, wxT(""));
	m_pServiceNote->SetForegroundColour( *wxRED );

	/////////////////////////////////////////
	// populate PROFILE panel
	// :: username
	sizerProfile->Add( new wxStaticText( pPanelProfile, -1, wxT("Username:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerProfile->Add( m_pUsername, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: password
	sizerProfile->Add( new wxStaticText( pPanelProfile, -1, wxT("Password:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerProfile->Add( m_pPassword, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: password confirm
	sizerProfile->Add( new wxStaticText( pPanelProfile, -1, wxT("Confirm Password:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerProfile->Add( m_pPasswordConfirm, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: email
	sizerProfile->Add( new wxStaticText( pPanelProfile, -1, wxT("Email:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerProfile->Add( m_pEmail, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: nickname
	sizerProfile->Add( new wxStaticText( pPanelProfile, -1, wxT("Nickname:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerProfile->Add( m_pNickname, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: account create/test buttons
	sizerProfile->Add( m_pTestAccount, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxTOP, 10 );
	sizerProfile->Add( m_pCreateAccount, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxTOP, 10 );

	/////////////////////////////////////////
	// populate OTHER panel
	// :: Service Type
	sizerOther->Add( new wxStaticText( pPanelOther, -1, wxT("Service:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerOther->Add( m_pServiceName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: username
	sizerOther->Add( new wxStaticText( pPanelOther, -1, wxT("Username:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerOther->Add( m_pServiceUsername, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: password
	sizerOther->Add( new wxStaticText( pPanelOther, -1, wxT("Password:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerOther->Add( m_pServicePassword, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: note
	wxFlexGridSizer* sizerOtherTxt = new wxFlexGridSizer( 1, 1, 5, 5 );
	sizerOtherTxt->Add( m_pServiceNote, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerOtherMain->Add( sizerOtherTxt, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxTOP, 5 );
	// :: account create/test buttons
	wxFlexGridSizer* sizerOtherBtn = new wxFlexGridSizer( 1, 2, 5, 5 );
	sizerOtherBtn->Add( m_pTestService, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxRIGHT, 30 );
	sizerOtherBtn->Add( m_pRegisterService, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerOtherMain->Add( sizerOtherBtn, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxTOP, 10 );

	// add sizer/panels to notebook
	pPanelProfile->SetSizer( sizerProfilePage );
	pNotebook->AddPage( pPanelProfile, wxT("Profile") );
	pPanelOther->SetSizer( sizerOtherPage );
	pNotebook->AddPage( pPanelOther, wxT("Services") );

	pTopSizer->Add( pNotebook, 0, wxGROW | wxALL|wxEXPAND, 10 );
	pTopSizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10 );

	pNotebook->Layout();
	SetSizer(pTopSizer);
	SetAutoLayout(TRUE);
	pTopSizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
COnlineCfgDlg::~COnlineCfgDlg( ) 
{
	// profile
	delete( m_pUsername );
	delete( m_pPassword );
	delete( m_pPasswordConfirm );
	delete( m_pEmail );
	delete( m_pNickname );
	delete( m_pTestAccount );
	delete( m_pCreateAccount );
	// other
	delete( m_pServiceName );
	delete( m_pServiceUsername );
	delete( m_pServicePassword );
	delete( m_pTestService );
	delete( m_pRegisterService );
	delete( m_pServiceNote );

	delete( pPanelOther );
}

////////////////////////////////////////////////////////////////////
void COnlineCfgDlg::SetConfig( CUnimapOnline* pUnimapOnline )
{
	int i=0;
	m_pUnimapOnline = pUnimapOnline;
	m_nServiceId = 0;

	int h=0;
	m_nWrapWidth = 0;
	this->GetSize( &m_nWrapWidth, &h );
	m_nWrapWidth -= 40;

	////////////
	// populate my fields
	// :: PROFILE
	m_pUsername->SetValue( m_pUnimapOnline->m_strUsername );
	m_pPassword->SetValue( wxString(m_pUnimapOnline->m_strPassword,wxConvUTF8) );
	m_pPasswordConfirm->SetValue( wxString(m_pUnimapOnline->m_strPassword,wxConvUTF8) );
	m_pEmail->SetValue( m_pUnimapOnline->m_strEmail );
	m_pNickname->SetValue( m_pUnimapOnline->m_strNickname );

	// populate service accounts
	m_pServiceName->Clear();
	for( i=0; i<pUnimapOnline->m_nServiceAccounts; i++ )
	{
		m_pServiceName->Append( pUnimapOnline->m_vectServiceAccounts[i].name );
		m_vectServiceUsername[i] = pUnimapOnline->m_vectServiceAccounts[i].username;
		strcpy( m_vectServicePassword[i], pUnimapOnline->m_vectServiceAccounts[i].password );
	}
	m_pServiceName->SetSelection(0);
	m_pServiceUsername->SetValue( m_vectServiceUsername[0] );
	m_pServicePassword->SetValue( wxString(m_vectServicePassword[0],wxConvUTF8) );
	m_pServiceNote->SetLabel( pUnimapOnline->m_vectServiceAccounts[0].note );
	m_pServiceNote->Wrap( m_nWrapWidth );

	pPanelOther->Layout();
	Refresh(false);
}

// Method:	OnCreateTestAccount
////////////////////////////////////////////////////////////////////
void COnlineCfgDlg::OnCreateTestAccount( wxCommandEvent& pEvent )
{
	int nActionId = pEvent.GetId();

	wxString strUserName = m_pUsername->GetLineText(0).Trim(0).Trim(1);
	wxString strPassword = m_pPassword->GetLineText(0).Trim(0).Trim(1);
	wxString strPasswordConfirm = m_pPasswordConfirm->GetLineText(0).Trim(0).Trim(1);
	wxString strEmail = m_pEmail->GetLineText(0).Trim(0).Trim(1);
	wxString strNickname = m_pNickname->GetLineText(0).Trim(0).Trim(1);

	// check if username is not empty
	if( strUserName.IsEmpty() )
	{
		wxMessageBox( wxT("Please enter your username!"), wxT("ERROR :: USERNAME EMPTY"), wxICON_EXCLAMATION|wxOK );
		return;

	}
	// check if password matches
	if( strPassword.Cmp( strPasswordConfirm ) != 0 )
	{
		wxMessageBox( wxT("Password confirmation doesn't match!\nPlease type your password again"), wxT("ERROR :: PASSWORD NO MATCH"), wxICON_EXCLAMATION|wxOK );
		return;
	}
	// check if password is empty
	if( strPassword.IsEmpty() )
	{
		wxMessageBox( wxT("Please enter your password!"), wxT("ERROR :: PASSWORD EMPTY"), wxICON_EXCLAMATION|wxOK );
		return;
	}

	// check type of action
	if( nActionId == wxID_ONLINE_CFG_CREATE_ACCOUNT )
	{
		if( m_pUnimapOnline->CreateAccount( strUserName, strPassword, strEmail, strNickname ) )
			wxMessageBox( wxT("Account created successfully!"), wxT("SUCCESS :: ACCOUNT OK!"), wxICON_EXCLAMATION|wxOK );
		else
			wxMessageBox( wxT("Failed to create account!"), wxT("ERROR :: ACCOUNT FAILED!"), wxICON_EXCLAMATION|wxOK );

	} else if( nActionId == wxID_ONLINE_CFG_TEST_ACCOUNT )
	{
		if( m_pUnimapOnline->LoginT( strUserName, strPassword ) )
			wxMessageBox( wxT("Account test success!"), wxT("SUCCESS :: TEST OK!"), wxICON_EXCLAMATION|wxOK );
		else
			wxMessageBox( wxT("Account test failed!"), wxT("ERROR :: TEST FAILED!"), wxICON_EXCLAMATION|wxOK );
	}
}

// Method:	OnSelectService
////////////////////////////////////////////////////////////////////
void COnlineCfgDlg::OnSelectService( wxCommandEvent& pEvent )
{
	int nId = m_pServiceName->GetSelection();
	if( nId < 0 || nId >= 10 ) return;
	// set prev fields
	m_vectServiceUsername[m_nServiceId] = m_pServiceUsername->GetLineText(0).Trim(0).Trim(1);
	strcpy( m_vectServicePassword[m_nServiceId], m_pServicePassword->GetLineText(0).Trim(0).Trim(1).ToAscii() );
	// set current service fields
	m_pServiceUsername->SetValue( m_vectServiceUsername[nId] );
	m_pServicePassword->SetValue( wxString(m_vectServicePassword[nId],wxConvUTF8) );
	m_pServiceNote->SetLabel( m_pUnimapOnline->m_vectServiceAccounts[nId].note );
	m_pServiceNote->Wrap( m_nWrapWidth );
	// now set the current id
	m_nServiceId = nId;

	pPanelOther->Layout();
	Refresh(false);
}

// Method:	OnTestService
////////////////////////////////////////////////////////////////////
void COnlineCfgDlg::OnTestService( wxCommandEvent& pEvent )
{
	int nId = m_pServiceName->GetSelection();
	if( nId < 0 || nId >= 10 ) return;

	// id=0 - meteoblue
	if( nId == 0 )
	{
		// test login meteoblue
		CMeteoBlue src;
		wxString strUser = m_pServiceUsername->GetLineText(0).Trim(0).Trim(1);
		wxString strPassword = m_pServicePassword->GetLineText(0).Trim(0).Trim(1);
		if( src.Login( strUser.ToAscii(), strPassword.ToAscii() ) )
			wxMessageBox( wxT("Account access successfully tested!"), wxT("Test Status"), wxICON_INFORMATION|wxOK );
		else
			wxMessageBox( wxT("Failed to access the account!\n Please check your login credentials!"), wxT("Test Status"), wxICON_ERROR|wxOK );
	}
}

// Method:	OnRegisterService
////////////////////////////////////////////////////////////////////
void COnlineCfgDlg::OnRegisterService( wxCommandEvent& pEvent )
{
	int nId = m_pServiceName->GetSelection();
	if( nId < 0 || nId >= 10 ) return;

	wxLaunchDefaultBrowser( m_pUnimapOnline->m_vectServiceAccounts[nId].reg_page, wxBROWSER_NEW_WINDOW );
}

// Method:	OnValidate
////////////////////////////////////////////////////////////////////
void COnlineCfgDlg::OnValidate( wxCommandEvent& pEvent )
{
	pEvent.Skip();

	int nId = m_pServiceName->GetSelection();
	m_vectServiceUsername[nId] = m_pServiceUsername->GetLineText(0).Trim(0).Trim(1);
	strcpy( m_vectServicePassword[nId], m_pServicePassword->GetLineText(0).Trim(0).Trim(1).ToAscii() );

	return;
}
