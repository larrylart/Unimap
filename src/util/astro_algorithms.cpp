/////////////////////////////////////////////////////////////////////
// Celestial Coordinate Algorithms
//
// John Kielkopf 
// kielkopf@louisville.edu
//
// Date: September 12, 2008        
// Version: 1.06
//
/////////////////////////////////////////////////////////////////////


/*
 * Usage:
 *
 * These algorithms convert astronomical catalog entries into apparent
 * celestial coordinates at a specific observatory site. They should have an
 * inherent accuracy of the order of a second of arc. 
 *
 * A correction for atmospheric refraction is included that
 * is reliable when objects are more than 15 degrees above the horizon.
 * At lower altitudes the atmosphere cannot be modeled with second of arc
 * precision without additional real-time corrections.  
 *
 * The algorithms should be used in conjuntion with routines that convert 
 * apparent celestial coordinates into real telescope coordinates, 
 * allowing for mounting mechanical errors, misalignment, and flexure.
 * Without such real world corrections, pointing errors typically will
 * be minutes, rather than seconds, of arc.  
 *
 * The precession formulae apply to catalogs in the FK5 system.
 * They will not give exact results for catalogs in the FK4 system. 
 * See Meeus, Astronomical Algorithms, p. 129, for an explanation.
 *
 */
 
#include <stdio.h>
#include <math.h>
#include <time.h>

// main header
#include "astro_algorithms.h"

/*  Compute the Julian Day for the given date */
/*  Julian Date is the number of days since noon of Jan 1 4713 B.C. */
/////////////////////////////////////////////////////////////////////
double CalcJD( int ny, int nm, int nd, double ut )
{
  double A, B, C, D, jd, day;

  day = nd + ut / 24.0;
  if ((nm == 1) || (nm == 2)) {
    ny = ny - 1;
    nm = nm + 12;
  }

  if (((double) ny + nm / 12.0 + day / 365.25) >=
    (1582.0 + 10.0 / 12.0 + 15.0 / 365.25)) 
  {
    A = ((int) (ny / 100.0));
    B = 2.0 - A + (int) (A / 4.0);
  } 
  else 
  {
    B = 0.0;
  }

  if (ny < 0.0) 
  {
    C = (int) ((365.25 * (double) ny) - 0.75);
  } 
    else 
  {
    C = (int) (365.25 * (double) ny);
  }

  D = (int) (30.6001 * (double) (nm + 1));
  jd = B + C + D + day + 1720994.5;
  return (jd);
}


/* Calculate the Julian date from the system clock */

double JDNow(void)
{
  int year,month,day;
  int hours,minutes,seconds;
  double ut,jd;
  time_t observatory;
  struct tm * greenwich;

  time(&observatory);
  greenwich=gmtime(&observatory);

  year = greenwich->tm_year;
  year = year + 1900;
  month = greenwich->tm_mon;
  month = month + 1;
  day = greenwich->tm_mday;
  hours = greenwich->tm_hour;
  minutes = greenwich->tm_min;
  seconds = greenwich->tm_sec;

  ut = ( (double) seconds )/3600. + 
    ( (double) minutes )/60. + 
    ( (double) hours );   

  jd = CalcJD(year, month, day, ut);
  
  /* To test for specific jd change this value and uncomment */
  
  /* jd = 2462088.69;  */
    
  return (jd) ;
}


/*  Compute Greenwich Mean Sidereal Time (gmst) */
/*  TU is number of Julian centuries since 2000 January 1.5 */
/*  Expression for gmst from the Astronomical Almanac Supplement */

double CalcLST(int year, int month, int day, double ut, double glong)
{
  double TU, TU2, TU3, T0;
  double gmst,lmst;

  TU = (CalcJD(year, month, day, 0.0) - 2451545.0) / 36525.0;
  TU2 = TU * TU;
  TU3 = TU2 * TU;
  T0 =
      (24110.54841 / 3600.0) +
      8640184.812866 / 3600.0 * TU + 0.093104 / 3600.0 * TU2 -
      6.2e-6 / 3600.0 * TU3;
  T0 = Map24(T0);

  gmst = Map24(T0 + ut * 1.002737909);
  lmst = 24.0 * frac((gmst - glong / 15.0) / 24.0);
  return (lmst);
}


// Purpose:	Calculate the local sidereal time from the system clock 
////////////////////////////////////////////////////////////////////
double LSTNow( double nSiteLongitude )
{
	int year,month,day;
	int hours,minutes,seconds;

	double glong;    
	double ut, lst;
	time_t observatory;
	struct tm * greenwich;

	time( &observatory );
	greenwich = gmtime( &observatory );

	year = greenwich->tm_year;
	year = year + 1900;
	month = greenwich->tm_mon;
	month = month + 1;
	day = greenwich->tm_mday;
	hours = greenwich->tm_hour;
	minutes = greenwich->tm_min;
	seconds = greenwich->tm_sec;

	ut = ( (double) seconds )/3600. + ( (double) minutes )/60. + ( (double) hours );   
	glong = nSiteLongitude; 
	lst = CalcLST( year, month, day, ut, glong );

	return (lst) ;
}

