////////////////////////////////////////////////////////////////////
// Package:		Logger Definition
// File:		logger.h
// Purpose:		log messages to screen/file/syslog daemon
//
// Created by:	Larry Lart on 22/04/2006
// Updated by:	Larry Lart on 19/06/2010
//
// Copyright:	(c) 2006-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _LOGGER_H
#define _LOGGER_H

// buffer dafaults
#define MAX_LOG_BUFFER_SIZE		5000

// setting defaults
#define GUI_LOGGER_DEFAULT_STYLE	0
#define GUI_LOGGER_ATTENTION_STYLE	1

#include "wx/wxprec.h"
#include "wx/thread.h"
#include <wx/textctrl.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers


class CLogger
{
// methods
public:
	CLogger();
	~CLogger();

	int Log( const wxString& strLogMsg = wxT(""), int nGUILoggingStyle = GUI_LOGGER_DEFAULT_STYLE );
	int Add( char *strLogMsg );
	void SetView( wxTextCtrl *pLoggerView );
	long IsFull( char *strLogMsg );
	int SetLoggingToFile( const wxString& strLogFileName );
	int LogToFile( const wxString& strLogMsg );

// data
public:
	char*		m_strMsgBuffer;
	wxTextCtrl *m_pLoggerView;
	// process syncronizer/locker
	wxMutex *m_pMutex;
	long		m_nLine;
	// flags
	int			m_bLoggingToFile;
	// file logging stuff
	wxString	m_strLogFileName;
	FILE*		m_pLogFile;
};

#endif
