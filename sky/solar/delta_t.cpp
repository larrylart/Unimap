#include <math.h>
#include <stdio.h>
#include "afuncs.h"

/* The following method for computing Delta-T works as follows.  For dates
between 1620 and 2008,  the value comes from the 'delta_t_table' (copied
in turn from Meeus' _Astronomical Algorithms_),  extended using data from
the USNO sites referenced below.  The table gives delta-t at two-year
intervals;  between those values,  a linear interpolation is used.  If
the year is before 1620,  one of two quadratic approximations is used.
If the date is after the end of the table,  a linear extrapolation is
used,  with a quadratic term added to that which assumes an acceleration
of 32.5 seconds/century^2.

   Updated 2 June 2001, 4 Jan 2005, 1 Nov 2007, 9 Jul 2008 to include
new Delta-t data from:

ftp://maia.usno.navy.mil/ser7/deltat.data
ftp://maia.usno.navy.mil/ser7/deltat.preds
*/

#define DELTA_T_TABLE_SIZE 205

static const short delta_t_table[DELTA_T_TABLE_SIZE] =
 {12400, 11500, 10600, 9800, 9100, 8500, 7900,  /*  1620-1632 */
   7400, 7000, 6500, 6200, 5800, 5500, 5300,    /*  1634-1646 */
   5000, 4800, 4600, 4400, 4200, 4000, 3700,    /*  1648-1660 */
   3500, 3300, 3100, 2800, 2600, 2400, 2200,    /*  1662-1674 */
   2000, 1800, 1600, 1400, 1300, 1200, 1100,    /*  1676-1688 */
   1000,  900,  900,  900,  900,  900,  900,    /*  1690-1702 */
    900,  900, 1000, 1000, 1000, 1000, 1000,    /*  1704-1716 */
   1100, 1100, 1100, 1100, 1100, 1100, 1100,    /*  1718-1730 */
   1100, 1200, 1200, 1200, 1200, 1200, 1300,    /*  1732-1744 */
   1300, 1300, 1300, 1400, 1400, 1400, 1500,    /*  1746-1758 */
   1500, 1500, 1500, 1600, 1600, 1600, 1600,    /*  1760-1772 */
   1600, 1700, 1700, 1700, 1700, 1700, 1700,    /*  1774-1786 */
   1700, 1700, 1600, 1600, 1500, 1400, 1370,    /*  1788-1800 */
   1310, 1270, 1250, 1250, 1250, 1250, 1250,    /*  1802-1814 */
   1250, 1230, 1200, 1140, 1060,  960,  860,    /*  1816-1828 */
    750,  660,  600,  570,  560,  570,  590,    /*  1830-1842 */
    620,  650,  680,  710,  730,  750,  770,    /*  1844-1856 */
    780,  790,  750,  640,  540,  290,  160,    /*  1858-1870 */
   -100, -270, -360, -470, -540, -520, -550,    /*  1872-1884 */
   -560, -580, -590, -620, -640, -610, -470,    /*  1886-1898 */
   -270,    0,  260,  540,  770, 1050, 1340,    /*  1900-1912 */
   1600, 1820, 2020, 2120, 2240, 2350, 2390,    /*  1914-1926 */
   2430, 2400, 2390, 2390, 2370, 2400, 2430,    /*  1928-1940 */
   2530, 2620, 2730, 2820, 2910, 3000, 3070,    /*  1942-1954 */
   3140, 3220, 3310, 3400, 3500, 3650, 3830,    /*  1956-1968 */
   4020, 4220, 4450, 4650, 4850, 5050, 5220,    /*  1970-1982 */
   5380, 5490, 5580,                            /*  1984-1988 */
  5686,    /* 1990  1 1    56.8554     .3286        -24.6714 */
  5831,    /* 1992  1 1    58.3093    -.1253        -26.1253 */
  5998,    /* 1994  1 1    59.9847     .1993        -27.8007 */
  6163,    /* 1996  1 1    61.6287     .5553        -29.4447 */
  6297,    /* 1998  1 1    62.9659     .2181        -30.7819 */
  6383,    /* 2000  1 1    63.8285268  .3554732     -31.6445268 */
  6430,    /* 2002  1 1    64.2998152 -.1158152     -32.1158152 */
  6457,    /* 2004  1 1    64.5736400 -.3896400     -32.3896400 */
  6485,    /* 2006  1 1    64.8452                              */
  6546,	   /* 2008  1 1:   65.4574                              */
  6570,	   // 2009  2  1  6570
  6650,		// 2010
  6710,		// 2011
  6800,		// 2012
  6800,		// 2013
  6900,		// 2014
  6900,		// 2015
  7000,		// 2016
  7000 };	// 2017

