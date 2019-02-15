////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

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
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include <wx/datetime.h>

// local headers
//#include "../util/units.h"
#include "../util/folders.h"
#include "../util/func.h"
#include "../util/webfunc.h"
#include "../weather/weather.h"

// main header
#include "clear_sky_chart.h"

////////////////////////////////////////////////////////////////////
CClearSkyChart::CClearSkyChart( )
{
	m_nLat = 0.0;
	m_nLon = 0.0;

	// http://cleardarksky.com/cgi-bin/find_chart.py
	m_strUrl = wxT("http://cleardarksky.com/");
	// url to get file - http://cleardarksky.com/txtc/KrsnkObSMIcsp.txt

	m_vectSiteChart.clear();
}

////////////////////////////////////////////////////////////////////
CClearSkyChart::~CClearSkyChart( )
{
	m_vectSiteChart.clear();
}

////////////////////////////////////////////////////////////////////
int CClearSkyChart::FindChart( double lat, double lon )
{
	// first with try to load the data from local cache
	if( LoadCharts( lat, lon ) ) return( 1 );

	int bStatus = 0;
	wxRegEx reEOL( wxT("\r\n|\n|\r"), wxRE_ADVANCED );
	wxRegEx reTdCompl( wxT("<td[^\\>]*?>"), wxRE_ADVANCED );
	wxRegEx reFontSigns( wxT("[\"\']+"), wxRE_ADVANCED );
	wxRegEx reChart( wxT("<tr valign\\=top><td>([0-9]*)<\\/td><td>([^\\<]*)<\\/td><td>.*?<\\/td><td><a href\\=..\\/c\\/(.*?)key.html.*?>([^\\<]*?)<.*?<\\/tr>"), wxRE_ADVANCED|wxRE_ICASE );
	//	<td><a href=..\\/c\\/(.*?)key.html.*?>([^\\<]*?)<.*?<\\/tr>", wxRE_ADVANCED|wxRE_ICASE );

	/*
	<input type=hidden name="Mn" value="cameras">
	<input name=olat type=text size=5>
	<input name=olong type=numeric size=5>
	<input name=olatd type=text size=2>
	<input name=olatm type=text size=2>
	<input name=olongd type=text size=2>
	<input name=olongm type=numeric size=2>
	<input name="unit" value=0 type="radio">
	<input name="unit" checked type="radio" value=1>
	<input type=submit value="Find">
	*/
	// set my post data - convert lat/lon to n/e
	wxString strPostData = wxT("");
	strPostData.sprintf( wxT("type=llmap&Mn=cameras&olat=%.6lf&olong=%.6lf&unit=0"), lat, -lon );

	// set my url
	wxString strUrl = wxT("");
	strUrl.sprintf( wxT("%s%s%s"), m_strUrl, wxT("cgi-bin/find_chart.py?"), strPostData );

	// get page
	wxString strPage=wxT("");
	if( !GetWebPage( strUrl, strPage ) ) return( 0 );

	// clean stuff
	reTdCompl.ReplaceAll( &strPage, wxT("<td>") );
	reFontSigns.ReplaceAll( &strPage, wxT("") );
	reEOL.ReplaceAll( &strPage, wxT("") );

	// remove eol
//	strPage.Replace( "\r\n", "" );
//	strPage.Replace( "\r", "" );
//	strPage.Replace( "\n", "" );

	// debug :: dump code
//	FILE* ff = fopen( "./csc.txt", "w" );
//	fputs( strPage, ff );
//	fclose( ff );

	// loop lokking for site/charts
	m_vectSiteChart.clear();
	DefSiteChartRec srec;
	while( reChart.Matches( strPage ) )
	{
		// process data
		srec.distance = wxAtof( reChart.GetMatch( strPage, 1 ) );
		wxStrcpy( srec.direction, reChart.GetMatch( strPage, 2 ) );
		wxStrcpy( srec.name, reChart.GetMatch( strPage, 3 ) );
		wxStrcpy( srec.full_name, reChart.GetMatch( strPage, 4 ) );

		// set to my vector
		m_vectSiteChart.push_back( srec );

		// replace
		reChart.Replace( &strPage, wxT(""), 1 );
	}

	// chekc if I got something
	if( m_vectSiteChart.size() )
	{
		SaveCharts( lat, lon );
		return( 1 );

	} else
		return( 0 );
}


