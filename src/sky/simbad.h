////////////////////////////////////////////////////////////////////
// Name:		Simbad header
// File:		simbad.h
// Purpose:		class to work with simbad online
//
// Created by:	Larry Lart on 12/10/2008
// Updated by:	Larry Lart on 30/01/2010
//
////////////////////////////////////////////////////////////////////

#ifndef _SIMBAD_H
#define _SIMBAD_H

//system headers
#include <math.h>
#include <stdio.h>
#include <vector>

// wx
#include "wx/wxprec.h"

// local includes
#include "star.h"

// local classes
class CUnimapWorker;
class CAstroImage;
class CSky;
class CBibDb;

class CSimbad
{
///////////////////
// public data
public:
	CAstroImage*	m_pAstroImage;
	CUnimapWorker*		m_pUnimapWorker;
	CSky*			m_pSky;
	CBibDb*			m_pBibDb;

	// object data
	wxString m_strObjectName;
	wxString m_strObjectType;

	wxString m_strCZ;
	wxString m_strRadialVelocity;
	wxString m_strProperMotions;
	wxString m_strParallax;
	wxString m_strRedshift;
	wxString m_strFluxB;
	wxString m_strFluxV;
	wxString m_strFluxJ;
	wxString m_strFluxH;
	wxString m_strFluxK;
	// spectral
	wxString m_strSpectralType;
	// morphological type
	wxString m_strMorphologicalType;
	// angular size
	wxString m_strAngSize;

	// flags
	int m_bHasDetails;
	int m_bHasIdentifiers;
	int m_bHasNotes;

	// object coordinates from object view dlg
	wxString		m_vectCoordLabel[10];
	wxString		m_vectCoordValue[10];
	int				m_nCoordType;

	// identifiers
	int			m_nIdentifiers;
	wxString	m_vectIdentifiersCat[200];
	wxString	m_vectIdentifiersName[200];

	// measures data
	std::vector<wxString> m_vectMeasuresName;

	int			m_nMeasuresTable;
	int			m_nMeasuresTableCol[20];
	int			m_nMeasuresTableRow[20];
	wxString	m_vectMeasuresTableHead[20][20];
	wxString	m_vectMeasuresTableValues[20][200][20];

	// notes
	wxString		m_strNotesPage;
	
////////////////////
// public methods
public:
	// constructor/destructor
	CSimbad( CAstroImage* pAstroImage, CUnimapWorker* pUnimapWorker, CSky* pSky, CBibDb* pBibDb=NULL );
	~CSimbad( );

	// get basic details for a list of objects
	int GetBasicDetailsForObjList( StarDef* vectObj, unsigned long nObj, StarBasicDetailsDef* vectObjDetails );
	// load simbad object list file
	int LoadSimbadObjectListDataFile( const wxString& strFile, StarDef* vectObj, 
										unsigned long nObjStart, unsigned long nObj,
										StarBasicDetailsDef* vectObjDetails );

	int GetObjectData( wxString& strObjectCatName );
	int LoadSimbadObjectDataFile( const wxString& strFile );
};

#endif
