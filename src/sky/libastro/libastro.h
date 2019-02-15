
#ifndef _LIBASTRO_GLOBAL_H
#define _LIBASTRO_GLOBAL_H

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

//  wx
//#include "wx/wxprec.h"

// starting point for MJD calculations - 31/12/1899 12:00
#define MJD0  2415020.0
// yes, 2000 January 1 at 12h - 2451544.5
#define MJ2000 (2451545.0 - MJD0)     

// special epoch flag: use epoch of date 
#define	EOD	(-9786)

// AU IN KM
#define AU_KM			149604970
#define AU_M			149604970000

#define	ERAD	(6.37816e6)	/* earth equitorial radius, m */
#define	MAU	(1.4959787e11)	/* m / au */

/* conversions among hours (of ra), degrees and radians. */
#define	degrad(x)	((x)*PI/180.)
#define	raddeg(x)	((x)*180./PI)
#define	hrdeg(x)	((x)*15.)
#define	deghr(x)	((x)/15.)
#define	hrrad(x)	degrad(hrdeg(x))
#define	radhr(x)	deghr(raddeg(x))

// info about the local observing circumstances and misc preferences 
typedef struct 
{
	// modified Julian date, ie, days since
	// Jan 0.5 1900 (== 12 noon, Dec 30, 1899), utc.
	// enough precision to get well better than 1 second.
	// N.B. if not first member, must move NOMJD inits.
	double n_mjd;	

	double n_lat;		// geographic (surface-normal) lt, >0 north, rads 
	double n_lng;		// longitude, >0 east, rads 
	double n_tz;		// time zone, hrs behind UTC 
	double n_temp;		// atmospheric temp, degrees C 
	double n_pressure;	// atmospheric pressure, mBar 
	double n_elev;		// elevation above sea level, earth radii 
	double n_dip;		// dip of sun below hzn at twilight, >0 below, rads 
	double n_epoch;		// desired precession display ep as an mjd, or EOD 
	char n_tznm[8];		// time zone name; 7 chars or less, always 0 at end 

} Now;

////////////////////////////////////////////////////////////////////
// manifest names for planets.
// N.B. must coincide with usage in pelement.c and plans.c.
// N.B. only the first 8 are valid for use with plans().
typedef enum 
{
    MERCURY,
    VENUS,
    MARS,
    JUPITER,
    SATURN,
    URANUS,
    NEPTUNE,
    PLUTO,
    SUN,
    MOON,
    NOBJ		// total number of basic objects
} PLCode;

// moon constants for pl_moon 
typedef enum 
{
	// use to mean planet itself 
    X_PLANET = 0,			
    PHOBOS = NOBJ, DEIMOS,
    IO, EUROPA, GANYMEDE, CALLISTO,
    MIMAS, ENCELADUS, TETHYS, DIONE, RHEA, TITAN, HYPERION, IAPETUS,
    ARIEL, UMBRIEL, TITANIA, OBERON, MIRANDA,
    NBUILTIN

} MCode;

/////////////////////////////////////////////////
// structures to describe objects of various types.


// magnitude values in two different systems 
typedef struct
{
	// yes, 2000 January 1 at 12h 
	float m1, m2;
	// one of MAG_gk or MAG_HG 
	int whichm;		

} Mag;

// o_type code.
// N.B. names are assigned in order in objmenu.c
// N.B. if add one add switch in obj_cir().
// N.B. UNDEFOBJ must be zero so new objects are undefinied by being zeroed.
// N.B. maintain the bitmasks too.
enum ObjType
{
    UNDEFOBJ=0,
    FIXEDOBJ, BINARYSTAR, ELLIPTICAL, HYPERBOLIC, PARABOLIC, EARTHSAT, PLANET,
    NOBJTYPES
};

////////////////////////////////////////////////////////////////////
// Obj is a massive union.
// many fields are in common so we use macros to make things a little easier.
////////////////////////////////////////////////////////////////////

#define	MAXNM	21

typedef unsigned char ObjType_t;
typedef unsigned char ObjAge_t;
typedef unsigned char byte;

// fields common to *all* structs in the Obj union 
#define	OBJ_COMMON_FLDS							\
    ObjType_t co_type;		/* current object type; see flags, below */	\
    byte co_flags;			/* FUSER*... used by others */			\
    ObjAge_t co_age;		/* update aging code; see db.c */		\
    char co_name[MAXNM];	/* name, including \0 */			\
    float co_ra;			/* geo/topo app/mean ra, rads */		\
    float co_dec;			/* geo/topo app/mean dec, rads */		\
    float co_gaera;			/* geo apparent ra, rads */			\
    float co_gaedec;		/* geo apparent dec, rads */			\
    float co_az;			/* azimuth, >0 e of n, rads */			\
    float co_alt;			/* altitude above topocentric horizon, rads */	\
    float co_elong;			/* angular sep btwen obj and sun, >0 E, degs */	\
    float co_size;			/* angular size, arc secs */			\
    short co_mag			/* visual magnitude * MAGSCALE */

// fields common to all solar system objects in the Obj union 
#define	OBJ_SOLSYS_FLDS							\
    OBJ_COMMON_FLDS;	/* all the fixed ones plus ... */		\
    float so_sdist;		/* dist from object to sun, au */		\
    float so_edist;		/* dist from object to earth, au */		\
    float so_hlong;		/* heliocentric longitude, rads */		\
    float so_hlat;		/* heliocentric latitude, rads */		\
    float so_phase		/* phase, % */

// fields common to all fixed objects in the Obj union 
#define	OBJ_FIXED_FLDS 							\
    char  fo_spect[2];	/* spectral codes, if appropriate */		\
    float fo_epoch;		/* eq of ra/dec and time when pm=0; mjd */ 	\
    float fo_ra;		/* ra, rads, in epoch frame */ 			\
    float fo_dec;		/* dec, rads, in epoch frame */ 		\
    float fo_pmra;		/* ra proper motion, rads/day/cos(dec) */ 	\
    float fo_pmdec;		/* dec proper motion, rads/day */ 		\
    char  fo_class		/* object class */

// a generic object 
typedef struct
{
    OBJ_COMMON_FLDS;

} ObjAny;

// a generic sol system object 
typedef struct 
{
    OBJ_SOLSYS_FLDS;

} ObjSS;

// basic Fixed object info.
typedef struct 
{
    OBJ_COMMON_FLDS;
    OBJ_FIXED_FLDS;

    // following are for galaxies 
    byte  fo_ratio;		// minor/major diameter ratio. use s/get_ratio() 
    byte  fo_pa;		// position angle, E of N, rads. use s/get_pa() 

} ObjF;

// true-orbit parameters of binary-star object type
typedef struct 
{
    float bo_T;		/* epoch of periastron, years */
    float bo_e;		/* eccentricity */
    float bo_o;		/* argument of periastron, degress */
    float bo_O;		/* longitude of node, degrees */
    float bo_i;		/* inclination to plane of sky, degrees */
    float bo_a;		/* semi major axis, arc secs */
    float bo_P;		/* period, years */

    // companion position, computed by obj_cir() iff b_2compute 
    float bo_pa;	/* position angle @ ep, rads E of N */
    float bo_sep;	/* separation @ ep, arc secs */
    float bo_ra;	/* geo/topo app/mean ra, rads */
    float bo_dec;	/* geo/topo app/mean dec, rads */

} BinOrbit;

typedef struct
{
    float bp_ep;	// epoch of pa/sep, year
    float bp_pa;	// position angle @ ep, rads E of N
    float bp_sep;	// separation @ ep, arc secs

    // companion position, computed by obj_cir() iff b_2compute 
    float bp_ra;	// geo/topo app/mean ra, rads 
    float bp_dec;	// geo/topo app/mean dec, rads 

} BinPos;

// max discrete epochs to store when no elements 
#define MAXBINPOS 2	

