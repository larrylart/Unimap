////////////////////////////////////////////////////////////////////
// Package:		worker implementation
// File:		worker.cpp
// Purpose:		base thread worker - cutomized
//
// Created by:	Larry Lart on 20/07/2007
// Updated by:	Larry Lart on 21/08/2009
//
// Copyright:	(c) 2004-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

// system headers
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include "wx/wxprec.h"
#include "wx/thread.h"
#include <wx/string.h>
#include <wx/regex.h>
//#include <wx/sound.h>
#include <wx/process.h>
#include <wx/utils.h>
#include <wx/app.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// custom headers
#ifdef WIN32
#include "util/func.h"
#endif

// local headers
#include "gui/frame.h"
#include "gui/waitdialog.h"
#include "unimap.h"

// main header include
#include "worker.h"

DEFINE_EVENT_TYPE( wxEVT_CUSTOM_MESSAGE_EVENT )

// a global mutex - to clean
//extern wxMutex *s_mutexProtectGlobalData;

//		***** CLASS CWorker *****
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CWorker
// Purpose:	Initialize my worker thread
// Input:	pointer to reference frame
// Output:	nothing	
////////////////////////////////////////////////////////////////////
CWorker::CWorker( CGUIFrame *pFrame, CLogger *pLogger ) : wxThread( wxTHREAD_DETACHED )
{ 
	// logger reference
	m_pLogger = pLogger;
	// get frame refrence
	m_pFrame = pFrame;
	
	m_bIsJustStarted = 1;

	// life default stopped
	m_bLife = 0;

	// default stoped
	m_bCurrentTaskSetToBreak = 0;
	m_bIsPaused = 0;
	m_bPauseLoop = 0;

	m_bChanged = 0;

	m_bIsProcessing = 0;
	m_bIsExit = 0;

	// set action to none
	m_nActionCmd = THREAD_ACTION_NONE;

	// init commands queues
	m_vectActionQueue.clear();
	m_vectTimedActionQueue.clear();

	// set defaults for manage commands
	m_nDeleteTimedActionId = -1;

	m_pMutex = new wxMutex( );

	return;
}
////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CWorker
// Purpose:	do soemthing on exit
// Input:	nothing
// Output:	nothing	
////////////////////////////////////////////////////////////////////
CWorker::~CWorker( )
{
	m_pLogger = NULL;
	if( m_pMutex ) delete( m_pMutex );
}

////////////////////////////////////////////////////////////////////
// Method:	On Exit
// Class:	CWorker
// Purpose:	do soemthing on exit
// Input:	nothing
// Output:	nothing	
////////////////////////////////////////////////////////////////////
void CWorker::OnExit( )
{
	// destroy - clean my place
	delete( m_pMutex );
	m_pMutex = NULL;
	// clear commands queues
	m_vectActionQueue.clear();
	m_vectTimedActionQueue.clear();

//	Exit( 0 );
	return;
}

////////////////////////////////////////////////////////////////////
// Method:	GetTime
// Class:	CWorker
// Purose:	routine to get time accurate - this would be called by 
//			all other components to keep in sync
// Input:	nothing
// Output:	number = accurate time count 
////////////////////////////////////////////////////////////////////
double CWorker::GetTime( void )
{
	//			
	struct timeb timeStamp;
	// get a time stamp
	ftime( &timeStamp );

	double nTime = (double) timeStamp.time*1000 + timeStamp.millitm;

	return( nTime );
}

////////////////////////////////////////////////////////////////////
// Method:		SetAction
// Class:		CWorker
// Purpose:		set a new action only when last is done
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CWorker::SetAction( DefCmdAction& rAction )
{
	m_vectActionQueue.push_back( rAction );
	
//	while( m_nActionCmd != THREAD_ACTION_NONE ) wxThread::Sleep( 70 );

//	m_nActionCmd = nAction;
}

