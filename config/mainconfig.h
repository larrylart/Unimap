////////////////////////////////////////////////////////////////////
// Package:		config definition
// File:		config.h
// Purpose:		manage software/hardware configuration
//
// Created by:	Larry Lart on 24-06-2007
// Updated by:	Larry Lart on 06-05-2009
//
// Copyright:	(c) 2007-2009 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _CONFIG_MAIN_H
#define _CONFIG_MAIN_H

#include "wx/wxprec.h"
#include <wx/listctrl.h>
#include <wx/fontdlg.h>

// units format
#define UNITS_FORMAT_SEXAGESIMAL	0
#define UNITS_FORMAT_DECIMAL		1

//////////////
// path shape types
#define DYNAMIC_OBJ_PATH_SHAPE_TYPE_LINE				0
#define DYNAMIC_OBJ_PATH_SHAPE_TYPE_SPLINE_DEFAULT		1
#define DYNAMIC_OBJ_PATH_SHAPE_TYPE_SPLINE_CATMULLROM	2
// path line type
#define GRAPHICS_LINE_STYLE_NORMAL		0
#define GRAPHICS_LINE_STYLE_DOTTED		1
#define GRAPHICS_LINE_STYLE_LDOTTED		2

// define sections ids
enum
{
	SECTION_ID_GUI, SECTION_ID_STYLE, SECTION_ID_ACTION, SECTION_ID_DETECTION,
	SECTION_ID_MATCHING, SECTION_ID_CATALOG, SECTION_ID_HINT, SECTION_ID_GRAHIPCS,
	SECTION_ID_GUI3D, SECTION_ID_SLIDESHOW, SECTION_ID_APPLICATION, SECTION_ID_SERVICES
};

/////////////////////////////
// define Section :: Detection - var IDs
#define CONF_DETECTION_PROFILE			0

//////////////
// autos
enum
{
	CONF_SERVICES_SERVER_RUN,
	CONF_SERVICES_SERVER_IP,
	CONF_SERVICES_SERVER_PORT,
	CONF_SERVICES_PROCESSING_QUEUE,
	CONF_SERVICES_PROCESSING_PIORITY,
	CONF_SERVICES_CLUSTER_NODE_ID,
	CONF_SERVICES_CLUSTER_MASTER_IP,
	CONF_SERVICES_CLUSTER_MASTER_PORT
};

/////////////////////////////
// define Section :: Application
enum
{
	CONF_APPLICATION_ID = 0,
	CONF_APPLICATION_VER
};

/////////////////////////////
// define Section :: Matching - var IDs
enum
{
	// :: search
	CONF_MATCHING_SEARCH_METHOD,
	CONF_MATCHING_SEARCH_AREA_INCREMENT_RA,
	CONF_MATCHING_SEARCH_AREA_INCREMENT_DEC,
	CONF_MATCHING_SEARCH_OVERLAP_DIV_RA,
	CONF_MATCHING_SEARCH_OVERLAP_DIV_DEC,
	CONF_MATCHING_SEARCH_ZOOM_STEPS,
	CONF_MATCHING_SEARCH_ZOOM_START,
	CONF_MATCHING_REVERSE_ZOOM_SEARCH,
	// :: objects
	CONF_MATCHING_IMG_OBJ_EXTRACT_BY_OBJ_SHAPE,
	CONF_MATCHING_IMG_OBJ_EXTRACT_BY_DETECT_TYPE,
	CONF_MATCHING_IMG_OBJ_NO,
	CONF_MATCHING_CAT_OBJ_NO,
	CONF_MATCHING_USE_MIN_CAT_STARS,
	CONF_MATCHING_MATCH_MIN_STARS,
	CONF_MATCHING_FINETUNE_IMAGE_STARS_FACT,
	CONF_MATCHING_FINETUNE_CATALOG_STARS_FACT,
	// :: match
	CONF_MATCHING_TRANS_TYPE,
	CONF_MATCHING_RECALC,
	CONF_MATCHING_MAX_DIST,
	CONF_MATCHING_TRIANG_FIT,
	CONF_MATCHING_TRIANG_SCALE,
	CONF_MATCHING_MAX_ITER,
	CONF_MATCHING_HALT_SIGMA,
	// :: proc
	CONF_MATCHING_PROJECTION_METHOD,
	CONF_MATCHING_DO_FINETUNE,
	CONF_MATCHING_DO_FINETUNE_WITH_PRJ,
	CONF_MATCHING_MATCH_BY_MOST_STARS,
	CONF_MATCHING_MATCH_BY_BEST_FIT,
	CONF_MATCHING_REFINE_BEST_AREA,
	CONF_MATCHING_STOP_ON_FIRST_FOUND,
	CONF_MATCHING_MIN_FOUND_NEEDED,
	CONF_MATCHING_ASSOC_MAX_DISTANCE,
	// :: finder
	CONF_MATCHING_LOCATE_CAT_STARS,
	CONF_MATCHING_LOCATE_DSO,
	CONF_MATCHING_LOCATE_XGAMMA,
	CONF_MATCHING_LOCATE_RADIO,
	CONF_MATCHING_LOCATE_SN,
	CONF_MATCHING_LOCATE_BH,
	CONF_MATCHING_LOCATE_MSTARS,
	CONF_MATCHING_LOCATE_EXOPLANETS,
	CONF_MATCHING_LOCATE_ASTEROIDS,
	CONF_MATCHING_LOCATE_COMETS,
	CONF_MATCHING_LOCATE_AES,
	CONF_MATCHING_LOCATE_SOLAR,
	CONF_MATCHING_FETCH_OBJ_DETAILS_ON_FIND,
	// :: solar
	CONF_MATCHING_LOCATE_TIMEFRAME_RANGE,
	CONF_MATCHING_LOCATE_TIMEFRAME_DIV,
	CONF_MATCHING_LOCATE_USE_TLE_IN_RANGE,
	CONF_MATCHING_LOCATE_TLE_RANGE_VALUE
};

