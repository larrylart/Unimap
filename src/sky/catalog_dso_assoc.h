////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_DSO_ASSOC_H
#define _CATALOG_DSO_ASSOC_H

// local include
#include "./star.h"
#include "../util/folders.h"

// catalog map files
#define CATALOG_MAP_PGC_TO_MESSIER_FILENAME	unMakeAppPath(wxT("data/map_cat_to_cat/pgc_to_messier.txt"))
#define CATALOG_MAP_PGC_TO_NGC_FILENAME unMakeAppPath(wxT("data/map_cat_to_cat/pgc_to_ngc.txt"))
#define CATALOG_MAP_PGC_TO_IC_FILENAME unMakeAppPath(wxT("data/map_cat_to_cat/pgc_to_ic.txt"))
#define CATALOG_MAP_MESSIER_TO_NGCIC_FILENAME unMakeAppPath(wxT("data/map_cat_to_cat/messier_to_ngcic.txt"))

//////////////////////////////////////////////////////////////////
// CATALOG TO CATALOG ID MAPS
////////////
// simple catalog to catalog map
typedef struct
{
	unsigned long src_no;
	unsigned long dest_no;

} MapCatToCatSimple;
// simple catalog to catalog map with destination type
typedef struct
{
	unsigned long src_no;
	unsigned long dest_no;
	unsigned char dest_cat_type;

} MapCatToCatSimpleWithDestType;

class CSkyCatalogDsoAssoc
{
///////////////
// METHODS
public:
	CSkyCatalogDsoAssoc( );
	~CSkyCatalogDsoAssoc( );

// to remove - use standard map
//	unsigned long GetMessierNgcNo( unsigned long nCatNo );

	/////////////////////////////////////////////
	// MAP catalog to catalog methods
	int LoadMessierCatMaps( CUnimapWorker* pWorker );
	int LoadPGCCatMaps( CUnimapWorker* pWorker );
	int IsPgcObjectMessier( unsigned long nCatNo );
	int IsPgcObjectNgc( unsigned long nCatNo );
	int IsPgcObjectIc( unsigned long nCatNo );
	int IsNgcIcObjectMessier( unsigned long nCatNo, unsigned char nCatType );
	int IsMessierObjectNgcIc( unsigned long nCatNo );

//////////////////////////
// DATA
public:

	/////////////////////////////////////////////
	// MAP catalog to catalog vectors
	MapCatToCatSimple*					m_vectMapPgcToMessier;
	unsigned long						m_nMapPgcToMessier;
	MapCatToCatSimple*					m_vectMapPgcToNgc;
	unsigned long						m_nMapPgcToNgc;
	MapCatToCatSimple*					m_vectMapPgcToIc;
	unsigned long						m_nMapPgcToIc;
	MapCatToCatSimpleWithDestType*		m_vectMapMessierToNgcIc;
	unsigned long						m_nMapMessierToNgcIc;

};

#endif
