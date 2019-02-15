////////////////////////////////////////////////////////////////////
// Telescope Control Protocol for the Celestron NexStar
//
// Copyright 2006, 2007, 2008 John Kielkopf 
// John Kielkopf (kielkopf@louisville.edu)  
// Date: September 7, 2008  
// Version: 5.0    
// Modified by: Larry Lart - Unimap integration
////////////////////////////////////////////////////////////////////
// Handcontroller interfacing notes 
//
// The command format for a connection through the hand controller is     
// where:
//
//   0x50 is a code requesting that the data pass through to the motors     
//  msgLen is how many bytes including msgId  and valid data bytes     
//   destId is the AUX command set destination number                    
//   msgId  is the AUX command set message number                        
//   data1-3 are up to 3 bytes of message data                           
//   responseBytes is the number of bytes to echo back in response       
//
// For example, the command to get data from the focus motor is          
//
//   focusCmd[] = { 0x50, 0x01, 0x12, 0x01, 0x00, 0x00, 0x00, 0x03 }     
//
// These pass-through commands are used in                               
//
//   GetFocus   (CDK20 only) 
//   Focus      (CDK20 only) 
//   StartTrack 
//   StopTrack  
//   StartSlew  
//   StopSlew   
//
////////////////////////////////////////////////////////////////////

// system headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

// local headers
#include "../util/astro_algorithms.h"
#include "../dataport/winserial.h"
#include "protocol.h"

// main header
#include "telescope_nexstar.h"

////////////////////////////////////////////////////////////////////
// Handcontroller interfacing notes 
//
// The command format for a connection through the hand controller is     
// where:
//
//   0x50 is a code requesting that the data pass through to the motors     
//  msgLen is how many bytes including msgId  and valid data bytes     
//   destId is the AUX command set destination number                    
//   msgId  is the AUX command set message number                        
//   data1-3 are up to 3 bytes of message data                           
//   responseBytes is the number of bytes to echo back in response       
//
// For example, the command to get data from the focus motor is          
//
//   focusCmd[] = { 0x50, 0x01, 0x12, 0x01, 0x00, 0x00, 0x00, 0x03 }     
//
// These pass-through commands are used in                               
//
//   GetFocus   (CDK20 only) 
//   Focus      (CDK20 only) 
//   StartTrack 
//   StopTrack  
//   StartSlew  
//   StopSlew   
//
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////
CTelescopeNexStar::CTelescopeNexStar( ) : CTelescopeDriver( )
{

}

////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////
CTelescopeNexStar::~CTelescopeNexStar( )
{

}

// Connect to the telescope serial interface 
// Returns without action if TelConnectFlag is TRUE 
// Sets TelConnectFlag TRUE on success 
// Two ports are reserved here but only the first one is used by the nexstar 
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::Connect( )
{
	if( m_bTelConnectFlag != 0 ) return( 1 );
	
	char vectRead[255];
	int nRead=0, nWrite=0;
	int limits, flag;

	// open serial port
	if( !m_pSerialPort->Open( m_nSerialPortNo, m_nSerialPortBaud ) ) 
		return( 0 );

	// Test connection 
	WriteToSerialPort( "Kx",2 );
	nRead = ReadFromSerialPort( vectRead, 3 );
	vectRead[nRead] = '\0';

	if( nRead == 2 ) m_bTelConnectFlag = true;

	// Perform startup tests 
	flag = GetLimits( &limits );
	printf( "Reading old limits flag %d and return value  %d\n", limits, flag );
	
	Sleep( 1000 );

	limits = FALSE;
	flag = SetLimits( limits );
	printf( "Setting new limits flag %d and return value  %d\n", limits, flag );

	Sleep( 1000 );

	flag = GetLimits( &limits );
	printf( "Reading new limits flag %d and return value  %d\n", limits, flag );

	// Diagnostic tests 
	//printf("ConnectTel read %d characters: %s\n",numRead,returnStr); 
	//printf("TelConnectFlag set to: %d\n",TelConnectFlag); 

	return( m_bTelConnectFlag );
}

////////////////////////////////////////////////////////////////////
void CTelescopeNexStar::Disconnect( )
{
	// disconect serial port
	if( m_bTelConnectFlag == 1 ) m_pSerialPort->Close( );

	m_bTelConnectFlag = 0;
}

