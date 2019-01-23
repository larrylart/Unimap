/****************************************
 * Various intersection functions
 * By Bill Perone (billperone@yahoo.com)
 * Original: 10-02-2002   
 *
 * © 2003, This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 ****************************************/


#pragma once

#include "line2.h"
#include "line3.h"
#include "plane.h"


//! returns whether the 2 lines intersected
// fix this: This solution is buggy for colinear lines!!
bool intersect(const line2 &l1, const line2 &l2, float *time);

//! returns whether the 2 line segments intersected
bool intersect(const lineseg2 &l1, const lineseg2 &l2, float *time);

//! returns whether the 2 lines intersected
// fix this: This solution is buggy for colinear lines!!
bool intersect(const line3 &l1, const line3 &l2, float *time);

//! returns whether the 2 line segments intersected
bool intersect(const lineseg3 &l1, const lineseg3 &l2, float *time);

//! returns whether a plane and a line intersected
bool intersect(const plane &p, const line3 &l, float *time);

//! returns whether a plane and a 3d line segment intersected
bool intersect(const plane &p, const lineseg3 &l, float *time);