/* Calculate the universal time in hours from the system clock */
/* Limited to 1 second accuracy by the time_t structure */
////////////////////////////////////////////////////////////////////
double UTNow(void)
{
  int hours,minutes,seconds;
 
  double ut;
  time_t observatory;
  struct tm * greenwich;
  
  time(&observatory);
  greenwich=gmtime(&observatory);

  hours = greenwich->tm_hour;
  minutes = greenwich->tm_min;
  seconds = greenwich->tm_sec;

  ut = ( (double) seconds )/3600. + 
    ( (double) minutes )/60. + 
    ( (double) hours );   

  return (ut) ;
}



/* Map a time in hours to the range  0  to 24 */

double Map24(double hour)
{
  int n;
  
  if (hour < 0.0) 
  {
     n = (int) (hour / 24.0) - 1;
     return (hour - n * 24.0);
  } 
  else if (hour > 24.0) 
  {
     n = (int) (hour / 24.0);
     return (hour - n * 24.0);
  } 
  else 
  {
     return (hour);
  }
}


/* Map an hourangle in hours to the range  -12  to +12 */

double Map12(double hour)
{
  double hour24;
  
  hour24=Map24(hour);
  
  if (hour24 > 12.0) 
  {
     return (hour24 - 24.0);
  } 
  else 
  {
     return (hour24);
  }
}


/* Map an angle in degrees to the range 0 to 360 */

double Map360(double angle)
{
  int n;

  if (angle < 0.0) 
  {
    n = (int) (angle / 360.0) - 1;
   return (angle - n * 360.0);
  } 
  else if (angle > 360.0)
  {
    n = (int) (angle / 360.0);
    return (angle - n * 360.0);
  } 
  else 
  {
    return (angle);
  }
}



/* Map an angle in degrees to the range -180 to 180 */

double Map180(double angle)
{
  double angle360;
  angle360 = Map360(angle);
  
  if (angle360 > 180.0) 
  {
     return (angle360 - 360.0);
  } 
  else 
  {
     return (angle360);
  }


}


/* Fractional part */

double frac(double x)
{
  x -= (int) x;
  return ((x < 0) ? x + 1.0 : x);
}
  

/* Precession from J2000 to EOD or back  */

void Precession(double *ra, double *dec, int dirflag)
{
  double tmpra, tmpdec;
  
  tmpra = *ra;
  tmpdec = *dec;
  if (dirflag > 0)
  {
    PrecessToEOD(2000.0, &tmpra, &tmpdec);
  }
  else if (dirflag < 0)
  {
    PrecessToEpoch(2000.0, &tmpra, &tmpdec);       
  } 

 /* Return values to the locations pointed to by ra and dec */

  *ra = tmpra;
  *dec = tmpdec;
}


/* Precess in place from epoch to EOD 
 *
 * Coordinates ra in hours and dec in degrees
 *
 * Call this in the form PrecessToEOD(epoch,&ra,&dec)
 *
 */
 

void PrecessToEOD(double epoch, double *ra, double *dec)
{
  double ra_in, dec_in, ra_out, dec_out;
  double a,b,c;
  double zeta,z,theta;
  double T, t;
  double jdfixed, jdnow;  
  
  /* Fetch the input values for ra and dec and save in radians */

  ra_in = *ra;
  dec_in = *dec;
  
  /* Convert to radians for use here */
  
  ra_in = ra_in*PI/12.;
  dec_in = dec_in*PI/180.;
   
  /* Find zeta, z, and theta at this moment */
  
  /* JD for the fixed epoch */
  
  jdfixed = (epoch - 2000.0)*365.25+2451545.0;

  /* JD for epoch of date */

  jdnow = JDNow();
  
  /* Julian centuries for the fixed epoch from a base epoch 2000.0 */

  T = (jdfixed - 2451545.0) / 36525.0;

  /* Julian centuries for the epoch of date from the fixed epoch */

  t = (jdnow - jdfixed) / 36525.0; 
  
  /* Evaluate the constants in arc seconds */
    
  zeta=(2306.2181 + 1.39656*T - 0.000139*T*T)*t +
    (0.30188 - 0.000344*T)*t*t +
    (0.017998)*t*t*t;
  
  z=(2306.2181 + 1.39656*T - 0.000139*T*T)*t +
    (1.09468 + 0.000066*T)*t*t +
    (0.018203)*t*t*t;
  
  theta=(2004.3109 - 0.85330*T - 0.000217*T*T)*t +
    (-0.42665 - 0.000217*T)*t*t +
    (-0.041833)*t*t*t;
  
  /* Convert to radians */
  
  zeta = zeta * PI / (180.*3600.);
  z = z *  PI / (180.*3600.);
  theta = theta * PI / (180.*3600.);
 
  /* Calculate the precession */
  
  a = sin(ra_in + zeta)*cos(dec_in);
  b = cos(ra_in + zeta)*cos(theta)*cos(dec_in) -
        sin(theta)*sin(dec_in);
  c = cos(ra_in + zeta)*sin(theta)*cos(dec_in) +
        cos(theta)*sin(dec_in);
  if (c > 0.9)
  {
    dec_out = acos(sqrt(a*a+b*b));
  }
  else if (c < -0.9)
  {
    dec_out = -acos(sqrt(a*a+b*b));  
  }
  else
  {  
    dec_out = asin(c);
  }
  ra_out = atan2(a,b) + z;
  
  /* Convert back to hours and degrees */
  
  ra_out = ra_out*12./PI;
  
  dec_out = dec_out*180./PI;

  /* Check for range and adjust to -90 -> +90 and 0 -> 24 and if needed */
 
  if (dec_out > 90. ) 
  {
    dec_out = 180. - dec_out;
    ra_out = ra_out + 12.;
  }
   if (dec_out < -90. ) 
  {
    dec_out = -180. - dec_out;
    ra_out = ra_out + 12.;
  }  

  ra_out = Map24(ra_out);        
 
  /* Return values to the locations pointed to by ra and dec */

  *ra = ra_out;
  *dec = dec_out;
       
}


