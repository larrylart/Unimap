/****************************************
 * Line, Ray and Line segment classes for 3d
 * By Bill Perone (billperone@yahoo.com)
 * Original: 10-02-2002  
 *
 * © 2003, This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 ****************************************/


#pragma once

#include "vector3.h"
#include <math.h>


//! a line in 3d space
struct line3
{
	vector3f p;   //!< a point on the line
	vector3f dir; //!< the direction of the line


	line3() {}
	line3(const vector3f &P, const vector3f &Dir): p(P), dir(Dir) {}

	//! returns the distance from this line to a point
	float distance(const vector3f &P)
	{	return ((vector3f)(dir%(P-p))).length()/dir.length();	}

	//! returns the angle between l and this line
	float angle(const line3 &l)
	{	return (float)acos((dir*l.dir)/(dir.length()*l.dir.length()));	}	
};


//! a ray in 3d space
struct ray3: public line3
{
};


//! a line segment in 3d space
struct lineseg3: public line3
{
	lineseg3() {}
	lineseg3(const vector3f &P1, const vector3f &P2): line3(P1, P2-P1) {}
};

