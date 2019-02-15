////////////////////////////////////////////////////////////////////
// Name:		Frame header
// File:		frame.h
// Purpose:		interface for the CGUIFrame class.
//
// Created by:	Larry Lart on 04/06/2006
// Updated by:	Larry Lart on 06/02/2010 - integrate intruments
//
// Copyright:	(c) 2006-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _MAINFRAME_H
#define _MAINFRAME_H

// DEBUG
//#ifndef CRTDBG_MAP_ALLOC
//#define CRTDBG_MAP_ALLOC
//#endif

// wx
#include "wx/wxprec.h"
#include <wx/listctrl.h>
#include <wx/fontdlg.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/timer.h>
#include <wx/help.h>
#include <wx/helpwin.h>
#include <wx/msw/helpchm.h>

// opencv headers
#include "cv.h"
#include "highgui.h"

///////////
// defines
#define UNIMAP_FRAME_TITLE	wxT("UniMap")

/////////////////////
// IDs for the controls and the menu commands
enum
{
	wxID_TEXT_CMDVIEW	= 1500,

	wxID_BUTTON_RANDOM	= 1600,
	wxID_BUTTON_NEXT,
	wxID_BUTTON_BACK,
//	wxID_BUTTON_MATCH,
	wxID_BUTTON_SAVE,
	wxID_BUTTON_NEW,
	wxID_BUTTON_OPEN,
	wxID_CHECK_INCLUDE_ALL_IMAGES,

	// toolbar stuff
//	wxID_OPTION_DETECT,
//	wxID_OPTION_MATCH,
	wxID_GROUP_SELECT,
	wxID_IMAGE_SELECT,
	wxID_BUTTON_OBJFIND,
//	wxID_BUTTON_POSHINT,

	wxID_BUTTON_OBJ_DETECT_AND_MATCH,
	wxID_BUTTON_SKY_GRID,
	wxID_BUTTON_DISTORSION_GRID,
//	wxID_BUTTON_OBJDETECT,
//	wxID_BUTTON_OBJDATA_FETCH,
	wxID_BUTTON_MEASURE_DISTANCE,
	wxID_BUTTON_IMGZOOM,
	wxID_BUTTON_IMGINV,
	wxID_BUTTON_AREA_ADD,
	wxID_TOOL_SHOW_CTRL_THUMBS,
	wxID_TOOL_ONLINE_BROWSE,
	wxID_TOOL_ONLINE_UPLOAD_GROUP,
	wxID_TOOL_ONLINE_UPLOAD,

	// menu items

	//////////////////////
	// :: FILE :: menu items
	wxID_MENU_FILE_ADD_IMAGES,
	wxID_MENU_FILE_OPEN_IMAGE,
	wxID_MENU_FILE_SAVE_IMAGE,
	wxID_MENU_FILE_SAVE_ALL,
	wxID_MENU_FILE_EXPORT,
	wxID_MENU_FILE_ONLINE,
	wxID_MENU_FILE_ONLINE_BROWSE,
	wxID_MENU_FILE_ONLINE_UPLOAD_CUSTOM,
	wxID_MENU_FILE_PRINT_DIALOG,
	wxID_MENU_FILE_PRINT_PAGE_SETUP,
	wxID_MENU_FILE_PRINT_PREVIEW,
	wxID_MENU_FILE_EXIT,

	/////////////////////////
	// view objects
	wxID_MENU_VIEW_OBJECTS,
	// :: VIEW :: constellations
	wxID_MENU_VIEW_CONSTELLATIONS,
	wxID_MENU_VIEW_CONSTELLATIONS_WESTERN,
	wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_HARAY,
	wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_IAU,
	wxID_MENU_VIEW_CONSTELLATIONS_ARTWORK,
	wxID_MENU_VIEW_CONSTELLATIONS_CHINESE,
	wxID_MENU_VIEW_CONSTELLATIONS_EGYPTIAN,
	wxID_MENU_VIEW_CONSTELLATIONS_INUIT,
	wxID_MENU_VIEW_CONSTELLATIONS_KOREAN,
	wxID_MENU_VIEW_CONSTELLATIONS_LAKOTA,
	wxID_MENU_VIEW_CONSTELLATIONS_MAORI,
	wxID_MENU_VIEW_CONSTELLATIONS_NAVAJO,
	wxID_MENU_VIEW_CONSTELLATIONS_NORSE,
	wxID_MENU_VIEW_CONSTELLATIONS_POLYNESIAN,
	wxID_MENU_VIEW_CONSTELLATIONS_TUPI,
	wxID_MENU_VIEW_CONSTELLATIONS_SHOW,
	wxID_MENU_VIEW_CONSTELLATIONS_LINES,
	wxID_MENU_VIEW_CONSTELLATIONS_LABEL,
	// :: VIEW :: areas
	wxID_MENU_VIEW_IMAGE_AREAS,

