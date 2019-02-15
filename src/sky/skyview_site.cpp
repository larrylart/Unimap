////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// wx
#include "wx/wxprec.h"
#include <wx/file.h>
#include <wx/string.h>
#include <wx/filesys.h>
#include <wx/html/htmlpars.h>
#include <wx/fs_inet.h>
#include <wx/regex.h>
#include <wx/uri.h>
#include <wx/tokenzr.h>

// local headers
#include "../util/folders.h"
#include "../util/func.h"
#include "../util/webfunc.h"
#include "../image/astroimage.h"

// main header
#include "skyview_site.h"

////////////////////////////////////////////////////////////////////
CSkyviewSite::CSkyviewSite( CAstroImage* pAstroImage, StarDef* pObject, wxString& strObjectCatName ) 
{
	m_pAstroImage = pAstroImage;
	m_pObject = pObject;
	m_strObjectCatName = strObjectCatName;

	m_nRemImageColorTable = 0;
	m_nSurveyImageOptical = 0;
	m_nSurveyImageInfrared = 0;
	m_nSurveyImageUV = 0;
	m_nSurveyImageRadio = 0;
	m_nSurveyImageXray = 0;
	m_nSurveyImageHardXray = 0;
	m_nSurveyImageDiffuseXray = 0;
	m_nSurveyImageGammaRay = 0;
	m_nRemImageCatalogOverlay = 0;

	m_vectImageContourOverlay.clear();
}

////////////////////////////////////////////////////////////////////
CSkyviewSite::~CSkyviewSite( ) 
{
	m_vectImageContourOverlay.clear();
}

////////////////////////////////////////////////////////////////////
// Method:	GetSkyviewImageUrl
// Class:	CSkyviewSite
// Purpose:	Get skyview image url from html page
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CSkyviewSite::GetSkyviewImageUrl( wxString& strUrl, wxString& strImageUrl ) 
{
	FILE* pLocalFile = NULL;
	wxString strFile;
	wxChar strLine[2000];
	int i = 0;
//	char strMsg[255];
	wxString strTmpUrl = wxT("");
	// file regex
	wxRegEx reImageUrl = wxT( "<img alt\\=\\\"Quicklook RGB image\\\" src\\=\\\"\\.\\.([^\\\"]+)\\\"><\\/a><\\/td><\\/tr>" ); 

	// set local temp file
	strFile.Printf( wxT("%s%s"), OBJECT_SKYVIEW_DATA_TEMP, wxT("image.html") );
	// check if folder exists
	if( ::wxDirExists( OBJECT_SKYVIEW_DATA_TEMP ) ==  0 ) ::wxMkdir( OBJECT_SKYVIEW_DATA_TEMP );

	// load webfile ... perhaps I should use LoadWebFileT...
	LoadWebFile( strUrl, strFile );

	///////////////////////////
	// here we process the file

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}
	// Reading lines from cfg file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );
//		wxString strWxLine strLine,wxConvUTF8);

		// check now fields
		if( reImageUrl.Matches( strLine ) )
		{
			strTmpUrl = reImageUrl.GetMatch( strLine, 1 );
			break;
		}
	}
	// check if anything
	if( strTmpUrl.CmpNoCase( wxT("") ) != 0 )
	{
		strImageUrl = wxT("http://skyview.gsfc.nasa.gov") + strTmpUrl;
	} else
		return( 0 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	GetRemoteImage
// Class:	CSkyviewSite
// Purpose:	Get remote image
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CSkyviewSite::GetRemoteImage( wxString& strImageSource, wxString& strSurvey, wxString& strRemImageType, int nColorTable, 
						int nGridOverlay, int nCatalogOverlay, int nContourOverlay,
						wxString& strRemImageRGBCompRed, wxString& strRemImageRGBCompGreen,
						wxString& strRemImageRGBCompBlue, wxString& strRemImageGridOverlay,
						wxString& strRemImageContourOverlay, int bHasName ) 
{
	wxString strFile;
	wxString strUrlGetData;
	int i = 0;
//	char strMsg[255];
	int bRgbImage = 0;

	// get current settings
	wxString strSurveyRGB = wxT( "" );
	// check by source type
	if( strImageSource.CmpNoCase( wxT("Color RGB") ) == 0 )
	{
		strSurvey = strRemImageRGBCompRed + wxT(",") +
					strRemImageRGBCompGreen + wxT(",") +
					strRemImageRGBCompBlue;
		strSurveyRGB = wxT("&rgb=1");
		// set flag to 1
		bRgbImage = 1;
	} 

	// grid overlay
	wxString strGridOverlay = wxT("");
	if( nGridOverlay > 0 ) strGridOverlay = wxT("&Grid=") +  URLEncode( strRemImageGridOverlay );
	// catalog overlay
	wxString strCatalogOverlay = wxT("");
	if( nCatalogOverlay > 0 ) strCatalogOverlay = wxT("&Catalog=") + m_vectRemImageCatalogOverlay[nCatalogOverlay-1] + wxT("&CatalogIds=YES");
	// set contour overlay
	wxString strContourOverlay = wxT("");
	if( nContourOverlay > 0 ) 
		strContourOverlay = wxT("&contour=") + URLEncode( strRemImageContourOverlay ); //+
									//":Log:6:1:1000";

	// calculate rotation angle
//	double nRotAngleRad = (double) ( m_pAstroImage->m_pBestTrans->c/m_pAstroImage->m_pBestTrans->b );
	double nRotAngle = 0;
	nRotAngle = ( atan2( m_pAstroImage->m_pBestTrans->c, m_pAstroImage->m_pBestTrans->b ) * 180.0 )/PI;
	// get field size square for now - use object cut width instead
	double nFieldSize = (double) 90.0/m_pAstroImage->m_nSkyProjectionScale;

	// set object name if not matched do use image name to avoid dup zero
	wxString strObjectName = m_strObjectCatName;
	// if no name use deetction id
	if( bHasName == 0 ) strObjectName.sprintf( wxT("%s_%d"), m_pAstroImage->m_strImageName, m_pObject->img_id );
	// setlocal file
	strFile.Printf( wxT("%s%s%s%s%s%d_%d_%d_%d%s"), GetSurveyImagePath(strRemImageType), strObjectName, wxT("__"), 
						CleanupFilename( strSurvey, 1, 0  ) , wxT("__"), 
						nColorTable , nCatalogOverlay, nContourOverlay, nGridOverlay, wxT(".jpg") );
	// clean file name
	wxString strWxFile = strFile;
	// check if file exists
	if( ::wxFileExists( strWxFile ) ==  0 )
	{
//		sprintf( strMsg, "Fetch ADS reference part %d ...", i );
//		m_pUnimap->SetWaitDialogMsg( strMsg );
		///////////////////////////////////
		// skyview.gsfc.nasa.gov
		// nasa best 
		// http://skyview.gsfc.nasa.gov/cgi-bin/runquery.pl
		// http://skyview.gsfc.nasa.gov/cgi-bin/runquery.pl?survey=DSS&coordinates=J2000.0&projection=Tan&scaling=Log&sampler=NN&lut=colortables/b-w-linear.bin&size=0.07083333,0.07083333&pixels=300&position=161.1267680904213,-59.74274684555258
		// http://skyview.gsfc.nasa.gov/cgi-bin/runquery.pl?survey=H-Alpha%20Comp&coordinates=J2000.0&projection=Tan&scaling=Log&sampler=NN&lut=Spectrum&size=6.25000005,6.25000005&pixels=300&position=155.8561489112096,-65.79381990300086
		// http://skyview.gsfc.nasa.gov/cgi-bin/runquery.pl?survey=0035MHz&coordinates=J2000.0&projection=Tan&scaling=Log&sampler=NN&lut=Spectrum&size=15,15&pixels=300&position=119.80283459428571,-69.45025820921098
		// http://skyview.gsfc.nasa.gov/cgi-bin/runquery.pl?survey=DSS2%20Blue&coordinates=J2000.0&projection=Tan&scaling=Log&sampler=NN&lut=colortables/b-w-linear.bin&size=0.041666655000000004,0.041666655000000004&pixels=&position=202.41039474862097,47.16645802355653
		// http://skyview.gsfc.nasa.gov/cgi-bin/images?Survey=digitized+sky+survey&position=14.5,36.5&Return=JPG
		// and here is doc
		// http://skyview.gsfc.nasa.gov/jar/skyviewinajar.html#_Toc181112111


		// http://skyview.gsfc.nasa.gov/cgi-bin/images?Survey=digitized+sky+survey&position=14.5,36.5&Return=JPG
		// rotation=?

		// now check for rgb exception
		if( bRgbImage ) 
		{
			wxString strImageUrl = wxT("");

			// set URL to get skyview html page
			strUrlGetData.sprintf( wxT("http://skyview.gsfc.nasa.gov/cgi-bin/runquery.pl?Survey=%s%s&coordinates=J2000.0&position=%.5f,%.5f&rotation=%.10f&size=%.10f&lut=%s%s%s%s&PlotColor=GREEN"),
									URLEncode( strSurvey ),	strSurveyRGB, 
									m_pObject->ra, m_pObject->dec,
									nRotAngle, nFieldSize, m_vectRemImageColorTable[nColorTable],
									strCatalogOverlay, strContourOverlay, strGridOverlay );


			if( GetSkyviewImageUrl( strUrlGetData, strImageUrl ) == 0 ) return( 0 );
			strUrlGetData = strImageUrl;

		} else
		{
			// set URL to get skyeview image data
			strUrlGetData.sprintf( wxT("http://skyview.gsfc.nasa.gov/cgi-bin/images?Survey=%s%s&coordinates=J2000.0&position=%.5f,%.5f&rotation=%.10f&size=%.10f&lut=%s%s%s%s&PlotColor=GREEN&Return=JPG"),
									URLEncode( strSurvey ),	strSurveyRGB, 
									m_pObject->ra, m_pObject->dec,
									nRotAngle, nFieldSize, m_vectRemImageColorTable[nColorTable],
									strCatalogOverlay, strContourOverlay, strGridOverlay );
		}

		// load webfile ... perhaps I should use LoadWebFileT...
		LoadWebFileT( strUrlGetData, strFile, NULL, 0, 1 );
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		GetSurveyImagePath
// Class:		CSkyviewSite
// Purpose:		get survey image path
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
wxString CSkyviewSite::GetSurveyImagePath( wxString& strSelectType )
{
	wxString strResult = wxT("");
//	wxString strSelectType = m_pRemImageType->GetStringSelection();

	// now check by link type
	if( strSelectType.CmpNoCase( wxT("Optical") ) == 0 )
	{
		strResult = OBJECT_REMOTE_IMAGE_OPTICAL_PATH;
	} else if( strSelectType.CmpNoCase( wxT("Infrared") ) == 0 )
	{
		strResult = OBJECT_REMOTE_IMAGE_IR_PATH;
	} else if( strSelectType.CmpNoCase( wxT("UV") ) == 0 )
	{
		strResult = OBJECT_REMOTE_IMAGE_UV_PATH;
	} else if( strSelectType.CmpNoCase( wxT("Radio") ) == 0 )
	{
		strResult = OBJECT_REMOTE_IMAGE_RADIO_PATH;
	} else if( strSelectType.CmpNoCase( wxT("X-ray") ) == 0 )
	{
		strResult = OBJECT_REMOTE_IMAGE_XRAY_PATH;
	} else if( strSelectType.CmpNoCase( wxT("Hard X-ray") ) == 0 )
	{
		strResult = OBJECT_REMOTE_IMAGE_HARDXRAY_PATH;
	} else if( strSelectType.CmpNoCase( wxT("Diffuse X-ray") ) == 0 )
	{
		strResult = OBJECT_REMOTE_IMAGE_DIFFUSEXRAY_PATH;
	} else if( strSelectType.CmpNoCase( wxT("Gamma Ray") ) == 0 )
	{
		strResult = OBJECT_REMOTE_IMAGE_GAMMARAY_PATH;
	}
	// check if folder exists else create
	if( ! ::wxDirExists( strResult ) )
	{
		::wxMkdir( strResult );
	}

	return( strResult );
}

////////////////////////////////////////////////////////////////////
// Method:		LoadSkyViewColorTable
// Class:		CSkyviewSite
// Purpose:		load skyview color table
// Input:		file name
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CSkyviewSite::LoadSkyViewColorTable( const wxString& strFile )
{
	FILE* pLocalFile = NULL;

	// flags

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[2000];
	wxString strWxLine = wxT("");
	wxRegEx reLine = wxT( "^([^,]+)\\ *,\\ *(.+)$" ); 

	// other variables used
	wxChar strTmp[1000];

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}
	// prepare my list box
//	m_pRemImageColorTable->Clear();
//	m_pRemImageColorTable->Freeze();
//	m_pRemImageColorTable = 0;
//	m_pRemImageColorTable->Append( wxT( "None" ) );

	// Reading lines from cfg file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );
		// copy in wxstring
		wxString strWxLine = strLine;
		strWxLine.Trim( 1 ).Trim( 0 );
		//strWxLine = "  " + strWxLine + "  ";

		// check now fields
		if( reLine.Matches( strWxLine ) )
		{
			wxString strValue = reLine.GetMatch( strWxLine, 1 );
			strValue.Trim( 1 ).Trim( 0 );
			wxString strName = reLine.GetMatch( strWxLine, 2 );
			strName.Trim( 1 ).Trim( 0 );
			
			m_vectRemImageColorTableLabel[m_nRemImageColorTable] = strName;
			m_vectRemImageColorTable[m_nRemImageColorTable] = strValue;
			// populate
			//m_pRemImageColorTable->Append( strName );

			m_nRemImageColorTable++;
		}
	}
	// re-enable list box
//	m_pRemImageColorTable->Thaw();
//	m_pRemImageColorTable->SetSelection( 0 );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadSkyViewCatalogOverlay
// Class:	CSkyviewSite
// Purpose:	load skyview catalog used as overlays
// Input:	file name
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyviewSite::LoadSkyViewCatalogOverlay( const wxString& strFile )
{
	FILE* pLocalFile = NULL;

	// flags

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[2000];
	wxString strWxLine = wxT("");
	wxRegEx reLine = wxT( "^([^,]+)\\ *,\\ *(.+)$" ); 

	// other variables used
	wxChar strTmp[1000];

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}
	m_nRemImageCatalogOverlay = 0;

	// prepare my list box
//	m_pRemImageCatalogOverlay->Clear();
//	m_pRemImageCatalogOverlay->Freeze();
//	m_pRemImageCatalogOverlay->Append( wxT( "None" ) );
	// Reading lines from cfg file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );
		// copy in wxstring
		wxString strWxLine = strLine;
		strWxLine.Trim( 1 ).Trim( 0 );
		//strWxLine = "  " + strWxLine + "  ";

		// check now fields
		if( reLine.Matches( strWxLine ) )
		{
			wxString strValue = reLine.GetMatch( strWxLine, 1 );
			strValue.Trim( 1 ).Trim( 0 );
			wxString strName = reLine.GetMatch( strWxLine, 2 );
			strName.Trim( 1 ).Trim( 0 );
			
			m_vectRemImageCatalogOverlayLabel[m_nRemImageCatalogOverlay] = strName;
			m_vectRemImageCatalogOverlay[m_nRemImageCatalogOverlay] = strValue;
//			m_pRemImageCatalogOverlay->Append( strName );

			m_nRemImageCatalogOverlay++;
		}
	}
	// re-enable list box
//	m_pRemImageCatalogOverlay->Thaw();
//	m_pRemImageCatalogOverlay->SetSelection( 0 );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadSkyViewSurveys
// Class:	CSkyviewSite
// Purpose:	load skyview surveys used for contour, rgb, etc
// Input:	file name
// Output:	status
/////////////////////////////////////////////////////////////////////
int CSkyviewSite::LoadSkyViewSurveys( const wxString& strFile )
{
	FILE* pLocalFile = NULL;

	// flags

	///////////////////////////////////////////////
	/// now here we load the region from file
	wxChar strLine[2000];
	wxString strWxLine = wxT("");
	wxRegEx reLine = wxT( "^([^,]+),(.+)$" ); 

	// other variables used
	wxChar strTmp[1000];

	// open local file to read from
	pLocalFile = wxFopen( strFile, wxT("r") );
	if( !pLocalFile )
	{
			return( 0 );
	}
	// prepare my list box
	// reset surveys
	m_nSurveyImageOptical = 0;
	m_nSurveyImageInfrared = 0;
	m_nSurveyImageUV = 0;
	m_nSurveyImageRadio = 0;
	m_nSurveyImageXray = 0;
	m_nSurveyImageHardXray = 0;
	m_nSurveyImageDiffuseXray = 0;
	m_nSurveyImageGammaRay= 0;
	m_vectImageContourOverlay.clear();

	// reset contour
//	m_pRemImageContourOverlay->Clear();
//	m_pRemImageContourOverlay->Freeze();
//	m_pRemImageContourOverlay->Append( wxT( "None" ) );
	// reset rgb
//	m_pRemImageRGBCompRed->Clear( );
//	m_pRemImageRGBCompRed->Freeze( );
//	m_pRemImageRGBCompRed->Append( wxT( "None" ) );
//	m_pRemImageRGBCompGreen->Clear( );
//	m_pRemImageRGBCompGreen->Freeze( );
//	m_pRemImageRGBCompGreen->Append( wxT( "None" ) );
//	m_pRemImageRGBCompBlue->Clear( );
//	m_pRemImageRGBCompBlue->Freeze( );
//	m_pRemImageRGBCompBlue->Append( wxT( "None" ) );
	// Reading lines from cfg file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );
		// copy in wxstring
//		wxString strWxLine(strLine,wxConvUTF8);

		// check now fields
		if( reLine.Matches( strLine ) )
		{
			wxString strSurveyType = reLine.GetMatch( strLine, 1 );
			strSurveyType.Trim( 1 ).Trim( 0 );
			wxString strName = reLine.GetMatch( strLine, 2 );
			strName.Trim( 1 ).Trim( 0 );

			// append to the contour list
			m_vectImageContourOverlay.push_back( strName );

//			m_pRemImageContourOverlay->Append( strName );
			// build the rgb components
//			m_pRemImageRGBCompRed->Append( strName );
//			m_pRemImageRGBCompGreen->Append( strName );
//			m_pRemImageRGBCompBlue->Append( strName );

			//////////////////////////////
			// populate surveys vectors
			if( strSurveyType.CmpNoCase( wxT("Optical") ) == 0 )
			{
				m_vectSurveyImageOptical[m_nSurveyImageOptical] = strName;
				m_nSurveyImageOptical++;

			} else if( strSurveyType.CmpNoCase( wxT("Infrared") ) == 0 )
			{
				m_vectSurveyImageInfrared[m_nSurveyImageInfrared] = strName;
				m_nSurveyImageInfrared++;

			} else if( strSurveyType.CmpNoCase( wxT("UV") ) == 0 )
			{
				m_vectSurveyImageUV[m_nSurveyImageUV] = strName;
				m_nSurveyImageUV++;

			} else if( strSurveyType.CmpNoCase( wxT("Radio") ) == 0 )
			{
				m_vectSurveyImageRadio[m_nSurveyImageRadio] = strName;
				m_nSurveyImageRadio++;

			} else if( strSurveyType.CmpNoCase( wxT("X-ray") ) == 0 )
			{
				m_vectSurveyImageXray[m_nSurveyImageXray] = strName;
				m_nSurveyImageXray++;

			} else if( strSurveyType.CmpNoCase( wxT("Hard X-ray") ) == 0 )
			{
				m_vectSurveyImageHardXray[m_nSurveyImageHardXray] = strName;
				m_nSurveyImageHardXray++;

			} else if( strSurveyType.CmpNoCase( wxT("Diffuse X-ray") ) == 0 )
			{
				m_vectSurveyImageDiffuseXray[m_nSurveyImageDiffuseXray] = strName;
				m_nSurveyImageDiffuseXray++;

			} else if( strSurveyType.CmpNoCase( wxT("Gamma Ray") ) == 0 )
			{
				m_vectSurveyImageGammaRay[m_nSurveyImageGammaRay] = strName;
				m_nSurveyImageGammaRay++;
			}

		}
	}
	// set survey
//	wxString strSurveyType = wxT( "Optical" );
//	SetImageSurveyType( strSurveyType );
//	m_pRemImageSurvey->SetSelection( 0 );
	// re-enable list box
//	m_pRemImageContourOverlay->Thaw();
//	m_pRemImageContourOverlay->SetSelection( 0 );
	// set rgb to default none
//	m_pRemImageRGBCompRed->Thaw();
//	m_pRemImageRGBCompGreen->Thaw( );
//	m_pRemImageRGBCompBlue->Thaw( );
//	m_pRemImageRGBCompRed->SetSelection( 0 );
//	m_pRemImageRGBCompGreen->SetSelection( 0 );
//	m_pRemImageRGBCompBlue->SetSelection( 0 );

	fclose( pLocalFile );

	return( 1 );
}
