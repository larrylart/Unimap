
#ifndef _LIBASTRO_H
#define _LIBASTRO_H

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

//  wx
#include "wx/wxprec.h"

#ifndef PI
#define	PI		3.141592653589793
#endif

/* conversions among hours (of ra), degrees and radians. */
#define	degrad(x)	((x)*PI/180.)
#define	raddeg(x)	((x)*180./PI)
#define	hrdeg(x)	((x)*15.)
#define	deghr(x)	((x)/15.)
#define	hrrad(x)	degrad(hrdeg(x))
#define	radhr(x)	deghr(raddeg(x))

/* ratio of from synodic (solar) to sidereal (stellar) rate */
#define	SIDRATE		.9972695677



/* the Now and Obj typedefs.
 * also, a few miscellaneous constants and declarations.
 */

#define	SPD	(24.0*3600.0)	/* seconds per day */
#define	MAU	(1.4959787e11)	/* m / au */
#define	LTAU	499.005		/* seconds light takes to travel 1 AU */
#define	ERAD	(6.37816e6)	/* earth equitorial radius, m */
#define	MRAD	(1.740e6)	/* moon equitorial radius, m */
#define	SRAD	(6.95e8)	/* sun equitorial radius, m */
#define	FTPM	3.28084		/* ft per m */
#define	ESAT_MAG	2	/* default satellite magnitude */
#define	FAST_SAT_RPD	0.25	/* max earth sat rev/day considered "fast" */



/* handy shorthands for fields in a Now pointer, np */
#define mjd	np->n_mjd
#define lat	np->n_lat
#define lng	np->n_lng
#define tz	np->n_tz
#define temp	np->n_temp
#define pressure np->n_pressure
#define elev	np->n_elev
#define	dip	np->n_dip
#define epoch	np->n_epoch
#define tznm	np->n_tznm
#define mjed	mm_mjed(np)


/* whichm */
#define MAG_HG          0       /* using 0 makes HG the initial default */
#define MAG_gk          1

/* we actually store magnitudes times this scale factor in a short int */
#define	MAGSCALE	100.0
#define	set_smag(op,m)	((op)->s_mag = (short)floor((m)*MAGSCALE + 0.5))
#define	set_fmag(op,m)	((op)->f_mag = (short)floor((m)*MAGSCALE + 0.5))
#define	get_mag(op)	((op)->s_mag / MAGSCALE)
#define	get_fmag(op)	((op)->f_mag / MAGSCALE)

/* longest object name, including trailing '\0' */
#include "libastro.h"

/* for o_flags -- everybody must agree */
#define	FUSER0		0x01
#define	FUSER1		0x02
#define	FUSER2		0x04
#define	FUSER3		0x08
#define	FUSER4		0x10
#define	FUSER5		0x20
#define	FUSER6		0x40
#define	FUSER7		0x80

/* mark an object as being a "field star" */
#define	FLDSTAR		FUSER3
/* mark an object as circum calculation failed */
#define NOCIRCUM	FUSER7

/* Obj shorthands: */
#define	o_type		any.co_type
#define	o_name		any.co_name
#define	o_flags		any.co_flags
#define	o_age		any.co_age
#define	s_ra		any.co_ra
#define	s_dec		any.co_dec
#define	s_gaera		any.co_gaera
#define	s_gaedec 	any.co_gaedec
#define	s_az		any.co_az
#define	s_alt		any.co_alt
#define	s_elong		any.co_elong
#define	s_size		any.co_size
#define	s_mag		any.co_mag

#define	s_sdist		anyss.so_sdist
#define	s_edist		anyss.so_edist
#define	s_hlong		anyss.so_hlong
#define	s_hlat		anyss.so_hlat
#define	s_phase 	anyss.so_phase

#define	s_elev		es.ess_elev
#define	s_range		es.ess_range
#define	s_rangev	es.ess_rangev
#define	s_sublat	es.ess_sublat
#define	s_sublng	es.ess_sublng
#define	s_eclipsed	es.ess_eclipsed

