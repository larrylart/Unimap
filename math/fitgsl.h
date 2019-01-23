/*
 * File:	fitgsl.h
 * Modified:	2002/3/28
 * Created by:	Leilehua Maly
 *
 * This file provides the interface through which the Levenberg-Marquart
 * curve fitting method is applied to f(x) = b + p * e^[0.5*((x - c)/w)^2].
 *
 * The following steps should be followed to calculate the coefficients for
 * a given dataset:
 *
 *	1) Allocate storage for the data using fitgsl_alloc_data().
 *	2) Fill in the appropriate (x,y) values
 *	3) Execute the fit using fitgsl_lm(), passing it the input data and
 *		an array of 4 float values to hold the results.
 *	4) Use the pre-defined macros B_INDEX, P_INDEX, C_INDEX and W_INDEX to
 *		read the corresponding coefficients out of the results array.
 *
 * Calling programs should link with the following library flags:
 *	-lgsl -lgslcblas -lm
 *
 */

#ifndef __FITGSL_H_INCLUDED
#define __FITGSL_H_INCLUDED

/* Include standard and GSL headers */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>

/* Coefficient info */
#define FITGSL_NUM_COEFF 4
#define FITGSL_B_INDEX   0
#define FITGSL_P_INDEX   1
#define FITGSL_C_INDEX   2
#define FITGSL_W_INDEX   3

/* 2-dimensional coordinate with floating-point values */
typedef struct _fitgsl_point2df
{
	float	x;
	float	y;
}fitgsl_point2df;

/* Data storage structure */
typedef struct _fitgsl_data
{
	int		n;
	fitgsl_point2df	*pt;
}fitgsl_data;

/* Fits the specified data to the function, storing the coefficients in 'results' */
int fitgsl_lm(const fitgsl_data *dat, float *results, int verbose);

/* Allocates a 'data' structure with n points */
fitgsl_data *fitgsl_alloc_data(int n);

/* Free a 'data' structure */
void fitgsl_free_data(fitgsl_data *dat);

/* Retrieve f(x) for a particular x using the given parameter values, where */
/*    f(x) = b + p * e^[-0.5 * ((x-c)/w)^2]                                 */
float fitgsl_fx(float b, float p, float c, float w, float x);


#endif	/* #ifndef __FITGSL_H_INCLUDED */
