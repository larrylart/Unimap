////////////////////////////////////////////////////////////////////
// Package:		Logger interpreter
// File:		logger.cpp
// Purpose:		log messages to screen/file/syslog daemon
//
// Created by:	Larry Lart on 22/04/2006
// Updated by:	Larry Lart on 19/06/2010
//
// Copyright:	(c) 2006-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <time.h>

// wx headers
#include "wx/wxprec.h"
//#include "wx/thread.h"
#include <wx/textctrl.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#ifdef _WINDOWS
#include "../util/func.h"
#endif

// include main header
#include "logger.h"

////////////////////////////////////////////////////////////////////
// Method:		Constructor
// Class:		CLogger
// Purpose:		build my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
CLogger::CLogger( )
{
	// this is not used anylonger ... so for now is empty
	m_strMsgBuffer = NULL;
	//m_strMsgBuffer = (char*) malloc( (MAX_LOG_BUFFER_SIZE+1)*sizeof(char) );
	//memset( m_strMsgBuffer, 0, MAX_LOG_BUFFER_SIZE );

	m_pLoggerView = NULL;
	m_nLine = 0;
	m_bLoggingToFile = 0;
	// intialize mutex
	m_pMutex = new wxMutex( );
}

////////////////////////////////////////////////////////////////////
// Method:		Destructor
// Class:		CLogger
// Purpose:		destroy my object
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
CLogger::~CLogger()
{
	delete( m_pMutex );
	m_pLogFile = NULL;
	m_pLoggerView = NULL;
}


////////////////////////////////////////////////////////////////////
// Method:		SetLoggingToFile
// Class:		CLogger
// Purpose:		get log file name and set logging to file = true
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CLogger::SetLoggingToFile( const wxString& strLogFileName )
{
	m_strLogFileName = strLogFileName;
	m_bLoggingToFile = 1;
	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		LogToFile
// Class:		CLogger
// Purpose:		write a text message to file
// Input:		pointer to log message
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CLogger::LogToFile( const wxString& strLogMsg )
{
	m_pLogFile = wxFopen( m_strLogFileName, wxT("a+") );

	// check if file was successfully opened
	if( !m_pLogFile )
	{
		// disable logging to file
		m_bLoggingToFile = 0;
		return( 0 );
	}

	wxFprintf( m_pLogFile, wxT("%s"), strLogMsg );
	fclose( m_pLogFile );	

	return( 1 );
} 

////////////////////////////////////////////////////////////////////
// Method:		SetView
// Class:		CLogger
// Purpose:		set my logger view reference
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CLogger::SetView( wxTextCtrl *pLoggerView )
{
	// lock mutex
	m_pMutex->Lock( );

	m_pLoggerView = pLoggerView;
	// adjust view size as pe local maximum +/- 50
//	m_pLoggerView->SetMaxLength( MAX_LOG_BUFFER_SIZE + 50 );

	// unlock mutex
	m_pMutex->Unlock( );
}

////////////////////////////////////////////////////////////////////
// Method:		Log
// Class:		CLogger
// Purpose:		Log current local messages buffer or pointed messages
//				to varios outputs
// Input:		pointer to message (null by default)
// Output:		status
////////////////////////////////////////////////////////////////////
int CLogger::Log( const wxString& strLogMsg, int nGUILoggingStyle )
{
	// lock mutex
	m_pMutex->Lock( );

	wxString strMsg;
	int i = 0;
	unsigned char bFlag = 0;

	// if pointed message window  initialized
	if( m_pLoggerView == NULL )
		bFlag = 0;
	else
		bFlag = 1;
	// check if buffer is full
	int bFound = 0;

/* ------- here do something to speed up circular buffer ????
	long nOverLimit = IsFull( strLogMsg );
	if( nOverLimit > 0 )
	{
		long nClenSize = 0;
		for( i=0; i<m_nLine && bFound == 0; i++ )
		{
			//get number of  line's size from viwer to fit my message
			long nLineLenght = m_pLoggerView->GetLineLength( 0 );
			m_pLoggerView->Freeze();
			// delete first line from viwer
			m_pLoggerView->Remove( 0, nLineLenght+2 );
			m_pLoggerView->Thaw();
			// add removed size to my counter
			nClenSize += nLineLenght;
			// decrement line no
			m_nLine--;
			// if over limit end loop
			if( nClenSize > nOverLimit ) bFound = 1;
		}
	}
-------------- */

	// if found flag = 1
	if( bFlag && strLogMsg )
	{
		// get time label
		time_t rawtime;
		struct tm * timeinfo;
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );

		// copy content of remote string in local buffer
		strMsg.Printf( wxT("%.2d:%.2d:%.2d %.2d/%.2d/%d :: %s\n"),
						timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, 
						timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, 
						strLogMsg );

		// check if style 
		if( nGUILoggingStyle == GUI_LOGGER_DEFAULT_STYLE )
		{
			m_pLoggerView->SetDefaultStyle(wxTextAttr(*wxBLACK));
		} else if( nGUILoggingStyle == GUI_LOGGER_ATTENTION_STYLE )
		{
			m_pLoggerView->SetDefaultStyle(wxTextAttr(*wxRED));
		}
		// display text
		long ppos = m_pLoggerView->GetLastPosition();
		m_pLoggerView->AppendText( strMsg );
		m_pLoggerView->ShowPosition(ppos);
		m_pLoggerView->ScrollLines( 1 );
		// log to file ... todo
		if( m_bLoggingToFile == 1 ) LogToFile( strMsg ); 

		// log to syslog ... todo

		// incement line no
		m_nLine++;
	}
	// unlock mutex
	m_pMutex->Unlock( ); 

	return( bFlag );
}

////////////////////////////////////////////////////////////////////
// Method:		Is Full
// Class:		CLogger
// Purpose:		check if my message goes over buffer
// Input:		pointer to message
// Output:		status
////////////////////////////////////////////////////////////////////
long CLogger::IsFull( char *strLogMsg )
{
	long nNewSize = m_pLoggerView->GetLastPosition() + strlen( strLogMsg );
	if(  nNewSize > MAX_LOG_BUFFER_SIZE ) 
		return( nNewSize - MAX_LOG_BUFFER_SIZE );
	else
		return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:		Add
// Class:		CLogger
// Purpose:		add message to local buffer
// Input:		pointer to message
// Output:		status
////////////////////////////////////////////////////////////////////
int CLogger::Add( char *strLogMsg )
{
	if( !m_strMsgBuffer ) return(0);

	// check for size
	if( strlen( m_strMsgBuffer ) + strlen( strLogMsg )  > MAX_LOG_BUFFER_SIZE ) 
		strcpy( m_strMsgBuffer, "" );
	// if pointed message not null
	if( strLogMsg != NULL )
	{
		// copy content of remote string in local buffer
		strcat( m_strMsgBuffer, strLogMsg );
		strcat( m_strMsgBuffer, "\n" );
	} else
		return( 0 );

	return( 1 );
}

