////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CGroupImgView_H
#define _CGroupImgView_H

#include <wx/listctrl.h>

// class :: CGroupImgView
////////////////////////////////////////////////
class CGroupImgView : public wxListCtrl
{
// public methods
public: 
	CGroupImgView( wxWindow *frame, wxWindowID id, const wxPoint& pos, const wxSize& size );
	virtual ~CGroupImgView( );

	void SelectImage( int nId );
	void UnselectAll();

// private methods
private:
	int m_bCustomSelect;

	// event handlers
	void OnPaint( wxPaintEvent& pEvent );
	void OnItemSelected( wxListEvent& pEvent );

// protected data
protected:
	DECLARE_EVENT_TABLE()

};


#endif
