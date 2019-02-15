////////////////////////////////////////////////////////////////////
// Package:		detect config definition
// File:		detectconfig.h
// Purpose:		define a config view window
//
// Created by:	Larry Lart on 19/06/2007
// Updated by:	Larry Lart on 06/03/2010
//
// Copyright:	(c) 2007-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _DETECTCONFIGVIEW_H
#define _DETECTCONFIGVIEW_H

// system headers
#include <time.h>
#include <vector>
#include <cmath>

// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// gui ids
enum
{
	wxID_BUTTON_ADD_PROFILE			= 3600,
	wxID_BUTTON_DELETE_PROFILE,
	wxID_SELECT_DETECT_PROFILE,
	wxID_SELECT_DETECT_DO_CLEAN,
	wxID_SELECT_DETECT_USE_CHECKIMAGE,
	wxID_BUTTON_CHECK_IMAGE_BROWSE
};

// external classes
class CConfigDetect;
class CConfigMain;

// class:	CDetectConfigView
///////////////////////////////////////////////////////
class CDetectConfigView : public wxDialog
{
// public methods
public:
    CDetectConfigView( wxWindow *parent );
	virtual ~CDetectConfigView( );

	void SetConfig( CConfigDetect* pDetectConfig, CConfigMain* pConfigMain );
	void StoreSelectedProfile( );
	void SetCurrentProfile( );

// public data
public:
	CConfigDetect* m_pDetectConfig;
	CConfigMain* m_pMainConfig;

	// vector to store the detection profiles
	std::vector<DefConfigDetector> m_vectProfile;
	// keep current selection
	int m_nSelectedProfile;

	// profile stuff
	wxChoice*	m_pProfileSelect;
	wxButton*	m_pAddProfile;
	wxButton*	m_pDeleteProfile;

	// detect tab
	wxChoice*	m_pImageChannels;
	wxCheckBox*	m_pUseMaskAreas;
	wxChoice*	m_pDetectType;
	wxTextCtrl*	m_pDetectMinArea;
	wxTextCtrl*	m_pDetectThresh;
	wxTextCtrl* m_pAnalysisThresh;
	wxTextCtrl*	m_pDeblendNThresh;
	wxTextCtrl* m_pDeblendNMinCont;
	wxCheckBox*	m_pDetectClean;
	wxTextCtrl*	m_pDetectCleanParam;

	// processing tab
	wxCheckBox*	m_pDetectFilter;
	wxChoice*	m_pDetectFilterName;
	wxTextCtrl*	m_pFilterThresh;
	wxChoice*	m_pMaskType;
	wxChoice*	m_pThreshType;
	wxChoice*	m_pWeightType;
	wxTextCtrl*	m_pScanIsoApRatio;
	wxCheckBox*	m_pMiscBlank;

	// Photometry tab
	wxTextCtrl*	m_pPhotApertures;
	wxTextCtrl*	m_pPhotAutoparamKron;
	wxTextCtrl*	m_pPhotAutoparamMinRad;
	wxTextCtrl*	m_pPhotPetroparamFact;
	wxTextCtrl*	m_pPhotPetroparamMinRad;
	wxTextCtrl*	m_pSaturationLevel;
	wxTextCtrl*	m_pMagZeroPoint;
	wxTextCtrl*	m_pMagGamma;
	wxTextCtrl*	m_pPhotometryGain;
	wxTextCtrl*	m_pPixelScale;

	// classification tab
	wxTextCtrl*	m_pSeeingFwhm;
	wxChoice*	m_pStarNnwName;
	wxTextCtrl*	m_pBackSize;
	wxTextCtrl*	m_pBackFilterSize;
	wxChoice*	m_pBackPhotoType;
	wxTextCtrl*	m_pBackFilterThresh;
	wxCheckBox*	m_pUseCheckImage;
	wxChoice*	m_pCheckImageType;
	wxTextCtrl*	m_pCheckImageName;
	wxButton*	m_pCheckImagePick;

	// Misc tab
//	wxTextCtrl*	m_pMemObjStack;
//	wxTextCtrl*	m_pMemPixStack;
//	wxTextCtrl*	m_pMemBufSize;
//	wxChoice*	m_pVerboseType;
//	wxCheckBox*	m_pWriteXml;
//	wxTextCtrl*	m_pXmlName;

private:
	// button handlers
	void OnAddProfile( wxCommandEvent& pEvent );
	void OnDeleteProfile( wxCommandEvent& pEvent );
	void OnSelectDetectProfile( wxCommandEvent& pEvent );
	void SelectDetectProfile( );
	// clean param 
	void OnUseCleaning( wxCommandEvent& pEvent );
	void SetCleaningState( int bValue );
	// check image params
	void OnUseCheckImage( wxCommandEvent& pEvent );
	void SetCheckImageState( int bValue );
	void OnCheckImageBrowse( wxCommandEvent& pEvent );
	// validate on ok
	void OnValidate( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};


#endif
