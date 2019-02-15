////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system headers
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <objbase.h>
#include <atlbase.h>
#include <sti.h>
#include <gdiplus.h>
#include <vector>
#include <wia.h>

// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/filename.h>

// canon eos headers
#include "canoneos/EDSDK.h"
#include "canoneos/EDSDKTypes.h"
#include "canoneos/CameraModelLegacy.h"
#include "canoneos/CameraEventListener.h"

// local headers
#include "../util/func.h"
#include "../observer/observer_hardware.h"
#include "camera.h"
#include "worker_dcam.h"

// main header
#include "camera_canoneos.h"

////////////////////////////////////////////////////////////////////
CCameraCanonEos::CCameraCanonEos( CCamera* pCamera ) : CCameraDriver( )
{
	m_pCamera = pCamera;
	m_bIsConnected = 0;
}

////////////////////////////////////////////////////////////////////
CCameraCanonEos::~CCameraCanonEos( )
{
	Close( );
}

// initialize interface and look for camera
////////////////////////////////////////////////////////////////////
int CCameraCanonEos::Init( )
{
	if( m_bIsInterfaceInit ) return( 1 );

	EdsError err = EDS_ERR_OK;
	EdsCameraListRef cameraList = NULL;
	camera = NULL;
	EdsUInt32	 count = 0;	
	m_isSDKLoaded = false;

	// Initialization of SDK
	err = EdsInitializeSDK();

	if( err == EDS_ERR_OK ) m_isSDKLoaded = true;

	// Acquisition of camera list
	if( err == EDS_ERR_OK ) err = EdsGetCameraList(&cameraList);
	// Acquisition of number of Cameras
	if( err == EDS_ERR_OK )
	{
		err = EdsGetChildCount(cameraList, &count);
		if( count == 0 ) err = EDS_ERR_DEVICE_NOT_FOUND;
	}
	// Acquisition of camera at the head of the list
	if( err == EDS_ERR_OK ) err = EdsGetChildAtIndex(cameraList , 0 , &camera);	

	//Acquisition of camera information
	EdsDeviceInfo deviceInfo;
	memset( &deviceInfo, 0, sizeof(EdsDeviceInfo) );
	if( err == EDS_ERR_OK )
	{	
		err = EdsGetDeviceInfo( camera , &deviceInfo );	
		if( err == EDS_ERR_OK && camera == NULL ) err = EDS_ERR_DEVICE_NOT_FOUND;
	}

	//Release camera list
	if( cameraList != NULL ) EdsRelease(cameraList);

	//Create Camera model
	if(err == EDS_ERR_OK )
	{
		//m_pModel = cameraModelFactory(camera, deviceInfo);
		// if Legacy protocol.
		if(deviceInfo.deviceSubType == 0)
			m_pModel = new CameraModelLegacy(camera);
		// PTP protocol.
		else
			m_pModel = new CameraModel(camera);

		if( m_pModel == NULL ) err = EDS_ERR_DEVICE_NOT_FOUND;
	}

	//////////////////////////////
	//Set Property Event Handler
	if( err == EDS_ERR_OK )
		err = EdsSetPropertyEventHandler( camera, kEdsPropertyEvent_All, CameraEventListener::handlePropertyEvent, (EdsVoid*) this );

	//Set Object Event Handler
	if( err == EDS_ERR_OK )
		err = EdsSetObjectEventHandler( camera, kEdsObjectEvent_All, CameraEventListener::handleObjectEvent, (EdsVoid*) this );

	//Set State Event Handler
	if(err == EDS_ERR_OK)
		err = EdsSetCameraStateEventHandler( camera, kEdsStateEvent_All, CameraEventListener::handleStateEvent, (EdsVoid*) this );

	// notify on error
	if( err != EDS_ERR_OK ) wxMessageBox( wxT("Cannot detect camera") );

	if( err == EDS_ERR_OK )
		m_bIsInterfaceInit = 1;
	else
		m_bIsInterfaceInit = 0;

	return( m_bIsInterfaceInit );
}