/* Precess from EOD to epoch 
 *
 * Mean coordinates ra in hours and dec in degrees returned in place 
 * Will not remove nutation and aberration 
 * Call this in the form PrecessToEpoch(epoch,&ra,&dec) 
 *
 */
 
void PrecessToEpoch(double epoch, double *ra, double *dec)
{
  double ra_in, dec_in, ra_out, dec_out;
  double a,b,c;
  double zeta,z,theta;
  double T, t;
  double jdfixed, jdnow;  

  /* Fetch the input values for ra and dec */

  ra_in = *ra;
  dec_in = *dec;

  /* Convert to radians for use here */
  
  ra_in = ra_in*PI/12.;
  dec_in = dec_in*PI/180.;

  /* JD for the fixed epoch */
  
  jdfixed = (epoch - 2000.0)*365.25+2451545.0;

  /* JD for epoch of date */

  jdnow = JDNow();
  
  /* Julian centuries for the fixed epoch from a base epoch 2000.0 */

  T = (jdnow - 2451545.0) / 36525.0;

  /* Julian centuries for the epoch of date from the fixed epoch */

  t = (jdfixed - jdnow) / 36525.0; 
  
  /* Evaluate the constants in arc seconds */
    
  zeta=(2306.2181 + 1.39656*T - 0.000139*T*T)*t +
    (0.30188 - 0.000344*T)*t*t +
    (0.017998)*t*t*t;
  
  z=(2306.2181 + 1.39656*T - 0.000139*T*T)*t +
    (1.09468 + 0.000066*T)*t*t +
    (0.018203)*t*t*t;
  
  theta=(2004.3109 - 0.85330*T - 0.000217*T*T)*t +
    (-0.42665 - 0.000217*T)*t*t +
    (-0.041833)*t*t*t;
  
  /* Convert to radians */
  
  zeta = zeta * PI / (180.*3600.);
  z = z *  PI / (180.*3600.);
  theta = theta * PI / (180.*3600.);

  /* Calculate the precession */

  a = sin(ra_in + zeta)*cos(dec_in);
  b = cos(ra_in + zeta)*cos(theta)*cos(dec_in) -
        sin(theta)*sin(dec_in);
  c = cos(ra_in + zeta)*sin(theta)*cos(dec_in) +
        cos(theta)*sin(dec_in);
  if (c > 0.9)
  {
    dec_out = acos(sqrt(a*a+b*b));
  }
  else if (c < -0.9)
  {
    dec_out = -acos(sqrt(a*a+b*b));  
  }
  else
  {  
    dec_out = asin(c);
  }
  ra_out = atan2(a,b) + z;
    
  /* Convert back to hours and degrees */
  
  ra_out = ra_out*12./PI;
  dec_out = dec_out*180./PI;

  /* Check for range and adjust to -90 -> +90 and 0 -> 24 and if needed */
  
  if (dec_out > 90. ) 
  {
    dec_out = 180. - dec_out;
    ra_out = ra_out + 12.;
  }
  if (dec_out < -90. ) 
  {
    dec_out = -180. - dec_out;
    ra_out = ra_out + 12.;
  }  

  ra_out = Map24(ra_out);        
  
  /* Return values to the locations pointed to by ra and dec */

  *ra = ra_out;
  *dec = dec_out;
           
}


