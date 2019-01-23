
// system header
#include <math.h>
#include <stdio.h>

#include "wx/wxprec.h"
#include <wx/image.h>
//precompiled headers
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
// other wxwidgets 
#include <wx/dcbuffer.h>
#include <wx/app.h>

// other headers
#include "../sky/star.h"
#include "../unimap.h"
#include "frame.h"
#include "../util/func.h"
#include "../image/astroimage.h"

// main header
#include "groupview.h"

// class :: CGroupImgView
///////////////////////////////////////////////
// implement message map
BEGIN_EVENT_TABLE( CGroupImgView, wxListCtrl )
//	EVT_PAINT( CGroupImgView::OnPaint )
	EVT_LIST_ITEM_SELECTED( -1, CGroupImgView::OnItemSelected )
END_EVENT_TABLE()


////////////////////////////////////////////////////////////////////
// Method:	Constructor
////////////////////////////////////////////////////////////////////
CGroupImgView::CGroupImgView( wxWindow *frame, wxWindowID id, const wxPoint& pos, const wxSize& size ):
			wxListCtrl(frame, id, pos, size, wxLC_ICON |wxLC_SINGLE_SEL )
{
	m_bCustomSelect = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
////////////////////////////////////////////////////////////////////
CGroupImgView::~CGroupImgView( )
{

}


// Method:	OnPaint
////////////////////////////////////////////////////////////////////
void CGroupImgView::OnPaint( wxPaintEvent& pEvent )
{

}

// this to capture select event from manual select
////////////////////////////////////////////////////////////////////
void CGroupImgView::OnItemSelected( wxListEvent& pEvent )
{
	if( m_bCustomSelect )
		m_bCustomSelect = 0;
	else
		pEvent.Skip();
}

// Method:	SelectGroup
////////////////////////////////////////////////////////////////////
void CGroupImgView::SelectImage( int nId )
{
	//wxPoint pt;
	//GetItemPosition( nSelect-1, pt );
	//ScrollList( 0, pt.y );

	EnsureVisible( nId );

	// check if already selected skip
	if( GetItemState( nId, wxLIST_STATE_SELECTED ) ) return;

	// also select
//		SetItemState( nSelect-1, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE  );
	//SetItemState( nId, 0, wxLIST_STATE_SELECTED );
	//UnselectAll();
	m_bCustomSelect = 1;
	SetItemState( nId, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED );
}

void CGroupImgView::UnselectAll()
{
	for (int i=0; i<GetItemCount(); i++)
	{
		SetItemState(i, 0, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED);
	}

}
