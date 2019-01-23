
#ifndef _CATALOG_STARTS_USNOB_H
#define _CATALOG_STARTS_USNOB_H

class CSkyCatalogStars;
class CUnimapWorker;

class CSkyCatalogStarsUsnob
{
// methods:
public:
	CSkyCatalogStarsUsnob( CSkyCatalogStars* pCatalogStars, CUnimapWorker* pWorker );
	~CSkyCatalogStarsUsnob( );

	// binary load/export methods
	int ExportBinary( const wxString& strFile );

	unsigned long Load( int nLoadType, int nLoadSource, int nLoadLimit, int bRegion, double nRa, double nDec, double nWidth, double nHeight );
//	int LoadBinary( const char* strFile, int nLoadLimit );
	unsigned long LoadBinary( const wxString& strFile, int nLoadLimit, int bRegion, double nRa=0, double nDec=0, double nWidth=0, double nHeight=0 );

//////////////////////////
// DATA
public:
	CSkyCatalogStars* m_pCatalogStars;
	CUnimapWorker* m_pUnimapWorker;
};

#endif