	//  :: VIEW :: other items
	wxID_MENU_VIEW_GROUP_THUMBNAILS,
	wxID_MENU_VIEW_GROUP_FULLSCREEN,
	wxID_MENU_VIEW_FIND_OBJECT,
	wxID_MENU_VIEW_LOGGER,

	/////////////////
	// :: GROUP  :: menu items
	wxID_MENU_GROUP_NEW,
	wxID_MENU_GROUP_DELETE,
	wxID_MENU_GROUP_DETECT,
	wxID_MENU_GROUP_MATCH,
	wxID_MENU_GROUP_REGISTER,
	wxID_MENU_GROUP_ALIGN,
	wxID_MENU_GROUP_UPLOAD,
	wxID_MENU_GROUP_PROPERTIES,
	wxID_MENU_IMAGE_DETECT,
	wxID_MENU_IMAGE_CLEAR_DETECTED,
	wxID_MENU_IMAGE_HINT,
	wxID_MENU_IMAGE_CLEAR_MATCHED,
	wxID_MENU_IMAGE_CLEAR_AREAS,
	wxID_MENU_IMAGE_DELETE,
	wxID_MENU_IMAGE_MATCH,
	wxID_MENU_GROUP_HINT,
	wxID_MENU_GROUP_STACK,
	// ... add here extra external processors
	wxID_MENU_GROUP_DISCOVER,
	wxID_MENU_GROUP_STACK_WITH,
	// :: type of external image stacking software
	wxID_MENU_GROUP_STACK_REGISTAX,
	wxID_MENU_GROUP_STACK_DEEPSKYSTACKER,
	wxID_MENU_GROUP_STACK_IRIS,
	wxID_MENU_GROUP_STACK_MAXIMDL,
	wxID_MENU_GROUP_STACK_IMAGEPLUS,
	wxID_MENU_GROUP_STACK_REGISTAR,
	wxID_MENU_GROUP_STACK_K3CCDTOOLS,

	////////////////////////
	// :: IMAGE :: menu items
	wxID_MENU_IMAGE_CLEAR,
	wxID_MENU_IMAGE_3D_DISTORTION_GRID,
	wxID_MENU_IMAGE_3D_COLOR_MAP,
	wxID_MENU_IMAGE_3D_ANALYSIS,
	wxID_MENU_IMAGE_3D_SPATIALITY,
//	wxID_MENU_IMAGE_NOTES,
	wxID_MENU_IMAGE_UPLOAD,
	wxID_MENU_IMAGE_PROPERTIES,
	wxID_MENU_IMAGE_PROCESS_WITH,
	wxID_MENU_IMAGE_PROCESS_PHOTOSHOP,
	wxID_MENU_IMAGE_PROCESS_GIMP,
	wxID_MENU_IMAGE_PROCESS_PIXINSIGHT,
	wxID_MENU_IMAGE_PROCESS_MAXIMDL,
	wxID_MENU_IMAGE_PROCESS_IMAGEPLUS,
	wxID_MENU_IMAGE_PROCESS,
	wxID_MENU_IMAGE_PROCESS_OP,
	// :: operators
	wxID_MENU_IMAGE_PROCESS_OP_NEG,
	wxID_MENU_IMAGE_PROCESS_OP_ADD,
	wxID_MENU_IMAGE_PROCESS_OP_SUB,
	wxID_MENU_IMAGE_PROCESS_OP_MUL,
	// :: colors
	wxID_MENU_IMAGE_PROCESS_COL_BCG,
	wxID_MENU_IMAGE_PROCESS_COL_MIDTONE,
	wxID_MENU_IMAGE_PROCESS_COL_BALANCE,
	wxID_MENU_IMAGE_PROCESS_COL_COLORIZE,
	wxID_MENU_IMAGE_PROCESS_COL_DESATURATE,
	wxID_MENU_IMAGE_PROCESS_COL_HUESATURATION,
	// :: filters :: blur
	wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_SMOOTH,
	wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_MEAN,
	wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_LOWPASS,
	wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_GAUSSIAN,
	wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_BILATERAL,
	wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_SELECTIVE,
	wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_FFTLOW,
	// :: filters :: sharp
	wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_SHARPEN,
	wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_UNSHARP,
	wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_UNSHARPMASK,
	wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_LAPGAUS,
	wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_EMBOSS,
	// :: filters :: edge
	wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_GRADIENT,
	wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_ROBERSCROSS,
	wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_LAPLACIAN,
	wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_MARRHILDRETH,
	wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_CANNY,
	// :: filters :: gradient
	wxID_MENU_IMAGE_PROCESS_FILTER_GRADIENT_REMOVE,
	wxID_MENU_IMAGE_PROCESS_FILTER_GRADIENT_ROTATIONAL,
	// :: filters :: deconvolution
	wxID_MENU_IMAGE_PROCESS_FILTER_DECONV_RLUCY,
	wxID_MENU_IMAGE_PROCESS_FILTER_DECONV_WIENER,
	wxID_MENU_IMAGE_PROCESS_FILTER_DECONV_DCTEM,
	// :: filters - submenus
	wxID_MENU_IMAGE_PROCESS_FILTER_BLUR,
	wxID_MENU_IMAGE_PROCESS_FILTER_SHARP,
	wxID_MENU_IMAGE_PROCESS_FILTER_EDGE,
	wxID_MENU_IMAGE_PROCESS_FILTER_GRADIENT,
	wxID_MENU_IMAGE_PROCESS_FILTER_DECONVOLUTION,
	// :: process - single options
	wxID_MENU_IMAGE_PROCESS_FILTER_MORPHOLOGICAL,
	wxID_MENU_IMAGE_PROCESS_DENOISING,
	wxID_MENU_IMAGE_PROCESS_DEBLOOMER,
	wxID_MENU_IMAGE_PROCESS_FILTER_DIGITAL,
	// :: other process ids
	wxID_MENU_IMAGE_PROCESS_COL,
	wxID_MENU_IMAGE_PROCESS_FILTER,
	// and other procs ... 

