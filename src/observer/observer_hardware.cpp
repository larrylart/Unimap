////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// wx
#include "wx/wxprec.h"

// main header
#include "observer_hardware.h"

////////////////////////////////////////////////////////////////////
CObserverHardware::CObserverHardware( )
{
	ResetAll( );
}

////////////////////////////////////////////////////////////////////
CObserverHardware::~CObserverHardware( ) 
{
	ClearAll( );
}

////////////////////////////////////////////////////////////////////
void CObserverHardware::ClearAll( ) 
{
	// delete allocated
/*	if( m_vectCamera ) free( m_vectCamera );
	if( m_vectTelescope ) free( m_vectTelescope );
	if( m_vectCameraLens ) free( m_vectCameraLens );
	if( m_vectEyepiece ) free( m_vectEyepiece );
	if( m_vectBarlowLens ) free( m_vectBarlowLens );
	if( m_vectFocalReducer ) free( m_vectFocalReducer );
	if( m_vectTFilter ) free( m_vectTFilter );
	if( m_vectLMount ) free( m_vectLMount );
	if( m_vectFocuser ) free( m_vectFocuser );

	if( m_vectSetup.size() ) m_vectSetup.clear();
*/
	ResetAll( );
}

////////////////////////////////////////////////////////////////////
void CObserverHardware::ResetAll( ) 
{
	// camera
//	m_nCamera = 0;
//	m_nCameraAllocated = 0;
	m_vectCamera.clear();
	// telescope
//	m_nTelescope = 0;
//	m_nTelescopeAllocated = 0;
	m_vectTelescope.clear();
	// camera lens
	m_vectCameraLens.clear();
//	m_nCameraLens = 0;
//	m_nCameraLensAllocated = 0;
	// eyepiece
	m_vectEyepiece.clear();
//	m_nEyepiece = 0;
//	m_nEyepieceAllocated = 0;
	// Barlow Lens
	m_vectBarlowLens.clear();
//	m_nBarlowLens = 0;
//	m_nBarlowLensAllocated = 0;
	// Focal Reducer
	m_vectFocalReducer.clear();
//	m_nFocalReducer = 0;
//	m_nFocalReducerAllocated = 0;
	// Telescope Mount
	m_vectLMount.clear();
//	m_nLMount = 0;
//	m_nLMountAllocated = 0;
	// telescope focuser
	m_vectFocuser.clear();
//	m_nFocuser = 0;
//	m_nFocuserAllocated = 0;
	// telescope filter
	m_vectTFilter.clear();
//	m_nTFilter = 0;
//	m_nTFilterAllocated = 0;

	// setups
//	m_nSetup = 0;
//	m_nSetupAllocated = 0;
	m_vectSetup.clear();
}

