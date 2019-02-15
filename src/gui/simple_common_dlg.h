////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _SIMPLE_COMMON_DLG_H
#define _SIMPLE_COMMON_DLG_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// class:	CSimpleEntryAddDlg
///////////////////////////////////////////////////////
class CSimpleEntryAddDlg : public wxDialog
{
// public methods
public:
    CSimpleEntryAddDlg( wxWindow *parent, wxString title, wxString label, int nType=0 );
	virtual ~CSimpleEntryAddDlg( );

// public data
public:
	int m_nType;
	wxTextCtrl* m_pName;
	wxChoice*	m_pNameSelect;
	wxSpinCtrl* m_pNameSpin;
};

#endif
