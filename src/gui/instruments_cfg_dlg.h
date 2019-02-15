////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _INSTRUMENTS_CFG_DLG_H
#define _INSTRUMENTS_CFG_DLG_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// gui ids
enum
{
	wxID_INSTR_CFG_TEL_USE_ASCOM = 17200,
	wxID_INSTR_CFG_TEL_ASCOM_GET,
	wxID_INSTR_CFG_TELESCOPE_SELECT,
	// focuser
	wxID_INSTR_CFG_FOCUSER_USE_TEL,
	wxID_INSTR_CFG_FOCUSER_USE_ASCOM,
	wxID_INSTR_CFG_FOC_ASCOM_GET,
	wxID_INSTR_CFG_FOCUSER_USE_CUSTOM,
	wxID_INSTR_CFG_FOCUSER_SELECT,
	// camera
	wxID_INSTR_CFG_CAMERA_SELECT,
	wxID_INSTR_CFG_CAMERA_ACTIVATE,
	// dome
	wxID_INSTR_CFG_DOME_USE_ASCOM,
	wxID_INSTR_CFG_DOME_ASCOM_GET,
	wxID_INSTR_CFG_DOME_USE_CUSTOM,
	wxID_INSTR_CFG_DOME_CUSTOM_SELECT,
	// conditions
	wxID_INSTR_CFG_COND_USE_ONLINE,
	wxID_INSTR_CFG_COND_ONLINE_SELECT,
	wxID_INSTR_CFG_COND_USE_OWN,
	wxID_INSTR_CFG_COND_OWN_SELECT
};

// external classes
class CTelescope;
class CFocuser;
class CCamera;

// class:	CInstrumentsCfgDlg
///////////////////////////////////////////////////////
class CInstrumentsCfgDlg : public wxDialog
{
// public methods
public:
    CInstrumentsCfgDlg( wxWindow *parent );
	virtual ~CInstrumentsCfgDlg( );

	void SetConfig( CTelescope* pTelescope, CFocuser* pFocuser, CCamera* pCamera );

// public data
public:
	// external objects
	CTelescope* m_pTelescope;
	CFocuser*	m_pFocuser;
	CCamera*	m_pCamera;

	//////////////
	// :: general
	wxCheckBox*		m_pAutoDetection;
	wxCheckBox*		m_pAutoMatching;
	wxCheckBox*		m_pAutoRegister;
	wxCheckBox*		m_pAutoStacking;
	wxCheckBox*		m_pAutoUpload;
	wxCheckBox*		m_pNotifyQuality;
	wxCheckBox*		m_pNotifyDiscovery;
	wxCheckBox*		m_pRemoteAccess;

	//////////////
	// :: guide
	wxCheckBox*		m_pReverseNS;
	wxCheckBox*		m_pReverseWE;
	wxSpinCtrl*		m_pRaPulseLength;
	wxSpinCtrl*		m_pDecPulseLength;

	//////////////
	// :: telescope :: ascom
	wxStaticText*	m_pTelAscomLabel;
	wxCheckBox*		m_pTelUseAscom;
	wxTextCtrl*		m_pTelAscomDriverName;
	wxButton*		m_pTelPickAscom;
	// :: telescope :: built in
	wxStaticText*	m_pTelescopeLabel;
	wxChoice*		m_pTelescopeSelect;
	wxStaticText*	m_pTelSerialPortLabel;
	wxChoice*		m_pTelSerialPortSelect;
	wxStaticText*	m_pTelSerialBaudLabel;
	wxChoice*		m_pTelSerialPortBaud;

	///////////////////////
	// :: focuser :: telescope
	int m_nUseFocuser;
	wxRadioButton*	m_pFocUseTel;
	wxStaticText*	m_pFocUseTelLabel;
	// :: focuser :: ascom
	wxRadioButton*	m_pFocUseAscom;
	wxStaticText*	m_pFocUseAscomLabel;
	wxTextCtrl*		m_pFocAscomDriverName;
	wxButton*		m_pFocPickAscom;
	// :: focuser :: built in
	wxRadioButton*	m_pFocUseCustom;
	wxStaticText*	m_pFocuserLabel;
	wxChoice*		m_pFocuserSelect;
	wxStaticText*	m_pFocSerialPortLabel;
	wxChoice*		m_pFocSerialPortSelect;
	wxStaticText*	m_pFocSerialBaudLabel;
	wxChoice*		m_pFocSerialPortBaud;

	///////////////////////
	// :: camera ::
	// local list of camera devices
	std::vector<DefHWDevice> m_vectCamDevices;

	int m_nCameraId;
	wxStaticBox* pCameraPropBox;
	wxChoice*		m_pCameraSelect;
	wxCheckBox*		m_pCamActivate;
	wxChoice*		m_pCamInterface;
	wxCheckBox*		m_pCamDoRecord;
	wxCheckBox*		m_pCamDoGuide;
	wxCheckBox*		m_pCamDoFocus;
	wxCheckBox*		m_pCamDoFinder;
	wxCheckBox*		m_pCamDoSpectroscopy;
	wxCheckBox*		m_pCamDoMonitor;

	///////////////////////
	// :: DOME :: ascom
	wxRadioButton*	m_pDomeUseAscom;
	wxStaticText*	m_pDomeUseAscomLabel;
	wxTextCtrl*		m_pDomeAscomDriverName;
	wxButton*		m_pDomePickAscom;
	// :: DOME :: built in
	wxRadioButton*	m_pDomeUseCustom;
	wxStaticText*	m_pDomeCustomLabel;
	wxChoice*		m_pDomeCustomSelect;

	///////////////////////
	// :: Conditions :: wheather
	wxRadioButton*	m_pCondUseOnline;
	wxStaticText*	m_pCondOnlineLabel;
	wxChoice*		m_pCondOnlineSelect;
	wxRadioButton*	m_pCondUseOwn;
	wxStaticText*	m_pCondOwnLabel;
	wxChoice*		m_pCondOwnSelect;
	wxStaticText*	m_pCondSerialPortLabel;
	wxChoice*		m_pCondSerialPortSelect;
	wxStaticText*	m_pCondSerialBaudLabel;
	wxChoice*		m_pCondSerialPortBaud;

// private methods
private:
	void OnUseAscom( wxCommandEvent& pEvent );
	void SetUseTelescope( int nOpt );
	void OnAscomChooser( wxCommandEvent& pEvent );
	void OnUseFocuser( wxCommandEvent& pEvent );
	void SetUseFocuser( int nOpt );
	// camera
	void OnCameraActivate( wxCommandEvent& pEvent );
	void SetCameraState( int nState );
	void OnCameraSelect( wxCommandEvent& pEvent );
	void GetCameraValues( );
	void SetCameraValues( );

	// validator
	void OnValidate( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif


