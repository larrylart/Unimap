
#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// system libs
#include <vector>
#include <cmath>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// wx includes
#include "wx/wxprec.h"
#include <wx/dialog.h>
#include "wx/statline.h"
#include "wx/minifram.h"
#include "wx/thread.h"
#include "wx/notebook.h"
#include <wx/fontdlg.h>
#include <wx/colordlg.h>

// opengl
#if defined(__WXMAC__) || defined(__WXCOCOA__)
#   ifdef __DARWIN__
#       include <OpenGL/gl.h>
#       include <OpenGL/glu.h>
#   else
#       include <gl.h>
#       include <glu.h>
#   endif
#else
#   include <GL/gl.h>
#   include <GL/glu.h>
//#	include <GL/glaux.h>
#endif

// local includes
#include "../graphics/tps/linalg3d.h"
#include "../util/func.h"
#include "../image/astroimage.h"
#include "../gui/astro_img_view.h"

// main header
#include "image3dbase.h"

// class :: CImage3dBase
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CImage3dBase, wxGLCanvas )
    EVT_SIZE( CImage3dBase::OnSize )
    EVT_PAINT( CImage3dBase::OnPaint )
    EVT_CHAR( CImage3dBase::OnChar)
    EVT_MOUSE_EVENTS( CImage3dBase::OnMouseEvent )
	EVT_MOUSEWHEEL( CImage3dBase::OnMouseWheel )
    EVT_ERASE_BACKGROUND( CImage3dBase::OnEraseBackground )
END_EVENT_TABLE()
/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CImage3dBase
// Purpose:	intialize my object
// Input:	parent, id, pos, size, style, title, gl attributes
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CImage3dBase::CImage3dBase( wxWindow *parent, wxWindowID id,
							const wxPoint& pos, const wxSize& size, long style,
							const wxString& name, int* gl_attrib )
							: wxGLCanvas(parent, id, pos, size, 
							style|wxFULL_REPAINT_ON_RESIZE|wxWANTS_CHARS, name, gl_attrib )
{
	// :: get window size
    int w, h;
    GetClientSize(&w, &h);
	// window sizes
	m_nWinWidth = w;
	m_nWinHeight = h;

	// init surface default size
	m_nStructureSurfaceWidth = DEFAULT_GRAPH_SURFACE_3D_WIDTH;
	m_nStructureSurfaceHeight = DEFAULT_GRAPH_SURFACE_3D_HEIGHT;
	m_nStructureSurfaceDepth = DEFAULT_GRAPH_SURFACE_3D_DEPTH;
	m_nStructureGridSpacer = DEFAULT_GRAPH_SURFACE_3D_GRID_SPACER;
	m_nStructureSurfaceSpacer = DEFAULT_GRAPH_SURFACE_3D_SURFACE_SPACER;
	m_nStructureZFactor = DEFAULT_GRAPH_SURFACE_3D_Z_FACTOR;

	// init graph properties
	m_nSurfaceFillMode = GL_FILL;
	m_nShadeModel = GL_SMOOTH;
	m_bShowAxis = 0;
	m_bMapImageToSurface = 0;
	m_bImageTestureInitialized = 0;
	m_bShowGraphGrid = 1;
	// init state flags
	m_bShowCursor = 1;
	m_bShowStateText = 0;
	m_bShowGraphGridLabels = 1;
//	m_bStructureGridLabels = 0;
	m_bStructureSurfaceLabels = 0;
	m_bNewGridConfig = 0;
	m_bNewGridConfigB = 0;
	m_bShowSurface = 1;
	m_bShowProjections = 0;

	// :: mouse states init
	m_nMouseX = -999;
	m_nMouseY = -999;
	m_bMouseDragging = 0;
	// :: camera init
	m_nCamAlpha=30;
	m_nCamBeta=5;
	m_nCamZoom=0;
	m_nCamX=0.0;
	m_nCamY=0.0;
	///////////////////////////////////////////////
	// :: Initial lighting params
	m_nLightCoordX = 50;		// [0]  X position 
	m_nLightCoordY = 80;		// [1]  Y position 
	m_nLightCoordZ = 100;		// [2]  Z position 
	m_nLightingAmbient = 45;	// [3]  Ambient light 
	m_nLightingDiffuse = 70;	// [4]  Diffuse light 
	m_nLightingSpecular = 100;	// [5]  Specular light
	m_nMaterialAmbient = 100;	// [6]  Ambient material
	m_nMaterialDiffuse = 100;	// [7]  Diffuse material
	m_nMaterialSpecular = 40;	// [8]  Specular material
	m_nMaterialShininess = 70;	// [9]  Shininess material
	m_nMaterialEmission = 0;	// [10] Emission material
	////////////////////////
	// :: init other params
	m_nStructureZFactorDiv = (float) DEFAULT_GRAPH_SURFACE_3D_SURFACE_Z_DIV;
	m_nGridType = DEFAULT_GRAPH_3D_GRID_TYPE_SIMPLE;

	// call to set current and show
//    parent->Show( true );
//    SetCurrent();
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CImage3dBase
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CImage3dBase::~CImage3dBase( )
{
	// delete texture
	CloseTextureMapping( );
	// Delete The Font List - All 96 Characters
	glDeleteLists( m_nFontBase, 96 );
	glDeleteLists( m_nFontBaseB, 96 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Init
// Class:	CImage3dBase
// Purpose:	init 3d view
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::Init( )
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glShadeModel( GL_SMOOTH );
	// The depth of the Z-buffer will be taken into account
    glEnable(GL_DEPTH_TEST);
	glDisable( GL_CULL_FACE );
	glEnable( GL_NORMALIZE );
	glDepthFunc( GL_LESS );
	// Lighting will be used
	glEnable(GL_LIGHTING);
	// Only one (first) source of light
	glEnable(GL_LIGHT0);
	// Material colors will be taken into account
//	glEnable(GL_COLOR_MATERIAL);

	// setup my light
    SetLight( );

    glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 25.0 );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef( 0.0, 0.0, -6.0 );

	BuildFont( m_nFontBase, 12, FW_BOLD );
	BuildFont( m_nFontBaseB, 24, FW_NORMAL );

	// build call lists
//	DrawGrid( );
//	DrawAxis( );

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	SetLight
// Class:	CImage3dBase
// Purpose:	set my light (0)
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::SetLight( )
{
	// Both surface sides are considered when calculating
	// each pixel color with the lighting formula
	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 1 );
	// test two lights
//	GLfloat lightAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f} ;
//	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lightAmbient);

	// Light source position depends on the object sizes scaled to (0,100)
	float fPos[] =
	{
		( m_nLightCoordX-50 )*m_nStructureSurfaceWidth/100,
		( m_nLightCoordY-50 )*m_nStructureSurfaceHeight/100,
		( m_nLightCoordZ-50 )*m_nStructureSurfaceDepth/100,
		1.f
	};
	glLightfv( GL_LIGHT0, GL_POSITION, fPos );

	// Ambient light intensity
	float f = m_nLightingAmbient/100.f;
	float fAmbient[4] = { f, f, f, 0.f };
	glLightfv( GL_LIGHT0, GL_AMBIENT, fAmbient );

	// Diffuse light intensity
	f = m_nLightingDiffuse/100.f;	
	float fDiffuse[4] = { f, f, f, 0.f };
	glLightfv( GL_LIGHT0, GL_DIFFUSE, fDiffuse );

	// Specular light intensity
	f = m_nLightingSpecular/100.f;
	float fSpecular[4] = { f, f, f, 0.f };
	glLightfv( GL_LIGHT0, GL_SPECULAR, fSpecular );

	/////////////////////////////
	// test second light
/*	glEnable(GL_LIGHT1);
	GLfloat light1Position[] = {0.5f, 0.7f, 0.2f, 0.0f} ;
	GLfloat light1Ambient[]  = {1.0f, 0.5f, 0.0f, 0.8f} ;
	GLfloat light1Diffuse[]  = {1.0f, 0.5f, 0.0f, 0.8f} ;
	GLfloat light1Specular[] = {1.0f, 0.5f, 0.0f, 0.8f} ;
	glLightfv(GL_LIGHT1, GL_POSITION,light1Position) ;
	glLightfv(GL_LIGHT1, GL_AMBIENT,light1Ambient) ;
	glLightfv(GL_LIGHT1, GL_DIFFUSE,light1Diffuse) ;
	glLightfv(GL_LIGHT1, GL_SPECULAR,light1Specular) ;
*/
	/////////////////////////////////////////////
	// Surface material reflection properties for each light component
	f = m_nMaterialAmbient/100.f;
	float fAmbMat[4] = { f, f, f, 0.f };
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, fAmbMat );

	f = m_nMaterialDiffuse/100.f;
	float fDifMat[4] = { f, f, f, 1.f };
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, fDifMat );

	f = m_nMaterialSpecular/100.f;
	float fSpecMat[4] = { f, f, f, 0.f };
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, fSpecMat );

	// Material shininess
	float fShine = 128 * m_nMaterialShininess/100.f;
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, fShine );

	// Material light emission property
	f = m_nMaterialEmission/100.f;
	float fEmission[4] = { f, f, f, 0.f };
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, fEmission );
/*
/////////////////////////////////////////////////////
	// the usual
  glEnable(GL_LIGHTING);
  GLfloat lightAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f} ;
  glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lightAmbient);

  glEnable(GL_LIGHT0);
  GLfloat light0Position[] = {0.7f, 0.5f, 0.9f, 0.0f} ;
  GLfloat light0Ambient[]  = {0.0f, 0.5f, 1.0f, 0.8f} ;
  GLfloat light0Diffuse[]  = {0.0f, 0.5f, 1.0f, 0.8f} ;
  GLfloat light0Specular[] = {0.0f, 0.5f, 1.0f, 0.8f} ;
  glLightfv(GL_LIGHT0, GL_POSITION,light0Position) ;
  glLightfv(GL_LIGHT0, GL_AMBIENT,light0Ambient) ;
  glLightfv(GL_LIGHT0, GL_DIFFUSE,light0Diffuse) ;
  glLightfv(GL_LIGHT0, GL_SPECULAR,light0Specular) ;

  glEnable(GL_LIGHT1);
  GLfloat light1Position[] = {0.5f, 0.7f, 0.2f, 0.0f} ;
  GLfloat light1Ambient[]  = {1.0f, 0.5f, 0.0f, 0.8f} ;
  GLfloat light1Diffuse[]  = {1.0f, 0.5f, 0.0f, 0.8f} ;
  GLfloat light1Specular[] = {1.0f, 0.5f, 0.0f, 0.8f} ;
  glLightfv(GL_LIGHT1, GL_POSITION,light1Position) ;
  glLightfv(GL_LIGHT1, GL_AMBIENT,light1Ambient) ;
  glLightfv(GL_LIGHT1, GL_DIFFUSE,light1Diffuse) ;
  glLightfv(GL_LIGHT1, GL_SPECULAR,light1Specular) ;
*/

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	InitTextureMapping
// Class:	CImage3dBase
// Purpose:	init texture mapping
// Input:	reference to image
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::InitTextureMapping( wxImage& pImage )
{
//	if( m_bImageTestureInitialized ) return;

	// Allocate space for texture
	glGenTextures( 1, &m_nTexture );
	// Set our Tex handle as current
	glBindTexture( GL_TEXTURE_2D, m_nTexture ); 
 
	int w = m_nStructureSurfaceWidth;
	int h = m_nStructureSurfaceHeight;

	// read image - from astro image
	m_pImageMap = pImage.Copy();
	w = m_pImageMap.GetWidth();
	h = m_pImageMap.GetHeight();

	char* pBitmapToMap = (char*) m_pImageMap.GetData();
	// larry : todo - check here why sometimes return is null ?
	if( !pBitmapToMap ) return;

	// generate my texture from image
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)pBitmapToMap );

	// specify texture parameters
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	// GL_MODULATE, GL_DECAL, GL_BLEND and GL_REPLACE.
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
//	glEnable( GL_TEXTURE_2D );

//	glBindTexture( GL_TEXTURE_2D, m_nTexture );

	m_bImageTestureInitialized = 1;

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	CloseTextureMapping
// Class:	CImage3dBase
// Purpose:	close/destroy texture mapping
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::CloseTextureMapping( )
{
	glDisable( GL_TEXTURE_2D );
	glDeleteTextures( 1, &m_nTexture );

	//free( m_pBitmapToMap );
	m_pImageMap.Destroy();
	m_bImageTestureInitialized = 0;
}

/////////////////////////////////////////////////////////////////////
// Method:	BuildFont
// Class:	CImage3dBase
// Purpose:	Build Our Bitmap Font
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
GLvoid CImage3dBase::BuildFont( GLuint& nFontId, int nSize, int nFontWeight )
{
	HFONT	font;										// Windows Font ID
	HFONT	oldfont;									// Used For Good House Keeping

	nFontId = glGenLists(96);						// Storage For 96 Characters

	font = CreateFont(	-nSize,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						wxT("Courier New"));					// Font Name

	wxWindowDC dc( this );
	HDC hDC = (HDC)dc.GetHDC();

	oldfont = (HFONT)SelectObject( hDC, font);           // Selects The Font We Want
	wglUseFontBitmaps( hDC, 32, 96, nFontId );				// Builds 96 Characters Starting At Character 32
	SelectObject( hDC, oldfont );							// Selects The Font We Want
	DeleteObject( font );									// Delete The Font
}

/////////////////////////////////////////////////////////////////////
// Method:	glPrint
// Class:	CImage3dBase
// Purpose:	Custom GL "Print" Routine
// Input:	font, params
// Output:	nothing
/////////////////////////////////////////////////////////////////////
GLvoid CImage3dBase::glPrint( GLuint nFont, const char *fmt, ... )
{
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if( fmt == NULL )									// If There's No Text
		return;											// Do Nothing

	va_start( ap, fmt );									// Parses The String For Variables
	vsprintf( text, fmt, ap );							// And Converts Symbols To Actual Numbers
	va_end( ap );											// Results Are Stored In Text

	glPushAttrib( GL_LIST_BIT );							// Pushes The Display List Bits
	glListBase( nFont - 32);						// Sets The Base Character to 32
	// Draws The Display List Text
	glCallLists( strlen(text), GL_UNSIGNED_BYTE, text );	
	glPopAttrib( );										// Pops The Display List Bits
}

