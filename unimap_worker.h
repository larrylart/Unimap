////////////////////////////////////////////////////////////////////
// Package:		worker definition
// File:		worker.h
// Purpose:		manage software hardware powercycle in sync
//
// Created by:	Larry Lart on 09-Dec-2006
// Updated by:	
//
// Copyright:	(c) 2006-2007 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _UNIMAP_WORKER_H
#define _UNIMAP_WORKER_H

// include other local headers
#include "worker.h"

//////////////
// define actions - start with 1 0 = none already defined
#define THREAD_ACTION_DETECT_START		1
//#define THREAD_ACTION_DETECT_STOP		2
#define THREAD_ACTION_DETECT_PAUSE		3
#define THREAD_ACTION_DETECT_RESTART	4
#define THREAD_ACTION_MATCH_START		5
//#define THREAD_ACTION_MATCH_STOP		6
#define THREAD_ACTION_MATCH_PAUSE		7
#define THREAD_ACTION_MATCH_RESTART		8
#define THREAD_ACTION_CATALOG_LOAD		9
#define THREAD_ACTION_CATALOG_UNLOAD	10
#define THREAD_ACTION_IMAGE_LOAD		11

//#define THREAD_ACTION_DETECT_AND_MATCH_START	11
//#define THREAD_ACTION_DETECT_AND_MATCH_STOP	12

// timed command for now reserve 50-99
#define THREAD_ACTION_TIMED_WEATHER_ONLINE		50

// some more custom commands
#define THREAD_ACTION_UNIMAP_LOAD_ONSTARTUP	100
#define THREAD_ACTION_LOAD_OBJECT_DATA		101
#define THREAD_ACTION_LOAD_OBJECT_IMAGE		102
// load object list details
#define THREAD_ACTION_LOAD_OBJECT_LIST_DETAILS	103
// set thumbnails for group
#define THREAD_ACTION_SET_GROUP_THUMBNAILS	104
// group actions
#define THREAD_ACTION_GROUP_DETECT			105
#define THREAD_ACTION_GROUP_MATCH			106
#define THREAD_ACTION_GROUP_REGISTER		107
#define THREAD_ACTION_GROUP_ALIGN			108
#define THREAD_ACTION_GROUP_STACK			109
#define THREAD_ACTION_GROUP_DYNAMICS_SEARCH	110
#define THREAD_ACTION_LOAD_EARTH_MAP_DATA	111
#define THREAD_ACTION_GET_HIST_WEATER_DATA	112
#define THREAD_ACTION_IMAGE_3D_COLORMAP		113
#define THREAD_ACTION_IMAGE_3D_DISTGRID		114
#define THREAD_ACTION_IMAGE_3D_SPACEDIST	115
// other
#define THREAD_ACTION_LP_MAP_PROCESS		150
// ONLINE handlers
#define THREAD_ACTION_UPLOAD_IMAGE						160
#define THREAD_ACTION_UPLOAD_GROUP						161
#define THREAD_ACTION_UPLOAD_CUSTOM						162
#define THREAD_ACTION_BROWSE_ONLINE_IMAGES				163
#define THREAD_ACTION_BROWSE_ONLINE_GET_IMAGE_PREVIEW	164
#define THREAD_ACTION_BROWSE_ONLINE_DOWNLOAD_IMAGE		165
// catalog stuff
#define THREAD_ACTION_CATALOG_DOWNLOAD		200
// tools :: weather current/forecast 
#define THREAD_ACTION_WEATHER_FORECAST_ALL_GET			300
#define THREAD_ACTION_WEATHER_FORECAST_DAY_GET			301
#define THREAD_ACTION_WEATHER_SEEING_PREDICT_GET		302
#define THREAD_ACTION_WEATHER_SEEING_PREDICT_GET_CUSTOM	303
#define THREAD_ACTION_ENV_LIGHT_POLLUTION_LOAD			304
#define THREAD_ACTION_ENV_OBSERVATORIES_LOAD			305
#define THREAD_ACTION_NEWS_FETCH_HEADLINES				306
#define THREAD_ACTION_WEATHER_SATELLITE_IMAGE			307
#define THREAD_ACTION_WEATHER_SUN_VIEW_IMAGE			308
#define THREAD_ACTION_WEATHER_FORECAST_ALL_GET_CUSTOM	309
#define THREAD_ACTION_PROCCESS_TEMPORARY_FOLDERS		310
// services
#define THREAD_ACTION_SERVICES_IMAGE_PLATESOLVE			500

///////////
// external classes
class CUnimap;
class CGUIFrame;
//class CLogger;
class CSky;
class CAstroImage;
class CSextractor;
class CStarMatch;
//class CStarViewDlg;
class CConfigMain;
class CLocationEarth;
class CObserver;
class CObserverSite;
class CImageDb;
class CUnimapService;

//////////////////////////////////////////////////
// class:	CUnimapWorker
/////////////////////////////////////////////////
class CUnimapWorker : public CWorker
{
// methods
public:
	CUnimapWorker( CGUIFrame *pFrame, CLogger *pLogger );
	~CUnimapWorker();

	void SetConfig( CConfigMain* pMainConfig );

	virtual void OnExit();

	// remote action methods
//	void DoStartDetectionAndMatching( CAstroImage* pAstroImage );
//	void DoStartDetection( CAstroImage* pAstroImage );
//	void DoStartMatching( CAstroImage* pAstroImage );
//	void DoStopDetection( );

	// local action methods
	int StartDetection( );
	int StopDetection( );
//	int StartMatching( );
	int LoadObjectsDetails( CAstroImage* pAstroImage );

	// runtime check handlers
	int CheckForCommands( );
	int CheckForTimedCommands( );

// data
public:

	wxChoice*		m_pValueSelect;
	CConfigMain*	m_pMainConfig;

	// object i need to manage
//	CStarViewDlg*	m_pStarDlg;

	CSky*			m_pSky;
	CAstroImage*	m_pAstroImage;
	CSextractor*	m_pDetect;
	CStarMatch*		m_pMatch;
	CLocationEarth*	m_pEarth;
	CObserver*		m_pObserver;
	CImageDb*		m_pImageDb;
	CUnimapService*	m_pUnimapService;

	////////////
	// action flags
	int m_nActSaveOnDetect;
	int m_nActSaveOnMatch;
	int m_nActSaveOnDetails;
	int m_nActSaveOnAutos;
//	int m_nActDetailsFetchOnObjFind;

// private data
private:

};

#endif