	//////////////////////////
	// :: INSTRUMENTS :: menu items
	wxID_MENU_INSTRUMENTS_CONTROL_PANEL,

	//////////////////////////
	// :: ONLINE :: menu items
//	wxID_MENU_ONLINE_UPLOAD,
//	wxID_MENU_ONLINE_UPLOAD_IMAGE,
//	wxID_MENU_ONLINE_UPLOAD_GROUP,
///	wxID_MENU_ONLINE_UPLOAD_CUSTOM,
//	wxID_MENU_ONLINE_BROWSE_IMAGES,

	//////////////////////////
	// :: TOOLS :: menu items
	wxID_MENU_TOOLS_SLIDE_SHOW,
	wxID_MENU_TOOLS_ENVIRONMENT,
	wxID_MENU_TOOLS_WEATHER_SAT_IMG,
	wxID_MENU_TOOLS_WEATHER_CURRENT_FORECAST,
	wxID_MENU_TOOLS_WEATHER_SEEING_PRED,
	wxID_MENU_TOOLS_LIGHT_POLLUTION,
	wxID_MENU_TOOLS_OBSERVATORIES,
	wxID_MENU_TOOLS_SUN_VIEW,
	wxID_MENU_TOOLS_NEWS_HEADLINES,
	wxID_MENU_TOOLS_ASTRO_CALCULATOR,
	wxID_MENU_TOOLS_CALC_UNITS,
	wxID_MENU_TOOLS_CALC_OPTICAL,
	
	////////////////////////////
	// :: OPTIONS :: menu items
	wxID_MENU_OPTIONS_DETECT,
	wxID_MENU_OPTIONS_MATCH,
	wxID_MENU_OPTIONS_CATALOG,
	wxID_MENU_OPTIONS_OBSERVER,
	wxID_MENU_OPTIONS_ONLINE,
	wxID_MENU_OPTIONS_INSTRUMENTS,
//	wxID_MENU_OPTIONS_PRIVACY,
	wxID_MENU_OPTIONS_GLOBALS,

	////////////////////////////
	// :: HELP :: menu items
	wxID_MENU_HELP_TOPICS,
	wxID_MENU_HELP_FEEDBACK,
	wxID_MENU_HELP_ABOUT,

	// list events
	wxID_LIST_EVENT,

	// other
	wxID_STATUS_BAR,
	wxID_REMOTE_LOGGER,
	wxID_NOTIFICATION_DIALOG,
	wxID_CLOSE_WAIT_DIALOG,
	wxID_CLOSE_OBJ_DETECTION,
//	wxID_SHOW_OBJECT_DATA_DIALOG,
	wxID_CLOSE_WAIT_DIALOG_MATCH,
	wxID_CLOSE_DETECT_AND_MATCH,
	wxID_CLOSE_OBJECT_LIST_DETAILS,
	wxID_GROUP_THUMBNAILS_LIST,
	wxID_CLOSE_WAIT_DIALOG_STACK,

	// timer
	wxID_SLIDESHOW_TIMER,
	wxID_THREAD_ASTROIMAGE_LOADED,
};