typedef struct
{
    OBJ_COMMON_FLDS;
    OBJ_FIXED_FLDS;

    byte b_2compute;	// whether to compute secondary positions 
    byte b_nbp;			// number of b_bp[] or 0 to use b_bo 
    short b_2mag;		// secondary's magnitude * MAGSCALE 
    char b_2spect[2];	// secondary's spectrum 

    // either a real orbit or a set of discrete pa/sep 
    union 
	{
		BinOrbit b_bo;				// orbital elements 
		BinPos b_bp[MAXBINPOS];		// table of discrete positions 

	} u;

} ObjB;

#define	fo_mag	co_mag	/* pseudonym for so_mag since it is not computed */
#define	fo_size	co_size	/* pseudonym for so_size since it is not computed */

/* macros to pack/unpack some fields */
#define	SRSCALE		255.0		/* galaxy size ratio scale */
#define	PASCALE		(255.0/(2*PI))	/* pos angle scale factor */
#define	get_ratio(op)	(((int)(op)->f_ratio)/SRSCALE)
#define	set_ratio(op,maj,min) ((op)->f_ratio = (byte)(((maj) > 0)	    \
					? ((min)*SRSCALE/(double)(maj)+0.5) \
					: 0))
#define	get_pa(op)	((double)(op)->f_pa/PASCALE)
#define	set_pa(op,s)	((op)->f_pa = (byte)((s)*PASCALE + 0.5))

// n potential fo_classes -- allow for all ASCII 
#define	NCLASSES	128 

///////////////////////////////
// basic planet object info 
typedef struct 
{
    OBJ_SOLSYS_FLDS;

    PLCode plo_code;				// which planet
    MCode plo_moon;					// which moon, or X_PLANET if planet
    char plo_evis, plo_svis;		// if moon: whether visible from earth, sun
    double plo_x, plo_y, plo_z;		// if moon: eq dist from center, planet radii
    double plo_aux1, plo_aux2;		// various values, depending on type

} ObjPl;

/////////////////////
// basic info about an object in elliptical heliocentric orbit
typedef struct 
{
    OBJ_SOLSYS_FLDS;

    float  eo_inc;		// inclination, degrees 
    float  eo_Om;		// longitude of ascending node, degrees 
    float  eo_om;		// argument of perihelion, degress 
    float  eo_a;		// mean distance, aka,semi-maj axis,AU 
    float  eo_M;		// mean anomaly, ie, degrees from perihelion at cepoch
    float  eo_size;		// angular size, in arc seconds at 1 AU 
    float  eo_startok;	// nominal first mjd this set is ok, else 0 
    float  eo_endok;	// nominal last mjd this set is ok, else 0 
    double eo_e;		// eccentricity (double for when near 1 computing q) 
    double eo_cepoch;	// epoch date (M reference), as an mjd 
    double eo_epoch;	// equinox year (inc/Om/om reference), as an mjd. 
    Mag    eo_mag;		// magnitude 

} ObjE;

///////////////////
// basic info about an object in hyperbolic heliocentric orbit 
typedef struct 
{
    OBJ_SOLSYS_FLDS;

    double ho_epoch;	// equinox year (inc/Om/om reference), as an mjd 
    double ho_ep;		// epoch of perihelion, as an mjd 
    float  ho_startok;	// nominal first mjd this set is ok, else 0 
    float  ho_endok;	// nominal last mjd this set is ok, else 0 
    float  ho_inc;		// inclination, degs 
    float  ho_Om;		// longitude of ascending node, degs 
    float  ho_om;		// argument of perihelion, degs. 
    float  ho_e;		// eccentricity 
    float  ho_qp;		// perihelion distance, AU 
    float  ho_g, ho_k;	// magnitude model coefficients 
    float  ho_size;		// angular size, in arc seconds at 1 AU 

} ObjH;

