
#ifndef _NOFS_NAVY_MIL_H
#define _NOFS_NAVY_MIL_H

class CSkyCatalogStars;
class CUnimapWorker;

// CNofsNavyMil class to handle online calls
class CNofsNavyMil
{
// methods
public:
	CNofsNavyMil( CSkyCatalogStars* pCatalogStars, CUnimapWorker* pWorker=NULL );
	~CNofsNavyMil( );

	long GetUSNOB1StarsFromNavyMil( double nRa, double nDec, 
							double nWidth, double nHeight );
	long FetchUSNOB1StarsFromNavyMil( wxString& strFile, double nRa, 
									   double nDec, double nRadius );
	long LoadUSNB1FileFromNavyMil( const wxString& strFile );

//////////////////////////
// DATA
public:
	CSkyCatalogStars* m_pCatalogStars;
	CUnimapWorker* m_pUnimapWorker;

};

#endif
