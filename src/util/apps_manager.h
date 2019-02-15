////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _APPS_MANAGER_H
#define _APPS_MANAGER_H

// system libs
#include <stdio.h>
#include <vector>

// wx includes
#include "wx/wxprec.h"

// application ids
// image processing
#define MGR_APPS_NAMEID_PHOTOSHOP		0
#define MGR_APPS_NAMEID_GIMP			1
#define MGR_APPS_NAMEID_PIXINSIGHT		2
#define MGR_APPS_NAMEID_MAXIMDL			3
#define MGR_APPS_NAMEID_IMAGEPLUS		4
// stacking
#define MGR_APPS_NAMEID_REGISTAX		5
#define MGR_APPS_NAMEID_DEEPSKYSTACKER	6
#define MGR_APPS_NAMEID_IRIS			7
#define MGR_APPS_NAMEID_REGISTAR		8
#define MGR_APPS_NAMEID_K3CCDTOOLS		9
// other
#define MGR_APPS_NAMEID_GOOGLEEARTH		10
#define MGR_APPS_NAMEID_WORLDWIND		11

// temporary folders structure
typedef struct
{
	wxString name;
	wxString system_name;
	wxString path;
	wxString exe;
	wxString desc;
	wxString cmd_format;
	wxString webpage;

	int found;
	int type;

} DefAppInstall;

// class: CAppsManager
///////////////////////////////////////////////////////
class CAppsManager 
{

// public methods
public:
	CAppsManager( );
	~CAppsManager( );

	void Init();

	void AddApp( wxString name, wxString exe, wxString web, int type, wxString desc=wxT("") );
	int CheckProductName( wxString name, int& id );

	// process temporary files/folders
	int LoadAppsDef( );
	void SaveAppsDef( );
	int SearchApps( int bForce=0 );
	void SearchRegistry( wxString base_key );
	int CheckRegistryApp( wxString base_key, wxString app_key, int app_id );
	int CheckRegistryAppExt( wxString base_key, wxString app_name, int app_id );

	// run an application by id
	int RunApp( int id, wxString strArg );

	int IsApp( int id ){ return( m_vectApps[id].found ); }

#ifdef USE_MSI
	void SearchMsiApps( );
#endif

	// exceptions
	wxString GetGimpExe( wxString path );
	wxString GetRegistaxExe( wxString path );

	std::vector<DefAppInstall> m_vectApps;

	int m_bAppsDefLoaded;
	int m_bAppsProcessed;
};

#endif
