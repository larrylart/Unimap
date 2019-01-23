
#ifndef _CATALOG_STARS_2MASS_H
#define _CATALOG_STARS_2MASS_H

class CSkyCatalogStars;
class CUnimapWorker;

class CSkyCatalogStars2Mass
{
// methods:
public:
	CSkyCatalogStars2Mass( CSkyCatalogStars* pCatalogStars, CUnimapWorker* pWorker );
	~CSkyCatalogStars2Mass( );

	// binary load/export methods
	unsigned long Load( int nLoadType, int nLoadLimit, int bRegion, double nRa, double nDec, double nWidth, double nHeight );
	unsigned long LoadBinary( const wxString& strFile, int nLoadLimit, int bRegion, double nRa=0, double nDec=0, double nWidth=0, double nHeight=0 );
	int ExportBinary( const wxString& strFile );

//////////////////////////
// DATA
public:
	CSkyCatalogStars* m_pCatalogStars;
	CUnimapWorker* m_pUnimapWorker;
};

#endif
