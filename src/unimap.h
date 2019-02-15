////////////////////////////////////////////////////////////////////
// Name:		Unimap Header
// File:		unimap.h
// Purpose:		Main header with main defines for unimap
//
// Created by:	Larry Lart on 26/04/2007
// Updated by:	Larry Lart on 01/02/2010 - clean timer
//
////////////////////////////////////////////////////////////////////

#ifndef _UNIMAP_H
#define _UNIMAP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__ )
//#define new DEBUG_NEW
//#endif

#define UNIMAP_LOG_FILE		wxT("unimap.log")

#include "resource.h"       // main symbols

#include "wx/wxprec.h"
#include "wx/thread.h"
#include <wx/intl.h>
#include <wx/cursor.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// defines
#define SYSTEM_FILE_TYPE_LINK		0
#define SYSTEM_FILE_TYPE_IMAGE		1

// laguage def rec
typedef struct
{
	wxLanguage	id;
	wxString	name;

} DefGuiLangRec;

// language display strings
const DefGuiLangRec g_vectGuiLanguage[] =
{
	{ wxLANGUAGE_ENGLISH_US, wxT("English (U.S.)") },	// :: 0
	{ wxLANGUAGE_GERMAN, _T("German") },				// :: 1
	{ wxLANGUAGE_ITALIAN, _T("Italian") },				// :: 2
	{ wxLANGUAGE_FRENCH, _T("French") },				// :: 3
	{ wxLANGUAGE_SPANISH, _T("Spanish") },				// :: 4
	{ wxLANGUAGE_PORTUGUESE, _T("Portuguese") },		// :: 5
	{ wxLANGUAGE_DUTCH, _T("Dutch") },					// :: 6
	{ wxLANGUAGE_GREEK, _T("Greek") },					// :: 7
	{ wxLANGUAGE_RUSSIAN, _T("Russian") },				// :: 8
	{ wxLANGUAGE_CHINESE_SIMPLIFIED, _T("Chinese") },	// :: 9
	{ wxLANGUAGE_JAPANESE, _T("Japanese") },			// :: 10
	{ wxLANGUAGE_VIETNAMESE, _T("Vietnamese") },		// :: 11
	{ wxLANGUAGE_HINDI, _T("Hindi") },					// :: 12
	{ wxLANGUAGE_CZECH, _T("Czech") },					// :: 13
	{ wxLANGUAGE_POLISH, _T("Polish") },				// :: 14
	{ wxLANGUAGE_SWEDISH, _T("Swedish") },				// :: 15
	{ wxLANGUAGE_ROMANIAN, _T("Romanian") },			// :: 16
	{ wxLANGUAGE_TURKISH, _T("Turkish") },				// :: 17
	{ wxLANGUAGE_ARABIC, _T("Arabic") }					// :: 18
};
const int g_nGuiLanguage = 19;

// define cursors
#define UNIMAP_CURSOR_IMAGE_DEFAULT		0
#define UNIMAP_CURSOR_IMAGE_DRAG		1
#define UNIMAP_CURSOR_IMAGE_ADD_AREA	2
#define UNIMAP_CURSOR_IMAGE_ZOOM_IN		3
#define UNIMAP_CURSOR_IMAGE_ZOOM_OUT	4
#define UNIMAP_CURSOR_AREA_MOVE			5
#define UNIMAP_CURSOR_AREA_RESIZE_AC	6
#define UNIMAP_CURSOR_AREA_RESIZE_BD	7
#define UNIMAP_CURSOR_DIST_MEASURMENT	8
#define UNIMAP_CURSOR_OBJ_DETAIL		9

// external classes
class CUnimap;
class CLogger;
class CGUIFrame;
class CImageDb;
//class CAstroImage;
class CSky;
class CSextractor;
class CStarMatch;
class CConfigDetect;
class CConfigMain;
class CUnimapWorker;
class CObserver;
class CCamera;
class CTelescope;
class CUnimapOnline;
class CFocuser;
class CSoundEngine;
class CAppsManager;
class CUnimapService;

// global pointer to unimap app
//CUnimap* g_pUnimap;

// main app/class unimap
///////////////////////////////////////////////////
class CUnimap : public wxApp
{
// public methods
public:
	CUnimap( );
	virtual ~CUnimap( );
	// to take care of some thing
	virtual bool Initialize( int& argc, wxChar **argv ){ CoUninitialize(); return( wxApp::Initialize( argc, argv ) ); }
	// init data folders
	int InitDataFolders( );

	int LoadOnStartup( );

	// services
	int StartServices();
	int StopServices();
	int RestartServices();

	// sound engine methods
	CSoundEngine* StartSoundEngine( const wxString& strSoundFile );
	int StopSoundEngine();

	// input files handlers
	int ProcessInputFile( const wxString& strInputFile, wxString& strName, 
						wxString& strFullName, wxString& strPath, wxString& strWorkPath,
						wxString& strIconPath, wxString& strArgs, int& nType );
	int ProcessShellLink( const wxString& strLink, wxString& strFullName, 
						wxString& strPath, wxString& strIconPath, wxString& strArgs );
	int IsFileExtOk( const wxString& strExt );

	// load all the app cursors
	void LoadCursors();
	wxCursor* GetCursor( int nId );

	// Get System Unique Id
	int GetSystemId( int bUpdate=0 );

/////////////////
// public data
public:

	// translation object
	wxLocale m_pLocale;

	// thrid party application manager
	CAppsManager* m_pAppsMgr;

	// other main objects
	CLogger*		m_pLogger;
	CGUIFrame*		m_pFrame;
	CConfigMain*	m_pMainConfig;
	CConfigDetect*	m_pDetectConfig;
	// and the working thread
	CUnimapWorker*	m_pUnimapWorker;
	// unimap service thread
	CUnimapService*	m_pUnimapService;

	// :: online connector
	CUnimapOnline*	m_pUnimapOnline;
	// :: image database
	CImageDb*		m_pImageDb;
	// :: sky
	CSky*	m_pSky;
	// :: observer
	CObserver*	m_pObserver;
	// :: camera
	CCamera*	m_pCamera;
	// :: telescope
	CTelescope*	m_pTelescope;
	// :: focuser
	CFocuser*	m_pFocuser;

	CSextractor*	m_pStarFinder;
	CStarMatch*		m_pStarMatch;

	// sound recorder/player
	CSoundEngine*	m_pSoundEngine;

	//////
	// cursors
	wxCursor*	m_vectCursors[11];

// protected methods
protected:
	bool OnInit( );
	int OnExit( );
	void OnKeyDown( wxKeyEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
