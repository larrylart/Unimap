////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _SOLAR_ORBITS_H
#define _SOLAR_ORBITS_H

// local headers
#include "solar/afuncs.h"
#include "solar/date.h"
#include "solar/comets.h"

#define N_GCVS_RECS 31992
//#define PI 3.14159265358979323
#define GAUSS_K .01720209895
#define SOLAR_GM (GAUSS_K * GAUSS_K)
#define EARTH_MAJOR_AXIS 6378140.
#define EARTH_MINOR_AXIS 6356755.
#define EARTH_AXIS_RATIO (EARTH_MINOR_AXIS / EARTH_MAJOR_AXIS)
#define EARTH_MAJOR_AXIS_IN_AU (EARTH_MAJOR_AXIS / (1000. * AU_IN_KM))
#define EARTH_MINOR_AXIS_IN_AU (EARTH_MINOR_AXIS / (1000. * AU_IN_KM))


// elemets structure used for calculations
/*typedef struct 
{
	double perih_time, q, ecc, incl, arg_per, asc_node;
	double epoch,  mean_anomaly;
	// derived quantities
	double lon_per, minor_to_major;
	double perih_vec[3], sideways[3];
	double angular_momentum, major_axis, t0, w0;
	double abs_mag, slope_param;
	int is_asteroid, central_obj;

} ELEMENTS;
*/

// functions
int get_earth_loc( double t_millenia, double *results);
int calc_asteroid_posn( double ut, ELEMENTS *class_elem, double *ra_dec, double *latlon);

#endif
