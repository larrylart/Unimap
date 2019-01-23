////////////////////////////////////////////////////////////////////
// Pointing Corrections for Telescope Control 
//
// Copyright 2008 John Kielkopf
// kielkopf@louisville.edu 
// Date: September 27, 2008 
// Version: 1.3
////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>

// local headers
#include "../dataport/winserial.h"
#include "../util/astro_algorithms.h"
#include "protocol.h"

// main header
#include "telescope_driver.h"

////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////
CTelescopeDriver::CTelescopeDriver( )
{
	m_bTelConnectFlag = 0;
	m_nFocuserSpeed = 0;

	// initialize serial port
	m_pSerialPort = new CSerial( );
}

////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////
CTelescopeDriver::~CTelescopeDriver( )
{
	// disconnect
	if( m_pSerialPort )
	{
		Disconnect( );
		delete( m_pSerialPort );
		m_pSerialPort = NULL;
	}
}

void CTelescopeDriver::FlushSerialPort( )
{ 
	m_pSerialPort->Flush(); 
}

////////////////////////////////////////////////////////////////////
int CTelescopeDriver::WriteToSerialPort( const char* vectData, int nData )
{ 
	int nWrite = m_pSerialPort->SendData( vectData, nData );
	Sleep( 50 ); 
	return( nWrite ); 
}

////////////////////////////////////////////////////////////////////
int CTelescopeDriver::ReadFromSerialPort( void* vectData, int nData )
{
	memset( vectData, 0, nData+1 );

	int nRead = m_pSerialPort->ReadData( vectData, nData ); 
	Sleep( 20 ); 
	return( nRead ); 
}

////////////////////////////////////////////////////////////////////
// Apply corrections to coordinates reported by the telescope 
// Input telescope raw coordinates assumed zero corrected 
// Return real coordinates corresponding to this raw point 
// Implemented in order: polar, decaxis, optaxis, flexure, refract, offset 
// Note that the order is reversed from that in PointingFromTel
////////////////////////////////////////////////////////////////////
void CTelescopeDriver::PointingFromTel( double *telra1, double *teldec1, double telra0, double teldec0, int pmodel )
{
	double tmpra, tmpha, tmpdec, tmplst;

	// Find the LST for the pointing corrections and save it
	// Otherwise the lapsed time to do corrections affects the resultant HA and RA 
	tmplst = LSTNow( SiteLongitude );
	tmpra = telra0;
	tmpha = tmplst - tmpra;
	tmpha = Map12(tmpha);
	tmpdec = teldec0;

	// Correct for mounting misalignment
	// For dir = -1 returns the real pointing given the apparent pointing
	if ( pmodel == (pmodel | POLAR) ) Polar( &tmpha,  &tmpdec, -1 );

	// Correct for dec axis not perpendicular to polar axis
	// For dir = -1 returns the real pointing given the apparent pointing
	if( pmodel == (pmodel | DECAXIS) ) Decaxis( &tmpha,  &tmpdec, -1 );

	// Correct for optical axis not coincident with mechanical axis 
	// For dir = -1 returns the real pointing given the apparent pointing 
	if( pmodel == (pmodel | OPTAXIS) ) Optaxis( &tmpha,  &tmpdec, -1 );

	// Correct for OTA flexure 
	// For dir = -1 returns the real pointing given the apparent pointing
	if( pmodel == (pmodel | FLEXURE) ) Flexure( &tmpha,  &tmpdec, -1);

	tmpra = tmplst - tmpha;

	// Correct for atmospheric refraction
	// For dir = -1 returns the real pointing given the apparent pointing
	if( pmodel == (pmodel | REFRACT) ) Refraction( &tmpha,  &tmpdec, -1);

	// Correct for offset
	// Offsets are defined as added to apparent pointing to give real pointing
	if( pmodel == (pmodel | OFFSET) )
	{
		tmpra = tmpra + offsetra;
		tmpdec = tmpdec + offsetdec;
	}

	tmpra = Map24(tmpra);
	tmpdec = Map360(tmpdec);

	*telra1 = tmpra;
	*teldec1 = tmpdec;

	return;
}  

