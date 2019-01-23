////////////////////////////////////////////////////////////////////
// Package:		Main config view Dialog definition
// File:		mainconfigview.h
// Purpose:		create a dialog to configure the main parameters
//
// Created by:	Larry Lart on 12-07-2007
// Updated by:	Larry Lart on 13-01-2009
//
// Copyright:	(c) 2007-2009 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _MAINCONFIGVIEW_H
#define _MAINCONFIGVIEW_H

// system libs
#include <stdio.h>
#include <vector>

// wx includes
#include "wx/wxprec.h"
//#include <wx/intl.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
// object label types
#define LABEL_TYPE_DETECTED_OBJECT		0
#define LABEL_TYPE_MATCHED_OBJECT		1
#define LABEL_TYPE_CATALOG_OBJECT		2
#define LABEL_TYPE_CATALOG_NEBULA		3
#define LABEL_TYPE_CATALOG_CLUSTER		4
#define LABEL_TYPE_CATALOG_GALAXY		5
#define LABEL_TYPE_CATALOG_SUPERNOVAE	6
#define LABEL_TYPE_CATALOG_RADIO		7
#define LABEL_TYPE_CATALOG_XRAY			8
#define LABEL_TYPE_CATALOG_GAMMA		9
#define LABEL_TYPE_CATALOG_BLACKHOLE	10
#define LABEL_TYPE_CATALOG_MSTARS		11
#define LABEL_TYPE_CATALOG_AST			12
#define LABEL_TYPE_CATALOG_COM			13
#define LABEL_TYPE_CATALOG_EXP			14
#define LABEL_TYPE_CATALOG_AES			15

// graphics design types
#define GRAPHICS_TYPE_OBJECT_PATH			0
#define GRAPHICS_TYPE_SKY_GRID				1
#define GRAPHICS_TYPE_DISTORTION_GRID		2
#define GRAPHICS_TYPE_CONSTELLATION_LINES	3

// controls ids
enum
{
	wxID_BUTTON_LABEL_FONT_COLOR	= 16000,
	wxID_SELECT_UNITS_FORMAT,
	wxID_SELECT_COORDINATE_SYSTEM,
	wxID_SELECT_LABEL_TYPE,
	// label structure
	wxID_CHECK_USE_ID,
	wxID_CHECK_USE_CATNO,
	wxID_CHECK_USE_CNAME,
	wxID_CHECK_USE_TYPE,
	wxID_CHECK_USE_MAG,
	wxID_SELECT_SYSTEM_EPOCH,
	wxID_SELECT_GUI_LANGUAGE,
	wxID_SELECT_OBJECT_SHAPE_TYPE,
	wxID_SELECT_OBJECT_SHAPE_LINE,
	// graphics
	wxID_SELECT_GRAPHICS_TYPE,
	wxID_SELECT_GRAPHICS_LINE_STYLE,
	wxID_SELECT_GRAPHICS_LINE_COLOUR,
	// props
	wxID_OBJ_PATH_DRAW_VERTICES	,
	wxID_OBJ_PATH_DRAW_STARTEND,
	wxID_OBJ_PATH_DRAW_DIRECTION,
	wxID_OBJ_PATH_DRAW_BASE_LINES,
	wxID_SKY_GRID_LABELS,
	wxID_SKY_GRID_SPLINE,
	wxID_SELECT_CONSTELLATION_TYPE,
	wxID_SKY_CONSTELLATION_SPLINE,
	wxID_SKY_CONSTELLATION_MARK_STARS,
	wxID_SKY_CONSTELLATION_DRAW_OUTLINE,
	wxID_SKY_CONSTELLATION_GREEK_LABELS,
	// temporary
	wxID_BUTTON_TEMPORARY_LIST,
	wxID_BUTTON_TEMPORARY_CLEAN,
	wxID_BUTTON_TEMPORARY_QUOTA,
	wxID_BUTTON_TEMPORARY_EXPLORE
};

