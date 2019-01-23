/*
 * File:	fitgsl.c
 * Modified:	2002/3/28
 * Created by:	Leilehua Maly
 *
 * Calling programs should link with the following library flags:
 *	-lgsl -lgslcblas -lm
 *
 */
 
#include <string.h>

 #include "fitgsl.h"
 
 /* Function parameter, passed to each of the f(x), f'(x) and fdf(x) */
typedef struct _fitgsl_fnparams
{
	int		n;
	float		*y;
	float		*sigma;
	fitgsl_point2df *pt;
}fitgsl_fnparams;


/* f(x), f'(x) and the GSL invocation function */
static int fx(const gsl_vector *x, void *params, gsl_vector *f);
static int dfx(const gsl_vector *x, void *params, gsl_matrix *J);
static int fdfx(const gsl_vector *x, void *params, gsl_vector *f, gsl_matrix *J);
static int print_state (size_t iter, gsl_multifit_fdfsolver * s);


/* Makes a guess for b, p, c and w based on the supplied data */
static void make_guess(const fitgsl_data *dat, gsl_vector *guess, int verbose);

/* Retrieve f(x) for a particular x using the given parameter values */
float fitgsl_fx(float b, float p, float c, float w, float x)
{
	float t;

	t = ((x - c) / w);
	t *= t;

	return (b + p * exp(-0.5f * t));
}

/* Allocates a 'data' structure with n points */
fitgsl_data *fitgsl_alloc_data(int n)
{
	fitgsl_data *d;
	
	/* Validate the number of points */
	if( n < 0 )
		return NULL;
	
	/* Allocate the structure */
	if( (d = (fitgsl_data*)malloc(sizeof(fitgsl_data))) == NULL )
		return NULL;

	/* Allocate the data point memory */
	if( (d->pt = (fitgsl_point2df*)malloc(n * sizeof(fitgsl_point2df))) == NULL )
	{
		free(d);
		return NULL;
	}	
	
	/* Initialize structure data */
	d->n	= n;
	memset(d->pt, 0, n * sizeof(fitgsl_point2df));
	
	return d;
}

/* Free a 'data' structure */
void fitgsl_free_data(fitgsl_data *dat)
{
	free(dat->pt);
	free(dat);
}

/* Makes a guess for b, p, c and w based on the supplied data */
void make_guess(const fitgsl_data *dat, gsl_vector *guess, int verbose)
{
	int	i;
	float	c, w;
	float	y_max;
	double y_mean;
	
	y_mean = 0;
	y_max	= dat->pt[0].y;
	c = dat->pt[0].x;

	for( i = 0; i < dat->n; i++ )
	{
	   y_mean += dat->pt[i].y;
	   if( y_max <  dat->pt[i].y ) 
	   {
	      y_max = dat->pt[i].y;
	      c = dat->pt[i].x;
	   }
	}
	y_mean /= (float)dat->n;
	w = (dat->pt[dat->n-1].x - dat->pt[0].x) / 10.0;
	
	gsl_vector_set(guess, FITGSL_B_INDEX, y_mean);
	gsl_vector_set(guess, FITGSL_P_INDEX, y_max);
	gsl_vector_set(guess, FITGSL_C_INDEX, c );
	gsl_vector_set(guess, FITGSL_W_INDEX, w );
	
	if( verbose )
	{
		printf("guess: b%g p%g c%g w%g \n", y_mean, y_max, c, w);
	}
}

int fitgsl_lm( const fitgsl_data *dat, float *results, int verbose )
{
	int	i;
	int	status;
	
	fitgsl_fnparams	p;

	/* Store the solver type */
	const gsl_multifit_fdfsolver_type *t = gsl_multifit_fdfsolver_lmder;
	
	/* The solver */
	gsl_multifit_fdfsolver		*solver;
	gsl_multifit_function_fdf	f;
	gsl_vector			*guess;
	
	/* Assign the solver type */
	t	= gsl_multifit_fdfsolver_lmsder;
	
	/* Create a Levenberg-Marquardt solver with n data points and 4 parameters */
	solver = gsl_multifit_fdfsolver_alloc(t, dat->n, 4);
	
	if( solver == NULL )
	{
		return -1;
	}
	
	/* Fill in function info */
	f.f		= fx;
	f.df		= dfx;
	f.fdf		= fdfx;
	f.n		= dat->n;
	f.p		= 4;
	f.params	= &p;
	
	/* Store and allocate parameter info */
	p.n		= dat->n;
	p.y		= (float*)malloc(sizeof(float) * dat->n);
	p.sigma		= (float*)malloc(sizeof(float) * dat->n);
	p.pt		= dat->pt;
	
	/* Copy the data into the function parameter object */
	for( i = 0; i < dat->n; i++ )
	{
		p.y[i]		= dat->pt[i].y;
		p.sigma[i]	= 0.1f;
	}
	
	/* Allocate the guess vector */
	guess = gsl_vector_alloc(4);
	
	/* Make initial guesses based on the data */
	make_guess(dat, guess, verbose);
	
	/* Initialize the solver */
	gsl_multifit_fdfsolver_set(solver, &f, guess);
	
	/* Reset i to count iterations */
	i = 0;
	
	/* Iterate to to find results */
	do
	{
		i++;

		status	= gsl_multifit_fdfsolver_iterate(solver);

		if( verbose )
		{
			print_state( i, solver );
		}

		if( status != 0 )
		{
			break;
		}
		
		status = gsl_multifit_test_delta( solver->dx, solver->x, 1e-4, 1e-4 );

	} while( status == GSL_CONTINUE && i < 500 );
	
	/* Store the results to be returned to the user */
	results[FITGSL_B_INDEX] = gsl_vector_get(solver->x, FITGSL_B_INDEX);
	results[FITGSL_P_INDEX] = gsl_vector_get(solver->x, FITGSL_P_INDEX);
	results[FITGSL_C_INDEX] = gsl_vector_get(solver->x, FITGSL_C_INDEX);
	results[FITGSL_W_INDEX] = gsl_vector_get(solver->x, FITGSL_W_INDEX);
	
	/* Return failure if the status is non-zero */
	if( verbose && (status != 0) )
	{
		printf(" can't fit status = %d \n", status);
	}	
	
	
	/* Free parameter memory */
	free(p.y);
	free(p.sigma);
	
	/* Free GSL memory */
	gsl_multifit_fdfsolver_free(solver);
	gsl_vector_free(guess);

	return (status==0 ? 0 : -1);
}

