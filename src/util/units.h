////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _UTIL_UNITS_H
#define _UTIL_UNITS_H

// define constants for conversion
#define UNITD_KM_TO_MILE		0.621371192 
// define astronomical conversion constants
#define UNITD_AU_TO_KM		149597870.691
#define UNITD_LY_TO_AU		63241.077
#define UNITD_PA_TO_AU		206264.806
#define UNITD_LD_TO_KM		384401.0
#define UNITD_ER_TO_KM		6378.1370
// masses
#define UNITM_EM_TO_KG		5.9742E+24
#define UNITM_SM_TO_KG		332950*UNITM_EM_TO_KG
#define UNITM_JM_TO_KG		318*UNITM_EM_TO_KG
#define UNITM_LB_TO_KG		0.4535924
#define UNITM_OZ_TO_KG		0.02834952
#define UNITM_ST_TO_KG		6.350293
// luminosity
#define UNITL_SL_TO_W		3.839E+26

//////////////
// DISTANCE
static double Convert_MilesToKm( double src ){ return( src/UNITD_KM_TO_MILE ); }
static double Convert_KmToMiles( double src ){ return( src*UNITD_KM_TO_MILE ); }

/* --- comment out for now ... better use defines directly ---
// astronomical
static double Convert_AuToKm( double src ){ return( src*UNITD_AU_TO_KM ); }
static double Convert_LyToAu( double src ){ return( src*UNITD_LY_TO_AU ); }
static double Convert_PaToAu( double src ){ return( src*UNITD_PA_TO_AU ); }
static double Convert_LdToKm( double src ){ return( src*UNITD_LD_TO_KM ); }
static double Convert_ErToKm( double src ){ return( src*UNITD_ER_TO_KM ); }
static double Convert_KmToEr( double src ){ return( src/UNITD_ER_TO_KM ); }

/////////////////
// MASS
static double Convert_EmToKg( double src ){ return( src*UNITM_EM_TO_KG ); }
static double Convert_SmToKg( double src ){ return( src*UNITM_SM_TO_KG ); }
static double Convert_JmToKg( double src ){ return( src*UNITM_JM_TO_KG ); }
static double Convert_LbToKg( double src ){ return( src*UNITM_LB_TO_KG ); }
static double Convert_OzToKg( double src ){ return( src*UNITM_OZ_TO_KG ); }
static double Convert_StToKg( double src ){ return( src*UNITM_ST_TO_KG ); }

///////////////
// LUMINOSITY
static double Convert_SlToW( double src ){ return( src*UNITL_SL_TO_W ); }
*/

#endif
