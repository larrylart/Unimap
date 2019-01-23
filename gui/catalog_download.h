
#ifndef _CATALOG_CONFIG_DLG_H
#define _CATALOG_CONFIG_DLG_H

#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// dialog gui enum
enum
{
	wxID_CATALOG_REMOTE_LOCATIONS		= 11500,
};

// class:	CCatalogDownloadDlg
///////////////////////////////////////////////////////
class CCatalogDownloadDlg : public wxDialog
{
// methods
public:
    CCatalogDownloadDlg( wxWindow *parent, CatalogDef* pCatalog, const wxString& title );
	~CCatalogDownloadDlg();

	int GetSourceId( );

//////////////////////////
// DATA
public:
	wxStaticText*	m_pCatalogName;
	wxChoice*		m_pCatalogRemoteLocation;
	wxChoice*		m_pCatlogLocalFormat;

	int m_vectMapSourceId[10];
	int m_nMapSourceId;
};

#endif

