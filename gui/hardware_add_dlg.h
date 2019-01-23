
#ifndef _HARDWARE_ADD_DLG_H
#define _HARDWARE_ADD_DLG_H

// system includes
#include <vector>
// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// gui enum
enum
{
	wxID_HARDWARE_SELECT_TYPE = 19000,
	wxID_HARDWARE_SELECT_BRAND,
	wxID_HARDWARE_SELECT_CAMERA,
	wxID_HARDWARE_OTHER_BRAND,
	wxID_HARDWARE_OTHER_NAME,
};

// external classes
class CCamera;
class CTelescope;

// class:	CHardwareAddDlg
///////////////////////////////////////////////////////
class CHardwareAddDlg : public wxDialog
{
// public methods
public:
    CHardwareAddDlg( wxWindow *parent, wxString title );
	virtual ~CHardwareAddDlg( );

	void SetData( CCamera* pCamera, int nHType=0 );
	void SetData( CTelescope* pTelescope, int nHType=0 );

	// geat real ids
	int GetBrandId(){ if( m_vectBrandId.size() ) return( m_vectBrandId.at(m_pBrandSelect->GetSelection()) ); else return(0); }
	int GetNameId(){ if( m_vectNameId.size() ) return( m_vectNameId.at(m_pNameSelect->GetSelection()) ); else return(0); }

// public data
public:
	CCamera*	m_pCamera;
	CTelescope*	m_pTelescope;
	// types
	int m_nObjectType;
//	int m_nMainType;
	int m_nHType;

	// :: select options
	wxStaticText*	m_pTypeLabel;
	wxChoice*	m_pTypeSelect;
	wxChoice*	m_pBrandSelect;
	wxChoice*	m_pNameSelect;
	// :: text entry options
	wxTextCtrl*	m_pBrandEntry;
	wxTextCtrl*	m_pNameEntry;
	// :: custom buttons
	wxButton*	m_pBrandButton;
	wxButton*	m_pNameButton;
	// :: extra types
	int m_nType;
	int m_nBrand;
	int m_nName;
	// switch types - used to switch from select to entry
	int m_nBrandType;
	int m_nNameType;

	// :: vectors to keep the real ids 
	std::vector<int> m_vectBrandId;
	std::vector<int> m_vectNameId;

// private methods
private:
	void OnSelectHardware( wxCommandEvent& pEvent );
	void OnActionType( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