//////////////////////////
// define Section :: CATALOGS - var IDs
enum
{
	// :: stars
	CONF_STARS_CATALOG_TO_USE,
	CONF_LOAD_STARS_CATALOG_REGION,
	CONF_STARS_CATALOG_MAG_LIMIT,
	CONF_LOAD_STARS_CATALOG_REMOTE,
	CONF_LOAD_STARS_CATALOG_REMOTE_LOCATION,
	// :: dso
	CONF_DSO_CATALOG_TO_USE,
	CONF_LOAD_DSO_CATALOG_REGION,
	CONF_LOAD_DSO_CATALOG_REMOTE,
	CONF_LOAD_DSO_CATALOG_REMOTE_LOCATION,
	CONF_DSO_CATALOG_MESSIER,
	CONF_DSO_CATALOG_NGC,
	CONF_DSO_CATALOG_IC,
	// :: radio
	CONF_RADIO_CATALOG_TO_USE,
	CONF_LOAD_RADIO_CATALOG_REGION,
	CONF_LOAD_RADIO_CATALOG_REMOTE,
	CONF_LOAD_RADIO_CATALOG_REMOTE_LOCATION,
	// :: xray/gamma
	CONF_XGAMMA_CATALOG_TO_USE,
	CONF_LOAD_XGAMMA_CATALOG_REGION,
	CONF_LOAD_XGAMMA_CATALOG_REMOTE,
	CONF_LOAD_XGAMMA_CATALOG_REMOTE_LOCATION,
	// :: supernovas
	CONF_SN_CATALOG_TO_USE,
	CONF_LOAD_SN_CATALOG_REGION,
	CONF_LOAD_SN_CATALOG_REMOTE,
	CONF_LOAD_SN_CATALOG_REMOTE_LOCATION,
	// :: blackholes
	CONF_BH_CATALOG_TO_USE,
	CONF_LOAD_BH_CATALOG_REGION,
	CONF_LOAD_BH_CATALOG_REMOTE,
	CONF_LOAD_BH_CATALOG_REMOTE_LOCATION,
	// :: multiple/double stars
	CONF_MS_CATALOG_TO_USE,
	CONF_LOAD_MS_CATALOG_REGION,
	CONF_LOAD_MS_CATALOG_REMOTE,
	CONF_LOAD_MS_CATALOG_REMOTE_LOCATION,
	// :: extra-solar planets
	CONF_EXP_CATALOG_TO_USE,
	CONF_LOAD_EXP_CATALOG_REGION,
	CONF_LOAD_EXP_CATALOG_REMOTE,
	CONF_LOAD_EXP_CATALOG_REMOTE_LOCATION,
	// :: asteroids
	CONF_AST_CATALOG_TO_USE,
	CONF_LOAD_AST_CATALOG_REGION,
	CONF_LOAD_AST_CATALOG_REMOTE,
	CONF_LOAD_AST_CATALOG_REMOTE_LOCATION,
	// :: comets
	CONF_COM_CATALOG_TO_USE,
	CONF_LOAD_COM_CATALOG_REGION,
	CONF_LOAD_COM_CATALOG_REMOTE,
	CONF_LOAD_COM_CATALOG_REMOTE_LOCATION,
	// :: artificial earth satellites
	CONF_AES_CATALOG_TO_USE,
	CONF_LOAD_AES_CATALOG_USE_TIMEFRAME,
	CONF_LOAD_AES_CATALOG_REGION,
	CONF_LOAD_AES_CATALOG_REMOTE,
	CONF_LOAD_AES_CATALOG_REMOTE_LOCATION,
	// - new
	CONF_LOAD_AES_CATALOG_REMOTE_UPDATE_LOCAL,
	CONF_LOAD_AES_CATALOG_OBJ_CATEGORY,
	CONF_LOAD_AES_CATALOG_OBJ_TYPE,
	CONF_LOAD_AES_CATALOG_DONT_USE_TYPE,
	CONF_LOAD_AES_CATALOG_USE_DATE_START,
	CONF_LOAD_AES_CATALOG_DATE_START,
	CONF_LOAD_AES_CATALOG_USE_DATE_END,
	CONF_LOAD_AES_CATALOG_DATE_END
};

////////////////////////////////
// define Section :: Hint - var IDs
enum
{
	CONF_HINT_USE_FIELD_TYPE,
	// camera
	CONF_HINT_CAMERA_OWN,
	CONF_HINT_CAMERA_TYPE,
	CONF_HINT_CAMERA_BRAND,
	CONF_HINT_CAMERA_NAME,
	CONF_HINT_SENSOR_WIDTH,
	CONF_HINT_SENSOR_HEIGHT,
	// telescope/camera lens
	CONF_HINT_TLENS_OWN,
	CONF_HINT_TLENS_TYPE,
	CONF_HINT_TLENS_TYPE_OF_TYPE,
	CONF_HINT_TLENS_BRAND,
	CONF_HINT_TLENS_NAME,
	CONF_HINT_FOCAL_LENGTH,
	// field
	CONF_HINT_FIELD_UNITS,
	CONF_HINT_FIELD_WIDTH,
	CONF_HINT_FIELD_HEIGHT
};

