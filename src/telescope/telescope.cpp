////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// WX includes
#include "wx/wxprec.h"
#include <wx/regex.h>
#include <wx/tokenzr.h>

// include locals
#include "../observer/observer.h"
// telescope/interfaces types
#include "protocol.h"
#include "ascom.h"
#include "telescope_lx200.h"
#include "telescope_nexstar.h"

// main header
#include "telescope.h"

////////////////////////////////////////////////////////////////////
CTelescope::CTelescope( )
{
	// init hardware objects
	m_nTelescopeDrvType = 0;
	m_pAscom = NULL;
	m_bUseAscom = 1;
	m_pTelescopeDrvLx200 = NULL;
	m_pTelescopeDrvNexStar = NULL;
	// port setup
	m_nSerialPortNo = 1;
	m_nSerialPortBaud = 9600;
	// reset flags
	m_bIsConnected = 0;

	// telescope
	m_vectTelescopeBrandDef.clear();
//	m_nTelescopeBrandDef = 0;
//	m_nTelescopeBrandDefAllocated = 0;
	// barlow lens
	m_vectBarlowLensBrandDef.clear();
//	m_nBarlowLensBrandDef = 0;
//	m_nBarlowLensBrandDefAllocated = 0;
	// focal reducer
	m_vectFocalReducerBrandDef.clear();
//	m_nFocalReducerBrandDef = 0;
//	m_nFocalReducerBrandDefAllocated = 0;
	// eyepiece
	m_vectEyepieceBrandDef.clear();
//	m_nEyepieceBrandDef = 0;
//	m_nEyepieceBrandDefAllocated = 0;
	// mount
	m_vectLMountBrandDef.clear();
//	m_nLMountBrandDef = 0;
//	m_nLMountBrandDefAllocated = 0;
	// focuser
	m_vectFocuserBrandDef.clear();
//	m_nFocuserBrandDef = 0;
//	m_nFocuserBrandDefAllocated = 0;
	// telescope filter
	m_vectTFilterBrandDef.clear();
//	m_nTFilterBrandDef = 0;
//	m_nTFilterBrandDefAllocated = 0;

	// now load configuration
	LoadConfig( );
}

////////////////////////////////////////////////////////////////////
CTelescope::~CTelescope( ) 
{
	// save config
	SaveConfig( );

	if( m_pAscom ) delete( m_pAscom );
	if( m_pTelescopeDrvLx200 ) delete( m_pTelescopeDrvLx200 );
	if( m_pTelescopeDrvNexStar ) delete( m_pTelescopeDrvNexStar );

	UnloadTelescopeBrandsDef( );
	UnloadBarlowLensBrandsDef( );
	UnloadFocalReducerBrandsDef( );
	UnloadEyepieceBrandsDef( );
	UnloadLMountBrandsDef( );
	UnloadFocuserBrandsDef( );
	UnloadTFilterBrandsDef( );
}

// Method:	SetTelescope
////////////////////////////////////////////////////////////////////
int CTelescope::SetTelescope( ) 
{
	// :: Ascom
	if( m_bUseAscom )
	{
		if( !m_pAscom ) m_pAscom = new CAscom( ASCOM_DRV_TYPE_TELESCOPE );
		m_pAscom->m_strDriverId = m_strAscomDriverName;

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		if( !m_pTelescopeDrvLx200 ) m_pTelescopeDrvLx200 = new CTelescopeLx200();
		m_pTelescopeDrvLx200->m_nSerialPortNo = m_nSerialPortNo;
		m_pTelescopeDrvLx200->m_nSerialPortBaud = m_nSerialPortBaud;

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		if( !m_pTelescopeDrvNexStar ) m_pTelescopeDrvNexStar = new CTelescopeNexStar();
		m_pTelescopeDrvNexStar->m_nSerialPortNo = m_nSerialPortNo;
		m_pTelescopeDrvNexStar->m_nSerialPortBaud = m_nSerialPortBaud;
	}

	return( 1 );
}

// Method:	Connect
////////////////////////////////////////////////////////////////////
int CTelescope::Connect( ) 
{
	int bState = 0;

	// set telescope
	if( !SetTelescope( ) ) return( 0 );

	// :: Ascom
	if( m_bUseAscom )
	{
		bState = m_pAscom->Connect( );
		m_bIsConnected = bState;

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		bState = m_pTelescopeDrvLx200->Connect( );
		m_bIsConnected = bState;

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		bState = m_pTelescopeDrvNexStar->Connect( );
		m_bIsConnected = bState;
	}

	return( bState );
}

// Method:	Disconnect
////////////////////////////////////////////////////////////////////
int CTelescope::Disconnect( ) 
{
	int bState = 0;

	// :: Ascom
	if( m_bUseAscom )
	{
		m_pAscom->Disconnect( );
		m_bIsConnected = 0;

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		 m_pTelescopeDrvLx200->Disconnect( );
		m_bIsConnected = 0;

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		m_pTelescopeDrvNexStar->Disconnect( );
		m_bIsConnected = 0;
	}

	return( bState );
}

// Method:	GetPosition
////////////////////////////////////////////////////////////////////
int CTelescope::GetPosition( double& ra, double& dec )
{
	if( !m_bIsConnected ) return( 0 );

	// :: Ascom
	if( m_bUseAscom )
	{
		m_pAscom->GetPosition( ra, dec );

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		// mode: RAW, POLAR, etc - todo: see where i could plug this modes
		m_pTelescopeDrvLx200->GetPosition( &ra, &dec, RAW );

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		m_pTelescopeDrvNexStar->GetPosition( &ra, &dec, RAW );
	}

	// check if there was a change 
	if( m_nCurrentRa != ra || m_nCurrentDec != dec )
	{
		// update locals
		m_nCurrentRa = ra;
		m_nCurrentDec = dec;

	} else
		return( 0 );

	return( 1 );
}

