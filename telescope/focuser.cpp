////////////////////////////////////////////////////////////////////
// Package:		Focuser implementation
// File:		focuser.cpp
// Purpose:		focuser class to manager various types of focusers
//				or telescope focuser
//
// Created by:	Larry Lart on 16/08/2004
// Updated by:  Larry Lart on 30/08/2009 - integration in unimap
//
////////////////////////////////////////////////////////////////////

// system headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/regex.h>
#include <wx/string.h>
#include "wx/thread.h"

// local headers
#include "../util/func.h"
#include "../logger/logger.h"
#include "focusevent.h"
#include "../observer/observer_hardware.h"
#include "telescope.h"
#include "ascom.h"

// class header
#include "focuser.h"

////////////////////////////////////////////////////////////////////
// Method:	Constructor
////////////////////////////////////////////////////////////////////
CFocuser::CFocuser( )
{
	// init defaults
	m_pLogger = NULL;
	m_pTelescope=NULL;
	// reset other focuser types to null
	m_pFocuserJmi=NULL;
	// ascom default
	m_pAscom = NULL;
	m_nFocuserInterface = 0;
	m_nFocuserDrvType = 0;
	// serial port defaults
	m_nSerialPortNo = 1;
	m_nSerialPortBaud = 9600;

	m_strEventsFile = wxT(DEFAULT_EVENT_FILE);
	// initialize defaut slops
	m_nMotorSlopeUnits = DEFAULT_MOTOR_FOCUSER_SLOP;
	m_nMirrorSlopeUnits = DEFAULT_MIRROR_FOCUSER_SLOP;
	// my default time step
	m_nTimeStep = DEFAULT_TIME_STEP;

	// set the startup values/flags
	m_nStep = 0;
	m_nUnits = 0;
	// set direction flag = 1 - focus in down last
	m_nDirectionFlag=1;
	m_nEvent = 0;

	// load config
//	LoadConfig( DEFAULT_CONFIG_FILE );

	/////////////////////////////////////
	// Initialization commands
//	SetSpeed( 1 );
//	FocusIn( m_nMotorFocuserSlop+m_nMirrorFocuserSlop+1000 );


	// load my default events
//	LoadEvent( m_strEventsFile );

}

///////////////////////////////////////////////////////////////////
// Method:	Destructor
////////////////////////////////////////////////////////////////////
CFocuser::~CFocuser( )
{

	// save my configuration
//	SaveConfig( DEFAULT_CONFIG_FILE );
	// save my  events
//	SaveEvent( m_strEventsFile );

	ClearEvents( );
	if( m_pFocuserJmi ) delete( m_pFocuserJmi );

	return;
}

// config reference to telescope type and serial port if exist
////////////////////////////////////////////////////////////////////
void CFocuser::SetConfig( CTelescope* pTelescope, int nInter, int nType, int nSerial, int nBaud )
{ 
	m_pTelescope = pTelescope;
	m_nFocuserInterface = nInter;
	m_nFocuserDrvType = nType;

	m_nSerialPortNo = nSerial;
	m_nSerialPortBaud = 9600;

	return;
}

