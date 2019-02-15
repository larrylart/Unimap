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

//  wx
#include "wx/wxprec.h"
#include "wx/thread.h"
//precompiled headers
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
#include <wx/regex.h>
#include <wx/string.h>
#include <wx/filesys.h>
#include <wx/html/htmlpars.h>
#include <wx/fs_inet.h>
#include <wx/tokenzr.h>

// local headers
#include "../util/folders.h"
#include "../util/webfunc.h"
#include "libastro/libastro.h"
#include "sky.h"
#include "../unimap.h"
#include "../util/func.h"
#include "../unimap_worker.h"

// main header
#include "celestraksite.h"

using namespace std; 

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CCelestrakSite
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CCelestrakSite::CCelestrakSite( CSkyCatalogAes* pCatalogAes, CUnimapWorker* pWorker )
{
	m_pCatalogAes = pCatalogAes;
	m_pUnimapWorker = pWorker;

	// set sites - http://www.space-track.org
	m_vectSite[0] = wxT("http://celestrak.com");
	m_nSite = 1;

	// larry :: i moved site type in main object as sat type of sats ... 
	// to be used in other places
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CCelestrakSite
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CCelestrakSite::~CCelestrakSite( )
{

}

/////////////////////////////////////////////////////////////////////
// Method:	LoadFetchedFile
// Class:	CCelestrakSite
// Purpose:	load file catalog objects from file from the site
// Input:	file name
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CCelestrakSite::LoadFetchedFile( const wxString& strFile, int nObjType, int nSatType )
{
	FILE* pLocalFile = NULL;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[1000];
	wxChar strLineOne[1000];
	wxChar strLineTwo[1000];
	int nLineSize=0, nLineSizeOne=0, nLineSizeTwo=0;
//	wxString strWxLineOne = wxT("");
	wxString strWxLineTwo = wxT("");
	// other variables used
	unsigned long nCatNo=0, nLongVar=0, nNoradCatNo=0, nNoradCatNoTwo=0;
	int bError=0;
	DefCatAes* pCatAes=NULL;
	// regex
	wxRegEx reNameHasDEB = wxT( "(.+)\\ DEB.*" );

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}

	// inti/calculate progress bar stuff
	long nPCount=0;
	int nPStep=0;
	long nFileSize = GetFileSize( pLocalFile );
	long nPCountStep = (long) nFileSize/1295;

	// Reading lines from cfg file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
		bzero( strLine, 255 );
		bzero( strLineOne, 255 );
		bzero( strLineTwo, 255 );

		// read a line of max 1000 chars
		wxFgets( strLine, 1000, pLocalFile );
		nLineSize = wxStrlen( strLine );

		// if line empty (less the a minimum of 2 lets say - jump 
		if( nLineSize < 2 ) continue;

		// get line one
		wxFgets( strLineOne, 1000, pLocalFile );
		nLineSizeOne = wxStrlen( strLineOne );
		if( nLineSizeOne < 30 ) continue; 
		// get line two
		wxFgets( strLineTwo, 1000, pLocalFile );
		nLineSizeTwo = wxStrlen( strLineTwo );
		if( nLineSizeTwo < 30 ) continue; 

		wxString strWxLine = strLine;

		// check if we have a TLE record
		if( strLineOne[0] == '1' && strLineOne[1] == ' ' &&
			strLineTwo[0] == '2' && strLineTwo[1] == ' ' )
		{
			// extract /and compare? norad cat no
			wxString strWxLineOne = strLineOne;
			// :: norad_cat_no - line one
			if( !strWxLineOne.Mid( 2, 5 ).ToULong( &nNoradCatNo ) ) 
				continue;
			// :: norad_cat_no - line two
			if( !strWxLineOne.Mid( 2, 5 ).ToULong( &nNoradCatNoTwo ) ) 
				continue;
			// check if same 
			if( nNoradCatNo != nNoradCatNoTwo ) 
				continue;

			// in case of custom import - no default cat - check if it's debris or other known types
			if( nObjType == EARTH_ORBIT_OBJECT_MIXED || nSatType == EARTH_SAT_TYPE_CUSTOM_MIXED )
			{
				// check line name 
				if( reNameHasDEB.Matches( strWxLine ) )
				{
					nObjType = ARTIFICIAL_EARTH_MIXED_DEBRIS;
				}
			}

			///////////////////////////////////
			// now check if object is already in ... if so just push satellite type ?
			pCatAes = m_pCatalogAes->GetSatTypeByNoradNo( nNoradCatNo );
			if( pCatAes != NULL )
			{
				// if yes - check if type already exists
				if( !m_pCatalogAes->IsObjCategory( pCatAes, nSatType ) )
					m_pCatalogAes->AddSatTypeToObject( pCatAes, nSatType );

				//  UDATE :: PROCESS TLE RECORD & PUSH NEW TLE RECORD
				ProcessTLETwoRecord( strLine, strLineOne, strLineTwo, pCatAes, 1 );

				continue;
			}
			// reset aes object to zero
			ResetObjCatAes( m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData] );
			// set current epoch

			/////////////////////////////
			//  CREATE :: PROCESS TLE RECORD
			ProcessTLETwoRecord( strLine, strLineOne, strLineTwo, &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData]), 0 );
			// set main properties
			m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].cat_no = m_pCatalogAes->m_nData;
			m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].cat_type = CAT_OBJECT_TYPE_CELESTRAK;
			m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].type = SKY_OBJECT_TYPE_AES;
			// set norad cat number
			m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].norad_cat_no = nNoradCatNo;
			// set object type
			m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData].obj_type = nObjType;
			// push sat type
			m_pCatalogAes->AddSatTypeToObject( &(m_pCatalogAes->m_vectData[m_pCatalogAes->m_nData]), nSatType );

			//increment counter
			m_pCatalogAes->m_nData++;
			
		}

		///////
		// GUI :: update progress - todo: move to main fetch to have a progress bar for all
		if( m_pUnimapWorker )
		{
			// check if break
			if( m_pUnimapWorker->m_bCurrentTaskSetToBreak )
			{
				bError = 1;
				break;
			}

			if( nPCount > nPCountStep )
			{
				nPCount=0;
				nPStep++;
				m_pUnimapWorker->SetWaitDialogProgress( nPStep );
				continue;

			} else
				nPCount += nLineSize + nLineSizeOne + nLineSizeTwo;
		}
	}
	// close my file
	fclose( pLocalFile );

	// check end of read state
	if( !bError )
	{
		m_pCatalogAes->m_nCatalogLoaded = CATALOG_ID_CELESTRAK;
		return( m_pCatalogAes->m_nData );

	} else
	{
		m_pCatalogAes->UnloadCatalog( );
		m_pCatalogAes->m_nCatalogLoaded = CATALOG_ID_NONE;
		return( 0 );
	}
}

