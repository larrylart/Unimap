////////////////////////////////////////////////////////////////////
// Name:		Wait Dialog Header
// File:		waitdialog.h
// Purpose:		defines a wait dialog window
//
// Created by:	Larry Lart on 04-Jan-2007
// Updated by:	
//
// Copyright:	(c) 2007 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _WAIT_DIALOG_H
#define _WAIT_DIALOG_H

// wxWindows includes
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// gui id's
enum
{
	wxID_WAIT_DIALOG_END_PROCESS = 33330,
	wxID_WAIT_DIALOG_SET_PROGRESS_STEP,
	wxID_WAIT_DIALOG_SET_MESSAGE,
	wxID_WAIT_DIALOG_SET_TITLE,
	wxID_WAIT_DIALOG_SET_UPDATE,
	wxID_WAIT_DIALOG_NOTIFY_ERROR
};
// worker type defines
#define WAITDLG_WORKER_UNIMAP	0
#define WAITDLG_WORKER_VCAM		1
#define WAITDLG_WORKER_DCAM		2
#define WAITDLG_WORKER_TEL		3

// custom defines
#define LAYOUT_ADD_PROGRESS_BAR		1
#define LAYOUT_ADD_CANCEL_BUTTON	2

// local headers
#include "../unimap_worker.h"
#include "../httpengine/wxcurl_http.h"

// external classes
class CVCameraWorker;
class CDCameraWorker;
class CTelescopeWorker;

////////////////////////////////////////////////////////////////
// class:	CWaitWindow
class CWaitDialog: public wxDialog
{

public:
	CWaitDialog( wxWindow *parent, const wxString& strTitle, const wxString& strMsg, 
				int bLayout=0, void* pWorker=NULL, DefCmdAction* act=NULL, int nType=WAITDLG_WORKER_UNIMAP );
	~CWaitDialog( );
	
	void SetText( const wxString& msg );

	wxGauge*	m_pProgressBar;
	wxStaticText* m_pMessage;

	int m_nProgressVal;

	int m_nType;
	void* m_pWorker;
	CUnimapWorker*		m_pUnimapWorker;
	CVCameraWorker*		m_pVCameraWorker;
	CDCameraWorker*		m_pDCameraWorker;
	CTelescopeWorker*	m_pTelescopeWorker;

	DefCmdAction m_rWorkerAct;

private:
	void OnInitDlg( wxInitDialogEvent& pEvent );
	void OnCancelTask( wxCommandEvent& pEvent );
	// custom event handlers
	void OnEndProcess( wxCommandEvent& pEvent );
	void OnSetProgessStep( wxCommandEvent& pEvent );
	void OnSetMessage( wxCommandEvent& pEvent );
	void OnSetUpdate( wxCommandEvent& pEvent );
	void OnNotifyError( wxCommandEvent& pEvent );
	// event progress bar
	void OnUploadProgress( wxCurlProgressEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
