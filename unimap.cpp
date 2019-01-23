////////////////////////////////////////////////////////////////////
// Name:		Unimap Main Routine
// File:		unimap.cpp
// Purpose:		Main routine for unimap
//
// Created by:	Larry Lart on 26/05/2004
// Updated by:	Larry Lart on 01/02/2010
//
////////////////////////////////////////////////////////////////////

// system headers
#include <Shlobj.h>
 
//#include "vld.h"

//#include "highgui.h"
//#include "cv.h"
//#include "cvaux.h"
 //#include "_cxcore.h"

// for memory leaks ?
//#define _CRTDBG_MAP_ALLOC
//#include <stdio.h>
//#include <crtdbg.h>

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// wx
#include "wx/wxprec.h"
#include "wx/statline.h"
#include "wx/minifram.h"
#include "wx/thread.h"
#include <wx/filesys.h>
#include <wx/fs_inet.h>
#include <wx/socket.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

//#include "../graphics/tps/vec.h"
//#include "../graphics/tps/linalg3d.h"
//#include "../graphics/tps/ludecomposition.h"

//////////////
// elynx headers
#include <elx/image/ImageFileManager.h>

// custom headers
#include "gui/frame.h"
#include "gui/astro_img_view.h"
//#include "gui/imageview3d.h"
#include "logger/logger.h"
#include "image/imagedb.h"
#include "image/astroimage.h"
#include "sky/sky.h"
#include "sky/sky_finder.h"
#include "observer/observer.h"
#include "camera/camera.h"
#include "telescope/telescope.h"
#include "telescope/focuser.h"
#include "config/config.h"
#include "config/detectconfig.h"
#include "config/mainconfig.h"
#include "online/unimap_online.h"
#include "sound/sound_engine.h"
#include "util/apps_manager.h"
#include "util/folders.h"
#include "util/geometry.h"
#include "sysinfo.h"

//#include "proc/fits/fitscat.h"
#define NOCTYPES
#include "proc/sextractor.h"
#undef NOCTYPES

// other local headers
#include "gui/waitdialog.h"
#include "match/starmatch.h"
#include "unimap_worker.h"
#include "unimap_service.h"

// include main header
#include "unimap.h"

// user data path
wxString g_strUserAppDataPath;

IMPLEMENT_APP(CUnimap)

//		***** CLASS CUnimap *****
////////////////////////////////////////////////////////////////////
// events table
BEGIN_EVENT_TABLE(CUnimap, wxApp)
	// key event
	EVT_KEY_DOWN( CUnimap::OnKeyDown )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CUnimap
