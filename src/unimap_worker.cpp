////////////////////////////////////////////////////////////////////
// Package:		worker implementation
// File:		worker.cpp
// Purpose:		manage detetction/maching and catalog loading
//
// Created by:	Larry Lart on 20/07/2007
// Updated by:	Larry Lart on 03/04/2010
//
// Copyright:	(c) 2004-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

// system headers
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include "wx/wxprec.h"
#include "wx/thread.h"
#include <wx/string.h>
#include <wx/regex.h>
#include <wx/sound.h>
#include <wx/process.h>
#include <wx/utils.h>
#include <wx/app.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// custom headers
#ifdef WIN32
#include "util/func.h"
#endif

//#include "logger/logger.h"
#include "gui/frame.h"
#include "gui/imgproperty.h"
#include "gui/astro_img_view.h"
#include "gui/waitdialog.h"
#include "gui/starviewdlg.h"
#include "gui/online_browse_dlg.h"
#include "unimap.h"
#include "sky/sky.h"
#include "sky/sky_finder.h"
#include "sky/catalog_stars.h"
#include "sky/catalog_stars_names.h"
#include "sky/catalog_dso.h"
#include "sky/catalog_dso_ngc.h"
#include "sky/catalog_dso_ic.h"
#include "sky/catalog_dso_messier.h"
#include "sky/catalog_dso_names.h"
#include "sky/simbad.h"
#include "sky/constellations.h"
#include "sky/objdetails.h"
#include "image/astroimage.h"
#include "image/imagegroup.h"
#include "image/imagedb.h"
#include "match/starmatch.h"
#include "config/config.h"
#include "config/mainconfig.h"
#include "land/earth.h"
#include "online/unimap_online.h"
#include "observer/observer.h"
#include "observer/observer_site.h"
#include "weather/weather.h"
#include "gui/light_pollution_dlg.h"
#include "gui/observatories_dlg.h"
#include "gui/imgview3dcolormap.h"
#include "util/files_manager.h"
#include "unimap_service.h"
// 3d dist grid
#include "graphics/tps/linalg3d.h"
#include "graphics/tps/ludecomposition.h"
#include "gui/imgview3ddistmap.h"
#include "gui/imgview3dspacedist.h"
#include "gui/weather_satvis_dlg.h"
#include "gui/sun_view_dlg.h"
#include "util/apps_manager.h"
// news
#include "news/news.h"
#define NOCTYPES
#include "proc/sextractor.h"
#undef NOCTYPES

// main header include
#include "unimap_worker.h"

//		***** CLASS CUnimapWorker *****
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CUnimapWorker
// Purpose:	Initialize my worker thread
// Input:	pointer to reference frame
// Output:	nothing	
////////////////////////////////////////////////////////////////////
CUnimapWorker::CUnimapWorker( CGUIFrame *pFrame, CLogger *pLogger ) : CWorker( pFrame, pLogger )
{ 
	// set my processing object to null
	m_pSky = NULL;
	m_pAstroImage = NULL;
	m_pDetect = NULL;
	m_pMatch = NULL;
	m_pUnimap =  NULL;

	// reset action flags
	m_nActSaveOnDetect = 0;
	m_nActSaveOnMatch = 0;
	m_nActSaveOnDetails = 0;
	m_nActSaveOnAutos = 0;
//	m_nActDetailsFetchOnObjFind = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	On Exit
// Class:	CUnimapWorker
// Purpose:	do soemthing on exit
// Input:	nothing
// Output:	nothing	
////////////////////////////////////////////////////////////////////
CUnimapWorker::~CUnimapWorker( )
{

}

////////////////////////////////////////////////////////////////////
// Method:	On Exit
// Class:	CUnimapWorker
// Purpose:	do soemthing on exit
// Input:	nothing
// Output:	nothing	
////////////////////////////////////////////////////////////////////
void CUnimapWorker::OnExit( )
{
	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CUnimapWorker
// Purpose:	set my action configuration
// Input:	pointe config object
// Output:	nothing	
////////////////////////////////////////////////////////////////////
void CUnimapWorker::SetConfig( CConfigMain* pMainConfig )
{
	// check if valid reference
	if( !pMainConfig ) return;
	m_pMainConfig = pMainConfig;

	int nVarInt = 0;
	int bChange = 0;

	// SECTION :: ACTION
	//////////////////////////////////
	// save on detect 
	if( pMainConfig->GetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_DETECTION, nVarInt) == 0 )
	{
		pMainConfig->SetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_DETECTION, 1 );
		bChange = 1;
	} else
	{
		m_nActSaveOnDetect = nVarInt;
	}
	// save on match 
	if( pMainConfig->GetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_MATCHING, nVarInt) == 0 )
	{
		pMainConfig->SetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_MATCHING, 1 );
		bChange = 1;
	} else
	{
		m_nActSaveOnMatch = nVarInt;
	}
	// save on details 
	if( pMainConfig->GetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_DETAILS, nVarInt) == 0 )
	{
		pMainConfig->SetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_DETAILS, 1 );
		bChange = 1;
	} else
	{
		m_nActSaveOnDetails = nVarInt;
	}
	// save on autos 
	if( pMainConfig->GetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_AUTOS, nVarInt) == 0 )
	{
		pMainConfig->SetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_AUTOS, 1 );
		bChange = 1;
	} else
	{
		m_nActSaveOnAutos = nVarInt;
	}

	// check if default updates then save config
	if( bChange > 0 ) pMainConfig->Save( );

	return;
}

