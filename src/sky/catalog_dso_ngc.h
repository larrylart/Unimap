////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_DSO_NGC_H
#define _CATALOG_DSO_NGC_H

#include "catalog_dso.h"

// external classes
class CSky;

class CSkyCatalogDsoNgc : public CSkyCatalogDso
{
// methods:
public:
	CSkyCatalogDsoNgc( CSky* pSky );
	~CSkyCatalogDsoNgc( );

	// NGC methods
	void UnLoadNgcRev( );
	long LoadNgcRev( const wxString& strFile );
	int GetNgcRaDec( unsigned long nCatNo, double& nRa, double& nDec );

	unsigned long LoadNgcBinary( );

//////////////////////////
// DATA
public:
	DsoDefCatNgcIc*	m_vectNgc;
	unsigned long	m_nNgc;

};

#endif

