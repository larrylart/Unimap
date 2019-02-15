////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// base system libs
#include <vector>
#include <cmath>

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "wx/timer.h"
//#include "wx/glcanvas.h"
#include "wx/math.h"
#include <wx/minifram.h>
#include <wx/image.h>

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

// disabled because this has apparently changed in OpenGL 1.2, so doesn't link
// correctly if this is on...
#ifdef GL_EXT_vertex_array
#undef GL_EXT_vertex_array
#endif

// boos numeric processing includes
#include <boost/numeric/ublas/matrix.hpp>

// local includes
#include "../graphics/tps/linalg3d.h"
#include "../graphics/tps/ludecomposition.h"
#include "../util/func.h"
#include "../config/mainconfig.h"
#include "../image/astroimage.h"
#include "../gui/astro_img_view.h"
#include "../gui/waitdialog.h"
#include "../unimap_worker.h"

using namespace boost::numeric::ublas;

// xpm icons
#include "../../resources/xpm/grid2d.xpm"
#include "../../resources/xpm/grid3d.xpm"
#include "../../resources/xpm/axis.xpm"
#include "../../resources/xpm/small/bmpbtn.xpm"
#include "../../resources/xpm/config_c.xpm"

// main header
#include "imgview3ddistmap.h"

BEGIN_EVENT_TABLE(CImgView3DDistMap, CImage3dBase)
	// add here event handlers
