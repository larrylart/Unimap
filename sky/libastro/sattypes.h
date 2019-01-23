
#ifndef _LIBASTRO_SATTYPES_H
#define _LIBASTRO_SATTYPES_H

/* $Id: sattypes.h,v 1.1 2000/09/25 17:21:25 ecdowney Exp $ */

typedef struct _Vec3 {
    double x, y, z;
} Vec3;


typedef struct _LookAngle {
    double az;
    double el;
    double r;
} LookAngle;


typedef struct _Geoloc {
    double lt;
    double ln;
    double h;
} GeoLoc;

#endif 