// Purpose:	Assign and save slewRate for use in StartSlew 
////////////////////////////////////////////////////////////////////
void CTelescopeNexStar::SetRate( int newRate )
{
	if( newRate == SLEW ) 
		slewRate = 9; 
	else if( newRate == FIND ) 
		slewRate = 6;
	else if( newRate == CENTER ) 
		slewRate = 3;
	else if( newRate == GUIDE )
		slewRate = 1;
}
 
// Start a slew in chosen direction at slewRate
// Use auxilliary NexStar command set through the hand control computer
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::StartSlew( int direction )
{
	char slewCmd[] = { 0x50, 0x02, 0x11, 0x24, 0x09, 0x00, 0x00, 0x00 };
	char inputStr[2048];
	int nWrite=0;

	if( direction == NORTH )
	{
		slewCmd[2] = 0x11; 
		slewCmd[3] = 0x24;
		slewCmd[4] = slewRate;

	} else if( direction == EAST )
	{
		slewCmd[2] = 0x10; 
		slewCmd[3] = 0x25;
		slewCmd[4] = slewRate;

	} else if( direction == SOUTH )
	{
		slewCmd[2] = 0x11; 
		slewCmd[3] = 0x25;
		slewCmd[4] = slewRate;

	} else if( direction == WEST )
	{
		slewCmd[2] = 0x10; 
		slewCmd[3] = 0x24;
		slewCmd[4] = slewRate;
	}

	nWrite = WriteToSerialPort( slewCmd, 8 );

	// Look for '#' acknowledgement of request
	for( ;; ) 
	{
		if( ReadFromSerialPort( inputStr, 1 ) ) 
		{
			if( inputStr[0] == '#' ) break;

		} else 
		{ 
			fprintf( stderr, "No acknowledgement from telescope in StartSlew.\n" );
		}
	}

	// check how many bytes were sent - todo: also loop for qa limited timeout!
	// perhaps i should implement a timeout and/or check ack function
	if( nWrite != 8 )
		return( 0 );
	else
		return( 1 );
}

// Stop the slew in chosen direction 
////////////////////////////////////////////////////////////////////
void CTelescopeNexStar::StopSlew( int direction )
{
	char slewCmd[] = { 0x50, 0x02, 0x11, 0x24, 0x00, 0x00, 0x00, 0x00 };
	char inputStr[2048];

	if( direction == NORTH )
	{
		slewCmd[2] = 0x11; 
		slewCmd[3] = 0x24;

	} else if( direction == EAST )
	{
		slewCmd[2] = 0x10; 
		slewCmd[3] = 0x24;

	} else if( direction == SOUTH )
	{
		slewCmd[2] = 0x11; 
		slewCmd[3] = 0x24;

	} else if( direction == WEST )
	{
		slewCmd[2] = 0x10; 
		slewCmd[3] = 0x24;
	}

	WriteToSerialPort(slewCmd,8);

	// Look for '#' acknowledgement of request
	for( ;; ) 
	{
		if( ReadFromSerialPort( inputStr, 1 ) ) 
		{
			if (inputStr[0] == '#') break;

		} else 
		{ 
			fprintf( stderr, "No acknowledgement from telescope in StopSlew.\n" );
		}
	}  
}

// Stop Slew in all directions
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::StopSlewAll( )
{  
	if( GetSlewStatus() == 1 )
	{
		StopSlew( NORTH );
		StopSlew( SOUTH );
		StopSlew( EAST );
		StopSlew( WEST );
//		Sleep( 1000 );

		return( 1 );
	}

	return( 0 );
}

