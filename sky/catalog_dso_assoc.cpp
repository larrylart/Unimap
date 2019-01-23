
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
#include "../unimap_worker.h"

// main header
#include "catalog_dso_assoc.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogDsoAssoc
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogDsoAssoc::CSkyCatalogDsoAssoc( )
{
	///////////////
	// init catalog maps vectors
	m_vectMapPgcToMessier = NULL;
	m_nMapPgcToMessier = 0;
	m_vectMapPgcToNgc = NULL;
	m_nMapPgcToNgc = 0;
	m_vectMapPgcToIc = NULL;
	m_nMapPgcToIc = 0;
	m_vectMapMessierToNgcIc = NULL;
	m_nMapMessierToNgcIc = 0;

}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogDsoAssoc
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogDsoAssoc::~CSkyCatalogDsoAssoc( )
{
	if( m_vectMapMessierToNgcIc ) free( m_vectMapMessierToNgcIc );
	if( m_vectMapPgcToMessier ) free( m_vectMapPgcToMessier );
	if( m_vectMapPgcToNgc ) free( m_vectMapPgcToNgc );
	if( m_vectMapPgcToIc ) free( m_vectMapPgcToIc );

	m_vectMapPgcToIc = NULL;
	m_vectMapPgcToNgc = NULL;
	m_vectMapPgcToMessier = NULL;
	m_vectMapMessierToNgcIc = NULL;
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadPGCCatMaps
// Class:	CSkyCatalogDsoAssoc
// Purpose:	load PGC galaxy catalog maps to other cat ids
// Input:	file name with galaxy catalog
// Output:	status - 0 = error
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoAssoc::LoadMessierCatMaps( CUnimapWorker* pWorker )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxString strLog;
	// other variables used
	wxChar strMessNo[255];
	wxChar strType[255];
	wxChar strNgcIcNo[255];

	// if maps loaded free
	if( m_vectMapMessierToNgcIc ) free( m_vectMapMessierToNgcIc );
	// reset counters
	m_vectMapMessierToNgcIc = NULL;
	m_nMapMessierToNgcIc	= 0;

	// allocate messier to Ngc/Ic vector - default hardcoded 105
	m_vectMapMessierToNgcIc = (MapCatToCatSimpleWithDestType*) malloc( 120 * sizeof(MapCatToCatSimpleWithDestType) );

	//////////////////////////////////////////////
	// :: LOAD PGC TO MESSIER MAP FILE
	if( pWorker ) pWorker->Log( wxT("INFO :: Loading MESSIER to NGC/IC map ...") );
	// Read the index
	pFile = wxFopen( CATALOG_MAP_MESSIER_TO_NGCIC_FILENAME, wxT("r") );
	// check if there is any configuration to load
	if( !pFile )
	{
		// debug
		if( pWorker )
		{
			strLog.Printf( wxT("Warning :: could not open messier map file %s"), CATALOG_MAP_MESSIER_TO_NGCIC_FILENAME );
			pWorker->Log( strLog );
		}
		return( -1 );
	}
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );

		// now check if i might need to jump over - just use a random min string of 10
		if( strLine[0] != 'M' ) continue;
		if( wxStrlen( strLine ) < 6 ) continue;

		if( wxSscanf( strLine, wxT("M%[^ ] %[^ ] %[^\0]"), strMessNo, strType, strNgcIcNo ) )
		{
			// check destination
			if( wxStrcmp( strType, wxT("NGC") ) == 0 )
				m_vectMapMessierToNgcIc[m_nMapMessierToNgcIc].dest_cat_type = CAT_OBJECT_TYPE_NGC;
			else if( wxStrcmp( strType, wxT("IC") ) == 0 )
				m_vectMapMessierToNgcIc[m_nMapMessierToNgcIc].dest_cat_type = CAT_OBJECT_TYPE_IC;
			else 
				continue;

			// set source and destination - 
			m_vectMapMessierToNgcIc[m_nMapMessierToNgcIc].src_no = wxAtol( strMessNo );
			m_vectMapMessierToNgcIc[m_nMapMessierToNgcIc].dest_no = wxAtol( strNgcIcNo );

			// increase object counter
			m_nMapMessierToNgcIc++;
		}
	}
	// close my file
	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadPGCCatMaps
