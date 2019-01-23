////////////////////////////////////////////////////////////////////
// Package:		Detect Filter implementation
// File:		detectfilter.cpp
// Purpose:		manage filter configuration from to file/argv
//
// Created by:	Larry Lart on 04-May-2008
// Updated by:	
//
// Copyright:	(c) 2008 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

// system headers
#include <stdlib.h>
#include <math.h>
#include <string.h>

// wx
#include "wx/wxprec.h"

// main header
#include "detectfilter.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CDetectFilter
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CDetectFilter::CDetectFilter( const wxString& strFilterName )
{
	int i=0, j=0;

	m_nDetectFilterId = -1;
	// set filter name
	m_strFilterName = strFilterName;
	// reset default values
	m_strFilterType = wxT("");
	m_strFilterMask = wxT("");
	m_strFilterFlag = wxT("");
	m_strFilterDesc = wxT("");
	m_nFilterWidth = 0;
	m_nFilterHeight = 0;
	m_nFilterPixelSize = 0.0;
	// reset data to zero
	for( i=0; i<MAX_FILTER_DATA_ROWS ;i++ )
	{
		for( j=0;j<MAX_FILTER_DATA_COLS ;j++ )
		{
			m_vectData[i][j] = 0;
		}
	}
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CDetectFilter
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CDetectFilter::~CDetectFilter( )
{
	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	AddDataRow
// Class:	CDetectFilter
// Purpose:	add a data row
// Input:	row id, row vect data, no of row column/elements
// Output:	nothing
/////////////////////////////////////////////////////////////////////
int CDetectFilter::AddDataRow( int nRowId, double* vectData, int nCol )
{
	int i=0;

	// set local data
	for( i=0; i<nCol; i++ )
	{
		m_vectData[nRowId][i] = vectData[i];
	}

	return( 1 );
}