/////////////////////////////////////////////////////////
//define Section :: Gui - var IDs
enum
{
	CONF_GUI_WIDTH,
	CONF_GUI_HEIGHT,
	CONF_GUI_ORIG_X,
	CONF_GUI_ORIG_Y,
	CONF_LOGS_VIEW,
	CONF_ADD_FILE_PATH,
	CONF_LAST_GROUP,
	CONF_LAST_GROUP_IMAGE,
	// view :: objects
	CONF_VIEW_OBJECTS_TYPE,
	CONF_VIEW_DETECTED_OBJECTS,
	CONF_VIEW_DETECTED_OBJECTS_NO_MATCH,
	CONF_VIEW_CATALOG_STARS,
	CONF_VIEW_CATALOG_DSO,
	CONF_VIEW_CATALOG_XGAMMA,
	CONF_VIEW_CATALOG_RADIO,
	CONF_VIEW_CATALOG_MSTARS,
	CONF_VIEW_CATALOG_SUPERNOVAS,
	CONF_VIEW_CATALOG_BLACKHOLES,
	CONF_VIEW_CATALOG_EXOPLANETS,
	CONF_VIEW_CATALOG_ASTERIODS,
	CONF_VIEW_CATALOG_COMETS,
	CONF_VIEW_CATALOG_AES,
	// gui :: constellations
	CONF_CONSTL_VIEW,
	CONF_CONSTL_CULTURE,
	CONF_CONSTL_VERSION,
	CONF_CONSTL_LINES,
	CONF_CONSTL_LABEL,
	CONF_CONSTL_ARTWORK,
	// areas
	CONF_VIEW_IMAGE_AREAS,
	// thumbs
	CONF_VIEW_GROUP_IMAGE_THUMBNAILS,
};

////////////////////////////
//define Section :: GUI 3D - var IDs
enum
{
	// colmap :: light 
	CONF_GUI_3D_COLMAP_LIGHTING_AMBIENT,
	CONF_GUI_3D_COLMAP_LIGHTING_DIFFUSE,
	CONF_GUI_3D_COLMAP_LIGHTING_SPECULAR,
	CONF_GUI_3D_COLMAP_LIGHT_COORD_X,
	CONF_GUI_3D_COLMAP_LIGHT_COORD_Y,
	CONF_GUI_3D_COLMAP_LIGHT_COORD_Z,
	// colmap :: materials
	CONF_GUI_3D_COLMAP_MATERIAL_AMBIENT,
	CONF_GUI_3D_COLMAP_MATERIAL_DIFFUSE,
	CONF_GUI_3D_COLMAP_MATERIAL_SPECULAR,
	CONF_GUI_3D_COLMAP_MATERIAL_SHININESS,
	CONF_GUI_3D_COLMAP_MATERIAL_EMISSION,
	// colmap :: structure
	CONF_GUI_3D_COLMAP_STRUCT_SURFACE_WIDTH,
	CONF_GUI_3D_COLMAP_STRUCT_SURFACE_HEIGHT,
	CONF_GUI_3D_COLMAP_STRUCT_GRID_SPACER,
	CONF_GUI_3D_COLMAP_STRUCT_SURFACE_SPACER,
	CONF_GUI_3D_COLMAP_STRUCT_Z_FACTOR,
	CONF_GUI_3D_COLMAP_STRUCT_GRID_LABELS,
	CONF_GUI_3D_COLMAP_STRUCT_SURFACE_LABELS,
	CONF_GUI_3D_COLMAP_STRUCT_SHOW_AXIS,
	CONF_GUI_3D_COLMAP_STRUCT_SHOW_PROJECTIONS,
	// space :: light 
	CONF_GUI_3D_SPACE_LIGHTING_AMBIENT,
	CONF_GUI_3D_SPACE_LIGHTING_DIFFUSE,
	CONF_GUI_3D_SPACE_LIGHTING_SPECULAR,
	CONF_GUI_3D_SPACE_LIGHT_COORD_X,
	CONF_GUI_3D_SPACE_LIGHT_COORD_Y,
	CONF_GUI_3D_SPACE_LIGHT_COORD_Z,
	// space :: materials
	CONF_GUI_3D_SPACE_MATERIAL_AMBIENT,
	CONF_GUI_3D_SPACE_MATERIAL_DIFFUSE,
	CONF_GUI_3D_SPACE_MATERIAL_SPECULAR,
	CONF_GUI_3D_SPACE_MATERIAL_SHININESS,
	CONF_GUI_3D_SPACE_MATERIAL_EMISSION,
	// space :: structure
	CONF_GUI_3D_SPACE_STRUCT_SURFACE_WIDTH,
	CONF_GUI_3D_SPACE_STRUCT_SURFACE_HEIGHT,
	CONF_GUI_3D_SPACE_STRUCT_GRID_SPACER,
	CONF_GUI_3D_SPACE_STRUCT_SURFACE_SPACER,
	CONF_GUI_3D_SPACE_STRUCT_Z_FACTOR,
	CONF_GUI_3D_SPACE_STRUCT_GRID_LABELS,
	CONF_GUI_3D_SPACE_STRUCT_SURFACE_LABELS,
	CONF_GUI_3D_SPACE_STRUCT_SHOW_AXIS,
	CONF_GUI_3D_SPACE_STRUCT_SHOW_PROJECTIONS,
	// dist :: light 
	CONF_GUI_3D_DIST_LIGHTING_AMBIENT,
	CONF_GUI_3D_DIST_LIGHTING_DIFFUSE,
	CONF_GUI_3D_DIST_LIGHTING_SPECULAR,
	CONF_GUI_3D_DIST_LIGHT_COORD_X,
	CONF_GUI_3D_DIST_LIGHT_COORD_Y,
	CONF_GUI_3D_DIST_LIGHT_COORD_Z,
	// dist :: materials
	CONF_GUI_3D_DIST_MATERIAL_AMBIENT,
	CONF_GUI_3D_DIST_MATERIAL_DIFFUSE,
	CONF_GUI_3D_DIST_MATERIAL_SPECULAR,
	CONF_GUI_3D_DIST_MATERIAL_SHININESS,
	CONF_GUI_3D_DIST_MATERIAL_EMISSION,
	// dist :: structure
	CONF_GUI_3D_DIST_STRUCT_SURFACE_WIDTH,
	CONF_GUI_3D_DIST_STRUCT_SURFACE_HEIGHT,
	CONF_GUI_3D_DIST_STRUCT_GRID_SPACER,
	CONF_GUI_3D_DIST_STRUCT_SURFACE_SPACER,
	CONF_GUI_3D_DIST_STRUCT_Z_FACTOR,
	CONF_GUI_3D_DIST_STRUCT_GRID_LABELS,
	CONF_GUI_3D_DIST_STRUCT_SURFACE_LABELS,
	CONF_GUI_3D_DIST_STRUCT_SHOW_AXIS,
	CONF_GUI_3D_DIST_STRUCT_SHOW_PROJECTIONS
};

