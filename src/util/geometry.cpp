////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system libs
#include <math.h>

// local headers
#include "func.h"

// main header
#include "geometry.h"

// my globals
float g_vectCtg[40];
float g_vectStg[40];

// init geometry data
void InitGeometryData( )
{
	int i=0;

	// init trigonometric functions values
	for( i=0; i<37; i++ )
	{
		g_vectCtg[i] = (float) cos(i*PI/18);
		g_vectStg[i] = (float) sin(i*PI/18);
	}

}

// check if ellipse is in rectangle
////////////////////////////////////////////////////////////////////
inline bool IsEllipseInRect( double x, double y, double a, double b, double ang, int rx, int ry, int rw, int rh )
{
	int		i;
	double	ct, st;
	double nX=0, nY=0, ac=0.0, bs=0.0;

	double rxe = rx+rw;
	double rye = ry+rh;

	ct = cos(PI*ang/180);
	st = sin(PI*ang/180);

	// now for every step
	for( i=0; i<37; i++ )
	{
		ac = a*g_vectCtg[i];
		bs = b*g_vectStg[i];

		nX = x + ac*ct - bs*st;
		nY = y + ac*st + bs*ct;
		// check if inside
		if( nX < rx || nY < ry || nX > rxe || nY > rye ) return(0);
	}

	return( 1 );
}

// check if ellipse is in rectangle
////////////////////////////////////////////////////////////////////
inline bool GetEllipseImageQuadrant( double x, double y, double a, double b, double ang,
								int width, int height, int wdiv, int hdiv,
								unsigned char& x_a, unsigned char& y_a )
{
	int w_s = (int) (width/wdiv);
	int h_s = (int) (height/hdiv);
	// then find in which division my target is
	x_a = (unsigned char) ((x/w_s)+1);
	y_a = (unsigned char) ((y/h_s)+1);
	// now check if on the division border then set to zero
	if( ( (int) x % w_s == 0 ) || ( (int) y % h_s == 0 ) )
	{
		x_a = 0;
		y_a = 0;
		return(0);

	} else
	{
		// also check if object ellipse border is outside of the area
		int rx = w_s*(x_a-1);
		int ry = h_s*(y_a-1);
		if( !IsEllipseInRect( x, y, a, b, ang, rx, ry, w_s, h_s ) )
		{
			x_a = 0;
			y_a = 0;
			return(0);
		}
	}

	return(1);
}

////////////////////////////////////////////////////////////////////
//  Determines the intersection point of the line defined by points A and B with the
//  line defined by points C and D.
//
//  Returns YES if the intersection point was found, and stores that point in X,Y.
//  Returns NO if there is no determinable intersection point, in which case X,Y will
//  be unmodified.
////////////////////////////////////////////////////////////////////
bool lineIntersection( double Ax, double Ay, double Bx, double By,
						double Cx, double Cy, double Dx, double Dy,
						double *X, double *Y )
{

  double  distAB, theCos, theSin, newX, ABpos ;

  //  Fail if either line is undefined.
  if (Ax==Bx && Ay==By || Cx==Dx && Cy==Dy) return( false );

  //  (1) Translate the system so that point A is on the origin.
  Bx-=Ax; By-=Ay;
  Cx-=Ax; Cy-=Ay;
  Dx-=Ax; Dy-=Ay;

  //  Discover the length of segment A-B.
  distAB=sqrt(Bx*Bx+By*By);

  //  (2) Rotate the system so that point B is on the positive X axis.
  theCos=Bx/distAB;
  theSin=By/distAB;
  newX=Cx*theCos+Cy*theSin;
  Cy  =Cy*theCos-Cx*theSin; Cx=newX;
  newX=Dx*theCos+Dy*theSin;
  Dy  =Dy*theCos-Dx*theSin; Dx=newX;

  //  Fail if the lines are parallel.
  if (Cy==Dy) return( false );

  //  (3) Discover the position of the intersection point along line A-B.
  ABpos=Dx+(Cx-Dx)*Dy/(Dy-Cy);

  //  (4) Apply the discovered position to line A-B in the original coordinate system.
  *X=Ax+ABpos*theCos;
  *Y=Ay+ABpos*theSin;

  //  Success.
  return( true );
}

