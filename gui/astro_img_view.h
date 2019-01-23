
#ifndef _ASTRO_IMAGE_VIEW_H
#define _ASTRO_IMAGE_VIEW_H

// view types
#define VIEW_TYPE_IMAGE			0
#define VIEW_TYPE_INSTRUMENTS	1
// DEFINE :: solar object orbit types
#define	SOLAR_OBJECT_TYPE_UNKNOWN	0
#define SOLAR_OBJECT_TYPE_ASTEROID	1
#define SOLAR_OBJECT_TYPE_COMET		2
#define SOLAR_OBJECT_TYPE_AES		3
#define SOLAR_OBJECT_TYPE_PLANETS	4
// max orbit points
#define MAX_ORBIT_POINTS_IN_IMAGE	100

// popup menu view
enum
{
	wxID_IMGVIEWSLEWTO = 23900,
	// view with options
	wxID_VIEWWITH,
	wxID_VIEWWITH_GOOGLEONLINE,
	wxID_VIEWWITH_GOOGLEEARTH,
	wxID_VIEWWITH_WORLDWIND,
	wxID_VIEWWITH_SKYMAPORG,
	wxID_VIEW_HIDE_SHOW_OBJ
};

// include svg stuff ... to use latter
#include "wxaggPixelFormatConvertor.h"

// c++ stuff
#include <vector>
using namespace std;

// wxwidgets
#include <wx/bitmap.h>
#include <wx/rawbmp.h>
#include <wx/window.h>
#include <wx/dcmemory.h>

// AGG headers
/*#include "../lib/agg/examples/svg_viewer/agg_svg_parser.h"
#include "agg_path_storage.h"
#include "agg_color_rgba.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p.h"
#include "agg_rendering_buffer.h"
#include "agg_pixfmt_rgb.h"
*/

// local includes
#include "../sky/star.h"
#include "../sky/constellations.h"
#include "dynamic_img_view.h"

// external classes
class CUnimap;
class CAstroImage;
class CSky;
class CStarViewDlg;
class CDistToolDataWindow;
class CDynamicImgView;
class CImageShift;
//class CGUIFrame;
//class CConfigMain;

    /// The conversion between wxWidgets' pixel format and AGG's pixel format
    typedef PixelFormatConvertor<wxNativePixelFormat> PixelFormat;

    /// The wxWidgets "pixel data" type, an accessor to the raw pixels
    typedef wxPixelData<wxBitmap, PixelFormat::wxWidgetsType> PixelData;

// object labels
typedef struct
{
	// position: 0=not set or default
	// 1=right down(default?)
	// 2=right up
	// 3=left up
	unsigned char n_pos;
	// label rectangle
	int x, y, x2, y2;
	// label size
	int w, h;
	// object type/id
	int obj_id;
	int obj_type;

} structObjLabel;

// inline function to speed rearange labels
extern int CAstroImgView_GetObjLabelPos( int id, structObjLabel* vectLabels, unsigned char rpos_mask,
											wxRect o_1_rect, wxRect o_2_rect, wxRect o_3_rect, wxRect o_4_rect  );

// class :: CAstroImgView
////////////////////////////////////////////////////////////////////////
class CAstroImgView : public CDynamicImgView
{
//////////////////////////
// METHODS
public: 
	CAstroImgView( wxWindow *frame, const wxPoint& pos, const wxSize& size );
	virtual ~CAstroImgView( );

	void InitVars( );

	void FreeAll( );
	void FreeData( );

	void FreeTargets( );
	void FreeDetectedStars( );
	void FreeCatStars( );
	void FreeDsoObjects( );
	void FreeRadioSources( );
	void FreeXGammaSources( );
	void FreeSupernovas( );
	void FreeBlackholes( );
	void FreeMStars( );
	void FreeExoplanets( );
	void FreeAsteroids( );
	void FreeComets( );
	void FreeAes( );
	void FreeSolar( );
	void FreeCostellations( );

	void SetImage( CAstroImage* pAstroImage );
	void WriteImage( wxString& strFile, int nWidth, int nHeight, int nType=0 );
	void WriteImageToDc( wxDC& dc, int nWidth, int nHeight );
	void Draw( wxDC& dc, int bSize=0, int bUpdate=1 );

	void SetCostellations( CAstroImage* pAstroImage );

	// set object names
	void	SetStars( CAstroImage* pAstroImage, int nLimit );
	void	SetCatStars( CAstroImage* pAstroImage, int nLimit );
	void	SetDsoObjects( CAstroImage* pAstroImage, int nLimit );
	void	SetRadioSources( CAstroImage* pAstroImage, int nLimit );
	void	SetXGammaSources( CAstroImage* pAstroImage, int nLimit );
	void	SetSupernovas( CAstroImage* pAstroImage, int nLimit );
	void	SetBlackholes( CAstroImage* pAstroImage, int nLimit );
	void	SetMStars( CAstroImage* pAstroImage, int nLimit );
	void	SetExoplanets( CAstroImage* pAstroImage, int nLimit );
	void	SetAsteroids( CAstroImage* pAstroImage, int nLimit );
	void	SetComets( CAstroImage* pAstroImage, int nLimit );
	void	SetAes( CAstroImage* pAstroImage, int nLimit );
	void	SetSolar( CAstroImage* pAstroImage, int nLimit );

	// sky grid
	void	SetSkyGrid( );
	void	DeleteSkyGrid( );

	int GetObjLabelPos( wxDC& dc, int otype, int id, int x, int y );


	////////////////
	// drawing
	int	DrawSkyObjectAndLabel( wxDC& dc, int nStarId, int nMatch, int nType, int objState,
											double nX, double nY, double nA, double nB, 
											double nTheta, wxString strName );

	int	DrawStar( wxDC& dc, int nStarId );
	int	DrawCatStar( wxDC& dc, int nStarId );
	int	DrawTarget( wxDC& dc, int nTargetId );
	int DrawDso( wxDC& dc, int nDsoId );
	int DrawRadioSource( wxDC& dc, int nSourceId );
	int DrawXGammaSource( wxDC& dc, int nSourceId );
	int DrawSupernovas( wxDC& dc, int nSourceId );
	int DrawBlackhole( wxDC& dc, int nSourceId );
	int DrawMStars( wxDC& dc, int nSourceId );
	int DrawExoplanet( wxDC& dc, int nSourceId );
	int DrawAsteroid( wxDC& dc, int nSourceId );
	int DrawSolarObjectOrbit( wxDC& dc, int nOrbitType, int nSourceId );
	int DrawComet( wxDC& dc, int nSourceId );
	int DrawAes( wxDC& dc, int nSourceId );
	int DrawSolar( wxDC& dc, int nSourceId );

	int DrawConstellationsLabel( wxDC& dc );
	int DrawConstellations( wxDC& dc );
	// custom drawing
//	int DrawSvg( wxDC& dc );
	int DrawDynamicObjectPaths( wxDC& dc );
	void DrawSkyGrid( wxDC& dc );
	void DrawDistGrid( wxDC& dc );
	// distance tool
	void DrawDistanceTool( wxDC& dc );

	// object find method
	int FindObjectByString( wxString& strObjToFind, int nObjType );
	// target methods
	int AddTarget( double x, double y, double ra, double dec, double a, double b );

	// overwrite zoom methods to add set status zoom 
	int ZoomOut( int x, int y, int nFlag );
	int ZoomIn( int x, int y, int nFlag );

private:
	// menu handlers
	void OnMouseMove( wxMouseEvent& pEvent );
	virtual void OnMouseLeave( wxMouseEvent& pEvent );
	virtual void OnRightClick( wxMouseEvent& pEvent );
	void OnMouseRDClick( wxMouseEvent& pEvent ){ return; };
	void OnViewWithGoogleOnline( wxCommandEvent& pEvent );
	void OnViewWithSkyMapOrg( wxCommandEvent& pEvent );
	void OnViewWithApp( wxCommandEvent& pEvent );
	void OnSlewToObject( wxCommandEvent& pEvent );
	void OnShowHideObject( wxCommandEvent& pEvent );
//	void OnShowObjectData( wxCommandEvent& pEvent );

	// mouse handlers
	void OnLeftClickDown( wxMouseEvent& pEvent );
	void OnLeftClickUp( wxMouseEvent& pEvent );
	void OnMouseLDClick( wxMouseEvent& pEvent );
	// create right click menu
	virtual wxMenu* CreatePopupMenu( );

	// area handlers
//	void OnAddArea( wxCommandEvent& pEvent );
//	void OnSetArea( wxCommandEvent& pEvent );
	void OnEditArea( wxCommandEvent& pEvent );
	void OnDeleteArea( wxCommandEvent& pEvent );
//	void OnActivateArea( wxCommandEvent& pEvent );
//	void OnZoomToArea( wxCommandEvent& pEvent );
//	void OnDeactivateArea( wxCommandEvent& pEvent );

// protected data types
protected:

//////////////////////////
// DATA
public:

	// constellation artwork struct
	typedef struct
	{
		wxImage* image;
		long x1,y1,x2,y2;
		double ra1,dec1,ra2,dec2;
		double sx1,sy1,sx2,sy2;
		double r2p_ratio;
		double con_ang_rad;
		int width, height;
		double w2pic, h2pic;

	} structConstArtworkView;
	// orbit point
	typedef struct
	{
		double ra;
		double dec;
		double orb_time;

		double x;
		double y;

	} ORBITPOINT;
	// soler orbit type
	typedef vector <ORBITPOINT> SOLARORBIT;

	// NEW :: CONSTELLATIONS LOCAL DATA
	CSkyConstellations*			m_pConstellations;
	structConstArtworkView		m_vectArtworkFile[100];
	ImgConstellationStructure*	m_vectConstellations;
	int m_nConstellations;

	CLogger*		m_pLogger;
	CUnimap*		m_pUnimap;
//	CConfigMain*	m_pMainConfig;
//	CGUIFrame*		m_pFrame;

