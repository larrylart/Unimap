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
#include "../util/units.h"

// main header
#include "weather_under.h"

////////////////////////////////////////////////////////////////////
int CWeatherUnder::Get_StationId( double lat, double lon, wxString& strStationId )
{
	wxXmlDocument xml;
//	wxString strPage="";
	wxString strUrl=wxT("");
	wxString strTmp=wxT("");
	unsigned long nVarULong;
	double nVarDouble;
	int bFound=0;

	// build url
	strTmp.Printf( wxT("%.6f%s%.6f"), lat, wxT("%2C"), lon );
	strUrl.Printf( wxT("http://www.wunderground.com/auto/wui/geo/GeoLookupXML/index.xml?query=%s"), strTmp );
	// get page
	if( !GetWebXmlDoc( strUrl, xml ) ) return( 0 );

	/////////////
	// parse XML
	// start processing the XML file
	if( xml.GetRoot()->GetName() != wxT("location") )
		return( 0 );

	wxXmlNode *child = xml.GetRoot()->GetChildren();
	while( child && !bFound )
	{
		// get tag
		if( child->GetName() == wxT("nearby_weather_stations") )
		{
			wxXmlNode* child2 = child->GetChildren();
			if( child2 && child2->GetName() == wxT("airport") )
			{
				wxXmlNode* child3 = child2->GetChildren();
				if( child3 && child3->GetName() == wxT("station") )
				{
					wxXmlNode* child4 = child3->GetChildren();
					while( child4 )
					{
						if( child4->GetName() == wxT("icao") )
						{
							strStationId = child4->GetNodeContent();
							bFound = 1;
							break;
						}
						child4 = child4->GetNext();
					}
				}

			} 
			// check next 
			child2 = child2->GetNext();
			if( child2 && child2->GetName() == wxT("pws") )
			{
				wxXmlNode* child3 = child2->GetChildren();
				if( child3 && child3->GetName() == wxT("station") )
				{
					wxXmlNode* child4 = child3->GetChildren();
					while( child4 )
					{
						if( child4->GetName() == wxT("id") )
						{
							strStationId = child4->GetNodeContent();
							bFound = 1;
							break;
						}
						child4 = child4->GetNext();
					}
				}

			}
		}
		// next child
		child = child->GetNext();
	}

	return( bFound );
}

////////////////////////////////////////////////////////////////////
int CWeatherUnder::Get_Current( DefWeatherRecord& rec, double lat, double lon )
{
	wxXmlDocument xml;
//	wxString strPage="";
	wxString strUrl=wxT("");
	wxString strTmp=wxT("");
	unsigned long nVarULong;
	double nVarDouble;
	int nState=0;

	// build url
	strTmp.Printf( wxT("%.6f%s%.6f"), lat, wxT("%2C"), lon );
	//wxString strData = URLEncode( strTmp );
	strUrl.Printf( wxT("http://www.wunderground.com/auto/wui/geo/WXCurrentObXML/index.xml?query=%s"), strTmp );
	// get page
	if( !GetWebXmlDoc( strUrl, xml ) ) return( 0 );

	/////////////
	// parse XML
	// start processing the XML file
	if( xml.GetRoot()->GetName() != wxT("current_observation") )
		return( 0 );

	wxXmlNode *child = xml.GetRoot()->GetChildren();
	while( child )
	{
		// get tag
		if( child->GetName() == wxT("station_id") )
		{
			wxStrcpy( rec.site_id, child->GetNodeContent() );

		} else if( child->GetName() == wxT("observation_epoch") ) 
		{
			// get content of my tag
			if( !child->GetNodeContent().ToULong( &nVarULong ) ) nVarULong = 0;
			time_t timeObs = (time_t) nVarULong;
			wxDateTime dateObs( timeObs );
			rec.datetime = dateObs.GetJDN();

			// process properties of <tag1>
//			wxString propvalue1 = child->GetPropVal(wxT("prop1"), wxT("default-value"));

		} else if( child->GetName() == wxT("temp_c") )
		{
			if( !child->GetNodeContent().ToDouble( &nVarDouble ) ) 
				nVarDouble = 0.0;
			else
				nState = 1;
			rec.temperature = nVarDouble;

		} else if( child->GetName() == wxT("relative_humidity") )
		{
			if( !child->GetNodeContent().RemoveLast().ToDouble( &nVarDouble ) ) nVarDouble = 0.0;
			rec.humidity = nVarDouble;

		} else if( child->GetName() == wxT("wind_degrees") )
		{
			if( !child->GetNodeContent().ToDouble( &nVarDouble ) ) nVarDouble = 0.0;
			rec.wind_dir = nVarDouble;

		} else if( child->GetName() == wxT("wind_dir") )
		{
			wxStrcpy( rec.wind_dir_str, child->GetNodeContent() );

		} else if( child->GetName() == wxT("wind_mph") )
		{
			if( !child->GetNodeContent().ToDouble( &nVarDouble ) ) nVarDouble = 0.0;
			rec.wind = Convert_MilesToKm( nVarDouble );

		} else if( child->GetName() == wxT("windchill_c") )
		{
			if( !child->GetNodeContent().ToDouble( &nVarDouble ) ) nVarDouble = 0.0;
			rec.windchill = nVarDouble;

		} else if( child->GetName() == wxT("pressure_mb") )
		{
			if( !child->GetNodeContent().ToDouble( &nVarDouble ) ) nVarDouble = 0.0;
			rec.pressure = nVarDouble;

		} else if( child->GetName() == wxT("dewpoint_c") )
		{
			if( !child->GetNodeContent().ToDouble( &nVarDouble ) ) nVarDouble = 0.0;
			rec.dew_point = nVarDouble;

		} else if( child->GetName() == wxT("heat_index_c") )
		{
			if( !child->GetNodeContent().ToDouble( &nVarDouble ) ) nVarDouble = 0.0;
			rec.heat_index = nVarDouble;

		} else if( child->GetName() == wxT("visibility_km") )
		{
			if( !child->GetNodeContent().ToDouble( &nVarDouble ) ) nVarDouble = 0.0;
			rec.visibility = nVarDouble;

		} else if( child->GetName() == wxT("icon") )
		{
			wxStrcpy( rec.cond_icon, child->GetNodeContent() );

		} else if( child->GetName() == wxT("weather") )
		{
			wxStrcpy( rec.cond_text, child->GetNodeContent() );
		}

		// get next key
		child = child->GetNext();
	}


	return( nState );
}

