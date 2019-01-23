
#ifndef _FOCUSER_DRIVER_H
#define _FOCUSER_DRIVER_H

// external classes
class CSerial;
class CLogger;

// class:	CFocuserDriver
///////////////////////////////////////////////////////
class CFocuserDriver
{
///////////////////
// public methods
public:
	CFocuserDriver( );
	~CFocuserDriver( );

	// set serial port
	void SetSerialPort( int nPortNo, int nPortBaud=9600 ){ m_nSerialPortNo=nPortNo; m_nSerialPortBaud=nPortBaud; }

	// connect/dosconnect serial
	int ConnectSerialPort( );
	int DisconnectSerialPort( );

	// Report connection status
	int IsConnected( ){ return( m_bIsConnected ); }

	// connect/disconnect
	virtual int Connect( ){ return(0); }
	virtual void Disconnect( ){ }

	// method to control writing and reading from/to serial port with delay
	int WriteToSerialPort( const char* vectData, int nData );
	int ReadFromSerialPort( void* vectData, int nData );

	// Prototypes 
	virtual int StartFocusIn( ){ return(0); }
	virtual int StartFocusOut( ){ return(0); }
	virtual int StopFocus( ){ return(0); }

////////////////
// public data
public:
	// external objects
	CLogger	*m_pLogger;	

	// serial port connector
	CSerial* m_pSerialPort;
	// port setup
	int m_nSerialPortNo;
	int m_nSerialPortBaud;

	// connection flag
	int m_bIsConnected;

	// Used to keep a focus position count
	int m_nFocusCount;    
	int m_nFocusSpeed;

	// other flags
	int m_nTimeout;
	int m_bMoving;
	long m_nLastPosition;

};

#endif
