
#ifndef _BASE_IMAGE_H
#define _BASE_IMAGE_H

class CImageLayer;
class CImageChannel;

// base image
class CBaseImage
{
public:
	CBaseImage( );
	CBaseImage( int w, int h );

	~CBaseImage( );

	// an image might have n layers
	CImageLayer** m_vectLayer;
	unsigned int m_nLayer;

	unsigned int m_nWidth;
	unsigned int m_nHeight;

	void AddLayer( );
};


#endif
