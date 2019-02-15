////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_STARTS_NOMAD_H
#define _CATALOG_STARTS_NOMAD_H

class CSkyCatalogStars;
class CUnimapWorker;

class CSkyCatalogStarsNomad
{
// methods:
public:
	CSkyCatalogStarsNomad( CSkyCatalogStars* pCatalogStars, CUnimapWorker* pWorker );
	~CSkyCatalogStarsNomad( );

	// binary load/export methods
	unsigned long Load( int nLoadType, int nLoadSource, int nLoadLimit, int bRegion, double nRa, double nDec, double nWidth, double nHeight );
	unsigned long LoadBinary( const wxString& strFile, int nLoadLimit, int bRegion, double nRa=0, double nDec=0, double nWidth=0, double nHeight=0 );
	int ExportBinary( const wxString& strFile );

//////////////////////////
// DATA
public:
	CSkyCatalogStars* m_pCatalogStars;
	CUnimapWorker* m_pUnimapWorker;
};

#endif