////////////////////////////////////////////////////////////////////
int CWeatherUnder::Get_Historical( DefWeatherRecord* vectRec, int& nRec,
									double lat, double lon, double time )
{
	wxString strStationId=wxT("");
	wxString strPage=wxT("");
	wxString strField=wxT("");
	wxString strUrl=wxT("");
	wxString strTmp=wxT("");
	unsigned long nVarULong;
	double nVarDouble;
	// regex
	// 12:00 AM,-11,-13,83,1042,20,SW,2.2,,,,,AAXX 30221 15260 22998 02301 11114 21132 39835 40421 57002 333 60005 91001 91102 92612<br />
	wxRegEx reLine( wxT("^([^\\<]*)\\<br[\\ \\/]*\\>") );

	// check if station exists
	if( !Get_StationId( lat, lon, strStationId ) ) return( 0 );

	// buid url
	wxDateTime timeObs( time );
	int nYear = timeObs.GetYear();
	int nMonth = timeObs.GetMonth()+1;
	int nDay = timeObs.GetDay();
	//strUrl.Printf( wxT("http://www.wunderground.com/history/airport/%s/%d/%d/%d/DailyHistory.html?format=1"), 
	//					strStationId, nYear, nMonth, nDay );
	// new format :: http://www.wunderground.com/weatherstation/WXDailyHistory.asp?ID=IBUENOSA135&day=9&year=2013&month=4&graphspan=day&format=1
	strUrl.Printf( wxT("http://www.wunderground.com/weatherstation/WXDailyHistory.asp?ID=%s&day=%d&year=%d&month=%d&graphspan=day&format=1"), 
						strStationId, nDay, nYear, nMonth );

	// get page
	if( !GetWebPage( strUrl, strPage ) ) return( 0 );

	int nCount=0;
	nRec=0;
	// go through the string
	while( reLine.Matches( strPage ) )
	{
		// break if got max
		if( nRec >= 100 ) break;

		// get line
		wxString strLine = reLine.GetMatch( strPage, 1 ).Trim(0).Trim(1);
		reLine.Replace( &strPage, wxT(""), 1 );
		strPage = strPage.Trim(0).Trim(1);
		// if first go to next
		if( nCount == 0 )
		{
			nCount++;
			continue;
		}

		// loop through the tokens
		//Time,TemperatureC,DewpointC,PressurehPa,WindDirection,WindDirectionDegrees,WindSpeedKMH,WindSpeedGustKMH,
		// Humidity,HourlyPrecipMM,Conditions,Clouds,dailyrainMM,SoftwareType,DateUTC
		wxStringTokenizer tkz( strLine, wxT(",") );
		//while ( tkz.HasMoreTokens() )
		// :: field :: time
		strField = tkz.GetNextToken().Trim(0).Trim(1);
		strTmp.Printf( wxT("%d/%d/%d %s"), nMonth, nDay, nYear, strField );
		timeObs.ParseDateTime( strTmp );
		vectRec[nRec].datetime = timeObs.FromUTC().GetJDN();
		// :: field :: Temperature
		strField = tkz.GetNextToken().Trim(0).Trim(1);
		if( !strField.ToDouble( &(vectRec[nRec].temperature) ) ) vectRec[nRec].temperature = 0.0;
		// :: field :: Dew PointC
		strField = tkz.GetNextToken().Trim(0).Trim(1);
		if( !strField.ToDouble( &(vectRec[nRec].dew_point) ) ) vectRec[nRec].dew_point = 0.0;
		// :: field :: Sea Level PressurehPa
		strField = tkz.GetNextToken().Trim(0).Trim(1);
		if( !strField.ToDouble( &(vectRec[nRec].pressure) ) ) vectRec[nRec].pressure = 0.0;
		// :: field :: Wind Direction
		strField = tkz.GetNextToken().Trim(0).Trim(1);
//		if( !strField.ToDouble( &(vectRec[nRec].wind_dir) ) ) vectRec[nRec].wind_dir = 0.0;
		// :: field :: WindDirectionDegrees
		strField = tkz.GetNextToken().Trim(0).Trim(1);
		// :: field :: Wind SpeedKm/h
		strField = tkz.GetNextToken().Trim(0).Trim(1);
		if( !strField.ToDouble( &(vectRec[nRec].wind) ) ) vectRec[nRec].wind = 0.0;
		// :: field :: WindSpeedGustKMH
		strField = tkz.GetNextToken().Trim(0).Trim(1);
		// :: field :: Humidity
		strField = tkz.GetNextToken().Trim(0).Trim(1);
		if( !strField.ToDouble( &(vectRec[nRec].humidity) ) ) vectRec[nRec].humidity = 0.0;
		// HourlyPrecipMM
		strField = tkz.GetNextToken().Trim(0).Trim(1);

		// :: field :: VisibilityKm - this was no more
		//strField = tkz.GetNextToken().Trim(0).Trim(1);
		//if( !strField.ToDouble( &(vectRec[nRec].visibility) ) ) vectRec[nRec].visibility = 0.0;

/*		// :: field :: Gust SpeedKm/h
		strField = tkz.GetNextToken().Trim(0).Trim(1);
		// :: field :: PrecipitationCm
		strField = tkz.GetNextToken().Trim(0).Trim(1);
		// :: field :: Events
		strField = tkz.GetNextToken().Trim(0).Trim(1);
		// :: field :: Conditions
		strField = tkz.GetNextToken().Trim(0).Trim(1);
		// :: field :: FullMetar
		strField = tkz.GetNextToken().Trim(0).Trim(1);
*/		
		// increment counter
		nRec++;
	}

	return( 1 );
}

