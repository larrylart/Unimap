/****************************************
 * Line, Ray and Line segment classes for 2d
 * By Bill Perone (billperone@yahoo.com)
 * Original: 10-02-2002  
 *
 * © 2003, This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 ****************************************/


#pragma once

#include "vector2.h"
#include <math.h>


//! a line in 2d space
struct line2
{
	vector2f p;   //!< a point on the line
	vector2f dir; //!< the direction of the line


	line2() {}
	line2(const vector2f &P, const vector2f &Dir): p(P), dir(Dir) {}

	//! returns the distance from this line to a point
	//float distance(const vector2f &P)
	//{	return ((vector3f)(dir%(P-p))).length()/dir.length();	}

	//! returns the angle between l and this line
	float angle(const line2 &l)
	{	return (float)acos((dir*l.dir)/(dir.length()*l.dir.length()));  }
};


//! a ray in 2d space
struct ray2: public line2
{
};


//! a line segment in 2d space
struct lineseg2: public line2
{
	lineseg2() {}
	lineseg2(const vector2f &P1, const vector2f &P2): line2(P1, P2-P1) {}
};


