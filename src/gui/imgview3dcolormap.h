////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////


#ifndef _IMAGE_VIEW_3D_COL_MAP_H
#define _IMAGE_VIEW_3D_COL_MAP_H

// local defines
// :: color profiler types
#define COLOR_3D_PROFILER_TYPE_NORMAL			0
#define COLOR_3D_PROFILER_TYPE_FFT				1
#define COLOR_3D_PROFILER_TYPE_VIGNETTING		2
#define COLOR_3D_PROFILER_TYPE_3D_DISTRIBUTION	3
// :: color channel select
#define COLOR_3D_CHANNEL_RGB	0
#define COLOR_3D_CHANNEL_RED	1
#define COLOR_3D_CHANNEL_GREEN	2
#define COLOR_3D_CHANNEL_BLUE	3
// opengl exec lists - starting at 10
#define OPENGL_EXEC_LIST_BUILD_RGB_SURFACE_MAP	10
#define OPENGL_EXEC_LIST_BUILD_RGB_CUBE			11
#define OPENGL_EXEC_LIST_BUILD_RGB_CLOUD		12

// button - ids
enum
{
	wxID_BUTTON_3D_SURFACE_GRID_B		= 4000,
	wxID_COLOR_3D_GRAPH_TYPE_SELECT		= 4001,
	wxID_BUTTON_3D_GRAPH_SURFACE		= 4002,
	wxID_BUTTON_3D_SURFACE_IMAGE_B		= 4003,
	wxID_BUTTON_3D_GRAPH_GRID_B			= 4004,
	wxID_COLOR_3D_CHANNEL_TYPE_SELECT	= 4005,
	wxID_COLOR_3D_CONFIG					= 4006,
	wxID_BUTTON_3D_SURFACE_IMAGE_INVERSE	= 4007
};

#include <vector>
#include <cmath>

// some includes
#include "wx/glcanvas.h"
#include <wx/minifram.h>

// local includes
#include "image3dbase.h"

// external classes
class CAstroImage;
class CAstroImgView;
class CImage3dBase;
class CColorMapViewDialog;
class CConfigMain;
class CUnimapWorker;

// structures
typedef struct
{
	double x;
	double y;
	double z;

} Point3D;

// class :: CImgView3DColMap
/////////////////////////////////////////////////////
class CImgView3DColMap: public CImage3dBase
{
public:
	CImgView3DColMap( CColorMapViewDialog *parent, wxWindowID id = wxID_ANY,
						const wxPoint& pos = wxDefaultPosition,
						const wxSize& size = wxDefaultSize, long style = 0,
						const wxString& name = _T("3D ColorMap"), int *gl_attrib = NULL);

	~CImgView3DColMap( );

	void DrawGrid( );
	// set surface points
	void SetImageColorMap( CAstroImage* pAstroImage, wxImage& pImage, int nType, int nChannel, int bInverse=0 );
	void SetImageView();

	void SetImageFFT( CAstroImage* pAstroImage, wxImage& pImage, int nType, int nChannel );
	void SetImageColorCube( CAstroImage* pAstroImage, wxImage& pImage, int nType, int nChannel );
	// draws
	void DrawColorMapSurface( );
	// draw structures
	void DrawStructures( );
	// color rgb cube/cloud
	void BuildColorCloud( wxImage& pImage );
	void BuildRGBCube( );

// public data
public:
	CColorMapViewDialog*		m_pBaseFrame;

	// color map surface points
	std::vector <Point3D> m_vectPoints;

	double m_nMinY;
	double m_nMaxY;
	int m_nViewType;

// private methods
private:

    DECLARE_EVENT_TABLE()
};

// class :: CColorMapViewDialog
////////////////////////////////////////////////////////////////////
class CColorMapViewDialog: public wxMiniFrame
{
public:
	CConfigMain* m_pMainConfig;
	CUnimapWorker* m_pUnimapWorker;

	CColorMapViewDialog( wxWindow *parent, const wxString& strTitle, CUnimapWorker* pUnimapWorker );
	~CColorMapViewDialog( );

	void SetConfig( CConfigMain* pConfig );

	CImgView3DColMap*	m_pImgView3D;
	CAstroImage*		m_pAstroImage;
	CAstroImgView*		m_pImageView;
	// option ids selected
	int m_nSelectId;
	int m_nColorId;
	// widgets
	wxChoice*		m_pGraphTypeSelect;
	wxChoice*		m_pColorTypeSelect;

	void SetAstroImage( CAstroImage* pImage );

private:
	void OnToolButton( wxCommandEvent& pEvent );
	void OnToolProcType( wxCommandEvent& pEvent );
	void OnConfig( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};


#endif

