////////////////////////////////////////////////////////////////////
// Name:		implementation of the CGUIFrame class
// File:		frame.cpp
// Purpose:		GUI mainframe system control methods
//
// Created by:	Larry Lart on 04/07/2006
// Updated by:	Larry Lart on 14/04/2010
//
// Copyright:	(c) 2004-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

///////////////// DEBUG ///////////////////////
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
///////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include "wx/wxprec.h"
#include "wx/statline.h"
#include "wx/minifram.h"
#include "wx/thread.h"
#include <wx/tokenzr.h>
#include <wx/printdlg.h>
#include <wx/print.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

//////////////
// elynx headers
#include <elx/core/CoreMacros.h>
#include <elx/math/TransfertFunction.h>
#include <elx/image/AbstractImageManager.h>
#include <elx/image/ImageFileManager.h>
#include <elx/math/MathCore.h>
#include <elx/image/ImageVariant.h>
#include <elx/image/ImageOperatorsImpl.h>
#include <elx/image/PixelIterator.h>

// include toolbar icons
//#include "../back.xpm"
//#include "../next.xpm"
//#include "../random.xpm"
#include "../../resources/xpm/toolchec.xpm"
#include "../../resources/xpm/new_dir.xpm"
#include "../../resources/xpm/save.xpm"
#include "../../resources/xpm/open.xpm"
#include "../../resources/xpm/find.xpm"
//#include "../center.xpm"
//#include "../skygrid.xpm"
//#include "../distgrid.xpm"
//#include "../target.xpm"
//#include "../detect_stars.xpm"
//#include "../linearrow.xpm"
// instruments xpms
#include "../../resources/xpm/camera.xpm"
//#include "../resources/small/earth-picture.xpm"

//#include "../resources/objdata.xpm"

// program icon
#ifndef WIN32
#include "../../resources/xpm/unimap.xpm"
#endif

#include "../graphics/tps/vec.h"
//#include "../graphics/tps/ludecomposition.h"

// other includes
#include "about.h"
#include "taskbar_icon.h"
#include "dist_tool_view.h"
#include "astro_img_view.h"
#include "print.h"
#include "image3dbase.h"
#include "imgview3dcolormap.h"
#include "imgview3ddistmap.h"
#include "imgview3dspacedist.h"
#include "imgproperty.h"
#include "groupproperty.h"
#include "groupview.h"
#include "poshintdlg.h"
#include "catalogconfig.h"
#include "waitdialog.h"
#include "waitdialogmatch.h"
#include "waitdialogstack.h"
#include "../unimap.h"
#include "../util/func.h"
#include "../util/image_func.h"
#include "../image/astroimage.h"
#include "../image/image_note.h"
#include "../image/imagedb.h"
#include "../image/imagegroup.h"
#include "../sky/sky.h"
#include "../sky/sky_finder.h"
//#include "../sky/catalog.h"
#include "../logger/logger.h"
#include "../unimap_worker.h"
#include "../unimap_service.h"
#include "../online/unimap_online.h"
#include "starviewdlg.h"
#include "image_notes_dlg.h"
#include "image_save_dlg.h"
#include "group_add_dlg.h"
#include "file_export_dlg.h"
#include "mmMultiButton.h"
// :: tools
#include "weather_satvis_dlg.h"
#include "sun_view_dlg.h"
#include "weather_cforecast_dlg.h"
#include "weather_seeing_dlg.h"
#include "light_pollution_dlg.h"
#include "observatories_dlg.h"
#include "../news/news.h"
#include "news_dlg.h"
#include "user_feedback_dlg.h"
// :: options
#include "../config/config.h"
#include "../config/detectconfig.h"
#include "../config/mainconfig.h"
#include "matchconfig.h"
#include "mainconfigview.h"
#include "detectconfigview.h"
#include "observer_cfg_dlg.h"
#include "online_cfg_dlg.h"
#include "upload_custom_dlg.h"
#include "online_browse_dlg.h"
#include "slideshow_dlg.h"
#include "calc_units_dlg.h"
#include "tlens_select_dlg.h"
#include "view_objects_dlg.h"
//#include "instruments_control_dlg.h"
#include "instruments_cfg_dlg.h"
#include "../telescope/focuser.h"
#include "../observer/observer.h"
#include "../observer/observer_site.h"
#include "../weather/weather.h"
#include "../util/apps_manager.h"

///////////
// :: filters
#include "process/colors_dlg.h"
#include "process/blur_dlg.h"
#include "process/sharp_dlg.h"
#include "process/edge_dlg.h"
#include "process/gradient_dlg.h"
#include "process/deconv_dlg.h"
#include "process/filter_dlg.h"

#define NOCTYPES
//#include "../proc/fits/fitscat.h"
#include "../proc/sextractor.h"
#undef NOCTYPES

#include "../match/starmatch.h"
#include "groupdlg.h"
#include "finddlg.h"

// main headers
#include "frame.h"
//#include "frame_instruments.h"

// get external unimap
DECLARE_APP(CUnimap)

using namespace eLynx;
using namespace eLynx::Image;

// class :: CGUIFrame
/////////////////////////////////////////////////////////////////////////////
// CGUIFrame
BEGIN_EVENT_TABLE(CGUIFrame, wxFrame)
	// process drag files event
	EVT_DROP_FILES( CGUIFrame::OnImagesDrop )

	/////////////////////
	// :: FILE :: menu items
	EVT_MENU( wxID_MENU_FILE_OPEN_IMAGE, CGUIFrame::OnAddImage )
	EVT_MENU( wxID_MENU_FILE_ADD_IMAGES, CGUIFrame::OnAddImages )
	EVT_MENU( wxID_MENU_FILE_SAVE_IMAGE, CGUIFrame::OnSaveImage )
	EVT_MENU( wxID_MENU_FILE_SAVE_ALL, CGUIFrame::OnSaveAll )
	EVT_MENU( wxID_MENU_FILE_EXPORT, CGUIFrame::OnFileExport )
//	EVT_MENU( wxID_MENU_FILE_OPTIONS, CGUIFrame::OnConfig )
	EVT_MENU( wxID_MENU_FILE_PRINT_PAGE_SETUP, CGUIFrame::OnPageSetup )
	EVT_MENU( wxID_MENU_FILE_PRINT_PREVIEW, CGUIFrame::OnPrintPreview )
	EVT_MENU( wxID_MENU_FILE_PRINT_DIALOG, CGUIFrame::OnPrint )
	EVT_MENU( wxID_MENU_FILE_EXIT, CGUIFrame::OnExitApp )
	
	//////////////////////
	// :: VIEW :: Menu items
	EVT_MENU( wxID_MENU_VIEW_OBJECTS, CGUIFrame::OnViewObjects )

	// :: VIEW :: constellations
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_SHOW, CGUIFrame::OnViewConstellations )
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_LINES, CGUIFrame::OnViewConstellations )
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_LABEL, CGUIFrame::OnViewConstellations )
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_ARTWORK, CGUIFrame::OnViewConstellations )
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_HARAY, CGUIFrame::OnViewConstellations )
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_IAU, CGUIFrame::OnViewConstellations )
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_CHINESE, CGUIFrame::OnViewConstellations )
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_EGYPTIAN, CGUIFrame::OnViewConstellations )
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_INUIT, CGUIFrame::OnViewConstellations )
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_KOREAN, CGUIFrame::OnViewConstellations )
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_LAKOTA, CGUIFrame::OnViewConstellations )
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_MAORI, CGUIFrame::OnViewConstellations )
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_NAVAJO, CGUIFrame::OnViewConstellations )
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_NORSE, CGUIFrame::OnViewConstellations )
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_POLYNESIAN, CGUIFrame::OnViewConstellations )
	EVT_MENU( wxID_MENU_VIEW_CONSTELLATIONS_TUPI, CGUIFrame::OnViewConstellations )

	// :: VIEW :: areas
	EVT_MENU( wxID_MENU_VIEW_IMAGE_AREAS, CGUIFrame::OnViewImageAreas )

	// :: VIEW :: thumbnails
	EVT_MENU( wxID_MENU_VIEW_GROUP_THUMBNAILS, CGUIFrame::OnViewGroupThumbnails )
	EVT_MENU( wxID_MENU_VIEW_GROUP_FULLSCREEN, CGUIFrame::OnViewFullScreen )

	// :: VIEW :: find objects
	EVT_MENU( wxID_MENU_VIEW_FIND_OBJECT, CGUIFrame::OnViewFindObject )
	// view logger
	EVT_MENU( wxID_MENU_VIEW_LOGGER, CGUIFrame::OnViewLogger )

	///////////////////////
	// group menu
	EVT_MENU( wxID_MENU_GROUP_NEW, CGUIFrame::OnNewGroup )
	EVT_MENU( wxID_MENU_GROUP_DELETE, CGUIFrame::OnDeleteGroup )
	EVT_MENU( wxID_MENU_GROUP_DETECT, CGUIFrame::OnGroupDetect )
	EVT_MENU( wxID_MENU_GROUP_HINT, CGUIFrame::OnGroupHint )
	EVT_MENU( wxID_MENU_GROUP_MATCH, CGUIFrame::OnGroupMatch )
	EVT_MENU( wxID_MENU_GROUP_REGISTER, CGUIFrame::OnGroupRegister )
	EVT_MENU( wxID_MENU_GROUP_ALIGN, CGUIFrame::OnGroupAlign )
	EVT_MENU( wxID_MENU_GROUP_DISCOVER, CGUIFrame::OnGroupDynamicAnalysis )
	EVT_MENU( wxID_MENU_GROUP_STACK, CGUIFrame::OnGroupStack )
	EVT_MENU( wxID_MENU_GROUP_STACK_REGISTAX, CGUIFrame::OnGroupStackWith )
	EVT_MENU( wxID_MENU_GROUP_STACK_DEEPSKYSTACKER, CGUIFrame::OnGroupStackWith )
	EVT_MENU( wxID_MENU_GROUP_STACK_IRIS, CGUIFrame::OnGroupStackWith )
	EVT_MENU( wxID_MENU_GROUP_STACK_MAXIMDL, CGUIFrame::OnGroupStackWith )
	EVT_MENU( wxID_MENU_GROUP_STACK_IMAGEPLUS, CGUIFrame::OnGroupStackWith )
	EVT_MENU( wxID_MENU_GROUP_STACK_REGISTAR, CGUIFrame::OnGroupStackWith )
	EVT_MENU( wxID_MENU_GROUP_STACK_K3CCDTOOLS, CGUIFrame::OnGroupStackWith )
	EVT_MENU( wxID_MENU_GROUP_PROPERTIES, CGUIFrame::OnGroupProperties )

	//////////////////////
	// :: IMAGE :: menu
	EVT_MENU( wxID_MENU_IMAGE_DETECT, CGUIFrame::OnDetect )
	EVT_MENU( wxID_MENU_IMAGE_CLEAR_DETECTED, CGUIFrame::OnDetectClear )
	EVT_MENU( wxID_MENU_IMAGE_CLEAR_MATCHED, CGUIFrame::OnMatchClear )
	EVT_MENU( wxID_MENU_IMAGE_CLEAR_AREAS, CGUIFrame::OnClearAreas )
	EVT_MENU( wxID_MENU_IMAGE_DELETE, CGUIFrame::OnImageDelete )
	EVT_MENU( wxID_MENU_IMAGE_MATCH, CGUIFrame::OnMatch )
	EVT_MENU( wxID_MENU_IMAGE_HINT, CGUIFrame::OnPosHint )
	EVT_MENU( wxID_VIEW_IMGNOTE, CGUIFrame::OnImageNotes )
	EVT_MENU( wxID_MENU_IMAGE_PROPERTIES, CGUIFrame::OnImageProperty )
	EVT_MENU( wxID_IMGVIEWPROPERTY, CGUIFrame::OnImageProperty )
	// 3d analysis
	EVT_MENU( wxID_MENU_IMAGE_3D_COLOR_MAP, CGUIFrame::OnView3dColorMap )
	EVT_MENU( wxID_MENU_IMAGE_3D_SPATIALITY, CGUIFrame::OnView3dSpatiality )
	EVT_MENU( wxID_MENU_IMAGE_3D_DISTORTION_GRID, CGUIFrame::OnView3dDistGrid )

	// :: IMAGE :: process menu
	// :: operators
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_OP_NEG, CGUIFrame::OnProcessImageOperator )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_OP_ADD, CGUIFrame::OnProcessImageOperator )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_OP_SUB, CGUIFrame::OnProcessImageOperator )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_OP_MUL, CGUIFrame::OnProcessImageOperator )
	// :: colors
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_COL_BCG, CGUIFrame::OnProcessImageColors )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_COL_MIDTONE, CGUIFrame::OnProcessImageColors )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_COL_BALANCE, CGUIFrame::OnProcessImageColors )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_COL_COLORIZE, CGUIFrame::OnProcessImageColors )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_COL_DESATURATE, CGUIFrame::OnProcessImageColors )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_COL_HUESATURATION, CGUIFrame::OnProcessImageColors )
	// :: filters :: blur
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_SMOOTH, CGUIFrame::OnProcessImageBlur )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_MEAN, CGUIFrame::OnProcessImageBlur )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_LOWPASS, CGUIFrame::OnProcessImageBlur )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_GAUSSIAN, CGUIFrame::OnProcessImageBlur )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_BILATERAL, CGUIFrame::OnProcessImageBlur )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_SELECTIVE, CGUIFrame::OnProcessImageBlur )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_FFTLOW, CGUIFrame::OnProcessImageBlur )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_COL_HUESATURATION, CGUIFrame::OnProcessImageBlur )
	// :: filters :: sharp
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_SHARPEN, CGUIFrame::OnProcessImageSharp )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_UNSHARP, CGUIFrame::OnProcessImageSharp )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_UNSHARPMASK, CGUIFrame::OnProcessImageSharp )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_LAPGAUS, CGUIFrame::OnProcessImageSharp )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_EMBOSS, CGUIFrame::OnProcessImageSharp )
	// :: filters :: edge
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_GRADIENT, CGUIFrame::OnProcessImageEdge )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_ROBERSCROSS, CGUIFrame::OnProcessImageEdge )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_LAPLACIAN, CGUIFrame::OnProcessImageEdge )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_MARRHILDRETH, CGUIFrame::OnProcessImageEdge )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_CANNY, CGUIFrame::OnProcessImageEdge )
	// :: filters :: gradient
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_GRADIENT_REMOVE, CGUIFrame::OnProcessImageGradient )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_GRADIENT_ROTATIONAL, CGUIFrame::OnProcessImageGradient )
	// :: filters :: deconvolution
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_DECONV_RLUCY, CGUIFrame::OnProcessImageDeconv )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_DECONV_WIENER, CGUIFrame::OnProcessImageDeconv )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_DECONV_DCTEM, CGUIFrame::OnProcessImageDeconv )
	// :: filters - single options
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_MORPHOLOGICAL, CGUIFrame::OnProcessImageFilter )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_FILTER_DIGITAL, CGUIFrame::OnProcessImageFilter )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_DENOISING, CGUIFrame::OnProcessImageFilter )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_DEBLOOMER, CGUIFrame::OnProcessImageFilter )

	// IMAGE :: Open With
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_PHOTOSHOP, CGUIFrame::OnImageOpenWith )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_GIMP, CGUIFrame::OnImageOpenWith )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_PIXINSIGHT, CGUIFrame::OnImageOpenWith )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_MAXIMDL, CGUIFrame::OnImageOpenWith )
	EVT_MENU( wxID_MENU_IMAGE_PROCESS_IMAGEPLUS, CGUIFrame::OnImageOpenWith )
	

	//////////////////////
	// :: INSTRUMENTS :: control
	EVT_MENU( wxID_MENU_INSTRUMENTS_CONTROL_PANEL, CGUIFrame::OnInstrumentsControl )

	//////////////////////
	// :: ONLINE :: upload
	EVT_MENU( wxID_MENU_IMAGE_UPLOAD, CGUIFrame::OnOnlineUpload )
	EVT_MENU( wxID_MENU_GROUP_UPLOAD, CGUIFrame::OnOnlineUpload )
	EVT_MENU( wxID_MENU_FILE_ONLINE_UPLOAD_CUSTOM, CGUIFrame::OnOnlineUpload )
	EVT_MENU( wxID_MENU_FILE_ONLINE_BROWSE, CGUIFrame::OnOnlineBrowse )

	//////////////////////
	// :: TOOLS :: weather
	EVT_MENU( wxID_MENU_TOOLS_SLIDE_SHOW, CGUIFrame::OnToolsSlideShow )
	EVT_MENU( wxID_MENU_TOOLS_WEATHER_SAT_IMG, CGUIFrame::OnToolsWeather )
	EVT_MENU( wxID_MENU_TOOLS_SUN_VIEW, CGUIFrame::OnToolsSunView )
	EVT_MENU( wxID_MENU_TOOLS_WEATHER_CURRENT_FORECAST, CGUIFrame::OnWeatherCurrentForecast )
	EVT_MENU( wxID_MENU_TOOLS_WEATHER_SEEING_PRED, CGUIFrame::OnWeatherSeeingPredict )
	EVT_MENU( wxID_MENU_TOOLS_LIGHT_POLLUTION, CGUIFrame::OnToolsLightPollution )
	EVT_MENU( wxID_MENU_TOOLS_OBSERVATORIES, CGUIFrame::OnToolsObservatories )
	EVT_MENU( wxID_MENU_TOOLS_NEWS_HEADLINES, CGUIFrame::OnNewsTool )
	// claculators
	EVT_MENU( wxID_MENU_TOOLS_CALC_UNITS, CGUIFrame::OnCalcUnits )
	EVT_MENU( wxID_MENU_TOOLS_CALC_OPTICAL, CGUIFrame::OnCalcOptical )

	//////////////////////
	// :: OPTIONS :: menu
	EVT_MENU( wxID_MENU_OPTIONS_DETECT, CGUIFrame::OnConfigDetection )
	EVT_MENU( wxID_MENU_OPTIONS_MATCH, CGUIFrame::OnConfigMatch )
	EVT_MENU( wxID_MENU_OPTIONS_CATALOG, CGUIFrame::OnConfigCatalog )
	EVT_MENU( wxID_MENU_OPTIONS_OBSERVER, CGUIFrame::OnConfigObserver )
	EVT_MENU( wxID_MENU_OPTIONS_ONLINE, CGUIFrame::OnConfigOnline )
	EVT_MENU( wxID_MENU_OPTIONS_INSTRUMENTS, CGUIFrame::OnConfigInstruments )
	EVT_MENU( wxID_MENU_OPTIONS_GLOBALS, CGUIFrame::OnConfig )

	//////////////////////
	// :: HELP :: menu
	EVT_MENU( wxID_MENU_HELP_TOPICS, CGUIFrame::OnHelp )
	EVT_MENU( wxID_MENU_HELP_FEEDBACK, CGUIFrame::OnFeedback )
	EVT_MENU( wxID_MENU_HELP_ABOUT, CGUIFrame::OnAbout )

	/////////////////////////
	// :: TOOLBAR :: events
	EVT_TOOL( wxID_BUTTON_NEW, CGUIFrame::OnNewGroup )
	EVT_TOOL( wxID_BUTTON_OPEN, CGUIFrame::OnAddImage )
	EVT_TOOL( wxID_BUTTON_SAVE, CGUIFrame::OnSaveImage )

	EVT_TOOL( wxID_BUTTON_OBJ_DETECT_AND_MATCH, CGUIFrame::OnDetectAndMatch )
//	EVT_TOOL( wxID_BUTTON_MATCH, CGUIFrame::OnMatch )
//	EVT_TOOL( wxID_BUTTON_OBJDETECT, CGUIFrame::OnDetect )
//	EVT_TOOL( wxID_BUTTON_POSHINT, CGUIFrame::OnPosHint )
//	EVT_TOOL( wxID_BUTTON_OBJDATA_FETCH, CGUIFrame::OnObjectDataFetch )
	EVT_TOOL( wxID_BUTTON_OBJFIND, CGUIFrame::OnViewFindObject )
	EVT_TOOL( wxID_BUTTON_MEASURE_DISTANCE, CGUIFrame::OnMeasureDistanceTool )
	EVT_TOOL( wxID_BUTTON_SKY_GRID, CGUIFrame::OnSkyGrid ) 
	EVT_TOOL( wxID_BUTTON_DISTORSION_GRID, CGUIFrame::OnDistortionGrid )
	EVT_TOOL( wxID_BUTTON_IMGZOOM, CGUIFrame::OnImageZoomTool )
	EVT_TOOL( wxID_BUTTON_IMGINV, CGUIFrame::OnProcessImageOperator )
	EVT_TOOL( wxID_BUTTON_AREA_ADD, CGUIFrame::OnImageAddArea )

	EVT_TOOL( wxID_TOOL_ONLINE_BROWSE, CGUIFrame::OnOnlineBrowse )
	EVT_MENU( wxID_TOOL_ONLINE_UPLOAD_GROUP, CGUIFrame::OnOnlineUpload )
	EVT_MENU( wxID_TOOL_ONLINE_UPLOAD, CGUIFrame::OnOnlineUpload )

	EVT_TOOL( wxID_TOOL_SHOW_CTRL_THUMBS, CGUIFrame::OnViewGroupThumbnails ) // old - OnThumbs )
	EVT_TOOL( wxID_INSTR_SHOW_CTRL_PANEL, CGUIFrame::OnInstrumentsControl )

	// on minimize
	EVT_ICONIZE( CGUIFrame::OnMinimize )
	// toolbar select options
//	EVT_CHOICE( wxID_OPTION_DETECT, CGUIFrame::OnDetectOption )

	// browsing options
	EVT_CHOICE( wxID_GROUP_SELECT, CGUIFrame::OnGroupSelect )
	EVT_CHOICE( wxID_IMAGE_SELECT, CGUIFrame::OnImageSelect )
	// browse from thumbnails list
	EVT_LIST_ITEM_SELECTED( wxID_GROUP_THUMBNAILS_LIST, CGUIFrame::OnImageThumbnailSelect )

	// on size
	EVT_SIZE( CGUIFrame::OnSize )
	// on close
	EVT_CLOSE( CGUIFrame::OnClose )

	////////////////////////
	// :: CUSTOM :: events
	EVT_COMMAND( wxID_REMOTE_LOGGER, wxEVT_CUSTOM_MESSAGE_EVENT, CGUIFrame::OnRemoteLogger )
	EVT_COMMAND( wxID_NOTIFICATION_DIALOG, wxEVT_CUSTOM_MESSAGE_EVENT, CGUIFrame::OnNotificationMessage )
	// image loaded in thread
	EVT_COMMAND( wxID_THREAD_ASTROIMAGE_LOADED, wxEVT_CUSTOM_MESSAGE_EVENT, CGUIFrame::OnAstroImageLoaded )

	////////////////////////////////////////////////////////////////
	// INTRUMENTS :: TOOLBAR :: events
	EVT_TOOL( wxID_INSTR_SHOW_CTRL_CAMERA, CGUIFrame::OnActivateCameraView )
	EVT_CHOICE( wxID_INSTR_CAMERA_SELECT, CGUIFrame::OnSelectCamera )
	EVT_TOOL( wxID_INSTR_SHOW_CTRL_SETTINGS, CGUIFrame::OnInstrSettings )
	// camera handlers
	EVT_TOGGLE( wxID_INSTR_CAMERA_CONNECT_BUTTON, CGUIFrame::OnCameraConnect )
	EVT_BUTTON( wxID_INSTR_CTRL_CAMERA_START, CGUIFrame::OnCameraAction )
	EVT_BUTTON( wxID_INSTR_CTRL_CAMERA_SETUP, CGUIFrame::OnCameraSetup )
	EVT_BUTTON( wxID_INSTR_CTRL_CAMERA_FORMAT, CGUIFrame::OnCameraFormat )
	EVT_CHECKBOX( wxID_INSTR_CTRL_CAMERA_USE_EXP, CGUIFrame::OnCameraOptions )
	EVT_CHECKBOX( wxID_INSTR_CTRL_CAMERA_USE_FRAMES, CGUIFrame::OnCameraOptions )
	EVT_CHECKBOX( wxID_INSTR_CTRL_CAMERA_USE_DELAY, CGUIFrame::OnCameraOptions )
	// telescope connect
	EVT_TOGGLE( wxID_INSTR_TELESCOPE_CONNECT_BUTTON, CGUIFrame::OnTelescopeConnect )
	// telescope setup
	EVT_BUTTON( wxID_INSTR_TELESCOPE_SETUP_BUTTON, CGUIFrame::OnTelescopeSetup )
	EVT_BUTTON( wxID_INSTR_TELESCOPE_OBJECT, CGUIFrame::OnTelescopeObjSet )
	// telescope movement handlers
	EVT_TOGGLE( wxID_INSTR_TELESCOPE_GOTO, CGUIFrame::OnTelescopeGoto )
	EVT_BUTTON( wxID_INSTR_MOVE_WEST, CGUIFrame::OnTelescopeMove )
	EVT_BUTTON( wxID_INSTR_MOVE_EAST, CGUIFrame::OnTelescopeMove )
	EVT_BUTTON( wxID_INSTR_MOVE_NORTH, CGUIFrame::OnTelescopeMove )
	EVT_BUTTON( wxID_INSTR_MOVE_SOUTH, CGUIFrame::OnTelescopeMove )
	EVT_BUTTON( wxID_INSTR_MOVE_STOP, CGUIFrame::OnTelescopeMove )
	// focuser handlers
	EVT_TOGGLE( wxID_INSTR_FOCUS_CONNECT, CGUIFrame::OnFocuserConnect )
	EVT_BUTTON( wxID_INSTR_FOCUS_IN, CGUIFrame::OnFocuserMove )
	EVT_BUTTON( wxID_INSTR_FOCUS_ZERO, CGUIFrame::OnFocuserMove )
	EVT_BUTTON( wxID_INSTR_FOCUS_OUT, CGUIFrame::OnFocuserMove )
	EVT_CHECKBOX( wxID_INSTR_FOCUS_USE_AUTO, CGUIFrame::OnFocusOptions )
	EVT_CHECKBOX( wxID_INSTR_FOCUS_USE_UNITS, CGUIFrame::OnFocusOptions )
	EVT_CHECKBOX( wxID_INSTR_FOCUS_USE_SLOPE, CGUIFrame::OnFocusOptions )
	// custom events :: telescope
	EVT_COMMAND( wxID_TELESCOPE_INFO_CONNECTED, wxEVT_CUSTOM_MESSAGE_EVENT, CGUIFrame::OnUpdateInfo )
	EVT_COMMAND( wxID_TELESCOPE_INFO_DISCONNECTED, wxEVT_CUSTOM_MESSAGE_EVENT, CGUIFrame::OnUpdateInfo )
	EVT_COMMAND( wxID_TELESCOPE_INFO_UPDATE, wxEVT_CUSTOM_MESSAGE_EVENT, CGUIFrame::OnUpdateInfo )
	// custom events :: camera
	EVT_COMMAND( wxID_DCAMERA_INFO_CONNECTED, wxEVT_CUSTOM_MESSAGE_EVENT, CGUIFrame::OnCameraUpdateInfo )
	EVT_COMMAND( wxID_DCAMERA_INFO_DISCONNECTED, wxEVT_CUSTOM_MESSAGE_EVENT, CGUIFrame::OnCameraUpdateInfo )
	EVT_COMMAND( wxID_DCAMERA_GOT_NEW_IMAGE, wxEVT_CUSTOM_MESSAGE_EVENT, CGUIFrame::OnCameraUpdateInfo )
	EVT_COMMAND( wxID_INSTR_CONDITIONS_UPDATE, wxEVT_CUSTOM_MESSAGE_EVENT, CGUIFrame::OnCondUpdate )

	// timer handler
	EVT_TIMER( wxID_SLIDESHOW_TIMER, CGUIFrame::OnSlideShowTimer )

