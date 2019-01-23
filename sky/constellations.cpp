
#include "wx/wxprec.h"
#include "wx/thread.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/regex.h>
#include <wx/string.h>
#include <wx/tokenzr.h>

// system headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// local headers
#include "../util/folders.h"
#include "../util/func.h"
#include "../image/astroimage.h"
#include "../config/mainconfig.h"
#include "../unimap_worker.h"
#include "sky.h"

// main header
#include "constellations.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyConstellations
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyConstellations::CSkyConstellations( CSky* pSky )
{
	m_pSky = pSky;

	m_vectConstName = NULL;
	m_nConstName = 0;
	m_vectConstellations =  NULL;
	m_nConstellations = 0;
	m_bHasArtwork = 0;

	// set default type
	m_nCulture = -1; //SKY_CULTURE_WESTERN;
	m_nVersion = -1; //CONSTELLATION_WESTERN_HARAY;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyConstellations
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyConstellations::~CSkyConstellations()
{
	UnLoadConstNames( );
	UnloadConstellations( );
}

/////////////////////////////////////////////////////////////////////
// Method:	UnLoadConstNames
// Class:	CSkyConstellations
// Purpose:	unload constelation common names
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyConstellations::UnLoadConstNames( )
{
	int i = 0;

	if( m_vectConstName )
	{
		for( i=0; i<m_nConstName; i++ )
			free( m_vectConstName[i].name );

		free( m_vectConstName );
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadConstellations
// Class:	CStarCatalog
// Purpose:	load constellations
// Input:	file name
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyConstellations::LoadConstellations( int nCulture )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	long nTempLong;
	wxString strFile;
	wxString strLog;

	// if same as loaded and detected return
	if( m_nCulture == nCulture ) return( 0 );

	// build file name
	if( nCulture == SKY_CULTURE_WESTERN )
		strFile = unMakeAppPath(wxT("data/constellations/western/struct.ini"));
	else if( nCulture == SKY_CULTURE_CHINESE )
		strFile = unMakeAppPath(wxT("data/constellations/chinese/struct.ini" ));
	else if( nCulture == SKY_CULTURE_EGYPTIAN )
		strFile = unMakeAppPath(wxT("data/constellations/egyptian/struct.ini" ));
	else if( nCulture == SKY_CULTURE_INUIT )
		strFile = unMakeAppPath(wxT("data/constellations/inuit/struct.ini" ));
	else if( nCulture == SKY_CULTURE_KOREAN )
		strFile = unMakeAppPath(wxT("data/constellations/korean/struct.ini" ));
	else if( nCulture == SKY_CULTURE_LAKOTA )
		strFile = unMakeAppPath(wxT("data/constellations/lakota/struct.ini" ));
	else if( nCulture == SKY_CULTURE_MAORI )
		strFile = unMakeAppPath(wxT("data/constellations/maori/struct.ini" ));
	else if( nCulture == SKY_CULTURE_NAVAJO )
		strFile = unMakeAppPath(wxT("data/constellations/navajo/struct.ini" ));
	else if( nCulture == SKY_CULTURE_NORSE )
		strFile = unMakeAppPath(wxT("data/constellations/norse/struct.ini" ));
	else if( nCulture == SKY_CULTURE_POLYNESIAN )
		strFile = unMakeAppPath(wxT("data/constellations/polynesian/struct.ini" ));
	else if( nCulture == SKY_CULTURE_TUPI )
		strFile = unMakeAppPath(wxT("data/constellations/tupi/struct.ini" ));
	else
		return( -1 );

	wxRegEx reConstName( wxT("^\\[(.+)\\] *(.*)") );
	wxRegEx reConstId = wxT( "^Id=([0-9]+)" );
	wxRegEx reConstShortName = wxT( "^ShortName=(.+)" );
	wxRegEx reConstArtworkFile = wxT( "^ArtworkFile=(.+)" );
	wxRegEx reConstArtworkMap = wxT( "^ArtworkMap=(.+) (.+) \\((.+) +(.+)\\) (.+) (.+) \\((.+) +(.+)\\) (.+) (.+) \\((.+) +(.+)\\)" );
	wxRegEx reConstLines = wxT( "^LINES\\[([0-9]+)\\]\\[([0-9]+)\\]=(.+)" );
	int bFoundConstellation = 0;

	// first check if one already is and close
	UnloadConstellations( );
	// allocate my vector - TODO HERE :: extend format -- allocate dynamic
	m_vectConstellations = (ConstellationStructure*) malloc( 300*sizeof( ConstellationStructure ) );

	// debug
	m_pUnimapWorker->Log( wxT("INFO :: Loading constellations ...") );

	// Read the index
	pFile = wxFopen( strFile, wxT("r") );
	// check if there is any configuration to load
	if( !pFile )
	{
		// debug
		strLog.Printf( wxT("Warning :: could not open constellation def file %s"), strFile );
		m_pUnimapWorker->Log( strLog );
		return( -1 );
	}

	m_bHasArtwork = 0;
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );
		// set this to wx string
//		wxString strWxLine(strLine,wxConvUTF8);

		// if line empty jump
		if( wxStrlen(strLine) < 3 )
			continue;

		/////////////////////////////
		// check first for constelation label
		if( reConstName.Matches( strLine ) )
		{
			// get constellation name
			wxString strConstName = reConstName.GetMatch(strLine, 1 );
			// set constellation name
			memset( &(m_vectConstellations[m_nConstellations]), 0, sizeof(ConstellationStructure) );
			m_vectConstellations[m_nConstellations].name = (wxChar*) malloc( (strConstName.Length()+1)*sizeof(wxChar) );
			wxStrcpy( m_vectConstellations[m_nConstellations].name, strConstName );
			///////////////
			// set const point counter to zero
			m_vectConstellations[m_nConstellations].nStar = 0;
			m_vectConstellations[m_nConstellations].nLineCount[0]=0; 
			m_vectConstellations[m_nConstellations].nLineCount[1]=0; 
			m_vectConstellations[m_nConstellations].vectLine[0]=NULL;
			m_vectConstellations[m_nConstellations].vectLine[1]=NULL;
			m_vectConstellations[m_nConstellations].short_name = NULL;
			m_vectConstellations[m_nConstellations].artwork_file = NULL;
			m_vectConstellations[m_nConstellations].x1 = 0;
			m_vectConstellations[m_nConstellations].y1 = 0;
			m_vectConstellations[m_nConstellations].x2 = 0;
			m_vectConstellations[m_nConstellations].y2 = 0;
			m_vectConstellations[m_nConstellations].x3 = 0;
			m_vectConstellations[m_nConstellations].y3 = 0;
			m_vectConstellations[m_nConstellations].ra1 = 0.0;
			m_vectConstellations[m_nConstellations].dec1 = 0.0;
			m_vectConstellations[m_nConstellations].ra2 = 0.0;
			m_vectConstellations[m_nConstellations].dec2 = 0.0;
			m_vectConstellations[m_nConstellations].ra3 = 0.0;
			m_vectConstellations[m_nConstellations].dec3 = 0.0;
			// set this older to null/zero for now - todo: remove or adapt
			m_vectConstellations[m_nConstellations].vectStar = NULL;
			m_vectConstellations[m_nConstellations].nStar = 0;

			// increase constellations counter
			m_nConstellations++;
			// set found constellation flag
			bFoundConstellation = 1;

		} else if( bFoundConstellation == 1 && wxStrlen( strLine ) > 3 )
		{
			// check internal labels
			if( reConstId.Matches( strLine ) )
			{
				reConstId.GetMatch( strLine, 1 ).Trim(0).Trim(1).ToLong( &nTempLong );
				m_vectConstellations[m_nConstellations-1].id = (int) nTempLong;

			} else if( reConstShortName.Matches( strLine ) )
			{
				// get constellation short name
				wxString strConstShortName = reConstShortName.GetMatch(strLine, 1 ).Trim(0).Trim(1);
				// set constellation short name
				m_vectConstellations[m_nConstellations-1].short_name = (wxChar*) malloc( (strConstShortName.Length()+1)*sizeof(wxChar) );
				wxStrcpy( m_vectConstellations[m_nConstellations-1].short_name, strConstShortName );

			} else if( reConstArtworkFile.Matches( strLine ) )
			{
				m_bHasArtwork = 1;
				// set constellation artwork filename
				wxString strConstArtworkFile = reConstArtworkFile.GetMatch( strLine, 1 ).Trim(0).Trim(1);
				m_vectConstellations[m_nConstellations-1].artwork_file = (wxChar*) malloc( (strConstArtworkFile.Length()+1)*sizeof(wxChar) );
				bzero( m_vectConstellations[m_nConstellations-1].artwork_file, strConstArtworkFile.Length()+1 );
				wxStrcpy( m_vectConstellations[m_nConstellations-1].artwork_file, strConstArtworkFile );

			} else if( reConstArtworkMap.Matches( strLine ) )
			{
				// get x/y
				reConstArtworkMap.GetMatch( strLine, 1 ).ToLong( &(m_vectConstellations[m_nConstellations-1].x1) );
				reConstArtworkMap.GetMatch( strLine, 2 ).ToLong( &(m_vectConstellations[m_nConstellations-1].y1) );
				reConstArtworkMap.GetMatch( strLine, 5 ).ToLong( &(m_vectConstellations[m_nConstellations-1].x2) );
				reConstArtworkMap.GetMatch( strLine, 6 ).ToLong( &(m_vectConstellations[m_nConstellations-1].y2) );
				reConstArtworkMap.GetMatch( strLine, 9 ).ToLong( &(m_vectConstellations[m_nConstellations-1].x3) );
				reConstArtworkMap.GetMatch( strLine, 10 ).ToLong( &(m_vectConstellations[m_nConstellations-1].y3) );
				// get ra/dec's
				reConstArtworkMap.GetMatch( strLine, 3 ).ToDouble( &(m_vectConstellations[m_nConstellations-1].ra1) );
				reConstArtworkMap.GetMatch( strLine, 4 ).ToDouble( &(m_vectConstellations[m_nConstellations-1].dec1) );
				reConstArtworkMap.GetMatch( strLine, 7 ).ToDouble( &(m_vectConstellations[m_nConstellations-1].ra2) );
				reConstArtworkMap.GetMatch( strLine, 8 ).ToDouble( &(m_vectConstellations[m_nConstellations-1].dec2) );
				reConstArtworkMap.GetMatch( strLine, 11 ).ToDouble( &(m_vectConstellations[m_nConstellations-1].ra3) );
				reConstArtworkMap.GetMatch( strLine, 12 ).ToDouble( &(m_vectConstellations[m_nConstellations-1].dec3) );

			} else if( reConstLines.Matches( strLine ) )
			{
				// get id
				long nLinesType;
				reConstLines.GetMatch( strLine, 1 ).ToLong( &nLinesType );

				// debug
				int ii=0;
				if( nLinesType == 0 )
					ii=1;

				// get number of lines
				long nLinesNo;
				reConstLines.GetMatch( strLine, 2 ).ToLong( &nLinesNo );
	
				// allocate 
				int nLinesAllocated = nLinesNo;
				m_vectConstellations[m_nConstellations-1].vectLine[nLinesType] = (ConstellationLine*) malloc( nLinesAllocated*sizeof(ConstellationLine) );

				wxString strData = reConstLines.GetMatch( strLine, 3 ).Trim( 1 ).Trim( 0 );
				// split string
				int nLine = 0;
				wxStringTokenizer tkz( strData, wxT(":") );
				while ( tkz.HasMoreTokens() )
				{
					wxString strNextStar = tkz.GetNextToken();
					// check if emty - aka end of line
					if( strNextStar.Length() != 0 )
					{
						double nRa1, nDec1, nRa2, nDec2;
						// add line - if exist
						if( wxSscanf( strNextStar, wxT("%lf %lf %lf %lf"), &nRa1, &nDec1, &nRa2, &nDec2 ) )
						{
							// check allocation
							if( nLine >= nLinesAllocated )
							{
								nLinesAllocated++;
								m_vectConstellations[m_nConstellations-1].vectLine[nLinesType] = (ConstellationLine*) malloc( nLinesAllocated*sizeof(ConstellationLine) );
							}

							m_vectConstellations[m_nConstellations-1].vectLine[nLinesType][nLine].ra1 = nRa1;
							m_vectConstellations[m_nConstellations-1].vectLine[nLinesType][nLine].dec1 = nDec1;
							m_vectConstellations[m_nConstellations-1].vectLine[nLinesType][nLine].ra2 = nRa2;
							m_vectConstellations[m_nConstellations-1].vectLine[nLinesType][nLine].dec2 = nDec2;
							nLine++;
						} 
					}
				}
				// set no of lines
				m_vectConstellations[m_nConstellations-1].nLineCount[nLinesType] = nLine;
			}
		}
	}
	// close my file
	fclose( pFile );

	// set curent loaded
	m_nCulture = nCulture;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadConstellations
// Class:	CSkyConstellations
// Purpose:	load constellations
// Input:	file name
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyConstellations::LoadConstellationsA( const wxChar* strFile )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxString strMsg;
	unsigned long nCatNo=0;
	double nDecDeg=0, nRaDeg=0;
	int nLineSize=0;

	// regex
	//wxRegEx reConstName( "\\[([a-zA-Z0-9\\ _\\-]+)\\]" );
	wxRegEx reConstName( wxT("\\[([a-zA-Z0-9\\ _\\-]+)\\] *(.*)") );
	wxRegEx reConstArtwork = wxT( "^([a-zA-Z]{3}) (.+) (.+) (.+) \\((.+) +(.+)\\) (.+) (.+) \\((.+) +(.+)\\) (.+) (.+) \\((.+) +(.+)\\)" );
	int bFoundConstellation = 0;

	// first check if one already is and close
//	UnLoadConstellations( );
	// allocate my vector
	m_vectConstellations = (ConstellationStructure*) malloc( 100*sizeof( ConstellationStructure ) );

	// debug
	m_pUnimapWorker->Log( wxT("INFO :: Loading constellations ...") );

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

	m_bHasArtwork = 0;
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
		bzero( strLine, 2000 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );
		nLineSize = wxStrlen( strLine );

		// if line empty jump
		if( nLineSize < 3 )
			continue;

		wxString strWxLine = strLine;

		/////////////////////////////
		// check first for constelation label
		if( reConstName.Matches( strLine ) )
		{
			// get constellation name
			wxString strConstName = reConstName.GetMatch(strLine, 1 );
			// set constellation name
			m_vectConstellations[m_nConstellations].name = (wxChar*) malloc( (strConstName.Length()+1)*sizeof(wxChar) );
			wxStrcpy( m_vectConstellations[m_nConstellations].name, strConstName );

			// set const point counter to zero
			m_vectConstellations[m_nConstellations].nStar = 0;

			wxString strLabel2 = reConstName.GetMatch(strLine, 2 );
			// check for extra data in title
			if( reConstArtwork.Matches( strLabel2 ) )
			{
				// set constellation sort name
				wxString strConstShortName = reConstArtwork.GetMatch(strLabel2, 1 );
				m_vectConstellations[m_nConstellations].short_name = (wxChar*) malloc( (strConstShortName.Length()+1)*sizeof(wxChar) );
				wxStrcpy( m_vectConstellations[m_nConstellations].short_name, strConstShortName );

				// set constellation artwork filename
				wxString strConstArtworkFile = reConstArtwork.GetMatch(strLabel2, 2 );
				m_vectConstellations[m_nConstellations].artwork_file = (wxChar*) malloc( (strConstArtworkFile.Length()+1)*sizeof(wxChar) );
				wxStrcpy( m_vectConstellations[m_nConstellations].artwork_file, strConstArtworkFile );

				// get x/y
				reConstArtwork.GetMatch(strLabel2, 3 ).ToLong( &(m_vectConstellations[m_nConstellations].x1) );
				reConstArtwork.GetMatch(strLabel2, 4 ).ToLong( &(m_vectConstellations[m_nConstellations].y1) );
				reConstArtwork.GetMatch(strLabel2, 7 ).ToLong( &(m_vectConstellations[m_nConstellations].x2) );
				reConstArtwork.GetMatch(strLabel2, 8 ).ToLong( &(m_vectConstellations[m_nConstellations].y2) );
				reConstArtwork.GetMatch(strLabel2, 11 ).ToLong( &(m_vectConstellations[m_nConstellations].x3) );
				reConstArtwork.GetMatch(strLabel2, 12 ).ToLong( &(m_vectConstellations[m_nConstellations].y3) );
				// get ra/dec's
				reConstArtwork.GetMatch(strLabel2, 5 ).ToDouble( &(m_vectConstellations[m_nConstellations].ra1) );
				reConstArtwork.GetMatch(strLabel2, 6 ).ToDouble( &(m_vectConstellations[m_nConstellations].dec1) );
				reConstArtwork.GetMatch(strLabel2, 9 ).ToDouble( &(m_vectConstellations[m_nConstellations].ra2) );
				reConstArtwork.GetMatch(strLabel2, 10 ).ToDouble( &(m_vectConstellations[m_nConstellations].dec2) );
				reConstArtwork.GetMatch(strLabel2, 13 ).ToDouble( &(m_vectConstellations[m_nConstellations].ra3) );
				reConstArtwork.GetMatch(strLabel2, 14 ).ToDouble( &(m_vectConstellations[m_nConstellations].dec3) );

			} else
			{
				m_vectConstellations[m_nConstellations].short_name = NULL;
				m_vectConstellations[m_nConstellations].artwork_file = NULL;
				m_vectConstellations[m_nConstellations].x1 = 0;
				m_vectConstellations[m_nConstellations].y1 = 0;
				m_vectConstellations[m_nConstellations].x2 = 0;
				m_vectConstellations[m_nConstellations].y2 = 0;
				m_vectConstellations[m_nConstellations].x3 = 0;
				m_vectConstellations[m_nConstellations].y3 = 0;
				m_vectConstellations[m_nConstellations].ra1 = 0.0;
				m_vectConstellations[m_nConstellations].dec1 = 0.0;
				m_vectConstellations[m_nConstellations].ra2 = 0.0;
				m_vectConstellations[m_nConstellations].dec2 = 0.0;
				m_vectConstellations[m_nConstellations].ra3 = 0.0;
				m_vectConstellations[m_nConstellations].dec3 = 0.0;

			}

			// increase constellations counter
			m_nConstellations++;
			// set found constellation flag
			bFoundConstellation = 1;

		} else if( bFoundConstellation == 1 && wxStrlen( strLine ) > 20 && (strLine[0] == ' ' || strLine[0] == '+') )
		{
			unsigned int nConstStar = m_vectConstellations[m_nConstellations-1].nStar;

			if( nConstStar == 0 )
				m_vectConstellations[m_nConstellations-1].vectStar = (ConstellationStar*) malloc( (nConstStar+1)*sizeof(ConstellationStar) );
			else
				m_vectConstellations[m_nConstellations-1].vectStar = (ConstellationStar*) realloc( m_vectConstellations[m_nConstellations-1].vectStar, (nConstStar+1)*sizeof(ConstellationStar) );

			// extract start flag
			if( strLine[0] == ' ' )
				m_vectConstellations[m_nConstellations-1].vectStar[nConstStar].nRefPoint = 0;
			else if( strLine[0] == '+' )
				m_vectConstellations[m_nConstellations-1].vectStar[nConstStar].nRefPoint = 1;

			// extract Ra 1-11
			wxString strRa = strWxLine.Mid( 1, 11 );
			// extract Dec 13-9
			wxString strDec = strWxLine.Mid( 13, 9 );

			// convert ra in deg
			if( !ConvertRaToDeg( strRa, nRaDeg ) )
				continue;

			// convert dec to deg
			if( !ConvertDecToDeg( strDec, nDecDeg ) )
				continue;

			// set ra/dec
			m_vectConstellations[m_nConstellations-1].vectStar[nConstStar].ra = nRaDeg;
			m_vectConstellations[m_nConstellations-1].vectStar[nConstStar].dec = nDecDeg;

			// extract name
			wxString strStarCode = strWxLine.Mid( 23, nLineSize-24 );
			m_vectConstellations[m_nConstellations-1].vectStar[nConstStar].name = (wxChar*) malloc( (strStarCode.Length() +1)*sizeof(wxChar) );
			wxStrcpy( m_vectConstellations[m_nConstellations-1].vectStar[nConstStar].name, strStarCode );

			// increment current counter
			m_vectConstellations[m_nConstellations-1].nStar++;
		}

	}
	// close my file
	fclose( pFile );

