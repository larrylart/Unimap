
#ifndef _CUSTOM_WXWIDGETS_H
#define _CUSTOM_WXWIDGETS_H

#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/tglbtn.h>

// class button extension
////////////////////////////////////////////////////////////////////
class wxSTButton : public wxButton
{
public:
	int m_bGoingDown;

	wxSTButton( wxWindow *parent, wxWindowID id, const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             long style = 0, const wxValidator& validator = wxDefaultValidator,
			 const wxString& name = wxButtonNameStr ) : wxButton( parent, id, label, pos, size, style, validator, name ){ m_bGoingDown=0; }

	virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
	{
		if( nMsg == WM_LBUTTONDOWN )
		{
			m_bGoingDown = 1;
			SendClickEvent();

		} else if( nMsg == WM_LBUTTONUP )
		{
			m_bGoingDown = 0;
		}

		return( wxButton::MSWWindowProc( nMsg,  wParam,  lParam ) );
	}

//	virtual void OnSTButtonDown( wxMouseEvent& pEvent ){ ActionOnDown(); pEvent.Skip(); };
//	virtual void OnSTButtonUp( wxMouseEvent& pEvent ){ ActionOnUp(); pEvent.Skip(); };
	// actions
//	virtual void ActionOnDown( ){ };
//	virtual void ActionOnUp( ){ };

private:
//	DECLARE_EVENT_TABLE()
};

// class image button extension
////////////////////////////////////////////////////////////////////
class wxSTIButton : public wxBitmapButton
{
public:
	int m_bGoingDown;

	wxSTIButton( wxWindow *parent,
                   wxWindowID id,
                   const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxButtonNameStr ) : wxBitmapButton( parent, id, bitmap, pos, size, style, validator, name ){ m_bGoingDown=0; }

	virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
	{
		if( nMsg == WM_LBUTTONDOWN )
		{
			m_bGoingDown = 1;
			SendClickEvent();

		} else if( nMsg == WM_LBUTTONUP )
		{
			m_bGoingDown = 0;
		}

		return( wxBitmapButton::MSWWindowProc( nMsg,  wParam,  lParam ) );
	}
};


#endif
