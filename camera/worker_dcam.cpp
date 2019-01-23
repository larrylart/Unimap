
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
#include "camera_canoneos.h"
#include "../gui/frame.h"
#include "../gui/waitdialog.h"

// main header
#include "worker_dcam.h"

////////////////////////////////////////////////////////////////////
CDCameraWorker::CDCameraWorker( CGUIFrame *pFrame, CLogger *pLogger ) : CWorker( pFrame, pLogger )
{
	m_pFrame = pFrame;
	m_evtWaitDialog = NULL;
}

////////////////////////////////////////////////////////////////////
CDCameraWorker::~CDCameraWorker( )
{
}

////////////////////////////////////////////////////////////////////
void CDCameraWorker::SetConfig( CCamera* pCamera )
{
	m_pCamera = pCamera;
}

////////////////////////////////////////////////////////////////////
// digital camera commands
////////////////////////////////////////////////////////////////////
void CDCameraWorker::TakePhoto( int nDevId, const wxString& strPath )
{
	DefCmdAction cmd;
	cmd.id = THREAD_DCAMERA_ACTION_TAKE_PHOTO;
	cmd.vectInt[0] = nDevId;
	cmd.vectStr[0] = strPath;
	cmd.handler = NULL;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
void CDCameraWorker::SetProperty( unsigned long cmd_id, unsigned long cmd_arg )
{
	DefCmdAction cmd;
	cmd.id = THREAD_DCAMERA_ACTION_SET_PROPERTY;
	cmd.vectInt[0] = cmd_id;
	cmd.vectInt[1] = cmd_arg;
	cmd.handler = NULL;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
// CHECK FOR COMMANDS
////////////////////////////////////////////////////////////////////
int CDCameraWorker::CheckForCommands( )
{
	///////////////
	// NOW GO ON WITH THE QUEUE
	if( m_vectActionQueue.size() == 0 ) return( 0 );
	char strMsg[255];

	m_nActionCmd = m_vectActionQueue.front().id;

	////////////////////////
	// check actions
	// :: DIGITAL CAMERA
	if( m_nActionCmd == THREAD_DCAMERA_ACTION_CONNECT )
	{
		m_pCamera->Connect( (int) m_vectActionQueue.front().vectInt[0] );
		wxThread::Sleep( 500 );
//		m_pCamera->m_pCameraCanonEos->GetPropertyCommand( kEdsPropID_Unknown );

		// now send event that is done
		SendEventType( m_vectActionQueue[0].handler, wxID_WAIT_DIALOG_END_PROCESS ); 

//		SendEventType( m_pFrame->GetEventHandler(), wxID_DCAMERA_INFO_CONNECTED ); 

	} else if( m_nActionCmd == THREAD_DCAMERA_ACTION_DISCONNECT )
	{
		// m_pCamera->Disconnect( );
		SendEventType( m_pFrame->GetEventHandler(), wxID_DCAMERA_INFO_DISCONNECTED ); 

	} else if( m_nActionCmd == THREAD_DCAMERA_ACTION_TAKE_PHOTO )
	{
		m_evtWaitDialog = m_vectActionQueue[0].handler;

		int nDevId = (int) m_vectActionQueue.front().vectInt[0];
		wxString strWorkPath = m_vectActionQueue.front().vectStr[0];
		m_pCamera->TakePhoto( nDevId, strWorkPath );

	} else if( m_nActionCmd == THREAD_DCAMERA_ACTION_NEW_IMAGE )
	{
		// if wait dialog - from the other place
		if( m_evtWaitDialog )
			SendEventType( m_evtWaitDialog, wxID_WAIT_DIALOG_END_PROCESS ); 

		SendEventType( m_pFrame->GetEventHandler(), wxID_DCAMERA_GOT_NEW_IMAGE ); 

	} else if( m_nActionCmd == THREAD_DCAMERA_ACTION_SET_PROPERTY )
	{
		m_pCamera->m_pCameraCanonEos->SetPropertyCommand( (EdsPropertyID) m_vectActionQueue.front().vectInt[0],  
															(EdsUInt32) m_vectActionQueue.front().vectInt[1] );
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