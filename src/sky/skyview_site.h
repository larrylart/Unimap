////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _SKYVIEW_SITE_H
#define _SKYVIEW_SITE_H

//system headers
#include <math.h>
#include <stdio.h>
#include <vector>

// wx
#include "wx/wxprec.h"

// some local headers
#include "../sky/star.h"

// external
class CAstroImage;

// class:	CSkyviewSite
///////////////////////////////////////////////////////
class CSkyviewSite
{
public:
	CSkyviewSite( CAstroImage* pAstroImage, StarDef* pObject, wxString& strObjectCatName );
	~CSkyviewSite();

	// handlers
	wxString GetSurveyImagePath( wxString& strSelectType );
	int GetRemoteImage( wxString& strImageSource, wxString& strSurvey, wxString& strRemImageType, int nColorTable, 
						int nGridOverlay, int nCatalogOverlay, int nContourOverlay,
						wxString& strRemImageRGBCompRed, wxString& strRemImageRGBCompGreen,
						wxString& strRemImageRGBCompBlue, wxString& strRemImageGridOverlay,
						wxString& strRemImageContourOverlay, int bHasName );

	int GetSkyviewImageUrl( wxString& strUrl, wxString& strImageUrl );

	int LoadSkyViewCatalogOverlay( const wxString& strFile );
	int LoadSkyViewSurveys( const wxString& strFile );
	int LoadSkyViewColorTable( const wxString& strFile );

///////////
// DATA
public:

	CAstroImage* m_pAstroImage;
	StarDef* m_pObject;
	wxString m_strObjectCatName;

	// surves labels
	int				m_nSurveyImageOptical;
	wxString		m_vectSurveyImageOptical[17];
	int				m_nSurveyImageInfrared;
	wxString		m_vectSurveyImageInfrared[16];
	int				m_nSurveyImageUV;
	wxString		m_vectSurveyImageUV[8];
	int				m_nSurveyImageRadio;
	wxString		m_vectSurveyImageRadio[11];
	int				m_nSurveyImageXray;
	wxString		m_vectSurveyImageXray[11];
	int				m_nSurveyImageHardXray;
	wxString		m_vectSurveyImageHardXray[14];
	int				m_nSurveyImageDiffuseXray;
	wxString		m_vectSurveyImageDiffuseXray[7];
	int				m_nSurveyImageGammaRay;
	wxString		m_vectSurveyImageGammaRay[4];

	// color table
	wxString	m_vectRemImageColorTableLabel[30];
	wxString	m_vectRemImageColorTable[30];
	int			m_nRemImageColorTable;
//	wxString		m_vectRemImageColorTableValue[30];

	// image catalaog overlay
	int			m_nRemImageCatalogOverlay;
	wxString	m_vectRemImageCatalogOverlayLabel[500];
	wxString	m_vectRemImageCatalogOverlay[500];

	// contour overlays labels/rgb comp red/green/blue
	std::vector<wxString>	m_vectImageContourOverlay;

};

#endif
