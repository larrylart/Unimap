////////////////////////////////////////////////////////////////////
// Package:		Main Config View Dialog implementation
// File:		mainconfigview.cpp
// Purpose:		create dialog to set main configurations params
//
// Created by:	Larry Lart on 12/07/2007
// Updated by:	Larry Lart on 06/02/2010 - remove fetch objects
//
// Copyright:	(c) 2007-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

// system libs
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/wxprec.h"
#include <wx/dialog.h>
#include "wx/statline.h"
#include "wx/minifram.h"
#include "wx/thread.h"
#include "wx/notebook.h"
#include <wx/fontdlg.h>
#include <wx/colordlg.h>
#include <wx/listctrl.h>
#include <wx/dir.h>
#include <wx/app.h>
#include <wx/filename.h>
#include <wx/spinctrl.h>

// local headers
#include "../util/func.h"
#include "../util/files_manager.h"
#include "../config/mainconfig.h"
#include "frame.h"
#include "waitdialog.h"
#include "../unimap.h"
#include "../unimap_worker.h"
#include "simple_common_dlg.h"

// main header
#include "mainconfigview.h"

// get external unimap
DECLARE_APP(CUnimap)

////////////////////////////////////////////////////////////////////
// CMainConfigView EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CMainConfigView, wxDialog )
	// on notebook page changed
	EVT_NOTEBOOK_PAGE_CHANGED( -1, CMainConfigView::OnNotebookTab )
	// object design
	EVT_BUTTON( wxID_BUTTON_LABEL_FONT_COLOR, CMainConfigView::OnLabelStyle )
	EVT_CHOICE( wxID_SELECT_LABEL_TYPE, CMainConfigView::OnLabelType )
	EVT_CHECKBOX( wxID_CHECK_USE_ID, CMainConfigView::OnUseStruct )
	EVT_CHECKBOX( wxID_CHECK_USE_CATNO, CMainConfigView::OnUseStruct )
	EVT_CHECKBOX( wxID_CHECK_USE_CNAME, CMainConfigView::OnUseStruct )
	EVT_CHECKBOX( wxID_CHECK_USE_TYPE, CMainConfigView::OnUseStruct )
	EVT_CHECKBOX( wxID_CHECK_USE_MAG, CMainConfigView::OnUseStruct )
	// shape and line
	EVT_CHOICE( wxID_SELECT_OBJECT_SHAPE_TYPE, CMainConfigView::OnObjectShapeAndLine )
	EVT_CHOICE( wxID_SELECT_OBJECT_SHAPE_LINE, CMainConfigView::OnObjectShapeAndLine )
	// graphics design
	EVT_BUTTON( wxID_SELECT_GRAPHICS_LINE_COLOUR, CMainConfigView::OnGraphicsColour )
	EVT_CHOICE( wxID_SELECT_GRAPHICS_TYPE, CMainConfigView::OnGraphicsType )
	EVT_CHOICE( wxID_SELECT_GRAPHICS_LINE_STYLE, CMainConfigView::OnGraphicsLine )
	// formats
	EVT_CHOICE( wxID_SELECT_GUI_LANGUAGE, CMainConfigView::OnSetGuiLanguage )
	// temporary folders list selection
	EVT_LIST_ITEM_DESELECTED( wxID_BUTTON_TEMPORARY_LIST, CMainConfigView::OnTempListSelect )
	EVT_LIST_ITEM_SELECTED( wxID_BUTTON_TEMPORARY_LIST, CMainConfigView::OnTempListSelect )
	EVT_LIST_ITEM_ACTIVATED( wxID_BUTTON_TEMPORARY_LIST, CMainConfigView::OnTempListSelect )
	EVT_BUTTON( wxID_BUTTON_TEMPORARY_CLEAN, CMainConfigView::OnTempEmpty )
	EVT_BUTTON( wxID_BUTTON_TEMPORARY_QUOTA, CMainConfigView::OnTempQuota )
	EVT_BUTTON( wxID_BUTTON_TEMPORARY_EXPLORE, CMainConfigView::OnTempExplore )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