/* Calculate the apparent coordinates of an object from J2000 ra and dec
 *   or find  J2000 ra and dec from apparent coordinates  
 *
 * The following assumes that the J2000 coordinates already include
 *   proper motion to the EOD.  That is, these J2000 coordinates are
 *   for the current position of the object, but given in an
 *   epoch 2000.0 coordinate system.
 *
 *   If proper motion is known and the coordinates do not yet include it,
 *     then this routine will allow for proper motion to EOD from epoch:
 *
 *     ProperMotion(epoch, &ra, &dec, pm_ra, pm_dec)
 *
 *
 * Procedure for finding apparent coordinates from J2000 coordinates:
 *
 *   1. Allow for precession by converting from catalog epoch to the EOD
 *   2. Add nutation for the EOD 
 *   3. Add stellar aberration for the EOD
 *   
 * Additional corrections for atmospheric refraction and
 *   telescope pointing errors should be added if needed 
 *
 * Call:
 *
 *   Apparent(&ra,&dec,dirflag)
 *
 * Input: 
 *   Pointers to ra and dec 
 *   Integer flag > 0 for from J2000 to EOD  and < 0 for from EOD to J2000
 *
 *   The forward transformation is exact but the inverse transformation
 *     requires iterating stellar aberration and is done only approximately
 *     in the aberration routine.
 * 
 * Output:
 *   In place pointers to modified ra and dec 
 *
 */
 
void Apparent(double *ra, double *dec, int dirflag)
{

  double tmpra, tmpdec;
  
  /* Create local copies of the ra and dec and work on these copies */

  tmpra = *ra;
  tmpdec = *dec;
        
  if(dirflag > 0)
  {
      
    /* Precess ra and dec to the EOD */
  
    Precession(&tmpra, &tmpdec, 1);
      
    /* Include nutation for ra and dec */
      
    Nutation(&tmpra, &tmpdec, 1);
      
    /* Include aberration for ra and dec */
     
    Aberration(&tmpra, &tmpdec, 1);
  
    /* Return the new EOD coordinates that are in the local variables */

    *ra = tmpra;
    *dec = tmpdec;
  }
  else if (dirflag < 0)
  {
  
    /* Remove aberration to EOD from J2000 */
  
    Aberration(&tmpra, &tmpdec, -1);
  
    /* Remove nutation to EOD from J2000 */
  
    Nutation(&tmpra, &tmpdec, -1);
  
    /* Remove precession to EOD from J2000 */
  
    Precession(&tmpra, &tmpdec, -1);

    /* Return the J2000 coordinates that are in the local variables */
  
    *ra = tmpra;
    *dec = tmpdec;  
  }
}



/* Evaluate proper motion to EOD for coordinates in the catalog epoch 
 * Call this in the form ProperMotion(epoch,&ra,&dec,pm_ra,pm_dec)
 * Units of 
 *   ra in hours 
 *   dec in degrees 
 *   pm_ra in seconds of time per year (note units)
 *   pm_dec in seconds of arc per year (note units) 
 *
 */
 
void ProperMotion(double epoch, double *ra, double *dec, 
  double pm_ra, double pm_dec)
{
  
  double ra_in, dec_in, ra_out, dec_out;
  double ut, jdfixed, jdnow, T;
  int year, month, day;
   
  /* JD for the fixed epoch */
  year = (int) epoch;
  month = 1;
  day = 1;
  ut = 12.0; 
  jdfixed = CalcJD ( year, month, day, ut ) + frac(epoch)*365.25;

  /* JD for epoch of date */

  jdnow = JDNow();
  
  /* Elapsed JD years */
     
  T = (jdnow - jdfixed)/365.25;

  /* Fetch the input values for ra and dec and save in radians */

  ra_in = *ra;
  dec_in = *dec;
  
  /* Calculate the new coordinates with proper motion */
   
  ra_out = ra_in + T*pm_ra/86400.;
  dec_out = dec_in + T*pm_dec/3600.;
  
  /* Check for range and adjust to -90 -> +90 and 0 -> 24 and if needed */
  
  if (dec_out > 90. ) 
  {
    dec_out = 180. - dec_out;
    ra_out = ra_out + 12.;
  }
  if (dec_out < -90. ) 
  {
    dec_out = -180. - dec_out;
    ra_out = ra_out + 12.;
  }  

  ra_out = Map24(ra_out);        

  /* Return values to the locations pointed to by ra and dec */

  *ra = ra_out;
  *dec = dec_out;
        
}


/* Add or remove nutation for this EOD 
 *
 * Call this in the form Nutation(&ra, &dec, dirflag) 
 *
 * Input: 
 *   Pointers to ra and dec
 *   Integer flag >=0 for add and <0 for subtract 
 * Output: 
 *   In place pointers to modified ra and dec 
 */
 

