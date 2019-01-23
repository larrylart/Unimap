
#ifndef _VCAMERA_WORKER_H
#define _VCAMERA_WORKER_H

// local headers
#include "../worker.h"

//////////////
// define actions - start with 1 0 = none already defined
#define THREAD_VCAMERA_ACTION_CONNECT			1
#define THREAD_VCAMERA_ACTION_DISCONNECT		2
#define THREAD_VCAMERA_ACTION_RECORD_MOVIE		3
#define THREAD_VCAMERA_ACTION_NEW_MOVIE			4
#define THREAD_VCAMERA_ACTION_SET_PROPERTY		5

// external classes
class CGUIFrame;
class CLogger;
class CCamera;
class CCameraVideo;

// class:	CDCameraWorker
/////////////////////////////////////////////////
class CVCameraWorker : public CWorker
{
// methods
public:
	CVCameraWorker( CGUIFrame *pFrame, CLogger *pLogger );
	~CVCameraWorker( );

	void SetConfig( CCamera* pCamera );

	// runtime check handlers
	int CheckForCommands( );

	// digital camera commands
	void RecordMovie( int nDevId );
	void SetProperty( unsigned long cmd_id, unsigned long cmd_arg );

// data
public:
	CCamera* m_pCamera;

	// gui stuff ??
	CGUIFrame* m_pFrame;

};

#endif

