/****************************************
 * 4x4 Matrix class
 * By Bill Perone (billperone@yahoo.com)
 * Original: 10-02-2002  
 * Revised:  25-09-2003
 *           24-10-2003
 *           19-11-2003
 *           28-11-2003
 *           12-12-2003
 *           20-12-2003
 *
 * © 2003, This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 ****************************************/


#pragma once


#include "matrix3.h"
#include "vector4.h"


//! matrix is in column major form (compatible with openGL)
//! normal C arrays must be transposed before converting
struct matrix4
{
	MATRIX_COMMON_INTERNAL(4)
	

	matrix4(const vector4f &v0, const vector4f &v1, const vector4f &v2, const vector4f &v3)
	{	col[0]= v0, col[1]= v1, col[2]= v2, col[3]= v3;  }

	matrix4(float m00, float m01, float m02, float m03,
		    float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33)
	{  
	   col[0].x= m00, col[0].y= m01, col[0].z= m02, col[0].w= m03;
	   col[1].x= m10, col[1].y= m11, col[1].z= m12, col[1].w= m13;
	   col[2].x= m20, col[2].y= m21, col[2].z= m22, col[2].w= m23;
	   col[3].x= m30, col[3].y= m31, col[3].z= m32, col[3].w= m33;
	}

	//! builds a matrix from a 3x3 basis matrix and a translation vector
	matrix4(const vector3f &top, const vector3f &mid, const vector3f &bot, const vector3f &trans)
	{
		col[0].x= top.x; col[1].x= top.y; col[2].x= top.z; col[3].x= trans.x;		
		col[0].y= mid.x; col[1].y= mid.y; col[2].y= mid.z; col[3].y= trans.y;		
		col[0].z= bot.x; col[1].z= bot.y; col[2].z= bot.z; col[3].z= trans.z;	
		col[0].w= 0;     col[1].w= 0;     col[2].w= 0;     col[3].w= 1;

		/*col[0].x= top.x; col[1].x= mid.x; col[2].x= bot.x; col[3].x= trans.x;		
		col[0].y= top.y; col[1].y= mid.y; col[2].y= bot.y; col[3].y= trans.y;		
		col[0].z= top.z; col[1].z= mid.z; col[2].z= bot.z; col[3].z= trans.z;	
		col[0].w= 0;     col[1].w= 0;     col[2].w= 0;     col[3].w= 1;*/
	}

	//! builds a matrix from a 3x3 basis matrix and a translation vector
	matrix4(const matrix3 &m, const vector3f &trans)
	{
		col[0].x= m[0].x; col[1].x= m[1].x; col[2].x= m[2].x; col[3].x= trans.x;		
		col[0].y= m[0].y; col[1].y= m[1].y; col[2].y= m[2].y; col[3].y= trans.y;		
		col[0].z= m[0].z; col[1].z= m[1].z; col[2].z= m[2].z; col[3].z= trans.z;	
		col[0].w= 0;      col[1].w= 0;      col[2].w= 0;      col[3].w= 1;
	}

	//! construct an x axis rotation matrix on 3x3 submatrix
	inline static void rotate_x(matrix4 &m, float angle)
	{
		m.col[0][0]= 1; m.col[0][1]= 0; m.col[0][2]= 0;
		m.col[1][0]= 0; m.col[1][1]= cosf(angle); m.col[1][2]= sinf(angle);
		m.col[2][0]= 0; m.col[2][1]= -sinf(angle); m.col[2][2]= cosf(angle);
	}

	//! construct an y axis rotation matrix on 3x3 submatrix
	inline static void rotate_y(matrix4 &m, float angle)
	{
		m.col[0][0]= cosf(angle); m.col[0][1]= 0; m.col[0][2]= -sinf(angle);
		m.col[1][0]= 0; m.col[1][1]= 0; m.col[1][2]= 0;
		m.col[2][0]= sinf(angle); m.col[2][1]= 0; m.col[2][2]= cosf(angle);
	}

