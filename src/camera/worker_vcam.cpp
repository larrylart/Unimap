////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// local headers
#include "../observer/observer_hardware.h"
#include "camera.h"
#include "camera_video.h"
#include "../gui/frame.h"

// main header
#include "worker_vcam.h"

////////////////////////////////////////////////////////////////////
CVCameraWorker::CVCameraWorker( CGUIFrame *pFrame, CLogger *pLogger ) : CWorker( pFrame, pLogger )
{
	m_pFrame = pFrame;
}

////////////////////////////////////////////////////////////////////
CVCameraWorker::~CVCameraWorker( )
{
	// on kill send stop ???
	//SendEventType( m_pFrame->GetEventHandler(), wxID_DCAMERA_INFO_DISCONNECTED );
}

////////////////////////////////////////////////////////////////////
void CVCameraWorker::SetConfig( CCamera* pCamera )
{
	m_pCamera = pCamera;
}

////////////////////////////////////////////////////////////////////
// video camera commands
////////////////////////////////////////////////////////////////////
void CVCameraWorker::RecordMovie( int nDevId )
{
	DefCmdAction cmd;
	cmd.id = THREAD_VCAMERA_ACTION_RECORD_MOVIE;
	cmd.vectInt[0] = nDevId;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
void CVCameraWorker::SetProperty( unsigned long cmd_id, unsigned long cmd_arg )
{
	DefCmdAction cmd;
	cmd.id = THREAD_VCAMERA_ACTION_SET_PROPERTY;
	cmd.vectInt[0] = cmd_id;
	cmd.vectInt[1] = cmd_arg;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
// CHECK FOR COMMANDS
////////////////////////////////////////////////////////////////////
int CVCameraWorker::CheckForCommands( )
{
	/////////////////////
	// VIDEO FRAME FETCH
	if( m_pCamera->m_pCameraVideo && m_pCamera->m_pCameraVideo->Run( ) == 0 )
	{
		wxThread::Sleep( 10 );
//			continue;
	}

	///////////////
	// NOW GO ON WITH THE QUEUE
	if( m_vectActionQueue.size() == 0 ) return( 0 );
	char strMsg[255];

	m_nActionCmd = m_vectActionQueue.front().id;

	////////////////////////
	// check actions
	// :: DIGITAL CAMERA
	if( m_nActionCmd == THREAD_VCAMERA_ACTION_CONNECT )
	{
		m_pCamera->Connect( (int) m_vectActionQueue.front().vectInt[0] );
		wxThread::Sleep( 500 );
//		m_pCamera->m_pCameraCanonEos->GetPropertyCommand( kEdsPropID_Unknown );

		SendEventType( m_pFrame->GetEventHandler(), wxID_DCAMERA_INFO_CONNECTED ); 

	} else if( m_nActionCmd == THREAD_VCAMERA_ACTION_DISCONNECT )
	{
		// m_pCamera->Disconnect( );
		SendEventType( m_pFrame->GetEventHandler(), wxID_DCAMERA_INFO_DISCONNECTED ); 

	} else if( m_nActionCmd == THREAD_VCAMERA_ACTION_RECORD_MOVIE )
	{
		//m_pCamera->TakePhoto( (int) m_vectActionQueue.front().vectNumber[0] );

	} else if( m_nActionCmd == THREAD_VCAMERA_ACTION_NEW_MOVIE )
	{
		//SendEventType( m_pInstrCtrlDlg->GetEventHandler(), wxID_DCAMERA_GOT_NEW_IMAGE ); 

	} else if( m_nActionCmd == THREAD_VCAMERA_ACTION_SET_PROPERTY )
	{
		//m_pCamera->m_pCameraCanonEos->SetPropertyCommand( (EdsPropertyID) m_vectActionQueue.front().vectNumber[0],  
		//													(EdsUInt32) m_vectActionQueue.front().vectNumber[1] );
		// wait for the command to finish
		wxThread::Sleep( 500 );

	} else
		return( 0 );

	// reset flag
	m_bCurrentTaskSetToBreak = 0;
	// set detect action flag to none
	m_nActionCmd = THREAD_ACTION_NONE;
	m_vectActionQueue.erase(m_vectActionQueue.begin());

	return( 0 );
}