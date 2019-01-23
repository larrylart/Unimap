
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/wxprec.h"
#include <wx/dirdlg.h>
#include <wx/filefn.h>

// main header
#include "group_add_dlg.h"

BEGIN_EVENT_TABLE(CGroupAddDlg, wxDialog)
	EVT_BUTTON( wxID_GROUP_SET_PATH_BUTTON, CGroupAddDlg::OnPathSet )
	EVT_CHECKBOX( wxID_GROUP_ADD_PATH_FILES, CGroupAddDlg::OnPathAddFiles )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CGroupAddDlg
// Purpose:	Initialize object
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CGroupAddDlg::CGroupAddDlg( wxWindow *parent, wxString title ) :
							wxDialog( parent, -1,
								title, wxDefaultPosition,
								wxDefaultSize, 
								wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	m_strPath = wxGetCwd();

	// main dialog sizer
	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 2, 1, 5, 5 );

	//////////////////////////////////
	// Mouse panel sizer
	wxFlexGridSizer *sizerEntryPanel = new wxFlexGridSizer( 1, 2, 5, 10 );
	
	// gui elements
	m_pName = new wxTextCtrl( this, -1, _T(""), wxDefaultPosition, wxSize(200,-1) );
	m_pPathSetButton = new wxButton( this, wxID_GROUP_SET_PATH_BUTTON, wxT("Path"), wxPoint(-1, -1), wxSize(40,-1) );
	m_pPath = new wxTextCtrl( this, -1, m_strPath, wxDefaultPosition, wxSize(200,-1) );
	m_pAddFiles = new wxCheckBox( this, wxID_GROUP_ADD_PATH_FILES, wxT("Add all files from in path") );
	m_pIncFolders = new wxCheckBox( this, -1, wxT("Include files from sub-folders") );
	m_pIncFolders->Disable();

	// :: group name
	sizerEntryPanel->Add(new wxStaticText( this, -1, wxT("Group name:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pName, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: group path
	sizerEntryPanel->Add( m_pPathSetButton, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pPath, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: add files from path
	sizerEntryPanel->AddStretchSpacer();
	sizerEntryPanel->Add( m_pAddFiles, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: include subdirectories
	sizerEntryPanel->AddStretchSpacer();
	sizerEntryPanel->Add( m_pIncFolders, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	topsizer->Add( sizerEntryPanel, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW | wxALL, 5 );
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL ), 1, wxALIGN_CENTER| wxCENTRE | wxALL, 5 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Fit();
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CGroupAddDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CGroupAddDlg::~CGroupAddDlg( ) 
{
	delete( m_pName );
	delete( m_pPathSetButton );
	delete( m_pPath );
	delete( m_pAddFiles );
	delete( m_pIncFolders );
}

// todo: check here why it's crasing
////////////////////////////////////////////////////////////////////
void CGroupAddDlg::OnPathSet( wxCommandEvent& pEvent )
{
	m_strPath = m_pPath->GetLineText(0);
	wxDirDialog pathDlg( this, wxT("Save current image in ..."), m_strPath  );

	if( pathDlg.ShowModal() == wxID_OK )
	{
		m_strPath = pathDlg.GetPath();
		m_pPath->SetValue( m_strPath );
	}
}

////////////////////////////////////////////////////////////////////
void CGroupAddDlg::OnPathAddFiles( wxCommandEvent& pEvent )
{
	if( pEvent.IsChecked() )
		m_pIncFolders->Enable();
	else
		m_pIncFolders->Disable();
}
