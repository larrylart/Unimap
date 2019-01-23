////////////////////////////////////////////////////////////////////
// Telescope Control Protocol for the Meade LX200 
//
// Purpose:		meade lx200 control driver
//
// Created by:	Larry Lart on 20/09/2009
// Updated by:	
//
// Copyright:	(c) 2004-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

// system
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
#include "telescope_lx200.h"

////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////
CTelescopeLx200::CTelescopeLx200( ) : CTelescopeDriver( )
{

}

////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////
CTelescopeLx200::~CTelescopeLx200( )
{

}

// Purpose:	Connect to the telescope serial interface One port is 
//			used by the lx200
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::Connect( )
{
	if( m_bTelConnectFlag != 0 ) return( 1 );
	
	char vectRead[255];
	int nRead, nWrite=0;

	// open serial port
	if( !m_pSerialPort->Open( m_nSerialPortNo, m_nSerialPortBaud ) ) 
		return( 0 );

	// send a # to clean up
	nWrite = WriteToSerialPort( "#", 1 );

	// ask for local time to test connection
	nWrite = WriteToSerialPort( "#:GL#", 5 );
	nRead = ReadFromSerialPort( vectRead, 9 );
	vectRead[nRead] = '\0';

	// diagnostic test of local time 
	if( nRead == 9 ) 
	{
		m_bTelConnectFlag = 1;

		// Check precision on RA and toggle to high precision if needed
		nWrite = WriteToSerialPort( "#:GR#", 5 );
		nRead = ReadFromSerialPort( vectRead, 9 );       
		if( nRead < 9 )
		{
			nWrite = WriteToSerialPort( "#:U#", 4 );
			Sleep( 250 );
		}            
	}  

	return( m_bTelConnectFlag );
}

////////////////////////////////////////////////////////////////////
void CTelescopeLx200::Disconnect( )
{
	// disconect serial port
	if( m_bTelConnectFlag == 1 ) m_pSerialPort->Close( );

	m_bTelConnectFlag = 0;
}

// Purpose:	Set the speed for a motion command 
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::SetRate( int newRate )
{
	int nWrite=0;

	if( newRate == SLEW ) 
		nWrite = WriteToSerialPort( "#:RS#", 5 );
	else if( newRate == FIND ) 
		nWrite = WriteToSerialPort( "#:RM#", 5 );
	else if( newRate == CENTER ) 
		nWrite = WriteToSerialPort( "#:RC#", 5 );
	else if( newRate == GUIDE ) 
		nWrite = WriteToSerialPort( "#:RG#", 5 );

	// check all ok
	if( nWrite != 5 )
		return( 0 );
	else
		return( 1 );
}

////////////////////////////////////////////////////////////////////
int CTelescopeLx200::StartSlew( int direction )
{
	int nWrite=0;

	if( direction == NORTH )
		nWrite = WriteToSerialPort( "#:Mn#", 5 );
	else if( direction == EAST )
		nWrite = WriteToSerialPort( "#:Me#", 5 );
	else if( direction == SOUTH )
		nWrite = WriteToSerialPort( "#:Ms#", 5 );
	else if( direction == WEST )
		nWrite = WriteToSerialPort( "#:Mw#", 5 );

	// check all ok
	if( nWrite != 5 )
		return( 0 );
	else
		return( 1 );
}

////////////////////////////////////////////////////////////////////
void CTelescopeLx200::StopSlew( int direction )
{
  if( direction == NORTH )
    WriteToSerialPort( "#:Qn#", 5 );
  else if( direction == EAST )
    WriteToSerialPort( "#:Qe#", 5 );
  else if( direction == SOUTH )
    WriteToSerialPort( "#:Qs#", 5 );
  else if( direction == WEST )
    WriteToSerialPort( "#:Qw#", 5 );
}

