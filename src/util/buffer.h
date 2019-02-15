// Buffer.h: interface for the Buffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFFER_H__986A9545_633E_412F_B042_7F857665203B__INCLUDED_)
#define AFX_BUFFER_H__986A9545_633E_412F_B042_7F857665203B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBuffer  
{
public:
	int* m_vectData;
	unsigned int m_nData;
	unsigned int m_nSizeData;
	// check the buffer pointer
	unsigned int m_nFirst;
	unsigned int m_nLast;
	unsigned int m_nCurrent;

public:
	int Pop( int* nWord );
	int Push( int nWord );
	CBuffer( unsigned int nData );
	virtual ~CBuffer();

};

#endif // !defined(AFX_BUFFER_H__986A9545_633E_412F_B042_7F857665203B__INCLUDED_)
