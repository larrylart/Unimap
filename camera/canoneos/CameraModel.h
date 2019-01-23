/******************************************************************************
*                                                                             *
*   PROJECT : EOS Digital Software Development Kit EDSDK                      *
*      NAME : CameraModel.h	                                                  *
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

//#include "Observer.h"

class CameraModel //: public Observable
{
protected:
	EdsCameraRef _camera;

	//Count of UIlock
	int		_lockCount;

	// Model name
	EdsChar  _modelName[EDS_MAX_NAME];

	// Taking a picture parameter
	EdsUInt32 _AEMode;
	EdsUInt32 _Av;
	EdsUInt32 _Tv;
	EdsUInt32 _Iso;
	EdsUInt32 _MeteringMode;
	EdsUInt32 _ExposureCompensation;
	EdsUInt32 _ImageQuality;
	EdsUInt32 _AvailableShot;
	EdsUInt32 _evfMode;
	EdsUInt32 _evfOutputDevice;
	EdsUInt32 _evfDepthOfFieldPreview;
	EdsUInt32 _evfZoom;
	EdsPoint  _evfZoomPosition;
	EdsUInt32 _evfAFMode;

	EdsFocusInfo _focusInfo;

	// List of value in which taking a picture parameter can be set
	EdsPropertyDesc _AEModeDesc;
	EdsPropertyDesc _AvDesc;
	EdsPropertyDesc _TvDesc;
	EdsPropertyDesc _IsoDesc;
	EdsPropertyDesc _MeteringModeDesc;
	EdsPropertyDesc _ExposureCompensationDesc;
	EdsPropertyDesc _ImageQualityDesc;
	EdsPropertyDesc _evfAFModeDesc;

public:
	// Constructor
	CameraModel(EdsCameraRef camera):_lockCount(0),_camera(camera){memset(&_focusInfo, 0, sizeof(_focusInfo));} 

	//Acquisition of Camera Object
	EdsCameraRef getCameraObject() const {return _camera;}


//Property
public:
	// Taking a picture parameter
	void setAEMode(EdsUInt32 value )				{ _AEMode = value;}
	void setTv( EdsUInt32 value )					{ _Tv = value;}
	void setAv( EdsUInt32 value )					{ _Av = value;}
	void setIso( EdsUInt32 value )					{ _Iso = value; }
	void setMeteringMode( EdsUInt32 value )			{ _MeteringMode = value; }
	void setExposureCompensation( EdsUInt32 value)	{ _ExposureCompensation = value; }
	void setImageQuality( EdsUInt32 value)			{ _ImageQuality = value; }
	void setEvfMode( EdsUInt32 value)				{ _evfMode = value; }
	void setEvfOutputDevice( EdsUInt32 value)		{ _evfOutputDevice = value; }
	void setEvfDepthOfFieldPreview( EdsUInt32 value){ _evfDepthOfFieldPreview = value; }
	void setEvfZoom( EdsUInt32 value)				{ _evfZoom = value; }
	void setEvfZoomPosition( EdsPoint value)		{ _evfZoomPosition = value; }
	void setModelName(EdsChar *modelName)			{ strcpy(_modelName, modelName); }
	void setEvfAFMode( EdsUInt32 value)				{ _evfAFMode = value; }
	void setFocusInfo( EdsFocusInfo value)				{ _focusInfo = value; }

	// Taking a picture parameter
	EdsUInt32 getAEMode() const					{ return _AEMode; }
	EdsUInt32 getTv() const						{ return _Tv; }
	EdsUInt32 getAv() const						{ return _Av; }
	EdsUInt32 getIso() const					{ return _Iso; }
	EdsUInt32 getMeteringMode() const			{ return _MeteringMode; }
	EdsUInt32 getExposureCompensation() const	{ return _ExposureCompensation; }
	EdsUInt32 getImageQuality() const			{ return _ImageQuality; }
	EdsUInt32 getEvfMode() const				{ return _evfMode; }
	EdsUInt32 getEvfOutputDevice() const		{ return _evfOutputDevice; }
	EdsUInt32 getEvfDepthOfFieldPreview() const	{ return _evfDepthOfFieldPreview; }
	EdsUInt32 getEvfZoom() const				{ return _evfZoom; }	
	EdsPoint  getEvfZoomPosition() const		{ return _evfZoomPosition; }	
	EdsUInt32 getEvfAFMode() const				{ return _evfAFMode; }
	EdsChar *getModelName()						{ return _modelName; }
	EdsFocusInfo getFocusInfo()const			{ return _focusInfo; }

	//List of value in which taking a picture parameter can be set
	EdsPropertyDesc getAEModeDesc() const					{ return _AEModeDesc;}
	EdsPropertyDesc getAvDesc() const						{ return _AvDesc;}
	EdsPropertyDesc getTvDesc()	const						{ return _TvDesc;}
	EdsPropertyDesc getIsoDesc()	const					{ return _IsoDesc;}
	EdsPropertyDesc getMeteringModeDesc()	const			{ return _MeteringModeDesc;}
	EdsPropertyDesc getExposureCompensationDesc()	const	{ return _ExposureCompensationDesc;}
	EdsPropertyDesc getImageQualityDesc()	const			{ return _ImageQualityDesc;}
	EdsPropertyDesc getEvfAFModeDesc()	const				{ return _evfAFModeDesc;}

	//List of value in which taking a picture parameter can be set
	void setAEModeDesc(const EdsPropertyDesc* desc)					{_AEModeDesc = *desc;}
	void setAvDesc(const EdsPropertyDesc* desc)						{_AvDesc = *desc;}
	void setTvDesc(const EdsPropertyDesc* desc)						{_TvDesc = *desc;}
	void setIsoDesc(const EdsPropertyDesc* desc)					{_IsoDesc = *desc;}
	void setMeteringModeDesc(const EdsPropertyDesc* desc)			{_MeteringModeDesc = *desc;}
	void setExposureCompensationDesc(const EdsPropertyDesc* desc)	{_ExposureCompensationDesc = *desc;}
	void setImageQualityDesc(const EdsPropertyDesc* desc)			{_ImageQualityDesc = *desc;}
	void setEvfAFModeDesc(const EdsPropertyDesc* desc)			{_evfAFModeDesc = *desc;}

public:
	//Setting of taking a picture parameter(UInt32)
	void setPropertyUInt32(EdsUInt32 propertyID, EdsUInt32 value)	
	{
		switch(propertyID) 
		{
			case kEdsPropID_AEMode:					setAEMode(value);					break;
			case kEdsPropID_Tv:						setTv(value);						break;		               
			case kEdsPropID_Av:						setAv(value);						break;           	  
			case kEdsPropID_ISOSpeed:				setIso(value);						break;       
			case kEdsPropID_MeteringMode:			setMeteringMode(value);				break;       
			case kEdsPropID_ExposureCompensation:	setExposureCompensation(value);		break;
			case kEdsPropID_ImageQuality:			setImageQuality(value);				break;
			case kEdsPropID_Evf_Mode:				setEvfMode(value);					break;
			case kEdsPropID_Evf_OutputDevice:		setEvfOutputDevice(value);			break;
			case kEdsPropID_Evf_DepthOfFieldPreview:setEvfDepthOfFieldPreview(value);	break;	
			case kEdsPropID_Evf_AFMode:				setEvfAFMode(value);				break;
		}
	}

	//Setting of taking a picture parameter(String)
	void setPropertyString(EdsUInt32 propertyID, EdsChar *str)	
	{	
		switch(propertyID) 
		{
			case kEdsPropID_ProductName:			setModelName(str);					break;
		}
	}

	void setProeprtyFocusInfo(EdsUInt32 propertyID, EdsFocusInfo info)
	{
		switch(propertyID) 
		{
			case kEdsPropID_FocusInfo:				setFocusInfo(info);				break;
		}
	}

	//Setting of value list that can set taking a picture parameter
	void setPropertyDesc(EdsUInt32 propertyID, const EdsPropertyDesc* desc)
	{
		switch(propertyID) 
		{
			case kEdsPropID_AEMode:					setAEModeDesc(desc);				break;
			case kEdsPropID_Tv:						setTvDesc(desc);					break;		               
			case kEdsPropID_Av:						setAvDesc(desc);					break;           	  
			case kEdsPropID_ISOSpeed:				setIsoDesc(desc);					break;       
			case kEdsPropID_MeteringMode:			setMeteringModeDesc(desc);			break;       
			case kEdsPropID_ExposureCompensation:	setExposureCompensationDesc(desc);	break;       
			case kEdsPropID_ImageQuality:			setImageQualityDesc(desc);			break;   
			case kEdsPropID_Evf_AFMode:				setEvfAFModeDesc(desc);				break;
		}	
	}

	//Acquisition of value list that can set taking a picture parameter
	EdsPropertyDesc getPropertyDesc(EdsUInt32 propertyID)	
	{
		EdsPropertyDesc desc = {0};
		switch(propertyID) 
		{
			case kEdsPropID_AEMode:					desc = getAEModeDesc();					break;
			case kEdsPropID_Tv:						desc = getTvDesc();						break;		               
			case kEdsPropID_Av:						desc = getAvDesc();						break;           	  
			case kEdsPropID_ISOSpeed:				desc = getIsoDesc();					break;       
			case kEdsPropID_MeteringMode:			desc = getMeteringModeDesc();			break;       
			case kEdsPropID_ExposureCompensation:	desc = getExposureCompensationDesc();	break;       
			case kEdsPropID_ImageQuality:			desc = getImageQualityDesc();			break;    
			case kEdsPropID_Evf_AFMode:				desc = getEvfAFModeDesc();				break;    
		}
		return desc;
	}

//Access to camera
public:
	virtual bool isLegacy()
	{
		return false;
	}

};

