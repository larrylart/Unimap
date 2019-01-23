
#ifndef _FILE_EXPORT_DLG_H
#define _FILE_EXPORT_DLG_H

// wx
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// defines
#define FILE_EXPORT_FORMATS wxT( "Html Files (*.html;*.html)|*.html;*.html|HTML (*.html)|*.htm" )

// gui ids
enum
{
	wxID_EXPORT_FILE_SET_BUTTON = 3500,
};

// external classes
class CAstroImage;
class CImageGroup;

// class:	CFileExportDlg
///////////////////////////////////////////////////////
class CFileExportDlg : public wxDialog
{
// public methods
public:
    CFileExportDlg( wxWindow *parent, wxString title, CImageGroup* pImageGroup, CAstroImage* pAstroImage );
	virtual ~CFileExportDlg( );

// public data
public:
	wxChoice*	m_pFormat;
	wxChoice*	m_pTarget;
	wxButton*	m_pFileSetButton;
	wxTextCtrl* m_pFile;

	// path/file
	wxString m_strPath;
	wxString m_strFile;
	wxString m_strFullFile;

	// external objects
	CAstroImage*	m_pAstroImage;
	CImageGroup*	m_pImageGroup;

private:
	void OnFileSet( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif
