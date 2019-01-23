
#ifndef _GEOMETRY_H
#define _GEOMETRY_H

// global data
extern float g_vectCtg[40];
extern float g_vectStg[40];

// init geometry data
void InitGeometryData();

// functions
extern bool IsEllipseInRect( double x, double y, double a, double b, double ang, int rx, int ry, int rw, int rh );
extern bool GetEllipseImageQuadrant( double x, double y, double a, double b, double ang,
								int width, int height, int wdiv, int hdiv, unsigned char& x_a, unsigned char& y_a );

bool lineIntersection( double Ax, double Ay, double Bx, double By, double Cx, double Cy, double Dx, double Dy, double *X, double *Y );
int IsLineIntersectRect( int x1, int y1, int x2, int y2, int rx, int ry, int width, int height, int& ox1, int& oy1, int& ox2, int& oy2 );
int IsLineIntersectRectZeroAx( int x1, int y1, int x2, int y2, int width, int height, int& ox1, int& oy1, int& ox2, int& oy2 );

int lines_intersect( long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4, long* x, long* y );

#endif
