////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _LEDA_SITE_H
#define _LEDA_SITE_H

class CSkyCatalogDso;
class CUnimapWorker;

class CLedaSite
{
//////////////
// METHODS
public:
	CLedaSite( CSkyCatalogDso* pCatalogDso, CUnimapWorker* pWorker=NULL );
	~CLedaSite( );

	long GetObjects( int nCatId, int bRegion=0, double nCenterRA=0.0, 
					double nCenterDEC=0.0, double nRadius=0.0 );

	// load methods from file in leda format
	long FetchObjectsFromLeda( const wxString& strFile, int nCatId=0, int bRegion=0,
								double nCenterRA=0.0, double nCenterDEC=0.0, double nRadius=0.0 );

	// load formated file
	long LoadHyperledaFileFromLeda( const wxString& strFile, int bRegion );

//////////////
// DATA
public:
	CSkyCatalogDso* m_pCatalogDso;
	CUnimapWorker* m_pUnimapWorker;
	// site+mirrors
	wxString m_vectSite[9];
	int m_nSite;

};

#endif
