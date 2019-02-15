////////////////////////////////////////////////////////////////////
// Name:		histogram window viewer implementation
// File:		HistogramView.cpp
// Purpose:		histogram viewer
//
// Created by:	Larry Lart on 06-02-2009
// Updated by:	
//
// Copyright:	(c) 2009 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include <wx/dcmemory.h>
#include <math.h>
#include <elx/core/CoreMacros.h>

// local headers
#include "../util/func.h"
#include "designSkin.h"
#include "../image/histogram.h"

// main header
#include "HistogramView.h"

// local defines
#define START_X   (5)
#define START_Y   (3)
#define LAST_Y    (5)
#define BAND_W    (256*2)
#define BAND_H    (128+64)
#define TOTAL_W   (START_X + BAND_W + START_X)
#define TOTAL_H   (START_Y + BAND_H + LAST_Y)

// name spaces
using namespace eLynx;

BEGIN_EVENT_TABLE( CHistogramView, wxWindow )
	EVT_PAINT( CHistogramView::OnPaint )
	EVT_ERASE_BACKGROUND( CHistogramView::OnEraseBackground )
	EVT_SIZE( CHistogramView::OnSize ) 
END_EVENT_TABLE( )
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CHistogramView
// Purpose:	Initialize object
// Input:	pointer to reference window, etc
// Output:	nothing
////////////////////////////////////////////////////////////////////
CHistogramView::CHistogramView( wxWindow* iprParent, const CImageHistogram* iprHistogram,
								const AbstractTransfertFunction* iprTransfertFunction,
								bool ibColor, wxWindowID iId, const wxPoint& iPosition, int nFlags ) :
									wxWindow( iprParent, iId, iPosition, wxDefaultSize, //wxSize(TOTAL_W, TOTAL_H),
											 wxRAISED_BORDER ),
							_prHistogram(iprHistogram),
							_prTransfertFunction(iprTransfertFunction),
							_bColor(ibColor)
{
	// init size
	SetMinSize( wxSize( 160, 120 ) );
//	SetClientSize(TOTAL_W, TOTAL_H);
	// init vars
	m_nStartX = START_X;
	m_nStartY = START_Y;
	m_nWidth = BAND_W;
	m_nHeight = BAND_H;

	m_nFlags = nFlags;
} 

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CHistogramView
// Purpose:	Delete my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CHistogramView::~CHistogramView( )
{
} 

////////////////////////////////////////////////////////////////////
// Method:	UpdateHistogram
// Class:	CHistogramView
// Purpose:	update histogram from reference pointer
// Input:	pointer to histogram and refresh flag
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CHistogramView::UpdateHistogram( const CImageHistogram* iprHistogram, bool ibRefresh )
{
	_prHistogram = iprHistogram;
	if (ibRefresh) Refresh(false);
} 

////////////////////////////////////////////////////////////////////
// Method:	GetHistogram
// Class:	CHistogramView
// Purpose:	get current histogram
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
const CImageHistogram* CHistogramView::GetHistogram( ) const
{
	return _prHistogram;
} 

////////////////////////////////////////////////////////////////////
// Method:	UpdateTransfertFunction
// Class:	CHistogramView
// Purpose:	updater transfer function
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CHistogramView::UpdateTransfertFunction( const AbstractTransfertFunction* iprTransfertFunction, bool ibRefresh )
{
	_prTransfertFunction = iprTransfertFunction;
	elxASSERT(NULL != _prTransfertFunction);

	if( ibRefresh ) Refresh(false);
} 

////////////////////////////////////////////////////////////////////
// Method:	OnEraseBackground
// Class:	CHistogramView
// Purpose:	nothing to todo = no more flicking
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CHistogramView::OnEraseBackground( wxEraseEvent& WXUNUSED(iEvent) )
{
	// nothing to todo = no more flicking
} 