// Find the coordinates at which the telescope is pointing
// Correct for the pointing model
////////////////////////////////////////////////////////////////////
void CTelescopeNexStar::GetPosition( double *telra, double *teldec, int pmodel )
{
	char returnStr[12];
	int countRA, countDec;
	double telra0, teldec0, telra1, teldec1;
	int numRead;

	WriteToSerialPort( "E", 1 );
	numRead=ReadFromSerialPort( returnStr, 10 );
	returnStr[4] = returnStr[9] = '\0';  

	// Diagnostic
	// printf( "GetRAandDec: %d read %x\n", numRead,returnStr ); 

	sscanf( returnStr, "%x", &countRA );                  
	sscanf( returnStr+5, "%x:", &countDec );               
	telra0 = (double) countRA;
	telra0 = telra0 / (3. * 15. * 60. * 65536./64800.);
	teldec0 = (double) countDec;
	teldec0 = teldec0 / (3. * 60. * 65536./64800.);


	// Account for the quadrant in declination: 90 to 180 
	if( (teldec0 > 90.) && (teldec0 <= 180.) )
		teldec0 = 180. - teldec0; 

	// :: 180 to 270
	if( (teldec0 > 180.) && (teldec0 <= 270.) )
		teldec0 = teldec0 - 270.;

	// :: 270 to 360 
	if( (teldec0 > 270.) && (teldec0 <= 360.) )
		teldec0 = teldec0 - 360.;

	// Correct the coordinates that are reported by the telescope 
	if( pmodel > 0 )
	{
		// apply correction model
		PointingFromTel( &telra1, &teldec1, telra0, teldec0, pmodel );

		// Return corrected values 

		*telra=telra1;
		*teldec=teldec1;

	} else
	{
		// just set with no correction 
		*telra = telra0;
		*teldec = teldec0;
	}

	return;
}


// Slew to new coordinates 
// Returns 1 if goto command is sent successfully and 0 otherwise
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::GoToCoords( double newRA, double newDec, int pmodel )
{
	double telra0, telra1, teldec0, teldec1;
	int countRA,countDec;
	char r0,r1,r2,r3,d0,d1,d2,d3;
	double degs, hrs;
	char outputStr[32], inputStr[2048];

	// Where do we really want to go to?  
	telra1 = newRA;
	teldec1 = newDec;

	// Where will the telescope point to make this happen?  
	PointingToTel(&telra0,&teldec0,telra1,teldec1,pmodel);

	// Work with the telescope coordinates   
	newRA = telra0;
	newDec = teldec0;

	// Set coordinates 
	hrs = newRA;
	degs = newDec;  

	// Convert float RA to integer count 
	hrs = hrs*(3. * 15. * 60. * 65536./64800.);
	countRA = (int) hrs;


	// Account for the quadrant in declination 
	if ( (newDec >= 0.0) && (newDec <= 90.0) )
	{
		degs = degs*(3. * 60. * 65536./64800.);

	} else if ( (newDec < 0.0) && (newDec >= -90.0) )
	{
		degs = (360. + degs)*(3. * 60. * 65536./64800.);

	} else
	{
		fprintf(stderr,"Invalid newDec in GoToCoords.\n");
		return 0;  
	}

	// Convert float Declination to integer count 
	countDec = (int) degs;

	// Convert each integer count to four HEX characters 
	// Inline coding just to be fast 
	if( countRA < 65536 )
	{
		r0 = countRA % 16;
		if( r0 < 10 )
			r0 = r0 + 48;
		else
			r0 = r0 + 55;
	
		countRA = countRA/16;
		r1 = countRA % 16;
		if( r1 < 10 )
			r1 = r1 + 48;
		else
			r1 = r1 + 55;

		countRA = countRA/16;
		r2 = countRA % 16;
		if( r2 < 10 )
			r2 = r2 + 48;
		else
			r2 = r2 + 55;

		r3 = countRA/16;
		if( r3 < 10 )
			r3 = r3 + 48;
		else
			r3 = r3 + 55;

	} else
	{
		printf("RA count overflow in GoToCoords.\n");
		return 0;
	}

	if( countDec < 65536 )
	{
		d0 = countDec % 16;
		if( d0 < 10 )
			d0 = d0 + 48;
		else
			d0 = d0 + 55;

		countDec = countDec/16;
		d1 = countDec % 16;
		if( d1 < 10 )
			d1 = d1 + 48;
		else
			d1 = d1 + 55;

		countDec = countDec/16;
		d2 = countDec % 16;
		if( d2 < 10 )
			d2 = d2 + 48;
		else
			d2 = d2 + 55;

		d3 = countDec/16;
		if( d3 < 10 )
			d3 = d3 + 48;
		else
			d3 = d3 + 55;

	} else
	{
		fprintf(stderr,"Dec count overflow in GoToCoords.\n");
		return( 0 );
	}
    
  
	// Send the command and characters to the NexStar 
	sprintf(outputStr,"R%c%c%c%c,%c%c%c%c",r3,r2,r1,r0,d3,d2,d1,d0);
	WriteToSerialPort(outputStr,10);

	// Look for '#' in response 
	for( ;; ) 
	{
		if ( ReadFromSerialPort( inputStr, 1 ) ) 
			if (inputStr[0] == '#') break;
		else 
			fprintf(stderr,"No acknowledgement from telescope after GoToCoords.\n");

		return( 0 );
	}

	return( 1 );
}


