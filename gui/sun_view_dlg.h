

#ifndef _SUN_VIEW_DIALOG_H
#define _SUN_VIEW_DIALOG_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

enum
{
	wxID_SUN_VIEW_IMAGE_SOURCE = 21900,
	wxID_SUN_VIEW_IMAGE_BAND
};

// external class
class CConfigMain;
class CDynamicImgView;
class CUnimapWorker;

// class:	CSunViewDlg
///////////////////////////////////////////////////////
class CSunViewDlg : public wxDialog
{
public:
	CSunViewDlg( wxWindow *parent, const wxString& strTitle, CUnimapWorker* pUnimapWorker );
	~CSunViewDlg( );

	void SetOptions( );
	void SetConfig( CConfigMain* pMainConfig );
	int FetchRemoteImage( );
	void SetRemoteImage( );
	int FetchRemoteImageDlg( );

// public data
public:
	CUnimapWorker* m_pUnimapWorker;

	CDynamicImgView* m_pImgView;

	wxStaticText*	m_pImgDateTime;
//	wxChoice*		m_pSunImgSource;
	wxChoice*		m_pObsBandType;

	int m_nSourceType;
	int m_bIsRemoteImage;
	wxString m_strImgFile;
	double m_nImgDateTime;

protected:
	//void OnSelectSunImgSource( wxCommandEvent& pEvent );
	void OnSelectObsBandType( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()

};

#endif