/* 8 Aug 2000:  Some people have expressed an interest in being able to
   insert their own formulae for Delta-T while running Guide.  I've
   enabled this by letting people add a string of coefficients in
   GUIDE.DAT;  gory details of how the string works are at

http://www.projectpluto.com/update7.htm#delta_t_def

   The following functions parse out the string of coefficients and
   let you set which string is used. */

static int evaluate_delta_t_string( const double year, double *delta_t,
                              const char *dt_string)
{
   int i, rval = -1;

   for( i = 0; rval && dt_string && dt_string[i]; i++)
      if( !i || dt_string[i - 1] == ';')
         {
         double year1, year2, constant, linear = 0., quadratic = 0.;

                         /* there must be at least year1, year2,  and  */
                         /* constant;  linear & quadratic are optional */
         if( sscanf( dt_string + i, "%lf,%lf:%lf,%lf,%lf",
               &year1, &year2, &constant, &linear, &quadratic) > 2)
            if( year >= year1 && year <= year2)
               {
               const double dt = (year - 2000.) / 100.;

               *delta_t = constant + dt * (linear + dt * quadratic);
               rval = 0;            /* we got it */
               }
         }
   return( rval);
}

static const char *td_minus_dt_string = NULL;

void reset_td_minus_dt_string( const char *string)
{
   td_minus_dt_string = string;
}

/* "-10000,948:2715.6,573.36,46.5;948,1620:50.6,67.5,22.5"; */

/* the #ifdef DONT_LIKE_THIS_WAY comments out the quadratic expression   */
/* for delta t arrived at by Morrison and Stephenson (Meeus, p 73, eq.   */
/* 9.1).  My problem with this expression is that it gives delta_t of    */
/* 102.3 seconds in the year 2000,  which would mean a sudden slowdown   */
/* in the earth's spin.  Since then,  I've occasionally had to adjust    */
/* the constant and linear terms to line up with the USNO predictions    */
/* shown in the above table (and at the above-referenced URLs.)          */

double td_minus_ut( const double jd)
{
   double year, dt, rval;
   /* first convert t from JD to years */

   year = 2000. + (jd - 2451545.) / 365.25;
   if( td_minus_dt_string)
      if( !evaluate_delta_t_string( year, &rval, td_minus_dt_string))
         return( rval);
   dt = (year - 2000.) / 100.;
   if( year < 948.)
      rval = 2715.6 + dt * (573.36 + 46.5 * dt);
   else if( year < 1620.)
      rval = 50.6 + dt * (67.5 + 22.5 * dt);
   else     /* from 1620 to +infinity */
      {
      double index_loc = (year - 1620.) / 2.;
      int index = (int)index_loc;
      const short *tptr;

      if( index > DELTA_T_TABLE_SIZE - 2)       /* running off end of table */
         index = DELTA_T_TABLE_SIZE - 2;
      dt = index_loc - (double)index;
      tptr = delta_t_table + index;
      rval = (double)tptr[0] + (double)(tptr[1] - tptr[0]) * dt;
      rval /= 100.;
      if( dt > 1.)            /* again, past end of table */
         {
         dt = (dt - 1.) / 50.;    /* cvt to centuries past end,  and add the */
         rval += 32.5 * dt * dt;  /* same 32.5 sec/cy^2 used by Stephenson   */
         }
      }
#ifdef TRY_OMITTING
   if( year < 1620.)       /* apply correction from _Astro Almanac_, */
      {                    /* 1991,  page K8: */
      const double n = -23.8946;   /* corrected lunar secular acceleration */

      rval -= 0.000091 * (n + 26.) * (year - 1955.) * (year - 1955.);
      }
#endif
   return( rval);
}

