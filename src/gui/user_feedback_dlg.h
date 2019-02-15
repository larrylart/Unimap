////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _USER_FEEDBACK_DLG_H
#define _USER_FEEDBACK_DLG_H

// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// defines
#define FILE_EXPORT_FORMATS wxT( "Html Files (*.html;*.html)|*.html;*.html|HTML (*.html)|*.htm" )

// gui ids
enum
{
	wxID_USER_FEEDBACK_SUBMIT = 3850,
	wxID_USER_FEEDBACK_CANCEL,
};

// external classes
class CUnimapOnline;

// class:	CUserFeedbackDlg
///////////////////////////////////////////////////////
class CUserFeedbackDlg : public wxDialog
{
// public methods
public:
    CUserFeedbackDlg( wxWindow *parent, wxString title, CUnimapOnline* pOnline );
	virtual ~CUserFeedbackDlg( );

// public data
public:
	wxChoice*	m_pType;
	wxChoice*	m_pSection;
	wxTextCtrl* m_pTitle;
	wxTextCtrl* m_pBody;
	wxButton*	m_pSubmit;
	wxButton*	m_pCancel;

	// external objects
	CUnimapOnline*	m_pUnimapOnline;

private:
//	void OnFileSet( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
