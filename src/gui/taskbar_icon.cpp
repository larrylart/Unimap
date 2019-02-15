////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

// local headers
#include "frame.h"

// main header
#include "taskbar_icon.h"

BEGIN_EVENT_TABLE(CFrameTaskBarIcon, wxTaskBarIcon)
	EVT_TASKBAR_LEFT_DCLICK(CFrameTaskBarIcon::OnShowFrame)
	EVT_MENU( wxID_RCLICK_ICON_EXIT, CFrameTaskBarIcon::OnExit )
	EVT_MENU( wxID_RCLICK_ICON_SHOW, CFrameTaskBarIcon::OnShow )
//	EVT_TASKBAR_RIGHT_DOWN()
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	OnShowFrame
// Class:	CFrameTaskBarIcon
// Purpose:	show frame on left double click
// Input:	reference to event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CFrameTaskBarIcon::OnShowFrame( wxTaskBarIconEvent& WXUNUSED(pEvent) )
{
	// first start video if
//	if( m_pFrame->m_bCamView == 1 ) m_pFrame->m_pCamView->SetVisible( 1 );
	// then start gesture if
//	if( m_pFrame->m_bGestureView == 1 ) m_pFrame->m_pGestureView->SetVisible( 1 );

	m_pFrame->Show(TRUE);
	m_pFrame->Iconize( false );
//	m_pFrame->Show(TRUE);
	m_pFrame->Raise();
	RemoveIcon( );
	m_pFrame->m_bIconized = 0;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	CreatePopupMenu
// Class:	CFrameTaskBarIcon
// Purpose:	overwrite default CreatePopupMenu
// Input:	reference to event
// Output:	nothing
////////////////////////////////////////////////////////////////////
wxMenu* CFrameTaskBarIcon::CreatePopupMenu( )
{
	wxMenu* pMenu = new wxMenu( wxT(""), 0 );
	pMenu->Append( wxID_RCLICK_ICON_EXIT, wxT("Exit") );
	pMenu->Append( wxID_RCLICK_ICON_SHOW, wxT("Show") );
//	pMenu->Append( wxID_RCLICK_ICON_STOP, wxT("Stop") );

/*	if( m_pFrame->m_pStartMonButton->IsEnabled( ) )
		pMenu->Enable( wxID_RCLICK_ICON_START, true );
	else
		pMenu->Enable( wxID_RCLICK_ICON_START, false );

	if( m_pFrame->m_pStopMonButton->IsEnabled( ) )
		pMenu->Enable( wxID_RCLICK_ICON_STOP, true );
	else
		pMenu->Enable( wxID_RCLICK_ICON_STOP, false );
*/
	return( pMenu );
}

////////////////////////////////////////////////////////////////////
// Purpose:	exit unimap program
////////////////////////////////////////////////////////////////////
void CFrameTaskBarIcon::OnExit( wxCommandEvent& WXUNUSED(pEvent) )
{
//    this->Destroy();
	m_pFrame->Close( );
	return;
}

////////////////////////////////////////////////////////////////////
// Purpose:		show frame
////////////////////////////////////////////////////////////////////
void CFrameTaskBarIcon::OnShow( wxCommandEvent& WXUNUSED(pEvent) )
{
	m_pFrame->Show(TRUE);
	m_pFrame->Iconize( false );
//	m_pFrame->Show(TRUE);
	m_pFrame->Raise();
	RemoveIcon( );
	m_pFrame->m_bIconized = 0;

	return;
}

