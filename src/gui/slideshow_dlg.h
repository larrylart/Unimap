////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _SLIDESHOW_DLG_H
#define _SLIDESHOW_DLG_H

// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/spinctrl.h>

// gui ids
enum
{
	//wxID_... = 9500,
};

// external classes
class CConfigMain;
class CImageDb;

// class:	CSlideShowDlg
///////////////////////////////////////////////////////
class CSlideShowDlg : public wxDialog
{
// public methods
public:
    CSlideShowDlg( wxWindow *parent, wxString title, CImageDb* pImageDb, CConfigMain* pMainConfig=NULL );
	virtual ~CSlideShowDlg( );

	void SetConfig( CConfigMain* pMainConfig );

// public data
public:
	wxChoice*	m_pSources;
	wxChoice*	m_pOrder;
	wxSpinCtrl* m_pShowTime;
	wxCheckBox*	m_pRepeat;
	wxCheckBox*	m_pFullScreen;

	// external objects
	CConfigMain* m_pMainConfig;
	CImageDb* m_pImageDb;

};

#endif
