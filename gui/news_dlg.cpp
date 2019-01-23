
// system headers
#include <stdio.h>

// wx headers
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/datetime.h>

// local headers
#include "../util/func.h"
#include "../unimap_worker.h"
#include "frame.h"
#include "../logger/logger.h"
#include "../news/news.h"

// main header
#include "news_dlg.h"

//		***** CLASS CNewsDlg *****
////////////////////////////////////////////////////////////////////
// CNewsDlg EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CNewsDlg, wxMiniFrame)
	/////////////////////////
	// :: TOOLBAR :: events
	EVT_CHOICE( wxID_NEWS_SOURCE_SELECT, CNewsDlg::OnNewsSourceSelect )
	EVT_TOOL( wxID_NEWS_VIEW_ARTICLE, CNewsDlg::OnViewArticleB )
	EVT_LIST_ITEM_ACTIVATED( wxID_NEWS_TABLE_LIST, CNewsDlg::OnViewArticle )
END_EVENT_TABLE()


// Constructor/Destructor
////////////////////////////////////////////////////////////////////
CNewsDlg::CNewsDlg( CGUIFrame* pFrame, const wxString& title ) : wxMiniFrame( pFrame, -1, title, wxPoint( -1, -1 ), 
											wxSize( 600, 250 ), wxSYSTEM_MENU|wxCAPTION|wxMAXIMIZE_BOX|wxCLOSE_BOX|wxRESIZE_BORDER )
{
	// default bg 
	SetBackgroundColour( wxNullColour );

	///////////////
	// tool bar
	wxToolBar* pToolBar = CreateToolBar( wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER );
	pToolBar->SetMargins( 3, 3 );
	pToolBar->SetToolPacking( 3 );

	// create source select
	m_pSourceSelect = new wxChoice( pToolBar, wxID_NEWS_SOURCE_SELECT, wxDefaultPosition, wxSize(100,-1), 0, 
										NULL, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pSourceSelect->SetSelection(0);

	// buttons
	wxImage imgDownload( wxT("resources/download.png") );
	wxImage imgDetails( wxT("resources/icon_details.gif") );
	wxImage imgViewPage( wxT("resources/view_page.gif") );

	// get toolbar size
    int wt = pToolBar->GetToolBitmapSize().GetWidth(),
        ht = pToolBar->GetToolBitmapSize().GetHeight();

	// resize icons to toolbar size
	wxBitmap bmpDownload = wxBitmap( imgDownload.Scale(wt, ht) );
	wxBitmap bmpDetails = wxBitmap( imgDetails.Scale(wt, ht) );
	wxBitmap bmpViewPage = wxBitmap( imgViewPage.Scale(wt, ht) );

	// toolbar layout
	pToolBar->AddControl( new wxStaticText( pToolBar, -1, _T("Source: ")) );
	pToolBar->AddControl( m_pSourceSelect );
	pToolBar->AddTool( wxID_NEWS_SOURCE_FETCH, wxT("Fetch"), bmpDownload, wxT("Fetch News Headlines"), wxITEM_CHECK );
	pToolBar->AddSeparator( );
	pToolBar->AddTool( wxID_NEWS_SHOW_DETAILS, wxT("Details"), bmpDetails, wxT("Show Details"), wxITEM_CHECK );
	pToolBar->AddTool( wxID_NEWS_VIEW_ARTICLE, wxT("View Page"), bmpViewPage, wxT("View Full Article"), wxITEM_CHECK );

	////////////////
	// TOP SIZER
	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	// create Notes panel elements
	m_pNewsTable = new wxListCtrl( this, wxID_NEWS_TABLE_LIST,
			wxPoint(-1,-1), wxSize(600,250), wxLC_REPORT|wxLC_HRULES|wxLC_VRULES|wxLC_SINGLE_SEL );

	topsizer->Add( m_pNewsTable, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND| wxALL, 2  );

	// bring out the tool
	pToolBar->Realize();

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	topsizer->Fit(this);
//	Layout();
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
CNewsDlg::~CNewsDlg( )
{
	delete( m_pSourceSelect );
	delete( m_pNewsTable );

	delete( m_pNews );
}

////////////////////////////////////////////////////////////////////
void CNewsDlg::SetConfig( CNews* pNews )
{
	int i=0;

	m_pNews = pNews;

	// populate sources
	m_pSourceSelect->Clear();
	for( i=0; i<4; i++ ) m_pSourceSelect->Append( m_pNews->m_vectNewsSources[i] );
	m_pSourceSelect->SetSelection(0);
	
	// make the news tables
	MakeNewsTable( 0 );

}

////////////////////////////////////////////////////////////////////
void CNewsDlg::MakeNewsTable( int nSource )
{
	if( !m_pNews ) return;

	int i=0;
	wxDateTime news_time;
	int nSrcId=0;

	// populate
	m_pNewsTable->ClearAll();
	m_pNewsTable->InsertColumn( 1, wxT("Date/Time"), wxLIST_FORMAT_LEFT, 120 );
	m_pNewsTable->InsertColumn( 1, wxT("Source"), wxLIST_FORMAT_LEFT, 50 );
	m_pNewsTable->InsertColumn( 2, wxT("Title"), wxLIST_FORMAT_LEFT, 400 );

	for( i=0; i<m_pNews->m_vectHeadlines.size(); i++ )
	{
		// get source
		nSrcId = m_pNews->m_vectHeadlines[i].source;

		if( nSource == 0 || nSource == nSrcId )
		{
			// add row
			long nIndex = m_pNewsTable->InsertItem( 55551+i, wxT("") );
			// :: set date/time
			news_time.Set( m_pNews->m_vectHeadlines[i].datetime );
			m_pNewsTable->SetItem( nIndex, 0, news_time.Format( wxT("%H:%M:%S %d/%m/%Y") ) );
			// set source
			m_pNewsTable->SetItem( nIndex, 1, m_pNews->m_vectNewsSources[nSrcId] );
			// set title
			m_pNewsTable->SetItem( nIndex, 2, m_pNews->m_vectHeadlines[i].title );
		}
	}

//	delete( pNews );
}

////////////////////////////////////////////////////////////////////
void CNewsDlg::OnNewsSourceSelect( wxCommandEvent& pEvent )
{
	int nSelect = m_pSourceSelect->GetSelection();
	MakeNewsTable( nSelect );
}

////////////////////////////////////////////////////////////////////
void CNewsDlg::OnViewArticle( wxListEvent& pEvent )
{
	int nId = pEvent.GetIndex();
	wxLaunchDefaultBrowser( m_pNews->m_vectHeadlines[nId].link, wxBROWSER_NEW_WINDOW );
}

////////////////////////////////////////////////////////////////////
void CNewsDlg::OnViewArticleB( wxCommandEvent& pEvent )
{
	long item=-1;
	// get first selected item
    item = m_pNewsTable->GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );

	if( item >= 0 )
		wxLaunchDefaultBrowser( m_pNews->m_vectHeadlines[item].link, wxBROWSER_NEW_WINDOW );

	GetToolBar()->ToggleTool( wxID_NEWS_VIEW_ARTICLE, 0 );
}
