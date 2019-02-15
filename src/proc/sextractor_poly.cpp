 /*
 				poly.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	A program using Polynomials
*
*	Author:		E.BERTIN (IAP)
*
*	Contents:	Polynomial fitting
*
*	Last modify:	13/12/2002
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#ifdef HAVE_CONFIG_H
#include        "config.h"
#endif

#include	<math.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

//#include	"sextractor_define.h"
//#include	"sextractor_globals.h"
//#include	"sextractor_poly.h"

//#include "fits/fitscat.h"
// include main header
#include "sextractor.h"

////////////////////////////////////////////////////////////////////
// Method:		PolyInit
// Class:		CSextractor
// Purpose:		Allocate and initialize a polynom structure.
// Input:		1D array containing the group for each parameter,
//				number of dimensions (parameters),
//				1D array with the polynomial degree for each group,
//				number of groups.
// Output:		polystruct pointer.
////////////////////////////////////////////////////////////////////
polystruct* CSextractor::PolyInit( int *group, int ndim, int *degree, int ngroup )
{
   polystruct	*poly;
   char		str[MAXCHAR];
   static int	nd[POLY_MAXDIM];
   int		*groupt,
		d,g,n,num,den;

  QCALLOC(poly, polystruct, 1);
  if ((poly->ndim=ndim) > POLY_MAXDIM)
    {
    sprintf(str, "The dimensionality of the polynom (%d) exceeds the maximum\n"
		"allowed one (%d)", ndim, POLY_MAXDIM);
    error(EXIT_FAILURE, "*Error*: ", str);
    }

  if (ndim)
    QMALLOC(poly->group, int, poly->ndim);
    for (groupt=poly->group, d=ndim; d--;)
      *(groupt++) = *(group++)-1;

  poly->ngroup = ngroup;
  if (ngroup)
    {
    group = poly->group;	/* Forget the original *group */

    QMALLOC(poly->degree, int, poly->ngroup);

/*-- Compute the number of context parameters for each group */
    memset(nd, 0, ngroup*sizeof(int));
    for (d=0; d<ndim; d++)
      {
      if ((g=group[d])>ngroup)
        error(EXIT_FAILURE, "*Error*: polynomial GROUP out of range", "");
      nd[g]++;
      }
    }

/* Compute the total number of coefficients */
  poly->ncoeff = 1;
  for (g=0; g<ngroup; g++)
    {
    if ((d=poly->degree[g]=*(degree++))>POLY_MAXDEGREE)
      {
      sprintf(str, "The degree of the polynom (%d) exceeds the maximum\n"
		"allowed one (%d)", poly->degree[g], POLY_MAXDEGREE);
      error(EXIT_FAILURE, "*Error*: ", str);
      }

/*-- There are (n+d)!/(n!d!) coeffs per group, that is Prod_(i<=d) (n+i)/i */
    for (num=den=1, n=nd[g]; d; num*=(n+d), den*=d--);
    poly->ncoeff *= num/den;
    }

	QMALLOC(poly->basis, double, poly->ncoeff);
	QCALLOC(poly->coeff, double, poly->ncoeff);

	return poly;
}

////////////////////////////////////////////////////////////////////
// Method:		PolyEnd
// Class:		CSextractor
// Purpose:		Free a polynom structure and everything it contains.
// Input:		polystruct pointer.
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::PolyEnd( polystruct *poly )
{
	free(poly->coeff);
	free(poly->basis);
	free(poly->degree);
	free(poly->group);
	free(poly);
}

////////////////////////////////////////////////////////////////////
// Method:		PolyFunc
// Class:		CSextractor
// Purpose:		Evaluate a multidimensional polynom. Values of the 
//				basis functions are updated in poly->basis.
// Input:		polystruct pointer, pointer to the 1D array of input
//				vector data.
// Output:		Polynom value.
////////////////////////////////////////////////////////////////////
double CSextractor::PolyFunc( polystruct *poly, double *pos )
{
   static double	xpol[POLY_MAXDIM+1];
   double		*post, *xpolt, *basis, *coeff, val, xval;
   static int		expo[POLY_MAXDIM+1], gexpo[POLY_MAXDIM+1];
   int			*expot, *degree,*degreet, *group,*groupt, *gexpot,
			d,g,t, ndim;

/* Prepare the vectors and counters */
  ndim = poly->ndim;
  basis = poly->basis;
  coeff = poly->coeff;
  group = poly->group;
  degree = poly->degree;
  if (ndim)
    {
    for (xpolt=xpol, expot=expo, post=pos, d=ndim; --d;)
      {
      *(++xpolt) = 1.0;
      *(++expot) = 0;
      }
    for (gexpot=gexpo, degreet=degree, g=poly->ngroup; g--;)
      *(gexpot++) = *(degreet++);
    if (gexpo[*group])
      gexpo[*group]--;
    }

/*
*pos=1.001;*(pos+1)=1.0001;*(pos+2)=1.00001;
*/
/* The constant term is handled separately */
  val = *(coeff++);
  *(basis++) = 1.0;
  *expo = 1;
  *xpol = *pos;

/* Compute the rest of the polynom */
  for (t=poly->ncoeff; --t; )
    {
/*-- xpol[0] contains the current product of the x^n's */
    val += (*(basis++)=*xpol)**(coeff++);
/*-- A complex recursion between terms of the polynom speeds up computations */
    post = pos;
    groupt = group;
    expot = expo;
    xpolt = xpol;
/*
printf("%d%d%d %7.5f %7.5f %7.5f  %d %d\n",
		*expo, *(expo+1), *(expo+2), *xpol,
		*(xpol+1), *(xpol+2) , *gexpo, *(gexpo+1));
*/
    for (d=0; d<ndim; d++, groupt++)
      if (gexpo[*groupt]--)
        {
        ++*(expot++);
        xval = (*(xpolt--) *= *post);
        while (d--)
          *(xpolt--) = xval;
        break;
        }
      else
        {
        gexpo[*groupt] = *expot;
        *(expot++) = 0;
        *(xpolt++) = 1.0;
        post++;
        }
    }

	return( val );
}

