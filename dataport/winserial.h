////////////////////////////////////////////////////////////////////
// Name:		windows serial port definition
// File:		winserial.h
// Purpose:		to process serial port data
//
// Created by:	Unknown
// Updated by:  Larry Lart on 22-Apr-2006 
//
////////////////////////////////////////////////////////////////////

#ifndef _SERIAL_H
#define _SERIAL_H

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

#include <windows.h>

class CSerial
{

public:
	CSerial();
	~CSerial();

	BOOL Open( int nPort = 0, int nBaud = 9600 );
	BOOL Close( void );

	int ReadData( void *, int );
	int SendData( const char *, int );
	int ReadDataWaiting( void );
	HRESULT Flush(DWORD dwFlag = PURGE_TXCLEAR | PURGE_RXCLEAR);

	BOOL IsOpened( void ){ return( m_bOpened ); }

protected:
	BOOL WriteCommByte( unsigned char );

	HANDLE		m_hIDComDev;
	OVERLAPPED	m_OverlappedRead, m_OverlappedWrite;
	BOOL		m_bOpened;

};

#endif
