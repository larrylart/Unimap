////////////////////////////////////////////////////////////////////
// Name:		Dynamic ImageView header
// File:		dynamic_img_view.h
// Purpose:		interface for the CImgView class.
//
// Created by:	Larry Lart on 04/07/2006
// Updated by:	Larry Lart on 25/02/2010
//
////////////////////////////////////////////////////////////////////

#ifndef _DYNAMIC_IMAGE_VIEW_H
#define _DYNAMIC_IMAGE_VIEW_H

//#include "cv.h"
//#include "highgui.h"
//#include "cvaux.h"

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/thread.h>
#include <wx/image.h>

// ELX
#include <elx/image/ImageVariant.h>
#include <elx/image/ImageImpl.h>
#include <elx/image/Pixels.h>

// local headers
#include "../config/mainconfig.h"
#include "../image/imagearea.h"
#include "../unimap.h"

// local defines
//#define TARGET_OBJ_RECT_LEFTUP		1
//#define TARGET_OBJ_RECT_LEFTDOWN	2
//#define TARGET_OBJ_RECT_RIGHTDOWN	3
// rectangles and circles defaults
//#define MAX_NO_RECTANGLES			40
//#define	MAX_NO_CIRCLES				40

// define tool type
#define IN_USE_TOOL_DEFAULT			0
#define IN_USE_TOOL_ZOOM			1
#define IN_USE_TOOL_ADD_AREA		2

// obj shapes types
#define SHAPE_TYPE_REAL_ELLIPSE		0
#define SHAPE_TYPE_CIRCLE_ICON		1
#define SHAPE_TYPE_SQUARE_ICON		2
#define SHAPE_TYPE_TRIANGLE_ICON	3
#define SHAPE_TYPE_CROSS_ICON		4
#define SHAPE_TYPE_TARGET_ICON		5
// obj shape line style
#define SHAPE_LINE_STYLE_NORMAL		0
#define SHAPE_LINE_STYLE_DOTTED		1
#define SHAPE_LINE_STYLE_NONE		2

// area actions
#define IMAGE_AREA_STATE_JUST_ADDED	1
#define IMAGE_AREA_STATE_RESIZING	2
#define	IMAGE_AREA_STATE_ACTIVE		3
#define	IMAGE_AREA_STATE_SET		4
#define	IMAGE_AREA_STATE_EDIT		5
// area coners
#define AREA_CORNER_TOP_LEFT		1
#define AREA_CORNER_TOP_RIGHT		2
#define AREA_CORNER_BOTTOM_RIGHT	3
#define AREA_CORNER_BOTTOM_LEFT		4
// target stuff
#define SKY_TARGET_BLOCK_SIZE_ALLOC	20

// popup menu view
enum
{
	wxID_IMGVIEWADDAREA			= 22900,
	wxID_IMGVIEWACTIVATEAREA,
	wxID_IMGVIEWEDITAREA,
	wxID_IMGVIEWDELAREA,
	wxID_IMGVIEWZOOMTOAREA,
	wxID_IMGVIEWSETAREA,
	wxID_IMGVIEWDEACTIVATEAREA,
	wxID_IMGVIEWPROPERTY,
	wxID_VIEW_IMGNOTE,
	wxID_IMGAREA,
};

// external classes
//class CCamera;
//class CColorEdit;
class CLogger;
class CGUIFrame;
class CPointArray;

using namespace eLynx;
using namespace eLynx::Image;


// class :: CDynamicImgView
////////////////////////////////////////////////////////////////////////
class CDynamicImgView : public wxWindow
{

public: 
	CDynamicImgView( wxWindow *frame, const wxPoint& pos, const wxSize& size );
	virtual ~CDynamicImgView( );

	void SetTCursor( int nId=UNIMAP_CURSOR_IMAGE_DEFAULT );

	void FreeImage( );
	void ResetData( );

	int IsInPicture( int nX, int nY );

	// set astro image // was virtual
	void SetWxImage( wxImage* pImage, int bRef=0, int bRepos=1 );
	void SetImageFromFile( const wxString& strFile );
	void ProcessInputImage( int bRepos=1 );
	// set image cut
	void SetImageCut( int x, int y, int w, int h );
	// virtual draw
	virtual void Draw( wxDC& dc, int bSize=0, int bUpdate=1 );
	// drawing methods
	void DrawArea( wxDC& dc, DefImageArea& area, int nType=0 );
	void DrawEllips( wxDC& dc, double x, double y, double a,
								double b, double theta, int dotflag );
	void DrawPath( wxDC& dc, int nPoints, wxPoint vectPoints[], int nType );

	void UpdateImage( );
	// handle all drags and decide what to drag
	void DragOn( int x, int y );
	// call to drag image
	int DragImage( int nShiftX, int nShiftY );
	void DrawDragArea( wxDC& dc );
	// zoom handlers
	void ResetZoom( );
	int ZoomToArea( int x, int y, int w, int h );
	virtual int ZoomOut( int x, int y, int nFlag );
	virtual int ZoomIn( int x, int y, int nFlag );
	// virtual handler
	virtual void UpdateOnImageCut( ){ };

