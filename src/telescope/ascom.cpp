////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// main header
#include "ascom.h"

////////////
// Required for IDictionaryPtr which is used by DriverHelper.Profile
#import "file:C:\\Windows\\System32\\ScrRun.dll" \
	no_namespace \
	rename("DeleteFile","DeleteFileItem") \
	rename("MoveFile","MoveFileItem") \
	rename("CopyFile","CopyFileItem") \
	rename("FreeSpace","FreeDriveSpace") \
	rename("Unknown","UnknownDiskType") \
	rename("Folder","DiskFolder")

// Creates COM "smart pointer" _com_ptr_t named _ChooserPtr
#import "progid:DriverHelper.Chooser" \
	rename("Yield","ASCOMYield") \
	rename("MessageBox","ASCOMMessageBox")

using namespace DriverHelper;

////////////////////////////////////////////////////////////////////
CAscom::CAscom( int nDrvType )
{
	// set type and init by
	m_nDriverType = nDrvType;
	if( m_nDriverType == ASCOM_DRV_TYPE_TELESCOPE )
		m_strDriverType = wxT("Telescope");	
	else if( m_nDriverType == ASCOM_DRV_TYPE_FOCUSER )
		m_strDriverType = wxT("Focuser");

	CoInitialize(NULL);	
	// [sentinel] Smart pointer again 
	m_pAscomTelescope = NULL;
	m_pAscomFocuser = NULL;
	m_bIsConnected = 0;
	m_bCanMoveAxis = 0;
	m_nMaxFocus = 0;
}

////////////////////////////////////////////////////////////////////
CAscom::~CAscom( )
{
	Disconnect( );
}

////////////////////////////////////////////////////////////////////
int CAscom::SelectInstrument( )
{
	// [sentinel] Using smart pointer (_com_ptr_t<>)
	_ChooserPtr C = NULL;									
	// Little known fact: ProgID will work here
	C.CreateInstance("DriverHelper.Chooser");							
	// Setting the "ByValue" flavor of DeviceType (_bstr_t magic here)
	C->DeviceTypeV = m_strDriverType.GetData();	

	// show the chooser
	_bstr_t  drvrId = C->Choose("");

	if( C != NULL )
	{
		// Chooser valid, release it now, we're done with it
		C.Release();
		// More _bstr_t magic, cast to convert BSTR to char *!!!
		//printf("Selected %s\nloading...", (char *)drvrId);

		wxString strWxTmp((char *)drvrId,wxConvUTF8);
		m_strDriverId = strWxTmp;

	} else
		return( 0 );

	return( 1 );
}

// main connect method for all types of drivers
////////////////////////////////////////////////////////////////////
int CAscom::Connect( )
{
	int bState = 0;

	if( m_nDriverType == ASCOM_DRV_TYPE_TELESCOPE )
		bState = ConnectTelescope( );
	else if( m_nDriverType == ASCOM_DRV_TYPE_FOCUSER )
		bState = ConnectFocuser( );
	
	return( bState );
}

// Method:	ConnectTelescope
////////////////////////////////////////////////////////////////////
int CAscom::ConnectTelescope( )
{
	if( m_nDriverType != ASCOM_DRV_TYPE_TELESCOPE ) return( 0 );

	// _bstr_t again..
	m_pAscomTelescope.CreateInstance( (LPCSTR) m_strDriverId.ToAscii() );	
	if( m_pAscomTelescope != NULL )
	{
		// check if already connected 
		m_pAscomTelescope->Connected = true;
		m_bIsConnected = m_pAscomTelescope->GetConnected();
		if( !m_bIsConnected )
		{
			printf( "Failed to connect %s\n", (char *) m_pAscomTelescope->Name );
		} else
		{
			// get properties by type
			m_bCanMoveAxis = m_pAscomTelescope->CanMoveAxis((TelescopeAxes) 0);

			// get telescope rates
			//m_pInstMovRateOnRA = m_pInstrument->AxisRates( (TelescopeAxes) 0 );
			//m_pInstMovRateOnDEC = m_pInstrument->AxisRates( (TelescopeAxes) 1 );

			// just test
//			int ncount = m_pInstMovRateOnRA->GetCount();
//			double min = m_pInstMovRateOnRA->GetItem(1)->Minimum;
//			double max = m_pInstMovRateOnRA->GetItem(1)->Maximum;

//			az += 10.0;
//			m_pInstrument->SlewToAltAz( az, alt );

//			ra += 1.0;
//			m_pInstrument->SlewToCoordinatesAsync( ra, dec );
//			m_pInstrument->MoveAxis( (TelescopeAxes) 0, 1.0 );

			// seed - Guiding Rate=RG#, Centering rate=RC# , Find Rate=RM#, fastest=RS#
//			m_pInstrument->CommandBlind( "RS#", 0 );

		}
		//m_pInstrument.Release();
	}

	return( m_bIsConnected );
}