#define	f_class		f.fo_class
#define	f_spect		f.fo_spect
#define	f_ratio		f.fo_ratio
#define	f_pa		f.fo_pa
#define	f_epoch		f.fo_epoch
#define	f_RA		f.fo_ra
#define	f_pmRA		f.fo_pmra
#define	f_dec		f.fo_dec
#define	f_pmdec		f.fo_pmdec
#define	f_mag		f.fo_mag
#define	f_size		f.fo_size

#define	e_cepoch 	e.eo_cepoch
#define	e_epoch		e.eo_epoch
#define	e_startok	e.eo_startok
#define	e_endok		e.eo_endok
#define	e_inc		e.eo_inc
#define	e_Om		e.eo_Om
#define	e_om		e.eo_om
#define	e_a		e.eo_a
#define	e_e		e.eo_e
#define	e_M		e.eo_M
#define	e_size		e.eo_size
#define	e_mag		e.eo_mag

#define	h_epoch		h.ho_epoch
#define	h_startok	h.ho_startok
#define	h_endok		h.ho_endok
#define	h_ep		h.ho_ep
#define	h_inc		h.ho_inc
#define	h_Om		h.ho_Om
#define	h_om		h.ho_om
#define	h_e		h.ho_e
#define	h_qp		h.ho_qp
#define	h_g		h.ho_g
#define	h_k		h.ho_k
#define	h_size		h.ho_size

#define	p_epoch		p.po_epoch
#define	p_startok	p.po_startok
#define	p_endok		p.po_endok
#define	p_ep		p.po_ep
#define	p_inc		p.po_inc
#define	p_qp		p.po_qp
#define	p_om		p.po_om
#define	p_Om		p.po_Om
#define	p_g		p.po_g
#define	p_k		p.po_k
#define	p_size		p.po_size

#define	es_epoch	es.eso_epoch
#define	es_startok	es.eso_startok
#define	es_endok	es.eso_endok
#define	es_inc		es.eso_inc
#define	es_raan		es.eso_raan
#define	es_e		es.eso_e
#define	es_ap		es.eso_ap
#define	es_M		es.eso_M
#define	es_n		es.eso_n
#define	es_decay	es.eso_decay
#define	es_drag		es.eso_drag
#define	es_orbit	es.eso_orbit

#define	pl_code		pl.plo_code
#define	pl_moon		pl.plo_moon
#define	pl_evis		pl.plo_evis
#define	pl_svis		pl.plo_svis
#define	pl_x		pl.plo_x
#define	pl_y		pl.plo_y
#define	pl_z		pl.plo_z
#define	pl_aux1		pl.plo_aux1
#define	pl_aux2		pl.plo_aux2

#define b_2compute	b.b_2compute
#define b_2spect	b.b_2spect
#define b_2mag		b.b_2mag
#define b_bo		b.u.b_bo
#define b_bp		b.u.b_bp
#define b_nbp		b.b_nbp

/* insure we always refer to the fields and no monkey business */
#undef OBJ_COMMON_FLDS
#undef OBJ_SOLSYS_FLDS



/* types as handy bitmasks too */
#define	OBJTYPE2MASK(t)	(1<<(t))
#define	FIXEDM		OBJTYPE2MASK(FIXED)
#define	BINARYSTARM	OBJTYPE2MASK(BINARYSTAR)
#define	ELLIPTICALM	OBJTYPE2MASK(ELLIPTICAL)
#define	HYPERBOLICM	OBJTYPE2MASK(HYPERBOLIC)
#define	PARABOLICM	OBJTYPE2MASK(PARABOLIC)
#define	EARTHSATM	OBJTYPE2MASK(EARTHSAT)
#define	PLANETM		OBJTYPE2MASK(PLANET)
#define	ALLM		(~0)

/* rise, set and transit information.
 */
typedef struct {
    int rs_flags;	/* info about what has been computed and any
			 * special conditions; see flags, below.
			 */
    double rs_risetm;	/* mjd time of rise today */
    double rs_riseaz;	/* azimuth of rise, rads E of N */
    double rs_trantm;	/* mjd time of transit today */
    double rs_tranalt;	/* altitude of transit, rads up from horizon */
    double rs_settm;	/* mjd time of set today */
    double rs_setaz;	/* azimuth of set, rads E of N */
} RiseSet;

//////////////////////
// RiseSet flags
#define	RS_NORISE	0x0001	/* object does not rise as such today */
#define	RS_NOSET	0x0002	/* object does not set as such today */
#define	RS_NOTRANS	0x0004	/* object does not transit as such today */
#define	RS_CIRCUMPOLAR	0x0010	/* object stays up all day today */
#define	RS_NEVERUP	0x0020	/* object never up at all today */
#define	RS_ERROR	0x1000	/* can't figure out anything! */
#define	RS_RISERR	(0x0100|RS_ERROR) /* error computing rise */
#define	RS_SETERR	(0x0200|RS_ERROR) /* error computing set */
#define	RS_TRANSERR	(0x0400|RS_ERROR) /* error computing transit */

#define	is_type(op,m)	(OBJTYPE2MASK((op)->o_type) & (m))

/* any planet or its moons */
#define	is_planet(op,p)	(is_type(op,PLANETM) && op->pl_code == (p))

/* any solar system object */
#define	is_ssobj(op)	is_type(op,PLANETM|HYPERBOLICM|PARABOLICM|ELLIPTICALM)

/* natural satellite support */

typedef struct {
    char *full;		/* full name */
    char *tag;		/* Roman numeral tag */
    float x, y, z;	/* sky loc in planet radii: +x:east +y:south +z:front */
    float ra, dec;	/* sky location in ra/dec */
    float mag;		/* magnitude */
    int evis;		/* whether geometrically visible from earth */
    int svis;		/* whether in sun light */
    int pshad;		/* whether moon is casting shadow on planet */
    int trans;		/* whether moon is transiting */
    float sx, sy;	/* shadow sky loc in planet radii: +x:east +y:south */
} MoonData;

/* separate set for each planet -- use in pl_moon */


enum _marsmoons {
    M_MARS = 0,					/* == X_PLANET */
    M_PHOBOS, M_DEIMOS,
    M_NMOONS					/* including planet at 0 */
};

enum _jupmoons {
    J_JUPITER = 0,				/* == X_PLANET */
    J_IO, J_EUROPA, J_GANYMEDE, J_CALLISTO,
    J_NMOONS					/* including planet */
};

enum _satmoons {
    S_SATURN = 0,				/* == X_PLANET */
    S_MIMAS, S_ENCELADUS, S_TETHYS, S_DIONE,
    S_RHEA, S_TITAN, S_HYPERION, S_IAPETUS,
    S_NMOONS					/* including planet */
};

enum _uramoons {
    U_URANUS = 0,				/* == X_PLANET */
    U_ARIEL, U_UMBRIEL, U_TITANIA, U_OBERON, U_MIRANDA,
    U_NMOONS					/* including planet */
};

#define	X_MAXNMOONS	S_NMOONS		/* N.B. chosen by hand */

/* global function declarations */

/* aa_hadec.c */
extern void aa_hadec (double lt, double alt, double az, double *ha, double *dec);
extern void hadec_aa (double lt, double ha, double dec, double *alt, double *az); 
extern void aaha_aux( double lt, double x, double y, double *p, double *q );

/* aberration.c */
extern void ab_ecl (double m, double lsn, double *lam, double *bet);
extern void ab_eq (double m, double lsn, double *ra, double *dec);

/* airmass.c */
extern void airmass (double aa, double *Xp);

/* anomaly.c */
extern void anomaly (double ma, double s, double *nu, double *ea);

/* ap_as.c */
extern void ap_as ( Now *np, double Mjd, double *rap, double *decp);
extern void as_ap ( Now *np, double Mjd, double *rap, double *decp);

/* atlas.c */
extern char *um_atlas (double ra, double dec);
extern char *u2k_atlas (double ra, double dec);
extern char *msa_atlas (double ra, double dec);

