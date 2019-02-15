/******************************************************************************
*                                                                             *
*   PROJECT : EOS Digital Software Development Kit EDSDK                      *
*      NAME : CameraController.h	                                          *
*                                                                             *
*   Description: This is the Sample code to show the usage of EDSDK.          *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
*   Written and developed by Camera Design Dept.53                            *
*   Copyright Canon Inc. 2006-2008 All Rights Reserved                        *
*                                                                             *
*******************************************************************************/

#pragma once

#include "EDSDK.h"
#include "CameraModel.h"
#include "Processor.h"

#include "ActionListener.h"
#include "ActionEvent.h"

#include "OpenSessionCommand.h"
#include "CloseSessionCommand.h"
#include "SaveSettingCommand.h"
#include "TakePictureCommand.h"
#include "DownloadCommand.h"
#include "GetPropertyCommand.h"
#include "GetPropertyDescCommand.h"
#include "SetPropertyCommand.h"
#include "SetCapacityCommand.h"
#include "NotifyCommand.h"

#include "StartEvfCommand.h"
#include "EndEvfCommand.h"
#include "DownloadEvfCommand.h"
#include "DriveLensCommand.h"
#include "PressShutterButtonCommand.h"
#include "GetPropertyCommand.h"
#include "DoEvfAFCommand.h"

class CameraController : public ActionListener
{

protected:
	// Camera model
	CameraModel* _model;
	
	// Command processing
	Processor _processor;

public:
	// Constructor
	CameraController(): _model(){}

	// Destoracta
	virtual ~CameraController(){}

	void setCameraModel(CameraModel* model) {_model = model;}

	//Execution beginning
	void run()
	{
		_processor.start();

		//The communication with the camera begins
		StoreAsync(new OpenSessionCommand(_model));

		//It is necessary to acquire the property information that cannot acquire in sending OpenSessionCommand automatically by manual operation.
		StoreAsync(new GetPropertyCommand(_model, kEdsPropID_ProductName));
	}

public:
	
