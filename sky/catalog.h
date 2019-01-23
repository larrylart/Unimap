
#ifndef _SKY_CATALOG_H
#define _SKY_CATALOG_H

// local headers
#include "./star.h"
#include "catalog_types.h"

// external classes
class CSky;
class CSkyCatalog;
class CUnimapWorker;

// structure :: CatalogDef
////////////////////
typedef struct
{
	int m_nId;
	int m_nObjectType;
	int m_nCustomUse;
	wxString m_strVersion;
	// :: number of records
	unsigned long m_nRecordsNo;

	wxString m_strFile;
	wxString m_strNameAbbreviation;
	wxString m_strName;
	wxString m_strFullName;
	wxString m_strLabel;
	wxString m_strOrganization;
	wxString m_strAuthor;
	// :: dates : creation/update
	wxString m_strCreationDate;
	wxString m_strUpdateDate;
	// :: bibcode
	wxString m_strBibcode;
	// :: sources : FULL
	int m_vectSourcesFull[10];
	int m_nSourcesFull;
	// :: sources : Query
	int m_vectSourcesQuery[10];
	int m_nSourcesQuery;
	// :: FOOT-PRINT/COVERAGE IMAGE
	wxString m_strFootPrintImg;
	// :: DESCRIPTION
	wxString m_strDescription;

} CatalogDef;

// class :: CSkyCatalog
////////////////////////////////////////////////////////////////////
class CSkyCatalog
{
/////////////
// METHODS
public:
	CSkyCatalog( CSky* pSky );
	~CSkyCatalog( );

	void SetProfile( CatalogDef& catProfile );

/////////////
// DATA
public:
	CSky* m_pSky;
	CUnimapWorker* m_pUnimapWorker;

	// common - objects loaded/allocated
	unsigned long		m_nData;
	unsigned long		m_nAllocated;

	// this gives me what star catalog has been loaded
	int m_nCatalogLoaded;
	// params of teh current catalog
	CatalogDef m_Def;

	int m_nObjectType;
	// :: number of records
	unsigned long m_nRecordsNo;

	wxString m_strFile;
	wxString m_strNameAbbreviation;
	wxString m_strName;
	wxString m_strFullName;
	wxString m_strLabel;
	wxString m_strOrganization;
	wxString m_strAuthor;
	// :: dates : creation/update
	wxString m_strCreationDate;
	wxString m_strUpdateDate;
	// :: bibcode
	wxString m_strBibcode;
	// :: sources : FULL
	int m_vectSourcesFull[10];
	int m_nSourcesFull;
	// :: sources : Query
	int m_vectSourcesQuery[10];
	int m_nSourcesQuery;
	// :: FOOT-PRINT/COVERAGE IMAGE
	wxString m_strFootPrintImg;
	// :: DESCRIPTION
	wxString m_strDescription;

};

#endif
