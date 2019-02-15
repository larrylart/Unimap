////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_DSO_NAMES_H
#define _CATALOG_DSO_NAMES_H

#include "sky.h"

// local headers
#include "../util/folders.h"

// files
#define DSO_COMMON_NAME_FILE unMakeAppPath(wxT("data/sky_obj_cnames/dso_cnames.txt"))

// dso common names flags
#define DSO_COMMON_NAME_HAS_NGC		1
#define DSO_COMMON_NAME_HAS_IC		2
#define DSO_COMMON_NAME_HAS_PGC		4
#define DSO_COMMON_NAME_HAS_RADEC	8

/////////////////////////////////////////////////
// dso common name structure
typedef struct
{
	// pointer to name - to be alocated
	wxChar*	name;

	// catalogs numbers
	unsigned long cat_no;
	// ra/dec - coordinates
	double ra;
	double dec;
	// type of coord record
	unsigned char ctype;

} DsoCommonName;

// external classes
class CConfigMain;

class CSkyCatalogDsoNames
{
//////////////////////
// METHODS
public:
	CSkyCatalogDsoNames( CSky* pSky );
	~CSkyCatalogDsoNames();

	//long LoadDsoNames( const char* strFile );
	long LoadDsoNames( CUnimapWorker* pWorker );
	void UnLoadDsoNames( );

	int GetDsoSimpleCatName( StarDef& pDso, wxString& strDsoName );
	int GetDsoSimpleCatName( DsoDefCatBasic& pDso, unsigned char nType, wxString& strDsoName );
	int GetDosCatName( StarDef& pDso, wxString& strDsoCatName );
	int GetDsoCommonName( wxString& strName, unsigned long nCatNo, int nType );
	int GetDsoNameRaDec( unsigned long nId, double& nRa, double& nDec );

//////////////////////////
// DATA
public:
	CSky* m_pSky;
	CConfigMain* m_pConfig;

	// dso common names
	DsoCommonName* m_vectDsoName;
	unsigned long m_nDsoName;
};

#endif
