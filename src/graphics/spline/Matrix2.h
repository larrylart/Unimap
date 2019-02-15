/****************************************
 * 2x2 Matrix class
 * By Bill Perone (billperone@yahoo.com)
 * Original: 9-16-2002  
 * Revised: 19-11-2003
 *          28-11-2003
 *          12-12-2003
 *
 * Dependancies: My templatized Matrix functions
 *               My 2d vector classes
 *
 * © 2003, This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 ****************************************/


#pragma once


#include <memory.h>

#include "MatrixN.h"
#include "vector2.h"


//! matrix is in column major form (compatible with openGL)
//! normal C arrays must be transposed before converting
struct matrix2
{
	MATRIX_COMMON_INTERNAL(2)


	matrix2(const vector2f &v0, const vector2f &v1)
	{	col[0]= v0, col[1]= v1;  }

	matrix2(float m00, float m01, float m10, float m11)
	{  
		col[0].x= m00, col[0].y= m01;
		col[1].x= m10, col[1].y= m11;
	}	

	//! construct a rotation matrix
	inline static void rotate(matrix2 &m, float angle)
	{
		m.col[0][0]= cosf(angle); m.col[0][1]= sinf(angle);
		m.col[1][0]= -sinf(angle); m.col[1][1]= cosf(angle);
	}

	//! multiplication by a matrix
	const matrix2 &operator *=(const matrix2 &m)
	{
		matrix2 temp(vector2f(col[0].x*m.col[0].x + col[1].x*m.col[0].y,
			                  col[0].y*m.col[0].x + col[1].y*m.col[0].y),
					 vector2f(col[0].x*m.col[1].x + col[1].x*m.col[1].y,
					          col[0].y*m.col[1].x + col[1].y*m.col[1].y));
		return *this= temp;
	}

	//! multiplication by a vector
	const vector2f operator *(const vector2f &v) const
	{	return vector2f(v.x*col[0].x + v.y*col[1].x, v.x*col[0].y + v.y*col[1].y);	}			

	//! multiplication by a scalar
	const matrix2 &operator *=(const float s)
	{
		col[0]*=s; col[1]*=s;		
		return *this;
	}

	//! division by a scalar
	const matrix2 &operator /=(const float s)
	{
		col[0]/=s; col[1]/=s;		
		return *this;
	}

	//! addition	
	const matrix2 operator +(const matrix2 &m) const
	{   return matrix2(col[0]+m.col[0], col[1]+m.col[1]);	 }	

	//! addition
	const matrix2 &operator +=(const matrix2 &m)
	{
		col[0]+= m.col[0];
		col[1]+= m.col[1];		
		return *this;
	}

	//! subtraction	
	const matrix2 operator -(const matrix2 &m) const
	{   return matrix2(col[0]-m.col[0], col[1]-m.col[1]);	 }	

	//! subtraction
	const matrix2 &operator -=(const matrix2 &m)
	{
		col[0]-= m.col[0];
		col[1]-= m.col[1];		
		return *this;
	}

	//! compute the trace
	float trace() const
	{   return col[0][0] + col[1][1];  }

	//! compute the determinant
	float determinant() const
	{	return col[0][0]*col[1][1] - col[0][1]*col[1][0];  }

	//! invert the matrix
	void invert()
	{
		Swap(col[0].x, col[1].y);
		col[0].y= -col[0].y;
		col[1].x= -col[1].x;		
		*this/=determinant();
	}	

	//! matrix inversion by the tilde operator
	const matrix2 operator ~(void) const
	{   
		matrix2 m(*this);
	    m.invert(); 	 
	    return m;
	}
};


MATRIX_COMMON_EXTERNAL(2)