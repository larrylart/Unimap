////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// wx includes
#include "wx/wxprec.h"
#include <wx/regex.h>
#include <wx/string.h>
#include <wx/tokenzr.h>

// local includes
#include "../util/func.h"

// main header
#include "observatories.h"

// Constructor/Destructor
////////////////////////////////////////////////////////////////////
CObservatories::CObservatories( )
{
	m_vectObservatory.clear();
	// load observatories
	LoadPro( );
}

////////////////////////////////////////////////////////////////////
CObservatories::~CObservatories( ) 
{
	m_vectObservatory.clear();
}

////////////////////////////////////////////////////////////////////
int CObservatories::LoadPro( ) 
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxChar strTmp[2000];
	int nLineSize=0, nVarInt=0, bObsFound=0, nObsId=0;
	long nVarLong=0;

	m_vectObservatory.clear();

	// open local file to read from
	pFile = wxFopen( OBSERVATORIES_PRO_DEF_FILE, wxT("r") );
	if( !pFile ) return( 0 );

	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );
		nLineSize = wxStrlen( strLine );

		//  if less then 3 skip
		if( nLineSize < 3 || strLine[0] == '#' ) continue; 

		///////////////
		// LOOK FOR SITE
		if( strLine[0] == '[' && strLine[nLineSize-2] == ']' )
		{
			wxChar* src = strLine;
			src++;
			wxStrncpy( strTmp, src, nLineSize-3 );
			strTmp[nLineSize-3] = '\0';
			// push element to my vector
			nObsId = m_vectObservatory.size();
			DefObservatory elem;
			elem.name = strTmp;
			elem.code = wxT("");
			elem.web = wxT("");
			elem.ini_file = wxT("");
			elem.has_details = 0;
			m_vectObservatory.push_back( elem );
			bObsFound = 1;

		} else if( bObsFound )
		{
			if( wxStrncmp( strLine, wxT("Code="), 5 ) == 0 )
			{
				wxSscanf( strLine, wxT("Code=%[^\n]\n"), strTmp );
				m_vectObservatory[nObsId].code = strTmp;

			} else if( wxStrncmp( strLine, wxT("Lat="), 4 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Lat=%lf\n"), &(m_vectObservatory[nObsId].lat) ) )
					m_vectObservatory[nObsId].lat = 0;

			} else if( wxStrncmp( strLine, wxT("Lon="), 4 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Lon=%lf\n"), &(m_vectObservatory[nObsId].lon) ) )
					m_vectObservatory[nObsId].lon = 0;

			} else if( wxStrncmp( strLine, wxT("Alt="), 4 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Alt=%lf\n"), &(m_vectObservatory[nObsId].alt) ) )
					m_vectObservatory[nObsId].alt = 0;

			} else if( wxStrncmp( strLine, wxT("Web="), 4 ) == 0 )
			{
				wxSscanf( strLine, wxT("Web=%[^\n]\n"), strTmp );
				m_vectObservatory[nObsId].web = strTmp;

			} else if( wxStrncmp( strLine, wxT("Ini="), 4 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("Ini=%[^\n]\n"), strTmp ) )
					m_vectObservatory[nObsId].ini_file = strTmp;
				else
					continue;

				// if ini file load
				if( !m_vectObservatory[nObsId].ini_file.IsEmpty() )
				{
					if( LoadDetails( nObsId ) ) m_vectObservatory[nObsId].has_details = 1;
				}
			}
		}
	}
	// close my file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CObservatories::LoadDetails( int nObsId ) 
{
	FILE* pFile = NULL;
	wxString strFile;
	wxChar strLine[2000];
	wxChar strTmp[2000];
	int nLineSize=0, nVarInt=0, bSectionId=0, nWebRefId=-1, nPhotoId=-1,
		bPhotoDesc=0, nInstrId=-1, bObsDesc=0;
	long nVarLong=0;

	// clear vectors
	m_vectObservatory[nObsId].vectWebRef.clear();
	m_vectObservatory[nObsId].vectPhoto.clear();
	m_vectObservatory[nObsId].vectInstr.clear();

	// open local file to read from
	strFile.Printf( wxT("./%s"), m_vectObservatory[nObsId].ini_file );
	pFile = wxFopen( strFile, wxT("r") );
	if( !pFile ) return( 0 );

	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );
		nLineSize = wxStrlen( strLine );

		//  if less then 3 skip
		if( nLineSize < 3 || strLine[0] == '#' ) continue; 

		///////////////
		// LOOK FOR SECTIONS
		if( wxStrncmp( strLine, wxT("[Info]"), 6 ) == 0 )
		{
			bSectionId = OBS_LIST_SECTION_INFO;
			bObsDesc=0;

		} else if( wxStrncmp( strLine, wxT("[Contact]"), 9 ) == 0 )
		{
			bSectionId = OBS_LIST_SECTION_CONTACT;

		} else if( wxStrncmp( strLine, wxT("[WebRef]"), 8 ) == 0 )
		{
			bSectionId = OBS_LIST_SECTION_WEBREF;
			DefObsWefRef elem;
			m_vectObservatory[nObsId].vectWebRef.push_back( elem );
			nWebRefId++;

		} else if( wxStrncmp( strLine, wxT("[Photo]"), 7 ) == 0 )
		{
			bSectionId = OBS_LIST_SECTION_PHOTO;
			bPhotoDesc=0;
			DefObsPhoto elem;
			m_vectObservatory[nObsId].vectPhoto.push_back( elem );
			nPhotoId++;

		} else if( wxStrncmp( strLine, wxT("[Telescope]"), 11 ) == 0 )
		{
			bSectionId = OBS_LIST_SECTION_TELESCOPE;
			DefObsInstrument elem;
			m_vectObservatory[nObsId].vectInstr.push_back( elem );
			nInstrId++;

		///////////////////////
		// SECTION :: INFO
		} else if( bSectionId == OBS_LIST_SECTION_INFO )
		{
			if( wxStrncmp( strLine, wxT("Organization="), 13 ) == 0 )
			{
				wxSscanf( strLine, wxT("Organization=%[^\n]\n"), strTmp );
				m_vectObservatory[nObsId].organization = strTmp;

			} else if( wxStrncmp( strLine, wxT("Built="), 6 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("Built=%[^\n]\n"), strTmp ) )
					m_vectObservatory[nObsId].built = strTmp;

			} else if( wxStrncmp( strLine, wxT("Weather="), 8 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("Weather=%[^\n]\n"), strTmp ) )
					m_vectObservatory[nObsId].weather = strTmp;

			// :: description
			} else if( wxStrncmp( strLine, wxT("<DESC_START>"), 12 ) == 0 )
			{
				bObsDesc=1;
				m_vectObservatory[nObsId].desc = wxT("");

			} else if( wxStrncmp( strLine, wxT("<DESC_END>"), 10 ) == 0 )
			{
				bObsDesc=0;

			} else if( bObsDesc == 1 )
			{
				m_vectObservatory[nObsId].desc += strLine;
			}


		// SECTION :: CONTACT
		} else if( bSectionId == OBS_LIST_SECTION_CONTACT )
		{
			if( wxStrncmp( strLine, wxT("Email="), 6 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("Email=%[^\n]\n"), strTmp ) )
					m_vectObservatory[nObsId].email = strTmp;

			} else if( wxStrncmp( strLine, wxT("Phone="), 6 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("Phone=%[^\n]\n"), strTmp ) )
					m_vectObservatory[nObsId].phone = strTmp;

			} else if( wxStrncmp( strLine, wxT("Fax="), 6 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("Fax=%[^\n]\n"), strTmp ) )
					m_vectObservatory[nObsId].fax = strTmp;

			}

		// SECTION :: WEBREF
		} else if( bSectionId == OBS_LIST_SECTION_WEBREF )
		{
			if( wxStrncmp( strLine, wxT("Name="), 5 ) == 0 )
			{
				wxSscanf( strLine, wxT("Name=%[^\n]\n"), strTmp );
				m_vectObservatory[nObsId].vectWebRef[nWebRefId].name = strTmp;

			} else if( wxStrncmp( strLine, wxT("Link="), 5 ) == 0 )
			{
				wxSscanf( strLine, wxT("Link=%[^\n]\n"), strTmp );
				m_vectObservatory[nObsId].vectWebRef[nWebRefId].link = strTmp;
			}


		// SECTION :: PHOTO
		} else if( bSectionId == OBS_LIST_SECTION_PHOTO )
		{
			if( wxStrncmp( strLine, wxT("File="), 5 ) == 0 )
			{
				wxSscanf( strLine, wxT("File=%[^\n]\n"), strTmp );
				m_vectObservatory[nObsId].vectPhoto[nPhotoId].file = strTmp;

			} else if( wxStrncmp( strLine, wxT("<TXT_START>"), 11 ) == 0 )
			{
				bPhotoDesc=1;
				m_vectObservatory[nObsId].vectPhoto[nPhotoId].desc = wxT("");

			} else if( wxStrncmp( strLine, wxT("<TXT_END>"), 9 ) == 0 )
			{
				m_vectObservatory[nObsId].vectPhoto[nPhotoId].desc.Trim(0).Trim(1);
				bPhotoDesc=0;

			} else if( bPhotoDesc == 1 )
			{
				m_vectObservatory[nObsId].vectPhoto[nPhotoId].desc += strLine;
			}

		// SECTION :: TELESCOPE
		} else if( bSectionId == OBS_LIST_SECTION_TELESCOPE )
		{
			if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				wxSscanf( strLine, wxT("Type=%[^\n]\n"), strTmp );
				if( wxStrcmp( strTmp, wxT("Optical") ) == 0 )
					m_vectObservatory[nObsId].vectInstr[nInstrId].type = 0;
				else if( wxStrcmp( strTmp, wxT("Radio") ) == 0 )
					m_vectObservatory[nObsId].vectInstr[nInstrId].type = 1;

			} else if( wxStrncmp( strLine, wxT("Name="), 5 ) == 0 )
			{
				wxSscanf( strLine, wxT("Name=%[^\n]\n"), strTmp );
				m_vectObservatory[nObsId].vectInstr[nInstrId].name = strTmp;

			} else if( wxStrncmp( strLine, wxT("Design="), 7 ) == 0 )
			{
				wxSscanf( strLine, wxT("Design=%[^\n]\n"), strTmp );
				m_vectObservatory[nObsId].vectInstr[nInstrId].design = strTmp;
			}

		}

	}
	// close my file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
/*int CObservatories::SavePro( ) 
{
	FILE* pFile = NULL;
	int i=0;

	// open local file to write
	pFile = fopen( OBSERVER_CONFIG_HARDWARE_FILE, "w" );
	if( !pFile ) return( 0 );

	/////////////////////////////
	// Save pro sites ??
	for( i=0; i<m_nObs...; i++ )
	{
	}

	// close my file
	fclose( pFile );

	return( 1 );
}
*/