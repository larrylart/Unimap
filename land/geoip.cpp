
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
// json
#include "../util/base64.h"
#include "../util/json/jsonval.h"
#include "../util/json/jsonreader.h"

// local headers
#include "../util/webfunc.h"

// main header
#include "geoip.h"

int CGeoIp::GetOwnIp( wxString& country, wxString& city, double& lat, double& lon )
{
	int nState = 0;

	nState = Get_Online_MaxMind_OwnIp( country, city, lat, lon );

	return( nState );
}

int CGeoIp::Get_Online_MaxMind_OwnIp( wxString& country, wxString& city, double& lat, double& lon )
{
	wxString strPage=wxT("");
	GetWebPage( wxT("http://www.maxmind.com//geoip/city_isp_org/me?demo=1"), strPage );

	wxJSONValue root;
	wxJSONReader reader;

	// Error: cannot find a start-object/array character
	int numErrors = reader.Parse( strPage, &root );

	// if all ok
	if( numErrors <= 0 && 
		( !root[_T("country")].IsInt() || root[_T("country")].AsInt() != -1 ) && 
		root[_T("country")][_T("iso_3166_1_alpha_2")].AsString() != wxT("") )
	{
		//country = root[_T("country")][_T("iso_3166_1_alpha_2")].AsString();
		country = root[_T("country")][_T("name")][_T("en")].AsString();
		city = root[_T("city")][_T("name")][_T("en")].AsString();
		lat = root[_T("location")][_T("latitude")].AsDouble();
		lon = root[_T("location")][_T("longitude")].AsDouble();
	}

	return( 1 );
}


int CGeoIp::Get_Online_MaxMind_OwnIp_old( wxString& country, wxString& city, double& lat, double& lon )
{
	wxRegEx reCountry( wxT("Countries\\<\\/td\\>[^\\<]*\\<td[^\\>]*\\>([^\\<]*)\\<\\/td\\>"), wxRE_ADVANCED );
	wxRegEx reCity( wxT("Global Cities\\<\\/td\\>[^\\<]*\\<td[^\\>]*\\>([^\\<]*)\\<\\/td\\>"), wxRE_ADVANCED );
	wxRegEx reLatLon( wxT("Latitude\\/Longitude\\<\\/td\\>[^\\<]*\\<td[^\\>]*\\>([^\\/]*)\\/([^\\<]*)\\<\\/td\\>"), wxRE_ADVANCED );

	wxString strPage=wxT("");
	GetWebPage( wxT("http://www.maxmind.com/app/locate_my_ip"), strPage );

	// parse page
	// :: country
	if( reCountry.Matches( strPage ) )
	{
		wxString strCountry = reCountry.GetMatch( strPage, 1 ).Trim(0).Trim(1);
		country =strCountry;
	}
	// :: city
	if( reCity.Matches( strPage ) )
	{
		wxString strCity = reCity.GetMatch( strPage, 1 ).Trim(0).Trim(1);
		city =strCity;
	}
	// :: latitude/longitude
	if( reLatLon.Matches( strPage ) )
	{
		reLatLon.GetMatch( strPage, 1 ).Trim(0).Trim(1).ToDouble( &lat );
		reLatLon.GetMatch( strPage, 2 ).Trim(0).Trim(1).ToDouble( &lon );
	}

	return( 1 );
}
