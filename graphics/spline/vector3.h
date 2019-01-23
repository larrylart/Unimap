/****************************************
 * 3D Vector Classes
 * By Bill Perone (billperone@yahoo.com)
 * Original: 9-16-2002
 * Revised: 19-11-2003
 *          11-12-2003
 *          18-12-2003
 *          06-06-2004
 *   
 * Dependancies: My 2D Vector class
 *
 * © 2003, This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 *
 * Notes:
 * if a*b = 0 then a & b are orthogonal
 * a%b = -b%a
 * a*(b%c) = (a%b)*c
 * a%b = a(cast to matrix)*b
 * (a%b).length() = area of parallelogram formed by a & b
 * (a%b).length() = a.length()*b.length() * sin(angle between a & b)
 * (a%b).length() = 0 if angle between a & b = 0 or a.length() = 0 or b.length() = 0
 * a * (b%c) = volume of parallelpiped formed by a, b, c
 * vector triple product: a%(b%c) = b*(a*c) - c*(a*b)
 * scalar triple product: a*(b%c) = c*(a%b) = b*(c%a)
 * vector quadruple product: (a%b)*(c%d) = (a*c)*(b*d) - (a*d)*(b*c)
 * if a is unit vector along b then a%b = -b%a = -b(cast to matrix)*a = 0
 * vectors a1...an are linearly dependant if there exists a vector of scalars (b) where a1*b1 + ... + an*bn = 0
 *           or if the matrix (A) * b = 0
 * 
 ****************************************/


#pragma once


#include "vector2.h"


template <typename T>
struct vector3: public vector2<T>
{
	T z;

	//! trivial ctor
	vector3<T>() {}

	//! setting ctor
	vector3<T>(const T x0, const T y0, const T z0): vector2<T>(x0, y0), z(z0) {}

	//! function call operator
	void operator ()(const T x0, const T y0, const T z0) 
	{	x= x0; y= y0; z= z0;  }

	//! test for equality
	bool operator==(const vector3<T> &v)
	{	return (x==v.x && y==v.y && z==v.z);	}

	//! test for inequality
	bool operator!=(const vector3<T> &v)
	{	return (x!=v.x || y!=v.y || z!=v.z);	}

	//! set to value
	const vector3<T> &operator =(const vector3<T> &v)
	{	
		x= v.x; y= v.y;	z= v.z;		
		return *this;
	}
	
	//! negation
	const vector3<T> operator -(void) const
	{	return vector3<T>(-x,-y,-z);	}

	//! addition	
	const vector3<T> operator +(const vector3<T> &v) const
	{   return vector3<T>(x+v.x, y+v.y, z+v.z);	 }	

	//! subtraction
	const vector3<T> operator -(const vector3<T> &v) const
	{   return vector3<T>(x-v.x, y-v.y, z-v.z);	 }	

	//! uniform scaling
	const vector3<T> operator *(const T num) const
	{
		vector3<T> temp(*this);			
		return temp*=num;
	}

	//! uniform scaling
	const vector3<T> operator /(const T num) const
	{
		vector3<T> temp(*this);			
		return temp/=num;
	}			

	//! addition
	const vector3<T> &operator +=(const vector3<T> &v)
	{
		x+=v.x;	y+=v.y;	z+=v.z;					
		return *this;
	}

	//! subtraction
	const vector3<T> &operator -=(const vector3<T> &v) 
	{ 
		x-=v.x;	y-=v.y;	z-=v.z;					
		return *this;	
	}

	//! uniform scaling
	const vector3<T> &operator *=(const T num)
	{
		x*=num; y*=num; z*=num;									
		return *this;
	}
	
	//! uniform scaling
	const vector3<T> &operator /=(const T num)
	{
		x/=num; y/=num; z/=num;									
		return *this;
	}

	//! dot product
	T operator *(const vector3<T> &v) const
	{	return x*v.x + y*v.y + z*v.z;	}

	//! cross product
	const vector3<T> operator %(const vector3<T> &v) const 
	{
		vector3<T> temp(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
		return temp;
	}	
};



// macro to make creating the ctors for derived vectors easier
#define VECTOR3_CTORS(name, type)   \
	/* trivial ctor */				\
	name() {}						\
	/* down casting ctor */			\
	name(const vector3<type> &v): vector3<type>(v.x, v.y, v.z) {}	\
	/* make x,y,z combination into a vector */						\
	name(type x0, type y0, type z0): vector3<type>(x0, y0, z0) {}
	    



struct vector3i: public vector3<int>
{
	VECTOR3_CTORS(vector3i, int)
};


struct vector3ui: public vector3<unsigned int>
{
	VECTOR3_CTORS(vector3ui, unsigned int)
};


struct vector3f: public vector3<float>
{
	VECTOR3_CTORS(vector3f, float)

	//! gets the length of this vector squared
	float length_squared() const
	{	return (float)(*this * *this);   }

	//! gets the length of this vector
	float length() const
	{	return (float)sqrt(*this * *this);   }

	//! normalizes this vector
	void normalize()
	{	*this/=length();	}

	//! returns the normalized version of this vector
	vector3f normalized() const
	{   return  *this/length();  }
	
	//! reflects this vector about n
	void reflect(const vector3f &n)
	{
		vector3f orig(*this);
		project(n);
		*this= *this*2 - orig;
	}

	//! projects this vector onto v
	void project(const vector3f &v)
	{	*this= v * (*this * v)/(v*v);	}

	//! returns this vector projected onto v
	vector3f projected(const vector3f &v)
	{   return v * (*this * v)/(v*v);	}
	
	//! computes the angle between 2 arbitrary vectors
	static inline float angle(const vector3f &v1, const vector3f &v2)
	{   return acosf((v1*v2) / (v1.length()*v2.length()));  }

	//! computes the angle between 2 arbitrary normalized vectors
	static inline float angle_normalized(const vector3f &v1, const vector3f &v2)
	{   return acosf(v1*v2);  }

	//! converts a normalized axis angle rotation to euler angles
	// doesnt work
/*	static inline vector3f axis_angle_to_euler(const vector3f &axis, float angle)
	{
		vector3f euler;
		float    c= 1-cosf(angle), s= sinf(angle);

		euler.x = atan2f(axis.x*axis.y*c + axis.z*s, 1 - (axis.y*axis.y-axis.z*axis.z)*c);
		euler.y = atan2f(axis.y*axis.z*c + axis.x*s, 1 - (axis.x*axis.x-axis.y*axis.y)*c);
		euler.z = asinf(-axis.x*axis.z*c - axis.y*s);
		return euler;
	}*/
};