void Nutation(double *ra, double *dec, int dirflag)
{
  double ra_in, dec_in, ra_out, dec_out;
  double lambda, beta;
  double elong, elat, dlong;
  double dpsi, deps, eps0;
  double dra, ddec;
  double dir;

  /* Routine will add nutation by default */

  dir = 1.0;
  if (dirflag < 0)
  {
    dir = -1.0;
  }

  ra_in = *ra;
  dec_in = *dec;
  
  /* Near the celestial pole convert to ecliptic coordinates */

  if(fabs(dec_in) > (double) 85.)
  {
    CelestialToEcliptical(ra_in, dec_in, &lambda, &beta);
  
    elong = lambda;
    elat = beta;

    dlong = dir*NLongitude();
    elong = elong + dlong;

    EclipticalToCelestial(elong, elat, &ra_out, &dec_out);
  }
  else
  {
    dpsi = dir*NLongitude();
    eps0 = MeanObliquity();
    deps = dir*NObliquity();
    dra = (cos(eps0*PI/180.) + 
      sin(eps0*PI/180.)*sin(ra_in*PI/12.)*tan(dec_in*PI/180.))*dpsi -
      cos(ra_in*PI/12.)*tan(dec_in*PI/180.)*deps;
    dra = dra/15.;
    ddec = sin(eps0*PI/180.)*cos(ra_in*PI/12.)*dpsi +
      sin(ra_in*PI/12.)*deps;
    ra_out = ra_in + dra;
    dec_out = dec_in + ddec;
  }         

  *ra = ra_out;
  *dec = dec_out;

}


/* Add or remove stellar aberration for this EOD 
 *
 * Call this in the form Aberration(&ra, &dec, dirflag) 
 *
 * Input: 
 *   Pointers to ra and dec 
 *   Integer flag >=0 for add and <0 for subtract 
 * Output: 
 *   In place pointers to modified ra and dec 
 *
 * The routine is exact for adding stellar aberration to celestial
 *   coordinates.  It is approximate for removing abberration from
 *   apparent coordinates since the amount of aberration depends on
 *   true celestial coordinates, not the apparent coordinates.  For
 *   greater accuracy the routine should iterate a few times but we
 *   only need seconds of arc corrections so this is adequate for
 *   pointing.
 *
 */

void Aberration(double *ra, double *dec, int dirflag)
{  
  double ra_in=0.0, dec_in=0.0, ra_out=0.0, dec_out=0.0;
  double lambda=0.0, beta=0.0;
  double elong=0.0, elat=0.0;  
  double dlong=0.0, dlat=0.0; 
  double ec=0.0, lp=0.0, glsun=0.0, ka=0.0, eps0=0.0;
  double c1=0.0, c2=0.0;
  double dra=0.0, ddec=0.0;
  double dir=0.0;
  
  /* Routine will add stellar aberration by default */

  dir = 1.0;
  if (dirflag < 0)
  {
    dir = -1.0;
  }
  
  ka = 20.49552;
  glsun = LongitudeSun();
  ec = Eccentricity();
  lp = LongitudePerihelion();
  eps0 = MeanObliquity();
  ra_in = *ra;
  dec_in = *dec;

  /* Near the celestial pole convert to ecliptic coordinates */

  if(fabs(dec_in) > (double) 85.)
  {
    CelestialToEcliptical(ra_in, dec_in, &lambda, &beta);
    elong = lambda;
    elat = beta;
      
    c1 = -cos((glsun - elong)*PI/180.) + ec*cos((lp - elong)*PI/180.);
    c2 = cos(elat*PI/180.);

    /* Indeterminate dlong near the ecliptic pole */  

    if(c2 < (double) 0.0001) c2 = 0.0001;
    dlong = dir*ka*(c1/c2)/3600.;
     
    c1 = - sin(elat*PI/180.);
    c2 = sin((glsun - elong)*PI/180.) - ec*sin((lp - elong)*PI/180.);
    dlat = dir*ka*c1*c2/3600.;
    elong = elong + dlong;
    elat = elat + dlat;
    
    /* Check for range and adjust to -90 -> +90 and 0 -> 360 if needed */
    
    elat = Map180(elat);
    if (elat > 90. ) 
    {
      elat = 180. - dec_out;
      elong = elong + 180.;
    }
    if (elat < -90. ) 
    {
      elat = -180. - elat;
      elong = elong + 180.;
    }  
    elong = Map360(elong);
    
    EclipticalToCelestial(elong, elat, &ra_out, &dec_out);
  }
  else
  {
    dra = (-cos(ra_in*PI/12.)*cos(glsun*PI/180.)*cos(eps0*PI/180.)
      - sin(ra_in*PI/12.)*sin(glsun*PI/180.)
      + ec*cos(ra_in*PI/12.)*cos(lp*PI/180.)*cos(eps0*PI/180)
      + ec*sin(ra_in*PI/12.)*sin(lp*PI/180.))/cos(dec_in*PI/180.);
    
    dra = dir*ka*dra;   
            
    dra = dra/(15.*3600.);  
    
    ddec = -cos(glsun*PI/180)*cos(eps0*PI/180.)
      *(tan(eps0*PI/180)*cos(dec_in*PI/180.) 
      - sin(ra_in*PI/12.)*sin(dec_in*PI/180.))
      - cos(ra_in*PI/12.)*sin(dec_in*PI/180.)*sin(glsun*PI/180)
      + ec*cos(lp*PI/180.)*cos(eps0*PI/180.)
      *(tan(eps0*PI/180.)*cos(dec_in*PI/180.) 
      - sin(ra_in*PI/12.)*sin(dec_in*PI/180.))
      + ec*cos(ra_in*PI/12.)*sin(dec_in*PI/180.)*sin(lp*PI/180.);
      
    ddec = dir*ka*ddec;
    ddec = ddec/3600.; 
    
    ra_out = ra_in + dra;
    dec_out = dec_in +ddec;
  }  


  /* Return values to the locations pointed to by ra and dec */

  *ra = ra_out;
  *dec = dec_out;  
  
}

