
#ifndef _SKY_OBJ_SPECTRUM_H
#define _SKY_OBJ_SPECTRUM_H

// wx
#include "wx/wxprec.h"

// defines
#define SPECTRUM_DATA_SIZE_INITIAL_ALLOCATION	2000
#define SPECTRUM_DATA_SIZE_INCREMENT_ALLOCATION	200

// external classes
class CUnimapWorker;

// class:	CSkyObjSpectrum
///////////////////////////////////////////////////////
class CSkyObjSpectrum
{
public:
	CSkyObjSpectrum( CUnimapWorker* pUnimapWorker );
	~CSkyObjSpectrum();

	// handlers
	int GetSpectrum( int nObjType, wxString& strObjectCatName );
	int LoadSpectrumIniFile( const wxString& strFile );
	int LoadSpectrumFile( const wxString& strLocalFile, double* vectDataX, double* vectDataY );

///////////
// DATA
public:

	CUnimapWorker* m_pUnimapWorker;

	// spectrum structure
	typedef struct
	{
		// label
		wxString strLabel;
		// file name
		wxString strFileName;
		// Region
		wxString strRegion;
		// Telescope
		wxString strTelescope;
		// Instrument
		wxString strInstrument;
		// Band
		wxString strBand;
		// x/y axis labels
		wxString strXLabel;
		wxString strYLabel;

	} ObjectSpectrumRef;

	// my vector with spectrum 
	ObjectSpectrumRef	m_vectSpectrum[100];
	int m_nSpectrum;

	// flags
	int m_bHasSpectrum;

	// spectrum data
	double* m_vectSpectrumDataX;
	double* m_vectSpectrumDataY;
	int m_nSpectrumDataSize;
	int m_nSpectrumDataSizeAllocated;

};

#endif