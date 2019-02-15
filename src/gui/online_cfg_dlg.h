////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _ONLINE_CFG_DLG_H
#define _ONLINE_CFG_DLG_H

// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// enum gui ids
enum
{
	wxID_ONLINE_CFG_CREATE_ACCOUNT	= 2330,
	wxID_ONLINE_CFG_TEST_ACCOUNT,
	wxID_ONLINE_SERVICE_NAME,
	wxID_ONLINE_CFG_TEST_SERVICE,
	wxID_ONLINE_CFG_REGISTER_SERVICE
};

// external classes
class CUnimapOnline;

// class:	COnlineCfgDlg
///////////////////////////////////////////////////////
class COnlineCfgDlg : public wxDialog
{
// public methods
public:
    COnlineCfgDlg( wxWindow *parent );
	virtual ~COnlineCfgDlg( );

	void SetConfig( CUnimapOnline* pUnimapOnline );

// public data
public:
	// pointer to the online engine
	CUnimapOnline* m_pUnimapOnline;

	// :: profile
	wxTextCtrl*	m_pUsername;
	wxTextCtrl*	m_pPassword;
	wxTextCtrl*	m_pPasswordConfirm;
	wxTextCtrl*	m_pEmail;
	wxTextCtrl*	m_pNickname;
	wxButton* m_pTestAccount;
	wxButton* m_pCreateAccount;

	// other
	wxPanel* pPanelOther;
	wxChoice*	m_pServiceName;
	wxTextCtrl*	m_pServiceUsername;
	wxTextCtrl*	m_pServicePassword;
	wxButton* m_pTestService;
	wxButton* m_pRegisterService;
	wxStaticText* m_pServiceNote;

	// data for services
	int m_nServiceId;
	wxString m_vectServiceUsername[10];
	char m_vectServicePassword[10][255];

	int m_nWrapWidth;

// private methods
private:
	void OnCreateTestAccount( wxCommandEvent& pEvent );
	void OnSelectService( wxCommandEvent& pEvent );
	void OnTestService( wxCommandEvent& pEvent );
	void OnRegisterService( wxCommandEvent& pEvent );

	// validator
	void OnValidate( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif


