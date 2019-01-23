
// wx
#include "wx/wxprec.h"
#include <wx/file.h>
#include <wx/string.h>
#include <wx/regex.h>
#include <wx/uri.h>
#include <wx/tokenzr.h>

// local headers
#include "../util/folders.h"
#include "../util/func.h"
#include "../unimap_worker.h"
#include "../sky/ned_site.h"
#include "../sky/nstars_site.h"

// main header
#include "spectrum.h"

////////////////////////////////////////////////////////////////////
CSkyObjSpectrum::CSkyObjSpectrum( CUnimapWorker* pUnimapWorker ) 
{
	m_pUnimapWorker = pUnimapWorker;

	// set other
	m_vectSpectrumDataX = NULL;
	m_vectSpectrumDataY = NULL;
	m_nSpectrumDataSize = 0;

}

////////////////////////////////////////////////////////////////////
CSkyObjSpectrum::~CSkyObjSpectrum( ) 
{
}

////////////////////////////////////////////////////////////////////
// Method:	GetSpectrum
// Class:	CSkyObjSpectrum
// Purpose:	Get object spectrum data 
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
int CSkyObjSpectrum::GetSpectrum( int nObjType, wxString& strObjectCatName ) 
{
		/////////////////
		// now check by type for spectrum
		if( nObjType == 3 )
		{
			// get spectrum from NED
			// pSpectrum = new CObjectSpectrum( obj_id, obj_type );
			// pSpectrum->GetSpectrumFromNED( );
			CNedSite site( this, m_pUnimapWorker );
			site.GetSpectrum( strObjectCatName );

		} else if( nObjType == 1 || nObjType == 2 )
		{
			CNStarsSite site( this, m_pUnimapWorker );
			site.GetSpectrum( strObjectCatName );
		}

		return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	LoadSpectrumFile
// Class:	CSkyObjSpectrum
// Purpose:	load spectrum file from ned
// Input:	remote file path, local file
// Output:	status
////////////////////////////////////////////////////////////////////
int CSkyObjSpectrum::LoadSpectrumFile( const wxString& strLocalFile, 
								   double* vectDataX, double* vectDataY ) 
{
	FILE* pLocalFile = NULL;
	wxString strSpecLocalPathFile;
	wxChar strLine[2000];
	int nCount = 0;
	double nX =0, nY = 0;
	// header regex
	wxRegEx reDataCols( wxT( "^\\ *([0-9e\\.\\+\\-]+)\\ +([0-9e\\.\\+\\-]+)" ), wxRE_ADVANCED  );

	// set spectrum ini file - just to check if exists
	strSpecLocalPathFile.Printf( wxT("%s%s"), OBJECT_SPECTRUM_DATA_PATH, strLocalFile );

	/////////////////////
	// process file header to extract axis
	pLocalFile = wxFopen( strSpecLocalPathFile, wxT("r") );
	if( !pLocalFile ) return( 0 );
	// Reading lines from  file
	while( !feof( pLocalFile ) )
	{
		// clear buffers
//		bzero( strLine, 255 );
		// read a line of max 2000 chars
		wxFgets( strLine, 2000, pLocalFile );

		// if comment line skip
		if( strLine[0] == '#' ) continue;

		// copy in wxstring
		wxString strWxLine = strLine;
		strWxLine.Trim( 1 ).Trim( 0 );

		// check for my pattern
		if( reDataCols.Matches( strWxLine ) )
		{
			// check allocation
			if( nCount >= m_nSpectrumDataSizeAllocated )
			{
				m_nSpectrumDataSizeAllocated += SPECTRUM_DATA_SIZE_INCREMENT_ALLOCATION;
		        vectDataX = (double*)realloc( vectDataX, m_nSpectrumDataSizeAllocated*sizeof(double) );
				vectDataY = (double*)realloc( vectDataY, m_nSpectrumDataSizeAllocated*sizeof(double) );
			}

			if( !reDataCols.GetMatch( strWxLine, 1 ).Trim( 1 ).Trim( 0 ).ToDouble( &nX ) ) nX = 0; 
			if( !reDataCols.GetMatch( strWxLine, 2 ).Trim( 1 ).Trim( 0 ).ToDouble( &nY ) ) nY = 0;
			// se my new data
			vectDataX[nCount] = nX;
			vectDataY[nCount] = nY;
			// increment count
			nCount++;
		}

	}
	// close file
	fclose( pLocalFile );

	return( nCount );
}

////////////////////////////////////////////////////////////////////
// Method:		LoadSpectrumIniFile
// Class:		CSkyObjSpectrum
// Purpose:		load spectrum ini file
// Input:		file name
// Output:		status
////////////////////////////////////////////////////////////////////
int CSkyObjSpectrum::LoadSpectrumIniFile( const wxString& strFile )
{
	FILE* pFile = NULL;
	wxChar strLine[255];
	int bFoundSpectrum = 0;
	int nCurrentSpectrum = 0;
	wxString strTemp = wxT("");

	wxRegEx reSpecLabel = wxT( "^\\[(.+)\\]" );
	wxRegEx reSpecFile = wxT( "^File=(.+)" );
	wxRegEx reSpecRegion = wxT( "^Region=(.+)" );
	wxRegEx reSpecTelescope = wxT( "^Telescope=(.+)" );
	wxRegEx reSpecInstrument = wxT( "^Instrument=(.+)" );
	wxRegEx reSpecBand = wxT( "^Band=(.+)" );
	wxRegEx reSpecLabelX = wxT( "^LabelX=(.+)" );
	wxRegEx reSpecLabelY = wxT( "^LabelY=(.+)" );

	// open file to read
	pFile = wxFopen( strFile, wxT("r") );
	// check if there is any configuration to load
	if( !pFile ) return( -1 );
	// reset counters
	m_nSpectrum = 0;

	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear thy buffers
		bzero( strLine, 255 );

		// get one line out of the config file
		wxFgets( strLine, 2000, pFile );
		//wxString strWxLine(strLine,wxConvUTF8);
		
		// check for image name label
		if( reSpecLabel.Matches( strLine ) )
		{
			// get image name
			wxString strSpecLabel = reSpecLabel.GetMatch( strLine, 1 );
			m_vectSpectrum[m_nSpectrum].strLabel = strSpecLabel;
			// empty the other fields
			m_vectSpectrum[m_nSpectrum].strBand = wxT("");
			m_vectSpectrum[m_nSpectrum].strFileName = wxT("");
			m_vectSpectrum[m_nSpectrum].strInstrument = wxT("");
			m_vectSpectrum[m_nSpectrum].strRegion = wxT("");
			m_vectSpectrum[m_nSpectrum].strTelescope = wxT("");
			m_vectSpectrum[m_nSpectrum].strXLabel = wxT("");
			m_vectSpectrum[m_nSpectrum].strYLabel = wxT("");

			nCurrentSpectrum = m_nSpectrum;
			m_nSpectrum++;
			bFoundSpectrum = 1;

		} else if( reSpecFile.Matches( strLine ) && bFoundSpectrum == 1 )
		{
			m_vectSpectrum[nCurrentSpectrum].strFileName = reSpecFile.GetMatch(strLine, 1 ).Trim(0).Trim(1);

		} else if( reSpecRegion.Matches( strLine ) && bFoundSpectrum == 1 )
		{
			m_vectSpectrum[nCurrentSpectrum].strRegion = reSpecRegion.GetMatch(strLine, 1 ).Trim(0).Trim(1);

		} else if( reSpecTelescope.Matches( strLine ) && bFoundSpectrum == 1 )
		{
			m_vectSpectrum[nCurrentSpectrum].strTelescope = reSpecTelescope.GetMatch(strLine, 1 ).Trim(0).Trim(1);

		} else if( reSpecInstrument.Matches( strLine ) && bFoundSpectrum == 1 )
		{
			m_vectSpectrum[nCurrentSpectrum].strInstrument = reSpecInstrument.GetMatch(strLine, 1 ).Trim(0).Trim(1);

		} else if( reSpecBand.Matches( strLine ) && bFoundSpectrum == 1 )
		{
			m_vectSpectrum[nCurrentSpectrum].strBand = reSpecBand.GetMatch(strLine, 1 ).Trim(0).Trim(1);

		} else if( reSpecLabelX.Matches( strLine ) && bFoundSpectrum == 1 )
		{
			m_vectSpectrum[nCurrentSpectrum].strXLabel = reSpecLabelX.GetMatch(strLine, 1 ).Trim(0).Trim(1);

		} else if( reSpecLabelY.Matches( strLine ) && bFoundSpectrum == 1 )
		{
			m_vectSpectrum[nCurrentSpectrum].strYLabel = reSpecLabelY.GetMatch(strLine, 1 ).Trim(0).Trim(1);
		}
	}

	// close file
	fclose( pFile );

	return( m_nSpectrum );
}
