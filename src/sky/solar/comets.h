
#ifndef _SOLAR_COMETS_H
#define _SOLAR_COMETS_H

//#define ELEMENTS struct elements

typedef struct
   {
   double perih_time, q, ecc, incl, arg_per, asc_node;
   double epoch,  mean_anomaly;
            /* derived quantities: */
   double lon_per, minor_to_major;
   double perih_vec[3], sideways[3];
   double angular_momentum, major_axis, t0, w0;
   double abs_mag, slope_param;
   int is_asteroid, central_obj;
   } ELEMENTS;


#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER            /* Microsoft Visual C/C++ lacks a 'stdint.h'; */
#include "stdintvc.h"      /* 'stdintvc.h' is a replacement version      */
#else
#include <stdint.h>
#endif

// void calc_vectors( ELEMENTS *elem, const double sqrt_gm);
int  calc_classical_elements( ELEMENTS *elem, const double *r,
                             const double t, const int ref, const double gm);
int calc_posn_and_vel( double *r, ELEMENTS *elem, double t, double sqrt_gm);
         /* Above is still in use by PERTURB... hope to eliminate it soon */
int  comet_posn_and_vel( ELEMENTS  *elem, double t,
                  double  *loc, double  *vel);
int  comet_posn( ELEMENTS  *elem, double t, double  *loc);       /* astfuncs.c */
void  derive_quantities( ELEMENTS  *e, const double gm);
int  setup_elems_from_ast_file( ELEMENTS  *class_elem,
              const uint32_t  *elem, const double t_epoch);

#ifdef __cplusplus
}
#endif

#endif
