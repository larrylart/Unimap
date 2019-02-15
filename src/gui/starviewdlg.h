////////////////////////////////////////////////////////////////////
// Package:		Star View Dialog definition
// File:		starviewdlg.h
// Purpose:		create a dialog to show a star position description
//
// Created by:	Larry Lart on 16/09/2007
// Updated by:	Larry Lart on 29/01/2010 - clean
//
// Copyright:	(c) 2008-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _STARVIEWDLG_H
#define _STARVIEWDLG_H

#include "wx/wxprec.h"
#include <wx/notebook.h>
#include <wx/html/htmlwin.h>
#include <wx/html/htmlcell.h>
#include <wx/listctrl.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "../image/histogram.h"

// controls ids
enum
{
	wxID_SELECT_IDENTIFIERS_CAT		= 11000,
	wxID_SELECT_IDENTIFIERS_NAME,
	wxID_SELECT_REFERENCES_BIBCODE,
	wxID_SELECT_REFERENCES_VIEWFORMAT,
	wxID_BUTTON_REFERENCES_VIEWFORMAT_BUTTON,
	wxID_SELECT_IMAGE_SOURCE,
	wxID_SELECT_IMAGE_SURVEY,
	wxID_SELECT_IMAGE_COLOR_TABLE,
	wxID_SELECT_IMAGE_SURVEY_TYPE,
//	wxID_SET_OBJECT_REMOTE_IMAGE,
	wxID_SELECT_IMAGE_CATALOG_OVERLAY,
	wxID_SELECT_IMAGE_CONTOUR_OVERLAY,
	wxID_SELECT_IMAGE_GRID_OVERLAY,
	wxID_SELECT_IMAGE_CHANNEL_TYPE,
	wxID_SELECT_OBJECT_SYSTEM_COORD_TYPE,
	wxID_SELECT_MEASURE_NAME,
	wxID_SELECT_SPECTRUM_NAME
};

// some local headers
#include "../sky/star.h"

// external classes
class CConfigMain;
class CSky;
class CSimbad;
class CAstroImage;
class CUnimapWorker;
class wxPlotCtrl;
class CHistogramView;
class CImageHistogram;
class CRadialProfileView;
class CSkyObjSpectrum;
class CBibDb;
class CSkyviewSite;

// class:	CCustomHtmlWindow
///////////////////////////////////////////////////////
class CCustomHtmlWindow : public wxHtmlWindow
{
// public methods
public:
	CCustomHtmlWindow( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size );
//	virtual ~CCustomHtmlWindow( );

// private methods
private:
	virtual void OnLinkClicked( const wxHtmlLinkInfo& link );

};

// class:	CObjectView
///////////////////////////////////////////////////////
class CObjectView : public wxWindow
{
// public methods
public: 
	CObjectView( wxWindow *pWindow, const wxPoint& pos, const wxSize& size );
	virtual ~CObjectView( );

	// Draw method
	void	SetImage( wxImage& pImage );

// public data
public:
	int m_bSizeIsRunning;
	int m_bHasImage;
	int m_nWidth;
	int m_nHeight;
	int m_nOrigPicWidth;
	int m_nOrigPicHeight;
	int m_nPicWidth;
	int m_nPicHeight;
	double m_nScaleFactor;

// private methods
private:
	wxBitmap	m_pBitmap;
	// event handlers
	void	OnPaint(wxPaintEvent& event);
	void	OnSize( wxSizeEvent& even );

// protected data
protected:

	DECLARE_EVENT_TABLE()
};

// class:	CStarViewDlg
///////////////////////////////////////////////////////
class CStarViewDlg : public wxDialog
{
// public methods
public:
    CStarViewDlg( CSky* pSky, wxWindow *parent, const wxString& title, CUnimapWorker* pUnimapWorker );
	virtual ~CStarViewDlg( );

	void GetObjectData( );

	void DisplayObjectData( );

	void SetObjectData( CAstroImage *pAstroImage, wxImage &pImage, 
						StarDef* pObject, unsigned char nObjType );
	void SetObjectHistogram( );

	void SelectBibcode(  );
	void SelectMeasurement( );

	int GetRemoteImage( );
	void DisplayImage( );

	void SetImageSurveyType( wxString& strSelectType );

	void DisableRemoteImagePanel( );
	void EnableRemoteImagePanel( );
	
	void SelectSpectrum( );

// public data
public:

	CSky* m_pSky;
	CSimbad* m_pSimbad;
	CSkyObjSpectrum* m_pSkyObjSpectrum;
	CBibDb* m_pBibDb;
	CSkyviewSite* m_pSkyview;

	// object data
	int m_nObjWinWidth;
	int m_nObjWinHeight;

	// image reference
	CAstroImage* m_pAstroImage;
	wxImage m_pLocalImage;

	StarDef* m_pObject;
	CUnimapWorker* m_pUnimapWorker;

	// name stuff
	wxString	m_strObjectCatName;
	int		m_bHasName;
	
	unsigned char		m_nObjType;

	CImageHistogram		m_rHistogram;
	CHistogramView*		m_pImageHistogramView;
	CRadialProfileView*	m_pImageRadialProfileView;

	// my bitmap
	//wxStaticBitmap* m_pObjectView;
	CObjectView*	m_pObjView;

