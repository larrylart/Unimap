////////////////////////////////////////////////////////////////////
// Name:		implementation of the CDynamicImgView class 
// File:		dynamic_img_view.cpp
// Purpose:		dynamic image view methods
//
// Created by:	Larry Lart on 22/02/2004
// Updated by:	Larry Lart on 17/02/2010
//				
// Copyright:	(c) 2004-2010 Larry Lart
// Licence:		Digital Entity 
////////////////////////////////////////////////////////////////////

///////////////// DEBUG ///////////////////////
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
///////////////////////////////////////////////

// system header
#include <math.h>
#include <stdio.h>

//#include "highgui.h"
//#include "cv.h"
//#include "cvaux.h"

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include "wx/wxprec.h"
//precompiled headers
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
// other wxwidgets 
#include <wx/image.h>
#include <wx/dcbuffer.h>
#include <wx/app.h>
#include <wx/thread.h>

// ELX
#include <elx/core/CoreMacros.h>
#include <elx/math/TransfertFunction.h>
#include <elx/image/AbstractImageManager.h>
#include <elx/image/ImageFileManager.h>
#include <elx/math/MathCore.h>
#include <elx/image/ImageVariant.h>
#include <elx/image/ImageOperatorsImpl.h>
#include <elx/image/PixelIterator.h>

// other headers
#include "../unimap.h"
#include "../unimap_worker.h"
#include "../util/func.h"
#include "../util/geometry.h"
#include "../graphics/spline/Spline.h"
#include "frame.h"

// main header
#include "dynamic_img_view.h"

// externals
//extern float g_vectCtg[];
// get external unimap
DECLARE_APP(CUnimap)

// class :: CDynamicImgView
///////////////////////////////////////////////
// implement message map
BEGIN_EVENT_TABLE( CDynamicImgView, wxWindow )
	EVT_PAINT( CDynamicImgView::OnPaint )
	EVT_MOUSEWHEEL( CDynamicImgView::OnMouseWheel )
	EVT_LEFT_DCLICK( CDynamicImgView::OnMouseLDClick )
//	EVT_RIGHT_DCLICK( CImgView::OnMouseRDClick )
	EVT_RIGHT_UP( CDynamicImgView::OnRightClick )
	EVT_LEFT_DOWN( CDynamicImgView::OnLeftClickDown )
	EVT_LEFT_UP( CDynamicImgView::OnLeftClickUp )
	EVT_MOTION( CDynamicImgView::OnMouseMove )
	EVT_ENTER_WINDOW( CDynamicImgView::OnMouseEnter )
	// key events
	EVT_KEY_DOWN( CDynamicImgView::OnKeyDown )
	EVT_KEY_UP( CDynamicImgView::OnKeyUp )
	// window events
	EVT_SIZE( CDynamicImgView::OnSize ) 
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
////////////////////////////////////////////////////////////////////
CDynamicImgView::CDynamicImgView( wxWindow *frame, const wxPoint& pos, const wxSize& size ):
			wxWindow(frame, -1, pos, size, wxSIMPLE_BORDER )
{
	//////////
	// init for auto buffered paint ???
//	SetBackgroundStyle(  wxBG_STYLE_CUSTOM  );

	m_pUnimap = &(wxGetApp());

	int i = 0;
//	m_pImgViewMenu = NULL;
	m_dc = NULL;
	m_pImage = NULL;
	m_nInUseTool = IN_USE_TOOL_DEFAULT;
	m_bZoomDirection = 0;
	m_bDragWithZoomTool = 0;

	m_nCurrentCursorId = -1;
	m_nLastCursorId = -1;

	// reset all flags to default
	ResetData( );

	// set my canvas width/height
	m_nWidth = size.GetWidth( );
	m_nHeight = size.GetHeight( );
	

	// set structure colors defaults
	wxDefaultCol.Set( 0, 0, 0 );
	wxDefaultPen.SetColour( wxDefaultCol );
	wxDefaultBrush.SetColour( wxDefaultCol );

	// create mutex to control draw view bmp
	s_mutexViewBmp = new wxMutex( );

	// set my custom cursor
	SetTCursor( );
}