////////////////////////////////////////////////////////////////////
// Apply corrections to find the apparent coordinates to send to the telescope 
// Input real target coordinates 
// Return apparent telescope coordinates to have it point at this target
// Implemented in order: offset, refract, flexure, optaxis, decaxis, polar 
// Note that the order is reversed from that in PointingFromTel 
////////////////////////////////////////////////////////////////////
void CTelescopeDriver::PointingToTel( double *telra0, double *teldec0, double telra1, double teldec1, int pmodel )
{
	double tmpra=0.0, tmpha=0.0, tmpdec=0.0, tmplst=0.0;

	tmpra = telra1;
	tmpdec = teldec1;

	// Correct for offset - Offsets are defined as added to apparent 
	// pointing to give real pointing 
	if ( pmodel == (pmodel | OFFSET) )
	{
		tmpra = tmpra - offsetra;
		tmpdec = tmpdec - offsetdec;
	}  

	// Save the LST for later use, find the HA for this LST and finish the model 
	tmplst = LSTNow( SiteLongitude );
	tmpha = tmplst - tmpra;
	tmpha = Map12(tmpha); 

   
	// Correct for atmospheric refraction
	// For dir = +1 returns the apparent pointing given the real target pointing
	if( pmodel == (pmodel | REFRACT) ) Refraction( &tmpha,  &tmpdec, 1);

	// Correct for OTA flexure
	// For dir = +1 returns the apparent pointing given the real pointing
	if( pmodel == (pmodel | FLEXURE) ) Flexure( &tmpha,  &tmpdec, 1);

	// Correct for optical axis not coincident with mechanical axis
	// For dir = +1 returns the apparent pointing given the real pointing
	if( pmodel == (pmodel | OPTAXIS) ) Optaxis( &tmpha,  &tmpdec, 1);

	// Correct for dec axis not perpendicular to polar axis
	// For dir = +1 returns the apparent pointing given the real pointing
	if( pmodel == (pmodel | DECAXIS) ) Decaxis( &tmpha,  &tmpdec, 1);

	// Correct for mounting misalignment
	// For dir = +1 returns the apparent pointing given the real pointing
	if( pmodel == (pmodel | POLAR) ) Polar( &tmpha,  &tmpdec, 1);

	tmpra = tmplst - tmpha;
	tmpra = Map24(tmpra);
	tmpdec = Map360(tmpdec);

	*telra0 = tmpra;
	*teldec0 = tmpdec;

	return;
}  


/* Correct ha and dec for atmospheric refraction 
 *
 * Call this in the form Refraction(&ha,&dec,dirflag)
 *
 * Input: 
 *   Pointers to ha and dec 
 *   Integer dirflag >=0 for add (real to apparent)
 *   Integer dirflag <0  for subtract (apparent to real)
 *   Valid above 15 degrees.  Below 15 degrees uses 15 degree value.
 *   Global local barometric SitePressure (Torr) 
 *   Global local air temperature SiteTemperature (C) 
 * Output:
 *   In place pointers to modified ha and dec 
 */

