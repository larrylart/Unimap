////////////////////////////////////////////////////////////////////
// Purpose:		JMI Focuser class implementing specification ver 3.02 
// Created By:	Larry Lart on 30/08/2009
////////////////////////////////////////////////////////////////////

// local headers
#include "../util/func.h"

// main header
#include "focuser_jmi.h"

////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////
CFocuserJmi::CFocuserJmi( ) : CFocuserDriver( )
{
}

////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////
CFocuserJmi::~CFocuserJmi( )
{

}

// Method:	Connect
////////////////////////////////////////////////////////////////////
int CFocuserJmi::Connect( )
{
	// first connect to serial port
	if( !ConnectSerialPort( ) ) return( 0 );

	// do here extra initialization
	char vectCmd[10] = "b";
	char vectData[10];
	int nWrite=0, nRead=0;

	nWrite = WriteToSerialPort( vectCmd, 1 );
	nRead = ReadFromSerialPort( vectData, 2 );
	// check answer
	if( nWrite == 1 && nRead == 2 && vectData[0] == 'b' && vectData[1] == 'j' ) 
	{
		m_bIsConnected = 1;
	}
	// now stop current focuser action
	StopFocus( );

	return( m_bIsConnected );
}

// Method:	Disconnect
////////////////////////////////////////////////////////////////////
void CFocuserJmi::Disconnect( )
{
	// first do things like stop focus etc
	StopFocus( );

	// disconnect serial port
	DisconnectSerialPort( );
}

// Method:	StartFocusIn
////////////////////////////////////////////////////////////////////
int CFocuserJmi::StartFocusIn( long nSpeed )
{
	char vectCmd[10] = "i";
	char vectData[10];
	int nWrite=0, nRead=0, bState=0, i=0;

	// check if to set speed 
	if( nSpeed > 0 ) SetMoveSpeed( nSpeed );

	// attempt three times
	for( i=0; i<3 && !bState; i++ )
	{
		// send command to serial port
		nWrite = WriteToSerialPort( vectCmd, 1 );
		nRead = ReadFromSerialPort( vectData, 1 );
		// check if wrote and return - if return == 'r' focus motor or
		// encoder error
		if( nWrite == 1 && nRead > 0 && vectData[0] == 'i' )
			bState = 1;
		else
			bState = 0;
	}

	return( bState );
}

// Method:	StartFocusOut
////////////////////////////////////////////////////////////////////
int CFocuserJmi::StartFocusOut( long nSpeed )
{
	char vectCmd[10] = "o";
	char vectData[10];
	int nWrite=0, nRead=0, bState=0, i=0;

	// check if to set speed 
	if( nSpeed > 0 ) SetMoveSpeed( nSpeed );

	// attempt three times
	for( i=0; i<3 && !bState; i++ )
	{
		// send command to serial port
		nWrite = WriteToSerialPort( vectCmd, 1 );
		nRead = ReadFromSerialPort( vectData, 1 );
		// check if wrote and return - if return == 'r' focus motor or
		// encoder error
		if( nWrite == 1 && nRead > 0 && vectData[0] == 'o' )
			bState = 1;
		else
			bState = 0;
	}

	return( bState );
}

// Method:	StopFocus
////////////////////////////////////////////////////////////////////
int CFocuserJmi::StopFocus( )
{
	char vectCmd[10] = "s";
	char vectData[10];
	int nWrite=0, nRead=0, bState=0, i=0;

	// attempt three times
	for( i=0; i<3 && !bState; i++ )
	{
		// send command to serial port
		nWrite = WriteToSerialPort( vectCmd, 1 );
		nRead = ReadFromSerialPort( vectData, 1 );
		// check if wrote and return - if return == 'r' focus motor or
		// encoder error
		if( nWrite == 1 && nRead > 0 && vectData[0] == 's' )
			bState = 1;
		else
			bState = 0;
	}

	return( bState );
}

// Method:	FocusIn
////////////////////////////////////////////////////////////////////
int CFocuserJmi::FocusIn( long nUnits )
{
	long nPosition=0;
	if( !GetPosition( nPosition ) ) return( 0 );
	// move from current postion to +units
	nPosition += nUnits;
	// move and return
	return( SetPosition( nPosition ) );
}

// Method:	FocusOut
////////////////////////////////////////////////////////////////////
int CFocuserJmi::FocusOut( long nUnits )
{
	long nPosition=0;
	if( !GetPosition( nPosition ) ) return( 0 );
	// move from current postion to +units
	nPosition -= nUnits;
	// move and return
	return( SetPosition( nPosition ) );
}

