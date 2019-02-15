/****************************************
 * Spline Class
 * By Bill Perone (billperone@yahoo.com)
 * Original: 21-10-2003
 * Revised:  05-11-2003
 *
 * This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 *
 * Dependancies: matrix4, vector2 class, STL vector, STL list
 * 
 ****************************************/

#pragma once

#include <vector>
#include <list>
#include "vector2.h"
#include "matrix4.h"

#ifdef WIN32
#pragma warning(disable:4786) // STL expands to beyond 256 chars in windows
#endif


#define ARCLEN_GENERATION_ERROR  0.001f  //!< error for adaptive arclen table generation


template <class Vector>
class Spline
{
public:
    struct ArclenTableEntry
    {
        float    time;   //!< the timestamp
        float    arclen; //!< the arclength at this time (monotonically growing in the table)
        Vector   pos;    //!< the space curve position at this point
    };

    enum SplineMode { Bezier, Hermite, CatMullRom, BSpline }; //!< default mode is bezier

protected:
    SplineMode mode;   //!< the current spline mode
    matrix4    basis;  //!< the current basis matrix

    unsigned int step; //!< the stepping value for control point interpolation

    //! returns nearest index into the arclength table matching arc_val
    inline int ArcTableBisectionSearch(float arc_val) const;

public:
    std::vector<Vector>  ctrlpts; //!< the control points

    typedef std::vector<ArclenTableEntry> ArclenTable;
    ArclenTable arclentable; //!< the arclength table


    void Mode(SplineMode m); //!< set the spline mode

    SplineMode Mode() const  //!< get the current spline mode
    {  return mode;  }

    unsigned int Step() const
    {  return step;  } //!< returns the stepping value

    Spline()
    {  Mode(Bezier);  }

    ~Spline()
    { }

    //! returns the interpolated value of the spline at a time value in a particular dimension
    inline float InterpCtrlPt(float p1, float p2, float p3, float p4, float t);

    //! normalizes the arclength table so that the final arclength = 1
    void  ArclenTableNormalize(void)
    {
        for (unsigned int i= 0; i < arclentable.size(); ++i)
             arclentable[i].arclen/=arclentable.back().arclen;
    }

    //! returns the time on the space curve given a partial arclength value
    float ArcTime(float arc_val) const;

    //! builds the arclength table
    virtual void  ArclenTableBuild()= 0;

    //! returns approximated space curve point given a partial arclength value
    virtual Vector ArcPoint(float arc_val)= 0;
};


//! 1D spline class
class Spline1D: public Spline<float>
{
private:
    std::list<ArclenTableEntry>::iterator InternalBuildArclenTable(std::list<ArclenTableEntry> &templist, std::list<ArclenTableEntry>::iterator curr, unsigned int ctrlpt_offset);

public:
    void  ArclenTableBuild();
    float ArcPoint(float arc_val);
};


//! 2D spline class
class Spline2D: public Spline<vector2f>
{
private:
    std::list<ArclenTableEntry>::iterator InternalBuildArclenTable(std::list<ArclenTableEntry> &templist, std::list<ArclenTableEntry>::iterator curr, unsigned int ctrlpt_offset);

public:
    void  ArclenTableBuild();
    vector2f ArcPoint(float arc_val);
};


// basis matrices

// Bezier Spline
// B= [P(i) P(i+1) P(i+2) P(i+3)
// C0 continuous, if tangents are aligned then C1 continuous
// step = 3
static matrix4 MatBezier(-1,  3, -3, 1,
                          3, -6,  3, 0,
                         -3,  3,  0, 0,
                          1,  0,  0, 0);

// Hermite Spline
// B= [P(i) P(i+1) P'(i) P'(i+1)]
// C0 continuous
// step = 1
static matrix4 MatHermite  ( 2, -2,  1,  1,
                            -3,  3, -2, -1,
                             0,  0,  1,  0,
                             1,  0,  0,  0);

// CatMullRom Spline
// B= [P(i-1) P(i) P(i+1) P(i+2)]
// C0 continuous
// curve guaranteed to go through all control points
// each 4 consecutive control points define a curve (ex: 0123, 1234, 2345...)
// step = 1
static matrix4 MatCatMullRom(-.5f,  1.5f, -1.5f,  .5f,
                                1, -2.5f,     2, -.5f,
                             -.5f,     0,   .5f,    0,
                                0,     1,     0,    0);

// BSpline
// B= [P(i) P(i+1) P(i+2) P(i+3)
// C2 continuous
// each 4 consecutive control points define a curve (ex: 0123, 1234, 2345...)
// step = 1
static matrix4 MatBSpline(-1.f/6,  3.f/6, -3.f/6, 1.f/6,
                           3.f/6, -6.f/6,  3.f/6, 0,
                          -3.f/6,      0,  3.f/6, 0,
                           1.f/6,  4.f/6,  1.f/6, 0);



// returns the interpolated value of a control point in a particular dimension
template <class Vector>
inline float Spline<Vector>::InterpCtrlPt(float p1, float p2, float p3, float p4, float t)
{
    static float t2, t3;

    t2= t*t; // precalculations
    t3= t2*t;

    return (basis[0][0]*t3 + basis[1][0]*t2 + basis[2][0]*t + basis[3][0])*p1 +
            (basis[0][1]*t3 + basis[1][1]*t2 + basis[2][1]*t + basis[3][1])*p2 +
            (basis[0][2]*t3 + basis[1][2]*t2 + basis[2][2]*t + basis[3][2])*p3 +
            (basis[0][3]*t3 + basis[1][3]*t2 + basis[2][3]*t + basis[3][3])*p4;
    /* optimally you want to do something like the following but the ++'s in C are ambiguous in the operation order and won't generate the correct ASM
	   You'd need to write the corresponding assembly code for the following to work.
    ptr= (float *)&basis;
    --ptr;
    return t3*((p1 * *++ptr) + (p2 * *++ptr) + (p3 * *++ptr) + (p4 * *++ptr)) +
            t2*((p1 * *++ptr) + (p2 * *++ptr) + (p3 * *++ptr) + (p4 * *++ptr)) +
            t* ((p1 * *++ptr) + (p2 * *++ptr) + (p3 * *++ptr) + (p4 * *++ptr)) +
               ((p1 * *++ptr) + (p2 * *++ptr) + (p3 * *++ptr) + (p4 * *++ptr));
    */

}


template <class Vector>
inline int Spline<Vector>::ArcTableBisectionSearch(float arc_val) const
{
    unsigned int upper, middle, lower;
    float ascnd;

    lower= 0;
    upper= arclentable.size();
    ascnd= (arclentable.back().arclen >= arclentable.front().arclen);
    while (upper-lower > 1)
    {
        middle= (upper+lower) >> 1;
        if( arc_val >= arclentable[middle].arclen == ascnd ) lower= middle;
                                                        else upper= middle;
    }
    return lower;
}


template <class Vector>
inline void Spline<Vector>::Mode(SplineMode m)
{
    mode= m;
    switch(mode)
    {
    case Bezier:     step= 3; basis= MatBezier; break;
    case Hermite:    step= 1; basis= MatHermite; break;
    case CatMullRom: step= 1; basis= MatCatMullRom; break;
    case BSpline:    step= 1; basis= MatBSpline; break;
    }
}


template <class Vector>
float Spline<Vector>::ArcTime(float arc_val) const
{
    // trivial cases
    if (arc_val==arclentable.front().arclen) return arclentable.front().time;
    else
    if (arc_val==arclentable.back().arclen) return arclentable.back().time;

    unsigned int lower= ArcTableBisectionSearch(arc_val);

    // perform a linear interpolation of 2 nearest time values
    return ( (arclentable[lower+1].arclen-arc_val)*arclentable[lower].time +
              (arc_val-arclentable[lower].arclen)*arclentable[lower+1].time ) /
              (arclentable[lower+1].time - arclentable[lower].time);
}