// Purpose:	get forcast xml
////////////////////////////////////////////////////////////////////
int CWeatherUnder::Get_Forecast( DefWeatherForecastRecord* vectRec, int& nRec, double lat, double lon )
{
	wxXmlDocument xml;
	wxString strUrl=wxT("");
	wxString strTmp=wxT("");
	unsigned long nVarULong;
	double nVarDouble;
	int nRecId=0;

	// build url
	strTmp.Printf( wxT("%.6f%s%.6f"), lat, wxT("%2C"), lon );
	strUrl.Printf( wxT("http://www.wunderground.com/auto/wui/geo/ForecastXML/index.xml?query=%s"), strTmp );
	// get page
	if( !GetWebXmlDoc( strUrl, xml ) ) return( 0 );

	/////////////
	// parse XML
	// start processing the XML file
	if( xml.GetRoot()->GetName() != wxT("forecast") )
		return( 0 );

	wxXmlNode *child = xml.GetRoot()->GetChildren();
	while( child )
	{
		// get tag : simpleforecast - forecastday - date(when)..not as important
		// simpleforecast - forecastday : high - celsius, low - celsius, conditions, icon, skyicon
		if( child->GetName() == wxT("simpleforecast") )
		{
			//strcpy( rec.site_id, child->GetNodeContent().GetData() );

			/////////////////
			// next node :: forecastday 1..7
			wxXmlNode* child2 = child->GetChildren();
			while( child2 )
			{
				// for each forecast day
				if( child2 && child2->GetName() == wxT("forecastday") )
				{
					// reset current record to zero
					memset( &(vectRec[nRecId]), 0, sizeof(DefWeatherForecastRecord) );

					wxXmlNode* child3 = child2->GetChildren();
					while( child3 )
					{
						// period
						if( child3 && child3->GetName() == wxT("period") )
						{
							// period - number 1...7 ?
							wxString strPeriod = child3->GetNodeContent();
							if( strPeriod.ToULong( &nVarULong ) )
								vectRec[nRecId].basic.id = (int) nVarULong;

						///////////
						// date
						} else if( child3 && child3->GetName() == wxT("date") )
						{
							// date fields
							wxXmlNode* child4 = child3->GetChildren();
							while( child4 )
							{
								// epoch
								if( child4->GetName() == wxT("epoch") )
								{
									wxString strEpoch = child4->GetNodeContent();
									if( strEpoch.ToULong( &nVarULong ) )
										vectRec[nRecId].epoch = (time_t) nVarULong;

								}
								//if( child4->GetName() == wxT("weekday") )
								//{
								//	strcpy( vectRec[nRecId].weekday, child4->GetNodeContent().GetData() );
								//}

								// get next child
								child4 = child4->GetNext();
							}

						//////////////
						// high
						} else if( child3 && child3->GetName() == wxT("high") )
						{
							// high :: celsius
							wxXmlNode* child4 = child3->GetChildren();
							while( child4 )
							{
								if( child4->GetName() == wxT("celsius") )
								{
									wxString strTempHigh = child4->GetNodeContent();
									if( strTempHigh.ToDouble( &nVarDouble ) )
										vectRec[nRecId].temperature_day = (int) nVarDouble;
								}
								child4 = child4->GetNext();
							}

						//////////////
						// low
						} else if( child3 && child3->GetName() == wxT("low") )
						{
							// low :: celsius
							wxXmlNode* child4 = child3->GetChildren();
							while( child4 )
							{
								if( child4->GetName() == wxT("celsius") )
								{
									wxString strTempLow = child4->GetNodeContent();
									if( strTempLow.ToDouble( &nVarDouble ) )
										vectRec[nRecId].temperature_night = (int) nVarDouble;
								}
								child4 = child4->GetNext();
							}

						///////////////////
						// conditions - Partly Cloudy
						} else if( child3 && child3->GetName() == wxT("conditions") )
						{
							// conditions
							wxStrcpy( vectRec[nRecId].basic.cond_text, child3->GetNodeContent() );

						/////////////////
						// icon
						} else if( child3 && child3->GetName() == wxT("icon") )
						{
							// icon
							wxStrcpy( vectRec[nRecId].basic.cond_icon, child3->GetNodeContent() );

						/////////////////
						// skyicon
						} else if( child3 && child3->GetName() == wxT("skyicon") )
						{
							// skyicon
							wxStrcpy( vectRec[nRecId].basic.skyicon, child3->GetNodeContent() );
						}

						// get next node 2
						child3 = child3->GetNext();
					}

					nRecId++;
				}

				// get next node 2
				child2 = child2->GetNext();
			}

		////////////////////
		// :: moon_phase, sunset, sunrise
		} if( child && child->GetName() == wxT("moon_phase") )
		{
			// percentIlluminated, ageOfMoon
			// sunset
			// sunrise
		}

		// get next key
		child = child->GetNext();
	}

	// set final count
	nRec = nRecId;

	return( 1 );
}

