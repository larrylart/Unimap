////////////////////////////////////////////////////////////////////
// Name:		graph plot base viewer implementation
// File:		graph_base_view.cpp
// Purpose:		graph plot viewer
//
// Created by:	Larry Lart on 29-08-2009
// Updated by:	
//
// Copyright:	(c) 2009 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include <wx/dcmemory.h>
#include <math.h>

// include gauss fit
#include "../math/fitgsl.h"

// local headers
#include "../util/func.h"
#include "designSkin.h"
#include "../image/radialprofile.h"

// main header
#include "graph_base_view.h"

// local defines
#define START_X   (5)
#define START_Y   (7)
#define LAST_Y    (5)
#define BAND_W    (256*2)
#define BAND_H    (128+64)
#define TOTAL_W   (START_X + BAND_W + START_X)
#define TOTAL_H   (START_Y + BAND_H + LAST_Y)

BEGIN_EVENT_TABLE( CGraphBaseView, wxWindow )
	EVT_PAINT( CGraphBaseView::OnPaint )
	EVT_ERASE_BACKGROUND( CGraphBaseView::OnEraseBackground )
	EVT_SIZE( CGraphBaseView::OnSize ) 
END_EVENT_TABLE( )
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CGraphBaseView
// Purpose:	Initialize object
// Input:	pointer to reference window, etc
// Output:	nothing
////////////////////////////////////////////////////////////////////
CGraphBaseView::CGraphBaseView( wxWindow* iprParent, wxWindowID iId, const wxPoint& iPosition , int nFlags ) :
									wxWindow( iprParent, iId, iPosition, wxSize(-1,250), wxRAISED_BORDER )
{
	// init size
	SetMinSize( wxSize( 160, 120 ) );
//	SetClientSize(TOTAL_W, TOTAL_H);
	// init vars
	m_nStartX = START_X;
	m_nStartY = START_Y;
	m_nWidth = BAND_W;
	m_nHeight = BAND_H;
	m_nGraphHeight = 0;

	m_nFlags = nFlags;

	m_vectData = NULL;
	m_nData = 0;
} 

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CGraphBaseView
// Purpose:	Delete my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CGraphBaseView::~CGraphBaseView( )
{
	if( m_vectData )
	{
		for( int i=0; i<m_nData; i++ )
			if( m_vectData[i].vectPoints ) free( m_vectData[i].vectPoints );

		free( m_vectData );
	}
} 

////////////////////////////////////////////////////////////////////
void CGraphBaseView::SetConfig( int nGraph )
{
	int i = 0;
	m_nData = 3;
	m_vectData = (DefGraphCurve*) malloc( nGraph*sizeof( DefGraphCurve ) );
	// allocate a max for each graph
	for( i=0; i<nGraph; i++ )
	{
		m_vectData[i].allocated = 100;
		m_vectData[i].vectPoints = (double*) malloc( 100*sizeof( double ) );
		m_vectData[i].nPoints = 0;
		m_vectData[i].min = 0.0;
		m_vectData[i].max = 0.0;
	}

	// default data size point on horizontal
	m_nDataSize = 20;
}

////////////////////////////////////////////////////////////////////
// Method:	SetData
// Class:	CGraphBaseView
// Purpose:	set my curve data
// Input:	pointer to plot data, number of data point and graph id 
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGraphBaseView::SetData( double* vectData, int nData, int nGraphId )
{
	if( nGraphId<0 || nGraphId >= m_nData ) return;

	int i=0;

	m_vectData[nGraphId].max = DBL_MIN;
	m_vectData[nGraphId].min = DBL_MAX;
	m_vectData[nGraphId].nPoints = nData;
	// check allocation
	if( nData >= m_vectData[nGraphId].allocated )
	{
		m_vectData[nGraphId].allocated = nData;
		m_vectData[nGraphId].vectPoints = (double*) realloc( m_vectData[nGraphId].vectPoints, nData*sizeof( double ) );
		m_nDataSize = nData;
	}

	// populate local vector
	for( i=0; i<nData; i++ )
	{
		// calculate max min
		if( vectData[i] > m_vectData[nGraphId].max ) m_vectData[nGraphId].max = vectData[i];
		if( vectData[i] < m_vectData[nGraphId].min ) m_vectData[nGraphId].min = vectData[i];

		m_vectData[nGraphId].vectPoints[i] = vectData[i];
	}
} 

////////////////////////////////////////////////////////////////////
// Method:	OnEraseBackground
// Class:	CGraphBaseView
// Purpose:	nothing to todo = no more flicking
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGraphBaseView::OnEraseBackground( wxEraseEvent& WXUNUSED(iEvent) )
{
	// nothing to todo = no more flicking
} 

