/****************************************
 * 3x3 Matrix class
 * By Bill Perone (billperone@yahoo.com)
 * Original: 9-19-2002  
 * Revised:  25-09-2003
 *           19-11-2003
 *           28-11-2003
 *           12-12-2003
 *
 * Dependancies: My templatized Matrix functions
 *               My 3d vector classes
 *
 * © 2003, This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 *
 * Notes: singular matrix is a matrix whose determinant = 0
 *        inverse of A = adjoint(A) / determinant(A)
 *        a matrix (A) is positive definite if x(transpose)*A*x > 0 (where x is any non zero vector)
 ****************************************/


#pragma once

#include "matrix2.h"
#include "vector3.h"


//! matrix is in column major form (compatible with openGL)
//! normal C arrays must be transposed before converting
struct matrix3
{
	MATRIX_COMMON_INTERNAL(3)


	//! casting from a vector3f to a matrix is the tilde operator
	matrix3(const vector3f &v)
	{
		col[0].x= 0,   col[0].y= -v.z, col[0].z= v.y;
		col[1].x= v.z, col[1].y=  0,   col[1].z=-v.x; 
		col[2].x= -v.y,col[2].y=  v.x, col[2].z= 0;
	}

	matrix3(const vector3f &v0, const vector3f &v1, const vector3f &v2)
	{	col[0]= v0, col[1]= v1, col[2]= v2;  }

	matrix3(float m00, float m01, float m02,
		    float m10, float m11, float m12,
			float m20, float m21, float m22)
	{  
	   col[0].x= m00, col[0].y= m01, col[0].z= m02;
	   col[1].x= m10, col[1].y= m11, col[1].z= m12;
	   col[2].x= m20, col[2].y= m21, col[2].z= m22;
	}
	
	//! reorthogonalizes a matrix
	void reorthogonalize()
	{
		col[0].normalize();
		// using iterated Gram-Schmidt procedure
		col[1]= col[1] - col[0]*(col[1]*col[0]);
		col[1].normalize();
		col[2]= col[0]%col[1];
		//col[2].normalize(); // should already be normalized because 0 & 1 are normalized
	}
	
	//! construct an x axis rotation matrix
	inline static void rotate_x(matrix3 &m, float angle)
	{
		m.col[0][0]= 1; m.col[0][1]= 0; m.col[0][2]= 0;
		m.col[1][0]= 0; m.col[1][1]= cosf(angle); m.col[1][2]= sinf(angle);
		m.col[2][0]= 0; m.col[2][1]= -sinf(angle); m.col[2][2]= cosf(angle);
	}

	//! construct an y axis rotation matrix
	inline static void rotate_y(matrix3 &m, float angle)
	{
		m.col[0][0]= cosf(angle); m.col[0][1]= 0; m.col[0][2]= -sinf(angle);
		m.col[1][0]= 0; m.col[1][1]= 0; m.col[1][2]= 0;
		m.col[2][0]= sinf(angle); m.col[2][1]= 0; m.col[2][2]= cosf(angle);
	}

	//! construct an z axis rotation matrix
	inline static void rotate_z(matrix3 &m, float angle)
	{
		m.col[0][0]= cosf(angle); m.col[0][1]= sinf(angle); m.col[0][2]= 0;
		m.col[1][0]= -sinf(angle); m.col[1][1]= cosf(angle);  m.col[1][2]= 0;
		m.col[2][0]= 0;           m.col[2][1]= 0;            m.col[2][2]= 1;
	}

	//! constructs a rotation matrix around an arbitrary vector
	inline static void rotate(matrix3 &m, const vector3f &v, const float angle)
	{
		float c= 1-cosf(angle), s= sinf(angle), cxy= c*v.x*v.y, cxz= c*v.x*v.z, cyz= c*v.y*v.z;

		m.col[0][0]= c*v.x*v.x + 1-c; m.col[0][1]= cxy     - v.z*s;	m.col[0][2]= cxz + v.y*s; 
		m.col[1][0]= cxy     + v.z*s; m.col[1][1]= c*v.y*v.y + 1-c; m.col[1][2]= cyz - v.x*s; 
		m.col[2][0]= cxz     - v.y*s; m.col[2][1]= cyz     + v.x*s; m.col[2][2]= c*v.z*v.z + 1-c; 
	}

	//! sets a rotation matrix up by euler angles
	//! uses pitch, roll, yaw (xyz) form
	inline static void rotate_euler(matrix3 &m, float x, float y, float z)
	{
		double cx = cos(x);
		double sx = sin(x);
		double cy = cos(y);
		double sy = sin(y);
		double cz = cos(z);
		double sz = sin(z);
		
		m.col[0].x = (float)(cy*cz); 
		m.col[0].y = (float)(cy*sz);
		m.col[0].z = (float)(-sy);

		double sxsy = sx*sy;
		double cxsy = cx*sy;
		
		m.col[1].x = (float)(sxsy*cz-cx*sz);
		m.col[1].y = (float)(sxsy*sz+cx*cz);
		m.col[1].z = (float)(sx*cy);
	
		m.col[2].x = (float)(cxsy*cz+sx*sz);
		m.col[2].y = (float)(cxsy*sz-sx*cz);
		m.col[2].z = (float)(cx*cy);
	}

