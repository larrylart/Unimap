
#ifndef _LIBASTRO_PROBFUNC_H
#define _LIBASTRO_PROBFUNC_H

#include <math.h>
#include <string.h>

extern void SolveSphere( double nA, double b, double cc, double sc, double *cap, double *Bp );
extern void zero_mem( void *loc, unsigned len );
extern void now_lst( Now *np, double *lstp );
extern void gk_mag( double g, double k, double rp, double rho, double *mp );
extern void hg_mag( double h, double g, double rp, double rho, double rsn, double *mp );
extern double atod( char *buf );

#endif
