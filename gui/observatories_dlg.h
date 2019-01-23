

#ifndef _OBSERVATORIES_DLG_H
#define _OBSERVATORIES_DLG_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// local headers
#include "earth_view.h"
#include "../land/earth.h"

// set a list of ids
enum
{
	wxID_OBS_FILTER_SELECT_KIND			= 23500,
	wxID_OBS_FILTER_SELECT_TYPE,
	wxID_OBS_FILTER_FIND_ENTRY,
	wxID_OBS_FILTER_FIND_BUTTON,
	wxID_OBS_CONTACT_BUTTON,
	wxID_OBS_INFO_WEBSITE_BUTTON,
	wxID_OBS_INFO_WEBREF_BUTTON,
	wxID_OBS_PHOTO_BACK,
	wxID_OBS_PHOTO_NEXT,
	wxID_OBS_PHOTO_VIEW
};

// external class
class CConfigMain;
class CDynamicImgView;
class CObservatories;

// class:	CObservatoriesDlg
///////////////////////////////////////////////////////
class CObservatoriesDlg : public wxDialog
{
public:
	CObservatoriesDlg( wxWindow *parent, const wxString& strTitle );
	~CObservatoriesDlg( );

	void SetConfig( );
	void LoadData( );

	void SetEarth( CLocationEarth* pEarth ){ m_pEarth = pEarth; m_pMapView->m_pEarth = pEarth; };

	void SetObservatory( int nId );
	void SetObservatoryPhoto( int nId, int nPhotoId );

	void SetCountries( int nCountryId=0 );
	void SetRegions( int nSelect=0 );
	void SetCities( int nSelect=0 );
	void SetCity( );
	void SetCustomPositionFromString( wxString& strLat, wxString& strLon, wxString& strAlt );
	void SetCustomPosition( double lat, double lon, double alt );

	void SetLocByName( wxString& strCountry, wxString& strProvince, wxString& strCity, int bCity );

// public data
public:
	CEarthMapView* m_pMapView;
	CLocationEarth* m_pEarth;
	CObservatories* m_pObservatories;

	int m_nSelectedObs;
	int m_nCurrentPhoto;

	wxChoice*	m_pFilterKind;
	wxChoice*	m_pFilterType;
	wxTextCtrl*	m_pFindEntry;
	wxButton*	m_pFindButton;
	wxTextCtrl*	m_pDescText;

	// photo
	wxButton*		m_pPhotoBack;
	wxButton*		m_pPhotoNext;
	wxButton*		m_pPhotoView;
	wxStaticBitmap* m_pPhoto;
	wxStaticText*	m_pPhotoText;

	// instruments
	wxListBox*	m_pInstruments;

	// info
	wxStaticText*	m_pOrgLabel;
	wxStaticText*	m_pOrgValue;
	wxStaticText*	m_pCodeLabel;
	wxStaticText*	m_pCodeValue;	
	wxStaticText*	m_pWeatherLabel;
	wxStaticText*	m_pWeatherValue;
	wxStaticText*	m_pBuiltLabel;
	wxStaticText*	m_pBuiltValue;


	// contact 
	wxButton*	m_pContactButton;
	wxChoice*	m_pContactAddr;

	// web site
	wxButton*		m_pWebSiteButton;
//	wxStaticText*	m_pWebSiteText;

	// web reference
	wxButton*	m_pWebRefButton;
	wxChoice*	m_pWebRefUrl;


protected:
	void OnContact( wxCommandEvent& pEvent );
	void OnViewWebSite( wxCommandEvent& pEvent );
	void OnViewWebRef( wxCommandEvent& pEvent );
	void OnSetPhoto( wxCommandEvent& pEvent );
	void OnPhotoViewFull( wxCommandEvent& pEvent );
	void OnUpdateObsLocation( wxCommandEvent& pEvent );

	void OnSelectCountry( wxCommandEvent& pEvent );
	void OnSelectRegion( wxCommandEvent& pEvent );
	void OnSelectCity( wxCommandEvent& pEvent );

	void OnUpdateLocation( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()

};

#endif