// Method:	GotoPosition
////////////////////////////////////////////////////////////////////
int CTelescope::GotoPosition( double ra, double dec, int bHighPrec )
{
	if( !m_bIsConnected ) return( 0 );

	// :: Ascom
	if( m_bUseAscom )
	{
		m_pAscom->GotoPosition( ra, dec );

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		// mode: RAW, POLAR, etc - todo: see where i could plug this modes
		m_pTelescopeDrvLx200->GoToCoords( ra, dec, RAW );

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		m_pTelescopeDrvNexStar->GoToCoords( ra, dec, RAW );
	}

	// check if there was a change 
	if( m_nCurrentRa != ra || m_nCurrentDec != dec )
	{
		// update locals
		m_nCurrentRa = ra;
		m_nCurrentDec = dec;

	} else
		return( 0 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	StartMovingEast
////////////////////////////////////////////////////////////////////
int CTelescope::StartMovingEast( ) 
{
	int bState = 0;

	// :: Ascom
	if( m_bUseAscom )
	{
		bState = m_pAscom->StartMovingEast( );

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		m_pTelescopeDrvLx200->StartMovingEast( );

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		m_pTelescopeDrvNexStar->StartMovingEast( );
	}

	return( bState );
}

// Method:	StartMovingWest
////////////////////////////////////////////////////////////////////
int CTelescope::StartMovingWest( ) 
{
	int bState = 0;

	// :: Ascom
	if( m_bUseAscom )
	{
		bState = m_pAscom->StartMovingWest();

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		bState = m_pTelescopeDrvLx200->StartMovingWest();

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		bState = m_pTelescopeDrvNexStar->StartMovingWest();
	}

	return( bState );
}

// Method:	StartMovingNorth
////////////////////////////////////////////////////////////////////
int CTelescope::StartMovingNorth( ) 
{
	int bState = 0;

	// :: Ascom
	if( m_bUseAscom )
	{
		bState = m_pAscom->StartMovingNorth();

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		bState = m_pTelescopeDrvLx200->StartMovingNorth();

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		bState = m_pTelescopeDrvNexStar->StartMovingNorth();
	}

	return( bState );
}

// Method:	StartMovingSouth
////////////////////////////////////////////////////////////////////
int CTelescope::StartMovingSouth( ) 
{
	int bState = 0;

	// :: Ascom
	if( m_bUseAscom )
	{
		bState = m_pAscom->StartMovingSouth();

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		bState = m_pTelescopeDrvLx200->StartMovingSouth();

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		bState = m_pTelescopeDrvNexStar->StartMovingSouth();
	}

	return( bState );
}

// Method:	StopSlew
////////////////////////////////////////////////////////////////////
int CTelescope::StopSlew( ) 
{
	int bState = 0;

	// :: Ascom
	if( m_bUseAscom )
	{
		bState = m_pAscom->StopSlew();

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		bState = m_pTelescopeDrvLx200->StopNSEW();

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		bState = m_pTelescopeDrvNexStar->StopSlewAll();
	}

	return( bState );
}

////////////////////////////////////////////////////////////////////
// Method:	SetFocusSpeed
////////////////////////////////////////////////////////////////////
int CTelescope::SetFocusSpeed( long nSpeed ) 
{
	int bState = 0;

	// :: Ascom
	if( m_bUseAscom )
	{
		// we cannot control focus via ascom telescope interface
		bState = 0;

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		bState = m_pTelescopeDrvLx200->SetFocusSpeed( nSpeed );

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		bState = m_pTelescopeDrvNexStar->SetFocusSpeed( nSpeed );
	}

	return( bState );
}

// Method:	StartFocusIn
////////////////////////////////////////////////////////////////////
int CTelescope::StartFocusIn( long nSpeed ) 
{
	int bState = 0;

	// :: Ascom
	if( m_bUseAscom )
	{
		// we cannot control focus via ascom telescope interface
		bState = 0;

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		bState = m_pTelescopeDrvLx200->StartFocusIn( nSpeed );

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		bState = m_pTelescopeDrvNexStar->StartFocusIn( nSpeed );
	}

	return( bState );
}

// Method:	StartFocusOut
////////////////////////////////////////////////////////////////////
int CTelescope::StartFocusOut( long nSpeed ) 
{
	int bState = 0;

	// :: Ascom
	if( m_bUseAscom )
	{
		// we cannot control focus via ascom telescope interface
		bState = 0;

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		bState = m_pTelescopeDrvLx200->StartFocusOut( nSpeed );

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		bState = m_pTelescopeDrvNexStar->StartFocusOut( nSpeed );
	}

	return( bState );
}

// Method:	StopFocus
////////////////////////////////////////////////////////////////////
int CTelescope::StopFocus( ) 
{
	int bState = 0;

	// :: Ascom
	if( m_bUseAscom )
	{
		// we cannot control focus via ascom telescope interface
		bState = 0;

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		bState = m_pTelescopeDrvLx200->StopFocus( );

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		bState = m_pTelescopeDrvNexStar->StopFocus( );
	}

	return( bState );
}

// Method:	FocusIn
////////////////////////////////////////////////////////////////////
int CTelescope::FocusIn( long nUnits ) 
{
	int bState = 0;

	// :: Ascom
	if( m_bUseAscom )
	{
		// we cannot control focus via ascom telescope interface
		bState = 0;

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		bState = m_pTelescopeDrvLx200->FocusIn( nUnits );

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		bState = m_pTelescopeDrvNexStar->FocusIn( nUnits );
	}

	return( bState );
}

// Method:	FocusOut
////////////////////////////////////////////////////////////////////
int CTelescope::FocusOut( long nUnits ) 
{
	int bState = 0;

	// :: Ascom
	if( m_bUseAscom )
	{
		// we cannot control focus via ascom telescope interface
		bState = 0;

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		bState = m_pTelescopeDrvLx200->FocusOut( nUnits );

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		bState = m_pTelescopeDrvNexStar->FocusOut( nUnits );
	}

	return( bState );
}

// Method:	FocusAt
////////////////////////////////////////////////////////////////////
int CTelescope::FocusAt( long nPosition ) 
{
	int bState = 0;

	// :: Ascom
	if( m_bUseAscom )
	{
		// we cannot control focus via ascom telescope interface
		bState = 0;

	// :: Meade lx200
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_MEADE_LX200 )
	{
		bState = m_pTelescopeDrvLx200->FocusAt( nPosition );

	// :: Celestron Nexstar
	} else if( m_nTelescopeDrvType == TELESCOPE_DRV_TYPE_CELESTRON_NEXSTAR )
	{
		bState = m_pTelescopeDrvNexStar->FocusAt( nPosition );
	}

	return( bState );
}

////////////////////////////////////////////////////////////////////
// Method:	SaveConfig
////////////////////////////////////////////////////////////////////
int CTelescope::SaveConfig( )
{
	FILE* pFile = NULL;
//	int i=0;

	// open local file to write
	pFile = wxFopen( DEFAULT_TELESCOPE_INI_FILE, wxT("w") );
	if( !pFile ) return( 0 );

	wxFprintf( pFile, wxT("\n") );

	// SAVE :: Telescope vars
//	wxFprintf( pFile, wxT("[Telescope]\n") );
	wxFprintf( pFile, wxT("UseAscom=%d\n"), m_bUseAscom );
	wxFprintf( pFile, wxT("AscomDriverName=%s\n"), m_strAscomDriverName );
	wxFprintf( pFile, wxT("TelescopeDrvType=%d\n"), m_nTelescopeDrvType );
	wxFprintf( pFile, wxT("SerialPortNo=%d\n"), m_nSerialPortNo );

	// close my file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	LoadConfig
////////////////////////////////////////////////////////////////////
int CTelescope::LoadConfig( )
{
	FILE* pFile = NULL;
	wxChar strLine[500];
	wxChar strTmp[255];
	int nLineSize=0;

	// open file to read
	pFile = wxFopen( DEFAULT_TELESCOPE_INI_FILE, wxT("r") );
	// check if there is any configuration to load
	if( !pFile ) return( -1 );

	// Reading lines from cameras ini file
	while( !feof( pFile ) )
	{
		// get one line out of the config file
		wxFgets( strLine, 500, pFile );
		nLineSize = wxStrlen( strLine );

		// check if to skip because line to short or comment
		if( nLineSize < 4 || strLine[0] == '#' ) continue;

		//////////////////
		// :: use ascom
		if( wxStrncmp( strLine, wxT("UseAscom="), 9 ) == 0 )
		{
			if( !wxSscanf( strLine, wxT("UseAscom=%d\n"), &m_bUseAscom ) ) m_bUseAscom = 0;

		} else if( wxStrncmp( strLine, wxT("AscomDriverName="), 16 ) == 0 )
		{
			wxSscanf( strLine, wxT("AscomDriverName=%[^\n]\n"), strTmp );
			m_strAscomDriverName = strTmp;

		} else if( wxStrncmp( strLine, wxT("TelescopeDrvType="), 17 ) == 0 )
		{
			if( !wxSscanf( strLine, wxT("TelescopeDrvType=%d\n"), &m_nTelescopeDrvType ) ) m_nTelescopeDrvType = 0;

		} else if( wxStrncmp( strLine, wxT("SerialPortNo="), 13 ) == 0 )
		{
			if( !wxSscanf( strLine, wxT("SerialPortNo=%d\n"), &m_nSerialPortNo ) ) m_nSerialPortNo = 0;
		} 
	}
	// close file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	AddTelescopeBrandDef
////////////////////////////////////////////////////////////////////
int CTelescope::AddTelescopeBrandDef( const wxString& strName )
{
	DefTelescopeBrand elem;
	elem.brand_name = strName;
	elem.vectData.clear();

	m_vectTelescopeBrandDef.push_back( elem );

	return( m_vectTelescopeBrandDef.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CTelescope::AddTelescopeDef( int nBrand, const wxString& strName )
{
	DefTelescope elem;
	elem.name = strName;
	elem.brand = m_vectTelescopeBrandDef[nBrand].brand_name;

	m_vectTelescopeBrandDef[nBrand].vectData.push_back( elem );

	return( m_vectTelescopeBrandDef[nBrand].vectData.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CTelescope::IsTelescopeDef( int nBrand, int nName )
{
	if( nBrand < 0 || nBrand >= m_vectTelescopeBrandDef.size() ) return(0);
	if( nName < 0 || nName >= m_vectTelescopeBrandDef[nBrand].vectData.size() ) return(0);

	return( 1 );
}

////////////////////////////////////////////////////////////////////
void CTelescope::UnloadTelescopeBrandsDef( )
{
	int i=0;
	if( m_vectTelescopeBrandDef.size() ) 
	{
		for( i=0; i<m_vectTelescopeBrandDef.size(); i++ ) UnloadTelescopeDef( i );
		//free( m_vectTelescopeBrandDef );
	}
	m_vectTelescopeBrandDef.clear();
//	m_vectTelescopeBrandDef = NULL;
//	m_nTelescopeBrandDef = 0;
//	m_nTelescopeBrandDefAllocated = 0;
}

////////////////////////////////////////////////////////////////////
void CTelescope::UnloadTelescopeDef( int nBrand )
{
	if( m_vectTelescopeBrandDef[nBrand].vectData.size() ) 
		m_vectTelescopeBrandDef[nBrand].vectData.clear();
//	m_vectTelescopeBrandDef[nBrand].vectData = NULL;
//	m_vectTelescopeBrandDef[nBrand].nData = 0;
//	m_vectTelescopeBrandDef[nBrand].nDataAllocated = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	AddBarlowLensBrandDef
////////////////////////////////////////////////////////////////////
int CTelescope::AddBarlowLensBrandDef( const wxString& strName )
{
/*	if( !m_vectBarlowLensBrandDef )
	{
		// do the initial allocation
		m_nBarlowLensBrandDef = 0;
		m_nBarlowLensBrandDefAllocated = 20;
		m_vectBarlowLensBrandDef = (DefBarlowLensBrand*) calloc( m_nBarlowLensBrandDefAllocated, sizeof(DefBarlowLensBrand) );

	} else if( m_nBarlowLensBrandDef >= m_nBarlowLensBrandDefAllocated )
	{
		m_nBarlowLensBrandDefAllocated += 10;
		m_vectBarlowLensBrandDef = (DefBarlowLensBrand*) _recalloc( m_vectBarlowLensBrandDef, 
									m_nBarlowLensBrandDefAllocated, sizeof(DefBarlowLensBrand) );
	}

	m_vectBarlowLensBrandDef[m_nBarlowLensBrandDef].brand_name = wxString(strName,wxConvUTF8);
	m_vectBarlowLensBrandDef[m_nBarlowLensBrandDef].nData = 0;
	m_vectBarlowLensBrandDef[m_nBarlowLensBrandDef].nDataAllocated= 0;
	m_vectBarlowLensBrandDef[m_nBarlowLensBrandDef].vectData = NULL;

	m_nBarlowLensBrandDef++;
*/
	DefBarlowLensBrand elem;
	elem.brand_name = strName;
	elem.vectData.clear();

	m_vectBarlowLensBrandDef.push_back( elem );

	return( m_vectBarlowLensBrandDef.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CTelescope::AddBarlowLensDef( int nBrand, const wxString& strName )
{
/*	if( !m_vectBarlowLensBrandDef[nBrand].vectData )
	{
		m_vectBarlowLensBrandDef[nBrand].nData = 0;
		m_vectBarlowLensBrandDef[nBrand].nDataAllocated = 10;
		m_vectBarlowLensBrandDef[nBrand].vectData= (DefBarlowLens*) calloc( m_vectBarlowLensBrandDef[nBrand].nDataAllocated, sizeof(DefBarlowLens) );

	} else if( m_vectBarlowLensBrandDef[nBrand].nData >= m_vectBarlowLensBrandDef[nBrand].nDataAllocated )
	{
		m_vectBarlowLensBrandDef[nBrand].nDataAllocated += 10;
		m_vectBarlowLensBrandDef[nBrand].vectData = (DefBarlowLens*) _recalloc( m_vectBarlowLensBrandDef[nBrand].vectData, 
															m_vectBarlowLensBrandDef[nBrand].nDataAllocated, sizeof(DefBarlowLens) );
	}
	int nLensId = m_vectBarlowLensBrandDef[nBrand].nData;

	m_vectBarlowLensBrandDef[nBrand].vectData[nLensId].name = wxString(strName,wxConvUTF8);
	// this might need to get a pointer index from the brands structure instead
	m_vectBarlowLensBrandDef[nBrand].vectData[nLensId].brand = wxString(m_vectBarlowLensBrandDef[nBrand].brand_name,wxConvUTF8);
	m_vectBarlowLensBrandDef[nBrand].nData++;
*/
	DefBarlowLens elem;
	elem.name = strName;
	elem.brand = m_vectBarlowLensBrandDef[nBrand].brand_name;

	m_vectBarlowLensBrandDef[nBrand].vectData.push_back( elem );

	return( m_vectBarlowLensBrandDef[nBrand].vectData.size()-1 );
}

////////////////////////////////////////////////////////////////////
void CTelescope::UnloadBarlowLensBrandsDef( )
{
	int i=0;
	if( m_vectBarlowLensBrandDef.size() ) 
	{
		for( i=0; i<m_vectBarlowLensBrandDef.size(); i++ ) UnloadBarlowLensDef( i );
		//free( m_vectBarlowLensBrandDef );
	}
	m_vectBarlowLensBrandDef.clear();
//	m_vectBarlowLensBrandDef = NULL;
//	m_nBarlowLensBrandDef = 0;
//	m_nBarlowLensBrandDefAllocated = 0;
}

////////////////////////////////////////////////////////////////////
void CTelescope::UnloadBarlowLensDef( int nBrand )
{
	if( m_vectBarlowLensBrandDef[nBrand].vectData.size() ) 
		m_vectBarlowLensBrandDef[nBrand].vectData.clear();
//	m_vectBarlowLensBrandDef[nBrand].vectData = NULL;
//	m_vectBarlowLensBrandDef[nBrand].nData = 0;
//	m_vectBarlowLensBrandDef[nBrand].nDataAllocated = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	AddFocalReducerBrandDef
////////////////////////////////////////////////////////////////////
int CTelescope::AddFocalReducerBrandDef( const wxString& strName )
{
/*	if( !m_vectFocalReducerBrandDef )
	{
		// do the initial allocation
		m_nFocalReducerBrandDef = 0;
		m_nFocalReducerBrandDefAllocated = 20;
		m_vectFocalReducerBrandDef = (DefFocalReducerBrand*) calloc( m_nFocalReducerBrandDefAllocated, sizeof(DefFocalReducerBrand) );

	} else if( m_nFocalReducerBrandDef >= m_nFocalReducerBrandDefAllocated )
	{
		m_nFocalReducerBrandDefAllocated += 10;
		m_vectFocalReducerBrandDef = (DefFocalReducerBrand*) _recalloc( m_vectFocalReducerBrandDef, 
									m_nFocalReducerBrandDefAllocated, sizeof(DefFocalReducerBrand) );
	}

	m_vectFocalReducerBrandDef[m_nFocalReducerBrandDef].brand_name = wxString(strName,wxConvUTF8);
	m_vectFocalReducerBrandDef[m_nFocalReducerBrandDef].nData = 0;
	m_vectFocalReducerBrandDef[m_nFocalReducerBrandDef].nDataAllocated= 0;
	m_vectFocalReducerBrandDef[m_nFocalReducerBrandDef].vectData = NULL;

	m_nFocalReducerBrandDef++;
*/
	DefFocalReducerBrand elem;
	elem.brand_name = strName;
	elem.vectData.clear();
	
	m_vectFocalReducerBrandDef.push_back( elem );

	return( m_vectFocalReducerBrandDef.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CTelescope::AddFocalReducerDef( int nBrand, const wxString& strName )
{
/*	if( !m_vectFocalReducerBrandDef[nBrand].vectData )
	{
		m_vectFocalReducerBrandDef[nBrand].nData = 0;
		m_vectFocalReducerBrandDef[nBrand].nDataAllocated = 10;
		m_vectFocalReducerBrandDef[nBrand].vectData = (DefFocalReducer*) calloc( m_vectFocalReducerBrandDef[nBrand].nDataAllocated, sizeof(DefFocalReducer) );

	} else if( m_vectFocalReducerBrandDef[nBrand].nData >= m_vectFocalReducerBrandDef[nBrand].nDataAllocated )
	{
		m_vectFocalReducerBrandDef[nBrand].nDataAllocated += 10;
		m_vectFocalReducerBrandDef[nBrand].vectData = (DefFocalReducer*) _recalloc( m_vectFocalReducerBrandDef[nBrand].vectData, 
															m_vectFocalReducerBrandDef[nBrand].nDataAllocated, sizeof(DefFocalReducer) );
	}
	int nFRedId = m_vectFocalReducerBrandDef[nBrand].nData;

	m_vectFocalReducerBrandDef[nBrand].vectData[nFRedId].name = wxString(strName,wxConvUTF8);
	// this might need to get a pointer index from the brands structure instead
	m_vectFocalReducerBrandDef[nBrand].vectData[nFRedId].brand = m_vectFocalReducerBrandDef[nBrand].brand_name;
	m_vectFocalReducerBrandDef[nBrand].nData++;
*/
	DefFocalReducer elem;
	elem.name = strName;
	elem.brand = m_vectFocalReducerBrandDef[nBrand].brand_name;

	m_vectFocalReducerBrandDef[nBrand].vectData.push_back( elem );

	return( m_vectFocalReducerBrandDef[nBrand].vectData.size()-1 );
}

////////////////////////////////////////////////////////////////////
void CTelescope::UnloadFocalReducerBrandsDef( )
{
	int i=0;
	if( m_vectFocalReducerBrandDef.size() ) 
	{
		for( i=0; i<m_vectFocalReducerBrandDef.size(); i++ ) UnloadFocalReducerDef( i );
		//free( m_vectFocalReducerBrandDef );
	}
	m_vectFocalReducerBrandDef.clear();
//	m_vectFocalReducerBrandDef = NULL;
//	m_nFocalReducerBrandDef = 0;
//	m_nFocalReducerBrandDefAllocated = 0;
}

////////////////////////////////////////////////////////////////////
void CTelescope::UnloadFocalReducerDef( int nBrand )
{
	if( m_vectFocalReducerBrandDef[nBrand].vectData.size() ) 
		m_vectFocalReducerBrandDef[nBrand].vectData.clear();
//	m_vectFocalReducerBrandDef[nBrand].vectData = NULL;
//	m_vectFocalReducerBrandDef[nBrand].nData = 0;
//	m_vectFocalReducerBrandDef[nBrand].nDataAllocated = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	AddEyepieceBrandDef
////////////////////////////////////////////////////////////////////
int CTelescope::AddEyepieceBrandDef( const wxString& strName )
{
	DefEyepieceBrand elem;
	elem.brand_name = strName;
	elem.vectData.clear();

	m_vectEyepieceBrandDef.push_back( elem );

	return( m_vectEyepieceBrandDef.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CTelescope::AddEyepieceDef( int nBrand, const wxString& strName )
{
	DefEyepiece elem;
	elem.name = strName;
	elem.brand = m_vectEyepieceBrandDef[nBrand].brand_name;

	m_vectEyepieceBrandDef[nBrand].vectData.push_back( elem );

	return( m_vectEyepieceBrandDef[nBrand].vectData.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CTelescope::IsEyepieceDef( int nBrand, int nName )
{
	if( nBrand < 0 || nBrand >= m_vectEyepieceBrandDef.size() ) return(0);
	if( nName < 0 || nName >= m_vectEyepieceBrandDef[nBrand].vectData.size() ) return(0);

	return( 1 );
}

////////////////////////////////////////////////////////////////////
void CTelescope::UnloadEyepieceBrandsDef( )
{
	int i=0;
	if( m_vectEyepieceBrandDef.size() ) 
	{
		for( i=0; i<m_vectEyepieceBrandDef.size(); i++ ) UnloadEyepieceDef( i );
		//free( m_vectEyepieceBrandDef );
	}
	m_vectEyepieceBrandDef.clear();
//	m_vectEyepieceBrandDef = NULL;
//	m_nEyepieceBrandDef = 0;
//	m_nEyepieceBrandDefAllocated = 0;
}

////////////////////////////////////////////////////////////////////
void CTelescope::UnloadEyepieceDef( int nBrand )
{
	if( m_vectEyepieceBrandDef[nBrand].vectData.size() ) 
		m_vectEyepieceBrandDef[nBrand].vectData.clear();
//	m_vectEyepieceBrandDef[nBrand].vectData = NULL;
//	m_vectEyepieceBrandDef[nBrand].nData = 0;
//	m_vectEyepieceBrandDef[nBrand].nDataAllocated = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	AddLMountBrandDef
////////////////////////////////////////////////////////////////////
int CTelescope::AddLMountBrandDef( const wxString& strName )
{
/*	if( !m_vectLMountBrandDef )
	{
		// do the initial allocation
		m_nLMountBrandDef = 0;
		m_nLMountBrandDefAllocated = 20;
		m_vectLMountBrandDef = (DefLMountBrand*) calloc( m_nLMountBrandDefAllocated, sizeof(DefLMountBrand) );

	} else if( m_nLMountBrandDef >= m_nLMountBrandDefAllocated )
	{
		m_nLMountBrandDefAllocated += 10;
		m_vectLMountBrandDef = (DefLMountBrand*) _recalloc( m_vectLMountBrandDef, 
									m_nLMountBrandDefAllocated, sizeof(DefLMountBrand) );
	}

	m_vectLMountBrandDef[m_nLMountBrandDef].brand_name = wxString(strName,wxConvUTF8);
	m_vectLMountBrandDef[m_nLMountBrandDef].nData = 0;
	m_vectLMountBrandDef[m_nLMountBrandDef].nDataAllocated = 0;
	m_vectLMountBrandDef[m_nLMountBrandDef].vectData = NULL;

	m_nLMountBrandDef++;
*/
	DefLMountBrand elem;
	elem.brand_name = strName;
	elem.vectData.clear();

	m_vectLMountBrandDef.push_back( elem );
	
	return( m_vectLMountBrandDef.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CTelescope::AddLMountDef( int nBrand, const wxString& strName )
{
/*	if( !m_vectLMountBrandDef[nBrand].vectData )
	{
		m_vectLMountBrandDef[nBrand].nData = 0;
		m_vectLMountBrandDef[nBrand].nDataAllocated = 50;
		m_vectLMountBrandDef[nBrand].vectData = (DefLMount*) calloc( m_vectLMountBrandDef[nBrand].nDataAllocated, sizeof(DefLMount) );

	} else if( m_vectLMountBrandDef[nBrand].nData >= m_vectLMountBrandDef[nBrand].nDataAllocated )
	{
		m_vectLMountBrandDef[nBrand].nDataAllocated += 10;
		m_vectLMountBrandDef[nBrand].vectData = (DefLMount*) _recalloc( m_vectLMountBrandDef[nBrand].vectData, 
															m_vectLMountBrandDef[nBrand].nDataAllocated, sizeof(DefLMount) );
	}
	int nMountId = m_vectLMountBrandDef[nBrand].nData;

	m_vectLMountBrandDef[nBrand].vectData[nMountId].name = wxString(strName,wxConvUTF8);
	// this might need to get a pointer index from the brands structure instead
	m_vectLMountBrandDef[nBrand].vectData[nMountId].brand = m_vectLMountBrandDef[nBrand].brand_name;
	m_vectLMountBrandDef[nBrand].nData++;
*/
	DefLMount elem;
	elem.name = strName;
	elem.brand = m_vectLMountBrandDef[nBrand].brand_name;

	m_vectLMountBrandDef[nBrand].vectData.push_back( elem );

	return( m_vectLMountBrandDef[nBrand].vectData.size()-1 );
}

////////////////////////////////////////////////////////////////////
void CTelescope::UnloadLMountBrandsDef( )
{
	int i=0;
	if( m_vectLMountBrandDef.size() ) 
	{
		for( i=0; i<m_vectLMountBrandDef.size(); i++ ) UnloadLMountDef( i );
		//free( m_vectLMountBrandDef );
	}
	m_vectLMountBrandDef.clear();
//	m_vectLMountBrandDef = NULL;
//	m_nLMountBrandDef = 0;
//	m_nLMountBrandDefAllocated = 0;
}

////////////////////////////////////////////////////////////////////
void CTelescope::UnloadLMountDef( int nBrand )
{
	if( m_vectLMountBrandDef[nBrand].vectData.size() ) 
		m_vectLMountBrandDef[nBrand].vectData.clear();
//	m_vectLMountBrandDef[nBrand].vectData = NULL;
//	m_vectLMountBrandDef[nBrand].nData = 0;
//	m_vectLMountBrandDef[nBrand].nDataAllocated = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	AddFocuserBrandDef
////////////////////////////////////////////////////////////////////
int CTelescope::AddFocuserBrandDef( const wxString& strName )
{
/*	if( !m_vectFocuserBrandDef )
	{
		// do the initial allocation
		m_nFocuserBrandDef = 0;
		m_nFocuserBrandDefAllocated = 20;
		m_vectFocuserBrandDef = (DefFocuserBrand*) calloc( m_nFocuserBrandDefAllocated, sizeof(DefFocuserBrand) );

	} else if( m_nFocuserBrandDef >= m_nFocuserBrandDefAllocated )
	{
		m_nFocuserBrandDefAllocated += 10;
		m_vectFocuserBrandDef = (DefFocuserBrand*) _recalloc( m_vectFocuserBrandDef, 
									m_nFocuserBrandDefAllocated, sizeof(DefFocuserBrand) );
	}

	m_vectFocuserBrandDef[m_nFocuserBrandDef].brand_name = wxString(strName,wxConvUTF8);
	m_vectFocuserBrandDef[m_nFocuserBrandDef].nData = 0;
	m_vectFocuserBrandDef[m_nFocuserBrandDef].nDataAllocated = 0;
	m_vectFocuserBrandDef[m_nFocuserBrandDef].vectData = NULL;

	m_nFocuserBrandDef++;
*/
	DefFocuserBrand elem;
	elem.brand_name = strName;
	elem.vectData.clear();

	m_vectFocuserBrandDef.push_back( elem );

	return( m_vectFocuserBrandDef.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CTelescope::AddFocuserDef( int nBrand, const wxString& strName )
{
/*	if( !m_vectFocuserBrandDef[nBrand].vectData )
	{
		m_vectFocuserBrandDef[nBrand].nData = 0;
		m_vectFocuserBrandDef[nBrand].nDataAllocated = 50;
		m_vectFocuserBrandDef[nBrand].vectData = (DefFocuser*) calloc( m_vectFocuserBrandDef[nBrand].nDataAllocated, sizeof(DefFocuser) );

	} else if( m_vectFocuserBrandDef[nBrand].nData >= m_vectFocuserBrandDef[nBrand].nDataAllocated )
	{
		m_vectFocuserBrandDef[nBrand].nDataAllocated += 10;
		m_vectFocuserBrandDef[nBrand].vectData = (DefFocuser*) _recalloc( m_vectFocuserBrandDef[nBrand].vectData, 
															m_vectFocuserBrandDef[nBrand].nDataAllocated, sizeof(DefFocuser) );
	}
	int nFocuser = m_vectFocuserBrandDef[nBrand].nData;

	m_vectFocuserBrandDef[nBrand].vectData[nFocuser].name = wxString(strName,wxConvUTF8);
	// this might need to get a pointer index from the brands structure instead
	m_vectFocuserBrandDef[nBrand].vectData[nFocuser].brand = m_vectFocuserBrandDef[nBrand].brand_name;
	m_vectFocuserBrandDef[nBrand].nData++;
*/
	DefFocuser elem;
	elem.name = strName;
	elem.brand = m_vectFocuserBrandDef[nBrand].brand_name;

	m_vectFocuserBrandDef[nBrand].vectData.push_back( elem );

	return( m_vectFocuserBrandDef[nBrand].vectData.size()-1 );
}

////////////////////////////////////////////////////////////////////
void CTelescope::UnloadFocuserBrandsDef( )
{
	int i=0;
	if( m_vectFocuserBrandDef.size() ) 
	{
		for( i=0; i<m_vectFocuserBrandDef.size(); i++ ) UnloadFocuserDef( i );
		//free( m_vectFocuserBrandDef );
	}
	m_vectFocuserBrandDef.clear();
//	m_vectFocuserBrandDef = NULL;
//	m_nFocuserBrandDef = 0;
//	m_nFocuserBrandDefAllocated = 0;
}

////////////////////////////////////////////////////////////////////
void CTelescope::UnloadFocuserDef( int nBrand )
{
	if( m_vectFocuserBrandDef[nBrand].vectData.size() ) 
		m_vectFocuserBrandDef[nBrand].vectData.clear();
//	m_vectFocuserBrandDef[nBrand].vectData = NULL;
//	m_vectFocuserBrandDef[nBrand].nData = 0;
//	m_vectFocuserBrandDef[nBrand].nDataAllocated = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	AddTFilterBrandDef
////////////////////////////////////////////////////////////////////
int CTelescope::AddTFilterBrandDef( const wxString& strName )
{
/*	if( !m_vectTFilterBrandDef )
	{
		// do the initial allocation
		m_nTFilterBrandDef = 0;
		m_nTFilterBrandDefAllocated = 20;
		m_vectTFilterBrandDef = (DefTFilterBrand*) calloc( m_nTFilterBrandDefAllocated, sizeof(DefTFilterBrand) );

	} else if( m_nTFilterBrandDef >= m_nTFilterBrandDefAllocated )
	{
		m_nTFilterBrandDefAllocated += 10;
		m_vectTFilterBrandDef = (DefTFilterBrand*) _recalloc( m_vectTFilterBrandDef, 
									m_nTFilterBrandDefAllocated, sizeof(DefTFilterBrand) );
	}

	m_vectTFilterBrandDef[m_nTFilterBrandDef].brand_name = wxString(strName,wxConvUTF8);
	m_vectTFilterBrandDef[m_nTFilterBrandDef].nData = 0;
	m_vectTFilterBrandDef[m_nTFilterBrandDef].nDataAllocated = 0;
	m_vectTFilterBrandDef[m_nTFilterBrandDef].vectData = NULL;

	m_nTFilterBrandDef++;
*/
	DefTFilterBrand elem;
	elem.brand_name = strName;
	elem.vectData.clear();

	m_vectTFilterBrandDef.push_back( elem );

	return( m_vectTFilterBrandDef.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CTelescope::AddTFilterDef( int nBrand, const wxString& strName )
{
/*	if( !m_vectTFilterBrandDef[nBrand].vectData )
	{
		m_vectTFilterBrandDef[nBrand].nData = 0;
		m_vectTFilterBrandDef[nBrand].nDataAllocated = 50;
		m_vectTFilterBrandDef[nBrand].vectData = (DefTFilter*) calloc( m_vectTFilterBrandDef[nBrand].nDataAllocated, sizeof(DefTFilter) );

	} else if( m_vectTFilterBrandDef[nBrand].nData >= m_vectTFilterBrandDef[nBrand].nDataAllocated )
	{
		m_vectTFilterBrandDef[nBrand].nDataAllocated += 10;
		m_vectTFilterBrandDef[nBrand].vectData = (DefTFilter*) _recalloc( m_vectTFilterBrandDef[nBrand].vectData, 
															m_vectTFilterBrandDef[nBrand].nDataAllocated, sizeof(DefTFilter) );
	}
	int nTFilter = m_vectTFilterBrandDef[nBrand].nData;

	m_vectTFilterBrandDef[nBrand].vectData[nTFilter].name = wxString(strName,wxConvUTF8);
	// this might need to get a pointer index from the brands structure instead
	m_vectTFilterBrandDef[nBrand].vectData[nTFilter].brand = m_vectTFilterBrandDef[nBrand].brand_name;
	m_vectTFilterBrandDef[nBrand].nData++;
*/
	DefTFilter elem;
	elem.name = strName;
	elem.brand = m_vectTFilterBrandDef[nBrand].brand_name;

	m_vectTFilterBrandDef[nBrand].vectData.push_back( elem );

	return( m_vectTFilterBrandDef[nBrand].vectData.size()-1 );
}

////////////////////////////////////////////////////////////////////
void CTelescope::UnloadTFilterBrandsDef( )
{
	int i=0;
	if( m_vectTFilterBrandDef.size() ) 
	{
		for( i=0; i<m_vectTFilterBrandDef.size(); i++ ) UnloadTFilterDef( i );
		//free( m_vectTFilterBrandDef );
	}
	m_vectTFilterBrandDef.clear();
//	m_vectTFilterBrandDef = NULL;
//	m_nTFilterBrandDef = 0;
//	m_nTFilterBrandDefAllocated = 0;
}

////////////////////////////////////////////////////////////////////
void CTelescope::UnloadTFilterDef( int nBrand )
{
	if( m_vectTFilterBrandDef[nBrand].vectData.size() ) 
		m_vectTFilterBrandDef[nBrand].vectData.clear();
//	m_vectTFilterBrandDef[nBrand].vectData = NULL;
//	m_vectTFilterBrandDef[nBrand].nData = 0;
//	m_vectTFilterBrandDef[nBrand].nDataAllocated = 0;
}

////////////////////////////////////////////////////////////////////
// Method:	LoadHardwareDef
////////////////////////////////////////////////////////////////////
int CTelescope::LoadHardwareDef( int nHType, int nType, int nBrand )
{
	// check if already loaded
	if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE && m_vectTelescopeBrandDef.size() ) return( 0 );
	if( nHType == TELESCOPE_HARDWARE_BARLOW_LENS && m_vectBarlowLensBrandDef.size() ) return( 0 );
	if( nHType == TELESCOPE_HARDWARE_FOCAL_REDUCER && m_vectFocalReducerBrandDef.size() ) return( 0 );
	if( nHType == TELESCOPE_HARDWARE_EYEPIECE && m_vectEyepieceBrandDef.size() ) return( 0 );
	if( nHType == TELESCOPE_HARDWARE_LMOUNT && m_vectLMountBrandDef.size() ) return( 0 );
	if( nHType == TELESCOPE_HARDWARE_FOCUSER && m_vectFocuserBrandDef.size() ) return( 0 );
	if( nHType == TELESCOPE_HARDWARE_TFILTER && m_vectTFilterBrandDef.size() ) return( 0 );

	FILE* pFile = NULL;
	wxString strFile;
	wxChar strLine[500];
	wxChar strTmp[500];
	int nLineSize=0, bFoundSection=0, i=0, nBrandCount=0;
	// regex :: brands.ini - all
//	wxRegEx reSectionLabel = wxT( "^\\[(.+)\\]" );
//	wxRegEx reBrandDefIni = wxT( "^IniFile=(.*)" );

	/////////////////////////
	// FREE ALLOACTION/BUILD FILE NAME BY TYPE
	if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE )
	{
		UnloadTelescopeBrandsDef();
		strFile.Printf( wxT("%s"), DEFAULT_TELESCOPE_BRAND_DEF_FILE );

	} else if( nHType == TELESCOPE_HARDWARE_BARLOW_LENS )
	{
		UnloadBarlowLensBrandsDef();
		strFile.Printf( wxT("%s"), DEFAULT_BARLOW_LENS_BRAND_DEF_FILE );

	} else if( nHType == TELESCOPE_HARDWARE_FOCAL_REDUCER )
	{
		UnloadFocalReducerBrandsDef();
		strFile.Printf( wxT("%s"), DEFAULT_FOCAL_REDUCER_BRAND_DEF_FILE );

	} else if( nHType == TELESCOPE_HARDWARE_EYEPIECE )
	{
		UnloadEyepieceBrandsDef();
		strFile.Printf( wxT("%s"), DEFAULT_EYEPIECE_BRAND_DEF_FILE );

	} else if( nHType == TELESCOPE_HARDWARE_LMOUNT )
	{
		UnloadLMountBrandsDef();
		strFile.Printf( wxT("%s"), DEFAULT_LMOUNTS_BRAND_DEF_FILE );

	} else if( nHType == TELESCOPE_HARDWARE_FOCUSER )
	{
		UnloadFocuserBrandsDef();
		strFile.Printf( wxT("%s"), DEFAULT_FOCUSER_BRAND_DEF_FILE );

	} else if( nHType == TELESCOPE_HARDWARE_TFILTER )
	{
		UnloadTFilterBrandsDef();
		strFile.Printf( wxT("%s"), DEFAULT_TFILTER_BRAND_DEF_FILE );
	}

	// open file to read
	pFile = wxFopen( strFile, wxT("r") );
	// check if there is any configuration to load
	if( !pFile ) return( -1 );

	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// get one line out of the config file
		wxFgets( strLine, 500, pFile );
		nLineSize = wxStrlen( strLine );

		// check if to skip because line to short or comment
		if( nLineSize < 4 || strLine[0] == '#' ) continue;

		// check section
		//if( reSectionLabel.Matches( strLine ) )
		if( strLine[0] == '[' && strLine[nLineSize-2] == ']' )
		{
			bFoundSection = 1;
//			wxString strName = reSectionLabel.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			wxChar strName[255];
			wxSscanf( strLine, wxT("\[%[^\]]\]\n"), strName );

			// add brand by type
			if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE )
				nBrandCount = AddTelescopeBrandDef( strName );
			else if( nHType == TELESCOPE_HARDWARE_BARLOW_LENS )
				nBrandCount = AddBarlowLensBrandDef( strName );
			else if( nHType == TELESCOPE_HARDWARE_FOCAL_REDUCER )
				nBrandCount = AddFocalReducerBrandDef( strName );
			else if( nHType == TELESCOPE_HARDWARE_EYEPIECE )
				nBrandCount = AddEyepieceBrandDef( strName );
			else if( nHType == TELESCOPE_HARDWARE_LMOUNT )
				nBrandCount = AddLMountBrandDef( strName );
			else if( nHType == TELESCOPE_HARDWARE_FOCUSER )
				nBrandCount = AddFocuserBrandDef( strName );
			else if( nHType == TELESCOPE_HARDWARE_TFILTER )
				nBrandCount = AddTFilterBrandDef( strName );

		} else if( bFoundSection && wxStrncmp( strLine, wxT("IniFile="), 8 ) == 0 )
		{
			wxSscanf( strLine, wxT("IniFile=%[^\n]\n"), strTmp );

			// set by hardware type
			if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE )
				m_vectTelescopeBrandDef[nBrandCount].brand_ini_file = wxString(strTmp,wxConvUTF8);
			else if( nHType == TELESCOPE_HARDWARE_BARLOW_LENS )
				m_vectBarlowLensBrandDef[nBrandCount].brand_ini_file = wxString(strTmp,wxConvUTF8);
			else if( nHType == TELESCOPE_HARDWARE_FOCAL_REDUCER )
				m_vectFocalReducerBrandDef[nBrandCount].brand_ini_file = wxString(strTmp,wxConvUTF8);
			else if( nHType == TELESCOPE_HARDWARE_EYEPIECE )
				m_vectEyepieceBrandDef[nBrandCount].brand_ini_file = wxString(strTmp,wxConvUTF8);
			else if( nHType == TELESCOPE_HARDWARE_LMOUNT )
				m_vectLMountBrandDef[nBrandCount].brand_ini_file = wxString(strTmp,wxConvUTF8);
			else if( nHType == TELESCOPE_HARDWARE_FOCUSER )
				m_vectFocuserBrandDef[nBrandCount].brand_ini_file = wxString(strTmp,wxConvUTF8);
			else if( nHType == TELESCOPE_HARDWARE_TFILTER )
				m_vectTFilterBrandDef[nBrandCount].brand_ini_file = wxString(strTmp,wxConvUTF8);

		} else if( bFoundSection && wxStrncmp( strLine, wxT("Types="), 6 ) == 0 )
		{
			wxSscanf( strLine, wxT("Types=%[^\n]\n"), strTmp );
			wxString strWxTmp = strTmp;

			unsigned char type_count=0;
			// extract individual nodes
			wxStringTokenizer tkz( strWxTmp, wxT(",") );
			while ( tkz.HasMoreTokens() )
			{
				// get next node from input string
				strWxTmp = tkz.GetNextToken();
				// get type id
				if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE )
					m_vectTelescopeBrandDef[nBrandCount].vectTypes[type_count] = GetHardwareTypeIdByString( nHType, strWxTmp );
				else if( nHType == TELESCOPE_HARDWARE_EYEPIECE )
					m_vectEyepieceBrandDef[nBrandCount].vectTypes[type_count] = GetHardwareTypeIdByString( nHType, strWxTmp );
				else if( nHType == TELESCOPE_HARDWARE_LMOUNT )
					m_vectLMountBrandDef[nBrandCount].vectTypes[type_count] = GetHardwareTypeIdByString( nHType, strWxTmp );
				else if( nHType == TELESCOPE_HARDWARE_FOCUSER )
					m_vectFocuserBrandDef[nBrandCount].vectTypes[type_count] = GetHardwareTypeIdByString( nHType, strWxTmp );
				else if( nHType == TELESCOPE_HARDWARE_TFILTER )
					m_vectTFilterBrandDef[nBrandCount].vectTypes[type_count] = GetHardwareTypeIdByString( nHType, strWxTmp );

				type_count++;
				// safety break
				if( type_count >= 40 ) break;
			}
			// set count by type
			if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE )
				m_vectTelescopeBrandDef[nBrandCount].nTypes = type_count;
			else if( nHType == TELESCOPE_HARDWARE_EYEPIECE )
				m_vectEyepieceBrandDef[nBrandCount].nTypes = type_count;
			else if( nHType == TELESCOPE_HARDWARE_LMOUNT )
				m_vectLMountBrandDef[nBrandCount].nTypes = type_count;
			else if( nHType == TELESCOPE_HARDWARE_FOCUSER )
				m_vectFocuserBrandDef[nBrandCount].nTypes = type_count;
			else if( nHType == TELESCOPE_HARDWARE_TFILTER )
				m_vectTFilterBrandDef[nBrandCount].nTypes = type_count;

		}
	}
	// close file
	fclose( pFile );

	///////////////////////
	// if -1 load all brands
	if( nBrand == -1 )
		for( i=0; i<nBrandCount+1; i++ ) LoadHardwareIniByBrandId( nHType, i );
	else if( nBrand >= 0 )
		LoadHardwareIniByBrandId( nHType, nBrand );

	return( nBrandCount+1 );
}

////////////////////////////////////////////////////////////////////
int CTelescope::GetHardwareTypeIdByString( int nHType, const wxString& strType )
{
	int i=0, nType=0;

	if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE )
	{
		// for all types
		for( i=0; i<g_nTelescopeTypes; i++ )
		{
			if( strType.CmpNoCase( wxString(g_vectTelescopeTypes[i].type_string,wxConvUTF8) ) == 0 )
			{
				nType = i;
				// also set the global use
				g_vectTelescopeTypes[i].has_type = 1;
			}
		}

	} else if( nHType == TELESCOPE_HARDWARE_EYEPIECE )
	{
		// for all types
		for( i=0; i<g_nTelescopeEyepieceTypes; i++ )
		{
			if( strType.CmpNoCase( wxString(g_vectTelescopeEyepieceTypes[i].type_string,wxConvUTF8) ) == 0 )
			{
				nType = i;
				// also set the global use
				g_vectTelescopeEyepieceTypes[i].has_type = 1;
			}
		}

	} else if( nHType == TELESCOPE_HARDWARE_LMOUNT )
	{
		// for all types
		for( i=0; i<g_nTelescopeMountTypes; i++ )
		{
			if( strType.CmpNoCase( wxString(g_vectTelescopeMountTypes[i].type_string,wxConvUTF8) ) == 0 )
			{
				nType = i;
				// also set the global use
				g_vectTelescopeMountTypes[i].has_type = 1;
			}
		}

	} else if( nHType == TELESCOPE_HARDWARE_FOCUSER )
	{
		// for all types
		for( i=0; i<g_nTelescopeFocuserTypes; i++ )
		{
			if( strType.CmpNoCase( wxString(g_vectTelescopeFocuserTypes[i].type_string,wxConvUTF8) ) == 0 )
			{
				nType = i;
				// also set the global use
				g_vectTelescopeFocuserTypes[i].has_type = 1;
			}
		}

	} else if( nHType == TELESCOPE_HARDWARE_TFILTER )
	{
		// for all types
		for( i=0; i<g_nTelescopeFilterTypes; i++ )
		{
			if( strType.CmpNoCase( wxString(g_vectTelescopeFilterTypes[i].type_string,wxConvUTF8) ) == 0 )
			{
				nType = i;
				// also set the global use
				g_vectTelescopeFilterTypes[i].has_type = 1;
			}
		}
	}

	return( nType );
}

////////////////////////////////////////////////////////////////////
int CTelescope::HasBrandHardwareType( int nHType, int nBrand, int nType )
{ 
	if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE )
	{
		for( int i=0; i<m_vectTelescopeBrandDef[nBrand].nTypes; i++ ) 
			if( m_vectTelescopeBrandDef[nBrand].vectTypes[i] == nType ) return(1); 

	} else if( nHType == TELESCOPE_HARDWARE_EYEPIECE )
	{
		for( int i=0; i<m_vectEyepieceBrandDef[nBrand].nTypes; i++ ) 
			if( m_vectEyepieceBrandDef[nBrand].vectTypes[i] == nType ) return(1); 

	} else if( nHType == TELESCOPE_HARDWARE_LMOUNT )
	{
		for( int i=0; i<m_vectLMountBrandDef[nBrand].nTypes; i++ ) 
			if( m_vectLMountBrandDef[nBrand].vectTypes[i] == nType ) return(1); 

	} else if( nHType == TELESCOPE_HARDWARE_FOCUSER )
	{
		for( int i=0; i<m_vectFocuserBrandDef[nBrand].nTypes; i++ ) 
			if( m_vectFocuserBrandDef[nBrand].vectTypes[i] == nType ) return(1); 

	} else if( nHType == TELESCOPE_HARDWARE_TFILTER )
	{
		for( int i=0; i<m_vectTFilterBrandDef[nBrand].nTypes; i++ ) 
			if( m_vectTFilterBrandDef[nBrand].vectTypes[i] == nType ) return(1); 

	}

	return(0); 
}

////////////////////////////////////////////////////////////////////
int CTelescope::LoadHardwareIniByBrandId( int nHType, int nBrandId )
{
	// don't reload if already loaded
	if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE && m_vectTelescopeBrandDef[nBrandId].vectData.size() ) return( 0 );
	if( nHType == TELESCOPE_HARDWARE_BARLOW_LENS && m_vectBarlowLensBrandDef[nBrandId].vectData.size() ) return( 0 );
	if( nHType == TELESCOPE_HARDWARE_FOCAL_REDUCER && m_vectFocalReducerBrandDef[nBrandId].vectData.size() ) return( 0 );
	if( nHType == TELESCOPE_HARDWARE_EYEPIECE && m_vectEyepieceBrandDef[nBrandId].vectData.size() ) return( 0 );
	if( nHType == TELESCOPE_HARDWARE_LMOUNT && m_vectLMountBrandDef[nBrandId].vectData.size() ) return( 0 );
	if( nHType == TELESCOPE_HARDWARE_FOCUSER && m_vectFocuserBrandDef[nBrandId].vectData.size() ) return( 0 );
	if( nHType == TELESCOPE_HARDWARE_TFILTER && m_vectTFilterBrandDef[nBrandId].vectData.size() ) return( 0 );

	FILE* pFile = NULL;
	wxString strFile=wxT("");
	wxChar strLine[500];
	wxChar strTmp[255];
	int nLineSize=0;
	int bFoundSection=0, nHardId=0;
	long nVarLong=0;

	/////////////////////////
	// FREE ALLOACTION/BUILD FILE NAME BY TYPE
	if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE )
	{
		//UnloadTelescopeDef( nBrandId );
		strFile.Printf( wxT("%s"), m_vectTelescopeBrandDef[nBrandId].brand_ini_file );

	} else if( nHType == TELESCOPE_HARDWARE_BARLOW_LENS )
	{
		//UnloadBarllowLensDef( nBrandId );
		strFile.Printf( wxT("%s"), m_vectBarlowLensBrandDef[nBrandId].brand_ini_file );

	} else if( nHType == TELESCOPE_HARDWARE_FOCAL_REDUCER )
	{
		//UnloadFocalReducerDef( nBrandId );
		strFile.Printf( wxT("%s"), m_vectFocalReducerBrandDef[nBrandId].brand_ini_file );

	} else if( nHType == TELESCOPE_HARDWARE_EYEPIECE )
	{
		//UnloadEyepieceDef( nBrandId );
		strFile.Printf( wxT("%s"), m_vectEyepieceBrandDef[nBrandId].brand_ini_file );

	} else if( nHType == TELESCOPE_HARDWARE_LMOUNT )
	{
		//UnloadLMountDef( nBrandId );
		strFile.Printf( wxT("%s"), m_vectLMountBrandDef[nBrandId].brand_ini_file );

	} else if( nHType == TELESCOPE_HARDWARE_FOCUSER )
	{
		//UnloadLMountDef( nBrandId );
		strFile.Printf( wxT("%s"), m_vectFocuserBrandDef[nBrandId].brand_ini_file );

	} else if( nHType == TELESCOPE_HARDWARE_TFILTER )
	{
		//UnloadTFilterDef( nBrandId );
		strFile.Printf( wxT("%s"), m_vectTFilterBrandDef[nBrandId].brand_ini_file );
	}

	// open file to read
	pFile = wxFopen( strFile, wxT("r") );
	// check if there is any configuration to load
	if( !pFile ) return( -1 );

	// Reading lines from cameras ini file
	while( !feof( pFile ) )
	{
		// get one line out of the config file
		wxFgets( strLine, 500, pFile );
		nLineSize = wxStrlen( strLine );

		// check if to skip because line to short or comment
		if( nLineSize < 4 || strLine[0] == '#' ) continue;

		// check section
		//if( reSectionLabel.Matches( strLine ) )
		if( strLine[0] == '[' && strLine[nLineSize-2] == ']' )
		{
			bFoundSection = 1;
			//wxString strName = reSectionLabel.GetMatch(strLine, 1 ).Trim(0).Trim(1);
			wxChar strName[255];
			wxSscanf( strLine, wxT("\[%[^\]]\]\n"), strName );

			// by hardware type
			if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE )
				nHardId = AddTelescopeDef( nBrandId, strName );
			else if( nHType == TELESCOPE_HARDWARE_BARLOW_LENS )
				nHardId = AddBarlowLensDef( nBrandId, strName );
			else if( nHType == TELESCOPE_HARDWARE_FOCAL_REDUCER )
				nHardId = AddFocalReducerDef( nBrandId, strName );
			else if( nHType == TELESCOPE_HARDWARE_EYEPIECE )
				nHardId = AddEyepieceDef( nBrandId, strName );
			else if( nHType == TELESCOPE_HARDWARE_LMOUNT )
				nHardId = AddLMountDef( nBrandId, strName );
			else if( nHType == TELESCOPE_HARDWARE_FOCUSER )
				nHardId = AddFocuserDef( nBrandId, strName );
			else if( nHType == TELESCOPE_HARDWARE_TFILTER )
				nHardId = AddTFilterDef( nBrandId, strName );

		////////////////////
		// TELESCOPE FIELDS
		} else if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE )
		{
			if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				wxSscanf( strLine, wxT("Type=%[^\n]\n"), strTmp );
				m_vectTelescopeBrandDef[nBrandId].vectData[nHardId].type = GetHardwareTypeIdByString( nHType, strTmp );			

			} else if( wxStrncmp( strLine, wxT("FocalLength="), 12 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("FocalLength=%lf\n"), &(m_vectTelescopeBrandDef[nBrandId].vectData[nHardId].focal_length) ) )
					m_vectTelescopeBrandDef[nBrandId].vectData[nHardId].focal_length = 0.0;

			} else if( wxStrncmp( strLine, wxT("ClearAperture="), 14 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("ClearAperture=%lf\n"), &(m_vectTelescopeBrandDef[nBrandId].vectData[nHardId].aperture) ) )
					m_vectTelescopeBrandDef[nBrandId].vectData[nHardId].aperture = 0.0;

			} else if( wxStrncmp( strLine, wxT("HandController="), 15 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("HandController=%[^\n]\n"), strTmp ) )
					m_vectTelescopeBrandDef[nBrandId].vectData[nHardId].controller = wxT( "" );
				else
					m_vectTelescopeBrandDef[nBrandId].vectData[nHardId].controller = strTmp;
			}

		//////////////////////
		// BARLOW LENS FIELDS
		} else if( nHType == TELESCOPE_HARDWARE_BARLOW_LENS )
		{
			if( wxStrncmp( strLine, wxT("Magnification="), 14 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Magnification=%lf\n"), &(m_vectBarlowLensBrandDef[nBrandId].vectData[nHardId].magnification) ) )
					m_vectBarlowLensBrandDef[nBrandId].vectData[nHardId].magnification = 0.0;

			} else if( wxStrncmp( strLine, wxT("Elements="), 9 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Elements=%d\n"), &(m_vectBarlowLensBrandDef[nBrandId].vectData[nHardId].elements) ) )
					m_vectBarlowLensBrandDef[nBrandId].vectData[nHardId].elements = 0;

			} else if( wxStrncmp( strLine, wxT("Barrel="), 7 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Barrel=%lf\n"), &(m_vectBarlowLensBrandDef[nBrandId].vectData[nHardId].barrel) ) )
					m_vectBarlowLensBrandDef[nBrandId].vectData[nHardId].barrel = 0.0;

			}

		//////////////////////
		// FOCAL REDUCER FIELDS
		} else if( nHType == TELESCOPE_HARDWARE_FOCAL_REDUCER )
		{
			if( wxStrncmp( strLine, wxT("FocalRatio="), 11 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("FocalRatio=%lf\n"), &(m_vectFocalReducerBrandDef[nBrandId].vectData[nHardId].focal_ratio) ) )
					m_vectFocalReducerBrandDef[nBrandId].vectData[nHardId].focal_ratio = 0.0;

			} else if( wxStrncmp( strLine, wxT("Elements="), 9 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Elements=%d\n"), &(m_vectFocalReducerBrandDef[nBrandId].vectData[nHardId].elements) ) )
					m_vectFocalReducerBrandDef[nBrandId].vectData[nHardId].elements = 0;

			} else if( wxStrncmp( strLine, wxT("Barrel="), 7 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Barrel=%lf\n"), &(m_vectFocalReducerBrandDef[nBrandId].vectData[nHardId].barrel) ) )
					m_vectFocalReducerBrandDef[nBrandId].vectData[nHardId].barrel = 0.0;

			} else if( wxStrncmp( strLine, wxT("ClearAperture="), 14 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("ClearAperture=%lf\n"), &(m_vectFocalReducerBrandDef[nBrandId].vectData[nHardId].clear_aperture) ) )
					m_vectFocalReducerBrandDef[nBrandId].vectData[nHardId].clear_aperture = 0.0;

			}

		//////////////////////
		// EYEPIECE FIELDS
		} else if( nHType == TELESCOPE_HARDWARE_EYEPIECE )
		{
			if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				wxSscanf( strLine, wxT("Type=%[^\n]\n"), strTmp );
				m_vectEyepieceBrandDef[nBrandId].vectData[nHardId].type = GetHardwareTypeIdByString( nHType, strTmp );			

			} else if( wxStrncmp( strLine, wxT("FocalLength="), 12 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("FocalLength=%lf\n"), &(m_vectEyepieceBrandDef[nBrandId].vectData[nHardId].focal_length) ) )
					m_vectEyepieceBrandDef[nBrandId].vectData[nHardId].focal_length = 0.0;

			} else if( wxStrncmp( strLine, wxT("Elements="), 9 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Elements=%d\n"), &(m_vectEyepieceBrandDef[nBrandId].vectData[nHardId].elements) ) )
					m_vectEyepieceBrandDef[nBrandId].vectData[nHardId].elements = 0;

			} else if( wxStrncmp( strLine, wxT("Barrel="), 7 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Barrel=%lf\n"), &(m_vectEyepieceBrandDef[nBrandId].vectData[nHardId].barrel) ) )
					m_vectEyepieceBrandDef[nBrandId].vectData[nHardId].barrel = 0.0;

			} else if( wxStrncmp( strLine, wxT("ApparentField="), 7 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("ApparentField=%lf\n"), &(m_vectEyepieceBrandDef[nBrandId].vectData[nHardId].apparent_field) ) )
					m_vectEyepieceBrandDef[nBrandId].vectData[nHardId].apparent_field = 0.0;

			}

		//////////////////////
		// TELESCOPE/LENS MOUNT
		} else if( nHType == TELESCOPE_HARDWARE_LMOUNT )
		{
			if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				wxSscanf( strLine, wxT("Type=%[^\n]\n"), strTmp );
				m_vectLMountBrandDef[nBrandId].vectData[nHardId].type = GetHardwareTypeIdByString( nHType, strTmp );			

			} else if( wxStrncmp( strLine, wxT("Controller="), 11 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Controller=%[^\n]\n"), strTmp ) )
					m_vectLMountBrandDef[nBrandId].vectData[nHardId].controller = wxT( "" );
				else
					m_vectLMountBrandDef[nBrandId].vectData[nHardId].controller = strTmp;

			} else if( wxStrncmp( strLine, wxT("PEM="), 4 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("PEM=%lf\n"), &(m_vectLMountBrandDef[nBrandId].vectData[nHardId].pem) ) )
					m_vectLMountBrandDef[nBrandId].vectData[nHardId].pem = 0.0;

			} else if( wxStrncmp( strLine, wxT("Capacity="), 9 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Capacity=%lf\n"), &(m_vectLMountBrandDef[nBrandId].vectData[nHardId].capacity) ) )
					m_vectLMountBrandDef[nBrandId].vectData[nHardId].capacity = 0.0;
			}

		//////////////////////
		// FOCUSER
		} else if( nHType == TELESCOPE_HARDWARE_FOCUSER )
		{
			if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				wxSscanf( strLine, wxT("Type=%[^\n]\n"), strTmp );
				m_vectFocuserBrandDef[nBrandId].vectData[nHardId].type = GetHardwareTypeIdByString( nHType, strTmp );			

			} else if( wxStrncmp( strLine, wxT("Controller="), 11 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Controller=%[^\n]\n"), strTmp ) )
					m_vectFocuserBrandDef[nBrandId].vectData[nHardId].controller = wxT( "" );
				else
					m_vectFocuserBrandDef[nBrandId].vectData[nHardId].controller = strTmp;

			} else if( wxStrncmp( strLine, wxT("Drawtube="), 9 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Drawtube=%lf\n"), &(m_vectFocuserBrandDef[nBrandId].vectData[nHardId].drawtube) ) )
					m_vectFocuserBrandDef[nBrandId].vectData[nHardId].drawtube = 0.0;

			} else if( wxStrncmp( strLine, wxT("Weight="), 7 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Weight=%lf\n"), &(m_vectFocuserBrandDef[nBrandId].vectData[nHardId].weight) ) )
					m_vectFocuserBrandDef[nBrandId].vectData[nHardId].weight = 0.0;
			}

		//////////////////////
		// TELESCOPE FILTERS
		} else if( nHType == TELESCOPE_HARDWARE_TFILTER )
		{
			if( wxStrncmp( strLine, wxT("Type="), 5 ) == 0 )
			{
				wxSscanf( strLine, wxT("Type=%[^\n]\n"), strTmp );
				m_vectTFilterBrandDef[nBrandId].vectData[nHardId].type = GetHardwareTypeIdByString( nHType, strTmp );			

			} else if( wxStrncmp( strLine, wxT("Bandpass="), 9 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Bandpass=%[^\n]\n"), strTmp ) )
					m_vectTFilterBrandDef[nBrandId].vectData[nHardId].band_pass = wxT( "" );
				else
					m_vectTFilterBrandDef[nBrandId].vectData[nHardId].band_pass = strTmp;

			} else if( wxStrncmp( strLine, wxT("Transmission="), 13 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("Transmission=%lf\n"), &(m_vectTFilterBrandDef[nBrandId].vectData[nHardId].transmission) ) )
					m_vectTFilterBrandDef[nBrandId].vectData[nHardId].transmission = 0.0;

			} else if( wxStrncmp( strLine, wxT("ClearAperture="), 14 ) == 0 )
			{
				if( !wxSscanf( strLine, wxT("ClearAperture=%lf\n"), &(m_vectTFilterBrandDef[nBrandId].vectData[nHardId].clear_aperture) ) )
					m_vectTFilterBrandDef[nBrandId].vectData[nHardId].clear_aperture = 0.0;
			}
		}
	}
	// close file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	SetGuiSelect
////////////////////////////////////////////////////////////////////
void CTelescope::SetGuiSelect( int nHType, wxChoice* pType, wxChoice* pBrand, wxChoice* pName, 
								int nType, int nBrand, int nName, std::vector<int> &vectBrandId, std::vector<int> &vectNameId )
{
	int i=0;
	int nHardSpecType=0, nHBrand=0, nBrandSelect=0;

	// set counts by type
	if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE )
	{
		nHardSpecType = g_nTelescopeTypes;
		nHBrand = m_vectTelescopeBrandDef.size();

	} else if( nHType == TELESCOPE_HARDWARE_BARLOW_LENS )
	{
		nHardSpecType = 0;
		nHBrand = m_vectBarlowLensBrandDef.size();

	} else if( nHType == TELESCOPE_HARDWARE_FOCAL_REDUCER )
	{
		nHardSpecType = 0;
		nHBrand = m_vectFocalReducerBrandDef.size();

	} else if( nHType == TELESCOPE_HARDWARE_EYEPIECE )
	{
		nHardSpecType = g_nTelescopeEyepieceTypes;
		nHBrand = m_vectEyepieceBrandDef.size();

	} else if( nHType == TELESCOPE_HARDWARE_LMOUNT )
	{
		nHardSpecType = g_nTelescopeMountTypes;
		nHBrand = m_vectLMountBrandDef.size();

	} else if( nHType == TELESCOPE_HARDWARE_FOCUSER )
	{
		nHardSpecType = g_nTelescopeFocuserTypes;
		nHBrand = m_vectFocuserBrandDef.size();

	} else if( nHType == TELESCOPE_HARDWARE_TFILTER )
	{
		nHardSpecType = g_nTelescopeFilterTypes;
		nHBrand = m_vectTFilterBrandDef.size();
	}

	// set types
	if( pType != NULL )
	{
		pType->Freeze();
		pType->Clear();
		for( i=0; i<nHardSpecType; i++ ) 
		{
			if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE )
				pType->Append( g_vectTelescopeTypes[i].type_label );
			else if( nHType == TELESCOPE_HARDWARE_EYEPIECE )
				pType->Append( g_vectTelescopeEyepieceTypes[i].type_label );
			else if( nHType == TELESCOPE_HARDWARE_LMOUNT )
				pType->Append( g_vectTelescopeMountTypes[i].type_label );
			else if( nHType == TELESCOPE_HARDWARE_FOCUSER )
				pType->Append( g_vectTelescopeFocuserTypes[i].type_label );
			else if( nHType == TELESCOPE_HARDWARE_TFILTER )
				pType->Append( g_vectTelescopeFilterTypes[i].type_label );
		}
		pType->SetSelection( nType );
		pType->Thaw();
	}

	// set brands
	vectBrandId.clear();
	pBrand->Freeze();
	pBrand->Clear();
	for( i=0; i<nHBrand; i++ ) 
	{
		if( nType == 0 || HasBrandHardwareType( nHType, i, nType ) )
		{
			if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE )
				pBrand->Append( m_vectTelescopeBrandDef[i].brand_name );
			else if( nHType == TELESCOPE_HARDWARE_BARLOW_LENS )
				pBrand->Append( m_vectBarlowLensBrandDef[i].brand_name );
			else if( nHType == TELESCOPE_HARDWARE_FOCAL_REDUCER )
				pBrand->Append( m_vectFocalReducerBrandDef[i].brand_name );
			else if( nHType == TELESCOPE_HARDWARE_EYEPIECE )
				pBrand->Append( m_vectEyepieceBrandDef[i].brand_name );
			else if( nHType == TELESCOPE_HARDWARE_LMOUNT )
				pBrand->Append( m_vectLMountBrandDef[i].brand_name );
			else if( nHType == TELESCOPE_HARDWARE_FOCUSER )
				pBrand->Append( m_vectFocuserBrandDef[i].brand_name );
			else if( nHType == TELESCOPE_HARDWARE_TFILTER )
				pBrand->Append( m_vectTFilterBrandDef[i].brand_name );
			else
				continue;

			// check/get brand select
			if( nBrand == i ) nBrandSelect = vectBrandId.size();
			vectBrandId.push_back( i );
		}
	}
	pBrand->SetSelection( nBrandSelect );
	pBrand->Thaw();

	// set telescope name
	if( vectBrandId.size() >  0 ) 
	{
		// load - check load
		LoadHardwareIniByBrandId( nHType, nBrand );
		// set name 
		SetGuiNameSelect( nHType, pName, nType, nBrand, nName, vectNameId );

	} else
	{
		pName->Clear();
		pName->SetSelection(0);
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CTelescope::SetGuiNameSelect( int nHType, wxChoice* pName, int nType, int nBrand, int nName, std::vector<int> &vectNameId )
{

	// check if hardware loaded
	if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE && !m_vectTelescopeBrandDef.size() ) 
		return;
	else if( nHType == TELESCOPE_HARDWARE_BARLOW_LENS && !m_vectBarlowLensBrandDef.size() ) 
		return;
	else if( nHType == TELESCOPE_HARDWARE_FOCAL_REDUCER && !m_vectFocalReducerBrandDef.size() ) 
		return;
	else if( nHType == TELESCOPE_HARDWARE_EYEPIECE && !m_vectEyepieceBrandDef.size() ) 
		return;
	else if( nHType == TELESCOPE_HARDWARE_LMOUNT && !m_vectLMountBrandDef.size() ) 
		return;
	else if( nHType == TELESCOPE_HARDWARE_FOCUSER && !m_vectFocuserBrandDef.size() ) 
		return;
	else if( nHType == TELESCOPE_HARDWARE_TFILTER && !m_vectTFilterBrandDef.size() ) 
		return;

	int i=0, nHName=0, nNameSelect=0;
	if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE )
		nHName = m_vectTelescopeBrandDef[nBrand].vectData.size();
	else if( nHType == TELESCOPE_HARDWARE_BARLOW_LENS )
		nHName = m_vectBarlowLensBrandDef[nBrand].vectData.size();
	else if( nHType == TELESCOPE_HARDWARE_FOCAL_REDUCER )
		nHName = m_vectFocalReducerBrandDef[nBrand].vectData.size();
	else if( nHType == TELESCOPE_HARDWARE_EYEPIECE )
		nHName = m_vectEyepieceBrandDef[nBrand].vectData.size();
	else if( nHType == TELESCOPE_HARDWARE_LMOUNT )
		nHName = m_vectLMountBrandDef[nBrand].vectData.size();
	else if( nHType == TELESCOPE_HARDWARE_FOCUSER )
		nHName = m_vectFocuserBrandDef[nBrand].vectData.size();
	else if( nHType == TELESCOPE_HARDWARE_TFILTER )
		nHName = m_vectTFilterBrandDef[nBrand].vectData.size();

	// set telescope names in select and map
	vectNameId.clear();
	pName->Freeze();
	pName->Clear();
	for( i=0; i<nHName; i++ ) 
	{
		if( nHType == TELESCOPE_HARDWARE_MAIN_SCOPE && (nType == 0 || nType == m_vectTelescopeBrandDef[nBrand].vectData[i].type) )
			pName->Append( m_vectTelescopeBrandDef[nBrand].vectData[i].name );
		else if( nHType == TELESCOPE_HARDWARE_BARLOW_LENS )
			pName->Append( m_vectBarlowLensBrandDef[nBrand].vectData[i].name );
		else if( nHType == TELESCOPE_HARDWARE_FOCAL_REDUCER )
			pName->Append( m_vectFocalReducerBrandDef[nBrand].vectData[i].name );
		else if( nHType == TELESCOPE_HARDWARE_EYEPIECE && (nType == 0 || nType == m_vectEyepieceBrandDef[nBrand].vectData[i].type) )
			pName->Append( m_vectEyepieceBrandDef[nBrand].vectData[i].name );
		else if( nHType == TELESCOPE_HARDWARE_LMOUNT && (nType == 0 || nType == m_vectLMountBrandDef[nBrand].vectData[i].type) )
			pName->Append( m_vectLMountBrandDef[nBrand].vectData[i].name );
		else if( nHType == TELESCOPE_HARDWARE_FOCUSER && (nType == 0 || nType == m_vectFocuserBrandDef[nBrand].vectData[i].type) )
			pName->Append( m_vectFocuserBrandDef[nBrand].vectData[i].name );
		else if( nHType == TELESCOPE_HARDWARE_TFILTER && (nType == 0 || nType == m_vectTFilterBrandDef[nBrand].vectData[i].type) )
			pName->Append( m_vectTFilterBrandDef[nBrand].vectData[i].name );
		else
			continue;

		// check/get brand select
		if( nName == i ) nNameSelect = vectNameId.size();
		vectNameId.push_back( i );

	}
	pName->SetSelection( nNameSelect );
	pName->Thaw();
}