////////////////////////////////////////////////////////////////////
// Convert local ha and dec to local az and alt 
// Geographic azimuth convention is followed:
// Due north is zero and azimuth increases from north to east
////////////////////////////////////////////////////////////////////
void EquatorialToHorizontal( double nSiteLatitude, double ha, double dec,
								double *az, double *alt )
{
	double phi, altitude, azimuth ;

	ha = ha*PI/12.;
	phi = nSiteLatitude*PI/180.;
	dec = dec*PI/180.;

	altitude = asin(sin(phi)*sin(dec)+cos(phi)*cos(dec)*cos(ha));
	altitude = altitude*180.0/PI;

	azimuth = atan2(-cos(dec)*sin(ha),
	sin(dec)*cos(phi)-sin(phi)*cos(dec)*cos(ha));
	azimuth = azimuth*180.0/PI;

	azimuth = Map360(azimuth);

	*alt = altitude;
	*az = azimuth;
}

////////////////////////////////////////////////////////////////////
// Convert local az and alt to local ha and dec
// Geographic azimuth convention is followed:
// Due north is zero and azimuth increases from north to east
////////////////////////////////////////////////////////////////////
void HorizontalToEquatorial( double nSiteLatitude, double az, double alt, 
								double *ha, double *dec )
{
	double phi, hourangle, declination ;

	alt = alt*PI/180.;
	az = Map360(az);
	az = az*PI/180.;
	phi = nSiteLatitude*PI/180.;

	hourangle = atan2(-sin(az)*cos(alt),cos(phi)*sin(alt)-sin(phi)*cos(alt)*cos(az));
	declination = asin(sin(phi)*sin(alt) + cos(phi)*cos(alt)*cos(az));
	hourangle = Map12(hourangle*12./PI);
	declination = Map180(declination*180./PI);

	/// Test for hemisphere 
	if( declination > 90. )
	{
		*ha = Map12(hourangle + 12.);
		*dec = 180. - declination;

	} else if(declination < -90.)
	{
		*ha = Map12(hourangle + 12.);
		*dec = declination + 180;

	} else
	{ 
		*ha = hourangle;
		*dec = declination;
	}
}


/* Convert celestial to ecliptical coordinates for the EOD */
/* ra is the input right ascension in hours */
/* dec is the input declination in degrees */
/* lambda is the pointer to the output ecliptical longitude in degrees */
/* beta is the pointer to the output ecliptical latitude in degrees */

void CelestialToEcliptical(double ra, double dec, double *lambda, double *beta)
{
  double elong, elat, eps;
  
  ra = ra*PI/12.;
  dec = dec*PI/180.;
  eps = MeanObliquity();
  eps = eps*PI/180;
  elong = atan2(sin(ra)*cos(eps) + tan(dec)*sin(eps),cos(ra));
  elong = Map360(elong*180./PI);
  elat = asin(sin(dec)*cos(eps)-cos(dec)*sin(eps)*sin(ra));
  elat = Map180(elat*180./PI);

  /* Test for hemisphere */
  
  if(elat > 90.)
  {
    *lambda = Map360(elong + 180.);
    *beta = 180. - elat;
  }
  else if(elat < -90.)
  {
    *lambda = Map360(elong +180.);
    *beta = elat + 180;
  } 
  else
  { 
    *lambda = elong;
    *beta = elat;
  }  
}


/* Convert ecliptical to celestial coordinates for the EOD */
/* lambda is the input ecliptical longitude in degrees */
/* beta is the input ecliptical latitude in degrees */
/* ra is a pointer to the output right ascension in hours */
/* dec is a pointer to the output declination in degrees */