	CAstroImage*	m_pAstroImage;
	CSky*			m_pSky;
	CStarViewDlg*	m_pObjectView;

	int m_bImageAltered;

	// sky grid data
	wxPoint** m_vectPrevLine;
	char** m_vectFlagInPic;
	int m_nRaUnits;
	int m_nDecUnits;
	int m_bShowSkyGrid;
	int m_bIsDistGrid;

	/////////////
	// flag if to draw star or not
	int			m_bDrawObjects;
	int			m_nStarDisplayLimit;
	int			m_nCatStarDisplayLimit;
	int			m_nCatDsoDisplayLimit;
	int			m_nCatRadioDisplayLimit;
	int			m_nCatXGammaDisplayLimit;
	int			m_nCatSupernovasDisplayLimit;
	int			m_nCatBlackholesDisplayLimit;
	int			m_nCatMStarsDisplayLimit;
	int			m_nCatExoplanetsDisplayLimit;
	int			m_nCatAsteroidsDisplayLimit;
	int			m_nCatCometsDisplayLimit;
	int			m_nCatAesDisplayLimit;
	int			m_nSolarDisplayLimit;

	// catalog stars
//	int	m_bDrawCatStar;
	// areas
//	int	m_bDrawAreas;

	// labels :: stars
	structObjLabel*	m_vectStarLabels;
	int	m_nStarLabels;
	// labels :: cat stars
	structObjLabel*	m_vectCatStarLabels;
	int	m_nCatStarLabels;
	// labels :: dso
	structObjLabel*	m_vectDsoLabels;
	int	m_nDsoLabels;

	// new dynamic allocation
	// :: stars
	int*		m_vectStarState;
	wxArrayString	m_vectStarName;
	int			m_nStar;
	// :: dso
	wxChar**		m_vectDsoName;
	int			m_nDso;
	// star cat names
	wxArrayString	m_vectCatStarName;
	int			m_nCatStar;
	// :: radio
	wxChar**		m_vectRadioName;
	int			m_nRadio;
	// :: x-ray/gamma
	wxChar**		m_vectXGammaName;
	int			m_nXGamma;
	// :: supernovas
	wxChar**		m_vectSupernovasName;
	int			m_nSupernovas;
	// :: blackholes
	wxChar**		m_vectBlackholesName;
	int			m_nBlackholes;
	// :: multiple/double stars
	wxChar**		m_vectMStarsName;
	int			m_nMStars;
	// :: extra-solar planets
	wxChar**		m_vectExoplanetsName;
	int			m_nExoplanets;
	// :: asteroids + orbits
	wxChar**			m_vectAsteroidsName;
	int				m_nAsteroids;
	SOLARORBIT*		m_vectAsteroidsOrbit;
	// :: comets + orbits
	wxChar**			m_vectCometsName;
	int				m_nComets;
	SOLARORBIT*		m_vectCometsOrbit;
	// :: satellites + orbits
	wxChar**			m_vectAesName;
	int				m_nAes;
	SOLARORBIT*		m_vectAesOrbit;
	// :: solar planets/sun/moons + orbits
	wxChar**			m_vectSolarName;
	int				m_nSolar;
	SOLARORBIT*		m_vectSolarOrbit;

	// target vector
	StarDef*	m_vectTarget;
	int			m_nTarget;
	int			m_nTargetAllocated;

	///////////////
	// distance tool
	CDistToolDataWindow* m_pDistTool;
	int m_bIsDistanceTool;
	// flags
	int m_bIsDistanceToolDragA;
	int m_bIsDistanceToolDragB;
	long m_nLastSelectedA;
	long m_nLastSelectedB;
	long m_nLastOverObject;
	int m_nLastOverObjectType;
	unsigned char m_nLastSelectedTypeA;
	unsigned char m_nLastSelectedTypeB;

	/////////
	// other flags
//	int m_bShowConstellations;
	int m_bShowDynamicObjPaths;

	// colours, pens and bruses
	wxColour	wxDefaultAreaCol;
	wxColour	wxSelectedAreaCol;
	wxColour	wxResizeAreaCol;
	wxColour	wxSetAreaCol;
	wxPen		wxDefaultAreaPen;
	wxPen		wxSelectedAreaPen;
	wxPen		wxResizeAreaPen;
	wxPen		wxSetAreaPen;
	wxBrush		wxDefaultAreaBrush;
	// image area states
	int m_nPrevAreaState;

//////////////////////////
// CAMERA VIEW DATA
public:
	int m_bMainTarget;
	CImageShift* m_pProcImgShift;

	// view type
	int m_nViewType;

	// location selected
	double m_nLocX;
	double m_nLocY;
	// track point
	double m_nTrackX;
	double m_nTrackY;

	int bIsLocSet;
	int m_bIsCamera;
	int m_bIsCameraVideo;

/////////////////////////
// CAMERA VIEW PROTECTED METHODS ??? 
protected:
	void DrawCam( wxDC& dc, int bSize, int bUpdate );
	void OnCamMouseLDClick( int x, int y );

protected:
	DECLARE_EVENT_TABLE()
};

#endif