/*
////////////////////////////////////////////////////////////////////
// Method:		DoStopDetection
// Class:		CUnimapWorker
// Purpose:		stop detection process
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CUnimapWorker::DoStopDetection( )
{
//	m_bPauseLoop = 1;
	DefCmdAction cmd;
	cmd.id = THREAD_ACTION_DETECT_STOP;

	SetAction( cmd );

	return;
}
*/
/*
////////////////////////////////////////////////////////////////////
// Method:		DoStartDetectionAndMatching
// Class:		CUnimapWorker
// Purpose:		set flag to start the detection and matching process 
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CUnimapWorker::DoStartDetectionAndMatching( CAstroImage* pAstroImage )
{
	if( !pAstroImage ) return;
	
	m_pAstroImage = pAstroImage;
	DefCmdAction cmd;
	cmd.id = THREAD_ACTION_DETECT_AND_MATCH_START;

	SetAction( cmd );

	return;
}
*/
/*
////////////////////////////////////////////////////////////////////
// Method:		DoStartDetection
// Class:		CUnimapWorker
// Purpose:		set flag to start the detection process
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CUnimapWorker::DoStartDetection( CAstroImage* pAstroImage )
{
	if( !pAstroImage ) return;

//	m_bPauseLoop = 0;
//	m_bIsPaused = 0;
	
	m_pAstroImage = pAstroImage;
	DefCmdAction cmd;
	cmd.id = THREAD_ACTION_DETECT_START;

	SetAction( cmd );

	return;
}

*/

////////////////////////////////////////////////////////////////////
// Method:		StartDetection
// Class:		CUnimapWorker
// Purpose:		start detection process in this thread
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CUnimapWorker::StartDetection( )
{
//	return( 0 );

//	char strMsg[255];

	wxMutexGuiEnter();
	m_pFrame->m_pImgView->SetStars( m_pAstroImage, 0 );
	wxMutexGuiLeave();

	Log( wxT("INFO :: Object detection has started...") );

	// free data from image - for now just set stars to zero
	m_pAstroImage->ClearDetected( );

	// create and initialize my star detection engine
	m_pDetect = new CSextractor( );
//	m_pDetect->m_pImgView = m_pFrame->m_pImgView;
//	m_pDetect->m_pLogger = m_pLogger;
	m_pDetect->m_pUnimapWorker = this;
	// set the config pointers
	m_pDetect->m_pDetectConfig = m_pFrame->m_pDetectConfig;
	m_pDetect->m_pMainConfig = m_pFrame->m_pMainConfig;
//	m_pFrame->m_pSextractor = m_pStarFinder;

	m_pDetect->Init( );
	m_pDetect->Make( m_pAstroImage );

	m_pAstroImage->ProcessStars( );

	///////////////////////
	// here we set that the image was hanged 
	m_pAstroImage->m_bIsChanged = 1;
	m_pAstroImage->m_bIsDetected = 1;
	m_pAstroImage->m_nImgSourceType = m_pDetect->prefs.detect_type;

	delete( m_pDetect );
	m_pDetect = NULL;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		StopDetection
// Class:		CUnimapWorker
// Purpose:		stop detection process in this thread
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CUnimapWorker::StopDetection( )
{
	char strMsg[255];
	int nStatus = -1;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		LoadObjectsDetails
// Class:		CUnimapWorker
// Purpose:		load object details
// Input:		nothing
// Output:		status ?
////////////////////////////////////////////////////////////////////
int CUnimapWorker::LoadObjectsDetails( CAstroImage* pAstroImage )
{
	// init allocate details for image
	pAstroImage->InitDetails( );

	CSimbad* pSimbad = new CSimbad( pAstroImage, this, m_pSky );
	// get image stars
	pSimbad->GetBasicDetailsForObjList( pAstroImage->m_vectStar, pAstroImage->m_nStar, pAstroImage->m_vectStarDetail );
	// get cat stars
	pSimbad->GetBasicDetailsForObjList( pAstroImage->m_vectCatStar, pAstroImage->m_nCatStar, pAstroImage->m_vectCatStarDetail );
	delete( pSimbad );

	/////////////
	// get data for dso
	CSkyObjDetails* pDsoDetails = new CSkyObjDetails( pAstroImage, this );
	pDsoDetails->LoadDsoDistances( pAstroImage->m_vectCatDso, pAstroImage->m_nCatDso, pAstroImage->m_vectCatDsoDetail );
	delete( pDsoDetails );

	// calculate iamge props and distances
	pAstroImage->CalcPropFromDetails( );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		CheckForTimedCommands
// Class:		CUnimapWorker
// Purpose:		check for timed action commands
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CUnimapWorker::CheckForTimedCommands( )
{
	if( m_vectTimedActionQueue.size() == 0 ) return( 0 );
	int i=0;

	m_pMutex->Lock();

	// get time now
	double nTimeDiff=0;
	double nTime = GetTime();

	// now, for every timed command check if time run
	for( i=0; i<m_vectTimedActionQueue.size(); i++ )
	{
		// if interval timed
		if( m_vectTimedActionQueue[i].time_type == 0 )
		{
			// calculate time diff
			nTimeDiff = nTime-m_vectTimedActionQueue[i].last_time;
			// check if over
			if( nTimeDiff >= m_vectTimedActionQueue[i].time_int )
			{
				// run my command - special commands here ??
				if( m_vectTimedActionQueue[i].id == THREAD_ACTION_TIMED_WEATHER_ONLINE )
				{
					int nSiteId = m_vectTimedActionQueue[i].vectInt[0];
					m_pObserver->m_vectSite[nSiteId]->GetCurrentWeatherWithHist( );

					// now send event that is done
					SendEventType( m_vectTimedActionQueue[i].handler, wxID_INSTR_CONDITIONS_UPDATE ); 
				}

				// update last run
				m_vectTimedActionQueue[i].last_time = nTime;
			}
		}
	}

	m_pMutex->Unlock( );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		CheckForCommands
// Class:		CUnimapWorker
// Purpose:		check for action commands
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CUnimapWorker::CheckForCommands( )
{
	if( m_vectActionQueue.size() == 0 ) return( 0 );
	char strMsg[255];

	m_nActionCmd = m_vectActionQueue.front().id;

	// actions
	if( m_nActionCmd == THREAD_ACTION_IMAGE_LOAD )
	{
		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;

		int load_type = (int) m_vectActionQueue[0].vectInt[0];
		int group_id = (int) m_vectActionQueue[0].vectInt[1];
		int image_id = (int) m_vectActionQueue[0].vectInt[2];
		int bCalDistGrid = (int) m_vectActionQueue[0].vectInt[3];
		// previous image
		CAstroImage* pCurAstroImage = (CAstroImage*) m_vectActionQueue[0].vectObj[0];

		// if current image allocated save/free
		if( pCurAstroImage )
		{
			// todo: do save/free by spliting the uload image data from guiframe
			//pCurAstroImage->SaveProfile(0);
			//pCurAstroImage->FreeData( );
		}

		// get image for database
		m_pImageDb->SelectImageFromGroup( group_id, image_id );
		CAstroImage* pAstroImage = m_pImageDb->m_pCurrentImage;

		// get image data
		pAstroImage->Load();

		// if image is matched copy trans to starmatch object
		if( pAstroImage->m_bIsMatched == 1 )
		{
			m_pMatch->CopyTrans( pAstroImage->m_pBestTrans, m_pMatch->m_pBestTrans );
			// do also the last trans ... that's most used
			m_pMatch->CopyTrans( pAstroImage->m_pBestTrans, m_pMatch->m_pLastTrans );
			// do inverse trans
			m_pMatch->CopyTrans( pAstroImage->m_pInvTrans, m_pMatch->m_pInvTrans );
		}
		// if set calculate dist grid
		if( bCalDistGrid ) pAstroImage->CalcDistGrid( );


		// also set flag from here
//		m_pFrame->m_bSlideLoading = 0;
		// instead set astro image pointer from here so it will get deleted 
		m_pFrame->m_pAstroImage = pAstroImage;

		// now send event that is done by type
		if( load_type == 0 )
			// notify wait dialog to close
			SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS );
		else if( load_type == 1 )
			// notify frame that image was loaded
			SendEventType( m_vectActionQueue[0].handler, wxID_THREAD_ASTROIMAGE_LOADED );

		// reset action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_DETECT_START )
	{
//		m_pUnimap->ShowWaitDialog( wxT("Detection Process"), wxT("Detecting objects in the image...") );

		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;

		m_pAstroImage = (CAstroImage*) m_vectActionQueue[0].vectObj[0];

		// Start detection
		StartDetection( );

		/////////////////
		// check if to save - todo: here use flags from cmd
		if( m_nActSaveOnDetect ) m_pAstroImage->SaveProfile( 0 );

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_MATCH_START )
	{
//		m_pUnimap->ShowWaitDialog( wxT("Matching Process"), 
//									wxT("Initializing matching engine..."), WAIT_DIALOG_TYPE_MATCH );
		// wait a little for the dialog to get set
//		while( m_pFrame->m_pWaitDialogMatch == NULL ) wxThread::Sleep( 70 ); 

		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;

		m_pAstroImage = (CAstroImage*) m_vectActionQueue[0].vectObj[0];
		m_pSky->m_pSkyFinder->m_pWaitDialogMatch = (CWaitDialogMatch*) m_vectActionQueue[0].vectObj[1];

		// Start matching 
		m_pSky->SolveAstroImage( m_pAstroImage, this );

		/////////////////
		// check if to save - don't save now if also need to fetch find
		if( m_nActSaveOnMatch && !m_pMainConfig->m_bMatchFetchDetails ) m_pAstroImage->SaveProfile( 0 );

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 
		m_pSky->m_pSkyFinder->m_pWaitDialogMatch = NULL;

		// a small delay
		wxThread::Sleep( 300 );

		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_CATALOG_LOAD )
	{
		// TODO: load catalog on thread...

	} else if( m_nActionCmd == THREAD_ACTION_CATALOG_DOWNLOAD )
	{
//		m_pUnimap->ShowWaitDialog( wxT("Download Catalog Data"), 
//									wxT("Fetching data from remote location ..."), WAIT_DIALOG_TYPE_PROGRESS );
		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;

		// get catalog id and source id
		int nCatType = (int) m_vectActionQueue[0].vectInt[0];
		int nCatId = (int) m_vectActionQueue[0].vectInt[1];
		int nCatSrcId = (int) m_vectActionQueue[0].vectInt[2];

		// reset flag
		m_bCurrentTaskSetToBreak = 0;

		// download catalog
		m_pSky->DownloadCatalog( nCatType, nCatId, nCatSrcId );

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// close dialog
//		CloseWaitDialog( );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_UNIMAP_LOAD_ONSTARTUP )
	{
		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;
		CAppsManager* pAppsMgr = (CAppsManager*) m_vectActionQueue[0].vectObj[0];
		CUnimapOnline* pUnimapOnline = (CUnimapOnline*) m_vectActionQueue[0].vectObj[1];

		SetWaitDialogMsg( wxT("Initialize image database ...") );
		m_pFrame->m_pImageDb->Init( );

		SetWaitDialogMsg( wxT("Search for applications ...") );
		pAppsMgr->Init( );

/**** this to load only when needed */

		SetWaitDialogMsg( wxT("Loading DSO common names ...") );
		// load basic data for catalogs
		m_pSky->m_pCatalogDsoNames->LoadDsoNames( this );
		// star common names
		SetWaitDialogMsg( wxT("Loading star common names ...") );
		m_pSky->m_pCatalogStarNames->LoadStarNames( this );

		/////////////////////////////////////
		// larry addition :: load sao ????
		// star common names
		SetWaitDialogMsg( wxT("Loading star sao common names ...") );
		m_pSky->m_pCatalogStarNames->LoadStripedSaoNames( this );
		m_pSky->m_pCatalogStarNames->LoadTychoNames( this );

		// constelations common names
		SetWaitDialogMsg( wxT("Loading constellations common names ...") );
		m_pSky->m_pConstellations->LoadConstNames( unMakeAppPath(wxT("data/sky_obj_cnames/common_const_names.txt")) );

		// messier
		SetWaitDialogMsg( wxT("Loading Messier catalog ...") );
		m_pSky->m_pCatalogDsoMessier->Load( );

		// ngc
		SetWaitDialogMsg( wxT("Loading NGC catalog ...") );
		m_pSky->m_pCatalogDsoNgc->Load( CATALOG_ID_NGC );

		// ic
		SetWaitDialogMsg( wxT("Loading IC catalog ...") );
		m_pSky->m_pCatalogDsoIc->Load( CATALOG_ID_IC );

		// constelations structures
		SetWaitDialogMsg( wxT("Loading constellations structures ...") );
		m_pSky->m_pConstellations->LoadConstellations( SKY_CULTURE_WESTERN );

/* end of not to load section at startup??? */

		// check for updates - 86400 - daily - half 43200 
		time_t time_now = time(NULL);
		long tDiff = abs( (long) time_now - (long) pUnimapOnline->m_nLastAppUpdateTime );
		if( !m_pMainConfig->m_strAppId.IsEmpty() && tDiff > 86400 )
		{
			SetWaitDialogMsg( wxT("Check for updates ...") );
			pUnimapOnline->CheckAppUpdates( );
		}

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// close dialog
		//CloseWaitDialog( );

		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_LOAD_OBJECT_DATA )
	{
		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;

		CStarViewDlg* pStarDlg = (CStarViewDlg*) m_vectActionQueue[0].vectObj[0];

		// get data 
		pStarDlg->GetObjectData( );

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// reset action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_LOAD_OBJECT_IMAGE )
	{
		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;

		CStarViewDlg* pStarDlg = (CStarViewDlg*) m_vectActionQueue[0].vectObj[0];

		// load image
		pStarDlg->GetRemoteImage();

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// reset action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_LOAD_OBJECT_LIST_DETAILS  )
	{
//		m_pUnimap->ShowWaitDialog( wxT("Load Objects Details"), wxT("Loading details ...") );
//		while( m_pUnimap->m_bWaitDialogStart == 1 ) wxThread::Sleep( 20 );

		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;

		CAstroImage* pAstroImage = (CAstroImage*) m_vectActionQueue[0].vectObj[0];

		// load object details
		LoadObjectsDetails( pAstroImage );

		/////////////////
		// check if to save - todo: get this flag re3motely via cmd
		if( m_nActSaveOnDetails ) pAstroImage->SaveProfile( 0 );

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// reset action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_SET_GROUP_THUMBNAILS  )
	{
		//wxMutexGuiEnter();
		m_pFrame->m_pImageDb->SetCurrentGroupThumbnails( );
		//wxMutexGuiLeave();

		// reset action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_GROUP_DETECT )
	{
//		m_pUnimap->ShowWaitDialog( wxT("Group Detection Process"), wxT("Detecting objects in the image...") );

		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;

		// create and initialize my star detection engine
		m_pDetect = new CSextractor( );
		m_pDetect->m_pUnimapWorker = this;
		// set the config pointers
		m_pDetect->m_pDetectConfig = (CConfigDetect*) m_vectActionQueue[0].vectObj[1];
		m_pDetect->m_pMainConfig = (CConfigMain*) m_vectActionQueue[0].vectObj[2];
		m_pDetect->Init( );

		//////////
		// do group detection
		CImageGroup* pGroup = (CImageGroup*) m_vectActionQueue[0].vectObj[0];
		pGroup->Detect( m_pDetect );
		
		// remove my detection object
		delete( m_pDetect );
		m_pDetect = NULL;

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// reset action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_GROUP_MATCH )
	{
		//////////////////////////////////
		// init wait dialog
//		m_pUnimap->ShowWaitDialog( wxT("Group Matching Process"), 
//									wxT("Initializing matching engine..."), WAIT_DIALOG_TYPE_GROUP_MATCH );
		// wait a little for the dialog to get set
//		while( m_pFrame->m_pWaitDialogMatch == NULL ) wxThread::Sleep( 70 ); 

		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;

		//////////////////////////////
		// Start group matching 
		CImageGroup* pGroup = (CImageGroup*) m_vectActionQueue[0].vectObj[0];
		m_pSky->m_pSkyFinder->m_pWaitDialogMatch = (CWaitDialogMatch*) m_vectActionQueue[0].vectObj[1];
		pGroup->Match( m_pSky );

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 
		m_pSky->m_pSkyFinder->m_pWaitDialogMatch = NULL;

		// reset action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_GROUP_REGISTER )
	{
//		m_pUnimap->ShowWaitDialog( wxT("Group Registration Process"), wxT("Register stack images...") );

		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;
	
		/////////////////
		// do registration for images in the group in stack (all for now)
		CImageGroup* pGroup = (CImageGroup*) m_vectActionQueue[0].vectObj[0];
		pGroup->Register( );

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// reset action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_GROUP_ALIGN )
	{
//		m_pUnimap->ShowWaitDialog( wxT("Group Alignment Process"), wxT("Align stack of images...") );

		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;

		/////////////////
		// do align images in the group stack
		CImageGroup* pGroup = (CImageGroup*) m_vectActionQueue[0].vectObj[0];
		pGroup->Align( );

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// reset action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	}  else if( m_nActionCmd == THREAD_ACTION_GROUP_DYNAMICS_SEARCH )
	{
//		m_pUnimap->ShowWaitDialog( wxT("Group Dynamics Analysis"), wxT("Initialize Dynamic Objects Search...") );

		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;

		/////////////////
		// do align images in the group stack 
		CImageGroup* pGroup = (CImageGroup*) m_vectActionQueue[0].vectObj[0];
		pGroup->FindDynamicObjects( );

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// reset action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_GROUP_STACK )
	{
//		m_pUnimap->ShowWaitDialog( wxT("Stacking Process"), 
//									wxT("Initializing stacking engine..."), WAIT_DIALOG_TYPE_GROUP_STACK );
		// wait a little for the dialog to get set
//		while( m_pFrame->m_pWaitDialogStack == NULL ) wxThread::Sleep( 70 ); 
	
		// Start stacking 
		CImageGroup* pGroup = (CImageGroup*) m_vectActionQueue[0].vectObj[1];
		pGroup->StackImages( m_vectActionQueue[0].vectObj[0] );

		/////////////////
		// check if to save
//		if( m_nActSaveOnMatch ) m_pAstroImage->SaveProfile( 0 );

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_LP_MAP_PROCESS )
	{
//		m_pUnimap->ShowWaitDialog( wxT("Process Light Pollution Map"), 
//									wxT("Load Light Pollution Map ..."), WAIT_DIALOG_TYPE_PROGRESS );

		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;
		// reset flag
		m_bCurrentTaskSetToBreak = 0;

		CLocationEarth* pEarth = (CLocationEarth*) m_vectActionQueue[0].vectObj[0];
		wxChoice* pLPSelect = (wxChoice*) m_vectActionQueue[0].vectObj[1];

		// process fecth & get 
		double level = pEarth->GetLightPollutionLevel( );
		// todo: move that from here in imageproperty:OnPollutionGet method somehow
		wxMutexGuiEnter();
		// set selection by level
		if( level >= 0.0 && level < 1.0 )		// none
			pLPSelect->SetSelection(4);
		else if( level >= 1.0 && level < 3.0 )	// light
			pLPSelect->SetSelection(2);
		else if( level >= 3.0 && level < 5.0 )	// medium
			pLPSelect->SetSelection(1);
		else if( level >= 5.0 && level <= 7.0 )	// heavy
			pLPSelect->SetSelection(0);
		wxMutexGuiLeave();

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_UPLOAD_IMAGE )
	{
//		m_pUnimap->ShowWaitDialog( wxT("Upload Image"), 
//									wxT("Process Files ..."), WAIT_DIALOG_TYPE_PROGRESS );

		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;
		// reset flag
		m_bCurrentTaskSetToBreak = 0;

		//////////////////////////
		// call to upload astro image
		CUnimapOnline* pUnimapOnline = m_pUnimap->m_pUnimapOnline;
		pUnimapOnline->UploadAstroImage( pUnimapOnline->m_pUploadAstroImage, 1, m_vectActionQueue[0].handler );

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_UPLOAD_GROUP )
	{
//		m_pUnimap->ShowWaitDialog( wxT("Upload Group Images"), 
//									wxT("Process Files ..."), WAIT_DIALOG_TYPE_PROGRESS );

		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;
		// reset flag
		m_bCurrentTaskSetToBreak = 0;

		CUnimapOnline* pUnimapOnline = m_pUnimap->m_pUnimapOnline;
		pUnimapOnline->UploadImageGroup( pUnimapOnline->m_pUploadImageGroup, 1, m_vectActionQueue[0].handler );

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_UPLOAD_CUSTOM )
	{
		// todo: do implementation here ...

	} else if( m_nActionCmd == THREAD_ACTION_BROWSE_ONLINE_IMAGES )
	{
//		m_pUnimap->ShowWaitDialog( wxT("Fetch Image List"), 
//									wxT("Retreive File List ..."), WAIT_DIALOG_TYPE_PROGRESS );

		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;
		// reset flag
		m_bCurrentTaskSetToBreak = 0;

		///////
		// Get file list
		CUnimapOnline* pUnimapOnline = m_pUnimap->m_pUnimapOnline;
		pUnimapOnline->GetFileList( (int) m_vectActionQueue[0].vectInt[0], 
									(int) m_vectActionQueue[0].vectInt[1],
									(int) m_vectActionQueue[0].vectInt[2],
									m_vectActionQueue[0].vectStr[0], 1, m_vectActionQueue[0].handler );

		// now send event that is done
		//SendEventType( m_vectActionQueue[0].handler, wxID_ONLINE_EVENT_LIST_GET_DONE );
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_BROWSE_ONLINE_GET_IMAGE_PREVIEW )
	{
//		m_pUnimap->ShowWaitDialog( wxT("Get Image Preview"), 
//									wxT("Loading Image Preview Icon ..."), WAIT_DIALOG_TYPE_PROGRESS );

		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;
		CUnimapOnline* pUnimapOnline = (CUnimapOnline*) m_vectActionQueue[0].vectObj[0];
		// reset flag
		m_bCurrentTaskSetToBreak = 0;

		///////
		// Get file list
		pUnimapOnline->GetPreviewIconFile( m_vectActionQueue[0].vectStr[0], m_vectActionQueue[0].vectStr[1],
											m_vectActionQueue[0].vectStr[2], 1, m_vectActionQueue[0].handler );

		// now send event that is done
		//SendEventType( m_vectActionQueue[0].handler, wxID_ONLINE_EVENT_PREVIEW_GET_DONE );
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS );

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_BROWSE_ONLINE_DOWNLOAD_IMAGE )
	{
		// set current event handler
		m_evtWaitDialog = m_vectActionQueue[0].handler;
		CUnimapOnline* pUnimapOnline = (CUnimapOnline*) m_vectActionQueue[0].vectObj[0];
		// reset flag
		m_bCurrentTaskSetToBreak = 0;

		///////
		// Get image data
		pUnimapOnline->GetImageData( m_vectActionQueue[0].vectStr[0], m_vectActionQueue[0].vectStr[1],
									m_vectActionQueue[0].vectStr[2], m_vectActionQueue[0].vectStr[3],
									m_vectActionQueue[0].vectStr[4], 1, m_vectActionQueue[0].handler );

		// now send event that is done
		//SendEventType( m_vectActionQueue[0].handler, wxID_ONLINE_EVENT_DOWNLOAD_IMAGE_DATA_DONE ); 
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS );

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_WEATHER_FORECAST_ALL_GET )
	{
		int nSiteId = (int) m_vectActionQueue[0].vectInt[0];
		CObserverSite* pSite = m_pObserver->m_vectSite[nSiteId];
		CWeather* pWeather = pSite->m_pWeather;

		pSite->GetWeatherForecast( );
		pSite->GetCurrentWeather( );

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_WEATHER_FORECAST_ALL_GET_CUSTOM )
	{
		CWeather* pWeather = (CWeather*) m_vectActionQueue[0].vectObj[0];
		double lat = m_vectActionQueue[0].vectFloat[0];
		double lon = m_vectActionQueue[0].vectFloat[1];

		pWeather->GetWeatherData_Forecast( lat, lon, 0 );
		pWeather->GetCurrentWeatherData( lat, lon, 0 );

		//pSite->GetWeatherForecast( );
		//pSite->GetCurrentWeather( );

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_WEATHER_FORECAST_DAY_GET )
	{
		//int nSiteId = (int) m_vectActionQueue[0].vectInt[0];
		int nDay = (int) m_vectActionQueue[0].vectInt[0];
		//CObserverSite* pSite = m_pObserver->m_vectSite[nSiteId];
		CWeather* pWeather = (CWeather*) m_vectActionQueue[0].vectObj[0]; //pSite->m_pWeather;

		double lat = m_vectActionQueue[0].vectFloat[0];
		double lon = m_vectActionQueue[0].vectFloat[1];

		pWeather->GetWeatherData_DailyForecast( nDay, lat, lon );

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_WEATHER_SEEING_PREDICT_GET )
	{
		int nSiteId = (int) m_vectActionQueue[0].vectInt[0];
		int nEngineId = (int) m_vectActionQueue[0].vectInt[1];
		CObserverSite* pSite = m_pObserver->m_vectSite[nSiteId];
//		CWeather* pWeather = pSite->m_pWeather;

		// call to get current seeing prediction
		pSite->GetSeeingPrediction( nEngineId );

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_WEATHER_SEEING_PREDICT_GET_CUSTOM )
	{
		CWeather rWeather;

		double lat = m_vectActionQueue[0].vectFloat[0];
		double lon = m_vectActionQueue[0].vectFloat[1];
		int nEngineId = (int) m_vectActionQueue[0].vectInt[0];

		// call to get current seeing prediction
		rWeather.GetSeeingPrediction( lat, lon, nEngineId );

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_ENV_LIGHT_POLLUTION_LOAD )
	{
		CLightPollutionDlg* pDlg = (CLightPollutionDlg*) m_vectActionQueue[0].vectObj[0];

		// load my data
		pDlg->LoadData( );

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_ENV_OBSERVATORIES_LOAD )
	{
		CObservatoriesDlg* pDlg = (CObservatoriesDlg*) m_vectActionQueue[0].vectObj[0];

		// load my data
		pDlg->LoadData( );

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_NEWS_FETCH_HEADLINES )
	{
		CNews* pNews = (CNews*) m_vectActionQueue[0].vectObj[0];
		int nType = (int) m_vectActionQueue[0].vectInt[0];

		// load my data
		pNews->Get_Headlines( nType );

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_LOAD_EARTH_MAP_DATA )
	{
		CEarthMapView* pMapView = (CEarthMapView*) m_vectActionQueue[0].vectObj[1];
		CLocationEarth* pEarth = (CLocationEarth*) m_vectActionQueue[0].vectObj[0];

		// load data
		pEarth->LoadAllCountries();
		pMapView->LoadImageMap();

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_GET_HIST_WEATER_DATA )
	{
		CWeather* pWeather = (CWeather*) m_vectActionQueue[0].vectObj[0];
		double lat = m_vectActionQueue[0].vectFloat[0];
		double lon = m_vectActionQueue[0].vectFloat[1];
		double timeObs = m_vectActionQueue[0].vectFloat[2];

		// fetch data
		pWeather->GetWeatherData_AtDateTime( lat, lon, timeObs, 0 );

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_IMAGE_3D_COLORMAP )
	{
		CAstroImage* pAstroImage = (CAstroImage*) m_vectActionQueue[0].vectObj[1];
		int nType = (int) m_vectActionQueue[0].vectInt[0];
		int nRGB = (int) m_vectActionQueue[0].vectInt[1];

		// set image data data
		if( nType == -1 )
		{
			CColorMapViewDialog* pDlg = (CColorMapViewDialog*) m_vectActionQueue[0].vectObj[0];
			pDlg->SetAstroImage( pAstroImage );

		} else if( nType == COLOR_3D_PROFILER_TYPE_NORMAL || nType == COLOR_3D_PROFILER_TYPE_VIGNETTING )
		{
			CImgView3DColMap* pView = (CImgView3DColMap*) m_vectActionQueue[0].vectObj[0];
			// build with vingeneting or other params alike
			wxImage* pWxImg = (wxImage*) m_vectActionQueue[0].vectObj[2];
			pView->SetImageColorMap( pAstroImage, *pWxImg, nType, nRGB );

		} else if( nType == COLOR_3D_PROFILER_TYPE_3D_DISTRIBUTION )
		{
			CImgView3DColMap* pView = (CImgView3DColMap*) m_vectActionQueue[0].vectObj[0];
			wxImage* pWxImg = (wxImage*) m_vectActionQueue[0].vectObj[2];
			//pView->SetImageColorCube( pAstroImage, *pWxImg, nType, nRGB );

		} else if( nType == COLOR_3D_PROFILER_TYPE_FFT )
		{
			CImgView3DColMap* pView = (CImgView3DColMap*) m_vectActionQueue[0].vectObj[0];
			wxImage* pWxImg = (wxImage*) m_vectActionQueue[0].vectObj[2];
			pView->SetImageFFT( pAstroImage, *pWxImg, nType, nRGB );
		}

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_IMAGE_3D_DISTGRID )
	{
		CAstroImage* pAstroImage = (CAstroImage*) m_vectActionQueue[0].vectObj[1];
		int nType = (int) m_vectActionQueue[0].vectInt[0];

		// calculate and set image data
		if( nType == -1 )
		{
			CDistGridViewDialog* pDlg = (CDistGridViewDialog*) m_vectActionQueue[0].vectObj[0];
			pAstroImage->CalcDisplacementPointsByDist( );
			pDlg->SetAstroImage( pAstroImage );

		} else if( nType == 0 )
		{
			CImgView3DDistMap* pView = (CImgView3DDistMap*) m_vectActionQueue[0].vectObj[0];
			pAstroImage->ClearDisplacementPoints( );
			pAstroImage->CalcDisplacementPointsByDist( );

			pView->SetImageDistGrid( pAstroImage );
			//pView->CalculateStructures( ); ... already done in prev method

		} else if( nType == 1 )
		{
			CImgView3DDistMap* pView = (CImgView3DDistMap*) m_vectActionQueue[0].vectObj[0];
			pAstroImage->ClearDisplacementPoints( );
			pAstroImage->CalcDisplacementPointsByPos( );

			pView->SetImageDistGrid( pAstroImage );
			//pView->CalculateStructures( );

		// on config ...
		} else if( nType == -2 )
		{
			CImgView3DDistMap* pView = (CImgView3DDistMap*) m_vectActionQueue[0].vectObj[0];
			//pAstroImage->ClearDisplacementPoints( );
			//pAstroImage->CalcDisplacementPointsByPos( );

			pView->SetImageDistGrid( pAstroImage );
			//pView->CalculateStructures( );
		}

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_IMAGE_3D_SPACEDIST )
	{
		CAstroImage* pAstroImage = (CAstroImage*) m_vectActionQueue[0].vectObj[1];
		int nType = (int) m_vectActionQueue[0].vectInt[0];

		// calculate and set image data
		if( nType == -1 )
		{
			CSpaceDistViewDialog* pDlg = (CSpaceDistViewDialog*) m_vectActionQueue[0].vectObj[0];
			pAstroImage->CalcDisplacementPointsByPos( );
			pDlg->SetAstroImage( pAstroImage );

		} else if( nType >= 0 )
		{
			CImgView3DSpaceDist* pView = (CImgView3DSpaceDist*) m_vectActionQueue[0].vectObj[0];
			pView->SetImageSpaceDist( pAstroImage, nType );
//			pView->CalculateStructures( );
		} 

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_WEATHER_SATELLITE_IMAGE )
	{
		CWeatherVisDlg* pDlg = (CWeatherVisDlg*) m_vectActionQueue[0].vectObj[0];

		// fetch remote image
		pDlg->FetchRemoteImage( );

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_WEATHER_SUN_VIEW_IMAGE )
	{
		CSunViewDlg* pDlg = (CSunViewDlg*) m_vectActionQueue[0].vectObj[0];

		// fetch remote image
		pDlg->FetchRemoteImage( );

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	} else if( m_nActionCmd == THREAD_ACTION_PROCCESS_TEMPORARY_FOLDERS )
	{
		CFilesManager* pFilesMgr = (CFilesManager*) m_vectActionQueue[0].vectObj[0];

		// process temporary files
		pFilesMgr->ProcessTemporary( );

		// send event to close dialog
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

		// a small delay
		wxThread::Sleep( 300 );

		// reset flag
		m_bCurrentTaskSetToBreak = 0;
		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

	//////////////////////////////////////////////////////////////////
	// :: SERVICES
	} else if( m_nActionCmd == THREAD_ACTION_SERVICES_IMAGE_PLATESOLVE )
	{
//		m_pUnimap->ShowWaitDialog( wxT("Detection Process"), wxT("Detecting objects in the image...") );

		// set current event handler
		//m_evtWaitDialog = m_vectActionQueue[0].handler;

		// get locker and signal
		wxMutex* m_pJobDoneMutex = (wxMutex*) m_vectActionQueue[0].vectObj[0];
		wxCondition* m_pJobDoneCondition = (wxCondition*) m_vectActionQueue[0].vectObj[1];
		// get catalog
		wxString strCat = m_vectActionQueue[0].vectStr[0];
		if( strCat.CmpNoCase( wxT("SAO") ) == 0 )
			m_pMainConfig->m_nCatalogForMatching = CATALOG_ID_SAO;
		else if( strCat.CmpNoCase( wxT("HIPPARCOS") ) == 0 )
			m_pMainConfig->m_nCatalogForMatching = CATALOG_ID_HIPPARCOS;
		else if( strCat.CmpNoCase( wxT("TYCHO2") ) == 0 )
			m_pMainConfig->m_nCatalogForMatching = CATALOG_ID_TYCHO_2;
		else if( strCat.CmpNoCase( wxT("USNOB1") ) == 0 )
			m_pMainConfig->m_nCatalogForMatching = CATALOG_ID_USNO_B1;
		else if( strCat.CmpNoCase( wxT("NOMAD") ) == 0 )
			m_pMainConfig->m_nCatalogForMatching = CATALOG_ID_NOMAD;
		else if( strCat.CmpNoCase( wxT("2MASS") ) == 0 )
			m_pMainConfig->m_nCatalogForMatching = CATALOG_ID_2MASS;
		else if( strCat.CmpNoCase( wxT("GSC") ) == 0 )
			m_pMainConfig->m_nCatalogForMatching = CATALOG_ID_GSC;
		else
			m_pMainConfig->m_nCatalogForMatching = CATALOG_ID_SAO;

		// :: ASTRO IMAGE
		CAstroImage* pAstroImage = (CAstroImage*) m_vectActionQueue[0].vectObj[2];
		pAstroImage->m_pMainConfig = m_pUnimap->m_pMainConfig;
		pAstroImage->SetSky( m_pSky );
		// load image data
		pAstroImage->Load(0);

		// free data from image - for now just set stars to zero
		pAstroImage->ClearDetected( );

		// create and initialize my star detection engine
		CSextractor* pDetect = new CSextractor( );
	//	m_pDetect->m_pImgView = m_pFrame->m_pImgView;
	//	m_pDetect->m_pLogger = m_pLogger;
		pDetect->m_pUnimapWorker = this;
		// set the config pointers
		pDetect->m_pDetectConfig = m_pUnimap->m_pDetectConfig;
		pDetect->m_pMainConfig = m_pUnimap->m_pMainConfig;
	//	m_pFrame->m_pSextractor = m_pStarFinder;

		pDetect->Init( );
		pDetect->Make( pAstroImage );

		pAstroImage->ProcessStars( );

		///////////////////////
		// here we set that the image was hanged 
		pAstroImage->m_bIsChanged = 1;
		pAstroImage->m_bIsDetected = 1;
		pAstroImage->m_nImgSourceType = pDetect->prefs.detect_type;

		delete( pDetect );

		////////////////////
		// :: do matching
		m_pSky->SolveAstroImage( pAstroImage, NULL );

		////////////////////
		// set names
		wxString strObjName = wxT("");
		for( int i=0; i<pAstroImage->m_nStar; i++ )
		{
			if( m_pSky->m_pCatalogStarNames->GetStarName( i, pAstroImage->m_vectStar[i], strObjName ) )
			{
				pAstroImage->m_vectStar[i].cat_name = (char*) malloc( strObjName.Len() + 1 );
				strcpy( pAstroImage->m_vectStar[i].cat_name, strObjName.mb_str() );
			}
		}


		pAstroImage->SaveProfile( 0 );
		//delete( pAstroImage );		

		// set detect action flag to none
		m_nActionCmd = THREAD_ACTION_NONE;
		m_vectActionQueue.erase(m_vectActionQueue.begin());

		// lock and signal
		wxMutexLocker lock( *m_pJobDoneMutex );
		m_pJobDoneCondition->Broadcast();

	}

	return( 0 );
}