#define	DONT_INTERSECT    0
#define	DO_INTERSECT      1
#define COLLINEAR         2

/**************************************************************
 *                                                            *
 *    NOTE:  The following macro to determine if two numbers  *
 *    have the same sign, is for 2's complement number        *
 *    representation.  It will need to be modified for other  *
 *    number systems.                                         *
 *                                                            *
 **************************************************************/

#define SAME_SIGNS( a, b )	\
		(((long) ((unsigned long) a ^ (unsigned long) b)) >= 0 )

/* lines_intersect */
int lines_intersect( long x1, long y1,   /* First line segment */
						long x2, long y2,

		     long x3, long y3,   /* Second line segment */
		     long x4, long y4,

		     long* x,
		     long* y         /* Output value:
		                * point of intersection */
               )
{
    long a1, a2, b1, b2, c1, c2; /* Coefficients of line eqns. */
    long r1, r2, r3, r4;         /* 'Sign' values */
    long denom, offset, num;     /* Intermediate values */

    /* Compute a1, b1, c1, where line joining points 1 and 2
     * is "a1 x  +  b1 y  +  c1  =  0".
     */

    a1 = y2 - y1;
    b1 = x1 - x2;
    c1 = x2 * y1 - x1 * y2;

    /* Compute r3 and r4.
     */


    r3 = a1 * x3 + b1 * y3 + c1;
    r4 = a1 * x4 + b1 * y4 + c1;

    /* Check signs of r3 and r4.  If both point 3 and point 4 lie on
     * same side of line 1, the line segments do not intersect.
     */

    if ( r3 != 0 &&
         r4 != 0 &&
         SAME_SIGNS( r3, r4 ))
        return ( DONT_INTERSECT );

    /* Compute a2, b2, c2 */

    a2 = y4 - y3;
    b2 = x3 - x4;
    c2 = x4 * y3 - x3 * y4;

    /* Compute r1 and r2 */

    r1 = a2 * x1 + b2 * y1 + c2;
    r2 = a2 * x2 + b2 * y2 + c2;

    /* Check signs of r1 and r2.  If both point 1 and point 2 lie
     * on same side of second line segment, the line segments do
     * not intersect.
     */

    if ( r1 != 0 &&
         r2 != 0 &&
         SAME_SIGNS( r1, r2 ))
        return ( DONT_INTERSECT );

    /* Line segments intersect: compute intersection point. 
     */

    denom = a1 * b2 - a2 * b1;
    if ( denom == 0 )
        return ( COLLINEAR );
    offset = denom < 0 ? - denom / 2 : denom / 2;

    /* The denom/2 is to get rounding instead of truncating.  It
     * is added or subtracted to the numerator, depending upon the
     * sign of the numerator.
     */

    num = b1 * c2 - b2 * c1;
    *x = ( num < 0 ? num - offset : num + offset ) / denom;

    num = a2 * c1 - a1 * c2;
    *y = ( num < 0 ? num - offset : num + offset ) / denom;

    return ( DO_INTERSECT );
} 


