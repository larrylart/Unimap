
#ifndef _VIZIER_H
#define _VIZIER_H

class CSkyCatalogStars;
class CSkyCatalogDso;
class CSkyCatalogRadio;
class CSkyCatalogXGamma;
class CSkyCatalogSupernovas;
class CSkyCatalogBlackholes;
class CSkyCatalogMStars;
class CSkyCatalogAsteroids;
class CSkyCatalogComets;
class CUnimapWorker;

// VIZIER class to handle online calls
class CVizier
{
// methods
public:
	CVizier( CSkyCatalogStars* pCatalogStars, CUnimapWorker* pWorker=NULL );
	CVizier( CSkyCatalogDso* pCatalogDso, CUnimapWorker* pWorker=NULL );
	CVizier( CSkyCatalogRadio* pCatalogRadio, CUnimapWorker* pWorker=NULL );
	CVizier( CSkyCatalogXGamma* pCatalogXGamma, CUnimapWorker* pWorker=NULL );
	CVizier( CSkyCatalogSupernovas* pCatalogSupernovas, CUnimapWorker* pWorker=NULL );
	CVizier( CSkyCatalogBlackholes* pCatalogBlackholes, CUnimapWorker* pWorker=NULL );
	CVizier( CSkyCatalogMStars* pCatalogMStars, CUnimapWorker* pWorker=NULL );
	CVizier( CSkyCatalogAsteroids* pCatalogAsteroids, CUnimapWorker* pWorker=NULL );
	CVizier( CSkyCatalogComets* pCatalogComets, CUnimapWorker* pWorker=NULL );
	~CVizier( );

	void Init( );

	long FetchObjectsFromVizier( const wxString& strFile, double nCenterRA, double nCenterDEC, 
									double nRadius, int nCatId );

	////////////////////////////
	// methods i call remotely
	// :: STARS
	long GetStarsFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
								double nWidth, double nHeight );
	// :: DSO
	long GetDsoFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
								double nWidth, double nHeight );
	// :: RADIO
	long GetRadioFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
								double nWidth, double nHeight );
	// :: X-RAY/GAMMA
	long GetXGammaFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
									double nWidth, double nHeight );
	// :: SUPERNOVAS
	long GetSupernovasFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
									double nWidth, double nHeight );
	// :: BLACKHOLES
	long GetBlackholesFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
									double nWidth, double nHeight );
	// :: MULTIPLE/DOUBLE STARS
	long GetMStarsFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
									double nWidth, double nHeight );
	// :: ASTEROIDS
	long GetAsteroidsFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
									double nWidth, double nHeight );
	// :: COMETS
	long GetCometsFromVizier( int nCatId, int bRegion, double nCenterRA, double nCenterDEC,
									double nWidth, double nHeight );

	/////////////////
	// different catalog formats load function
	// :: STAR catalogs
	long LoadSAOFileFromVizier( const wxString& strFile, int bRegion );
	long LoadHIPPARCOSFileFromVizier( const wxString& strFile, int bRegion );
	long LoadTYCHOFileFromVizier( const wxString& strFile, int bRegion );
	long LoadUSNOB1FileFromVizier( const wxString& strFile );
	long LoadNOMADFileFromVizier( const wxString& strFile );
	long Load2MASSFileFromVizier( const wxString& strFile );
	long LoadGSCFileFromVizier( const wxString& strFile );
	// :: DSO catalogs
	long LoadHyperledaFileFromVizier( const wxString& strFile, int bRegion );
	long LoadUgcFileFromVizier( const wxString& strFile, int bRegion );
	long LoadMgcFileFromVizier( const wxString& strFile, int bRegion );
	// :: RADIO sources
	long LoadSpecfindFileFromVizier( const wxString& strFile, int bRegion );
	long LoadVlssFileFromVizier( const wxString& strFile, int bRegion );
	long LoadNvssFileFromVizier( const wxString& strFile, int bRegion );
	long LoadMslFileFromVizier( const wxString& strFile, int bRegion );
	// :: X-RAY/GAMMA sources
	long LoadRosatFileFromVizier( const wxString& strFile, int bRegion );
	long LoadBatseFileFromVizier( const wxString& strFile, int bRegion );
	long LoadIntegralFileFromVizier( const wxString& strFile, int bRegion );
	long LoadBepposaxFileFromVizier( const wxString& strFile, int bRegion );
	long Load2XMMiFileFromVizier( const wxString& strFile, int bRegion );
	long LoadHeaoA1FileFromVizier( const wxString& strFile, int bRegion );
	// :: SUPERNOVAS
	long LoadAscFileFromVizier( const wxString& strFile, int bRegion );
	long LoadSscFileFromVizier( const wxString& strFile, int bRegion );
	// :: BLACKHOLES
	long LoadSdssdr5FileFromVizier( const wxString& strFile, int bRegion );
	long LoadRxeagnFileFromVizier( const wxString& strFile, int bRegion );
	// :: Multiple/double stars
	long LoadWdsFileFromVizier( const wxString& strFile, int bRegion );
	long LoadCcdmFileFromVizier( const wxString& strFile, int bRegion );
	// :: Asteroids
	long LoadOmpFileFromVizier( const wxString& strFile, int bRegion );
	// :: Comets
	long LoadCocomFileFromVizier( const wxString& strFile, int bRegion );

//////////////////////////
// DATA
public:
	CSkyCatalogStars* m_pCatalogStars;
	CSkyCatalogDso* m_pCatalogDso;
	CSkyCatalogRadio* m_pCatalogRadio;
	CSkyCatalogXGamma* m_pCatalogXGamma;
	CSkyCatalogSupernovas* m_pCatalogSupernovas;
	CSkyCatalogBlackholes* m_pCatalogBlackholes;
	CSkyCatalogMStars* m_pCatalogMStars;
	CSkyCatalogAsteroids* m_pCatalogAsteroids;
	CSkyCatalogComets* m_pCatalogComets;

	CUnimapWorker* m_pUnimapWorker;

	wxString m_vectSite[9];
	int m_nSite;
};

#endif
