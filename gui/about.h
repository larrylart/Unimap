////////////////////////////////////////////////////////////////////
// Package:		GUI About definition
// File:		about.h
// Purpose:		define about window
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:  
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _UNIMAP_ABOUT_H 
#define _UNIMAP_ABOUT_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// class:	CUnimapAbout
///////////////////////////////////////////////////////
class CUnimapAbout : public wxDialog
{
// methods
public:
    CUnimapAbout(wxWindow *parent);
};


#endif
