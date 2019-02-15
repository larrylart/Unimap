////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// windows
#include <windows.h>
#include <Setupapi.h>

// WX includes
#include "wx/wxprec.h"
#include <wx/regex.h>
#include <wx/tokenzr.h>

// include locals
//#include "gdiplus_init.h"
#include "../observer/observer.h"
#include "camera_canoneos.h"
#include "camera_wia.h"
#include "camera_video.h"
#include "worker_dcam.h"
#include "worker_vcam.h"
#include "../gui/frame.h"
#include "../util/folders.h"
#include "../util/windows_func.h"

// main header
#include "camera.h"

////////////////////////////////////////////////////////////////////
CCamera::CCamera( CGUIFrame* pFrame )
{
	m_pFrame = pFrame;
	m_pCameraCanonEos = NULL;
	m_pCameraVideo = NULL;
	m_pCameraWia = NULL;

	m_vectCameraBrandDef.clear();
//	m_nCameraBrandDef=0;
//	m_nCameraBrandDefAllocated=0;
	m_vectCameraLensBrandDef.clear();
//	m_nCameraLensBrandDef=0;
//	m_nCameraLensBrandDefAllocated=0;
	m_vectHWDevices.clear();

	// this uses setup api ... maybe use this somewhere else latter
	// ListImagingDrivers();

	// test wia
	//CCameraWia::WiaGetDevices( NULL, this );
	if( !IsWin7OrLater() ) 
	{
		CCameraWia::WiaGetDevices( NULL, this );

	} else
	{
		CCameraWia::WiaGetDevices2( NULL, this );
		CCameraWia::DirectShowGetDevices( this );
	}
}

////////////////////////////////////////////////////////////////////
CCamera::~CCamera( ) 
{
	m_vectHWDevices.clear();
	if( m_pCameraCanonEos ) delete( m_pCameraCanonEos );
	if( m_pCameraWia ) delete( m_pCameraWia );
	if( m_pCameraVideo ) delete( m_pCameraVideo );

	UnloadCameraBrandsDef( );
	UnloadCameraLensBrandsDef( );
}

////////////////////////////////////////////////////////////////////
void CCamera::AddHWDevice( wxString strDevId, wxString strName, wxString strVendor, int nWiaType, int bConnected, int bTakePic ) 
{
	// vendors/models regex match
	wxRegEx reCanon( wxT("\\ *canon\\ *"), wxRE_EXTENDED|wxRE_ICASE );
	wxRegEx reCanonEOS( wxT("\\ *EOS\\ *"), wxRE_EXTENDED|wxRE_ICASE );

	DefHWDevice elem;

	elem.own_id = m_vectHWDevices.size();
	elem.strDevId = strDevId;
	elem.strName = strName;
	elem.strVendor = strVendor;
	elem.wia_type = nWiaType;
	elem.connected = bConnected;
	elem.wia_can_take_picture = bTakePic;
	elem.active = 0;
	elem.do_imaging = 0;
	elem.do_guide = 0;
	elem.do_focus = 0;
	elem.do_finder = 0;
	elem.do_spectroscopy = 0;
	elem.do_monitor = 0;

	// set interface - ceck varios vendors/models
	if( reCanon.Matches( strVendor ) && reCanonEOS.Matches( strName ) )
	{
		elem.interface_id = 1;
		elem.has_factory_driver = 1;

	} else
	{
		elem.interface_id = 0;
		elem.has_factory_driver = 0;
	}

	m_vectHWDevices.push_back( elem );
}

