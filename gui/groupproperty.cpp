
#include "wx/wxprec.h"
#include "wx/statline.h"
#include "wx/minifram.h"
#include "wx/thread.h"
#include "wx/notebook.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// opencv includes
#include "cv.h"

// other local header
#include "frame.h"
#include "groupdlg.h"
//#include "../config/config.h"
#include "../config/mainconfig.h"
#include "../util/func.h"
#include "../image/astroimage.h"
#include "../image/imagegroup.h"
#include "../image/imagedb.h"
// resources
#include "../cresources/up.xpm"
#include "../cresources/down.xpm"

// main header
#include "groupproperty.h"

//		***** CLASS CGroupProperty *****
////////////////////////////////////////////////////////////////////
// CImageTextProperty EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CGroupProperty, wxDialog)
//	EVT_CHECKBOX( wxID_CHECKBOX_TIMEFRAME, CGroupProperty::OnCheckBox )

	// image list buttons
	EVT_BUTTON( wxID_IMAGE_GROUP_IMGLIST_BUTTON_UP, CGroupProperty::OnImgListButtonUpDown )
	EVT_BUTTON( wxID_IMAGE_GROUP_IMGLIST_BUTTON_DOWN, CGroupProperty::OnImgListButtonUpDown )
	EVT_BUTTON( wxID_BUTTON_IMG_LIST_ADD_IMAGES, CGroupProperty::OnImgListAddImages )
	EVT_BUTTON( wxID_BUTTON_IMG_LIST_DELETE_IMAGES, CGroupProperty::OnImgListDeleteImages )
	EVT_BUTTON( wxID_BUTTON_IMG_LIST_ADD_TO_STACK, CGroupProperty::OnImgListAddToStack )
	// selection
	EVT_LIST_ITEM_SELECTED( wxID_IMAGE_GROUP_IMGLIST, CGroupProperty::OnImgListSelect )
	EVT_LIST_ITEM_DESELECTED( wxID_IMAGE_GROUP_IMGLIST, CGroupProperty::OnImgListSelect )
	EVT_LIST_COL_CLICK( wxID_IMAGE_GROUP_IMGLIST, CGroupProperty::OnImgListColumn )
	EVT_LIST_COL_CLICK( wxID_IMAGE_GROUP_STACKLIST, CGroupProperty::OnStackListColumn )
	// stack list buttons
	EVT_BUTTON( wxID_IMAGE_GROUP_STACKLIST_BUTTON_UP, CGroupProperty::OnStackListButtonUpDown )
	EVT_BUTTON( wxID_IMAGE_GROUP_STACKLIST_BUTTON_DOWN, CGroupProperty::OnStackListButtonUpDown )
	EVT_BUTTON( wxID_BUTTON_STACK_LIST_REMOVE_FROM_STACK, CGroupProperty::OnStackListRemoveImage )
	// selection
	EVT_LIST_ITEM_SELECTED( wxID_IMAGE_GROUP_STACKLIST, CGroupProperty::OnStackListSelect )
	EVT_LIST_ITEM_DESELECTED( wxID_IMAGE_GROUP_STACKLIST, CGroupProperty::OnStackListSelect )
	// stack properties events
	EVT_BUTTON( wxID_BUTTON_STACK_BIAS, CGroupProperty::OnSelectImageFile )
	EVT_BUTTON( wxID_BUTTON_STACK_DARK, CGroupProperty::OnSelectImageFile )
	EVT_BUTTON( wxID_BUTTON_STACK_FLAT, CGroupProperty::OnSelectImageFile )