void CTelescopeDriver::Refraction( double *ha, double *dec, int dirflag )
{
  double altitude, azimuth, dalt, arg;
  double hourangle, declination;

  /* Calculate the change in apparent altitude due to refraction */
  /* Uses 15 degree value for altitudes below 15 degrees */

  /* Real to apparent */
  /* Object appears to be higher due to refraction */
  
  hourangle = *ha;
  declination = *dec;
  EquatorialToHorizontal( SiteLatitude, hourangle, declination, &azimuth, &altitude );

  if (dirflag >= 0)
  {  
    if (altitude >= 15.)
    {
      arg = (90.0 - altitude)*PI/180.0;
      dalt = tan(arg);
      dalt = 58.276 * arg - 0.0824 * arg * arg * arg;
      dalt = dalt / 3600.;
      dalt = dalt * (SitePressure/(760.*1.01))*(283./(273.+SiteTemperature));
    }
   else
    {
      arg = (90.0 - 15.0)*PI/180.0;
      dalt = tan(arg);
      dalt = 58.276 * arg - 0.0824 * arg * arg * arg;
      dalt = dalt / 3600.;
      dalt = dalt * (SitePressure/(760.*1.01))*(283./(273.+SiteTemperature));
    }
    altitude = altitude + dalt;
  }
  
  /* Apparent to real */
  /* Object appears to be higher due to refraction */
  
  else if (dirflag < 0)
  {
    if (altitude >= 15.)
    {
      arg = (90.0 - altitude)*PI/180.0;
      dalt = tan(arg);
      dalt = 58.294 * arg - 0.0668 * arg * arg * arg;
      dalt = dalt / 3600.;
      dalt = dalt * (SitePressure/(760.*1.01))*(283./(273.+SiteTemperature));
    }
   else
    {
      arg = (90.0 - 15.0)*PI/180.0;
      dalt = tan(arg);
      dalt = 58.294 * arg - 0.0668 * arg * arg * arg;
      dalt = dalt / 3600.;
      dalt = dalt * (SitePressure/(760.*1.01))*(283./(273.+SiteTemperature));
    }

    altitude = altitude - dalt;
  }
   HorizontalToEquatorial( SiteLatitude, azimuth, altitude, &hourangle, &declination );
  *ha = hourangle;
  *dec = declination; 
}

/* Allow for known polar alignment parameters 
 *
 * Call this in the form Polar(&ha,&dec,dirflag)
 *
 * Input: 
 *   Pointers to ha and dec 
 *   Integer dirflag >=0  real to apparent
 *   Integer dirflag <0   apparent to real
 * Output:
 *   In place pointers to modified ha and dec 
 * Requires:
 *   SiteLatitude in degrees
 *   polaralt polar axis altitude error in degrees
 *   polaraz  polar axis azimuth  error in degrees
 */

void CTelescopeDriver::Polar(double *ha, double *dec, int dirflag)
{
  double hourangle, declination;
  double da, db;
  double epsha, epsdec;
  double latitude;
  
  /* Make local copies of input */
  
  hourangle = *ha;
  declination = *dec;  
  da = polaraz;
  db = polaralt;
  latitude = SiteLatitude;
  
  /* Convert to radians for the calculations */
   
  hourangle = hourangle*PI/12.;
  declination = declination*PI/180.;
  da = da*PI/180.;
  db = db*PI/180.; 
  latitude = latitude*PI/180.; 
  
  /* Evaluate the corrections in radians for this ha and dec */
  /* Sign check:  on the meridian at the equator */
  /*   epsha goes as -da */
  /*   epsdec goes as +db */
  /*   polaraz and polaralt signed + from the true pole to the mount pole */
    
  epsha = db*tan(declination)*sin(hourangle) - 
    da*(sin(latitude) - tan(declination)*cos(hourangle)*cos(latitude));
  epsdec = db*cos(hourangle) - da*sin(hourangle)*cos(latitude);    
    
  /* Real to apparent */
  
  if (dirflag >= 0)
  {  
    hourangle = hourangle + epsha;
    declination = declination + epsdec;
  }
  
  /* Apparent to real */
  
  else if (dirflag < 0)
  {
    hourangle = hourangle - epsha;
    declination = declination - epsdec;
  }

  /* Convert from radians to hours for ha and degrees for declination */

  hourangle = hourangle*12./PI;
  declination = declination*180./PI;
  
  /* Check for range and adjust to -90 -> +90 and 0 -> 24  */
 
  if (declination > 90. ) 
  {
    declination = 180. - declination;
    hourangle = hourangle + 12.;
  }
   if (declination < -90. ) 
  {
    declination = -180. - declination;
    hourangle = hourangle + 12.;
  }  

  hourangle = Map24(hourangle);         
  *ha = hourangle;
  *dec = declination;
}


/* Allow for known declination axis skew  
 *
 * Call this in the form Decaxis(&ha,&dec,dirflag)
 *
 * Based on:
 *   Trueblood and Genet page 125
 * Input: 
 *   Pointers to ha and dec 
 *   Integer dirflag >=0  real to apparent
 *   Integer dirflag <0   apparent to real
 * Output:
 *   In place pointers to modified ha and dec 
 * Requires global values:
 *   decskew = declination axis skew angle in degrees
 *   basis = +1 for looking to the west from the east side
 *     and  -1 for looking from the east to the west side
 *  
 * Cautionary note:
 *   The sign of the skew correction changes when a German equatorial 
 *   is flipped across the meridian: basis parameter flags this flip.
 *   Set decskew value and sign for basis = +1; 
 */

void CTelescopeDriver::Decaxis(double *ha, double *dec, int dirflag)
{
  double hourangle, declination;
  double da, dsign;
  double epsha, epsdec;
  
  /* Make local copies of input */
  
  hourangle = *ha;
  declination = *dec;  
  dsign = basis;
  da = dsign*decaxis;
  
  /* Note about the basis                                                      */
  /* We use a global flag to indicate a flip of the OTA/mounting               */
  /* By our own convention a +1 basis is the one with the OTA east of the pier */
  /*   looking to the west (e.g. to + HA)                                      */
  /*   and a -1 basis is with the OTA on the west side of the pier looking to  */
  /*   the east (e.g. to - HA)                                                 */
  /* The basis flag should be set by the telescope control software to alert   */
  /*   pointing algorithms of a flip of the OTA                                */
  /* In a fork mounting you may safely set basis = 1 and ignore the flip       */
  /*   unless the telescope is allowed to rotate through the fork arms         */
  
  /* Convert to radians for the calculations */
   
  hourangle = hourangle*PI/12.;
  declination = declination*PI/180.;
  da = da*PI/180.;
  
  /* Evaluate the corrections in radians for this ha and dec */
  
  epsha = da*sin(declination);
  epsdec = da*epsha;
      
  /* Real to apparent */
  
  if (dirflag >= 0)
  {  
    hourangle = hourangle + epsha;
    declination = declination + epsdec;
  }
  
  /* Apparent to real */
  
  else if (dirflag < 0)
  {
    hourangle = hourangle - epsha;
    declination = declination - epsdec;
  }

  /* Convert from radians to hours for ha and degrees for declination */

  hourangle = hourangle*12./PI;
  declination = declination*180./PI;
  
  /* Check for range and adjust to -90 -> +90 and 0 -> 24  */
 
  if (declination > 90. ) 
  {
    declination = 180. - declination;
    hourangle = hourangle + 12.;
  }
   if (declination < -90. ) 
  {
    declination = -180. - declination;
    hourangle = hourangle + 12.;
  }  
  hourangle = Map24(hourangle);         
  
  /* Return modified ha and dec */
  
  *ha = hourangle;
  *dec = declination;
}


/* Allow for known optical axis skew  
 *
 * Call this in the form Optaxis(&ha,&dec,dirflag)
 *
 * Based on:
 *   Trueblood and Genet page 127
 * Input: 
 *   Pointers to ha and dec 
 *   Integer dirflag >=0  real to apparent
 *   Integer dirflag <0   apparent to real
 * Output:
 *   In place pointers to modified ha and dec 
 * Requires global values:
 *   optew = optical axis east-west skew angle in degrees
 *   optns = optical axis north-south skew angle in degrees
 *   basis = +1 for looking to the west from the east side
 *     and  -1 for looking from the east to the west side
 *  
 * Cautionary note:
 *   The sign of the skew corrections change when a German equatorial 
 *   is flipped across the meridian: basis parameter flags this flip.
 *   Set both skew value and sign for basis = +1;
 *   On a flip across the meridian the telescope is rotated 180 degrees
 *   NS and EW directions on the sky are changed in sign 
 *
 * Regarding NS correction:
 *   optns affects only the dec and is a constant correction.
 *   optns = 0. when the axes are zeroed from a starfield on startup. 
 */

