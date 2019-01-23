

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

// opencv includes
#include "cv.h"
#include "highgui.h"

#include "wx/timer.h"
//#include "wx/glcanvas.h"
#include "wx/math.h"
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

//#include <boost/numeric/ublas/matrix.hpp>

//#include "../graphics/tps/vec.h"
#include "../graphics/tps/linalg3d.h"
//#include "../graphics/tps/ludecomposition.h"

class Mtx;
class Vec;

// local includes
#include "../util/func.h"
#include "../config/mainconfig.h"
#include "../image/astroimage.h"
#include "../image/imagefft.h"
#include "../gui/astro_img_view.h"
#include "../gui/image3dbase.h"
#include "../gui/waitdialog.h"
#include "../unimap_worker.h"

// xpm icons
#include "../cresources/grid2d.xpm"
#include "../cresources/grid3d.xpm"
#include "../cresources/axis.xpm"
#include "../cresources/small/bmpbtn.xpm"
#include "../cresources/swap_layer.xpm"
#include "../cresources/config_c.xpm"

// main header
#include "imgview3dcolormap.h"

BEGIN_EVENT_TABLE(CImgView3DColMap, CImage3dBase)
	// specific implementations
END_EVENT_TABLE()
/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CImgView3DColMap
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CImgView3DColMap::CImgView3DColMap( CColorMapViewDialog *parent, wxWindowID id,
							const wxPoint& pos, const wxSize& size, long style,
							const wxString& name, int* gl_attrib )
							: CImage3dBase( parent, id, pos, size, 
							style|wxFULL_REPAINT_ON_RESIZE|wxWANTS_CHARS, name, gl_attrib )
{
	m_pBaseFrame = parent;

	// clear my control vector points
	m_vectPoints.clear();
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CImgView3DColMap
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CImgView3DColMap::~CImgView3DColMap( )
{
	// delete my points used
	m_vectPoints.clear();
}

/////////////////////////////////////////////////////////////////////
// Method:	DrawGrid
// Class:	CImgView3DColMap
// Purpose:	draw 3d grid by type
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImgView3DColMap::DrawGrid( )
{
	if( m_nViewType == COLOR_3D_PROFILER_TYPE_3D_DISTRIBUTION )
	{
		glEnable( GL_COLOR_MATERIAL );
		BuildRGBCube( );

	} else
	{
		CImage3dBase::DrawGrid( );
	}

	// reset flag
	m_bNewGridConfigB = 0;

	return;
}
/////////////////////////////////////////////////////////////////////
// Method:	DrawStructures
// Class:	CImgView3DColMap
// Purpose:	draw 3d surface and grid
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImgView3DColMap::DrawStructures( )
{
	if( m_bNewGridConfigB ) DrawGrid( );

	if( m_nViewType == COLOR_3D_PROFILER_TYPE_NORMAL ||
		m_nViewType == COLOR_3D_PROFILER_TYPE_VIGNETTING || m_nViewType == COLOR_3D_PROFILER_TYPE_FFT )
	{
		// first check if to setup again - if config has changed
		if( m_bNewGridConfig )
		{
			///////////
			// SET THREAD ACTION VALUES
			DefCmdAction act;
			act.id = THREAD_ACTION_IMAGE_3D_COLORMAP;
			act.vectObj[0] = (void*) this;
			act.vectObj[1] = (void*) m_pBaseFrame->m_pAstroImage;
			act.vectObj[2] = (void*) m_pBaseFrame->m_pImageView->m_pImage;
			act.vectInt[0] = m_pBaseFrame->m_nSelectId;
			act.vectInt[1] = m_pBaseFrame->m_nColorId;

			/////////////////
			// show wait dialog
			this->Freeze();
			CWaitDialog waitDialog( m_pBaseFrame, wxT("Wait..."), wxT("Converting Image Data..."), 0, m_pBaseFrame->m_pUnimapWorker, &act );
			waitDialog.ShowModal();
			this->Thaw();

			if( m_nViewType == COLOR_3D_PROFILER_TYPE_FFT )
			{
//				SetImageFFT( m_pBaseFrame->m_pAstroImage, 
//								*(m_pBaseFrame->m_pImageView->m_pImage),
//								m_pBaseFrame->m_nSelectId,
//								m_pBaseFrame->m_nColorId );
			} else
			{
//				SetImageColorMap( m_pBaseFrame->m_pAstroImage, 
//								*(m_pBaseFrame->m_pImageView->m_pImage),
//								m_pBaseFrame->m_nSelectId,
//								m_pBaseFrame->m_nColorId );
				SetImageView();
			}

			m_bNewGridConfig = 0;
			DrawColorMapSurface( );
		}

		glPushMatrix();

		//DrawColorMapSurface( );
		if( m_bShowSurface ) glCallList( OPENGL_EXEC_LIST_BUILD_RGB_SURFACE_MAP );

		if( m_bMapImageToSurface ) glDisable( GL_TEXTURE_2D );
		glDisable(GL_LIGHTING);

		// draw grid &| axis
		if( m_bShowGraphGrid )
		{
			// build call lists
			if( m_bShowAxis ) glCallList( OPENGL_EXEC_LIST_BUILD_RGB_SURFACE_AXIS );
			glCallList( OPENGL_EXEC_LIST_BUILD_RGB_SURFACE_GRID );

			//if( m_bShowAxis ) DrawAxis( );
			//DrawGrid( );
		}

		glPopMatrix();

	} else if( m_nViewType == COLOR_3D_PROFILER_TYPE_3D_DISTRIBUTION )
	{
		// set to color surface ?
		glEnable( GL_COLOR_MATERIAL );

		// cube
		if( m_bShowGraphGrid ) glCallList( OPENGL_EXEC_LIST_BUILD_RGB_CUBE ); 
		// clouds
		if( m_bShowSurface ) glCallList( OPENGL_EXEC_LIST_BUILD_RGB_CLOUD ); 
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	DrawColorMapSurface
// Class:	CImgView3DColMap
// Purpose:	draw 3d surface and grid
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImgView3DColMap::DrawColorMapSurface( )
{
	glNewList( OPENGL_EXEC_LIST_BUILD_RGB_SURFACE_MAP , GL_COMPILE_AND_EXECUTE );

//	SetDrawing( );

	//glPopMatrix();
//	glPushMatrix();

	//glEnable( GL_COLOR_MATERIAL );
	glBegin( GL_QUADS );

	// get width/height
	int w = m_nStructureSurfaceWidth;
	int h = m_nStructureSurfaceHeight;
	// Image surface sizes
	UINT	nx = w-1,
			nz = h-1;
	
	double hw = (double) w/2.0;
	double hh = (double) h/2.0;
	double u = 0.0;
	double v = 0.0;

	// color setup 
	static GLfloat color1[] = {0.1, 0.8, 0.1, 1.0};
	static GLfloat color2[] = {0.8, 0.1, 0.1, 1.0};
	GLfloat mycolor[] = {255, 0, 0, 200};
	//float myrgb[3];
	float y_range = m_nMaxY-m_nMinY;
	// material setup
	static GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	static GLfloat mat_shininess[] = { 100.0 };
	// for all point in the grid ???
	for( UINT z=0, i=0;  z < nz;  z+=m_nStructureSurfaceSpacer, i+=m_nStructureSurfaceSpacer )
	{
		for( UINT x=0;  x < nx;  x+=m_nStructureSurfaceSpacer, i+=m_nStructureSurfaceSpacer )
		{
			//if( ( x&8 )^( z&8 ) )
			//	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color1);
			//else
			//	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color2);

			//////////////////
			// if no texture
			if( !m_bMapImageToSurface ) 
			{
				CalcRGBFromY( m_vectPoints[i].y, y_range, m_nMinY, mycolor );
				//mycolor[0] = myrgb[0]/255.0;mycolor[1] = myrgb[1]/255.0;mycolor[2] = myrgb[2]/255.0;
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mycolor);
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mycolor);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mycolor);
				//glColor3f( mycolor[0], mycolor[1], mycolor[2] );
			}

			//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
			//glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

			///////////////////
			// calculus 
			// Counter Clockwise direction
			
			// Other vertices indices - neighbour points next, down(+width)+1..
			int	j = i + w,	
				k = j+m_nStructureSurfaceSpacer,
				n = i+m_nStructureSurfaceSpacer;

			// Get coordinates of 4 vertices
			float
				xi = m_vectPoints[i].x,
				yi = m_vectPoints[i].y,
				zi = m_vectPoints[i].z,

				xj = m_vectPoints[j].x,
				yj = m_vectPoints[j].y,
				zj = m_vectPoints[j].z,

				xk = m_vectPoints[k].x,
				yk = m_vectPoints[k].y,
				zk = m_vectPoints[k].z,

				xn = m_vectPoints[n].x,
				yn = m_vectPoints[n].y,
				zn = m_vectPoints[n].z,

				// Quad side lines vectors coordinates
				ax = xi-xn,
				ay = yi-yn,

				by = yj-yi,
				bz = zj-zi,

				// Normal vector coordinates
				vx = ay*bz,
				vy = -bz*ax,
				vz = ax*by,

				// Normal vector length
				v  = float( sqrt(vx*vx + vy*vy + vz*vz) );
			
			// Scale to unity
			vx /= v;
			vy /= v;
			vz /= v;

			// Set the normal vector
			glNormal3f (vx,vy,vz);

			// Vertices are given in counter clockwise direction order
			//glColor3f( 0.2f, 0.8f, 1.f );
			if( m_bMapImageToSurface )
			{
				u = ((xi-hw)/w)+1; v = ((zi-hh)/h)+1;
				glTexCoord2f( u, v );
			}
			glVertex3f( xi, yi, zi );

			//glColor3f (0.6f, 0.7f, 1.f);	
			if( m_bMapImageToSurface )
			{
				u = ((xj-hw)/w)+1; v = ((zj-hh)/h)+1;
				glTexCoord2f( u, v );
			}
			glVertex3f (xj, yj, zj);

			//glColor3f( 0.7f, 0.9f, 1.f );			
			if( m_bMapImageToSurface )
			{
				u = ((xk-hw)/w)+1; v = ((zk-hh)/h)+1;
				glTexCoord2f( u, v );
			}
			glVertex3f (xk, yk, zk);

			//glColor3f( 0.7f, 0.8f, 1.f );
			
			if( m_bMapImageToSurface )
			{
				u = ((xn-hw)/w)+1; v = ((zn-hh)/h)+1;
				glTexCoord2f( u, v );
			}
			glVertex3f (xn, yn, zn);
		}
	}

//	glPopMatrix();

	glEnd();

/*	if( m_bMapImageToSurface ) glDisable( GL_TEXTURE_2D );
	glDisable(GL_LIGHTING);

//	glPopMatrix();
//	glPushMatrix();

	// draw grid &| axis
	if( m_bShowGraphGrid )
	{
		// build call lists
		DrawGrid( );
		if( m_bShowAxis ) DrawAxis( );
		//glCallList( OPENGL_EXEC_LIST_BUILD_RGB_SURFACE_AXIS );
	}

	glPopMatrix();
*/
	glEndList();

	return;
}

