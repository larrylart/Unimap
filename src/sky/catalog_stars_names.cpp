////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

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

// local headers
#include "../unimap_worker.h"
#include "../util/func.h"
#include "../config/mainconfig.h"
#include "sky.h"

// main header
#include "catalog_stars_names.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogStarNames
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogStarNames::CSkyCatalogStarNames( CSky* pSky )
{
	m_pSky = pSky;
	m_pUnimapWorker =  NULL;

	m_vectStarName = NULL;
	m_nStarName= 0;

	////////////////////////////
	// allocate mempory for my star names
//	m_vectWord.Alloc( 7000 );
//	m_vectWord.Empty( );
	// fill my string array with nothing
	m_nWord = 0;
	for( int i=0; i<8000; i++ )
	{
		m_vectWord[i].cat_type = CATALOG_ID_NONE;
		memset( m_vectWord[i].name, 0, 100*sizeof(wxChar) );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogStarNames
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogStarNames::~CSkyCatalogStarNames()
{
	// also clean the names
	UnLoadStarNames( );

//	m_vectWord.Clear();
//	free( m_vectData );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetNameRaDec
// Class:	CSkyCatalogStarNames
// Purpose:	get dso name from common names coord or related catalogs
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogStarNames::GetNameRaDec( unsigned long nId, double& nRa,
														double& nDec )
{
	if( !m_vectStarName || nId < 0 || nId > m_nStarName-1 ) return( 0 );

	// todo :: here same as in dso names - check by type if loaded main catalog
	nRa = m_vectStarName[nId].ra;
	nDec = m_vectStarName[nId].dec;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadStarNames
// Class:	CSkyCatalogStarNames
// Purpose:	load stars common names
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyCatalogStarNames::LoadStarNames( CUnimapWorker* pWorker )
{
	FILE* pFile = NULL;
	wxChar strLine[500];
	wxChar strStarName[500];
	wxString strMsg;
	unsigned long nSao=0;
	double nDecDeg=0.0, nRaDeg=0.0;

	// first check if one already is and close
	UnLoadStarNames( );
	// allocate my vector
	m_vectStarName = (StarCommonName*) malloc( 300*sizeof( StarCommonName ) );

	// debug
	if( pWorker ) pWorker->Log( wxT("INFO :: Loading star names ...") );

	// Read the index
	pFile = wxFopen( STARS_COMMON_NAME_FILE, wxT("r") );
	// check if there is any configuration to load
	if( !pFile )
	{
		// debug
		if( pWorker )
		{
			strMsg.Printf( wxT("Warning :: could not open config file %s"), STARS_COMMON_NAME_FILE );
			pWorker->Log( strMsg );
		}
		return( -1 );
	}
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
		memset( &strLine, 0, 500*sizeof(wxChar) );
		memset( &strStarName, 0, 500*sizeof(wxChar) );

		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );

		// if line empty jump
		if( wxStrlen( strLine ) < 3 ) continue;
		// read line for ra/dec and sao number (sao not used at the moment)
		if( wxSscanf( strLine, wxT("%[^=]=%lf,%lf,%lu\n"), strStarName, &nRaDeg, &nDecDeg, &nSao ) )
		{
			// set ra/dec
			m_vectStarName[m_nStarName].ra = nRaDeg;
			m_vectStarName[m_nStarName].dec = nDecDeg;

			// set star name
			int nAllocSize = wxStrlen(strStarName)+1;
			m_vectStarName[m_nStarName].name = (wxChar*) malloc( nAllocSize*sizeof(wxChar) );
			memset( m_vectStarName[m_nStarName].name, 0, nAllocSize*sizeof(wxChar) );

			wxStrcpy( m_vectStarName[m_nStarName].name, strStarName );

			// increase dso common name counter
			m_nStarName++;
		}
	}
	// close my file
	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	UnLoadStarNames
// Class:	CSkyCatalogStarNames
// Purpose:	unload star common names
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyCatalogStarNames::UnLoadStarNames( )
{
	int i = 0;

	if( m_vectStarName )
	{
		for( i=0; i<m_nStarName; i++ )
			free( m_vectStarName[i].name );

		free( m_vectStarName );
	}

	return;
}


/////////////////////////////////////////////////////////////////////
// Method:	GetStarNameId
// Class:	CSkyCatalogStarNames
// Purpose:	get star name id by sao no
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogStarNames::GetStarNameId( StarDef& pStar, unsigned long& nId )
{
	unsigned long i = 0;
	for( i=0; i<m_nWord; i++ )
	{
		if( m_vectWord[i].cat_type == pStar.cat_type &&  
			(pStar.cat_type == CAT_OBJECT_TYPE_SAO && m_vectWord[i].cat_no == pStar.cat_no) || 
			(pStar.cat_type == CAT_OBJECT_TYPE_TYCHO && 
				m_vectWord[i].zone_no == pStar.zone_no && 
				m_vectWord[i].cat_no == pStar.cat_no && 
				m_vectWord[i].comp_no == pStar.comp_no )  )
		{
			nId = i;
			return( 1 );
		}
	}

	return( 0 );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetStarNameByCatNo
// Class:	CSkyCatalogStarNames
// Purpose:	get star name by catalog number
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogStarNames::GetStarNameByCatNo( StarDef& pStar, wxString& strName )
{
	unsigned long nId = 0;
	if( GetStarNameId( pStar, nId ) )
	{
		strName = m_vectWord[nId].name;
		return( 1 );

	} else
	{
		strName = wxT("");
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	GetStarSimpleCatName
// Class:	CSkyCatalogStarNames
// Purpose:	Get a star simple name CATANAME+CATID
// Input:	star, reference to string
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogStarNames::GetStarSimpleCatName( StarDef& pStar, wxString& strStarName, int bMatch )
{
	// empty string
	strStarName = wxT("");
	// return if no match
	if( !pStar.match && bMatch ) return( 0 );

	unsigned long nCatNo = pStar.cat_no;

	// now check by object type
	if( pStar.cat_type == CAT_OBJECT_TYPE_SAO )
	{
		strStarName.Printf( wxT("SAO%d"), pStar.cat_no );

	} else if( pStar.cat_type == CAT_OBJECT_TYPE_USNO )
	{
		strStarName.Printf( wxT("USNOB%.4d-%.7d"),
										pStar.zone_no,
										pStar.cat_no );

	} else if( pStar.cat_type == CAT_OBJECT_TYPE_NOMAD )
	{
		strStarName.Printf( wxT("NOMAD%.4d-%.7d"),
										pStar.zone_no,
										pStar.cat_no );

	} else if( pStar.cat_type == CAT_OBJECT_TYPE_TYCHO )
	{
		strStarName.Printf( wxT("TYC%.4d-%.5d-%.1d"), 
										pStar.zone_no,
										pStar.cat_no,
										pStar.comp_no );

	} else if( pStar.cat_type == CAT_OBJECT_TYPE_HIPPARCOS )
	{
		strStarName.Printf( wxT("HIP%d"), pStar.cat_no );

	} else if( pStar.cat_type == CAT_OBJECT_TYPE_2MASS )
	{
		strStarName.Printf( wxT("2MASS %hs"), pStar.cat_name );

	} else if( pStar.cat_type == CAT_OBJECT_TYPE_GSC )
	{
		strStarName.Printf( wxT("GSC %hs"), pStar.cat_name );

	} else
	{
		// no cat to match return 0
		return( 0 );
	}


	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetStarName
// Class:	CSkyCatalogStarNames
// Purpose:	Get a star name based on the catalog etc
// Input:	star id ?
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogStarNames::GetStarName( int nStarId, StarDef& pStar, wxString& strStarName )
{
	int k = 0;
	wxString strName;
	wxString strTmp;
	// vector of lavbel fields used to construct the label
	wxString vectLabelField[6];
	int nLabelField=0;

	// if the star was matched
	if( pStar.match )
	{
		unsigned long nCatNo = pStar.cat_no;

		// check if id to be included
		if( m_pConfig->m_bMatchObjLabelUseId ) 
			vectLabelField[nLabelField++].Printf( wxT("%d"), nStarId );

//		unsigned long nId = 0;
		// now check if to include catalog number
		if( m_pConfig->m_bMatchObjLabelUseCatNo )
		{
			GetStarSimpleCatName( pStar, vectLabelField[nLabelField++] );
		}

		// use common name - hack - will work only for sao
		// todo: to implement latter for other catalogs based on catalog assoc 
		if( m_pConfig->m_bMatchObjLabelUseCName  && 
			( pStar.cat_type == CAT_OBJECT_TYPE_SAO  || pStar.cat_type == CAT_OBJECT_TYPE_TYCHO ) && 
			m_pSky->m_pCatalogStarNames->GetStarNameByCatNo( pStar, strTmp ) )
		{
			vectLabelField[nLabelField++] = strTmp;
		}

	// if star wasn't matched
	} else
	{
		// check if id to be included for detected only
		if( m_pConfig->m_bDetectObjLabelUseId )
			vectLabelField[nLabelField++].Printf( wxT("%d"), nStarId );
		// check if magnitude to be included for detected only
		if( m_pConfig->m_bDetectObjLabelUseMag )
			vectLabelField[nLabelField++].Printf( wxT("%d"), pStar.mag );

	}

	////////////////////////////
	strName = wxT("");
	// stars :: now from all fields build name
	for( k=0; k<nLabelField; k ++ )
	{
		if( k == 0 ) 
			strName = vectLabelField[k];
		else
			strName += wxT(" - ") + vectLabelField[k];
	}

	// free  and allocate my string
	//if( strStarName ) free( m_vectStar[i].name );

	if( nLabelField )
	{
		//strStarName = (char*) malloc( (strlen(strName)+1)*sizeof(char) );
		strStarName = strName;

	} else
	{
		//strStarName = wxT("");
		return( 0 );
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetCatStarName
// Class:	CSkyCatalogStarNames
// Purpose:	Get a star name based on the catalog etc
// Input:	star id ?
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogStarNames::GetCatStarName( int nStarId, StarDef& pStar, wxString& strStarName )
{
	if( nStarId < 0  ) return( 0 );

	int k = 0, ret = 0;
	wxString strName;
	wxString strTmp;
	// vector of lavbel fields used to construct the label
	wxString vectLabelField[6];
	int nLabelField=0;

	unsigned long nCatNo = pStar.cat_no;

	// check if id to be included
	if( m_pConfig->m_bCatStarLabelUseId )
		vectLabelField[nLabelField++].Printf( wxT("%d"), nStarId );

//		unsigned long nId = 0;
	// now check if to include catalog number
	if( m_pConfig->m_bCatStarLabelUseCatNo )
	{
		ret = GetStarSimpleCatName( pStar, vectLabelField[nLabelField++], 0 );
	}

	/////////////////
	// use common name
	if( m_pConfig->m_bCatStarLabelUseCName && 
		( pStar.cat_type == CAT_OBJECT_TYPE_SAO || pStar.cat_type == CAT_OBJECT_TYPE_TYCHO ) && 
		m_pSky->m_pCatalogStarNames->GetStarNameByCatNo( pStar, strTmp ) )
	{
		vectLabelField[nLabelField++] = strTmp;
		ret = 2;
	}


	////////////////////////////
	strName = wxT("");
	// stars :: now from all fields build name
	for( k=0; k<nLabelField; k ++ )
	{
		if( k == 0 ) 
			strName = vectLabelField[k];
		else
			strName += wxT(" - ") + vectLabelField[k];
	}

	// free  and allocate my string
	//if( strStarName ) free( m_vectStar[i].name );

	if( nLabelField )
	{
		//strStarName = (char*) malloc( (strlen(strName)+1)*sizeof(char) );
		strStarName = strName;

	} else
	{
		//strStarName = wxT("");
		return( 0 );
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadStripedSaoNames
// Class:	CSkyCatalogStarNames
// Purpose:	load star names map for sao catalog
// Input:	reference to worker thread
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyCatalogStarNames::LoadStripedSaoNames( CUnimapWorker* pWorker )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
//	wxString strMsg;
	// define my regex: event id = name, lat, ra, mag, spect
	wxRegEx reLine( wxT("^([0-9]+)\\|(.*)\n") );

	// debug
	if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT( "INFO :: Loading SAO names ...") );

	// Read the index
	pFile = wxFopen( STARS_SAO_NAME_FILE, wxT("r") );
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
		memset( &strLine, 0, 1000*sizeof(wxChar) );
//		bzero( strLine, 255 );
		// get one line out of the config file
		wxFgets( strLine, 2000, pFile );
		//wxString strWxLine = strLine;

		// check if event list
		if( reLine.Matches( strLine ) )
		{
			wxString strStarNo = reLine.GetMatch(strLine, 1 );
			wxString strStarName = reLine.GetMatch(strLine, 2 );
			long nStarNo = 0;
			if( strStarNo.ToLong( &nStarNo ) )
			{
				m_vectWord[m_nWord].cat_type = CAT_OBJECT_TYPE_SAO;
				m_vectWord[m_nWord].cat_no = nStarNo;
				wxStrcpy( m_vectWord[m_nWord].name, strStarName );
				m_nWord++;
			}

		}
	}
	// close my file
	fclose( pFile );

	return( 1 );

}

/////////////////////////////////////////////////////////////////////
long CSkyCatalogStarNames::LoadTychoNames( CUnimapWorker* pWorker )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxChar strStarName[500];
	unsigned long t1=0, t2=0, t3=0;

	// debug
	if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT( "INFO :: Loading Tycho names ...") );

	// Read the index
	pFile = wxFopen( STARS_TYCHO_NAME_FILE, wxT("r") );
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
		memset( &strLine, 0, 1000*sizeof(wxChar) );
		memset( &strStarName, 0, 500*sizeof(wxChar) );

		// get one line out of the config file
		wxFgets( strLine, 2000, pFile );

		// check if event list
		if( wxSscanf( strLine, wxT("TYC %lu-%lu-%lu=%[^\n]\n"), &t1, &t2, &t3, strStarName ) )
		{
			m_vectWord[m_nWord].cat_type = CAT_OBJECT_TYPE_TYCHO;
			m_vectWord[m_nWord].zone_no = (unsigned int) t1;
			m_vectWord[m_nWord].cat_no = (unsigned int) t2;
			m_vectWord[m_nWord].comp_no = (unsigned char) t3;
			wxStrcpy( m_vectWord[m_nWord].name, strStarName );

			m_nWord++;
		}
	}
	// close my file
	fclose( pFile );

	return( 1 );

}
