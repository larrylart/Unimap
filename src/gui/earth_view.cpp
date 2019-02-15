////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// local headers
#include "../util/folders.h"
#include "../util/func.h"
#include "../config/mainconfig.h"

// main header
#include "earth_view.h"

DEFINE_EVENT_TYPE( wxEVT_CUSTOM_MESSAGE_UPDATE_EVENT )

// class:	CEarthMapView
////////////////////////////////////////////////////////////////////
// implement message map
BEGIN_EVENT_TABLE( CEarthMapView, CDynamicImgView )
	EVT_LEFT_DCLICK( CEarthMapView::OnMouseLDClick )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CEarthMapView
// Purpose:	Initialize my eart location dialog
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CEarthMapView::CEarthMapView(wxWindow *parent, const wxPoint& pos, const wxSize& size, const wxString& strFile, int bLoad ) 
								: CDynamicImgView( parent, pos, size )
{
	m_bMainTarget = 0;
	bIsLocSet = 0;
	m_nLDClickType = 0;
	m_nSelectedMark = -1;

	m_vectMarks.clear();

	// load image from file - if null, load default
	if( strFile == wxT("") ) 
		m_strMapImgFile = unMakeAppPath(wxT("data/earth/earthmap_hires.jpg"));
	else
		m_strMapImgFile = strFile;

	// if set to load immediately(default)
	if( bLoad ) SetImageFromFile( m_strMapImgFile );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CEarthMapView
// Purpose:	destroy my  view
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CEarthMapView::~CEarthMapView( ) 
{
	m_vectMarks.clear();
}

////////////////////////////////////////////////////////////////////
// Method:	Draw
// Class:	CEarthMapView
// Purose:	image canvas drawing
// Input:	reference to dc
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CEarthMapView::Draw( wxDC& dc, int bSize, int bUpdate )
{
	CDynamicImgView::Draw( dc );

	int nPointX=0, nPointY=0, i=0;

	///////////////////////
	// check if any marks
	if( m_vectMarks.size() )
	{
		int nMarkSize = 0;
		if( m_nZoomCount > 5 && m_nZoomCount < 10 )
			nMarkSize = 1;
		else if( m_nZoomCount >= 10 && m_nZoomCount < 15 )
			nMarkSize = 2;
		else if( m_nZoomCount >= 15 )
			nMarkSize = 3;

		int x=0, y=0, nMaxMarksNo=0;
		wxSize sizeTxt;

		///////////////////////////////////
		// :: DRAW BASE MARKS
		for( i=0; i<m_vectMarks.size(); i++ )
		{
			m_vectMarks[i].isHi = 0;
			if( !m_vectMarks[i].is_base  || i == m_nSelectedMark ) continue;

			double xf = m_vectMarks[i].x*m_nScaleFactor;
			double yf = m_vectMarks[i].y*m_nScaleFactor;
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

			// if outside skip
			if( x<0 || y<0 || x>m_nWidth || y>m_nHeight ) continue;

			// if more then no stop 
			if( nMaxMarksNo < 20 )
			{
				// draw pinpoint
				dc.SetPen( wxPen( wxColor(255,255,0) ) );
				dc.SetBrush( *wxTRANSPARENT_BRUSH );
				dc.DrawCircle( x, y, 4 );

				// draw text label
				wxFont smallFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT );
				dc.SetFont( smallFont );

				sizeTxt = dc.GetTextExtent( m_vectMarks[i].name );
				dc.SetTextForeground( wxColor(255,255,0) );
				dc.DrawText( m_vectMarks[i].name, x+6, y-sizeTxt.GetHeight()-2 );

				// set mark as highlited
				m_vectMarks[i].isHi = 1;

				nMaxMarksNo++;

			}

		}

		///////////////////////////////////
		// :: DRAW OTHER MARKS IF LEFT
		for( i=0; i<m_vectMarks.size(); i++ )
		{
			if( m_vectMarks[i].isHi || i == m_nSelectedMark ) continue;

			double xf = m_vectMarks[i].x*m_nScaleFactor;
			double yf = m_vectMarks[i].y*m_nScaleFactor;
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

			// if outside skip
			if( x<0 || y<0 || x>m_nWidth || y>m_nHeight ) continue;

			///////////////////////////
			// draw pinpoint
			dc.SetPen( *wxRED_PEN );
			dc.SetBrush( *wxTRANSPARENT_BRUSH );
			// by type
			if( nMarkSize == 0 )
				dc.DrawPoint( x, y );
			else
				dc.DrawCircle( x, y, nMarkSize );

			// set label - only if zoom level more then 10 ?
			//if( m_nZoomCount > -10 )
			// if more then no stop 
			if( nMaxMarksNo < 20 )
			{
				wxFont smallFont( 7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT );
				dc.SetFont( smallFont );

				sizeTxt = dc.GetTextExtent( m_vectMarks[i].name );
				dc.SetTextForeground( *wxRED );
				dc.DrawText( m_vectMarks[i].name, x+6, y-sizeTxt.GetHeight()-2 );

				nMaxMarksNo++;
			}
		}
	}

	////////////////////////////
	// check for main target
	if( m_bMainTarget )
	{
		double nPointFloatX = m_nMousePicX * m_nScaleFactor;
		double nPointFloatY = m_nMousePicY * m_nScaleFactor;

		nPointX = (int) round( m_nPicOrigX + nPointFloatX );
		nPointY = (int) round( m_nPicOrigY + nPointFloatY );
		// calculate zoom/scale
		if( m_nZoomCount > 0.5 )
		{
			nPointX = (int) round( m_nPicOrigX + (nPointFloatX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
			nPointY = (int) round( m_nPicOrigY + (nPointFloatY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
		}

		dc.SetPen( *wxRED_PEN );
		if( m_bMainTarget == 2 )
		{
			dc.DrawLine( nPointX, 0, nPointX, (int) round(nPointY)-6 );
			dc.DrawLine( nPointX, (int) round(nPointY)+6, nPointX, (int) round(m_nHeight) );
			dc.DrawLine( 0, nPointY, (int) round(nPointX)-6, nPointY );
			dc.DrawLine( (int) round(nPointX)+6, nPointY, (int) round(m_nWidth), nPointY );

		} else
		{
			dc.DrawLine( nPointX, 0, nPointX, (int) round(m_nHeight) );
			dc.DrawLine( 0, nPointY, (int) round(m_nWidth), nPointY );
		}
	}

	///////////////////////////
	// check if location set
	if( bIsLocSet )
	{
		wxFont smallFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT );
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

		dc.SetPen( *wxGREEN_PEN );
		dc.SetBrush( *wxTRANSPARENT_BRUSH );
		dc.DrawCircle( x, y, 4 );

		// set label
		sizeTxt = dc.GetTextExtent( m_strLocName );
		dc.SetTextForeground( *wxGREEN );
		dc.DrawText( m_strLocName, x+6, y-sizeTxt.GetHeight()-2 );
	}

	return;
}

// x/y are the coord in the actual source image
////////////////////////////////////////////////////////////////////
void CEarthMapView::SetZoom( double x, double y )
{
	int nPointX=0, nPointY=0;

	double nPointFloatX = m_nMousePicX * m_nScaleFactor;
	double nPointFloatY = m_nMousePicY * m_nScaleFactor;

	this->ResetZoom();

	nPointX = (int) round( m_nPicOrigX + nPointFloatX );
	nPointY = (int) round( m_nPicOrigY + nPointFloatY );
	// calculate zoom/scale
	if( m_nZoomCount > 0.5 )
	{
		nPointX = (int) round( m_nPicOrigX + (nPointFloatX-m_nCutX*m_nScaleFactor)*m_nLastZoomFactor );
		nPointY = (int) round( m_nPicOrigY + (nPointFloatY-m_nCutY*m_nScaleFactor)*m_nLastZoomFactor );
	}

	// try to zoom in on location
	this->m_nZoomCount = 20;
	this->ZoomIn( nPointX, nPointY, 0 );
}

////////////////////////////////////////////////////////////////////
// Method:	OnMouseLDClick
// Class:	CEarthMapView
// Purose:	action on mouse duble click
// Input:	reference to mouse event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CEarthMapView::OnMouseLDClick( wxMouseEvent& pEvent )
{
	if( !m_bHasImage ) return;

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

	//////////////////////
	// IF TYPE GET CLOSESTS TARGET
	if( m_nLDClickType == 1 ) 
	{
		int nFoundTarget=-1;
		double nDist=0.0, nMinDist=DBL_MAX;

		m_nTargetLat = 90.-m_nMousePicY/m_nOrigPicHeight*180.;
		m_nTargetLon = m_nMousePicX/m_nOrigPicWidth*360.-180.;

		for( int i=0; i<m_vectMarks.size(); i++ )
		{
			// use sky distance - similar
			nDist = CalcSkyDistance( m_nTargetLat, m_nTargetLon, m_vectMarks[i].lat, m_vectMarks[i].lon );
			// look for smallest
			if( nDist < nMinDist )
			{
				nMinDist = nDist;
				nFoundTarget = i;
			}
		}
		// if found
		if( nFoundTarget >= 0 )
		{
			m_nSelectedMark = nFoundTarget;

			double lat = m_vectMarks[nFoundTarget].lat;
			double lon = m_vectMarks[nFoundTarget].lon;
			m_nLocX = (double) ((lon+180.)/360.*m_nOrigPicWidth);
			m_nLocY = (double) ((lat-90.)/-180.*m_nOrigPicHeight);
			m_strLocName = m_vectMarks[nFoundTarget].name;
			bIsLocSet = 1;

			wxCommandEvent event( wxEVT_CUSTOM_MESSAGE_UPDATE_EVENT, wxID_EVENT_UPDATE_OBS_LOCATION );
			event.SetEventObject( NULL );
			// Send it
			wxPostEvent( GetParent(), event );
		}

	} else if( m_pEarth )
	{
		////////////
		// call to locate closest city
		m_nCountryId=-1;
		m_nRegionId=-1;
		m_nCityId=-1;
		m_nTargetLat = 90.-m_nMousePicY/m_nOrigPicHeight*180.;
		m_nTargetLon = m_nMousePicX/m_nOrigPicWidth*360.-180.;
		if( m_pEarth->FindClosestCity( m_nTargetLat, m_nTargetLon, 30.0, m_nCountryId, m_nRegionId, m_nCityId ) )
		{
			double lat = m_pEarth->m_vectCountry[m_nCountryId].vectRegion[m_nRegionId].vectCity[m_nCityId].lat;
			double lon = m_pEarth->m_vectCountry[m_nCountryId].vectRegion[m_nRegionId].vectCity[m_nCityId].lon;
			m_nLocX = (double) ((lon+180.)/360.*m_nOrigPicWidth);
			m_nLocY = (double) ((lat-90.)/-180.*m_nOrigPicHeight);
			m_strLocName = wxString(m_pEarth->m_vectCountry[m_nCountryId].vectRegion[m_nRegionId].vectCity[m_nCityId].name,wxConvUTF8);
			bIsLocSet = 1;

			wxCommandEvent event( wxEVT_CUSTOM_MESSAGE_UPDATE_EVENT, wxID_EVENT_UPDATE_LOCATION );
			event.SetEventObject( NULL );
			// Send it
			wxPostEvent( GetParent(), event );
		}
	}
	// set main target flag
	m_bMainTarget = 1;

	Refresh( false );

	return;
}

////////////////////////////////////////////////////////////////////
void CEarthMapView::AddMark( double lat, double lon, const wxString& name, int is_base, int is_selected )
{
	DefMapMarkPoint elem;

//	if( is_base == 1 )
//		elem.is_base = 2;

	elem.is_base = is_base;
	elem.is_selected = is_selected;

	elem.lat = lat;
	elem.lon = lon;

	elem.x = (double) ((lon+180.)/360.*m_nOrigPicWidth);
	elem.y = (double) ((lat-90.)/-180.*m_nOrigPicHeight);

	elem.name = name;

	m_vectMarks.push_back( elem );
}
