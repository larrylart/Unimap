////////////////////////////////////////////////////////////////////
// Package:		Star Catalog implementation
// File:		ctatlog.cpp
// Purpose:		manage star catalog
//
// Created by:	Larry Lart on 11-Nov-2004
// Updated by:	
//
// Copyright:	(c) 2004 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// opencv includes
#include "highgui.h"
#include "cv.h"
#include "cvaux.h"

#include "wx/wxprec.h"
#include "wx/thread.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/regex.h>
#include <wx/string.h>

// custom headers
#ifdef _WIN32
#include "../util/func.h"
#endif

#include "../logger/logger.h"
#include "../util/_hfti.h"

// main header
#include "catalog.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CStarCatalog
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CStarCatalog::CStarCatalog( )
{
	m_pLogger = NULL;

	DEBUG = 0;

	m_nStar = 0;
	m_nMaxMag = 0;
	m_nMinMag = 0;
	m_nMagLimitIdx = 0;
	// allocate the vector size
//	m_nSizeData = nData;
//	m_vectData = (int*) calloc( m_nSizeData+1, sizeof(int) );
//	// initialize the buffer's pointers
//	m_nData = 0;
//	m_nFirst = 0;
//	m_nLast = 0;
//	m_nCurrent = 0;

	////////////////////////////
	// allocate mempory for my star names
//	m_vectWord.Alloc( 7000 );
//	m_vectWord.Empty( );
	// fill my string array with nothing
	m_nWord = 0;
	for( int i=0; i<7000; i++ )
	{
		m_vectWord[i].sao_no = -1;
		bzero( m_vectWord[i].name, 100 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CStarCatalog
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CStarCatalog::~CStarCatalog()
{
//	m_vectWord.Clear();
//	free( m_vectData );
}

/////////////////////////////////////////////////////////////////////
// Method:	Load
// Class:	CStarCatalog
// Purpose:	load star catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CStarCatalog::Load( const char* strFile )
{
	FILE* pFile = NULL;
	char strLine[255];
	char strMsg[255];
	// define my regex: event id = name, lat, ra, mag, spect  
	wxRegEx reLine( "^SAO ([0-9]+)\\|([0-9\\.\\-]+)\\|([0-9\\.\\-]+)\\|([0-9\\.\\-]+)\\|([a-zA-Z0-9\\.\\-]+)" );

	m_nMinX = 99999999;
	m_nMinY = 99999999;
	m_nMaxX = -99999999;
	m_nMaxY = -99999999;

	// debug
//	m_pLogger->Log( "INFO :: Loading configuration." );
	// Read the index
	pFile = fopen( strFile, "r" );
	// check if there is any configuration to load
	if( !pFile )
	{
		// debug
//		sprintf( strMsg, "Warning :: could not open config file %s", strFile );
//		m_pLogger->Log( strMsg );
		return( -1 );
	}
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
		bzero( strLine, 255 );
		// get one line out of the config file
		fgets( strLine, 2000, pFile );

		// check if event list
		if( reLine.Matches( strLine ) )
		{
			wxString strStarNo = reLine.GetMatch(strLine, 1 );
			wxString strStarLat = reLine.GetMatch(strLine, 2 );
			wxString strStarAlt = reLine.GetMatch(strLine, 3 );
			wxString strStarMag = reLine.GetMatch(strLine, 4 );
			long nStarNo = 0;
			if( strStarNo.ToLong( &nStarNo ) )
			{
				double nLat =0, nAlt = 0, nMag = 0;
				strStarLat.ToDouble( &nLat );
				strStarAlt.ToDouble( &nAlt );
				strStarMag.ToDouble( &nMag );
				// mark star
				m_vectStar[m_nStar].sao_no = nStarNo;
				m_vectStar[m_nStar].y = nAlt;
				m_vectStar[m_nStar].x = nLat;
//				m_vectStar[m_nStar].x = (cos(nAlt)* sin(nLat) / (1 + cos(nAlt)*cos(nLat)))*STAR_MAP_SIZE;
//				m_vectStar[m_nStar].y = (sin(nAlt) / (1 + cos(nAlt)*cos(nLat)))*STAR_MAP_SIZE;
//				m_vectStar[m_nStar].x = (cos(nAlt)* sin(nLat))*STAR_MAP_SIZE;
//				m_vectStar[m_nStar].y = (sin(nAlt))*STAR_MAP_SIZE;
				m_vectStar[m_nStar].power = nMag;
				m_vectStar[m_nStar].angle_a = 0;
				m_vectStar[m_nStar].angle_b = 0;
				m_vectStar[m_nStar].angle_c = 0;
				m_vectStar[m_nStar].point_b = -1;
				m_vectStar[m_nStar].point_c = -1;


				// conver with alg 2
				ProjectStar( m_vectStar[m_nStar], 0 , 0 );
				//StoRect( (float&) m_vectStar[m_nStar].x, (float&) m_vectStar[m_nStar].y );

				m_vectStar[m_nStar].x = m_vectStar[m_nStar].x*100;
				m_vectStar[m_nStar].y = m_vectStar[m_nStar].y*100;

				// mind max and min
				if( m_vectStar[m_nStar].x < m_nMinX ) m_nMinX = m_vectStar[m_nStar].x;
				if( m_vectStar[m_nStar].x > m_nMaxX ) m_nMaxX = m_vectStar[m_nStar].x;

				if( m_vectStar[m_nStar].y < m_nMinY ) m_nMinY = m_vectStar[m_nStar].y;
				if( m_vectStar[m_nStar].y > m_nMaxY ) m_nMaxY = m_vectStar[m_nStar].y;

				// increase star counter
				m_nStar++;

			}

		}
	}
	// close my file
	fclose( pFile );

	return( 1 );

}


////////////////////////////////////////////////////////////
//StoRect
//  Spherical to Rect conversion, results in unit vector.
//  in puts in RA, Dec, outputs RECTVEX.x.y.z
//  RECTVEC conv is z = Dec=90, x =(RA=0Dec=0),...y = (RA=90Dec=0)
//  Uses shpere to rect eqs, corrects RA to phi convention,
//  so should probably be named CtoRect since it converts celest
//  coords to rect coord.
/////////////////////////////////////////////////////////////
void CStarCatalog::StoRect( float& nRA, float& nDec )
{
//	RECTVEC R;
	double theta,phi;
	double DtoR = ( 2 * 3.14159265) / 360;
	double RtoD = 1 / DtoR;
	theta = DtoR * nRA;
	phi = DtoR * (90 - fabs(nDec));

//	R.x = cos(theta) * sin(phi);
//	R.y = sin(theta) * sin(phi);
//	R.z = cos(phi);

	nRA = cos(theta) * sin(phi);
	nDec = sin(theta) * sin(phi);


//	if (Dec < 0)
//		R.z = -R.z;

//	return(R);
}


////////////////////////////////////////////////////////////
//AltAztoRect()
//AltAz to Rect conversion
//inputs Alt Az, outputs RECTXYZ.
//RECTVEC convention is;x = north, y= west, z=zenith
/////////////////////////////////////////////////////////////
void CStarCatalog::AltAztoRect( float& nAlt, float& nAz )
{

//	RECTVEC R;
	double theta,phi;
	double DtoR = ( 2 * 3.14159265) / 360;
	double RtoD = 1 / DtoR;
	theta = DtoR *(360.0 -  nAz);
	phi = DtoR * (90.0 - fabs(nAlt));

//	R.x = cos(theta) * sin(phi);
//	R.y = sin(theta) * sin(phi);
//	R.z = cos(phi);

	nAlt = cos(theta) * sin(phi);
	nAz = sin(theta) * sin(phi);

//	if (Alt < 0)
//		R.z = -R.z;

//	return(R);
}

/////////////////////////////////////////////////////////////////////
// Method:	ProjectStar
// Class:	CStarCatalog
// Purpose:	load star catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CStarCatalog::ProjectStar( StarDef& star, const double x0, const double y0)
{
   double dtheta = (star.x - x0) * (PI / 180.);
   double phi = star.y * (PI / 180.), phi0;
   double x, y, z, cos_phi0, sin_phi0, cos_phi, sin_phi;
   int is_drift_scan = 0;

   phi0 = y0 * (PI / 180.);           /* now we're all in radians */
   cos_phi0 = cos( phi0);
   if( is_drift_scan )
   {
      star.x = (star.x - x0) * cos_phi0;
      star.y -= y0;

	} else
	{
      sin_phi0 = sin( phi0);
      cos_phi = cos( phi);
      sin_phi = sin( phi);
      x = sin( dtheta) * cos_phi;
      y = cos_phi0 * sin_phi - cos_phi *  cos( dtheta) * sin_phi0;
      z = sqrt( 1. - x * x - y * y);
#ifndef STEREOGRAPHIC_INSTEAD_OF_TANGENT_PLANE
      z = 2. / (1. + z);
#else
      z = 1. / z;
#endif
      star.x = x * z * (180. / PI);
      star.y = y * z * (180. / PI);
	  star.y = -star.y;
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadNames
// Class:	CStarCatalog
// Purpose:	load star names for catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CStarCatalog::LoadNames( const char* strFile )
{
	FILE* pFile = NULL;
	char strLine[255];
	char strMsg[255];
	// define my regex: event id = name, lat, ra, mag, spect  
	wxRegEx reLine( "^([0-9]+)\\|(.*)\n" );

	// debug
//	m_pLogger->Log( "INFO :: Loading configuration." );
	// Read the index
	pFile = fopen( strFile, "r" );
	// check if there is any configuration to load
	if( !pFile )
	{
		// debug
//		sprintf( strMsg, "Warning :: could not open config file %s", strFile );
//		m_pLogger->Log( strMsg );
		return( -1 );
	}
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
		bzero( strLine, 255 );
		// get one line out of the config file
		fgets( strLine, 2000, pFile );

		// check if event list
		if( reLine.Matches( strLine ) )
		{
			wxString strStarNo = reLine.GetMatch(strLine, 1 );
			wxString strStarName = reLine.GetMatch(strLine, 2 );
			long nStarNo = 0;
			if( strStarNo.ToLong( &nStarNo ) )
			{
				m_vectWord[m_nWord].sao_no = nStarNo;
				strcpy( m_vectWord[m_nWord].name, strStarName.GetData() );
				m_nWord++;
			}

		}
	}
	// close my file
	fclose( pFile );

	return( 1 );

}

/////////////////////////////////////////////////////////////////////
// Method:	Draw Sky
// Class:	CStarCatalog
// Purpose:	draw no of stars sorted by magnitude on an images
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CStarCatalog::DrawSky( long nStars )
{
	int i = 0;
	int nScaleFact = 10;
	char strTmp[255];

	long nWidth = m_nMaxX - m_nMinX;
	long nHeight = m_nMaxY - m_nMinY;

	int nWidthScale = (int) (nWidth/nScaleFact + 20);
	int nHeightScale = (int) (nHeight/nScaleFact + 20);

	IplImage* pImage = cvCreateImage (cvSize(nWidthScale, nHeightScale), IPL_DEPTH_8U, 3);

		CvFont* pFont = new CvFont();
		cvInitFont( pFont, CV_FONT_VECTOR0, 0.5, 0.5, 0, 1 );

		CvScalar col;
		col.val[0] = 255;
		col.val[1] = 0;
		col.val[2] = 0;

	for( i=0; i<nStars; i++ )
	{
			int nXAbs = (int) ((m_vectStar[i].x - m_nMinX)/nScaleFact);
			int nYAbs = (int) ((m_vectStar[i].y - m_nMinY)/nScaleFact);



		long nSao = m_vectStar[i].sao_no;

		if( nSao == 28553 || nSao == 15384 || nSao == 44752 || nSao == 28737 ||
			nSao == 27876 || nSao == 28179 || nSao == 28315 )
		{
			cvCircle( pImage, cvPoint( nXAbs, nYAbs ), 
					3, CV_RGB( 255,0,0 ), -1 );

			long starid = GetStarNameId( nSao );
	
			sprintf( strTmp, "%s", m_vectWord[starid].name );

			cvPutText( pImage, strTmp, cvPoint( nXAbs+10, nYAbs+10 ),
					pFont, col );
		}
	}

	cvSaveImage( "skymap.jpg", pImage );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetStarNameId
// Class:	CStarCatalog
// Purpose:	get star name id by sao no
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CStarCatalog::GetStarNameId( long nSaoNo )
{
	long i = 0;
	for( i=0; i<m_nWord; i++ )
	{
		if( m_vectWord[i].sao_no == nSaoNo ) return( i );
	}

}

/////////////////////////////////////////////////////////////////////
// Method:	sort stars  by magnitude
// Class:	CStarCatalog
// Purpose:	sort stars in order of magnitude
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CStarCatalog::SortByMagnitude( )
{
	long i = 0;
	int bOrder = 1;
	long nCount = 0;

	while( bOrder == 1 )
	{
		bOrder = 0;
		// for every star in my catalog
		for( i=0; i<m_nStar-1; i++ )
		{
			// if my star is bellow my reference magnitude
			if( m_vectStar[i].power > m_vectStar[i+1].power )
			{
				// backup next star
				long nStarNo = m_vectStar[i+1].sao_no;
				double nStarX = m_vectStar[i+1].x;
				double nStarY = m_vectStar[i+1].y;
				double nStarPower = m_vectStar[i+1].power;
				// move content from i to i+1
				m_vectStar[i+1].sao_no = m_vectStar[i].sao_no;
				m_vectStar[i+1].x = m_vectStar[i].x;
				m_vectStar[i+1].y = m_vectStar[i].y;
				m_vectStar[i+1].power = m_vectStar[i].power;
				// move backup of i+1 in i
				m_vectStar[i].sao_no = nStarNo;
				m_vectStar[i].x = nStarX;
				m_vectStar[i].y = nStarY;
				m_vectStar[i].power = nStarPower;
				// mark flag order
				bOrder = 1;
			}
		}
		nCount++;
	}
	m_nMaxMag = m_vectStar[m_nStar-1].power;
	m_nMinMag = m_vectStar[0].power;
	// return status ...
	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	cut window from catalog
// Class:	CStarCatalog
// Purpose:	cut a windows from catalog by coord
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CStarCatalog::CutWindowFromCatalog( double nX, double nY, double nWidth,
										double nHeight, StarDef* vectStar, int nStarCount )
{
	int i = 0;
	int nCount = 0;

	// for every star in my catalog
	for( i=0; i<m_nStar-1; i++ )
	{
		// check if start is in my window
		if( m_vectStar[i].x >= nX && m_vectStar[i].y >= nY &&
			m_vectStar[i].x <= nX+nWidth && m_vectStar[i].y <= nY+nHeight )
		{
			if( nCount == nStarCount )
			{
				break;
			} else
			{
				vectStar[nCount].sao_no = m_vectStar[i].sao_no;
				vectStar[nCount].x = m_vectStar[i].x - nX;
				vectStar[nCount].y = m_vectStar[i].y - nY;
				vectStar[nCount].power = m_vectStar[i].power;

				nCount++;
			}
		}
	}

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	connect stars 
// Class:	CStarCatalog
// Purpose:	connect star limited by magnitute power
// Input:	magnitude
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CStarCatalog::Connect( double nMag, long t=0 )
{
	long i = 0, j = 0;
	long nMagIdx = 0;

	// find index for my magnitude frame
	// for every star in my catalog
	for( i=0; i<m_nStar; i++ )
	{
		// if my star is bellow my reference magnitude
		if( m_vectStar[i].power > nMag )
		{
			nMagIdx = i;
			break;
		}
	}
	m_nMagLimitIdx = nMagIdx;

	// the trick - for debug
	nMagIdx = t;

	// for every star bellow my magnitude
	for( i=0; i<nMagIdx; i++ )
	{
		float min_dist = 99999999999;
		float min_distb = 99999999999;
		int min_id = -1;
		int min_idb = -1;
		float a,b,c;

		// find first closest - todo - limite frame for closest ?
		for( j=0; j<nMagIdx; j++ )
		{
			if( i != j )
			{
				
				float dx = m_vectStar[i].x-m_vectStar[j].x;
				float dy = m_vectStar[i].y-m_vectStar[j].y;
				float dist = sqrt( dx*dx + dy*dy );
				if( dist < min_dist )
				{
					min_dist = dist;
					min_id = j;
				}
				
				
			}
		}
		m_vectStar[i].point_b = min_id;
		m_vectStar[i].line_c = min_dist;
		c = min_dist;
		
		// find second closest
		for( j=0; j<nMagIdx; j++ )
		{
			if( i != j && j != min_id )
			{
				float dx = m_vectStar[i].x-m_vectStar[j].x;
				float dy = m_vectStar[i].y-m_vectStar[j].y;
				float dist = sqrt( dx*dx + dy*dy );
				if( dist < min_distb )
				{
					min_distb = dist;
					min_idb = j;
				}
			}
		}
		m_vectStar[i].point_c = min_idb;
		m_vectStar[i].line_b = min_distb;
		b = min_distb;
		// claculate line a
		float dx = m_vectStar[min_id].x-m_vectStar[min_idb].x;
		float dy = m_vectStar[min_id].y-m_vectStar[min_idb].y;
		float dist = sqrt( dx*dx + dy*dy );
		
		m_vectStar[i].line_a = dist;
		a = dist;
		
		m_vectStar[i].angle_a = acos( (b*b+c*c-a*a)/(2*b*c) );
		m_vectStar[i].angle_b = acos( (a*a+c*c-b*b)/(2*a*c) );
		m_vectStar[i].angle_c = acos( (a*a+b*b-c*c)/(2*a*b) );
	
		if( m_vectStar[i].sao_no == 28553 )
			Sleep( 10 );

		if( m_vectStar[i].angle_a < 0  ) m_vectStar[i].angle_a=PI;
		if( m_vectStar[i].angle_b < 0  ) m_vectStar[i].angle_b=PI;
		if( m_vectStar[i].angle_c < 0  ) m_vectStar[i].angle_c=PI;

	}

	return( 1 );

}

/////////////////////////////////////////////////////////////////////
// Method:	match stars 
// Class:	CStarCatalog
// Purpose:	connect star limited by magnitute power
// Input:	magnitude
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CStarCatalog::MatchStars( StarDef* vectStarB, int nStarCount )
{
	long e = 0, f = 0, t=0;
	long nCount = 0;
	double nDev = 0.9;

	for( t=15; t<500; t+=10 )
	{
		Connect( 0, t );

	// for every star in my photo
	for( e=0; e<15; e++ )
	{
		if( e == 10 )
			Sleep(10);
		// and for every star in my map
		for( f=0; f<t; f++ )
		{
			if( ( m_vectStar[f].angle_a <= vectStarB[e].angle_a+nDev && 
				  m_vectStar[f].angle_a >= vectStarB[e].angle_a-nDev ) && 
				( ( m_vectStar[f].angle_b <= vectStarB[e].angle_b+nDev &&
					m_vectStar[f].angle_b >= vectStarB[e].angle_b-nDev ) || 
				  ( m_vectStar[f].angle_b <= vectStarB[e].angle_c+nDev &&
					m_vectStar[f].angle_b >= vectStarB[e].angle_c-nDev ) ) )
			{
				int nB_ID = m_vectStar[f].point_b;
				int nRB_ID = 0;
				int nC_ID = m_vectStar[f].point_c;
				int nRC_ID = 0;
				// lines calc
				float line_a = m_vectStar[f].y-m_vectStar[m_vectStar[f].point_b].y;
				float line_b = m_vectStar[f].x-m_vectStar[m_vectStar[f].point_b].x;
				float line_ap = 0;
				float line_bp = 0;
				float b = m_vectStar[f].line_c;
				float c = 0;
				double diffB = 0;
				double diffC = 0;

				// if reverse
				if( m_vectStar[f].angle_b <= vectStarB[e].angle_b+nDev && 
					m_vectStar[f].angle_b >= vectStarB[e].angle_b-nDev )
				{
					nRB_ID = vectStarB[e].point_b;
					nRC_ID = vectStarB[e].point_c;
					line_ap = vectStarB[e].y-vectStarB[vectStarB[e].point_b].y;		
					line_bp = vectStarB[e].x-vectStarB[vectStarB[e].point_b].x;
					c = vectStarB[e].line_c;
					diffB = m_vectStar[f].line_b/vectStarB[e].line_b;
					diffC = m_vectStar[f].line_c/vectStarB[e].line_c;
				} else
				{
					nRB_ID = vectStarB[e].point_c;
					nRC_ID = vectStarB[e].point_b;
					line_ap = vectStarB[e].y-vectStarB[vectStarB[e].point_c].y;
					line_bp = vectStarB[e].x-vectStarB[vectStarB[e].point_c].x;
					c = vectStarB[e].line_b;
					diffB = m_vectStar[f].line_b/vectStarB[e].line_c;
					diffC = m_vectStar[f].line_c/vectStarB[e].line_b;
				}

				// For every angle B and C
				if( ( m_vectStar[nB_ID].angle_a <= vectStarB[nRB_ID].angle_a+nDev && 
					m_vectStar[nB_ID].angle_a >= vectStarB[nRB_ID].angle_a-nDev ) &&
					( m_vectStar[nC_ID].angle_a <= vectStarB[nRC_ID].angle_a+nDev &&
					m_vectStar[nC_ID].angle_a >= vectStarB[nRC_ID].angle_a-nDev ) &&
					( ( m_vectStar[nB_ID].angle_b <= vectStarB[nRB_ID].angle_b+nDev &&
					 m_vectStar[nB_ID].angle_b >= vectStarB[nRB_ID].angle_b-nDev ) ||
					 ( m_vectStar[nB_ID].angle_b <= vectStarB[nRB_ID].angle_c+nDev &&
					 m_vectStar[nB_ID].angle_b >= vectStarB[nRB_ID].angle_c-nDev ) ) &&
					( ( m_vectStar[nC_ID].angle_b <= vectStarB[nRC_ID].angle_b+nDev &&
					 m_vectStar[nC_ID].angle_b >= vectStarB[nRC_ID].angle_b-nDev ) ||
					 ( m_vectStar[nC_ID].angle_b <= vectStarB[nRC_ID].angle_c+nDev &&
					 m_vectStar[nC_ID].angle_b >= vectStarB[nRC_ID].angle_c-nDev ) )
					 )
				{
					int nWordId = GetStarNameId( m_vectStar[f].sao_no );
					if( m_vectStar[f].sao_no == 28553 )
						nCount++;
				}
			}
		}
	}

	// the end of various groups
	}

	return( nCount );
}


/////////////////////////////////////////////////////////////////////
// Method:	Draw Cut
// Class:	CStarCatalog
// Purpose:	draw no of stars sorted by magnitude on an images
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CStarCatalog::DrawCut( StarDef* vectStar, int nStarCount, long nWidth, long nHeight,
							   int icont, int jcont )
{
	int i = 0;
	int bFoundit = 0;
	char strTmp[255];
	char strFile[255];
	int nScaleFact = 10;

		CvFont* pFont = new CvFont();
		cvInitFont( pFont, CV_FONT_VECTOR0, 0.5, 0.5, 0, 1 );

		CvScalar col;
		col.val[0] = 255;
		col.val[1] = 0;
		col.val[2] = 0;

	int nWidthScale = (int) (nWidth/nScaleFact + 20);
	int nHeightScale = (int) (nHeight/nScaleFact + 20);

	IplImage* pImage = cvCreateImage( cvSize(nWidthScale, nHeightScale), IPL_DEPTH_8U, 3 );

	for( i=0; i<nStarCount; i++ )
	{
		int nXAbs = (int) (vectStar[i].x/nScaleFact);
		int nYAbs = (int) (vectStar[i].y/nScaleFact);

		long nSao = vectStar[i].sao_no;

		cvCircle( pImage, cvPoint( nXAbs, nYAbs ), 
					3, CV_RGB( 255,0,0 ), -1 );

		long starid = GetStarNameId( nSao );

		if( nSao == 28553 || nSao == 15384 || nSao == 44752 || nSao == 28737 ||
			nSao == 27876 || nSao == 28179 || nSao == 28315 )
		{
			bFoundit++;

			sprintf( strTmp, "%s", m_vectWord[starid].name );

			cvPutText( pImage, strTmp, cvPoint( nXAbs+10, nYAbs+10 ),
					pFont, col );
		}

	}

	if( bFoundit >=1 )
	{
		sprintf( strFile, "skycut_%d_%d.jpg", icont, jcont );
		cvSaveImage( strFile, pImage );
	}

	cvReleaseImage( &pImage );
}

/////////////////////////////////////////////////////////////////////
// Method:	Draw Sky
// Class:	CStarCatalog
// Purpose:	draw no of stars sorted by magnitude on an images
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CStarCatalog::DrawSkyRect( long nStars, long nX, long nY, long nWidth, long nHeight,
							   int icont, int jcont )
{
	int i = 0;
	int bFoundit = 0;
	int nScaleFact = 10;
	char strTmp[255];
	char strFile[255];

	long nWidthA = m_nMaxX - m_nMinX;
	long nHeightA = m_nMaxY - m_nMinY;

	int nWidthScale = (int) (nWidthA/nScaleFact + 20);
	int nHeightScale = (int) (nHeightA/nScaleFact + 20);

	IplImage* pImage = cvCreateImage (cvSize(nWidthScale, nHeightScale), IPL_DEPTH_8U, 3);

		CvFont* pFont = new CvFont();
		cvInitFont( pFont, CV_FONT_VECTOR0, 0.5, 0.5, 0, 1 );

		CvScalar col;
		col.val[0] = 255;
		col.val[1] = 0;
		col.val[2] = 0;

	for( i=0; i<nStars; i++ )
	{

		// check if start is in my window

		int nXAbs = (int) ((m_vectStar[i].x - m_nMinX)/nScaleFact);
		int nYAbs = (int) ((m_vectStar[i].y - m_nMinY)/nScaleFact);



		long nSao = m_vectStar[i].sao_no;

	//		if( nSao == 28553 || nSao == 15384 || nSao == 44752 || nSao == 28737 ||
	//			nSao == 27876 || nSao == 28179 || nSao == 28315 )
	//		{
		if( m_vectStar[i].x >= nX && m_vectStar[i].y >= nY &&
			m_vectStar[i].x <= nX+nWidth && m_vectStar[i].y <= nY+nHeight )
		{
				cvCircle( pImage, cvPoint( nXAbs, nYAbs ), 
						3, CV_RGB( 255,0,0 ), -1 );

				long starid = GetStarNameId( nSao );
			if( nSao == 28553 || nSao == 15384 || nSao == 44752 || nSao == 28737 ||
				nSao == 27876 || nSao == 28179 || nSao == 28315 )
			{
				bFoundit++;

				sprintf( strTmp, "%s", m_vectWord[starid].name );

				cvPutText( pImage, strTmp, cvPoint( nXAbs+10, nYAbs+10 ),
						pFont, col );
			}

		} else
		{

				cvCircle( pImage, cvPoint( nXAbs, nYAbs ), 
						3, CV_RGB( 0,255,0 ), -1 );

				long starid = GetStarNameId( nSao );
		
//				sprintf( strTmp, "%s", m_vectWord[starid].name );

//				cvPutText( pImage, strTmp, cvPoint( nXAbs+10, nYAbs+10 ),
//						pFont, col );

		}

	}

	int nOrigX = (int) ((nX-m_nMinX)/nScaleFact);
	int nOrigY = (int) ((nY-m_nMinY)/nScaleFact);
	int nEndX = (int) ((nX+nWidth-m_nMinX)/nScaleFact);
	int nEndY = (int) ((nY+nHeight-m_nMinY)/nScaleFact);

	cvRectangle( pImage, cvPoint( nOrigX, nOrigY), cvPoint( nEndX, nEndY ), 
					CV_RGB( 0,0,255 ), 1 );

	if( bFoundit >= 6 )
	{
		sprintf( strFile, "skymap_%d_%d.jpg", icont, jcont );
		cvSaveImage( strFile, pImage );
	}

	cvReleaseImage( &pImage );
}

////////////////////////////////////////////////////////////////////
// *****************************************************************
////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Method:	Draw Sky
// Class:	CStarCatalog
// Purpose:	draw no of stars sorted by magnitude on an images
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CStarCatalog::FindStars( const FOUND_STAR *found, const int n_stars,
           double *params, const double *search_loc, const int n0,
           const REF_STAR *gsc_stars, const int n_gsc_stars,
           const int photo_band)
{
   int n_source = 0, i, j, order;
   int best_n_within_tolerance = 0;
   int n_pairs = n0 * (n0 - 1), step0;
   long t2;
   double search_dist;
   double max_dist = 0.;
   double best_rms_sum = 0., best_xform[N_FIND_PARAMS];
   double max_allowed_residual = search_loc[4] / 3600.;
   STAR_PAIR *pairs, *pair_ptr;
   REF_STAR *istars, *end_star_ptr;
   extern FILE *log_file;
   extern FILE *overlay_file;

                  /* We want to have a copy of the GSC (or other catalog)  */
                  /* stars,  that can be sorted in x.  This speeds up the  */
                  /* process of finding out "what star is closest to (x,y)"*/
                  /* immensely (see the function find_closest( ) above to  */
                  /* see how this helps.)                                  */
   n_source = n_gsc_stars;
   istars = (REF_STAR *)calloc( n_source, sizeof( REF_STAR));
   if( !istars)
      return( -1);
                        /* Copy in the catalog stars... */
   memcpy( istars, gsc_stars, n_source * sizeof( REF_STAR));
   end_star_ptr = istars + n_source;

                       /* Convert 'em from RA/dec to "plate" coordinates */
                       /* (basically,  eta/xi)                           */
	for( i = 0; i < n_source; i++)
	{
		project_star( istars + i, search_loc[0], search_loc[1]);
		istars[i].dec = -istars[i].dec;   /* get around an axis flipping problem */
	}
                       /* Sort the stars in x: */
   qsort( istars, n_source, sizeof( REF_STAR), ref_star_compare);

                       /* Now make a list of all pairs of image stars that */
                       /* we're going to look at.  For each pair,  we want */
                       /* to know the distance between them.               */
                       /* We also keep track of the maximum distance found */
                       /* in this manner.  The way this speeds matters up  */
                       /* will be seen later.                              */

   pairs = pair_ptr = (STAR_PAIR *)calloc( n_pairs, sizeof( STAR_PAIR));
   for( i = 0; i < n0; i++)
      for( j = 0; j < n0; j++)
         if( i != j)
            {
            double dx = found[i].x - found[j].x;
            double dy = found[i].y - found[j].y;

            pair_ptr->start = i;
            pair_ptr->end = j;
            pair_ptr->dist = dx * dx + dy * dy;
            if( max_dist < pair_ptr->dist)
               max_dist = pair_ptr->dist;
            pair_ptr++;
            }

   max_dist = sqrt( max_dist);

               /* Now sort the list of pairs,  in order of distance: */
   qsort( pairs, n_pairs, sizeof( STAR_PAIR), pair_compare);

   t2 = time( NULL);
   search_dist = max_dist / 3600.;
               /* We'll be doing some binary searching in the list of */
               /* pairs,  so it helps to compute step0.  Step0 is the */
               /* largest power of 2 that is less than n_pairs.       */
   for( step0 = 1; step0 * 2 < n_pairs; step0 <<= 1)
      ;
               /* Now we look at every catalog star,  and try to match */
               /* it to an image star.                                 */
   for( i = 0; i < n_source; i++)
      {
      double ymin, ymax, xmax;
      REF_STAR *tstar = istars + i + 1;

      xmax  = istars[i].ra + search_dist;
      ymax  = istars[i].dec + search_dist;
      ymin  = istars[i].dec - search_dist;
      if( t2 != time( NULL))
         {
         printf( "%4.1lf%% complete\r",
                               (double)i * 100. / (double)n_source);
         t2 = time( NULL);
#ifdef __WATCOMC__
         if( kbhit( ))
            if( getch( ) == 27)
               exit( 0);
#endif
         }

            /* Now we look from the 'current' catalog star to the end    */
            /* of the list of catalog stars.  There are some quick tests */
            /* we can use to reduce the number of possible matches that  */
            /* we have to check,  using a bounding box.  Also,  we can   */
            /* make excellent use of the fact that the catalog stars are */
            /* sorted in order of x;  once we reach tstar->x < xmax,     */
            /* it's not necessary to go any farther.                     */

      for( ; tstar < end_star_ptr && tstar->ra < xmax; tstar++)
         if( tstar->dec > ymin && tstar->dec < ymax)
            {
            double dist, minimum, maximum, xform[N_FIND_PARAMS];
            int k = 0, m, step;
            double dx = tstar->ra  - istars[i].ra;
            double dy = tstar->dec - istars[i].dec;

            *xform = 1.;                  /* use a linear xformation only */
            dist = dx * dx + dy * dy;
            dist *= 3600. * 3600.;
                          /* Search_loc[3] contains the 'scale tolerance'  */
                          /* parameter.  The two catalog stars in question */
                          /* are separated by 'dist';  therefore,  we have */
                          /* to look hard at any pair of image stars that  */
                          /* are separated by 'minimum' to 'maximum' units. */
            minimum = dist / search_loc[3];
            maximum = dist * search_loc[3];
                          /* We can immediately skip over any image star     */
                          /* pairs whose separation is less than 'minimum'.. */
            for( step = step0; step; step >>= 1)
               if( (m = (k | step)) < n_pairs && minimum > pairs[m].dist)
                  k = m;
                          /* ...and,  we can stop considering image star   */
                          /* pairs when their separation is > 'maximum'.   */
                          /* This is why we went through all the trouble   */
                          /* to make a sorted list of image star pairs.    */
            for( k++; k < n_pairs && maximum > pairs[k].dist; k++)
               {
               int n_within_tolerance = 2;
               int check_it = 1;
               double rms_sum = 0.;

                            /* We construct a linear,  orthogonal transform */
                           /* between image space and catalog stars,  based */
                          /* on the assumption that the two image stars    */
                         /* happen to be the same as the two catalog stars. */
               calc_xform( xform, found + pairs[k].start,
                                  found + pairs[k].end,
                                  istars + i, tstar);
                      /* 'n_within_tolerance = 2' reflects the fact that     */
                      /* we know full well that at least those two will      */
                      /* get matched.  But how about the other n0-2          */
                      /* image stars?  Do they have catalog counterparts?    */
                      /* (Check first to be sure that we have a valid        */
                      /* transformation,  not tilted at too great an angle.  */
                    /* The tilt tolerance is in search_loc[5].  The expected */
                    /* tilt is in search_loc[6];  if it's zero,  we can skip */
                    /* an atan2( ) call,  a pleasant speedup.)               */
               if( search_loc[5])
                  if( !search_loc[6])        /* "standard" N/S tilt expected */
                     {
                     double axis_tilt = fabs( xform[7] / xform[6]);

                     if( axis_tilt > fabs( search_loc[5]))
                        check_it = 0;     /* too much tilt to the N/S axis */
//  Why did I        if( search_loc[5] > 0.)   /* check north_up only */
//  have these          if( xform[6] > 0.)
//  three lines??          check_it = 0;
                     }
                  else
                     {
                     double tilt = atan2( xform[7], xform[6]) - search_loc[6];

                     while( tilt > PI)
                        tilt -= PI + PI;
                     while( tilt < -PI)
                        tilt += PI + PI;
                     if( tilt > search_loc[5] || tilt < -search_loc[5])
                        check_it = 0;
                     }
               if( check_it)
                  for( m = 0; m < n0; m++)
                     if( m != pairs[k].start && m != pairs[k].end)
                        {
                        REF_STAR loc;
                        double d;
                        int closest;
                        double x = found[m].x, y = found[m].y;
                        const double *yform = xform + N_FIND_COEFFS;

                             /* Transform the image star to catalog space. */
                        loc.ra = xform[5] + xform[6]*x + xform[7] * y;
                        loc.dec = yform[5] + yform[6]*x + yform[7] * y;
             /* Look for the result in the list of catalog stars. */
                        d = find_closest( n_source, istars, &loc, &closest,
                                       max_allowed_residual * 5.);
             /* If the result happens to be within tolerances,  add it in. */
                        if( d < max_allowed_residual)
                           {
                           n_within_tolerance++;
                           rms_sum += d * d;
                           }
                        }
                           /* Next,  ask:  did this 'possible match' get */
                           /* more stars within tolerance than any   */
                           /* other we've tried,  or at least reduce our */
                           /* RMS residuals?  i.e.,  is it 'the best yet'? */
               if( n_within_tolerance > best_n_within_tolerance ||
                            (n_within_tolerance == best_n_within_tolerance &&
                                      rms_sum < best_rms_sum))
                  {
                           /* If we've got a winner,  save the xformation */
                           /* and similar data. */
                  best_n_within_tolerance = n_within_tolerance;
                  best_rms_sum = rms_sum;
                  memcpy( best_xform, xform, N_FIND_PARAMS * sizeof( double));
                  }
               }
            }
      }


               /* OK,  we can now assume we've got a good match.  Let's */
               /* go on to take our 'best case' and try to match some   */
               /* more stars to it.                                     */

   for( i = 3; i < N_FIND_COEFFS; i++)   /* zero out higher-order coeffs */
      best_xform[i + 5] = best_xform[i + 5 + N_FIND_COEFFS] = 0.;

                /* We'll start by improving our _linear_ fit.  If asked,  */
                /* we'll also improve the quad and/or cubic fit.          */
   for( order = 1; order <= (int)( params[0] + .5); order++)
      for( i = 0; i < N_PASSES; i++)
         {
         int n_coeffs = (order + 1) * (order + 2) / 2;
         void *lsquare_x = lsquare_init( n_coeffs);
         void *lsquare_y = lsquare_init( n_coeffs);
         double diff[N_FIND_COEFFS], sum_x2 = 0., sum_y2 = 0.;
         double magnitude_top = 0., magnitude_bottom = 0.;
         int n_within_tolerance = 0, err;

         *best_xform = (double)order;
                     /* We're now looking at _all_ image stars... not just */
                     /* the n0 brightest ones.                             */
         for( j = 0; j < n_stars; j++)
            {
            REF_STAR loc, tloc;
            double d, dx, dy, mag = 99.99;
            int closest;
            char tbuff[120];

                           /* Convert the image star to catalog space. */
            xform_point( best_xform, found + j, &loc);
            tloc = loc;
            tloc.dec = -tloc.dec;   /* deal with the silly y convention again */
            deproject_star( &tloc, search_loc[0], search_loc[1]);
            closest = 0;
                           /* As before,  find the nearest catalog star... */
            d = find_closest( n_source, istars, &loc, &closest,
                                          max_allowed_residual * 5.);
            dx = (double)( istars[closest].ra - loc.ra);
            dy = (double)( istars[closest].dec - loc.dec);
                      /* ..and if it's within tolerance,  add it to the */
                      /* least squares fit.  (This _is_ one difference: */
                      /* in the initial pattern matching,  all we had to */
                      /* do was create a transform matching stars A, B to */
                      /* catalog stars C, D,  getting a perfect match for */
                      /* them.  Now,  we have to do a full-fledged least  */
                      /* squares step.)                                   */
            if( d < max_allowed_residual)
               {
               double slopes[N_FIND_COEFFS];

               compute_powers( slopes, found[j].x, found[j].y, order);
               lsquare_add_observation( lsquare_x, dx, 1., slopes);
               lsquare_add_observation( lsquare_y, dy, 1., slopes);
                           /* We may also have to figure out the magnitude */
                           /* parameters,  as follows.                     */
                           /* We should only include those from the  */
                           /* desired band in the magnitude fit:     */
               if( !use_input_mags && found[j].bright > 0)
                  if( !istars[closest].photo_band || !photo_band ||
                               photo_band == istars[closest].photo_band)
                     {
                     double pseudo_mag, sqrt_counts;

                     mag = (double)istars[closest].mag / 100.;
                     sqrt_counts = sqrt( (double)found[j].bright);
                     pseudo_mag = -2.5 * log10( (double)found[j].bright);
                     magnitude_top += (mag - pseudo_mag) * sqrt_counts;
                     magnitude_bottom += sqrt_counts;
                     }
               n_within_tolerance++;
               }
            dx *= 3600.;
            dy *= 3600.;
            if( d < max_allowed_residual)
               {
               sum_x2 += dx * dx;
               sum_y2 += dy * dy;
               }
                     /* Tell the user what's going on. */
            if( d < max_allowed_residual * 3. && i == N_PASSES - 1)
               {                                /* only print on last pass */
               char ra_dec_str[100];
               extern int suppress_display_of_residuals;

               if( found[j].bright > 0)
                  {
                  double pseudo_mag = -2.5 * log10( (double)found[j].bright);

                  mag = best_xform[1] + pseudo_mag;
                  }
               else
                  mag = 0.;
               put_ra_dec_in_str( ra_dec_str, tloc.ra, tloc.dec, 2);
               sprintf( tbuff, "%2d: %4d%5d   %s %4d %5.2lf dist %6.3lf  ",
                  j, istars[closest].zone, istars[closest].number,
                  ra_dec_str, istars[closest].mag, mag, d * 3600.);
               sprintf( tbuff + strlen( tbuff), "dx%6.3lf dy%6.3lf", dx, dy);
               if( istars[closest].ppm_num > 0L && istars[closest].ppm_num < 1000000L)
                  sprintf( tbuff + strlen( tbuff), " PPM %ld\n",
                                               istars[closest].ppm_num);
               else
                  strcat( tbuff, "\n");
               if( !suppress_display_of_residuals)
                  log_printf( log_file, "%s", tbuff);
               }
            if( overlay_file && i == N_PASSES - 1 && found[j].bright)
               {
               if( use_input_mags)        /* for xy lists */
                  log_printf( overlay_file, "s 0%4ld\n", found[j].bright);
               else
                  log_printf( overlay_file, "s 0%4d\n", (int)( mag * 100. + .5));
               stuff_ang_into_buff( tbuff, -tloc.ra);
               stuff_ang_into_buff( tbuff + 3, tloc.dec);
               fwrite( tbuff, 6, 1, overlay_file);
               }
            }
         if( i == N_PASSES - 1)
            {
            best_n_within_tolerance = n_within_tolerance;
            log_printf( log_file, "%d %s\n", n_within_tolerance, strings[109]);
            log_printf( log_file, "rms dx = %.2lf   rms dy = %.2lf  rms err = %.2lf\n",
                                   sqrt( sum_x2 / (double)n_within_tolerance),
                                   sqrt( sum_y2 / (double)n_within_tolerance),
                       sqrt( (sum_x2 + sum_y2) / (double)n_within_tolerance));
            }

                  /* Now do the least squares math,  and adjust the xform. */
         err = lsquare_solve( lsquare_x, diff);
         if( !err)
            for( j = 0; j < n_coeffs; j++)
               best_xform[j + 5] += diff[j];

         err = lsquare_solve( lsquare_y, diff);
         if( !err)
            for( j = 0; j < n_coeffs; j++)
               best_xform[j + N_FIND_COEFFS + 5] += diff[j];

         if( !use_input_mags)
            best_xform[1] = magnitude_top / magnitude_bottom;

         lsquare_free( lsquare_x);
         lsquare_free( lsquare_y);
         }

   free( pairs);
   free( istars);
               /* Return the 'best xform' we've found. */
   memcpy( params, best_xform, N_FIND_PARAMS * sizeof( double));
   params[3] = search_loc[0];
   params[4] = search_loc[1];
   return( best_n_within_tolerance);
}
