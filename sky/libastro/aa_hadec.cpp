/* function to convert between alt/az and ha/dec.
 */

#include <stdio.h>
#include <math.h>
#include <stddef.h>

#include "../../util/func.h"
#include "astro.h"
#include "misc.h"
#include "probfunc.h"

/* the actual formula is the same for both transformation directions so
 * do it here once for each way.
 * N.B. all arguments are in radians.
 */
void aaha_aux( double lt, double x, double y, double* p, double* q )
{
	static double last_lt = -3434;
	double slt=0.0, clt=0.0;
	double cap=0.0, B=0.0;

	if( lt != last_lt ) 
	{
	    slt = sin(lt);
	    clt = cos(lt);
	    last_lt = lt;
	}

	SolveSphere( -x, PI/2.0-y, slt, clt, &cap, &B );

	*p = B;
	*q = PI/2 - acos(cap);
}

//void aaha_aux( double lt, double x, double y, double *p, double *q );
// void solve_sphere( double A, double b, double cc, double sc, double *cap, double *Bp );

/* given geographical latitude (n+, radians), lt, altitude (up+, radians),
 * alt, and azimuth (angle round to the east from north+, radians),
 * return hour angle (radians), ha, and declination (radians), dec.
 */
void aa_hadec( double lt, double alt, double az, double *ha, double *dec )
{
	aaha_aux( lt, az, alt, ha, dec );
	if( *ha > PI ) *ha -= 2*PI;
}

/* given geographical (n+, radians), lt, hour angle (radians), ha, and
 * declination (radians), dec, return altitude (up+, radians), alt, and
 * azimuth (angle round to the east from north+, radians),
 */
void hadec_aa( double lt, double ha, double dec, double *alt, double *az )
{
	aaha_aux (lt, ha, dec, az, alt);
}

#ifdef NEED_GEOC
/* given a geographic (surface-normal) latitude, phi, return the geocentric
 * latitude, psi.
 */
double
geoc_lat (
double phi)
{
#define	MAXLAT	degrad(89.9999)	/* avoid tan() greater than this */
	return (fabs(phi)>MAXLAT ? phi : atan(tan(phi)/1.00674));
}
#endif


