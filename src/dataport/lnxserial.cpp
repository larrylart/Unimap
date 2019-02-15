////////////////////////////////////////////////////////////////////
// Name:		linux serial port implementation
// File:		lnxserial.cpp
// Purpose:		to process unix serial port data
//
// Created by:	Unknown
// Updated by:  Larry Lart on 12-Apr-2003
//				Larry Lart on 27-Apr-2003
//
////////////////////////////////////////////////////////////////////

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "lnxserial.h"

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSerial
// Purose:	build my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CSerial::CSerial( )
{
	m_bOpened = 0;
	m_pFd = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSerial
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CSerial::~CSerial( )
{

	Close( );

}

////////////////////////////////////////////////////////////////////
// Method:	Open
// Class:	CSerial
// Purose:	Open a serial port
// Input:	int port and boud rate
// Output:	if port was opened or not
////////////////////////////////////////////////////////////////////
int CSerial::Open( int nPort, int nBaud )
{
	char strPort[100];
	struct termios new_termios, old_termios;
	int term_stat = 0, status;
	int writestat;

	// check if port is already open
	if( m_bOpened ) return( 1 );
	// make my unix port string from number
	sprintf( strPort, "/dev/ttyS%d", nPort-1 );
	printf( "My serial port: %s\n", strPort );

	m_pFd = open( strPort, O_RDWR | O_NOCTTY | O_NONBLOCK );
	if( m_pFd == 0 )
	{
		return( 0 );
	}

	puts( "INFO :: serial port opened ok step 1.0\n" );
	term_stat = tcgetattr( m_pFd, &old_termios );
	if( term_stat != 0 )
	{
		puts( "ERROR :: serial port not opened ok step 1.1\n" );
		return( 0 );
	}
	puts( "INFO :: serial port opened ok step 1.1\n" );

	//////////////////////////////////////////////
	//cfmakeraw( &serial_termios );
	////////////////////////////////////////
	// cfmake raw manual replacement... for these systems
	//were is missing
	new_termios.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
	new_termios.c_oflag &= ~OPOST;
	new_termios.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
	new_termios.c_cflag &= ~(CSIZE|PARENB);
	new_termios.c_cflag |= CS8;

	new_termios.c_cflag = B9600 | CS8 | CRTSCTS | CLOCAL | HUPCL;
	status = tcsetattr( m_pFd, TCSANOW, &new_termios );

	puts( "INFO :: serial port opened ok step 1.2\n" );
	m_bOpened = 1;

	return( m_bOpened );
}

////////////////////////////////////////////////////////////////////
// Method:	Close
// Class:	CSerial
// Purose:	close serial port
// Input:	void
// Output:	if port was closed successfully
////////////////////////////////////////////////////////////////////
int CSerial::Close( void )
{
	if( m_pFd != 0 )
	{
			tcsetattr( m_pFd, TCSANOW, &m_stOrigSerialTermios );
			close( m_pFd );
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	WriteCommByte
// Class:	CSerial
// Purose:	write a byte to serial port
// Input:	byte
// Output:	write operation status
////////////////////////////////////////////////////////////////////
int CSerial::WriteCommByte( unsigned char ucByte )
{
	int nWriteStat;
	char strTmp[1];

	strTmp[0] = ucByte;
	nWriteStat = write( m_pFd, strTmp, 1 );

	return( nWriteStat );
}

////////////////////////////////////////////////////////////////////
// Method:	SendData
// Class:	CSerial
// Purose:	send data to a serial port
// Input:	pointer to my data
// Output:	write operation status
////////////////////////////////////////////////////////////////////
int CSerial::SendData( const char *strBuffer, int nSize )
{
	int nWrite = 0;

	char outstr[255];
	//char *outStatus;

	//int nRead = read( _fd, outstr, 1 );
	// initialize serial
	sprintf(outstr, "%c%c%c\r", 22, 4, 3);
	printf( "write command: %d %d %d : size=%d(%d)\n", strBuffer[0],
					strBuffer[1], strBuffer[2], nSize, strlen(strBuffer) );

	puts( "INFO :CSerial: start sending data to serial port" );
	//write( m_pFd, strBuffer, nSize );
	//lseek( m_pFd, 1, 1 );
	//tcflush( m_pFd, TCOFLUSH );
	write( m_pFd, outstr, 4 );
	puts( "INFO :CSerial: end sending data to serial port" );

	return( nWrite );
}

////////////////////////////////////////////////////////////////////
// Method:	Read Data Waiting
// Class:	CSerial
// Purose:	check if is any data waiting in my serial port
// Input:	void
// Output:	if there is data waiting
////////////////////////////////////////////////////////////////////
int CSerial::ReadDataWaiting( void )
{
	fd_set readfds;

	FD_ZERO( &readfds );
	FD_SET( m_pFd, &readfds );

	int selectstat = select( m_pFd + 1, &readfds, 0, 0, NULL );

	return ( FD_ISSET( m_pFd, &readfds ) );
}

////////////////////////////////////////////////////////////////////
// Method:	Read Data
// Class:	CSerial
// Purose:	read data from my serial port
// Input:	pointer to buffer and read limit
// Output:	no of bytes read
////////////////////////////////////////////////////////////////////
int CSerial::ReadData( void *vectBuffer, int nLimit )
{
	int nRead = read( m_pFd, vectBuffer, nLimit );

	return( nRead );
}

