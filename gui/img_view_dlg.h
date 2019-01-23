

#ifndef _SUN_VIEW_DIALOG_H
#define _SUN_VIEW_DIALOG_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// external class
class CDynamicImgView;

// class:	CImageViewDlg
///////////////////////////////////////////////////////
class CImageViewDlg : public wxDialog
{
public:
	CImageViewDlg( wxWindow *parent, const wxString& strTitle );
	~CImageViewDlg( );

	void SetImageFromFile( const wxString& strImgFile );

// public data
public:
	CDynamicImgView* m_pImgView;

protected:
	DECLARE_EVENT_TABLE()

};

#endif
