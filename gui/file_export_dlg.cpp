
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/wxprec.h"

// local includes
#include "../image/astroimage.h"
#include "../image/imagegroup.h"

// main header
#include "file_export_dlg.h"

BEGIN_EVENT_TABLE(CFileExportDlg, wxDialog)
	EVT_BUTTON( wxID_EXPORT_FILE_SET_BUTTON, CFileExportDlg::OnFileSet )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CFileExportDlg
// Purpose:	Initialize object
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CFileExportDlg::CFileExportDlg( wxWindow *parent, wxString title, CImageGroup* pImageGroup, CAstroImage* pAstroImage ) 
						: wxDialog( parent, -1,
                          title, wxDefaultPosition,
                          wxDefaultSize, //wxSize( 350, 310 ),
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	m_pImageGroup = pImageGroup;
	m_pAstroImage = pAstroImage;
	m_strPath = m_pAstroImage->m_strImagePath;
	m_strFile = m_pAstroImage->m_strImageName + + wxT(".html");
	m_strFullFile = m_pAstroImage->GetFullNameBase() + wxT(".html");

	wxString vectExportFormat[2] = { wxT("HTML Page") };
	wxString vectExportTarget[2] = { wxT("Current Image") };

	// main dialog sizer
	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 1, 1, 5, 5 );

	// entries panel sizer
	wxFlexGridSizer *sizerEntryPanel = new wxFlexGridSizer( 1, 2, 5, 10 );

	// define gui elements
	m_pFormat = new wxChoice( this, -1, wxDefaultPosition, wxSize(120,-1), 1, vectExportFormat );
	m_pFormat->SetSelection(0);
	m_pTarget = new wxChoice( this, -1, wxDefaultPosition, wxSize(120,-1), 1, vectExportTarget );
	m_pTarget->SetSelection(0);
	m_pFileSetButton = new wxButton( this, wxID_EXPORT_FILE_SET_BUTTON, wxT("File"), wxPoint(-1, -1), wxSize(40,-1) );
	m_pFile = new wxTextCtrl( this, -1, m_strFullFile, wxDefaultPosition, wxSize(300,-1) );

	// :: export format
	sizerEntryPanel->Add(new wxStaticText( this, -1, wxT("Format:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pFormat, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: target
	sizerEntryPanel->Add(new wxStaticText( this, -1, wxT("Target:") ), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pTarget, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: path
	sizerEntryPanel->Add( m_pFileSetButton, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerEntryPanel->Add( m_pFile, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	topsizer->Add( sizerEntryPanel, 1, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL|wxGROW | wxALL, 3 );
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL ), 1, wxALIGN_CENTER| wxCENTRE | wxALL, 5 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);

}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CFileExportDlg
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CFileExportDlg::~CFileExportDlg( ) 
{
	delete( m_pFormat );
	delete( m_pTarget );
	delete( m_pFileSetButton );
	delete( m_pFile );
}

// todo: check here why it's crasing
////////////////////////////////////////////////////////////////////
void CFileExportDlg::OnFileSet( wxCommandEvent& pEvent )
{
	wxFileDialog fileDlg( this, wxT("Export current image as ..."),
							m_strPath, 
							m_strFile, 
							FILE_EXPORT_FORMATS, wxFD_SAVE|wxFD_OVERWRITE_PROMPT );

	if( fileDlg.ShowModal() == wxID_OK )
	{
		m_strPath = fileDlg.GetDirectory();
		m_strFile = fileDlg.GetFilename();

		m_strFullFile = fileDlg.GetPath();
		// set fullname
		m_pFile->SetValue( m_strFullFile );
	}

	return;
}
