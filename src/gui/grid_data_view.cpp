////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// wx includes
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/dcbuffer.h>

// main header
#include "../util/func.h"
#include "grid_data_view.h"

BEGIN_EVENT_TABLE( CGridDataView, wxWindow )
	EVT_PAINT( CGridDataView::OnPaint )
	EVT_SIZE( CGridDataView::OnSize )
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
CGridDataView::CGridDataView( wxWindow *parent, const wxPoint& pos, const wxSize& size ):
			wxWindow(parent, -1, pos, size, wxSIMPLE_BORDER )
{
	SetBackgroundStyle(  wxBG_STYLE_CUSTOM  );

	// set my canvas width/height
	m_nWidth = size.GetWidth( );
	m_nHeight = size.GetHeight( );

	m_bIsNewData = 0;
	m_bImageData = 0;

	m_vectCells = NULL;
	m_vectColHead = NULL;
	m_vectRowHead = NULL;

	m_nRows = 0;
	m_nCols = 0;
	m_nRowHead = 0;
	m_nColHead = 0;

	smallFont = wxFont( 7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT );
	smallBFont = wxFont( 7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );

	// create bitmap
	m_rBitmap = wxBitmap( m_nWidth, m_nHeight );
}

////////////////////////////////////////////////////////////////////
CGridDataView::~CGridDataView( )
{
	ClearGridData( );
}

////////////////////////////////////////////////////////////////////
void CGridDataView::ClearGridData( )
{
	if( m_vectCells ) free( m_vectCells );
	if( m_vectColHead ) free( m_vectColHead );
	if( m_vectRowHead ) free( m_vectRowHead );
	m_vectCells = NULL;
	m_vectColHead = NULL;
	m_vectRowHead = NULL;
}

////////////////////////////////////////////////////////////////////
void CGridDataView::OnPaint( wxPaintEvent& pEvent )
{
	// use this instead -- better ??
	wxAutoBufferedPaintDC dc(this);
	Draw( dc );
}

////////////////////////////////////////////////////////////////////
void CGridDataView::SetType( int type )
{
	if( type == 0 )
	{
		m_bImageData = 0;

	} else if( type == 1 )
	{
		m_bImageData = 1;
		ClearGridData( );
	}
}

////////////////////////////////////////////////////////////////////
void CGridDataView::InitGrid( int cols, int rows, int chead, int rhead )
{
	m_nRows = rows;
	m_nCols = cols;
	m_nMinColHead = chead;
	m_nMinRowHead = rhead;

	// allocate cell data strcuture
	m_vectCells = (DefDataGridCell*) calloc( m_nRows*m_nCols, sizeof(DefDataGridCell) );
	m_vectColHead = (DefDataGridCell*) calloc( m_nCols, sizeof(DefDataGridCell) );
	m_vectRowHead = (DefDataGridCell*) calloc( m_nRows, sizeof(DefDataGridCell) );

	// calculate sizes
	CalcSizes( );

	SetType( 0 );
}

////////////////////////////////////////////////////////////////////
void CGridDataView::SetCellLabel( int row, int col, const wxString& str, wxColour& color )
{
	if( !m_nRows || !m_nCols ) return;

	int id = row*m_nCols + col;
	wxStrcpy( m_vectCells[id].val, str );
	m_vectCells[id].bg_colour = color;
	m_bIsNewData = 1;
}

////////////////////////////////////////////////////////////////////
void CGridDataView::SetColLabel( int id, const wxString& str, wxColour& col )
{
	if( !m_nRows || !m_nCols ) return;

	wxStrcpy( m_vectColHead[id].val, str );
	m_vectColHead[id].bg_colour = col;
	m_bIsNewData = 1;
}

////////////////////////////////////////////////////////////////////
void CGridDataView::SetRowLabel( int id, const wxString& str, wxColour& col )
{
	if( !m_nRows || !m_nCols ) return;

	wxStrcpy( m_vectRowHead[id].val, str );
	m_vectRowHead[id].bg_colour = col;
	m_bIsNewData = 1;
}

////////////////////////////////////////////////////////////////////
void CGridDataView::CalcSizes( )
{
	if( !m_nRows || !m_nCols ) return;

	// :: calculate row header - horizontal label
	if( m_nMinRowHead == 0 )
	{
		// if zero = same size as col
		m_nCWidth = m_nWidth/(m_nCols+1);
		m_nRowHead = m_nWidth-(m_nCols*m_nCWidth);

	} else
	{
		m_nCWidth = (m_nWidth-m_nMinRowHead)/m_nCols;
		m_nRowHead = m_nWidth-(m_nCols*m_nCWidth)-2;
	}
	// calculate col header - vertical label
	if( m_nMinColHead == 0 )
	{
		m_nCHeight = m_nHeight/(m_nRows+1);
		m_nColHead = m_nHeight-(m_nRows*m_nCHeight);

	} else
	{
		m_nCHeight = (m_nHeight-m_nMinColHead)/m_nRows;
		m_nColHead = m_nHeight-(m_nRows*m_nCHeight)-2;
	}

	// set start point for cells(-headers) - 1 or 2 ???? ok ? should i cal before?
	m_nStartX = m_nWidth-(m_nCols*m_nCWidth)-2;
	m_nStartY = m_nHeight-(m_nRows*m_nCHeight)-2;

	m_bIsNewData = 1;
}