////////////////////////////////////////////////////////////////////
int CWorker::RemoveTimedAction( int nId )
{
	m_nDeleteTimedActionId = nId;
	return( 1 );
}

////////////////////////////////////////////////////////////////////
void CWorker::ManageCommands()
{
	int i=0;

//	m_pMutex->Lock();

	// check if to delete - todo: a better implementation
	if( m_nDeleteTimedActionId >= 0 )
	{
		for( i=0; i<m_vectTimedActionQueue.size(); i++ )
		{
			if( m_vectTimedActionQueue[i].id == m_nDeleteTimedActionId ) 
				m_vectTimedActionQueue.erase( m_vectTimedActionQueue.begin()+i );
		}
		m_nDeleteTimedActionId = -1;
	}

//	m_pMutex->Unlock( );
}

////////////////////////////////////////////////////////////////////
// Method:		IsChanged
// Class:		CWorker
// Purpose:		if 0 change over if 1 changed
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CWorker::IsChanged( int nState )
{
	m_pMutex->Lock();
	m_bChanged = nState;
	// reset counters to zero when change done = 0
	m_pMutex->Unlock( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		Log
// Class:		CWorker
// Purpose:		log event by sening it using a command message
// Input:		message to log
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CWorker::Log( const wxString& strMsg, int nGUILoggingStyle )
{
//	return;

	wxCommandEvent event( wxEVT_CUSTOM_MESSAGE_EVENT, wxID_REMOTE_LOGGER );
	event.SetEventObject( NULL );
	// Give it some contents
	event.SetString( strMsg );
	// set the loggin style
	event.SetInt( nGUILoggingStyle );
	// Send it
	m_pFrame->GetEventHandler()->ProcessEvent( event );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		DisplayNotification
// Class:		CWorker
// Purpose:		log event by sening it using a command message
// Input:		message to log
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CWorker::DisplayNotification( const wxString& strMsg, int nType )
{
	wxCommandEvent event( wxEVT_CUSTOM_MESSAGE_EVENT, wxID_NOTIFICATION_DIALOG );
	event.SetEventObject( NULL );
	// Give it some contents
	event.SetString( strMsg );
	// set the loggin style
	event.SetInt( nType );
	// Send it
	m_pFrame->GetEventHandler()->ProcessEvent( event );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		CloseWaitDialog
// Class:		CWorker
// Purpose:		send notification to close wait dialog
// Input:		message to log
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CWorker::CloseWaitDialog( )
{
/*	wxCommandEvent event( wxEVT_CUSTOM_MESSAGE_EVENT, wxID_CLOSE_WAIT_DIALOG );
	event.SetEventObject( NULL );
	// Give it some contents
//	event.SetString( wxT(strMsg) );
	// set the loggin style
//	event.SetInt( nType );
	// Send it
	m_pFrame->GetEventHandler()->ProcessEvent( event );
*/
	wxMutexGuiEnter();
	wxCommandEvent event( wxEVT_CUSTOM_MESSAGE_EVENT, wxID_CLOSE_WAIT_DIALOG );
	event.SetEventObject( NULL );
	// Send it
	wxPostEvent( m_pFrame, event );
	wxMutexGuiLeave();

	return;
}

////////////////////////////////////////////////////////////////////
void CWorker::SetWaitDialogUpdate( )
{
	if( !m_evtWaitDialog ) return;

	wxMutexGuiEnter();
	wxCommandEvent pEvent( wxEVT_CUSTOM_MESSAGE_EVENT, wxID_WAIT_DIALOG_SET_UPDATE );
	pEvent.SetEventObject( NULL );
	// Send it
	wxPostEvent( m_evtWaitDialog, pEvent );
	wxMutexGuiLeave();
}

////////////////////////////////////////////////////////////////////
void CWorker::SetWaitDialogProgress( int nPStep )
{
	if( !m_evtWaitDialog ) return;

	if( !wxIsMainThread() ) wxMutexGuiEnter();
	wxCommandEvent pEvent( wxEVT_CUSTOM_MESSAGE_EVENT, wxID_WAIT_DIALOG_SET_PROGRESS_STEP );
	pEvent.SetEventObject( NULL );
	pEvent.SetInt( nPStep );
	// Send it
	wxPostEvent( m_evtWaitDialog, pEvent );
	if( !wxIsMainThread() ) wxMutexGuiLeave();
}

////////////////////////////////////////////////////////////////////
void CWorker::SetWaitDialogTitle( const wxString& strTitle )
{
	if( !m_evtWaitDialog ) return;

	wxMutexGuiEnter();
	wxCommandEvent pEvent( wxEVT_CUSTOM_MESSAGE_EVENT, wxID_WAIT_DIALOG_SET_TITLE );
	pEvent.SetEventObject( NULL );
	pEvent.SetString( strTitle );
	// Send it
	wxPostEvent( m_evtWaitDialog, pEvent );
	wxMutexGuiLeave();
}

////////////////////////////////////////////////////////////////////
void CWorker::SetWaitDialogMsg( const wxString& strMsg )
{
	if( !m_evtWaitDialog ) return;

	if( !wxIsMainThread() ) wxMutexGuiEnter();
	wxCommandEvent pEvent( wxEVT_CUSTOM_MESSAGE_EVENT, wxID_WAIT_DIALOG_SET_MESSAGE );
	pEvent.SetEventObject( NULL );
	pEvent.SetString( strMsg );
	// Send it
	wxPostEvent( m_evtWaitDialog, pEvent );
	if( !wxIsMainThread() ) wxMutexGuiLeave();
}

////////////////////////////////////////////////////////////////////
void CWorker::SendEventType( wxEvtHandler* pHandler, int nEventType, int bDelay )
{
	if( bDelay )  wxThread::Sleep( 300 );

//	wxMutexGuiEnter();
	wxCommandEvent pEvent( wxEVT_CUSTOM_MESSAGE_EVENT, nEventType );
	pEvent.SetEventObject( NULL );
	// Send it
	wxPostEvent( pHandler, pEvent );
//	wxMutexGuiLeave();

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		Entry
// Class:		CWorker
// Purpose:		the main executable body of my thread
// Input:		nothing
// Output:		void pointer
////////////////////////////////////////////////////////////////////
void *CWorker::Entry( )
{
//	char strMsg[255];
	unsigned int i = 0;
	m_bLife = 1;

	//When using the SDK from another thread in Windows, 
	// you must initialize the COM library by calling CoInitialize 
	CoInitializeEx( NULL, COINIT_MULTITHREADED );

	// interface to init in entry from derived class
	Entry_Init();

	////////////////////////////////////////////////////////////////
	// Start Life Cycle
	////////////////////////////////////////////////////////////////
//	m_pLogger->Log( "INFO :worker: start life cycle" );
	// loop as long as flag m_bLife = 1
	while( m_bLife )
	{
		m_bIsProcessing = 0;

		if( TestDestroy( ) == 1 ) 
			break;

		// get current time
//		m_nCurrentTime = GetTime( );

		if( m_bIsPaused == 1 )
		{
			if( m_bChanged == 1 ) IsChanged( 0 );
			wxThread::Sleep( 200 );
			continue;
		}
	
		/////////////////////////////////
		// check to manage commands like: delete/edit/perhaps add ?
		ManageCommands();

		//////////////////////////////////
		// check timed commands
		CheckForTimedCommands( );

		//////////////////////////////////
		// check for commands: ie to detect match or load catalogs
		CheckForCommands( );

		// check destroy again
		if( TestDestroy( ) == 1 ) 
			break;

		// Thread sleep
		if( m_bPauseLoop == 0 )	
			wxThread::Sleep( 10 );

		if( m_bChanged == 1 ) IsChanged( 0 );
	}
	// end of my life time
//	m_pLogger->Log( "INFO :worker: finished my life span" );

	CoUninitialize();

    return( NULL );
}

