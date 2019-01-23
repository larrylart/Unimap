////////////////////////////////////////////////////////////////////
// Package:		Catalog Config  header/definition
// File:		catalogconfig.h
// Purpose:		define a config view window for catalogs
//
// Created by:	Larry Lart on 02-07-2007
// Updated by:	Larry Lart on 09-04-2009
//
// Copyright:	(c) 2007-2009 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_CONFIG
#define _CATALOG_CONFIG

#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/notebook.h"
#include <wx/datectrl.h>

// dialog gui enum
enum
{
	// stars
	wxID_STARS_CATALOG_TO_USE		= 11000,
	wxID_BUTTON_DOWNLOAD_STARS,
	wxID_BUTTON_CATVIEW_STARS,
	wxID_STARS_CAT_LOAD_REGION,
	wxID_STARS_CAT_USE_REMOTE,
	wxID_STARS_CATALOG_REMOTE_LOCATIONS,
	// dso
	wxID_DSO_CATALOG_TO_USE,
	wxID_BUTTON_DOWNLOAD_DSO,
	wxID_BUTTON_CATVIEW_DSO,
	wxID_DSO_CAT_LOAD_REGION,
	wxID_DSO_CAT_USE_REMOTE,
	wxID_DSO_CATALOG_REMOTE_LOCATIONS,
	wxID_CHECK_USE_CAT_NGC,
	wxID_CHECK_USE_CAT_IC,
	wxID_CHECK_USE_CAT_MESSIER,
	// radio
	wxID_RADIO_CATALOG_TO_USE,
	wxID_BUTTON_DOWNLOAD_RADIO,
	wxID_BUTTON_CATVIEW_RADIO,
	wxID_RADIO_CAT_LOAD_REGION,
	wxID_RADIO_CAT_USE_REMOTE,
	wxID_RADIO_CATALOG_REMOTE_LOCATIONS,
	// x-ray/gamma
	wxID_XGAMMA_CATALOG_TO_USE,
	wxID_BUTTON_DOWNLOAD_XGAMMA,
	wxID_BUTTON_CATVIEW_XGAMMA,
	wxID_XGAMMA_CAT_LOAD_REGION,
	wxID_XGAMMA_CAT_USE_REMOTE,
	wxID_XGAMMA_CATALOG_REMOTE_LOCATIONS,
	// supernovas
	wxID_SUPERNOVAS_CATALOG_TO_USE,
	wxID_BUTTON_DOWNLOAD_SUPERNOVAS,
	wxID_BUTTON_CATVIEW_SUPERNOVAS,
	wxID_SUPERNOVAS_CAT_LOAD_REGION,
	wxID_SUPERNOVAS_CAT_USE_REMOTE,
	wxID_SUPERNOVAS_CATALOG_REMOTE_LOCATIONS,
	// black-holes
	wxID_BHOLES_CATALOG_TO_USE,
	wxID_BUTTON_DOWNLOAD_BHOLES,
	wxID_BUTTON_CATVIEW_BHOLES,
	wxID_BHOLES_CAT_LOAD_REGION,
	wxID_BHOLES_CAT_USE_REMOTE,
	wxID_BHOLES_CATALOG_REMOTE_LOCATIONS,
	// double/multiple stars
	wxID_MSTARS_CATALOG_TO_USE,
	wxID_BUTTON_DOWNLOAD_MSTARS,
	wxID_BUTTON_CATVIEW_MSTARS,
	wxID_MSTARS_CAT_LOAD_REGION,
	wxID_MSTARS_CAT_USE_REMOTE,
	wxID_MSTARS_CATALOG_REMOTE_LOCATIONS,
	// solar
//	wxID_SOLAR_CATALOG_TO_USE,
//	wxID_BUTTON_DOWNLOAD_SOLAR,
//	wxID_SOLAR_CAT_LOAD_REGION,
//	wxID_SOLAR_CAT_USE_REMOTE,
//	wxID_SOLAR_CATALOG_REMOTE_LOCATIONS,
	// EXOPLANETS
	wxID_EXP_CATALOG_TO_USE,
	wxID_BUTTON_DOWNLOAD_EXP,
	wxID_BUTTON_CATVIEW_EXP,
	wxID_EXP_CAT_LOAD_REGION,
	wxID_EXP_CAT_USE_REMOTE,
	wxID_EXP_CATALOG_REMOTE_LOCATIONS,
	// ASTEROIDS
	wxID_AST_CATALOG_TO_USE,
	wxID_BUTTON_DOWNLOAD_AST,
	wxID_BUTTON_CATVIEW_AST,
	wxID_AST_CAT_LOAD_REGION,
	wxID_AST_CAT_USE_REMOTE,
	wxID_AST_CATALOG_REMOTE_LOCATIONS,
	// COMETS
	wxID_COM_CATALOG_TO_USE,
	wxID_BUTTON_DOWNLOAD_COM,
	wxID_BUTTON_CATVIEW_COM,
	wxID_COM_CAT_LOAD_REGION,
	wxID_COM_CAT_USE_REMOTE,
	wxID_COM_CATALOG_REMOTE_LOCATIONS,
	// AES
	wxID_AES_CATALOG_TO_USE,
	wxID_BUTTON_DOWNLOAD_AES,
	wxID_BUTTON_CATVIEW_AES,
	wxID_AES_CAT_USE_TIMEFRAME,
	wxID_AES_CAT_LOAD_REGION,
	wxID_AES_CAT_USE_REMOTE,
	wxID_AES_CATALOG_REMOTE_LOCATIONS,
	wxID_AES_CATALOG_SAT_OBJECT_CAT,
	wxID_AES_CAT_USE_DATE_RANGE_START,
	wxID_AES_CAT_USE_DATE_RANGE_END

};