	//! construct an z axis rotation matrix on 3x3 submatrix
	inline static void rotate_z(matrix4 &m, float angle)
	{
		m.col[0][0]= cosf(angle); m.col[0][1]= sinf(angle); m.col[0][2]= 0;
		m.col[1][0]= -sinf(angle); m.col[1][1]= cosf(angle); m.col[1][2]= 0;
		m.col[2][0]= 0; m.col[2][1]= 0; m.col[2][2]= 1;
	}	

	//! sets a rotation matrix up by euler angles
	//! uses pitch, roll, yaw (xyz) form
	inline static void rotate_euler(matrix4 &m, float x, float y, float z)
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

	//! sets a rotation matrix up for the 3x3 submatrix by euler angles
	//! uses pitch, roll, yaw (xyz) form
	inline static void rotate_euler(matrix4 &m, const vector3f &angles)
	{	rotate_euler(m, angles.x, angles.y, angles.z);	}

	//! sets the translation values for a matrix
	inline static void translation(matrix4 &m, float x, float y, float z)
	{
		m.col[3].x= x;		
		m.col[3].y= y;		
		m.col[3].z= z;		
	}

	//! sets the translation values for a matrix
	inline static void translation(matrix4 &m, const vector3f &t)
	{	translation(m, t.x, t.y, t.z);		}

	//! generates a perspective transformation matrix in the same manner opengl does
	inline static matrix4 frustum(float left, float right, float bottom, float top, float Near, float Far)
	{
		float width= right-left,
			  height= top-bottom,
			  depth= Far-Near,
			  near2= Near*2;

		return matrix4(near2/width, 0, (right+left)/width, 0,
			           0, near2/height, (top+bottom)/height, 0, 
					   0, 0, -(Far+Near)/depth, -near2*Far/depth,
					   0, 0, -1, 0);
	}

	//! generates an orthographic projection matrix in the same manner opengl does
	inline static matrix4 orthographic(float left, float right, float bottom, float top, float Near, float Far)
	{
		float width= right-left,
			  height= top-bottom,
			  depth= Far-Near;

		return matrix4(2/width, 0,0, -(right+left)/width,
			           0, 2/height, 0, -(top+bottom)/height,
					   0,0,-2/depth, (Far+Near)/depth,
					   0,0,0,1);
	}

	//! generates a perspective projection matrix just like gluPerspective
	inline static matrix4 perspective(float fovy, float aspect, float zNear, float zFar)
	{
		float f= 1/tanf(fovy/2);
		return matrix4(f/aspect, 0,0,0,
			           0,f,0,0,
					   0,0, (zNear+zFar)/(zNear-zFar), 2*zNear*zFar/(zNear-zFar),
					   0,0,-1,0);
	}
		