/////////////////////////////////////////////////////////////////////
// Purpose:	set image's color map
/////////////////////////////////////////////////////////////////////
void CImgView3DColMap::SetImageColorMap( CAstroImage* pAstroImage, 
											wxImage& pImage, int nType, 
											int nChannel, int bInverse )
{
	m_nViewType = nType;

	int i=0, x=0, y=0;
	Point3D nPoint3D;

	m_vectPoints.clear();
	m_nMinY = DBL_MAX;
	m_nMaxY = DBL_MIN;

	int nImgWidth = pImage.GetWidth();
	int nImgHeight = pImage.GetHeight();
	// copy image localy
	//wxImage pLImage = pImage.Blur( 7 );

	// calculate scale factor
	m_nScaleToGraph = (double) ( (double) m_nStructureSurfaceWidth / (double) nImgWidth );
	m_nStructureSurfaceHeight = (int) round( nImgHeight*m_nScaleToGraph );

	int w = m_nStructureSurfaceWidth;
	int h = m_nStructureSurfaceHeight;
	double hw = (double) w/2.0;
	double hh = (double) h/2.0;

	// get an opencv image
	IplImage* pIplImgF = WxImageToIplImage( pImage );
        
	// apply filter - blur types or  distance ?
	// blur methods : CV_BLUR, CV_GAUSSIAN, CV_MEDIAN, CV_BILATERAL
	if( nType == COLOR_3D_PROFILER_TYPE_NORMAL )
		cvSmooth( pIplImgF, pIplImgF, CV_GAUSSIAN, 21 );
	if( nType == COLOR_3D_PROFILER_TYPE_VIGNETTING )
		cvSmooth( pIplImgF, pIplImgF, CV_GAUSSIAN, 201 );
	
	// resize
	IplImage* pIplImg = cvCreateImage( cvSize(w, h), IPL_DEPTH_8U, 3 ); 
	cvResize( pIplImgF, pIplImg );
	// if vigneting do a second smooth - with a max 255 value
	if( nType == COLOR_3D_PROFILER_TYPE_VIGNETTING ) cvSmooth( pIplImg, pIplImg, CV_GAUSSIAN, 101 );

	RgbImage iplImgT( pIplImg );

	int nX=0, nY=0;
	unsigned char nRed, nGreen, nBlue;
	
	for( y=0; y<h; y++ )
	{
		for( x=0; x<w; x++ )
		{
			nPoint3D.x = (double) x-hw;
			nPoint3D.z = (double) y-hh;
			// calculate z as color
			nRed = iplImgT[y][x].r; //pLImage.GetRed( nX, nY );
			nGreen = iplImgT[y][x].g; //pLImage.GetGreen( nX, nY );
			nBlue = iplImgT[y][x].b; //pLImage.GetBlue( nX, nY );
			
			// by channel type
			if( nChannel == COLOR_3D_CHANNEL_RGB )
				nPoint3D.y = (double) ( ( 0.299*nRed + 0.587*nGreen + 0.114*nBlue ) * 0.5 );
			else if( nChannel == COLOR_3D_CHANNEL_RED )
				nPoint3D.y = (double) nRed*0.5;
			else if( nChannel == COLOR_3D_CHANNEL_GREEN )
				nPoint3D.y = (double) nGreen*0.5;
			else if( nChannel == COLOR_3D_CHANNEL_BLUE )
				nPoint3D.y = (double) nBlue*0.5;
			// push in my vector this point
			m_vectPoints.push_back( nPoint3D );
			/////
			// get min max for y
			if( nPoint3D.y > m_nMaxY ) m_nMaxY = (double) nPoint3D.y;
			if( nPoint3D.y < m_nMinY ) m_nMinY = (double) nPoint3D.y;
		}
	}

	//////////////////////
	// extra processing here
	if( bInverse )
	{
		for( i=0; i<m_vectPoints.size(); i++ )
		{
			m_vectPoints[i].y = (double) fabs( m_nMaxY-m_vectPoints[i].y );
		}
	}

	//pLImage.Destroy( );
	cvReleaseImage( &pIplImgF );
	cvReleaseImage( &pIplImg );

//	SetImageView( );

	return;
}

