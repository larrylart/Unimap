
#ifndef _SKY_OBJECT_SELECT_DLG_H
#define _SKY_OBJECT_SELECT_DLG_H

#include "wx/wxprec.h"
#include <wx/listctrl.h>

// object type ids
enum
{
	SKYOBJ_TYPE_STAR_NAMES = 0,
	SKYOBJ_TYPE_DSO_NAMES,
	SKYOBJ_TYPE_CONST_NAMES,
	SKYOBJ_TYPE_MESSIER_ID,
	SKYOBJ_TYPE_NGC_ID,
	SKYOBJ_TYPE_IC_ID,
	SKYOBJ_TYPE_SAO_ID
};

// gui ids
enum
{
	wxID_SKY_OBJ_FILTER_SELECT = 9800,
	wxID_SKY_OBJ_CAT_SELECT,
	wxID_SKY_OBJ_NAME_SELECT,
	wxID_SKY_OBJ_NAME_ENTRY
};

// external classes
class CSky;
class CConfigMain;

// class:	CSkyObjSelectDlg
///////////////////////////////////////////////////////
class CSkyObjSelectDlg : public wxDialog
{
// methods
public:
    CSkyObjSelectDlg( wxWindow *parent, const wxString& title );
	~CSkyObjSelectDlg( );

	void SetConfig( CSky* pSky );
	void SetOrig( );

/////////////
// ::DATA
public:
	CSky* m_pSky;
	CConfigMain* m_pMainConfig;

	int m_nSkyObjType;

	double m_nRaDeg;
	double m_nDecDeg;

	wxChoice*		m_pObjFilter;
	wxChoice*		m_pObjCatSelect;
	wxStaticText*	m_pObjectLabel;
	wxChoice*		m_pObjNameSelect;
	wxTextCtrl*		m_pObjectId;
	// view
	wxStaticText*	m_pObjectRA;
	wxStaticText*	m_pObjectDEC;
	wxStaticText*	m_pObjectMAG;

private:
	void OnSelectType( wxCommandEvent& pEvent );
	void SetSelectType( );
	void SetObjectNameList( int nType );

	void OnSelectName( wxCommandEvent& pEvent );
	void OnSetObjectId( wxCommandEvent& pEvent );

//	void OnSetObjectId( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