// Method:	SetZeroPosition
////////////////////////////////////////////////////////////////////
int CFocuserJmi::SetZeroPosition( )
{
	char vectCmd[10] = "z";
	char vectData[10];
	int nWrite=0, nRead=0, bState=0, i=0;

	// attempt three times
	for( i=0; i<3 && !bState; i++ )
	{
		// send command to serial port
		nWrite = WriteToSerialPort( vectCmd, 1 );
		nRead = ReadFromSerialPort( vectData, 1 );
		// check if wrote and return - if return == 'r' focus motor or
		// encoder error
		if( nWrite == 1 && nRead > 0 && vectData[0] == 'z' )
			bState = 1;
		else
			bState = 0;
	}

	return( bState );
}

// Method:	ReInitialize
////////////////////////////////////////////////////////////////////
int CFocuserJmi::ReInitialize( )
{
	char vectCmd[10] = "h";
	char vectData[10];
	int nWrite=0, nRead=0, bState=0, i=0;

	// attempt three times
	for( i=0; i<3 && !bState; i++ )
	{
		// send command to serial port
		nWrite = WriteToSerialPort( vectCmd, 1 );
		nRead = ReadFromSerialPort( vectData, 1 );
		// check if wrote and return - if return == 'r' focus motor or
		// encoder error
		if( nWrite == 1 && nRead > 0 && vectData[0] == 'h' )
			bState = 1;
		else
			bState = 0;
	}
	//////////////////
	// now if the command was executed ok wait loop 
	// until focuser is reinitialized
	if( bState )
	{
		bState = 0;
		// loop 18 times = 3 minutes
		for( i=0; i<12 && !bState; i++ )
		{
			// sleep 10 seconds
			Sleep( 10000 );
			// try read anything
			nRead = ReadFromSerialPort( vectData, 1 );
			// check result
			if( nRead > 0 && vectData[0] == 'c' )
				bState = 1;
			else if( nRead > 0 && vectData[0] == 'r' )
				break;
		}
	}

	return( bState );
}

// Method:	SetMaxTravel
////////////////////////////////////////////////////////////////////
int CFocuserJmi::SetMaxTravel( long nMax )
{
	if( !IsConnected() ) return( 0 );
	
	char vectCmd[10] = "w";
	char vectData[10];
	int nWrite=0, nRead=0, bState=0, i=0;

	// attempt five times
	for( i=0; i<5 && !bState; i++ )
	{
		vectCmd[1] = (nMax >> 8 ) & 0xff;
		vectCmd[2] = nMax & 0xff;

		// send command to serial port
		nWrite = WriteToSerialPort( vectCmd, 3 );
		nRead = ReadFromSerialPort( vectData, 1 );

		// check if wrote and return 
		if( nWrite == 3 && nRead > 0 && vectData[0] == 'w' )
			bState = 1;
		else
			bState = 0;
	}

	return( bState );
}

// Method:	SetPositionSpeed
////////////////////////////////////////////////////////////////////
int CFocuserJmi::SetPositionSpeed( long nSpeed )
{
	if( !IsConnected() ) return( 0 );
	
	char vectCmd[10] = "d";
	char vectData[10];
	int nWrite=0, nRead=0, bState=0, i=0;

	// attempt five times
	for( i=0; i<5 && !bState; i++ )
	{
		vectCmd[1] = (nSpeed >> 8 ) & 0xff;
		vectCmd[2] = nSpeed & 0xff;

		// send command to serial port
		nWrite = WriteToSerialPort( vectCmd, 3 );
		nRead = ReadFromSerialPort( vectData, 1 );

		// check if wrote and return 
		if( nWrite == 3 && nRead > 0 && vectData[0] == 'd' )
			bState = 1;
		else
			bState = 0;
	}

	return( bState );
}

// Method:	SetMoveSpeed
////////////////////////////////////////////////////////////////////
int CFocuserJmi::SetMoveSpeed( long nSpeed )
{
	if( !IsConnected() ) return( 0 );
	
	char vectCmd[10] = "e";
	char vectData[10];
	int nWrite=0, nRead=0, bState=0, i=0;

	// attempt five times
	for( i=0; i<5 && !bState; i++ )
	{
		vectCmd[1] = (nSpeed >> 8 ) & 0xff;
		vectCmd[2] = nSpeed & 0xff;

		// send command to serial port
		nWrite = WriteToSerialPort( vectCmd, 3 );
		nRead = ReadFromSerialPort( vectData, 1 );

		// check if wrote and return 
		if( nWrite == 3 && nRead > 0 && vectData[0] == 'e' )
			bState = 1;
		else
			bState = 0;
	}

	return( bState );
}

