////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_RADIO_NVSS_H
#define _CATALOG_RADIO_NVSS_H

// local headers
#include "catalog_radio.h"

// external classes
class CSkyCatalogRadio;
class CUnimapWorker;

class CSkyCatalogRadioNvss
{
// methods:
public:
	CSkyCatalogRadioNvss( CSkyCatalogRadio* pCatalogRadio, CUnimapWorker* pWorker=NULL );
	~CSkyCatalogRadioNvss( );

	/////////////////////////////////////
	// NVSS radio sources catalog methods
	unsigned long Load( int nLoadType=0, int nLoadSource=0, int nLoadLimit=0, int bRegion=0, double nRa=0.0, double nDec=0.0, double nRadius=0.0 );

	unsigned long LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatBasicRadio* vectCatalog, unsigned int nSize );

//////////////////////////
// DATA
public:
	CSkyCatalogRadio* m_pCatalogRadio;
	CUnimapWorker* m_pUnimapWorker;
};

#endif
