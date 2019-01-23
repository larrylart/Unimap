/////////////////////////////////////////////////////////////////////
// Protocol header for telescope control algoriths 
//
// Copyright 2008 John Kielkopf 
// kielkopf@louisville.edu
//
// Date: September 12, 2008 
// Version: 1.06 
//
/////////////////////////////////////////////////////////////////////
 
#ifndef _ASTRO_FUNCTIONS_H
#define _ASTRO_FUNCTIONS_H

// The difference between terrestrial and ephemeris time is determined 
// by the accumulated number of leapseconds at this moment.
#ifndef LEAPSECONDS
#define LEAPSECONDS    33.0
#endif

// Important physical constants 
#ifndef PI
#define PI             3.14159265358
#endif

// Calculation utilities
double Map24(double hour);
double Map12(double hour);
double Map360(double angle);
double Map180(double angle);
double frac(double x);

// Date and time 
double CalcJD(int ny, int nm, int nd, double ut);
double JDNow(void);
double CalcLST(int year, int month, int day, double ut, double glong);
double LSTNow( double nSiteLongitude );
double UTNow(void);

// Precession, nutation and stellar aberration at any epoch FK5 coordinates 
void PrecessToEOD( double epoch, double *ra, double *dec );
void PrecessToEpoch( double epoch, double *ra, double *dec );
void ProperMotion( double epoch, double *ra, double *dec, double pm_ra, double pm_dec );

// J2000 to and from EOD 
void Precession( double *ra, double *dec, int dirflag );
void Apparent( double *ra, double *dec, int dirflag );  

// EOD corrections 
void Nutation(double *ra, double *dec, int dirflag);
void Aberration(double *ra, double *dec, int dirflag);

// Coordinate transformations 
void EquatorialToHorizontal( double nSiteLatitude, double ha, double dec, double *az, double *alt );
void HorizontalToEquatorial( double nSiteLatitude, double az, double alt, double *ha, double *dec );
void CelestialToEcliptical( double ra, double dec, double *lambda, double *beta );
void EclipticalToCelestial( double lambda, double beta, double *ra, double *dec );

// Earth and its orbit 
double TrueObliquity(void);
double MeanObliquity(void);
double NObliquity(void);
double NLongitude(void);
double LongitudeSun(void);
double Eccentricity(void);
double LongitudePerihelion(void);

#endif
