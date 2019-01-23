
// local headers
#include "../unimap_worker.h"
#include "../util/func.h"
#include "../config/mainconfig.h"
#include "sky.h"
#include "../unimap.h"

// main header
#include "catalog.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSkyCatalog
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalog::CSkyCatalog( CSky* pSky )
{
	m_pSky = pSky;
	m_pUnimapWorker = NULL;

	m_strFile = wxT( "" );
	m_strNameAbbreviation = wxT("");
	m_strName = wxT("");
	m_strFullName = wxT("");
	m_strLabel = wxT("");
	m_strOrganization = wxT("");
	m_strAuthor = wxT("");
	m_strCreationDate = wxT("");
	m_strUpdateDate = wxT("");
	m_strBibcode = wxT("");
	m_strFootPrintImg = wxT("");

	m_nCatalogLoaded = CATALOG_ID_NONE;
	m_nObjectType = CATALOG_TYPE_OTHER;
	m_nSourcesFull = 0;
	m_nSourcesQuery = 0;

	m_nRecordsNo = 0;

	m_nData = 0;
	m_nAllocated = 0;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSkyCatalog
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CSkyCatalog::~CSkyCatalog( )
{

}

/////////////////////////////////////////////////////////////////////
// Method:	SetProfile
// Class:	CSkyCatalog
// Purpose:	set cat local vars from profile
// Input:	reference to catalog profile
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CSkyCatalog::SetProfile( CatalogDef& catProfile )
{
	m_strFile = catProfile.m_strFile;
	m_strNameAbbreviation = catProfile.m_strNameAbbreviation;
	m_strName = catProfile.m_strName;
	m_strFullName = catProfile.m_strFullName;
	m_strLabel = catProfile.m_strLabel;
	m_strOrganization = catProfile.m_strOrganization;
	m_strAuthor = catProfile.m_strAuthor;
	m_strCreationDate = catProfile.m_strCreationDate;
	m_strUpdateDate = catProfile.m_strUpdateDate;
	m_strBibcode = catProfile.m_strBibcode;
	m_strFootPrintImg = catProfile.m_strFootPrintImg;

//	m_nCatalogLoaded = CATALOG_ID_NONE;
	m_nObjectType = catProfile.m_nObjectType;
//	m_nSourcesFull = 0;
//	m_nSourcesQuery = 0;

	m_nRecordsNo = catProfile.m_nRecordsNo;

}
