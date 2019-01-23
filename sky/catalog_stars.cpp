
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

// local headers
#include "../unimap_worker.h"
#include "../util/func.h"
#include "../config/mainconfig.h"
#include "sky.h"
#include "../unimap.h"
#include "unimapsite.h"
// catalogs
#include "catalog_stars_2mass.h"
#include "catalog_stars_gsc.h"

// main header
#include "catalog_stars.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalogStars
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogStars::CSkyCatalogStars( CSky* pSky ) : CSkyCatalog( pSky )
{
	m_nObjectType = CAT_OBJECT_TYPE_STAR;

	m_vectStar = NULL;
	m_nStar = 0;
	m_nStarAllocated = 0;

	m_nMaxMag = 0;
	m_nMinMag = 0;

	// reset last region loaded to zero
	m_bLastLoadRegion = 0;
	m_nLastRegionLoadedCenterRa = 0;
	m_nLastRegionLoadedCenterDec = 0;
	m_nLastRegionLoadedWidth = 0;
	m_nLastRegionLoadedHeight = 0;

	m_nCatalogLoaded = CATALOG_ID_NONE;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalogStars
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalogStars::~CSkyCatalogStars()
{
	//free( m_vectStar );
	UnloadCatalog( );
}

/////////////////////////////////////////////////////////////////////
// Method:	UnloadCatalog
// Class:	CSkyCatalogStars
// Purpose:	free catalog data, reset counters etc
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyCatalogStars::UnloadCatalog( )
{
	int i=0;

	for( i=0; i<m_nStar; i++ )
	{
		if( m_vectStar[i].cat_name ) free( m_vectStar[i].cat_name );
		m_vectStar[i].cat_name = NULL;
	}

	if( m_vectStar ) free( m_vectStar );
	m_vectStar = NULL;
	m_nStar = 0;

	m_nCatalogLoaded = CATALOG_ID_NONE;

	// reset last region loaded to zero
	m_nLastRegionLoadedCenterRa = 0;
	m_nLastRegionLoadedCenterDec = 0;
	m_nLastRegionLoadedWidth = 0;
	m_nLastRegionLoadedHeight = 0;

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	GetSimpleCatName
// Class:	CSkyCatalogStars
// Purpose:	Get a star simple name CATANAME+CATID
// Input:	star, reference to string
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogStars::GetSimpleCatName( StarDefCatBasic& pStar, wxString& strStarName )
{
	// empty string
	strStarName = wxT("");

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
		strStarName.Printf( wxT("NOMAD%.4d-%.7d") ,
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
void CSkyCatalogStars::SetStarStringCatalogName( StarDefCatBasic& pStar, const char* strCatName )
{
	if( pStar.cat_name ) free( pStar.cat_name );
	pStar.cat_name = (char*) malloc( (strlen(strCatName)+1)*sizeof(char) );
	strcpy( pStar.cat_name, strCatName );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExtractStarsFromCatalog
// Class:	CSkyCatalogStars
// Purpose:	extract some stars from catalog
// Input:	vector to put stars in and how many, projection used
// Output:	how many stars where extracted
/////////////////////////////////////////////////////////////////////
int CSkyCatalogStars::ExtractStarsFromCatalog( StarDef* vectStar, int nStarCount,
										unsigned int nImgWidth, unsigned int nImgHeight,
										double& nMinX, double& nMaxX, 
										double& nMinY, double& nMaxY, int nPrjType )
{
	// set centern in the middle 
	double nOrigRA = 180.0;
	double nOrigDEC = 0.0;

	unsigned long i = 0;
	unsigned long nCount = 0;
	StarDef star;
	// init to min/max - to do use the system function
	nMinX = DBL_MAX;
	nMinY = DBL_MAX;
	nMaxX = DBL_MIN;
	nMaxY = DBL_MIN;
	// default winscale
	double nWinScale = 10;

	// for every star in my catalog
	for( i=0; i<m_nStar-1; i++ )
	{
		// if count over the limit I want exist
		if( nCount == nStarCount )
		{
			break;
		} else
		{
/*			// if name -copy
			if( m_vectStar[i].cat_name )
			{
				if( vectStar[nCount].cat_name ) free( vectStar[nCount].cat_name );
				vectStar[nCount].cat_name = (char*) malloc( (strlen(m_vectStar[i].cat_name)+1)*sizeof(char) );
				strcpy( vectStar[nCount].cat_name, m_vectStar[i].cat_name );

			}
*/
			vectStar[nCount].cat_name = NULL;

			vectStar[nCount].cat_no = m_vectStar[i].cat_no;
			vectStar[nCount].cat_type = m_vectStar[i].cat_type;
			vectStar[nCount].zone_no = m_vectStar[i].zone_no;
			vectStar[nCount].comp_no = m_vectStar[i].comp_no;
			vectStar[nCount].cat_id = i;

			vectStar[nCount].ra = m_vectStar[i].ra;
			vectStar[nCount].dec = m_vectStar[i].dec;
			vectStar[nCount].mag = m_vectStar[i].mag;
			// reset match to zero
			vectStar[nCount].match = 0;
			// reset other vars to zero as well
			vectStar[nCount].theta = 0;
			vectStar[nCount].a = 0;
			vectStar[nCount].b = 0;
			vectStar[nCount].fwhm = 0;
			vectStar[nCount].vmag = 0;
			vectStar[nCount].kronfactor = 0;
			vectStar[nCount].state = 0;
			vectStar[nCount].img_id = 0;
			vectStar[nCount].type = 0;

			// temp debug
			int nerror = 0;
			if( m_vectStar[i].dec < 40. )
				nerror = 1;

			// now project window
			m_pSky->ProjectStar( vectStar[nCount], nOrigRA, nOrigDEC, nPrjType );

			// apply scale factor
//				vectStar[nCount].x = (double) (vectStar[nCount].x*nWinScale); //10;
//				vectStar[nCount].y = (double) (vectStar[nCount].y*nWinScale); //10;

			// mind max and min
			if( vectStar[nCount].x < nMinX ) nMinX = vectStar[nCount].x;
			if( vectStar[nCount].x > nMaxX ) nMaxX = vectStar[nCount].x;

			if( vectStar[nCount].y < nMinY ) nMinY = vectStar[nCount].y;
			if( vectStar[nCount].y > nMaxY ) nMaxY = vectStar[nCount].y;

			nCount++;
		}
	}

	// now rescale using min/max from image abd perhaps ...
	// realign the stars found to 0
	nWinScale = (double) nImgWidth/(nMaxX-nMinX);
	for( i=0; i<nCount; i++ )
	{
		vectStar[i].x = (double) (vectStar[i].x*nWinScale);
		vectStar[i].y = (double) (vectStar[i].y*nWinScale);
	}

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	CutAreaFromCatalog
// Class:	CSkyCatalogStars
// Purpose:	cut a windows from catalog by coord
// Input:	center ra/dec field width/height, return star vector
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyCatalogStars::CutAreaFromCatalog( double nCenterRa, double nCenterDec, double nRadius,
										StarDef* vectStar, int nStarCount,
										unsigned int nImgWidth, unsigned int nImgHeight,
										double& nMinX, double& nMaxX, double& nMinY, double& nMaxY, 
										int bWithImgProjection, double nPrjScale )
{
	unsigned long i = 0;
	unsigned long nCount = 0;
	StarDef star;
	int bDecOverTheTop = 0;
	// init to min/max - to do use the system function

	nMinX = DBL_MAX;
	nMinY = DBL_MAX;
	nMaxX = DBL_MIN;
	nMaxY = DBL_MIN;
	// default winscale
	double nWinScale = 10;

	// first we clean the incoming vector
	for( i=0; i<nStarCount; i++ )
	{
		// clear and set to null
		if( vectStar[nCount].cat_name )
		{
			free( vectStar[nCount].cat_name );
			vectStar[nCount].cat_name = NULL;
		}
	}
	memset( vectStar, 0, nStarCount*sizeof(StarDef) );

	// -- instead of (double) nRadius*PI_FACT;
	double nRadiusDeg = nRadius;
	if( nRadiusDeg > 180.0*0.8 ) nRadiusDeg = 180.0*0.8;

	double nDistDeg = DBL_MAX;

	// for every star in my catalog - was  i<m_nStar-1 ????  why ?!!!!
	for( i=0; i<m_nStar; i++ )
	{
		////////////////////////////////////////////
		// calculate distance 
		nDistDeg = CalcSkyDistance( m_vectStar[i].ra, m_vectStar[i].dec, nCenterRa, nCenterDec );
		// check if in radius 
		if( nDistDeg <= nRadiusDeg  )
		{
			// check if my projection is in the image if flag set so
			if( bWithImgProjection )
			{
				memset( &star, 0, sizeof(StarDef) );

				star.ra = m_vectStar[i].ra;
				star.dec = m_vectStar[i].dec;

				// project star on tangent plan using the found orig
				m_pSky->ProjectStar( star, nCenterRa, nCenterDec );
				// larry :: my damn multiplication factor
				star.x = star.x*nPrjScale; //10;
				star.y = star.y*nPrjScale; //10;
				// and now apply inverse trans
				m_pSky->TransObj_SkyToImage( star );
//				star.x = -star.x;

				// check if object is in my image
				if( star.x < 0 || star.y < 0 || star.x > nImgWidth || star.y > nImgHeight )
				{
					continue;
				}
			}
			// if count over the limit I want exist
			if( nCount == nStarCount )
			{
				break;
			} else
			{
				// get the orig cat star id
				vectStar[nCount].cat_id = i;

/*				// if name -copy
				if( m_vectStar[i].cat_name )
				{
					//if( vectStar[nCount].cat_name ) free( vectStar[nCount].cat_name );
					vectStar[nCount].cat_name = (char*) malloc( (strlen(m_vectStar[i].cat_name)+1)*sizeof(char) );
					strcpy( vectStar[nCount].cat_name, m_vectStar[i].cat_name );

				}
*/
				vectStar[nCount].cat_name = NULL;

				// the get the other params
				vectStar[nCount].cat_no = m_vectStar[i].cat_no;
				vectStar[nCount].cat_type = m_vectStar[i].cat_type;
				vectStar[nCount].zone_no = m_vectStar[i].zone_no;
				vectStar[nCount].comp_no = m_vectStar[i].comp_no;

				vectStar[nCount].ra = m_vectStar[i].ra;
				vectStar[nCount].dec = m_vectStar[i].dec;
				vectStar[nCount].mag = m_vectStar[i].mag;
				// reset match to zero
				vectStar[nCount].match = 0;
				// also set other potential harmfull vars - if not set
				vectStar[nCount].vmag = 0;
				vectStar[nCount].fwhm = 0;
				vectStar[nCount].kronfactor = 0;
				vectStar[nCount].img_id = i;

				///////////////////////
				// temp debug
				int nerror = 0;
				if( m_vectStar[i].dec < 40. )
					nerror = 1;
				//////////////////////

// - this wont work because I use previous projection calculated here also
//				if( bWithImgProjection )
//				{
//					vectStar[nCount].x = star.x;
//					vectStar[nCount].y = star.y;
//				} else
//				{
					m_pSky->ProjectStar( vectStar[nCount], nCenterRa, nCenterDec );
//				}

				// apply scale factor
//				vectStar[nCount].x = (double) (vectStar[nCount].x*nWinScale); //10;
//				vectStar[nCount].y = (double) (vectStar[nCount].y*nWinScale); //10;

				// mind max and min
				if( vectStar[nCount].x < nMinX ) nMinX = vectStar[nCount].x;
				if( vectStar[nCount].x > nMaxX ) nMaxX = vectStar[nCount].x;

				if( vectStar[nCount].y < nMinY ) nMinY = vectStar[nCount].y;
				if( vectStar[nCount].y > nMaxY ) nMaxY = vectStar[nCount].y;

				nCount++;
			}

		}
	}

	// now rescale using min/max from image abd perhaps ...
	// realign the stars found to 0
	nWinScale = (double) nImgWidth/(nMaxX-nMinX);
	for( i=0; i<nCount; i++ )
	{
		vectStar[i].x = (double) (vectStar[i].x*nWinScale);
		vectStar[i].y = (double) (vectStar[i].y*nWinScale);
	}

/*	nMaxX = nImgWidth;
	nMinX = 0;
	nMaxY = nImgHeight;
	nMinY = 0;
*/
	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	sort stars  by magnitude
// Class:	CSkyCatalogStars
// Purpose:	sort stars in order of magnitude
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyCatalogStars::SortByMagnitude( )
{
//	unsigned long i = 0;
//	int bOrder = 1;
//	long nCount = 0;

	std::sort( m_vectStar, m_vectStar+m_nStar, OnSortCatStar );

	m_nMaxMag = m_vectStar[m_nStar-1].mag;
	m_nMinMag = m_vectStar[0].mag;
	// return status ...
	return( m_nStar );
}

/////////////////////////////////////////////////////////////////////
// Method:	IsCatStar
// Class:	CSkyCatalogStars
// Purpose:	check if a star is in a certain catalog - for dups removal ?
// Input:	cat type, no, zone no, comp no ?
// Output:	0/1 foudn or not
/////////////////////////////////////////////////////////////////////
int CSkyCatalogStars::IsCatStar( unsigned char nCatType, unsigned long nCatNo,
								unsigned int nZoneNo, unsigned char nCompNo, char* strCatName )
{
	int bFound = 0;
	int i=0;

	for( i=0; i<m_nStar; i++ )
	{
		if( nCatType == m_vectStar[i].cat_type )
		{
			if( nCatType == CAT_OBJECT_TYPE_TYCHO )
			{
				if( nZoneNo == m_vectStar[i].zone_no &&
					nCatNo == m_vectStar[i].cat_no &&
					nCompNo == m_vectStar[i].comp_no )
				{
					bFound = 1;
					break;
				}

			} else if( nCatType == CAT_OBJECT_TYPE_USNO )
			{
				if( nZoneNo == m_vectStar[i].zone_no &&
					nCatNo == m_vectStar[i].cat_no )
				{
					bFound = 1;
					break;
				}

			} else if( nCatType == CAT_OBJECT_TYPE_NOMAD )
			{
				if( nZoneNo == m_vectStar[i].zone_no &&
					nCatNo == m_vectStar[i].cat_no )
				{
					bFound = 1;
					break;
				}

			} else if( nCatType == CAT_OBJECT_TYPE_2MASS )
			{
				if( strcmp( strCatName, m_vectStar[i].cat_name ) == 0 ) 
				{
					bFound = 1;
					break;
				}

			} else if( nCatType == CAT_OBJECT_TYPE_GSC )
			{
				if( strcmp( strCatName, m_vectStar[i].cat_name ) == 0 ) 
				{
					bFound = 1;
					break;
				}

			}
		}
	}

	return( bFound );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetStarRaDec
// Class:	CSkyCatalogStars
// Purpose:	get current cat star ra/dec by cat no
// Input:	catalog number, reference to double ra dec
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogStars::GetStarRaDec( unsigned long nCatNo, double& nRa,
														double& nDec )
{
	int i =0;

	// look in the entire catalog for my cat no
	for( i=0; i<m_nStar; i++ )
	{
		if( m_vectStar[i].cat_no == nCatNo )
		{
			nRa = m_vectStar[i].ra;
			nDec = m_vectStar[i].dec;
		}
	}


	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	DownloadCatalog
// Class:	CSkyCatalogStars
// Purpose:	download a star catalog from a remote location
// Input:	catalog number
// Output:	status 0=failure
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogStars::DownloadCatalog( int nCatId, int nCatSource )
{
	unsigned long nCount = 0;

	// check by source type - if unimap just call class unimapsite
	if( nCatSource == CATALOG_DOWNLOAD_SOURCE_UNIMAP )
	{
		CUnimapSite rBinaryCat( this, m_pUnimapWorker );
		//m_strFile += wxT( "_unimap_test_" );
		nCount = rBinaryCat.DownloadCatalog( m_strFile, nCatId );

	} else if( nCatSource == CATALOG_DOWNLOAD_SOURCE_VIZIER )
	{
		// just to make sure
		UnloadCatalog( );

		//////////////
		// now check by type - catalog id
		if( nCatId == CATALOG_ID_SAO )
		{
			CSkyCatalogStarsSao rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile += wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				ExportBinary( );
			}

		} else if( nCatId == CATALOG_ID_HIPPARCOS )
		{
			CSkyCatalogStarsHipparcos rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile += wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				ExportBinary( );
			}

		} else if( nCatId == CATALOG_ID_TYCHO_2 )
		{
			CSkyCatalogStarsTycho rCat( this, m_pUnimapWorker );
			nCount = rCat.Load( CATALOG_LOAD_REMOTE );

			// if data present - export
			if( nCount > 0 )
			{
				// export binary - add an extra string for now - to test without breaking the existant
				//m_strFile += wxT( "_" );
				if( m_pUnimapWorker ) m_pUnimapWorker->SetWaitDialogMsg( wxT("Save catalog in binary format ...") );
				rCat.ExportTycho2Binary( );
			}
		}
	}

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadCatalogRegion
// Class:	CSkyCatalogStars
// Purpose:	load a region from catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogStars::LoadCatalogRegion( int nCatType, double nRa, double nDec,
												   double nWidth, double nHeight )
{
	unsigned long nCount = 0;
	// first we unload what is currently loaded - a brutal by simpler
	// aproach - to do later the more extesive eficient loading

	// check if already loaded this region
	if( m_nCatalogLoaded == nCatType && m_nStar > 0 && 
		nRa == m_nLastRegionLoadedCenterRa && nDec == m_nLastRegionLoadedCenterDec &&
		nWidth == m_nLastRegionLoadedWidth && nHeight == m_nLastRegionLoadedHeight )
	{
		return( m_nStar );
	}

	// if not loaded then load now
	UnloadCatalog( );

	// check load type
	int nLoadType = CATALOG_LOAD_FILE_BINARY;
	if( m_pSky->m_pConfig->m_bStarsCatLoadRemote ) nLoadType = CATALOG_LOAD_REMOTE;
	int nLoadSource = m_pSky->m_pConfig->m_nStarsCatLoadRemoteLocation;

	// then we check to load catalog type
	if( nCatType == CATALOG_ID_SAO )
	{
		CSkyCatalogStarsSao rCat( this, m_pUnimapWorker );
		// now if region
		if( m_pSky->m_pConfig->m_bStarsCatLoadLocalRegion )
		{
			nCount = rCat.Load( nLoadType, 0, 1, nRa, nDec, nWidth, nHeight );
		} else
		{
			nCount = rCat.Load( );
		}

	} else if( nCatType == CATALOG_ID_HIPPARCOS )
	{
		CSkyCatalogStarsHipparcos rCat( this, m_pUnimapWorker );
		// now if region
		if( m_pSky->m_pConfig->m_bStarsCatLoadLocalRegion )
		{
			nCount = rCat.Load( nLoadType, 0, 1, nRa, nDec, nWidth, nHeight );
		} else
		{
			nCount = rCat.Load( );
		}

	}  else if( nCatType == CATALOG_ID_TYCHO_2 )
	{
		CSkyCatalogStarsTycho rCat( this, m_pUnimapWorker );
		// now if region
		if( m_pSky->m_pConfig->m_bStarsCatLoadLocalRegion )
		{
			nCount = rCat.Load( nLoadType, 0, 1, nRa, nDec, nWidth, nHeight );
		} else
		{
			nCount = rCat.Load( );
		}

	} else if( nCatType == CATALOG_ID_USNO_B1 )
	{
		// get region from online usnbo 1
		// nCount = GetUSNOB1StarsFromNavyMil( nRa, nDec, nWidth, nHeight );
		// from vizier
		CSkyCatalogStarsUsnob rCat( this, m_pUnimapWorker );
		// first is limit to load - next if to load region ... do i need this ?
		nCount = rCat.Load( CATALOG_LOAD_REMOTE, nLoadSource, 0, 1, nRa, nDec, nWidth, nHeight );

	} else if( nCatType == CATALOG_ID_NOMAD )
	{
		CSkyCatalogStarsNomad rCat( this, m_pUnimapWorker );
		// first is limit to load - next if to load region ... do i need this ?
		nCount = rCat.Load( CATALOG_LOAD_REMOTE, nLoadSource, 0, 1, nRa, nDec, nWidth, nHeight );

	} else if( nCatType == CATALOG_ID_2MASS )
	{
		CSkyCatalogStars2Mass rCat( this, m_pUnimapWorker );
		// first is limit to load - next if to load region ... do i need this ?
		nCount = rCat.Load( CATALOG_LOAD_REMOTE, 0, 1, nRa, nDec, nWidth, nHeight );

	} else if( nCatType == CATALOG_ID_GSC )
	{
		CSkyCatalogStarsGsc rCat( this, m_pUnimapWorker );
		// first is limit to load - next if to load region ... do i need this ?
		nCount = rCat.Load( CATALOG_LOAD_REMOTE, 0, 1, nRa, nDec, nWidth, nHeight );
	}

	/////////////
	// if loaded save last region
	if( nCount > 0 ) 
	{
		m_nLastRegionLoadedCenterRa = nRa;
		m_nLastRegionLoadedCenterDec = nDec;
		m_nLastRegionLoadedWidth = nWidth;
		m_nLastRegionLoadedHeight = nHeight;
	}

	return( nCount );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadCatalog
// Class:	CSkyCatalogStars
// Purpose:	load an entire catalog
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogStars::LoadCatalog( int nCatType )
{
	unsigned long nCount=0;
	// first we unload what is currently loaded - a brutal by simpler
	// aproach - to do later the more extesive eficient loading
	if( m_nCatalogLoaded == nCatType && m_nStar > 0 ) return( m_nStar );
		
	UnloadCatalog( );

	// then we check to load catalog type
	if( nCatType == CATALOG_ID_SAO )
	{
		CSkyCatalogStarsSao rCat( this, m_pUnimapWorker );
		nCount = rCat.Load( );

	} else if( nCatType == CATALOG_ID_HIPPARCOS )
	{
		CSkyCatalogStarsHipparcos rCat( this, m_pUnimapWorker );
//		nCount = rCat.LoadAscii( "./star_catalog/hip_new.txt" );
//		SortByMagnitude( );
//		ExportBinary( );
		nCount = rCat.Load( );

	}  else if( nCatType == CATALOG_ID_TYCHO_2 )
	{
		CSkyCatalogStarsTycho rCat( this, m_pUnimapWorker );
		nCount = rCat.LoadTycho2Binary( DEFAULT_TYCHO2_BINARY_FILENAME, 0, 0 );

	} else if( nCatType == CATALOG_ID_USNO_B1 )
	{
		// get region from online usnbo 1
		return( 0 );
	} 

	return( nCount );

}

/////////////////////////////////////////////////////////////////////
// Method:	LoadBinary
// Class:	CSkyCatalogStars
// Purpose:	load the binary version of the catalog to increase speed
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
unsigned long CSkyCatalogStars::LoadBinary( const wxChar* strFile, int nLoadLimit,
								int bRegion, double nRa, double nDec,
										double nWidth, double nHeight )
{
	wxString strLog;

	// check catalog loaded
	if( m_vectStar != NULL ) UnloadCatalog();

	FILE* pFile = NULL;

	// calculate min/max
	double nMinRa = nRa-nWidth/2;
	double nMaxRa = nRa+nWidth/2;
	double nMinDec = nDec-nHeight/2;
	double nMaxDec = nDec+nHeight/2;

	// init min and max
	m_nMinX = DBL_MAX;
	m_nMinY = DBL_MAX;
	m_nMaxX = DBL_MIN;
	m_nMaxY = DBL_MIN;

	// reset star count to zero
	m_nStar = 0;

	if( m_pUnimapWorker )
	{
		strLog.Printf( wxT("INFO :: loading %s catalog ..."), m_strName );
		m_pUnimapWorker->Log( strLog );
	}

	// open file to write
	pFile = wxFopen( strFile, wxT("rb") );
	if( !pFile )
	{
		return( 0 );
	}

	// check if limit
	if( nLoadLimit > 0 )
	{
		// allocate my vector + 1 unit just to be sure
		m_vectStar = (StarDefCatBasic*) malloc( (nLoadLimit+1)*sizeof(StarDefCatBasic) );
		// set alloc
		m_nStarAllocated = nLoadLimit;

	} else
	{
		// now go at the end of the file and see the size
		fseek( pFile, 0, SEEK_END );
		// get the file size and calculate how many elements i have
		int nRecords = (int) ( ftell( pFile )/( sizeof(unsigned long)+3*sizeof(double) ));
		// allocate my vector + 1 unit just to be sure
		m_vectStar = (StarDefCatBasic*) malloc( (nRecords+1)*sizeof(StarDefCatBasic) );
		// go at the begining of the file
		fseek( pFile, 0, SEEK_SET );
		// set alloc
		m_nStarAllocated = nRecords;
	}
	// now write all stars info in binary format
	while( !feof( pFile ) )
	{
		// first reset current record
		memset( &(m_vectStar[m_nStar]), 0, sizeof(StarDefCatBasic) );

		if( !fread( (void*) &(m_vectStar[m_nStar].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectStar[m_nStar].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectStar[m_nStar].dec), sizeof(double), 1, pFile ) )
			break;
		if( !fread( (void*) &(m_vectStar[m_nStar].mag), sizeof(double), 1, pFile ) )
			break;

		if( bRegion && !(m_vectStar[m_nStar].ra <= nMaxRa &&
						m_vectStar[m_nStar].ra >= nMinRa &&
						m_vectStar[m_nStar].dec <= nMaxDec &&
						m_vectStar[m_nStar].dec >= nMinDec) )
		{
			// skip - don't add this star as is not in the region
			continue;
		}
		// set object cat type
		m_vectStar[m_nStar].cat_name = NULL;
		m_vectStar[m_nStar].cat_type = m_nObjectType;
		m_vectStar[m_nStar].type = SKY_OBJECT_TYPE_STAR;

		// calculate min and max --- ???? do i need or just delete ?!!!!
		if( m_vectStar[m_nStar].ra < m_nMinX ) m_nMinX = m_vectStar[m_nStar].ra;
		if( m_vectStar[m_nStar].ra > m_nMaxX ) m_nMaxX = m_vectStar[m_nStar].ra;

		if( m_vectStar[m_nStar].dec < m_nMinY ) m_nMinY = m_vectStar[m_nStar].dec;
		if( m_vectStar[m_nStar].dec > m_nMaxY ) m_nMaxY = m_vectStar[m_nStar].dec;

		// increment counter
		m_nStar++;

		// check if i am over the load limit
		if( nLoadLimit > 0 && m_nStar >= nLoadLimit ) break;

		// check if I need to reallocate
		if( m_nStar >= m_nStarAllocated )
		{
			// incremen counter
			m_nStarAllocated += VECT_STARS_START_MEM_ALLOC_BLOCK_SIZE;
			// reallocate
			m_vectStar = (StarDefCatBasic*) realloc( m_vectStar, (m_nStarAllocated+1)*sizeof(StarDefCatBasic) );
		}
	}

	fclose( pFile );

	return( m_nStar );
}

/////////////////////////////////////////////////////////////////////
// Method:	ExportBinary
// Class:	CSkyCatalogStars
// Purpose:	export current catalog as binary
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyCatalogStars::ExportBinary( )
{
	FILE* pFile = NULL;
	unsigned long i = 0;
	int bError=0;

	// open file to write
	pFile = wxFopen( m_strFile, wxT("wb") );
	if( !pFile )
	{
		return( 0 );
	}

	// calculate size
	long nPCount=0;
	int nPStep=0, nLineSize=0;
	long nFileSize = m_nStar*(sizeof(unsigned long)+3*sizeof(double));
	long nPCountStep = (long) nFileSize/100;

	// now write all stars info in binary format
	for( i=0; i<m_nStar; i++ )
	{
		if( !fwrite( (void*) &(m_vectStar[i].cat_no), sizeof(unsigned long), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectStar[i].ra), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectStar[i].dec), sizeof(double), 1, pFile ) )
			break;
		if( !fwrite( (void*) &(m_vectStar[i].mag), sizeof(double), 1, pFile ) )
			break;

		// add here progress
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
				nPCount += nLineSize;
		}
	}

	fclose( pFile );
	// delete file downloaded if error
	if( bError ) wxRemoveFile( m_strFile );

	return( 1 );
}
