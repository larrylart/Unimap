////////////////////////////////////////////////////////////////////
// Name:        Wait Dialog Match
// File:		waitdialogmatch.cpp
// Purpose:     display a waiting window for the match process
//
// Created by:	Larry Lart on 26/06/2008
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
//#include "../image/astroimage.h"
#include "../unimap_worker.h"

// main header
#include "waitdialogmatch.h"


// Class :: CSkyScanView
////////////////////////////////////////////////////////////////////
// implement message map
BEGIN_EVENT_TABLE( CSkyScanView, wxWindow )
	EVT_PAINT( CSkyScanView::OnPaint )
	// size/move window events
	EVT_SIZE( CSkyScanView::OnSize )
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyScanView
// Purose:	build my CObjectView object  
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CSkyScanView::CSkyScanView( wxWindow *pWindow, const wxPoint& pos, const wxSize& size ):
			wxWindow(pWindow, -1, pos, size, wxSIMPLE_BORDER, wxT( "Object matching process view" ) )
{
	// get reference
	m_pParent = (CWaitDialogMatch*) pWindow;

	//////////
	// init for auto buffered paint ???
	SetBackgroundStyle(  wxBG_STYLE_CUSTOM  );

	// set flags
	m_bSizeIsRunning = 0;
	m_bHasImage = 0;
	// reset vars 
	m_nObjects = 0;
	// set size
	m_nWidth = size.GetWidth();
	m_nHeight = size.GetHeight();

	// main canvas
	m_pBitmap = wxBitmap( m_nWidth, m_nHeight );
	// auxiliary canvases
	m_pBitmapTarget  = wxBitmap( m_nWidth, m_nHeight );
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
// Class:	CSkyScanView
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CSkyScanView::~CSkyScanView( )
{
//	m_pBitmap.Destroy( );
}

////////////////////////////////////////////////////////////////////
// Method:	OnPaint
// Class:	CSkyScanView
// Purose:	on paint event
// Input:	reference to paint event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CSkyScanView::OnPaint( wxPaintEvent& event )
{
	if( m_bDrawing ) return;
	m_bDrawing = 1;

//	wxPaintDC dc(this);
	// use this instead -- better ??
	wxAutoBufferedPaintDC dc(this);

	dc.BeginDrawing();
	dc.Clear( );

	if( m_pParent->m_bNewObjects == 1 ) DrawObjects( );
	if( m_pParent->m_bImageArea == 1 ) DrawImageArea( );

	DrawScanView( );

	dc.DrawBitmap( m_pBitmap, 0 , 0 );

	dc.EndDrawing();
	
	m_bDrawing = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	AddObject
// Class:	CSkyScanView
// Purose:	draw object
// Input:	x, y coord
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CSkyScanView::AddObject( int nX, int nY, unsigned char nState )
{
	if( m_nObjects >= MAX_NO_OF_WAIT_DIALOG_MATCH_OBJ ) return;

	m_vectObjects[m_nObjects].x = nX;
	m_vectObjects[m_nObjects].y = nY;
	m_vectObjectsState[m_nObjects] = nState;
	// increment counter
	m_nObjects++;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	DrawObjects
// Class:	CSkyScanView
// Purose:	draw object
// Input:	x, y coord
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CSkyScanView::DrawObjects( )
{
	int i = 0;

	wxMemoryDC sky_dc;
	sky_dc.SelectObject( m_pBitmapSky );
	
	// set pen
	sky_dc.SetPen( *wxGREEN );

	for( i = 0; i<m_nObjects; i++ )
	{
		// draw object if in range
		if( m_vectObjects[i].x <= m_nWidth && m_vectObjects[i].x >= 0 &&
			m_vectObjects[i].y <= m_nHeight && m_vectObjects[i].y >= 0 )
		{
			if( m_vectObjectsState[i] == 1 )
				sky_dc.SetPen( *wxGREEN );
			else
				sky_dc.SetPen( *wxRED );

			sky_dc.DrawCircle( m_vectObjects[i].x, m_vectObjects[i].y, 1 );
		}
	}

	m_pParent->m_bNewObjects = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	DrawImageArea
// Class:	CSkyScanView
// Purose:	draw image area 
// Input:	x, y , width, height
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CSkyScanView::DrawImageArea( )
{
	wxMemoryDC sky_dc;
	sky_dc.SelectObject( m_pBitmapSky );
	
	// set pen
	sky_dc.SetPen( wxPen( *wxBLUE, 1, wxDOT ) );
	// set brush
	sky_dc.SetBrush( *wxTRANSPARENT_BRUSH );
	// draw object
	sky_dc.DrawRectangle( m_pParent->m_nImageAreaX, m_pParent->m_nImageAreaY, 
							m_pParent->m_nImageAreaWidth, m_pParent->m_nImageAreaHeight );

	m_pParent->m_bImageArea = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	DrawScanView
// Class:	CSkyScanView
// Purose:	set position
// Input:	x,y orig and field width
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CSkyScanView::DrawScanView( )
{
	if( m_pParent->m_bScanUpdate == 0 ) return;
	m_pParent->m_bScanUpdate = 0;

	char strLabel[255];

	//////////////////////////
	// get & calculate local vars
	double nFactX = (double) (m_nWidth/m_pParent->m_nFieldWidth);
	double nFactY = (double) (m_nHeight/m_pParent->m_nFieldHeight);
	double x = (m_pParent->m_nOrigX-m_pParent->m_nOrigRa)*nFactX;
	double y = (m_pParent->m_nOrigY-m_pParent->m_nOrigDec)*nFactY;
	double w = m_pParent->m_nCutWidth*nFactX;
	double h = m_pParent->m_nCutHeight*nFactY;

	////////////////
	// get dc
	wxMemoryDC draw_dc;
	draw_dc.SelectObject( m_pBitmap );

	// copy star map over base grid
	draw_dc.DrawBitmap( m_pBitmapSky, 0 , 0 );

	///////////////////////////////////
	// draw best target
	// set pen
	draw_dc.SetPen( wxPen( *wxCYAN, 1, wxDOT ) );
	// set brush
	draw_dc.SetBrush( *wxTRANSPARENT_BRUSH );
	// draw object
	draw_dc.DrawRectangle( m_pParent->m_nBestTargetX,m_pParent->m_nBestTargetY, 
							m_pParent->m_nBestTargetWidth, m_pParent->m_nBestTargetHeight );

	// set brush
	draw_dc.SetBrush( *wxTRANSPARENT_BRUSH );
	// set pen
	draw_dc.SetPen( *wxRED );

	// and vertical line 
	draw_dc.DrawLine( (int) (x + 0.499), 0, (int) (x + 0.499), m_nHeight );
	// and horizontal line
	draw_dc.DrawLine( 0, (int) (y + 0.499), m_nWidth, (int) (y + 0.499) );
	//  and draw my search area
	draw_dc.DrawRectangle( (int) (x-w/2), (int) (y-h/2), w, h );

	// set font and draw axis text
//	draw_dc.SetFont( wxFont( 7, wxROMAN, wxNORMAL, wxNORMAL ) );
//	int nTextW = 0, nTextH = 0;
//	dc.GetTextExtent( strAngle, &nTextW, &nTextH )'
//	sprintf( strLabel, "%d", (int) x );
//	draw_dc.DrawText( strLabel , (int) x+GRID_HORIZONTAL_MARGIN + 3, 2 );

//	Refresh();

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnSize
// Class:	CSkyScanView
// Purose:	adjust on windows resize
// Input:	reference to size event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CSkyScanView::OnSize( wxSizeEvent& even )
{
	even.Skip();

	m_nWidth = even.GetSize().GetWidth();
	m_nHeight = even.GetSize().GetHeight();

	// main canvas
	m_pBitmap = wxBitmap( m_nWidth, m_nHeight );
	// auxiliary canvases
	m_pBitmapTarget  = wxBitmap( m_nWidth, m_nHeight );
	m_pBitmapSky = wxBitmap( m_nWidth, m_nHeight );

	//////////////////////////////////
	// sky map layer - TO MOVE DOWN
	wxMemoryDC sky_dc;
	sky_dc.SelectObject( m_pBitmapSky );
	
	// and clear
	sky_dc.SetBackground( *wxBLACK_BRUSH );
	sky_dc.Clear();

	m_pParent->m_bNewObjects = 1;
	m_pParent->m_bImageArea = 1;

	return;
}

// Class :: CWaitDialogMatch
////////////////////////////////////////////////////////////////////
// event table
BEGIN_EVENT_TABLE( CWaitDialogMatch, wxDialog )
	EVT_INIT_DIALOG( CWaitDialogMatch::OnInitDlg )
	EVT_BUTTON( wxID_BUTTON_CANCEL_MATCH, CWaitDialogMatch::OnStatusButton )
	EVT_TOGGLEBUTTON( wxID_BUTTON_PAUSE_MATCH, CWaitDialogMatch::OnStatusButton )
	EVT_BUTTON( wxID_BUTTON_STOP_MATCH, CWaitDialogMatch::OnStatusButton )
	EVT_BUTTON( wxID_BUTTON_SKIP_MATCH, CWaitDialogMatch::OnStatusButton )
	// custom messages
	EVT_COMMAND( wxID_WAIT_DIALOG_END_PROCESS, wxEVT_CUSTOM_MESSAGE_EVENT, CWaitDialogMatch::OnEndProcess )
	EVT_COMMAND( wxID_WAIT_DIALOG_SET_MESSAGE, wxEVT_CUSTOM_MESSAGE_EVENT, CWaitDialogMatch::OnSetMessage )
	EVT_COMMAND( wxID_WAIT_DIALOG_SET_TITLE, wxEVT_CUSTOM_MESSAGE_EVENT, CWaitDialogMatch::OnSetTitle )
	EVT_COMMAND( wxID_WAIT_DIALOG_SET_UPDATE, wxEVT_CUSTOM_MESSAGE_EVENT, CWaitDialogMatch::OnSetUpdate )

END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////
CWaitDialogMatch::CWaitDialogMatch( wxWindow *parent, const wxString& strTitle, const wxString& strMsg, 
								   CUnimapWorker* pWorker, void* pTargetObject, int bGroup ):
 									wxDialog(parent, -1, strTitle, wxPoint( -1, -1 ), 
									wxSize( 300, 200 ), wxCAPTION|wxDIALOG_MODAL )
{
	m_pUnimapWorker = pWorker;
	m_pTargetObject = pTargetObject;
	m_bGroup = bGroup;

	m_strTitle = strTitle;
	// reset vars
	m_nOrigRa = 0;
	m_nOrigDec = 0;
	m_nFieldWidth = 0;
	m_nFieldHeight = 0;
	m_nOrigX = 0;
	m_nOrigY = 0;
	m_nCutWidth = 0;
	m_nCutHeight = 0;
	// reset flags
	m_bScanUpdate = 0;
	m_bFoundBestTarget = 0;
	m_bNewObjects = 0;
	m_bImageArea = 0;
	m_bActionStatus = 0;
	m_bUpdating = 0;

	SetMinSize( wxSize( 200, 60 ) );
	// main dialog sizer
	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
//	wxFlexGridSizer* topsizer = new wxFlexGridSizer( 3, 1, 50, 5 );
	topsizer->SetMinSize( wxSize( 200, 20 ) );

	// catalog sizer
	wxFlexGridSizer* pCatalogSizer = new wxFlexGridSizer( 1, 2, 5, 5 );
	pCatalogSizer->AddGrowableCol( 0 );
	topsizer->Add( pCatalogSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 5  );

	// header layout
	wxFlexGridSizer* pHeaderSizer = new wxFlexGridSizer( 1, 2, 5, 5 );
	pHeaderSizer->AddGrowableCol( 0 );
	topsizer->Add( pHeaderSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 5  );

	// data sizer
	wxFlexGridSizer* pDataSizer = new wxFlexGridSizer( 5, 2, 5, 5 );
	pDataSizer->AddGrowableCol( 1 );
	pDataSizer->AddGrowableRow( 0 );
	pHeaderSizer->Add( pDataSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 5  );

	/////////////////////////
	// set data elements
	m_pCatalog = new wxStaticText( this, -1, _T("N/A"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pRa = new wxStaticText( this, -1, _T("0"), wxPoint( -1, -1 ), wxSize( 80, -1 ) );
	m_pDec = new wxStaticText( this, -1, _T("0") );
	m_pCutWidth = new wxStaticText( this, -1, _T("0") );
	m_pCutHeight = new wxStaticText( this, -1, _T("0") );
	m_pCutObjects = new wxStaticText( this, -1, _T("0") );
	m_pFoundBest = new wxStaticText( this, -1, _T("0") );
	m_pFoundBestAll = new wxStaticText( this, -1, _T("0") );
	m_pCurrentFound = new wxStaticText( this, -1, _T("0") );
	m_pMostMatched = new wxStaticText( this, -1, _T("0") );

	///////////////////////////
	// LAYOUT
	// :: catalog
	pCatalogSizer->Add( new wxStaticText( this, -1, _T("Use Catalog: ")), 0, wxALIGN_RIGHT | wxALIGN_BOTTOM  );
	pCatalogSizer->Add( m_pCatalog, 0, wxALIGN_LEFT | wxALIGN_BOTTOM );

	// ra ::
	pDataSizer->Add( new wxStaticText( this, -1, _T("Target RA: ")), 0, wxALIGN_RIGHT | wxALIGN_BOTTOM  );
	pDataSizer->Add( m_pRa, 0, wxALIGN_LEFT | wxALIGN_BOTTOM );
	// dec ::
	pDataSizer->Add( new wxStaticText( this, -1, _T("Target DEC: ")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pDataSizer->Add( m_pDec, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// cut width ::
	pDataSizer->Add( new wxStaticText( this, -1, _T("Target Width: ")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pDataSizer->Add( m_pCutWidth, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// cut height ::
	pDataSizer->Add( new wxStaticText( this, -1, _T("Target Height: ")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pDataSizer->Add( m_pCutHeight, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// cut objects ::
	pDataSizer->Add( new wxStaticText( this, -1, _T("Catalog Objects: ")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pDataSizer->Add( m_pCutObjects, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// found best ::
	pDataSizer->Add( new wxStaticText( this, -1, _T("Best Match: ")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pDataSizer->Add( m_pFoundBest, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// found best all::
	pDataSizer->Add( new wxStaticText( this, -1, _T("Best Match All: ")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pDataSizer->Add( m_pFoundBestAll, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// current match ::
	pDataSizer->Add( new wxStaticText( this, -1, _T("Current Match: ")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	pDataSizer->Add( m_pCurrentFound, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// most match ::
	pDataSizer->Add( new wxStaticText( this, -1, _T("Most Matched: ")), 0, wxALIGN_RIGHT | wxALIGN_TOP  );
	pDataSizer->Add( m_pMostMatched, 0, wxALIGN_LEFT | wxALIGN_TOP );

	// create skyview 
	m_pSkyScanView = new CSkyScanView( this, wxPoint( -1, -1 ), wxSize( 300, 230 ) );
	pHeaderSizer->Add( m_pSkyScanView, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 5  );

	// use this message as a status ?
	m_pMessage = new wxStaticText( this, -1, strMsg );
	// add to the top sizer
	topsizer->Add( m_pMessage, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 5  );

	////////////////
	// buttons section
	m_pCancel = new wxButton( this, wxID_BUTTON_CANCEL_MATCH, wxT("Cancel"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pPause = new wxToggleButton( this, wxID_BUTTON_PAUSE_MATCH, wxT("Pause"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pStop = new wxButton( this, wxID_BUTTON_STOP_MATCH, wxT("Stop"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pSkip = new wxButton( this, wxID_BUTTON_SKIP_MATCH, wxT("Skip"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	// layout
	wxFlexGridSizer* pButtonsSizer = new wxFlexGridSizer( 1, 4, 5, 5 );
	pButtonsSizer->AddGrowableCol( 0 );
	pButtonsSizer->AddGrowableCol( 3 );
	// set elements in layout
	pButtonsSizer->Add( m_pCancel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	pButtonsSizer->Add( m_pPause, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	pButtonsSizer->Add( m_pStop, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	pButtonsSizer->Add( m_pSkip, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// add buttons layout to teh top sizer
	topsizer->Add( pButtonsSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 5  );

	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);

	Centre( wxBOTH );
}

////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////
CWaitDialogMatch::~CWaitDialogMatch( )
{
	delete( m_pCatalog );
	delete( m_pRa );
	delete( m_pDec );
	delete( m_pCutWidth );
	delete( m_pCutHeight );
	delete( m_pCutObjects );
	delete( m_pFoundBest );
	delete( m_pFoundBestAll );
	delete( m_pCurrentFound );
	delete( m_pMostMatched );

	delete( m_pSkyScanView );
	delete( m_pMessage );
	delete( m_pCancel );
	delete( m_pPause );
	delete( m_pStop );
	delete( m_pSkip );
}

////////////////////////////////////////////////////////////////////
void CWaitDialogMatch::OnInitDlg( wxInitDialogEvent& pEvent )
{
	pEvent.Skip();

//	m_pUnimapWorker->DoStartMatching( m_pAstroImage );

	DefCmdAction act;
	act.handler = this->GetEventHandler();
	act.vectObj[0] = m_pTargetObject;
	act.vectObj[1] = (void*) this; 

	// check if this dialog is intended for a group or not
	if( !m_bGroup )
		act.id = THREAD_ACTION_MATCH_START;
	else
		act.id = THREAD_ACTION_GROUP_MATCH;

	m_pUnimapWorker->SetAction( act );

//	if( m_pUnimapWorker && m_rWorkerAct.id >= 0 ) 
//		m_pUnimapWorker->SetAction( m_rWorkerAct );
}

////////////////////////////////////////////////////////////////////
void CWaitDialogMatch::SetCatalog( const wxString& strCat )
{
	m_pCatalog->SetLabel( strCat );
}

////////////////////////////////////////////////////////////////////
// Method:	SetField
// Class:	CWaitDialogMatch
// Purose:	set the field size
// Input:	ra, dec
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CWaitDialogMatch::SetField( double nOrigRa, double nOrigDec, double nWidth, double nHeight )
{
	wxString strWidth;
	wxString strHeight;

	m_nOrigRa = nOrigRa;
	m_nOrigDec = nOrigDec;
	m_nFieldWidth = nWidth;
	m_nFieldHeight = nHeight;

	// set title
	DegToHHMMSS( m_nFieldWidth, strWidth );
	DegToHHMMSS( m_nFieldHeight, strHeight );
	m_strTitle = m_strTitle + wxT(" :: field ") + strWidth + wxT(" x ") + strHeight;
	SetTitle( m_strTitle );

	// set update flag
	m_bScanUpdate = 1;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetTarget
// Class:	CWaitDialogMatch
// Purose:	set position
// Input:	x,y orig and field width, height, number of objects in cut
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CWaitDialogMatch::SetTarget( double x, double y, double w, double h, int nObj )
{
	m_bUpdating = 1;

	wxString strMsg;

	wxMutexGuiEnter();

	m_nOrigX = x;
	m_nOrigY = y;
	m_nCutWidth = w;
	m_nCutHeight = h;

	// set labels
	// ra ::
	RaDegToSexagesimal( m_nOrigX, strMsg );
	m_pRa->SetLabel( strMsg );
	// dec ::
	DecDegToSexagesimal( m_nOrigY, strMsg );
	m_pDec->SetLabel( strMsg );
	// cut width ::
	DegToHHMMSS( m_nCutWidth, strMsg );
	m_pCutWidth->SetLabel( strMsg );
	// cut height ::
	DegToHHMMSS( m_nCutHeight, strMsg );
	m_pCutHeight->SetLabel( strMsg );
	// set no of object
	strMsg.Printf( wxT("%d"), nObj );
	m_pCutObjects->SetLabel( strMsg );

//	Update();
//	Refresh();

	wxMutexGuiLeave();

	// set update flag
	m_bScanUpdate = 1;

	m_bUpdating = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetFound
// Class:	CWaitDialogMatch
// Purose:	set number of objects found
// Input:	hwo many were found
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CWaitDialogMatch::SetFound( int nFound )
{
	m_bUpdating = 1;

	wxString strMsg;

	wxMutexGuiEnter();

	// set objects found
	strMsg.Printf( wxT("%d"), nFound );
	m_pCurrentFound->SetLabel( strMsg );

	wxMutexGuiLeave();

	m_bUpdating = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetBestMatch
// Class:	CWaitDialogMatch
// Purose:	set best match
// Input:	hwo many were found
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CWaitDialogMatch::SetBestMatch( int nFound, int nFoundAll  )
{
	m_bUpdating = 1;

	wxString strMsg;

	wxMutexGuiEnter();

	// set best
	strMsg.Printf( wxT("%d"), nFound );
	m_pFoundBest->SetLabel( strMsg );
	// set best all
	strMsg.Printf( wxT("%d"), nFoundAll );
	m_pFoundBestAll->SetLabel( strMsg );

	wxMutexGuiLeave();

	m_bUpdating = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetMostMatched
// Class:	CWaitDialogMatch
// Purose:	set best match
// Input:	hwo many were found
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CWaitDialogMatch::SetMostMatched( int nFound )
{
	m_bUpdating = 1;

	wxString strMsg;

	wxMutexGuiEnter();

	// set objects found
	strMsg.Printf( wxT("%d"), nFound );
	m_pMostMatched->SetLabel( strMsg );

	wxMutexGuiLeave();

	m_bUpdating = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetObjects
// Class:	CWaitDialogMatch
// Purose:	set object for image
// Input:	pointer to objects vector, no of objects
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CWaitDialogMatch::SetObjects( StarDef* vectStar, int nStar, 
								double nMinX, double nMaxX, 
								double nMinY, double nMaxY, double nRemScale )
{
	int i = 0;

	wxMutexGuiEnter();

	// calculate big win width
	double nBigWidth = (double) ( nMaxX - nMinX );
	double nBigHeight = (double) ( nMaxY - nMinY );
	// calculate scale to this win
	double nScale = (double) ( m_pSkyScanView->m_nWidth/nBigWidth );
	// calculte new view height and rescale 
	int nNewHeight = (int) ( nBigHeight*nScale + 0.499 );
	m_pSkyScanView->SetMinSize( wxSize( m_pSkyScanView->m_nWidth, nNewHeight ) );
//	m_pSkyScanView->Layout();
	m_pSkyScanView->m_nHeight = nNewHeight;

	int nState = 0;
	for( i=0; i<nStar; i++ )
	{
		int nX = (int) ( (vectStar[i].x/nRemScale-nMinX)*nScale + 0.499 );
		int nY = (int) ( (vectStar[i].y/nRemScale-nMinY)*nScale + 0.499 );

		if( vectStar[i].ra > 180 ) 
			nState = 1;
		else
			nState = 0;
		// call to draw
		m_pSkyScanView->AddObject( nX, nY, nState );
	}

	wxMutexGuiLeave();
	// set flag
	m_bNewObjects = 1;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetImageAreaFactor
// Class:	CWaitDialogMatch
// Purose:	set image based on multiplication ratio
// Input:	ration on x and Y
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CWaitDialogMatch::SetImageAreaFactor( double nXRatio, double nYRatio )
{
	int i = 0;
	
	// calculate image rectangle
	m_nImageAreaWidth = (int) (m_pSkyScanView->m_nWidth/nXRatio );
	m_nImageAreaHeight = (int) (m_pSkyScanView->m_nHeight/nYRatio );
	m_nImageAreaX = (int) ( (m_pSkyScanView->m_nWidth/2) - m_nImageAreaWidth/2 + 0.499 );
	m_nImageAreaY = (int) ( (m_pSkyScanView->m_nHeight/2) - m_nImageAreaHeight/2 + 0.499 );

	m_bImageArea = 1;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetBestTarget
// Class:	CWaitDialogMatch
// Purose:	draw best target area
// Input:	x,y orig and field width, height
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CWaitDialogMatch::SetBestTarget( double x, double y, double w, double h )
{

	//////////////////////////
	// calculate best target coord
	double nFactX = (double) m_pSkyScanView->m_nWidth/m_nFieldWidth;
	double nFactY = (double) m_pSkyScanView->m_nHeight/m_nFieldHeight;

	m_nBestTargetWidth = (int) ( w*nFactX );
	m_nBestTargetHeight = (int) ( h*nFactY );
	m_nBestTargetX = (int) ( x*nFactX - m_nBestTargetWidth/2 + 0.499 );
	m_nBestTargetY = (int) ( y*nFactY - m_nBestTargetHeight/2 + 0.499 );

	// also set flag
	m_bFoundBestTarget = 1;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnStatusButton
// Class:		CWaitDialogMatch
// Purpose:		when status button pressed
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CWaitDialogMatch::OnStatusButton( wxCommandEvent& even )
{
	int nId = even.GetId();
	
	// set status by button
	if( nId == wxID_BUTTON_CANCEL_MATCH )
		m_bActionStatus = WAIT_DIALOG_MATCH_STATUS_CANCEL;
	else if( nId == wxID_BUTTON_PAUSE_MATCH )
	{
		if( m_bActionStatus != WAIT_DIALOG_MATCH_STATUS_PAUSE )
			m_bActionStatus = WAIT_DIALOG_MATCH_STATUS_PAUSE;
		else
			m_bActionStatus = 0;

	} else if( nId == wxID_BUTTON_STOP_MATCH )
		m_bActionStatus = WAIT_DIALOG_MATCH_STATUS_STOP;
	else if( nId == wxID_BUTTON_SKIP_MATCH )
		m_bActionStatus = WAIT_DIALOG_MATCH_STATUS_SKIP;
	
	return;
}

////////////////////////////////////////////////////////////////////
void CWaitDialogMatch::OnEndProcess( wxCommandEvent& pEvent )
{
	EndModal( 0 );
}

////////////////////////////////////////////////////////////////////
void CWaitDialogMatch::OnSetTitle( wxCommandEvent& pEvent )
{
	m_strTitle = pEvent.GetString();
	this->SetTitle( m_strTitle );
	GetSizer()->Layout( );
	Fit( );

}

////////////////////////////////////////////////////////////////////
void CWaitDialogMatch::OnSetMessage( wxCommandEvent& pEvent )
{
	wxString& strMsg = pEvent.GetString();
	m_pMessage->SetLabel( strMsg );
	GetSizer()->Layout( );
	Fit( );

}

////////////////////////////////////////////////////////////////////
void CWaitDialogMatch::OnSetUpdate( wxCommandEvent& pEvent )
{
	Layout();
	Update();
	Refresh();
}