// external classes
class CConfigMain;
class CGUIFrame;
class CFilesManager;
class CUnimapWorker;

// class:	CMainConfigView
///////////////////////////////////////////////////////
class CMainConfigView : public wxDialog
{
// public methods
public:
    CMainConfigView( wxWindow *parent );
	virtual ~CMainConfigView( );

	void SetConfig( CConfigMain* pMainConfig, CGUIFrame* pFrame );
	void SetGraphicsType( int nType );
	void GetGraphicsType( int nType );

	void SetTemporaryList( );

// public data
public:

	CFilesManager* m_pFileManager;
	CUnimapWorker* m_pUnimapWorker;

	wxFlexGridSizer* sizerObjLabels;
	wxFlexGridSizer* sizerGraphics;
	wxGridSizer* sizerFormats;
	wxFlexGridSizer* sizerGraphicsStruct;

	wxChoice*		m_pObjectShapeType;
	wxChoice*		m_pObjectShapeLine;
	wxChoice*		m_pLabelType;
	wxButton*		m_pLabelFontColor;
	wxStaticText*	m_pLabelSampleText;

	///////////////////////
	// graphics
	int				m_nGraphicsPrevType;
	wxChoice*		m_pGraphicsType;
	wxChoice*		m_pGraphicsLineStyle;
	wxButton*		m_pGraphicsLineColour;
	wxStaticText*	m_pGraphicsLineColourSample;
	// object path other props
	wxCheckBox*		m_pGraphicsPathVertex;
	wxCheckBox*		m_pGraphicsPathStartEnd;
	wxCheckBox*		m_pGraphicsPathDirection;
	wxCheckBox*		m_pGraphicsPathBaseLines;
	// sky grid other props
	wxBoxSizer*		sizerGridRaDiv;
	wxBoxSizer*		sizerGridDecDiv;
	wxCheckBox*		m_pSkyGridShowLabels;
	wxCheckBox*		m_pGridDrawSplines;
	wxTextCtrl*		m_pGridRaDivValue;
	wxTextCtrl*		m_pGridDecDivValue;
	// dist grid props
	wxBoxSizer*		sizerGridBumpFact;
	wxTextCtrl*		m_pDistGridBumpFactValue;
	// constellation props 
	wxCheckBox*		m_pConstellationMarkStars;
	wxCheckBox*		m_pConstellationOutLine;
	wxCheckBox*		m_pConstellationGreekLabels;

	//////////////
	// graphics colors,etc
	// :: object path
	wxColour		colGraphicsObjectPath;
	int				m_nGraphicsObjectPathLine;
	int				m_bGraphicsPathVertex;
	int				m_bGraphicsPathStartEnd;
	int				m_bGraphicsPathDirection;
	int				m_bGraphicsPathBaseLines;
	// :: sky grid
	wxColour		colGraphicsSkyGrid;
	int				m_nGraphicsSkyGridLine;
	int				m_bSkyGridShowLabels;
	int				m_bSkyGridDrawSplines;
	double			m_nSkyGridRaDivValue;
	double			m_nSkyGridDecDivValue;
	// :: distorsion grid
	wxColour		colGraphicsDistortionGrid;
	int				m_nGraphicsDistortionGridLine;
	int				m_bDistGridDrawSplines;
	double			m_nDistGridRaDivValue;
	double			m_nDistGridDecDivValue;
	double			m_nDistGridBumpFactValue;
	// :: constellations
	wxColour		colGraphicsConstellationLines;
	int				m_nGraphicsConstellationLinesLine;
	int				m_bConstellationMarkStars;
	int				m_bConstellationOutLine;
	int				m_bConstellationGreekLabels;

	////////////
	// label structure
	wxCheckBox*		m_pLabelStructId;
	wxCheckBox*		m_pLabelStructCatNo;
	wxCheckBox*		m_pLabelStructCNames;
	// object type
	wxCheckBox*		m_pLabelStructType;
	// other detected objects fields
	wxCheckBox*		m_pLabelStructMag;