// Purpose:	get current and forecast based on the mobile html pharser
////////////////////////////////////////////////////////////////////
int CWeatherUnder::Get_CurrentAndForecastHtml( DefWeatherRecord& rec, 
											  DefWeatherForecastRecord* vectRec, 
											  int& nRec, double lat, double lon )
{


	return( 1 );
}

// Purpose:	get forecast based on the mobile html pharser
////////////////////////////////////////////////////////////////////
int CWeatherUnder::Get_Forecast_viaHtml( CWeather* pWeather, double lat, double lon )
{
	// check time stamp of last update - to be > 30 minutes = 1800 seconds
	time_t tnow = time(NULL);
	long nDiff = tnow - pWeather->m_nForecastFetchTime;
	// very difference
	if( nDiff < 1800 ) return( 1 );

	int i=0, j=0, nBaseRec=7;
	
	pWeather->m_nFirstForecastRec = 0;

	//////////////////////
	// get main forecast list
	Get_Forecast_Overview_viaHtml( pWeather, lat, lon );

	// option 0 - fetch only first record
	Get_Forecast_Daily_LiteHtml( 0, pWeather->m_vectForecastRec, pWeather->m_nFirstForecastRec, lat, lon );

	return( 1 );

	/////////////////////
	// get detailed all 7 days of available forecast
	for( i=pWeather->m_nFirstForecastRec; i<pWeather->m_nForecastRec; i++ )
	{
		// fetch/pharse html 
		Get_Forecast_Daily_LiteHtml( i, pWeather->m_vectForecastRec, pWeather->m_nFirstForecastRec, lat, lon );
		// & calculates daily averages
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CWeatherUnder::Get_Forecast_Overview_viaHtml( CWeather* pWeather, double lat, double lon )
{
	wxString strPage=wxT("");
	wxString strUrl=wxT("");
	wxString strTmp=wxT("");
	int nRec=0;
	// regex
	wxRegEx reCurrentTable( wxT("> *Updated: *\\<b\\> *(.*) +([A-Z]+) +on +(.*?) +([0-9]+)\\, *([0-9]+)\\<\\/(.*?)\\<\\/table\\>"),  wxRE_ADVANCED );
	wxRegEx reForecastTable( wxT("\\>Forecast as of (.*?) +on +(.*?) +([0-9]+) *\\, *([0-9]+) *\\<(.*?)\\<\\/table\\>"),  wxRE_ADVANCED );
	wxRegEx reDay( wxT("\\<td.*?\\<img src\\=.*\\/(.*?)\\.gif.*?\\<td.*?\\<b\\> *(.*?) *\\<.*?\\<br *\\/\\>(.*?)\\. *(Low|High) *\\: *([0-9]*) *\\&deg\\;"),  wxRE_ADVANCED );
	// current regex elem
	wxRegEx reTemperature( wxT("\\>Temperature\\<.*?\\<b\\> *([0-9]*) *\\<\\/b\\>"), wxRE_ADVANCED );
	wxRegEx reHumidity( wxT("\\>Humidity\\<.*?\\<b\\> *([0-9]*)\\% *\\<\\/b\\>"), wxRE_ADVANCED );
	wxRegEx reDew( wxT("\\>Dew Point\\<.*?\\<b\\> *([0-9]*) *\\<\\/b\\>"), wxRE_ADVANCED );
	wxRegEx reWind( wxT("\\>Wind\\<.*?\\<b\\> *([NEWS]*) *\\<\\/b\\>.*?\\<b\\> *([0-9]*) *\\<\\/b\\>"), wxRE_ADVANCED );
	wxRegEx rePressure( wxT("\\>Pressure\\<.*?\\<b\\> *([0-9]*) *\\<\\/b\\>.*?\\<b\\> *\\((.*?)\\) *\\<\\/b\\>"), wxRE_ADVANCED );
	wxRegEx reCond( wxT("\\>Conditions\\<.*?\\<b\\> *(.*?) *\\<\\/b\\>"), wxRE_ADVANCED );
	wxRegEx reVisibility( wxT("\\>Visibility\\<.*?\\<b\\> *([0-9\\.]*) *\\<\\/b\\>"), wxRE_ADVANCED );
	wxRegEx reUV( wxT("\\<td\\>UV\\<\\/td\\>.*?\\<td\\> *([0-9]*) +out +of +([0-9]*) *\\<\\/td\\>"), wxRE_ADVANCED );
	wxRegEx reClouds( wxT("\\<td\\>Clouds\\<\\/td\\>.*?\\<td\\>(.*?)\\<\\/td\\>"), wxRE_ADVANCED );

	// build url
	strTmp.Printf( wxT("%.6f%s%.6f"), lat, wxT("%2C"), lon );
	// strUrl.Printf( wxT("http://www.wunderground.com/cgi-bin/findweather/getForecast?brand=mobile_metric&query=%s"), strTmp );
	// http://mobile.wunderground.com/cgi-bin/findweather/getForecast?brand=mobile_metric&query=
	strUrl.Printf( wxT("http://mobile.wunderground.com/cgi-bin/findweather/getForecast?brand=mobile_metric&query=%s"), strTmp );

	// debug :: dump code
//	FILE* fft = fopen( "./wudo_b.txt", "w" );
//	fputs( "tttest", fft );
//	fclose( fft );

	// get page
	if( !GetWebPage( strUrl, strPage ) ) return( 0 );

	// debug :: dump code
//	FILE* ff = fopen( "./wudo.txt", "w" );
//	fputs( strPage.ToAscii(), ff );
//	fclose( ff );

	// remore characters
	strPage.Replace( wxT("\n"), wxT("") );
	strPage.Replace( wxT("\r"), wxT("") );
	strPage.Replace( wxT("\t"), wxT("") );

	/////////////////////////////////////////
	// check if I got current conditions table
	if( reCurrentTable.Matches( strPage ) )
	{
		// get start forecast date/time
		wxString strTime = reCurrentTable.GetMatch( strPage, 1 ).Trim(0).Trim(1);
		int nUpdateDay = wxAtoi( reCurrentTable.GetMatch( strPage, 4 ).Trim(0).Trim(1) );

		// get forecast table
		wxString strCurrentTable = reCurrentTable.GetMatch( strPage, 6 ).Trim(0).Trim(1);

		// :: temperature
		if( reTemperature.Matches( strCurrentTable ) )
		{
			pWeather->m_rCurrent.temperature = wxAtoi( reTemperature.GetMatch( strCurrentTable, 1 ).Trim(0).Trim(1) );
			reTemperature.Replace( &strCurrentTable, wxT(""), 1 );
		}
		// :: humidity
		if( reHumidity.Matches( strCurrentTable ) )
		{
			pWeather->m_rCurrent.humidity = wxAtoi( reHumidity.GetMatch( strCurrentTable, 1 ).Trim(0).Trim(1) );
			reHumidity.Replace( &strCurrentTable, wxT(""), 1 );
		}
		// :: dew point
		if( reDew.Matches( strCurrentTable ) )
		{
			pWeather->m_rCurrent.dew_point = wxAtoi( reDew.GetMatch( strCurrentTable, 1 ).Trim(0).Trim(1) );
			reDew.Replace( &strCurrentTable, wxT(""), 1 );
		}
		// :: wind
		if( reWind.Matches( strCurrentTable ) )
		{
			wxStrcpy( pWeather->m_rCurrent.wind_dir_str, reWind.GetMatch( strCurrentTable, 1 ).Trim(0).Trim(1) );
			pWeather->m_rCurrent.wind = wxAtoi( reWind.GetMatch( strCurrentTable, 2 ).Trim(0).Trim(1) );
			reWind.Replace( &strCurrentTable, wxT(""), 1 );
		}
		// :: pressure
		if( rePressure.Matches( strCurrentTable ) )
		{
			pWeather->m_rCurrent.pressure = wxAtoi( rePressure.GetMatch( strCurrentTable, 1 ).Trim(0).Trim(1) );
			// presure options: Rising, ?
			wxString strPressureTrend = rePressure.GetMatch( strCurrentTable, 2 ).Trim(0).Trim(1);
			rePressure.Replace( &strCurrentTable, wxT(""), 1 );
		}
		// :: Conditions
		if( reCond.Matches( strCurrentTable ) )
		{
			wxString strConditions = reCond.GetMatch( strCurrentTable, 1 ).Trim(0).Trim(1);
			wxStrcpy( pWeather->m_rCurrent.cond_text, strConditions );
			// now process conditions for the icon
			strConditions.Replace( wxT(" "), wxT("") );
			strConditions.LowerCase();
			wxStrcpy( pWeather->m_rCurrent.cond_icon, strConditions );
			if( strConditions.CmpNoCase( wxT("Overcast") ) == 0 ) wxStrcpy( pWeather->m_rCurrent.cond_icon, wxT("cloudy") );
			// remove finding
			reCond.Replace( &strCurrentTable, wxT(""), 1 );
		}
		// :: Visibility
		if( reVisibility.Matches( strCurrentTable ) )
		{
			pWeather->m_rCurrent.visibility = wxAtof( reVisibility.GetMatch( strCurrentTable, 1 ).Trim(0).Trim(1) );
			reVisibility.Replace( &strCurrentTable, wxT(""), 1 );
		}
		// :: UV
		if( reUV.Matches( strCurrentTable ) )
		{
			pWeather->m_rCurrent.uv = wxAtoi( reUV.GetMatch( strCurrentTable, 1 ).Trim(0).Trim(1) );
			reUV.Replace( &strCurrentTable, wxT(""), 1 );
		}
		// :: Clouds
		if( reClouds.Matches( strCurrentTable ) )
		{
			// get clouds coverage and process
			wxString strClouds = reClouds.GetMatch( strCurrentTable, 1 ).Trim(0).Trim(1);
			reClouds.Replace( &strCurrentTable, wxT(""), 1 );
		}

	}

	/////////////////////////////////////////
	// check if I got a forecast table
	if( reForecastTable.Matches( strPage ) )
	{
		pWeather->m_nForecastRec = 0;
		// set fetch time - here set the main m_nFetchTime instead
		pWeather->m_nForecastFetchTime = time(NULL);

		// get start forecast day
		int nForecastDay = atoi( reForecastTable.GetMatch( strPage, 3 ).Trim(0).Trim(1).ToAscii() );
		// look for exception :: get today and compare
		wxDateTime now;
		now.SetToCurrent();
		int nToday = now.GetDay();
		// set if different
		if( nToday != nForecastDay )
			pWeather->m_nFirstForecastRec = -1;
		else
			pWeather->m_nFirstForecastRec = 0;

		// get forecast table
		wxString strForecastTable = reForecastTable.GetMatch( strPage, 5 ).Trim(0).Trim(1);

		// get day by day forecast
		while( reDay.Matches( strForecastTable ) )
		{
			wxString strIcon = reDay.GetMatch( strForecastTable, 1 ).Trim(0).Trim(1);
			wxString strDay = reDay.GetMatch( strForecastTable, 2 ).Trim(0).Trim(1);
			wxString strText = reDay.GetMatch( strForecastTable, 3 ).Trim(0).Trim(1);
			wxString strTempType = reDay.GetMatch( strForecastTable, 4 ).Trim(0).Trim(1);
			wxString strTemp = reDay.GetMatch( strForecastTable, 5 ).Trim(0).Trim(1);

			// now check period type - if low=night, high=day
			if( strTempType.CmpNoCase( wxT("High") ) == 0 )
			{
				// set day vars
				pWeather->m_vectForecastRec[nRec].temperature_day = atof( strTemp.ToAscii() );
				wxStrcpy( pWeather->m_vectForecastRec[nRec].cond_icon_day, strIcon );
				wxStrcpy( pWeather->m_vectForecastRec[nRec].cond_text_day, strText );
				// set also the day time
				pWeather->m_vectForecastRec[nRec].epoch = time(NULL) + 86400*(nRec+pWeather->m_nFirstForecastRec);

			// if night
			} else if( strTempType.CmpNoCase( wxT("Low") ) == 0 )
			{
				// set night vars
				pWeather->m_vectForecastRec[nRec].temperature_night = atof( strTemp.ToAscii() );
				wxStrcpy( pWeather->m_vectForecastRec[nRec].cond_icon_night, strIcon );
				wxStrcpy( pWeather->m_vectForecastRec[nRec].cond_text_night, strText );

				// go to next record
				nRec++;
			}

			// remove finding
			reDay.Replace( &strForecastTable, wxT(""), 1 );
		}

	} 
	//else
	//	return( 0 );

	// set number of records
	pWeather->m_nForecastRec = nRec;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CWeatherUnder::Get_Forecast_Daily_LiteHtml( int nDay, DefWeatherForecastRecord* vectForecast, 
											   int firstRec, double lat, double lon )
{
	// check time stamp of last update - to be > 30 minutes = 1800 seconds
	time_t tnow = time(NULL);
	long nDiff = tnow - vectForecast[nDay].fetch_time;
	// very difference
	if( nDiff < 1800 ) return( 1 );

	DefWeatherForecastBaseRecord* vectRec = vectForecast[nDay].vectDetail;
	vectForecast[nDay].nDetail = 0;

	wxString strPage=wxT("");
	wxString strField=wxT("");
	wxString strUrl=wxT("");
	wxString strTmp=wxT("");
	unsigned long nVarULong;
	double nVarDouble;
	int nCount=0;

	// regex
	wxRegEx reMainTable( wxT("\\<table bgcolor\\=\\#bbbbbb cellpadding\\=2 cellspacing\\=1 class\\=smalltable\\>(.*?)\\<\\/table\\>"),  wxRE_ADVANCED );
	wxRegEx reHours( wxT("\\>Jump to\\: .*?\\<\\/tr\\>(.*?)\\<\\/tr\\>"), wxRE_ADVANCED );
	wxRegEx reHour( wxT("\\<font class\\=smalltableheader.*?\\<b\\>([0-9]+)\\&nbsp\\;(AM|PM)\\<\\/b\\>"), wxRE_ADVANCED );
	wxRegEx reTempDew( wxT("\\>Temperature \\/ Dew Point.*?\\<\\/tr\\>(.*?)\\<\\/tr\\>"), wxRE_ADVANCED );
	wxRegEx reTempDewElem( wxT("\\<font class\\=smalltabletext\\> *([0-9]*) *\\/ *([0-9]*)\\<"), wxRE_ADVANCED );
	// :: humidity
	wxRegEx reHum( wxT("\\>Humidity\\<\\/b\\>.*?\\<\\/tr\\>(.*?)\\<\\/tr\\>"), wxRE_ADVANCED );
	wxRegEx reHumElem( wxT("\\<font class\\=smalltabletext\\> *([0-9]*) *\\% *\\<"), wxRE_ADVANCED );
	// :: wind
	wxRegEx reWind( wxT("\\>Wind\\<\\/b\\>.*?\\<\\/tr\\>(.*?)\\<\\/tr\\>"), wxRE_ADVANCED );
	wxRegEx reWindElem( wxT("\\<td .*?\\<font class\\=smalltabletext\\> *([0-9]*) *\\<br.*?<font class\\=smalltabletext\\> *([0-9]*) *\\&deg\\; *\\<br\\>"), wxRE_ADVANCED );
	// :: conditions
	wxRegEx reCond( wxT("\\>Conditions\\<\\/b\\>.*?\\<\\/tr\\>(.*?)\\<\\/tr\\>"), wxRE_ADVANCED );
	wxRegEx reCondIconTxtElem( wxT("\\<td .*?\\<img src\\=.*\\/(.*?)\\.GIF.*?\\<br\\> *(.*?) *\\<\\/td\\>"), wxRE_ADVANCED );
	// :: precipitations
	wxRegEx rePrecip( wxT("\\>Probability of Precipitation\\<\\/b\\>.*?\\<\\/tr\\>(.*?)\\<\\/tr\\>"), wxRE_ADVANCED );
	wxRegEx rePrecipElem( wxT("\\<font class\\=smalltabletext\\> *([0-9]*) *\\% *\\<"), wxRE_ADVANCED );
	// :: clouds
	wxRegEx reClouds( wxT("\\>Cloud Cover\\<\\/b\\>.*?\\<\\/tr\\>(.*?)\\<\\/tr\\>"), wxRE_ADVANCED );
	wxRegEx reCloudsElem( wxT("\\<font class\\=smalltabletext\\> *([0-9]*) *\\% *\\<"), wxRE_ADVANCED );

	// build url
	strTmp.Printf( wxT("%.6f%s%.6f"), lat, wxT("%2C"), lon );
	// use mobile
	//sprintf( strUrl, "http://mobile.wunderground.com/cgi-bin/findweather/getForecast?brand=mobile_metric&query=%s", strTmp );
	// use lite
	wxDateTime now;
	now.SetToCurrent();
	int nYearDay = now.GetDayOfYear() -1 + nDay + firstRec;
	strUrl.Printf( wxT("http://www.wund.com/cgi-bin/findweather/getForecast?hourly=1&query=%s&yday=%d"), strTmp, nYearDay );
	// get page
	if( !GetWebPage( strUrl, strPage ) ) return( 0 );

	// remore characters
	strPage.Replace( wxT("\n"), wxT("") );
	strPage.Replace( wxT("\r"), wxT("") );
	strPage.Replace( wxT("\t"), wxT("") );


	// check if't matching main html page
	if( reMainTable.Matches( strPage ) )
	{
		// set fetch time
		vectForecast[nDay].fetch_time = time(NULL);

		// get main table
		wxString strMainTable = reMainTable.GetMatch( strPage, 1 ).Trim(0).Trim(1);

		//////////////////////////
		// GET HOURS
		if( reHours.Matches( strMainTable ) )
		{
			wxString strHours = reHours.GetMatch( strMainTable, 1 ).Trim(0).Trim(1);
			// get hours with am/pm
			nCount = 0;
			while( reHour.Matches( strHours ) )
			{
				wxString strHourVal = reHour.GetMatch( strHours, 1 ).Trim(0).Trim(1);
				wxString strHourSet = reHour.GetMatch( strHours, 2 ).Trim(0).Trim(1);
				// calculate time
				vectRec[nCount].time = atoi( strHourVal.ToAscii() );
				if( strHourSet.CmpNoCase( wxT("pm") ) == 0 && vectRec[nCount].time != 12 ) vectRec[nCount].time += 12;
				if( strHourSet.CmpNoCase( wxT("am") ) == 0 && vectRec[nCount].time == 12 ) vectRec[nCount].time = 0;
				// remove finding
				reHour.Replace( &strHours, wxT(""), 1 );
				nCount++;
			}
		}

		/////////////////////////////
		// GET TEMPERATURE/DEW
		if( reTempDew.Matches( strMainTable ) )
		{
			wxString strTempDew = reTempDew.GetMatch( strMainTable, 1 ).Trim(0).Trim(1);
			// loop: get all temp/dew
			nCount = 0;
			while( reTempDewElem.Matches( strTempDew ) )
			{
				wxString strTemp = reTempDewElem.GetMatch( strTempDew, 1 ).Trim(0).Trim(1);
				wxString strDew = reTempDewElem.GetMatch( strTempDew, 2 ).Trim(0).Trim(1);
				// calculate 
				vectRec[nCount].temperature = atof( strTemp.ToAscii() );
				vectRec[nCount].dew = atof( strDew.ToAscii() );
				// remove finding
				reTempDewElem.Replace( &strTempDew, wxT(""), 1 );
				nCount++;
			}
		}

		/////////////////////////////
		// GET HUMIDITY
		if( reHum.Matches( strMainTable ) )
		{
			wxString strHum = reHum.GetMatch( strMainTable, 1 ).Trim(0).Trim(1);
			// loop: get all humidity %
			nCount = 0;
			while( reHumElem.Matches( strHum ) )
			{
				wxString strHumElem = reHumElem.GetMatch( strHum, 1 ).Trim(0).Trim(1);
				// calculate 
				vectRec[nCount].humidity = atof( strHumElem.ToAscii() );
				// remove finding
				reHumElem.Replace( &strHum, wxT(""), 1 );
				nCount++;
			}
		}

		/////////////////////////////
		// GET WIND
		if( reWind.Matches( strMainTable ) )
		{
			wxString strWind = reWind.GetMatch( strMainTable, 1 ).Trim(0).Trim(1);
			// loop: get all Wind Speed/Direction
			nCount = 0;
			while( reWindElem.Matches( strWind ) )
			{
				wxString strWindSpeedElem = reWindElem.GetMatch( strWind, 1 ).Trim(0).Trim(1);
				wxString strWindDirElem = reWindElem.GetMatch( strWind, 2 ).Trim(0).Trim(1);
				// calculate 
				vectRec[nCount].wind_speed = atof( strWindSpeedElem.ToAscii() );
				vectRec[nCount].wind_dir = atof( strWindDirElem.ToAscii() );
				// remove finding
				reWindElem.Replace( &strWind, wxT(""), 1 );
				nCount++;
			}
		}

		/////////////////////////////
		// GET CONDITIONS
		if( reCond.Matches( strMainTable ) )
		{
			wxString strCond = reCond.GetMatch( strMainTable, 1 ).Trim(0).Trim(1);
			// loop: get all conditions icons/text
			nCount = 0;
			while( reCondIconTxtElem.Matches( strCond ) )
			{
				wxString strCondIconElem = reCondIconTxtElem.GetMatch( strCond, 1 ).Trim(0).Trim(1);
				wxString strCondTextElem = reCondIconTxtElem.GetMatch( strCond, 2 ).Trim(0).Trim(1);
				// calculate 
				wxStrcpy( vectRec[nCount].cond_icon, strCondIconElem );
				wxStrcpy( vectRec[nCount].cond_text, strCondTextElem );
				// remove finding
				reCondIconTxtElem.Replace( &strCond, wxT(""), 1 );
				nCount++;
			}
			// here I set my number of element as well
			vectForecast[nDay].nDetail = nCount;
		}

		/////////////////////////////
		// GET PRECIPITATION
		if( rePrecip.Matches( strMainTable ) )
		{
			wxString strPrecip = rePrecip.GetMatch( strMainTable, 1 ).Trim(0).Trim(1);
			// loop: get all precipitations %
			nCount = 0;
			while( rePrecipElem.Matches( strPrecip ) )
			{
				wxString strPrecipElem = rePrecipElem.GetMatch( strPrecip, 1 ).Trim(0).Trim(1);
				// calculate 
				vectRec[nCount].precip = atof( strPrecipElem.ToAscii() );
				// remove finding
				rePrecipElem.Replace( &strPrecip, wxT(""), 1 );
				nCount++;
			}
		}

		/////////////////////////////
		// GET CLOUDS(cover)
		if( reClouds.Matches( strMainTable ) )
		{
			wxString strClouds = reClouds.GetMatch( strMainTable, 1 ).Trim(0).Trim(1);
			// loop: get all cloud cover %
			nCount = 0;
			while( reCloudsElem.Matches( strClouds ) )
			{
				wxString strCloudsElem = reCloudsElem.GetMatch( strClouds, 1 ).Trim(0).Trim(1);
				// calculate 
				vectRec[nCount].clouds = atof( strCloudsElem.ToAscii() );
				// remove finding
				reCloudsElem.Replace( &strClouds, wxT(""), 1 );
				nCount++;
			}
		}

		// debug
//		FILE* ff = fopen( "./pg.txt", "w" );
//		fputs( strMainTable, ff );
//		fclose( ff );

	} else
		return( 0 );

	return( 1 );
}