	wxFlexGridSizer* m_pSizerHeadDataTop;
	wxNotebook* m_pNotebook;
	wxPanel* m_pPanelDetails;
	wxPanel* m_pPanelIdentifiers;
	wxPanel* m_pPanelReferences;
	wxPanel* m_pPanelMeasures;
	wxPanel* m_pPanelNotes;
	wxPanel* m_pPanelSpectrum;
	wxPanel* m_pPanelLocal;
	wxPanel* m_pPanelHist;
	wxPanel* m_pPanelRadial;
	// panel flags
	int m_bPanelDetails;
	int m_bPanelIdentifiers;
	int m_bPanelReferences;
	int m_bPanelMeasures;
	int m_bPanelNotes;
	int m_bPanelSpectrum;
	int m_bPanelLocal;
	int m_bPanelHist;
	int m_bPanelRadial;

	// header image
	wxStaticBoxSizer* m_pImgDataSurveyBoxSizer;
	wxGridSizer*	m_pSizerHeadData;
	wxChoice*		m_pUseImageSource;
	wxChoice*		m_pRemImageChannel;
	// one channel image
	wxStaticBox*	m_pImgDataSurveyBox;
	wxFlexGridSizer* m_pSizerHeadDataSurvey;
	wxChoice*		m_pRemImageType;
	wxChoice*		m_pRemImageSurvey;

	// surveys 
	wxChoice*		m_pRemImageColorTable;

	// grid overlay
	wxStaticBox*		m_pImgDataOverlayBox;
	wxFlexGridSizer*	m_pSizerHeadDataOverlay;
	wxChoice*			m_pRemImageGridOverlay;
	// survey from contour
	wxChoice*		m_pRemImageContourOverlay;
	// catalog overlay vars
	wxChoice*		m_pRemImageCatalogOverlay;
	// rgb composition
	wxStaticBox*	m_pImgDataRgbBox;
	wxFlexGridSizer* m_pSizerHeadDataRgb;
	wxChoice*		m_pRemImageRGBCompRed;
	wxChoice*		m_pRemImageRGBCompGreen;
	wxChoice*		m_pRemImageRGBCompBlue;
	wxStaticBoxSizer* m_pImgDataRgbBoxSizer;

	/////////////////////////////
	// object details data
	// coord
	wxChoice*		m_pCoordType;
	wxStaticText*	m_pCoordValue;

	// other
	wxStaticText*	m_pName;
	wxStaticText*	m_pType;
	wxStaticText*	m_pProperMotions;
	wxStaticText*	m_pParallax;
	wxStaticText*	m_pRadialVelocityLabel;
	wxStaticText*	m_pRadialVelocity;
	wxStaticText*	m_pSpectralType;
	wxStaticText*	m_pRedshift;
	wxStaticText*	m_pMorphologicalType;
	wxStaticText*	m_pAngularSize;
	wxStaticText*	m_pFluxB;
	wxStaticText*	m_pFluxV;
	wxStaticText*	m_pFluxJ;
	wxStaticText*	m_pFluxH;
	wxStaticText*	m_pFluxK;

	/////////////////////////////////
	// idetifieres
//	wxChoice*	m_pIdentifiersCat;
//	wxChoice*	m_pIdentifiersName;
	wxFlexGridSizer* m_pIdentifiersDataSizer;
	wxGridSizer* pReferencesSizer;

	// references vars
	wxStaticText* m_pRefTitle;
	wxStaticText* m_pRefJournal;
	wxStaticText* m_pRefAuthor;
	wxHtmlWindow* m_pRefSummary;
	wxChoice*	m_pRefBibcode;
	wxChoice*	m_pRefViewFormat;
	wxButton*	m_pRefViewFormatButton;


	// other data

	//////////////
	// measures
	wxChoice*	m_pMeasuresName;
	wxListCtrl* m_pMeasuresTable;

	///////////////
	// notes
	CCustomHtmlWindow*	m_pNotesWin;

	////////////////
	// local 
	wxStaticText*	m_pObjectPosX;
	wxStaticText*	m_pObjectPosY;
	wxStaticText*	m_pObjectCoordRa;
	wxStaticText*	m_pObjectCoordDec;
	wxStaticText*	m_pObjectMag;
	wxStaticText*	m_pObjectKron;
	wxStaticText*	m_pObjectEllipseWidth;
	wxStaticText*	m_pObjectEllipseHeight;
	wxStaticText*	m_pObjectEllipseAngle;
	wxStaticText*	m_pObjectFwhm;

	//////////////////////
	// spectrum
	wxPlotCtrl*		m_pSpectrumView;
	wxChoice*		m_pSpectrumList;

// private methods
private:
	void OnSelectImageSource( wxCommandEvent& event );
	void OnSelectImageChannelType( wxCommandEvent& event );

	void OnSelectImageSurvey( wxCommandEvent& event );
	void OnSelectImageSurveyType( wxCommandEvent& event );
	void OnSelectCatalogOverlay( wxCommandEvent& event );

	void OnSelectCoordType( wxCommandEvent& event );
	void OnSelectBibcode( wxCommandEvent& event );
	void OnViewReferenceDoc( wxCommandEvent& event );
	void OnSelectMeasurement( wxCommandEvent& event );
	void OnSelectSpectrum( wxCommandEvent& event );

	// from custom event
	void SetRemoteImage( );

	DECLARE_EVENT_TABLE()

};


#endif