/////////////////////////////////////////////////////////////////////
int CCelestrakSite::ProcessTLETwoRecord( wxChar *name, wxChar *l1, wxChar *l2, 
											DefCatAes* op, int bUdate )
{
	double ep=0.0;
	int i=0;
	long nVarLong=0;
	unsigned long nVarULong=0;

	// get reference to current record
	DefTLERecord aesTLE;

	// check for correct line numbers, macthing satellite numbers and
	// correct checksums.
	while( isspace(*l1) ) l1++;
	if( *l1 != '1' ) return( -1 );
	while( isspace(*l2) ) l2++;
	if( *l2 != '2' ) return( -1 );
	if( wxStrncmp( l1+2, l2+2, 5 ) ) return( -1 );
	if( wx_tle_sum( l1 ) < 0 ) return( -1 );
	if( wx_tle_sum( l2 ) < 0 ) return( -1 );


	///////////////////////////
	// FIRST :: extract epoch time and ...

//#define SAT_USE_SAT_CODE_2
#ifdef SAT_USE_SAT_CODE_2
	double year = l1[19] - '0';
	if( l1[18] >= '0') year += (l1[18] - '0') * 10;
	 // cycle around Y2K 
	if( year < 57) year += 100;
	aesTLE->epoch_time = atof( l1 + 20 ) + JD1900 + (double)year * 365. + (double)((year - 1) / 4);
#else
	i = (int) wx_tle_fld( l1, 19, 20 );
	if( i < 57 ) i += 100;
	cal_mjd( 1, wx_tle_fld(l1, 21, 32), i+1900, &ep );
	// :: eso_epoch
	aesTLE.epoch_time = ep;
#endif
	// if bUdate == 1 check if already exist, then return
	if( bUdate == 1 && m_pCatalogAes->IsObjTLEEpoch( op, aesTLE.epoch_time ) ) return( -1 );

	////////////////////////////////
	// NAME - line 0
	if( bUdate == 0 )
	{
		wxString strSatName(name,wxConvUTF8);
		strSatName.Trim(0).Trim(1);
		// set name
		printf( op->cat_name, "%s", strSatName );
	}

	//////////////
	// PROCESS LINE 1
	wxString strLineOne(l1,wxConvUTF8);
//	// :: norad_cat_no - done .. up in the main loop
//	if( !strLineOne.Mid( 2, 5 ).ToULong( &nVarULong ) ) nVarULong = 0;
//	op->norad_cat_no = nVarULong;

	if( bUdate == 0 )
	{
		// :: security_class
		op->security_class = (unsigned char) (strLineOne.Mid( 7, 1 ))[0];
		// :: int_id_year
		if( !strLineOne.Mid( 9, 2 ).ToLong( &nVarLong ) ) nVarLong = 0;
		if( nVarLong < 57 ) nVarLong += 100;
		op->int_id_year = (unsigned int) nVarLong + 1900;
		// :: int_id_launch_no
		if( !strLineOne.Mid( 11, 3 ).ToLong( &nVarLong ) ) nVarLong = 0;
		op->int_id_launch_no = (unsigned int) nVarLong;
		// :: int_id_launch_piece
		strcpy( op->int_id_launch_piece, strLineOne.Mid( 14, 3 ).ToAscii() );
		// :: ephemeris_type - substract code for character '0'
		op->ephemeris_type = (unsigned char) ( (strLineOne.Mid( 62, 1 ))[0] - '0' );
	}

	// :: element_number
	if( !strLineOne.Mid( 64, 4 ).ToLong( &nVarLong ) ) nVarLong=0;
	aesTLE.element_number = (unsigned int) nVarLong;

	// :: mean_motion_second_time_deriv - not used in computation ???
	aesTLE.mean_motion_second_time_deriv = (float) wx_tle_expfld( l1, 45 );
	// :: eso_drag
	aesTLE.object_drag_coeff = (float) wx_tle_expfld( l1, 54 );
	// :: eso_decay
	aesTLE.orbit_decay_rate = (float) wx_tle_fld( l1, 34, 43 );

	////////////////////////
	// PROCESS LINE 2
	// :: eso_n
	aesTLE.mean_motion = wx_tle_fld( l2, 53, 63 );
	// :: eso_inc
	aesTLE.inclination = (float) wx_tle_fld( l2, 9, 16 );
	// :: eso_raan
	aesTLE.asc_node_ra = (float) wx_tle_fld( l2, 18, 25 );
	// :: eso_e
	aesTLE.eccentricity = (float)(wx_tle_fld( l2, 27, 33 ) * 1e-7);
	// :: eso_ap
	aesTLE.arg_of_perigee = (float) wx_tle_fld( l2, 35, 42 );
	// :: eso_M
	aesTLE.mean_anomaly = (float) wx_tle_fld( l2, 44, 51 );
	// :: eso_orbit
	aesTLE.orb_no_epoch = (int) wx_tle_fld( l2, 64, 68 );

	//////////
	// push tle in object record vector
	m_pCatalogAes->AddTLERecordToObject( op, aesTLE );

	///////////////////////
	// todo future :: set other object properties ?
 
	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	FetchObjects
// Class:	CCelestrakSite
// Purpose:	fetch catalog data from celestrak.com site into a file
// Input:	file name to save localy
// Output:	how many bytes where loaded
/////////////////////////////////////////////////////////////////////
long CCelestrakSite::FetchObjects( const wxString& strFile, int nCatId, int nPartId )
{
	long nCount=0;
	int i = 0;
	wxString strLog;
	unsigned long nWebFileSize=0;

	wxString strUrl;
	char strQuery[2000];

	// set aprox webfile size - for the meter 
	nWebFileSize=922000;

	//////////////////////////
	// now here we should check first if we already have that info localy
	if( !wxFileExists( wxString(strFile,wxConvUTF8) ) )
	{
		// for all available mirros
		for( i=0; i<m_nSite; i++ )
		{
			if( m_pUnimapWorker && m_pUnimapWorker->m_bCurrentTaskSetToBreak )
			{
				nCount = 0;
				break;
			}

			// build name for the url to request
			// http://celestrak.com/NORAD/elements/tle-new.txt
			strUrl.Printf( wxT("%s/NORAD/elements/%s.txt"), m_vectSite[i], m_pCatalogAes->m_vectSatType[nPartId].tle_file );

			// call online - to do call multiple site addresse - vizier has a few mirrors
			nCount = LoadWebFileT( strUrl, strFile, m_pUnimapWorker, nWebFileSize );
			// if success - exit loop
			if( nCount > 0 ) break;

			// log message
			if( m_pUnimapWorker && i<m_nSite-1 )
			{
				strLog.Printf( wxT("INFO :: failed to load from celestrak site %s try next mirror %s"), m_vectSite[i], m_vectSite[i-1] );
				m_pUnimapWorker->Log( strLog );
			}
		}

	} else
		nCount = 1;

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetObjects
// Class:	CCelestrakSite
// Purpose:	get objects from online celestrak site
// Input:	catalog id 
// Output:	no of objects
/////////////////////////////////////////////////////////////////////
long CCelestrakSite::GetObjects( int nCatId )
{
	wxString strLog;
	wxString strLocalFile;
	int i=0, j=0;
	wxString strCatName = wxT("UnknownCat");
	long nLoadedObjects=0, nLoadedBytes=0;

	///////////////////////////
	// set vars by cat type
	// :: DSO
	if( nCatId == CATALOG_ID_CELESTRAK )
		strCatName = wxT("CELESTRAK");

	///////////////////////
	// FOR ALL SITE PARTS - as i don't have a single big file
	for( i=0; i<m_pCatalogAes->m_nSatType; i++ )
	{
		// build name for local file
		strLocalFile.Printf( wxT("%s/%s_%s_cat.txt"), unMakeAppPath(wxT("data/remote/Celestrak")), strCatName, m_pCatalogAes->m_vectSatType[i].tle_file );

		// log info
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("INFO :: Fetching catalog objects ...") );

		/////////
		// call method to get stars from online site
		nLoadedBytes = FetchObjects( strLocalFile, nCatId, i );

		// check if anything was loaded - else skip to next - to check for return = 0 from all at the end
		if( nLoadedBytes <=0 )
		{
			if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("Failed to fetch remote data ...") );
			continue; //return( 0 );
		}

		// log info
		if( m_pUnimapWorker )
		{
			m_pUnimapWorker->SetWaitDialogMsg( wxT("Import data to local format ...") );

			strLog.Printf( wxT("INFO :: Loading fetched file=%s in local format ..."), strLocalFile );
			m_pUnimapWorker->Log( strLog );
		}

		////////////////////////////
		// and now load the file just downloaded
		if( nCatId == CATALOG_ID_CELESTRAK )
		{
			nLoadedObjects += LoadFetchedFile( strLocalFile, m_pCatalogAes->m_vectSatType[i].obj_type, m_pCatalogAes->m_vectSatType[i].sat_type );

		} else
			continue; //return( 0 );

		// now remove remote file
		//wxRemoveFile( strLocalFile );
	}

	/////////////////////////////////////////////////
	// LOAD CUSTOM TLE FILES - TODO :: move to catalog_aes
	nLoadedObjects += LoadFetchedFile( unMakeAppPath(wxT("data/catalog/aes/custom_import.tle")), EARTH_ORBIT_OBJECT_MIXED, EARTH_SAT_TYPE_CUSTOM_MIXED );

	////////////////////
	// check if anything was loaded
	if( nLoadedObjects > 0 )
	{
		// and now sort my catalog by magnitude
		if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Sort catalog objects ...") );
		// first sort elements
		m_pCatalogAes->SortByNoradCatNo( );
		// then sort tle records
		m_pCatalogAes->SortTLERecords( );

		// log message
		if( m_pUnimapWorker )
		{
			strLog.Printf( wxT("INFO :: loaded %d artifical earth satellites"), m_pCatalogAes->m_nData );
			m_pUnimapWorker->Log( strLog );
		}

	} else
	{
		// log message
		if( m_pUnimapWorker ) m_pUnimapWorker->Log( wxT("INFO :: failed to get dso objects from celestrak site") );
	}

	return( nLoadedObjects );
}