	////////////////
	// FORMATS
	wxChoice*		m_pSkyCoordDisplayFormat;
	wxChoice*		m_pSkyCoordEntryFormat;
	wxChoice*		m_pCoordSystem;
	wxChoice*		m_pSystemEpoch;
	wxChoice*		m_pGuiLanguage;
	wxChoice*		m_pGuiTheme;

	/////////////
	// AUTOS
	wxCheckBox*		m_pSaveOnDetect;
	wxCheckBox*		m_pSaveOnMatch;
	wxCheckBox*		m_pSaveOnDetails;
	wxCheckBox*		m_pSaveOnAutos;
	wxCheckBox*		m_pHideOnMinimize;

	/////////////
	// SERVICES
	wxCheckBox*		m_pEnableSoapServices;
	wxTextCtrl*		m_pSoapServerIp;
	wxTextCtrl*		m_pSoapServerPort;

	/////////////
	// TEMPORARY
	wxListCtrl*		m_pTemporaryList;
	wxButton*		m_pTemporaryDelete;
	wxButton*		m_pTemporaryQuota;
	wxButton*		m_pTemporaryExplore;

	CConfigMain*	m_pMainConfig;
	CGUIFrame*		m_pFrame;
//	wxLocale*		m_pLocale;

	/////////////////////
	// font and colors by type
	// :: detected
	wxFont			fntDObjLabel;
	wxColour		colDObjLabel;
	// :: matched
	wxFont			fntMObjLabel;
	wxColour		colMObjLabel;
	// :: catalog
	wxFont			fntCObjLabel;
	wxColour		colCObjLabel;
	// :: nebula
	wxFont			fntNebObjLabel;
	wxColour		colNebObjLabel;
	// :: cluster
	wxFont			fntCluObjLabel;
	wxColour		colCluObjLabel;
	// :: galaxy
	wxFont			fntGalObjLabel;
	wxColour		colGalObjLabel;
	// :: supernovae
	wxFont			fntSNObjLabel;
	wxColour		colSNObjLabel;
	// :: radio
	wxFont			fntRadObjLabel;
	wxColour		colRadObjLabel;
	// :: x-ray
	wxFont			fntXRayObjLabel;
	wxColour		colXRayObjLabel;
	// :: gamma
	wxFont			fntGammaObjLabel;
	wxColour		colGammaObjLabel;
	// :: black holes
	wxFont			fntBHObjLabel;
	wxColour		colBHObjLabel;
	// :: multiple/double stars
	wxFont			fntMSObjLabel;
	wxColour		colMSObjLabel;
	// :: asteroids
	wxFont			fntASTObjLabel;
	wxColour		colASTObjLabel;
	// :: comets
	wxFont			fntCOMObjLabel;
	wxColour		colCOMObjLabel;
	// :: extrasolar planets
	wxFont			fntEXPObjLabel;
	wxColour		colEXPObjLabel;
	// :: artificial earth satellites
	wxFont			fntAESObjLabel;
	wxColour		colAESObjLabel;