	virtual void OnLeftClickUp( wxMouseEvent& pEvent );
	virtual void OnRightClick( wxMouseEvent& pEvent );

	// copy dc properties
	bool SetGdcPropsFromDc( wxDC& dc );

	//////////////////
	// elx library image handlers - temp until I migrate the data
	ImageVariant GetElxBitmap( );
	void SetElxBitmap( ImageVariant* pElxImage );
	ImageVariant GetElxImage( );
	void SetElxImage( ImageVariant* pElxImage );

private:
	
	// event handlers
	void OnPaint(wxPaintEvent& pEvent);
	virtual void OnMouseLDClick( wxMouseEvent& pEvent ) {return;};
	void	OnMouseWheel( wxMouseEvent& pEvent );
	virtual void OnMouseMove( wxMouseEvent& pEvent );
	virtual void OnMouseEnter( wxMouseEvent& pEvent ){ SetFocus(); }
	virtual void OnLeftClickDown( wxMouseEvent& pEvent ){ pEvent.Skip(); return; };
	virtual void OnSize( wxSizeEvent& even );
	virtual void OnMouseRDClick( wxMouseEvent& pEvent );
	// key  events
	virtual void OnKeyDown( wxKeyEvent& pEvent );
	virtual void OnKeyUp( wxKeyEvent& pEvent );

	// create right click menu
	virtual wxMenu* CreatePopupMenu( ){ return(NULL); }

// Public data
public:

	wxGraphicsContext* m_dc;

	wxMutex* s_mutexViewBmp;

//	CLogger*		m_pLogger;
	CUnimap*		m_pUnimap;
	CGUIFrame*		m_pFrame;
	CConfigMain*	m_pMainConfig;

	int m_nInUseTool;
	// flags
	int m_bZoomDirection;

	// math data to use
//	float m_vectCtg[40];
//	float m_vectStg[40];

	// image data
	wxImage*		m_pImage;
	unsigned char	m_bSharedImage;
	//unsigned char	m_bCutUpdated;

	int m_nWidth;
	int m_nHeight;

	double m_nPicOrigX;
	double m_nPicOrigY;
	double m_nPicEndX;
	double m_nPicEndY;

	double m_nBasePicOrigX;
	double m_nBasePicOrigY;

	double m_nPicWidth;
	double m_nPicHeight;

	double m_nBasePicWidth;
	double m_nBasePicHeight;
	// main copy of my original picture
	double m_nOrigPicWidth;
	double m_nOrigPicHeight;

	double m_nOrigPicWinRatio;
	double m_nCutWinRatio;
//	double m_nWidthCutWinRatio;
	double m_nCutX;
	double m_nCutY;
	double m_nCutWidth;
	double m_nCutHeight;

	int m_nZoomX;
	int m_nZoomY;
	// test
	double m_nLastZoomX;
	double m_nLastZoomY;
	double nLastNX;
	double nLastNY;
	double nOrigNX;
	double nOrigNY;

	// test
	double m_nLastZoomFactor;
	double m_nLastXMarg;
	double m_nLastYMarg;
	double m_nBasePicOrigXShift;
	double m_nBasePicOrigYShift;

	double m_nZoomCount;

	double m_nScaleFactor;

	// basic menu - for cases when I need this ???
//	wxMenu*	m_pImgViewMenu;

	////////////////////////////////////
	//  MOUSE COORD DATA
	// to use set mouse position on menu options
	int m_nCurrentMouseX;
	int m_nCurrentMouseY;
	// right click in piture position - to keep for menus, notes etc
	int m_nPicRightClickPosX;
	int m_nPicRightClickPosY;
	// curent position in window
	int m_nMouseWinX;
	int m_nMouseWinY;
	// current position in pic in window - margins
	int m_nMouseWinPicX;
	int m_nMouseWinPicY;
	// picture corespondece for mouse position
	double m_nMousePicX;
	double m_nMousePicY;

	// last drag position
	int m_nDragLastX;
	int m_nDragLastY;
	// larry add drag start pos
	int m_nStartDragX;
	int m_nStartDragY;
	// last drag difference
	int m_nDragDiffX;
	int m_nDragDiffY;

	// colours, pens and bruses
	wxColour	wxDefaultCol;
	wxPen		wxDefaultPen;
	wxBrush		wxDefaultBrush;

	int		m_bDragging;
	int		m_bDragWithZoomTool;
	int		m_bHasImage;

	///////////////////////
	// AREA :: selection
	int m_nAreaSelected;
	int m_nAreaEdit;
	int m_bAreaMove;
	// resize vertex
	int m_AreaResizeVertex;

	// current/last cursor id
	int m_nCurrentCursorId;
	int m_nLastCursorId;

// Protected data
protected:
	wxBitmap	m_pBitmap;

  	bool	m_bDrawing;
	bool	m_bNewImage;

	wxWindow* m_pParent;

	int m_nOldWidth;
	int m_nOldHeight;
	int m_nOldX;
	int m_nOldY;

// protected data
protected:

	DECLARE_EVENT_TABLE()
};

#endif