////////////////////////////////////////////////////////////////////
int CCamera::ListImagingDrivers( )
{
	wxString name_class=wxT("image"); //name of the class whose GUID we need
	GUID class_GuidList;
	DWORD required_size;

	int nn = SetupDiClassGuidsFromName( name_class, &class_GuidList, 1, &required_size );

	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i;

       // Create a HDEVINFO with all present devices.
       hDevInfo = SetupDiGetClassDevs( &class_GuidList, 0, 0, DIGCF_PROFILE );
       
       if( hDevInfo == INVALID_HANDLE_VALUE ) return( 1 );
       
       // Enumerate through all devices in Set.
       DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
       for( i=0; SetupDiEnumDeviceInfo( hDevInfo, i, &DeviceInfoData ); i++ )
       {
           DWORD DataT;
           LPTSTR buffer = NULL;
           DWORD buffersize = 0;
           
           //
           // Call function with null to begin with, 
           // then use the returned buffer size (doubled)
           // to Alloc the buffer. Keep calling until
           // success or an unknown failure.
           //
           //  Double the returned buffersize to correct
           //  for underlying legacy CM functions that 
           //  return an incorrect buffersize value on 
           //  DBCS/MBCS systems.
           // 
		   // use: SPDRP_FRIENDLYNAME
/*           while( !SetupDiGetDeviceRegistryProperty( hDevInfo, &DeviceInfoData, 
					SPDRP_FRIENDLYNAME, &DataT, (PBYTE)buffer, buffersize, &buffersize) )
           {
               if( GetLastError() == ERROR_INSUFFICIENT_BUFFER )
               {
                   // Change the buffer size.
                   if (buffer) LocalFree(buffer);
                   // Double the size to avoid problems on 
                   // W2k MBCS systems per KB 888609. 
                   buffer = (LPTSTR) LocalAlloc(LPTR,buffersize * 2);

               } else
               {
                   // Insert error handling here.
                   break;
               }
           }

		DWORD class_datatype = 0, dwData = 0;
*/
	   DWORD bf=0;

		while( !SetupDiGetClassRegistryProperty( &DeviceInfoData.ClassGuid, SPCRP_DEVTYPE, &DataT,
					(PBYTE)buffer, buffersize, &buffersize, NULL, NULL ) )
		{
				DWORD err = GetLastError();
               if( err == ERROR_INSUFFICIENT_BUFFER )
               {
                   // Change the buffer size.
                   if (buffer) LocalFree(buffer);
                   // Double the size to avoid problems on 
                   // W2k MBCS systems per KB 888609. 
                   buffer = (LPTSTR) LocalAlloc(LPTR,buffersize * 2);

               } else
               {
                   // Insert error handling here.
                   break;
               }

		}


           printf("Result:[%s]\n",buffer);
           
           if (buffer) LocalFree(buffer);
       }
       

       
       if ( GetLastError()!=NO_ERROR && GetLastError()!=ERROR_NO_MORE_ITEMS )
       {
           // Insert error handling here.
           return 1;
       }
       
       //  Cleanup
       SetupDiDestroyDeviceInfoList(hDevInfo);
       
       return 0;

/*	HDEVINFO dev_info = SetupDiGetClassDevs( &class_GuidList, NULL, NULL, DIGCF_PRESENT );

	SP_DEVINFO_DATA dinfo;

	int ret = SetupDiEnumDeviceInfo( dev_info, (DWORD) i, &DeviceInfoData );
	SetupDiGetDeviceRegistryProperty( dev_info, &DeviceInfoData, SPDRP_DEVICEDESC,
										&prop_datatype, prop_buffer, 100, &req_bufsize );


	SetupDiDestroyDeviceInfoList( dev_info );
*/

/*	HDEVINFO h = SetupDiGetClassDevs( &class_GuidList, NULL, NULL, DIGCF_PRESENT );
	if( h != INVALID_HANDLE_VALUE )
	{
	   bool Success = true;
	   int i = 0;
	   while( Success )
	   {
		   CM_Get_Device_ID(DevInstParent, szDeviceIdString, MAX_PATH, 0);
		 // create a Device Interface Data structure
		 SP_DEVICE_INTERFACE_DATA dia;
		 dia.cbSize = sizeof(dia);

		 // start the enumeration 
		 Success = SetupDiEnumDeviceInterfaces( h, NULL, &class_GuidList, i, &dia );
		 if( Success )
		 {
		   // build a DevInfo Data structure
		   SP_DEVINFO_DATA da;
		   da.cbSize = sizeof(da);

		   // build a Device Interface Detail Data structure
		   SP_DEVICE_INTERFACE_DETAIL_DATA didd;
		   didd.cbSize = sizeof(didd); //4 + SystemDefaultCharSize; // trust me :)

		   // now we can get some more detailed information
		   int nRequiredSize = 0;
		   int nBytes = 255; //BUFFER_SIZE;
		   if( SetupDiGetDeviceInterfaceDetail( h, &dia, &didd, nBytes, nRequiredSize, &da ) )
		   {
			 // current InstanceID is at the "USBSTOR" level, so we
			 // need up "move up" one level to get to the "USB" level
			 IntPtr ptrPrevious;
			 CM_Get_Parent( out ptrPrevious, da.DevInst, 0 );

			 // Now we get the InstanceID of the USB level device
			 IntPtr ptrInstanceBuf = Marshal.AllocHGlobal(nBytes);
			 CM_Get_Device_ID(ptrPrevious, ptrInstanceBuf, nBytes, 0);
			 string InstanceID = Marshal.PtrToStringAuto(ptrInstanceBuf);

			 Marshal.FreeHGlobal(ptrInstanceBuf);
		   }
		 }
		 i++;
	   }
	}
	SetupDiDestroyDeviceInfoList(h);
*/


	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CCamera::Init( int nDevId )
{ 
	if( nDevId < 0 || nDevId >= m_vectHWDevices.size() ) return( 0 );

	// create unique download path
	wxString strDownloadPath;
	strDownloadPath.Printf( wxT("%s/%s/"), unMakeAppPath(wxT("data/temp/camera")), m_vectHWDevices[nDevId].strName );
	// check if folder exists
	if( !wxDirExists( strDownloadPath ) ) wxMkdir( strDownloadPath );

	// :: CANON EOS
	if( m_vectHWDevices[nDevId].interface_id == 1 )
	{
		if( !m_pCameraCanonEos ) m_pCameraCanonEos = new CCameraCanonEos( this ); 
		m_pCameraCanonEos->m_strDownloadPath = strDownloadPath;
		m_pCameraCanonEos->Init(); 

	// :: VIA INTERFACE :: DIGITAL SNAP
	} else if( m_vectHWDevices[nDevId].wia_type == 2 && m_vectHWDevices[nDevId].interface_id == 0 )
	{
		if( !m_pCameraWia ) m_pCameraWia = new CCameraWia( this );
		m_pCameraWia->SetDownloadPath( strDownloadPath );

	// :: OPENCV
	} else if( m_vectHWDevices[nDevId].wia_type == 3 )
	{
		if( !m_pCameraVideo ) 
		{
			m_pCameraVideo = new CCameraVideo();
			m_pCameraVideo->SetConfig( this, m_pFrame, &(m_vectHWDevices[nDevId]) );
		}
		m_pCameraVideo->Start();
	}


	return( 1 );
}

////////////////////////////////////////////////////////////////////
// connect to camera
////////////////////////////////////////////////////////////////////
int CCamera::Connect( int nDevId )
{
	if( nDevId < 0 || nDevId >= m_vectHWDevices.size() ) return( 0 );

	// :: CANON EOS
	if( m_vectHWDevices[nDevId].interface_id == 1 )
	{
		m_pCameraCanonEos->Connect();

	// :: VIA INTERFACE :: DIGITAL SNAP
	} else if( m_vectHWDevices[nDevId].wia_type == 2 && m_vectHWDevices[nDevId].interface_id == 0 )
	{
		// do something on connect ??
		m_pCameraWia->Connect();

	// :: OPENCV
	} else if( m_vectHWDevices[nDevId].wia_type == 3 )
	{
		//m_pCameraVideo->Start();
	}
	// set flag to update gui
	m_vectHWDevices[nDevId].do_update_gui = 1;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CCamera::Disconnect( int nDevId )
{

	// :: CANON EOS
	if( m_vectHWDevices[nDevId].interface_id == 1 )
	{
		delete( m_pCameraCanonEos );
		m_pCameraCanonEos = NULL;

	// :: VIA INTERFACE :: DIGITAL SNAP
	} else if( m_vectHWDevices[nDevId].wia_type == 2 && m_vectHWDevices[nDevId].interface_id == 0 )
	{
		delete( m_pCameraWia );
		m_pCameraWia = NULL;

	// :: OPENCV
	} else if( m_vectHWDevices[nDevId].wia_type == 3 )
	{
		if( m_pCameraVideo ) m_pCameraVideo->Stop();
	}
	// set flag to not update gui - just in case
	m_vectHWDevices[nDevId].do_update_gui = 0;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CCamera::SendDisconnect( )
{
	DefCmdAction cmd;
	cmd.id = THREAD_DCAMERA_ACTION_DISCONNECT;
	m_pDCameraWorker->SetAction( cmd );

	return( 1 );
}

// take a photo
////////////////////////////////////////////////////////////////////
int CCamera::TakePhoto( int nDevId, const wxString& strPath )
{
	if( nDevId < 0 || nDevId >= m_vectHWDevices.size() ) return( 0 );

	// :: CANON EOS
	if( m_vectHWDevices[nDevId].interface_id == 1 )
	{
		m_pCameraCanonEos->TakePictureCommand( strPath );

	// :: VIA INTERFACE :: DIGITAL SNAP
	} else if( m_vectHWDevices[nDevId].wia_type == 2 && m_vectHWDevices[nDevId].interface_id == 0 )
	{
		m_pCameraWia->WiaTakePicture( m_vectHWDevices[nDevId].strDevId, NULL, strPath );
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// add image frame
////////////////////////////////////////////////////////////////////
void CCamera::AddImageFrame( const wxString& file_name )
{
	DefCameraFrames elem;
	elem.file_name = file_name;
	m_vectCameraFrames.push_back( elem );

	DefCmdAction cmd;
	cmd.id = THREAD_DCAMERA_ACTION_NEW_IMAGE;
	m_pDCameraWorker->SetAction( cmd );

}

////////////////////////////////////////////////////////////////////
int CCamera::LoadHardwareDef( int nHType, int nType, int nBrand )
{
	// check if already loaded
	if( nHType == CAMERA_HARDWARE_MAIN_BODY && m_vectCameraBrandDef.size() ) return( 0 );
	if( nHType == CAMERA_HARDWARE_LENS && m_vectCameraLensBrandDef.size() ) return( 0 );

	FILE* pFile = NULL;
	wxString strFile;
	wxChar strLine[500];
	wxChar strTmp[500];
	int nLineSize=0, bFoundSection=0, i=0, nBrandCount=0;

	/////////////////////////
	// FREE ALLOACTION/BUILD FILE NAME BY TYPE
	if( nHType == CAMERA_HARDWARE_MAIN_BODY )
	{
		UnloadCameraBrandsDef();
		strFile.Printf( wxT("%s"), DEFAULT_CAMERA_BRAND_DEF_FILE );

	} else if( nHType == CAMERA_HARDWARE_LENS )
	{
		UnloadCameraLensBrandsDef();
		strFile.Printf( wxT("%s"), DEFAULT_CAMERA_LENS_BRAND_DEF_FILE );

	}

	//////////////////////////
	// CAMERA BRANDS DEF INI FILE
	// build file name
//	sprintf( strFile, "%s", strFile );
	// open file to read
	pFile = wxFopen( strFile, wxT("r") );
	// check if there is any configuration to load
	if( !pFile ) return( -1 );

	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// get one line out of the config file
		wxFgets( strLine, 500, pFile );
		nLineSize = wxStrlen( strLine );

		// check if to skip because line to short or comment
		if( nLineSize < 4 || strLine[0] == '#' ) continue;

		// check section
		if( strLine[0] == '[' && strLine[nLineSize-2] == ']' )
		{
			bFoundSection = 1;
			wxChar strName[255];
			wxSscanf( strLine, wxT("\[%[^\]]\]\n"), strName );
			wxString strWxName = strName;

			// add brand by type
			if( nHType == CAMERA_HARDWARE_MAIN_BODY )
				nBrandCount = AddCameraBrandDef( strWxName );
			else if( nHType == CAMERA_HARDWARE_LENS )
				nBrandCount = AddCameraLensBrandDef( strWxName );

		} else if( bFoundSection && wxStrncmp( strLine, wxT("IniFile="), 8 ) == 0 )
		{
			wxSscanf( strLine, wxT("IniFile=%[^\n]\n"), strTmp );

			// set by hardware type
			if( nHType == CAMERA_HARDWARE_MAIN_BODY )
				m_vectCameraBrandDef[nBrandCount].brand_ini_file = wxString(strTmp,wxConvUTF8);
			else if( nHType == CAMERA_HARDWARE_LENS )
				m_vectCameraLensBrandDef[nBrandCount].brand_ini_file = wxString(strTmp,wxConvUTF8);

		} else if( bFoundSection && wxStrncmp( strLine, wxT("Types="), 6 ) == 0 )
		{
			wxSscanf( strLine, wxT("Types=%[^\n]\n"), strTmp );
			wxString strWxTmp = strTmp;

			unsigned char type_count=0;
			// extract individual nodes
			wxStringTokenizer tkz( strWxTmp, wxT(",") );
			while ( tkz.HasMoreTokens() )
			{
				// get next node from input string
				strWxTmp = tkz.GetNextToken();
				// get type id
				if( nHType == CAMERA_HARDWARE_MAIN_BODY )
					m_vectCameraBrandDef[nBrandCount].vectTypes[type_count] = GetHardwareTypeIdByString( nHType, strWxTmp );
				else if( nHType == CAMERA_HARDWARE_LENS )
					m_vectCameraLensBrandDef[nBrandCount].vectTypes[type_count] = GetHardwareTypeIdByString( nHType, strWxTmp );

				type_count++;
				// safety break
				if( type_count >= 40 ) break;
			}
			// set count by type
			if( nHType == CAMERA_HARDWARE_MAIN_BODY )
				m_vectCameraBrandDef[nBrandCount].nTypes = type_count;
			else if( nHType == CAMERA_HARDWARE_LENS )
				m_vectCameraLensBrandDef[nBrandCount].nTypes = type_count;

		}
	}
	// close file
	fclose( pFile );

	///////////////////////
	// if -1 load all brands
	if( nBrand == -1 )
		for( i=0; i<nBrandCount+1; i++ ) LoadHardwareIniByBrandId( nHType, i );
	else if( nBrand >= 0 )
		LoadHardwareIniByBrandId( nHType, nBrand );

	return( nBrandCount+1 );
}

////////////////////////////////////////////////////////////////////
int CCamera::GetHardwareTypeIdByString( int nHType, const wxChar* strType )
{
	int i=0, nType=0, nTypes=0;

	if( nHType == CAMERA_HARDWARE_MAIN_BODY )
	{
		// for all types
		for( i=0; i<g_nCameraTypes; i++ )
		{
			wxString strRType = wxString(g_vectCameraTypes[i].type_string,wxConvUTF8);
			if( wxStricmp( strType, strRType ) == 0 )
			{
				nType = i;
				// also set the global use
				g_vectCameraTypes[i].has_type = 1;
			}
		}

	} else if( nHType == CAMERA_HARDWARE_LENS )
	{
		// for all types
		for( i=0; i<g_nCameraLensTypes; i++ )
		{
			wxString strRType = wxString(g_vectCameraLensTypes[i].type_string,wxConvUTF8);
			if( wxStricmp( strType, strRType ) == 0 )
			{
				nType = i;
				// also set the global use
				g_vectCameraLensTypes[i].has_type = 1;
			}
		}
	}

	return( nType );
}

////////////////////////////////////////////////////////////////////
int CCamera::LoadHardwareIniByBrandId( int nHType, int nBrandId )
{
	// don't reload if already loaded
	if( nHType == CAMERA_HARDWARE_MAIN_BODY && m_vectCameraBrandDef[nBrandId].vectData.size() ) return( 0 );
	if( nHType == CAMERA_HARDWARE_LENS && m_vectCameraLensBrandDef[nBrandId].vectData.size() ) return( 0 );

	FILE* pFile = NULL;
	wxString strFile;
	wxChar strLine[500];
	wxChar strTmp[500];
	int nLineSize=0;
	int bFoundSection=0, nHardId=0;

	/////////////////////////
	// FREE ALLOACTION/BUILD FILE NAME BY TYPE
	if( nHType == CAMERA_HARDWARE_MAIN_BODY )
	{
		//UnloadCameraDef( nBrandId );
		strFile.Printf( wxT("%s"), m_vectCameraBrandDef[nBrandId].brand_ini_file );

	} else if( nHType == CAMERA_HARDWARE_LENS )
	{
		//UnloadCameraLensDef( nBrandId );
		strFile.Printf( wxT("%s"), m_vectCameraLensBrandDef[nBrandId].brand_ini_file );

	}

	// open file to read
	pFile = wxFopen( strFile, wxT("r") );
	// check if there is any configuration to load
	if( !pFile ) return( -1 );

	// Reading lines from cameras ini file
	while( !feof( pFile ) )
	{
		// get one line out of the config file
		wxFgets( strLine, 500, pFile );
		nLineSize = wxStrlen( strLine );

		// check if to skip because line to short or comment
		if( nLineSize < 4 || strLine[0] == '#' ) continue;

		// check section
		//if( reSectionLabel.Matches( strLine ) )
		if( strLine[0] == '[' && strLine[nLineSize-2] == ']' )
		{
			bFoundSection = 1;
			wxChar strName[255];
			wxSscanf( strLine, wxT("\[%[^\]]\]\n"), strName );

			// by hardware type
			if( nHType == CAMERA_HARDWARE_MAIN_BODY )
				nHardId = AddCameraDef( nBrandId, strName );
			else if( nHType == CAMERA_HARDWARE_LENS )
				nHardId = AddCameraLensDef( nBrandId, strName );

		////////////////////
		// CAMERA FIELDS
		} else if( nHType == CAMERA_HARDWARE_MAIN_BODY )
		{
			if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				wxSscanf( strLine, wxT("Type=%[^\n]\n"), strTmp );
				m_vectCameraBrandDef[nBrandId].vectData[nHardId].type = GetHardwareTypeIdByString( nHType, strTmp );			

			} else if( wxStrncmp( strLine, wxT("SensorWidth="), 12 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("SensorWidth=%lf\n"), &(m_vectCameraBrandDef[nBrandId].vectData[nHardId].sensor_width) ) )
					m_vectCameraBrandDef[nBrandId].vectData[nHardId].sensor_width = 0;

			} else if( wxStrncmp( strLine, wxT("SensorHeight="), 13 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("SensorHeight=%lf\n"), &(m_vectCameraBrandDef[nBrandId].vectData[nHardId].sensor_height) ) )
					m_vectCameraBrandDef[nBrandId].vectData[nHardId].sensor_height = 0;

			} else if( wxStrncmp( strLine, wxT("SensorPixelSize="), 16 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("SensorPixelSize=%lf\n"), &(m_vectCameraBrandDef[nBrandId].vectData[nHardId].sensor_psize) ) )
					m_vectCameraBrandDef[nBrandId].vectData[nHardId].sensor_psize = 0;

			}

		////////////////////
		// LENS FIELDS
		} else if( nHType == CAMERA_HARDWARE_LENS )
		{
			if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				wxSscanf( strLine, wxT("Type=%[^\n]\n"), strTmp );
				m_vectCameraLensBrandDef[nBrandId].vectData[nHardId].type = GetHardwareTypeIdByString( nHType, strTmp );			

			} else if( wxStrncmp( strLine, wxT("FocalLength="), 12 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("FocalLength=%lf\n"), &(m_vectCameraLensBrandDef[nBrandId].vectData[nHardId].focal_length) ) )
					m_vectCameraLensBrandDef[nBrandId].vectData[nHardId].focal_length = 0;

			} else if( wxStrncmp( strLine, wxT("Aperture="), 9 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Aperture=%lf\n"), &(m_vectCameraLensBrandDef[nBrandId].vectData[nHardId].aperture) ) )
					m_vectCameraLensBrandDef[nBrandId].vectData[nHardId].aperture = 0;

			} else if( wxStrncmp( strLine, wxT("ViewAngle="), 10 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("ViewAngle=%lf\n"), &(m_vectCameraLensBrandDef[nBrandId].vectData[nHardId].view_angle) ) )
					m_vectCameraLensBrandDef[nBrandId].vectData[nHardId].view_angle = 0;
			}
		}
	}

	// close file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CCamera::HasBrandHardwareType( int nHType, int nBrand, int nType )
{
	if( nHType == CAMERA_HARDWARE_MAIN_BODY )
	{
		for( int i=0; i<m_vectCameraBrandDef[nBrand].nTypes; i++ )
			if( m_vectCameraBrandDef[nBrand].vectTypes[i] == nType ) return(1); 

	} else if( nHType == CAMERA_HARDWARE_LENS )
	{
		for( int i=0; i<m_vectCameraLensBrandDef[nBrand].nTypes; i++ )
			if( m_vectCameraLensBrandDef[nBrand].vectTypes[i] == nType ) return(1); 
	}

	return(0); 
}

