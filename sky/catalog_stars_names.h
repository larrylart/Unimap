
#ifndef _CATALOG_STAR_NAMES_H
#define _CATALOG_STAR_NAMES_H

// local headers
#include "../util/folders.h"

#define STARS_COMMON_NAME_FILE	unMakeAppPath(wxT("data/sky_obj_cnames/stars_cnames.txt"))
#define STARS_SAO_NAME_FILE		unMakeAppPath(wxT("data/sky_obj_cnames/stars_sao_names.txt"))
#define STARS_TYCHO_NAME_FILE	unMakeAppPath(wxT("data/sky_obj_cnames/stars_tycho_names.txt"))

// local includes
#include "./star.h"

// catalog typedef
typedef struct
{
	// catalogs numbers
	unsigned char cat_type;
	unsigned int zone_no;
	unsigned long cat_no;
	unsigned char comp_no;

//	unsigned long sao_no;

	wxChar name[100];

} StarNameDef;

// external classes
class CUnimapWorker;
class CSky;
class CConfigMain;

class CSkyCatalogStarNames
{
//////////////////////
// METHODS
public:
	CSkyCatalogStarNames( CSky* pSky );
	~CSkyCatalogStarNames();

	long LoadStarNames( CUnimapWorker* pWorker );
	long LoadStripedSaoNames( CUnimapWorker* pWorker );
	long LoadTychoNames( CUnimapWorker* pWorker );

	void UnLoadStarNames( );

	int GetNameRaDec( unsigned long nId, double& nRa, double& nDec );
	int GetStarNameId( StarDef& pStar, unsigned long& nId );
	int GetStarNameByCatNo( StarDef& pStar, wxString& strName );

	int GetStarSimpleCatName( StarDef& pStar, wxString& strStarName, int bMatch=1 );
	int GetStarName( int nStarId, StarDef& pStar, wxString& strStarName );
	int GetCatStarName( int nStarId, StarDef& pStar, wxString& strStarName );

//////////////////////////
// DATA
public:
	CSky* m_pSky;
	CUnimapWorker* m_pUnimapWorker;
	CConfigMain* m_pConfig;

	// Stars common names
	StarCommonName* m_vectStarName;
	unsigned long m_nStarName;

	// star names ? check where this plugs in ?
	StarNameDef m_vectWord[8000];
	unsigned long m_nWord;

};

#endif
