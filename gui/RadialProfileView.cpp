////////////////////////////////////////////////////////////////////
// Name:		radial profile window viewer implementation
// File:		RadialProfileView.cpp
// Purpose:		radial profile viewer
//
// Created by:	Larry Lart on 08-02-2009
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
#include "RadialProfileView.h"

// local defines
#define START_X   (5)
#define START_Y   (7)
#define LAST_Y    (5)
#define BAND_W    (256*2)
#define BAND_H    (128+64)
#define TOTAL_W   (START_X + BAND_W + START_X)
#define TOTAL_H   (START_Y + BAND_H + LAST_Y)

BEGIN_EVENT_TABLE( CRadialProfileView, wxWindow )
	EVT_PAINT( CRadialProfileView::OnPaint )
	EVT_ERASE_BACKGROUND( CRadialProfileView::OnEraseBackground )
	EVT_SIZE( CRadialProfileView::OnSize ) 
END_EVENT_TABLE( )
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CRadialProfileView
// Purpose:	Initialize object
// Input:	pointer to reference window, etc
// Output:	nothing
////////////////////////////////////////////////////////////////////
CRadialProfileView::CRadialProfileView( wxWindow* iprParent, wxWindowID iId, const wxPoint& iPosition , int nFlags ) :
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
	m_vectGaussData = NULL;
	m_nDataSize = 0;
} 

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CRadialProfileView
// Purpose:	Delete my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CRadialProfileView::~CRadialProfileView( )
{
	free( m_vectData );
	free( m_vectGaussData );
} 

////////////////////////////////////////////////////////////////////
// Method:	SetData
// Class:	CRadialProfileView
// Purpose:	set my radial profile plot data
// Input:	pointer to radial plot data and size
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CRadialProfileView::SetData( wxImage& pImage, StarDef* pObject, unsigned char nObjType )
{
	int i=0;
	m_nFwhm = pObject->fwhm;

	double a = pObject->kronfactor ? pObject->a*pObject->kronfactor : pObject->a;
	double b = pObject->kronfactor ? pObject->b*pObject->kronfactor : pObject->b;

	double nResFact = 10.0;
	unsigned int nRing = (unsigned int) round( nResFact*(a+10) );
	double* vectRing = (double*) malloc( 2*nRing*sizeof( double ) );
	//unsigned int nRing=0;
	getObjectRadialProfile( pImage, pObject->x, pObject->y, a, b, pObject->theta, vectRing, nRing );

	m_vectData = (double*) malloc( 2*nRing*sizeof( double ) );
	m_vectGaussData = (double*) malloc( 2*nRing*sizeof( double ) );
	m_nDataSize = 2*nRing;

	m_nDataMax = DBL_MIN;
	m_nDataMin = DBL_MAX;

	for( i=0; i<2*nRing-1; i++ )
	{
		if( vectRing[i] > m_nDataMax ) m_nDataMax = vectRing[i];
		if( vectRing[i] < m_nDataMin ) m_nDataMin = vectRing[i];
		// set current 
		//m_vectData[nRing-1-i] = vectRing[i];
		m_vectData[i] = vectRing[i];
		// set mirror 
		//m_vectData[nRing-1+i] = vectRing[i];
	}
	free( vectRing );

	///////////////
	// calc gauss fit
	fitgsl_data	*d;
	int r;
	
	d = fitgsl_alloc_data( m_nDataSize-1 );
	// populate input vector
	for( i=0; i<m_nDataSize-1; i++ )
	{
		d->pt[i].x = i;
		d->pt[i].y = (float) (m_vectData[i]-m_nDataMin);
	}
	// fit gauss curve
	r = fitgsl_lm(d, m_vectFitGlsGauss, 0);
	////////////////
	// populate gauss vector
	m_nGaussDataMax = DBL_MIN;
	m_nGaussDataMin = DBL_MAX;
	for( i=0; i<m_nDataSize-1; i++ )
	{
		m_vectGaussData[i] = fitgsl_fx( m_vectFitGlsGauss[FITGSL_B_INDEX], m_vectFitGlsGauss[FITGSL_P_INDEX], 
								m_vectFitGlsGauss[FITGSL_C_INDEX], m_vectFitGlsGauss[FITGSL_W_INDEX], i );
		// get min max
		if( m_vectGaussData[i] > m_nGaussDataMax ) m_nGaussDataMax = m_vectGaussData[i];
		if( m_vectGaussData[i] < m_nGaussDataMin ) m_nGaussDataMin = m_vectGaussData[i];
	}
	///////////////////
	// reset my data to zero
	for( i=0; i<m_nDataSize-1; i++ )
	{
		m_vectData[i] = m_vectData[i] - m_nDataMin;
		m_vectGaussData[i] = m_vectGaussData[i] - m_nDataMin;
	}

	// get drawing are best sizes	
	if( m_nDataMax < m_nGaussDataMax ) 
		m_nGraphHeight = (int) round( m_nGaussDataMax - m_nDataMin );
	else
		m_nGraphHeight = (int) round( m_nDataMax - m_nDataMin );

	fitgsl_free_data(d);
} 

