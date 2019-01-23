#include "intersect.h"
#include <float.h>



//! returns whether the 2 lines intersected
// fix this: This solution is buggy for colinear lines!!
bool intersect(const line2 &l1, const line2 &l2, float *time)
{	
	if (l1.dir.x!=l2.dir.x) 
	{			
		*time= (l1.p.x-l2.p.x)/(l2.dir.x-l1.dir.x);
		return true;
	} else 
	if (l1.dir.y!=l2.dir.y)
	{			
		*time= (l1.p.y-l2.p.y)/(l2.dir.y-l1.dir.y);
		return true;
	} else // check if they are colinear
	if (l1.dir.x) 
	{
		if (l2.p.y==l1.p.y + ( (l2.p.x-l1.p.x)/l1.dir.x )*l1.dir.y)
		{				
			*time= FLT_MAX; // colinear
			return true;
		} 
	} else // we assume that at least dir.z is valid otherwise this would not be a line!		
	if (l2.p.x==l1.p.x + ( (l2.p.y-l1.p.y)/l1.dir.y )*l1.dir.x)
	{
		*time= FLT_MAX;
		return true;				
	} 	

	return false;		
}


//! returns whether the 2 line segments intersected
bool intersect(const lineseg2 &l1, const lineseg2 &l2, float *time)
{
	if (intersect((line2)l1, (line2)l2, time) && *time >= 0 && *time <= 1)
		return true;	
	return false;
}


//! returns whether the 2 lines intersected
// fix this: This solution is buggy for colinear lines!!
bool intersect(const line3 &l1, const line3 &l2, float *time)
{	
	if (l1.dir.x!=l2.dir.x) 
	{			
		*time= (l1.p.x-l2.p.x)/(l2.dir.x-l1.dir.x);
		return true;
	} else 
	if (l1.dir.y!=l2.dir.y)
	{			
		*time= (l1.p.y-l2.p.y)/(l2.dir.y-l1.dir.y);
		return true;
	} else 
	if (l1.dir.z!=l2.dir.z)
	{			
		*time= (l1.p.z-l2.p.z)/(l2.dir.z-l1.dir.z);
		return true;
	} else // check if they are colinear
	if (l1.dir.x) 
	{
		if (l2.p.y==l1.p.y + ( (l2.p.x-l1.p.x)/l1.dir.x )*l1.dir.y &&
			l2.p.z==l1.p.z + ( (l2.p.x-l1.p.x)/l1.dir.x )*l1.dir.z)
		{				
			*time= FLT_MAX; // colinear
			return true;
		} 
	} else 
	if (l1.dir.y) 
	{ 
		if (l2.p.x==l1.p.x + ( (l2.p.y-l1.p.y)/l1.dir.y )*l1.dir.x &&
			l2.p.z==l1.p.z + ( (l2.p.y-l1.p.y)/l1.dir.y )*l1.dir.z)
		{
			*time= FLT_MAX;
			return true;				
		} 
	} else // we assume that at least dir.z is valid otherwise this would not be a line!		
	if (l2.p.x==l1.p.x + ( (l2.p.z-l1.p.z)/l1.dir.z )*l1.dir.x &&
		l2.p.y==l1.p.y + ( (l2.p.z-l1.p.z)/l1.dir.z )*l1.dir.y)
	{
		*time= FLT_MAX;
		return true;				
	} 

	return false;		
}


//! returns whether the 2 line segments intersected
bool intersect(const lineseg3 &l1, const lineseg3 &l2, float *time)
{
	if (intersect((line3)l1, (line3)l2, time) && *time >= 0 && *time <= 1)
		return true;	
	return false;
}


bool intersect(const plane &p, const line3 &l, float *time)
{
	if (p.n*l.dir)
	{
		*time= ( p.n*(p.p-l.p) )/(p.n*l.dir);
		return true;
	} 

	// check if the line is on the plane
	if (!((l.p-p.p)*p.n))
	{
		*time= FLT_MAX;
		return true;
	}
	return false;
}


//! returns whether a plane and a 3d line segment intersected
bool intersect(const plane &p, const lineseg3 &l, float *time)
{
	if (p.n*l.dir)
	{
		*time= ( p.n*(p.p-l.p) )/(p.n*l.dir);
		if (*time >= 0 && *time <= 1) return true;
		return false;
	} 

	// check if the line is on the plane
	if (!((l.p-p.p)*p.n))
	{
		*time= FLT_MAX;
		return true;
	}
	return false;
}