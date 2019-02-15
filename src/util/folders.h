////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _DATAFOLDERS_H
#define _DATAFOLDERS_H

#include <wx/stdpaths.h>

extern wxString g_strUserAppDataPath;

// make dir if not exist
static void unMakeDirIf( wxString strPath ){ if( !wxDirExists( strPath ) ) wxMkdir( strPath ); }
static wxString unMakeAppPath( wxString strPath ){ return( g_strUserAppDataPath + wxT("/") + strPath ); }

#define TEMP_ONLINE_OBJECT_DATA_PATH			unMakeAppPath(wxT("data/object/tmp/"))
//#define BIBCODES_ONLINE_OBJECT_DATA_PATH		unMakeAppPath(wxT("data/object/"))
#define OBJECT_REMOTE_IMAGE_OPTICAL_PATH		unMakeAppPath(wxT("data/object/img/optical/"))
#define OBJECT_REMOTE_IMAGE_IR_PATH				unMakeAppPath(wxT("data/object/img/ir/"))
#define OBJECT_REMOTE_IMAGE_UV_PATH				unMakeAppPath(wxT("data/object/img/uv/"))
#define OBJECT_REMOTE_IMAGE_RADIO_PATH			unMakeAppPath(wxT("data/object/img/radio/"))
#define OBJECT_REMOTE_IMAGE_XRAY_PATH			unMakeAppPath(wxT("data/object/img/xray/"))
#define OBJECT_REMOTE_IMAGE_HARDXRAY_PATH		unMakeAppPath(wxT("data/object/img/hardxray/"))
#define OBJECT_REMOTE_IMAGE_DIFFUSEXRAY_PATH	unMakeAppPath(wxT("data/object/img/diffusexray/"))
#define OBJECT_REMOTE_IMAGE_GAMMARAY_PATH		unMakeAppPath(wxT("data/object/img/gammaray/"))
#define BIBCODES_SUMMARY_DATA_PATH				unMakeAppPath(wxT("data/bibcodes/"))
// skyview data
#define OBJECT_SKYVIEW_COLOR_TABLE_FILE			unMakeAppPath(wxT("data/skyview/skyview_color_table.txt"))
#define OBJECT_SKYVIEW_CATALOG_OVERLAY_FILE		unMakeAppPath(wxT("data/skyview/skyview_cat_overlay.txt"))
#define OBJECT_SKYVIEW_SURVEYS_FILE				unMakeAppPath(wxT("data/skyview/skyview_surveys.txt"))
#define OBJECT_SKYVIEW_DATA_TEMP				unMakeAppPath(wxT("data/skyview/tmp/"))
// spectrum
#define OBJECT_SPECTRUM_DATA_PATH				unMakeAppPath(wxT("data/object/spectrum/"))
// thumbnails
#define GROUP_IMAGE_THUMBNAILS_PATH				unMakeAppPath(wxT("data/thumbnails/"))

#endif