//////////////////////////////////////////////////////////
//define Section :: Style - var IDs
enum
{
	CONF_STYLE_FONT_ENCODING,
	// :: detected
	CONF_STYLE_DFONT_DESC,
	CONF_STYLE_DFONT_COLOR,
	// :: matched
	CONF_STYLE_MFONT_DESC,
	CONF_STYLE_MFONT_COLOR,
	// :: catalog
	CONF_STYLE_CFONT_DESC,
	CONF_STYLE_CFONT_COLOR,
	// :: nebula
	CONF_STYLE_NEBFONT_DESC,
	CONF_STYLE_NEBFONT_COLOR,
	// :: cluster
	CONF_STYLE_CLUFONT_DESC,
	CONF_STYLE_CLUFONT_COLOR,
	// :: galaxy
	CONF_STYLE_GALFONT_DESC,
	CONF_STYLE_GALFONT_COLOR,
	// :: supernovae
	CONF_STYLE_SNFONT_DESC,
	CONF_STYLE_SNFONT_COLOR,
	// :: black holes
	CONF_STYLE_BHFONT_DESC,
	CONF_STYLE_BHFONT_COLOR,
	// :: radio
	CONF_STYLE_RADFONT_DESC,
	CONF_STYLE_RADFONT_COLOR,
	// :: x-ray
	CONF_STYLE_XRAYFONT_DESC,
	CONF_STYLE_XRAYFONT_COLOR,
	// :: gamma
	CONF_STYLE_GAMMAFONT_DESC,
	CONF_STYLE_GAMMAFONT_COLOR,
	// :: multiple/double stars
	CONF_STYLE_MSFONT_DESC,
	CONF_STYLE_MSFONT_COLOR,
	// :: asteroids
	CONF_STYLE_ASTFONT_DESC,
	CONF_STYLE_ASTFONT_COLOR,
	// :: comets
	CONF_STYLE_COMFONT_DESC,
	CONF_STYLE_COMFONT_COLOR,
	// :: extrasolar planets
	CONF_STYLE_EXPFONT_DESC,
	CONF_STYLE_EXPFONT_COLOR,
	// :: artificial eart satellites
	CONF_STYLE_AESFONT_DESC,
	CONF_STYLE_AESFONT_COLOR,

	/////////
	// and label struct 
	// :: detected
	CONF_STYLE_DOBJ_USE_ID,
	CONF_STYLE_DOBJ_USE_MAG,
	// :: matched
	CONF_STYLE_MOBJ_USE_ID,
	CONF_STYLE_MOBJ_USE_CATNO,
	CONF_STYLE_MOBJ_USE_CNAME,
	// :: catalog
	CONF_STYLE_COBJ_USE_CATNO,
	CONF_STYLE_COBJ_USE_CNAME,
	// :: nebula
	CONF_STYLE_NEBOBJ_USE_CATNO,
	CONF_STYLE_NEBOBJ_USE_CNAME,
	CONF_STYLE_NEBOBJ_USE_TYPE,
	// :: cluster
	CONF_STYLE_CLUOBJ_USE_CATNO,
	CONF_STYLE_CLUOBJ_USE_CNAME,
	CONF_STYLE_CLUOBJ_USE_TYPE,
	// :: galaxy
	CONF_STYLE_GALOBJ_USE_CATNO,
	CONF_STYLE_GALOBJ_USE_CNAME,
	CONF_STYLE_GALOBJ_USE_TYPE,
	// :: supernovae
	CONF_STYLE_SNOBJ_USE_CATNO,
	CONF_STYLE_SNOBJ_USE_CNAME,
	CONF_STYLE_SNOBJ_USE_TYPE,
	// :: black holes
	CONF_STYLE_BHOBJ_USE_CATNO,
	CONF_STYLE_BHOBJ_USE_TYPE,
	// :: radio
	CONF_STYLE_RADOBJ_USE_CATNO,
	CONF_STYLE_RADOBJ_USE_TYPE,
	// :: x-ray
	CONF_STYLE_XRAYOBJ_USE_CATNO,
	CONF_STYLE_XRAYOBJ_USE_TYPE,
	// :: gamma
	CONF_STYLE_GAMMAOBJ_USE_CATNO,
	CONF_STYLE_GAMMAOBJ_USE_TYPE,
	// :: multiple/double stars
	CONF_STYLE_MSOBJ_USE_CATNO,
	CONF_STYLE_MSOBJ_USE_TYPE,
	// :: asteroids
	CONF_STYLE_ASTOBJ_USE_CATNO,
	CONF_STYLE_ASTOBJ_USE_TYPE,
	// :: comets
	CONF_STYLE_COMOBJ_USE_CATNO,
	CONF_STYLE_COMOBJ_USE_TYPE,
	// :: extrasolar planets
	CONF_STYLE_EXPOBJ_USE_CATNO,
	CONF_STYLE_EXPOBJ_USE_TYPE,
	// :: artificial eart satellites
	CONF_STYLE_AESOBJ_USE_CATNO,
	CONF_STYLE_AESOBJ_USE_TYPE,