	/////////////////////
	// shape and line type
	// :: detected
	int m_nDetectObjShapeType;
	int m_nDetectObjShapeLine;
	// :: matched
	int m_nMatchObjShapeType;
	int m_nMatchObjShapeLine;
	// :: catalog star
	int m_nCatObjShapeType;
	int m_nCatObjShapeLine;
	// :: nebula
	int m_nNebObjShapeType;
	int m_nNebObjShapeLine;
	// :: cluster
	int m_nCluObjShapeType;
	int m_nCluObjShapeLine;
	// :: galaxy
	int m_nGalObjShapeType;
	int m_nGalObjShapeLine;
	// :: Supernovae
	int m_nSNObjShapeType;
	int m_nSNObjShapeLine;
	// :: radio
	int m_nRadObjShapeType;
	int m_nRadObjShapeLine;
	// :: x-ray
	int m_nXRayObjShapeType;
	int m_nXRayObjShapeLine;
	// :: gamma
	int m_nGammaObjShapeType;
	int m_nGammaObjShapeLine;
	// :: black holes
	int m_nBHObjShapeType;
	int m_nBHObjShapeLine;
	// :: multiple/double stars
	int m_nMSObjShapeType;
	int m_nMSObjShapeLine;
	// :: asteroids
	int m_nASTObjShapeType;
	int m_nASTObjShapeLine;
	// :: comets
	int m_nCOMObjShapeType;
	int m_nCOMObjShapeLine;
	// :: extrasolar planets
	int m_nEXPObjShapeType;
	int m_nEXPObjShapeLine;
	// :: artificial earth satellites
	int m_nAESObjShapeType;
	int m_nAESObjShapeLine;

	/////////////////////////////
	// label structure flag
	// :: detected
	int m_bDetectObjLabelUseId;
	int m_bDetectObjLabelUseMag;
	// :: matched
	int m_bMatchObjLabelUseId;
	int m_bMatchObjLabelUseCatNo;
	int m_bMatchObjLabelUseCName;
	// :: catalog
	int m_bCatObjLabelUseCatNo;
	int m_bCatObjLabelUseCName;
	// :: nebula
	int m_bNebObjLabelUseCatNo;
	int m_bNebObjLabelUseCName;
	int m_bNebObjLabelUseType;
	// :: cluster
	int m_bCluObjLabelUseCatNo;
	int m_bCluObjLabelUseCName;
	int m_bCluObjLabelUseType;
	// :: galaxy
	int m_bGalObjLabelUseCatNo;
	int m_bGalObjLabelUseCName;
	int m_bGalObjLabelUseType;
	// :: supernovae
	int m_bSNObjLabelUseCatNo;
	int m_bSNObjLabelUseCName;
	int m_bSNObjLabelUseType;
	// :: radio
	int m_bRadObjLabelUseCatNo;
	int m_bRadObjLabelUseType;
	// :: x-ray
	int m_bXRayObjLabelUseCatNo;
	int m_bXRayObjLabelUseType;
	// :: gamma
	int m_bGammaObjLabelUseCatNo;
	int m_bGammaObjLabelUseType;
	// :: black holes
	int m_bBHObjLabelUseCatNo;
	int m_bBHObjLabelUseType;
	// :: multiple/double stars
	int m_bMSObjLabelUseCatNo;
	int m_bMSObjLabelUseType;
	// :: asteroids
	int m_bASTObjLabelUseCatNo;
	int m_bASTObjLabelUseType;
	// :: comets
	int m_bCOMObjLabelUseCatNo;
	int m_bCOMObjLabelUseType;
	// :: extrasolar planets
	int m_bEXPObjLabelUseCatNo;
	int m_bEXPObjLabelUseType;
	// :: artificial earth satellites
	int m_bAESObjLabelUseCatNo;
	int m_bAESObjLabelUseType;

	int m_nGuiLanguage;

// private methods
private:
	void OnNotebookTab( wxNotebookEvent& pEvent );
	// object design
	void OnLabelStyle( wxCommandEvent& event );
	void OnLabelType( wxCommandEvent& event );
	void OnUseStruct( wxCommandEvent& event );
	void OnObjectShapeAndLine( wxCommandEvent& event );
	// graphics design
	void OnGraphicsColour( wxCommandEvent& event );
	void OnGraphicsType( wxCommandEvent& event );
	void OnGraphicsLine( wxCommandEvent& event );
	// formats
	void OnSetGuiLanguage( wxCommandEvent& event );
	// list selection
	void OnTempListSelect( wxListEvent& pEvent );
	void OnTempEmpty( wxCommandEvent& pEvent );
	void OnTempQuota( wxCommandEvent& pEvent );
	void OnTempExplore( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()

};


#endif
