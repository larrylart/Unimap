////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _IMAGE_FORMAT_FITS_H
#define _IMAGE_FORMAT_FITS_H

// system headers
#include <time.h>
#include <vector>
#include <cmath>

// eLynx.Core
#include <elx/core/CoreCore.h>
#include <elx/core/CoreFile.h>
#include <elx/core/IPlugin.h>
#include <elx/core/IPluginPackage.h>

// eLynx.Image
#include <elx/image/ImageVariant.h>
//#include <elx/image/IImageFilePlugin.h>
#include <elx/image/ImageFileInfo.h>

// boost
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost;
using namespace eLynx;
using namespace Image;

// cfitsio
#include "cfitsio/fitsio.h"

#define fitsOpenFile          ffopen
#define fitsCloseFile         ffclos
#define fitsGetImageParam     ffgipr
#define fitsReadPixels        ffgpxv
#define fitsReadKey           ffgky

// local header
typedef struct
{
	wxString name;
	wxString value;
	wxString comment;
	int type;

} DefRecFitsKey;

// external classes
class CAstroImage;

// class: CImgFmtFits
class CImgFmtFits 
{

// public methods
public:
	// constructor/destructor
	CImgFmtFits( );
	~CImgFmtFits( );

	// read fits header
	bool ReadHeader( const wxString& strFileName );
	// write fits header
	bool WriteHeader( fitsfile *fptr );

	// add a key to local header
	void AddHeaderKey( char* name, char* value, char* comment );

	void UpdateHeaderKey( wxString strName, wxString strValue, wxString strComment, int type );
	int GetHeaderKey( wxString strName );

	// hint functions
	int GetHint( double& ra, double& dec, double& focal );

	//////////////////////
	// methods from pugin
	EImageContent GetContent( const char* iprString );
	bool IsSupported( const char* iprFilename, ImageFileInfo& oInfo, bool ibThumbnail );
	shared_ptr<AbstractImage> LoadFile( const char*, ImageFileInfo&, ProgressNotifier& );
	bool Load( ImageVariant& oImage, const char* iprFilename, ImageFileInfo& oInfo, ProgressNotifier& iNotifier );
	// save
	bool Save( const ImageVariant& iImage, const char* iprFilename, ProgressNotifier& iNotifier, const ImageFileOptions* iprOptions=NULL );
	bool SaveL( const ImageVariant& iImage, const char* iprFilename );
	bool SaveLA( const ImageVariant& iImage, const char* iprFilename );
	bool SaveRGB( const ImageVariant& iImage, const char* iprFilename );
	bool SaveRGBA( const ImageVariant& iImage, const char* iprFilename );

	void SetHeader( CAstroImage* pAstroImage );

//////////
// params
public:

	int m_bHasHeader;

	std::vector<DefRecFitsKey> m_vectHeader;

	wxString m_strObjectName;
	wxString m_strObservationType;

	double m_nFocalLength;
	double m_nApertureDiameter;
	double m_nApertureArea;
	double m_nShutter;
	wxString m_strFilter;
	// telescope
	wxString m_strTelescope;
	wxString m_strTelConf;
	wxString m_strTelTcs;
	double m_nTelRa;
	double m_nTelDec;
	// camera/detectors
	wxString m_strInstrument;
	wxString m_strDetector;
	wxString m_strCamera;
	double m_nIsoSpeed;
	double m_nExpTime;
	// date of observarion
	time_t m_nObsDatetime;
	// tempereture
	double m_nTempSet;
	double m_nTempCcd;
	// sky position
	double m_nTargetRa;
	double m_nTargetDec;
	double m_nObjectRa;
	double m_nObjectDec;
	double m_nObjectAlt;
	double m_nObjectAz;
	double m_nObjectHa;
	// site
	double m_nSiteLat;
	double m_nSiteLon;
	// other
	wxString m_strObserverName;
	wxString m_strObservatoryName;
	wxString m_strNotes;
	wxString m_strSoftwareCre;
	wxString m_strSoftwareMod;

	// flags
	int m_bHasTelRaDec;
	int m_bHasTargetRaDec;
	int m_bHasObjectRaDec;
	int m_bHasFocal;

};

#endif
