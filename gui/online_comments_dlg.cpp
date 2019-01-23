
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/wxprec.h"

// local headers
#include "../online/unimap_online.h"

// main header
#include "online_comments_dlg.h"

BEGIN_EVENT_TABLE(COnlineCommnetsDlg, wxDialog)
	EVT_BUTTON( wxID_ONLINE_COMMENT_BUTTON, COnlineCommnetsDlg::OnNewView )
	EVT_CHOICE( wxID_ONLINE_COMMENT_SELECT, COnlineCommnetsDlg::OnSelectComment )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	COnlineCommnetsDlg
// Purpose:	Initialize object
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
COnlineCommnetsDlg::COnlineCommnetsDlg( wxWindow *parent, wxString title, int nType ) 
						: wxDialog( parent, -1,
                          title, wxDefaultPosition,
                          wxDefaultSize, //wxSize( 350, 310 ),
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	m_nViewType = nType;

	// main dialog sizer
	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 3, 1, 5, 10 );
	topsizer->AddGrowableRow( 2 );

	// select sizer
	wxFlexGridSizer *sizerHead = new wxFlexGridSizer( 2, 4, 5, 10 );
	//wxFlexGridSizer *sizerTitle = new wxFlexGridSizer( 1, 3, 5, 10 );
	// now add these to top
	topsizer->Add( sizerHead, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW | wxALL, 5 );
	//topsizer->Add( sizerTitle, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW | wxALL, 3 );

	////////////
	// elements
	m_pCommentsList = new wxChoice( this, wxID_ONLINE_COMMENT_SELECT,
										wxDefaultPosition, wxSize(300,-1), 0, NULL,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pCommentsList->SetSelection( 0 );
	m_pTitle = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(300,-1) );
	m_pBody = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(350,80), wxTE_MULTILINE );
	m_pNewViewButton = new wxButton( this, wxID_ONLINE_COMMENT_BUTTON, wxT("View"), wxPoint(-1, -1), wxSize(45,-1) );
	// hide select by default
	if( m_nViewType )
	{
		m_pTitle->Hide();
		m_pBody->SetEditable(0);

	} else
	{
		m_pCommentsList->Hide();
		m_pBody->SetEditable(1);
	}

	///////
	// layout
	// :: title
	sizerHead->Add(new wxStaticText( this, -1, wxT("Title:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerHead->Add( m_pTitle, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerHead->Add( m_pCommentsList, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerHead->Add( m_pNewViewButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: body
	topsizer->Add( m_pBody, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxGROW|wxALL, 5 );

	// add buttons to top sizer
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL ), 1, wxALIGN_CENTER| wxCENTRE | wxALL, 5 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);

}

////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////
COnlineCommnetsDlg::~COnlineCommnetsDlg( ) 
{
	delete( m_pCommentsList );
	delete( m_pTitle );
	delete( m_pBody );
	delete( m_pNewViewButton );
}

// Method:	SetConfig
////////////////////////////////////////////////////////////////////
void COnlineCommnetsDlg::SetConfig( CUnimapOnline* pUnimapOnline, int nImageId )
{
	int i=0;

	m_pUnimapOnline = pUnimapOnline;
	m_nImageId = nImageId;

	// populate select box
	m_pCommentsList->Clear();
	if( m_pUnimapOnline->m_vectRemoteFileList[m_nImageId].vectComments.size() > 0 )
	{
		for( i=0; i<m_pUnimapOnline->m_vectRemoteFileList[m_nImageId].vectComments.size(); i++ )
		{
			m_pCommentsList->Append( m_pUnimapOnline->m_vectRemoteFileList[m_nImageId].vectComments[i].title );
		}
		m_pCommentsList->SetSelection(0);
		m_pBody->SetValue( m_pUnimapOnline->m_vectRemoteFileList[m_nImageId].vectComments[0].message );

		m_nViewType = 1;
		m_pTitle->Hide();
		m_pCommentsList->Show();
		m_pBody->SetEditable(0);
		m_pNewViewButton->SetLabel( wxT("New") );

	} else
	{

		m_nViewType = 0;
		m_pTitle->Show();
		m_pCommentsList->Hide();
		m_pBody->SetEditable(1);
		m_pNewViewButton->SetLabel( wxT("View") );
	}
}

// Method:	OnNewView
////////////////////////////////////////////////////////////////////
void COnlineCommnetsDlg::OnNewView( wxCommandEvent& pEvent )
{
	// if view=new(0) go into view view=1
	if( m_nViewType == 0 )
	{
		m_pTitle->Hide();
		m_pCommentsList->Show();
		m_strBodyBackup = m_pBody->GetValue();
		m_pBody->Clear();
		m_pBody->SetEditable( 0 );
		m_pNewViewButton->SetLabel( wxT("New") );

		// populate with message
		int nSelected = m_pCommentsList->GetSelection();
		m_pBody->SetValue( m_pUnimapOnline->m_vectRemoteFileList[m_nImageId].vectComments[nSelected].message );

		m_nViewType = 1;

	} else if( m_nViewType == 1 )
	{
		m_pCommentsList->Hide();
		m_pTitle->Show();
		m_pBody->SetValue( m_strBodyBackup );
		m_pBody->SetEditable( 1 );
		m_pNewViewButton->SetLabel( wxT("View") );
		m_pNewViewButton->Disable();

		m_nViewType = 0;
	}

	GetSizer()->Layout();
	Layout();
	Update();
	Refresh(false);
}

// Method:	OnSelectComment
////////////////////////////////////////////////////////////////////
void COnlineCommnetsDlg::OnSelectComment( wxCommandEvent& pEvent )
{
	int nSelected = m_pCommentsList->GetSelection();
	m_pBody->SetValue( m_pUnimapOnline->m_vectRemoteFileList[m_nImageId].vectComments[nSelected].message );
}
