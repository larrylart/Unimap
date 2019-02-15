////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_XGAMMA_INTEGRAL_H
#define _CATALOG_XGAMMA_INTEGRAL_H

// integral types
static char m_vectCatIntegralTypes[50][30] = 
 { "Unidentified",
 "AGN",
 "BL Lac",
 "Be Star",
 "Blazar",
 "Cataclysmic Var. AM Her type",
 "Cataclysmic Var. DQ Her type",
 "Cataclysmic Variable Star",
 "Cluster of Galaxies",
 "Dwarf Nova",
 "Eclipsing Binary",
 "Emission-line Galaxy",
 "Emission-line Star",
 "Em-line Star",
 "Flare Star",
 "GRB",
 "Galaxy",
 "Gamma-ray source",
 "Group of Galaxies",
 "HMXB",
 "IR source",
 "Interacting Galaxies",
 "LINER-type AGN",
 "LMXB",
 "Liner AGN",
 "Low Surface Brightness Galax",
 "Molecular Cloud",
 "Nebula of unknown nature",
 "Pulsar",
 "Quasar",
 "Radio Galaxy",
 "Radio source",
 "SN",
 "SNR",
 "Sey",
 "Sey-1",
 "Sey-2",
 "Seyfert Galaxy",
 "Star",
 "Star in Cluster",
 "Supercluster of Galaxies",
 "Variable Star",
 "Variable of RS CVn type",
 "WR Star",
 "Wolf-Rayet Star",
 "XRB",
 "X-ray Source",
 "X-ray source" };

// local headers
#include "catalog_xgamma.h"

// external classes
class CUnimapWorker;

class CSkyCatalogXGammaIntegral
{
// methods:
public:
	CSkyCatalogXGammaIntegral( CSkyCatalogXGamma* pCatalogXGamma, CUnimapWorker* pWorker=NULL );
	~CSkyCatalogXGammaIntegral( );

	/////////////////////////////////////
	// MSL radio sources catalog methods
	unsigned long Load( int nLoadType=0, int nLoadSource=0, int nLoadLimit=0, int bRegion=0, double nRa=0.0, double nDec=0.0, double nRadius=0.0 );

	unsigned long LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatBasicXGamma* vectCatalog, unsigned int nSize );

//////////////////////////
// DATA
public:
	CSkyCatalogXGamma* m_pCatalogXGamma;
	CUnimapWorker* m_pUnimapWorker;
};

#endif