// Calculates f(x) = b + p * e^[0.5*((x-c)/w)] for each data point
int fx(const gsl_vector *x, void *params, gsl_vector *f)
{
	int	n;
	float	*y;
	float	*sigma;
	
	float	b;
	float	p;
	float	c;
	float	w;
	
	int	i;
	float	t;
	float	yi;
	
	fitgsl_point2df *pt;
	
	/* Store the parameter data */
	n	= ((fitgsl_fnparams*)params)->n;
	y	= ((fitgsl_fnparams*)params)->y;
	sigma	= ((fitgsl_fnparams*)params)->sigma;
	pt	= ((fitgsl_fnparams*)params)->pt;
	
	/* Store the current coefficient values */
	b	= gsl_vector_get(x, FITGSL_B_INDEX);
	p	= gsl_vector_get(x, FITGSL_P_INDEX);
	c	= gsl_vector_get(x, FITGSL_C_INDEX);
	w	= gsl_vector_get(x, FITGSL_W_INDEX);
	
	/* Execute Levenberg-Marquart on f(x) */
	for( i = 0; i < n; i++ )
	{
		t	= (float)pt[i].x;
		yi	= fitgsl_fx(b, p, c, w, t);/*b + p * exp(-0.5f * ((t - c) / w) * ((t - c) / w));*/
		gsl_vector_set(f, i, (yi - y[i]) / sigma[i]);
	}
	
	return GSL_SUCCESS;
}

/* Calculates the Jacobian (derivative) matrix of f(x) = b + p * e^[0.5*((x-c)/w)^2] for each data point */
int dfx(const gsl_vector *x, void *params, gsl_matrix *J)
{
	int		n;
	float		*sigma;
	fitgsl_point2df	*pt;
	
	float	b;
	float	p;
	float	c;
	float	w;
	float	e;
	float	x_minus_c;
	
	int	i;
	
	float	w2;
	float	w3;
	
	/* Store parameter values */
	n	= ((fitgsl_fnparams*)params)->n;
	sigma	= ((fitgsl_fnparams*)params)->sigma;
	pt	= ((fitgsl_fnparams*)params)->pt;
	
	/* Store current coefficients */
	b	= gsl_vector_get(x, FITGSL_B_INDEX);
	p	= gsl_vector_get(x, FITGSL_P_INDEX);
	c	= gsl_vector_get(x, FITGSL_C_INDEX);
	w	= gsl_vector_get(x, FITGSL_W_INDEX);
	
	/* Store non-changing calculations */
	w2	= w * w;
	w3	= w2 * w;
	
	for( i = 0; i < n; i++ )
	{
		x_minus_c	= (pt[i].x - c);
		e		= exp(-0.5f * (x_minus_c / w) * (x_minus_c / w));

		gsl_matrix_set(J, i, FITGSL_B_INDEX, 1 / sigma[i]);
		gsl_matrix_set(J, i, FITGSL_P_INDEX, e / sigma[i]);
		gsl_matrix_set(J, i, FITGSL_C_INDEX, (p * e * x_minus_c) / (sigma[i] * w2));
		gsl_matrix_set(J, i, FITGSL_W_INDEX, (p * e * x_minus_c * x_minus_c) / (sigma[i] * w3));
	}
	
	return GSL_SUCCESS;
}

/* Invokes f(x) and f'(x) */
int fdfx(const gsl_vector *x, void *params, gsl_vector *f, gsl_matrix *J)
{
	fx(x, params, f);
	dfx(x, params, J);
	
	return GSL_SUCCESS;
}

int print_state (size_t iter, gsl_multifit_fdfsolver * s)
{
  printf( "iter: %3u x = % 15.8f % 15.8f % 15.8f %15.8f "
          "|f(x)| = %g\n",
          iter,
          gsl_vector_get( s->x, 0 ),
          gsl_vector_get( s->x, 1 ),
          gsl_vector_get( s->x, 2 ),
          gsl_vector_get( s->x, 3 ),
          gsl_blas_dnrm2( s->f ) );
  
  return 0;
}