END_EVENT_TABLE()
/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CImgView3DDistMap
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CImgView3DDistMap::CImgView3DDistMap( CDistGridViewDialog *parent, wxWindowID id,
							const wxPoint& pos, const wxSize& size, long style,
							const wxString& name, int* gl_attrib )
							: CImage3dBase( parent, id, pos, size, 
							style|wxFULL_REPAINT_ON_RESIZE|wxWANTS_CHARS, name, gl_attrib )
{
	m_pBaseFrame = parent;

	m_vectGrid = NULL;
	m_nModifiers = 0;
	m_nRegularization = 0.0;
	m_nBendingEnergy = 0.0;
	m_vectControlPoints.clear( );
	m_nControlPoints = 0;

    // Make sure server supports the vertex array extension 
//   char* extensions = (char *) glGetString( GL_EXTENSIONS );
//   if( !extensions || !strstr( extensions, "GL_EXT_vertex_array" ) )
//   {
//       use_vertex_arrays = GL_FALSE;
//   }
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CImgView3DDistMap
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CImgView3DDistMap::~CImgView3DDistMap( )
{
	int i=0;

	// delete allocated grid
	for( i=0; i<m_nStructureSurfaceWidth; i++ ) free( m_vectGrid[i] );
	free( m_vectGrid );
	// clear all control points
	m_vectControlPoints.clear( );
}

/////////////////////////////////////////////////////////////////////
// Method:	CalculateStructures
// Class:	CImgView3DDistMap
// Purpose:	Calculate Thin Plate Spline weights from control points 
//			and build a new height grid by interpolating with them.
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImgView3DDistMap::CalculateStructures( )
{
	// You We need at least 3 points to define a plane
	if( m_vectControlPoints.size() < 3 ) return;

	int w = m_nStructureSurfaceWidth;
	int h = m_nStructureSurfaceHeight;
	unsigned p = m_vectControlPoints.size( );

	// Allocate the matrix and vector
	matrix<double> mtx_l(p+3, p+3);
	matrix<double> mtx_v(p+3, 1);
	matrix<double> mtx_orig_k(p, p);

	// Fill K (p x p, upper left of L) and calculate
	// mean edge length from control points
	// K is symmetrical so we really have to
	// calculate only about half of the coefficients.
	double a = 0.0;
	// for all distortion points
	for ( unsigned i=0; i<p; ++i )
	{
		// and from current point to all remaining
		for ( unsigned j=i+1; j<p; ++j )
		{
			Vec pt_i = m_vectControlPoints[i];
			Vec pt_j = m_vectControlPoints[j];
			pt_i.y = pt_j.y = 0;
			double elen = (pt_i - pt_j).len();
			mtx_l(i,j) = mtx_l(j,i) = mtx_orig_k(i,j) = mtx_orig_k(j,i) = TpsBaseFunc(elen);
			// same for upper & lower tri
			a += elen * 2; 
		}
	}
	a /= (double)(p*p);

	// Fill the rest of L
	for ( unsigned i=0; i<p; ++i )
	{
		// diagonal: reqularization parameters (lambda * a^2)
		mtx_l(i,i) = mtx_orig_k(i,i) = m_nRegularization * (a*a);

		// P (p x 3, upper right)
		mtx_l(i, p+0) = 1.0;
		mtx_l(i, p+1) = m_vectControlPoints[i].x;
		mtx_l(i, p+2) = m_vectControlPoints[i].z;

		// P transposed (3 x p, bottom left)
		mtx_l(p+0, i) = 1.0;
		mtx_l(p+1, i) = m_vectControlPoints[i].x;
		mtx_l(p+2, i) = m_vectControlPoints[i].z;
	}

	// O (3 x 3, lower right)
	for ( unsigned i=p; i<p+3; ++i )
		for ( unsigned j=p; j<p+3; ++j ) mtx_l(i,j) = 0.0;

	// Fill the right hand vector V
	for ( unsigned i=0; i<p; ++i ) mtx_v(i,0) = m_vectControlPoints[i].y;
	mtx_v(p+0, 0) = mtx_v(p+1, 0) = mtx_v(p+2, 0) = 0.0;

	// Solve the linear system "inplace"
	if (0 != LU_Solve(mtx_l, mtx_v))
	{
		//puts( "Singular matrix! Aborting." );
		return;
	}

	// Interpolate grid heights
	for( int x=-w/2; x<w/2; ++x )
	{
		for( int z=-h/2; z<h/2; ++z )
		{
			double _h = mtx_v(p+0, 0) + mtx_v(p+1, 0)*x + mtx_v(p+2, 0)*z;
			Vec pt_i, pt_cur(x,0,z);
			for( unsigned i=0; i<p; ++i )
			{
				pt_i = m_vectControlPoints[i];
				pt_i.y = 0;
				_h += mtx_v(i,0) * TpsBaseFunc( ( pt_i - pt_cur ).len());
			}
			m_vectGrid[x+w/2][z+h/2] = _h;
		}
	}

	// Calc bending energy
	matrix<double> _w( p, 1 );
	for( int i=0; i<p; ++i ) _w(i,0) = mtx_v(i,0);

	matrix<double> be = prod( prod<matrix<double> >( trans(_w), mtx_orig_k ), _w );
	m_nBendingEnergy = be(0,0);

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	DrawStructures
// Class:	CImgView3DDistMap
// Purpose:	draw 3d surface and grid
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImgView3DDistMap::DrawStructures( )
{
	// init grid gl pre-com list 
	if( m_bNewGridConfigB ) 
	{
		CImage3dBase::DrawGrid( );
		m_bNewGridConfigB = 0;
	}

	// first check if to setup again - if config has changed
	if( m_bNewGridConfig )
	{
		///////////
		// SET THREAD ACTION VALUES
		DefCmdAction act;
		act.id = THREAD_ACTION_IMAGE_3D_DISTGRID;
		act.vectObj[0] = (void*) this;
		act.vectObj[1] = (void*) m_pBaseFrame->m_pAstroImage;
		act.vectInt[0] = -2;

		/////////////////
		// show wait dialog
		this->Freeze();
		CWaitDialog waitDialog( m_pBaseFrame, wxT("Wait..."), wxT("Converting Image Data..."), 0, m_pBaseFrame->m_pUnimapWorker, &act );
		waitDialog.ShowModal();
		this->Thaw();

//		SetImageDistGrid( m_pBaseFrame->m_pAstroImage );

		m_bNewGridConfig = 0;
		DrawDistMapSurface( );
	}

	glPushMatrix();

	if( m_bShowSurface ) glCallList( OPENGL_EXEC_LIST_BUILD_DIST_SURFACE_MAP );

	if( m_bMapImageToSurface ) glDisable( GL_TEXTURE_2D );
	glDisable(GL_LIGHTING);

	// draw grid &| axis
	if( m_bShowGraphGrid )
	{
		// build call lists
		if( m_bShowAxis ) glCallList( OPENGL_EXEC_LIST_BUILD_RGB_SURFACE_AXIS );
		glCallList( OPENGL_EXEC_LIST_BUILD_RGB_SURFACE_GRID );
	}

	glPopMatrix();

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	ClearGrid
// Class:	CImgView3DDistMap
// Purpose:	clear grid
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImgView3DDistMap::ClearGrid( )
{
	for( int x=0; x<m_nStructureSurfaceWidth; ++x )
	{
		for( int z=0; z<m_nStructureSurfaceHeight; ++z )
		{
			m_vectGrid[x][z] = 0; 
		}
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	DrawDistMapSurface
// Class:	CImgView3DDistMap
// Purpose:	draw 3d distortion surface 
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImgView3DDistMap::DrawDistMapSurface( )
{
	glNewList( OPENGL_EXEC_LIST_BUILD_DIST_SURFACE_MAP , GL_COMPILE );

	int i=0;

	glBegin( GL_QUADS );

	double w = (double) m_nStructureSurfaceWidth;
	double h = (double) m_nStructureSurfaceHeight;
	//double nTScaleW = 1.0/w;
	//double nTScaleH = 1.0/h;
	double hw = (double) w/2.0;
	double hh = (double) h/2.0;
	// and get the integer version as well
	int ihw = (int) hw;
	int ihh = (int) hh;
	double u = 0.0;
	double v = 0.0;

	// set colors
	static GLfloat color1[] = {0.8, 0.4, 0.1, 1.0};
	static GLfloat color2[] = {0.1, 0.4, 0.9, 1.0};
	//static GLfloat red[] = {1.0, 0.0, 0.0, 1.0};
	//static GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
	//static GLfloat blue[] = {0.0, 0.0, 1.0, 1.0};

	GLfloat mycolor[] = {255, 0, 0, 200};
	float y_range = m_nMaxY-m_nMinY;

	// set materials
	static GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	static GLfloat mat_shininess[] = { 100.0 };
	// for all point in the grid ???
	for ( int x=-ihw; x<ihw-1; ++x )
	{
		for ( int z=-ihh; z<ihh-1; ++z )
		{

			//////////////////
			// if no texture
			if( !m_bMapImageToSurface ) 
			{
				if( m_nControlPoints != 0 )
				{
					CalcRGBFromY( m_vectGrid[x+0+ihw][z+0+ihh], y_range, m_nMinY, mycolor );
					//mycolor[0] = myrgb[0]/255.0;mycolor[1] = myrgb[1]/255.0;mycolor[2] = myrgb[2]/255.0;
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mycolor);
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mycolor);
					glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mycolor);
					//glColor3f( mycolor[0], mycolor[1], mycolor[2] );
				} else
				{
					if ( (x&8)^(z&8) )
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color1);
					else
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color2);

					glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
					glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
				}
			}


			float a[] = { x+0, m_vectGrid[x+0+ihw][z+0+ihh], z+0 };
			float b[] = { x+0, m_vectGrid[x+0+ihw][z+1+ihh], z+1 };
			float c[] = { x+1, m_vectGrid[x+1+ihw][z+1+ihh], z+1 };
			float d[] = { x+1, m_vectGrid[x+1+ihw][z+0+ihh], z+0 };

			#define V_MINUS(A,B) {A[0]-B[0], A[1]-B[1], A[2]-B[2]}
			#define V_CROSS(A,B) \
					{A[1]*B[2]-A[2]*B[1], \
					A[2]*B[0]-A[0]*B[2], \
					A[0]*B[1]-A[1]*B[0]}

			float ab[] = V_MINUS(a,b);
			float cb[] = V_MINUS(c,b);
			float n[] = V_CROSS( cb, ab );

			glNormal3f( n[0],n[1],n[2] );

			if( m_bMapImageToSurface )
			{
				u = ((a[0]-hw)/w)+1; v = ((a[2]-hh)/h)+1;
				glTexCoord2f( u, v );
			}
			glVertex3f( a[0],a[1],a[2] );
		
			if( m_bMapImageToSurface ) 
			{
				u = ((b[0]-hw)/w)+1; v = ((b[2]-hh)/h)+1;
				glTexCoord2f( u, v );
			}
			glVertex3f( b[0],b[1],b[2] );

			if( m_bMapImageToSurface )
			{
				u = ((c[0]-hw)/w)+1; v = ((c[2]-hh)/h)+1;
				glTexCoord2f( u, v );
			}
			glVertex3f( c[0],c[1],c[2] );
	
			if( m_bMapImageToSurface )
			{
				u = ((d[0]-hw)/w)+1; v = ((d[2]-hh)/h)+1;
				glTexCoord2f( u, v );
			}
			glVertex3f( d[0],d[1],d[2] );
		}
	}
	glEnd();

	////////////////////////////
	// Control points
	//int old_sel = selected_cp;
	//if( mouseState[0] == mouseState[1] == mouseState[2] == 0 ) selected_cp = -1;

/******* this is implemented in base class ***** 

	if( m_bShowGraphGrid )
	{
		for( i=0; i < m_vectControlPoints.size(); ++i )
		{
			const Vec& cp = m_vectControlPoints[i];
			if( ( cp - m_vCursorLocation ).len() < 2.0 )
			{
				m_nSelectedCtrlPoint = i;
				//glutSetCursor( GLUT_CURSOR_UP_DOWN );
			}

			glPushMatrix();
			glTranslatef(cp.x, cp.y, cp.z);

			if ( m_nSelectedCtrlPoint == i )
				glColor3ub( 0, 255, 255 );
			else
				glColor3ub( 255, 255, 0 );

			drawSolidSphere(1.0,12,12);
			glPopMatrix();

			glBegin( GL_LINES );
			glVertex3f( cp.x, 0, cp.z );
			glVertex3f( cp.x, cp.y, cp.z );
			glEnd();
		}
	}
*/

	glEndList();

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	SetImageDistGrid
// Class:	CImgView3DDistMap
// Purpose:	set image's distortion grid
// Input:	astro image
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImgView3DDistMap::SetImageDistGrid( CAstroImage* pImage )
{
	int i=0;
	Vec tmpPoint;

	if( m_vectGrid )
	{
		for( i=0; i<m_nStructureSurfaceWidth; i++ ) free( m_vectGrid[i] );
		free( m_vectGrid );
	}

	// calculate scale factor
	//m_nGridWidth = 300;
	m_nScaleToGraph = (double) ( (double) m_nStructureSurfaceWidth/ (double) pImage->m_nWidth);
	m_nStructureSurfaceHeight = (int) round( pImage->m_nHeight*m_nScaleToGraph );

	// allocate grid
	m_vectGrid = (float**) calloc( m_nStructureSurfaceWidth, sizeof(float*) );
	for( i=0; i<m_nStructureSurfaceWidth; i++ ) m_vectGrid[i] = (float*) malloc( sizeof(float)*m_nStructureSurfaceHeight );
	
	double w2 = (double) m_nStructureSurfaceWidth/2.0;
	double h2 = (double) m_nStructureSurfaceHeight/2.0;

	m_vectControlPoints.clear();
	m_nControlPoints = 0;
	m_nMinY = DBL_MAX;
	m_nMaxY = DBL_MIN;

	// add control points
	for( i=0; i<pImage->m_vectDisplacementPoints.size(); i++ )
	{
		tmpPoint.x = w2 - (pImage->m_vectDisplacementPoints[i].x*m_nScaleToGraph);
		tmpPoint.z = h2 - (pImage->m_vectDisplacementPoints[i].y*m_nScaleToGraph);
		//if( pImage->m_vectDistGridZ[y][x] < 5.0 ) continue;
		tmpPoint.y = pImage->m_vectDisplacementPoints[i].z*1.5;
		// add to my local dist points
		m_vectControlPoints.push_back( tmpPoint );

		/////
		// get min max for y
		if( tmpPoint.y > m_nMaxY ) m_nMaxY = (double) tmpPoint.y;
		if( tmpPoint.y < m_nMinY ) m_nMinY = (double) tmpPoint.y;

	}

	m_nControlPoints = m_vectControlPoints.size();

	ClearGrid( );
	CalculateStructures( );

	return;
}

/////////////////////////////////////////////////////////////////////
void CImgView3DDistMap::SetImageView( )
{
	SetCurrent( );

	// build call lists
	DrawGrid( );
	DrawAxis( );

	// build call list
	DrawDistMapSurface( );
}

// class :: CDistGridViewDialog
///////////////////////////////////////////////////////////////////
// event handlers
BEGIN_EVENT_TABLE(CDistGridViewDialog, wxMiniFrame)
	EVT_TOOL( wxID_BUTTON_3D_GRAPH_SURFACE_A, CDistGridViewDialog::OnToolButton )
	EVT_TOOL( wxID_BUTTON_3D_SURFACE_GRID, CDistGridViewDialog::OnToolButton )
	EVT_TOOL( wxID_BUTTON_3D_GRAPH_GRID, CDistGridViewDialog::OnToolButton )
	EVT_TOOL( wxID_BUTTON_3D_SURFACE_IMAGE, CDistGridViewDialog::OnToolButton )
	EVT_CHOICE( wxID_GRID_TYPE_SELECT, CDistGridViewDialog::OnToolProcType )
	EVT_TOOL( wxID_COLOR_3D_CONFIG_A, CDistGridViewDialog::OnConfig )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:		Constructor
// Class:		CDistGridViewDialog
// Purpose:		Define a constructor for my robot view
// Input:		pointer to frame & position
// Output:		nothing
////////////////////////////////////////////////////////////////////
CDistGridViewDialog::CDistGridViewDialog( wxWindow *parent, const wxString& strTitle, CUnimapWorker* pUnimapWorker ):
 											wxMiniFrame(parent, -1, strTitle, wxPoint( -1, -1 ), 
											wxSize( 600, 400 ), wxDEFAULT_FRAME_STYLE  )
{
	m_pUnimapWorker = pUnimapWorker;
	m_nSelectId = 0;

	SetMinSize( wxSize( 300, 200 ) );

	wxString vectGridTypeSelect[2];

	vectGridTypeSelect[0] = wxT("Distortion");
	vectGridTypeSelect[1] = wxT("Displacement");

	// create status bar
	int nWidths[3] =  { -10, 200, 23 };
	int nStyle[3] =  { wxSB_FLAT, wxSB_NORMAL, wxSB_FLAT }; 
	CreateStatusBar( 3, wxST_SIZEGRIP, -1 );
	GetStatusBar()->SetStatusWidths( 3, nWidths );
	GetStatusBar()->SetStatusStyles( 3, nStyle );

	// tool bar
	wxToolBar* pToolBar = CreateToolBar( );
	wxBitmap bmpViewSurface = wxBitmap( grid3d_xpm );
	wxBitmap bmpViewSurfaceGrid = wxBitmap( grid2d_xpm );
	wxBitmap bmpViewGrid = wxBitmap( axis_xpm );
	wxBitmap bmpTexture = wxBitmap( magick_xpm );
	wxBitmap bmpConfig = wxBitmap( config_xpm );

	// get toolbar size
    int wt = pToolBar->GetToolBitmapSize().GetWidth(),
        ht = pToolBar->GetToolBitmapSize().GetHeight();
	// resize icons to toolbar size
	bmpViewSurface = wxBitmap(bmpViewSurface.ConvertToImage().Scale(wt, ht));
	bmpViewSurfaceGrid = wxBitmap(bmpViewSurfaceGrid.ConvertToImage().Scale(wt, ht));
	bmpViewGrid = wxBitmap(bmpViewGrid.ConvertToImage().Scale(wt, ht));
	bmpTexture = wxBitmap(bmpTexture.ConvertToImage().Scale(wt, ht));
	bmpConfig = wxBitmap(bmpConfig.ConvertToImage().Scale(wt, ht));

	// 3d graph control buttons
	pToolBar->AddTool( wxID_BUTTON_3D_GRAPH_SURFACE_A, wxT("Surface"), bmpViewSurface, wxT("Show Surface"), wxITEM_CHECK );
	pToolBar->AddTool( wxID_BUTTON_3D_SURFACE_GRID, wxT("SurfaceGrid"), bmpViewSurfaceGrid, wxT("Surface Grid"), wxITEM_CHECK );
	pToolBar->AddTool( wxID_BUTTON_3D_GRAPH_GRID, wxT("GraphGrid"), bmpViewGrid, wxT("Graph Grid"), wxITEM_CHECK );
	pToolBar->AddTool( wxID_BUTTON_3D_SURFACE_IMAGE, wxT("Image"), bmpTexture, wxT("Apply Image"), wxITEM_CHECK );
	pToolBar->AddSeparator();
	pToolBar->ToggleTool( wxID_BUTTON_3D_GRAPH_GRID, 1 );
	pToolBar->ToggleTool( wxID_BUTTON_3D_GRAPH_SURFACE_A, 1 );

	m_pGridTypeSelect = new wxChoice( pToolBar, wxID_GRID_TYPE_SELECT,
										wxPoint(420, 20),
										wxSize(120,-1), 2, vectGridTypeSelect,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	m_pGridTypeSelect->SetSelection( 0 );
	pToolBar->AddControl( new wxStaticText( pToolBar, -1, wxT("Calc Type:")) );
	pToolBar->AddControl( m_pGridTypeSelect );
	pToolBar->AddSeparator();
	// :: config button
	pToolBar->AddTool( wxID_COLOR_3D_CONFIG_A, wxT("Config"), bmpConfig, wxT("3d Config"), wxITEM_CHECK );

	// main dialog sizer
	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
//	wxFlexGridSizer* topsizer = new wxFlexGridSizer( 3, 1, 50, 5 );
	topsizer->SetMinSize( wxSize( 300, 200 ) );

	//////////////////
	// build image canvas
	int *gl_attrib = NULL;
	m_pImgView3D = new CImgView3DDistMap( this, wxID_ANY, wxDefaultPosition, wxSize(600,400), 0, wxT("3dview"), gl_attrib );
	// set static box
	wxStaticBox *pImage3DBox = new wxStaticBox( this, -1, wxT("3d view"), wxDefaultPosition, wxDefaultSize );
	wxStaticBoxSizer *pImage3DSizer = new wxStaticBoxSizer( pImage3DBox, wxHORIZONTAL );
	topsizer->Add( pImage3DSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );
	// set to sizer
	pImage3DSizer->Add( m_pImgView3D, 1, wxCAPTION |wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );
	m_pImgView3D->SetAutoLayout( 1 );
	// init 
//	m_pImgView3D->SetCurrent( );
//	m_pImgView3D->Init( );

//	topsizer->Add( m_pImgView3D, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 5  );

	// bring out the tool
	pToolBar->Realize();


	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);

	Centre( wxBOTH );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CDistGridViewDialog
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CDistGridViewDialog::~CDistGridViewDialog( )
{
	m_pAstroImage->ClearDisplacementPoints( );
	delete( m_pGridTypeSelect );
	delete( m_pImgView3D );
}

////////////////////////////////////////////////////////////////////
// Method:	SetAstroImage
// Class:	CDistGridViewDialog
// Purose:	destroy my object
// Input:	pointer to astro image
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CDistGridViewDialog::SetAstroImage( CAstroImage* pImage )
{
	m_pAstroImage = pImage;
	m_pImgView3D->SetImageDistGrid( m_pAstroImage );
}

////////////////////////////////////////////////////////////////////
// Method:		OnToolButton
// Class:		CDistGridViewDialog
// Purpose:		set type of surface gird-line flat
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CDistGridViewDialog::OnToolButton( wxCommandEvent& pEvent )
{
	int nButtonId = pEvent.GetId();

	if( pEvent.IsChecked() )
	{
		if( nButtonId == wxID_BUTTON_3D_GRAPH_SURFACE_A )
			m_pImgView3D->m_bShowSurface = 1;
		else if( nButtonId == wxID_BUTTON_3D_SURFACE_GRID )
			m_pImgView3D->m_nSurfaceFillMode = GL_LINE;
		else if( nButtonId == wxID_BUTTON_3D_SURFACE_IMAGE )
		{
			if( !m_pImgView3D->m_bImageTestureInitialized ) m_pImgView3D->InitTextureMapping( *(m_pImageView->m_pImage) );
			m_pImgView3D->m_bMapImageToSurface = 1;

		} else if( nButtonId == wxID_BUTTON_3D_GRAPH_GRID )
			m_pImgView3D->m_bShowGraphGrid = 1;


	} else
	{
		if( nButtonId == wxID_BUTTON_3D_GRAPH_SURFACE_A )
			m_pImgView3D->m_bShowSurface = 0;
		else if( nButtonId == wxID_BUTTON_3D_SURFACE_GRID )
			m_pImgView3D->m_nSurfaceFillMode = GL_FILL;
		else if( nButtonId == wxID_BUTTON_3D_SURFACE_IMAGE )
		{
			m_pImgView3D->m_bMapImageToSurface = 0;
			m_pImgView3D->CloseTextureMapping( );

		} else if( nButtonId == wxID_BUTTON_3D_GRAPH_GRID )
			m_pImgView3D->m_bShowGraphGrid = 0;
	}

	m_pImgView3D->DrawDistMapSurface( );

	m_pImgView3D->Refresh(false);

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnToolProcType
// Class:		CDistGridViewDialog
// Purpose:		set type of grid proc data 
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CDistGridViewDialog::OnToolProcType( wxCommandEvent& pEvent )
{
	m_nSelectId = m_pGridTypeSelect->GetSelection();

	///////////
	// SET THREAD ACTION VALUES
	DefCmdAction act;
	act.id = THREAD_ACTION_IMAGE_3D_DISTGRID;
	act.vectObj[0] = (void*) m_pImgView3D;
	act.vectObj[1] = (void*) m_pAstroImage;
	act.vectInt[0] = m_nSelectId;

	/////////////////
	// show wait dialog
	CWaitDialog waitDialog( this, wxT("Wait..."), wxT("Converting Image Data..."), 0, m_pUnimapWorker, &act );
	waitDialog.ShowModal();

/*	if( m_nSelectId == 0 )
	{

		m_pAstroImage->ClearDisplacementPoints( );
		m_pAstroImage->CalcDisplacementPointsByDist( );

	} else if( nSelectId == 1 )
	{
		m_pAstroImage->ClearDisplacementPoints( );
		m_pAstroImage->CalcDisplacementPointsByPos( );
	}

	m_pImgView3D->SetImageDistGrid( m_pAstroImage );
	m_pImgView3D->CalculateStructures( );
*/

	m_pImgView3D->DrawDistMapSurface( );

	m_pImgView3D->Refresh(false);

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnConfig
// Class:		CDistGridViewDialog
// Purpose:		dialog to configure 3d view
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CDistGridViewDialog::OnConfig( wxCommandEvent& pEvent )
{
	CConfig3DDialog* pDialog = new CConfig3DDialog( this, wxT("3D View Configuration") );
	pDialog->Set3dView( m_pImgView3D );

	// if not ok - restore the original config
	if( pDialog->ShowModal() != wxID_OK )
	{
		// :: lights
		m_pImgView3D->m_nLightingAmbient = pDialog->m_nLightingAmbient;
		m_pImgView3D->m_nLightingDiffuse = pDialog->m_nLightingDiffuse;
		m_pImgView3D->m_nLightingSpecular = pDialog->m_nLightingSpecular;
		m_pImgView3D->m_nLightCoordX = pDialog->m_nLightCoordX;
		m_pImgView3D->m_nLightCoordY = pDialog->m_nLightCoordY;
		m_pImgView3D->m_nLightCoordZ = pDialog->m_nLightCoordZ;
		// :: materials
		m_pImgView3D->m_nMaterialAmbient = pDialog->m_nMaterialAmbient;
		m_pImgView3D->m_nMaterialDiffuse = pDialog->m_nMaterialDiffuse;
		m_pImgView3D->m_nMaterialSpecular = pDialog->m_nMaterialSpecular;
		m_pImgView3D->m_nMaterialShininess = pDialog->m_nMaterialShininess;
		m_pImgView3D->m_nMaterialEmission = pDialog->m_nMaterialEmission;
		// :: structures
		m_pImgView3D->m_nStructureSurfaceWidth = pDialog->m_nStructureSurfaceWidth;
		m_pImgView3D->m_nStructureSurfaceHeight = pDialog->m_nStructureSurfaceHeight;
		m_pImgView3D->m_nStructureSurfaceDepth = pDialog->m_nStructureSurfaceDepth;
		m_pImgView3D->m_nStructureGridSpacer = pDialog->m_nStructureGridSpacer;
		m_pImgView3D->m_nStructureSurfaceSpacer = pDialog->m_nStructureSurfaceSpacer;
		m_pImgView3D->m_nStructureZFactor = pDialog->m_nStructureZFactor;
		m_pImgView3D->m_bShowAxis = pDialog->m_bShowAxis;
		m_pImgView3D->m_bShowGraphGridLabels = pDialog->m_bStructureGridLabels;
		m_pImgView3D->m_bStructureSurfaceLabels = pDialog->m_bStructureSurfaceLabels;

	} else
	{
		// get structure text entries
		long nVarLong=0;
		double nFloatVar=0.0;
		pDialog->m_pStructureSurfaceWidth->GetLineText(0).ToLong( &nVarLong );
		m_pImgView3D->m_nStructureSurfaceWidth = (int) nVarLong;
		pDialog->m_pStructureSurfaceHeight->GetLineText(0).ToLong( &nVarLong );
		m_pImgView3D->m_nStructureSurfaceHeight = (int) nVarLong;
		pDialog->m_pStructureGridSpacer->GetLineText(0).ToLong( &nVarLong );
		m_pImgView3D->m_nStructureGridSpacer = (int) nVarLong;
		pDialog->m_pStructureSurfaceSpacer->GetLineText(0).ToLong( &nVarLong );
		m_pImgView3D->m_nStructureSurfaceSpacer = (int) nVarLong;
		pDialog->m_pStructureZFactor->GetLineText(0).ToDouble( &nFloatVar );
		m_pImgView3D->m_nStructureZFactor = (float) nFloatVar;

		////////////////////////////
		// set main config
		// :: lights
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_LIGHTING_AMBIENT, m_pImgView3D->m_nLightingAmbient );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_LIGHTING_DIFFUSE, m_pImgView3D->m_nLightingDiffuse );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_LIGHTING_SPECULAR, m_pImgView3D->m_nLightingSpecular );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_LIGHT_COORD_X, m_pImgView3D->m_nLightCoordX );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_LIGHT_COORD_Y, m_pImgView3D->m_nLightCoordY );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_LIGHT_COORD_Z, m_pImgView3D->m_nLightCoordZ );
		// :: materials
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_MATERIAL_AMBIENT, m_pImgView3D->m_nMaterialAmbient );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_MATERIAL_DIFFUSE, m_pImgView3D->m_nMaterialDiffuse );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_MATERIAL_SPECULAR, m_pImgView3D->m_nMaterialSpecular );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_MATERIAL_SHININESS, m_pImgView3D->m_nMaterialShininess );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_MATERIAL_EMISSION, m_pImgView3D->m_nMaterialEmission );
		// :: structures
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_SURFACE_WIDTH, m_pImgView3D->m_nStructureSurfaceWidth );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_SURFACE_HEIGHT, m_pImgView3D->m_nStructureSurfaceHeight );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_GRID_SPACER, m_pImgView3D->m_nStructureGridSpacer );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_SURFACE_SPACER, m_pImgView3D->m_nStructureSurfaceSpacer );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_Z_FACTOR, (float) m_pImgView3D->m_nStructureZFactor );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_GRID_LABELS, m_pImgView3D->m_bShowGraphGridLabels );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_SURFACE_LABELS, m_pImgView3D->m_bStructureSurfaceLabels );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_SHOW_AXIS, m_pImgView3D->m_bShowAxis );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_SHOW_PROJECTIONS, m_pImgView3D->m_bShowProjections );

		// save
		m_pMainConfig->Save( );
	}

	// update & refresh
	pDialog->m_p3DView->m_bNewGridConfig = 1;
	pDialog->m_p3DView->Refresh( FALSE );

	GetToolBar()->ToggleTool( wxID_COLOR_3D_CONFIG_A, 0 );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SetConfig