/* Some notes for other time systems that I may,  someday,  get around
to implementing:

TDT = TAI + 32.184 seconds

TAI & UTC differ by an integer number of seconds;  after 1/1/1999,
for example,  TAI-UTC = 32 seconds,  TDT-UTC = 64.184 seconds.
USNO & IERS bulletins often give TDT-UT1;  from this you can get
UT1-UTC (always between -1 and +1 second)

TCB - TDB = L * (jd - 2443144.5) * 86400,
   where L=1.550505e-08

TAI-GPS = 19 seconds ("fixed" as of 1980;  at that point,  GPS time
was identical to UTC.  But since then,  14 leap seconds have been added
to UTC,  so that GPS and UTC have slipped apart by that much.)
*/

#define N_LEAP_SECONDS 25

double td_minus_utc( const double jd_utc)
{
   const double tdt_minus_tai = 32.184;
   const double mjd_utc = jd_utc - 2400000.5;
   int i;

   if( mjd_utc < 41317.)  /* between jan 1961 & dec 1972 */
      {
      static const unsigned short ranges[13] =  { 37300, 37512, 37665,
                                    38334, 38395, 38486, 38639, 38761,
                                    38820, 38942, 39004, 39126, 39887 };
      static const double offset[13] = { 1.4228180,  1.3728180, 1.8458580,
                   1.9458580, 3.2401300, 3.3401300, 3.4401300, 3.5401300,
                   3.6401300, 3.7401300, 3.8401300, 4.3131700, 4.2131700 };
      static const unsigned short mjd_off[13] = { 37300, 37300, 37665,
                                    37665, 38761, 38761, 38761, 38761,
                                    38761, 38761, 38761, 39126, 39126 };
      static const short scale[13] =            { 12960, 12960, 11232,
                                    11232, 12960, 12960, 12960, 12960,
                                    12960, 12960, 12960, 25920, 25920 };
      for( i = 12; i >= 0; i--)
         if( mjd_utc >= (double)ranges[i])
            {
            const double tai_minus_utc = offset[i] +
                  (mjd_utc - (double)mjd_off[i]) * (double)scale[i] * 1.e-7;

            return( tdt_minus_tai + tai_minus_utc);
            }
      }
   else              /* integral leap seconds */
      {
      static const unsigned short leap_intervals[N_LEAP_SECONDS] = {
                 41317, 41499, 41683,    /* jan 1972, jul 1972, jan 1973 */
                 42048, 42413, 42778,    /* jan 1974, jan 1975, jan 1976 */
                 43144, 43509, 43874,    /* jan 1977, jan 1978, jan 1979 */
                 44239, 44786, 45151,    /* jan 1980, jul 1981, jul 1982 */
                 45516, 46247, 47161,    /* jul 1983, jul 1985, jan 1988 */
                 47892, 48257, 48804,    /* jan 1990, jan 1991, jul 1992 */
                 49169, 49534, 50083,    /* jul 1993, jul 1994, jan 1996 */
                 50630, 51179, 53736,    /* jul 1997, jan 1998, jan 2006 */
                 54832 };                /* jan 2009 */

      for( i = N_LEAP_SECONDS - 1; i >= 0; i--)
         if( mjd_utc >= (double)leap_intervals[i])
            return( (double)(i + 10) + tdt_minus_tai);
      }
                     /* still here?  Must be before jan 1961,  so UTC = UT1: */
   return( td_minus_ut( jd_utc));
}
