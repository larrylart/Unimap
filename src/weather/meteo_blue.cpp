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
#include <wx/datetime.h>
#include <wx/app.h>

// local headers
//#include "../util/units.h"
#include "../util/folders.h"
#include "../unimap.h"
#include "../unimap_worker.h"
#include "../online/unimap_online.h"
#include "../util/func.h"
#include "../util/webfunc.h"
#include "../weather/weather.h"

// main header
#include "meteo_blue.h"

// get external unimap
DECLARE_APP(CUnimap)

////////////////////////////////////////////////////////////////////
CMeteoBlue::CMeteoBlue( )
{
	// get references to worker and online objects
	CUnimap* pUnimap = &(wxGetApp());
	m_pUnimapOnline = pUnimap->m_pUnimapOnline;
	m_pUnimapWorker = pUnimap->m_pUnimapWorker;

	m_strUrl = wxT("http://my.meteoblue.com/");
//	m_pHttp = NULL;
	m_bIsLogin = 0;
}

////////////////////////////////////////////////////////////////////
CMeteoBlue::~CMeteoBlue( )
{
//	if( m_pHttp ) delete( m_pHttp );
//	m_pHttp = NULL;
}

////////////////////////////////////////////////////////////////////
int CMeteoBlue::Login( const char* strUser, const char* strPassword )
{
	if( m_bIsLogin ) return( 1 );

	int bStatus = 0;
	wxRegEx reExpired( wxT("<h2> Your mymeteoblue account expired\.<\/h2>"), wxRE_ADVANCED );
	wxRegEx reBInput( wxT("<INPUT TYPE=HIDDEN NAME=\"(ION__.*?)\" VALUE=\"(.*?)\""), wxRE_ADVANCED );

	wxString strUrl = wxT("");
	wxString strPostData = wxT("");
	wxString strExpUrl = wxT("");
//	wxString strExpPostData = wxT("");

	//strPostData.sprintf( "page=zoommap.ion&username=%s&p=%s&userid=&domain=NMM22&ptype=XY", strUser, strPassword );
	strPostData.sprintf( wxT("page=zoommap.ion&username=%s&p=%s"), URLEncode(wxString(strUser,wxConvUTF8)), URLEncode(wxString(strPassword,wxConvUTF8)) );

	// set proper url call
	strUrl.sprintf( wxT("%s%s%s"), m_strUrl, wxT("cgi/ion-p?"), strPostData );

	// get page
	wxString strPage=wxT("");
	if( !GetWebPage( strUrl, strPage ) ) return( 0 );

	// debug :: dump code
//	FILE* ff = fopen( "./bm30.txt", "w" );
//	fputs( strPage.ToAscii(), ff );
//	fclose( ff );

	// remove eol
	strPage.Replace( wxT("\r\n"), wxT("") );
	strPage.Replace( wxT("\r"), wxT("") );
	strPage.Replace( wxT("\n"), wxT("") );


	// verify if result ok and build map req string
	m_strAuthInput = wxT("");

	// check if expired
	if( reExpired.Matches( strPage ) )
	{
		m_strAuthInput = wxT("");
		// loop to get all ion values
		while( reBInput.Matches( strPage ) )
		{
			bStatus = 1;

			wxString strName = reBInput.GetMatch( strPage, 1 );
			wxString strValue = reBInput.GetMatch( strPage, 2 );

			m_strAuthInput += strName;
			m_strAuthInput += wxT("=");
			m_strAuthInput += URLEncode(strValue);
			m_strAuthInput += wxT("&");

			reBInput.Replace( &strPage, wxT(""), 1 );
		}
		if( !bStatus ) return( 0 );

		// set to update with 90+ days 
		m_strAuthInput += wxT("ndays=90&extendaccount=ok");
		// set exp url
		strExpUrl.sprintf( wxT("%s%s%s"), m_strUrl, wxT("cgi/ion-p?"), m_strAuthInput );
		// call the expire page get
		if( !GetWebPage( strExpUrl, strPage ) ) return( 0 );

	// debug :: dump code
//	FILE* ff = fopen( "./bm40.txt", "w" );
//	fputs( strPage.ToAscii(), ff );
//	fclose( ff );

		// then try to login again
		strPage = wxT("");
		if( !GetWebPage( strUrl, strPage ) ) return( 0 );
		m_strAuthInput = wxT("");
		bStatus = 0;
	}

	// check if login - loop to get all ion_values
	while( reBInput.Matches( strPage ) )
	{
		bStatus = 1;

		wxString strName = reBInput.GetMatch( strPage, 1 );
		wxString strValue = reBInput.GetMatch( strPage, 2 );

		m_strAuthInput += strName;
		m_strAuthInput += wxT("=");
		m_strAuthInput += URLEncode(strValue);
		m_strAuthInput += wxT("&");

		reBInput.Replace( &strPage, wxT(""), 1 );
	}
	// if b status = 1 the add the rest of stuff except for lat/long to the string
	m_strAuthInput += wxT("model=NMM22&ptypef=PT&CHOICE=Create+new+...&");
	m_strAuthInput += wxT("imgxs=900&imgys=900&domain_java=NMM22&showmap=make+Map&");
	m_strAuthInput += wxT("countryi=0&ptype=PT&domain=NMM22&fields=seeing&imgsize=900&newMap=new+Map&");
	

	// set satus of login
	m_bIsLogin = bStatus;

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
int CMeteoBlue::GetPredictionData( CWeather* pWeather, double lat, double lon )
{
	m_pWeather = pWeather;

	// first with try to load the data from local cache
	if( LoadData( lat, lon ) )
	{
		return( 1 );
	}

	// then we try to login
	//if( !Login( "larry@larryo.org", "JTKGRYJL" ) ) return( 0 );
	//if( !Login( "1o1@o2.ie", "OSSMHVCL" ) )

	// get user/password from online object
	wxString strUser = m_pUnimapOnline->m_vectServiceAccounts[0].username;
	wxString strPassword( m_pUnimapOnline->m_vectServiceAccounts[0].password, wxConvUTF8 );
	// check if user password not empty
	if( strUser.IsEmpty() || strPassword.IsEmpty() )
		m_pUnimapWorker->DisplayNotification( wxT("Meteo-Blue: User/Password is not set! \nCheck Menu/Options/Online/Services!"), 0 );
	
	if( !Login( strUser.ToAscii(), strPassword.ToAscii() ) )
	{
		m_pUnimapWorker->DisplayNotification( wxT("Meteo-Blue: Failed to login! \nCheck Menu/Options/Online/Services!"), 0 );
		return( 0 );
	}

	int bStatus = 0;
	wxRegEx reFontClean( wxT("<font[^\\>]*?>"), wxRE_ADVANCED|wxRE_ICASE );
	wxRegEx reFontEClean( wxT("<\\/font>"), wxRE_ADVANCED|wxRE_ICASE );
	wxRegEx reFontSigns( wxT("[\"\']+"), wxRE_ADVANCED );
	wxRegEx reDay( wxT("<tr><td width=200><b>.*? (.*?)<\\/b>.*?\\<\\/tr>.*?<\\/tr>(.*?)(<tr><td width=200><b>|<\\/table>)"), wxRE_ADVANCED|wxRE_ICASE );
	wxRegEx reHighClouds( wxT("\\>High Clouds(.*?)<\\/tr>"), wxRE_ADVANCED|wxRE_ICASE );
	wxRegEx reMidClouds( wxT("\\>Mid Clouds(.*?)<\\/tr>"), wxRE_ADVANCED|wxRE_ICASE );
	wxRegEx reLowClouds( wxT("\\>Low Clouds(.*?)<\\/tr>"), wxRE_ADVANCED|wxRE_ICASE );
	wxRegEx reSeeingId1( wxT("\\>Seeing Index 1(.*?)<\\/tr>"), wxRE_ADVANCED|wxRE_ICASE );
	wxRegEx reSeeingId2( wxT("\\>Seeing Index 2(.*?)<\\/tr>"), wxRE_ADVANCED|wxRE_ICASE );
	wxRegEx reSeeingArcSec( wxT("\\>Seeing arcSeconds(.*?)<\\/tr>"), wxRE_ADVANCED|wxRE_ICASE );
	wxRegEx reJet( wxT("\\> Jet stream(.*?)<\\/tr>"), wxRE_ADVANCED|wxRE_ICASE );
	wxRegEx reRelHum( wxT("\\>2m Rel. Hum.(.*?)<\\/tr>"), wxRE_ADVANCED|wxRE_ICASE );
	wxRegEx reBadTop( wxT("\\> Bad Layers Top(.*?)<\\/tr>"), wxRE_ADVANCED|wxRE_ICASE );
	wxRegEx reBadBot( wxT("\\> Bad Layers Bot(.*?)<\\/tr>"), wxRE_ADVANCED|wxRE_ICASE );
	wxRegEx reBadK( wxT("\\> Bad Layers K(.*?)<\\/tr>"), wxRE_ADVANCED|wxRE_ICASE );
//	wxRegEx reTdCElem( "<td *(bgcolor=\"\\#[^\"]*?\")* *>([^\\<]*?)\\<\\/td>", wxRE_ADVANCED|wxRE_ICASE );

	// set my target coordinates
	wxString strOrig=wxT("");
	strOrig.Printf( wxT("lat0=%.6lf&lat1=%.6lf&lon0=%.6lf&lon1=%.6lf"), lat, lat, lon, lon );
	// set my post data
	wxString strPostData = wxT("");
	strPostData.sprintf( wxT("%s%s"), m_strAuthInput, strOrig );
	// set my url
	wxString strUrl = wxT("");
	strUrl.sprintf( wxT("%s%s%s"), m_strUrl, wxT("cgi/ion-p?"), strPostData );

	// get page
	wxString strPage=wxT("");
	if( !GetWebPage( strUrl, strPage ) ) return( 0 );

	// clean font
	reFontEClean.ReplaceAll( &strPage, wxT("") );
	reFontClean.ReplaceAll( &strPage, wxT("") );
	reFontSigns.ReplaceAll( &strPage, wxT("") );

	// debug :: dump code
//	FILE* ff = fopen( "./bmm2.txt", "w" );
//	fputs( strPage, ff );
//	fclose( ff );

	// remove eol
//	strPage.Replace( "\r\n", "" );
//	strPage.Replace( "\r", "" );
//	strPage.Replace( "\n", "" );

	//////////////////
	// we search first for day tables
	int nRec=0, nCol=0;
	while( reDay.Matches( strPage ) )
	{
		// process date
		wxString strDate = reDay.GetMatch( strPage, 1 );
		wxDateTime date;
		date.ParseFormat( strDate, wxT("%d.%m.%Y") );
		pWeather->m_vectSeeingPredict[nRec].date = date.GetTicks();

		///////////////////////////
		// get my big fat day table
		wxString strDayTable = reDay.GetMatch( strPage, 2 );

		// look for :: High Clouds
		if( reHighClouds.Matches( strDayTable ) )
		{
			wxString strHighClouds = reHighClouds.GetMatch( strDayTable, 1 );
			// now process
			ProcessRow( nRec, 0, strHighClouds );
			// replace
			reHighClouds.Replace( &strDayTable, wxT(""), 1 );
		}

		// look for :: Mid Clouds
		if( reMidClouds.Matches( strDayTable ) )
		{
			wxString strMidClouds = reMidClouds.GetMatch( strDayTable, 1 );
			// now process
			ProcessRow( nRec, 1, strMidClouds );
			// replace
			reMidClouds.Replace( &strDayTable, wxT(""), 1 );
		}

		// look for :: Low Clouds
		if( reLowClouds.Matches( strDayTable ) )
		{
			wxString strLowClouds = reLowClouds.GetMatch( strDayTable, 1 );
			// now process
			ProcessRow( nRec, 2, strLowClouds );
			// replace
			reLowClouds.Replace( &strDayTable, wxT(""), 1 );
		}

		// look for :: seeing index 1
		if( reSeeingId1.Matches( strDayTable ) )
		{
			wxString strSeeingId1 = reSeeingId1.GetMatch( strDayTable, 1 );
			// now process
			ProcessRow( nRec, 3, strSeeingId1 );
			// replace
			reSeeingId1.Replace( &strDayTable, wxT(""), 1 );
		}

		// look for :: seeing index 2
		if( reSeeingId2.Matches( strDayTable ) )
		{
			wxString strSeeingId2 = reSeeingId2.GetMatch( strDayTable, 1 );
			// now process
			ProcessRow( nRec, 4, strSeeingId2 );
			// replace
			reSeeingId2.Replace( &strDayTable, wxT(""), 1 );
		}

		// look for :: seeing arcsec
		if( reSeeingArcSec.Matches( strDayTable ) )
		{
			wxString strSeeingArcSec = reSeeingArcSec.GetMatch( strDayTable, 1 );
			// now process
			ProcessRow( nRec, 5, strSeeingArcSec );
			// replace
			reSeeingArcSec.Replace( &strDayTable, wxT(""), 1 );
		}

		// look for :: Jet Stream
		if( reJet.Matches( strDayTable ) )
		{
			wxString strJet = reJet.GetMatch( strDayTable, 1 );
			// now process
			ProcessRow( nRec, 6, strJet );
			// replace
			reJet.Replace( &strDayTable, wxT(""), 1 );
		}

		// look for :: Relative Humidity
		if( reRelHum.Matches( strDayTable ) )
		{
			wxString strRelHum = reRelHum.GetMatch( strDayTable, 1 );
			// now process
			ProcessRow( nRec, 7, strRelHum );
			// replace
			reRelHum.Replace( &strDayTable, wxT(""), 1 );
		}

		// look for :: Bad Layers Top
		if( reBadTop.Matches( strDayTable ) )
		{
			wxString strBadTop = reBadTop.GetMatch( strDayTable, 1 );
			// now process
			ProcessRow( nRec, 8, strBadTop );
			// replace
			reBadTop.Replace( &strDayTable, wxT(""), 1 );
		}

		// look for :: Bad Layers Bot
		if( reBadBot.Matches( strDayTable ) )
		{
			wxString strBadBot = reBadBot.GetMatch( strDayTable, 1 );
			// now process
			ProcessRow( nRec, 9, strBadBot );
			// replace
			reBadBot.Replace( &strDayTable, wxT(""), 1 );
		}

		// look for :: Bad Layers K
		if( reBadK.Matches( strDayTable ) )
		{
			wxString strBadK = reBadK.GetMatch( strDayTable, 1 );
			// now process
			ProcessRow( nRec, 10, strBadK );
			// replace
			reBadK.Replace( &strDayTable, wxT(""), 1 );
		}

		// repleace all but last field - to keep to continue with next
		reDay.Replace( &strPage, wxT("\\3"), 1 );
		// increment record count
		nRec++;
	}
	// set found records
	pWeather->m_nSeeingPredict = nRec;

	// save my prediction data
	SaveData( lat, lon );

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
int CMeteoBlue::ProcessRow( int nRec, int nId, wxString& strText )
{
	wxRegEx reTdCElem( wxT("<td *(bgcolor=\\#[^>]*?)*>([^\\<]*?)\\<\\/td>"), wxRE_ADVANCED|wxRE_ICASE );
	int nCol = 0;

	// now loop through all values and set
	while( reTdCElem.Matches( strText ) )
	{
		if( nCol > 23 ) break;
		// :: get color
		wxString strColor = reTdCElem.GetMatch( strText, 1 ).Trim(0).Trim(1);
		if( !strColor.IsEmpty() )
		{
			//wxString strCCC = strColor.Mid( 10, 6 ); 
			int nRed=0, nGreen=0, nBlue=0;
			wxSscanf( strColor.Mid( 9, 2 ), wxT("%x"), &nRed );
			wxSscanf( strColor.Mid( 11, 2 ), wxT("%x"), &nGreen );
			wxSscanf( strColor.Mid( 13, 2 ), wxT("%x"), &nBlue );
			// set in my records
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].red[nId] = (unsigned char) nRed;
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].green[nId] = (unsigned char) nGreen;
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].blue[nId] = (unsigned char) nBlue;

		} else
		{
			// set to white
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].red[nId] = 255;
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].green[nId] = 255;
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].blue[nId] = 255;

		}

		// :: get value
		wxString strValue = reTdCElem.GetMatch( strText, 2 ).Trim(0).Trim(1);
		// set by type
		if( nId == 0 )
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].high_clouds = (unsigned char) wxAtoi( strValue );
		else if( nId == 1 )
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].mid_clouds = (unsigned char) wxAtoi( strValue );
		else if( nId == 2 )
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].low_clouds = (unsigned char) wxAtoi( strValue );
		else if( nId == 3 )
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].seeing_id_1 = (unsigned char) wxAtoi( strValue );
		else if( nId == 4 )
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].seeing_id_2 = (unsigned char) wxAtoi( strValue );
		else if( nId == 5 )
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].seeing_arcsec = (double) wxAtof( strValue );
		else if( nId == 6 )
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].jet_stream = (double) wxAtof( strValue );
		else if( nId == 7 )
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].rel_hum_2m = (unsigned char) wxAtoi( strValue );
		else if( nId == 8 )
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].bad_layers_top = (double) wxAtof( strValue );
		else if( nId == 9 )
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].bad_layers_bot = (double) wxAtof( strValue );
		else if( nId == 10 )
			m_pWeather->m_vectSeeingPredict[nRec].hour[nCol].bad_layers_k = (double) wxAtof( strValue );

		reTdCElem.Replace( &strText, wxT(""), 1 );
		nCol++;
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CMeteoBlue::SaveData( double lat, double lon )
{
	FILE* pFile = NULL;
	wxString strFile=wxT("");
	int i=0, j=0;

	// make file name
	strFile.Printf( wxT("%s/%.4lf_%.4lf.dat"), unMakeAppPath(wxT("data/weather/meteoblue")), lat, lon );

	// check if the folder exists
	unMakeDirIf( unMakeAppPath(wxT("data/weather/meteoblue")) );
	// open local file to write
	pFile = wxFopen( strFile, wxT("w") );
	if( !pFile ) return( 0 );

	// for every prediction day
	for( i=0; i<m_pWeather->m_nSeeingPredict; i++ )
	{
		wxFprintf( pFile, wxT("\n[%lu]\n"), (unsigned long) m_pWeather->m_vectSeeingPredict[i].date );

		// for all 24 hours :: high_clouds
		wxFprintf( pFile, wxT("HClouds=") );
		for( j=0; j<24; j++ )
		{
			if( j != 0 ) wxFprintf( pFile, wxT(",") );
			wxFprintf( pFile, wxT("%d:%d:%d:%d"), m_pWeather->m_vectSeeingPredict[i].hour[j].high_clouds,
					m_pWeather->m_vectSeeingPredict[i].hour[j].red[0], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].green[0], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].blue[0] );
		}
		wxFprintf( pFile, wxT("\n") );

		// for all 24 hours :: mid_clouds
		wxFprintf( pFile, wxT("MClouds=") );
		for( j=0; j<24; j++ )
		{
			if( j != 0 ) wxFprintf( pFile, wxT(",") );
			wxFprintf( pFile, wxT("%d:%d:%d:%d"), m_pWeather->m_vectSeeingPredict[i].hour[j].mid_clouds,
					m_pWeather->m_vectSeeingPredict[i].hour[j].red[1], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].green[1], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].blue[1] );
		}
		wxFprintf( pFile, wxT("\n") );

		// for all 24 hours :: low_clouds
		wxFprintf( pFile, wxT("LClouds=") );
		for( j=0; j<24; j++ )
		{
			if( j != 0 ) wxFprintf( pFile, wxT(",") );
			wxFprintf( pFile, wxT("%d:%d:%d:%d"), m_pWeather->m_vectSeeingPredict[i].hour[j].low_clouds,
					m_pWeather->m_vectSeeingPredict[i].hour[j].red[2], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].green[2], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].blue[2] );
		}
		wxFprintf( pFile, wxT("\n") );

		// for all 24 hours :: seeing_id_1
		wxFprintf( pFile, wxT("See1=") );
		for( j=0; j<24; j++ )
		{
			if( j != 0 ) wxFprintf( pFile, wxT(",") );
			wxFprintf( pFile, wxT("%d:%d:%d:%d"), m_pWeather->m_vectSeeingPredict[i].hour[j].seeing_id_1,
					m_pWeather->m_vectSeeingPredict[i].hour[j].red[3], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].green[3], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].blue[3] );
		}
		wxFprintf( pFile, wxT("\n") );

		// for all 24 hours :: seeing_id_2
		wxFprintf( pFile, wxT("See2=") );
		for( j=0; j<24; j++ )
		{
			if( j != 0 ) wxFprintf( pFile, wxT(",") );
			wxFprintf( pFile, wxT("%d:%d:%d:%d"), m_pWeather->m_vectSeeingPredict[i].hour[j].seeing_id_2,
					m_pWeather->m_vectSeeingPredict[i].hour[j].red[4], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].green[4], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].blue[4] );
		}
		wxFprintf( pFile, wxT("\n") );

		// for all 24 hours :: seeing_arcsec
		wxFprintf( pFile, wxT("SeeA=") );
		for( j=0; j<24; j++ )
		{
			if( j != 0 ) wxFprintf( pFile, wxT(",") );
			wxFprintf( pFile, wxT("%lf:%d:%d:%d"), m_pWeather->m_vectSeeingPredict[i].hour[j].seeing_arcsec,
					m_pWeather->m_vectSeeingPredict[i].hour[j].red[5], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].green[5], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].blue[5] );
		}
		wxFprintf( pFile, wxT("\n") );

		// for all 24 hours :: jet_stream
		wxFprintf( pFile, wxT("JetS=") );
		for( j=0; j<24; j++ )
		{
			if( j != 0 ) wxFprintf( pFile, wxT(",") );
			wxFprintf( pFile, wxT("%lf:%d:%d:%d"), m_pWeather->m_vectSeeingPredict[i].hour[j].jet_stream,
					m_pWeather->m_vectSeeingPredict[i].hour[j].red[6], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].green[6], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].blue[6] );
		}
		wxFprintf( pFile, wxT("\n") );

		// for all 24 hours :: rel_hum_2m
		wxFprintf( pFile, wxT("HumR=") );
		for( j=0; j<24; j++ )
		{
			if( j != 0 ) wxFprintf( pFile, wxT(",") );
			wxFprintf( pFile, wxT("%d:%d:%d:%d"), m_pWeather->m_vectSeeingPredict[i].hour[j].rel_hum_2m,
					m_pWeather->m_vectSeeingPredict[i].hour[j].red[7], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].green[7], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].blue[7] );
		}
		wxFprintf( pFile, wxT("\n") );

		// for all 24 hours :: bad_layers_top
		wxFprintf( pFile, wxT("BLTop=") );
		for( j=0; j<24; j++ )
		{
			if( j != 0 ) wxFprintf( pFile, wxT(",") );
			wxFprintf( pFile, wxT("%lf:%d:%d:%d"), m_pWeather->m_vectSeeingPredict[i].hour[j].bad_layers_top,
					m_pWeather->m_vectSeeingPredict[i].hour[j].red[8], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].green[8], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].blue[8] );
		}
		wxFprintf( pFile, wxT("\n") );

		// for all 24 hours :: bad_layers_bot
		wxFprintf( pFile, wxT("BLBot=") );
		for( j=0; j<24; j++ )
		{
			if( j != 0 ) wxFprintf( pFile, wxT(",") );
			wxFprintf( pFile, wxT("%lf:%d:%d:%d"), m_pWeather->m_vectSeeingPredict[i].hour[j].bad_layers_bot,
					m_pWeather->m_vectSeeingPredict[i].hour[j].red[9], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].green[9], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].blue[9] );
		}
		wxFprintf( pFile, wxT("\n") );

		// for all 24 hours :: bad_layers_k
		wxFprintf( pFile, wxT("BLK=") );
		for( j=0; j<24; j++ )
		{
			if( j != 0 ) wxFprintf( pFile, wxT(",") );
			wxFprintf( pFile, wxT("%lf:%d:%d:%d"), m_pWeather->m_vectSeeingPredict[i].hour[j].bad_layers_k,
					m_pWeather->m_vectSeeingPredict[i].hour[j].red[10], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].green[10], 
					m_pWeather->m_vectSeeingPredict[i].hour[j].blue[10] );
		}
		wxFprintf( pFile, wxT("\n") );
	}

	// close my file
	fclose( pFile );

	return( 1 );
}


