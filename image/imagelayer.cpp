
#include <stdio.h>
#include <stdlib.h>

// local headers
#include "imagechannel.h"

// main header
#include "imagelayer.h"

// class:: CImageLayer
////////////////////////////////////////////////////////////////////
CImageLayer::CImageLayer( int w, int h )
{
	m_nChannel = 0;
}

void CImageLayer::AddChannel( int w, int h )
{
	m_vectChannel[m_nChannel] = new CImageChannel( w*h );
	m_nChannel++;
}

CImageLayer::~CImageLayer( )
{

}
