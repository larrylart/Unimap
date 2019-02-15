// Buffer.cpp: implementation of the Buffer class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuffer::CBuffer( unsigned int nData )
{
	// allocate the vector size
	m_nSizeData = nData;
	m_vectData = (int*) calloc( m_nSizeData+1, sizeof(int) );
	// initialize the buffer's pointers
	m_nData = 0;
	m_nFirst = 0;
	m_nLast = 0;
	m_nCurrent = 0;
}

CBuffer::~CBuffer()
{
	free( m_vectData );
}

int CBuffer::Push( int nWord )
{
	// check if your circular buffer is full
	if( m_nLast == m_nSizeData )
		m_nLast = 0;
	else
		m_nLast++;

	m_vectData[m_nLast] = nWord;
	//printf( "Add in buffer(%d) = %d\n", m_nLast, nWord );

	// check the size of the buffer
	if( m_nData < m_nSizeData ) m_nData++;

	return( 0 );
}

int CBuffer::Pop( int* nWord )
{
	// check if the buffer is empty
	if( !m_nData ) return( 0 );

	// Get the current element
	*nWord = m_vectData[m_nLast];
	//printf( "Get from buffer(%d) = %d\n", m_nLast, *nWord );

	if( m_nLast == 0 ) 
		m_nLast = m_nSizeData;
	else
		m_nLast--;

	m_nData--;

	return( 1 );
}
