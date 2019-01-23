
#ifndef _UPLOAD_CUSTOM_DLG_H
#define _UPLOAD_CUSTOM_DLG_H

#include "wx/wxprec.h"
#include <wx/listctrl.h>

enum
{

};

// external classes
class CUnimapOnline;

// class:	CUploadCustomDlg
///////////////////////////////////////////////////////
class CUploadCustomDlg : public wxDialog
{
// methods
public:
    CUploadCustomDlg( wxWindow *parent, const wxString& title );
	~CUploadCustomDlg( );

	void SetConfig( CUnimapOnline* pUnimapOnline );

/////////////
// ::DATA
	CUnimapOnline* m_pUnimapOnline;

//	wxButton*		m_pPrevButton;
//	wxListCtrl*		m_pCatalogTable;

private:
//	void OnAddToUploadList( wxCommandEvent& event );

	DECLARE_EVENT_TABLE()
};

#endif
