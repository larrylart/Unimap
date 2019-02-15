/****************************************
 * 4D Vector Classes
 * By Bill Perone (billperone@yahoo.com)
 * Original: 10-02-2002
 * Revised:  21-11-2003
 *           06-06-2004
 *   
 * Dependancies: My 3D Vector class
 *
 * © 2003, This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 ****************************************/


#pragma once


#include "vector3.h"


//! the w component isn't really used at all in the class, it's just kept along for the ride
template <typename T>
struct vector4: public vector3<T>
{
	T w;

	//! trivial ctor
	vector4<T>() {}

	//! setting ctor
	vector4<T>(const T x0, const T y0, const T z0, const T w0): vector3<T>(x0, y0, z0), w(w0) {}

	//! function call operator
	void operator ()(const T x0, const T y0, const T z0, const T w0) 
	{	x= x0; y= y0; z= z0; w= w0; }

	//! test for equality
	bool operator==(const vector4<T> &v)
	{	return (x==v.x && y==v.y && z==v.z && w==v.w);	}

	//! test for inequality
	bool operator!=(const vector4<T> &v)
	{	return (x!=v.x || y!=v.y || z!=v.z || w!=v.w);	}

	//! set to value
	const vector4<T> &operator =(const vector4<T> &v)
	{	
		x= v.x; y= v.y;	z= v.z;	w= v.w;	
		return *this;
	}
	
	//! negation
	const vector4<T> operator -(void) const
	{	return vector4<T>(-x,-y,-z,w);	}

	//! addition; not sure what to do with w component	
	const vector4<T> operator +(const vector4<T> &v) const
	{   return vector4<T>(x+v.x, y+v.y, z+v.z, w);	 }	

	//! subtraction; results in a vector
	const vector4<T> operator -(const vector4<T> &v) const
	{   return vector4<T>(x-v.x, y-v.y, z-v.z, 0);	 }	

	//! uniform scaling
	const vector4<T> operator *(const T num) const
	{
		vector4<T> temp(*this);			
		return temp*=num;
	}

	//! uniform scaling
	const vector4<T> operator /(const T num) const
	{
		vector4<T> temp(*this);			
		return temp/=num;
	}			

	//! addition
	const vector4<T> &operator +=(const vector4<T> &v)
	{
		x+=v.x;	y+=v.y;	z+=v.z;	
		return *this;
	}

	//! subtraction
	const vector4<T> &operator -=(const vector4<T> &v) 
	{ 
		x-=v.x;	y-=v.y;	z-=v.z;	
		return *this;	
	}

	//! uniform scaling, note this scales the w component as well
	const vector4<T> &operator *=(const T num)
	{
		x*=num; y*=num; z*=num;	w*=num;
		return *this;
	}
	
	//! uniform scaling, note this scales the w component as well
	const vector4<T> &operator /=(const T num)
	{
		x/=num; y/=num; z/=num;	w/=num;								
		return *this;
	}

	//! dot product
	T operator *(const vector4<T> &v) const
	{	return x*v.x + y*v.y + z*v.z;	}

	//! cross product, returns a vector
	const vector4<T> operator %(const vector4<T> &v) const 
	{
		vector4<T> temp(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x, 0);
		return temp;
	}	
};



// macro to make creating the ctors for derived vectors easier
#define VECTOR4_CTORS(name, type)   \
	/* trivial ctor */				\
	name() {}						\
	/* down casting ctor */			\
	name(const vector4<type> &v): vector4<type>(v.x, v.y, v.z, v.w) {}	\
	/* make x,y,z,w combination into a vector */						\
	name(type x0, type y0, type z0, type w0): vector4<type>(x0, y0, z0, w0) {}
	    


struct vector4f: public vector4<float>
{
	VECTOR4_CTORS(vector4f, float)

	//! cast from vector3f to vector4f
	vector4f(const vector3f &v, float w0): vector4<float>(v.x, v.y, v.z, w0) {}

	//! gets the length of this vector squared
	float length_squared() const
	{	return (float)(*this * *this);   }

	//! gets the length of this vector
	float length() const
	{	return (float)sqrt(*this * *this);   }

	//! normalizes this vector
	void normalize()
	{	*this/=length();	}
	
	//! reflects this vector about n
	void reflect(const vector4f &n)
	{
		vector4f orig(*this);
		project(n);
		*this= *this*2 - orig;
	}

	//! projects this vector onto v
	void project(const vector4f &v)
	{	*this= v * (*this * v)/(v*v);	}

	//! returns this vector projected onto v
	vector4f projected(const vector4f &v)
	{   return v * (*this * v)/(v*v);	}
};

