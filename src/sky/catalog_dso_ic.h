////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_DSO_IC_H
#define _CATALOG_DSO_IC_H

#include "catalog_dso.h"

class CSky;

class CSkyCatalogDsoIc : public CSkyCatalogDso
{
// methods:
public:
	CSkyCatalogDsoIc( CSky* pSky );
	~CSkyCatalogDsoIc( );

	// IC methods
	void UnLoadIcRev( );
	long LoadIcRev( const wxString& strFile );
	int GetIcRaDec( unsigned long nCatNo, double& nRa, double& nDec );
	// temp - to remove latter after I remove that messier field from binary
	unsigned long LoadIcBinary( );

//////////////////////////
// DATA
public:
	DsoDefCatNgcIc*	m_vectIc;
	unsigned long	m_nIc;

};

#endif

