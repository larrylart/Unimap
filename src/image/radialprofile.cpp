////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// local headers
#include "../util/func.h"

// main header
#include "radialprofile.h"

void getObjectRadialProfile( wxImage& rImage, double x, double y, double nA, double nB, double ang_deg, double* vectRing, unsigned int& nRingNo )
{
	if( !rImage.IsOk() ) return;

	// get image details & ref
	unsigned char* srcImg = rImage.GetData();
	int w = rImage.GetWidth();
	int h = rImage.GetHeight();
	unsigned long mpic_size = w*h*3;
	unsigned int red, green, blue, i;

	double t=0, e_half_perimeter, step;
	int eX=0, eY=0;
	unsigned long pix_idx = 0;
	double a, b, nVal;

	// some presets
	double ang_rad = PI*(180.0-ang_deg)/180.0;
	double e_ratio = nB/nA;

	// set my ring numbers as a
	//nRingNo = (unsigned int) round( 2.0*nA );
	// allocate ring
//	vectRing = (double*) malloc( nRingNo*sizeof( double ) );
	unsigned int* vectRingNo = (unsigned int*) malloc( 2*nRingNo*sizeof( unsigned int ) );

	pix_idx = ((int)round(y)*w+(int) round(x))*3;
	red = (unsigned int) srcImg[pix_idx];
	green = (unsigned int) srcImg[pix_idx+1];
	blue = (unsigned int) srcImg[pix_idx+2];
	// calculate total value
	nVal = (double) ( ( 0.299*red + 0.587*green + 0.114*blue ) * 1.0 /*65535*/ );
	// init total avg sums
	double nAvgDensSum = nVal;
	int nAvgDensNo = 1;

	// add to ring - nRingNo-1 = central
	vectRing[nRingNo-1] = nVal;
	vectRingNo[nRingNo-1] = 1;

	a = 0.0;
	double r_div = 0.1;
	unsigned int idxRingA, idxRingB;

	for( i=1; i<nRingNo; i++ )
	{
		idxRingA = nRingNo-1-i;
		idxRingB = nRingNo-1+i;
		// reset
		vectRing[idxRingA] = 0.0;
		vectRingNo[idxRingA] = 0;
		vectRing[idxRingB] = 0.0;
		vectRingNo[idxRingB] = 0;

		// set a/b 
		a += r_div;
		b = a*e_ratio;
		e_half_perimeter = (double) PI*sqrt( (sqr(a) + sqr(b))/2.0 );
		step = (double) PI/(e_half_perimeter + 5);

		// now for every step
		for( t=0; t<2*PI; t+=step )
		{
			eX = (int) round( x + a*cos(t)*cos(ang_rad) - b*sin(t)*sin(ang_rad));
			eY = (int) round( y + b*sin(t)*cos(ang_rad) + a*cos(t)*sin(ang_rad));
			// check for zero exception
			if( eX<0 || eY<0 ) continue;

			// extract pixel
			pix_idx = (eY*w+eX)*3;
			// check for out of boundary
			if( pix_idx > mpic_size-3 ) continue; 

			red = (unsigned int) srcImg[pix_idx];
			green = (unsigned int) srcImg[pix_idx+1];
			blue = (unsigned int) srcImg[pix_idx+2];
			// calculate total value
			nVal = (double) ( ( 0.299*red + 0.587*green + 0.114*blue ) * 1.0 /*65535*/ );
			// add to ring
			if( t < PI )
			{
				vectRing[idxRingA] += nVal;
				vectRingNo[idxRingA]++;

			} else
			{
				vectRing[idxRingB] += nVal;
				vectRingNo[idxRingB]++;
			}
			// add to total sums
			nAvgDensSum += nVal;
			nAvgDensNo++;
		}
		// now at the end of the ring get median value
		vectRing[idxRingA] = (double) (vectRing[idxRingA]/vectRingNo[idxRingA]);
		vectRing[idxRingB] = (double) (vectRing[idxRingB]/vectRingNo[idxRingB]);
	}
	// variation method
/*	for( i=1; i<nRingNo; i++ )
	{
		vectRing[i] = (double) (
	}
*/
	// apply radial distribution formula
/*	double nAvgDens = (double) (nAvgDensSum/nAvgDensNo);
	a = 0.0;
	for( i=0; i<nRingNo; i++ )
	{
		a += r_div;
		vectRing[i] = (double) ( (double) vectRing[i]/( 4*PI*r_div*nAvgDens*sqr(a)) )/5.0;
	}
*/
	free( vectRingNo );

	//////// :: doc - g(r)= np /(ro * 4 * PI * sqr(r) * r_div )
	// np = number of points on ellipse - radius - this should be 
	// ro - mean atom ? density - maybe make a summ of all point / number of points


	return;
}
