
#ifndef _EXOPLANET_EU_SITE_H
#define _EXOPLANET_EU_SITE_H

class CSkyCatalogExoplanets;
class CUnimapWorker;

class CExoplanetEuSite
{
//////////////
// METHODS
public:
	CExoplanetEuSite( CSkyCatalogExoplanets* pCatalogExoplanets, CUnimapWorker* pWorker=NULL );
	~CExoplanetEuSite( );

	long GetObjects( int nCatId, int bRegion=0, double nCenterRA=0.0, 
					double nCenterDEC=0.0, double nRadius=0.0 );

	// load methods from file in leda format
	long FetchObjects( const wxString& strFile, int nCatId=0, int bRegion=0,
								double nCenterRA=0.0, double nCenterDEC=0.0, double nRadius=0.0 );

	// load formated file
	long LoadFetchedFile( const wxString& strFile, int bRegion );

//////////////
// DATA
public:
	CSkyCatalogExoplanets* m_pCatalogExoplanets;
	CUnimapWorker* m_pUnimapWorker;
	// site+mirrors
	wxString m_vectSite[10];
	int m_nSite;

};

#endif