// Method:	ConnectFocuser
////////////////////////////////////////////////////////////////////
int CAscom::ConnectFocuser( )
{
	if( m_nDriverType != ASCOM_DRV_TYPE_FOCUSER ) return( 0 );

	// _bstr_t again..
	m_pAscomFocuser.CreateInstance( (LPCSTR) m_strDriverId.ToAscii() );	
	if( m_pAscomFocuser != NULL )
	{
		// check if already connected 
		m_pAscomFocuser->Link  = true;
		m_bIsConnected = m_pAscomFocuser->Link;
		if( !m_bIsConnected )
		{
			printf( "Failed to connect %s\n", m_strDriverId );
		} else
		{
			// get focuser max limit
			m_nMaxFocus = m_pAscomFocuser->MaxStep;
		}
	}

	return( m_bIsConnected );
}

////////////////////////////////////////////////////////////////////
void CAscom::Disconnect( )
{
	if( m_pAscomTelescope != NULL ) m_pAscomTelescope.Release();
	if( m_pAscomFocuser != NULL ) m_pAscomFocuser.Release();
}

// Method:	GetPosition
////////////////////////////////////////////////////////////////////
int CAscom::GetPosition( double& ra, double& dec )
{
	if( m_pAscomTelescope != NULL )
	{
		// convert ra from hours
		ra = m_pAscomTelescope->GetRightAscension()*15.0;
		dec = m_pAscomTelescope->GetDeclination();
	} else
		return( 0 );

	return( 1 );
}

// Method:	GotoPosition
////////////////////////////////////////////////////////////////////
int CAscom::GotoPosition( double ra, double dec )
{
	if( m_pAscomTelescope != NULL )
	{
		// convert ra to hours
		ra /= 15.0;

		if( m_pAscomTelescope->CanSlew && !m_pAscomTelescope->AtPark )
		{
			//m_pAscomTelescope->SlewToAltAz( 10.0, 30.0 );
			m_pAscomTelescope->SlewToCoordinates( ra, dec );
			// try slew to target
			//m_pAscomTelescope->TargetRightAscension = ra;
			//m_pAscomTelescope->TargetDeclination = dec;
			//m_pAscomTelescope->SlewToTarget();

		} else if( m_pAscomTelescope->CanSlewAsync )
			// slew to async - return imediately 
			m_pAscomTelescope->SlewToCoordinatesAsync( ra, dec );

	} else
		return( 0 );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int CAscom::StartMovingEast()
{ 
	if( m_bCanMoveAxis )
	{
		m_pAscomTelescope->MoveAxis( (TelescopeAxes) 0, m_pInstMovRateOnRA->GetItem(0)->Maximum  ); 

	// meade damn case ... brute force
	} else if( m_strDriverId.CmpNoCase( wxT("MeadeEx.Telescope") ) == 0 )
	{
		// set max speed
		m_pAscomTelescope->CommandBlind( "RS#", 0 );
		// start moving east
		m_pAscomTelescope->CommandBlind( "Me#", 0 );
	}

	return( 1 );
} 

////////////////////////////////////////////////////////////////////
int CAscom::StartMovingWest()
{ 
	if( m_bCanMoveAxis )
	{
		m_pAscomTelescope->MoveAxis( (TelescopeAxes) 0, -m_pInstMovRateOnRA->GetItem(0)->Maximum  ); 

	// meade damn case ... brute force
	} else if( m_strDriverId.CmpNoCase( wxT("MeadeEx.Telescope") ) == 0 )
	{
		// set max speed
		m_pAscomTelescope->CommandBlind( "RS#", 0 );
		// start moving east
		m_pAscomTelescope->CommandBlind( "Mw#", 0 );
	}

	return( 1 );
} 

////////////////////////////////////////////////////////////////////
int CAscom::StartMovingNorth()
{ 
	if( m_bCanMoveAxis )
	{
		m_pAscomTelescope->MoveAxis( (TelescopeAxes) 1, m_pInstMovRateOnDEC->GetItem(0)->Maximum  ); 

	// meade damn case ... brute force
	} else if( m_strDriverId.CmpNoCase( wxT("MeadeEx.Telescope") ) == 0 )
	{
		// set max speed
		m_pAscomTelescope->CommandBlind( "RS#", 0 );
		// start moving east
		m_pAscomTelescope->CommandBlind( "Mn#", 0 );
	}

	return( 1 );
} 

////////////////////////////////////////////////////////////////////
int CAscom::StartMovingSouth()
{ 
	if( m_bCanMoveAxis )
	{
		m_pAscomTelescope->MoveAxis( (TelescopeAxes) 1, -m_pInstMovRateOnDEC->GetItem(0)->Maximum  ); 

	// meade damn case ... brute force
	} else if( m_strDriverId.CmpNoCase( wxT("MeadeEx.Telescope") ) == 0 )
	{
		// set max speed
		m_pAscomTelescope->CommandBlind( "RS#", 0 );
		// start moving east
		m_pAscomTelescope->CommandBlind( "Ms#", 0 );
	}

	return( 1 );
} 