////////////////////////////////////////////////////////////////////
void CGridDataView::Draw( wxDC& dc )
{
	if( !dc.Ok( ) ){ return; }

	if( m_bIsNewData ) DrawGrid( );

	//////////////
	// draw bitmap on the canvas
	int x,y,w,h;
	// clear dc
	dc.Clear( );
	dc.GetClippingBox( &x, &y, &w, &h );
	dc.DrawBitmap( m_rBitmap, x, y );
}

////////////////////////////////////////////////////////////////////
void CGridDataView::DrawGrid( )
{	
	int i=0, j=0;

	wxMemoryDC memDC;
	memDC.SelectObject( m_rBitmap );
	// clear
	memDC.SetPen( *wxLIGHT_GREY_PEN );
	memDC.SetBrush( *wxWHITE_BRUSH );
	memDC.DrawRectangle(0, 0, m_nWidth, m_nHeight );

	memDC.SetFont( smallBFont );
	wxSize sizeTxt;

	///////////////////
	// DRAW :: col headers
	for( i=0; i<m_nCols; i++ )
	{
		int x = m_nStartX + i*m_nCWidth;
		int y = 0;

		// set brush
		memDC.SetBrush( wxBrush( m_vectColHead[i].bg_colour ) );
		// draw header cell
		memDC.DrawRectangle( x, 0, m_nCWidth+1, m_nColHead+1 );
		// draw text
		wxString strVal = m_vectColHead[i].val;
		sizeTxt = memDC.GetTextExtent( strVal );
		memDC.DrawText( strVal, 
						x+m_nCWidth/2-round(sizeTxt.GetWidth()/2), 
						y+m_nColHead/2-round(sizeTxt.GetHeight()/2) );
	}

	///////////////////
	// DRAW :: row headers
	for( i=0; i<m_nRows; i++ )
	{
		int x = 0;
		int y = m_nStartY + i*m_nCHeight;
		// set brush
		memDC.SetBrush( wxBrush( m_vectRowHead[i].bg_colour ) );
		// draw header cell
		memDC.DrawRectangle( 0, y, m_nRowHead+1, m_nCHeight+1 );
		// draw text
		wxString strVal = m_vectRowHead[i].val;
		sizeTxt = memDC.GetTextExtent( strVal );
		memDC.DrawText( strVal, 
						x+m_nRowHead/2-round(sizeTxt.GetWidth()/2), 
						y+m_nCHeight/2-round(sizeTxt.GetHeight()/2) );
	}

	memDC.SetFont( smallFont );

	///////////////////
	// DRAW :: data table
	// :: cols
	for( i=0; i<m_nCols; i++ )
	{
		int x = m_nStartX + i*m_nCWidth;

		// :: rows
		for( j=0; j<m_nRows; j++ )
		{
			int y = m_nStartY + j*m_nCHeight;
			// cal vector index
			int cnt = j*m_nCols + i;

			// set brush
			memDC.SetBrush( wxBrush( m_vectCells[cnt].bg_colour ) );
			// draw cell
			memDC.DrawRectangle( x, y, m_nCWidth+1, m_nCHeight+1 );
			// draw cell text
			wxString strVal = m_vectCells[cnt].val;
			sizeTxt = memDC.GetTextExtent( strVal );
			memDC.DrawText( strVal, 
							x+m_nCWidth/2-round(sizeTxt.GetWidth()/2), 
							y+m_nCHeight/2-round(sizeTxt.GetHeight()/2) );

		}
	}

	// draw memdc
//	dc.Blit(0, 0, m_nWidth, m_nHeight, &memDC, 0, 0, wxCOPY, FALSE);

	// reset flag
	m_bIsNewData = 0;

	return;
}

////////////////////////////////////////////////////////////////////
void CGridDataView::OnSize( wxSizeEvent& pEvent )
{
	m_nWidth = pEvent.GetSize().GetWidth();
	m_nHeight = pEvent.GetSize().GetHeight();

	// re-calculate sizes
	if( !m_bImageData )
	{
		CalcSizes( );
		// re-create bitmap
		m_rBitmap = wxBitmap( m_nWidth, m_nHeight );
	} 

	//Layout();
	Refresh( FALSE );
}

////////////////////////////////////////////////////////////////////
void CGridDataView::SetImageFromFile( const wxString& file )
{
//	wxImage img(file);

	SetType( 1 );

	wxMemoryDC memDC;
	memDC.SelectObject( m_rBitmap );
	// clear
	memDC.SetPen( *wxBLACK_PEN );
	memDC.SetBrush( *wxBLACK_BRUSH );
	memDC.DrawRectangle(0, 0, m_nWidth, m_nHeight );

	wxBitmap bitmap( file, wxBITMAP_TYPE_GIF );
	int x = m_nWidth/2 - bitmap.GetWidth()/2;
	// draw my image centered
	memDC.DrawBitmap( bitmap, x, 0 );

//	SetSize( m_rBitmap.GetWidth(), m_rBitmap.GetHeight() );

//	Layout();
//	Fit();
//	Update();
	Refresh( FALSE );
}
