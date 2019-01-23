////////////////////////////////////////////////////////////////////                                                                            *
//   PROJECT : EOS Digital Software Development Kit EDSDK 
//      NAME : CameraEventListener.h
//
// Description: This is the Sample code to show the usage of EDSDK.
//
//      Written and developed by Camera Design Dept.53
//    Copyright Canon Inc. 2006-2008 All Rights Reserved 
//
////////////////////////////////////////////////////////////////////

#pragma once 

#include "EDSDK.h"
#include "../camera_canoneos.h"
//#include "CameraController.h"

class CameraEventListener
{
public:

	static EdsError EDSCALLBACK  handleObjectEvent(
						EdsUInt32			inEvent,
						EdsBaseRef			inRef,
						EdsVoid*			inContext )
	{
		CCameraCanonEos* pCamera = (CCameraCanonEos*) inContext;

		switch(inEvent)
		{
			case kEdsObjectEvent_DirItemRequestTransfer:
			{
				pCamera->DownloadCommand( inRef );
				break;
			}
		
			default:
			{
				//Object without the necessity is released
				if( inRef != NULL )
				{ 
					EdsRelease(inRef);
				}
				break;
			}
		}

		return( EDS_ERR_OK );
	}	

	static EdsError EDSCALLBACK handlePropertyEvent(
						EdsUInt32			inEvent,
						EdsUInt32			inPropertyID,
						EdsUInt32			inParam, 
						EdsVoid*			inContext )
	{
		CCameraCanonEos* pCamera = (CCameraCanonEos*) inContext;

		switch(inEvent)
		{
			case kEdsPropertyEvent_PropertyChanged:
					pCamera->GetPropertyCommand( inPropertyID );
					break;

			case kEdsPropertyEvent_PropertyDescChanged:
					pCamera->GetPropertyDescCommand( inPropertyID );
					break;
		}

		return( EDS_ERR_OK );
	}	

	static EdsError EDSCALLBACK  handleStateEvent(
						EdsUInt32			inEvent,
						EdsUInt32			inParam, 
						EdsVoid *			inContext )
	{

		CCameraCanonEos* pCamera = (CCameraCanonEos*) inContext;

		switch(inEvent)
		{
			case kEdsStateEvent_Shutdown:
					pCamera->ShutDown( );
					break;
		}

		return( EDS_ERR_OK );
	}	

};