////////////////////////////
// INSTRUMENTS id's gui ids
enum
{
	// toolbar
	wxID_INSTR_SHOW_CTRL_PANEL = 16300,
	wxID_INSTR_SHOW_CTRL_CAMERA,
	wxID_INSTR_CAMERA_SELECT,
	wxID_INSTR_SHOW_CTRL_FOCUS,
	wxID_INSTR_SHOW_CTRL_GRAPH,
	wxID_INSTR_GRAPH_A_SELECT,
	wxID_INSTR_GRAPH_B_SELECT,
	wxID_INSTR_GRAPH_C_SELECT,
	wxID_INSTR_SHOW_CTRL_SETTINGS,
	// graph weather event
	wxID_INSTR_CONDITIONS_UPDATE,
	// image thumbnails
	wxID_INSTR_THUMBNAILS_LIST,
	// camera
	wxID_INSTR_CAMERA_CONNECT_BUTTON,
	wxID_INSTR_CTRL_CAMERA_START,
	wxID_INSTR_CTRL_CAMERA_PAUSE,
	wxID_INSTR_CTRL_CAMERA_STOP,
	wxID_INSTR_CTRL_CAMERA_SETUP,
	wxID_INSTR_CTRL_CAMERA_FORMAT,
	wxID_INSTR_CTRL_CAMERA_USE_EXP,
	wxID_INSTR_CTRL_CAMERA_USE_FRAMES,
	wxID_INSTR_CTRL_CAMERA_USE_DELAY,
	// telescope connect/setup
	wxID_INSTR_TELESCOPE_CONNECT_BUTTON,
	wxID_INSTR_TELESCOPE_ALIGN_BUTTON,
	wxID_INSTR_TELESCOPE_PARK_BUTTON,
	wxID_INSTR_TELESCOPE_SETUP_BUTTON,
	// obj/ccord
//	wxID_INSTR_OBJ_CAT_SELECT,
//	wxID_INSTR_OBJ_NAME_SELECT,
//	wxID_INSTR_OBJ_NAME_ENTRY,
	wxID_INSTR_TELESCOPE_GOTO,
	wxID_INSTR_TELESCOPE_OBJECT,
	wxID_INSTR_TELESCOPE_GUIDE,
	wxID_INSTR_TELESCOPE_TRACK,
	wxID_INSTR_TELESCOPE_CIRCLES,
	// script 
	wxID_INSTR_TELESCOPE_SCRIPT_PLAY,
	wxID_INSTR_TELESCOPE_SCRIPT_SELECT,
	// func 2
	wxID_INSTR_TELESCOPE_UNWRAP,
	wxID_INSTR_TELESCOPE_HIPREC,
	wxID_INSTR_TELESCOPE_TEMPCTRL,
	wxID_INSTR_TELESCOPE_SYNC,
	// navigation buttons
	wxID_INSTR_MOVE_WEST,
	wxID_INSTR_MOVE_EAST,
	wxID_INSTR_MOVE_NORTH,
	wxID_INSTR_MOVE_SOUTH,
	wxID_INSTR_MOVE_STOP,
	// thread cutom updates
	wxID_TELESCOPE_INFO_CONNECTED,
	wxID_TELESCOPE_INFO_DISCONNECTED,
	wxID_TELESCOPE_INFO_UPDATE,
	// focuser
	wxID_INSTR_FOCUS_CONNECT,
	wxID_INSTR_FOCUS_IN,
	wxID_INSTR_FOCUS_ZERO,
	wxID_INSTR_FOCUS_OUT,
	wxID_INSTR_FOCUS_SETUP,
	wxID_INSTR_FOCUS_USE_AUTO,
	wxID_INSTR_FOCUS_USE_UNITS,
	wxID_INSTR_FOCUS_USE_SLOPE,
	// camera
	wxID_DCAMERA_INFO_CONNECTED,
	wxID_DCAMERA_INFO_DISCONNECTED,
	wxID_DCAMERA_INFO_PHOTO_TAKEN,
	wxID_DCAMERA_GOT_NEW_IMAGE
};

// local headers
#include "../httpengine/wxcurl_http.h"
#include "custom_wxwidgets.h"
#include "../image/histogram.h"
#include "../observer/observer_hardware.h"
#include "../camera/camera.h"

static wxMenuBar *menuBar = NULL;

// external types
//extern typedef struct DefHWDevice;

// external classes
class CUnimap;
class CFrameTaskBarIcon;
class CGroupImgView;
class CAstroImgView;
class CLogger;
class CAstroImage;
class CSextractor;
class CStarMatch;
class CImageDb;
class CSky;
class CConfigMain;
class CConfigDetect;
class CWaitDialog;
class CWaitDialogMatch;
class CWaitDialogStack;
class CUnimapWorker;
class CDistToolDataWindow;
class CInstrumentsCfgDlg;
// external classes - for instruments
class CObserver;
class CTelescope;
class CFocuser;
class CTelescopeWorker;
class CCameraView;
class CGroupImgView;
class CRadialProfileView;
class CHistogramView;
class CGraphBaseView;
class CCamera;
class CDCameraWorker;
class CVCameraWorker;
class CImageShift;
//class wxTTButton;
//class wxBitmapToggleButton;
class mmMultiButton;

// class :: CGUIFrame
//////////////////////////////////////////////////////////
class CGUIFrame : public wxFrame
{
// public methods	
public:
	CGUIFrame( wxFrame *frame, const wxPoint& pos, const wxSize& size );
	virtual ~CGUIFrame( );