/////////////////////////////////////////////////////////////////////
// Method:	SetDrawing
// Class:	CImage3dBase
// Purpose:	set drawing parameters, camera, perspective, etc
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::SetDrawing( )
{
	// Make a rotation matrix out of mouse point
	const Mtx& rot = rotateY( m_nCamBeta ) * rotateX( m_nCamAlpha );

	// Rotate camera
	Vec cam_loc(0,0,-400), cam_up(0,1,0);
	cam_loc = cam_loc * rot;
	cam_up = cam_up * rot;

	// Clear the screen
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	// Prepare zoom by changiqng FOV
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float fov = 45 + m_nCamZoom;
	if( fov < 5 ) fov = 5;
	if( fov > 160 ) fov = 160;
	gluPerspective( fov, (float)m_nWinWidth/(float)m_nWinHeight, 1.0, 900.0 );

	gluLookAt( cam_loc.x, cam_loc.y, cam_loc.z,		// eye
					m_nCamX, m_nCamY, 0,			// target
					cam_up.x, cam_up.y, cam_up.z );	// up
	//  init other 
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_COLOR_MATERIAL );

	// set lighting
	if( m_bUseLighting )
	{
		glEnable( GL_LIGHTING );
		glEnable( GL_LIGHT0 );
		//glEnable( GL_LIGHT1 );

	} else
	{
		glDisable( GL_LIGHTING );
		glDisable( GL_LIGHT0 );
		//glDisable( GL_LIGHT1 );
	}
	// set shade model - check if it works ?
	glShadeModel( m_nShadeModel );
	// set surface mode
	glPolygonMode( GL_FRONT_AND_BACK, m_nSurfaceFillMode );
	// set texture if to use
	if( m_bMapImageToSurface )
	{
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, m_nTexture );
	}

	// reset line style
	glEnable( GL_LINE_WIDTH );
	glLineWidth( 1 );
	glDisable( GL_LINE_WIDTH );

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	DrawGrid
// Class:	CImage3dBase
// Purpose:	draw grid
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::DrawGrid( )
{
	glNewList( OPENGL_EXEC_LIST_BUILD_RGB_SURFACE_GRID , GL_COMPILE );

	int x, y, z;

	// get size
	int w = m_nStructureSurfaceWidth;
	int h = m_nStructureSurfaceHeight;
	float hw = (float) w/2.0;
	float hh = (float) h/2.0;
	int d = m_nStructureSurfaceDepth;
	float hd = (float) d/2.0;
	int s = m_nStructureGridSpacer;
	int s2 = 2*s;
	float s_p = (float) s - 0.5;

	glPopMatrix();
	glPushMatrix();

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
//	glBegin( GL_QUADS );

	//////////////////////////////////
	// Flat grid :: down z/x
	// set line style
	glEnable( GL_LINE_STIPPLE );
	glLineWidth( 1 );
	glLineStipple( 2, 0x9999 );

	// set color
	glColor3ub( 128, 128, 128 );
	// :: style doted
	for( z=hh; z>=-hh; z-=s )
	{
		glBegin( GL_LINES );
		glVertex3f( -0.5-hw, -0.5f, z-0.5 );
		glVertex3f( -0.5+hw, -0.5f, z-0.5 );
		glEnd();
	}
	// :: veritical lines
	for( x=hw; x>=-hw; x-=s )
	{
		glBegin( GL_LINES );
		glVertex3f( x-0.5, -0.5f, -0.5-hh );
		glVertex3f( x-0.5, -0.5f, -0.5+hh );
		glEnd();
	}

/*	for( x=-w/2; x<w/2-(s-1); x+=s )
	{
		for( z=-h/2; z<h/2-(s-1); z+=s )
		{
			glBegin( GL_QUADS );
			glVertex3f( -0.5, -0.5f, z-0.5 );
			glVertex3f( x+w-0.5, -0.5f, z-0.5 );
			glEnd();
			glVertex3f( x-0.5, -0.5f, z-0.5 );
			glVertex3f( x-0.5, -0.5f, z+s_p );
			glVertex3f( x+s_p, -0.5f, z+s_p );
			glVertex3f( x+s_p, -0.5f, z-0.5 );
		}
	}
*/

	// if set for nice grid - draw the other sides
	if( m_nGridType == DEFAULT_GRAPH_3D_GRID_TYPE_SIMPLE )
	{
		////////////////////////////
		// Flat grid :: right
		glColor3ub( 128, 150, 128 );
		for( x=-w/2; x<w/2+s; x+=s )
		{
			glBegin( GL_LINES );
			glVertex3f( x-0.5, -0.5f, -0.5f+hh );
			glVertex3f( x-0.5, -0.5f+d, -0.5f+hh );
			glEnd();
		}
		// horizontal lines
		for( y=s; y<d+1; y+=s )
		{
			glBegin( GL_LINES );
			glVertex3f( -0.5-hw, -0.5f+y, -0.5f+hh );
			glVertex3f( -0.5+hw, -0.5f+y, -0.5f+hh );
			glEnd();
		}

/*		for( x=-w/2; x<w/2-(s-1); x+=s )
		{
			for( y=0; y<d-(s-1); y+=s )
			{
				glVertex3f( x-0.5, y-0.5f, hh-0.5f );
				glVertex3f( x-0.5, y+s_p, hh-0.5f );
				glVertex3f( x+s_p, y+s_p, hh-0.5f );
				glVertex3f( x+s_p, y-0.5f, hh-0.5f );
			}
		}
*/
		/////////////////////////////////
		// Flat grid :: left
		glColor3ub( 150, 128, 128 );
		for( z=-h/2; z<h/2-(s-1); z+=s )
		{
			glBegin( GL_LINES );
			glVertex3f( -0.5+hw, -0.5f, z-0.5 );
			glVertex3f( -0.5+hw, -0.5f+d, z-0.5 );
			glEnd();
		}
		// horizontal lines
		for( y=s; y<d+1; y+=s )
		{
			glBegin( GL_LINES );
			glVertex3f( -0.5+hw, -0.5f+y, -0.5f-hh );
			glVertex3f( -0.5+hw, -0.5f+y, -0.5f+hh );
			glEnd();
		}

/*		for( z=-h/2; z<h/2-(s-1); z+=s )
		{
			for( y=0; y<d-(s-1); y+=s )
			{
				glVertex3f( hw-0.5, y-0.5f, z-0.5f );
				glVertex3f( hw-0.5, y-0.5f, z+s_p );
				glVertex3f( hw-0.5, y+s_p, z+s_p );
				glVertex3f( hw-0.5, y+s_p, z-0.5f );
			}
		}
*/
	}
	glDisable( GL_LINE_STIPPLE );

//	glEnd();

	////////////////////////////////////
	// draw labels/ticks if selected
	if( m_bShowGraphGridLabels )
	{
		////////////////////////////////////////////////
		// DRAW TICKS
		int nTickSize = 6;
		int nTick = nTickSize;
		// set line style
		glEnable( GL_LINE_WIDTH );
		glLineWidth( 2 );
		// :: x ticks
		glColor3ub( 128, 128, 128 );
		for( x=hw; x>=-hw; x-=s )
		{
			if( (int)(x-hw) % s2 )
			{
				glLineWidth( 1 );
				nTick = nTickSize/2;
			} else
			{
				glLineWidth( 2 );
				nTick = nTickSize;
			}
			// draw tick line
			glBegin( GL_LINES );
			glVertex3f( x-0.5, -0.5f, -0.5-hh );
			glVertex3f( x-0.5, -0.5f, -0.5-hh-nTick );
			glEnd();
		}
		// :: z ticks
		for( z=hh; z>=-hh; z-=s )
		{
			if( (int)(z-hh) % s2 )
			{
				glLineWidth( 1 );
				nTick = nTickSize/2;
			} else
			{
				glLineWidth( 2 );
				nTick = nTickSize;
			}
			// draw tick line
			glBegin( GL_LINES );
			glVertex3f( -0.5-hw, -0.5f, z-0.5 );
			glVertex3f( -0.5-hw-nTick, -0.5f, z-0.5 );
			glEnd();
		}
		// :: y ticks
		glColor3ub( 150, 128, 128 );
		for( y=0; y<d+1; y+=s )
		{
			if( y % s2 )
			{
				glLineWidth( 1 );
				nTick = nTickSize/2;
			} else
			{
				glLineWidth( 2 );
				nTick = nTickSize;
			}
			// draw tick line
			glBegin( GL_LINES );
			glVertex3f( -0.5+hw, -0.5f+y, -0.5f-hh );
			glVertex3f( -0.5+hw, -0.5f+y, -0.5f-hh-nTick );
			glEnd();
		}
		// end of line style
		glDisable( GL_LINE_WIDTH );

		////////////////////////////////////////////////
		// :: draw labels x
		glPopMatrix();
		glPushMatrix();
		glColor3f( 255, 255, 0 );
		glTranslatef( hw+s2, -4.0f, -hh-nTickSize-3 );
		for( x=hw; x>=-hw; x-=s2 )
		{
			// set labels
			glTranslatef( -s2, 0, 0 );	
			glRasterPos2f( (GLfloat) 1.9, (GLfloat) -0.9 );
			int count = (int) ((hw-x)/m_nScaleToGraph);
			glPrint( m_nFontBase, "%d", count );
		}
		// :: draw labels z
		glPopMatrix();
		glPushMatrix();

		glColor3f( 255, 255, 0 );
		glTranslatef( -hw-nTickSize-3, -4.0f, hh+s2 );
		for( z=hh; z>=-hh; z-=s2 )
		{
			// set labels
			glTranslatef( 0, 0, -s2 );	
			glRasterPos2f( (GLfloat) -1.9, (GLfloat) -0.9 );
			int count = (int) ((hh-z)/m_nScaleToGraph);
			glPrint( m_nFontBase, "%d", count );
		}
		// :: draw labels y
		glPopMatrix();
		glPushMatrix();

		glColor3f( 255, 255, 0 );
		glTranslatef( hw+10, -5, -hh-nTickSize-5 );
		for( y=s2; y<d+1; y+=s2 )
		{
			// set labels
			glTranslatef( 0, s2, 0 );	
			glRasterPos2f( (GLfloat) -2.9, (GLfloat) -0.9 );
			int count = (int) (y/m_nScaleToGraph);
			glPrint( m_nFontBase, "%d", count );
		}
		glPopMatrix();
		glPushMatrix();
	}	

	glPopMatrix();
	glPushMatrix();

	glEndList();

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	DrawAxis
// Class:	CImage3dBase
// Purpose:	draw axis
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::DrawAxis( )
{
	glNewList( OPENGL_EXEC_LIST_BUILD_RGB_SURFACE_AXIS, GL_COMPILE );

	glPopMatrix();
	glPushMatrix();

	if( m_nGridType == DEFAULT_GRAPH_3D_GRID_TYPE_SIMPLE )
	{
		///////////////
		// Axes: (x,y,z)=(r,g,b)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// set line style
		glEnable( GL_LINE_WIDTH );
		glLineWidth( 2 );
		glColor3ub( 255, 0, 0 );
		float hw = m_nStructureSurfaceWidth/2;
		float hh = m_nStructureSurfaceHeight/2;
		float d = m_nStructureSurfaceDepth;

		glPopMatrix();
		glPushMatrix();

		////////////////
		// axis :: z
		glBegin( GL_LINES );
		glVertex3f( hw, 0.0f, hh );
		glVertex3f( hw, 0.0f, -hh-50 );
		glEnd();
		// end arrow
		glTranslatef( hw, 0.0, -hh-50-3 );
		glRotatef( 180.0, 0.0, 1.0, 0.0 );
		drawSolidCone( 8.0, 10.0, 3, 1 );
		// set label
		glTranslatef( 0, 12, 0 );	
		glRasterPos2f( (GLfloat) -2.9, (GLfloat) -0.9 );
		glPrint( m_nFontBaseB, "Z" );

		glPopMatrix();
		glPushMatrix();

		////////////////
		// axis :: x
		glBegin( GL_LINES );
		glVertex3f( hw, 0.0f, hh );
		glVertex3f( -hw-50, 0.0f, hh );
		glEnd();
		// end arrow
		glTranslatef( -hw-50+3, 0.0, hh-6 );
		glRotatef( -90.0, 0.0, 1.0, 0.0 );
		drawSolidCone( 8.0, 10.0, 3, 1 );
		// set label
		glTranslatef( 0, 12, 0 );	
		glRasterPos2f( (GLfloat) -2.9, (GLfloat) -0.9 );
		glPrint( m_nFontBaseB, "X" );

		glPopMatrix();
		glPushMatrix();

		////////////////
		// axis :: y
		glBegin( GL_LINES );
		glVertex3f( hw, 0.0f, hh );
		glVertex3f( hw, d+50.0f, hh );
		glEnd();
		// end arrow
		glTranslatef( hw, d+50.0f-4, hh-6 );
		glRotatef( -90.0, 1.0, 0.0, 0.0 );
		drawSolidCone( 8.0, 10.0, 3, 1 );
		// set label
		glTranslatef( -10, 0, -10 );	
		glRasterPos2f( (GLfloat) -2.9, (GLfloat) -0.9 );
		glPrint( m_nFontBaseB, "Y" );

		glDisable( GL_LINE_WIDTH );

/*		////////////////////////
		//old style
		glPushMatrix();
		glRotatef(90.0, 0.0, 1.0, 0.0);
		glColor3ub( 255, 0, 0 );
		drawSolidCone( 0.5, 170.0, 5, 1 );
		glPopMatrix();
		glPushMatrix();
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		glColor3ub( 0, 255,  0 );
		drawSolidCone(0.5, 80.0, 5, 1);
		glPopMatrix();
		glPushMatrix();
		glRotatef(0.0, 1.0, 0.0, 0.0);
		glColor3ub( 0, 0, 255 );
		drawSolidCone(0.5, 120.0, 5, 1);
		glPopMatrix();
*/
	}

	glPopMatrix();
	glPushMatrix();
	
	glEndList();

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	DrawCursor
// Class:	CImage3dBase
// Purpose:	draw cursor
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::DrawCursor( )
{
	GLdouble model[16], proj[16];
	GLint view[4];
	GLfloat z;
	GLdouble ox, oy, oz;

	glGetDoublev( GL_MODELVIEW_MATRIX, model );
	glGetDoublev( GL_PROJECTION_MATRIX, proj );
	glGetIntegerv( GL_VIEWPORT, view );

	glReadPixels( m_nMouseX, view[3]-m_nMouseY-1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z );
	gluUnProject( m_nMouseX, view[3]-m_nMouseY-1, z, model, proj, view, &ox, &oy, &oz );

	//cursor_loc = Vec(ox, oy, oz);

	// Draw the cursor
	glPushMatrix( );
	glDisable( GL_LIGHTING ) ;
	glTranslatef( ox, oy, oz );
	glColor3ub( 255, 0, 255 );
	drawSolidSphere( 0.5, 12, 12 );
	glPopMatrix( );

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	DrawStateText
// Class:	CImage3dBase
// Purpose:	draw state text
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::DrawStateText( )
{
//	glDisable( GL_DEPTH_TEST );
	//glLoadIdentity();
	//gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
	//glColor3ub( 255, 255, 0 );
	glTranslatef( -m_nStructureSurfaceWidth/2-4.0, 10.0f, 0.0f);	
	glColor3f( 255, 255, 0 );
	//glVertex3f(0.0,-1.0,0.2);
	glRasterPos2f( (GLfloat) -0.0, (GLfloat) -0.0 );

	// print my string ... just a text here
	glPrint( m_nFontBase, "LA LA LA state: %4.3f", 0.0 );

//	glEnable( GL_DEPTH_TEST );

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	drawSolidCone
// Class:	CImage3dBase
// Purpose:	draw a solig cone 
// Input:	cone data
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::drawSolidCone( GLdouble base, GLdouble height, GLint slices, GLint stacks )
{
	glBegin(GL_LINE_LOOP);
	GLUquadricObj* quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);

	gluCylinder(quadric, base, 0, height, slices, stacks);

	gluDeleteQuadric(quadric);
	glEnd();
}

/////////////////////////////////////////////////////////////////////
// Method:	drawSolidSphere
// Class:	CImage3dBase
// Purpose:	draw a solig sphere 
// Input:	cone data
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::drawSolidSphere( GLdouble radius, GLint slices, GLint stacks )
{
	glBegin(GL_LINE_LOOP);
	GLUquadricObj* quadric = gluNewQuadric();
	
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluSphere(quadric, radius, slices, stacks);
	
	gluDeleteQuadric(quadric);
	glEnd();	
}

/////////////////////////////////////////////////////////////////////
// Method:	CalculateStructures
// Class:	CImage3dBase
// Purpose:	calculate structures
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::CalculateStructures( )
{
	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	DrawStructures
// Class:	CImage3dBase
// Purpose:	draw structures
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::DrawStructures( )
{
	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	OnPaint
// Class:	CImage3dBase
// Purpose:	on paint event
// Input:	pointer to event
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    if (!GetContext()) return;
    if( !IsShown() ) return;
	SetCurrent( );

	///////////////////////
	// set lights
	SetLight( );
	// set drawing mode - camera, fov, etc
	SetDrawing( );

//	glPopAttrib();
//	glPushAttrib( GL_ALL_ATTRIB_BITS  );

	//////////////////////////////////
	// draw my structure - this to be re-defined in other objects
	DrawStructures( );

//	glPopAttrib();
//	glPushAttrib( GL_ALL_ATTRIB_BITS );

	//////////////////////////
	// Visual Helpers :: axes, grid etc
	// :: disable texture and lights for next graph elements
//	if( m_bMapImageToSurface ) glDisable( GL_TEXTURE_2D );
//	glDisable(GL_LIGHTING);
	// draw grid &| axis
/*	if( m_bShowGraphGrid )
	{
		glCallList( OPENGL_EXEC_LIST_BUILD_RGB_SURFACE_GRID ); 

		// draw axis
		if( m_bShowAxis ) glCallList( OPENGL_EXEC_LIST_BUILD_RGB_SURFACE_AXIS );
	}
*/	// draw cursor
	if( m_bShowCursor )
	{
//		DrawCursor( );
	}
	// draw state text
//	if( m_bShowStateText ) DrawStateText( );

	glFlush();
    SwapBuffers( );
}

/////////////////////////////////////////////////////////////////////
// Method:	OnSize
// Class:	CImage3dBase
// Purpose:	on size event
// Input:	pointer to event
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::OnSize( wxSizeEvent& event )
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);

    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
    int w, h;
    GetClientSize(&w, &h);
	// window sizes
	m_nWinWidth = w;
	m_nWinHeight = h;

    if( GetContext() )
    {
        if( IsShown() ) SetCurrent();
        glViewport(0, 0, (GLint) w, (GLint) h);
    }
}

/////////////////////////////////////////////////////////////////////
// Method:	OnChar
// Class:	CImage3dBase
// Purpose:	on char event
// Input:	pointer to event
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::OnChar( wxKeyEvent& event )
{
    switch( event.GetKeyCode() )
    {
		// rotate camera left
		case WXK_LEFT: { m_nCamBeta -= 5.0; break; }
		// rotate camera right
		case WXK_RIGHT: { m_nCamBeta += 5.0; break; }
		// roatate camera up
		case WXK_UP: { m_nCamAlpha += 5.0; break; }
		// rotate camera down
		case WXK_DOWN: { m_nCamAlpha -= 5.0; break; }
		// if set to smooth
		case 's': case 'S':
		{
			if( m_nShadeModel == GL_FLAT )
				m_nShadeModel = GL_SMOOTH;
			else
				m_nShadeModel = GL_FLAT;
			break;
		}
		// if turn light on/off
		case 'l': case 'L':
		{
			if( m_bUseLighting == 0 )
				m_bUseLighting = 1;
			else
				m_bUseLighting = 0;
			break;
		}
		// if increase Z multiplication factor
		case '*':
		{
			m_nStructureZFactor += m_nStructureZFactorDiv;
			m_bCalculateStructures = 1;
			break;
		}
		// if decrease Z multiplication factor
		case '/':
		{
			m_nStructureZFactor -= m_nStructureZFactorDiv;
			if( m_nStructureZFactor < 0 ) m_nStructureZFactor = 0;
			m_bCalculateStructures = 1;
			break;
		}
		// if zoom in
		case '+': { m_nCamZoom += 1; break; }
		// if zoom out
		case '-': { m_nCamZoom -= 1; break; }

		default:
			event.Skip();
			return;
    }

	// check if pre-display calculations
	if( m_bCalculateStructures ) CalculateStructures( );
	// call for repaint
    Refresh(false);
}

/////////////////////////////////////////////////////////////////////
// Method:	OnMouseEvent
// Class:	CImage3dBase
// Purpose:	on mouse event
// Input:	pointer to event
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::OnMouseEvent( wxMouseEvent& pEvent )
{
	// get current x/y
	float x = (float) pEvent.GetX();
	float y = (float) pEvent.GetY();
	// set dragging 
	float nDragX = 0.0;
	float nDragY = 0.0;
	// if drag - set drag if also click event
    if( !m_bMouseDragging && ( pEvent.LeftIsDown() || pEvent.RightIsDown() ) )
    {
        m_bMouseDragging = 1;
		// set new mouse coordinates
		m_nMouseX = x;
		m_nMouseY = y;
		// do event skip and exit
		pEvent.Skip();
		return;

    } else
    {
		nDragY = (float) (y - (float) m_nMouseY);
		nDragX = (float) (x - (float) m_nMouseX);
	}
	// set new mouse coordinates
	m_nMouseX = x;
	m_nMouseY = y;

	////////////////////////////////////
	// now event mouse click type
	if( pEvent.LeftIsDown() )
    {
		// set new camera angles
		m_nCamAlpha += nDragY;
		m_nCamBeta += -nDragX;
        Refresh(false);

    } else if( pEvent.RightIsDown() )
    {
		// set new camera target position
		m_nCamY += nDragY;
		m_nCamX += nDragX;
        Refresh(false);

	} else
        m_bMouseDragging = 0;

	// skip event and return
	pEvent.Skip();
	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnMouseWheel
// Class:	CImage3dBase
// Purpose:	show menu for robot view
// Input:	mouse events
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CImage3dBase::OnMouseWheel( wxMouseEvent& pEvent )
{
	int x = pEvent.GetX(),
		y = pEvent.GetY();

	if( pEvent.m_wheelRotation > 0 )
		m_nCamZoom -= 1;
	else if( pEvent.m_wheelRotation < 0 )
		m_nCamZoom += 1;

	Refresh(false);
}

/////////////////////////////////////////////////////////////////////
// Method:	OnEraseBackground
// Class:	CImage3dBase
// Purpose:	on erase background event 
// Input:	pointer to event
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImage3dBase::OnEraseBackground( wxEraseEvent& WXUNUSED(pEvent) )
{
    // Do nothing, to avoid flashing.
}


// class :: CConfig3DDialog
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CConfig3DDialog, wxDialog )
	EVT_SCROLL( CConfig3DDialog::OnScrollChange )
	EVT_CHECKBOX( -1, CConfig3DDialog::OnCheckChange )
	EVT_TEXT_ENTER( -1, CConfig3DDialog::OnTextChange ) 
END_EVENT_TABLE( )
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CConfig3DDialog
// Purpose:	Initialize my 3d view config dialog 
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CConfig3DDialog::CConfig3DDialog( wxWindow *parent, const wxString& strTitle )
               : wxDialog(parent, -1,
							strTitle,
							wxDefaultPosition,
							wxDefaultSize,
							wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	wxFont smallFont( 6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT );

	// main dialog sizer
	wxFlexGridSizer *pTopSizer = new wxFlexGridSizer( 2, 1, 5, 5 );

	// building configure as a note book
	wxNotebook* pNotebook = new wxNotebook( this, -1, wxPoint(-1,-1), wxDefaultSize ); 
	// add lights panel
	wxPanel* pPanelLights = new wxPanel( pNotebook );
	// add meterials panel
	wxPanel* pPanelMaterials = new wxPanel( pNotebook );
	// add Structures coord panel
	wxPanel* pPanelStructures = new wxPanel( pNotebook );

	///////////////////////////////////////
	// Lights sizer - layout of elements
	wxGridSizer* sizerLights = new wxGridSizer( 1, 2, 10, 5 );
	wxFlexGridSizer* sizerLightsGirdLeft = new wxFlexGridSizer( 3, 1, 10, 2 );
	wxFlexGridSizer* sizerLightsGirdRight = new wxFlexGridSizer( 3, 1, 10, 2 );
	// :: lightining box
	wxStaticBox* pLightsLightingBox = new wxStaticBox( pPanelLights, -1, wxT("Lighting"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerLightsLightingBox = new wxStaticBoxSizer( pLightsLightingBox, wxHORIZONTAL );
	sizerLightsLightingBox->Add( sizerLightsGirdLeft, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// :: lightining box
	wxStaticBox* pLightsLightCoordBox = new wxStaticBox( pPanelLights, -1, wxT("Light Coordinates"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerLightsLightCoordBox = new wxStaticBoxSizer( pLightsLightCoordBox, wxHORIZONTAL );
	sizerLightsLightCoordBox->Add( sizerLightsGirdRight, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// add box sizer to top sizer
	sizerLights->Add( sizerLightsLightingBox, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerLights->Add( sizerLightsLightCoordBox, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	///////////////////////////////////////
	// Materials sizer - layout of elements
	wxGridSizer* sizerMaterials = new wxGridSizer( 1, 2, 10, 5 );
	wxFlexGridSizer* sizerMaterialsGirdLeft = new wxFlexGridSizer( 3, 1, 10, 2 );
	wxFlexGridSizer* sizerMaterialsGirdRight = new wxFlexGridSizer( 3, 1, 10, 2 );
	// add left/rights sizers to top sizer
	sizerMaterials->Add( sizerMaterialsGirdLeft, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerMaterials->Add( sizerMaterialsGirdRight, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////////////////
	// Structures sizer
	wxFlexGridSizer* sizerStructures = new wxFlexGridSizer( 7, 2, 5, 5 );
	sizerStructures->AddGrowableCol( 1 );
	sizerStructures->AddGrowableCol( 0 );
	wxBoxSizer* sizerStructuresPage = new wxBoxSizer( wxHORIZONTAL );
	sizerStructuresPage->Add( sizerStructures, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	///////////////////////////
	// Lights panel elements
	// :: ambient
	m_pLightingAmbient = new wxSlider( pPanelLights, -1, 15, 0, 100, wxPoint(-1, -1),
										wxSize(150, 32 ), wxSL_HORIZONTAL|wxSL_LABELS );
	m_pLightingAmbient->SetFont( smallFont );
	// :: diffuse
	m_pLightingDiffuse = new wxSlider( pPanelLights, -1, 50, 0, 100, wxPoint(-1, -1),
										wxSize(150, 32 ), wxSL_HORIZONTAL|wxSL_LABELS );
	m_pLightingDiffuse->SetFont( smallFont );
	// :: specular
	m_pLightingSpecular = new wxSlider( pPanelLights, -1, 100, 0, 100, wxPoint(-1, -1),
										wxSize(150, 32 ), wxSL_HORIZONTAL|wxSL_LABELS );
	m_pLightingSpecular->SetFont( smallFont );
	// :: coord x
	m_pLightCoordX = new wxSlider( pPanelLights, -1, 50, 0, 100, wxPoint(-1, -1),
										wxSize(150, 32 ), wxSL_HORIZONTAL|wxSL_LABELS );
	m_pLightCoordX->SetFont( smallFont );
	// :: coord y
	m_pLightCoordY = new wxSlider( pPanelLights, -1, 80, 0, 100, wxPoint(-1, -1),
										wxSize(150, 32 ), wxSL_HORIZONTAL|wxSL_LABELS );
	m_pLightCoordY->SetFont( smallFont );
	// :: coord z
	m_pLightCoordZ = new wxSlider( pPanelLights, -1, 70, 0, 100, wxPoint(-1, -1),
										wxSize(150, 32 ), wxSL_HORIZONTAL|wxSL_LABELS );
	m_pLightCoordZ->SetFont( smallFont );


	///////////////////////////
	// Materials panel elements
	// :: ambient
	m_pMaterialAmbient = new wxSlider( pPanelMaterials, -1, 15, 0, 100, wxPoint(-1, -1),
										wxSize(150, 32 ), wxSL_HORIZONTAL|wxSL_LABELS );
	m_pMaterialAmbient->SetFont( smallFont );
	// :: diffuse
	m_pMaterialDiffuse = new wxSlider( pPanelMaterials, -1, 30, 0, 100, wxPoint(-1, -1),
										wxSize(150, 32 ), wxSL_HORIZONTAL|wxSL_LABELS );
	m_pMaterialDiffuse->SetFont( smallFont );
	// :: specular
	m_pMaterialSpecular = new wxSlider( pPanelMaterials, -1, 60, 0, 100, wxPoint(-1, -1),
										wxSize(150, 32 ), wxSL_HORIZONTAL|wxSL_LABELS );
	m_pMaterialSpecular->SetFont( smallFont );
	// :: shininess
	m_pMaterialShininess = new wxSlider( pPanelMaterials, -1, 60, 0, 100, wxPoint(-1, -1),
										wxSize(150, 32 ), wxSL_HORIZONTAL|wxSL_LABELS );
	m_pMaterialShininess->SetFont( smallFont );
	// :: emission
	m_pMaterialEmission = new wxSlider( pPanelMaterials, -1, 80, 0, 100, wxPoint(-1, -1),
										wxSize(150, 32 ), wxSL_HORIZONTAL|wxSL_LABELS );
	m_pMaterialEmission->SetFont( smallFont );

	///////////////////////////
	// Materials panel elements
	// :: surface width
	m_pStructureSurfaceWidth = new wxTextCtrl( pPanelStructures, 
									wxID_3D_STRUCTURE_SURFACE_WIDTH_TEXT, wxT(""), 
									wxDefaultPosition, wxSize(50,17), wxTE_PROCESS_ENTER );
	// :: surface height
	m_pStructureSurfaceHeight = new wxTextCtrl( pPanelStructures, 
									wxID_3D_STRUCTURE_SURFACE_HEIGHT_TEXT, wxT(""),
									wxDefaultPosition, wxSize(50,17), wxTE_PROCESS_ENTER );
	// :: grid spacer
	m_pStructureGridSpacer = new wxTextCtrl( pPanelStructures, 
									wxID_3D_STRUCTURE_GRID_SPACER_TEXT, wxT(""), 
									wxDefaultPosition, wxSize(50,17), wxTE_PROCESS_ENTER );
	// :: surface spacer
	m_pStructureSurfaceSpacer = new wxTextCtrl( pPanelStructures, -1, wxT(""), wxDefaultPosition, wxSize(50,17), wxTE_PROCESS_ENTER );
	// :: Z Factor
	m_pStructureZFactor = new wxTextCtrl( pPanelStructures, -1, wxT(""), wxDefaultPosition, wxSize(50,17), wxTE_PROCESS_ENTER );
	// :: grid axis
	m_pShowAxis = new wxCheckBox( pPanelStructures, -1, wxT("")  );
	// :: grid labels
	m_pStructureGridLabels = new wxCheckBox( pPanelStructures, -1, wxT("")  );
	// :: surface labels
	m_pStructureSurfaceLabels = new wxCheckBox( pPanelStructures, -1, wxT("")  );
	// :: Show Projections
	m_pShowProjections = new wxCheckBox( pPanelStructures, -1, wxT("")  );

	/////////////////////////////////////////
	// populate Light panel
	// :: ambient
	sizerLightsGirdLeft->Add( new wxStaticText( pPanelLights, -1, wxT("Ambient")), 0,
													wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL);
	sizerLightsGirdLeft->Add( m_pLightingAmbient, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );
	// :: diffuse
	sizerLightsGirdLeft->Add( new wxStaticText( pPanelLights, -1, wxT("Diffuse")), 0,
													wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL);
	sizerLightsGirdLeft->Add( m_pLightingDiffuse, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );
	// :: specular
	sizerLightsGirdLeft->Add( new wxStaticText( pPanelLights, -1, wxT("Specular")), 0,
													wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL);
	sizerLightsGirdLeft->Add( m_pLightingSpecular, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );
	// :: coord x
	sizerLightsGirdRight->Add( new wxStaticText( pPanelLights, -1, wxT("X")), 0,
													wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL);
	sizerLightsGirdRight->Add( m_pLightCoordX, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );
	// :: coord y
	sizerLightsGirdRight->Add( new wxStaticText( pPanelLights, -1, wxT("Y")), 0,
													wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL);
	sizerLightsGirdRight->Add( m_pLightCoordY, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );
	// :: coord z
	sizerLightsGirdRight->Add( new wxStaticText( pPanelLights, -1, wxT("Z")), 0,
													wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL);
	sizerLightsGirdRight->Add( m_pLightCoordZ, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL  );

	/////////////////////////////////////////
	// populate Materials panel
	// :: ambient
	sizerMaterialsGirdLeft->Add( new wxStaticText( pPanelMaterials, -1, wxT("Ambient")), 0,
													wxALIGN_CENTER | wxALIGN_BOTTOM );
	sizerMaterialsGirdLeft->Add( m_pMaterialAmbient, 1, wxALIGN_CENTER | wxALIGN_TOP  );
	// :: diffuse
	sizerMaterialsGirdLeft->Add( new wxStaticText( pPanelMaterials, -1, wxT("Diffuse")), 0,
													wxALIGN_CENTER | wxALIGN_BOTTOM );
	sizerMaterialsGirdLeft->Add( m_pMaterialDiffuse, 1, wxALIGN_CENTER | wxALIGN_TOP  );
	// :: specular
	sizerMaterialsGirdLeft->Add( new wxStaticText( pPanelMaterials, -1, wxT("Specular")), 0,
													wxALIGN_CENTER | wxALIGN_BOTTOM );
	sizerMaterialsGirdLeft->Add( m_pMaterialSpecular, 1, wxALIGN_CENTER | wxALIGN_TOP  );
	// :: shininess
	sizerMaterialsGirdRight->Add( new wxStaticText( pPanelMaterials, -1, wxT("Shininess")), 0,
													wxALIGN_CENTER | wxALIGN_BOTTOM );
	sizerMaterialsGirdRight->Add( m_pMaterialShininess, 1, wxALIGN_CENTER | wxALIGN_TOP  );
	// :: emission
	sizerMaterialsGirdRight->Add( new wxStaticText( pPanelMaterials, -1, wxT("Emission")), 0,
													wxALIGN_CENTER | wxALIGN_BOTTOM );
	sizerMaterialsGirdRight->Add( m_pMaterialEmission, 1, wxALIGN_CENTER | wxALIGN_TOP  );

	/////////////////////////////////////////
	// populate Structures panel
	// :: surface width
	sizerStructures->Add( new wxStaticText( pPanelStructures, -1, wxT("Surface Width:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerStructures->Add( m_pStructureSurfaceWidth, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: surface height
	sizerStructures->Add( new wxStaticText( pPanelStructures, -1, wxT("Surface Height:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerStructures->Add( m_pStructureSurfaceHeight, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: grid spacer
	sizerStructures->Add( new wxStaticText( pPanelStructures, -1, wxT("Grid Spacer:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerStructures->Add( m_pStructureGridSpacer, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: surface spacer
	sizerStructures->Add( new wxStaticText( pPanelStructures, -1, wxT("Surface Spacer:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerStructures->Add( m_pStructureSurfaceSpacer, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: z factor
	sizerStructures->Add( new wxStaticText( pPanelStructures, -1, wxT("Z Factor:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerStructures->Add( m_pStructureZFactor, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: axis
	sizerStructures->Add( new wxStaticText( pPanelStructures, -1, wxT("Show Axis:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerStructures->Add( m_pShowAxis, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: grid labels
	sizerStructures->Add( new wxStaticText( pPanelStructures, -1, wxT("Grid Labels:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerStructures->Add( m_pStructureGridLabels, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: surface labels
	sizerStructures->Add( new wxStaticText( pPanelStructures, -1, wxT("Surface Labels:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerStructures->Add( m_pStructureSurfaceLabels, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: show projections
	sizerStructures->Add( new wxStaticText( pPanelStructures, -1, wxT("Show Projections:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerStructures->Add( m_pShowProjections, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// set notebook panels
	pPanelLights->SetSizer( sizerLights );
	pNotebook->AddPage( pPanelLights, wxT("Lights") );
	pPanelMaterials->SetSizer( sizerMaterials );
	pNotebook->AddPage( pPanelMaterials, wxT("Materials") );
	pPanelStructures->SetSizer( sizerStructuresPage );
	pNotebook->AddPage( pPanelStructures, wxT("Structures") );

	pTopSizer->Add( pNotebook, 0, wxGROW | wxALL|wxEXPAND, 10 );
	pTopSizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10 );

	pNotebook->Layout();
	SetSizer(pTopSizer);
	SetAutoLayout(TRUE);
	pTopSizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CConfig3DDialog
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CConfig3DDialog::~CConfig3DDialog( ) 
{
	delete( m_pLightingAmbient );
	delete( m_pLightingDiffuse );
	delete( m_pLightingSpecular );
	delete( m_pLightCoordX );
	delete( m_pLightCoordY );
	delete( m_pLightCoordZ );
	// materials
	delete( m_pMaterialAmbient );
	delete( m_pMaterialDiffuse );
	delete( m_pMaterialSpecular );
	delete( m_pMaterialShininess );
	delete( m_pMaterialEmission );
	// structure
	delete( m_pStructureSurfaceWidth );
	delete( m_pStructureSurfaceHeight );
	delete( m_pStructureGridSpacer );
	delete( m_pStructureSurfaceSpacer );
	delete( m_pStructureZFactor );
	delete( m_pStructureGridLabels );
	delete( m_pStructureSurfaceLabels );
	delete( m_pShowAxis );
	delete( m_pShowProjections );
}

////////////////////////////////////////////////////////////////////
// Method:	Set3dView
// Class:	CConfig3DDialog
// Purpose:	set 3d view to handle
// Input:	pointer to 3d view
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CConfig3DDialog::Set3dView( CImage3dBase* p3DView )
{
	wxString strTmp;
	m_p3DView = p3DView;

	//////////////////////////
	// todo :: get back up for cancel !!!
	// :: lights
	m_nLightingAmbient = m_p3DView->m_nLightingAmbient;
	m_nLightingDiffuse = m_p3DView->m_nLightingDiffuse;
	m_nLightingSpecular = m_p3DView->m_nLightingSpecular;
	m_nLightCoordX = m_p3DView->m_nLightCoordX;
	m_nLightCoordY = m_p3DView->m_nLightCoordY;
	m_nLightCoordZ = m_p3DView->m_nLightCoordZ;
	// :: materials
	m_nMaterialAmbient = m_p3DView->m_nMaterialAmbient;
	m_nMaterialDiffuse = m_p3DView->m_nMaterialDiffuse;
	m_nMaterialSpecular = m_p3DView->m_nMaterialSpecular;
	m_nMaterialShininess = m_p3DView->m_nMaterialShininess;
	m_nMaterialEmission = m_p3DView->m_nMaterialEmission;
	// :: structures
	m_nStructureSurfaceWidth = m_p3DView->m_nStructureSurfaceWidth;
	m_nStructureSurfaceHeight = m_p3DView->m_nStructureSurfaceHeight;
	m_nStructureSurfaceDepth = m_p3DView->m_nStructureSurfaceDepth;
	m_nStructureGridSpacer = m_p3DView->m_nStructureGridSpacer;
	m_nStructureSurfaceSpacer = m_p3DView->m_nStructureSurfaceSpacer;
	m_nStructureZFactor = m_p3DView->m_nStructureZFactor;
	m_bStructureGridLabels = m_p3DView->m_bShowGraphGridLabels;
	m_bStructureSurfaceLabels = m_p3DView->m_bStructureSurfaceLabels;

	/////////////////////
	// set my data with info from view
	m_pLightingAmbient->SetValue( m_p3DView->m_nLightingAmbient );
	m_pLightingDiffuse->SetValue( m_p3DView->m_nLightingDiffuse );
	m_pLightingSpecular->SetValue( m_p3DView->m_nLightingSpecular );
	m_pLightCoordX->SetValue( m_p3DView->m_nLightCoordX );
	m_pLightCoordY->SetValue( m_p3DView->m_nLightCoordY );
	m_pLightCoordZ->SetValue( m_p3DView->m_nLightCoordZ );
	//  materials
	m_pMaterialAmbient->SetValue( m_p3DView->m_nMaterialAmbient );
	m_pMaterialDiffuse->SetValue( m_p3DView->m_nMaterialDiffuse );
	m_pMaterialSpecular->SetValue( m_p3DView->m_nMaterialSpecular );
	m_pMaterialShininess->SetValue( m_p3DView->m_nMaterialShininess );
	m_pMaterialEmission->SetValue( m_p3DView->m_nMaterialEmission );
	// structure params
	strTmp.Printf( wxT("%d"), m_p3DView->m_nStructureSurfaceWidth );
	m_pStructureSurfaceWidth->SetValue( strTmp );
	strTmp.Printf( wxT("%d"), m_p3DView->m_nStructureSurfaceHeight );
	m_pStructureSurfaceHeight->SetValue( strTmp );
	strTmp.Printf( wxT("%d"), m_p3DView->m_nStructureGridSpacer );
	m_pStructureGridSpacer->SetValue( strTmp );
	strTmp.Printf( wxT("%d"), m_p3DView->m_nStructureSurfaceSpacer );
	m_pStructureSurfaceSpacer->SetValue( strTmp );
	strTmp.Printf( wxT("%.2f"), m_p3DView->m_nStructureZFactor );
	m_pStructureZFactor->SetValue( strTmp );

	// check boxes
	m_pShowAxis->SetValue( m_p3DView->m_bShowAxis );
	m_pStructureGridLabels->SetValue( m_p3DView->m_bShowGraphGridLabels );
	m_pStructureSurfaceLabels->SetValue( m_p3DView->m_bStructureSurfaceLabels );
	m_pShowProjections->SetValue( m_p3DView->m_bShowProjections );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnScrollChange
// Class:	CConfig3DDialog
// Purpose:	when something in the config has changed
// Input:	pointer to event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CConfig3DDialog::OnScrollChange( wxScrollEvent& pEvent )
{
	// lights
	m_p3DView->m_nLightingAmbient = m_pLightingAmbient->GetValue( );
	m_p3DView->m_nLightingDiffuse = m_pLightingDiffuse->GetValue( );
	m_p3DView->m_nLightingSpecular = m_pLightingSpecular->GetValue( );
	m_p3DView->m_nLightCoordX = m_pLightCoordX->GetValue( );
	m_p3DView->m_nLightCoordY = m_pLightCoordY->GetValue( );
	m_p3DView->m_nLightCoordZ = m_pLightCoordZ->GetValue( );
	// materials
	m_p3DView->m_nMaterialAmbient = m_pMaterialAmbient->GetValue( );
	m_p3DView->m_nMaterialDiffuse = m_pMaterialDiffuse->GetValue( );
	m_p3DView->m_nMaterialSpecular = m_pMaterialSpecular->GetValue( );
	m_p3DView->m_nMaterialShininess = m_pMaterialShininess->GetValue( );
	m_p3DView->m_nMaterialEmission = m_pMaterialEmission->GetValue( );

	m_p3DView->Refresh( FALSE );
	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnCheckChange
// Class:	CConfig3DDialog
// Purpose:	when something in the config has changed
// Input:	pointer to event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CConfig3DDialog::OnCheckChange( wxCommandEvent& pEvent )
{
	m_p3DView->m_bShowGraphGridLabels = m_pStructureGridLabels->GetValue(  );
	m_p3DView->m_bStructureSurfaceLabels = m_pStructureSurfaceLabels->GetValue(  );
	m_p3DView->m_bShowAxis = m_pShowAxis->GetValue(  );
	m_p3DView->m_bShowProjections = m_pShowProjections->GetValue(  );

	// recall to build list
	m_p3DView->m_bNewGridConfigB = 1;
	//m_p3DView->DrawGrid( );

	m_p3DView->Refresh( FALSE );
	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnFocusChange
// Class:	CConfig3DDialog
// Purpose:	when something in the config has changed
// Input:	pointer to event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CConfig3DDialog::OnTextChange( wxCommandEvent& pEvent )
{
	int nId = pEvent.GetId();
	long nVarLong;
	double nFloatVar;

	// get structure text entries
	m_pStructureSurfaceWidth->GetLineText(0).ToLong( &nVarLong );
	m_p3DView->m_nStructureSurfaceWidth = (int) nVarLong;
	m_pStructureSurfaceHeight->GetLineText(0).ToLong( &nVarLong );
	m_p3DView->m_nStructureSurfaceHeight = (int) nVarLong;
	m_pStructureGridSpacer->GetLineText(0).ToLong( &nVarLong );
	m_p3DView->m_nStructureGridSpacer = (int) nVarLong;
	m_pStructureSurfaceSpacer->GetLineText(0).ToLong( &nVarLong );
	m_p3DView->m_nStructureSurfaceSpacer = (int) nVarLong;
	m_pStructureZFactor->GetLineText(0).ToDouble( &nFloatVar );
	m_p3DView->m_nStructureZFactor = (float) nFloatVar;

	// set the surface again - m_bNewGridConfig flag will cause reset image in draw 
	m_p3DView->m_bNewGridConfig = 1;
	m_p3DView->Refresh( FALSE );

//	if( nId == wxID_3D_STRUCTURE_SURFACE_WIDTH_TEXT )
//	{
//	}

	pEvent.Skip();
	return;
}
