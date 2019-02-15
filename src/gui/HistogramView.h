////////////////////////////////////////////////////////////////////
// Name:		histogram window viewer header
// File:		HistogramView.h
// Purpose:		histogram viewer
//
// Created by:	Larry Lart on 06-02-2009
// Updated by:	
//
// Copyright:	(c) 2009 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _HISTOGRAM_VIEW
#define _HISTOGRAM_VIEW
 
#include <wx/window.h>
#include <elx/math/TransfertFunction.h>
//#include <elx/image/ImageHistogram.h>

class CImageHistogram;

using namespace eLynx::Math;
//using namespace eLynx::Image;

class CHistogramView : public wxWindow
{
public:
	CHistogramView( wxWindow * iprParent, const CImageHistogram * iprHistogram,
					const AbstractTransfertFunction * iprTransfertFunction,
					bool ibColor = true, wxWindowID iId = wxID_ANY, 
					const wxPoint& iPosition = wxDefaultPosition, int nFlags=0 );
  
	virtual ~CHistogramView( );

	const CImageHistogram* GetHistogram() const;
	void UpdateHistogram( const CImageHistogram* iprHistogram, bool ibRefresh=true );
	void UpdateTransfertFunction( const AbstractTransfertFunction* iprTransfertFunction, bool ibRefresh=true );

// public data
public:
	int m_nStartX;
	int m_nStartY;
	int m_nWidth;
	int m_nHeight;

	int m_nFlags;

private:
	const CImageHistogram* _prHistogram;
	const AbstractTransfertFunction* _prTransfertFunction;
	bool _bColor;

	// event
	void OnPaint( wxPaintEvent& iEvent );
	void OnEraseBackground( wxEraseEvent& iEvent );
	void OnSize( wxSizeEvent& pEvent );

	DECLARE_EVENT_TABLE();
};

#endif


