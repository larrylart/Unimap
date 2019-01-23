
#ifndef _DISTANCE_TOOL_VIEW_H
#define _DISTANCE_TOOL_VIEW_H

// wx includes
#include "wx/wxprec.h"

// external classes
class CAstroImage;
class CAstroImgView;
class CSky;
//class CGUIFrame;

// class :: CDistToolDataWindow
////////////////////////////////////////////////
class CDistToolDataWindow : public wxWindow
{
// methods
public: 
	CDistToolDataWindow( CSky* pSky, CAstroImgView *pImgView, const wxPoint& pos, const wxSize& size );
	virtual ~CDistToolDataWindow( );

	void SetAstroImage( CAstroImage* pAstroImage );

	void SetPointA( double x, double y );
	void SetPointB( double x, double y );
	double CalcDistInPixels( );
	double CalcDistAngular( );
	double CalcDistWithObj( );
	double CalcDistExtraGalactic( );
	double CalcDistGalactic( );
	double CalcEffectiveFocalLength( );

	// labels sets
	void SetDataLabels( );
	void SetPixelsLabel( );
	void SetAngularLabel( );
	void SetObjLabel( );
	void SetGalacticLabel( );
	void SetExtraGalacticLabel( );
	void SetEffectiveFocalLength( );

// data
public:
	CAstroImage*	m_pAstroImage;
	CAstroImgView*	m_pImgView;
	CSky* m_pSky;

	wxStaticText*	m_pDistancePixel;
	wxStaticText*	m_pDistanceAnglular;
	wxStaticText*	m_pDistanceLY;
	wxStaticText*	m_pAvgGalDistanceLY;
	wxStaticText*	m_pAvgExtraGalDistanceLY;
	wxStaticText*	m_pEffectiveFocalLength;

	// data
	double m_nDistanceToolA_X;
	double m_nDistanceToolA_Y;
	double m_nDistanceToolB_X;
	double m_nDistanceToolB_Y;
	// distance calculation
	double m_nDistanceToolSizePixels;
	double m_nDistanceToolSizeAngle;
	double m_nDistanceToolSizeObj;
	double m_nDistanceToolSizeGalactic;
	double m_nDistanceToolSizeExtraGalactic;
	double m_nEffectiveFocalLength;

private:
//	void OnSize( wxSizeEvent& even );

	DECLARE_EVENT_TABLE()
};

#endif