	/////////////////
	// units and coord
	CONF_STYLE_SKY_COORD_DISPLAY_FORMAT,
	CONF_STYLE_SKY_COORD_ENTRY_FORMAT,
	CONF_STYLE_COORD_SYSTEM,
	CONF_STYLE_GUI_LANGUAGE,
	CONF_STYLE_GUI_THEME,

	//////////////////
	// shape & lines
	// :: detected
	CONF_STYLE_DOBJ_SHAPE_TYPE,
	CONF_STYLE_DOBJ_SHAPE_LINE,
	// :: matched
	CONF_STYLE_MOBJ_SHAPE_TYPE,
	CONF_STYLE_MOBJ_SHAPE_LINE,
	// :: catalog
	CONF_STYLE_COBJ_SHAPE_TYPE,
	CONF_STYLE_COBJ_SHAPE_LINE,
	// :: dso - nebula
	CONF_STYLE_NEBOBJ_SHAPE_TYPE,
	CONF_STYLE_NEBOBJ_SHAPE_LINE,
	// :: dso - clusters
	CONF_STYLE_CLUOBJ_SHAPE_TYPE,
	CONF_STYLE_CLUOBJ_SHAPE_LINE,
	// :: dso - galaxy
	CONF_STYLE_GALOBJ_SHAPE_TYPE,
	CONF_STYLE_GALOBJ_SHAPE_LINE,
	// :: supernovae
	CONF_STYLE_SNOBJ_SHAPE_TYPE,
	CONF_STYLE_SNOBJ_SHAPE_LINE,
	// :: black holes
	CONF_STYLE_BHOBJ_SHAPE_TYPE,
	CONF_STYLE_BHOBJ_SHAPE_LINE,
	// :: radio
	CONF_STYLE_RADOBJ_SHAPE_TYPE,
	CONF_STYLE_RADOBJ_SHAPE_LINE,
	// :: x-ray
	CONF_STYLE_XRAYOBJ_SHAPE_TYPE,
	CONF_STYLE_XRAYOBJ_SHAPE_LINE,
	// :: gamma
	CONF_STYLE_GAMMAOBJ_SHAPE_TYPE,
	CONF_STYLE_GAMMAOBJ_SHAPE_LINE,
	// :: multiple/double stars
	CONF_STYLE_MSOBJ_SHAPE_TYPE,
	CONF_STYLE_MSOBJ_SHAPE_LINE,
	// :: asteroids
	CONF_STYLE_ASTOBJ_SHAPE_TYPE,
	CONF_STYLE_ASTOBJ_SHAPE_LINE,
	// :: comets
	CONF_STYLE_COMOBJ_SHAPE_TYPE,
	CONF_STYLE_COMOBJ_SHAPE_LINE,
	// :: extrasolar planets
	CONF_STYLE_EXPOBJ_SHAPE_TYPE,
	CONF_STYLE_EXPOBJ_SHAPE_LINE,
	// :: artificial eart satellites
	CONF_STYLE_AESOBJ_SHAPE_TYPE,
	CONF_STYLE_AESOBJ_SHAPE_LINE
};

//////////////
// autos
enum
{
	CONF_ACTION_SAVE_ON_DETECTION,
	CONF_ACTION_SAVE_ON_MATCHING,
	CONF_ACTION_SAVE_ON_DETAILS,
	CONF_ACTION_SAVE_ON_AUTOS,
	CONF_ACTION_HIDE_ON_MINIZE
};

///////////////////
// SECTION :: GRAPHICS
enum
{
	// :: object path
	CONF_GRAHIPCS_OBJPATH_LINE_STYLE,
	CONF_GRAHIPCS_OBJPATH_LINE_COLOR,
	CONF_GRAHIPCS_OBJPATH_VERTEX,
	CONF_GRAHIPCS_OBJPATH_STARTEND,
	CONF_GRAHIPCS_OBJPATH_DIRECTION,
	CONF_GRAHIPCS_OBJPATH_BASELINE,
	// :: sky grid
	CONF_GRAHIPCS_SKYGRID_LINE_STYLE,
	CONF_GRAHIPCS_SKYGRID_LINE_COLOR,
	CONF_GRAHIPCS_SKYGRID_SHOW_LABELS,
	CONF_GRAHIPCS_SKYGRID_DRAW_SPLINES,
	CONF_GRAHIPCS_SKYGRID_RA_DIVISIONS,
	CONF_GRAHIPCS_SKYGRID_DEC_DIVISIONS,
	// :: distortion grid
	CONF_GRAHIPCS_DISTGRID_LINE_STYLE,
	CONF_GRAHIPCS_DISTGRID_LINE_COLOR,
	CONF_GRAHIPCS_DISTGRID_DRAW_SPLINES,
	CONF_GRAHIPCS_DISTGRID_RA_DIVISIONS,
	CONF_GRAHIPCS_DISTGRID_DEC_DIVISIONS,
	CONF_GRAHIPCS_DISTGRID_BUMP_FACTOR,
	// :: constellations
	CONF_GRAHIPCS_CONSTL_LINE_STYLE,
	CONF_GRAHIPCS_CONSTL_LINE_COLOR,
	CONF_GRAHIPCS_CONSTL_SHAPE_TYPE,
	CONF_GRAHIPCS_CONSTL_MARK_STARS,
	CONF_GRAHIPCS_CONSTL_OUTLINE,
	CONF_GRAHIPCS_CONSTL_GREEK_LABELS
};

