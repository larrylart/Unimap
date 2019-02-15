////////////////////////////////////////////////////////////////////
// Package:		GUI About implementation
// File:		about.cpp
// Purpose:		define about window
//
// Created by:	Larry Lart on 04-Jul-2006
// Updated by:  
//
////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include "wx/wxprec.h"
#include <wx/bitmap.h>
#include <wx/gdicmn.h>
#include "wx/statline.h"
//#include "wx/minifram.h"
//#include "wx/thread.h"

#if defined(__WXMSW__)
    #ifdef wxHAVE_RAW_BITMAP
    #include "wx/rawbmp.h"
    #endif
#endif

#if defined(__WXMAC__) || defined(__WXGTK__)
    #define wxHAVE_RAW_BITMAP
    #include "wx/rawbmp.h"
#endif

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#ifdef __WXX11__
//#include "ronn.xpm"
#endif

#include "about.h"

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CUnimapAbout
// Purpose:	Initialize my about dialog
// Input:	pointer to reference window
// Output:	nothing	
////////////////////////////////////////////////////////////////////
CUnimapAbout::CUnimapAbout(wxWindow *parent)
               : wxDialog(parent, -1,
                          _("About UniMap"),
                          wxDefaultPosition,
                          wxDefaultSize,
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL)
{
// comment gpahic logo for now
    // logo
    wxBitmap bmp(wxBITMAP(IMG_UNIMAPABOUT));
//	bmp.LoadFile( "x10pingabout.bmp", wxBITMAP_TYPE_BMP );
#if !defined(__WXGTK__) && !defined(__WXMOTIF__) && !defined(__WXMAC__)
    bmp.SetMask(new wxMask(bmp, *wxBLUE));
#endif
    wxStaticBitmap *sbmp = new wxStaticBitmap(this, -1, bmp);

    // layout components
	wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    sizer->Add( sbmp, 0, wxCENTRE | wxALL, 10 );
	sizer->Add( new wxStaticLine(this, -1), 0, wxGROW | wxLEFT | wxRIGHT, 5 );

	// major.minor.[.maintenance[.build]]
	wxString strMsg = wxT("UniMap version 0.0.4\n\nCreated by: Larry Lart\nEmail To: i@larryo.org\n");
	sizer->Add( CreateTextSizer( strMsg ), 0, wxCENTRE | wxALL, 10 );

	sizer->Add( new wxStaticLine(this, -1), 0, wxGROW | wxLEFT | wxRIGHT, 5 );
	sizer->Add( CreateButtonSizer(wxOK), 0, wxCENTRE | wxALL, 10 );

	// activate
	SetSizer(sizer);
	SetAutoLayout(TRUE);
	sizer->SetSizeHints(this);
	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

