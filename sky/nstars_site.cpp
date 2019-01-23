
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
#include "../util/webfunc.h"
#include "../unimap_worker.h"
#include "../spectrum/spectrum.h"

// main header
#include "nstars_site.h"

////////////////////////////////////////////////////////////////////
CNStarsSite::CNStarsSite( CSkyObjSpectrum* pSkyObjSpectrum, CUnimapWorker* pUnimapWorker ) 
{
	m_pUnimapWorker = pUnimapWorker;
	m_pSkyObjSpectrum = pSkyObjSpectrum;
}

////////////////////////////////////////////////////////////////////
CNStarsSite::~CNStarsSite( ) 
{
}

////////////////////////////////////////////////////////////////////
// Method:	GetSpectrum
// Class:	CNStarsSite
// Purpose:	Get spectrum data from NStars, Appalachian State University,
//			Dark Sky Observatory 
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CNStarsSite::GetSpectrum( wxString& strObjectCatName ) 
{
	m_strObjectCatName = strObjectCatName;

	wxString strFile;
	wxString strSpecIniFile;
	wxString strUrlGetData;

	// set spectrum ini file - just to check if exists
	strSpecIniFile.Printf( wxT("%s%s%s%s"), OBJECT_SPECTRUM_DATA_PATH, wxT("nstars_"), m_strObjectCatName, wxT(".ini") );
	// setlocal temp file
	strFile.Printf( wxT("%s%s%s%s%s"), TEMP_ONLINE_OBJECT_DATA_PATH, wxT("spectrum/"), wxT("nstars_"), m_strObjectCatName, wxT(".txt") );

	wxString strWxIniFile = strSpecIniFile;
	if( ::wxFileExists( strWxIniFile ) ==  0 )
	{

		// NStars :: Appalachian State University - Dark Sky Observatory 
		// http://stellar.phys.appstate.edu/cgi-bin/spectra/specdisplay.pl?starname=Alioth

		m_pUnimapWorker->SetWaitDialogMsg( wxT("Look for spectrum data at NStars ...") );

		// set URL to get ned list of data
		strUrlGetData.Printf( wxT("http://stellar.phys.appstate.edu/cgi-bin/spectra/specdisplay.pl?starname=%s"),
									m_strObjectCatName );

		// load webfile ... perhaps I should use LoadWebFileT...
		LoadWebFile( strUrlGetData, strFile );

		m_pUnimapWorker->SetWaitDialogMsg( wxT("Process spectrum list file from NStars ...") );
		// now load local file
		ProcessSpectrumListFileFromNStars( strFile );

	}

	// load spectrum ini file 
	if( m_pSkyObjSpectrum->LoadSpectrumIniFile( strSpecIniFile ) > 0 )
	{
		m_pSkyObjSpectrum->m_bHasSpectrum = 1;
	} else
	{
		m_pSkyObjSpectrum->m_bHasSpectrum = 0;
	}

	return( 1 );
}


