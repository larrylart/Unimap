////////////////////////////////////////////////////////////////////
// Package:		Detect Config implementation
// File:		detectconfig.cpp
// Purpose:		store detection config/profiles
//
// Created by:	Larry Lart on 22/04/2007
// Updated by:	Larry Lart on 07/03/2010
//
// Copyright:	(c) 2007-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

// system headers
#include <atlbase.h>
#include <tchar.h>
#include <windows.h>
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include <functional>
#include <numeric>

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "wx/defs.h"

// custom
#ifdef _WINDOWS
#include "../util/func.h"
#endif

// wxwindows includes
#include "wx/wxprec.h"
#include <wx/string.h>
#include <wx/regex.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// custom headers
#include "../unimap.h"
#include "../proc/detectfilter.h"
#include "mainconfig.h"

// main header
#include "detectconfig.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CConfigDetect
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CConfigDetect::CConfigDetect( CConfigMain* pMainConfig, wxString strFileName ) 
{
	int nVarInt = 0;

	m_pMainConfig = pMainConfig;
	m_strConfigFile = strFileName;

	m_nDetectProfileId = 0;
	m_nDetectFilter = 0;
	m_vectNNWClass.Empty();
	m_vectProfile.clear();
	m_nProfile = 0;

	// init common stuff
	m_nNumberOfThreads = 1;
	m_nMemObjStack = 50000;
	m_nMemPixStack = 512000;
	m_nMemBufSize = 256240;
	m_nVerboseType = 0;
	m_bWriteXmlFile = 0;
	m_strXmlFileName = wxT("");

	// load profiles from file
	LoadProfiles( );

	// check if not profile create default 
	if( m_vectProfile.size() <= 0 )
	{
		// add the default profile
		AddDetectProfile( wxT("Default") );
		// also save
		SaveProfiles();
	} 

	// get current profile
	m_pMainConfig->GetIntVar( SECTION_ID_DETECTION, CONF_DETECTION_PROFILE, m_nDetectProfileId, 0 );

	// load filters
	LoadFilters( wxT("./cfg/detect_filter.ini") );

	// load classification (neural net) profile names
	LoadNNConfig( );
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CConfigDetect
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CConfigDetect::~CConfigDetect( )
{
	int i = 0;

	// first we save detection profile
	SaveProfiles( );

	// the clear the rest
	for( i=0; i<m_nDetectFilter; i++ ) delete( m_vectDetectFilter[i] );
	m_vectNNWClass.Clear();
	m_vectProfile.clear();

}

/////////////////////////////////////////////////////////////////////
void CConfigDetect::InitProfile( DefConfigDetector& src )
{
	memset( &src, 0, sizeof(DefConfigDetector) );
	// init all vars to defaults
	// detect
	wxStrcpy( src.name, wxT("") );
	src.ImageChannels = 2;
	src.MaskAreas = 1;
	src.ImageType = 0;
	src.DetectMinArea = 3;
	src.DetectThresh = 10.3;
	src.AnalysisThresh = 1.5;
	src.DeblendNThresh = 32;
	src.DeblendNMinCont = 0.005;
	src.DetectClean = 1;
	src.CleanParam = 1;

	// process
	src.isFilter = 1;
	src.FilterName = 16;	
	src.FilterThresh = 25.8;
	src.MaskType = 2;
	src.ThreshType = 1;
	src.WeightType = 0;
	src.ScanIsoApRatio = 0.60;
	src.Blank = 0;

	// photometry
	src.PhotApertures = 4;
	src.PhotAutoparamKron = 2.0;
	src.PhotAutoparamMinRad = 3.0;
	src.PhotPetroparamFact = 2.0;
	src.PhotPetroparamMinRad = 3.0;
	src.SaturationLevel = 5000.0;
	src.MagZeroPoint = 0.0;
	src.MagGamma = 4.0;
	src.Gain = 0.0;
	src.PixelScale = 1.0;

	// classification
	src.SeeingFwhm = 1.2;
	// classification engine type string ?
	wxStrcpy( src.NNClassName, wxT("Default") );

	// Group :: Background
	src.BackSize = 16;
	src.BackFilterSize = 3;
	src.BackPhotoType = 0;
	src.BackFilterThresh = 0.5;

	// Group :: Check Image
	src.CheckImageType = 0;
	wxStrcpy( src.CheckImageFile, wxT("") );
}

/////////////////////////////////////////////////////////////////////
int CConfigDetect::AddDetectProfile( const wxChar* strProfileName )
{
	DefConfigDetector src;
	InitProfile(src);
	wxStrcpy( src.name, strProfileName );
	m_vectProfile.push_back( src );

	return( m_vectProfile.size()-1 );
}

/////////////////////////////////////////////////////////////////////
int CConfigDetect::DeleteProfile( int nId )
{
	m_vectProfile.erase( m_vectProfile.begin()+nId );
	return(1);
}	

/////////////////////////////////////////////////////////////////////
// Purpose:	save my detection profiles in file
/////////////////////////////////////////////////////////////////////
int CConfigDetect::SaveProfiles( )
{
	FILE* pFile = NULL;
	int i=0;

	// open local file to write
	pFile = wxFopen( m_strConfigFile, wxT("w") );
	if( !pFile ) return( 0 );

	//////////////
	// first save header global common vars
	fprintf( pFile, "\n{GLOBALS}\n" );
	fprintf( pFile, "MemObjStack=%lu\n", m_nMemObjStack );
	fprintf( pFile, "MemPixStack=%lu\n", m_nMemPixStack );
	fprintf( pFile, "MemBufSize=%lu\n", m_nMemBufSize );
	fprintf( pFile, "NoOfThreads=%d\n", m_nNumberOfThreads );
	fprintf( pFile, "VerboseType=%d\n", m_nVerboseType );
	fprintf( pFile, "WriteXml=%d\n", m_bWriteXmlFile );
	wxFprintf( pFile, wxT("XmlFile=%s\n"), m_strXmlFileName );

	///////////
	// for every detection profile
	for( i=0; i<m_vectProfile.size();  i++ )
	{
		fprintf( pFile, "\n" );
		wxFprintf( pFile, wxT("\[%s\]\n"), m_vectProfile[i].name );
		// detection
		fprintf( pFile, "ImageChannels=%d\n", m_vectProfile[i].ImageChannels );
		fprintf( pFile, "MaskAreas=%d\n", m_vectProfile[i].MaskAreas );
		fprintf( pFile, "ImageType=%d\n", m_vectProfile[i].ImageType ); 
		fprintf( pFile, "DetectMinArea=%d\n", m_vectProfile[i].DetectMinArea ); 
		fprintf( pFile, "DetectThresh=%.6lf\n", m_vectProfile[i].DetectThresh );  
		fprintf( pFile, "AnalysisThresh=%.6lf\n", m_vectProfile[i].AnalysisThresh ); 
		fprintf( pFile, "DeblendNThresh=%d\n",  m_vectProfile[i].DeblendNThresh ); 
		fprintf( pFile, "DeblendNMinCont=%.6lf\n",  m_vectProfile[i].DeblendNMinCont ); 
		fprintf( pFile, "Clean=%d\n",  m_vectProfile[i].DetectClean );
		fprintf( pFile, "CleanParam=%d\n",  m_vectProfile[i].CleanParam );
		// processing
		fprintf( pFile, "isFilter=%d\n", m_vectProfile[i].isFilter );
		fprintf( pFile, "FilterName=%d\n",  m_vectProfile[i].FilterName );	
		fprintf( pFile, "FilterThresh=%.6lf\n",  m_vectProfile[i].FilterThresh );
		fprintf( pFile, "MaskType=%d\n",  m_vectProfile[i].MaskType );
		fprintf( pFile, "ThreshType=%d\n",  m_vectProfile[i].ThreshType );
		fprintf( pFile, "WeightType=%d\n",  m_vectProfile[i].WeightType );
		fprintf( pFile, "ScanIsoApRatio=%.6lf\n", m_vectProfile[i].ScanIsoApRatio );
		fprintf( pFile, "Blank=%d\n", m_vectProfile[i].Blank );
		// photometry
		fprintf( pFile, "PhotApertures=%d\n",  m_vectProfile[i].PhotApertures );
		fprintf( pFile, "PhotAutoparamKron=%.6lf\n", m_vectProfile[i].PhotAutoparamKron );
		fprintf( pFile, "PhotAutoparamMinRad=%.6lf\n", m_vectProfile[i].PhotAutoparamMinRad );
		fprintf( pFile, "PhotPetroparamFact=%.6lf\n", m_vectProfile[i].PhotPetroparamFact );
		fprintf( pFile, "PhotPetroparamMinRad=%.6lf\n", m_vectProfile[i].PhotPetroparamMinRad );
		fprintf( pFile, "SaturationLevel=%.6lf\n", m_vectProfile[i].SaturationLevel );
		fprintf( pFile, "MagZeroPoint=%.6lf\n", m_vectProfile[i].MagZeroPoint );
		fprintf( pFile, "MagGamma=%.6lf\n", m_vectProfile[i].MagGamma );
		fprintf( pFile, "Gain=%.6lf\n", m_vectProfile[i].Gain );
		fprintf( pFile, "PixelScale=%.6lf\n", m_vectProfile[i].PixelScale );
		// classification
		fprintf( pFile, "SeeingFwhm=%.6lf\n", m_vectProfile[i].SeeingFwhm );
		wxFprintf( pFile, wxT("NNClassName=%s\n"), m_vectProfile[i].NNClassName );
		fprintf( pFile, "BackSize=%d\n",  m_vectProfile[i].BackSize );
		fprintf( pFile, "BackFilterSize=%d\n", m_vectProfile[i].BackFilterSize );
		fprintf( pFile, "BackPhotoType=%d\n", m_vectProfile[i].BackPhotoType );
		fprintf( pFile, "BackFilterThresh=%.6lf\n", m_vectProfile[i].BackFilterThresh );
		fprintf( pFile, "CheckImageType=%d\n", m_vectProfile[i].CheckImageType );
		wxFprintf( pFile, wxT("CheckImageFile=%s\n"), m_vectProfile[i].CheckImageFile );
	}

	// close my file
	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Purpose:	load my detection profiles from file
/////////////////////////////////////////////////////////////////////
int CConfigDetect::LoadProfiles( )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxChar strTmp[2000];
	int nRead=0, nSection=0;
	DefConfigDetector* profile=NULL;

	// open the config file
	pFile = wxFopen( m_strConfigFile, wxT("r") );
	// check if ok
	if( !pFile ) return( 0 );

	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// get one line out of the config file
		memset( &strLine, 0, 2000*sizeof(wxChar) );
		wxFgets( strLine, 2000, pFile );
		nRead = wxStrlen(strLine);
		// check if empty or comment
		if( nRead < 3 ) continue; 
		if( strLine[0] == '#' ) continue;

		////////////////////////////
		// START GLOBALS SECTION
		if( wxStrncmp( strLine, wxT("{GLOBALS}"), 9 ) == 0 )
		{
			nSection = DETECT_CONFIG_SECTION_GLOBALS;

		// START PROFILE SECTION
		} else if( strLine[0] == '[' && strLine[nRead-2] == ']' && 
					wxSscanf( strLine, wxT("\[%[^\]]\]\n"), strTmp ) )
		{
			nSection = DETECT_CONFIG_SECTION_PROFILE;
			// add new profile
			int nIndex = AddDetectProfile( strTmp );
			DefConfigDetector* proTmp = &(m_vectProfile[nIndex]);
			if( proTmp != NULL ) profile = proTmp;

		// PROCESS GLOBALS SECTION
		} else if( nSection == DETECT_CONFIG_SECTION_GLOBALS )
		{
			if( wxStrncmp( strLine, wxT("MemObjStack="), 12 ) == 0 )
				wxSscanf( strLine, wxT("MemObjStack=%lu\n"), &m_nMemObjStack );
			else if( wxStrncmp( strLine, wxT("MemPixStack="), 12 ) == 0 )
				wxSscanf( strLine, wxT("MemPixStack=%lu\n"), &m_nMemPixStack );
			else if( wxStrncmp( strLine, wxT("MemBufSize="), 11 ) == 0 )
				wxSscanf( strLine, wxT("MemBufSize=%lu\n"), &m_nMemBufSize );
			else if( wxStrncmp( strLine, wxT("NoOfThreads="), 12 ) == 0 )
				wxSscanf( strLine, wxT("NoOfThreads=%d\n"), &m_nNumberOfThreads );
			else if( wxStrncmp( strLine, wxT("VerboseType="), 12 ) == 0 )
				wxSscanf( strLine, wxT("VerboseType=%d\n"), &m_nVerboseType );
			else if( wxStrncmp( strLine, wxT("WriteXml="), 9 ) == 0 )
				wxSscanf( strLine, wxT("WriteXml=%d\n"), &m_bWriteXmlFile );
			else if( wxStrncmp( strLine, wxT("XmlFile="), 8 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("XmlFile=%[^\n]\n"), strTmp ) )
					m_strXmlFileName = strTmp;
			}

		// PROCESS PROFILE SECTION
		} else if( nSection == DETECT_CONFIG_SECTION_PROFILE && profile != NULL )
		{
			// detection
			if( wxStrncmp( strLine, wxT("ImageChannels="), 14 ) == 0 )
				wxSscanf( strLine, wxT("ImageChannels=%d\n"), &(profile->ImageChannels) );
			else if( wxStrncmp( strLine, wxT("MaskAreas="), 10 ) == 0 )
				wxSscanf( strLine, wxT("MaskAreas=%d\n"), &(profile->MaskAreas) );
			else if( wxStrncmp( strLine, wxT("ImageType="), 10 ) == 0 )
				wxSscanf( strLine, wxT("ImageType=%d\n"), &(profile->ImageType) );
			else if( wxStrncmp( strLine, wxT("DetectMinArea="), 14 ) == 0 )
				wxSscanf( strLine, wxT("DetectMinArea=%d\n"), &(profile->DetectMinArea) );
			else if( wxStrncmp( strLine, wxT("DetectThresh="), 13 ) == 0 )
				wxSscanf( strLine, wxT("DetectThresh=%lf\n"), &(profile->DetectThresh) );
			else if( wxStrncmp( strLine, wxT("AnalysisThresh="), 15 ) == 0 )
				wxSscanf( strLine, wxT("AnalysisThresh=%lf\n"), &(profile->AnalysisThresh) );
			else if( wxStrncmp( strLine, wxT("DeblendNThresh="), 15 ) == 0 )
				wxSscanf( strLine, wxT("DeblendNThresh=%d\n"), &(profile->DeblendNThresh) );
			else if( wxStrncmp( strLine, wxT("DeblendNMinCont="), 16 ) == 0 )
				wxSscanf( strLine, wxT("DeblendNMinCont=%lf\n"), &(profile->DeblendNMinCont) );
			else if( wxStrncmp( strLine, wxT("Clean="), 6 ) == 0 )
				wxSscanf( strLine, wxT("Clean=%d\n"), &(profile->DetectClean) );
			else if( wxStrncmp( strLine, wxT("CleanParam="), 11 ) == 0 )
				wxSscanf( strLine, wxT("CleanParam=%d\n"), &(profile->CleanParam) );
			// processing
			else if( wxStrncmp( strLine, wxT("isFilter="), 9 ) == 0 )
				wxSscanf( strLine, wxT("isFilter=%d\n"), &(profile->isFilter) );
			else if( wxStrncmp( strLine, wxT("FilterName="), 11 ) == 0 )
				wxSscanf( strLine, wxT("FilterName=%d\n"), &(profile->FilterName) );
			else if( wxStrncmp( strLine, wxT("FilterThresh="), 13 ) == 0 )
				wxSscanf( strLine, wxT("FilterThresh=%lf\n"), &(profile->FilterThresh) );
			else if( wxStrncmp( strLine, wxT("MaskType="), 9 ) == 0 )
				wxSscanf( strLine, wxT("MaskType=%d\n"), &(profile->MaskType) );
			else if( wxStrncmp( strLine, wxT("ThreshType="), 11 ) == 0 )
				wxSscanf( strLine, wxT("ThreshType=%d\n"), &(profile->ThreshType) );
			else if( wxStrncmp( strLine, wxT("WeightType="), 11 ) == 0 )
				wxSscanf( strLine, wxT("WeightType=%d\n"), &(profile->WeightType) );
			else if( wxStrncmp( strLine, wxT("ScanIsoApRatio="), 15 ) == 0 )
				wxSscanf( strLine, wxT("ScanIsoApRatio=%lf\n"), &(profile->ScanIsoApRatio) );
			else if( wxStrncmp( strLine, wxT("Blank="), 6 ) == 0 )
				wxSscanf( strLine, wxT("Blank=%d\n"), &(profile->Blank) );
			// photometry
			else if( wxStrncmp( strLine, wxT("PhotApertures="), 14 ) == 0 )
				wxSscanf( strLine, wxT("PhotApertures=%d\n"), &(profile->PhotApertures) );
			else if( wxStrncmp( strLine, wxT("PhotAutoparamKron="), 18 ) == 0 )
				wxSscanf( strLine, wxT("PhotAutoparamKron=%lf\n"), &(profile->PhotAutoparamKron) );
			else if( wxStrncmp( strLine, wxT("PhotAutoparamMinRad="), 20 ) == 0 )
				wxSscanf( strLine, wxT("PhotAutoparamMinRad=%lf\n"), &(profile->PhotAutoparamMinRad) );
			else if( wxStrncmp( strLine, wxT("PhotPetroparamFact="), 19 ) == 0 )
				wxSscanf( strLine, wxT("PhotPetroparamFact=%lf\n"), &(profile->PhotPetroparamFact) );
			else if( wxStrncmp( strLine, wxT("PhotPetroparamMinRad="), 21 ) == 0 )
				wxSscanf( strLine, wxT("PhotPetroparamMinRad=%lf\n"), &(profile->PhotPetroparamMinRad) );
			else if( wxStrncmp( strLine, wxT("SaturationLevel="), 16 ) == 0 )
				wxSscanf( strLine, wxT("SaturationLevel=%lf\n"), &(profile->SaturationLevel) );
			else if( wxStrncmp( strLine, wxT("MagZeroPoint="), 13 ) == 0 )
				wxSscanf( strLine, wxT("MagZeroPoint=%lf\n"), &(profile->MagZeroPoint) );
			else if( wxStrncmp( strLine, wxT("MagGamma="), 9 ) == 0 )
				wxSscanf( strLine, wxT("MagGamma=%lf\n"), &(profile->MagGamma) );
			else if( wxStrncmp( strLine, wxT("Gain="), 5 ) == 0 )
				wxSscanf( strLine, wxT("Gain=%lf\n"), &(profile->Gain) );
			else if( wxStrncmp( strLine, wxT("PixelScale="), 11 ) == 0 )
				wxSscanf( strLine, wxT("PixelScale=%lf\n"), &(profile->PixelScale) );
			// classification
			else if( wxStrncmp( strLine, wxT("SeeingFwhm="), 11 ) == 0 )
				wxSscanf( strLine, wxT("SeeingFwhm=%lf\n"), &(profile->SeeingFwhm) );
			else if( wxStrncmp( strLine, wxT("NNClassName="), 12 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("NNClassName=%[^\n]\n"), strTmp ) )
					wxStrcpy( profile->NNClassName, strTmp );
			} else if( wxStrncmp( strLine, wxT("BackSize="), 9 ) == 0 )
				wxSscanf( strLine, wxT("BackSize=%d\n"), &(profile->BackSize) );
			else if( wxStrncmp( strLine, wxT("BackFilterSize="), 15 ) == 0 )
				wxSscanf( strLine, wxT("BackFilterSize=%d\n"), &(profile->BackFilterSize) );
			else if( wxStrncmp( strLine, wxT("BackPhotoType="), 14 ) == 0 )
				wxSscanf( strLine, wxT("BackPhotoType=%d\n"), &(profile->BackPhotoType) );
			else if( wxStrncmp( strLine, wxT("BackFilterThresh="), 17 ) == 0 )
				wxSscanf( strLine, wxT("BackFilterThresh=%lf\n"), &(profile->BackFilterThresh) );
			else if( wxStrncmp( strLine, wxT("CheckImageType="), 15 ) == 0 )
				wxSscanf( strLine, wxT("CheckImageType=%d\n"), &(profile->CheckImageType) );
			else if( wxStrncmp( strLine, wxT("CheckImageFile="), 15 ) == 0 )
			{
				if( wxSscanf( strLine, wxT("CheckImageFile=%[^\n]\n"), strTmp ) )
					wxStrcpy( profile->CheckImageFile, strTmp );
			}
		}
	}

	// close file
	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadFilters
// Class:	CConfigDetect
// Purpose:	load filters from file
// Input:	string file name with filters
// Output:	status
/////////////////////////////////////////////////////////////////////
int CConfigDetect::LoadFilters( const wxString& strFileName )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
//	char strVarName[256];
//	char strVarValue[256];
	double nVarFloatValue = 0;
	long nVarIntValue = 0;
	int nCurrentIndex = 0;
	// define my regex 
	wxRegEx reSection = wxT( "^\\[([a-zA-Z0-9_\\ \\,\\.\\-]+)\\]" );
	wxRegEx reVariable = wxT( "([a-zA-Z0-9_\\-]+)\\ *=\\ *([^\n^\r^\t]+)\\ *" );
	wxRegEx reData = wxT( "Data\\[([0-9]+)\\]\\ *=\\ *([0-9\\.\\,\\+\\ \\-]+)\\ *" );
	wxRegEx reColNo = wxT( "([0-9\\+\\.\\-]+)" );

	// open file with filters
	pFile = wxFopen( strFileName, wxT("r") );

	// check if there is any configuration to load
	if( !pFile ) return( 0 );

	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear thy buffers
		nVarIntValue = 0;
		nVarFloatValue = 0;
//		bzero( strLine, 2000 );
//		bzero( strVarName, 256 );
//		bzero( strVarValue, 256 );

		// get one line out of the config file
		wxFgets( strLine, 2000, pFile );

