////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _IMAGE_LAYER_H
#define _IMAGE_LAYER_H

class CImageChannel;

// an image layer
class CImageLayer
{
public:
	CImageLayer( int w, int h );
	~CImageLayer( );

	// an image layer might have x channels - which make up colors in different rep
	CImageChannel** m_vectChannel;
	unsigned int m_nChannel;

	/////////////////////
	// COMPLEX CHANNELS - ie RGB/RGBA
	unsigned char* m_vectMixChannels_8Bit;
	unsigned char m_nMixChannels;

	// every layer has his own coordinates
	double x, y;
	unsigned int w, h;

	void AddChannel( int w, int h );
};

#endif
