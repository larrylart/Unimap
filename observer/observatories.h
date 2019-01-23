
#ifndef _OBSERVATORIES_H
#define _OBSERVATORIES_H

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// wx includes
#include "wx/wxprec.h"
#include <wx/string.h>

// local headers
#include "../util/folders.h"

// init file
#define OBSERVATORIES_PRO_DEF_FILE		unMakeAppPath(wxT("data/observatories_pro_list.ini"))

// enum section
enum
{
	OBS_LIST_SECTION_NONE = 0,
	OBS_LIST_SECTION_INFO,
	OBS_LIST_SECTION_CONTACT,
	OBS_LIST_SECTION_WEBREF,
	OBS_LIST_SECTION_PHOTO,
	OBS_LIST_SECTION_TELESCOPE
};

// web ref structure
typedef struct
{
	wxString name;
	wxString link;

} DefObsWefRef;

// photo structure
typedef struct
{
	wxString file;
	wxString desc;

} DefObsPhoto;

// instrument structure
typedef struct
{
	int type;
	wxString name;
	wxString design;

} DefObsInstrument;

// observatory structure - todo: maybe define as a separate class
typedef struct
{
	wxString name;
	wxString code;

	double lat;
	double lon;
	double alt;

	wxString web;
	wxString ini_file;
	int has_details;

	// details
	wxString organization;
	wxString built;
	wxString first_light;
	wxString weather;

	// contact
	wxString email;
	wxString phone;
	wxString fax;
	wxString desc;

	// web references
	std::vector<DefObsWefRef> vectWebRef;

	// photo
	std::vector<DefObsPhoto> vectPhoto;

	// instruments
	std::vector<DefObsInstrument> vectInstr;

} DefObservatory;

// local includes
//#include "observer_site.h"
//#include "observer_hardware.h"

// external classes

using namespace std;

// class:	CObservatories
///////////////////////////////////////////////////////
class CObservatories
{
// public methods
public:
	CObservatories( );
	~CObservatories( );

	int		LoadPro( );	
//	int		SavePro( );

	int LoadDetails( int nObsId );

// public data
public:

	// observatories
	vector<DefObservatory>	m_vectObservatory;

};

#endif