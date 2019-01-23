
#ifndef _IMAGE_SAVE_DLG_H
#define _IMAGE_SAVE_DLG_H

#include "wx/wxprec.h"
#include <wx/listctrl.h>
#include <wx/spinctrl.h>

enum
{
	wxID_IMAGE_SAVE_FILE_BUTTON		= 9600,
	wxID_IMAGE_SAVE_PATH_BUTTON,
	wxID_IMAGE_SAVE_SIZE_WIDTH,
	wxID_IMAGE_SAVE_SIZE_HEIGHT,
	wxID_IMAGE_SAVE_VIEW_TYPE
};

// external classes
class CAstroImage;

// class:	CImageSaveDlg
///////////////////////////////////////////////////////
class CImageSaveDlg : public wxDialog
{
// methods
public:
    CImageSaveDlg( wxWindow *parent, const wxString& title, CAstroImage* pImage );
	~CImageSaveDlg( );

/////////////
// ::DATA
public:
	wxButton*		m_pFileSetButton;
	wxTextCtrl*		m_pFileSetEntry;
	wxButton*		m_pPathSetButton;
	wxTextCtrl*		m_pPathSetEntry;
	wxSpinCtrl*		m_pSizeWidth;
	wxSpinCtrl*		m_pSizeHeight;
	wxCheckBox*		m_pUseSizeConstr;
	wxCheckBox*		m_pUseTags;

	wxString		m_strPath;
	wxString		m_strFile;
	CAstroImage*	m_pAstroImage;
	double			m_ImgRatio;

	// flags
	int m_bOnWidthSetValue;
	int m_bOnHeightSetValue;

private:
	void OnFileSet( wxCommandEvent& pEvent );
	void OnPathSet( wxCommandEvent& pEvent );
	void OnSizeChangeS( wxSpinEvent& pEvent );
	void OnSizeChangeT( wxCommandEvent& pEvent );
	void SetSizeChange( int nId, int nValue );

	DECLARE_EVENT_TABLE()
};

#endif
