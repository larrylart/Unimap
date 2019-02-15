////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/image.h>

////////////////////
// eLynx
#include <elx/core/CoreMacros.h>
#include <elx/math/MathCore.h>
#include <elx/math/TransfertFunction.h>
//#include <elx/image/ImageVariant.h>
#include <elx/image/AbstractImageManager.h>
#include <elx/image/ImageFileManager.h>
#include <elx/image/IImageFilePlugin.h>
#include <elx/image/PixelIterator.h>

// local headers
#include "../util/folders.h"
#include "../util/func.h"
#include "../util/webfunc.h"
#include "../util/image_func.h"
#include "../unimap_worker.h"

// main header
#include "earth.h"

using namespace eLynx;
using namespace eLynx::Math;
using namespace eLynx::Image;

////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////
CLocationEarth::CLocationEarth( CUnimapWorker* pUnimapWorker )
{
	m_pUnimapWorker = pUnimapWorker;
	m_nCountry=0;

	// load main definition
	LoadCountries( );
}

////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////
CLocationEarth::~CLocationEarth( ) 
{
	FreeData();
}

////////////////////////////////////////////////////////////////////
void CLocationEarth::FreeData( ) 
{
	int i=0, j=0, k=0;

	for( i=0; i<m_nCountry; i++ )
	{
//		free( m_vectCountry[i].name );
		// for every region
		for( j=0; j<m_vectCountry[i].nRegion; j++ )
		{
//			free( m_vectCountry[i].vectRegion[j].name );
			// for every city
//			for( k=0; k<m_vectCountry[i].vectRegion[j].nCity; k++ )
//			{
//				free( m_vectCountry[i].vectRegion[j].vectCity[k].name );
//			}

			free( m_vectCountry[i].vectRegion[j].vectCity );

//			delete[] m_vectCountry[i].vectRegion[j].vectCity;
		}
		free( m_vectCountry[i].vectRegion );
		//delete[] m_vectCountry[i].vectRegion;
	}

	m_nCountry = 0;
}

////////////////////////////////////////////////////////////////////
// Purpose:	load countries 
////////////////////////////////////////////////////////////////////
int CLocationEarth::LoadCountries( )
{
	FILE* pFile = NULL;
	char strLine[2000];
	char strCode[10];
	char strName[255];
	int nSize=0;
	int nRegions=0;

	// free all
	FreeData();

	pFile = wxFopen( unMakeAppPath(wxT("data/earth/countries.txt")), wxT("rb") );
	if( !pFile ){ return( 0 ); }

	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
		memset( &strLine, 0, 255 );
		// get one line out of the config file
		fgets( strLine, 255, pFile );
		nSize = strlen(strLine);
		if( nSize < 6 ) continue;
	
		/////////////////////////////////////
		// check for country code label
		if( sscanf( strLine, "%[^=]=%[^\|]\|%d", strCode, strName, &nRegions ) )
		{
			// reset oother to zero/null
			memset( &(m_vectCountry[m_nCountry]), 0, sizeof(locCountry) );

			wxStrcpy( m_vectCountry[m_nCountry].name, wxString(strName,wxConvUTF8) );
			// set country code
			m_vectCountry[m_nCountry].code[0] = (char) strCode[0];
			m_vectCountry[m_nCountry].code[1] = (char) strCode[1];

			// allocate memory
			m_vectCountry[m_nCountry].vectRegion = (locRegion*) malloc( nRegions*sizeof(locRegion) );
			//m_vectCountry[nCountry].vectRegion = (locRegion*) new locRegion[nRegions+1]();
//			m_vectCountry[m_nCountry].nRegion = 0;

			m_nCountry++;
		}
	}
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CLocationEarth::LoadCountry( int nId )
{
	FILE* pFile = NULL;
	wxString strFileName;
	char strLine[2000];
	int nSize=0;

//	m_nCountry = 0;

	/////////////////////////////////////////////////
	// CITIES :: open file to read
	char strCity[255];
	char strTmp[500];
	char strRegion[255];
	char strTypeCode[255];
	char strPopulation[255];
	char strLon[255];
	char strLat[255];
	char strAlt[255];
	char strLightPolution[255];
	int nCities=0, nRegion=0;
	int bFoundCountry=0, bFoundRegion=0;

	locCountry* pCountry  = &(m_vectCountry[nId]);
	locRegion* pRegion= NULL;
	locCity* pCity;

	strFileName.Printf( wxT("%s/%hs.txt"), unMakeAppPath(wxT("data/earth/countries")), m_vectCountry[nId].code );
	pFile = wxFopen( strFileName, wxT("rb") );
	if( !pFile ){ return( 0 ); }

	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
		memset( &strLine, 0, 255 );
		// get one line out of the config file
		fgets( strLine, 255, pFile );
		nSize = strlen(strLine);
		if( nSize < 6 ) continue;
	
		/////////////////////////////////////
		// check for REGION code label
		if( strLine[0] == '{' )
		{
			nRegion = m_vectCountry[nId].nRegion;
			pRegion = &(pCountry->vectRegion[nRegion]);

			if( sscanf( strLine, "\{%[^\}]\}\{%d\}", strRegion, &nCities ) )
			{
				// set name
				wxStrcpy( pRegion->name, wxString(strRegion,wxConvUTF8) );

				// allocate memory
				pRegion->vectCity = (locCity*) malloc( nCities*sizeof(locCity) );
				//m_vectCountry[nCountry].vectRegion[nRegion].vectCity = new locCity[nCities+1]();
				pRegion->nCity = 0;

				pCountry->nRegion++;
				bFoundRegion = 1;

				continue;
			}
		}

		if( !bFoundRegion ) continue;

		// extract city, country code, etc
		if( sscanf( strLine, "%[^=]=%[^,],%[^,],%[^,]", strCity, strLat, strLon, strAlt ) )
		{

			pCity = &(pRegion->vectCity[pRegion->nCity]);

			// set name
			strcpy( pCity->name, strCity );

			//////////////////
			// set latitude
			pCity->lat = atof(strLat); 
			pCity->lon = atof(strLon); 
			pCity->alt = atof(strAlt); 

			// increment cities counter
			pRegion->nCity++;
		}
	}
	fclose( pFile );
}

