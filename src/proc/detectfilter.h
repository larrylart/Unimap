////////////////////////////////////////////////////////////////////
// Package:		detection filter definition
// File:		detectfilter.h
// Purpose:		manage software/hardware configuration
//
// Created by:	Larry Lart on 04/05/2008
// Updated by:	Larry Lart on 12/01/2010 - update to unicode
//
// Copyright:	(c) 2008-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _DETECT_FILTER_H
#define _DETECT_FILTER_H

// data side defines
#define MAX_FILTER_DATA_ROWS	100
#define	MAX_FILTER_DATA_COLS	100

// CLASS :: CDETECTFILTER
////////////////////////////////////
class CDetectFilter 
{
// methods
public:
	CDetectFilter( const wxString& strFilterName );
	~CDetectFilter( );
	
	int AddDataRow( int nRowId, double* vectData, int nCol );

// data
public:
	wxString	m_strFilterName;
	wxString	m_strFilterType;
	wxString	m_strFilterMask;
	wxString	m_strFilterFlag;
	wxString	m_strFilterDesc;
	int		m_nFilterWidth;
	int		m_nFilterHeight;
	double	m_nFilterPixelSize;
	int		m_nDetectFilterId;
	// 2d vector with data
	double	m_vectData[MAX_FILTER_DATA_ROWS][MAX_FILTER_DATA_COLS];
};

#endif
