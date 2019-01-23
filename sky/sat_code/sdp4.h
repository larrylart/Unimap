
#ifndef _SAT_CODE_SDP4_H
#define _SAT_CODE_SDP4_H

#include <math.h>
#include <stdlib.h>

#include "norad.h"

// from sdp4.cpp
#define c1           params[2]
#define c4           params[3]
#define xnodcf       params[4]
#define t2cof        params[5]
#define deep_arg     ((deep_arg_t *)( params + 10))

extern void SDP4_init( double *params, const tle_t *tle);
extern int SC_SDP4( const double tsince, const tle_t *tle, const double *params, double *pos, double *vel );

#endif

