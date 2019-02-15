////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _TELESCOPE_WORKER_H
#define _TELESCOPE_WORKER_H

// local headers
#include "../worker.h"

//////////////
// define actions - start with 1 0 = none already defined
#define THREAD_TELESCOPE_ACTION_CONNECT			1
#define THREAD_TELESCOPE_ACTION_DISCONNECT		2
#define THREAD_TELESCOPE_ACTION_MOVE_EAST		3
#define THREAD_TELESCOPE_ACTION_MOVE_WEST		4
#define THREAD_TELESCOPE_ACTION_MOVE_NORTH		5
#define THREAD_TELESCOPE_ACTION_MOVE_SOUTH		6
#define THREAD_TELESCOPE_ACTION_MOVE_STOP		7
#define THREAD_TELESCOPE_ACTION_GUIDE			10
#define THREAD_TELESCOPE_ACTION_GOTO			11
// focuser
#define THREAD_TELESCOPE_ACTION_START_FOCUS_IN	50
#define THREAD_TELESCOPE_ACTION_START_FOCUS_OUT	51
#define THREAD_TELESCOPE_ACTION_STOP_FOCUS		52

// external classes
//class CGUIFrame;
//class CLogger;
class CTelescope;
class CFocuser;

// class:	CTelescopeWorker
/////////////////////////////////////////////////
class CTelescopeWorker : public CWorker
{
// methods
public:
	CTelescopeWorker( CGUIFrame *pFrame, CLogger *pLogger );
	~CTelescopeWorker();

	void SetConfig( CTelescope* pTelescope, CFocuser* pFocuser );

	// runtime check handlers
	int CheckForCommands( );

	// telescope commands
	void StartMovingEast();
	void StartMovingWest();
	void StartMovingNorth();
	void StartMovingSouth();
	void StopSlew();

	// focuser command
	void StartFocusIn();
	void StartFocusOut();
	void StopFocus();

// data
public:
	CTelescope* m_pTelescope;
	CFocuser* m_pFocuser;

	// gui stuff ??
//	CGUIFrame* m_pFrame;

	// flags
	int m_bUpdateTelescopeInfo;

};

#endif