////////////////////////////////////////////////////////////////////
// Method:		PolyFit
// Class:		CSextractor
// Purpose:		Least-Square fit of a multidimensional polynom to 
//				weighted data. If different from NULL, extbasis can be 
//				provided to store the values of the basis functions. 
//				If x==NULL and extbasis!=NULL, the precomputed basis 
//				functions stored in extbasis are used (which saves
// Input:		polystruct pointer, pointer to the (pseudo)2D array 
//				of inputs to basis functions,
//				pointer to the 1D array of data values,
//				pointer to the 1D array of data weights,
//				number of data points,
//				pointer to a (pseudo)2D array of computed basis function 
//				values.
// Output:		Chi2 of the fit.
////////////////////////////////////////////////////////////////////
void CSextractor::PolyFit( polystruct *poly, double *x, double *y, 
							double *w, int ndata, double *extbasis )
{
   double	*alpha,*alphat, *beta,*betat, *basis,*basis1,*basis2, *coeff,
		*extbasist,
		val,wval,yval;
   int		ncoeff, ndim, matsize,
		i,j,n;

  if (!x && !extbasis)
    error(EXIT_FAILURE, "*Internal Error*: One of x or extbasis should be "
	"different from NULL\nin ", "poly_func()");
  ncoeff = poly->ncoeff;
  ndim = poly->ndim;
  matsize = ncoeff*ncoeff;
  basis = poly->basis;
  extbasist = extbasis;
  QCALLOC(alpha, double, matsize);
  QCALLOC(beta, double, ncoeff);

/* Build the covariance matrix */
  for (n=ndata; n--;)
    {
    if (x)
      {
/*---- If x!=NULL, compute the basis functions */
      PolyFunc(poly, x);
      x+=ndim;
/*---- If, in addition, extbasis is provided, then fill it */
      if (extbasis)
        for (basis1=basis,j=ncoeff; j--;)
          *(extbasist++) = *(basis1++);
      }
    else
/*---- If x==NULL, then rely on pre-computed basis functions */
      for (basis1=basis,j=ncoeff; j--;)
        *(basis1++) = *(extbasist++);

    basis1 = basis;
    wval = *(w++);
    yval = *(y++);
    betat = beta;
    alphat = alpha;
    for (j=ncoeff; j--;)
      {
      val = *(basis1++)*wval;
      *(betat++) += val*yval;
      for (basis2=basis,i=ncoeff; i--;)
        *(alphat++) += val**(basis2++);
      }
    }

/* Solve the system */
  CholSolve(alpha,beta,ncoeff);

  free(alpha);

/* Now fill the coeff array with the result of the fit */
  betat = beta;
  coeff = poly->coeff;
  for (j=ncoeff; j--;)
    *(coeff++) = *(betat++);

	free(beta);

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		CholSolve
// Class:		CSextractor
// Purpose:		Solve a system of linear equations, using Cholesky 
//				decomposition. Based on Numerical Recipes, 2nd ed. 
//				(Chap 2.9). The matrix of coefficients must be 
//				symmetric and positive definite.
// Input:		Pointer to the (pseudo 2D) matrix of coefficients,
//				pointer to the 1D column vector, matrix size.
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::CholSolve( double *a, double *b, int n )
{
   double	*p, *x, sum;
   int		i,j,k;

/* Allocate memory to store the diagonal elements */
  QMALLOC(p, double, n);

/* Cholesky decomposition */
  for (i=0; i<n; i++)
    for (j=i; j<n; j++)
      {
      for (sum=a[i*n+j],k=i-1; k>=0; k--)
        sum -= a[i*n+k]*a[j*n+k];
      if (i==j)
        {
        if (sum <= 0.0)
          error(EXIT_FAILURE, "*Error*: Non positive definite matrix in ",
				"cholsolve()");
        p[i] = sqrt(sum);
        }
      else
        a[j*n+i] = sum/p[i];
      }

/* Solve the system */
  x = b;		/* Just to save memory:  the solution replaces b */
  for (i=0; i<n; i++)
    {
    for (sum=b[i],k=i-1; k>=0; k--)
      sum -= a[i*n+k]*x[k];
    x[i] = sum/p[i];
    }

  for (i=n-1; i>=0; i--)
    {
    for (sum=x[i],k=i+1; k<n; k++)
      sum -= a[k*n+i]*x[k];
    x[i] = sum/p[i];
    }

	free(p);

	return;
}