// Stop all motion 
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::StopNSEW( )
{
	int nCount=0;

	nCount += WriteToSerialPort( "#:Qn#", 5 );
	nCount += WriteToSerialPort( "#:Qe#", 5 );
	nCount += WriteToSerialPort( "#:Qs#", 5 );
	nCount += WriteToSerialPort( "#:Qw#", 5 );

	if( nCount != 20 )
		return( 0 );
	else
		return( 1 );
}

// Read the telescope right ascension 
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::GetRA( double& nRa )
{
	char  returnStr[255];
	double raFMin=0;
	int raHr=0, raMin=0, raSec=0;
	int  numRead;

//	WriteToSerialPort( "#", 1 );
//	numRead=ReadFromSerialPort( returnStr, 1 );
	FlushSerialPort();

	WriteToSerialPort( "#:GR#", 5 );
	Sleep( 70 );
	numRead=ReadFromSerialPort( returnStr, 9 );

	// Most LX200's support high precision. ConnectTel tests the precision and 
	// tries to toggle to high precision. Low precision mode is not supported 
	// without modifying the following.
//	returnStr[8] = '\0';  
	nRa = 0.0;
	if( numRead > 3 )
	{
		if( sscanf( returnStr, "%d%*c%d%*c%d#", &raHr, &raMin, &raSec ) )                 
			nRa = (double)raHr*15.0 + (double)raMin/4.0 + (double)raSec/240.0;
		else if( sscanf( returnStr, "%d:%lf#", &raHr, &raFMin ) ) 
			nRa = (double)raHr*15.0 + raFMin/4.0;
		else
			return( 0 );

	} else
		return( 0 );

	return( 1 );
}

// Purpose:	Read the telescope declination
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::GetDec( double& nDec )
{
	char returnStr[255];
	int decDeg=0, decMin=0, decSec=0;
	double decFMin=0.0;
	int numRead;

	FlushSerialPort();

	WriteToSerialPort( "#:GD#",5);
	Sleep( 70 );
	numRead=ReadFromSerialPort( returnStr, 10 );

	// Most LX200's support high precision, so ConnectTel tests the precision 
	// and tries to toggle to high precision. Low precision mode is not 
	// supported without modifying the following. Since the LX200 may return 
	// something other than ':' as a delimiter, we parse each piece. 
//	returnStr[3] = returnStr[6] = returnStr[9]='\0';  

	nDec = 0.0;
	if( numRead > 3 )
	{
/*		sscanf( returnStr, "%d", &decDeg );
		sscanf( returnStr+4, "%d:", &decMin );
		sscanf( returnStr+7, "%d", &decSec );
		nDec = (double) fabs(decDeg) + decMin/60.0 + decSec/3600.0;
		if( returnStr[0] == '-' ) nDec *= -1.;
*/
		if( sscanf( returnStr, "%d%*[^0-9]%d%*[^0-9]%d#", &decDeg, &decMin, &decSec ) )
			nDec = (double) abs(decDeg) + (double)decMin/60.0 + (double)decSec/3600.0;
		else if( sscanf( returnStr, "%d%*[^0-9]%d#", &decDeg, &decMin, &decSec ) )
			nDec = (double) abs(decDeg) + (double)decMin/60.0;
		else
			return(0);
		// adjust sign
		if( returnStr[0] == '-' ) nDec *= -1.;

	} else
		return(0);

	return( 1 );
}

