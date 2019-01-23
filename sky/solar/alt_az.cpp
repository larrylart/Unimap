
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "afuncs.h"
//#include "lunar.h"

#define PI 3.141592653589793238462643383279502884197169399375105
#define CONVERT (1000000. * 180. / PI)
#define TWO_PI (2. * PI)
#define J2000 2451545.0

static void ra_dec_to_alt_az( const double hr_ang, const double dec,
        double *alt, double *az, const double lat)
{
   double temp, cos_lat = cos( lat);

   *alt = asine( sin( lat) * sin( dec) + cos_lat * cos( dec) * cos( hr_ang));
   if( cos_lat < .00001)         /* polar case */
      *az = hr_ang;
   else
      {
      temp = (sin( dec) - sin( *alt) * sin( lat)) / (cos( *alt) * cos_lat);
      temp = PI - acose( temp);
      *az = ((sin( hr_ang) < 0.) ? temp : -temp);
      }
}

void full_ra_dec_to_alt_az( const DPT *ra_dec,
                DPT *alt_az,
                DPT *loc_epoch, const DPT *latlon,
                const double jd_utc, double *hr_ang)
{
   double ha, nutation_lon, t_centuries = (jd_utc - J2000) / 36525.;
   const double cos_obliquity = .917482;
   DPT loc_at_epoch;
   memset( &loc_at_epoch, 0, sizeof(DPT) );

   if( ra_dec)
      precess_pt( &loc_at_epoch, ra_dec, 2000., 2000. + t_centuries * 100.);
   ha = -loc_at_epoch.x - (green_sidereal_time( jd_utc) + latlon->x);
   Sol_nutation( t_centuries, &nutation_lon, NULL);
   ha -= cos_obliquity * nutation_lon * (PI / 180.) / 3600.;
   ha = fmod( ha, TWO_PI);
   if( ha > PI) ha -= TWO_PI;
   if( ha <-PI) ha += TWO_PI;
   ra_dec_to_alt_az( ha, loc_at_epoch.y, &alt_az->y, &alt_az->x, latlon->y);
   if( hr_ang)
      *hr_ang = ha;
   if( loc_epoch)
      *loc_epoch = loc_at_epoch;
}

void alt_az_to_ra_dec( double alt, double az,
                         double *hr_ang,
                         double *dec, const double lat)
{
   double temp, sin_dec, cos_lat = cos( lat);

   if( alt > PI / 2.)
      {
      alt = PI - alt;
      az += PI;
      }
   if( alt < -PI / 2.)
      {
      alt = -PI - alt;
      az -= PI;
      }
   sin_dec = sin( lat) * sin( alt) + cos_lat * cos( alt) * cos( az);
   *dec = asine( sin_dec);
   if( cos_lat < .00001)         /* polar case */
      *hr_ang = az + PI;
   else
      {
      temp = cos_lat * cos( *dec);
      temp = (sin( alt) - sin( lat) * sin_dec) / temp;
      temp = acose( -temp);
      if( sin( az) > 0.)
         *hr_ang = PI - temp;
      else
         *hr_ang = PI + temp;
      }
}

void full_alt_az_to_ra_dec( DPT *ra_dec,
                              const DPT *alt_az,
                              const double jd_utc, const DPT *latlon)
{
   double hr_ang, ra;
   DPT tmp;

   alt_az_to_ra_dec( alt_az->y, alt_az->x, &hr_ang,
                                         &tmp.y, latlon->y);
   ra = hr_ang + green_sidereal_time( jd_utc) + latlon->x;
   tmp.x = fmod( -ra, TWO_PI);
   precess_pt( ra_dec, &tmp, 2000. + (jd_utc - J2000) / 365.25, 2000.);
}

/* The following matrix was derived from the code in 'superga2.cpp'.  */

const double * j2000_to_supergalactic_matrix( void)
{
   static const double rval[9] = {
           0.37501548,  0.34135896,  0.86188018,
          -0.89832046, -0.09572714,  0.42878511,
           0.22887497, -0.93504565,  0.27075058 };

   return( rval);
}

void ra_dec_to_supergalactic( const double ra, const double dec,
                   double *glat, double *glon)
{
   double ipt[2], opt[2];

   ipt[0] = ra;
   ipt[1] = dec;
   precess_ra_dec( j2000_to_supergalactic_matrix( ), opt, ipt, 0);
   *glon = opt[0];
   *glat = opt[1];
}

void supergalactic_to_ra_dec( const double glat, double glon,
                    double *ra, double *dec)
{
   double ipt[2], opt[2];

   ipt[0] = glon;
   ipt[1] = glat;
   precess_ra_dec( j2000_to_supergalactic_matrix( ), opt, ipt, 1);
   *ra  = opt[0];
   *dec = opt[1];
}

const double * j2000_to_galactic_matrix( void)
{
               /* The following matrix comes from _The Hipparcos & Tycho */
               /* Catalogues:  Introduction & Guide to the Data_, p 92:  */
   static const double rval[9] = {
      -.0548755604, -.8734370902, -.4838350155,
       .4941094279, -.4448296300,  .7469822445,
      -.8676661490, -.1980763734,  .4559837762 };

   return( rval);
}

void ra_dec_to_galactic( const double ra, const double dec,
                   double *glat, double *glon)
{
   double ipt[2], opt[2];

   ipt[0] = ra;
   ipt[1] = dec;
   precess_ra_dec( j2000_to_galactic_matrix( ), opt, ipt, 0);
   *glon = opt[0];
   *glat = opt[1];
}

void galactic_to_ra_dec( const double glat, double glon,
                    double *ra, double *dec)
{
   double ipt[2], opt[2];

   ipt[0] = glon;
   ipt[1] = glat;
   precess_ra_dec( j2000_to_galactic_matrix( ), opt, ipt, 1);
   *ra  = opt[0];
   *dec = opt[1];
}

void precess_pt( DPT *opt, const DPT *ipt,
                        double from_time, double to_time)
{
   double precess[9];
   double temp_opt[2], temp_ipt[2];
   int dir = 0;

   if( from_time == to_time)
      {
      *opt = *ipt;
      return;
      }
   if( from_time == 2000.)
      {
      from_time = to_time;
      to_time = 2000.;
      dir = 1;
      }
   setup_precession( precess, from_time, to_time);
   temp_ipt[0] = -ipt->x;
   temp_ipt[1] = ipt->y;
   precess_ra_dec( precess, temp_opt, temp_ipt, dir);
   opt->x = -temp_opt[0];
   opt->y = temp_opt[1];
}
