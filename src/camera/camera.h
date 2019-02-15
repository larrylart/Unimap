////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CAMERA_H
#define _CAMERA_H

// system libs
#include <tchar.h>
#include <stdio.h>
#include <vector>

// wx
#include "wx/wxprec.h"

// local headers
#include "camera_defs.h"
//#include "wia/WiaWrap.h"

// defines :: ini files
#define DEFAULT_CAMERA_BRAND_DEF_FILE	unMakeAppPath(wxT("data/hardware/camera/camera_brands.ini"))
#define DEFAULT_CAMERA_LENS_BRAND_DEF_FILE	unMakeAppPath(wxT("data/hardware/camera_lens/camera_lens_brands.ini"))

////////////////////////////
// structure to keep files from camera
typedef struct
{
	wxString file_name;
	wxString icon_filename;

} DefCameraFrames;

//////////////////////////////
// Hardware device
typedef struct
{
	// my ownid
	int own_id;

	// camera stream type 0=video, 1=still, 2=both
	int stream_type;
	// 0=default, 1=scanner, 2=digital, 3=video
	int wia_type;

	// strin propeties
	wxString strDevId;
	wxString strName;
	wxString strVendor;
	int wia_can_take_picture;
	int connected;
	int active;
	int interface_id;
	int has_factory_driver;
	// props
	int do_imaging;
	int do_guide;
	int do_focus;
	int do_finder;
	int do_spectroscopy;
	int do_monitor;
	// other flags
	int do_update_gui;

} DefHWDevice;

// external classes
class CCameraCanonEos;
class CDCameraWorker;
class CVCameraWorker;
class CCameraVideo;
class CCameraWia;
class CGUIFrame;

// class:	CCamera
///////////////////////////////////////////////////////
class CCamera
{
// public methods
public:
	CCamera( CGUIFrame* pFrame );
	~CCamera( );

	// add hardware devices
	void AddHWDevice( wxString strDevId, wxString strName, wxString strVendor, int nWiaType, int bConnected, int bTakePic );

	// camera action handlers
	int ListImagingDrivers();
	int Init( int nDevId );
	int Connect( int nDevId );
	int Disconnect( int nDevId );
	int SendDisconnect( );
	int TakePhoto( int nDevId, const wxString& strPath=wxT("") );
	int IsConnected( int nDevId );
	void SetGuiUpdate( int nDevId, int bState );

	// frames handlers
	void AddImageFrame( const wxString& file_name );

	// defined camera models
	int LoadHardwareDef( int nHType, int nType=-1, int nBrand=-1 );
	int LoadHardwareIniByBrandId( int nHType, int nBrandId );
	int GetHardwareTypeIdByString( int nHType, const wxChar* strType );
	int HasBrandHardwareType( int nHType, int nBrand, int nType );

	// camera
	int AddCameraBrandDef( const wxString& strName );
	int AddCameraDef( int nBrand, const wxString& strName );
	int IsCameraDef( int nBrand, int nName );
	void UnloadCameraBrandsDef( );
	void UnloadCameraDef( int nBrand );
	// camera lens
	int AddCameraLensBrandDef( const wxString& strName );
	int AddCameraLensDef( int nBrand, const wxString& strName );
	int IsCameraLensDef( int nBrand, int nName );
	void UnloadCameraLensBrandsDef( );
	void UnloadCameraLensDef( int nBrand );

	////////
	// some gui related
	void SetGuiSelect( int nHType, wxChoice* pType, wxChoice* pBrand, wxChoice* pName, 
						int nType, int nBrand, int nName, std::vector<int> &vectBrandId, std::vector<int> &vectNameId ); 
	void SetGuiNameSelect( int nHType, wxChoice* pName, int nType, int nBrand, int nName, std::vector<int> &vectNameId );

// public data
public:
	// list of devices
	std::vector<DefHWDevice> m_vectHWDevices;

	// camera object types
	CCameraCanonEos*	m_pCameraCanonEos;
	CDCameraWorker*		m_pDCameraWorker;
	CVCameraWorker*		m_pVCameraWorker;
	CCameraWia*			m_pCameraWia;
	CCameraVideo*		m_pCameraVideo;
	// gui frame
	CGUIFrame* m_pFrame;

	// files from camera
	std::vector <DefCameraFrames> m_vectCameraFrames;

	// :: camera - just body - or both body/lens if compact
	std::vector<DefCameraBrand>	m_vectCameraBrandDef;
//	int m_nCameraBrandDef;
//	int m_nCameraBrandDefAllocated;

	// :: camera lens
	std::vector<DefCameraLensBrand>	m_vectCameraLensBrandDef;
//	int m_nCameraLensBrandDef;
//	int m_nCameraLensBrandDefAllocated;

};

#endif