///////////////////
// SECTION :: SLIDESHOW
enum
{
	CONF_SLIDESHOW_SOURCES,
	CONF_SLIDESHOW_ORDER,
	CONF_SLIDESHOW_DELAY,
	CONF_SLIDESHOW_REPEAT,
	CONF_SLIDESHOW_FULLSCREEN
};

#include <wx/arrstr.h>

#include "config.h"

// external classes
//class CConfigView;
//class CConfig;

// CLASS :: CCONFIGMAIN
////////////////////////////////////
class CConfigMain : public CConfig
{
// methods
public:
	CConfigMain( const wxString& strPath, const wxString& strName );
	~CConfigMain( );

	// init/read methods
	void InitApp( );
	void InitServices();
	void InitLocals( );
	void InitLocalConstellations( );
	void InitLocalSky( );
	void InitStyles( );
	void InitGui( );
	void InitMatch( );
	void InitTools( );

	// update/set/save methods
	void UpdateApp( );
	void UpdateLocalConstellations( );
	void UpdateLocalSky( );
	void UpdateStyles( );
	void UpdateGui( );
	void UpdateMatch( );
	void UpdateTools( );

	// get style specifics
	void GetStarStyle( int nMatch, int nType, wxColour &colDraw, wxFont &fontDraw, int &nLineType, int &nShapeType );

//////////////////////
// DATA
public:

	// app
	wxString m_strAppId;
	wxString m_strAppVer;
	double m_nAppLastUpdate;

	// Behaiviour
	int m_bHideOnMinimize;

	// services
	int m_bEnableSoapServices;
	wxString m_strSoapServerIp;
	int m_nSoapServerPort;

	//////////////
	// VIEW :: objects
	int m_bViewObjectsType;
	int m_nViewDetectedObjects;
	int m_bViewDetectedObjectsNoMatch;
	int m_nViewCatalogStars;
	int m_nViewCatalogDso;
	int m_nViewCatalogXGamma;
	int m_nViewCatalogRadio;
	int m_nViewCatalogMStars;
	int m_nViewCatalogSupernovas;
	int m_nViewCatalogBlackholes;
	int m_nViewCatalogExoplanets;
	int m_nViewCatalogAsteroids;
	int m_nViewCatalogComets;
	int m_nViewCatalogAes;
	// gui setting and flags
	int m_nGuiFrameWidth;
	int m_nGuiFrameHeight;
	int m_bLogsView;
	int m_bShowImageThumbs;
	wxString m_strGroupPath;

	/////////////////////////////
	// constellations
	int m_bShowConstellations;
	int m_nConstellationCulture;
	int m_nConstellationVersion;
	int m_bConstellationLines;
	int m_bConstellationLabel;
	int m_bConstellationArtwork;

	// areas
	int m_bViewImageAreas;

	//////////////////
	// astro image labels - for name build
	// :: detected
	int m_bDetectObjLabelUseId;
	int m_bDetectObjLabelUseMag;
	// :: matched
	int	m_bMatchObjLabelUseId;
	int m_bMatchObjLabelUseCatNo;
	int m_bMatchObjLabelUseCName; 
	// :: cat stars
	int m_bCatStarLabelUseId;
	int m_bCatStarLabelUseCatNo; 
	int m_bCatStarLabelUseCName;
	int m_bCatStarLabelUseType;
	// :: nebula
//	int m_bNebObjLabelUseId;
	int m_bNebObjLabelUseCatNo; 
	int m_bNebObjLabelUseCName;
	int m_bNebObjLabelUseType;
	// :: cluster
	int m_bCluObjLabelUseCatNo; 
	int m_bCluObjLabelUseCName;
	int m_bCluObjLabelUseType;
	// :: galaxy
	int m_bGalObjLabelUseCatNo; 
	int m_bGalObjLabelUseCName;
	int m_bGalObjLabelUseType;
	// :: supernovae
	int m_bSNObjLabelUseCatNo; 
	int m_bSNObjLabelUseCName;
	int m_bSNObjLabelUseType;
	// :: black holes
	int m_bBHObjLabelUseCatNo; 
	int m_bBHObjLabelUseType;
	// :: radio
	int m_bRadObjLabelUseCatNo; 
	int m_bRadObjLabelUseType;
	// :: x-ray
	int m_bXRayObjLabelUseCatNo; 
	int m_bXRayObjLabelUseType;
	// :: gamma
	int m_bGammaObjLabelUseCatNo; 
	int m_bGammaObjLabelUseType;
	// :: multiple/double stars
	int m_bMSObjLabelUseCatNo; 
	int m_bMSObjLabelUseType;
	// :: asteroids
	int m_bASTObjLabelUseCatNo; 
	int m_bASTObjLabelUseType;
	// :: comets
	int m_bCOMObjLabelUseCatNo; 
	int m_bCOMObjLabelUseType;
	// :: extrasolar planets
	int m_bEXPObjLabelUseCatNo; 
	int m_bEXPObjLabelUseType;
	// :: artificial eart satellites
	int m_bAESObjLabelUseCatNo; 
	int m_bAESObjLabelUseType;

