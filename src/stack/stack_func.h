////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _STACK_FUNC_H_
#define _STACK_FUNC_H_

// defines
#define STACK_MAX_NO_OF_FRAMES		10000
// stack method defines
#define IMAGE_STACK_METHOD_AVERAGE		0
#define IMAGE_STACK_METHOD_MEDIAN		1
#define IMAGE_STACK_METHOD_MEAN_MEDIAN	2
#define IMAGE_STACK_METHOD_KAPPA_SIGMA	3
// other defaults 
#define DEFAULT_STACK_CCDRED_SIGMAS		1.5
#define DEFAULT_STACK_CCDRED_ITER		4
// images combination methods
#define IMAGES_COMBINE_METHOD_SUBSTRACT	0
#define IMAGES_COMBINE_METHOD_ADD		1
#define IMAGES_COMBINE_METHOD_FLAT		2
// max gain accepted when flatfielding
#define IMAGES_COMBINE_MAX_FLAT_GAIN	1.5

// :: stack functions
extern float CalcPixelAverage( unsigned char *vectPixel, int n, float sigmas, int iter );
extern float CalcFloatPixelAverage( float *dp[], int n, float sigmas, int iter );
extern float CalcPixelMedian( unsigned char *vectPixel, int n, float sigmas, int iter );
extern float CalcFloatPixelMedian( float *dp[], int n, float sigmas, int iter );
extern float CalcPixelMeanMedian( unsigned char *vectPixel, int n, float sigmas, int iter );
extern float CalcFloatPixelMeanMedian( float *dp[], int n, float sigmas, int iter );
extern float CalcPixelKappaSigma( unsigned char *vectPixel, int n, float sigmas, int iter );
extern float CalcFloatPixelKappaSigma( float *dp[], int n, float sigmas, int iter );
// :: median functions
extern double CalcDoubleMedian( double a[], int n );
extern float CalcFloatMedian( float a[], int n );

#endif

