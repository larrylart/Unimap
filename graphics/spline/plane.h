/****************************************
 * plane class
 * By Bill Perone (billperone@yahoo.com)
 * Original: 10-02-2002  
 * Revised:  29-11-2003
 *           05-12-2003
 *
 * © 2003, This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 ****************************************/


#pragma once


#include "vector3.h"


//! a plane
struct plane
{
	vector3f p;   //!< a point on the plane
	vector3f n;   //!< the normal to the plane


	plane() {}
	plane(const vector3f &P, const vector3f &N): p(P), n(N) {}
	plane(const vector3f &P1, const vector3f &P2, const vector3f &P3): p(P1), n(vector3f((P2-P1)%(P3-P1)).normalized()) {}

	//! returns the distance from this plane to a point
	float distance_squared(const vector3f &P) const
	{	return ((vector3f)(P-p)).projected(n).length_squared();  	}

	//! returns the distance from this plane to a point
	float distance(const vector3f &P) const
	{	return ((vector3f)(P-p)).projected(n).length();  	}

	//! returns the angle between 2 planes with arbitrary length normals
	float angle(const plane &p) const
	{   return vector3f::angle(n, p.n);	}

	//! returns the angle between 2 planes with normalized normals
	float angle_normalized(const plane &p) const
	{   return vector3f::angle_normalized(n, p.n);	}
};

