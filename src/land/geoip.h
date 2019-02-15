////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _GEOIP_H
#define _GEOIP_H

// wx includes
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif 
//precompiled headers
#include "wx/wxprec.h"
// other wx
#include <wx/string.h>


class CGeoIp
{
public:
	CGeoIp( ){};
	~CGeoIp( ){};

	int GetOwnIp( wxString& country, wxString& city, double& lat, double& lon );

	// online specific methods
	int Get_Online_MaxMind_OwnIp( wxString& country, wxString& city, double& lat, double& lon );
	int Get_Online_MaxMind_OwnIp_old( wxString& country, wxString& city, double& lat, double& lon );
};


#endif

