////////////////////////////////////////////////////////////////////
// Package:		Focuser definition
// File:		focuser.h
// Purpose:		
//
// Created by:	Larry Lart on 16/08/2004
// Updated by:  Larry Lart on 30/08/2009 - integration in unimap
//
////////////////////////////////////////////////////////////////////
#ifndef _FOCUSER_H
#define _FOCUSER_H

// wx includes
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/textctrl.h>

// interface types
enum
{
	FOCUSER_INTERFACE_TELESCOPE = 0,
	FOCUSER_INTERFACE_ASCOM,
	FOCUSER_INTERFACE_CUSTOM
};

// enum custom focuser classes implemented
enum
{
	FOCUSER_DRV_TYPE_NONE = 0,
	FOCUSER_DRV_TYPE_JMI
};

// defaults
#define MAX_EVENT_NO	200
#define DEFAULT_EVENT_FILE "./cfg/focus_events.ini"
#define DEFAULT_CONFIG_FILE "./cfg/focuser.ini"

#define DEFAULT_MOTOR_FOCUSER_SLOP		500
#define DEFAULT_MIRROR_FOCUSER_SLOP		500
// my default time step (1000 for test)
#define DEFAULT_TIME_STEP				1000

// local headers
#include "focuser_jmi.h"

// external classes
class CLogger;
class CTelescope;
class CFocusEvent;
class CAscom;

//////////////////////////////////////////////////
// class:	CFocuser
/////////////////////////////////////////////////
class CFocuser 
{
// methods
public:
	CFocuser( );
	~CFocuser( );

	void SetConfig( CTelescope* pTelescope, int nInter=0, int nType=0, int nSerial=1, int nBaud=9600 );
	int SetFocuser( );

	int Connect( );
	void Disconnect( );
	int IsConnected( );

	// save/load configuration
	int LoadConfig( const wxString& strFile );
	int SaveConfig( const wxString& strFile );

	// save/load focus events
	int LoadEvent( const wxString& strFile );
	int SaveEvent( const wxString& strFile );

	//////////////////////////
	// Movement handlers
	// set speed methods
	int SetSpeed( long nSpeed );

	int StartFocusIn( long nSpeed=0 );
	int StartFocusOut( long nSpeed=0 );
	int StopFocus( );
	// absolute focus positioning
	int FocusIn( long nUnits );
	int FocusOut( long nUnits );
	int FocusAt( long nPosition );

	// step  size
	void SetStep( unsigned int nTimeStep ){ m_nTimeStep = nTimeStep; }
	int GetStep( ){ return( m_nTimeStep ); }

	// mirror slope
	void SetMirrorSlope( unsigned int nSlopeUnits ){ m_nMirrorSlopeUnits = nSlopeUnits; }
	int GetMirrorSlope( ){ return( m_nMirrorSlopeUnits ); }
	// focuser slope
	void SetFocuserSlope( unsigned int nSlopeUnits ){ m_nMotorSlopeUnits = nSlopeUnits; }
	int GetFocuserSlope( ){ return( m_nMotorSlopeUnits ); }

	// position and units
	int GetPos( ){ return( m_nStep ); }
	int GetUnits( ){ return( m_nUnits ); }

	// reset move to zero position
	int ResetMove( );

	// focus events handlers
	CFocusEvent* AddEvent( wxString& strEvent, int nStep, int nUnits,
									int nDirection );
	int DeleteEvent( int nId );
	int PlayEvent( int nEventId );
	int ClearEvents( );

// data
public:
	// local port setup
	int m_nSerialPortNo;
	int m_nSerialPortBaud;

	// focuser interface: ascom, telescope, custom
	int m_nFocuserInterface;
	// focuser custom control class
	int m_nFocuserDrvType;

	// external objects
	CLogger			*m_pLogger;	
	CTelescope*		m_pTelescope;

	// ascom specific
	CAscom* m_pAscom;
	wxString m_strAscomDriverName;

	// other focuser objects
	CFocuserJmi*	m_pFocuserJmi;

	// keep properties vars
	long m_nLastSpeed;
	long m_nLastStep;

	// flags
	int m_bIsConnected;
	int m_nDirectionFlag;

	// speed/steps/units
	long m_nSpeed;
	long m_nTimeStep;
	long m_nStep;
	long m_nUnits;
	// slop errors
	long m_nSlopeUnits;
	long m_nMotorSlopeUnits;
	long m_nMirrorSlopeUnits;

	// focus events
	wxString			m_strEventsFile;
	CFocusEvent*	m_vectEvent[MAX_EVENT_NO];
	int				m_nEvent;


// private data
private:


};

#endif
