/****************************************
 * Various constants and utility functions
 * By Bill Perone (billperone@yahoo.com)
 * Original: 17-09-2003   
 * Revised:  05-12-2003
 *           12-12-2003
 *
 * Special thanks to Cbloom, Graphics Gems, Braeden Shosa
 *
 * © 2003, This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 *
 * Notes: 
 *    e^(i*pi) + 1 = 0
 *    e^(i*z) = cos(z) + i*sin(z)
 *    cos(z) = ( e^(i*z) + e^(-i*z) ) / 2
 *    sin(z) = ( e^(i*z) - e^(-i*z) ) / (2*i)
 *    sum(n=0 to k)(z^n) = ( 1-z^(k+1) ) / (1-z)
 *    sine rule: a/sin(A) = b/sin(B) = c/sin(C) 
 *    cos(a)*cos(a) + sin(a)*sin(a) = 1
 *    sin(-t) = -sin(t) 
 *    cos(-t) = cos(t) 
 *    tan(-t) = -tan(t)
 *    sin(pi-t) = sin(t) 
 *    cos(pi-t) = -cos(t) 
 *    tan(pi-t) = -tan(t) 
 *    sin(s+t) = sin(s)*cos(t) + cos(s)*sin(t) 
 *    cos(s+t) = cos(s)*cos(t) - sin(s)*sin(t)
 *    sin(s-t) = sin(s)*cos(t) - cos(s)*sin(t) 
 *    cos(s-t) = cos(s)*cos(t) + sin(s)*sin(t)
 *    sin(2*t) = 2*sin(t)*cos(t) 
 *    cos(2*t) = cos(2*t) - sin(2*t) = 2*cos(2*t) - 1 = 1 - 2*sin(2*t)
 *    sin(t/2) = ±sqrt((1 - cos(t)) / 2) 
 *    cos(t/2) = ±sqrt((1 + cos(t)) / 2)
 *    sin(s) + sin(t) = 2 * sin((s+t)/2) * cos((s-t)/2)
 *    sin(s) - sin(t) = 2 * sin((s-t)/2) * cos((s+t)/2)
 *    cos(s) + cos(t) = 2 * cos((s+t)/2) * cos((s-t)/2)
 *    cos(s) - cos(t) = -2 * sin((s+t)/2) * sin((s-t)/2)
 *    sin(s)*cos(t) = ( sin(s+t) + sin(s-t) ) / 2
 *    cos(s)*cos(t) = ( cos(s+t) + cos(s-t) ) / 2
 *    sin(s)*sin(t) = ( cos(s-t) - cos(s+t) ) / 2
 ****************************************/


#pragma once


#include "Assert.h"


// swap a and b
template<class Type> inline void Swap(Type &a,Type &b)
{  Type c(a); a = b; b = c;  }

// clamp x to be within lo & hi range
#define CLAMP(v,l,h)	((v) < (l) ? (l) : (v) > (h) ? (h) : (v))

// minimum of a or b
#define MIN(a,b)    ( (a) < (b) ? (a) : (b) )

// maximum of a or b
#define MAX(a,b)    ( (a) > (b) ? (a) : (b) )

// absolute value of a 
#define ABS(a)		(((a) < 0) ? -(a) : (a))

// round a to nearest int 
#define ROUND(a)	((a) > 0 ? (int)((a)+0.5) : -(int)(0.5-(a)))

// take sign of a, either -1, 0, or 1 
#define ZSGN(a)		(((a)<0) ? -1 : (a) > 0 ? 1 : 0)	

// take binary sign of a, either -1, or 1 if >= 0 
#define SGN(a)		(((a)<0) ? -1 : 1)




// cast float to it's bitwise representation
#define FLOAT_AS_INT(f)			(reinterpret_cast<const unsigned long &>(f))
#define INT_AS_FLOAT(i)			(reinterpret_cast<const float &>(i))

#define DBL_EPSILON     2.2204460492503131e-016 // smallest such that 1.0+DBL_EPSILON != 1.0 
#define DBL_MAX         1.7976931348623158e+308 // max value 
#define DBL_MIN         2.2250738585072014e-308 // min positive value 
#define FLT_EPSILON     1.192092896e-07F        // smallest such that 1.0+FLT_EPSILON != 1.0 
#define FLT_MAX         3.402823466e+38F        // max value 
#define FLT_MIN         1.175494351e-38F        // min positive value 