	//! multiplication by a matrix
	const matrix4 &operator *=(const matrix4 &m)
	{
		matrix4 temp(col[0].x*m.col[0].x + col[1].x*m.col[0].y + col[2].x*m.col[0].z + col[3].x*m.col[0].w,
			         col[0].y*m.col[0].x + col[1].y*m.col[0].y + col[2].y*m.col[0].z + col[3].y*m.col[0].w,
					 col[0].z*m.col[0].x + col[1].z*m.col[0].y + col[2].z*m.col[0].z + col[3].z*m.col[0].w,
					 col[0].w*m.col[0].x + col[1].w*m.col[0].y + col[2].w*m.col[0].z + col[3].w*m.col[0].w,

			         col[0].x*m.col[1].x + col[1].x*m.col[1].y + col[2].x*m.col[1].z + col[3].x*m.col[1].w,
			         col[0].y*m.col[1].x + col[1].y*m.col[1].y + col[2].y*m.col[1].z + col[3].y*m.col[1].w,
					 col[0].z*m.col[1].x + col[1].z*m.col[1].y + col[2].z*m.col[1].z + col[3].z*m.col[1].w,
					 col[0].w*m.col[1].x + col[1].w*m.col[1].y + col[2].w*m.col[1].z + col[3].w*m.col[1].w,

					 col[0].x*m.col[2].x + col[1].x*m.col[2].y + col[2].x*m.col[2].z + col[3].x*m.col[2].w,
			         col[0].y*m.col[2].x + col[1].y*m.col[2].y + col[2].y*m.col[2].z + col[3].y*m.col[2].w,
					 col[0].z*m.col[2].x + col[1].z*m.col[2].y + col[2].z*m.col[2].z + col[3].z*m.col[2].w,
					 col[0].w*m.col[2].x + col[1].w*m.col[2].y + col[2].w*m.col[2].z + col[3].w*m.col[2].w,
					  
					 col[0].x*m.col[3].x + col[1].x*m.col[3].y + col[2].x*m.col[3].z + col[3].x*m.col[3].w,
			         col[0].y*m.col[3].x + col[1].y*m.col[3].y + col[2].y*m.col[3].z + col[3].y*m.col[3].w,
					 col[0].z*m.col[3].x + col[1].z*m.col[3].y + col[2].z*m.col[3].z + col[3].z*m.col[3].w,
					 col[0].w*m.col[3].x + col[1].w*m.col[3].y + col[2].w*m.col[3].z + col[3].w*m.col[3].w);
		return *this= temp;
	}

	//! multiplication by a vector
	const vector4f operator *(const vector4f &v) const
	{	return vector4f(v.x*col[0].x + v.y*col[1].x + v.z*col[2].x + v.w*col[3].x, 
	                    v.x*col[0].y + v.y*col[1].y + v.z*col[2].y + v.w*col[3].y, 
	                    v.x*col[0].z + v.y*col[1].z + v.z*col[2].z + v.w*col[3].z,
						v.x*col[0].w + v.y*col[1].w + v.z*col[2].w + v.w*col[3].w);	}			
	
	//! multiplication by a scalar
	const matrix4 &operator *=(const float s)
	{
		col[0]*=s; col[1]*=s; col[2]*=s; col[3]*=s;
		return *this;
	}

	//! division by a scalar
	const matrix4 &operator /=(const float s)
	{
		col[0]/=s; col[1]/=s; col[2]/=s; col[3]/=s;
		return *this;
	}

	//! addition	
	const matrix4 operator +(const matrix4 &m) const
	{   return matrix4(vector4f(col[0]+m.col[0]), vector4f(col[1]+m.col[1]), 
	                   vector4f(col[2]+m.col[2]), vector4f(col[3]+m.col[3]));	 }	

	//! addition
	const matrix4 &operator +=(const matrix4 &m)
	{
		col[0]+= m.col[0];
		col[1]+= m.col[1];
		col[2]+= m.col[2];
		col[3]+= m.col[3];
		return *this;
	}

	//! subtraction	
	const matrix4 operator -(const matrix4 &m) const
	{   return matrix4(vector4f(col[0]-m.col[0]), vector4f(col[1]-m.col[1]), 
	                   vector4f(col[2]-m.col[2]), vector4f(col[3]-m.col[3]));	 }	

	//! subtraction
	const matrix4 &operator -=(const matrix4 &m)
	{
		col[0]-= m.col[0];
		col[1]-= m.col[1];
		col[2]-= m.col[2];
		col[3]-= m.col[3];
		return *this;
	}

	//! compute the trace
	float trace() const
	{   return col[0].x + col[1].y + col[2].z + col[3].w;  }

