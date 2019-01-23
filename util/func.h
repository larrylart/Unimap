////////////////////////////////////////////////////////////////////
// Package:		functions declaration
// File:		func.h
// Purpose:		build a library of global functions
//
// Created by:	Larry Lart on 22-04-2006
// Updated by:  
//
// Copyright:	(c) 2006-2008 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _FUNC_H
#define _FUCN_H

#include "wx/wxprec.h"
#include "wx/thread.h"
#include <wx/textctrl.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "../lib/fftw/api/fftw3.h"

#include <cv.h>

// local includes
#include "../sky/star.h"
#include "../match/starmatch.h"

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679
#define HALF_PI 1.5707963267948966192313216916398
#define PI_FACT 0.017453292519943295769236907684883
//#define DEG2RAD 0.017453292519943295769236907684883
#define DEG2RAD 0.0174532925199432957692369076848861271344287188854172545609719144017100911460344944368224156963450948
//#define RAD2DEG 57.295779513082320876798154814105
#define RAD2DEG 57.2957795130823208767981548141051703324054724665643215491602438612028471483215526324409689958511109456
// units defines
#define UNIT_DISTANCE_PARSEC_LY			3.26
#define UNIT_DISTANCE_PARSEC_AU			206265

// DEBUG DEFINES
#define DEBUG_SHOW_PROC_TIMES_ON_FILE_LOAD	1

#define  HLSMAX   255
#define  RGBMAX   255
#define UNDEFINED (HLSMAX*2/3)

// define functions bzero for windows
#ifdef WIN32 
static void bzero( char* pString, unsigned int nString ){ memset( pString, 0, nString*sizeof(char) ); }
static void bzero( wxChar* pString, unsigned int nString ){ memset( pString, 0, nString*sizeof(wxChar) ); }
#endif

// define sleep
#ifndef WIN32 
#include <unistd.h>
void Sleep( unsigned long nTimeout );
#endif

///////////////////////////////
// MACROS
#define round(x) (x<0?ceil((x)-0.5):floor((x)+0.5))

static inline double cot(double d) { return (1 / tan(d)); }

// define get uniq time ID
long GetUniqTimeId( );
// get double time
double GetDTime( void );

long GetFileSize( FILE* pFile );

// get random
long GetRandInt( long start, long end );
double GetRandFloat( double start, double end );

// --these I should probably collect into a different file skycalc.h/cpp

// transform ra to sexagesimal
int RaDegToSexagesimal( double nRa, wxString& strRa, wxChar delm=wxT(':') );
// transform dec to sexagesimal
int DecDegToSexagesimal( double nDec, wxString& strDec, wxChar delm=wxT(':') );
// transform field deg in sexagesimal
int DegToHHMMSS( double nDeg, wxString& strDeg );
// adjust over the to ra/dec
int AdjustRaDec( double& nX, double& nY );

/////////////////
// imported from catalog - cleanup
int ConvertRaToDeg( wxString& strRa, double& nRa );
int ConvertDecToDeg( wxString& strDec, double& nDec );

// calculate sky disance
double CalcSkyDistance( double nRa1, double nDec1, double nRa2, double nDec2 );

//--------------- end sect here

// Makes sIn suitable for inclusion in an URL, by escaping all chars that could cause trouble.
wxString URLEncode(const wxString& sIn);
// clean file names
wxString CleanupFilename(const wxString& filename, bool keepSpaces, bool fat32);

// reset star vector
void ResetStarVector( StarDef* vectStar, long nStar );
void ResetStarVectorMatch( StarDef* vectStar, long nStar );

// claculate planar distance
double CalcPlanarDistance( double x1, double y1, double x2, double y2 );
// check if point in the area by radius
static inline int IsPointInRadius( double x, double y, double origX, double origY, int nRadius )
{
	// calculate distance between my center and mouse position
	double xDiff = x-origX;
	double yDiff = y-origY;
	double nDistSq = xDiff*xDiff + yDiff*yDiff;
	double nDist = sqrt( nDistSq );

	if( nDist < nRadius )
		return( 1 );
	else
		return( 0 );

}


// write match trans to a file
int WriteTransToFile( FILE* pFile, TRANS* pTrans, const char* strLabel );
// extract a trans from a string
int GetTransFromString( const char* strTrans, TRANS* pTrans );

// get ellipse bounding box
void ellipseBoundingBox( double x, double y, double a, double b, double ang_deg, double& w, double& h );

// color conversion
void HSVtoRGB( float hsv[3], float rgb[3] );
void CalcRGBFromY( float y_val, float y_range, float y_min, float rgb[3] );

//////////////////////
// :: some macros
static inline double sqr(double x) { return x*x; }
// tps base funtion
static inline double TpsBaseFunc( double r )
{
  if( r == 0.0 )
    return( 0.0 );
  else
    return( r*r * log(r) );
}

///////////////////////////////////////
// opencv<->wxwigets conversions
IplImage* WxImageToIplImage(const wxImage& wx_Image);
wxImage* IplImageToWxImage( const IplImage* cv_Image );

/////////
// some templates
template <class T> class OpImage
{
private:  
	IplImage *imgp;public:  OpImage(IplImage *img = 0) {imgp = img;}  
	~OpImage() {imgp = 0;}  
	void operator = (IplImage *img){imgp = img;}  
	inline T* operator[](const int rowIndx)
	{      
		return ((T*)(imgp->imageData + rowIndx *imgp ->widthStep));
	}
};
typedef struct{  unsigned char b, g, r;}RgbPixel;
typedef struct{  float b, g, r;}RgbPixelFloat;
typedef OpImage<RgbPixel>      RgbImage;
typedef OpImage<RgbPixelFloat> RgbImageFloat;
typedef OpImage<unsigned char> BwImage;
typedef OpImage<float>         BwImageFloat;

///////////////////////////////////////////////
// check if point in ellipse
static inline int IsPointInEllipse( double px, double py, 
									double x, double y, double a, double b, double ang, double min_dist )
{
	double nSR = a < b ? a : b;

	// calculate distance between my star and mouse position
	double xDiff = px-x;
	double yDiff = py-y;
	double nDistSq = xDiff*xDiff + yDiff*yDiff;
	double nDist = sqrt( nDistSq );

	if( nDist > nSR || nDist > min_dist ) return( 0 );

	// now here we do the ellipse heavy stuff
	double ang_rad = (180.0-ang)*DEG2RAD;
	double sin_a = sin( ang_rad );
	double cos_a = cos( ang_rad );
	double ap = y*((1-cos_a*cos_a)/sin_a)-(x-px)*cos_a-py*sin_a;
	double ap2 = ap*ap;
	double bp = sqrt( nDistSq - ap2 );
	// and now calc my fact
	double kDist = ap2/(a*a) + (bp*bp)/(b*b);

	if( kDist <= 1 )
		return( 1 );
	else
		return( 0 );
}

///////////////////////////////////
// string encoding/decoding
void XORString( const char* str_in, const char* str_key, char* retval );
void TEncryptString( const char* str_in, const char* str_key, char* str_out );
void TDecryptString( const char* str_in, const char* str_key, char* str_out );


#endif
