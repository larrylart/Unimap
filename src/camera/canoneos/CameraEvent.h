/******************************************************************************
*                                                                             *
*   PROJECT : EOS Digital Software Development Kit EDSDK                      *
*      NAME : Event.h	                                                      *
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


class CameraEvent
{
	std::string _event;	
	void* _arg;
public:
	const std::string getEvent() const{ return _event; }
	void* getArg() const{ return _arg; }

	CameraEvent(std::string event,void* arg=0) : 
	 _event(event), _arg(arg) {}

};