/////////////////////////////////////////////////////////////////////
void CImgView3DColMap::SetImageView( )
{
	SetCurrent( );

	// build call lists
	DrawGrid( );
	DrawAxis( );

	// create call list
	//InitTextureMapping( m_pBaseFrame->m_pImageView->m_pImage );
	DrawColorMapSurface( );
}

/////////////////////////////////////////////////////////////////////
// Method:	SetImageColorCube
// Class:	CImgView3DColMap
// Purpose:	set image's color distribution in cube
// Input:	astro image, image from view, and rgb channel
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImgView3DColMap::SetImageColorCube( CAstroImage* pAstroImage, 
											wxImage& pImage, int nType, int nChannel )
{
	m_nViewType = nType;

	m_vectPoints.clear();
	m_nMinY = 0.0;
	m_nMaxY = 0.0;
	m_nScaleToGraph = 0.0;

	BuildRGBCube( );
	BuildColorCloud( pImage );

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	SetImageFFT
// Class:	CImgView3DColMap
// Purpose:	set image's color distribution by FFT
// Input:	astro image, image from view, and rgb channel
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImgView3DColMap::SetImageFFT( CAstroImage* pAstroImage, 
										wxImage& pImage, int nType, int nChannel )
{
	m_nViewType = nType;

	int i=0, x=0, y=0;
	Point3D nPoint3D;

	m_vectPoints.clear();
	m_nMinY = DBL_MAX;
	m_nMaxY = DBL_MIN;

	int nImgWidth = pImage.GetWidth();
	int nImgHeight = pImage.GetHeight();
	// copy image localy
	//wxImage pLImage = pImage.Blur( 7 );

	// calculate scale factor
	m_nScaleToGraph = (double) ( (double) m_nStructureSurfaceWidth / (double) nImgWidth );
	m_nStructureSurfaceHeight = (int) round( nImgHeight*m_nScaleToGraph );

	int w = m_nStructureSurfaceWidth;
	int h = m_nStructureSurfaceHeight;
	double hw = (double) w/2.0;
	double hh = (double) h/2.0;

	// create fft object
	CImageFFT* pImgFFT = new CImageFFT( pImage );
	// get my ipl image out
	IplImage* pIplImgF = pImgFFT->GetImageFFT( );
	// get rid of my fft object
	delete( pImgFFT );

        
	// apply filter - blur types or  distance ?
	// blur methods : CV_BLUR, CV_GAUSSIAN, CV_MEDIAN, CV_BILATERAL
	cvSmooth( pIplImgF, pIplImgF, CV_GAUSSIAN, 21 );
		
//	cvSaveImage( "./fft_s.bmp", pIplImgF );

	// resize
	IplImage* pIplImg = cvCreateImage( cvSize(w, h), IPL_DEPTH_8U, 3 ); 
	cvResize( pIplImgF, pIplImg );
	
//	cvSaveImage( "./fft_ss.bmp", pIplImgF );

	RgbImage iplImgT( pIplImg );

	int nX=0, nY=0;
	unsigned char nRed, nGreen, nBlue;
	
	for( y=0; y<h; y++ )
	{
		for( x=0; x<w; x++ )
		{
			nPoint3D.x = (double) x-hw;
			nPoint3D.z = (double) y-hh;
			// calculate z as color
			nRed = iplImgT[y][x].r; 
			nGreen = iplImgT[y][x].g; 
			nBlue = iplImgT[y][x].b; 
			
			// by channel type
			if( nChannel == COLOR_3D_CHANNEL_RGB )
				nPoint3D.y = (double) ( ( 0.299*nRed + 0.587*nGreen + 0.114*nBlue ) * 0.5 );
			else if( nChannel == COLOR_3D_CHANNEL_RED )
				nPoint3D.y = (double) nRed*0.5;
			else if( nChannel == COLOR_3D_CHANNEL_GREEN )
				nPoint3D.y = (double) nGreen*0.5;
			else if( nChannel == COLOR_3D_CHANNEL_BLUE )
				nPoint3D.y = (double) nBlue*0.5;
			// push in my vector this point
			m_vectPoints.push_back( nPoint3D );
			/////
			// get min max for y
			if( nPoint3D.y > m_nMaxY ) m_nMaxY = (double) nPoint3D.y;
			if( nPoint3D.y < m_nMinY ) m_nMinY = (double) nPoint3D.y;
		}
	}

	//////////////////////
	// extra processing here
/*	if( bInverse )
	{
		for( i=0; i<m_vectPoints.size(); i++ )
		{
			m_vectPoints[i].y = (double) fabs( m_nMaxY-m_vectPoints[i].y );
		}
	}
*/

	// release my used images
	cvReleaseImage( &pIplImgF );
	cvReleaseImage( &pIplImg );

	// build call lists
//	DrawGrid( );
//	DrawAxis( );

	// create call list
	//InitTextureMapping( m_pBaseFrame->m_pImageView->m_pImage );
//	DrawColorMapSurface( );

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	BuildRGBCube
// Class:	CImgView3DColMap
// Purpose:	build rgb color cube 
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImgView3DColMap::BuildRGBCube( )
{
	float m_Size = 80.5f;

	GLUquadricObj* pQuadric = gluNewQuadric( );

	glNewList( OPENGL_EXEC_LIST_BUILD_RGB_CUBE , GL_COMPILE_AND_EXECUTE );

	float x = m_Size;

	// RGB cube
	glBegin(GL_LINE_LOOP);
	  glColor3ub(0,0,0);
		glVertex3d(-x,-x,-x);
	  glColor3ub(255,0,0);
		glVertex3d(x,-x,-x);
	  glColor3ub(255,255,0);
		glVertex3d(x,x,-x);
	  glColor3ub(0,255,0);
		glVertex3d(-x,x,-x);
	glEnd();

	glBegin(GL_LINE_LOOP);
	  glColor3ub(0,0,255);
		glVertex3d(-x,-x,x);
	  glColor3ub(255,0,255);
		glVertex3d(x,-x,x);
	  glColor3ub(255,255,255);
		glVertex3d(x,x,x);
	  glColor3ub(0,255,255);
		glVertex3d(-x,x,x);
	glEnd();

	glBegin(GL_LINES);

	  glColor3ub(0,0,0);
		glVertex3d(-x,-x,-x);
	  glColor3ub(0,0,255);
		glVertex3d(-x,-x,x);

	  glColor3ub(255,0,0);
		glVertex3d(x,-x,-x);
	  glColor3ub(255,0,255);
		glVertex3d(x,-x,x);

	  glColor3ub(255,255,0);
		glVertex3d(x,x,-x);
	  glColor3ub(255,255,255);
		glVertex3d(x,x,x);

	  glColor3ub(0,255,0);
		glVertex3d(-x,x,-x);
	  glColor3ub(0,255,255);
		glVertex3d(-x,x,x);

	glEnd();

	// - spheres are like axis kinda 
	if( m_bShowAxis ) 
	{
		// Spheres
		glPolygonMode(GL_FRONT,GL_FILL);
		glPolygonMode(GL_BACK,GL_FILL);
		float radius = 6.1f;

		glPushMatrix();
		glTranslated(-m_Size,-m_Size,-m_Size);
		glColor3ub(0,0,0);
		gluSphere(pQuadric,radius,12,12); 
		glPopMatrix();

		glPushMatrix();
		glTranslated(m_Size,-m_Size,-m_Size);
		glColor3ub(255,0,0);
		gluSphere(pQuadric,radius,12,12); 
		glPopMatrix();

		glPushMatrix();
		glTranslated(-m_Size,m_Size,-m_Size);
		glColor3ub(0,255,0);
		gluSphere(pQuadric,radius,12,12); 
		glPopMatrix();

		glPushMatrix();
		glTranslated(-m_Size,-m_Size,m_Size);
		glColor3ub(0,0,255);
		gluSphere(pQuadric,radius,12,12); 
		glPopMatrix();

		glPushMatrix();
		glTranslated(m_Size,m_Size,-m_Size);
		glColor3ub(255,255,0);
		gluSphere(pQuadric,radius,12,12); 
		glPopMatrix();

		glPushMatrix();
		glTranslated(-m_Size,m_Size,m_Size);
		glColor3ub(0,255,255);
		gluSphere(pQuadric,radius,12,12); 
		glPopMatrix();

		glPushMatrix();
		glTranslated(m_Size,-m_Size,m_Size);
		glColor3ub(255,0,255);
		gluSphere(pQuadric,radius,12,12); 
		glPopMatrix();

		glPushMatrix();
		glTranslated(m_Size,m_Size,m_Size);
		glColor3ub(255,255,255);
		gluSphere(pQuadric,radius,12,12); 
		glPopMatrix();
	}

	glEndList();

	gluDeleteQuadric(pQuadric);
}

/////////////////////////////////////////////////////////////////////
// Method:	BuildColorCloud
// Class:	CImgView3DColMap
// Purpose:	build rgb color cloud 
// Input:	astro image, image from view, and type
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CImgView3DColMap::BuildColorCloud( wxImage& pImage )
{
	float m_Size = 80.5f;
	// Image area
	unsigned int area = pImage.GetWidth( )*pImage.GetHeight( );

	// Need valid image (24 bits)
	//if( area == 0 || pImage.GetDepth() != 24 ) return;

	// Maximum -> area distinct colors / 2^24
	// This table is memory expansive, but short lived
	unsigned char *pTable = new unsigned char[16777216];
	// init 0
	memset( pTable, 0, 16777216 ); 

	// Image data
	unsigned int wb32 = pImage.GetWidth(); //pImage.GetWidthByte32();
	unsigned char *pData = pImage.GetData();

	// Build a new list
	int nb = 0;
	// set execution list 
	glNewList( OPENGL_EXEC_LIST_BUILD_RGB_CLOUD, GL_COMPILE_AND_EXECUTE );

	glBegin( GL_POINTS );
	float tmp = 2.0f/255.0f*m_Size;
	for( unsigned int j=0;j<pImage.GetHeight();j++ )
	{
		for( unsigned int i=0;i<pImage.GetWidth();i++ )
		{
			unsigned char b = pData[wb32*j+3*i];
			unsigned char g = pData[wb32*j+3*i+1];
			unsigned char r = pData[wb32*j+3*i+2];
			if( !pTable[b*65536+g*256+r] )
			{
				glColor3ub(r,g,b);
				float x = -m_Size+(float)r*tmp;
				float y = -m_Size+(float)g*tmp;
				float z = -m_Size+(float)b*tmp;
				glVertex3d( x, y, z );
				pTable[b*65536+g*256+r] = 1;
				nb++;
			}
		}
	}
	glEnd();

	// end of my execution list
	glEndList();
	// delete allocated
	delete [] pTable;

	return;
}

// class :: CColorMapViewDialog
///////////////////////////////////////////////////////////////////
// event handlers
BEGIN_EVENT_TABLE(CColorMapViewDialog, wxMiniFrame)
	EVT_TOOL( wxID_BUTTON_3D_GRAPH_SURFACE, CColorMapViewDialog::OnToolButton )
	EVT_TOOL( wxID_BUTTON_3D_SURFACE_GRID_B, CColorMapViewDialog::OnToolButton )
	EVT_TOOL( wxID_BUTTON_3D_GRAPH_GRID_B, CColorMapViewDialog::OnToolButton )
	EVT_TOOL( wxID_BUTTON_3D_SURFACE_IMAGE_INVERSE, CColorMapViewDialog::OnToolButton )
	EVT_TOOL( wxID_BUTTON_3D_SURFACE_IMAGE_B, CColorMapViewDialog::OnToolButton )
	EVT_CHOICE( wxID_COLOR_3D_GRAPH_TYPE_SELECT, CColorMapViewDialog::OnToolProcType )
	EVT_CHOICE( wxID_COLOR_3D_CHANNEL_TYPE_SELECT, CColorMapViewDialog::OnToolProcType )
	EVT_TOOL( wxID_COLOR_3D_CONFIG, CColorMapViewDialog::OnConfig )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:		Constructor
// Class:		CColorMapViewDialog
// Purpose:		Define a constructor for my robot view
// Input:		pointer to frame & position
// Output:		nothing
////////////////////////////////////////////////////////////////////
CColorMapViewDialog::CColorMapViewDialog( wxWindow *parent, const wxString& strTitle, CUnimapWorker* pUnimapWorker ):
 											wxMiniFrame(parent, -1, strTitle, wxPoint( -1, -1 ), 
											wxSize( 600, 400 ), wxDEFAULT_FRAME_STYLE  )
{
	m_pUnimapWorker = pUnimapWorker;

	SetMinSize( wxSize( 300, 200 ) );

	m_nSelectId = 0;
	m_nColorId = 0;

	wxString vectGraphTypeSelect[4];
	wxString vectColorTypeSelect[4];

	// :: graph type
	vectGraphTypeSelect[0] = wxT("Color Map");
	vectGraphTypeSelect[1] = wxT("FFT Map");
	vectGraphTypeSelect[2] = wxT("Vignetting");
	vectGraphTypeSelect[3] = wxT("Distribution");
	// :: color
	vectColorTypeSelect[0] = wxT("RGB");
	vectColorTypeSelect[1] = wxT("Red");
	vectColorTypeSelect[2] = wxT("Green");
	vectColorTypeSelect[3] = wxT("Blue");

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
	wxBitmap bmpInverse = wxBitmap( swap_layer_xpm );
	wxBitmap bmpConfig = wxBitmap( config_xpm );

	// get toolbar size
    int wt = pToolBar->GetToolBitmapSize().GetWidth(),
        ht = pToolBar->GetToolBitmapSize().GetHeight();
	// resize icons to toolbar size
	bmpViewSurface = wxBitmap(bmpViewSurface.ConvertToImage().Scale(wt, ht));
	bmpViewSurfaceGrid = wxBitmap(bmpViewSurfaceGrid.ConvertToImage().Scale(wt, ht));
	bmpViewGrid = wxBitmap(bmpViewGrid.ConvertToImage().Scale(wt, ht));
	bmpTexture = wxBitmap(bmpTexture.ConvertToImage().Scale(wt, ht));
	bmpInverse = wxBitmap(bmpInverse.ConvertToImage().Scale(wt, ht));
	bmpConfig = wxBitmap(bmpConfig.ConvertToImage().Scale(wt, ht));

	// 3d graph control buttons
	pToolBar->AddTool( wxID_BUTTON_3D_GRAPH_SURFACE, wxT("Surface"), bmpViewSurface, wxT("Show Surface"), wxITEM_CHECK );
	pToolBar->AddTool( wxID_BUTTON_3D_SURFACE_GRID_B, wxT("SurfaceGrid"), bmpViewSurfaceGrid, wxT("Surface Grid"), wxITEM_CHECK );
	pToolBar->AddTool( wxID_BUTTON_3D_GRAPH_GRID_B, wxT("GraphGrid"), bmpViewGrid, wxT("Graph Grid"), wxITEM_CHECK );
	pToolBar->AddTool( wxID_BUTTON_3D_SURFACE_IMAGE_INVERSE, wxT("Image"), bmpInverse, wxT("Apply Image"), wxITEM_CHECK );
	pToolBar->AddTool( wxID_BUTTON_3D_SURFACE_IMAGE_B, wxT("Inverse"), bmpTexture, wxT("Inverse Color"), wxITEM_CHECK );
	pToolBar->AddSeparator();
	pToolBar->ToggleTool( wxID_BUTTON_3D_GRAPH_GRID_B, 1 );
	pToolBar->ToggleTool( wxID_BUTTON_3D_GRAPH_SURFACE, 1 );
	// :: graph type
	m_pGraphTypeSelect = new wxChoice( pToolBar, wxID_COLOR_3D_GRAPH_TYPE_SELECT,
										wxPoint(420, 20),
										wxSize(80,-1), 4, vectGraphTypeSelect,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	m_pGraphTypeSelect->SetSelection( 0 );
	pToolBar->AddControl( new wxStaticText( pToolBar, -1, wxT("Graph:")) );
	pToolBar->AddControl( m_pGraphTypeSelect );
	pToolBar->AddSeparator();
	// :: color channel
	m_pColorTypeSelect = new wxChoice( pToolBar, wxID_COLOR_3D_CHANNEL_TYPE_SELECT,
										wxPoint(420, 20),
										wxSize(60,-1), 4, vectColorTypeSelect,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	m_pColorTypeSelect->SetSelection( 0 );
	pToolBar->AddControl( new wxStaticText( pToolBar, -1, wxT("Channel:")) );
	pToolBar->AddControl( m_pColorTypeSelect );
	pToolBar->AddSeparator();
	// :: config button
	pToolBar->AddTool( wxID_COLOR_3D_CONFIG, wxT("Config"), bmpConfig, wxT("3d Config"), wxITEM_CHECK );

	// main dialog sizer
	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
//	wxFlexGridSizer* topsizer = new wxFlexGridSizer( 3, 1, 50, 5 );
	topsizer->SetMinSize( wxSize( 300, 200 ) );

	//////////////////
	// build image canvas
	int *gl_attrib = NULL;
	m_pImgView3D = new CImgView3DColMap( this, wxID_ANY, wxDefaultPosition, wxSize(600,400), 0, wxT("3dview"), gl_attrib );
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
// Class:	CColorMapViewDialog
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CColorMapViewDialog::~CColorMapViewDialog( )
{
	//m_pAstroImage->ClearDisplacementPoints( );
	delete( m_pGraphTypeSelect );
	delete( m_pColorTypeSelect );
	delete( m_pImgView3D );
}

////////////////////////////////////////////////////////////////////
// Purose:	set astro image and calculation 
////////////////////////////////////////////////////////////////////
void CColorMapViewDialog::SetAstroImage( CAstroImage* pImage )
{
	m_pAstroImage = pImage;
	m_pImgView3D->SetImageColorMap( m_pAstroImage, *(m_pImageView->m_pImage), 
									COLOR_3D_PROFILER_TYPE_NORMAL, COLOR_3D_CHANNEL_RGB );
}

////////////////////////////////////////////////////////////////////
// Method:		OnToolButton
// Class:		CColorMapViewDialog
// Purpose:		set type of surface gird-line flat
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CColorMapViewDialog::OnToolButton( wxCommandEvent& pEvent )
{
	int nButtonId = pEvent.GetId();
	m_nSelectId = m_pGraphTypeSelect->GetSelection();
	m_nColorId = m_pColorTypeSelect->GetSelection();

	if( pEvent.IsChecked() )
	{
		if( nButtonId == wxID_BUTTON_3D_GRAPH_SURFACE )
			m_pImgView3D->m_bShowSurface = 1;
		else if( nButtonId == wxID_BUTTON_3D_SURFACE_GRID_B )
			m_pImgView3D->m_nSurfaceFillMode = GL_LINE;
		else if( nButtonId == wxID_BUTTON_3D_SURFACE_IMAGE_B )
		{
			// create texture
			if( !m_pImgView3D->m_bImageTestureInitialized ) m_pImgView3D->InitTextureMapping( *(m_pImageView->m_pImage) );
			// set flag
			m_pImgView3D->m_bMapImageToSurface = 1;

		} else if( nButtonId == wxID_BUTTON_3D_SURFACE_IMAGE_INVERSE )
		{
			if( m_nSelectId == COLOR_3D_PROFILER_TYPE_NORMAL || m_nSelectId == COLOR_3D_PROFILER_TYPE_VIGNETTING )
			{
				m_pImgView3D->SetImageColorMap( m_pAstroImage, *(m_pImageView->m_pImage), m_nSelectId, m_nColorId, 1 );
				m_pImgView3D->SetImageView();
			}

		} else if( nButtonId == wxID_BUTTON_3D_GRAPH_GRID_B )
			m_pImgView3D->m_bShowGraphGrid = 1;


	} else
	{
		if( nButtonId == wxID_BUTTON_3D_GRAPH_SURFACE )
			m_pImgView3D->m_bShowSurface = 0;
		else if( nButtonId == wxID_BUTTON_3D_SURFACE_GRID_B )
			m_pImgView3D->m_nSurfaceFillMode = GL_FILL;
		else if( nButtonId == wxID_BUTTON_3D_SURFACE_IMAGE_B )
		{
			m_pImgView3D->m_bMapImageToSurface = 0;
			m_pImgView3D->CloseTextureMapping( );

		} else if( nButtonId == wxID_BUTTON_3D_SURFACE_IMAGE_INVERSE )
		{
			if( m_nSelectId == COLOR_3D_PROFILER_TYPE_NORMAL || m_nSelectId == COLOR_3D_PROFILER_TYPE_VIGNETTING )
			{
				m_pImgView3D->SetImageColorMap( m_pAstroImage, *(m_pImageView->m_pImage), m_nSelectId, m_nColorId );
				m_pImgView3D->SetImageView();
			}

		} else if( nButtonId == wxID_BUTTON_3D_GRAPH_GRID_B )
			m_pImgView3D->m_bShowGraphGrid = 0;
	}

	// recreate call list
	if( m_nSelectId != COLOR_3D_PROFILER_TYPE_3D_DISTRIBUTION ) m_pImgView3D->DrawColorMapSurface( );

	m_pImgView3D->Refresh(false);

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnToolProcType
// Class:		CColorMapViewDialog
// Purpose:		set type of grid proc data 
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CColorMapViewDialog::OnToolProcType( wxCommandEvent& pEvent )
{
	m_nSelectId = m_pGraphTypeSelect->GetSelection();
	m_nColorId = m_pColorTypeSelect->GetSelection();

	///////////
	// SET THREAD ACTION VALUES
	DefCmdAction act;
	act.id = THREAD_ACTION_IMAGE_3D_COLORMAP;
	act.vectObj[0] = (void*) m_pImgView3D;
	act.vectObj[1] = (void*) m_pAstroImage;
	act.vectObj[2] = (void*) m_pImageView->m_pImage;
	act.vectInt[0] = m_nSelectId;
	act.vectInt[1] = m_nColorId;

	if( m_nSelectId == COLOR_3D_PROFILER_TYPE_3D_DISTRIBUTION )
	{
		GetToolBar()->EnableTool( wxID_BUTTON_3D_SURFACE_GRID_B, 0 );
		GetToolBar()->EnableTool( wxID_BUTTON_3D_SURFACE_IMAGE_B, 0 );
		//GetToolBar()->EnableTool( wxID_BUTTON_3D_GRAPH_GRID_B, 0 );
		m_pColorTypeSelect->Disable();

		/////////////////
		// show wait dialog
		//CWaitDialog waitDialog( this, wxT("Wait..."), wxT("Convert Image Data..."), 0, m_pUnimapWorker, &act );
		//waitDialog.ShowModal();

		m_pImgView3D->SetImageColorCube( m_pAstroImage, *(m_pImageView->m_pImage), m_nSelectId, m_nColorId );

	} else if( m_nSelectId == COLOR_3D_PROFILER_TYPE_NORMAL ||
				m_nSelectId == COLOR_3D_PROFILER_TYPE_VIGNETTING )
	{
		GetToolBar()->EnableTool( wxID_BUTTON_3D_SURFACE_GRID_B, 1 );
		GetToolBar()->EnableTool( wxID_BUTTON_3D_SURFACE_IMAGE_B, 1 );
		//GetToolBar()->EnableTool( wxID_BUTTON_3D_GRAPH_GRID_B, 1 );
		m_pColorTypeSelect->Enable();

		/////////////////
		// show wait dialog
		CWaitDialog waitDialog( this, wxT("Wait..."), wxT("Converting Image Data..."), 0, m_pUnimapWorker, &act );
		waitDialog.ShowModal();

		//m_pImgView3D->SetImageColorMap( m_pAstroImage, *(m_pImageView->m_pImage), m_nSelectId, m_nColorId );

		m_pImgView3D->SetImageView();
		// recreate call list
//		m_pImgView3D->DrawColorMapSurface( );

	} else if( m_nSelectId == COLOR_3D_PROFILER_TYPE_FFT )
	{
		GetToolBar()->EnableTool( wxID_BUTTON_3D_SURFACE_GRID_B, 1 );
		GetToolBar()->EnableTool( wxID_BUTTON_3D_SURFACE_IMAGE_B, 1 );
		//GetToolBar()->EnableTool( wxID_BUTTON_3D_GRAPH_GRID_B, 1 );
		m_pColorTypeSelect->Enable();

		/////////////////
		// show wait dialog
		CWaitDialog waitDialog( this, wxT("Wait..."), wxT("Converting Image Data..."), 0, m_pUnimapWorker, &act );
		waitDialog.ShowModal();

		m_pImgView3D->SetImageFFT( m_pAstroImage, *(m_pImageView->m_pImage), m_nSelectId, m_nColorId );

		m_pImgView3D->SetImageView();

		// recreate call list
//		m_pImgView3D->DrawColorMapSurface( );

	}

	m_pImgView3D->Refresh(false);

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnConfig
// Class:		CColorMapViewDialog
// Purpose:		dialog to configure 3d view
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CColorMapViewDialog::OnConfig( wxCommandEvent& pEvent )
{
	CConfig3DDialog* pDialog = new CConfig3DDialog( this, _("3D View Configuration") );
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
		m_pImgView3D->m_bShowProjections = pDialog->m_bShowProjections;

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
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_LIGHTING_AMBIENT, m_pImgView3D->m_nLightingAmbient );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_LIGHTING_DIFFUSE, m_pImgView3D->m_nLightingDiffuse );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_LIGHTING_SPECULAR, m_pImgView3D->m_nLightingSpecular );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_LIGHT_COORD_X, m_pImgView3D->m_nLightCoordX );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_LIGHT_COORD_Y, m_pImgView3D->m_nLightCoordY );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_LIGHT_COORD_Y, m_pImgView3D->m_nLightCoordZ );
		// :: materials
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_MATERIAL_AMBIENT, m_pImgView3D->m_nMaterialAmbient );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_MATERIAL_DIFFUSE, m_pImgView3D->m_nMaterialDiffuse );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_MATERIAL_SPECULAR, m_pImgView3D->m_nMaterialSpecular );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_MATERIAL_SHININESS, m_pImgView3D->m_nMaterialShininess );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_MATERIAL_EMISSION, m_pImgView3D->m_nMaterialEmission );
		// :: structures
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_SURFACE_WIDTH, m_pImgView3D->m_nStructureSurfaceWidth );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_SURFACE_HEIGHT, m_pImgView3D->m_nStructureSurfaceHeight );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_GRID_SPACER, m_pImgView3D->m_nStructureGridSpacer );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_SURFACE_SPACER, m_pImgView3D->m_nStructureSurfaceSpacer );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_Z_FACTOR, (float) m_pImgView3D->m_nStructureZFactor );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_GRID_LABELS, m_pImgView3D->m_bShowGraphGridLabels );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_SURFACE_LABELS, m_pImgView3D->m_bStructureSurfaceLabels );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_SHOW_AXIS, m_pImgView3D->m_bShowAxis );
		m_pMainConfig->SetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_SHOW_PROJECTIONS, m_pImgView3D->m_bShowProjections );

		// save
		m_pMainConfig->Save( );
	}

	// update & refresh
	pDialog->m_p3DView->m_bNewGridConfig = 1;
	pDialog->m_p3DView->Refresh( FALSE );

	GetToolBar()->ToggleTool( wxID_COLOR_3D_CONFIG, 0 );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SetConfig