	void SetConfig( CConfigMain* pMainConfig, CConfigDetect* pDetectConfig );
	void SetLogger( CLogger* pLogger );
	void SetSky( CSky* pSky );

	void SetParentApp( CUnimap *pUnimap );
	
	// set full screen
	void SetFullScreen( );

	int AddGroupToSelect( const wxString& strGroupName );
	int AddImageToSelect( wxString& strImageName );
	int AddImageToGroupThumbs( int nId, wxString& strImageName );
	void AssignImagesToGroupThumbnails( wxImageList* vectImg, int nImg );
	int SetGroupThumbnailImage( int nId, wxBitmap& bmpImg );

	int ClearImageBrowsingList( );
	void ClearGroupThumbnails( );
	int SelectImageFromList( int nIndex );
//	int SelectGroupImageThumbnail( int nIndex );
	
	void UnloadSelectedImage( int bReset=0 );
	void LoadSelectedImage( int bKeepInstr=0 );
	void LoadImageWait( int group_id, int image_id );
	void LoadImageBG( int group_id, int image_id );
	void SetAstroImageView( );

	// status bar methods
	void SetStatusZoom( int nZoom );
	void SetStatusXY( int nX, int nY );
	void SetStatusRaDec( double nRa, double nDec );
	void SetStatusDetected( int nDet );
	void SetStatusMatched( int nMatch );
	void SetStatusDso( int nDso );
	void SetStatusCatStar( int nCatStarNo );

//	void SetDetectOptions( );
	void SetPosHint( );
	void SetGroupPosHint( );

//	int GetViewObjects( int nMenuId );
	void SetViewAllObjects( int bNoCnstl=0 );

	// main image call processing methods
	int RunObjectDetection( int bRun );
	int RunObjectMatching( int bRun );
	int RunObjectDataFetch( );

	// other aux methods
	void SetInstrumentsSettings( CInstrumentsCfgDlg* pDlg );

	// get next slide
	void GetNextSlide( int bDirection=0 );
	void StopSlideShow( );
	// show thumbnails
	void ViewGroupThumbnails( int bState, int nId );
	// set menu states
	void SetMenuState( int nType, int bState );
	void SetMenuAppsState( );

////////////////////////
// INSTRUMENTS METHODS
public:
    void AddInstrumentsPanel( );
	void DeleteInstrumentsPanel( );

	void SetIntrumentsConfig( CTelescope* pTelescope, CFocuser* pFocuser, 
								CCamera* pCamera, CObserver* pObserver );

	void SetTelescopeRaDecEntry( double ra, double dec );
	void SlewToPosition( int bRemote=0 );

	// gui methods
	void SetCameraImage( wxImage* pImage, IplImage* pIplImage=NULL, int bSetView=1 );
	void SetTelescopeInfo( double ra, double dec );
//	void SetThumbnails( );
	void SetHistogram( );

////////////////////
// public data
public:
	CUnimap*		m_pUnimap;
	CUnimapWorker*	m_pUnimapWorker;
	CLogger*		m_pLogger;
	CConfigMain*	m_pMainConfig;
	CConfigDetect*	m_pDetectConfig;
	// taskbar icon
	CFrameTaskBarIcon* m_pFrameTaskBarIcon;

	int m_bIsClosing;
	// slide show times
	wxTimer* m_pSlideShowTimer;
	int m_bSlideLoading;
	int m_nSlideShowStatus;

	wxTextCtrl* m_pLoggerView;

	wxStaticBox* m_pImageBox;

	CGroupImgView*	m_pGroupImgView;
	CAstroImgView*	m_pImgView;
//	CImageView3D*	m_pImgView3D;

//	wxChoice*	m_pDetectOption;
	wxChoice*	m_pGroupSelect;
	wxChoice*	m_pImageSelect;

//	wxPanel *panel1;
	wxFlexGridSizer* sizerGroupAndImage;

	/////////////////
	// print data
	wxPrintData*			m_pPrintData;
	wxPageSetupDialogData*	m_pPageSetupData;
	int m_bHasPageSetup;

	// group path
	CImageDb*		m_pImageDb;
	
	CAstroImage*	m_pAstroImage;
	CSextractor*	m_pSextractor;
	CStarMatch*		m_pStarMatch;
	CSky*			m_pSky;

	int m_nCurrentGroup;
	int m_nCurrentImage;

	// gui setting and flags
	int m_nWidth;
	int m_nHeight;

	int m_bGroupThumbnailsInitialized;
	int m_bInit;

	// backup vars 
	double m_nLastCurRa;
	double m_nLastCurDec;

	// full screen flag
	bool m_bIsFullScreen;
	// iconized
	int m_bIconized;

	// distance tool
	CDistToolDataWindow* m_pDistanceTool;

#if USE_EXT_HELP
   wxExtHelpController      m_rHelp;
#else
//   wxHelpController         m_rHelp;
   wxCHMHelpController		m_rHelp;
#endif

/////////////////////////
// INSTRUMNETS DATA
public:
	wxPanel* m_pGraphPanel;
	wxPanel* m_pCamTelPanel;

