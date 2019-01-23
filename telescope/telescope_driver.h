
#ifndef _TELESCOPE_DRIVER_H
#define _TELESCOPE_DRIVER_H

// defines ... 
#define NULL_PTR(x) (x *)0

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// external classes
class CSerial;

// class:	CTelescopeDriver
///////////////////////////////////////////////////////
class CTelescopeDriver
{
///////////////////
// public methods
public:
	CTelescopeDriver( );
	~CTelescopeDriver( );

	// method to control writing and reading from/to serial port with delay
	int WriteToSerialPort( const char* vectData, int nData );
	int ReadFromSerialPort( void* vectData, int nData );
	void FlushSerialPort( );

	// Prototypes 
	void PointingFromTel( double *telra1, double *teldec1, double telra0, double teldec0, int pmodel );
	void PointingToTel( double *telra0, double *teldec0, double telra1, double teldec1, int pmodel );
	void Refraction( double *ha, double *dec, int dirflag );
	void Polar( double *ha, double *dec, int dirflag );
	void Decaxis( double *ha, double *dec, int dirflag );
	void Optaxis( double *ha, double *dec, int dirflag );
	void Flexure( double *ha, double *dec, int dirflag );

	// virtual driver
	virtual void Disconnect( ){ }

////////////////
// public data
public:
	// serial port connector
	CSerial* m_pSerialPort;
	// port setup
	int m_nSerialPortNo;
	int m_nSerialPortBaud;

	// connection flag
	int m_bTelConnectFlag;

	// Telescope and observatory parameters 
	// Coordinate offsets: telescope coordinates + offset = real value 
	// If you point a telescope to ra,dec with offset turned on it will
	// really be pointing at ra+offsetra, dec+offsetdec
	// Offset correction is the last applied going from raw to target coordinates
	// It is the first applied going from target to raw pointing coordinates

	// Right ascension (hours) 
	double offsetra;     
	// eclination (degrees)
	double offsetdec;

	// Mounting polar axis correction: from true pole to mount polar axis direction 
	// Parameter polaralt will be + if the mount is above the true pole
	// Parmater polaraz will be + if the mount is east of the true pole

	// Altitude (degrees) 
	double polaralt;     
	// Azimuth (degrees) 
	double polaraz;       

	// Mounting dec axis correction: from ideal axis to mount dec axis direction 
	// Declination axis skew angle in degrees for + basis 
	double decaxis;
	// Flag indicating OTA flip for German mount 
	int basis;         

	// Optical axis corrections: from mechanical axis to optical axis direction
	double optew;
	double optns;

	// Flexure parameter is flexure in degrees for a 90 degree zenith distance 
	double flexure;

	// Observatory coordinates 
	double SiteLongitude;  
	double SiteLatitude;   

	// local site conditions - to take from observatory/site object
	double SiteTemperature;
	double SitePressure;
	
	// Used to keep a focus position count
	int m_nFocusCount;    
	int m_nFocuserSpeed;
};

#endif
