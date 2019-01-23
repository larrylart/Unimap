

#include "astro.h"

#include "probfunc.h"

/* solve a spherical triangle:
 *           A
 *          /  \
 *         /    \
 *      c /      \ b
 *       /        \
 *      /          \
 *    B ____________ C
 *           a
 *
 * given A, b, c find B and a in range 0..B..2PI and 0..a..PI, respectively..
 * cap and Bp may be NULL if not interested in either one.
 * N.B. we pass in cos(c) and sin(c) because in many problems one of the sides
 *   remains constant for many values of A and b.
 */
void SolveSphere( double nA, double b, double cc, double sc, double *cap, double *Bp )
{
	double cb = cos(b), sb = sin(b);
	double sA, cA = cos(nA);
	double x, y;
	double ca;
	double nB;

	ca = cb*cc + sb*sc*cA;
	if (ca >  1.0) ca =  1.0;
	if (ca < -1.0) ca = -1.0;
	if (cap)
	    *cap = ca;

	if (!Bp)
	    return;

	if (sc < 1e-7)
	    nB = cc < 0 ? nA : PI-nA;
	else {
	    sA = sin(nA);
	    y = sA*sb*sc;
	    x = cb - ca*cc;
	    nB = y ? (x ? atan2(y,x) : (y>0 ? PI/2 : -PI/2)) : (x>=0 ? 0 : PI);
	}

	*Bp = nB;
	range( Bp, 2*PI );
}

// zero from loc for len bytes 
void zero_mem( void *loc, unsigned len )
{
	(void) memset( loc, 0, len );
}

// Purpose:	given a Now *, find the local apparent sidereal time, 
//			in hours.
////////////////////////////////////////////////////////////////////
void now_lst( Now *np, double *lstp )
{
	static double last_mjd = -23243, last_lng = 121212, last_lst;
	double eps=0.0, lst=0.0, deps=0.0, dpsi=0.0;

	if (last_mjd == mjd && last_lng == lng) {
	    *lstp = last_lst;
	    return;
	}

	utc_gst (mjd_day(mjd), mjd_hr(mjd), &lst);
	lst += radhr(lng);

	obliquity(mjd, &eps);
	nutation(mjd, &deps, &dpsi);
	lst += radhr(dpsi*cos(eps+deps));

	range (&lst, 24.0);

	last_mjd = mjd;
	last_lng = lng;
	*lstp = last_lst = lst;
}

// Purpose:	computer visual magnitude using the g/k parameters 
//			commonly used for comets.
// Input:	sun-obj dist(AU), earth-obj dist(AU)
////////////////////////////////////////////////////////////////////
void gk_mag( double g, double k, double rp, double rho, double *mp )
{
	*mp = g + 5.0*log10(rho) + 2.5*k*log10(rp);
}

// Purpose:	compute visual magnitude using the H/G parameters used 
//			in the Astro Almanac. these are commonly used for asteroids.
// Input:	sun-obj dist(AU), earth-obj dist(AU), sun-earth dist(AU)
////////////////////////////////////////////////////////////////////
void hg_mag( double h, double g, double rp, double rho, double rsn, double *mp )
{
	double psi_t, Psi_1, Psi_2, beta;
	double c;
	double tb2;

	c = (rp*rp + rho*rho - rsn*rsn)/(2*rp*rho);

	if( c <= -1 )
	    beta = PI;
	else if (c >= 1)
	    beta = 0;
	else
	    beta = acos(c);;

	tb2 = tan(beta/2.0);
	// psi_t = exp(log(tan(beta/2.0))*0.63); 
	psi_t = pow( tb2, 0.63 );
	Psi_1 = exp( -3.33*psi_t );
	// psi_t = exp(log(tan(beta/2.0))*1.22);
	psi_t = pow( tb2, 1.22 );
	Psi_2 = exp( -1.87*psi_t );
	*mp = h + 5.0*log10( rp*rho );

	if( Psi_1 || Psi_2 ) *mp -= 2.5*log10((1-g)*Psi_1 + g*Psi_2);
}

// Purpose:	given a string convert to floating point and return it 
//			as a double. this is to isolate possible unportabilities
//			associated with declaring atof(). it's worth it because 
//			atof() is often some 50% faster than sscanf ("%lf");
////////////////////////////////////////////////////////////////////
double atod( char *buf )
{
	return (strtod (buf, NULL));
}
