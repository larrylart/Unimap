////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _ASCOM_H
#define _ASCOM_H

#include "wx/wxprec.h"

////////////////
// Import the ASCOM Interfaces en masse
#import "file:c:\Program Files (x86)\Common Files\ASCOM\Interface\AscomMasterInterfaces.tlb"

// ascom name spaces
using namespace AscomInterfacesLib;

// enum ascom drv types
enum
{
	ASCOM_DRV_TYPE_UNKNOWN = 0,
	ASCOM_DRV_TYPE_TELESCOPE,
	ASCOM_DRV_TYPE_FOCUSER,
	ASCOM_DRV_TYPE_DOME,
	ASCOM_DRV_TYPE_ROTATOR,
	ASCOM_DRV_TYPE_FILTER,
	ASCOM_DRV_TYPE_CAMERA
};

// class:	CAscom
///////////////////////////////////////////////////////
class CAscom
{
// methods
public:
    CAscom( int nDrvType );
	~CAscom( );

	// setup
	int SelectInstrument( );
	// connection 
	int Connect( );
	int ConnectTelescope( );
	int ConnectFocuser( );
	void Disconnect( );
	int IsConnected( ){ return( m_bIsConnected ); }

	////////////////////////
	// TELESCOPE METHODS
	// get data
	int GetPosition( double& ra, double& dec );
	// set position
	int GotoPosition( double ra, double dec );
	// commands
	int StopSlew(){ return( (int) m_pAscomTelescope->AbortSlew() ); } 
	int StartMovingEast();
	int StartMovingWest();
	int StartMovingNorth();
	int StartMovingSouth();

	////////////////////////
	// FOCUSER METHODS
	int StartFocusIn(){ m_pAscomFocuser->Move( m_nMaxFocus ); return(1); }
	int StartFocusOut(){ m_pAscomFocuser->Move( -m_nMaxFocus ); return(1); }
	int StopFocus(){ m_pAscomFocuser->Halt(); return(1); }
	// absolute focus positioning
	int FocusIn( long nUnits ){ m_pAscomFocuser->Move( nUnits ); return(1); }
	int FocusOut( long nUnits ){ m_pAscomFocuser->Move( -nUnits ); return(1); }
	int FocusAt( long nPosition ){ m_pAscomFocuser->Move( nPosition ); return(1); }

/////////////
// ::DATA
public:
	int m_nDriverType;
	wxString m_strDriverType;

	ITelescopePtr m_pAscomTelescope;
	IFocuserPtr m_pAscomFocuser;

	IAxisRatesPtr m_pInstMovRateOnRA;
	IAxisRatesPtr m_pInstMovRateOnDEC;

	wxString m_strDriverId;

	int m_bCanMoveAxis;
	int m_bIsConnected;
	// focus props
	long m_nMaxFocus;
};

#endif
