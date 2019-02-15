/****************************************
 * Rectangle class
 * By Bill Perone (billperone@yahoo.com)
 * Original: 05-12-2003
 *
 * © 2003, This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 ****************************************/

#pragma once


#include "matrix2.h"


template <class type>
struct Rect
{
	vector2<type> p1, p2;	


	Rect<type>() {}
	Rect<type>(const vector2<type> P1, const vector2<type> &P2): p1(P1), p2(P2) {}	
	Rect<type>(const type X, const type Y, const type X2, const type Y2): p1(X,Y), p2(X2,Y2) {}
	
	type Width() const 
	{ return p2.x-p1.x; }

	type Height() const
	{ return p2.y-p1.y; }

	bool Contains(const vector2<type> &p)
	{ return (p2.x >= p.x && p.x >= p1.x) && (p2.y >= p.y && p.y >= p1.y); }	

	bool Contains(const type X, const type Y)
	{ return (p2.x >= X && X >= p1.x) && (p2.y >= Y && Y >= p1.y); }	
};


struct Rectf: public Rect<float>
{
	Rectf() {}
	Rectf(const vector2f &P1, const vector2f &P2): Rect<float>(P1, P2) {}
	Rectf(const float X, const float Y, const float X2, const float Y2): Rect<float>(X,Y,X2,Y2) {}

	//! multiplication by a matrix
	const Rectf operator *(const matrix2 &m) const
	{ return Rectf(m*p1, m*p2); }

	//! multiplication by a matrix
	const Rectf &operator *=(const matrix2 &m)
	{
		p1= m*p1, p2= m*p2;
		return *this;
	}
};


struct Recti: public Rect<int>
{
};