END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CGUIFrame
// Purose:	build my MainFrame object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CGUIFrame::CGUIFrame( wxFrame *frame, const wxPoint& pos, const wxSize& size ):
							wxFrame( frame, -1, UNIMAP_FRAME_TITLE, pos, size, wxSYSTEM_MENU | wxMAXIMIZE_BOX |
									wxMINIMIZE_BOX|wxCLOSE_BOX| wxCAPTION |wxRESIZE_BORDER )
{
	// get references to worker and online objects
	m_pUnimap = &(wxGetApp());

	m_pMainConfig = NULL;
	m_pUnimapWorker = NULL;
	m_pLogger = NULL;
	m_pAstroImage = NULL;
	m_pSextractor = NULL;
	m_pStarMatch = NULL;
	m_nLastCurRa = 0;
	m_nLastCurDec = 0;
	m_pDistanceTool = NULL;
	m_bHasPageSetup = 0;
	m_bInstrActive = 0;
	m_nCurrentGroup = 0;
	m_nCurrentImage = 0;
	m_bInstrCameraView = 0;
	m_bIsFullScreen = 0;
	m_pSlideShowTimer = NULL;
	m_bSlideLoading = 0;
	m_bIsClosing = 0;
	m_nSlideShowStatus = 0;
	m_bIconized = 0;

	m_bGroupThumbnailsInitialized = 0;
	// init print vars
	m_pPrintData = (wxPrintData*) NULL;
	m_pPageSetupData = (wxPageSetupDialogData*) NULL;

	// set first default size/flags
	m_nWidth = size.GetWidth();
	m_nHeight = size.GetHeight();

	m_bInit = 0;

#ifdef WIN32
	SetIcon( wxICON( ICON_UNIMAP ) );
#else
	wxIcon icon( unimap_xpm );
	SetIcon( icon );
#endif

	SetMinSize( wxSize( 500, 200 ) );

	// set bg color for my frame
	//SetBackgroundColour( *wxLIGHT_GREY );
	SetBackgroundColour( wxNullColour );

	// create taskbar icon
	m_pFrameTaskBarIcon = new CFrameTaskBarIcon();
	m_pFrameTaskBarIcon->m_pFrame = this;

	wxString vectStrMatchOption[2];
	wxString vectStrDetectOption[2];
	wxString vectStrImageSelect[1];
	wxString vectStrGroupSelect[1];


	vectStrGroupSelect[0] = wxT("Default");
	vectStrImageSelect[0] = wxT("Select Image");
	// detection profiles
	vectStrDetectOption[0] = wxT("Default");
	vectStrDetectOption[1] = wxT("nebula");
	// match toolbar options
	vectStrMatchOption[0] = wxT("All Sky");
	vectStrMatchOption[1] = wxT("Use Hint");

	// create status bar
	int nWidths[10] =  { -10, 40, 70, 190, 60, 42, 55, 47, 23 };
	int nStyle[10] =  { wxSB_FLAT, wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL, 
					wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL, wxSB_FLAT };
	CreateStatusBar( 9, wxST_SIZEGRIP, wxID_STATUS_BAR );
	GetStatusBar()->SetStatusWidths( 9, nWidths );
	GetStatusBar()->SetStatusStyles( 9, nStyle );

	//////////////////
	// :: FILE :: menu items
	wxMenu *pFileMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pFileMenu->Append( wxID_MENU_FILE_OPEN_IMAGE, wxT("Open Image") );
	pFileMenu->Append( wxID_MENU_FILE_ADD_IMAGES, wxT("Add Images...") );
	pFileMenu->Append( wxID_MENU_FILE_SAVE_IMAGE, wxT("Save Image") );
	pFileMenu->Append( wxID_MENU_FILE_SAVE_ALL, wxT("Save All...") );
	pFileMenu->Append( wxID_MENU_FILE_EXPORT, wxT("Export...") );
	pFileMenu->AppendSeparator( );

	wxMenu* pFileOnlineSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pFileOnlineSubMenu->Append( wxID_MENU_FILE_ONLINE_BROWSE, wxT("Browse/Download") );
	pFileOnlineSubMenu->Append( wxID_MENU_FILE_ONLINE_UPLOAD_CUSTOM, wxT("Upload Custom") );
	pFileMenu->Append( wxID_MENU_FILE_ONLINE, wxT("Online"), pFileOnlineSubMenu );
	pFileMenu->AppendSeparator( );

	pFileMenu->Append( wxID_MENU_FILE_PRINT_PAGE_SETUP, wxT("Page Setup...") );
	pFileMenu->Append( wxID_MENU_FILE_PRINT_PREVIEW, wxT("Print Preview...") );
	pFileMenu->Append( wxID_MENU_FILE_PRINT_DIALOG, wxT("Print...") );
//	pFileMenu->AppendSeparator( );
//	pFileMenu->Append( wxID_MENU_FILE_OPTIONS, "Options" );
	pFileMenu->AppendSeparator( );
	pFileMenu->Append( wxID_MENU_FILE_EXIT, wxT("Exit") );

	////////////////////////////
	// :: VIEW menu bar
	wxMenu *pViewMenu = new wxMenu;
	pViewMenu->Append( wxID_MENU_VIEW_OBJECTS, wxT("Objects") );
//	pViewMenu->AppendCheckItem( wxID_MENU_VIEW_OBJECTS, wxT("View Objects") , wxT("View Objects") );
//	pViewMenu->Check( wxID_MENU_VIEW_OBJECTS,  1 );

	////////////////////////////
	// :: VIEW :: constellations
	wxMenu* pConstellationsSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	// default none case
	pConstellationsSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_SHOW, wxT("Show"));
	pConstellationsSubMenu->AppendSeparator( );
	pConstellationsSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_LINES, wxT("Lines"), wxT("Show Lines") );
	pConstellationsSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_LABEL, wxT("Label"), wxT("Show Label") );
	pConstellationsSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_ARTWORK, wxT("Artwork"), wxT("Show ArtWork") );
	pConstellationsSubMenu->AppendSeparator( );
	// western case constl sub menu of constellations
	wxMenu* pConstlWesternSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pConstlWesternSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_HARAY, wxT("H.A. Ray"), wxT("H.A. Ray") );
	pConstlWesternSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_IAU, wxT("I.A.U."), wxT("I.A.U.") );
	pConstellationsSubMenu->Append( wxID_MENU_VIEW_CONSTELLATIONS_WESTERN, wxT("Western"), pConstlWesternSubMenu );
	// all other main cases
	pConstellationsSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_CHINESE, wxT("Chinese"));
	pConstellationsSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_EGYPTIAN, wxT("Egyptian"));
	pConstellationsSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_INUIT, wxT("Inuit"));
	pConstellationsSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_KOREAN, wxT("Korean"));
	pConstellationsSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_LAKOTA, wxT("Lakota"));
	pConstellationsSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_MAORI, wxT("Maori"));
	pConstellationsSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_NAVAJO, wxT("Navajo"));
	pConstellationsSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_NORSE, wxT("Norse"));
	pConstellationsSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_POLYNESIAN, wxT("Polynesian"));
	pConstellationsSubMenu->AppendCheckItem( wxID_MENU_VIEW_CONSTELLATIONS_TUPI, wxT("Tupi"));
	// sub constl to main view
	pViewMenu->Append( wxID_MENU_VIEW_CONSTELLATIONS, wxT("Constellations"), pConstellationsSubMenu );
	
	// view areas
	pViewMenu->AppendCheckItem( wxID_MENU_VIEW_IMAGE_AREAS, wxT("Areas"), wxT("View Image Areas") );
	pViewMenu->Check( wxID_MENU_VIEW_IMAGE_AREAS,  0 );

	// :: VIEW :: thumbnails option
	pViewMenu->AppendSeparator( );
	pViewMenu->AppendCheckItem( wxID_MENU_VIEW_GROUP_THUMBNAILS, wxT("Thumbnails"), wxT("View Thumbnails Window") );
	pViewMenu->Check( wxID_MENU_VIEW_GROUP_THUMBNAILS,  0 );

	// :: VIEW :: find option
	pViewMenu->AppendSeparator( );
	pViewMenu->Append( wxID_MENU_VIEW_FIND_OBJECT, wxT("Find") );
	// :: VIEW :: processes logs window
	pViewMenu->AppendSeparator( );
	pViewMenu->AppendCheckItem( wxID_MENU_VIEW_LOGGER, wxT("Log Window"), wxT("View Log Window") );
	pViewMenu->Append( wxID_MENU_VIEW_GROUP_FULLSCREEN, wxT("Full Screen") );
	pViewMenu->Check( wxID_MENU_VIEW_LOGGER,  1 );

	//////////////////////
	// make Group menu bar
	wxMenu *pGroupMenu = new wxMenu;
	pGroupMenu->Append( wxID_MENU_GROUP_NEW, wxT("New") );
	pGroupMenu->Append( wxID_MENU_GROUP_DELETE, wxT("Delete") );
	pGroupMenu->AppendSeparator( );
	pGroupMenu->Append( wxID_MENU_GROUP_DETECT, wxT("Detect") );
	pGroupMenu->Append( wxID_MENU_GROUP_HINT, wxT("Hint") );
	pGroupMenu->Append( wxID_MENU_GROUP_MATCH, wxT("Match") );
	pGroupMenu->AppendSeparator( );
	pGroupMenu->Append( wxID_MENU_GROUP_REGISTER, wxT("Register") );
	pGroupMenu->Append( wxID_MENU_GROUP_ALIGN, wxT("Align") );
	pGroupMenu->Append( wxID_MENU_GROUP_DISCOVER, wxT("Discover") );
	pGroupMenu->AppendSeparator( );
	pGroupMenu->Append( wxID_MENU_GROUP_STACK, wxT("Stack") );
	// stack with sub menu
	wxMenu* pStackWithGroupSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pStackWithGroupSubMenu->Append( wxID_MENU_GROUP_STACK_REGISTAX, wxT("Registax") );
	pStackWithGroupSubMenu->Append( wxID_MENU_GROUP_STACK_DEEPSKYSTACKER, wxT("DeepSkyStacker") );
	pStackWithGroupSubMenu->Append( wxID_MENU_GROUP_STACK_IRIS, wxT("IRIS") );
	pStackWithGroupSubMenu->Append( wxID_MENU_GROUP_STACK_MAXIMDL, wxT("MaxImDL") );
	pStackWithGroupSubMenu->Append( wxID_MENU_GROUP_STACK_IMAGEPLUS, wxT("ImagePlus") );
	pStackWithGroupSubMenu->Append( wxID_MENU_GROUP_STACK_REGISTAR, wxT("Registar") );
	pStackWithGroupSubMenu->Append( wxID_MENU_GROUP_STACK_K3CCDTOOLS, wxT("K3CCDTools") );
	pGroupMenu->Append( wxID_MENU_GROUP_STACK_WITH, wxT("Stack with"), pStackWithGroupSubMenu );
	pGroupMenu->AppendSeparator( );
	pGroupMenu->Append( wxID_MENU_GROUP_UPLOAD, wxT("Upload") );
	pGroupMenu->AppendSeparator( );
	pGroupMenu->Append( wxID_MENU_GROUP_PROPERTIES, wxT("Properties") );

	//////////////////////
	// make Image menu bar --- 
	wxMenu *pImageMenu = new wxMenu;
	pImageMenu->Append( wxID_MENU_IMAGE_DETECT, wxT("Detect") );
	pImageMenu->Append( wxID_MENU_IMAGE_HINT, wxT("Hint") );
	pImageMenu->Append( wxID_MENU_IMAGE_MATCH, wxT("Match") );
	pImageMenu->AppendSeparator( );
	// :: clear sub menu 
	wxMenu* pClearImageSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pClearImageSubMenu->Append( wxID_MENU_IMAGE_CLEAR_DETECTED, wxT("Detected") );
	pClearImageSubMenu->Append( wxID_MENU_IMAGE_CLEAR_MATCHED, wxT("Matched") );
	pClearImageSubMenu->Append( wxID_MENU_IMAGE_CLEAR_AREAS, wxT("Areas") );
	pImageMenu->Append( wxID_MENU_IMAGE_CLEAR, wxT("Clear"), pClearImageSubMenu );
	pImageMenu->Append( wxID_MENU_IMAGE_DELETE, wxT("Remove") );
	pImageMenu->AppendSeparator( );
	// :: IMAGE :: 3d view submenu
	wxMenu* p3dImageSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	p3dImageSubMenu->Append( wxID_MENU_IMAGE_3D_COLOR_MAP, wxT("Color Map") );
	p3dImageSubMenu->Append( wxID_MENU_IMAGE_3D_SPATIALITY, wxT("Spatiality") );
	p3dImageSubMenu->Append( wxID_MENU_IMAGE_3D_DISTORTION_GRID, wxT("Distortion") );
	pImageMenu->Append( wxID_MENU_IMAGE_3D_ANALYSIS, wxT("3D Analysis"), p3dImageSubMenu );
	pImageMenu->AppendSeparator( );
	
	/////////////////////////////////
	// :: process with sub menu
	wxMenu* pProcImageSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	///////////////////
	// :: process operators
	wxMenu* pProcOpImageSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pProcOpImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_OP_NEG, wxT("Negative") );
	pProcOpImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_OP_ADD, wxT("Addition") );
	pProcOpImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_OP_SUB, wxT("Substraction") );
	pProcOpImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_OP_MUL, wxT("Multiplication") );
	//////////////////
	// :: process colors
	wxMenu* pProcColImageSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pProcColImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_COL_BCG, wxT("Brightness/Contrast/Gamma") );
	pProcColImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_COL_MIDTONE, wxT("Midtone") );
	pProcColImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_COL_BALANCE, wxT("Balance") );
	pProcColImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_COL_COLORIZE, wxT("Colorize") );
	pProcColImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_COL_DESATURATE, wxT("Desaturate") );
	pProcColImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_COL_HUESATURATION, wxT("Hue/Saturation") );
	////////////////////
	// :: process filters
	wxMenu* pProcFilterImageSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	//////////
	// :: process filters :: blur
	wxMenu* pProcFilterBlurImageSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pProcFilterBlurImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_SMOOTH, wxT("Smooth") );
	pProcFilterBlurImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_MEAN, wxT("Mean") );
	pProcFilterBlurImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_LOWPASS, wxT("Lowpass") );
	pProcFilterBlurImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_GAUSSIAN, wxT("Gaussian") );
	pProcFilterBlurImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_BILATERAL, wxT("Bilateral") );
	pProcFilterBlurImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_SELECTIVE, wxT("Selective") );
	pProcFilterBlurImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_FFTLOW, wxT("FFT Low") );
	//////////
	// :: process filters :: sharp
	wxMenu* pProcFilterSharpImageSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pProcFilterSharpImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_SHARPEN, wxT("Sharpen") );
	pProcFilterSharpImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_UNSHARP, wxT("Unsharp") );
	pProcFilterSharpImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_UNSHARPMASK, wxT("Unsharp Mask") );
	pProcFilterSharpImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_LAPGAUS, wxT("Laplacian-Gaussian") );
	pProcFilterSharpImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_EMBOSS, wxT("Emboss") );
	//////////
	// :: process filters :: edge
	wxMenu* pProcFilterEdgeImageSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pProcFilterEdgeImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_GRADIENT, wxT("Gradient") );
	pProcFilterEdgeImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_ROBERSCROSS, wxT("Robers Cross") );
	pProcFilterEdgeImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_LAPLACIAN, wxT("Laplacian") );
	pProcFilterEdgeImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_MARRHILDRETH, wxT("Marr Hildreth") );
	pProcFilterEdgeImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_CANNY, wxT("Canny") );
	//////////
	// :: process filters :: gradient
	wxMenu* pProcFilterGradientImageSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pProcFilterGradientImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_GRADIENT_REMOVE, wxT("Remove") );
	pProcFilterGradientImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_GRADIENT_ROTATIONAL, wxT("Rotational") );
	//////////
	// :: process filters :: deconvolution
	wxMenu* pProcFilterDeconvImageSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pProcFilterDeconvImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_DECONV_RLUCY, wxT("Richardson-Lucy") );
	pProcFilterDeconvImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_DECONV_WIENER, wxT("Wiener") );
	pProcFilterDeconvImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_DECONV_DCTEM, wxT("DCT-Complexity") );

	// add filters-blur to filters submenu
	pProcFilterImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_BLUR, wxT("Blur"), pProcFilterBlurImageSubMenu );
	pProcFilterImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_SHARP, wxT("Sharp"), pProcFilterSharpImageSubMenu );
	pProcFilterImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_EDGE, wxT("Edge"), pProcFilterEdgeImageSubMenu );
	pProcFilterImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_GRADIENT, wxT("Gradient"), pProcFilterGradientImageSubMenu );
	pProcFilterImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_DECONVOLUTION, wxT("Deconvolution"), pProcFilterDeconvImageSubMenu );
	// add the rest of the options
	pProcFilterImageSubMenu->AppendSeparator( );
	pProcFilterImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_MORPHOLOGICAL, wxT("Morphological") );
	pProcFilterImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER_DIGITAL, wxT("Digital") );

	// add to proc submenu
	pProcImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_OP, wxT("Operators"), pProcOpImageSubMenu );
	pProcImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_COL, wxT("Colors"), pProcColImageSubMenu );
	pProcImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_FILTER, wxT("Filters"), pProcFilterImageSubMenu );
	// single opts
	pProcImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_DENOISING, wxT("Denoising") );
	pProcImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_DEBLOOMER, wxT("Debloomer") );

	// append process to image menu
	pImageMenu->Append( wxID_MENU_IMAGE_PROCESS, wxT("Process..."), pProcImageSubMenu );

	//////////////////////////////
	// :: process with sub menu
	wxMenu* pProcWithImageSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pProcWithImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_PHOTOSHOP, wxT("Photoshop") );
	pProcWithImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_GIMP, wxT("Gimp") );
	pProcWithImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_PIXINSIGHT, wxT("PixInsight") );
	pProcWithImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_MAXIMDL, wxT("MaxImDL") );
	pProcWithImageSubMenu->Append( wxID_MENU_IMAGE_PROCESS_IMAGEPLUS, wxT("ImagePlus") );
	pImageMenu->Append( wxID_MENU_IMAGE_PROCESS_WITH, wxT("Process with"), pProcWithImageSubMenu );
	// :: image notes
//	pImageMenu->AppendSeparator( );
//	pImageMenu->Append( wxID_MENU_IMAGE_NOTES, "Notes" );
	pImageMenu->AppendSeparator( );
	pImageMenu->Append( wxID_MENU_IMAGE_UPLOAD, wxT("Upload") );
	// :: image properties
	pImageMenu->AppendSeparator( );
	pImageMenu->Append( wxID_MENU_IMAGE_PROPERTIES, wxT("Properties") );

	//////////////////////
	// :: ONLINE menu bar
/*	wxMenu *pOnlineMenu = new wxMenu;
	wxMenu* pOnlineUploadSubMenu = new wxMenu( "", wxMENU_TEAROFF );
	pOnlineUploadSubMenu->Append( wxID_MENU_ONLINE_UPLOAD_IMAGE, "Image" );
	pOnlineUploadSubMenu->Append( wxID_MENU_ONLINE_UPLOAD_GROUP, "Group" );
	pOnlineUploadSubMenu->AppendSeparator( );
	pOnlineUploadSubMenu->Append( wxID_MENU_ONLINE_UPLOAD_CUSTOM, "Custom" );
	pOnlineMenu->Append( wxID_MENU_ONLINE_UPLOAD, "Upload", pOnlineUploadSubMenu );
	pOnlineMenu->AppendSeparator( );
	pOnlineMenu->Append( wxID_MENU_ONLINE_BROWSE_IMAGES, "Browse" );
*/

	//////////////////////
	// :: TOOLS menu bar
	wxMenu *pToolsMenu = new wxMenu;

	pToolsMenu->Append( wxID_MENU_INSTRUMENTS_CONTROL_PANEL, wxT("Instruments") );
	pToolsMenu->AppendSeparator( );

	pToolsMenu->AppendCheckItem( wxID_MENU_TOOLS_SLIDE_SHOW, wxT("SlideShow"), wxT("Slide Show") );
	pToolsMenu->Check( wxID_MENU_TOOLS_SLIDE_SHOW,  0 );

	pToolsMenu->AppendSeparator( );
	wxMenu* pToolsEnvSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pToolsEnvSubMenu->Append( wxID_MENU_TOOLS_WEATHER_SAT_IMG, wxT("Weather Satellites Images") );
	pToolsEnvSubMenu->Append( wxID_MENU_TOOLS_WEATHER_CURRENT_FORECAST, wxT("Weather Current/Forcast") );
	pToolsEnvSubMenu->Append( wxID_MENU_TOOLS_WEATHER_SEEING_PRED, wxT("Weather Seeing Prediction") );
	pToolsEnvSubMenu->Append( wxID_MENU_TOOLS_LIGHT_POLLUTION, wxT("Light Pollution") );
	pToolsMenu->Append( wxID_MENU_TOOLS_ENVIRONMENT, wxT("Environment"), pToolsEnvSubMenu );

	pToolsMenu->Append( wxID_MENU_TOOLS_SUN_VIEW, wxT("Sun View") );

	pToolsMenu->AppendSeparator( );
	pToolsMenu->Append( wxID_MENU_TOOLS_OBSERVATORIES, wxT("Observatories") );
	pToolsMenu->Append( wxID_MENU_TOOLS_NEWS_HEADLINES, wxT("News Headlines") );

	wxMenu* pToolsCalcSubMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pToolsCalcSubMenu->Append( wxID_MENU_TOOLS_CALC_UNITS, wxT("Units Conversion") );
	pToolsCalcSubMenu->Append( wxID_MENU_TOOLS_CALC_OPTICAL, wxT("Optical Systems") );
	pToolsMenu->Append( wxID_MENU_TOOLS_ASTRO_CALCULATOR, wxT("Calculators"), pToolsCalcSubMenu );

	/////////////////////////////////
	// :: OPTIONS menu bar
	wxMenu *pOptionsMenu = new wxMenu;
	pOptionsMenu->Append( wxID_MENU_OPTIONS_DETECT, wxT("Detection") );
	pOptionsMenu->Append( wxID_MENU_OPTIONS_MATCH, wxT("Matching") );
	pOptionsMenu->AppendSeparator( );
	pOptionsMenu->Append( wxID_MENU_OPTIONS_CATALOG, wxT("Catalogs") );
	pOptionsMenu->AppendSeparator( );
	pOptionsMenu->Append( wxID_MENU_OPTIONS_OBSERVER, wxT("Observer") );
	pOptionsMenu->Append( wxID_MENU_OPTIONS_ONLINE, wxT("Online") );
	pOptionsMenu->Append( wxID_MENU_OPTIONS_INSTRUMENTS, wxT("Instruments") );
//	pOptionsMenu->Append( wxID_MENU_OPTIONS_PRIVACY, "Privacy" );
	pOptionsMenu->AppendSeparator( );
	pOptionsMenu->Append( wxID_MENU_OPTIONS_GLOBALS, wxT("Preferences") );

	
	//////////////////////
	// :: HELP :: menu items
	wxMenu *pHelpMenu = new wxMenu( wxT(""), wxMENU_TEAROFF );
	pHelpMenu->Append( wxID_MENU_HELP_TOPICS, _("Topics") );
	pHelpMenu->AppendSeparator( );
	pHelpMenu->Append( wxID_MENU_HELP_FEEDBACK, _("Feedback") );
	pHelpMenu->AppendSeparator( );
	pHelpMenu->Append( wxID_MENU_HELP_ABOUT, _("About") );

	// make menu toolbar
	menuBar = new wxMenuBar;
	menuBar->Append( pFileMenu, _("File") );
	menuBar->Append( pViewMenu, _("View") );
	menuBar->Append( pGroupMenu, _("Group") );
	menuBar->Append( pImageMenu, _("Image") );
	menuBar->Append( pToolsMenu, _("Tools") );
	menuBar->Append( pOptionsMenu, _("Options") );
	menuBar->Append( pHelpMenu, _("Help") );
	this->SetMenuBar(menuBar);

	// get client size
	int width, height;
	this->GetClientSize(&width, &height);

	////////////////////////
	// create main toolbar
	wxToolBar* pToolBar = CreateToolBar( wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER );

	wxBitmap bmpNewFolder = wxBitmap( new_dir_xpm );
	wxBitmap bmpOpen = wxBitmap( open_xpm );
	wxBitmap bmpSave = wxBitmap( save_xpm );
//	wxBitmap bmpTarget = wxBitmap( target_xpm );
	wxBitmap bmpMatch = wxBitmap( toolchec_xpm );
//	wxBitmap bmpObjDetect = wxBitmap( detect_stars_xpm );
//	wxBitmap bmpClue = wxBitmap( center_xpm );
	wxImage imgObjFind( wxT("resources/search.gif") );
	wxBitmap bmpImgZoom = wxBitmap( find_xpm );
	wxImage imgDistanceTool( wxT("resources/icon_distance.gif") );
	wxImage imgSkyGrid( wxT("resources/sky_grd.gif") ); 
	wxImage imgDistGrid( wxT("resources/dist_grid.gif") );
	wxImage imgImgInv( wxT("resources/invert_colors.png") );
	wxImage imgAreaAdd( wxT("resources/area_add.gif") );
	// online
	wxImage imgOnlineBrowse( wxT("resources/online-database.gif") );
	wxImage imgOnlineUploadGroup( wxT("resources/upload_folder.gif") );
	wxImage imgOnlineUpload( wxT("resources/upload_file.gif") );
	// section frame tools
	wxImage imgViewThumbs( wxT("resources/thumbs.gif") );
	wxImage imgViewInstruments( wxT("resources/telescope2.png") );

	// get toolbar size
    int wt = pToolBar->GetToolBitmapSize().GetWidth(),
        ht = pToolBar->GetToolBitmapSize().GetHeight();
	// resize icons to toolbar size
	bmpNewFolder = wxBitmap(bmpNewFolder.ConvertToImage().Scale(wt, ht));
	bmpOpen = wxBitmap(bmpOpen.ConvertToImage().Scale(wt, ht));
	bmpSave = wxBitmap(bmpSave.ConvertToImage().Scale(wt, ht));
//	bmpTarget = wxBitmap(bmpTarget.ConvertToImage().Scale(wt, ht));
	bmpMatch = wxBitmap(bmpMatch.ConvertToImage().Scale(wt, ht));
//	bmpObjDetect = wxBitmap(bmpObjDetect.ConvertToImage().Scale(wt, ht));
//	bmpClue = wxBitmap(bmpClue.ConvertToImage().Scale(wt, ht));
	wxBitmap bmpObjFind = wxBitmap( imgObjFind.Scale(wt, ht) );
	bmpImgZoom = wxBitmap(bmpImgZoom.ConvertToImage().Scale(wt, ht));
	wxBitmap bmpDistanceTool = wxBitmap( imgDistanceTool.Scale(wt, ht) );
	wxBitmap bmpSkyGrid = wxBitmap( imgSkyGrid.Scale(wt, ht) );
	wxBitmap bmpDistGrid = wxBitmap( imgDistGrid.Scale(wt, ht) );
	wxBitmap bmpImgInv = wxBitmap( imgImgInv.Scale(wt, ht) );
	wxBitmap bmpAreaAdd = wxBitmap( imgAreaAdd.Scale(wt, ht) );
	// online
	wxBitmap bmpOnlineBrowse = wxBitmap( imgOnlineBrowse.Scale(wt,ht) );
	wxBitmap bmpOnlineUploadGroup = wxBitmap( imgOnlineUploadGroup.Scale(wt,ht) );
	wxBitmap bmpOnlineUpload = wxBitmap( imgOnlineUpload.Scale(wt,ht) );
	// section frame tools
	wxBitmap bmpViewThumbs = wxBitmap( imgViewThumbs.Scale(wt, ht) );
	wxBitmap bmpViewInstruments = wxBitmap( imgViewInstruments.Scale(wt, ht) );

//	pToolBar->SetToolBitmapSize(wxSize(wt, ht));

	pToolBar->AddTool( wxID_BUTTON_NEW, wxT("New"), bmpNewFolder, wxT("New Group") );
	pToolBar->AddTool( wxID_BUTTON_OPEN, wxT("Open"), bmpOpen, wxT("Open Images") );
	pToolBar->AddTool( wxID_BUTTON_SAVE, wxT("Save"), bmpSave, wxT("Save Image") );
	pToolBar->AddSeparator();

	////////////////////
	// create main sizer
	wxFlexGridSizer *pMainSizer = new wxFlexGridSizer( 4,1,0,0 );
	pMainSizer->AddGrowableCol( 0 );
	pMainSizer->AddGrowableRow( 1 );
