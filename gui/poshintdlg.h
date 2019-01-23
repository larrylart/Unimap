////////////////////////////////////////////////////////////////////
// Package:		Position Hint Dialog definition
// File:		poshintdlg.h
// Purpose:		create a dialog to se a position hint
//
// Created by:	Larry Lart on 28/06/2007
// Updated by:	Larry Lart on 14/02/2010
//
// Copyright:	(c) 2007-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _POSHINTDLG_H
#define _POSHINTDLG_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// defines
#define ORIGIN_LIST_TYPE_STAR_NAMES		0
#define ORIGIN_LIST_TYPE_DSO_NAMES		1
#define ORIGIN_LIST_TYPE_CONST_NAMES	2
#define ORIGIN_LIST_TYPE_MESSIER_ID		3
#define ORIGIN_LIST_TYPE_NGC_ID			4
#define ORIGIN_LIST_TYPE_IC_ID			5
#define ORIGIN_LIST_TYPE_SAO_ID			6
// coord entry type
#define ORIGIN_COORD_ENTRY_TYPE_DEG		1
#define FIELD_SIZE_ENTRY_TYPE_DEG		1

// controls ids
enum
{
	wxID_SELECT_ORIG_TYPE		= 12000,
	wxID_RADIO_ORIG_A,
	wxID_RADIO_ORIG_B,
	wxID_SELECT_ORIG_ENTRY_TYPE,
	wxID_RADIO_FIELD_A,
	wxID_RADIO_FIELD_B,
	wxID_SELECT_ORIG_NAME,
	wxID_SET_ORIG_RA_VALUE,
	wxID_SET_ORIG_DEC_VALUE,
	wxID_SENSOR_WIDTH,
	wxID_SENSOR_HEIGHT,
	wxID_SET_FOCAL_VALUE,
	wxID_SELECT_FIELD_FORMAT_TYPE,
	wxID_SELECT_ORIG_ID,
	wxID_SELECT_USE_HINT,
	wxID_CHECK_USE_GROUP_HINT,
	wxID_CHECK_GROUP_USE_HINT,
	wxID_SELECT_IMG_USE_GROUP_HINT,
	wxID_CAMERA_SELECT_BUTTON,
	wxID_TLENS_SELECT_BUTTON,
	wxID_SELECT_HARWARE_SETUP
};

// external classes
class CConfigMain;
class CSky; 
class CAstroImage;
class CImageGroup;
class CCamera;
class CObserver;
class CTelescope;

// local headers
#include "../observer/observer.h"

// class:	CPosHintDlg
///////////////////////////////////////////////////////
class CPosHintDlg : public wxDialog
{
// public methods
public:
    CPosHintDlg( wxWindow *parent, const wxString& title, int nType );
	virtual ~CPosHintDlg( );

	void SetConfig( CConfigMain* pMainConfig, CSky* pSky, CAstroImage* pAstroImage, 
					 CObserver* pObserver, CCamera* pCamera, CTelescope* pTelescope );
	void SetGroupConfig( CConfigMain* pMainConfig, CSky* pSky, CImageGroup* pImageGroup, 
						 CObserver* pObserver, CCamera* pCamera, CTelescope* pTelescope );
	void SetObjectNameList( int nType );
	void SetDefaultData( );

	void UpdateRaDecValues( int nType );
	void SetOrig( );
	void SetSelectOrigType( );

//	double CalculateFocalLength( DefTLensSetup& rLensSetup );
	void SetFocalLength( );
	void UpdateFieldValues( int nType );
	void CalculateFieldValues( );
	void SetField( );

	// enable/disable
	void EnableOrigA( );
	void EnableOrigB( );
	void DisableOrigA( );
	void DisableOrigB( );
	void EnableFieldA( );
	void EnableFieldB( );
	void DisableFieldA( );
	void DisableFieldB( );
	void EnableAll( );
	void DisableAll( );

// public data
public:
	CConfigMain*	m_pMainConfig;
	CSky*			m_pSky;
	CAstroImage*	m_pAstroImage;
	CObserver*		m_pObserver;
	CCamera*		m_pCamera;
	CTelescope*		m_pTelescope;