////////////////////////////////////////////////////////////////////
void CDynamicImgView::SetTCursor( int nId )
{
	// special case
	if( nId == UNIMAP_CURSOR_IMAGE_DEFAULT ) m_bZoomDirection = 1;

	// keep ids
	m_nLastCursorId = m_nCurrentCursorId;
	m_nCurrentCursorId = nId;

	SetCursor( *(m_pUnimap->GetCursor( nId )) );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
////////////////////////////////////////////////////////////////////
CDynamicImgView::~CDynamicImgView( )
{
//	if( m_pImgViewMenu ) delete( m_pImgViewMenu );
	FreeImage( );
	delete( s_mutexViewBmp );
	if( m_dc ) delete(m_dc);
}

////////////////////////////////////////////////////////////////////
void CDynamicImgView::FreeImage( )
{
	if( m_pImage && m_bSharedImage == 0 )
	{
		if( m_pImage->IsOk() ) m_pImage->Destroy( );
//		if( m_pImage ) delete( m_pImage );
	}
	m_pImage = NULL;
	m_bHasImage = 0;
	ClearBackground();
	// reset data
	ResetData( );

	if( HasCapture() ) ReleaseMouse();
}

////////////////////////////////////////////////////////////////////
void CDynamicImgView::ResetData( )
{
	m_nPicOrigX = 0;
	m_nPicOrigY = 0;
	m_nPicEndX = 0;
	m_nPicEndY = 0;
	m_nBasePicOrigX = 0;
	m_nBasePicOrigY = 0;
	m_nPicWidth = 0;
	m_nPicHeight = 0;
	m_nBasePicWidth = 0;
	m_nBasePicHeight = 0;
	m_nOrigPicWidth = 0;
	m_nOrigPicHeight = 0;
	m_nOrigPicWinRatio = 0;
	m_nCutWinRatio = 0;
	m_nCutX = 0;
	m_nCutY = 0;
	m_nCutWidth = 0;
	m_nCutHeight = 0;
	m_nZoomX = 0;
	m_nZoomY = 0;
	// test
	m_nLastZoomX = -1;
	m_nLastZoomY = -1;
	nLastNX = 0;
	nLastNY = 0;
	nOrigNX = 0;
	nOrigNY = 0;
	m_nLastZoomFactor = 0.0;
	m_nLastXMarg = 0;
	m_nLastYMarg = 0;
	m_nBasePicOrigXShift = 0;
	m_nBasePicOrigYShift = 0;
	m_nScaleFactor = 0;
	m_nCurrentMouseX = 0;
	m_nCurrentMouseY = 0;
	m_nPicRightClickPosX = -1;
	m_nPicRightClickPosY = -1;
	m_nMouseWinX = 0;
	m_nMouseWinY = 0;
	m_nMouseWinPicX = 0;
	m_nMouseWinPicY = 0;
	m_nMousePicX = 0;
	m_nMousePicY = 0;
	m_nDragLastX = 0;
	m_nDragLastY = 0;
	m_nStartDragX = 0;
	m_nStartDragY = 0;
	// old stuff ... still used ??? - i think in full screen
	m_nOldWidth = 0;
	m_nOldHeight = 0;
	m_nOldX = 0;
	m_nOldY = 0;
	// reset other
	m_bDrawing = false;
	m_nZoomCount = 0.5;
	m_bHasImage = 0;
	m_bDragging = 0;

	// area
	m_nAreaSelected = -1;
	m_nAreaEdit = -1;
	m_bAreaMove = 0;
	m_AreaResizeVertex = -1;
}

////////////////////////////////////////////////////////////////////
// Method:	OnPaint
// Class:	CDynamicImgView
// Purose:	on paint event
// Input:	reference to paint event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::OnPaint( wxPaintEvent& pEvent )
{
	if( !m_bHasImage )
	{
		wxPaintDC dc(this);
		dc.Clear( );
		Draw( dc );

		//////////////////////////
		// DRAW DRAG TOOL ZOOM AREA
//		DrawZoomToArea( dc );

	} else
	{
		// use this instead -- better ??
		wxAutoBufferedPaintDC dc(this);

		// version 2 - try antialaising
		m_dc = wxGraphicsContext::Create( (wxAutoBufferedPaintDC&) dc );

		Draw( dc );

		//////////////////////////
		// DRAW DRAG TOOL ZOOM AREA
		if( ( m_nInUseTool == IN_USE_TOOL_ZOOM || m_nInUseTool == IN_USE_TOOL_ADD_AREA ) && 
			m_bDragging && !m_bDragWithZoomTool )
		{
			DrawDragArea( dc );
		}

		delete(m_dc);
		m_dc = NULL;
	}
}

////////////////////////////////////////////////////////////////////
bool CDynamicImgView::SetGdcPropsFromDc( wxDC& dc )
{
	if( m_dc ) 
	{
		m_dc->SetPen( dc.GetPen() );
		m_dc->SetBrush( dc.GetBrush() );
		m_dc->SetFont( dc.GetFont(), dc.GetTextForeground() );

		return(true);

	} else
		return(false);
}

////////////////////////////////////////////////////////////////////
// Method:	Draw
// Class:	CDynamicImgView
// Purose:	image canvas drawing
// Input:	reference to dc
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::Draw( wxDC& dc, int bSize, int bUpdate )
{
	int i = 0;

	// check if dc available
	if( !dc.Ok( ) || m_bDrawing == false ){ return; }

	m_bDrawing = true;

	///////////////////////////
	// if size adjust to size - to use mainly for printing 
	if( bSize )
	{
		// ????? - do something or clean
	}

//	dc.BeginDrawing();
	dc.Clear( );

	int x,y,w,h;
	dc.GetClippingBox( &x, &y, &w, &h );

	//////////////////////////////////////////
	// if there is a new image to draw
	if( m_bNewImage )
	{
		s_mutexViewBmp->Lock();
		dc.DrawBitmap( m_pBitmap, m_nPicOrigX+x, m_nPicOrigY+y );
		s_mutexViewBmp->Unlock();

/*		wxMemoryDC memDC;
		memDC.SelectObject( m_pBitmap );
		dc.Blit( m_nPicOrigX+x, m_nPicOrigY+y, m_nPicWidth, m_nPicHeight, 
					&memDC, 0, 0, wxCOPY, FALSE);
*/

//			m_bNewImage = false;
	} else
	{
		// draw inter frame ?
	}

//	dc.EndDrawing();
//	m_bDrawing = false;

	//	Refresh( FALSE );
	if( bUpdate ) Update( );

	return;
}

////////////////////////////////////////////////////////////////////
void CDynamicImgView::DrawDragArea( wxDC& dc )
{
	// draw zoom to area
	int w = m_nDragLastX-m_nStartDragX;
	int h = m_nDragLastY-m_nStartDragY;

	// if add area 
	if( m_nInUseTool == IN_USE_TOOL_ADD_AREA )
	{
		wxPen mypen( *wxRED, 1, wxDOT );
		dc.SetPen( mypen );
		//wxBrush mybrush( *wxRED, wxBDIAGONAL_HATCH );
		//dc.SetBrush( mybrush );
		dc.SetBrush( *wxTRANSPARENT_BRUSH );
		dc.DrawRectangle( m_nStartDragX, m_nStartDragY, w, h );

	} else
	{
		wxPen mypen(*wxGREEN, 1, wxDOT);
		dc.SetPen( mypen );
		dc.SetBrush( *wxTRANSPARENT_BRUSH );
		dc.DrawRectangle( m_nStartDragX, m_nStartDragY, w, h );
	}
}

// draw image area - todo: move this to dynamic image
////////////////////////////////////////////////////////////////////
void CDynamicImgView::DrawArea( wxDC& dc, DefImageArea& area, int nType )
{
	////////////////////////////////
	// first we calculate position - size etc
	double x = area.x;
	double y = area.y;
	double w = area.width;
	double h = area.height;

	// scale area data from picture size to win size
	double nX = x*m_nScaleFactor;
	double nY = y*m_nScaleFactor;
	double nW = w*m_nScaleFactor;
	double nH = h*m_nScaleFactor;
	// if there is zoom in 
	if( m_nZoomCount > 0.5 )
	{
		nX = (int) round( (nX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
		nY = (int) round( (nY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
		nW = nW*m_nLastZoomFactor;
		nH = nH*m_nLastZoomFactor;
	}
	// adjust with picture orig x/y
	nX += m_nPicOrigX;
	nY += m_nPicOrigY;

	///////////
	// draw 
	if( nType == 1 || nType == 2 )
	{
		wxPen mypen( *wxGREEN, 1, wxDOT );
		dc.SetPen( mypen );
		if( nType == 2 )
		{
			dc.SetBrush( *wxTRANSPARENT_BRUSH );

		} else
		{
			wxBrush mybrush( *wxGREEN, wxBDIAGONAL_HATCH );
			dc.SetBrush( mybrush );
		}

	} else
	{
		wxPen mypen( *wxRED, 1, wxSOLID );
		dc.SetPen( mypen );
		wxBrush mybrush( *wxRED, wxBDIAGONAL_HATCH );
		dc.SetBrush( mybrush );
	}
	dc.DrawRectangle( nX, nY, nW, nH );

	// if area is edited
	if( nType == 2 )
	{
		// draw corners
		wxPen mypen( *wxCYAN, 1, wxSOLID );
		dc.SetPen( mypen );
		dc.SetBrush( *wxTRANSPARENT_BRUSH );
		dc.DrawRectangle( nX-3, nY-3, 7, 7 );
		dc.DrawRectangle( nX+nW-4, nY-3, 7, 7 );
		dc.DrawRectangle( nX+nW-4, nY+nH-4, 7, 7 );
		dc.DrawRectangle( nX-3, nY+nH-4, 7, 7 );
	}

}

////////////////////////////////////////////////////////////////////
// Method:	DrawEllips
// Class:	CDynamicImgView
// Purpose:	draw an ellipse
// Input:	reference to command event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::DrawEllips( wxDC& dc, double x, double y, double a,
								double b, double theta, int dotflag )
{
	int		i;
	double	ct, st;
	double nX1=0, nY1=0, nX2=0, nY2=0, ac=0.0, bs=0.0;
	wxGraphicsPath gpath;

	ct = cos(PI*theta/180);
	st = sin(PI*theta/180);
	// get the first point
	nX1 = x+a*ct;
	nY1 = y+a*st;

	// check for antialiasing
	if( m_dc )
	{
		gpath = m_dc->CreatePath();
		//gpath.MoveToPoint( nX1, nY1 );
	}

	// now for every step
	for (i=1; i<37; i++)
	{
		ac = a*g_vectCtg[i];
		bs = b*g_vectStg[i];

		if (dotflag && !(i&1))
		{
			nX1 = x + ac*ct - bs*st; 
			nY1 = y + ac*st + bs*ct;

		} else
		{
			nX2 = x + ac*ct - bs*st;
			nY2 = y + ac*st + bs*ct;
			// and now draw the line
			if( m_dc ) 
			{
				gpath.MoveToPoint( nX1, nY1 );
				gpath.AddLineToPoint( nX2 , nY2 );

			} else
			{
				dc.DrawLine( (int) round( nX1 ), (int) round( nY1 ), 
								(int) round( nX2 ), (int) round( nY2 ) );

			}

			// now set my pointer to 1
			nX1 = nX2;
			nY1 = nY2;
		}
	}

	if( m_dc ) m_dc->DrawPath( gpath );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	DrawPath
// Class:	CDynamicImgView
// Purpose:	draw adynamic object path
// Input:	dc, number of point and refence to points
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::DrawPath( wxDC& dc, int nPoints, wxPoint vectPoints[], int nType )
{
	Spline2D spline;
	int i=0;

	// set it to Cat Mull Rom mode (spline will actually go through the control points)
	// Bezier, Hermite, CatMullRom, BSpline
	spline.Mode(Spline2D::CatMullRom);  
	// add new control points to the spline (you need at least 3 to render a spline)
	spline.ctrlpts.push_back( vector2f( (float) vectPoints[0].x-1, (float) vectPoints[0].y-1 ) );
	for( i=0; i<nPoints; i++ )
	{
		spline.ctrlpts.push_back( vector2f( (float) vectPoints[i].x, (float) vectPoints[i].y ) );  
	}
	spline.ctrlpts.push_back( vector2f( (float) vectPoints[nPoints-1].x+1, (float) vectPoints[nPoints-1].y+1 ) );
	// erase a point from the spline
//	spline.ctrlpts.erase(spline.ctrlpts.begin() + ctrlpt_to_erase); 
	// build the arc length table
//	spline.ArclenTableBuild();     
	// normalize the arc length table to be 0 - 1
//	spline.ArclenTableNormalize(); 


	// render the spline
	if (spline.ctrlpts.size() >= 3) // only draw the spline if it has enough control points
	{
		//unsigned int i;
		i=0;
		float t, err=0;
		float x, y, x2, y2;
	
		// draw curves
//		glBegin(GL_LINES);
//		glColor3f(1,0,0);
		int nSplineStep = spline.Step();
		unsigned int max= spline.ctrlpts.size()-(nSplineStep-1);
		unsigned int size= spline.ctrlpts.size();
		for( i=0; i < max; i+= nSplineStep )
		{
			for( t= 0; t < 1; t+=0.01f )
			{
				int nIdA = i%size;
				int nIdB = (i+1)%size;
				int nIdC = (i+2)%size;
				int nIdD = (i+3)%size;
				
				//if( nIdA == 0 ) continue;
				// when C == 0 we got at the end already
				if( nIdB == 0 ) break;
				if( nIdC == 0 ) break;
				if( nIdD == 0 ) break;

				// calculate point 1
				x = (float) spline.InterpCtrlPt(spline.ctrlpts[nIdA].x, spline.ctrlpts[nIdB].x, spline.ctrlpts[nIdC].x, spline.ctrlpts[nIdD].x, t);
				y = (float) spline.InterpCtrlPt(spline.ctrlpts[nIdA].y, spline.ctrlpts[nIdB].y, spline.ctrlpts[nIdC].y, spline.ctrlpts[nIdD].y, t);
				// calculate point 2
				x2 = spline.InterpCtrlPt(spline.ctrlpts[nIdA].x, spline.ctrlpts[nIdB].x, spline.ctrlpts[nIdC].x, spline.ctrlpts[nIdD].x, t+0.01);
				y2 = spline.InterpCtrlPt(spline.ctrlpts[nIdA].y, spline.ctrlpts[nIdB].y, spline.ctrlpts[nIdC].y, spline.ctrlpts[nIdD].y, t+0.01);

				// now draw by type
				if( nType == GRAPHICS_LINE_STYLE_NORMAL )
				{
					dc.DrawLine( x, y, x2, y2 );

				} else 
				{
					dc.DrawPoint( x, y );
					dc.DrawPoint( x2, y2 );
				}
			}
		}
//		glEnd();
	}

	// draw generated (approximated) path from the arc table
/*	if (spline.arclentable.size())
	{
//		glBegin(GL_POINTS);
		Spline2D::ArclenTable::iterator ai= spline.arclentable.begin();
		while (ai!=spline.arclentable.end())
		{
//			glColor3f(0, ai->time,1); // use the color to see what direction it's going in
//			glVertex2fv((GLfloat *)&ai->pos);
			
			dc.DrawPoint( ai->pos.x, ai->pos.y );

			++ai;
		}
//		glEnd();
	}
*/

	// clear out all the control points
	spline.ctrlpts.clear();        

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetImageFromFile
// Class:	CDynamicImgView
// Purose:	CImgView drawing
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::SetImageFromFile( const wxString& strFile )
{
	wxImage* pImage = new wxImage();
	pImage->LoadFile( strFile );
	// call to set image
	SetWxImage( pImage, 1 );
	// hack turn image ref off so I can destroy on exit
	m_bSharedImage = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	SetWxImage
// Class:	CDynamicImgView
// Purose:	CImgView drawing
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::SetWxImage( wxImage* pImage, int bRef, int bRepos )
{
	//////////
	// init for auto buffered paint ???
	SetBackgroundStyle(  wxBG_STYLE_CUSTOM  );

	if( pImage == NULL || !pImage->IsOk() ) return;

//	if( m_bDrawing ) return;
//	m_bDrawing = true;

	m_bSharedImage = bRef;
	// copy image localy
	if( bRef )
		m_pImage = pImage;
	else
	{
		if( m_pImage ) FreeImage( );
		m_pImage = new wxImage( *pImage );
	}
	//m_pImage = &(pImage->Copy());

	m_bHasImage = 1;

	// process input image
	ProcessInputImage( bRepos );
}

////////////////////////////////////////////////////////////////////
void CDynamicImgView::ProcessInputImage( int bRepos )
{
	int i = 0;
	wxRect nRect;

	// get image rectangle
	nRect.x = 0;
	nRect.y = 0;
	nRect.width = m_pImage->GetWidth();
	nRect.height = m_pImage->GetHeight();

	wxImage pTempImg = m_pImage->GetSubImage( nRect );

	// calculate new image scale
	int img_w = pTempImg.GetWidth();
	int img_h = pTempImg.GetHeight();

	//TODO :: consider option where heigth is bigger then width
	if( bRepos || ( m_nOrigPicWidth == 0 && m_nOrigPicHeight == 0 ) )
	{
		// calculate new image scale
		m_nOrigPicWidth = img_w;
		m_nOrigPicHeight = img_h;

		// calculate picture/win proportions
		double nPicProp = (double) m_nOrigPicWidth/m_nOrigPicHeight;
		double nWinProp = (double) m_nWidth/m_nHeight;
		if( nPicProp <= nWinProp )
		{
			m_nScaleFactor = (double) m_nHeight/m_nOrigPicHeight;
			m_nPicWidth = (int) m_nOrigPicWidth*m_nScaleFactor;

			m_nPicHeight = m_nHeight;

			m_nPicOrigX = (int) ( (m_nWidth/2) - (m_nPicWidth/2) );
			m_nPicOrigY = 0;

		} else
		{
			m_nScaleFactor = (double) m_nWidth/m_nOrigPicWidth;
			m_nPicHeight = (int) m_nOrigPicHeight*m_nScaleFactor;

			m_nPicWidth = m_nWidth;

			m_nPicOrigY = (int) ( (m_nHeight/2) - (m_nPicHeight/2) );
			m_nPicOrigX = 0;

		}

		m_nPicEndX	= m_nPicOrigX + m_nPicWidth;
		m_nPicEndY	= m_nPicOrigY + m_nPicHeight;

		m_nBasePicOrigX = m_nPicOrigX;
		m_nBasePicOrigY = m_nPicOrigY;
		m_nBasePicWidth = m_nPicWidth;
		m_nBasePicHeight = m_nPicHeight;

		// calculate ratio between new resize and orig
		m_nOrigPicWinRatio = (double) m_nOrigPicWidth/m_nPicWidth;

		// also set my cut
		m_nCutX = 0;
		m_nCutY = 0;
		m_nCutWidth = m_nOrigPicWidth;
		m_nCutHeight = m_nOrigPicHeight;

		m_nZoomCount = 0.5;
		m_nLastZoomFactor = m_nZoomCount*1.1;

		// set my image
		pTempImg.Rescale( m_nPicWidth, m_nPicHeight);
		s_mutexViewBmp->Lock();
		m_pBitmap = wxBitmap( pTempImg );
		s_mutexViewBmp->Unlock();

	} else
	{
		// set my image
		pTempImg.Rescale( m_nPicWidth, m_nPicHeight);
		s_mutexViewBmp->Lock();
		m_pBitmap = wxBitmap( pTempImg );
		s_mutexViewBmp->Unlock();

		if( m_nZoomCount > 0.5 ) ZoomIn( m_nZoomX, m_nZoomY, 0 );
	}

	m_bNewImage = true;
	m_bDrawing = true;

	Refresh( FALSE );

	Update( );
}

////////////////////////////////////////////////////////////////////
// Method:	OnLeftClickUp
// Class:	CDynamicImgView
// Purpose:	when left click is released
// Input:	mouse events
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::OnLeftClickUp( wxMouseEvent& pEvent )
{
	pEvent.Skip();

	if( !m_bHasImage ) return;

	// local variables
	long	x = pEvent.GetX(),
			y = pEvent.GetY();

	if( HasCapture() ) ReleaseMouse();

	/////////////////
	// IF ZOOM TOOL
	if( m_nInUseTool == IN_USE_TOOL_ZOOM && !m_bDragWithZoomTool ) 
	{
		// check if dragging 
		if( m_bDragging == 1 )
		{
			int nDragDiffX = x-m_nStartDragX;
			int nDragDiffY = y-m_nStartDragY;

			m_bDragging = 0;
			m_nDragLastX = 0;
			m_nDragLastY = 0;

			// zoom to area
			if( m_nStartDragX >= 0 &&  m_nStartDragY >= 0 && 
				abs(nDragDiffX) > 2 && abs(nDragDiffY) > 2 )
			{
				ZoomToArea( m_nStartDragX, m_nStartDragY, nDragDiffX, nDragDiffY );
			}

		} else
		{
			// else just do simple zoom
			if( m_bZoomDirection == 0 )
				ZoomIn( x, y, 1 );
			else if( m_bZoomDirection == 1 )
				ZoomOut( x, y, 1 );
		}

		return;
	} 

	////////////////////////
	// if dragging picture
	if( m_bDragging == 1 )
	{
		// if zoom - so drag
		if(  m_nZoomCount > 0.5 )
		{
			m_nZoomX = m_nZoomX-(x-m_nDragLastX);
			m_nZoomY = m_nZoomY-(y-m_nDragLastY);
	//		ZoomIn( (int) m_nZoomX , (int) m_nZoomY , 0 );

			int nDragDiffX = m_nStartDragX-x;
			int nDragDiffY = m_nStartDragY-y;

			// calculate new orig x 
			m_nLastZoomX -= nDragDiffX;
			m_nLastZoomY -= nDragDiffY;

			m_nBasePicOrigXShift -= nDragDiffX;
			m_nBasePicOrigYShift -= nDragDiffY;

	//		m_nBasePicOrigXShift += ((x-m_nLastZoomX) - (x-m_nLastZoomX)/m_nLastZoomFactor);
	//		m_nBasePicOrigYShift += ((y-m_nLastZoomY) - (y-m_nLastZoomY)/m_nLastZoomFactor);

			// call to update extra processing
			UpdateOnImageCut( );
			s_mutexViewBmp->Unlock();
		}

		// and reset 
		//SetTCursor();
		if( m_nLastCursorId >= 0 ) SetTCursor( m_nLastCursorId );

		m_bDragging = 0;
		m_nDragLastX = 0;
		m_nDragLastY = 0;
	}

}

////////////////////////////////////////////////////////////////////
// Method:	OnMouseRDClick
// Class:	CDynamicImgView
// Purose:	action on mouse right duble click
// Input:	reference to mouse event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::OnMouseRDClick( wxMouseEvent& pEvent )
{
	if( !m_bHasImage ) return;

	int x = pEvent.GetX(),
		y = pEvent.GetY();

	ZoomOut( x, y, 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	UpdateImage
// Class:	CDynamicImgView
// Purose:	update image - when changed
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::UpdateImage( )
{
	// set my rectangle
	wxRect nRect;
	nRect.x = (int) round(m_nCutX);
	nRect.y = (int) round(m_nCutY);
	nRect.width = (int) round(m_nCutWidth);
	nRect.height = (int) round(m_nCutHeight);

	// substract my sub image from orig
	wxImage pTempImg = m_pImage->GetSubImage( nRect );

	pTempImg.Rescale( (int) round(m_nPicWidth), (int) round(m_nPicHeight), 
						wxIMAGE_QUALITY_NORMAL );
	s_mutexViewBmp->Lock();
	m_pBitmap = wxBitmap( pTempImg );
	s_mutexViewBmp->Unlock();

	m_bNewImage = true;
	m_bDrawing = true;

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	DragImage
// Class:	CDynamicImgView
// Purose:	drag current view window over zoomed image
// Input:	reference to mouse event
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CDynamicImgView::DragImage( int nShiftX, int nShiftY )
{
	double nZoomFactor = m_nZoomCount*1.1;
	double nXFromOrig = 0;
	double nYFromOrig = 0;

	m_nLastXMarg -= nShiftX;
	m_nLastYMarg -= nShiftY;

	double nDiffWinLeftX = m_nLastXMarg;
	double nDiffWinRightX = m_nBasePicWidth*nZoomFactor-m_nWidth-nDiffWinLeftX;
	double nDiffWinUpY = m_nLastYMarg;
	double nDiffWinDownY = m_nBasePicHeight*nZoomFactor-m_nHeight-nDiffWinUpY;

	////////////////////////////////
	// check if x is out
	if( nDiffWinLeftX >= 0 )
	{
		// set origin of picture in window to zero (start from left border)
		m_nPicOrigX = 0.0;
		// calculate the start point in the big picture
		nXFromOrig = nDiffWinLeftX*m_nCutWinRatio;

		// now check if the right side goes over
		if( nDiffWinRightX >= 0 )
			m_nPicWidth = (double) m_nWidth;
		else
			m_nPicWidth = (double) (m_nWidth+nDiffWinRightX);

	} else // if left corner x cut inside
	{
		m_nPicOrigX = -nDiffWinLeftX;
		nXFromOrig = 0.0;

		if( nDiffWinRightX >= 0 )
			m_nPicWidth = (double) (m_nWidth+nDiffWinLeftX);
		else
			m_nPicWidth = (double) (m_nWidth+nDiffWinRightX+nDiffWinLeftX);
	}

	//////////////////////////////////////
	// here check if y is out
	if( nDiffWinUpY >= 0 )
	{
		m_nPicOrigY = 0;
		nYFromOrig = nDiffWinUpY*m_nCutWinRatio;

		if( nDiffWinDownY >= 0 )
			m_nPicHeight = (double) m_nHeight;
		else
			m_nPicHeight = (double) (m_nHeight+nDiffWinDownY);
	} else
	{
		nYFromOrig = 0;
		m_nPicOrigY = -nDiffWinUpY;

		if( nDiffWinDownY >= 0 )
			m_nPicHeight = (double) (m_nHeight+nDiffWinUpY);
		else
			m_nPicHeight = (double) (m_nHeight+nDiffWinDownY+nDiffWinUpY);
	}

	double nHeightCut = m_nPicHeight*m_nCutWinRatio; 
	double nWidthCut = m_nPicWidth*m_nCutWinRatio;

	// check on error
	if( nWidthCut <= 0 || nHeightCut<=0 || nXFromOrig < 0 || nYFromOrig < 0 ) return( 0 );

	/////////////////////////////////////////
	// set my image cut
	SetImageCut( (int) round(nXFromOrig), (int) round(nYFromOrig), 
				(int) round(nWidthCut), (int) round(nHeightCut) );

	// set to reound because - I use it in the image rounded
	m_nCutX = (int) round(nXFromOrig);
	m_nCutY = (int) round(nYFromOrig);

	m_bNewImage = true;
	m_bDrawing = true;

	Refresh( FALSE );
	Update( );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	SetImageCut
// Class:	CDynamicImgView
// Purose:	cut main image to draw on canvas
// Input:	reference to mouse event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::SetImageCut( int x, int y, int w, int h )
{
	// just caution
	if( x < 0 || y < 0 || w <= 0 || h <= 0 ) 
		return;

	// set my rectangle
	wxRect nRect;
	nRect.x = x;
	nRect.y = y;
	nRect.width = w;
	nRect.height = h;

	// substract my sub image from orig
	wxImage pTempImg = m_pImage->GetSubImage( nRect );

	pTempImg.Rescale( (int) round(m_nPicWidth), (int) round(m_nPicHeight), 
						wxIMAGE_QUALITY_NORMAL );
//	s_mutexViewBmp->Lock();
	m_pBitmap = wxBitmap( pTempImg );
//	s_mutexViewBmp->Unlock();

	// set my update flag
	UpdateOnImageCut( );
	//m_bCutUpdated = 1;
}

////////////////////////////////////////////////////////////////////
void CDynamicImgView::ResetZoom( )
{
	m_nZoomCount = 0;
	m_nLastZoomFactor = 0;
	m_nCutX = 0;
	m_nCutY = 0;
	m_nCutWinRatio = 0;
	m_nCutWidth = 0;
	m_nCutHeight = 0;
	m_nZoomX = 0;
	m_nZoomY = 0;

	m_nLastZoomX = 0;
	m_nLastZoomY = 0;
	m_nLastXMarg = 0;
	m_nLastYMarg = 0;
}

////////////////////////////////////////////////////////////////////
// Purose:	zoom to a specific rectangular area
////////////////////////////////////////////////////////////////////
int CDynamicImgView::ZoomToArea( int x, int y, int w, int h )
{
	// check if valid
	if( x < 0 || y < 0 || abs(w) < 2 || abs(h) < 2 ) return(0);

	double nW=0.0, nH=0.0;

	// i need to calculate my dimension from window to image
	// now if i am zoom mode
	if( m_nZoomCount > 0.5 )
	{
		nW = fabs( (double) w*m_nCutWinRatio );
		nH = fabs( (double) h*m_nCutWinRatio );

	} else
	{
		nW = fabs( (double) w/m_nScaleFactor );
		nH = fabs( (double) h/m_nScaleFactor );
	}

	// try this
	// calculate zoom factor/cound
	double nZoomFactor  = (double) ( (m_nOrigPicHeight*m_nHeight) / (m_nBasePicHeight*nH) );
	int fzc = (int) ((fabs(nZoomFactor)/1.1)/0.5);
	m_nZoomCount = (double) fzc * 0.5;
	// call to zoomin in the area center
	ZoomIn( x+w/2, y+h/2, 0 );

	return(1);
}

////////////////////////////////////////////////////////////////////
// Purose:	zoom in to coord
////////////////////////////////////////////////////////////////////
int CDynamicImgView::ZoomIn( int x, int y, int nFlag )
{
	// init my target index
	if( nFlag > 0 ) m_nZoomCount += 0.5;

	// x, y coord in picture rescaled in original window
	double nX = 0.0;
	double nY = 0.0;

	double nZoomFactor = m_nZoomCount*1.1;
	double nXFromOrig = 0;
	double nYFromOrig = 0;


	if( ((x > m_nPicOrigX && x < m_nPicEndX ) &&
		( y > m_nPicOrigY && y < m_nPicEndY ) ) ||nFlag == 0 )
	{
		// calculate mouse position inside picture
		nX = (double) ( x - m_nPicOrigX );
		nY = (double) ( y - m_nPicOrigY );

		double nDiffWinLeftX = 0.0;
		double nDiffWinRightX = 0.0;
		double nDiffWinUpY = 0.0;
		double nDiffWinDownY = 0.0;

		double nBasePicOrigX = m_nBasePicOrigX;
		double nBasePicOrigY = m_nBasePicOrigY;

		// here we check if this is the first zoom or not
		if( m_nZoomCount > 1 && m_nLastZoomFactor > 1 )
		{
			// get x/y as relative to base image no zoom
			if(( fabs(m_nLastZoomX - x) >= 1 || fabs(m_nLastZoomY - y) >= 1 ) || m_nLastZoomX == -1 )
			{
				// calculate new orig x 
				m_nBasePicOrigXShift += ((nX-m_nLastZoomX) - (nX-m_nLastZoomX)/m_nLastZoomFactor);
				m_nBasePicOrigYShift += ((nY-m_nLastZoomY) - (nY-m_nLastZoomY)/m_nLastZoomFactor);
			}

//			nX = (double) (nX + m_nLastXMarg)/m_nLastZoomFactor;
//			nY = (double) (nY + m_nLastYMarg)/m_nLastZoomFactor;
			nX = ((nX * m_nCutWinRatio) + m_nCutX)*m_nScaleFactor;
			nY = ((nY * m_nCutWinRatio) + m_nCutY)*m_nScaleFactor;
	
		} else
		{
			m_nBasePicOrigXShift = 0.0;
			m_nBasePicOrigYShift = 0.0;
		}

		nBasePicOrigX += m_nBasePicOrigXShift;
		nBasePicOrigY += m_nBasePicOrigYShift;

		m_nLastZoomX = (double) x;
		m_nLastZoomY = (double) y;
		m_nLastZoomFactor = nZoomFactor;
		m_nLastXMarg = nX*(nZoomFactor-1.0)-nBasePicOrigX;
		m_nLastYMarg = nY*(nZoomFactor-1.0)-nBasePicOrigY;

		// check if zoom is out of window
		nDiffWinLeftX = nX*(nZoomFactor-1.0)-nBasePicOrigX;
		nDiffWinRightX = m_nBasePicWidth*nZoomFactor-m_nWidth-nDiffWinLeftX;
		nDiffWinUpY = nY*(nZoomFactor-1.0)-nBasePicOrigY;
		nDiffWinDownY = m_nBasePicHeight*nZoomFactor-m_nHeight-nDiffWinUpY;

		// if left corner is outside
		if( nDiffWinLeftX >= 0.0 )
		{
			// set origin of picture in window to zero (start from left border)
			m_nPicOrigX = 0.0;
			// calculate the start point in the big picture
			nXFromOrig = m_nOrigPicWidth * (nX*(nZoomFactor-1.0)-nBasePicOrigX)/(m_nBasePicWidth*nZoomFactor);

			// now check if the right side goes over
			if( nDiffWinRightX >= 0.0 )
			{
				m_nPicWidth = (double) m_nWidth;
			} else
			{
				m_nPicWidth = (double) (m_nWidth+nDiffWinRightX);
			}

		} else // if leftt corner x cut inside
		{
			m_nPicOrigX = -nDiffWinLeftX;
			nXFromOrig = 0.0;

			if( nDiffWinRightX >= 0.0 )
			{
				m_nPicWidth = (double) (m_nWidth+nDiffWinLeftX);

			} else
			{
				m_nPicWidth = (double) (m_nWidth+nDiffWinRightX+nDiffWinLeftX);
			}
		}

		//////////////////////////////////////
		// here check if y is out
		if( nDiffWinUpY >= 0.0 )
		{
			m_nPicOrigY = 0.0;
			nYFromOrig = m_nOrigPicHeight * (nY*nZoomFactor-(nY+nBasePicOrigY))/(m_nBasePicHeight*nZoomFactor);

			if( nDiffWinDownY >= 0 )
			{
				m_nPicHeight = (double) m_nHeight;

			} else
			{
				m_nPicHeight = (double) (m_nHeight+nDiffWinDownY);
			}

		} else
		{
			nYFromOrig = 0.0;
			m_nPicOrigY = -nDiffWinUpY;

			if( nDiffWinDownY >= 0 )
			{
				m_nPicHeight = (double) (m_nHeight+nDiffWinUpY);

			} else
			{
				m_nPicHeight = (double) (m_nHeight+nDiffWinDownY+nDiffWinUpY);
			}

		}

		double nHeightCut = (double) ( (m_nOrigPicHeight*m_nPicHeight) / (m_nBasePicHeight*nZoomFactor) ); 
		double nWidthCut = (double) ( (m_nOrigPicWidth * m_nPicWidth) / (m_nBasePicWidth*nZoomFactor) );
	
		// check on error
		if( nWidthCut <= 0 || nHeightCut<=0 || nXFromOrig < 0 || nYFromOrig < 0 ) return( 0 );

		////////////////////////////
		// set my image cut
		s_mutexViewBmp->Lock();
		SetImageCut( (int) round(nXFromOrig), (int) round(nYFromOrig), 
						(int) round(nWidthCut), (int) round(nHeightCut) );
		s_mutexViewBmp->Unlock();

		// calculate cut ratio between new window resize and original image
		int nHeightCutCor = (int) round(nHeightCut);
		int nPicHeightCor = (int) round(m_nPicHeight);
		// ration between orig image cut and zoom image cut
//		m_nCutWinRatio = (double) nHeightCutCor/nPicHeightCor;
		m_nCutWinRatio = (double) (nHeightCut/m_nPicHeight);
		// or commented ratio between orig image and zoomed image
//		m_nCutWinRatio = (double) m_nOrigPicWidth / (m_nBasePicWidth*nZoomFactor);

		m_nCutX = nXFromOrig;
		m_nCutY = nYFromOrig;

		// larry :: fix set to double orig
//		m_nCutWidth = (int) round(nWidthCut);
//		m_nCutHeight = (int) round(nHeightCut);
		m_nCutWidth = nWidthCut;
		m_nCutHeight = nHeightCut;

		// larry :: fix set to double orig
//		m_nPicEndX = m_nPicOrigX + (int) round(m_nPicWidth);
//		m_nPicEndY = m_nPicOrigY + (int) round(m_nPicHeight);
		m_nPicEndX = m_nPicOrigX + m_nPicWidth;
		m_nPicEndY = m_nPicOrigY + m_nPicHeight;

		if( nFlag > 0 ) 
		{
			m_nZoomX = x;
			m_nZoomY = y;
		}

		m_bNewImage = true;
		m_bDrawing = true;

		Refresh( FALSE );
		Update( );

	} else
		return( 0 );
	
	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Purose:	zoom out in the picture
////////////////////////////////////////////////////////////////////
int CDynamicImgView::ZoomOut( int x, int y, int nFlag )
{
	// init my target index
	if( nFlag > 0 && m_nZoomCount >= 1.0 ) m_nZoomCount -= 0.5;

	double nX = 0;
	double nY = 0;

	double nZoomFactor = m_nZoomCount*1.1;
	double nXFromOrig = 0;
	double nYFromOrig = 0;


	if( m_nZoomCount == 0.5 ) nZoomFactor = 1;

	if( (x > m_nPicOrigX && x < m_nPicEndX ) &&
		( y > m_nPicOrigY && y < m_nPicEndY ) )
	{
		// calculate mouse position inside picture
		nX = (double) (x - m_nPicOrigX);
		nY = (double) (y - m_nPicOrigY);

		double nDiffWinLeftX = 0.0;
		double nDiffWinRightX = 0.0;
		double nDiffWinUpY = 0.0;
		double nDiffWinDownY = 0.0;

		double nBasePicOrigX = m_nBasePicOrigX;
		double nBasePicOrigY = m_nBasePicOrigY;

		if( m_nZoomCount > 1 )
		{
			// get x/y as relative to base image no zoom
			if(( fabs(m_nLastZoomX - x) >= 1 || fabs(m_nLastZoomY - y) >= 1 ) || m_nLastZoomX ==-1 )
			{
				// calculate new orig x 
				m_nBasePicOrigXShift += ((nX-m_nLastZoomX) - (nX-m_nLastZoomX)/m_nLastZoomFactor);
				m_nBasePicOrigYShift += ((nY-m_nLastZoomY) - (nY-m_nLastZoomY)/m_nLastZoomFactor);
			}

//			nX = (double) (nX + m_nLastXMarg)/m_nLastZoomFactor;
//			nY = (double) (nY + m_nLastYMarg)/m_nLastZoomFactor;
			nX = ((nX * m_nCutWinRatio) + m_nCutX)*m_nScaleFactor;
			nY = ((nY * m_nCutWinRatio) + m_nCutY)*m_nScaleFactor;

		}  else
		{
			m_nBasePicOrigXShift = 0;
			m_nBasePicOrigYShift = 0;
		}

		nBasePicOrigX += m_nBasePicOrigXShift;
		nBasePicOrigY += m_nBasePicOrigYShift;


		m_nLastZoomX = (double) x;
		m_nLastZoomY = (double) y;
		m_nLastZoomFactor = nZoomFactor;
		m_nLastXMarg = nX*(nZoomFactor-1.0)-nBasePicOrigX;
		m_nLastYMarg = nY*(nZoomFactor-1.0)-nBasePicOrigY;

		// check if zoom is out of window
		nDiffWinLeftX = nX*(nZoomFactor-1.0)-nBasePicOrigX;
		nDiffWinRightX = m_nBasePicWidth*nZoomFactor-m_nWidth-nDiffWinLeftX;
		nDiffWinUpY = nY*(nZoomFactor-1.0)-nBasePicOrigY;
		nDiffWinDownY = m_nBasePicHeight*nZoomFactor-m_nHeight-nDiffWinUpY;

		// if left corner is outside
		if( nDiffWinLeftX >= 0 )
		{
			// set origin of picture in window to zero (start from left border)
			m_nPicOrigX = 0;
			// calculate the start point in the big picture
			nXFromOrig = m_nOrigPicWidth * (nX*nZoomFactor-(nX+nBasePicOrigX))/(m_nBasePicWidth*nZoomFactor);

			// now check if the right side goes over
			if( nDiffWinRightX >= 0 )
			{
				m_nPicWidth = (double) m_nWidth;
			} else
			{
				m_nPicWidth = (double) (m_nWidth+nDiffWinRightX);
			}

		} else // if leftt corner x cut inside
		{
			m_nPicOrigX = -nDiffWinLeftX;
			nXFromOrig = 0.0;

			if( nDiffWinRightX >= 0 )
			{
//				m_nPicWidth = m_nWidth-(m_nBasePicOrigX-nDiffWinLeftX);
				m_nPicWidth = (double) (m_nWidth+nDiffWinLeftX);

			} else
			{
//				m_nPicWidth = m_nBasePicWidth+nDiffWinLeftX+nDiffWinRightX;
				m_nPicWidth = (double) (m_nWidth+nDiffWinRightX+nDiffWinLeftX);
			}
		}

		//////////////////////////////////////
		// here check if y is out
//		if( nDiffWinUpY >= m_nBasePicOrigY )
		if( nDiffWinUpY >= 0 )
		{
			m_nPicOrigY = 0.0;
			nYFromOrig = m_nOrigPicHeight * (nY*nZoomFactor-(nY+nBasePicOrigY))/(m_nBasePicHeight*nZoomFactor);

			if( nDiffWinDownY >= 0 )
			{
				m_nPicHeight = (double) m_nHeight;

			} else
			{
				m_nPicHeight = (double) (m_nHeight+nDiffWinDownY);
			}

		} else
		{
			nYFromOrig = 0.0;
			m_nPicOrigY = -nDiffWinUpY;

			if( nDiffWinDownY >= 0 )
			{
				m_nPicHeight = (double) (m_nHeight+nDiffWinUpY);

			} else
			{
				m_nPicHeight = (double) (m_nHeight+nDiffWinDownY+nDiffWinUpY);
			}

		}

		double nHeightCut = (double) ((m_nOrigPicHeight*m_nPicHeight) / (m_nBasePicHeight*nZoomFactor)); 
		double nWidthCut = (double) ((m_nOrigPicWidth * m_nPicWidth) / (m_nBasePicWidth*nZoomFactor));

		// set my image cut
		s_mutexViewBmp->Lock();
		SetImageCut( (int) round(nXFromOrig), (int) round(nYFromOrig), 
						(int) round(nWidthCut), (int) round(nHeightCut) );
		s_mutexViewBmp->Unlock();

		// calculate cut ratio between new resize and orig
		m_nCutWinRatio = (double) ( nHeightCut/m_nPicHeight );

		m_nCutX = nXFromOrig;
		m_nCutY = nYFromOrig;
		m_nCutWidth = nWidthCut;
		m_nCutHeight = nHeightCut;

		m_nPicEndX = m_nPicOrigX + m_nPicWidth;
		m_nPicEndY = m_nPicOrigY + m_nPicHeight;

		if( nFlag > 0 ) 
		{
			m_nZoomX = x;
			m_nZoomY = y;
		}

		m_bNewImage = true;
		m_bDrawing = true;

		Refresh( FALSE );

		Update( );

	} else
		return( 0 );
	
	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	OnMouseWheel
// Class:	CDynamicImgView
// Purpose:	show menu for robot view
// Input:	mouse events
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::OnMouseWheel( wxMouseEvent& pEvent )
{
	if( !m_bHasImage ) return;

	int x = pEvent.GetX(),
		y = pEvent.GetY();

	if( pEvent.m_wheelRotation > 0 )
	{
		ZoomIn( x, y, 1 );

	} else if( pEvent.m_wheelRotation < 0 )
	{
		ZoomOut( x, y, 1 );
	}

}

////////////////////////////////////////////////////////////////////
// Method:	On Right Click
// Class:	CDynamicImgView
// Purpose:	show menu for robot view
// Input:	mouse events
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::OnRightClick( wxMouseEvent& pEvent )
{
	if( !m_bHasImage ) return;

//	if( pEvent.ButtonDClick( ) ) return;

	// local variables
	long	x = pEvent.GetX(),
			y = pEvent.GetY();

	if( IsInPicture( x, y ) == 0 ) return;

	m_nCurrentMouseX = x-m_nPicOrigX;
	m_nCurrentMouseY = y-m_nPicOrigY;

	/////////////////
	// IF ZOOM TOOL
	if( m_nInUseTool == IN_USE_TOOL_ZOOM ) 
	{
		// else just do simple zoom
		if( m_bZoomDirection == 1 )
			ZoomIn( x, y, 1 );
		else if( m_bZoomDirection == 0 )
			ZoomOut( x, y, 1 );

	} else
	{
		// store position
		m_nPicRightClickPosX = m_nCurrentMouseX;
		m_nPicRightClickPosY = m_nCurrentMouseY;

		// create context popup menu
		wxMenu* pImgViewMenu = CreatePopupMenu();
		if( pImgViewMenu )
		{
			PopupMenu( pImgViewMenu, x, y );
			delete( pImgViewMenu );

			// if area selected 
			if( m_nAreaSelected >=0 )
			{
				m_nAreaSelected = -1;
				Refresh(false);
			}
		}
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	IsInPicture
// Class:	CDynamicImgView
// Purpose:	add action key
// Input:	reference to command event
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CDynamicImgView::IsInPicture( int nX, int nY )
{
	if( ( nX > m_nPicOrigX && nX < m_nPicEndX ) &&
		( nY > m_nPicOrigY && nY < m_nPicEndY ) )
	{
		return( 1 );
	} else
		return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:	OnMouseMove
// Class:	CDynamicImgView
// Purose:	actions when mouse is moving
// Input:	reference to mouse event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::OnMouseMove( wxMouseEvent& pEvent )
{
	pEvent.Skip();

	if( !m_bHasImage ) return;

	// get my new coord
	m_nMouseWinX = pEvent.GetX();
	m_nMouseWinY = pEvent.GetY();

	// get win coord relative to image
	m_nMouseWinPicX = m_nMouseWinX - m_nPicOrigX;
	m_nMouseWinPicY = m_nMouseWinY - m_nPicOrigY;

	if( !IsInPicture( m_nMouseWinX, m_nMouseWinY ) ) return;

//	m_pFrame->SetStatusXY( m_nMouseWinPicX, m_nMouseWinPicY );

	////////////////////////////////////////
	// IF DRAGGING ACTION - i should move this back !!!!! drag on is universal!!!
	if( pEvent.Dragging() && pEvent.LeftIsDown() && 
		!pEvent.RightIsDown() && !pEvent.MiddleIsDown() ) DragOn( m_nMouseWinX, m_nMouseWinY );

}

////////////////////////////////////////////////////////////////////
// Method:	DragOn
// Class:	CDynamicImgView
// Purose:	drag image action
// Input:	reference to mouse event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::DragOn( int x, int y )
{

	// calculate dragging
	if( m_bDragging == 0 )
	{
		m_nStartDragX = x;
		m_nStartDragY = y;

		m_nDragLastX = x;
		m_nDragLastY = y;

		m_bDragging = 1;
		s_mutexViewBmp->Lock();

		CaptureMouse();

		// set drag cursor - if default but not area resize
		if( m_nInUseTool == IN_USE_TOOL_DEFAULT && m_AreaResizeVertex < 0 && !m_bAreaMove ) 
		{
			// set to drag
			SetTCursor( UNIMAP_CURSOR_IMAGE_DRAG );
		}
	}

	// get dragging difference
	m_nDragDiffX = (double) (x-m_nDragLastX);
	m_nDragDiffY = (double) (y-m_nDragLastY);

	// larry new just get last x drag
	m_nDragLastX = x;
	m_nDragLastY = y;

	///////////////
	// here we stop - if these conditions
	if( m_nAreaEdit >= 0 && m_AreaResizeVertex >= 0 || m_bAreaMove ) return;

	/////////////////
	// if all ok then drag my image
	if( m_nInUseTool == IN_USE_TOOL_DEFAULT || m_bDragWithZoomTool ) 
	{

//		if( x < 5 && y < 5 ) return;

		//  move picture around
		// if there is a zoom 
		if( m_nZoomCount > 0.5 )
		{
			if( m_nDragDiffX != 0 || m_nDragDiffY != 0 )
			{
				//ZoomIn( (int) m_nZoomX - nDragDiffX, 
				//	(int) m_nZoomY - nDragDiffY, 0 );
				//ZoomIn( x, y , 0 );

				// todo: do I need double here ???
				DragImage( m_nDragDiffX, m_nDragDiffY );
			} 

		}

	// else if zoom tool drag zoom to area
	} else if( m_nInUseTool == IN_USE_TOOL_ZOOM || m_nInUseTool == IN_USE_TOOL_ADD_AREA ) 
	{
		// do update or refresh here ?
		Refresh( FALSE );
//		Update( );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnSize
// Class:	CDynamicImgView
// Purose:	adjust on windows resize
// Input:	reference to size event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::OnSize( wxSizeEvent& even )
{

	int nWidth = even.GetSize().GetWidth();
	int nHeight = even.GetSize().GetHeight();

	m_nWidth = nWidth;
	m_nHeight = nHeight;

	if( m_bHasImage == 0 ) return;

	wxRect nRect;

	if( m_nZoomCount > 0.5 )
	{
		double nZoomFactor = m_nZoomCount*1.1;

		// calculate difference between ration to get the best way to fit
		double nPicProp = (double) m_nOrigPicWidth/m_nOrigPicHeight;
		double nWinProp = (double) m_nWidth/m_nHeight;
		// if use use max width or height
		if( nPicProp <= nWinProp )
		{
			m_nScaleFactor = (double) m_nHeight/m_nOrigPicHeight;
			m_nPicWidth = m_nOrigPicWidth*m_nScaleFactor;

			m_nPicHeight = m_nHeight;

			m_nPicOrigX = (m_nWidth/2) - (m_nPicWidth/2);
			m_nPicOrigY = 0;

		} else
		{
			m_nScaleFactor = (double) (m_nWidth/m_nOrigPicWidth);
			m_nPicHeight = m_nOrigPicHeight*m_nScaleFactor;

			m_nPicWidth = m_nWidth;

			m_nPicOrigY = (double) (m_nHeight/2.0) - (double) (m_nPicHeight/2.0);
			m_nPicOrigX = 0.0;

		}

		m_nPicEndX	= m_nPicOrigX + m_nPicWidth;
		m_nPicEndY	= m_nPicOrigY + m_nPicHeight;

		m_nBasePicOrigX = m_nPicOrigX;
		m_nBasePicOrigY = m_nPicOrigY;
		m_nBasePicWidth = m_nPicWidth;
		m_nBasePicHeight = m_nPicHeight;

		// calculate ratio between new resize and orig
		m_nOrigPicWinRatio = (double) (m_nOrigPicWidth/m_nPicWidth);

		m_nCutHeight = (double) ((m_nOrigPicHeight*m_nPicHeight) / (m_nBasePicHeight*nZoomFactor));
		m_nCutWidth = (double) ( (m_nOrigPicWidth*m_nPicWidth) / (m_nBasePicWidth*nZoomFactor) );

		m_nCutWinRatio = (double) (m_nCutHeight/m_nPicHeight);

		// larry :: use dragimage instead of zoomin ?
		//ZoomIn( m_nZoomX, m_nZoomY, 0 );
//		m_nLastXMarg = 0; //m_nCutX*m_nScaleFactor*(nZoomFactor-1.0)-m_nBasePicOrigX;
//		m_nLastYMarg = 0; //m_nCutY*m_nScaleFactor*(nZoomFactor-1.0)-m_nBasePicOrigY;
//		m_nDragLastX = 0;
//		m_nDragLastY = 0;
		DragImage( 0, 0 );
		//UpdateImage( );

	} else
	{
		// get image rectangle
		nRect.x = 0;
		nRect.y = 0;
		nRect.width = m_pImage->GetWidth();
		nRect.height = m_pImage->GetHeight();

		wxImage pTempImg = m_pImage->GetSubImage( nRect );

		// calculate new image scale
		m_nOrigPicWidth = pTempImg.GetWidth();
		m_nOrigPicHeight = pTempImg.GetHeight();
		// calculate difference between ration to get the best way to fit
		double nPicProp = (double) m_nOrigPicWidth/m_nOrigPicHeight;
		double nWinProp = (double) m_nWidth/m_nHeight;

		if( nPicProp <= nWinProp )
		{
			m_nScaleFactor = (double) m_nHeight/m_nOrigPicHeight;
			m_nPicWidth = m_nOrigPicWidth*m_nScaleFactor;

			m_nPicHeight = m_nHeight;

			m_nPicOrigX = (double) (m_nWidth/2) - (double) (m_nPicWidth/2);
			m_nPicOrigY = 0.0;

		} else
		{
			m_nScaleFactor = (double) m_nWidth/m_nOrigPicWidth;
			m_nPicHeight = m_nOrigPicHeight*m_nScaleFactor;

			m_nPicWidth = m_nWidth;

			m_nPicOrigY = (double) (m_nHeight/2) - (double) (m_nPicHeight/2);
			m_nPicOrigX = 0.0;

		}

		m_nPicEndX	= m_nPicOrigX + m_nPicWidth;
		m_nPicEndY	= m_nPicOrigY + m_nPicHeight;

		m_nBasePicOrigX = m_nPicOrigX;
		m_nBasePicOrigY = m_nPicOrigY;
		m_nBasePicWidth = m_nPicWidth;
		m_nBasePicHeight = m_nPicHeight;

		pTempImg.Rescale( round( m_nPicWidth ), round( m_nPicHeight ) );
		s_mutexViewBmp->Lock();
		m_pBitmap = wxBitmap( pTempImg );
		s_mutexViewBmp->Unlock();

		// calculate ratio between new resize and orig
		m_nOrigPicWinRatio = (double) m_nOrigPicWidth/m_nPicWidth;
	}

	// set sizer dimensions
	wxSizer* sizerMain = GetSizer();
	if( sizerMain ) sizerMain->SetDimension( 0, 0, m_nWidth-2, m_nHeight-1 );
}


////////////////////////////////////////////////////////////////////
// Method:	GetElxBitmap
// Class:	CDynamicImgView
// Purpose:	get bitmap in ELX format image variant
// Input:	nothing
// Output:	pointer to image variant elx format
////////////////////////////////////////////////////////////////////
ImageVariant CDynamicImgView::GetElxBitmap( )
{
	ImageVariant rElxImage;

	// get abstract image info
	const uint32 w = m_pBitmap.GetWidth();
	const uint32 h = m_pBitmap.GetHeight();
    const uint32 size = w*h*3;

	wxImage pImage = m_pBitmap.ConvertToImage( );

	// create rgb image
	boost::shared_ptr<ImageRGBub> spImage( new ImageRGBub(w, h) );
    unsigned char* prDst = (unsigned char*) spImage->GetPixel(); 
    unsigned char* prSrc = (unsigned char*) pImage.GetData();

	/////////
    // copy from wxImage to ImageVariant
    ::memcpy( (void*) prDst, (const void*) prSrc, size );
	// set back image
	rElxImage = *spImage;

	return( rElxImage ); 
}

////////////////////////////////////////////////////////////////////
// Method:	SetElxBitmap
// Class:	CDynamicImgView
// Purpose:	set bitmap from ELX format 
// Input:	pointer to image variant elx format
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::SetElxBitmap( ImageVariant* pElxImage )
{
	// get abstract image info
	const uint32 w = pElxImage->GetWidth(); //m_pBitmap.GetWidth();
	const uint32 h = pElxImage->GetHeight(); //m_pBitmap.GetHeight();
    const uint32 size = w*h*3;
	// create image
	wxImage pImage( w, h );

	boost::shared_ptr<ImageRGBub> spImage = elxCreateImageRGBub( *pElxImage->GetImpl() );
	unsigned char* prDst = (unsigned char*) spImage->GetPixel(); 
	unsigned char* prSrc = (unsigned char*) pImage.GetData();

	::memcpy( prSrc, (void*) prDst, size );

	// convert from image 
	s_mutexViewBmp->Lock();
	m_pBitmap = wxBitmap( pImage );
	s_mutexViewBmp->Unlock();

	Refresh( FALSE );
//	UpdateImage( );
}

////////////////////////////////////////////////////////////////////
// Method:	GetElxImage
// Class:	CDynamicImgView
// Purpose:	get iamge in ELX format imaeg variant
// Input:	nothing
// Output:	pointer to image variant elx format
////////////////////////////////////////////////////////////////////
ImageVariant CDynamicImgView::GetElxImage( )
{
	ImageVariant rElxImage;

	// get abstract image info
	const uint32 w = m_pImage->GetWidth();
	const uint32 h = m_pImage->GetHeight();
    const uint32 size = w*h*3;
	// create rgb image
	boost::shared_ptr<ImageRGBub> spImage( new ImageRGBub(w, h) );
    unsigned char* prDst = (unsigned char*) spImage->GetPixel(); 
    unsigned char* prSrc = (unsigned char*) m_pImage->GetData();

	/////////
    // copy from wxImage to ImageVariant
    ::memcpy( (void*) prDst, (const void*) prSrc, size );
	// set back image
	rElxImage = *spImage;

	return( rElxImage ); 
}

////////////////////////////////////////////////////////////////////
// Method:	SetElxImage
// Class:	CDynamicImgView
// Purpose:	set image in ELX format to current image
// Input:	pointer to image variant elx format
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDynamicImgView::SetElxImage( ImageVariant* pElxImage )
{
	// get abstract image info
	const uint32 w = m_pImage->GetWidth();
	const uint32 h = m_pImage->GetHeight();
    const uint32 size = w*h*3;

	boost::shared_ptr<ImageRGBub> spImage = elxCreateImageRGBub( *pElxImage->GetImpl() );
	unsigned char* prDst = (unsigned char*) spImage->GetPixel(); 
	unsigned char* prSrc = (unsigned char*) m_pImage->GetData();

	::memcpy( prSrc, (void*) prDst, size );

	UpdateImage( );
}

////////////////////////////////////////////////////////////////////
void CDynamicImgView::OnKeyDown( wxKeyEvent& pEvent )
{
	//int mod = pEvent.GetModifiers();
	int key = pEvent.GetKeyCode();
	int mod = pEvent.GetModifiers();

	pEvent.Skip();

	// if alt down and cursor tool is zoom
	if( m_nInUseTool == IN_USE_TOOL_ZOOM )
	{
		if( key == WXK_ALT )
		{
			// set to zoom out
			SetTCursor( UNIMAP_CURSOR_IMAGE_ZOOM_OUT );
			m_bZoomDirection = 1;

		} else if( key == WXK_CONTROL )
		{
			// set to zoom out
			SetTCursor( UNIMAP_CURSOR_IMAGE_DRAG );
			m_bDragWithZoomTool = 1;
		}

	} else
	{
		// if validate edited area
		if( m_nAreaEdit >=0 && key == WXK_RETURN  )
		{
			m_nAreaSelected = -1;
			m_nAreaEdit = -1;
			m_AreaResizeVertex = -1;
			m_bAreaMove = 0;
			SetTCursor();

			Refresh(false);
		}
	}
}

////////////////////////////////////////////////////////////////////
void CDynamicImgView::OnKeyUp( wxKeyEvent& pEvent )
{
	int key = pEvent.GetKeyCode();

	pEvent.Skip();

	if( m_nInUseTool == IN_USE_TOOL_ZOOM )
	{
		if( ( m_bZoomDirection == 1 && key == WXK_ALT ) ||
			( m_bDragWithZoomTool && key == WXK_CONTROL ) )
		{
			SetTCursor( UNIMAP_CURSOR_IMAGE_ZOOM_IN );
			// if alt also set the direction flag back to zoom in
			if( key == WXK_ALT ) 
				m_bZoomDirection = 0;
			else
				m_bDragWithZoomTool = 0;

		} 

	} else
	{
		// other keys ...
	}
}
