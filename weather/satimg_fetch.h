
#ifndef _SATIMG_FVALK_H
#define _SATIMG_FVALK_H

/////////
// :: http://www.fvalk.com/day_image.htm
int Get_LatestWeatherSatImage_fvalk( int nZone, double lat, double lon, wxString& strImgFile, double& nImgDateTime );
// :: http://www.meteoam.it
int Get_LatestWeatherSatImage_MeteoamIt( int nType, wxString& strImgFile, double& nImgDateTime );
// :: http://www.eurometeo.com
int Get_LatestWeatherSatImage_EuroMeteo( int nType, int nZoneNo, double lat, double lon, wxString& strImgFile, double& nImgDateTime );

#endif
