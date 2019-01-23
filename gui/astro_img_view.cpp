
// wx
#include "wx/wxprec.h"
#include <wx/regex.h>
#include <wx/tooltip.h>
#include <wx/utils.h>
#include <wx/graphics.h>
#include <wx/dcbuffer.h>

// local headers
#include "../util/folders.h"
#include "../sky/star.h"
#include "../image/astroimage.h"
#include "../image/imagegroup.h"
#include "../image/image_shift.h"
#include "../sky/sky.h"
#include "../sky/sky_finder.h"
#include "../sky/catalog_stars_names.h"
#include "../sky/catalog_dso_names.h"
#include "../match/starmatch.h"
#include "starviewdlg.h"
#include "../unimap.h"
#include "../unimap_worker.h"
#include "../util/func.h"
#include "../util/geometry.h"
//#include "../util/geometry.h"
#include "../util/apps_manager.h"
#include "frame.h"
#include "dist_tool_view.h"
#include "RadialProfileView.h"
// catalogs
#include "../sky/catalog_radio.h"
#include "../sky/catalog_xgamma.h"
#include "../sky/catalog_supernovas.h"
#include "../sky/catalog_blackholes.h"
#include "../sky/catalog_mstars.h"
#include "../sky/catalog_exoplanets.h"
#include "../sky/catalog_asteroids.h"
#include "../sky/catalog_comets.h"
#include "../sky/catalog_aes.h"

// main header
#include "astro_img_view.h"

// get external unimap
DECLARE_APP(CUnimap)

// class :: CAstroImgView
///////////////////////////////////////////////
// implement message map
BEGIN_EVENT_TABLE( CAstroImgView, CDynamicImgView )
	// mouse handlers
//	EVT_LEFT_DCLICK( CAstroImgView::OnMouseLDClick )
//	EVT_RIGHT_UP( CAstroImgView::OnRightClick )
//	EVT_LEFT_DOWN( CAstroImgView::OnLeftClickDown )
//	EVT_LEFT_UP( CAstroImgView::OnLeftClickUp )
//	EVT_MOTION( CAstroImgView::OnMouseMove ) 	
	EVT_LEAVE_WINDOW( CAstroImgView::OnMouseLeave )
	// gen menu calls
	EVT_MENU( wxID_VIEW_HIDE_SHOW_OBJ, CAstroImgView::OnShowHideObject )
	EVT_MENU( wxID_VIEWWITH_GOOGLEONLINE, CAstroImgView::OnViewWithGoogleOnline )
	EVT_MENU( wxID_VIEWWITH_SKYMAPORG, CAstroImgView::OnViewWithSkyMapOrg )
	EVT_MENU( wxID_VIEWWITH_GOOGLEEARTH, CAstroImgView::OnViewWithApp )
	EVT_MENU( wxID_VIEWWITH_WORLDWIND, CAstroImgView::OnViewWithApp )
	EVT_MENU( wxID_IMGVIEWSLEWTO, CAstroImgView::OnSlewToObject )
	// area menu 
	EVT_MENU( wxID_IMGVIEWEDITAREA, CAstroImgView::OnEditArea )
	EVT_MENU( wxID_IMGVIEWDELAREA, CAstroImgView::OnDeleteArea )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////
CAstroImgView::CAstroImgView( wxWindow *frame, const wxPoint& pos, const wxSize& size ):
								CDynamicImgView(frame, pos, size )
{
	// get references to worker and online objects
	m_pUnimap = &(wxGetApp());

	m_pDistTool =  NULL;
	m_nLastSelectedA = -1;
	m_nLastSelectedB = -1;
	m_nLastSelectedTypeA = 0;
	m_nLastSelectedTypeB = 0;

	// color/pen/brush set
	wxDefaultAreaCol.Set( 255, 0, 0 );
	wxSelectedAreaCol.Set( 0, 255, 0 );
	wxResizeAreaCol.Set( 0, 0, 255 );
	wxSetAreaCol.Set( 0, 140, 176 );
	// pen
	wxDefaultAreaPen.SetColour( wxDefaultAreaCol );
	wxSelectedAreaPen.SetColour( wxSelectedAreaCol );
	wxResizeAreaPen.SetColour( wxResizeAreaCol );
	wxSetAreaPen.SetColour( wxSetAreaCol );
	// brush
	wxDefaultAreaBrush.SetColour( wxDefaultAreaCol );
	wxDefaultAreaBrush.SetStyle( wxTRANSPARENT );

	/////////////
	// set various flags
	// set all my dynamic vectors to null
	// :: constellations
	m_vectConstellations = NULL;
	m_nConstellations = 0;
	// :: stars
	m_vectStarState = NULL;
	m_vectStarName = NULL;
	m_nStar = 0;
	// :: cat star
	m_vectCatStarName = NULL;
	m_nCatStar = 0;
	// :: dso
	m_vectDsoName = NULL;
	m_nDso = 0;
	// :: radio
	m_vectRadioName = NULL;
	m_nRadio = 0;
	// :: x-ray/gamma
	m_vectXGammaName = NULL;
	m_nXGamma = 0;
	// :: supernovas
	m_vectSupernovasName = NULL;
	m_nSupernovas = 0;
	// :: blackholes
	m_vectBlackholesName = NULL;
	m_nBlackholes = 0;
	// :: multiple/double stars
	m_vectMStarsName = NULL;
	m_nMStars = 0;
	// :: extra-solar planets
	m_vectExoplanetsName = NULL;
	m_nExoplanets = 0;
	// :: asteroids
	m_vectAsteroidsName = NULL;
	m_nAsteroids = 0;
	m_vectAsteroidsOrbit = NULL;
	// :: comets
	m_vectCometsName = NULL;
	m_nComets = 0;
	m_vectCometsOrbit = NULL;
	// :: aes
	m_vectAesName = NULL;
	m_nAes = 0;
	m_vectAesOrbit = NULL;
	// :: solar
	m_vectSolarName = NULL;
	m_nSolar = 0;
	m_vectSolarOrbit = NULL;

	////////////
	// set labels
	m_vectStarLabels = NULL;
	m_vectCatStarLabels = NULL;
	m_vectDsoLabels = NULL;
	m_nStarLabels = 0;
	m_nCatStarLabels = 0;
	m_nDsoLabels = 0;

	// reset pointer to astro image
	m_pAstroImage = NULL;

	// display limits
	m_nStarDisplayLimit = 0;
	m_nCatStarDisplayLimit = 0;
	m_nCatDsoDisplayLimit = 0;
	m_nCatRadioDisplayLimit = 0;
	m_nCatXGammaDisplayLimit = 0;
	m_nCatSupernovasDisplayLimit = 0;
	m_nCatBlackholesDisplayLimit = 0;
	m_nCatMStarsDisplayLimit = 0;
	m_nCatExoplanetsDisplayLimit = 0;
	m_nCatAsteroidsDisplayLimit = 0;
	m_nCatCometsDisplayLimit = 0;
	m_nCatAesDisplayLimit = 0;
	m_nSolarDisplayLimit = 0;

	// flags
	m_bImageAltered = 0;
	m_bShowSkyGrid = 0;
	m_bIsDistGrid = 0;
	m_bDrawObjects = 1;
//	m_bDrawCatStar = 1;
//	m_bDrawAreas = 1;

	// set targets
	m_nTarget = 0;
	m_nTargetAllocated = 0;
	m_vectTarget = NULL;

	// init distance tool
	m_bIsDistanceTool = 0;
	m_bIsDistanceToolDragA = 0;
	m_bIsDistanceToolDragB = 0;
	// other flags
//	m_pMainConfig->m_bShowConstellations = 1;
	m_bShowDynamicObjPaths = 1;

	InitVars();

	m_pMainConfig = NULL;

	/////////
	// set camera view vars
	m_bMainTarget = 0;
	bIsLocSet = 0;
	m_bIsCamera = 0;
	m_bIsCameraVideo = 0;

	// main window sizer - to use for overlays
	wxBoxSizer *pSizerMain = new wxBoxSizer( wxVERTICAL ); 
	pSizerMain->SetDimension( 0, 0, m_nWidth-2, m_nHeight-1 );
	this->SetSizer( pSizerMain );
}

////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////
CAstroImgView::~CAstroImgView( )
{
	FreeAll( );	
}

// init all the vars
////////////////////////////////////////////////////////////////////
void CAstroImgView::InitVars()
{
	// reset other flags - todo: maybe I should have the others here
	m_nLastOverObject = -1;
	m_nLastOverObjectType = -1;
}

///////////////////////////////////////////////////////////////////
// Purose:	free previos allocated data for stars etc
////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeData( )
{
	// detected stars
	FreeDetectedStars( );
	// catalog stars
	FreeCatStars( );
	// dso
	FreeDsoObjects( );
	// radio
	FreeRadioSources( );
	// x-ray/gamma
	FreeXGammaSources( );
	// supernovas
	FreeSupernovas( );
	// blackholes
	FreeBlackholes( );
	// multiple/double stars
	FreeMStars( );
	// extra-solar planets
	FreeExoplanets( );
	// asteroids
	FreeAsteroids( );
	// comets
	FreeComets( );
	// aes
	FreeAes( );
	// solar
	FreeSolar( );
	// free constelations
	FreeCostellations( );

	//////////////////////////
	// also release sky grid point if any
	if( m_bShowSkyGrid ) DeleteSkyGrid( );

	// clear targets
	FreeTargets();
}

////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeAll( )
{
	FreeData();

	// free image stuff
	m_pAstroImage = NULL;
	m_bHasImage = 0;
	FreeImage( );

	InitVars();

	return;
}

////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeCostellations( )
{
	// free artwork
	int i;
	for( i=0; i<m_nConstellations; i++ )
	{
		if( m_vectArtworkFile[i].image != NULL )
		{
			if( m_vectArtworkFile[i].image->IsOk() ) m_vectArtworkFile[i].image->Destroy( );
			if( m_vectArtworkFile[i].image != NULL ) delete( m_vectArtworkFile[i].image );
			m_vectArtworkFile[i].image = NULL;
		}
	}
	// free if already allocated
	if( m_vectConstellations )
	{
		for( i=0; i<m_nConstellations; i++ )
		{
			if( m_vectConstellations[i].name ) free( m_vectConstellations[i].name );
//			free( m_vectConstellations[i].name );
			if( m_vectConstellations[i].vectLine ) free( m_vectConstellations[i].vectLine );
			m_vectConstellations[i].name = NULL;
			m_vectConstellations[i].vectLine = NULL;
		}
		free( m_vectConstellations );
		m_vectConstellations = NULL;
	}
}

///////////////////////////////////////////////////////////////////
// Method:	FreeTargets
// Class:	CAstroImgView
// Purose:	free allocated targets
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeTargets( )
{
	int i = 0;

	if( m_vectTarget )
	{
		free( m_vectTarget );
		m_vectTarget = NULL;
	}

	m_nTarget = 0;
	m_nTargetAllocated = 0;

	return;
}

///////////////////////////////////////////////////////////////////
// Method:	FreeDetectedStars
// Class:	CAstroImgView
// Purose:	free previos allocated detected stars 
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeDetectedStars( )
{
	int i = 0;

/*	if( m_vectStarName )
	{
		// free star names vector
		for( i=0; i<m_nStar; i++ )
		{
			if( m_vectStarName[i] )
			{
				free( m_vectStarName[i] );
				m_vectStarName[i] = NULL;
			}
		}
		free( m_vectStarName );
		m_vectStarName = NULL;
	}

*/
	m_vectStarName.Clear();
	if( m_vectStarState ) free( m_vectStarState );
	if( m_vectStarLabels ) free( m_vectStarLabels );

	m_nStar = 0;
	m_nStarLabels = 0;
	m_vectStarLabels = NULL;
	m_nStarDisplayLimit = 0;

	return;
}

///////////////////////////////////////////////////////////////////
// Method:	FreeCatStars
// Class:	CAstroImgView
// Purose:	free previos allocated data for catalog stars 
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeCatStars( )
{
	int i = 0;

	// check if star cat vector allocated
/*	if( m_vectCatStarName )
	{
		// free star names vector
		for( i=0; i<m_nCatStar; i++ )
		{
			if( m_vectCatStarName[i] )
			{
				free( m_vectCatStarName[i] );
				m_vectCatStarName[i] = NULL;
			}
		}
		free( m_vectCatStarName );
		m_vectCatStarName = NULL;
	}
*/
	m_vectCatStarName.Clear();
	if( m_vectCatStarLabels ) free( m_vectCatStarLabels );

	m_nCatStar = 0;
	m_nCatStarLabels = 0;
	m_vectCatStarLabels = NULL;
	m_nCatStarDisplayLimit = 0;

	return;
}

///////////////////////////////////////////////////////////////////
// Method:	FreeDsoObjects
// Class:	CAstroImgView
// Purose:	free previos allocated data dso objects
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeDsoObjects( )
{
	int i = 0;

	// check if star cat vector allocated
	if( m_vectDsoName )
	{
		////////////////////////////////
		// free DSO name vector
		for( i=0; i<m_nDso; i++ )
		{
			if( m_vectDsoName[i] )
			{
				free( m_vectDsoName[i] );
				m_vectDsoName[i] = NULL;
			}
		}
		free( m_vectDsoName );
		m_vectDsoName = NULL;
	}

	if( m_vectDsoLabels ) free( m_vectDsoLabels );

	// reset counters to zero
	m_nDso = 0;
	m_nDsoLabels = 0;
	m_vectDsoLabels = NULL;
	m_nCatDsoDisplayLimit = 0;

	return;
}

///////////////////////////////////////////////////////////////////
// Method:	FreeRadioSources
// Class:	CAstroImgView
// Purose:	free previos allocated data radio sources
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeRadioSources( )
{
	int i = 0;

	// check if star cat vector allocated
	if( m_vectRadioName )
	{
		////////////////////////////////
		// free RADIO name vector
		for( i=0; i<m_nRadio; i++ )
		{
			if( m_vectRadioName[i] )
			{
				free( m_vectRadioName[i] );
				m_vectRadioName[i] = NULL;
			}
		}
		free( m_vectRadioName );
		m_vectRadioName = NULL;
	}
	// reset counters to zero
	m_nRadio = 0;
	m_nCatRadioDisplayLimit = 0;

	return;
}

///////////////////////////////////////////////////////////////////
// Method:	FreeXGammaSources
// Class:	CAstroImgView
// Purose:	free previos allocated data x-ray/gamma sources
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeXGammaSources( )
{
	int i = 0;

	// check if cat vector allocated
	if( m_vectXGammaName )
	{
		////////////////////////////////
		// free RADIO name vector
		for( i=0; i<m_nXGamma; i++ )
		{
			if( m_vectXGammaName[i] )
			{
				free( m_vectXGammaName[i] );
				m_vectXGammaName[i] = NULL;
			}
		}
		free( m_vectXGammaName );
		m_vectXGammaName = NULL;
	}
	// reset counters to zero
	m_nXGamma = 0;
	m_nCatXGammaDisplayLimit = 0;

	return;
}

///////////////////////////////////////////////////////////////////
// Method:	FreeSupernovas
// Class:	CAstroImgView
// Purose:	free previos allocated supernovas names 
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeSupernovas( )
{
	int i = 0;

	// check if cat vector allocated
	if( m_vectSupernovasName )
	{
		////////////////////////////////
		// free SUPERNOVAS name vector
		for( i=0; i<m_nSupernovas; i++ )
		{
			if( m_vectSupernovasName[i] )
			{
				free( m_vectSupernovasName[i] );
				m_vectSupernovasName[i] = NULL;
			}
		}
		free( m_vectSupernovasName );
		m_vectSupernovasName = NULL;
	}
	// reset counters to zero
	m_nSupernovas = 0;
	m_nCatSupernovasDisplayLimit = 0;

	return;
}

///////////////////////////////////////////////////////////////////
// Method:	FreeBlackholes
// Class:	CAstroImgView
// Purose:	free previos allocated blackholes names 
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeBlackholes( )
{
	int i = 0;

	// check if cat vector allocated
	if( m_vectBlackholesName )
	{
		////////////////////////////////
		// free BLACK HOLES name vector
		for( i=0; i<m_nBlackholes; i++ )
		{
			if( m_vectBlackholesName[i] )
			{
				free( m_vectBlackholesName[i] );
				m_vectBlackholesName[i] = NULL;
			}
		}
		free( m_vectBlackholesName );
		m_vectBlackholesName = NULL;
	}
	// reset counters to zero
	m_nBlackholes = 0;
	m_nCatBlackholesDisplayLimit = 0;

	return;
}

///////////////////////////////////////////////////////////////////
// Method:	FreeMStars
// Class:	CAstroImgView
// Purose:	free previos allocated multiple/double stars names 
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeMStars( )
{
	int i = 0;

	// check if cat vector allocated
	if( m_vectMStarsName )
	{
		////////////////////////////////
		// free MStars name vector
		for( i=0; i<m_nMStars; i++ )
		{
			if( m_vectMStarsName[i] )
			{
				free( m_vectMStarsName[i] );
				m_vectMStarsName[i] = NULL;
			}
		}
		free( m_vectMStarsName );
		m_vectMStarsName = NULL;
	}
	// reset counters to zero
	m_nMStars = 0;
	m_nCatMStarsDisplayLimit = 0;

	return;
}

///////////////////////////////////////////////////////////////////
// Method:	FreeExoplanets
// Class:	CAstroImgView
// Purose:	free previos allocated extra-solar planets names 
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeExoplanets( )
{
	int i = 0;

	// check if cat vector allocated
	if( m_vectExoplanetsName )
	{
		////////////////////////////////
		// free Exoplanets name vector
		for( i=0; i<m_nExoplanets; i++ )
		{
			if( m_vectExoplanetsName[i] )
			{
				free( m_vectExoplanetsName[i] );
				m_vectExoplanetsName[i] = NULL;
			}
		}
		free( m_vectExoplanetsName );
		m_vectExoplanetsName = NULL;
	}
	// reset counters to zero
	m_nExoplanets = 0;
	m_nCatExoplanetsDisplayLimit = 0;

	return;
}

///////////////////////////////////////////////////////////////////
// Method:	FreeAsteroids
// Class:	CAstroImgView
// Purose:	free previos allocated asteroids names 
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeAsteroids( )
{
	int i = 0;

	// check if cat vector allocated
	if( m_vectAsteroidsName )
	{
		////////////////////////////////
		// free Asteroids name vector
		for( i=0; i<m_nAsteroids; i++ )
		{
			if( m_vectAsteroidsName[i] )
			{
				free( m_vectAsteroidsName[i] );
				m_vectAsteroidsName[i] = NULL;
			}
		}
		free( m_vectAsteroidsName );
		m_vectAsteroidsName = NULL;
	}

	// free orbits
	if( m_vectAsteroidsOrbit )
	{
		for( i=0; i<m_nAsteroids; i++ )
			m_vectAsteroidsOrbit[i].clear();
		
		delete [] m_vectAsteroidsOrbit;
		m_vectAsteroidsOrbit = NULL;
	}	

	// reset counters to zero
	m_nAsteroids = 0;
	m_nCatAsteroidsDisplayLimit = 0;

	return;
}

///////////////////////////////////////////////////////////////////
// Method:	FreeComets
// Class:	CAstroImgView
// Purose:	free previos allocated comets names 
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeComets( )
{
	int i = 0;

	// check if cat vector allocated
	if( m_vectCometsName )
	{
		////////////////////////////////
		// free Comets name vector
		for( i=0; i<m_nComets; i++ )
		{
			if( m_vectCometsName[i] )
			{
				free( m_vectCometsName[i] );
				m_vectCometsName[i] = NULL;
			}
		}
		free( m_vectCometsName );
		m_vectCometsName = NULL;
	}

	// free orbits
	if( m_vectCometsOrbit )
	{
		for( i=0; i<m_nComets; i++ )
			m_vectCometsOrbit[i].clear();
		
		delete [] m_vectCometsOrbit;
		m_vectCometsOrbit = NULL;
	}	

	// reset counters to zero
	m_nComets = 0;
	m_nCatCometsDisplayLimit = 0;

	return;
}

///////////////////////////////////////////////////////////////////
// Method:	FreeAes
// Class:	CAstroImgView
// Purose:	free previos allocated earth satellites names 
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeAes( )
{
	int i = 0;

	// check if cat vector allocated
	if( m_vectAesName )
	{
		////////////////////////////////
		// free AES name vector
		for( i=0; i<m_nAes; i++ )
		{
			if( m_vectAesName[i] )
			{
				free( m_vectAesName[i] );
				m_vectAesName[i] = NULL;
			}
		}
		free( m_vectAesName );
		m_vectAesName = NULL;
	}

	// free orbits
	if( m_vectAesOrbit )
	{
		for( i=0; i<m_nAes; i++ )
			m_vectAesOrbit[i].clear();
		
		delete [] m_vectAesOrbit;
		m_vectAesOrbit = NULL;
	}	

	// reset counters to zero
	m_nAes = 0;
	m_nCatAesDisplayLimit = 0;

	return;
}

///////////////////////////////////////////////////////////////////
// Method:	FreeSolar
// Class:	CAstroImgView
// Purose:	free previos allocated solar/sun/moon names 
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::FreeSolar( )
{
	int i = 0;

	// check if cat vector allocated
	if( m_vectSolarName )
	{
		////////////////////////////////
		// free Solar name vector
		for( i=0; i<m_nSolar; i++ )
		{
			if( m_vectSolarName[i] )
			{
				free( m_vectSolarName[i] );
				m_vectSolarName[i] = NULL;
			}
		}
		free( m_vectSolarName );
		m_vectSolarName = NULL;
	}

	// free orbits
	if( m_vectSolarOrbit )
	{
		for( i=0; i<m_nSolar; i++ )
		{
			//vector<SOLARORBIT*>::iterator itPos = m_vectSolarOrbit[i].begin();
			// clear all elements from the array
			//for( int j=0; j < m_vectSolarOrbit[i].size(); j++ ) delete( m_vectSolarOrbit[i].at[j] );   

			// finally, clear all elements from the array
			m_vectSolarOrbit[i].clear();

			SOLARORBIT temp;
			m_vectSolarOrbit[i].swap( temp );

			
		}
		
		delete [] m_vectSolarOrbit;
		m_vectSolarOrbit = NULL;
	}	

	// reset counters to zero
	m_nSolar = 0;
	m_nSolarDisplayLimit = 0;

	return;
}

////////////////////////////////////////////////////////////////////
void CAstroImgView::WriteImage( wxString& strFile, int nWidth, int nHeight, int nType )
{
	if( nType == 1 )
	{
		wxMemoryDC memDC;
//		memDC.SetFont( smallFont );

		// set the dc size - either use the original or new
		int width=0, height=0;
		if( nWidth <= 0 || nHeight <= 0 )
		{
			width = m_nOrigPicWidth;
			height = m_nOrigPicHeight;

		} else
		{
			width = nWidth;
			height = nHeight;
		}

		wxBitmap bitmap( width, height );
		memDC.SelectObject( bitmap );

		///////////////////////////
		// write to bitmap dc
		WriteImageToDc( memDC, nWidth, nHeight );

		// call to save
		wxImage img = bitmap.ConvertToImage();
		// get the original image quality - todo: not working at the moment
		int nQ = wxAtoi( m_pImage->GetOption( wxT("quality") ) );
		// set quality - default 100 % ...
		img.SetOption( wxT("quality"), 100 );
		//bitmap.SaveFile( strFile, wxBITMAP_TYPE_JPEG );
		m_pAstroImage->SaveImage( img, strFile );

	} else if( nType == 0 )
	{
		m_pAstroImage->SaveImage( *m_pImage, strFile );
	}
}

////////////////////////////////////////////////////////////////////
void CAstroImgView::WriteImageToDc( wxDC& dc, int nWidth, int nHeight )
{
		///////////////////////////
		// lock view
		this->Freeze();
		// backup values
		int _nWidth = m_nWidth;
		int _nHeight = m_nHeight;
		// set new values
		if( nWidth <= 0 || nHeight <= 0 )
		{
			m_nWidth = m_nOrigPicWidth;
			m_nHeight = m_nOrigPicHeight;

		} else
		{
			m_nWidth = nWidth;
			m_nHeight = nHeight;
		}

		// reprocess input image with new size
		ProcessInputImage( );
		// draw all my tags
		Draw( dc );

		// restore values
		m_nWidth = _nWidth;
		m_nHeight = _nHeight;
		// reprocess input image with new size
		ProcessInputImage( );		

		this->Thaw();
		Refresh(false);
}