// Method:	SetShuttleSpeed
////////////////////////////////////////////////////////////////////
int CFocuserJmi::SetShuttleSpeed( long nSpeed )
{
	if( !IsConnected() ) return( 0 );
	
	char vectCmd[10] = "f";
	char vectData[10];
	int nWrite=0, nRead=0, bState=0, i=0;

	// attempt five times
	for( i=0; i<5 && !bState; i++ )
	{
		vectCmd[1] = (nSpeed >> 8 ) & 0xff;
		vectCmd[2] = nSpeed & 0xff;

		// send command to serial port
		nWrite = WriteToSerialPort( vectCmd, 3 );
		nRead = ReadFromSerialPort( vectData, 1 );

		// check if wrote and return 
		if( nWrite == 3 && nRead > 0 && vectData[0] == 'f' )
			bState = 1;
		else
			bState = 0;
	}

	return( bState );
}

// Method:	SetPosition
////////////////////////////////////////////////////////////////////
int CFocuserJmi::SetPosition( long nPosition )
{
	if( !IsConnected() ) return( 0 );
	
	char vectCmd[10] = "g";
	char vectData[10];
	int nWrite=0, nRead=0, bState=0, i=0;

	// attempt five times
	for( i=0; i<5 && !bState; i++ )
	{
		vectCmd[1] = (nPosition >> 8 ) & 0xff;
		vectCmd[2] = nPosition & 0xff;

		// send command to serial port
		nWrite = WriteToSerialPort( vectCmd, 3 );
		nRead = ReadFromSerialPort( vectData, 1 );

		// check if wrote and return 
		if( nWrite == 3 && nRead > 0 && vectData[0] == 'g' )
			bState = 1;
		else
			bState = 0;
	}

	// if ok 
	if( bState )
	{
		m_bMoving = 1;
		// Twenty second to??? timeout
		m_nTimeout = clock() + 20 * CLOCKS_PER_SEC;  
	}

	return( bState );
}

// Method:	GetPosition
////////////////////////////////////////////////////////////////////
int CFocuserJmi::GetPosition( long& nPosition )
{
	if( !IsConnected() ) return( 0 );

	char vectCmd[10] = "p";
	char vectData[10];
	int nWrite=0, nRead=0, bState=0, i=0;

	// attempt three times
	for( i=0; i<3 && !bState; i++ )
	{
		// send command to serial port
		nWrite = WriteToSerialPort( vectCmd, 1 );
		nRead = ReadFromSerialPort( vectData, 3 );

		// check if wrote and return 
		if( nWrite == 1 && nRead == 3 && vectData[0] == 'p' )
		{
			nPosition = (int ( vectData[1] ) << 8) + vectData[2];
			m_nLastPosition = nPosition;
			bState = 1;
		}
	}

	// stop focuser to clear buffer
	StopFocus();

	return( bState );
}

// Method:	GetStatus
////////////////////////////////////////////////////////////////////
int CFocuserJmi::GetStatus( char& nStatus )
{
	if( !IsConnected() ) return( 0 );

	char vectCmd[10] = "t";
	char vectData[10];
	int nWrite=0, nRead=0, bState=0, i=0;

	// attempt three times
	for( i=0; i<3 && !bState; i++ )
	{
		// send command to serial port
		nWrite = WriteToSerialPort( vectCmd, 1 );
		nRead = ReadFromSerialPort( vectData, 2 );

		// check if wrote and return 
		if( nWrite == 1 && nRead == 2 && vectData[0] == 't' )
		{
			nStatus = vectData[1];
			bState = 1;
		}
	}

	return( bState );
}

// Method:	GetDeviceId
////////////////////////////////////////////////////////////////////
int CFocuserJmi::GetDeviceId( char& nDeviceId )
{
	if( !IsConnected() ) return( 0 );

	char vectCmd[10] = "b";
	char vectData[10];
	int nWrite=0, nRead=0, bState=0, i=0;

	// attempt three times
	for( i=0; i<3 && !bState; i++ )
	{
		// send command to serial port
		nWrite = WriteToSerialPort( vectCmd, 1 );
		nRead = ReadFromSerialPort( vectData, 2 );

		// check if wrote and return 
		if( nWrite == 1 && nRead == 2 && vectData[0] == 'b' )
		{
			nDeviceId = vectData[1];
			bState = 1;
		}
	}

	return( bState );
}
