
#ifndef _LIBASTRO_MISC_H
#define _LIBASTRO_MISC_H

#include <math.h>
#include <string.h>

#include "astro.h"

int tickmarks (double min, double max, int numdiv, double ticks[]);
int lc (int cx, int cy, int cw, int x1, int y1, int x2, int y2,
    int *sx1, int *sy1, int *sx2, int *sy2);

int magdiam (int fmag, int magstp, double scale, double mag,
    double size);

double delra (double dra);

void radec2ha (Now *np, double ra, double dec, double *hap);
void gha (Now *np, Obj *op, double *ghap);
char *obj_description (Obj *op);
int is_deepsky (Obj *op);



#endif

