////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
#include "../util/func.h"
#include "../unimap_worker.h"
#include "../config/mainconfig.h"
#include "catalog_dso_ngc.h"
#include "catalog_dso_ic.h"
#include "catalog_dso_assoc.h"

// main header
#include "catalog_dso_names.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogDsoNames
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogDsoNames::CSkyCatalogDsoNames( CSky* pSky )
{
	m_pSky = pSky;
	m_vectDsoName = NULL;
	m_nDsoName = 0;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogDsoNames
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogDsoNames::~CSkyCatalogDsoNames()
{
	// also clean the names
	UnLoadDsoNames( );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetDsoSimpleCatName
// Class:	CSkyCatalogDsoNames
// Purpose:	Get a dso simple name CATANAME+CATID
// Input:	dso id , reference to string
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoNames::GetDsoSimpleCatName( StarDef& pDso, wxString& strDsoName )
{
	// empty string
	strDsoName = wxT("");
	// return if no match
//	if( !m_vectCatDso[nDsoId].match ) return( 0 );

//	unsigned long nCatNo = m_vectCatDso[nDsoId].cat_no;

	// now check by object type
	if( pDso.cat_type == CAT_OBJECT_TYPE_MESSIER )
	{
		strDsoName.Printf( wxT("M%d"), pDso.cat_no );

	} else if( pDso.cat_type == CAT_OBJECT_TYPE_NGC )
	{
		strDsoName.Printf( wxT("NGC%d"), pDso.cat_no );

	} else if( pDso.cat_type == CAT_OBJECT_TYPE_IC )
	{
		strDsoName.Printf( wxT("IC%d"), pDso.cat_no );

	} else if( pDso.cat_type == CAT_OBJECT_TYPE_PGC )
	{
		strDsoName.Printf( wxT("PGC%d"), pDso.cat_no );

	} else if( pDso.cat_type == CAT_OBJECT_TYPE_MGC )
	{
		strDsoName.Printf( wxT("MGC%d"), pDso.cat_no );

	} else if( pDso.cat_type == CAT_OBJECT_TYPE_UGC )
	{
		strDsoName.Printf( wxT("UGC%d"), pDso.cat_no );

	} else
	{
		// no cat to match return 0
		return( 0 );
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoNames::GetDsoSimpleCatName( DsoDefCatBasic& pDso, unsigned char nType, wxString& strDsoName )
{
	StarDef obj;
	obj.cat_no = pDso.cat_no;
	//obj.cat_type = nType; ?? what is this used for ???
	obj.cat_type = pDso.cat_type;
	return( GetDsoSimpleCatName( obj, strDsoName ) );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetDsoCatName
// Class:	CSkyCatalogDsoNames
// Purpose:	Get a dso catalog name
// Input:	dso id, pointer to string to return
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoNames::GetDosCatName( StarDef& pDso, wxString& strDsoCatName )
{
	int k = 0, ret=0;
	wxString strCommonName;
	wxString strName;
	// vector of lavbel fields used to construct the label
	wxString vectLabelField[6];
	int nLabelField=0;
	int bDSOObjLabelUseCatNo, bDSOObjLabelUseCName, bDsoObjLabelUseType;

	/////////////
	// set fields by object type
	if( pDso.type == SKY_OBJECT_TYPE_NEBULA )
	{
		bDSOObjLabelUseCatNo = m_pConfig->m_bNebObjLabelUseCatNo;
		bDSOObjLabelUseCName = m_pConfig->m_bNebObjLabelUseCName;
		bDsoObjLabelUseType = m_pConfig->m_bNebObjLabelUseType;

	} else if( pDso.type == SKY_OBJECT_TYPE_CLUSTER )
	{
		bDSOObjLabelUseCatNo = m_pConfig->m_bCluObjLabelUseCatNo;
		bDSOObjLabelUseCName = m_pConfig->m_bCluObjLabelUseCName;
		bDsoObjLabelUseType = m_pConfig->m_bCluObjLabelUseType;

	} else if( pDso.type == SKY_OBJECT_TYPE_GALAXY )
	{
		bDSOObjLabelUseCatNo = m_pConfig->m_bGalObjLabelUseCatNo;
		bDSOObjLabelUseCName = m_pConfig->m_bGalObjLabelUseCName;
		bDsoObjLabelUseType = m_pConfig->m_bGalObjLabelUseType;

	} else
	{
		bDSOObjLabelUseCatNo = m_pConfig->m_bGalObjLabelUseCatNo;
		bDSOObjLabelUseCName = m_pConfig->m_bGalObjLabelUseCName;
		bDsoObjLabelUseType = m_pConfig->m_bGalObjLabelUseType;
	}

	if( pDso.cat_type == CAT_OBJECT_TYPE_MESSIER )
	{
		ret = 1;
		// if to use cat no
		if( bDSOObjLabelUseCatNo )
			vectLabelField[nLabelField++].Printf( wxT("M%d"), pDso.cat_no );
		// if to use common name
		if( bDSOObjLabelUseCName && GetDsoCommonName( strCommonName, pDso.cat_no, CAT_OBJECT_TYPE_MESSIER ) )
		{
			vectLabelField[nLabelField++].Printf( wxT("%s"), strCommonName );
			ret = 2;
		}

	} else if( pDso.cat_type == CAT_OBJECT_TYPE_NGC )
	{
		ret = 1;
		// if to use cat no
		if( bDSOObjLabelUseCatNo )
			vectLabelField[nLabelField++].Printf( wxT("NGC%d"), pDso.cat_no );
		// if to use common name
		if( bDSOObjLabelUseCName && GetDsoCommonName( strCommonName, pDso.cat_no, CAT_OBJECT_TYPE_NGC ) )
		{
			vectLabelField[nLabelField++].Printf( wxT("%s"), strCommonName );
			ret = 2;
		}

	} else if( pDso.cat_type == CAT_OBJECT_TYPE_IC )
	{
		ret = 1;
		// if to use cat no
		if( bDSOObjLabelUseCatNo )
			vectLabelField[nLabelField++].Printf( wxT("IC%d"), pDso.cat_no );
		// if to use common name
		if( bDSOObjLabelUseCName && GetDsoCommonName( strCommonName, pDso.cat_no, CAT_OBJECT_TYPE_IC ) )
		{
			vectLabelField[nLabelField++].Printf( wxT("%s"), strCommonName );
			ret = 2;
		}

	} else if( pDso.cat_type == CAT_OBJECT_TYPE_PGC )
	{
		ret = 1;
		// if to use cat no
		if( bDSOObjLabelUseCatNo )
			vectLabelField[nLabelField++].Printf( wxT("PGC%d"), pDso.cat_no );
		// if to use common name
		if( bDSOObjLabelUseCName && GetDsoCommonName( strCommonName, pDso.cat_no, CAT_OBJECT_TYPE_PGC ) )
		{
			vectLabelField[nLabelField++].Printf( wxT("%s"), strCommonName );
			ret = 2;
		}

	} else if( pDso.cat_type == CAT_OBJECT_TYPE_MGC )
	{
		ret = 1;
		// if to use cat no
		if( bDSOObjLabelUseCatNo )
			vectLabelField[nLabelField++].Printf( wxT("MGC%d"), pDso.cat_no );
		// if to use common name
		if( bDSOObjLabelUseCName && GetDsoCommonName( strCommonName, pDso.cat_no, CAT_OBJECT_TYPE_MGC ) )
		{
			vectLabelField[nLabelField++].Printf( wxT("%s"), strCommonName );
			ret = 2;
		}

	} else if( pDso.cat_type == CAT_OBJECT_TYPE_UGC )
	{
		ret = 1;
		// if to use cat no
		if( bDSOObjLabelUseCatNo )
			vectLabelField[nLabelField++].Printf( wxT("UGC%d"), pDso.cat_no );
		// if to use common name
		if( bDSOObjLabelUseCName && GetDsoCommonName( strCommonName, pDso.cat_no, CAT_OBJECT_TYPE_UGC ) )
		{
			vectLabelField[nLabelField++].Printf( wxT("%s"), strCommonName );
			ret = 2;
		}

	} else
	{
		ret = 0;
	}


	////////////////////////////
	strName = wxT("");
	// cat dso :: now from all fields build name
	for( k=0; k<nLabelField; k ++ )
	{
		if( k == 0 ) 
			strName = vectLabelField[k];
		else
			strName += wxT(" - ") + vectLabelField[k];
	}
	
	// free remote if
	// if( strDsoCatName ) free( strDsoCatName );

	if( nLabelField )
	{
		//strDsoCatName = (char*) malloc( (strlen(strName)+1)*sizeof(char) );
		strDsoCatName = strName;
	} else
	{
		//strDsoCatName = wxT("");
	}

	return( ret );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetDsoNameRaDec
// Class:	CSkyCatalogDsoNames
// Purpose:	get dso name from common names coord or related catalogs
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoNames::GetDsoNameRaDec( unsigned long nId, double& nRa,
														double& nDec )
{
	if( !m_vectDsoName ) return( 0 );

	if( m_vectDsoName[nId].ctype == DSO_COMMON_NAME_HAS_RADEC )
	{
		nRa = m_vectDsoName[nId].ra;
		nDec = m_vectDsoName[nId].dec;

	} else if( m_vectDsoName[nId].ctype == DSO_COMMON_NAME_HAS_NGC )
	{
		m_pSky->m_pCatalogDsoNgc->GetRaDec( m_vectDsoName[nId].cat_no, nRa, nDec );

	} else if( m_vectDsoName[nId].ctype == DSO_COMMON_NAME_HAS_IC )
	{
		m_pSky->m_pCatalogDsoIc->GetRaDec( m_vectDsoName[nId].cat_no,  nRa, nDec );
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	UnLoadDsoNames
// Class:	CSkyCatalogDsoNames
// Purpose:	unload dso common names
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyCatalogDsoNames::UnLoadDsoNames( )
{
	unsigned int i = 0;

	if( m_vectDsoName )
	{
		for( i=0; i<m_nDsoName; i++ )
			free( m_vectDsoName[i].name );

		free( m_vectDsoName );
		m_vectDsoName = NULL;
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadDsoNames
// Class:	CSkyCatalogDsoNames
// Purpose:	load dso common names
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
//long CSkyCatalogDsoNames::LoadDsoNames( const char* strFile )
long CSkyCatalogDsoNames::LoadDsoNames( CUnimapWorker* pWorker )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxString strLog;
	unsigned long nCatNo=0;
	double nDecDeg=0, nRaDeg=0;
//	wxString strWxLine = wxT("");
	double nTimeRef = 0;

	// first check if one already is and close
	UnLoadDsoNames( );
	// allocate my vector
	m_vectDsoName = (DsoCommonName*) malloc( 500*sizeof( DsoCommonName ) );

	// info
	if( pWorker ) pWorker->Log( wxT("INFO :: Loading DSO names ...") );
	nTimeRef = GetDTime();

	// Read the index
//	pFile = fopen( strFile, "r" );
	pFile = wxFopen( DSO_COMMON_NAME_FILE, wxT("r") );

	// check if there is any configuration to load
	if( !pFile )
	{
		// debug
		if( pWorker )
		{
			strLog.Printf( wxT("Warning :: could not open dso cnames file %s"), DSO_COMMON_NAME_FILE );
			pWorker->Log( strLog );
		}
		return( -1 );
	}
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );

		// if line empty jump
		if( wxStrlen( strLine ) < 3 )
			continue;

		// set this to wx string
		wxString strWxLine = strLine;

		/////////////////////////////
		// here we extract field we need

		// extract first character
//		wxString strType = strWxLine.Mid( 0, 1 );
		// check if commented line
		if( strLine[0] == '#' )
			continue;
		// set type
		if( strLine[0] == 'N' )
		{
			m_vectDsoName[m_nDsoName].ctype = DSO_COMMON_NAME_HAS_NGC;

		} else if( strLine[0] == 'I' )
		{
			m_vectDsoName[m_nDsoName].ctype = DSO_COMMON_NAME_HAS_IC;

		} else if( strLine[0] == 'C' )
		{
			m_vectDsoName[m_nDsoName].ctype = DSO_COMMON_NAME_HAS_RADEC;

		} else
			continue;

		// DSO common name
		wxString strDsoName = strWxLine.Mid( 2, 28 ).Trim(0).Trim(1);
		// if type is ra dec then
		if( m_vectDsoName[m_nDsoName].ctype == DSO_COMMON_NAME_HAS_RADEC )
		{
			// extract Ra 48-57
			wxString strRa = strWxLine.Mid( 48, 10 );
			// extract Dec 59-68
			wxString strDec = strWxLine.Mid( 59, 10 );

			// convert ra in deg
			if( !ConvertRaToDeg( strRa, nRaDeg ) )
				continue;

			// convert dec to deg
			if( !ConvertDecToDeg( strDec, nDecDeg ) )
				continue;

			// set my cats records
			m_vectDsoName[m_nDsoName].ra = nRaDeg;
			m_vectDsoName[m_nDsoName].dec = nDecDeg;

		} else
		{
			// extract catalog no - 31-46
			if( !strWxLine.Mid( 31, 4 ).ToULong( &nCatNo ) )
				continue;

			m_vectDsoName[m_nDsoName].cat_no = nCatNo;
		}
		// Extract comment 69 - 200 ... or more
//		wxString strComment = strWxLine.Mid( 69, 255 );

		// set dso name
		m_vectDsoName[m_nDsoName].name = (wxChar*) malloc( (strDsoName.Length()+1)*sizeof(wxChar) );
		wxStrcpy( m_vectDsoName[m_nDsoName].name, strDsoName );

		// increase dso common name counter
		m_nDsoName++;
	}
	// close my file
	fclose( pFile );

	// debug
	nTimeRef = GetDTime()-nTimeRef;
	if( pWorker )
	{
		strLog.Printf( wxT("DEBUG :: dso names loaded in %lu.%lu"),
					(unsigned long) nTimeRef/1000, (unsigned long) nTimeRef%1000 );
		pWorker->Log( strLog );
	}


	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetDsoCommonName
// Class:	CSkyCatalogDsoNames
// Purpose:	get dso name based on cat no
// Input:	reference to name, cat no, cat type
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogDsoNames::GetDsoCommonName( wxString& strName, unsigned long nCatNo,
									 int nType )
{
	unsigned long i = 0;
	unsigned long nNgcCatNo = nCatNo;
	int bFound = 0;

	// if messier we need the ngc no
	if( nType == CAT_OBJECT_TYPE_MESSIER )
	{
		// larry todo: think here to speed up search
		// nNgcCatNo = GetMessierNgcNo( nCatNo ); - this was the old code - get rid of it
		nNgcCatNo = m_pSky->m_pCatalogDsoAssoc->IsMessierObjectNgcIc( nNgcCatNo );
		if( nNgcCatNo == 0 ) return( 0 );
	}

	// now look for id either ngc or ic
	if( nType == CAT_OBJECT_TYPE_NGC || nType == CAT_OBJECT_TYPE_MESSIER )
	{
		for( i=0; i<m_nDsoName; i++ )
		{
			if( m_vectDsoName[i].ctype == DSO_COMMON_NAME_HAS_NGC &&
					m_vectDsoName[i].cat_no == nNgcCatNo )
			{
					strName = m_vectDsoName[i].name;
					bFound = 1;
					break;
			}
		}

	} else if( nType == CAT_OBJECT_TYPE_IC )
	{
		for( i=0; i<m_nDsoName; i++ )
		{
			if( m_vectDsoName[i].ctype == DSO_COMMON_NAME_HAS_IC &&
					m_vectDsoName[i].cat_no == nCatNo )
			{
					strName = m_vectDsoName[i].name;
					bFound = 1;
					break;
			}
		}

	}  else if( nType == CAT_OBJECT_TYPE_PGC )
	{
		for( i=0; i<m_nDsoName; i++ )
		{
			if( m_vectDsoName[i].ctype == DSO_COMMON_NAME_HAS_PGC &&
					m_vectDsoName[i].cat_no == nCatNo )
			{
					strName = m_vectDsoName[i].name;
					bFound = 1;
					break;
			}
		}

	}

	return( bFound );
}