// stuct type :: panel def elements
typedef struct
{
	int cat_type;

	// gui elements
	wxStaticText*	pRecords;
	wxStaticText*	pVersion;
	wxStaticText*	pDate;
	wxStaticText*	pAuthor;
	wxStaticText*	pOrganization;
	// properties
	wxChoice*		pCatalogToUse;
	wxButton*		pCatalogDownload;
	wxButton*		pCatalogView;

	wxCheckBox*		pCatLoadTimeFrame;
	wxCheckBox*		pCatLoadRegion;

	///////////////////
	// remote
	wxCheckBox*		pCatUseRemote;
	wxChoice*		pCatRemoteLocation;
	wxCheckBox*		pCatRemoteUpdateLocal;

	//////////////
	// SECTION :: FILTERS
	wxChoice*		pCatObjCat;
	wxChoice*		pCatObjType;
	wxCheckBox*		pCatDontUseType;
	// date range
	wxCheckBox*			pCatRecUseDateStart;
	wxDatePickerCtrl*	pCatRecDateStart;
	wxCheckBox*			pCatRecUseDateEnd;
	wxDatePickerCtrl*	pCatRecDateEnd;

	// details
	wxStaticBitmap* pFootprint;
	wxTextCtrl*		pDesc;
	// sizers
	wxFlexGridSizer* sizerHeaderA;
	wxFlexGridSizer* sizerHeaderB;
	// cat id maps
	unsigned int vectMapCatalogId[20];
	int nMapCatalogId;
	// query source maps
	int vectMapSourceId[20];
	int nMapSourceId;
	// flags
	int bSelectionChanged;

} DefPanelCatalogType;

// external classes
class CSky;
class CUnimapWorker;
class CAstroImage;

// class:	CCatalogConfig
///////////////////////////////////////////////////////
class CCatalogConfig : public wxDialog
{
// methods
public:
    CCatalogConfig( wxWindow *parent, const wxString& title );
	~CCatalogConfig();

	void SetConfig( CSky* pSky, CConfigMain* pMainConfig, CAstroImage* pAstroImage );

	// :: catalog index stuff
	int GetCatIndexFromSelection( int nCatType );
	int GetCatIdFromSelection( int nCatType );
	int GetSelectedCatId( int nPanelId, int nCatId );

	void SetUseRemoteCat( int nCatType, int bRemote );
	void SetUseFilterDateRange( int nCatType, int nStartEnd, int bUse );

	int GetQuerySourceId( int nCatType );
	int GetQuerySourceSelectionId( int nCatType, int nQuerySourceId );

	int GetPanelIdByCatalogType( int nCatType );
	DefPanelCatalogType* GetPanelByCatalogType( int nCatType );

///////////////////
// DATA
public:
	CSky* m_pSky;
	CUnimapWorker* m_pUnimapWorker;
	CAstroImage* m_pAstroImage;

	wxBitmap m_bmpFootprint;
	wxBitmap m_bmpNoneFootprint;
	wxNotebook *m_pNotebook;

	// catalog panels
	DefPanelCatalogType	m_vectCatalogPanel[20];
	int m_nCatalogPanel;

	//////////////////
	// CUSTOM ELEMENTS
	// :: STARS
	wxTextCtrl*		m_pStarsCatMaxMagLimit;
	// :: DSO
	wxCheckBox*		m_pUseMessierCat;
	wxCheckBox*		m_pUseNgcCat;
	wxCheckBox*		m_pUseIcCat;

private:
	void AddCatalogPanel( int nCatType );

	// a general cat select...
	void CheckCatalogByType( int nCatType );

	void OnSetCatalog( wxCommandEvent& pEvent );

	void OnCatalogDownload( wxCommandEvent& pEvent );
	void OnCatalogView( wxCommandEvent& pEvent );

	void OnUseRemote( wxCommandEvent& pEvent );
	void OnUseDateRange( wxCommandEvent& pEvent );

	void OnValidate( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