// Purose:	build my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CUnimap::CUnimap( ) : wxApp()
{
//	g_pUnimap = this;
	m_pSoundEngine = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CUnimap
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CUnimap::~CUnimap( )
{
//	_CrtDumpMemoryLeaks();  // dump memory leaks

//	delete( m_pConfig );
//	delete( m_pLogger );
}

////////////////////////////////////////////////////////////////////
int CUnimap::InitDataFolders( )
{
	unMakeDirIf( unMakeAppPath( wxT("data") ) );

	unMakeDirIf( unMakeAppPath( wxT("data/imagedb") ) );
	// object
	unMakeDirIf( unMakeAppPath( wxT("data/object") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/object/img") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/object/img/ir") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/object/img/optical") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/object/img/radio") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/object/img/uv") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/object/img/xray") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/object/spectrum") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/object/tmp") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/object/tmp/spectrum") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/skyview/tmp") ) );
	// bibcodes
	unMakeDirIf( unMakeAppPath( wxT("data/bibcodes") ) );
	unMakeDirIf( unMakeAppPath( wxT("") ) );
	// remote folders
	unMakeDirIf( unMakeAppPath( wxT("data/remote") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/remote/Vizier") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/remote/Leda") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/remote/NavyMil") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/remote/NavyMil") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/remote/ExoplanetEu") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/remote/Celestrak") ) );
	// temp folders
	unMakeDirIf( unMakeAppPath( wxT("data/temp") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/temp/camera") ) );
	// temp :: online
	unMakeDirIf( unMakeAppPath( wxT("data/temp/uonline") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/temp/uonline/astro_images") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/temp/uonline/preview_icons") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/temp/uonline/thumb_icons") ) );
	// catalog folders
	unMakeDirIf( unMakeAppPath( wxT("data/catalog") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/catalog/stars") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/catalog/dso") ) );
	// thumbnails
	unMakeDirIf( unMakeAppPath( wxT("data/thumbnails") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/sun") ) );
	// weather
	unMakeDirIf( unMakeAppPath( wxT("data/weather") ) );
	unMakeDirIf( unMakeAppPath( wxT("data/weather/sat_images") ) );

	return( 1 );
}

// load all the cursors in a global vector
////////////////////////////////////////////////////////////////////
void CUnimap::LoadCursors( )
{
	int i=0;
	wxCursor* pCursor;

	for( i=0; i<10; i++ ) m_vectCursors[i] = &wxNullCursor;

	// default cursor
	pCursor = new wxCursor( wxT("./resources/precision.cur"), wxBITMAP_TYPE_CUR );
	if( pCursor && pCursor->IsOk() )
		m_vectCursors[UNIMAP_CURSOR_IMAGE_DEFAULT] = pCursor;
	else if( pCursor )
		delete( pCursor );

	// image drag
	pCursor = new wxCursor( wxT("./resources/drag.cur"), wxBITMAP_TYPE_CUR );
	if( pCursor && pCursor->IsOk() )
		m_vectCursors[UNIMAP_CURSOR_IMAGE_DRAG] = pCursor;
	else if( pCursor )
		delete( pCursor );

	// image add area
	pCursor = new wxCursor( wxT("./resources/area_add.cur"), wxBITMAP_TYPE_CUR );
	if( pCursor && pCursor->IsOk() )
		m_vectCursors[UNIMAP_CURSOR_IMAGE_ADD_AREA] = pCursor;
	else if( pCursor )
		delete( pCursor );

	// image zoom in
	pCursor = new wxCursor( wxT("./resources/zoomin.cur"), wxBITMAP_TYPE_CUR );
	if( pCursor && pCursor->IsOk() )
		m_vectCursors[UNIMAP_CURSOR_IMAGE_ZOOM_IN] = pCursor;
	else if( pCursor )
		delete( pCursor );

	// image zoom in
	pCursor = new wxCursor( wxT("./resources/zoomout.cur"), wxBITMAP_TYPE_CUR );
	if( pCursor && pCursor->IsOk() )
		m_vectCursors[UNIMAP_CURSOR_IMAGE_ZOOM_OUT] = pCursor;
	else if( pCursor )
		delete( pCursor );

	// area move
	pCursor = new wxCursor( wxT("./resources/area_move.cur"), wxBITMAP_TYPE_CUR );
	if( pCursor && pCursor->IsOk() )
		m_vectCursors[UNIMAP_CURSOR_AREA_MOVE] = pCursor;
	else if( pCursor )
		delete( pCursor );

	// area resize ac
	pCursor = new wxCursor( wxT("./resources/area_resize_ac.cur"), wxBITMAP_TYPE_CUR );
	if( pCursor && pCursor->IsOk() )
		m_vectCursors[UNIMAP_CURSOR_AREA_RESIZE_AC] = pCursor;
	else if( pCursor )
		delete( pCursor );

	// area resize bd
	pCursor = new wxCursor( wxT("./resources/area_resize_bd.cur"), wxBITMAP_TYPE_CUR );
	if( pCursor && pCursor->IsOk() )
		m_vectCursors[UNIMAP_CURSOR_AREA_RESIZE_BD] = pCursor;
	else if( pCursor )
		delete( pCursor );

	// distance measument drag cursor
	pCursor = new wxCursor( wxT("./resources/dist_tool.cur"), wxBITMAP_TYPE_CUR );
	if( pCursor && pCursor->IsOk() )
		m_vectCursors[UNIMAP_CURSOR_DIST_MEASURMENT] = pCursor;
	else if( pCursor )
		delete( pCursor );

	// distance measument drag cursor
	pCursor = new wxCursor( wxT("./resources/norm08.cur"), wxBITMAP_TYPE_CUR );
	if( pCursor && pCursor->IsOk() )
		m_vectCursors[UNIMAP_CURSOR_OBJ_DETAIL] = pCursor;
	else if( pCursor )
		delete( pCursor );

}

// get loeaded cursor with id
////////////////////////////////////////////////////////////////////
wxCursor* CUnimap::GetCursor( int nId )
{
	if( nId < 0 && nId >= 10 ) return( &wxNullCursor );
	// return from my vector
	return( m_vectCursors[nId] );
}

////////////////////////////////////////////////////////////////////
// Method:	On Init
// Class:	CUnimap
// Purose:	initialize my application
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
bool CUnimap::OnInit( )
{
	SetVendorName( wxT("Larry Lart") );
	// init data folder
	g_strUserAppDataPath = wxT("."); //wxStandardPaths::Get().GetUserDataDir();
//	unMakeDirIf( g_strUserAppDataPath );

	// init all data folders
	InitDataFolders( );
	// init all image handlers
	wxInitAllImageHandlers();
	// load cursors
	LoadCursors( );

#ifndef _DEBUG
	bool ok = the_ImageFileManager.Register( "./io/" );
#else
	bool ok = the_ImageFileManager.Register( "./iod/" );
#endif

	if( !ok ) elxThrow( elxErrInvalidParams, "Image plugins registration failed." );

	// init geometry/math globals
	InitGeometryData();

	// init net handler
//	wxFileSystem::AddHandler( new wxInternetFSHandler );
	wxSocketBase::Initialize();

	// the logger
	m_pLogger = new CLogger( );
	m_pLogger->SetLoggingToFile( unMakeAppPath(wxT("/unimap.log")) );

	////////////////////////////////////
	// initialize configurations
	m_pMainConfig = new CConfigMain( g_strUserAppDataPath, wxT("/unimap.ini") );
	m_pDetectConfig = new CConfigDetect( m_pMainConfig, unMakeAppPath(wxT("cfg/detect.ini")) );

	////////////////////
	// initialize locale
	int cr_loc = m_pLocale.GetSystemLanguage();
	// wxLANGUAGE_ENGLISH_US = 58
	int ii = m_pLocale.Init( g_vectGuiLanguage[m_pMainConfig->m_nGuiLanguage].id, wxLOCALE_CONV_ENCODING );

	wxLocale::AddCatalogLookupPathPrefix( wxT("./lang/") );
	ii = m_pLocale.AddCatalog(wxT("unimap"));

	///////////////////////
	// SET/GET SYSTEM ID 
	//wxString strHardwareId=wxT("");
	GetSystemId( );

	////////////////////////////////
	// create widows main frame
	m_pFrame = new CGUIFrame( NULL, wxPoint(-1, -1), wxSize(525, 200) );

	// set parent
	m_pFrame->m_pUnimap = this;
	m_pLogger->SetView( m_pFrame->m_pLoggerView );
	m_pFrame->SetLogger( m_pLogger );
	// set frame config
	m_pFrame->SetConfig( m_pMainConfig, m_pDetectConfig );

	///////////////////////////
	// create and initialize image db
	m_pImageDb = new CImageDb( );
	m_pImageDb->m_pFrame = m_pFrame;
	m_pImageDb->SetLogger( m_pLogger );
	m_pFrame->m_pImageDb = m_pImageDb;

	///////////////////////////
	// create the OBSERVER object
	m_pObserver = new CObserver( );
	m_pObserver->Init( );
	m_pFrame->m_pObserver = m_pObserver;

	///////////////////////////
	// create the CAMERA object
	m_pCamera = new CCamera( m_pFrame );
	m_pFrame->m_pCamera = m_pCamera;

	///////////////////////////
	// create the TELESCOPE object
	m_pTelescope = new CTelescope( );
	m_pFrame->m_pTelescope =  m_pTelescope;

	///////////////////////////
	// create the FOCUSER object
	m_pFocuser = new CFocuser( );
	m_pFocuser->SetConfig( m_pTelescope );

	///////////////////////////
	// create the sky object
	m_pSky = new CSky( );
	m_pSky->SetUnimap( this );
	m_pSky->SetMainFrame( m_pFrame );
	m_pSky->SetConfig( m_pMainConfig );

	// create match object
	m_pStarMatch = new CStarMatch( );
	m_pStarMatch->SetConfig( m_pMainConfig );
//	m_pStarMatch->m_pLogger = m_pLogger;
	m_pStarMatch->Init( );
	m_pSky->m_pSkyFinder->m_pStarMatch = m_pStarMatch;
	// set frame refence
	m_pFrame->m_pStarMatch = m_pStarMatch;

	////////////////////////////////////////////////////////
	// create and initialize my thread
	m_pUnimapWorker = new CUnimapWorker( m_pFrame, m_pLogger );
	m_pUnimapWorker->SetConfig( m_pMainConfig );

	if ( m_pUnimapWorker->Create() != wxTHREAD_NO_ERROR )
	{
//		m_pLogger->Add( "ERROR :: worker thread initialization failure\n" );
	} else
	{
//		m_pLogger->Add( "INFO :: thread initialization ok\n" );
	}

	// set local pointers
	m_pUnimapWorker->m_pUnimap = this;
	m_pUnimapWorker->m_pSky = m_pSky;
	m_pUnimapWorker->m_pMatch = m_pStarMatch;
	m_pUnimapWorker->m_pObserver = m_pObserver;

	// set remote pointers
//	m_pConfig->m_pPButtonWorker = m_pPButtonWorker;
	m_pFrame->m_pUnimapWorker = m_pUnimapWorker;
	m_pSky->SetWorker( m_pUnimapWorker );
	m_pStarMatch->m_pUnimapWorker = m_pUnimapWorker;

	// start the thread
	if ( m_pUnimapWorker->Run() != wxTHREAD_NO_ERROR )
	{
//		m_pLogger->Add( "ERROR :: worker thread running failure\n" );
	} else
	{
//		m_pLogger->Add( "INFO :: worker thread running ok\n" );
	}

	//////////////////////////////////////////////////
	// create unimap online object
	m_pUnimapOnline = new CUnimapOnline();
	m_pUnimapOnline->SetConfig( m_pFrame, m_pUnimapWorker, m_pMainConfig );


	/////////////////////////////////////////////////
	// :: create unimap services
	StartServices();

	/////////////////////
	// set image db pointers
	m_pImageDb->m_pUnimapWorker = m_pUnimapWorker;
	m_pUnimapWorker->m_pImageDb = m_pImageDb;
	m_pImageDb->m_pUnimap = this;
	m_pImageDb->m_pMainConfig = m_pMainConfig;
	m_pImageDb->m_pDetectConfig = m_pDetectConfig;
	m_pImageDb->m_pSky = m_pSky;
//	m_pImageDb->Init( );

	///////////////////////
	// TEST APPS MANAGER
	m_pAppsMgr = new CAppsManager();
//	m_pAppsMgr->Init();

	/////////////////////////
	// load things on startup
	LoadOnStartup( );

	/////////////////////	
	// Show the frame
	m_pFrame->SetMenuAppsState( );
	m_pFrame->SetMenuState( 0, 0 );
	m_pFrame->Show(TRUE);
	SetTopWindow(m_pFrame);
	m_pFrame->ViewGroupThumbnails( m_pMainConfig->m_bShowImageThumbs, -1 );

	/////////////////////
	// show if update
	if( m_pUnimapOnline->m_bAppUpdates && m_pUnimapOnline->m_vectAppUpdates.size() > 0 )
	{
		wxString strUpdateMsg=wxT("");
		// build message
		for( int i=0; i<m_pUnimapOnline->m_vectAppUpdates.size(); i++ ) 
			strUpdateMsg += m_pUnimapOnline->m_vectAppUpdates[i].title + wxT("\n");
		// check if there is something
		wxMessageBox( strUpdateMsg, wxT("Updates"), wxICON_INFORMATION|wxOK );
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////
int CUnimap::StartServices()
{
	m_pUnimapService = NULL;
	if( m_pMainConfig->m_bEnableSoapServices == 1 )
	{
		m_pUnimapService = new CUnimapService( m_pLogger );
		m_pUnimapService->SetConfig( m_pMainConfig );
		m_pUnimapService->m_pDetectConfig = m_pDetectConfig;

		if ( m_pUnimapService->Create() != wxTHREAD_NO_ERROR )
		{
			m_pUnimapService = NULL;
	//		m_pLogger->Add( "ERROR :: worker thread initialization failure\n" );
		} else
		{
	//		m_pLogger->Add( "INFO :: thread initialization ok\n" );
		}

		// set local pointers
		m_pUnimapService->m_pUnimap = this;
		m_pUnimapService->m_pUnimapWorker = m_pUnimapWorker;

		// set remote pointers
	//	m_pConfig->m_pPButtonWorker = m_pPButtonWorker;
		m_pUnimapWorker->m_pUnimapService = m_pUnimapService;

		// start the thread
		if ( m_pUnimapService->Run() != wxTHREAD_NO_ERROR )
		{
			m_pUnimapService = NULL;
	//		m_pLogger->Add( "ERROR :: worker thread running failure\n" );
		} else
		{
	//		m_pLogger->Add( "INFO :: worker thread running ok\n" );
		}
	}

	return(1);
}

////////////////////////////////////////////////////////////////////
int CUnimap::StopServices()
{
	if( m_pUnimapService != NULL )
	{
		m_pUnimapService->Pause( );
		m_pUnimapService->StopService();
		//m_pUnimapService->Wait();
		//m_pUnimapService->Delete( );
		m_pUnimapService = NULL;
	}

	return(1);
}

////////////////////////////////////////////////////////////////////
int CUnimap::RestartServices()
{
	StopServices();
	StartServices();

	return(1);
}

////////////////////////////////////////////////////////////////////
// Method:	OnExit
// Class:	CUnimap
// Purose:	Managing closing all app properly on close event received
//			from various obejcts
// Input:	nothing
// Output:	status ?
////////////////////////////////////////////////////////////////////
int CUnimap::OnExit( )
{
	int i=0;

// kill the context
/*
typedef struct
{
	const char* file;
	int         line;

} CvStackRecord;

typedef struct CvContext
{
	int  err_code;
	int  err_mode;
	CvErrorCallback error_callback;
	void*  userdata;
	char  err_msg[4096];
	CvStackRecord  err_ctx;

} CvContext;

	CvContext *pContext;
	if( g_TlsIndex != TLS_OUT_OF_INDEXES )
	{
		pContext = (CvContext*) TlsGetValue( g_TlsIndex );
		if( pContext != NULL ) free( pContext );
	}
	TlsFree( g_TlsIndex );
*/

//	TlsFree( TLS_OUT_OF_INDEXES ); 

///////////////


	// stop sound engine
	if( m_pSoundEngine ) 
	{
		m_pSoundEngine->StopRecording( NULL );
		m_pSoundEngine->Delete();
	}

	// delete worker thread
	if( m_pUnimapWorker != NULL ) 
	{
		m_pUnimapWorker->Delete( );
		m_pUnimapWorker = NULL;
	}

	// stop/delete services
	StopServices();	

	// delete other objects
	delete( m_pLogger );
	delete( m_pImageDb );
	delete( m_pObserver );
	delete( m_pCamera );
	delete( m_pTelescope );
	delete( m_pFocuser );
	delete( m_pSky );
	delete( m_pStarMatch );
	delete( m_pUnimapOnline );

	// to the destructor last - so other object will have time to
	// save their configuration here 
	delete( m_pDetectConfig );
	delete( m_pMainConfig );

	// delete cursors
	for( i=0; i<10; i++ ) 
		if( m_vectCursors[i] && m_vectCursors[i] != &wxNullCursor )
			delete( m_vectCursors[i] );

	delete( m_pAppsMgr );

	return( true );
}

////////////////////////////////////////////////////////////////////
int CUnimap::LoadOnStartup( )
{
	/////////////////////////////
	// SET THREAD TO LOAD THE STARTUP PROFILE
	DefCmdAction act;
	act.id = THREAD_ACTION_UNIMAP_LOAD_ONSTARTUP;
	act.vectObj[0] = (void*) m_pAppsMgr;
	act.vectObj[1] = (void*) m_pUnimapOnline;

	/////////////////
	// show wait dialog
	CWaitDialog* pWaitDialog = new CWaitDialog( m_pFrame, wxT("Wait..."), 
						wxT("Loading/Initializing base data ..."), 0, m_pUnimapWorker, &act );
	pWaitDialog->ShowModal();
	// delete/reset wait dialog
	delete( pWaitDialog );
	pWaitDialog = NULL;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		OnKeyDown
// Class:		CUnimap
// Purpose:		when a key was pressed
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CUnimap::OnKeyDown( wxKeyEvent& pEvent )
{
	int key = pEvent.GetKeyCode();
	int mod = pEvent.GetModifiers();

	// full screen case
	if( (mod & wxMOD_ALT && key == WXK_RETURN) || 
		(m_pFrame->m_bIsFullScreen && key == WXK_ESCAPE) )
	{
		// set image full screen
		m_pFrame->SetFullScreen( );

	// slideshow :: pause
	} else if( m_pFrame->m_pSlideShowTimer && key == WXK_SPACE )
	{
		if( m_pFrame->m_nSlideShowStatus == 1 )
			m_pFrame->m_nSlideShowStatus = 2;
		else if( m_pFrame->m_nSlideShowStatus == 2 )
			m_pFrame->m_nSlideShowStatus = 1;

	// slideshow :: next
	} else if( m_pFrame->m_pSlideShowTimer && key == WXK_RIGHT )
	{
		m_pFrame->GetNextSlide(0);

	// slideshow :: back
	} else if( m_pFrame->m_pSlideShowTimer && key == WXK_LEFT )
	{
		m_pFrame->GetNextSlide(1);

	// slideshow :: exit
	} else if( m_pFrame->m_pSlideShowTimer && key == WXK_ESCAPE )
	{
		m_pFrame->StopSlideShow();

	} else 
		pEvent.Skip();
}

////////////////////////////////////////////////////////////////////
CSoundEngine* CUnimap::StartSoundEngine( const wxString& strSoundFile )
{
	if( !m_pSoundEngine ) m_pSoundEngine = new CSoundEngine( strSoundFile );

	return( m_pSoundEngine );
}

////////////////////////////////////////////////////////////////////
int CUnimap::StopSoundEngine( )
{
	if( !m_pSoundEngine ) return( 0 );

	m_pSoundEngine->Delete();
	m_pSoundEngine = NULL;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	ProcessInputFile
// Class:	CUnimap
// Purose:	process an input file name/link
// Input:	link name, references to name, full name and path
// Output:	status
////////////////////////////////////////////////////////////////////
int CUnimap::ProcessInputFile( const wxString& strInputFile, wxString& strName, 
									wxString& strFullName, wxString& strPath, wxString& strWorkPath,
									wxString& strIconPath, wxString& strArgs, int& nType )
{
	int nStatus = 1;
	wxString strTempPath = wxT("");
	wxString strTempName = wxT("");
	wxString strExt = wxT("");
	// empty string babies
	strName = wxT("");
	strFullName = wxT("");
	strPath = wxT("");
	strWorkPath = wxT("");
	strIconPath = wxT("");
	strArgs = wxT("");
	nType = -1;

	// extract extension from link file
	wxSplitPath( strInputFile, &strTempPath, &strTempName, &strExt );

	// if file is a link
	if( strExt.CmpNoCase( wxT("lnk") ) == 0 )
	{
		// populate name using link name
		strName = strTempName;

		strTempPath = wxT("");
		strTempName = wxT("");
		strExt = wxT("");

		// extract target from icon
		nStatus = ProcessShellLink( strInputFile, strFullName, strPath, strIconPath, strArgs );
		// exit imediately - no point to continue
		if( nStatus == 0 ) return( 0 );

		strWorkPath = strPath;

		wxSplitPath( strFullName, &strTempPath, &strTempName, &strExt );
		strTempName += wxT(".") + strExt;

		//check if work dir emty use xtracted path
		if( strPath.IsEmpty() ) strPath = strTempPath;
		nType = SYSTEM_FILE_TYPE_LINK;

	// if input file is an exe, com or bat 
	} else if( IsFileExtOk( strExt ) )
	{
		strTempName += wxT(".") + strExt;
		// process link name directly
		strName = strTempName;
		strFullName = strInputFile;
		strPath = strTempPath;
		strIconPath = wxT("");
		strArgs = wxT("");
		nType = SYSTEM_FILE_TYPE_IMAGE;

	} else
		nStatus = 0;

	return( nStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	ProcessLink
// Class:	CUnimap
// Purose:	process a windows shell/file system link
// Input:	link
// Output:	full path, path, icon path and arguments
////////////////////////////////////////////////////////////////////
int CUnimap::ProcessShellLink( const wxString& strLink, wxString& strFullName, 
									wxString& strPath, wxString& strIconPath, wxString& strArgs )
{
	wxString strMsg=wxT("");
	int nStatus = 1;

	IShellLink *psl;                           
	HRESULT hres;
	WIN32_FIND_DATA wfd;
	char szGotPath[MAX_PATH];
	wxChar str_temp[2000];

	// Get pointer to the IShellLink interface.
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
								IID_IShellLink, (LPVOID *)&psl);

	if(SUCCEEDED(hres))
	{
		// Get pointer to the IPersistFile interface.

		IPersistFile *ppf;
		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID *)&ppf);

		if(SUCCEEDED(hres))
		{
			//WORD wsz[MAX_PATH];
			LPWSTR wsz=wxT("");

			// Ensure string is Unicode.
			MultiByteToWideChar(CP_ACP, 0, strLink.mb_str(), -1, wsz, MAX_PATH);

			// Load the shell link
			hres = ppf->Load((LPCOLESTR) wsz, STGM_READ);

			if (SUCCEEDED(hres))
			{
				// Resolve the link.

				hres = psl->Resolve(0, SLR_ANY_MATCH);
				//                  ^
				// Using 0 instead -| of hWnd, as hWnd is only used if
				// interface needs to prompt for more information. Should use
				// hWnd from current console in the long run.

				if (SUCCEEDED(hres))
				{
					strcpy(szGotPath, strLink.ToAscii());

					wxStrcpy( str_temp, strFullName );
					hres = psl->GetPath( (LPWSTR) str_temp, MAX_PATH,
								(WIN32_FIND_DATA *)&wfd, SLGP_UNCPRIORITY );
					strFullName = wxString(str_temp,wxConvUTF8);

					// check if pwe got the path
					if (!SUCCEEDED(hres))
					{
						wxMessageBox(wxT("GetPath failed!\n"));
						nStatus = 0;
					}

					/////////////////////////
					// now check if the file extension in the path is what we want!!!!
					wxString strTempPath = wxT("");
					wxString strTempName = wxT("");
					wxString strExt = wxT("");
					wxSplitPath( strFullName, &strTempPath, &strTempName, &strExt );
					if( !IsFileExtOk( strExt ) )
					{
						wxMessageBox(wxT("This shell link points to a non image file.\n"));
						nStatus = 0;
					}

					wxStrcpy( str_temp, strPath );
					hres = psl->GetWorkingDirectory( str_temp, MAX_PATH );
					if( !hres )
						strPath = wxT("");
					else
						strPath = wxString(str_temp,wxConvUTF8);

					// check if if's a directory
					if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						wxMessageBox(wxT("This points to a directory\n"));
						nStatus = 0;
					}

					// get icon path
					int nIconId;
					wxStrcpy( str_temp, strIconPath );
					hres = psl->GetIconLocation( str_temp, MAX_PATH, &nIconId );
					// if there is an icon in path
					if( !hres ) 
						strIconPath = wxT("");
					else
						strIconPath = wxString(str_temp,wxConvUTF8);
					
					// get arguments
					wxStrcpy( str_temp, strArgs );
					hres = psl->GetArguments( str_temp, MAX_PATH );
					if( !hres ) 
						strIconPath = wxT("");
					else
						strIconPath = wxString(str_temp,wxConvUTF8);
				}

			} else
			{
				wxMessageBox(wxT("IPersistFile Load Error\n"));
				nStatus = 0;
			}

			ppf->Release();
		} else
		{
			wxMessageBox(wxT("QueryInterface Error\n"));
			nStatus = 0;
		}

		psl->Release();

	} else
	{
		strMsg.Printf( wxT("CoCreateInstance Error - hres = %08x\n"), hres );
		wxMessageBox( strMsg );
		nStatus = 0;
	}

	return( nStatus );
}

// check if file extesion supported - todo: move this to digital image
// and integrate with fileio module
////////////////////////////////////////////////////////////////////
int CUnimap::IsFileExtOk( const wxString& strExt )
{
	wxString vectExt[] = { 
		wxT("jpg"), wxT("jpeg"), wxT("png"), wxT("tif"), wxT("tiff"),
		wxT("tga"), wxT("gif"), wxT("psd"), wxT("fit"), wxT("fits"),
		wxT("jpe"), wxT("bmp"), wxT("rle"), wxT("dib"), wxT("pcx"),
		wxT("pnm"), wxT("pbm"), wxT("pgm"), wxT("ppm"), wxT("pfm"),
		wxT("pam"), wxT("vda"), wxT("icb"), wxT("vst"), wxT("raw") };


	for( int i=0; i<25; i++ )
		if( strExt.CmpNoCase( vectExt[i] ) == 0 ) return( 1 );

	return( 0 );
}

////////////////////////////////////////////////////////////////////
int CUnimap::GetSystemId( int bUpdate )
{
	if( m_pMainConfig->m_strAppId.IsEmpty() || bUpdate )
	{
		CSystemInfo hdi;
		hdi.GetHardwareId( m_pMainConfig->m_strAppId );
	}

	return(0);
}