void EclipticalToCelestial(double lambda, double beta, double *ra, double *dec)
{
  double ra_out, dec_out, eps;
  
  lambda = lambda*PI/180.;
  beta = beta*PI/180.;
  eps = MeanObliquity()*PI/180;  
  ra_out = atan2(sin(lambda)*cos(eps) - tan(beta)*sin(eps), cos(lambda));
  ra_out = Map24(ra_out*12./PI);
  dec_out = asin(sin(beta)*cos(eps) + cos(beta)*sin(eps)*sin(lambda));
  dec_out = Map180(dec_out*180./PI);
  
  /* Test for hemisphere */
  
  if(dec_out > 90.)
  {
    *ra = Map24(ra_out + 12.);
    *dec = 180. - dec_out;
  }
  else if(dec_out < -90.)
  {
    *ra = Map24(ra_out + 12.);
    *dec = dec_out + 180;
  } 
  else
  { 
    *ra = ra_out;
    *dec = dec_out;
  }
  
}


/* True obliquity of the ecliptic for the EOD in degrees*/

double TrueObliquity(void)
{
  double eps, eps0, deps;
  
  eps0 = MeanObliquity();
  deps = NObliquity();
  eps = eps0 + deps;

  return (eps);
}


/* Routines for dynamical astronomy 
 *
 *
 * Time matters ... 
 *
 * TAI is the International Atomic Time standard. 
 * TT is the terrestrial (dynamical) time for ephemeris calculations. 
 * TT runs (very nearly) at the SI second on the Earth's rotating geoid. 
 *
 * The difference TT - TAI is 32.184 seconds and does not change. 
 *
 * UTC is Coordinated Universal Time, by which we set network clocks. 
 * UTC runs at the TAI rate.  
 * UT1 runs at the  the slowly decreasing mean solar day rate. 
 * The UT1 counter must read 12h at each mean solar noon, 
 *   but since the rotation of the Earth is slowing, 
 *   by the UTC counter this event is at an increasingly later time. 
 *
 * The difference UTC - UT1 < 0.9 seconds is maintained by  
 *   not incrementing the UTC counter for 1 second when necessary. 
 *   This uncounted second is called a Leap Second. 
 *
 * Whenever the UTC clock counter is not incremented for one tick, 
 *   UTC falls behind TT an additional second. TAI and TT are 
 *   always incremented. 
 *
 * TT = TAI + 32.184 is the basis for the ephemeris here. 
 * The difference between TAI and UTC is the number of leap seconds 
 *   accumulated since the clocks were initially synchronized. 
 * We define LEAPSECONDS = TAI - UTC in the header file.
 * It was incremented to 32 on January 1.0, 1999. 
 * It will be 33 after January 1.0, 2006. 
 * Here dt = TT - UTC = LEAPSECONDS + 32.184 
 *
 */

/* Mean obliquity of the ecliptic for the EOD in degrees */

double MeanObliquity(void)
{
  double eps0;
  double jdnow, dt, t;
  
  dt = LEAPSECONDS;
  dt = dt + 32.184;
  
  /* Change units to centuries */
    
  dt = dt/ (36525 * 24. * 60. * 60.);
  
  /* JD for epoch of date */

  jdnow = JDNow();

  /* Julian centuries for the EOD from a base epoch 2000.0 */

  t = (jdnow - 2451545.0) / 36525.0;
  
  /* Correct for dt = tdt - ut1 (not significant) */
  
  t = t + dt ;
    
  /* Mean obliquity in degrees */

  eps0 = 23.0+26./60+21.448/3600.;
  eps0 = eps0 +(- 46.8150*t - 0.00059*t*t + 0.001813*t*t*t)/3600.;
  
  return (eps0);
}


/* Nutation of the obliquity of the ecliptic for the EOD in degrees */

double NObliquity(void)
{
  double deps, lsun, lmoon, omega;
  double jdnow, dt, t;
  
  dt = LEAPSECONDS;
  dt = dt + 32.184;
  
  /* Change units to centuries */
    
  dt = dt/ (36525 * 24. * 60. * 60.);
  
  /* JD for epoch of date */

  jdnow = JDNow();

  /* Julian centuries for the EOD from a base epoch 2000.0 */

  t = (jdnow - 2451545.0) / 36525.0;
  
  /* Correct for dt = tt - ut1  */
  
  t = t + dt ;
    
  /* Longitude of the ascending node of the Moon's mean orbit in degrees */
  
  omega = 125.04452 - 1934.136261*t + 0.0020708*t*t + t*t*t/450000.;
    
  /* Mean longitudes of the Sun and the Moon in degrees */
  
  lsun = Map360(280.4665 + 3600.7698*t);
  lmoon = Map360(218.3165 + 481267.8813*t);
  
  /* Convert to radians */

  omega = omega*PI/180.;
  lsun = lsun*PI/180.;
  lmoon = lmoon*PI/180.;
  
  /* Nutation of the obliquity in seconds of arc for the EOD */
  
  deps = 9.20*cos(omega) + 0.57*cos(2.*lsun) + 
    0.1*cos(2.*lmoon) - 0.09*cos(2.*omega);
      
  /* Convert to degrees */
 
  deps = deps/3600.;
  
  return (deps);
}