	/////////////////////////////
	// SKY/CATALOG/FINDER/MATCH
	// - CATALOGS
	// :: stars
	int		m_nCatalogForMatching;
	int		m_bStarsCatLoadLocalRegion;
	double	m_nStarsCatMaxMagLimit;
	int		m_bStarsCatLoadRemote;
	int		m_nStarsCatLoadRemoteLocation;
	// :: dso
	int		m_nDsoCatalogToUse;
	int		m_bDsoCatLoadLocalRegion;
	int		m_bDsoCatLoadRemote;
	int		m_nDsoCatLoadRemoteLocation;
	int		m_bUseMessier;
	int		m_bUseIc;
	int		m_bUseNgc;
	// :: radio
	int		m_nRadioCatalogToUse;
	int		m_bRadioCatLoadLocalRegion;
	int		m_bRadioCatLoadRemote;
	int		m_nRadioCatLoadRemoteLocation;
	// :: x-ray/gamma
	int		m_nXGammaCatalogToUse;
	int		m_bXGammaCatLoadLocalRegion;
	int		m_bXGammaCatLoadRemote;
	int		m_nXGammaCatLoadRemoteLocation;
	// :: supernovas
	int		m_nSNCatalogToUse;
	int		m_bSNCatLoadLocalRegion;
	int		m_bSNCatLoadRemote;
	int		m_nSNCatLoadRemoteLocation;
	// :: blackholes
	int		m_nBHCatalogToUse;
	int		m_bBHCatLoadLocalRegion;
	int		m_bBHCatLoadRemote;
	int		m_nBHCatLoadRemoteLocation;
	// :: multiple/double stars
	int		m_nMSCatalogToUse;
	int		m_bMSCatLoadLocalRegion;
	int		m_bMSCatLoadRemote;
	int		m_nMSCatLoadRemoteLocation;
	// :: exoplanets
	int		m_nEXPCatalogToUse;
	int		m_bEXPCatLoadLocalRegion;
	int		m_bEXPCatLoadRemote;
	int		m_nEXPCatLoadRemoteLocation;
	// :: asteroids
	int		m_nASTCatalogToUse;
	int		m_bASTCatLoadLocalRegion;
	int		m_bASTCatLoadRemote;
	int		m_nASTCatLoadRemoteLocation;
	// :: comets
	int		m_nCOMCatalogToUse;
	int		m_bCOMCatLoadLocalRegion;
	int		m_bCOMCatLoadRemote;
	int		m_nCOMCatLoadRemoteLocation;
	// :: artificial earth satellites
	int		m_nAESCatalogToUse;
	int		m_bAESCatLoadTimeFrame;
	int		m_bAESCatLoadLocalRegion;
	int		m_bAESCatLoadRemote;
	int		m_nAESCatLoadRemoteLocation;
	// - new stuff
	int		m_bAESCatRemoteUpdateLocal;
	int		m_nAESCatObjCat;
	int		m_nAESCatObjType;
	int		m_bAESCatDontUseType;
	int		m_bAESCatRecUseDateStart;
	double	m_nAESCatRecDateStart;
	int		m_bAESCatRecUseDateEnd;
	double	m_nAESCatRecDateEnd;

//	int		m_bUseOnlineCat; -- do I use this ??
	// do i use this ?????
//	int		m_bUseConstellation;
	// dso catalog to use

	/////////////////////////
	// labels/shapes appereance
	// :: detected
	wxFont			fontDObjLabel;
	wxColour		colorDObjLabel;
	// :: matched
	wxFont			fontMObjLabel;
	wxColour		colorMObjLabel;
	// :: catalog
	wxFont			fontCObjLabel;
	wxColour		colorCObjLabel;
	// :: nebula
	wxFont			fontNebObjLabel;
	wxColour		colorNebObjLabel;
	// :: cluster
	wxFont			fontCluObjLabel;
	wxColour		colorCluObjLabel;
	// :: galaxy
	wxFont			fontGalObjLabel;
	wxColour		colorGalObjLabel;
	// :: supernovae
	wxFont			fontSNObjLabel;
	wxColour		colorSNObjLabel;
	// :: black holes
	wxFont			fontBHObjLabel;
	wxColour		colorBHObjLabel;
	// :: radio
	wxFont			fontRadObjLabel;
	wxColour		colorRadObjLabel;
	// :: xray
	wxFont			fontXRayObjLabel;
	wxColour		colorXRayObjLabel;
	// :: gamma
	wxFont			fontGammaObjLabel;
	wxColour		colorGammaObjLabel;
	// :: multiple/double stars
	wxFont			fontMSObjLabel;
	wxColour		colorMSObjLabel;
	// :: asteroids
	wxFont			fontASTObjLabel;
	wxColour		colorASTObjLabel;
	// :: comets
	wxFont			fontCOMObjLabel;
	wxColour		colorCOMObjLabel;
	// :: extrasolar planets
	wxFont			fontEXPObjLabel;
	wxColour		colorEXPObjLabel;
	// :: artificial earth satellites
	wxFont			fontAESObjLabel;
	wxColour		colorAESObjLabel;

