/****************************************
 * Quaternion class
 * By Will Perone (billperone@yahoo.com)
 * Original: 12-09-2003  
 * Revised:  27-09-2003
 *           22-11-2003
 *           10-12-2003
 *           15-01-2004
 *           16-04-2004
 *   
 * Dependancies: My 4x4 matrix class
 * 
 * © 2003, This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 *
 * Notes:  
 * if |q|=1 then q is a unit quaternion
 * if q=(0,v) then q is a pure quaternion 
 * if |q|=1 then q conjugate = q inverse
 * if |q|=1 then q= [cos(angle), u*sin(angle)] where u is a unit vector 
 * q and -q represent the same rotation 
 * q*q.conjugate = (q.length_squared, 0) 
 * ln(cos(theta),sin(theta)*v)= ln(e^(theta*v))= (0, theta*v)
 ****************************************/

#pragma once


#include "matrix4.h"
#include "assert.h"


struct quaternion
{
	union {
		struct {
			float    s; //!< the real component
			vector3f v; //!< the imaginary components
		};

		struct { float elem[4]; }; //! the raw elements of the quaternion
	};


	//! ctors
	quaternion() {}
	quaternion(float real, float x, float y, float z): s(real), v(x,y,z) {}
	quaternion(float real, const vector3f &i): s(real), v(i) {}

	//! from 3 euler angles
	quaternion(float theta_z, float theta_y, float theta_x)//float heading, float attitude, float bank) 
	{
		/*float angle, sr, sp, sy, cr, cp, cy;

		angle = bank/2;
		sy = sinf(angle);
		cy = cosf(angle);
		angle = attitude/2;
		sp = sinf(angle);
		cp = cosf(angle);
		angle = heading/2;
		sr = sinf(angle);
		cr = cosf(angle);

		double crcp = cr*cp, srsp = sr*sp;

		v.z = (float)( sr*cp*cy-cr*sp*sy );
		v.y = (float)( cr*sp*cy+sr*cp*sy );
		v.x = (float)( crcp*sy-srsp*cy );
		s = (float)( crcp*cy+srsp*sy ); */

		float cos_z_2 = 0.5*cosf(theta_z);
		float cos_y_2 = 0.5*cosf(theta_y);
		float cos_x_2 = 0.5*cosf(theta_x);

		float sin_z_2 = 0.5*sinf(theta_z);
		float sin_y_2 = 0.5*sinf(theta_y);
		float sin_x_2 = 0.5*sinf(theta_x);

		// and now compute quaternion
		s   = cos_z_2*cos_y_2*cos_x_2 + sin_z_2*sin_y_2*sin_x_2;
		v.x = cos_z_2*cos_y_2*sin_x_2 - sin_z_2*sin_y_2*cos_x_2;
		v.y = cos_z_2*sin_y_2*cos_x_2 + sin_z_2*cos_y_2*sin_x_2;
		v.z = sin_z_2*cos_y_2*cos_x_2 - cos_z_2*sin_y_2*sin_x_2;

	}
	
	//! from 3 euler angles 
	quaternion(const vector3f &angles)
	{	
		float cos_z_2 = 0.5*cosf(angles.z);
		float cos_y_2 = 0.5*cosf(angles.y);
		float cos_x_2 = 0.5*cosf(angles.x);

		float sin_z_2 = 0.5*sinf(angles.z);
		float sin_y_2 = 0.5*sinf(angles.y);
		float sin_x_2 = 0.5*sinf(angles.x);

		// and now compute quaternion
		s   = cos_z_2*cos_y_2*cos_x_2 + sin_z_2*sin_y_2*sin_x_2;
		v.x = cos_z_2*cos_y_2*sin_x_2 - sin_z_2*sin_y_2*cos_x_2;
		v.y = cos_z_2*sin_y_2*cos_x_2 + sin_z_2*cos_y_2*sin_x_2;
		v.z = sin_z_2*cos_y_2*cos_x_2 - cos_z_2*sin_y_2*sin_x_2;

		/*float c1,c2,c3,s1,s2,s3;

		// something strange is going on here
		c1 = cosf(-angles.z / 2);
		s1 = sinf(-angles.z / 2); 
		c2 = cosf(-angles.y / 2); 
		s2 = sinf(-angles.y / 2); 
		c3 = cosf(angles.x / 2); 
		s3 = sinf(angles.x / 2); 

		s= c1*c2*c3 + s1*s2*s3;
		v.x= c1*c2*s3 - s1*s2*c3;
		v.y= c1*s2*c3 + s1*c2*s3;
		v.z= s1*c2*c3 - c1*s2*s3;*/

		/*float angle, sr, sp, sy, cr, cp, cy;

		angle = angles.z/2;
		sy = sinf(angle);
		cy = cosf(angle);
		angle = angles.y/2;
		sp = sinf(angle);
		cp = cosf(angle);
		angle = angles.x/2;
		sr = sinf(angle);
		cr = cosf(angle);

		double crcp = cr*cp, srsp = sr*sp;

		v.z = (float)( sr*cp*cy-cr*sp*sy );
		v.y = (float)( cr*sp*cy+sr*cp*sy );
		v.x = (float)( crcp*sy-srsp*cy );
		s = (float)( crcp*cy+srsp*sy ); */
	} 
		
	//! basic operations
	quaternion &operator =(const quaternion &q)		
	{	s= q.s; v= q.v;	return *this;		}

	const quaternion operator +(const quaternion &q) const	
	{	return quaternion(s+q.s, v+q.v);	}

	const quaternion operator -(const quaternion &q) const	
	{	return quaternion(s-q.s, v-q.v);	}

	const quaternion operator *(const quaternion &q) const	
	{	return quaternion(s*q.s - v*q.v,
						  v.y*q.v.z - v.z*q.v.y + s*q.v.x + v.x*q.s,
						  v.z*q.v.x - v.x*q.v.z + s*q.v.y + v.y*q.s,
						  v.x*q.v.y - v.y*q.v.x + s*q.v.z + v.z*q.s);
	}

	/*const quaternion operator /(const quaternion &q) const
	{
		float denominator = q.length();
       
		// unsure if this is correct 
        return quaternion((s*q.s + v*q.v)/denominator,  
                          (-s*q.v.x + v.x*q.s - v.y*q.v.z + v.z*q.v.y)/denominator,  
                          (-s*q.v.y + v.x*q.v.z + v.y*q.s - v.z*q.v.x)/denominator,  
                          (-s*q.v.z - v.x*q.v.y + v.y*q.v.x + v.z*q.s)/denominator);  
	}*/
	const quaternion operator /(const quaternion &q) const	
	{	
			quaternion p(q); 
			p.invert(); 
			return *this * p;
	}

	const quaternion operator *(float scale) const			
		{	return quaternion(s*scale,v*scale);		}

	const quaternion operator /(float scale) const
		{	return quaternion(s/scale,v/scale);		}

	const quaternion operator -() const						
		{	return quaternion(-s, -v);				}
	
	const quaternion &operator +=(const quaternion &q)		
		{	v+=q.v; s+=q.s; return *this;			}

	const quaternion &operator -=(const quaternion &q)		
		{	v-=q.v; s-=q.s; return *this;			}

	const quaternion &operator *=(const quaternion &q)		
	{	
			s= s*q.s - v*q.v;
			v.x= v.y*q.v.z - v.z*q.v.y + s*q.v.x + v.x*q.s;
			v.y= v.z*q.v.x - v.x*q.v.z + s*q.v.y + v.y*q.s;
			v.z= v.x*q.v.y - v.y*q.v.x + s*q.v.z + v.z*q.s;
		    return *this;		
	}
	
	const quaternion &operator *= (float scale)			
		{	v*=scale; s*=scale; return *this;		}

	const quaternion &operator /= (float scale)			
		{	v/=scale; s/=scale; return *this;		}
	

	//! gets the length of this quaternion
	float length() const
	{	return (float)sqrt(s*s + v*v);   }

	//! gets the squared length of this quaternion
	float length_squared() const
	{	return (float)(s*s + v*v);   }

	//! normalizes this quaternion
	void normalize()
	{	*this/=length();	}

	//! returns the normalized version of this quaternion
	quaternion normalized() const
	{   return  *this/length();  }

	//! computes the conjugate of this quaternion
	void conjugate()
	{	v=-v;   }

	//! inverts this quaternion
	void invert()
	{	conjugate(); *this/=length_squared(); 	}
	
	//! returns the logarithm of a quaternion = v*a where q = [cos(a),v*sin(a)]
	quaternion log() const
	{
		float a = (float)acos(s);
		float sina = (float)sin(a);
		quaternion ret;

		ret.s = 0;
		if (sina > 0)
		{
			ret.v.x = a*v.x/sina;
			ret.v.y = a*v.y/sina;
			ret.v.z = a*v.z/sina;
		} else {
			ret.v.x= ret.v.y= ret.v.z= 0;
		}
		return ret;
	}

	//! returns e^quaternion = exp(v*a) = [cos(a),vsin(a)]
	quaternion exp() const
	{
		float a = (float)v.length();
		float sina = (float)sin(a);
		float cosa = (float)cos(a);
		quaternion ret;

		ret.s = cosa;
		if (a > 0)
		{
			ret.v.x = sina * v.x / a;
			ret.v.y = sina * v.y / a;
			ret.v.z = sina * v.z / a;
		} else {
			ret.v.x = ret.v.y = ret.v.z = 0;
		}
		return ret;
	}

	//! casting to a 4x4 isomorphic matrix for right multiplication with vector
	operator matrix4() const
	{			
		return matrix4(s,  -v.x, -v.y,-v.z,
			           v.x,  s,  -v.z, v.y,
					   v.y, v.z,    s,-v.x,
					   v.z,-v.y,  v.x,   s);
	}

	//! casting to a 4x4 isomorphic matrix for left multiplication with vector
	/*operator matrix4() const
	{			
		return matrix4(   s, v.x, v.y, v.z,
			           -v.x,   s,-v.z, v.y,
					   -v.y, v.z,   s,-v.x,
					   -v.z,-v.y, v.x,   s);
	}*/

	//! casting to 3x3 rotation matrix
	operator matrix3() const
	{
		Assert(length() > 0.9999 && length() < 1.0001, "quaternion is not normalized");		
		return matrix3(1-2*(v.y*v.y+v.z*v.z), 2*(v.x*v.y-s*v.z),   2*(v.x*v.z+s*v.y),   
					   2*(v.x*v.y+s*v.z),   1-2*(v.x*v.x+v.z*v.z), 2*(v.y*v.z-s*v.x),   
					   2*(v.x*v.z-s*v.y),   2*(v.y*v.z+s*v.x),   1-2*(v.x*v.x+v.y*v.y));
	}

	//! computes the dot product of 2 quaternions
	static inline float dot(const quaternion &q1, const quaternion &q2) 
	{   return q1.v*q2.v + q1.s*q2.s;  }

	//! linear quaternion interpolation
	static quaternion lerp(const quaternion &q1, const quaternion &q2, float t) 
	{	return (q1*(1-t) + q2*t).normalized();	}

	//! spherical linear interpolation
	static quaternion slerp(const quaternion &q1, const quaternion &q2, float t) 
	{
		quaternion q3;
		float dot = quaternion::dot(q1, q2);

		/*	dot = cos(theta)
			if (dot < 0), q1 and q2 are more than 90 degrees apart,
			so we can invert one to reduce spinning	*/
		if (dot < 0)
		{
			dot = -dot;
			q3 = -q2;
		} else q3 = q2;
		
		if (dot < 0.95f)
		{
			float angle = acosf(dot);
			return (q1*sinf(angle*(1-t)) + q3*sinf(angle*t))/sinf(angle);
		} else // if the angle is small, use linear interpolation								
			return lerp(q1,q3,t);		
	}

	//! This version of slerp, used by squad, does not check for theta > 90.
	static quaternion slerpNoInvert(const quaternion &q1, const quaternion &q2, float t) 
	{
		float dot = quaternion::dot(q1, q2);

		if (dot > -0.95f && dot < 0.95f)
		{
			float angle = acosf(dot);			
			return (q1*sinf(angle*(1-t)) + q2*sinf(angle*t))/sinf(angle);
		} else  // if the angle is small, use linear interpolation								
			return lerp(q1,q2,t);			
	}

	//! spherical cubic interpolation
	static quaternion squad(const quaternion &q1,const quaternion &q2,const quaternion &a,const quaternion &b,float t)
	{
		quaternion c= slerpNoInvert(q1,q2,t),
			       d= slerpNoInvert(a,b,t);		
		return slerpNoInvert(c,d,2*t*(1-t));
	}

	//! Shoemake-Bezier interpolation using De Castlejau algorithm
	static quaternion bezier(const quaternion &q1,const quaternion &q2,const quaternion &a,const quaternion &b,float t)
	{
		// level 1
		quaternion q11= slerpNoInvert(q1,a,t),
			       q12= slerpNoInvert(a,b,t),
			       q13= slerpNoInvert(b,q2,t);		
		// level 2 and 3
		return slerpNoInvert(slerpNoInvert(q11,q12,t), slerpNoInvert(q12,q13,t), t);
	}

	//! Given 3 quaternions, qn-1,qn and qn+1, calculate a control point to be used in spline interpolation
	static quaternion spline(const quaternion &qnm1,const quaternion &qn,const quaternion &qnp1)
	{
		quaternion qni(qn.s, -qn.v);		
		
		return qn * (( (qni*qnm1).log()+(qni*qnp1).log() )/-4).exp();
	}

	//! converts from a normalized axis - angle pair rotation to a quaternion
	static inline quaternion from_axis_angle(const vector3f &axis, float angle)
	{	return quaternion(cosf(angle/2), axis*sinf(angle/2)); 	}

	//! returns the axis and angle of this unit quaternion
	void to_axis_angle(vector3f &axis, float &angle) const
	{
		angle = acosf(s);

		// pre-compute to save time
		float sinf_theta_inv = 1.0/sinf(angle);

		// now the vector
		axis.x = v.x*sinf_theta_inv;
		axis.y = v.y*sinf_theta_inv;
		axis.z = v.z*sinf_theta_inv;

		// multiply by 2
		angle*=2;
	}

	//! rotates v by this quaternion (quaternion must be unit)
	vector3f rotate(const vector3f &v)
	{   
		quaternion V(0, v);
	    quaternion conjugate(*this);
		conjugate.conjugate();
		return (*this * V * conjugate).v;
	}

	//! returns the euler angles from a rotation quaternion
	vector3f euler_angles() const
	{
		float sqw = s*s;    
		float sqx = v.x*v.x;    
		float sqy = v.y*v.y;    
		float sqz = v.z*v.z;    

		vector3f euler;
		euler.x = atan2f(2.f * (v.x*v.y + v.z*s), sqx - sqy - sqz + sqw);    		
		euler.y = asinf(-2.f * (v.x*v.z - v.y*s));
		euler.z = atan2f(2.f * (v.y*v.z + v.x*s), -sqx - sqy + sqz + sqw);    
		return euler;
	}
};