// Return a flag indicating whether a slew is now in progress 
//   1 -- slew is in progress 
//   0 -- slew not in progress  
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::GetSlewStatus(void)
{
	char inputStr[2048];
	WriteToSerialPort( "L", 1 );

	// Look for '0#' in response indicating goto is not in progress 
	for( ;; ) 
	{
		if( ReadFromSerialPort( inputStr, 2 ) ) 
			if( (inputStr[0] == '0') &&  (inputStr[1] == '#') ) break;
		else 
			return( 1 );
	}

	return( 0 );
}


// Test whether the destination was reached 
// With the NexStar we use the goto in progress query 
// Return value is 
//   0 -- goto in progress
//   1 -- goto complete within tolerance
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::CheckGoTo( double desRA, double desDec, int pmodel )
{
	double telra0, telra1, teldec0, teldec1;
	double errorRA, errorDec, nowRA, nowDec;

	// Where do we really want to go to?  
	telra1 = desRA;
	teldec1 = desDec;

	// Where will the telescope point to make this happen? 
	PointingToTel( &telra0, &teldec0, telra1, teldec1, pmodel );

	// Work with the telescope coordinates 
	desRA = telra0;
	desDec = teldec0;

	if( GetSlewStatus() == 1 ) return( 0 );

	GetPosition( &nowRA, &nowDec, pmodel );
	errorRA = nowRA - desRA;
	errorDec = nowDec - desDec;

	// For 6 minute of arc precision; change as needed. 
	if( fabs(errorRA) > (0.1/15.) || fabs(errorDec) > 0.1 )
		return( 1 );
	else
		return( 2 );
}

// Coordinates and time 
////////////////////////////////////////////////////////////////////
// Synchronize remote telescope to this ra dec pair
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::SyncTelOffsets(double newoffsetra, double newoffsetdec)
{
	offsetra = newoffsetra;
	offsetdec = newoffsetdec;
	return( 0 );
}

// Synchronize remote telescope to this UTC
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::SyncTelToUTC(double newutc)
{
	return( 0 );
}

// Synchronize remote telescope to this observatory location 
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::SyncTelToLocation(double newlong, double newlat, double newalt)
{
	return( 0 );
}

// Synchronize remote telescope to this local sidereal time 
// Overrides UTC and location when implemented 
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::SyncTelToLST(double newlst)
{
	return( 0 );
}

// Start sidereal tracking 
// StartTrack is aware of the latitude and will set the direction accordingly 
// Call StartTrack at least once after the driver has the latitude 
////////////////////////////////////////////////////////////////////
void CTelescopeNexStar::StartTrack( )
{ 
	char slewCmd[] = { 0x50, 0x03, 0x10, 0x06, 0xff, 0x0ff, 0x00, 0x00 };

	// 0x50 is pass through code 
	// 0x03 is number of data bytes including msgId 
	// 0x10 is the destId for the ra drive, or 0x11 for declination 
	// 0x06 is the msgId to set a positive drive rate 
	// 0xff is the first of two data bytes for the sidereal drive rate 
	// 0xff is the second of two data bytes for the sidereal drive rate 
	// 0x00 is a null byte 
	// 0x00 is a request to send no data back other than the # ack 

	char inputStr[2048];
  
	// Test for southern hemisphere 
	// Set negative drive rate if we're south of the equator 
	if( SiteLatitude < 0. ) slewCmd[3] = 0x07;
  
	WriteToSerialPort( slewCmd, 8 );

	// Look for '#' acknowledgement of request 
	for( ;; ) 
	{
		if ( ReadFromSerialPort( inputStr, 1 ) ) 
		{
			if (inputStr[0] == '#') break;

		} else 
		{ 
			fprintf(stderr,"No acknowledgement from telescope in StartTrack.\n");
		}
	}  

} 

