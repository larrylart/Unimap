/*
 *  norad.h v. 01.beta 03/17/2001
 *
 *  Header file for norad.c
 */

#ifndef NORAD_H
#define NORAD_H 1


/* #define RETAIN_PERTURBATION_VALUES_AT_EPOCH 1 */

#include "norad_in.h"
//#include "deep.h"

#define JD2000 2451545.5
#define JD1900 (JD2000 - 36525. - 1.)

#define DEEP_ARG_T_PARAMS     94

#define N_SGP_PARAMS          11
#define N_SGP4_PARAMS         30
#define N_SGP8_PARAMS         25
#define N_SDP4_PARAMS        (10 + DEEP_ARG_T_PARAMS)
#define N_SDP8_PARAMS        (11 + DEEP_ARG_T_PARAMS)

/* 94 = maximum possible size of the 'deep_arg_t' structure,  in 8-byte units */
/* You can use the above constants to minimize the amount of memory used,
   but if you use the following constant,  you can be assured of having
   enough memory for any of the five models: */

#define N_SAT_PARAMS         (11 + DEEP_ARG_T_PARAMS)

/* Byte 63 of the first line of a TLE contains the ephemeris type.  The */
/* following five values are recommended,  but it seems the non-zero    */
/* values are only used internally;  "published" TLEs all have type 0.  */

#define TLE_EPHEMERIS_TYPE_DEFAULT           0
#define TLE_EPHEMERIS_TYPE_SGP               1
#define TLE_EPHEMERIS_TYPE_SGP4              2
#define TLE_EPHEMERIS_TYPE_SDP4              3
#define TLE_EPHEMERIS_TYPE_SGP8              4
#define TLE_EPHEMERIS_TYPE_SDP8              5

#define SXPX_DPSEC_INTEGRATION_ORDER         0
#define SXPX_DUNDEE_COMPLIANCE               1
#define SXPX_ZERO_PERTURBATIONS_AT_EPOCH     2

/* Funtion prototypes */
/* norad.c */

         /* The Win32 version can be compiled to make a .DLL,  if the     */
         /* functions are declared to be of type __stdcall... _and_ the   */
         /* functions must be declared to be extern "C",  something I     */
         /* overlooked and added 24 Sep 2002.  The macro lets    */
         /* this coexist peacefully with other OSes.                      */

void SGP_init( double *params, const tle_t *tle);
void SGP(  const double tsince, const tle_t *tle, const double *params,
                                     double *pos, double *vel);

void SGP4_init( double *params, const tle_t *tle);
extern int  SC_SGP4( const double tsince, const tle_t *tle, const double *params,
                                     double *pos, double *vel);

void SGP8_init( double *params, const tle_t *tle);
int  SGP8( const double tsince, const tle_t *tle, const double *params,
                                     double *pos, double *vel);

void SDP8_init( double *params, const tle_t *tle);
int  SDP8( const double tsince, const tle_t *tle, const double *params,
                                     double *pos, double *vel);

int select_ephemeris( const tle_t *tle);
int parse_elements( const char *line1, const char *line2, tle_t *sat);
int tle_checksum( const char *buff);
void write_elements_in_tle_format( char *buff, const tle_t *tle);

void sxpx_set_implementation_param( const int param_index,
                                              const int new_param);
void sxpx_set_dpsec_integration_step( const double new_step_size);

//////////////////////
// larry :: other

 #endif
