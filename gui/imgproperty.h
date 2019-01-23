
#ifndef _IMAGEPROPERTY_H
#define _IMAGEPROPERTY_H

#include "wx/wxprec.h"
#include <wx/notebook.h>
#include <wx/html/htmlwin.h>
#include <wx/html/htmlcell.h>
#include <wx/listctrl.h>
#include <wx/datectrl.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// elynx
#include <elx/core/CoreMacros.h>
#include <elx/math/TransfertFunction.h>
//#include <elx/image/ImageHistogram.h>

// local headers
#include "../image/histogram.h"
#include "../land/earth.h"

using namespace eLynx::Math;
//using namespace eLynx::Image;

// dialg ids
enum
{
	wxID_OBSERVER_NAME		= 16700,
	wxID_OBSERVER_COUNTRY	= 16701,
	wxID_OBSERVER_LATITUDE	= 16702,
	wxID_OBSERVER_LONGITUDE	= 16703,
	wxID_OBSERVER_ALTITUDE	= 16704,
	wxID_OBSERVER_DATE		= 16705,
	wxID_OBSERVER_TIME		= 16706,
	wxID_TARGET_NAME		= 16707,
	wxID_TARGET_DESC		= 16708,
	wxID_TARGET_RA			= 16709,
	wxID_TARGET_DEC			= 16710,
	wxID_TELESCOPE_NAME		= 16711,
	wxID_TELESCOPE_TYPE		= 16712,
	wxID_TELESCOPE_SPEED	= 16713,
	wxID_TELESCOPE_APERTURE	= 16714,
	wxID_TELESCOPE_MOUNT	= 16715,
	wxID_CAMERA_NAME		= 16716,
	wxID_CONDITION_SEEING			= 16717,
	wxID_CONDITION_TRANSPARENCY		= 16718,
	wxID_COND_WIND_SPEED			= 16719,
	wxID_COND_ATM_PRESSURE,
	wxID_COND_TEMPERATURE,
	wxID_COND_TEMP_VARIATION,
	wxID_COND_HUMIDITY,
	wxID_CONDITION_LIGHT_POLUTION,
	wxID_EXPOSURE_TOTAL_TIME,
	wxID_NO_EXPOSURES,
	wxID_EXPOSURE_ISO,
	wxID_REGISTRATION_PROGRAM,
	wxID_STACKING_PROGRAM,
	wxID_STACK_METHOD,
	wxID_PROCESSING_PROGRAM,
	wxID_BUTTON_LOCATION_MY_SITES,
	wxID_BUTTON_LOCATION_MAP,
	wxID_BUTTON_LOCATION_GEOIP,
	wxID_BUTTON_LOCATION_GPS,
	wxID_BUTTON_DATETIME_NOW,
	wxID_BUTTON_DATETIME_EXIF,
	wxID_BUTTON_DATETIME_FILE,
	wxID_BUTTON_OBSERVER_MYSELF,
	wxID_BUTTON_LP_GET,
	wxID_BUTTON_LP_EVAL,
	// add for location
	wxID_OBSERVER_REGION,
	wxID_OBSERVER_CITY,
	wxID_OBSERVER_CITY_OTHER,
	wxID_BUTTON_GET_WEATHER,
	wxID_SELECT_WEATHER_SOURCE,
	// notes panel
	wxID_BUTTON_NOTES_LIST,
	wxID_BUTTON_NOTES_ADD,
	wxID_BUTTON_NOTES_EDIT,
	wxID_BUTTON_NOTES_REMOVE
};

// external classes
class CAstroImage;
class CImageHistogram;
class CHistogramView;
class CObserver;
class CUnimapWorker;

// class:	CImageProperty
///////////////////////////////////////////////////////
class CImageProperty : public wxDialog
{
// methods
public:
    CImageProperty( wxWindow *parent, const wxString& title, CAstroImage* pAstroImage );
	~CImageProperty();

	void SetConfig( CUnimapWorker* pUnimapWorker, CObserver* pObserver, int nUnitsFormat );
	void SetNotesList( );

	void SetHistogram( wxImage& pImage );
	void SetCity( );

	void SetLPLevel( double level );

// data
public:

	CUnimapWorker* m_pUnimapWorker;
	CAstroImage* m_pAstroImage;
	CObserver* m_pObserver;
	//ImageHistogram m_sHistogram;
	CImageHistogram m_rHistogram;
	CHistogramView* m_pImageHistogramView;