////////////////////////////////////////////////////////////////////
int CLocationEarth::LoadAllCountries( )
{
	int i=0;

	// if main definition not loaded load
	if( m_nCountry <=0 ) LoadCountries( );

	for( i=0; i<m_nCountry; i++ )
	{
		if( m_vectCountry[i].nRegion <= 0 ) 
			LoadCountry( i );
		else 
			continue;
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CLocationEarth::FindCountryId( const char* strCode )
{
	int i;

	for( i=0; i<m_nCountry; i++ )
	{
		if( m_vectCountry[i].code[0] == strCode[0] && m_vectCountry[i].code[1] == strCode[1] ) return( i );
	}

	return( -1 );
}

////////////////////////////////////////////////////////////////////
int CLocationEarth::FindCountryByName( const wxString& strName )
{
	int i;

	for( i=0; i<m_nCountry; i++ )
	{
		if( strName.CmpNoCase( m_vectCountry[i].name ) == 0 ) return( i );
	}

	return( -1 );
}

////////////////////////////////////////////////////////////////////
int CLocationEarth::FindRegionByName( int nCountryId, const wxString& strName )
{
	int i;

	//////////////////
	// first check if to load country
	if( m_vectCountry[nCountryId].nRegion <= 0 ) LoadCountry( nCountryId );

	for( i=0; i<m_vectCountry[nCountryId].nRegion; i++ )
	{
		if( strName.CmpNoCase( m_vectCountry[nCountryId].vectRegion[i].name ) == 0 ) return( i );
	}

	return( -1 );
}

////////////////////////////////////////////////////////////////////
int CLocationEarth::FindCityByName( int nCountryId, int nRegionId, const wxString& strName )
{
	int i;

	//////////////////
	// first check if to load country
	if( m_vectCountry[nCountryId].nRegion <= 0 ) LoadCountry( nCountryId );

	for( i=0; i<m_vectCountry[nCountryId].vectRegion[nRegionId].nCity; i++ )
	{
		wxString l_name(m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[i].name,wxConvUTF8);
		if( strName.CmpNoCase( l_name ) == 0 ) return( i );
	}

	return( -1 );
}

////////////////////////////////////////////////////////////////////
int CLocationEarth::FindRegionCityByName( const wxString& strName, int nCountryId, 
											int& nRegionId, int& nCityId )
{
	int i, j;

	//////////////////
	// first check if to load country
	if( m_vectCountry[nCountryId].nRegion <= 0 ) LoadCountry( nCountryId );

	for( i=0; i<m_vectCountry[nCountryId].nRegion; i++ )
	{
		for( j=0; j<m_vectCountry[nCountryId].vectRegion[i].nCity; j++ )
		{
			wxString l_name(m_vectCountry[nCountryId].vectRegion[i].vectCity[j].name,wxConvUTF8);
			if( strName.CmpNoCase( l_name ) == 0 )
			{
				nRegionId = i;
				nCityId = j;
				return( 1 );
			}
		}
	}
	// else return not found
	return( 0 );
}

////////////////////////////////////////////////////////////////////
int CLocationEarth::FindClosestCity( double lat, double lon, double max, int& country, int& region, int& city )
{
	int bFound = 0;
	int i=0, j=0, k=0;
	double nDist=0.0, nMinDist=DBL_MAX;

	for( i=0; i<m_nCountry; i++ )
	{
		for( j=0; j<m_vectCountry[i].nRegion; j++ )
		{
			for( k=0; k<m_vectCountry[i].vectRegion[j].nCity; k++ )
			{
				// use sky distance - similar
				nDist = CalcSkyDistance( lat, lon, m_vectCountry[i].vectRegion[j].vectCity[k].lat, 
													m_vectCountry[i].vectRegion[j].vectCity[k].lon );
				// look for smallest
				if( nDist < nMinDist )
				{
					nMinDist = nDist;
					country = i;
					region = j;
					city = k;
					bFound = 1;
				}
			}
		}
	}
	
	if( bFound && country >=0 && region >=0 && city >=0 && nMinDist <= max )
		return( 1 );
	else
		return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:	SetCountriesWidget
// Class:	CLocationEarth
// Purpose:	set countries gui widget
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
void CLocationEarth::SetCountriesWidget( wxChoice* pWidget, int nSelect )
{
	int i = 0;

	pWidget->Freeze( );
	pWidget->Clear();
	for( i=0; i<m_nCountry; i++ )
	{
		//if( m_vectCountry[i].nRegion != 0 && m_vectCountry[i].vectRegion != NULL )
		pWidget->Append( m_vectCountry[i].name );
	}
	pWidget->Thaw( );
	// select first
	pWidget->SetSelection( nSelect );
}

////////////////////////////////////////////////////////////////////
// Purpose:	set regions for current country
////////////////////////////////////////////////////////////////////
void CLocationEarth::SetRegionsWidget( wxChoice* pWidget, int nCountryId, int nSelect )
{
	int i = 0;

	//////////////////
	// first check if to load country
	if( m_vectCountry[nCountryId].nRegion <= 0 ) LoadCountry( nCountryId );

	pWidget->Freeze( );
	pWidget->Clear();
	for( i=0; i<m_vectCountry[nCountryId].nRegion; i++ )
	{
		pWidget->Append( m_vectCountry[nCountryId].vectRegion[i].name );
	}
	pWidget->Thaw( );
	// select first
	pWidget->SetSelection( nSelect );
}

////////////////////////////////////////////////////////////////////
// Purpose:	set cities for current country/region
////////////////////////////////////////////////////////////////////
void CLocationEarth::SetCitiesWidget( wxChoice* pWidget, int nCountryId, int nRegionId, int nSelect )
{
	int i = 0;

	//////////////////
	// first check if to load country
	if( m_vectCountry[nCountryId].nRegion <= 0 ) LoadCountry( nCountryId );

	pWidget->Freeze( );
	pWidget->Clear();
	for( i=0; i<m_vectCountry[nCountryId].vectRegion[nRegionId].nCity; i++ )
	{
		// pWidget->Append( m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[i].name ); 
		pWidget->Append( wxString(m_vectCountry[nCountryId].vectRegion[nRegionId].vectCity[i].name, wxConvUTF8) );
	}
	pWidget->Thaw( );
	// select first
	pWidget->SetSelection( nSelect );
}

////////////////////////////////////////////////////////////////////
// Method:	GetLightPollutionLevel
////////////////////////////////////////////////////////////////////
double CLocationEarth::GetLightPollutionLevel( )
{
	double nLevel=0.0;
	int i=0, j=0;

	////////////////////
	// look to fecth image from online location(unimap site now) if not exist locally
	if( !wxFileExists( EARTH_LIGTH_POLLUTION_MAP_FILE ) && m_pUnimapWorker )
	{
		int nWebFileSize = 1036544;
		// todo : i should probably move this in class unimap site
		int nCount = LoadWebFileT( wxT("http://unimap.larryo.org/data/earth/light_pollution/earth_lp_map.gif"), 
								EARTH_LIGTH_POLLUTION_MAP_FILE, m_pUnimapWorker, nWebFileSize, 1 );

	} else if( !m_pUnimapWorker )
		return( 0 );

	////////////////////
	// load light pollution map image
	wxImage imgLP;
	imgLP.LoadFile( EARTH_LIGTH_POLLUTION_MAP_FILE, wxBITMAP_TYPE_ANY );

	// get light pollution level
	nLevel = GetLPLevelFromImage( &imgLP, m_nCurrentLat, m_nCurrentLon );

	// unload/destroy my image
	imgLP.Destroy();

	m_nCurrentLPLevel = nLevel;
	return( nLevel );
}

////////////////////////////////////////////////////////////////////
double CLocationEarth::GetLPLevelFromImage( wxImage* pImg, double lat, double lon )
{
	if( !pImg ) return( -1.0 );

	double nLevel=0.0;

	double nImgW = (double) pImg->GetWidth();
	double nImgH = (double) pImg->GetHeight();

/*	ImageVariant rImage;
	ProgressNotifier iNotifier;
    ImageFileInfo Info;
    int nStatus = (bool) the_ImageFileManager.Import( rImage, EARTH_LIGTH_POLLUTION_MAP_FILE, &Info, iNotifier );
	if( !nStatus ) elxThrow( elxErrOperationFailed, elxMsgFormat("Unable to load image %s.", EARTH_LIGTH_POLLUTION_MAP_FILE ) );
	boost::shared_ptr<ImageRGBub> spImageRGBub = _ELX_CreateRGBub( rImage, 0 );

	double nImgW = (double) spImageRGBub->GetWidth(); //imgLP.GetWidth();
	double nImgH = (double) spImageRGBub->GetHeight(); //imgLP.GetHeight();
*/

	// convert lat/lon to pixel x/y
	double nX = (double) ((lon+180.)/360.*nImgW);
	double nY = (double) ((lat-90.)/-180.*nImgH);
	int x = (int) round(nX);
	int y = (int) round(nY);
	// corretion 5355-4462 = 893, 1076-1030=46
//	x -= 893;
//	y -= 46;

	// extract area of interest for now square 3/3 ??
//	for( i=x-1; i<=x+1; i++ )
//	{
//		for( j=y-1; j<=y+1; j++ )
//		{
			unsigned char red = pImg->GetRed( x, y );
			unsigned char green = pImg->GetGreen( x, y );
			unsigned char blue = pImg->GetBlue( x, y );
			// calculate lp index
			if( red < 20 && green < 20 && blue < 20 )			// black
				nLevel = 0.0;
			else if( red < 60 && green < 60 && blue < 60 )		// gray
				nLevel = 1.0;
			else if( red < 20 && green < 20 && blue > 200 )		// blue
				nLevel = 2.0;
			else if( red < 20 && green > 200 && blue < 20 )		// green
				nLevel = 3.0;
			else if( red > 200 && green > 200 && blue < 20 )	// yellow
				nLevel = 4.0;
			else if( red > 200 && green > 120 && blue < 20 )	// orange
				nLevel = 5.0;
			else if( red > 200 && green < 120 && blue < 20 )	// red
				nLevel = 6.0;
			else if( red > 200 && green > 200 && blue > 200 )	// white
				nLevel = 7.0;

//		}
//	}

/*	PixelRGBub* pixel = spImageRGBub->GetPixel( x, y );
	unsigned char red = pixel->GetRed();
	unsigned char green = pixel->GetGreen();
	unsigned char blue = pixel->GetBlue();
*/
	//////////////
	// codes
	// GREY=41,41,41(56max) - BLUE=0,0,255 - GREEN = 0,255,0 - YELLOW=255,255,0 - ORANGE=255,138,0 - RED=255,0,0 - WHITE=0,0,0

	return( nLevel );
}

////////////////////////////////////////////////////////////////////
void CLocationEarth::GetLPLevelAsString( double level, wxString& strLevel )
{
	// set selection by level
	if( level >= 0.0 && level < 1.0 )		// none
		strLevel = wxT( "none" );
	else if( level >= 1.0 && level < 2.0 )	// light
		strLevel = wxT("light" );
	else if( level >= 2.0 && level < 5.0 )	// medium
		strLevel = wxT("medium" );
	else if( level >= 5.0 && level <= 7.0 )	// heavy
		strLevel = wxT("heavy" );
}
