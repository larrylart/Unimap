
#ifndef _GROUPPROPERTY_H
#define _GROUPPROPERTY_H

#include "wx/wxprec.h"
#include <wx/notebook.h>
#include <wx/html/htmlwin.h>
#include <wx/html/htmlcell.h>
#include <wx/listctrl.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// dialg ids
enum
{
	wxID_IMAGE_GROUP_PROC_TYPE					= 19000,
	wxID_IMAGE_GROUP_CLASS_TYPE					= 19001,
	wxID_IMAGE_GROUP_IMGLIST_BUTTON_UP			= 19002,
	wxID_IMAGE_GROUP_IMGLIST_BUTTON_DOWN		= 19003,
	wxID_IMAGE_GROUP_STACK_FORMAT				= 19004,
	wxID_IMAGE_GROUP_IMGLIST					= 19005,
	wxID_IMAGE_GROUP_STACKLIST					= 19006,
	wxID_BUTTON_IMG_LIST_ADD_TO_STACK			= 19007,
	wxID_BUTTON_STACK_LIST_REMOVE_FROM_STACK	= 19008,
	// stack
	wxID_IMAGE_GROUP_STACKLIST_BUTTON_UP		= 19009,
	wxID_IMAGE_GROUP_STACKLIST_BUTTON_DOWN		= 19010,
	wxID_BUTTON_STACK_LIST_EDIT_PROPERTIES		= 19011,
	//
	wxID_BUTTON_IMG_LIST_ADD_IMAGES				= 19012,
	wxID_BUTTON_IMG_LIST_DELETE_IMAGES			= 19013,
	wxID_IMAGE_GROUP_STACK_METHOD				= 19014,
	wxID_BUTTON_STACK_BIAS						= 19015,
	wxID_BUTTON_STACK_DARK						= 19016,
	wxID_BUTTON_STACK_FLAT						= 19017,
	wxID_IMAGE_GROUP_STACK_BLUR_METHOD			= 19018,
	wxID_OBJ_HISTOGRAM_CMP_METHOD				= 19019
};

// external classes
class CAstroImage;
class CImageGroup;

// class:	CGroupProperty
///////////////////////////////////////////////////////
class CGroupProperty : public wxDialog
{
// methods
public:
    CGroupProperty( wxWindow *parent, const wxString& title );
	~CGroupProperty( );

	void SetConfig( CImageGroup* pImageGroup, int nUnitsFormat );
	void SetImageList( );
	void SetStackList( );

// data
public:
	// config and object references
	CImageGroup* m_pImageGroup;

	// params
	wxChoice*	m_pGroupProcType;
	wxChoice*	m_pGroupClassType;
	wxCheckBox*	m_pGroupSaveAlignedOver;
	wxChoice*	m_pGroupStackFormat;
	wxCheckBox*	m_pGroupStackSize;
	wxCheckBox*	m_pGroupStackTranslate;
	wxCheckBox*	m_pGroupStackDelTemp;

	// stacking
	wxChoice*		m_pGroupStackMethod;
	wxTextCtrl*		m_pBiasFrameEntry;
	wxButton*		m_pBiasFrameButton;
	wxTextCtrl*		m_pDarkFrameEntry;
	wxButton*		m_pDarkFrameButton;
	wxTextCtrl*		m_pFlatFrameEntry;
	wxButton*		m_pFlatFrameButton;
	wxTextCtrl*		m_pStackSigma;
	wxTextCtrl*		m_pStackIterations;
	wxChoice*		m_pStackFrameBlurMethod;

	// discovery
	wxChoice*		m_pObjHistogramCmpMethod;
	wxTextCtrl*		m_pObjHistogramNormalize;
	wxTextCtrl*		m_pDynamicObjMatchMaxFwhm;
	wxTextCtrl*		m_pDynamicObjMinDistMatch;
	wxTextCtrl*		m_pObjDynamicsMagTol;
	wxTextCtrl*		m_pObjDynamicsKronFactorTol;
	wxTextCtrl*		m_pObjDynamicsFwhmTol;
	wxTextCtrl*		m_pPathDevAngleTolerance;

	// image list 
	wxListCtrl*		m_pImageListData;
	wxBitmapButton*	m_pImageListButtonUp;
	wxBitmapButton*	m_pImageListButtonDown;
	wxButton*		m_pImageListAddImages;
	wxButton*		m_pImageListDeleteImages;
	wxButton*		m_pImageListAddToStack;
	// stack list
	wxListCtrl*		m_pStackListData;
	wxBitmapButton*	m_pStackListButtonUp;
	wxBitmapButton*	m_pStackListButtonDown;
	wxButton*		m_pStackListRemoveFromStack;
	wxButton*		m_pStackListEditProperties;

	////////
	// sorting flags etc
	int m_bSortImgListByIdAsc;
	int m_bSortImgListByNameAsc;
	int m_bSortImgListByFileTimeAsc;
	// :: stack image list sorting flags
	int m_bSortStackListByIdAsc;
	int m_bSortStackListByNameAsc;
	int m_bSortStackListByFileTimeAsc;

// private methods
private:
	// list buttons
	void OnImgListButtonUpDown( wxCommandEvent& pEvent );
	void OnImgListAddToStack( wxCommandEvent& pEvent );
	void OnImgListAddImages( wxCommandEvent& pEvent );
	void OnImgListDeleteImages( wxCommandEvent& pEvent );
	// image list events
	void OnImgListSelect( wxListEvent& pEvent );
	void OnImgListColumn( wxListEvent& pEvent );
	// stack list events
	void OnStackListSelect( wxListEvent& pEvent );
	void OnStackListColumn( wxListEvent& pEvent );
	// stack buttons
	void OnStackListButtonUpDown( wxCommandEvent& pEvent );
	void OnStackListRemoveImage( wxCommandEvent& pEvent );
	// stack properties methods
	void OnSelectImageFile( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif

