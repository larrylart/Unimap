////////////////////////////////////////////////////////////////////
// Name:		graph plot base viewer header
// File:		graph_base_view.h
// Purpose:		graph plot viewer
//
// Created by:	Larry Lart on 01-10-2009
// Updated by:	
//
// Copyright:	(c) 2009 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _GRAPH_BASE_VIEW_H
#define _GRAPH_BASE_VIEW_H
 
#include <wx/window.h>

// graph curve structure
typedef struct
{
	double* vectPoints;
	int		nPoints;
	int		allocated;
	double max;
	double min;

} DefGraphCurve;

class CGraphBaseView : public wxWindow
{
public:
	CGraphBaseView( wxWindow * iprParent, wxWindowID iId = wxID_ANY, 
					const wxPoint& iPosition = wxDefaultPosition, int nFlags=0 );
  
	~CGraphBaseView( );

	void SetConfig( int nGraph );
	void SetData( double* vectData, int nData, int nGraphId );

// public data
public:
	int m_nStartX;
	int m_nStartY;
	int m_nWidth;
	int m_nHeight;
	int m_nGraphHeight;

	int m_nFlags;

private:
	DefGraphCurve* m_vectData;
	unsigned int m_nData;
	// graph global data size points on horizontal
	int m_nDataSize;

	bool _bColor;

	// event
	void OnPaint( wxPaintEvent& iEvent );
	void OnEraseBackground( wxEraseEvent& iEvent );
	void OnSize( wxSizeEvent& pEvent );

	DECLARE_EVENT_TABLE();
};

#endif