namespace Math
{
	// pi
	const double _PI=       3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;
	// pi^2
	const double PI_SQ=    _PI*_PI;
	// degrees to radians conversion
	const double _DEG2RAD=  _PI/180;
	// radians to degrees conversion
	const double _RAD2DEG=  180/_PI;
	// natural logarithmic base
    const double E=        2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274;
	// euler constant
	const double GAMMA=    0.57721566490153286060651209008240243104215933593992359880576723488486772677766467093694706329174674;
	// golden ratio = (1+sqrt(5))/2
	const double PHI=      1.6180339887498948482045868343656381177203091798057628621354486227052604628189024497072072041893911374;
	// 1/log(2)
	const double inv_log2= 1.44269504088896340735992468100189213742664595415298593413544940693110921918118507988552662289350;
	// ln(10)
	const double LN10=     2.3025850929940456840179914546843642076011014886287729760333279009675726096773524802359972050895982983;
	// ln(pi)
	const double LNPI=     1.1447298858494001741434273513530587116472948129153115715136230714721377698848260797836232702754897077;	
	// lg(e)
	const double LOG2E=    1.44269504088896340736;
	// log(e)
	const double LOG10E=   0.434294481903251827651;
	// ln(2)
	const double LN2=      0.693147180559945309417;	
	// sqrt(2)
	const double SQRT2=    1.4142135623730950488016887242097;
	// sqrt(3)
	const double SQRT3=    1.7320508075688772935274463415059;


	//! evaluates a degree (arraysize-1) polynomial in the order x^n*array[0] + ... + array[arraysize-1]
	template <class type> 
	inline type HornerEval(const float x, const type *array, unsigned int arraysize)
	{			
	    type p = *array;
	    while (--arraysize) p= *++array + p*x;	
		return p;
	}


	//! linear interpolation of 2 points
	template <class type>
	inline double interp_lin(const type &a, const type &b, float t)
	{	return a*(1-t) + b*t; 	}

	//! cosine interpolation of 2 points
	template <class type>
	inline double interp_cos(const type &a, const type &b, float t)
	{   
		t = (1 - cos(t*PI))/2;
	    return  a*(1-t) + b*t;
	}

	//! returns the cubic interpolation of 4 points
	template <class type>
	inline double interp_cubic(const type &v0, const type &v1, const type &v2, const type &v3, float t)
	{
		type P = (v3 - v2) - (v0 - v1);
		type Q = (v0 - v1) - P;	
		return ((P*t + Q)*t + (v2-v0))*t + v1; //using expanded out horner's rule
	}	


	//! retuns the angle to side a given an arbitrary triangle of sides of length a,b,c
	inline float cosine_law_sss(float a, float b, float c)
	{   return acosf((b*b + c*c - a*a)/(2*b*c));  }

	//! returns the length of the side across from angle in an arbitrary triangle
	inline float cosine_law_sas(float len1, float angle, float len2)
	{   return sqrtf(len1*len1 + len2*len2 - 2*len1*len2*cosf(angle));  }


	//! returns if a float is valid
	inline bool valid(const float f)
	{
		// assert on the freed memory values :
		// these *are* valid floats, but the chance of
		// getting one on purpose is very slim
		Assert(FLOAT_AS_INT(f) != 0xCDCDCDCD, "Probably incorrect float: 0xCDCDCDCD");
		Assert(FLOAT_AS_INT(f) != 0xDDDDDDDD, "Probably incorrect float: 0xDDDDDDDD");
		
		// this works because NAN always returns false on compares 
		return (f >= -FLT_MAX && f <= FLT_MAX);
	}

	//! performs a fast round on a float
	inline int _round(const float f)
	{
		// Add a magical cookie to the float to transform its bits into its rounded integer representation
		// See http://www.d6.com/users/checker/pdfs/gdmfp.pdf		
		return (int)( (double)(f + 6755399441055744.L) );
	}

	// tests for floating point fuzzy equalities
	inline bool equal(const float f1,const float f2, const float epsilon= FLT_EPSILON)
	{	return f1 >= f2 - epsilon && f1 <= f2 + epsilon;	}

	inline bool lessthan(const float f1,const float f2, const float epsilon= FLT_EPSILON)
	{   return f1 + epsilon < f2;  }

	inline bool lessthanorequalto(const float f1,const float f2, const float epsilon= FLT_EPSILON)
	{   return f1 + epsilon <= f2;  }

	inline bool greaterthan(const float f1,const float f2, const float epsilon= FLT_EPSILON)
	{   return f1 - epsilon > f2;  }

	inline bool greaterthanorequalto(const float f1,const float f2, const float epsilon= FLT_EPSILON)
	{   return f1 - epsilon >= f2;  }
};

