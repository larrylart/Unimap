////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

// main header
#include "imagechannel.h"

// class:: CImageChannel
////////////////////////////////////////////////////////////////////
CImageChannel::CImageChannel( unsigned int nSize )
{
	m_vectData_8Bit = NULL;
	m_vectData_16Bit = NULL;
	m_vectData_24Bit = NULL;
	m_vectData_32Bit = NULL;
	m_vectData_64Bit = NULL;
	m_vectData_128Bit = NULL;

	m_nType = IMAGE_CHANNEL_TYPE_8BIT;
	m_nData = nSize;
	m_vectData_8Bit = (unsigned char*) malloc( m_nData*sizeof(unsigned char) );
}

CImageChannel::~CImageChannel( )
{
	if( m_vectData_8Bit ) free( m_vectData_8Bit );
	if( m_vectData_16Bit ) free( m_vectData_16Bit );
	if( m_vectData_24Bit ) free( m_vectData_24Bit );
	if( m_vectData_32Bit ) free( m_vectData_32Bit );
	if( m_vectData_64Bit ) free( m_vectData_64Bit );
	if( m_vectData_128Bit ) free( m_vectData_128Bit );
}

void CImageChannel::ConvertTo( unsigned char nType )
{ 
	if( m_nType == nType ) return;

	// allocate new channel type
	AllocateChannel( nType );
	// convert all data to new channel type
	if( m_nType = IMAGE_CHANNEL_TYPE_8BIT )
		_COPY_8BIT_TO( nType );
	else if( m_nType = IMAGE_CHANNEL_TYPE_16BIT )
		_COPY_16BIT_TO( nType );
	else if( m_nType = IMAGE_CHANNEL_TYPE_24BIT )
		_COPY_24BIT_TO( nType );
	else if( m_nType = IMAGE_CHANNEL_TYPE_32BIT )
		_COPY_32BIT_TO( nType );
	else if( m_nType = IMAGE_CHANNEL_TYPE_64BIT )
		_COPY_64BIT_TO( nType );
	else if( m_nType = IMAGE_CHANNEL_TYPE_128BIT )
		_COPY_128BIT_TO( nType );
	// free old channel
	FreeChannel( m_nType );
	// set new channel type
	m_nType = nType;
};

void CImageChannel::AllocateChannel( unsigned char nType )
{
	if( nType == IMAGE_CHANNEL_TYPE_8BIT )
		m_vectData_8Bit = (unsigned char*) malloc( m_nData*sizeof(unsigned char) );
	else if( nType == IMAGE_CHANNEL_TYPE_16BIT )
		m_vectData_16Bit = (unsigned int*) malloc( m_nData*sizeof(unsigned int) );
	else if( nType == IMAGE_CHANNEL_TYPE_24BIT )
		m_vectData_24Bit = (unsigned long*) malloc( m_nData*sizeof(unsigned long) );
	else if( nType == IMAGE_CHANNEL_TYPE_32BIT )
		m_vectData_32Bit = (float*) malloc( m_nData*sizeof(float) );
	else if( nType == IMAGE_CHANNEL_TYPE_64BIT )
		m_vectData_64Bit = (double*) malloc( m_nData*sizeof(double) );
	else if( nType == IMAGE_CHANNEL_TYPE_128BIT )
		m_vectData_128Bit = (long double*) malloc( m_nData*sizeof(long double) );
}

void CImageChannel::FreeChannel( unsigned char nType )
{ 
	if( nType == IMAGE_CHANNEL_TYPE_8BIT )
	{
		free( m_vectData_8Bit );
		m_vectData_8Bit = NULL;

	} else if( nType == IMAGE_CHANNEL_TYPE_16BIT )
	{
		free( m_vectData_16Bit );
		m_vectData_16Bit =  NULL;

	} else if( nType == IMAGE_CHANNEL_TYPE_24BIT )
	{
		free( m_vectData_24Bit );
		m_vectData_24Bit = NULL;

	} else if( nType == IMAGE_CHANNEL_TYPE_32BIT )
	{
		free( m_vectData_32Bit );
		m_vectData_32Bit = NULL;

	} else if( nType == IMAGE_CHANNEL_TYPE_64BIT )
	{
		free( m_vectData_64Bit );
		m_vectData_64Bit = NULL;

	} else if( nType == IMAGE_CHANNEL_TYPE_128BIT )
	{
		free( m_vectData_128Bit );
		m_vectData_128Bit = NULL;
	}
}

