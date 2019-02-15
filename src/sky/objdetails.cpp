////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// wx includes
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
//precompiled headers
#include "wx/wxprec.h"
// other wx
#include <wx/file.h>
#include <wx/string.h>
#include <wx/filesys.h>
#include <wx/html/htmlpars.h>
#include <wx/fs_inet.h>
#include <wx/regex.h>
#include <wx/uri.h>
#include <wx/tokenzr.h>

// local headers
#include "../util/func.h"
#include "../unimap_worker.h"
#include "../image/astroimage.h"

// main header
#include "objdetails.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyObjDetails
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyObjDetails::CSkyObjDetails( CAstroImage* pAstroImage, CUnimapWorker* pUnimapWorker )
{
	m_pAstroImage = pAstroImage;
	m_pUnimapWorker = pUnimapWorker;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyObjDetails
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyObjDetails::~CSkyObjDetails( )
{
	m_pAstroImage = NULL;
	m_pUnimapWorker = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:	LoadDsoDistances
// Class:	CSkyObjDetails
// Purpose:	Get basic details for an object list from simbad
// Input:	input object list, output details list 
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CSkyObjDetails::LoadDsoDistances( StarDef* vectObj, unsigned long nObj,
										StarBasicDetailsDef* vectObjDetails )
{
	//wxString strFile;
	wxChar strLine[2000];
	FILE* pFile = NULL;
	unsigned long nCatNo = 0;
	double nDistance = 0;
	long nDsoId = -1;

	m_pAstroImage->m_bIsChanged = 1;

	m_pUnimapWorker->SetWaitDialogMsg( wxT("Load dso data file ...") );

	///////////////////////////
	// LOAD :: NGC
	pFile = wxFopen( FILE_DSO_DETAILS_DISTANCE_NGC, wxT("r") );
	if( !pFile ) return( 0 );
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );
		// if line less then 10 chars jump
		if( wxStrlen( strLine ) < 10 ) continue;
		// copy in wxstring
		wxString strWxLine = strLine;

		// get ngc code
		if( !strWxLine.Mid( 0, 7 ).Trim(0).Trim(1).ToULong( &nCatNo ) ) continue;
		// get dos id if any
		nDsoId = m_pAstroImage->GetDsoObjByCatNo( nCatNo, CAT_OBJECT_TYPE_NGC );
		// if dso exis
		if( nDsoId >= 0 )
		{
			// extract distance
			if( strWxLine.Mid( 7, 20 ).Trim(0).Trim(1).ToDouble( &nDistance ) )
				vectObj[nDsoId].distance = nDistance;
			else
				vectObj[nDsoId].distance = 0;
		}
	}
	fclose( pFile );

	///////////////////////////
	// LOAD :: IC
	pFile = wxFopen( FILE_DSO_DETAILS_DISTANCE_IC, wxT("r") );
	if( !pFile ) return( 0 );
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );
		// if line less then 10 chars jump
		if( wxStrlen( strLine ) < 10 ) continue;
		// copy in wxstring
		wxString strWxLine = strLine;

		// get ic code
		if( !strWxLine.Mid( 0, 8 ).Trim(0).Trim(1).ToULong( &nCatNo ) ) continue;
		// get dos id if any
		nDsoId = m_pAstroImage->GetDsoObjByCatNo( nCatNo, CAT_OBJECT_TYPE_IC );
		// if dso exis
		if( nDsoId >= 0 )
		{
			// extract distance
			if( strWxLine.Mid( 8, 20 ).Trim(0).Trim(1).ToDouble( &nDistance ) )
				vectObj[nDsoId].distance = nDistance;
			else
				vectObj[nDsoId].distance = 0;
		}
	}
	fclose( pFile );

	///////////////////////////
	// LOAD :: MESSIER
	pFile = wxFopen( FILE_DSO_DETAILS_DISTANCE_MESSIER, wxT("r") );
	if( !pFile ) return( 0 );
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );
		// if line less then 10 chars jump
		if( wxStrlen( strLine ) < 10 ) continue;
		// copy in wxstring
		wxString strWxLine = strLine;

		// get ic code
		if( !strWxLine.Mid( 0, 9 ).Trim(0).Trim(1).ToULong( &nCatNo ) ) continue;
		// get dos id if any
		nDsoId = m_pAstroImage->GetDsoObjByCatNo( nCatNo, CAT_OBJECT_TYPE_MESSIER );
		// if dso exis
		if( nDsoId >= 0 )
		{
			// extract distance
			if( strWxLine.Mid( 9, 20 ).Trim(0).Trim(1).ToDouble( &nDistance ) )
				vectObj[nDsoId].distance = nDistance;
			else
				vectObj[nDsoId].distance = 0;
		}
	}
	fclose( pFile );

	return( 1 );
}