	int m_bUseHint;
	int m_bUseGroupHint;

	int m_nOrigType;
	double m_nRaDeg;
	double m_nDecDeg;

	double m_nSensorWidth;
	double m_nSensorHeight;
	double m_nPixelSize;
	double m_nFocalLength;

	double m_nFieldX;
	double m_nFieldY;

	int m_nSetupId;
	///////////////
	// camera selection if any else -1
	int m_nCameraSource;
	int m_nCameraType;
	int m_nCameraBrand;
	int m_nCameraName;
//	int m_nCameraNameOwn;
	///////////////
	// LENS SETUP
	int m_nTLensSource;
	DefTLensSetup m_rLensSetup;

	wxChoice*		m_pUseImgPosHint;
	wxCheckBox*		m_pUseGroupPosHint;
	wxChoice*		m_pUseImgGroupPosHint;

	wxRadioButton*	m_pOrigOptA;
	wxRadioButton*	m_pOrigOptB;
	wxRadioButton*	m_pFieldOptA;
	wxRadioButton*	m_pFieldOptB;

	wxFlexGridSizer* sizerOrigAPanel;
	wxStaticBox* m_pOrigBox;
	wxStaticBox* m_pFieldBox;

	wxStaticText*	m_pObjectTypeLabel;
	wxChoice*		m_pObjectType;
	wxStaticText*	m_pObjectNameLabel;
	wxChoice*		m_pObjectName;
	wxStaticText*	m_pObjectIdLabel;
	wxTextCtrl*		m_pObjectId;

	wxStaticText*	m_pRaDecEntryTypeLabel;
	wxChoice*		m_pRaDecEntryType;
	wxStaticText*	m_pRaDecLabel;
	wxTextCtrl*		m_pRaEntry;
	wxTextCtrl*		m_pDecEntry;

	/////////////////////////////////
	// FIELD
	// setups
	wxStaticText*	m_pSetupLabel;
	wxChoice*		m_pSetupSelect;
	// camera
	wxStaticText*	m_pCameraTypeLabel;
	wxTextCtrl*		m_pSensorWidth;
	wxTextCtrl*		m_pSensorHeight;
	wxButton*		m_pCameraSelectButton;
	// telescope/lens
	wxStaticText*	m_pFocalLabel;
	wxButton*		m_pTLensSelectButton;
	wxTextCtrl*		m_pFocal;

	wxStaticText*	m_pFieldFormatLabel;
	wxChoice*		m_pFieldFormat;
	wxStaticText*	m_pFieldSizeLabel;
	wxTextCtrl*		m_pFieldWidth;
	wxTextCtrl*		m_pFieldHeight;

// private methods
private:
	void OnUseHint( wxCommandEvent& pEvent );
	void OnUseGroupHint( wxCommandEvent& pEvent );

	void OnSelectOrigType( wxCommandEvent& pEvent );
	void OnSelectOrigName( wxCommandEvent& pEvent );
	void OnSetObjectId( wxCommandEvent& pEvent );

	void OnRadioOrigEntry( wxCommandEvent& pEvent );
	void OnRadioFieldEntry( wxCommandEvent& pEvent );
	void OnEntryOrigType( wxCommandEvent& pEvent );
	// camera
	void OnSetSensorSize( wxCommandEvent& pEvent );
	void OnSelectCamera( wxCommandEvent& pEvent );
	// telescope
	void OnSetFocalLength( wxCommandEvent& pEvent );
	void OnSelectTLens( wxCommandEvent& pEvent );
	// field
	void OnFieldFormat( wxCommandEvent& pEvent );

	void OnValidate( wxCommandEvent& event );

	DECLARE_EVENT_TABLE()
};


#endif
