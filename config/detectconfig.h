////////////////////////////////////////////////////////////////////
// Package:		detect config definition
// File:		detectconfig.h
// Purpose:		manage detection configuration
//
// Created by:	Larry Lart on 24/06/2007
// Updated by:	Larry Lart on 07/03/2010
//
// Copyright:	(c) 2007-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _DETECT_CONFIG_H
#define _DETECT_CONFIG_H

// system headers
#include <time.h>
#include <vector>
#include <cmath>

// wx
#include <wx/arrstr.h>

// max number of detection filters
#define MAX_NO_OF_DETECTION_FILTERS		100
// define config sections
#define DETECT_CONFIG_SECTION_GLOBALS	1
#define DETECT_CONFIG_SECTION_PROFILE	2

// profile structure
typedef struct 
{
	wxChar name[255];

	// detect
	int ImageChannels;
	int MaskAreas;
	int ImageType;
	int DetectMinArea;
	double DetectThresh;
	double AnalysisThresh;
	int DeblendNThresh;
	double DeblendNMinCont;
	int DetectClean;
	int CleanParam;

	// process
	int isFilter;
	int FilterName;	
	double FilterThresh;
	int MaskType;
	int ThreshType;
	int WeightType;
	double ScanIsoApRatio;
	int Blank;

	// photometry
	int PhotApertures;
	double PhotAutoparamKron;
	double PhotAutoparamMinRad;
	double PhotPetroparamFact;
	double PhotPetroparamMinRad;
	double SaturationLevel;
	double MagZeroPoint;
	double MagGamma;
	double Gain;
	double PixelScale;

	// classification
	double SeeingFwhm;
	// classification engine type string ?
	wxChar NNClassName[255];

	// Group :: Background
	int BackSize;
	int BackFilterSize;
	int BackPhotoType;
	double BackFilterThresh;

	// Group :: Check Image
	int CheckImageType;
	wxChar CheckImageFile[500];

} DefConfigDetector;

// local headers
#include "../proc/sextractor_neurro.h"

// external classes
class CConfigMain;
class CDetectFilter;

// CLASS :: CCONFIGDETECT
////////////////////////////////////
class CConfigDetect
{
// methods
public:
	CConfigDetect( CConfigMain* pMainConfig, wxString strFileName );
	~CConfigDetect( );

	void InitProfile( DefConfigDetector& src );

	int AddDetectProfile( const wxChar* strProfileName );
	int DeleteProfile( int nId );

	int LoadProfiles( );
	int SaveProfiles( );

	int LoadFilters( const wxString& strFileName );
	int LoadNNConfig( wxString strProfile=wxT(""), brainstruct* brain=NULL );

// data
public:
	CConfigMain*	m_pMainConfig;

	wxString m_strConfigFile;

	int m_nDetectProfileId;

	// vector to store the detection profiles
	std::vector<DefConfigDetector> m_vectProfile;
	int m_nProfile;

	// vars to handle detection filters
	CDetectFilter* m_vectDetectFilter[MAX_NO_OF_DETECTION_FILTERS];
	int m_nDetectFilter;

	// other vars
	///////////////////////////////////
	int m_nNumberOfThreads;
	//  memory control
	unsigned long m_nMemObjStack;
	unsigned long m_nMemPixStack;
	unsigned long m_nMemBufSize;

	// miscellaneous
	int m_nVerboseType;
	int m_bWriteXmlFile;
	wxString m_strXmlFileName;


	// neuro classification filter names
	wxArrayString m_vectNNWClass;
};

#endif
