////////////////////////////////////////////////////////////////////
// Package:		Star Catalog definition
// File:		catalog.h
// Purpose:		implement star catalog definition
//
// Created by:	Larry Lart on 11-Nov-2004
// Updated by:	
//
// Copyright:	(c) 2004 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////
#if !defined(_STARCATALOG_H)
#define _STARCATALOG_H

// WX includes
#include "wx/wxprec.h"
#include <wx/regex.h>
#include <wx/string.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#define STAR_CATALOG_DATA_FILE		"sao.txt"
#define STAR_MAP_SIZE				3000

// catalog typedef
typedef struct {
	long sao_no;
	char name[100];
} StarNameDef;

// triangle list typedef
typedef struct triangulo { 
	    short  s1;	/* Object index at vertex between a and b */
	    short  s2;	/* Object index at vertex between a and c */
	    short  s3;	/* Object index at vertex between b and c */
	    float x, y;	/* Triangle space coordinate */
} TRIANGLE;


#include "../util/star.h"

/////////////////////
// external classes
class CLogger;

class CStarCatalog  
{
public:

	CLogger*	m_pLogger;

	double m_nMinX, m_nMinY, m_nMaxX, m_nMaxY;

	StarDef m_vectStar[300000];
//	int* m_vectStar;
	unsigned int m_nStar;
	double m_nMaxMag;
	double m_nMinMag;
	long   m_nMagLimitIdx;
	unsigned int m_nSizeData;
	// check the buffer pointer
	unsigned int m_nFirst;
	unsigned int m_nLast;
	unsigned int m_nCurrent;
	StarNameDef m_vectWord[7000];
	unsigned int m_nWord;

	int	DEBUG;

public:
	CStarCatalog( );
	virtual ~CStarCatalog();
	long Load( const char* strFile );
	long LoadNames( const char* strFile );
	long CStarCatalog::SortByMagnitude( );
	long Connect( double nMag, long t );
	long MatchStars( StarDef* vectStarB, int nStarCount );
	long GetStarNameId( long nSaoNo );

	void ProjectStar( StarDef& star, const double x0, const double y0);
	void StoRect( float& nRA, float& nDec );
	void AltAztoRect( float& nAlt, float& nAz );

	void DrawSky( long nStars );

	void DrawSkyRect( long nStars, long nX, long nY, long nWidth, long nHeight,
							   int icont, int jcont );

	void DrawCut( StarDef* vectStar, int nStarCount, long nWidth, long nHeight,
							   int icont, int jcont );

	int CutWindowFromCatalog( double nX, double nY, double nWidth,
					double nHeight, StarDef* vectStar, int nStarCount );

	// CHARON methods

};

#endif // !defined(AFX_BUFFER_H__986A9545_633E_412F_B042_7F857665203B__INCLUDED_)