////////////////////////////////////////////////////////////////////
// Method:	Draw
// Class:	CAstroImgView
// Purose:	image canvas drawing
// Input:	reference to dc
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::Draw( wxDC& dc, int bSize, int bUpdate )
{
	int i=0, nObjState=0;
	long c=0;

	// check if dc available
	if( !dc.Ok( ) || m_bDrawing == false || (!m_pAstroImage && !m_bIsCameraVideo) ){ return; }

	// check first for svg drawing on bitmap
//	DrawSvg( dc );
	// call to main image window draw method
	CDynamicImgView::Draw( dc, bSize, 0 );

	///////////////////////////////////////
	// check see if to draw stars
	if( m_pAstroImage && m_bDrawObjects == 1 )
	{
		dc.SetPen( wxDefaultAreaPen );
		dc.SetBrush( wxDefaultAreaBrush );

		//////////////////////////////
		// draw constelations
		if( m_pMainConfig->m_bShowConstellations ) DrawConstellations( dc );

		///////////////////////
		// draw stars
		c = 0;
		for( i=0; i<m_pAstroImage->m_nStar; i++ )
		{
			nObjState = m_pAstroImage->m_vectStar[i].state;
			// selected items will be always shown
			if( ( nObjState & OBJECT_STATE_MOUSE_OVER) ||
				( nObjState & OBJECT_STATE_CUSTOM_SHOW) ||
				// selected is used for distance tool
				( (nObjState & OBJECT_STATE_SELECTED) || c < m_nStarDisplayLimit || 
					 //c < m_nStarDisplayLimit ) && 
					 (nObjState & OBJECT_STATE_CUSTOM_SHOW) ) &&
					( m_pMainConfig->m_bViewDetectedObjectsNoMatch || m_pAstroImage->m_vectStar[i].match ) )
			{
				if( DrawStar( dc, i ) ) c++;
			}
		}

		///////////////////////////
		// draw catalog stars
		c = 0;
		for( i=0; i<m_pAstroImage->m_nCatStar; i++ )
		{
			nObjState = m_pAstroImage->m_vectCatStar[i].state;

			if( (nObjState & OBJECT_STATE_MOUSE_OVER) ||
				(nObjState & OBJECT_STATE_CUSTOM_SHOW) ||
				( (nObjState & OBJECT_STATE_SELECTED) || 
				c < m_nCatStarDisplayLimit ) )
			{
				if( DrawCatStar( dc, i ) ) c++;
			}
		}

		//////////////////////////
		// draw objects from cats dso
		c = 0;
		for( i=0; i<m_pAstroImage->m_nCatDso; i++ )
		{
			nObjState = m_pAstroImage->m_vectCatDso[i].state;

			if( (nObjState & OBJECT_STATE_MOUSE_OVER) ||
				(nObjState & OBJECT_STATE_CUSTOM_SHOW) ||
				( ( nObjState & OBJECT_STATE_SELECTED ) || 
				c < m_nCatDsoDisplayLimit ) )
			{
				if( DrawDso( dc, i ) ) c++;
			}
		}

		//////////////////////////
		// draw :: RADIO SOURCES
		for( i=0; i<m_pAstroImage->m_nCatRadio; i++ )
		{
			if( i < m_nCatRadioDisplayLimit )
			{
				DrawRadioSource( dc, i );
			}
		}

		//////////////////////////
		// draw :: X-RAY/GAMMA SOURCES
		for( i=0; i<m_pAstroImage->m_nCatXGamma; i++ )
		{
			if( i < m_nCatXGammaDisplayLimit )
			{
				DrawXGammaSource( dc, i );
			}
		}

		//////////////////////////
		// draw :: SUPERNOVAS
		for( i=0; i<m_pAstroImage->m_nCatSupernova; i++ )
		{
			if( i < m_nCatSupernovasDisplayLimit )
			{
				DrawSupernovas( dc, i );
			}
		}

		//////////////////////////
		// draw :: BLACKHOLES
		for( i=0; i<m_pAstroImage->m_nCatBlackhole; i++ )
		{
			if( i < m_nCatBlackholesDisplayLimit )
			{
				DrawBlackhole( dc, i );
			}
		}

		//////////////////////////
		// draw :: MULTIPLE/DOUBLE STARS
		for( i=0; i<m_pAstroImage->m_nCatMStars; i++ )
		{
			if( i < m_nCatMStarsDisplayLimit )
			{
				DrawMStars( dc, i );
			}
		}

		//////////////////////////
		// draw :: EXTRA-SOLAR PLANETS
		for( i=0; i<m_pAstroImage->m_nCatExoplanet; i++ )
		{
			if( i < m_nCatExoplanetsDisplayLimit )
			{
				DrawExoplanet( dc, i );
			}
		}

		//////////////////////////
		// draw :: ASTEROIDS
		for( i=0; i<m_pAstroImage->m_nCatAsteroid; i++ )
		{
			if( i < m_nCatAsteroidsDisplayLimit )
			{
				DrawAsteroid( dc, i );
				DrawSolarObjectOrbit( dc, SOLAR_OBJECT_TYPE_ASTEROID, i );
			}
		}

		//////////////////////////
		// draw :: COMETS
		for( i=0; i<m_pAstroImage->m_nCatComet; i++ )
		{
			if( i < m_nCatCometsDisplayLimit )
			{
				DrawComet( dc, i );
				DrawSolarObjectOrbit( dc, SOLAR_OBJECT_TYPE_COMET, i );
			}
		}

		//////////////////////////
		// draw :: ARTIFICIAL EARTH SATELLITES
		for( i=0; i<m_pAstroImage->m_nCatAes; i++ )
		{
			if( i < m_nCatAesDisplayLimit )
			{
				DrawAes( dc, i );
				DrawSolarObjectOrbit( dc, SOLAR_OBJECT_TYPE_AES, i );
			}
		}

		//////////////////////////
		// draw :: SOLAR
		for( i=0; i<m_pAstroImage->m_nSolarPlanets; i++ )
		{
			if( i < m_nSolarDisplayLimit )
			{
				DrawSolar( dc, i );
				if( m_pAstroImage->m_vectSolarPlanets[i].obj_type == 0 )
					DrawSolarObjectOrbit( dc, SOLAR_OBJECT_TYPE_PLANETS, i );
			}
		}

		///////////////////////////////
		// draw object paths
		if( m_bShowDynamicObjPaths ) DrawDynamicObjectPaths( dc );

		////////////////////////////
		// draw constellations labels
		if( m_pMainConfig->m_bConstellationLabel ) DrawConstellationsLabel( dc );

		/////////////////////////////////
		// draw sky grid
		if( m_bShowSkyGrid ) DrawSkyGrid( dc );

		/////////////////////////////////
		// draw distortion grid
		if( m_bIsDistGrid ) DrawDistGrid( dc );

	}


	//////////////////////////
	// draw :: AREAS
	if( !m_bIsCameraVideo && m_pMainConfig->m_bViewImageAreas )
	{
		for( i=0; i<m_pAstroImage->m_vectArea.size(); i++ )
		{
			if( m_nAreaEdit == i )
				DrawArea( dc, m_pAstroImage->m_vectArea[i], 2 );
			else if( m_nAreaSelected == i )
				DrawArea( dc, m_pAstroImage->m_vectArea[i], 1 );
			else 
				DrawArea( dc, m_pAstroImage->m_vectArea[i], 0 );
		}
	}

	//////////////////////////
	// draw targets
	if( m_nTarget > 0 )
	{
		// draw targets
		for( i=0; i<m_nTarget; i++ )
		{
			DrawTarget( dc, i );
		}

	}

	///////////////////////
	// Distance tool
	if( m_bIsDistanceTool ) DrawDistanceTool( dc );

	////////////////////////
	// draw camera stuff
	if( m_bIsCamera || m_bIsCameraVideo ) DrawCam( dc, bSize, bUpdate );

	// set default brush and pen  back on 
	dc.SetPen( wxDefaultPen );
	dc.SetBrush( wxDefaultBrush );

//	dc.EndDrawing();
//	m_bDrawing = false;

	//	Refresh( FALSE );
	Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	DrawDistGrid
// Class:	CAstroImgView
// Purpose:	draw sky grid
// Input:	units ??
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::DrawDistGrid( wxDC& dc )
{
	if( !m_pAstroImage->m_bIsDistGrid ) return;

	int x=0, y=0;
	double nPointAX, nPointAY;
	double nPointBX, nPointBY;

	///////////////////////////////////////////////
	//  save current pen
	wxPen currentPen = dc.GetPen();
	// set grid pen
	wxColour colGrid = m_pMainConfig->colGraphicsDistortionGrid;
	wxPen penDrawGrid;
	// get line style
	int nLineStyle = m_pMainConfig->m_nGraphicsDistortionGridLine;
	// set color
	penDrawGrid.SetColour( colGrid );
	// set line stype
	if( nLineStyle == GRAPHICS_LINE_STYLE_NORMAL )
		penDrawGrid.SetStyle( wxSOLID );
	else if( nLineStyle == GRAPHICS_LINE_STYLE_DOTTED )
		penDrawGrid.SetStyle( wxDOT );
	else if( nLineStyle == GRAPHICS_LINE_STYLE_LDOTTED )
		penDrawGrid.SetStyle( wxSHORT_DASH );
	// set dc pen
	dc.SetPen( penDrawGrid );

	/////////////////////////////////////
	// DRAWING PART
	if( m_pMainConfig->m_bDistGridDrawSplines == 0 )
	{
		for( y=0; y<m_pAstroImage->m_nDistGridYUnits; y++ )
		{
			for( x=0; x<m_pAstroImage->m_nDistGridXUnits; x++ )
			{
				nPointBX = m_pAstroImage->m_vectDistGridX[y][x] * m_nScaleFactor;
				nPointBY = m_pAstroImage->m_vectDistGridY[y][x] * m_nScaleFactor;

				if( m_nZoomCount > 0.5 )
				{
					nPointBX = (int) round( (nPointBX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
					nPointBY = (int) round( (nPointBY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
				}

				// check if to draw horizontal line
				if( x > 0  )
				{
					// also calculate star ax
					nPointAX = m_pAstroImage->m_vectDistGridX[y][x-1] * m_nScaleFactor;
					nPointAY = m_pAstroImage->m_vectDistGridY[y][x-1] * m_nScaleFactor;

					if( m_nZoomCount > 0.5 )
					{
						nPointAX = (int) round( (nPointAX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
						nPointAY = (int) round( (nPointAY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
					}

					// draw line
					dc.DrawLine( m_nPicOrigX+nPointAX, m_nPicOrigY+nPointAY, 
									m_nPicOrigX+nPointBX, m_nPicOrigY+nPointBY );
				}
				// draw vertical
				if( y > 0 )
				{
					// also calculate star ax
					nPointAX = m_pAstroImage->m_vectDistGridX[y-1][x] * m_nScaleFactor;
					nPointAY = m_pAstroImage->m_vectDistGridY[y-1][x] * m_nScaleFactor;

					if( m_nZoomCount > 0.5 )
					{
						nPointAX = (int) round( (nPointAX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
						nPointAY = (int) round( (nPointAY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
					}

					// draw line
					dc.DrawLine( m_nPicOrigX+nPointAX, m_nPicOrigY+nPointAY, 
									m_nPicOrigX+nPointBX, m_nPicOrigY+nPointBY );
				}

			}
		}

	} else if( m_pMainConfig->m_bDistGridDrawSplines == 1 )
	{
		///////////////
		// option 2 - draw with splines
		int max_size = m_pAstroImage->m_nDistGridYUnits;
		if( max_size < m_pAstroImage->m_nDistGridXUnits ) max_size = m_pAstroImage->m_nDistGridXUnits;
		wxPoint* vectPoints = (wxPoint*) malloc( (max_size+1)*sizeof(wxPoint) );
		// :: horizontal lines
		for( y=0; y<m_pAstroImage->m_nDistGridYUnits; y++ )
		{
			for( x=0; x<m_pAstroImage->m_nDistGridXUnits; x++ )
			{
				nPointBX = m_pAstroImage->m_vectDistGridX[y][x] * m_nScaleFactor;
				nPointBY = m_pAstroImage->m_vectDistGridY[y][x] * m_nScaleFactor;

				if( m_nZoomCount > 0.5 )
				{
					nPointBX = (int) round( (nPointBX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
					nPointBY = (int) round( (nPointBY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
				}
				vectPoints[x].x = m_nPicOrigX+nPointBX;
				vectPoints[x].y = m_nPicOrigY+nPointBY;
			}
			// draw spline
			dc.DrawSpline( m_pAstroImage->m_nDistGridXUnits, vectPoints );
		}
		// :: vertical lines
		for( x=0; x<m_pAstroImage->m_nDistGridXUnits; x++ )
		{
			for( y=0; y<m_pAstroImage->m_nDistGridYUnits; y++ )
			{
				nPointBX = m_pAstroImage->m_vectDistGridX[y][x] * m_nScaleFactor;
				nPointBY = m_pAstroImage->m_vectDistGridY[y][x] * m_nScaleFactor;

				if( m_nZoomCount > 0.5 )
				{
					nPointBX = (int) round( (nPointBX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
					nPointBY = (int) round( (nPointBY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
				}
				vectPoints[y].x = m_nPicOrigX+nPointBX;
				vectPoints[y].y = m_nPicOrigY+nPointBY;
			}
			// draw spline
			dc.DrawSpline( m_pAstroImage->m_nDistGridYUnits, vectPoints );
		}
		free( vectPoints );
	}

	// restore current pen
	dc.SetPen( currentPen );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawSkyGrid
// Class:	CAstroImgView
// Purpose:	draw sky grid
// Input:	units ??
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::DrawSkyGrid( wxDC& dc )
{
	int x=0, y=0;
	double nStarAX = 0;
	double nStarAY = 0;
	double nStarBX = 0;
	double nStarBY = 0;
	wxGraphicsPath gpath;

	///////////////////////////////////////////////
	//  save current pen
	wxPen currentPen = dc.GetPen();
	// set grid pen
	wxColour colGrid = m_pMainConfig->colGraphicsSkyGrid;
	wxPen penDrawGrid;
	// get line style
	int nLineStyle = m_pMainConfig->m_nGraphicsSkyGridLine;
	// set color
	penDrawGrid.SetColour( colGrid );
	// set line stype
	if( nLineStyle == GRAPHICS_LINE_STYLE_NORMAL )
		penDrawGrid.SetStyle( wxSOLID );
	else if( nLineStyle == GRAPHICS_LINE_STYLE_DOTTED )
		penDrawGrid.SetStyle( wxDOT );
	else if( nLineStyle == GRAPHICS_LINE_STYLE_LDOTTED )
		penDrawGrid.SetStyle( wxSHORT_DASH );
	// set dc pen
	dc.SetPen( penDrawGrid );

	if( SetGdcPropsFromDc(dc) ) 
	{
		m_dc->Clip( m_nPicOrigX, m_nPicOrigY, m_nPicWidth, m_nPicHeight );
		gpath = m_dc->CreatePath();
	}

	/////////////////////////////////////
	// DRAW PART
	for( y=0; y<m_nDecUnits; y++ )
	{
		for( x=0; x<m_nRaUnits; x++ )
		{
			// debug
//			if( m_vectPrevLine[y][x].x == 0 && m_vectPrevLine[y][x].y == 0 )
//				continue;

			nStarBX = m_vectPrevLine[y][x].x * m_nScaleFactor;
			nStarBY = m_vectPrevLine[y][x].y * m_nScaleFactor;

			if( m_nZoomCount > 0.5 )
			{
				nStarBX = (int) round( (nStarBX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
				nStarBY = (int) round( (nStarBY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
			}

			// check if to draw horizontal line
			if( x > 0 && ( m_vectFlagInPic[y][x] || m_vectFlagInPic[y][x-1] ) )
			{
				// also calculate star ax
				nStarAX = m_vectPrevLine[y][x-1].x * m_nScaleFactor;
				nStarAY = m_vectPrevLine[y][x-1].y * m_nScaleFactor;

				if( m_nZoomCount > 0.5 )
				{
					nStarAX = (int) round( (nStarAX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
					nStarAY = (int) round( (nStarAY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
				}

				// draw line
				if( m_dc )
				{
					gpath.MoveToPoint( m_nPicOrigX+nStarAX, m_nPicOrigY+nStarAY );
					gpath.AddLineToPoint( m_nPicOrigX+nStarBX, m_nPicOrigY+nStarBY );

				} else
				{
					dc.DrawLine( m_nPicOrigX+nStarAX, m_nPicOrigY+nStarAY, 
									m_nPicOrigX+nStarBX, m_nPicOrigY+nStarBY );
				}
			}
			// draw vertical
			if( y > 0 && ( m_vectFlagInPic[y][x] || m_vectFlagInPic[y-1][x] ) )
			{
				// also calculate star ax
				nStarAX = m_vectPrevLine[y-1][x].x * m_nScaleFactor;
				nStarAY = m_vectPrevLine[y-1][x].y * m_nScaleFactor;

				if( m_nZoomCount > 0.5 )
				{
					nStarAX = (int) round( (nStarAX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
					nStarAY = (int) round( (nStarAY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
				}

				// draw line
				if( m_dc )
				{
					gpath.MoveToPoint( m_nPicOrigX+nStarAX, m_nPicOrigY+nStarAY );
					gpath.AddLineToPoint( m_nPicOrigX+nStarBX, m_nPicOrigY+nStarBY );

				} else
				{
					dc.DrawLine( m_nPicOrigX+nStarAX, m_nPicOrigY+nStarAY, 
									m_nPicOrigX+nStarBX, m_nPicOrigY+nStarBY );
				}
			}

		}
	}

	if( m_dc ) 
	{
		m_dc->DrawPath( gpath );
		m_dc->ResetClip();
	}

	// restore current pen
	dc.SetPen( currentPen );
}

////////////////////////////////////////////////////////////////////
// Method:	SetSkyGrid
// Class:	CAstroImgView
// Purpose:	set sky grid
// Input:	units ??
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetSkyGrid( )
{
	if( !m_pSky || !m_pAstroImage || !m_pAstroImage->m_pSky ) return;

	double ra = 0, dec=0;
	int i=0,j=0;
	double nDecDiv = (double) m_pAstroImage->m_nFieldHeight/20.0;
	double nRaDiv = (double) m_pAstroImage->m_nFieldWidth/10.0;
	StarDef star;
	double nRaSize = 180;
	// excepption center
	if( m_pAstroImage->m_nOrigDec > -35.0 && m_pAstroImage->m_nOrigDec < 35.0 )
	{
		nRaSize = 90.0;
	}
	// matrix of point - calculate how many on ra/dec
	int nRaUnits = (int) ( (double) (nRaSize*2.0)/nRaDiv + 20 );
	double nDecStart = m_pAstroImage->m_nDecStart - m_pAstroImage->m_nFieldHeight;
	// check if less the -86.0
	if( nDecStart < -86.0 ) nDecStart = -86.0;
	double nDecSize = m_pAstroImage->m_nFieldHeight + 2.0*m_pAstroImage->m_nFieldHeight;
	int nDecUnits = (int) ( (double) nDecSize/nDecDiv + 20);
	// allocate vectors
	wxPoint** vectPrevLine = (wxPoint**) malloc( nDecUnits*sizeof(wxPoint*) );
	for(i=0;i<nDecUnits;i++ ) vectPrevLine[i] = (wxPoint*) malloc( nRaUnits*sizeof(wxPoint) );
	char** vectFlagInPic = (char**) malloc( nDecUnits*sizeof(char*) );
	for(i=0;i<nDecUnits;i++ ) vectFlagInPic[i] = (char*) malloc( nRaUnits*sizeof(char) );
	// reset
	for(i=0;i<nDecUnits;i++ ) 
	{
		memset( vectFlagInPic[i], 0, nRaUnits*sizeof(char) );
		for(j=0;j<nRaUnits;j++ ) 
		{
			vectPrevLine[i][j].x = 0;
			vectPrevLine[i][j].y = 0;
		}
	}

	// set min/max area
	int nMinCountRa = INT_MAX;
	int nMaxCountRa = INT_MIN;
	int nMinCountDec = INT_MAX;
	int nMaxCountDec = INT_MIN;

	double nStarAX = 0;
	double nStarAY = 0;
	double nStarBX = 0;
	double nStarBY = 0;

	int nCountDec = 0;

	for( dec=nDecStart; dec<nDecStart+nDecSize && dec<86.0; dec+=nDecDiv )
	{
		int bFlagRaGotStart = 0;
		int nCountRa = 0;
		star.dec = dec;
		int bStopRaLoop = 0;

		for( ra=m_pAstroImage->m_nOrigRa-nRaSize; ra<m_pAstroImage->m_nOrigRa+nRaSize+nRaDiv && bStopRaLoop == 0; ra+=nRaDiv )
//		for( ra=0.0; ra<360.0; ra+=nRaDiv )
//		for( ra=m_pAstroImage->m_nRaStart-m_pAstroImage->m_nFieldWidth/2; 
//				ra<m_pAstroImage->m_nRaEnd+m_pAstroImage->m_nFieldWidth/2; ra+=nRaDiv )
		{
			int bFlagPointInPic = 1;

			// do this for the last line exeception
			if( ra>m_pAstroImage->m_nOrigRa+nRaSize )
			{
				ra = m_pAstroImage->m_nOrigRa-nRaSize;
				bStopRaLoop = 1;
				//continue;
			}

			star.ra = ra;
			m_pSky->ProjectStar( star, m_pAstroImage->m_nOrigRa, m_pAstroImage->m_nOrigDec );

			star.x = star.x*m_pAstroImage->m_nSkyProjectionScale;
			star.y = star.y*m_pAstroImage->m_nSkyProjectionScale;

			m_pSky->TransObj_SkyToImage( star );

			// check if in my image - else skip
			if( star.x < 0 || star.y < 0 || star.x > m_pAstroImage->m_nWidth || star.y > m_pAstroImage->m_nHeight )
				bFlagPointInPic = 0;
			else
				bFlagPointInPic = 1;

			if( nCountRa > 0 && ( bFlagPointInPic || vectFlagInPic[nCountDec][nCountRa-1] ) )
			{
				// set min/max count ra
				if( nCountRa-1 < nMinCountRa ) nMinCountRa = nCountRa-1;
				if( nCountRa > nMaxCountRa ) nMaxCountRa = nCountRa;
				if( nCountDec < nMinCountDec ) nMinCountDec = nCountDec;
				if( nCountDec > nMaxCountDec ) nMaxCountDec = nCountDec;

			}
			// draw vertical
			if( nCountDec > 0 && ( bFlagPointInPic || vectFlagInPic[nCountDec-1][nCountRa] ) )
			{
				// set min/max count ra
				if( nCountRa < nMinCountRa ) nMinCountRa = nCountRa;
				if( nCountRa > nMaxCountRa ) nMaxCountRa = nCountRa;
				if( nCountDec < nMinCountDec ) nMinCountDec = nCountDec;
				if( nCountDec > nMaxCountDec ) nMaxCountDec = nCountDec;

			}
	
			//keep points for vertical
			vectFlagInPic[nCountDec][nCountRa] = bFlagPointInPic;
			vectPrevLine[nCountDec][nCountRa].x = (int) star.x;
			vectPrevLine[nCountDec][nCountRa].y = (int) star.y;

			nCountRa++;

		}
		//////////////////
		// here draw last line if at least one point in the picture
/*		if( vectFlagInPic[nCountDec][0] || vectFlagInPic[nCountDec][nCountRa-1] )
		{
			dc.DrawLine( m_nPicOrigX+vectPrevLine[nCountDec][0].x, 
							m_nPicOrigY+vectPrevLine[nCountDec][0].y, 
							m_nPicOrigX+vectPrevLine[nCountDec][nCountRa-1].x, 
							m_nPicOrigY+vectPrevLine[nCountDec][nCountRa-1].y );
		}
*/		

		nCountDec++;
	}

	//////////////////////////////////////////
	// ALLOCATE MIN GRID
	int x=0, y=0;
	m_nRaUnits = nMaxCountRa-nMinCountRa+1;
	m_nDecUnits = nMaxCountDec-nMinCountDec+1;
	// allocate vectors
	m_vectPrevLine = (wxPoint**) malloc( m_nDecUnits*sizeof(wxPoint*) );
	for(i=0;i<m_nDecUnits;i++ ) m_vectPrevLine[i] = (wxPoint*) malloc( m_nRaUnits*sizeof(wxPoint) );
	m_vectFlagInPic = (char**) malloc( m_nDecUnits*sizeof(char*) );
	for(i=0;i<m_nDecUnits;i++ ) m_vectFlagInPic[i] = (char*) malloc( m_nRaUnits*sizeof(char) );
	// reset
	for(i=0;i<m_nDecUnits;i++ ) 
	{
		memset( m_vectFlagInPic[i], 0, m_nRaUnits*sizeof(char) );
		for(j=0;j<m_nRaUnits;j++ ) 
		{
			m_vectPrevLine[i][j].x = 0;
			m_vectPrevLine[i][j].y = 0;
		}
	}

	// transfer to mingrid
	for( y=nMinCountDec; y<=nMaxCountDec; y++ )
	{
		for( x=nMinCountRa; x<=nMaxCountRa; x++ )
		{
//			if( vectPrevLine[y][x].x == 0 && vectPrevLine[y][x].y == 0 )
//				continue;

			// set coords
			m_vectPrevLine[y-nMinCountDec][x-nMinCountRa].x = vectPrevLine[y][x].x;
			m_vectPrevLine[y-nMinCountDec][x-nMinCountRa].y = vectPrevLine[y][x].y;
			// set flag
			m_vectFlagInPic[y-nMinCountDec][x-nMinCountRa] = vectFlagInPic[y][x];
		}
	}

	// free temp allocated
	for(i=0;i<nDecUnits;i++ ) free( vectPrevLine[i] );
	free( vectPrevLine );
	for(i=0;i<nDecUnits;i++ ) free( vectFlagInPic[i] );
	free( vectFlagInPic );

	// set show sky grid flag
	m_bShowSkyGrid = 1;

	Refresh();

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	DeleteSkyGrid
// Class:	CAstroImgView
// Purpose:	delete sky grid
// Input:	units ??
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::DeleteSkyGrid( )
{
	m_bShowSkyGrid = 0;
	int i=0;

	// free main vectors - to move this in close image - stop grid
	if( m_vectPrevLine )
	{
		for(i=0;i<m_nDecUnits;i++ ) free( m_vectPrevLine[i] );
		free( m_vectPrevLine );
		m_vectPrevLine = NULL;
	}

	if( m_vectFlagInPic )
	{
		for(i=0;i<m_nDecUnits;i++ ) free( m_vectFlagInPic[i] );
		free( m_vectFlagInPic );
		m_vectFlagInPic = NULL;
	}

	m_nDecUnits = 0;
	m_nRaUnits = 0;

	Refresh();
}

////////////////////////////////////////////////////////////////////
// Method:	DrawSkyObjectAndLabel
// Class:	CAstroImgView
// Purpose:	draw a a general purpose sky object and atatched label
// Input:	reference to dc,  coordinates
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawSkyObjectAndLabel( wxDC& dc, int nStarId, int nMatch, int nType, int objState,
											double nX, double nY, double nA, double nB, 
											double nTheta, wxString strName )
{
	// if out of canvas return
	if( m_nPicOrigX+nX < 0 || m_nPicOrigY+nY < 0 ||
		m_nPicOrigX+nX >m_nWidth || m_nPicOrigY+nY > m_nHeight ) return( 0 );

	wxColour colDraw;
	wxFont fontDraw;
	int nShapeType=0;
	int nLineType=0;
	int bEllipseLineType=0;

	///////////////////////////////////////////////
	//  save current pen
	wxPen currentPen = dc.GetPen();
	// set pen used to draw dso
	wxPen penDrawDMObj;

	// get style
	m_pMainConfig->GetStarStyle( nMatch, nType, colDraw, fontDraw, nLineType, nShapeType );

	/////////////////
	// process line type
	if( nLineType == SHAPE_LINE_STYLE_NORMAL )
	{
		bEllipseLineType = 0;
		penDrawDMObj.SetStyle( wxSOLID );

	} else if( nLineType == SHAPE_LINE_STYLE_DOTTED )
	{
		bEllipseLineType = 1;
		penDrawDMObj.SetStyle( wxDOT );
	}

	// get object font/color by type - detected/matched
	if( ( objState & OBJECT_STATE_SELECTED ) && m_bIsDistanceTool )
	{
			colDraw.Set( 0, 255, 255 );
	}

	////////////////////////////////////
	// HERE WE START TO DRAW
	// set font & color	
	dc.SetTextForeground( colDraw );
	dc.SetFont( fontDraw );
	penDrawDMObj.SetColour( colDraw );
	// now set my pen
	dc.SetPen( penDrawDMObj );

	SetGdcPropsFromDc(dc);

	// if line different then none draw my dso contour/icon
	if( nLineType != SHAPE_LINE_STYLE_NONE )
	{
		if( nShapeType == SHAPE_TYPE_REAL_ELLIPSE )
		{
			// try here to draw the ellipse instead
			DrawEllips( dc, m_nPicOrigX+nX+1, m_nPicOrigY+nY+1,
							nA, nB, m_pAstroImage->m_vectStar[nStarId].theta, bEllipseLineType );

			// some stuff with crowded here ???
			// m_vectStar[nStarId].flag&OBJ_CROWDED );

		} else if( nShapeType == SHAPE_TYPE_CIRCLE_ICON )
		{
			dc.DrawCircle( m_nPicOrigX+nX+1, m_nPicOrigY+nY+1, 5 );

		} else if( nShapeType == SHAPE_TYPE_SQUARE_ICON )
		{
			dc.DrawRectangle( (int) round( (m_nPicOrigX+nX)-3 ), 
								(int)round( (m_nPicOrigY+nY)-3 ), 7, 7 );

		} else if( nShapeType == SHAPE_TYPE_TRIANGLE_ICON )
		{
			wxPoint points[3];
			points[0].x = round( m_nPicOrigX+nX );
			points[0].y = round( m_nPicOrigY+nY );
			points[1].x = points[0].x+8;
			points[1].y = points[0].y+8;
			points[2].x = points[0].x-8;
			points[2].y = points[0].y+8;
			// draw triangle - offset with +3 on y
			dc.DrawPolygon( 3, points, 0, 3 );

		} else if( nShapeType == SHAPE_TYPE_CROSS_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nX );
			int nObjY = (int) round( m_nPicOrigY+nY );
			dc.DrawLine( nObjX, nObjY-5, nObjX, nObjY+5 );
			dc.DrawLine( nObjX-5, nObjY, nObjX+5, nObjY );

		} else if( nShapeType == SHAPE_TYPE_TARGET_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nX );
			int nObjY = (int) round( m_nPicOrigY+nY );
			dc.DrawLine( nObjX, nObjY-7, nObjX, nObjY+7 );
			dc.DrawLine( nObjX-7, nObjY, nObjX+7, nObjY );
			dc.DrawCircle( nObjX, nObjY, 5 );
		}
	}

	///////////////////////////////
	// draw text label
	if( strName.Length() != 0  )
	{
		int tx = (int) nX;
		int ty = (int) nY;
		GetObjLabelPos( dc, 0, nStarId, tx, ty );

		// larry :: attempt to antialaiasing
		if( m_dc ) 
			m_dc->DrawText( m_vectStarName[nStarId], m_nPicOrigX+m_vectStarLabels[nStarId].x, m_nPicOrigY+m_vectStarLabels[nStarId].y );
		else
			dc.DrawText( m_vectStarName[nStarId], m_nPicOrigX+m_vectStarLabels[nStarId].x, m_nPicOrigY+m_vectStarLabels[nStarId].y );
	} 

	dc.SetPen( currentPen );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawStar
// Class:	CAstroImgView
// Purpose:	draw a normal stars detected in teh image
// Input:	reference to command event
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawStar( wxDC& dc, int nStarId )
{
	if( m_pAstroImage->m_nStar <=0 || nStarId < 0 || nStarId >= m_pAstroImage->m_nStar ) return( 0 );

//	char strName[255];


	////////////////////////////////
	// first we calculate position - size etc
	double x = m_pAstroImage->m_vectStar[nStarId].x;
	double y = m_pAstroImage->m_vectStar[nStarId].y;
	double a = m_pAstroImage->m_vectStar[nStarId].a;
	double b = m_pAstroImage->m_vectStar[nStarId].b;
	double kronfactor = m_pAstroImage->m_vectStar[nStarId].kronfactor;

	// scale area data from picture size to win size
	double nStarX = x*m_nScaleFactor;
	double nStarY = y*m_nScaleFactor;
	// ellipse factors
	double nStarA = a*kronfactor*m_nScaleFactor;
	double nStarB = b*kronfactor*m_nScaleFactor;

	if( m_nZoomCount > 0.5 )
	{
		nStarX = (int) round( (nStarX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
		nStarY = (int) round( (nStarY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );

		// adjust ellipse zoom factors 
		nStarA = nStarA*m_nLastZoomFactor;
		nStarB = nStarB*m_nLastZoomFactor;
	}
	
	// draw my object/label
	DrawSkyObjectAndLabel( dc, nStarId, m_pAstroImage->m_vectStar[nStarId].match, 
								m_pAstroImage->m_vectStar[nStarId].type,
								m_pAstroImage->m_vectStar[nStarId].state,
								nStarX, nStarY, nStarA, nStarB,
								m_pAstroImage->m_vectStar[nStarId].theta,
								m_vectStarName[nStarId] );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CAstroImgView::GetObjLabelPos( wxDC& dc, int otype, int id, int x, int y )
{
	int i=0;
	unsigned char ret=1;
	int nFoundPos = 1;
	int rw=0, rh=0;
	unsigned char rpos_mask = 0;
	wxSize txtSize;
	structObjLabel* vectLabels;
	int nShiftRX=7, nShiftRY=1;
	int nShiftLX=7, nShiftLY=1;

// 		m_nPicOrigX+nDsoX+1+3+6, m_nPicOrigY+nDsoY+1+3

	// first we calculate text size
	// get by type 0=detected star, 1=catalog stars, 2=dso
	if( otype == 0 )
	{
		txtSize = dc.GetTextExtent( m_vectStarName[id] );
		vectLabels = m_vectStarLabels;

	} else if( otype == 1 )
	{
		txtSize = dc.GetTextExtent( m_vectStarName[id] );
		vectLabels = m_vectCatStarLabels;

	} else if( otype == 2 )
	{
		txtSize = dc.GetTextExtent( m_vectStarName[id] );
		vectLabels = m_vectDsoLabels;
		nShiftRX = 10;
		nShiftRY = 4;

	} else 
	{
		// for any other types just return
		return(0);
	}

	
	rw = txtSize.GetWidth();
	rh = txtSize.GetHeight();

	// also make adjustments if default is out ?? - start in pos 1 
	// default all points
	rpos_mask = 15;
	if( x+rw+7 > m_nWidth )
		rpos_mask &= 6; // 2 + 3(4)
	else if( x-rw-7 < 0 )
		rpos_mask &= 9; // 1 + 4(8)
	// also on the vertical
	if( y+rh+1 > m_nHeight )
		rpos_mask &= 12; // 3(4) + 4(8)
	else if( y-rh-1 < 0 )
		rpos_mask &= 3; // 1 + 2

	// calculate the four rectangles for this position
	wxRect o_1_rect( x+nShiftRX, y+nShiftRY, rw, rh );
	wxRect o_2_rect( x-rw-nShiftLX, y+nShiftRY, rw, rh );
	wxRect o_3_rect( x-rw-nShiftLX, y-rh-nShiftLY, rw, rh );
	wxRect o_4_rect( x+rw+nShiftRX, y-rh-nShiftLY, rw, rh );

	/////////////////////////////////////////
	// DETECTED STARS
	if( id > 0 && id < m_nStarLabels )
		nFoundPos = CAstroImgView_GetObjLabelPos( id, m_vectStarLabels, rpos_mask, o_1_rect, o_2_rect, o_3_rect, o_4_rect );

	/////////////////////////////////////////
	// CATALOG STARS
	if( id > 0 && id < m_nCatStarLabels )
		nFoundPos = CAstroImgView_GetObjLabelPos( id, m_vectCatStarLabels, rpos_mask, o_1_rect, o_2_rect, o_3_rect, o_4_rect );

	/////////////////////////////////////////
	// DSO
	if( id > 0 && id < m_nDsoLabels )
		nFoundPos = CAstroImgView_GetObjLabelPos( id, m_vectDsoLabels, rpos_mask, o_1_rect, o_2_rect, o_3_rect, o_4_rect );

	/////////////////////////////
	// set label pos/size by position id found
	if( nFoundPos <= 1 || nFoundPos > 4 )
	{
		vectLabels[id].x = o_1_rect.x;
		vectLabels[id].y = o_1_rect.y;
		vectLabels[id].w = o_1_rect.width;
		vectLabels[id].h = o_1_rect.height;

	} else if( nFoundPos == 2 )
	{
		vectLabels[id].x = o_2_rect.x;
		vectLabels[id].y = o_2_rect.y;
		vectLabels[id].w = o_2_rect.width;
		vectLabels[id].h = o_2_rect.height;

	} else if( nFoundPos == 3 )
	{
		vectLabels[id].x = o_3_rect.x;
		vectLabels[id].y = o_3_rect.y;
		vectLabels[id].w = o_3_rect.width;
		vectLabels[id].h = o_3_rect.height;

	} else if( nFoundPos == 4 )
	{
		vectLabels[id].x = o_4_rect.x;
		vectLabels[id].y = o_4_rect.y;
		vectLabels[id].w = o_4_rect.width;
		vectLabels[id].h = o_4_rect.height;
	} 

	return( ret );
}

////////////////////////////////////////////////////////////////////
inline int CAstroImgView_GetObjLabelPos( int id, structObjLabel* vectLabels, unsigned char rpos_mask,
											wxRect o_1_rect, wxRect o_2_rect, 
											wxRect o_3_rect, wxRect o_4_rect  )
{
	int i=0;
	bool bFoundPos=false;
	int nFoundPos = 1;
	int ox=0, oy=0, ow=0, oh=0, opos=0;
	structObjLabel* pLabel;

	/////////////////////////////////////////
	// DETECTED STARS
	for( i=0; i<id; i++ )
	{
		pLabel = &(vectLabels[i]);
		ox = pLabel->x;
		ox = pLabel->y;
		ow = pLabel->w;
		oh = pLabel->h;
		opos = pLabel->n_pos;

		//if( i == id ) continue;
		if( ow == 0 || oh == 0 ) continue;

		bFoundPos = false;
		// get the rectangle for this object
		wxRect trect( ox, oy, ow, oh );

		//////////////
		// CHECK POSITION 1
		if( nFoundPos <= 1 && (rpos_mask & 1) && !trect.Intersects( o_1_rect ) )
		{
			nFoundPos = 1;
			bFoundPos = true; 
		}

		// IF NOT FOUND -> check point 2
		if( !bFoundPos && nFoundPos <= 2 && (rpos_mask & 2) && !trect.Intersects( o_2_rect ) )
		{
			nFoundPos = 2;
			bFoundPos = true; 
		} 

		// IF NOT FOUND -> check point 3
		if( !bFoundPos && nFoundPos <= 3 && (rpos_mask & 4) && !trect.Intersects( o_3_rect ) )
		{
			nFoundPos = 3;
			bFoundPos = true; 
		} 

		// IF NOT FOUND -> check point 4
		if( !bFoundPos && nFoundPos <= 4 && (rpos_mask & 8) && !trect.Intersects( o_4_rect ) )
		{
			nFoundPos = 4;
			bFoundPos = true; 
		} 

		// at last if not found just set it to point 1
		//if( !bFoundPos ) nFoundPos = 1;
	}

	return( nFoundPos );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawCatStar
// Class:	CAstroImgView
// Purpose:	draw a catalog stars detected in teh image
// Input:	reference to dc, star id
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawCatStar( wxDC& dc, int nStarId )
{
	if( m_pAstroImage->m_nCatStar <= 0 || nStarId < 0 || nStarId >= m_pAstroImage->m_nCatStar ) return( 0 );

	char strName[255];
	wxColour colDraw;
	wxFont fontDraw;
	int nShapeType=0;
	int nLineType=0;
	int bEllipseLineType=0;

	////////////////////////////////
	// first we calculate position - size etc
	double x = m_pAstroImage->m_vectCatStar[nStarId].x;
	double y = m_pAstroImage->m_vectCatStar[nStarId].y;
	double a = m_pAstroImage->m_vectCatStar[nStarId].a;
	double b = m_pAstroImage->m_vectCatStar[nStarId].b;
	double kronfactor = m_pAstroImage->m_vectCatStar[nStarId].kronfactor;

	// scale area data from picture size to win size
	double nStarX = x*m_nScaleFactor;
	double nStarY = y*m_nScaleFactor;
	// ellipse factors
	double nStarA = a*kronfactor*m_nScaleFactor;
	double nStarB = b*kronfactor*m_nScaleFactor;

	if( m_nZoomCount > 0.5 )
	{
		nStarX = (int) round( (nStarX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
		nStarY = (int) round( (nStarY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );

		// adjust ellipse zoom factors 
		nStarA = nStarA*m_nLastZoomFactor;
		nStarB = nStarB*m_nLastZoomFactor;
	}
	
	// if out of canvas return
	if( m_nPicOrigX+nStarX < 0 || m_nPicOrigY+nStarY < 0 ||
		m_nPicOrigX+nStarX >m_nWidth || m_nPicOrigY+nStarY > m_nHeight ) return( 0 );

	///////////////////////////////////////////////
	//  save current pen
	wxPen currentPen = dc.GetPen();
	// set pen used to draw dso
	wxPen penDrawCatObj;

	// get catalog star font//color shape/line etc
	colDraw = m_pMainConfig->colorCObjLabel;
	fontDraw = m_pMainConfig->fontCObjLabel;
	nShapeType = m_pMainConfig->m_nCatObjShapeType;
	nLineType = m_pMainConfig->m_nCatObjShapeLine;

	/////////////////
	// process line type
	if( nLineType == SHAPE_LINE_STYLE_NORMAL )
	{
		bEllipseLineType = 0;
		penDrawCatObj.SetStyle( wxSOLID );

	} else if( nLineType == SHAPE_LINE_STYLE_DOTTED )
	{
		bEllipseLineType = 1;
		penDrawCatObj.SetStyle( wxDOT );
	}

	// get object font/color by type - detected/matched
	if( ( m_pAstroImage->m_vectCatStar[nStarId].state & OBJECT_STATE_SELECTED ) && m_bIsDistanceTool )
	{
			colDraw.Set( 0, 255, 255 );
	}

	// set font & color
	dc.SetTextForeground( colDraw );
	dc.SetFont( fontDraw );
	penDrawCatObj.SetColour( colDraw );
	// now set my pen
	dc.SetPen( penDrawCatObj );

	// if line different then none draw my dso contour/icon
	if( nLineType != SHAPE_LINE_STYLE_NONE )
	{
		if( nShapeType == SHAPE_TYPE_REAL_ELLIPSE )
		{
			// dra simple circle - sinc cat star doesn't have ellipse factors
			dc.DrawCircle( m_nPicOrigX+nStarX+1, m_nPicOrigY+nStarY+1, nStarA );

		} else if( nShapeType == SHAPE_TYPE_CIRCLE_ICON )
		{
			dc.DrawCircle( m_nPicOrigX+nStarX+1, m_nPicOrigY+nStarY+1, 5 );

		} else if( nShapeType == SHAPE_TYPE_SQUARE_ICON )
		{
			dc.DrawRectangle( (int) round(m_nPicOrigX+nStarX-3), 
								(int) round(m_nPicOrigY+nStarY-3), 7, 7 );

		} else if( nShapeType == SHAPE_TYPE_TRIANGLE_ICON )
		{
			wxPoint points[3];
			points[0].x = (int) round( m_nPicOrigX+nStarX );
			points[0].y = (int) round( m_nPicOrigY+nStarY );
			points[1].x = points[0].x+8;
			points[1].y = points[0].y+8;
			points[2].x = points[0].x-8;
			points[2].y = points[0].y+8;
			// draw triangle - offset with +3 on y
			dc.DrawPolygon( 3, points, 0, 3 );

		} else if( nShapeType == SHAPE_TYPE_CROSS_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nStarX );
			int nObjY = (int) round( m_nPicOrigY+nStarY );
			dc.DrawLine( nObjX, nObjY-5, nObjX, nObjY+5 );
			dc.DrawLine( nObjX-5, nObjY, nObjX+5, nObjY );

		} else if( nShapeType == SHAPE_TYPE_TARGET_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nStarX );
			int nObjY = (int) round( m_nPicOrigY+nStarY );
			dc.DrawLine( nObjX, nObjY-7, nObjX, nObjY+7 );
			dc.DrawLine( nObjX-7, nObjY, nObjX+7, nObjY );
			dc.DrawCircle( nObjX, nObjY, 5 );
		}
	}

	///////////////////////////////
	// draw text label
	if( m_vectCatStarName[nStarId].Length() > 0 )
	{
		// check if if name goes out over margin
//		dc.DrawText( m_vectCatStarName[nStarId], m_nPicOrigX+nStarX+7, m_nPicOrigY+nStarY+1 );

		int tx = (int) nStarX;
		int ty = (int) nStarY;
		GetObjLabelPos( dc, 1, nStarId, tx, ty );
		dc.DrawText( m_vectCatStarName[nStarId], m_nPicOrigX+m_vectCatStarLabels[nStarId].x, m_nPicOrigY+m_vectCatStarLabels[nStarId].y );
	} 

	dc.SetPen( currentPen );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawDso
// Class:	CAstroImgView
// Purpose:	draw dso object
// Input:	reference to dc and dso obj id
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawDso( wxDC& dc, int nDsoId )
{
	if( m_pAstroImage->m_nCatDso <= 0 || nDsoId < 0 ||
		nDsoId >= m_pAstroImage->m_nCatDso ) return( 0 );

	char strLabel[255];
	wxColour colDraw;
	wxFont fontDraw;
	int nShapeType=0;
	int nLineType=0;
	int bEllipseLineType=0;

	//////////////////////////////
	// scale area data from picture size to win size
	double nDsoX = m_pAstroImage->m_vectCatDso[nDsoId].x*m_nScaleFactor;
	double nDsoY = m_pAstroImage->m_vectCatDso[nDsoId].y*m_nScaleFactor;

	// prepare a or b
	double a = 1;
	double b = 1;
	// if not a then b
	if( m_pAstroImage->m_vectCatDso[nDsoId].a != 0 ) 
		a = m_pAstroImage->m_vectCatDso[nDsoId].a;
	else if( m_pAstroImage->m_vectCatDso[nDsoId].b != 0 )
		a = m_pAstroImage->m_vectCatDso[nDsoId].b;
	// if not b then a
	if( m_pAstroImage->m_vectCatDso[nDsoId].b != 0 ) 
		b = m_pAstroImage->m_vectCatDso[nDsoId].b;
	else if( m_pAstroImage->m_vectCatDso[nDsoId].a != 0 )
		b = m_pAstroImage->m_vectCatDso[nDsoId].a;

	// use instead :: m_nImgTransScale
	double nStarA = a*m_nScaleFactor;
	double nStarB = b*m_nScaleFactor;

	if( m_nZoomCount > 0.5 )
	{
		double nZoomFactor = m_nZoomCount*1.1;
		nDsoX = (nDsoX-m_nCutX*m_nScaleFactor)*nZoomFactor;
		nDsoY = (nDsoY-m_nCutY*m_nScaleFactor)*nZoomFactor;

		// adjust ellipse zoom factors 
		nStarA = nStarA*nZoomFactor;
		nStarB = nStarB*nZoomFactor;
	}

	// if out of canvas return
	if( m_nPicOrigX+nDsoX < 0 || m_nPicOrigY+nDsoY < 0 ||
		m_nPicOrigX+nDsoX >m_nWidth || m_nPicOrigY+nDsoY > m_nHeight ) return( 0 );

	//////////////
	// save current pen
	wxPen currentPen = dc.GetPen();
	// pen used to draw dso
	wxPen penDrawDso;

	////////////////////
	// set font/color and shape/line by type
	if( m_pAstroImage->m_vectCatDso[nDsoId].type == SKY_OBJECT_TYPE_NEBULA )
	{
		colDraw = m_pMainConfig->colorNebObjLabel;
		fontDraw = m_pMainConfig->fontNebObjLabel;
		nShapeType = m_pMainConfig->m_nNebObjShapeType;
		nLineType = m_pMainConfig->m_nNebObjShapeLine;

	} else if( m_pAstroImage->m_vectCatDso[nDsoId].type == SKY_OBJECT_TYPE_CLUSTER )
	{
		colDraw = m_pMainConfig->colorCluObjLabel;
		fontDraw = m_pMainConfig->fontCluObjLabel;
		nShapeType = m_pMainConfig->m_nCluObjShapeType;
		nLineType = m_pMainConfig->m_nCluObjShapeLine;

	} else if( m_pAstroImage->m_vectCatDso[nDsoId].type == SKY_OBJECT_TYPE_GALAXY )
	{
		colDraw = m_pMainConfig->colorGalObjLabel;
		fontDraw = m_pMainConfig->fontGalObjLabel;
		nShapeType = m_pMainConfig->m_nGalObjShapeType;
		nLineType = m_pMainConfig->m_nGalObjShapeLine;

	} else 
	{
		colDraw = m_pMainConfig->colorGalObjLabel;
		fontDraw = m_pMainConfig->fontGalObjLabel;
		nShapeType = m_pMainConfig->m_nGalObjShapeType;
		nLineType = m_pMainConfig->m_nGalObjShapeLine;
	}

	/////////////////
	// process line type
	if( nLineType == SHAPE_LINE_STYLE_NORMAL )
	{
		bEllipseLineType = 0;
		penDrawDso.SetStyle( wxSOLID );

	} else if( nLineType == SHAPE_LINE_STYLE_DOTTED )
	{
		bEllipseLineType = 1;
		penDrawDso.SetStyle( wxDOT );
	}

	// if selected for distance tool
	if( ( m_pAstroImage->m_vectCatDso[nDsoId].state & OBJECT_STATE_SELECTED ) && m_bIsDistanceTool )
	{
			colDraw.Set( 0, 255, 255 );
	}

	// set fonts and colors for text
	dc.SetTextForeground( colDraw );
	dc.SetFont( fontDraw );
	penDrawDso.SetColour( colDraw );
	// set my pen
	dc.SetPen( penDrawDso );

	SetGdcPropsFromDc(dc);

	// if line different then none draw my dso contour/icon
	if( nLineType != SHAPE_LINE_STYLE_NONE )
	{
		if( nShapeType == SHAPE_TYPE_REAL_ELLIPSE )
		{
			// try here to draw the ellipse instead
			double d_ang = (double) (m_pAstroImage->m_nImgRotAngle - m_pAstroImage->m_vectCatDso[nDsoId].theta)+90.0;
			DrawEllips( dc, m_nPicOrigX+nDsoX+1, m_nPicOrigY+nDsoY+1,
							nStarA, nStarB,
							d_ang, bEllipseLineType );

			// test bounding box
			double c_x = m_nPicOrigX+nDsoX+1;
			double c_y = m_nPicOrigY+nDsoY+1;
			double nMaxW=DBL_MIN, nMaxH=DBL_MIN;

//			ellipseBoundingBox( c_x, c_y, nStarA, nStarB, d_ang, nMaxW, nMaxH );
//			dc.DrawRectangle( c_x-nMaxW, c_y-nMaxH, 2*nMaxW, 2*nMaxH );

		} else if( nShapeType == SHAPE_TYPE_CIRCLE_ICON )
		{
			dc.DrawCircle( m_nPicOrigX+nDsoX+1, m_nPicOrigY+nDsoY+1, 5 );

		} else if( nShapeType == SHAPE_TYPE_SQUARE_ICON )
		{
			dc.DrawRectangle( (int) round(m_nPicOrigX+nDsoX-3), 
								(int) round(m_nPicOrigY+nDsoY-3), 7, 7 );

		} else if( nShapeType == SHAPE_TYPE_TRIANGLE_ICON )
		{
			wxPoint points[3];
			points[0].x = round( m_nPicOrigX+nDsoX );
			points[0].y = round( m_nPicOrigY+nDsoY );
			points[1].x = points[0].x+8;
			points[1].y = points[0].y+8;
			points[2].x = points[0].x-8;
			points[2].y = points[0].y+8;
			// draw triangle - offset with +3 on y
			dc.DrawPolygon( 3, points, 0, 3 );

		} else if( nShapeType == SHAPE_TYPE_CROSS_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nDsoX );
			int nObjY = (int) round( m_nPicOrigY+nDsoY );
			dc.DrawLine( nObjX, nObjY-5, nObjX, nObjY+5 );
			dc.DrawLine( nObjX-5, nObjY, nObjX+5, nObjY );

		} else if( nShapeType == SHAPE_TYPE_TARGET_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nDsoX );
			int nObjY = (int) round( m_nPicOrigY+nDsoY );
			dc.DrawLine( nObjX, nObjY-7, nObjX, nObjY+7 );
			dc.DrawLine( nObjX-7, nObjY, nObjX+7, nObjY );
			dc.DrawCircle( nObjX, nObjY, 5 );
		}
	}

	/////////////////////////
	// draw text
	if( m_vectDsoName[nDsoId] != NULL )
	{
//		dc.DrawText( m_vectDsoName[nDsoId], m_nPicOrigX+nDsoX+1+3+6, m_nPicOrigY+nDsoY+1+3 );
		// here draw name from current vector instead

		int tx = (int) nDsoX;
		int ty = (int) nDsoY;
		GetObjLabelPos( dc, 2, nDsoId, tx, ty );

		if( m_dc )
			m_dc->DrawText( m_vectDsoName[nDsoId], m_nPicOrigX+m_vectDsoLabels[nDsoId].x, m_nPicOrigY+m_vectDsoLabels[nDsoId].y );
		else
			dc.DrawText( m_vectDsoName[nDsoId], m_nPicOrigX+m_vectDsoLabels[nDsoId].x, m_nPicOrigY+m_vectDsoLabels[nDsoId].y );
	}

	dc.SetPen( currentPen );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawRadioSource
// Class:	CAstroImgView
// Purpose:	draw radio source
// Input:	reference to dc and dso obj id
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawRadioSource( wxDC& dc, int nSourceId )
{
	if( m_pAstroImage->m_nCatRadio <= 0 || nSourceId < 0 ||
		nSourceId >= m_pAstroImage->m_nCatRadio ) return( 0 );

	char strLabel[255];
	wxColour colDraw;
	wxFont fontDraw;
	int nShapeType=0;
	int nLineType=0;
	int bEllipseLineType=0;

	//////////////////////////////
	// scale area data from picture size to win size
	double nSourceX = m_pAstroImage->m_vectCatRadio[nSourceId].x*m_nScaleFactor;
	double nSourceY = m_pAstroImage->m_vectCatRadio[nSourceId].y*m_nScaleFactor;

	// if zoom
	if( m_nZoomCount > 0.5 )
	{
		double nZoomFactor = m_nZoomCount*1.1;
		nSourceX = (nSourceX-m_nCutX*m_nScaleFactor)*nZoomFactor;
		nSourceY = (nSourceY-m_nCutY*m_nScaleFactor)*nZoomFactor;
	}

	// if out of canvas return
	if( m_nPicOrigX+nSourceX < 0 || m_nPicOrigY+nSourceY < 0 ||
		m_nPicOrigX+nSourceX >m_nWidth || m_nPicOrigY+nSourceY > m_nHeight ) return( 0 );

	//////////////
	// save current pen
	wxPen currentPen = dc.GetPen();
	// pen used to draw radio
	wxPen penDrawRadio;

	////////////////////
	// set font/color and shape/line by type
	if( m_pAstroImage->m_vectCatRadio[nSourceId].type == SKY_OBJECT_TYPE_RADIO )
	{
		colDraw = m_pMainConfig->colorRadObjLabel;
		fontDraw = m_pMainConfig->fontRadObjLabel;
		nShapeType = m_pMainConfig->m_nRadObjShapeType;
		nLineType = m_pMainConfig->m_nRadObjShapeLine;

	}

	/////////////////
	// process line type
	if( nLineType == SHAPE_LINE_STYLE_NORMAL )
	{
		bEllipseLineType = 0;
		penDrawRadio.SetStyle( wxSOLID );

	} else if( nLineType == SHAPE_LINE_STYLE_DOTTED )
	{
		bEllipseLineType = 1;
		penDrawRadio.SetStyle( wxDOT );
	}

	// set fonts and colors for text
	dc.SetTextForeground( colDraw );
	dc.SetFont( fontDraw );
	penDrawRadio.SetColour( colDraw );
	// set my pen
	dc.SetPen( penDrawRadio );

	// if line different then none draw my dso contour/icon
	if( nLineType != SHAPE_LINE_STYLE_NONE )
	{
		if( nShapeType == SHAPE_TYPE_REAL_ELLIPSE )
		{
			// try here to draw the ellipse instead
			double d_ang = (double) 0.0;
			DrawEllips( dc, m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1,
							5, 10,
							d_ang, bEllipseLineType );

		} else if( nShapeType == SHAPE_TYPE_CIRCLE_ICON )
		{
			dc.DrawCircle( m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1, 5 );

		} else if( nShapeType == SHAPE_TYPE_SQUARE_ICON )
		{
			dc.DrawRectangle( (int) round(m_nPicOrigX+nSourceX-3), 
								(int) round(m_nPicOrigY+nSourceY-3), 7, 7 );

		} else if( nShapeType == SHAPE_TYPE_TRIANGLE_ICON )
		{
			wxPoint points[3];
			points[0].x = round( m_nPicOrigX+nSourceX );
			points[0].y = round( m_nPicOrigY+nSourceY );
			points[1].x = points[0].x+8;
			points[1].y = points[0].y+8;
			points[2].x = points[0].x-8;
			points[2].y = points[0].y+8;
			// draw triangle - offset with +3 on y
			dc.DrawPolygon( 3, points, 0, 3 );

		} else if( nShapeType == SHAPE_TYPE_CROSS_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-5, nObjX, nObjY+5 );
			dc.DrawLine( nObjX-5, nObjY, nObjX+5, nObjY );

		} else if( nShapeType == SHAPE_TYPE_TARGET_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-7, nObjX, nObjY+7 );
			dc.DrawLine( nObjX-7, nObjY, nObjX+7, nObjY );
			dc.DrawCircle( nObjX, nObjY, 5 );
		}
	}

	/////////////////////////
	// draw text
	if( m_vectRadioName[nSourceId] != NULL )
	{
		dc.DrawText( m_vectRadioName[nSourceId], m_nPicOrigX+nSourceX+1+3+6, m_nPicOrigY+nSourceY+1+3 );
		// here draw name from current vector instead
	}

	dc.SetPen( currentPen );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawXGammaSource
// Class:	CAstroImgView
// Purpose:	draw radio source
// Input:	reference to dc and dso obj id
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawXGammaSource( wxDC& dc, int nSourceId )
{
	if( m_pAstroImage->m_nCatXGamma <= 0 || nSourceId < 0 ||
		nSourceId >= m_pAstroImage->m_nCatXGamma ) return( 0 );

	char strLabel[255];
	wxColour colDraw;
	wxFont fontDraw;
	int nShapeType=0;
	int nLineType=0;
	int bEllipseLineType=0;

	//////////////////////////////
	// scale area data from picture size to win size
	double nSourceX = m_pAstroImage->m_vectCatXGamma[nSourceId].x*m_nScaleFactor;
	double nSourceY = m_pAstroImage->m_vectCatXGamma[nSourceId].y*m_nScaleFactor;

	// if zoom
	if( m_nZoomCount > 0.5 )
	{
		double nZoomFactor = m_nZoomCount*1.1;
		nSourceX = (nSourceX-m_nCutX*m_nScaleFactor)*nZoomFactor;
		nSourceY = (nSourceY-m_nCutY*m_nScaleFactor)*nZoomFactor;
	}

	// if out of canvas return
	if( m_nPicOrigX+nSourceX < 0 || m_nPicOrigY+nSourceY < 0 ||
		m_nPicOrigX+nSourceX >m_nWidth || m_nPicOrigY+nSourceY > m_nHeight ) return( 0 );

	//////////////
	// save current pen
	wxPen currentPen = dc.GetPen();
	// pen used to draw radio
	wxPen penDrawXGamma;

	////////////////////
	// set font/color and shape/line by type
	if( m_pAstroImage->m_vectCatXGamma[nSourceId].type == SKY_OBJECT_TYPE_XRAY ||
		m_pAstroImage->m_vectCatXGamma[nSourceId].type == SKY_OBJECT_TYPE_XGAMMA )
	{
		colDraw = m_pMainConfig->colorXRayObjLabel;
		fontDraw = m_pMainConfig->fontXRayObjLabel;
		nShapeType = m_pMainConfig->m_nXRayObjShapeType;
		nLineType = m_pMainConfig->m_nXRayObjShapeLine;

	} else if( m_pAstroImage->m_vectCatXGamma[nSourceId].type == SKY_OBJECT_TYPE_GAMMA )
	{
		colDraw = m_pMainConfig->colorGammaObjLabel;
		fontDraw = m_pMainConfig->fontGammaObjLabel;
		nShapeType = m_pMainConfig->m_nGammaObjShapeType;
		nLineType = m_pMainConfig->m_nGammaObjShapeLine;

	}

	/////////////////
	// process line type
	if( nLineType == SHAPE_LINE_STYLE_NORMAL )
	{
		bEllipseLineType = 0;
		penDrawXGamma.SetStyle( wxSOLID );

	} else if( nLineType == SHAPE_LINE_STYLE_DOTTED )
	{
		bEllipseLineType = 1;
		penDrawXGamma.SetStyle( wxDOT );
	}

	// set fonts and colors for text
	dc.SetTextForeground( colDraw );
	dc.SetFont( fontDraw );
	penDrawXGamma.SetColour( colDraw );
	// set my pen
	dc.SetPen( penDrawXGamma );

	// if line different then none draw my dso contour/icon
	if( nLineType != SHAPE_LINE_STYLE_NONE )
	{
		if( nShapeType == SHAPE_TYPE_REAL_ELLIPSE )
		{
			// try here to draw the ellipse instead
			double d_ang = (double) 0.0;
			DrawEllips( dc, m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1,
							5, 10,
							d_ang, bEllipseLineType );

		} else if( nShapeType == SHAPE_TYPE_CIRCLE_ICON )
		{
			dc.DrawCircle( m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1, 5 );

		} else if( nShapeType == SHAPE_TYPE_SQUARE_ICON )
		{
			dc.DrawRectangle( (int) round(m_nPicOrigX+nSourceX-3), 
								(int) round(m_nPicOrigY+nSourceY-3), 7, 7 );

		} else if( nShapeType == SHAPE_TYPE_TRIANGLE_ICON )
		{
			wxPoint points[3];
			points[0].x = round( m_nPicOrigX+nSourceX );
			points[0].y = round( m_nPicOrigY+nSourceY );
			points[1].x = points[0].x+8;
			points[1].y = points[0].y+8;
			points[2].x = points[0].x-8;
			points[2].y = points[0].y+8;
			// draw triangle - offset with +3 on y
			dc.DrawPolygon( 3, points, 0, 3 );

		} else if( nShapeType == SHAPE_TYPE_CROSS_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-5, nObjX, nObjY+5 );
			dc.DrawLine( nObjX-5, nObjY, nObjX+5, nObjY );

		} else if( nShapeType == SHAPE_TYPE_TARGET_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-7, nObjX, nObjY+7 );
			dc.DrawLine( nObjX-7, nObjY, nObjX+7, nObjY );
			dc.DrawCircle( nObjX, nObjY, 5 );
		}
	}

	/////////////////////////
	// draw text
	if( m_vectXGammaName[nSourceId] != NULL )
	{
		dc.DrawText( m_vectXGammaName[nSourceId], m_nPicOrigX+nSourceX+1+3+6, m_nPicOrigY+nSourceY+1+3 );
		// here draw name from current vector instead
	}

	dc.SetPen( currentPen );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawSupernovas
// Class:	CAstroImgView
// Purpose:	draw supernovas
// Input:	reference to dc and dso obj id
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawSupernovas( wxDC& dc, int nSourceId )
{
	if( m_pAstroImage->m_nCatSupernova <= 0 || nSourceId < 0 ||
		nSourceId >= m_pAstroImage->m_nCatSupernova ) return( 0 );

	char strLabel[255];
	wxColour colDraw;
	wxFont fontDraw;
	int nShapeType=0;
	int nLineType=0;
	int bEllipseLineType=0;

	//////////////////////////////
	// scale area data from picture size to win size
	double nSourceX = m_pAstroImage->m_vectCatSupernova[nSourceId].x*m_nScaleFactor;
	double nSourceY = m_pAstroImage->m_vectCatSupernova[nSourceId].y*m_nScaleFactor;

	// if zoom
	if( m_nZoomCount > 0.5 )
	{
		double nZoomFactor = m_nZoomCount*1.1;
		nSourceX = (nSourceX-m_nCutX*m_nScaleFactor)*nZoomFactor;
		nSourceY = (nSourceY-m_nCutY*m_nScaleFactor)*nZoomFactor;
	}

	// if out of canvas return
	if( m_nPicOrigX+nSourceX < 0 || m_nPicOrigY+nSourceY < 0 ||
		m_nPicOrigX+nSourceX >m_nWidth || m_nPicOrigY+nSourceY > m_nHeight ) return( 0 );

	//////////////
	// save current pen
	wxPen currentPen = dc.GetPen();
	// pen used to draw radio
	wxPen penDrawSupernova;

	////////////////////
	// set font/color and shape/line by type
	if( m_pAstroImage->m_vectCatSupernova[nSourceId].type == SKY_OBJECT_TYPE_SUPERNOVA )
	{
		colDraw = m_pMainConfig->colorSNObjLabel;
		fontDraw = m_pMainConfig->fontSNObjLabel;
		nShapeType = m_pMainConfig->m_nSNObjShapeType;
		nLineType = m_pMainConfig->m_nSNObjShapeLine;

	}

	/////////////////
	// process line type
	if( nLineType == SHAPE_LINE_STYLE_NORMAL )
	{
		bEllipseLineType = 0;
		penDrawSupernova.SetStyle( wxSOLID );

	} else if( nLineType == SHAPE_LINE_STYLE_DOTTED )
	{
		bEllipseLineType = 1;
		penDrawSupernova.SetStyle( wxDOT );
	}

	// set fonts and colors for text
	dc.SetTextForeground( colDraw );
	dc.SetFont( fontDraw );
	penDrawSupernova.SetColour( colDraw );
	// set my pen
	dc.SetPen( penDrawSupernova );

	// if line different then none draw my source contour/icon
	if( nLineType != SHAPE_LINE_STYLE_NONE )
	{
		if( nShapeType == SHAPE_TYPE_REAL_ELLIPSE )
		{
			// try here to draw the ellipse instead
			double d_ang = (double) 0.0;
			DrawEllips( dc, m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1,
							5, 10,
							d_ang, bEllipseLineType );

		} else if( nShapeType == SHAPE_TYPE_CIRCLE_ICON )
		{
			dc.DrawCircle( m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1, 5 );

		} else if( nShapeType == SHAPE_TYPE_SQUARE_ICON )
		{
			dc.DrawRectangle( (int) round(m_nPicOrigX+nSourceX-3), 
								(int) round(m_nPicOrigY+nSourceY-3), 7, 7 );

		} else if( nShapeType == SHAPE_TYPE_TRIANGLE_ICON )
		{
			wxPoint points[3];
			points[0].x = round( m_nPicOrigX+nSourceX );
			points[0].y = round( m_nPicOrigY+nSourceY );
			points[1].x = points[0].x+8;
			points[1].y = points[0].y+8;
			points[2].x = points[0].x-8;
			points[2].y = points[0].y+8;
			// draw triangle - offset with +3 on y
			dc.DrawPolygon( 3, points, 0, 3 );

		} else if( nShapeType == SHAPE_TYPE_CROSS_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-5, nObjX, nObjY+5 );
			dc.DrawLine( nObjX-5, nObjY, nObjX+5, nObjY );

		} else if( nShapeType == SHAPE_TYPE_TARGET_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-7, nObjX, nObjY+7 );
			dc.DrawLine( nObjX-7, nObjY, nObjX+7, nObjY );
			dc.DrawCircle( nObjX, nObjY, 5 );
		}
	}

	/////////////////////////
	// draw text
	if( m_vectSupernovasName[nSourceId] != NULL )
	{
		dc.DrawText( m_vectSupernovasName[nSourceId], m_nPicOrigX+nSourceX+1+3+6, m_nPicOrigY+nSourceY+1+3 );
		// here draw name from current vector instead
	}

	dc.SetPen( currentPen );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawBlackhole
// Class:	CAstroImgView
// Purpose:	draw blackhole - icon/label
// Input:	reference to dc and dso obj id
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawBlackhole( wxDC& dc, int nSourceId )
{
	if( m_pAstroImage->m_nCatBlackhole <= 0 || nSourceId < 0 ||
		nSourceId >= m_pAstroImage->m_nCatBlackhole ) return( 0 );

	char strLabel[255];
	wxColour colDraw;
	wxFont fontDraw;
	int nShapeType=0;
	int nLineType=0;
	int bEllipseLineType=0;

	//////////////////////////////
	// scale area data from picture size to win size
	double nSourceX = m_pAstroImage->m_vectCatBlackhole[nSourceId].x*m_nScaleFactor;
	double nSourceY = m_pAstroImage->m_vectCatBlackhole[nSourceId].y*m_nScaleFactor;

	// if zoom
	if( m_nZoomCount > 0.5 )
	{
		double nZoomFactor = m_nZoomCount*1.1;
		nSourceX = (nSourceX-m_nCutX*m_nScaleFactor)*nZoomFactor;
		nSourceY = (nSourceY-m_nCutY*m_nScaleFactor)*nZoomFactor;
	}

	// if out of canvas return
	if( m_nPicOrigX+nSourceX < 0 || m_nPicOrigY+nSourceY < 0 ||
		m_nPicOrigX+nSourceX >m_nWidth || m_nPicOrigY+nSourceY > m_nHeight ) return( 0 );

	//////////////
	// save current pen
	wxPen currentPen = dc.GetPen();
	// pen used to draw radio
	wxPen penDraw;

	////////////////////
	// set font/color and shape/line by type
	if( m_pAstroImage->m_vectCatBlackhole[nSourceId].type == SKY_OBJECT_TYPE_BLACKHOLE )
	{
		colDraw = m_pMainConfig->colorBHObjLabel;
		fontDraw = m_pMainConfig->fontBHObjLabel;
		nShapeType = m_pMainConfig->m_nBHObjShapeType;
		nLineType = m_pMainConfig->m_nBHObjShapeLine;

	}

	/////////////////
	// process line type
	if( nLineType == SHAPE_LINE_STYLE_NORMAL )
	{
		bEllipseLineType = 0;
		penDraw.SetStyle( wxSOLID );

	} else if( nLineType == SHAPE_LINE_STYLE_DOTTED )
	{
		bEllipseLineType = 1;
		penDraw.SetStyle( wxDOT );
	}

	// set fonts and colors for text
	dc.SetTextForeground( colDraw );
	dc.SetFont( fontDraw );
	penDraw.SetColour( colDraw );
	// set my pen
	dc.SetPen( penDraw );

	// if line different then none draw my source contour/icon
	if( nLineType != SHAPE_LINE_STYLE_NONE )
	{
		if( nShapeType == SHAPE_TYPE_REAL_ELLIPSE )
		{
			// try here to draw the ellipse instead
			double d_ang = (double) 0.0;
			DrawEllips( dc, m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1,
							5, 10,
							d_ang, bEllipseLineType );

		} else if( nShapeType == SHAPE_TYPE_CIRCLE_ICON )
		{
			dc.DrawCircle( m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1, 5 );

		} else if( nShapeType == SHAPE_TYPE_SQUARE_ICON )
		{
			dc.DrawRectangle( (int) round(m_nPicOrigX+nSourceX-3), 
								(int) round(m_nPicOrigY+nSourceY-3), 7, 7 );

		} else if( nShapeType == SHAPE_TYPE_TRIANGLE_ICON )
		{
			wxPoint points[3];
			points[0].x = round( m_nPicOrigX+nSourceX );
			points[0].y = round( m_nPicOrigY+nSourceY );
			points[1].x = points[0].x+8;
			points[1].y = points[0].y+8;
			points[2].x = points[0].x-8;
			points[2].y = points[0].y+8;
			// draw triangle - offset with +3 on y
			dc.DrawPolygon( 3, points, 0, 3 );

		} else if( nShapeType == SHAPE_TYPE_CROSS_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-5, nObjX, nObjY+5 );
			dc.DrawLine( nObjX-5, nObjY, nObjX+5, nObjY );

		} else if( nShapeType == SHAPE_TYPE_TARGET_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-7, nObjX, nObjY+7 );
			dc.DrawLine( nObjX-7, nObjY, nObjX+7, nObjY );
			dc.DrawCircle( nObjX, nObjY, 5 );
		}
	}

	/////////////////////////
	// draw text
	if( m_vectBlackholesName[nSourceId] != NULL )
	{
		dc.DrawText( m_vectBlackholesName[nSourceId], m_nPicOrigX+nSourceX+1+3+6, m_nPicOrigY+nSourceY+1+3 );
		// here draw name from current vector instead
	}

	dc.SetPen( currentPen );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawMStars
// Class:	CAstroImgView
// Purpose:	draw multiple/double stars - icon/label
// Input:	reference to dc and obj id
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawMStars( wxDC& dc, int nSourceId )
{
	if( m_pAstroImage->m_nCatMStars <= 0 || nSourceId < 0 ||
		nSourceId >= m_pAstroImage->m_nCatMStars ) return( 0 );

	char strLabel[255];
	wxColour colDraw;
	wxFont fontDraw;
	int nShapeType=0;
	int nLineType=0;
	int bEllipseLineType=0;

	//////////////////////////////
	// scale area data from picture size to win size
	double nSourceX = m_pAstroImage->m_vectCatMStars[nSourceId].x*m_nScaleFactor;
	double nSourceY = m_pAstroImage->m_vectCatMStars[nSourceId].y*m_nScaleFactor;

	// if zoom
	if( m_nZoomCount > 0.5 )
	{
		double nZoomFactor = m_nZoomCount*1.1;
		nSourceX = (nSourceX-m_nCutX*m_nScaleFactor)*nZoomFactor;
		nSourceY = (nSourceY-m_nCutY*m_nScaleFactor)*nZoomFactor;
	}

	// if out of canvas return
	if( m_nPicOrigX+nSourceX < 0 || m_nPicOrigY+nSourceY < 0 ||
		m_nPicOrigX+nSourceX >m_nWidth || m_nPicOrigY+nSourceY > m_nHeight ) return( 0 );

	//////////////
	// save current pen
	wxPen currentPen = dc.GetPen();
	// pen used to draw radio
	wxPen penDraw;

	////////////////////
	// set font/color and shape/line by type
	if( m_pAstroImage->m_vectCatMStars[nSourceId].type == SKY_OBJECT_TYPE_MSTARS )
	{
		colDraw = m_pMainConfig->colorMSObjLabel;
		fontDraw = m_pMainConfig->fontMSObjLabel;
		nShapeType = m_pMainConfig->m_nMSObjShapeType;
		nLineType = m_pMainConfig->m_nMSObjShapeLine;

	}

	/////////////////
	// process line type
	if( nLineType == SHAPE_LINE_STYLE_NORMAL )
	{
		bEllipseLineType = 0;
		penDraw.SetStyle( wxSOLID );

	} else if( nLineType == SHAPE_LINE_STYLE_DOTTED )
	{
		bEllipseLineType = 1;
		penDraw.SetStyle( wxDOT );
	}

	// set fonts and colors for text
	dc.SetTextForeground( colDraw );
	dc.SetFont( fontDraw );
	penDraw.SetColour( colDraw );
	// set my pen
	dc.SetPen( penDraw );

	// if line different then none draw my source contour/icon
	if( nLineType != SHAPE_LINE_STYLE_NONE )
	{
		if( nShapeType == SHAPE_TYPE_REAL_ELLIPSE )
		{
			// try here to draw the ellipse instead
			double d_ang = (double) 0.0;
			DrawEllips( dc, m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1,
							5, 10,
							d_ang, bEllipseLineType );

		} else if( nShapeType == SHAPE_TYPE_CIRCLE_ICON )
		{
			dc.DrawCircle( m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1, 5 );

		} else if( nShapeType == SHAPE_TYPE_SQUARE_ICON )
		{
			dc.DrawRectangle( (int) round(m_nPicOrigX+nSourceX-3), 
								(int) round(m_nPicOrigY+nSourceY-3), 7, 7 );

		} else if( nShapeType == SHAPE_TYPE_TRIANGLE_ICON )
		{
			wxPoint points[3];
			points[0].x = round( m_nPicOrigX+nSourceX );
			points[0].y = round( m_nPicOrigY+nSourceY );
			points[1].x = points[0].x+8;
			points[1].y = points[0].y+8;
			points[2].x = points[0].x-8;
			points[2].y = points[0].y+8;
			// draw triangle - offset with +3 on y
			dc.DrawPolygon( 3, points, 0, 3 );

		} else if( nShapeType == SHAPE_TYPE_CROSS_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-5, nObjX, nObjY+5 );
			dc.DrawLine( nObjX-5, nObjY, nObjX+5, nObjY );

		} else if( nShapeType == SHAPE_TYPE_TARGET_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-7, nObjX, nObjY+7 );
			dc.DrawLine( nObjX-7, nObjY, nObjX+7, nObjY );
			dc.DrawCircle( nObjX, nObjY, 5 );
		}
	}

	/////////////////////////
	// draw text
	if( m_vectMStarsName[nSourceId] != NULL )
	{
		dc.DrawText( m_vectMStarsName[nSourceId], m_nPicOrigX+nSourceX+1+3+6, m_nPicOrigY+nSourceY+1+3 );
		// here draw name from current vector instead
	}

	dc.SetPen( currentPen );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawExoplanet
// Class:	CAstroImgView
// Purpose:	draw extra-solar planet - icon/label
// Input:	reference to dc and obj id
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawExoplanet( wxDC& dc, int nSourceId )
{
	if( m_pAstroImage->m_nCatExoplanet <= 0 || nSourceId < 0 ||
		nSourceId >= m_pAstroImage->m_nCatExoplanet ) return( 0 );

	char strLabel[255];
	wxColour colDraw;
	wxFont fontDraw;
	int nShapeType=0;
	int nLineType=0;
	int bEllipseLineType=0;

	//////////////////////////////
	// scale area data from picture size to win size
	double nSourceX = m_pAstroImage->m_vectCatExoplanet[nSourceId].x*m_nScaleFactor;
	double nSourceY = m_pAstroImage->m_vectCatExoplanet[nSourceId].y*m_nScaleFactor;

	// if zoom
	if( m_nZoomCount > 0.5 )
	{
		double nZoomFactor = m_nZoomCount*1.1;
		nSourceX = (nSourceX-m_nCutX*m_nScaleFactor)*nZoomFactor;
		nSourceY = (nSourceY-m_nCutY*m_nScaleFactor)*nZoomFactor;
	}

	// if out of canvas return
	if( m_nPicOrigX+nSourceX < 0 || m_nPicOrigY+nSourceY < 0 ||
		m_nPicOrigX+nSourceX >m_nWidth || m_nPicOrigY+nSourceY > m_nHeight ) return( 0 );

	//////////////
	// save current pen
	wxPen currentPen = dc.GetPen();
	// pen used to draw radio
	wxPen penDraw;

	////////////////////
	// set font/color and shape/line by type
	if( m_pAstroImage->m_vectCatExoplanet[nSourceId].type == SKY_OBJECT_TYPE_EXOPLANET )
	{
		colDraw = m_pMainConfig->colorEXPObjLabel;
		fontDraw = m_pMainConfig->fontEXPObjLabel;
		nShapeType = m_pMainConfig->m_nEXPObjShapeType;
		nLineType = m_pMainConfig->m_nEXPObjShapeLine;

	}

	/////////////////
	// process line type
	if( nLineType == SHAPE_LINE_STYLE_NORMAL )
	{
		bEllipseLineType = 0;
		penDraw.SetStyle( wxSOLID );

	} else if( nLineType == SHAPE_LINE_STYLE_DOTTED )
	{
		bEllipseLineType = 1;
		penDraw.SetStyle( wxDOT );
	}

	// set fonts and colors for text
	dc.SetTextForeground( colDraw );
	dc.SetFont( fontDraw );
	penDraw.SetColour( colDraw );
	// set my pen
	dc.SetPen( penDraw );

	// if line different then none draw my source contour/icon
	if( nLineType != SHAPE_LINE_STYLE_NONE )
	{
		if( nShapeType == SHAPE_TYPE_REAL_ELLIPSE )
		{
			// try here to draw the ellipse instead
			double d_ang = (double) 0.0;
			DrawEllips( dc, m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1,
							5, 10,
							d_ang, bEllipseLineType );

		} else if( nShapeType == SHAPE_TYPE_CIRCLE_ICON )
		{
			dc.DrawCircle( m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1, 5 );

		} else if( nShapeType == SHAPE_TYPE_SQUARE_ICON )
		{
			dc.DrawRectangle( (int) round(m_nPicOrigX+nSourceX-3), 
								(int) round(m_nPicOrigY+nSourceY-3), 7, 7 );

		} else if( nShapeType == SHAPE_TYPE_TRIANGLE_ICON )
		{
			wxPoint points[3];
			points[0].x = round( m_nPicOrigX+nSourceX );
			points[0].y = round( m_nPicOrigY+nSourceY );
			points[1].x = points[0].x+8;
			points[1].y = points[0].y+8;
			points[2].x = points[0].x-8;
			points[2].y = points[0].y+8;
			// draw triangle - offset with +3 on y
			dc.DrawPolygon( 3, points, 0, 3 );

		} else if( nShapeType == SHAPE_TYPE_CROSS_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-5, nObjX, nObjY+5 );
			dc.DrawLine( nObjX-5, nObjY, nObjX+5, nObjY );

		} else if( nShapeType == SHAPE_TYPE_TARGET_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-7, nObjX, nObjY+7 );
			dc.DrawLine( nObjX-7, nObjY, nObjX+7, nObjY );
			dc.DrawCircle( nObjX, nObjY, 5 );
		}
	}

	/////////////////////////
	// draw text
	if( m_vectExoplanetsName[nSourceId] != NULL )
	{
		// get text size
		wxSize sizeTxt = dc.GetTextExtent( m_vectExoplanetsName[nSourceId] );

		dc.DrawText( m_vectExoplanetsName[nSourceId], m_nPicOrigX+nSourceX-1-3-6-sizeTxt.GetWidth(), m_nPicOrigY+nSourceY+1+3 );
		// here draw name from current vector instead
	}

	dc.SetPen( currentPen );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawAsteroid
// Class:	CAstroImgView
// Purpose:	draw easteroid - icon/label and todo latter: trajetory 
// Input:	reference to dc and obj id
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawAsteroid( wxDC& dc, int nSourceId )
{
	if( m_pAstroImage->m_nCatAsteroid <= 0 || nSourceId < 0 ||
		nSourceId >= m_pAstroImage->m_nCatAsteroid ) return( 0 );

	char strLabel[255];
	wxColour colDraw;
	wxFont fontDraw;
	int nShapeType=0;
	int nLineType=0;
	int bEllipseLineType=0;

	//////////////////////////////
	// scale area data from picture size to win size
	double nSourceX = m_pAstroImage->m_vectCatAsteroid[nSourceId].x*m_nScaleFactor;
	double nSourceY = m_pAstroImage->m_vectCatAsteroid[nSourceId].y*m_nScaleFactor;

	// if zoom
	if( m_nZoomCount > 0.5 )
	{
		double nZoomFactor = m_nZoomCount*1.1;
		nSourceX = (nSourceX-m_nCutX*m_nScaleFactor)*nZoomFactor;
		nSourceY = (nSourceY-m_nCutY*m_nScaleFactor)*nZoomFactor;
	}

	// if out of canvas return
	if( m_nPicOrigX+nSourceX < 0 || m_nPicOrigY+nSourceY < 0 ||
		m_nPicOrigX+nSourceX >m_nWidth || m_nPicOrigY+nSourceY > m_nHeight ) return( 0 );

	//////////////
	// save current pen
	wxPen currentPen = dc.GetPen();
	// pen used to draw radio
	wxPen penDraw;

	////////////////////
	// set font/color and shape/line by type
	if( m_pAstroImage->m_vectCatAsteroid[nSourceId].type == SKY_OBJECT_TYPE_ASTEROID )
	{
		colDraw = m_pMainConfig->colorASTObjLabel;
		fontDraw = m_pMainConfig->fontASTObjLabel;
		nShapeType = m_pMainConfig->m_nASTObjShapeType;
		nLineType = m_pMainConfig->m_nASTObjShapeLine;

	}

	/////////////////
	// process line type
	if( nLineType == SHAPE_LINE_STYLE_NORMAL )
	{
		bEllipseLineType = 0;
		penDraw.SetStyle( wxSOLID );

	} else if( nLineType == SHAPE_LINE_STYLE_DOTTED )
	{
		bEllipseLineType = 1;
		penDraw.SetStyle( wxDOT );
	}

	// set fonts and colors for text
	dc.SetTextForeground( colDraw );
	dc.SetFont( fontDraw );
	penDraw.SetColour( colDraw );
	// set my pen
	dc.SetPen( penDraw );

	// if line different then none draw my source contour/icon
	if( nLineType != SHAPE_LINE_STYLE_NONE )
	{
		if( nShapeType == SHAPE_TYPE_REAL_ELLIPSE )
		{
			// try here to draw the ellipse instead
			double d_ang = (double) 0.0;
			DrawEllips( dc, m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1,
							5, 10,
							d_ang, bEllipseLineType );

		} else if( nShapeType == SHAPE_TYPE_CIRCLE_ICON )
		{
			dc.DrawCircle( m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1, 5 );

		} else if( nShapeType == SHAPE_TYPE_SQUARE_ICON )
		{
			dc.DrawRectangle( (int) round(m_nPicOrigX+nSourceX-3), 
								(int) round(m_nPicOrigY+nSourceY-3), 7, 7 );

		} else if( nShapeType == SHAPE_TYPE_TRIANGLE_ICON )
		{
			wxPoint points[3];
			points[0].x = round( m_nPicOrigX+nSourceX );
			points[0].y = round( m_nPicOrigY+nSourceY );
			points[1].x = points[0].x+8;
			points[1].y = points[0].y+8;
			points[2].x = points[0].x-8;
			points[2].y = points[0].y+8;
			// draw triangle - offset with +3 on y
			dc.DrawPolygon( 3, points, 0, 3 );

		} else if( nShapeType == SHAPE_TYPE_CROSS_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-5, nObjX, nObjY+5 );
			dc.DrawLine( nObjX-5, nObjY, nObjX+5, nObjY );

		} else if( nShapeType == SHAPE_TYPE_TARGET_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-7, nObjX, nObjY+7 );
			dc.DrawLine( nObjX-7, nObjY, nObjX+7, nObjY );
			dc.DrawCircle( nObjX, nObjY, 5 );
		}
	}

	/////////////////////////
	// draw text
	if( m_vectAsteroidsName[nSourceId] != NULL )
	{
		// get text size
		//wxSize sizeTxt = dc.GetTextExtent( m_vectAsteroidsName[nSourceId] );

		dc.DrawText( m_vectAsteroidsName[nSourceId], m_nPicOrigX+nSourceX+1+3+6, m_nPicOrigY+nSourceY+1+3 );
		// here draw name from current vector instead
	}

	dc.SetPen( currentPen );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawSolarObjectOrbit
// Class:	CAstroImgView
// Purpose:	draw solar/earth object orbit/trajectory 
// Input:	reference to dc and obj id
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawSolarObjectOrbit( wxDC& dc, int nOrbitType, int nSourceId )
{
//	return( 0 );

	SOLARORBIT* vectOrbit;

	//////////////////////
	// pick orbit by type
	if( nOrbitType == SOLAR_OBJECT_TYPE_ASTEROID )
		vectOrbit = m_vectAsteroidsOrbit;
	else if( nOrbitType == SOLAR_OBJECT_TYPE_COMET )
		vectOrbit = m_vectCometsOrbit;
	else if( nOrbitType == SOLAR_OBJECT_TYPE_AES )
		vectOrbit = m_vectAesOrbit;
	else if( nOrbitType == SOLAR_OBJECT_TYPE_PLANETS )
		vectOrbit = m_vectSolarOrbit;
	else
		return( 0 );

	int nObj = vectOrbit[nSourceId].size();

	if( nObj <= 0 || nSourceId < 0 || nSourceId >= nObj ) return( 0 );

	int i=0;
	int bLastOut=1;
	int nObjX=0, nObjY=0, nLastX=0, nLastY=0;
	char strLabel[255];
	wxColour colDraw;
	
	//////////////
	// save current pen
	wxPen currentPen = dc.GetPen();
	// pen used to draw orbit path
	wxPen penDraw;
	penDraw.SetStyle( wxDOT );

	// color by type ?
//	if( nOrbitType == SOLAR_OBJECT_TYPE_PLANETS )
		penDraw.SetColour( wxColour( 255, 255, 80 ) );

	// set my pen
	dc.SetPen( penDraw );

	// for all points in the orbit
	for( i=0; i<vectOrbit[nSourceId].size(); i++ )
	{
		//////////////////////////////
		// scale area data from picture size to win size
		double nSourceX = vectOrbit[nSourceId][i].x*m_nScaleFactor;
		double nSourceY = vectOrbit[nSourceId][i].y*m_nScaleFactor;

		// if zoom
		if( m_nZoomCount > 0.5 )
		{
			double nZoomFactor = m_nZoomCount*1.1;
			nSourceX = (nSourceX-m_nCutX*m_nScaleFactor)*nZoomFactor;
			nSourceY = (nSourceY-m_nCutY*m_nScaleFactor)*nZoomFactor;
		}

		nObjX = (int) round( m_nPicOrigX+nSourceX );
		nObjY = (int) round( m_nPicOrigY+nSourceY );

		// if at least one of the point is in the image - last or current
		if( nObjX < 0 || nObjY < 0 || nObjX >m_nWidth || nObjY > m_nHeight )
		{
			bLastOut = 1;
			continue;

		} else 
		{
			// draw only if at least the last was out- or initially is default 1
			if( bLastOut == 0 )
			{
				dc.DrawLine( nLastX, nLastY, nObjX, nObjY );		
			}
			// set last found in
			bLastOut = 0;
		}
		// set last point
		nLastX = nObjX;
		nLastY = nObjY;

	} 

	// set back the default pen
	dc.SetPen( currentPen );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawComet
// Class:	CAstroImgView
// Purpose:	draw comet - icon/label 
// Input:	reference to dc and obj id
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawComet( wxDC& dc, int nSourceId )
{
	if( m_pAstroImage->m_nCatComet <= 0 || nSourceId < 0 ||
		nSourceId >= m_pAstroImage->m_nCatComet ) return( 0 );

	char strLabel[255];
	wxColour colDraw;
	wxFont fontDraw;
	int nShapeType=0;
	int nLineType=0;
	int bEllipseLineType=0;

	//////////////////////////////
	// scale area data from picture size to win size
	double nSourceX = m_pAstroImage->m_vectCatComet[nSourceId].x*m_nScaleFactor;
	double nSourceY = m_pAstroImage->m_vectCatComet[nSourceId].y*m_nScaleFactor;

	// if zoom
	if( m_nZoomCount > 0.5 )
	{
		double nZoomFactor = m_nZoomCount*1.1;
		nSourceX = (nSourceX-m_nCutX*m_nScaleFactor)*nZoomFactor;
		nSourceY = (nSourceY-m_nCutY*m_nScaleFactor)*nZoomFactor;
	}

	// if out of canvas return
	if( m_nPicOrigX+nSourceX < 0 || m_nPicOrigY+nSourceY < 0 ||
		m_nPicOrigX+nSourceX >m_nWidth || m_nPicOrigY+nSourceY > m_nHeight ) return( 0 );

	//////////////
	// save current pen
	wxPen currentPen = dc.GetPen();
	// pen used to draw radio
	wxPen penDraw;

	////////////////////
	// set font/color and shape/line by type
	if( m_pAstroImage->m_vectCatComet[nSourceId].type == SKY_OBJECT_TYPE_COMET )
	{
		colDraw = m_pMainConfig->colorCOMObjLabel;
		fontDraw = m_pMainConfig->fontCOMObjLabel;
		nShapeType = m_pMainConfig->m_nCOMObjShapeType;
		nLineType = m_pMainConfig->m_nCOMObjShapeLine;

	}

	/////////////////
	// process line type
	if( nLineType == SHAPE_LINE_STYLE_NORMAL )
	{
		bEllipseLineType = 0;
		penDraw.SetStyle( wxSOLID );

	} else if( nLineType == SHAPE_LINE_STYLE_DOTTED )
	{
		bEllipseLineType = 1;
		penDraw.SetStyle( wxDOT );
	}

	// set fonts and colors for text
	dc.SetTextForeground( colDraw );
	dc.SetFont( fontDraw );
	penDraw.SetColour( colDraw );
	// set my pen
	dc.SetPen( penDraw );

	// if line different then none draw my source contour/icon
	if( nLineType != SHAPE_LINE_STYLE_NONE )
	{
		if( nShapeType == SHAPE_TYPE_REAL_ELLIPSE )
		{
			// try here to draw the ellipse instead
			double d_ang = (double) 0.0;
			DrawEllips( dc, m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1,
							5, 10,
							d_ang, bEllipseLineType );

		} else if( nShapeType == SHAPE_TYPE_CIRCLE_ICON )
		{
			dc.DrawCircle( m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1, 5 );

		} else if( nShapeType == SHAPE_TYPE_SQUARE_ICON )
		{
			dc.DrawRectangle( (int) round(m_nPicOrigX+nSourceX-3), 
								(int) round(m_nPicOrigY+nSourceY-3), 7, 7 );

		} else if( nShapeType == SHAPE_TYPE_TRIANGLE_ICON )
		{
			wxPoint points[3];
			points[0].x = round( m_nPicOrigX+nSourceX );
			points[0].y = round( m_nPicOrigY+nSourceY );
			points[1].x = points[0].x+8;
			points[1].y = points[0].y+8;
			points[2].x = points[0].x-8;
			points[2].y = points[0].y+8;
			// draw triangle - offset with +3 on y
			dc.DrawPolygon( 3, points, 0, 3 );

		} else if( nShapeType == SHAPE_TYPE_CROSS_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-5, nObjX, nObjY+5 );
			dc.DrawLine( nObjX-5, nObjY, nObjX+5, nObjY );

		} else if( nShapeType == SHAPE_TYPE_TARGET_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-7, nObjX, nObjY+7 );
			dc.DrawLine( nObjX-7, nObjY, nObjX+7, nObjY );
			dc.DrawCircle( nObjX, nObjY, 5 );
		}
	}

	/////////////////////////
	// draw text
	if( m_vectCometsName[nSourceId] != NULL )
	{
		// get text size
		//wxSize sizeTxt = dc.GetTextExtent( m_vectCometsName[nSourceId] );

		dc.DrawText( m_vectCometsName[nSourceId], m_nPicOrigX+nSourceX+1+3+6, m_nPicOrigY+nSourceY+1+3 );
		// here draw name from current vector instead
	}

	dc.SetPen( currentPen );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawAes
// Class:	CAstroImgView
// Purpose:	draw satellite - icon/label 
// Input:	reference to dc and obj id
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawAes( wxDC& dc, int nSourceId )
{
	if( m_pAstroImage->m_nCatAes <= 0 || nSourceId < 0 ||
		nSourceId >= m_pAstroImage->m_nCatAes ) return( 0 );

	char strLabel[255];
	wxColour colDraw;
	wxFont fontDraw;
	int nShapeType=0;
	int nLineType=0;
	int bEllipseLineType=0;

	//////////////////////////////
	// scale area data from picture size to win size
	double nSourceX = m_pAstroImage->m_vectCatAes[nSourceId].x*m_nScaleFactor;
	double nSourceY = m_pAstroImage->m_vectCatAes[nSourceId].y*m_nScaleFactor;

	// if zoom
	if( m_nZoomCount > 0.5 )
	{
		double nZoomFactor = m_nZoomCount*1.1;
		nSourceX = (nSourceX-m_nCutX*m_nScaleFactor)*nZoomFactor;
		nSourceY = (nSourceY-m_nCutY*m_nScaleFactor)*nZoomFactor;
	}

	// if out of canvas return
	if( m_nPicOrigX+nSourceX < 0 || m_nPicOrigY+nSourceY < 0 ||
		m_nPicOrigX+nSourceX >m_nWidth || m_nPicOrigY+nSourceY > m_nHeight ) return( 0 );

	//////////////
	// save current pen
	wxPen currentPen = dc.GetPen();
	// pen used to draw radio
	wxPen penDraw;

	////////////////////
	// set font/color and shape/line by type
	if( m_pAstroImage->m_vectCatAes[nSourceId].type == SKY_OBJECT_TYPE_AES )
	{
		colDraw = m_pMainConfig->colorAESObjLabel;
		fontDraw = m_pMainConfig->fontAESObjLabel;
		nShapeType = m_pMainConfig->m_nAESObjShapeType;
		nLineType = m_pMainConfig->m_nAESObjShapeLine;

	}

	/////////////////
	// process line type
	if( nLineType == SHAPE_LINE_STYLE_NORMAL )
	{
		bEllipseLineType = 0;
		penDraw.SetStyle( wxSOLID );

	} else if( nLineType == SHAPE_LINE_STYLE_DOTTED )
	{
		bEllipseLineType = 1;
		penDraw.SetStyle( wxDOT );
	}

	// set fonts and colors for text
	dc.SetTextForeground( colDraw );
	dc.SetFont( fontDraw );
	penDraw.SetColour( colDraw );
	// set my pen
	dc.SetPen( penDraw );

	// if line different then none draw my source contour/icon
	if( nLineType != SHAPE_LINE_STYLE_NONE )
	{
		if( nShapeType == SHAPE_TYPE_REAL_ELLIPSE )
		{
			// try here to draw the ellipse instead
			double d_ang = (double) 0.0;
			DrawEllips( dc, m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1,
							5, 10,
							d_ang, bEllipseLineType );

		} else if( nShapeType == SHAPE_TYPE_CIRCLE_ICON )
		{
			dc.DrawCircle( m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1, 5 );

		} else if( nShapeType == SHAPE_TYPE_SQUARE_ICON )
		{
			dc.DrawRectangle( (int) round(m_nPicOrigX+nSourceX-3), 
								(int) round(m_nPicOrigY+nSourceY-3), 7, 7 );

		} else if( nShapeType == SHAPE_TYPE_TRIANGLE_ICON )
		{
			wxPoint points[3];
			points[0].x = round( m_nPicOrigX+nSourceX );
			points[0].y = round( m_nPicOrigY+nSourceY );
			points[1].x = points[0].x+8;
			points[1].y = points[0].y+8;
			points[2].x = points[0].x-8;
			points[2].y = points[0].y+8;
			// draw triangle - offset with +3 on y
			dc.DrawPolygon( 3, points, 0, 3 );

		} else if( nShapeType == SHAPE_TYPE_CROSS_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-5, nObjX, nObjY+5 );
			dc.DrawLine( nObjX-5, nObjY, nObjX+5, nObjY );

		} else if( nShapeType == SHAPE_TYPE_TARGET_ICON )
		{
			int nObjX = (int) round( m_nPicOrigX+nSourceX );
			int nObjY = (int) round( m_nPicOrigY+nSourceY );
			dc.DrawLine( nObjX, nObjY-7, nObjX, nObjY+7 );
			dc.DrawLine( nObjX-7, nObjY, nObjX+7, nObjY );
			dc.DrawCircle( nObjX, nObjY, 5 );
		}
	}

	/////////////////////////
	// draw text
	if( m_vectAesName[nSourceId] != NULL )
	{
		// get text size
		//wxSize sizeTxt = dc.GetTextExtent( m_vectAesName[nSourceId] );

		dc.DrawText( m_vectAesName[nSourceId], m_nPicOrigX+nSourceX+1+3+6, m_nPicOrigY+nSourceY+1+3 );
		// here draw name from current vector instead
	}

	dc.SetPen( currentPen );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawSolar
// Class:	CAstroImgView
// Purpose:	draw solar/sun/moon - icon/label 
// Input:	reference to dc and obj id
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawSolar( wxDC& dc, int nSourceId )
{
	if( m_pAstroImage->m_nSolarPlanets <= 0 || nSourceId < 0 ||
		nSourceId >= m_pAstroImage->m_nSolarPlanets ) return( 0 );

	char strLabel[255];
	wxColour colDraw;
	wxFont fontDraw;

	//////////////////////////////
	// scale area data from picture size to win size
	double nSourceX = m_pAstroImage->m_vectSolarPlanets[nSourceId].x*m_nScaleFactor;
	double nSourceY = m_pAstroImage->m_vectSolarPlanets[nSourceId].y*m_nScaleFactor;

	// if zoom
	if( m_nZoomCount > 0.5 )
	{
		double nZoomFactor = m_nZoomCount*1.1;
		nSourceX = (nSourceX-m_nCutX*m_nScaleFactor)*nZoomFactor;
		nSourceY = (nSourceY-m_nCutY*m_nScaleFactor)*nZoomFactor;
	}

	// if out of canvas return
	if( m_nPicOrigX+nSourceX < 0 || m_nPicOrigY+nSourceY < 0 ||
		m_nPicOrigX+nSourceX >m_nWidth || m_nPicOrigY+nSourceY > m_nHeight ) return( 0 );

	//////////////
	// save current pen
	wxPen currentPen = dc.GetPen();
	// pen used to draw radio
	wxPen penDraw;

	////////////////////
	// set font/color and shape/line by type
	colDraw = wxColour( 255, 10, 10 );
	fontDraw = m_pMainConfig->fontCOMObjLabel;

	// line type
	penDraw.SetStyle( wxSOLID );

	// set fonts and colors for text
	dc.SetTextForeground( colDraw );
	dc.SetFont( fontDraw );
	penDraw.SetColour( colDraw );
	// set my pen
	dc.SetPen( penDraw );

	// draw object only circle shape - with specific TODO: radius ???
	if( m_pAstroImage->m_vectSolarPlanets[nSourceId].obj_type == 1 )
	{
		dc.DrawCircle( m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1, 5 );

	} else
	{
		dc.DrawCircle( m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1, 8 );
		penDraw.SetColour( wxColour( 255, 50, 50 ) );
		dc.SetPen( penDraw );
		dc.DrawCircle( m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1, 10 );
	}

	// ring 
//			double d_ang = (double) 0.0;
//			DrawEllips( dc, m_nPicOrigX+nSourceX+1, m_nPicOrigY+nSourceY+1,
//							5, 10,
//							d_ang, bEllipseLineType );


	/////////////////////////
	// draw text
	if( m_vectSolarName[nSourceId] != NULL )
	{
		// get text size
		//wxSize sizeTxt = dc.GetTextExtent( m_vectSolarName[nSourceId] );

		dc.DrawText( m_vectSolarName[nSourceId], m_nPicOrigX+nSourceX+1+3+6, m_nPicOrigY+nSourceY+1+3 );
		// here draw name from current vector instead
	}

	dc.SetPen( currentPen );

	return( 1 );
}

/*
////////////////////////////////////////////////////////////////////
// Method:	DrawSvg
int CAstroImgView::DrawSvg( wxDC& dc )
{
	// Get raw access to the wxWidgets bitmap -- this locks the pixels and 
	// unlocks on destruction.
	PixelData data( m_pBitmap );

	// This cast looks like it is ignoring byte-ordering, but the 
	// pixel format already explicitly handles that.
    wxAlphaPixelFormat::ChannelType* pd = (wxAlphaPixelFormat::ChannelType*) &data.GetPixels().Data();

    // Upshot: if the stride is negative, rewind the pointer from the end of 
    // the buffer to the beginning. 
    const int stride = data.GetRowStride();
    if (stride < 0) 
        pd += (data.GetHeight() - 1) * stride;

	// render buffer
	agg::rendering_buffer rBuf;
	// Scanline rasterizer
	agg::rasterizer_scanline_aa<> rasterizer;
	// Scanline container
	agg::scanline_p8  scanline; 

	rBuf.attach(pd, data.GetWidth(), data.GetHeight(), stride);

	///////////////////////
	// here I draw my svg
	agg::svg::path_renderer* pPath = new agg::svg::path_renderer();
	double m_min_x;
	double m_min_y;
	double m_max_x;
	double m_max_y;

	agg::svg::parser p(*pPath);
	p.parse("ursa-major.svg");
//	pPath->arrange_orientations();
//	pPath->bounding_rect(&m_min_x, &m_min_y, &m_max_x, &m_max_y);

	typedef agg::pixfmt_bgra32 pixfmt;
	typedef agg::renderer_base<pixfmt> renderer_base;
	typedef agg::renderer_scanline_aa_solid<renderer_base> renderer_solid;

	//pixfmt pixf(rBuf);
	PixelFormat::AGGType pixf(rBuf);

	/// The AGG base renderer
	typedef agg::renderer_base<PixelFormat::AGGType> RendererBase;
	//renderer_base rb(pixf);
	RendererBase rb(pixf);

	/// The AGG solid renderer
	typedef agg::renderer_scanline_aa_solid<RendererBase> RendererSolid;
	//renderer_solid ren(rb);
	RendererSolid ren( rb );

	//rb.clear(agg::rgba(1,1,1));

	//agg::rasterizer_scanline_aa<> ras;
	//agg::scanline_p8 sl;
	agg::trans_affine mtx;

//        ras.gamma(agg::gamma_power(m_gamma.value()));
//      mtx *= agg::trans_affine_translation((m_min_x + m_max_x) * -0.5, (m_min_y + m_max_y) * -0.5);
//        mtx *= agg::trans_affine_scaling(m_scale.value());
//        mtx *= agg::trans_affine_rotation(agg::deg2rad(m_rotate.value()));
//        mtx *= agg::trans_affine_translation((m_min_x + m_max_x) * 0.5 + m_x, (m_min_y + m_max_y) * 0.5 + m_y + 30);

		//for( int i=0; i<100; i+=10 )
		//{
	mtx *= agg::trans_affine_translation(-128, -128);
	mtx *= agg::trans_affine_rotation(agg::deg2rad(45.0));
	mtx *= agg::trans_affine_translation(256, 256);
	mtx *= agg::trans_affine_scaling( 1.5, 1.5 );

//		mtx = agg::trans_affine_translation(-128, -128);
//		mtx *= agg::trans_affine_rotation(agg::deg2rad(45.0));
//		mtx *= agg::trans_affine_translation(128, 128);

		

	//        m_path.expand(m_expand.value());
	//        start_timer();
	pPath->render( rasterizer, scanline, ren, mtx, rb.clip_box(), 1.0 );

	delete( pPath );

	return( 0 );
}
*/

////////////////////////////////////////////////////////////////////
// Method:	DrawConstellationsLabel
// Class:	CAstroImgView
// Purpose:	draw constelations
// Input:	reference to dc
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawConstellationsLabel( wxDC& dc )
{
	if( m_nConstellations <= 0 || m_vectConstellations == NULL ) return( 0 );
	int i = 0;

	wxFont constlFont( 15, wxSWISS, wxNORMAL, wxBOLD );
	dc.SetFont( constlFont );
	wxColor labelCol( 255, 100, 0 );
	wxSize sizeTxt;

//	dc.SetLogicalFunction( wxOR );
	for( i=0; i<m_nConstellations; i++ )
	{
		/////////////////////////////////////
		// label
		// :: calculate center relativ instead ??
		double c_x = m_vectConstellations[i].center_x*m_nScaleFactor;
		double c_y = m_vectConstellations[i].center_y*m_nScaleFactor;
		if( m_nZoomCount > 0.5 )
		{
			c_x = (int) round( (c_x-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
			c_y = (int) round( (c_y-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
		}

		// draw label
		dc.SetTextForeground( labelCol );
		if( m_dc )
		{
			SetGdcPropsFromDc(dc);
			double t_w=0, t_h=0, t_d=0, t_l=0;
			m_dc->GetTextExtent( m_vectConstellations[i].name, &t_w, &t_h, &t_d, &t_l );
			m_dc->DrawText( m_vectConstellations[i].name, m_nPicOrigX+c_x-t_w/2, 
												m_nPicOrigY+c_y-t_h/2 );

		} else
		{
			sizeTxt = dc.GetTextExtent( m_vectConstellations[i].name );
			//dc.SetBrush( *wxCYAN_BRUSH );
			dc.DrawText( m_vectConstellations[i].name, m_nPicOrigX+c_x-sizeTxt.GetWidth()/2, 
															m_nPicOrigY+c_y-sizeTxt.GetHeight()/2 );
		}
	}
//	dc.SetLogicalFunction( wxCOPY );

	return( 0 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawConstellations
// Class:	CAstroImgView
// Purpose:	draw constelations
// Input:	reference to dc
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawConstellations( wxDC& dc )
{
	if( m_nConstellations <= 0 || m_vectConstellations == NULL ) return( 0 );
	// and now return
	if( m_pMainConfig->m_bConstellationLines == 0 ) return( 0 );

	int i = 0, j = 0;
	wxGraphicsPath gpath;

	//////////////
	// save current pen
	wxPen currentPen = dc.GetPen();

	// set pen used to draw constellations
	int nLineStyle = m_pMainConfig->m_nGraphicsConstellationLinesLine;
	wxPen penDrawConst;
	// set color
	penDrawConst.SetColour( m_pMainConfig->colGraphicsConstellationLines );
	// set line stype
	if( nLineStyle == GRAPHICS_LINE_STYLE_NORMAL )
		penDrawConst.SetStyle( wxSOLID );
	else if( nLineStyle == GRAPHICS_LINE_STYLE_DOTTED )
		penDrawConst.SetStyle( wxDOT );
	else if( nLineStyle == GRAPHICS_LINE_STYLE_LDOTTED )
		penDrawConst.SetStyle( wxSHORT_DASH );
	// set dc pen
	dc.SetPen( penDrawConst );

	if( SetGdcPropsFromDc(dc) ) 
	{
		m_dc->Clip( m_nPicOrigX, m_nPicOrigY, m_nPicWidth, m_nPicHeight );
		gpath = m_dc->CreatePath();
	}

	dc.SetClippingRegion( m_nPicOrigX, m_nPicOrigY, m_nPicWidth, m_nPicHeight );

	for( i=0; i<m_nConstellations; i++ )
	{
		////////////////////////////////////////////////
		// test :: draw constellation art-work **************
/*		if( m_vectArtworkFile[i].image != NULL )
		{
			int nId = m_pAstroImage->m_vectConstellation[i].id;
			long x1 = m_vectArtworkFile[i].x1;
			long y1 = m_vectArtworkFile[i].y1;
			long x2 = m_pStarCatalog->m_vectConstellations[nId].x2;
			long y2 = m_pStarCatalog->m_vectConstellations[nId].y2;

			////////////////////////////////
			//  DRAW ARTWORK
			///////////////////////////////
			// get local
			double sx1 =  m_vectArtworkFile[i].sx1;
			double sy1 =  m_vectArtworkFile[i].sy1;
			double sx2 =  m_vectArtworkFile[i].sx2;
			double sy2 =  m_vectArtworkFile[i].sy2;
			double r2p_ratio = m_vectArtworkFile[i].r2p_ratio;
			double con_ang_rad = m_vectArtworkFile[i].con_ang_rad;

			wxImage img = m_vectArtworkFile[i].image->Copy();
			int w = m_vectArtworkFile[i].width;//img.GetWidth();
			int h = m_vectArtworkFile[i].height;//img.GetHeight();
			// resize image
			double nw = m_vectArtworkFile[i].w2pic;//(double) w*r2p_ratio;
			double nh = m_vectArtworkFile[i].h2pic;//(double) h*r2p_ratio;


			////////////////
			// calc my pos
			double xa = (double) sx1-x1*r2p_ratio; 
			double ya = (double) sy1-y1*r2p_ratio; 

			// calc scale to image view
			double wa = (double) nw*m_nScaleFactor;
			double ha = (double) nh*m_nScaleFactor;

			xa = xa*m_nScaleFactor;
			ya = ya*m_nScaleFactor;

			if( m_nZoomCount > 0.5 )
			{
				xa = (int) round( (xa-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
				ya = (int) round( (ya-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );

				wa = (int) round( wa*m_nLastZoomFactor );
				ha = (int) round( ha*m_nLastZoomFactor );

			}
		
			// set back here - maybe i should calculate shift scale ?
			w = (int) wa;
			h = (int) ha;

			img.Rescale( w, h );
			// set mask
			img.SetMask( true );
			img.SetMaskColour( 0, 0, 0 );

			wxBitmap bmp_img( img );

			int nTotalW = img.GetWidth();
			int nTotalH = img.GetHeight();

			wxMemoryDC memDC;
			//wxBitmap bitmap( nTotalW, nTotalH );
			memDC.SelectObject( bmp_img );
			dc.Blit( m_nPicOrigX+(int)xa, m_nPicOrigY+(int)ya, nTotalW, nTotalH, &memDC, 0, 0, wxOR, true);
				
			img.Destroy();
		}
*/		// ************************* end draw here - might wana put it back when do svg?


		////////////////////////////////////////////
		// draw constelations lines
		for( j=0; j<m_vectConstellations[i].nLine; j++ )
		{
			////////////////////////////////
			// first we calculate position - size etc
			double x1 = m_vectConstellations[i].vectLine[j].x1;
			double y1 = m_vectConstellations[i].vectLine[j].y1;
			double x2 = m_vectConstellations[i].vectLine[j].x2;
			double y2 = m_vectConstellations[i].vectLine[j].y2;

			// scale area data from picture size to win size
			double nStarX1 = x1*m_nScaleFactor;
			double nStarY1 = y1*m_nScaleFactor;
			double nStarX2 = x2*m_nScaleFactor;
			double nStarY2 = y2*m_nScaleFactor;

			if( m_nZoomCount > 0.5 )
			{
				nStarX1 = (int) round( (nStarX1-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
				nStarY1 = (int) round( (nStarY1-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
				nStarX2 = (int) round( (nStarX2-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
				nStarY2 = (int) round( (nStarY2-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );

			}

			if( m_dc )
			{
				gpath.MoveToPoint( m_nPicOrigX+nStarX1+1, m_nPicOrigY+nStarY1+1 );
				gpath.AddLineToPoint( m_nPicOrigX+nStarX2+1, m_nPicOrigY+nStarY2+1 );

			} else
			{
			
				dc.DrawLine( m_nPicOrigX+nStarX1+1, m_nPicOrigY+nStarY1+1, 
								m_nPicOrigX+nStarX2+1, m_nPicOrigY+nStarY2+1 );

			}
				/*
				int ox1=0, oy1=0, ox2=0, oy2=0;
				if( IsLineIntersectRect( m_nPicOrigX+nStarX1+1, m_nPicOrigY+nStarY1+1, 
											m_nPicOrigX+nStarX2+1, m_nPicOrigY+nStarY2+1,
											m_nPicOrigX, m_nPicOrigY, m_nPicWidth, m_nPicHeight, 
											ox1, oy1, ox2, oy2 ) )
				{
					//dc.DrawLine( ox1, oy1, ox2, oy2 );

					dc.DrawLine( m_nPicOrigX+nStarX1+1, m_nPicOrigY+nStarY1+1, 
								m_nPicOrigX+nStarX2+1, m_nPicOrigY+nStarY2+1 );

				} else
				{
					dc.DrawLine( ox1, oy1, ox2, oy2 );

					//dc.DrawLine( m_nPicOrigX+nStarX1+1, m_nPicOrigY+nStarY1+1, 
					//			m_nPicOrigX+nStarX2+1, m_nPicOrigY+nStarY2+1 );
				}*/

			//}
		}

/****************** MY CRAPY TEST _ TO CLEAN ONCE STABLE
		////////////////////////////////////////////
		// NEW METHOD :: draw constelations lines from
		StarDef star;
		int nLineType=0;
		int nId = m_pAstroImage->m_vectConstellation[i].id; // GetConstellationNoById( );
		
		int nLines = m_pStarCatalog->m_vectConstellations[nId].nLineCount[nLineType];
		ConstellationLine* vectLines = m_pStarCatalog->m_vectConstellations[nId].vectLine[nLineType];
		// check if this is not the use the other or skip
		if( !vectLines ) continue;

		for( j=0; j<nLines; j++ )
		{
			////////////////////////
			// deproject point A
			star.ra = vectLines[j].ra1;
			star.dec = vectLines[j].dec1;

			// project star using the found orig
			m_pStarCatalog->ProjectStar( star, m_pAstroImage->m_nOrigRa, m_pAstroImage->m_nOrigDec );
			// larry :: my damn multiplication factor
			star.x = star.x*m_pAstroImage->m_nSkyProjectionScale; //10;
			star.y = star.y*m_pAstroImage->m_nSkyProjectionScale; //10;
			// and now apply inverse trans
			m_pStarCatalog->m_pStarMatch->ApplyObjInv( star );

			double x1 = star.x;
			double y1 = star.y;

			////////////////////////
			// deproject point B
			star.ra = vectLines[j].ra2;
			star.dec = vectLines[j].dec2;

			// project star using the found orig
			m_pStarCatalog->ProjectStar( star, m_pAstroImage->m_nOrigRa, m_pAstroImage->m_nOrigDec );
			// larry :: my damn multiplication factor
			star.x = star.x*m_pAstroImage->m_nSkyProjectionScale; //10;
			star.y = star.y*m_pAstroImage->m_nSkyProjectionScale; //10;
			// and now apply inverse trans
			m_pStarCatalog->m_pStarMatch->ApplyObjInv( star );

			double x2 = star.x;
			double y2 = star.y;


			// scale area data from picture size to win size
			double nStarX1 = x1*m_nScaleFactor;
			double nStarY1 = y1*m_nScaleFactor;
			double nStarX2 = x2*m_nScaleFactor;
			double nStarY2 = y2*m_nScaleFactor;

			if( m_nZoomCount > 0.5 )
			{
				nStarX1 = (int) round( (nStarX1-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
				nStarY1 = (int) round( (nStarY1-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
				nStarX2 = (int) round( (nStarX2-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
				nStarY2 = (int) round( (nStarY2-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );

			}

			dc.DrawLine( m_nPicOrigX+nStarX1+1, m_nPicOrigY+nStarY1+1, 
						m_nPicOrigX+nStarX2+1, m_nPicOrigY+nStarY2+1 );
		}
*/

	}

	if( m_dc )
	{
		m_dc->DrawPath( gpath );
		m_dc->ResetClip();
	}

	dc.SetPen( currentPen );

	dc.DestroyClippingRegion();

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawDynamicObjectPath
// Class:	CAstroImgView
// Purpose:	draw dynamic object path
// Input:	reference to dc
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawDynamicObjectPaths( wxDC& dc )
{
	int nPaths = m_pAstroImage->m_pImageGroup->m_nDynamicObjPath;
	if( nPaths <= 0 ) return( 0 );
	int i=0,j=0;

	int nPathShapeType = m_pMainConfig->m_nObjsPathShapeType;
	int nPathLineStyle = m_pMainConfig->m_nGraphicsObjectPathLine;
	wxPen penDrawPath;
	wxPen trackLinePen;
	// set line stype
	if( nPathLineStyle == GRAPHICS_LINE_STYLE_NORMAL )
	{
		penDrawPath.SetStyle( wxSOLID );
		trackLinePen.SetStyle( wxSOLID );

	} else if( nPathLineStyle == GRAPHICS_LINE_STYLE_DOTTED )
	{
		penDrawPath.SetStyle( wxDOT );
		trackLinePen.SetStyle( wxDOT );

	} else if( nPathLineStyle == GRAPHICS_LINE_STYLE_LDOTTED )
	{
		penDrawPath.SetStyle( wxSHORT_DASH );
		trackLinePen.SetStyle( wxSHORT_DASH );
	}
	// set big path colour
	penDrawPath.SetColour( m_pMainConfig->colGraphicsObjectPath );
	// set 2 points path - small path colour
	wxColour colTrackLine;
	unsigned char nRed = m_pMainConfig->colGraphicsObjectPath.Red();
	unsigned char nGreen = m_pMainConfig->colGraphicsObjectPath.Green();
	unsigned char nBlue = m_pMainConfig->colGraphicsObjectPath.Blue();
	unsigned char nLessCol = 50;
	// down garde nuance for objects not like a star class
	nRed =  ( nRed-nLessCol > 0 ) ? nRed -= 125 : nRed = nLessCol;
	nGreen = ( nGreen-nLessCol > 0 ) ? nGreen -= nLessCol : nGreen = nLessCol;
	nBlue = ( nBlue-nLessCol > 0 ) ? nBlue -= nLessCol : nBlue = nLessCol;
	// set color nuance
	colTrackLine.Set( nRed, nGreen, nBlue );
	trackLinePen.SetColour( colTrackLine );

	for( i=0; i<nPaths; i++ )
	{
		int nPoints = m_pAstroImage->m_pImageGroup->m_vectDynamicObjPath[i].vectNode.size();
		double nScore = m_pAstroImage->m_pImageGroup->m_vectDynamicObjPath[i].path_score;
		if( nPoints <= 1 ) continue;
		//if( nPoints <= 2 ) continue;

		// do only the min path
//		if( i != m_pAstroImage->m_pImageGroup->m_nMinPathId ) continue;

		wxPoint vectPoints[200];
		// get points
		for( j=0; j<nPoints; j++ )
		{
			double nStarX = m_pAstroImage->m_pImageGroup->m_vectDynamicObjPath[i].vectNode[j].x*m_nScaleFactor;
			double nStarY = m_pAstroImage->m_pImageGroup->m_vectDynamicObjPath[i].vectNode[j].y*m_nScaleFactor;
			
			// check if zoom
			if( m_nZoomCount > 0.5 )
			{
				vectPoints[j].x = (int) round( m_nPicOrigX + (nStarX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
				vectPoints[j].y = (int) round( m_nPicOrigY + (nStarY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
				//dc.DrawCircle( vectPoints[j].x, vectPoints[j].y, 8 );
			} else
			{
				vectPoints[j].x = (int) round( m_nPicOrigX+nStarX );
				vectPoints[j].y = (int) round( m_nPicOrigY+nStarY );
				//dc.DrawCircle( vectPoints[j].x, vectPoints[j].y, 5 );
			}

			// draw vertices
			if( m_pMainConfig->m_bGraphicsPathVertex ) 
				dc.DrawCircle( vectPoints[j].x, vectPoints[j].y, 8 );

		}
		// draw a spline or line for a path with only two points
		if( nPoints == 2 )
		{
			
			dc.SetPen( trackLinePen );
			dc.DrawLine( vectPoints[0].x, vectPoints[0].y, vectPoints[1].x, vectPoints[1].y );
			// mark ends
			if( m_pMainConfig->m_bGraphicsPathStartEnd )
			{
				dc.DrawCircle( vectPoints[0].x, vectPoints[0].y, 6 );
				dc.DrawCircle( vectPoints[1].x, vectPoints[1].y, 6 );
			}

		} else
		{
			// set path drawing pen
			dc.SetPen( penDrawPath );
			
			// draw math if multipe shapes
			if( nPathShapeType == DYNAMIC_OBJ_PATH_SHAPE_TYPE_LINE )
				for( j=1; j<nPoints; j++ ) dc.DrawLine( vectPoints[j-1].x, vectPoints[j-1].y, vectPoints[j].x, vectPoints[j].y );
			else if( nPathShapeType == DYNAMIC_OBJ_PATH_SHAPE_TYPE_SPLINE_DEFAULT )
				dc.DrawSpline( nPoints, vectPoints );
			else if( nPathShapeType == DYNAMIC_OBJ_PATH_SHAPE_TYPE_SPLINE_CATMULLROM )
				DrawPath( dc, nPoints, vectPoints, nPathLineStyle );

			// mark better ends
			if( m_pMainConfig->m_bGraphicsPathStartEnd )
			{
				dc.DrawCircle( vectPoints[0].x, vectPoints[0].y, 6 );
				dc.DrawCircle( vectPoints[nPoints-1].x, vectPoints[nPoints-1].y, 6 );
			}
			
			// also draw base line if selected - and line type not line...
			if( m_pMainConfig->m_bGraphicsPathBaseLines && nPathShapeType != DYNAMIC_OBJ_PATH_SHAPE_TYPE_LINE )
				for( j=1; j<nPoints; j++ ) dc.DrawLine( vectPoints[j-1].x, vectPoints[j-1].y, vectPoints[j].x, vectPoints[j].y );
		}
	}
	
	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawTarget
// Class:	CAstroImgView
// Purpose:	draw a target
// Input:	reference to dc, target id
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::DrawTarget( wxDC& dc, int nTargetId )
{
	if( nTargetId < 0 || nTargetId >= m_nTarget ) return( 0 );

	char strName[255];

	////////////////////////////////
	// first we calculate position - size etc
	double x = m_vectTarget[nTargetId].x;
	double y = m_vectTarget[nTargetId].y;
	double a = m_vectTarget[nTargetId].a;
	double b = m_vectTarget[nTargetId].b;
	double kronfactor = m_vectTarget[nTargetId].kronfactor;

	// scale area data from picture size to win size
	double nTargetX = x*m_nScaleFactor;
	double nTargetY = y*m_nScaleFactor;
	// ellipse factors
	double nTargetA = a*kronfactor*m_nScaleFactor;
	double nTargetB = b*kronfactor*m_nScaleFactor;

	if( m_nZoomCount > 0.5 )
	{
		nTargetX = (int) round( (nTargetX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
		nTargetY = (int) round( (nTargetY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );

		// adjust ellipse zoom factors 
		nTargetA = nTargetA*m_nLastZoomFactor;
		nTargetB = nTargetB*m_nLastZoomFactor;
	}
	
	// if out of canvas return
	if( m_nPicOrigX+nTargetX < 0 || m_nPicOrigY+nTargetY < 0 ||
		m_nPicOrigX+nTargetX >m_nWidth || m_nPicOrigY+nTargetY > m_nHeight ) return( 0 );

	///////////////////////////////////////////////
	//  save current pen
	wxPen currentPen = dc.GetPen();

	// set pen used to draw dso
	wxPen penDrawCatObj;
	
	wxColour myCol;
	myCol.Set( 255, 0, 255 );
	dc.SetTextForeground( myCol );
	penDrawCatObj.SetColour( myCol );
	// now set my pen
	dc.SetPen( penDrawCatObj );
	dc.SetBrush( *wxTRANSPARENT_BRUSH );

	//////////////////////////
	// draw my target - two circles first
	double nCenterX = m_nPicOrigX+nTargetX+1;
	double nCenterY = m_nPicOrigY+nTargetY+1;
	int nTargetRadiusA = nTargetA+7;
	int nTargetRadiusB = nTargetA+10;
	dc.DrawCircle( nCenterX, nCenterY, nTargetRadiusA );
	dc.DrawCircle( nCenterX, nCenterY, nTargetRadiusB );
	// then draw four radial line 
	dc.DrawLine( nCenterX, nCenterY+nTargetRadiusA-3, nCenterX, nCenterY+nTargetRadiusB+3 );
	dc.DrawLine( nCenterX, nCenterY-nTargetRadiusA+3, nCenterX, nCenterY-nTargetRadiusB-3 );
	dc.DrawLine( nCenterX+nTargetRadiusA-3, nCenterY, nCenterX+nTargetRadiusB+3, nCenterY );
	dc.DrawLine( nCenterX-nTargetRadiusA+3, nCenterY, nCenterX-nTargetRadiusB-3, nCenterY );

	////////////////////
	// try here to draw the ellipse instead
//	DrawEllips( dc, m_nPicOrigX+nStarX+1, 
//					m_nPicOrigY+nStarY+1, 
//					nStarA, nStarB,
//					m_pAstroImage->m_vectStar[nStarId].theta, 0 );

	dc.SetPen( currentPen );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawDistanceTool
// Class:	CAstroImgView
// Purpose:	draw distance tool
// Input:	reference to dc
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::DrawDistanceTool( wxDC& dc )
{
	wxPoint nPointA, nPointB;
	wxGraphicsPath gpath;

	// scale area data from picture size to win size
	double nPointFloatA_X = m_pDistTool->m_nDistanceToolA_X * m_nScaleFactor;
	double nPointFloatA_Y = m_pDistTool->m_nDistanceToolA_Y * m_nScaleFactor;
	double nPointFloatB_X = m_pDistTool->m_nDistanceToolB_X * m_nScaleFactor;
	double nPointFloatB_Y = m_pDistTool->m_nDistanceToolB_Y * m_nScaleFactor;

	// claculate zoom /scale
	if( m_nZoomCount > 0.5 )
	{
		nPointA.x = m_nPicOrigX + (int) round( (nPointFloatA_X-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
		nPointA.y = m_nPicOrigY + (int) round( (nPointFloatA_Y-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
		nPointB.x = m_nPicOrigX + (int) round( (nPointFloatB_X-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
		nPointB.y = m_nPicOrigY + (int) round( (nPointFloatB_Y-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );

	} else
	{
		nPointA.x = m_nPicOrigX + (int) round( nPointFloatA_X );
		nPointA.y = m_nPicOrigY + (int) round( nPointFloatA_Y );
		nPointB.x = m_nPicOrigX + (int) round( nPointFloatB_X );
		nPointB.y = m_nPicOrigY + (int) round( nPointFloatB_Y );
	}

	//////////////
	// save current pen
	wxPen currentPen = dc.GetPen();
	// set pen used to draw tool
	wxPen penDrawTool;

	wxColour myCol;
	myCol.Set( 0, 255, 255 );
	penDrawTool.SetColour( myCol );

	dc.SetPen( penDrawTool );
	// brush
	wxBrush currentBrush = dc.GetBrush();
	dc.SetBrush( myCol );

	if( SetGdcPropsFromDc(dc) ) gpath = m_dc->CreatePath();

	// draw line
	if( m_dc )
	{
		gpath.MoveToPoint( nPointA.x, nPointA.y );
		gpath.AddLineToPoint( nPointB.x , nPointB.y );

	} else
	{
		dc.DrawLine( nPointA, nPointB );
	}

	wxPoint vectArrow[3];
	vectArrow[0] = nPointB;
	
	double R = 8; 
	double ang = atan2( (double) (nPointB.y-nPointA.y) , (double) (nPointB.x-nPointA.x) );
	double arrow_ang = PI/8;

	// point 1
	double my_ang = PI/2-ang-arrow_ang;
	double x_t_1 = R*sin( my_ang );
	double y_t_1 = R*cos( my_ang );
	vectArrow[1] = wxPoint( nPointB.x - x_t_1, nPointB.y - y_t_1 );

	// point 2
	my_ang = ang-arrow_ang;
	double y_t_2 = R*sin( my_ang );
	double x_t_2 = R*cos( my_ang );
	vectArrow[2] = wxPoint( nPointB.x - x_t_2, nPointB.y - y_t_2 );

	// draw  arrow B
	dc.DrawPolygon( 3, vectArrow );

	////////////////
	// arrow B
	vectArrow[0] = nPointA;
	// point 1
	vectArrow[1] = wxPoint( nPointA.x + x_t_1, nPointA.y + y_t_1 );
	// point 2
	vectArrow[2] = wxPoint( nPointA.x + x_t_2, nPointA.y + y_t_2 );
	// draw  arrow A
	dc.DrawPolygon( 3, vectArrow );

	if( m_dc ) m_dc->DrawPath( gpath );

	// restore brush/pen
	dc.SetPen( currentPen );
	dc.SetBrush( currentBrush );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetImage
// Class:	CAstroImgView
// Purose:	CImgView drawing
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetImage( CAstroImage* pAstroImage )
{
	if( !pAstroImage ) return;

	int i =0;
	m_nStar = 0;

	// reset all to null - start here
	for( i=0; i<m_nConstellations; i++ ) m_vectArtworkFile[i].image = NULL;
	// check if already allocated
	if( m_vectConstellations )
	{
		for( i=0; i<m_nConstellations; i++ ) free( m_vectConstellations[i].vectLine );
		m_nConstellations = 0;
		free( m_vectConstellations );
		m_vectConstellations = NULL;
	}

	// get remote image node
	m_pAstroImage = pAstroImage;

	// load image from remote image node
	wxImage* pImage = pAstroImage->GetImage( );
	// check if load failed
	if( pImage == NULL || !pImage ) return;

	SetWxImage( pImage );

	// release node image
	pAstroImage->CloseImage( );
	// set flag to clean
	m_bImageAltered = 0;

	// why this crap here ???
//	pAstroImage = NULL;

	// :: set constellations here
//	if( m_pMainConfig->m_bShowConstellations ) SetCostellations( m_pAstroImage );

	//////
	// set distance tool
	if( m_pDistTool ) m_pDistTool->SetAstroImage( m_pAstroImage );

	/////
	// set status frame zoom
	m_pFrame->SetStatusZoom( (int) (m_nScaleFactor*100.0) );

	Refresh( FALSE );
	Update( );
}

////////////////////////////////////////////////////////////////////
// Method:	SetCostellations
// Class:	CAstroImgView
// Purose:	set constelations , projection, artwork etc ... 
//			just calc/load for now here
// Input:	pointer to astro image
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetCostellations( CAstroImage* pAstroImage )
{
	if( !pAstroImage ) return;
	if( !m_pMainConfig->m_bShowConstellations ) return;
	if( m_pAstroImage->m_nConstellation <=0 )
	{
		if( m_vectConstellations ) FreeCostellations( ); 

		Refresh( FALSE );
		Update( );

		return;
	}

	int i=0, j=0;
	int bFound = 0;
	StarDef star;

	int nCulture = m_pMainConfig->m_nConstellationCulture;
	int nLineType = m_pMainConfig->m_nConstellationVersion;

	m_pConstellations = m_pSky->m_pConstellations;

	// check if already allocated
	if( m_vectConstellations ) FreeCostellations( );


//	{
//		for( i=0; i<m_nConstellations; i++ ) free( m_vectConstellations[i].vectLine );
//		m_nConstellations = 0;
//		free( m_vectConstellations );
//		m_vectConstellations = NULL;
//	}

	// check here if not the default found constelations is to use
	int vectConstl[300];
	int nConstl=0;
	// if culture/version western/iau - which is the default image detect - get constl vect from image
	if( nCulture == SKY_CULTURE_WESTERN )
	{
		for( i=0; i<m_pAstroImage->m_nConstellation; i++ )
		{
			vectConstl[nConstl] = m_pAstroImage->m_vectConstellation[i].id;
			nConstl++;
		}
		// now check if constl object is default loaded and load if not
		if( m_pConstellations->m_nCulture != SKY_CULTURE_WESTERN )
		{
			m_pConstellations->LoadConstellations( SKY_CULTURE_WESTERN );
		}

	} else
	{
		// if not default find if not already loaded ?
		m_pConstellations->GetConstlInImage( m_pAstroImage, nCulture, nLineType, vectConstl, nConstl );
	}

	//////////////////////////////////////
	// first we extract and calculate constelations
	m_nConstellations = nConstl;

	// allocate local structure
	m_vectConstellations = (ImgConstellationStructure*) malloc( m_nConstellations*sizeof(ImgConstellationStructure) );

	for( i=0; i<m_nConstellations; i++ )
	{
		// just blind init - to delete latter
		m_vectArtworkFile[i].image = NULL;

		// set min/max
		double nMinX=DBL_MAX, nMinY=DBL_MAX, nMaxX=DBL_MIN, nMaxY=DBL_MIN;

		m_vectConstellations[i].nLine = 0;
		m_vectConstellations[i].vectLine = NULL;

		int nId = vectConstl[i]; // GetConstellationNoById( vectConstl[i] ); ???
		m_vectConstellations[i].id = nId;

		// copy name
		m_vectConstellations[i].name = (wxChar*) malloc( (wxStrlen(m_pConstellations->m_vectConstellations[nId].name)+1)*sizeof(wxChar));
		memset( m_vectConstellations[i].name, 0, (wxStrlen(m_pConstellations->m_vectConstellations[nId].name)+1)*sizeof(wxChar) );
		wxStrcpy( m_vectConstellations[i].name, m_pConstellations->m_vectConstellations[nId].name );

		int nLines = m_pConstellations->m_vectConstellations[nId].nLineCount[nLineType];
		ConstellationLine* vectLines = m_pConstellations->m_vectConstellations[nId].vectLine[nLineType];
		// check if this is not the use the other or skip
		if( !vectLines ) continue;

		// allocate local lines 
		m_vectConstellations[i].nLine = nLines;
		m_vectConstellations[i].vectLine = (ImgConstellationLine*) malloc( nLines*sizeof(ImgConstellationLine) );

		// hack :: vector 
		int vectHackX[100];
		int vectHackY[100];
		int nHack=0;
		m_vectConstellations[i].center_x = 0;
		m_vectConstellations[i].center_y = 0;

		for( j=0; j<nLines; j++ )
		{
			long x1, y1, x2, y2;

			////////////////////////
			// deproject point A
			star.ra = vectLines[j].ra1;
			star.dec = vectLines[j].dec1;

			// project star using the found orig
			m_pSky->ProjectStar( star, m_pAstroImage->m_nOrigRa, m_pAstroImage->m_nOrigDec );
			// larry :: my damn multiplication factor
			star.x = star.x*m_pAstroImage->m_nSkyProjectionScale;
			star.y = star.y*m_pAstroImage->m_nSkyProjectionScale; 
			// and now apply inverse trans
			m_pSky->TransObj_SkyToImage( star );

			m_vectConstellations[i].vectLine[j].x1 = star.x;
			m_vectConstellations[i].vectLine[j].y1 = star.y;
			x1 = star.x;
			y1 = star.y;

			// get min/max x/y
			if( star.x >= 0 && star.x < m_nOrigPicWidth && star.y >= 0 && star.y < m_nOrigPicHeight )
			{
				if( star.x < nMinX ) nMinX = star.x;
				if( star.y < nMinY ) nMinY = star.y;
				if( star.x > nMaxX ) nMaxX = star.x;
				if( star.y > nMaxY ) nMaxY = star.y;

				// hack
				vectHackX[nHack] = (int) round( star.x );
				vectHackY[nHack] = (int) round( star.y );
				nHack++;

				m_vectConstellations[i].center_x += (int) round( star.x );
				m_vectConstellations[i].center_y += (int) round( star.y );

			}

			////////////////////////
			// deproject point B
			star.ra = vectLines[j].ra2;
			star.dec = vectLines[j].dec2;

			// project star using the found orig
			m_pSky->ProjectStar( star, m_pAstroImage->m_nOrigRa, m_pAstroImage->m_nOrigDec );
			// larry :: my damn multiplication factor
			star.x = star.x*m_pAstroImage->m_nSkyProjectionScale; //10;
			star.y = star.y*m_pAstroImage->m_nSkyProjectionScale; //10;
			// and now apply inverse trans
			m_pSky->TransObj_SkyToImage( star );

			m_vectConstellations[i].vectLine[j].x2 = star.x;
			m_vectConstellations[i].vectLine[j].y2 = star.y;
			x2 = star.x;
			y2 = star.y;

			/////////////////////////
			// check if both point are in the image
			if( ( x1 >= 0 && x1 < m_nOrigPicWidth && y1 >= 0 && y1 < m_nOrigPicHeight ) &&
				( x2 >= 0 && x2 < m_nOrigPicWidth && y2 >= 0 && y2 < m_nOrigPicHeight ) )
			{
				m_vectConstellations[i].center_x = x1+(x2-x1)/2;
				m_vectConstellations[i].center_y = y1+(y2-y1)/2;

			} else if( x1 >= 0 && x1 < m_nOrigPicWidth && y1 >= 0 && y1 < m_nOrigPicHeight )
			{
				m_vectConstellations[i].center_x = x1;
				m_vectConstellations[i].center_y = y1;

			} else if ( x2 >= 0 && x2 < m_nOrigPicWidth && y2 >= 0 && y2 < m_nOrigPicHeight )
			{
				m_vectConstellations[i].center_x = x2;
				m_vectConstellations[i].center_y = y2;
			}

/******************* previous method
			// get min/max x/y
			if( star.x >= 0 && star.x < m_nOrigPicWidth && star.y >= 0 && star.y < m_nOrigPicHeight )
			{
				if( star.x < nMinX ) nMinX = star.x;
				if( star.y < nMinY ) nMinY = star.y;
				if( star.x > nMaxX ) nMaxX = star.x;
				if( star.y > nMaxY ) nMaxY = star.y;
				
				// hack
				vectHackX[nHack] = (int) round( star.x );
				vectHackY[nHack] = (int) round( star.y );
				nHack++;

				m_vectConstellations[i].center_x += (int) round( star.x );
				m_vectConstellations[i].center_y += (int) round( star.y );

			}
*/
		}

/******************* previous method
		// set min/max
		m_vectConstellations[i].min_x = (int) round( nMinX );
		m_vectConstellations[i].min_y = (int) round( nMinY );
		m_vectConstellations[i].max_x = (int) round( nMaxX );
		m_vectConstellations[i].max_y = (int) round( nMaxY );

		// todo: i got a bug here with division by zero ... see why ?
		if( nHack != 0 )
		{
			m_vectConstellations[i].center_x = m_vectConstellations[i].center_x/nHack;
			m_vectConstellations[i].center_y = m_vectConstellations[i].center_y/nHack;
		}
*/

/*		/////////////////////////
		// find center x/y to write label
		// :: calculate relative center to image borders
		if( nMinX < 0 ) nMinX = 0.0;
		if( nMinY < 0 ) nMinY = 0.0;
		if( nMaxX > m_nOrigPicWidth ) nMaxX = m_nOrigPicWidth;
		if( nMaxY > m_nOrigPicHeight ) nMaxY = m_nOrigPicHeight;
		// get center
		int ncx = (int) round( nMinX+(nMaxX-nMinX)/2.0 );
		int ncy = (int) round( nMinY+(nMaxY-nMinY)/2.0 );
		// get the closest
		double nDistMin = DBL_MAX;
		int nCenterFound=0;
		for( j=0; j<nHack; j++ )
		{
			// calc dist to center
			double nDist = (double) CalcPlanarDistance( (double) ncx, (double) ncy, (double) vectHackX[j], (double) vectHackY[j] );
			if( nDist < nDistMin )
			{
				nCenterFound = j;
				nDistMin = nDist;
			}
		}
		// set center
		m_vectConstellations[i].center_x = vectHackX[nCenterFound];
		m_vectConstellations[i].center_y = vectHackY[nCenterFound];
		//m_vectConstellations[i].center_x = (int) round( nMinX+(nMaxX-nMinX)/2.0 );
		//m_vectConstellations[i].center_y = (int) round( nMinY+(nMaxY-nMinY)/2.0 );
*/
	}

	//////////
	// clean here artwork if any ?? -- remove blited image - reinit image
	if( m_bImageAltered )
	{	
		m_bImageAltered = 0;
		// load image from remote image node
		wxImage* pImage = pAstroImage->GetImage( );
		// check if load failed
		if( pImage == NULL || !pImage ) return;
		// set image
		SetWxImage( pImage );

		// release node image
		pAstroImage->CloseImage( );
	}

	/////////
	// check if art work
	if( m_pMainConfig->m_bConstellationArtwork == 0 || !m_pConstellations->m_bHasArtwork )
	{
		Refresh( FALSE );
		Update( );
		return;
	}

	//////////////////////////////////////////////////
	// CONSTELATIPONS ARTWORK - for already local
	wxMemoryDC memDCDest;
	wxBitmap orig_bmp( *m_pImage );
	memDCDest.SelectObject( orig_bmp );

	wxString strFile;
	for( i=0; i<m_nConstellations; i++ )
	{
		// get the id - set from catalog to image --- this could change ... maybe 
		// not a good idea to use
		int nId = m_vectConstellations[i].id;
		if( m_pConstellations->m_vectConstellations[nId].artwork_file != NULL && 
			wxStrlen( m_pConstellations->m_vectConstellations[nId].artwork_file ) > 0 )
		{
			bFound = 1;
			// load artwork image
			m_vectArtworkFile[i].image = NULL;
			if( m_pConstellations->m_nCulture == SKY_CULTURE_WESTERN )
				strFile.Printf( wxT("%s/%s"), unMakeAppPath(wxT("data/constellations/western")), m_pConstellations->m_vectConstellations[nId].artwork_file );
			else if( m_pConstellations->m_nCulture == SKY_CULTURE_INUIT )
				strFile.Printf( wxT("%s/%s"), unMakeAppPath(wxT("data/constellations/inuit")), m_pConstellations->m_vectConstellations[nId].artwork_file );

			if( !wxFileExists( strFile ) )
				continue;

			///////////////////////////////////////
			// process here localy
			int x1 = m_pConstellations->m_vectConstellations[nId].x1;
			int y1 = m_pConstellations->m_vectConstellations[nId].y1;
			int x2 = m_pConstellations->m_vectConstellations[nId].x2;
			int y2 = m_pConstellations->m_vectConstellations[nId].y2;

			// now :: inv project ra1/dec1 to image
			star.ra = m_pConstellations->m_vectConstellations[nId].ra1;
			star.dec = m_pConstellations->m_vectConstellations[nId].dec1;

			// project star using the found orig
			m_pSky->ProjectStar( star, m_pAstroImage->m_nOrigRa, m_pAstroImage->m_nOrigDec );
			// larry :: my damn multiplication factor
			star.x = star.x*m_pAstroImage->m_nSkyProjectionScale; //10;
			star.y = star.y*m_pAstroImage->m_nSkyProjectionScale; //10;
			// and now apply inverse trans
			m_pSky->TransObj_SkyToImage( star );

			double sx1 = star.x;
			double sy1 = star.y;
			m_vectArtworkFile[i].sx1 = sx1;
			m_vectArtworkFile[i].sy1 = sy1;
			
			//////////
			// now calc to deproject ra/dec2
			star.ra = m_pConstellations->m_vectConstellations[nId].ra2;
			star.dec = m_pConstellations->m_vectConstellations[nId].dec2;

			// project star using the found orig
			m_pSky->ProjectStar( star, m_pAstroImage->m_nOrigRa, m_pAstroImage->m_nOrigDec );
			// larry :: my damn multiplication factor
			star.x = star.x*m_pAstroImage->m_nSkyProjectionScale; //10;
			star.y = star.y*m_pAstroImage->m_nSkyProjectionScale; //10;
			// and now apply inverse trans
			m_pSky->TransObj_SkyToImage( star );

			double sx2 =  star.x;
			double sy2 =  star.y;
			m_vectArtworkFile[i].sx2 = sx2;
			m_vectArtworkFile[i].sy2 = sy2;

			// calc pic/real size
			double real_size = sqrt( sqr(sx2-sx1) + sqr(sy2-sy1) );
			double pic_size = sqrt( sqr(x2-x1) + sqr(y2-y1) );
			double r2p_ratio = (double) real_size/pic_size;

			m_vectArtworkFile[i].r2p_ratio = r2p_ratio;

			/////////////////////////
			// calc rotation
			double dx1 = (double) (x2-x1);
			double dy1 = (double) (y2-y1);
			double dx2 = (double) (sx2-sx1);
			double dy2 = (double) (sy2-sy1);
			double magcos = dx1 * dx2 + dy1 * dy2;
			double magsin = dx1 * dy2 - dx2 * dy1;

			double con_ang_rad = 2.0*PI-atan2( magsin, magcos ); 
			double con_ang_rad_n = atan2( magsin, magcos );
			if( con_ang_rad < 0.0 ) con_ang_rad = con_ang_rad + 2.0*PI;
			m_vectArtworkFile[i].con_ang_rad = con_ang_rad;

			////////////////////////////////////////////
			// NEW ROT
			wxImage img( strFile, wxBITMAP_TYPE_ANY );
			if( !img.IsOk() )
				continue;

			int w_orig = img.GetWidth();
			int h_orig = img.GetHeight();
			// get center
			int x_c = (int) round( w_orig/2 );
			int y_c = (int) round( h_orig/2 );
			// get xr/yr
			int x_r = x_c-x1;
			int y_r = y_c-y1;
			// get dist from my point to center
			double r = sqrt( sqr(x_r) + sqr(y_r) );
			// get alpha angle for my x1y1_to_center line - or gamma ?
			double ang_rad_x1y1_to_center = atan2( (double)y_r, (double)x_r );
			// get my new angle after rotation 180-(rot+alpha) or gamma-rot ?
			double ang_rad_rot = con_ang_rad_n+ang_rad_x1y1_to_center; //PI-(con_ang_rad+ang_rad_x1y1_to_center);
			// get new x/y in pic
			double rx1 = (double) r*cos(ang_rad_rot);
			double ry1 = (double) r*sin(ang_rad_rot);
			int rot_x1 = x_c-(int) round( rx1 );
			int rot_y1 = y_c-(int) round( ry1 );

			// rotate image around my center
			wxPoint srot;
			wxImage t_img = img.Rotate( con_ang_rad, wxPoint( x_c, y_c ), true, &srot );
			// adjust new x/y with rot size shift
			rot_x1 = rot_x1+(-1*srot.x);
			rot_y1 = rot_y1+(-1*srot.y);
			// and update my coords for x1,y1
			m_vectArtworkFile[i].x1 = rot_x1;
			m_vectArtworkFile[i].y1 = rot_y1;

// out for now ... maybe replace latter with SVG
//			m_vectArtworkFile[i].image = new wxImage( t_img ); //new wxImage( strFile, wxBITMAP_TYPE_ANY );

			m_vectArtworkFile[i].width = t_img.GetWidth();
			m_vectArtworkFile[i].height = t_img.GetHeight();

			// set const artwork size with ratio
			m_vectArtworkFile[i].w2pic = (double) m_vectArtworkFile[i].width*r2p_ratio;
			m_vectArtworkFile[i].h2pic = (double) m_vectArtworkFile[i].height*r2p_ratio;

			/////////////////////////////////////
			// METHOD :: DRAW ON THE IMAGE

			double xa = (double) m_vectArtworkFile[i].sx1-m_vectArtworkFile[i].x1*m_vectArtworkFile[i].r2p_ratio; 
			double ya = (double) m_vectArtworkFile[i].sy1-m_vectArtworkFile[i].y1*m_vectArtworkFile[i].r2p_ratio; 

		
			// set back here - maybe i should calculate shift scale ?
			int w = (int) m_vectArtworkFile[i].w2pic;
			int h = (int) m_vectArtworkFile[i].h2pic;

			t_img.Rescale( w, h );
			// set mask
			t_img.SetMask( true );
			t_img.SetMaskColour( 0, 0, 0 );

			int nTotalW = t_img.GetWidth();
			int nTotalH = t_img.GetHeight();


			wxMemoryDC memDC;
			wxBitmap bitmap( nTotalW, nTotalH );
			memDC.SelectObject( wxBitmap( t_img ) );

			memDCDest.Blit( (int)xa, (int)ya, nTotalW, nTotalH, &memDC, 0, 0, wxOR, true);			
		}
	}
	
	// if constellation found then recreate the image
	if( bFound )
	{
		// keep copy of the orig here ??? or mark to reload if free ?
		m_bImageAltered = 1;

		m_pImage->Destroy( );
		m_pImage = new wxImage( orig_bmp.ConvertToImage() );
		UpdateImage( );
	}

	Refresh( FALSE );
	Update( );
}

////////////////////////////////////////////////////////////////////
// Method:	SetStars
// Class:	CAstroImgView
// Purose:	CImgView drawing
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetStars( CAstroImage* pAstroImage, int nLimit = 60 )
{
	if( !pAstroImage ) return;

	int i=0, j=0, nShown=0;
	wxString strName = wxT("");

	// now first we free stars allocated
	FreeDetectedStars( );

	if( nLimit == 0 )
	{
		Refresh( FALSE );
		Update( );

		return;
	}

	// and just set the star/dso no
	m_nStar = pAstroImage->m_nStar;

	// set m_nStar with limit ?
	if( nLimit < m_nStar && nLimit >= 0 )
		m_nStarDisplayLimit = nLimit;
	else if( nLimit == -2 )
		m_nStarDisplayLimit = 0;
	else
		m_nStarDisplayLimit = m_nStar;

	// now allocate my vectors
	//m_vectStarName = (char**) calloc( m_nStar+1, sizeof(char*) );
//	m_vectStarName.Alloc( m_nStar+1 );
	// allocate labels props
	m_vectStarLabels = (structObjLabel*) calloc( m_nStar+1, sizeof(structObjLabel) );
	m_nStarLabels = m_nStar;

	////////////////////////////////////
	// set names for star
	for( i=0; i<m_nStar; i++ )
	{
		// set what to be shown
		// (m_pAstroImage->m_vectStar[i].state & OBJECT_STATE_CUSTOM_SHOW)  ||
//		if( nShown < m_nStarDisplayLimit  && 
//			( m_pMainConfig->m_bViewDetectedObjectsNoMatch || m_pAstroImage->m_vectStar[i].match ) )
//		{
//			pAstroImage->m_vectStar[i].state = OBJECT_STATE_CUSTOM_SHOW;
//			nShown++;
//
//		} else
//			pAstroImage->m_vectStar[i].state = 0;

		// no try to set name
		if( m_pSky->m_pCatalogStarNames->GetStarName( i, pAstroImage->m_vectStar[i], strName ) )
		{
			// now allocate memory for the name and set it
			//int nStrSize = strlen(strName);
			//m_vectStarName[i] = (char*) malloc( strlen(strName)+1 );
			m_vectStarName.Add( strName );
		}

		////////////////////////////////////
		// set arange labels
		memset( &(m_vectStarLabels[i]), 0, sizeof(structObjLabel) );
		m_vectStarLabels[i].n_pos = 1;
	
	}

	Refresh( FALSE );
	Update( );

}

////////////////////////////////////////////////////////////////////
// Method:	SetCatStars
// Class:	CAstroImgView
// Purose:	CImgView drawing
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetCatStars( CAstroImage* pAstroImage, int nLimit = 60 )
{
	if( !pAstroImage ) return;

	int i = 0;
	wxString strName = wxT("");

	// now first we free stars allocated
	FreeCatStars( );

	if( nLimit == 0 )
	{
		Refresh( FALSE );
		Update( );

		return;
	}

	// and just set the star/dso no
	m_nCatStar = pAstroImage->m_nCatStar;

	// set m_nStar with limit ?
	if( nLimit < pAstroImage->m_nCatStar && nLimit >= 0 )
		m_nCatStarDisplayLimit = nLimit;
	else if( nLimit == -2 )
		m_nCatStarDisplayLimit = 0;
	else
		m_nCatStarDisplayLimit = pAstroImage->m_nCatStar;

	// now allocate my vectors
	//m_vectCatStarName = (char**) calloc( m_nCatStar+1, sizeof(char*) );
//	m_vectCatStarName.Alloc( m_nCatStar+1 );
	// allocate labels props
	m_vectCatStarLabels = (structObjLabel*) calloc( m_nCatStar+1, sizeof(structObjLabel) );
	m_nCatStarLabels = m_nCatStar;

	////////////////////////////////////
	// set names for catalog star
	for( i=0; i<m_nCatStar; i++ )
	{
		// no try to set name
		if( m_pSky->m_pCatalogStarNames->GetCatStarName( i, pAstroImage->m_vectCatStar[i], strName ) )
		{
			// now allocate memory for the name and set it
			//int nStrSize = strName.Length();
			//m_vectCatStarName[i] = (char*) malloc( strlen(strName)+1 );
			m_vectCatStarName.Add( strName );
		}

		////////////////////////////////////
		// set arange labels
		memset( &(m_vectCatStarLabels[i]), 0, sizeof(structObjLabel) );
		m_vectCatStarLabels[i].n_pos = 1;
	}

	Refresh( FALSE );
	Update( );
}

////////////////////////////////////////////////////////////////////
// Method:	SetDsoObjects
// Class:	CAstroImgView
// Purose:	set dso objects with display limit
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetDsoObjects( CAstroImage* pAstroImage, int nLimit = 60 )
{
	if( !pAstroImage ) return;

	int i = 0;
	wxString strName;

	// now first we free stars allocated
	FreeDsoObjects( );

	if( nLimit == 0 )
	{
		Refresh( FALSE );
		Update( );

		return;
	}

	// and just set the star/dso no
	m_nDso = pAstroImage->m_nCatDso;

	// set m_nStar with limit ?
	if( nLimit < pAstroImage->m_nCatDso && nLimit >= 0 )
		m_nCatDsoDisplayLimit = nLimit;
	else if( nLimit == -2 )
		m_nCatDsoDisplayLimit = 0;
	else
		m_nCatDsoDisplayLimit = pAstroImage->m_nCatDso;

	// now allocate my vectors
	m_vectDsoName = (wxChar**) calloc( m_nDso+1, sizeof(wxChar*) );
	// allocate labels props
	m_vectDsoLabels = (structObjLabel*) calloc( m_nDso+1, sizeof(structObjLabel) );
	m_nDsoLabels = m_nDso;

	////////////////////////////////////
	// set names for DSO
	for( i=0; i<m_nDso; i++ )
	{
		// no try to set name
		if( m_pSky->m_pCatalogDsoNames->GetDosCatName( pAstroImage->m_vectCatDso[i], strName ) )
		{
			// now allocate memory for the name and set it
			//int nStrSize = strlen(strName);
			m_vectDsoName[i] = (wxChar*) malloc( (strName.Length()+1)*sizeof(wxChar) );
			wxStrcpy( m_vectDsoName[i], strName );
		}

		////////////////////////////////////
		// set arange labels
		memset( &(m_vectDsoLabels[i]), 0, sizeof(structObjLabel) );
		m_vectDsoLabels[i].n_pos = 1;
	}

	Refresh( FALSE );
	Update( );

}

////////////////////////////////////////////////////////////////////
// Method:	SetRadioSources
// Class:	CAstroImgView
// Purose:	set radio srouces with display limit
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetRadioSources( CAstroImage* pAstroImage, int nLimit = 60 )
{
	if( !pAstroImage ) return;

	int i = 0;
	wxString strName;

	// now first we free stars allocated
	FreeRadioSources( );

	if( nLimit == 0 )
	{
		Refresh( FALSE );
		Update( );

		return;
	}

	// and just set the source no
	m_nRadio = pAstroImage->m_nCatRadio;

	// set m_nStar with limit ?
	if( nLimit < pAstroImage->m_nCatRadio && nLimit >= 0 )
		m_nCatRadioDisplayLimit = nLimit;
	else if( nLimit == -2 )
		m_nCatRadioDisplayLimit = 0;
	else
		m_nCatRadioDisplayLimit = pAstroImage->m_nCatRadio;

	// now allocate my vectors
	m_vectRadioName = (wxChar**) calloc( m_nRadio+1, sizeof(wxChar*) );

	////////////////////////////////////
	// set names for RADIO SOURCES
	for( i=0; i<m_nRadio; i++ )
	{
		// no try to set name
		if( m_pSky->m_pCatalogRadio->GetName( pAstroImage->m_vectCatRadio[i], strName ) )
		{
			// now allocate memory for the name and set it
			//int nStrSize = strlen(strName);
			m_vectRadioName[i] = (wxChar*) malloc( (strName.Length()+1)*sizeof(wxChar) );
			wxStrcpy( m_vectRadioName[i], strName );
		}
	}

	Refresh( FALSE );
	Update( );

}

////////////////////////////////////////////////////////////////////
// Method:	SetXGammaSources
// Class:	CAstroImgView
// Purose:	set x-ray/gamma sources with display limit
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetXGammaSources( CAstroImage* pAstroImage, int nLimit = 60 )
{
	if( !pAstroImage ) return;

	int i = 0;
	wxString strName;

	// now first we free stars allocated
	FreeXGammaSources( );

	if( nLimit == 0 )
	{
		Refresh( FALSE );
		Update( );

		return;
	}

	// and just set the source no
	m_nXGamma = pAstroImage->m_nCatXGamma;

	// set m_nStar with limit ?
	if( nLimit < pAstroImage->m_nCatXGamma && nLimit >= 0 )
		m_nCatXGammaDisplayLimit = nLimit;
	else if( nLimit == -2 )
		m_nCatXGammaDisplayLimit = 0;
	else
		m_nCatXGammaDisplayLimit = pAstroImage->m_nCatXGamma;

	// now allocate my vectors
	m_vectXGammaName = (wxChar**) calloc( m_nXGamma+1, sizeof(wxChar*) );

	////////////////////////////////////
	// set names for XGamma SOURCES
	for( i=0; i<m_nXGamma; i++ )
	{
		// no try to set name
		if( m_pSky->m_pCatalogXGamma->GetName( pAstroImage->m_vectCatXGamma[i], strName ) )
		{
			// now allocate memory for the name and set it
			//int nStrSize = strlen(strName);
			m_vectXGammaName[i] = (wxChar*) malloc( (strName.Length()+1)*sizeof(wxChar) );
			wxStrcpy( m_vectXGammaName[i], strName );
		}
	}

	Refresh( FALSE );
	Update( );

}

////////////////////////////////////////////////////////////////////
// Method:	SetSupernovas
// Class:	CAstroImgView
// Purose:	set supernovas with display limit
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetSupernovas( CAstroImage* pAstroImage, int nLimit = 60 )
{
	if( !pAstroImage ) return;

	int i = 0;
	wxString strName;

	// now first we free supernovas allocated
	FreeSupernovas( );

	if( nLimit == 0 )
	{
		Refresh( FALSE );
		Update( );

		return;
	}

	// and just set the source no
	m_nSupernovas = pAstroImage->m_nCatSupernova;

	// set m_nStar with limit ?
	if( nLimit < pAstroImage->m_nCatSupernova && nLimit >= 0 )
		m_nCatSupernovasDisplayLimit = nLimit;
	else if( nLimit == -2 )
		m_nCatSupernovasDisplayLimit = 0;
	else
		m_nCatSupernovasDisplayLimit = pAstroImage->m_nCatSupernova;

	// now allocate my vectors
	m_vectSupernovasName = (wxChar**) calloc( m_nSupernovas+1, sizeof(wxChar*) );

	////////////////////////////////////
	// set names for Supernovas 
	for( i=0; i<m_nSupernovas; i++ )
	{
		// no try to set name
		if( m_pSky->m_pCatalogSupernovas->GetName( pAstroImage->m_vectCatSupernova[i], strName ) )
		{
			// now allocate memory for the name and set it
			//int nStrSize = strlen(strName);
			m_vectSupernovasName[i] = (wxChar*) malloc( (strName.Length()+1)*sizeof(wxChar) );
			wxStrcpy( m_vectSupernovasName[i], strName );
		}
	}

	Refresh( FALSE );
	Update( );

}

////////////////////////////////////////////////////////////////////
// Method:	SetBlackholes
// Class:	CAstroImgView
// Purose:	set blackholes with display limit
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetBlackholes( CAstroImage* pAstroImage, int nLimit = 60 )
{
	if( !pAstroImage ) return;

	int i = 0;
	wxString strName;

	// now first we free blackholes allocated
	FreeBlackholes( );

	if( nLimit == 0 )
	{
		Refresh( FALSE );
		Update( );

		return;
	}

	// and just set the source no
	m_nBlackholes = pAstroImage->m_nCatBlackhole;

	// set m_nStar with limit ?
	if( nLimit < pAstroImage->m_nCatBlackhole && nLimit >= 0 )
		m_nCatBlackholesDisplayLimit = nLimit;
	else if( nLimit == -2 )
		m_nCatBlackholesDisplayLimit = 0;
	else
		m_nCatBlackholesDisplayLimit = pAstroImage->m_nCatBlackhole;

	// now allocate my vectors
	m_vectBlackholesName = (wxChar**) calloc( m_nBlackholes+1, sizeof(wxChar*) );

	////////////////////////////////////
	// set names for Blackholes 
	for( i=0; i<m_nBlackholes; i++ )
	{
		// no try to set name
		if( m_pSky->m_pCatalogBlackholes->GetName( pAstroImage->m_vectCatBlackhole[i], strName ) )
		{
			// now allocate memory for the name and set it
			//int nStrSize = strlen(strName);
			m_vectBlackholesName[i] = (wxChar*) malloc( (strName.Length()+1)*sizeof(wxChar) );
			wxStrcpy( m_vectBlackholesName[i], strName );
		}
	}

	Refresh( FALSE );
	Update( );

}

////////////////////////////////////////////////////////////////////
// Method:	SetMStars
// Class:	CAstroImgView
// Purose:	set multiple/double stars with display limit
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetMStars( CAstroImage* pAstroImage, int nLimit = 60 )
{
	if( !pAstroImage ) return;

	int i = 0;
	wxString strName;

	// now first we free blackholes allocated
	FreeMStars( );

	if( nLimit == 0 )
	{
		Refresh( FALSE );
		Update( );

		return;
	}

	// and just set the source no
	m_nMStars = pAstroImage->m_nCatMStars;

	// set m_nStar with limit ?
	if( nLimit < pAstroImage->m_nCatMStars && nLimit >= 0 )
		m_nCatMStarsDisplayLimit = nLimit;
	else if( nLimit == -2 )
		m_nCatMStarsDisplayLimit = 0;
	else
		m_nCatMStarsDisplayLimit = pAstroImage->m_nCatMStars;

	// now allocate my vectors
	m_vectMStarsName = (wxChar**) calloc( m_nMStars+1, sizeof(wxChar*) );

	////////////////////////////////////
	// set names for multiple/double stars 
	for( i=0; i<m_nMStars; i++ )
	{
		// no try to set name
		if( m_pSky->m_pCatalogMStars->GetName( pAstroImage->m_vectCatMStars[i], strName ) )
		{
			// now allocate memory for the name and set it
			//int nStrSize = strlen(strName);
			m_vectMStarsName[i] = (wxChar*) malloc( (strName.Length()+1)*sizeof(wxChar) );
			wxStrcpy( m_vectMStarsName[i], strName );
		}
	}

	Refresh( FALSE );
	Update( );

}

////////////////////////////////////////////////////////////////////
// Method:	SetExoplanets
// Class:	CAstroImgView
// Purose:	set extra-solar planets names with display limit
// Input:	pointer to astroimage and disaply limit
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetExoplanets( CAstroImage* pAstroImage, int nLimit = 60 )
{
	if( !pAstroImage ) return;

	int i = 0;
	wxString strName;

	// now first we free prevoius allocated - if any
	FreeExoplanets( );

	if( nLimit == 0 )
	{
		Refresh( FALSE );
		Update( );

		return;
	}

	// and just set the source no
	m_nExoplanets = pAstroImage->m_nCatExoplanet;

	// set m_nStar with limit ?
	if( nLimit < pAstroImage->m_nCatExoplanet && nLimit >= 0 )
		m_nCatExoplanetsDisplayLimit = nLimit;
	else if( nLimit == -2 )
		m_nCatExoplanetsDisplayLimit = 0;
	else
		m_nCatExoplanetsDisplayLimit = pAstroImage->m_nCatExoplanet;

	// now allocate my vectors
	m_vectExoplanetsName = (wxChar**) calloc( m_nExoplanets+1, sizeof(wxChar*) );

	////////////////////////////////////
	// set names for extra-solar planets 
	for( i=0; i<m_nExoplanets; i++ )
	{
		// no try to set name
		if( m_pSky->m_pCatalogExoplanets->GetName( pAstroImage->m_vectCatExoplanet[i], strName ) )
		{
			// now allocate memory for the name and set it
			//int nStrSize = strlen(strName);
			m_vectExoplanetsName[i] = (wxChar*) malloc( (strName.Length()+1)*sizeof(wxChar) );
			wxStrcpy( m_vectExoplanetsName[i], strName );
		}
	}

	Refresh( FALSE );
	Update( );

}

////////////////////////////////////////////////////////////////////
// Method:	SetAsteroids
// Class:	CAstroImgView
// Purose:	set asteroids names with display limit
// Input:	pointer to astroimage and disaply limit
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetAsteroids( CAstroImage* pAstroImage, int nLimit = 60 )
{
	if( !pAstroImage ) return;

	int i = 0;
	wxString strName;

	// now first we free prevoius allocated - if any
	FreeAsteroids( );

	if( nLimit == 0 )
	{
		Refresh( FALSE );
		Update( );

		return;
	}

	// and just set the source no
	m_nAsteroids = pAstroImage->m_nCatAsteroid;

	// set m_nStar with limit ?
	if( nLimit < pAstroImage->m_nCatAsteroid && nLimit >= 0 )
		m_nCatAsteroidsDisplayLimit = nLimit;
	else if( nLimit == -2 )
		m_nCatAsteroidsDisplayLimit = 0;
	else
		m_nCatAsteroidsDisplayLimit = pAstroImage->m_nCatAsteroid;

	// allocate vector for names
	m_vectAsteroidsName = (wxChar**) calloc( m_nAsteroids, sizeof(wxChar*) );
	// allocate vector for orbits
	//m_vectAsteroidsOrbit = (SOLARORBIT*) calloc( m_nAsteroids, sizeof(SOLARORBIT) );
	m_vectAsteroidsOrbit = new SOLARORBIT[m_nAsteroids];

	// set current longitude/latitude
	double nRa=0.0, nDec=0.0, nX=0.0, nY=0.0; 
	double nLat=pAstroImage->m_nObsLatitude, nLon=pAstroImage->m_nObsLongitude, 
			nAlt=pAstroImage->m_nObsAltitude;
	double timeTaken = pAstroImage->m_nObsDateTime.GetJDN();

	////////////////////////////////////
	// set names for extra-solar planets 
	for( i=0; i<m_nAsteroids; i++ )
	{
		DefCatAsteroid* pObj = &(pAstroImage->m_vectCatAsteroid[i]);

		// no try to set name
		if( m_pSky->m_pCatalogAsteroids->GetName( *pObj, strName ) )
		{
			///////////////
			// calculte the orbit point 
			// use step 0.01 = aprox 14.4 minutes ?
			double t=0.0;
			int bOut = 0;
			m_vectAsteroidsOrbit[i].clear();
			// star with current time and decrese first
			for( t=timeTaken; bOut == 0; t -= 0.1 )
			{
				// calculate position at that point
				m_pSky->m_pCatalogAsteroids->CalcRaDecX( *pObj, t, nLat, nLon, nAlt, nRa, nDec );

				// project from sky to image
				m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, nX, nY );

				/////////////////////////
				// check if object is out of  my image
				if( nX < 0 || nY < 0 || nX > pAstroImage->m_nWidth || nY > pAstroImage->m_nHeight )
					bOut = 1;

				//  if all ok insert at the beginign of the vector
				ORBITPOINT ptOrb;
				ptOrb.x = nX;
				ptOrb.y = nY;
				ptOrb.ra = nRa;
				ptOrb.dec = nDec;
				ptOrb.orb_time = t;
				m_vectAsteroidsOrbit[i].insert( m_vectAsteroidsOrbit[i].begin( ), ptOrb );
			}
			// then start with current time and increase
			bOut = 0;
			for( t=timeTaken+0.1; bOut == 0; t += 0.1 )
			{
				// calculate position at that point
				m_pSky->m_pCatalogAsteroids->CalcRaDecX( *pObj, t, nLat, nLon, nAlt, nRa, nDec );

				// project from sky to image
				m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, nX, nY );

				/////////////////////////
				// check if object is out of  my image
				if( nX < 0 || nY < 0 || nX > pAstroImage->m_nWidth || nY > pAstroImage->m_nHeight )
					bOut = 1;

				//  if all ok insert at the beginign of the vector
				ORBITPOINT ptOrb;
				ptOrb.x = nX;
				ptOrb.y = nY;
				ptOrb.ra = nRa;
				ptOrb.dec = nDec;
				ptOrb.orb_time = t;
				m_vectAsteroidsOrbit[i].push_back( ptOrb );

			}

			// now allocate memory for the name and set it
			//int nStrSize = strlen(strName);
			m_vectAsteroidsName[i] = (wxChar*) malloc( (strName.Length()+20)*sizeof(wxChar) );
			//strcpy( m_vectAsteroidsName[i], strName );

			// add to name time 
			int nTrackTime = (m_vectAsteroidsOrbit[i].size()-1)*6;
			int nTrackTimeH = (int) (nTrackTime/60);
			int nTrackTimeM = nTrackTime % 60;
			wxSprintf( m_vectAsteroidsName[i], wxT("%s - %dh:%dm"), strName,
						 nTrackTimeH, nTrackTimeM );
		}
	}

	Refresh( FALSE );
	Update( );

}

////////////////////////////////////////////////////////////////////
// Method:	SetComets
// Class:	CAstroImgView
// Purose:	set comets names with display limit
// Input:	pointer to astroimage and disaply limit
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetComets( CAstroImage* pAstroImage, int nLimit = 60 )
{
	if( !pAstroImage ) return;

	int i = 0;
	wxString strName;

	// now first we free prevoius allocated - if any
	FreeComets( );

	if( nLimit == 0 )
	{
		Refresh( FALSE );
		Update( );

		return;
	}

	// and just set the source no
	m_nComets = pAstroImage->m_nCatComet;

	// set display limit 
	if( nLimit < pAstroImage->m_nCatComet && nLimit >= 0 )
		m_nCatCometsDisplayLimit = nLimit;
	else if( nLimit == -2 )
		m_nCatCometsDisplayLimit = 0;
	else
		m_nCatCometsDisplayLimit = pAstroImage->m_nCatComet;

	// allocate vector for names
	m_vectCometsName = (wxChar**) calloc( m_nComets, sizeof(wxChar*) );
	// allocate vector for orbits
	//m_vectCometsOrbit = (SOLARORBIT*) calloc( m_nComets+1, sizeof(SOLARORBIT) );
	m_vectCometsOrbit = new SOLARORBIT[m_nComets];

	// set current longitude/latitude
	double nRa=0.0, nDec=0.0, nX=0.0, nY=0.0; 
	double nLat=pAstroImage->m_nObsLatitude, nLon=pAstroImage->m_nObsLongitude, 
			nAlt=pAstroImage->m_nObsAltitude;
	double timeTaken = pAstroImage->m_nObsDateTime.GetJDN();
//	StarDef star;

	////////////////////////////////////
	// set names/orbits for comets
	for( i=0; i<m_nComets; i++ )
	{
		DefCatComet* pObj = &(pAstroImage->m_vectCatComet[i]);

		// no try to set name
		if( m_pSky->m_pCatalogComets->GetName( *pObj, strName ) )
		{
			///////////////
			// calculte the orbit point 
			// use step 0.01 = aprox 14.4 minutes ?
			double t=0.0;
			int bOut = 0;
			m_vectCometsOrbit[i].clear();
			// star with current time and decrese first
			for( t=timeTaken; bOut == 0; t -= 0.1 )
			{
				// calculate position at that point
				m_pSky->m_pCatalogComets->CalcRaDec( *pObj, t, nLat, nLon, nAlt, nRa, nDec );

				// project from sky to image
				m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, nX, nY );

				/////////////////////////
				// check if object is out of  my image
				if( nX < 0 || nY < 0 || nX > pAstroImage->m_nWidth || nY > pAstroImage->m_nHeight )
					bOut = 1;

				//  if all ok insert at the beginign of the vector
				ORBITPOINT ptOrb;
				ptOrb.x = nX;
				ptOrb.y = nY;
				ptOrb.ra = nRa;
				ptOrb.dec = nDec;
				ptOrb.orb_time = t;
				m_vectCometsOrbit[i].insert( m_vectCometsOrbit[i].begin( ), ptOrb );
			}
			// then start with current time and increase
			bOut = 0;
			for( t=timeTaken+0.1; bOut == 0; t += 0.1 )
			{
				// calculate position at that point
				m_pSky->m_pCatalogComets->CalcRaDec( *pObj, t, nLat, nLon, nAlt, nRa, nDec );
				// project from sky to image
				m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, nX, nY );

				/////////////////////////
				// check if object is out of  my image
				if( nX < 0 || nY < 0 || nX > pAstroImage->m_nWidth || nY > pAstroImage->m_nHeight )
					bOut = 1;

				//  if all ok insert at the beginign of the vector
				ORBITPOINT ptOrb;
				ptOrb.x = nX;
				ptOrb.y = nY;
				ptOrb.ra = nRa;
				ptOrb.dec = nDec;
				ptOrb.orb_time = t;
				m_vectCometsOrbit[i].push_back( ptOrb );

			}

			// now allocate memory for the name and set it
			//int nStrSize = strlen(strName);
			m_vectCometsName[i] = (wxChar*) malloc( (strName.Length()+20)*sizeof(wxChar) );
			//strcpy( m_vectCometsName[i], strName );

			// add to name time 
			int nTrackTime = (m_vectCometsOrbit[i].size()-1)*6;
			int nTrackTimeH = (int) (nTrackTime/60);
			int nTrackTimeM = nTrackTime % 60;
			wxSprintf( m_vectCometsName[i], wxT("%s - %dh:%dm"), strName,
						 nTrackTimeH, nTrackTimeM );
		}
	}

	Refresh( FALSE );
	Update( );

}

////////////////////////////////////////////////////////////////////
// Method:	SetAes
// Class:	CAstroImgView
// Purose:	set earth satellites names with display limit
// Input:	pointer to astroimage and disaply limit
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetAes( CAstroImage* pAstroImage, int nLimit = 60 )
{
	if( !pAstroImage ) return;

	int i = 0;
	wxString strName;

	// now first we free prevoius allocated - if any
	FreeAes( );

	if( nLimit == 0 )
	{
		Refresh( FALSE );
		Update( );

		return;
	}

	// and just set the source no
	m_nAes = pAstroImage->m_nCatAes;

	// set display limit 
	if( nLimit < pAstroImage->m_nCatAes && nLimit >= 0 )
		m_nCatAesDisplayLimit = nLimit;
	else if( nLimit == -2 )
		m_nCatAesDisplayLimit = 0;
	else
		m_nCatAesDisplayLimit = pAstroImage->m_nCatAes;

	// allocate vector for names
	m_vectAesName = (wxChar**) calloc( m_nAes, sizeof(wxChar*) );
	// allocate vector for orbits
	//m_vectAesOrbit = (SOLARORBIT*) calloc( m_nAes, sizeof(SOLARORBIT) );
	m_vectAesOrbit = new SOLARORBIT[m_nAes];

	// set current longitude/latitude
	double nRa=0.0, nDec=0.0, nX=0.0, nY=0.0, nSatDist=0.0; 
	double nLat=pAstroImage->m_nObsLatitude, nLon=pAstroImage->m_nObsLongitude, 
			nAlt=pAstroImage->m_nObsAltitude;
	double timeTaken = pAstroImage->Get_UTC_JDN();
	StarDef star;

	// get cat ref
	CSkyCatalogAes* pCat = m_pSky->m_pCatalogAes;

	////////////////////////////////////
	// set names/orbits for satellites
	for( i=0; i<m_nAes; i++ )
	{
		// no try to set name
		if( pCat->GetName( pAstroImage->m_vectCatAes[i], strName ) )
		{
			//////////////////////
			// first we estimate the best step for an orbit point
			double xA=0.0, yA=0.0, xB=0.0, yB=0.0;
			// calculate position at point A
			pCat->CalcRaDec( pAstroImage->m_vectCatAes[i], 
								timeTaken+pAstroImage->m_vectCatAes[i].detect_time_shift, 
								nLat, nLon, nAlt, nRa, nDec, nSatDist, 1 );
			// project A from sky
			m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, xA, yA );
			///////////
			// calculate position at point B + 0.00001
			pCat->CalcRaDec( pAstroImage->m_vectCatAes[i], 
												timeTaken+pAstroImage->m_vectCatAes[i].detect_time_shift+0.00001, 
												nLat, nLon, nAlt, nRa, nDec, nSatDist, 1 );
			// project B from sky
			m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, xB, yB );

			// estimate best step = 10 pixels
			double nSampleDist = sqrt( sqr(xB-xA) + sqr(yB-yA) );
			double nOrbTimeStep = (10.0*0.00001)/nSampleDist;

			///////////////
			// calculte the orbit points
			// use step 0.01 = aprox 14.4 minutes ?
			double t=0.0, firstTime=0.0, lastTime=0.0;
			int bOut = 0;
			int nMaxCount=0;
			m_vectAesOrbit[i].clear();
			// start with current time and decrese first
			for( t=timeTaken+pAstroImage->m_vectCatAes[i].detect_time_shift; 
					bOut == 0 && nMaxCount<MAX_ORBIT_POINTS_IN_IMAGE; t -= nOrbTimeStep )
			{
				// calculate position at that point
				pCat->CalcRaDec( pAstroImage->m_vectCatAes[i], t, nLat, nLon, nAlt, nRa, nDec, nSatDist, 1 );
				// project from sky to image
				m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, nX, nY );

				/////////////////////////
				// check if object is out of  my image
				if( nX < 0 || nY < 0 || nX > pAstroImage->m_nWidth || nY > pAstroImage->m_nHeight )
				{
					bOut = 1;
					firstTime = t;
				}
				//  if all ok insert at the beginign of the vector
				ORBITPOINT ptOrb;
				ptOrb.x = nX;
				ptOrb.y = nY;
				ptOrb.ra = nRa;
				ptOrb.dec = nDec;
				ptOrb.orb_time = t;
				m_vectAesOrbit[i].insert( m_vectAesOrbit[i].begin( ), ptOrb );

				nMaxCount++;
			}
			// then start with current time and increase
			bOut = 0;
			nMaxCount = 0;
			for( t=timeTaken+pAstroImage->m_vectCatAes[i].detect_time_shift+nOrbTimeStep; 
					bOut == 0 && nMaxCount<MAX_ORBIT_POINTS_IN_IMAGE; t += nOrbTimeStep )
			{
				// calculate position at that point
				pCat->CalcRaDec( pAstroImage->m_vectCatAes[i], t, nLat, nLon, nAlt, nRa, nDec, nSatDist, 1 );
				// project from sky to image
				m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, nX, nY );

				/////////////////////////
				// check if object is out of  my image
				if( nX < 0 || nY < 0 || nX > pAstroImage->m_nWidth || nY > pAstroImage->m_nHeight )
				{
					bOut = 1;
					lastTime = t;
				}
				//  if all ok insert at the beginign of the vector
				ORBITPOINT ptOrb;
				ptOrb.x = nX;
				ptOrb.y = nY;
				ptOrb.ra = nRa;
				ptOrb.dec = nDec;
				ptOrb.orb_time = t;
				m_vectAesOrbit[i].push_back( ptOrb );

				nMaxCount++;
			}

			// now allocate memory for the name and set it
			//int nStrSize = strlen(strName);
			m_vectAesName[i] = (wxChar*) malloc( (strName.Length()+20)*sizeof(wxChar) );
			//strcpy( m_vectAesName[i], strName );

			// add to name time 
/*			long nTrackTime = (long) round((lastTime-firstTime)*86400.0);
			int nTrackTimeH = (int) (nTrackTime/60);
			int nTrackTimeM = nTrackTime % 60;
			sprintf( m_vectAesName[i], "%s - %dh:%dm", strName,
						 nTrackTimeH, nTrackTimeM );
*/
			wxDateTime myTime;
			myTime.Set( pAstroImage->m_nObsDateTime.GetJDN()+pAstroImage->m_vectCatAes[i].detect_time_shift );

			wxSprintf( m_vectAesName[i], wxT("%s - %s"), strName, myTime.FormatTime() );
		}
	}

	Refresh( FALSE );
	Update( );

}

////////////////////////////////////////////////////////////////////
// Method:	SetSolar
// Class:	CAstroImgView
// Purose:	set solar planets/sun/moon with display limit
// Input:	pointer to astroimage and disaply limit
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::SetSolar( CAstroImage* pAstroImage, int nLimit = 60 )
{
	if( !pAstroImage ) return;

	int i = 0;
	wxString strName;

	// now first we free prevoius allocated - if any
	FreeSolar( );

	if( nLimit == 0 )
	{
		Refresh( FALSE );
		Update( );

		return;
	}

	// and just set the source no
	m_nSolar = pAstroImage->m_nSolarPlanets;

	// set display limit 
	if( nLimit < pAstroImage->m_nSolarPlanets && nLimit >= 0 )
		m_nSolarDisplayLimit = nLimit;
	else if( nLimit == -2 )
		m_nSolarDisplayLimit = 0;
	else
		m_nSolarDisplayLimit = pAstroImage->m_nSolarPlanets;

	// allocate vector for names
	m_vectSolarName = (wxChar**) calloc( m_nSolar, sizeof(wxChar*) );
	// allocate vector for orbits
	//m_vectSolarOrbit = (SOLARORBIT*) calloc( m_nSolar, sizeof(SOLARORBIT) );
	m_vectSolarOrbit = new SOLARORBIT[m_nSolar];

	// set current longitude/latitude
	int bVis=0;
	double nRa=0.0, nDec=0.0, nX=0.0, nY=0.0, nSolDist=0.0; 
	double nLat=pAstroImage->m_nObsLatitude, nLon=pAstroImage->m_nObsLongitude, 
			nAlt=pAstroImage->m_nObsAltitude;
	double timeTaken = pAstroImage->Get_UTC_JDN();

	// get cat ref
	CSkyCatalogAes* pCat = m_pSky->m_pCatalogAes;

	////////////////////////////////////
	// set names/orbits for satellites
	for( i=0; i<m_nSolar; i++ )
	{
		// no try to set name
		if( Get_Solar_Object_Name( pAstroImage->m_vectSolarPlanets[i], strName ) )
		{
			/////////////////////////////
			// CASE :: natural satellite
			if( pAstroImage->m_vectSolarPlanets[i].obj_type == 1 )
			{
				//int nStrSize = strlen(strName);
				m_vectSolarName[i] = (wxChar*) malloc( (strName.Length()+20)*sizeof(wxChar) );
				bzero( m_vectSolarName[i], strName.Length()+20 );

				wxSprintf( m_vectSolarName[i], wxT("%s"), strName );

				//wxDateTime myTime;
				//myTime.Set( pAstroImage->m_nObsDateTime.GetJDN()+pAstroImage->m_vectSolarPlanets[i].detect_time_shift );

				//sprintf( m_vectSolarName[i], "%s - %s", strName, myTime.FormatTime().GetData() );

				continue;
			}

			//////////////////////
			// first we estimate the best step for an orbit point
			double xA=0.0, yA=0.0, xB=0.0, yB=0.0;
			// calculate position at point A
			SolarPlanet_CalcRaDec( pAstroImage->m_vectSolarPlanets[i], 
								timeTaken+pAstroImage->m_vectSolarPlanets[i].detect_time_shift, 
								nLat, nLon, nAlt, nRa, nDec, nSolDist, bVis );
			// project A from sky
			m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, xA, yA );
			///////////
			// calculate position at point B + 0.00001
			SolarPlanet_CalcRaDec( pAstroImage->m_vectSolarPlanets[i], 
								timeTaken+pAstroImage->m_vectSolarPlanets[i].detect_time_shift+0.001, 
								nLat, nLon, nAlt, nRa, nDec, nSolDist, bVis );
			// project B from sky
			m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, xB, yB );

			// estimate best step = 10 pixels
			double nSampleDist = sqrt( sqr(xB-xA) + sqr(yB-yA) );
			double nOrbTimeStep = (10.0*0.001)/nSampleDist;

			///////////////
			// calculte the orbit points
			// use step 0.01 = aprox 14.4 minutes ?
			double t=0.0, firstTime=0.0, lastTime=0.0;
			int bOut = 0;
			int nMaxCount=0;
			m_vectSolarOrbit[i].clear();

			// start with current time and decrese first
			for( t=timeTaken+pAstroImage->m_vectSolarPlanets[i].detect_time_shift; 
					bOut == 0 && nMaxCount<MAX_ORBIT_POINTS_IN_IMAGE; t -= nOrbTimeStep )
			{
				// calculate position at that point
				SolarPlanet_CalcRaDec( pAstroImage->m_vectSolarPlanets[i], t, nLat, nLon, nAlt, nRa, nDec, nSolDist, bVis );
				// project from sky to image
				m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, nX, nY );

				/////////////////////////
				// check if object is out of  my image
				if( nX < 0 || nY < 0 || nX > pAstroImage->m_nWidth || nY > pAstroImage->m_nHeight )
				{
					bOut = 1;
					firstTime = t;
				}
				//  if all ok insert at the begining of the vector
				ORBITPOINT ptOrb;
				ptOrb.x = nX;
				ptOrb.y = nY;
				ptOrb.ra = nRa;
				ptOrb.dec = nDec;
				ptOrb.orb_time = t;
				m_vectSolarOrbit[i].insert( m_vectSolarOrbit[i].begin( ), ptOrb );

				nMaxCount++;
			}

			// then start with current time and increase
			bOut = 0;
			nMaxCount = 0;
			for( t=timeTaken+pAstroImage->m_vectSolarPlanets[i].detect_time_shift+nOrbTimeStep; 
					bOut == 0 && nMaxCount<MAX_ORBIT_POINTS_IN_IMAGE; t += nOrbTimeStep )
			{
				// calculate position at that point
				SolarPlanet_CalcRaDec( pAstroImage->m_vectSolarPlanets[i], t, nLat, nLon, nAlt, nRa, nDec, nSolDist, bVis );
				// project from sky to image
				m_pSky->ProjectSkyCoordToImageXY( pAstroImage, nRa, nDec, nX, nY );

				/////////////////////////
				// check if object is out of  my image
				if( nX < 0 || nY < 0 || nX > pAstroImage->m_nWidth || nY > pAstroImage->m_nHeight )
				{
					bOut = 1;
					lastTime = t;
				}
				//  if all ok insert at the end of the vector
				ORBITPOINT ptOrb;
				ptOrb.x = nX;
				ptOrb.y = nY;
				ptOrb.ra = nRa;
				ptOrb.dec = nDec;
				ptOrb.orb_time = t;
				m_vectSolarOrbit[i].push_back( ptOrb );

				nMaxCount++;
			}

			// now allocate memory for the name and set it
			//int nStrSize = strlen(strName);
			m_vectSolarName[i] = (wxChar*) malloc( (strName.Length()+20)*sizeof(wxChar) );
			bzero( m_vectSolarName[i], strName.Length()+20 );
			//strcpy( m_vectAesName[i], strName );

			// add to name time 
/*			long nTrackTime = (long) round((lastTime-firstTime)*86400.0);
			int nTrackTimeH = (int) (nTrackTime/60);
			int nTrackTimeM = nTrackTime % 60;
			sprintf( m_vectAesName[i], "%s - %dh:%dm", strName,
						 nTrackTimeH, nTrackTimeM );
*/

			wxDateTime myTime;
			myTime.Set( pAstroImage->m_nObsDateTime.GetJDN()+pAstroImage->m_vectSolarPlanets[i].detect_time_shift );

			wxSprintf( m_vectSolarName[i], wxT("%s - %s"), strName, myTime.FormatTime() );
		}
	}

	Refresh( FALSE );
	Update( );

}

////////////////////////////////////////////////////////////////////
void CAstroImgView::OnRightClick( wxMouseEvent& pEvent )
{
	if( !m_bHasImage ) return;
	// local variables
	long	x = pEvent.GetX(),
			y = pEvent.GetY();

	/////////
	// check if area edit tool
	if( m_nAreaEdit >= 0 )
	{
			m_nAreaSelected = -1;
			m_nAreaEdit = -1;
			m_AreaResizeVertex = -1;
			m_bAreaMove = 0;
			SetTCursor();
			Refresh(false);
			return;
	}

	if( m_nInUseTool == IN_USE_TOOL_DEFAULT ) 
	{
		// reset flags
//		m_nAreaSelected = -1;
//		m_nAreaEdit = -1;

		// transform to image coord
		double nX = x - m_nPicOrigX;
		double nY = y - m_nPicOrigY;

		// now if i am zoom mode
		if( m_nZoomCount > 0.5 )
		{
			nX = (nX * m_nCutWinRatio) + m_nCutX;
			nY = (nY * m_nCutWinRatio) + m_nCutY;

		} else
		{
			nX = (double) nX/m_nScaleFactor;
			nY = (double) nY/m_nScaleFactor;
		}

		int i=0;
		m_nAreaSelected = -1;
		// look for areas 
		for( i=0; i<m_pAstroImage->m_vectArea.size(); i++ )
		{
			// check if inside the area
			if( nX >= m_pAstroImage->m_vectArea[i].x && nX < m_pAstroImage->m_vectArea[i].x+m_pAstroImage->m_vectArea[i].width &&
				nY >= m_pAstroImage->m_vectArea[i].y && nY < m_pAstroImage->m_vectArea[i].y+m_pAstroImage->m_vectArea[i].height )
			{
				m_nAreaSelected = i;
			}
		}

		// todo: look for objects

		//if( m_nAreaSelected >=0 ) 
		Refresh(false);
	}
	
	// call to base
	CDynamicImgView::OnRightClick( pEvent );
}

////////////////////////////////////////////////////////////////////
// Method:	OnMouseLDClick
// Class:	CAstroImgView
// Purose:	action on mouse duble click
// Input:	reference to mouse event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::OnMouseLDClick( wxMouseEvent& pEvent )
{
	if( !m_bHasImage ) return;
	// if in use tool other then default dont't process! ?? tothink??
	if( m_nInUseTool != IN_USE_TOOL_DEFAULT ) return;

	/////////
	// check if area edit tool
	if( m_nAreaEdit >= 0 )
	{
			m_nAreaSelected = -1;
			m_nAreaEdit = -1;
			m_AreaResizeVertex = -1;
			m_bAreaMove = 0;
			SetTCursor();
			Refresh(false);
			return;
	}

	// get my new coord
	m_nMouseWinX = pEvent.GetX();
	m_nMouseWinY = pEvent.GetY();

	if( !IsInPicture( m_nMouseWinX, m_nMouseWinY ) ) return;

	// get win coord relative to image
	m_nMouseWinPicX = m_nMouseWinX - m_nPicOrigX;
	m_nMouseWinPicY = m_nMouseWinY - m_nPicOrigY;

	// get picture coord
	// now if i am zoom mode
	if( m_nZoomCount > 0.5 )
	{
		m_nMousePicX = (double) ((double) m_nMouseWinPicX*m_nCutWinRatio) + m_nCutX;
		m_nMousePicY = (double) ((double) m_nMouseWinPicY*m_nCutWinRatio) + m_nCutY;
	} else
	{
		m_nMousePicX = (double) m_nMouseWinPicX/m_nScaleFactor;
		m_nMousePicY = (double) m_nMouseWinPicY/m_nScaleFactor;
	}

	// todo: here check if camera and other key pressed like ctrl???
	if( m_bIsCamera || m_bIsCameraVideo )
	{
		OnCamMouseLDClick( m_nMouseWinX, m_nMouseWinY );
		return;

	} else if( !m_pAstroImage ) 
		return;


	///////////////////////////
	// from here do the normal image stuff
	wxString strMsg=wxT("");
	wxString strStarName = wxT("");
	int nFoundType = 0; 
	int nFound = -1;

//	int x = pEvent.GetX(),
//		y = pEvent.GetY();

	// find object in image
	nFound = m_pAstroImage->GetObjectAtXY( m_nMousePicX, m_nMousePicY, -1, -1, nFoundType, 0 );

	// debug size
//	int nsize = sizeof( CStarViewDlg );

	//////////////////////////////////////
	// now if we found it display 
	if( nFound >= 0 )
	{
		// now by type of findind
		if( nFoundType == 1 )
		{
			int bHasName = m_pSky->m_pCatalogStarNames->GetStarSimpleCatName( m_pAstroImage->m_vectStar[nFound], strStarName );
			if( bHasName )
				strMsg.Printf( wxT("Found star name=%s"), strStarName );
			else
				strMsg.Printf( wxT("Found star id=%d"), nFound );

//			wxMessageBox( strMsg, _T("Found Star") );

			wxString strTitle = wxT("");
			if( bHasName )
				strTitle.Printf( wxT("%s%s"), wxT("Object :: ") , strStarName );
			else
				strTitle.Printf( wxT("%s%d"), wxT("Object :: ") , m_pAstroImage->m_vectStar[nFound].img_id );

			// display star dlg
			m_pObjectView = new CStarViewDlg( m_pSky, this, strTitle, this->m_pFrame->m_pUnimap->m_pUnimapWorker );
			// Get online data
			m_pObjectView->SetObjectData( m_pAstroImage, *m_pImage, &m_pAstroImage->m_vectStar[nFound], 1 );

		} else if( nFoundType == 2 )
		{
			//int bHasName = m_pAstroImage->GetCatStarSimpleCatName( nFound, strStarName );
			int bHasName = m_pSky->m_pCatalogStarNames->GetStarSimpleCatName( m_pAstroImage->m_vectCatStar[nFound], strStarName );
			if( bHasName )
				strMsg.Printf( wxT("Found star name=%s"), strStarName );
			else
				strMsg.Printf( wxT("Found star id=%d"), nFound );

//			wxMessageBox( strMsg, _T("Found Star") );

			wxString strTitle = wxT("");
			if( bHasName )
				strTitle.sprintf( wxT("%s%s"), wxT("Object :: ") , strStarName );
			else
				strTitle.sprintf( wxT("%s%d"), wxT("Object :: ") , m_pAstroImage->m_vectCatStar[nFound].img_id );

			// display star dlg
			m_pObjectView = new CStarViewDlg( m_pSky, this, strTitle, this->m_pFrame->m_pUnimap->m_pUnimapWorker );
			// Get online data
			m_pObjectView->SetObjectData( m_pAstroImage, *m_pImage, &m_pAstroImage->m_vectCatStar[nFound], 2 ); 
		
		// type 3 = DSO
		} else if( nFoundType == 3 )
		{
			//int bHasName = m_pAstroImage->GetDsoSimpleCatName( nFound, strStarName );
			int bHasName = m_pSky->m_pCatalogDsoNames->GetDsoSimpleCatName( m_pAstroImage->m_vectCatDso[nFound], strStarName );
			if( bHasName )
				strMsg.Printf( wxT("Found DSO name=%s"), strStarName );
			else
				strMsg.Printf( wxT("Found DSO id=%d"), nFound );

//			wxMessageBox( strMsg, _T("Found Star") );

			wxString strTitle = wxT("");
			if( bHasName )
				strTitle.sprintf( wxT("%s%s"), wxT("Object :: ") , strStarName );
			else
				strTitle.sprintf( wxT("%s%d"), wxT("Object :: ") , m_pAstroImage->m_vectCatDso[nFound].img_id );

			// display star/dso dlg
			m_pObjectView = new CStarViewDlg( m_pSky, this, strTitle, this->m_pFrame->m_pUnimap->m_pUnimapWorker );
			// Get online data
			m_pObjectView->SetObjectData( m_pAstroImage, *m_pImage, &m_pAstroImage->m_vectCatDso[nFound], 3 );

		} else
			return;

		////////////////////
		// show object details dialog
		m_pObjectView->DisplayObjectData( );
		// show the dialog window
		m_pObjectView->ShowModal();

		delete( m_pObjectView );
		m_pObjectView = NULL;

	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnCamMouseLDClick
// Class:	CAstroImgView
// Purose:	action on mouse duble click
// Input:	reference to mouse event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::OnCamMouseLDClick( int x, int y )
{
	// set track vars
	m_nTrackX = m_nMousePicX;
	m_nTrackY = m_nMousePicY;

	////////////
	// call to locate closest object - OR BETTER CALL DETECT ON REGION
	if( m_pProcImgShift ) m_pProcImgShift->SetTarget( m_nTrackX, m_nTrackY );

	// set main target flag
	m_bMainTarget = 2;

	// set focus view
	if( m_bMainTarget && m_pFrame->m_pFocusView )
	{
		////////////
		// hard code setup to show - todo: remove
		// do set radial plot
		StarDef obj; 
		// position
		obj.x = m_nTrackX; 
		obj.y = m_nTrackY; 
		// sizw
		obj.a = 10.0; 
		obj.b = 10.0; 
		// orientation
		obj.theta = 0.0; 
		// ??
		obj.kronfactor = 3.0; 
		obj.fwhm = 8.514443;
		m_pFrame->m_pFocusView->SetData( *m_pImage, &obj, 0 );
		m_pFrame->m_pFocusView->Refresh( false );
	}

	Refresh( false );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnLeftClickDown
// Class:	CAstroImgView
// Purpose:	when left click down - do set object pick tool etc 
// Input:	mouse events
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::OnLeftClickDown( wxMouseEvent& pEvent )
{
	pEvent.Skip();

	// local variables
	long	x = pEvent.GetX(),
			y = pEvent.GetY();
	// radius to check 
	double nRadius = 10;
	// adjust to image
	int nX = x-m_nPicOrigX;
	int nY = y-m_nPicOrigY;
	// if zoom
	if( m_nZoomCount > 0.5 )
	{
		nX = (nX-0.499)/m_nLastZoomFactor+m_nCutX*m_nScaleFactor;
		nY = (nY-0.499)/m_nLastZoomFactor+m_nCutY*m_nScaleFactor;
		//nRadius = nRadius/m_nLastZoomFactor+1;
	}
	// fina scale factor
	nX = nX/m_nScaleFactor;
	nY = nY/m_nScaleFactor;

	///////////////////////////
	// IF CTRL DOWN AND DEFAULT TOOL
	bool isCtrl = ::wxGetKeyState( WXK_CONTROL );
	if( isCtrl && m_nInUseTool == IN_USE_TOOL_DEFAULT && 
		m_nLastOverObjectType >=0 && m_nLastOverObject >= 0 ) 
	{
		if( m_nLastOverObjectType == 1 )
			m_pAstroImage->m_vectStar[m_nLastOverObject].state ^= OBJECT_STATE_CUSTOM_SHOW;
		else if( m_nLastOverObjectType == 2 )
			m_pAstroImage->m_vectCatStar[m_nLastOverObject].state ^= OBJECT_STATE_CUSTOM_SHOW;
		else if( m_nLastOverObjectType == 3 )
			m_pAstroImage->m_vectCatDso[m_nLastOverObject].state ^= OBJECT_STATE_CUSTOM_SHOW;
	}

	///////////////////////////
	// if we have the measure tool stuff
	if( m_bIsDistanceTool )
	{
		// set tool drag B - to do adjust radius
		if( IsPointInRadius( (double) nX, (double) nY, m_pDistTool->m_nDistanceToolB_X, m_pDistTool->m_nDistanceToolB_Y, nRadius ) )
		{
			m_bIsDistanceToolDragB = 1;

		} else if( IsPointInRadius( (double) nX, (double) nY, m_pDistTool->m_nDistanceToolA_X, m_pDistTool->m_nDistanceToolA_Y, nRadius ) )
		{
			m_bIsDistanceToolDragA = 1;
		}

	}

	return;
}

////////////////////////////////////////////////////////////////////
// Purpose:	when left click is released
////////////////////////////////////////////////////////////////////
void CAstroImgView::OnLeftClickUp( wxMouseEvent& pEvent )
{
	pEvent.Skip();

	if( !m_bHasImage ) return;

	// local variables
	long	x = pEvent.GetX(),
			y = pEvent.GetY();
	
	// reset some flags
	if( m_bIsDistanceToolDragA ) m_bIsDistanceToolDragA = 0;
	if( m_bIsDistanceToolDragB ) m_bIsDistanceToolDragB = 0;

	// ??? - todo: remove this if not used
//	if( m_bAreaDragging == 1 )
///	{
//		m_bAreaDragging = 0;
//		m_vectStarState[m_nCurrentAreaDragging] = m_nPrevAreaState;

//		Refresh( FALSE );
//		Update( );

//	} 

	///////////////////
	// if to add area
	if( m_nInUseTool == IN_USE_TOOL_ADD_AREA )
	{
		if( m_bDragging == 1 )
		{
			int nDragDiffX = x-m_nStartDragX;
			int nDragDiffY = y-m_nStartDragY;

			m_bDragging = 0;
			m_nDragLastX = 0;
			m_nDragLastY = 0;

			// add area
			if( m_nStartDragX >= 0 &&  m_nStartDragY >= 0 && 
				abs(nDragDiffX) > 2 && abs(nDragDiffY) > 2 )
			{
				int ax=m_nStartDragX, ay=m_nStartDragY, 
					aw=nDragDiffX, ah=nDragDiffY;
				double nX=0.0, nY=0.0, nW=0.0, nH=0.0;

				// adjust negative area  on x
				if( aw < 0 )
				{
					aw = abs(aw);
					ax -= aw;
				}
				// adjust negative area on y
				if( ah < 0 )
				{
					ah = abs(ah);
					ay -= ah;
				}

				// calculate mouse position inside picture
				nX = ax - m_nPicOrigX;
				nY = ay - m_nPicOrigY;

				// now if i am zoom mode
				if( m_nZoomCount > 0.5 )
				{
					nX = (nX * m_nCutWinRatio) + m_nCutX;
					nY = (nY * m_nCutWinRatio) + m_nCutY;

					nW = (double) aw*m_nCutWinRatio;
					nH = (double) ah*m_nCutWinRatio;

				} else
				{
					nX = (double) nX/m_nScaleFactor;
					nY = (double) nY/m_nScaleFactor;
					nW = (double) aw/m_nScaleFactor;
					nH = (double) ah/m_nScaleFactor;
				}
				// call to add a brand new area
				m_pAstroImage->AddArea( (int) nX, (int) nY, (int) nW, (int) nH );
				m_pAstroImage->m_bIsChanged = 1;
				Refresh(false);
				// togle button/function back off
				SetTCursor();
				m_nInUseTool = IN_USE_TOOL_DEFAULT;
				m_pFrame->GetToolBar()->ToggleTool( wxID_BUTTON_AREA_ADD, 0 );

			}
		}

	} else if( m_bDragging )
	{
		m_bAreaMove = 0;
		m_AreaResizeVertex = -1;
	}

	// call to base
	CDynamicImgView::OnLeftClickUp( pEvent );

	return;
}

///////////////////////////////////////////////////////////////////
// Method:	OnViewWithGoogleOnline
// Class:	CAstroImgView
// Purpose:	open google browser to show this part of the sky
// Input:	reference to command event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::OnViewWithGoogleOnline( wxCommandEvent& pEvent )
{
	if( !m_bHasImage ) return;
	wxString strMyUrl = wxT("");

	// m_pCurrentMouseX, m_pCurrentMouseY
	// get coord relative to image
//	int nX = m_pCurrentMouseX - m_nPicOrigX;
//	int nY = m_pCurrentMouseY - m_nPicOrigY;

	// now if we have a matched image
	if( m_pAstroImage->m_bIsMatched )
	{
		wxString strRa;
		wxString strDec;

		// calculate point position in the original image
//		double nOrigX = m_pCurrentMouseX-m_nPicOrigX; 
//		double nOrigY = m_pCurrentMouseY-m_nPicOrigY; 
		double nOrigX = m_nCurrentMouseX; 
		double nOrigY = m_nCurrentMouseY; 

		// now if i am zoom mode
		if( m_nZoomCount > 0.5 )
		{
			nOrigX = (nOrigX*m_nCutWinRatio) + m_nCutX;
			nOrigY = (nOrigY*m_nCutWinRatio) + m_nCutY;
		} else
		{
			nOrigX = nOrigX/m_nScaleFactor;
			nOrigY = nOrigY/m_nScaleFactor;
		}
		// calculate best trans 
		m_pFrame->m_pStarMatch->CalcPointTrans( nOrigX, nOrigY );
		// de-project coord from image to 
		double x0 = m_pAstroImage->m_nOrigRa;
		double y0 = m_pAstroImage->m_nOrigDec;
		StarDef star;
		star.x = nOrigX/m_pAstroImage->m_nSkyProjectionScale;
		star.y = nOrigY/m_pAstroImage->m_nSkyProjectionScale;

		m_pSky->DeprojectStar( star, x0, y0 );

		RaDegToSexagesimal( star.ra, strRa );
		DecDegToSexagesimal( star.dec, strDec );
		// basic url to open google sky online
		// http://www.google.com/sky/#latitude=5:34:32.01&longitude=32:00:45.96&zoom=11

		strMyUrl.sprintf( wxT("http://www.google.com/sky/#latitude=%.10f&longitude=%.10f&zoom=10&"),
							 star.dec, 180+star.ra );

		wxLaunchDefaultBrowser( strMyUrl, wxBROWSER_NEW_WINDOW );
	}

	return;
}

///////////////////////////////////////////////////////////////////
// Method:	OnViewWithSkyMapOrg
// Class:	CAstroImgView
// Purpose:	open browser to show this part of the sky with sky-map.org
// Input:	reference to command event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::OnViewWithSkyMapOrg( wxCommandEvent& pEvent )
{
	if( !m_bHasImage ) return;
	wxString strMyUrl = wxT("");

	// now if we have a matched image
	if( m_pAstroImage->m_bIsMatched )
	{
		char strRa[255];
		char strDec[255];

		// calculate point position in the original image
		double nOrigX = m_nCurrentMouseX; 
		double nOrigY = m_nCurrentMouseY; 

		// now if i am zoom mode
		if( m_nZoomCount > 0.5 )
		{
			nOrigX = (nOrigX*m_nCutWinRatio) + m_nCutX;
			nOrigY = (nOrigY*m_nCutWinRatio) + m_nCutY;
		} else
		{
			nOrigX = nOrigX/m_nScaleFactor;
			nOrigY = nOrigY/m_nScaleFactor;
		}
		// calculate best trans 
		m_pFrame->m_pStarMatch->CalcPointTrans( nOrigX, nOrigY );
		// de-project coord from image to 
		double x0 = m_pAstroImage->m_nOrigRa;
		double y0 = m_pAstroImage->m_nOrigDec;
		StarDef star;
		star.x = nOrigX/m_pAstroImage->m_nSkyProjectionScale;
		star.y = nOrigY/m_pAstroImage->m_nSkyProjectionScale;

		m_pSky->DeprojectStar( star, x0, y0 );
		// set my coord
		double nRaCenter = (double) star.ra/15.0;
		double nDecCenter = (double) star.dec;
		double nFieldSize = (double) m_pAstroImage->m_nFieldWidth/3.0;

		///////////////////////////////////////
		// TRY TO FIND OBJECT IN THE IMAGE
		int nFoundType = 0;
		int nFound = m_pAstroImage->GetObjectAtXY( nOrigX, nOrigY, -1, -1, nFoundType, 0 );

		//////////////////////////////////////
		// now if we found it display 
		if( nFound >= 0 )
		{
			// now by type of findind type 1=detected star 2=cat dso
			if( nFoundType == 1 )
			{
				nRaCenter = m_pAstroImage->m_vectStar[nFound].ra/15.0;
				nDecCenter = m_pAstroImage->m_vectStar[nFound].dec;

				// calculate real a ... in the sky no in the image
				StarDef star;
				double xd = m_pAstroImage->m_vectStar[nFound].x + m_pAstroImage->m_vectStar[nFound].a;
				double yd = m_pAstroImage->m_vectStar[nFound].y;
				// calculate inverse
				m_pSky->m_pSkyFinder->m_pStarMatch->CalcPointTrans( xd, yd );
				// scale back
				star.x = xd/m_pAstroImage->m_nSkyProjectionScale;
				star.y = yd/m_pAstroImage->m_nSkyProjectionScale;
				x0 = m_pAstroImage->m_nOrigRa;
				y0 = m_pAstroImage->m_nOrigDec;
				// call deproject
				m_pSky->DeprojectStar( star, x0, y0 );
				// calculate distance from center to a
				double nDistA = CalcSkyDistance( m_pAstroImage->m_vectStar[nFound].ra,
									m_pAstroImage->m_vectStar[nFound].dec, star.ra, star.dec );

				nFieldSize = nDistA*5.5;

			} else if( nFoundType == 2 )
			{
				nRaCenter = m_pAstroImage->m_vectCatStar[nFound].ra/15.0;
				nDecCenter = m_pAstroImage->m_vectCatStar[nFound].dec;

				// calculate real a ... in the sky no in the image
				StarDef star;
				double xd = m_pAstroImage->m_vectCatStar[nFound].x + m_pAstroImage->m_vectCatStar[nFound].a;
				double yd = m_pAstroImage->m_vectCatStar[nFound].y;
				// calculate inverse
				m_pSky->m_pSkyFinder->m_pStarMatch->CalcPointTrans( xd, yd );
				// scale back
				star.x = xd/m_pAstroImage->m_nSkyProjectionScale;
				star.y = yd/m_pAstroImage->m_nSkyProjectionScale;
				x0 = m_pAstroImage->m_nOrigRa;
				y0 = m_pAstroImage->m_nOrigDec;
				// call deproject
				m_pSky->DeprojectStar( star, x0, y0 );
				// calculate distance from center to a
				double nDistA = CalcSkyDistance( m_pAstroImage->m_vectCatStar[nFound].ra,
									m_pAstroImage->m_vectCatStar[nFound].dec, star.ra, star.dec );

				nFieldSize = nDistA*5.5;

			} if( nFoundType == 3 )
			{
				nRaCenter = m_pAstroImage->m_vectCatDso[nFound].ra/15.0;
				nDecCenter = m_pAstroImage->m_vectCatDso[nFound].dec;

				// calculate real a ... in the sky no in the image
				StarDef star;
				double xd = m_pAstroImage->m_vectCatDso[nFound].x + m_pAstroImage->m_vectCatDso[nFound].a;
				double yd = m_pAstroImage->m_vectCatDso[nFound].y;
				// calculate inverse
				m_pSky->m_pSkyFinder->m_pStarMatch->CalcPointTrans( xd, yd );
				// scale back
				star.x = xd/m_pAstroImage->m_nSkyProjectionScale;
				star.y = yd/m_pAstroImage->m_nSkyProjectionScale;
				x0 = m_pAstroImage->m_nOrigRa;
				y0 = m_pAstroImage->m_nOrigDec;
				// call deproject
				m_pSky->DeprojectStar( star, x0, y0 );
				// calculate distance from center to a
				double nDistA = CalcSkyDistance( m_pAstroImage->m_vectCatDso[nFound].ra,
									m_pAstroImage->m_vectCatDso[nFound].dec, star.ra, star.dec );

				nFieldSize = nDistA*1.5;
			}
		}


		// basic url to open skymap org
		// http://www.sky-map.org/?ra=2.5301945000000034&de=89.26417000000001&zoom=10&show_grid=1&show_constellation_lines=1&show_constellation_boundaries=1&show_const_names=0&show_galaxies=1&show_box=1&box_ra=2.5301945&box_de=89.26417&box_width=50&box_height=50&img_source=IMG_all

		strMyUrl.sprintf( wxT("http://server3.sky-map.org/snapshot?ra=%.10f&de=%.10f&angle=%.10f&rotation=%.10f&show_grid=1&show_constellation_lines=1&show_constellation_boundaries=1&show_const_names=0&show_galaxies=1&show_box=1&box_ra=%.10f&box_de=%.10f&box_width=50&box_height=50&img_source=IMG_all"),
							 nRaCenter, nDecCenter, nFieldSize, -1.0*m_pAstroImage->m_nImgRotAngle,
							 nRaCenter, nDecCenter );

		wxLaunchDefaultBrowser( strMyUrl ); //wxBROWSER_NEW_WINDOW );
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CAstroImgView::OnViewWithApp( wxCommandEvent& pEvent )
{
	if( !m_bHasImage ) return;
	int nId = pEvent.GetId();

	wxString strArg=wxT("");

	if( nId == wxID_VIEWWITH_GOOGLEEARTH )
	{
		m_pUnimap->m_pAppsMgr->RunApp( MGR_APPS_NAMEID_GOOGLEEARTH, strArg );

	} else if( nId == wxID_VIEWWITH_WORLDWIND )
	{
		m_pUnimap->m_pAppsMgr->RunApp( MGR_APPS_NAMEID_WORLDWIND, strArg );
	}
}

////////////////////////////////////////////////////////////////////
void CAstroImgView::OnSlewToObject( wxCommandEvent& pEvent )
{
	if( !m_bHasImage ) return;
	wxString strRa = wxT("");
	wxString strDec = wxT("");

	// calculate point position in the original image
	double nOrigX = m_nCurrentMouseX; 
	double nOrigY = m_nCurrentMouseY; 

	// now if i am zoom mode
	if( m_nZoomCount > 0.5 )
	{
		nOrigX = (nOrigX*m_nCutWinRatio) + m_nCutX;
		nOrigY = (nOrigY*m_nCutWinRatio) + m_nCutY;
	} else
	{
		nOrigX = nOrigX/m_nScaleFactor;
		nOrigY = nOrigY/m_nScaleFactor;
	}
	// calculate best trans 
	m_pFrame->m_pStarMatch->CalcPointTrans( nOrigX, nOrigY );
	// de-project coord from image to 
	double x0 = m_pAstroImage->m_nOrigRa;
	double y0 = m_pAstroImage->m_nOrigDec;
	StarDef star;
	star.x = nOrigX/m_pAstroImage->m_nSkyProjectionScale;
	star.y = nOrigY/m_pAstroImage->m_nSkyProjectionScale;

	m_pSky->DeprojectStar( star, x0, y0 );

	// set target entry
	m_pFrame->SetTelescopeRaDecEntry( star.ra, star.dec );
	// star move to position
	m_pFrame->SlewToPosition(1);
}

////////////////////////////////////////////////////////////////////
// Method:	FindObjectByString
// Class:	CAstroImgView
// Purpose:	find object by a string
// Input:	string to look for
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::FindObjectByString( wxString& strObjToFind, int nObjType )
{
	int i = 0;
	int nFound = 0;
	wxString strLog;
	wxRegEx reObjPattern;

	// free target data at first
	FreeTargets( );

	// do regex search - build regex pattern first
	if( !reObjPattern.Compile( strObjToFind, wxRE_ICASE ) )
		return( 0 );

	// search detected star names
	for( i=0; i<m_nStar; i++ )
	{
		// check if match
		if( reObjPattern.Matches( m_vectStarName[i] ) )
		{
			// log
			strLog.Printf( wxT("INFO :: FOUND OBJ=%s"), m_vectStarName[i] );
			m_pLogger->Log( strLog );
			// add to my target list
			AddTarget( m_pAstroImage->m_vectStar[i].x, m_pAstroImage->m_vectStar[i].y,
						m_pAstroImage->m_vectStar[i].ra, m_pAstroImage->m_vectStar[i].dec, 
						m_pAstroImage->m_vectStar[i].a, m_pAstroImage->m_vectStar[i].b );
			// increment found counter
			nFound++;
		}
	}
	// search catalog star names
	for( i=0; i<m_nCatStar; i++ )
	{
		// check if match
		if( reObjPattern.Matches( m_vectCatStarName[i] ) )
		{
			// log
			strLog.Printf( wxT("INFO :: FOUND OBJ=%s"), m_vectCatStarName[i] );
			m_pLogger->Log( strLog );
			// add to my target list
			AddTarget( m_pAstroImage->m_vectCatStar[i].x, m_pAstroImage->m_vectCatStar[i].y,
						m_pAstroImage->m_vectCatStar[i].ra, m_pAstroImage->m_vectCatStar[i].dec, 
						m_pAstroImage->m_vectCatStar[i].a, m_pAstroImage->m_vectCatStar[i].b );
			// increment found counter
			nFound++;
		}
	}
	// search catalog dso names
	for( i=0; i<m_nDso; i++ )
	{
		// check if match
		if( reObjPattern.Matches( m_vectDsoName[i] ) )
		{
			// log
			strLog.Printf( wxT("INFO :: FOUND OBJ=%s"), m_vectDsoName[i] );
			m_pLogger->Log( strLog );
			// add to my target list
			AddTarget( m_pAstroImage->m_vectCatDso[i].x, m_pAstroImage->m_vectCatDso[i].y,
						m_pAstroImage->m_vectCatDso[i].ra, m_pAstroImage->m_vectCatDso[i].dec, 
						m_pAstroImage->m_vectCatDso[i].a, m_pAstroImage->m_vectCatDso[i].b );
			// increment found counter
			nFound++;
		}
	}

	Refresh( FALSE );
	Update( );

	return( nFound );
}

////////////////////////////////////////////////////////////////////
// Method:	OnMouseMove
// Class:	CAstroImgView
// Purose:	actions when mouse is moving
// Input:	reference to mouse event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::OnMouseMove( wxMouseEvent& pEvent )
{
	pEvent.Skip();

	if( !m_bHasImage ) return;

	// get my new coord
	m_nMouseWinX = pEvent.GetX();
	m_nMouseWinY = pEvent.GetY();
	// check if it's in picture
	if( !IsInPicture( m_nMouseWinX, m_nMouseWinY ) ) return;

	// get win coord relative to image
	m_nMouseWinPicX = m_nMouseWinX - m_nPicOrigX;
	m_nMouseWinPicY = m_nMouseWinY - m_nPicOrigY;

	// get picture coord
	// now if i am zoom mode
	if( m_nZoomCount > 0.5 )
	{
		m_nMousePicX = (double) ((double) m_nMouseWinPicX*m_nCutWinRatio) + m_nCutX;
		m_nMousePicY = (double) ((double) m_nMouseWinPicY*m_nCutWinRatio) + m_nCutY;
	} else
	{
		m_nMousePicX = (double) m_nMouseWinPicX/m_nScaleFactor;
		m_nMousePicY = (double) m_nMouseWinPicY/m_nScaleFactor;
	}

	// set my frame status with pic coord
	m_pFrame->SetStatusXY( (int) m_nMousePicX, (int) m_nMousePicY );

	/////////////////////////////////////////
	// now if we have a matched image
	if( m_pAstroImage && m_pAstroImage->m_bIsMatched )
	{
		// calculate best trans -- covert for display
		double nX=m_nMousePicX, nY=m_nMousePicY;
		m_pFrame->m_pStarMatch->CalcPointTrans( nX, nY );

		// de-project coord from image to sky
		double x0 = m_pAstroImage->m_nOrigRa;
		double y0 = m_pAstroImage->m_nOrigDec;
		StarDef star;
		star.x = nX/m_pAstroImage->m_nSkyProjectionScale;
		star.y = nY/m_pAstroImage->m_nSkyProjectionScale;

		m_pSky->DeprojectStar( star, x0, y0 );
		// set status bar ra dec in deg
		m_pFrame->SetStatusRaDec( star.ra, star.dec );
	}

	/////////////////////////////////////////
	// INTERACTIVE OBJECT SHOW
	int nFoundObj=0, nFoundType=0, nObjState=0, bOverObjFound=1;
	if( m_pAstroImage && !m_bIsDistanceTool && m_pAstroImage->m_bIsDetected && 
		!m_bDragging && m_nInUseTool != IN_USE_TOOL_ADD_AREA && 
		m_nInUseTool != IN_USE_TOOL_ZOOM && m_nAreaEdit < 0 )
	{
		//StarDef* vectStar = m_pAstroImage->m_vectStar;
		// for all stars in the view

		// find object in image
		nFoundObj = m_pAstroImage->GetObjectAtXY( m_nMousePicX, m_nMousePicY, m_nLastOverObject, m_nLastOverObjectType, nFoundType, 0 ); //FLAG_FIND_OBJ_NOT_SHOWN );

		// IF WHAT IS FOUND IS DIFFERENT IN ID AND TYPE
		if( nFoundObj != m_nLastOverObject || nFoundType != m_nLastOverObjectType )
		{
			//////////////////
			// check if there was a last 
			if( m_nLastOverObject >= 0 )
			{
				nObjState = -1;

				if( m_nLastOverObjectType == 1 )
					nObjState = m_pAstroImage->m_vectStar[m_nLastOverObject].state;
				else if( m_nLastOverObjectType == 2 )
					nObjState = m_pAstroImage->m_vectCatStar[m_nLastOverObject].state;
				else if( m_nLastOverObjectType == 3 )
					nObjState = m_pAstroImage->m_vectCatDso[m_nLastOverObject].state;

				if( nObjState != -1 && !(nObjState & OBJECT_STATE_CUSTOM_SHOW) )
				{
					if( m_nLastOverObjectType == 1 )
						m_pAstroImage->m_vectStar[m_nLastOverObject].state ^= OBJECT_STATE_MOUSE_OVER;
					else if( m_nLastOverObjectType == 2 )
						m_pAstroImage->m_vectCatStar[m_nLastOverObject].state ^= OBJECT_STATE_MOUSE_OVER;
					else if( m_nLastOverObjectType == 3 )
						m_pAstroImage->m_vectCatDso[m_nLastOverObject].state ^= OBJECT_STATE_MOUSE_OVER;

					// set flag to refresh
					bOverObjFound++;
				}

				m_nLastOverObject = -1;
				m_nLastOverObjectType = 0;

				// if there was a last cursor return to it
				//if( m_nLastCursorId >= 0 ) SetTCursor( m_nLastCursorId );
				SetTCursor( UNIMAP_CURSOR_IMAGE_DEFAULT );

				//SetToolTip( NULL );
			} 

			//////////////////////
			// if found object 
			if( nFoundObj >= 0 )
			{
				// in any case set last over object
				m_nLastOverObjectType = nFoundType;
				m_nLastOverObject = nFoundObj;
				// set cursor
				SetTCursor( UNIMAP_CURSOR_OBJ_DETAIL );

				nObjState = -1;

				if( nFoundType == 1 )
					nObjState = m_pAstroImage->m_vectStar[nFoundObj].state;
				else if( nFoundType == 2 )
					nObjState = m_pAstroImage->m_vectCatStar[nFoundObj].state;
				else if( nFoundType == 3 )
					nObjState = m_pAstroImage->m_vectCatDso[nFoundObj].state;

				// IF NOT CUSTOM SHOW
				if( nObjState != -1 && !(nObjState & OBJECT_STATE_CUSTOM_SHOW) )
				{
					// set flag to refresh
					bOverObjFound++;

					if( nFoundType == 1 )
						m_pAstroImage->m_vectStar[nFoundObj].state |= OBJECT_STATE_MOUSE_OVER;
					else if( nFoundType == 2 )
						m_pAstroImage->m_vectCatStar[nFoundObj].state |= OBJECT_STATE_MOUSE_OVER;
					else if( nFoundType == 3 )
						m_pAstroImage->m_vectCatDso[nFoundObj].state |= OBJECT_STATE_MOUSE_OVER;
					else
					{
						m_nLastOverObject = -1;
						m_nLastOverObjectType -1;
						bOverObjFound--;
					}

				}

				// tooltip test
/*				wxToolTip* pTip = GetToolTip();
				if( pTip != NULL )
					pTip->SetTip( wxT("Testing tooltip\nLine 2\line 3") );
				else
				{
					SetToolTip( wxT("set Testing tooltip\nLine 2\line 3") );
				}
*/
			}

			// check if to refresh
			if( bOverObjFound > 0 )
			{
					Refresh(false);
					Update();
			}
		}
	}

	////////////////////////////////////////////
	// IF DISTANCE MEASUMENT TOOL
	if( m_bIsDistanceTool )
	{
		// on tool tip close set measure cursor
		if( m_bIsDistanceToolDragA || m_bIsDistanceToolDragB ||
			IsPointInRadius( (double) m_nMousePicX, (double) m_nMousePicY, m_pDistTool->m_nDistanceToolB_X, m_pDistTool->m_nDistanceToolB_Y, 10.0 ) || 
			IsPointInRadius( (double) m_nMousePicX, (double) m_nMousePicY, m_pDistTool->m_nDistanceToolA_X, m_pDistTool->m_nDistanceToolA_Y, 10.0 ) )

			SetTCursor( UNIMAP_CURSOR_DIST_MEASURMENT );
		else 
			SetTCursor();

	}


	//////////////////////////////////////
	// AREA EDIT MODE
	if( m_nAreaEdit >= 0 && !m_bDragging )
	{
		// covert to view
		double a_x = m_pAstroImage->m_vectArea[m_nAreaEdit].x*m_nScaleFactor;
		double a_y = m_pAstroImage->m_vectArea[m_nAreaEdit].y*m_nScaleFactor;
		double a_w = m_pAstroImage->m_vectArea[m_nAreaEdit].width*m_nScaleFactor;
		double a_h = m_pAstroImage->m_vectArea[m_nAreaEdit].height*m_nScaleFactor;
		// if there is zoom in 
		if( m_nZoomCount > 0.5 )
		{
			a_x = (int) round( (a_x-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
			a_y = (int) round( (a_y-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
			a_w = a_w*m_nLastZoomFactor;
			a_h = a_h*m_nLastZoomFactor;
		}
		// adjust with picture orig x/y
		a_x += m_nPicOrigX;
		a_y += m_nPicOrigY;

		//////////////
		// if inside area
		if( m_nMousePicX > m_pAstroImage->m_vectArea[m_nAreaEdit].x + 4 &&
			m_nMousePicX < m_pAstroImage->m_vectArea[m_nAreaEdit].x + m_pAstroImage->m_vectArea[m_nAreaEdit].width - 4 &&
			m_nMousePicY > m_pAstroImage->m_vectArea[m_nAreaEdit].y + 4 &&
			m_nMousePicY < m_pAstroImage->m_vectArea[m_nAreaEdit].y + m_pAstroImage->m_vectArea[m_nAreaEdit].height - 4 )
		{
			SetTCursor( UNIMAP_CURSOR_AREA_MOVE );
			m_bAreaMove = 1;
			m_AreaResizeVertex = -1;

		} else
		{
			m_bAreaMove = 0;

			// calc diff
			double xd = m_nMouseWinX-a_x;
			double yd = m_nMouseWinY-a_y;
			// calculate distance to current mouse
			double dist_a = sqrt( sqr(xd) + sqr(yd) );
			double dist_b = sqrt( sqr(xd-a_w) + sqr(yd) );
			double dist_c = sqrt( sqr(xd-a_w) + sqr(yd-a_h) );
			double dist_d = sqrt( sqr(xd)+sqr(yd-a_h) );

			// set cursor area resize
			if( dist_a <= 5 || dist_c <= 5 )
			{
				SetTCursor( UNIMAP_CURSOR_AREA_RESIZE_AC );
				// set which vertex to use to resize
				if( dist_a <= 5 )
					m_AreaResizeVertex = 0;
				else
					m_AreaResizeVertex = 2;

			} else if( dist_b <= 5 || dist_d <= 5 )
			{
				SetTCursor( UNIMAP_CURSOR_AREA_RESIZE_BD );
				// set which vertex to use to resize
				if( dist_b <= 5 )
					m_AreaResizeVertex = 1;
				else
					m_AreaResizeVertex = 3;

			} else
			{
				SetTCursor();
				m_AreaResizeVertex = -1;
			}

		}

	}

	// if there is no action do nothing
	if( !pEvent.Dragging() || !pEvent.LeftIsDown() || 
		pEvent.RightIsDown() || pEvent.MiddleIsDown() ) return;

	/////////////////////////////////////////////////
	// FROM HERE WE DO DRAGGING :: image or tools or areas etc
	if( m_bIsDistanceToolDragA || m_bIsDistanceToolDragB )
	{
		// cancel base pic dragging
		m_bDragging = 0;
		// check if object with FLAG_FIND_OBJ_WITH_DISTANCE
		int nFoundType = 0;
		int nFound = m_pAstroImage->GetObjectAtXY( m_nMousePicX, m_nMousePicY, -1, -1, nFoundType, FLAG_FIND_OBJ_WITH_DISTANCE );

		// claculate my image coord
		double nX = (double) m_nMouseWinPicX;
		double nY = (double) m_nMouseWinPicY;

		if( m_nZoomCount > 0.5 )
		{
			nX = (nX-0.499)/m_nLastZoomFactor+m_nCutX*m_nScaleFactor;
			nY = (nY-0.499)/m_nLastZoomFactor+m_nCutY*m_nScaleFactor;
		}

		if( m_bIsDistanceToolDragA )
		{
			if( m_nLastSelectedA >= 0 ) 
			{
				if( m_nLastSelectedTypeA == 1 )
					m_pAstroImage->m_vectStar[m_nLastSelectedA].state ^= OBJECT_STATE_SELECTED;
				else if( m_nLastSelectedTypeA == 2 )
					m_pAstroImage->m_vectCatStar[m_nLastSelectedA].state ^= OBJECT_STATE_SELECTED;
				else if( m_nLastSelectedTypeA == 3 )
					m_pAstroImage->m_vectCatDso[m_nLastSelectedA].state ^= OBJECT_STATE_SELECTED;
			}
			// if found
			if( nFound >=0 )
			{
				m_nLastSelectedA = nFound;
				m_nLastSelectedTypeA = nFoundType;
				// set selection
				if( nFoundType == 1 )
					m_pAstroImage->m_vectStar[nFound].state |= OBJECT_STATE_SELECTED;
				else if( nFoundType == 2 )
					m_pAstroImage->m_vectCatStar[nFound].state |= OBJECT_STATE_SELECTED;
				else if( nFoundType == 3 )
					m_pAstroImage->m_vectCatDso[nFound].state |= OBJECT_STATE_SELECTED;
			} else
			{
				m_nLastSelectedA = -1;
				m_nLastSelectedTypeA = 0;
			}

			m_pDistTool->SetPointA( nX/m_nScaleFactor, nY/m_nScaleFactor );

		} else if( m_bIsDistanceToolDragB )
		{
			if( m_nLastSelectedB >= 0 )
			{
				if( m_nLastSelectedTypeB == 1 )
					m_pAstroImage->m_vectStar[m_nLastSelectedB].state ^= OBJECT_STATE_SELECTED;
				else if( m_nLastSelectedTypeB == 2 )
					m_pAstroImage->m_vectCatStar[m_nLastSelectedB].state ^= OBJECT_STATE_SELECTED;
				else if( m_nLastSelectedTypeB == 3 )
					m_pAstroImage->m_vectCatDso[m_nLastSelectedB].state ^= OBJECT_STATE_SELECTED;
			}
			// if found
			if( nFound >=0 )
			{
				m_nLastSelectedB = nFound;
				m_nLastSelectedTypeB = nFoundType;
				// set selection
				if( nFoundType == 1 )
					m_pAstroImage->m_vectStar[nFound].state |= OBJECT_STATE_SELECTED;
				else if( nFoundType == 2 )
					m_pAstroImage->m_vectCatStar[nFound].state |= OBJECT_STATE_SELECTED;
				else if( nFoundType == 3 )
					m_pAstroImage->m_vectCatDso[nFound].state |= OBJECT_STATE_SELECTED;
			} else
			{
				m_nLastSelectedB = -1;
				m_nLastSelectedTypeB = 0;
			}

			m_pDistTool->SetPointB( nX/m_nScaleFactor, nY/m_nScaleFactor );
		} 

		// resfresh & return
		Refresh( FALSE );
		Update( );

		return;
	}

	/////////////////
	// AT LAST IMAGE DRAG CALL ???? 
	DragOn( m_nMouseWinX, m_nMouseWinY );

	///////////////////////////////
	// if area dragging ???
	if( m_nAreaEdit >=0 && m_bAreaMove == 1 && m_bDragging )
	{
		double nDX=0.0, nDY=0.0;

		if( m_nZoomCount > 0.5 )
		{
			nDX = (double) m_nDragDiffX*m_nCutWinRatio;
			nDY = (double) m_nDragDiffY*m_nCutWinRatio;

		} else
		{
			nDX = (double) m_nDragDiffX/m_nScaleFactor;
			nDY = (double) m_nDragDiffY/m_nScaleFactor;
		}

		double nx = m_pAstroImage->m_vectArea[m_nAreaEdit].x + nDX;
		double ny = m_pAstroImage->m_vectArea[m_nAreaEdit].y + nDY;

		if( nx >= 0 && nx + m_pAstroImage->m_vectArea[m_nAreaEdit].width < m_pAstroImage->m_nWidth )
			m_pAstroImage->m_vectArea[m_nAreaEdit].x = nx;
		if( ny >= 0 && ny + m_pAstroImage->m_vectArea[m_nAreaEdit].height < m_pAstroImage->m_nHeight )
			m_pAstroImage->m_vectArea[m_nAreaEdit].y = ny;

		Refresh(false);
	} 

	////////////////////////////////////////////
	// AREA RESIZE
	if( m_nAreaEdit >=0 && m_AreaResizeVertex >= 0 && m_bDragging )
	{
		// convert drag diff to image size
		double nDX=0.0, nDY=0.0;

		// calculate mouse position inside picture
		nDX = (double) m_nDragLastX - m_nPicOrigX;
		nDY = (double) m_nDragLastY - m_nPicOrigY;

		// now if i am zoom mode
		if( m_nZoomCount > 0.5 )
		{
			nDX = (nDX * m_nCutWinRatio) + m_nCutX;
			nDY = (nDY * m_nCutWinRatio) + m_nCutY;

		} else
		{
			nDX = nDX/m_nScaleFactor;
			nDY = nDY/m_nScaleFactor;
		}
		// check if position is outside picture adjust
		if( nDX < 0.0 ) nDX = 0.0;
		if( nDX >= m_pAstroImage->m_nWidth ) nDX = m_pAstroImage->m_nWidth-1;
		if( nDY < 0.0 ) nDY = 0.0;
		if( nDY >= m_pAstroImage->m_nHeight ) nDY = m_pAstroImage->m_nHeight-1;

		double ax = m_pAstroImage->m_vectArea[m_nAreaEdit].x;
		double ay = m_pAstroImage->m_vectArea[m_nAreaEdit].y;
		double aw = m_pAstroImage->m_vectArea[m_nAreaEdit].width;
		double ah = m_pAstroImage->m_vectArea[m_nAreaEdit].height;
		double axw = ax + aw;
		double ayh = ay + ah;

		// adjust area size by vertex if
		if( m_AreaResizeVertex == 0 )
		{
			m_pAstroImage->m_vectArea[m_nAreaEdit].x = nDX;
			m_pAstroImage->m_vectArea[m_nAreaEdit].y = nDY;
			m_pAstroImage->m_vectArea[m_nAreaEdit].width = axw - nDX;
			m_pAstroImage->m_vectArea[m_nAreaEdit].height = ayh - nDY;
			
		} else if( m_AreaResizeVertex == 1 )
		{
			m_pAstroImage->m_vectArea[m_nAreaEdit].y = nDY;
			m_pAstroImage->m_vectArea[m_nAreaEdit].width = nDX - ax;
			m_pAstroImage->m_vectArea[m_nAreaEdit].height = ayh - nDY;

		} else if( m_AreaResizeVertex == 2 )
		{
			m_pAstroImage->m_vectArea[m_nAreaEdit].width = nDX - ax;
			m_pAstroImage->m_vectArea[m_nAreaEdit].height = nDY - ay;

		} else if( m_AreaResizeVertex == 3 )
		{
			m_pAstroImage->m_vectArea[m_nAreaEdit].x = nDX;
			m_pAstroImage->m_vectArea[m_nAreaEdit].width = axw - nDX;
			m_pAstroImage->m_vectArea[m_nAreaEdit].height = nDY - ay;
		}

		Refresh(false);
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	AddTarget
// Class:	CAstroImgView
// Purpose:	add a target
// Input:	target x/y, ra/dec, ellipse a/b
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CAstroImgView::AddTarget( double x, double y, double ra, double dec, 
						double a, double b )
{
	// here we deal with the allocation 
	if( !m_vectTarget )
	{
		// first allocation
		m_nTargetAllocated = SKY_TARGET_BLOCK_SIZE_ALLOC;
		m_vectTarget = (StarDef*) malloc( m_nTargetAllocated*sizeof(StarDef) );

	} else if( m_nTarget == m_nTargetAllocated )
	{
		// allocate an extra block
		m_nTargetAllocated += SKY_TARGET_BLOCK_SIZE_ALLOC;
		m_vectTarget = (StarDef*) realloc( m_vectTarget, m_nTargetAllocated*sizeof(StarDef) );
	}

	// set my taget vars
	m_vectTarget[m_nTarget].x = x;
	m_vectTarget[m_nTarget].y = y;
	m_vectTarget[m_nTarget].ra = ra;
	m_vectTarget[m_nTarget].dec = dec;
	m_vectTarget[m_nTarget].a = a;
	m_vectTarget[m_nTarget].b = b;
	m_vectTarget[m_nTarget].kronfactor = 1;

	// increment target counter
	m_nTarget++; 

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DrawCam
// Class:	CAstroImgView
// Purose:	Draw camera target ???
// Input:	reference to dc
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CAstroImgView::DrawCam( wxDC& dc, int bSize, int bUpdate )
{
//	CDynamicImgView::Draw( dc );

	// check for main target
	if( m_bMainTarget )
	{
		double nPointFloatX = m_nTrackX * m_nScaleFactor;
		double nPointFloatY = m_nTrackY * m_nScaleFactor;

		int nPointX = (int) round( m_nPicOrigX + nPointFloatX );
		int nPointY = (int) round( m_nPicOrigY + nPointFloatY );
		// calculate zoom/scale
		if( m_nZoomCount > 0.5 )
		{
			nPointX = (int) round( m_nPicOrigX + (nPointFloatX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
			nPointY = (int) round( m_nPicOrigY + (nPointFloatY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
		}

		dc.SetPen( *wxRED_PEN );
		if( m_bMainTarget == 2 )
		{
			// draw 4 lines
			dc.DrawLine( nPointX, 0, nPointX, (int) round(nPointY)-6 );
			dc.DrawLine( nPointX, (int) round(nPointY)+6, nPointX, (int) round(m_nHeight) );
			dc.DrawLine( 0, nPointY, (int) round(nPointX)-6, nPointY );
			dc.DrawLine( (int) round(nPointX)+6, nPointY, (int) round(m_nWidth), nPointY );
			// aditional circle
			dc.DrawCircle( round(nPointX), round(nPointY), 4 );

		} else
		{
			// draw just two lins
			dc.DrawLine( nPointX, 0, nPointX, (int) round(m_nHeight) );
			dc.DrawLine( 0, nPointY, (int) round(m_nWidth), nPointY );
		}
	}

	// check if location set - THIS TO BE USED LATTER TO DISPLAY ADITIONAL LABEL
/*	if( bIsLocSet )
	{
		wxFont smallFont( 7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT );
		wxSize sizeTxt;
		dc.SetFont( smallFont );

		int x, y;

		double xf = m_nLocX*m_nScaleFactor;
		double yf = m_nLocY*m_nScaleFactor;
		// claculate zoom /scale
		if( m_nZoomCount > 0.5 )
		{
			x = (int) round( m_nPicOrigX + (xf-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
			y = (int) round( m_nPicOrigY + (yf-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
		} else
		{
			x = (int) round( m_nPicOrigX + xf );
			y = (int) round( m_nPicOrigY + yf );
		}

		dc.SetPen( *wxRED_PEN );
		dc.SetBrush( *wxTRANSPARENT_BRUSH );
		dc.DrawCircle( x, y, 4 );

		// set label
		sizeTxt = dc.GetTextExtent( m_strLocName );
		dc.SetTextForeground( *wxRED );
		dc.DrawText( m_strLocName, x+6, y-sizeTxt.GetHeight()-2 );
	}
*/

	return;
}

////////////////////////////////////////////////////////////////////
wxMenu* CAstroImgView::CreatePopupMenu( )
{
	int nObjState=0;

	// build my right click menus
	wxMenu* pImgViewMenu = new wxMenu( wxT(""), 0 );

	////////////////////////////
	// OBJECT FUNCTIONS - IF SELECTED
	if( m_nLastOverObject >= 0 && m_nLastOverObjectType >= 0 )
	{
		if( m_nLastOverObjectType == 1 )
			nObjState = m_pAstroImage->m_vectStar[m_nLastOverObject].state;
		else if( m_nLastOverObjectType == 2 )
			nObjState = m_pAstroImage->m_vectCatStar[m_nLastOverObject].state;
		else if( m_nLastOverObjectType == 3 )
			nObjState = m_pAstroImage->m_vectCatDso[m_nLastOverObject].state;

		if( nObjState & OBJECT_STATE_CUSTOM_SHOW ) 
			pImgViewMenu->Append( wxID_VIEW_HIDE_SHOW_OBJ, wxT("Hide Object") );
		else
			pImgViewMenu->Append( wxID_VIEW_HIDE_SHOW_OBJ, wxT("Show Object") );

		pImgViewMenu->AppendSeparator( );
	}

	/////////////////
	// :: notes
	pImgViewMenu->Append( wxID_VIEW_IMGNOTE, wxT("Add Note") );
	// temporary disable the notes - todo later
//	m_pImgViewMenu->Enable( wxID_IMGNOTES, 0 );

	/////////////////////////
	// :: area submenu
	wxMenu* pAreaSubMenu = NULL;
	if( m_nAreaSelected >=0 )
	{
		pAreaSubMenu = new wxMenu( wxT(""), 0 );
//		pAreaSubMenu->Append( wxID_IMGVIEWADDAREA, wxT("Add Area"));
//		pAreaSubMenu->Append( wxID_IMGVIEWSETAREA, wxT("Set Area"));
		pAreaSubMenu->Append( wxID_IMGVIEWEDITAREA, wxT("Edit"));
		pAreaSubMenu->Append( wxID_IMGVIEWDELAREA, wxT("Delete"));
//		pAreaSubMenu->Append( wxID_IMGVIEWZOOMTOAREA, wxT("Zoom to Area"));
		// temporary disable this options - todo later
//		pAreaSubMenu->Enable( wxID_IMGVIEWADDAREA, 0 );
//		pAreaSubMenu->Enable( wxID_IMGVIEWSETAREA, 0 );
//		pAreaSubMenu->Enable( wxID_IMGVIEWEDITAREA, 0 );
//		pAreaSubMenu->Enable( wxID_IMGVIEWDELAREA, 0 );
//		pAreaSubMenu->Enable( wxID_IMGVIEWZOOMTOAREA, 0 );
		// add area sub menu to view
		pImgViewMenu->AppendSeparator( );
		pImgViewMenu->Append( wxID_IMGAREA, wxT("Area"), pAreaSubMenu );
	}

	////////////////////////
	// view with options
	pImgViewMenu->AppendSeparator( );
	// :: view with submenu
	wxMenu* pViewWithSubMenu = new wxMenu( wxT(""), 0 );
	pViewWithSubMenu->Append( wxID_VIEWWITH_SKYMAPORG, wxT("SKY-MAP.ORG")); 
	pViewWithSubMenu->Append( wxID_VIEWWITH_GOOGLEONLINE, wxT("Google Online"));
	pViewWithSubMenu->Append( wxID_VIEWWITH_GOOGLEEARTH, wxT("Google Earth")); 
	pViewWithSubMenu->Append( wxID_VIEWWITH_WORLDWIND, wxT("World Wind"));
	// temporary disable last two otions - todo:: check if these apps are instaled
	if( !m_pUnimap->m_pAppsMgr->IsApp(MGR_APPS_NAMEID_GOOGLEEARTH) ) pViewWithSubMenu->Enable( wxID_VIEWWITH_GOOGLEEARTH, 0 );
	if( !m_pUnimap->m_pAppsMgr->IsApp(MGR_APPS_NAMEID_WORLDWIND) ) pViewWithSubMenu->Enable( wxID_VIEWWITH_WORLDWIND, 0 );
	// add to view with menu option
	pImgViewMenu->Append( wxID_VIEWWITH, wxT("View With"), pViewWithSubMenu );

	/////////////////////
	// telescope goto in intrumets view if connected
	if( m_nViewType == VIEW_TYPE_INSTRUMENTS && m_pAstroImage->m_bIsMatched &&
		m_pFrame->m_pTelescope && m_pFrame->m_pTelescope->IsConnected() )
	{
		pImgViewMenu->AppendSeparator( );
		pImgViewMenu->Append( wxID_IMGVIEWSLEWTO, wxT("Slew To...") );
	}

	///////////////////
	// add properties
	pImgViewMenu->AppendSeparator( );
	pImgViewMenu->Append( wxID_IMGVIEWPROPERTY, wxT("Properties") );

	return(pImgViewMenu);
}

////////////////////////////////////////////////////////////////////
void CAstroImgView::OnMouseLeave( wxMouseEvent& pEvent )
{
	// if main frame parent and not dragging
	if( m_pFrame && !m_bDragging ) m_pFrame->m_pImageSelect->SetFocus();
}

// AREA :: edit
////////////////////////////////////////////////////////////////////
void CAstroImgView::OnEditArea( wxCommandEvent& pEvent )
{
	m_nAreaEdit = m_nAreaSelected;
}

////////////////////////////////////////////////////////////////////
void CAstroImgView::OnDeleteArea( wxCommandEvent& pEvent )
{
	if( m_nAreaSelected >= 0 )
	{
		m_pAstroImage->m_vectArea.erase( m_pAstroImage->m_vectArea.begin()+m_nAreaSelected );
		Refresh(false);
	}
}

////////////////////////////////////////////////////////////////////
// Purose:	zoom in to coord
////////////////////////////////////////////////////////////////////
int CAstroImgView::ZoomIn( int x, int y, int nFlag )
{
	int nRet = CDynamicImgView::ZoomIn( x, y, nFlag );

	/////
	// set status frame zoom
	m_pFrame->SetStatusZoom( (int) (m_nScaleFactor*m_nLastZoomFactor*100.0) );

	return( nRet );
}

////////////////////////////////////////////////////////////////////
// Purpose:	zoom out in the picture
////////////////////////////////////////////////////////////////////
int CAstroImgView::ZoomOut( int x, int y, int nFlag )
{
	int nRet = CDynamicImgView::ZoomOut( x, y, nFlag );

	/////
	// set status frame zoom
	m_pFrame->SetStatusZoom( (int) (m_nScaleFactor*m_nLastZoomFactor*100.0) );

	return( nRet );
}

////////////////////////////////////////////////////////////////////
void CAstroImgView::OnShowHideObject( wxCommandEvent& pEvent )
{
	if( m_nLastOverObjectType >=0 && m_nLastOverObject >= 0 ) 
	{
		if( m_nLastOverObjectType == 1 )
			m_pAstroImage->m_vectStar[m_nLastOverObject].state ^= OBJECT_STATE_CUSTOM_SHOW;
		else if( m_nLastOverObjectType == 2 )
			m_pAstroImage->m_vectCatStar[m_nLastOverObject].state ^= OBJECT_STATE_CUSTOM_SHOW;
		else if( m_nLastOverObjectType == 3 )
			m_pAstroImage->m_vectCatDso[m_nLastOverObject].state ^= OBJECT_STATE_CUSTOM_SHOW;
	}
}
