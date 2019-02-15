////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CANONEOS_SETUP_DLG_H
#define _CANONEOS_SETUP_DLG_H

// system
#include <map>

// wx
#include "wx/wxprec.h"

// canon eos headers
#include "../camera/canoneos/EDSDK.h"
#include "../camera/canoneos/EDSDKTypes.h"

enum
{
//	wxID_CATALOG_VIEW_PAGE_BACK		= 2600,
//	wxID_CATALOG_VIEW_PAGE_NEXT		= 2601
};

// external classes
class CCameraCanonEos;

// class:	CCanonEosSetupDlg
///////////////////////////////////////////////////////
class CCanonEosSetupDlg : public wxDialog
{
// methods
public:
    CCanonEosSetupDlg( wxWindow *parent, const wxString& title );
	~CCanonEosSetupDlg( );

	void SetConfig( CCameraCanonEos* pCamera );

	void InitISO( );
	void InitTV( );
	void InitIMQ( );
	void InitAV( );
	void InitAEM( );
	void InitMM( );

	void SetupProperty( wxChoice* pSelect, long* vectMap, std::map<EdsUInt32, const char *>& vectPropertyTable, const EdsPropertyDesc* desc );
	void SelectProperty( wxChoice* pSelect, long* vectMap, EdsUInt32 value );

	// get properties
	EdsUInt32 GetProperty( wxChoice* pSelect, long* vectMap );
	// specific properties
	EdsUInt32 GetTv(){ return( GetProperty( m_pTVSelect, m_vectMapTvSelect ) ); }
	EdsUInt32 GetIso(){ return( GetProperty( m_pISOSelect, m_vectMapIsoSelect ) ); }
	EdsUInt32 GetAv(){ return( GetProperty( m_pAVSelect, m_vectMapAvSelect ) ); }
	EdsUInt32 GetMm(){ return( GetProperty( m_pMMSelect, m_vectMapMmSelect ) ); }

/////////////
// ::DATA
public:
	// gui elements
	wxChoice*	m_pTVSelect;
	wxChoice*	m_pISOSelect;
	wxChoice*	m_pIMQSelect;
	wxChoice*	m_pAVSelect;
	wxChoice*	m_pAEMSelect;
	wxChoice*	m_pMMSelect;

	// data elements
	std::map<EdsUInt32, const char *> m_vectTvPropertyTable;
	std::map<EdsUInt32, const char *> m_vectIsoPropertyTable;
	std::map<EdsUInt32, const char *> m_vectImqPropertyTable;
	std::map<EdsUInt32, const char *> m_vectAvPropertyTable;
	std::map<EdsUInt32, const char *> m_vectAemPropertyTable;
	std::map<EdsUInt32, const char *> m_vectMmPropertyTable;

	// select maps
	long m_vectMapTvSelect[80];
	long m_vectMapIsoSelect[25];
	long m_vectMapImqSelect[70];
	long m_vectMapAvSelect[60];
	long m_vectMapAemSelect[25];
	long m_vectMapMmSelect[25];

private:
//	void OnDoFind( wxCommandEvent& event );

	DECLARE_EVENT_TABLE()
};

#endif
