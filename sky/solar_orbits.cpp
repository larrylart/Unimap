
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// local headers
#include "../util/func.h"

// main header
#include "solar_orbits.h"

////////////////////////////////////////////////////////////////////
int calc_asteroid_posn( double ut, ELEMENTS *class_elem, double *ra_dec,
                                double *latlon )
{
	double td = ut + td_minus_ut( ut ) / 86400.;
	double earth_loc[6], target_loc[4], dist = 0., x, y, z;
	const double sin_obliq_2000 = .397777156;
	const double cos_obliq_2000 = .917482062;
	int i, j;

	get_earth_loc( (td - 2451545.0) / 365250., earth_loc);
	double r3 = -100.;

	//for( i = 0; i < 2; i++)
	while( fabs( dist - r3) > .01 )
	{
		r3 = dist;

		if( class_elem )
			comet_posn( class_elem, td - dist / AU_PER_DAY, target_loc);
		else
			for( j = 0; j < 4; j++)
				target_loc[j] = 0.;

		dist = 0.;

		for( j = 0; j < 3; j++)
		{
			target_loc[j] -= earth_loc[j];
			dist += target_loc[j] * target_loc[j];
		}
		dist = sqrt( dist);
	}

	x = target_loc[0];
	y = target_loc[1] * cos_obliq_2000 - target_loc[2] * sin_obliq_2000;
	z = target_loc[1] * sin_obliq_2000 + target_loc[2] * cos_obliq_2000;

	if( latlon && latlon[1] < 2. )
	{
		double hr_ang = green_sidereal_time( ut ) + latlon[0];
		double u, x0, y0;

		u = atan( EARTH_AXIS_RATIO * sin( latlon[1]) / cos(latlon[1]));
		y0 = EARTH_MINOR_AXIS_IN_AU * sin( u );
		x0 = EARTH_MAJOR_AXIS_IN_AU * cos( u );

		x -= cos( hr_ang) * x0;
		y -= sin( hr_ang) * x0;
		z -=                y0;
		dist = sqrt( x * x + y * y + z * z );
	}

	ra_dec[0] = atan2( y, x ) * RAD2DEG;
	if( ra_dec[0] < 0.) ra_dec[0] += 360.;
	ra_dec[1] = asin( z / dist ) * RAD2DEG;

	return( 0 );
}