void CImageChannel::_COPY_8BIT_TO( unsigned char nType )
{
	if( nType == IMAGE_CHANNEL_TYPE_8BIT ) return;

	unsigned char* src_start = m_vectData_8Bit;
	unsigned int* dst_start = m_vectData_16Bit;

	for( unsigned int i=0; i<m_nData; i++ )
	{
		if( nType == IMAGE_CHANNEL_TYPE_16BIT )
			*(dst_start++) = (unsigned int) *(src_start++);
			//m_vectData_16Bit[i] = (unsigned int) m_vectData_8Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_24BIT )
			m_vectData_24Bit[i] = (unsigned long) m_vectData_8Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_32BIT )
			m_vectData_32Bit[i] = (float) m_vectData_8Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_64BIT )
			m_vectData_64Bit[i] = (double) m_vectData_8Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_128BIT )
			m_vectData_128Bit[i] = (long double) m_vectData_8Bit[i];
	}
}

void CImageChannel::_COPY_16BIT_TO( unsigned char nType )
{
	if( nType == IMAGE_CHANNEL_TYPE_16BIT ) return;

	for( unsigned int i=0; i<m_nData; i++ )
	{
		if( nType == IMAGE_CHANNEL_TYPE_8BIT )
			m_vectData_8Bit[i] = (unsigned char) m_vectData_16Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_24BIT )
			m_vectData_24Bit[i] = (unsigned long) m_vectData_16Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_32BIT )
			m_vectData_32Bit[i] = (float) m_vectData_16Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_64BIT )
			m_vectData_64Bit[i] = (double) m_vectData_16Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_128BIT )
			m_vectData_128Bit[i] = (long double) m_vectData_16Bit[i];
	}
}

void CImageChannel::_COPY_24BIT_TO( unsigned char nType )
{
	if( nType == IMAGE_CHANNEL_TYPE_24BIT ) return;

	for( unsigned int i=0; i<m_nData; i++ )
	{
		if( nType == IMAGE_CHANNEL_TYPE_8BIT )
			m_vectData_8Bit[i] = (unsigned char) m_vectData_24Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_16BIT )
			m_vectData_16Bit[i] = (unsigned int) m_vectData_24Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_32BIT )
			m_vectData_32Bit[i] = (float) m_vectData_24Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_64BIT )
			m_vectData_64Bit[i] = (double) m_vectData_24Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_128BIT )
			m_vectData_128Bit[i] = (long double) m_vectData_24Bit[i];
	}

}

void CImageChannel::_COPY_32BIT_TO( unsigned char nType )
{
	if( nType == IMAGE_CHANNEL_TYPE_32BIT ) return;

	for( unsigned int i=0; i<m_nData; i++ )
	{
		if( nType == IMAGE_CHANNEL_TYPE_8BIT )
			m_vectData_8Bit[i] = (unsigned char) m_vectData_32Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_16BIT )
			m_vectData_16Bit[i] = (unsigned int) m_vectData_32Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_24BIT )
			m_vectData_24Bit[i] = (unsigned long) m_vectData_32Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_64BIT )
			m_vectData_64Bit[i] = (double) m_vectData_32Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_128BIT )
			m_vectData_128Bit[i] = (long double) m_vectData_32Bit[i];
	}
}

void CImageChannel::_COPY_64BIT_TO( unsigned char nType )
{
	if( nType == IMAGE_CHANNEL_TYPE_64BIT ) return;

	for( unsigned int i=0; i<m_nData; i++ )
	{
		if( nType == IMAGE_CHANNEL_TYPE_8BIT )
			m_vectData_8Bit[i] = (unsigned char) m_vectData_64Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_16BIT )
			m_vectData_16Bit[i] = (unsigned int) m_vectData_64Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_24BIT )
			m_vectData_24Bit[i] = (unsigned long) m_vectData_64Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_32BIT )
			m_vectData_32Bit[i] = (float) m_vectData_64Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_128BIT )
			m_vectData_128Bit[i] = (long double) m_vectData_64Bit[i];
	}
}

void CImageChannel::_COPY_128BIT_TO( unsigned char nType )
{
	if( nType == IMAGE_CHANNEL_TYPE_128BIT ) return;

	for( unsigned int i=0; i<m_nData; i++ )
	{
		if( nType == IMAGE_CHANNEL_TYPE_8BIT )
			m_vectData_8Bit[i] = (unsigned char) m_vectData_128Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_16BIT )
			m_vectData_16Bit[i] = (unsigned int) m_vectData_128Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_24BIT )
			m_vectData_24Bit[i] = (unsigned long) m_vectData_128Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_32BIT )
			m_vectData_32Bit[i] = (float) m_vectData_128Bit[i];
		else if( nType == IMAGE_CHANNEL_TYPE_64BIT )
			m_vectData_64Bit[i] = (double) m_vectData_128Bit[i];
	}
}

