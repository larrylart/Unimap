

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// math libs
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include <functional>
#include <numeric>

// wx
#include "wx/wxprec.h"
#include "wx/thread.h"
//precompiled headers
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
// wx :: other 
#include <wx/regex.h>
#include <wx/string.h>
#include <wx/tokenzr.h>

// local headers
#include "../util/folders.h"
#include "../unimap_worker.h"
#include "../util/func.h"
#include "sky.h"

// main header
#include "catalog_dso_ngc.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogDsoNgc
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogDsoNgc::CSkyCatalogDsoNgc( CSky* pSky ) : CSkyCatalogDso( pSky )
{
	m_vectNgc = NULL;
	m_nNgc = 0;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogDsoNgc
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogDsoNgc::~CSkyCatalogDsoNgc()
{
//	UnLoadNgcRev( );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadNgcRev
// Class:	CSkyCatalogDsoNgc
// Purpose:	load ngc revised catalog
// Input:	file name with galaxy catalog
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyCatalogDsoNgc::LoadNgcRev( const wxString& strFile )
{
	// if catalog loaded return
	if( m_vectNgc )
	{
		return( 0 );
	}

	FILE* pFile = NULL;
	wxChar strLine[1000];
	wxString strLog;
	double nTimeRef=0;
//	wxString strWxLine = wxT("");
	// define my regex for ra and dec
	wxRegEx reRa( wxT("^\ *([0-9\.]{2,})\ ([0-9\.]{2,})\ ([0-9\.]{2,})\ *") );
	wxRegEx reDec( wxT("^\ *([\ \+\-]{1})([0-9\.]{2,})\ ([0-9\.]{2,})\ ([0-9\.]{2,})\ *") );
	// messier regex
	wxRegEx reMessier( wxT("^\ *M\ ([0-9]+)\ *") );
	// other variables used
	unsigned long nCatNo=0, nMessierNo=0;
	double nRaDeg=0, nDecDeg=0, nMag=0, nHH=0, nMM=0, nA=0, nB=0, nPa=0, nSS=0;

	// start from 0
	m_nNgc = 0;
//	m_nIc = 0;

	// allocate vector
	m_vectNgc = (DsoDefCatNgcIc*) malloc( (9200+1) * sizeof(DsoDefCatNgcIc) );

	// info
//	m_pUnimapWorker->Log( "INFO :: Loading NGC catalog ..." );
	nTimeRef = GetDTime();
	// Read the index
	pFile = wxFopen( strFile, wxT("r") );
	// check if there is any configuration to load
	if( !pFile )
	{
		// debug
//		sprintf( strMsg, "Warning :: could not open config file %s", strFile );
//		m_pUnimapWorker->Log( strMsg );
		return( -1 );
	}
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );
		// set this to wx string
		wxString strWxLine = strLine;

		// now check if i might need to jump over
		if( strLine[7] != '*' ) continue;
		unsigned long nJump = 0;
		if( !strWxLine.Mid( 9, 1 ).ToULong( &nJump ) ) nJump = 0;
		if( nJump > 1 ) continue;

		// extract fields
		if( !strWxLine.Mid( 0, 4 ).ToULong( &nCatNo ) )
			nCatNo = 0;
		wxString strRa = strWxLine.Mid( 19, 11 );
		wxString strDec = strWxLine.Mid( 31, 9 );
		if( !strWxLine.Mid( 48, 6 ).ToDouble( &nMag ) ) nMag = 0;
		if( !strWxLine.Mid( 61, 6 ).ToDouble( &nA ) ) nA = 0;
		if( !strWxLine.Mid( 68, 5 ).ToDouble( &nB ) ) nB = 0;
		if( !strWxLine.Mid( 74, 3 ).ToDouble( &nPa ) ) nPa = 0;
		wxString strId = strWxLine.Mid( 96, 15 );
		///////////////////////////
		// now convert RA to deg
		if( reRa.Matches( strRa ) )
		{
			if( !reRa.GetMatch(strRa, 1 ).ToDouble( &nHH ) ) nHH = 0;
			if( !reRa.GetMatch(strRa, 2 ).ToDouble( &nMM ) ) nMM = 0;
			if( !reRa.GetMatch(strRa, 3 ).ToDouble( &nSS ) ) nSS = 0;
			// calculate value in degrees
			nRaDeg = (double) ( (double) nHH*15 + (double) nMM/4 + (double) nSS/240);

		} else
		{
			nRaDeg = 0;
		}
		//////////////////////////////
		// and convert DEC to deg
		if( reDec.Matches( strDec ) )
		{
			int nSign = 1;
			// get sign if any
			if( reDec.GetMatch(strDec, 1 ).Cmp( wxT("-") ) == 0 )
				nSign = -1;
			// get hours, minutes and seconds
			if( !reDec.GetMatch(strDec, 2 ).ToDouble( &nHH ) ) nHH = 0;
			if( !reDec.GetMatch(strDec, 3 ).ToDouble( &nMM ) ) nMM = 0;
			if( !reDec.GetMatch(strDec, 4 ).ToDouble( &nSS ) ) nSS = 0;
			// calculate value in degrees
			nDecDeg = (double) (( double) (nSign*( (double) nHH + (double) nMM/60 + (double) nSS/3600) ) );

		} else
		{
			nDecDeg = 0;
		}
		////////////////////////////
		// and check if messier id given
		if( reMessier.Matches( strId ) )
		{
			if( !reMessier.GetMatch(strId, 1).ToULong( &nMessierNo ) ) nMessierNo = 0;
		} else
		{
			nMessierNo = 0;
		}

		// here we set the cat - nice and clean even a dupe
		m_vectNgc[m_nNgc].cat_no = nCatNo;
		m_vectNgc[m_nNgc].ra = nRaDeg;
		m_vectNgc[m_nNgc].dec = nDecDeg;
		m_vectNgc[m_nNgc].mag = nMag;
		m_vectNgc[m_nNgc].a = nA;
		m_vectNgc[m_nNgc].b = nB;
		m_vectNgc[m_nNgc].pa = nPa;
		m_vectNgc[m_nNgc].messier_no = (unsigned char) nMessierNo;

		// increase star counter
		m_nNgc++;

	}
	// close my file
	fclose( pFile );

	// debug
	nTimeRef = GetDTime()-nTimeRef;
	strLog.Printf( wxT("DEBUG :: ngc catalog was loaded in %lu.%lu"),
					(unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );
//	m_pUnimapWorker->Log( strLog );

	return( 1 );

}

/////////////////////////////////////////////////////////////////////
// Method:	GetNgcRaDec
// Class:	CSkyCatalogDsoNgc
// Purpose:	get ngc ra/dec by cat no
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoNgc::GetNgcRaDec( unsigned long nCatNo, double& nRa,
														double& nDec )
{
	int i =0;

	// if catalog not loaded load
	if( !m_vectDso )
	{
		LoadNgcBinary( );
	}
	// look in the entire catalog for my cat no
	for( i=0; i<m_nDso; i++ )
	{
		if( m_vectDso[i].cat_no == nCatNo )
		{
			nRa = m_vectDso[i].ra;
			nDec = m_vectDso[i].dec;
		}
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	UnLoadNgcRev
// Class:	CSkyCatalogDsoNgc
// Purpose:	unload ngc revised catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyCatalogDsoNgc::UnLoadNgcRev( )
{
	if( m_vectNgc ) free( m_vectNgc );
	m_vectNgc = NULL;
	m_nNgc = 0;

	return;
}
/////////////////////////////////////////////////////////////////////
// Method:	LoadIcBinary
// Class:	CSkyCatalogDsoNgc
// Purpose:	load the binary version of the ngc/ic catalog
// Input:	type
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogDsoNgc::LoadNgcBinary( )
{
	FILE* pFile = NULL;
	double nTimeRef=0;
	wxString strLog;
//	char strFile[255];
	unsigned char nTempNo=0;

	if( m_vectDso != NULL ) free( m_vectDso );
	m_vectDso = NULL;
	m_nDso = 0;

	//strcpy( strFile, DEFAULT_NGC_BINARY_FILENAME );
	// info
	m_pUnimapWorker->Log( wxT("INFO :: Loading Ngc catalog ...") );

	nTimeRef = GetDTime();

	// open file to write
	pFile = wxFopen( DEFAULT_NGC_BINARY_FILENAME, wxT("rb") );
	if( !pFile )
	{
		return( 0 );
	}

	// now go at the end of the file and see the size
	fseek( pFile, 0, SEEK_END );
	// get the file size and calculate how many elements i have
	int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+6*sizeof(double)+
													sizeof(unsigned char) ));
	// allocate my vector + 1 unit just to be sure
	m_vectDso = (DsoDefCatBasic*) malloc( (nRecords+1)*sizeof(DsoDefCatBasic) );
	m_nDsoAllocated = nRecords+1;

	// go at the begining of the file
	fseek( pFile, 0, SEEK_SET );

	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		ResetObjCatDso( m_vectDso[m_nDso] );

		if( !fread( (void*) &(m_vectDso[m_nDso].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectDso[m_nDso].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectDso[m_nDso].dec), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectDso[m_nDso].mag), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectDso[m_nDso].a), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectDso[m_nDso].b), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectDso[m_nDso].pa), sizeof(double), 1, pFile ) )
			break;
		// todo: here maybe remove this - since I have a map - start down with export method
//		if( !fread( (void*) &(vectNgcIc[nNgcIc].messier_no), sizeof(unsigned char), 1, pFile ) )
//			break;
		// hack - read to temp blind var
		if( !fread( (void*) &(nTempNo), sizeof(unsigned char), 1, pFile ) )
			break;

		// increment counter
		m_nDso++;
	}

	fclose( pFile );

	// debug
	nTimeRef = GetDTime()-nTimeRef;

	//debug
	strLog.Printf( wxT("DEBUG :: IC catalog was loaded in %lu.%lu seconds"),
				(unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );

	m_pUnimapWorker->Log( strLog );

	return( m_nDso );
}
