

#ifndef _LOCATION_DIALOG_H
#define _LOCATION_DIALOG_H

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
	wxID_SELECT_COUNTRY			= 23500,
	wxID_SELECT_REGION			= 23501,
	wxID_SELECT_CITY			= 23502
};

// external class
class CConfigMain;
class CDynamicImgView;
//class CUnimapWorker;

// class:	CEarthmMapDlg
///////////////////////////////////////////////////////
class CEarthmMapDlg : public wxDialog
{
public:
	CEarthmMapDlg( wxWindow *parent, const wxString& strTitle );
	~CEarthmMapDlg( );

	void SetConfig( CConfigMain* pMainConfig, int nCountryId=0, int nRegionId=0, int nCityId=0 );
	void SetEarth( CLocationEarth* pEarth ){ m_pEarth = pEarth; m_pMapView->m_pEarth = pEarth; };

	void LoadData( wxWindow* parent, CUnimapWorker* pWorker );

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

//	CUnimapWorker* m_pWorker;

	wxChoice*	m_pCountry;
	wxChoice*	m_pRegion;
	wxChoice*	m_pCity;
	wxTextCtrl*	m_pLatitude;
	wxTextCtrl*	m_pLongitude;
	wxTextCtrl*	m_pAltitude;

protected:

//	void OnInitDlg( wxInitDialogEvent& pEvent );

	void OnSelectCountry( wxCommandEvent& pEvent );
	void OnSelectRegion( wxCommandEvent& pEvent );
	void OnSelectCity( wxCommandEvent& pEvent );

	void OnUpdateLocation( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()

};

#endif
