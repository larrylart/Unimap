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
#include "bibdb.h"
#include "../unimap_worker.h"

// main header
#include "ads_site.h"

/////////////////////////////////////////////////////////////////////
CAdsSite::CAdsSite( CBibDb* pBibDb, CUnimapWorker* pUnimapWorker )
{
	m_pBibDb = pBibDb;
	m_pUnimapWorker = pUnimapWorker;
}

/////////////////////////////////////////////////////////////////////
CAdsSite::~CAdsSite( )
{
}

/////////////////////////////////////////////////////////////////////
// Method:	FindBibIdByName
// Class:	CAdsSite
// Purpose:	retreives the bib vector index if any
// Input:	bib name
// Output:	bib id or -1
/////////////////////////////////////////////////////////////////////
int CAdsSite::FindBibIdByName( const wxString& strBibName )
{
	int nBibId = -1;
	int i = 0;

	for( i=0; i<m_pBibDb->m_nBibCode; i++ )
	{
		if( strBibName.CmpNoCase( m_pBibDb->m_vectBibCode[i] ) == 0 ) nBibId = i;
	}

	return( nBibId );
}

////////////////////////////////////////////////////////////////////
// Method:	GetADSBibcodes
// Class:	CAdsSite
// Purpose:	Get information from online ADS bibcodes
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAdsSite::GetADSBibcodes( ) 
{
	if( m_pBibDb->m_nBibCode <= 0 ) return( 0 );

	wxFSFile* pFile = NULL;
	wxInputStream* pStream = NULL;
	FILE* pLocalFile = NULL;
	wxString strFile;
	wxString strUrlGetData;
	int nPageNo = 0;
	int nInPageCount = 0;
	wxString strBibCodes[200];
	int i = 0;
	wxString strMsg;
	int nBibcodesToGet = 0;

	// build bibcodes string
	for( i=0; i<m_pBibDb->m_nBibCode; i++ )
	{
		// now we first check if this is local
		wxString strBibcodeFullName = ProcessPathFromBibCode( m_pBibDb->m_vectBibCode[i] );

		// check if bicode file already exists
		if( !::wxFileExists( strBibcodeFullName ) )
		{
			// build pages
			if( nInPageCount >= 100 )
			{
				// incepment page count
				nPageNo++;
				// reset in page counter to zero
				nInPageCount = 0;
			}
			// if counter zero init string
			if( nInPageCount == 0 ) strBibCodes[nPageNo] = wxT("");

			// encode and concatenate
			if( i == m_pBibDb->m_nBibCode-1 || nInPageCount == 99 )
				strBibCodes[nPageNo] += URLEncode( m_pBibDb->m_vectBibCode[i] );
			else
				strBibCodes[nPageNo] += URLEncode( m_pBibDb->m_vectBibCode[i] ) + wxT(",");
			// increment the in page counter
			nInPageCount++;
			// increment no of bicodes to get
			nBibcodesToGet++;
		}
	}
	// if no codes to get return
	if( nBibcodesToGet == 0 )
	{
		// setdefault bibcodes
//		SelectBibcode(  );
//		GetSizer()->Layout( );
//		Fit( );

		return( 1 );
	}

	// now for every page - get file
	for( i=0; i<=nPageNo; i++ ) 
	{
		// setlocal file
		strFile.Printf( wxT("%s%s%d%s%s"), TEMP_ONLINE_OBJECT_DATA_PATH, wxT("bibcodes__"), i, wxT("__"), wxT(".txt") );

//		wxString strWxFile = strFile;
//		if( ::wxFileExists( strWxFile ) ==  0 )
//		{
			strMsg.Printf( wxT("Fetch ADS reference part %d ..."), i+1 );
			m_pUnimapWorker->SetWaitDialogMsg( strMsg );
			///////////////////////////////////
			// ADS form to get bibcodes 
			// http://adsabs.harvard.edu/cgi-bin/nph-abs_connect?db_key=ALL&PRE=YES&warnings=YES&version=1&start_nr=1&bibcode=%s
			// nr_to_return = ??
			// eu = faster
			// http://cdsads.u-strasbg.fr/cgi-bin/nph-basic

			// set URL to get ADS data
//			strUrlGetData.sprintf( "http://adsabs.harvard.edu/cgi-bin/nph-abs_connect?db_key=ALL&PRE=YES&warnings=YES&version=1&start_nr=1&data_type=PORTABLE&bibcode=%s",
//										strBibCodes[i] );
			strUrlGetData.sprintf( wxT("http://cdsads.u-strasbg.fr/cgi-bin/nph-abs_connect?db_key=ALL&PRE=YES&warnings=YES&version=1&start_nr=1&data_type=PORTABLE&bibcode=%s"),
										strBibCodes[i] );

			////////////////////////////
			// here we initialize the reading process
			wxFileSystem *pUrl = new wxFileSystem();
			pUrl->AddHandler( new wxInternetFSHandler );

			// then open data
			pFile = pUrl->OpenFile( strUrlGetData );

			if( !pFile || pFile == NULL ) return( 0 );

			pStream = pFile->GetStream( );

			// open local file to write
			pLocalFile = wxFopen( strFile, wxT("w") );
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
//		}

		strMsg.Printf( wxT("Load ADS reference part %d ..."), i+1 );
		m_pUnimapWorker->SetWaitDialogMsg( strMsg );

		// now process/load local file
		ProcessADSBibcodesPortableFile( strFile );

	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	ProcessPathFromBibCode
// Class:	CAdsSite
// Purpose:	process bibcodes file - and create path if not exist
// Input:	bicode name
// Output:	full name - path + name 
/////////////////////////////////////////////////////////////////////
wxString CAdsSite::ProcessPathFromBibCode( wxString& strBibcode )
{
//	0123456789012345678
//  YYYYJJJJJVVVVMPPPPA
//	wxRegEx reBibCode( wxT( "^([0-9]{4})([.]{5})([.]{4})([.]{1})([.]{4})([.]{1})$" ), wxRE_ADVANCED  );

	wxString strBicodePathYear = BIBCODES_SUMMARY_DATA_PATH + strBibcode.Mid( 0, 4 );
	wxString strBicodePublication = strBibcode.Mid( 4, 5 );
	strBicodePublication.Replace( wxT("."), wxT("_") );
	wxString strBicodePathYearPublication = strBicodePathYear + wxT("/") + strBicodePublication;

	// check if path year exists else create
	if( !::wxDirExists( strBicodePathYear ) )
	{
		::wxMkdir( strBicodePathYear );
	}
	// check if path year+publication exists or create
	if( !::wxDirExists( strBicodePathYearPublication ) )
	{
		::wxMkdir( strBicodePathYearPublication );
	}

	wxString strBibcodeFullName = strBicodePathYearPublication + wxT("/") + strBibcode;
	
	return( strBibcodeFullName );
}

/////////////////////////////////////////////////////////////////////
// Method:	ProcessADSBibcodesHtmlFile
// Class:	CAdsSite
// Purpose:	process ADS bibcodes data file
// Input:	file name
// Output:	statsus
/////////////////////////////////////////////////////////////////////
int CAdsSite::ProcessADSBibcodesPortableFile( const wxString& strFile )
{
	FILE* pLocalFile = NULL;
	FILE* pFileBibcode = NULL;
	int nBibCode = -1;

	// flags
	int bTitle = 0;
	int bAuthor = 0;
	int bProcF = 0;
	int bJournal = 0;
	int bProcK = 0;
	int bProcG = 0;
	int bCopyrights = 0;
	int bDataLinks = 0;
	int bAbstract = 0;
	int bPrint = 0;
	// local strings
	wxString strBibTitle = wxT("");
	wxString strBibAuthor = wxT("");
	wxString strBibDataLinks = wxT("");
	wxString strBibJournal = wxT("");
	wxString strBibSummary = wxT("");

//	wxFont smallFont( 6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT );

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[2000];
	// sections regex - ^[\\ ]+([^\\ ]+)\\ ([^\\ ]+)?\\ \\ 
	wxRegEx reBibCode( wxT( "^\\%R\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reTitle( wxT( "^\\%T\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reAuthor( wxT( "^\\%A\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reProcF( wxT( "^\\%F\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reJournal( wxT( "^\\%J\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reVolume( wxT( "^\\%V\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reDate( wxT( "^\\%D\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reFirstPage( wxT( "^\\%B\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reLastPage( wxT( "^\\%L\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reProcK( wxT( "^\\%K\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reProcG( wxT( "^\\%G\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reCopyrights( wxT( "^\\%C\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reDataLinks( wxT( "^\\%I\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reUrl( wxT( "^\\%U\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reVersion( wxT( "^\\%S\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx reAbstract( wxT( "^\\%B\\ +(.+)$" ), wxRE_ADVANCED  );
	wxRegEx rePrint( wxT( "^\\%Y\\ +(.+)$" ), wxRE_ADVANCED  );
	// data links 
	wxRegEx reDataLinkAbstract( wxT( "ABSTRACT:\\ +(.+)$" ) );
	wxRegEx reDataLinkEjournal( wxT( "EJOURNAL:\\ +(.+)$" ) );
	wxRegEx reDataLinkArticle( wxT( "ARTICLE:\\ +(.+)$" ) );
	wxRegEx reDataLinkPreprint( wxT( "PREPRINT:\\ +(.+)$" ) );
	wxRegEx reDataLinkData( wxT( "DATA:\\ +(.+)$" ) );
	wxRegEx reDataLinkReferences( wxT( "REFERENCES:\\ +(.+)$" ) );
	wxRegEx reDataLinkSimbad( wxT( "SIMBAD:\\ +(.+)$" ) );
	wxRegEx reDataLinkAlsoread( wxT( "AR:\\ +(.+)$" ) );
	wxRegEx reDataLinkCitations( wxT( "CITATIONS:\\ +(.+)$" ) );
	wxRegEx reDataLinkAssociated( wxT( "ASSOCIATED:\\ +(.+)$" ) );

	// other variables used
	wxChar strTmp[1000];

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
		// check for paterns
		if( reBibCode.Matches( strWxLine ) )
		{
			// first we check to see aprevious file was open
			if( pFileBibcode != NULL )
			{
				// write file
				wxFprintf( pFileBibcode, wxT("Title=%s\n"), strBibTitle );
				wxFprintf( pFileBibcode, wxT("Author=%s\n"), strBibAuthor );
				wxFprintf( pFileBibcode, wxT("DataLinks=%s\n"), strBibDataLinks );
				wxFprintf( pFileBibcode, wxT("Journal=%s\n"), strBibJournal );
				wxFprintf( pFileBibcode, wxT("Summary=%s\n"), strBibSummary );
				// close file
				fclose( pFileBibcode );
				pFileBibcode = NULL;
			}
			// empty strings
			strBibTitle = wxT("");
			strBibAuthor = wxT("");
			strBibDataLinks = wxT("");
			strBibJournal = wxT("");
			strBibSummary = wxT("");
			// extract bibcode from line
			wxString strBibcode = reBibCode.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 );
			// check if bibcode is in my list
			nBibCode = FindBibIdByName( strBibcode );
			if( nBibCode >= 0 )
			{
				// now we first check if this is local
				wxString strBibcodeFullName = ProcessPathFromBibCode( strBibcode );
				if( ::wxFileExists( strBibcodeFullName ) )
				{
					// set to skip until next bibcode
					nBibCode = -1;
					continue;

				} else
				{
					pFileBibcode = wxFopen( strBibcodeFullName, wxT("wb") );
				}

			} else
				continue;		

			// reset flags
			bTitle = 0;
			bAuthor = 0;
			bProcF = 0;
			bJournal = 0;
			bProcK = 0;
			bProcG = 0;
			bCopyrights = 0;
			bDataLinks = 0;
			bAbstract = 0;
			bPrint = 0;

		} else if( reTitle.Matches( strWxLine ) && nBibCode >=0 )
		{
			strBibTitle = reTitle.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 );
			strBibTitle.Trim( 1 ).Trim( 0 );
			// set flag 
			bTitle = 1;
			// reset flags
			bAuthor = 0;
			bProcF = 0;
			bJournal = 0;
			bProcK = 0;
			bProcG = 0;
			bCopyrights = 0;
			bDataLinks = 0;
			bAbstract = 0;
			bPrint = 0;

			continue;

		} else if( reAuthor.Matches( strWxLine ) && nBibCode >=0 )
		{
			strBibAuthor = reAuthor.GetMatch( strWxLine, 1 );
			strBibAuthor.Trim( 1 ).Trim( 0 );
			// set flag 
			bAuthor = 1;
			// reset flags
			bTitle = 0;
			bProcF = 0;
			bJournal = 0;
			bProcK = 0;
			bProcG = 0;
			bCopyrights = 0;
			bDataLinks = 0;
			bAbstract = 0;
			bPrint = 0;

			continue;

		} else if( reDataLinks.Matches( strWxLine ) && nBibCode >=0 )
		{
			strWxLine = reDataLinks.GetMatch( strWxLine, 1 );
			strWxLine.Trim( 1 ).Trim( 0 );
			// set no of links to zero
//			m_nBibDataLinks[nBibCode] = 0;
			// set flag 
			bDataLinks = 1;
			// reset flags
			bTitle = 0;
			bAuthor = 0;
			bProcF = 0;
			bJournal = 0;
			bProcK = 0;
			bProcG = 0;
			bCopyrights = 0;
			bAbstract = 0;
			bPrint = 0;

//			continue;

		} else if( reJournal.Matches( strWxLine ) && nBibCode >=0 )
		{
			strBibJournal = reJournal.GetMatch( strWxLine, 1 );
			strBibJournal.Trim( 1 ).Trim( 0 );
			// set flag 
			bJournal = 1;
			// reset flags
			bDataLinks = 0;
			bTitle = 0;
			bAuthor = 0;
			bProcF = 0;
			bProcK = 0;
			bProcG = 0;
			bCopyrights = 0;
			bAbstract = 0;
			bPrint = 0;

			continue;

		} else if( reAbstract.Matches( strWxLine ) && nBibCode >=0 )
		{
			strBibSummary = reAbstract.GetMatch( strWxLine, 1 );
			strBibSummary.Trim( 1 ).Trim( 0 );
			strBibSummary += wxT(" ");
			// set flag 
			bAbstract = 1;
			// reset flags
			bDataLinks = 0;
			bTitle = 0;
			bAuthor = 0;
			bProcF = 0;
			bJournal = 0;
			bProcK = 0;
			bProcG = 0;
			bCopyrights = 0;
			bPrint = 0;

			continue;

		} else if( reVolume.Matches( strWxLine ) && nBibCode >=0 )
		{
//			m_vectBibVolume[nBibCode] = reVolume.GetMatch( strLine, 1 );
			// reset flags
			bAbstract = 0;
			bDataLinks = 0;
			bTitle = 0;
			bAuthor = 0;
			bProcF = 0;
			bJournal = 0;
			bProcK = 0;
			bProcG = 0;
			bCopyrights = 0;
			bPrint = 0;

			continue;

		} else if( rePrint.Matches( strWxLine ) && nBibCode >=0 )
		{
			//m_vectBibPrint ???[nBibCode] = rePrint.GetMatch( strLine, 1 );
			// set flag 
			bPrint = 1;
			// reset flags
			bAbstract = 0;
			bDataLinks = 0;
			bTitle = 0;
			bAuthor = 0;
			bProcF = 0;
			bJournal = 0;
			bProcK = 0;
			bProcG = 0;
			bCopyrights = 0;

			continue;
		} else if( reProcF.Matches( strWxLine ) && nBibCode >=0 )
		{
			//m_vectBibPrint ???[nBibCode] = reProcF.GetMatch( strLine, 1 );
			// set flag
			bProcF = 0;
			// reset flags
			bAbstract = 0;
			bDataLinks = 0;
			bTitle = 0;
			bAuthor = 0;
			bJournal = 0;
			bProcK = 0;
			bProcG = 0;
			bCopyrights = 0;
			bPrint = 0;

			continue;
		}

		//////////////////////////////
		// process by flags
		if( bTitle ==  1 && nBibCode >=0 )
		{
			strBibTitle += strWxLine;

		} else if( bAuthor ==  1 && nBibCode >=0 )
		{
			strBibAuthor += strWxLine;

		} else if( bJournal ==  1 && nBibCode >=0 )
		{
			strBibJournal += strWxLine;

		} else if( bAbstract ==  1 && nBibCode >=0 )
		{
			// first process for tags
			// add to my string
			strBibSummary += strWxLine + wxT(" ");

		} else if( bDataLinks ==  1 && nBibCode >=0 )
		{
			// now check for types of data links
			if( reDataLinkAbstract.Matches( strWxLine ) )
			{
				strBibDataLinks += wxT( "Abstract" );
				strBibDataLinks += wxT( "," );

			} else if( reDataLinkEjournal.Matches( strWxLine ) )
			{
				strBibDataLinks += wxT( "eJournal" );
				strBibDataLinks += wxT( "," );

			} else if( reDataLinkArticle.Matches( strWxLine ) )
			{
				strBibDataLinks += wxT( "Article (Pdf/Ps)" );
				strBibDataLinks += wxT( "," );

			} else if( reDataLinkPreprint.Matches( strWxLine ) )
			{
				strBibDataLinks += wxT( "Pre-Print" );
				strBibDataLinks += wxT( "," );

			} else if( reDataLinkData.Matches( strWxLine ) )
			{
				strBibDataLinks += wxT( "Online Data" );
				strBibDataLinks += wxT( "," );

			} else if( reDataLinkReferences.Matches( strWxLine ) )
			{
				strBibDataLinks += wxT( "References" );
				strBibDataLinks += wxT( "," );

			} else if( reDataLinkSimbad.Matches( strWxLine ) )
			{
				strBibDataLinks += wxT( "Simbad" );
				strBibDataLinks += wxT( "," );

			} else if( reDataLinkAlsoread.Matches( strWxLine ) )
			{
				strBibDataLinks += wxT( "Also Read" );
				strBibDataLinks += wxT( "," );

			} else if( reDataLinkCitations.Matches( strWxLine ) )
			{
				strBibDataLinks += wxT( "Citations" );
				strBibDataLinks += wxT( "," );

			} else if( reDataLinkAssociated.Matches( strWxLine ) )
			{
				strBibDataLinks += wxT( "Associated" );
				strBibDataLinks += wxT( "," );
			}
		}

	}

	// close my file
	fclose( pLocalFile );
	// if last file open close
	if( pFileBibcode != NULL )
	{
		// write file
		wxFprintf( pFileBibcode, wxT("Title=%s\n"), strBibTitle );
		wxFprintf( pFileBibcode, wxT("Author=%s\n"), strBibAuthor );
		wxFprintf( pFileBibcode, wxT("DataLinks=%s\n"), strBibDataLinks );
		wxFprintf( pFileBibcode, wxT("Journal=%s\n"), strBibJournal );
		wxFprintf( pFileBibcode, wxT("Summary=%s\n"), strBibSummary );
		// close file
		fclose( pFileBibcode );
		pFileBibcode = NULL;
	}

	// setdefault bibcodes
//	SelectBibcode(  );

	return( 1 );
}