// Stop tracking if it is running 
////////////////////////////////////////////////////////////////////
void CTelescopeNexStar::StopTrack( )
{
	char slewCmd[] = { 0x50, 0x03, 0x10, 0x06, 0x00, 0x00, 0x00, 0x00 };

	// 0x50 is pass through code 
	// 0x03 is number of data bytes including msgId 
	// 0x10 is the destId for the ra drive 
	// 0x06 is the msgId to set a positive drive rate 
	// 0x00 is the first of two data bytes for the drive rate 
	// 0x00 is the second of two data bytes for the drive rate 
	// 0x00 is a null byte 
	// 0x00 is a request to send no data back other than the # ack 

	char inputStr[2048]; 

	// Test for southern hemisphere 
	// Set negative drive rate if we're south of the equator 
	if( SiteLatitude < 0. ) slewCmd[3] = 0x07;

	WriteToSerialPort( slewCmd, 8 );

	// Look for a '#' acknowledgement of request
	for( ;; ) 
	{
		if( ReadFromSerialPort( inputStr, 1 ) ) 
		{
			if (inputStr[0] == '#') break;

		} else 
		{ 
			fprintf( stderr, "No acknowledgement from telescope in StopTrack.\n" );
		}
	}
}

// Full stop 
////////////////////////////////////////////////////////////////////
void CTelescopeNexStar::FullStop( )
{  
	if( GetSlewStatus()==1 )
	{
		printf( "NexStar FullStop: stopping N slew.\n" );
		StopSlew( NORTH );
		printf( "NexStar FullStop: stopping S slew.\n" );
		StopSlew( SOUTH );
		printf( "NexStar FullStop: stopping E slew.\n" );
		StopSlew( EAST );
		printf( "NexStar FullStop: stopping W slew.\n" );
		StopSlew( WEST );
		Sleep( 1000 );
	}

	printf( "NexStar: stopping sidereal tracking.\n" );
	StopTrack();
}

// Set slew limits control off or on 
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::SetLimits( int limits )
{
	char inputStr[2048];
	int b0;
	char limitCmd[] = { 0x50, 0x02, 0x10, 0xef, 0x00, 0x00, 0x00, 0x00 };

	// 0x50 is pass through code 
	// 0x02 is number of data bytes including msgId 
	// 0x10 is the destId for the RA drive 
	// 0xef is the msgId to set hardstop limits state 
	// 0x00 is the limits control data byte 
	// 0x00 is the second null data byte 
	// 0x00 is the third null data byte 
	// 0x00 is a request to send no data back other than the # ack 

	if( limits == TRUE )
	{
		limitCmd[4] = 0x01;  
		fprintf( stderr, "Limits enabled\n" ); 

	} else
	{
		fprintf( stderr, "Limits disabled\n" );   
	}

	// Send the command 
	WriteToSerialPort( limitCmd, 8 );

	// Wait for an acknowledgement 
	b0 = 1;

	for( ;; ) 
	{
		if( ReadFromSerialPort( inputStr, 1 ) ) 
		{
			if( inputStr[0] == '#' ) 
			{
				b0 = 0;
				break;
			}  
		}
	} 

	return( b0 );
}

// Get status of slew limits control
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::GetLimits( int *limits )
{
	char inputStr[2048];
	int b0, b1;
	char limitCmd[] = { 0x50, 0x01, 0x10, 0xee, 0x00, 0x00, 0x00, 0x01 };

	// 0x50 is pass through code 
	// 0x01 is number of data bytes including msgId 
	// 0x10 is the destId for the RA drive 
	// 0xee is the msgId to get hardstop limit state 
	// 0x00 is the first null data byte 
	// 0x00 is the second null data byte 
	// 0x00 is the third null data byte 
	// 0x01 is a request to send one byte data and # ack 
	 
	// Send the command 
	WriteToSerialPort( limitCmd, 8 );

	// Read a response 
	ReadFromSerialPort( inputStr, 2 );

	// Mask the bytes 
	b0 = (0x000EF & inputStr[0]);

	if( b0 == 1 )
		*limits = 1;
	else
		*limits = 0;

	if( inputStr[1] == '#' )
		b1 = 0;
	else
		b1 = 1;

	return( b1 );
}
 
