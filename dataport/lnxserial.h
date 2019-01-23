////////////////////////////////////////////////////////////////////
// Name:		linux serial port definition
// File:		lnxserial.h
// Purpose:		to process serial port data in linux/unix
//
// Created by:	Unknown
// Updated by:  Larry Lart on 12-Apr-2003
//				Larry Lart on 29-Apr-2003
//
////////////////////////////////////////////////////////////////////

#ifndef _SERIAL_H
#define _SERIAL_H

#include <unistd.h>
#include <termios.h>

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

class CSerial
{

//public methods
public:
	CSerial();
	~CSerial();

	int Open( int nPort = 2, int nBaud = 9600 );
	int Close( void );

	int ReadData( void *vectBuffer, int nLimit );
	int SendData( const char *strBuffer, int nSize );
	int ReadDataWaiting( void );

	int IsOpened( void ){ return( m_bOpened ); }
// methods protected
protected:
	int WriteCommByte( unsigned char );

// public data
public:
	int 			m_pFd;
	struct termios	m_stOrigSerialTermios;
	int 			m_bOpened;


};

#endif