////////////////////////////////////////////////////////////////////
int CClearSkyChart::LoadChartData( int id )
{
	if( !m_vectSiteChart.size() ) return( 0 );

	/////////////////////////////
	// also calculate here / data file - load save

	int bStatus = 0;
	wxRegEx reLine( wxT("\\(\"([^\"]+)\",[\\ \t]*([0-9]+|None),[\\ \t]*([0-9]+|None),[\\ \t]*([0-9]+|None),[\\ \t]*([0-9]+|None),[\\ \t]*([0-9]+|None),[\\ \t]*([0-9]+|None),[\\ \t]*\\),"), wxRE_ADVANCED|wxRE_ICASE );

	// set my url
	wxString strUrl = wxT("");
	strUrl.sprintf( wxT("%s%s%s%s"), m_strUrl, wxT("txtc/"), m_vectSiteChart[id].name, wxT("csp.txt") );

	// get page
	wxString strPage=wxT("");
	if( !GetWebPage( strUrl, strPage ) ) return( 0 );

	// debug :: dump code
//	FILE* ff = fopen( "./csc_f.txt", "w" );
//	fputs( strPage.ToAscii(), ff );
//	fclose( ff );

	// load records
	while( reLine.Matches( strPage ) )
	{
		wxString strDateTime = reLine.GetMatch( strPage, 1 );
		wxString strCloud = reLine.GetMatch( strPage, 2 );
		wxString strTrans = reLine.GetMatch( strPage, 3 );
		wxString strSeeing = reLine.GetMatch( strPage, 4 );
		wxString strWind = reLine.GetMatch( strPage, 4 );
		wxString strHum = reLine.GetMatch( strPage, 4 );
		wxString strTemp = reLine.GetMatch( strPage, 4 );
		// todo: here i should store the data in my weather vector
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CClearSkyChart::LoadChartImg( int id )
{
	// check if there are sites
	if( !m_vectSiteChart.size() ) return( 0 );

	int bStatus = 0;
	wxString strImgUrl=wxT("");

	// make file name
	m_strImgFile.Printf( wxT("%s/%.4lf_%.4lf.gif"), unMakeAppPath(wxT("data/weather/clearsky")), m_nLat, m_nLon );

	// check if the folder exists
	unMakeDirIf( unMakeAppPath(wxT("data/weather/clearsky")) );

	// verify time stamp - not older the half a day - 43200
	if( wxFileExists( m_strImgFile ) ) 
	{
		time_t ftime = wxFileModificationTime( wxString(m_strImgFile,wxConvUTF8) );
		time_t time_now = time(NULL);
		long time_diff = time_now-ftime;
		if( time_diff > 43200 ) return( 0 );
	}

	// make url
	strImgUrl.Printf( wxT("http://cleardarksky.com/csk/getcsk.php?id=%s"), m_vectSiteChart[id].name );

	if( !LoadWebFileT( strImgUrl, m_strImgFile, NULL, 0, 1 ) ) return( 0 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CClearSkyChart::GetPredictionData( CWeather* pWeather, double lat, double lon )
{
	m_pWeather = pWeather;
	int bStatus = 0;

	m_nLat = lat;
	m_nLon = lon;

	// first find chart
	if( FindChart( lat, lon ) )
	{
		// load data file for first chart data
//		if( !LoadChartData( 0 ) ) return( 0 );
		if( !LoadChartImg( 0 ) ) return( 0 );

	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CClearSkyChart::SaveCharts( double lat, double lon )
{
	FILE* pFile = NULL;
	wxString strFile=wxT("");
	int i=0, j=0;

	// make file name
	strFile.Printf( wxT("%s/%.4lf_%.4lf.ini"), unMakeAppPath(wxT("data/weather/clearsky")), lat, lon );

	// check if the folder exists
	unMakeDirIf( unMakeAppPath(wxT("data/weather/clearsky")) );

	// open local file to write
	pFile = wxFopen( strFile, wxT("w") );
	if( !pFile ) return( 0 );

	// for every prediction day
	for( i=0; i<m_vectSiteChart.size(); i++ )
	{
		wxFprintf( pFile, wxT("\n[%s]\n"), m_vectSiteChart[i].name );
		wxFprintf( pFile, wxT("FName=%s\n"), m_vectSiteChart[i].full_name );
		wxFprintf( pFile, wxT("Dist=%lf\n"), m_vectSiteChart[i].distance );
		wxFprintf( pFile, wxT("Dir=%s\n"), m_vectSiteChart[i].direction );
		wxFprintf( pFile, wxT("\n") );
	}

	// close my file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CClearSkyChart::LoadCharts( double lat, double lon )
{
	wxString strFile=wxT("");

	// make file name
	strFile.Printf( wxT("%s/%.4lf_%.4lf.ini"), unMakeAppPath(wxT("data/weather/clearsky")), lat, lon );

	// check if file exists
	if( !wxFileExists(strFile) ) return( 0 );

	// then we verify time stamp - not older the half a day - 8640000
	time_t ftime = wxFileModificationTime( strFile );
	time_t time_now = time(NULL);
	long time_diff = time_now-ftime;
	if( time_diff > 8640000 ) return( 0 );

	// else we go ahead and load the file data
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxChar strTmp[2000];
	int nLineSize=0, bFoundSection=0, nCr=0;
	unsigned long nULong=0;
	double nFloat;

	// open file to read
	pFile = wxFopen( strFile, wxT("r") );
	// check if there is any configuration to load
	if( !pFile ) return( -1 );

	// Reading lines from cfg file
	m_vectSiteChart.clear();
	while( !feof( pFile ) )
	{
		// get one line out of the config file
		wxFgets( strLine, 2000, pFile );
		nLineSize = wxStrlen( strLine );

		// check if to skip because line to short or comment
		if( nLineSize < 4 || strLine[0] == '#' ) continue;

		if( strLine[0] == '[' && strLine[nLineSize-2] == ']' )
		{
			bFoundSection = 1;
			//char strName[255];
			wxSscanf( strLine, wxT("\[%[^\]]\]\n"), strTmp );

			// add to vector
			nCr = m_vectSiteChart.size();
			DefSiteChartRec rec;
			wxStrcpy( rec.name, strTmp );
			m_vectSiteChart.push_back( rec );

		// :: full name
		} else if( bFoundSection && wxStrncmp( strLine, wxT("FName="), 6 ) == 0 )
		{
			wxSscanf( strLine, wxT("FName=%[^\n]\n"), strTmp );
			wxStrcpy( m_vectSiteChart[nCr].full_name, strTmp );

		// :: Distance
		} else if( bFoundSection && wxStrncmp( strLine, wxT("Dist="), 5 ) == 0 )
		{
			wxSscanf( strLine, wxT("Dist=%[^\n]\n"), strTmp );
			m_vectSiteChart[nCr].distance = wxAtof( strTmp );

		// :: full name
		} else if( bFoundSection && wxStrncmp( strLine, wxT("Dir="), 4 ) == 0 )
		{
			wxSscanf( strLine, wxT("Dir=%[^\n]\n"), strTmp );
			wxStrcpy( m_vectSiteChart[nCr].direction, strTmp );
		}
	}
	// close file
	fclose( pFile );

	return( 1 );
}

// just check for the existance/date of image - for now ...
////////////////////////////////////////////////////////////////////
int CClearSkyChart::IsToUpdate( double lat, double lon )
{
	// make file name
	m_strImgFile.Printf( wxT("%s/%.4lf_%.4lf.gif"), unMakeAppPath(wxT("data/weather/clearsky")), lat, lon );

	// verify time stamp - not older the half a day - 43200
	if( wxFileExists( m_strImgFile ) ) 
	{
		time_t ftime = wxFileModificationTime( m_strImgFile );
		time_t time_now = time(NULL);
		long time_diff = time_now-ftime;

		// if more then my limit time the yes - else is ok
		if( time_diff > 43200 ) 
			return( 1 );
		else
			return( 0 );

	} else
		return( 1 );
}