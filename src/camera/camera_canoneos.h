////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CAMERA_CANON_EOS_H
#define _CAMERA_CANON_EOS_H

// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// canon eos headers
#include "canoneos/CameraModel.h"
//#include "canoneos/Processor.h"

// local headers
#include "camera_driver.h"

// external classes
class CCamera;

// class:	CCameraCanonEos
/////////////////////////////////////////////////
class CCameraCanonEos : public CCameraDriver
{
// methods
public:
	CCameraCanonEos( CCamera* pCamera );
	~CCameraCanonEos();

	int Init( );
	int Connect( );
	int Close( );
	// camera commands
	int OpenSessionCommand( );
	int GetPropertyCommand( EdsPropertyID propertyID );
	int GetPropertyDescCommand( EdsPropertyID propertyID );
	int SetPropertyCommand( EdsPropertyID propertyID, EdsUInt32 data );
	int TakePictureCommand( const wxString& strPath=wxT("") );
	int DownloadCommand( EdsDirectoryItemRef dirItem );
	void ShutDown( );

	// progress call back
	static EdsError EDSCALLBACK DownloadProgressFunc( EdsUInt32 inPercent, EdsVoid* inContext, EdsBool* outCancel );

// data
public:
	CCamera* m_pCamera;

	CameraModel* m_pModel;
	wxString m_strDownloadPath;
	int m_isSDKLoaded;
	EdsCameraRef camera;

	// flags
	int m_bIsConnected;

// private
private:
	int getProperty( EdsPropertyID propertyID );
	EdsError getPropertyDesc(EdsPropertyID propertyID);

};

#endif

