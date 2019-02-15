////////////////////////////////////////////////////////////////////
// Name:        Wait Dialog Stack
// File:		waitdialogstack.cpp
// Purpose:     display a waiting window for the stacking process
//
// Created by:	Larry Lart on 16/12/2008
// Updated by:	Larry Lart on 31/01/2010
//
// Copyright:	(c) 2008-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////


// system headers
#include <stdlib.h>
#include <math.h>
#include <time.h>


// wxwindows header
#include "wx/wxprec.h"
//precompiled headers
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
// other wxwidgets 
#include <wx/dcbuffer.h>

// local headers
#include "../util/func.h"
#include "waitdialog.h"
#include "../unimap_worker.h"

// main header
#include "waitdialogstack.h"


// Class :: CStackImageView
////////////////////////////////////////////////////////////////////
// implement message map
BEGIN_EVENT_TABLE( CStackImageView, wxWindow )
	EVT_PAINT( CStackImageView::OnPaint )
	// size/move window events
	EVT_SIZE( CStackImageView::OnSize )
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CStackImageView
// Purose:	build my CObjectView object  
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CStackImageView::CStackImageView( wxWindow *pWindow, const wxPoint& pos, const wxSize& size ):
			wxWindow(pWindow, -1, pos, size, wxSIMPLE_BORDER, wxT( "Group stacking process view" ) )
{
	// get reference
	m_pParent = (CWaitDialogStack*) pWindow;

	//////////
	// init for auto buffered paint ???
	SetBackgroundStyle(  wxBG_STYLE_CUSTOM  );

	// set flags
	m_bSizeIsRunning = 0;
	m_bHasImage = 0;
	// set size
	m_nWidth = size.GetWidth();
	m_nHeight = size.GetHeight();

	// main canvas
	m_pBitmap = wxBitmap( m_nWidth, m_nHeight );
	// auxiliary canvases
	m_pBitmapSky = wxBitmap( m_nWidth, m_nHeight );

	//////////////////////////////////
	// sky map layer - TO MOVE DOWN
	wxMemoryDC sky_dc;
	sky_dc.SelectObject( m_pBitmapSky );
	
	// and clear
	sky_dc.SetBackground( *wxBLACK_BRUSH );
	sky_dc.Clear();

	m_bDrawing = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CStackImageView
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CStackImageView::~CStackImageView( )
{
//	m_pBitmap.Destroy( );
}

////////////////////////////////////////////////////////////////////
// Method:	OnPaint
// Class:	CStackImageView
// Purose:	on paint event
// Input:	reference to paint event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CStackImageView::OnPaint( wxPaintEvent& event )
{
	if( m_bDrawing ) return;
	m_bDrawing = 1;

//	wxPaintDC dc(this);
	// use this instead -- better ??
	wxAutoBufferedPaintDC dc(this);

	dc.BeginDrawing();
	dc.Clear( );

//	DrawImageView( );

	dc.DrawBitmap( m_pBitmap, 0 , 0 );

	dc.EndDrawing();
	
	m_bDrawing = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	DrawImageView
// Class:	CStackImageView
// Purose:	set position
// Input:	x,y orig and field width
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CStackImageView::SetImageView( wxImage* pImage )
{
	if( m_pParent->m_bImageUpdate == 0 ) return;
	m_pParent->m_bImageUpdate = 0;
	m_bDrawing = 1;

	int w = pImage->GetWidth();
	int h = pImage->GetHeight();
	double nScale = (double) m_nWidth/w;
	int nHeight = (int) h*nScale;

	wxImage pTempImg = pImage->Copy();
	pTempImg.Rescale( m_nWidth, nHeight );
	m_pBitmap = wxBitmap( pTempImg );

	Refresh( FALSE );
	Update( );

	m_bDrawing = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnSize
// Class:	CStackImageView
// Purose:	adjust on windows resize
// Input:	reference to size event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CStackImageView::OnSize( wxSizeEvent& even )
{
	even.Skip();

	m_nWidth = even.GetSize().GetWidth();
	m_nHeight = even.GetSize().GetHeight();

	// main canvas
	m_pBitmap = wxBitmap( m_nWidth, m_nHeight );
	// auxiliary canvases
	m_pBitmapSky = wxBitmap( m_nWidth, m_nHeight );

	//////////////////////////////////
	// sky map layer - TO MOVE DOWN
	wxMemoryDC sky_dc;
	sky_dc.SelectObject( m_pBitmapSky );
	
	// and clear
	sky_dc.SetBackground( *wxBLACK_BRUSH );
	sky_dc.Clear();

	return;
}

// Class :: CWaitDialogStack
////////////////////////////////////////////////////////////////////
// event table
BEGIN_EVENT_TABLE( CWaitDialogStack, wxDialog )
	EVT_INIT_DIALOG( CWaitDialogStack::OnInitDlg )
	EVT_BUTTON( wxID_BUTTON_CANCEL_STACK, CWaitDialogStack::OnStatusButton )
	EVT_TOGGLEBUTTON( wxID_BUTTON_PAUSE_STACK, CWaitDialogStack::OnStatusButton )
	EVT_BUTTON( wxID_BUTTON_STOP_STACK, CWaitDialogStack::OnStatusButton )
	EVT_BUTTON( wxID_BUTTON_SKIP_STACK, CWaitDialogStack::OnStatusButton )
	// custom events
	// custom messages
	EVT_COMMAND( wxID_WAIT_DIALOG_END_PROCESS, wxEVT_CUSTOM_MESSAGE_EVENT, CWaitDialogStack::OnEndProcess )
	EVT_COMMAND( wxID_WAIT_DIALOG_SET_MESSAGE, wxEVT_CUSTOM_MESSAGE_EVENT, CWaitDialogStack::OnSetMessage )

END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:		Constructor
// Class:		CWaitDialogStack
// Purpose:		Define a constructor
// Input:		pointer to frame & position
// Output:		nothing
////////////////////////////////////////////////////////////////////
CWaitDialogStack::CWaitDialogStack( wxWindow *parent, wxString& strTitle, wxString& strMsg, 
								   CUnimapWorker* pWorker, void* pTargetObject ):
 									wxDialog(parent, -1, strTitle, wxPoint( -1, -1 ), 
									wxSize( 300, 200 ), wxCAPTION|wxDIALOG_MODAL )
{
	m_pUnimapWorker = pWorker;
	m_pTargetObject = pTargetObject;

	m_strTitle = strTitle;
	// reset flags
	m_bImageUpdate = 0;
	m_bActionStatus = 0;
	m_bUpdating = 0;

	SetMinSize( wxSize( 200, 60 ) );
	// main dialog sizer
	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
//	wxFlexGridSizer* topsizer = new wxFlexGridSizer( 3, 1, 50, 5 );
	topsizer->SetMinSize( wxSize( 200, 20 ) );

	// header layout
	wxFlexGridSizer* pHeaderSizer = new wxFlexGridSizer( 1, 2, 5, 5 );
	pHeaderSizer->AddGrowableCol( 0 );
	topsizer->Add( pHeaderSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 5  );

	// data sizer
	wxFlexGridSizer* pDataSizer = new wxFlexGridSizer( 5, 2, 5, 5 );
	pDataSizer->AddGrowableCol( 1 );
	pDataSizer->AddGrowableRow( 0 );
	pHeaderSizer->Add( pDataSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 5  );
	// set data elements
	m_pPixX = new wxStaticText( this, -1, wxT("0"), wxPoint( -1, -1 ), wxSize( 80, -1 ) );
	m_pPixY = new wxStaticText( this, -1, wxT("0") );
	m_pCurrentValue = new wxStaticText( this, -1, wxT("0") );
	m_pStackMethod = new wxStaticText( this, -1, wxT("0") );
	m_pWidth = new wxStaticText( this, -1, wxT("0") );
	m_pHeight = new wxStaticText( this, -1, wxT("0") );
	m_pRemainingTime = new wxStaticText( this, -1, wxT("0") );
	// add to sizer
	// x ::
	pDataSizer->Add( new wxStaticText( this, -1, wxT("Pixel X: ")), 0, wxALIGN_RIGHT | wxALIGN_BOTTOM  );
	pDataSizer->Add( m_pPixX, 0, wxALIGN_LEFT | wxALIGN_BOTTOM );
	// y ::
	pDataSizer->Add( new wxStaticText( this, -1, wxT("Pixel Y: ")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pDataSizer->Add( m_pPixY, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// value ::
	pDataSizer->Add( new wxStaticText( this, -1, wxT("Value: ")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pDataSizer->Add( m_pCurrentValue, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// stack method ::
	pDataSizer->Add( new wxStaticText( this, -1, wxT("Method: ")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pDataSizer->Add( m_pStackMethod, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// width ::
	pDataSizer->Add( new wxStaticText( this, -1, wxT("Width: ")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pDataSizer->Add( m_pWidth, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// Height ::
	pDataSizer->Add( new wxStaticText( this, -1, wxT("Height: ")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pDataSizer->Add( m_pHeight, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// remainig time ETA::
	pDataSizer->Add( new wxStaticText( this, -1, wxT("ETA: ")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pDataSizer->Add( m_pRemainingTime, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// create skyview 
	m_pStackImageView = new CStackImageView( this, wxPoint( -1, -1 ), wxSize( 340, 230 ) );
	pHeaderSizer->Add( m_pStackImageView, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 5  );

	// use this message as a status ?
	m_pMessage = new wxStaticText( this, -1, strMsg );
	// add to the top sizer
	topsizer->Add( m_pMessage, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 5  );

	////////////////
	// buttons section
	m_pCancel = new wxButton( this, wxID_BUTTON_CANCEL_STACK, wxT("Cancel"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pPause = new wxToggleButton( this, wxID_BUTTON_PAUSE_STACK, wxT("Pause"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pStop = new wxButton( this, wxID_BUTTON_STOP_STACK, wxT("Stop"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pSkip = new wxButton( this, wxID_BUTTON_SKIP_STACK, wxT("Skip"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	// layout
	wxFlexGridSizer* pButtonsSizer = new wxFlexGridSizer( 1, 4, 5, 5 );
	pButtonsSizer->AddGrowableCol( 0 );
	pButtonsSizer->AddGrowableCol( 3 );
	// set elements in layout
	pButtonsSizer->Add( m_pCancel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	pButtonsSizer->Add( m_pPause, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	pButtonsSizer->Add( m_pStop, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	pButtonsSizer->Add( m_pSkip, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// add buttosn layout to teh top sizer
	topsizer->Add( pButtonsSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 5  );

	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);

	Centre( wxBOTH );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CWaitDialogStack
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CWaitDialogStack::~CWaitDialogStack( )
{
	delete( m_pPixX );
	delete( m_pPixY );
	delete( m_pCurrentValue );
	delete( m_pStackMethod );
	delete( m_pWidth );
	delete( m_pHeight );
	delete( m_pRemainingTime );

	delete( m_pStackImageView );
	delete( m_pMessage );
	delete( m_pCancel );
	delete( m_pPause );
	delete( m_pStop );
	delete( m_pSkip );
}

////////////////////////////////////////////////////////////////////
void CWaitDialogStack::OnInitDlg( wxInitDialogEvent& pEvent )
{
	pEvent.Skip();

//	m_pUnimapWorker->DoStartMatching( m_pAstroImage );

	DefCmdAction act;
	act.id = THREAD_ACTION_GROUP_STACK;
	act.handler = this->GetEventHandler();
	act.vectObj[0] = (void*) this;
	act.vectObj[1] = m_pTargetObject;

	m_pUnimapWorker->SetAction( act );

}

////////////////////////////////////////////////////////////////////
// Method:	SetImage
// Class:	CWaitDialogStack
// Purose:	set the field size
// Input:	ra, dec
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CWaitDialogStack::SetImage( wxImage* pImage )
{
	wxMutexGuiEnter();
	m_pStackImageView->SetImageView( pImage );
	wxMutexGuiLeave();

	// set update flag
	m_bImageUpdate = 1;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetMethod
// Class:	CWaitDialogStack
// Purose:	set stack method
// Input:	stack method string
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CWaitDialogStack::SetMethod( const wxString& strMethod )
{
	m_bUpdating = 1;

	wxMutexGuiEnter();
	m_pStackMethod->SetLabel( strMethod );
	wxMutexGuiLeave();

	m_bUpdating = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetPixelStatus
// Class:	CWaitDialogStack
// Purose:	set stack method
// Input:	stack method string
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CWaitDialogStack::SetPixelStatus( int x, int y, float value )
{
	m_bUpdating = 1;

	wxString strMsg;

	wxMutexGuiEnter();

	// :: set x
	strMsg.Printf( wxT("%d"), x );
	m_pPixX->SetLabel( strMsg );
	// :: set y
	strMsg.Printf( wxT("%d"), y );
	m_pPixY->SetLabel( strMsg );
	// :: set x
	strMsg.Printf( wxT("%.4f"), value );
	m_pCurrentValue->SetLabel( strMsg );

	wxMutexGuiLeave();

	m_bUpdating = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetImageSize
// Class:	CWaitDialogStack
// Purose:	set stack frame size
// Input:	width and height
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CWaitDialogStack::SetImageSize( int w, int h )
{
	m_bUpdating = 1;

	wxString strMsg;

	wxMutexGuiEnter();

	// :: set width
	strMsg.Printf( wxT("%d"), w );
	m_pWidth->SetLabel( strMsg );
	// :: set height
	strMsg.Printf( wxT("%d"), h );
	m_pHeight->SetLabel( strMsg );

	wxMutexGuiLeave();

	m_bUpdating = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetEta
// Class:	CWaitDialogStack
// Purose:	set eta=time estiamtion to finish method
// Input:	eta value in seconds
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CWaitDialogStack::SetEta( double nEta )
{
	m_bUpdating = 1;

	wxString strMsg;

	wxMutexGuiEnter();
	strMsg.Printf( wxT("%.2d:%.2d:%.2d"), (int)nEta/3600, (int)nEta/60, (int)nEta%60 );
	m_pRemainingTime->SetLabel( strMsg );
	wxMutexGuiLeave();

	m_bUpdating = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnStatusButton
// Class:		CWaitDialogStack
// Purpose:		when status button pressed
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CWaitDialogStack::OnStatusButton( wxCommandEvent& even )
{
	int nId = even.GetId();
	
	// set status by button
	if( nId == wxID_BUTTON_CANCEL_STACK )
		m_bActionStatus = WAIT_DIALOG_STACK_STATUS_CANCEL;
	else if( nId == wxID_BUTTON_PAUSE_STACK )
	{
		if( m_bActionStatus != WAIT_DIALOG_STACK_STATUS_PAUSE )
			m_bActionStatus = WAIT_DIALOG_STACK_STATUS_PAUSE;
		else
			m_bActionStatus = 0;

	} else if( nId == wxID_BUTTON_STOP_STACK )
		m_bActionStatus = WAIT_DIALOG_STACK_STATUS_STOP;
	else if( nId == wxID_BUTTON_SKIP_STACK )
		m_bActionStatus = WAIT_DIALOG_STACK_STATUS_SKIP;
	
	return;
}

////////////////////////////////////////////////////////////////////
void CWaitDialogStack::OnEndProcess( wxCommandEvent& pEvent )
{
	EndModal( 0 );
}

////////////////////////////////////////////////////////////////////
void CWaitDialogStack::OnSetMessage( wxCommandEvent& pEvent )
{
	wxString& strMsg = pEvent.GetString();
	m_pMessage->SetLabel( strMsg );
	GetSizer()->Layout( );
	Fit( );

}