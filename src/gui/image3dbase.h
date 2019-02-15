////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _IMAGE_3D_BASE
#define _IMAGE_3D_BASE

// system libs
#include <vector>
#include <cmath>

// wx includes
#include "wx/wxprec.h"

// some includes
#include "wx/glcanvas.h"

// defines
#define DEFAULT_GRAPH_SURFACE_3D_WIDTH				300
#define DEFAULT_GRAPH_SURFACE_3D_HEIGHT				200
#define DEFAULT_GRAPH_SURFACE_3D_DEPTH				120
#define DEFAULT_GRAPH_SURFACE_3D_Z_FACTOR			100
#define DEFAULT_GRAPH_SURFACE_3D_SURFACE_Z_DIV		0.025
#define DEFAULT_GRAPH_SURFACE_3D_GRID_SPACER		15
#define DEFAULT_GRAPH_SURFACE_3D_SURFACE_SPACER		1
// :: grid types
#define DEFAULT_GRAPH_3D_GRID_TYPE_SIMPLE			0	
#define DEFAULT_GRAPH_3D_GRID_TYPE_NICE				1	
// :: calls
#define OPENGL_EXEC_LIST_BUILD_RGB_SURFACE_GRID		1
#define OPENGL_EXEC_LIST_BUILD_RGB_SURFACE_AXIS		2

enum
{
	wxID_3D_STRUCTURE_SURFACE_WIDTH_TEXT		= 7200,
	wxID_3D_STRUCTURE_SURFACE_HEIGHT_TEXT		= 7201,
	wxID_3D_STRUCTURE_GRID_SPACER_TEXT			= 7202
};

// class :: CImgView3DColMap
/////////////////////////////////////////////////////
class CImage3dBase: public wxGLCanvas
{
// public methods
public:
    CImage3dBase( wxWindow *parent, wxWindowID id = wxID_ANY,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize, long style = 0,
					const wxString& name = _T("3D Image View"), int *gl_attrib = NULL);

   ~CImage3dBase( );
	// initialization
	void Init( );
	void SetLight( );
	// texture
	void InitTextureMapping( wxImage& pImage );
	void CloseTextureMapping( );
	
	// pre-drawing/calculations calls
	virtual void CalculateStructures( );

	// main drawing methods
	void SetDrawing( );
	virtual void DrawStructures( );
	void DrawGrid( );
	void DrawAxis( );
	void DrawCursor( );
	void DrawStateText( );

	// elements drawing methods
	void drawSolidCone( GLdouble base, GLdouble height, GLint slices, GLint stacks );
	void drawSolidSphere( GLdouble radius, GLint slices, GLint stacks );
	// font methods
	GLvoid glPrint( GLuint nFont, const char *fmt, ... );
	GLvoid BuildFont( GLuint& nFontId, int nSize=24, int nFontWeight=FW_NORMAL );

// public data
public:
	// :: window size
	int m_nWinWidth;
	int m_nWinHeight;
	// :: mouse
	int m_nMouseX, m_nMouseY;
	int m_bMouseDragging;
	// :: camera
	float m_nCamAlpha, m_nCamBeta, m_nCamZoom;
	float m_nCamX, m_nCamY;
	// :: lights
	int	m_nLightingAmbient;
	int	m_nLightingDiffuse;
	int	m_nLightingSpecular;
	int	m_nLightCoordX;
	int	m_nLightCoordY;
	int	m_nLightCoordZ;
	// :: materials
	int	m_nMaterialAmbient;
	int	m_nMaterialDiffuse;
	int	m_nMaterialSpecular;
	int	m_nMaterialShininess;
	int	m_nMaterialEmission;
	// :: structures
	int		m_nStructureSurfaceWidth;
	int		m_nStructureSurfaceHeight;
	int		m_nStructureSurfaceDepth;
	int		m_nStructureGridSpacer;
	int		m_nStructureSurfaceSpacer;
	float	m_nStructureZFactor;
	int		m_bShowAxis;
//	int		m_bStructureGridLabels;
	int		m_bStructureSurfaceLabels;
	int		m_bShowProjections;
	// :: other vars
	float	m_nStructureZFactorDiv;
	GLuint	m_nTexture;
	GLenum	m_nSurfaceFillMode;
	GLenum	m_nShadeModel;
	wxImage m_pImageMap;
	int		m_nGridType;
	double	m_nScaleToGraph;
	// Base Display List For The Font Set
	GLuint	m_nFontBase;
	GLuint	m_nFontBaseB;	
	// :: state flags
	int		m_bCalculateStructures;
	int		m_bUseLighting;
	int		m_bMapImageToSurface;
	int		m_bImageTestureInitialized;
	int		m_bShowGraphGrid;
	int		m_bShowCursor;
	int		m_bShowStateText;
	int		m_bShowGraphGridLabels;
//	int		m_bStructureSurfaceLabels;
	int		m_bNewGridConfig;
	int		m_bNewGridConfigB;
	int		m_bShowSurface;

// private methods
private:
   // handlers
    void OnPaint( wxPaintEvent& pEvent );
    void OnSize( wxSizeEvent& pEvent );
    void OnEraseBackground( wxEraseEvent& pEvent );
    void OnChar( wxKeyEvent& pEvent );
    void OnMouseEvent( wxMouseEvent& pEvent );
	void OnMouseWheel( wxMouseEvent& pEvent );

