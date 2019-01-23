////////////////////////////////////////////////////////////////////
// Package:		stack functions, median functions
////////////////////////////////////////////////////////////////////

// system includes
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

// local includes
#include "../util/func.h"

// main header
#include "stack_func.h"

// PACKAGE :: stack comp functions
////////////////////////////////////////////////////////////////////
// Function:	CalcPixelAverage
// Purpose:		calculate pixel straight average
// Input:		pixel vector and size, sigma and number of iterations
// Output:		result pixel value
////////////////////////////////////////////////////////////////////
inline float CalcPixelAverage( unsigned char *vectPixel, int n, float sigmas, int iter )
{
	int i;
	float m=0.0;

	for (i=0; i<n; i++)
	{
		m += (float) vectPixel[i];
	}

	return( (float) m/n );
}

////////////////////////////////////////////////////////////////////
// Function:	CalcFloatPixelAverage
// Purpose:		calculate pixel straight average
// Input:		pixel vector and size, sigma and number of iterations
// Output:		result pixel value
////////////////////////////////////////////////////////////////////
static inline float CalcFloatPixelAverage( float *dp[], int n, float sigmas, int iter )
{
	int i;
	float m=0.0;

	for (i=0; i<n; i++)
	{
		m += *dp[i];
	}

	return( (float) m/n );
}

////////////////////////////////////////////////////////////////////
// Function:	CalcPixelMedian
// Purpose:		compute straight median
// Input:		pixel vector and size, sigma and number of iterations
// Output:		result pixel value
////////////////////////////////////////////////////////////////////
inline float CalcPixelMedian( unsigned char *vectPixel, int n, float sigmas, int iter )
{
	int i;
	float pix[STACK_MAX_NO_OF_FRAMES];

	// copy pixels to the pix array
	for (i=0; i<n; i++)
	{
		pix[i] = vectPixel[i];
	}

	return( (float) CalcFloatMedian( pix, n ) );
}

////////////////////////////////////////////////////////////////////
// Function:	CalcFloatPixelMedian
// Purpose:		compute straight median
// Input:		pixel vector and size, sigma and number of iterations
// Output:		result pixel value
////////////////////////////////////////////////////////////////////
static inline float CalcFloatPixelMedian( float *dp[], int n, float sigmas, int iter )
{
	int i;
	float pix[STACK_MAX_NO_OF_FRAMES];

	// copy pixels to the pix array
	for (i=0; i<n; i++)
	{
		pix[i] = *dp[i];
	}

	return( (float) CalcFloatMedian( pix, n ) );
}

////////////////////////////////////////////////////////////////////
// Function:	CalcPixelMeanMedian
// Purpose:		compute mean-median; we average pixels within sigmas
//				of the median
// Input:		pixel vector and size, sigma and number of iterations
// Output:		result pixel value
////////////////////////////////////////////////////////////////////
inline float CalcPixelMeanMedian( unsigned char *vectPixel, int n, float sigmas, int iter )
{
	int i, k = 0;
	float pix[STACK_MAX_NO_OF_FRAMES];
	float sum = 0.0, sumsq = 0.0;
	float m, s;

	sigmas = sqr( sigmas );

	// copy pixels to the pix array
	for( i=0; i<n; i++ )
	{
		sum += vectPixel[i];
		sumsq += sqr( vectPixel[i] );
		pix[i] = vectPixel[i];
	}

	m = sum / (1.0 * n);
	s = sigmas * (sumsq / (1.0 * n) - sqr(m));
	m = CalcFloatMedian(pix, n);
	sum = 0;

	for( i=0; i<n; i++ )
	{
		if( sqr(vectPixel[i] - m) < s )
		{
			sum += vectPixel[i];
			k++;
		}
	}

	if( k != 0 )
		return( sum / (1.0 * k) );
	else
		return( vectPixel[0] );
}

////////////////////////////////////////////////////////////////////
// Function:	CalcFloatPixelMeanMedian
// Purpose:		compute mean-median; we average pixels within sigmas
//				of the median
// Input:		pixel vector and size, sigma and number of iterations
// Output:		result pixel value
////////////////////////////////////////////////////////////////////
static inline float CalcFloatPixelMeanMedian( float *dp[], int n, float sigmas, int iter )
{
	int i, k = 0;
	float pix[STACK_MAX_NO_OF_FRAMES];
	float sum = 0.0, sumsq = 0.0;
	float m, s;

	sigmas = sqr( sigmas );

	// copy pixels to the pix array
	for( i=0; i<n; i++ )
	{
		sum += *dp[i];
		sumsq += sqr(*dp[i]);
		pix[i] = *dp[i];
	}

	m = sum / (1.0 * n);
	s = sigmas * (sumsq / (1.0 * n) - sqr(m));
	m = CalcFloatMedian(pix, n);
	sum = 0;

	for( i=0; i<n; i++ )
	{
		if( sqr(*dp[i] - m) < s )
		{
			sum += *dp[i];
			k++;
		}
	}

	if( k != 0 )
		return( sum / (1.0 * k) );
	else
		return( *dp[0] );
}