////////////////////////////////////////////////////////////////////
void CObserverHardware::CopyTo( CObserverHardware* pDst ) 
{
	int i=0;

	pDst->ClearAll();
	///////////////////////
	// copy camera data
	for( i=0; i<m_vectCamera.size(); i++ ) pDst->AddCamera( m_vectCamera[i] );

	///////////////////////
	// copy telescope data
	for( i=0; i<m_vectTelescope.size(); i++ ) pDst->AddTelescope( m_vectTelescope[i] );

	///////////////////////
	// copy camera lens data
	for( i=0; i<m_vectCameraLens.size(); i++ ) pDst->AddCameraLens( m_vectCameraLens[i] );

	///////////////////////
	// copy barlow lens data
	for( i=0; i<m_vectBarlowLens.size(); i++ ) pDst->AddBarlowLens( m_vectBarlowLens[i] );

	///////////////////////
	// copy focal reducer data
	for( i=0; i<m_vectFocalReducer.size(); i++ ) pDst->AddFocalReducer( m_vectFocalReducer[i] );

	///////////////////////
	// copy eyepiece data
	for( i=0; i<m_vectEyepiece.size(); i++ ) pDst->AddEyepiece( m_vectEyepiece[i] );

	///////////////////////
	// copy mount data
	for( i=0; i<m_vectLMount.size(); i++ ) pDst->AddLMount( m_vectLMount[i] );

	///////////////////////
	// copy focuser data
	for( i=0; i<m_vectFocuser.size(); i++ ) pDst->AddFocuser( m_vectFocuser[i] );

	///////////////////////
	// copy telescope filter data
	for( i=0; i<m_vectTFilter.size(); i++ ) pDst->AddTFilter( m_vectTFilter[i] );

	///////////////////////
	// copy SETUPS data
	for( i=0; i<m_vectSetup.size(); i++ ) pDst->AddSetup( m_vectSetup[i] );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::AddCamera( DefCamera& src ) 
{
/*	// first check allocation for cameras
	if( !m_vectCamera )
	{
		m_nCameraAllocated = 5;
		m_vectCamera = (DefCamera*) calloc( m_nCameraAllocated, sizeof(DefCamera) );

	} else if( m_nCamera >= m_nCameraAllocated )
	{
		m_nCameraAllocated += 5;
		m_vectCamera = (DefCamera*) _recalloc( m_vectCamera, m_nCameraAllocated, sizeof(DefCamera) );	
	}

	// now copy the fields in the current - new method
	memcpy( &(m_vectCamera[m_nCamera]), &src, sizeof(DefCamera) );

	// increment counter
	m_nCamera++;
*/
	m_vectCamera.push_back( src );

	return( m_vectCamera.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::DeleteCamera( int id ) 
{
	m_vectCamera.erase( m_vectCamera.begin() + id );
	int nSize = m_vectCamera.size();
	if( nSize == 0 )
		return(-1);
	else if( id >= nSize )
		return( id-1 );
	else
		return( id );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::AddCameraLens( DefCameraLens& src ) 
{
	m_vectCameraLens.push_back( src );
	return( m_vectCameraLens.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::DeleteCameraLens( int id ) 
{
	m_vectCameraLens.erase( m_vectCameraLens.begin() + id );
	int nSize = m_vectCameraLens.size();
	if( nSize == 0 )
		return(-1);
	else if( id >= nSize )
		return( id-1 );
	else
		return( id );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::AddTelescope( DefTelescope& src ) 
{
	m_vectTelescope.push_back( src );
	return( m_vectTelescope.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::DeleteTelescope( int id ) 
{
	m_vectTelescope.erase( m_vectTelescope.begin() + id );
	int nSize = m_vectTelescope.size();
	if( nSize == 0 )
		return(-1);
	else if( id >= nSize )
		return( id-1 );
	else
		return( id );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::AddBarlowLens( DefBarlowLens& src ) 
{
	m_vectBarlowLens.push_back( src );
	return( m_vectBarlowLens.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::DeleteBarlowLens( int id ) 
{
	m_vectBarlowLens.erase( m_vectBarlowLens.begin() + id );
	int nSize = m_vectBarlowLens.size();
	if( nSize == 0 )
		return(-1);
	else if( id >= nSize )
		return( id-1 );
	else
		return( id );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::AddFocalReducer( DefFocalReducer& src ) 
{
	m_vectFocalReducer.push_back( src );
	return( m_vectFocalReducer.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::DeleteFocalReducer( int id ) 
{
	m_vectFocalReducer.erase( m_vectFocalReducer.begin() + id );
	int nSize = m_vectFocalReducer.size();
	if( nSize == 0 )
		return(-1);
	else if( id >= nSize )
		return( id-1 );
	else
		return( id );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::AddEyepiece( DefEyepiece& src ) 
{
	m_vectEyepiece.push_back( src );
	return( m_vectEyepiece.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::DeleteEyepiece( int id ) 
{
	m_vectEyepiece.erase( m_vectEyepiece.begin() + id );
	int nSize = m_vectEyepiece.size();
	if( nSize == 0 )
		return(-1);
	else if( id >= nSize )
		return( id-1 );
	else
		return( id );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::AddLMount( DefLMount& src ) 
{
	m_vectLMount.push_back( src );
	return( m_vectLMount.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::DeleteLMount( int id ) 
{
	m_vectLMount.erase( m_vectLMount.begin() + id );
	int nSize = m_vectLMount.size();
	if( nSize == 0 )
		return(-1);
	else if( id >= nSize )
		return( id-1 );
	else
		return( id );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::AddFocuser( DefFocuser& src ) 
{
	m_vectFocuser.push_back( src );
	return( m_vectFocuser.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::DeleteFocuser( int id ) 
{
	m_vectFocuser.erase( m_vectFocuser.begin() + id );
	int nSize = m_vectFocuser.size();
	if( nSize == 0 )
		return(-1);
	else if( id >= nSize )
		return( id-1 );
	else
		return( id );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::AddTFilter( DefTFilter& src ) 
{
	m_vectTFilter.push_back( src );
	return( m_vectTFilter.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::DeleteTFilter( int id ) 
{
	m_vectTFilter.erase( m_vectTFilter.begin() + id );
	int nSize = m_vectTFilter.size();
	if( nSize == 0 )
		return(-1);
	else if( id >= nSize )
		return( id-1 );
	else
		return( id );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::AddSetup( DefHarwareSetup& src ) 
{
	m_vectSetup.push_back( src );
	return( m_vectSetup.size()-1 );
}

////////////////////////////////////////////////////////////////////
int CObserverHardware::DeleteSetup( int id ) 
{
	m_vectSetup.erase( m_vectSetup.begin() + id );
	int nSize = m_vectSetup.size();
	if( nSize == 0 )
		return(-1);
	else if( id >= nSize )
		return( id-1 );
	else
		return( id );
}