// Class:		CColorMapViewDialog
// Purpose:		set main config from main config object
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CColorMapViewDialog::SetConfig( CConfigMain* pConfig )
{
	m_pMainConfig = pConfig;
	int bState = 1;

	/////////////////////////////////////////
	// load config
	// :: light
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_LIGHTING_AMBIENT, m_pImgView3D->m_nLightingAmbient, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_LIGHTING_DIFFUSE, m_pImgView3D->m_nLightingDiffuse, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_LIGHTING_SPECULAR, m_pImgView3D->m_nLightingSpecular, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_LIGHT_COORD_X, m_pImgView3D->m_nLightCoordX, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_LIGHT_COORD_Y, m_pImgView3D->m_nLightCoordY, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_LIGHT_COORD_Y, m_pImgView3D->m_nLightCoordZ, 1 );
	// :: materials
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_MATERIAL_AMBIENT, m_pImgView3D->m_nMaterialAmbient, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_MATERIAL_DIFFUSE, m_pImgView3D->m_nMaterialDiffuse, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_MATERIAL_SPECULAR, m_pImgView3D->m_nMaterialSpecular, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_MATERIAL_SHININESS, m_pImgView3D->m_nMaterialShininess, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_MATERIAL_EMISSION, m_pImgView3D->m_nMaterialEmission, 1 );
	// :: structure
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_SURFACE_WIDTH, m_pImgView3D->m_nStructureSurfaceWidth, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_SURFACE_HEIGHT, m_pImgView3D->m_nStructureSurfaceHeight, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_GRID_SPACER, m_pImgView3D->m_nStructureGridSpacer, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_SURFACE_SPACER, m_pImgView3D->m_nStructureSurfaceSpacer, 1 );
	bState = m_pMainConfig->GetFloatVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_Z_FACTOR, m_pImgView3D->m_nStructureZFactor, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_GRID_LABELS, m_pImgView3D->m_bShowGraphGridLabels, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_SURFACE_LABELS, m_pImgView3D->m_bStructureSurfaceLabels, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_SHOW_AXIS, m_pImgView3D->m_bShowAxis, 1 );
	bState = m_pMainConfig->GetIntVar( SECTION_ID_GUI3D, CONF_GUI_3D_COLMAP_STRUCT_SHOW_PROJECTIONS, m_pImgView3D->m_bShowProjections, 1 );

	if( !bState ) m_pMainConfig->Save( );

	return;
}