	wxToolBar* m_pInstrToolBar;
	int m_bInstrActive;
	int m_bInstrCameraView;
	int m_bIsInstrClosing;

	double m_nTelescopeTargetRa;
	double m_nTelescopeTargetDec;
	int m_nCurrentSiteId;

	wxImage m_rImage;
	CImageShift* m_pProcImgShift;

	// main objects
	CObserver* m_pObserver;
	CTelescope* m_pTelescope;
	CTelescopeWorker* m_pTelescopeWorker;
	CFocuser* m_pFocuser;
	CCamera* m_pCamera;
	int m_nCurrentCamera;
	CDCameraWorker* m_pDCameraWorker;
	CVCameraWorker* m_pVCameraWorker;
	// camera devices
	std::vector<DefHWDevice> m_vectCamDevices;

	// main elements
//	CGroupImgView*		m_pGroupImgView;
//	CCameraView*		m_pImgView; ------------ here move from camera view to astroview
	CHistogramView*		m_pHistogramView;
	CImageHistogram		m_rHistogram;
	CRadialProfileView* m_pFocusView;
	CGraphBaseView*		m_pCondView;
	// other 
	wxStaticBox* pCondViewBox;
	// graphs selectors
	wxChoice*		m_pGraphASelect;
	wxChoice*		m_pGraphBSelect;
	wxChoice*		m_pGraphCSelect;

	// camera
	wxChoice*		m_pCameraSelect;
	mmMultiButton*	m_pCameraConnect;
	wxBitmapButton*	m_pCameraStart;
	wxBitmapButton*	m_pCameraPause;
	wxBitmapButton*	m_pCameraStop;
	wxBitmapButton*	m_pCameraSetup;
	wxBitmapButton*	m_pCameraFormat;
	// camera :: exposure
	wxCheckBox* m_pUseCamExp;
	wxStaticText* m_pCamExpLabel;
	wxSpinCtrl* m_pCamExpValue;
	// camera :: frames
	wxCheckBox* m_pUseCamFrames;
	wxStaticText* m_pCamFramesLabel;
	wxSpinCtrl* m_pCamFramesValue;
	// camera :: delay
	wxCheckBox* m_pUseCamDelay;
	wxStaticText* m_pCamDelayLabel;
	wxSpinCtrl* m_pCamDelayValue;

	/////////////////////////
	// telescope connection
	mmMultiButton* m_pTelescopeConnectButton;
	wxBitmapButton* m_pTelescopeAlignButton;
	wxBitmapButton* m_pTelescopeParkButton;
	wxBitmapButton*	m_pTelescopeSetupButton;
	// info data
	mmMultiButton*	m_pGotoButton;
	wxBitmapButton*	m_pTargetPick;
	wxStaticText*	m_pInfoScreen;

	// object nav
//	wxChoice*	m_pObjCatSelect;
//	wxChoice*	m_pObjNameSelect;
//	wxTextCtrl*	m_pObjNameEntry;
	// coord nav

	wxStaticText*	m_pTargetRaLabel;
	wxTextCtrl*	m_pTargetRa;
	wxStaticText*	m_pTargetDecLabel;
	wxTextCtrl*	m_pTargetDec;
	// function buttons 1
	mmMultiButton*	m_pTrackButton;
	mmMultiButton*	m_pGuideButton;
	mmMultiButton*	m_pCircleButton;
	// script 
	mmMultiButton*	m_pScriptPlayButton;
	wxChoice*		m_pScriptSelect;
	// function buttons 2
//	wxBitmapButton*	m_pUnwrapButton;
//	wxBitmapButton*	m_pHiprecButton;
//	wxBitmapButton*	m_pTempCtrlButton;
//	wxBitmapButton*	m_pFanButton;

	// telescope control buttons
	wxSTButton*	m_pMoveWest;
	wxSTButton*	m_pMoveEast;
	wxSTButton*	m_pMoveNorth;
	wxSTButton*	m_pMoveSouth;
	wxSTButton*	m_pMoveStop;

	///////////
	// focuser elements
	mmMultiButton*	m_pFocusConnect;
	wxSTIButton*	m_pFocusIn;
	wxSTIButton*	m_pFocusZero;
	wxSTIButton*	m_pFocusOut;
	wxSTIButton*	m_pFocusSetup;
	// focuser auto adjutment
	wxCheckBox*		m_pFocusUseAuto;
	wxStaticText*	m_pFocusUseAutoLabel;
	// focuser units
	wxCheckBox* m_pFocusUseUnits;
	wxStaticText* m_pFocusUseUnitsLabel;
	wxSpinCtrl* m_pFocusUnits;
	// focuser slope
	wxCheckBox* m_pFocusUseSlope;
	wxStaticText* m_pFocusUseSlopeLabel;
	wxSpinCtrl* m_pFocusSlope;

///////////////////////////////
// message map functions
protected:
	// secial gui config set methods
	void SetConstellationsMenu( );

