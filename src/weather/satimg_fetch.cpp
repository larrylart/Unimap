////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

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
#include <wx/xml/xml.h>
#include <wx/datetime.h>

// local headers
#include "../util/func.h"
#include "../util/webfunc.h"
#include "../util/folders.h"

// include main header
#include "satimg_fetch.h"

////////////////////////////////////////////////////////////////////
int Get_LatestWeatherSatImage_fvalk( int nZone, double lat, double lon, 
									wxString& strImgFile, double& nImgDateTime )
{
	wxString strPage=wxT("");
	wxString strField=wxT("");
	wxString strUrl;
	wxString strTmp;
	unsigned long nVarULong;
	double nVarDouble;
	// regex
	wxRegEx reImg;
	if( nZone == 0 )
	{
		// compile zone auto from lat/lon - defaults to 1=europe
		nZone = 1;
	}
	// by zone 
	if( nZone == 1 )
		reImg.Compile( wxT("\\>METEOSAT\\-8\\-([0-9]+)\\-EUR\\.jpg\\<\\/A\\>\\ +([^\\ ]+)\\ ([^\\ ]+)\\ +([^\\<]+)\\<") );
	else if( nZone == 2 )
		reImg.Compile( wxT("\\>METEOSAT\\-8\\-([0-9]+)\\-WLD\\.jpg\\<\\/A\\>\\ +([^\\ ]+)\\ ([^\\ ]+)\\ +([^\\<]+)\\<") );
	 else if( nZone == 3 )
		reImg.Compile( wxT("\\>METEOSAT\\-7\\-([0-9]+)\\.jpg\\<\\/A\\>\\ +([^\\ ]+)\\ ([^\\ ]+)\\ +([^\\<]+)\\<") );
	else if( nZone == 4 )
		reImg.Compile( wxT("\\>GOES\\-9\\-([0-9]+)\\.jpg\\<\\/A\\>\\ +([^\\ ]+)\\ ([^\\ ]+)\\ +([^\\<]+)\\<") );
	else if( nZone == 5 )
		reImg.Compile( wxT("\\>GOES\\-12\\-([0-9]+)\\.jpg\\<\\/A\\>\\ +([^\\ ]+)\\ ([^\\ ]+)\\ +([^\\<]+)\\<") );
	else if( nZone == 6 )
		reImg.Compile( wxT("\\>GOES\\-10\\-([0-9]+)\\.jpg\\<\\/A\\>\\ +([^\\ ]+)\\ ([^\\ ]+)\\ +([^\\<]+)\\<") );

	// build url
//	sprintf( strTmp, "%.6f%s%.6f", lat, "%2C", lon );
	strUrl = wxT( "http://www.fvalk.com/images/Day_image/" );
	// get page
	if( !GetWebPage( strUrl, strPage ) ) return( 0 );

	wxString strImgTime = wxT("");
	wxString strFileDate = wxT("");
	wxString strFileSize = wxT("");
	wxDateTime datetimeImg;
	unsigned long nImgFileSize = 0;
	while( reImg.Matches( strPage ) )
	{
		// process file date
		strFileDate = reImg.GetMatch( strPage, 2 ).Trim(0).Trim(1);
		datetimeImg.ParseDate( strFileDate );
		// process image time
		strImgTime = reImg.GetMatch( strPage, 1 ).Trim(0).Trim(1);
		wxString strImgH = strImgTime.Mid( 0, 2 );
		wxString strImgM = strImgTime.Mid( 2, 2 );
		strTmp.Printf( wxT("%s:%s:00"), strImgH, strImgM );
		datetimeImg.ParseTime( strTmp );
		// process file size
		strFileSize = reImg.GetMatch( strPage, 4 ).Trim(0).Trim(1);
		char chrSizeUnit = strFileSize.Last();
		unsigned long nSizeUnit = 0;
		if( chrSizeUnit == 'k' || chrSizeUnit == 'K' )
			nSizeUnit = 1000;
		else if( chrSizeUnit == 'm' || chrSizeUnit == 'M' )
			nSizeUnit = 1000000;
		// check if found last
		if( nSizeUnit != 0 )
			strFileSize = strFileSize.RemoveLast();
		else
			nSizeUnit = 1;
		// calculate size
		if( !strFileSize.ToULong( &nImgFileSize ) ) nImgFileSize = 300;
		nImgFileSize *= nSizeUnit;

		reImg.Replace( &strPage, wxT(""), 1 );
		strPage = strPage.Trim(0).Trim(1);
	}

	// check if there is an image from meteosat8
	if( strImgTime.IsEmpty() ) return( 0 );
	// else get image 
	// :: set image date/time
	nImgDateTime = datetimeImg.GetJDN();
	// :: build date/time string
	strTmp.Printf( wxT("%s"), datetimeImg.Format( wxT("%d%m%Y_%H%M%S") ) );
	// :: check by zone 
	if( nZone == 1 )
	{
		strUrl.Printf( wxT("http://www.fvalk.com/images/Day_image/METEOSAT-8-%s-EUR.jpg"), strImgTime );
		strImgFile.Printf( wxT("%s/fvalk_meteosat8_eur_%s.jpg"), unMakeAppPath(wxT("data/weather/sat_images")), strTmp );

	} else if( nZone == 2 )
	{
		strUrl.Printf( wxT("http://www.fvalk.com/images/Day_image/METEOSAT-8-%s-WLD.jpg"), strImgTime );
		strImgFile.Printf( wxT("%s/fvalk_meteosat8_wld_%s.jpg"), unMakeAppPath(wxT("data/weather/sat_images")), strTmp );

	} else if( nZone == 3 )
	{
		strUrl.Printf( wxT("http://www.fvalk.com/images/Day_image/METEOSAT-7-%s.jpg"), strImgTime );
		strImgFile.Printf( wxT("%s/fvalk_meteosat7_casia_%s.jpg"), unMakeAppPath(wxT("data/weather/sat_images")), strTmp );

	} else if( nZone == 4 )
	{
		strUrl.Printf( wxT("http://www.fvalk.com/images/Day_image/GOES-9-%s.jpg"), strImgTime );
		strImgFile.Printf( wxT("%s/fvalk_meteosat7_easia_%s.jpg"), unMakeAppPath(wxT("data/weather/sat_images")), strTmp );

	} else if( nZone == 5 )
	{
		strUrl.Printf( wxT("http://www.fvalk.com/images/Day_image/GOES-12-%s.jpg"), strImgTime );
		strImgFile.Printf( wxT("%s/fvalk_meteosat7_americas_%s.jpg"), unMakeAppPath(wxT("data/weather/sat_images")), strTmp );

	} else if( nZone == 6 )
	{
		strUrl.Printf( wxT("http://www.fvalk.com/images/Day_image/GOES-10-%s.jpg"), strImgTime );
		strImgFile.Printf( wxT("%s/fvalk_meteosat7_wus_%s.jpg"), unMakeAppPath(wxT("data/weather/sat_images")), strTmp );
	}

	// now check if file exist
	if( !wxFileExists( strImgFile ) )
		if( !LoadWebFileT( strUrl, strImgFile, NULL, nImgFileSize, 1 ) ) return( 0 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int Get_LatestWeatherSatImage_MeteoamIt( int nType, wxString& strImgFile, double& nImgDateTime )
{
	wxString strPage=wxT("");
	wxString strImgUrl=wxT("");
	wxString strUrl;
	wxString strTmp;
	unsigned long nVarULong;
	double nVarDouble;
	// regex
	// <input type="image" name="Infrarosso3" src="satellit/WWWP/infrarosso7_2009-07-07 22:30:00.jpg"
	// <input type="image" name="Visibile3" src="satellit/WWWP/visibile7_2009-07-07 16:45:00.jpg"
	// <input type="image" name="Vapore Acqueo3" src="satellit/WWWP/vapore7_2009-07-07 22:30:00.jpg"
	wxRegEx reImg;
	if( nType == 0 )
	{
		reImg.Compile( wxT("fileNameView\\=satellit\\/WWW\\/visibile([0-9]+)_([^\\.]+)\\.jpg") );
		strImgUrl = wxT("http://www.meteoam.it/satellit/WWW/visibile");	

	} else if( nType == 1 )
	{
		reImg.Compile( wxT("fileNameView\\=satellit\\/WWW\\/infrarosso([0-9]+)_([^\\.]+)\\.jpg") );
		strImgUrl = wxT("http://www.meteoam.it/satellit/WWW/infrarosso");	

	} else if( nType == 2 )
	{
		reImg.Compile( wxT("fileNameView\\=satellit\\/WWW\\/vapore([0-9]+)_([^\\.]+)\\.jpg") );
		strImgUrl = wxT("http://www.meteoam.it/satellit/WWW/vapore");

	} else
		return( 0 );

	// build url
	strUrl.Printf( wxT("http://www.meteoam.it/modules.php?name=meteosat7") );
	// get page
	if( !GetWebPage( strUrl, strPage ) ) return( 0 );

	wxString strImgNo = wxT("");
	wxString strImgDateTime = wxT("");
	wxDateTime datetimeImg;
	while( reImg.Matches( strPage ) )
	{
		// get image no
		strImgNo = reImg.GetMatch( strPage, 1 ).Trim(0).Trim(1);
		// get/process image date/time
		strImgDateTime = reImg.GetMatch( strPage, 2 ).Trim(0).Trim(1);
		datetimeImg.ParseFormat( strImgDateTime, wxT("%Y-%m-%d %H:%M:%S") );

		// remove what we found so far
		reImg.Replace( &strPage, wxT(""), 1 );
		//strPage = strPage.Trim(0).Trim(1);
	}
	// address to use: http://www.meteoam.it/satellit/WWW/
	// check if there is an image from meteosat8
	if( strImgNo.IsEmpty() || strImgDateTime.IsEmpty() ) return( 0 );

	// :: set image date/time
	nImgDateTime = datetimeImg.GetJDN();
	// built output file date/time
	strTmp.Printf( wxT("%s"), datetimeImg.Format( wxT("%d%m%Y_%H%M%S") ) );
	// build image file path by type
	if( nType == 0 )
		strImgFile.Printf( wxT("%s/meteoamit_meteosat7_vis_%s.jpg"), unMakeAppPath(wxT("data/weather/sat_images")), strTmp );
	else if( nType == 1 )
		strImgFile.Printf( wxT("%s/meteoamit_meteosat7_ir_%s.jpg"), unMakeAppPath(wxT("data/weather/sat_images")), strTmp );
	else if( nType == 2 )
		strImgFile.Printf( wxT("%s/meteoamit_meteosat7_wv_%s.jpg"), unMakeAppPath(wxT("data/weather/sat_images")), strTmp );

	// now check if file exist
	if( !wxFileExists( strImgFile ) )
	{
		// build url
		strUrl.Printf( wxT("%s%s_%s.jpg"), strImgUrl, strImgNo, strImgDateTime );
		if( !LoadWebFileT( strUrl, strImgFile, NULL, 32000, 1 ) ) return( 0 );
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int Get_LatestWeatherSatImage_EuroMeteo( int nType, int nZoneNo, double lat, 
										double lon, wxString& strImgFile, double& nImgDateTime )
{
	wxString strPage=wxT("");
	wxString strImgUrl=wxT("");
	wxString strUrl;
	wxString strTmp;
	unsigned long nVarULong;
	double nVarDouble;
	// hard code for now
	int vectZones[17] = { 14, 5, 6, 15, 4, 2, 3, 7, 16, 17, 18, 12, 11, 10, 9, 8, 1 };
	// regex
	// array_nom_imagen[0]="wUwAX6iWPIJlz"
	wxRegEx reImg = wxT("array_nom_imagen\\[0\\]\\=\\\"([^\\\"]+)\\\"");
	// <option value="0">08/07/09   12:00 UTC</option>
	wxRegEx reImgDateTime = wxT("\\<option\\ value\\=\\\"0\\\"\\>([^\\ ]+)\\ +([^\\ ]+)\\ +UTC\\<");
	// by zone ??
	if( nZoneNo == 0 ) nZoneNo = 9;
	if( nZoneNo >= 1 && nZoneNo < 18 )
	{
		// build main url
		strUrl.Printf( wxT("http://oiswww.eumetsat.org/IPPS/html/MSG/RGB/EVIEW/SEGMENT%d/index.htm"), vectZones[nZoneNo-1] );
		// build base image url
		strImgUrl.sprintf( wxT("http://oiswww.eumetsat.org/IPPS/html/MSG/RGB/EVIEW/SEGMENT%d/IMAGESDisplay/"), vectZones[nZoneNo-1] );	

	} else
		return( 0 );

	// get page
	if( !GetWebPage( strUrl, strPage ) ) return( 0 );

	wxString strImgCode = wxT("");
	wxString strImgDate = wxT("");
	wxString strImgTime = wxT("");
	wxDateTime datetimeImg;
	while( reImg.Matches( strPage ) )
	{
		// get image code
		strImgCode = reImg.GetMatch( strPage, 1 ).Trim(0).Trim(1);
		// get/process image date/time
		if( reImgDateTime.Matches( strPage ) )
		{
			strImgDate = reImgDateTime.GetMatch( strPage, 1 ).Trim(0).Trim(1);
			strImgTime = reImgDateTime.GetMatch( strPage, 2 ).Trim(0).Trim(1);
			strTmp.Printf( wxT("%s %s:00"), strImgDate, strImgTime );
			datetimeImg.ParseFormat( strTmp, wxT("%d/%m/%y %H:%M:%S") );

		} else
			strImgCode = wxT("");

		
		// exit loop ... found
		break;

		// not used yet : maybe use latter when animation implemente: remove what we found so far
		// reImg.Replace( &strPage, "", 1 );
		//strPage = strPage.Trim(0).Trim(1);
	}
	// check if there is an image from meteosat
	if( strImgCode.IsEmpty() ) return( 0 );

	// :: set image date/time
	nImgDateTime = datetimeImg.GetJDN();
	// built output file date/time
	strTmp.Printf( wxT("%s"), datetimeImg.Format( wxT("%d%m%Y_%H%M%S") ) );
	// build image file path by type
	strImgFile.Printf( wxT("%s/eumetsatorg_%d_%s.jpg"), unMakeAppPath(wxT("data/weather/sat_images")), vectZones[nZoneNo-1], strTmp );

	// now check if file exist
	if( !wxFileExists( strImgFile ) )
	{
		// build url
		strUrl.Printf( wxT("%s%s"), strImgUrl, strImgCode );
		if( !LoadWebFileT( strUrl, strImgFile, NULL, 32000, 1 ) ) return( 0 );
	}

	return( 1 );
}