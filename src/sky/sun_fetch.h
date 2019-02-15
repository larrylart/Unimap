////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _SUN_ONLINE_FETCH_H
#define _SUN_ONLINE_FETCH_H

// :: Get latest sun images from soho nasa or mirror eas
int GetLatest_SunSohoImage( int sourceId, int nBand, wxString& strImgFile, double& nImgDateTime );
// :: Get latest sun images from soho nasa or mirror eas
int GetLatest_SunXrtImage( int nType, wxString& strImgFile, double& nImgDateTime );
// :: get last solar event from lockhead martin site ...
int GetLatest_SunEventImageLmsal( int nType, wxString& strImgFile, double& nImgDateTime );

#endif
