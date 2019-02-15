////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _GRID_DATA_VIEW_H
#define _GRID_DATA_VIEW_H

// wx includes
#include "wx/wxprec.h"
#include <wx/grid.h>

// gui ids
enum
{
};

// cell structure
typedef struct
{
	wxChar val[50];
	wxColour bg_colour;

} DefDataGridCell;

// external classes

// class:	CGridDataView
///////////////////////////////////////////////////////
class CGridDataView : public wxWindow
{
// methods
public:
    CGridDataView( wxWindow *parent, const wxPoint& pos, const wxSize& size );
	~CGridDataView( );

	void ClearGridData( );
	void SetType( int type );

	void InitGrid( int cols, int rows, int chead=0, int rhead=0 );
	void Draw( wxDC& dc );

	void SetCellLabel( int row, int col, const wxString& str, wxColour& color );
	void SetColLabel( int id, const wxString& str, wxColour& col );
	void SetRowLabel( int id, const wxString& str, wxColour& col );

	void SetImageFromFile( const wxString& file );

/////////////
// ::DATA
public:
	int m_nWidth;
	int m_nHeight;

	int m_nRows;
	int m_nCols;
	// cell size
	int m_nCWidth;
	int m_nCHeight;
	// start x/y for data grid - headers
	int m_nStartX;
	int m_nStartY;
	// col/row head
	int m_nMinColHead;
	int m_nColHead;
	int m_nMinRowHead;
	int m_nRowHead;

	// flags
	int m_bIsNewData;
	int m_bImageData;

private:
	void CalcSizes( );
	void DrawGrid( );
	// event handlers
	void OnPaint(wxPaintEvent& pEvent);
	void OnSize( wxSizeEvent& pEvent );

protected:
	wxBitmap	m_rBitmap;
	wxFont		smallFont;
	wxFont		smallBFont;

	// cells/heads
	DefDataGridCell* m_vectCells;
	DefDataGridCell* m_vectColHead;
	DefDataGridCell* m_vectRowHead;

	DECLARE_EVENT_TABLE()
};

#endif