	void actionPerformed(const ActionEvent& event)
	{
		std::string command = event.getActionCommand();


		if ( command == "opensession" )
		{
			StoreAsync(new OpenSessionCommand(_model));
		}

		if ( command == "set_AEMode" )
		{
			StoreAsync(new SetPropertyCommand<EdsUInt32>(_model, kEdsPropID_AEMode, *static_cast<EdsUInt32*>(event.getArg())));			
		}

		if ( command == "set_Av" )
		{
			StoreAsync(new SetPropertyCommand<EdsUInt32>(_model, kEdsPropID_Av, *static_cast<EdsUInt32*>(event.getArg())));			
		}

		if ( command == "set_Tv" )
		{
			StoreAsync(new SetPropertyCommand<EdsUInt32>(_model, kEdsPropID_Tv, *static_cast<EdsUInt32*>(event.getArg())));			
		}

		if ( command == "set_ISOSpeed" )
		{
			StoreAsync(new SetPropertyCommand<EdsUInt32>(_model, kEdsPropID_ISOSpeed, *static_cast<EdsUInt32*>(event.getArg())));			
		}

		if ( command == "set_MeteringMode" )
		{
			StoreAsync(new SetPropertyCommand<EdsUInt32>(_model, kEdsPropID_MeteringMode, *static_cast<EdsUInt32*>(event.getArg())));			
		}

		if ( command == "set_ExposureCompensation" )
		{
			StoreAsync(new SetPropertyCommand<EdsUInt32>(_model, kEdsPropID_ExposureCompensation, *static_cast<EdsUInt32*>(event.getArg())));			
		}
		
		if ( command == "set_ImageQuality" )
		{
			StoreAsync(new SetPropertyCommand<EdsUInt32>(_model, kEdsPropID_ImageQuality, *static_cast<EdsUInt32*>(event.getArg())));			
		}

		if ( command == "pressingHalfway" )
		{
			StoreAsync(new PressShutterButtonCommand(_model, kEdsCameraCommand_ShutterButton_Halfway));			
		}
		if ( command == "pressingCompletely" )
		{
			StoreAsync(new PressShutterButtonCommand(_model, kEdsCameraCommand_ShutterButton_Completely));			
		}
		if ( command == "pressingOff" )
		{
			StoreAsync(new PressShutterButtonCommand(_model, kEdsCameraCommand_ShutterButton_OFF));			
		}

		if ( command == "TakePicture" )
		{
			StoreAsync(new TakePictureCommand(_model));
		}

		if ( command == "set_Capacity" )
		{
			//EdsCapacity capacity = {0x7FFFFFFF, 0x1000, 1};
			StoreAsync(new SetCapacityCommand(_model, *static_cast<EdsCapacity*>(event.getArg())));
		}

		if ( command == "get_Property" )
		{ 
			StoreAsync(new GetPropertyCommand(_model, *static_cast<EdsUInt32*>(event.getArg())));
		}

		if( command == "get_PropertyDesc" ) 
		{
			StoreAsync(new GetPropertyDescCommand(_model, *static_cast<EdsUInt32*>(event.getArg())));
		}

		if ( command == "download" )
		{
			StoreAsync(new DownloadCommand(_model, static_cast<EdsBaseRef>(event.getArg())));
		}

		if( command == "shutDown")
		{
			StoreAsync(new NotifyCommand(_model, "shutDown"));
		}

		if( command == "closing")
		{
			_processor.setCloseCommand(new CloseSessionCommand(_model));
			_processor.stop();
			_processor.join();		
		}

/////////////////////////////
//		EVF Control
/////////////////////////////

		if ( command == "startEVF" )
		{
			StoreAsync(new StartEvfCommand(_model));
		}

		if ( command == "endEVF" )
		{
			StoreAsync(new EndEvfCommand(_model));
		}	
		
		if ( command == "downloadEVF" )
		{
			StoreAsync(new DownloadEvfCommand(_model));
		}

		if ( command == "focus_Near3" || command == "focus_Near2" || command == "focus_Near1" ||
			 command == "focus_Far3"  || command == "focus_Far2"  || command == "focus_Far1")
		{
			EdsUInt32 driveLens; 
			if ( command == "focus_Near3") driveLens = kEdsEvfDriveLens_Near3;
			if ( command == "focus_Near2") driveLens = kEdsEvfDriveLens_Near2;
			if ( command == "focus_Near1") driveLens = kEdsEvfDriveLens_Near1;
			if ( command == "focus_Far3") driveLens = kEdsEvfDriveLens_Far3;
			if ( command == "focus_Far2") driveLens = kEdsEvfDriveLens_Far2;
			if ( command == "focus_Far1") driveLens = kEdsEvfDriveLens_Far1;
			StoreAsync(new DriveLensCommand(_model, driveLens));			
		}
	
		if ( command == "focus_Up" || command == "focus_Down")
		{
			const int stepY = 128;

			EdsPoint point = _model->getEvfZoomPosition();
			if ( command == "focus_Up")
			{
				point.y -= stepY;
				if(point.y < 0) point.y = 0;
			}
			if ( command == "focus_Down") point.y += stepY;
			StoreAsync(new SetPropertyCommand<EdsPoint>(_model, kEdsPropID_Evf_ZoomPosition, point));			
		}
		

		if ( command == "focus_Left" || command == "focus_Right")
		{
			const int stepX = 128;

			EdsPoint point = _model->getEvfZoomPosition();
			if(command == "focus_Left")
			{
				point.x -= stepX;
				if(point.x < 0) point.x = 0;
			}
			if(command == "focus_Right")point.x += stepX;
			StoreAsync(new SetPropertyCommand<EdsPoint>(_model, kEdsPropID_Evf_ZoomPosition, point));			
		}

		if ( command == "zoom_Fit")
		{
			StoreAsync(new SetPropertyCommand<EdsUInt32>(_model, kEdsPropID_Evf_Zoom, kEdsEvfZoom_Fit));		
		}

		if( command == "zoom_Zoom")
		{
			StoreAsync(new SetPropertyCommand<EdsUInt32>(_model, kEdsPropID_Evf_Zoom, kEdsEvfZoom_x5));		
		}

		if (command == "set_EvfAFMode")
		{
			StoreAsync(new SetPropertyCommand<EdsUInt32>(_model, kEdsPropID_Evf_AFMode, *static_cast<EdsUInt32*>(event.getArg())));			
		}
		if (command == "evfAFOn")
		{
			StoreAsync(new DoEvfAFCommand(_model, kEdsCameraCommand_EvfAf_ON));
		}
		if (command == "evfAFOff")
		{
			StoreAsync(new DoEvfAFCommand(_model, kEdsCameraCommand_EvfAf_OFF));			
		}

	}


public:
	//The command is received
	void StoreAsync( Command *command )
	{
		if ( command != NULL )
		{
			_processor.enqueue( command );
		}
	}


};
