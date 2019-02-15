////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#include "wx/wx.h"
#include "designSkin.h"

static CDesignSkin s_Skin;

//  default constructor
CDesignSkin::CDesignSkin( )
{
  // setup colors
#if 0 // orange
  _Dark  = wxColour(0x93, 0x63, 0x05); // #936305
  _Norm  = wxColour(0xe4, 0x99, 0x08); // #e49908
  _Light = wxColour(0xf9, 0xb9, 0x31); // #f9b931
#endif

#if 1 // Bleu
  _Dark  = wxColour(0x11, 0x1D, 0x40); // #111D40
  _Norm  = wxColour(0x0A, 0x24, 0x6A); // #0A246A
  _Light = wxColour(0x00, 0x29, 0x97); // #002997
#endif

  _Background = wxColour(0xD4, 0xD0, 0xC8); // #D4D0C8
  _White = wxColour(0xFF, 0xFF, 0xFF);
  _Black = wxColour(0x00, 0x00, 0x00);

  _LightGrey = wxColour(0xD4, 0xD0, 0xC8);
  _DarkGrey = wxColour(0x9A, 0x95, 0x87);
  _Grey = wxColour(0xD4, 0xD0, 0xC8);
  _Yellow = wxColour(0xFF, 0xFF, 0x00);
  _Red = wxColour(0xFF, 0x00, 0x00);
  _Green = wxColour(0x00, 0xFF, 0x00);
  _Blue = wxColour(0x00, 0x00, 0xFF);

  // setup pens
  _BackgroundPen = wxPen(_Background);
  _DarkPen  = wxPen(_Dark);
  _NormPen  = wxPen(_Norm);
  _LightPen = wxPen(_Light);

  _WhitePen = wxPen(_White);
  _BlackPen = wxPen(_Black);
  _LightGreyPen = wxPen(_LightGrey);
  _GreyPen = wxPen(_Grey);
  _DarkGreyPen = wxPen(_DarkGrey);
  _YellowPen = wxPen(_Yellow);
  _RedPen = wxPen(_Red);
  _GreenPen = wxPen(_Green);
  _BluePen = wxPen(_Blue);

  // setup brush
  _BackgroundBrush = wxBrush(_Background);
  _NormBrush = wxBrush(_Norm);
  _DarkGreyBrush = wxBrush(_DarkGrey);
  _GreyBrush = wxBrush(_Grey);
  _LightGreyBrush = wxBrush(_LightGrey);
  _WhiteBrush = wxBrush(_White);
  _BlackBrush = wxBrush(_Black);

  // textInfo
  _TextInfoBrush = _WhiteBrush;
  _TextInfoPen = _BlackPen;
#if defined __WXMSW__
  _TextInfoFont = wxFont(10, wxMODERN, wxNORMAL, wxNORMAL);
#else
  _TextInfoFont = wxFont(12, wxMODERN, wxNORMAL, wxNORMAL);
#endif

} 

//  destructor
CDesignSkin::~CDesignSkin( )
{
}

// global method to get skin
const CDesignSkin& GetSkin( )
{
  return s_Skin;
}