	//! sets a rotation matrix up by euler angles
	//! uses pitch, roll, yaw (xyz) form
	inline static void rotate_euler(matrix3 &m, const vector3f &angles)
	{	rotate_euler(m, angles.x, angles.y, angles.z);	}

	//! multiplication by a matrix
	const matrix3 &operator *=(const matrix3 &m)
	{
		matrix3 temp(vector3f(col[0].x*m.col[0].x + col[1].x*m.col[0].y + col[2].x*m.col[0].z,
							  col[0].y*m.col[0].x + col[1].y*m.col[0].y + col[2].y*m.col[0].z,
							  col[0].z*m.col[0].x + col[1].z*m.col[0].y + col[2].z*m.col[0].z),
					 vector3f(col[0].x*m.col[1].x + col[1].x*m.col[1].y + col[2].x*m.col[1].z,
					          col[0].y*m.col[1].x + col[1].y*m.col[1].y + col[2].y*m.col[1].z,
                              col[0].z*m.col[1].x + col[1].z*m.col[1].y + col[2].z*m.col[1].z),
					 vector3f(col[0].x*m.col[2].x + col[1].x*m.col[2].y + col[2].x*m.col[2].z,
						      col[0].y*m.col[2].x + col[1].y*m.col[2].y + col[2].y*m.col[2].z,
							  col[0].z*m.col[2].x + col[1].z*m.col[2].y + col[2].z*m.col[2].z));		
		return *this= temp;
	}

	//! multiplication by a vector
	const vector3f operator *(const vector3f &v) const
	{	return vector3f(v.x*col[0].x + v.y*col[1].x + v.z*col[2].x, 
	                    v.x*col[0].y + v.y*col[1].y + v.z*col[2].y, 
	                    v.x*col[0].z + v.y*col[1].z + v.z*col[2].z);	}			

	//! multiplication by a scalar
	const matrix3 &operator *=(const float s)
	{
		col[0]*=s; col[1]*=s; col[2]*=s;		
		return *this;
	}

	//! division by a scalar
	const matrix3 &operator /=(const float s)
	{
		col[0]/=s; col[1]/=s; col[2]/=s;		
		return *this;
	}

	//! addition	
	const matrix3 operator +(const matrix3 &m) const
	{   return matrix3(col[0]+m.col[0], col[1]+m.col[1], col[2]+m.col[2]);	 }	

	//! addition
	const matrix3 &operator +=(const matrix3 &m)
	{
		col[0]+= m.col[0];
		col[1]+= m.col[1];
		col[2]+= m.col[2];
		return *this;
	}

	//! subtraction	
	const matrix3 operator -(const matrix3 &m) const
	{   return matrix3(col[0]-m.col[0], col[1]-m.col[1], col[2]-m.col[2]);	 }	

	//! subtraction
	const matrix3 &operator -=(const matrix3 &m)
	{
		col[0]-= m.col[0];
		col[1]-= m.col[1];
		col[2]-= m.col[2];
		return *this;
	}

	//! compute the trace
	float trace() const
	{   return col[0].x + col[1].y + col[2].z;  }

	//! returns the 2x2 submatrix with row i, column j deleted
	matrix2 submatrix(unsigned char i, unsigned char j) const
	{	
		switch(i)
		{
		case 0: switch(j)
				{
				case 0: return matrix2(col[1].y,col[1].z, col[2].y,col[2].z);
				case 1: return matrix2(col[0].y,col[0].z, col[2].y,col[2].z);
				default:return matrix2(col[0].y,col[0].z, col[1].y,col[1].z);// case 2
				} 
		case 1: switch(j)
				{
				case 0: return matrix2(col[1].x,col[1].z, col[2].x,col[2].z);
				case 1: return matrix2(col[0].x,col[0].z, col[2].x,col[2].z);
				default:return matrix2(col[0].x,col[0].z, col[1].x,col[1].z); // case 2
				} 
		default: switch(j) // case 2
				{
				case 0: return matrix2(col[1].x,col[1].y, col[2].x,col[2].y);
				case 1: return matrix2(col[0].x,col[0].y, col[2].x,col[2].y);
				default:return matrix2(col[0].x,col[0].y, col[1].x,col[1].y); // case 2
				} 
		}
	}

	//! compute the determinant
	float determinant() const
	{	return col[0].x*(col[1].y*col[2].z-col[1].z*col[2].y) - 
	           col[1].x*(col[0].y*col[2].z-col[0].z*col[2].y) + 
			   col[2].x*(col[0].y*col[1].z-col[0].z*col[1].y);  }

	//! invert the matrix
	void invert()
	{
	    unsigned char i, k;
		float	detA= determinant();
		matrix3 invmat;
		bool    sign= false;

		
		if (!detA) return;		
		for (i=0; i < 3; ++i)
		for (k=0; k < 3; ++k, sign= !sign) 
		{			
			sign ? invmat[i][k]= -submatrix(i, k).determinant()/detA
			     : invmat[i][k]= submatrix(i, k).determinant()/detA;			
		}
		*this= invmat;
	}	

	//! matrix inversion by the tilde operator
	const matrix3 operator ~(void) const
	{   
		matrix3 m(*this);
	    m.invert(); 	 
	    return m;
	}
};



MATRIX_COMMON_EXTERNAL(3)
