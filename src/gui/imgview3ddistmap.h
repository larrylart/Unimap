////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////


#ifndef _IMAGE_VIEW_3D_DIST_MAP_H
#define _IMAGE_VIEW_3D_DIST_MAP_H

// local defines
#define MAXVERTS 10000
#define SQUARE(x) ((x)*(x))
//#define GRID_W 100
//#define GRID_H 100
// opengl exec lists - starting at 10
#define OPENGL_EXEC_LIST_BUILD_DIST_SURFACE_MAP	15

// button - ids
enum
{
	wxID_BUTTON_3D_SURFACE_GRID		= 3000,
	wxID_GRID_TYPE_SELECT			= 3001,
	wxID_BUTTON_3D_GRAPH_SURFACE_A	= 3002,
	wxID_BUTTON_3D_SURFACE_IMAGE	= 3003,
	wxID_BUTTON_3D_GRAPH_GRID		= 3004,
	wxID_COLOR_3D_CONFIG_A			= 3005
};

#include <vector>
#include <cmath>

// some includes
#include "wx/glcanvas.h"

// local includes
#include "image3dbase.h"

// external classes
class CAstroImage;
class CAstroImgView;
class Vec;
class Mtx;
class CImage3dBase;
class CDistGridViewDialog;
class CConfigMain;
class CUnimapWorker;

// class :: CImgView3DDistMap
/////////////////////////////////////////////////////
class CImgView3DDistMap: public CImage3dBase
{
public:
    CImgView3DDistMap( CDistGridViewDialog* parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = _T("3D Spatial Distortion Map"), int *gl_attrib = NULL);

   ~CImgView3DDistMap( );

	// set surface points
	void SetImageDistGrid( CAstroImage* pImage );
	void SetImageView( );

	void ClearGrid();

	////////////////////
	// TPS dist grid methods
	void CalculateStructures();
	// tps visualization
	void DrawStructures( );
	void DrawDistMapSurface( );


// public data
public:

	CDistGridViewDialog*		m_pBaseFrame;
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

// private methods
private:

    DECLARE_EVENT_TABLE()
};

// class :: CDistGridViewDialog
////////////////////////////////////////////////////////////////////
class CDistGridViewDialog: public wxMiniFrame
{
public:
	CDistGridViewDialog( wxWindow *parent, const wxString& strTitle, CUnimapWorker* pUnimapWorker );
	~CDistGridViewDialog( );

	void SetConfig( CConfigMain* pConfig );

	CImgView3DDistMap*	m_pImgView3D;
	CAstroImage*		m_pAstroImage;
	CAstroImgView*			m_pImageView;

	int m_nSelectId;

	// widgets
	wxChoice*			m_pGridTypeSelect;

	CConfigMain* m_pMainConfig;
	CUnimapWorker* m_pUnimapWorker;

	void SetAstroImage( CAstroImage* pImage );

private:
	void OnToolButton( wxCommandEvent& pEvent );
	void OnToolProcType( wxCommandEvent& pEvent );
	void OnConfig( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif

