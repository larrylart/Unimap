////////////////////////////////////////////////////////////////////
// Name:        Wait Dialog
// File:		waitdialog.cpp
// Purpose:     display a waiting window
//
// Created by:	Larry Lart on 04-Jan-2007
// Updated by:	
//
// Copyright:	(c) 2007 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////


// system headers
#include <stdlib.h>
#include <math.h>
#include <time.h>


// wxwindows header
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// local headers
//#include "../unimap_worker.h"
#include "../camera/worker_dcam.h"
#include "../camera/worker_vcam.h"
#include "../telescope/telworker.h"

// main header
#include "waitdialog.h"

BEGIN_EVENT_TABLE(CWaitDialog, wxDialog)
	EVT_BUTTON( wxID_CANCEL, CWaitDialog::OnCancelTask )
	EVT_INIT_DIALOG( CWaitDialog::OnInitDlg )
	EVT_COMMAND( wxID_WAIT_DIALOG_END_PROCESS, wxEVT_CUSTOM_MESSAGE_EVENT, CWaitDialog::OnEndProcess )
	EVT_COMMAND( wxID_WAIT_DIALOG_SET_PROGRESS_STEP, wxEVT_CUSTOM_MESSAGE_EVENT, CWaitDialog::OnSetProgessStep )
	EVT_COMMAND( wxID_WAIT_DIALOG_SET_MESSAGE, wxEVT_CUSTOM_MESSAGE_EVENT, CWaitDialog::OnSetMessage )
	EVT_COMMAND( wxID_WAIT_DIALOG_SET_UPDATE, wxEVT_CUSTOM_MESSAGE_EVENT, CWaitDialog::OnSetUpdate )
	EVT_COMMAND( wxID_WAIT_DIALOG_NOTIFY_ERROR, wxEVT_CUSTOM_MESSAGE_EVENT, CWaitDialog::OnNotifyError )
	// curl event handler
	EVT_CURL_PROGRESS( CWaitDialog::OnUploadProgress )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:		Constructor
// Class:		CWaitDialog
// Purpose:		Define a constructor for my robot view
// Input:		pointer to frame & position
// Output:		nothing
////////////////////////////////////////////////////////////////////
CWaitDialog::CWaitDialog( wxWindow *parent, const wxString& strTitle, const wxString& strMsg, 
							int bLayout, void* pWorker, DefCmdAction* act, int nType ):
 							wxDialog(parent, -1, strTitle, wxPoint( -1, -1 ), 
							wxSize( 200, 60 ), wxCAPTION|wxDIALOG_MODAL )
{
	m_rWorkerAct.id = -1;
	int i=0;

	m_nType = nType;
	m_pWorker = pWorker;
	// set worker by type
	if( nType == WAITDLG_WORKER_UNIMAP )
		m_pUnimapWorker = (CUnimapWorker*) pWorker;
	else if( nType == WAITDLG_WORKER_VCAM )
		m_pVCameraWorker = (CVCameraWorker*) pWorker;
	else if( nType == WAITDLG_WORKER_DCAM )
		m_pDCameraWorker = (CDCameraWorker*) pWorker;
	else if( nType == WAITDLG_WORKER_TEL )
		m_pTelescopeWorker = (CTelescopeWorker*) pWorker;

	if( pWorker != NULL && act != NULL ) 
	{
		//memcpy( &m_rWorkerAct, act, sizeof(DefCmdAction) );
		for( i=0; i<20; i++ )
		{
			m_rWorkerAct.vectStr[i] = act->vectStr[i];
			m_rWorkerAct.vectInt[i] = act->vectInt[i];
			m_rWorkerAct.vectFloat[i] = act->vectFloat[i];
			m_rWorkerAct.vectObj[i] = act->vectObj[i];
		}
		m_rWorkerAct.id = act->id;
		m_rWorkerAct.handler = this->GetEventHandler();
		m_rWorkerAct.waitdlg = this;
	}

	SetMinSize( wxSize( 200, 60 ) );
	// main dialog sizer
	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 3, 1, 0, 5 );
	topsizer->AddGrowableRow( 1 );
