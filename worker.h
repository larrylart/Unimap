////////////////////////////////////////////////////////////////////
// Package:		worker definition
// File:		worker.h
// Purpose:		manage software hardware powercycle in sync
//
// Created by:	Larry Lart on 09/12/2006
// Updated by:	Larry Lart on 09/02/2010
//
// Copyright:	(c) 2006-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _WORKER_H
#define _WORKER_H

// c++
#include <iostream>
#include <vector>
using namespace std;

// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/thread.h"
#include <wx/process.h>

// include other local headers
#include "logger/logger.h"

// default loop delay values
#define THREAD_LOOP_CHECK_DELAY		20000
// define actions
#define THREAD_ACTION_NONE				0

// external classes
class CConfigMain;
class CGUIFrame;
class CUnimap;
class CWaitDialog;

/*
// class:	wxLoggerEvent
/////////////////////////////////////////////////
class wxLoggerEvent: public wxNotifyEvent
{
public:
    wxLoggerEvent( wxEventType commandType = wxEVT_NULL, int id = 0 );

    // required for sending with wxPostEvent()
    wxEvent* Clone();

private:
    char	m_strMsg[1000];
};

DECLARE_EVENT_MACRO( wxEVT_LOGGER_LOG, -1 )

typedef void (wxEvtHandler::*wxPlotEventFunction)(wxPlotEvent&);

#define EVT_PLOT(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_LOGGER_LOG, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),

*/

//////////////////////////////////
// struct :: action command
typedef struct
{
	int id;
	wxEvtHandler* handler;
	CWaitDialog* waitdlg;
	wxString vectStr[20];
	long vectInt[20];
	double vectFloat[20];
	// pointers to objects
	void*	vectObj[20];

} DefCmdAction;

//////////////////////////////////
// struct :: timed action command
typedef struct
{
	int id;

	// time type 0 = interval, 1=run once at start time, 2=run from start time at time interval
	int time_type;
	// time interval in seconds
	double time_int;
	// start time in seconds - unix time stamp
	double start_time;
	// end time to run
	double end_time;
	// count how many time to run
	long run_count;
	// last time it run
	double last_time;

	// data
	wxEvtHandler* handler;
	wxString vectStr[10];
	long vectInt[10];
	double vectFloat[10];

} DefCmdTimedAction;

DECLARE_EVENT_TYPE( wxEVT_CUSTOM_MESSAGE_EVENT, -1 )

//////////////////////////////////////////////////
// class:	CWorker
// todo: do i need the frame reference here ?
/////////////////////////////////////////////////
class CWorker : public wxThread
{
// methods
public:
	CWorker( CGUIFrame *pFrame, CLogger *pLogger );
	~CWorker();

	virtual void SetConfig( ){ }

	void SetAction( DefCmdAction& rAction );
	// time
	double GetTime( void );

	// timed actions 
	void SetTimedAction( DefCmdTimedAction& rAction ){ m_vectTimedActionQueue.push_back( rAction ); }
	int RemoveTimedAction( int nId );

	// logging
	void Log( const wxString& strMsg, int nGUILoggingStyle = GUI_LOGGER_DEFAULT_STYLE );
	void DisplayNotification( const wxString& strMsg, int nType );
	// wait dialog calls
	void CloseWaitDialog( );
	// send event with a type
	void SendEventType( wxEvtHandler* pHandler, int nEventType, int bDelay=1 );

	///////////////
	// wait dialog methods
	void SetWaitDialogUpdate( );
	void SetWaitDialogTitle( const wxString& strTitle );
	void SetWaitDialogMsg( const wxString& strMsg );
	void SetWaitDialogProgress( int nPStep );

	virtual void Entry_Init(){ return; };
	// thread entry point
	virtual void *Entry();
	virtual void OnExit();

	void IsChanged( int nState );

	// runtime check handlers
	void ManageCommands();
	virtual int CheckForCommands( ){ return(0); }
	virtual int CheckForTimedCommands( ){ return(0); }

/////////
// data
public:

	CLogger*		m_pLogger;	
	CUnimap*		m_pUnimap;
    CGUIFrame*		m_pFrame;

	wxEvtHandler*	m_evtWaitDialog;

	// custom :: objects i need to manage

	// action command 
	int m_nActionCmd;
	vector <DefCmdAction> m_vectActionQueue;
	// timed actions
	vector <DefCmdTimedAction> m_vectTimedActionQueue;
	// manage commands vars
	int m_nDeleteTimedActionId;

	// other flags
	int m_bIsWaitDialogOpen;
	// flag to stop current task
	int m_bCurrentTaskSetToBreak;

	int m_nLoopCheckDelay;
	// thread loop flags
	int m_bIsPaused;
	int m_bPauseLoop;
	int m_bChanged;
	int m_bIsJustStarted;

	bool	m_bLife;
	int		m_bIsProcessing;
	int		m_bIsExit;

	double m_nCurrentTime;

	// add here: action flags

	// process syncronizer/locker
	wxMutex *m_pMutex;

// private data
private:

};

#endif


