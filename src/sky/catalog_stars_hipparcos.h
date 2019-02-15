////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_STARTS_HIPPARCOS_H
#define _CATALOG_STARTS_HIPPARCOS_H

class CSkyCatalogStars;
class CUnimapWorker;

class CSkyCatalogStarsHipparcos
{
// methods:
public:
	CSkyCatalogStarsHipparcos( CSkyCatalogStars* pCatalogStars, CUnimapWorker* pWorker );
	~CSkyCatalogStarsHipparcos( );

	// binary load/export methods
//	int ExportBinary(  );
//	int LoadBinary( const char* strFile, int nLoadLimit );

	unsigned long Load( int nLoadType=0, int nLoadLimit=0,
								int bRegion=0, double nRa=0, double nDec=0,
										double nWidth=0, double nHeight=0 );
	// load ascii format
	long LoadAscii( const wxString& strFile );

//////////////////////////
// DATA
public:
	CSkyCatalogStars* m_pCatalogStars;
	CUnimapWorker* m_pUnimapWorker;
};

#endif