// Set slew speed limited by MAXSLEWRATE 
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::SetSlewRate( int slewRate )
{
  fprintf(stderr,"NexStar does not support remote setting of slew rate.\n");
  return 0;
}

// Control the dew heater 
////////////////////////////////////////////////////////////////////
void CTelescopeNexStar::Heater( int heatercmd )
{
}

// Control the fan 
////////////////////////////////////////////////////////////////////
void CTelescopeNexStar::Fan( int fancmd )
{
}

// Adjust the focus 
////////////////////////////////////////////////////////////////////
void CTelescopeNexStar::Focus( int focuscmd, int focusspd )
{
	char focusCmd[] = { 0x50, 0x02, 0x12, 0x24, 0x01, 0x00, 0x00, 0x00 };
	char inputStr[2048];
	static int focusflag;
	static double focustime;

	if( focuscmd == FOCUSCMDIN )
	{
		// Run the focus motor in 
		focusflag = -1;
		focustime = UTNow();

		// Set the direction 
		focusCmd[3] = 0x25;

		// Set the speed  but enforce the speed limit first 
		if( focusspd > FOCUSSPDMAX )
			focusCmd[4] = 0x08;
		else if( focusspd == FOCUSSPDFAST )
			focusCmd[4] = 0x04;
		else if(focusspd == FOCUSSPDSLOW )
			focusCmd[4] = 0x02;
		else if( focusspd == FOCUSSPDMIN )
			focusCmd[4] = 0x01;
		else 
			focusCmd[4] = 0x01;

		// Send the command 
		WriteToSerialPort( focusCmd, 8 );

		// Wait up to a second for an acknowledgement 
		for( ;; ) 
		{
			if( ReadFromSerialPort( inputStr, 1 ) ) 
			{
				if( inputStr[0] == '#' ) break;

			} else 
			{ 
				fprintf( stderr, "No acknowledgement from focus in.\n" );
			}
		}      
	}

	if( focuscmd == FOCUSCMDOUT )
	{  
		// Run the focus motor out
		focusflag = 1;
		focustime = UTNow();

		// Set the direction
		focusCmd[3] = 0x24;

		// Set the speed  but enforce the speed limit first
		if( focusspd > FOCUSSPDMAX )
			focusspd = FOCUSSPDMAX;    

		focusCmd[4] = (char) focusspd;

		// Send the command 
		WriteToSerialPort( focusCmd, 8 );    

		// Wait up to a second for an acknowledgement
		for( ;; ) 
		{
			if( ReadFromSerialPort( inputStr, 1 ) ) 
			{
				if( inputStr[0] == '#' ) break;

			} else 
			{ 
				fprintf( stderr, "No acknowledgement from focus out.\n" );
			}
		}      
	}

	if( focuscmd == FOCUSCMDOFF )
	{
		if( focusflag != 0 )
		{
			// Focus is in motion so we note the time and then stop it 
			// Keep track of count as number of seconds at slowest speed 
			focustime = UTNow() - focustime;

			// Set the speed to zero to stop the motion 
			focusCmd[4] = 0x00;

			// Send the command
			WriteToSerialPort(focusCmd,8);    

			// Wait up to a second for an acknowledgement 
			for( ;; ) 
			{
				if( ReadFromSerialPort( inputStr, 1 ) ) 
				{
					if( inputStr[0] == '#' ) break;

				} else 
				{ 
					fprintf(stderr,"No acknowledgement from focus stop.\n");
				}
			}   

			// Just in case the UT clock rolled over one day while focusing 
			// Try to trap the error but ... 
			// for very short focustime roundoff error may give negative
			// values instead of zero.  We trap those cases.
			if( focustime < 0. )
			{
				if( focustime < -23.9 ) // larry: crap ?
				{
					focustime = focustime + 24.;
				}
				focustime = 0.;
			}

			// Count time in tenths of seconds to get the focus change
			// This is to give more feedback to the observer but
			// UTNow does not pick up fractions of a second so least count is 10
			focustime = focustime*36000.;

			if( focusflag == -1 )
				m_nFocusCount = m_nFocusCount - focusspd * ( (int) focustime);

			if( focusflag == 1 )
				m_nFocusCount = m_nFocusCount + focusspd * ( (int) focustime);

			focusflag = 0;
		}

		if( focusflag == 0 )
		{
			// Focus motion is already stopped so there is nothing to do
		}  
	}    
}


