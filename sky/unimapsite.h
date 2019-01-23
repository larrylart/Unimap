
#ifndef _UNIMAP_SITE_H
#define _UNIMAP_SITE_H

class CSkyCatalogStars;
class CSkyCatalogDso;
class CUnimapWorker;

class CUnimapSite
{
//////////////
// METHODS
public:
	CUnimapSite( CSkyCatalogStars* pCatalogStars, CUnimapWorker* pWorker=NULL );
	CUnimapSite( CSkyCatalogDso* pCatalogDso, CUnimapWorker* pWorker );
	~CUnimapSite( );

	void Init( );

	long DownloadCatalog( const wxString& strFile, int nCatalogId );

//////////////
// DATA
public:
	CSkyCatalogStars* m_pCatalogStars;
	CSkyCatalogDso* m_pCatalogDso;

	CUnimapWorker* m_pUnimapWorker;
	// site+mirrors
	wxString m_vectSite[9];
	int m_nSite;

};

#endif
