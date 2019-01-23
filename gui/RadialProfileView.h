////////////////////////////////////////////////////////////////////
// Name:		radial profile window viewer header
// File:		RadialProfileView.h
// Purpose:		radial profile viewer
//
// Created by:	Larry Lart on 08-02-2009
// Updated by:	
//
// Copyright:	(c) 2009 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _RADIAL_PROFILE_VIEW
#define _RADIAL_PROFILE_VIEW
 
#include <wx/window.h>

class CRadialProfileView : public wxWindow
{
public:
	CRadialProfileView( wxWindow * iprParent, wxWindowID iId = wxID_ANY, 
					const wxPoint& iPosition = wxDefaultPosition, int nFlags=0 );
  
	~CRadialProfileView( );

	void SetData( wxImage& pImage, StarDef* pObject, unsigned char nObjType );

// public data
public:
	int m_nStartX;
	int m_nStartY;
	int m_nWidth;
	int m_nHeight;
	int m_nGraphHeight;

	int m_nFlags;

private:
	double* m_vectData;
	double* m_vectGaussData;
	unsigned int m_nDataSize;

	double m_nDataMax;
	double m_nDataMin;
	double m_nGaussDataMax;
	double m_nGaussDataMin;

	double m_nFwhm;
	float m_vectFitGlsGauss[4];

	bool _bColor;

	// event
	void OnPaint( wxPaintEvent& iEvent );
	void OnEraseBackground( wxEraseEvent& iEvent );
	void OnSize( wxSizeEvent& pEvent );

	DECLARE_EVENT_TABLE();
};

#endif


