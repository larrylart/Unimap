////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _GROUP_ADD_DLG_H
#define _GROUP_ADD_DLG_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// gui ids
enum
{
	wxID_GROUP_SET_PATH_BUTTON = 29000,
	wxID_GROUP_ADD_PATH_FILES
};

// class:	CGroupAddDlg
///////////////////////////////////////////////////////
class CGroupAddDlg : public wxDialog
{
// public methods
public:
    CGroupAddDlg( wxWindow *parent, wxString title);
	virtual ~CGroupAddDlg( );

// public data
public:
	wxTextCtrl* m_pName;
	wxButton* m_pPathSetButton;
	wxTextCtrl* m_pPath;
	wxCheckBox* m_pAddFiles;
	wxCheckBox* m_pIncFolders;

	wxString m_strPath;

private:
	void OnPathSet( wxCommandEvent& pEvent );
	void OnPathAddFiles( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