//	wxFlexGridSizer* topsizer = new wxFlexGridSizer( 3, 1, 50, 5 );
	topsizer->SetMinSize( wxSize( 200, 20 ) );

	if( bLayout & LAYOUT_ADD_PROGRESS_BAR )
	{
		m_pProgressBar = new wxGauge( this, -1, 100, wxDefaultPosition, wxSize(250,10), wxGA_HORIZONTAL|wxGA_SMOOTH );
		m_pProgressBar->SetValue(0);
		topsizer->Add( m_pProgressBar, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 5  );
	} else
		m_pProgressBar = NULL;

	m_pMessage = new wxStaticText( this, -1, strMsg );
	topsizer->Add( m_pMessage, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxALL, 5  );

	// wait dialog with button
	if( bLayout & LAYOUT_ADD_CANCEL_BUTTON )
	{
		//wxButton* pCancelButton = new wxButton( this, -1, "Download" );
		topsizer->Add( CreateButtonSizer(wxCANCEL), 0, wxALIGN_CENTER |wxCENTRE | wxALL, 10 );
	}

	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);

	Centre( wxBOTH );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CWaitDialog
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CWaitDialog::~CWaitDialog( )
{
	if( m_pProgressBar ) delete( m_pProgressBar );
	delete( m_pMessage );
}

////////////////////////////////////////////////////////////////////
void CWaitDialog::OnCancelTask( wxCommandEvent& pEvent )
{
	if( m_pUnimapWorker ) m_pUnimapWorker->m_bCurrentTaskSetToBreak = 1;

	return;
}

////////////////////////////////////////////////////////////////////
void CWaitDialog::OnInitDlg( wxInitDialogEvent& pEvent )
{
	pEvent.Skip();

	if( m_pWorker && m_rWorkerAct.id >= 0 ) 
	{
		// action by type
		if( m_nType == WAITDLG_WORKER_UNIMAP )
			m_pUnimapWorker->SetAction( m_rWorkerAct );
		else if( m_nType == WAITDLG_WORKER_VCAM )
			m_pVCameraWorker->SetAction( m_rWorkerAct );
		else if( m_nType == WAITDLG_WORKER_DCAM )
			m_pDCameraWorker->SetAction( m_rWorkerAct );
		else if( m_nType == WAITDLG_WORKER_TEL )
			m_pTelescopeWorker->SetAction( m_rWorkerAct );

	}
}

////////////////////////////////////////////////////////////////////
void CWaitDialog::SetText( const wxString& msg )
{
	wxMutexGuiEnter();
	m_pMessage->SetLabel( msg );
	wxMutexGuiLeave();
}

////////////////////////////////////////////////////////////////////
void CWaitDialog::OnEndProcess( wxCommandEvent& pEvent )
{
	EndModal( 0 );
}

////////////////////////////////////////////////////////////////////
void CWaitDialog::OnSetProgessStep( wxCommandEvent& pEvent )
{
	int nProgress = pEvent.GetInt();

	m_pProgressBar->SetValue( nProgress );
	GetSizer()->Layout( );
	Fit( );
}

////////////////////////////////////////////////////////////////////
void CWaitDialog::OnSetMessage( wxCommandEvent& pEvent )
{
	wxString strMsg = pEvent.GetString();

	m_pMessage->SetLabel( strMsg );
	GetSizer()->Layout( );
	Fit( );

}

////////////////////////////////////////////////////////////////////
void CWaitDialog::OnSetUpdate( wxCommandEvent& pEvent )
{
	Layout();
	Update();
	Refresh();
}

// Method:	OnUploadProgress
////////////////////////////////////////////////////////////////////
void CWaitDialog::OnUploadProgress( wxCurlProgressEvent& pEvent )
{
	m_pProgressBar->SetValue( pEvent.UploadPercent() );
	Refresh();
}

////////////////////////////////////////////////////////////////////
void CWaitDialog::OnNotifyError( wxCommandEvent& pEvent )
{
	wxString strError = pEvent.GetString();
	wxMessageBox( strError, wxT("ERROR"), wxICON_ERROR|wxOK  );
	return;
}