////////////////////////////////////////////////////////////////////
// Method:	OnPaint
// Class:	CGraphBaseView
// Purpose:	paint
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGraphBaseView::OnPaint( wxPaintEvent& WXUNUSED(iEvent) )
{
	wxFont smallFont( 7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT );

	wxPaintDC dc(this);
	wxMemoryDC memDC;
	memDC.SetFont( smallFont );

	wxString strTmp;
	wxSize sizeTxt;
	int nTicksSpace = 14;
	// case: if 1 space for labels = 1
	if( m_nFlags == 1 ) nTicksSpace = 1;

	int nHeight = m_nHeight - nTicksSpace;
	int nStartY = START_Y; //+nTicksSpace;

	// get max y labels extent
	strTmp.Printf( wxT("%d"), nHeight );
	sizeTxt = memDC.GetTextExtent( strTmp );
	int nTicksSpaceY = sizeTxt.GetWidth() + 3;
	// case: if 1 space for labels = 1
	if( m_nFlags == 1 ) nTicksSpaceY = 1;

	int nWidth = m_nWidth - nTicksSpaceY;
	int nStartX = START_X+nTicksSpaceY;
	// get total paint area
	int nTotalW = START_X + m_nWidth + START_X;
	int nTotalH = START_Y + m_nHeight + LAST_Y;
	// create and set my paint bitmap
	wxBitmap bitmap( nTotalW, nTotalH );
	memDC.SelectObject( bitmap );

	// clear all
	memDC.SetPen( GetSkin().GetBackgroundPen() );
	memDC.SetBrush( GetSkin().GetBackgroundBrush() );
	memDC.DrawRectangle(0, 0, nTotalW, nTotalH );

	// clear histogram
	memDC.SetPen( GetSkin().GetBlackPen() );
	memDC.SetBrush( GetSkin().GetWhiteBrush() );
	memDC.DrawRectangle( nStartX-1, nStartY-1, nWidth+2, nHeight+2 );

	// draw tag
	unsigned int x,y;
	int divs_h = 4;
	double step_h = (double) nHeight / divs_h;
	double dx, dy;
	int i=0, j=0, nTickVal;
	const double zoomx = 1.0 * double(nWidth) / 512.0;
	
	double tzoom_h = 256.0/10.0;

	///////////////////////
	// ticks :: vertical
	for( i=0; i<=divs_h; i++ )
	{
		dy = i*step_h;
		// calculate y
		y = nStartY + nHeight - (int) round(dy);

		// draw horizontal grid lines
		memDC.SetPen( GetSkin().GetGreyPen() );
		memDC.DrawLine( nStartX, y, nStartX+nWidth, y );
		// draw ticks
		memDC.SetPen( GetSkin().GetBlackPen() );
		memDC.DrawLine( nStartX-4, y, nStartX, y );
		// labels
		if( m_nFlags != 1 )
		{
			nTickVal = round( ( double) i*tzoom_h );
			strTmp.Printf( wxT("%d"), nTickVal );
			sizeTxt = memDC.GetTextExtent( strTmp );
			
			memDC.DrawText( strTmp, nStartX-4-2-sizeTxt.GetWidth(), y-sizeTxt.GetHeight()/2 );
		}
	}

	// draw main vertical axe - center
//	memDC.SetPen( GetSkin().GetDarkGreyPen() );
//	memDC.DrawLine(nStartX + nWidth/2, nStartY, nStartX + nWidth/2, nStartY + nHeight);

	////////////////////////////////////
	// do the grid and ticks - horizontal
	int divs_w = 10;
	double step_w = (double) nWidth/divs_w;
	double nRealDataSize = (double) (m_nDataSize-1)/10.0;
	double tzoom_w = (double) ( (double) nWidth/ (double) (m_nDataSize-1)); //512.0/100.0;
	double tstep_w = (double) (nRealDataSize/divs_w);

	for( i=1; i<=divs_w; i++ )
	{
		// value
		dx = i * step_w;
		// calculate x
		x = nStartX + (int) round(dx);
		// draw vertical grid lines
		memDC.SetPen( GetSkin().GetGreyPen() );
		memDC.DrawLine(x, nStartY, x, nStartY + nHeight);
		// ticks - from center
		memDC.SetPen( GetSkin().GetBlackPen() );
		memDC.DrawLine(x, nStartY + nHeight +1, x, nStartY + nHeight+4 );

		// labels
		if( m_nFlags != 1 )
		{
			nTickVal = round( ( double) (tstep_w*i) );
			strTmp.Printf( wxT("%d"), nTickVal );
			sizeTxt = memDC.GetTextExtent( strTmp );

			if( i == 0 )
				memDC.DrawText( strTmp, x, START_Y+nHeight+4+sizeTxt.GetHeight()/2-2 );
			else if( i == divs_w )
				memDC.DrawText( strTmp, x-sizeTxt.GetWidth()+1, START_Y+nHeight+4+sizeTxt.GetHeight()/2-2 );
			else
				memDC.DrawText( strTmp, x-round(sizeTxt.GetWidth()/2)+2, START_Y+nHeight+4+sizeTxt.GetHeight()/2-2 );

			// check for start/end align
/*			if( i == 0 )
				memDC.DrawText( strTmp, x, nStarY+nHeight+4+sizeTxt.GetHeight()/2-2 );
			else if( i == divs )
				memDC.DrawText( strTmp, x-sizeTxt.GetWidth()+1, nStarY+nHeight+4+sizeTxt.GetHeight()/2-2 );
			else
				memDC.DrawText( strTmp, x-round(sizeTxt.GetWidth()/2)+2, nStarY+nHeight+4+sizeTxt.GetHeight()/2-2 ); */
		}
	}

	//////////////////////////////////
	// GRAPH :: draw graphs
	if( m_vectData != NULL )
	{
		// draw max
//		wxPen penMaxLine( *wxGREEN, 1, wxDOT ); 
//		memDC.SetPen( penMaxLine );
//		int nMaxLineY = nStartY + nHeight - 1 - round( d_size*height_ratio );
//		memDC.DrawLine( nStartX, nMaxLineY, nStartX+nWidth, nMaxLineY );

		// draw middle
//		wxPen penMidLine( GetSkin().GetYellow(), 1, wxDOT ); 
//		memDC.SetPen( penMidLine );
//		int nMidLineY = nStartY + nHeight - 1 - round( (d_size/2.0)*height_ratio );
//		memDC.DrawLine( nStartX, nMidLineY, nStartX+nWidth, nMidLineY );

		// draw lines
//		wxPen penCurveLine( *wxBLUE, 1, wxDOT ); 
//		memDC.SetPen( penCurveLine );
//		int nFwhm1 = nStartX + nShiftX + (int) round( ((m_nDataSize-2)*width_ratio )/2.0 - m_nFwhm*width_ratio*(10.0/2) );
//		int nFwhm2 = nStartX + nShiftX + (int) round( ((m_nDataSize-2)*width_ratio )/2.0 + m_nFwhm*width_ratio*(10.0/2) );
//		memDC.DrawLine( nFwhm1, nStartY, nFwhm1, nStartY + nHeight );
//		memDC.DrawLine( nFwhm2, nStartY, nFwhm2, nStartY + nHeight );

		///////////////////////
		// DRAW CURVES
		for( i=0; i<m_nData; i++ )
		{
			// calculate shift x??
			double d_size = m_vectData[i].max - m_vectData[i].min;
			double height_ratio = (double) ( (double)nHeight/(m_vectData[i].max+10.0) ); //d_size;
			double width_ratio = (double) ( (double) nWidth/(double) (m_nDataSize-1));
			int nShiftX = (int) round( ( (double) nWidth - (double) (m_nDataSize-2)*width_ratio )/2.0 );

			// plot graph curve i
			for( j=1; j<m_vectData[i].nPoints; j++ )
			{
				////////////////////
				// the graph curve[i]
				int y1 = (int) nStartY + nHeight - 1 - round( m_vectData[i].vectPoints[j-1]*height_ratio );
				int y2 = (int) nStartY + nHeight - 1 - round( m_vectData[i].vectPoints[j]*height_ratio );

				int x1 = (int) nStartX+nShiftX+(j-1)*width_ratio;
				int x2 = (int) nStartX+nShiftX+j*width_ratio;

				// set pen to graph by no red/green/blue...
				if( i == 0 )
					memDC.SetPen( GetSkin().GetRedPen() );
				else if( i == 1 )
					memDC.SetPen( GetSkin().GetGreenPen() );
				else if( i == 2 )
					memDC.SetPen( GetSkin().GetBluePen() );

				// draw line from point 1 to point 2	
				memDC.DrawLine( x1, y1, x2, y2 );

				// set spline
/*				if( i==1 )
				{
					vectPoints[0].x = x1;
					vectPoints[0].y = y1;
				}
				vectPoints[i].x = x2;
				vectPoints[i].y = y2;
*/
			}
			//memDC.DrawSpline( m_nDataSize, vectPoints );
			//free( vectPoints );
		}
	}

	dc.Blit(0, 0, nTotalW, nTotalH, &memDC, 0, 0, wxCOPY, FALSE);
} 

////////////////////////////////////////////////////////////////////
// Method:	OnSize
// Class:	CGraphBaseView
// Purose:	adjust on windows resize
// Input:	reference to size event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGraphBaseView::OnSize( wxSizeEvent& pEvent )
{
	m_nWidth = pEvent.GetSize().GetWidth()- (START_X+START_X)-6;
	m_nHeight = pEvent.GetSize().GetHeight()- (START_Y+LAST_Y)-6;

	//Layout();
	Refresh( FALSE );
}