
#ifndef _TELESCOPE_SETUP_DLG_H
#define _TELESCOPE_SETUP_DLG_H

#include "wx/wxprec.h"
#include <wx/calctrl.h>
#include <wx/datectrl.h>
#include <wx/datetime.h>

enum
{
	wxID_TELESCOPE_SETUP_SITE_ADD		= 8600,
};

// external classes
class CTelescope;
class CCamera;
class CObserver;
class CUnimapWorker;

// class:	CTelescopeSetupDlg
///////////////////////////////////////////////////////
class CTelescopeSetupDlg : public wxDialog
{
// methods
public:
    CTelescopeSetupDlg( wxWindow *parent, CTelescope* pTelescope, const wxString& title );
	~CTelescopeSetupDlg( );

	void SetConfig( CObserver* pObserver, CCamera* pCamera, CUnimapWorker* pUnimapWorker );

/////////////
// ::DATA
public:
	CTelescope* m_pTelescope;
	CCamera* m_pCamera;
	CObserver* m_pObserver;

	CUnimapWorker* m_pUnimapWorker;

	wxChoice*			m_pMountTypeSelect;
	wxChoice*			m_pSiteNameSelect;
	wxButton*			m_pSiteSetButton;
	wxDatePickerCtrl*	m_pDate;
	wxTextCtrl*			m_pTime;
	wxChoice*			m_pGuideCamSelect;

	wxCheckBox*			m_pReverseLeftRight;
	wxCheckBox*			m_pReverseUpDown;
	wxCheckBox*			m_pCordWrap;
	wxCheckBox*			m_pHighPrecision;

private:
	void OnSiteAdd( wxCommandEvent& event );

	DECLARE_EVENT_TABLE()
};

#endif