////////////////////////////////////////////////////////////////////
// Method:	OnPaint
// Class:	CHistogramView
// Purpose:	paint
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CHistogramView::OnPaint( wxPaintEvent& WXUNUSED(iEvent) )
{
	int nTotalW = START_X + m_nWidth + START_X;
	int nTotalH = START_Y + m_nHeight + LAST_Y;

	int nTicksSpace = 14;
	// case: if 1 space for labels = 1
	if( m_nFlags == 1 ) nTicksSpace = 1;

	int nHeight = m_nHeight - nTicksSpace;
	wxString strTmp;
	wxFont smallFont( 7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT );

	wxPaintDC dc(this);
	wxMemoryDC memDC;
	memDC.SetFont( smallFont );

	wxBitmap bitmap( nTotalW, nTotalH );
	memDC.SelectObject( bitmap );

	// clear all
	memDC.SetPen( GetSkin().GetBackgroundPen() );
	memDC.SetBrush( GetSkin().GetBackgroundBrush() );
	memDC.DrawRectangle(0, 0, nTotalW, nTotalH );

	// clear histogram
	memDC.SetPen( GetSkin().GetBlackPen() );
	memDC.SetBrush( GetSkin().GetGreyBrush() );
	memDC.DrawRectangle(START_X-1, START_Y-1, m_nWidth+2, nHeight+2 );

	// draw tag
	uint x,h;
	int divs = 10;
	double step = double(m_nWidth-1) / divs;
	double dx;
	int i;
	const double zoomx = 1.0 * double(m_nWidth) / 512.0;
	double tzoom = 512.0/10.0;

	// do the grid and ticks
	wxSize sizeTxt;
	for( i=0; i<=divs; i++ )
	{
		dx = 0.5 + i * step;
		x = START_X + int(dx);

		memDC.SetPen( GetSkin().GetWhitePen() );
		memDC.DrawLine(x, START_Y, x, START_Y + nHeight);
		memDC.DrawLine(x, START_Y + nHeight+1, x, START_Y + nHeight+5 );
		// ticks
		memDC.SetPen( GetSkin().GetDarkGreyPen() );
		memDC.DrawLine(x+1, START_Y + nHeight+1, x+1, START_Y + nHeight+4 );
		// labels
		if( m_nFlags != 1 )
		{
			int nTickVal = round( ( double) i*tzoom );
			strTmp.Printf( wxT("%d"), nTickVal );
			sizeTxt = memDC.GetTextExtent( strTmp );
			// check for start/end align
			if( i == 0 )
				memDC.DrawText( strTmp, x, START_Y+nHeight+4+sizeTxt.GetHeight()/2-2 );
			else if( i == divs )
				memDC.DrawText( strTmp, x-sizeTxt.GetWidth()+1, START_Y+nHeight+4+sizeTxt.GetHeight()/2-2 );
			else
				memDC.DrawText( strTmp, x-round(sizeTxt.GetWidth()/2)+2, START_Y+nHeight+4+sizeTxt.GetHeight()/2-2 );
		}
	}

	// draw histogram bar
	if( NULL != _prHistogram )
	{
		memDC.SetPen( GetSkin().GetDarkGreyPen() );

		const uint * prMap = _prHistogram->GetChannel(0).GetMap();
		const uint min = _prHistogram->GetMin();
		uint max = _prHistogram->GetMax();
		if( min == max ) max++;

		const double lmin = ::log( 1.0 + double(min) );
		const double lmax = ::log( 1.0 + double(max-min) );
		const double zoom = 1.0 * double(nHeight-5) / (lmax-lmin);
		
		double d;
		// bars
		for( i=0; i<HistogramSamples; i++ )
		{
			x = START_X + i*zoomx;
			d = (::log(1.0 + double(prMap[i] - min)) ) * zoom;
			h = int(d + 0.5);
			if( h > nHeight ) h = nHeight;
			memDC.DrawRectangle( x, START_Y+nHeight - h, round(1.0*zoomx), h );
		}

		const uint nChannel = _prHistogram->GetChannelCount();
		if( 4 <= nChannel )
		{ 
			// curve at the top
			wxPen pen;
			for( uint c=1; c<nChannel; c++ )
			{
				prMap = _prHistogram->GetChannel(c).GetMap();
				if      (1==c) pen = GetSkin().GetRedPen();
				else if (2==c) pen = GetSkin().GetGreenPen();
				else if (3==c) pen = GetSkin().GetBluePen();
				else if (4==c) pen = GetSkin().GetWhiteColor();
				memDC.SetPen( pen );

				int xold = START_X;
				d = (::log(1.0 + double(prMap[0]) - min) ) * zoom;
				int y, yold = START_Y + nHeight - round(d);

				for( i=1; i<HistogramSamples; i++ )
				{
					x = START_X + i*zoomx;
					d = (::log(1.0 + double(prMap[i]) - min) ) * zoom;
					y = START_Y + nHeight - round(d)-1;
					//if( y >= nHeight ) y = nHeight;
					memDC.DrawLine(xold, yold, x, y);
					xold = x, yold = y;
				}
			}
		}
	}

	// draw transfert function
	if( NULL != _prTransfertFunction )
	{
		memDC.SetPen( GetSkin().GetYellowPen() );
		unsigned char src[256];
		for( i=0; i<256; i++ ) src[i] = (unsigned char)i;
		_prTransfertFunction->Transform( src, src, 256, RT_UINT8 );

		int y, w=1;
		int x0 = START_X;
		int y0 = START_Y+nHeight-1 - (nHeight * src[0] / 256);
		for( i=1; i<256; i++ )
		{
			if( i==255 ) w=2;
			x = START_X + i*2;
			y = START_Y+nHeight-1 -(nHeight * src[i] / 256);
			memDC.DrawLine(x0, y0, x+w, y);
			x0 = x;
			y0 = y;
		}
	}

	dc.Blit(0, 0, nTotalW, nTotalH, &memDC, 0, 0, wxCOPY, FALSE);
} 

////////////////////////////////////////////////////////////////////
// Method:	OnSize
// Class:	CHistogramView
// Purose:	adjust on windows resize
// Input:	reference to size event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CHistogramView::OnSize( wxSizeEvent& pEvent )
{
	m_nWidth = pEvent.GetSize().GetWidth()- (START_X+START_X)-6;
	m_nHeight = pEvent.GetSize().GetHeight()- (START_Y+LAST_Y)-6;

	//Layout();
	Refresh( FALSE );
}