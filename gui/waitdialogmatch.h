////////////////////////////////////////////////////////////////////
// Name:		Wait Dialog Match Header
// File:		waitdialogmatch.h
// Purpose:		defines a wait dialog window for the matching process
//
// Created by:	Larry Lart on 26/06/2008
// Updated by:	Larry Lart on 31/01/2010
//
// Copyright:	(c) 2007-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _WAIT_DIALOG_MATCH_H
#define _WAIT_DIALOG_MATCH_H

// wxWindows includes
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/tglbtn.h>

// local defines
//#define GRID_HORIZONTAL_MARGIN		20
//#define GRID_VERTICAL_MARGIN			15
#define MAX_NO_OF_WAIT_DIALOG_MATCH_OBJ	1000	
// define statuses
#define WAIT_DIALOG_MATCH_STATUS_CANCEL	1
#define WAIT_DIALOG_MATCH_STATUS_PAUSE	2
#define WAIT_DIALOG_MATCH_STATUS_STOP	3
#define WAIT_DIALOG_MATCH_STATUS_SKIP	4

// ids
enum
{
	wxID_BUTTON_CANCEL_MATCH	= 22100,
	wxID_BUTTON_PAUSE_MATCH		= 22101,
	wxID_BUTTON_STOP_MATCH		= 22102,
	wxID_BUTTON_SKIP_MATCH		= 22103
};

// include locals
#include "../sky/star.h"

// used classes
class CWaitDialogMatch;
class CAstroImage;
class CUnimapWorker;

// class:	CSkyScanView
///////////////////////////////////////////////////////
class CSkyScanView : public wxWindow
{
// public methods
public: 
	CSkyScanView( wxWindow *pWindow, const wxPoint& pos, const wxSize& size );
	virtual ~CSkyScanView( );

	// Draw method
	void	AddObject( int nX, int nY, unsigned char nState );
	void	DrawObjects( );
	void	DrawImageArea( );
	void	DrawScanView( );

// public data
public:
	wxPoint	m_vectObjects[MAX_NO_OF_WAIT_DIALOG_MATCH_OBJ];
	unsigned char	m_vectObjectsState[MAX_NO_OF_WAIT_DIALOG_MATCH_OBJ];
	int		m_nObjects;

	int m_bSizeIsRunning;
	int m_bHasImage;

	int m_nWidth;
	int m_nHeight;

	int m_bDrawing;

// private methods
private:
	CWaitDialogMatch*	m_pParent;
	wxBitmap			m_pBitmap;
	wxBitmap			m_pBitmapSky;
	wxBitmap			m_pBitmapTarget;
	// event handlers
	void	OnPaint(wxPaintEvent& event);
	void	OnSize( wxSizeEvent& even );

// protected data
protected:

	DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////
// class:	CWaitDialogMatch
class CWaitDialogMatch: public wxDialog
{
// publice methods
public:
	CWaitDialogMatch( wxWindow *parent, const wxString& strTitle, const wxString& strMsg,
						CUnimapWorker* pWorker, void* pTargetObject, int bGroup=0 );
	~CWaitDialogMatch( );

	void SetCatalog( const wxString& strCat );

	void SetField( double nOrigRa, double nOrigDec, double nWidth, double nHeight );
	void SetTarget( double x, double y, double w, double h, int nObj );
	void SetFound( int nFound );
	void SetBestMatch( int nFound, int nFoundAll );
	void SetMostMatched( int nFound );
	void SetObjects( StarDef* vectStar, int nStar, double nMinX, double nMaxX, double nMinY, double nMaxY, double nRemScale );
	void SetImageAreaFactor( double nXRatio, double nYRatio );
	void SetBestTarget( double x, double y, double w, double h );

// publid data
public:
	void*	m_pTargetObject;
	CUnimapWorker*	m_pUnimapWorker;

	// flags
	int m_bGroup;

	wxString		m_strTitle;

	CSkyScanView*	m_pSkyScanView;
	wxStaticText*	m_pMessage;
	// data fields
	wxStaticText*	m_pCatalog;
	wxStaticText*	m_pRa;
	wxStaticText*	m_pDec;
	wxStaticText*	m_pCutWidth;
	wxStaticText*	m_pCutHeight;
	wxStaticText*	m_pCutObjects;
	wxStaticText*	m_pFoundBest;
	wxStaticText*	m_pFoundBestAll;
	wxStaticText*	m_pCurrentFound;
	wxStaticText*	m_pMostMatched;

	// action buttons
	wxButton*		m_pCancel;
	wxToggleButton*	m_pPause;
	wxButton*		m_pStop;
	wxButton*		m_pSkip;
	// flags 
	int				m_bScanUpdate;
	int				m_bFoundBestTarget;
	int				m_bNewObjects;
	int				m_bImageArea;
	int				m_bActionStatus;
	int				m_bUpdating;

	// field orig/size
	double			m_nOrigRa;
	double			m_nOrigDec;
	double			m_nFieldWidth;
	double			m_nFieldHeight;
	// current cut pos/size
	double			m_nOrigX;
	double			m_nOrigY;
	double			m_nCutWidth;
	double			m_nCutHeight;
	// best target
	int				m_nBestTargetX;
	int				m_nBestTargetY;
	int				m_nBestTargetWidth;
	int				m_nBestTargetHeight;
	// image area
	int				m_nImageAreaX;
	int				m_nImageAreaY;
	int				m_nImageAreaWidth;
	int				m_nImageAreaHeight;

// private methods
private:
	void OnInitDlg( wxInitDialogEvent& pEvent );
	void OnStatusButton( wxCommandEvent& even );

// protected data
protected:
	void OnSetTitle( wxCommandEvent& pEvent );
	void OnSetMessage( wxCommandEvent& pEvent );
	void OnEndProcess( wxCommandEvent& pEvent );
	void OnSetUpdate( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