// Class:	CSkyCatalogDsoAssoc
// Purpose:	load PGC galaxy catalog maps to other cat ids
// Input:	file name with galaxy catalog
// Output:	status - 0 = error
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoAssoc::LoadPGCCatMaps( CUnimapWorker* pWorker )
{
	FILE* pFile = NULL;
	wxChar strLine[1000];
	wxString strLog;
//	wxString strWxLine = "";
	// other variables used
	unsigned long nSrcCatNo=0, nDestCatNo=0;

	// if maps loaded free
	if( m_vectMapPgcToMessier ) free( m_vectMapPgcToMessier );
	if( m_vectMapPgcToNgc ) free( m_vectMapPgcToNgc );
	if( m_vectMapPgcToIc ) free( m_vectMapPgcToIc );
	// reset counters
	m_vectMapPgcToMessier = NULL;
	m_vectMapPgcToNgc = NULL;
	m_vectMapPgcToIc = NULL;
	m_nMapPgcToMessier	= 0;
	m_nMapPgcToNgc = 0;
	m_nMapPgcToIc = 0;

	// allocate pgc to messier vector - default hardcoded 105
	m_vectMapPgcToMessier = (MapCatToCatSimple*) malloc( (105+1) * sizeof(MapCatToCatSimple) );
	// allocate pgc to ngc vector - default hardcoded 7680
	m_vectMapPgcToNgc = (MapCatToCatSimple*) malloc( (7680+1) * sizeof(MapCatToCatSimple) );
	// allocate pgc to ic vector - default hardcoded 3697
	m_vectMapPgcToIc = (MapCatToCatSimple*) malloc( (3697+1) * sizeof(MapCatToCatSimple) );

	//////////////////////////////////////////////
	// :: LOAD PGC TO MESSIER MAP FILE
	if( pWorker ) pWorker->Log( wxT("INFO :: Loading PGC to MESSIER map ...") );
	// Read the index
	pFile = wxFopen( CATALOG_MAP_PGC_TO_MESSIER_FILENAME, wxT("r") );
	// check if there is any configuration to load
	if( !pFile )
	{
		// debug
		if( pWorker )
		{
			strLog.Printf( wxT("Warning :: could not open pgc map file %s"), CATALOG_MAP_PGC_TO_MESSIER_FILENAME );
			pWorker->Log( strLog );
		}
		return( -1 );
	}
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );
		// set this to wx string
		wxString strWxLine = strLine;

		// now check if i might need to jump over - just use a random min string of 10
		if( wxStrlen( strLine ) < 6 ) continue;

		// get pgc number
		if( !strWxLine.Mid( 0, 7 ).Trim(0).Trim(1).ToULong( &nSrcCatNo ) ) nSrcCatNo = 0;
		// now get associeted messier number
		if( !strWxLine.Mid( 8, 4 ).Trim(0).Trim(1).ToULong( &nDestCatNo ) ) nDestCatNo = 0;

		// set source and destination
		m_vectMapPgcToMessier[m_nMapPgcToMessier].src_no = nSrcCatNo;
		m_vectMapPgcToMessier[m_nMapPgcToMessier].dest_no = nDestCatNo;

		// increase object counter
		m_nMapPgcToMessier++;

	}
	// close my file
	fclose( pFile );

	//////////////////////////////////////////////
	// :: LOAD PGC TO NGC MAP FILE
	if( pWorker ) pWorker->Log( wxT("INFO :: Loading PGC to NGC map ...") );
	// Read the index
	pFile = wxFopen( CATALOG_MAP_PGC_TO_NGC_FILENAME, wxT("r") );
	// check if there is any configuration to load
	if( !pFile )
	{
		// debug
		if( pWorker )
		{
			strLog.Printf( wxT("Warning :: could not open pgc map file %s"), CATALOG_MAP_PGC_TO_NGC_FILENAME );
			pWorker->Log( strLog );
		}
		return( -1 );
	}
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );
		// set this to wx string
		wxString strWxLine = strLine;

		// now check if i might need to jump over - just use a random min string of 10
		if( wxStrlen( strLine ) < 6 ) continue;

		// get pgc number
		if( !strWxLine.Mid( 0, 7 ).Trim(0).Trim(1).ToULong( &nSrcCatNo ) ) nSrcCatNo = 0;
		// now get associeted ngc number
		if( !strWxLine.Mid( 8, 4 ).Trim(0).Trim(1).ToULong( &nDestCatNo ) ) nDestCatNo = 0;

		// set source and destination
		m_vectMapPgcToNgc[m_nMapPgcToNgc].src_no = nSrcCatNo;
		m_vectMapPgcToNgc[m_nMapPgcToNgc].dest_no = nDestCatNo;

		// increase object counter
		m_nMapPgcToNgc++;

	}
	// close my file
	fclose( pFile );

	//////////////////////////////////////////////
	// :: LOAD PGC TO IC MAP FILE
	if( pWorker ) pWorker->Log( wxT("INFO :: Loading PGC to IC map ...") );
	// Read the index
	pFile = wxFopen( CATALOG_MAP_PGC_TO_IC_FILENAME, wxT("r") );
	// check if there is any configuration to load
	if( !pFile )
	{
		// debug
		if( pWorker )
		{
			strLog.Printf( wxT("Warning :: could not open pgc map file %s"), CATALOG_MAP_PGC_TO_IC_FILENAME );
			pWorker->Log( strLog );
		}
		return( -1 );
	}
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );
		// set this to wx string
		wxString strWxLine = strLine;

		// now check if i might need to jump over - just use a random min string of 10
		if( wxStrlen( strLine ) < 6 ) continue;

		// get pgc number
		if( !strWxLine.Mid( 0, 7 ).Trim(0).Trim(1).ToULong( &nSrcCatNo ) ) nSrcCatNo = 0;
		// now get associeted messier number
		if( !strWxLine.Mid( 8, 4 ).Trim(0).Trim(1).ToULong( &nDestCatNo ) ) nDestCatNo = 0;

		// set source and destination
		m_vectMapPgcToIc[m_nMapPgcToIc].src_no = nSrcCatNo;
		m_vectMapPgcToIc[m_nMapPgcToIc].dest_no = nDestCatNo;

		// increase object counter
		m_nMapPgcToIc++;

	}
	// close my file
	fclose( pFile );

	return( 1 );

}