//		wxString strWxLine(strLine,wxConvUTF8);

		// check if filter start
		if( reSection.Matches( strLine ) )
		{
			wxString strSection = reSection.GetMatch(strLine, 1 );
			// add new filter object
			m_vectDetectFilter[m_nDetectFilter] = new CDetectFilter( strSection );
			m_vectDetectFilter[m_nDetectFilter]->m_nDetectFilterId = m_nDetectFilter;
			// preserve current index
			nCurrentIndex=m_nDetectFilter;
			// increment counter
			m_nDetectFilter++;

		} else if( m_nDetectFilter > 0 && reVariable.Matches( strLine ) )
		{
			wxString strVarName = reVariable.GetMatch(strLine, 1 );
			wxString strVarValue = reVariable.GetMatch(strLine, 2 );
			
			// check my definition vars
			if( strVarName.CmpNoCase( wxT("Type") ) == 0 )
			{
				m_vectDetectFilter[nCurrentIndex]->m_strFilterType = strVarValue;

			} else if( strVarName.CmpNoCase( wxT("Mask") ) == 0 )
			{
				m_vectDetectFilter[nCurrentIndex]->m_strFilterMask = strVarValue;

			} else if( strVarName.CmpNoCase( wxT("Flag") ) == 0 )
			{
				m_vectDetectFilter[nCurrentIndex]->m_strFilterFlag = strVarValue;

			} else if( strVarName.CmpNoCase( wxT("Desc") ) == 0 )
			{
				m_vectDetectFilter[nCurrentIndex]->m_strFilterDesc = strVarValue;

			} else if( strVarName.CmpNoCase( wxT("Width") ) == 0 )
			{
				m_vectDetectFilter[nCurrentIndex]->m_nFilterWidth = wxAtoi( strVarValue );

			} else if( strVarName.CmpNoCase( wxT("Height") ) == 0 )
			{
				m_vectDetectFilter[nCurrentIndex]->m_nFilterHeight = wxAtoi( strVarValue );

			} else if( strVarName.CmpNoCase( wxT("PixelSize") ) == 0 )
			{
				strVarValue.ToDouble( &( m_vectDetectFilter[nCurrentIndex]->m_nFilterPixelSize ) );
			} 

		// if we are in the data segment
		} else if( m_nDetectFilter > 0 && reData.Matches( strLine ) )
		{
			int nDataRowId = 0;
			int nCol = 0;
			double vectDataRow[MAX_FILTER_DATA_COLS];
			// get row id
			nDataRowId = wxAtoi( reData.GetMatch(strLine, 1 ) );
			// get row content
			wxString strDataRowContent = reData.GetMatch(strLine, 2 );

			// for every match of number in the row get vars
			while( reColNo.Matches( strDataRowContent ) )
			{
				wxString strColNo = reColNo.GetMatch( strDataRowContent, 1 );
				if( !strColNo.ToDouble( &( vectDataRow[nCol] ) ) ) vectDataRow[nCol] = 0;
				// remove number from string
				strDataRowContent.Replace( strColNo, wxT(""), false );
				// increment counter
				nCol++;
			}
			// add data row
			m_vectDetectFilter[nCurrentIndex]->AddDataRow( nDataRowId, vectDataRow, nCol );
		}
	}

	// close file
	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
