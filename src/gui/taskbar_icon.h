////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _TASKBAR_ICON_H
#define _TASKBAR_ICON_H

// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/taskbar.h>

enum
{
	wxID_RCLICK_ICON_EXIT = 3344,
	wxID_RCLICK_ICON_SHOW
};

// external classes
class CGUIFrame;

// class:	CFrameTaskBarIcon
///////////////////////////////////////////////////////
class CFrameTaskBarIcon : public wxTaskBarIcon
{
// methods
public:
	void OnShowFrame( wxTaskBarIconEvent& pEvent );
	virtual wxMenu* CreatePopupMenu( );
	void OnExit( wxCommandEvent& pEvent );
	void OnShow( wxCommandEvent& pEvent );

// public data
public:
	CGUIFrame *m_pFrame;
	
	DECLARE_EVENT_TABLE()
};

#endif