/* aux.c */
extern double mm_mjed (Now *np);

/* chap95.c */
extern int chap95 (double m, int obj, double prec, double *ret);

/* chap95_data.c */

// :: comet.cpp
extern void GetCometPos( double m, double ep, double inc, double ap, double qp,
							double om, double *lpd, double *psi, double *rp, 
							double *rho, double *lam, double *bet );

/* constel.c */
#define	NCNS	89
extern int cns_pick (double r, double d, double e);
extern int cns_id (char *abbrev);
extern char *cns_name (int id);
extern int cns_edges (double e, double **ra0p, double **dec0p, double **ra1p,
    double **dec1p);
extern int cns_list (double ra, double dec, double e, double rad, int ids[]);
extern int cns_figure (int id, double e, double ra[],double dec[],int dcodes[]);
extern int cns_loadfigs (FILE *fp, char msg[]);

/* dbfmt.c */
extern int db_crack_line (char s[], Obj *op, char nm[][MAXNM], int nnm,
    char whynot[]);
extern void db_write_line (Obj *op, char *lp);
extern int dbline_candidate (char line[]);
extern int get_fields (char *s, int delim, char *fields[]);
extern int db_tle (char *name, char *l1, char *l2, Obj *op);
extern int dateRangeOK (Now *np, Obj *op);

/* deltat.c */
extern double deltat (double m);

/* earthsat.c */
extern int obj_earthsat (Now *np, Obj *op);

/* eq_ecl.c */
extern void eq_ecl (double m, double ra, double dec, double *lt,double *lg);
extern void ecl_eq (double m, double lt, double lg, double *ra,double *dec);

/* eq_gal.c */
extern void eq_gal (double m, double ra, double dec, double *lt,double *lg);
extern void gal_eq (double m, double lt, double lg, double *ra,double *dec);

/* formats.c */
extern int fs_sexa (char *out, double a, int w, int fracbase);
extern int fs_date (char out[], int format, double jd);
extern int f_scansexa (const char *str, double *dp);
extern void f_sscandate (char *bp, int pref, int *m, double *d, int *y);

/* helio.c */
extern void heliocorr (double jd, double ra, double dec, double *hcp);

/* jupmoon.c */
extern void jupiter_data (double Mjd, char dir[], Obj *sop, Obj *jop,
    double *jupsize, double *cmlI, double *cmlII, double *polera,
    double *poledec, MoonData md[J_NMOONS]); 

/* libration.c */
extern void llibration (double JD, double *llatp, double *llonp);

/* magdecl.c */
extern int magdecl (double l, double L, double e, double y, char *dir,
    double *dp, char *err);

/* marsmoon.c */
extern void marsm_data (double Mjd, char dir[], Obj *sop, Obj *mop,
    double *marssize, double *polera, double *poledec, MoonData md[M_NMOONS]); 

/* mjd.c */

extern void mjd_cal (double m, int *mn, double *dy, int *yr);
extern int mjd_dow (double m, int *dow);
extern int isleapyear (int year);
extern void mjd_dpm (double m, int *ndays);
extern void mjd_year (double m, double *yr);
extern void year_mjd (double y, double *m);
extern void rnd_second (double *t);
extern void mjd_dayno (double jd, int *yr, double *dy);
extern double mjd_day (double jd);
extern double mjd_hr (double jd);
extern void range (double *v, double r);
extern void radecrange (double *ra, double *dec);

/* moon.c */
extern void moon (double m, double *lam, double *bet, double *rho,
    double *msp, double *mdp);

/* mooncolong.c */
extern void moon_colong (double jd, double lt, double lg, double *cp,
    double *kp, double *ap, double *sp);

/* moonnf.c */
extern void moonnf (double mj, double *mjn, double *mjf);


/* obliq.c */
extern void obliquity (double m, double *eps);

/* parallax.c */
extern void ta_par (double tha, double tdec, double phi, double ht,
    double *rho, double *aha, double *adec);

/* parallactic.c */
extern double parallacticLDA (double lt, double dec, double alt);
extern double parallacticLHD (double lt, double ha, double dec);