// Class:		CDistGridViewDialog
// Purpose:		set main config from main config object
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CDistGridViewDialog::SetConfig( CConfigMain* pConfig )
{
	m_pMainConfig = pConfig;
	int bState = 1;

	/////////////////////////////////////////
	// load config
	// :: light
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_LIGHTING_AMBIENT, m_pImgView3D->m_nLightingAmbient, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_LIGHTING_DIFFUSE, m_pImgView3D->m_nLightingDiffuse, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_LIGHTING_SPECULAR, m_pImgView3D->m_nLightingSpecular, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_LIGHT_COORD_X, m_pImgView3D->m_nLightCoordX, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_LIGHT_COORD_Y, m_pImgView3D->m_nLightCoordY, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_LIGHT_COORD_Z, m_pImgView3D->m_nLightCoordZ, 1 );
	// :: materials
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_MATERIAL_AMBIENT, m_pImgView3D->m_nMaterialAmbient, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_MATERIAL_DIFFUSE, m_pImgView3D->m_nMaterialDiffuse, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_MATERIAL_SPECULAR, m_pImgView3D->m_nMaterialSpecular, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_MATERIAL_SHININESS, m_pImgView3D->m_nMaterialShininess, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_MATERIAL_EMISSION, m_pImgView3D->m_nMaterialEmission, 1 );
	// :: structure
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_SURFACE_WIDTH, m_pImgView3D->m_nStructureSurfaceWidth, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_SURFACE_HEIGHT, m_pImgView3D->m_nStructureSurfaceHeight, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_GRID_SPACER, m_pImgView3D->m_nStructureGridSpacer, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_SURFACE_SPACER, m_pImgView3D->m_nStructureSurfaceSpacer, 1 );
	bState = m_pMainConfig->GetFloatVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_Z_FACTOR, m_pImgView3D->m_nStructureZFactor, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_GRID_LABELS, m_pImgView3D->m_bShowGraphGridLabels, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_SURFACE_LABELS, m_pImgView3D->m_bStructureSurfaceLabels, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_SHOW_AXIS, m_pImgView3D->m_bShowAxis, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_DIST_STRUCT_SHOW_PROJECTIONS, m_pImgView3D->m_bShowProjections, 1 );

	if( !bState ) m_pMainConfig->Save( );

	return;
}