	//! returns the 3x3 submatrix with row i, column j deleted
	matrix3 submatrix(unsigned char i, unsigned char j) const
	{
		switch(i)
		{
		case 0: switch(j)
				{
				case 0: return matrix3(col[1].y,col[1].z,col[1].w, 
							           col[2].y,col[2].z,col[2].w,
									   col[3].y,col[3].z,col[3].w);									   
				case 1: return matrix3(col[0].y,col[0].z,col[0].w, 
							           col[2].y,col[2].z,col[2].w,
									   col[3].y,col[3].z,col[3].w);									   
				case 2: return matrix3(col[0].y,col[0].z,col[0].w, 
							           col[1].y,col[1].z,col[1].w,
									   col[3].y,col[3].z,col[3].w);
				default:return matrix3(col[0].y,col[0].z,col[0].w, // case 3
							           col[1].y,col[1].z,col[1].w, 
							           col[2].y,col[2].z,col[2].w);
				} 
		case 1: switch(j)
				{
				case 0: return matrix3(col[1].x,col[1].z,col[1].w, 
							           col[2].x,col[2].z,col[2].w,
									   col[3].x,col[3].z,col[3].w);									   
				case 1: return matrix3(col[0].x,col[0].z,col[0].w, 
							           col[2].x,col[2].z,col[2].w,
									   col[3].x,col[3].z,col[3].w);									   
				case 2: return matrix3(col[0].x,col[0].z,col[0].w, 
							           col[1].x,col[1].z,col[1].w,
									   col[3].x,col[3].z,col[3].w);
				default:return matrix3(col[0].x,col[0].z,col[0].w, // case 3
							           col[1].x,col[1].z,col[1].w, 
							           col[2].x,col[2].z,col[2].w);
				} 
		case 2: switch(j) 
				{
				case 0: return matrix3(col[1].x,col[1].y,col[1].w, 
							           col[2].x,col[2].y,col[2].w,
									   col[3].x,col[3].y,col[3].w);									   
				case 1: return matrix3(col[0].x,col[0].y,col[0].w, 
							           col[2].x,col[2].y,col[2].w,
									   col[3].x,col[3].y,col[3].w);									   
				case 2: return matrix3(col[0].x,col[0].y,col[0].w, 
							           col[1].x,col[1].y,col[1].w,
									   col[3].x,col[3].y,col[3].w);
				default:return matrix3(col[0].x,col[0].y,col[0].w, // case 3
							           col[1].x,col[1].y,col[1].w, 
							           col[2].x,col[2].y,col[2].w);
				} 
		default: switch(j) // case 3
				{
				case 0: return matrix3(col[1].x,col[1].y,col[1].z, 
							           col[2].x,col[2].y,col[2].z,
									   col[3].x,col[3].y,col[3].z);									   
				case 1: return matrix3(col[0].x,col[0].y,col[0].z, 
							           col[2].x,col[2].y,col[2].z,
									   col[3].x,col[3].y,col[3].z);									   
				case 2: return matrix3(col[0].x,col[0].y,col[0].z, 
							           col[1].x,col[1].y,col[1].z,
									   col[3].x,col[3].y,col[3].z);
				default:return matrix3(col[0].x,col[0].y,col[0].z, // case 3
							           col[1].x,col[1].y,col[1].z, 
							           col[2].x,col[2].y,col[2].z);
				} 
		}
	}
	//! compute the determinant
	float determinant() const
	{	
		return col[0].x*submatrix(0,0).determinant() - col[0].y*submatrix(0,1).determinant() + 
			   col[0].z*submatrix(0,2).determinant() - col[0].w*submatrix(0,3).determinant();		
	}

	//! invert the matrix
	void invert()
	{
		unsigned char i, k;
		float	detA= determinant();
		matrix4 invmat;
		bool    sign= false;

		
		if (!detA) return;						
		for (i=0; i < 4; ++i, sign= !sign)
		for (k=0; k < 4; ++k, sign= !sign) 
		{	
			sign ? invmat[i][k]= -submatrix(i,k).determinant()/detA
			     : invmat[i][k]= submatrix(i,k).determinant()/detA;			
		}
		*this= invmat;
	}	

	//! matrix inversion by the tilde operator
	const matrix4 operator ~(void) const
	{   
		matrix4 m(*this);
	    m.invert(); 	 
	    return m;
	}
};


MATRIX_COMMON_EXTERNAL(4)