	void OnMinimize( wxIconizeEvent& pEvent );

	//////////////////
	// FILE MENU HANDLERS
	void OnAddImages( wxCommandEvent& pEvent );
	void AddImages( );
	void OnAddImage( wxCommandEvent& pEvent );
	void OnSaveAll( wxCommandEvent& pEvent );
	void OnSaveImage( wxCommandEvent& pEvent );
	void OnFileExport( wxCommandEvent& pEvent );
	void OnPrint( wxCommandEvent& pEvent );
	void OnPrintPreview(wxCommandEvent& pEvent );
	void OnPageSetup( wxCommandEvent& pEvent );
	void OnExitApp( wxCommandEvent& pEvent );

	// tools image navigation
	void OnBackImage( wxCommandEvent& pEvent );
	void OnNextImage( wxCommandEvent& pEvent );

	void OnDetectAndMatch( wxCommandEvent& pEvent );
	void OnDetect( wxCommandEvent& pEvent );
	void OnMatch( wxCommandEvent& pEvent );
	void OnPosHint( wxCommandEvent& pEvent );
	void OnSkyGrid( wxCommandEvent& pEvent );
	void OnDistortionGrid( wxCommandEvent& pEvent );

	void OnImageZoomTool( wxCommandEvent& pEvent );
	void OnImageAddArea( wxCommandEvent& pEvent );

//	void OnDetectOption( wxCommandEvent& pEvent );

	void OnGroupSelect( wxCommandEvent& pEvent );
	void OnImageSelect( wxCommandEvent& pEvent );

	void OnViewLogger( wxCommandEvent& pEvent );
	void OnViewObjects( wxCommandEvent& pEvent );
	void OnViewConstellations( wxCommandEvent& pEvent );
	void SetMenuCnstlState( int bState );
	void OnViewImageAreas( wxCommandEvent& pEvent );
	void OnView3dDistGrid( wxCommandEvent& pEvent );
	void OnView3dColorMap( wxCommandEvent& pEvent );
	void OnView3dSpatiality( wxCommandEvent& pEvent );

	// view find object
	void OnViewFindObject( wxCommandEvent& pEvent );
	// measure distance tool
	void OnMeasureDistanceTool( wxCommandEvent& pEvent );

	void OnSize( wxSizeEvent& pEvent );
	void OnClose( wxCloseEvent& pEvent );

	// custom even handlers
	void OnRemoteLogger( wxCommandEvent& pEvent );
	void OnNotificationMessage( wxCommandEvent& pEvent );

	// thumbnail select
	void OnViewGroupThumbnails( wxCommandEvent& pEvent );
	void UpdateGroupThumbnails( int nGroup );
	void OnViewFullScreen( wxCommandEvent& pEvent );
	void OnImageThumbnailSelect( wxListEvent& pEvent );

	////////////////////////
	// image handlers
	void OnImageNotes( wxCommandEvent& pEvent );
	void OnImageProperty( wxCommandEvent& pEvent );
	// :: image process handlers
	void OnProcessImageOperator( wxCommandEvent& pEvent );
	void OnProcessImageColors( wxCommandEvent& pEvent );
	void OnProcessImageBlur( wxCommandEvent& pEvent );
	void OnProcessImageSharp( wxCommandEvent& pEvent );
	void OnProcessImageEdge( wxCommandEvent& pEvent );
	void OnProcessImageGradient( wxCommandEvent& pEvent );
	void OnProcessImageDeconv( wxCommandEvent& pEvent );
	void OnProcessImageFilter( wxCommandEvent& pEvent );
	// image open with third party application
	void OnImageOpenWith( wxCommandEvent& pEvent );

	////////////////
	// group menu handlers
	void OnNewGroup( wxCommandEvent& pEvent );
	void OnDeleteGroup( wxCommandEvent& pEvent );
	void OnGroupDetect( wxCommandEvent& pEvent );
	void OnGroupMatch( wxCommandEvent& pEvent );
	void OnGroupHint( wxCommandEvent& pEvent );
	void OnGroupRegister( wxCommandEvent& pEvent );
	void OnGroupAlign( wxCommandEvent& pEvent );
	void OnGroupDynamicAnalysis( wxCommandEvent& pEvent );
	void OnGroupStack( wxCommandEvent& pEvent );
	void OnGroupStackWith( wxCommandEvent& pEvent );
	void OnGroupProperties( wxCommandEvent& pEvent );

	void OnDetectClear( wxCommandEvent& pEvent );
	void OnMatchClear( wxCommandEvent& pEvent );
	void OnClearAreas( wxCommandEvent& pEvent );
	void OnImageDelete( wxCommandEvent& pEvent );

