
#ifndef _DCAMERA_WORKER_H
#define _DCAMERA_WORKER_H

// local headers
#include "../worker.h"

//////////////
// define actions - start with 1 0 = none already defined
#define THREAD_DCAMERA_ACTION_CONNECT			1
#define THREAD_DCAMERA_ACTION_DISCONNECT		2
#define THREAD_DCAMERA_ACTION_TAKE_PHOTO		3
#define THREAD_DCAMERA_ACTION_NEW_IMAGE			4
#define THREAD_DCAMERA_ACTION_SET_PROPERTY		5

// external classes
class CGUIFrame;
class CLogger;
class CCamera;
class CCameraCanonEos;

// class:	CDCameraWorker
/////////////////////////////////////////////////
class CDCameraWorker : public CWorker
{
// methods
public:
	CDCameraWorker( CGUIFrame *pFrame, CLogger *pLogger );
	~CDCameraWorker();

	void SetConfig( CCamera* pCamera );

	// runtime check handlers
	int CheckForCommands( );

	// digital camera commands
	void TakePhoto( int nDevId, const wxString& strPath );
	void SetProperty( unsigned long cmd_id, unsigned long cmd_arg );

// data
public:
	CCamera* m_pCamera;

	// gui stuff ??
	CGUIFrame* m_pFrame;

};

#endif