    DECLARE_EVENT_TABLE()
};

// class:	CConfig3DDialog
///////////////////////////////////////////////////////
class CConfig3DDialog : public wxDialog
{
public:
	CConfig3DDialog( wxWindow *parent, const wxString& strTitle );
	~CConfig3DDialog( );
	// set object to handle
	void Set3dView( CImage3dBase* p3DView );

// public data
public:
	// ref to main view
	CImage3dBase* m_p3DView;

	// :: lights
	wxSlider*	m_pLightingAmbient;
	wxSlider*	m_pLightingDiffuse;
	wxSlider*	m_pLightingSpecular;
	wxSlider*	m_pLightCoordX;
	wxSlider*	m_pLightCoordY;
	wxSlider*	m_pLightCoordZ;
	// :: materials
	wxSlider*	m_pMaterialAmbient;
	wxSlider*	m_pMaterialDiffuse;
	wxSlider*	m_pMaterialSpecular;
	wxSlider*	m_pMaterialShininess;
	wxSlider*	m_pMaterialEmission;
	// :: structures
	wxTextCtrl*		m_pStructureSurfaceWidth;
	wxTextCtrl*		m_pStructureSurfaceHeight;
	wxTextCtrl*		m_pStructureGridSpacer;
	wxTextCtrl*		m_pStructureSurfaceSpacer;
	wxTextCtrl*		m_pStructureZFactor;
	wxCheckBox*		m_pStructureGridLabels;
	wxCheckBox*		m_pStructureSurfaceLabels;
	wxCheckBox*		m_pShowAxis;
	wxCheckBox*		m_pShowProjections;

	////////////
	// local backup vars
	// :: lights
	int	m_nLightingAmbient;
	int	m_nLightingDiffuse;
	int	m_nLightingSpecular;
	int	m_nLightCoordX;
	int	m_nLightCoordY;
	int	m_nLightCoordZ;
	// :: materials
	int	m_nMaterialAmbient;
	int	m_nMaterialDiffuse;
	int	m_nMaterialSpecular;
	int	m_nMaterialShininess;
	int	m_nMaterialEmission;
	// :: structures
	int		m_nStructureSurfaceWidth;
	int		m_nStructureSurfaceHeight;
	int		m_nStructureSurfaceDepth;
	int		m_nStructureGridSpacer;
	int		m_nStructureSurfaceSpacer;
	float	m_nStructureZFactor;
	int		m_bShowAxis;
	int		m_bStructureGridLabels;
	int		m_bStructureSurfaceLabels;
	int		m_bShowProjections;

// protected methods
protected:
	void OnScrollChange( wxScrollEvent& pEvent );
	void OnCheckChange( wxCommandEvent& pEvent );
	void OnTextChange( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
