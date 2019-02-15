////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// local headers
#include "../sky/star.h"
#include "../util/func.h"
#include "../image/astroimage.h"
#include "../sky/sky.h"
#include "../match/starmatch.h"
#include "astro_img_view.h"

// main header
#include "dist_tool_view.h"

BEGIN_EVENT_TABLE( CDistToolDataWindow, wxWindow )
	// window events
//	EVT_SIZE( CDistToolDataWindow::OnSize ) 
END_EVENT_TABLE()
// class :: CDistToolDataWindow
////////////////////////////////////////////////////////////////////
// Purose:	Constructor
////////////////////////////////////////////////////////////////////
CDistToolDataWindow::CDistToolDataWindow( CSky* pSky, CAstroImgView *pImgView, 
										 const wxPoint& pos, const wxSize& size ):
										wxWindow( pImgView, -1, pos, size, wxSIMPLE_BORDER )
{
	m_pImgView = pImgView;
	m_pSky = pSky;
	// init data
	m_pAstroImage = NULL;
	m_nDistanceToolA_X = 10;
	m_nDistanceToolA_Y = 10;
	m_nDistanceToolB_X = 200;
	m_nDistanceToolB_Y = 200;
	m_nDistanceToolSizePixels = 0.0000;
	m_nDistanceToolSizeAngle = 0;
	m_nDistanceToolSizeObj = 0;
	m_nDistanceToolSizeGalactic = 0;
	m_nDistanceToolSizeExtraGalactic = 0;
	m_nEffectiveFocalLength = 0.0;

	// init GUI layout
	wxGridSizer *topsizer = new wxGridSizer( 1, 1, 2, 2 );

	wxStaticBox* pDataBox = new wxStaticBox( this, -1, wxT("Distance Measurements"), 
					wxPoint(2,0), 
					wxSize( size.GetWidth()-5, size.GetHeight()-3 ) );
	wxStaticBoxSizer* sizerDataPage = new wxStaticBoxSizer( pDataBox, wxHORIZONTAL );
	// tabular data sizer
	wxFlexGridSizer* sizerTabData = new wxFlexGridSizer( 3, 2, 4, 2 );
	sizerTabData->AddGrowableCol( 1 );
	// add to data box
	sizerDataPage->Add( sizerTabData, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10 );

	/////////////////
	// init my fields
	m_pDistancePixel = new wxStaticText( this, -1, wxT("0.0"));
	m_pDistanceAnglular = new wxStaticText( this, -1, wxT("0.0"));
	m_pDistanceLY = new wxStaticText( this, -1, wxT("0.0"));
	m_pAvgGalDistanceLY = new wxStaticText( this, -1, wxT("0.0"));
	m_pAvgExtraGalDistanceLY = new wxStaticText( this, -1, wxT("0.0"));
	m_pEffectiveFocalLength = new wxStaticText( this, -1, wxT("0.0"));

	/////////////
	// add data to sizer
	// :: pixel
	sizerTabData->Add( new wxStaticText( this, -1, wxT("Pixels: ")), 0, wxALIGN_RIGHT | wxALIGN_BOTTOM );
	sizerTabData->Add( m_pDistancePixel, 0, wxALIGN_LEFT | wxALIGN_BOTTOM  );
	// :: angular
	sizerTabData->Add( new wxStaticText( this, -1, wxT("Angular: ")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerTabData->Add( m_pDistanceAnglular, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: distance between Light Years
	sizerTabData->Add( new wxStaticText( this, -1, wxT("Between Objects(ly): ")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerTabData->Add( m_pDistanceLY, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: Average Galactic Light Years
	sizerTabData->Add( new wxStaticText( this, -1, wxT("Avg. Galactic(ly): ")), 0, wxALIGN_RIGHT | wxALIGN_TOP );
	sizerTabData->Add( m_pAvgGalDistanceLY, 0, wxALIGN_LEFT | wxALIGN_TOP  );
	// :: Average extra-galactic Light Years
	sizerTabData->Add( new wxStaticText( this, -1, wxT("Avg. Extra-Galactic(ly): ")), 0, wxALIGN_RIGHT | wxALIGN_TOP );
	sizerTabData->Add( m_pAvgExtraGalDistanceLY, 0, wxALIGN_LEFT | wxALIGN_TOP  );
	// :: Effective Focal Length
	sizerTabData->Add( new wxStaticText( this, -1, wxT("Effective Focal Length: ")), 0, wxALIGN_RIGHT | wxALIGN_TOP );
	sizerTabData->Add( m_pEffectiveFocalLength, 0, wxALIGN_LEFT | wxALIGN_TOP  );

	// add this to main/top sizer
	topsizer->Add( sizerDataPage, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxGROW | wxALL, 2 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	Layout();
//	topsizer->SetSizeHints(this);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CDistToolDataWindow
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CDistToolDataWindow::~CDistToolDataWindow( )
{
	delete( m_pDistancePixel );
	delete( m_pDistanceAnglular );
	delete( m_pDistanceLY );
	delete( m_pAvgGalDistanceLY );
	delete( m_pAvgExtraGalDistanceLY );
	delete( m_pEffectiveFocalLength );
}

////////////////////////////////////////////////////////////////////
// Method:	SetAstroImage
// Class:	CDistToolDataWindow
// Purose:	set astro image to measure
// Input:	pointer to astro image
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDistToolDataWindow::SetAstroImage( CAstroImage* pAstroImage )
{
	m_pAstroImage = pAstroImage;
	// set tool init position - in the middle
	int w = m_pAstroImage->m_nWidth;
	int h = m_pAstroImage->m_nHeight;
	SetPointA( (int) w/2-w/6, (int) h/2 );
	SetPointB( (int) w/2+w/6, (int) h/2 );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetPointA
// Class:	CDistToolDataWindow
// Purose:	set coord for point A
// Input:	float x/y in pixels
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDistToolDataWindow::SetPointA( double x, double y )
{
	m_nDistanceToolA_X = x;
	m_nDistanceToolA_Y = y;
	SetDataLabels( );
}

////////////////////////////////////////////////////////////////////
// Method:	SetPointB
// Class:	CDistToolDataWindow
// Purose:	set coord for point B
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDistToolDataWindow::SetPointB( double x, double y )
{
	m_nDistanceToolB_X = x;
	m_nDistanceToolB_Y = y;
	SetDataLabels( );
}

////////////////////////////////////////////////////////////////////
// Method:	SetPixelsLabel
// Class:	CDistToolDataWindow
// Purose:	set label distance in pixels
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDistToolDataWindow::SetPixelsLabel( )
{
	wxString strTmp;

	CalcDistInPixels( );
	strTmp.Printf( wxT("%.10f"), m_nDistanceToolSizePixels );
	m_pDistancePixel->SetLabel( strTmp );
}

////////////////////////////////////////////////////////////////////
// Method:	SetObjLabel
// Class:	CDistToolDataWindow
// Purose:	set label distance when linked to at least one object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDistToolDataWindow::SetObjLabel( )
{
	wxString strTmp;

	CalcDistWithObj( );
	strTmp.Printf( wxT("%.5f"), m_nDistanceToolSizeObj );
	m_pDistanceLY->SetLabel( strTmp );
}

////////////////////////////////////////////////////////////////////
// Method:	SetGalacticLabel
// Class:	CDistToolDataWindow
// Purose:	set label distance in out galactic plane (near)
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDistToolDataWindow::SetGalacticLabel( )
{
	wxString strTmp;

	CalcDistGalactic( );
	strTmp.Printf( wxT("%.8f"), m_nDistanceToolSizeGalactic );
	m_pAvgGalDistanceLY->SetLabel( strTmp );
}

////////////////////////////////////////////////////////////////////
// Method:	SetExtraGalacticLabel
// Class:	CDistToolDataWindow
// Purose:	set label distance in the extra-galactic plane (far)
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDistToolDataWindow::SetExtraGalacticLabel( )
{
	wxString strTmp;

	CalcDistExtraGalactic( );
	strTmp.Printf( wxT("%.4f"), m_nDistanceToolSizeExtraGalactic );
	m_pAvgExtraGalDistanceLY->SetLabel( strTmp );
}

////////////////////////////////////////////////////////////////////
void CDistToolDataWindow::SetEffectiveFocalLength( )
{
	wxString strTmp;

	CalcEffectiveFocalLength( );
	strTmp.Printf( wxT("%.1f"), m_nEffectiveFocalLength );
	m_pEffectiveFocalLength->SetLabel( strTmp );
}

////////////////////////////////////////////////////////////////////
// Method:	CalcDistInPixels
// Class:	CDistToolDataWindow
// Purose:	set distance in pixels
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
double CDistToolDataWindow::CalcDistInPixels( )
{
	// calculate distance
	m_nDistanceToolSizePixels = sqrt( pow( (m_nDistanceToolB_X-m_nDistanceToolA_X), 2 ) +
									pow( (m_nDistanceToolB_Y-m_nDistanceToolA_Y), 2 ) );

	return( m_nDistanceToolSizePixels );
}

////////////////////////////////////////////////////////////////////
// Method:	SetAngularLabel
// Class:	CDistToolDataWindow
// Purose:	set label distance angular
// Input:	float distance in pixels
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDistToolDataWindow::SetAngularLabel( )
{
	wxString strTmp;

	CalcDistAngular( );
	strTmp.Printf( wxT("%.10f"), m_nDistanceToolSizeAngle );
	m_pDistanceAnglular->SetLabel( strTmp );
}

////////////////////////////////////////////////////////////////////
// Method:	CalcDistAngular
// Class:	CDistToolDataWindow
// Purose:	set distance in degrees in the sky
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
double CDistToolDataWindow::CalcDistAngular( )
{
	if( !m_pAstroImage->m_bIsMatched ) return( 0 );

	StarDef star;

	/////////////////////
	// deproject point A
	double a_x = m_nDistanceToolA_X;
	double a_y = m_nDistanceToolA_Y;
	m_pSky->CalcPointTrans( a_x, a_y );
	a_x /= m_pAstroImage->m_nSkyProjectionScale;
	a_y /= m_pAstroImage->m_nSkyProjectionScale;
	// set to deproject
	star.x=a_x; star.y=a_y;
	m_pSky->DeprojectStar( star, m_pAstroImage->m_nOrigRa, m_pAstroImage->m_nOrigDec );
	double a_ra = star.ra;
	double a_dec = star.dec;

	/////////////////////
	// deproject point B
	double b_x = m_nDistanceToolB_X;
	double b_y = m_nDistanceToolB_Y;
	m_pSky->CalcPointTrans( b_x, b_y );
	b_x /= m_pAstroImage->m_nSkyProjectionScale;
	b_y /= m_pAstroImage->m_nSkyProjectionScale;
	// set to deproject
	star.x=b_x; star.y=b_y;
	m_pSky->DeprojectStar( star, m_pAstroImage->m_nOrigRa, m_pAstroImage->m_nOrigDec );
	double b_ra = star.ra;
	double b_dec = star.dec;

	// calculate distance
	m_nDistanceToolSizeAngle = CalcSkyDistance( a_ra, a_dec, b_ra, b_dec );

	return( m_nDistanceToolSizeAngle );
}

////////////////////////////////////////////////////////////////////
// Method:	CalcDistWithObj
// Class:	CDistToolDataWindow
// Purose:	calculate distance in between two objects or one object
//			and average in light years
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
double CDistToolDataWindow::CalcDistWithObj( )
{
	m_nDistanceToolSizeObj = 0;
	double nDistA = 0;
	double nDistB = 0;

	// image view remotes - todo :: pass this from image view instead 
	long nLastSelectedA = m_pImgView->m_nLastSelectedA;
	unsigned char nLastSelectedTypeA = m_pImgView->m_nLastSelectedTypeA;
	long nLastSelectedB = m_pImgView->m_nLastSelectedB;
	unsigned char nLastSelectedTypeB = m_pImgView->m_nLastSelectedTypeB;

	// check if details defined or not
	if( ( ( nLastSelectedTypeA == 1 || nLastSelectedTypeB == 1 ) && !m_pAstroImage->m_vectStar ) || 
		( ( nLastSelectedTypeA == 2 || nLastSelectedTypeB == 2 ) && !m_pAstroImage->m_vectCatStar ) ||
		( ( nLastSelectedTypeA == 3 || nLastSelectedTypeB == 3 ) && !m_pAstroImage->m_vectCatDso ) ) 
	{
		return( 0 );
	}

	// check point A
	if( nLastSelectedA >= 0 )
	{
		if( nLastSelectedTypeA == 1 )
			nDistA = m_pAstroImage->m_vectStar[nLastSelectedA].distance;
		else if( nLastSelectedTypeA == 2 )
			nDistA = m_pAstroImage->m_vectCatStar[nLastSelectedA].distance;
		else if( nLastSelectedTypeA == 3 )
			nDistA = m_pAstroImage->m_vectCatDso[nLastSelectedA].distance;
	}
	// check point B
	if( nLastSelectedB >= 0 )
	{
		if( nLastSelectedTypeB == 1 )
			nDistB = m_pAstroImage->m_vectStar[nLastSelectedB].distance;
		else if( nLastSelectedTypeB == 2 )
			nDistB = m_pAstroImage->m_vectCatStar[nLastSelectedB].distance;
		else if( nLastSelectedTypeB == 3 )
			nDistB = m_pAstroImage->m_vectCatDso[nLastSelectedB].distance;
	}
	// now if one is zero 
	if( nDistA == 0 && nDistB != 0 )
	{
		// set A to average by type
		if( nLastSelectedTypeB == 1 || nLastSelectedTypeB == 2 )
			nDistA = m_pAstroImage->m_nDistanceAverageGalactic;
		else if( nLastSelectedTypeB == 3 )
			nDistA = m_pAstroImage->m_nDistanceAverageExtraGalactic;

	} else if( nDistA != 0 && nDistB == 0 )
	{
		// set B to average by type
		if( nLastSelectedTypeA == 1 || nLastSelectedTypeA == 2 )
			nDistB = m_pAstroImage->m_nDistanceAverageGalactic;
		else if( nLastSelectedTypeA == 3 )
			nDistB = m_pAstroImage->m_nDistanceAverageExtraGalactic;

	} else if( nDistA == 0 && nDistB == 0 )
	{
		return( 0 );
	}

	//////////////////////////
	// calculate distance - triangle law of cosines
	double ang_rad = (double) m_nDistanceToolSizeAngle*DEG2RAD;
	m_nDistanceToolSizeObj = sqrt( nDistA*nDistA + nDistB*nDistB - 2.0*nDistA*nDistB*cos( ang_rad ) );

	return( m_nDistanceToolSizeObj );
}

////////////////////////////////////////////////////////////////////
// Method:	CalcDistGalactic
// Class:	CDistToolDataWindow
// Purose:	calculate distance in the galactic plane
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
double CDistToolDataWindow::CalcDistGalactic( )
{
	m_nDistanceToolSizeGalactic = 0;
	if( m_pAstroImage->m_nDistanceAverageGalactic == 0 ) return( 0 );

	double half_ang_rad = (double) (m_nDistanceToolSizeAngle/2.0)*DEG2RAD;
	double avgdist = m_pAstroImage->m_nDistanceAverageGalactic;
	m_nDistanceToolSizeGalactic = (double) (avgdist * tan( half_ang_rad )) * 2.0;

	return( m_nDistanceToolSizeGalactic );
}

////////////////////////////////////////////////////////////////////
// Method:	CalcDistExtraGalactic
// Class:	CDistToolDataWindow
// Purose:	calculate distance in the extra-galactic plane
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
double CDistToolDataWindow::CalcDistExtraGalactic( )
{
	m_nDistanceToolSizeExtraGalactic = 0;
	if( m_pAstroImage->m_nDistanceAverageExtraGalactic == 0 ) return( 0 );

	double half_ang_rad = (double) (m_nDistanceToolSizeAngle/2.0)*DEG2RAD;
	double avgdist = m_pAstroImage->m_nDistanceAverageExtraGalactic;
	m_nDistanceToolSizeExtraGalactic = (double) (avgdist * tan( half_ang_rad )) * 2.0;

	return( m_nDistanceToolSizeExtraGalactic );
}

////////////////////////////////////////////////////////////////////
double CDistToolDataWindow::CalcEffectiveFocalLength( )
{
	double w = (double) m_pAstroImage->m_nWidth;
	double h = (double) m_pAstroImage->m_nHeight;


/* METHOD 1 
	// values: 1, 2, 3, 4 (4x4)
	double pixel_bin = 1.;
	// canon default
	double ccd_pix = 7.38;
//	double obj_pix = m_nDistanceToolSizePixels;
	double obj_pix = sqrt(w*w+h*h);
	//  sky field of view in arc seconds = deg*60min*60sec
	double sky_fov = m_nDistanceToolSizeAngle * 3600.;

	m_nEffectiveFocalLength = (ccd_pix * pixel_bin * obj_pix * 206.265) / sky_fov;
*/
	double sky_fov = m_nDistanceToolSizeAngle;
	// default width for canon in mm
	double sensor_size = sqrt(22.2*22.2 + 15.1*15.1); //22.2;

	m_nEffectiveFocalLength = sensor_size / (2. * tan(PI * sky_fov / 360.));
	//f =  f / crop;


	return( m_nEffectiveFocalLength );
}

////////////////////////////////////////////////////////////////////
// Method:	SetDataLabels
// Class:	CDistToolDataWindow
// Purose:	set all data labels
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDistToolDataWindow::SetDataLabels( )
{
	SetPixelsLabel( );
	SetAngularLabel( );
	SetObjLabel( );
	SetGalacticLabel( );
	SetExtraGalacticLabel( );
	SetEffectiveFocalLength( );

	return;
}

/*
////////////////////////////////////////////////////////////////////
void CDistToolDataWindow::OnSize( wxSizeEvent& even )
{
//	int nWidth = even.GetSize().GetWidth();
//	int nHeight = even.GetSize().GetHeight();

	// adjust distance tool position
	Move(m_pImgView->m_nWidth-240, 1);
}
*/
