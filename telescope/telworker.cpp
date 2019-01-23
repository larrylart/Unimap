
// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// local headers
#include "../observer/observer_hardware.h"
#include "telescope.h"
#include "focuser.h"
#include "../gui/frame.h"
#include "../gui/waitdialog.h"

// main header
#include "telworker.h"

////////////////////////////////////////////////////////////////////
CTelescopeWorker::CTelescopeWorker( CGUIFrame *pFrame, CLogger *pLogger ) : CWorker( pFrame, pLogger )
{
	m_bUpdateTelescopeInfo = 1;
	m_pFrame = pFrame;
}

////////////////////////////////////////////////////////////////////
CTelescopeWorker::~CTelescopeWorker( )
{
}

////////////////////////////////////////////////////////////////////
void CTelescopeWorker::SetConfig( CTelescope* pTelescope, CFocuser* pFocuser )
{
	m_pTelescope = pTelescope;
	m_pFocuser = pFocuser;
}

////////////////////////////////////////////////////////////////////
// telescope commands
////////////////////////////////////////////////////////////////////
void CTelescopeWorker::StartMovingEast( )
{
	DefCmdAction cmd;
	cmd.id = THREAD_TELESCOPE_ACTION_MOVE_EAST;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
void CTelescopeWorker::StartMovingWest( )
{
	DefCmdAction cmd;
	cmd.id = THREAD_TELESCOPE_ACTION_MOVE_WEST;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
void CTelescopeWorker::StartMovingNorth( )
{
	DefCmdAction cmd;
	cmd.id = THREAD_TELESCOPE_ACTION_MOVE_NORTH;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
void CTelescopeWorker::StartMovingSouth( )
{
	DefCmdAction cmd;
	cmd.id = THREAD_TELESCOPE_ACTION_MOVE_SOUTH;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
void CTelescopeWorker::StopSlew( )
{
	DefCmdAction cmd;
	cmd.id = THREAD_TELESCOPE_ACTION_MOVE_STOP;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
// focuser commands
////////////////////////////////////////////////////////////////////
void CTelescopeWorker::StartFocusIn( )
{
	DefCmdAction cmd;
	cmd.id = THREAD_TELESCOPE_ACTION_START_FOCUS_IN;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
void CTelescopeWorker::StartFocusOut( )
{
	DefCmdAction cmd;
	cmd.id = THREAD_TELESCOPE_ACTION_START_FOCUS_OUT;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
void CTelescopeWorker::StopFocus( )
{
	DefCmdAction cmd;
	cmd.id = THREAD_TELESCOPE_ACTION_STOP_FOCUS;
	SetAction( cmd );
}

////////////////////////////////////////////////////////////////////
// CHECK FOR COMMANDS
////////////////////////////////////////////////////////////////////
int CTelescopeWorker::CheckForCommands( )
{
	/////////////////////////
	// DO THE ALL ON COMMANDS
	// :: get telescope pointing
	double ra=0.0, dec=0.0;
	if( m_bUpdateTelescopeInfo )
	{
		if( m_pTelescope->GetPosition( ra, dec ) )
		{
			m_bUpdateTelescopeInfo = 0;
			SendEventType( m_pFrame->GetEventHandler(), wxID_TELESCOPE_INFO_UPDATE ); 
		}
	}

	///////////////
	// NOW GO ON WITH THE QUEUE
	if( m_vectActionQueue.size() == 0 ) return( 0 );
	char strMsg[255];

	m_nActionCmd = m_vectActionQueue.front().id;

	////////////////////////
	// check actions
	// :: TELESCOPE
	if( m_nActionCmd == THREAD_TELESCOPE_ACTION_CONNECT )
	{
		if( m_pTelescope->Connect( ) )
		{
			m_bUpdateTelescopeInfo = 1;
			SendEventType( m_pFrame->GetEventHandler(), wxID_TELESCOPE_INFO_CONNECTED ); 

		} else
		{
			m_bUpdateTelescopeInfo = 0;
		}

	} else if( m_nActionCmd == THREAD_TELESCOPE_ACTION_DISCONNECT )
	{
		m_bUpdateTelescopeInfo = 0;
		m_pTelescope->Disconnect( );
		SendEventType( m_pFrame->GetEventHandler(), wxID_TELESCOPE_INFO_DISCONNECTED ); 

	} else if( m_nActionCmd == THREAD_TELESCOPE_ACTION_GOTO )
	{
		m_evtWaitDialog = m_vectActionQueue[0].handler;

		double ra = m_vectActionQueue[0].vectFloat[0];
		double dec = m_vectActionQueue[0].vectFloat[1];
		m_pTelescope->GotoPosition( ra, dec );

		// now send event that is done
		SendEventType( m_evtWaitDialog, wxID_WAIT_DIALOG_END_PROCESS ); 

	} else if( m_nActionCmd == THREAD_TELESCOPE_ACTION_MOVE_EAST )
	{
		m_pTelescope->StartMovingEast();

	} else if( m_nActionCmd == THREAD_TELESCOPE_ACTION_MOVE_WEST )
	{
		m_pTelescope->StartMovingWest();

	} else if( m_nActionCmd == THREAD_TELESCOPE_ACTION_MOVE_NORTH )
	{
		m_pTelescope->StartMovingNorth();

	} else if( m_nActionCmd == THREAD_TELESCOPE_ACTION_MOVE_SOUTH )
	{
		m_pTelescope->StartMovingSouth();

	} else if( m_nActionCmd == THREAD_TELESCOPE_ACTION_MOVE_STOP )
	{
		m_pTelescope->StopSlew();

	// :: FOCUSER
	} else if( m_nActionCmd == THREAD_TELESCOPE_ACTION_START_FOCUS_IN )
	{
		m_pFocuser->StartFocusIn();

	} else if( m_nActionCmd == THREAD_TELESCOPE_ACTION_START_FOCUS_OUT )
	{
		m_pFocuser->StartFocusOut();

	} else if( m_nActionCmd == THREAD_TELESCOPE_ACTION_STOP_FOCUS )
	{
		m_pFocuser->StopFocus();

	} else
		return( 0 );

	// reset flag
	m_bCurrentTaskSetToBreak = 0;
	// set detect action flag to none
	m_nActionCmd = THREAD_ACTION_NONE;
	m_vectActionQueue.erase(m_vectActionQueue.begin());

	return( 0 );
}