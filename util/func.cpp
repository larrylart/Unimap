////////////////////////////////////////////////////////////////////
// Package:		functions implementation
// File:		func.cpp
// Purpose:		build a library of global functions
//
// Created by:	Larry Lart on 22/04/2006
// Updated by:  Larry Lart on 18/08/2010
//
// Copyright:	(c) 2006-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cctype>

// wx
#include <wx/filename.h>
#include <wx/url.h>
#include "../httpengine/base64.h" 

//#include "StringFunctions.h"
#include "highgui.h"

// main header
#include "func.h"

// define functions for windows
#ifdef _WINDOWS

#endif

// define function Sleep if not windows
#ifndef _WINDOWS
void Sleep( unsigned long nTimeout )
{ 
	usleep( nTimeout );
}
#endif

// define get uniq id function based on time and string
long GetUniqTimeId( )
{
	time_t seconds;
	seconds = time (NULL);

	return( (long) seconds );
}

////////////////////////////////////////////////////////////////////
// Method:	GetFileSize
long GetFileSize( FILE* pFile )
{
	long nCount = -1;

	fseek( pFile, 0, SEEK_END );
	nCount = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );

	return( nCount );
}

////////////////////////////////////////////////////////////////////
long GetRandInt( long start, long end )
{
	srand( (unsigned int) GetUniqTimeId() );

	long inter = abs(end-start);
	long nRand = rand() % inter;
	// increment with start
	nRand += start;
	// check if in range
	if( nRand < start ) nRand = start;
	if( nRand > end ) nRand = end;

	return( nRand );
}

// todo: implement
////////////////////////////////////////////////////////////////////
double GetRandFloat( double start, double end )
{

	return( start );
}

////////////////////////////////////////////////////////////////////
// Method:	GetTime
// Purose:	routine to get time accurate - this would be called by
//			all other components to keep in sync
// Input:	nothing
// Output:	number = accurate time count
////////////////////////////////////////////////////////////////////
double GetDTime( void )
{
	//
	struct timeb timeStamp;
	// get a time stamp
	ftime( &timeStamp );

	double nTime = timeStamp.time*1000 + timeStamp.millitm;

	return( nTime );
	// i remove this for the moment
	//return (double)cvGetTickCount()*1e-3/(m_nCpuFreq+1e-10);
}