END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CGroupProperty
// Purpose:	Initialize add devicedialog
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CGroupProperty::CGroupProperty( wxWindow *parent, const wxString& title ) : wxDialog(parent, -1,
                          title,
                          wxDefaultPosition,
                          wxDefaultSize,
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL|wxRESIZE_BORDER  )
{
	int width, height;
	this->GetClientSize(&width, &height);

	// init flags
	m_bSortImgListByIdAsc = 0;
	m_bSortImgListByNameAsc = 0;
	m_bSortImgListByFileTimeAsc = 0;
	m_bSortStackListByIdAsc = 0;
	m_bSortStackListByNameAsc = 0;
	m_bSortStackListByFileTimeAsc = 0;

	wxString vectGroupProcType[8];
	wxString vectGroupClassType[8];
	wxString vectGroupStackFormat[8];
	wxString vectGroupStackMethod[4];
	wxString vectStackFrameBlurMethod[2];
	wxString vectObjHistogramCmpMethod[4];

	// set image proc type defaults
	vectGroupProcType[0] = wxT("Image Collection");
	vectGroupProcType[1] = wxT("Stack");
	vectGroupProcType[2] = wxT("Mixed");
	// set image proc type defaults
	vectGroupClassType[0] = wxT("None");
	vectGroupClassType[1] = wxT("Reference");
	vectGroupClassType[2] = wxT("Test");
	// image format types defaults
	vectGroupStackFormat[0] = wxT("Orig");
	vectGroupStackFormat[1] = wxT("Jpg");
	vectGroupStackFormat[2] = wxT("Bmp");
	// stack method
	vectGroupStackMethod[0] = wxT("Average");
	vectGroupStackMethod[1] = wxT("Median");
	vectGroupStackMethod[2] = wxT("Mean-Median");
	vectGroupStackMethod[3] = wxT("Kappa-Sigma");
	// blur methods
	vectStackFrameBlurMethod[0] = wxT("None");
	vectStackFrameBlurMethod[1] = wxT("Gaussian");
	// histogram compare methods
	vectObjHistogramCmpMethod[0] = wxT("Correlation");
	vectObjHistogramCmpMethod[1] = wxT("Chi-Square");
	vectObjHistogramCmpMethod[2] = wxT("Intersect"); 
	vectObjHistogramCmpMethod[3] = wxT("Bhattacharyya");

	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	// building configure as a note book
	wxNotebook* pNotebook = new wxNotebook( this, -1 ); 

	// add panel :: param - image size time file date
	wxPanel* pPanelParam = new wxPanel( pNotebook );
	// add panel :: stacking - method, bias frame etc
	wxPanel* pPanelStacking = new wxPanel( pNotebook );
	// add panel :: discovery - method, bias frame etc
	wxPanel* pPanelDiscovery = new wxPanel( pNotebook );
	// anel :: image list - image exif data
	wxPanel* pPanelImageList = new wxPanel( pNotebook );
	// add panel :: Observer - name, location name/long/lat, date time 
	wxPanel* pPanelStackList = new wxPanel( pNotebook );

	///////////////////////////////
	// layout of group of elements in the panel
	// :: Param
	wxFlexGridSizer* sizerParamPanel = new wxFlexGridSizer( 7, 2, 3, 3 );
	sizerParamPanel->AddGrowableCol( 1 );
	wxGridSizer* sizerParamPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerParamPage->Add( sizerParamPanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// :: Stacking
	wxFlexGridSizer* sizerStackingPanel = new wxFlexGridSizer( 7, 3, 3, 3 );
	sizerStackingPanel->AddGrowableCol( 1 );
	wxGridSizer* sizerStackingPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerStackingPage->Add( sizerStackingPanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// :: Discovery
	wxFlexGridSizer* sizerDiscoveryPanel = new wxFlexGridSizer( 7, 2, 3, 3 );
	sizerDiscoveryPanel->AddGrowableCol( 1 );
	wxGridSizer* sizerDiscoveryPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerDiscoveryPage->Add( sizerDiscoveryPanel, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// :: image list
	wxStaticBox* pImageListBoxPanel = new wxStaticBox( pPanelImageList, -1, wxT("Group List Of All Images"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pImageListBoxSizer = new wxStaticBoxSizer( pImageListBoxPanel, wxHORIZONTAL );
	wxFlexGridSizer* sizerImageListData = new wxFlexGridSizer( 1, 1, 10, 10 );
	sizerImageListData->AddGrowableCol( 0 );
	sizerImageListData->AddGrowableRow( 0 );
	sizerImageListData->Add( pImageListBoxSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxEXPAND );
	// :: stack list
	wxStaticBox* pStackListBoxPanel = new wxStaticBox( pPanelStackList, -1, wxT("Stack List Of All Images"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pStackListBoxSizer = new wxStaticBoxSizer( pStackListBoxPanel, wxHORIZONTAL );
	wxFlexGridSizer* sizerStackListData = new wxFlexGridSizer( 1, 1, 10, 10 );
	sizerStackListData->AddGrowableCol( 0 );
	sizerStackListData->AddGrowableRow( 0 );
	sizerStackListData->Add( pStackListBoxSizer, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxEXPAND );

	///////////////////////////////////
	// :: bitmaps used
	wxBitmap bmpUp = wxBitmap( up_xpm );
	wxBitmap bmpDown = wxBitmap( down_xpm );
	////////
	// :: create param panel elements
	m_pGroupProcType = new wxChoice( pPanelParam, wxID_IMAGE_GROUP_PROC_TYPE,
										wxDefaultPosition,
										wxSize(110,-1), 3, vectGroupProcType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pGroupProcType->SetSelection( 0 );
	m_pGroupClassType = new wxChoice( pPanelParam, wxID_IMAGE_GROUP_CLASS_TYPE,
										wxDefaultPosition,
										wxSize(80,-1), 3, vectGroupClassType, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pGroupClassType->SetSelection( 0 );
	m_pGroupSaveAlignedOver = new wxCheckBox( pPanelParam, -1, wxT("") );
	m_pGroupStackFormat = new wxChoice( pPanelParam, wxID_IMAGE_GROUP_STACK_FORMAT,
										wxDefaultPosition,
										wxSize(50,-1), 3, vectGroupStackFormat, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pGroupStackFormat->SetSelection( 0 );
	m_pGroupStackSize = new wxCheckBox( pPanelParam, -1, wxT("") );
	m_pGroupStackTranslate = new wxCheckBox( pPanelParam, -1, wxT("") );
	m_pGroupStackDelTemp = new wxCheckBox( pPanelParam, -1, wxT("") );

	////////
	// :: create Stacking panel elements
	m_pGroupStackMethod = new wxChoice( pPanelStacking, wxID_IMAGE_GROUP_STACK_METHOD,
										wxDefaultPosition,
										wxSize(110,-1), 4, vectGroupStackMethod, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pGroupStackMethod->SetSelection( 0 );
	m_pStackSigma = new wxTextCtrl( pPanelStacking, -1, wxT(""), wxDefaultPosition, wxSize(50,-1) );
	m_pStackIterations = new wxTextCtrl( pPanelStacking, -1, wxT(""), wxDefaultPosition, wxSize(30,-1) );
	m_pBiasFrameEntry = new wxTextCtrl( pPanelStacking, -1, wxT(""), wxDefaultPosition, wxSize(160,-1) );
	m_pDarkFrameEntry = new wxTextCtrl( pPanelStacking, -1, wxT(""), wxDefaultPosition, wxSize(160,-1) );
	m_pFlatFrameEntry = new wxTextCtrl( pPanelStacking, -1, wxT(""), wxDefaultPosition, wxSize(160,-1) );
	m_pBiasFrameButton = new wxButton( pPanelStacking, wxID_BUTTON_STACK_BIAS, wxT("Browse ..."), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pDarkFrameButton = new wxButton( pPanelStacking, wxID_BUTTON_STACK_DARK, wxT("Browse ..."), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pFlatFrameButton = new wxButton( pPanelStacking, wxID_BUTTON_STACK_FLAT, wxT("Browse ..."), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pStackFrameBlurMethod = new wxChoice( pPanelStacking, wxID_IMAGE_GROUP_STACK_BLUR_METHOD,
										wxDefaultPosition,
										wxSize(110,-1), 2, vectStackFrameBlurMethod, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pStackFrameBlurMethod->SetSelection( 0 );

	////////
	// :: create Discovery panel elements
	m_pObjHistogramCmpMethod = new wxChoice( pPanelDiscovery, wxID_OBJ_HISTOGRAM_CMP_METHOD,
										wxDefaultPosition,
										wxSize(110,-1), 4, vectObjHistogramCmpMethod, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pObjHistogramCmpMethod->SetSelection( 0 );
	m_pObjHistogramNormalize = new wxTextCtrl( pPanelDiscovery, -1, wxT(""), wxDefaultPosition, wxSize(80,-1) );
	m_pDynamicObjMatchMaxFwhm = new wxTextCtrl( pPanelDiscovery, -1, wxT(""), wxDefaultPosition, wxSize(80,-1) );
	m_pDynamicObjMinDistMatch = new wxTextCtrl( pPanelDiscovery, -1, wxT(""), wxDefaultPosition, wxSize(50,-1) );
	m_pObjDynamicsMagTol = new wxTextCtrl( pPanelDiscovery, -1, wxT(""), wxDefaultPosition, wxSize(80,-1) );
	m_pObjDynamicsKronFactorTol = new wxTextCtrl( pPanelDiscovery, -1, wxT(""), wxDefaultPosition, wxSize(80,-1) );
	m_pObjDynamicsFwhmTol = new wxTextCtrl( pPanelDiscovery, -1, wxT(""), wxDefaultPosition, wxSize(80,-1) );
	m_pPathDevAngleTolerance = new wxTextCtrl( pPanelDiscovery, -1, wxT(""), wxDefaultPosition, wxSize(80,-1) );

	//////////
	// :: create image list objects
	m_pImageListData = new wxListCtrl( pPanelImageList, wxID_IMAGE_GROUP_IMGLIST, wxPoint(-1,-1), wxSize(360,200), wxLC_REPORT|wxLC_HRULES|wxLC_VRULES );
	m_pImageListButtonUp = new wxBitmapButton( pPanelImageList, wxID_IMAGE_GROUP_IMGLIST_BUTTON_UP, bmpUp );
	m_pImageListButtonDown = new wxBitmapButton( pPanelImageList, wxID_IMAGE_GROUP_IMGLIST_BUTTON_DOWN, bmpDown );
	// disabled by default 
	m_pImageListButtonUp->Disable();
	m_pImageListButtonDown->Disable();
	// other buttons
	m_pImageListAddImages = new wxButton( pPanelImageList, wxID_BUTTON_IMG_LIST_ADD_IMAGES, wxT("Add Image(s)"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pImageListDeleteImages = new wxButton( pPanelImageList, wxID_BUTTON_IMG_LIST_DELETE_IMAGES, wxT("Delete Image(s)"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pImageListAddToStack = new wxButton( pPanelImageList, wxID_BUTTON_IMG_LIST_ADD_TO_STACK, wxT("Add To The Stack List"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	// disabled by default 
//	m_pImageListAddImages->Disable();
	m_pImageListDeleteImages->Disable();
	m_pImageListAddToStack->Disable();

	//////////
	// :: create stack list objects
	m_pStackListData = new wxListCtrl( pPanelStackList, wxID_IMAGE_GROUP_STACKLIST, wxPoint(-1,-1), wxSize(360,200), wxLC_REPORT|wxLC_HRULES|wxLC_VRULES );
	m_pStackListButtonUp = new wxBitmapButton( pPanelStackList, wxID_IMAGE_GROUP_STACKLIST_BUTTON_UP, bmpUp );
	m_pStackListButtonDown = new wxBitmapButton( pPanelStackList, wxID_IMAGE_GROUP_STACKLIST_BUTTON_DOWN, bmpDown );
	// disabled by default 
	m_pStackListButtonUp->Disable();
	m_pStackListButtonDown->Disable();
	// bottom buttons
	m_pStackListRemoveFromStack = new wxButton( pPanelStackList, wxID_BUTTON_STACK_LIST_REMOVE_FROM_STACK, wxT("Remove From Stack"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pStackListEditProperties = new wxButton( pPanelStackList, wxID_BUTTON_STACK_LIST_EDIT_PROPERTIES, wxT("Edit Stack Frame Properties"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	// disabled by default 
	m_pStackListEditProperties->Disable();
	m_pStackListRemoveFromStack->Disable();

	////////////////
	// Populate PARAM sizer
	// :: group proc type
	sizerParamPanel->Add( new wxStaticText( pPanelParam, -1, wxT("Group Proc Type:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerParamPanel->Add( m_pGroupProcType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: group class type
	sizerParamPanel->Add( new wxStaticText( pPanelParam, -1, wxT("Group Class Type:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerParamPanel->Add( m_pGroupClassType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: save aligned over
	sizerParamPanel->Add( new wxStaticText( pPanelParam, -1, wxT("Save Aligned Over:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerParamPanel->Add( m_pGroupSaveAlignedOver, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: group stack format
	sizerParamPanel->Add( new wxStaticText( pPanelParam, -1, wxT("Align Images Format:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerParamPanel->Add( m_pGroupStackFormat, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: preserve image size
	sizerParamPanel->Add( new wxStaticText( pPanelParam, -1, wxT("Preserve Image size:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerParamPanel->Add( m_pGroupStackSize, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: Do image translation
	sizerParamPanel->Add( new wxStaticText( pPanelParam, -1, wxT("Apply Image Translation:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerParamPanel->Add( m_pGroupStackTranslate, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: Delete temporary files
	sizerParamPanel->Add( new wxStaticText( pPanelParam, -1, wxT("Delete Temporary Files:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerParamPanel->Add( m_pGroupStackDelTemp, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );

	////////////////
	// Populate STACKING sizer
	// :: stack method
	sizerStackingPanel->Add( new wxStaticText( pPanelStacking, -1, wxT("Stacking Method:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackingPanel->Add( m_pGroupStackMethod, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackingPanel->AddStretchSpacer( );
	// :: stack sigma
	sizerStackingPanel->Add( new wxStaticText( pPanelStacking, -1, wxT("Stacking Sigma:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackingPanel->Add( m_pStackSigma, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackingPanel->AddStretchSpacer( );
	// :: stack iterartions
	sizerStackingPanel->Add( new wxStaticText( pPanelStacking, -1, wxT("Iterations:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackingPanel->Add( m_pStackIterations, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackingPanel->AddStretchSpacer( );
	// :: bias frame
	sizerStackingPanel->Add( new wxStaticText( pPanelStacking, -1, wxT("Bias Frame:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackingPanel->Add( m_pBiasFrameEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackingPanel->Add( m_pBiasFrameButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: dark frame
	sizerStackingPanel->Add( new wxStaticText( pPanelStacking, -1, wxT("Dark Frame:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackingPanel->Add( m_pDarkFrameEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackingPanel->Add( m_pDarkFrameButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: flat frame
	sizerStackingPanel->Add( new wxStaticText( pPanelStacking, -1, wxT("Flat Frame:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackingPanel->Add( m_pFlatFrameEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackingPanel->Add( m_pFlatFrameButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: blur method
	sizerStackingPanel->Add( new wxStaticText( pPanelStacking, -1, wxT("Blur Method:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackingPanel->Add( m_pStackFrameBlurMethod, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackingPanel->AddStretchSpacer( );

	////////////////
	// Populate Discovery sizer
	// :: histogram compare method
	sizerDiscoveryPanel->Add( new wxStaticText( pPanelDiscovery, -1, wxT("Histogram Compare Method:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerDiscoveryPanel->Add( m_pObjHistogramCmpMethod, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: ObjHistogramNormalize
	sizerDiscoveryPanel->Add( new wxStaticText( pPanelDiscovery, -1, wxT("Histogram Normalization:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerDiscoveryPanel->Add( m_pObjHistogramNormalize, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: DynamicObjMatchMaxFwhm
	sizerDiscoveryPanel->Add( new wxStaticText( pPanelDiscovery, -1, wxT("Extraction Max FWHM:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerDiscoveryPanel->Add( m_pDynamicObjMatchMaxFwhm, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: DynamicObjMinDistMatch
	sizerDiscoveryPanel->Add( new wxStaticText( pPanelDiscovery, -1, wxT("Match Maximum Distance:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerDiscoveryPanel->Add( m_pDynamicObjMinDistMatch, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: m_pObjDynamicsMagTol
	sizerDiscoveryPanel->Add( new wxStaticText( pPanelDiscovery, -1, wxT("Magnitude Variation:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerDiscoveryPanel->Add( m_pObjDynamicsMagTol, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: m_pObjDynamicsKronFactorTol
	sizerDiscoveryPanel->Add( new wxStaticText( pPanelDiscovery, -1, wxT("Kron Factor Variation:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerDiscoveryPanel->Add( m_pObjDynamicsKronFactorTol, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: m_pObjDynamicsFwhmTol
	sizerDiscoveryPanel->Add( new wxStaticText( pPanelDiscovery, -1, wxT("FWHM Variation:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerDiscoveryPanel->Add( m_pObjDynamicsFwhmTol, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	// :: m_pPathDevAngleTolerance
	sizerDiscoveryPanel->Add( new wxStaticText( pPanelDiscovery, -1, wxT("Maximum Path Deviation Angle:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerDiscoveryPanel->Add( m_pPathDevAngleTolerance, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );

	////////////////
	// Populate IMAGE LIST sizer
	pImageListBoxSizer->Add( m_pImageListData, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL|wxEXPAND, 2  );
	// add left buttons
	wxGridSizer* sizerImageListLeftButtons = new wxGridSizer( 2, 1, 10, 10 );
	sizerImageListLeftButtons->Add( m_pImageListButtonUp, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerImageListLeftButtons->Add( m_pImageListButtonDown, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	pImageListBoxSizer->Add( sizerImageListLeftButtons, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5 );
	// add bottom buttons
	wxFlexGridSizer* sizerImageListDownButtons = new wxFlexGridSizer( 1, 3, 10, 10 );
	sizerImageListDownButtons->Add( m_pImageListAddImages, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerImageListDownButtons->Add( m_pImageListDeleteImages, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerImageListDownButtons->Add( m_pImageListAddToStack, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerImageListData->Add( sizerImageListDownButtons, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 2  );

	////////////////
	// Populate STACK LIST sizer
	pStackListBoxSizer->Add( m_pStackListData, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL|wxEXPAND, 2  );
	// add left buttons
	wxGridSizer* sizerStackListLeftButtons = new wxGridSizer( 2, 1, 10, 10 );
	sizerStackListLeftButtons->Add( m_pStackListButtonUp, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerStackListLeftButtons->Add( m_pStackListButtonDown, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	pStackListBoxSizer->Add( sizerStackListLeftButtons, 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5 );
	// add bottom buttons
	wxGridSizer* sizerStackListDownButtons = new wxGridSizer( 1, 2, 10, 10 );
	sizerStackListDownButtons->Add( m_pStackListRemoveFromStack, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackListDownButtons->Add( m_pStackListEditProperties, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL| wxALL, 2  );
	sizerStackListData->Add( sizerStackListDownButtons, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 2  );

	// calculate sizers
	topsizer->Add( pNotebook, 1, wxGROW | wxALL, 10 );
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxCENTRE | wxALL, 10 );

	// :: param
	pPanelParam->SetSizer( sizerParamPage );
	pNotebook->AddPage( pPanelParam, wxT("Main") );
	// :: Stacking
	pPanelStacking->SetSizer( sizerStackingPage );
	pNotebook->AddPage( pPanelStacking, wxT("Stacking") );
	// :: Discovery
	pPanelDiscovery->SetSizer( sizerDiscoveryPage );
	pNotebook->AddPage( pPanelDiscovery, wxT("Discovery") );
	// :: image list
	pPanelImageList->SetSizer( sizerImageListData );
	pNotebook->AddPage( pPanelImageList, wxT("Image List") );
	// :: stack list
	pPanelStackList->SetSizer( sizerStackListData );
	pNotebook->AddPage( pPanelStackList, wxT("Stack List") );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre( wxBOTH );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CGroupProperty
// Purpose:	Delete my object
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CGroupProperty::~CGroupProperty( )
{
	// remove params
	delete( m_pGroupProcType );
	delete( m_pGroupClassType );
	delete( m_pGroupSaveAlignedOver );
	delete( m_pGroupStackFormat );
	delete( m_pGroupStackSize );
	delete( m_pGroupStackTranslate );
	delete( m_pGroupStackDelTemp );
	// remove stacking
	delete( m_pGroupStackMethod );
	delete( m_pStackSigma );
	delete( m_pStackIterations );
	delete( m_pBiasFrameEntry );
	delete( m_pBiasFrameButton );
	delete( m_pDarkFrameEntry );
	delete( m_pDarkFrameButton );
	delete( m_pFlatFrameEntry );
	delete( m_pFlatFrameButton );
	delete( m_pStackFrameBlurMethod );
	// discovery
	delete( m_pObjHistogramCmpMethod );
	delete( m_pObjHistogramNormalize );
	delete( m_pDynamicObjMatchMaxFwhm );
	delete( m_pDynamicObjMinDistMatch );
	delete( m_pObjDynamicsMagTol );
	delete( m_pObjDynamicsKronFactorTol );
	delete( m_pObjDynamicsFwhmTol );
	delete( m_pPathDevAngleTolerance );
	// remove image list
	delete( m_pImageListData );
	delete( m_pImageListButtonUp );
	delete( m_pImageListButtonDown );
	delete( m_pImageListAddImages );
	delete( m_pImageListDeleteImages );
	delete( m_pImageListAddToStack );
	// remove stack list
	delete( m_pStackListData );
	delete( m_pStackListButtonUp );
	delete( m_pStackListButtonDown );
	delete( m_pStackListRemoveFromStack );
	delete( m_pStackListEditProperties );
}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CGroupProperty
// Purpose:	Set and get my reference using group object
// Input:	pointer to group and units fromat to use
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGroupProperty::SetConfig( CImageGroup* pImageGroup, int nUnitsFormat )
{
	wxString strTmp;
	int i =0;
	CAstroImage* pImage = NULL;

	m_pImageGroup = pImageGroup;

	////////////////////////////////////
	// set stacking params
	m_pGroupStackMethod->SetSelection( m_pImageGroup->m_nImageStackMethod );
	m_pBiasFrameEntry->SetValue( wxString(m_pImageGroup->m_strStackBiasFrame,wxConvUTF8) );
	m_pDarkFrameEntry->SetValue( wxString(m_pImageGroup->m_strStackDarkFrame,wxConvUTF8) );
	m_pFlatFrameEntry->SetValue( wxString(m_pImageGroup->m_strStackFlatFrame,wxConvUTF8) );
	// :: sigma
	strTmp.Printf( wxT("%.5f"), m_pImageGroup->m_nStackFrameSigma );
	m_pStackSigma->SetValue( strTmp );
	// :: iterations
	strTmp.Printf( wxT("%d"), m_pImageGroup->m_StackFrameIteration );
	m_pStackIterations->SetValue( strTmp );
	// :: blur method
	m_pStackFrameBlurMethod->SetSelection( m_pImageGroup->m_nStackFrameBlurMethod );

	///////////////////////////////////////
	// set dynamics params
	// :: histogram compare method
	if( m_pImageGroup->m_nHistCmpMethod == CV_COMP_CORREL )
		m_pObjHistogramCmpMethod->SetSelection( 0 );
	else if( m_pImageGroup->m_nHistCmpMethod == CV_COMP_CHISQR )
		m_pObjHistogramCmpMethod->SetSelection( 1 );
	else if( m_pImageGroup->m_nHistCmpMethod == CV_COMP_INTERSECT )
		m_pObjHistogramCmpMethod->SetSelection( 2 );
	else if( m_pImageGroup->m_nHistCmpMethod == CV_COMP_BHATTACHARYYA )
		m_pObjHistogramCmpMethod->SetSelection( 3 );
	// :: histogram normalization
	strTmp.Printf( wxT("%.4f"), m_pImageGroup->m_nObjHistogramNormalize );
	m_pObjHistogramNormalize->SetValue( strTmp );
	// :: ObjMatchMaxFwhm
	strTmp.Printf( wxT("%.6f"), m_pImageGroup->m_nDynamicObjMatchMaxFwhm );
	m_pDynamicObjMatchMaxFwhm->SetValue( strTmp );
	// :: ObjMinDistMatch
	strTmp.Printf( wxT("%.2f"), m_pImageGroup->m_nDynamicObjMinDistMatch );
	m_pDynamicObjMinDistMatch->SetValue( strTmp );
	// :: magnitude tolerance
	strTmp.Printf( wxT("%.6f"), m_pImageGroup->m_nObjDynamicsMagTol );
	m_pObjDynamicsMagTol->SetValue( strTmp );
	// :: kron tolerance
	strTmp.Printf( wxT("%.6f"), m_pImageGroup->m_nObjDynamicsKronFactorTol );
	m_pObjDynamicsKronFactorTol->SetValue( strTmp );
	// :: FWHM tolerance
	strTmp.Printf( wxT("%.6f"), m_pImageGroup->m_nObjDynamicsFwhmTol );
	m_pObjDynamicsFwhmTol->SetValue( strTmp );
	// :: max path deviation angle
	strTmp.Printf( wxT("%.4f"), m_pImageGroup->m_nPathDevAngleTolerance );
	m_pPathDevAngleTolerance->SetValue( strTmp );

	// set image list
	SetImageList( );

	// set stack list
	SetStackList( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	SetImageList
// Class:	CGroupProperty
// Purpose:	Set group image list in the properties dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGroupProperty::SetImageList( )
{
	wxString strTmp;
	int i =0;
	CAstroImage* pImage = NULL;

	/////////////////////
	// set image list
	m_pImageListData->ClearAll();
	// add columns
	m_pImageListData->InsertColumn( 1, wxT("No"), wxLIST_FORMAT_LEFT, 30 );
	m_pImageListData->InsertColumn( 2, wxT("Id"), wxLIST_FORMAT_LEFT, 30 );
	m_pImageListData->InsertColumn( 3, wxT("Name"), wxLIST_FORMAT_LEFT, 150 );
	m_pImageListData->InsertColumn( 4, wxT("Date/Time"), wxLIST_FORMAT_LEFT, 120 );

	for( i=0; i<m_pImageGroup->m_nImageNode; i++ )
	{
		// add row
		long nIndex = m_pImageListData->InsertItem( 55551+i, wxT("") );

		///////////
		// color differently if image in a stack
		if( m_pImageGroup->GetImageStackId( m_pImageGroup->m_vectImageNode[i]->m_nId ) >= 0 )
		{
			m_pImageListData->SetItemBackgroundColour( nIndex, *wxCYAN );
		}

		// set no
		strTmp.Printf( wxT("%d"), i );
		m_pImageListData->SetItem( nIndex, 0, strTmp );

		// set id
		strTmp.Printf( wxT("%d"), m_pImageGroup->m_vectImageNode[i]->m_nId );
		m_pImageListData->SetItem( nIndex, 1, strTmp );

		// set name
		m_pImageListData->SetItem( nIndex, 2, wxString(m_pImageGroup->m_vectImageNode[i]->m_strImageName,wxConvUTF8) );

		// set time	
		struct tm *tmp;
		tmp = localtime( &(m_pImageGroup->m_vectImageNode[i]->m_nFileTime) );
		char str_tmp[255];
		strftime( str_tmp, 255, "%d/%m/%Y %H:%M:%S", tmp );
		m_pImageListData->SetItem( nIndex, 3, wxString(str_tmp,wxConvUTF8) );

	}
}

////////////////////////////////////////////////////////////////////
// Method:	SetStackList
// Class:	CGroupProperty
// Purpose:	Set group stack list in the properties dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGroupProperty::SetStackList( )
{
	wxString strTmp;
	int i =0;
	CAstroImage* pImage = NULL;

	/////////////////////
	// set stack list
	m_pStackListData->ClearAll();
	// add columns
	m_pStackListData->InsertColumn( 1, wxT("No"), wxLIST_FORMAT_LEFT, 30 );
	m_pStackListData->InsertColumn( 2, wxT("Id"), wxLIST_FORMAT_LEFT, 30 );
	m_pStackListData->InsertColumn( 3, wxT("Name"), wxLIST_FORMAT_LEFT, 150 );
	// props
	m_pStackListData->InsertColumn( 4, wxT("x-shift"), wxLIST_FORMAT_LEFT, 60 );
	m_pStackListData->InsertColumn( 5, wxT("y-shift"), wxLIST_FORMAT_LEFT, 60 );
	m_pStackListData->InsertColumn( 6, wxT("scale"), wxLIST_FORMAT_LEFT, 60 );
	m_pStackListData->InsertColumn( 7, wxT("rotation"), wxLIST_FORMAT_LEFT, 60 );
	m_pStackListData->InsertColumn( 8, wxT("Date/Time"), wxLIST_FORMAT_LEFT, 120 );

	// for every image in the stack
	for( i=0; i<m_pImageGroup->m_nStack; i++ )
	{
		//int nImageId = m_pImageGroup->m_vectStack[i].image_id;
		pImage = m_pImageGroup->GetImageByStackId( i ); //m_pImageGroup->m_vectImageNode[nImageId];

		// add row
		long nIndex = m_pStackListData->InsertItem( 55551+i, wxT("") );
//		m_pStackListData->SetItemBackgroundColour( nIndex, *wxCYAN );
		// set no
		strTmp.Printf( wxT("%d"), i );
		m_pStackListData->SetItem( nIndex, 0, strTmp );
		// set id
		strTmp.Printf( wxT("%d"), m_pImageGroup->m_vectStack[i].image_id );
		m_pStackListData->SetItem( nIndex, 1, strTmp );
		// set image name
		m_pStackListData->SetItem( nIndex, 2, wxString(pImage->m_strImageName,wxConvUTF8) );

		///////////////////////////
		// props :: shift x
		strTmp.Printf( wxT("%.5lf"), m_pImageGroup->m_vectStack[i].shift_x );
		m_pStackListData->SetItem( nIndex, 3, strTmp );
		// props :: shift y
		strTmp.Printf( wxT("%.5lf"), m_pImageGroup->m_vectStack[i].shift_y );
		m_pStackListData->SetItem( nIndex, 4, strTmp );
		// props :: scale
		strTmp.Printf( wxT("%.5lf"), m_pImageGroup->m_vectStack[i].scale );
		m_pStackListData->SetItem( nIndex, 5, strTmp );
		// props :: rotation
		strTmp.Printf( wxT("%.5lf"), m_pImageGroup->m_vectStack[i].angle_rad*RAD2DEG );
		m_pStackListData->SetItem( nIndex, 6, strTmp );

		// set time	
		struct tm *tmp;
		tmp = localtime( &(m_pImageGroup->m_vectStack[i].p_image->m_nFileTime) );
		char str_tmp[255];
		strftime( str_tmp, 255, "%d/%m/%Y %H:%M:%S", tmp );
		m_pStackListData->SetItem( nIndex, 7, wxString(str_tmp,wxConvUTF8) );

	}
}

////////////////////////////////////////////////////////////////////
// Method:		OnImgListSelect
// Class:		CGroupProperty
// Purpose:		select/deselect image(s) from list
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGroupProperty::OnImgListSelect( wxListEvent& pEvent )
{
	if( m_pImageListData->GetSelectedItemCount() > 0 )
	{
		m_pImageListButtonUp->Enable();
		m_pImageListButtonDown->Enable();
		// temp enable - todo : should check first if not in stack list
//		m_pImageListAddImages->Enable();
		m_pImageListDeleteImages->Enable();
		m_pImageListAddToStack->Enable();		

	} else
	{
		m_pImageListButtonUp->Disable();
		m_pImageListButtonDown->Disable();
		// temp disable - todo : should check first 
//		m_pImageListAddImages->Disable();
		m_pImageListDeleteImages->Disable();
		m_pImageListAddToStack->Disable();
	}
}

////////////////////////////////////////////////////////////////////
// Method:		OnImgListColumn
// Class:		CGroupProperty
// Purpose:		image list column was clicked
// Input:		pointer to list event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGroupProperty::OnImgListColumn( wxListEvent& pEvent )
{
	int i = 0;
	// get column
	int nColumnId = pEvent.GetColumn();

	////////
	// get selected rows
	vector <long> vectSelectedIds;
    long item = -1;
    for ( ;; )
    {
        item = m_pImageListData->GetNextItem( item, wxLIST_NEXT_ALL,
											wxLIST_STATE_SELECTED );
        if ( item == -1 ) break;

        // push selected to vector 
		vectSelectedIds.push_back( m_pImageGroup->m_vectImageNode[item]->m_nId );
    }

	///////////////
	// Check by column id
	// :: if sort by id
	if( nColumnId == 1 )
	{
		if( m_bSortImgListByIdAsc == 0 )
		{
			m_pImageGroup->SortImagesById( 1 );
			m_bSortImgListByIdAsc = 1;

		} else
		{
			m_pImageGroup->SortImagesById( 0 );
			m_bSortImgListByIdAsc = 0;
		}

	// :: if sort by name
	} else if( nColumnId == 2 )
	{
		if( m_bSortImgListByNameAsc == 0 )
		{
			m_pImageGroup->SortImagesByName( 1 );
			m_bSortImgListByNameAsc = 1;

		} else
		{
			m_pImageGroup->SortImagesByName( 0 );
			m_bSortImgListByNameAsc = 0;
		}

	// :: if sort by time stamp
	} else if( nColumnId == 3 )
	{
		if( m_bSortImgListByFileTimeAsc == 0 )
		{
			m_pImageGroup->SortImagesByFileTime( 1 );
			m_bSortImgListByFileTimeAsc = 1;

		} else
		{
			m_pImageGroup->SortImagesByFileTime( 0 );
			m_bSortImgListByFileTimeAsc = 0;
		}
	}

	// re draw image list
	SetImageList( );

	// set slection
	long nSelected = -1;
	for( i=0; i<vectSelectedIds.size(); i++ )
	{
		// get image no for that old id
		nSelected = m_pImageGroup->GetImageNoById( vectSelectedIds[i] );
		// if none skip
		if( nSelected < 0 ) continue;
		// set selecttion
		if( nSelected >= 0 && nSelected < m_pImageGroup->m_nImageNode )
			m_pImageListData->SetItemState( nSelected, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED  );
	}
	// clear selected list
	vectSelectedIds.clear( );
	// set focus back on
	m_pImageListData->SetFocus( );

	// refresh
	m_pImageListData->Refresh( FALSE );
	m_pImageListData->Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnStackListColumn
// Class:		CGroupProperty
// Purpose:		stack list column was clicked
// Input:		pointer to list event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGroupProperty::OnStackListColumn( wxListEvent& pEvent )
{
	int i = 0;
	// get column
	int nColumnId = pEvent.GetColumn();

	////////
	// get selected rows
	vector <long> vectSelectedIds;
    long item = -1;
    for ( ;; )
    {
        item = m_pStackListData->GetNextItem( item, wxLIST_NEXT_ALL,
											wxLIST_STATE_SELECTED );
        if ( item == -1 ) break;

        // push selected to vector 
		vectSelectedIds.push_back( m_pImageGroup->m_vectImageNode[item]->m_nId );
    }

	///////////////
	// Check by column id
	// :: if sort by id
	if( nColumnId == 1 )
	{
		if( m_bSortStackListByIdAsc == 0 )
		{
			m_pImageGroup->SortStackImagesById( 1 );
			m_bSortStackListByIdAsc = 1;

		} else
		{
			m_pImageGroup->SortStackImagesById( 0 );
			m_bSortStackListByIdAsc = 0;
		}

	// :: if sort by name
	} else if( nColumnId == 2 )
	{
		if( m_bSortStackListByNameAsc == 0 )
		{
			m_pImageGroup->SortStackImagesByName( 1 );
			m_bSortStackListByNameAsc = 1;

		} else
		{
			m_pImageGroup->SortStackImagesByName( 0 );
			m_bSortStackListByNameAsc = 0;
		}

	// :: if sort by file time stamp
	} else if( nColumnId == 7 )
	{
		if( m_bSortStackListByFileTimeAsc == 0 )
		{
			m_pImageGroup->SortStackImagesByFileTime( 1 );
			m_bSortStackListByFileTimeAsc = 1;

		} else
		{
			m_pImageGroup->SortStackImagesByFileTime( 0 );
			m_bSortStackListByFileTimeAsc = 0;
		}
	}

	// re draw image list
	SetStackList( );

	// set slection
	long nSelected = -1;
	for( i=0; i<vectSelectedIds.size(); i++ )
	{
		// get image no for that old id
		nSelected = m_pImageGroup->GetImageNoById( vectSelectedIds[i] );
		// if none skip
		if( nSelected < 0 ) continue;
		// set selecttion
		if( nSelected >= 0 && nSelected < m_pImageGroup->m_nImageNode )
			m_pStackListData->SetItemState( nSelected, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED  );
	}
	// clear selected list
	vectSelectedIds.clear( );
	// set focus back on
	m_pStackListData->SetFocus( );

	// refresh
	m_pStackListData->Refresh( FALSE );
	m_pStackListData->Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnImgListButtonUpDown
// Class:		CGroupProperty
// Purpose:		move image in the image list up or down
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGroupProperty::OnImgListButtonUpDown( wxCommandEvent& pEvent )
{
	// if none selected
	if( m_pImageListData->GetSelectedItemCount() <= 0 ) return;

	int nActionId = pEvent.GetId();

	// search slected items
	int i = 0;
	vector <long> vectItems;
	vector <long> vectSelected;
    long item = -1;
    for ( ;; )
    {
        item = m_pImageListData->GetNextItem( item, wxLIST_NEXT_ALL,
											wxLIST_STATE_SELECTED );
        if ( item == -1 ) break;

        // set to vector 
   	   vectItems.push_back( item );
    }

	// check swap by order 
	if( nActionId == wxID_IMAGE_GROUP_IMGLIST_BUTTON_UP )
	{
		// loop top to bottom
		for( i=0; i<vectItems.size(); i++ )
		{
			m_pImageGroup->SwapImagesPositionById( vectItems[i], vectItems[i]-1 );
			vectSelected.push_back( vectItems[i]-1 );
		}

	} else if( nActionId == wxID_IMAGE_GROUP_IMGLIST_BUTTON_DOWN )
	{
		// loop bottom to top
		for( i=vectItems.size()-1; i>=0; i-- )
		{
			m_pImageGroup->SwapImagesPositionById( vectItems[i], vectItems[i]+1 );
			vectSelected.push_back( vectItems[i]+1 );
		}
	}

	// re draw image list
	SetImageList( );
	// set slection
	long nSelected = -1;
	for( i=0; i<vectSelected.size(); i++ )
	{
		nSelected = vectSelected[i];
		if( nSelected >= 0 && nSelected < m_pImageGroup->m_nImageNode )
			m_pImageListData->SetItemState( nSelected, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED  );
	}
	// set focus back on
	m_pImageListData->SetFocus();

	vectItems.clear( );
	vectSelected.clear( );
	// refresh
	m_pImageListData->Refresh( FALSE );
	m_pImageListData->Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnImgListAddImages
// Class:		CGroupProperty
// Purpose:		add image(s) to the list
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGroupProperty::OnImgListAddImages( wxCommandEvent& pEvent )
{
	wxString strTmp;

	CConfigMain* m_pMainConfig = m_pImageGroup->m_pImageDb->m_pFrame->m_pMainConfig;

	strTmp.Printf( wxT("Add Image Files To Group :: %s"), m_pImageGroup->m_strGroupNameId );
	CGroupSetDialog *pGroupDlg = new CGroupSetDialog( this, strTmp, m_pImageGroup->m_strGroupPath, 1 );
	// and set the config
//	pGroupDlg->SetConfig( m_pImageGroup->m_pm_pMainConfig, m_pImageDb );
	
	if( pGroupDlg->ShowModal() == wxID_OK )
	{
		// get group data
		wxString strGroupName = wxT(""); 
		if( pGroupDlg->m_nActionType == 1 )
			strGroupName = pGroupDlg->m_pGroupNameEntry->GetLineText(0);
		else
			strGroupName = pGroupDlg->m_pSelectGroup->GetStringSelection();
		// get files
		wxString strGroupFiles = pGroupDlg->m_pGroupPathEntry->GetLineText(0);
		// trim strings
		strGroupName.Trim( 0 ).Trim( 1 );
		strGroupFiles.Trim( 0 ).Trim( 1 );
		// check if empty
		if( !strGroupFiles.IsEmpty() )
		{
			// to add the files to my image data base
			int nFirstImage = -1;
			int nNoOfImagesAdded = m_pImageGroup->AddImages( strGroupFiles, nFirstImage );
			//m_pImageGroup->m_pImageDb->AddImages( strGroupName.GetData(), strGroupFiles.GetData() );

			// load current selected image - the last image will be displayed
//			LoadSelectedImage( );
			// and also save groups / images
//			m_pImageDb->SaveGroups( );
//			m_pImageDb->SaveAllGroups( );
			// and set last path used
			m_pImageGroup->m_strGroupPath = pGroupDlg->m_strPath;
			strTmp.Printf( wxT("%s"), m_pImageGroup->m_strGroupPath );

			// todo: chage SetStrVar( to wxString& 
			//char str_tmp[255];
			//strcpy( str_tmp, strTmp.ToAscii() );
			m_pMainConfig->SetStrVar( SECTION_ID_GUI, CONF_ADD_FILE_PATH, strTmp );

	//		// add my group to the list
	//		AddGroupToSelect( strGroupName.GetData() );
		}
	}

	delete( pGroupDlg );

	// re draw stack/image list
	SetImageList( );

	// refresh
	m_pImageListData->Refresh( FALSE );
	m_pImageListData->Update( );

}

////////////////////////////////////////////////////////////////////
// Method:		OnImgListDeleteImages
// Class:		CGroupProperty
// Purpose:		delete image(s) from the list
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGroupProperty::OnImgListDeleteImages( wxCommandEvent& pEvent )
{
	// if none selected
	if( m_pImageListData->GetSelectedItemCount() <= 0 ) return;

	// search slected items
	int i = 0;
	vector <long> vectItems;
    long item = -1;
    for ( ;; )
    {
        item = m_pImageListData->GetNextItem( item, wxLIST_NEXT_ALL,
											wxLIST_STATE_SELECTED );
        if ( item == -1 ) break;

        // set to vector 
   	   vectItems.push_back( item );
    }

	// loop bottom to top
	for( i=vectItems.size()-1; i>=0; i-- )
	{
		m_pImageGroup->RemoveImageByPosId( vectItems[i] );
	}

	// clean used
	vectItems.clear( );

	// re draw stack/image list
	SetStackList( );
	SetImageList( );

	// refresh
	m_pStackListData->Refresh( FALSE );
	m_pStackListData->Update( );
	m_pImageListData->Refresh( FALSE );
	m_pImageListData->Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnImgListAddToStack
// Class:		CGroupProperty
// Purpose:		add image from list to the stack
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGroupProperty::OnImgListAddToStack( wxCommandEvent& pEvent )
{
    long item = -1;
    for ( ;; )
    {
        item = m_pImageListData->GetNextItem( item, wxLIST_NEXT_ALL,
											wxLIST_STATE_SELECTED );
        if ( item == -1 ) break;

		// check if image in stack already
		if( m_pImageGroup->IsImageInStack( m_pImageGroup->m_vectImageNode[item]->m_nId ) ) continue;

		// init stack if zero
		int nStackId = m_pImageGroup->m_nStack;
		//if( nStackId <= 0 ) 
		m_pImageGroup->m_vectStack.resize( m_pImageGroup->m_vectStack.size() + 1 );
		// push into the stack vector
		m_pImageGroup->m_vectStack[nStackId].image_id = m_pImageGroup->m_vectImageNode[item]->m_nId;
		m_pImageGroup->m_vectStack[nStackId].p_image = m_pImageGroup->m_vectImageNode[item];
		// set the others to zero
		m_pImageGroup->m_vectStack[nStackId].angle_deg = 0;
		m_pImageGroup->m_vectStack[nStackId].angle_rad = 0;
		m_pImageGroup->m_vectStack[nStackId].exim_time = 0;
		m_pImageGroup->m_vectStack[nStackId].file_time = 0;
		m_pImageGroup->m_vectStack[nStackId].register_score = 0;
		m_pImageGroup->m_vectStack[nStackId].scale = 0;
		m_pImageGroup->m_vectStack[nStackId].shift_x = 0;
		m_pImageGroup->m_vectStack[nStackId].shift_y = 0;
	
		m_pImageGroup->m_nStack++;
	}

	// set stack and image list again
	SetImageList( );
	SetStackList( );

	// refresh
	m_pImageListData->Refresh( FALSE );
	m_pImageListData->Update( );
	m_pStackListData->Refresh( FALSE );
	m_pStackListData->Update( );

}

////////////////////////////////////////////////////////////////////
// Method:		OnStackListSelect
// Class:		CGroupProperty
// Purpose:		select/deselect image(s) from stack list
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGroupProperty::OnStackListSelect( wxListEvent& pEvent )
{
	if( m_pStackListData->GetSelectedItemCount() > 0 )
	{
		m_pStackListButtonUp->Enable();
		m_pStackListButtonDown->Enable();
		// temp enable - todo : should check first if not in stack list
		m_pStackListRemoveFromStack->Enable();
//		m_pStackListEditProperties->Enable();
		

	} else
	{
		m_pStackListButtonUp->Disable();
		m_pStackListButtonDown->Disable();
		// temp disable - todo : should check first 
		m_pStackListRemoveFromStack->Disable();
//		m_pStackListEditProperties->Disable();
	}
}

////////////////////////////////////////////////////////////////////
// Method:		OnStackListButtonUpDown
// Class:		CGroupProperty
// Purpose:		move image in the stack list up or down
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGroupProperty::OnStackListButtonUpDown( wxCommandEvent& pEvent )
{
	// if none selected
	if( m_pStackListData->GetSelectedItemCount() <= 0 ) return;

	int nActionId = pEvent.GetId();

	// search slected items
	int i = 0;
	vector <long> vectItems;
	vector <long> vectSelected;
    long item = -1;
    for ( ;; )
    {
        item = m_pStackListData->GetNextItem( item, wxLIST_NEXT_ALL,
											wxLIST_STATE_SELECTED );
        if ( item == -1 ) break;

        // set to vector 
   	   vectItems.push_back( item );
    }

	// check swap by order 
	if( nActionId == wxID_IMAGE_GROUP_STACKLIST_BUTTON_UP )
	{
		// loop top to bottom
		for( i=0; i<vectItems.size(); i++ )
		{
			m_pImageGroup->SwapStakImagesPositionById( vectItems[i], vectItems[i]-1 );
			vectSelected.push_back( vectItems[i]-1 );
		}

	} else if( nActionId == wxID_IMAGE_GROUP_STACKLIST_BUTTON_DOWN )
	{
		// loop top to bottom
		for( i=vectItems.size()-1; i>=0; i-- )
		{
			m_pImageGroup->SwapStakImagesPositionById( vectItems[i], vectItems[i]+1 );
			vectSelected.push_back( vectItems[i]+1 );
		}
	}

	// re draw image list
	SetStackList( );
	// set slection
	long nSelected = -1;
	for( i=0; i<vectSelected.size(); i++ )
	{
		nSelected = vectSelected[i];
		if( nSelected >= 0 && nSelected < m_pImageGroup->m_vectStack.size() )
			m_pStackListData->SetItemState( nSelected, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED  );
	}
	// set focus back on
	m_pStackListData->SetFocus();

	vectItems.clear( );
	vectSelected.clear( );
	// refresh
	m_pStackListData->Refresh( FALSE );
	m_pStackListData->Update( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnStackListRemoveImage
// Class:		CGroupProperty
// Purpose:		remove image(s) from stack list
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGroupProperty::OnStackListRemoveImage( wxCommandEvent& pEvent )
{
	// if none selected
	if( m_pStackListData->GetSelectedItemCount() <= 0 ) return;

    long item = -1;
    for ( ;; )
    {
        item = m_pStackListData->GetNextItem( item, wxLIST_NEXT_ALL,
											wxLIST_STATE_SELECTED );
        if ( item == -1 ) break;

		// remove from stack list
		m_pImageGroup->m_vectStack.erase( m_pImageGroup->m_vectStack.begin( ) + item );
		m_pImageGroup->m_nStack--;
	}

	// re draw stack/image list
	SetStackList( );
	SetImageList( );

	// refresh
	m_pStackListData->Refresh( FALSE );
	m_pStackListData->Update( );
	m_pImageListData->Refresh( FALSE );
	m_pImageListData->Update( );

}

////////////////////////////////////////////////////////////////////
// Method:		OnSelectImageFile
// Class:		CGroupProperty
// Purpose:		when selected stack image files
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGroupProperty::OnSelectImageFile( wxCommandEvent& pEvent )
{
	int nSelectType = pEvent.GetId( );
	wxTextCtrl* pEntry = NULL;
	wxString strTitle = wxT("Select Image");

	// check by type
	if( nSelectType == wxID_BUTTON_STACK_BIAS )
	{
		pEntry = m_pBiasFrameEntry;
		strTitle = wxT("Select Bias Frame");

	} else if( nSelectType == wxID_BUTTON_STACK_DARK )
	{
		pEntry = m_pDarkFrameEntry;
		strTitle = wxT("Select Dark Frame");

	} else if( nSelectType == wxID_BUTTON_STACK_FLAT )
	{
		pEntry = m_pFlatFrameEntry;
		strTitle = wxT("Select Flat Frame");
	}

	wxFileDialog fileDlg( this, strTitle,
							wxString(m_pImageGroup->m_vectStack[0].p_image->m_strImagePath,wxConvUTF8),
							wxT(""), wxT(DEFAULT_LOAD_IMAGE_FILE_TYPE), wxFD_DEFAULT_STYLE );

	if( fileDlg.ShowModal() == wxID_OK )
	{
		wxString strFilename = wxT("");
		strFilename = fileDlg.GetPath( );
		// set value
		pEntry->SetValue( strFilename );
	}

	return;
}
