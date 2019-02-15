////////////////////////////////////////////////////////////////////
// Name:		Wait Dialog Stack Header
// File:		waitdialogstack.h
// Purpose:		defines a wait dialog window for the stacking process
//
// Created by:	Larry Lart on 16-12-2008
// Updated by:	Larry Lart on 31/01/2010
//
// Copyright:	(c) 2007-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _WAIT_DIALOG_STACK_H
#define _WAIT_DIALOG_STACK_H

// wxWindows includes
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/tglbtn.h>

// local defines

// define statuses
#define WAIT_DIALOG_STACK_STATUS_CANCEL	1
#define WAIT_DIALOG_STACK_STATUS_PAUSE	2
#define WAIT_DIALOG_STACK_STATUS_STOP	3
#define WAIT_DIALOG_STACK_STATUS_SKIP	4

// ids
enum
{
	wxID_BUTTON_CANCEL_STACK	= 22600,
	wxID_BUTTON_PAUSE_STACK		= 22601,
	wxID_BUTTON_STOP_STACK		= 22602,
	wxID_BUTTON_SKIP_STACK		= 22603
};


// used classes
class CWaitDialogStack;
class CUnimapWorker;

// class:	CSkyScanView
///////////////////////////////////////////////////////
class CStackImageView : public wxWindow
{
// public methods
public: 
	CStackImageView( wxWindow *pWindow, const wxPoint& pos, const wxSize& size );
	virtual ~CStackImageView( );

	// methods
	void	SetImageView( wxImage* pImage );

// public data
public:
	int m_bSizeIsRunning;
	int m_bHasImage;

	int m_nWidth;
	int m_nHeight;

	int m_bDrawing;

// private methods
private:
	CWaitDialogStack*	m_pParent;
	wxBitmap			m_pBitmap;
	wxBitmap			m_pBitmapSky;
	// event handlers
	void	OnPaint(wxPaintEvent& event);
	void	OnSize( wxSizeEvent& even );

// protected data
protected:

	DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////
// class:	CWaitDialogStack
class CWaitDialogStack: public wxDialog
{
// publice methods
public:
	CWaitDialogStack( wxWindow *parent, wxString& strTitle, wxString& strMsg, 
						CUnimapWorker* pWorker, void* pTargetObject );
	~CWaitDialogStack( );

	void SetImage( wxImage* pImage );
	void SetMethod( const wxString& strMethod );
	void SetPixelStatus( int x, int y, float value );
	void SetEta( double nEta );
	void SetImageSize( int w, int h );

// publid data
public:
	void* m_pTargetObject;
	CUnimapWorker*	m_pUnimapWorker;

	wxString		m_strTitle;

	CStackImageView*	m_pStackImageView;
	wxStaticText*		m_pMessage;
	// data fields
	wxStaticText*	m_pPixX;
	wxStaticText*	m_pPixY;
	wxStaticText*	m_pCurrentValue;
	wxStaticText*	m_pStackMethod;
	wxStaticText*	m_pWidth;
	wxStaticText*	m_pHeight;
	wxStaticText*	m_pRemainingTime;

	// action buttons
	wxButton*		m_pCancel;
	wxToggleButton*	m_pPause;
	wxButton*		m_pStop;
	wxButton*		m_pSkip;
	// flags 
	int				m_bImageUpdate;
	int				m_bUpdating;
	int				m_bActionStatus;

// private methods
private:
	void OnInitDlg( wxInitDialogEvent& pEvent );
	void OnStatusButton( wxCommandEvent& even );
	void OnSetMessage( wxCommandEvent& pEvent );
	void OnEndProcess( wxCommandEvent& pEvent );

// protected data
protected:

	DECLARE_EVENT_TABLE()
};

#endif