////////////////////////////////////////////////////////////////////
CMainConfigView::CMainConfigView(wxWindow *parent)
               : wxDialog(parent, -1,
                          _("UniMap Options"),
                          wxDefaultPosition,
                          wxSize( 760, 440 ),
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	// get references to worker and online objects
	CUnimap* pUnimap = &(wxGetApp());
	m_pUnimapWorker = pUnimap->m_pUnimapWorker;

	m_pMainConfig = NULL;
	m_nGraphicsPrevType = -1;
	m_nGuiLanguage = 0;
	m_pFileManager = new CFilesManager();
	
	wxString vectObjectShapeType[6];
	wxString vectObjectShapeLine[3];
	wxString vectLabelType[20];
	wxString vectUnitsFormat[2];
	wxString vectCoordSystem[2];
	wxString vectSystemEpoch[2];
	wxString vectGraphicsType[4];
	wxString vectGraphicsLine[3];
	wxString vectConstellationType[2];

	// object shape types
	vectObjectShapeType[0] = _T("Real Ellipse");
	vectObjectShapeType[1] = _T("Circle Icon");
	vectObjectShapeType[2] = _T("Square Icon");
	vectObjectShapeType[3] = _T("Triangle Icon");
	vectObjectShapeType[4] = _T("Cross Icon");
	vectObjectShapeType[5] = _T("Target Icon");
	// object shape line
	vectObjectShapeLine[0] = _T("Normal");
	vectObjectShapeLine[1] = _T("Dotted");
	vectObjectShapeLine[2] = _T("None");

	// label types
	vectLabelType[0] = _T("Detected Object");
	vectLabelType[1] = _T("Matched Object");
	vectLabelType[2] = _T("Catalog Object");
	vectLabelType[3] = _T("Catalog Nebulaes");
	vectLabelType[4] = _T("Catalog Clusters");
	vectLabelType[5] = _T("Catalog Galaxies");
	vectLabelType[6] = _T("Catalog Supernovae");
	vectLabelType[7] = _T("Catalog Radio Sources");
	vectLabelType[8] = _T("Catalog X-Ray Sources");
	vectLabelType[9] = _T("Catalog Gamma Sources");
	vectLabelType[10] = _T("Catalog Black Holes");
	vectLabelType[11] = _T("Multiple/Double Stars");
	vectLabelType[12] = _T("Solar System Asteroids");
	vectLabelType[13] = _T("Solar System Comets");
	vectLabelType[14] = _T("Extrasolar Planets");
	vectLabelType[15] = _T("Artificial Earth Satellites");

	// graphics type
	vectGraphicsType[0] = _T("Objects Path");
	vectGraphicsType[1] = _T("Sky Grid");
	vectGraphicsType[2] = _T("Distortion Grid");
	vectGraphicsType[3] = _T("Constellation Lines");
	// graphics line
	vectGraphicsLine[0] = _T("Normal");
	vectGraphicsLine[1] = _T("Dotted");
	vectGraphicsLine[2] = _T("Short Dash");
	// constellation modes
	vectConstellationType[0] = _T("IAU");
	vectConstellationType[1] = _T("H.A.Rey");

	// ra/dec entry type
	vectUnitsFormat[0] = _T("Sexagesimal");
	vectUnitsFormat[1] = _T("Decimal");
	// coord system ra/dec - alt/az
	vectCoordSystem[0] = _T("Ra/Dec");
//	vectCoordSystem[1] = _T("Alt/Az");
	// set system ecpochs
	vectSystemEpoch[0] = _T("J2000.0");
	// themes
	wxString vectTheme[] = { _T("Default") };

	// main dialog sizer
	wxFlexGridSizer *pTopSizer = new wxFlexGridSizer( 2, 1, 5, 5 );
	//wxBoxSizer *pTopSizer = new wxBoxSizer( wxVERTICAL );

	// building configure as a note book
	wxNotebook* pNotebook = new wxNotebook( this, -1, wxPoint(-1,-1), wxSize( 400, 240 ) ); 
	// add objects layout panel
	wxPanel* pPanelObjLayout = new wxPanel( pNotebook );
	// add Graphics panel
	wxPanel* pPanelGraphics = new wxPanel( pNotebook );
	// add formats panel
	wxPanel* pPanelFormats = new wxPanel( pNotebook );
	// add autos panel
	wxPanel* pPanelAutos = new wxPanel( pNotebook );
	// add services panel
	wxPanel* pPanelServices = new wxPanel( pNotebook );
	// add temporary panel
	wxPanel* pPanelTemp = new wxPanel( pNotebook );

	/////////////////////////////
	// Object Labes sizer - layout of elements
	sizerObjLabels = new wxFlexGridSizer( 3, 2, 10, 5 );
	sizerObjLabels->AddGrowableCol( 0 );
	sizerObjLabels->AddGrowableCol( 1 );
	wxBoxSizer* sizerObjLabelsStruct = new wxBoxSizer( wxVERTICAL );
	// layout of group of elements in the orig panel 
	wxStaticBox* pLabelsBox = new wxStaticBox( pPanelObjLayout, -1, wxT("Shapes/Labels/Design"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerLabelsPage = new wxStaticBoxSizer( pLabelsBox, wxHORIZONTAL );
	sizerLabelsPage->Add( sizerObjLabels, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////
	// Graphics sizer - layout of elements
	sizerGraphics = new wxFlexGridSizer( 3, 2, 10, 5 );
	sizerGraphics->AddGrowableCol( 0 );
	sizerGraphics->AddGrowableCol( 1 );
	sizerGraphicsStruct = new wxFlexGridSizer( 1, 1, 2, 2 );
	// layout of group of elements in the orig panel 
	wxStaticBox* pGraphicsBox = new wxStaticBox( pPanelGraphics, -1, wxT("Graphics Design"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerGraphicsPage = new wxStaticBoxSizer( pGraphicsBox, wxHORIZONTAL );
	sizerGraphicsPage->Add( sizerGraphics, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////
	// units and coordinate system sizer - layout of elements
	sizerFormats = new wxGridSizer( 4, 2, 5, 5 );
//	sizerUnitsCoord->AddGrowableCol( 1 );
	// layout of group of elements in the orig panel 
	wxStaticBox* pFormatsBox = new wxStaticBox( pPanelFormats, -1, wxT("Formating GUI System"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* sizerFormatsPage = new wxStaticBoxSizer( pFormatsBox, wxHORIZONTAL );
	sizerFormatsPage->Add( sizerFormats, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	////////////
	// autos sizer
	wxFlexGridSizer* sizerAutos = new wxFlexGridSizer( 3, 2, 5, 5 );
	sizerAutos->AddGrowableCol( 1 );
	sizerAutos->AddGrowableCol( 0 );
	wxBoxSizer* sizerAutosPage = new wxBoxSizer( wxHORIZONTAL );
	sizerAutosPage->Add( sizerAutos, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	////////////
	// services sizer
	wxFlexGridSizer* sizerServices = new wxFlexGridSizer( 3, 2, 5, 5 );
	sizerServices->AddGrowableCol( 1 );
	sizerServices->AddGrowableCol( 0 );
	wxBoxSizer* sizerServicesPage = new wxBoxSizer( wxHORIZONTAL );
	sizerServicesPage->Add( sizerServices, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );


	////////////
	// Temp sizer
	wxFlexGridSizer* sizerTemp = new wxFlexGridSizer( 2, 1, 5, 5 );
	sizerTemp->AddGrowableRow( 0 );
	sizerTemp->AddGrowableCol( 0 );
	wxBoxSizer* sizerTempPage = new wxBoxSizer( wxHORIZONTAL );
	sizerTempPage->Add( sizerTemp, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND );

	///////////////////////////////////////////
	// Object Labels - elements definition 
	// :: object type
	m_pObjectShapeType = new wxChoice( pPanelObjLayout, wxID_SELECT_OBJECT_SHAPE_TYPE,
										wxDefaultPosition,
										wxSize(90,-1), 6, vectObjectShapeType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pObjectShapeType->SetSelection( 0 );
	// :: object line
	m_pObjectShapeLine = new wxChoice( pPanelObjLayout, wxID_SELECT_OBJECT_SHAPE_LINE,
										wxDefaultPosition,
										wxSize(70,-1), 3, vectObjectShapeLine, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pObjectShapeLine->SetSelection( 0 );
	// :: label
	m_pLabelType = new wxChoice( pPanelObjLayout, wxID_SELECT_LABEL_TYPE,
										wxDefaultPosition,
										wxSize(140,-1), 16, vectLabelType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pLabelType->SetSelection( 0 );

	m_pLabelFontColor = new wxButton( pPanelObjLayout, wxID_BUTTON_LABEL_FONT_COLOR, 
								wxT("Label Style"), wxPoint(-1, -1), wxSize(80,20) );
	m_pLabelSampleText =  new wxStaticText( pPanelObjLayout, -1, wxT("Label Font and Color"));
	m_pLabelStructId = new wxCheckBox( pPanelObjLayout, wxID_CHECK_USE_ID, wxT("Use Detection Id")  );
	m_pLabelStructCatNo = new wxCheckBox( pPanelObjLayout, wxID_CHECK_USE_CATNO, wxT("Use Catalog Number")  );
	m_pLabelStructCNames = new wxCheckBox( pPanelObjLayout, wxID_CHECK_USE_CNAME, wxT("Use Common Names") );
	m_pLabelStructType = new wxCheckBox( pPanelObjLayout, wxID_CHECK_USE_TYPE, wxT("Use Object Type") );
	m_pLabelStructMag = new wxCheckBox( pPanelObjLayout, wxID_CHECK_USE_MAG, wxT("Use Magnitude") );

	///////////////////////////////////////////
	// Graphics - elements definition 
	// :: graphics type
	m_pGraphicsType = new wxChoice( pPanelGraphics, wxID_SELECT_GRAPHICS_TYPE,
										wxDefaultPosition,
										wxSize(140,-1), 4, vectGraphicsType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pGraphicsType->SetSelection( 0 );
	// :: graphics line style
	m_pGraphicsLineStyle = new wxChoice( pPanelGraphics, wxID_SELECT_GRAPHICS_LINE_STYLE,
										wxDefaultPosition,
										wxSize(80,-1), 3, vectGraphicsLine, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pGraphicsLineStyle->SetSelection( 0 );
	// :: graphics line colour
	m_pGraphicsLineColour = new wxButton( pPanelGraphics, wxID_SELECT_GRAPHICS_LINE_COLOUR, 
								wxT("Line Colour"), wxPoint(-1, -1), wxSize(80,20) );
	m_pGraphicsLineColourSample =  new wxStaticText( pPanelGraphics, -1, wxT("   Sample of a Line Colour   "));
	// :: object path ther properties
	m_pGraphicsPathVertex = new wxCheckBox( pPanelGraphics, wxID_OBJ_PATH_DRAW_VERTICES, wxT("Draw Vertices")  );
	m_pGraphicsPathStartEnd = new wxCheckBox( pPanelGraphics, wxID_OBJ_PATH_DRAW_STARTEND, wxT("Draw Start/End")  );
	m_pGraphicsPathDirection = new wxCheckBox( pPanelGraphics, wxID_OBJ_PATH_DRAW_DIRECTION, wxT("Draw Direction")  );
	m_pGraphicsPathBaseLines = new wxCheckBox( pPanelGraphics, wxID_OBJ_PATH_DRAW_BASE_LINES, wxT("Draw Base Lines")  );
	// :: sky grid props
	m_pSkyGridShowLabels = new wxCheckBox( pPanelGraphics, wxID_SKY_GRID_LABELS, wxT("Sky Grid Labels") );
	m_pGridDrawSplines = new wxCheckBox( pPanelGraphics, wxID_SKY_GRID_SPLINE, wxT("Draw Splines") );
	m_pGridRaDivValue = new wxTextCtrl( pPanelGraphics, -1, wxT(""), wxDefaultPosition, wxSize(50,17) );
	m_pGridDecDivValue = new wxTextCtrl( pPanelGraphics, -1, wxT(""), wxDefaultPosition, wxSize(50,17) );
	// :: dist grid props
	m_pDistGridBumpFactValue = new wxTextCtrl( pPanelGraphics, -1, wxT(""), wxDefaultPosition, wxSize(50,17) );
	// :: constellation props
	m_pConstellationMarkStars = new wxCheckBox( pPanelGraphics, wxID_SKY_CONSTELLATION_MARK_STARS, wxT("Mark Stars") );
	m_pConstellationOutLine = new wxCheckBox( pPanelGraphics, wxID_SKY_CONSTELLATION_DRAW_OUTLINE, wxT("Draw Outline") );
	m_pConstellationGreekLabels = new wxCheckBox( pPanelGraphics, wxID_SKY_CONSTELLATION_GREEK_LABELS, wxT("Greek Labels") );

	///////////////////////////////////////////
	// :: FORMATS : Units, coords, language - elements definition 
	m_pSkyCoordDisplayFormat = new wxChoice( pPanelFormats, -1,
										wxDefaultPosition,
										wxSize(90,-1), 2, vectUnitsFormat, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSkyCoordDisplayFormat->SetSelection( 0 );
	m_pSkyCoordEntryFormat = new wxChoice( pPanelFormats, -1,
										wxDefaultPosition,
										wxSize(90,-1), 2, vectUnitsFormat, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSkyCoordEntryFormat->SetSelection( 0 );
	// coordinate system selector
	m_pCoordSystem = new wxChoice( pPanelFormats, wxID_SELECT_COORDINATE_SYSTEM,
										wxDefaultPosition,
										wxSize(90,-1), 1, vectCoordSystem, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCoordSystem->SetSelection( 0 );
	// epoch
	m_pSystemEpoch = new wxChoice( pPanelFormats, wxID_SELECT_SYSTEM_EPOCH,
										wxDefaultPosition,
										wxSize(65,-1), 1, vectSystemEpoch, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSystemEpoch->SetSelection( 0 );
	// languages
	m_pGuiLanguage = new wxChoice( pPanelFormats, wxID_SELECT_GUI_LANGUAGE,
										wxDefaultPosition,
										wxSize(120,-1), 0, NULL, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pGuiLanguage->SetSelection( 0 );
	// themes
	m_pGuiTheme = new wxChoice( pPanelFormats, wxID_SELECT_GUI_LANGUAGE,
										wxDefaultPosition,
										wxSize(120,-1), 1, vectTheme, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pGuiTheme->SetSelection( 0 );

	///////////////////
	// Autos - elements definition
	m_pSaveOnDetect = new wxCheckBox( pPanelAutos, -1, wxT("") );
	m_pSaveOnMatch = new wxCheckBox( pPanelAutos, -1, wxT("") );
	m_pSaveOnDetails = new wxCheckBox( pPanelAutos, -1, wxT("") );
	m_pSaveOnAutos = new wxCheckBox( pPanelAutos, -1, wxT("") );
	m_pHideOnMinimize = new wxCheckBox( pPanelAutos, -1, wxT("") );

	///////////////////
	// Services - elements definition
	m_pEnableSoapServices = new wxCheckBox( pPanelServices, -1, wxT("") );
	m_pSoapServerIp = new wxTextCtrl( pPanelServices, -1, wxT("0.0.0.0"), wxDefaultPosition, wxSize(80,-1), wxTE_READONLY  );
	m_pSoapServerPort = new wxTextCtrl( pPanelServices, -1, wxT("11011"), wxDefaultPosition, wxSize(40,-1) );

	///////////////////
	// Temporary - elements definition
	m_pTemporaryList = new wxListCtrl( pPanelTemp, wxID_BUTTON_TEMPORARY_LIST,
			wxPoint(-1,-1), wxSize(-1,-1), wxLC_REPORT|wxLC_HRULES|wxLC_VRULES|wxLC_SINGLE_SEL );
	m_pTemporaryDelete = new wxButton( pPanelTemp, wxID_BUTTON_TEMPORARY_CLEAN, wxT("Empty"), wxPoint(100, -1), wxSize(-1,-1) );
	m_pTemporaryQuota = new wxButton( pPanelTemp, wxID_BUTTON_TEMPORARY_QUOTA, wxT("Quota"), wxPoint(100,-1), wxSize(-1,-1) );
	m_pTemporaryExplore = new wxButton( pPanelTemp, wxID_BUTTON_TEMPORARY_EXPLORE, wxT("Explore"), wxPoint(100,-1), wxSize(-1,-1) );
	// disable all on default
	m_pTemporaryDelete->Disable();
	m_pTemporaryQuota->Disable();
	m_pTemporaryExplore->Disable();

	//////////////////////////////////////////////////
	// populate - object labels panel
	// row label type
	sizerObjLabels->Add( new wxStaticText( pPanelObjLayout, -1, wxT("Label Type:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerObjLabels->Add( m_pLabelType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row object shape
	sizerObjLabels->Add( new wxStaticText( pPanelObjLayout, -1, wxT("Object shape:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerObjLabels->Add( m_pObjectShapeType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row shape line
	sizerObjLabels->Add( new wxStaticText( pPanelObjLayout, -1, wxT("Shape Line:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerObjLabels->Add( m_pObjectShapeLine, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row font type
	sizerObjLabels->Add(m_pLabelFontColor, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerObjLabels->Add( m_pLabelSampleText, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row for label structure
	sizerObjLabels->Add(  new wxStaticText( pPanelObjLayout, -1, wxT("Label Structure:  ")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerObjLabels->Add( sizerObjLabelsStruct, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// now add to the sizer struct options
	sizerObjLabelsStruct->Add( m_pLabelStructId, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerObjLabelsStruct->Add( m_pLabelStructCatNo, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerObjLabelsStruct->Add( m_pLabelStructCNames, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// and the other struct types
	sizerObjLabelsStruct->Add( m_pLabelStructType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerObjLabelsStruct->Add( m_pLabelStructMag, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// default display
	m_pLabelStructCatNo->Hide();
	m_pLabelStructCNames->Hide();
	m_pLabelStructType->Hide();
	m_pLabelStructId->Show();
	m_pLabelStructMag->Show();

	//////////////////////////////////////////////////
	// populate - graphics panel
	// :: row graphics type
	sizerGraphics->Add( new wxStaticText( pPanelGraphics, -1, wxT("Graphics Type:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerGraphics->Add( m_pGraphicsType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: row graphics line style
	sizerGraphics->Add( new wxStaticText( pPanelGraphics, -1, wxT("Line Style:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerGraphics->Add( m_pGraphicsLineStyle, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: row graphics line colour
	sizerGraphics->Add( m_pGraphicsLineColour, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerGraphics->Add( m_pGraphicsLineColourSample, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row for label structure
	sizerGraphics->Add(  new wxStaticText( pPanelGraphics, -1, wxT("Structural Properties:  ")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerGraphics->Add( sizerGraphicsStruct, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// object path other props
	sizerGraphicsStruct->Add( m_pGraphicsPathVertex, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerGraphicsStruct->Add( m_pGraphicsPathStartEnd, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerGraphicsStruct->Add( m_pGraphicsPathDirection, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerGraphicsStruct->Add( m_pGraphicsPathBaseLines, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: sky grid other props
	sizerGraphicsStruct->Add( m_pSkyGridShowLabels, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerGraphicsStruct->Add( m_pGridDrawSplines, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// - grid ra division
	sizerGridRaDiv = new wxBoxSizer( wxHORIZONTAL );
	sizerGridRaDiv->Add( m_pGridRaDivValue, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerGridRaDiv->Add( new wxStaticText( pPanelGraphics, -1, wxT(" RA Division")), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	sizerGraphicsStruct->Add( sizerGridRaDiv, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// - grid dec division
	sizerGridDecDiv = new wxBoxSizer( wxHORIZONTAL );
	sizerGridDecDiv->Add( m_pGridDecDivValue, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerGridDecDiv->Add( new wxStaticText( pPanelGraphics, -1, wxT(" DEC Division")), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	sizerGraphicsStruct->Add( sizerGridDecDiv, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// dist grid props
	sizerGridBumpFact = new wxBoxSizer( wxHORIZONTAL );
	sizerGridBumpFact->Add( m_pDistGridBumpFactValue, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerGridBumpFact->Add( new wxStaticText( pPanelGraphics, -1, wxT(" Bump Factor")), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	sizerGraphicsStruct->Add( sizerGridBumpFact, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// constellation props 
	sizerGraphicsStruct->Add( m_pConstellationMarkStars, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerGraphicsStruct->Add( m_pConstellationOutLine, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerGraphicsStruct->Add( m_pConstellationGreekLabels, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// default state
	m_pSkyGridShowLabels->Hide();
	m_pGridDrawSplines->Hide();
	sizerGraphicsStruct->Show( sizerGridRaDiv, 0, 1 );
	sizerGraphicsStruct->Show( sizerGridDecDiv, 0, 1 );
	sizerGraphicsStruct->Show( sizerGridBumpFact, 0, 1 );
	m_pConstellationGreekLabels->Hide();
	m_pConstellationMarkStars->Hide();
	m_pConstellationOutLine->Hide();


	//////////////////////////////////////////////////
	// populate FORMATS - units and coord system, language
	// row global ?? (or just status bar?) bar units
	sizerFormats->Add( new wxStaticText( pPanelFormats, -1, wxT("Sky Coord Display Format:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerFormats->Add( m_pSkyCoordDisplayFormat, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// sky entry coords
	sizerFormats->Add( new wxStaticText( pPanelFormats, -1, wxT("Sky Coord Entry Format:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerFormats->Add( m_pSkyCoordEntryFormat, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row for status bar coordinate system ra/dec or alt/az
	sizerFormats->Add( new wxStaticText( pPanelFormats, -1, wxT("Coordinate System:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerFormats->Add( m_pCoordSystem, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row to set the epoch 
	sizerFormats->Add( new wxStaticText( pPanelFormats, -1, wxT("System Epoch:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerFormats->Add( m_pSystemEpoch, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row to set the language 
	sizerFormats->Add( new wxStaticText( pPanelFormats, -1, _("Language:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerFormats->Add( m_pGuiLanguage, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// theme 
	sizerFormats->Add( new wxStaticText( pPanelFormats, -1, _("Theme:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerFormats->Add( m_pGuiTheme, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	/////////////////////////////////////////
	// populate Autos panel
	// :: save on detect
	sizerAutos->Add( m_pSaveOnDetect, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerAutos->Add( new wxStaticText( pPanelAutos, -1, wxT(" Save After Object Detection       ")), 0,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	// :: save on matching
	sizerAutos->Add( m_pSaveOnMatch, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerAutos->Add( new wxStaticText( pPanelAutos, -1, wxT(" Save After Object Matching")), 0,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	// :: save on details
	sizerAutos->Add( m_pSaveOnDetails, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerAutos->Add( new wxStaticText( pPanelAutos, -1, wxT(" Save After Details Update")), 0,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	// :: save on auto actions
	sizerAutos->Add( m_pSaveOnAutos, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerAutos->Add( new wxStaticText( pPanelAutos, -1, wxT(" Save After Auto Actions")), 0,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	// :: hide on minimize
	sizerAutos->Add( m_pHideOnMinimize, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerAutos->Add( new wxStaticText( pPanelAutos, -1, wxT(" Hide On Minimize")), 0,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

	/////////////////////////////////////////
	// populate Services panel
	// :: enable soap server
	sizerServices->Add( m_pEnableSoapServices, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL  );
	sizerServices->Add( new wxStaticText( pPanelServices, -1, wxT(" Enable Soap Services       ")), 0,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	// :: soap server ip
	sizerServices->Add( new wxStaticText(pPanelServices, -1, wxT("Soap Server Interface Ip:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerServices->Add( m_pSoapServerIp, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// :: soap server ip
	sizerServices->Add( new wxStaticText(pPanelServices, -1, wxT("Soap Server Interface Port:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerServices->Add( m_pSoapServerPort, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	/////////////////////////////////////////
	// populate Temp panel
	// :: temporary list
	sizerTemp->Add( m_pTemporaryList, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );
	// :: list control buttons
	wxFlexGridSizer* sizerTempBtn = new wxFlexGridSizer( 1, 3, 5, 5 );
	sizerTempBtn->Add( m_pTemporaryDelete, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxRIGHT, 10 );
	sizerTempBtn->Add( m_pTemporaryQuota, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxRIGHT, 10 );
	sizerTempBtn->Add( m_pTemporaryExplore, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
	sizerTemp->Add( sizerTempBtn, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );

	// set notebook panels
	pPanelObjLayout->SetSizer( sizerLabelsPage );
	pNotebook->AddPage( pPanelObjLayout, wxT("Objects") );
	pPanelGraphics->SetSizer( sizerGraphicsPage );
	pNotebook->AddPage( pPanelGraphics, wxT("Graphics") );
	pPanelFormats->SetSizer( sizerFormatsPage );
	pNotebook->AddPage( pPanelFormats, wxT("Formats") );
	pPanelAutos->SetSizer( sizerAutosPage );
	pNotebook->AddPage( pPanelAutos, wxT("Behavior") );
	pPanelServices->SetSizer( sizerServicesPage );
	pNotebook->AddPage( pPanelServices, wxT("Services") );
	pPanelTemp->SetSizer( sizerTempPage );
	pNotebook->AddPage( pPanelTemp, wxT("Temporary") );

	pTopSizer->Add( pNotebook, 0, wxGROW | wxALL|wxEXPAND, 10 );
	pTopSizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10 );

//	pMainPanel->SetSizer( sizerMainPage );

	// activate
	//pTopSizer->Layout( );
	pNotebook->Layout();
	SetSizer(pTopSizer);
	SetAutoLayout(TRUE);
	//Layout( );
//	pTopSizer->SetSizeHints(this);
	pTopSizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CMainConfigView
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CMainConfigView::~CMainConfigView( ) 
{
	delete( m_pFileManager );

	delete( m_pObjectShapeType );
	delete( m_pObjectShapeLine );

	delete( m_pLabelType );
	delete( m_pLabelFontColor );
	delete( m_pLabelSampleText );
	delete( m_pLabelStructCNames );
	delete( m_pSkyCoordDisplayFormat );
	delete( m_pSkyCoordEntryFormat );
	delete( m_pCoordSystem );
	delete( m_pGuiLanguage );
	delete( m_pGuiTheme );

	delete( m_pLabelStructId );
	delete( m_pLabelStructCatNo );
	delete( m_pLabelStructMag );
	delete( m_pLabelStructType );
	// graphics
	delete( m_pGraphicsType );
	delete( m_pGraphicsLineStyle );
	delete( m_pGraphicsLineColour );
	delete( m_pGraphicsLineColourSample );
	// obj path props
	delete( m_pGraphicsPathVertex );
	delete( m_pGraphicsPathStartEnd );
	delete( m_pGraphicsPathDirection );
	delete( m_pGraphicsPathBaseLines );
	// sky grid props
	delete( m_pSkyGridShowLabels );
	delete( m_pGridDrawSplines );
	delete( m_pGridRaDivValue );
	delete( m_pGridDecDivValue );
	// dist grid props
	delete( m_pDistGridBumpFactValue );
	// constellation props
	delete( m_pConstellationMarkStars );
	delete( m_pConstellationOutLine );
	delete( m_pConstellationGreekLabels );

	// autos
	delete( m_pSaveOnDetect );
	delete( m_pSaveOnMatch );
	delete( m_pSaveOnDetails );
	delete( m_pSaveOnAutos );
	delete( m_pHideOnMinimize );

	// services
	delete( m_pEnableSoapServices );
	delete( m_pSoapServerIp );
	delete( m_pSoapServerPort );

	// temporary
	delete( m_pTemporaryList );
	delete( m_pTemporaryDelete );
	delete( m_pTemporaryQuota );
	delete( m_pTemporaryExplore );
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CMainConfigView
// Purpose:	Set and get my reference from the config object
// Input:	pointer to detect config and main config
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CMainConfigView::SetConfig( CConfigMain* pMainConfig, CGUIFrame* pFrame )
{
	int bChange=0, nVarInt=0, i=0;

	m_pMainConfig = pMainConfig;
	m_pFrame = pFrame;
//	m_pLocale = pLocale;

	// gete references from frame	
	//////////////////////////
	// :: detected
	m_nDetectObjShapeType = m_pMainConfig->m_nDetectObjShapeType;
	m_nDetectObjShapeLine = m_pMainConfig->m_nDetectObjShapeLine;
	fntDObjLabel = m_pMainConfig->fontDObjLabel;
	colDObjLabel = m_pMainConfig->colorDObjLabel;
	// :: matched
	m_nMatchObjShapeType = m_pMainConfig->m_nMatchObjShapeType;
	m_nMatchObjShapeLine = m_pMainConfig->m_nMatchObjShapeLine;
	fntMObjLabel = m_pMainConfig->fontMObjLabel;
	colMObjLabel = m_pMainConfig->colorMObjLabel;
	// :: catalog
	m_nCatObjShapeType = m_pMainConfig->m_nCatObjShapeType;
	m_nCatObjShapeLine = m_pMainConfig->m_nCatObjShapeLine;
	fntCObjLabel = m_pMainConfig->fontCObjLabel;
	colCObjLabel = m_pMainConfig->colorCObjLabel;
	// :: nebula
	m_nNebObjShapeType = m_pMainConfig->m_nNebObjShapeType;
	m_nNebObjShapeLine = m_pMainConfig->m_nNebObjShapeLine;
	fntNebObjLabel = m_pMainConfig->fontNebObjLabel;
	colNebObjLabel = m_pMainConfig->colorNebObjLabel;
	// :: cluster
	m_nCluObjShapeType = m_pMainConfig->m_nCluObjShapeType;
	m_nCluObjShapeLine = m_pMainConfig->m_nCluObjShapeLine;
	fntCluObjLabel = m_pMainConfig->fontCluObjLabel;
	colCluObjLabel = m_pMainConfig->colorCluObjLabel;
	// :: galaxy
	m_nGalObjShapeType = m_pMainConfig->m_nGalObjShapeType;
	m_nGalObjShapeLine = m_pMainConfig->m_nGalObjShapeLine;
	fntGalObjLabel = m_pMainConfig->fontGalObjLabel;
	colGalObjLabel = m_pMainConfig->colorGalObjLabel;
	// :: supernovae 
	m_nSNObjShapeType = m_pMainConfig->m_nSNObjShapeType;
	m_nSNObjShapeLine = m_pMainConfig->m_nSNObjShapeLine;
	fntSNObjLabel = m_pMainConfig->fontSNObjLabel;
	colSNObjLabel = m_pMainConfig->colorSNObjLabel;
	// :: Radio
	m_nRadObjShapeType = m_pMainConfig->m_nRadObjShapeType;
	m_nRadObjShapeLine = m_pMainConfig->m_nRadObjShapeLine;
	fntRadObjLabel = m_pMainConfig->fontRadObjLabel;
	colRadObjLabel = m_pMainConfig->colorRadObjLabel;
	// :: xray 
	m_nXRayObjShapeType = m_pMainConfig->m_nXRayObjShapeType;
	m_nXRayObjShapeLine = m_pMainConfig->m_nXRayObjShapeLine;
	fntXRayObjLabel = m_pMainConfig->fontXRayObjLabel;
	colXRayObjLabel = m_pMainConfig->colorXRayObjLabel;
	// :: gamma
	m_nGammaObjShapeType = m_pMainConfig->m_nGammaObjShapeType;
	m_nGammaObjShapeLine = m_pMainConfig->m_nGammaObjShapeLine;
	fntGammaObjLabel = m_pMainConfig->fontGammaObjLabel;
	colGammaObjLabel = m_pMainConfig->colorGammaObjLabel;
	// :: black holes
	m_nBHObjShapeType = m_pMainConfig->m_nBHObjShapeType;
	m_nBHObjShapeLine = m_pMainConfig->m_nBHObjShapeLine;
	fntBHObjLabel = m_pMainConfig->fontBHObjLabel;
	colBHObjLabel = m_pMainConfig->colorBHObjLabel;
	// :: multiple/double stars
	m_nMSObjShapeType = m_pMainConfig->m_nMSObjShapeType;
	m_nMSObjShapeLine = m_pMainConfig->m_nMSObjShapeLine;
	fntMSObjLabel = m_pMainConfig->fontMSObjLabel;
	colMSObjLabel = m_pMainConfig->colorMSObjLabel;
	// :: asteroids
	m_nASTObjShapeType = m_pMainConfig->m_nASTObjShapeType;
	m_nASTObjShapeLine = m_pMainConfig->m_nASTObjShapeLine;
	fntASTObjLabel = m_pMainConfig->fontASTObjLabel;
	colASTObjLabel = m_pMainConfig->colorASTObjLabel;
	// :: comets
	m_nCOMObjShapeType = m_pMainConfig->m_nCOMObjShapeType;
	m_nCOMObjShapeLine = m_pMainConfig->m_nCOMObjShapeLine;
	fntCOMObjLabel = m_pMainConfig->fontCOMObjLabel;
	colCOMObjLabel = m_pMainConfig->colorCOMObjLabel;
	// :: extrasolar planets
	m_nEXPObjShapeType = m_pMainConfig->m_nEXPObjShapeType;
	m_nEXPObjShapeLine = m_pMainConfig->m_nEXPObjShapeLine;
	fntEXPObjLabel = m_pMainConfig->fontEXPObjLabel;
	colEXPObjLabel = m_pMainConfig->colorEXPObjLabel;
	// :: artificial earth satellites
	m_nAESObjShapeType = m_pMainConfig->m_nAESObjShapeType;
	m_nAESObjShapeLine = m_pMainConfig->m_nAESObjShapeLine;
	fntAESObjLabel = m_pMainConfig->fontAESObjLabel;
	colAESObjLabel = m_pMainConfig->colorAESObjLabel;

	// set my sample/reference text font - default detected obj 
	m_pLabelSampleText->SetFont( m_pMainConfig->fontDObjLabel );
	m_pLabelSampleText->SetForegroundColour( m_pMainConfig->colorDObjLabel );

	// SECTION :: STYLE - LABEL STRUCTURE 
	////////////////////////////////////////
	// get detected object use id
	// :: detected
	m_bDetectObjLabelUseId = m_pMainConfig->m_bDetectObjLabelUseId;
	m_bDetectObjLabelUseMag = m_pMainConfig->m_bDetectObjLabelUseMag;
	// :: matched
	m_bMatchObjLabelUseId = m_pMainConfig->m_bMatchObjLabelUseId;
	m_bMatchObjLabelUseCatNo = m_pMainConfig->m_bMatchObjLabelUseCatNo;
	m_bMatchObjLabelUseCName = m_pMainConfig->m_bMatchObjLabelUseCName;
	// :: star catalog
	m_bCatObjLabelUseCatNo = m_pMainConfig->m_bCatStarLabelUseCatNo;
	m_bCatObjLabelUseCName = m_pMainConfig->m_bCatStarLabelUseCName;
	// :: Nebula
	m_bNebObjLabelUseCatNo = m_pMainConfig->m_bNebObjLabelUseCatNo;
	m_bNebObjLabelUseCName = m_pMainConfig->m_bNebObjLabelUseCName;
	m_bNebObjLabelUseType = m_pMainConfig->m_bNebObjLabelUseType;
	// :: Clusters
	m_bCluObjLabelUseCatNo = m_pMainConfig->m_bCluObjLabelUseCatNo;
	m_bCluObjLabelUseCName = m_pMainConfig->m_bCluObjLabelUseCName;
	m_bCluObjLabelUseType = m_pMainConfig->m_bCluObjLabelUseType;
	// :: Galaxy 
	m_bGalObjLabelUseCatNo = m_pMainConfig->m_bGalObjLabelUseCatNo;
	m_bGalObjLabelUseCName = m_pMainConfig->m_bGalObjLabelUseCName;
	m_bGalObjLabelUseType = m_pMainConfig->m_bGalObjLabelUseType;
	// :: Supernovae
	m_bSNObjLabelUseCatNo = m_pMainConfig->m_bSNObjLabelUseCatNo;
	m_bSNObjLabelUseCName = m_pMainConfig->m_bSNObjLabelUseCName;
	m_bSNObjLabelUseType = m_pMainConfig->m_bSNObjLabelUseType;
	// :: Radio
	m_bRadObjLabelUseCatNo = m_pMainConfig->m_bRadObjLabelUseCatNo;
	m_bRadObjLabelUseType = m_pMainConfig->m_bRadObjLabelUseType;
	// :: XRay 
	m_bXRayObjLabelUseCatNo = m_pMainConfig->m_bXRayObjLabelUseCatNo;
	m_bXRayObjLabelUseType = m_pMainConfig->m_bXRayObjLabelUseType;
	// :: Gamma
	m_bGammaObjLabelUseCatNo = m_pMainConfig->m_bGammaObjLabelUseCatNo;
	m_bGammaObjLabelUseType = m_pMainConfig->m_bGammaObjLabelUseType;
	// :: Black Holes
	m_bBHObjLabelUseCatNo = m_pMainConfig->m_bBHObjLabelUseCatNo;
	m_bBHObjLabelUseType = m_pMainConfig->m_bBHObjLabelUseType;
	// :: multiple/double stars
	m_bMSObjLabelUseCatNo = m_pMainConfig->m_bMSObjLabelUseCatNo;
	m_bMSObjLabelUseType = m_pMainConfig->m_bMSObjLabelUseType;
	// :: asteroids
	m_bASTObjLabelUseCatNo = m_pMainConfig->m_bASTObjLabelUseCatNo;
	m_bASTObjLabelUseType = m_pMainConfig->m_bASTObjLabelUseType;
	// :: comets
	m_bCOMObjLabelUseCatNo = m_pMainConfig->m_bCOMObjLabelUseCatNo;
	m_bCOMObjLabelUseType = m_pMainConfig->m_bCOMObjLabelUseType;
	// :: extrasolar planets
	m_bEXPObjLabelUseCatNo = m_pMainConfig->m_bEXPObjLabelUseCatNo;
	m_bEXPObjLabelUseType = m_pMainConfig->m_bEXPObjLabelUseType;
	// :: artificial earth satellites
	m_bAESObjLabelUseCatNo = m_pMainConfig->m_bAESObjLabelUseCatNo;
	m_bAESObjLabelUseType = m_pMainConfig->m_bAESObjLabelUseType;

	// SECTION :: ACTION 
	/////////////////////////////////////////////////////////
	// get save on detection
	if( m_pMainConfig->GetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_DETECTION, nVarInt ) == 0 )
	{
		m_pMainConfig->SetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_DETECTION, 1 );
		m_pSaveOnDetect->SetValue( 1 );
		bChange = 1;
	} else
	{
		m_pSaveOnDetect->SetValue( nVarInt );
	}
	// get save on matching
	if( m_pMainConfig->GetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_MATCHING, nVarInt ) == 0 )
	{
		m_pMainConfig->SetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_MATCHING, 1 );
		m_pSaveOnMatch->SetValue( 1 );
		bChange = 1;
	} else
	{
		m_pSaveOnMatch->SetValue( nVarInt );
	}
	// get save on details
	if( m_pMainConfig->GetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_DETAILS, nVarInt ) == 0 )
	{
		m_pMainConfig->SetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_DETAILS, 1 );
		m_pSaveOnDetails->SetValue( 1 );
		bChange = 1;
	} else
	{
		m_pSaveOnDetails->SetValue( nVarInt );
	}
	// get save on autos
	if( m_pMainConfig->GetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_AUTOS, nVarInt ) == 0 )
	{
		m_pMainConfig->SetIntVar( SECTION_ID_ACTION, CONF_ACTION_SAVE_ON_AUTOS, 1 );
		m_pSaveOnAutos->SetValue( 1 );
		bChange = 1;
	} else
	{
		m_pSaveOnAutos->SetValue( nVarInt );
	}
	// hide on minimize
	m_pHideOnMinimize->SetValue( m_pMainConfig->m_bHideOnMinimize );


	// SECTION :: SERVICES 
	/////////////////////////////////////////////////////////
	// get enable soap server
	if( m_pMainConfig->GetIntVar( SECTION_ID_SERVICES, CONF_SERVICES_SERVER_RUN, nVarInt ) == 0 )
	{
		m_pMainConfig->SetIntVar( SECTION_ID_SERVICES, CONF_SERVICES_SERVER_RUN, 1 );
		m_pEnableSoapServices->SetValue( 1 );
		bChange = 1;
	} else
	{
		m_pEnableSoapServices->SetValue( nVarInt );
	}

	// SECTION :: GRAPHICS
	//////////////////////////////////////////
	// :: object path
	m_nGraphicsObjectPathLine = m_pMainConfig->m_nGraphicsObjectPathLine;
	colGraphicsObjectPath = m_pMainConfig->colGraphicsObjectPath;
	m_bGraphicsPathVertex = m_pMainConfig->m_bGraphicsPathVertex;
	m_bGraphicsPathStartEnd = m_pMainConfig->m_bGraphicsPathStartEnd;
	m_bGraphicsPathDirection = m_pMainConfig->m_bGraphicsPathDirection;
	m_bGraphicsPathBaseLines = m_pMainConfig->m_bGraphicsPathBaseLines;
	// :: sky grid
	m_nGraphicsSkyGridLine = m_pMainConfig->m_nGraphicsSkyGridLine;
	colGraphicsSkyGrid = m_pMainConfig->colGraphicsSkyGrid;
	m_bSkyGridShowLabels = m_pMainConfig->m_bSkyGridShowLabels;
	m_bSkyGridDrawSplines = m_pMainConfig->m_bSkyGridDrawSplines;
	m_nSkyGridRaDivValue = m_pMainConfig->m_nSkyGridRaDivValue;
	m_nSkyGridDecDivValue = m_pMainConfig->m_nSkyGridDecDivValue;
	// :: distortion grid
	m_nGraphicsDistortionGridLine = m_pMainConfig->m_nGraphicsDistortionGridLine;
	colGraphicsDistortionGrid = m_pMainConfig->colGraphicsDistortionGrid;
	m_bDistGridDrawSplines = m_pMainConfig->m_bDistGridDrawSplines;
	m_nDistGridRaDivValue = m_pMainConfig->m_nDistGridRaDivValue;
	m_nDistGridDecDivValue = m_pMainConfig->m_nDistGridDecDivValue;
	m_nDistGridBumpFactValue = m_pMainConfig->m_nDistGridBumpFactValue;
	// :: constellations
	m_nGraphicsConstellationLinesLine = m_pMainConfig->m_nGraphicsConstellationLinesLine;
	colGraphicsConstellationLines = m_pMainConfig->colGraphicsConstellationLines;
	m_bConstellationMarkStars = m_pMainConfig->m_bConstellationMarkStars;
	m_bConstellationOutLine = m_pMainConfig->m_bConstellationOutLine;
	m_bConstellationGreekLabels = m_pMainConfig->m_bConstellationGreekLabels;

	////////////////////////
	// check if default updates then save config
	if( bChange > 0 ) m_pMainConfig->Save( );

	// units format and coordinate system
	m_pSkyCoordDisplayFormat->SetSelection( m_pMainConfig->m_nSkyCoordDisplayFormat );
	m_pSkyCoordEntryFormat->SetSelection( m_pMainConfig->m_nSkyCoordEntryFormat );

	m_pCoordSystem->SetSelection( m_pMainConfig->m_nCoordSystem );
	// :: set/select language
	m_pGuiLanguage->Clear();
	for( i=0; i<g_nGuiLanguage; i++ ) m_pGuiLanguage->Append( g_vectGuiLanguage[i].name );
	m_nGuiLanguage = m_pMainConfig->m_nGuiLanguage;
	m_pGuiLanguage->SetSelection( m_nGuiLanguage ); 

	// and now set my check boxes - for default detetcted
	m_pLabelStructId->SetValue( m_bDetectObjLabelUseId );
	m_pLabelStructMag->SetValue( m_bDetectObjLabelUseMag );

	// set object type panel
	SetGraphicsType( -1 );

	// reset layout
	sizerObjLabels->Layout( );
	sizerGraphics->Layout( );
	sizerFormats->Layout( );
	GetSizer()->Layout( );
	Fit( );

	return;
}

// set temporary list
////////////////////////////////////////////////////////////////////
void CMainConfigView::SetTemporaryList( )
{
	// clean up
	int w=0, h=0, i=0;
	wxString strTmp;

	m_pTemporaryList->GetSize( &w, &h );
	m_pTemporaryList->ClearAll();

	// create header
	m_pTemporaryList->InsertColumn( 1, wxT("Name"), wxLIST_FORMAT_CENTRE, 110 );
	m_pTemporaryList->InsertColumn( 2, wxT("Size(Mb)"), wxLIST_FORMAT_CENTRE, 55 );
	m_pTemporaryList->InsertColumn( 3, wxT("No."), wxLIST_FORMAT_CENTRE, 40 );
	m_pTemporaryList->InsertColumn( 3, wxT("Quota"), wxLIST_FORMAT_CENTRE, 50 );
	m_pTemporaryList->InsertColumn( 4, wxT("Description"), wxLIST_FORMAT_LEFT, 150 );

	// load temp folders from ini file and process
	DefCmdAction act;
	act.id = THREAD_ACTION_PROCCESS_TEMPORARY_FOLDERS;
	act.vectObj[0] = (void*) m_pFileManager;
	/////////////////
	// show wait dialog
	CWaitDialog waitDialog( this, wxT("Wait..."), wxT("Processing temporary folders..."), 0, m_pUnimapWorker, &act );
	waitDialog.ShowModal();

	// populate in the list
	for( i=0; i<m_pFileManager->m_vectTemporary.size(); i++ )
	{
		long nIndex = m_pTemporaryList->InsertItem( 55551+i, wxT("") );
		m_pTemporaryList->SetItem( nIndex, 0, m_pFileManager->m_vectTemporary[i].name );
		// :: size in mb
		//unsigned __int64 uu = m_vectTemporary[i].size.GetValue();
		double sizeMB = (double) (m_pFileManager->m_vectTemporary[i].size.GetValue()/1048576.0);
		strTmp.Printf( wxT("%.2lf"), sizeMB );
		m_pTemporaryList->SetItem( nIndex, 1, strTmp );
		// :: number of files
		strTmp.Printf( wxT("%d"), m_pFileManager->m_vectTemporary[i].no );
		m_pTemporaryList->SetItem( nIndex, 2, strTmp );
		// :: quota
		if( m_pFileManager->m_vectTemporary[i].quota == 0 )
			strTmp = wxT("No");
		else
			strTmp.Printf( wxT("%d"), m_pFileManager->m_vectTemporary[i].quota );
		m_pTemporaryList->SetItem( nIndex, 3, strTmp );
		// :: description
		m_pTemporaryList->SetItem( nIndex, 4, m_pFileManager->m_vectTemporary[i].desc );

	}
}

////////////////////////////////////////////////////////////////////
// Method:		OnLabelStyle
// Class:		CMainConfigView
// Purpose:		cheange/populate dialog on action type
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CMainConfigView::OnLabelStyle( wxCommandEvent& event )
{
	wxFont canvasFont;
	wxColour canvasTextColour;
	wxFontData data;

	canvasFont = m_pLabelSampleText->GetFont();
	canvasTextColour = m_pLabelSampleText->GetForegroundColour();
	data.SetInitialFont(canvasFont);
	data.SetColour(canvasTextColour);

	wxFontDialog dialog(this, &data);
	if (dialog.ShowModal() == wxID_OK)
	{
		wxFontData retData = dialog.GetFontData();
		canvasFont = retData.GetChosenFont();
		canvasTextColour = retData.GetColour();

		// set and refresh my window
		m_pLabelSampleText->SetFont( canvasFont );
		m_pLabelSampleText->SetForegroundColour( canvasTextColour );

		/////////////////////////////////
		// get selected format 
		int nType = m_pLabelType->GetSelection();
		// now set my configuration based on label type
		if( nType == LABEL_TYPE_DETECTED_OBJECT )
		{
			fntDObjLabel = canvasFont;
			colDObjLabel = canvasTextColour;
		} else if( nType == LABEL_TYPE_MATCHED_OBJECT )
		{
			fntMObjLabel = canvasFont;
			colMObjLabel = canvasTextColour;
		} else if( nType == LABEL_TYPE_CATALOG_OBJECT )
		{
			fntCObjLabel = canvasFont;
			colCObjLabel = canvasTextColour;
		} else if( nType == LABEL_TYPE_CATALOG_NEBULA )
		{
			fntNebObjLabel = canvasFont;
			colNebObjLabel = canvasTextColour;
		} else if( nType == LABEL_TYPE_CATALOG_CLUSTER )
		{
			fntCluObjLabel = canvasFont;
			colCluObjLabel = canvasTextColour;
		} else if( nType == LABEL_TYPE_CATALOG_GALAXY )
		{
			fntGalObjLabel = canvasFont;
			colGalObjLabel = canvasTextColour;
		} else if( nType == LABEL_TYPE_CATALOG_SUPERNOVAE )
		{
			fntSNObjLabel = canvasFont;
			colSNObjLabel = canvasTextColour;
		} else if( nType == LABEL_TYPE_CATALOG_BLACKHOLE )
		{
			fntBHObjLabel = canvasFont;
			colBHObjLabel = canvasTextColour;
		} else if( nType == LABEL_TYPE_CATALOG_RADIO )
		{
			fntRadObjLabel = canvasFont;
			colRadObjLabel = canvasTextColour;
		} else if( nType == LABEL_TYPE_CATALOG_XRAY )
		{
			fntXRayObjLabel = canvasFont;
			colXRayObjLabel = canvasTextColour;
		} else if( nType == LABEL_TYPE_CATALOG_GAMMA )
		{
			fntGammaObjLabel = canvasFont;
			colGammaObjLabel = canvasTextColour;
		} else if( nType == LABEL_TYPE_CATALOG_MSTARS )
		{
			fntMSObjLabel = canvasFont;
			colMSObjLabel = canvasTextColour;
		} else if( nType == LABEL_TYPE_CATALOG_AST )
		{
			fntASTObjLabel = canvasFont;
			colASTObjLabel = canvasTextColour;
		} else if( nType == LABEL_TYPE_CATALOG_COM )
		{
			fntCOMObjLabel = canvasFont;
			colCOMObjLabel = canvasTextColour;
		} else if( nType == LABEL_TYPE_CATALOG_EXP )
		{
			fntEXPObjLabel = canvasFont;
			colEXPObjLabel = canvasTextColour;
		} else if( nType == LABEL_TYPE_CATALOG_AES )
		{
			fntAESObjLabel = canvasFont;
			colAESObjLabel = canvasTextColour;
		}


		// reset layout
		sizerObjLabels->Layout( );
		sizerGraphics->Layout( );
		sizerFormats->Layout( );
		GetSizer()->Layout( );
		Fit( );

		Refresh();
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnGraphicsColour
// Class:		CMainConfigView
// Purpose:		popup color picker dialog
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CMainConfigView::OnGraphicsColour( wxCommandEvent& event )
{
	wxColour canvasTextColour;
	wxColour newColour;
//	wxColourData data;

	canvasTextColour = m_pGraphicsLineColourSample->GetForegroundColour();
//	data.SetColour(canvasTextColour);
//	data.SetChooseFull( 1 );

//	wxColourDialog dialog(this, &data);
//	if( dialog.ShowModal() == wxID_OK )

	newColour = wxGetColourFromUser( this, canvasTextColour );

	if( newColour.IsOk() )
	{
//		wxColourData retData = dialog.GetColourData();
//		canvasTextColour = retData.GetColour();

		// set and refresh my window
		m_pGraphicsLineColourSample->SetForegroundColour( newColour );
		m_pGraphicsLineColourSample->SetBackgroundColour( newColour );

		/////////////////////////////////
		// get selected format 
		int nType = m_pGraphicsType->GetSelection();
		// now set my configuration based on label type
		if( nType == GRAPHICS_TYPE_OBJECT_PATH )
			colGraphicsObjectPath = newColour;
		else if( nType == GRAPHICS_TYPE_SKY_GRID )
			colGraphicsSkyGrid = newColour;
		else if( nType == GRAPHICS_TYPE_DISTORTION_GRID )
			colGraphicsDistortionGrid = newColour;
		else if( nType == GRAPHICS_TYPE_CONSTELLATION_LINES )
			colGraphicsConstellationLines = newColour;

		// reset layout
		sizerObjLabels->Layout( );
		sizerGraphics->Layout( );
		sizerFormats->Layout( );
		GetSizer()->Layout( );
		Fit( );
		Refresh();
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnLabelType
// Class:		CMainConfigView
// Purpose:		on selection of a label type
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CMainConfigView::OnLabelType( wxCommandEvent& event )
{
	// get selected format 
	int nType = m_pLabelType->GetSelection();

	// now set my configuration based on label type
	if( nType == LABEL_TYPE_DETECTED_OBJECT )
	{
		m_pLabelStructCatNo->Hide();
		m_pLabelStructCNames->Hide();
		m_pLabelStructType->Hide();
		m_pLabelStructId->Show();
		m_pLabelStructMag->Show();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntDObjLabel );
		m_pLabelSampleText->SetForegroundColour( colDObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nDetectObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nDetectObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructId->SetValue( m_bDetectObjLabelUseId );
		m_pLabelStructMag->SetValue( m_bDetectObjLabelUseMag );


	} else if( nType == LABEL_TYPE_MATCHED_OBJECT )
	{
		m_pLabelStructId->Show();
		m_pLabelStructCatNo->Show();
		m_pLabelStructCNames->Show();
		m_pLabelStructType->Hide();
		m_pLabelStructMag->Hide();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntMObjLabel );
		m_pLabelSampleText->SetForegroundColour( colMObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nMatchObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nMatchObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructId->SetValue( m_bMatchObjLabelUseId );
		m_pLabelStructCatNo->SetValue( m_bMatchObjLabelUseCatNo );
		m_pLabelStructCNames->SetValue( m_bMatchObjLabelUseCName );

	} else if( nType == LABEL_TYPE_CATALOG_OBJECT )
	{
		m_pLabelStructId->Hide();
		m_pLabelStructCatNo->Show();
		m_pLabelStructCNames->Show();
		m_pLabelStructType->Hide();
		m_pLabelStructMag->Hide();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntCObjLabel );
		m_pLabelSampleText->SetForegroundColour( colCObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nCatObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nCatObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructCatNo->SetValue( m_bCatObjLabelUseCatNo );
		m_pLabelStructCNames->SetValue( m_bCatObjLabelUseCName );

	} else if( nType == LABEL_TYPE_CATALOG_NEBULA )
	{
		m_pLabelStructId->Hide();
		m_pLabelStructCatNo->Show();
		m_pLabelStructCNames->Show();
		m_pLabelStructType->Show();
		m_pLabelStructMag->Hide();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntNebObjLabel );
		m_pLabelSampleText->SetForegroundColour( colNebObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nNebObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nNebObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructCatNo->SetValue( m_bNebObjLabelUseCatNo );
		m_pLabelStructCNames->SetValue( m_bNebObjLabelUseCName );
		m_pLabelStructType->SetValue( m_bNebObjLabelUseType );

	} else if( nType == LABEL_TYPE_CATALOG_CLUSTER )
	{
		m_pLabelStructId->Hide();
		m_pLabelStructCatNo->Show();
		m_pLabelStructCNames->Show();
		m_pLabelStructType->Show();
		m_pLabelStructMag->Hide();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntCluObjLabel );
		m_pLabelSampleText->SetForegroundColour( colCluObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nCluObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nCluObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructCatNo->SetValue( m_bCluObjLabelUseCatNo );
		m_pLabelStructCNames->SetValue( m_bCluObjLabelUseCName );
		m_pLabelStructType->SetValue( m_bCluObjLabelUseType );

	} else if( nType == LABEL_TYPE_CATALOG_GALAXY )
	{
		m_pLabelStructId->Hide();
		m_pLabelStructCatNo->Show();
		m_pLabelStructCNames->Show();
		m_pLabelStructType->Show();
		m_pLabelStructMag->Hide();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntGalObjLabel );
		m_pLabelSampleText->SetForegroundColour( colGalObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nGalObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nGalObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructCatNo->SetValue( m_bGalObjLabelUseCatNo );
		m_pLabelStructCNames->SetValue( m_bGalObjLabelUseCName );
		m_pLabelStructType->SetValue( m_bGalObjLabelUseType );

	} else if( nType == LABEL_TYPE_CATALOG_SUPERNOVAE )
	{
		m_pLabelStructId->Hide();
		m_pLabelStructCatNo->Show();
		m_pLabelStructCNames->Show();
		m_pLabelStructType->Show();
		m_pLabelStructMag->Hide();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntSNObjLabel );
		m_pLabelSampleText->SetForegroundColour( colSNObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nSNObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nSNObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructCatNo->SetValue( m_bSNObjLabelUseCatNo );
		m_pLabelStructCNames->SetValue( m_bSNObjLabelUseCName );
		m_pLabelStructType->SetValue( m_bSNObjLabelUseType );

	} else if( nType == LABEL_TYPE_CATALOG_BLACKHOLE )
	{
		m_pLabelStructId->Hide();
		m_pLabelStructCatNo->Show();
		m_pLabelStructCNames->Hide();
		m_pLabelStructType->Show();
		m_pLabelStructMag->Hide();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntBHObjLabel );
		m_pLabelSampleText->SetForegroundColour( colBHObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nBHObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nBHObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructCatNo->SetValue( m_bBHObjLabelUseCatNo );
		m_pLabelStructType->SetValue( m_bBHObjLabelUseType );

	} else if( nType == LABEL_TYPE_CATALOG_RADIO )
	{
		m_pLabelStructId->Hide();
		m_pLabelStructCatNo->Show();
		m_pLabelStructCNames->Hide();
		m_pLabelStructType->Show();
		m_pLabelStructMag->Hide();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntRadObjLabel );
		m_pLabelSampleText->SetForegroundColour( colRadObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nRadObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nRadObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructCatNo->SetValue( m_bRadObjLabelUseCatNo );
		m_pLabelStructType->SetValue( m_bRadObjLabelUseType );

	} else if( nType == LABEL_TYPE_CATALOG_XRAY )
	{
		m_pLabelStructId->Hide();
		m_pLabelStructCatNo->Show();
		m_pLabelStructCNames->Hide();
		m_pLabelStructType->Show();
		m_pLabelStructMag->Hide();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntXRayObjLabel );
		m_pLabelSampleText->SetForegroundColour( colXRayObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nXRayObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nXRayObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructCatNo->SetValue( m_bXRayObjLabelUseCatNo );
		m_pLabelStructType->SetValue( m_bXRayObjLabelUseType );

	} else if( nType == LABEL_TYPE_CATALOG_GAMMA )
	{
		m_pLabelStructId->Hide();
		m_pLabelStructCatNo->Show();
		m_pLabelStructCNames->Hide();
		m_pLabelStructType->Show();
		m_pLabelStructMag->Hide();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntGammaObjLabel );
		m_pLabelSampleText->SetForegroundColour( colGammaObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nGammaObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nGammaObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructCatNo->SetValue( m_bGammaObjLabelUseCatNo );
		m_pLabelStructType->SetValue( m_bGammaObjLabelUseType );

	} else if( nType == LABEL_TYPE_CATALOG_MSTARS )
	{
		m_pLabelStructId->Hide();
		m_pLabelStructCatNo->Show();
		m_pLabelStructCNames->Hide();
		m_pLabelStructType->Show();
		m_pLabelStructMag->Hide();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntMSObjLabel );
		m_pLabelSampleText->SetForegroundColour( colMSObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nMSObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nMSObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructCatNo->SetValue( m_bMSObjLabelUseCatNo );
		m_pLabelStructType->SetValue( m_bMSObjLabelUseType );

	} else if( nType == LABEL_TYPE_CATALOG_AST )
	{
		m_pLabelStructId->Hide();
		m_pLabelStructCatNo->Show();
		m_pLabelStructCNames->Hide();
		m_pLabelStructType->Show();
		m_pLabelStructMag->Hide();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntASTObjLabel );
		m_pLabelSampleText->SetForegroundColour( colASTObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nASTObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nASTObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructCatNo->SetValue( m_bASTObjLabelUseCatNo );
		m_pLabelStructType->SetValue( m_bASTObjLabelUseType );

	} else if( nType == LABEL_TYPE_CATALOG_COM )
	{
		m_pLabelStructId->Hide();
		m_pLabelStructCatNo->Show();
		m_pLabelStructCNames->Hide();
		m_pLabelStructType->Show();
		m_pLabelStructMag->Hide();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntCOMObjLabel );
		m_pLabelSampleText->SetForegroundColour( colCOMObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nCOMObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nCOMObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructCatNo->SetValue( m_bCOMObjLabelUseCatNo );
		m_pLabelStructType->SetValue( m_bCOMObjLabelUseType );

	} else if( nType == LABEL_TYPE_CATALOG_EXP )
	{
		m_pLabelStructId->Hide();
		m_pLabelStructCatNo->Show();
		m_pLabelStructCNames->Hide();
		m_pLabelStructType->Show();
		m_pLabelStructMag->Hide();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntEXPObjLabel );
		m_pLabelSampleText->SetForegroundColour( colEXPObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nEXPObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nEXPObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructCatNo->SetValue( m_bEXPObjLabelUseCatNo );
		m_pLabelStructType->SetValue( m_bEXPObjLabelUseType );

	} else if( nType == LABEL_TYPE_CATALOG_AES )
	{
		m_pLabelStructId->Hide();
		m_pLabelStructCatNo->Show();
		m_pLabelStructCNames->Hide();
		m_pLabelStructType->Show();
		m_pLabelStructMag->Hide();
		// set my sample/reference text font 
		m_pLabelSampleText->SetFont( fntAESObjLabel );
		m_pLabelSampleText->SetForegroundColour( colAESObjLabel );
		// set shape and line type
		m_pObjectShapeType->SetSelection( m_nAESObjShapeType );
		m_pObjectShapeLine->SetSelection( m_nAESObjShapeLine );
		// and now set my check boxes 
		m_pLabelStructCatNo->SetValue( m_bAESObjLabelUseCatNo );
		m_pLabelStructType->SetValue( m_bAESObjLabelUseType );

	}

	// reset layout
	sizerObjLabels->Layout( );
	sizerGraphics->Layout( );
	sizerFormats->Layout( );
	GetSizer()->Layout( );
	Fit( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnGraphicsType
// Class:		CMainConfigView
// Purpose:		on selection of graphics type
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CMainConfigView::OnGraphicsType( wxCommandEvent& event )
{
	SetGraphicsType( -1 );

	// reset layout
	sizerObjLabels->Layout( );
	
	//sizerGridRaDiv->Layout();
	sizerGraphicsStruct->Layout();
	sizerGraphics->Layout( );

	sizerFormats->Layout( );
	GetSizer()->Layout( );
	Fit( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SetGraphicsType
// Class:		CMainConfigView
// Purpose:		on selection of graphics type
// Input:		graphics type to set
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CMainConfigView::SetGraphicsType( int nType )
{
	char strTmp[255];

	// get selected format 
	if( nType == -1 )
		nType = m_pGraphicsType->GetSelection();

	////////////////////////////////
	// get vars if previous type was set
	if( m_nGraphicsPrevType != -1 ) GetGraphicsType( m_nGraphicsPrevType );

	// now set my configuration based on label type
	if( nType == GRAPHICS_TYPE_OBJECT_PATH )
	{
		// hide sky/dist grid
		m_pSkyGridShowLabels->Hide();
		m_pGridDrawSplines->Hide();
		sizerGraphicsStruct->Show( sizerGridRaDiv, 0, 1 );
		sizerGraphicsStruct->Show( sizerGridDecDiv, 0, 1 );
		// hide dist grid
		sizerGraphicsStruct->Show( sizerGridBumpFact, 0, 1 );
		// hide constellation
		m_pConstellationMarkStars->Hide();
		m_pConstellationOutLine->Hide();
		m_pConstellationGreekLabels->Hide();

		///////////////////
		// the show obj path
		m_pGraphicsPathVertex->Show();
		m_pGraphicsPathStartEnd->Show();
		m_pGraphicsPathDirection->Show();
		m_pGraphicsPathBaseLines->Show();

		////////////////////
		// set my sample/reference text font 
		m_pGraphicsLineColourSample->SetForegroundColour( colGraphicsObjectPath );
		m_pGraphicsLineColourSample->SetBackgroundColour( colGraphicsObjectPath );
		// setline type
		m_pGraphicsLineStyle->SetSelection( m_nGraphicsObjectPathLine );
		// and now set my struct check boxes 
		m_pGraphicsPathVertex->SetValue( m_bGraphicsPathVertex );
		m_pGraphicsPathStartEnd->SetValue( m_bGraphicsPathStartEnd );
		m_pGraphicsPathDirection->SetValue( m_bGraphicsPathDirection );
		m_pGraphicsPathBaseLines->SetValue( m_bGraphicsPathBaseLines );

	} else if( nType == GRAPHICS_TYPE_SKY_GRID )
	{
		// hide object path
		m_pGraphicsPathVertex->Hide();
		m_pGraphicsPathStartEnd->Hide();
		m_pGraphicsPathDirection->Hide();
		m_pGraphicsPathBaseLines->Hide();
		// hide dist grid
		sizerGraphicsStruct->Show( sizerGridBumpFact, 0, 1 );
		// hide constellation
		m_pConstellationMarkStars->Hide();
		m_pConstellationOutLine->Hide();
		m_pConstellationGreekLabels->Hide();

		// the show sky/dist grid
		m_pSkyGridShowLabels->Show();
		m_pGridDrawSplines->Show();
		sizerGraphicsStruct->Show( sizerGridRaDiv, 1, 1 );
		sizerGraphicsStruct->Show( sizerGridDecDiv, 1, 1 );

		// set my sample/reference text font 
		m_pGraphicsLineColourSample->SetForegroundColour( colGraphicsSkyGrid );
		m_pGraphicsLineColourSample->SetBackgroundColour( colGraphicsSkyGrid );
		// setline type
		m_pGraphicsLineStyle->SetSelection( m_nGraphicsSkyGridLine );
		// and now set my struct check boxes 
		m_pSkyGridShowLabels->SetValue( m_bSkyGridShowLabels );
		m_pGridDrawSplines->SetValue( m_bSkyGridDrawSplines );
		sprintf( strTmp, "%.3f", m_nSkyGridRaDivValue );
		m_pGridRaDivValue->SetValue( wxString(strTmp,wxConvUTF8) );
		sprintf( strTmp, "%.3f", m_nSkyGridDecDivValue );
		m_pGridDecDivValue->SetValue( wxString(strTmp,wxConvUTF8) );

	} else if( nType == GRAPHICS_TYPE_DISTORTION_GRID )
	{
		// obj path hide
		m_pGraphicsPathVertex->Hide();
		m_pGraphicsPathStartEnd->Hide();
		m_pGraphicsPathDirection->Hide();
		m_pGraphicsPathBaseLines->Hide();
		// hide sky grid
		m_pSkyGridShowLabels->Hide();
		// hide constellation
		m_pConstellationMarkStars->Hide();
		m_pConstellationOutLine->Hide();
		m_pConstellationGreekLabels->Hide();

		// the show dist grid
		sizerGraphicsStruct->Show( sizerGridBumpFact, 1, 1 );	
		m_pGridDrawSplines->Show();
		sizerGraphicsStruct->Show( sizerGridRaDiv, 1, 1 );
		sizerGraphicsStruct->Show( sizerGridDecDiv, 1, 1 );

		// set my sample/reference text font 
		m_pGraphicsLineColourSample->SetForegroundColour( colGraphicsDistortionGrid );
		m_pGraphicsLineColourSample->SetBackgroundColour( colGraphicsDistortionGrid );
		// setline type
		m_pGraphicsLineStyle->SetSelection( m_nGraphicsDistortionGridLine );
		// and now set my struct check boxes 
		m_pGridDrawSplines->SetValue( m_bDistGridDrawSplines );
		sprintf( strTmp, "%.3f", m_nDistGridRaDivValue );
		m_pGridRaDivValue->SetValue( wxString(strTmp,wxConvUTF8) );
		sprintf( strTmp, "%.3f", m_nDistGridDecDivValue );
		m_pGridDecDivValue->SetValue( wxString(strTmp,wxConvUTF8) );
		sprintf( strTmp, "%.3f", m_nDistGridBumpFactValue );
		m_pDistGridBumpFactValue->SetValue( wxString(strTmp,wxConvUTF8) );

	}  else if( nType == GRAPHICS_TYPE_CONSTELLATION_LINES )
	{
		// obj path hide
		m_pGraphicsPathVertex->Hide();
		m_pGraphicsPathStartEnd->Hide();
		m_pGraphicsPathDirection->Hide();
		m_pGraphicsPathBaseLines->Hide();
		// hide dist grid
		sizerGraphicsStruct->Show( sizerGridBumpFact, 0, 1 );
		// the show sky/dist grid
		m_pSkyGridShowLabels->Hide();
		m_pGridDrawSplines->Hide();
		sizerGraphicsStruct->Show( sizerGridRaDiv, 0, 1 );
		sizerGraphicsStruct->Show( sizerGridDecDiv, 0, 1 );

		// show constellation
		m_pConstellationMarkStars->Show();
		m_pConstellationOutLine->Show();
		m_pConstellationGreekLabels->Show();

		// set my sample/reference text font 
		m_pGraphicsLineColourSample->SetForegroundColour( colGraphicsConstellationLines );
		m_pGraphicsLineColourSample->SetBackgroundColour( colGraphicsConstellationLines );
		// setline type
		m_pGraphicsLineStyle->SetSelection( m_nGraphicsConstellationLinesLine );
		// set struct values
		m_pConstellationMarkStars->SetValue( m_bConstellationMarkStars );
		m_pConstellationOutLine->SetValue( m_bConstellationOutLine );
		m_pConstellationGreekLabels->SetValue( m_bConstellationGreekLabels );
	} 

	// set prev graph type to get next time
	m_nGraphicsPrevType = nType;

	return;

}

////////////////////////////////////////////////////////////////////
// Method:		GetGraphicsType
// Class:		CMainConfigView
// Purpose:		save struct vars of graphics type
// Input:		graphics type
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CMainConfigView::GetGraphicsType( int nType )
{
	// now set my configuration based on label type
	if( nType == GRAPHICS_TYPE_OBJECT_PATH )
	{
		m_bGraphicsPathVertex = m_pGraphicsPathVertex->GetValue( );
		m_bGraphicsPathStartEnd = m_pGraphicsPathStartEnd->GetValue( );
		m_bGraphicsPathDirection = m_pGraphicsPathDirection->GetValue( );
		m_bGraphicsPathBaseLines = m_pGraphicsPathBaseLines->GetValue( );

	} else if( nType == GRAPHICS_TYPE_SKY_GRID )
	{
		m_bSkyGridShowLabels = m_pSkyGridShowLabels->GetValue( );
		m_bSkyGridDrawSplines = m_pGridDrawSplines->GetValue( );
		m_pGridRaDivValue->GetLineText(0).ToDouble( &m_nSkyGridRaDivValue );
		m_pGridDecDivValue->GetLineText(0).ToDouble( &m_nSkyGridDecDivValue );

	} else if( nType == GRAPHICS_TYPE_DISTORTION_GRID )
	{
		m_bDistGridDrawSplines = m_pGridDrawSplines->GetValue( );
		m_pGridRaDivValue->GetLineText(0).ToDouble( &m_nDistGridRaDivValue );
		m_pGridDecDivValue->GetLineText(0).ToDouble( &m_nDistGridDecDivValue );
		m_pDistGridBumpFactValue->GetLineText(0).ToDouble( &m_nDistGridBumpFactValue );

	} else if( nType == GRAPHICS_TYPE_CONSTELLATION_LINES )
	{
		m_bConstellationMarkStars = m_pConstellationMarkStars->GetValue( );
		m_bConstellationOutLine = m_pConstellationOutLine->GetValue( );
		m_bConstellationGreekLabels = m_pConstellationGreekLabels->GetValue( );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Purpose:	on label structure fields
////////////////////////////////////////////////////////////////////
void CMainConfigView::OnUseStruct( wxCommandEvent& event )
{
	// get selected format 
	int nType = m_pLabelType->GetSelection();

	if( nType == LABEL_TYPE_DETECTED_OBJECT )
	{
		m_bDetectObjLabelUseId = m_pLabelStructId->GetValue( );
		m_bDetectObjLabelUseMag = m_pLabelStructMag->GetValue( );

	} else if( nType == LABEL_TYPE_MATCHED_OBJECT )
	{
		m_bMatchObjLabelUseId = m_pLabelStructId->GetValue( );
		m_bMatchObjLabelUseCatNo = m_pLabelStructCatNo->GetValue( );
		m_bMatchObjLabelUseCName = m_pLabelStructCNames->GetValue( );

	} else if( nType == LABEL_TYPE_CATALOG_OBJECT )
	{
		m_bCatObjLabelUseCatNo = m_pLabelStructCatNo->GetValue( );
		m_bCatObjLabelUseCName = m_pLabelStructCNames->GetValue( );

	} else if( nType == LABEL_TYPE_CATALOG_NEBULA )
	{
		m_bNebObjLabelUseCatNo = m_pLabelStructCatNo->GetValue( );
		m_bNebObjLabelUseCName = m_pLabelStructCNames->GetValue( );
		m_bNebObjLabelUseType = m_pLabelStructType->GetValue( );

	} else if( nType == LABEL_TYPE_CATALOG_CLUSTER )
	{
		m_bCluObjLabelUseCatNo = m_pLabelStructCatNo->GetValue( );
		m_bCluObjLabelUseCName = m_pLabelStructCNames->GetValue( );
		m_bCluObjLabelUseType = m_pLabelStructType->GetValue( );

	} else if( nType == LABEL_TYPE_CATALOG_GALAXY )
	{
		m_bGalObjLabelUseCatNo = m_pLabelStructCatNo->GetValue( );
		m_bGalObjLabelUseCName = m_pLabelStructCNames->GetValue( );
		m_bGalObjLabelUseType = m_pLabelStructType->GetValue( );

	} else if( nType == LABEL_TYPE_CATALOG_SUPERNOVAE )
	{
		m_bSNObjLabelUseCatNo = m_pLabelStructCatNo->GetValue( );
		m_bSNObjLabelUseCName = m_pLabelStructCNames->GetValue( );
		m_bSNObjLabelUseType = m_pLabelStructType->GetValue( );

	} else if( nType == LABEL_TYPE_CATALOG_BLACKHOLE )
	{
		m_bBHObjLabelUseCatNo = m_pLabelStructCatNo->GetValue( );
		m_bBHObjLabelUseType = m_pLabelStructType->GetValue( );

	} else if( nType == LABEL_TYPE_CATALOG_RADIO )
	{
		m_bRadObjLabelUseCatNo = m_pLabelStructCatNo->GetValue( );
		m_bRadObjLabelUseType = m_pLabelStructType->GetValue( );

	} else if( nType == LABEL_TYPE_CATALOG_XRAY )
	{
		m_bXRayObjLabelUseCatNo = m_pLabelStructCatNo->GetValue( );
		m_bXRayObjLabelUseType = m_pLabelStructType->GetValue( );

	} else if( nType == LABEL_TYPE_CATALOG_GAMMA )
	{
		m_bGammaObjLabelUseCatNo = m_pLabelStructCatNo->GetValue( );
		m_bGammaObjLabelUseType = m_pLabelStructType->GetValue( );

	} else if( nType == LABEL_TYPE_CATALOG_MSTARS )
	{
		m_bMSObjLabelUseCatNo = m_pLabelStructCatNo->GetValue( );
		m_bMSObjLabelUseType = m_pLabelStructType->GetValue( );

	} else if( nType == LABEL_TYPE_CATALOG_AST )
	{
		m_bASTObjLabelUseCatNo = m_pLabelStructCatNo->GetValue( );
		m_bASTObjLabelUseType = m_pLabelStructType->GetValue( );

	} else if( nType == LABEL_TYPE_CATALOG_COM )
	{
		m_bCOMObjLabelUseCatNo = m_pLabelStructCatNo->GetValue( );
		m_bCOMObjLabelUseType = m_pLabelStructType->GetValue( );

	} else if( nType == LABEL_TYPE_CATALOG_EXP )
	{
		m_bEXPObjLabelUseCatNo = m_pLabelStructCatNo->GetValue( );
		m_bEXPObjLabelUseType = m_pLabelStructType->GetValue( );

	} else if( nType == LABEL_TYPE_CATALOG_AES )
	{
		m_bAESObjLabelUseCatNo = m_pLabelStructCatNo->GetValue( );
		m_bAESObjLabelUseType = m_pLabelStructType->GetValue( );
	}
	
	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnObjectShapeAndLine
// Class:		CMainConfigView
// Purpose:		on selection of object shape or line
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CMainConfigView::OnObjectShapeAndLine( wxCommandEvent& event )
{
	// get selected format 
	int nType = m_pLabelType->GetSelection();

	if( nType == LABEL_TYPE_DETECTED_OBJECT )
	{
		m_nDetectObjShapeType = m_pObjectShapeType->GetSelection();
		m_nDetectObjShapeLine = m_pObjectShapeLine->GetSelection();

	} else if( nType == LABEL_TYPE_MATCHED_OBJECT )
	{
		m_nMatchObjShapeType = m_pObjectShapeType->GetSelection();
		m_nMatchObjShapeLine = m_pObjectShapeLine->GetSelection();

	} else if( nType == LABEL_TYPE_CATALOG_OBJECT )
	{
		m_nCatObjShapeType = m_pObjectShapeType->GetSelection();
		m_nCatObjShapeLine = m_pObjectShapeLine->GetSelection();

	} else if( nType == LABEL_TYPE_CATALOG_NEBULA )
	{
		m_nNebObjShapeType = m_pObjectShapeType->GetSelection();
		m_nNebObjShapeLine = m_pObjectShapeLine->GetSelection();

	} else if( nType == LABEL_TYPE_CATALOG_CLUSTER )
	{
		m_nCluObjShapeType = m_pObjectShapeType->GetSelection();
		m_nCluObjShapeLine = m_pObjectShapeLine->GetSelection();

	} else if( nType == LABEL_TYPE_CATALOG_GALAXY )
	{
		m_nGalObjShapeType = m_pObjectShapeType->GetSelection();
		m_nGalObjShapeLine = m_pObjectShapeLine->GetSelection();

	} else if( nType == LABEL_TYPE_CATALOG_SUPERNOVAE )
	{
		m_nSNObjShapeType = m_pObjectShapeType->GetSelection();
		m_nSNObjShapeLine = m_pObjectShapeLine->GetSelection();

	} else if( nType == LABEL_TYPE_CATALOG_BLACKHOLE )
	{
		m_nBHObjShapeType = m_pObjectShapeType->GetSelection();
		m_nBHObjShapeLine = m_pObjectShapeLine->GetSelection();

	} else if( nType == LABEL_TYPE_CATALOG_RADIO )
	{
		m_nRadObjShapeType = m_pObjectShapeType->GetSelection();
		m_nRadObjShapeLine = m_pObjectShapeLine->GetSelection();

	} else if( nType == LABEL_TYPE_CATALOG_XRAY )
	{
		m_nXRayObjShapeType = m_pObjectShapeType->GetSelection();
		m_nXRayObjShapeLine = m_pObjectShapeLine->GetSelection();

	} else if( nType == LABEL_TYPE_CATALOG_GAMMA )
	{
		m_nGammaObjShapeType = m_pObjectShapeType->GetSelection();
		m_nGammaObjShapeLine = m_pObjectShapeLine->GetSelection();

	} else if( nType == LABEL_TYPE_CATALOG_MSTARS )
	{
		m_nMSObjShapeType = m_pObjectShapeType->GetSelection();
		m_nMSObjShapeLine = m_pObjectShapeLine->GetSelection();

	} else if( nType == LABEL_TYPE_CATALOG_AST )
	{
		m_nASTObjShapeType = m_pObjectShapeType->GetSelection();
		m_nASTObjShapeLine = m_pObjectShapeLine->GetSelection();

	} else if( nType == LABEL_TYPE_CATALOG_COM )
	{
		m_nCOMObjShapeType = m_pObjectShapeType->GetSelection();
		m_nCOMObjShapeLine = m_pObjectShapeLine->GetSelection();

	} else if( nType == LABEL_TYPE_CATALOG_EXP )
	{
		m_nEXPObjShapeType = m_pObjectShapeType->GetSelection();
		m_nEXPObjShapeLine = m_pObjectShapeLine->GetSelection();

	} else if( nType == LABEL_TYPE_CATALOG_AES )
	{
		m_nAESObjShapeType = m_pObjectShapeType->GetSelection();
		m_nAESObjShapeLine = m_pObjectShapeLine->GetSelection();
	}
	
	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnGraphicsLine
// Class:		CMainConfigView
// Purpose:		on selection of graphics line
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CMainConfigView::OnGraphicsLine( wxCommandEvent& event )
{
	// get selected format 
	int nType = m_pGraphicsType->GetSelection();

	if( nType == GRAPHICS_TYPE_OBJECT_PATH )
	{
		m_nGraphicsObjectPathLine = m_pGraphicsLineStyle->GetSelection();

	} else if( nType == GRAPHICS_TYPE_SKY_GRID )
	{
		m_nGraphicsSkyGridLine = m_pGraphicsLineStyle->GetSelection();

	} else if( nType == GRAPHICS_TYPE_DISTORTION_GRID )
	{
		m_nGraphicsDistortionGridLine = m_pGraphicsLineStyle->GetSelection();

	} else if( nType == GRAPHICS_TYPE_CONSTELLATION_LINES )
	{
		m_nGraphicsConstellationLinesLine = m_pGraphicsLineStyle->GetSelection();
	}

	return;
}

////////////////////////////////////////////////////////////////////
void CMainConfigView::OnSetGuiLanguage( wxCommandEvent& event )
{
	m_nGuiLanguage = m_pGuiLanguage->GetSelection();
}

////////////////////////////////////////////////////////////////////
void CMainConfigView::OnNotebookTab( wxNotebookEvent& pEvent )
{
	int nId = pEvent.GetSelection();
	// check for id of temporary page - 5
	if( nId == 5 ) 
	{
		// here initialize my temporary stuff
		if( !m_pFileManager->m_bTemporaryProcessed ) SetTemporaryList( );
	}
}

////////////////////////////////////////////////////////////////////
void CMainConfigView::OnTempListSelect( wxListEvent& pEvent )
{
	// get curren note selected
	long item = -1;
	// get first item only
    item = m_pTemporaryList->GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

	if( item >= 0 )
	{
		m_pTemporaryDelete->Enable();
		m_pTemporaryQuota->Enable();
		m_pTemporaryExplore->Enable();

	} else
	{
		m_pTemporaryDelete->Disable();
		m_pTemporaryQuota->Disable();
		m_pTemporaryExplore->Disable();
	}
}

////////////////////////////////////////////////////////////////////
void CMainConfigView::OnTempEmpty( wxCommandEvent& pEvent )
{
	int i=0;

	// get curren note selected
	long nId = -1;
    nId = m_pTemporaryList->GetNextItem( nId, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

	wxArrayString files;
	wxDir::GetAllFiles( m_pFileManager->m_vectTemporary[nId].path, &files );
	for( i=0; i<files.Count(); i++ )
	{
		wxRemoveFile( files[i] );
	}
}

////////////////////////////////////////////////////////////////////
void CMainConfigView::OnTempQuota( wxCommandEvent& pEvent )
{
	wxString strTmp;
	// get curren note selected
	long nId = -1;
    nId = m_pTemporaryList->GetNextItem( nId, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

	// pop value dialog
	CSimpleEntryAddDlg dlg( this, wxT("Set Folder Quota"), wxT("Maximum Quota:"), 2 );
	dlg.m_pNameSpin->SetValue( (int) m_pFileManager->m_vectTemporary[nId].quota );

	if( dlg.ShowModal() == wxID_OK )
	{
		m_pFileManager->m_vectTemporary[nId].quota = (unsigned long) dlg.m_pNameSpin->GetValue();
		// set quota in list
		if( m_pFileManager->m_vectTemporary[nId].quota == 0 )
			strTmp = wxT("No");
		else
			strTmp.Printf( wxT("%d"), m_pFileManager->m_vectTemporary[nId].quota );
		m_pTemporaryList->SetItem( nId, 3, strTmp );

	}
}

////////////////////////////////////////////////////////////////////
void CMainConfigView::OnTempExplore( wxCommandEvent& pEvent )
{
	// get curren note selected
	long nId = -1;
    nId = m_pTemporaryList->GetNextItem( nId, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

	wxFileName dirname( m_pFileManager->m_vectTemporary[nId].path, wxEmptyString );
	dirname.MakeAbsolute();

	wxString strCmd = wxT("Explorer.EXE ") + dirname.GetPath();;
	wxExecute( strCmd, wxEXEC_SYNC );
}
