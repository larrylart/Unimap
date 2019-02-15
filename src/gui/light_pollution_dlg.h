////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////


#ifndef _LIGHT_POLLUTION_DLG_H
#define _LIGHT_POLLUTION_DLG_H

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
	wxID_LIGHT_POLLUTION_SITE_SELECT	= 23500,
	wxID_SELECT_COUNTRY,
	wxID_SELECT_REGION,
	wxID_SELECT_CITY
};

// external class
//class CConfigMain;
class CObserver;
class CObserverSite;
class CUnimapWorker;

// class:	CLightPollutionDlg
///////////////////////////////////////////////////////
class CLightPollutionDlg : public wxDialog
{
public:
	CLightPollutionDlg( wxWindow *parent, const wxString& strTitle, CUnimapWorker* pUnimapWorker );
	~CLightPollutionDlg( );

	void SetConfig( CObserver* pObserver, int siteid );
	void LoadData( );
	void SetSite( int site );

	void SetEarth( CLocationEarth* pEarth ){ m_pEarth = pEarth; m_pMapView->m_pEarth = pEarth; };

	void SetCountries( int nCountryId=0 );
	void SetRegions( int nSelect=0 );
	void SetCities( int nSelect=0 );
	void SetCity( );
	void SetCustomPositionFromString( wxString& strLat, wxString& strLon, wxString& strAlt );
	void SetCustomPosition( double lat, double lon, double alt );

	void SetLocByName( wxString& strCountry, wxString& strProvince, wxString& strCity, int bCity );

// public data
public:
	CLocationEarth* m_pEarth;
	CObserver*		m_pObserver;
	CObserverSite*	m_pSite;
	CUnimapWorker*	m_pUnimapWorker;

	CEarthMapView* m_pMapView;
	int m_nSiteId;

	wxChoice*		m_pSiteSelect;
	wxStaticText*	m_pLightPollutionLevel;

	wxChoice*	m_pCountry;
	wxChoice*	m_pRegion;
	wxChoice*	m_pCity;
	wxTextCtrl*	m_pLatitude;
	wxTextCtrl*	m_pLongitude;
	wxTextCtrl*	m_pAltitude;

protected:
	void OnChangeSite( wxCommandEvent& pEvent );
	void OnSelectCountry( wxCommandEvent& pEvent );
	void OnSelectRegion( wxCommandEvent& pEvent );
	void OnSelectCity( wxCommandEvent& pEvent );

	void OnUpdateLocation( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()

};

#endif