// Method:	SetFocuser
////////////////////////////////////////////////////////////////////
int CFocuser::SetFocuser( ) 
{
	// :: Ascom
	if( m_nFocuserInterface == FOCUSER_INTERFACE_ASCOM )
	{
		if( !m_pAscom ) m_pAscom = new CAscom( ASCOM_DRV_TYPE_FOCUSER );
		m_pAscom->m_strDriverId = m_strAscomDriverName;

	// :: Telescope
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_TELESCOPE )
	{
		// do something here ???

	// :: CUSTOM FOCUSER INTERFACE
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_CUSTOM )
	{
		// :: JMI focuser
		if( m_nFocuserDrvType == FOCUSER_DRV_TYPE_JMI )
		{
			if( !m_pFocuserJmi ) m_pFocuserJmi = new CFocuserJmi();
			m_pFocuserJmi->SetSerialPort( m_nSerialPortNo, m_nSerialPortBaud );
		}
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	Connect
// Class:	CFocuser
// Purpose:	connect to focuser
// Input:	nothing
// Output:	status error not zero ok=zero	
////////////////////////////////////////////////////////////////////
int CFocuser::Connect( )
{
	if( IsConnected() ) return( 0 );

	int bState = 0;
	// set telescope
	if( !SetFocuser( ) ) return( 0 );

	// :: Ascom
	if( m_nFocuserInterface == FOCUSER_INTERFACE_ASCOM )
	{
		bState = m_pAscom->Connect( );
		m_bIsConnected = 1;

	// :: Telescope
	} else if(  m_nFocuserInterface == FOCUSER_INTERFACE_TELESCOPE )
	{
		bState = m_pTelescope->Connect( );
		m_bIsConnected = 1;

	// :: CUSTOM FOCUSER INTERFACE
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_CUSTOM )
	{
		// :: JMI focuser
		if( m_nFocuserDrvType == FOCUSER_DRV_TYPE_JMI )
		{
			bState = m_pFocuserJmi->Connect( );
		}
	}
	
	return( bState );
}

////////////////////////////////////////////////////////////////////
void CFocuser::Disconnect( )
{
	if( !IsConnected() ) return;

	// :: Ascom
	if( m_nFocuserInterface == FOCUSER_INTERFACE_ASCOM )
	{
		m_pAscom->Disconnect( );
		m_bIsConnected = 1;

	// :: CUSTOM FOCUSER INTERFACE
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_CUSTOM )
	{
		// :: JMI focuser
		if( m_nFocuserDrvType == FOCUSER_DRV_TYPE_JMI )
			m_pFocuserJmi->Disconnect( );
	}
}

// check if connection is/has been made
////////////////////////////////////////////////////////////////////
int CFocuser::IsConnected( )
{
	// :: Ascom
	if( m_nFocuserInterface == FOCUSER_INTERFACE_ASCOM )
		return( m_pAscom->IsConnected() );
	// :: Telescope
	if( m_nFocuserInterface == FOCUSER_INTERFACE_TELESCOPE )
		return(  m_pTelescope->IsConnected() );
	// :: CUSTOM FOCUSER INTERFACE
	else if( m_nFocuserInterface == FOCUSER_INTERFACE_CUSTOM  )
	{
		if( m_nFocuserDrvType == FOCUSER_DRV_TYPE_JMI )
			return( m_pFocuserJmi->IsConnected() );
		else
			return( 0 );
	} else
		return( 0 );
}

// Method:	Set Speed
////////////////////////////////////////////////////////////////////
int CFocuser::SetSpeed( long nSpeed )
{
	if( !IsConnected( ) ) return( 0 );

	int bStatus=0;

	// :: Ascom
	if( m_nFocuserInterface == FOCUSER_INTERFACE_ASCOM )
	{
		// ascom doesn't have speed option ...
	// :: Telescope
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_TELESCOPE )
	{
		bStatus = m_pTelescope->SetFocusSpeed( nSpeed );
	// :: CUSTOM FOCUSER INTERFACE
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_CUSTOM  )
	{
		if( m_nFocuserDrvType == FOCUSER_DRV_TYPE_JMI )
			bStatus = m_pFocuserJmi->SetMoveSpeed( nSpeed );
	}		

	// now also set local
	if( bStatus ) m_nSpeed = nSpeed;

	return( bStatus );
}

// Method:	StartFocusIn
////////////////////////////////////////////////////////////////////
int CFocuser::StartFocusIn( long nSpeed )
{
	if( !IsConnected( ) ) return( 0 );

	int bStatus=0;

	// :: Ascom
	if( m_nFocuserInterface == FOCUSER_INTERFACE_ASCOM )
	{
		bStatus = m_pAscom->StartFocusIn();

	// :: Telescope
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_TELESCOPE )
	{
		bStatus = m_pTelescope->StartFocusIn( nSpeed );
	// :: CUSTOM FOCUSER INTERFACE
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_CUSTOM  )
	{
		if( m_nFocuserDrvType == FOCUSER_DRV_TYPE_JMI )
			bStatus = m_pFocuserJmi->StartFocusIn( nSpeed );
	}		
	
	// on OK - keep last speed/step
	if( bStatus ) 
	{
		m_nLastSpeed = nSpeed;
		m_nDirectionFlag = 1;
	}

	return( bStatus );
}

// Method:	StartFocusOut
////////////////////////////////////////////////////////////////////
int CFocuser::StartFocusOut( long nSpeed )
{
	if( !IsConnected( ) ) return( 0 );

	int bStatus=0;

	// :: Ascom
	if( m_nFocuserInterface == FOCUSER_INTERFACE_ASCOM )
	{
		bStatus = m_pAscom->StartFocusOut();

	// :: Telescope
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_TELESCOPE )
	{
		bStatus = m_pTelescope->StartFocusOut( nSpeed );
	// :: CUSTOM FOCUSER INTERFACE
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_CUSTOM  )
	{
		if( m_nFocuserDrvType == FOCUSER_DRV_TYPE_JMI )
			bStatus = m_pFocuserJmi->StartFocusOut( nSpeed );
	}		
	
	// on OK - keep last speed/step
	if( bStatus ) 
	{
		m_nLastSpeed = nSpeed;
		m_nDirectionFlag = -1;
	}

	return( bStatus );
}

// Method:	StopFocus
////////////////////////////////////////////////////////////////////
int CFocuser::StopFocus( )
{
	if( !IsConnected( ) ) return( 0 );

	int bStatus=0;

	// :: Ascom
	if( m_nFocuserInterface == FOCUSER_INTERFACE_ASCOM )
	{
		bStatus = m_pAscom->StopFocus();

	// :: Telescope
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_TELESCOPE )
	{
		bStatus = m_pTelescope->StopFocus();
	// :: CUSTOM FOCUSER INTERFACE
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_CUSTOM  )
	{
		if( m_nFocuserDrvType == FOCUSER_DRV_TYPE_JMI )
			bStatus = m_pFocuserJmi->StopFocus();
	}		
	
	return( bStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	FocusIn
////////////////////////////////////////////////////////////////////
int CFocuser::FocusIn( long nUnits )
{
	if( !IsConnected( ) ) return( 0 );

	int bStatus=0;
	wxString strMsg;
	long nTotalUnits = nUnits;

	// apply slop if direction has changed
	if( m_nDirectionFlag == -1 )
	{
		nTotalUnits += m_nSlopeUnits;
		// debug logging
		if( m_pLogger )
		{
			strMsg.Printf( wxT("INFO :: direction changed. Slope added=%d."), m_nSlopeUnits );
			m_pLogger->Log( strMsg );
		}
	}
		
	//////////////////
	// :: Ascom
	if( m_nFocuserInterface == FOCUSER_INTERFACE_ASCOM )
	{
		bStatus = m_pAscom->FocusIn( nTotalUnits );

	// :: Telescope
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_TELESCOPE )
	{
		bStatus = m_pTelescope->FocusIn( nTotalUnits );
	// :: CUSTOM FOCUSER INTERFACE
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_CUSTOM  )
	{
		if( m_nFocuserDrvType == FOCUSER_DRV_TYPE_JMI )
			bStatus = m_pFocuserJmi->FocusIn( nTotalUnits );
	}		

	// set direction flag to focus in
	m_nDirectionFlag = 1;
	// adjust units
	m_nStep--;
	m_nUnits -= nUnits;

	// debug logging
	if( m_pLogger )
	{
		strMsg.Printf( wxT("INFO :: focus IN at step=%d."), nTotalUnits );
		m_pLogger->Log( strMsg );
	}

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	FocusOut
////////////////////////////////////////////////////////////////////
int CFocuser::FocusOut( long nUnits )
{
	if( !IsConnected( ) ) return( 0 );

	int bStatus=0;
	wxString strMsg;
	long nTotalUnits = nUnits;

	// apply slop if direction has changed
	if( m_nDirectionFlag == 1 )
	{
		nTotalUnits += m_nSlopeUnits;
		// debug logging
		if( m_pLogger )
		{
			strMsg.Printf( wxT("INFO :: direction changed. Slope added=%d."), m_nSlopeUnits );
			m_pLogger->Log( strMsg );
		}
	}
		
	//////////////////
	// :: Ascom
	if( m_nFocuserInterface == FOCUSER_INTERFACE_ASCOM )
	{
		bStatus = m_pAscom->FocusOut( nTotalUnits );

	// :: Telescope
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_TELESCOPE )
	{
		bStatus = m_pTelescope->FocusOut( nTotalUnits );
	// :: CUSTOM FOCUSER INTERFACE
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_CUSTOM  )
	{
		if( m_nFocuserDrvType == FOCUSER_DRV_TYPE_JMI )
			bStatus = m_pFocuserJmi->FocusOut( nTotalUnits );
	}		

	// set direction flag to focus out
	m_nDirectionFlag = -1;
	// adjust units
	m_nStep++;
	m_nUnits += nUnits;

	// debug logging
	if( m_pLogger )
	{
		strMsg.Printf( wxT("INFO :: focus OUT at step=%d."), nTotalUnits );
		m_pLogger->Log( strMsg );
	}

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	FocusAt
////////////////////////////////////////////////////////////////////
int CFocuser::FocusAt( long nPosition )
{
	if( !IsConnected( ) ) return( 0 );

	int bStatus=0;
	wxString strMsg;

// HERE I SHOULD THINK IF THERE IS A POINT IN TALKING ABOUT SLOPE
// IN ABSOLUTE POSITIONING ?
/*
	// get current position
	long nCurrentPosition = 0;
	GetPosition( nCurrentPosition );
	// if position = with current return
	if( nCurrentPosition == nPosition ) return( 1 );

	// check direction to achive this postion
	int nDir == -1;
	if( nPosition > nCurrentPosition )
		nDir = 1;

	// apply slope if direction has changed
	if( m_nDirectionFlag != nDir )
	{
		nTotalUnits += m_nSlopeUnits;
		// debug logging
		if( m_pLogger )
		{
			sprintf( strMsg, "INFO :: direction changed. Slope added=%d.", m_nSlopeUnits );
			m_pLogger->Log( strMsg );
		}
	}
*/		
	//////////////////
	// :: Ascom
	if( m_nFocuserInterface == FOCUSER_INTERFACE_ASCOM )
	{
		bStatus = m_pAscom->FocusAt( nPosition );

	// :: Telescope
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_TELESCOPE )
	{
		bStatus = m_pTelescope->FocusAt( nPosition );
	// :: CUSTOM FOCUSER INTERFACE
	} else if( m_nFocuserInterface == FOCUSER_INTERFACE_CUSTOM  )
	{
		if( m_nFocuserDrvType == FOCUSER_DRV_TYPE_JMI )
			bStatus = m_pFocuserJmi->FocusAt( nPosition );
	}		

/* ???
	// set direction flag to focus out
	m_nDirectionFlag = -1;
	// adjust units
	m_nStep++;
	m_nUnits += nUnits;
*/
	// debug logging
	if( m_pLogger )
	{
		strMsg.Printf( wxT("INFO :: focus at position=%d."), nPosition );
		m_pLogger->Log( strMsg );
	}

	return( bStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	Reset Move
// Class:	CFocuser
// Purpose:	reset parameters to zero for current move
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
int CFocuser::ResetMove( )
{
	// set the startup values/flags
	m_nStep = 0;
	m_nUnits = 0;
	return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:	LoadConfig
// Class:	CFocuser
// Purose:	Load configuration data
// Input:	nothing
// Output:	status	
////////////////////////////////////////////////////////////////////
int CFocuser::LoadConfig( const wxString& strFile )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxString strMsg;
	// define my regex: event id =    steps,   units,   direction,  
	wxRegEx reCfg( wxT("^([a-zA-Z]+)\\s*=\\s*([0-9a-zA-Z\\_\\-\\(\\)\\ \\_\\B\\.[:punct:]]+)\\s*\n") );

	// Read the index
	pFile = wxFopen( strFile, wxT("r") );
	// check if there is any configuration to load
	if( !pFile )
	{
		// debug
		strMsg.Printf( wxT("Warning :: could not open config file %s"), strFile );
		m_pLogger->Log( strMsg, GUI_LOGGER_ATTENTION_STYLE );
		return( -1 );
	}
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// get one line out of the config file
		wxFgets( strLine, 2000, pFile );
//		wxString strWxLine(strLine,wxConvUTF8);

		// check if event list
		if( reCfg.Matches( strLine ) )
		{
			wxString strVarName = reCfg.GetMatch(strLine, 1 );
			wxString strVarValue = reCfg.GetMatch(strLine, 2 );
			// now just do a basic if case swipe
			if( !strVarName.CmpNoCase( wxT("SearialPortNo") ) && strVarValue.IsNumber() )
			{
				m_nSerialPortNo = wxAtoi( strVarValue );

			} else if( !strVarName.CmpNoCase( wxT("MotorSlopeUnits") ) &&
						strVarValue.IsNumber() )
			{
				m_nMotorSlopeUnits = wxAtoi( strVarValue );
			} else if( !strVarName.CmpNoCase( wxT("MirrorSlopeUnits") ) &&
						strVarValue.IsNumber() )
			{
				m_nMirrorSlopeUnits = wxAtoi( strVarValue );

			} else if( !strVarName.CmpNoCase( wxT("TimeStep") ) &&
						strVarValue.IsNumber() )
			{
				m_nTimeStep = wxAtoi( strVarValue );

			} else if( !strVarName.CmpNoCase( wxT("DirectionFlag") ) &&
						strVarValue.IsNumber() )
			{
				m_nDirectionFlag = wxAtoi( strVarValue );

			} else if( !strVarName.CmpNoCase( wxT("LastUsedEventsFile") ) )
			{
				m_strEventsFile = strVarValue;
			}
		}
	}
	// close my file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	SaveConfig
// Class:	CFocuser
// Purose:	Save config data
// Input:	file name with path
// Output:	status
////////////////////////////////////////////////////////////////////
int CFocuser::SaveConfig( const wxString& strFile )
{
	FILE* pFile = NULL;
//	char strLine[255];
	wxString strMsg;
	int i = 0;

	// open reflex file to write
	pFile = wxFopen( strFile, wxT("w") );
	// check if config file was opened ok for writing
	if( !pFile )
	{
		// debug
		strMsg.Printf( wxT("Warning :: could not write config file %s"), strFile );
		m_pLogger->Log( strMsg, GUI_LOGGER_ATTENTION_STYLE );
		return( -1 );
	}
	// for every variable
	fprintf( pFile, "SearialPortNo=%d\n", m_nSerialPortNo );
	fprintf( pFile, "MotorSlopeUnits=%d\n", m_nMotorSlopeUnits );
	fprintf( pFile, "MirrorSlopeUnits=%d\n", m_nMirrorSlopeUnits );
	fprintf( pFile, "TimeStep=%d\n", m_nTimeStep );
	fprintf( pFile, "DirectionFlag=%d\n", m_nDirectionFlag );
	wxFprintf( pFile, wxT("LastUsedEventsFile=%s\n"), m_strEventsFile );

	// close my file handler
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	ClearEvents
// Class:	CFocuser
// Purpose:	clear all events
// Input:	nothing
// Output:	status error not zero ok=zero	
////////////////////////////////////////////////////////////////////
int CFocuser::ClearEvents( )
{
	int i = 0;

	for( i=0; i<m_nEvent; i++ )
	{
		delete( m_vectEvent[i] );
		m_vectEvent[i] = NULL;
	}
	m_nEvent = 0;
	// initialize defaut slops
	m_nMotorSlopeUnits = DEFAULT_MOTOR_FOCUSER_SLOP;
	m_nMirrorSlopeUnits = DEFAULT_MIRROR_FOCUSER_SLOP;
	// my default time step
	m_nTimeStep = DEFAULT_TIME_STEP;
	// set the startup values/flags
	m_nStep = 0;
	m_nUnits = 0;
	// set direction flag = 1 - focus in down last
	m_nDirectionFlag=1;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	LoadEvent
// Class:	CFocuser
// Purose:	Load data for predefined events
// Input:	nothing
// Output:	status	
////////////////////////////////////////////////////////////////////
int CFocuser::LoadEvent( const wxString& strFile )
{
	FILE* pFile = NULL;
	wxChar strLine[2000];
	wxString strMsg;
	int bFoundEvent = 0;

	wxRegEx reEventLabel = wxT( "^\\[([a-zA-Z0-9\\ \\_[:punct:]]+)\\]" );
	wxRegEx reEventId = wxT( "^Id=([0-9\\.[:punct:]]+)" );
	wxRegEx reEventStep = wxT( "^Step=([0-9\\.[:punct:]]+)" );
	wxRegEx reEventUnits = wxT( "^Units=([0-9\\.[:punct:]]+)" );
	wxRegEx reEventDirection = wxT( "^Direction=([0-9\\.[:punct:]]+)" );

	// reset to default
	ClearEvents( );

	m_strEventsFile = strFile;

	// Read the index
	pFile = wxFopen( m_strEventsFile, wxT("r") );
	// check if there is any configuration to load
	if( !pFile )
	{
		// debug
		strMsg.Printf( wxT("Warning :: could not open event file %s"), m_strEventsFile );
		m_pLogger->Log( strMsg, GUI_LOGGER_ATTENTION_STYLE );
		return( -1 );
	}
	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// get one line out of the config file
		wxFgets( strLine, 2000, pFile );
//		wxString strWxLine(strLine,wxConvUTF8);
		
		// check for event label
		if( reEventLabel.Matches( strLine ) )
		{
			bFoundEvent = 0;

			// get event name
			wxString strEventName = reEventLabel.GetMatch(strLine, 1 );
			// create default event
			m_vectEvent[m_nEvent] = new CFocusEvent( strEventName,
											m_nEvent, 0, 0, 0 );
			m_nEvent++;

			bFoundEvent = 1;

		// check if event list
		} else if( reEventId.Matches( strLine ) && bFoundEvent == 1 )
		{
			m_vectEvent[m_nEvent-1]->m_nId = wxAtol( reEventId.GetMatch( strLine, 1 ) );
			
		} else if( reEventStep.Matches( strLine ) && bFoundEvent == 1 )
		{
			m_vectEvent[m_nEvent-1]->m_nStep = wxAtol( reEventStep.GetMatch( strLine, 1 ) );

		} else if( reEventUnits.Matches( strLine ) && bFoundEvent == 1 )
		{
			m_vectEvent[m_nEvent-1]->m_nUnits = wxAtol( reEventUnits.GetMatch( strLine, 1 ) );

		} else if( reEventDirection.Matches( strLine ) && bFoundEvent == 1 )
		{
			m_vectEvent[m_nEvent-1]->m_nDirection = wxAtol( reEventDirection.GetMatch( strLine, 1 ) );
		}

	}
	// close my file
	fclose( pFile );

	// debug
	strMsg.Printf( wxT("INFO :: Loaded %d events from file"), m_nEvent );
	m_pLogger->Log( strMsg );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	SaveEvent
// Class:	CFocuser
// Purose:	Save events data
// Input:	file name with path
// Output:	status
// Todo:	
////////////////////////////////////////////////////////////////////
int CFocuser::SaveEvent( const wxString& strFile )
{
	FILE* pFile = NULL;
//	char strLine[255];
	wxString strMsg;
	int i = 0;

	// now copy in the file name as the last used
	m_strEventsFile = strFile;

	// open reflex file to write
	pFile = wxFopen( m_strEventsFile, wxT("w") );
	// check if even file was opened ok for writing
	if( !pFile )
	{
		// debug
		strMsg.Printf( wxT("Warning :: could not write event file %s"), m_strEventsFile );
		m_pLogger->Log( strMsg, GUI_LOGGER_ATTENTION_STYLE );
		return( -1 );
	}
	// for each event
	for( i=0; i<m_nEvent; i++ )
	{
		CFocusEvent* pFocusEvent = m_vectEvent[i];

		wxFprintf( pFile, wxT("[%s]\n"), pFocusEvent->m_strEventName );
		fprintf( pFile, "Id=%d\n", pFocusEvent->m_nId );
		fprintf( pFile, "Step=%d\n", pFocusEvent->m_nStep );
		fprintf( pFile, "Units=%d\n", pFocusEvent->m_nUnits );
		fprintf( pFile, "Direction=%d\n", pFocusEvent->m_nDirection );

		// add an extra line
		fprintf( pFile, "\n" );

	}

	// close my file handler
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	Add Event
// Class:	CFocuser
// Purpose:	add a focusing event to local list
// Input:	reference to focus event 
// Output:	pointer to string 
////////////////////////////////////////////////////////////////////
CFocusEvent* CFocuser::AddEvent( wxString& strEvent, int nStep, int nUnits,
									int nDirection )
{
	wxString strMsg;
//	int nEventId = -1;

	m_vectEvent[m_nEvent] = new CFocusEvent( strEvent, m_nEvent, 
							nStep, abs( nUnits ), nDirection );
	m_nEvent++;

	// debug logging
	strMsg.Printf( wxT("INFO :: added event id=%d, step=%d, units=%d."), 
						m_nEvent-1, m_nStep, m_nUnits );
	m_pLogger->Log( strMsg );

	// return event label
	return( m_vectEvent[m_nEvent-1] );
}

////////////////////////////////////////////////////////////////////
// Method:	DeleteEvent
// Class:	CFocuser
// Purpose:	add a focusing event to local list
// Input:	reference to focus event 
// Output:	pointer to string 
////////////////////////////////////////////////////////////////////
int CFocuser::DeleteEvent( int nId )
{
	int i = 0;

	delete( m_vectEvent[nId] );
	m_vectEvent[nId] = NULL;

	for( i=nId+1; i<m_nEvent; i++ )
	{
		m_vectEvent[i-1] = m_vectEvent[i];
	}

	m_nEvent--;
	m_vectEvent[m_nEvent] = NULL;


	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	Play Event
// Class:	CFocuser
// Purpose:	play a focusing event
// Input:	event index
// Output:	status
////////////////////////////////////////////////////////////////////
int CFocuser::PlayEvent( int nEventId )
{
	wxString strMsg;
	if( nEventId < 0 ) return( -1 ); 

//	nEventId = m_vectEvent.Add( strMyEvent );
	// debug logging
	strMsg.Printf( wxT("INFO :: play event id=%d."), nEventId );
	m_pLogger->Log( strMsg );

	if( m_vectEvent[nEventId]->m_nDirection == -1 )
	{
		FocusOut( m_vectEvent[nEventId]->m_nUnits );
	} else if( m_vectEvent[nEventId]->m_nDirection == 1 )
	{
		FocusIn( m_vectEvent[nEventId]->m_nUnits );
	}

	// return event label
	return( 0 );
}
