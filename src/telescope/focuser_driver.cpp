////////////////////////////////////////////////////////////////////
// Focuser driver base class
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>

// local headers
#include "../dataport/winserial.h"
#include "../logger/logger.h"

// main header
#include "focuser_driver.h"

////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////
CFocuserDriver::CFocuserDriver( )
{
	m_bIsConnected = 0;
	m_nFocusSpeed = 0;
	m_nFocusCount = 0;
	m_nTimeout = 0;
	m_bMoving = 1;
	m_nLastPosition = 0;
	m_pLogger = NULL;
	// serial port defaults
	m_nSerialPortNo = 1;
	m_nSerialPortBaud = 9600;

	// initialize serial port
	m_pSerialPort = new CSerial( );
}

////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////
CFocuserDriver::~CFocuserDriver( )
{
	// disconnect
	if( m_pSerialPort )
	{
		Disconnect( );
		delete( m_pSerialPort );
		m_pSerialPort = NULL;
	}
}

////////////////////////////////////////////////////////////////////
int CFocuserDriver::ConnectSerialPort( )
{
	wxString strMsg;

	// check first if it's open
	if( m_pSerialPort->IsOpened( ) ) m_pSerialPort->Close( );

	// open serial port
	if( !m_pSerialPort->Open( m_nSerialPortNo, m_nSerialPortBaud ) ) 
	{
		if( m_pLogger ) 
		{
			strMsg.Printf( wxT("ERROR :: unable to open serial port=COM%d"), m_nSerialPortNo ); 
			m_pLogger->Log( strMsg, GUI_LOGGER_ATTENTION_STYLE );
		}
		return( -1 );
	}

	if( m_pLogger ) 
	{
		strMsg.Printf( wxT("INFO :: serial port=COM%d opened ok."), m_nSerialPortNo );
		m_pLogger->Log( strMsg );
	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CFocuserDriver::DisconnectSerialPort( )
{
	// disconect serial port
	if( m_pSerialPort->IsOpened() ) m_pSerialPort->Close( );
	m_bIsConnected = 0;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CFocuserDriver::WriteToSerialPort( const char* vectData, int nData )
{ 
	int nWrite = m_pSerialPort->SendData( vectData, nData );
	Sleep( 50 ); 
	return( nWrite ); 
}

////////////////////////////////////////////////////////////////////
int CFocuserDriver::ReadFromSerialPort( void* vectData, int nData )
{ 
	int nRead = m_pSerialPort->ReadData( vectData, nData ); 
	Sleep( 10 ); 
	return( nRead ); 
}

