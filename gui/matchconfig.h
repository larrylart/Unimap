////////////////////////////////////////////////////////////////////
// Package:		Match configuration header/decalration
// File:		matchconfig.h
// Purpose:		GUI dialog window to config matching process
//
// Created by:	Larry Lart on 11/02/2007
// Updated by:  Larry Lart on 05/02/2010
//
// Copyright:	(c) 2007-2010 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _MATCH_CONFIG
#define _MATCH_CONFIG

// include some wxwidgets headers
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/notebook.h"
#include <wx/spinctrl.h>

// enumeration of windows ids
enum
{
	wxID_TRANS_TYPE_SELECT		= 11500,
	wxID_CHECK_RECALC_MATCH		= 11501,
	wxID_SKY_AREA_SEARCH_METHOD	= 11502,
	wxID_IMAGE_OBJECT_EXTRACTION_BY_OBJ_SHAPE	= 11503,
	wxID_IMAGE_OBJECT_EXTRACTION_BY_DETECT_TYPE	= 11504,
	// proc panel elem ids
	wxID_SKY_AREA_PROJECTION_METHOD			= 11510,
	wxID_MATCH_DO_FINETUNE					= 11511,
	wxID_MATCH_DO_FINETUNE_WITH_PROJECTION	= 11512,
	wxID_MATCH_BY_MOST_STARS				= 11513,
	wxID_MATCH_BY_BEST_FIT					= 11514,
	wxID_MATCH_REFINE_BEST_AREA				= 11515,
	wxID_MATCH_STOP_ON_FIRST_FOUND			= 11516,
	wxID_MATCH_STOP_MIN_STARS_NEEDED		= 11517,
	wxID_REVERSE_ZOOM_SEARCH				= 11518,
	wxID_MATCH_ASSOC_MAX_DISTANCE			= 11519,
	// finder
	wxID_FINDER_CAT_STARS,

};


// class:	CMatchConfig
///////////////////////////////////////////////////////
class CMatchConfig : public wxDialog
{
// methods
public:
    CMatchConfig( wxWindow *parent, const wxString& title );
	~CMatchConfig();

	void SetConfig( CConfigMain* pMainConfig );

// data
public:

	// search panel elements
	wxChoice*		m_pSearchMethod;
	wxTextCtrl*		m_pSearchAreaIncrementOnRA;
	wxTextCtrl*		m_pSearchAreaIncrementOnDEC;
	wxTextCtrl*		m_pRaSearchOverlapDivision;
	wxTextCtrl*		m_pDecSearchOverlapDivision;
	wxTextCtrl*		m_pSearchZoomSteps;
	wxTextCtrl*		m_pSearchZoomStart;
	wxCheckBox*		m_pDoReverseZoomSearch;
	
	// proc panel elements
	wxChoice*		m_pProjectionMethod;
	wxCheckBox*		m_pDoMatchFineTune;
	wxCheckBox*		m_pDoMatchFineTuneWithProjection;
	wxCheckBox*		m_pMatchByMostStars;
	wxCheckBox*		m_pMatchByBestFit;
	wxCheckBox*		m_pDoMatchRefineBestArea;
	wxCheckBox*		m_pDoMatchStopOnFirstFound;
	wxTextCtrl*		m_pMatchStopOnMinFound;
	wxTextCtrl*		m_pAssocMatchMaxDist;

	// objects panel elements
	wxCheckBox*		m_pImgObjectExtractByObjectShape;
	wxCheckBox*		m_pImgObjectExtractByDetectionType;
	wxTextCtrl*		m_pImgNoObjMatch;
	wxTextCtrl*		m_pCatNoObjMatch;
	wxTextCtrl*		m_pSearchMatchMinCatStars;
	wxTextCtrl*		m_pMatchFirstLevelMinStars;
	wxTextCtrl*		m_pMatchFineTuneImgStarsFactor;
	wxTextCtrl*		m_pMatchFineTuneCatStarsFactor;	

	// match panel elements
	wxChoice*		m_pTransTypeSelect;
	wxCheckBox*		m_pMatchRecalc;
	wxTextCtrl*		m_pMaxMatchDist;
	wxTextCtrl*		m_pTriangFit;
	wxTextCtrl*		m_pTriangScale;
	wxTextCtrl*		m_pMaxIter;
	wxTextCtrl*		m_pHaltSigma;

	// :: FINDER :: panel elements
	wxCheckBox*		m_pLocateCatStars;
	wxCheckBox*		m_pLocateDso;
	wxCheckBox*		m_pLocateXGamma;
	wxCheckBox*		m_pLocateRadio;
	wxCheckBox*		m_pLocateSupernovas;
	wxCheckBox*		m_pLocateBlackholes;
	wxCheckBox*		m_pLocateMStars;
	wxCheckBox*		m_pLocateExoplanets;
	wxCheckBox*		m_pLocateAsteroids;
	wxCheckBox*		m_pLocateComets;
	wxCheckBox*		m_pLocateAes;
	wxCheckBox*		m_pLocateSolar;
	wxCheckBox*		m_pFecthDetailsOnFind;

	// :: SOLAR :: panel elements
	wxSpinCtrl*		m_pTimeFrameRange;
	wxSpinCtrl*		m_pTimeFrameDivision;
	wxCheckBox*		m_pOnlyTleInRange;
	wxSpinCtrl*		m_pTleInRangeValue;

	wxFlexGridSizer*	sizerObjectsPanel;

	wxNotebook *pNotebook;

	void OnValidate( wxCommandEvent& event );

private:
	void OnCheckStopOnFirstFound( wxCommandEvent& event );

	DECLARE_EVENT_TABLE()
};

#endif
