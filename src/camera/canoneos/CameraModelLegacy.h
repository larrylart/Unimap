/******************************************************************************
*                                                                             *
*   PROJECT : EOS Digital Software Development Kit EDSDK                      *
*      NAME : CameraModelLegacy.h	                                          *
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

#include "CameraModel.h"

class CameraModelLegacy : public CameraModel
{

public:
	// Constructor
	CameraModelLegacy(EdsCameraRef camera):CameraModel(camera){} 


	virtual bool isLegacy()
	{
		return true;
	}
};