	CLocationEarth* m_pEarth;

	wxNotebook *m_pNotebook;
	int m_nObserverPageId;

	// global panels
	wxPanel* m_pPanelExif;
	wxPanel* m_pPanelFits;
	wxPanel* m_pPanelObserver;
	wxPanel* m_pPanelTarget;
	wxPanel* m_pPanelEquipment;
	wxPanel* m_pPanelCondition;
	wxPanel* m_pPanelImaging;
	wxPanel* m_pPanelNotes;
	// sizers
	wxFlexGridSizer* sizerInfoPanel;
	wxGridSizer* sizerInfoPage;
	// info
	wxStaticText*	m_pImageSize;
	wxStaticText*	m_pFileType;
	wxStaticText*	m_pRaCenter;
	wxStaticText*	m_pDecCenter;
	wxStaticText*	m_pFieldWidth;
	wxStaticText*	m_pFieldHeight;
	wxStaticText*	m_pFileTime;
	// exif
	wxListCtrl*		m_pExifData;
	// fits
	wxListCtrl*		m_pFitsData;
	// observer
	wxTextCtrl*		m_pObsName;
	wxChoice*		m_pObsCountry;
	wxChoice*		m_pObsRegion;
	wxChoice*		m_pObsCity;
	wxTextCtrl*		m_pObsLatitude;
	wxTextCtrl*		m_pObsLongitude;
	wxTextCtrl*		m_pObsAltitude;
	wxDatePickerCtrl*	m_pObsDate;
	wxTextCtrl*			m_pObsTime;
	wxButton*		m_pDateTimeExifButton;
	wxButton*		m_pLocMySitesButton;
	// target
	wxTextCtrl*		m_pTargetName;
	wxTextCtrl*		m_pTargetDesc;
	wxTextCtrl*		m_pTargetRa;
	wxTextCtrl*		m_pTargetDec;
	// equipment
	wxTextCtrl*		m_pTelescopeName;
	wxChoice*		m_pTelescopeType;
	wxTextCtrl*		m_pTelescopeFocal;
	wxTextCtrl*		m_pTelescopeAperture;
	wxChoice*		m_pTelescopeMount;
	wxTextCtrl*		m_pCameraName;
	// conditions
	wxButton*		m_pWeatherGetButton;
	wxChoice*		m_pWeatherGetSource;
	wxChoice*		m_pCondSeeing;
	wxChoice*		m_pCondTransp;
	wxTextCtrl*		m_pCondAtmPressure;
	wxTextCtrl*		m_pCondWindSpeed;
	wxTextCtrl*		m_pCondTemp;
	wxTextCtrl*		m_pCondTempVar;
	wxTextCtrl*		m_pCondHumid;
	wxChoice*		m_pCondLp;
	// imaging
	wxTextCtrl*		m_pExpTotalTime;
	wxTextCtrl*		m_pNoOfExp;
	wxTextCtrl*		m_pExpIso;
	wxTextCtrl*		m_pRegProg;
	wxTextCtrl*		m_pStackProg;
	wxChoice*		m_pStackMethod;
	wxTextCtrl*		m_pProcProg;
	// notes
	int m_nNotesTitleSize;
	wxListCtrl*		m_pNotesTable;
	wxButton*		m_pNotesAddButton;
	wxButton*		m_pNotesEditButton;
	wxButton*		m_pNotesRemoveButton;

	////////////
	// flags
	int m_bPanelExif;
	int m_bPanelFits;

// protected
protected:
	void OnNotebookTab( wxNotebookEvent& pEvent );
	void InitEarthLocation( );

	void OnEartMapLocation( wxCommandEvent& pEvent );
	void OnGeoIpLocation( wxCommandEvent& pEvent );
	void OnSelectCountry( wxCommandEvent& pEvent );
	void OnSelectRegion( wxCommandEvent& pEvent );
	void OnSelectCity( wxCommandEvent& pEvent );
	void OnGetWeatherData( wxCommandEvent& pEvent );
	void OnDateTimeSet( wxCommandEvent& pEvent );
	void OnObserverName( wxCommandEvent& pEvent );
	void OnMySitesLocation( wxCommandEvent& pEvent );
	void OnLightPollutionGet( wxCommandEvent& pEvent );
	// notes
	void OnNotesListSelect( wxListEvent& pEvent );
	void OnNotesListAction( wxListEvent& pEvent );
	void OnNotesAction( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