/////////////////////////////////////////////////////////////////////
// Method:	IsPgcObjectMessier
// Class:	CSkyCatalogDsoAssoc
// Purpose:	check if a PGC object is a messier
// Input:	pgc catalog number
// Output:	messier cat number or 0 if not
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoAssoc::IsPgcObjectMessier( unsigned long nCatNo )
{
	unsigned int i=0;

	for( i=0; i<m_nMapPgcToMessier; i++ )
	{	
		if( m_vectMapPgcToMessier[i].src_no == nCatNo ) return( m_vectMapPgcToMessier[i].dest_no );
	}

	return( 0 );
}

/////////////////////////////////////////////////////////////////////
// Method:	IsPgcObjectNgc
// Class:	CSkyCatalogDsoAssoc
// Purpose:	check if a PGC object is an Ngc
// Input:	pgc catalog number
// Output:	ngc cat number or 0 if not
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoAssoc::IsPgcObjectNgc( unsigned long nCatNo )
{
	unsigned int i=0;

	for( i=0; i<m_nMapPgcToNgc; i++ )
	{	
		if( m_vectMapPgcToNgc[i].src_no == nCatNo ) return( m_vectMapPgcToNgc[i].dest_no );
	}

	return( 0 );
}

/////////////////////////////////////////////////////////////////////
// Method:	IsPgcObjectIc
// Class:	CSkyCatalogDsoAssoc
// Purpose:	check if a PGC object is an Ic
// Input:	pgc catalog number
// Output:	ic cat number or 0 if not
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoAssoc::IsPgcObjectIc( unsigned long nCatNo )
{
	unsigned int i=0;

	for( i=0; i<m_nMapPgcToIc; i++ )
	{	
		if( m_vectMapPgcToIc[i].src_no == nCatNo ) return( m_vectMapPgcToIc[i].dest_no );
	}

	return( 0 );
}

/////////////////////////////////////////////////////////////////////
// Method:	IsNgcIcObjectMessier
// Class:	CSkyCatalogDsoAssoc
// Purpose:	check if an Ngc or Ic object is a messier
// Input:	ngc/ic catalog number, ngc or ic = 0/1
// Output:	messier cat number or 0 if not
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoAssoc::IsNgcIcObjectMessier( unsigned long nCatNo, unsigned char nCatType )
{
	unsigned int i=0;

	for( i=0; i<m_nMapMessierToNgcIc; i++ )
	{	
		if( m_vectMapMessierToNgcIc[i].dest_cat_type == nCatType && 
			m_vectMapMessierToNgcIc[i].dest_no == nCatNo ) 
		{
			return( m_vectMapMessierToNgcIc[i].src_no );
		}
	}

	return( -1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	IsMessierObjectNgcIc
// Class:	CSkyCatalogDsoAssoc
// Purpose:	check if a messier object is an Ngc or Ic object
// Input:	messier catalog number, ngc or ic = 0/1
// Output:	ngc/ic cat number or 0 if not
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoAssoc::IsMessierObjectNgcIc( unsigned long nCatNo )
{
	unsigned int i=0;

	for( i=0; i<m_nMapMessierToNgcIc; i++ )
	{	
		if( m_vectMapMessierToNgcIc[i].src_no == nCatNo ) return( m_vectMapMessierToNgcIc[i].dest_no );
	}

	return( 0 );
}

// GET RID OF THIS - USE THE ONE ABOVE
/////////////////////////////////////////////////////////////////////
// Method:	GetMessierNgcNo
// Class:	CSkyCatalogDsoAssoc
// Purpose:	get ngc number for messier objects
// Input:	nothing
// Output:	ngc number for messier ?
/////////////////////////////////////////////////////////////////////
/*unsigned long CSkyCatalogDsoAssoc::GetMessierNgcNo( unsigned long nCatNo )
{
	unsigned long i = 0;
	unsigned long nNgcCatNo = 0;

	for( i=0; i<m_nNgc; i++ )
	{
		if( m_vectNgc[i].messier_no == nCatNo )
		{
			nNgcCatNo = m_vectNgc[i].cat_no;
			break;
		}
	}

	return( nNgcCatNo );
}*/


