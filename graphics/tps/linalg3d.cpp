
/*
 *  One file long C++ library of linear algebra primitives for
 *  simple 3D programs
 *
 *  Copyright (C) 2001-2003 by Jarno Elonen
 *
 *  Permission to use, copy, modify, distribute and sell this software
 *  and its documentation for any purpose is hereby granted without fee,
 *  provided that the above copyright notice appear in all copies and
 *  that both that copyright notice and this permission notice appear
 *  in supporting documentation.  The authors make no representations
 *  about the suitability of this software for any purpose.
 *  It is provided "as is" without express or implied warranty.
 */

#include <cmath>
#include "../../util/defines.h"

#include "linalg3d.h"


 inline Vec operator * ( const float src, const Vec& v ) { Vec tmp( v ); return ( tmp *= src ); }

// =========================================
// 4 x 4 matrix MTX
// =========================================


Mtx::Mtx()
  {
    for ( int i = 0; i < 16; ++i )
      data[ i ] = 0;
    data[ 0 + 0 ] = data[ 4 + 1 ] = data[ 8 + 2 ] = data[ 12 + 3 ] = 1;
  }

  // Returns the transpose of this matrix
Mtx Mtx::transpose() const
  {
    Mtx m;
    int idx = 0;
    for ( int row = 0; row < 4; ++row )
      for ( int col = 0; col < 4; ++col, ++idx )
        m.data[ idx ] = data[ row + ( col * 4 ) ];
    return m;
  }


// Creates an euler rotation matrix (by X-axis)
Mtx rotateX( float ang )
{
  float s = ( float ) sin( Deg2Rad( ang ) );
  float c = ( float ) cos( Deg2Rad( ang ) );

  Mtx m;
  m.data[ 4 + 1 ] = c; m.data[ 4 + 2 ] = -s;
  m.data[ 8 + 1 ] = s; m.data[ 8 + 2 ] = c;
  return m;
}

// Creates an euler rotation matrix (by Y-axis)
Mtx rotateY( float ang )
{
  float s = ( float ) sin( Deg2Rad( ang ) );
  float c = ( float ) cos( Deg2Rad( ang ) );

  Mtx m;
  m.data[ 0 + 0 ] = c; m.data[ 0 + 2 ] = s;
  m.data[ 8 + 0 ] = -s; m.data[ 8 + 2 ] = c;
  return m;
}

// Creates an euler rotation matrix (by Z-axis)
Mtx rotateZ( float ang )
{
  float s = ( float ) sin( Deg2Rad( ang ) );
  float c = ( float ) cos( Deg2Rad( ang ) );

  Mtx m;
  m.data[ 0 + 0 ] = c; m.data[ 0 + 1 ] = -s;
  m.data[ 4 + 0 ] = s; m.data[ 4 + 1 ] = c;
  return m;
}

// Creates an euler rotation matrix (pitch/head/roll (x/y/z))
Mtx rotate( float pitch, float head, float roll )
{
  float sp = ( float ) sin( Deg2Rad( pitch ) );
  float cp = ( float ) cos( Deg2Rad( pitch ) );
  float sh = ( float ) sin( Deg2Rad( head ) );
  float ch = ( float ) cos( Deg2Rad( head ) );
  float sr = ( float ) sin( Deg2Rad( roll ) );
  float cr = ( float ) cos( Deg2Rad( roll ) );

  Mtx m;
  m.data[ 0 + 0 ] = cr * ch - sr * sp * sh;
  m.data[ 0 + 1 ] = -sr * cp;
  m.data[ 0 + 2 ] = cr * sh + sr * sp * ch;

  m.data[ 4 + 0 ] = sr * ch + cr * sp * sh;
  m.data[ 4 + 1 ] = cr * cp;
  m.data[ 4 + 2 ] = sr * sh - cr * sp * ch;

  m.data[ 8 + 0 ] = -cp * sh;
  m.data[ 8 + 1 ] = sp;
  m.data[ 8 + 2 ] = cp * ch;
  return m;
}

// Creates an arbitraty rotation matrix
Mtx makeRotationMatrix( const Vec &dir, const Vec &up )
{
  Vec x = cross( up, dir ), y = cross( dir, x ), z = dir;
  Mtx m;
  m.data[ 0 ] = x.x; m.data[ 1 ] = x.y; m.data[ 2 ] = x.z;
  m.data[ 4 ] = y.x; m.data[ 5 ] = y.y; m.data[ 6 ] = y.z;
  m.data[ 8 ] = z.x; m.data[ 9 ] = z.y; m.data[ 10 ] = z.z;
  return m;
}

// =========================================
// Plane
// =========================================

  // Classifies a point (<0 == back, 0 == on plane, >0 == front)
float Plane::classify( const Vec& pt ) const
  {
    float f = dot( normal, pt ) + d;
    return ( f > -EPSILON && f < EPSILON ) ? 0 : f;
  }


