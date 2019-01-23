

#ifndef __IMAGE_PROCESS_BASE_DLG_H
#define __IMAGE_PROCESS_BASE_DLG_H

// wx includes
#include "wx/wxprec.h"
// ELX
#include <elx/image/ImageVariant.h>
#include <elx/image/ImageImpl.h>
#include <elx/image/Pixels.h>

// local defines
#define IMAGE_PROCESS_MAX_SLIDERS	10
#define IMAGE_PROCESS_MAX_CHOICES	10

using namespace eLynx;
using namespace eLynx::Image;

// external classes
class CDynamicImgView;
class CAstroImgView;
class CImageProcBaseDlg;

// class:	CDynamicImgPreview
////////////////////////////////////////////////////////////////////
class CDynamicImgPreview : public CDynamicImgView
{
public:
	CDynamicImgPreview( CImageProcBaseDlg *pImgPreviewDlg, const wxPoint& pos, const wxSize& size );
	void UpdateOnImageCut( );

	CImageProcBaseDlg* m_pImgPreviewDlg;
};

// class:	CImageProcBaseDlg
////////////////////////////////////////////////////////////////////
class CImageProcBaseDlg : public wxDialog
{
public:
	CImageProcBaseDlg( wxWindow *parent, const wxString& strTitle, int nViewType );
	~CImageProcBaseDlg( );

	void AddSlider( const char* strName, int min, int max, double f_min, double f_max, double f_current, int bRev=0 );
	void AddSelector( const char* strLabel, int nChoices, const wxString choices[], const int values[] );
	// set image local - for now in ImageVariant format
	void SetImage( wxImage& pImage, CAstroImgView* pImgView );
	void DoProcess( );
	// process image
	virtual void Process( ImageVariant& rElxImage ){ return; };
	// process full imase
	void ProcessImage( wxImage& pImage );
	// update image
	void UpdateImage( );

// data
public:
	int m_nType;
	int m_nViewType;
	int m_bHasChanged;

	CDynamicImgPreview* m_pImagePreview;

	wxFlexGridSizer* m_sizerSliders;
	wxFlexGridSizer* m_sizerChoices;

	wxSlider*	m_vectSlider[IMAGE_PROCESS_MAX_SLIDERS];
	int m_nSlider;

	wxChoice*	m_vectChoice[IMAGE_PROCESS_MAX_CHOICES];
	int m_nChoice;
	int				m_vectChoiceCount[IMAGE_PROCESS_MAX_CHOICES];
	int*			m_vectChoiceValue[IMAGE_PROCESS_MAX_CHOICES];
	int				m_vectChoiceSelected[IMAGE_PROCESS_MAX_CHOICES];

	// ready proc data
	int m_vectSliderStartVisual[IMAGE_PROCESS_MAX_SLIDERS];
	int m_vectSliderEndVisual[IMAGE_PROCESS_MAX_SLIDERS];
	int m_vectSliderCurrentVisual[IMAGE_PROCESS_MAX_SLIDERS];
	int m_vectSliderDataOrder[IMAGE_PROCESS_MAX_SLIDERS];
	// real data
	double m_vectSliderStartReal[IMAGE_PROCESS_MAX_SLIDERS];
	double m_vectSliderEndReal[IMAGE_PROCESS_MAX_SLIDERS];
	double m_vectSliderCurrentReal[IMAGE_PROCESS_MAX_SLIDERS];
	// ratio
	double m_vectSlideVisualToRealRatio[IMAGE_PROCESS_MAX_SLIDERS];

	ImageVariant m_pElxImage;

	CAstroImgView* m_pImgView;

	// protected methods
protected:
	void OnSlideChange( wxScrollEvent& pEvent );
	void OnChoiceChange( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif


