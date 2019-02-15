////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _RADIAL_PROFILE_H
#define _RADIAL_PROFILE_H

#include "wx/wx.h"

void getObjectRadialProfile( wxImage& rImage, double x, double y, double nA, double nB, double ang_deg, double* vectRing, unsigned int& nRingNo );

#endif
