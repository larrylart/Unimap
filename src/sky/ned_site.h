////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _NED_SITE_H
#define _NED_SITE_H

#include "wx/wxprec.h"

// external classes
class CSkyObjSpectrum;
class CUnimapWorker;

// class:	CNedSite
///////////////////////////////////////////////////////
class CNedSite
{
public:
	CNedSite( CSkyObjSpectrum* pSkyObjSpectrum, CUnimapWorker* pUnimapWorker );
	~CNedSite();

	// handlers
	int GetSpectrum( wxString& strObjectCatName );
	int ProcessSpectrumListFileFromNed( const wxString& strFile );
	int GetSpectrumFileFromNed( const wxString& strRemoteFile, const wxString& strLocalFile,
								wxString& strXLabel, wxString& strYLabel );

////////////////
// DATA
public:

	CUnimapWorker* m_pUnimapWorker;
	CSkyObjSpectrum* m_pSkyObjSpectrum;

	wxString m_strObjectCatName;
};

#endif
