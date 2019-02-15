////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CELESTRAK_SITE_H
#define _CELESTRAK_SITE_H

#include "catalog_aes.h"

#define JD2000 2451545.5
#define JD1900 (JD2000 - 36525. - 1.)

//class CSkyCatalogAes;
class CUnimapWorker;

// class :: CCelestrakSite
////////////////////////////////////////////////////////////////////
class CCelestrakSite
{
//////////////
// METHODS
public:
	CCelestrakSite( CSkyCatalogAes* pCatalogAes, CUnimapWorker* pWorker=NULL );
	~CCelestrakSite( );

	// process text lines in TLE 2 lines format
	int ProcessTLETwoRecord( wxChar *name, wxChar *l1, wxChar *l2, DefCatAes* op, int bUdate=0 );
	// process text lines in TLE 3 line3 format
	int ProcessTLEThreeRecord( wxChar *name, wxChar *l1, wxChar *l2, DefCatAes* op );

	long GetObjects( int nCatId );

	// load methods from file in TLE format - multiple files 
	long FetchObjects( const wxString& strFile, int nCatId=0, int nPartId=0 );

	// load formated file
	long LoadFetchedFile( const wxString& strFile, int nObjType=0, int nSatType=0 );

//////////////
// DATA
public:
	CSkyCatalogAes* m_pCatalogAes;
	CUnimapWorker* m_pUnimapWorker;
	// site+mirrors
	wxString m_vectSite[10];
	int m_nSite;

};

#endif
