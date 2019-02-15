////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _NEWS_DLG_H
#define _NEWS_DLG_H

// wx
#include "wx/wxprec.h"
#include <wx/minifram.h>

// gui ids
enum
{
	wxID_NEWS_SOURCE_SELECT = 26200,
	wxID_NEWS_TABLE_LIST,
	wxID_NEWS_SOURCE_FETCH,
	wxID_NEWS_SHOW_DETAILS,
	wxID_NEWS_VIEW_ARTICLE
};

// external classes

// class:	CNewsDlg
///////////////////////////////////////////////////////
class CNewsDlg : public wxMiniFrame
{
// methods
public:
    CNewsDlg( CGUIFrame* m_pFrame, const wxString& title );
	~CNewsDlg( );

	void SetConfig( CNews* pNews );

/////////////
// ::DATA
public:

	wxChoice*	m_pSourceSelect;
	wxListCtrl*	m_pNewsTable;

	CNews* m_pNews;

	void MakeNewsTable( int nSource );

private:
	// toolbar
	void OnNewsSourceSelect( wxCommandEvent& pEvent );
	void OnViewArticle( wxListEvent& pEvent );
	void OnViewArticleB( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

#endif

