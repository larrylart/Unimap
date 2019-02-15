////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

// local headers
#include "imagechannel.h"
#include "imagelayer.h"

// main headers
#include "baseimage.h"

// class:: CImageLayer
////////////////////////////////////////////////////////////////////

CBaseImage::CBaseImage( )
{
	m_nLayer = 0;
}

CBaseImage::CBaseImage( int w, int h )
{
	CBaseImage( );

	m_nWidth = w;
	m_nHeight = h;
	// add main layers
	//AddLayer( );
}

CBaseImage::~CBaseImage( )
{
	for( int i=0; i<m_nLayer; i++ ) 
	{
		delete( m_vectLayer[i] );
		m_vectLayer[i] = NULL;
	}
	m_nLayer = 0;
}


void CBaseImage::AddLayer( )
{
	m_vectLayer[m_nLayer] = new CImageLayer( m_nWidth, m_nHeight );
	m_nLayer++;
}