////////////////////////////////////////////////////////////////////
int IsLineIntersectRect( int x1, int y1, int x2, int y2,
								int rx, int ry, int width, int height,
								int& ox1, int& oy1, int& ox2, int& oy2 )
{
	if( x1 >= rx && x1 <= rx+width &&
		x2 >= rx && x2 <= rx+width &&
		y1 >= ry && y1 <= ry+height &&
		y1 >= ry && y1 <= ry+height )
	{
		// line insde rectangle
		return(1);
	}

	// when line point outside check intersection if all the lines
	long nTmpX=0, nTmpY=0;
	double nAX=x1, nAY=y1;
	double nBX=x2, nBY=y2;
	bool bPointA=false;

	// :: TOP LINE
	if( lines_intersect( rx, ry, rx+width, ry, x1, y1, x2, y2, &nTmpX, &nTmpY ) == 1 &&
		nTmpX >= rx && nTmpX <= rx+width && nTmpY >= ry && nTmpY <= ry+height )
	{
		bPointA = true;
		nAX = nTmpX;
		nAY = nTmpY;
	}

	// :: LEFT LINE
	if( lines_intersect( rx, ry, rx, ry+height, x1, y1, x2, y2, &nTmpX, &nTmpY ) == 1  &&
		nTmpX >= rx && nTmpX <= rx+width && nTmpY >= ry && nTmpY <= ry+height )
	{
		if( !bPointA ) 
		{
			bPointA = true;
			nAX = nTmpX;
			nAY = nTmpY;

		} else
		{
			nBX = nTmpX;
			nBY = nTmpY;
		}
	}

	// :: RIGHT LINE
	if( lines_intersect( rx+width, ry, rx+width, ry+height, x1, y1, x2, y2, &nTmpX, &nTmpY )  == 1 &&
		nTmpX >= rx && nTmpX <= rx+width && nTmpY >= ry && nTmpY <= ry+height )
	{
		if( !bPointA ) 
		{
			bPointA = true;
			nAX = nTmpX;
			nAY = nTmpY;

		} else
		{
			nBX = nTmpX;
			nBY = nTmpY;
		}
	}

	// :: BOTTOM LINE
	if( lines_intersect( rx, ry+height, rx+width, ry+height, x1, y1, x2, y2, &nTmpX, &nTmpY ) == 1  &&
		nTmpX >= rx && nTmpX <= rx+width && nTmpY >= ry && nTmpY <= ry+height )
	{
		if( !bPointA ) 
		{
			bPointA = true;
			nAX = nTmpX;
			nAY = nTmpY;

		} else
		{
			nBX = nTmpX;
			nBY = nTmpY;
		}
	}

	if( x1 < rx || x1 > rx+width || y1 < ry || y1 > ry+height )
	{
		ox1 = (int) nAX;
		oy1 = (int) nAY;
	}

	if( x2 < rx || x2 > rx+width || y2 < ry || y2 > ry+height )
	{
		ox2 = (int) nBX;
		oy2 = (int) nBY;
	}

	return(0);
}

////////////////////////////////////////////////////////////////////
// Purpose:	finds if a line intersects a rectangle
////////////////////////////////////////////////////////////////////
int IsLineIntersectRectZeroAx( int x1, int y1, int x2, int y2, int width,
							  int height, int& ox1, int& oy1, int& ox2, int& oy2 )
{
    int result = 0;

    int c1, c2;
    int right = width-1, bottom = height-1;

    if( right < 0 || bottom < 0 ) return( 0 );

    c1 = (x1 < 0) + (x1 > right) * 2 + (y1 < 0) * 4 + (y1 > bottom) * 8;
    c2 = (x2 < 0) + (x2 > right) * 2 + (y2 < 0) * 4 + (y2 > bottom) * 8;

    if( (c1 & c2) == 0 && (c1 | c2) != 0 )
    {
        int a;

        if( c1 & 12 )
        {
            a = c1 < 8 ? 0 : bottom;
            x1 += (int) (((int) (a - y1)) * (x2 - x1) / (y2 - y1));
            y1 = a;
            c1 = (x1 < 0) + (x1 > right) * 2;
        }
        if( c2 & 12 )
        {
            a = c2 < 8 ? 0 : bottom;
            x2 += (int) (((int) (a - y2)) * (x2 - x1) / (y2 - y1));
            y2 = a;
            c2 = (x2 < 0) + (x2 > right) * 2;
        }
        if( (c1 & c2) == 0 && (c1 | c2) != 0 )
        {
            if( c1 )
            {
                a = c1 == 1 ? 0 : right;
                y1 += (int) (((int) (a - x1)) * (y2 - y1) / (x2 - x1));
                x1 = a;
                c1 = 0;
            }
            if( c2 )
            {
                a = c2 == 1 ? 0 : right;
                y2 += (int) (((int) (a - x2)) * (y2 - y1) / (x2 - x1));
                x2 = a;
                c2 = 0;
            }
        }

		// assert( (c1 & c2) != 0 || (x1 | y1 | x2 | y2) >= 0 );

        ox1 = x1;
        oy1 = y1;
        ox2 = x2;
        oy2 = y2;
    }

    result = ( c1 | c2 ) == 0;

    return result;
}