////////////////////////////////////////////////////////////////////
int CMeteoBlue::LoadData( double lat, double lon )
{
	wxString strFile=wxT("");

	// make file name
	strFile.Printf( wxT("%s/%.4lf_%.4lf.dat"), unMakeAppPath(wxT("data/weather/meteoblue")), lat, lon );

	// check if file exists
	if( !wxFileExists(strFile) ) return( 0 );

	// then we verify time stamp - not older the half a day - 43200
	time_t ftime = wxFileModificationTime( strFile );
	time_t time_now = time(NULL);
	long time_diff = time_now-ftime;
	if( time_diff > 43200 ) return( 0 );

	// else we go ahead and load the file data
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxChar strTmp[2000];
	int nLineSize=0, bFoundSection=0;
	unsigned long nULong=0;
	double nFloat;
	int nDay=0, nCDay=0;
	int red=0, green=0, blue=0, intVal=0;

	// open file to read
	pFile = wxFopen( strFile, wxT("r") );
	// check if there is any configuration to load
	if( !pFile ) return( -1 );

	// Reading lines from cfg file
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
			char strName[255];
			wxSscanf( strLine, wxT("\[%lu\]\n"), &nULong );
			m_pWeather->m_vectSeeingPredict[nDay].date = (time_t) nULong;
			nCDay = nDay;
			nDay++;

		// :: high_clouds
		} else if( bFoundSection && wxStrncmp( strLine, wxT("HClouds="), 8 ) == 0 )
		{
			wxSscanf( strLine, wxT("HClouds=%[^\n]\n"), strTmp );
			// process with token by comma
			wxStringTokenizer tkz( strTmp, wxT(",") );
			int h=0;
			while ( tkz.HasMoreTokens() )
			{
				wxString strCell = tkz.GetNextToken();
				// extract value and color
				wxSscanf( strCell, wxT("%d:%d:%d:%d"), &intVal, &red, &green, &blue );
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].high_clouds = (unsigned char) intVal;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].red[0] = (unsigned char) red;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].green[0] = (unsigned char) green;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].blue[0] = (unsigned char) blue;
				h++;
			}

		// :: mid_clouds
		} else if( bFoundSection && wxStrncmp( strLine, wxT("MClouds="), 8 ) == 0 )
		{
			wxSscanf( strLine, wxT("MClouds=%[^\n]\n"), strTmp );
			// process with token by comma
			wxStringTokenizer tkz( strTmp, wxT(",") );
			int h=0;
			while ( tkz.HasMoreTokens() )
			{
				wxString strCell = tkz.GetNextToken();
				// extract value and color
				wxSscanf( strCell, wxT("%d:%d:%d:%d"), &intVal, &red, &green, &blue );
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].mid_clouds = (unsigned char) intVal;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].red[1] = (unsigned char) red;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].green[1] = (unsigned char) green;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].blue[1] = (unsigned char) blue;
				h++;
			}


		// :: low_clouds
		} else if( bFoundSection && wxStrncmp( strLine, wxT("LClouds="), 8 ) == 0 )
		{
			wxSscanf( strLine, wxT("LClouds=%[^\n]\n"), strTmp );
			// process with token by comma
			wxStringTokenizer tkz( strTmp, wxT(",") );
			int h=0;
			while ( tkz.HasMoreTokens() )
			{
				wxString strCell = tkz.GetNextToken();
				// extract value and color
				wxSscanf( strCell, wxT("%d:%d:%d:%d"), &intVal, &red, &green, &blue );
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].low_clouds = (unsigned char) intVal;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].red[2] = (unsigned char) red;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].green[2] = (unsigned char) green;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].blue[2] = (unsigned char) blue;
				h++;
			}

		// :: seeing_id_1
		} else if( bFoundSection && wxStrncmp( strLine, wxT("See1="), 5 ) == 0 )
		{
			wxSscanf( strLine, wxT("See1=%[^\n]\n"), strTmp );
			// process with token by comma
			wxStringTokenizer tkz( strTmp, wxT(",") );
			int h=0;
			while ( tkz.HasMoreTokens() )
			{
				wxString strCell = tkz.GetNextToken();
				// extract value and color
				wxSscanf( strCell, wxT("%d:%d:%d:%d"), &intVal, &red, &green, &blue );
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].seeing_id_1 = (unsigned char) intVal;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].red[3] = (unsigned char) red;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].green[3] = (unsigned char) green;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].blue[3] = (unsigned char) blue;
				h++;
			}

		// :: seeing_id_2
		} else if( bFoundSection && wxStrncmp( strLine, wxT("See2="), 5 ) == 0 )
		{
			wxSscanf( strLine, wxT("See2=%[^\n]\n"), strTmp );
			// process with token by comma
			wxStringTokenizer tkz( strTmp, wxT(",") );
			int h=0;
			while ( tkz.HasMoreTokens() )
			{
				wxString strCell = tkz.GetNextToken();
				// extract value and color
				wxSscanf( strCell, wxT("%d:%d:%d:%d"), &intVal, &red, &green, &blue );
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].seeing_id_2 = (unsigned char) intVal;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].red[4] = (unsigned char) red;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].green[4] = (unsigned char) green;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].blue[4] = (unsigned char) blue;
				h++;
			}

		// :: seeing_arcsec
		} else if( bFoundSection && wxStrncmp( strLine, wxT("SeeA="), 5 ) == 0 )
		{
			wxSscanf( strLine, wxT("SeeA=%[^\n]\n"), strTmp );
			// process with token by comma
			wxStringTokenizer tkz( strTmp, wxT(",") );
			int h=0;
			while ( tkz.HasMoreTokens() )
			{
				wxString strCell = tkz.GetNextToken();
				// extract value and color
				wxSscanf( strCell, wxT("%lf:%d:%d:%d"), &nFloat, &red, &green, &blue );
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].seeing_arcsec = nFloat;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].red[5] = (unsigned char) red;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].green[5] = (unsigned char) green;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].blue[5] = (unsigned char) blue;
				h++;
			}

		// :: jet_stream
		} else if( bFoundSection && wxStrncmp( strLine, wxT("JetS="), 5 ) == 0 )
		{
			wxSscanf( strLine, wxT("JetS=%[^\n]\n"), strTmp );
			// process with token by comma
			wxStringTokenizer tkz( strTmp, wxT(",") );
			int h=0;
			while ( tkz.HasMoreTokens() )
			{
				wxString strCell = tkz.GetNextToken();
				// extract value and color
				wxSscanf( strCell, wxT("%lf:%d:%d:%d"), &nFloat, &red, &green, &blue );
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].jet_stream = nFloat;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].red[6] = (unsigned char) red;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].green[6] = (unsigned char) green;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].blue[6] = (unsigned char) blue;
				h++;
			}

		// :: rel_hum_2m
		} else if( bFoundSection && wxStrncmp( strLine, wxT("HumR="), 5 ) == 0 )
		{
			wxSscanf( strLine, wxT("HumR=%[^\n]\n"), strTmp );
			// process with token by comma
			wxStringTokenizer tkz( strTmp, wxT(",") );
			int h=0;
			while ( tkz.HasMoreTokens() )
			{
				wxString strCell = tkz.GetNextToken();
				// extract value and color
				wxSscanf( strCell, wxT("%d:%d:%d:%d"), &intVal, &red, &green, &blue );
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].rel_hum_2m = (unsigned char) intVal;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].red[7] = (unsigned char) red;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].green[7] = (unsigned char) green;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].blue[7] = (unsigned char) blue;
				h++;
			}

		// :: bad_layers_top
		} else if( bFoundSection && wxStrncmp( strLine, wxT("BLTop="), 6 ) == 0 )
		{
			wxSscanf( strLine, wxT("BLTop=%[^\n]"), strTmp );
			// process with token by comma
			wxStringTokenizer tkz( strTmp, wxT(",") );
			int h=0;
			while ( tkz.HasMoreTokens() )
			{
				wxString strCell = tkz.GetNextToken();
				// extract value and color
				wxSscanf( strCell, wxT("%lf:%d:%d:%d"), &nFloat, &red, &green, &blue );
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].bad_layers_top = nFloat;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].red[8] = (unsigned char) red;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].green[8] = (unsigned char) green;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].blue[8] = (unsigned char) blue;
				h++;
			}

		// :: bad_layers_bot
		} else if( bFoundSection && wxStrncmp( strLine, wxT("BLBot="), 6 ) == 0 )
		{
			wxSscanf( strLine, wxT("BLBot=%[^\n]\n"), strTmp );
			// process with token by comma
			wxStringTokenizer tkz( strTmp, wxT(",") );
			int h=0;
			while ( tkz.HasMoreTokens() )
			{
				wxString strCell = tkz.GetNextToken();
				// extract value and color
				wxSscanf( strCell, wxT("%lf:%d:%d:%d"), &nFloat, &red, &green, &blue );
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].bad_layers_bot = nFloat;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].red[9] = (unsigned char) red;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].green[9] = (unsigned char) green;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].blue[9] = (unsigned char) blue;
				h++;
			}

		// :: bad_layers_k
		} else if( bFoundSection && wxStrncmp( strLine, wxT("BLK="), 4 ) == 0 )
		{
			wxSscanf( strLine, wxT("BLK=%[^\n]\n"), strTmp );
			// process with token by comma
			wxStringTokenizer tkz( strTmp, wxT(",") );
			int h=0;
			while ( tkz.HasMoreTokens() )
			{
				wxString strCell = tkz.GetNextToken();
				// extract value and color
				wxSscanf( strCell, wxT("%lf:%d:%d:%d"), &nFloat, &red, &green, &blue );
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].bad_layers_k = nFloat;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].red[10] = (unsigned char) red;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].green[10] = (unsigned char) green;
				m_pWeather->m_vectSeeingPredict[nCDay].hour[h].blue[10] = (unsigned char) blue;
				h++;
			}

		}

	}
	// close file
	fclose( pFile );

	// set number of prediction days
	m_pWeather->m_nSeeingPredict = nDay;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CMeteoBlue::IsToUpdate( double lat, double lon )
{
	wxString strFile;

	// make file name
	strFile.Printf( wxT("%s/%.4lf_%.4lf.dat"), unMakeAppPath(wxT("data/weather/meteoblue")), lat, lon );

	// check if file exists
	if( !wxFileExists(strFile) ) return( 1 );

	// then we verify time stamp - not older the half a day - 43200
	time_t ftime = wxFileModificationTime( strFile );
	time_t time_now = time(NULL);
	long time_diff = time_now-ftime;
	if( time_diff > 43200 ) return( 1 );

	return( 0 );
}