/* Nutation of the longitude of the ecliptic for the EOD in degrees */

double NLongitude(void)
{
  double dpsi, lsun, lmoon, omega;
  double jdnow, dt, t;
    
  dt = LEAPSECONDS;
  dt = dt + 32.184;
  
  /* Change units to centuries */
    
  dt = dt/ (36525 * 24. * 60. * 60.);
  
  /* JD for epoch of date */

  jdnow = JDNow();
  
  /* Julian centuries for the EOD from a base epoch 2000.0 */

  t = (jdnow - 2451545.0) / 36525.0;
  
  /* Correct for dt = tt - ut1  */
  
  t = t + dt ;
    
  /* Longitude of the ascending node of the Moon's mean orbit */
  
  omega = Map360(125.04452 - 1934.136261*t + 0.0020708*t*t + t*t*t/450000.);

  /* Mean longitude of the Moon */
  
  lmoon = Map360(218.31654591 + 481267.88134236*t
    - 0.00163*t*t + t*t*t/538841. - t*t*t*t/65194000.);
    
  /* Mean longitude of the Sun */
  
  lsun = LongitudeSun();    
      
  /* Convert to radians */
  
  omega = omega*PI/180.;  
  lsun = lsun*PI/180.;
  lmoon = lmoon*PI/180.;

  /* Nutation in longitude in seconds of arc for the EOD */
  
  dpsi = -17.20*sin(omega) - 1.32*sin(2.*lsun) - 
    0.23*sin(2.*lmoon) + 0.21*sin(2.*omega);
        
  /* Convert to degrees */
    
  dpsi = dpsi/3600.;
  
  return (dpsi);
}


/* True geometric solar longitude for the EOD in degrees */

double LongitudeSun(void)
{
  double lsun, glsun, msun, csun;
  double jdnow, dt, t;
    
  dt = LEAPSECONDS;
  dt = dt + 32.184;
  
  /* Change units to centuries */
    
  dt = dt/ (36525 * 24. * 60. * 60.);
  
  /* JD for epoch of date */

  jdnow = JDNow();

  /* Julian centuries for the EOD from a base epoch 2000.0 */

  t = (jdnow - 2451545.0) / 36525.0;
  
  /* Correct for dt = tt - ut1  */
  
  t = t + dt ;
  
  lsun = Map360(280.46645 + 36000.76983*t + 0.0003032*t*t);
  
  /* Mean anomaly */
  
  msun = Map360(357.52910 + 35999.05030*t - 0.0001559*t*t -
    0.00000048*t*t*t);
    
  msun = msun*PI/180.;  
    
  /* Sun's center */
  
  csun = (1.9146000 - 0.004817*t - 0.000014*t*t)*sin(msun)  
    + (0.019993 - 0.000101*t)*sin(2.*msun)
    + 0.000290*sin(3.*msun);
    
  /* True geometric longitude */  
    
  glsun = Map360(lsun  + csun);
 
  return (glsun);
}


/* Eccentricity of the Earth's orbit at EOD */

double Eccentricity(void)
{
  double ec;
  double jdnow, dt, t;
  
  
  dt = LEAPSECONDS;
  dt = dt + 32.184;
  
  /* Change units to centuries */
    
  dt = dt/ (36525 * 24. * 60. * 60.);
  
  /* JD for epoch of date */

  jdnow = JDNow();

  /* Julian centuries for the EOD from a base epoch 2000.0 */

  t = (jdnow - 2451545.0) / 36525.0;
  
  /* Correct for dt = tt - ut1  */
  
  t = t + dt ;
    
  ec = 0.016708617 - 0.000042037*t - 0.0000001236*t*t;

  return (ec);
}


/* Longitude of perihelion of the Earth's orbit in degrees for the EOD */

double LongitudePerihelion(void)
{
  double lp;
  double jdnow, dt, t;
  
  dt = LEAPSECONDS;
  dt = dt + 32.184;
  
  /* Change units to centuries */
    
  dt = dt/ (36525 * 24. * 60. * 60.);
  
  /* JD for epoch of date */

  jdnow = JDNow();

  /* Julian centuries for the EOD from a base epoch 2000.0 */

  t = (jdnow - 2451545.0) / 36525.0;
  
  /* Correct for dt = tt - ut1  */
  
  t = t + dt ;
    
  /* Longitude of perihelion */
  
  lp = 102.93735 + 0.71953*t + 0.00046*t*t;
  lp = Map360(lp);

  return (lp);
}