// Report the current focus count 
// Use the readout from the encoder if it is available 
// Otherwise use the timed focuscount saved in Focus() 
////////////////////////////////////////////////////////////////////
void CTelescopeNexStar::GetFocus( double *telfocus )
{
	// Focus motor device ID 0x12 
	// MC_GET_POSITION is 0x01 
	// Bytes in response are 0x03 
	char focusCmd[] = { 0x50, 0x01, 0x12, 0x01, 0x00, 0x00, 0x00, 0x03 };
	char inputStr[2048];  
	int b0,b1,b2;

	// Send the command 
	WriteToSerialPort( focusCmd, 8 );    

	// Read a response
	ReadFromSerialPort( inputStr, 4 );

	// Mask the high order bytes 
	b0 = (0x000EF & inputStr[2]);
	b1 = (0x000EF & inputStr[1]);
	b2 = (0x000EF & inputStr[0]);

	// count = 256*256*b2 + 256*b1 + b0 

	// On our CDK20 focuser one count in b2 is approximately 9 microns 
	// Therefore one count corresponds to 1 pixel at f/1 
	// At f/6.8 on the CDK20 it takes about 6.8 b2 counts to defocus 1 pixel 
	// Only b2 is useful 

	// Focuser diagnostic -- comment out except for testing 

	// printf("Focus encoder b2=%d  b1=%d  b0=%d \n", b2,b1,b0); 
	// printf ("Focus timer count=%d\n", focuscount); 

	*telfocus = (double) b2;
}

// Set focus speep from 1..9 by larry
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::SetFocusSpeed( int nSpeed )
{
	Focus( FOCUSCMDOFF, nSpeed );
	return( 1 );
}

// start to focus in
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::StartFocusIn( long nSpeed )
{
	Focus( FOCUSCMDIN, FOCUSSPDSLOW );
	return( 1 );
}

// start to focus out
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::StartFocusOut( long nSpeed )
{
	Focus( FOCUSCMDOUT, FOCUSSPDSLOW );
	return( 1 );
}

// stop my focuser
//////////////////////////////////////////////////
int CTelescopeNexStar::StopFocus( )
{
	Focus( FOCUSCMDOFF, 0 );
	return( 1 );
}

// move focus in with n time units ... in this case miliseconds
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::FocusIn( long nUnits )
{
	Focus( FOCUSCMDIN, FOCUSSPDSLOW );
	Sleep( nUnits );
	Focus( FOCUSCMDOFF, 0 );
	return( 1 );
}

// move focus out with n time units ... in this case miliseconds
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::FocusOut( long nUnits )
{
	Focus( FOCUSCMDOUT, FOCUSSPDSLOW );
	Sleep( nUnits );
	Focus( FOCUSCMDOFF, 0 );
	return( 1 );
}

// move at absolute position 
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::FocusAt( long nUnits )
{
	// todo: implement
	return( 1 );
}

// Adjust the rotation 
////////////////////////////////////////////////////////////////////
void CTelescopeNexStar::Rotate(int rotatecmd, int rotatespd)
{
}

// Report the rotation setting 
////////////////////////////////////////////////////////////////////
void CTelescopeNexStar::GetRotate(double *telrotate)
{
}

// Report the temperature 
////////////////////////////////////////////////////////////////////
void CTelescopeNexStar::GetTemperature(double *teltemperature)
{
}

// Time synchronization utilities 
////////////////////////////////////////////////////////////////////
// Reset the telescope sidereal time 
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::SyncLST( double newTime )
{		
	fprintf(stderr,"NexStar does not support remote setting of sidereal time.\n");
	return( -1 );
}


// Reset the telescope local time 
////////////////////////////////////////////////////////////////////
int CTelescopeNexStar::SyncLocalTime()
{
	fprintf(stderr,"NexStar does not support remote setting of local time.\n");
	return( -1 );
}