////////////////////////////////////////////////////////////////////
// Method:	GetSpectrumFileFromNStars
// Class:	CNStarsSite
// Purpose:	Get spectrum file from NStars
// Input:	remote file path, local file
// Output:	status
////////////////////////////////////////////////////////////////////
int CNStarsSite::GetSpectrumFileFromNStars( const wxString& strRemoteFile, 
										 const wxString& strLocalFile,
										 wxString& strObject, 
										 wxString& strObservatory, 
										 wxString& strInstrument ) 
{
	wxFSFile* pFile = NULL;
	wxInputStream* pStream = NULL;
	FILE* pLocalFile = NULL;
	wxString strSpecLocalPathFile;
	wxString strUrlGetData;
	wxChar strLine[2000];
	// header regex
	wxRegEx reObservatory( wxT( "^#OBSERVATORY\\ +(.+)" ), wxRE_ADVANCED  );
	wxRegEx reInstrument( wxT( "^#INSTRUMENT\\ +(.+)" ), wxRE_ADVANCED  );
	wxRegEx reObject( wxT( "^#OBJECT\\ +(.+)" ), wxRE_ADVANCED  );

	// set spectrum ini file - just to check if exists
	strSpecLocalPathFile.Printf( wxT("%s%s"), OBJECT_SPECTRUM_DATA_PATH, strLocalFile );
	// check if exists
	wxString strWxFile = strSpecLocalPathFile;
	if( ::wxFileExists( strWxFile ) ==  0 )
	{
		m_pUnimapWorker->SetWaitDialogMsg( wxT("Get spectrum data file from NStars ...") );

		// set URL to get ned list of data
		strUrlGetData.Printf( wxT("http://stellar.phys.appstate.edu/SPECTRA%s"), strRemoteFile );

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

	strObject = wxT("");
	strObservatory = wxT("");
	strInstrument = wxT("");
	/////////////////////
	// process file header to extract axis
	pLocalFile = wxFopen( strSpecLocalPathFile, wxT("r") );
	if( !pLocalFile ) return( 0 );
	// Reading lines from  file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );
		// copy in wxstring
		wxString strWxLine = strLine;
		strWxLine.Trim(1).Trim(0);

		// if not # skip
		if( strLine[0] != '#' ) continue;

		// check for my pattern
		if( reObservatory.Matches( strWxLine ) )
		{
			strObservatory = reObservatory.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 );

		} else if( reInstrument.Matches( strWxLine ) )
		{
			strInstrument = reInstrument.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 );

		} else if( reObject.Matches( strWxLine ) )
		{
			strObject = reObject.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 );
		} 
	}
	// close file
	fclose( pLocalFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	ProcessSpectrumListFileFromNStars
// Class:	CNStarsSite
// Purpose:	process spectrum list file from NStars
// Input:	file name
// Output:	statsus
/////////////////////////////////////////////////////////////////////
int CNStarsSite::ProcessSpectrumListFileFromNStars( const wxString& strFile )
{
	FILE* pLocalFile = NULL;
	FILE* pSpecIniFile = NULL;
	wxString strSpecLocalFile;
	wxString strSpecIniFile;

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[2000];
	// sections regex - ^[\\ ]+([^\\ ]+)\\ ([^\\ ]+)?\\ \\ 
	wxRegEx reSpectrumFile( wxT( "file\\=([^\"]+)" ), wxRE_ADVANCED  );
	wxRegEx reSpectrumFileFields( wxT( "\\/(.+)\\/(.+)\\/(.+)\\." ), wxRE_ADVANCED  );

	// other variables used
	wxChar strTmp[1000];

	/////////////////////////////////////////
	// TO SAVE CURRENT SPECTRUM 
	// make my spec ini filename
	strSpecIniFile.Printf( wxT("%s%s%s%s"), OBJECT_SPECTRUM_DATA_PATH, wxT("nstars_"), m_strObjectCatName, wxT(".ini") );
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
		strWxLine.Trim(1).Trim(0);
		//strWxLine = "  " + strWxLine + "  ";

		/////////////////////////////
		// check for spectrum file pattern
		while( reSpectrumFile.Matches( strWxLine ) )
		{
			// extract spectrum file name
			wxString strSpecRemoteFile = reSpectrumFile.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 );

			///////////////////////////////////
			// extract fields from spectrum file
			if( reSpectrumFileFields.Matches( strSpecRemoteFile ) )
			{
				wxString strFieldA = reSpectrumFileFields.GetMatch( strSpecRemoteFile, 1 ).Trim( 1 ).Trim( 0 );
				wxString strFieldB = reSpectrumFileFields.GetMatch( strSpecRemoteFile, 2 ).Trim( 1 ).Trim( 0 );
				wxString strFieldC = reSpectrumFileFields.GetMatch( strSpecRemoteFile, 3 ).Trim( 1 ).Trim( 0 );
				strFieldA.Replace( wxT("/"), wxT(""), 1 );
				strFieldB.Replace( wxT("/"), wxT(""), 1 );
				strFieldC.Replace( wxT("/"), wxT(""), 1 );
				// make local file with full path
				strSpecLocalFile.Printf( wxT("%s%s%s%s%s%s%s%s%s"), wxT("nstars_"), m_strObjectCatName, 
								wxT("_"), strFieldA, wxT("_"), strFieldB, wxT("_"), strFieldC, wxT(".txt"));
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
			wxString strLabelX = wxT("Spectral-axis");
			wxString strLabelY = wxT("Intensity");
			wxString strObject = wxT("");
			wxString strObservatory = wxT("");
			wxString strInstrument = wxT("");
			GetSpectrumFileFromNStars( strSpecRemoteFile, strSpecLocalFile,
										strObject, strObservatory, strInstrument );
			// set labels
			wxFprintf( pSpecIniFile, wxT("LabelX=%s\n"), strLabelX );
			wxFprintf( pSpecIniFile, wxT("LabelY=%s\n"), strLabelY );
			// other fields
			wxFprintf( pSpecIniFile, wxT("Region=%s\n"), strObject );
			wxFprintf( pSpecIniFile, wxT("Telescope=%s\n"), strObservatory );
			wxFprintf( pSpecIniFile, wxT("Instrument=%s\n"), strInstrument );

			///////////////////
			// set my ref vector :: file
			wxFprintf( pSpecIniFile, wxT("File=%s\n"), strSpecLocalFile );

			// replace current
			wxString strReplace = wxT("file=");
			strReplace += strSpecRemoteFile;
			strWxLine.Replace( strReplace, wxT(""), 1 );
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