////////////////////////////////////////////////////////////////////
// Function:	CalcPixelKappaSigma
// Purpose:		kappa-sigma. We start with a robust estimator: the
//				median. Then, we elliminate the pixels that are more
//				than sigmas away from the median. For the remaining
//				pixels, we calculate the mean and variance, and
//				iterate (with the mean replacing the median) until no
//				pixels are elliminated or iter is reached.
// Input:		pixel vector and size, sigma and number of iterations
// Output:		result pixel value
////////////////////////////////////////////////////////////////////
inline float CalcPixelKappaSigma( unsigned char *vectPixel, int n, float sigmas, int iter )
{
	int i, k = 0, r;
	float pix[STACK_MAX_NO_OF_FRAMES];
	float sum = 0.0, sumsq = 0.0;
	float m, s;

	sigmas = sqr( sigmas );

	// copy pixels to the pix array
	for( i=0; i<n; i++ )
	{
		sum += vectPixel[i];
		sumsq += sqr(vectPixel[i]);
		pix[i] = vectPixel[i];
	}

	s = sigmas * (sumsq / (1.0 * n) - sqr(sum / (1.0 * n)));
	m = CalcFloatMedian(pix, n);

	do
	{
		iter --;
		sum = 0.0;
		sumsq = 0.0;
		r = k;
		k = 0;
		for( i=0; i<n; i++ )
		{
			if( sqr(vectPixel[i] - m) < s )
			{
				sum += vectPixel[i];
				sumsq += sqr(vectPixel[i]);
				k++;
			}
		}

		if( k == 0 ) break;

		m = sum / (1.0 * (k));
		s = sigmas * (sumsq / (1.0 * (k)) - sqr(m));

	} while( (iter > 0) && (k != r) );

	return( (float) m );
}

////////////////////////////////////////////////////////////////////
// Function:	CalcFloatPixelKappaSigma
// Purpose:		kappa-sigma. We start with a robust estimator: the
//				median. Then, we elliminate the pixels that are more
//				than sigmas away from the median. For the remaining
//				pixels, we calculate the mean and variance, and
//				iterate (with the mean replacing the median) until no
//				pixels are elliminated or iter is reached.
// Input:		pixel vector and size, sigma and number of iterations
// Output:		result pixel value
////////////////////////////////////////////////////////////////////
static inline float CalcFloatPixelKappaSigma( float *dp[], int n, float sigmas, int iter )
{
	int i, k = 0, r;
	float pix[STACK_MAX_NO_OF_FRAMES];
	float sum = 0.0, sumsq = 0.0;
	float m, s;

	sigmas = sqr( sigmas );

	// copy pixels to the pix array
	for( i=0; i<n; i++ )
	{
		sum += *dp[i];
		sumsq += sqr(*dp[i]);
		pix[i] = *dp[i];
	}

	s = sigmas * (sumsq / (1.0 * n) - sqr(sum / (1.0 * n)));
	m = CalcFloatMedian(pix, n);

	do
	{
		iter --;
		sum = 0.0;
		sumsq = 0.0;
		r = k;
		k = 0;
		for( i=0; i<n; i++ )
		{
			if( sqr(*dp[i] - m) < s )
			{
				sum += *dp[i];
				sumsq += sqr(*dp[i]);
				k++;
			}
		}

		if( k == 0 ) break;

		m = sum / (1.0 * (k));
		s = sigmas * (sumsq / (1.0 * (k)) - sqr(m));

	} while( (iter > 0) && (k != r) );

	return( (float) m );
}

// PACKAGE :: MATH FUNCTIONS :: MEDIAN
////////////////////////////////////////////////////////////////////
// Function:	dkth_smallest
// Purpose:		find the kth smallest element in the array
// Input:		array of elements, # of elements in the array, rank k
// Output:		one element
////////////////////////////////////////////////////////////////////
static inline double dkth_smallest(double a[], int n, int k)
{
    int i, j, l, m;
    double t, x ;

    l=0 ;
    m=n-1;

    while( l<m )
    {
        x=a[k] ;
        i=l;
        j=m;

        do
        {
			while( a[i]<x ) i++ ;
			while( x<a[j] ) j-- ;

			if (i<=j)
			{
				t=a[i];
				a[i]=a[j];
				a[j]=t;
                i++;
                j--;
            }

        } while( i<=j );

        if( j<k ) l=i;
        if( k<i ) m=j;
    }

    return( a[k] );
}

////////////////////////////////////////////////////////////////////
// Function:	CalcDoubleMedian
// Purpose:		find median of n double array. N.B. array is
//				rearranged IN PLACE.
// Input:		array values and size?
// Output:		median value
////////////////////////////////////////////////////////////////////
static inline double CalcDoubleMedian( double a[], int n )
{
	return( dkth_smallest( a, n, n/2 ) );
}

////////////////////////////////////////////////////////////////////
// Function:	fkth_smallest
// Purpose:		find float median ?
// Input:		array of elements, # of elements in the array, rank k
// Output:		one element
////////////////////////////////////////////////////////////////////
static inline float fkth_smallest(float a[], int n, int k)
{
    int i,j,l,m ;
    float t, x ;

    l=0;
    m=n-1;

    while( l<m )
    {
        x=a[k] ;
        i=l;
        j=m;

        do
        {
            while (a[i]<x) i++;
            while (x<a[j]) j--;
            if (i<=j)
            {
				t=a[i];
				a[i]=a[j];
				a[j]=t;
                i++;
                j--;
            }

        } while( i<=j );

        if (j<k) l=i;
        if (k<i) m=j;
    }

    return( a[k] );
}


////////////////////////////////////////////////////////////////////
// Function:	CalcFloatMedian
// Purpose:		find median of n double array. N.B. array is
//				rearranged IN PLACE.
// Input:		array values and size?
// Output:		median value
////////////////////////////////////////////////////////////////////
static inline float CalcFloatMedian( float a[], int n )
{
	return( fkth_smallest(a, n, n/2) );
}


