////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// wx
#include "wx/wxprec.h"
#include <wx/file.h>
#include <wx/string.h>
#include <wx/filesys.h>
#include <wx/html/htmlpars.h>
#include <wx/fs_inet.h>
#include <wx/regex.h>
#include <wx/uri.h>
#include <wx/tokenzr.h>

// local headers
#include "../util/folders.h"
#include "../util/func.h"
#include "../unimap_worker.h"
#include "../util/webfunc.h"
#include "../spectrum/spectrum.h"

// main header
#include "ned_site.h"

////////////////////////////////////////////////////////////////////
CNedSite::CNedSite( CSkyObjSpectrum* pSkyObjSpectrum, CUnimapWorker* pUnimapWorker ) 
{
	m_pUnimapWorker = pUnimapWorker;
	m_pSkyObjSpectrum = pSkyObjSpectrum;
}

////////////////////////////////////////////////////////////////////
CNedSite::~CNedSite( ) 
{
}

////////////////////////////////////////////////////////////////////
// Method:	GetSpectrum
// Class:	CNedSite
// Purpose:	Get spectrum data from ned
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CNedSite::GetSpectrum( wxString& strObjectCatName ) 
{
	m_strObjectCatName = strObjectCatName;

	wxString strFile;
	wxString strSpecIniFile;
	wxString strUrlGetData;

	// set spectrum ini file - just to check if exists
	strSpecIniFile.Printf( wxT("%s%s%s%s"), OBJECT_SPECTRUM_DATA_PATH, wxT("ned_"), m_strObjectCatName, wxT(".ini") );
	// setlocal temp file
	strFile.Printf( wxT("%s%s%s%s%s"), TEMP_ONLINE_OBJECT_DATA_PATH, wxT("spectrum/"), wxT("ned_"), m_strObjectCatName, wxT(".txt") );

	wxString strWxIniFile = strSpecIniFile;
	if( ::wxFileExists( strWxIniFile ) ==  0 )
	{

		// NED
		// http://nedwww.ipac.caltech.edu/cgi-bin/NEDspectra?objname=NGC891&extend=always&detail=1&preview=1&numpp=20&refcode=ANY&bandpass=ANY&line=ANY
		// NED :: best 
		// http://nedwww.ipac.caltech.edu/cgi-bin/NEDspectra?objname=m81&extend=never&detail=1&preview=0&numpp=5000&refcode=ANY&bandpass=ANY&line=ANY

		m_pUnimapWorker->SetWaitDialogMsg( wxT("Look for spectrum data at NED ...") );

		// set URL to get ned list of data
		strUrlGetData.Printf( wxT("http://nedwww.ipac.caltech.edu/cgi-bin/NEDspectra?objname=%s&extend=multi&detail=0&preview=0&numpp=5000&refcode=ANY&bandpass=ANY&line=ANY"),
									m_strObjectCatName );

		// load webfile ... perhaps I should use LoadWebFileT...
		LoadWebFile( strUrlGetData, strFile );

		m_pUnimapWorker->SetWaitDialogMsg( wxT("Process spectrum list file from NED ...") );
		// now load local file
		ProcessSpectrumListFileFromNed( strFile );

	}

	// load spectrum ini file 
	if( m_pSkyObjSpectrum->LoadSpectrumIniFile( strSpecIniFile ) > 0 )
		m_pSkyObjSpectrum->m_bHasSpectrum = 1;
	else
		m_pSkyObjSpectrum->m_bHasSpectrum = 0;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	GetSpectrumFileFromNed
// Class:	CNedSite
// Purpose:	Get spectrum file from ned
// Input:	remote file path, local file
// Output:	status
////////////////////////////////////////////////////////////////////
int CNedSite::GetSpectrumFileFromNed( const wxString& strRemoteFile, 
										 const wxString& strLocalFile,
										 wxString& strXLabel,
										 wxString& strYLabel ) 
{
	wxFSFile* pFile = NULL;
	wxInputStream* pStream = NULL;
	FILE* pLocalFile = NULL;
	wxString strSpecLocalPathFile;
	wxString strUrlGetData;
	wxChar strLine[2000];
	// header regex
	wxRegEx reAxisInfo( wxT( "^#\\ *(\\[[^\\]]+\\])\\ +(\\[[^\\]]+\\])" ), wxRE_ADVANCED  );

	// set spectrum ini file - just to check if exists
	strSpecLocalPathFile.Printf( wxT("%s%s"), OBJECT_SPECTRUM_DATA_PATH, strLocalFile );
	// check if exists
	wxString strWxFile = strSpecLocalPathFile;
	if( ::wxFileExists( strWxFile ) ==  0 )
	{
		m_pUnimapWorker->SetWaitDialogMsg( wxT("Get spectrum data file from NED ...") );

		// set URL to get ned list of data
		strUrlGetData.Printf( wxT("http://nedwww.ipac.caltech.edu%s"), strRemoteFile );

		////////////////////////////
		// here we initialize the reading process
		wxFileSystem *pUrl = new wxFileSystem();
		pUrl->AddHandler( new wxInternetFSHandler );

		// then open data
		pFile = pUrl->OpenFile( strUrlGetData );

		if( !pFile || pFile == NULL ) return( 0 );
		
		pStream = pFile->GetStream( );

		// open local file to write
		pLocalFile = wxFopen( strSpecLocalPathFile, wxT("w") );
		if( !pLocalFile )
		{
			return( 0 );
		}

		// read the entire bufer
		do
		{
			unsigned char nChar = pStream->GetC();
			if( pStream->LastRead() ) fputc( nChar, pLocalFile );  

		} while( pStream->LastRead() );

		// close file for writing
		fclose( pLocalFile );

		delete( pFile );
		delete( pUrl );

	}

	// init axis labels
	strXLabel = wxT("Spectral-axis ");
	strYLabel = wxT("Intensity ");
	/////////////////////
	// process file header to extract axis
	pLocalFile = wxFopen( strSpecLocalPathFile, wxT("r") );
	if( !pLocalFile ) return( 0 );
	int nExitCount = 0;
	// Reading lines from  file
	while( !feof( pLocalFile ) )
	{
		if( nExitCount > 5 ) break;
		// clear buffers
		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );
		// copy in wxstring
		wxString strWxLine = strLine;
		strWxLine.Trim(1).Trim(0);

		// check for my pattern
		if( reAxisInfo.Matches( strWxLine ) )
		{
			wxString strFieldA = reAxisInfo.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 );
			wxString strFieldB = reAxisInfo.GetMatch( strWxLine, 2 ).Trim( 1 ).Trim( 0 );
			// se my new labels
			strXLabel = strXLabel + strFieldA;
			strYLabel = strYLabel + strFieldB;
			// and exit
			break;
		}
		// increment exist count - to read just 5 lines
		nExitCount++;
	}
	// close file
	fclose( pLocalFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	ProcessSpectrumListFileFromNed
// Class:	CNedSite
// Purpose:	process spectrum list file from ned
// Input:	file name
// Output:	statsus
/////////////////////////////////////////////////////////////////////
int CNedSite::ProcessSpectrumListFileFromNed( const wxString& strFile )
{
	FILE* pLocalFile = NULL;
	FILE* pSpecIniFile = NULL;
	wxString strSpecLocalFile;
	wxString strSpecIniFile;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[2000];
	// sections regex - ^[\\ ]+([^\\ ]+)\\ ([^\\ ]+)?\\ \\ 
	wxRegEx reSpectrumFile( wxT( "\\<a\\ href\\=\"(.+)\"\\>NED\\-ASCII\\<\\/a\\>" ), wxRE_ADVANCED  );
	wxRegEx reSpectrumFileFields( wxT( "\\:(.+)\\:(.+)\\:(.+)_NED\\.txt" ), wxRE_ADVANCED  );
	wxRegEx reRegion( wxT( "\\>Region\\:\\<\\/b\\>\\<\\/font\\>\\<\\/td\\>\\ *\\<td\\ align\\=left\\ nowrap\\>(.+)\\<\\/td\\>" ), wxRE_ADVANCED  );
	wxRegEx reTelescope( wxT( "\\>Telescope\\:\\<\\/b\\>\\<\\/font\\>\\<\\/td\\>\\ *\\<td\\ align\\=left\\ nowrap\\>(.+)\\<\\/td\\>" ), wxRE_ADVANCED  );
	wxRegEx reInstrument( wxT( "\\>Instrument\\:\\<\\/b\\>\\<\\/font\\>\\<\\/td\\>\\ *\\<td\\ align\\=left\\ nowrap\\>(.+)\\<\\/td\\>" ), wxRE_ADVANCED  );
	wxRegEx reBand( wxT( "\\>Band\\:\\<\\/b\\>\\<\\/font\\>\\<\\/td\\>\\<td\\ align\\=left\\ nowrap\\>(.+)\\<\\/td\\>" ), wxRE_ADVANCED  );

	// other variables used
	wxChar strTmp[1000];

	/////////////////////////////////////////
	// TO SAVE CURRENT SPECTRUM 
	// make my spec ini filename
	strSpecIniFile.Printf( wxT("%s%s%s%s"), OBJECT_SPECTRUM_DATA_PATH, wxT("ned_"), m_strObjectCatName, wxT(".ini") );
	// open local file to read from
	pSpecIniFile = wxFopen( strSpecIniFile, wxT("w") );
	if( !pSpecIniFile ) return( 0 );
	/////////////////////////////////////
	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}
	// Reading lines from cfg file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );
		// copy in wxstring
		wxString strWxLine = strLine;
		strWxLine.Trim( 1 ).Trim( 0 );
		//strWxLine = "  " + strWxLine + "  ";

		/////////////////////////////
		// check for spectrum file pattern
		if( reSpectrumFile.Matches( strWxLine ) )
		{
			// extract spectrum file name
			wxString strSpecRemoteFile = reSpectrumFile.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 );

			///////////////////////////////////
			// extract fields from spectrum file
			if( reSpectrumFileFields.Matches( strWxLine ) )
			{
				wxString strFieldA = reSpectrumFileFields.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 );
				wxString strFieldB = reSpectrumFileFields.GetMatch( strWxLine, 2 ).Trim( 1 ).Trim( 0 );
				wxString strFieldC = reSpectrumFileFields.GetMatch( strWxLine, 3 ).Trim( 1 ).Trim( 0 );
				// make local file with full path
				strSpecLocalFile.Printf( wxT("%s%s%s%s%s%s%s%s%s"), wxT("ned_"), m_strObjectCatName, 
								wxT("_"), strFieldA, wxT("_"), strFieldB, wxT("_"), strFieldC, wxT(".txt") );
				// also make menu list label
				//sprintf( m_vectSpectrum[m_nSpectrum].strLabel, "%s:%s:%s", strFieldA, strFieldB, strFieldC );
				wxFprintf( pSpecIniFile, wxT("\n[%s:%s:%s]\n"), strFieldA, strFieldB, strFieldC );

			} else
			{
				// not formated name ... just skip
				continue;
			}

			//////////////////////////////
			// now load spectrum 
			wxString strLabelX = wxT("");
			wxString strLabelY = wxT("");
			GetSpectrumFileFromNed( strSpecRemoteFile, strSpecLocalFile, strLabelX, strLabelY );
			// set labels
			//strcpy( m_vectSpectrum[m_nSpectrum].strXLabel, strLabelX );
			//strcpy( m_vectSpectrum[m_nSpectrum].strYLabel, strLabelY );
			wxFprintf( pSpecIniFile, wxT("LabelX=%s\n"), strLabelX );
			wxFprintf( pSpecIniFile, wxT("LabelY=%s\n"), strLabelY );
			
			///////////////////
			// set my ref vector :: file
			//strcpy( m_vectSpectrum[m_nSpectrum].strFileName, strSpecLocalFile );
			wxFprintf( pSpecIniFile, wxT("File=%s\n"), strSpecLocalFile );

		} else
		// Region:
		if( reRegion.Matches( strWxLine ) )
		{
			wxString strRegion = reRegion.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 );
			// set my ref vector
			//strcpy( m_vectSpectrum[nCurrentSpectrum].strRegion, strRegion ); 
			wxFprintf( pSpecIniFile, wxT("Region=%s\n"), strRegion );

		} else
		// Telescope:
		if( reTelescope.Matches( strWxLine ) )
		{
			wxString strTelescope = reTelescope.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 );
			// set my ref vector
			//strcpy( m_vectSpectrum[nCurrentSpectrum].strTelescope, strTelescope ); 
			wxFprintf( pSpecIniFile, wxT("Telescope=%s\n"), strTelescope );

		} else
		// Instrument:
		if( reInstrument.Matches( strWxLine ) )
		{
			wxString strInstrument = reInstrument.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 );
			// set my ref vector
			//strcpy( m_vectSpectrum[nCurrentSpectrum].strInstrument, strInstrument ); 
			wxFprintf( pSpecIniFile, wxT("Instrument=%s\n"), strInstrument );

		} else
		// Band:
		if( reBand.Matches( strWxLine ) )
		{
			wxString strBand = reBand.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 );
			// set my ref vector
			//strcpy( m_vectSpectrum[nCurrentSpectrum].strBand, strBand ); 
			wxFprintf( pSpecIniFile, wxT("Band=%s\n"), strBand );
		} 

	}
	// close my file
	fclose( pLocalFile );

	// close my spec ini file
	fclose( pSpecIniFile );

	// if no spectrum just exist
	if( m_pSkyObjSpectrum->m_nSpectrum <= 0 ) return( 0 );

	return( m_pSkyObjSpectrum->m_nSpectrum );
}
