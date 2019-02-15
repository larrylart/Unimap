////////////////////////////////////////////////////////////////////
// Protocol header for Celestron NexStar telescope control 
//
// Copyright 2008 John Kielkopf  
// kielkopf@louisville.edu 
//                                      
// Date: September 11, 2008 
// Version: 5.0  
// Modified by:	Larry Lart
////////////////////////////////////////////////////////////////////
 
#ifndef _TELESCOPE_NEXSTAR_H
#define _TELESCOPE_NEXSTAR_H

// local headers
#include "telescope_driver.h"

// class:	CTelescopeNexStar
///////////////////////////////////////////////////////
class CTelescopeNexStar : public CTelescopeDriver
{
// public methods
public:
	CTelescopeNexStar( );
	~CTelescopeNexStar( );

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
	void SetRate( int newRate );
	int StartSlew( int direction );
	void StopSlew( int direction );
	int StopSlewAll( );
	void StartTrack( );
	void StopTrack( );
	void FullStop( );

	// Coordinates and time 
	void GetPosition( double *telra, double *teldec, int pmodel );
	int  GoToCoords(double newRA, double newDec, int pmodel);
	int  CheckGoTo(double desRA, double desDec, int pmodel);

	// Slew limits 
	int  GetSlewStatus(void);
	int  SetLimits(int limits);
	int  GetLimits(int *limits);

	// Synchronizing 
	int  SyncTelOffsets(double newoffsetra, double newoffsetdec);
	int  SyncTelToUTC(double newutc);
	int  SyncTelToLocation(double newlong, double newlat, double newalt);
	int  SyncTelToLST(double newTime);

	// Instrumentation 
	void Heater(int heatercmd);
	void Fan(int fancmd);
	void Rotate(int rotatecmd, int rotatespd);
	void GetRotate(double *telrotate);
	void GetTemperature(double *teltemperature);	  

	/////////////////
	// FOCUSER
	void Focus(int focuscmd, int focusspd);
	void GetFocus(double *telfocus);
	// external commands
	int SetFocusSpeed( int nSpeed );
	int StartFocusIn( long nSpeed=0 );
	int StartFocusOut( long nSpeed=0 );
	int StopFocus( );
	// absolute focus positioning
	int FocusIn( long nUnits );
	int FocusOut( long nUnits );
	int FocusAt( long nPosition );

	// other
	int SetSlewRate( int slewRate );
	int SyncLST( double newTime );
	int SyncLocalTime();

// public data
public:
	// NexStar local data 
	// Rate for slew request in StartSlew 
	int slewRate;        
};

#endif


