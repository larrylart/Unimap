
#ifndef _VIEW_OBJECTS_DLG_H
#define _VIEW_OBJECTS_DLG_H

// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/spinctrl.h>

// defines
class CConfigMain;

// gui ids
enum
{
	wxID_VIEW_ALL_OBJECTS = 11500,
	wxID_VIEW_HIDE_ALL_OBJECTS,
	wxID_VIEW_HIDE_NO_MATCH,
	wxID_VIEW_OBJ_DETECTED,
	wxID_VIEW_OBJ_DETECTED_ALL,
	wxID_VIEW_CAT_STARS,
	wxID_VIEW_CAT_STARS_ALL,
	wxID_VIEW_CAT_DSO,
	wxID_VIEW_CAT_DSO_ALL,
	wxID_VIEW_CAT_GAMMA_XRAY,
	wxID_VIEW_CAT_GAMMA_XRAY_ALL,
	wxID_VIEW_CAT_RADIO,
	wxID_VIEW_CAT_RADIO_ALL,
	wxID_VIEW_CAT_MSTARS,
	wxID_VIEW_CAT_MSTARS_ALL,
	wxID_VIEW_CAT_SUPERNOVAS,
	wxID_VIEW_CAT_SUPERNOVAS_ALL,
	wxID_VIEW_CAT_BLACKHOLES,
	wxID_VIEW_CAT_BLACKHOLES_ALL,
	wxID_VIEW_CAT_EXO_PLANETS,
	wxID_VIEW_CAT_EXO_PLANETS_ALL,
	wxID_VIEW_CAT_ASTEROIDS,
	wxID_VIEW_CAT_ASTEROIDS_ALL,
	wxID_VIEW_CAT_COMETS,
	wxID_VIEW_CAT_COMETS_ALL,
	wxID_VIEW_CAT_SATELLITES,
	wxID_VIEW_CAT_SATELLITES_ALL,
};

// objs ids
enum
{
	VIEW_OBJ_DETECT = 0,
	VIEW_OBJ_CAT_STARS,
	VIEW_OBJ_CAT_DSO,
	VIEW_OBJ_CAT_GAMMA_XRAY,
	VIEW_OBJ_CAT_RADIO,
	VIEW_OBJ_CAT_MSTARS,
	VIEW_OBJ_CAT_SUPERNOVAS,
	VIEW_OBJ_CAT_BLACKHOLES,
	VIEW_OBJ_CAT_EXO_PLANETS,
	VIEW_OBJ_CAT_ASTEROIDS,
	VIEW_OBJ_CAT_COMETS,
	VIEW_OBJ_CAT_SATELLITES,
};

// external classes

// class:	CViewObjectsDlg
///////////////////////////////////////////////////////
class CViewObjectsDlg : public wxDialog
{
// public methods
public:
    CViewObjectsDlg( wxWindow *parent, CConfigMain* pMainConfig );
	virtual ~CViewObjectsDlg( );

	void SetConfig( CConfigMain* pMainConfig );
	void SetObjSrc( int nId, int nVal );
	int GetObjSrcVal( int nId );

// public data
public:
	wxFlexGridSizer* sizerList;
	
	// header
	wxCheckBox*		m_pViewAll;
	wxCheckBox*		m_pHideAll;
	wxCheckBox*		m_pHideNoMatch;

	// objects/sources props
	wxCheckBox*		m_vectObj[20];
	wxStaticText*	m_vectLabel[20];
	wxSpinCtrl*		m_vectLimit[20];
	wxChoice*		m_vectSort[20];
	wxChoice*		m_vectOrder[20];
	wxCheckBox*		m_vectAll[20];
	int m_nObj;

	// external objects
	CConfigMain* m_pMainConfig;

private:
	void AddObjOptions( int nId, int nIdAll, const wxString label, const wxString* vectSort, int nSort );
	void SetStateAll( int bState=1 );

	void OnViewHideAll( wxCommandEvent& pEvent );
	void SetViewHideAll( int nId, int bState );
	void OnViewObjects( wxCommandEvent& pEvent );
	void SetViewObjects( int nId, int bState );
	void OnObjViewObjects( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