/* TODO :: do I need this here ????

	////////////////////////////////////////////
	// TEMP :: save const in my new format
	int i, j;
	pFile = wxFopen( unMakeAppPath(wxT("data/constellations/western/new_cnstl.txt")), wxT("w") );
	
	for( i=0; i<m_nConstellations; i++ )
	{
		wxFprintf( pFile, wxT("[%s]\n"), m_vectConstellations[i].name );
		fprintf( pFile, "Id=%d\n", i );
		if( m_vectConstellations[i].short_name != NULL && 
			strlen(m_vectConstellations[i].short_name) > 0 ) wxFprintf( pFile, wxT("ShortName=%s\n"), m_vectConstellations[i].short_name );
		if( m_vectConstellations[i].artwork_file != NULL && 
			strlen(m_vectConstellations[i].artwork_file) > 0 ) 
		{
			wxFprintf( pFile, wxT("ArtworkFile=%s\n"), m_vectConstellations[i].artwork_file );
			wxFprintf( pFile, wxT("ArtworkMap=%d %d (%.7f %.7f) %d %d (%.7f %.7f) %d %d (%.7f %.7f)\n"),
						// point 1
						m_vectConstellations[i].x1, m_vectConstellations[i].y1,
						m_vectConstellations[i].ra1, m_vectConstellations[i].dec1,
						// point 2
						m_vectConstellations[i].x2, m_vectConstellations[i].y2,
						m_vectConstellations[i].ra2, m_vectConstellations[i].dec2,
						// point 3
						m_vectConstellations[i].x3, m_vectConstellations[i].y3,
						m_vectConstellations[i].ra3, m_vectConstellations[i].dec3 );
		}

		
		wxChar strTmp[2000];
		wxStrcpy( strTmp, wxT("") );
		// now add constelation lines
		double nLastRa=0.0, nLastDec=0.0;
		int cnt=0;
		for( j=0; j<m_vectConstellations[i].nStar; j++ )
		{
			if( m_vectConstellations[i].vectStar[j].nRefPoint == 0 && j > 0 )
			{
				wxSprintf( strTmp, wxT("%s%.8f %.8f %.8f %.8f"), strTmp, nLastRa, nLastDec, 
							m_vectConstellations[i].vectStar[j].ra, m_vectConstellations[i].vectStar[j].dec );

				cnt++;
				if( cnt < m_vectConstellations[i].nStar-1 ) wxSprintf( strTmp, wxT("%s:"), strTmp );
			}
			nLastRa = m_vectConstellations[i].vectStar[j].ra;
			nLastDec = m_vectConstellations[i].vectStar[j].dec;
		}
		wxFprintf( pFile, wxT("LINES[1][%d]=%s\n"), cnt, strTmp );

		fprintf( pFile, "\n" );
	}
	fclose( pFile );
*/

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	UnloadConstellations
// Class:	CSkyConstellations
// Purpose:	unload constelations
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyConstellations::UnloadConstellations( )
{
	int i = 0, j = 0;

	if( m_vectConstellations )
	{
		for( i=0; i<m_nConstellations; i++ )
		{
			// free names
			if( m_vectConstellations[i].name )
			{
				free( m_vectConstellations[i].name );
				m_vectConstellations[i].name = NULL;
			}

			// free short names
			if( m_vectConstellations[i].short_name )
			{
				free( m_vectConstellations[i].short_name );
				m_vectConstellations[i].short_name = NULL;
			}

			// free art work
			if( m_vectConstellations[i].artwork_file )
			{
				free( m_vectConstellations[i].artwork_file );
				m_vectConstellations[i].artwork_file = NULL;
			}

			// for every const point name free
			for( j=0; j<m_vectConstellations[i].nStar; j++ )
			{
				free( m_vectConstellations[i].vectStar[j].name );
				m_vectConstellations[i].vectStar[j].name = NULL;
			}
			if( m_vectConstellations[i].vectStar != NULL ) free( m_vectConstellations[i].vectStar );
			m_vectConstellations[i].vectStar = NULL;
			m_vectConstellations[i].nStar = 0;

			// free lines
			if( m_vectConstellations[i].vectLine[0] ) free( m_vectConstellations[i].vectLine[0] );
			if( m_vectConstellations[i].vectLine[1] ) free( m_vectConstellations[i].vectLine[1] );
			m_vectConstellations[i].vectLine[0] = NULL;
			m_vectConstellations[i].vectLine[1] = NULL;
		}

		free( m_vectConstellations );
		m_nConstellations = 0;
	}


	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	GetConstellationNoById
// Class:	CSkyConstellations
// Purpose:	get constelation order no by id
// Input:	pointer to my astro image
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CSkyConstellations::GetConstellationNoById( int nId )
{
	int nNo = -1;

	for( int i=0; i<m_nConstellations; i++ )
	{
		if( m_vectConstellations[i].id == nId )
		{
			nNo = i;
			break;
		}
	}

	return( nNo );
}

/////////////////////////////////////////////////////////////////////
// Method:	GetConstlInImage
// Class:	CSkyConstellations
// Purpose:	find constellations in image by culture version 
// Input:	pointer to my astro image, culture, version and return pointer
// Output:	vector of constl, and no of constlelations found
/////////////////////////////////////////////////////////////////////
long CSkyConstellations::GetConstlInImage( CAstroImage* pAstroImage, 
										  int nCulture, int nVersion, 
										  int* vectCnstl, int& nCnstl )
{
	unsigned long i=0, j=0;
	StarDef star;

	// if same as loaded and detected return
//	if( m_nCulture == nCulture && m_nVersion == nVersion ) return( 0 );

	// try to load
	if( LoadConstellations( nCulture ) < 0 ) return( -1 );

	// reset input vars
	nCnstl = 0;

	// calculate dist from start to center
	double nSkyZoneRadius = CalcSkyDistance( pAstroImage->m_nRaStart, pAstroImage->m_nDecStart, 
											pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );

	//////////////////////
	// for each constelation check if any point in the image
	for( i=0; i<m_nConstellations; i++ )
	{
		ConstellationLine* vectLines = m_vectConstellations[i].vectLine[nVersion];
		// check if this is not the use the other or skip
		if( !vectLines ) continue;

		///////////////////////////////
		// and for each point - aka star
		for( j=0; j<m_vectConstellations[i].nLineCount[nVersion]; j++ )
		{
			// WRONG !!!! DEPROJECT - to be right
			// calc dist for this point
			double dist_a = CalcSkyDistance( pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec, vectLines[j].ra1, vectLines[j].dec1 );
			double dist_b = CalcSkyDistance( pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec, vectLines[j].ra2, vectLines[j].dec2 );
			if( dist_a <= nSkyZoneRadius && dist_b <= nSkyZoneRadius )
			{
				memset( &star, 0, sizeof(StarDef) );
				// point A
				star.ra = vectLines[j].ra1;
				star.dec = vectLines[j].dec1;

				// project star on tangent plan using the found orig
				//m_pSky->ProjectStar( star, m_pSky->m_nWinOrigX, m_pSky->m_nWinOrigY );
				m_pSky->ProjectStar( star, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );
				// larry :: my damn multiplication factor
				star.x = star.x*pAstroImage->m_nSkyProjectionScale; 
				star.y = star.y*pAstroImage->m_nSkyProjectionScale; 
				// and now apply inverse trans
				m_pSky->TransObj_SkyToImage( star );

				// check if object is in my image
				if( star.x >= 0 && star.y >= 0 &&
					star.x <= pAstroImage->m_nWidth &&
					star.y <= pAstroImage->m_nHeight )
				{

					// and now add this to input vector of catalog object in the image
					vectCnstl[nCnstl] = i;
					nCnstl++;
					// stop here
					break;
				}

				// point B
				star.ra = vectLines[j].ra2;
				star.dec = vectLines[j].dec2;

				// project star on tangent plan using the found orig
				//m_pSky->ProjectStar( star, m_pSky->m_nWinOrigX, m_pSky->m_nWinOrigY );
				m_pSky->ProjectStar( star, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );
				// larry :: my damn multiplication factor
				star.x = star.x*pAstroImage->m_nSkyProjectionScale; //10;
				star.y = star.y*pAstroImage->m_nSkyProjectionScale; //10;
				// and now apply inverse trans
				m_pSky->TransObj_SkyToImage( star );

				// check if object is in my image
				if( star.x >= 0 && star.y >= 0 &&
					star.x <= pAstroImage->m_nWidth &&
					star.y <= pAstroImage->m_nHeight )
				{

					// and now add this to the input vector 
					vectCnstl[nCnstl] = i;
					nCnstl++;
					// stop here
					break;

				}
			}
		}

	}

	// set version
	m_nVersion = nVersion;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	FindConstellation
// Class:	CSkyConstellations
// Purpose:	find if there is any constellations in the image
// Input:	pointer to my astro image
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyConstellations::FindConstellation( CAstroImage* pAstroImage )
{
//	wxString strLog;
	unsigned long i=0, j=0;
	StarDef star;
	// set flags
	int nConstId = -1;

	// get default in image
	int vectConstl[300];
	int nConstl=0;
	int nCulture = SKY_CULTURE_WESTERN;
	int nVersion = CONSTELLATION_WESTERN_IAU;
	// get what is in image
	GetConstlInImage( pAstroImage, nCulture, nVersion, vectConstl, nConstl );

	//////////////////////
	// for each found constelation 
	for( i=0; i<nConstl; i++ )
	{
		int nId = vectConstl[i];
		// and now add this to vector of catalog object in the image
		nConstId = pAstroImage->AddConstellation( m_vectConstellations[nId].name, nId );

		ConstellationLine* vectLines = m_vectConstellations[nId].vectLine[nVersion];
		// check if this is not the use the other or skip
		if( !vectLines ) continue;

		// NEW TEMP :: to remove someday and just keep ids
		// .... add constelation lines
		for( j=0; j<m_vectConstellations[nId].nLineCount[nVersion]; j++ )
		{
			memset( &star, 0, sizeof(StarDef) );
			// point a
			star.ra = vectLines[j].ra1;
			star.dec = vectLines[j].dec1;

			// project star on tangent plan using the found orig - todo: what m_nWinOrigX ? can i just get that from image instead  
			// use pAstroImage->m_nOrigRa?
			//m_pSky->ProjectStar( star, m_pSky->m_nWinOrigX, m_pSky->m_nWinOrigY );
			m_pSky->ProjectStar( star, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );
			// larry :: my damn multiplication factor - use image on
//			star.x = star.x*m_pSky->m_nProjectionScale; 
//			star.y = star.y*m_pSky->m_nProjectionScale; 
			star.x = star.x*pAstroImage->m_nSkyProjectionScale;
			star.y = star.y*pAstroImage->m_nSkyProjectionScale; 

			// and now apply inverse trans
			m_pSky->TransObj_SkyToImage( star );
			double x1=star.x;
			double y1=star.y;

			// point b
			star.ra = vectLines[j].ra2;
			star.dec = vectLines[j].dec2;

			// project star on tangent plan using the found orig
			// old crap
			//m_pSky->ProjectStar( star, m_pSky->m_nWinOrigX, m_pSky->m_nWinOrigY );
			// larry :: my damn multiplication factor
			//star.x = star.x*m_pSky->m_nProjectionScale; 
			//star.y = star.y*m_pSky->m_nProjectionScale; 

			m_pSky->ProjectStar( star, pAstroImage->m_nOrigRa, pAstroImage->m_nOrigDec );
			star.x = star.x*pAstroImage->m_nSkyProjectionScale;
			star.y = star.y*pAstroImage->m_nSkyProjectionScale; 

			// and now apply inverse trans
			m_pSky->TransObj_SkyToImage( star );
			double x2=star.x;
			double y2=star.y;

			pAstroImage->AddConstellationLine( nConstId, x1, y1, x2, y2 );
		}
		// set constellation score
		pAstroImage->SetConstellationScore( nConstId );
	}
	// now at last sort constellations
	pAstroImage->SortConstellations();

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadConstNames
// Class:	CSkyConstellations
// Purpose:	load constelation common names
// Input:	file name
// Output:	nothing
/////////////////////////////////////////////////////////////////////
long CSkyConstellations::LoadConstNames( const wxChar* strFile )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxString strMsg;
	unsigned long nCatNo=0;
	double nDecDeg=0, nRaDeg=0;
	int nLineSize=0;

	// first check if one already is and close
	UnLoadConstNames( );
	// allocate my vector
	m_vectConstName = (ConstCommonName*) malloc( 100*sizeof( ConstCommonName ) );

	// debug
	m_pUnimapWorker->Log( wxT("INFO :: Loading constellations names ...") );

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
		bzero( strLine, 2000 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pFile );
		nLineSize = wxStrlen(strLine);


		// if line empty jump
		if( nLineSize < 3 )
			continue;

		wxString strWxLine = strLine;

		/////////////////////////////
		// here we extract field we need

		// extract Ra 48-57
		wxString strRa = strWxLine.Mid( 0, 9 );
		// extract Dec 59-68
		wxString strDec = strWxLine.Mid( 10, 9 );

		// convert ra in deg
		if( !ConvertRaToDeg( strRa, nRaDeg ) )
			continue;

		// convert dec to deg
		if( !ConvertDecToDeg( strDec, nDecDeg ) )
			continue;

		// set ra/dec
		m_vectConstName[m_nConstName].ra = nRaDeg;
		m_vectConstName[m_nConstName].dec = nDecDeg;

		// DSO common name
		wxString strConstName = strWxLine.Mid( 27, 20 ).Trim(0).Trim(1);

		// set dso name
		m_vectConstName[m_nConstName].name = (wxChar*) malloc( (strConstName.Length()+1)*sizeof(wxChar) );
		wxStrcpy( m_vectConstName[m_nConstName].name, strConstName );

		// increase dso common name counter
		m_nConstName++;
	}
	// close my file
	fclose( pFile );

	return( 1 );
}