////////////////////////////////////////////////////////////////////
void CCamera::SetGuiSelect( int nHType, wxChoice* pType, wxChoice* pBrand, wxChoice* pName, 
							int nType, int nBrand, int nName, std::vector<int> &vectBrandId, std::vector<int> &vectNameId )
{
	int i=0;
	int nHardSpecType=0, nHBrand=0, nBrandSelect=0;

	// set counts by type
	if( nHType == CAMERA_HARDWARE_MAIN_BODY )
	{
		nHardSpecType = g_nCameraTypes;
		nHBrand = m_vectCameraBrandDef.size();

	} else if( nHType == CAMERA_HARDWARE_LENS )
	{
		nHardSpecType = g_nCameraLensTypes;
		nHBrand = m_vectCameraLensBrandDef.size();
	}

	// set types
	if( pType != NULL )
	{
		pType->Freeze();
		pType->Clear();
		for( i=0; i<nHardSpecType; i++ ) 
		{
			if( nHType == CAMERA_HARDWARE_MAIN_BODY )
				pType->Append( g_vectCameraTypes[i].type_label );
			else if( nHType == CAMERA_HARDWARE_LENS )
				pType->Append( g_vectCameraLensTypes[i].type_label );
		}
		pType->SetSelection( nType );
		pType->Thaw();
	}

	///////////////////
	// set brands
	vectBrandId.clear();
	pBrand->Freeze();
	pBrand->Clear();
	for( i=0; i<nHBrand; i++ )
	{
		// :: if all(0) or specific
		if( nType == 0 || HasBrandHardwareType( nHType, i, nType ) )
		{
			if( nHType == CAMERA_HARDWARE_MAIN_BODY )
				pBrand->Append( m_vectCameraBrandDef[i].brand_name );
			else if( nHType == CAMERA_HARDWARE_LENS )
				pBrand->Append( m_vectCameraLensBrandDef[i].brand_name );
			else
				continue;

			// check/get brand select
			if( nBrand == i ) nBrandSelect = vectBrandId.size();
			vectBrandId.push_back( i );
		}

	}
	pBrand->SetSelection( nBrandSelect );
	pBrand->Thaw();

	if( vectBrandId.size() >  0 ) 
	{
		// load - check load
		LoadHardwareIniByBrandId( nHType, nBrand );
		// set name 
		SetGuiNameSelect( nHType, pName, nType, nBrand, nName, vectNameId );

	} else
	{
		pName->Clear();
		pName->SetSelection(0);
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CCamera::SetGuiNameSelect( int nHType, wxChoice* pName, int nType,
							   int nBrand, int nName, std::vector<int> &vectNameId )
{
	int i=0;
	int nHName=0, nNameSelect=0, bFound=0;

	if( nHType == CAMERA_HARDWARE_MAIN_BODY )
		nHName = m_vectCameraBrandDef[nBrand].vectData.size();
	else if( nHType == CAMERA_HARDWARE_LENS )
		nHName = m_vectCameraLensBrandDef[nBrand].vectData.size();

	// populate slect and ids
	vectNameId.clear();
	pName->Freeze();
	pName->Clear();
	for( i=0; i<nHName; i++ ) 
	{
		// check for hardware type
		if( nHType == CAMERA_HARDWARE_MAIN_BODY )
		{
			// :: if all(-1) or specific
			if( nType == 0 || nType == m_vectCameraBrandDef[nBrand].vectData[i].type )
			{
				pName->Append( m_vectCameraBrandDef[nBrand].vectData[i].name );
				bFound = 1;
			}

		} else if( nHType == CAMERA_HARDWARE_LENS )
		{
			// :: if all(-1) or specific
			if( nType == 0 || nType == m_vectCameraLensBrandDef[nBrand].vectData[i].type )
			{
				pName->Append( m_vectCameraLensBrandDef[nBrand].vectData[i].name );
				bFound = 1;
			}
		}

		// if found
		if( bFound )
		{
			// check/get brand select
			if( nName == i ) nNameSelect = vectNameId.size();
			vectNameId.push_back( i );
			bFound = 0;
		}
	}
	pName->SetSelection( nNameSelect );
	pName->Thaw();
}

////////////////////////////////////////////////////////////////////
// Method:	AddCameraBrandDef
////////////////////////////////////////////////////////////////////
int CCamera::AddCameraBrandDef( const wxString& strName )
{
/*
	if( !m_vectCameraBrandDef )
	{
		// do the initial allocation
		m_nCameraBrandDef = 0;
		m_nCameraBrandDefAllocated = 20;
		m_vectCameraBrandDef = (DefCameraBrand*) calloc( m_nCameraBrandDefAllocated, sizeof(DefCameraBrand) );

	} else if( m_nCameraBrandDef >= m_nCameraBrandDefAllocated )
	{
		m_nCameraBrandDefAllocated += 10;
		m_vectCameraBrandDef = (DefCameraBrand*) _recalloc( m_vectCameraBrandDef, 
									m_nCameraBrandDefAllocated, sizeof(DefCameraBrand) );
	}
	// set camera brand to zero
	memset( &(m_vectCameraBrandDef[m_nCameraBrandDef]), 0, sizeof(DefCameraBrand) );

	m_vectCameraBrandDef[m_nCameraBrandDef].brand_name = strName;
	m_vectCameraBrandDef[m_nCameraBrandDef].nData = 0;
	m_vectCameraBrandDef[m_nCameraBrandDef].nDataAllocated = 0;
	m_vectCameraBrandDef[m_nCameraBrandDef].vectData = NULL;

	m_nCameraBrandDef++;
*/
	DefCameraBrand elem;
	elem.brand_name = strName;
//	elem.nData = 0;
//	elem.nDataAllocated = 0;
	elem.vectData.clear();

	m_vectCameraBrandDef.push_back( elem );

	return( m_vectCameraBrandDef.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CCamera::AddCameraDef( int nBrand, const wxString& strName )
{
/*	if( !m_vectCameraBrandDef[nBrand].vectData )
	{
		m_vectCameraBrandDef[nBrand].nData = 0;
		m_vectCameraBrandDef[nBrand].nDataAllocated = 50;
		m_vectCameraBrandDef[nBrand].vectData = (DefCamera*) calloc( m_vectCameraBrandDef[nBrand].nDataAllocated, sizeof(DefCamera) );

	} else if( m_vectCameraBrandDef[nBrand].nData >= m_vectCameraBrandDef[nBrand].nDataAllocated )
	{
		m_vectCameraBrandDef[nBrand].nDataAllocated += 10;
		m_vectCameraBrandDef[nBrand].vectData = (DefCamera*) _recalloc( m_vectCameraBrandDef[nBrand].vectData, 
															m_vectCameraBrandDef[nBrand].nDataAllocated, sizeof(DefCamera) );
	}
	int nCamera = m_vectCameraBrandDef[nBrand].nData;
	// set camera hardware to zero
	memset( &(m_vectCameraBrandDef[nBrand].vectData[nCamera]), 0, sizeof(DefCamera) );

	m_vectCameraBrandDef[nBrand].vectData[nCamera].name = wxString(strName,wxConvUTF8);
	// this might need to get a pointer index from the brands structure instead
	m_vectCameraBrandDef[nBrand].vectData[nCamera].brand = m_vectCameraBrandDef[nBrand].brand_name;
	m_vectCameraBrandDef[nBrand].nData++;
*/
	DefCamera elem;
	elem.name = strName;
	elem.brand = m_vectCameraBrandDef[nBrand].brand_name;

	m_vectCameraBrandDef[nBrand].vectData.push_back( elem );

	return( m_vectCameraBrandDef[nBrand].vectData.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CCamera::IsCameraDef( int nBrand, int nName )
{
	if( nBrand < 0 || nBrand >= m_vectCameraBrandDef.size() ) return(0);
	if( nName < 0 || nName >= m_vectCameraBrandDef[nBrand].vectData.size() ) return(0);

	return( 1 );
}

////////////////////////////////////////////////////////////////////
void CCamera::UnloadCameraBrandsDef( )
{
	int i=0;
	if( m_vectCameraBrandDef.size() ) 
	{
		for( i=0; i<m_vectCameraBrandDef.size(); i++ ) UnloadCameraDef( i );
		//free( m_vectCameraBrandDef );
	}
/*	m_vectCameraBrandDef = NULL;
	m_nCameraBrandDef = 0;
	m_nCameraBrandDefAllocated = 0;
*/
	m_vectCameraBrandDef.clear();
}

////////////////////////////////////////////////////////////////////
void CCamera::UnloadCameraDef( int nBrand )
{
/*	if( m_vectCameraBrandDef[nBrand].vectData ) 
		free( m_vectCameraBrandDef[nBrand].vectData );
	m_vectCameraBrandDef[nBrand].vectData = NULL;
	m_vectCameraBrandDef[nBrand].nData = 0;
	m_vectCameraBrandDef[nBrand].nDataAllocated = 0;
*/
	m_vectCameraBrandDef[nBrand].vectData.clear();
}

////////////////////////////////////////////////////////////////////
// Method:	AddCameraLensBrandDef
////////////////////////////////////////////////////////////////////
int CCamera::AddCameraLensBrandDef( const wxString& strName )
{
/*	if( !m_vectCameraLensBrandDef )
	{
		// do the initial allocation
		m_nCameraLensBrandDef = 0;
		m_nCameraLensBrandDefAllocated = 20;
		m_vectCameraLensBrandDef = (DefCameraLensBrand*) calloc( m_nCameraLensBrandDefAllocated, sizeof(DefCameraLensBrand) );

	} else if( m_nCameraLensBrandDef >= m_nCameraLensBrandDefAllocated )
	{
		m_nCameraLensBrandDefAllocated += 10;
		m_vectCameraLensBrandDef = (DefCameraLensBrand*) _recalloc( m_vectCameraLensBrandDef, 
									m_nCameraLensBrandDefAllocated, sizeof(DefCameraLensBrand) );
	}
	// set camera lens brand to zero
	memset( &(m_vectCameraLensBrandDef[m_nCameraLensBrandDef]), 0, sizeof(DefCameraLensBrand) );

	m_vectCameraLensBrandDef[m_nCameraLensBrandDef].brand_name = wxString(strName,wxConvUTF8);
	m_vectCameraLensBrandDef[m_nCameraLensBrandDef].nData = 0;
	m_vectCameraLensBrandDef[m_nCameraLensBrandDef].nDataAllocated = 0;
	m_vectCameraLensBrandDef[m_nCameraLensBrandDef].vectData = NULL;

	m_nCameraLensBrandDef++;
*/
	DefCameraLensBrand elem;
	elem.brand_name = strName;
	elem.vectData.clear();

	m_vectCameraLensBrandDef.push_back( elem );

	return( m_vectCameraLensBrandDef.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CCamera::IsCameraLensDef( int nBrand, int nName )
{
	if( nBrand < 0 || nBrand >= m_vectCameraLensBrandDef.size() ) return(0);
	if( nName < 0 || nName >= m_vectCameraLensBrandDef[nBrand].vectData.size() ) return(0);

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CCamera::AddCameraLensDef( int nBrand, const wxString& strName )
{
/*	if( !m_vectCameraLensBrandDef[nBrand].vectData )
	{
		m_vectCameraLensBrandDef[nBrand].nData = 0;
		m_vectCameraLensBrandDef[nBrand].nDataAllocated = 50;
		m_vectCameraLensBrandDef[nBrand].vectData = (DefCameraLens*) calloc( m_vectCameraLensBrandDef[nBrand].nDataAllocated, sizeof(DefCameraLens) );

	} else if( m_vectCameraLensBrandDef[nBrand].nData >= m_vectCameraLensBrandDef[nBrand].nDataAllocated )
	{
		m_vectCameraLensBrandDef[nBrand].nDataAllocated += 10;
		m_vectCameraLensBrandDef[nBrand].vectData = (DefCameraLens*) _recalloc( m_vectCameraLensBrandDef[nBrand].vectData, 
															m_vectCameraLensBrandDef[nBrand].nDataAllocated, sizeof(DefCameraLens) );
	}
	int nCameraLens = m_vectCameraLensBrandDef[nBrand].nData;
	// set camera lens hardware to zero
	memset( &(m_vectCameraLensBrandDef[nBrand].vectData[nCameraLens]), 0, sizeof(DefCameraLens) );

	m_vectCameraLensBrandDef[nBrand].vectData[nCameraLens].name = wxString(strName,wxConvUTF8);
	// this might need to get a pointer index from the brands structure instead
	m_vectCameraLensBrandDef[nBrand].vectData[nCameraLens].brand = m_vectCameraLensBrandDef[nBrand].brand_name;
	m_vectCameraLensBrandDef[nBrand].nData++;
*/
	DefCameraLens elem;
	elem.name = strName;
	elem.brand = m_vectCameraLensBrandDef[nBrand].brand_name;

	m_vectCameraLensBrandDef[nBrand].vectData.push_back( elem );
		
	return( m_vectCameraLensBrandDef[nBrand].vectData.size()-1 );
}

////////////////////////////////////////////////////////////////////
void CCamera::UnloadCameraLensBrandsDef( )
{
	int i=0;
	if( m_vectCameraLensBrandDef.size() ) 
	{
		for( i=0; i<m_vectCameraLensBrandDef.size(); i++ ) UnloadCameraLensDef( i );
		//free( m_vectCameraLensBrandDef );
	}

	m_vectCameraLensBrandDef.clear();
//	m_nCameraLensBrandDef = 0;
//	m_nCameraLensBrandDefAllocated = 0;
}

////////////////////////////////////////////////////////////////////
void CCamera::UnloadCameraLensDef( int nBrand )
{
	if( m_vectCameraLensBrandDef[nBrand].vectData.size() ) 
		m_vectCameraLensBrandDef[nBrand].vectData.clear();
//	m_vectCameraLensBrandDef[nBrand].vectData = NULL;
//	m_vectCameraLensBrandDef[nBrand].nData = 0;
//	m_vectCameraLensBrandDef[nBrand].nDataAllocated = 0;
}

////////////////////////////////////////////////////////////////////
int CCamera::IsConnected( int nDevId )
{
	// :: CANON EOS
	if( m_vectHWDevices[nDevId].interface_id == 1 )
	{
		if( m_pCameraCanonEos )
			return( m_pCameraCanonEos->m_bIsConnected );
		else
			return( 0 );

	// :: VIA INTERFACE :: DIGITAL SNAP
	} else if( m_vectHWDevices[nDevId].wia_type == 2 && m_vectHWDevices[nDevId].interface_id == 0 )
	{
		// do something on connect ?? - m_bConnected
		if( m_pCameraWia )
			return( m_pCameraWia->m_bConnected );
		else
			return( 0 );

	// if camera video
	} else if( m_vectHWDevices[nDevId].wia_type == 3 )
	{
		if( m_pCameraVideo )
			return( m_pCameraVideo->m_isRunning );
		else
			return( 0 );
	}

	return( 0 );
}

////////////////////////////////////////////////////////////////////
void CCamera::SetGuiUpdate( int nDevId, int bState )
{
	m_vectHWDevices[nDevId].do_update_gui = bState;
}
