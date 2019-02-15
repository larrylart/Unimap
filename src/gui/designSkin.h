////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _DESIGN_SKIN_HEADER
#define _DESIGN_SKIN_HEADER

#include <wx/colour.h>
#include <wx/pen.h>
#include <wx/brush.h>
#include <wx/font.h>

class CDesignSkin 
{
public:
	CDesignSkin();
	virtual ~CDesignSkin(); 

	// colors
	const wxColor& GetBackground() const { return _Background; }
	const wxColor& GetDark() const       { return _Dark; }
	const wxColor& GetNorm() const       { return _Norm; }
	const wxColor& GetLight() const      { return _Light; }

	const wxColor& GetBlack() const      { return _Black; }
	const wxColor& GetDarkGrey() const   { return _DarkGrey; }
	const wxColor& GetGrey() const       { return _Grey; }
	const wxColor& GetLightGrey() const  { return _LightGrey; }
	const wxColor& GetWhiteColor() const { return _White; }

	const wxColor& GetYellow() const     { return _Yellow; }
	const wxColor& GetRed() const        { return _Red; }
	const wxColor& GetGreen() const      { return _Green; }
	const wxColor& GetBlue() const       { return _Blue; }

	// pens
	const wxPen& GetBackgroundPen() const{ return _BackgroundPen; }
	const wxPen& GetDarkPen() const      { return _DarkPen; }
	const wxPen& GetNormPen() const      { return _NormPen; }
	const wxPen& GetLightPen() const     { return _LightPen; }

	const wxPen& GetBlackPen() const     { return _BlackPen; }
	const wxPen& GetDarkGreyPen() const  { return _DarkGreyPen; }
	const wxPen& GetGreyPen() const      { return _GreyPen; }
	const wxPen& GetLightGreyPen() const { return _LightGreyPen; }
	const wxPen& GetWhitePen() const     { return _WhitePen; }
	const wxPen& GetRedPen() const       { return _RedPen; }
	const wxPen& GetGreenPen() const     { return _GreenPen; }
	const wxPen& GetBluePen() const      { return _BluePen; }

	const wxPen& GetYellowPen() const    { return _YellowPen; }

	// brushes
	const wxBrush& GetBackgroundBrush() const   { return _BackgroundBrush; }
	const wxBrush& GetNormBrush() const         { return _NormBrush; }
	const wxBrush& GetDarkGreyBrush() const     { return _DarkGreyBrush; }
	const wxBrush& GetGreyBrush() const         { return _GreyBrush; }
	const wxBrush& GetLightGreyBrush() const    { return _LightGreyBrush; }
	const wxBrush& GetBlackBrush() const        { return _BlackBrush; }
	const wxBrush& GetWhiteBrush() const        { return _WhiteBrush; }

	// textInfo
	const wxBrush& GetTextInfoBrush() const { return _TextInfoBrush; }
	const wxFont& GetTextInfoFont() const   { return _TextInfoFont; }
	const wxPen& GetTextInfoPen() const     { return _TextInfoPen; }

private:
	wxColour _Background;
	wxColour _Dark;
	wxColour _Norm;
	wxColour _Light;
	wxColour _White;
	wxColour _LightGrey;
	wxColour _Grey;
	wxColour _DarkGrey;
	wxColour _Black;
	wxColour _Yellow;
	wxColour _Red;
	wxColour _Green;
	wxColour _Blue;

	wxPen _BackgroundPen;
	wxPen _DarkPen;
	wxPen _NormPen;
	wxPen _LightPen;
	wxPen _WhitePen;
	wxPen _BlackPen;
	wxPen _LightGreyPen;
	wxPen _GreyPen;
	wxPen _DarkGreyPen;
	wxPen _YellowPen;
	wxPen _RedPen;
	wxPen _GreenPen;
	wxPen _BluePen;

	wxBrush _BackgroundBrush;
	wxBrush _NormBrush;
	wxBrush _DarkGreyBrush;
	wxBrush _GreyBrush; 
	wxBrush _LightGreyBrush; 
	wxBrush _WhiteBrush;
	wxBrush _BlackBrush;

	// textInfo
	wxFont    _TextInfoFont;
	wxPen     _TextInfoPen;
	wxBrush   _TextInfoBrush;
};

extern const CDesignSkin& GetSkin( );

#endif 
