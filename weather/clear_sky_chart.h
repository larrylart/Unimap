
#ifndef _CLEAR_SKY_CHART_H
#define _CLEAR_SKY_CHART_H

// system libs
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <math.h>

// wx includes
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
//precompiled headers
#include "wx/wxprec.h"
// other wx
#include <wx/string.h>

// site chart record
typedef struct
{
	double distance;

	wxChar direction[10];
	
	wxChar name[50];
	wxChar full_name[255];

} DefSiteChartRec;

// local headers
#include "weather.h"

using namespace std;

// class:	CClearSkyChart
///////////////////////////////////////////////////////
class CClearSkyChart
{
// methods
public:
	CClearSkyChart( );
	~CClearSkyChart( );

	int FindChart( double lat, double lon );
	int LoadChartData( int id );
	int LoadChartImg( int id );
	int GetPredictionData( CWeather* pWeather, double lat, double lon );

	int SaveCharts( double lat, double lon );
	int LoadCharts( double lat, double lon );

	// method to check if there is an update
	int IsToUpdate( double lat, double lon );

// data
public:
	wxString m_strUrl;
	CWeather* m_pWeather;

	double m_nLat;
	double m_nLon;

	wxString m_strImgFile;

	vector<DefSiteChartRec> m_vectSiteChart;
};

#endif
