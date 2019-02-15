////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_DSO_MESSIER_H
#define _CATALOG_DSO_MESSIER_H

// binary file to load
#define DEFAULT_MESSIER_ASCII_FILENAME	unMakeAppPath(wxT("data/catalog/dso/messier.txt"))

class CSkyCatalogDsoMessier : public CSkyCatalogDso
{
// methods:
public:
	CSkyCatalogDsoMessier( CSky* pSky );
	~CSkyCatalogDsoMessier( );

	int Load( );
	int LoadMessier( const wxString& strFile );
	int GetMessierRaDec( unsigned long nCatNo, double& nRa, double& nDec );

//////////////////////////
// DATA
public:

	// messier catalog
	DsoDefCatMessier* m_vectMessier;
	unsigned int m_nMessier;

};

#endif