	void OnCatalogDownload( wxCommandEvent& pEvent );

	//////////////////
	// intruments handlers
	void OnInstrumentsControl( wxCommandEvent& pEvent );

	//////////////////
	// online handlers
	void OnOnlineUpload( wxCommandEvent& pEvent );
	void OnOnlineBrowse( wxCommandEvent& pEvent );
	// event progress bar
//	void OnUploadProgress( wxCurlProgressEvent& pEvent );

	//////////////////
	// tools handlers
	void OnToolsSlideShow( wxCommandEvent& pEvent );
	void OnToolsWeather( wxCommandEvent& pEvent );
	void OnToolsSunView( wxCommandEvent& pEvent );
	void OnWeatherCurrentForecast( wxCommandEvent& pEvent );
	void OnWeatherSeeingPredict( wxCommandEvent& pEvent );
	void OnToolsLightPollution( wxCommandEvent& pEvent );
	void OnToolsObservatories( wxCommandEvent& pEvent );
	void OnNewsTool( wxCommandEvent& pEvent );
	// calcultors
	void OnCalcUnits( wxCommandEvent& pEvent );
	void OnCalcOptical( wxCommandEvent& pEvent );

	//////////////
	// OPTIONS MENU HANDLERS
	void OnConfigDetection( wxCommandEvent& pEvent );
	void ConfigDetection();
	void OnConfigMatch( wxCommandEvent& pEvent );
	void OnConfigCatalog( wxCommandEvent& pEvent );
	void ConfigCatalog( );
	void OnConfig( wxCommandEvent& pEvent );
	void OnConfigObserver( wxCommandEvent& pEvent );
	void OnConfigOnline( wxCommandEvent& pEvent );
	// intrumets 
	void OnConfigInstruments( wxCommandEvent& pEvent );
	// on files drop
	void OnImagesDrop( wxDropFilesEvent& pEvent );

	/////////////
	// HELP MENU HANDLERS
	void OnHelp( wxCommandEvent& pEvent );
	void OnFeedback( wxCommandEvent& pEvent );
	void OnAbout( wxCommandEvent& pEvent );

////////////////////////////
// INSTRUMENTS PRIVATE DATA
private:
	// toolbar
	void OnActivateCameraView( wxCommandEvent& pEvent );
	void OnSelectCamera( wxCommandEvent& pEvent );
//	void OnThumbs( wxCommandEvent& pEvent );
	void OnInstrSettings( wxCommandEvent& pEvent );

	////////////////////////////
	// CAMERA HANDLERS
	void OnCameraOptions( wxCommandEvent& pEvent );
	void OnCameraConnect( wxCommandEvent& pEvent );
	// digital camera threads
	void StartDCamWorker( );
	void StopDCamWorker( );
	// video camera threads
	void StartVCamWorker( );
	void StopVCamWorker( );
	// camera actions
	void OnCameraUpdateInfo( wxCommandEvent& pEvent );
	void OnCameraAction( wxCommandEvent& pEvent );
	void OnCameraSetup( wxCommandEvent& pEvent );
	void OnCameraFormat( wxCommandEvent& pEvent );
	// camera control states
	void SetCameraCtrlState( int bState );
	void SetCameraExpCtrlState( int bState );
	void SetCameraFramesCtrlState( int bState );
	void SetCameraDelayCtrlState( int bState );

	///////////////////////////////
	// TELESCOPE HANDLERS
	void OnScope( wxCommandEvent& pEvent );
	void OnTelescopeConnect( wxCommandEvent& pEvent );
	void OnTelescopeSetup( wxCommandEvent& pEvent );
	void OnTelescopeObjSet( wxCommandEvent& pEvent );
	void StartTelWorker( );
	void StopTelWorker( );
	// move handlers
	void OnTelescopeGoto( wxCommandEvent& pEvent );
	void OnTelescopeMove( wxCommandEvent& pEvent );
	void SetGuiOnTelescopeState( int bState );

	////////////////////////////
	// FOCUSER HANDLERS
	void OnFocusOptions( wxCommandEvent& pEvent );
	void OnFocuserConnect( wxCommandEvent& pEvent );
	void OnFocuserMove( wxCommandEvent& pEvent );
	void SetGuiFocuserPanelState( int nState );
	void SetFocuserPanelAutoState( int nState );
	void SetFocuserPanelUnitsState( int nState );
	void SetFocuserPanelSlopeState( int nState );

	// custom thread handlers
	void OnUpdateInfo( wxCommandEvent& pEvent );
	void OnCondUpdate( wxCommandEvent& pEvent );

	// timer handlers
	void OnSlideShowTimer( wxTimerEvent& pEvent );
	void DeleteSlideShowTimer( );
	// astro image loaded
	void OnAstroImageLoaded( wxCommandEvent& pEvent );

	/////////////////////////
	// events table
	DECLARE_EVENT_TABLE()
};

#endif