void CTelescopeDriver::Optaxis(double *ha, double *dec, int dirflag)
{
  double hourangle, declination;
  double da, db, dsign;
  double epsha, epsdec;
  
  /* Make local copies of input */
  
  hourangle = *ha;
  declination = *dec;  
  dsign = basis;
  da = optew*dsign;
  db = optns*dsign;
  
  /* Reminder about optns: it gives a constant dec correction                  */
  /*   Simplest to set to zero and let encoder reference handle it             */
  /*   Otherwise it interacts with a zero offset of the encoder.               */  
  
  /* Note about the basis                                                      */
  /* We use a global flag to indicate a flip of the OTA/mounting               */
  /* By our own convention a +1 basis is the one with the OTA east of the pier */
  /*   looking to the west (e.g. to + HA)                                      */
  /*   and a -1 basis is with the OTA on the west side fo the peir looking to  */
  /*   the east (e.g. to - HA)                                                 */
  /* The basis flag should be set by the telescope control software to alert   */
  /*   pointing algorithms of a flip of the OTA                                */
  /* In a fork mounting you may safely set basis = 1 and ignore the flip       */
  /*   unless the telescope is allowed to rotate through the fork arms         */
  
  /* Convert to radians for the calculations */
   
  hourangle = hourangle*PI/12.;
  declination = declination*PI/180.;
  da = da*PI/180.;
  db = db*PI/180.;
  
  /* Evaluate the ha correction in radians for this dec */
  
  if (fabs(90. - declination) > 0.5)
  {
    /* Not needed at the pole */
    epsha = da/cos(declination);
  } 

  /* The dec correction is constant.  */
  /* Set optns = 0. to ignore. */
  
  epsdec = db;
      
  /* Real to apparent */
  
  if (dirflag >= 0)
  {  
    hourangle = hourangle + epsha;
    declination = declination + epsdec;
  }
  
  /* Apparent to real */
  
  else if (dirflag < 0)
  {
    hourangle = hourangle - epsha;
    declination = declination - epsdec;
  }

  /* Convert from radians to hours for ha and degrees for declination */

  hourangle = hourangle*12./PI;
  declination = declination*180./PI;
  
  /* Check for range and adjust to -90 -> +90 and 0 -> 24  */
 
  if (declination > 90. ) 
  {
    declination = 180. - declination;
    hourangle = hourangle + 12.;
  }
   if (declination < -90. ) 
  {
    declination = -180. - declination;
    hourangle = hourangle + 12.;
  }  
  hourangle = Map24(hourangle);         
  
  /* Return modified ha and dec */
  
  *ha = hourangle;
  *dec = declination;
  
}

/* Allow for known OTA flexure
 *
 * Call this in the form Flexure(&ha,&dec,dirflag)
 *
 * Based on:
 *   Trueblood and Genet page 128
 * Input: 
 *   Pointers to ha and dec 
 *   Integer dirflag >=0  real to apparent
 *   Integer dirflag <0   apparent to real
 * Output:
 *   In place pointers to modified ha and dec 
 * Requires global value:
 *   flexure = optical axis flexure at 90 degree zenith distance (degrees)
 *   SiteLatitude = latitude in degrees
 *  
 * Cautionary notes:
 *   This is very crude, but it might help to give arcsecond pointing.
 *   It does not allow for mount flexure directly, but if determined
 *   empirically, mount flexure may be part of it.
 *   It would probably be worthwhile to replace this with an empirical 
 *   fitting for a function of both H and Z based on an analysis of
 *   pointing residuals with other corrections in place.
 * 
 */  

void CTelescopeDriver::Flexure(double *ha, double *dec, int dirflag)
{
  double hourangle, declination;
  double azimuth, altitude;
  double da,dalt;
  
  /* Make local copies of input */
  
  hourangle = *ha;
  declination = *dec;
  da = flexure;  
  
  /* Behaves qualitatively like refraction but with opposite sign */
  /* Correction dalt is how much lower the OTA is aimed than the mount is pointed */

  EquatorialToHorizontal( SiteLatitude, hourangle, declination, &azimuth, &altitude );
  dalt = -da*cos(altitude*PI/180.);
  
  /* Real to apparent */
  
  if (dirflag >= 0)
  {  
    altitude = altitude + dalt;
  }
  
  /* Apparent to real */
  
  else if (dirflag < 0)
  {
    altitude = altitude - dalt;
  }
  HorizontalToEquatorial( SiteLatitude, azimuth, altitude, &hourangle, &declination );
  *ha = hourangle;
  *dec = declination;
}