	/////////////////////
	// shape and line type
	// :: detected
	int m_nDetectObjShapeType;
	int m_nDetectObjShapeLine;
	// :: matched
	int m_nMatchObjShapeType;
	int m_nMatchObjShapeLine;
	// :: catalog star
	int m_nCatObjShapeType;
	int m_nCatObjShapeLine;
	// :: nebula
	int m_nNebObjShapeType;
	int m_nNebObjShapeLine;
	// :: cluster
	int m_nCluObjShapeType;
	int m_nCluObjShapeLine;
	// :: galaxy
	int m_nGalObjShapeType;
	int m_nGalObjShapeLine;
	// :: Supernovae
	int m_nSNObjShapeType;
	int m_nSNObjShapeLine;
	// :: black holes
	int m_nBHObjShapeType;
	int m_nBHObjShapeLine;
	// :: radio
	int m_nRadObjShapeType;
	int m_nRadObjShapeLine;
	// :: x-ray
	int m_nXRayObjShapeType;
	int m_nXRayObjShapeLine;
	// :: gamma
	int m_nGammaObjShapeType;
	int m_nGammaObjShapeLine;
	// :: black holes
	int m_nMSObjShapeType;
	int m_nMSObjShapeLine;
	// :: asteroids
	int m_nASTObjShapeType;
	int m_nASTObjShapeLine;
	// :: comets
	int m_nCOMObjShapeType;
	int m_nCOMObjShapeLine;
	// :: extrasolar planets
	int m_nEXPObjShapeType;
	int m_nEXPObjShapeLine;
	// :: artificial eart satellites
	int m_nAESObjShapeType;
	int m_nAESObjShapeLine;

	// units and coordinate systems
//	int	m_nUnitsFormat;
	int m_nSkyCoordDisplayFormat;
	int m_nSkyCoordEntryFormat;
	int	m_nCoordSystem;
	int	m_nGuiLanguage;
	int m_nGuiTheme;

	////////////////////// ?????
	// dynamic object path styles/etc - move to frame
	int m_nObjsPathShapeType;
	//////////////
	// SECTION :: GRAPHICS
	// :: object path
	wxColour		colGraphicsObjectPath;
	int				m_nGraphicsObjectPathLine;
	int				m_bGraphicsPathVertex;
	int				m_bGraphicsPathStartEnd;
	int				m_bGraphicsPathDirection;
	int				m_bGraphicsPathBaseLines;
	// :: sky grid
	wxColour		colGraphicsSkyGrid;
	int				m_nGraphicsSkyGridLine;
	int				m_bSkyGridShowLabels;
	int				m_bSkyGridDrawSplines;
	double			m_nSkyGridRaDivValue;
	double			m_nSkyGridDecDivValue;
	// :: distorsion grid
	wxColour		colGraphicsDistortionGrid;
	int				m_nGraphicsDistortionGridLine;
	int				m_bDistGridDrawSplines;
	double			m_nDistGridRaDivValue;
	double			m_nDistGridDecDivValue;
	double			m_nDistGridBumpFactValue;
	// :: constellations
	wxColour		colGraphicsConstellationLines;
	int				m_nGraphicsConstellationLinesLine;
	int				m_bConstellationMarkStars;
	int				m_bConstellationOutLine;
	int				m_bConstellationGreekLabels;
	// view flag
//	int				m_bShowConstellations;

	////////////////////////////////////////////////////////////////
	// :: MATCH :: search panel
	int m_nSkySearchMethod;
	double m_nSearchAreaIncrementOnRA;
	double m_nSearchAreaIncrementOnDEC;
	int	m_nRaSearchOverlapDivision;
	int	m_nDecSearchOverlapDivision;
	int m_nSearchZoomSteps;
	int m_nSearchZoomStart;
	unsigned char m_bDoReverseZoomSearch;

	// :: MATCH :: proc panel - projection type, etc
	int m_nSkyProjectionMethod;
	int m_bDoMatchFineTune;
	int m_bDoMatchFineTuneWithProjection;
	int m_bDoMatchRefineBestArea;
	// match check methods
	int m_bMatchCheckByNoOfStars;
	int m_bMatchCheckByBestFit;
	// match stop when first best area found
	int m_bMatchStopWhenMinIsFound;
	int m_nMatchMinFoundToStop;
	int m_nSearchMatchMinCatStars;
	int m_nMatchFirstLevelMinStars;
	double m_nMatchFineTuneImgStarsFactor;
	double m_nMatchFineTuneCatStarsFactor;
	double m_nAssocMatchMaxDist;

	// :: MATCH :: object panel - extraction method
	int m_nImgObjectExtractByObjShape;
	int m_nImgObjectExtractByDetectType;
	// from matching config
	unsigned int m_nScanMatchImgObjNo;
	unsigned int m_nScanMatchCatObjNo;
	
	// :: MATCH :: match - module specific
	int m_nMatchTransType;
	int m_bMatchDoRecalc;
	double m_nMatchMaxDist;
	double m_nMatchTriangFit;
	double m_nMatchTriangScale;
	int m_nMatchMaxIter;
	double m_nMatchHaltSigma;

	// MATCH :: FINDER
	int m_bMatchLocateCatStars;
	int m_bMatchLocateDso;
	int m_bMatchLocateXGamma;
	int m_bMatchLocateRadio;
	int m_bMatchLocateSupernovas;
	int m_bMatchLocateBlackholes;
	int m_bMatchLocateMStars;
	int m_bMatchLocateExoplanets;
	int m_bMatchLocateAsteroids;
	int m_bMatchLocateComets;
	int m_bMatchLocateAes;
	int m_bMatchLocateSolar;
	int m_bMatchFetchDetails;

	// MATCH :: SOLAR
	int m_nMatchLocateTimeFrameRange;
	int m_nMatchLocateTimeFrameDivision;
	int m_bOnlyTleInRange;
	int m_nTleInRangeValue;

	//////////////////
	// INSTRUMENTS GENERAL - options like preferd camera etc?
	// ???

	//////////////////
	// SLIDESHOW
	int m_nSlideShowSources;
	int m_nSlideShowOrder;
	int m_nSlideShowDelay;
	int m_nSlideShowRepeat;
	int m_nSlideShowFullScreen;

};

#endif
