////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _HARDWARE_SELECT_DLG_H
#define _HARDWARE_SELECT_DLG_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// gui enum
enum
{
	wxID_HARDWARE_SELECT_SOURCE = 19000,
	wxID_HARDWARE_SELECT_TYPE,
	wxID_HARDWARE_SELECT_BRAND,
	wxID_HARDWARE_SELECT_NAME
};

// external classes
class CCamera;
class CTelescope;
class CObserver;

// class:	CHardwareSelectDlg
///////////////////////////////////////////////////////
class CHardwareSelectDlg : public wxDialog
{
// public methods
public:
    CHardwareSelectDlg( wxWindow *parent, wxString title, wxString label );
	virtual ~CHardwareSelectDlg( );

	// geat real ids
	int GetBrandId(){ if( m_vectBrandId.size() ) return( m_vectBrandId.at(m_pBrandSelect->GetSelection()) ); else return(0); }
	int GetNameId(){ if( m_vectNameId.size() ) return( m_vectNameId.at(m_pNameSelect->GetSelection()) ); else return(0); }
	// reverse
	int GetBrandSelectFromId( int nId ){ if( m_vectBrandId.size() ) for(int i=0;i<m_vectBrandId.size();i++) if(m_vectBrandId[i] == nId ) return( i );  return(0); }

	void SetData( CObserver* pObserver, CCamera* pCamera, int nHType=0, int nSourceId=0, int nType=0, int nBrand=0, int nName=0 );
	void SetData( CObserver* pObserver, CTelescope* pTelescope, int nHType=0 );
	// other handles
	void SetHardwareData( int nHType=0 );

// public data
public:
	CObserver*	m_pObserver;
	CCamera*	m_pCamera;
	CTelescope*	m_pTelescope;

	// :: select options
	wxChoice*	m_pSourceSelect;
	wxStaticText*	m_pTypeBrandLabel;
	wxChoice*	m_pTypeSelect;
	wxChoice*	m_pBrandSelect;
	wxChoice*	m_pNameSelect;
	// :: extra types
	int m_nSourceId;
	int m_nHardwareType;
	int m_nHType;
	// hardware props
	int m_nType;
	int m_nBrand;
	int m_nName;
	int m_nNameOwn;
	// :: vectors to keep the real ids 
	std::vector<int> m_vectBrandId;
	std::vector<int> m_vectNameId;

// private methods
private:
	void OnSelectHardware( wxCommandEvent& pEvent );
	void OnSourceType( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
