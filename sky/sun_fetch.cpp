
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
#include "../util/folders.h"
#include "../util/func.h"
#include "../util/webfunc.h"

// main header
#include "sun_fetch.h"

////////////////////////////////////////////////////////////////////
// Purpose:	Get latest sun images from soho nasa or mirror eas
////////////////////////////////////////////////////////////////////
int GetLatest_SunSohoImage( int sourceId, int nBand, wxString& strImgFile, double& nImgDateTime )
{
	// check band 
	if( nBand < 0 || nBand > 7 ) return( 0 );

	wxString strPage=wxT("");
	wxString strTmp;
	wxString strUrl;
	unsigned long nImgFileSize = 0;
	wxString strBaseUrl;
	wxString strUrlImgExt;
	// band strings
	wxString vectBand[8] = { wxT("171"), wxT("195"), wxT("284"), wxT("304"), wxT("mdi_igr"), wxT("mdi_mag"), wxT("c2"), wxT("c3") };
	// regex - compile by case
	wxRegEx reImg;
	if( nBand < 4 )
		strTmp.Printf( wxT("\\/data\\/realtime\\/javagif\\/gifs\\/([0-9]+)_([0-9]+)_eit_%s\\.gif"), vectBand[nBand] );
	else
		strTmp.Printf( wxT("\\/data\\/realtime\\/javagif\\/gifs\\/([0-9]+)_([0-9]+)_%s\\.gif"), vectBand[nBand] );

	reImg.Compile( strTmp );

	// check if source is nasa or esa
	if( sourceId == 0 )
	{
		strBaseUrl = wxT("http://soho.esac.esa.int");
		strUrlImgExt = wxT("gif");

	} else if( sourceId == 1 )
	{
		strBaseUrl = wxT("http://sohowww.nascom.nasa.gov");
		strUrlImgExt = wxT("jpg");

	} else
		return( 0 );

	//////////////////////
	// build list url
	strUrl.Printf( wxT("http://soho.esac.esa.int/data/realtime/realtime-%s-1024.html"), vectBand[nBand] );

	// get page
	if( !GetWebPage( strUrl, strPage ) ) return( 0 );

	wxString strImgTime = wxT("");
	wxString strImgDate = wxT("");
	wxDateTime datetimeImg;
	while( reImg.Matches( strPage ) )
	{
		// get image date
		strImgDate = reImg.GetMatch( strPage, 1 ).Trim(0).Trim(1);
		// get image time 
		strImgTime = reImg.GetMatch( strPage, 2 ).Trim(0).Trim(1);
		// process image date/time
		strTmp.Printf( wxT("%s/%s/%s %s:%s:00"), 
						// date
						strImgDate.Mid( 0, 4 ),
						strImgDate.Mid( 4, 2 ),
						strImgDate.Mid( 6, 2 ),
						// time
						strImgTime.Mid( 0, 2 ), 
						strImgTime.Mid( 2, 2 ) );
		datetimeImg.ParseFormat( strTmp, wxT("%Y/%m/%d %H:%M:%S") );
	
		// break after first match .. no more needed
		break;
		//reImg.Replace( &strPage, "", 1 );
		//strPage = strPage.Trim(0).Trim(1);
	}

	// check if there is an image from meteosat8
	if( strImgDate.IsEmpty() || strImgTime.IsEmpty() ) return( 0 );
	// else get image 
	// :: set image date/time
	nImgDateTime = datetimeImg.GetJDN();
	// :: build date/time string
	strTmp.Printf( wxT("%s"), datetimeImg.Format( wxT("%d%m%Y_%H%M%S") ) );

	/////////////////
	// build image url
	if( nBand < 4 )
		strUrl.Printf( wxT("%s/data/realtime/javagif/gifs/%s_%s_eit_%s.%s"), 
				strBaseUrl, strImgDate, strImgTime, vectBand[nBand], strUrlImgExt );
	else
		strUrl.Printf( wxT("%s/data/realtime/javagif/gifs/%s_%s_%s.%s"), 
				strBaseUrl, strImgDate, strImgTime, vectBand[nBand], strUrlImgExt );
	// build image file name
	strImgFile.Printf( wxT("%s/soho_%s.%s"), unMakeAppPath(wxT("data/sun")), vectBand[nBand], strTmp, strUrlImgExt );

	// now check if file exist
	if( !wxFileExists( wxString(strImgFile,wxConvUTF8) ) )
		if( !LoadWebFileT( strUrl, strImgFile, NULL, nImgFileSize, 1 ) ) return( 0 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Purpose:	Get latest sun images from soho nasa or mirror eas
////////////////////////////////////////////////////////////////////
int GetLatest_SunXrtImage( int nType, wxString& strImgFile, double& nImgDateTime )
{
	wxString strPage=wxT("");
	wxString strTmp;
	wxString strUrl;
	unsigned long nImgFileSize = 725261;
	// regex
	wxRegEx reImg( wxT("\\>\\ +XRT_SYN_([0-9]+)_([0-9]+).([0-9]+).1024.png\\<\\/") );

	// get current date
	wxDateTime timeNow;
	timeNow.SetToCurrent();
	int nYear = timeNow.FromUTC().GetYear();
	int nMonth = timeNow.FromUTC().GetMonth()+1;

	// build list url
	strUrl.Printf( wxT("http://xrt.cfa.harvard.edu/data/%d/%.2d/"), nYear, nMonth );

	// get page
	if( !GetWebPage( strUrl, strPage ) ) return( 0 );

	wxString strImgTime = wxT("");
	wxString strImgDate = wxT("");
	wxString strImgCount = wxT("");
	wxDateTime datetimeImg;
	while( reImg.Matches( strPage ) )
	{
		// get image date
		strImgDate = reImg.GetMatch( strPage, 1 ).Trim(0).Trim(1);
		// get image time 
		strImgTime = reImg.GetMatch( strPage, 2 ).Trim(0).Trim(1);
		// process image date/time
		strTmp.Printf( wxT("%s/%s/%s %s:%s:%s"), 
						// date
						strImgDate.Mid( 0, 4 ),
						strImgDate.Mid( 4, 2 ),
						strImgDate.Mid( 6, 2 ),
						// time
						strImgTime.Mid( 0, 2 ), 
						strImgTime.Mid( 2, 2 ), 
						strImgTime.Mid( 4, 2 ) );
		datetimeImg.ParseFormat( strTmp, wxT("%Y/%m/%d %H:%M:%S") );
		// get image count
		strImgCount = reImg.GetMatch( strPage, 3 ).Trim(0).Trim(1);

		reImg.Replace( &strPage, wxT(""), 1 );
		strPage = strPage.Trim(0).Trim(1);
	}

	// check if there is an image from meteosat8
	if( strImgDate.IsEmpty() || strImgTime.IsEmpty() ) return( 0 );
	// else get image 
	// :: set image date/time
	nImgDateTime = datetimeImg.GetJDN();
	// :: build date/time string
	strTmp.Printf( wxT("%s"), datetimeImg.Format( wxT("%d%m%Y_%H%M%S") ) );

	// build file name - also use img count for the day or ??
	strImgFile.Printf( wxT("%s/xrt_%s_%s.png"), unMakeAppPath(wxT("data/sun")), strImgCount, strTmp );

	// build image url
	strUrl.Printf( wxT("http://xrt.cfa.harvard.edu/data/%d/%.2d/XRT_SYN_%s_%s.%s.1024.png"), 
						nYear, nMonth, strImgDate, strImgTime, strImgCount );

	// now check if file exist
	if( !wxFileExists( wxString(strImgFile,wxConvUTF8) ) )
		if( !LoadWebFileT( strUrl, strImgFile, NULL, nImgFileSize, 1 ) ) return( 0 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Purpose:	get last solar event from lockhead martin site ...
////////////////////////////////////////////////////////////////////
int GetLatest_SunEventImageLmsal( int nType, wxString& strImgFile, double& nImgDateTime )
{
	wxString strPage=wxT("");
	wxString strTmp;
	wxString strUrl;
	unsigned long nImgFileSize = 725261;
	// regex
	wxRegEx reImg( wxT("\\/solarsoft\\/l1q_synop\\/pfss_([0-9]+)_([0-9]+)_Bfield_([0-9]+)_([0-9]+)_full_eclip\\.png") );

	// build list url
	strUrl = wxT( "http://www.lmsal.com/solarsoft/last_events/" );

	// get page
	if( !GetWebPage( strUrl, strPage ) ) return( 0 );

	wxString strImgFromDate = wxT(""), strImgToDate = wxT("");
	wxString strImgFromTime = wxT(""), strImgToTime = wxT("");
	wxDateTime datetimeToImg, datetimeFromImg;
	while( reImg.Matches( strPage ) )
	{
		// get image To date
		strImgToDate = reImg.GetMatch( strPage, 1 ).Trim(0).Trim(1);
		// get image To time 
		strImgToTime = reImg.GetMatch( strPage, 2 ).Trim(0).Trim(1);
		// process image date/time
		strTmp.Printf( wxT("%s/%s/%s %s:%s:00"), 
						// date
						strImgToDate.Mid( 0, 4 ),
						strImgToDate.Mid( 4, 2 ),
						strImgToDate.Mid( 6, 2 ),
						// time
						strImgToTime.Mid( 0, 2 ), 
						strImgToTime.Mid( 2, 2 ) );
		datetimeToImg.ParseFormat( strTmp, wxT("%Y/%m/%d %H:%M:%S") );

		// get image From date
		strImgFromDate = reImg.GetMatch( strPage, 3 ).Trim(0).Trim(1);
		// get image From time 
		strImgFromTime = reImg.GetMatch( strPage, 4 ).Trim(0).Trim(1);
		// process image date/time
		strTmp.Printf( wxT("%s/%s/%s %s:%s:00"), 
						// date
						strImgFromDate.Mid( 0, 4 ),
						strImgFromDate.Mid( 4, 2 ),
						strImgFromDate.Mid( 6, 2 ),
						// time
						strImgFromTime.Mid( 0, 2 ), 
						strImgFromTime.Mid( 2, 2 ) );
		datetimeFromImg.ParseFormat( strTmp, wxT("%Y/%m/%d %H:%M:%S") );

		// then break 
		break;
		//reImg.Replace( &strPage, "", 1 );
		//strPage = strPage.Trim(0).Trim(1);
	}

	// check if there is an image from meteosat8
	if( strImgFromDate.IsEmpty() || strImgFromTime.IsEmpty() ) return( 0 );
	// else get image 
	// :: set image date/time
	nImgDateTime = datetimeToImg.GetJDN();
	// :: build date/time string
	strTmp.Printf( wxT("%s"), datetimeToImg.Format( wxT("%d%m%Y_%H%M%S") ) );

	// build file name - also use img count for the day or ??
	strImgFile.Printf( wxT("%s/lmsal_event_%s.png"), unMakeAppPath(wxT("data/sun")), strTmp );

	// build image url
	strUrl.Printf( wxT("http://www.lmsal.com/solarsoft/l1q_synop/pfss_%s_%s_Bfield_%s_%s_full_eclip.png"), 
						strImgToDate, strImgToTime, strImgFromDate, strImgFromTime );

	// now check if file exist
	if( !wxFileExists( strImgFile ) )
		if( !LoadWebFileT( strUrl, strImgFile, NULL, nImgFileSize, 1 ) ) return( 0 );

	return( 1 );
}