/* plans.c */
extern void plans (double m, PLCode p, double *lpd0, double *psi0,
    double *rp0, double *rho0, double *lam, double *bet, double *dia,
    double *mag);

/* plshadow.c */
extern int plshadow (Obj *op, Obj *sop, double polera,
    double poledec, double x, double y, double z, float *sxp, float *syp);

/* plmoon_cir.c */
extern int plmoon_cir (Now *np, Obj *moonop);
extern int getBuiltInObjs (Obj **opp);
extern void setMoonDir (char *dir);

/* precess.c */
extern void precess (double mjd1, double mjd2, double *ra, double *dec);

// :: reduce.cpp
extern void Sol_ReduceElements( double mjd0, double m, double inc0,
    double ap0, double om0, double *inc, double *ap, double *om );

/* refract.c */
extern void unrefract (double pr, double tr, double aa, double *ta);
extern void refract (double pr, double tr, double ta, double *aa);

/* rings.c */
extern void satrings (double sb, double sl, double sr, double el, double er,
    double JD, double *etiltp, double *stiltp);

/* riset.c */
extern void riset (double ra, double dec, double lt, double dis,
    double *lstr, double *lsts, double *azr, double *azs, int *status);

/* riset_cir.c */
extern void riset_cir (Now *np, Obj *op, double dis, RiseSet *rp);
extern void twilight_cir (Now *np, double dis, double *dawn, double *dusk,
    int *status);

/* satmoon.c */
extern void saturn_data (double Mjd, char dir[], Obj *eop, Obj *sop,
    double *satsize, double *etilt, double *stlit, double *polera, 
    double *poledec, MoonData md[S_NMOONS]); 

/* sphcart.c */
extern void sphcart (double l, double b, double r, double *x, double *y,
    double *z);
extern void cartsph (double x, double y, double z, double *l, double *b,
    double *r);

/* sun.c */
extern void sunpos (double m, double *lsn, double *rsn, double *bsn);

/* twobody.c */
extern int vrc (double *v, double *r, double tp, double e, double q);

/* umoon.c */
extern void uranus_data (double Mjd, char dir[], Obj *sop, Obj *uop,
    double *usize, double *polera, double *poledec, MoonData md[U_NMOONS]); 

/* utc_gst.c */
extern void utc_gst (double m, double utc, double *gst);
extern void gst_utc (double m, double gst, double *utc);

/* vsop87.c */
extern int vsop87 (double m, int obj, double prec, double *ret);

////////////////////////////////////////////////////////////////////
// LARRY PLUG CPP

// :: circum
static int Sol_CalcObjParabolicPos( Now *np, Obj *op );
static int Sol_CalcObjEllipticalPos( Now *np, Obj *op );

static int obj_planet (Now *np, Obj *op);
static int obj_binary (Now *np, Obj *op);
static int obj_2binary (Now *np, Obj *op);
static int obj_fixed (Now *np, Obj *op);
static int obj_hyperbolic (Now *np, Obj *op);
static int sun_cir (Now *np, Obj *op);
static int moon_cir (Now *np, Obj *op);
static double solveKepler (double M, double e);
static void binaryStarOrbit (double t, double T, double e, double o, double O,
    double i, double a, double P, double *thetap, double *rhop);
static void cir_sky (Now *np, double lpd, double psi, double rp, double *rho,
    double lam, double bet, double lsn, double rsn, Obj *op);
static void cir_pos (Now *np, double bet, double lam, double *rho, Obj *op);
static void elongation (double lam, double bet, double lsn, double *el);
static void deflect (double mjd1, double lpd, double psi, double rsn,
    double lsn, double rho, double *ra, double *dec);
static double h_albsize (double H);

// :: plans.c
extern void plans (double mj, PLCode p, double *lpd0, double *psi0, double *rp0,
		double *rho0, double *lam, double *bet, double *dia, double *mag);
// :: sun.c
extern void sunpos (double mj, double *lsn, double *rsn, double *bsn);

#endif 


