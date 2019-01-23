
#ifndef _FOCUSER_JMI_H
#define _FOCUSER_JMI_H

// local headers
#include "focuser_driver.h"

// class:	CFocuserJmi
///////////////////////////////////////////////////////
class CFocuserJmi : public CFocuserDriver
{
// public methods
public:
	CFocuserJmi( );
	~CFocuserJmi( );

	// Interface control 
	int Connect( );
	void Disconnect( );

	// commands
	int StartFocusIn( long nSpeed=0 );
	int StartFocusOut( long nSpeed=0 );
	int StopFocus( );
	// absolute focus positioning
	int FocusIn( long nUnits );
	int FocusOut( long nUnits );
	int FocusAt( long nPosition ){ return( SetPosition( nPosition ) ); }

	// set variuos speeds
	int SetMaxTravel( long nMax );
	int SetPositionSpeed( long nSpeed );
	int SetMoveSpeed( long nSpeed );
	int SetShuttleSpeed( long nSpeed );

	// other specific 
	int ReInitialize( );
	int SetZeroPosition( );
	int GetStatus( char& nStatus );
	int GetDeviceId( char& nDeviceId );

	// set/get position
	int SetPosition( long nPosition );
	int GetPosition( long& nPosition );

// public data
public:
	
};

#endif

