
#ifndef _IMAGE_CHANNEL_H
#define _IMAGE_CHANNEL_H

// define image channel types
#define IMAGE_CHANNEL_TYPE_8BIT		0
#define IMAGE_CHANNEL_TYPE_16BIT	1
#define IMAGE_CHANNEL_TYPE_24BIT	2
#define IMAGE_CHANNEL_TYPE_32BIT	3
#define IMAGE_CHANNEL_TYPE_64BIT	4
#define IMAGE_CHANNEL_TYPE_128BIT	5

// an image channel - simple data vector with a resolution
class CImageChannel
{
public:
	CImageChannel( unsigned int nSize );
	~CImageChannel( );

	// a channel could be in different res - 8bit, 16bit, 24bit, 32bit, etc
	unsigned char* m_vectData_8Bit;
	unsigned int* m_vectData_16Bit;
	unsigned long* m_vectData_24Bit;
	float* m_vectData_32Bit;
	double* m_vectData_64Bit;
	long double* m_vectData_128Bit;

	// no of units
	unsigned long m_nData;
	// type
	unsigned char m_nType;

	// conversion methods
	void ConvertTo( unsigned char nType );
	void AllocateChannel( unsigned char nType );
	void FreeChannel( unsigned char nType );
	// local copy by type
	void _COPY_8BIT_TO( unsigned char nType );
	void _COPY_16BIT_TO( unsigned char nType );
	void _COPY_24BIT_TO( unsigned char nType );
	void _COPY_32BIT_TO( unsigned char nType );
	void _COPY_64BIT_TO( unsigned char nType );
	void _COPY_128BIT_TO( unsigned char nType );
	
};

#endif
