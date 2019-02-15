
#ifndef _SAT_CODE_DEEP_H
#define _SAT_CODE_DEEP_H

#include <math.h>
#include <stdlib.h>

#include "norad_in.h"

// omega_E = number of (sidereal) rotations of the earth per UT day: 
const double omega_E = 1.00273790934;

#ifdef USE_ACCURATE_ANOMALISTICS
         /* The anomalistic month is the mean time it takes the moon to go
            from perigee to perigee.  The anomalistic year is the mean time
            it takes the earth to go from perihelion to perihelion.  */
const double days_per_anomalistic_month =  27.554551;
const double days_per_anomalistic_year = 365.259635864;
const double zns_per_day = twopi / days_per_anomalistic_year;
const double zns_per_min = zns_per_day / minutes_per_day;
const double znl_per_day = twopi / days_per_anomalistic_month;
const double znl_per_min = znl_per_day / minutes_per_day;
         /* thdt = angular velocity of the earth,  in radians/minute: */
const double thdt = twopi * omega_E / minutes_per_day;
#else
const double zns_per_min = 1.19459E-5;
const double zns_per_day = 0.017201977;
const double znl_per_day = 0.228027132;
const double znl_per_min = 1.5835218E-4;
const double thdt =   4.37526908801129966e-3;
#endif
         /* zes = mean eccentricity of earth's orbit */
         /* zel = mean eccentricity of the moon's orbit */
#define zes      0.01675
#define zel      0.05490

extern void Deep_dpinit( const tle_t *tle, deep_arg_t *deep_arg );
extern void Deep_dpsec( const tle_t *tle, deep_arg_t *deep_arg );
extern void Deep_dpper( const tle_t *tle, deep_arg_t *deep_arg );

#endif

