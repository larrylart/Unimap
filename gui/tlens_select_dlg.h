
#ifndef _TLENS_SELECT_DLG_H
#define _TLENS_SELECT_DLG_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// gui enum
enum
{
	wxID_TLENS_SELECT_SOURCE = 19100,
	// main lens
	wxID_MAIN_LENS_SELECT_TYPE,
	wxID_MAIN_LENS_SELECT_OPTICS,
	wxID_MAIN_LENS_SELECT_BRAND,
	wxID_MAIN_LENS_SELECT_NAME,
	// barlow lens
	wxID_CHECK_USE_BARLOW_LENS,
	wxID_BARLOW_LENS_SELECT_BRAND,
	wxID_BARLOW_LENS_SELECT_NAME,
	// focal reducer
	wxID_CHECK_USE_FOCAL_REDUCER,
	wxID_FOCAL_REDUCER_SELECT_BRAND,
	wxID_FOCAL_REDUCER_SELECT_NAME,
	// eyepiece
	wxID_CHECK_USE_EYEPIECE,
	wxID_EYEPIECE_SELECT_TYPE,
	wxID_EYEPIECE_SELECT_BRAND,
	wxID_EYEPIECE_SELECT_NAME,
	// camera lens
	wxID_CHECK_USE_CAMERA_LENS,
	wxID_CAMERA_LENS_SELECT_TYPE,
	wxID_CAMERA_LENS_SELECT_BRAND,
	wxID_CAMERA_LENS_SELECT_NAME,
};

// external classes
class CCamera;
class CTelescope;
class CObserver;

// local include
#include "../observer/observer.h"

// class:	CTLensSelectDlg
///////////////////////////////////////////////////////
class CTLensSelectDlg : public wxDialog
{
// public methods
public:
    CTLensSelectDlg( wxWindow *parent, wxString title, int nType=0 );
	virtual ~CTLensSelectDlg( );

	void SetData( CObserver* pObserver, CCamera* pCamera, CTelescope* pTelescope, int nSourceId, DefTLensSetup& rLensSetup ); 

	// other handles
	void SetSetupData( );
	void GetAllFields( );
	void SetFocalValues( );
	double CalculateFocalLength( DefTLensSetup& rLensSetup );

	// geat lens real ids
	int GetLensBrandId();
	int GetLensNameId();
	// get barlow lens real ids
	int GetBarlowLensBrandId(){ if( m_vectBarlowLensBrandId.size() ) return( m_vectBarlowLensBrandId.at(m_pBarlowLensBrandSelect->GetSelection()) ); else return(0); }
	int GetBarlowLensNameId(){ if( m_vectBarlowLensNameId.size() ) return( m_vectBarlowLensNameId.at(m_pBarlowLensNameSelect->GetSelection()) ); else return(0); }
	// get focal reducer real ids
	int GetFocalReducerBrandId(){ if( m_vectFocalReducerBrandId.size() ) return( m_vectFocalReducerBrandId.at(m_pFocalReducerBrandSelect->GetSelection()) ); else return(0); }
	int GetFocalReducerNameId(){ if( m_vectFocalReducerNameId.size() ) return( m_vectFocalReducerNameId.at(m_pFocalReducerNameSelect->GetSelection()) ); else return(0); }
	// geat eyepiece real ids
	int GetEyepieceBrandId();
	int GetEyepieceNameId();
	// geat eyepiece real ids
	int GetCameraLensBrandId();
	int GetCameraLensNameId();

// public data
public:
	CObserver*	m_pObserver;
	CCamera*	m_pCamera;
	CTelescope*	m_pTelescope;

	int m_nDlgType;
	// :: select options
	wxChoice*	m_pSourceSelect;

	// output data
	wxTextCtrl* m_pFocalVal;
	wxTextCtrl* m_pFocalRatioVal;

	// main lens options
	wxStaticText*	m_pLensLabel;
	wxChoice*	m_pLensTypeSelect;
	wxChoice*	m_pLensOpticsSelect;
	wxChoice*	m_pLensBrandSelect;
	wxChoice*	m_pLensNameSelect;

	// barlow lens options
	wxCheckBox*	m_pBarlowLensLabel;
	wxChoice*	m_pBarlowLensBrandSelect;
	wxChoice*	m_pBarlowLensNameSelect;

	// Focal Reducer options
	wxCheckBox*	m_pFocalReducerLabel;
	wxChoice*	m_pFocalReducerBrandSelect;
	wxChoice*	m_pFocalReducerNameSelect;

	// eyepiece options
	wxCheckBox*	m_pEyepieceLabel;
	wxChoice*	m_pEyepieceTypeSelect;
	wxChoice*	m_pEyepieceBrandSelect;
	wxChoice*	m_pEyepieceNameSelect;

	// camera lens options
	wxCheckBox*	m_pCameraLensLabel;
	wxChoice*	m_pCameraLensTypeSelect;
	wxChoice*	m_pCameraLensBrandSelect;
	wxChoice*	m_pCameraLensNameSelect;

	// :: extra types
	int m_nSourceId;
	// lens setup
	DefTLensSetup m_rLensSetup;

	// vectors to keep the real ids
	// :: main lens
	std::vector<int> m_vectLensBrandId;
	std::vector<int> m_vectLensNameId;
	// :: barlow lens
	std::vector<int> m_vectBarlowLensBrandId;
	std::vector<int> m_vectBarlowLensNameId;
	// :: focal reducer
	std::vector<int> m_vectFocalReducerBrandId;
	std::vector<int> m_vectFocalReducerNameId;
	// :: eyepiece
	std::vector<int> m_vectEyepieceBrandId;
	std::vector<int> m_vectEyepieceNameId;
	// :: camera lens
	std::vector<int> m_vectCameraLensBrandId;
	std::vector<int> m_vectCameraLensNameId;


// private methods
private:
	void OnSourceType( wxCommandEvent& pEvent );
	void OnUseSystemPart( wxCommandEvent& pEvent );
	void OnSelectMainLens( wxCommandEvent& pEvent );
	void OnSelectOtherLens( wxCommandEvent& pEvent );
	// visibility handlers
	void SetBalowLensState( int nType, bool bValue );
	void SetFocalReducerState( int nType, bool bValue );
	void SetEyepieceState( int nType, bool bValue );
	void SetCameraLensState( int nType, bool bValue );

	DECLARE_EVENT_TABLE()
};

#endif
