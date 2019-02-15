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
#include "ads_site.h"

// main header
#include "bibdb.h"

/////////////////////////////////////////////////////////////////////
CBibDb::CBibDb( CUnimapWorker* pUnimapWorker )
{
	m_pUnimapWorker = pUnimapWorker;

	m_nBibCode = 0;
}

/////////////////////////////////////////////////////////////////////
CBibDb::~CBibDb( )
{
}

// Purpose:	Get bibcodes data details from various sources
/////////////////////////////////////////////////////////////////////
void CBibDb::GetBibcodesData( )
{
	CAdsSite site( this, m_pUnimapWorker );
	site.GetADSBibcodes();
}

/////////////////////////////////////////////////////////////////////
// Method:	FindBibIdByName
// Class:	CBibDb
// Purpose:	retreives the bib vector index if any
// Input:	bib name
// Output:	bib id or -1
/////////////////////////////////////////////////////////////////////
int CBibDb::FindBibIdByName( const wxString& strBibName )
{
	int nBibId = -1;
	int i = 0;

	for( i=0; i<m_nBibCode; i++ )
	{
		if( strBibName.CmpNoCase( m_vectBibCode[i] ) == 0 ) nBibId = i;
	}

	return( nBibId );
}

/////////////////////////////////////////////////////////////////////
// Method:	ProcessPathFromBibCode
// Class:	CBibDb
// Purpose:	process bibcodes file - and create path if not exist
// Input:	bicode name
// Output:	full name - path + name 
/////////////////////////////////////////////////////////////////////
wxString CBibDb::ProcessPathFromBibCode( wxString& strBibcode )
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