////////////////////////////////////////////////////////////////////
// Method:	RaDegToSexagesimal
// Purose:	here we transform ra from deg in sexagismal string
// Input:	ra in deg and refernce to string to return
// Output:	status ?
////////////////////////////////////////////////////////////////////
int RaDegToSexagesimal( double nRa, wxString& strRa, wxChar delm )
{
	double ival = 0;
	long nLVal = 0;

	ival = nRa;
	if( ival < 0.) ival += 360.;
	if( ival > 360.) ival -= 360.;
	ival /= 15.;

	nLVal = (long) ( ival*(60. * 60. * 1000. + 0.001 ) ) ; //+ 0.5; // +5 or +50 ?
	strRa.Printf( wxT("%3.02ld%c%02ld%c%02ld.%03ld"),  nLVal / 3600000L, delm,
			(nLVal / 60000) % 60L, delm, (nLVal / 1000) % 60L, nLVal % 1000L );


	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DecDegToSexagesimal
// Purose:	here we transform dec from deg in sexagismal string
// Input:	dec in deg and refernce to string to return
// Output:	status ?
////////////////////////////////////////////////////////////////////
int DecDegToSexagesimal( double nDec, wxString& strDec, wxChar delm )
{
	long nLVal = 0;
	char chSign = '+';
	double ival = nDec;
	if( ival < 0 )
	{
		chSign = '-';
		ival = -ival;
	}

	nLVal = (long) ( ival * (60. * 60. * 1000. + 0.001) ); //+ 0.5; // +5 or +50 ?
	strDec.Printf( wxT("%c%02ld%c%02ld%c%02ld.%02ld"), chSign, nLVal / 3600000L, delm,
			(nLVal / 60000) % 60L, delm, (nLVal / 1000) % 60L, nLVal % 1000L );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DegToHHMMSS
// Purose:	here we transform from deg to sexagesimal format hhmmss
// Input:	deg number and refernce to string to return
// Output:	status ?
////////////////////////////////////////////////////////////////////
int DegToHHMMSS( double nDeg, wxString& strDeg )
{
	double ival = 0;
	long nLVal = 0;

	ival = nDeg;

	nLVal = (long) ( ival*(60. * 60. * 1000. + 0.001 ) ) ; //+ 0.5; // +5 or +50 ?
	strDeg.Printf( wxT("%02ld:%02ld:%02ld.%03ld"),  nLVal / 3600000L,
			(nLVal / 60000) % 60L, (nLVal / 1000) % 60L, nLVal % 1000L );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	ConvertRaToDeg
// Purpose:	convert ra from sexdecimal to degres
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int ConvertRaToDeg( wxString& strRa, double& nRa )
{
	double nHH=0, nMM=0, nSS=0;
	int nStatus = 0;

	nStatus = sscanf( strRa.ToAscii(), "%lf%*c%lf%*c%lf", &nHH, &nMM, &nSS );
	// check if read was ok
	if( nStatus == 0 || nStatus == EOF )
		return( 0 );

	// now calculate ra in deg
	nRa = (double) ( (double) nHH*15 + (double) nMM/4 + (double) nSS/240);
	// and return success
	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	ConvertDecToDeg
// Purpose:	convert dec from sexdecimal to degres 
//			this should work for lat/lon as well
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int ConvertDecToDeg( wxString& strDec, double& nDec )
{
	double nHH=0, nMM=0, nSS=0;
	double nh=0, nm=0;
	int nSign = 1;
	int nStatus = 0;

	strDec.Trim(0).Trim(1);
	nStatus = sscanf( strDec.ToAscii(), "%lf%*c%lf%*c%lf", &nHH, &nMM, &nSS );
	// check if read was ok
	if( nStatus == 0 || nStatus == EOF )
		return( 0 );

	// deal with the sign
	if( nHH < 0 || strDec[0] == wxT('-') )
	{
		nSign = -1;
		nHH = -nHH;
	}

	// now calculate ra in deg
	nDec = (double) (( double) (nSign*( (double) nHH + (double) nMM/60 + (double) nSS/3600) ) );
	// and return success
	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	AdjustRaDec
// Purose:	adjust over the top ra/dec
// Input:	reference to ra dec to adjust
// Output:	status ?
////////////////////////////////////////////////////////////////////
int AdjustRaDec( double& nX, double& nY )
{
	int bAdjust = 0;

	//////////////////////////////////
	// DEC - exception over 90/-90
	if( nY > 90.0 )
	{
		nY = 180.0 - nY;
		// calculate over the top nX - mirror
		nX = 180.0 + nX;

		bAdjust = 1;

	} else if( nY < -90.0 )
	{
		nY = -180.0 - nY;
		// calculate over the top nX - mirror
		nX = 180.0 + nX;

		bAdjust = 1;
	}
	///////////////////////////////
	// RA exception over 360 or less the 0
	if( nX > 360.0 )
	{
		nX = nX - 360.0;
		bAdjust = 1;

	} else if( nX < 0.0 )
	{
		nX = 360.0 - fabs( nX );
		bAdjust = 1;
	}

	return( bAdjust );
}

// Implementation of the non-inlines
static byte base16Chars[17] = "0123456789ABCDEF";

////////////////////////////////////////////////////////////////////
// Method:	URLEncode
// Purose:	encode string for http request
// Input:	string
// Output:	status ?
////////////////////////////////////////////////////////////////////
wxString URLEncode( const wxString& sIn )
{
	wxString sOut;
	unsigned char curChar;
	
	for ( unsigned int i = 0; i < sIn.Length(); ++i )
	{
		curChar = sIn.GetChar( i );

		if( isalnum( curChar ) )
		{
	        sOut += curChar;
	    } else if( isspace ( curChar ) )
		{
		    sOut += wxT("+");
		} else
		{
			sOut += wxT("%");
			sOut += base16Chars[ curChar >> 4];
			sOut += base16Chars[ curChar & 0xf];
		}

	}

	return sOut;
}

////////////////////////////////////////////////////////////////////
// Method:	CleanupFilename
// Purose:	Strips specific chars to ensure legal filenames
// Input:	string file name
// Output:	status ?
////////////////////////////////////////////////////////////////////
wxString CleanupFilename(const wxString& filename, bool keepSpaces, bool fat32)
{
//#ifdef __WXMSW__
//	fat32 = true;
//#endif
	fat32 = false;
	wxString result;

	for ( unsigned int i = 0; i < filename.Length(); i++ )
	{
		switch ( filename[ i ] )
		{
			case wxT('/'):
				continue;
				
			case wxT('\"'):
			case wxT('*'):
			case wxT('<'):
			case wxT('>'):
			case wxT('?'):
			case wxT('|'):
			case wxT('\\'):
			case wxT(':'):
				if (fat32)
				{
					continue;
				}
				
			case wxT(' '):
				if ( !keepSpaces )
				{
					result += wxT('_');
					continue;
				}
				
			default:
				// Many illegal for filenames in windows below the 32th char (which is space).
				if ( (wxUChar) filename[i] > 31 )
				{
						result += filename[i];
				}
		}
	}

	return result;
}

////////////////////////////////////////////////////////////////////
// Method:	ResetStarVector
// Purose:	reset all vars of a star vector
// Input:	star vector pointer and size
// Output:	void
////////////////////////////////////////////////////////////////////
void ResetStarVector( StarDef* vectStar, long nStar )
{
	for( int k=0; k<nStar; k++ )
	{
		vectStar[k].cat_name = NULL;
		vectStar[k].match = 0;
		vectStar[k].ra = 0;
		vectStar[k].dec = 0;
		vectStar[k].state = 0;
		vectStar[k].x = 0;
		vectStar[k].y = 0;
		vectStar[k].mag = 0;
		vectStar[k].cat_no = 0;
		vectStar[k].cat_type = 0;
		vectStar[k].comp_no = 0;
		vectStar[k].img_id = 0;
		vectStar[k].cat_id = 0;
		vectStar[k].type = 0;
		vectStar[k].zone_no = 0;
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	ResetStarVectorMatch
// Purose:	reset all vars of a star vector
// Input:	star vector pointer and size
// Output:	void
////////////////////////////////////////////////////////////////////
void ResetStarVectorMatch( StarDef* vectStar, long nStar )
{
	for( int k=0; k<nStar; k++ )
	{
		vectStar[k].match = 0;
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	CalcSkyDistance
// Purose:	calculate distance between two points in the sky
// Input:	ra/dec for both points
// Output:	distance angle - degrees
////////////////////////////////////////////////////////////////////
double CalcSkyDistance( double nRa1, double nDec1, double nRa2, double nDec2 )
{
	double nOrigRa = nRa1;
	double nOrigDecRad = (double) nDec1 * DEG2RAD;
	double nRaRad = (double) nRa2 * DEG2RAD;
	double nDecRad = (double) nDec2 * DEG2RAD;

	double nValue = sin(nOrigDecRad)*sin(nDecRad) + cos(nOrigDecRad)*cos(nDecRad)*cos((nOrigRa-nRa2)*DEG2RAD);
	// correction
	if( nValue > 1.0 || nValue < -1.0 ) nValue = 1.0;

	double nDistRad = (double) acos( nValue );
	double nDistDeg = nDistRad*RAD2DEG;

	// other method
	// degrees(asin(sqrt((sin(0.5*radians(69.06529472-de2000)))^2 +cos(radians(69.06529472))*cos(radians(de2000))*(sin(0.5*radians(148.88822108-al2000*15)))^2)))

	///////
	// use a second formula if angle small
/*	double nDeltaRA = nXRad-nOrigXRad;
	double nDeltaDec = nYRad - nOrigYRad;
	double nDecZero = (nYRad + nOrigYRad)/2.0;
	if( nRadiusRad < PI/90 )
		nRadiusRad = (double) asin( sqrt( sin(nDeltaRA)*sin(nDeltaRA)*cos(nDecZero)*cos(nDecZero) +
									sin(nDeltaDec)*sin(nDeltaDec) ) );
*/
	return( nDistDeg );
}

////////////////////////////////////////////////////////////////////
// Method:	CalcPlanarDistance
// Purose:	calculate distance between two points in the planar space
// Input:	x/y for both points
// Output:	distance pixels
////////////////////////////////////////////////////////////////////
double CalcPlanarDistance( double x1, double y1, double x2, double y2 )
{
	double nDist = 0.0;

	double a = (double) (x2-x1);
	double b = (double) (y2-y1);

	nDist = (double) sqrt( (double) (a*a) + (double) (b*b) );

	return( nDist );
}

////////////////////////////////////////////////////////////////////
// Method:	WriteTransToFile
// Purose:	write frame to frame transfomation in file
// Input:	file pointer, trans, label to use  
// Output:	status
////////////////////////////////////////////////////////////////////
int WriteTransToFile( FILE* pFile, TRANS* pTrans, const char* strLabel )
{
	fprintf( pFile, "%s%d,%d,%d,%d,%lf,%lf,%lf,", strLabel, pTrans->id, 
					pTrans->order, pTrans->nr, pTrans->nm, 
					pTrans->sig, pTrans->sx, pTrans->sy );
	fprintf( pFile, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", 
					pTrans->a, pTrans->b, pTrans->c,
					pTrans->d, pTrans->e, pTrans->f, 
					pTrans->g, pTrans->h, pTrans->i,
					pTrans->j, pTrans->k, pTrans->l,
					pTrans->m, pTrans->n, pTrans->o, pTrans->p );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	GetTransFromString
// Purose:	get  transfomation from a string
// Input:	string trans, pointer trans
// Output:	status
////////////////////////////////////////////////////////////////////
int GetTransFromString( const char* strTrans, TRANS* pTrans )
{
	if( !sscanf( strTrans, 
		"%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", 
			&(pTrans->id), 
			&(pTrans->order), &(pTrans->nr), &(pTrans->nm), 
			&(pTrans->sig), &(pTrans->sx), &(pTrans->sy),
			&(pTrans->a), &(pTrans->b), &(pTrans->c),
			&(pTrans->d), &(pTrans->e), &(pTrans->f), 
			&(pTrans->g), &(pTrans->h), &(pTrans->i),
			&(pTrans->j), &(pTrans->k), &(pTrans->l),
			&(pTrans->m), &(pTrans->n), &(pTrans->o), &(pTrans->p) ) )
	{
		pTrans->id = 0;
		return( 0 );
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Convert wxImage to IplImage format
////////////////////////////////////////////////////////////////////
IplImage* WxImageToIplImage(const wxImage& wx_Image)
{
	//if (wx_Image == NULL) return NULL ;

	int nWidth = wx_Image.GetWidth ();
	int nHeight = wx_Image.GetHeight ();   
	IplImage* cv_Image = NULL ;   
	cv_Image = cvCreateImage(cvSize(nWidth, nHeight), IPL_DEPTH_8U, 3);   
	char* pcvImgData = cv_Image->imageData ;   
	memcpy((void*)pcvImgData, (void*)wx_Image.GetData(), nWidth*nHeight*3) ;   
	cvCvtColor(cv_Image, cv_Image, CV_BGR2RGB); 
	//the 2nd argument return 
	//must cast to IplImage* type for executing correctly. CvArr* type   
	return( (IplImage*)cv_Image );  
}

////////////////////////////////////////////////////////////////////
// Convert IplImage to wxImage format
////////////////////////////////////////////////////////////////////
wxImage* IplImageToWxImage( const IplImage* cv_Image )
{   
	if (cv_Image == NULL) return NULL ;   
	int nWidth = cv_Image->width ;   
	int nHeight = cv_Image->height;   
	wxImage* wx_Image = new wxImage() ;   
	IplImage* cv_ImageBuf = cvCreateImage (cvSize(nWidth, nHeight), IPL_DEPTH_8U, 3) ;   
	cvCvtColor((CvArr*)cv_Image, cv_ImageBuf, CV_BGR2RGB);   
	wx_Image->SetData((unsigned char*)cv_ImageBuf->imageData, nWidth, nHeight, false) ;   
	return( wx_Image );
}

////////////////////////////////////////////////////////////////////
// HSV to RGB calculator
////////////////////////////////////////////////////////////////////
void HSVtoRGB( float hsv[3], float rgb[3] )
{
	float h=0.0, s=0.0, v=0.0;			// hue, sat, value
	float r=0.0, g=0.0, b=0.0;			// red, green, blue
	float i=0.0, f=0.0, p=0.0, q=0.0, t=0.0;		// interim values


	// guarantee valid input:

	h = hsv[0] / 60.;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;


	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}


	// get an rgb from the hue itself:
	
	i = floor( h );
	f = h - i;
	p = v * ( 1. - s );
	q = v * ( 1. - s*f );
	t = v * ( 1. - ( s * (1.-f) ) );

	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}
	// set rgb
	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;

	return;
}

////////////////////////////////////////////////////////////////////
// calculate rgb from Y
////////////////////////////////////////////////////////////////////
void CalcRGBFromY( float y_val, float y_range, float y_min, float rgb[3] )
{
	// fix saturation, value (brightness)
	float sat = 2.0;
	float brightness_max = (float) 1.2;
	float hsv[3];

	// calc hue
	float hue = ((y_val - y_min)/y_range) * 360;

	// shift up hue a little bit - here to chaneg to start from a lighter blue
	hue += 80;

	// lets make it dimmer if lower color, but max out
	float value = (hue/130) * brightness_max;

	// clip so it's in range 0 - 360
	if( hue > 360 )
		hue = 360;
	else if( hue < 100 )
		hue = 100;

	// we want to go the other way, change the angle
	hue = 360 - hue;

	// convert to rgb
	hsv[0] = hue;
	hsv[1] = sat;
	hsv[2] = value;
	HSVtoRGB( hsv, rgb );

	return;
}

////////////////////////////////////////////////////////////////////
// get ellipse bounding box
////////////////////////////////////////////////////////////////////
void ellipseBoundingBox( double x, double y, double a, double b, double ang_deg, double& w, double& h )
{
	// check if no a or b
	a = a != 0 ? a : b;
	b = b != 0 ? b : a;
	if( a == 0 && b == 0 ) return;

	// test bounding box
//	int i=0;
	double ang_rad = ang_deg*DEG2RAD;
	double tan_a = (double) tan(ang_rad);
	double cotan_a = (double) 1.0/tan_a;

//	double nMaxW=DBL_MIN, nMaxH=DBL_MIN;
//	for( i=0; i<=3; i++ )
//	{
	double t = atan2( -b*tan_a, a ); // + i*HALF_PI;
	double e_x = x + a*cos(t)*cos(ang_rad) - b*sin(t)*sin(ang_rad);
	t = atan2( b*cotan_a, a ); // + i*HALF_PI;
	double e_y = y + b*sin(t)*cos(ang_rad) + a*cos(t)*sin(ang_rad);

	// get return
	w = (double) fabs(e_x-x);
	h = (double) fabs(e_y-y);

//	int n_w = abs(e_x-x);
//	int n_h = abs(e_y-y);
		// get max
//		if( n_w > nMaxW ) nMaxW = n_w;
//		if( n_h > nMaxH ) nMaxH = n_h;
//	}

//	w = nMaxW;
//	h = nMaxH;
}


////////////////////////////////////////////////////////////////////
void XORString( const char* str_in, const char* str_key, char* retval )
{
//	char retval[255];
	bzero( retval, 255 );
    strcpy( retval, str_in );

    short unsigned int vlen = strlen( str_in );
	short unsigned int klen = strlen( str_key );
    short unsigned int k = 0;
    short unsigned int v = 0;
    
    for( v=0; v<vlen; v++ )
    {
        retval[v]=str_in[v]^str_key[k];
        k=(++k<klen?k:0);
    }
    
//	wxString strReturn = wxString(retval,wxConvUTF8);
//    return( strReturn );
}

////////////////////////////////////////////////////////////////////
void TEncryptString(const char* str_in, const char* str_key, char* str_out )
{
	char str_tmp[255];
	// apply xor with the key then encode base64
	XORString( str_in, str_key, str_tmp );
	wxString strOut =  wxBase64Encode( wxString(str_tmp,wxConvUTF8) );

	strcpy( str_out, strOut.ToAscii() );
//	return( str_out );
}

////////////////////////////////////////////////////////////////////
void TDecryptString(const char* str_in, const char* str_key, char* str_out )
{
	// then apply xor again
	wxString strTmp = wxBase64Decode( wxString(str_in,wxConvUTF8) );
	XORString( strTmp.ToAscii(), str_key, str_out );

//	return( str_out );
}