int CConfigDetect::LoadNNConfig( wxString strProfile, brainstruct* brain )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxChar strTmp[2000];
	wxChar* save_ptr;
	int nLineSize=0, i=0, nLayerId=0, nFoundProfile=0;
	int nProfile=-1;

	// open file with filters
	pFile = wxFopen( wxT("./cfg/detect_class.ini"), wxT("r") );

	// check if there is any configuration to load
	if( !pFile ) return( 0 );

	// Reading lines from cfg file
	while( !feof(pFile) )
	{
		// get one line out of the config file
		wxFgets( strLine, 2000, pFile );
		nLineSize = wxStrlen( strLine );

		// check if filter start
		if( strLine[0] == '[' && strLine[nLineSize-2] == ']' &&
			wxSscanf( strLine, wxT("\[%[^\]]\]\n"), strTmp ) )
		{
			// if to load a specific profile
			if( strProfile.CmpNoCase(strTmp) == 0 )
			{
				// initialize brain
				brain->layersnb = 3;
				nProfile = 0;

			} else
			{
				nProfile = -1;
				m_vectNNWClass.Add( strTmp );
				nFoundProfile++;
			}

		} else if( nProfile >=0 )
		{
			//////////////
			// get number of neurons on input/core layers
			if( wxStrncmp( strLine, wxT("LayerNeurons="), 13 ) == 0 )
				wxSscanf( strLine, wxT("LayerNeurons=%d\n"), &(brain->nn[0]) );	
			// get number of core layers
			else if( wxStrncmp( strLine, wxT("CoreLayers="), 11 ) == 0 )
				wxSscanf( strLine, wxT("CoreLayers=%d\n"), &(brain->nn[1]) );
			// get number of neurons on output layer
			else if( wxStrncmp( strLine, wxT("OutputNeurons="), 13 ) == 0 )
				wxSscanf( strLine, wxT("OutputNeurons=%d\n"), &(brain->nn[2]) );

			/////////////
			// get the input bias
			else if( wxStrncmp( strLine, wxT("InputBias="), 10 ) == 0 )
			{
				wxSscanf( strLine, wxT("InputBias=%[^\n]\n"), strTmp );
				save_ptr = strTmp;
				// set input bias layer
				for( i=0; i<brain->nn[0]; i++ )
					brain->inbias[i] = wxAtof( wxStrtok( save_ptr, wxT(", \t\n"), &save_ptr ) );
	
			/////////////
			// get the input scale
			} else if( wxStrncmp( strLine, wxT("InputScale="), 11 ) == 0 )
			{
				wxSscanf( strLine, wxT("InputScale=%[^\n]\n"), strTmp );
				save_ptr = strTmp;
				// for every neuron on input layer
				for( i=0; i<brain->nn[0]; i++ )
					brain->inscale[i] = wxAtof( wxStrtok( save_ptr, wxT(", \t\n"), &save_ptr ) );

			///////////////
			// get the values of core
			} else if( wxStrncmp( strLine, wxT("Core="), 5 ) == 0 )
			{
				wxSscanf( strLine, wxT("Core=%[^\n]\n"), strTmp );
				save_ptr = strTmp;
				// for every neuron in nain layer
				for( i=0; i<brain->nn[0]; i++ )
					brain->w[0][i][nLayerId] = wxAtof( wxStrtok( save_ptr, wxT(", \t\n"), &save_ptr ) );
				// last value is bias?
				brain->b[0][nLayerId] = wxAtof( wxStrtok( save_ptr, wxT(", \t\n"), &save_ptr ) );
				// next layer
				nLayerId++;

			////////////
			// get the values of core back propagation
			} else if( wxStrncmp( strLine, wxT("CoreBack="), 9 ) == 0 )
			{
				wxSscanf( strLine, wxT("CoreBack=%[^\n]\n"), strTmp );
				save_ptr = strTmp;
				// for every neuron in main core feedback layer
				nLayerId = 0;
				for( i=0; i<brain->nn[1]; i++ )
					brain->w[1][i][nLayerId] = wxAtof( wxStrtok( save_ptr, wxT(", \t\n"), &save_ptr ) );
				// last value is bias?
				brain->b[1][nLayerId] = wxAtof( wxStrtok( save_ptr, wxT(", \t\n"), &save_ptr ) );

			/////////////
			// get the output bias
			} else if( wxStrncmp( strLine, wxT("OutputBias="), 11 ) == 0 )
			{
				wxSscanf( strLine, wxT("OutputBias=%[^\n]\n"), strTmp );
				save_ptr = strTmp;
				for( i=0; i<brain->nn[brain->layersnb-1]; i++ )
					brain->outbias[i] = wxAtof( wxStrtok( save_ptr, wxT(", \t\n"), &save_ptr ) );

			/////////////
			// get the output scale
			} else if( wxStrncmp( strLine, wxT("OutputScale="), 12 ) == 0 )
			{
				wxSscanf( strLine, wxT("OutputScale=%[^\n]\n"), strTmp );
				save_ptr = strTmp;
				for( i=0; i<brain->nn[brain->layersnb-1]; i++ )
					brain->outscale[i] = wxAtof( wxStrtok( save_ptr, wxT(", \t\n"), &save_ptr ) );
			}
		}
	}
	// close file
	fclose( pFile );

	return( 1 );
}
