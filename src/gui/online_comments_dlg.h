////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _ONLINE_COMMENTS_DLG_H
#define _ONLINE_COMMENTS_DLG_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// gui ids
enum
{
	wxID_ONLINE_COMMENT_SELECT = 14400,
	wxID_ONLINE_COMMENT_BUTTON
};

// external classes
class CUnimapOnline;

// class:	COnlineCommnetsDlg
///////////////////////////////////////////////////////
class COnlineCommnetsDlg : public wxDialog
{
// public methods
public:
    COnlineCommnetsDlg( wxWindow *parent, wxString title, int nType=0 );
	virtual ~COnlineCommnetsDlg( );

	void SetConfig( CUnimapOnline* pUnimapOnline, int nImageId );

// public data
public:
	CUnimapOnline* m_pUnimapOnline;
	int m_nImageId;

	wxChoice*	m_pCommentsList;
	wxTextCtrl* m_pTitle;
	wxTextCtrl* m_pBody;
	wxButton*	m_pNewViewButton;

	// values
	wxString m_strBodyBackup;
	// flags
	int m_nViewType;

private:
	void OnNewView( wxCommandEvent& pEvent );
	void OnSelectComment( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