//	pMainSizer->AddGrowableRow( 2 );
//	pMainSizer->AddGrowableRow( 3 );

	// group select option
	m_pGroupSelect = new wxChoice( pToolBar, wxID_GROUP_SELECT,
										wxDefaultPosition,
										wxSize(120,-1), 0, vectStrGroupSelect,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	m_pGroupSelect->SetSelection( 0 );
	pToolBar->AddControl( m_pGroupSelect );

	// image in the group select option
	m_pImageSelect = new wxChoice( pToolBar, wxID_IMAGE_SELECT,
										wxDefaultPosition,
										wxSize(170,-1), 1, vectStrImageSelect,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	m_pImageSelect->SetSelection( 0 );
	pToolBar->AddControl( m_pImageSelect );

	// online actions
	pToolBar->AddSeparator();
	pToolBar->AddTool( wxID_TOOL_ONLINE_BROWSE, wxT("OnlineBrowse"), bmpOnlineBrowse, wxT("Browse Online Images"), wxITEM_CHECK );
	pToolBar->AddTool( wxID_TOOL_ONLINE_UPLOAD_GROUP, wxT("OnlineUploadGroup"), bmpOnlineUploadGroup, wxT("Upload Image Group Online"), wxITEM_CHECK );
	pToolBar->AddTool( wxID_TOOL_ONLINE_UPLOAD, wxT("OnlineUpload"), bmpOnlineUpload, wxT("Upload Image Online"), wxITEM_CHECK );

	// MATCHING - PLATE SOLVING 
	pToolBar->AddSeparator( );
	/////////////////////////////////////////////
	// add target - main button to run detection and find in one go
	pToolBar->AddTool( wxID_BUTTON_OBJ_DETECT_AND_MATCH, wxT("Match"), bmpMatch, wxT("Match Stars"), wxITEM_CHECK );
	pToolBar->AddSeparator( );

	/////////////////////////////////////////////
	// add in image object finder button
//	pToolBar->AddTool( wxID_BUTTON_OBJDETECT, wxT("Detect"), bmpObjDetect, wxT("Detect Objects"), wxITEM_CHECK );

	//////////////////////
	// detect option - profiles
//	m_pDetectOption = new wxChoice( pToolBar, wxID_OPTION_DETECT, wxDefaultPosition,
//										wxSize(80,-1), 2, vectStrDetectOption,
//										wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL );
//
//	m_pDetectOption->SetSelection( 0 );
//	pToolBar->AddControl( m_pDetectOption );

//	pToolBar->AddSeparator();
	// and now add match button and option
//	pToolBar->AddTool( wxID_BUTTON_MATCH, wxT("Match"), bmpThink, _("Match Stars"), wxITEM_CHECK );
	// clue/hint button 
//	pToolBar->AddTool( wxID_BUTTON_POSHINT, wxT("PosHint"), bmpClue, _("Set Position Hint"), wxITEM_CHECK );	
//	pToolBar->AddSeparator();

	////////////////////////////////////////////
	// object data fetch
//	pToolBar->AddTool( wxID_BUTTON_OBJDATA_FETCH, wxT("Fetch"), bmpObjData, wxT("Fetch Objects Data"), wxITEM_CHECK );
	// object find
	pToolBar->AddTool( wxID_BUTTON_OBJFIND, wxT("Find"), bmpObjFind, wxT("Find Objects"), wxITEM_CHECK );
	// distance tool
	pToolBar->AddTool( wxID_BUTTON_MEASURE_DISTANCE, wxT("Distance"), bmpDistanceTool, wxT("Measure Distance"), wxITEM_CHECK );
	// sky grid 
	pToolBar->AddTool( wxID_BUTTON_SKY_GRID, wxT("SkyGrid"), bmpSkyGrid, wxT("Sky Grid Lines"), wxITEM_CHECK );
	// distorsion grid
	pToolBar->AddTool( wxID_BUTTON_DISTORSION_GRID, wxT("DistorsionGrid"), bmpDistGrid, wxT("Distorsion Grid Lines"), wxITEM_CHECK );
	// image tools
	pToolBar->AddSeparator();
	pToolBar->AddTool( wxID_BUTTON_IMGZOOM, wxT("Zoom"), bmpImgZoom, wxT("Zoom Image"), wxITEM_CHECK );
	pToolBar->AddTool( wxID_BUTTON_IMGINV, wxT("Invert"), bmpImgInv, wxT("Invert Colors"), wxITEM_CHECK );
	pToolBar->AddTool( wxID_BUTTON_AREA_ADD, wxT("NewArea"), bmpAreaAdd, wxT("Add New Area"), wxITEM_CHECK );
	// frame tools: thumbs, instruments 
	pToolBar->AddSeparator();
	pToolBar->AddTool( wxID_TOOL_SHOW_CTRL_THUMBS, wxT("Thumbs"), bmpViewThumbs, wxT("Show Thumbs"), wxITEM_CHECK );
	pToolBar->AddTool( wxID_INSTR_SHOW_CTRL_PANEL, wxT("Instruments"), bmpViewInstruments, wxT("Show Instruments"), wxITEM_CHECK );

	/////////////////////////////////////
	// INSTRUMENTS TOOLBAR
	// graph A select defaults
	wxString vectViewGraphA[2];
	vectViewGraphA[0] = wxT("Histogram");
	// graph B select defaults
	wxString vectViewGraphB[2];
	vectViewGraphB[0] = wxT("Conditions");
	// graph A select defaults
	wxString vectViewGraphC[2];
	vectViewGraphC[0] = wxT("Radial Plot");

	m_pInstrToolBar = new wxToolBar(this,-1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTB_FLAT|wxTB_HORIZONTAL );
	// construct bitmaps
	wxBitmap bmpViewCamera = wxBitmap( camera_xpm );
	wxImage imgViewFocus( wxT("resources/focus-view.gif") );
	wxImage imgViewGraph( wxT("resources/graph_icon.gif") );
	wxImage imgViewSettings( wxT("resources/ico_18_settings.gif") );

	// get toolbar size
	wt = m_pInstrToolBar->GetToolBitmapSize().GetWidth();
    ht = m_pInstrToolBar->GetToolBitmapSize().GetHeight();

	// adjust buttons
	bmpViewCamera = wxBitmap(bmpViewCamera.ConvertToImage().Scale(wt, ht));
	wxBitmap bmpViewFocus = wxBitmap( imgViewFocus.Scale(wt, ht) );
	wxBitmap bmpViewGraph = wxBitmap( imgViewGraph.Scale(wt, ht) );
	wxBitmap bmpViewSettings = wxBitmap( imgViewSettings.Scale(wt, ht) );
	// create camera select
	m_pCameraSelect = new wxChoice( m_pInstrToolBar, wxID_INSTR_CAMERA_SELECT, wxDefaultPosition, wxSize(200,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCameraSelect->SetSelection(0);
	// graph A selector
	m_pGraphASelect = new wxChoice( m_pInstrToolBar, wxID_INSTR_GRAPH_A_SELECT, wxDefaultPosition, wxSize(100,-1), 1, 
										vectViewGraphA, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pGraphASelect->SetSelection(0);
	// graph B selector
	m_pGraphBSelect = new wxChoice( m_pInstrToolBar, wxID_INSTR_GRAPH_B_SELECT, wxDefaultPosition, wxSize(100,-1), 1, 
										vectViewGraphB, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pGraphBSelect->SetSelection(0);
	// graph C selector
	m_pGraphCSelect = new wxChoice( m_pInstrToolBar, wxID_INSTR_GRAPH_C_SELECT, wxDefaultPosition, wxSize(100,-1), 1, 
										vectViewGraphC, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pGraphCSelect->SetSelection(0);

	// toolbar buttons
	m_pInstrToolBar->AddTool( wxID_INSTR_SHOW_CTRL_CAMERA, wxT("Camera"), bmpViewCamera, wxT("Show Camera"), wxITEM_CHECK );
	m_pInstrToolBar->AddControl( m_pCameraSelect );
	m_pInstrToolBar->AddSeparator();
	m_pInstrToolBar->AddTool( wxID_INSTR_SHOW_CTRL_FOCUS, wxT("Focus"), bmpViewFocus, wxT("Show Focuser"), wxITEM_CHECK );
	m_pInstrToolBar->AddSeparator();
	m_pInstrToolBar->AddTool( wxID_INSTR_SHOW_CTRL_GRAPH, wxT("Graphs"), bmpViewGraph, wxT("Show Graphs"), wxITEM_CHECK );
	m_pInstrToolBar->AddControl( m_pGraphASelect );
	m_pInstrToolBar->AddControl( m_pGraphBSelect );
	m_pInstrToolBar->AddControl( m_pGraphCSelect );
	m_pInstrToolBar->AddSeparator();
	m_pInstrToolBar->AddTool( wxID_INSTR_SHOW_CTRL_SETTINGS, wxT("Settings"), bmpViewSettings, wxT("Show Settings"), wxITEM_CHECK );

	m_pInstrToolBar->Realize();
//	SetToolBar( pToolBarB );
	pMainSizer->Add( m_pInstrToolBar, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxEXPAND );
	m_pInstrToolBar->Hide();

	/////////////////////////////
	// create group/Image sizer
	sizerGroupAndImage = new wxFlexGridSizer( 1, 3, 5, 5 );
	sizerGroupAndImage->AddGrowableCol( 1 );
	//sizerGroupAndImage->AddGrowableCol( 2 );
	sizerGroupAndImage->AddGrowableRow( 0 );
	pMainSizer->Add( sizerGroupAndImage, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );

	///////////////////////////////////
	// GROUP BOX/SIZER
	wxStaticBox *pGroupImgBox = new wxStaticBox( this, -1, wxT("group view"),
							wxDefaultPosition, wxSize(170,-1) );
	wxStaticBoxSizer *pGroupImgSizer = new wxStaticBoxSizer( pGroupImgBox, wxHORIZONTAL );
	sizerGroupAndImage->Add( pGroupImgSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );
	//////////////////
	// build group image canvas
	m_pGroupImgView = new CGroupImgView( this, wxID_GROUP_THUMBNAILS_LIST, wxPoint(5,58), wxSize(170, -1) );
	pGroupImgSizer->Add( m_pGroupImgView, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );
	m_pGroupImgView->Hide();

	/////////////////////////////////////////////////////////////
	// build image box/sizer
	m_pImageBox = new wxStaticBox( this, -1, wxT("image view"),
							wxDefaultPosition, wxDefaultSize );
	wxStaticBoxSizer *pImageSizer = new wxStaticBoxSizer( m_pImageBox, wxVERTICAL );
	sizerGroupAndImage->Add( pImageSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );
	//////////////////
	// build image canvas
	m_pImgView = new CAstroImgView( this, wxDefaultPosition, wxDefaultSize );
	m_pImgView->m_pFrame = this;
	pImageSizer->Add( m_pImgView, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );
	m_pImgView->SetAutoLayout( 1 );

	/////////////////////////////
	// graphs panel
	m_pGraphPanel = new wxPanel( this, -1, wxDefaultPosition, wxSize(170,-1) );
	sizerGroupAndImage->Add( m_pGraphPanel, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );
//	m_pGraphPanel->Hide();

	/////////////////////////////
	// instruments panel
	m_pCamTelPanel = new wxPanel( this, -1, wxDefaultPosition, wxSize(-1,115) );
	pMainSizer->Add( m_pCamTelPanel, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );
//	m_pCamTelPanel->Hide();

	/////////////////////////
	// Logger Box/sizer
	wxStaticBox *pLoggerBox = new wxStaticBox( this, -1, wxT("process logs"),
							wxDefaultPosition, wxSize(width,120) );
	wxStaticBoxSizer *pLoggerSizer = new wxStaticBoxSizer( pLoggerBox, wxHORIZONTAL );
	pMainSizer->Add( pLoggerSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );

	/////////////////////////////
	// logger view
	m_pLoggerView = new wxTextCtrl(this, -1, wxT(""),
			wxDefaultPosition, wxSize(width,60), wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxTE_RICH2 );

	pLoggerSizer->Add( m_pLoggerView, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );
//	m_pLoggerView->Hide();

	// bring out the tool
	pToolBar->Realize();

	this->SetSizer( pMainSizer );
	pMainSizer->SetSizeHints( this );

//	this->SetAutoLayout( TRUE );
//	sizer1->Fit( this );

	// display my stuff
	SetAutoLayout( TRUE );

	// hide the two firstm - has to be done here for autolaout to learn ???
	m_pGraphPanel->Hide();
	m_pCamTelPanel->Hide();

	// set to accept drag files
	DragAcceptFiles( true );

	//////////////////////////
	// finaly set default status bar values
	SetStatusZoom( 0 );
	SetStatusXY( 0, 0 );
	SetStatusRaDec( 0.0, 0.0 );
	SetStatusDetected( 0 );
	SetStatusMatched( 0 );
	SetStatusDso( 0 );
	SetStatusCatStar( 0 );

	// get window size
	GetClientSize(&m_nWidth, &m_nHeight);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CGUIFrame
// Purose:	delete/destroy GUI MainFrame object
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CGUIFrame::~CGUIFrame( )
{
	// detele print data
	delete( m_pPrintData );
	delete( m_pPageSetupData );
	delete( m_pFrameTaskBarIcon );

	// intruments related
	if( m_bInstrActive ) DeleteInstrumentsPanel( );
	delete( m_pCameraSelect );
	delete( m_pGraphASelect );
	delete( m_pGraphBSelect );
	delete( m_pGraphCSelect );
	delete( m_pInstrToolBar );
	delete( m_pGraphPanel );
	delete( m_pCamTelPanel );

	// remove slideshow timer if set
	DeleteSlideShowTimer( );

	// stop and delete the thread
//	m_pUnimapWorker->DoPause( );
	m_pUnimapWorker->Delete( );

	// remove tools measure distance
	if( m_pDistanceTool )
	{
		delete( m_pDistanceTool );
		m_pDistanceTool = NULL;
	}	

	// save current profile if exists
	if( m_pAstroImage ) 
	{
		m_pAstroImage->SaveProfile( 1 );
		m_pAstroImage->FreeData();
	}
	m_pAstroImage = NULL;
	m_pSextractor = NULL;
	m_pStarMatch = NULL;
	m_pSky = NULL;
	m_pImageDb = NULL;

	// clean on exit
//	delete( m_pImageBox ); - i think this is deleted by box sizer
	delete( m_pImgView );
//	delete( m_pImgView3D );
	delete( m_pLoggerView );
	delete( m_pGroupImgView );
	// set to null
	m_pImgView = NULL;
//	m_pImgView3D = NULL;
	m_pLoggerView = NULL;
	m_pGroupImgView = NULL;

//	delete( m_pDetectOption );
	delete( m_pImageSelect );
	delete( m_pGroupSelect );
//	m_pDetectOption = NULL;
	m_pImageSelect = NULL;
	m_pGroupSelect = NULL;

//	delete( this );

	m_pMainConfig = NULL;
	m_pDetectConfig = NULL;
	m_pUnimapWorker = NULL;

	// and do the app reset to null as well
	m_pUnimap->m_pUnimapWorker = NULL;
	m_pUnimap->m_pFrame = NULL;

	// delete service thread
	/*if( m_pUnimap->m_pUnimapService != NULL )
	{
		m_pUnimap->m_pUnimapService->Pause( );
		m_pUnimap->m_pUnimapService->StopService();
		m_pUnimap->m_pUnimapService->Delete( );
		m_pUnimap->m_pUnimapService = NULL;
	}*/

}

////////////////////////////////////////////////////////////////////
// Purose:	set reference to logger
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetLogger( CLogger* pLogger )
{
	m_pLogger = pLogger;
	m_pImgView->m_pLogger = pLogger;
	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetSky( CSky* pSky )
{
	m_pSky = pSky;
	m_pImgView->m_pSky = m_pSky;
	return;
}

////////////////////////////////////////////////////////////////////
// Purose:	set my configuration and initialize
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetConfig( CConfigMain* pMainConfig, 
								CConfigDetect* pDetectConfig )
{
	// check if valid reference
	if( !pMainConfig ) return;
	int bChange = 0;
	int nVarInt = 0;
	double nVarDouble = 0.0;
	char strVar[255];

	m_pMainConfig = pMainConfig;
	m_pDetectConfig = pDetectConfig;
	// also set children
	m_pImgView->m_pMainConfig = m_pMainConfig;

	////////////////////
	// init print data
    m_pPrintData = new wxPrintData;
    // You could set an initial paper size here - LETTER for Americans, A4 for everyone else  

    m_pPageSetupData = new wxPageSetupDialogData;
    // copy over initial paper size from print record
    (*m_pPageSetupData) = *m_pPrintData;
    // Set some initial page margins in mm. 
	m_pPageSetupData->SetMarginTopLeft(wxPoint(15, 15));
    m_pPageSetupData->SetMarginBottomRight(wxPoint(15, 15));
	m_pPageSetupData->SetPaperId( wxPAPER_A4 );
	(*m_pPrintData) = m_pPageSetupData->GetPrintData();

 	m_pPrintData->SetPaperId( wxPAPER_A4 );
	m_pPrintData->SetQuality( wxPRINT_QUALITY_HIGH );
	m_pPrintData->SetColour( 1 );
	

	//  first get view menu
	wxMenu* pViewMenu = GetMenuBar()->GetMenu( 1 );

	// set detection options
//	SetDetectOptions( );

	///////////////////
	// get main config data?
	m_nWidth = m_pMainConfig->m_nGuiFrameWidth;
	m_nHeight = m_pMainConfig->m_nGuiFrameHeight;

	//////////////////////////
	// :: view constellations
	SetConstellationsMenu( );
	SetMenuCnstlState( m_pMainConfig->m_bShowConstellations );

	// set view areas
	pViewMenu->Check( wxID_MENU_VIEW_IMAGE_AREAS, m_pMainConfig->m_bViewImageAreas );

	/* --- to clean when stable - replaced by method up
	m_bShowConstellations = 1;
	if( m_pMainConfig->GetIntVar( SECTION_ID_GUI, CONF_CONSTL_VIEW, m_bShowConstellations, 1 ) == 0 ) bChange = 1;
	// set image view
	m_pImgView->m_bShowConstellations = m_bShowConstellations;
	// also set gui
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_ARTWORK, 1 );
	if( m_bShowConstellations )
		pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_HARAY, 1 );
	else
		pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_HARAY, 0 );
*/

	/////////////////////////////
	// units format
	if( m_pMainConfig->m_nSkyCoordDisplayFormat == UNITS_FORMAT_SEXAGESIMAL )
	{
		int nWidths[10] =  { -10, 40, 70, 190, 60, 42, 55, 47, 23 };
		GetStatusBar()->SetStatusWidths( 9, nWidths );

	} else if( m_pMainConfig->m_nSkyCoordDisplayFormat == UNITS_FORMAT_DECIMAL )
	{
		int nWidths[10] =  { -10, 40, 70, 140, 60, 42, 55, 47, 23 };
		GetStatusBar()->SetStatusWidths( 9, nWidths );
	}
	// and also call to reformat toolbar 
	//SetStatusRaDec( 999.9999, -99.9999 );
	SetStatusRaDec( m_nLastCurRa, m_nLastCurDec );


	// check if default updates then save config
	if( bChange > 0 ) m_pMainConfig->Save( );

	//////////////////////////////////////////////////////
	// now using the value update my frame and menus/etc
	// check if logs are to be shown
	if( m_pMainConfig->m_bLogsView == 1 )
	{
		// show logs
		m_pLoggerView->Show();
		pViewMenu->Check( wxID_MENU_VIEW_LOGGER, 1 );
	} else
	{
		// hide logs
		m_pLoggerView->Hide();
		pViewMenu->Check( wxID_MENU_VIEW_LOGGER, 0 );
	}

	// set menu states on startup
//	SetMenuState( 0, 0 );

	// set size and reset layout
	SetSize( m_nWidth, m_nHeight );
	GetSizer()->Layout( );
	this->Layout( );

	// set flag init to 1 so onsize knows that config was done
	m_bInit = 1;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetParentApp
// Class:	CGUIFrame
// Purose:	Set/propagate pointer to parrent application here
// Input:	pointer to parent app - umouse
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetParentApp( CUnimap *pUnimap )
{
	m_pUnimap = pUnimap;
}

////////////////////////////////////////////////////////////////////
// Purpose:	show help manual
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnHelp( wxCommandEvent& pEvent )
{
	m_rHelp.Initialize( wxT("unimap.chm") );
	m_rHelp.DisplayContents();
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnFeedback( wxCommandEvent& pEvent )
{
	CUserFeedbackDlg dlg( this, wxT("User Feedback"), m_pUnimap->m_pUnimapOnline );
	if( dlg.ShowModal() == wxID_USER_FEEDBACK_SUBMIT )
	{
		int nType = dlg.m_pType->GetSelection();
		int nSectionId = dlg.m_pSection->GetSelection();
		wxString strTitle = dlg.m_pTitle->GetLineText(0);
		wxString strMsg = dlg.m_pBody->GetValue();
		m_pUnimap->m_pUnimapOnline->PostFeedback( nType, nSectionId, strTitle, strMsg );
	}
}

// Purpose:	display about dialog
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnAbout( wxCommandEvent& pEvent )
{
	CUnimapAbout dialog(this);
    dialog.ShowModal();

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnSaveImage
// Class:		CGUIFrame
// Purpose:		popup save dialog
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnSaveImage( wxCommandEvent& pEvent )
{
	CImageSaveDlg dlg( this, wxT("Save Image"), m_pAstroImage );

	if( dlg.ShowModal( ) == wxID_OK )
	{
		// get name
		wxString strFilePathToSave = dlg.m_pPathSetEntry->GetLineText(0) + wxT("/") +
										dlg.m_pFileSetEntry->GetLineText(0);
		int nWidth = dlg.m_pSizeWidth->GetValue();
		int nHeight = dlg.m_pSizeHeight->GetValue();
		int nTags = dlg.m_pUseTags->GetValue();

		// save by type 0=with tags, 1=without
		m_pImgView->WriteImage( strFilePathToSave, nWidth, nHeight, nTags );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnNextImage
// Class:		CGUIFrame
// Purpose:		display about dialog
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnNextImage( wxCommandEvent& pEvent )
{
	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnBackImage
// Class:		CGUIFrame
// Purpose:		display about dialog
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnBackImage( wxCommandEvent& pEvent )
{
	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnGroupHint
// Class:		CGUIFrame
// Purpose:		create dialog to set group position hint
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnGroupHint( wxCommandEvent& pEvent )
{
	// gui dialog to set position hint
	SetGroupPosHint( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnPosHint
// Class:		CGUIFrame
// Purpose:		create dialog to set position hint
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnPosHint( wxCommandEvent& pEvent )
{
	// first we check if images was loaded
	if( m_pAstroImage == NULL )
	{
		wxMessageBox( wxT("No Image is currently selected"), 
						wxT("Warning"), wxICON_EXCLAMATION );
		// set button back to normal
//		GetToolBar()->ToggleTool( wxID_BUTTON_POSHINT, 0 );
		return;
	}

	// gui dialog to set position hint
	SetPosHint( );

	// set button back to normal
//	GetToolBar()->ToggleTool( wxID_BUTTON_POSHINT, 0 );
	
	return;
}

// Purpose:	when sky grid button is clicked
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnSkyGrid( wxCommandEvent& pEvent )
{
	if( !m_pAstroImage )
	{
		GetToolBar()->ToggleTool( wxID_BUTTON_SKY_GRID, 0 );
		return;
	}

	if( pEvent.IsChecked() )
		m_pImgView->SetSkyGrid( );
	else
		m_pImgView->DeleteSkyGrid( );

	return;
}

// Purpose:	when distotion grid button is clicked
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnDistortionGrid( wxCommandEvent& pEvent )
{
	if( !m_pAstroImage )
	{
		GetToolBar()->ToggleTool( wxID_BUTTON_DISTORSION_GRID, 0 );
		return;
	}

	if( pEvent.IsChecked() )
	{
		m_pAstroImage->CalcDistGrid( );
		m_pImgView->m_bIsDistGrid = 1;
		m_pImgView->Refresh();

	} else
	{
		m_pImgView->m_bIsDistGrid = 0;
		m_pAstroImage->ClearDistGrid( );
		m_pImgView->Refresh();
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnImageZoomTool( wxCommandEvent& pEvent )
{
	if( pEvent.IsChecked() )
	{

		m_pImgView->SetTCursor( UNIMAP_CURSOR_IMAGE_ZOOM_IN );
		m_pImgView->m_nInUseTool = IN_USE_TOOL_ZOOM;
		m_pImgView->m_bZoomDirection = 0;

	} else
	{
		m_pImgView->SetTCursor();
		m_pImgView->m_nInUseTool = IN_USE_TOOL_DEFAULT;
	}

}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnImageAddArea( wxCommandEvent& pEvent )
{
	if( pEvent.IsChecked() )
	{
		m_pImgView->SetTCursor( UNIMAP_CURSOR_IMAGE_ADD_AREA );
		m_pImgView->m_nInUseTool = IN_USE_TOOL_ADD_AREA;
		//m_pImgView->m_bZoomDirection = 0;

		// force view areas
		wxMenu* pViewMenu = GetMenuBar()->GetMenu( 1 );
		pViewMenu->Check( wxID_MENU_VIEW_IMAGE_AREAS, 1 );
		m_pMainConfig->m_bViewImageAreas = 1; 
		m_pImgView->Refresh(false);

	} else
	{
		m_pImgView->SetTCursor();
		m_pImgView->m_nInUseTool = IN_USE_TOOL_DEFAULT;
	}

}

////////////////////////////////////////////////////////////////////
// Method:		SetPosHint
// Class:		CGUIFrame
// Purpose:		create dialog to set position hint
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetPosHint( )
{
	CPosHintDlg *pPosHint = new CPosHintDlg( this, _("Set Image Position Hint"), 0 );
	pPosHint->SetConfig( m_pMainConfig, m_pSky, m_pAstroImage, m_pUnimap->m_pObserver, m_pUnimap->m_pCamera, m_pUnimap->m_pTelescope );
	// check if return is ok
	if ( pPosHint->ShowModal( ) == wxID_OK )
	{
		int nHint = pPosHint->m_pUseImgPosHint->GetSelection();
		if( nHint != m_pAstroImage->m_nUseHint ) m_pAstroImage->m_bIsChanged = 1;
		m_pAstroImage->m_nUseHint = nHint;
		// check if use hint was set
		if( nHint )
		{
			// update the base fields
			pPosHint->UpdateRaDecValues( -1 );
			pPosHint->UpdateFieldValues( -1 );
			// set position hint params into the astro image
			m_pAstroImage->SetHint( pPosHint->m_nRaDeg, pPosHint->m_nDecDeg, 
									pPosHint->m_nFieldX, pPosHint->m_nFieldY );
			// set other image vars
			if( pPosHint->m_pOrigOptA->GetValue() )
				m_pAstroImage->m_nHintOrigType = 0;
			else
				m_pAstroImage->m_nHintOrigType = 1;
			m_pAstroImage->m_nHintObjOrigType = pPosHint->m_pObjectType->GetSelection();
			m_pAstroImage->m_nHintObjName = pPosHint->m_pObjectName->GetSelection();
			m_pAstroImage->m_strHintObjId = pPosHint->m_pObjectId->GetLineText(0);
			m_pAstroImage->m_nHintOrigUnits = pPosHint->m_pRaDecEntryType->GetSelection();
			// field
			if( pPosHint->m_pFieldOptA->GetValue() )
				m_pAstroImage->m_nHintFieldType = 0;
			else
				m_pAstroImage->m_nHintFieldType = 1;

			// setup
			m_pAstroImage->m_nHintSetupId = pPosHint->m_pSetupSelect->GetSelection();

			// camera
			m_pAstroImage->m_nHintCameraSource = pPosHint->m_nCameraSource;
			m_pAstroImage->m_nHintCameraType = pPosHint->m_nCameraType;
			m_pAstroImage->m_nHintCameraBrand = pPosHint->m_nCameraBrand;
			m_pAstroImage->m_nHintCameraName = pPosHint->m_nCameraName;
//			if( m_pAstroImage->m_nHintCameraSource == 1 )
//				m_pAstroImage->m_nHintCameraName = pPosHint->m_nCameraNameOwn;

			// sensor
			if( !pPosHint->m_pSensorWidth->GetLineText(0).ToDouble( &(m_pAstroImage->m_nHintSensorWidth) ) )
				m_pAstroImage->m_nHintSensorWidth = 0.0;
			if( !pPosHint->m_pSensorHeight->GetLineText(0).ToDouble( &(m_pAstroImage->m_nHintSensorHeight) ) )
				m_pAstroImage->m_nHintSensorHeight = 0.0;

			// telescope
			m_pAstroImage->m_nHintTLensSource = pPosHint->m_nTLensSource;
			// copy own to name - if source
			if( m_pAstroImage->m_nHintTLensSource == 1 )
			{
				pPosHint->m_rLensSetup.nLensName = pPosHint->m_rLensSetup.nLensNameOwn;
				pPosHint->m_rLensSetup.nBarlowLensName = pPosHint->m_rLensSetup.nBarlowLensNameOwn;
				pPosHint->m_rLensSetup.nFocalReducerName = pPosHint->m_rLensSetup.nFocalReducerNameOwn;
				pPosHint->m_rLensSetup.nEyepieceName = pPosHint->m_rLensSetup.nEyepieceNameOwn;
				pPosHint->m_rLensSetup.nCameraLensName = pPosHint->m_rLensSetup.nCameraLensNameOwn;
			}
			// copy structure
			memcpy( &(m_pAstroImage->m_rHintTLensSetup), &(pPosHint->m_rLensSetup), sizeof(DefTLensSetup) );

			// system focal length
			if( !pPosHint->m_pFocal->GetLineText(0).ToDouble( &(m_pAstroImage->m_nHintFocalLength) ) )
					m_pAstroImage->m_nHintFocalLength = 0.0;

			// field format
			m_pAstroImage->m_nHintFieldFormat = pPosHint->m_pFieldFormat->GetSelection();

			// and also set the config
			m_pMainConfig->SetIntVar( SECTION_ID_HINT, CONF_HINT_CAMERA_BRAND, m_pAstroImage->m_nHintCameraBrand );
			m_pMainConfig->SetIntVar( SECTION_ID_HINT, CONF_HINT_CAMERA_NAME, m_pAstroImage->m_nHintCameraName );
			m_pMainConfig->SetFloatVar( SECTION_ID_HINT, CONF_HINT_FOCAL_LENGTH, m_pAstroImage->m_nHintFocalLength );
			m_pMainConfig->SetIntVar( SECTION_ID_HINT, CONF_HINT_FIELD_UNITS, m_pAstroImage->m_nHintFieldType );
			m_pMainConfig->SetFloatVar( SECTION_ID_HINT, CONF_HINT_FIELD_WIDTH, pPosHint->m_nFieldX );
			m_pMainConfig->SetFloatVar( SECTION_ID_HINT, CONF_HINT_FIELD_HEIGHT, pPosHint->m_nFieldY );

			///////////////////////
			// here we set that the image was changed 
			m_pAstroImage->m_bIsChanged = 1;
		}
	}

	delete( pPosHint );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SetGroupPosHint
// Class:		CGUIFrame
// Purpose:		set group position hint
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetGroupPosHint( )
{
	CImageGroup* pImgGroup = m_pImageDb->m_pCurrentGroup;
	// create hint dialog
	CPosHintDlg *pPosHint = new CPosHintDlg( this, _("Set Group Position Hint"), 1 );
	pPosHint->SetGroupConfig( m_pMainConfig, m_pSky, pImgGroup, m_pUnimap->m_pObserver, m_pUnimap->m_pCamera, m_pUnimap->m_pTelescope );
	// check if return is ok
	if ( pPosHint->ShowModal( ) == wxID_OK )
	{
//		pImgGroup->m_bIsHint = pPosHint->m_pUseGroupPosHint->GetValue();
		// check if use hint was set
		if( pPosHint->m_bUseGroupHint )
		{
			// update the base fields
			pPosHint->UpdateRaDecValues( -1 );
			pPosHint->UpdateFieldValues( -1 );
			// set position hint params into the astro image
			pImgGroup->SetHint( pPosHint->m_nRaDeg, pPosHint->m_nDecDeg, 
									pPosHint->m_nFieldX, pPosHint->m_nFieldY );
			// set other image vars
			if( pPosHint->m_pOrigOptA->GetValue() )
				pImgGroup->m_nHintOrigType = 0;
			else
				pImgGroup->m_nHintOrigType = 1;

			pImgGroup->m_nHintObjOrigType = pPosHint->m_pObjectType->GetSelection();
			pImgGroup->m_nHintObjName = pPosHint->m_pObjectName->GetSelection();
			pImgGroup->m_strHintObjId = pPosHint->m_pObjectId->GetLineText(0);
			pImgGroup->m_nHintOrigUnits = pPosHint->m_pRaDecEntryType->GetSelection();
			// field
			if( pPosHint->m_pFieldOptA->GetValue() )
				pImgGroup->m_nHintFieldType = 0;
			else
				pImgGroup->m_nHintFieldType = 1;

			// setup
			pImgGroup->m_nHintSetupId = pPosHint->m_pSetupSelect->GetSelection();

			// camera
			pImgGroup->m_nHintCameraSource = pPosHint->m_nCameraSource;
			pImgGroup->m_nHintCameraType = pPosHint->m_nCameraType;
			pImgGroup->m_nHintCameraBrand = pPosHint->m_nCameraBrand;
			pImgGroup->m_nHintCameraName = pPosHint->m_nCameraName;
//			if( pImgGroup->m_nHintCameraSource == 1 )
//				pImgGroup->m_nHintCameraName = pPosHint->m_nCameraNameOwn;

			// sensor
			if( !pPosHint->m_pSensorWidth->GetLineText(0).ToDouble( &(pImgGroup->m_nHintSensorWidth) ) )
				pImgGroup->m_nHintSensorWidth = 0.0;
			if( !pPosHint->m_pSensorHeight->GetLineText(0).ToDouble( &(pImgGroup->m_nHintSensorHeight) ) )
				pImgGroup->m_nHintSensorHeight = 0.0;

			// telescope
			pImgGroup->m_nHintTLensSource = pPosHint->m_nTLensSource;
			// copy own to name - if source
			if( pImgGroup->m_nHintTLensSource == 1 )
			{
				pPosHint->m_rLensSetup.nLensName = pPosHint->m_rLensSetup.nLensNameOwn;
				pPosHint->m_rLensSetup.nBarlowLensName = pPosHint->m_rLensSetup.nBarlowLensNameOwn;
				pPosHint->m_rLensSetup.nFocalReducerName = pPosHint->m_rLensSetup.nFocalReducerNameOwn;
				pPosHint->m_rLensSetup.nEyepieceName = pPosHint->m_rLensSetup.nEyepieceNameOwn;
				pPosHint->m_rLensSetup.nCameraLensName = pPosHint->m_rLensSetup.nCameraLensNameOwn;
			}
			// copy structure
			memcpy( &(pImgGroup->m_rHintTLensSetup), &(pPosHint->m_rLensSetup), sizeof(DefTLensSetup) );

			// telescope/lens
			if( !pPosHint->m_pFocal->GetLineText(0).ToDouble( &(pImgGroup->m_nHintFocalLength) ) )
					pImgGroup->m_nHintFocalLength = 0;

			// fields
			pImgGroup->m_nHintFieldFormat = pPosHint->m_pFieldFormat->GetSelection();

			// and also set the config
			m_pMainConfig->SetIntVar( SECTION_ID_HINT, CONF_HINT_CAMERA_TYPE, 
												pImgGroup->m_nHintCameraType );
			m_pMainConfig->SetFloatVar( SECTION_ID_HINT, CONF_HINT_FOCAL_LENGTH, 
												pImgGroup->m_nHintFocalLength );
			m_pMainConfig->SetIntVar( SECTION_ID_HINT, CONF_HINT_FIELD_UNITS, 
												pImgGroup->m_nHintFieldType );
			m_pMainConfig->SetFloatVar( SECTION_ID_HINT, CONF_HINT_FIELD_WIDTH, 
												pPosHint->m_nFieldX );
			m_pMainConfig->SetFloatVar( SECTION_ID_HINT, CONF_HINT_FIELD_HEIGHT, 
												pPosHint->m_nFieldY );

			//////////////////////////
			// check if to apply pos hint to all images
			pImgGroup->m_nHintImgUseGroup = pPosHint->m_pUseImgGroupPosHint->GetSelection();
			if( pImgGroup->m_nHintImgUseGroup > 0 )
			{
				pImgGroup->SetHintToAllImages( pImgGroup->m_nHintImgUseGroup );
			}


			///////////////////////
			// here we set that the image was changed 
			pImgGroup->m_bIsChanged = 1;

		} else
		{
			pImgGroup->m_bIsHint = 0;
		}
	}

	delete( pPosHint );

	return;
}

// Purpose:		when detect & match button is clicked
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnDetectAndMatch( wxCommandEvent& pEvent )
{
	int nDetectStatus = 0;
	int nMatchStatus = 0;

	// check if image
	if( m_pAstroImage )
	{
		/////////////
		// first call detection
		// - check if already detected ok - at least 7
		if( m_pAstroImage->m_nStar > 7 )
		{
			nDetectStatus = 1;
			// ask if to redetect with a different profile
			int answer = wxMessageBox( wxT("Light sources already detected!\n Do you want to try again with different settings?"), wxT("Confirm"), wxYES_NO|wxICON_QUESTION, this );
			if( answer == wxYES )
			{
				// pop the detection config dialog
				ConfigDetection();
				nDetectStatus = RunObjectDetection( 1 );
			} 

		} else
		{
			nDetectStatus = RunObjectDetection( 1 );
		}

		////////////////
		// select catalog if none selected so far
		if( m_pAstroImage->m_nCatalogForMatching < 0 ) ConfigCatalog();

		///////////
		// the if detection ok - run match
		if( nDetectStatus ) nMatchStatus = RunObjectMatching( 1 );
		
		// do somethign if ok ???
		//if( nDetectStatus && nMatchStatus ) 
	}

	// toogle button back off
	GetToolBar()->ToggleTool( wxID_BUTTON_OBJ_DETECT_AND_MATCH, 0 );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnMatchClear
// Class:		CGUIFrame
// Purpose:		clear matched
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnMatchClear( wxCommandEvent& pEvent )
{
	m_pAstroImage->ClearMatched( );
	// and reset all abjects to menu options
	SetViewAllObjects( );

	// todo :: execption - this should go in setviewall objects
	// .. but there for now it will be called twice once from ther and 
	// once from imgview set image
//	m_pImgView->SetCostellations( m_pAstroImage );

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnClearAreas( wxCommandEvent& pEvent )
{
	m_pAstroImage->m_vectArea.clear();
	m_pImgView->Refresh(false);
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnImageDelete( wxCommandEvent& pEvent )
{
	int nImageId = m_pImageSelect->GetSelection()-1;

	wxString strMsg=wxT("");

	int nGroup = m_pGroupSelect->GetSelection();
	strMsg.Printf( wxT("Delete image %s ?"), m_pAstroImage->GetImageName() );
	int nOk = wxMessageBox(  strMsg, wxT("Confirm"), wxICON_EXCLAMATION|wxYES_NO );

	if( nOk == wxYES )
	{
		if( m_pAstroImage ) UnloadSelectedImage();
		m_pImageSelect->Delete( nImageId+1 );
		m_pImageSelect->SetSelection(0);
		m_pImageDb->m_pCurrentGroup->DeleteImage( nImageId );

		if( m_pMainConfig->m_bShowImageThumbs ) UpdateGroupThumbnails( m_nCurrentGroup );
		SetMenuState( 0, 0 );
	}
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetViewAllObjects( int bNoCnstl )
{
	// check if to hide all
	if( m_pMainConfig->m_bViewObjectsType == 2 ) 
		m_pImgView->m_bDrawObjects = 0;
	else
		m_pImgView->m_bDrawObjects = 1;

	if( m_pMainConfig->m_bViewObjectsType == 1 )
	{
		m_pImgView->SetStars( m_pAstroImage, -1 );
		m_pImgView->SetCatStars( m_pAstroImage, -1 );
		m_pImgView->SetDsoObjects( m_pAstroImage, -1 );
		m_pImgView->SetRadioSources( m_pAstroImage, -1 );
		m_pImgView->SetXGammaSources( m_pAstroImage, -1 );
		m_pImgView->SetSupernovas( m_pAstroImage, -1 );
		m_pImgView->SetBlackholes( m_pAstroImage, -1 );
		m_pImgView->SetMStars( m_pAstroImage, -1 );
		m_pImgView->SetExoplanets( m_pAstroImage, -1 );
		m_pImgView->SetAsteroids( m_pAstroImage, -1 );
		m_pImgView->SetComets( m_pAstroImage, -1 );
		m_pImgView->SetAes( m_pAstroImage, -1 );
		m_pImgView->SetSolar( m_pAstroImage, 100 );

	} else
	{
		m_pImgView->SetStars( m_pAstroImage, m_pMainConfig->m_nViewDetectedObjects );
		m_pImgView->SetCatStars( m_pAstroImage, m_pMainConfig->m_nViewCatalogStars );
		m_pImgView->SetDsoObjects( m_pAstroImage, m_pMainConfig->m_nViewCatalogDso );
		m_pImgView->SetRadioSources( m_pAstroImage, m_pMainConfig->m_nViewCatalogRadio );
		m_pImgView->SetXGammaSources( m_pAstroImage, m_pMainConfig->m_nViewCatalogXGamma );
		m_pImgView->SetSupernovas( m_pAstroImage, m_pMainConfig->m_nViewCatalogSupernovas );
		m_pImgView->SetBlackholes( m_pAstroImage, m_pMainConfig->m_nViewCatalogBlackholes );
		m_pImgView->SetMStars( m_pAstroImage, m_pMainConfig->m_nViewCatalogMStars );
		m_pImgView->SetExoplanets( m_pAstroImage, m_pMainConfig->m_nViewCatalogExoplanets );
		m_pImgView->SetAsteroids( m_pAstroImage, m_pMainConfig->m_nViewCatalogAsteroids );
		m_pImgView->SetComets( m_pAstroImage, m_pMainConfig->m_nViewCatalogComets );
		m_pImgView->SetAes( m_pAstroImage, m_pMainConfig->m_nViewCatalogAes );
		m_pImgView->SetSolar( m_pAstroImage, 100 );
	}

	if( !bNoCnstl ) m_pImgView->SetCostellations( m_pAstroImage );

	return;
}

////////////////////////////////////////////////////////////////////
// Purpose:	when match button is clicked
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnMatch( wxCommandEvent& pEvent )
{
	int nStatus = 0;
	nStatus = RunObjectMatching( 1 );
	if( !nStatus ) GetToolBar()->ToggleTool( wxID_BUTTON_OBJFIND, 0 );

	return;
}

////////////////////////////////////////////////////////////////////
// Purpose:	run object matching process
////////////////////////////////////////////////////////////////////
int CGUIFrame::RunObjectMatching( int bRun )
{
	// first we check if images was loaded
	if( m_pAstroImage == NULL )
	{
		wxMessageBox( wxT("           No Image is currently selected\n")
						wxT("You need first to select an image and then detect objects"), 
						wxT("Warning"), wxICON_EXCLAMATION );

		return( 0 );

	} else if( m_pAstroImage->m_nStar < 30 )
	{
		wxMessageBox( wxT("No Objects or to few were detected\n"),
						wxT("Warning"), wxICON_EXCLAMATION );
		return( 0 );
	}

	// if no hint given
	if( !m_pAstroImage->IsHint() )
	{
		// display warning notification
		wxString strNoteMsg = wxT( "WARNING! If no hint of position in the sky and field size\n is given, the matching process could take a very long time!\n Click <YES> to set a hint or <NO> to do a full sky search." );
		int nRet = wxMessageBox( strNoteMsg, wxT("Warning"), wxICON_EXCLAMATION|wxYES_NO );

		if( nRet == wxYES )
		{
			// pop up hint selection
			SetPosHint( );
		}
	}

	// set thread to start matching process
	if( bRun ) 
	{
//		m_pUnimapWorker->DoStartMatching( m_pAstroImage );

		wxString strFullTitle = wxT("Matching Process");
		wxString strMsg = wxT("Initializing matching engine...");
		if( m_pAstroImage ) strFullTitle += wxT( " :: " ) + m_pAstroImage->m_strImageName;
		// catalog name
		CatalogDef* pCat = &(m_pSky->m_vectCatalogDef[m_pMainConfig->m_nCatalogForMatching-1]);
		wxString strCatName = pCat->m_strFullName;
		if( strCatName.IsEmpty() ) strCatName = pCat->m_strName;
		if( strCatName.IsEmpty() ) strCatName = pCat->m_strNameAbbreviation;
		if( strCatName.IsEmpty() ) strCatName = pCat->m_strLabel;
		if( strCatName.IsEmpty() ) strCatName.Printf( wxT("id=%d"), m_pMainConfig->m_nCatalogForMatching );

		CWaitDialogMatch* pWaitDialogMatch = new CWaitDialogMatch( this, strFullTitle, strMsg, m_pUnimapWorker, (void*) m_pAstroImage );
		pWaitDialogMatch->SetCatalog( strCatName );
	//	m_pWaitDialog->Show( 1 );
		pWaitDialogMatch->ShowModal();
		delete( pWaitDialogMatch );

		/////////////////
		// run object data fetch if set - todo: move this from worker in main config
		if( m_pMainConfig->m_bMatchFetchDetails && 
			m_pAstroImage->m_bIsMatched ) RunObjectDataFetch( );

		//////////////////////////
		// set limit for image view and redraw
		SetViewAllObjects( );

		// set status bar 
		SetStatusMatched( m_pAstroImage->m_nMatchedStars );
		SetStatusDso( m_pAstroImage->m_nCatDso );
		SetStatusCatStar( m_pAstroImage->m_nCatStar );

		// set menu states
		SetMenuState( 0, 0 );

		///////////////////////
		// IF NO MATCH THEN POPUP INFO DIALOG TO HELP THE USER DIAG
		if( !m_pAstroImage->m_bIsMatched )
		{
			if( m_pSky->m_pSkyFinder->m_bMatchStatus > 10 )
			{
				strMsg = wxT("Could not find a match!\n");
				// check sky finder return
				if( m_pSky->m_pSkyFinder->m_bMatchStatus == MATCH_STATE_SMALL_CATALOG )
					strMsg += wxT("Too few stars in this catalog for the area!\nPlease try a bigger catalog!");
				else if( m_pSky->m_pSkyFinder->m_bMatchStatus == MATCH_STATE_FEW_IMAGE_STARS )
					strMsg += wxT("Too few stars in the image!\nPlease try to lower detection threshold!");
				else if( m_pSky->m_pSkyFinder->m_bMatchStatus == MATCH_STATE_HAS_NO_HINT )
					strMsg += wxT("Maybe you should try using a hint!");
				else
					strMsg += wxT("Try to refine your detection/matching/catalog setting!");
			}

			wxMessageBox( strMsg, wxT("Warning"), wxICON_EXCLAMATION|wxOK );	
		}


		// toogle button back off
//		GetToolBar()->ToggleTool( wxID_BUTTON_MATCH, 0 );
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Purpose:	popup print dialog
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnPrintPreview( wxCommandEvent& WXUNUSED(pEvent) )
{
	// auto check orientation
	if( !m_bHasPageSetup && m_pAstroImage)
	{
		// if width more the height
		if( m_pAstroImage->m_nWidth >= m_pAstroImage->m_nHeight )
			m_pPrintData->SetOrientation( wxLANDSCAPE );
		else
			m_pPrintData->SetOrientation( wxPORTRAIT );
	}

    // Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData( *m_pPrintData );
    wxPrintPreview *preview = new wxPrintPreview( new CImgPrintout(m_pImgView, m_pPageSetupData), 
								new CImgPrintout(m_pImgView, m_pPageSetupData), &printDialogData );
    if (!preview->Ok())
    {
        delete preview;
        wxMessageBox(wxT("There was a problem previewing.\nPerhaps your current printer is not set correctly?"), wxT("Previewing"), wxOK);
        return;
    }

    wxPreviewFrame *frame = new wxPreviewFrame(preview, this, wxT("Demo Print Preview"), wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show();
}

////////////////////////////////////////////////////////////////////
// Purpose:	popup print page setup dialog
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnPageSetup( wxCommandEvent& WXUNUSED(pEvent) )
{
	// auto check orientation
	if( !m_bHasPageSetup && m_pAstroImage)
	{
		// if width more the height
		if( m_pAstroImage->m_nWidth >= m_pAstroImage->m_nHeight )
			m_pPrintData->SetOrientation( wxLANDSCAPE );
		else
			m_pPrintData->SetOrientation( wxPORTRAIT );
	}

 //   (*m_pPageSetupData) = *m_pPrintData;
//	m_pPageSetupData->SetPrintData(*m_pPrintData);
//	m_pPageSetupData->SetPaperId( m_pPrintData->GetPaperId() );

    wxPageSetupDialog pageSetupDialog( this, m_pPageSetupData );
    pageSetupDialog.ShowModal();

    (*m_pPrintData) = pageSetupDialog.GetPageSetupDialogData().GetPrintData();
    (*m_pPageSetupData) = pageSetupDialog.GetPageSetupDialogData();

	m_bHasPageSetup = 1;
}

////////////////////////////////////////////////////////////////////
// Purpose:	popup print dialog
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnPrint( wxCommandEvent& pEvent )
{
	// auto check orientation
	if( !m_bHasPageSetup && m_pAstroImage)
	{
		// if width more the height
		if( m_pAstroImage->m_nWidth >= m_pAstroImage->m_nHeight )
			m_pPrintData->SetOrientation( wxLANDSCAPE );
		else
			m_pPrintData->SetOrientation( wxPORTRAIT );
	}

	//wxPrintDialog* dialog = new wxPrintDialog( m_pImgView );
    wxPrintDialogData printDialogData( *m_pPrintData );
    wxPrinter printer( &printDialogData );
	 // here myprintout
    CImgPrintout printout( m_pImgView, m_pPageSetupData, wxT("Astro-Image Printout") );
    if( !printer.Print( this, &printout, true /*prompt*/ ) )
    {
        if( wxPrinter::GetLastError() == wxPRINTER_ERROR )
            wxMessageBox(wxT("There was a problem printing.\nPerhaps your current printer is not set correctly?"), wxT("Printing"), wxOK);
        else
            wxMessageBox(wxT("You canceled printing"), wxT("Printing"), wxOK);
    } else
    {
        (*m_pPrintData) = printer.GetPrintDialogData().GetPrintData();
    }

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnExitApp( wxCommandEvent& WXUNUSED(pEvent) )
{
	Close();
}

////////////////////////////////////////////////////////////////////
// Purpose:	the global configuration for unimap - fonts etc
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnConfig( wxCommandEvent& pEvent )
{
	int nVarInt = 0;

	CMainConfigView *pConfigView = new CMainConfigView( this );
	pConfigView->SetConfig( m_pMainConfig, this );
	// check if return is ok
	if ( pConfigView->ShowModal( ) == wxID_OK )
	{
		long nIntVar = 0;
		double nFloatVar = 0;
		char strVar[255];

		// set font/color and shape/line localy
		// :: detected
		m_pMainConfig->m_nDetectObjShapeType = pConfigView->m_nDetectObjShapeType;
		m_pMainConfig->m_nDetectObjShapeLine = pConfigView->m_nDetectObjShapeLine;
		m_pMainConfig->fontDObjLabel = pConfigView->fntDObjLabel;
		m_pMainConfig->colorDObjLabel = pConfigView->colDObjLabel;
		// :: matched
		m_pMainConfig->m_nMatchObjShapeType = pConfigView->m_nMatchObjShapeType;
		m_pMainConfig->m_nMatchObjShapeLine = pConfigView->m_nMatchObjShapeLine;
		m_pMainConfig->fontMObjLabel = pConfigView->fntMObjLabel;
		m_pMainConfig->colorMObjLabel = pConfigView->colMObjLabel;
		// :: catalog
		m_pMainConfig->m_nCatObjShapeType = pConfigView->m_nCatObjShapeType;
		m_pMainConfig->m_nCatObjShapeLine = pConfigView->m_nCatObjShapeLine;
		m_pMainConfig->fontCObjLabel = pConfigView->fntCObjLabel;
		m_pMainConfig->colorCObjLabel = pConfigView->colCObjLabel;
		// :: nebula
		m_pMainConfig->m_nNebObjShapeType = pConfigView->m_nNebObjShapeType;
		m_pMainConfig->m_nNebObjShapeLine = pConfigView->m_nNebObjShapeLine;
		m_pMainConfig->fontNebObjLabel = pConfigView->fntNebObjLabel;
		m_pMainConfig->colorNebObjLabel = pConfigView->colNebObjLabel;
		// :: cluster
		m_pMainConfig->m_nCluObjShapeType = pConfigView->m_nCluObjShapeType;
		m_pMainConfig->m_nCluObjShapeLine = pConfigView->m_nCluObjShapeLine;
		m_pMainConfig->fontCluObjLabel = pConfigView->fntCluObjLabel;
		m_pMainConfig->colorCluObjLabel = pConfigView->colCluObjLabel;
		// :: galaxy
		m_pMainConfig->m_nGalObjShapeType = pConfigView->m_nGalObjShapeType;
		m_pMainConfig->m_nGalObjShapeLine = pConfigView->m_nGalObjShapeLine;
		m_pMainConfig->fontGalObjLabel = pConfigView->fntGalObjLabel;
		m_pMainConfig->colorGalObjLabel = pConfigView->colGalObjLabel;
		// :: supernovae
		m_pMainConfig->m_nSNObjShapeType = pConfigView->m_nSNObjShapeType;
		m_pMainConfig->m_nSNObjShapeLine = pConfigView->m_nSNObjShapeLine;
		m_pMainConfig->fontSNObjLabel = pConfigView->fntSNObjLabel;
		m_pMainConfig->colorSNObjLabel = pConfigView->colSNObjLabel;
		// :: radio
		m_pMainConfig->m_nRadObjShapeType = pConfigView->m_nRadObjShapeType;
		m_pMainConfig->m_nRadObjShapeLine = pConfigView->m_nRadObjShapeLine;
		m_pMainConfig->fontRadObjLabel = pConfigView->fntRadObjLabel;
		m_pMainConfig->colorRadObjLabel = pConfigView->colRadObjLabel;
		// :: xray
		m_pMainConfig->m_nXRayObjShapeType = pConfigView->m_nXRayObjShapeType;
		m_pMainConfig->m_nXRayObjShapeLine = pConfigView->m_nXRayObjShapeLine;
		m_pMainConfig->fontXRayObjLabel = pConfigView->fntXRayObjLabel;
		m_pMainConfig->colorXRayObjLabel = pConfigView->colXRayObjLabel;
		// :: gamma
		m_pMainConfig->m_nGammaObjShapeType = pConfigView->m_nGammaObjShapeType;
		m_pMainConfig->m_nGammaObjShapeLine = pConfigView->m_nGammaObjShapeLine;
		m_pMainConfig->fontGammaObjLabel = pConfigView->fntGammaObjLabel;
		m_pMainConfig->colorGammaObjLabel = pConfigView->colGammaObjLabel;
		// :: black holes
		m_pMainConfig->m_nBHObjShapeType = pConfigView->m_nBHObjShapeType;
		m_pMainConfig->m_nBHObjShapeLine = pConfigView->m_nBHObjShapeLine;
		m_pMainConfig->fontBHObjLabel = pConfigView->fntBHObjLabel;
		m_pMainConfig->colorBHObjLabel = pConfigView->colBHObjLabel;
		// :: multiple/double stars
		m_pMainConfig->m_nMSObjShapeType = pConfigView->m_nMSObjShapeType;
		m_pMainConfig->m_nMSObjShapeLine = pConfigView->m_nMSObjShapeLine;
		m_pMainConfig->fontMSObjLabel = pConfigView->fntMSObjLabel;
		m_pMainConfig->colorMSObjLabel = pConfigView->colMSObjLabel;
		// :: asteroids
		m_pMainConfig->m_nASTObjShapeType = pConfigView->m_nASTObjShapeType;
		m_pMainConfig->m_nASTObjShapeLine = pConfigView->m_nASTObjShapeLine;
		m_pMainConfig->fontASTObjLabel = pConfigView->fntASTObjLabel;
		m_pMainConfig->colorASTObjLabel = pConfigView->colASTObjLabel;
		// :: comets
		m_pMainConfig->m_nCOMObjShapeType = pConfigView->m_nCOMObjShapeType;
		m_pMainConfig->m_nCOMObjShapeLine = pConfigView->m_nCOMObjShapeLine;
		m_pMainConfig->fontCOMObjLabel = pConfigView->fntCOMObjLabel;
		m_pMainConfig->colorCOMObjLabel = pConfigView->colCOMObjLabel;
		// :: extrasolar planets
		m_pMainConfig->m_nEXPObjShapeType = pConfigView->m_nEXPObjShapeType;
		m_pMainConfig->m_nEXPObjShapeLine = pConfigView->m_nEXPObjShapeLine;
		m_pMainConfig->fontEXPObjLabel = pConfigView->fntEXPObjLabel;
		m_pMainConfig->colorEXPObjLabel = pConfigView->colEXPObjLabel;
		// :: artificial eart satellites
		m_pMainConfig->m_nAESObjShapeType = pConfigView->m_nAESObjShapeType;
		m_pMainConfig->m_nAESObjShapeLine = pConfigView->m_nAESObjShapeLine;
		m_pMainConfig->fontAESObjLabel = pConfigView->fntAESObjLabel;
		m_pMainConfig->colorAESObjLabel = pConfigView->colAESObjLabel;

		////////////////////
		// here we set the label structure
		// :: detected
		m_pMainConfig->m_bDetectObjLabelUseId = pConfigView->m_bDetectObjLabelUseId;
		m_pMainConfig->m_bDetectObjLabelUseMag = pConfigView->m_bDetectObjLabelUseMag;
		// :: matched
		m_pMainConfig->m_bMatchObjLabelUseId = pConfigView->m_bMatchObjLabelUseId;
		m_pMainConfig->m_bMatchObjLabelUseCatNo = pConfigView->m_bMatchObjLabelUseCatNo;
		m_pMainConfig->m_bMatchObjLabelUseCName = pConfigView->m_bMatchObjLabelUseCName;
		// :: catalog stars
		m_pMainConfig->m_bCatStarLabelUseCatNo = pConfigView->m_bCatObjLabelUseCatNo;
		m_pMainConfig->m_bCatStarLabelUseCName = pConfigView->m_bCatObjLabelUseCName;
		// :: nebula
		m_pMainConfig->m_bNebObjLabelUseCatNo = pConfigView->m_bNebObjLabelUseCatNo;
		m_pMainConfig->m_bNebObjLabelUseCName = pConfigView->m_bNebObjLabelUseCName;
		m_pMainConfig->m_bNebObjLabelUseType = pConfigView->m_bNebObjLabelUseType;
		// :: cluster
		m_pMainConfig->m_bCluObjLabelUseCatNo = pConfigView->m_bCluObjLabelUseCatNo;
		m_pMainConfig->m_bCluObjLabelUseCName = pConfigView->m_bCluObjLabelUseCName;
		m_pMainConfig->m_bCluObjLabelUseType = pConfigView->m_bCluObjLabelUseType;
		// :: galaxy
		m_pMainConfig->m_bGalObjLabelUseCatNo = pConfigView->m_bGalObjLabelUseCatNo;
		m_pMainConfig->m_bGalObjLabelUseCName = pConfigView->m_bGalObjLabelUseCName;
		m_pMainConfig->m_bGalObjLabelUseType = pConfigView->m_bGalObjLabelUseType;
		// :: supernovae
		m_pMainConfig->m_bSNObjLabelUseCatNo = pConfigView->m_bSNObjLabelUseCatNo;
		m_pMainConfig->m_bSNObjLabelUseCName = pConfigView->m_bSNObjLabelUseCName;
		m_pMainConfig->m_bSNObjLabelUseType = pConfigView->m_bSNObjLabelUseType;
		// :: radio
		m_pMainConfig->m_bRadObjLabelUseCatNo = pConfigView->m_bRadObjLabelUseCatNo;
		m_pMainConfig->m_bRadObjLabelUseType = pConfigView->m_bRadObjLabelUseType;
		// :: xray
		m_pMainConfig->m_bXRayObjLabelUseCatNo = pConfigView->m_bXRayObjLabelUseCatNo;
		m_pMainConfig->m_bXRayObjLabelUseType = pConfigView->m_bXRayObjLabelUseType;
		// :: gamma
		m_pMainConfig->m_bGammaObjLabelUseCatNo = pConfigView->m_bGammaObjLabelUseCatNo;
		m_pMainConfig->m_bGammaObjLabelUseType = pConfigView->m_bGammaObjLabelUseType;
		// :: black holes
		m_pMainConfig->m_bBHObjLabelUseCatNo = pConfigView->m_bBHObjLabelUseCatNo;
		m_pMainConfig->m_bBHObjLabelUseType = pConfigView->m_bBHObjLabelUseType;
		// :: multiple/double stars
		m_pMainConfig->m_bMSObjLabelUseCatNo = pConfigView->m_bMSObjLabelUseCatNo;
		m_pMainConfig->m_bMSObjLabelUseType = pConfigView->m_bMSObjLabelUseType;
		// :: asteroids
		m_pMainConfig->m_bASTObjLabelUseCatNo = pConfigView->m_bASTObjLabelUseCatNo;
		m_pMainConfig->m_bASTObjLabelUseType = pConfigView->m_bASTObjLabelUseType;
		// :: comets
		m_pMainConfig->m_bCOMObjLabelUseCatNo = pConfigView->m_bCOMObjLabelUseCatNo;
		m_pMainConfig->m_bCOMObjLabelUseType = pConfigView->m_bCOMObjLabelUseType;
		// :: extrasolar planets
		m_pMainConfig->m_bEXPObjLabelUseCatNo = pConfigView->m_bEXPObjLabelUseCatNo;
		m_pMainConfig->m_bEXPObjLabelUseType = pConfigView->m_bEXPObjLabelUseType;
		// :: artificial eart satellites
		m_pMainConfig->m_bAESObjLabelUseCatNo = pConfigView->m_bAESObjLabelUseCatNo;
		m_pMainConfig->m_bAESObjLabelUseType = pConfigView->m_bAESObjLabelUseType;

		//////////////////////////////////
		// SECTION :: GRAPHICS
		if( pConfigView->m_nGraphicsPrevType != -1 ) pConfigView->GetGraphicsType( pConfigView->m_nGraphicsPrevType );
		// :: object path
		m_pMainConfig->m_nGraphicsObjectPathLine = pConfigView->m_nGraphicsObjectPathLine;
		m_pMainConfig->colGraphicsObjectPath = pConfigView->colGraphicsObjectPath;
		m_pMainConfig->m_bGraphicsPathVertex = pConfigView->m_bGraphicsPathVertex;
		m_pMainConfig->m_bGraphicsPathStartEnd = pConfigView->m_bGraphicsPathStartEnd;
		m_pMainConfig->m_bGraphicsPathDirection = pConfigView->m_bGraphicsPathDirection;
		m_pMainConfig->m_bGraphicsPathBaseLines = pConfigView->m_bGraphicsPathBaseLines;
		// :: sky grid
		m_pMainConfig->m_nGraphicsSkyGridLine = pConfigView->m_nGraphicsSkyGridLine;
		m_pMainConfig->colGraphicsSkyGrid = pConfigView->colGraphicsSkyGrid;
		m_pMainConfig->m_bSkyGridShowLabels = pConfigView->m_bSkyGridShowLabels;
		m_pMainConfig->m_bSkyGridDrawSplines = pConfigView->m_bSkyGridDrawSplines;
		m_pMainConfig->m_nSkyGridRaDivValue = pConfigView->m_nSkyGridRaDivValue;
		m_pMainConfig->m_nSkyGridDecDivValue = pConfigView->m_nSkyGridDecDivValue;
		// :: distortion grid
		m_pMainConfig->m_nGraphicsDistortionGridLine = pConfigView->m_nGraphicsDistortionGridLine;
		m_pMainConfig->colGraphicsDistortionGrid = pConfigView->colGraphicsDistortionGrid;
		m_pMainConfig->m_bDistGridDrawSplines = pConfigView->m_bDistGridDrawSplines;
		m_pMainConfig->m_nDistGridRaDivValue = pConfigView->m_nDistGridRaDivValue;
		m_pMainConfig->m_nDistGridDecDivValue = pConfigView->m_nDistGridDecDivValue;
		m_pMainConfig->m_nDistGridBumpFactValue = pConfigView->m_nDistGridBumpFactValue;
		// :: constellations
		m_pMainConfig->m_nGraphicsConstellationLinesLine = pConfigView->m_nGraphicsConstellationLinesLine;
		m_pMainConfig->colGraphicsConstellationLines = pConfigView->colGraphicsConstellationLines;
		m_pMainConfig->m_bConstellationMarkStars = pConfigView->m_bConstellationMarkStars;
		m_pMainConfig->m_bConstellationOutLine = pConfigView->m_bConstellationOutLine;
		m_pMainConfig->m_bConstellationGreekLabels = pConfigView->m_bConstellationGreekLabels;

		///////////////////////////////
		// SECTION :: units format and coordinate system
		nIntVar = pConfigView->m_pSkyCoordDisplayFormat->GetSelection();
		// now if units format has changes
		if( m_pMainConfig->m_nSkyCoordDisplayFormat != nIntVar )
		{
			if( nIntVar == UNITS_FORMAT_SEXAGESIMAL )
			{
				int nWidths[10] =  { -10, 40, 70, 190, 60, 42, 55, 47, 23 };
				GetStatusBar()->SetStatusWidths( 9, nWidths );
			} else if( nIntVar == UNITS_FORMAT_DECIMAL )
			{
				int nWidths[10] =  { -10, 40, 70, 140, 60, 42, 55, 47, 23 };
				GetStatusBar()->SetStatusWidths( 9, nWidths );
			}
		}
		m_pMainConfig->m_nSkyCoordDisplayFormat = nIntVar;
		m_pMainConfig->m_nSkyCoordEntryFormat = pConfigView->m_pSkyCoordEntryFormat->GetSelection();
		m_pMainConfig->m_nCoordSystem = pConfigView->m_pCoordSystem->GetSelection();
		// check/set language
		if( m_pMainConfig->m_nGuiLanguage != pConfigView->m_nGuiLanguage )
		{
			m_pMainConfig->m_nGuiLanguage = pConfigView->m_nGuiLanguage;
			wxMessageBox( wxT("The GUI language has been changed!\n You need to restart the application for it to take effect!"), 
							wxT("Warning"), wxICON_EXCLAMATION );
		}
		// theme
		m_pMainConfig->m_nGuiTheme = pConfigView->m_pGuiTheme->GetSelection();

		// and now call set names again
		if( m_pAstroImage ) m_pAstroImage->SetSky( m_pSky );
		// and also call to reformat toolbar 
		SetStatusRaDec( m_nLastCurRa, m_nLastCurDec );

		/////////////
		// get actions panel
		// save on detect
		nVarInt = pConfigView->m_pSaveOnDetect->GetValue();
		m_pMainConfig->SetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_DETECTION, nVarInt );
		m_pUnimapWorker->m_nActSaveOnDetect = nVarInt;
		// save on match
		nVarInt = pConfigView->m_pSaveOnMatch->GetValue();
		m_pMainConfig->SetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_MATCHING, nVarInt );
		m_pUnimapWorker->m_nActSaveOnMatch = nVarInt;
		// save on details
		nVarInt = pConfigView->m_pSaveOnDetails->GetValue();
		m_pMainConfig->SetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_DETAILS, nVarInt );
		m_pUnimapWorker->m_nActSaveOnDetails = nVarInt;
		// save on autos
		nVarInt = pConfigView->m_pSaveOnAutos->GetValue();
		m_pMainConfig->SetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_AUTOS, nVarInt );
		m_pUnimapWorker->m_nActSaveOnAutos = nVarInt;
		// hide on minimize
		m_pMainConfig->m_bHideOnMinimize = pConfigView->m_pHideOnMinimize->GetValue();

		/////////////
		// get services panel
		int bRestartServices = 0;
		// enable soap services
		nVarInt = pConfigView->m_pEnableSoapServices->GetValue();
		if( m_pMainConfig->SetIntVar( SECTION_ID_SERVICES, CONF_SERVICES_SERVER_RUN, nVarInt ) > 0 ) bRestartServices = 1;

		// check if to restart services
		if( bRestartServices > 0 ) m_pUnimap->RestartServices();

		//////////////
		// call to update styles in config
		m_pMainConfig->UpdateStyles( );
		m_pMainConfig->UpdateGui();

		Refresh();
	}

	delete( pConfigView );

	return;
}

////////////////////////////////////////////////////////////////////
// Purpose:	set the configuration of the detection process
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnConfigDetection( wxCommandEvent& pEvent )
{
	ConfigDetection();
	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::ConfigDetection( )
{
	CDetectConfigView *pConfigView = new CDetectConfigView( this );
	pConfigView->SetConfig( m_pDetectConfig, m_pMainConfig );
	// check if return is ok
	if ( pConfigView->ShowModal( ) == wxID_OK )
	{
		int i=0;
		// get first the section
		int nProfileId = pConfigView->m_pProfileSelect->GetSelection();
		m_pDetectConfig->m_nDetectProfileId = nProfileId;

		// copy all profiles back
		m_pDetectConfig->m_vectProfile.clear();
		for( i=0; i<pConfigView->m_vectProfile.size(); i++ )
		{
			DefConfigDetector src;
			memcpy( &src, &(pConfigView->m_vectProfile[i]), sizeof(DefConfigDetector) );
			m_pDetectConfig->m_vectProfile.push_back( src );
		}
		
		////////////////////////
		// TAB :: MISC
/*		// m_pMemObjStack CONF_MEMORY_OBJSTACK int
		if( pConfigView->m_pMemObjStack->GetLineText(0).ToLong( &nIntVar ) )
			m_pDetectConfig->SetIntVar( nProfileId, CONF_MEMORY_OBJSTACK, (int) nIntVar );
		// m_pMemPixStack CONF_MEMORY_PIXSTACK int
		if( pConfigView->m_pMemPixStack->GetLineText(0).ToLong( &nIntVar ) )
			m_pDetectConfig->SetIntVar( nProfileId, CONF_MEMORY_PIXSTACK, (int) nIntVar );
		// m_pMemBufSize CONF_MEMORY_BUFSIZE int
		if( pConfigView->m_pMemBufSize->GetLineText(0).ToLong( &nIntVar ) )
			m_pDetectConfig->SetIntVar( nProfileId, CONF_MEMORY_BUFSIZE, (int) nIntVar );
		// m_pVerboseType CONF_VERBOSE_TYPE
		m_pDetectConfig->SetIntVar( nProfileId, CONF_VERBOSE_TYPE, (int) pConfigView->m_pVerboseType->GetSelection() );
		// m_pWriteXml CONF_WRITE_XML
		m_pDetectConfig->SetIntVar( nProfileId, CONF_WRITE_XML, (int) pConfigView->m_pWriteXml->GetValue() );
		// m_pXmlName CONF_XML_NAME
		m_pDetectConfig->SetStrVar( nProfileId, CONF_XML_NAME, pConfigView->m_pXmlName->GetLineText(0) );
*/
		// update main frame 
//		SetDetectOptions( );

		// call save config routine
//		pConfigView->StoreConfig( );
	}

	delete( pConfigView );
}

////////////////////////////////////////////////////////////////////
// Purpose:	show the dialog for the matching configuration
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnConfigMatch( wxCommandEvent& pEvent )
{
	CMatchConfig *pMatchConfigView = new CMatchConfig( this, wxT("Matching Configuration") );
	pMatchConfigView->SetConfig( m_pMainConfig );
	// check if return is ok
	if ( pMatchConfigView->ShowModal( ) == wxID_OK )
	{
		long nIntVar = 0;
		unsigned long nUIntVar=0;
		double nFloatVar = 0;

		////////////////////////////////////////////////
		// :: SEARCH
		// get transformation type
		m_pMainConfig->m_nSkySearchMethod = pMatchConfigView->m_pSearchMethod->GetSelection();
		// SearchAreaIncrementOnRA
		if( pMatchConfigView->m_pSearchAreaIncrementOnRA->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pMainConfig->m_nSearchAreaIncrementOnRA = nFloatVar;
		// SearchAreaIncrementOnDEC
		if( pMatchConfigView->m_pSearchAreaIncrementOnDEC->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pMainConfig->m_nSearchAreaIncrementOnDEC = nFloatVar;
		// m_pRaSearchOverlapDivision
		if( pMatchConfigView->m_pRaSearchOverlapDivision->GetLineText(0).ToULong( &nUIntVar ) )
			m_pMainConfig->m_nRaSearchOverlapDivision = (int) nUIntVar;
		// DecSearchOverlapDivision
		if( pMatchConfigView->m_pDecSearchOverlapDivision->GetLineText(0).ToULong( &nUIntVar ) )
			m_pMainConfig->m_nDecSearchOverlapDivision = (int) nUIntVar;
		// SearchZoomSteps
		if( pMatchConfigView->m_pSearchZoomSteps->GetLineText(0).ToULong( &nUIntVar ) )
			m_pMainConfig->m_nSearchZoomSteps = (int) nUIntVar;
		// SearchZoomStart
		if( pMatchConfigView->m_pSearchZoomStart->GetLineText(0).ToULong( &nUIntVar ) )
			m_pMainConfig->m_nSearchZoomStart = (int) nUIntVar;
		// DoReverseZoomSearch
		m_pMainConfig->m_bDoReverseZoomSearch = pMatchConfigView->m_pDoReverseZoomSearch->GetValue();

		////////////////////////////////////////////////
		// :: PROC
		// get projection type
		m_pMainConfig->m_nSkyProjectionMethod = pMatchConfigView->m_pProjectionMethod->GetSelection();
		// DoMatchFineTune
		m_pMainConfig->m_bDoMatchFineTune = (int) pMatchConfigView->m_pDoMatchFineTune->GetValue();
		// DoMatchFineTuneWithProjection
		m_pMainConfig->m_bDoMatchFineTuneWithProjection = (int) pMatchConfigView->m_pDoMatchFineTuneWithProjection->GetValue();
		// MatchByMostStars
		m_pMainConfig->m_bMatchCheckByNoOfStars = pMatchConfigView->m_pMatchByMostStars->GetValue();
		// MatchByBestFit
		m_pMainConfig->m_bMatchCheckByBestFit = pMatchConfigView->m_pMatchByBestFit->GetValue();
		// DoMatchRefineBestArea
		m_pMainConfig->m_bDoMatchRefineBestArea = pMatchConfigView->m_pDoMatchRefineBestArea->GetValue();
		// DoMatchStopOnFirstFound
		m_pMainConfig->m_bMatchStopWhenMinIsFound = pMatchConfigView->m_pDoMatchStopOnFirstFound->GetValue();
		// MatchStopOnMinFound
		if( pMatchConfigView->m_pMatchStopOnMinFound->GetLineText(0).ToULong( &nUIntVar ) )
			m_pMainConfig->m_nMatchMinFoundToStop = (int) nUIntVar;
		// AssocMatchMaxDist
		if( pMatchConfigView->m_pAssocMatchMaxDist->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pMainConfig->m_nAssocMatchMaxDist = nFloatVar;

		////////////////////////////////////////////////
		// :: OBJECTS
		// extract by object shape
		m_pMainConfig->m_nImgObjectExtractByObjShape = pMatchConfigView->m_pImgObjectExtractByObjectShape->GetValue();
		// extract by detection type
		m_pMainConfig->m_nImgObjectExtractByDetectType = pMatchConfigView->m_pImgObjectExtractByDetectionType->GetValue();
		// number of objects in the image to use for scan-match
		if( pMatchConfigView->m_pImgNoObjMatch->GetLineText(0).ToULong( &nUIntVar ) )
			m_pMainConfig->m_nScanMatchImgObjNo = (unsigned int) nUIntVar;
		// number of objects in the catalog to use for scan-match
		if( pMatchConfigView->m_pCatNoObjMatch->GetLineText(0).ToULong( &nUIntVar ) )
			m_pMainConfig->m_nScanMatchCatObjNo = (unsigned int) nUIntVar;
		// SearchMatchMinCatStars
		if( pMatchConfigView->m_pSearchMatchMinCatStars->GetLineText(0).ToULong( &nUIntVar ) )
			m_pMainConfig->m_nSearchMatchMinCatStars = (int) nUIntVar;
		// MatchFirstLevelMinStars
		if( pMatchConfigView->m_pMatchFirstLevelMinStars->GetLineText(0).ToULong( &nUIntVar ) )
			m_pMainConfig->m_nMatchFirstLevelMinStars = (int) nUIntVar;
		// MatchFineTuneImgStarsFactor
		if( pMatchConfigView->m_pMatchFineTuneImgStarsFactor->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pMainConfig->m_nMatchFineTuneImgStarsFactor = nFloatVar;
		// MatchFineTuneCatStarsFactor
		if( pMatchConfigView->m_pMatchFineTuneCatStarsFactor->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pMainConfig->m_nMatchFineTuneCatStarsFactor= nFloatVar;

		////////////////////////////////////////////////
		// :: MATCH 
		// get transformation type
		m_pMainConfig->m_nMatchTransType = pMatchConfigView->m_pTransTypeSelect->GetSelection();
//		m_pStarMatch->SetTransType( nIntVar+1 );
		// do recalculate trans
		m_pMainConfig->m_bMatchDoRecalc = pMatchConfigView->m_pMatchRecalc->GetValue();
		// max matching distance
		if( pMatchConfigView->m_pMaxMatchDist->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pMainConfig->m_nMatchMaxDist = nFloatVar;
		// triangle fit
		if( pMatchConfigView->m_pTriangFit->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pMainConfig->m_nMatchTriangFit = nFloatVar;
		// triangle scale
		if( pMatchConfigView->m_pTriangScale->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pMainConfig->m_nMatchTriangScale = nFloatVar;
		// max iterations
		if( pMatchConfigView->m_pMaxIter->GetLineText(0).ToULong( &nUIntVar ) )
			m_pMainConfig->m_nMatchMaxIter = nUIntVar;
		// halt sigma
		if( pMatchConfigView->m_pHaltSigma->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pMainConfig->m_nMatchHaltSigma = nFloatVar;

		////////////////////////////////////////////////
		// :: MATCH :: FINDER
		m_pMainConfig->m_bMatchLocateCatStars = pMatchConfigView->m_pLocateCatStars->GetValue();
		m_pMainConfig->m_bMatchLocateDso = pMatchConfigView->m_pLocateDso->GetValue();
		m_pMainConfig->m_bMatchLocateXGamma = pMatchConfigView->m_pLocateXGamma->GetValue();
		m_pMainConfig->m_bMatchLocateRadio = pMatchConfigView->m_pLocateRadio->GetValue();
		m_pMainConfig->m_bMatchLocateSupernovas = pMatchConfigView->m_pLocateSupernovas->GetValue();
		m_pMainConfig->m_bMatchLocateBlackholes = pMatchConfigView->m_pLocateBlackholes->GetValue();
		m_pMainConfig->m_bMatchLocateMStars = pMatchConfigView->m_pLocateMStars->GetValue();
		m_pMainConfig->m_bMatchLocateExoplanets = pMatchConfigView->m_pLocateExoplanets->GetValue();
		m_pMainConfig->m_bMatchLocateAsteroids = pMatchConfigView->m_pLocateAsteroids->GetValue();
		m_pMainConfig->m_bMatchLocateComets = pMatchConfigView->m_pLocateComets->GetValue();
		m_pMainConfig->m_bMatchLocateAes = pMatchConfigView->m_pLocateAes->GetValue();
		m_pMainConfig->m_bMatchLocateSolar = pMatchConfigView->m_pLocateSolar->GetValue();
		m_pMainConfig->m_bMatchFetchDetails = pMatchConfigView->m_pFecthDetailsOnFind->GetValue();

		////////////////////////////////////////////////
		// :: MATCH :: SOLAR
		m_pMainConfig->m_nMatchLocateTimeFrameRange = pMatchConfigView->m_pTimeFrameRange->GetValue();
		m_pMainConfig->m_nMatchLocateTimeFrameDivision = pMatchConfigView->m_pTimeFrameDivision->GetValue();
		m_pMainConfig->m_bOnlyTleInRange = pMatchConfigView->m_pOnlyTleInRange->GetValue();
		m_pMainConfig->m_nTleInRangeValue = pMatchConfigView->m_pTleInRangeValue->GetValue();

		// call tu update config
		m_pMainConfig->UpdateMatch( );
	}

	delete( pMatchConfigView );

	return;
}

////////////////////////////////////////////////////////////////////
// Purpose:	dialog request to download a specific catalog locally
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnCatalogDownload( wxCommandEvent& pEvent )
{

	return;
}

////////////////////////////////////////////////////////////////////
// Purpose:	show the dialog for the catalog configuration
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnConfigCatalog( wxCommandEvent& pEvent )
{
	ConfigCatalog();
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::ConfigCatalog( )
{
	CCatalogConfig *pCatConfigView = new CCatalogConfig( this, wxT("Catalog Configuration") );
	pCatConfigView->SetConfig( m_pSky, m_pMainConfig, m_pAstroImage );
	// check if return is ok
	if ( pCatConfigView->ShowModal( ) == wxID_OK )
	{
		long nIntVar = 0;
		double nFloatVar = 0;
		DefPanelCatalogType* pPanelData;

		/////////////////////////////////////
		// :: STARS PANEL
		pPanelData = pCatConfigView->GetPanelByCatalogType( CATALOG_TYPE_STARS );
		// get stars catalog for matching
		m_pMainConfig->m_nCatalogForMatching = pCatConfigView->GetCatIdFromSelection( CATALOG_TYPE_STARS );
		// use only hint area
		m_pMainConfig->m_bStarsCatLoadLocalRegion = pPanelData->pCatLoadRegion->GetValue();
		// magnitude limit
		if( pCatConfigView->m_pStarsCatMaxMagLimit->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pMainConfig->m_nStarsCatMaxMagLimit = nFloatVar;
		// load remote cat
		m_pMainConfig->m_bStarsCatLoadRemote = pPanelData->pCatUseRemote->GetValue();
		// load remote cat location
		m_pMainConfig->m_nStarsCatLoadRemoteLocation = pCatConfigView->GetQuerySourceId( CATALOG_TYPE_STARS );
	
		//////////////////////////////////////////
		// :: DSO PANEL
		pPanelData = pCatConfigView->GetPanelByCatalogType( CATALOG_TYPE_DSO );
		// set what dso catalog to use
		m_pMainConfig->m_nDsoCatalogToUse = pCatConfigView->GetCatIdFromSelection( CATALOG_TYPE_DSO );
		// use only hint area
		m_pMainConfig->m_bDsoCatLoadLocalRegion = pPanelData->pCatLoadRegion->GetValue();
		// load remote cat
		m_pMainConfig->m_bDsoCatLoadRemote = pPanelData->pCatUseRemote->GetValue();
		// load remote cat location
		m_pMainConfig->m_nDsoCatLoadRemoteLocation = pCatConfigView->GetQuerySourceId( CATALOG_TYPE_DSO );
		// use messier
		m_pMainConfig->m_bUseMessier = pCatConfigView->m_pUseMessierCat->GetValue();
		// use ngc catalog
		m_pMainConfig->m_bUseNgc = pCatConfigView->m_pUseNgcCat->GetValue();
		// use IC catalog
		m_pMainConfig->m_bUseIc = pCatConfigView->m_pUseIcCat->GetValue();

		//////////////////////////////////////////
		// :: RADIO PANEL
		pPanelData = pCatConfigView->GetPanelByCatalogType( CATALOG_TYPE_RADIO );
		// set what dso catalog to use
		m_pMainConfig->m_nRadioCatalogToUse = pCatConfigView->GetCatIdFromSelection( CATALOG_TYPE_RADIO );
		// use only hint area
		m_pMainConfig->m_bRadioCatLoadLocalRegion = pPanelData->pCatLoadRegion->GetValue();
		// load remote cat
		m_pMainConfig->m_bRadioCatLoadRemote = pPanelData->pCatUseRemote->GetValue();
		// load remote cat location
		m_pMainConfig->m_nRadioCatLoadRemoteLocation = pCatConfigView->GetQuerySourceId( CATALOG_TYPE_RADIO );

		//////////////////////////////////////////
		// :: XGAMMA PANEL
		pPanelData = pCatConfigView->GetPanelByCatalogType( CATALOG_TYPE_XGAMMA );
		// set what dso catalog to use
		m_pMainConfig->m_nXGammaCatalogToUse = pCatConfigView->GetCatIdFromSelection( CATALOG_TYPE_XGAMMA );
		// use only hint area
		m_pMainConfig->m_bXGammaCatLoadLocalRegion = pPanelData->pCatLoadRegion->GetValue();
		// load remote cat
		m_pMainConfig->m_bXGammaCatLoadRemote = pPanelData->pCatUseRemote->GetValue();
		// load remote cat location
		m_pMainConfig->m_nXGammaCatLoadRemoteLocation = pCatConfigView->GetQuerySourceId( CATALOG_TYPE_XGAMMA );

		//////////////////////////////////////////
		// :: SUPERNOVA PANEL
		pPanelData = pCatConfigView->GetPanelByCatalogType( CATALOG_TYPE_SUPERNOVAS );
		// set what catalog to use
		m_pMainConfig->m_nSNCatalogToUse = pCatConfigView->GetCatIdFromSelection( CATALOG_TYPE_SUPERNOVAS );
		// use only hint area
		m_pMainConfig->m_bSNCatLoadLocalRegion = pPanelData->pCatLoadRegion->GetValue();
		// load remote cat
		m_pMainConfig->m_bSNCatLoadRemote = pPanelData->pCatUseRemote->GetValue();
		// load remote cat location
		m_pMainConfig->m_nSNCatLoadRemoteLocation = pCatConfigView->GetQuerySourceId( CATALOG_TYPE_SUPERNOVAS );

		//////////////////////////////////////////
		// :: BLACKHOLES PANEL
		pPanelData = pCatConfigView->GetPanelByCatalogType( CATALOG_TYPE_BLACKHOLES );
		// set what catalog to use
		m_pMainConfig->m_nBHCatalogToUse = pCatConfigView->GetCatIdFromSelection( CATALOG_TYPE_BLACKHOLES );
		// use only hint area
		m_pMainConfig->m_bBHCatLoadLocalRegion = pPanelData->pCatLoadRegion->GetValue();
		// load remote cat
		m_pMainConfig->m_bBHCatLoadRemote = pPanelData->pCatUseRemote->GetValue();
		// load remote cat location
		m_pMainConfig->m_nBHCatLoadRemoteLocation = pCatConfigView->GetQuerySourceId( CATALOG_TYPE_BLACKHOLES );

		//////////////////////////////////////////
		// :: MULTIPLE/DOUBLE STARS PANEL
		pPanelData = pCatConfigView->GetPanelByCatalogType( CATALOG_TYPE_MSTARS );
		// set what catalog to use
		m_pMainConfig->m_nMSCatalogToUse = pCatConfigView->GetCatIdFromSelection( CATALOG_TYPE_MSTARS );
		// use only hint area
		m_pMainConfig->m_bMSCatLoadLocalRegion = pPanelData->pCatLoadRegion->GetValue();
		// load remote cat
		m_pMainConfig->m_bMSCatLoadRemote = pPanelData->pCatUseRemote->GetValue();
		// load remote cat location
		m_pMainConfig->m_nMSCatLoadRemoteLocation = pCatConfigView->GetQuerySourceId( CATALOG_TYPE_MSTARS );

		//////////////////////////////////////////
		// :: EXTRA-SOLAR PLANETS PANEL
		pPanelData = pCatConfigView->GetPanelByCatalogType( CATALOG_TYPE_EXOPLANETS );
		// set what catalog to use
		m_pMainConfig->m_nEXPCatalogToUse = pCatConfigView->GetCatIdFromSelection( CATALOG_TYPE_EXOPLANETS );
		// use only hint area
		m_pMainConfig->m_bEXPCatLoadLocalRegion = pPanelData->pCatLoadRegion->GetValue();
		// load remote cat
		m_pMainConfig->m_bEXPCatLoadRemote = pPanelData->pCatUseRemote->GetValue();
		// load remote cat location
		m_pMainConfig->m_nEXPCatLoadRemoteLocation = pCatConfigView->GetQuerySourceId( CATALOG_TYPE_EXOPLANETS );

		//////////////////////////////////////////
		// :: ASTEROIDS PANEL
		pPanelData = pCatConfigView->GetPanelByCatalogType( CATALOG_TYPE_ASTEROIDS );
		// set what catalog to use
		m_pMainConfig->m_nASTCatalogToUse = pCatConfigView->GetCatIdFromSelection( CATALOG_TYPE_ASTEROIDS );
		// use only hint area
		m_pMainConfig->m_bASTCatLoadLocalRegion = pPanelData->pCatLoadRegion->GetValue();
		// load remote cat
		m_pMainConfig->m_bASTCatLoadRemote = pPanelData->pCatUseRemote->GetValue();
		// load remote cat location
		m_pMainConfig->m_nASTCatLoadRemoteLocation = pCatConfigView->GetQuerySourceId( CATALOG_TYPE_ASTEROIDS );

		//////////////////////////////////////////
		// :: COMETS PANEL
		pPanelData = pCatConfigView->GetPanelByCatalogType( CATALOG_TYPE_COMETS );
		// set what catalog to use
		m_pMainConfig->m_nCOMCatalogToUse = pCatConfigView->GetCatIdFromSelection( CATALOG_TYPE_COMETS );
		// use only hint area
		m_pMainConfig->m_bCOMCatLoadLocalRegion = pPanelData->pCatLoadRegion->GetValue();
		// load remote cat
		m_pMainConfig->m_bCOMCatLoadRemote = pPanelData->pCatUseRemote->GetValue();
		// load remote cat location
		m_pMainConfig->m_nCOMCatLoadRemoteLocation = pCatConfigView->GetQuerySourceId( CATALOG_TYPE_COMETS );

		//////////////////////////////////////////
		// :: ARTIFICIAL EARTH SATELLITES PANEL
		pPanelData = pCatConfigView->GetPanelByCatalogType( CATALOG_TYPE_AES );
		// set what catalog to use
		m_pMainConfig->m_nAESCatalogToUse = pCatConfigView->GetCatIdFromSelection( CATALOG_TYPE_AES );
		// use only time-frame area 
		m_pMainConfig->m_bAESCatLoadTimeFrame = pPanelData->pCatLoadTimeFrame->GetValue();
		// use only hint area
		m_pMainConfig->m_bAESCatLoadLocalRegion = pPanelData->pCatLoadRegion->GetValue();
		// load remote cat
		m_pMainConfig->m_bAESCatLoadRemote = pPanelData->pCatUseRemote->GetValue();
		// load remote cat location
		m_pMainConfig->m_nAESCatLoadRemoteLocation = pCatConfigView->GetQuerySourceId( CATALOG_TYPE_AES );
		// -- new --
		// load remote update local
		m_pMainConfig->m_bAESCatRemoteUpdateLocal = pPanelData->pCatRemoteUpdateLocal->GetValue();
		// satellite category
		m_pMainConfig->m_nAESCatObjCat = pPanelData->pCatObjCat->GetSelection();
		// object type
		m_pMainConfig->m_nAESCatObjType = pPanelData->pCatObjType->GetSelection();
		// dont use object type
		m_pMainConfig->m_bAESCatDontUseType = pPanelData->pCatDontUseType->GetValue();
		// use start date
		m_pMainConfig->m_bAESCatRecUseDateStart = pPanelData->pCatRecUseDateStart->GetValue();
		// start date
		m_pMainConfig->m_nAESCatRecDateStart = pPanelData->pCatRecDateStart->GetValue().GetJDN();
		// use end date
		m_pMainConfig->m_bAESCatRecUseDateEnd = pPanelData->pCatRecUseDateEnd->GetValue();
		// end date
		m_pMainConfig->m_nAESCatRecDateEnd = pPanelData->pCatRecDateEnd->GetValue().GetJDN();


		/////
		// update config
		m_pMainConfig->UpdateLocalSky( );
	}

	delete( pCatConfigView );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnConfigObserver
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnConfigObserver( wxCommandEvent& pEvent )
{
	CObserver* pObs = m_pUnimap->m_pObserver;
	CObserverCfgDlg* pObsCfgDlg = new CObserverCfgDlg( this );
	pObsCfgDlg->SetConfig( pObs, m_pUnimap->m_pCamera, m_pUnimap->m_pTelescope, m_pUnimapWorker );
	// check if return is ok
	if( pObsCfgDlg->ShowModal( ) == wxID_OK )
	{
		int i=0;

		// PROFILE :: get data 
		pObs->m_strFirstName = pObsCfgDlg->m_pFirstName->GetLineText(0);
		pObs->m_strMiddleName = pObsCfgDlg->m_pMidName->GetLineText(0);
		pObs->m_strLastName = pObsCfgDlg->m_pLastName->GetLineText(0);
		pObs->m_strNickName = pObsCfgDlg->m_pNickName->GetLineText(0);
		pObs->m_strProfession = pObsCfgDlg->m_pProfession->GetLineText(0);
		pObs->m_srtHobbies = pObsCfgDlg->m_pHobbies->GetLineText(0);
		pObs->m_strWebsite = pObsCfgDlg->m_pWebsite->GetLineText(0);
		pObs->m_strPhotoFile = pObsCfgDlg->m_strProfilePhotoFile;
		pObs->m_nBirthday = pObsCfgDlg->m_pBirthday->GetValue().GetJDN();
		// PROFILE :: ADDRESS
		pObs->m_strAddress = pObsCfgDlg->m_pCntAddrStreet->GetLineText(0);
		pObs->m_strZipCode = pObsCfgDlg->m_pCntZipCode->GetLineText(0);
		pObs->m_strCity = pObsCfgDlg->m_pCntCity->GetLabelText();
		pObs->m_strProvince = pObsCfgDlg->m_pCntProvince->GetLabelText();
		pObs->m_strCountry = pObsCfgDlg->m_pCntCountry->GetLabelText();
		// latitude
		wxString strLat = pObsCfgDlg->m_pCntLatitude->GetLineText(0).Trim(0).Trim(1);
		if( !strLat.ToDouble( &(pObs->m_nLatitude) ) ) pObs->m_nLatitude = 0.0;
		// longitude
		wxString strLon = pObsCfgDlg->m_pCntLongitude->GetLineText(0).Trim(0).Trim(1);
		if( !strLon.ToDouble( &(pObs->m_nLongitude) ) ) pObs->m_nLongitude = 0.0;
		// altitude
		wxString strAlt = pObsCfgDlg->m_pCntAltitude->GetLineText(0).Trim(0).Trim(1);
		if( !strAlt.ToDouble( &(pObs->m_nAltitude) ) ) pObs->m_nAltitude = 0.0;
		// PROFILE :: CONTACT
		pObs->m_strMobile = pObsCfgDlg->m_pMobile->GetLineText(0);
		pObs->m_strPhone = pObsCfgDlg->m_pPhone->GetLineText(0);
		pObs->m_strFax = pObsCfgDlg->m_pFax->GetLineText(0);
		pObs->m_strEmail = pObsCfgDlg->m_pEmail->GetLineText(0);
		// messenger addresses
		pObs->m_strMessengerYahoo = pObsCfgDlg->m_vectMessenger[0];
		pObs->m_strMessengerSkype = pObsCfgDlg->m_vectMessenger[1];
		pObs->m_strMessengerGTalk = pObsCfgDlg->m_vectMessenger[2];
		pObs->m_strMessengerMsn = pObsCfgDlg->m_vectMessenger[3];
		pObs->m_strMessengerAol = pObsCfgDlg->m_vectMessenger[4];
		pObs->m_strMessengerIcq = pObsCfgDlg->m_vectMessenger[5];
		// defaults
		pObs->m_nDefaultSite = pObsCfgDlg->m_pDefaultSite->GetSelection();
		pObs->m_nDefaultSetup = pObsCfgDlg->m_pDefaultSetup->GetSelection();
		pObs->m_nDefaultTelescope = pObsCfgDlg->m_pDefaultTelescope->GetSelection();
		pObs->m_nDefaultCamera = pObsCfgDlg->m_pDefaultCamera->GetSelection();
		pObs->m_nDefaultMount = pObsCfgDlg->m_pDefaultMount->GetSelection();
		pObs->m_nDefaultFocuser = pObsCfgDlg->m_pDefaultFocuser->GetSelection();

		////////////////////////
		// SITES
		pObs->m_nSite = pObsCfgDlg->m_nSite;
		// allocate
		if( pObs->m_vectSite == NULL )
			pObs->m_vectSite = (CObserverSite**) calloc( pObs->m_nSite, sizeof(CObserverSite*) );
		else
			pObs->m_vectSite = (CObserverSite**) _recalloc( pObs->m_vectSite, pObs->m_nSite, sizeof(CObserverSite*) );
		// copy all over 
		for( i=0; i<pObsCfgDlg->m_nSite; i++ )
		{
			pObs->m_vectSite[i] = new CObserverSite();

			pObs->m_vectSite[i]->m_strSiteName = pObsCfgDlg->m_vectSite[i]->m_strSiteName;
			pObs->m_vectSite[i]->m_strCity = pObsCfgDlg->m_vectSite[i]->m_strCity;
			pObs->m_vectSite[i]->m_strProvince = pObsCfgDlg->m_vectSite[i]->m_strProvince;
			pObs->m_vectSite[i]->m_strCountry = pObsCfgDlg->m_vectSite[i]->m_strCountry;
			pObs->m_vectSite[i]->m_nLatitude = pObsCfgDlg->m_vectSite[i]->m_nLatitude;
			pObs->m_vectSite[i]->m_nLongitude = pObsCfgDlg->m_vectSite[i]->m_nLongitude;
			pObs->m_vectSite[i]->m_nAltitude = pObsCfgDlg->m_vectSite[i]->m_nAltitude;
			pObs->m_vectSite[i]->m_strSiteNotes = pObsCfgDlg->m_vectSite[i]->m_strSiteNotes;
		}

		////////////////////////
		// HARDWARE
		// use copy instead
		pObsCfgDlg->m_pHardware->CopyTo( pObs->m_pHardware );

		// now save
		pObs->Save( );
	}	
	// delete object
	delete( pObsCfgDlg );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnConfigOnline
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnConfigOnline( wxCommandEvent& pEvent )
{
	int i=0;
	CUnimapOnline* pUnimapOnline = m_pUnimap->m_pUnimapOnline;

	COnlineCfgDlg* pOnlineCfgDlg = new COnlineCfgDlg( this );
	pOnlineCfgDlg->SetConfig( pUnimapOnline );
	// check if return is ok
	if( pOnlineCfgDlg->ShowModal( ) == wxID_OK )
	{
		pUnimapOnline->m_strUsername = pOnlineCfgDlg->m_pUsername->GetLineText(0).Trim(0).Trim(1);
		strcpy( pUnimapOnline->m_strPassword, pOnlineCfgDlg->m_pPassword->GetLineText(0).Trim(0).Trim(1).ToAscii() );
		pUnimapOnline->m_strEmail = pOnlineCfgDlg->m_pEmail->GetLineText(0).Trim(0).Trim(1);
		pUnimapOnline->m_strNickname = pOnlineCfgDlg->m_pNickname->GetLineText(0).Trim(0).Trim(1);
	//	pUnimapOnline->m_nUseSecure = pOnlineCfgDlg->m_pSelectSecure->GetSelection();

		// set the other/services accounts
		for( i=0; i<pUnimapOnline->m_nServiceAccounts; i++ )
		{
			pUnimapOnline->m_vectServiceAccounts[i].username = pOnlineCfgDlg->m_vectServiceUsername[i];
			strcpy( pUnimapOnline->m_vectServiceAccounts[i].password, pOnlineCfgDlg->m_vectServicePassword[i] );
		}

		// now save my new config
		pUnimapOnline->SaveCfg( );
	}
	// delete object
	delete( pOnlineCfgDlg );

	return;
}

// Method:	OnConfigInstruments
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnConfigInstruments( wxCommandEvent& pEvent )
{
	CInstrumentsCfgDlg* pDlg = new CInstrumentsCfgDlg( this );
	pDlg->SetConfig( m_pUnimap->m_pTelescope, m_pUnimap->m_pFocuser, m_pUnimap->m_pCamera );
	// check if return is ok
	if( pDlg->ShowModal( ) == wxID_OK )
	{
		SetInstrumentsSettings( pDlg );
	}
	// delete object
	delete( pDlg );

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetInstrumentsSettings( CInstrumentsCfgDlg* pDlg )
{
	if( !pDlg ) return;

	////////////////// :: TELESCOPE
	// get driver type
	int nInterfaceType = pDlg->m_pTelUseAscom->GetValue( );
	// now by type 1=ascom, 0=drv class
	if( nInterfaceType == 1 )
	{
		m_pUnimap->m_pTelescope->m_bUseAscom = 1;
		m_pUnimap->m_pTelescope->m_strAscomDriverName = pDlg->m_pTelAscomDriverName->GetLineText(0).Trim(0).Trim(1);

	} else
	{
		m_pUnimap->m_pTelescope->m_bUseAscom = 0;
		m_pUnimap->m_pTelescope->m_nTelescopeDrvType = pDlg->m_pTelescopeSelect->GetSelection();
		m_pUnimap->m_pTelescope->m_nSerialPortNo = pDlg->m_pTelSerialPortSelect->GetSelection()+1;
	}

	///////////////// :: FOCUSER
	nInterfaceType = pDlg->m_nUseFocuser;
	m_pUnimap->m_pFocuser->m_nFocuserInterface = nInterfaceType;
	// now by type set values
	if( nInterfaceType == 1 )
	{
		m_pUnimap->m_pFocuser->m_strAscomDriverName = pDlg->m_pFocAscomDriverName->GetLineText(0).Trim(0).Trim(1);

	} else if( nInterfaceType == 2 )
	{
		m_pUnimap->m_pFocuser->m_nFocuserDrvType = pDlg->m_pFocuserSelect->GetSelection();
		m_pUnimap->m_pFocuser->m_nSerialPortNo = pDlg->m_pFocSerialPortSelect->GetSelection()+1;
	}

	///////////////// :: CAMERA
	m_pUnimap->m_pCamera->m_vectHWDevices.clear();
	for( int i=0; i<pDlg->m_vectCamDevices.size(); i++ ) 
		m_pUnimap->m_pCamera->m_vectHWDevices.push_back( pDlg->m_vectCamDevices[i] );

}

////////////////////////////////////////////////////////////////////
// Method:		OnGroupDetect
// Class:		CGUIFrame
// Purpose:		do whole group object detection
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnGroupDetect( wxCommandEvent& pEvent )
{
	DefCmdAction act;
	act.id = THREAD_ACTION_GROUP_DETECT;
	act.vectObj[0] = (void*) m_pImageDb->m_pCurrentGroup;
	act.vectObj[1] = (void*) m_pDetectConfig;
	act.vectObj[2] = (void*) m_pMainConfig;

//	m_pUnimapWorker->SetAction( cmd );

	/////////////////
	// show wait dialog
	CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Group Detection Process"), 
						wxT("Detecting objects in the image..."), 0, m_pUnimapWorker, &act );
	pWaitDialog->ShowModal();
	// delete/reset wait dialog
	delete( pWaitDialog );
	pWaitDialog = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnGroupMatch
// Class:		CGUIFrame
// Purpose:		do whole group object matching
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnGroupMatch( wxCommandEvent& pEvent )
{
	DefCmdAction cmd;
	cmd.id = THREAD_ACTION_GROUP_MATCH;

//	m_pUnimapWorker->SetAction( cmd );

	// build catalog name
	CatalogDef* pCat = &(m_pSky->m_vectCatalogDef[m_pMainConfig->m_nCatalogForMatching-1]);
	wxString strCatName = pCat->m_strFullName;
	if( strCatName.IsEmpty() ) strCatName = pCat->m_strName;
	if( strCatName.IsEmpty() ) strCatName = pCat->m_strNameAbbreviation;
	if( strCatName.IsEmpty() ) strCatName = pCat->m_strLabel;
	if( strCatName.IsEmpty() ) strCatName.Printf( wxT("id=%d"), m_pMainConfig->m_nCatalogForMatching );

	wxString strFullTitle = wxT("Group Matching Process");
	wxString strMsg = wxT("Initializing matching engine...");
	strFullTitle += wxT( " :: " ) + m_pImageDb->m_pCurrentGroup->m_strGroupNameId;
	CWaitDialogMatch* pWaitDialogMatch = new CWaitDialogMatch( this, strFullTitle, strMsg, 
									m_pUnimapWorker, (void*) m_pImageDb->m_pCurrentGroup, 1 );
	pWaitDialogMatch->SetCatalog( strCatName );

//	m_pWaitDialog->Show( 1 );
	pWaitDialogMatch->ShowModal();
	delete( pWaitDialogMatch );
	
	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnGroupRegister
// Class:		CGUIFrame
// Purpose:		register images in the group
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnGroupRegister( wxCommandEvent& pEvent )
{
	DefCmdAction act;
	act.id = THREAD_ACTION_GROUP_REGISTER;
	act.vectObj[0] = (void*) m_pImageDb->m_pCurrentGroup;

//	m_pUnimapWorker->SetAction( cmd );

	/////////////////
	// show wait dialog
	CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Group Registration Process"), 
						wxT("Register stack images..."), 0, m_pUnimapWorker, &act );
	pWaitDialog->ShowModal();
	// delete/reset wait dialog
	delete( pWaitDialog );
	pWaitDialog = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnGroupAlign
// Class:		CGUIFrame
// Purpose:		align images in the group - apply registration
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnGroupAlign( wxCommandEvent& pEvent )
{
	DefCmdAction act;
	act.id = THREAD_ACTION_GROUP_ALIGN;
	act.vectObj[0] = (void*) m_pImageDb->m_pCurrentGroup;

//	m_pUnimapWorker->SetAction( cmd );

	/////////////////
	// show wait dialog
	CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Group Alignment Process"), 
						wxT("Align stack of images..."), 0, m_pUnimapWorker, &act );
	pWaitDialog->ShowModal();
	// delete/reset wait dialog
	delete( pWaitDialog );
	pWaitDialog = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnGroupDynamicAnalysis
// Class:		CGUIFrame
// Purpose:		do dynamic analysis for all images in the group stack
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnGroupDynamicAnalysis( wxCommandEvent& pEvent )
{
	DefCmdAction act;
	act.id = THREAD_ACTION_GROUP_DYNAMICS_SEARCH;
	act.vectObj[0] = (void*) m_pImageDb->m_pCurrentGroup;

//	m_pUnimapWorker->SetAction( cmd );

	/////////////////
	// show wait dialog
	CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Group Dynamics Analysis"), 
						wxT("Initialize Dynamic Objects Search..."), 0, m_pUnimapWorker, &act );
	pWaitDialog->ShowModal();
	// delete/reset wait dialog
	delete( pWaitDialog );
	pWaitDialog = NULL;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnGroupStack
// Class:		CGUIFrame
// Purpose:		stack images in the group
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnGroupStack( wxCommandEvent& pEvent )
{
//	DefCmdAction cmd;
//	cmd.id = THREAD_ACTION_GROUP_STACK;
//	m_pUnimapWorker->SetAction( cmd );

	wxString strFullTitle = wxT("Stacking Process");
	wxString strMsg = wxT("Initializing stacking engine...");
	strFullTitle += wxT( " :: " ) + m_pImageDb->m_pCurrentGroup->m_strGroupNameId;
	CWaitDialogStack* pWaitDialogStack = new CWaitDialogStack( this, strFullTitle, strMsg, 
							m_pUnimapWorker, (void*) m_pImageDb->m_pCurrentGroup );
//	m_pWaitDialog->Show( 1 );
	pWaitDialogStack->ShowModal();
	delete(pWaitDialogStack);

	/////////////////////////
	// check if to add new stack frame to the list ?
	if( m_pImageDb->m_pCurrentGroup->m_bNewStackOutput )
	{
		// re-display current group
		int nSelectGroup = m_pGroupSelect->GetSelection();
		m_pImageDb->SelectGroup( nSelectGroup );

		// select last image
		int nStackImage = m_pImageSelect->GetCount()-1;
		m_pImageSelect->SetSelection( nStackImage );
		LoadSelectedImage( );

// from image db
//	m_pFrame->m_pGroupSelect->SetSelection( nGroupId );
//	SelectGroup( nGroupId );
//	m_pFrame->m_pImageSelect->SetSelection( nFirstImageId+1 );
//	SelectImageFromGroup( nGroupId, nFirstImageId );

	}

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnGroupStackWith( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	wxString strArg = wxT("");

	if( nId == wxID_MENU_GROUP_STACK_REGISTAX )
	{
		m_pUnimap->m_pAppsMgr->RunApp( MGR_APPS_NAMEID_REGISTAX, strArg );

	} else if( nId == wxID_MENU_GROUP_STACK_DEEPSKYSTACKER )
	{
		m_pUnimap->m_pAppsMgr->RunApp( MGR_APPS_NAMEID_DEEPSKYSTACKER, strArg );
	}
}

////////////////////////////////////////////////////////////////////
// Method:	OnProcessImageOperator
// Class:	CGUIFrame
// Purpose:	on process image operator
// Input:	reference to command event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnProcessImageOperator( wxCommandEvent& pEvent )
{
	if( !m_pImgView->m_bHasImage ) return;
	if( !m_pImgView->m_pImage->IsOk() ) return;

	int nId = pEvent.GetId();
	ImageVariant rElxImage = m_pImgView->GetElxImage( );

	// get abstract image info
/*	const uint w = m_pImgView->m_pImage->GetWidth();
	const uint h = m_pImgView->m_pImage->GetHeight();
    const uint size = w*h*3;
	// create rgb image
	boost::shared_ptr<ImageRGBub> spImage( new ImageRGBub(w, h) );
    ubyte* prDst = (ubyte*) spImage->GetPixel(); 
    ubyte* prSrc = (ubyte*) m_pImgView->m_pImage.GetData();

	/////////
    // copy from wxImage to ImageVariant
    ::memcpy( (void*) prDst, (const void*) prSrc, size );
	// set back image
	rElxImage = *spImage;
*/

	int m_bHasChanged = 1;
	////////////////////////////////////////
	// HERE we call process operators finally
	if( nId == wxID_MENU_IMAGE_PROCESS_OP_NEG || wxID_BUTTON_IMGINV )
		rElxImage.Operator( IOP_Neg, 100, CM_All );
	else if( nId == wxID_MENU_IMAGE_PROCESS_OP_ADD )
		rElxImage.Operator( IOP_Add, 0.12, CM_All );
	else if( nId == wxID_MENU_IMAGE_PROCESS_OP_SUB )
		rElxImage.Operator( IOP_Sub, 0.05, CM_All );
	else if( nId == wxID_MENU_IMAGE_PROCESS_OP_MUL )
		rElxImage.Operator( IOP_Mul, 2.1, CM_All );
	else
		m_bHasChanged = 0;

	// update image - check if there was a change
	if( m_bHasChanged ) m_pImgView->SetElxImage( &rElxImage );

/*
	/////
	// debug
	//rElxImage.Save( "./operator.bmp" );

	spImage = elxCreateImageRGBub( *rElxImage.GetImpl() );
	prDst = (ubyte*) spImage->GetPixel(); 

	////////
	// copy from ImageVariant to wxImage
    ::memcpy( prSrc, (void*) prDst, size );
	// set back image
//	rElxImage = *spImage;


	m_pImgView->UpdateImage( );
*/
	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnProcessImageColors
// Class:	CGUIFrame
// Purpose:	on process image colors
// Input:	reference to command event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnProcessImageColors( wxCommandEvent& pEvent )
{
	if( !m_pImgView->m_bHasImage ) return;
	if( !m_pImgView->m_pImage->IsOk() ) return;

	int nId = pEvent.GetId();
	////////////////////////////////////////
	// HERE we call process colors
	// :: Brightness/Contrast/Gamma
	int nProcColType = -1;
	if( nId == wxID_MENU_IMAGE_PROCESS_COL_BCG )
		nProcColType = IMAGE_PROCESS_TYPE_BCG;
	else if( nId == wxID_MENU_IMAGE_PROCESS_COL_MIDTONE )
		nProcColType = IMAGE_PROCESS_TYPE_MIDTONE;
	else if( nId == wxID_MENU_IMAGE_PROCESS_COL_BALANCE )
		nProcColType = IMAGE_PROCESS_TYPE_BALANCE;
	else if( nId == wxID_MENU_IMAGE_PROCESS_COL_COLORIZE )
		nProcColType = IMAGE_PROCESS_TYPE_COLORIZE;
	else if( nId == wxID_MENU_IMAGE_PROCESS_COL_DESATURATE )
		nProcColType = IMAGE_PROCESS_TYPE_DESATURATE;
	else if( nId == wxID_MENU_IMAGE_PROCESS_COL_HUESATURATION )
		nProcColType = IMAGE_PROCESS_TYPE_HUESATURATION;

	////////////////////////////
	// COLORS :: PROC
	if( nProcColType >= 0 )
	{
		CProcessColorsDlg fliterDlg( this, nProcColType, 1 );
		fliterDlg.SetImage( *(m_pImgView->m_pImage), m_pImgView );

		if( fliterDlg.ShowModal() == wxID_OK )
		{
			// process with image here - i use bitmap
			if( fliterDlg.m_bHasChanged ) fliterDlg.ProcessImage( *(m_pImgView->m_pImage) );
		}
		// if there was a change - even on cancel - do copy back
		//if( fliterDlg.m_bHasChanged ) m_pImgView->SetElxImage( &(fliterDlg.m_pElxImage) );
	}
}

////////////////////////////////////////////////////////////////////
// Method:	OnProcessImageBlur
// Class:	CGUIFrame
// Purpose:	on image blur
// Input:	reference to command event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnProcessImageBlur( wxCommandEvent& pEvent )
{
	if( !m_pImgView->m_bHasImage ) return;
	if( !m_pImgView->m_pImage->IsOk() ) return;

	int nId = pEvent.GetId();

	int nProcBlurType = -1;
	////////////////////////////////////////
	// HERE we call process :: filters :: blur
	if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_SMOOTH )
		nProcBlurType = IMAGE_PROCESS_TYPE_BLUR_SMOOTH;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_MEAN )
		nProcBlurType = IMAGE_PROCESS_TYPE_BLUR_MEAN;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_LOWPASS )
		nProcBlurType = IMAGE_PROCESS_TYPE_BLUR_LOWPASS;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_GAUSSIAN )
		nProcBlurType = IMAGE_PROCESS_TYPE_BLUR_GAUSSIAN;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_BILATERAL )
		nProcBlurType = IMAGE_PROCESS_TYPE_BLUR_BILATERAL;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_SELECTIVE )
		nProcBlurType = IMAGE_PROCESS_TYPE_BLUR_SELECTIVE;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_BLUR_FFTLOW )
		nProcBlurType = IMAGE_PROCESS_TYPE_BLUR_FFTLOW;

	////////////////////////////
	// BLUR :: PROC
	if( nProcBlurType >= 0 )
	{
		CProcessBlurDlg fliterDlg( this, nProcBlurType, 1 );
		fliterDlg.SetImage( *(m_pImgView->m_pImage), m_pImgView );

		if( fliterDlg.ShowModal() == wxID_OK )
		{
			// process with image here - i use bitmap
			if( fliterDlg.m_bHasChanged ) fliterDlg.ProcessImage( *(m_pImgView->m_pImage) );
		}
		// if there was a change - even on cancel - do copy back
		//if( fliterDlg.m_bHasChanged ) m_pImgView->SetElxImage( &(fliterDlg.m_pElxImage) );
	}

}

////////////////////////////////////////////////////////////////////
// Method:	OnProcessImageSharp
// Class:	CGUIFrame
// Purpose:	on image sharp
// Input:	reference to command event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnProcessImageSharp( wxCommandEvent& pEvent )
{
	if( !m_pImgView->m_bHasImage ) return;
	if( !m_pImgView->m_pImage->IsOk() ) return;

	int nId = pEvent.GetId();

	int nProcSharpType = -1;
	////////////////////////////////////////
	// HERE we call process :: filters :: sharp
	if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_SHARPEN )
		nProcSharpType = IMAGE_PROCESS_TYPE_SHARP_SHARPEN;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_UNSHARP )
		nProcSharpType = IMAGE_PROCESS_TYPE_SHARP_UNSHARP;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_UNSHARPMASK )
		nProcSharpType = IMAGE_PROCESS_TYPE_SHARP_UNSHARPMASK;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_LAPGAUS )
		nProcSharpType = IMAGE_PROCESS_TYPE_SHARP_LAPGAUS;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_SHARP_EMBOSS )
		nProcSharpType = IMAGE_PROCESS_TYPE_SHARP_EMBOSS;

	////////////////////////////
	// SHARP :: PROC
	if( nProcSharpType >= 0 )
	{
		CProcessSharpDlg fliterDlg( this, nProcSharpType, 1 );
		fliterDlg.SetImage( *(m_pImgView->m_pImage), m_pImgView );

		if( fliterDlg.ShowModal() == wxID_OK )
		{
			// process with image here - i use bitmap
			if( fliterDlg.m_bHasChanged ) fliterDlg.ProcessImage( *(m_pImgView->m_pImage) );
		}
		// if there was a change - even on cancel - do copy back
		//if( fliterDlg.m_bHasChanged ) m_pImgView->SetElxImage( &(fliterDlg.m_pElxImage) );
	}

}

////////////////////////////////////////////////////////////////////
// Method:	OnProcessImageEdge
// Class:	CGUIFrame
// Purpose:	on image edge
// Input:	reference to command event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnProcessImageEdge( wxCommandEvent& pEvent )
{
	if( !m_pImgView->m_bHasImage ) return;
	if( !m_pImgView->m_pImage->IsOk() ) return;

	int nId = pEvent.GetId();

	int nProcEdgeType = -1;
	////////////////////////////////////////
	// HERE we call process :: filters :: edge
	if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_GRADIENT )
		nProcEdgeType = IMAGE_PROCESS_TYPE_EDGE_GRADIENT;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_ROBERSCROSS )
		nProcEdgeType = IMAGE_PROCESS_TYPE_EDGE_ROBERSCROSS;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_LAPLACIAN )
		nProcEdgeType = IMAGE_PROCESS_TYPE_EDGE_LAPLACIAN;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_MARRHILDRETH )
		nProcEdgeType = IMAGE_PROCESS_TYPE_EDGE_MARRHILDRETH;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_EDGE_CANNY )
		nProcEdgeType = IMAGE_PROCESS_TYPE_EDGE_CANNY;

	////////////////////////////
	// EDGE :: PROC
	if( nProcEdgeType >= 0 )
	{
		CProcessEdgeDlg fliterDlg( this, nProcEdgeType, 1 );
		fliterDlg.SetImage( *(m_pImgView->m_pImage), m_pImgView );

		if( fliterDlg.ShowModal() == wxID_OK )
		{
			// process with image here - i use bitmap
			if( fliterDlg.m_bHasChanged ) fliterDlg.ProcessImage( *(m_pImgView->m_pImage) );
		}
		// if there was a change - even on cancel - do copy back
		//if( fliterDlg.m_bHasChanged ) m_pImgView->SetElxImage( &(fliterDlg.m_pElxImage) );
	}

}

////////////////////////////////////////////////////////////////////
// Method:	OnProcessImageGradient
// Class:	CGUIFrame
// Purpose:	other image gradient
// Input:	reference to command event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnProcessImageGradient( wxCommandEvent& pEvent )
{
	if( !m_pImgView->m_bHasImage ) return;
	if( !m_pImgView->m_pImage->IsOk() ) return;

	int nId = pEvent.GetId();

	int nProcGradientType = -1;
	////////////////////////////////////////
	// HERE we call process :: filters :: gradient
	if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_GRADIENT_REMOVE )
		nProcGradientType = IMAGE_PROCESS_TYPE_GRADIENT_REMOVE;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_GRADIENT_ROTATIONAL )
		nProcGradientType = IMAGE_PROCESS_TYPE_GRADIENT_ROTATIONAL;

	////////////////////////////
	// GRADIENT :: PROC
	if( nProcGradientType >= 0 )
	{
		CProcessGradientDlg fliterDlg( this, nProcGradientType, 1 );
		fliterDlg.SetImage( *(m_pImgView->m_pImage), m_pImgView );

		if( fliterDlg.ShowModal() == wxID_OK )
		{
			// process with image here - i use bitmap
			if( fliterDlg.m_bHasChanged ) fliterDlg.ProcessImage( *(m_pImgView->m_pImage) );
		}
		// if there was a change - even on cancel - do copy back
		//if( fliterDlg.m_bHasChanged ) m_pImgView->SetElxImage( &(fliterDlg.m_pElxImage) );
	}

}

////////////////////////////////////////////////////////////////////
// Method:	OnProcessImageDeconv
// Class:	CGUIFrame
// Purpose:	image deconvolution methods
// Input:	reference to command event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnProcessImageDeconv( wxCommandEvent& pEvent )
{
	if( !m_pImgView->m_bHasImage ) return;
	if( !m_pImgView->m_pImage->IsOk() ) return;

	int nId = pEvent.GetId();

	int nProcDeconvType = -1;
	////////////////////////////////////////
	// HERE we call process :: filters :: deconvolution
	if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_DECONV_RLUCY )
		nProcDeconvType = IMAGE_PROCESS_TYPE_DECONV_RLUCY;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_DECONV_WIENER )
		nProcDeconvType = IMAGE_PROCESS_TYPE_DECONV_WIENER;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_DECONV_DCTEM )
		nProcDeconvType = IMAGE_PROCESS_TYPE_DECONV_DCTEM;

	////////////////////////////
	// DECONVOLUTION :: PROC
	if( nProcDeconvType >= 0 )
	{
		CProcessDeconvolutionDlg fliterDlg( this, nProcDeconvType, 1 );
		fliterDlg.SetImage( *(m_pImgView->m_pImage), m_pImgView );

		if( fliterDlg.ShowModal() == wxID_OK )
		{
			// process with image here - i use bitmap
			if( fliterDlg.m_bHasChanged ) fliterDlg.ProcessImage( *(m_pImgView->m_pImage) );
		}
		// if there was a change - even on cancel - do copy back
		//if( fliterDlg.m_bHasChanged ) m_pImgView->SetElxImage( &(fliterDlg.m_pElxImage) );
	}
}

////////////////////////////////////////////////////////////////////
// Method:	OnProcessImageFilter
// Class:	CGUIFrame
// Purpose:	other image filters
// Input:	reference to command event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnProcessImageFilter( wxCommandEvent& pEvent )
{
	if( !m_pImgView->m_bHasImage ) return;
	if( !m_pImgView->m_pImage->IsOk() ) return;

	int nId = pEvent.GetId();

	int nProcFilterType = -1;
	////////////////////////////////////////
	// HERE we call process :: filters :: single options
	if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_MORPHOLOGICAL )
		nProcFilterType = IMAGE_PROCESS_TYPE_FILTER_MORPHOLOGICAL;
	else if( nId == wxID_MENU_IMAGE_PROCESS_FILTER_DIGITAL )
		nProcFilterType = IMAGE_PROCESS_TYPE_FILTER_DIGITAL;

	////////////////////////////////////////
	// HERE we call process :: single options
	if( nId == wxID_MENU_IMAGE_PROCESS_DENOISING )
		nProcFilterType = IMAGE_PROCESS_TYPE_PROCESS_DENOISING;
	else if( nId == wxID_MENU_IMAGE_PROCESS_DEBLOOMER )
		nProcFilterType = IMAGE_PROCESS_TYPE_PROCESS_DEBLOOMER;

	////////////////////////////
	// FILTER :: PROC
	if( nProcFilterType >= 0 )
	{
		CProcessFiltersDlg fliterDlg( this, nProcFilterType, 1 );
		fliterDlg.SetImage( *(m_pImgView->m_pImage), m_pImgView );

		if( fliterDlg.ShowModal() == wxID_OK )
		{
			// process with image here - i use bitmap
			if( fliterDlg.m_bHasChanged ) fliterDlg.ProcessImage( *(m_pImgView->m_pImage) );
		}
		// if there was a change - even on cancel - do copy back
		//if( fliterDlg.m_bHasChanged ) m_pImgView->SetElxImage( &(fliterDlg.m_pElxImage) );
	}
}

////////////////////////////////////////////////////////////////////
// Image open with thrid party application
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnImageOpenWith( wxCommandEvent& pEvent )
{
	if( !m_pImgView->m_bHasImage ) return;
	if( !m_pImgView->m_pImage->IsOk() ) return;

	int nId = pEvent.GetId();
	wxString strImgFile = m_pAstroImage->GetFullName();

	if( nId == wxID_MENU_IMAGE_PROCESS_PHOTOSHOP )
	{
		m_pUnimap->m_pAppsMgr->RunApp( MGR_APPS_NAMEID_PHOTOSHOP, strImgFile );

	} else if( nId == wxID_MENU_IMAGE_PROCESS_GIMP )
	{
		m_pUnimap->m_pAppsMgr->RunApp( MGR_APPS_NAMEID_GIMP, strImgFile );
	}
}

////////////////////////////////////////////////////////////////////
// Method:	OnImageNotes
// Class:	CGUIFrame
// Purpose:	open image notes dialog
// Input:	reference to command event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnImageNotes( wxCommandEvent& pEvent )
{
	if( !m_pAstroImage ) return;

	CImageNotesDlg* pDlg = new CImageNotesDlg( this, _("Add Image Note") );
	// set local note
	CImageNote* pNote = new CImageNote( m_pAstroImage );
	pDlg->SetConfig( pNote );

	// display dialog
	if( pDlg->ShowModal() == wxID_OK )
	{
		// add new note to the image notes or ?
		pNote->m_nType = pDlg->m_pNoteType->GetSelection();
		pNote->m_strTitle = pDlg->m_pNoteTitle->GetLineText(0);
		pNote->m_strMessage = pDlg->m_pNoteBody->GetValue();
		pNote->m_rDateTime.SetToCurrent();
		// set positions
		pNote->x = m_pImgView->m_nPicRightClickPosX;
		pNote->y = m_pImgView->m_nPicRightClickPosY;

		// add note to image
		m_pAstroImage->AddNote( pNote );
	}
	// release data
	delete( pDlg );
	delete( pNote );
}

////////////////////////////////////////////////////////////////////
// Method:	OnImageProperty
// Class:	CGUIFrame
// Purpose:	open image properties dialog
// Input:	reference to command event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnImageProperty( wxCommandEvent& pEvent )
{
	if( !m_pImgView->m_bHasImage ) return;
	long nVarLong=0;
	char chL=0;

	CImageProperty* pImgProperty = new CImageProperty( this, _("Image Properties"), m_pAstroImage );
	// set configuration from image
	pImgProperty->SetConfig( m_pUnimapWorker, m_pUnimap->m_pObserver, m_pMainConfig->m_nSkyCoordDisplayFormat );
	pImgProperty->SetHistogram( *(m_pImgView->m_pImage) );
	// display dialog
	if( pImgProperty->ShowModal() == wxID_OK )
	{
		// set observer info
		m_pAstroImage->m_strObsName = pImgProperty->m_pObsName->GetLineText(0).Trim();
		// location :: country/region/city
		if( pImgProperty->m_pEarth )
		{
			m_pAstroImage->m_strObsCountry = pImgProperty->m_pObsCountry->GetLabelText();
			m_pAstroImage->m_strObsRegion = pImgProperty->m_pObsRegion->GetLabelText();
			m_pAstroImage->m_strObsCity = pImgProperty->m_pObsCity->GetLabelText();
		}
		/////////////////////
		// position on eart
		// :: latitude
		wxString strLat = pImgProperty->m_pObsLatitude->GetLineText(0).Trim(0).Trim(1);
/*		chL = strLat.Last();
		if( chL == 'E'  || chL == 'e' || chL == 'W' || chL == 'w' )
		{
			m_pAstroImage->m_chObsLatitude = chL;
			strLat.RemoveLast();
		}
*/		if( !strLat.ToDouble( &(m_pAstroImage->m_nObsLatitude) ) ) m_pAstroImage->m_nObsLatitude=0;
		// :: longitude
		wxString strLon = pImgProperty->m_pObsLongitude->GetLineText(0).Trim(0).Trim(1);
/*		chL = strLon.Last();
		if( chL == 'N' || chL == 'n' || chL == 'S' || chL == 's' )
		{
			m_pAstroImage->m_chObsLongitude = chL;
			strLon.RemoveLast();
		}
*/		if( !strLon.ToDouble( &(m_pAstroImage->m_nObsLongitude) ) ) m_pAstroImage->m_nObsLongitude=0;
		// :: altitude
		if( !pImgProperty->m_pObsAltitude->GetLineText(0).Trim().ToDouble( &(m_pAstroImage->m_nObsAltitude) ) )
			m_pAstroImage->m_nObsAltitude = 0;

		//////////////////
		// set date/time - WITH TIME ZONE
		m_pAstroImage->m_nObsDateTime = pImgProperty->m_pObsDate->GetValue();
		m_pAstroImage->m_nObsDateTime.ParseTime( pImgProperty->m_pObsTime->GetLineText(0).Trim() );
		// calculate/set time zone
		int nTZShift = (int) round(m_pAstroImage->m_nObsLongitude/15.0);
		m_pAstroImage->m_nObsTZSecs = (long) nTZShift * 3600;
		// set time-zone to my current object
//		m_pAstroImage->m_nObsDateTime.MakeTimezone( nTZSecs );

		// set target
		m_pAstroImage->m_strTargetName = pImgProperty->m_pTargetName->GetLineText(0).Trim();
		m_pAstroImage->m_strTargetDesc = pImgProperty->m_pTargetDesc->GetLineText(0).Trim();
		// process target ra
		wxString strRa = pImgProperty->m_pTargetRa->GetLineText(0).Trim();
		if( m_pMainConfig->m_nSkyCoordDisplayFormat == UNITS_FORMAT_SEXAGESIMAL )
		{
			if( !ConvertRaToDeg( strRa, m_pAstroImage->m_nTargetRa  ) )
				m_pAstroImage->m_nTargetRa = 0;

		} else if( m_pMainConfig->m_nSkyCoordDisplayFormat == UNITS_FORMAT_DECIMAL )
		{
			if( !strRa.ToDouble( &(m_pAstroImage->m_nTargetRa) ) )
				m_pAstroImage->m_nTargetRa = 0;
		}
		// process target dec
		wxString strDec = pImgProperty->m_pTargetDec->GetLineText(0).Trim();
		if( m_pMainConfig->m_nSkyCoordDisplayFormat == UNITS_FORMAT_SEXAGESIMAL )
		{
			if( !ConvertDecToDeg( strDec, m_pAstroImage->m_nTargetDec  ) )
				m_pAstroImage->m_nTargetDec = 0;

		} else if( m_pMainConfig->m_nSkyCoordDisplayFormat == UNITS_FORMAT_DECIMAL )
		{
			if( !strDec.ToDouble( &(m_pAstroImage->m_nTargetDec) ) )
				m_pAstroImage->m_nTargetDec = 0;
		}
		// equipment
		m_pAstroImage->m_strTelescopeName = pImgProperty->m_pTelescopeName->GetLineText(0).Trim();
		m_pAstroImage->m_nTelescopeType = pImgProperty->m_pTelescopeType->GetSelection();
		if( !pImgProperty->m_pTelescopeFocal->GetLineText(0).Trim().ToDouble( &(m_pAstroImage->m_nTelescopeFocal) ) )
			m_pAstroImage->m_nTelescopeFocal = 0;
		if( !pImgProperty->m_pTelescopeAperture->GetLineText(0).Trim().ToDouble( &(m_pAstroImage->m_nTelescopeAperture) ) )
			m_pAstroImage->m_nTelescopeAperture = 0;
		m_pAstroImage->m_nTelescopeMount = pImgProperty->m_pTelescopeMount->GetSelection();
		m_pAstroImage->m_strCameraName = pImgProperty->m_pCameraName->GetLineText(0).Trim();
		// conditions
		m_pAstroImage->m_nCondSeeing = pImgProperty->m_pCondSeeing->GetSelection();
		m_pAstroImage->m_nCondTransp = pImgProperty->m_pCondTransp->GetSelection();
		if( !pImgProperty->m_pCondWindSpeed->GetLineText(0).Trim().ToDouble( &(m_pAstroImage->m_nCondWindSpeed) ) )
			m_pAstroImage->m_nCondWindSpeed = 0;
		if( !pImgProperty->m_pCondAtmPressure->GetLineText(0).Trim().ToDouble( &(m_pAstroImage->m_nCondAtmPressure) ) )
			m_pAstroImage->m_nCondAtmPressure = 0;
		if( !pImgProperty->m_pCondTemp->GetLineText(0).Trim().ToDouble( &(m_pAstroImage->m_nCondTemp) ) )
			m_pAstroImage->m_nCondTemp = 0;
		if( !pImgProperty->m_pCondTempVar->GetLineText(0).Trim().ToDouble( &(m_pAstroImage->m_nCondTempVar) ) )
			m_pAstroImage->m_nCondTempVar = 0;
		if( !pImgProperty->m_pCondHumid->GetLineText(0).Trim().ToDouble( &(m_pAstroImage->m_nCondHumid) ) )
			m_pAstroImage->m_nCondHumid = 0;
		m_pAstroImage->m_nCondLp = pImgProperty->m_pCondLp->GetSelection();
		// imaging
		if( !pImgProperty->m_pExpTotalTime->GetLineText(0).Trim().ToDouble( &(m_pAstroImage->m_nExpTotalTime) ) )
			m_pAstroImage->m_nExpTotalTime = 0;
		if( !pImgProperty->m_pNoOfExp->GetLineText(0).ToLong( &nVarLong ) )
			m_pAstroImage->m_nNoOfExp = 0;
		else
			m_pAstroImage->m_nNoOfExp = nVarLong;
		if( !pImgProperty->m_pExpIso->GetLineText(0).ToLong( &nVarLong ) )
			m_pAstroImage->m_nExpIso = 0;
		else
			m_pAstroImage->m_nExpIso = nVarLong;
		m_pAstroImage->m_strRegProg = pImgProperty->m_pRegProg->GetLineText(0).Trim();
		m_pAstroImage->m_strStackProg = pImgProperty->m_pStackProg->GetLineText(0).Trim();
		m_pAstroImage->m_nStackMethod = pImgProperty->m_pStackMethod->GetSelection();
		m_pAstroImage->m_strProcProg = pImgProperty->m_pProcProg->GetLineText(0).Trim();

		// set the flag changed to save again
		m_pAstroImage->m_bIsChanged = 1;
	}

	delete( pImgProperty );

}

////////////////////////////////////////////////////////////////////
// Method:		OnGroupProperties
// Class:		CGUIFrame
// Purpose:		set group properties
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnGroupProperties( wxCommandEvent& pEvent )
{
	CGroupProperty* pGroupProperty = new CGroupProperty( this, wxT("Group Properties") );
	// first save group profile
	m_pImageDb->m_pCurrentGroup->Save( );

	// get current group image if loaded
	wxString strCurrImgName = wxT("");
	wxString strCurrImgExt = wxT("");
	if( m_pAstroImage )
	{
		m_pAstroImage->SaveProfile( 0 );
		strCurrImgName = wxString(m_pImageDb->m_pCurrentImage->m_strImageName,wxConvUTF8);
		strCurrImgExt = wxString(m_pImageDb->m_pCurrentImage->m_strImageExt,wxConvUTF8);
		m_pAstroImage = NULL;
	}

	// create a dup group to play with
	CImageGroup* pTempGroup = new CImageGroup( m_pImageDb->m_pCurrentGroup->m_strGroupNameId );
	pTempGroup->m_pImageDb = m_pImageDb;
	pTempGroup->SetPath( m_pImageDb->m_pCurrentGroup->m_strGroupPath );
	// init
	pTempGroup->Init( );

	// set configuration from this group
	pGroupProperty->SetConfig( pTempGroup, m_pMainConfig->m_nSkyCoordDisplayFormat );
	// display dialog
	if( pGroupProperty->ShowModal() == wxID_OK )
	{
		long nIntVar = 0;
		double nFloatVar = 0;
		wxString strTmp;

		////////////////////////////////////
		// replace current group :: first delete the old group
		m_pImageDb->m_pCurrentGroup->m_bDontSaveOnDestroy = 1;
		delete( m_pImageDb->m_pCurrentGroup );
		// then set the temp group as this one
		m_pImageDb->m_pCurrentGroup = pTempGroup;
		m_pImageDb->m_vectImageGroup[m_pImageDb->m_nCurrentGroup] = pTempGroup;

		// reset group image list
		m_pImageDb->SelectGroup( m_pImageDb->m_nCurrentGroup );
		// also set frame - check to see if last image is in this group
		if( !strCurrImgName.IsEmpty() )
		{
			int nImgId = -1;
			CAstroImage* pLoadedImage = m_pImageDb->m_pCurrentGroup->FindImageByName( strCurrImgName, strCurrImgExt, nImgId );
			if( pLoadedImage )
			{
				m_pImageDb->m_pCurrentImage = pLoadedImage;
				m_pImageSelect->SetSelection( nImgId+1 );
				LoadSelectedImage();
			}
		}

		//////////////////////////
		//set stacking options
		// :: stack method
		nIntVar = pGroupProperty->m_pGroupStackMethod->GetSelection();
		m_pImageDb->m_pCurrentGroup->m_nImageStackMethod = nIntVar;
		// :: sigmas
		if( pGroupProperty->m_pStackSigma->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pImageDb->m_pCurrentGroup->m_nStackFrameSigma = nFloatVar;
		// :: iterations
		if( pGroupProperty->m_pStackIterations->GetLineText(0).ToLong( &nIntVar ) )
			m_pImageDb->m_pCurrentGroup->m_StackFrameIteration = (int) nIntVar;
		// :: bias frame
		strTmp = pGroupProperty->m_pBiasFrameEntry->GetLineText(0);
		m_pImageDb->m_pCurrentGroup->m_strStackBiasFrame = strTmp;
		// :: dark frame
		strTmp = pGroupProperty->m_pDarkFrameEntry->GetLineText(0);
		m_pImageDb->m_pCurrentGroup->m_strStackDarkFrame = strTmp;
		// :: flat frame
		strTmp = pGroupProperty->m_pFlatFrameEntry->GetLineText(0);
		m_pImageDb->m_pCurrentGroup->m_strStackFlatFrame = strTmp;
		// :: blur method
		nIntVar = pGroupProperty->m_pStackFrameBlurMethod->GetSelection();
		m_pImageDb->m_pCurrentGroup->m_nStackFrameBlurMethod = nIntVar;

		//////////////////////////
		//set discovery options
		// :: histogram compare method
		nIntVar = pGroupProperty->m_pObjHistogramCmpMethod->GetSelection();
		m_pImageDb->m_pCurrentGroup->m_nHistCmpMethod = nIntVar;
		// :: histogram normalization
		if( pGroupProperty->m_pObjHistogramNormalize->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pImageDb->m_pCurrentGroup->m_nObjHistogramNormalize = nFloatVar;
		// :: DynamicObjMatchMaxFwhm
		if( pGroupProperty->m_pDynamicObjMatchMaxFwhm->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pImageDb->m_pCurrentGroup->m_nDynamicObjMatchMaxFwhm = nFloatVar;
		// :: DynamicObjMinDistMatch
		if( pGroupProperty->m_pDynamicObjMinDistMatch->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pImageDb->m_pCurrentGroup->m_nDynamicObjMinDistMatch = nFloatVar;
		// :: ObjDynamicsMagTol
		if( pGroupProperty->m_pObjDynamicsMagTol->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pImageDb->m_pCurrentGroup->m_nObjDynamicsMagTol = nFloatVar;
		// :: ObjDynamicsKronFactorTol
		if( pGroupProperty->m_pObjDynamicsKronFactorTol->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pImageDb->m_pCurrentGroup->m_nObjDynamicsKronFactorTol = nFloatVar;
		// :: ObjDynamicsFwhmTol
		if( pGroupProperty->m_pObjDynamicsFwhmTol->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pImageDb->m_pCurrentGroup->m_nObjDynamicsFwhmTol = nFloatVar;
		// :: PathDevAngleTolerance
		if( pGroupProperty->m_pPathDevAngleTolerance->GetLineText(0).ToDouble( &nFloatVar ) )
			m_pImageDb->m_pCurrentGroup->m_nPathDevAngleTolerance = nFloatVar;

		/////////////////
		// init group  thumbs
		if( m_pMainConfig->m_bShowImageThumbs )
		{
			m_pImageDb->CreateGroupThumbnails( m_pImageDb->m_nCurrentGroup );
			// now set worker thread
			DefCmdAction cmd;
			cmd.id = THREAD_ACTION_SET_GROUP_THUMBNAILS;
			m_pUnimapWorker->SetAction( cmd );
			// set init flag
			m_bGroupThumbnailsInitialized = 1;
		}

		// save group profile
		m_pImageDb->m_pCurrentGroup->Save( );

	} else
	{
		pTempGroup->m_bDontSaveOnDestroy = 1;
		delete( pTempGroup );
	}

	delete( pGroupProperty );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnDetectClear
// Class:		CGUIFrame
// Purpose:		clear object detection
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnDetectClear( wxCommandEvent& pEvent )
{
	m_pAstroImage->ClearDetected( );
	// also call for clear of the view
	m_pImgView->FreeData();

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnDetect
// Class:		CGUIFrame
// Purpose:		do object detection in an image
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnDetect( wxCommandEvent& pEvent )
{
	int nStatus = 0;
	nStatus = RunObjectDetection( 1 );
//	if( !nStatus ) GetToolBar()->ToggleTool( wxID_BUTTON_OBJDETECT, 0 );
	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnDetect
// Class:		CGUIFrame
// Purpose:		find an object in the image
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CGUIFrame::RunObjectDetection( int bRun )
{
	// first we check if images was loaded
	if( m_pAstroImage == NULL )
	{
		wxMessageBox( wxT("No Image is currently selected"), 
						wxT("Warning"), wxICON_EXCLAMATION );
		return( 0 );
	}
	if( bRun )
	{
//		m_pUnimapWorker->DoStartDetection( m_pAstroImage );
	
		DefCmdAction act;
		act.id = THREAD_ACTION_DETECT_START;
		act.vectObj[0] = (void*) m_pAstroImage;

		/////////////////
		// show wait dialog
		CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Detection Process"), 
							wxT("Detecting objects in the image..."), 0, m_pUnimapWorker, &act );
		pWaitDialog->ShowModal();
		// delete/reset wait dialog
		delete( pWaitDialog );
		pWaitDialog = NULL;

		///////////////////
		// done on close object detection
		m_pImgView->SetStars( m_pAstroImage, m_pMainConfig->m_nViewDetectedObjects );
		m_pLogger->Log( wxT("INFO :: end of object detection") );

		SetStatusDetected( m_pAstroImage->m_nStar );
		// set menu state
		SetMenuState( 0, 0 );

		Refresh( FALSE );
		Update( );

		// set button back to normal
//		GetToolBar()->ToggleTool( wxID_BUTTON_OBJDETECT, 0 );

	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	OnMinimize
// Class:	CGUIFrame
// Purose:	when minimize
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnMinimize( wxIconizeEvent& pEvent )
{
	if( pEvent.Iconized() && m_pMainConfig->m_bHideOnMinimize )
	{
		//wxIcon icon( unimap_xpm );
		m_pFrameTaskBarIcon->SetIcon( wxICON( ICON_UNIMAP ), wxT("UniMap Control") );
//		Iconize(TRUE);
		this->Show(FALSE);
		m_bIconized = 1;

		// todo: here we optimizez memory usage in iconize mode... 

	} else
		pEvent.Skip();

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	AddGroupToSelect
// Class:	CGUIFrame
// Purose:	add a group to the list of groups forbrowse
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CGUIFrame::AddGroupToSelect( const wxString& strGroupName )
{

	m_pGroupSelect->Append( strGroupName );

	return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:	AddImageToSelect
// Class:	CGUIFrame
// Purose:	add a group to the list of groups forbrowse
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CGUIFrame::AddImageToSelect( wxString& strImageName )
{

	m_pImageSelect->Append( strImageName );

	return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:	AddImageToGroupThumbs
// Class:	CGUIFrame
// Purose:	add imate to the group thumbnails
// Input:	image name
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CGUIFrame::AddImageToGroupThumbs( int nId, wxString& strImageName )
{

	//InsertItem( 0, "0aaaa" );
	m_pGroupImgView->InsertItem( nId, strImageName );

	return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:	AssignImagesToGroupThumbnails
// Class:	CGUIFrame
// Purose:	add imagelist to the group thumbnails
// Input:	image list, count
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::AssignImagesToGroupThumbnails( wxImageList* vectImg, int nImg )
{
	int i=0;
	// associate list
	m_pGroupImgView->AssignImageList( vectImg, wxIMAGE_LIST_NORMAL );

	//////////////////////
	// assign images to items
	for( i=0; i<nImg; i++ )
	{
		m_pGroupImgView->SetItemImage( i, i );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetGroupThumbnailImage
// Class:	CGUIFrame
// Purose:	set new bitmap image
// Input:	image id, new bitmap
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CGUIFrame::SetGroupThumbnailImage( int nId, wxBitmap& bmpImg )
{
	wxImageList* vectImg = m_pGroupImgView->GetImageList( wxIMAGE_LIST_NORMAL );
	vectImg->Replace( nId, bmpImg );

	m_pGroupImgView->Refresh( FALSE );
	//Update( )

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CGUIFrame::ClearImageBrowsingList(  )
{
	m_pImageSelect->Clear();
	m_pImageSelect->Append( wxT("Select Image") );

	return( 0 );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::ClearGroupThumbnails( )
{
	m_pGroupImgView->ClearAll();
	return;
}

////////////////////////////////////////////////////////////////////
int CGUIFrame::SelectImageFromList( int nIndex )
{
	m_pImageSelect->Select( nIndex );
	return(0);
}

/*
////////////////////////////////////////////////////////////////////
int CGUIFrame::SelectGroupImageThumbnail( int nIndex )
{
	m_pGroupImgView->SetItemState( nIndex, wxLIST_STATE_SELECTED, wxLIST_MASK_STATE  );

	return( 0 );
}
*/

// Purpose:	when a group is selected
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnGroupSelect( wxCommandEvent& pEvent )
{
	if( m_pAstroImage ) UnloadSelectedImage();

	m_nCurrentGroup = m_pGroupSelect->GetSelection();

	m_pImageDb->SelectGroup( m_nCurrentGroup );
	// update thumbnails if visible
	if( m_pMainConfig->m_bShowImageThumbs ) UpdateGroupThumbnails( m_nCurrentGroup );
	SetMenuState( 0, 0 );

	// save last group
	m_pMainConfig->SetIntVar( SECTION_ID_GUI, CONF_LAST_GROUP, m_nCurrentGroup );

	return;
}

// Purpose:	when an image is selected
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnImageSelect( wxCommandEvent& pEvent )
{
	m_nCurrentImage = m_pImageSelect->GetSelection()-1;
	if( m_nCurrentImage < 0 ) return;

	// if thumbs visible also scroll on thumbs
	if( m_bGroupThumbnailsInitialized )
		m_pGroupImgView->SelectImage( m_nCurrentImage );

	//  call load selected image
	LoadSelectedImage( );

	// save last image
	m_pMainConfig->SetIntVar( SECTION_ID_GUI, CONF_LAST_GROUP_IMAGE, m_nCurrentImage+1 );

//	m_pCmdView->SetFocus( );

	return;
}

// input flags not used yet
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetMenuState( int nType, int bState )
{
	int bImgState=0, bGrpState = 0,  bDetect = 0, bMatch = 0;
	// set group state
	if( m_pImageDb->m_pCurrentGroup->m_vectImageNode.size() ) bGrpState = 1;
	// set base image state
	if( m_pAstroImage ) bImgState = 1;
	bImgState = bGrpState & bImgState;
	// detect state
	if( m_pAstroImage && m_pAstroImage->m_nStar > 0 ) bDetect = 1;
	// matched
	if( m_pAstroImage && m_pAstroImage->m_bIsMatched ) bMatch = 1;


	//////////////////////
	// MENUS
	wxMenu* pFileMenu = GetMenuBar()->GetMenu(0);
	wxMenu* pViewMenu = GetMenuBar()->GetMenu(1);
	wxMenu* pGroupMenu = GetMenuBar()->GetMenu(2);
	wxMenu* pImageMenu = GetMenuBar()->GetMenu(3);

	//////////////////////
	// group specific states - if/not group empty
	pGroupMenu->Enable( wxID_MENU_GROUP_DETECT, bGrpState );
	pGroupMenu->Enable( wxID_MENU_GROUP_MATCH, bGrpState );
	pGroupMenu->Enable( wxID_MENU_GROUP_REGISTER, bGrpState );
	pGroupMenu->Enable( wxID_MENU_GROUP_ALIGN, bGrpState );
	pGroupMenu->Enable( wxID_MENU_GROUP_UPLOAD, bGrpState );
	pGroupMenu->Enable( wxID_MENU_GROUP_STACK, bGrpState );
	pGroupMenu->Enable( wxID_MENU_GROUP_HINT, bGrpState );
	pGroupMenu->Enable( wxID_MENU_GROUP_DISCOVER, bGrpState );
	pGroupMenu->Enable( wxID_MENU_GROUP_STACK_WITH, bGrpState );

	//////////////
	// image loaded enable/disable all related
	// file
	pFileMenu->Enable( wxID_MENU_FILE_SAVE_IMAGE, bImgState );
	pFileMenu->Enable( wxID_MENU_FILE_SAVE_ALL, bImgState );
	pFileMenu->Enable( wxID_MENU_FILE_EXPORT, bImgState );
	pFileMenu->Enable( wxID_MENU_FILE_ONLINE_UPLOAD_CUSTOM, bImgState );
	pFileMenu->Enable( wxID_MENU_FILE_PRINT_DIALOG, bImgState );
	pFileMenu->Enable( wxID_MENU_FILE_PRINT_PAGE_SETUP, bImgState );
	pFileMenu->Enable( wxID_MENU_FILE_PRINT_PREVIEW, bImgState );
	// view
	pViewMenu->Enable( wxID_MENU_VIEW_OBJECTS, bImgState & bDetect );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS, bImgState & bDetect & bMatch );
	pViewMenu->Enable( wxID_MENU_VIEW_IMAGE_AREAS, bImgState );
	pViewMenu->Enable( wxID_MENU_VIEW_GROUP_FULLSCREEN, bImgState );
	pViewMenu->Enable( wxID_MENU_VIEW_FIND_OBJECT, bImgState & bDetect );
	// images disable all menu
	GetMenuBar()->EnableTop( 3, bImgState );
	// images menu
	if( bImgState )
	{
		pImageMenu->Enable( wxID_MENU_IMAGE_CLEAR_DETECTED, bDetect );
		pImageMenu->Enable( wxID_MENU_IMAGE_MATCH, bDetect );
		pImageMenu->Enable( wxID_MENU_IMAGE_CLEAR_MATCHED, bDetect & bMatch );
		pImageMenu->Enable( wxID_MENU_IMAGE_3D_DISTORTION_GRID, bDetect );
		pImageMenu->Enable( wxID_MENU_IMAGE_3D_SPATIALITY, bDetect & bMatch );
	}

	///////////////////
	// TOOLBAR
	GetToolBar()->EnableTool( wxID_BUTTON_SAVE, bImgState );
	GetToolBar()->EnableTool( wxID_BUTTON_OBJ_DETECT_AND_MATCH, bImgState );
	GetToolBar()->EnableTool( wxID_BUTTON_SKY_GRID, bImgState & bDetect & bMatch );
	GetToolBar()->EnableTool( wxID_BUTTON_DISTORSION_GRID, bImgState & bDetect );
	GetToolBar()->EnableTool( wxID_BUTTON_OBJFIND, bImgState & bDetect );
	GetToolBar()->EnableTool( wxID_BUTTON_MEASURE_DISTANCE, bImgState & bDetect & bMatch );
	GetToolBar()->EnableTool( wxID_BUTTON_IMGZOOM, bImgState );
	GetToolBar()->EnableTool( wxID_BUTTON_IMGINV, bImgState );
	GetToolBar()->EnableTool( wxID_TOOL_ONLINE_UPLOAD_GROUP, bGrpState );
	GetToolBar()->EnableTool( wxID_TOOL_ONLINE_UPLOAD, bImgState );
	GetToolBar()->EnableTool( wxID_BUTTON_AREA_ADD, bImgState );

}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetMenuAppsState( )
{
	wxMenu* pGroupMenu = GetMenuBar()->GetMenu(2);
	wxMenu* pImageMenu = GetMenuBar()->GetMenu(3);
	CAppsManager* pApps = m_pUnimap->m_pAppsMgr;

	pGroupMenu->Enable( wxID_MENU_GROUP_STACK_REGISTAX, pApps->IsApp(MGR_APPS_NAMEID_REGISTAX) );
	pGroupMenu->Enable( wxID_MENU_GROUP_STACK_DEEPSKYSTACKER, pApps->IsApp(MGR_APPS_NAMEID_DEEPSKYSTACKER) );
	pGroupMenu->Enable( wxID_MENU_GROUP_STACK_IRIS, pApps->IsApp(MGR_APPS_NAMEID_IRIS) );
	pGroupMenu->Enable( wxID_MENU_GROUP_STACK_MAXIMDL, pApps->IsApp(MGR_APPS_NAMEID_MAXIMDL) );
	pGroupMenu->Enable( wxID_MENU_GROUP_STACK_IMAGEPLUS, pApps->IsApp(MGR_APPS_NAMEID_IMAGEPLUS) );
	pGroupMenu->Enable( wxID_MENU_GROUP_STACK_REGISTAR, pApps->IsApp(MGR_APPS_NAMEID_REGISTAR) );
	pGroupMenu->Enable( wxID_MENU_GROUP_STACK_K3CCDTOOLS, pApps->IsApp(MGR_APPS_NAMEID_K3CCDTOOLS) );

	pImageMenu->Enable( wxID_MENU_IMAGE_PROCESS_PHOTOSHOP, pApps->IsApp(MGR_APPS_NAMEID_PHOTOSHOP) );
	pImageMenu->Enable( wxID_MENU_IMAGE_PROCESS_GIMP, pApps->IsApp(MGR_APPS_NAMEID_GIMP) );
	pImageMenu->Enable( wxID_MENU_IMAGE_PROCESS_PIXINSIGHT, pApps->IsApp(MGR_APPS_NAMEID_PIXINSIGHT) );
	pImageMenu->Enable( wxID_MENU_IMAGE_PROCESS_MAXIMDL, pApps->IsApp(MGR_APPS_NAMEID_MAXIMDL) );
	pImageMenu->Enable( wxID_MENU_IMAGE_PROCESS_IMAGEPLUS, pApps->IsApp(MGR_APPS_NAMEID_IMAGEPLUS) );
}

// Purpose:	when an image thumbnail is selected
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnImageThumbnailSelect( wxListEvent& pEvent )
{
	int nGroupIndex = m_pGroupSelect->GetSelection();
	int nSelect = pEvent.GetIndex();
	if( nGroupIndex == m_nCurrentGroup && m_nCurrentImage == nSelect && nSelect > 0 ) 
		return;

	// also set the list
	m_nCurrentImage = nSelect;
	m_pImageSelect->SetSelection( m_nCurrentImage+1 );

	LoadSelectedImage( );

	// save last image
	m_pMainConfig->SetIntVar( SECTION_ID_GUI, CONF_LAST_GROUP_IMAGE, nSelect+1 );

	return;
}

////////////////////////////////////////////////////////////////////
// Purpose:	unload current selected image/and set selection 
//			widget to none if asked to
////////////////////////////////////////////////////////////////////
void CGUIFrame::UnloadSelectedImage( int bReset )
{
	int nImageSelect = m_pImageSelect->GetSelection();
	if( nImageSelect <= 0 ) return;

	m_pAstroImage->SaveProfile( 1 );

	// here i should de-allocate all the image data .. delete object?
	// or delete all from current group loaded
	m_pAstroImage->FreeData( );
	m_pAstroImage = NULL;

	m_pImgView->FreeAll( );
	//m_pImgView->m_pAstroImage = NULL;

	if( bReset ) m_pImageSelect->SetSelection(0);
}

////////////////////////////////////////////////////////////////////
// Purpose:		Load current selected image
////////////////////////////////////////////////////////////////////
void CGUIFrame::LoadSelectedImage( int bKeepInstr )
{
	////////////////
	// first check if this is with camera view
	if( m_bInstrActive && m_bInstrCameraView && !bKeepInstr )
	{
		m_bInstrCameraView = 0;
		m_pInstrToolBar->ToggleTool( wxID_INSTR_SHOW_CTRL_CAMERA, 0 );
		m_pCameraSelect->Disable();
	}

	//  we get our next selection
	int nGroupSelect = m_pGroupSelect->GetSelection();
	int nImageSelect = m_pImageSelect->GetSelection();

	//delete( m_pAstroImage ); - larry :: i should do here close data
	// and save the data (stars detected matched) in a file before close
	if( m_pAstroImage ) UnloadSelectedImage();

	// check if it's not first option
	if( nImageSelect == 0 ) return;

	// call to get image for database
	LoadImageWait( nGroupSelect, nImageSelect-1 );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetAstroImageView( )
{
	/////////////////////////////////////////
	// now display current image
//	m_pAstroImage = new CAstroImage( );
	m_pImgView->SetImage( m_pAstroImage );

	// now if stars were loaded from profile set names
	if( m_pAstroImage->m_nStar > 0 ) SetViewAllObjects( );

	// if image is matched copy trans to starmatch object
	if( m_pAstroImage->m_bIsMatched == 1 )
	{
		// set sky grid if button is on
		if( GetToolBar()->GetToolState( wxID_BUTTON_SKY_GRID ) )  m_pImgView->SetSkyGrid( );
		// set distortion grid if button is on
		if( GetToolBar()->GetToolState( wxID_BUTTON_DISTORSION_GRID ) )
		{
			//m_pAstroImage->CalcDistGrid( );
			m_pImgView->m_bIsDistGrid = 1;
			m_pImgView->Refresh();
		}
	}

	//////////////////////////////////
	// here we set the status bar with the info on the image
	SetStatusDetected( m_pAstroImage->m_nStar );
	SetStatusMatched( m_pAstroImage->m_nMatchedStars );
	SetStatusDso( m_pAstroImage->m_nCatDso );
	SetStatusCatStar( m_pAstroImage->m_nCatStar );

	////////////////////
	// if instruments view
	if( m_bInstrActive )
	{
		// set histogram
		SetHistogram( );
	}

	m_bHasPageSetup = 0;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::LoadImageWait( int group_id, int image_id )
{
	DefCmdAction act;
	act.id = THREAD_ACTION_IMAGE_LOAD;
	act.vectInt[0] = 0;
	act.vectInt[1] = group_id;
	act.vectInt[2] = image_id;
	act.vectInt[3] = GetToolBar()->GetToolState( wxID_BUTTON_DISTORSION_GRID );
	act.vectObj[0] = (void*) m_pAstroImage;

	/////////////////
	// show wait dialog
	CWaitDialog dlgWait( this, wxT("Load Astro-Image"), wxT("Loading image ..."), 0, m_pUnimapWorker, &act );
	dlgWait.ShowModal();

	// now set the astro view
	m_pAstroImage = m_pImageDb->m_pCurrentImage;

	// check other toolbar buttons if to let on or off
	if( !m_pAstroImage->m_bIsMatched )
	{
		GetToolBar()->ToggleTool( wxID_BUTTON_SKY_GRID, 0 );
		GetToolBar()->ToggleTool( wxID_BUTTON_DISTORSION_GRID, 0 );
		GetToolBar()->ToggleTool( wxID_BUTTON_MEASURE_DISTANCE, 0 );
		if( m_pDistanceTool ) m_pDistanceTool->Hide();
		m_pImgView->m_bIsDistanceTool = 0;
	}
	// also toggle off some other button
	GetToolBar()->ToggleTool( wxID_BUTTON_IMGINV, 0 );
	GetToolBar()->ToggleTool( wxID_BUTTON_AREA_ADD, 0 );
	GetToolBar()->ToggleTool( wxID_BUTTON_IMGZOOM, 0 );

	// update current view
	SetAstroImageView( );

	// set menu states
	SetMenuState( 0, 0 );
	// set title with image name
	wxString strTitle;
	strTitle.Printf( wxT("%s :: %s"), UNIMAP_FRAME_TITLE, m_pAstroImage->m_strImageName );
	SetTitle( strTitle );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::LoadImageBG( int group_id, int image_id )
{
	DefCmdAction act;
	act.id = THREAD_ACTION_IMAGE_LOAD;
	act.vectInt[0] = 1;
	act.vectInt[1] = group_id;
	act.vectInt[2] = image_id;
	act.vectInt[3] = GetToolBar()->GetToolState( wxID_BUTTON_DISTORSION_GRID );
	act.vectObj[0] = (void*) m_pAstroImage;
	act.handler = this->GetEventHandler();
	// set thread
	m_pUnimapWorker->SetAction( act );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnAstroImageLoaded( wxCommandEvent& pEvent )
{
	// now set the astro view
	m_pAstroImage = m_pImageDb->m_pCurrentImage;
	SetAstroImageView( );

	// set menu states
	SetMenuState( 0, 0 );
	// set title with image name
	wxString strTitle;
	strTitle.Printf( wxT("%s :: %s"), UNIMAP_FRAME_TITLE, m_pAstroImage->m_strImageName );
	SetTitle( strTitle );

	// if set to close 
	if( m_bIsClosing ) 
	{
		DeleteSlideShowTimer();
		this->Destroy();
	}

	// reset loading flag back
	m_bSlideLoading = 0;
}

////////////////////////////////////////////////////////////////////
// Method:		OnSaveAll
// Class:		CGUIFrame
// Purpose:		save all
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnSaveAll( wxCommandEvent& pEvent )
{
	m_pImageDb->SaveAllGroups( );
	m_pImageDb->SaveGroups( );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnFileExport( wxCommandEvent& pEvent )
{
	CFileExportDlg dlg( this, wxT("Export File"), m_pImageDb->m_pCurrentGroup, m_pAstroImage );
	if( dlg.ShowModal() == wxID_OK )
	{
		wxString strFile = dlg.m_pFile->GetLineText(0);
		// export image in html format
		m_pAstroImage->ExportToHtml( strFile );
	}
}

// Purpose:	add new images to current/new group
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnAddImages( wxCommandEvent& pEvent )
{
	// call add images dialog
	AddImages( );
	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::AddImages( )
{
	wxString strTmp;

	CGroupSetDialog *pGroupDlg = new CGroupSetDialog( this, wxT("Add Image Files"), 
												m_pMainConfig->m_strGroupPath );
	// and set the config
	pGroupDlg->SetConfig( m_pMainConfig, m_pImageDb );
	
	if( pGroupDlg->ShowModal() == wxID_OK )
	{
		// get group data
		wxString strGroupName = wxT(""); 
		if( pGroupDlg->m_nActionType == 1 )
			strGroupName = pGroupDlg->m_pGroupNameEntry->GetLineText(0);
		else
			strGroupName = pGroupDlg->m_pSelectGroup->GetStringSelection();
		// get files
		wxString strGroupFiles = pGroupDlg->m_pGroupPathEntry->GetLineText(0);
		// trim strings
		strGroupName.Trim( 0 ).Trim( 1 );
		strGroupFiles.Trim( 0 ).Trim( 1 );
		// check if empty
		if( !strGroupFiles.IsEmpty() )
		{
			// to add the files to my image data base
			m_pImageDb->AddImages( strGroupName, strGroupFiles );

			// update thumbnails if visible
			int nSelectGroup = m_pGroupSelect->GetSelection();
			if( m_pMainConfig->m_bShowImageThumbs ) UpdateGroupThumbnails( nSelectGroup );

			// load current selected image - the last image will be displayed
			LoadSelectedImage( );
			// and also save groups / images
			m_pImageDb->SaveGroups( );
			m_pImageDb->SaveAllGroups( );
			// and set last path used
			m_pMainConfig->m_strGroupPath = pGroupDlg->m_strPath;
			//sprintf( strTmp, "%s", m_pMainConfig->m_strGroupPath.GetData() );
			m_pMainConfig->SetStrVar( SECTION_ID_GUI, CONF_ADD_FILE_PATH, m_pMainConfig->m_strGroupPath );

	//		// add my group to the list
	//		AddGroupToSelect( strGroupName.GetData() );
		}
	}

	delete( pGroupDlg );

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnNewGroup( wxCommandEvent& pEvent )
{
	CGroupAddDlg dlg( this, wxT("Create a new group of images...") );
	if( dlg.ShowModal() == wxID_OK )
	{
		if( m_pAstroImage ) UnloadSelectedImage();

		// clean group view 
		m_pGroupImgView->ClearAll();

		wxString strGroupName = dlg.m_pName->GetLineText(0);
		wxString strGroupPath = dlg.m_pPath->GetLineText(0);
		int bAddImages = dlg.m_pAddFiles->GetValue();
		int bInclFolders = dlg.m_pIncFolders->GetValue();

		int nGroupId = m_pImageDb->AddGroup( strGroupName, strGroupPath, bAddImages, bInclFolders );
		m_pGroupSelect->SetSelection( nGroupId );
		m_pImageDb->SelectGroup( nGroupId );
		SetMenuState( 0, 0 );

		// check if initialized
		int nSelect = m_pGroupSelect->GetSelection();
		// update icons
		UpdateGroupThumbnails( nSelect );

		// call to select current if any
		//if( m_nCurrentImage > 0 ) m_pGroupImgView->SelectImage( m_nCurrentImage );
	}
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnDeleteGroup( wxCommandEvent& pEvent )
{
	wxString strMsg=wxT("");

	int nGroup = m_pGroupSelect->GetSelection();
	strMsg.Printf( wxT("Delete group=%s ?"), m_pImageDb->m_vectImageGroup[nGroup]->m_strGroupNameId );
	int nOk = wxMessageBox(  strMsg, wxT("Delete group?"), wxICON_EXCLAMATION|wxYES_NO );

	if( nOk == wxYES )
	{
		// don't delete the default group or if not exist
		if( m_pImageDb->DeleteGroup( nGroup ) )
		{
			m_pGroupSelect->Delete( nGroup );
			m_pGroupSelect->SetSelection( nGroup-1 );
		}
	}
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnAddImage( wxCommandEvent& pEvent )
{
	// from elynx - get supported type
//	const char * prIn = the_ImageFileManager.GetInputFileFormatList( true );
//	wxString strImageFormatListIn = wxString::FromAscii( prIn );

	wxString strImageFormatListIn = GetSupportedImageReadTypes();

	wxFileDialog fileDlg( this, wxT("Select Image to add to the group"),
							m_pMainConfig->m_strGroupPath, wxT(""), strImageFormatListIn, wxFD_DEFAULT_STYLE );

	if( fileDlg.ShowModal() == wxID_OK )
	{
		wxString strImage = fileDlg.GetPath();
		int nImageId = m_pImageDb->m_pCurrentGroup->AddImage( strImage );
		// reselect group - todo: - o don't think i need to reselect group- just add image to choice
		int nSelectGroup = m_pGroupSelect->GetSelection();
		m_pImageDb->SelectGroup( nSelectGroup );

		// update thumbnails if visible
		if( m_pMainConfig->m_bShowImageThumbs ) UpdateGroupThumbnails( nSelectGroup );

		// select image in group
		m_pImageSelect->SetSelection( nImageId+1 );
		LoadSelectedImage( );
	}

	return;
}


////////////////////////////////////////////////////////////////////
// Method:		OnViewLogger
// Class:		CGUIFrame
// Purpose:		display about dialog
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnViewLogger( wxCommandEvent& pEvent )
{
	if( pEvent.IsChecked( ) )
	{
		m_pLoggerView->Show();
		m_pMainConfig->m_bLogsView = 1;
	} else
	{
		m_pLoggerView->Hide();
		m_pMainConfig->m_bLogsView = 0;
	}

//	m_pMainConfig->SetIntVar( SECTION_ID_GUI, CONF_LOGS_VIEW, m_pMainConfig->m_bLogsView );
//	m_pCmdView->SetFocus( );

	GetSizer()->Layout( );
	this->Layout( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnViewGroupThumbnails
// Class:		CGUIFrame
// Purpose:		view thumbnails window
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnViewGroupThumbnails( wxCommandEvent& pEvent )
{
	int bState = pEvent.IsChecked( );
	int nId = pEvent.GetId();
	ViewGroupThumbnails( bState, nId );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::ViewGroupThumbnails( int bState, int nId )
{
	if( bState )
	{
		// check if initialized
		if( !m_bGroupThumbnailsInitialized )
		{
			int nSelect = m_pGroupSelect->GetSelection();
			//m_pImageDb->SelectGroup( nSelect );
			// update icons
			UpdateGroupThumbnails( nSelect );
		}
		// show group
		m_pGroupImgView->Show();
		m_pMainConfig->m_bShowImageThumbs = 1;
		// call to select current if any
		if( m_nCurrentImage > 0 ) m_pGroupImgView->SelectImage( m_nCurrentImage );

	} else
	{
		m_pGroupImgView->Hide();
		m_pMainConfig->m_bShowImageThumbs = 0;
	}

	// now check the origin
	if( nId == wxID_TOOL_SHOW_CTRL_THUMBS || nId == -1 )
	{
		// mark also the menu option
		wxMenu* pViewMenu = GetMenuBar()->GetMenu( 1 );
		pViewMenu->Check( wxID_MENU_VIEW_GROUP_THUMBNAILS, bState );

	}
	if( nId == wxID_MENU_VIEW_GROUP_THUMBNAILS || nId == -1 )
	{
		// toggle button on/off
		GetToolBar()->ToggleTool( wxID_TOOL_SHOW_CTRL_THUMBS, bState );
	}

//	m_pMainConfig->SetIntVar( SECTION_ID_GUI, CONF_GROUP_THUMBNAILS_VIEW, m_pMainConfig->m_bShowImgeThumbs );

	GetSizer()->Layout( );
	this->Layout( );

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnViewFullScreen( wxCommandEvent& pEvent )
{
	// set image full screen
	SetFullScreen( );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::UpdateGroupThumbnails( int nGroup )
{
	m_pImageDb->CreateGroupThumbnails( nGroup );
	// now set worker thread
	DefCmdAction cmd;
	cmd.id = THREAD_ACTION_SET_GROUP_THUMBNAILS;
	m_pUnimapWorker->SetAction( cmd );
	// set init flag
	m_bGroupThumbnailsInitialized = 1;
}

////////////////////////////////////////////////////////////////////
// Purpose:	set the number of objects by type to be showen
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnViewObjects( wxCommandEvent& pEvent )
{
	// if checked show dialog
//	if( pEvent.IsChecked() )
//	{
		CViewObjectsDlg dlg( this, m_pMainConfig );
		if( dlg.ShowModal() == wxID_OK )
		{
			// get heade options
			int bViewAll = dlg.m_pViewAll->GetValue();
			int bHideAll = dlg.m_pHideAll->GetValue();
			m_pMainConfig->m_bViewDetectedObjectsNoMatch = !(dlg.m_pHideNoMatch->GetValue());
			// set config
			m_pMainConfig->m_bViewObjectsType = 0;
			if( bViewAll ) m_pMainConfig->m_bViewObjectsType = 1;
			if( bHideAll ) m_pMainConfig->m_bViewObjectsType = 2;

			// save setting in main config
			if( !bViewAll && !bHideAll )
			{
				// get objects sources values
				m_pMainConfig->m_nViewDetectedObjects = dlg.GetObjSrcVal(VIEW_OBJ_DETECT);
				m_pMainConfig->m_nViewCatalogStars = dlg.GetObjSrcVal(VIEW_OBJ_CAT_STARS);
				m_pMainConfig->m_nViewCatalogDso = dlg.GetObjSrcVal(VIEW_OBJ_CAT_DSO);
				m_pMainConfig->m_nViewCatalogXGamma = dlg.GetObjSrcVal(VIEW_OBJ_CAT_GAMMA_XRAY);
				m_pMainConfig->m_nViewCatalogRadio = dlg.GetObjSrcVal(VIEW_OBJ_CAT_RADIO);
				m_pMainConfig->m_nViewCatalogMStars = dlg.GetObjSrcVal(VIEW_OBJ_CAT_MSTARS);
				m_pMainConfig->m_nViewCatalogSupernovas = dlg.GetObjSrcVal(VIEW_OBJ_CAT_SUPERNOVAS);
				m_pMainConfig->m_nViewCatalogBlackholes = dlg.GetObjSrcVal(VIEW_OBJ_CAT_BLACKHOLES);
				m_pMainConfig->m_nViewCatalogExoplanets = dlg.GetObjSrcVal(VIEW_OBJ_CAT_EXO_PLANETS);
				m_pMainConfig->m_nViewCatalogAsteroids = dlg.GetObjSrcVal(VIEW_OBJ_CAT_ASTEROIDS);
				m_pMainConfig->m_nViewCatalogComets = dlg.GetObjSrcVal(VIEW_OBJ_CAT_COMETS);
				m_pMainConfig->m_nViewCatalogAes = dlg.GetObjSrcVal(VIEW_OBJ_CAT_SATELLITES);
			}

			// redisplay all
			SetViewAllObjects( 1 );
		}

//	} else
//	{

//	}
}

////////////////////////////////////////////////////////////////////
// Method:		SetConstellationsMenu
// Class:		CGUIFrame
// Purpose:		set constellations menu items based on the config
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetConstellationsMenu( )
{
	wxMenu* pViewMenu = GetMenuBar()->GetMenu( 1 );

	// first zero all constellations
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_SHOW, 0 );
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_LINES, 0 );
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_LABEL, 0 );
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_ARTWORK, 0 );
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_HARAY, 0 );
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_IAU, 0 );
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_CHINESE, 0 );
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_EGYPTIAN, 0 );
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_INUIT, 0 );
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_KOREAN, 0 );
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_LAKOTA, 0 );
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_MAORI, 0 );
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_NAVAJO, 0 );
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_NORSE, 0 );
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_POLYNESIAN, 0 );
	pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_TUPI, 0 );

	///////////////
	// option to show hide
	if( m_pMainConfig->m_bShowConstellations ) pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_SHOW, 1 );
	// :: lines
	if( m_pMainConfig->m_bConstellationLines ) pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_LINES, 1 );
	// :: labels
	if( m_pMainConfig->m_bConstellationLabel ) pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_LABEL, 1 );
	// :: artwork
	if( m_pMainConfig->m_bConstellationArtwork ) pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_ARTWORK, 1 );

	/////////////
	// now based on the other opts
	if( m_pMainConfig->m_nConstellationCulture == SKY_CULTURE_WESTERN )
	{
		if( m_pMainConfig->m_nConstellationVersion == CONSTELLATION_WESTERN_HARAY )
			pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_HARAY, 1 );
		else if( m_pMainConfig->m_nConstellationVersion == CONSTELLATION_WESTERN_IAU )
			pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_IAU, 1 );

	} else if( m_pMainConfig->m_nConstellationCulture == SKY_CULTURE_CHINESE )
		pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_CHINESE, 1 );
	else if( m_pMainConfig->m_nConstellationCulture == SKY_CULTURE_EGYPTIAN )
		pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_EGYPTIAN, 1 );
	else if( m_pMainConfig->m_nConstellationCulture == SKY_CULTURE_INUIT )
		pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_INUIT, 1 );
	else if( m_pMainConfig->m_nConstellationCulture == SKY_CULTURE_KOREAN )
		pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_KOREAN, 1 );
	else if( m_pMainConfig->m_nConstellationCulture == SKY_CULTURE_LAKOTA )
		pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_LAKOTA, 1 );
	else if( m_pMainConfig->m_nConstellationCulture == SKY_CULTURE_MAORI )
		pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_MAORI, 1 );
	else if( m_pMainConfig->m_nConstellationCulture == SKY_CULTURE_NAVAJO )
		pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_NAVAJO, 1 );
	else if( m_pMainConfig->m_nConstellationCulture == SKY_CULTURE_NORSE )
		pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_NORSE, 1 );
	else if( m_pMainConfig->m_nConstellationCulture == SKY_CULTURE_POLYNESIAN )
		pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_POLYNESIAN, 1 );
	else if( m_pMainConfig->m_nConstellationCulture == SKY_CULTURE_TUPI )
		pViewMenu->Check( wxID_MENU_VIEW_CONSTELLATIONS_TUPI, 1 );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::SetMenuCnstlState( int bState )
{
	wxMenu* pViewMenu = GetMenuBar()->GetMenu( 1 );

	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_LINES, bState );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_LABEL, bState );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_ARTWORK, bState );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_WESTERN, bState );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_HARAY, bState );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_IAU, bState );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_CHINESE, bState );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_EGYPTIAN, bState );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_INUIT, bState );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_KOREAN, bState );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_LAKOTA, bState );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_MAORI, bState );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_NAVAJO, bState );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_NORSE, bState );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_POLYNESIAN, bState );
	pViewMenu->Enable( wxID_MENU_VIEW_CONSTELLATIONS_TUPI, bState );

}

////////////////////////////////////////////////////////////////////
// Purpose:	set to view constellations
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnViewConstellations( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	int nVal = 0;
	if( pEvent.IsChecked() ) nVal = 1;

	if( nId == wxID_MENU_VIEW_CONSTELLATIONS_SHOW )
	{
		m_pMainConfig->m_bShowConstellations = nVal;
		// set menu state
		SetMenuCnstlState( nVal );

	} else if( nId == wxID_MENU_VIEW_CONSTELLATIONS_LINES )
		m_pMainConfig->m_bConstellationLines = nVal;
	else if( nId == wxID_MENU_VIEW_CONSTELLATIONS_LABEL )
		m_pMainConfig->m_bConstellationLabel = nVal;
	else if( nId == wxID_MENU_VIEW_CONSTELLATIONS_ARTWORK )
		m_pMainConfig->m_bConstellationArtwork = nVal;
	else if( nVal == 1 ) 
	{	
		if( nId == wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_HARAY )
		{
			m_pMainConfig->m_nConstellationCulture = SKY_CULTURE_WESTERN;
			m_pMainConfig->m_nConstellationVersion = CONSTELLATION_WESTERN_HARAY;

		} else if( nId == wxID_MENU_VIEW_CONSTELLATIONS_WESTERN_IAU )
		{
			m_pMainConfig->m_nConstellationCulture = SKY_CULTURE_WESTERN;
			m_pMainConfig->m_nConstellationVersion = CONSTELLATION_WESTERN_IAU;

		} else if( nId == wxID_MENU_VIEW_CONSTELLATIONS_CHINESE )
			m_pMainConfig->m_nConstellationCulture = SKY_CULTURE_CHINESE;
		else if( nId == wxID_MENU_VIEW_CONSTELLATIONS_EGYPTIAN )
			m_pMainConfig->m_nConstellationCulture = SKY_CULTURE_EGYPTIAN;
		else if( nId == wxID_MENU_VIEW_CONSTELLATIONS_INUIT )
			m_pMainConfig->m_nConstellationCulture = SKY_CULTURE_INUIT;
		else if( nId == wxID_MENU_VIEW_CONSTELLATIONS_KOREAN )
			m_pMainConfig->m_nConstellationCulture = SKY_CULTURE_KOREAN;
		else if( nId == wxID_MENU_VIEW_CONSTELLATIONS_LAKOTA )
			m_pMainConfig->m_nConstellationCulture = SKY_CULTURE_LAKOTA;
		else if( nId == wxID_MENU_VIEW_CONSTELLATIONS_MAORI )
			m_pMainConfig->m_nConstellationCulture = SKY_CULTURE_MAORI;
		else if( nId == wxID_MENU_VIEW_CONSTELLATIONS_NAVAJO )
			m_pMainConfig->m_nConstellationCulture = SKY_CULTURE_NAVAJO;
		else if( nId == wxID_MENU_VIEW_CONSTELLATIONS_NORSE )
			m_pMainConfig->m_nConstellationCulture = SKY_CULTURE_NORSE;
		else if( nId == wxID_MENU_VIEW_CONSTELLATIONS_POLYNESIAN )
			m_pMainConfig->m_nConstellationCulture = SKY_CULTURE_POLYNESIAN;
		else if( nId == wxID_MENU_VIEW_CONSTELLATIONS_TUPI )
			m_pMainConfig->m_nConstellationCulture = SKY_CULTURE_TUPI;

	}	
	// set constellation menu
	SetConstellationsMenu( );

	////////////
	// call img view to set costellation by type
	if( m_pAstroImage ) m_pImgView->SetCostellations( m_pAstroImage ); //m_pImgView->SetImage( m_pAstroImage );

	// update
	Refresh( FALSE );
	Update( );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnViewImageAreas( wxCommandEvent& pEvent )
{
	if( pEvent.IsChecked( ) )
	{
		m_pMainConfig->m_bViewImageAreas = 1;
	} else
	{
		m_pMainConfig->m_bViewImageAreas = 0;
	}

	m_pImgView->Refresh(false);
}

////////////////////////////////////////////////////////////////////
int CGUIFrame::RunObjectDataFetch( )
{
	DefCmdAction act;
	act.id = THREAD_ACTION_LOAD_OBJECT_LIST_DETAILS;
//	act.vectObj[0] = this;
	act.vectObj[0] = (void*) m_pAstroImage;

	/////////////////
	// show wait dialog
	CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Load Objects Details"), 
						wxT("Loading details ..."), 0, m_pUnimapWorker, &act );
	pWaitDialog->ShowModal();
	// delete/reset wait dialog
	delete( pWaitDialog );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		OnViewFindObject
// Class:		CGUIFrame
// Purpose:		show dialog window with find options
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnViewFindObject( wxCommandEvent& pEvent )
{

	// pop up dialog window to find first
	CFindObjectDialog *pFindObj = new CFindObjectDialog( this, wxT("Find Object") );
	// check if return is ok
	if ( pFindObj->ShowModal( ) == wxID_OK )
	{
		// get string pattern to find
		wxString strObjToFind = pFindObj->m_pFindObjectEntry->GetLineText(0);
		// search in images view between labels - just all for now
		m_pImgView->FindObjectByString( strObjToFind, 0 );
	}
	// delete object
	delete( pFindObj );
	// also toogle button 
	GetToolBar()->ToggleTool( wxID_BUTTON_OBJFIND, 0 );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnView3dSpatiality
// Class:		CGUIFrame
// Purpose:		view space dist grid dialog
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnView3dSpatiality( wxCommandEvent& pEvent )
{
	if( !m_pAstroImage ) return;

	CSpaceDistViewDialog *p3dSpaceDist = new CSpaceDistViewDialog( this, wxT("View Spatiality Map"), m_pUnimapWorker );
	p3dSpaceDist->SetConfig( m_pMainConfig );
	p3dSpaceDist->m_pImageView = m_pImgView;

	///////////
	// SET THREAD ACTION
	DefCmdAction act;
	act.id = THREAD_ACTION_IMAGE_3D_SPACEDIST;
	act.vectObj[0] = (void*) p3dSpaceDist;
	act.vectObj[1] = (void*) m_pAstroImage;
	act.vectInt[0] = -1;
	/////////////////
	// show wait dialog
	CWaitDialog waitDialog( this, wxT("Wait..."), wxT("Converting Image Data..."), 0, m_pUnimapWorker, &act );
	waitDialog.ShowModal();

	// calc dist grid
//	m_pAstroImage->CalcDisplacementPointsByPos( );
	// set dist grid calc from image to dialog
//	p3dSpaceDist->SetAstroImage( m_pAstroImage );

	// check if return is ok
	p3dSpaceDist->Show();
	p3dSpaceDist->m_pImgView3D->SetCurrent( );
	p3dSpaceDist->m_pImgView3D->Init( );
	p3dSpaceDist->m_pImgView3D->SetImageView();

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnView3dDistGrid
// Class:		CGUIFrame
// Purpose:		view dist grid dialog
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnView3dDistGrid( wxCommandEvent& pEvent )
{
	if( !m_pAstroImage ) return;

	CDistGridViewDialog *p3dDistGrid = new CDistGridViewDialog( this, wxT("View Distortion Grid"), m_pUnimapWorker );
	p3dDistGrid->SetConfig( m_pMainConfig );
	p3dDistGrid->m_pImageView = m_pImgView;


	///////////
	// SET THREAD ACTION
	DefCmdAction act;
	act.id = THREAD_ACTION_IMAGE_3D_DISTGRID;
	act.vectObj[0] = (void*) p3dDistGrid;
	act.vectObj[1] = (void*) m_pAstroImage;
	act.vectInt[0] = -1;
	/////////////////
	// show wait dialog
	CWaitDialog waitDialog( this, wxT("Wait..."), wxT("Converting Image Data..."), 0, m_pUnimapWorker, &act );
	waitDialog.ShowModal();

	// calc dist grid
//	m_pAstroImage->CalcDisplacementPointsByPos( );
	// set dist grid calc from image to dialog
//	p3dDistGrid->SetAstroImage( m_pAstroImage );

	// check if return is ok
	p3dDistGrid->Show();
	p3dDistGrid->m_pImgView3D->SetCurrent( );
	p3dDistGrid->m_pImgView3D->Init( );
	p3dDistGrid->m_pImgView3D->SetImageView();

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnView3dColorMap
// Class:		CGUIFrame
// Purpose:		view dist grid dialog
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnView3dColorMap( wxCommandEvent& pEvent )
{
	if( !m_pAstroImage ) return;

	CColorMapViewDialog *p3dColorMap = new CColorMapViewDialog( this, wxT("View Image Color Map"), m_pUnimapWorker );
	p3dColorMap->SetConfig( m_pMainConfig );
	p3dColorMap->m_pImageView = m_pImgView;


	///////////
	// SET THREAD ACTION
	DefCmdAction act;
	act.id = THREAD_ACTION_IMAGE_3D_COLORMAP;
	act.vectObj[0] = (void*) p3dColorMap;
	act.vectObj[1] = (void*) m_pAstroImage;
	act.vectInt[0] = -1;
	act.vectInt[1] = -1;
	/////////////////
	// show wait dialog
	CWaitDialog waitDialog( this, wxT("Wait..."), wxT("Converting Image Data..."), 0, m_pUnimapWorker, &act );
	waitDialog.ShowModal();


	// set dist grid calc from image to dialog
//	p3dColorMap->SetAstroImage( m_pAstroImage );
//	p3dColorMap->m_pImgView3D->SetImageView();

	// show dialog
	p3dColorMap->Show();
	p3dColorMap->m_pImgView3D->SetCurrent( );
	p3dColorMap->m_pImgView3D->Init( );

//	p3dColorMap->SetAstroImage( m_pAstroImage );

	p3dColorMap->m_pImgView3D->SetImageView();

	return;
}

////////////////////////////////////////////////////////////////////
// Purpose:		activate tool to measure distances in the sky
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnMeasureDistanceTool( wxCommandEvent& pEvent )
{
	if( !m_pAstroImage ) 
	{
		GetToolBar()->ToggleTool( wxID_BUTTON_MEASURE_DISTANCE, 0 );
		return;
	}

	if( pEvent.IsChecked( ) )
	{
		// check if image has distance data
		if( !m_pAstroImage->HasDistancesData() )
		{
			// popup to get distance data or skip
			int nOk = wxMessageBox( wxT("Thre is no object distances for this image! Do you want to try to fetch data for it?"), 
							wxT("Warning"), wxICON_EXCLAMATION|wxYES_NO );
			// check answer
			if( nOk == wxYES )
			{
				// call to fetch data
				RunObjectDataFetch( );
				// check again if there are distances - if not notify return
				if( !m_pAstroImage->HasDistancesData() )
				{
					// notify
					int nOk = wxMessageBox( wxT("Sorry but there is no distances data for any object in this image!"), wxT("Error"), wxICON_ERROR|wxOK );
					// toogle back button
					//GetToolBar()->ToggleTool( wxID_BUTTON_MEASURE_DISTANCE, 0 );
					//return;
				}

			} else
			{
				// toogle tool button back off and return
				//GetToolBar()->ToggleTool( wxID_BUTTON_MEASURE_DISTANCE, 0 );
				//return;
			}
		}

		// check if distance tool is created
		if( !m_pDistanceTool ) 
		{
			// create the tool window
			m_pDistanceTool = new CDistToolDataWindow( m_pSky, m_pImgView, 
										wxPoint(m_pImgView->m_nWidth-240, 1), 
										wxSize( 237, 123 ) );
			// add to view sizer to handle the position at top/right
			m_pImgView->GetSizer()->SetDimension( 0, 0, m_pImgView->m_nWidth-2, m_pImgView->m_nHeight-1 );
			m_pImgView->GetSizer()->Add( m_pDistanceTool, 0, wxALIGN_TOP|wxALIGN_RIGHT|wxFIXED_MINSIZE );

			m_pDistanceTool->SetAstroImage( m_pAstroImage );
			m_pImgView->m_pDistTool = m_pDistanceTool;
			// set defaults
			m_pDistanceTool->SetDataLabels( );

		} else
		{
			m_pDistanceTool->Show();
			m_pImgView->GetSizer()->SetDimension( 0, 0, m_pImgView->m_nWidth-2, m_pImgView->m_nHeight-1 );
		}
		m_pImgView->m_bIsDistanceTool = 1;

	} else
	{
		m_pImgView->m_bIsDistanceTool = 0;
		if( m_pDistanceTool )
		{
			//delete( m_pDistanceTool );
			//m_pDistanceTool = NULL;
			m_pDistanceTool->Hide();
		}	
	}	

	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Purose:	set status zoom
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetStatusZoom( int nZoom )
{
	wxString strTmp, strNum;

	strNum.Printf( wxT("%d%s"), nZoom, "%" );
	strTmp.Printf( wxT("%6s"), strNum );
	GetStatusBar()->SetStatusText( strTmp, 1 );

}

////////////////////////////////////////////////////////////////////
// Purose:	set status x,y coord field
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetStatusXY( int nX, int nY )
{
	wxString strTmp;

	strTmp.Printf( wxT("X:%4d Y:%4d"), nX, nY );
	GetStatusBar()->SetStatusText( strTmp, 2 );

}

// Purose:	set status x,y coord field
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetStatusRaDec( double nRa, double nDec )
{
	if( !m_pMainConfig ) return;

	wxString strTmp=wxT("");
	wxString strRa=wxT("");
	wxString strDec=wxT("");

	m_nLastCurRa = nRa;
	m_nLastCurDec = nDec;

	if( m_pMainConfig->m_nSkyCoordDisplayFormat == UNITS_FORMAT_SEXAGESIMAL )
	{
		// set ra/dec
		RaDegToSexagesimal( nRa, strRa );
		DecDegToSexagesimal( nDec, strDec );

		// set main label
		strTmp.Printf( wxT("RA %s DEC %s"), strRa, strDec );

	} else if( m_pMainConfig->m_nSkyCoordDisplayFormat == UNITS_FORMAT_DECIMAL )
	{
		strRa.Printf( wxT("%.4f"), nRa );
		strDec.Printf( wxT("%.4f"), nDec );
		strTmp.Printf( wxT("RA %8s DEC %8s"), strRa, strDec );
	}

	GetStatusBar()->SetStatusText( strTmp, 3 );

	return;
}

// Purose:	set no of objectes detected in status bar
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetStatusDetected( int nDet )
{
	wxString strTmp;

	strTmp.Printf( wxT("DOBJ:%4d"), nDet );
	GetStatusBar()->SetStatusText( strTmp, 4 );

}

// Purose:	set no of objectes matched in status bar
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetStatusMatched( int nMatch )
{
	wxString strTmp;

	strTmp.Printf( wxT("M:%4d"), nMatch );
	GetStatusBar()->SetStatusText( strTmp, 5 );

}

// Purose:	set no of dso object from catalog
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetStatusDso( int nDso )
{
	wxString strTmp;

	strTmp.Printf( wxT("DSO:%4d"), nDso );
	GetStatusBar()->SetStatusText( strTmp, 6 );

}

// Purose:	set no of star/objects from catalog
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetStatusCatStar( int nCatStarNo )
{
	wxString strTmp;

	strTmp.Printf( wxT("CS:%4d"), nCatStarNo );
	GetStatusBar()->SetStatusText( strTmp, 7 );

}

////////////////////////////////////////////////////////////////////
// Method:	OnSize
// Class:	CGUIFrame
// Purose:	adjust on windows resize
// Input:	reference to size event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnSize( wxSizeEvent& pEvent )
{
	m_nWidth = pEvent.GetSize().GetWidth();
	m_nHeight = pEvent.GetSize().GetHeight();

	if( m_bInit == 1 )
	{
		m_pMainConfig->SetIntVar( SECTION_ID_GUI, CONF_GUI_WIDTH, m_nWidth );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI, CONF_GUI_HEIGHT, m_nHeight );
	}

	pEvent.Skip();
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnClose( wxCloseEvent& pEvent )
{
//	DeleteSlideShowTimer( );

	if( m_pSlideShowTimer )
		m_bIsClosing = 1;
	else
		pEvent.Skip();

}

////////////////////////////////////////////////////////////////////
// Method:		OnRemoteLogger
// Class:		CGUIFrame
// Purpose:		on remote loggin
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnRemoteLogger( wxCommandEvent& pEvent )
{
	wxString strMsg;
	// get my logging text
	strMsg = pEvent.GetString();
	int nGUILoggingStyle = pEvent.GetInt();

	// and log my message
	if( nGUILoggingStyle != GUI_LOGGER_DEFAULT_STYLE )
		m_pLogger->Log( strMsg, nGUILoggingStyle );
	else
		m_pLogger->Log( strMsg );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnNotificationMessage
// Class:		CGUIFrame
// Purpose:		on notification message pop up dialog
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnNotificationMessage( wxCommandEvent& pEvent )
{
	wxString strMsg;
	int nMsgType = 0;
	
	// get my message
	strMsg = pEvent.GetString();
	// get my message type
	nMsgType = pEvent.GetInt();
	// now based on type display notification dialog
	if( nMsgType == 1 )
		wxMessageBox( strMsg, wxT("Warning"), wxICON_EXCLAMATION|wxOK );
	else if( nMsgType == 2 )
		wxMessageBox( strMsg, wxT("Error"), wxICON_ERROR|wxOK );
	else
		wxMessageBox( strMsg, wxT("Note"), wxICON_INFORMATION|wxOK );


	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnOnlineUpload
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnOnlineUpload( wxCommandEvent& pEvent )
{
	int nActionId = pEvent.GetId();
	int nConfirm=0;
	CUnimapOnline* pUnimapOnline = m_pUnimap->m_pUnimapOnline;

	// check by action id type
	if( nActionId == wxID_MENU_IMAGE_UPLOAD || nActionId == wxID_TOOL_ONLINE_UPLOAD )
	{
		if( !m_pAstroImage ) return;

		nConfirm = wxMessageBox( wxT("Do you want to upload astro-image?"), wxT("Confirm"), wxYES_NO|wxICON_QUESTION, this );
		if( nConfirm == wxYES )
		{
			// set image/group  pointers for upload
			pUnimapOnline->m_pUploadAstroImage = m_pAstroImage;
			pUnimapOnline->m_pUploadImageGroup = m_pImageDb->m_pCurrentGroup;

			// set upload action
			DefCmdAction act;
			act.id = THREAD_ACTION_UPLOAD_IMAGE;
			//m_pUnimapWorker->SetAction( cmd );

			/////////////////
			// show wait dialog
			CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Upload Image"), 
								wxT("Process Files ..."), 1, m_pUnimapWorker, &act );
			pWaitDialog->ShowModal();
			// delete/reset wait dialog
			delete( pWaitDialog );
		}

		if( nActionId == wxID_TOOL_ONLINE_UPLOAD ) GetToolBar()->ToggleTool( wxID_TOOL_ONLINE_UPLOAD, 0 );

/*		char strFile[500];
		char strImageName[500];
		m_pAstroImage->GetFullName( strFile );
		m_pAstroImage->GetImageName( strImageName );

		// popup wait dialog
		m_pWaitDialog = new CWaitDialog( this, "Upload process", "Uploading file...", 3 );
		m_pWaitDialog->Show();

		// set handler for progress bar
		pUnimapOnline->SetHandler( this, 1 );

		// call to upload current image/with current group
		pUnimapOnline->UploadImage( strFile, m_pImageDb->m_pCurrentGroup->m_strGroupNameId, strImageName );

		// close wait dialog
		delete( m_pWaitDialog );
		m_pWaitDialog = NULL;
*/
	} else if( nActionId == wxID_MENU_GROUP_UPLOAD || nActionId == wxID_TOOL_ONLINE_UPLOAD_GROUP )
	{
		nConfirm = wxMessageBox( wxT("Do you want to upload all images in the group?"), wxT("Confirm"), wxYES_NO|wxICON_QUESTION, this );
		if( nConfirm == wxYES )
		{
			pUnimapOnline->m_pUploadImageGroup = m_pImageDb->m_pCurrentGroup;
			// add action to the worker queue
			DefCmdAction act;
			act.id = THREAD_ACTION_UPLOAD_GROUP;
			//m_pUnimapWorker->SetAction( cmd );

			/////////////////
			// show wait dialog
			CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Upload Group Images"), 
								wxT("Process Files ..."), 1, m_pUnimapWorker, &act );
			pWaitDialog->ShowModal();
			// delete/reset wait dialog
			delete( pWaitDialog );
		}

		if( nActionId == wxID_TOOL_ONLINE_UPLOAD_GROUP ) GetToolBar()->ToggleTool( wxID_TOOL_ONLINE_UPLOAD_GROUP, 0 );

	} else if( nActionId == wxID_MENU_FILE_ONLINE_UPLOAD_CUSTOM )
	{
		// display the cutom upload selection dialog
		CUploadCustomDlg* pDlg = new CUploadCustomDlg( this, _("Custom Upload") );
		pDlg->SetConfig( pUnimapOnline );
		if( pDlg->ShowModal( ) == wxID_OK )
		{
		}
		delete( pDlg );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnOnlineBrowse
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnOnlineBrowse( wxCommandEvent& pEvent )
{
	int nActionId = pEvent.GetId();
	CUnimapOnline* pUnimapOnline = m_pUnimap->m_pUnimapOnline;
	int nSelectGroup = m_pGroupSelect->GetSelection();

	// display the cutom upload selection dialog
	COnlineBrowseDlg* pDlg = new COnlineBrowseDlg( this, _("Online Browse Images") );
	pDlg->SetConfig( pUnimapOnline, m_pUnimapWorker, m_pImageDb, nSelectGroup );
	pDlg->ShowModal( );
	// if update flag do update
	if( pDlg->m_bNewImageToGroup )
	{
		// update the main frame list of images
		m_pImageDb->SelectGroup( nSelectGroup );
		// update thumbnails if visible
		if( m_pMainConfig->m_bShowImageThumbs ) UpdateGroupThumbnails( nSelectGroup );
	}
	delete( pDlg );
	
	// if toolbar button pop back out
	if( nActionId == wxID_TOOL_ONLINE_BROWSE ) GetToolBar()->ToggleTool( wxID_TOOL_ONLINE_BROWSE, 0 );

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnToolsSlideShow( wxCommandEvent& pEvent )
{
	// delete if exist
	DeleteSlideShowTimer( );

	// check if option checked - else do nothing
	if( pEvent.IsChecked() )
	{
		////////////
		// show slideshow setup dialog
		CSlideShowDlg dlg( this, wxT("Setup Slide-Show"), m_pImageDb, m_pMainConfig );
		if( dlg.ShowModal( ) == wxID_OK )
		{
			//////
			// set all the data from dialog in main config
			m_pMainConfig->m_nSlideShowSources = dlg.m_pSources->GetSelection();
			m_pMainConfig->m_nSlideShowOrder = dlg.m_pOrder->GetSelection();
			m_pMainConfig->m_nSlideShowDelay = dlg.m_pShowTime->GetValue();
			m_pMainConfig->m_nSlideShowRepeat = dlg.m_pRepeat->GetValue();
			m_pMainConfig->m_nSlideShowFullScreen = dlg.m_pFullScreen->GetValue();

			///////
			// init image db
			m_pImageDb->InitImageList( m_pMainConfig->m_nSlideShowSources, m_pMainConfig->m_nSlideShowOrder );
			m_bSlideLoading = 0;
			m_nSlideShowStatus = 1;

			// disable selects
			m_pGroupSelect->Disable();
			m_pImageSelect->Disable();

			///////////////////
			// start timer
			m_pSlideShowTimer = new wxTimer( this->GetEventHandler(), wxID_SLIDESHOW_TIMER );
			// default 5 seconds - in milliseconds ... see factor
			m_pSlideShowTimer->Start( m_pMainConfig->m_nSlideShowDelay*1000 );
			// if full screen
			if( m_pMainConfig->m_nSlideShowFullScreen ) SetFullScreen( );

		} else
		{
			wxMenu* pViewMenu = GetMenuBar()->GetMenu( 4 );
			pViewMenu->Check( wxID_MENU_TOOLS_SLIDE_SHOW, 0 );
		}

	} else
	{
		StopSlideShow( );
	}
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::DeleteSlideShowTimer( )
{
	// if timer set stop and delete
	if( m_pSlideShowTimer )
	{
		m_pSlideShowTimer->Stop();

		// wait to finish loading
//		while( m_bSlideLoading ) Sleep( 100 ); 

		delete( m_pSlideShowTimer );
		m_nSlideShowStatus = 0;
		m_pSlideShowTimer = NULL;
	}
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::StopSlideShow( )
{
	DeleteSlideShowTimer( );
	// enable selects
	m_pGroupSelect->Enable();
	m_pImageSelect->Enable();
	// set image selection on last item
	m_pGroupSelect->SetSelection( m_nCurrentGroup );
	m_pImageDb->SelectGroup( m_nCurrentGroup );
	m_pImageSelect->SetSelection( m_nCurrentImage+1 );

	wxMenu* pViewMenu = GetMenuBar()->GetMenu( 4 );
	pViewMenu->Check( wxID_MENU_TOOLS_SLIDE_SHOW, 0 );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnSlideShowTimer( wxTimerEvent& pEvent )
{
	// if current image still loading or pause
	if( m_bSlideLoading || m_nSlideShowStatus == 2 ) return;

	// get necxt frame
	GetNextSlide();
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::GetNextSlide( int bDirection )
{
	int bRet=0;

	// get next image id
	if( bDirection == 0 )
		bRet = m_pImageDb->GetNextImageFromList( m_nCurrentGroup, m_nCurrentImage );
	else
		bRet = m_pImageDb->GetPreviousImageFromList( m_nCurrentGroup, m_nCurrentImage );

	if( bRet )
	{
		// call to free
		if( m_pAstroImage )
		{
			m_pAstroImage->FreeData( );
			m_pAstroImage = NULL;
		}
		m_pImgView->FreeAll( );

		// set flag to loading
		m_bSlideLoading = 1;
		// Set to start loading image via worker
		LoadImageBG( m_nCurrentGroup, m_nCurrentImage );
		// set in gui
//		SetAstroImageView( );

	} else if( m_pMainConfig->m_nSlideShowRepeat )
	{
		// set image list counter back to zero
		if( bDirection == 0 )
			m_pImageDb->m_nImageListCurrent = 0;
		else
			m_pImageDb->m_nImageListCurrent = m_pImageDb->m_vectImageList.size()+1;

	} else
	{
		// stop slideshow
		StopSlideShow( );
		// if full screen return back
		if( m_bIsFullScreen ) SetFullScreen( );
	}

}

////////////////////////////////////////////////////////////////////
// Method:	OnToolsWeather
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnToolsWeather( wxCommandEvent& pEvent )
{
	// pop up dialog window 
	CWeatherVisDlg *pWeatherDlg = new CWeatherVisDlg( this, wxT("Visualize :: Weather Satellite Image"), m_pUnimapWorker );
	pWeatherDlg->SetConfig( m_pMainConfig );

	///////////
	// prepare action command
	DefCmdAction act;
	act.id = THREAD_ACTION_WEATHER_SATELLITE_IMAGE;
	act.vectObj[0] = (void*) pWeatherDlg;

	/////////////////
	// show wait dialog
	CWaitDialog waitDialog( this, wxT("Wait..."), wxT("Fetch Remote Satellite Image..."), 0, m_pUnimapWorker, &act );
	waitDialog.ShowModal();

	// set remote image
	pWeatherDlg->SetRemoteImage( );
	// show dialog
	pWeatherDlg->ShowModal( );

	// delete object
	delete( pWeatherDlg );

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnToolsSunView( wxCommandEvent& pEvent )
{
	// pop up dialog window 
	CSunViewDlg *pSunViewDlg = new CSunViewDlg( this, wxT("Sun Viewer"), m_pUnimapWorker );
	pSunViewDlg->SetConfig( m_pMainConfig );

	///////////
	// prepare action command
	DefCmdAction act;
	act.id = THREAD_ACTION_WEATHER_SUN_VIEW_IMAGE;
	act.vectObj[0] = (void*) pSunViewDlg;

	/////////////////
	// show wait dialog
	CWaitDialog waitDialog( this, wxT("Wait..."), wxT("Fetch Remote Sun Image..."), 0, m_pUnimapWorker, &act );
	waitDialog.ShowModal();

	// set remote image
	pSunViewDlg->SetRemoteImage();
	// check if return is ok
	pSunViewDlg->ShowModal();

	// delete object
	delete( pSunViewDlg );

	return;
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnWeatherCurrentForecast( wxCommandEvent& pEvent )
{
	wxString strTmp;

	// todo: here insert site id default - or current selected ???
	int nDefSiteId = m_pUnimap->m_pObserver->m_nDefaultSite;
	if( nDefSiteId < 0 ) nDefSiteId = 0;
	CObserverSite* pSite = m_pUnimap->m_pObserver->m_vectSite[nDefSiteId];
//	CWeather* pWeather = m_pUnimap->m_pObserver->m_vectSite[0]->m_pWeather;

	///////////////
	// check if weather for current site(0) is to update
	if( pSite->IsForecast_ToUpdate() )
	{
		///////////
		// prepare action command
		DefCmdAction act;
		act.id = THREAD_ACTION_WEATHER_FORECAST_ALL_GET;
		act.vectInt[0] = nDefSiteId;

		/////////////////
		// show wait dialog
		CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Wait..."),
								wxT("Updating weather current and forecast data..."), 0, m_pUnimapWorker, &act );
		pWaitDialog->ShowModal();
		delete( pWaitDialog );
	}

	/////////////
	// create the dlg
	strTmp.Printf( wxT("%s/%s :: Current Weather/Forecast"), pSite->m_strCity, pSite->m_strCountry );
	CWeatherCForecastDlg* pDlg = new CWeatherCForecastDlg( this, strTmp, pSite, m_pUnimapWorker );
	pDlg->SetConfig( m_pUnimap->m_pObserver, nDefSiteId, m_pUnimapWorker );

	pDlg->ShowModal();
	delete( pDlg );

}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnWeatherSeeingPredict( wxCommandEvent& pEvent )
{
	if( !m_pUnimap->m_pObserver->m_vectSite ) return;

	char strTmp[255];

	// todo: here insert site id default - or current selected ???
	int nDefSiteId = m_pUnimap->m_pObserver->m_nDefaultSite;
	if( nDefSiteId < 0 ) nDefSiteId = 0;
	CObserverSite* pSite = m_pUnimap->m_pObserver->m_vectSite[0];
	CWeather* pWeather = pSite->m_pWeather;
	double lat = pSite->m_nLatitude;
	double lon = pSite->m_nLongitude;

	////////////////////
	// Get best seeing prediction for this site
	int engine = pWeather->GetBestSeeingPredictEngine( lat, lon );

	///////////////
	// check if weather for current site(0) is to update
	if( pWeather->IsSeeing_ToUpdate( lat, lon, engine ) )
	{
		///////////
		// prepare action command
		DefCmdAction act;
		act.id = THREAD_ACTION_WEATHER_SEEING_PREDICT_GET;
		// site id
		act.vectInt[0] = nDefSiteId;
		// engine id
		act.vectInt[1] = engine;

		/////////////////
		// show wait dialog
		CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Fetch Seeing Prediction Data"),
								wxT("Updating weather seeing prediction data..."), 0, m_pUnimapWorker, &act );
		pWaitDialog->ShowModal();
		delete( pWaitDialog );

	} else
	{
		// call to get current seeing prediction - local cache
		pSite->GetSeeingPrediction( engine );
	}


	/////////////
	// create the dlg
	CWeatherSeeingDlg* pDlg = new CWeatherSeeingDlg( this, wxT("Seeing Prediction") );
	pDlg->SetConfig( m_pUnimap->m_pObserver, 0, engine, m_pUnimapWorker );

	if( pDlg->ShowModal() == wxID_OK )
	{
		// todo: something here ??? save last used view ?
	}
	delete( pDlg );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnToolsLightPollution( wxCommandEvent& pEvent )
{
	char strTmp[255];

	int nDefSiteId = m_pUnimap->m_pObserver->m_nDefaultSite;
	if( nDefSiteId < 0 ) nDefSiteId = 0;

	CLightPollutionDlg* pDlg = new CLightPollutionDlg( this, wxT("Light Pollution Map"), m_pUnimapWorker );
	pDlg->SetConfig( m_pUnimap->m_pObserver, nDefSiteId );

	///////////
	// SET THREAD TO LOAD
	DefCmdAction act;
	act.id = THREAD_ACTION_ENV_LIGHT_POLLUTION_LOAD;
	// site id
	act.vectObj[0] = (void*) pDlg;

	/////////////////
	// show wait dialog
	CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Wait..."), wxT("Loading Light Pollution Data..."), 0, m_pUnimapWorker, &act );
	pWaitDialog->ShowModal();
	// delete/reset wait dialog
	delete( pWaitDialog );

	// show dialog
	if( pDlg->ShowModal() == wxID_OK )
	{
		// todo: something here ??? save last used view ?
	}
	delete( pDlg );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnToolsObservatories( wxCommandEvent& pEvent )
{
	CObservatoriesDlg* pDlg = new CObservatoriesDlg( this, wxT("Astronomical Observatories Map") ); //, m_pUnimapWorker );

	///////////
	// SET THREAD TO LOAD
	DefCmdAction act;
	act.id = THREAD_ACTION_ENV_OBSERVATORIES_LOAD;
	// site id
	act.vectObj[0] = (void*) pDlg;
	/////////////////
	// show wait dialog
	CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Wait..."), wxT("Loading Observatories Data..."), 0, m_pUnimapWorker, &act );
	pWaitDialog->ShowModal();
	// delete/reset wait dialog
	delete( pWaitDialog );

	pDlg->SetConfig( );
	// show dialog
	pDlg->ShowModal();

	delete( pDlg );
}

////////////////////////////////////////////////////////////////////
void CGUIFrame::OnNewsTool( wxCommandEvent& pEvent )
{
	CNews* pNews = new CNews();

	///////////
	// SET THREAD TO LOAD
	DefCmdAction act;
	act.id = THREAD_ACTION_NEWS_FETCH_HEADLINES;
	// fetch type by edefault 0 -  todo: plug in here preferences
	act.vectInt[0] = 0;
	// site id
	act.vectObj[0] = (void*) pNews;

	/////////////////
	// show wait dialog
	CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Wait..."), wxT("Fetching News Headlines..."), 0, m_pUnimapWorker, &act );
	pWaitDialog->ShowModal();
	// delete/reset wait dialog
	delete( pWaitDialog );

	// create dialog
	CNewsDlg *pDlg = new CNewsDlg( this, wxT("News Headlines") );
	pDlg->SetConfig( pNews );
	// show news dialog/frame
	pDlg->Show( ); 

	// delete objects
//	delete( pNews );
}

// show units conversion dialog
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnCalcUnits( wxCommandEvent& pEvent )
{
	CCalcUnitsDlg dlg( this, wxT("Units Conversion") );
	// set defaults
	dlg.SetFromTo( );
	dlg.ShowModal();
}

// show optical system calcultor dialog
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnCalcOptical( wxCommandEvent& pEvent )
{
	CTLensSelectDlg dlg( this, wxT("Optical System"), 1 );
	// create a lens setup with zero
	DefTLensSetup rLensSetup;
	memset( &rLensSetup, 0, sizeof(DefTLensSetup) );
	dlg.SetData( m_pObserver, m_pCamera, m_pTelescope, 0, rLensSetup );
	dlg.ShowModal();
}

////////////////////////////////////////////////////////////////////
// Method:	OnInstrumentsControl
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnInstrumentsControl( wxCommandEvent& pEvent )
{
	// pop up dialog window 
/*	CInstrCtrlDlg *pDlg = new CInstrCtrlDlg( this, wxT("Instruments Control Panel") );
	pDlg->SetConfig( m_pUnimap->m_pTelescope, m_pUnimap->m_pFocuser, m_pUnimap->m_pCamera, 
						m_pUnimap->m_pObserver, m_pUnimap->m_pSky, m_pUnimapWorker );

	// check if return is ok
	pDlg->Show( ); 
*/
	///////////
	// new way ...
	if( !m_bInstrActive )
	{
		// set view type instruments
		m_pImgView->m_nViewType = VIEW_TYPE_INSTRUMENTS;
		// set camera flag active - todo: maybe a better more precise way
		m_pImgView->m_bIsCamera = 1;
		// show intruments toolbar
		m_pInstrToolBar->Show();
		AddInstrumentsPanel( );
		// set instruments configuration
		SetIntrumentsConfig( m_pUnimap->m_pTelescope, m_pUnimap->m_pFocuser, 
							m_pUnimap->m_pCamera, m_pUnimap->m_pObserver );

		m_pGraphPanel->Show();
		m_pCamTelPanel->Show();
		m_pGraphPanel->Layout();
		m_pCamTelPanel->Layout();
		m_bInstrActive = 1;

	} else
	{
		m_pImgView->m_bIsCamera = 0;
		m_pGraphPanel->Hide();
		m_pCamTelPanel->Hide();
		DeleteInstrumentsPanel( );
		m_pInstrToolBar->Hide();
		m_bInstrActive = 0;
		// set view type back to image
		m_pImgView->m_nViewType = VIEW_TYPE_IMAGE;
	}

	// delete object
//	delete( pDlg );

	//Fit();
//	sizerGroupAndImage->Layout();
	GetSizer()->Layout();
	Layout();

//	Update();
	Refresh(false);

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnImagesDrop
// Class:		CGUIFrame
// Purpose:		process dropped files
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnImagesDrop( wxDropFilesEvent& pEvent )
{
	int i = 0;
	wxString strMsg=wxT("");
	wxString strName=wxT("");
	wxString strPath=wxT("");
	wxString strFullName=wxT("");
	wxString strIconPath=wxT("");
	wxString strWorkPath=wxT("");
	wxString strArgs=wxT("");
	int nType = -1;
	int nLastImageId=-1;

	int nFiles = pEvent.GetNumberOfFiles();
	wxString* vectFiles = pEvent.GetFiles();

	int nCount = 0;
	// for ever file droped
	for( i=0; i<nFiles; i++ )
	{
		// init
		strName = wxT("");
		strFullName = wxT("");
		strPath = wxT("");

		// process file
		int nFileState = m_pUnimap->ProcessInputFile( vectFiles[i], strName, strFullName, strPath, 
										strWorkPath, strIconPath, strArgs, nType );
		// if type is wrong
		if( nFileState == 0 ) continue;

		// logs
		strMsg.Printf( wxT("INFO :: added image=%s"), strName );
		m_pLogger->Log( strMsg );

		/////////////////////
		// add image
		nLastImageId = m_pImageDb->m_pCurrentGroup->AddImage( strFullName );

		// increment counter
		nCount++;
	}

	// save if there are any items and select last
	if( nCount > 0 && nLastImageId >= 0 ) 
	{
		// reselect group
		int nSelectGroup = m_pGroupSelect->GetSelection();
		m_pImageDb->SelectGroup( nSelectGroup );
		if( m_pMainConfig->m_bShowImageThumbs ) UpdateGroupThumbnails( nSelectGroup );

		// select image in group
		m_pImageSelect->SetSelection( nLastImageId+1 );
		LoadSelectedImage( );
	}
}

////////////////////////////////////////////////////////////////////
// Method:	SetFullScreen
// Class:	CDynamicImgView
// Purose:	set my image full screen
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetFullScreen( )
{
	if( !m_pImgView->m_bHasImage ) return;

	int w, h;

	if( m_bIsFullScreen  == 0 )
	{
		m_pImageBox->Hide();
		m_pLoggerView->Hide();
		m_bIsFullScreen = 1;

	} else
	{
		m_pImageBox->Show();
		if( m_pMainConfig->m_bLogsView ) m_pLoggerView->Show();
		m_bIsFullScreen = 0;
	}

	// call to toplevelwin method
	ShowFullScreen( m_bIsFullScreen );

	return;
}
