
#ifndef _ADS_SITE_H
#define _ADS_SITE_H

// wx
#include "wx/wxprec.h"

// external classes
class CBibDb;
class CUnimapWorker;

// class:	CAdsSite
///////////////////////////////////////////////////////
class CAdsSite
{
public:

	CAdsSite( CBibDb* pBibDb, CUnimapWorker* pUnimapWorker );
	~CAdsSite();

	// handlers
	wxString ProcessPathFromBibCode( wxString& strBibcode );

	int FindBibIdByName( const wxString& strBibName );
	int GetADSBibcodes( );
	int ProcessADSBibcodesPortableFile( const wxString& strFile );

//////////////
// DATA
public:

	CBibDb* m_pBibDb;
	CUnimapWorker* m_pUnimapWorker;

};

#endif
