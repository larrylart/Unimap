////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _NSTARS_SITE_H
#define _NSTARS_SITE_H

// wx
#include "wx/wxprec.h"

// external classes
class CSkyObjSpectrum;
class CUnimapWorker;

// class:	CNStarsSite
///////////////////////////////////////////////////////
class CNStarsSite
{
public:
	CNStarsSite( CSkyObjSpectrum* pSkyObjSpectrum, CUnimapWorker* pUnimapWorker );
	~CNStarsSite();

	// handlers
	int GetSpectrum( wxString& strObjectCatName );
	int ProcessSpectrumListFileFromNStars( const wxString& strFile );
	int GetSpectrumFileFromNStars( const wxString& strRemoteFile, const wxString& strLocalFile,
								wxString& strObject, wxString& strObservatory, wxString& strInstrument );

//////////////////
// DATA
public:

	CUnimapWorker* m_pUnimapWorker;
	CSkyObjSpectrum* m_pSkyObjSpectrum;

	wxString m_strObjectCatName;
};

#endif