// Purpose:	Find the coordinates at which the telescope is pointing
////////////////////////////////////////////////////////////////////
void CTelescopeLx200::GetPosition( double *telra, double *teldec, int pmodel )
{
	double telra0=0.0, teldec0=0.0, telra1=0.0, teldec1=0.0;

	// Read the telescope RA 
	if( !GetRA( telra0 ) ) return;
//	telra0 = GetRA();

	// Read the telescope Dec 
	if( !GetDec( teldec0 ) ) return;
//	teldec0 = GetDec();

	// Correct the coordinates that are reported by the telescope 
	if( pmodel > 0 )
	{
		// apply correction model
		PointingFromTel( &telra1, &teldec1, telra0, teldec0, pmodel );

		// set corrected values 
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

////////////////////////////////////////////////////////////////////
int CTelescopeLx200::GoToCoords( double newRA, double newDec, int pmodel )
{
	double telra0=0.0, telra1=0.0, teldec0=0.0, teldec1=0.0;
	int raHr=0, raMin=0, raSec=0;
	int decDeg=0, decMin=0, decSec=0;
	double dms=0.0, hms=0.0, ms=0.0;
	char outputStr[32], inputStr[2048];
	int returnVal=0;

	// Where do we really want to go to?
	telra1 = newRA;
	teldec1 = newDec;

	// Where will the telescope point to make this happen? 
	if( pmodel > 0 )
		PointingToTel( &telra0, &teldec0, telra1, teldec1, pmodel );
	else
	{
		telra0 = telra1;
		teldec0 = teldec1;
	}

	// Work with the telescope coordinates 
	newRA = telra0;
	newDec = teldec0;

	// In high precision mode, we must send hh:mm:ss and dd:mm:ss 
	// Allow for truncation if input is in seconds of arc 
/*	hms = newRA;
	hms = hms + 0.5/3600.;
	raHr = (int) hms;
	ms = hms - (double) raHr;
	ms = ms*60.;
	raMin = (int) ms;
	raSec = (int) 60.*(ms - (double) raMin);
*/
	double ival = 0.0;
	int nLVal = 0;

	ival = newRA;
	if( ival < 0.) ival += 360.;
	if( ival > 360.) ival -= 360.;
	ival /= 15.;

	nLVal = (long) ( ival*(60. * 60. * 1000. + 0.001 ) ) ; //+ 0.5; // +5 or +50 ?

	///////////////////////////
	// MOVE ON RA
	FlushSerialPort();
	sprintf( outputStr, "#:Sr%02d:%02d:%02d#\n", nLVal / 3600000L,
			(nLVal / 60000) % 60L, (nLVal / 1000) % 60L );
	WriteToSerialPort( outputStr, 15 );
	Sleep(70);
	if( ReadFromSerialPort( inputStr, 1 ) != 1 ) return( 0 ); 

	if( inputStr[0] != '1' )
	{
		fprintf( stderr, "Error setting object RA in GoToCoords!\n" );
		fprintf( stderr, "Tried to send command string %s\n", outputStr );
		fprintf( stderr, "Received string %s\n", inputStr ); 
	}

	//////////
	// calculate dec
/*	if( newDec >= 0.0 )
	{
		dms = newDec;
		dms = dms + 0.5/3600.;
		decDeg = (int) dms;
		ms = dms - (double) decDeg;
		ms = ms*60.;
		decMin = (int) ms;
		decSec = (int) 60.*(ms - (double) decMin);
		sprintf( outputStr, "#:Sd+%02d*%02d:%02d#\n", decDeg, decMin, decSec );

	} else
	{
		dms = -newDec;
		dms = dms + 0.5/3600.;
		decDeg = (int) dms;
		ms = dms - (double) decDeg;
		ms = ms*60.;
		decMin = (int) ms;
		decSec = (int) 60.*(ms - (double) decMin);
		sprintf( outputStr, "#:Sd-%02d*%02d:%02d#\n", decDeg, decMin, decSec );
	}
*/
	char chSign = '+';
	nLVal = 0;
	ival = newDec;
	if( ival < 0 )
	{
		chSign = '-';
		ival = -ival;
	}

	nLVal = (long) ( ival * (60. * 60. * 1000. + 0.001) ); //+ 0.5; // +5 or +50 ?
	sprintf( outputStr, "#:Sd%c%02d*%02d:%02d#\n", chSign, nLVal / 3600000L,
			(nLVal / 60000) % 60L, (nLVal / 1000) % 60L );

	///////////////////////////
	// MOVE ON DEC
	FlushSerialPort();
	WriteToSerialPort( outputStr, 16 );
	Sleep(70);
	if( ReadFromSerialPort( inputStr, 1 ) != 1 ) 
	{
		Disconnect( );
		return( -1 );
	}

	if( inputStr[0] != '1' ) 
	{
		fprintf( stderr, "Error setting object Dec in GoToCoords!\n" );
		fprintf( stderr, "Tried to send command string %s\n", outputStr );
		fprintf( stderr, "Received string %s\n", inputStr );
	}

	FlushSerialPort();
	WriteToSerialPort( "#:MS#",5 );
	Sleep(70);
	if( ReadFromSerialPort( inputStr, 1 ) != 1 ) 
	{
		Disconnect( );
		return( -1 );
	}

	switch( inputStr[0] ) 
	{
		case '0':
		{
			return 0;
			break;
		}
		case '1':
		{
			returnVal = 1; 
			fprintf( stderr, "Object is below horizon in GoToCoords.\n");
			break;
		}
		case '2':
		{
			returnVal = 2;
			fprintf( stderr, "Declination is above upper limit in GoToCoords.\n" );
			break;
		}
		default:
		{
			fprintf( stderr, "Unexpected return from command #:MS#\n" );
			return -1;
			break;
		}
	}

	// Look for '#'
	for( ;; ) 
	{
		if( ReadFromSerialPort( inputStr, 1 ) ) 
		{
			if (inputStr[0] == '#') break;
		} else 
			fprintf( stderr, "No acknowledgement from telescope after GoToCoords.\n" );

		return( -1 );
	}

	return( returnVal );
}

// Purpose:	Test whether the destination was reached 
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::CheckGoTo( double desRA, double desDec, int pmodel )
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

	// todo: here check if error on get ra/dec
	GetRA( nowRA );
	errorRA = nowRA - desRA;
	GetDec( nowDec );
	errorDec = nowDec - desDec;

	// For 2 minute of arc precision 
	//if( fabs(raError) > 1.0/(15.0*30.0) || fabs(decError) > 1.0/30.0)

	// For 6 minute of arc precision 
	if( fabs(errorRA) > (0.1/15.) || fabs(errorDec) > 0.1 )
		return( 0 );
	else
		return( 1 );
}

// Purpose:	Synchronize remote telescope to this ra dec pair 
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::SyncTelOffsets( double newoffsetra, double newoffsetdec )
{
	offsetra = newoffsetra;
	offsetdec = newoffsetdec;

	return( 0 );
}

// Set lower and upper RA limits to protect hardware
////////////////////////////////////////////////////////////////////
void CTelescopeLx200::SetRALimits( double limitLower, double limitHigher )
{

}

// Set lower and upper dec limits to protect hardware
////////////////////////////////////////////////////////////////////
void CTelescopeLx200::SetDecLimits( double limitLower, double limitHigher )
{
	int min, max;
	char outputStr[32], inputStr[2048];

	min = (int) limitLower;
	max = (int) limitHigher;

	sprintf( outputStr, "#:So %02d#\n", min );
	WriteToSerialPort( outputStr, 9 );

	if (ReadFromSerialPort( inputStr, 1 ) != 1 ) 
	{
		fprintf(stderr,"Unable to set Dec lower limit\n");
	}  

	sprintf( outputStr, "#:Sh %02d#\n", max );
	WriteToSerialPort( outputStr, 8 );

	if (ReadFromSerialPort( inputStr, 1 ) != 1 ) 
	{
		fprintf(stderr,"Unable to set Dec upper limit\n");
	}  
}

// Set slew speed limited by MAXSLEWRATE 
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::SetSlewRate( int slewRate )
{
	char outputStr[32], inputStr[2048];

	if( slewRate > 1 && slewRate <= MAXSLEWRATE )
	{
		sprintf( outputStr,"#:Sw %1d#\n", slewRate );
		WriteToSerialPort( outputStr, 8 );

		if( ReadFromSerialPort( inputStr, 1 ) != 1 )
		{
			fprintf( stderr, "Error: no response to set Slew Rate to %1d deg/sec.\n", slewRate );

		} else
		{
			printf( "Slew Rate set to %1d deg/sec\n", slewRate );
			return( 1 );
		}
	}

	printf( "Invalid Slew Rate: %1d deg/sec, must be 2 to %1d deg/sec \n", slewRate, MAXSLEWRATE );
	return( 0 );
}

// Control the reticle function using predefined values 
////////////////////////////////////////////////////////////////////
void CTelescopeLx200::Reticle( int reticle )
{
	if( reticle == BRIGHTER) WriteToSerialPort( "#:B+#", 5 );
	else if( reticle == DIMMER ) WriteToSerialPort( "#:B-#", 5 );
	else if( reticle == BLINK0 ) WriteToSerialPort( "#:B0#", 5 );  
	else if( reticle == BLINK1 ) WriteToSerialPort( "#:B1#", 5 );     
	else if( reticle == BLINK2 ) WriteToSerialPort( "#:B2#", 5 ); 
	else if( reticle == BLINK3 ) WriteToSerialPort( "#:B3#", 5 );
}

////////////////////////////////////////////////////////////////////
// Run the focus using focuscmd 
// The LX200 has fast and slow defined, so we have only two states 
// Focus count is maintained by timing because the LX200 does not encode focus 
////////////////////////////////////////////////////////////////////
void CTelescopeLx200::Focus( int focuscmd, int focusspd )
{
	static int focusflag;
	static double focustime;

	// moves away from sky - focus out
	if( focuscmd == FOCUSCMDOUT ) 
	{
		focusflag = 1;
		focustime = UTNow();

		if( focusspd > FOCUSSPDSLOW )
			WriteToSerialPort( "#:FF#", 5 ); 
		else
			WriteToSerialPort( "#:FS#", 5 );
 
		WriteToSerialPort( "#:F+#", 5 );

		return;
	}

	// moves toward the sky - foucus is
	if( focuscmd == FOCUSCMDIN )
	{
		focusflag = -1;
		focustime = UTNow();

		if( focusspd > FOCUSSPDSLOW )
			WriteToSerialPort( "#:FF#", 5 ); 
		else
			WriteToSerialPort( "#:FS#", 5 );

		WriteToSerialPort( "#:F-#", 5 );

		return;
	}  

	// does not move - stop ?
	if( focuscmd == FOCUSCMDOFF ) 
	{  
		if( focusflag != 0 )
		{
			// Focus is in motion so we note the time and then stop it
			// Keep track of count as number of seconds at slowest speed 
			focustime = UTNow() - focustime;

			// Send the command   
			WriteToSerialPort( "#:F0#", 5 ) ;

			// Just in case the UT clock rolled over one day while focusing 
			// Try to trap the error but ...  
			//   for very short focustime roundoff error may give negative  
			//   values instead of zero.  We trap those cases. 
			if( focustime < 0. )
			{
				// larry: again this doesn't make much sens
				if( focustime < -23.9 ) focustime = focustime + 24.;
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

// simpler focuser command 
//////////////////////////////////////////////////
void CTelescopeLx200::FocusMove( int nCmd )
{
	if( nCmd == FOCUSCMDOUT )
		WriteToSerialPort( "#:F+#", 5 );
	else if( nCmd == FOCUSCMDIN )
		WriteToSerialPort( "#:F-#", 5 );
	if( nCmd == FOCUSCMDOFF )
		WriteToSerialPort( "#:F0#", 5 ) ;
}

// Report the focus  setting
////////////////////////////////////////////////////////////////////
void CTelescopeLx200::GetFocus( double *telfocus )
{ 
  *telfocus = (double) m_nFocusCount;
}

// Set focus speep from 1..9 by larry
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::SetFocusSpeed( int nSpeed )
{
	char vectCmd[10] = ":F1#";
//	char strMsg[255];

	if( nSpeed >= 1 && nSpeed <= 9 && nSpeed != m_nFocuserSpeed )
	{
		// set speed command
		m_nFocuserSpeed = nSpeed;
		vectCmd[2] = '0' + nSpeed;
		// send to serial port
		WriteToSerialPort( vectCmd, 4 );

		// debug logging
//		if( m_pLogger ) 
//		{
//			sprintf( strMsg, "INFO :: set speed=%d.", nSpeed );
//			m_pLogger->Log( strMsg );
//		}
		return( 1 );

	} else if( nSpeed >= 1 && nSpeed <= 9 )
	{
		// debug logging
//		if( m_pLogger ) 
//		{
//			sprintf( strMsg, "ERROR :: speed=%d is out of 1-9 range.", nSpeed );
//			m_pLogger->Log( strMsg, GUI_LOGGER_ATTENTION_STYLE );
//		}
		return( 0 );

	} else
		return( 0 );
}

// start to focus in
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::StartFocusIn( long nSpeed )
{
	// check if need speed adjustment
	if( nSpeed > 0 ) SetFocusSpeed( (int) nSpeed );

	FocusMove( FOCUSCMDIN );
	return( 1 );
}

// start to focus out
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::StartFocusOut( long nSpeed )
{
	// check if need speed adjustment
	if( nSpeed > 0 ) SetFocusSpeed( (int) nSpeed );

	FocusMove( FOCUSCMDOUT );
	return( 1 );
}

// stop my focuser
//////////////////////////////////////////////////
int CTelescopeLx200::StopFocus( )
{
	WriteToSerialPort( "#:FQ#", 5 ) ;
	return( 1 );
}

// move focus in with n time units ... in this case miliseconds
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::FocusIn( long nUnits )
{
	FocusMove( FOCUSCMDIN );
	Sleep( nUnits );
	StopFocus( );
	return( 1 );
}

// move focus out with n time units ... in this case miliseconds
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::FocusOut( long nUnits )
{
	FocusMove( FOCUSCMDOUT );
	Sleep( nUnits );
	StopFocus( );
	return( 1 );
}

// move at absolute position 
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::FocusAt( long nUnits )
{
	// todo: implement
	return( 1 );
}

// Adjust the rotation 
// LX200 rotator compensates for sidereal motion in an alt-az mounting 
////////////////////////////////////////////////////////////////////
void CTelescopeLx200::Rotate( int rotatecmd, int rotatespd )
{
	if( rotatecmd != ROTATECMDOFF) 
	{
		if( rotatespd == ROTATESPDSIDEREAL )
			WriteToSerialPort( "#:r+#", 5 );

	} else if( rotatecmd == ROTATECMDOFF ) 
	{
		WriteToSerialPort( "#:r-#", 5 );
	}
}

// Control the fan 
// LX200 fan has only one speed 
////////////////////////////////////////////////////////////////////
void CTelescopeLx200::Fan( int fancmd )
{
	if( fancmd != FANCMDOFF ) 
		WriteToSerialPort( "#:f+#", 5 );
	else if( fancmd == FANCMDOFF ) 
		WriteToSerialPort( "#:f-#", 5 );
}

// :: Time synchronization utilities from Peter McCullough 
////////////////////////////////////////////////////////////////////
// Reset the telescope sidereal time 
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::SyncTelToLST( double newTime )
{		
	int Hr, Min, Sec;
	double hms, ms;
	char outputStr[32], inputStr[2048];
	int numRead;

	hms = newTime;
	hms = hms + 0.5/3600.;
	Hr = (int) hms;
	ms = hms - (double) Hr;
	ms = ms*60.;
	Min = (int) ms;
	Sec = (int) 60.*(ms - (double) Min);

	sprintf( outputStr, "#:SS %02d:%02d:%02d#\n", Hr, Min, Sec );

	WriteToSerialPort( outputStr, 15 );
	if( ReadFromSerialPort( inputStr, 1 ) != 1 )
	{ 
		fprintf( stderr, "Telescope not responding to set LST in SyncLST.\n" ); 
		return( -1 );

	} else
	{
		printf( "LST on telescope is now set to %s", outputStr );
	}

	// ask for local date 
	WriteToSerialPort( "#:GC#", 5 );
	numRead=ReadFromSerialPort( inputStr, 9 );
	// inputStr should = 'MM/DD/YY#' 
	if( numRead == 9 )  
	{
		inputStr[numRead] = '\0';
		printf( "FYI, Local date on telescope is %s\n", inputStr );

		return( 0 );

	} else
		return( -1 );
}

// Reset the telescope local time from the computer system 
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::SyncTelToLocalTime( )
{
	time_t tp;
	struct tm *ptr;
	char outputStr[32], inputStr[2048];
	int numRead;

	if( !time(&tp) )
	{
		fprintf( stderr,"System not providing local time from PC in SyncLocalTime.\n" ); 
		return -1;
	}

	ptr = localtime( &tp );
	strftime( outputStr, 25, "#:SL %H:%M:%S#\n", ptr );

	WriteToSerialPort( outputStr, 15 );
	if( ReadFromSerialPort( inputStr, 1 ) != 1 )
	{ 
		fprintf( stderr, "Telescope not responding to set local ime in SyncLocalTime\n" ); 
		return( -1 );

	} else
	{
		printf( "Reset local time on telescope to %s", outputStr );
	}

	// ask for local date 
	WriteToSerialPort( "#:GC#", 5 );
	numRead=ReadFromSerialPort( inputStr, 9 );
	// inputStr should = 'MM/DD/YY#' 
	if( numRead == 9 )  
	{
		inputStr[numRead] = '\0';
		printf( "FYI, Local date on telescope is %s\n", inputStr );
		return( 0 );

	} else
		return( -1 );
}

// Tracking on/off utilities 
////////////////////////////////////////////////////////////////////
// Purpose:	Start tracking if it is not on 
////////////////////////////////////////////////////////////////////
void CTelescopeLx200::StartTrack( )
{
}

// Stop tracking if it is on 
////////////////////////////////////////////////////////////////////
void CTelescopeLx200::StopTrack( )
{
}

// Stop all motion 
////////////////////////////////////////////////////////////////////
void CTelescopeLx200::FullStop( )
{
}

// LX200 UNSUPPORTED FUNCTIONS
////////////////////////////////////////////////////////////////////
// Purpose:	Return a flag indicating whether a slew is now in progress 
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::GetSlewStatus( )
{
	// Always return a no-slew state
	return (0);
}

// Set slew limits control off or on 
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::SetLimits( int limits )
{
	return( 0 );
}

// Get status of slew limits control 
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::GetLimits( int *limits )
{
	*limits = 0;
	return( 0 );
}

// Synchronize remote telescope to this UTC 
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::SyncTelToUTC( double newutc )
{
	return( 0 );
}

// Synchronize remote telescope to this observatory location
////////////////////////////////////////////////////////////////////
int CTelescopeLx200::SyncTelToLocation( double newlong, double newlat, double newalt )
{
  return( 0 );
}

// Purpose:	Control the dew heater 
////////////////////////////////////////////////////////////////////
void CTelescopeLx200::Heater( int heatercmd )
{
}

// Report the rotation setting
////////////////////////////////////////////////////////////////////
void CTelescopeLx200::GetRotate( double *telrotate )
{
}

// Purpose:	Report the temperature 
////////////////////////////////////////////////////////////////////
void CTelescopeLx200::GetTemperature( double *teltemperature )
{
}