// terminate/release camera
////////////////////////////////////////////////////////////////////
int CCameraCanonEos::Close( )
{
	//Release Camera
	if( camera != NULL )
	{
		EdsRelease(camera);
		camera = NULL;
	}

	//Termination of SDK
	if( m_isSDKLoaded )
	{
		EdsTerminateSDK();
	}
	
	if( m_pModel != NULL )
	{
		delete( m_pModel );
		m_pModel = NULL;
	}
	m_bIsConnected = 0;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Connect to camera
////////////////////////////////////////////////////////////////////
int CCameraCanonEos::Connect( )
{
	// check interface initialization 
	if( !m_bIsInterfaceInit ) return( 0 );

	// make connection to camera
	OpenSessionCommand( );
	// get initial properties
	GetPropertyCommand( kEdsPropID_ProductName );

	return( 1 );
}

// Open A Session to camera
////////////////////////////////////////////////////////////////////
int CCameraCanonEos::OpenSessionCommand( )
{
	EdsError err = EDS_ERR_OK;
	bool locked = false;

	//The communication with the camera begins
	err = EdsOpenSession( m_pModel->getCameraObject() );

	if( m_pModel->isLegacy() )
	{
		//Preservation ahead is set to PC
		if( err == EDS_ERR_OK )
		{
			EdsUInt32 saveTo = kEdsSaveTo_Host;
			err = EdsSetPropertyData( m_pModel->getCameraObject(), kEdsPropID_SaveTo, 0, sizeof(saveTo) , &saveTo );
		}

		GetPropertyCommand( kEdsPropID_Unknown );
		GetPropertyDescCommand( kEdsPropID_Unknown );
	
	} else
	{
		//Preservation ahead is set to PC
		if( err == EDS_ERR_OK )
		{
			EdsUInt32 saveTo = kEdsSaveTo_Host;
			err = EdsSetPropertyData(m_pModel->getCameraObject(), kEdsPropID_SaveTo, 0, sizeof(saveTo) , &saveTo);
		}

		//UI lock
		if( err == EDS_ERR_OK )
			err = EdsSendStatusCommand(m_pModel->getCameraObject(), kEdsCameraStatusCommand_UILock, 0);

		if( err == EDS_ERR_OK )
			locked = true;
		
		if( err == EDS_ERR_OK )
		{
			EdsCapacity capacity = {0x7FFFFFFF, 0x1000, 1};
			err = EdsSetCapacity( m_pModel->getCameraObject(), capacity);
		}
		
		//It releases it when locked
		if( locked )
			EdsSendStatusCommand(m_pModel->getCameraObject(), kEdsCameraStatusCommand_UIUnLock, 0);
	}

	//Notification of error
	if(err != EDS_ERR_OK)
	{
//		CameraEvent e("error", &err);
//		m_pModel->notifyObservers(&e);

	} //else
	//{
		m_bIsConnected = 1;
	//}

	return( 1 );
}

// get camera properties
////////////////////////////////////////////////////////////////////
int CCameraCanonEos::GetPropertyCommand( EdsPropertyID propertyID )
{
	EdsError err = EDS_ERR_OK;
	bool	 locked = false;
	
	// For cameras earlier than the 30D , the UI must be locked before commands are reissued
	if( m_pModel->isLegacy() )
	{
		err = EdsSendStatusCommand( m_pModel->getCameraObject(), kEdsCameraStatusCommand_UILock, 0 );
		if(err == EDS_ERR_OK)
		{
			locked = true;
		}
	}		
	
	//Get property value
	if( err == EDS_ERR_OK )
	{
		err = getProperty( propertyID );
	}

	//It releases it when locked
	if( locked )
	{
		EdsSendStatusCommand( m_pModel->getCameraObject(), kEdsCameraStatusCommand_UIUnLock, 0 );
	}


	//Notification of error
	if( err != EDS_ERR_OK )
	{
		// It retries it at device busy
		if( (err & EDS_ERRORID_MASK) == EDS_ERR_DEVICE_BUSY )
		{
//			CameraEvent e("DeviceBusy");
//			m_pModel->notifyObservers(&e);
			return false;
		}

//		CameraEvent e("error", &err);
//		m_pModel->notifyObservers(&e);
	}

	return( 1 );	
}

// get property
////////////////////////////////////////////////////////////////////
int CCameraCanonEos::getProperty( EdsPropertyID propertyID )
{
	EdsError err = EDS_ERR_OK;
	EdsDataType	dataType = kEdsDataType_Unknown;
	EdsUInt32   dataSize = 0;


	if(propertyID == kEdsPropID_Unknown)
	{
		//If unknown is returned for the property ID , the required property must be retrieved again
		if(err == EDS_ERR_OK) err = getProperty(kEdsPropID_AEMode);
		if(err == EDS_ERR_OK) err = getProperty(kEdsPropID_Tv);
		if(err == EDS_ERR_OK) err = getProperty(kEdsPropID_Av);
		if(err == EDS_ERR_OK) err = getProperty(kEdsPropID_ISOSpeed);
		if(err == EDS_ERR_OK) err = getProperty(kEdsPropID_MeteringMode);
		if(err == EDS_ERR_OK) err = getProperty(kEdsPropID_ExposureCompensation);
		if(err == EDS_ERR_OK) err = getProperty(kEdsPropID_ImageQuality);
		
		return err;
	}

	//Acquisition of the property size
	if(err == EDS_ERR_OK)
	{
		err = EdsGetPropertySize( m_pModel->getCameraObject(),
								  propertyID,
								  0,
								  &dataType,
								  &dataSize );
	}

	if(err == EDS_ERR_OK)
	{
		
		if(dataType == kEdsDataType_UInt32)
		{
			EdsUInt32 data;

			//Acquisition of the property
			err = EdsGetPropertyData( m_pModel->getCameraObject(),
									propertyID,
									0,
									dataSize,
									&data );

			//Acquired property value is set
			if(err == EDS_ERR_OK)
			{
				m_pModel->setPropertyUInt32(propertyID, data);
			}
		}
		
		if(dataType == kEdsDataType_String)
		{
			
			EdsChar str[EDS_MAX_NAME];
			//Acquisition of the property
			err = EdsGetPropertyData( m_pModel->getCameraObject(),
									propertyID,
									0,
									dataSize,
									str );

			//Acquired property value is set
			if(err == EDS_ERR_OK)
			{
				m_pModel->setPropertyString(propertyID, str);
			}			
		}
		if(dataType == kEdsDataType_FocusInfo)
		{
			EdsFocusInfo focusInfo;
			//Acquisition of the property
			err = EdsGetPropertyData( m_pModel->getCameraObject(),
									propertyID,
									0,
									dataSize,
									&focusInfo );

			//Acquired property value is set
			if(err == EDS_ERR_OK)
			{
				m_pModel->setFocusInfo(focusInfo);
			}		
		}
	}


	//Update notification
	if(err == EDS_ERR_OK)
	{
//		CameraEvent e("PropertyChanged", &propertyID);
//		m_pModel->notifyObservers(&e);
	}

	return err;
}

// Get Property Desc Command
////////////////////////////////////////////////////////////////////
int CCameraCanonEos::GetPropertyDescCommand( EdsPropertyID propertyID )
{
	EdsError err = EDS_ERR_OK;
	bool	 locked = false;
	
	// For cameras earlier than the 30D , the UI must be locked before commands are reissued
	if( m_pModel->isLegacy() )
	{
		err = EdsSendStatusCommand( m_pModel->getCameraObject(), kEdsCameraStatusCommand_UILock, 0);
	
		if(err == EDS_ERR_OK)
		{
			locked = true;
		}		
	}		
	
	//Get property
	if(err == EDS_ERR_OK)
	{
		err = getPropertyDesc( propertyID );
	}

	//It releases it when locked
	if(locked)
	{
		EdsSendStatusCommand(m_pModel->getCameraObject(), kEdsCameraStatusCommand_UIUnLock, 0);
	}

	//Notification of error
	if(err != EDS_ERR_OK)
	{
		// It retries it at device busy
		if((err & EDS_ERRORID_MASK) == EDS_ERR_DEVICE_BUSY)
		{
//			CameraEvent e("DeviceBusy");
//			m_pModel->notifyObservers(&e);
			return false;
		}

//		CameraEvent e("error", &err);
//		m_pModel->notifyObservers(&e);
	}

	return true;

}	

// Get Property Desc 
////////////////////////////////////////////////////////////////////
EdsError CCameraCanonEos::getPropertyDesc(EdsPropertyID propertyID)
{
	EdsError  err = EDS_ERR_OK;
	EdsPropertyDesc	 propertyDesc = {0};
	
	if(propertyID == kEdsPropID_Unknown)
	{
		//If unknown is returned for the property ID , the required property must be retrieved again
		if(err == EDS_ERR_OK) err = getPropertyDesc(kEdsPropID_AEMode);
		if(err == EDS_ERR_OK) err = getPropertyDesc(kEdsPropID_Tv);
		if(err == EDS_ERR_OK) err = getPropertyDesc(kEdsPropID_Av);
		if(err == EDS_ERR_OK) err = getPropertyDesc(kEdsPropID_ISOSpeed);
		if(err == EDS_ERR_OK) err = getPropertyDesc(kEdsPropID_MeteringMode);
		if(err == EDS_ERR_OK) err = getPropertyDesc(kEdsPropID_ExposureCompensation);			
		if(err == EDS_ERR_OK) err = getPropertyDesc(kEdsPropID_ImageQuality);			
		
		return err;
	}		

	//Acquisition of value list that can be set
	if(err == EDS_ERR_OK)
	{
		err = EdsGetPropertyDesc( m_pModel->getCameraObject(),
								propertyID,
								&propertyDesc);
	}

	//The value list that can be the acquired setting it is set		
	if(err == EDS_ERR_OK)
	{
		m_pModel->setPropertyDesc(propertyID , &propertyDesc);
	}

	//Update notification
	if(err == EDS_ERR_OK)
	{
//		CameraEvent e("PropertyDescChanged", &propertyID);
//		m_pModel->notifyObservers(&e);
	}

	return( err );
}

// set property command
////////////////////////////////////////////////////////////////////
int CCameraCanonEos::SetPropertyCommand( EdsPropertyID propertyID, EdsUInt32 data )
{
	EdsError err = EDS_ERR_OK;
	bool locked = false;

	// For cameras earlier than the 30D , the UI must be locked before commands are reissued
	if( m_pModel->isLegacy() )
	{
		err = EdsSendStatusCommand( m_pModel->getCameraObject(), kEdsCameraStatusCommand_UILock, 0 );
	
		if( err == EDS_ERR_OK ) locked = true;
	}

	// Set property
	if(err == EDS_ERR_OK)
	{		
		err = EdsSetPropertyData( m_pModel->getCameraObject(), propertyID,
									0, sizeof(data), (EdsVoid *) &data );
	}

	//It releases it when locked
	if(locked)
	{
		EdsSendStatusCommand( m_pModel->getCameraObject(), kEdsCameraStatusCommand_UIUnLock, 0 );
	}


	//Notification of error
	if( err != EDS_ERR_OK )
	{
		// It retries it at device busy
		if( err == EDS_ERR_DEVICE_BUSY )
		{
//			CameraEvent e("DeviceBusy");
//			_model->notifyObservers(&e);
			return( false );
		}

//		CameraEvent e("error", &err);
//		_model->notifyObservers(&e);
	}

	return( true );
}

// take a picture
////////////////////////////////////////////////////////////////////
int CCameraCanonEos::TakePictureCommand( const wxString& strPath )
{
	// check if specific path 
	if( !strPath.IsEmpty() ) m_strDownloadPath = strPath;

	EdsError err = EDS_ERR_OK;
	bool	 locked = false;

	m_pCamera->m_pDCameraWorker->SetWaitDialogMsg( wxT("Start shoting photo...") );

	// For cameras earlier than the 30D , the UI must be locked before commands are reissued
	if( m_pModel->isLegacy())
	{
		err = EdsSendStatusCommand( m_pModel->getCameraObject(), kEdsCameraStatusCommand_UILock, 0 );
		if( err == EDS_ERR_OK ) locked = true;
	}
	
	//Taking a picture
	err = EdsSendCommand(m_pModel->getCameraObject(), kEdsCameraCommand_TakePicture, 0);

	//It releases it when locked
	if(locked) err = EdsSendStatusCommand(m_pModel->getCameraObject(), kEdsCameraStatusCommand_UIUnLock, 0);

	//Notification of error
	if(err != EDS_ERR_OK)
	{
		// It retries it at device busy
		if(err == EDS_ERR_DEVICE_BUSY)
		{
//			CameraEvent e("DeviceBusy");
//			m_pModel->notifyObservers(&e);
			return true;
		}
		
//		CameraEvent e("error", &err); 
//		m_pModel->notifyObservers(&e);
	}

	return( 1 );
}

// Download Command
////////////////////////////////////////////////////////////////////
int CCameraCanonEos::DownloadCommand( EdsDirectoryItemRef dirItem ) 
{
	EdsError err = EDS_ERR_OK;
	EdsStreamRef stream = NULL;
	wxString strTmp;
	wxString strFile=wxT("");

	//Acquisition of the downloaded image information
	EdsDirectoryItemInfo dirItemInfo;
	err = EdsGetDirectoryItemInfo( dirItem, &dirItemInfo);

	// Forwarding beginning notification	
	if(err == EDS_ERR_OK)
	{
//		CameraEvent e("DownloadStart");
//		_model->notifyObservers(&e);
	}

	//Make the file stream at the forwarding destination
	if( err == EDS_ERR_OK )
	{	
		wxString strFName(dirItemInfo.szFileName,wxConvUTF8);
		wxFileName strT( strFName );

		strTmp.Printf( wxT("%s/%d.%s"), m_strDownloadPath, GetUniqTimeId( ), strT.GetExt() );
		// tocheck: conversion here to ascii 
		err = EdsCreateFileStream( strTmp.ToAscii(), kEdsFileCreateDisposition_CreateAlways, kEdsAccess_ReadWrite, &stream );
		// chek if this is a thumbnail
		if( strT.GetExt().CmpNoCase( wxT("THM") ) != 0 ) strFile = strTmp;

	}	

	//Set Progress
	if( err == EDS_ERR_OK )
	{
		err = EdsSetProgressCallback(stream, DownloadProgressFunc, kEdsProgressOption_Periodically, (EdsVoid *) this );
	}


	//Download image
	if( err == EDS_ERR_OK )
	{
		m_pCamera->m_pDCameraWorker->SetWaitDialogMsg( wxT("Downloading Image...") );
		err = EdsDownload( dirItem, dirItemInfo.size, stream);
	}

	//Forwarding completion
	if( err == EDS_ERR_OK )
	{
		err = EdsDownloadComplete( dirItem);
	}

	//Release Item
	if( dirItem != NULL )
	{
		err = EdsRelease( dirItem );
		dirItem = NULL;
	}

	//Release stream
	if( stream != NULL )
	{
		err = EdsRelease(stream);
		stream = NULL;
	}		
	
	// Forwarding completion notification
	if( err == EDS_ERR_OK )
	{
		if( strFile.Length() ) m_pCamera->AddImageFrame( strFile );
//		CameraEvent e("DownloadComplete", &err);
//		_model->notifyObservers(&e);
	}

	//Notification of error
	if( err != EDS_ERR_OK)
	{
//		CameraEvent e("error", &err);
//		_model->notifyObservers(&e);
	}

	return true;
}

////////////////////////////////////////////////////////////////////
EdsError EDSCALLBACK CCameraCanonEos::DownloadProgressFunc( EdsUInt32 inPercent, EdsVoid* inContext, EdsBool* outCancel )
{
	// set progress
	CCameraCanonEos* mec = (CCameraCanonEos*) inContext;
	mec->m_pCamera->m_pDCameraWorker->SetWaitDialogProgress( (int) inPercent );

//	Command *command = (Command *)inContext;
//	CameraEvent e("ProgressReport", &inPercent);
//	command->getCameraModel()->notifyObservers(&e);
	return EDS_ERR_OK;
}

// shut down
////////////////////////////////////////////////////////////////////
void CCameraCanonEos::ShutDown( )
{
	// just notify here that camera has been shut down ???
	m_pCamera->SendDisconnect();
}