////////////////////////////////////////////////////////////////////
// Method:	OnEraseBackground
// Class:	CRadialProfileView
// Purpose:	nothing to todo = no more flicking
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CRadialProfileView::OnEraseBackground( wxEraseEvent& WXUNUSED(iEvent) )
{
	// nothing to todo = no more flicking
} 

////////////////////////////////////////////////////////////////////
// Method:	OnPaint
// Class:	CRadialProfileView
// Purpose:	paint
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CRadialProfileView::OnPaint( wxPaintEvent& WXUNUSED(iEvent) )
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
	int i, nTickVal;
	const double zoomx = 1.0 * double(nWidth) / 512.0;
	
	double tzoom_h = 256.0/10.0;

	///////////////////////
	// ticks :: vertical
	for( i=0; i<=divs_h; i++ )
	{
		dy = i*step_h;
		y = nStartY + nHeight - (int) round(dy);
		// ticks
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
	memDC.SetPen( GetSkin().GetDarkGreyPen() );
	memDC.DrawLine(nStartX + nWidth/2, nStartY, nStartX + nWidth/2, nStartY + nHeight);
	// central tick / label = 0 
	memDC.SetPen( GetSkin().GetBlackPen() );
	memDC.DrawLine( nStartX + nWidth/2, nStartY + nHeight+1, nStartX + nWidth/2, nStartY + nHeight+4 );
	strTmp.Printf( wxT("%d"), 0 );
	sizeTxt = memDC.GetTextExtent( strTmp );
	memDC.DrawText( strTmp, nStartX + nWidth/2 - sizeTxt.GetWidth()/2, nStartY+nHeight+4+sizeTxt.GetHeight()/2-2 );

	////////////////////////////////////
	// do the grid and ticks - horizontal
	int divs_w2 = 5;
	double step_w = (double) (nWidth/2)/divs_w2;
	double nRealDataSize = (double) (m_nDataSize-1)/10.0;
	double tzoom_w = (double) ( (double) nWidth/ (double) (m_nDataSize-1)); //512.0/100.0;
	double tstep_w = (double) ((nRealDataSize/2)/divs_w2);

	for( i=1; i<=divs_w2; i++ )
	{
		// value
		dx = i * step_w;
		// labels
		if( m_nFlags != 1 )
		{
			nTickVal = round( ( double) (tstep_w*i) );
			strTmp.Printf( wxT("%d"), nTickVal );
			sizeTxt = memDC.GetTextExtent( strTmp );

			// ticks - from center
			memDC.SetPen( GetSkin().GetBlackPen() );
			// :: right
			x = nStartX + nWidth/2 + (int) round(dx);
			memDC.DrawLine(x, nStartY + nHeight +1, x, nStartY + nHeight+4 );
			if( i == divs_w2 )
				memDC.DrawText( strTmp, x-sizeTxt.GetWidth()+1, nStartY+nHeight+4+sizeTxt.GetHeight()/2-2 );
			else
				memDC.DrawText( strTmp, x-sizeTxt.GetWidth()/2, nStartY+nHeight+4+sizeTxt.GetHeight()/2-2 );
			// :: left
			x = nStartX + nWidth/2 - (int) round(dx);
			memDC.DrawLine(x, nStartY + nHeight+1, x, nStartY + nHeight+4 );
			if( i == divs_w2 )
				memDC.DrawText( strTmp, x+1, nStartY+nHeight+4+sizeTxt.GetHeight()/2-2 );			
			else
				memDC.DrawText( strTmp, x-sizeTxt.GetWidth()/2, nStartY+nHeight+4+sizeTxt.GetHeight()/2-2 );
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
	// GRAPH :: draw radial profile
	if( m_vectData != NULL )
	{
		double d_size = m_nDataMax - m_nDataMin;
		double height_ratio = (double) ( (double)nHeight/300.0); //d_size;
		double width_ratio = (double) ( (double) nWidth/(double) (m_nDataSize-1));
		int nShiftX = (int) round( ( (double) nWidth - (double) (m_nDataSize-2)*width_ratio )/2.0 );

		// draw max
		wxPen penMaxLine( *wxGREEN, 1, wxDOT ); 
		memDC.SetPen( penMaxLine );
		int nMaxLineY = nStartY + nHeight - 1 - round( d_size*height_ratio );
		memDC.DrawLine( nStartX, nMaxLineY, nStartX+nWidth, nMaxLineY );
		// draw middle
		wxPen penMidLine( GetSkin().GetYellow(), 1, wxDOT ); 
		memDC.SetPen( penMidLine );
		int nMidLineY = nStartY + nHeight - 1 - round( (d_size/2.0)*height_ratio );
		memDC.DrawLine( nStartX, nMidLineY, nStartX+nWidth, nMidLineY );
		// draw fwhm
		wxPen penFwhmLine( *wxBLUE, 1, wxDOT ); 
		memDC.SetPen( penFwhmLine );
		int nFwhm1 = nStartX + nShiftX + (int) round( ((m_nDataSize-2)*width_ratio )/2.0 - m_nFwhm*width_ratio*(10.0/2) );
		int nFwhm2 = nStartX + nShiftX + (int) round( ((m_nDataSize-2)*width_ratio )/2.0 + m_nFwhm*width_ratio*(10.0/2) );
		memDC.DrawLine( nFwhm1, nStartY, nFwhm1, nStartY + nHeight );
		memDC.DrawLine( nFwhm2, nStartY, nFwhm2, nStartY + nHeight );

//		wxPoint* vectPoints = (wxPoint*) malloc( (m_nDataSize+1)*sizeof(wxPoint) );

		for( i=1; i<m_nDataSize; i++ )
		{
			int y1 = (int) nStartY + nHeight - 1 - round( m_vectData[i-1]*height_ratio );
			int y2 = (int) nStartY + nHeight - 1 - round( m_vectData[i]*height_ratio );

			int x1 = (int) nStartX+nShiftX+(i-1)*width_ratio;
			int x2 = (int) nStartX+nShiftX+i*width_ratio;
			// set pen to graph
			memDC.SetPen( GetSkin().GetRedPen() );
			memDC.DrawLine( x1, y1, x2, y2 );

			// plot gauss - if not out down
			if( m_vectGaussData[i-1] >= 0 && m_vectGaussData[i] >= 0 )
			{
				y1 = (int)  nStartY + nHeight - 1 - round(m_vectGaussData[i-1]*height_ratio);
				y2 = (int)  nStartY + nHeight - 1 - round(m_vectGaussData[i]*height_ratio);
				// set pen to graph
				memDC.SetPen( GetSkin().GetDarkGreyPen() );
				memDC.DrawLine(  x1, y1,  x2, y2 );
			}

			// set spline
/*			if( i==1 )
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

	dc.Blit(0, 0, nTotalW, nTotalH, &memDC, 0, 0, wxCOPY, FALSE);
} 

////////////////////////////////////////////////////////////////////
// Method:	OnSize
// Class:	CRadialProfileView
// Purose:	adjust on windows resize
// Input:	reference to size event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CRadialProfileView::OnSize( wxSizeEvent& pEvent )
{
	m_nWidth = pEvent.GetSize().GetWidth()- (START_X+START_X)-6;
	m_nHeight = pEvent.GetSize().GetHeight()- (START_Y+LAST_Y)-6;

	//Layout();
	Refresh( FALSE );
}