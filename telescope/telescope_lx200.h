////////////////////////////////////////////////////////////////////
// Purpose:	Protocol header for LX200 telescope control
//
// Copyright 2008 John Kielkopf
// kielkopf@louisville.edu 
//
// Date: September 12, 2008
// Version: 5.0 
////////////////////////////////////////////////////////////////////

#ifndef _TELESCOPE_LX200_H
#define _TELESCOPE_LX200_H

// LX200 Reticle defines 
#define BRIGHTER        16  // increase 
#define DIMMER          8   // decrease 
#define BLINK0          0   // no blinking 
#define BLINK1          1   // blink rate 1 
#define BLINK2          2   // blink rate 2 
#define BLINK3          4   // blink rate 3 

// local headers
#include "telescope_driver.h"

// class:	CTelescopeLx200
///////////////////////////////////////////////////////
class CTelescopeLx200 : public CTelescopeDriver
{
// public methods
public:
	CTelescopeLx200( );
	~CTelescopeLx200( );

	// Interface control 
	int Connect( );
	void Disconnect( );
	// Report on telescope connection status
	int CheckConnectTel( ){ return( m_bTelConnectFlag ); }

	// my main moving controls
	int StartMovingEast( ){ SetRate( SLEW ); return( StartSlew( EAST ) ); }
	int StartMovingWest( ){ SetRate( SLEW ); return( StartSlew( WEST ) ); }
	int StartMovingNorth( ){ SetRate( SLEW ); return( StartSlew( NORTH ) ); }
	int StartMovingSouth( ){ SetRate( SLEW ); return( StartSlew( SOUTH ) ); }

	// Slew and track control
	int SetRate(int newRate);
	int StartSlew(int direction);
	void StopSlew(int direction);
	int StopNSEW(void);
	void StartTrack( );
	void StopTrack( );
	void FullStop( );

	// Coordinates and time 
	void GetPosition( double *telra, double *teldec, int pmodel );
	int  GoToCoords(double newRA, double newDec, int pmodel);
	int  CheckGoTo(double desRA, double desDec, int pmodel);

	// Slew limits 
	int  GetSlewStatus( );
	int  SetLimits(int limits);
	int  GetLimits(int *limits);

	// Synchronize 
	int  SyncTelOffsets(double newoffsetra, double newoffsetdec);
	int  SyncTelToUTC(double newutc);
	int  SyncTelToLocation(double newlong, double newlat, double newalt);
	int  SyncTelToLST(double newtime);
	int  SyncTelToLocalTime( );

	// LX200 utilities 
	int GetRA( double& nRa );
	int GetDec( double& nDec );
	void SetRALimits(double limitLower, double limitHigher);
	void SetDecLimits(double limitLower, double limitHigher);
	int  SetSlewRate(int slewRate);
	int  SyncLocalTime();

	// Instrumentation 
	void Reticle(int reticle);
	void Rotate(int rotatecmd, int rotatespd);
	void GetRotate(double *telrotate);
	void Fan(int fancmd);

	/////////////////
	// FOCUSER
	void Focus( int focuscmd, int focusspd );
	void FocusMove( int nCmd );
	void GetFocus(double *telfocus);
	int SetFocusSpeed( int nSpeed );
	// external commands
	int StartFocusIn( long nSpeed=0 );
	int StartFocusOut( long nSpeed=0 );
	int StopFocus( );
	// absolute focus positioning
	int FocusIn( long nUnits );
	int FocusOut( long nUnits );
	int FocusAt( long nPosition );

	// other
	void Heater( int heatercmd );
	void GetTemperature( double *teltemperature );

// public data
public:

};

#endif
