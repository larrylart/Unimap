

#ifndef _IMAGE_VIEW_3D_SPACE_DIST_H
#define _IMAGE_VIEW_3D_SPACE_DIST_H

// local defines
#define MAXVERTS 10000
#define SQUARE(x) ((x)*(x))
// processing/view types
#define PROC_3D_SPACE_DIST_GALACTIC_PLANE			0
#define PROC_3D_SPACE_DIST_EXTRA_GALACTIC			1

// opengl exec lists - starting at 10
#define OPENGL_EXEC_LIST_BUILD_SPACE_DIST_SURFACE	16

// button - ids
enum
{
	wxID_BUTTON_3D_SURFACE_GRID_C	= 8000,
	wxID_GRID_TYPE_SELECT_C			= 8001,
	wxID_BUTTON_3D_GRAPH_SURFACE_C	= 8002,
	wxID_BUTTON_3D_SURFACE_IMAGE_C	= 8003,
	wxID_BUTTON_3D_GRAPH_GRID_C		= 8004,
	wxID_COLOR_3D_CONFIG_C			= 8005
};

#include <vector>
#include <cmath>

// some includes
#include "wx/glcanvas.h"

// external classes
class CAstroImage;
class CAstroImgView;
class Vec;
class Mtx;
class CImage3dBase;
class CSpaceDistViewDialog;
class CConfigMain;
class CUnimapWorker;

// class :: CImgView3DSpaceDist
/////////////////////////////////////////////////////
class CImgView3DSpaceDist: public CImage3dBase
{
public:
    CImgView3DSpaceDist( CSpaceDistViewDialog* parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("3D Spatial Distortion Map"), int *gl_attrib = NULL);

   ~CImgView3DSpaceDist( );

	// set surface points
	void SetImageSpaceDist( CAstroImage* pImage, int nType );
	void SetImageView( );

	void ClearGrid();

	////////////////////
	// TPS dist grid methods
	void CalculateStructures();
	// tps visualization
	void DrawStructures( );
	void DrawSpaceDistSurface( );


// public data
public:

	CSpaceDistViewDialog*		m_pBaseFrame;
	// grid data
	float** m_vectGrid;

	//////////////////////
	// dist grid TPS data
	std::vector <Vec> m_vectControlPoints;
	int m_nControlPoints;

	int m_nSelectedCtrlPoint;
	double m_nRegularization;
	double m_nBendingEnergy;
	// tps visualization
	int m_nModifiers;
	Vec m_vCursorLocation;

	double m_nMinY;
	double m_nMaxY;
	double m_nMinDepth;
	double m_nMaxDepth;

	int m_nViewType;

// private methods
private:

    DECLARE_EVENT_TABLE()
};

// class :: CSpaceDistViewDialog
////////////////////////////////////////////////////////////////////
class CSpaceDistViewDialog: public wxMiniFrame
{
public:
	CSpaceDistViewDialog( wxWindow *parent, const wxString& strTitle, CUnimapWorker* pUnimapWorker );
	~CSpaceDistViewDialog( );

	void SetConfig( CConfigMain* pConfig );

	CImgView3DSpaceDist*	m_pImgView3D;
	CAstroImage*			m_pAstroImage;
	CAstroImgView*			m_pImageView;
	// widgets
	wxChoice*			m_pGridTypeSelect;

	CConfigMain* m_pMainConfig;
	CUnimapWorker* m_pUnimapWorker;

	int m_nSelectId;

	void SetAstroImage( CAstroImage* pImage );

private:
	void OnToolButton( wxCommandEvent& pEvent );
	void OnToolProcType( wxCommandEvent& pEvent );
	void OnConfig( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif

