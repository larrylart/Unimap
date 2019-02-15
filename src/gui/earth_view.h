////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _EARTH_MAP_VIEW_H
#define _EARTH_MAP_VIEW_H

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// wx
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// local headers
#include "dynamic_img_view.h"
#include "../land/earth.h"

enum
{
	wxID_EVENT_UPDATE_LOCATION	= 23400,
	wxID_EVENT_UPDATE_OBS_LOCATION
};

// structure mark point
typedef struct
{
	wxString name;
	int is_base;
	int is_selected;

	double x;
	double y;

	double lat;
	double lon;

	// flags
	unsigned char isHi;

} DefMapMarkPoint;


// external class
class CConfigMain;
class CDynamicImgView;

DECLARE_EVENT_TYPE( wxEVT_CUSTOM_MESSAGE_UPDATE_EVENT, -1 )

using namespace std;

// class:	CEarthMapView
///////////////////////////////////////////////////////
class CEarthMapView : public CDynamicImgView
{
public:
	CEarthMapView( wxWindow *parent, const wxPoint& pos, const wxSize& size, const wxString& strFile=wxT(""), int bLoad=1 );
	~CEarthMapView( );

	void LoadMap(){ SetImageFromFile( m_strMapImgFile ); }

	void SetZoom( double x, double y );
	void AddMark( double lat, double lon, const wxString& name, int is_base=0, int is_selected=0 );

	void LoadImageMap() { SetImageFromFile( m_strMapImgFile ); }

// data
public:
	wxString m_strMapImgFile;

	int m_bMainTarget;
	CLocationEarth* m_pEarth;

	int m_nSelectedMark;

	double m_nLocX;
	double m_nLocY;
	double m_nTargetLat;
	double m_nTargetLon;
	wxString m_strLocName;
	int bIsLocSet;

	// mark points
	vector<DefMapMarkPoint> m_vectMarks;

	int m_nCountryId;
	int m_nRegionId;
	int m_nCityId;

	// flags
	int m_nLDClickType;

protected:
	void Draw( wxDC& dc, int bSize, int bUpdate );
	void OnMouseLDClick( wxMouseEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