//////////////////
// basic info about an object in parabolic heliocentric orbit 
typedef struct
{
    OBJ_SOLSYS_FLDS;

    double po_epoch;	// reference epoch, as an mjd 
    double po_ep;		// epoch of perihelion, as an mjd 
    float  po_startok;	// nominal first mjd this set is ok, else 0 
    float  po_endok;	// nominal last mjd this set is ok, else 0
    float  po_inc;		// inclination, degs 
    float  po_qp;		// perihelion distance, AU 
    float  po_om;		// argument of perihelion, degs. 
    float  po_Om;		// longitude of ascending node, degs 
    float  po_g, po_k;	// magnitude model coefficients 
    float  po_size;		// angular size, in arc seconds at 1 AU 

} ObjP;

/////////////////////
// basic earth satellite object info
typedef struct
{
    OBJ_COMMON_FLDS;

	// reference epoch, as an mjd 
    double eso_epoch;	
	// mean motion, rev/day N.B. we need double due to a sensitive differencing
	// operation used to compute MeanAnomaly in esat_main()/satellite.c.
    double eso_n;	

    float  eso_startok;		// nominal first mjd this set is ok, else 0
    float  eso_endok;		// nominal last mjd this set is ok, else 0 
    float  eso_inc;			// inclination, degs 
    float  eso_raan;		// RA of ascending node, degs 
    float  eso_e;			// eccentricity 
    float  eso_ap;			// argument of perigee at epoch, degs 
    float  eso_M;			// mean anomaly, ie, degrees from perigee at epoch 
    float  eso_decay;		// orbit decay rate, rev/day^2 
    float  eso_drag;		// object drag coefficient, (earth radii)^-1 
    int    eso_orbit;		// integer orbit number of epoch 

    // computed "sky" results unique to earth satellites 
    float  ess_elev;		// height of satellite above sea level, m 
    float  ess_range;		// line-of-site distance from observer to satellite, m
    float  ess_rangev;		// rate-of-change of range, m/s 
    float  ess_sublat;		// latitude below satellite, >0 north, rads 
    float  ess_sublng;		// longitude below satellite, >0 east, rads 
    int    ess_eclipsed;	// 1 if satellite is in earth's shadow, else 0 

} ObjES;

typedef union
{
    ObjAny  any;	// these fields valid for all types 
    ObjSS   anyss;	// these fields valid for all solar system types 
    ObjPl   pl;		// planet 
    ObjF    f;		// fixed object, plus proper motion 
    ObjB    b;		// bona fide binary stars (doubles are stored in f) 
    ObjE    e;		// object in heliocentric elliptical orbit 
    ObjH    h;		// object in heliocentric hyperbolic trajectory 
    ObjP    p;		// object in heliocentric parabolic trajectory 
    ObjES   es;		// earth satellite 

} Obj;

////////////////////////////////////////////////////////////////////
// METHODS USED EXTERNALY 
////////////////////////////////////////////////////////////////////
// :: circum.cpp
extern int Sol_CalcObjOrbitPos( Now *np, Obj *op );

// :: mjd.cpp
extern void cal_mjd( int mn, double dy, int yr, double *m );

// :: dbfmt.cpp
extern int tle_sum( char *l );
extern double tle_fld( char *l, int from, int thru );
extern double tle_expfld( char *l, int start );

extern int wx_tle_sum( wxChar *l );
extern double wx_tle_fld( wxChar *l, int from, int thru );
extern double wx_tle_expfld( wxChar *l, int start );

// :: nutation.cpp
extern void nutation (double m, double *deps, double *dpsi);
extern void nut_eq (double m, double *ra, double *dec);

// :: precess.cpp
void precess( double mjd1, double mjd2, double *ra, double *dec );

void radecrange( double *ra, double *dec );

double deltat( double mj );

void eq_ecl( double mj, double ra, double dec, double *lt, double *lg );
void sunpos( double mj, double *lsn, double *rsn, double *bsn );
void ab_eq( double mj, double lsn, double *ra, double *dec );

void now_lst( Now *np, double *lstp );

void ta_par( double tha, double tdec, double phi, double ht, double *rho, double *aha, double *adec );
void hadec_aa( double lt, double ha, double dec, double *alt, double *az );
void refract( double pr, double tr, double ta, double *aa );

#endif
